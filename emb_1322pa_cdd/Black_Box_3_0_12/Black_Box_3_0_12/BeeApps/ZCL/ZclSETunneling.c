/******************************************************************************
* ZclSETunneling.c
*
* This module contains code that handles ZigBee Smart Energy Tunneling
functionality and commands.
*
* Copyright (c) 2011, Freescale, Inc.  All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
*******************************************************************************/

#include "EmbeddedTypes.h"
#include "zigbee.h"
#include "ApsMgmtInterface.h"

#include "SEProfile.h"
#include "ZclSETunneling.h"
#include "zcl.h"
#include "EndPointConfig.h"

static zbStatus_t ZCL_SETunnel_Server_Process_RequestTunnelReq(zbApsdeDataIndication_t *pIndication);
static zbStatus_t ZCL_SETunnel_Server_Process_CloseTunnelReq(zbApsdeDataIndication_t *pIndication);
static zbStatus_t ZCL_SETunnel_Server_Process_GetSupportedProtocolsReq(zbApsdeDataIndication_t *pIndication);
static zbStatus_t ZCL_SETunnel_Server_RequestTunnelRsp(zclSETunneling_RequestTunnelRsp_t *pReq);
static zbStatus_t ZCL_SETunnel_Server_TransferDataErrorReq(zclSETunneling_TransferDataErrorReq_t *pReq);
static zbStatus_t ZCL_SETunnel_Server_SupportedTunnelProtocolsRsp(zclSETunneling_SupportedTunnelingProtocolsRsp_t *pReq);
#if gASL_ZclSETunnelingOptionals_d
static zbStatus_t ZCL_SETunnel_Server_AckTransferDataRsp(zclSETunneling_AckTransferDataRsp_t *pReq);
#endif

static zbStatus_t ZCL_SETunnel_Client_TransferDataErrorReq(zclSETunneling_TransferDataErrorReq_t *pReq);
static zbStatus_t ZCL_SETunnel_Client_GetSupportedProtocolsReq(zclSETunneling_GetSupportedTunnelProtocolsReq_t *pReq);
static zbStatus_t ZCL_SETunnel_Client_Process_RequestTunnelRsp(zbApsdeDataIndication_t *pIndication);
#if gASL_ZclSETunnelingOptionals_d
static zbStatus_t ZCL_SETunnel_Client_AckTransferDataRsp(zclSETunneling_AckTransferDataRsp_t *pReq);
#endif

static zbStatus_t ZCL_SETunnel_Process_TransferDataReq(zbApsdeDataIndication_t *pIndication, bool_t client);
#if gASL_ZclSETunnelingOptionals_d
static zbStatus_t ZCL_SETunnel_ReadyDataReq(zclSETunneling_ReadyDataReq_t *pReq, bool_t client);
static zbStatus_t ZCL_SETunnel_Process_AckTransferDataRsp(zbApsdeDataIndication_t *pIndication, bool_t client);
static zbStatus_t ZCL_SETunnel_Process_ReadyDataReq(zbApsdeDataIndication_t *pIndication, bool_t client);
#endif

#if gASL_ZclSETunneling_d
static zbStatus_t ZCL_SETunnel_TransferDataReq(zclSETunneling_TransferDataReq_t *pReq);
#endif
static void ResetTunnelTimeout(uint8_t iTTE);
static uint8_t getFreeTunnelTableEntry();
static bool_t IsProtocolSupported(uint8_t protocolID);
static uint8_t getTunnelTableEntry(uint16_t tunnelID);
static uint8_t getTunnelTableEntryPending();
static void CloseTunnel(uint16_t tunnelID);
static void CloseTunnelTimerCallback(tmrTimerID_t tmrID);

static uint16_t tunnelIDCrt = 0;
static uint8_t rxData[gZclSETunnelMaxDataHandlingCapacity_c][gZclSETunnelRxBufferSize_c];
bool_t gAcceptTunnel;
tunnelingTable_t gaZclSETunnelingTable[gZclSETunnelMaxDataHandlingCapacity_c];
static zclSETunneling_Protocol_t protocolIDs[gZclSETunnelNumberOfSupportedProtocolIDs_c] = {{gZclSETunnel_NoManufacturerCode_c, TestProtocol}};

#if gASL_ZclZtcSETunnelingTesting_d
uint8_t gZtcTestData[gASL_ZclZtcSETunnelingTestingBufferSize_c + sizeof(zclSETunneling_TransferDataReq_t) - sizeof(zclLongOctetStr_t)];
#endif

extern afToApsdeMessage_t *ZCL_CreateFragmentsFrame(zclCmd_t command, zclFrameControl_t frameControl, uint8_t *pTransactionId, uint16_t *pPayloadLen, void *pPayload);
extern afToApsdeMessage_t *ZCL_CreateNoFragmentsFrame(zclCmd_t command, zclFrameControl_t frameControl, uint8_t *pTransactionId, uint8_t *pPayloadLen, void *pPayload);

#if gASL_ZclSETunneling_d
const zclAttrDef_t gaZclSETunnelingServerAttrDef[] = {
  { gZclAttrSETunnelingCloseTunnelTimeout_c, gZclDataTypeUint16_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint16_t), (void *)MbrOfs(seTunnelingAttrRAM_t, closeTunnelTimeout)}
};

const zclAttrDefList_t gZclSETunnelingServerAttrDefList = {
  NumberOfElements(gaZclSETunnelingServerAttrDef),
  gaZclSETunnelingServerAttrDef
};
#endif

