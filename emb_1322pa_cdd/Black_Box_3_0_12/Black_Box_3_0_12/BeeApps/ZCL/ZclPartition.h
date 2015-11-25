/******************************************************************************
* ZclProtocInterf.c
*
* This module contains code that handles ZigBee Cluster Library commands and
* clusters for the ZCL Partition cluster
*
* Copyright (c) 2010, Freescale, Inc.  All rights reserved.
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
#ifndef _ZCL_PARTITION_H
#define _ZCL_PARTITION_H

#include "ZclOptions.h"
#include "AfApsInterface.h"
#include "AppAfInterface.h"
#include "BeeStackInterface.h"
#include "EmbeddedTypes.h"
#include "ZCL.h"


/******************************************************************************
*******************************************************************************
* Public macros and data types definitions.
*******************************************************************************
******************************************************************************/

#define gZclPartitionMaxRegisteredFrames_c               1
#define gZclPartitionMaxRegisteredClusters               2

#define gZclPartitionCmdRx_TransferPartFrame_c       0x00    /* M - Transfer Partitioned Frame */  
#define gZclPartitionCmdRx_ReadHandshakeParam_c      0x01    /* M - Read Handshake Parameters */
#define gZclPartitionCmdRx_WriteHandshakeParam_c     0x02    /* M - Write Handshake Parameters */
#define gZclPartitionCmdTx_MultipleACKs              0x00    /* M - Multiple ACK */
#define gZclPartitionCmdTx_ReadHandshakeParamRsp_c   0x01    /* M - Read Handshake Params Response */

#define gBitwiseAckBufferSize_c 0x1F
/*************************************************************
  Cluster definitions
**************************************************************/

#if ( gBigEndian_c  != TRUE)
#define gZclAttrPartitionMaxIncomingTransferSize_c    0x0000  
#define gZclAttrPartitionMaxOutgoingTransferSize_c    0x0001  
#define gZclAttrPartitionPartitionedFrameSize_c       0x0002
#define gZclAttrPartitionLargeFrameSize_c             0x0003
#define gZclAttrPartitionNumberOfACKFrame_c           0x0004
#define gZclAttrPartitionNACKTimeout_c                0x0005
#define gZclAttrPartitionInterframeDelay_c            0x0006
#define gZclAttrPartitionNumberOfSendRetries_c        0x0007
#define gZclAttrPartitionSenderTimeout_c              0x0008
#define gZclAttrPartitionReceiverTimeout_c            0x0009
#else
#define gZclAttrPartitionMaxIncomingTransferSize_c    0x0000  
#define gZclAttrPartitionMaxOutgoingTransferSize_c    0x0100  
#define gZclAttrPartitionPartitionedFrameSize_c       0x0200
#define gZclAttrPartitionLargeFrameSize_c             0x0300
#define gZclAttrPartitionNumberOfACKFrame_c           0x0400
#define gZclAttrPartitionNACKTimeout_c                0x0500
#define gZclAttrPartitionInterframeDelay_c            0x0600
#define gZclAttrPartitionNumberOfSendRetries_c        0x0700
#define gZclAttrPartitionSenderTimeout_c              0x0800
#define gZclAttrPartitionReceiverTimeout_c            0x0900
#endif 

#ifdef __IAR_SYSTEMS_ICC__
#pragma pack(1)
#endif

typedef uint8_t zclPartitionFragOptions_t;

#define gZclPartitionFragOptions_FirstBlockMask       0x01
#define gZclPartitionFragOptions_PartitionIdLenMask   0x02

#define gZclPartitionFragOptions_SubsequentBlock      0x00
#define gZclPartitionFragOptions_FirstBlock           0x01

#define gZclPartitionFragOptions_PartitionIdLen8bit   0x00
#define gZclPartitionFragOptions_PartitionIdLen16bit  0x02


typedef struct zclPartitionCmdTransferPartitionedFrame_tag
{
  zclPartitionFragOptions_t  fragOptions;
  uint8_t                    partitionId[2];
} zclPartitionCmdTransferPartitionedFrame_t;

typedef uint8_t zclPartitionAckOptions_t;

#define gZclPartitionAckOptions_IdLengthMask       0x01

#define gZclPartitionAckOptions_IdLen8bit          0x00
#define gZclPartitionAckOptions_IdLen16bit         0x01


typedef struct zclPartitionCmdMultipleACK_tag {
  zclPartitionAckOptions_t ackOptions;
  uint8_t firstFrameId;
  uint8_t nackId[1];
} zclPartitionCmdMultipleACK_t;

typedef struct zclPartitionCmdMultipleACK16bit_tag {
  zclPartitionAckOptions_t ackOptions;
  uint8_t firstFrameId[2];
  uint8_t nackId[1];
} zclPartitionCmdMultipleACK16bit_t;

