/*****************************************************************************
* Oep11073.c
*
* IEEE 11073 Optimized Exchange Protocol implementation
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
#include "OepManager.h"
#include "NomenclatureCodes.h"
#include "OEPClassSupport.h"
#include "HcZtc.h"

#include "Oep11073Config.h"
#include "Oep11073.h"

/******************************************************************************
*******************************************************************************
* Private type definitions
*******************************************************************************
******************************************************************************/
#define mTsOem11073TaskPriority_c (gTsAppTaskPriority_c+1)
#define mTaskEvent_c 0x01

/******************************************************************************
*******************************************************************************
* Private Prototypes
*******************************************************************************
******************************************************************************/
  void HandleConnAssociatingState(event_t event);
/******************************************************************************
*******************************************************************************
* Private Memory Declarations
*******************************************************************************
******************************************************************************/
static  OepAgentSmState_t currentState = stateDisconnected_c;
static  uint8_t managerBuffer[200];


static anchor_t gOepManagerQueue;
static tsTaskID_t gOepManagerTaskID;
static Oep11073AddressInfo_t mManagerAddrInfo;

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

  Event handlers

*****************************************************************************/


/*****************************************************************************
  Handler for  Disconnected State
*****************************************************************************/
void HandleDisconnectedState(event_t event) 
{
  extern bool_t initAssociateOnBind;
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
      } else if  (pApdu->choice == oepAssocChoice_StartOepAssocRec)
      {       
#if (gHcDcu_c == gHcDeviceType_d) 
        unknownConfigIdAlways = pApdu->payload[0];
#endif        
        if (pApdu->payload[1] != 1 || initAssociateOnBind)
        {        
          /* Goto Unassociated state  */
          currentState = stateConnUnassociated_c;
        }                           
      }
    }
  }
}