zbStatus_t ZCL_SETunnelingServer
(
zbApsdeDataIndication_t *pIndication, /* IN: */
afDeviceDef_t *pDev                /* IN: */
)
{
  zclCmd_t Cmd;
  zbStatus_t status = gZclSuccessDefaultRsp_c;
  
  (void) pDev;
  
  Cmd = ((zclFrame_t *)pIndication->pAsdu)->command;
  
  switch(Cmd)
  {
    case gZclCmdSETunneling_Client_RequestTunnelReq_c: 
    {
      return ZCL_SETunnel_Server_Process_RequestTunnelReq(pIndication);
    }
    case gZclCmdSETunneling_Client_CloseTunnelReq_c:
    {
      return ZCL_SETunnel_Server_Process_CloseTunnelReq(pIndication);
    }
    case gZclCmdSETunneling_Client_TransferData_c:
    {
      return ZCL_SETunnel_Process_TransferDataReq(pIndication, FALSE);
    }
    case gZclCmdSETunneling_Client_TransferDataError_c:
    {
      zclCmdSETunneling_TransferDataErrorReq_t *pReq = (zclCmdSETunneling_TransferDataErrorReq_t *) (pIndication->pAsdu + sizeof(zclFrame_t));
      BeeAppUpdateDevice(pIndication->dstEndPoint, gZclUI_SETunnel_TransferDataErrorReceived_c, 0, 0, pReq);
      return gZclSuccess_c;
    }
#if gASL_ZclSETunnelingOptionals_d
    case gZclCmdSETunneling_Client_AckTransferDataRsp_c:
    {
      return ZCL_SETunnel_Process_AckTransferDataRsp(pIndication, FALSE);
    }
    case gZclCmdSETunneling_Client_ReadyDataReq_c:
    {
      return ZCL_SETunnel_Process_ReadyDataReq(pIndication, FALSE);
    }
#endif
    case gZclCmdSETunneling_Client_GetSupportedTunnelProtocols_c:
    {
      return ZCL_SETunnel_Server_Process_GetSupportedProtocolsReq(pIndication);
    }
    default:
    status = gZclUnsupportedClusterCommand_c;
    break;  
  }
  return status;
}

zbStatus_t ZCL_SETunnelingClient
(
	zbApsdeDataIndication_t *pIndication, /* IN: */
	afDeviceDef_t *pDev                /* IN: */
) 
{
  zclCmd_t Cmd;
  zbStatus_t status = gZclSuccessDefaultRsp_c;

 (void) pDev;

 Cmd = ((zclFrame_t *)pIndication->pAsdu)->command;
 switch(Cmd)
 {
   case gZclCmdSETunneling_Server_RequestTunnelRsp_c: 
   {
     return ZCL_SETunnel_Client_Process_RequestTunnelRsp(pIndication);
   }
   case gZclCmdSETunneling_Server_TransferData_c:
   {
     return ZCL_SETunnel_Process_TransferDataReq(pIndication, TRUE);
   }
   case gZclCmdSETunneling_Server_TransferDataError_c:
   {
     zclCmdSETunneling_TransferDataErrorReq_t *pReq = (zclCmdSETunneling_TransferDataErrorReq_t *) (pIndication->pAsdu + sizeof(zclFrame_t));
     BeeAppUpdateDevice(pIndication->dstEndPoint, gZclUI_SETunnel_TransferDataErrorReceived_c, 0, 0, pReq);
     return gZclSuccess_c;
   }
#if gASL_ZclSETunnelingOptionals_d
   case gZclCmdSETunneling_Server_AckTransferDataRsp_c:
   {
     return ZCL_SETunnel_Process_AckTransferDataRsp(pIndication, TRUE);//zclCmdSETunneling_AckTransferDataRsp_t *pReq = (zclCmdSETunneling_AckTransferDataRsp_t *) (pIndication->pAsdu + sizeof(zclFrame_t));  
   }
   case gZclCmdSETunneling_Server_ReadyDataReq_c:
   {
     return ZCL_SETunnel_Process_ReadyDataReq(pIndication, TRUE);
   }
#endif
   case gZclCmdSETunneling_Server_SupportedTunnelProtocolsRsp_c:
   {
     zclCmdSETunneling_SupportedTunnelingProtocolsRsp_t *pRsp = (zclCmdSETunneling_SupportedTunnelingProtocolsRsp_t *) (pIndication->pAsdu + sizeof(zclFrame_t));
     BeeAppUpdateDevice(pIndication->dstEndPoint, gZclUI_SETunnel_TransferDataErrorReceived_c, 0, 0, pRsp);
     return gZclSuccess_c;
   }
   default:
   status = gZclUnsupportedClusterCommand_c;
   break;  
 }
 return status;
}

void ZCL_SETunnel_InitData(void)
{
  uint8_t i;
  
  for(i=0; i<gZclSETunnelMaxDataHandlingCapacity_c; i++)
  {
    BeeUtilZeroMemory(&gaZclSETunnelingTable[i], sizeof(tunnelingTable_t));
    gaZclSETunnelingTable[i].tunnelID = gZclSETunnel_TunnelFail_c;
    gaZclSETunnelingTable[i].rxData = &rxData[i][0];
  }
}

bool_t IsProtocolSupported(uint8_t protocolID)
{
  uint8_t i;
  
  for(i=0; i<gZclSETunnelNumberOfSupportedProtocolIDs_c; i++)
  {
    if(protocolIDs[i].protocolID == protocolID)
    {
      return TRUE;
    }
  }
  
  return FALSE;
}

uint8_t getFreeTunnelTableEntry()
{
  uint8_t i;
  
  for(i=0; i<gZclSETunnelMaxDataHandlingCapacity_c; i++)
  {
    if(gaZclSETunnelingTable[i].tunnelStatus == gZclSETunnel_TTE_Free_c)
    {
      return i;
    }
  }
  
  return gZclSETunnel_TableFull_c;
}

uint8_t getTunnelTableEntry(uint16_t tunnelID)
{
  uint8_t i;
  
  for(i=0; i<gZclSETunnelMaxDataHandlingCapacity_c; i++)
  {
    if(gaZclSETunnelingTable[i].tunnelID == tunnelID)
    {
      return i;
    }
  }
  
  return gZclSETunnel_Fail_c;
}

uint8_t getTunnelTableEntryPending()
{
  uint8_t i;
  
  for(i=0; i<gZclSETunnelMaxDataHandlingCapacity_c; i++)
  {
    if(gaZclSETunnelingTable[i].tunnelStatus == gZclSETunnel_TTE_AwaitingTunnelRequestRsp_c)
    {
      return i;
    }
  }
  
  return gZclSETunnel_Fail_c;
}

