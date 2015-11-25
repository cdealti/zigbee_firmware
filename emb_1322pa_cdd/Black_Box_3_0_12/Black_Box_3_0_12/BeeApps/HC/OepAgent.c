/*****************************************************************************
* OepAgent.c
*
* IEEE 11073 Optimized Exchange Protocol implementation of Agent
*
* Copyright (c) 2010, Freescale, Inc. All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
*
*****************************************************************************/
#ifdef gHcGenericApp_d
#include "OepAgent.h"
#include "NomenclatureCodes.h"
#include "OEPClassSupport.h"
#include "HcZtc.h"

/******************************************************************************
*******************************************************************************
* Private type definitions
*******************************************************************************
******************************************************************************/
#define mTsOem11073TaskPriority_c (gTsAppTaskPriority_c+1)

tmrTimerID_t mTimer;

/******************************************************************************
*******************************************************************************
* Private Prototypes
*******************************************************************************
******************************************************************************/

/******************************************************************************
*******************************************************************************
* Private Memory Declarations
*******************************************************************************
******************************************************************************/
static  OepAgentSmState_t currentState = stateDisconnected_c;
static  uint8_t agentBuffer[200];


static anchor_t gOepAgentQueue;
static tsTaskID_t gOepAgentTaskID;
static Oep11073AddressInfo_t mAgentAddrInfo;

static OepFragmentedApdu_t* pCurrentFragApdu = NULL;

/******************************************************************************
*******************************************************************************
* Public memory declarations
*******************************************************************************
******************************************************************************/



/******************************************************************************
*******************************************************************************
* Private Functions
*******************************************************************************
******************************************************************************/


/*****************************************************************************
 Timer handler
*****************************************************************************/
static void OnTimeOut(uint8_t tmr) 
{
  (void) tmr;
  
   /* Initiate association */
   TS_SendEvent(gOepAgentTaskID, evtAssocReq_c);  
}


/*****************************************************************************

  Event handlers

*****************************************************************************/


/*****************************************************************************
  Handler for  Disconnected State
  
  On oepAssocChoice_StartOepAssocRec    
    - go to Connected->Unassociated state
    - initiate association 
*****************************************************************************/
static void HandleDisconnectedState(event_t event) 
{
  oepGenericApdu_t* pApdu = NULL;  

  if (event & evtApduReceived_c) 
  {    
    if (pCurrentFragApdu) 
    {      
      pApdu = (oepGenericApdu_t*)&pCurrentFragApdu->fragment.data;      
      if(pApdu->choice == oepAssocChoice_TransportConnectInd)
      {       
          /* Goto Unassociated state  */
          currentState = stateConnUnassociated_c;                 
      }
    }
  }
}

/*****************************************************************************
  Handler for  Connected->Disconnecting State
*****************************************************************************/
static void HandleConnDissasociatingState(event_t event) 
{   

  oepGenericApdu_t* pApdu = NULL;  

  if (event & evtApduReceived_c) 
  {    
    if (pCurrentFragApdu) 
    {      
      pApdu = (oepGenericApdu_t*)&pCurrentFragApdu->fragment.data;      

      if (pApdu->choice == gRlre_d)
      {
        currentState = stateConnUnassociated_c; 
      }
      else if (pApdu->choice == oepAssocChoice_TransportDisconnectInd)
      {
        currentState = stateDisconnected_c;
      }        
    }
  }
  
}

static void CopyAgentAddrInfoToMsg(OepOutgoingMessage_t *pOutMsg) {
    pOutMsg->dstAddrMode = mAgentAddrInfo.dstAddrMode;
    pOutMsg->srcEndPoint = mAgentAddrInfo.srcEndPoint;
    pOutMsg->dstEndPoint = mAgentAddrInfo.dstEndPoint;
    FLib_MemCpy(&pOutMsg->aDstAddr, mAgentAddrInfo.aDstAddr, 8); 
}

