/******************************************************************************
* ZclPartition.c
*
* This module contains code that handles ZigBee Cluster Library commands and
* attributes for the ZCL Partition cluster
*
* Copyright (c) 2010 Freescale, Inc.  All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
* Documents used in this specification:
* [R1] - 075307r07ZB_TA_PTG-Telecom_Application_Profile.pdf
* [R2] - 075123r00ZB_AFG-ZigBee_Cluster_Library_Specification.pdf
*******************************************************************************/

#include "EmbeddedTypes.h"
#include "zigbee.h"
#include "FunctionLib.h"
#include "BeeStackConfiguration.h"
#include "BeeStack_Globals.h"
#include "AppAfInterface.h"
#include "AfApsInterface.h"
#include "BeeStackInterface.h"

#include "zcl.h"
#include "ZclPartition.h"
#include "ZclFoundation.h"
#include "HcProfile.h"


#include "ZigBee.h"
#include "FunctionLib.h"

#if gZclEnablePartition_d
/******************************************************************************
*******************************************************************************
* Private macros
*******************************************************************************
******************************************************************************/

#define gInvalidTableIndex_c 0xFF


/******************************************************************************
*******************************************************************************
* Private prototypes
*******************************************************************************
******************************************************************************/

index_t ZCL_PartitionFindFrameIdx
(
  zclPartitionedClusterFrame_t* frameTable,
  zbClusterId_t clusterId, 
  uint8_t transactionId 
);


index_t ZCL_PartitionFindFrameIdxForTimer
(
  zclPartitionedClusterFrame_t* frameTable,
  tmrTimerID_t timerId,
  uint8_t timerOffset
);

index_t ZCL_PartitionFindClusterIdx
(
  zbClusterId_t clusterId
);

index_t ZCL_PartitionAllocFrame
(
  zclPartitionedClusterFrame_t* frameTable
);


zclPartitionStatus_t ZCL_PartitionTransferPartitionedFrameReq
(
  afAddrInfo_t* pAddrInfo, 
  uint8_t transactionId,
  uint8_t pReqLen,
  zclPartitionCmdTransferPartitionedFrame_t* pReq
);


bool_t ZCL_PartitionTryMultipleAckReq
(
  index_t entryIdx,
  bool_t sendNacks
);

void ZCL_PartitionReceiverTimeoutExpired
(
  tmrTimerID_t timerId
);

void ZCL_PartitionNackTimeoutExpired
(
  tmrTimerID_t timerId
);

void ZCL_PartitionHandleTransferPartitionedFrame
(
  zbApsdeDataIndication_t *pIndication, 
  zclFrame_t *pFrame,
  uint8_t asduLen
);

void ZCL_PartitionTxTimerHandler
(
  tmrTimerID_t timerId
);

void ZCL_PartitionCancelRxFrame
(
  uint8_t frameIdx
);

void ZCL_PartitionCancelTxFrame
(
  uint8_t frameIdx
);

void ZCL_PartitionSenderTimeoutExpired
(
  tmrTimerID_t timer
);

void ZCL_PartitionHandleMultipleAckFrame
(
  zclFrame_t *pFrame, 
  uint8_t asduLength
);

zclPartitionStatus_t ZCL_HandleWriteHandshakeParamReq
(
  zbApsdeDataIndication_t *pIndication 
);


/******************************************************************************
*******************************************************************************
* Private type definitions
*******************************************************************************
******************************************************************************/

static const zbClusterId_t gaPartitionClusterId_c = { gaZclClusterPartition_c };



/******************************************************************************
*******************************************************************************
* Public memory definitions
*******************************************************************************
******************************************************************************/


const zclAttrDef_t gaPartitionClusterAttrDef[] = {  
  {gZclAttrPartitionMaxIncomingTransferSize_c,  gZclDataTypeUint16_c,    gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(int16_t), (void *)MbrOfs(zclPartitionAttrs_t,maxInTrsSize)},
  {gZclAttrPartitionMaxOutgoingTransferSize_c,  gZclDataTypeUint16_c,    gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(int16_t), (void *)MbrOfs(zclPartitionAttrs_t,maxOutTrsSize)},
  {gZclAttrPartitionPartitionedFrameSize_c,  gZclDataTypeUint8_c,    gZclAttrFlagsInRAM_c, sizeof(int8_t), (void *)MbrOfs(zclPartitionAttrs_t,partitionedFrameSize)},
  {gZclAttrPartitionLargeFrameSize_c,  gZclDataTypeUint16_c,    gZclAttrFlagsInRAM_c, sizeof(int16_t), (void *)MbrOfs(zclPartitionAttrs_t,largeFrameSize)},
  {gZclAttrPartitionNumberOfACKFrame_c,  gZclDataTypeUint8_c,    gZclAttrFlagsInRAM_c, sizeof(int8_t), (void *)MbrOfs(zclPartitionAttrs_t,noOfAckFrame)},
  {gZclAttrPartitionNACKTimeout_c,  gZclDataTypeUint16_c,    gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(int16_t), (void *)MbrOfs(zclPartitionAttrs_t,nackTimeout)},
  {gZclAttrPartitionInterframeDelay_c,  gZclDataTypeUint8_c,    gZclAttrFlagsInRAM_c, sizeof(int8_t), (void *)MbrOfs(zclPartitionAttrs_t,interframeDelay)},
  {gZclAttrPartitionNumberOfSendRetries_c,  gZclDataTypeUint8_c,    gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(int8_t), (void *)MbrOfs(zclPartitionAttrs_t,noOfSendRetries)},
  {gZclAttrPartitionSenderTimeout_c,  gZclDataTypeUint16_c,    gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(int16_t), (void *)MbrOfs(zclPartitionAttrs_t,senderTimeout)},
  {gZclAttrPartitionReceiverTimeout_c,  gZclDataTypeUint16_c,    gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(int16_t), (void *)MbrOfs(zclPartitionAttrs_t,receiverTimeout)}  
 
};

const zclAttrDefList_t gPartitionClusterAttrDefList = {
  NumberOfElements(gaPartitionClusterAttrDef),
  gaPartitionClusterAttrDef
};

#if gBigEndian_c
zclPartitionAttrs_t gDefaultPartitionAttrs = {
  0x0005, /* maxInTrsSize */
  0x0005, /* maxOutTrsSize */
  0x08,   /* partitionedFrameSize */
  0x0005, /* largeFrameSize */
  0x04,   /* noOfAckFrames */
  0x000A, /* nackTimeout */
  0xFA,   /* interFrameDelay */
  0x03,   /* noOfSendRetries */
  0x000F, /* senderTimeout */
  0x000F  /* receiverTimeout */
};
#else
zclPartitionAttrs_t gDefaultPartitionAttrs = {
  0x0500, /* maxInTrsSize */
  0x0500, /* maxOutTrsSize */
  0x08,   /* partitionedFrameSize */
  0x0500, /* largeFrameSize */
  0x04,   /* noOfAckFrames */
  0x0A00, /* nackTimeout */
  0xFA,   /* interFrameDelay */
  0x03,   /* noOfSendRetries */
  0x0F00, /* senderTimeout */
  0x0F00  /* receiverTimeout */
};
#endif


zclPartitionedClusterFrame_t gZclPartitionTxFrameTable[gZclPartitionMaxRegisteredFrames_c];
zclPartitionedClusterFrame_t gZclPartitionRxFrameTable[gZclPartitionMaxRegisteredFrames_c];
zclPartitionedClusterInfoRecord_t gZclPartitionClusterTable[gZclPartitionMaxRegisteredClusters];


zclPartitionedClusterFrameInfo_t gPendingTxFrame;


/******************************************************************************
*******************************************************************************
* Private functions
*******************************************************************************
******************************************************************************/


/******************************************************************************
*  ZCL_PartitionSendDataReq
*  
*  Internal callback that sets up Transfered Partition Frame headers and
*  calls partitioned cluster to provide fragment data
******************************************************************************/

