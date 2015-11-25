/******************************************************************************
* ZclProtocInterf.c
*
* This module contains code that handles ZigBee Cluster Library commands and
* clusters for Protocol Interface Clusters
*
* Copyright (c) 2009, Freescale, Inc.  All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
* Documents used in this specification:
* [R1] - 075360r07ZB_AFG-PHHC-Profile.pdf
* [R2] - 075123r00ZB_AFG-ZigBee_Cluster_Library_Specification.pdf
*******************************************************************************/
#ifdef gHcGenericApp_d
#if gHcGenericApp_d
#include "EmbeddedTypes.h"
#include "zigbee.h"
#include "FunctionLib.h"
#include "BeeStackConfiguration.h"
#include "BeeStack_Globals.h"
#include "AppAfInterface.h"
#include "AfApsInterface.h"
#include "BeeStackInterface.h"
#include "ApsMgmtInterface.h"
#include "OepAgent.h"

#include "zcl.h"
#include "ZclProtocInterf.h"
#include "HcProfile.h"

#include "ZigBee.h"
#include "Oep11073.h"
#include "FunctionLib.h"
#include "MsgSystem.h"
#if gZclEnablePartition_d
#include "ZclPartition.h"
#endif
#include "ASL_ZdpInterface.h"



/******************************************************************************
*******************************************************************************
* Private macros
*******************************************************************************
******************************************************************************/

#define gZclInvalidIdleTimer_c 0xFFFF

#if gZclEnablePartition_d
#define g11073TunnelClusterPartitionLenThresholdDefault 0xFE
#define gMaxPartitionBuffer 0xF0
#endif

/******************************************************************************
*******************************************************************************
* Private prototypes
*******************************************************************************
******************************************************************************/

#if gASL_ZclGroupAddGroupReq_d
zclStatus_t ASL_ZclGroupAddGroupReq(zclGroupAddGroupReq_t *pReq);
zclStatus_t ASL_ZclGroupAddGroupIfIdentifyReq(zclGroupAddGroupIfIdentifyingReq_t*pReq);
zbStatus_t ASL_ZclAddGroupIfIdentifyUI(uint8_t mode, zbGroupId_t aGroupId);
#else
#define ASL_ZclGroupAddGroupReq(pReq)  FALSE
#define ASL_ZclGroupAddGroupIfIdentifyReq(pReq) FALSE
#define ASL_ZclAddGroupIfIdentifyUI(mode, aGroupId)
#endif


zbStatus_t ZCL_11073TunnelClusterHandleTransferAPDU
(
  zbApsdeDataIndication_t *pIndication, 
  afDeviceDef_t *pDev
);
#if (gHcDeviceType_d != gHcDcu_c) 
zbStatus_t ZCL_11073TunnelClusterHandleConnectRequest
(
  zbApsdeDataIndication_t *pIndication, 
  afDeviceDef_t *pDev
);
zbStatus_t ZCL_11073TunnelClusterHandleDisconnectRequest
(
  zbApsdeDataIndication_t *pIndication, 
  afDeviceDef_t *pDev
);
#define ZCL_11073TunnelClusterHandleConnectStatusNotif(pIndication, pDev) gZclUnsupportedClusterCommand_c
#else
zbStatus_t ZCL_11073TunnelClusterHandleConnectStatusNotif
(
  zbApsdeDataIndication_t *pIndication, 
  afDeviceDef_t *pDev
);
#define ZCL_11073TunnelClusterHandleConnectRequest(pIndication, pDev) gZclUnsupportedClusterCommand_c
#define ZCL_11073TunnelClusterHandleDisconnectRequest(pIndication, pDev) gZclUnsupportedClusterCommand_c
#endif



/******************************************************************************
*******************************************************************************
* Private type definitions
*******************************************************************************
******************************************************************************/

#if gZclEnablePartition_d
typedef struct IEEE11073ProtocolTunnelPartitionTxFrameBuffer_tag {
  uint8_t *nextBuffer;
  uint8_t bufferLen;
  union 
  {
    uint8_t data[1];
    zclFrame_t zclHeader;
  } payload;
} IEEE11073ProtocolTunnelPartitionTxFrameBuffer_t;


typedef struct IEEE11073ProtocolTunnelPartitionRxFrameBuffer_tag {
  bool_t framePending;
  uint8_t zclTransactionId;
  zbNwkAddr_t srcAddr;
  uint8_t srcEndPoint;
  uint8_t dstEndPoint;
  uint16_t totalLen;
  uint8_t fragmentedMsg[gMaxPartitionBuffer];
} IEEE11073ProtocolTunnelPartitionRxFrameBuffer_t;
#endif


/******************************************************************************
*******************************************************************************
* Public memory definitions
*******************************************************************************
******************************************************************************/


const zclAttrDef_t gaGtClusterAttrDef[] = {
  {gZclAttrGTMaxInTrsSize_c,  gZclDataTypeUint16_c,    gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(int16_t), (void *)MbrOfs(zclGenericAppAttrRAM_t,maxInTrsSize)},
  {gZclAttrGTMaxOutTrsSize_c, gZclDataTypeUint16_c,    gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(int16_t), (void *)MbrOfs(zclGenericAppAttrRAM_t,maxOutTrsSize)},
  {gZclAttrGTProtoAddr_c,     gZclDataTypeOctetStr_c, gZclAttrFlagsInRAM_c, gZclGtMaxProtocolAddress, (void *)MbrOfs(zclGenericAppAttrRAM_t,protocolAddress)}
  
};

const zclAttrDefList_t gGtClusterAttrDefList = {
  NumberOfElements(gaGtClusterAttrDef),
  gaGtClusterAttrDef
};