/*****************************************************************************
  Handler for  Connected->Disconnecting State
*****************************************************************************/
void HandleConnDissasociatingState(event_t event) 
{

  apdu_t* pApdu = NULL;
  
  
  if (event & evtApduReceived_c) 
  {
    if (NULL != pCurrentFragApdu)
    {    
      pApdu = (apdu_t*)((uint8_t*)&pCurrentFragApdu->fragment.data + 2);      
      if(pApdu->choice == gRlre_d || pApdu->choice == gAbrt_d) 
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



/*****************************************************************************
  Handler for  Connected->Unassociated State
*****************************************************************************/
associateResult_t associateResult;
void HandleConnUnassociatedState(event_t event) 
{

  apdu_t* pApdu = NULL;
  

  if (event & evtApduReceived_c) 
  {       
    if (NULL != pCurrentFragApdu)
    {
      pApdu = (apdu_t*)((uint8_t*)&pCurrentFragApdu->fragment.data + 2);      
      if(pApdu->choice == gAarq_d)
      {
        /* Association request was received - verify it and decide on an associate response */
        
        (void)oepCsVerifyAssociateReq((aarqOepApdu_t*)&pApdu->u.aarq, &associateResult);
        
        /* Fill the addressing information */
        mManagerAddrInfo.dstAddrMode = gZbAddrMode16Bit_c;
        mManagerAddrInfo.aDstAddr[0] = pCurrentFragApdu->aSrcAddr[0];
        mManagerAddrInfo.aDstAddr[1] = pCurrentFragApdu->aSrcAddr[1];
        mManagerAddrInfo.dstEndPoint = pCurrentFragApdu->srcEndPoint;
        
        currentState = stateConnAssociating_c;
        //TS_SendEvent(gOepManagerTaskID, evtAssocReq_c);
        
        HandleConnAssociatingState(evtAssocReq_c);
      }
      else if (pApdu->choice == oepAssocChoice_TransportDisconnectInd) 
      {
        currentState = stateDisconnected_c;     
      }
    }
  }  
}


/*****************************************************************************
  Handler for  Connected->Associating State
  
  If response is Accepted
    - go to Operating state
  If response is Accepted/UnknownConfig
    - go to Configuring->WaitForConfig
  If response is Rejected
    - go to Unassociated state
*****************************************************************************/
void HandleConnAssociatingState(event_t event) 
{
  OepOutgoingMessage_t outMsg;
   
  if (event & evtAssocReq_c) 
  {   
    /* Create and send the response */
    (void)oepCsCreateAssociateRes(&managerBuffer[0], associateResult, &outMsg.length);
    
    outMsg.dstAddrMode = mManagerAddrInfo.dstAddrMode;
    outMsg.dstEndPoint = mManagerAddrInfo.dstEndPoint;
    outMsg.aDstAddr[0] = mManagerAddrInfo.aDstAddr[0];
    outMsg.aDstAddr[1] = mManagerAddrInfo.aDstAddr[1];
    outMsg.srcEndPoint = mManagerAddrInfo.srcEndPoint;
    outMsg.ztcOnly = FALSE;
    outMsg.pApdu = &managerBuffer[0];
    
    (void)Oep_ZCL_SAPHandler(&outMsg);

        
    if (gAccepted_d == associateResult) 
    {
      currentState = stateConnAssocOperating_c;            
    } 
    else if (gAcceptedUnknownConfig_d == associateResult) 
    {
      currentState = stateConnAssocConfWc_c;      
    } 
    else /* Rejected */
    {
      currentState = stateConnUnassociated_c; 
    }
  }
  
  if (event & evtApduReceived_c) 
  {
    if (NULL != pCurrentFragApdu)
    {    
      apdu_t* pApdu = (apdu_t*)((uint8_t*)&pCurrentFragApdu->fragment.data + 2);      
      if (pApdu->choice == oepAssocChoice_TransportDisconnectInd) 
      {
        currentState = stateDisconnected_c;     
      }
    }
  }  
}


/*****************************************************************************
  Handler for  Connected->Associated->Operating State
*****************************************************************************/
void HandleConnConnAssocOperatingState(event_t event) 
{

  apdu_t* pApdu = NULL;
  prstOepCfgRepApdu_t* pPrstApdu = NULL;
  OepOutgoingMessage_t outMsg;  
  hcZtcSendAssocReleaseReq_t* hcZtcSendAssocRelease;
  hcZtcSendGetMdsRequest_t* hcZtcSendGetMdsRequest;
  uint16_t invokeId;
  
  
  if (event & evtApduReceived_c) 
  {
    if (NULL != pCurrentFragApdu)
    {    
      pApdu = (apdu_t*)((uint8_t*)&pCurrentFragApdu->fragment.data + 2);      
      if(pApdu->choice == gPrst_d) 
      {        
        pPrstApdu = &(pApdu->u.cfgReport);                
        if( (pPrstApdu->choice == gRoivCmipConfirmedEventReport_d) &&
            (pPrstApdu->eventType == MDC_NOTI_SCAN_REPORT_FIXED) 
          )
        {      
          invokeId = pApdu->u.dataResponse.invokeId;
          (void)oepCsCreateDataResponse(&managerBuffer[0], &outMsg.length, invokeId);
          // ZHCTODO - setting addresses needs to be factored
          outMsg.dstAddrMode = mManagerAddrInfo.dstAddrMode;
          outMsg.dstEndPoint = mManagerAddrInfo.dstEndPoint;
          outMsg.aDstAddr[0] = mManagerAddrInfo.aDstAddr[0];
          outMsg.aDstAddr[1] = mManagerAddrInfo.aDstAddr[1];
          outMsg.srcEndPoint = mManagerAddrInfo.srcEndPoint;
          outMsg.ztcOnly = FALSE;
          outMsg.pApdu = &managerBuffer[0];

          (void)Oep_ZCL_SAPHandler(&outMsg);      
        }
      } 
      else if (pApdu->choice == oepAssocChoice_TransportDisconnectInd)
      {
        currentState = stateDisconnected_c;
      }
      else if (pApdu->choice == oepAssocChoice_SendAssocReleaseReq)
      {
        hcZtcSendAssocRelease = (hcZtcSendAssocReleaseReq_t *)&pApdu->length;
#if (gBigEndian_c)       
        Swap2BytesArray((uint8_t*)&hcZtcSendAssocRelease->reason);
#endif       
        (void)oepCsCreateRlrq(&managerBuffer[0], &outMsg.length, hcZtcSendAssocRelease->reason);
        // ZHCTODO - setting addresses needs to be factored
        outMsg.dstAddrMode = mManagerAddrInfo.dstAddrMode;
        outMsg.dstEndPoint = hcZtcSendAssocRelease->dstEndPoint;
        outMsg.aDstAddr[0] = hcZtcSendAssocRelease->dstAddr[0];
        outMsg.aDstAddr[1] = hcZtcSendAssocRelease->dstAddr[1];
        outMsg.srcEndPoint = mManagerAddrInfo.srcEndPoint;
        outMsg.ztcOnly = FALSE;
        outMsg.pApdu = &managerBuffer[0];
        
        currentState = stateConnDissasociating_c;   

        (void)Oep_ZCL_SAPHandler(&outMsg);      
      }
      else if (pApdu->choice == oepAssocChoice_SendGetMdsReq)
      {
        hcZtcSendGetMdsRequest = (hcZtcSendGetMdsRequest_t *)&pApdu->length;
        
        #if (gBigEndian_c)       
        Swap2BytesArray((uint8_t*)&hcZtcSendGetMdsRequest->invokeId);
        #endif

        (void)oepCsCreateGetAllAtributesFrame(&managerBuffer[0], &outMsg.length, hcZtcSendGetMdsRequest->invokeId, 0);
        // ZHCTODO - setting addresses needs to be factored
        outMsg.dstAddrMode = mManagerAddrInfo.dstAddrMode;
        outMsg.dstEndPoint = hcZtcSendGetMdsRequest->dstEndPoint;
        outMsg.aDstAddr[0] = hcZtcSendGetMdsRequest->dstAddr[0];
        outMsg.aDstAddr[1] = hcZtcSendGetMdsRequest->dstAddr[1];
        outMsg.srcEndPoint = mManagerAddrInfo.srcEndPoint;
        outMsg.ztcOnly = FALSE;
        outMsg.pApdu = &managerBuffer[0];

        (void)Oep_ZCL_SAPHandler(&outMsg);      
      }
      
      else if (pApdu->choice == gRlrq_d)
      {
        (void)oepCsCreateRlre(&managerBuffer[0], &outMsg.length, pApdu->u.rlrq.reason);
        // ZHCTODO - setting addresses needs to be factored
        outMsg.dstAddrMode = mManagerAddrInfo.dstAddrMode;
        outMsg.dstEndPoint = mManagerAddrInfo.dstEndPoint;
        outMsg.aDstAddr[0] = mManagerAddrInfo.aDstAddr[0];
        outMsg.aDstAddr[1] = mManagerAddrInfo.aDstAddr[1];
        outMsg.srcEndPoint = mManagerAddrInfo.srcEndPoint;
        outMsg.ztcOnly = FALSE;
        outMsg.pApdu = &managerBuffer[0];

        currentState = stateConnUnassociated_c;           
        
       (void)Oep_ZCL_SAPHandler(&outMsg);  
      } 
      else if (pApdu->choice == gAbrt_d) 
        currentState = stateConnUnassociated_c;           
    }
  }
}



/*****************************************************************************
  Handler for  Connected->Associated->Configuration->Wc State
  
  When ConfigEventReportRequest is received from Agent
    - check configuration
    - send ConfigEventReportResponse
    - if result is unsupported
        - go to WaitingForConfig state
    - if result is accepted config
        - go to Operating state
    
*****************************************************************************/
void HandleConnAssocConfWcState(event_t event) 
{
  apdu_t* pApdu = NULL;
  prstOepCfgRepApdu_t* pPrstApdu = NULL;
  OepOutgoingMessage_t outMsg;
  

  if (event & evtApduReceived_c) 
  {       
    if (NULL != pCurrentFragApdu)
    {
      pApdu = (apdu_t*)((uint8_t*)&pCurrentFragApdu->fragment.data + 2);      
      if(pApdu->choice == gAbrt_d)      
        currentState = stateConnUnassociated_c; 
      else if(pApdu->choice == gPrst_d)
      {
          pPrstApdu = &(pApdu->u.cfgReport);      
          
          if( (pPrstApdu->choice == gRoivCmipConfirmedEventReport_d) &&
              (pPrstApdu->eventType == MDC_NOTI_CONFIG)
             )
          {
            
            /* A ConfigEventReportRequest has been received */
            
            /* Check configuration */
            /* ... */
            
            
            /* Send ConfigEventReportResponse */
            (void)oepCsGenerateConfigRes(&managerBuffer[0],  &outMsg.length, pPrstApdu->invokeId,
                                          pPrstApdu->cfgReport.configReportId, gConfigAccepted_d);
            
            outMsg.dstAddrMode = mManagerAddrInfo.dstAddrMode;
            outMsg.dstEndPoint = mManagerAddrInfo.dstEndPoint;
            outMsg.aDstAddr[0] = mManagerAddrInfo.aDstAddr[0];
            outMsg.aDstAddr[1] = mManagerAddrInfo.aDstAddr[1];
            outMsg.srcEndPoint = mManagerAddrInfo.srcEndPoint;
            outMsg.ztcOnly = FALSE;
            outMsg.pApdu = &managerBuffer[0];
            
            (void)Oep_ZCL_SAPHandler(&outMsg);          
            
            /* Go to the appropiate state */            
            currentState = stateConnAssocOperating_c;
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
  Handler for  Connected->Associated->Configuration->Cc State
*****************************************************************************/

void HandleConnAssocConfCcState(event_t event) 
{

  

  switch (event) 
  {
  case evtApduReceived_c:
    break;
  
  default:
    break;
  
  }
  
}




/*****************************************************************************
  OepManagerTask
  11073 Manager State Machine
*****************************************************************************/

void OepManagerTask
(  
  event_t event
)
{
  oepGenericApdu_t* pApdu = NULL;  


  if (event & evtApduReceived_c) 
  {    
    pCurrentFragApdu = MSG_DeQueue(&gOepManagerQueue);

    /* Check if this is a ZTC configuration message */
    if (pCurrentFragApdu != NULL) 
    {
      pApdu = (oepGenericApdu_t*) &(pCurrentFragApdu->fragment.data);
      
      if(GetOepMsgType(pApdu) == msgType_ObjCfgMsg)
      {        
        oepCsManagerHandleConfigMsg(pApdu, &managerBuffer[0], sizeof(managerBuffer), mManagerAddrInfo.srcEndPoint);
        
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
   
   case stateConnAssocConfWc_c:   
    HandleConnAssocConfWcState(event);
    break;
   
   case stateConnAssocConfCc_c:
   break;
   
   default:
   break;       
   }


   if (pCurrentFragApdu)
   {
     OepFreeFragmentedApdu(pCurrentFragApdu);
     pCurrentFragApdu = NULL;
   }
   
         
   if (MSG_Pending(&gOepManagerQueue))
    TS_SendEvent(gOepManagerTaskID, evtApduReceived_c);


}


/******************************************************************************
*******************************************************************************
* Public Functions
*******************************************************************************
******************************************************************************/



/*****************************************************************************
  OepManager
  Initialize the IEEE 11073 Oep layer
*****************************************************************************/
void OepManagerInit(zbEndPoint_t endPoint) 
{
  mManagerAddrInfo.dstAddrMode = gZbAddrMode16Bit_c;
  mManagerAddrInfo.srcEndPoint = endPoint;

  /* Create the 11073 task */

  gOepManagerTaskID = TS_CreateTask(mTsOem11073TaskPriority_c, OepManagerTask);

  /* Initialize queues */
  List_ClearAnchor(&gOepManagerQueue);
     
}



/*****************************************************************************
  OepManger_HandleApdu
  Handle APDU's sent by ZCL
*****************************************************************************/
void OepManager_HandleApdu(OepFragmentedApdu_t *pApdu) 
{

  /* Parse the APDU */
  
   
  /* Add to Oep11073 queue */
  MSG_Queue(&gOepManagerQueue, pApdu);  
  
  /* Send event to 11073 task */
  TS_SendEvent(gOepManagerTaskID, evtApduReceived_c);
  
}

#endif