/*****************************************************************************
  Handler for  Connected->Unassociated State
  
  On evtAssocReq_c
    - send an associate request to the manager
    - go to Connected->Associating state
*****************************************************************************/
static void HandleConnUnassociatedState(event_t event) 
{
  extern bool_t initAssociateOnBind;
  bool_t sendAssocReq = FALSE;
  if (event & evtApduReceived_c) 
  {    
    if (pCurrentFragApdu) 
    {      
      oepGenericApdu_t* pApdu = (oepGenericApdu_t*)&pCurrentFragApdu->fragment.data;      
      if(pApdu->choice == oepAssocChoice_StartOepAssocRec)
      {       
        if (pApdu->payload[1] != oepStartAssocOrigin_EndDeviceBind || initAssociateOnBind)
        {        
          /* Initiate association */
          if (pApdu->payload[1] == oepStartAssocOrigin_EndDeviceBind)
            TMR_StartTimer(mTimer, gTmrSingleShotTimer_c , 2000, OnTimeOut);
          else 
            sendAssocReq = TRUE;
        }                           
      }
      else if (pApdu->choice == oepAssocChoice_TransportDisconnectInd)
      {
        currentState = stateDisconnected_c;
      }                  
    }
  }
  if (sendAssocReq || event & evtAssocReq_c) 
  {
    OepOutgoingMessage_t outMsg;
    /* Create an association request */   
    (void)oepCsCreateAssociateReq(&agentBuffer[0], &outMsg.length);

    CopyAgentAddrInfoToMsg(&outMsg);

    outMsg.ztcOnly = FALSE;
    outMsg.pApdu = &agentBuffer[0];
    (void)Oep_ZCL_SAPHandler(&outMsg);

    /* Move to the Associating state */
    currentState = stateConnAssociating_c;       
  }
}


/*****************************************************************************
  Handler for  Connected->Associating State
  
  On reception of Associate Response:
    - If result is Accepted
        - go to Associated->Operating state        
    - If result is Accepted_Unknown_Config
        - send Config report
        - go to Associated->Wait Approval state
          (NOTE: because a single configuration is supported,
                 we skip Associating->SendingConfig step)
    - If result is rejected
        - go to Unassociated state
*****************************************************************************/
static void HandleConnAssociatingState(event_t event) 
{
  apdu_t* pApdu = NULL;
  associateResult_t res;
  OepOutgoingMessage_t outMsg;  


  if (event & evtApduReceived_c)
  {
    if (NULL != pCurrentFragApdu)
    {
      pApdu = (apdu_t*)((uint8_t*)&pCurrentFragApdu->fragment.data + 2);
      if (pApdu->choice == gAbrt_d)
        currentState = stateConnUnassociated_c; 
      else if(pApdu->choice == gAare_d)
      {
        /* Association response was received  */                        
        res = pApdu->u.aare.result;
        
        switch (res) 
        {
        case gAccepted_d:
          currentState = stateConnAssocOperating_c;
          break;
        case gAcceptedUnknownConfig_d:
          
          /* Send configuration */        
         (void)oepCsCreateReportConfig(&agentBuffer[0], &outMsg.length, 0x1234);
         
          CopyAgentAddrInfoToMsg(&outMsg);

          outMsg.ztcOnly = FALSE;
          outMsg.pApdu = &agentBuffer[0];
          (void)Oep_ZCL_SAPHandler(&outMsg);
          
          currentState = stateConnAssocConfWa_c;
          break;
          
        default:
          /* Rejected */
          currentState = stateConnUnassociated_c;
          
        } /* switch */
      }
      else if (pApdu->choice == oepAssocChoice_TransportDisconnectInd)
      {
        currentState = stateDisconnected_c;
      }             
    }
  }  
  

}


/*****************************************************************************
  Handler for  Connected->Associated->Operating State
  
*****************************************************************************/