void ZCL_PartitionSendDataReq
(
  zclPartitionAppEvent_t *pPartitionEvent, 
  pfnPartitionedClusterDataReq_t callback
)
{
  uint8_t reqLen;
  uint8_t idLen = 1;
  zclPartitionCmdTransferPartitionedFrame_t* pReq;
  
  switch(pPartitionEvent->eventType) {
    case gZclPartitionAppEventType_DataRequest:
      reqLen = pPartitionEvent->frameInfo->partitionAttrs.partitionedFrameSize;
      pReq = AF_MsgAlloc();
      if (pReq != NULL) {
        if (pPartitionEvent->partitionedFrameId <= 0xFF) {
          pReq->fragOptions = gZclPartitionFragOptions_PartitionIdLen8bit;
          pReq->partitionId[0] = (uint8_t)pPartitionEvent->partitionedFrameId;        
        }
        else {
          idLen = 2;
          pReq->fragOptions = gZclPartitionFragOptions_PartitionIdLen16bit;
#if ( gBigEndian_c != TRUE)
          {
            //ARM7 memory aligment handling
            volatile uint8_t* pTemp8 = (uint8_t*)(&pPartitionEvent->partitionedFrameId);
            pReq->partitionId[0] = *pTemp8++;
            pReq->partitionId[1] = *pTemp8;
          }
#else
        *(uint16_t*)pReq->partitionId = Native2OTA16(pPartitionEvent->partitionedFrameId);      
#endif
        }
        
        if (pPartitionEvent->partitionedFrameId == 0x0000) {
          uint16_t frameCount = OTA2Native16(pPartitionEvent->frameInfo->frameCount);
          if (frameCount <= 0xFF) {
            idLen = 1;
            pReq->fragOptions = gZclPartitionFragOptions_FirstBlock | gZclPartitionFragOptions_PartitionIdLen8bit;
            pReq->partitionId[0] = (uint8_t)frameCount;        
          }
          else {
            idLen = 2;
            pReq->fragOptions = gZclPartitionFragOptions_FirstBlock | gZclPartitionFragOptions_PartitionIdLen16bit;
  #if ( gBigEndian_c != TRUE)
            {
              //ARM7 memory aligment handling
              volatile uint8_t* pTemp8 = (uint8_t*)(&frameCount);
              pReq->partitionId[0] = *pTemp8++;
              pReq->partitionId[1] = *pTemp8;
            }
  #else
          *(uint16_t*)pReq->partitionId = pPartitionEvent->frameInfo->frameCount;  
  #endif
          }
        }

        {
          uint8_t dataLen = 0;
          uint8_t* buffer;
          FLib_MemSet((uint8_t*)pReq->partitionId + idLen + 1, 0x00, reqLen); 
          buffer = callback(pPartitionEvent, &dataLen);          
          if (buffer != NULL) {
            FLib_MemCpy((uint8_t*)pReq->partitionId + idLen + 1, buffer, dataLen);
            MSG_Free(buffer);
          }
          
          *((uint8_t*)pReq->partitionId + idLen) = dataLen;
            
        }
        (void)ZCL_PartitionTransferPartitionedFrameReq(&pPartitionEvent->frameInfo->addrInfo,
                                               pPartitionEvent->zclTransactionId, reqLen + idLen +
                                               MbrSizeof(zclPartitionCmdTransferPartitionedFrame_t, fragOptions) + 1, 
                                               pReq);
      }
      break;
  }
}




/******************************************************************************
* ZCL_PartitionFindFrameIdx
*
* Returns a partition frame table index based on clusterId and transactionId
******************************************************************************/

index_t ZCL_PartitionFindFrameIdx
(
  zclPartitionedClusterFrame_t frameTable[],
  zbClusterId_t clusterId, 
  uint8_t transactionId
) 
{
  index_t i;
  zclPartitionedClusterFrame_t* pFrameEntry;

  /* now look for frame entry and search for free place in entry table at the same time*/
  for (i = 0; i < gZclPartitionMaxRegisteredFrames_c; i++) {
  
    /* get registration information for this frame table entry */
    pFrameEntry = &frameTable[i];
    
    /* compare clusterId and transactionId to see if this entry matches the req */
    if (!pFrameEntry->isFree && pFrameEntry->frame.zclTransactionId == transactionId &&
         (clusterId == NULL || IsEqual2Bytes(pFrameEntry->frame.addrInfo.aClusterId, clusterId))) 
      return i;
  }
  return gInvalidTableIndex_c;
}


/******************************************************************************
*  ZCL_PartitionFindFrameIdxForTimer
*
*  Returns a partition frame table index based on timer (called from timer callback)
******************************************************************************/

index_t ZCL_PartitionFindFrameIdxForTimer
(
  zclPartitionedClusterFrame_t* frameTable,
  tmrTimerID_t timerId,
  uint8_t timerOffset
) 
{
  index_t i;
  zclPartitionedClusterFrame_t* pFrameEntry;

  /* look for frame entry */
  for (i = 0; i < gZclPartitionMaxRegisteredFrames_c; i++) {
  
    /* get registration information for this frame table entry */
    pFrameEntry = &frameTable[i];
    
    /* compare clusterId and transactionId to see if this entry matches the req */
    if (!pFrameEntry->isFree && 
        timerId == *(tmrTimerID_t *)((uint8_t *)&pFrameEntry->state + timerOffset))
      return i;
  }
  return gInvalidTableIndex_c;
}


/******************************************************************************
* ZCL_PartitionFindClusterIdx
*
* Returns a partition frame table index based on clusterId
******************************************************************************/

index_t ZCL_PartitionFindClusterIdx
(
  zbClusterId_t clusterId
) 
{
  
  index_t i;
  
  /* check if cluster is registered with the partition server */
  for (i = 0; i < NumberOfElements(gZclPartitionClusterTable); i++) {
  
    /* get registration information for this cluster table entry */
    zclPartitionedClusterInfoRecord_t* pClusterInfo = 
                      (zclPartitionedClusterInfoRecord_t*)&gZclPartitionClusterTable[i];
    
    /* compare with cluster id in request and set the cluster entry index value*/
    if (IsEqual2Bytes(pClusterInfo->info.clusterId, clusterId)) {
      return i;
    }
  }  
  
  return gInvalidTableIndex_c;
}


/******************************************************************************
*  ZCL_PartitionAllocFrame
*
*  Checks if the partition frame table has space to store a new frame and 
*  sets the isFree flag of the entry to false.
******************************************************************************/

index_t ZCL_PartitionAllocFrame
(
  zclPartitionedClusterFrame_t frameTable[]
) 
{
  index_t i;
  /* look for free frame entry */
  
  for (i = 0; i < gZclPartitionMaxRegisteredFrames_c; i++)
    if (frameTable[i].isFree) {
      frameTable[i].isFree = FALSE;
      return i;
    }
  
  return gInvalidTableIndex_c;
}


/******************************************************************************
* ZCL_PartitionTryMultipleAckReq
*
* Called after receiving a partition fragment to check whether a MultipleAck
* should be sent to the transmitter.
******************************************************************************/