typedef struct zclPartitionNackIdTable_tag {
  uint8_t sizeofNackId;
  uint8_t nackIdCount;
  uint8_t nackIds[1];
} zclPartitionNACKIdTable_t;

typedef struct zclPartitionBitwiseAckTable_t {
  uint8_t ackTable[gBitwiseAckBufferSize_c];
} zclPartitionBitwiseAckTable_t;

typedef struct zclPartitionCmdReadHandshakeParamFrame_tag
{
  zbClusterId_t clusterId;  /* partitioned cluster id */  
  zclAttrId_t  aAttr[3];    /* variable length array of attributes */
} zclPartitionCmdReadHandshakeParamFrame_t;

typedef struct zclPartitionCmdReadHandshakeParamReq_tag
{
  uint8_t           transactionId;
  afAddrInfo_t      addrInfo;     /* IN: source/dst address, cluster, etc... */
  uint8_t           count;        /* IN: how many to read? */
  zclPartitionCmdReadHandshakeParamFrame_t  cmdFrame;     /* IN: read attribute command frame */
} zclPartitionCmdReadHandshakeParamReq_t;


typedef struct zclPartitionCmdWriteHandshakeParamRecord_tag
{
  zclAttrId_t   attrId;
  zclAttrType_t attrType;  
  uint8_t       attrLen;    /* length, so the request knows how much to copy */
  uint8_t       aData[4];   /* up to 4 bytes data */
} zclPartitionCmdWriteHandshakeParamRecord_t;

typedef struct zclPartitionWriteHandshakeParamReq_tag
{
  uint8_t             transactionId;
  afAddrInfo_t        addrInfo;     /* IN: source/dst address, cluster, etc... */
  zbClusterId_t       clusterId;
  uint8_t             count;        /* IN: how many records to write? */
  zclPartitionCmdWriteHandshakeParamRecord_t   aCmdFrame[1]; /* IN: array of variable length command frames */
} zclPartitionWriteHandshakeParamReq_t;

typedef struct zclPartitionAttrs_tag
{
  uint16_t maxInTrsSize;
  uint16_t maxOutTrsSize;
  uint8_t  partitionedFrameSize;
  uint16_t largeFrameSize;
  uint8_t  noOfAckFrame;
  uint16_t nackTimeout;
  uint8_t  interframeDelay;
  uint8_t  noOfSendRetries;
  uint16_t senderTimeout;
  uint16_t receiverTimeout;
} zclPartitionAttrs_t;

typedef struct zclPartitionTxFrameState_tag {
  uint8_t currentSendRetries;
  tmrTimerID_t txTimer;
  tmrTimerID_t senderTimer;
  bool_t sendingNacks;
  uint16_t currentStartFrame;
  uint16_t currentFrameIndex;
  zclPartitionNACKIdTable_t* pListResend;
} zclPartitionTxFrameState_t;

typedef struct zclPartitionRxFrameState_tag {
  tmrTimerID_t nackTimer;
  tmrTimerID_t receiverTimer;
  uint8_t currentStartFrame;
  zclPartitionBitwiseAckTable_t* pAckList;
} zclPartitionRxFrameState_t;

typedef uint8_t zclPartitionStatus_t;

#define gZclPartitionSuccess_c        0x00 /* A request has been executed successfully */
#define gZclPartitionFailed_c         0x81 /* general failure */
#define gZclPartitionNoMem_c          0x82 /* no more space in tables to do the request */
#define gZclPartitionRegExists_c      0x83 /* the device or frame is already registered */ 
#define gZclPartitionTooLarge_c       0x84 /* frame length or value is too large */
#define gZclPartitionClusterNotReg_c  0x85 /* the cluster must register first in order to do the operation */
#define gZclPartitionCanceledReq_c    0x86 /* the request cannot be performed due to a canceled operation */



typedef struct zclPartitionedClusterFrameInfo_tag {
  uint8_t  zclTransactionId;
  afAddrInfo_t addrInfo;
  uint16_t frameCount;  
  zclPartitionAttrs_t partitionAttrs;
} zclPartitionedClusterFrameInfo_t;

typedef struct zclPartitionedDataIndication_tag {
  uint8_t zclTransactionId;
  uint16_t partitionId;
  uint8_t partitionLength;
  uint8_t dataLength;
  zbNwkAddr_t srcAddr;   
  uint8_t srcEndPoint;   
  uint8_t dstEndPoint;   
  uint8_t partitionedFrame[1]; 
} zclPartitionedDataIndication_t;