const zclAttrDef_t ga11073TunnelClusterAttrDef[] = {  
  {gZclAttr11703ProtocTunnelDeviceIdList_c, gZclDataTypeArray_c, gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(uint8_t) + sizeof(uint16_t)+sizeof(int16_t), (void *)MbrOfs(zclGenericAppAttrRAM_t,deviceIDList)},
  {gZclAttr11703ProtocTunnelManagerTarget_c, gZclDataTypeIeeeAddr_c, gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, 8, (void *)MbrOfs(zclGenericAppAttrRAM_t,managerTarget)},
  {gZclAttr11703ProtocTunnelManagerEndpoint_c, gZclDataTypeUint8_c, gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, 1, (void *)MbrOfs(zclGenericAppAttrRAM_t,managerEndpoint)},
  {gZclAttr11703ProtocTunnelConnected_c, gZclDataTypeBool_c, gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, 1, (void *)MbrOfs(zclGenericAppAttrRAM_t,connected)},
  {gZclAttr11703ProtocTunnelPreemptible_c, gZclDataTypeBool_c, gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, 1, (void *)MbrOfs(zclGenericAppAttrRAM_t,preemtible)},        
  {gZclAttr11703ProtocTunnelIdleTimeout_c, gZclDataTypeUint16_c, gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(uint16_t), (void *)MbrOfs(zclGenericAppAttrRAM_t,idleTimeout)}
};

const zclAttrDefList_t g11073TunnelClusterAttrDefList = {
  NumberOfElements(ga11073TunnelClusterAttrDef),
  ga11073TunnelClusterAttrDef
};

zbClusterId_t  ClusterId={gaZclCluster11073Tunnel_c};

#if gZclEnablePartition_d
static IEEE11073ProtocolTunnelPartitionTxFrameBuffer_t* gpPendingPartitionTxFrame;
static IEEE11073ProtocolTunnelPartitionRxFrameBuffer_t gPendingPartitionRxFrame;

static uint8_t g11073TunnelClusterPartitionLenThreshold = g11073TunnelClusterPartitionLenThresholdDefault;
#endif

/******************************************************************************
*******************************************************************************
* Private functions
*******************************************************************************
******************************************************************************/

zbStatus_t ZCL_SimpleRequest
(
  afAddrInfo_t *pAddrInfo,  
  zclCmd_t command, 
  zclFrameControl_t frameControl, 
  uint8_t iPayloadLen,  
  uint8_t* pData
) 
{
  afToApsdeMessage_t *pMsg;

  pMsg = ZCL_CreateFrame( pAddrInfo, 
                          command,
                          frameControl, 
                          NULL, 
                          &iPayloadLen,
                          pData);
  if(!pMsg)
    return gZclNoMem_c;
 
 return ZCL_DataRequestNoCopy(pAddrInfo, iPayloadLen,pMsg);  
}

/*****************************************************************************
* ZCL_11073IdleTimerCallback
*
* Called when the idle timer expires to auto-disconnect from manager
*****************************************************************************/
void ZCL_11073IdleTimerCallback
(
  tmrTimerID_t timer
) 
{
  index_t i;
  zbSimpleDescriptor_t *pSimpleDesc;
  zclGenericAppAttrRAM_t *pDeviceParams;
  zbProfileId_t  ProfileId = {gHcProfileIdb_c};
  
  /* determine the endpoint for this timer */
  for(i=0; i<gNum_EndPoints_c; ++i) {
    const endPointDesc_t *pEndpointDesc = endPointList[i].pEndpointDesc;
    if(pEndpointDesc) {
      uint8_t endPoint;
      pSimpleDesc = pEndpointDesc->pSimpleDesc;
      endPoint = pSimpleDesc->endPoint;
      /* more ways than one to validate the endpoint is a healthcare one */
      /* we use the profile id for now */
      if(IsEqual2Bytes(pSimpleDesc->aAppProfId, ProfileId)) {
        pDeviceParams = (zclGenericAppAttrRAM_t*)((afDeviceDef_t *)endPointList[i].pDevice)->pData;
        /* check if the timer is the one for which we're getting called back */
        if (pDeviceParams->idleTimer == timer) {
          zclCmd11073_ConnectStatusNotif_t mgrNotif;
          OepFragmentedApdu_t *FragmentedpApdu;  
  
          /* kill the timer */
          TMR_FreeTimer(pDeviceParams->idleTimer);
          pDeviceParams->idleTimer = gTmrInvalidTimerID_c;
          
          /* set connected to FALSE */
          pDeviceParams->connected = FALSE;
          
          FragmentedpApdu = OepGetTransportDisconnectedFragment(endPoint);          
          ZCL_Oep_SAPHandler(FragmentedpApdu); 
          
          /* send disconnected status to the manager */
          
          mgrNotif.addrInfo.srcEndPoint = endPoint;
          mgrNotif.addrInfo.dstAddrMode = gZbAddrMode64Bit_c;
          mgrNotif.addrInfo.dstEndPoint = pDeviceParams->managerEndpoint;
          mgrNotif.addrInfo.txOptions = afTxOptionsDefault_c;
          Copy2Bytes(mgrNotif.addrInfo.aClusterId, ClusterId);
          mgrNotif.connectStatus = zcl11073Status_Disconnected;
          
          Copy8Bytes(mgrNotif.addrInfo.dstAddr.aIeeeAddr, pDeviceParams->managerTarget);          
          (void)IEEE11073ProtocolTunnel_ConnectStatusNotif(&mgrNotif);                
        }
      }
    }
  }
}


/*****************************************************************************
* ZCL_11073ResetIdleTimer
*
* Finds the matching idle timer value based on device definition and either
* restarts or stops the timer.
*****************************************************************************/
void ZCL_11073ResetIdleTimer
(
  afDeviceDef_t *pDev, bool_t killTimer
) 
{
  zclGenericAppAttrRAM_t *pDeviceParams = (zclGenericAppAttrRAM_t*)pDev->pData;
  uint16_t idleTimeout;
  tmrTimerID_t timerId = pDeviceParams->idleTimer;

  if (timerId != gTmrInvalidTimerID_c) {
    if (!killTimer) {
      idleTimeout = OTA2Native16(pDeviceParams->idleTimeout);
      TMR_StartMinuteTimer(timerId, idleTimeout, ZCL_11073IdleTimerCallback);
    }
    else {
      TMR_FreeTimer(timerId);
      pDeviceParams->idleTimer = gTmrInvalidTimerID_c;      
    }
  }
}


/*****************************************************************************
* GetApduFromIndication
*
* Parse the pIndication fragmented frame
* Return a fragmented 11073 APDU frame
* pApdu points to the beginning of Apdu in the pIndication payload
*****************************************************************************/