void CloseTunnel(uint16_t tunnelID)
{
  uint8_t i;
  
  for(i=0; i<gZclSETunnelMaxDataHandlingCapacity_c; i++)
  {
    if(gaZclSETunnelingTable[i].tunnelID == tunnelID)
    {
      BeeUtilZeroMemory(&gaZclSETunnelingTable[i], sizeof(tunnelingTable_t));
      gaZclSETunnelingTable[i].tunnelID = gZclSETunnel_TunnelFail_c;
    }
  }
}

void CloseTunnelTimerCallback(tmrTimerID_t tmrID)
{
  uint8_t i;
  
  for(i=0; i<gZclSETunnelMaxDataHandlingCapacity_c; i++)
  {
    if(gaZclSETunnelingTable[i].closeTunnelTmr == tmrID)
    {
      TMR_FreeTimer(tmrID);
      CloseTunnel(gaZclSETunnelingTable[i].tunnelID);
    }
  }
}

void ResetTunnelTimeout(uint8_t iTTE)
{
#if gNum_EndPoints_c != 0    
  uint16_t closeTunnelTimeout;
  zbClusterId_t tunnelClusterId = {gaZclClusterSETunneling_c};
  
  TMR_StopTimer(gaZclSETunnelingTable[iTTE].closeTunnelTmr); 
  (void)ZCL_GetAttribute(endPointList[0].pEndpointDesc->pSimpleDesc->endPoint, tunnelClusterId , gZclAttrSETunnelingCloseTunnelTimeout_c, &closeTunnelTimeout, NULL);
  TMR_StartSecondTimer(gaZclSETunnelingTable[iTTE].closeTunnelTmr, OTA2Native16(closeTunnelTimeout), CloseTunnelTimerCallback);
#endif  
}

#if gASL_ZclSETunnelingOptionals_d
zbStatus_t ZCL_SETunnel_ReadyRx(zclSETunneling_ReadyRx_t *pReq)
{
  uint8_t iTTE;
  bool_t SendReadyData = FALSE;
  tunnelingTable_t *pTTE;
  zclSETunneling_ReadyDataReq_t readyData;
  
  iTTE = getTunnelTableEntry(pReq->tunnelID);
  pTTE = &gaZclSETunnelingTable[iTTE];
  /* Check if we were able to receive more than 2 octets. The other side shouldn't send
  just 2 octets or less since a zcl long octets string has at least 3 octets. */
  if(pTTE->rxDataLength >= gZclSETunnelRxBufferSize_c - 2)
  {
    /* The ReadyData command must be sent to notify the other device that we can receive data*/
    SendReadyData = TRUE;
  }
  
  pTTE->rxDataLength = 0;
  
  if(SendReadyData)
  {
    pTTE->partnerAddrInfo.txOptions &= ~gApsTxOptionFragmentationRequested_c;
    readyData.addrInfo = pTTE->partnerAddrInfo;
    readyData.cmdFrame.tunnelId = OTA2Native16(pReq->tunnelID);
    readyData.cmdFrame.numberOfBytesLeft = Native2OTA16(gZclSETunnelRxBufferSize_c);
    (void)ZCL_SETunnel_ReadyDataReq(&readyData, pReq->client);
  }
  return gZclSuccess_c;
}
#endif

zbStatus_t ZCL_SETunnel_Server_Process_RequestTunnelReq(zbApsdeDataIndication_t *pIndication)
{
  uint8_t iTTE;
  uint16_t closeTunnelTimeout = 0, maxIncomingTransferSize;
  tunnelingTable_t *pTTE;
  zclSETunneling_RequestTunnelRsp_t requestTunnelResponse;
  zclCmdSETunneling_RequestTunnelReq_t *pReq;  
 #if gNum_EndPoints_c != 0   
  zbClusterId_t tunnelClusterId = {gaZclClusterSETunneling_c};
 #endif  
  pReq = (zclCmdSETunneling_RequestTunnelReq_t *) ((pIndication->pAsdu) + sizeof(zclFrame_t));
  AF_PrepareForReply(&requestTunnelResponse.addrInfo, pIndication);
  requestTunnelResponse.zclTransactionId = ((zclFrame_t *)pIndication->pAsdu)->transactionId;
  
  requestTunnelResponse.cmdFrame.tunnelId = gZclSETunnel_TunnelFail_c;
  requestTunnelResponse.cmdFrame.maximumIncomingtransferSize = 0;
  iTTE = getFreeTunnelTableEntry();
  pTTE = &gaZclSETunnelingTable[iTTE];
  gAcceptTunnel = FALSE;
  /* Notify the application of the tunnel request */
  BeeAppUpdateDevice(pIndication->dstEndPoint, gZclUI_SETunnel_RequestTunnelReqReceived_c, 0, 0, pReq);
  
  /* Check if the application accepts the tunnel */
  if(iTTE == gZclSETunnel_TableFull_c)
  {
    requestTunnelResponse.cmdFrame.tunnelStatus = gZclSETunnel_Busy_c;
  }
  
  /* Check if we have the ieee address of the partner*/
  else if(!APS_GetIeeeAddress(pIndication->aSrcAddr, pTTE->partnerIeeeAddr))
  {
    requestTunnelResponse.cmdFrame.tunnelStatus = gZclSETunnel_GeneralFailiure_c;
  }
  
  /* Check if the protocol ID is supported */
  else if(!IsProtocolSupported(pReq->protocolID))
  {
    requestTunnelResponse.cmdFrame.tunnelStatus = gZclSETunnel_ProtocolNotSupported_c;
  }
  
#if !gASL_ZclSETunnelingOptionals_d
  /* Check for flow control */
  else if(pReq->flowControlSupport)
  {
    requestTunnelResponse.cmdFrame.tunnelStatus = gZclSETunnel_FlowControlNotSupported_c;
  }
#endif
  
  /* Check for available resources */
  else if(iTTE == gZclSETunnel_TableFull_c)
  {
    requestTunnelResponse.cmdFrame.tunnelStatus = gZclSETunnel_Busy_c;
  }
  
  else if(!gAcceptTunnel)
  {
    requestTunnelResponse.cmdFrame.tunnelStatus = gZclSETunnel_GeneralFailiure_c;
  }
  
  else
  {
    /* Initialize the tunnel */
    pTTE->protocolId = pReq->protocolID;
    pTTE->manufacturerCode = OTA2Native16(pReq->manufacturerCode);
    pTTE->flowControlSupport = pReq->flowControlSupport;
    
    /* Set the maximum incoming transfer size */
    
    maxIncomingTransferSize = OTA2Native16(pReq->maximumIncomingtransferSize);
    
    if(pReq->maximumIncomingtransferSize < gZclSETunnelRxBufferSize_c)
    {
      pTTE->maximumIncomingTransferSize = maxIncomingTransferSize;
    }
    else
    {
      pTTE->maximumIncomingTransferSize = gZclSETunnelRxBufferSize_c;
    }
    pTTE->numberOfOctetsLeft = pTTE->maximumIncomingTransferSize;
    
    requestTunnelResponse.cmdFrame.maximumIncomingtransferSize = Native2OTA16(pTTE->maximumIncomingTransferSize);
    pTTE->rxDataLength = 0;
    pTTE->txDataLength = 0;   
#if gNum_EndPoints_c != 0       
    if(ZCL_GetAttribute(endPointList[0].pEndpointDesc->pSimpleDesc->endPoint, tunnelClusterId , gZclAttrSETunnelingCloseTunnelTimeout_c, &closeTunnelTimeout, NULL) != gZbSuccess_c)
    {
      CloseTunnel(iTTE);
      requestTunnelResponse.cmdFrame.tunnelStatus = gZclSETunnel_GeneralFailiure_c;
    }
    /* Check if all the tunnel ids were exhausted */
    else 
#endif
      if(tunnelIDCrt == gZclSETunnel_TunnelFail_c)
    {
      CloseTunnel(iTTE);
      requestTunnelResponse.cmdFrame.tunnelStatus = gZclSETunnel_NoMoreTunnelIDs_c;
    }
    else
    {
      pTTE->partnerAddrInfo = requestTunnelResponse.addrInfo;
      pTTE->tunnelID = tunnelIDCrt;
      requestTunnelResponse.cmdFrame.tunnelId = Native2OTA16(tunnelIDCrt);
      tunnelIDCrt++;
      pTTE->closeTunnelTmr = TMR_AllocateTimer();
      /* Start the close tunnel timeout timer */
      TMR_StartSecondTimer(pTTE->closeTunnelTmr, OTA2Native16(closeTunnelTimeout), CloseTunnelTimerCallback);
      requestTunnelResponse.cmdFrame.tunnelStatus = gZclSETunnel_Success_c;
      pTTE->tunnelStatus = gZclSETunnel_TTE_Active_c;
    }  
  } 
  /* Send the response */
  return ZCL_SETunnel_Server_RequestTunnelRsp(&requestTunnelResponse); 
}