bool_t ZCL_PartitionTryMultipleAckReq
(
  index_t entryIdx,
  bool_t sendNacks
) 
{
  zclPartitionedClusterFrame_t* pFrameEntry;
  zclPartitionRxFrameState_t* pRxFrameState;
  zclPartitionAttrs_t* pRxFrameAttrs;
  zclPartitionCmdMultipleACK16bit_t* pMultipleAckReq;
  uint8_t* pNackIdList;
  bool_t blockComplete = TRUE;
  afToApsdeMessage_t *pMsg;
  uint8_t framesToCheck;
  uint8_t iPayloadLen = 0;
  index_t i;
  bool_t finishedFrame = FALSE;

  pFrameEntry = (zclPartitionedClusterFrame_t*)&gZclPartitionRxFrameTable[entryIdx];
  pRxFrameState = &pFrameEntry->state.Rx;
  pRxFrameAttrs = &pFrameEntry->frame.partitionAttrs; 
  
  pMsg = ZCL_CreateFrame(&(pFrameEntry->frame.addrInfo),gZclPartitionCmdTx_MultipleACKs,
          gZclFrameControl_FrameTypeSpecific | gZclFrameControl_DirectionRsp | gZclFrameControl_DisableDefaultRsp,
          &pFrameEntry->frame.zclTransactionId, &iPayloadLen, NULL);

  if(!pMsg)
    /* ZHCTODO cleanup here */
    return finishedFrame;

  iPayloadLen = sizeof(zclFrame_t) + MbrOfs(zclPartitionCmdMultipleACK16bit_t, nackId);
  pMultipleAckReq = (void *)(&((uint8_t *)pMsg)[ApsmeGetAsduOffset() +  sizeof(zclFrame_t)]);
  
  pMultipleAckReq->ackOptions = gZclPartitionAckOptions_IdLen16bit;
  Set2Bytes(&pMultipleAckReq->firstFrameId, Native2OTA16(pRxFrameState->currentStartFrame));
  pNackIdList = (uint8_t*)&pMultipleAckReq->nackId;
  
  framesToCheck = OTA2Native16(pFrameEntry->frame.frameCount) - pRxFrameState->currentStartFrame;
  
  if (framesToCheck > pRxFrameAttrs->noOfAckFrame)
    framesToCheck =  pRxFrameAttrs->noOfAckFrame;

  for (i = 0; i < framesToCheck; i++) {
    uint8_t idx = i / 8;
    uint8_t mask = 1;
    mask <<= i % 8;
    if ((pRxFrameState->pAckList->ackTable[idx] & mask) == 0) {
      blockComplete = FALSE;
      if (sendNacks) {
         iPayloadLen += MbrSizeof(zclPartitionCmdMultipleACK16bit_t, firstFrameId);
         Set2Bytes(pNackIdList, Native2OTA16(pRxFrameState->currentStartFrame + i));
         pNackIdList+=sizeof(uint16_t);
      } 
      else
        break;
    }
  } 

    
  if (!sendNacks && blockComplete) {
    pRxFrameState->currentStartFrame += pRxFrameAttrs->noOfAckFrame;
    FLib_MemSet(pRxFrameState->pAckList, 0x00, sizeof(zclPartitionBitwiseAckTable_t));  
    if (pRxFrameState->currentStartFrame >= OTA2Native16(pFrameEntry->frame.frameCount)) {
      /* ask partitioned cluster to send data as we cannot store it on this platform*/

        finishedFrame = TRUE;
    }
  }
  
  if (blockComplete || sendNacks) {
    afAddrInfo_t addrInfo;
    FLib_MemCpy(&addrInfo, &(pFrameEntry->frame.addrInfo), sizeof(afAddrInfo_t));
    /* tunnel the cluster */
    Copy2Bytes(addrInfo.aClusterId, gaPartitionClusterId_c); 
    /* send packet over the air */
    
    /* uncomment on receiver for test 5.9.7 */
    /*
     MSG_Free(pMsg);
     return; 
     */
    (void)ZCL_DataRequestNoCopy(&addrInfo, iPayloadLen, pMsg);
    

    /* restart nack Timer */
    if (!sendNacks)
      TMR_StartSingleShotTimer(pRxFrameState->nackTimer, OTA2Native16(pRxFrameAttrs->nackTimeout), 
                             ZCL_PartitionNackTimeoutExpired);
  }
  else
    MSG_Free(pMsg);
  
 if (finishedFrame) {
  
    pfnPartitionedClusterDataReq_t dataReqFunc = NULL;
    zclPartitionAppEvent_t appEventParams; 
    
    i = ZCL_PartitionFindClusterIdx(pFrameEntry->frame.addrInfo.aClusterId);
    if (i != gInvalidTableIndex_c)
      dataReqFunc = gZclPartitionClusterTable[i].info.dataReqHandler;    
    
    appEventParams.eventType = gZclPartitionAppEventType_FrameComplete;
    appEventParams.zclTransactionId = pFrameEntry->frame.zclTransactionId;
    
    (void)dataReqFunc(&appEventParams, NULL);
    ZCL_PartitionCancelRxFrame(entryIdx);
 }
  return finishedFrame;
}



/******************************************************************************
* ZCL_PartitionReceiverTimeoutExpired
*
* Called when the partition receiver timeout expires
******************************************************************************/

void ZCL_PartitionReceiverTimeoutExpired
(
  tmrTimerID_t timerId
) 
{
  index_t entryIdx = ZCL_PartitionFindFrameIdxForTimer(gZclPartitionRxFrameTable, timerId, 
                                    MbrOfs(zclPartitionRxFrameState_t, receiverTimer));  
  if (entryIdx !=  gInvalidTableIndex_c)
    ZCL_PartitionCancelRxFrame(entryIdx);
}


/******************************************************************************
* ZCL_PartitionNackTimeoutExpired
*
* Called when the NACK timeout expires on the receiver
******************************************************************************/

void ZCL_PartitionNackTimeoutExpired
(
  tmrTimerID_t timerId
) 
{
  index_t entryIdx = ZCL_PartitionFindFrameIdxForTimer(gZclPartitionRxFrameTable, timerId, 
                                    MbrOfs(zclPartitionRxFrameState_t, nackTimer));  
  
  if (entryIdx !=  gInvalidTableIndex_c) 
    /* Nack Timeout has expired -> send back MultipleACK with NACKIds */
    ZCL_PartitionTryMultipleAckReq(entryIdx, TRUE);
}


/******************************************************************************
* ZCL_PartitionHandleTransferPartitionedFrame
*
* Called when a partition fragment is received
******************************************************************************/

void ZCL_PartitionHandleTransferPartitionedFrame
(
  zbApsdeDataIndication_t *pInd, 
  zclFrame_t *pFrame,
  uint8_t asduLen
) 
{
  zclPartitionCmdTransferPartitionedFrame_t* pPartitionedFrame;  
  zclPartitionedClusterFrame_t* pFrameEntry;
  zclPartitionRxFrameState_t* pRxFrameState;
  pfnPartitionedClusterDataInd_t pfDataIndication;
  uint8_t isPartitionId16bit = 0;
  zclPartitionedDataIndication_t* pIndication;
  index_t clusterIdx;
  uint16_t partitionId;
  index_t entryIdx, frameOffset;
  bool_t finished = FALSE;

  /* compute actual data length; last added term is for first byte of PartitionId*/
  asduLen -= sizeof(zclFrame_t) + sizeof(zclPartitionFragOptions_t) + 1;
  
  /* find the RX frame for which this was sent (based on transaction Id) */ 
  entryIdx = ZCL_PartitionFindFrameIdx(gZclPartitionRxFrameTable, NULL, pFrame->transactionId);
  
  if (entryIdx != gInvalidTableIndex_c) {
  
    pPartitionedFrame = (zclPartitionCmdTransferPartitionedFrame_t*)(pFrame + 1);
    
    pFrameEntry = &gZclPartitionRxFrameTable[entryIdx];
    pRxFrameState = &pFrameEntry->state.Rx;
      
    clusterIdx = ZCL_PartitionFindClusterIdx(pFrameEntry->frame.addrInfo.aClusterId);
  
    if (clusterIdx != gInvalidTableIndex_c) {
    
      pfDataIndication = gZclPartitionClusterTable[clusterIdx].info.dataIndHandler;
      /* check PartitionId Len in fragmentation options */
      if (pPartitionedFrame->fragOptions & gZclPartitionFragOptions_PartitionIdLen16bit)
      {
        isPartitionId16bit = 1;
  #if ( gBigEndian_c != TRUE)
        {
          //ARM7 memory aligment handling
          volatile uint8_t* pSrc8 = (uint8_t*)(&pPartitionedFrame->partitionId);
          volatile uint8_t* pDst8 = (uint8_t*)(&partitionId);
          pDst8[0] = *pSrc8++;
          pDst8[1] = *pSrc8;
        }
  #else
        partitionId = *(uint16_t*)pPartitionedFrame->partitionId;         
        partitionId = OTA2Native16(partitionId);          
  #endif        
        asduLen--;      
      }
      else {
        isPartitionId16bit = 0;
        partitionId = *(uint8_t*)&pPartitionedFrame->partitionId;
      }

      if (pPartitionedFrame->fragOptions & gZclPartitionFragOptions_FirstBlock) {
        pFrameEntry->frame.frameCount = Native2OTA16(partitionId);
        partitionId = 0;
      }
      
     
      /* send the frame data to the partitioned cluster */
      pIndication = AF_MsgAlloc();
      if (pIndication != NULL) {
        pIndication->zclTransactionId = pFrame->transactionId;
        pIndication->partitionId = partitionId;
        pIndication->partitionLength = pFrameEntry->frame.partitionAttrs.partitionedFrameSize;          
        Copy2Bytes(pIndication->srcAddr, pInd->aSrcAddr);
        pIndication->dstEndPoint = pInd->dstEndPoint;
        pIndication->srcEndPoint = pInd->srcEndPoint;
        FLib_MemCpy(pIndication->partitionedFrame, (uint8_t *)&pPartitionedFrame->partitionId + 
                    sizeof(uint8_t) + isPartitionId16bit, asduLen);
        pIndication->dataLength = (uint8_t)pIndication->partitionedFrame[0];                    
        if (pIndication->partitionLength * OTA2Native16(pFrameEntry->frame.frameCount)
            <=  gZclPartitionClusterTable[clusterIdx].info.clusterBufferSize)
            pfDataIndication(pIndication);
        MSG_Free(pIndication);
      }
      
      frameOffset = partitionId - pRxFrameState->currentStartFrame;
      
      pRxFrameState->pAckList->ackTable[frameOffset / 8] |= (1 << (frameOffset % 8));
      
      finished = ZCL_PartitionTryMultipleAckReq(entryIdx, FALSE);
    }
    
    if (!finished) {
      
    /* restart receiver timer */
    TMR_StartSingleShotTimer(pRxFrameState->receiverTimer, 
                             OTA2Native16(pFrameEntry->frame.partitionAttrs.receiverTimeout), 
                             ZCL_PartitionReceiverTimeoutExpired);
    
    /* if this is the first incoming frame start the nackTimer */
    if (partitionId == 0)
      TMR_StartSingleShotTimer(pRxFrameState->nackTimer, 
                               OTA2Native16(pFrameEntry->frame.partitionAttrs.nackTimeout),
                               ZCL_PartitionNackTimeoutExpired);
    }
  }
}