OepFragmentedApdu_t *GetApduFromIndication(zbApsdeDataIndication_t *pIndication, zclApdu_t *pTunnelApdu)
{
  OepFragmentedApdu_t *fragmentedApdu; /* The Apdu we will return */
      
  uint8_t bytesNeeded;
  OepApduFragment_t *fragment, *prevFragment;
  uint8_t* nextFrag;
  uint8_t len;

  
  /* 
    We actually need to consider the start of the apsdeToAfMessage_t,
    which is 1 byte before the start of the pIndication.
  */
  
  /* minimum size of first fragment */
  bytesNeeded = sizeof(OepFragmentedApdu_t) - sizeof(uint8_t);
  
  /* payload Len */
  len = pIndication->asduLength - 
                    (uint8_t)((uint8_t *)pTunnelApdu - (uint8_t *)pIndication->pAsdu);
  
  /* add actual first fragment length */
  bytesNeeded += len;
                    
  /* ZHCTODO if bytesNeeded >  gMaxPacketBufferSize_c there will be alloc issues
     but that should not happen */
  
  /* check if we can get away with allocating a small buffer or a large buffer is needed */
  if (bytesNeeded <= sizeof(block1sizeOrigin_t))
    fragmentedApdu = (OepFragmentedApdu_t*)MSG_Alloc(sizeof(block1sizeOrigin_t));
  else
    fragmentedApdu = (OepFragmentedApdu_t*)MSG_Alloc(gMaxPacketBufferSize_c);  
  
  /* copy addresses for first fragment */
  Copy2Bytes(fragmentedApdu->aSrcAddr, pIndication->aSrcAddr);
  fragmentedApdu->dstEndPoint = pIndication->dstEndPoint;
  fragmentedApdu->srcEndPoint = pIndication->srcEndPoint;
  
  fragmentedApdu->fragment.len = len;
  fragmentedApdu->fragment.nextFragment = NULL;
  
  FLib_MemCpy(fragmentedApdu->fragment.data, (uint8_t*)pTunnelApdu, len);
  
  if (pIndication->fragmentHdr.pNextDataBlock == NULL)
    return fragmentedApdu;
  
  prevFragment = &fragmentedApdu->fragment;
  nextFrag = (uint8_t *)pIndication->fragmentHdr.pNextDataBlock;
 
  while (nextFrag != NULL) {
    zbRxFragmentedHdr_t* apsFragment = (zbRxFragmentedHdr_t *)nextFrag;
    uint8_t iDataSize = apsFragment->iDataSize;
    
    nextFrag = (uint8_t *)apsFragment->pNextDataBlock;   
    bytesNeeded = MbrOfs(OepApduFragment_t, data) + iDataSize;

    if (bytesNeeded <= sizeof(block1sizeOrigin_t))
      fragment = (OepApduFragment_t*)MSG_Alloc(sizeof(block1sizeOrigin_t));
    else
      fragment = (OepApduFragment_t*)MSG_Alloc(gMaxPacketBufferSize_c);
    
    prevFragment->nextFragment = fragment;
    
    fragment->len = iDataSize;
    fragment->nextFragment = NULL;
    
    prevFragment = fragment;
    
    FLib_MemCpy(fragment->data, apsFragment->pData, iDataSize);
    
 
  }

  return fragmentedApdu; 
}


/*****************************************************************************
* GetApduFromPartitionBuffer
*
* Parse the pIndication fragmented frame from a partitioned frame
* Return a fragmented 11073 APDU frame
* pApdu points to the beginning of Apdu in the pIndication payload
*****************************************************************************/
#if gZclEnablePartition_d
OepFragmentedApdu_t *GetApduFromPartitionBuffer(IEEE11073ProtocolTunnelPartitionRxFrameBuffer_t *buffer)
{
  OepFragmentedApdu_t *fragmentedApdu; /* The Apdu we will return */
  uint8_t *pTunnelApdu = buffer->fragmentedMsg + sizeof(zclFrame_t);
  uint16_t len  = buffer->totalLen - sizeof(zclFrame_t);
  OepApduFragment_t *fragment, *prevFragment;
  uint8_t headerSize = sizeof(OepFragmentedApdu_t) + sizeof(uint8_t);
  uint8_t fragLen = gMaxPacketBufferSize_c - headerSize;
  uint16_t currentLen = 0;
  
  if (len < fragLen)
    fragLen = (uint8_t)len;

  fragmentedApdu = (OepFragmentedApdu_t*)MSG_Alloc(fragLen + headerSize);  
  
  /* copy addresses for first fragment */
  Copy2Bytes(fragmentedApdu->aSrcAddr, buffer->srcAddr);
  fragmentedApdu->dstEndPoint = buffer->dstEndPoint;
  fragmentedApdu->srcEndPoint = buffer->srcEndPoint;
  
  fragmentedApdu->fragment.len = fragLen;
  fragmentedApdu->fragment.nextFragment = NULL;
  
  FLib_MemCpy(fragmentedApdu->fragment.data, (uint8_t*)pTunnelApdu, fragLen);
  
  currentLen += fragLen;
  
  if (currentLen >= len)
    return fragmentedApdu;
  
  prevFragment = &fragmentedApdu->fragment;
 
  while (currentLen < len) {
    headerSize = sizeof(OepApduFragment_t) + sizeof(uint8_t);
    fragLen = gMaxPacketBufferSize_c - headerSize;
    if (len - currentLen < fragLen)
      fragLen = len - currentLen;
    
    fragment = (OepApduFragment_t*)MSG_Alloc(fragLen + headerSize);

    prevFragment->nextFragment = fragment;
    
    fragment->len = fragLen;
    fragment->nextFragment = NULL;
    prevFragment = fragment;
    
    FLib_MemCpy(fragment->data, (uint8_t*)pTunnelApdu + currentLen, fragLen);
    
    currentLen += fragLen;
  }

  return fragmentedApdu; 
}
#endif



