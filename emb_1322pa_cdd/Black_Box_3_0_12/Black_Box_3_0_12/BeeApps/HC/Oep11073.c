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
#include "Oep11073.h"
#include "OepManager.h"
#include "OepAgent.h"
#include "ZtcInterface.h"
#include "HcZtc.h"

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

/******************************************************************************
*******************************************************************************
* Private Memory Declarations
*******************************************************************************
******************************************************************************/

/*
  List of endpoints that implement 11073 devices
  Endpoint number, handler
*/
static OepEndpointData_t oepEndPoints[gOepMaxDevices_c];


/******************************************************************************
*******************************************************************************
* Public memory declarations
*******************************************************************************
******************************************************************************/

anchor_t gDataIndicationQueue;
tsTaskID_t gOep11073TaskID;

/******************************************************************************
*******************************************************************************
* Private Functions
*******************************************************************************
******************************************************************************/

void Oep11073InitDevice(zbEndPoint_t ep, genericClass_t* (*oepAppObjectList)[]) 
{
  /* Initialize the object array pointer from the OEP support library */
  pObjectArray = oepAppObjectList;
  
  /* Set the endpoint on which the healtcare app runs */
  /* ZHCTODO only one endpoint is supported at the moment */
  oepEndPoints[0].ep = ep;
  
  /* set ZigBee data fragmentation options */
  ApsmeSetRequest(gApsMaxWindowSize_c, gOepFragOptions_WindowSize);      
  ApsmeSetRequest(gApsInterframeDelay_c, gOepFragOptions_InterFrameDelay); 
  ApsmeSetRequest(gApsMaxFragmentLength_c, gOepFragOptions_MaxFragmentLength);
}

/******************************************************************************
*******************************************************************************
* Public Functions
*******************************************************************************
******************************************************************************/

/*****************************************************************************
  Oep11073InitManagerDevice
  Called by the application to initialize a ZigBee Oep 11073 Manager device
  on a specified endpoint
*****************************************************************************/

void Oep11073InitManagerDevice(zbEndPoint_t ep, genericClass_t* (*oepAppObjectList)[]) 
{
  Oep11073InitDevice(ep, oepAppObjectList);

  /* Set the data handler and run the manager task */
  /* ZHCTODO only one endpoint is supported at the moment */
  oepEndPoints[0].handler = &OepManager_HandleApdu;
  OepManagerInit(ep);
}
  

/*****************************************************************************
  Oep11073InitAgentDevice
  Called by the application to initialize a ZigBee Oep 11073 Agent device
  on a specified endpoint
*****************************************************************************/

void Oep11073InitAgentDevice(zbEndPoint_t ep, genericClass_t* (*oepAppObjectList)[]) 
{
  Oep11073InitDevice(ep, oepAppObjectList);

  /* Set the data handler and run the agent task */
  /* ZHCTODO only one endpoint is supported at the moment */
  oepEndPoints[0].handler = &OepAgent_HandleApdu;
  OepAgentInit(ep);
}


/*****************************************************************************
  OepGetStartOepFragment
  Start association procedure of the device running on the specified endpoint.
*****************************************************************************/

OepFragmentedApdu_t *OepGetStartOepFragment(zbEndPoint_t ep, 
                                            bool_t knownConfiguration,
                                            OepStartAssocOrigin_t origin
                                           )
{

   
  oepGenericApdu_t    *genericApdu;
  OepFragmentedApdu_t *pApdu;
  
  pApdu = MSG_Alloc(sizeof(OepFragmentedApdu_t) + sizeof(oepGenericApdu_t) + 1);   
  
  if (NULL != pApdu) 
  {
    pApdu->dstEndPoint = ep;
    pApdu->fragment.len = MbrOfs(oepGenericApdu_t, payload) + 2;
    pApdu->fragment.nextFragment = NULL;
    
    genericApdu = (oepGenericApdu_t*)(pApdu->fragment.data);
    genericApdu->apduLen =  pApdu->fragment.len - MbrSizeof(oepGenericApdu_t, apduLen);
    genericApdu->choice = oepAssocChoice_StartOepAssocRec;
    genericApdu->payload[0] = knownConfiguration;
    genericApdu->payload[1] = (uint8_t)origin;
  }
     
  return (pApdu);
  
}

OepFragmentedApdu_t *OepGetReleaseOepFragment(zbEndPoint_t ep)
{
  oepGenericApdu_t    *genericApdu;
  OepFragmentedApdu_t *pApdu;
  
  pApdu = MSG_Alloc(sizeof(OepFragmentedApdu_t) + sizeof(oepGenericApdu_t) + 4);   
  
  if (NULL != pApdu) 
  {
    pApdu->dstEndPoint = ep;
    pApdu->fragment.len = MbrOfs(oepGenericApdu_t, payload) + 5;
    pApdu->fragment.nextFragment = NULL;
    
    genericApdu = (oepGenericApdu_t*)(pApdu->fragment.data);
    genericApdu->apduLen =  pApdu->fragment.len - MbrSizeof(oepGenericApdu_t, apduLen);
    genericApdu->choice = oepAssocChoice_SendAssocReleaseReq;
    genericApdu->payload[0] = 0;
    genericApdu->payload[1] = 0;
    genericApdu->payload[2] = 0;
    genericApdu->payload[3] = 0;
    genericApdu->payload[4] = 0;
  }
     
  return (pApdu);
}