zbStatus_t ZCL_SETunnel_Server_Process_CloseTunnelReq(zbApsdeDataIndication_t *pIndication)
{
  zclCmdSETunneling_CloseTunnelReq_t *pReq;
  uint8_t iTTE;
  tunnelingTable_t *pTTE;
  zbIeeeAddr_t sourceIeeeAddr;
  
  pReq = (zclCmdSETunneling_CloseTunnelReq_t *) (pIndication->pAsdu + sizeof(zclFrame_t));
  iTTE = getTunnelTableEntry(pReq->tunnelId);
  
  /* Check if the tunnel exists */
  if(iTTE != gZclSETunnel_Fail_c)
  {
    /* Check if the device that sent the request is authorized to access to tunnel */
    if(APS_GetIeeeAddress(pIndication->aSrcAddr, (uint8_t*)sourceIeeeAddr))
    {
      pTTE = &gaZclSETunnelingTable[iTTE];
      if(Cmp8Bytes(pTTE->partnerIeeeAddr, (uint8_t*)sourceIeeeAddr))
      {
        pTTE = &gaZclSETunnelingTable[iTTE];
        TMR_FreeTimer(pTTE->closeTunnelTmr);
        CloseTunnel(pReq->tunnelId);
      }
    }
  }
  
  return gZclSuccess_c;
}

zbStatus_t ZCL_SETunnel_Server_Process_GetSupportedProtocolsReq(zbApsdeDataIndication_t *pIndication)
{
  zclCmdSETunneling_GetSupportedTunnelProtocolsReq_t *pReq;
  zclSETunneling_SupportedTunnelingProtocolsRsp_t	 *pResponse;
  uint8_t protocolIDsEndIndex;
  uint8_t i;
  zbStatus_t status;
  
  // Get the request
  pReq  = (zclCmdSETunneling_GetSupportedTunnelProtocolsReq_t *) (pIndication->pAsdu + sizeof(zclFrame_t));
  
  // If the protocol offset is higher than the number of protocols no response will be sent
  if(pReq->protocolOffset >= gZclSETunnelNumberOfSupportedProtocolIDs_c)
  {
    return gZclSuccess_c;
  }
  
  // Calculate the end index in the protocolIDs list so that the received offset and the end index represent the list of protocols that would be sent in the response. gZclSETunnelNumberOfProtocolIDsInSTPR_c represent the maximum number of protocols that can be sent in a response
  if((gZclSETunnelNumberOfSupportedProtocolIDs_c - pReq->protocolOffset) <= gZclSETunnelNumberOfProtocolIDsInSTPR_c)
  {
    protocolIDsEndIndex = gZclSETunnelNumberOfSupportedProtocolIDs_c;
  }
  else
  {
    protocolIDsEndIndex = pReq->protocolOffset + gZclSETunnelNumberOfProtocolIDsInSTPR_c;
  }
  
  // Allocate memory for the response
  pResponse = MSG_Alloc(sizeof(zclSETunneling_SupportedTunnelingProtocolsRsp_t) + (protocolIDsEndIndex - pReq->protocolOffset - 1) * sizeof(zclSETunneling_Protocol_t));
  // Add the address information
  AF_PrepareForReply(&pResponse->addrInfo, pIndication);
  pResponse->zclTransactionId = ((zclFrame_t *)pIndication->pAsdu)->transactionId;
  
  // Check if the protocol list is completed
  if(protocolIDsEndIndex == gZclSETunnelNumberOfSupportedProtocolIDs_c)
  {
    pResponse->cmdFrame.protocolListComplete = TRUE;
  }
  else
  {
    pResponse->cmdFrame.protocolListComplete = FALSE;
  }
  
  // Set the number of protocols
  pResponse->cmdFrame.protocolCount = protocolIDsEndIndex - pReq->protocolOffset;
  
  // Add the protocols
  for(i = pReq->protocolOffset; i<protocolIDsEndIndex; i++)
  {
    pResponse->cmdFrame.protocolList[i].manufacturerCode = protocolIDs[i].manufacturerCode;
    pResponse->cmdFrame.protocolList[i].protocolID = protocolIDs[i].protocolID;
  }
  
  // Send the response OTA
  status = ZCL_SETunnel_Server_SupportedTunnelProtocolsRsp(pResponse);
  MSG_Free(pResponse);
  return status;
}