/*****************************************************************************
* IEEE11073ProtocolTunnel_TransferApduPtr
*
* Sends a unpartitioned 11073 APDU
*****************************************************************************/
zbStatus_t IEEE11073ProtocolTunnel_TransferApduPtrInternal
(
    zclCmd11073_TransferApduPtr_t *pReq
)
{
  uint8_t iPayloadLen;
#if (gHcDeviceType_d != gHcDcu_c)      
  bool_t connected = FALSE;
  uint8_t connectedLen; 
  (void)ZCL_GetAttribute(pReq->addrInfo.srcEndPoint, ClusterId, gZclAttr11703ProtocTunnelConnected_c, 
                            &connected, &connectedLen);    
  if (!connected)
      return gZclFailure_c;
  
  ZCL_11073ResetIdleTimer(AF_GetEndPointDevice(pReq->addrInfo.srcEndPoint), FALSE);  
#endif                              

  iPayloadLen = (uint8_t)OTA2Native16(pReq->apdu->apduLen);
  iPayloadLen  += 2;
              
  return ZCL_SimpleRequest(&(pReq->addrInfo), gZclCmdRx11073_TransferAPDU_c,
                           gZclFrameControl_FrameTypeSpecific | gZclFrameControl_DisableDefaultRsp,  
                           iPayloadLen, (uint8_t*)pReq->apdu);

}



/*****************************************************************************
* IEEE11073ProtocolTunnel_PartitionCallback
*
* Called by the partitioned cluster when it needs to signal a data or control
* event to the IEEE 11073 tunnel cluster
*****************************************************************************/
#if gZclEnablePartition_d
uint8_t* IEEE11073ProtocolTunnel_PartitionCallback
(
  zclPartitionAppEvent_t* pPartitionEvent,
  uint8_t* dataLen
) 
{
  if (pPartitionEvent->eventType == gZclPartitionAppEventType_DataRequest) {
    
    uint8_t partitionSize = pPartitionEvent->frameInfo->partitionAttrs.partitionedFrameSize;

    uint8_t* msg;
    uint8_t dataLength = partitionSize;
    uint16_t bufferIndex = pPartitionEvent->partitionedFrameId * partitionSize;
    if (gPendingPartitionRxFrame.totalLen - bufferIndex < partitionSize)
      dataLength = gPendingPartitionRxFrame.totalLen - bufferIndex;
    
    msg = MSG_Alloc(partitionSize);
    FLib_MemCpy(msg, (uint8_t*)gPendingPartitionRxFrame.fragmentedMsg + bufferIndex, dataLength);
    
    *dataLen = dataLength;
    return msg;        
    
  } else if  (pPartitionEvent->eventType == gZclPartitionAppEventType_FrameComplete) {
      zbClusterId_t ClusterId={gaZclClusterGeneralTunnel_c};
      OepFragmentedApdu_t *pFragmentedApdu = GetApduFromPartitionBuffer(&gPendingPartitionRxFrame);
      gPendingPartitionRxFrame.framePending = FALSE;
      BeeAppUpdateDevice(gPendingPartitionRxFrame.dstEndPoint, gHcEventApduRcvd_c, 0, ClusterId, pFragmentedApdu);                
      
  } else if  (pPartitionEvent->eventType == gZclPartitionAppEventType_TxFrameComplete) {
      IEEE11073ProtocolTunnelPartitionTxFrameBuffer_t* currentBuffer = gpPendingPartitionTxFrame;
      while (currentBuffer != NULL) {
        IEEE11073ProtocolTunnelPartitionTxFrameBuffer_t* nextBuffer = 
                (IEEE11073ProtocolTunnelPartitionTxFrameBuffer_t*)currentBuffer->nextBuffer;
        MSG_Free(currentBuffer);
        currentBuffer = nextBuffer;
      }
      gpPendingPartitionTxFrame = NULL;
  }  
  return NULL;
}       
#endif

/*****************************************************************************
* ZCL_11073TunnelPartitionServer
*
* Handles data buffering for incoming partitioned fragments
*****************************************************************************/
#if gZclEnablePartition_d
void ZCL_11073TunnelPartitionServer(zclPartitionedDataIndication_t* partitionDataIndication) 
{
  if (!gPendingPartitionRxFrame.framePending) {
    gPendingPartitionRxFrame.framePending = TRUE;
    gPendingPartitionRxFrame.zclTransactionId = partitionDataIndication->zclTransactionId;
    Copy2Bytes(gPendingPartitionRxFrame.srcAddr, partitionDataIndication->srcAddr);
    gPendingPartitionRxFrame.srcEndPoint = partitionDataIndication->srcEndPoint;
    gPendingPartitionRxFrame.dstEndPoint = partitionDataIndication->dstEndPoint;
  }
  
  if (gPendingPartitionRxFrame.zclTransactionId == partitionDataIndication->zclTransactionId)
  {
    uint16_t bufferIndex = partitionDataIndication->partitionId * partitionDataIndication->partitionLength;
    gPendingPartitionRxFrame.totalLen = bufferIndex + partitionDataIndication->dataLength;
    FLib_MemCpy((uint8_t*)gPendingPartitionRxFrame.fragmentedMsg + bufferIndex, 
                partitionDataIndication->partitionedFrame + 1,  partitionDataIndication->dataLength);
  }
}
#endif                    
                    

/******************************************************************************
*******************************************************************************
* Public functions
*******************************************************************************
******************************************************************************/

/*****************************************************************************
* ZCL_11073Init 
*
* Registers the 11073 tunnel cluster to the partition cluster
*****************************************************************************/
void ZCL_11073Init() {
#if gZclEnablePartition_d
  zclPartitionedClusterInfo_t clusterReg;
  clusterReg.clusterId[0] = 0x14;
  clusterReg.clusterId[1] = 0x06;
  clusterReg.dataIndHandler = ZCL_11073TunnelPartitionServer;
  clusterReg.dataReqHandler = IEEE11073ProtocolTunnel_PartitionCallback;
  clusterReg.clusterBufferSize = gMaxPartitionBuffer;
  
  gpPendingPartitionTxFrame = NULL;
  gPendingPartitionRxFrame.framePending = FALSE;  
  
  (void)ZCL_PartitionRegisterCluster(&clusterReg);
  //ZCL_11073TunnelClusterServer(NULL, NULL);
#endif  
}


/*****************************************************************************
* ZCL_GenericTunnelClusterClient
*
* GenericTunnel Client handler
*****************************************************************************/
zbStatus_t ZCL_GenericTunnelClusterClient
(
    zbApsdeDataIndication_t *pIndication, 
    afDeviceDef_t *pDev
)
{
    zclFrame_t *pFrame;
    zclCmd_t command;

    /* prevent compiler warning */
    (void)pDev;
    pFrame = (void *)pIndication->pAsdu;
    /* handle the command */
    command = pFrame->command;  
    switch(command)
    {
          /* These should be passed up to a host*/
        case gZclCmdTxGTMpaRes_c:
        case gZclCmdTxGTAdvertiseProtoAddr_c:       
        
          return gZbSuccess_c;
        default:
          return gZclUnsupportedClusterCommand_c;
    }
}