static void HandleConnConnAssocOperatingState(event_t event) 
{  
  oepGenericApdu_t* pGenericApdu = NULL;  
  apdu_t* pApdu = NULL;
  OepOutgoingMessage_t outMsg;   
  hcZtcSendDataReport_t* hcZtcSendDataReport;
  hcZtcSendAssocReleaseReq_t* hcZtcSendAssocRelease;
  prstOepGetFrame_t* pPrstApdu = NULL;
  bool_t sendRsp = FALSE;       

  if (event & evtApduReceived_c) 
  {
    if (pCurrentFragApdu) 
    {      
      pGenericApdu = (oepGenericApdu_t*)&pCurrentFragApdu->fragment.data;      
      if(pGenericApdu->choice == oepReportChoice_SendDataReportReq)
      {       
          
          hcZtcSendDataReport = (hcZtcSendDataReport_t *)pGenericApdu->payload;
#if (gBigEndian_c)       
          Swap2BytesArray((uint8_t*)&hcZtcSendDataReport->invokeId);
          Swap2BytesArray((uint8_t*)&hcZtcSendDataReport->dataReqId);
          Swap2BytesArray((uint8_t*)&hcZtcSendDataReport->scanReportNo);          
#endif    

          (void)oepCsCreateDataReport(&agentBuffer[0], &outMsg.length, hcZtcSendDataReport->invokeId,
                                        hcZtcSendDataReport->dataReqId, hcZtcSendDataReport->scanReportNo
                                     );
          
          sendRsp = TRUE;

      }     
      
      else if (pGenericApdu->choice == oepAssocChoice_SendAssocReleaseReq)
      {
        hcZtcSendAssocRelease = (hcZtcSendAssocReleaseReq_t *)pGenericApdu->payload;
#if (gBigEndian_c)       
        Swap2BytesArray((uint8_t*)&hcZtcSendAssocRelease->reason);
#endif       
        (void)oepCsCreateRlrq(&agentBuffer[0], &outMsg.length, hcZtcSendAssocRelease->reason);
        
        currentState = stateConnDissasociating_c; 
        
        sendRsp = TRUE;
      }
      else if (pGenericApdu->choice == oepAssocChoice_TransportDisconnectInd)
      {
        currentState = stateDisconnected_c; 
        
        sendRsp = FALSE;
      }      
      
      else if (pGenericApdu->choice == gRlrq_d)
      {
        (void)oepCsCreateRlre(&agentBuffer[0], &outMsg.length, gReleaseRequestReasonNormal_d);

        currentState = stateConnUnassociated_c; 
        
        sendRsp = TRUE;           
      } 
      else if (pGenericApdu->choice == gAbrt_d)
      {
        currentState = stateConnUnassociated_c; 
      }       
      else if (pGenericApdu->choice == gPrst_d) 
      {
          pApdu  = (apdu_t*)&pGenericApdu->choice;
          pPrstApdu = &(pApdu->u.getFrame);      
          
          if( (pPrstApdu->choice == gRoivCmipGet_d) &&
              (pPrstApdu->objHandle == 0)
             )
          {
            uint16_t invokeId = HcOTA2Native16(pPrstApdu->invokeId);
            (void)oepCsCreateGetMdsResponseFrame(&agentBuffer[0], &outMsg.length, invokeId);
 
            sendRsp = TRUE;
          }      
      }
      
      if (sendRsp) {
        CopyAgentAddrInfoToMsg(&outMsg);
        outMsg.ztcOnly = FALSE;
        outMsg.pApdu = &agentBuffer[0];
        (void)Oep_ZCL_SAPHandler(&outMsg);        
      }
    }
  }
}



/*****************************************************************************
  Handler for  Connected->Associated->Configuration->Wa State
    
  When ConfigEventReportResponse is received:
    - If result is AcceptedConfig
        - go to Operating state
    - If result is UnsupportedConfig or UnknownConfig
        - go back to SendingConfig
  
*****************************************************************************/
static void HandleConnAssocConfWaState(event_t event) 
{
  apdu_t* pApdu = NULL;
  prstOepCfgRspApdu_t* pPrstApdu = NULL;
  configResult_t res;
  OepOutgoingMessage_t outMsg;     
  
  if (event & evtApduReceived_c) 
  {       
    if (NULL != pCurrentFragApdu)
    {
      pApdu = (apdu_t*)((uint8_t*)&pCurrentFragApdu->fragment.data + 2);      
      if (pApdu->choice == gAbrt_d)
        currentState = stateConnUnassociated_c;       
      else if(pApdu->choice == gPrst_d)
      { 
          pPrstApdu = (prstOepCfgRspApdu_t*)&(pApdu->u.cfgReport);      
          
          if( (pPrstApdu->choice == gRorsCmipConfirmedEventReport_d) &&
              (pPrstApdu->eventType == MDC_NOTI_CONFIG)
             )
          {
            res = pPrstApdu->configResult;
            
            if  (gConfigAccepted_d == res)
            {
              currentState = stateConnAssocOperating_c;
            }
            else 
            {          
              currentState = stateConnUnassociated_c;
            }
          } 
   
          else if( (pPrstApdu->choice == gRoivCmipGet_d) &&
                   (pPrstApdu->objHandle == 0)
                 )
          {
            uint16_t invokeId = HcOTA2Native16(pPrstApdu->invokeId);
            (void)oepCsCreateGetMdsResponseFrame(&agentBuffer[0], &outMsg.length, invokeId);
 
          	CopyAgentAddrInfoToMsg(&outMsg);
          	outMsg.ztcOnly = FALSE;
          	outMsg.pApdu = &agentBuffer[0];
          	(void)Oep_ZCL_SAPHandler(&outMsg); 
          }    
      }  
          
      else if (pApdu->choice == oepAssocChoice_TransportDisconnectInd)
      {
        currentState = stateDisconnected_c;
      }         
    }
  }  
}