/******************************************************************************
* ZCL_PartitionTxTimerHandler
*
* Called when the interframe delay timer expires and the transmitter is ready
* to send a new fragment
******************************************************************************/

void ZCL_PartitionTxTimerHandler
(
  tmrTimerID_t timerId
) 
{
  zclPartitionedClusterFrame_t* pFrameEntry;
  zclPartitionTxFrameState_t* pTxFrameState;
  zclPartitionAttrs_t* pTxFrameAttrs;
  zclPartitionNACKIdTable_t* pNackTable;
  index_t i, entryIdx;
  pfnPartitionedClusterDataReq_t dataReqFunc = NULL;
  bool_t restartTimer = FALSE;
  uint16_t dataReqIndex;
  
  /* find the TX frame for which this was sent (based on timer) */   
  entryIdx = ZCL_PartitionFindFrameIdxForTimer(gZclPartitionTxFrameTable, timerId, 
                                    MbrOfs(zclPartitionTxFrameState_t, txTimer));

  if (entryIdx != gInvalidTableIndex_c) {
    
    pFrameEntry = &gZclPartitionTxFrameTable[entryIdx];
    pTxFrameState = &pFrameEntry->state.Tx;
    pTxFrameAttrs = &pFrameEntry->frame.partitionAttrs;
    pNackTable = pTxFrameState->pListResend;
    
    /* do a data request to the partitioned cluster for the current frame */
    /* first look for the cluster in the cluster registration table */

    /* check if cluster is registered with the partition server */
    i = ZCL_PartitionFindClusterIdx(pFrameEntry->frame.addrInfo.aClusterId);
    if (i != gInvalidTableIndex_c)
      dataReqFunc = gZclPartitionClusterTable[i].info.dataReqHandler;
    
    if (dataReqFunc != NULL) {
      zclPartitionAppEvent_t appEventParams;
      
      if (!pTxFrameState->sendingNacks)
        dataReqIndex =  pTxFrameState->currentFrameIndex;
      else {
        /* when sending NACKs we must look in the buffer from MultipleACKs */
        if (pNackTable->sizeofNackId == 2)
          dataReqIndex = OTA2Native16(((uint16_t*)(&pNackTable->nackIds))[pTxFrameState->currentFrameIndex]);
        else
          dataReqIndex = ((uint8_t*)(&pNackTable->nackIds))[pTxFrameState->currentFrameIndex];
      }
      
      appEventParams.eventType = gZclPartitionAppEventType_DataRequest;
      appEventParams.partitionedFrameId = dataReqIndex;
      appEventParams.zclTransactionId = pFrameEntry->frame.zclTransactionId;
      appEventParams.frameInfo = &pFrameEntry->frame;
      
      /* ask partitioned cluster to send data as we cannot store it on this platform*/
      ZCL_PartitionSendDataReq(&appEventParams, dataReqFunc);      
      pTxFrameState->currentFrameIndex++;              
    }
    
    /* decide whether to send another fragment or not */
    
    /* sending a block (and not NACKs)... */
    if (!pTxFrameState->sendingNacks) {
        
      /* other fragments to send? (globally); if not, we'll cleanup in multipleACKs handler */
      if (pTxFrameState->currentFrameIndex < OTA2Native16(pFrameEntry->frame.frameCount)) {
        
        /* last fragment in this block? */
        if (pTxFrameState->currentFrameIndex - pTxFrameState->currentStartFrame 
              < pTxFrameAttrs->noOfAckFrame){

          /* re-arm timer to send next fragment */
          restartTimer = TRUE;
        } 
      }
    } 
    else { /* sending a NACK fragment... */
    
      /* other Nacks to send? */
      if (pTxFrameState->currentFrameIndex < pNackTable->nackIdCount) 
        restartTimer = TRUE;
      
      /* else the multipleAcks or sendTimeout will cleanup */
    }
    
    /* restart sender timer */
    TMR_StartSingleShotTimer(pTxFrameState->senderTimer, 
                             OTA2Native16(pFrameEntry->frame.partitionAttrs.senderTimeout), 
                             ZCL_PartitionSenderTimeoutExpired);
                             
    if (restartTimer)
      TMR_StartSingleShotTimer(timerId, pTxFrameAttrs->interframeDelay, ZCL_PartitionTxTimerHandler);
  }
}


/******************************************************************************
* ZCL_PartitionCancelRxFrame
*
* Cleans up information and timers for a received frame for which transmission
* was finalized or that expired
******************************************************************************/

void ZCL_PartitionCancelRxFrame
(
  uint8_t frameIdx
) 
{

  TMR_FreeTimer(gZclPartitionRxFrameTable[frameIdx].state.Rx.receiverTimer);
  
  /* cancel timers */
  TMR_FreeTimer(gZclPartitionRxFrameTable[frameIdx].state.Rx.nackTimer);
  

  
  /* free Ack buffer */
  MSG_Free(gZclPartitionRxFrameTable[frameIdx].state.Rx.pAckList);
  
  /* mark entry as free */
  gZclPartitionRxFrameTable[frameIdx].isFree = TRUE;
   
   /* ZHCTODO cancel event to application */
}


/******************************************************************************
* ZCL_PartitionCancelTxFrame
*
* Cleans up information and timers for a pending frame to be sent for which 
* transmission was finalized or that expired
******************************************************************************/

void ZCL_PartitionCancelTxFrame
(
  uint8_t frameIdx
) 
{
  /* cancel timers */
  TMR_FreeTimer(gZclPartitionTxFrameTable[frameIdx].state.Tx.senderTimer);
  
  TMR_FreeTimer(gZclPartitionTxFrameTable[frameIdx].state.Tx.txTimer);
  
  /* free resend buffer */
  MSG_Free(gZclPartitionRxFrameTable[frameIdx].state.Tx.pListResend);
  
  /* mark entry as free */
  gZclPartitionTxFrameTable[frameIdx].isFree = TRUE;
   
   /* ZHCTODO cancel event to application */
}


/******************************************************************************
* ZCL_PartitionSenderTimeoutExpired
*
* Called when the partition sender timeout expires
******************************************************************************/

void ZCL_PartitionSenderTimeoutExpired
(
  tmrTimerID_t timer
) 
{

  zclPartitionedClusterFrame_t* pFrameEntry;
  zclPartitionTxFrameState_t* pTxFrameState;
  
  /* find the TX frame for which this was sent (based on timer) */   
  index_t frameIdx = ZCL_PartitionFindFrameIdxForTimer(gZclPartitionTxFrameTable, timer, 
                                    MbrOfs(zclPartitionTxFrameState_t, senderTimer));
  
  if (frameIdx != gInvalidTableIndex_c) {
  
    pFrameEntry = &gZclPartitionTxFrameTable[frameIdx];
    pTxFrameState = &pFrameEntry->state.Tx;
    
    /* if retries is 0 */
    if (pTxFrameState->currentSendRetries == 0) {
      
      /* cleanup registration table */
      ZCL_PartitionCancelTxFrame(frameIdx);   

      return;
    }
    /* else if retries is not 0 */    
    else {
    
      /* if state of the frame is in send block */
      if (pTxFrameState->sendingNacks == FALSE)
        
        /* reset current frame index to block start index */
        pTxFrameState->currentFrameIndex = pTxFrameState->currentStartFrame;
    
      /* else if not in send block mode but in NACK mode */  
      else
      
        /* reset current frame index to 0 */    
        pTxFrameState->currentFrameIndex = 0;      
    }
    
    /* decrement the retries */
    pTxFrameState->currentSendRetries--;
    

    /* restart sender timer */
    TMR_StartSingleShotTimer(pTxFrameState->senderTimer, 
                             OTA2Native16(pFrameEntry->frame.partitionAttrs.senderTimeout), 
                             ZCL_PartitionSenderTimeoutExpired);

    /* restart TX timer */
    TMR_StartSingleShotTimer(pTxFrameState->txTimer,  
                             pFrameEntry->frame.partitionAttrs.interframeDelay, 
                             ZCL_PartitionTxTimerHandler);
  }
}