/*****************************************************************************
* ZCL_GenericTunnelClusterServer
*
* Generic Tunnel Server handler
*****************************************************************************/
zbStatus_t ZCL_GenericTunnelClusterServer
(
    zbApsdeDataIndication_t *pIndication, 
    afDeviceDef_t *pDev
)
{
    zclFrame_t    *pFrame;
    zclCmd_t      command;
    zbClusterId_t ClusterId={gaZclClusterGeneralTunnel_c};
    uint8_t       ProtocolAddress[gZclGtMaxProtocolAddress];
    uint8_t       ProtocolAddressLen;
    uint8_t       *mpa;

    /* prevent compiler warning */
    (void)pDev;
    pFrame = (void *)pIndication->pAsdu;
    /* handle the command */
    command = pFrame->command;  
    switch(command)
    {        
        case gZclCmdRxGTMpa_c:
        {
          afAddrInfo_t replyAddrInfo;
          
          /* Check received protocol address with own */
          (void)ZCL_GetAttribute(pIndication->dstEndPoint, ClusterId, gZclAttrGTProtoAddr_c, &ProtocolAddress, &ProtocolAddressLen);
          mpa = (uint8_t*)((uint8_t*)pIndication->pAsdu + sizeof(zclFrame_t));
                    
          if (FLib_MemCmp(mpa, ProtocolAddress, ProtocolAddressLen))
          {
            AF_PrepareForReply((afAddrInfo_t*)&replyAddrInfo, pIndication);            
            return ZCL_Reply(pIndication, 0, NULL);
          }
          else
            return gZclSuccess_c;
        }              
        default:
          return gZclUnsupportedClusterCommand_c; 
    }
}




/*****************************************************************************
* GenericTunnel_MatchProtocolAddress
*
* Send a MatchProtocolAddress command to the server
*****************************************************************************/
zbStatus_t GenericTunnel_MatchProtocolAddress 
(
    zclCmdGtMpa_t *pReq
)
{
  return  ZCL_SimpleRequest(&(pReq->addrInfo), gZclCmdRxGTMpa_c, gZclFrameControl_FrameTypeSpecific,
                            pReq->protocolAddress.AddrLen+1, (uint8_t*)&(pReq->protocolAddress));
}




/*****************************************************************************
* GenericTunnel_MatchProtocolAddressResponse
*
* Respond with MatchProtocolAddressRes to the client
*****************************************************************************/
zbStatus_t GenericTunnel_MatchProtocolAddressResponse 
(
    zclCmdGtMpaRes_t *pReq
)
{
  
  return  ZCL_SimpleRequest(&(pReq->addrInfo), gZclCmdTxGTMpaRes_c, 
                            gZclFrameControl_FrameTypeSpecific | gZclFrameControl_DirectionRsp,
                            0, NULL);
}




/*****************************************************************************
* GenericTunnel_AdvertiseProtocolAddress
*
* Send a AdvertiseProtocolAddress command 
*****************************************************************************/
zbStatus_t GenericTunnel_AdvertiseProtocolAddress 
(
    zclCmdGtAdvertiseProtoAddr_t *pReq
)
{	
  return  ZCL_SimpleRequest(&(pReq->addrInfo), gZclCmdTxGTAdvertiseProtoAddr_c, 
                            gZclFrameControl_FrameTypeSpecific | gZclFrameControl_DirectionRsp,
                            pReq->protocolAddress.AddrLen+1, (uint8_t*)&(pReq->protocolAddress));
}



/*****************************************************************************
* ZCL_11073TunnelClusterClient
*
* 11073 Tunnel Client
*****************************************************************************/
zbStatus_t ZCL_11073TunnelClusterClient
(
  zbApsdeDataIndication_t *pIndication,
  afDeviceDef_t *pDev
)
{
    zclFrame_t *pFrame;
    zclCmd_t command;

    /* prevent compiler warning */
    (void)pDev;
    pFrame = (void *)pIndication->pAsdu;
    /* handle the command */
    command = pFrame->command;  
    switch(command)
    {
          /* These should be passed up to a host*/
   
        default:
          return gZclUnsupportedClusterCommand_c;
    }
}



zbStatus_t ZCL_11073TunnelClusterHandleTransferAPDU
(
  zbApsdeDataIndication_t *pIndication, 
  afDeviceDef_t *pDev
)
{
  zclApdu_t      *pApdu;

  OepFragmentedApdu_t *pFragmentedApdu;

#if (gHcDeviceType_d != gHcDcu_c)    
    zclGenericAppAttrRAM_t* pEndpointData;  
#endif    
  /* prevent compiler warning */
  (void)pDev;
  (void)pIndication;
  
  
#if (gHcDeviceType_d != gHcDcu_c)    
    pEndpointData = (zclGenericAppAttrRAM_t*)pDev->pData;        
  
    if (!pEndpointData->connected)
      return gZclSuccess_c;
    ZCL_11073ResetIdleTimer(pDev, FALSE);
#endif
      

    pApdu = (zclApdu_t*)((uint8_t*)pIndication->pAsdu + sizeof(zclFrame_t));

    /*  Handle received Apdu */      
    pFragmentedApdu = GetApduFromIndication(pIndication, pApdu);

    BeeAppUpdateDevice(pIndication->dstEndPoint, gHcEventApduRcvd_c, 0, ClusterId, pFragmentedApdu);              

    return gZclSuccess_c; 
}