#define gZclPartitionAppEventType_DataRequest            0x00
#define gZclPartitionAppEventType_SenderTimeoutExpired   0x01
#define gZclPartitionAppEventType_RecvTimeoutExpired     0x02
#define gZclPartitionAppEventType_PartitionCanceled      0x03
#define gZclPartitionAppEventType_FrameComplete          0x04
#define gZclPartitionAppEventType_TxFrameComplete        0x05


typedef struct {
  uint8_t zclTransactionId;
  uint8_t eventType;  
  uint16_t partitionedFrameId;
  zclPartitionedClusterFrameInfo_t* frameInfo;
} zclPartitionAppEvent_t;

typedef void (*pfnPartitionedClusterDataInd_t)(zclPartitionedDataIndication_t *pIndication);
typedef uint8_t* (*pfnPartitionedClusterDataReq_t)(zclPartitionAppEvent_t* pEvent, uint8_t* dataLen);


typedef struct zclPartitionedClusterInfo_tag {
  zbClusterId_t clusterId;
  pfnPartitionedClusterDataInd_t dataIndHandler;
  pfnPartitionedClusterDataReq_t dataReqHandler;
  uint16_t clusterBufferSize;
} zclPartitionedClusterInfo_t;


typedef struct zclPartitionedClusterInfoRecord_tag {
  bool_t isFree;
  zclPartitionedClusterInfo_t info;
} zclPartitionedClusterInfoRecord_t;




typedef struct zclPartitionedClusterFrame_tag {
  bool_t isFree;
  zclPartitionedClusterFrameInfo_t frame;
  union {
    zclPartitionRxFrameState_t Rx;
    zclPartitionTxFrameState_t Tx;
  } state;
} zclPartitionedClusterFrame_t;



#ifdef __IAR_SYSTEMS_ICC__
#pragma pack()
#endif



/******************************************************************************
*******************************************************************************
* Public functions prototypes
*******************************************************************************
******************************************************************************/

extern const zclAttrDef_t gaPartitionClusterAttrDef[];
extern const zclAttrDefList_t gPartitionClusterAttrDefList;
extern uint8_t gZclPartition_TestDataPattern;
extern uint8_t gZclPartition_TestDataLen;
extern zclPartitionAttrs_t gDefaultPartitionAttrs;

void ZCL_PartitionInit
(
  void
);

zclPartitionStatus_t ZCL_PartitionSendPartitionedClusterData
(
  uint8_t transactionId,
  uint8_t pReqLen,
  zclPartitionCmdTransferPartitionedFrame_t* pReq
);

zclPartitionStatus_t ZCL_PartitionWriteHandshakeParamReq
(
  zclPartitionWriteHandshakeParamReq_t *pReq
);

zclPartitionStatus_t ZCL_PartitionReadHandshakeParamReq
(
  zclPartitionCmdReadHandshakeParamReq_t *pReq
) ;

zbStatus_t ZCL_PartitionClusterServer
(
  zbApsdeDataIndication_t *pIndication, 
  afDeviceDef_t *pDev
);

zbStatus_t ZCL_PartitionClusterClient
(
  zbApsdeDataIndication_t *pIndication, 
  afDeviceDef_t *pDev
);

zclPartitionStatus_t ZCL_PartitionRegisterCluster
(
  zclPartitionedClusterInfo_t *pClusterRegistration
);

zclPartitionStatus_t ZCL_PartitionRegisterTxFrame
(
  zclPartitionedClusterFrameInfo_t *pTxFrameRegistration
);

zclPartitionStatus_t ZCL_PartitionRegisterRxFrame
(
  zclPartitionedClusterFrameInfo_t *pRxFrameRegistration
);

zclPartitionStatus_t ZCL_PartitionSetDefaultAttrs
(
  zclPartitionAttrs_t *pDefaultAttrs
);

void ZCL_PartitionTestDataIndication
(
  zclPartitionedDataIndication_t *pIndication
);


uint8_t* ZCL_PartitionTestDataEvent
(
  zclPartitionAppEvent_t *pPartitionEvent,
  uint8_t* dataLen
);

zclPartitionStatus_t ZCL_PartitionRegisterClusterWithTestDataGenerator (
  zbClusterId_t clusterId
);


zclAttrDef_t* ZCL_FindPartitionAttr
(
  zclAttrId_t attrId, 
  zbClusterId_t partitionedCluster, 
  uint8_t transactionId
);

void* ZCL_GetPartitionAttributePtr
(
    zclAttrId_t attrId,           /* IN: attribute ID (in little endian) */
    zbClusterId_t partitionedCluster, 
    uint8_t transactionId,   
    uint8_t *pAttrLen             /* IN/OUT: return size of attribute (actual not max). */
);

#endif