zbStatus_t ZCL_SETunnel_Client_Process_RequestTunnelRsp(zbApsdeDataIndication_t *pIndication)
{
  zclCmdSETunneling_RequestTunnelRsp_t *pRsp;
  
  pRsp = (zclCmdSETunneling_RequestTunnelRsp_t *) ((pIndication->pAsdu) + sizeof(zclFrame_t));
  
  if((pRsp->tunnelStatus == gZclSETunnel_Success_c) && (pRsp->tunnelId != gZclSETunnel_TunnelFail_c))
  {
    uint8_t iTTE;
    tunnelingTable_t *pTTE;
    zbIeeeAddr_t sourceIeeeAddr;
    
    iTTE = getTunnelTableEntryPending();
    
    if(iTTE == gZclSETunnel_TableFull_c)
    {
      return gZclSuccess_c;
    }
    
    pTTE = &gaZclSETunnelingTable[iTTE];
    
    if(APS_GetIeeeAddress(pIndication->aSrcAddr, (uint8_t*)sourceIeeeAddr))
    {
      if(!Cmp8Bytes(pTTE->partnerIeeeAddr, (uint8_t*)sourceIeeeAddr))
      {
        return gZclSuccess_c;
      }
    }
    else
    {
      return gZclSuccess_c;
    }
    
    pTTE->tunnelID = OTA2Native16(pRsp->tunnelId);
    pTTE->tunnelStatus = gZclSETunnel_TTE_Active_c;
    pTTE->numberOfOctetsLeft = OTA2Native16(pRsp->maximumIncomingtransferSize);
  }
  
  BeeAppUpdateDevice(pIndication->dstEndPoint, gZclUI_SETunnel_TransferDataErrorReceived_c, 0, 0, pRsp);
  return gZclSuccess_c;
}

zbStatus_t ZCL_SETunnel_Process_TransferDataReq(zbApsdeDataIndication_t *pIndication, bool_t client)
{
  zclCmdSETunneling_TransferDataReq_t *pReq;
  zbRxFragmentedHdr_t *pFragHead;
  zclSETunneling_TransferDataErrorReq_t transferDataError;
  uint8_t iTTE;
  uint8_t  rxLength;
  tunnelingTable_t *pTTE;
  uint8_t status = gZclSETunnel_NoError_c;
  zbIeeeAddr_t sourceIeeeAddr;
#if gASL_ZclSETunnelingOptionals_d
  zclSETunneling_AckTransferDataRsp_t transferDataAck;
#endif
  
  pReq = (zclCmdSETunneling_TransferDataReq_t *) (pIndication->pAsdu + sizeof(zclFrame_t));
  iTTE = getTunnelTableEntry(pReq->tunnelId);
  
  AF_PrepareForReply(&transferDataError.addrInfo, pIndication);
  transferDataError.zclTransactionId = ((zclFrame_t *)pIndication->pAsdu)->transactionId;
  transferDataError.cmdFrame.tunnelId = pReq->tunnelId;
  
  if(iTTE == gZclSETunnel_Fail_c)
  {
    status = gZclSETunnel_NoSuchTunnel_c;
  }
  else
  {
    pTTE = &gaZclSETunnelingTable[iTTE];
    
#if gASL_ZclSETunnelingOptionals_d
    if(OTA2Native16(pReq->data.length) > gZclSETunnelRxBufferSize_c - pTTE->rxDataLength)
#else
    if(OTA2Native16(pReq->data.length) > gZclSETunnelRxBufferSize_c)
#endif
    {
      status = gZclSETunnel_DataOverflow_c;
    }
    else if(APS_GetIeeeAddress(pIndication->aSrcAddr, (uint8_t*)sourceIeeeAddr))
    {
      if(!Cmp8Bytes(pTTE->partnerIeeeAddr, (uint8_t*)sourceIeeeAddr))
      {
        status = gZclSETunnel_WrongDevice_c;
      }
    }
    else
    {
      status = gZclSETunnel_WrongDevice_c;
    }
  }
  
  if(status != gZclSETunnel_NoError_c)
  {
    transferDataError.cmdFrame.transferDataStatus = status;
    
    if(client)
    {
      (void)ZCL_SETunnel_Client_TransferDataErrorReq(&transferDataError);
    }
    else
    {
      (void)ZCL_SETunnel_Server_TransferDataErrorReq(&transferDataError);
    }
    
    return gZclSuccess_c;
  }
  
  pFragHead = pIndication->fragmentHdr.pNextDataBlock;
  rxLength = pIndication->asduLength - sizeof(zclFrame_t) - 2;//tunnel id is 2 octests
  
  if (pFragHead == NULL)
  {
    FLib_MemCpy(&pTTE->rxData[pTTE->rxDataLength], pIndication->pAsdu + sizeof(zclFrame_t) + 2, rxLength);
    pTTE->rxDataLength = pTTE->rxDataLength + rxLength;
  }
  else
  {
    zbRxFragmentedHdr_t *pNextFrag = pFragHead;
    FLib_MemCpy(pTTE->rxData + pTTE->rxDataLength,  pIndication->pAsdu + sizeof(zclFrame_t) + 2, rxLength);
    pTTE->rxDataLength = pTTE->rxDataLength + rxLength;
    
    while(pNextFrag)
    {
      FLib_MemCpy(pTTE->rxData + pTTE->rxDataLength, pNextFrag->pData, pNextFrag->iDataSize);
      pTTE->rxDataLength = pTTE->rxDataLength + pNextFrag->iDataSize;
      pNextFrag = pNextFrag->pNextDataBlock;
    }
  }
  
  if(!client)
  {
    ResetTunnelTimeout(iTTE);
  }
  BeeAppUpdateDevice(pIndication->dstEndPoint, gZclUI_SETunnel_TransferDataIndication_c, 0, 0, pTTE->rxData);
#if gASL_ZclSETunnelingOptionals_d
  if (pTTE->flowControlSupport)
  {
    transferDataAck.cmdFrame.numberOfBytesLeft = gZclSETunnelRxBufferSize_c - pTTE->rxDataLength;
    transferDataAck.cmdFrame.tunnelId = pTTE->tunnelID;
    pTTE->partnerAddrInfo.txOptions &= ~gApsTxOptionFragmentationRequested_c;
    transferDataAck.addrInfo = pTTE->partnerAddrInfo;
    if(client)
    {
      (void)ZCL_SETunnel_Client_AckTransferDataRsp(&transferDataAck);
    }
    else
    {
      (void)ZCL_SETunnel_Server_AckTransferDataRsp(&transferDataAck);
    }
  }
#endif
  return gZclSuccess_c;
}