#if (gHcDeviceType_d != gHcDcu_c) 
zbStatus_t ZCL_11073TunnelClusterHandleConnectRequest
(
  zbApsdeDataIndication_t *pIndication, 
  afDeviceDef_t *pDev
)
{
  zclFrame_t     *pFrame;
         
  zclCmd11073_ConnectStatusNotif_t statusNotif;
  zclCmd11073_ConnectRequestParams_t* pConnectRequest;
  zclGenericAppAttrRAM_t* pEndpointData;
  /* prevent compiler warning */
  (void)pDev;
  

  pFrame = (void *)pIndication->pAsdu;
  pEndpointData = (zclGenericAppAttrRAM_t*)pDev->pData;    

  
  /* received on agent - connection request */
  pConnectRequest = (zclCmd11073_ConnectRequestParams_t*)(pFrame + 1);
  AF_PrepareForReply(&statusNotif.addrInfo, pIndication);                              

  if (pEndpointData->connected) {
    statusNotif.connectStatus = zcl11073Status_AlreadyConnected;
    (void)IEEE11073ProtocolTunnel_ConnectStatusNotif(&statusNotif);
  } else {
    Oep11073AddressInfo_t agentAddrInfo;
    OepFragmentedApdu_t *ConnectApdu, *StartApdu;  

    pEndpointData->connected = TRUE;
    pEndpointData->preemtible = pConnectRequest->connectControl;
    pEndpointData->idleTimeout = pConnectRequest->idleTimeout;
    pEndpointData->managerEndpoint = pConnectRequest->managerEndpoint;
    FLib_MemCpy(pEndpointData->managerTarget, &pConnectRequest->ieeeAddress, MbrSizeof(zclGenericAppAttrRAM_t, managerTarget));
    
    statusNotif.connectStatus = zcl11073Status_Connected;
    
    /* set the IEEE 11073 addressing info for the agent */
    agentAddrInfo.dstAddrMode = gZbAddrMode64Bit_c;
    FLib_MemCpy(agentAddrInfo.aDstAddr, pConnectRequest->ieeeAddress, sizeof(pConnectRequest->ieeeAddress));
    agentAddrInfo.dstEndPoint = pIndication->srcEndPoint;
    agentAddrInfo.srcEndPoint = pIndication->dstEndPoint;
    OepAgentSetAddressInfo(&agentAddrInfo);        
    
    /* allocate and start the idle timer */
    if (pEndpointData->idleTimer == gTmrInvalidTimerID_c && pConnectRequest->idleTimeout != gZclInvalidIdleTimer_c)
      pEndpointData->idleTimer = TMR_AllocateMinuteTimer();
    if (pEndpointData->idleTimer != gTmrInvalidTimerID_c && pConnectRequest->idleTimeout != gZclInvalidIdleTimer_c)
      TMR_StartMinuteTimer(pEndpointData->idleTimer, OTA2Native16(pConnectRequest->idleTimeout), ZCL_11073IdleTimerCallback);
    
    /* send the transport connected indication */
    ConnectApdu = OepGetTransportConnectedFragment(pIndication->dstEndPoint);          
    ZCL_Oep_SAPHandler(ConnectApdu);         
    
    /* send notification back to manager */
    (void)IEEE11073ProtocolTunnel_ConnectStatusNotif(&statusNotif);
    
    ASL_NWK_addr_req(NULL, (uint8_t *)gaBroadcastRxOnIdle, pConnectRequest->ieeeAddress, 0x00, 0x00);
    
    /* start the IEEE 11073 association */
    StartApdu = OepGetStartOepFragment(pIndication->dstEndPoint, TRUE, oepStartAssocOrigin_EndDeviceBind);          
    ZCL_Oep_SAPHandler(StartApdu); 
  }

  return gZclSuccess_c;  
}
#endif

#if (gHcDeviceType_d != gHcDcu_c) 
zbStatus_t ZCL_11073TunnelClusterHandleDisconnectRequest
(
  zbApsdeDataIndication_t *pIndication, 
  afDeviceDef_t *pDev
)
{
  zclFrame_t     *pFrame;

  bool_t connected = FALSE;
  zclCmd11073_ConnectStatusNotif_t statusNotif;
  zclGenericAppAttrRAM_t* pEndpointData;
  /* prevent compiler warning */
  (void)pDev;
  

  pFrame = (void *)pIndication->pAsdu;
  
  pEndpointData = (zclGenericAppAttrRAM_t*)pDev->pData;  

  AF_PrepareForReply(&statusNotif.addrInfo, pIndication); 
  statusNotif.connectStatus = zcl11073Status_Disconnected;      
  if (pEndpointData->connected) {
    uint8_t tempAddress[8];
    bool_t authorized = FALSE;
    
    uint8_t *pMgrAddress = (uint8_t *)&pEndpointData->managerTarget;

    /* checking authorization */
    if (pEndpointData->preemtible || FLib_MemCmp((uint8_t*)(pFrame + 1), pMgrAddress, 8))
      authorized = TRUE;

    /* acting on authorization */
    if (authorized) {
      zclCmd11073_ConnectStatusNotif_t mgrNotif;
      uint8_t * aRequestIeeeAddr=NULL;
      OepFragmentedApdu_t *FragmentedpApdu;            
      connected = FALSE;
      
      FragmentedpApdu = OepGetTransportDisconnectedFragment(pIndication->dstEndPoint);          
      ZCL_Oep_SAPHandler(FragmentedpApdu); 
      
      pEndpointData->connected = connected;

      ZCL_11073ResetIdleTimer(pDev, TRUE);     
      
      aRequestIeeeAddr = APS_GetIeeeAddress(statusNotif.addrInfo.dstAddr.aNwkAddr, tempAddress);
      if (aRequestIeeeAddr == NULL || !FLib_MemCmp(aRequestIeeeAddr, pMgrAddress, 8)) {
        FLib_MemCpy(&mgrNotif.addrInfo, &statusNotif.addrInfo, sizeof(afAddrInfo_t));
        FLib_MemCpy(&mgrNotif.addrInfo.dstAddr.aIeeeAddr, pMgrAddress, 8);
        mgrNotif.addrInfo.dstAddrMode = gZbAddrMode64Bit_c;
        mgrNotif.addrInfo.dstEndPoint = pEndpointData->managerEndpoint;
        mgrNotif.connectStatus = zcl11073Status_Disconnected;
        (void)IEEE11073ProtocolTunnel_ConnectStatusNotif(&mgrNotif);                
      }
    }
    else
      statusNotif.connectStatus = zcl11073Status_NotAuthorized;
    
  }
  (void)IEEE11073ProtocolTunnel_ConnectStatusNotif(&statusNotif);      

  return gZclSuccess_c;
}
#endif      