OepFragmentedApdu_t *OepGetInternalTransportStatusFragment(zbEndPoint_t ep, uint16_t status) {
  oepGenericApdu_t    *genericApdu;
  OepFragmentedApdu_t *pApdu;
  
  pApdu = MSG_Alloc(sizeof(OepFragmentedApdu_t) + sizeof(oepGenericApdu_t));   
  
  if (NULL != pApdu) 
  {
    pApdu->dstEndPoint = ep;
    pApdu->fragment.len = MbrOfs(oepGenericApdu_t, payload);
    pApdu->fragment.nextFragment = NULL;
    
    genericApdu = (oepGenericApdu_t*)(pApdu->fragment.data);
    genericApdu->apduLen =  pApdu->fragment.len - MbrSizeof(oepGenericApdu_t, apduLen);
    genericApdu->choice = status;
  }

  return (pApdu); 
}

OepFragmentedApdu_t *OepGetTransportConnectedFragment(zbEndPoint_t ep) 
{
  return OepGetInternalTransportStatusFragment(ep, oepAssocChoice_TransportConnectInd);
}

OepFragmentedApdu_t *OepGetTransportDisconnectedFragment(zbEndPoint_t ep) 
{
  return OepGetInternalTransportStatusFragment(ep, oepAssocChoice_TransportDisconnectInd);
}


OepFragmentedApdu_t *OepGetSendDataReportFragment(zbEndPoint_t ep, uint16_t invokeId, 
                                                  uint16_t dataReqId, uint16_t scanReportNo
                                                 )
{
  hcZtcSendDataReport_t  *sendDataReport;
  oepGenericApdu_t       *genericApdu;  
  OepFragmentedApdu_t    *pApdu;
  
  pApdu = MSG_Alloc(sizeof(OepFragmentedApdu_t) - 1 + MbrOfs(oepGenericApdu_t, payload) +
                     sizeof(hcZtcSendDataReport_t));
  
  if (NULL != pApdu) 
  {
    pApdu->dstEndPoint = ep;
    pApdu->fragment.len = MbrOfs(oepGenericApdu_t, payload) + sizeof(hcZtcSendDataReport_t);
    pApdu->fragment.nextFragment = NULL;
    
    genericApdu = (oepGenericApdu_t*)(pApdu->fragment.data);
    genericApdu->apduLen =  pApdu->fragment.len - MbrSizeof(oepGenericApdu_t, apduLen);
    genericApdu->choice = oepReportChoice_SendDataReportReq;
    
    sendDataReport = (hcZtcSendDataReport_t*)genericApdu->payload;
    sendDataReport->invokeId = invokeId;
    sendDataReport->dataReqId = dataReqId;
    sendDataReport->scanReportNo = scanReportNo;

  }
     
  return (pApdu);
}


uint8_t Oep_GetApduMsgId(zclApdu_t *pApdu) 
{

  (void) pApdu;
  return 0;  
}


void Oep_ProcessApdu(zclApdu_t *pApdu) 
{
  (void) pApdu;
  
}



/*****************************************************************************
  OepFreeFragmentedApdu
  Free Apdu and its fragments
*****************************************************************************/
void OepFreeFragmentedApdu(OepFragmentedApdu_t *pApdu) 
{

  OepApduFragment_t *fragment;
  
  fragment = pApdu->fragment.nextFragment;
  
  while (NULL != fragment) 
  {    
     OepApduFragment_t *f = fragment->nextFragment;
     MSG_Free(fragment);
     fragment = f;
  }
  
  
  MSG_Free(pApdu);

  
}



/*****************************************************************************
  ZCL_Oep_SAPHandler
  Handle APDU's sent by ZCL
*****************************************************************************/
void ZCL_Oep_SAPHandler(OepFragmentedApdu_t *FragmentedpApdu) 
{

#if gHcEnable11073UartTunnel  
  OepApduFragment_t *pFragment = &FragmentedpApdu->fragment;
#endif
        
  /* ZTC monitor */
  
#if !gHcDisable11073Processing  
  HcIncomingZtcTaskEventMonitor(FragmentedpApdu);
  /* Forward APDU to the corresponding endpoint task */
  if (oepEndPoints[0].ep == FragmentedpApdu->dstEndPoint) 
  {
    oepEndPoints[0].handler(FragmentedpApdu);        
  }
#else 
#if gHcEnable11073UartTunnel
  /* Send the message over the serial connection */
  while(pFragment)
  {
    while(!UartX_Transmit(pFragment->data + 2, pFragment->len - 2, NULL));
    pFragment = pFragment->nextFragment;
  }
#endif  
  OepFreeFragmentedApdu(FragmentedpApdu);  
#endif  
  
}

/*****************************************************************************
  ZCL_Oep_SAPHandler
  Send APDU's to ZCL
*****************************************************************************/
void Oep_ZCL_SAPHandler(OepOutgoingMessage_t *pMsg)
{

  zclCmd11073_TransferApduPtr_t cmd;

  HcOutgoingZtcTaskEventMonitor(pMsg);
  
  cmd.addrInfo.dstAddrMode = pMsg->dstAddrMode; 
  FLib_MemCpy(&cmd.addrInfo.dstAddr.aIeeeAddr, &pMsg->aDstAddr, 8);
  cmd.addrInfo.dstEndPoint = pMsg->dstEndPoint;
  Set2Bytes(cmd.addrInfo.aClusterId, gZclCluster11073Tunnel_c);
  cmd.addrInfo.srcEndPoint = pMsg->srcEndPoint;
  cmd.addrInfo.txOptions = gApsTxOptionFragmentationRequested_c;
  cmd.addrInfo.radiusCounter = afDefaultRadius_c;    
  cmd.apdu = (zclApdu_t*)pMsg->pApdu;
  
  
  if (!pMsg->ztcOnly)      
    (void) IEEE11073ProtocolTunnel_TransferApduPtr(&cmd);
     
}

#endif