#if gASL_ZclSETunnelingOptionals_d

zbStatus_t ZCL_SETunnel_Process_AckTransferDataRsp(zbApsdeDataIndication_t *pIndication, bool_t client)
{
  zclCmdSETunneling_AckTransferDataRsp_t *pRsp;
  zclSETunneling_TransferDataReq_t *pTransferDataReq;
  uint8_t iTTE;
  tunnelingTable_t *pTTE;
  
  pRsp = (zclCmdSETunneling_AckTransferDataRsp_t *) (pIndication->pAsdu + sizeof(zclFrame_t));
  iTTE = getTunnelTableEntry(pRsp->tunnelId);
  
  if(iTTE == gZclSETunnel_Fail_c)
  {
    return gZclSuccess_c;
  }
  
  pTTE = &gaZclSETunnelingTable[iTTE];
  pTTE->numberOfOctetsLeft = OTA2Native16(pRsp->numberOfBytesLeft);
  
  if((OTA2Native16(pRsp->numberOfBytesLeft) > 0) && (pTTE->txDataSent < pTTE->txDataLength))
  {
    pTransferDataReq = (zclSETunneling_TransferDataReq_t *)pTTE->txData;
    FLib_MemCpy(&pTransferDataReq->cmdFrame.data.aData[0], &pTransferDataReq->cmdFrame.data.aData[pTTE->txDataSent - 2], pTTE->txDataLength - pTTE->txDataSent);
    pTransferDataReq->client = client;
    pTransferDataReq->cmdFrame.data.length =  pTTE->txDataLength - pTTE->txDataSent;
    (void)ZCL_SETunnel_TransferDataReq(pTransferDataReq);
  }
  
  return gZclSuccess_c;
}

zbStatus_t ZCL_SETunnel_Process_ReadyDataReq(zbApsdeDataIndication_t *pIndication, bool_t client)
{
  uint8_t iTTE;
  tunnelingTable_t *pTTE;
  zclCmdSETunneling_ReadyDataReq_t *pReq;
  zclSETunneling_TransferDataReq_t *pTransferDataReq;
  
  
  pReq = (zclCmdSETunneling_ReadyDataReq_t *) (pIndication->pAsdu + sizeof(zclFrame_t));
  iTTE = getTunnelTableEntry(OTA2Native16(pReq->tunnelId));
  
  if(iTTE == gZclSETunnel_Fail_c)
  {
    return gZclSuccess_c;
  }
  
  pTTE = &gaZclSETunnelingTable[iTTE];
  pTTE->numberOfOctetsLeft = OTA2Native16(pReq->numberOfBytesLeft);
  if(!client)
  {
    ResetTunnelTimeout(iTTE);
  }
  
  if((pTTE->txDataLength - pTTE->txDataSent) > 0)
  {
    pTransferDataReq = (zclSETunneling_TransferDataReq_t *)pTTE->txData;
    FLib_MemCpy(&pTransferDataReq->cmdFrame.data.aData[0], &pTransferDataReq->cmdFrame.data.aData[pTTE->txDataSent - 2], pTTE->txDataLength - pTTE->txDataSent);
    pTransferDataReq->client = client;
    pTransferDataReq->cmdFrame.data.length =  Native2OTA16(pTTE->txDataLength - pTTE->txDataSent);
    (void)ZCL_SETunnel_TransferDataReq(pTransferDataReq);
  }
  
  return gZclSuccess_c;
}

#endif

//client generated commands
zbStatus_t ZCL_SETunnel_Client_RequestTunnelReq
(
zclSETunneling_RequestTunnelReq_t *pReq
)
{
  uint8_t iTTE;
  tunnelingTable_t *pTTE;
  
  iTTE = getFreeTunnelTableEntry();
  pTTE = &gaZclSETunnelingTable[iTTE];
  
  if(iTTE == gZclSETunnel_TableFull_c)
  {
    return gZclFailure_c;
  }
  
  if(!APS_GetIeeeAddress(pReq->addrInfo.dstAddr.aNwkAddr, pTTE->partnerIeeeAddr))
  {
    return gZclFailure_c;
  }
  
  pTTE->tunnelStatus = gZclSETunnel_TTE_AwaitingTunnelRequestRsp_c;
  pTTE->protocolId = pReq->cmdFrame.protocolID;
  pTTE->flowControlSupport = pReq->cmdFrame.flowControlSupport;
  pTTE->manufacturerCode = OTA2Native16(pReq->cmdFrame.manufacturerCode);
  pTTE->maximumIncomingTransferSize = OTA2Native16(pReq->cmdFrame.maximumIncomingtransferSize);
  pTTE->rxDataLength = 0;
  pTTE->txDataLength = 0;
  pTTE->partnerAddrInfo = pReq->addrInfo;
  
  return ZCL_SendClientReqSeqPassed(gZclCmdSETunneling_Client_RequestTunnelReq_c, sizeof(zclCmdSETunneling_RequestTunnelReq_t),(zclGenericReq_t *)pReq);
}