#if (gHcDeviceType_d == gHcDcu_c) 
zbStatus_t ZCL_11073TunnelClusterHandleConnectStatusNotif
(
  zbApsdeDataIndication_t *pIndication, 
  afDeviceDef_t *pDev
)
{
  zclFrame_t     *pFrame;
  zclCmd11073_ConnectStatus_t connectStatus;
#if !gHcDisable11073Processing
  OepFragmentedApdu_t *Apdu;            
#endif
  zclCmd11073_ConnectRequest_t req;

  /* prevent compiler warning */
  (void)pDev;
  
  pFrame = (void *)pIndication->pAsdu;
  
  /* received on mgr - just form a connect request and send it */
  connectStatus = *(zclCmd11073_ConnectStatus_t*)(pFrame + 1);

  switch (connectStatus) {
    case zcl11073Status_ReconnectRequest: 
      AF_PrepareForReply(&req.addrInfo, pIndication);
      req.params.connectControl = TRUE;
      req.params.idleTimeout = gZclInvalidIdleTimer_c;
      FLib_MemCpy(&req.params.ieeeAddress, aExtendedAddress, sizeof(req.params.ieeeAddress));
      req.params.managerEndpoint = pIndication->dstEndPoint;
      (void)IEEE11073ProtocolTunnel_ConnectRequest(&req);    
      break;
    case zcl11073Status_Disconnected: 
#if !gHcDisable11073Processing
      Apdu = OepGetTransportDisconnectedFragment(pIndication->dstEndPoint);          
      ZCL_Oep_SAPHandler(Apdu);       
#endif
      break;
    case zcl11073Status_Connected: 
      /* send the transport connected indication */
#if !gHcDisable11073Processing
      Apdu = OepGetTransportConnectedFragment(pIndication->dstEndPoint);          
      ZCL_Oep_SAPHandler(Apdu);     
#endif
      break;
  }
  return gZclSuccess_c;
}
#endif      

/*****************************************************************************
* ZCL_11073TunnelClusterServer
*
* 11073 Tunnel Server
*****************************************************************************/
#if gHcEnable11073UartTunnel
extern OepOutgoingMessage_t tunnelOutMsg;
#endif
zbStatus_t ZCL_11073TunnelClusterServer
(
zbApsdeDataIndication_t *pIndication, 
afDeviceDef_t *pDev
)
{
  zclCmd_t       command;
  zclFrame_t     *pFrame;

  /* prevent compiler warning */
  (void)pDev;
  
  pFrame = (void *)pIndication->pAsdu;
  /* handle the command */
  command = pFrame->command; 

#if gHcEnable11073UartTunnel
  tunnelOutMsg.dstAddrMode = pIndication->srcAddrMode;
  tunnelOutMsg.dstEndPoint = pIndication->srcEndPoint;
  tunnelOutMsg.srcEndPoint = pIndication->dstEndPoint;
  FLib_MemCpy(&tunnelOutMsg.aDstAddr, pIndication->aSrcAddr, sizeof(zbNwkAddr_t));   
#endif             

  switch (command) {
    case gZclCmdRx11073_TransferAPDU_c:
      return ZCL_11073TunnelClusterHandleTransferAPDU(pIndication, pDev);
    case gZclCmdRx11073_ConnectRequest_c:
      return ZCL_11073TunnelClusterHandleConnectRequest(pIndication, pDev);
    case gZclCmdRx11073_DisconnectRequest_c:
      return ZCL_11073TunnelClusterHandleDisconnectRequest(pIndication, pDev);        
    case gZclCmdRx11073_ConnectStatusNotif_c:
      return ZCL_11073TunnelClusterHandleConnectStatusNotif(pIndication, pDev);
  }
  
  return gZclUnsupportedClusterCommand_c;
}


/*****************************************************************************
* IEEE11073ProtocolTunnel_TransferApdu
*
* Send a 11073 APDU
*****************************************************************************/
zbStatus_t IEEE11073ProtocolTunnel_TransferApdu
(
    zclCmd11073_TransferApdu_t *pReq
)
{
  uint16_t iPayloadLen;
  uint8_t  len;
#if (gHcDeviceType_d != gHcDcu_c)      
  bool_t connected = FALSE;
  uint8_t connectedLen; 
  (void)ZCL_GetAttribute(pReq->addrInfo.srcEndPoint, ClusterId, gZclAttr11703ProtocTunnelConnected_c, 
                            &connected, &connectedLen);    
  if (!connected)
      return gZclFailure_c;
  
  ZCL_11073ResetIdleTimer(AF_GetEndPointDevice(pReq->addrInfo.srcEndPoint), FALSE);  
#endif             
  
  iPayloadLen = OTA2Native16(pReq->apdu.apduLen);  
  iPayloadLen += 2;
  
  len = (uint8_t) iPayloadLen;
    
  return  ZCL_SimpleRequest(&(pReq->addrInfo), gZclCmdRx11073_TransferAPDU_c, 
                            gZclFrameControl_FrameTypeSpecific | gZclFrameControl_DisableDefaultRsp,
                            len, (uint8_t*)&(pReq->apdu));
}

/*****************************************************************************
* IEEE11073ProtocolTunnel_ConnectRequest
*
* Sends a 11073 tunnel connect request
*****************************************************************************/

zbStatus_t IEEE11073ProtocolTunnel_ConnectRequest (zclCmd11073_ConnectRequest_t *pReq) 
{
  return  ZCL_SimpleRequest(&(pReq->addrInfo), gZclCmdRx11073_ConnectRequest_c, 
                            gZclFrameControl_FrameTypeSpecific | gZclFrameControl_DisableDefaultRsp, 
                            sizeof(zclCmd11073_ConnectRequestParams_t), (uint8_t*)&(pReq->params));  
}