/******************************************************************************
* ZCL_PartitionHandleMultipleAckFrame
*
* Called when the transmitter receives a MultipleAck frame
******************************************************************************/

void ZCL_PartitionHandleMultipleAckFrame
(
  zclFrame_t *pFrame, 
  uint8_t asduLength
) 
{
  
  
  zclPartitionedClusterFrame_t* pFrameEntry;
  zclPartitionTxFrameState_t* pTxFrameState;
  zclPartitionCmdMultipleACK_t* pMultipleACKFrame = (zclPartitionCmdMultipleACK_t *)(pFrame + 1);
//zclPartitionCmdMultipleACK16bit_t* pMultipleACKFrame16Bit = (zclPartitionCmdMultipleACK16bit_t*)pMultipleACKFrame;
  bool_t isId16bit = FALSE;
  uint8_t nackCount = 0;
  index_t frameIdx;
  
  /* uncomment on sender for test 5.9.6 */
  /* return; */
  asduLength -= sizeof(zclFrame_t);

  /* find the TX frame for which this was sent (based on transaction Id?) */ 
  frameIdx = ZCL_PartitionFindFrameIdx(gZclPartitionTxFrameTable, NULL, pFrame->transactionId);
  
  if (frameIdx != gInvalidTableIndex_c) {
  
    pFrameEntry = &gZclPartitionTxFrameTable[frameIdx];
    pTxFrameState = &pFrameEntry->state.Tx;
    
    /* verify NACK count; last sizeof is for the FirstFrameId first byte */
    asduLength -=  sizeof(zclPartitionAckOptions_t) + sizeof(uint8_t);
    
    /* check if FirstFrameId and NACKIds are 16bit */
    if (pMultipleACKFrame->ackOptions & gZclPartitionAckOptions_IdLengthMask == 
        gZclPartitionAckOptions_IdLen16bit) {
        isId16bit = TRUE;
        
        /* decrease 2nd byte of FirstFrameId */
        asduLength--;
    }
    
    /* if ACK does not contain NACKs */
    if (asduLength == 0) {
      
      /* set retries back to Max */
      pTxFrameState->currentSendRetries = pFrameEntry->frame.partitionAttrs.noOfSendRetries; 
      
      /* free the NACK buffer is that exists */
      MSG_Free(pTxFrameState->pListResend);
      
      /* if the full frame has been sent */
      if (pTxFrameState->currentFrameIndex == OTA2Native16(pFrameEntry->frame.frameCount)) {
        
        pfnPartitionedClusterDataReq_t dataReqFunc = NULL;
        zclPartitionAppEvent_t appEventParams; 
      
        index_t i = ZCL_PartitionFindClusterIdx(pFrameEntry->frame.addrInfo.aClusterId);
        if (i != gInvalidTableIndex_c)
          dataReqFunc = gZclPartitionClusterTable[i].info.dataReqHandler;    
      
        appEventParams.eventType = gZclPartitionAppEventType_TxFrameComplete;
        appEventParams.zclTransactionId = pFrameEntry->frame.zclTransactionId;
        (void)dataReqFunc(&appEventParams, NULL);
      
        /* cleanup the registration table */
        ZCL_PartitionCancelTxFrame(frameIdx);
        
        return;
      }
      
      /* else if there are still blocks to be sent */
      else {
        /* set state to sending block */
        pTxFrameState->sendingNacks = FALSE;
        
        /* set currentStartFrame of block and index to start of next block */
        pTxFrameState->currentStartFrame += pFrameEntry->frame.partitionAttrs.noOfAckFrame;
        pTxFrameState->currentFrameIndex = pTxFrameState->currentStartFrame;
      }
        
    } 
    
    /* else if ACK contains NACKs */
    else {
      
      nackCount = asduLength;
      
      if (isId16bit)
        nackCount = asduLength / 2;
      
      /* if frame is not in sending NACKs */
      if (!pTxFrameState->sendingNacks) {
        
        /* reset retries */
        pTxFrameState->currentSendRetries = pFrameEntry->frame.partitionAttrs.noOfSendRetries; 
        
        /* set state to sending NACKs */      
        pTxFrameState->sendingNacks = TRUE;
      }
      /* else frame was in sending NACK retries mode*/    
      else {
        /* decrement retries */  
        pTxFrameState->currentSendRetries--;
        
        /* if retries == 0 */
        if (pTxFrameState->currentSendRetries == 0) {
          
          /* cleanup registration table */
          ZCL_PartitionCancelTxFrame(frameIdx);   
          return;          
        }
      }
      
      /* allocate a buffer */
      pTxFrameState->pListResend = AF_MsgAlloc();
      if (pTxFrameState->pListResend) {
        
        pTxFrameState->pListResend->sizeofNackId = isId16bit ? 2 : 1;
        pTxFrameState->pListResend->nackIdCount = nackCount;
        
        pTxFrameState->currentFrameIndex = 0;
        
        /* copy the Nacks */
        FLib_MemCpy(&pTxFrameState->pListResend->nackIds, (uint8_t*)&pMultipleACKFrame->nackId + isId16bit, asduLength);  
      }
      /* else ZHCTODO if buffer cannot be allocated */
    }

    /* restart sender timer */
    TMR_StartSingleShotTimer(pTxFrameState->senderTimer, 
                             OTA2Native16(pFrameEntry->frame.partitionAttrs.senderTimeout), 
                             ZCL_PartitionSenderTimeoutExpired);

    /* restart TX timer */
    TMR_StartSingleShotTimer(pTxFrameState->txTimer,  
                             pFrameEntry->frame.partitionAttrs.interframeDelay, 
                             ZCL_PartitionTxTimerHandler);
  }
}


/******************************************************************************
* ZCL_PartitionTransferPartitionedFrameReq
*
* Called to send a fragment over the air
******************************************************************************/

zclPartitionStatus_t ZCL_PartitionTransferPartitionedFrameReq
(
  afAddrInfo_t* pAddrInfo,
  uint8_t transactionId,
  uint8_t pReqLen,
  zclPartitionCmdTransferPartitionedFrame_t* pReq
) 
{
  afAddrInfo_t addrInfo;
  afToApsdeMessage_t *pMsg;
  uint8_t payloadLen;

  /* create the frame and copy in payload */
  payloadLen = 0;
  pMsg = ZCL_CreateFrame(pAddrInfo, gZclPartitionCmdRx_TransferPartFrame_c, 
                          gZclFrameControl_FrameTypeSpecific | gZclFrameControl_DisableDefaultRsp,
                          &transactionId, &payloadLen, NULL);
  if(!pMsg)
    return gZclNoMem_c;
  
  FLib_MemCpy((void *)(&((uint8_t *)pMsg)[ApsmeGetAsduOffset() +  sizeof(zclFrame_t)]),
              pReq, pReqLen);
              
  MSG_Free(pReq);
  
  FLib_MemCpy(&addrInfo, pAddrInfo, sizeof(afAddrInfo_t));
  Copy2Bytes(addrInfo.aClusterId, gaPartitionClusterId_c);
              
  /* send the packet */
  return ZCL_DataRequestNoCopy(&addrInfo, pReqLen + sizeof(zclFrame_t), pMsg);    
}


/******************************************************************************
* ZCL_PartitionRegisterRxFrame
*
* Registers a new frame to be sent by filling in the frame information in the
* partition frame table.
******************************************************************************/