zbStatus_t ZCL_SETunnel_Client_CloseTunnelReq
(
zclSETunneling_CloseTunnelReq_t *pReq
)
{
  return ZCL_SendClientReqSeqPassed(gZclCmdSETunneling_Client_CloseTunnelReq_c, sizeof(zclCmdSETunneling_CloseTunnelReq_t),(zclGenericReq_t *)pReq);
}

#if gASL_ZclZtcSETunnelingTesting_d

zbStatus_t ZCL_SETunnel_ZTCLoadFragment(zclSETunneling_ZTCLoadFragment_t *pReq)
{
  if((OTA2Native16(pReq->index) + pReq->length) > gASL_ZclZtcSETunnelingTestingBufferSize_c)
  {
    return gZclFailure_c;
  }
  FLib_MemCpy(&gZtcTestData[OTA2Native16(pReq->index) + sizeof(zclSETunneling_TransferDataReq_t) - sizeof(zclLongOctetStr_t)], &pReq->data, pReq->length);
  return gZclSuccess_c;
}

zbStatus_t ZCL_SETunnel_ZTCTransferDataReq(zclSETunneling_ZTCTransferDataReq_t *pReq)
{
  zclSETunneling_TransferDataReq_t *pTransferData = (zclSETunneling_TransferDataReq_t*)&gZtcTestData[0];
  pTransferData->client = pReq->client;
  pTransferData->cmdFrame.tunnelId = OTA2Native16(pReq->tunnelId);
  
  return ZCL_SETunnel_TransferDataReq(pTransferData);
}

#endif


zbStatus_t ZCL_SETunnel_TransferDataReq
(
zclSETunneling_TransferDataReq_t *pReq
)
{
  uint8_t iTTE;
  tunnelingTable_t *pTTE;
  uint16_t length, requestLength;
  afToApsdeMessage_t *pZclMsg;
  zbApsConfirmId_t apsConfirmID = 0;
  
  iTTE = getTunnelTableEntry(pReq->cmdFrame.tunnelId);
  
  if(iTTE == gZclSETunnel_TableFull_c)
  {
    return gZclFailure_c;
  }
  pTTE = &gaZclSETunnelingTable[iTTE];
  
  requestLength = OTA2Native16(pReq->cmdFrame.data.length);
  
#if gASL_ZclSETunnelingOptionals_d
  pTTE->txData = (uint8_t*)pReq;
  pTTE->txDataLength = requestLength + 2;
  
  if(requestLength + 2 > pTTE->numberOfOctetsLeft)
  {
    if(pTTE->numberOfOctetsLeft < 3)
    {
      return gZclFailure_c;
    }
    else if(pTTE->flowControlSupport)
    {
      requestLength = pTTE->numberOfOctetsLeft - 2;
      pTTE->txDataSent = pTTE->numberOfOctetsLeft;
    }
    else
    {
      return gZclFailure_c;
    }
  }
  else
  {
    pTTE->txDataSent = pTTE->txDataLength;
  }
#else
  if(requestLength > pTTE->maximumIncomingTransferSize)
  {
    return gZclFailure_c;
  }
#endif
  
  length = requestLength + sizeof(zclCmdSETunneling_TransferDataReq_t) - 1;
  if(!pReq->client)
  {
    ResetTunnelTimeout(iTTE);
  }
  
  pReq->cmdFrame.data.length = Native2OTA16(requestLength);
  
  if(length > gMaxApsDataSize)
  {
    pTTE->partnerAddrInfo.txOptions |= gApsTxOptionFragmentationRequested_c;
    if(pReq->client)
    {
      pZclMsg = ZCL_CreateFragmentsFrame(gZclCmdSETunneling_Client_TransferData_c, gZclFrameControl_FrameTypeSpecific, NULL, &length, &pReq->cmdFrame);
    }
    else
    {
      pZclMsg = ZCL_CreateFragmentsFrame(gZclCmdSETunneling_Server_TransferData_c, gZclFrameControl_FrameTypeSpecific | gZclFrameControl_DirectionRsp, NULL, &length, &pReq->cmdFrame);
    }
    if (pZclMsg)
    {
      return AF_DataRequestFragmentedNoCopy(&pTTE->partnerAddrInfo, pZclMsg, &apsConfirmID);
    }
  }
  else
  {
    uint8_t length8 = (uint8_t)length;
    pTTE->partnerAddrInfo.txOptions &= ~gApsTxOptionFragmentationRequested_c;
    if(pReq->client)
    {
      pZclMsg = ZCL_CreateNoFragmentsFrame(gZclCmdSETunneling_Client_TransferData_c, gZclFrameControl_FrameTypeSpecific, NULL, &length8, &pReq->cmdFrame);
    }
    else
    {
      pZclMsg = ZCL_CreateNoFragmentsFrame(gZclCmdSETunneling_Server_TransferData_c, gZclFrameControl_FrameTypeSpecific | gZclFrameControl_DirectionRsp, NULL, &length8, &pReq->cmdFrame);
    }
    if (pZclMsg)
    {
      return AF_DataRequestNoCopy(&pTTE->partnerAddrInfo, length8, pZclMsg, &apsConfirmID);
    }
  }
  
  return gZclSuccess_c;
}

zbStatus_t ZCL_SETunnel_Client_TransferDataErrorReq
(
zclSETunneling_TransferDataErrorReq_t *pReq
)
{
  return ZCL_SendClientReqSeqPassed(gZclCmdSETunneling_Client_TransferDataError_c, sizeof(zclCmdSETunneling_TransferDataErrorReq_t),(zclGenericReq_t *)pReq);
}

#if gASL_ZclSETunnelingOptionals_d

zbStatus_t ZCL_SETunnel_Client_AckTransferDataRsp
(
zclSETunneling_AckTransferDataRsp_t *pReq
)
{
  return ZCL_SendClientRspSeqPassed(gZclCmdSETunneling_Client_AckTransferDataRsp_c, sizeof(zclCmdSETunneling_AckTransferDataRsp_t),(zclGenericReq_t *)pReq);
}