/*****************************************************************************
* IEEE11073ProtocolTunnel_InternalDisconnectReq
*
* Sends a 11073 tunnel disconnect request, including updating the state of the
* local 11073 layer
*****************************************************************************/
zbStatus_t IEEE11073ProtocolTunnel_InternalDisconnectReq(zclCmd11073_ConnectStatusNotif_t* statusNotif) {
  zclCmd11073_ConnectStatusNotif_t mgrNotif;
  OepFragmentedApdu_t *FragmentedpApdu;            
  afDeviceDef_t *pDeviceDef;
  zclGenericAppAttrRAM_t* pEndpointData;
  
  statusNotif->connectStatus = zcl11073Status_Disconnected; 
  
  pDeviceDef = AF_GetEndPointDevice(statusNotif->addrInfo.srcEndPoint);
  pEndpointData = (zclGenericAppAttrRAM_t*)pDeviceDef->pData;
  
     
  FragmentedpApdu = OepGetTransportDisconnectedFragment(statusNotif->addrInfo.srcEndPoint);          
  ZCL_Oep_SAPHandler(FragmentedpApdu); 
  
  pEndpointData->connected = FALSE;

  ZCL_11073ResetIdleTimer(AF_GetEndPointDevice(statusNotif->addrInfo.srcEndPoint), TRUE);     
        
  FLib_MemCpy(&mgrNotif.addrInfo, &statusNotif->addrInfo, sizeof(afAddrInfo_t));
  Copy8Bytes(mgrNotif.addrInfo.dstAddr.aIeeeAddr, pEndpointData->managerTarget);
  mgrNotif.addrInfo.dstAddrMode = gZbAddrMode64Bit_c;
  mgrNotif.addrInfo.dstEndPoint = pEndpointData->managerEndpoint;
  mgrNotif.connectStatus = zcl11073Status_Disconnected;
  return IEEE11073ProtocolTunnel_ConnectStatusNotif(&mgrNotif);                
}




/*****************************************************************************
* IEEE11073ProtocolTunnel_DisconnectRequest
*
* Sends a 11073 tunnel disconnect request, without updating the state of the
* local 11073 layer
*****************************************************************************/
zbStatus_t IEEE11073ProtocolTunnel_DisconnectRequest (zclCmd11073_DisconnectRequest_t *pReq) 
{
  return  ZCL_SimpleRequest(&(pReq->addrInfo), gZclCmdRx11073_DisconnectRequest_c, 
                            gZclFrameControl_FrameTypeSpecific | gZclFrameControl_DisableDefaultRsp, 
                            sizeof(pReq->ieeeAddress), (uint8_t*)&(pReq->ieeeAddress));  
}


/*****************************************************************************
* IEEE11073ProtocolTunnel_ConnectStatusNotif
*
* Sends a 11073 status notification message disconnect request
*****************************************************************************/
zbStatus_t IEEE11073ProtocolTunnel_ConnectStatusNotif (zclCmd11073_ConnectStatusNotif_t *pReq)
{
  return  ZCL_SimpleRequest(&(pReq->addrInfo), gZclCmdRx11073_ConnectStatusNotif_c, 
                            gZclFrameControl_FrameTypeSpecific | gZclFrameControl_DisableDefaultRsp, 
                            sizeof(zclCmd11073_ConnectStatus_t), (uint8_t*)&(pReq->connectStatus));  
}


/*****************************************************************************
* IEEE11073ProtocolTunnel_TransferApduPtr
*
* Called by the IEEE 11073 layer to transfer an APDU through the tunnel
* verifies if the length is larger than the partition threshold length and uses
* the partition cluster to send the frame in that case or calls the internal
* direct function to send an unpartitioned frame
*****************************************************************************/

zbStatus_t IEEE11073ProtocolTunnel_TransferApduPtr
(
    zclCmd11073_TransferApduPtr_t *pReq
) 
{

#if gZclEnablePartition_d  
  uint16_t reqPayloadLen;
  /* add zcl header len and apdu length field to payloadlen */
  reqPayloadLen = OTA2Native16(pReq->apdu->apduLen) + sizeof(zclFrame_t) + sizeof(uint16_t);

  if (reqPayloadLen > g11073TunnelClusterPartitionLenThreshold) {
    zclPartitionedClusterFrameInfo_t partitionTxFrameInfo;
    zclFrame_t* zclFrame = (zclFrame_t*)gPendingPartitionRxFrame.fragmentedMsg;

    zclFrame->frameControl = gZclFrameControl_FrameTypeSpecific | gZclFrameControl_DisableDefaultRsp; 
    zclFrame->transactionId = gZclTransactionId++;
    zclFrame->command = gZclCmdRx11073_TransferAPDU_c;    
     
    FLib_MemCpy( gPendingPartitionRxFrame.fragmentedMsg + sizeof(zclFrame_t), (uint8_t*)pReq->apdu, (uint8_t)reqPayloadLen);
    gPendingPartitionRxFrame.totalLen = reqPayloadLen;
    
    partitionTxFrameInfo.zclTransactionId = gpPendingPartitionTxFrame->payload.zclHeader.transactionId;
    FLib_MemCpy(&partitionTxFrameInfo.addrInfo, &pReq->addrInfo, sizeof(pReq->addrInfo));
    partitionTxFrameInfo.addrInfo.txOptions &= ~gApsTxOptionFragmentationRequested_c;
    
    partitionTxFrameInfo.frameCount = reqPayloadLen / gDefaultPartitionAttrs.partitionedFrameSize; 
    if (reqPayloadLen % gDefaultPartitionAttrs.partitionedFrameSize != 0)
      partitionTxFrameInfo.frameCount++;
    
    partitionTxFrameInfo.frameCount = Native2OTA16(partitionTxFrameInfo.frameCount);
    FLib_MemCpy((void *)&partitionTxFrameInfo.partitionAttrs, (void *)&gDefaultPartitionAttrs, sizeof(zclPartitionAttrs_t));
    partitionTxFrameInfo.partitionAttrs.partitionedFrameSize = gDefaultPartitionAttrs.partitionedFrameSize;
    (void)ZCL_PartitionRegisterTxFrame(&partitionTxFrameInfo);
  } 
  else {
#endif  
    return IEEE11073ProtocolTunnel_TransferApduPtrInternal(pReq);
    /* send the frame using the normal ZCL-APS (possibly fragmented) */
#if gZclEnablePartition_d      
  }
  
  return gZbSuccess_c;
#endif    
    
}

/*****************************************************************************
* IEEE11073ProtocolTunnel_SetPartitionThreshold
*
* Allows changing the partition threshold dynamically
*****************************************************************************/
#if gZclEnablePartition_d  
zbStatus_t IEEE11073ProtocolTunnel_SetPartitionThreshold(uint8_t threshold) 
{
    g11073TunnelClusterPartitionLenThreshold = threshold;
    return gZbSuccess_c;
}
#endif

zbStatus_t IEEE11073ProtocolTunnel_GetIEEE11073MessageProcStatus()
{
    return !gHcDisable11073Processing;
}

#endif
#endif