zclPartitionStatus_t ZCL_PartitionRegisterRxFrame
(
  zclPartitionedClusterFrameInfo_t *pRxFrameRegistration
) 
{
  index_t entryIdx;
  zclPartitionedClusterFrame_t *pFrameEntry;
  
  /* check if cluster is registered with the partition server */
  entryIdx = ZCL_PartitionFindClusterIdx(pRxFrameRegistration->addrInfo.aClusterId);
  
  /* cluster has not been registred to be partitioned */
  if (entryIdx == gInvalidTableIndex_c)
    return gZclPartitionClusterNotReg_c;

  /* now look for frame entry */
  entryIdx = ZCL_PartitionFindFrameIdx(gZclPartitionRxFrameTable, 
                                       pRxFrameRegistration->addrInfo.aClusterId,
                                       pRxFrameRegistration->zclTransactionId);

  if (entryIdx != gInvalidTableIndex_c)                                       
    return  gZclPartitionRegExists_c;
    
  entryIdx =  ZCL_PartitionAllocFrame(gZclPartitionRxFrameTable);
    
  if (entryIdx != gInvalidTableIndex_c) {
    
    pFrameEntry = &gZclPartitionRxFrameTable[entryIdx];

    /* found a free entry in the table so add registration info there*/
    FLib_MemCpy(&pFrameEntry->frame, pRxFrameRegistration, 
                      MbrOfs(zclPartitionedClusterFrame_t, state));
    
        /* small buffer */
    pFrameEntry->state.Rx.pAckList = MSG_Alloc(sizeof(zclPartitionBitwiseAckTable_t));
    
    if (!pFrameEntry->state.Rx.pAckList) {
      pFrameEntry->isFree = TRUE;
      return gZclPartitionNoMem_c;
    }
    
    /* initialize timers first so that we catch a frame alloc error early */
    pFrameEntry->state.Rx.nackTimer = TMR_AllocateTimer();
    pFrameEntry->state.Rx.receiverTimer = TMR_AllocateTimer();
    
    if (pFrameEntry->state.Rx.nackTimer == gTmrInvalidTimerID_c ||
        pFrameEntry->state.Rx.receiverTimer == gTmrInvalidTimerID_c) {
      pFrameEntry->isFree = TRUE;      
      return gZclPartitionFailed_c;
    }

    /* initialize the state struct members */
    pFrameEntry->state.Rx.currentStartFrame = 0;
    
    return gZclPartitionSuccess_c;
  }
  
  /* no free entry in frame table */
  return gZclPartitionNoMem_c;
}

/******************************************************************************
* ZCL_HandleWriteHandshakeParamReq
*
* Called when a WriteHandshakeParam frame is received
******************************************************************************/

zclPartitionStatus_t ZCL_HandleWriteHandshakeParamReq
(
  zbApsdeDataIndication_t *pIndication
) 
{
  index_t entryIdx;
  zclPartitionedClusterFrame_t *pFrameEntry;
  zbClusterId_t clusterId;
  
  zclFrame_t* pFrame = (zclFrame_t *)pIndication->pAsdu;
  
  /* the partitioned cluster id is the first field of the writehandshake frame */
  Copy2Bytes(&clusterId, (uint8_t *)(pFrame + 1));
  
  /* check if cluster is registered with the partition server */
  entryIdx = ZCL_PartitionFindClusterIdx(clusterId);
  
  /* cluster has not been registred to be partitioned */
  if (entryIdx == gInvalidTableIndex_c)
    return gZclPartitionClusterNotReg_c;

  /* now look for frame entry */
  entryIdx = ZCL_PartitionFindFrameIdx(gZclPartitionRxFrameTable, 
                                       clusterId,
                                       pFrame->transactionId);

  if (entryIdx != gInvalidTableIndex_c)                                       
    return  gZclPartitionRegExists_c;
    
  entryIdx =  ZCL_PartitionAllocFrame(gZclPartitionRxFrameTable);
    
  if (entryIdx != gInvalidTableIndex_c) {
    
    pFrameEntry = &gZclPartitionRxFrameTable[entryIdx];
    
    AF_PrepareForReply(&pFrameEntry->frame.addrInfo, pIndication);
    
    Copy2Bytes(pFrameEntry->frame.addrInfo.aClusterId, &clusterId);
    
    /* small buffer */
    pFrameEntry->state.Rx.pAckList = MSG_Alloc(sizeof(zclPartitionBitwiseAckTable_t));
    
    if (!pFrameEntry->state.Rx.pAckList) {
      pFrameEntry->isFree = TRUE;
      return gZclPartitionNoMem_c;
    }
    
    FLib_MemSet(pFrameEntry->state.Rx.pAckList, 0x00, sizeof(zclPartitionBitwiseAckTable_t));  
    FLib_MemCpy(&pFrameEntry->frame.partitionAttrs, (void*)&gDefaultPartitionAttrs, sizeof(zclPartitionAttrs_t));
    
    /* initialize timers first so that we catch a frame alloc error early */
    pFrameEntry->state.Rx.nackTimer = TMR_AllocateTimer();
    pFrameEntry->state.Rx.receiverTimer = TMR_AllocateTimer();
    
    if (pFrameEntry->state.Rx.nackTimer == gTmrInvalidTimerID_c ||
        pFrameEntry->state.Rx.receiverTimer == gTmrInvalidTimerID_c) {
      pFrameEntry->isFree = TRUE;      
      return gZclPartitionFailed_c;
    }

    pFrameEntry->frame.zclTransactionId = pFrame->transactionId;
     
    /* initialize the state struct members */
    pFrameEntry->state.Rx.currentStartFrame = 0;

    
    return gZclPartitionSuccess_c;
  }
  
  /* no free entry in frame table */
  return gZclPartitionNoMem_c;
}



/******************************************************************************
*******************************************************************************
* Public functions
*******************************************************************************
******************************************************************************/



/******************************************************************************
*  ZCL_PartitionInit
*
* Resets partition frame table state
******************************************************************************/

void ZCL_PartitionInit
(
) 
{
  index_t i;
  
  /* set registration tables as free */
  for (i = 0; i < NumberOfElements(gZclPartitionClusterTable); i++)
     gZclPartitionClusterTable[i].isFree = TRUE;
     
  for (i = 0; i < NumberOfElements(gZclPartitionTxFrameTable); i++) {
     gZclPartitionTxFrameTable[i].isFree = TRUE;
     gZclPartitionRxFrameTable[i].isFree = TRUE;
  }
}


/******************************************************************************
* ZCL_PartitionWriteHandshakeParamReq
*
* Sends a WriteHandshakeParam frame from the transmitter 
******************************************************************************/

zclPartitionStatus_t ZCL_PartitionWriteHandshakeParamReq
(
  zclPartitionWriteHandshakeParamReq_t *pReq
)
{
  afToApsdeMessage_t *pMsg;
  uint8_t payloadLen;
  zclPartitionCmdWriteHandshakeParamRecord_t *pWriteReqRecord;
  zclCmdWriteAttrRecord_t *pOtaWriteRecord;
  index_t i;
  uint8_t attrLen;

  /* nothing to do! */
  if(!pReq->count)
    return gZbSuccess_c;

  /* create the frame and copy in payload */
  payloadLen = sizeof(zbClusterId_t);
  pMsg = ZCL_CreateFrame(&(pReq->addrInfo), gZclPartitionCmdRx_WriteHandshakeParam_c, 
                          gZclFrameControl_FrameTypeSpecific | gZclFrameControl_DisableDefaultRsp, 
                          &pReq->transactionId, &payloadLen,NULL);
  if(!pMsg)
    return gZclNoMem_c;

  /* pointer to input command frames */
  pWriteReqRecord = pReq->aCmdFrame;

  /* were to write the write attr request OTA frame */
  pOtaWriteRecord = (void *)(&((uint8_t *)pMsg)[ApsmeGetAsduOffset() +  sizeof(zclFrame_t)]);
  
  Copy2Bytes(pOtaWriteRecord, pReq->clusterId);
  
  pOtaWriteRecord = (zclCmdWriteAttrRecord_t *)((uint8_t*)pOtaWriteRecord + sizeof(zbClusterId_t));

  /* attribute length */
  for(i=0; i<pReq->count; ++i) {

    pOtaWriteRecord->attrId = pWriteReqRecord->attrId;
    pOtaWriteRecord->attrType = pWriteReqRecord->attrType;
    
    attrLen = pWriteReqRecord->attrLen;
    FLib_MemCpy(pOtaWriteRecord->aData, pWriteReqRecord->aData, attrLen);

    /* on to next attribute in OTA frame */
    attrLen += sizeof(zclAttrId_t) + sizeof(zclAttrType_t);
    pOtaWriteRecord = ( zclCmdWriteAttrRecord_t* )( ( (uint8_t *)pOtaWriteRecord ) + attrLen );
    payloadLen += attrLen;

    /* on to next request */
    ++attrLen;  /* one extra byte for length in request structure */
    pWriteReqRecord = ( zclPartitionCmdWriteHandshakeParamRecord_t* )( ( (uint8_t *)pWriteReqRecord ) + attrLen );
  }
  
  pReq->addrInfo.txOptions |= gApsTxOptionAckTx_c;

  /* send the packet */
  return ZCL_DataRequestNoCopy(&(pReq->addrInfo), payloadLen, pMsg);
}