zbStatus_t ZCL_SETunnel_ReadyDataReq
(
zclSETunneling_ReadyDataReq_t *pReq,
bool_t client
)
{
  if(client)
  {
  return ZCL_SendClientReqSeqPassed(gZclCmdSETunneling_Client_ReadyDataReq_c, sizeof(zclCmdSETunneling_ReadyDataReq_t),(zclGenericReq_t *)pReq);
  }
  else
  {
    return ZCL_SendServerReqSeqPassed(gZclCmdSETunneling_Server_ReadyDataReq_c, sizeof(zclCmdSETunneling_ReadyDataReq_t),(zclGenericReq_t *)pReq);
  }
}

#endif

zbStatus_t ZCL_SETunnel_Client_GetSupportedProtocolsReq
(
zclSETunneling_GetSupportedTunnelProtocolsReq_t *pReq
)
{
  return ZCL_SendClientReqSeqPassed(gZclCmdSETunneling_Client_GetSupportedTunnelProtocols_c, sizeof(zclCmdSETunneling_GetSupportedTunnelProtocolsReq_t),(zclGenericReq_t *)pReq);
}

zbStatus_t ZCL_SETunnel_SendTunnelReq(zbNwkAddr_t *pNwkAddress, uint8_t protocolID, uint16_t manufacturerCode, bool_t flowControlSupport, uint16_t maximumIncomingtransferSize)
{
  zclSETunneling_RequestTunnelReq_t tunnelReq;
  
  /* Address information */
  Copy2Bytes(tunnelReq.addrInfo.dstAddr.aNwkAddr, pNwkAddress);
  tunnelReq.addrInfo.dstAddrMode = gZbAddrMode16Bit_c;
#if gNum_EndPoints_c != 0  
  tunnelReq.addrInfo.dstEndPoint = endPointList[0].pEndpointDesc->pSimpleDesc->endPoint;
  tunnelReq.addrInfo.srcEndPoint = endPointList[0].pEndpointDesc->pSimpleDesc->endPoint;
#endif  
  tunnelReq.addrInfo.aClusterId[1] = gZclCluster_High_Tunneling_c;
  tunnelReq.addrInfo.aClusterId[0] = gZclCluster_Low_Tunneling_c;
  tunnelReq.addrInfo.txOptions = gApsTxOptionNone_c;
  tunnelReq.addrInfo.radiusCounter = gNwkMaximumDepth_c;
  
  /* Tunnel request fields */
  tunnelReq.cmdFrame.protocolID = protocolID;
  tunnelReq.cmdFrame.manufacturerCode = manufacturerCode;
  tunnelReq.cmdFrame.flowControlSupport = flowControlSupport;
  tunnelReq.cmdFrame.maximumIncomingtransferSize = maximumIncomingtransferSize;
  
  return ZCL_SETunnel_Client_RequestTunnelReq(&tunnelReq);
}

zbStatus_t ZCL_SETunnel_SendGetSupportedProtocols(zbNwkAddr_t *pNwkAddress)
{
  zclSETunneling_GetSupportedTunnelProtocolsReq_t request;
  
  /* Address information */
  Copy2Bytes(request.addrInfo.dstAddr.aNwkAddr, pNwkAddress);
  request.addrInfo.dstAddrMode = gZbAddrMode16Bit_c;
#if gNum_EndPoints_c != 0     
  request.addrInfo.dstEndPoint = endPointList[0].pEndpointDesc->pSimpleDesc->endPoint;
  request.addrInfo.srcEndPoint = endPointList[0].pEndpointDesc->pSimpleDesc->endPoint;
#endif    
  request.addrInfo.aClusterId[1] = gZclCluster_High_Tunneling_c;
  request.addrInfo.aClusterId[0] = gZclCluster_Low_Tunneling_c;
  request.addrInfo.txOptions = gApsTxOptionNone_c;
  request.addrInfo.radiusCounter = gNwkMaximumDepth_c;
  
  request.cmdFrame.protocolOffset = 0x00;
  
  return ZCL_SETunnel_Client_GetSupportedProtocolsReq(&request);
}

//server generated commands
zbStatus_t ZCL_SETunnel_Server_RequestTunnelRsp
(
zclSETunneling_RequestTunnelRsp_t *pReq
)
{
  return ZCL_SendServerRspSeqPassed(gZclCmdSETunneling_Server_RequestTunnelRsp_c, sizeof(zclCmdSETunneling_RequestTunnelRsp_t),(zclGenericReq_t *)pReq);
}

zbStatus_t ZCL_SETunnel_Server_SetNextTunnelID(uint16_t tunnelID)
{
  tunnelIDCrt = tunnelID;
  return gZclSuccess_c;
}

zbStatus_t ZCL_SETunnel_Server_TransferDataErrorReq
(
zclSETunneling_TransferDataErrorReq_t *pReq
)
{
  return ZCL_SendServerRspSeqPassed(gZclCmdSETunneling_Server_TransferDataError_c, sizeof(zclCmdSETunneling_TransferDataErrorReq_t),(zclGenericReq_t *)pReq);
}

#if gASL_ZclSETunnelingOptionals_d

zbStatus_t ZCL_SETunnel_Server_AckTransferDataRsp
(
zclSETunneling_AckTransferDataRsp_t *pReq
)
{
  return ZCL_SendServerRspSeqPassed(gZclCmdSETunneling_Server_AckTransferDataRsp_c, sizeof(zclCmdSETunneling_AckTransferDataRsp_t),(zclGenericReq_t *)pReq);
}

zbStatus_t ZCL_SETunnel_Server_ReadyData
(
zclSETunneling_ReadyDataReq_t *pReq
)
{
  return ZCL_SendServerReqSeqPassed(gZclCmdSETunneling_Server_ReadyDataReq_c, sizeof(zclCmdSETunneling_ReadyDataReq_t),(zclGenericReq_t *)pReq);
}

#endif

zbStatus_t ZCL_SETunnel_Server_SupportedTunnelProtocolsRsp
(
zclSETunneling_SupportedTunnelingProtocolsRsp_t *pReq
)
{
  return ZCL_SendServerRspSeqPassed(gZclCmdSETunneling_Server_SupportedTunnelProtocolsRsp_c, (sizeof(zclCmdSETunneling_SupportedTunnelingProtocolsRsp_t) + (pReq->cmdFrame.protocolCount - 1) * sizeof(zclSETunneling_Protocol_t)), (zclGenericReq_t *)pReq);
}