/*****************************************************************************
  Handler for  Connected->Associated->Configuration->Sc State
  
  NOTE: we currently skip this state and go directly to WaitApproval
*****************************************************************************/
/*
static void HandleConnAssocConfScState(event_t event) 
{
  (void) event;

}
*/

/*****************************************************************************
  OepAgentTask
  11073 Agent State Machine
*****************************************************************************/

void OepAgentTask
(  
  event_t event
)
{ 
  oepGenericApdu_t* pApdu = NULL;  


  if (event & evtApduReceived_c) 
  {    
    pCurrentFragApdu = MSG_DeQueue(&gOepAgentQueue);

    /* Check if this is a ZTC configuration message */
    if (pCurrentFragApdu != NULL) 
    {
      pApdu = (oepGenericApdu_t*) &(pCurrentFragApdu->fragment.data);
      
      if(GetOepMsgType(pApdu) == msgType_ObjCfgMsg)
      {        
        oepCsManagerHandleConfigMsg(pApdu, &agentBuffer[0], 200, mAgentAddrInfo.srcEndPoint);
        
        /* Don't handle the pCurrentFragApdu fragment further */
        //event &= ~evtApduReceived_c;
      }
    }
  }
  

   switch (currentState) 
   {
   case stateDisconnected_c:
    HandleDisconnectedState(event);
    break;
   
   case stateConnDissasociating_c:
    HandleConnDissasociatingState(event);
   break;
   
   
   case stateConnUnassociated_c:
   HandleConnUnassociatedState(event);
   break;
   
   case stateConnAssociating_c:
    HandleConnAssociatingState(event);
   break;
   
   
   case stateConnAssocOperating_c:
    HandleConnConnAssocOperatingState(event);
   break;      
   
   case stateConnAssocConfSc_c:
    /* 
       skip this state, 
       go directly to stateConnAssocConfWa_c
    */
    break;
   
   case stateConnAssocConfWa_c:
    HandleConnAssocConfWaState(event);
   break;
   
   default:
   break;       
   }
   
   
   
   if (pCurrentFragApdu)
   {
     OepFreeFragmentedApdu(pCurrentFragApdu);
     pCurrentFragApdu = NULL;
   }
   
         
   if (MSG_Pending(&gOepAgentQueue))
    TS_SendEvent(gOepAgentTaskID, evtApduReceived_c);
}

/******************************************************************************
*******************************************************************************
* Public Functions
*******************************************************************************
******************************************************************************/


/*****************************************************************************
  OepAgentSetAddressParams
  
*****************************************************************************/

void OepAgentSetAddressInfo(Oep11073AddressInfo_t *addrInfo)
{
  FLib_MemCpy(&mAgentAddrInfo, addrInfo, sizeof(Oep11073AddressInfo_t));
}


/*****************************************************************************
  OepAgentInit
  Initialize the IEEE 11073 Oep layer
*****************************************************************************/
void OepAgentInit(zbEndPoint_t ep) 
{
  mAgentAddrInfo.srcEndPoint = ep;
  /* Create the 11073 task */
  gOepAgentTaskID = TS_CreateTask(mTsOem11073TaskPriority_c, OepAgentTask);

  /* Initialize queues */
  List_ClearAnchor(&gOepAgentQueue);
  
  mTimer = TMR_AllocateTimer();
   
     
}



/*****************************************************************************
  OepAgent_HandleApdu
  Handle APDU's sent by ZCL
*****************************************************************************/
void OepAgent_HandleApdu(OepFragmentedApdu_t *pApdu) 
{
    
  /* Add to Oep11073 queue */
  MSG_Queue(&gOepAgentQueue, pApdu);  
  
  /* Send event to 11073 task */
  TS_SendEvent(gOepAgentTaskID, evtApduReceived_c);
  
}

#endif