/******************************************************************************
* ZCL_PartitionClusterServer
*
* Incoming partition cluster frame processing loop for cluster server side
******************************************************************************/

zbStatus_t ZCL_PartitionClusterServer
(
  zbApsdeDataIndication_t *pIndication, 
  afDeviceDef_t *pDev
)
{
    zclCmd_t       command;
    zclFrame_t     *pFrame;
    // zbClusterId_t  ClusterId={gaZclClusterPartition_c};
    zbStatus_t     result = gZclUnsupportedClusterCommand_c;    
    afToApsdeMessage_t *pMsgRsp;  
    zclFrame_t *pFrameRsp;  
    uint8_t iPayloadLen;  
    uint8_t attrCount;  
    
    /* prevent compiler warning */
    (void)pDev;

    pFrame = (void *)pIndication->pAsdu;

    /* handle the command */
    command = pFrame->command;
    
    if  (command == gZclPartitionCmdRx_TransferPartFrame_c) {
      ZCL_PartitionHandleTransferPartitionedFrame(pIndication, pFrame, pIndication->asduLength);
      result = gZbSuccess_c;
    } 
    else if (command == gZclPartitionCmdRx_ReadHandshakeParam_c) 
    {
      pMsgRsp = AF_MsgAlloc();
      if(!pMsgRsp)
        return gZclNoMem_c;      
      
      pFrameRsp = (void *)(((uint8_t *)pMsgRsp) + gAsduOffset_c);
        /* number of attributes in the list */  
      attrCount = (uint8_t)((uint8_t)(pIndication->asduLength - (uint8_t)sizeof(zclFrame_t) -
                  (uint8_t)sizeof(zbClusterId_t)) / (uint8_t)sizeof(zclAttrId_t));
      iPayloadLen = ZCL_BuildReadAttrResponseWithPartition(
                                            pDev,                   /* IN: ptr to device, for instance data */
                                            ZCL_FindCluster(pDev, pIndication->aClusterId),/* IN: cluster to read from */
                                            attrCount,              /* read attr count */
                                            pFrame,                 /* IN: request input frame */
                                            pFrameRsp,
                                            TRUE
                                           );         
      return ZCL_ReplyNoCopy(pIndication, iPayloadLen - sizeof(zclFrame_t), pMsgRsp);
    } 
    else if (command == gZclPartitionCmdRx_WriteHandshakeParam_c) 
    {
      bool_t forceWritingROAttrs = FALSE;
      
      zclPartitionStatus_t writeResult = ZCL_HandleWriteHandshakeParamReq(pIndication);
      
      if (writeResult == gZclPartitionSuccess_c)
        forceWritingROAttrs = TRUE;
      
      if (writeResult == gZclPartitionSuccess_c || writeResult == gZclPartitionRegExists_c) {
        
        pMsgRsp = AF_MsgAlloc();
        if(!pMsgRsp)
          return gZclNoMem_c;      
        
        (void)ZCL_BuildWriteAttrResponseWithPartition(pDev, ZCL_FindCluster(pDev, pIndication->aClusterId), (uint8_t)pIndication->asduLength,
                                                     pFrame, (zclFrame_t*)pMsgRsp, TRUE, forceWritingROAttrs);
                                                     
        MSG_Free(pMsgRsp);
      }
      
      result = gZbSuccess_c;
    }
    
    return result;
}

/******************************************************************************
* ZCL_PartitionRegisterTxFrame
*
* Used by the partitioned cluster to register and begin sending a partitioned
* frame.
******************************************************************************/

zclPartitionStatus_t ZCL_PartitionRegisterTxFrame
(
  zclPartitionedClusterFrameInfo_t *pTxFrameRegistration
) 
{
  index_t i, entryIdx;
  zclPartitionedClusterFrame_t *pFrameEntry;
  zclPartitionWriteHandshakeParamReq_t* writeHandshakeParamReq;
  zclPartitionCmdWriteHandshakeParamRecord_t* currentWriteParamRecord;
  zclPartitionAttrs_t* pFrameAttrs;
  
  /* check if cluster is registered with the partition server */
  entryIdx = ZCL_PartitionFindClusterIdx(pTxFrameRegistration->addrInfo.aClusterId);
  
  /* cluster has not been registred to be partitioned */
  if (entryIdx == gInvalidTableIndex_c)
    return gZclPartitionClusterNotReg_c;

  /* now look for frame entry */
  entryIdx = ZCL_PartitionFindFrameIdx(gZclPartitionTxFrameTable, 
                                       pTxFrameRegistration->addrInfo.aClusterId,
                                       pTxFrameRegistration->zclTransactionId);

  if (entryIdx != gInvalidTableIndex_c)                                       
    return  gZclPartitionRegExists_c;
    
  entryIdx =  ZCL_PartitionAllocFrame(gZclPartitionTxFrameTable);
    
  if (entryIdx != gInvalidTableIndex_c) {
    
    pFrameEntry = &gZclPartitionTxFrameTable[entryIdx];
    pFrameAttrs = &pFrameEntry->frame.partitionAttrs;
    
    /* initialize timers first so that we catch a frame alloc error early */
    pFrameEntry->state.Tx.txTimer = TMR_AllocateTimer();
    pFrameEntry->state.Tx.senderTimer = TMR_AllocateTimer();
    
    if (pFrameEntry->state.Tx.txTimer == gTmrInvalidTimerID_c ||
        pFrameEntry->state.Tx.senderTimer == gTmrInvalidTimerID_c)
      return gZclPartitionFailed_c;

    /* initialize buffer for writeHandshakeReq so that we catch a frame alloc error early */
    writeHandshakeParamReq = AF_MsgAlloc();
    if (!writeHandshakeParamReq)
      return gZclNoMem_c;      

    /* found a free entry in the table so add registration info there*/
    FLib_MemCpy(&pFrameEntry->frame, pTxFrameRegistration, 
                      MbrOfs(zclPartitionedClusterFrame_t, state));
    
    /* initialize the rest of state struct members */
    pFrameEntry->state.Tx.currentSendRetries = pFrameAttrs->noOfSendRetries;
    pFrameEntry->state.Tx.sendingNacks = FALSE;
    pFrameEntry->state.Tx.currentStartFrame = 0;
    pFrameEntry->state.Tx.currentFrameIndex = 0;
    
    /* begin sending the WriteHandshakeParam to the other device */
    FLib_MemCpy(&writeHandshakeParamReq->addrInfo, &pFrameEntry->frame.addrInfo, sizeof(afAddrInfo_t));
    
    /* replace partitioned cluster id in addrinfo with the partition cluster id */
    Copy2Bytes(writeHandshakeParamReq->addrInfo.aClusterId, &gaPartitionClusterId_c);
    
    /* set all attributes to be written in the writeHandshakeParamReq */
    writeHandshakeParamReq->count = 4; //NumberOfElements(gaPartitionClusterAttrDef);
    
    /* set partitioned clusterId to be used as firest field in the writeHandshake Req */
    Copy2Bytes(&writeHandshakeParamReq->clusterId, pFrameEntry->frame.addrInfo.aClusterId);
    
    writeHandshakeParamReq->transactionId = pFrameEntry->frame.zclTransactionId;
    
    /* initialize currentWriteParamRecord to startOfFrame */
    currentWriteParamRecord = (zclPartitionCmdWriteHandshakeParamRecord_t*)&writeHandshakeParamReq->aCmdFrame;

    /* copy attributes for this frame to build writeParamRecordReq */
    for (i = 0; i < NumberOfElements(gaPartitionClusterAttrDef); i++) {
      zclAttrDef_t* pAttrDef = (zclAttrDef_t*)&gaPartitionClusterAttrDef[i];
      if (i != 0x02 &&
          i != 0x03 &&
          i != 0x04 &&
          i != 0x06) 
          continue;
      currentWriteParamRecord->attrId = pAttrDef->id;
      currentWriteParamRecord->attrType = pAttrDef->type;
      currentWriteParamRecord->attrLen = pAttrDef->maxLen;
      FLib_MemCpy(currentWriteParamRecord->aData, (uint8_t*)pFrameAttrs + pAttrDef->data.offset, 
                    pAttrDef->maxLen);
      currentWriteParamRecord = (zclPartitionCmdWriteHandshakeParamRecord_t*)
                                ((uint8_t*)currentWriteParamRecord + 
                                  MbrOfs(zclPartitionCmdWriteHandshakeParamRecord_t, aData) +
                                  currentWriteParamRecord->attrLen);
    }
    
    /* send the handshake command */
    (void)ZCL_PartitionWriteHandshakeParamReq(writeHandshakeParamReq);
    
    MSG_Free(writeHandshakeParamReq);
    
    /* start Tx timer */
    TMR_StartSingleShotTimer(pFrameEntry->state.Tx.txTimer, pFrameAttrs->interframeDelay, ZCL_PartitionTxTimerHandler);

    return gZclPartitionSuccess_c;
  }
  
  /* no free entry in frame table */
  return gZclPartitionNoMem_c;
}



/******************************************************************************
* ZCL_PartitionClusterClient
*
* Incoming partition cluster frame processing loop for cluster client side
******************************************************************************/

zbStatus_t ZCL_PartitionClusterClient
(
  zbApsdeDataIndication_t *pIndication, 
  afDeviceDef_t *pDev
)
{
    zclCmd_t       command;
    zclFrame_t     *pFrame;
    // zbClusterId_t  ClusterId={gaZclClusterPartition_c};
    zbStatus_t     result = gZclUnsupportedClusterCommand_c;    
    
    /* prevent compiler warning */
    (void)pDev;

    pFrame = (void *)pIndication->pAsdu;

    /* handle the command */
    command = pFrame->command;

    if  (command == gZclPartitionCmdTx_MultipleACKs) {
      ZCL_PartitionHandleMultipleAckFrame(pFrame, pIndication->asduLength);
      result = gZbSuccess_c;
    } 
    else if (command == gZclPartitionCmdTx_ReadHandshakeParamRsp_c) {
      result = gZbSuccess_c;
    }

    return result;
}


/******************************************************************************
* ZCL_PartitionRegisterCluster
*
* Used by the partitioned cluster to register itself with the partition cluster
******************************************************************************/

zclPartitionStatus_t ZCL_PartitionRegisterCluster
(
  zclPartitionedClusterInfo_t *pClusterRegistration
) 
{
  index_t i, firstFreeIdx = gInvalidTableIndex_c;
  
  /* check if there is room in the registered clusters frame or if the cluster exists */
  for (i = 0; i < NumberOfElements(gZclPartitionClusterTable); i++) {
  
    /* get registration information for this cluster table entry */
    zclPartitionedClusterInfoRecord_t* pClusterInfo = &gZclPartitionClusterTable[i];
    
    /* compare with cluster id in request and return error if cluster exists*/
    if (IsEqual2Bytes(pClusterInfo->info.clusterId, pClusterRegistration->clusterId))
      return gZclPartitionRegExists_c;
      
    /* check if this entry is free and store the first free entry idx in firstFreeIdx */
    if (firstFreeIdx == gInvalidTableIndex_c && pClusterInfo->isFree)
      firstFreeIdx = i;
  }
  
  if (firstFreeIdx != gInvalidTableIndex_c) {
    
    gZclPartitionClusterTable[firstFreeIdx].isFree = FALSE;
    
    /* found a free entry in the table so add registration info there*/
    FLib_MemCpy(&gZclPartitionClusterTable[firstFreeIdx].info, pClusterRegistration, 
                  sizeof(zclPartitionedClusterInfo_t));
    
    return gZclPartitionSuccess_c;
  }
  
  /* no free entry in cluster table */
  return gZclPartitionNoMem_c;
}
  
  

  

/******************************************************************************
* ZCL_PartitionReadHandshakeParamReq
*
* Sends the partition ReadHandshakeParamRequest frame
******************************************************************************/  

zclPartitionStatus_t ZCL_PartitionReadHandshakeParamReq
(
  zclPartitionCmdReadHandshakeParamReq_t *pReq
) 
{
  uint8_t iPayloadLen;
  afToApsdeMessage_t *pMsg;

  /* create the frame and copy in payload */
  iPayloadLen = MbrOfs(zclPartitionCmdReadHandshakeParamFrame_t, aAttr) + 
                  pReq->count * sizeof(zclAttrId_t);
                  
              
  pMsg = ZCL_CreateFrame(&(pReq->addrInfo),gZclPartitionCmdRx_ReadHandshakeParam_c,
          gZclFrameControl_FrameTypeSpecific, &pReq->transactionId, &iPayloadLen,&(pReq->cmdFrame));
  if(!pMsg)
    return gZbNoMem_c;

  /* send packet over the air */
  return ZCL_DataRequestNoCopy(&(pReq->addrInfo), iPayloadLen, pMsg);
}


/******************************************************************************
* ZCL_FindPartitionAttr
*
* Finds a partition cluster attribute definition
******************************************************************************/

zclAttrDef_t *ZCL_FindPartitionAttr
(
  zclAttrId_t attrId, 
  zbClusterId_t partitionedCluster, 
  uint8_t transactionId
) 
{
  
  zclAttrDefList_t  *pAttrList = (zclAttrDefList_t  *)&gPartitionClusterAttrDefList;
  zclAttrDef_t const *pAttrDef;
  uint16_t            i;

  (void)partitionedCluster;
  (void)transactionId;
  
  pAttrDef = pAttrList->pAttrDef;
  for(i=0; i<pAttrList->count; ++i)
  {
    zclAttrDef_t const *pCurrentAttrDef = &(pAttrDef[i]);
    
    if(attrId == pCurrentAttrDef->id) {
        return (void *)(pCurrentAttrDef);      
    }
      
  }

  /* not found */
  return NULL;
}


/******************************************************************************
* ZCL_GetPartitionAttributePtr
*
* Gets a pointer to the partition attribute
******************************************************************************/

void *ZCL_GetPartitionAttributePtr
(
    zclAttrId_t attrId,           /* IN: attribute ID (in little endian) */
    zbClusterId_t partitionedCluster, 
    uint8_t transactionId,   
    uint8_t *pAttrLen             /* IN/OUT: return size of attribute (actual not max). */
 )
{
  zclAttrDef_t *pAttrDef;
  index_t entryIdx;
  zclPartitionedClusterFrame_t *pFrameAttrs;

  /* get pointer to attribute structure */
  pAttrDef = ZCL_FindPartitionAttr(attrId, partitionedCluster, transactionId);


  entryIdx = ZCL_PartitionFindFrameIdx(gZclPartitionRxFrameTable, 
                                       partitionedCluster,
                                       transactionId);
  
  pFrameAttrs = &gZclPartitionRxFrameTable[entryIdx];
  
  if (entryIdx == gInvalidTableIndex_c) {
    
      entryIdx = ZCL_PartitionFindFrameIdx(gZclPartitionTxFrameTable, 
                                         partitionedCluster,
                                         transactionId);
                                         
      pFrameAttrs = &gZclPartitionTxFrameTable[entryIdx];
  }

  if (entryIdx != gInvalidTableIndex_c) {
    uint8_t* pAttr = (uint8_t*)&pFrameAttrs->frame.partitionAttrs;
    pAttr += pAttrDef->data.offset;
    *pAttrLen = pAttrDef->maxLen;
    return (void *)pAttr;    
  } else {
    uint8_t* pAttr = (uint8_t*)&gDefaultPartitionAttrs;
    pAttr += pAttrDef->data.offset;
    *pAttrLen = pAttrDef->maxLen;
    return (void *)pAttr;      
  }
}

/******************************************************************************
* ZCL_PartitionSetDefaultAttrs
*
* Sets the default attributes for the partition cluster
******************************************************************************/
zclPartitionStatus_t ZCL_PartitionSetDefaultAttrs
(
  zclPartitionAttrs_t *pDefaultAttrs
) 
{
  FLib_MemCpy(&gDefaultPartitionAttrs, pDefaultAttrs, sizeof(zclPartitionAttrs_t));
  return gZbSuccess_c;
}

#endif
