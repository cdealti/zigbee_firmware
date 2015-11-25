/******************************************************************************
* Implementation of the ZigBee Test Client.
* ZTC task public functions, and various internals.
*
* Copyright (c) 2008, Freescale, Inc.  All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
******************************************************************************/

#include "EmbeddedTypes.h"
#if gBeeStackIncluded_d
#include "ZigBee.h"
#include "BeeStackFunctionality.h"
#include "BeeStackConfiguration.h"

#include "BeeStackInterface.h"
#include "BeeCommon.h"
#include "BeeStack_Globals.h"
#endif


#include "MsgSystem.h"
#include "FunctionLib.h"
#include "PublicConst.h"
#include "MacPhyInit.h"
#include "TS_Interface.h"
#include "NVM_Interface.h"
#include "NV_Flash.h"
#include "NV_FlashHAL.h"

#if gBeeStackIncluded_d
#include "BeeStackUtil.h"
#include "ZDOStateMachineHandler.h"
#include "nwkcommon.h"
#include "AfZdoInterface.h"
#include "ZdoApsInterface.h"
#include "ApsMgmtInterface.h"
#include "AfApsInterface.h"
#endif

/*
#include "PWRLib.h"
*/
#include "PublicConst.h"
#if gBeeStackIncluded_d
#include "BeeAppInit.h"
#endif

#include "AspZtc.h"
#include "LlcZtc.h"
#include "ZtcInterface.h"
#include "ZtcPrivate.h"
#include "ZtcClientCommunication.h"
#include "ZtcMsgTypeInfo.h"
#include "ZtcSAPHandlerInfo.h"
#include "ZtcCopyData.h"

#if gMacStandAlone_d
#include "Mac_Globals.h"
#endif

#ifdef __IAR_SYSTEMS_ICC__
#include "AppMacInterrupt.h"
#endif


#if (gZtcIncluded_d && gFragmentationCapability_d)
static void ZtcApsde_Handler(void);
#endif
/******************************************************************************
*******************************************************************************
* Private type definitions
*******************************************************************************
******************************************************************************/

/* Format of the messages echanged between Ztc and the SAP Handlers.
 *
 * *warning* All of the SAP Handlers receive a sapHandlerMsg_t, containing a
 * msgType.
 * The AFDE -> APP and APP -> AFDE SAP Handler send back only raw data,
 * without a msgType, to ZTC_TaskEventMonitor().
 * All other SAP Handlers send back a sapHandlerMsg_t *with* a msgType.
 */
typedef struct sapHandlerMsgHdr_tag {
  ztcMsgType_t msgType;
} sapHandlerMsgHdr_t;

#define mSapHandlerMsgTotalLen_c    144
#define mMaxPacketSize_c            144
typedef union sapHandlerMsg_tag {
  uint8_t raw[mSapHandlerMsgTotalLen_c];
  struct {
    sapHandlerMsgHdr_t header;
    uint8_t payload[mSapHandlerMsgTotalLen_c - sizeof(sapHandlerMsgHdr_t)];
  } structured;
} sapHandlerMsg_t;

/* The ICanHearYou table is a debugging tool used in BeeStack. */
#if gBeeStackIncluded_d
typedef struct ztcICanHearYouTable_tag {
  index_t count;                                /* Number of entries in table. */
  zbNwkAddr_t table[(sizeof(clientPacket_t) - sizeof(clientPacketHdr_t))
                    / sizeof(zbNwkAddr_t)];     /* First entry of variable-length table. */
} ztcICanHearYouTable_t;
#endif

/* Permissions Configuration Table. */
#if (gZtcIncluded_d && (gStandardSecurity_d || gHighSecurity_d) && gApsMaxEntriesForPermissionsTable_c)
typedef struct ztcPermissionsTable_tag {
  index_t count;                                /* Number of entries in table. */
  permissionsTable_t table[(sizeof(clientPacket_t) - sizeof(clientPacketHdr_t))
                    / sizeof(permissionsTable_t)];     /* First entry of variable-length table. */
} ztcPermissionsTable_t;
#endif  /* (gZtcIncluded_d && (gStandardSecurity_d || gHighSecurity_d) && gApsMaxEntriesForPermissionsTable_c) */


typedef struct ztcAbelWrite_tag {
  uint8_t  abelReg;
  uint16_t value;
} ztcAbelWrite_t;

#pragma pack(1)
/* a single fragment as seen by ZTC */
typedef struct apsdeFragment_tag {
  uint8_t iBlockNumber;
  uint8_t iMoreBlocks;
  uint8_t iLength;
  uint8_t aData[1];
} apsdeFragment_t;
#pragma pack()
/******************************************************************************
*******************************************************************************
* Private prototypes
*******************************************************************************
******************************************************************************/
#if gZtcIncluded_d
static void pktFromMsgAsync(uint8_t const *pMsgFromSAPHandler,
                            ztcSAPHandlerInfo_t const *pSrcSAPInfo,
                            ztcMsgTypeInfo_t const *pSrcMsgTypeInfo);

static void pktFromMsgSync(uint8_t const *pMsgFromSAPHandler,
                           ztcSAPHandlerInfo_t const *pSrcSAPInfo,
                           ztcMsgTypeInfo_t const *pSrcMsgTypeInfo,
                           uint8_t const srcStatus);

static void ZtcReq_Handler(void);

static void ZtcCheckRxBufferStatus(void); 


#endif
static void ZtcMsgSetCnfStatus(uint8_t status);

/******************************************************************************
*******************************************************************************
* Private memory declarations
*******************************************************************************
******************************************************************************/

/* fragmentation fields */
extern uint8_t giFragmentedPreAcked;

/* Used to recognize monitor copies of messages that Ztc sent to other code. */
#if gZtcIncluded_d
static bool_t mMsgOriginIsZtc;
extern uint8_t gZDOJoinMode;
#endif

#if gZtcErrorReporting_d
/* Set to TRUE when ZtcError() is called. */
static bool_t mZtcErrorReported;
#endif

#if gBeeStackIncluded_d
#if gZTC_RegEpCapability && gZtcIncluded_d
static zbSimpleDescriptorPackageStore_t maSimpleDescriptors[gNumberOfEndPoints_c];
static endPointDesc_t maEndPointDesc[NumberOfElements(maSimpleDescriptors)];
/* Register maxim 10 clusters */
static zbClusterId_t maInputClusterIdLists [NumberOfElements(maSimpleDescriptors)][10];
static zbClusterId_t maOutputClusterIdLists[NumberOfElements(maSimpleDescriptors)]
                                           [NumberOfElements(maInputClusterIdLists[0])];
#endif                                  /* #if gZTC_RegEpCapability */
#endif /* gBeeStackIncluded_d */

/******************************************************************************
*******************************************************************************
* Public memory definitions
*******************************************************************************
******************************************************************************/

/* The rest of BeeStack can set this pointer to allow handling of messages
 * from the external client that have opcode groups not normally
 * recognized by Ztc.
 */
#if gZtcIncluded_d
gpfTestClientToApplication_t gpfTestClientToApplicationFuncPtr = NULL;
#endif

#if gZtcIncluded_d
tsTaskID_t gZTCTaskID;
#endif

/******************************************************************************
*******************************************************************************
* Public functions
*******************************************************************************
******************************************************************************/

/* Main routine for the ZTC task.
 *
 * Ztc only understands one event, which signals that the UART ISR has received
 * an octet from a local communication port (e.g. serial port or USB).
 *
 * Note that no messages are ever sent to the Ztc task.
 *
 * The event flags are cleared by the kernel.
 *
 * Note that the ISR declares an event for every octet, so this routine is
 * often called before there is enough of a packet received from the external
 * client to process.
 *
 * Once a complete and well formed packet has arrived, copy to a SAP Handler
 * message, with whatever changes are needed to the format of the data, and
 * forward it to the SAP Handler.
 *
 * The SAP Handler will eventually call ZTC_TaskEventMonitor() to record the
 * SAP Handler's receipt of a message.
 */
void Ztc_Task(event_t events) {
#if !gZtcIncluded_d
  (void) events;
#else
  sapHandlerMsg_t *pMsgToSAPHandler;
  ztcMsgTypeInfo_t const *pMsgTypeInfo;
  ztcOpcodeGroup_t opcodeGroup;
  ztcSAPHandlerInfo_t const *pSAPInfo;
  ztcFmtInfo_t const *pFmtInfo;
  index_t dstDataLen;
  pZtcMsgFromPkt_t pCopyFunc;
  uint8_t status;

  (void) events;

  ZtcCheckRxBufferStatus();
  /* If a complete and valid packet is not received yet, exit */
  if (!ZtcComm_ZtcReadPacketFromClient()) 
  {
    return;
  }

#if gZtcErrorReporting_d
  mZtcErrorReported = FALSE;
#endif

  opcodeGroup = gZtcPacketFromClient.structured.header.opcodeGroup;

  /* *warning* ZtcReq_Handler() depends on it's input data being in the */
  /* global gZtcPacketFromClient, and it modifies gZtcPacketToClient. */
  if (opcodeGroup == gZtcReqOpcodeGroup_c) {
    ZtcReq_Handler();
    return;
  }

  /* special fragmented APSDE commands */
#if (gFragmentationCapability_d)
  if (opcodeGroup == gAfApsdeOpcodeGroup_c && 
      gZtcPacketFromClient.structured.header.msgType != gApsdeDataReqMsgType_d)
  {
    ZtcApsde_Handler();
    return;
  }
#endif

  pSAPInfo = pZtcSAPInfoFromOpcodeGroup(opcodeGroup);

  /* If this is a recognized opcode group, but one which has been disabled */
  /* by an option setting, let the user know. Otherwise, Test Tool users */
  /* may wonder why they are not getting any responses to valid requests. */
#if gZtcErrorReporting_d
  if (pSAPInfo == gZtcIsDisabledOpcodeGroup_c) {
    ZtcError(gZtcOpcodeGroupIsDisabled_c);
    return;
  }
#endif

  /* If the opcode group is not familiar, call a general purpose hook. */
  if (!pSAPInfo) {
    if (gpfTestClientToApplicationFuncPtr) {
      (*gpfTestClientToApplicationFuncPtr)((void *) gZtcPacketFromClient.raw);
    } else {
      ZtcError(gZtcUnknownOpcodeGroup_c);
    }
    return;
  }

  /* The client has sent a well formed packet, with a known opcode group. */
  pMsgTypeInfo = pZtcMsgTypeInfoFromMsgType(pSAPInfo->pMsgTypeTable,
                                            *(pSAPInfo->pMsgTypeTableLen),
                                            gZtcPacketFromClient.structured.header.msgType);
  if (!pMsgTypeInfo) {
    ZtcError(gZtcUnknownOpcode_c);
    return;
  }

  /* The size of the destination message isn't known yet, so allocate */
  /* a large message. */
  pMsgToSAPHandler = (sapHandlerMsg_t *) MSG_Alloc(sizeof(sapHandlerMsg_t));

  if (!pMsgToSAPHandler) {
    ZtcError(gZtcOutOfMessages_c);
    return;
  }

  /* Copy the content of the packet to the SAP Handler message, changing */
  /* formats as necessary. */
  pFmtInfo = &(maZtcFmtInfoTable[ZtcMsgGetFmt(pMsgTypeInfo->flagAndFormat)]);
  pCopyFunc = maMsgFromPktFunctionTable[ZtcGetMsgFromPktFuncIndex(pFmtInfo->copyFunctionIndexes)];

  dstDataLen = (*pCopyFunc)(pMsgToSAPHandler->structured.payload,
                            sizeof(pMsgToSAPHandler->structured.payload),
                            gZtcPacketFromClient.structured.payload,
                            gZtcPacketFromClient.structured.header.len,
                            pFmtInfo->msgFromPktParametersIndex);

  if (dstDataLen == gTooBig_c) {
    MSG_Free(pMsgToSAPHandler);
    ZtcError(gZtcTooBig_c);
    return;
  }

  /* mMsgOriginIsZtc lets ZTC_TaskEventMonitor() know that this message */
  /* originated with Ztc. */
  pMsgToSAPHandler->structured.header.msgType = gZtcPacketFromClient.structured.header.msgType;
  mMsgOriginIsZtc = TRUE;
  status = (pSAPInfo->pSAPHandler)(pMsgToSAPHandler);
  mMsgOriginIsZtc = FALSE;

  /* SAP Handlers don't free buffers that contain synchronous messages. */
  if (ZtcMsgGetFlg(pMsgTypeInfo->flagAndFormat) == gZtcMsgFlagSync_c) {
    #if gSAPMessagesEnableMlme_d
      /* For a MLME.Reset request synchronous message, the memory pool is re-initiated and
         all allocated messages are deallocated. Because of this, we don't need 
         to free the allocated message. */
      if ((opcodeGroup != gNwkMlmeOpcodeGroup_c)||(pMsgTypeInfo->msgType != gMlmeResetReq_c)) {
    #endif
        MSG_Free(pMsgToSAPHandler);
    #if gSAPMessagesEnableMlme_d
      }
    #endif
  } else if (status != gZbSuccess_c) {
    /* This is an async message, and the SAP Handler is unhappy. Synthesize */
    /* a confirmation message containing the status, and send it back to the */
    /* external client. This is the SAP Handler's issue, not a ZtcError(). */
    
    ZTCQueue_QueueToTestClient(&status, pSAPInfo->converseOpcodeGroup, pMsgTypeInfo->cnfType, sizeof(status));
  }
#endif                                  /* #if !gZtcIncluded_d #else */
}                                       /* Ztc_Task() */

/****************************************************************************/

/* Initialization routine for the ZTC task. Called once by the kernel at */
/* startup. */


void Ztc_TaskInit(void) {
#if gZtcIncluded_d
  gZTCTaskID = TS_CreateTask(gTsZTCTaskPriority_c, Ztc_Task);
  ZtcComm_Init();
  gZtcCommTxBlocking = TRUE;
  
#endif
}                                       /* Ztc_TaskInit() */

/****************************************************************************/

/* Send an arbitrary event from an application to the external client. The
 * data, opcode group and message type can be anything.
 */
void ZTCQueue_QueueToTestClient
  (
  uint8_t const*  const pData,
  uint8_t const   opcodeGroup,
  uint8_t const   msgType,
  uint8_t const  dataLength
  )
{
#if !gZtcIncluded_d
  (void) pData;
  (void) opcodeGroup;
  (void) msgType;
  (void) dataLength;
#else

#if gZtcErrorReporting_d
  mZtcErrorReported = FALSE;
#endif

  if (dataLength > sizeof(gZtcPacketToClient.structured.payload)) {
    ZtcError(gZtcAppMsgTooBig_c);
    return;
  }

  if (bZtcPacketToClientIsBusy) {
    return;
  }

  gZtcPacketToClient.structured.header.opcodeGroup    = opcodeGroup;
  gZtcPacketToClient.structured.header.msgType        = msgType;
  gZtcPacketToClient.structured.header.len  = dataLength;

  
  FLib_MemCpy(gZtcPacketToClient.structured.payload, (void *)pData, (uint8_t)dataLength);
  ZtcComm_WritePacketToClient(sizeof(gZtcPacketToClient.structured.header) + dataLength);
#endif                                  /* #if !gZtcIncluded_d #else */
}                                       /* ZTCQueue_QueueToTestClient() */

/****************************************************************************/

/* Called by other functions in BeeStack, generally SAP Handlers.
 * A message is passing through the SAP Handler, and it is letting us know.
 * A SAP Handler can receive a message from a BeeStack layer, or from Ztc.
 * The SAP Handler doesn't know the origin, and calls this function
 * regardless.
 *
 * If the message is async and it was sent to the SAP Handler by Ztc,
 * ignore it.
 *
 * If the message is async and it was not sent to the SAP Handler by Ztc,
 * forward a copy to the external client.
 *
 * Sync messages are always responses to requests. The origin of the request
 * (Ztc or other BeeStack code) does not matter; this routine will only see
 * the response, not the request.
 * Alter the opcode group to make the message look like it is coming from
 * the converse of the SAP Handler that actually sent it here, and change
 * the message type to the type of the corresponding response. Forward the
 * edited message to the external client.
 *
 * Note that the message from the SAP Handler is sent to the external client
 * directly from here, eliminating the need for logic to queue more than
 * one packet.
 */
void ZTC_TaskEventMonitor
  (
  ztcIntSAPId_t const srcIntSAPId,
  uint8_t *pMsgFromSAPHandler,
  clientPacketStatus_t const srcStatus
  )
{
#if !gZtcIncluded_d
  (void) srcIntSAPId;
  (void) pMsgFromSAPHandler;
  (void) srcStatus;
#else
  ztcMsgTypeInfo_t const *pSrcMsgTypeInfo;
  ztcSAPHandlerInfo_t const *pSrcSAPInfo;

#if gZtcErrorReporting_d
  /* Sync messages might be originated by Ztc_Task(), which already set this */
  /* once. Setting it again here could lead to reporting two errors. Since */
  /* are likely to be errors in different contexts, this isn't generally a */
  /* problem. */
  mZtcErrorReported = FALSE;
#endif

  pSrcSAPInfo = pZtcSAPInfoFromIntSAPId(srcIntSAPId);
  if (!pSrcSAPInfo) {
    /* Calling ZtcError() here might just annoy the user, repeatedly */
    /* sending an error message about something they already know. */
    return;
  }

  ZtcPrint(FALSE, &srcStatus, sizeof(srcStatus));
  ZtcPrint(FALSE, &pMsgFromSAPHandler, sizeof(pMsgFromSAPHandler));
  ZtcPrint(FALSE, &pSrcSAPInfo->opcodeGroup, sizeof(pSrcSAPInfo->opcodeGroup));
  ZtcPrint(TRUE,  pMsgFromSAPHandler, 32);

  if (maZtcSAPModeTable[pSrcSAPInfo->modeIndex] == gZtcSAPModeDisable_c) {
    return;
  }

  pSrcMsgTypeInfo = pZtcMsgTypeInfoFromMsgType(pSrcSAPInfo->pMsgTypeTable,
                                               *pSrcSAPInfo->pMsgTypeTableLen,
                                               ((sapHandlerMsg_t *) pMsgFromSAPHandler)->structured.header.msgType);
  if (!pSrcMsgTypeInfo) {
    /* Calling ZtcError() here might just annoy the user, repeatedly */
    /* sending an error message about something they already know. */
    return;
  }

  if (bZtcPacketToClientIsBusy) {
    return;
  }

  if (ZtcMsgGetFlg(pSrcMsgTypeInfo->flagAndFormat) == gZtcMsgFlagAsync_c) {
    pktFromMsgAsync(pMsgFromSAPHandler, pSrcSAPInfo, pSrcMsgTypeInfo);
  } else {
    pktFromMsgSync(pMsgFromSAPHandler, pSrcSAPInfo, pSrcMsgTypeInfo, srcStatus);
  }
#endif  /* #if !gZtcIncluded_d #else */
}  /* ZTC_TaskEventMonitor() */

/****************************************************************************/

#if gZtcIncluded_d
void pktFromMsgAsync
(
	uint8_t const *pMsgFromSAPHandler,
	ztcSAPHandlerInfo_t const *pSrcSAPInfo,
	ztcMsgTypeInfo_t const *pSrcMsgTypeInfo
)
{
	index_t dstDataLen;
	pZtcPktFromMsg_t pCopyFunc;
	ztcFmtInfo_t const *pFmtInfo;
	nwkMessage_t *pMsg=NULL; /* MAC Confirm/Indication data type */

	/* If this message is originated in Ztc, ignore it. */
	if (mMsgOriginIsZtc)
		return;

 	(void)pMsg; /* Set stub the pointer used for the Queue */
  	pFmtInfo = &(maZtcFmtInfoTable[ZtcMsgGetFmt(pSrcMsgTypeInfo->flagAndFormat)]);
	  pCopyFunc = maPktFromMsgFunctionTable[ZtcGetPktFromMsgFuncIndex(pFmtInfo->copyFunctionIndexes)];

	  /* Copy the payload from the message to the client packet. */
	  dstDataLen = (*pCopyFunc)(gZtcPacketToClient.structured.payload,
		  												sizeof(gZtcPacketToClient.structured.payload),
			  											((sapHandlerMsg_t *) pMsgFromSAPHandler)->structured.payload,
				  										pSrcMsgTypeInfo->len,
					  									pFmtInfo->pktFromMsgParametersIndex);  

	/* If the SAP Handler that sent the message here is in hook mode, dequeue */
	/* the message and clear the event to prevent the message from reaching the */
	/* destination SAP Handler. */
	if (maZtcSAPModeTable[pSrcSAPInfo->modeIndex] == gZtcSAPModeHook_c)
	{
		if (pSrcSAPInfo->pMsgQueue)
		{
			/* If there is a Message in the Queue, then pint to it */
			pMsg = MSG_DeQueue(pSrcSAPInfo->pMsgQueue);
 
		  #if gBeeStackIncluded_d
			if (pSrcSAPInfo->opcodeGroup == gNlmeZdoOpcodeGroup_c) /* If the message came from NWK layer */
			{
				if (pSrcMsgTypeInfo->msgType == gNlmeNetworkDiscoveryConfirm_c) /* If the messaage is a Discovery confirm */
				{
					if (((nlmeZdoMessage_t *) pMsg)->msgData.networkDiscoveryConf.networkCount != 0) /* If the internal list of the discovery has some data or point to some place */
					  /*If the discovery confirmation is obtained the NetworkDescriptor is freed*/
            MSG_Free(((nlmeZdoMessage_t *) pMsg)->msgData.networkDiscoveryConf.pNetworkDescriptor );

				}			  
				if (pSrcMsgTypeInfo->msgType == gNlmeEnergyScanConfirm_c) /* If the messaage is a Energy scan confirm */
				{
					  /*If the discovery confirmation is obtained the NetworkDescriptor is freed*/
            MSG_Free(((nlmeZdoMessage_t *) pMsg)->msgData.EnergyScanConf.resList.pEnergyDetectList );
				}			  
				
			}
			#endif
			/* The MAC Layer has some exceptions in some message that need to free some internal memory */
			if (pSrcSAPInfo->opcodeGroup == gMlmeNwkOpcodeGroup_c) /* If the message came from MAC layer */
			{
				if (pSrcMsgTypeInfo->msgType == gNwkScanCnf_c) /* If the messaage is a ScanRequest */
				{
					if (pMsg->msgData.scanCnf.resList.pPanDescriptorBlocks->descriptorList) /* If the internal list of the scan has somem data or point to some place */
						/* Free the internal list comming in the the Scan from the MAC (second block) */
						if(pMsg->msgData.scanCnf.resList.pPanDescriptorBlocks->pNext)
						  MSG_Free(pMsg->msgData.scanCnf.resList.pPanDescriptorBlocks->pNext);
						/* Free the internal list comming in the the Scan from the MAC (first block) */
						MSG_Free(pMsg->msgData.scanCnf.resList.pPanDescriptorBlocks);
				}
				if (pSrcMsgTypeInfo->msgType == gNwkBeaconNotifyInd_c) /* If The messge is a Beacon Notify Indiction it has  messge allocated inside the message already allocted */
				{
					if (pMsg->msgData.beaconNotifyInd.pBufferRoot)
						MSG_Free(pMsg->msgData.beaconNotifyInd.pBufferRoot);
				}
			}
		}

		if (pSrcSAPInfo->pTaskID[0]) /* Take the message out of the Queue for no further processing */
		{
			TS_ClearEvent(pSrcSAPInfo->pTaskID[0], pSrcSAPInfo->msgEvent);
		}
	}

	if (dstDataLen == gTooBig_c)
	{
		ZtcError(gZtcTooBig_c);
		return;
	}

	gZtcPacketToClient.structured.header.opcodeGroup = pSrcSAPInfo->opcodeGroup;
	gZtcPacketToClient.structured.header.msgType = ((sapHandlerMsg_t *) pMsgFromSAPHandler)->structured.header.msgType;
	gZtcPacketToClient.structured.header.len = dstDataLen;

	ZtcComm_WritePacketToClient(sizeof(gZtcPacketToClient.structured.header) + dstDataLen);
	if (pMsg && (maZtcSAPModeTable[pSrcSAPInfo->modeIndex] == gZtcSAPModeHook_c)) {
#if (gFragmentationCapability_d)
  /* if fragmentation is enabled then the packet is not freed here but in the
     ZTC_FragEventMonitor()*/
  if ((pSrcSAPInfo->opcodeGroup == gApsdeAfOpcodeGroup_c) && (pSrcMsgTypeInfo->msgType == gApsdeDataIndMsgType_d)) 
  {
    return;
  } 
#endif  

		MSG_Free(pMsg); /* Free the whole message comming from the Queue */

	}
}  /* pktFromMsgAsync() */
#endif

/****************************************************************************/

/* Handle a synchronous message from a SAP Handler.
 * Sync messages are always requests. Translate this message into the
 * corresponding response.
 *
 * By convention, the request message type info contains the format
 * information.
 */
#if gZtcIncluded_d
void pktFromMsgSync
(
	uint8_t const *pMsgFromSAPHandler,
	ztcSAPHandlerInfo_t const *pSrcSAPInfo,
	ztcMsgTypeInfo_t const *pSrcMsgTypeInfo,
	uint8_t const srcStatus
)
{
	index_t dstDataLen;
	pZtcPktFromMsg_t pCopyFunc;
	ztcFmtInfo_t const *pFmtInfo;
	uint8_t *pSrcData;

	/* For sync messages, a status code was passed here as a parameter, */
	/* and must be stored in the first byte of the packet payload. */
	gZtcPacketToClient.structured.payload[0] = srcStatus;
	pSrcData = ((sapHandlerMsg_t *) pMsgFromSAPHandler)->structured.payload;

	pFmtInfo = &(maZtcFmtInfoTable[ZtcMsgGetFmt(pSrcMsgTypeInfo->flagAndFormat)]);
	pCopyFunc = maPktFromMsgFunctionTable[ZtcGetPktFromMsgFuncIndex(pFmtInfo->copyFunctionIndexes)];

	/* Copy the payload from the message to the client packet. */
	dstDataLen = (*pCopyFunc)(&gZtcPacketToClient.structured.payload[sizeof(clientPacketStatus_t)],
														sizeof(gZtcPacketToClient.structured.payload) - sizeof(clientPacketStatus_t),
														pSrcData,
														pSrcMsgTypeInfo->len,
														pFmtInfo->pktFromMsgParametersIndex);

	if (dstDataLen == gTooBig_c)
	{
		ZtcError(gZtcTooBig_c);
		return;
	}

	gZtcPacketToClient.structured.header.opcodeGroup = pSrcSAPInfo->converseOpcodeGroup;
	gZtcPacketToClient.structured.header.msgType = pSrcMsgTypeInfo->cnfType;
	gZtcPacketToClient.structured.header.len = dstDataLen + sizeof(clientPacketStatus_t);

	ZtcComm_WritePacketToClient(dstDataLen
												+ sizeof(gZtcPacketToClient.structured.header)
												+ sizeof(clientPacketStatus_t));
}  /* pktFromMsgSync() */
#endif

/******************************************************************************
*******************************************************************************
* Private functions
*******************************************************************************
******************************************************************************/
/****************************************************************************
This function checks if there is any bytes in the uart buffer, and 
if there is any left, the ZTC task is signaled.
****************************************************************************/
#if gZtcIncluded_d
extern index_t         bytesReceived;
static void ZtcCheckRxBufferStatus(void) 
{
  if (!ZtcComm_BufferIsEmpty() || bytesReceived) 
  {
    TS_SendEvent(gZTCTaskID, gDataFromTestClientToZTCEvent_c);
  }
}
#endif 

/* Send an error message back to the external client. */
#if gZtcErrorReporting_d
void ZtcError(uint8_t errorCode)
{
  uint8_t buf[MbrSizeof(clientPacket_t, headerAndStatus)];

  if (mZtcErrorReported)
    return;                             /* Don't cascade error messages. */


  ((clientPacket_t *) buf)->headerAndStatus.header.opcodeGroup = gZtcCnfOpcodeGroup_c;
  ((clientPacket_t *) buf)->headerAndStatus.header.msgType = mZtcMsgError_c;
  ((clientPacket_t *) buf)->headerAndStatus.header.len = sizeof(((clientPacket_t *) buf)->headerAndStatus.status);
  ((clientPacket_t *) buf)->headerAndStatus.status = errorCode;

  ZtcComm_TransmitBufferBlock(buf, sizeof(buf));
  mZtcErrorReported = TRUE;
}
#endif                                  /* #if gZtcErrorReporting_d */

/****************************************************************************/

/* Ztc's version of printf() debugging.
 *
 * To save RAM, use a dynamically allocated message buffer to construct
 * the message. If no buffer is available, send an error buffer and give up.
 *
 * Write as much of the requested data as possible to the buffer, but there
 * is no guarantee there the buffer will be big enough.
 *
 * Concatenate the source data to the print buffer, then
 *  If readyToSend is false, do nothing.
 *  If readyToSend is true,  send the buffer to the external client.
 */
#if gZtcDebug_d
void ZtcPrint
  (
  bool_t readyToSend,
  void const *pSrc,
  index_t len
  )
{
  static index_t bufLen = 0;
  static index_t dataInBuf = 0;
  static uint8_t *pBuf = NULL;

  /* Search for a valid and available message buffer size, instead of */
  /* depending on knowing the possible buffer sizes. */
#define ztcPrintDecrement       8       /* Ensure decrementing to zero. */
#define ztcPrintNumberOfTries   8
  if (!bufLen) {
    for (pBuf = 0, bufLen = ztcPrintDecrement * ztcPrintNumberOfTries;
         !pBuf && bufLen;
         bufLen -= ztcPrintDecrement) {
      pBuf = MSG_Alloc(bufLen);
      dataInBuf = sizeof(clientPacketHdr_t);
    }

    if (!bufLen) {
      ZtcError(gZtcDebugPrintFailed_c);
      return;
    }
  }

  /* Concatenate the source data to the print buffer, if there's room. */
  if (dataInBuf + len < bufLen) {
    FLib_MemCpy(pBuf + dataInBuf, (void*)pSrc, len);
    dataInBuf += len;
  }

  if (readyToSend) {
    ((clientPacketHdr_t *) pBuf)->opcodeGroup = gZtcCnfOpcodeGroup_c;
    ((clientPacketHdr_t *) pBuf)->msgType = mZtcMsgDebugPrint_c;
    ((clientPacketHdr_t *) pBuf)->len = dataInBuf - sizeof(clientPacketHdr_t);
    ZtcComm_TransmitBufferBlock(pBuf, dataInBuf);
    MSG_Free(pBuf);
    bufLen = 0;
  }
}
#endif                                  /* #if gZtcDebug_d */


/*
  Called from both AF_APSDE_SapHandler() and APSDE_AF_SapHandler(), this function will 
  display all fragmented blocks to ZTC if both fragmentation and ZTC are enabled. 
*/
void ZTC_FragEventMonitor
  (
  ztcIntSAPId_t const srcIntSAPId,
  uint8_t *pMsgFromSAPHandler
  )
{
#if (!gZtcIncluded_d)
  /* avoid compiler warning on unused parameters */
  (void)srcIntSAPId;
  (void)pMsgFromSAPHandler;

/* ZTC is included */
#else
  uint8_t msgType;
 #if (gFragmentationCapability_d)
  zbTxFragmentedHdr_t *pFragHdr;
  apsdeFragment_t sZtcFragment;
  uint8_t *pData;
  uint8_t dataSize;
  uint8_t opCodeGroup;
  uint8_t iBlockNumber;
 #endif

#if (gSAPMessagesDisableApsdeEndpoint0Monitoring_d)  
  /*If gSAPMessagesDisableApsdeEndpoint0Monitoring_d is set to TRUE the apdse data request, data indications and confirms
    sent and received on endpoint 0x0 is NOT monitored by ZTC.
    This mode is reduces traffic to host process and is ideal for a Black box type application.
  */
  if(srcIntSAPId == gAF_APSDE_SAPHandlerId_c)
  {
    /*Filter out apsde data requests sent on source endpoint 0 */
    if (((afToApsdeMessage_t*) pMsgFromSAPHandler)->msgData.dataReq.srcEndPoint == 0)
    {
      return;
    }
  }
  
  if(srcIntSAPId == gAPSDE_AF_SAPHandlerId_c)
  { /* Filter out apsde data indication received on destination endpoint 0 */
    if ((((apsdeToAfMessage_t*) pMsgFromSAPHandler)->msgType == gApsdeDataIndMsgType_d) &&
       (((apsdeToAfMessage_t*) pMsgFromSAPHandler)->msgData.dataIndication.dstEndPoint == 0))
    {
      return;
    }
   /*remove data confirms which matches data request sent on source endpoint 0 */
  if ((((apsdeToAfMessage_t*) pMsgFromSAPHandler)->msgType == gApsdeDataCnfMsgType_d) &&
      (((apsdeToAfMessage_t*) pMsgFromSAPHandler)->msgData.dataConfirm.srcEndPoint == 0))
    {
    return;
    }
  }
#endif  

  /* first, send the main message to ZTC */
  ZTC_TaskEventMonitor(srcIntSAPId, (uint8_t *) pMsgFromSAPHandler, gZbSuccess_c);

 #if (!gFragmentationCapability_d)
  /* if this a data request just increment confirmId */
  msgType = *pMsgFromSAPHandler;
  if(srcIntSAPId == gAF_APSDE_SAPHandlerId_c)
  {
    /* make sure message is a data request */
    if(msgType != gApsdeDataReqMsgType_d)
      return;
  }
 #else
  /* fragmentation is enable set the confirmation id and prepare for fragments */
  /* find fragHdr for this message */
  /* the message will either come from the gAF_APSDE_SAPHandlerId_c or the gAPSDE_AF_SAPHandlerId_c */
  /* note: this code assumes both zbTxFragmentedHdr_t and zbRxFragmentedHdr_t begin with same 2 fields */
  msgType = *pMsgFromSAPHandler;
  if(srcIntSAPId == gAF_APSDE_SAPHandlerId_c)
  {
    /* make sure message is a data request */
    if(msgType != gApsdeDataReqMsgType_d)
      return;

    /* find first fragment header */
    pFragHdr = &(((afToApsdeMessage_t *)pMsgFromSAPHandler)->msgData.dataReq.fragmentHdr);
    opCodeGroup = 0x9c;
  }

  /* assumes srcIntSAPId == gAPSDE_AF_SAPHandlerId_c */
  else
  {
    /* make sure message is a data indication */
    if(msgType != gApsdeDataIndMsgType_d)
      return;

    /* find first fragment header */
    pFragHdr = (zbTxFragmentedHdr_t *)(&(((apsdeToAfMessage_t *)pMsgFromSAPHandler)->msgData.dataIndication.fragmentHdr));
    opCodeGroup = 0x9d;
  }

  /* skip first block (with dataReq or dataInd), as that's already been sent to ZTC */
  pFragHdr = pFragHdr->pNextDataBlock;

  /* send all subsequent blocks to ZTC */
  iBlockNumber = 1;
  while(pFragHdr)
  {
    /* copy data into ZTC Tx buffer */
    dataSize = pFragHdr->iDataSize;
    gZtcPacketToClient.structured.header.opcodeGroup = opCodeGroup;
    gZtcPacketToClient.structured.header.msgType = pFragHdr->iMsgType;
    gZtcPacketToClient.structured.header.len = (sizeof(apsdeFragment_t) - 1) + dataSize;
    sZtcFragment.iLength = dataSize;
    sZtcFragment.iBlockNumber = iBlockNumber++;
    sZtcFragment.iMoreBlocks = pFragHdr->pNextDataBlock ? TRUE : FALSE;
    gZtcPacketToClient.structured.payload[0] = dataSize;
    if(srcIntSAPId == gAF_APSDE_SAPHandlerId_c)
      pData = (uint8_t *)(pFragHdr + 1);    /* on Tx, data immediately follows TxFragHdr */
    else
      pData = ((zbRxFragmentedHdr_t *)pFragHdr)->pData; /* on Rx, it's pointed to by RxFragHdr */
    FLib_MemCpy(gZtcPacketToClient.structured.payload, &sZtcFragment, sizeof(apsdeFragment_t) - 1);
    FLib_MemCpy(&gZtcPacketToClient.structured.payload[sizeof(apsdeFragment_t) - 1], pData, dataSize);

    /* send through ZTC */
	  ZtcComm_WritePacketToClient(sizeof(gZtcPacketToClient.structured.header) + 
	    (sizeof(apsdeFragment_t) - 1) + dataSize);

    /* go on to next block */
    pFragHdr = pFragHdr->pNextDataBlock;
  }
  /*A Data indication is not freed in the lower layer of ZTC when apsde is hooked as it can contain fragments
  therefor it is handled here:
   */
  if ((maZtcSAPModeTable[gSAPModeIndexApsde_c] == gZtcSAPModeHook_c)  && (msgType == gApsdeDataIndMsgType_d))
  { 
    AF_FreeDataIndicationMsg((apsdeToAfMessage_t*)pMsgFromSAPHandler);
  }
 #endif /* gFragmentationCapability_d */
#endif  /* gZtcIncluded_d */
}

#if (gZtcIncluded_d && gFragmentationCapability_d)

/* for loading fragments prior to a data request */
static zbTxFragmentedHdr_t *mpFragHead;  /* if NULL, then no fragments loaded */

/*
  For handling the "special" APSDE commands for fragmentation. These do not go to the SAP 
  handler.

  ResetFragments   Free any memory allocated by LoadFragment. Only required if aborting a
                   Data request, or if
  LoadFragment     Load a fragment into memory through the ZTC interface.
  SetFragAckMask   Used for passing ZCP tests. Pretend a packet has already been ACKed so
                   it's not sent first time.
    
  Normal sequence for initiating an APSDE-DATA.request over ZTC is:
  1. Call LoadFragment (for all fragments but the first one. These fragments may be any length, 
     but should be the max size that will fit in a message (128 bytes) 
  2. Call APSDE-DATA.request for the first fragment. AsduLen should be the length of the first
     fragment only. ZTC will add the total size of all fragments.

  Note: the above sequence is different than from an application. The AF_DataRequestFragmented()
  and AF_DataRequestFragmentedNoCopy() should be called instead.

  Note: ZtcApsde_Handler() receives data in the global gZtcPacketFromClient, and modifies 
  gZtcPacketToClient.
*/
static void ZtcApsde_Handler(void)
{

  /* these commands do not actually go through the SAP, but are valid only in ZTC */
  #define gZtcMsgTypeResetFragments_c       0x10
  #define gZtcMsgTypeLoadFragment_c         0x11
  #define gZtcMsgTypeSetFragAckMask_c       0x12
  #define gZtcMsgTypeSetMaxFragmentLength_c 0x13
  #define gZtcMsgTypeSetWindowSize_c        0x14
  #define gZtcMsgTypeSetInterframeDelay_c   0x15

  zbTxFragmentedHdr_t *pFragBlock;    /* allocated message */
  zbTxFragmentedHdr_t *pFragNext;     /* next fragmented buffer */
  uint8_t fragLen;
  ztcMsgType_t msgType;
  clientPacketStatus_t ztcError = gZtcSuccess_c;

  msgType = gZtcPacketFromClient.structured.header.msgType;
  switch(msgType)
  {
    /* ResetFragments command. Will free any memory allocated with LoadFragment command. */
    case gZtcMsgTypeResetFragments_c:

      /* free every buffer in the linked list */
      while(mpFragHead)
      {
        pFragNext = mpFragHead->pNextDataBlock;
        MSG_Free(mpFragHead);
        mpFragHead = pFragNext;
      }
      /* note: mpFragHead will end up NULL */
    break;

    /* LoadFragment command */
    case gZtcMsgTypeLoadFragment_c:

      /* allocate a message buffer for the fragment */
      fragLen = gZtcPacketFromClient.structured.header.len - 1;
      pFragBlock = MSG_Alloc(sizeof(zbTxFragmentedHdr_t) + fragLen);
      if(!pFragBlock)
      {
        ztcError = gZtcOutOfMessages_c;
        break;
      }

      /* find tail, and link in allocated buffer at end */
      if(!mpFragHead)
        mpFragHead = pFragBlock;
      else
      {
        /* find tail */
        pFragNext = mpFragHead;
        while(pFragNext->pNextDataBlock)
          pFragNext = pFragNext->pNextDataBlock;
        pFragNext->pNextDataBlock = pFragBlock;
      }

      /* set up allocated buffer */
      pFragBlock->iDataSize = fragLen;
      pFragBlock->iMsgType = gApsdeDataReqFragMsgType_d;
      pFragBlock->pNextDataBlock = NULL;

      /* copy actual octets (they immediately follow the header) */
      FLib_MemCpy((uint8_t *)pFragBlock + sizeof(zbTxFragmentedHdr_t), 
          &(gZtcPacketFromClient.structured.payload[1]), fragLen);
    break;

    /* set the fragmentation mask (for debugging and ZCP testing) */
    case gZtcMsgTypeSetFragAckMask_c:
      giFragmentedPreAcked = *gZtcPacketFromClient.structured.payload;
    break;

    /* set the maximum OTA fragment length */
    case gZtcMsgTypeSetMaxFragmentLength_c:
      ApsmeSetRequest(gApsMaxFragmentLength_c, *gZtcPacketFromClient.structured.payload);
    break;

    /* set the window size (1-8) */
    case gZtcMsgTypeSetWindowSize_c:
      ApsmeSetRequest(gApsMaxWindowSize_c, *gZtcPacketFromClient.structured.payload);
    break;

    /* set the interframe delay 1-255 milliseconds */
    case gZtcMsgTypeSetInterframeDelay_c:
      ApsmeSetRequest(gApsInterframeDelay_c, *gZtcPacketFromClient.structured.payload);
    break;

    /* unknown opcode (aka msgType) */
    default:
      ztcError = gZtcUnknownOpcode_c;
    break;
  }

  /* send response */
  gZtcPacketToClient.structured.header.opcodeGroup = gApsdeAfOpcodeGroup_c;
  gZtcPacketToClient.structured.header.msgType = msgType;
  gZtcPacketToClient.structured.header.len = sizeof(clientPacketStatus_t);
  gZtcPacketToClient.structured.payload[0] = ztcError;
  ZtcComm_WritePacketToClient(sizeof(gZtcPacketToClient.structured.header)
                               + gZtcPacketToClient.structured.header.len);

}
#endif // #if (gZtcIncluded_d && gFragmentationCapability_d)


#if (gZtcIncluded_d && gBeeStackIncluded_d)
/*
  Called on inbound APSDE data requests

  This function both services APSDE-DATA.requests
*/
index_t ZtcMsgFromPktApsde
  (
  uint8_t *pMsg,                        /* Pointer to message payload (just after msgType byte) */
  index_t msgLen,                       /* Length  of message payload */
  uint8_t *pPkt,                        /* Pointer to packet  payload */
  index_t pktLen,                       /* Length  of packet  payload */
  index_t fmtParametersIndex
  )
{
  zbApsdeDataReq_t *pDataReq;
  uint8_t asduLen;
  apsdeToAfMessage_t confirmMsg;

  /* unused parameters, mark so no compiler warnings */
  (void)msgLen;
  (void)fmtParametersIndex;

  /* copy in 1st part header (up to asdu) */
  pDataReq = (void *)pMsg;
  FLib_MemCpy(pDataReq, pPkt, MbrOfs(zbApsdeDataReq_t, pAsdu));

  /* asdu too large? we're done */
  asduLen = pktLen - (sizeof(zbApsdeDataReq_t) - sizeof(zbTxFragmentedHdr_t) - sizeof(uint8_t *) - sizeof(zbApsCounter_t));
  pDataReq->asduLength = asduLen;

  /* copy in 2nd part of header */
  FLib_MemCpy(&pDataReq->txOptions, pPkt + MbrOfs(zbApsdeDataReq_t, pAsdu) + asduLen, 
    MbrOfs(zbApsdeDataReq_t, fragmentHdr) - MbrOfs(zbApsdeDataReq_t, txOptions));
  BeeUtilZeroMemory(&pDataReq->fragmentHdr, sizeof(pDataReq->fragmentHdr));

  /* copy in ASDU */
  pDataReq->pAsdu = (void *)((pMsg - 1) + ApsmeGetAsduOffset());
  FLib_MemCpy(pDataReq->pAsdu, pPkt + MbrOfs(zbApsdeDataReq_t, pAsdu), asduLen);

  /* Get a valid confirm ID. */
  pDataReq->confirmId = ApsGetConfirmId();
  APS_IncConfirmId();

  /* Report the confirm ID back in here. */
  confirmMsg.msgType = gApsdeConfirmIdMsgType_d;
  confirmMsg.msgData.confirmId = pDataReq->confirmId;
  ZTC_TaskEventMonitor(gAPSDE_AF_SAPHandlerId_c, (void *)&confirmMsg, gZbSuccess_c);

  /* set up fragmented part */
#if (gFragmentationCapability_d)
  pDataReq->fragmentHdr.pNextDataBlock = mpFragHead;
  pDataReq->fragmentHdr.iDataSize = asduLen;
  pDataReq->fragmentHdr.iMsgType = gApsdeDataReqMsgType_d;

  /* mark frag head as NULL for next data request */
  /* all buffers in linked list will be freed by APSDE-DATA.request itself */
  mpFragHead = NULL;
#else
  pDataReq->fragmentHdr.pNextDataBlock = NULL;
  pDataReq->fragmentHdr.iDataSize = asduLen;
  pDataReq->fragmentHdr.iMsgType = gApsdeDataReqMsgType_d;
#endif

  return 1; // everything is OK
}
#endif /* (gZtcIncluded_d && gBeeStackIncluded_d) */

/****************************************************************************/

/* Handle Ztc configuration commands/requests from the external client.
 * This code would be easier to read, but slower and larger, if each
 * case called a function instead of processing in-line.
 *
 * On arrival, the opcodeGroup in the pMsg has been changed to the opcode
 * group used for Ztc responses.
 *
 * The functions that actually handle the packets are responsible for the
 * contents of the return packet, except for the return packet's opcode
 * group. The message type of the return packet will be unchanged from the
 * request packet.
 *
 * *warning* ZtcReq_Handler() receives data in the global gZtcPacketFromClient,
 * and modifies gZtcPacketToClient.
 */
#if gZtcIncluded_d
void ZtcReq_Handler(void) {
  ztcMsgType_t msgType = gZtcPacketFromClient.structured.header.msgType;
  index_t i;

  if (bZtcPacketToClientIsBusy) 
  {
    return;
  }

  for (i = 0; i < gZtcInternalMsgTypeInfoTableLen; ++i) {
    if (gaZtcInternalMsgTypeInfoTable[i].msgType == msgType) {

      gZtcPacketToClient.structured.header.len = sizeof(clientPacketStatus_t);
      gZtcPacketToClient.structured.payload[0] = gZtcSuccess_c;

      (*gaZtcInternalMsgTypeInfoTable[i].pConfigFunc)();

#if gZtcErrorReporting_d
      if (!mZtcErrorReported)
#endif
    
       /* Set default values. */
      gZtcPacketToClient.structured.header.opcodeGroup = gZtcCnfOpcodeGroup_c;
      gZtcPacketToClient.structured.header.msgType = msgType;
        ZtcComm_WritePacketToClient(sizeof(gZtcPacketToClient.structured.header)
                               + gZtcPacketToClient.structured.header.len);

      return;
    }
  }

  ZtcError(gZtcUnknownOpcode_c);
} 

#endif
/****************************************************************************/ 
static void ZtcMsgSetCnfStatus(uint8_t status)
{
  gZtcPacketToClient.structured.header.len = sizeof(clientPacketStatus_t);
  gZtcPacketToClient.structured.payload[0] = status;
}
                                     /* ZtcReq_Handler() */

/****************************************************************************/
#if gBeeStackIncluded_d

#if gZtcIncluded_d
void ZtcMsgAFResetReqFunc(void) {
  TS_AfTaskInit();
  ZtcMsgSetCnfStatus(gZtcSuccess_c);
}                                       /* ZtcMsgAFResetReqFunc() */
#endif

/****************************************************************************/

/****************************************************************************/

#if gZtcIncluded_d
void ZtcMsgAPSResetReqFunc(void) {
  apsmeMessage_t apsmeMessage;

  apsmeMessage.msgType = gApsmeReset_c;
  apsmeMessage.msgData.resetTypeReq = gZtcPacketFromClient.structured.payload[0];
  (void) ZDO_APSME_SapHandler(&apsmeMessage);
  ZtcMsgSetCnfStatus(gZtcSuccess_c);
}                                       /* ZtcMsgAPSResetReqFunc() */
#endif

#endif /* gBeeStackIncluded_d */

/****************************************************************************/
#if gBeeStackIncluded_d

#if gZtcIncluded_d
typedef struct ZtcGetEndPointDescriptionPkt_tag {
  zbEndPoint_t endPoint;
} ZtcGetEndPointDescriptionPkt_t;

void ZtcMsgDeregisterEndPointFunc(void) {
#if !gZTC_RegEpCapability
  ZtcError(gZtcRequestIsDisabled_c);
#else
  index_t i;
  ZtcGetEndPointDescriptionPkt_t *pScratch;
  uint8_t status;

  pScratch = (ZtcGetEndPointDescriptionPkt_t *) gZtcPacketFromClient.structured.payload;
  status = AF_DeRegisterEndPoint(pScratch->endPoint);

  if (status == gZbSuccess_c) {
    for (i = 0; i < NumberOfElements(maSimpleDescriptors); i++) {
      if (maSimpleDescriptors[i].endPoint == pScratch->endPoint) {
        maSimpleDescriptors[i].endPoint = 0;
        break;
      }
    }
  }

  ZtcMsgSetCnfStatus(status);
#endif                                  /* #if !gZTC_RegEpCapability */
}                                       /* ZtcMsgDeregisterEndPointFunc() */
#endif

#endif /* gBeeStackIncluded_d */

/****************************************************************************/

/* Note that gApsCurrentChannelMask is stored little-endian. */
/* Channel numbers start at 0. */
#if gBeeStackIncluded_d
#if gZtcIncluded_d
void ZtcMsgGetChannelReqFunc(void) {

  uint8_t channelNumber = 0;

  channelNumber = NlmeGetRequest(gNwkLogicalChannel_c);
  gZtcPacketToClient.structured.payload[sizeof(clientPacketStatus_t)] = channelNumber;
  gZtcPacketToClient.structured.header.len = sizeof(clientPacketStatus_t)
                                           + sizeof(gZtcPacketToClient.structured.payload[0]);
}                                       /* ZtcMsgGetChannelReqFunc() */
#endif

/****************************************************************************/

#if gZtcIncluded_d
void ZtcMsgGetEndPointDescriptionFunc(void) {
#if !gZTC_RegEpCapability
  ZtcError(gZtcRequestIsDisabled_c);
#else
  index_t dstPacketLen;
  ZtcGetEndPointDescriptionPkt_t *pGetEndPointDescPkt;
  zbSimpleDescriptorPackageStore_t *pSimple;

  pGetEndPointDescPkt = (ZtcGetEndPointDescriptionPkt_t *) gZtcPacketFromClient.structured.payload;
  pSimple = (zbSimpleDescriptorPackageStore_t *) AF_FindEndPointDesc(pGetEndPointDescPkt->endPoint);
  if (!pSimple) {
    ZtcError(gZtcEndPointNotFound_c);
    return;
  }

  dstPacketLen = ZtcPktFromMsgTwoArrayPtrs(&gZtcPacketToClient.structured.payload[sizeof(clientPacketStatus_t)],
                                           sizeof(gZtcPacketToClient.structured.payload)
                                           - sizeof(clientPacketStatus_t),
                                           (uint8_t *) pSimple,
                                           gUnused_c,
                                           mFmtZTCRegisterEndPointReq_c);

  if (dstPacketLen == gTooBig_c) {
    ZtcError(gZtcTooBig_c);
  } else {
    gZtcPacketToClient.structured.header.len = dstPacketLen + sizeof(clientPacketStatus_t);
  }
#endif                                  /* #if !gZTC_RegEpCapability #else */
}                                       /* ZtcMsgGetEndPointDescriptionFunc() */

void ZtcMsgGetEpMaxWindowSizeFunc(void)
{
#if !gZTC_RegEpCapability
  ZtcError(gZtcRequestIsDisabled_c);
#else
  zbMaxWindowSize_t  maxWindowSize;

  /* The first byte contains the End point id */
  maxWindowSize = AF_GetEpMaxWindowSize(gZtcPacketFromClient.structured.payload[0]);

  /* If the End point is invalid or can not be found report the error. */
  if (maxWindowSize == 0xFF)
  {
    ZtcError(gZtcEndPointNotFound_c);
    return;
  }

  /* Build the confirm to be send back by setting the size of the data plus the status. */
  gZtcPacketToClient.structured.header.len = sizeof(zbMaxWindowSize_t) + sizeof(clientPacketStatus_t);

  /* The first byte must be status, so send a no error status. */
  gZtcPacketToClient.structured.payload[0] = gZbSuccess_c;

  /* For some reason not specified, the first byte is the status nad by some mean it remains in zero. */
  gZtcPacketToClient.structured.payload[1] = maxWindowSize;
#endif
}

/*
  Reads form the APS secure material, the information assoicated to given IEEE address.

  The confirm to be send back has the form:

  On success only:
  [IEEE address][Key Tpe][Incoming Frame Counter][Outgoing Frame Counter][Key in Use]
*/
void ZtcMsgGetApsDeviceKeyPairSet(void)
{
#if !gApsLinkKeySecurity_d
  ZtcError(gZtcRequestIsDisabled_c);
#else
  uint8_t *pIeeeAddress;
  zbApsDeviceKeyPairSet_t  *pSecurityMaterial;
  uint8_t *pPtr;
  zbStatus_t  status = gZbSuccess_c;
  zbApsDeviceKeyPairSet_t whereToCpyEntry;

  /* the packet paylod is the I */
  pIeeeAddress = (void *)&gZtcPacketFromClient.structured.payload[0];

  pSecurityMaterial = SSP_ApsGetSecurityMaterilaEntry(pIeeeAddress, &whereToCpyEntry);

  if (!pSecurityMaterial)
  {
    /* Halt no info to send back. */
    status = gZtcError_c;
  }

  if (pSecurityMaterial->iKey == 0xFF)
  {
    /* Halt no info to send back.! */
    status = gZtcError_c;
  }

  /* Calculate thesize to be send back */
  gZtcPacketToClient.structured.header.len = sizeof(zbIeeeAddr_t) + sizeof(zbKeyType_t) + sizeof(zbAESKey_t) + (sizeof(zbFrameCounter_t) * 2);

  /* Halt, send the confirm back. */
  if (status != gZbSuccess_c)
  {
    /* Clear the packet to report no info. */
    BeeUtilZeroMemory(gZtcPacketToClient.structured.payload, gZtcPacketToClient.structured.header.len);
    return;
  }

  /*
    The packet to send back will have the form:
    IEEE Address.
    Key Type (if any)
    The Incomming counter.
    The Outgoing counter.
    The key (if any)
  */
  pPtr = gZtcPacketToClient.structured.payload;

  /* Build the confirm using the above description. */
  Copy8Bytes(pPtr, pIeeeAddress);
  pPtr+= sizeof(zbIeeeAddr_t);
  ((zbKeyType_t *)pPtr)[0] = pSecurityMaterial->keyType;
  pPtr += sizeof(zbKeyType_t);
  FLib_MemCpy(pPtr, &pSecurityMaterial->IncomingFrameCounter, sizeof(zbFrameCounter_t));
  pPtr += sizeof(zbFrameCounter_t);
  FLib_MemCpy(pPtr, &pSecurityMaterial->OutgoingFrameCounter, sizeof(zbFrameCounter_t));
  pPtr += sizeof(zbFrameCounter_t);
  (void)KeySet_GetTableEntry(pSecurityMaterial->iKey,(zbAESKey_t *)pPtr);
#endif
}

/*
  Fill a secure material entry if possible for the given address.

  The confirm to send back has onlytwo possible status.
  - 0x00 success.
  - 0xb1 table full (make sno difference between secure maerial, key table or address map,
                     they are attached between them).
*/
void ZtcMsgSetApsDeviceKeyPairSet(void)
{
#if !gApsLinkKeySecurity_d
  ZtcError(gZtcRequestIsDisabled_c);
#else
  uint8_t *pIeeeAddress;
  zbApsDeviceKeyPairSet_t  *pSecurityMaterial;
  zbKeyType_t *pKeyType;
  uint8_t *pKey;
  zbStatus_t status = gZbSuccess_c;
  zbApsDeviceKeyPairSet_t whereToCpyEntry;

  addrMapIndex_t index;

  /* The first thing in the packet paylod is the IEEE address of the device to be added. */
  pIeeeAddress = (void *)&gZtcPacketFromClient.structured.payload[0];

  /* The next thing is the key type to be added. */
  pKeyType = (void *)((uint8_t *)pIeeeAddress + sizeof(zbIeeeAddr_t));

  /* Then the very last thing is the key */
  pKey = (void *)((uint8_t *)pKeyType + sizeof(zbKeyType_t));

  /*
    Check if the current device is already on our address map, just to avoids re-adding it
    with an invalid short address.
  */
  index = APS_FindIeeeInAddressMap(pIeeeAddress);
  if (index == gNotInAddressMap_c)
  {
    /*
      NOTE: we can not do as the function SSP_ApsRegisterLinkKeyData does, and serach on NT
      for the current device, because this node may be the TC, or the network may be too big,
      if we dont know the node the short address will be null and we will hope to update
      it later.
    */
    index = APS_AddToAddressMapPermanent(pIeeeAddress, (void *)gaBroadcastAddress);
    if (index == gAddressMapFull_c)
    {
      /* Catch the error. */
      status = gZbTableFull_c;
    }
  }

  /* At this point we have an index in the address map, lets start filling the info. */
  APS_RegisterLinkKeyData(pIeeeAddress, *pKeyType, pKey);

  /* The function above may fail but it does not returns any errors, so we just check.. just in case. */
  pSecurityMaterial = SSP_ApsGetSecurityMaterilaEntry(pIeeeAddress, &whereToCpyEntry);

  /* Unable to read the secure material, report table full any way.! */
  if (!pSecurityMaterial)
  {
    status = gZbTableFull_c;
  }


  /* So far so good everything seems to be wroking lets build the confirm. */

  /*
    The packet to send back will have the form:
    Status - 0x00 success.
           - 0xb1 table full (make sno difference between secure maerial, key table or
                              address map, they are attached between them).
  */
  ZtcMsgSetCnfStatus(status);
#endif
}

/*
  Removes form the secure material and the address map the info related to the given IEEE
  address.

  Confirm is only success this command can not fail.
*/
void ZtcMsgClearApsDeviceKeyPairSet(void)
{
#if !gApsLinkKeySecurity_d
  ZtcError(gZtcRequestIsDisabled_c);
#else
  uint8_t *pIeeeAddress;

  /* The first thing in the packet paylod is the IEEE address of the device to be added. */
  pIeeeAddress = (void *)&gZtcPacketFromClient.structured.payload[0];

  /* Clear the Aps secure material (if any) */
  APS_RemoveSecurityMaterialEntry(pIeeeAddress);

  /* Clear the address map table. */
  APS_RemoveFromAddressMap(pIeeeAddress);

  /*
    The packet to send back will have the form:
    Status - 0x00 success.
  */
  ZtcMsgSetCnfStatus(gZtcSuccess_c);
#endif
}


#endif

/****************************************************************************/

#if gZtcIncluded_d
void ZtcMsgGetEndPointIdListFunc(void) {
#if !gZTC_RegEpCapability
  ZtcError(gZtcRequestIsDisabled_c);
#else
  index_t numberOfEndPointsInList;
  zbEndPoint_t *pEndPointList;
  index_t listLen;                      /* Risky on an 8 bit CPU. */
  /* This is needed to avoid a compiler "multiply by 1" warning. */
  index_t sizeofEndPoint = sizeof(*pEndPointList);

  pEndPointList = AF_EndPointsList(&numberOfEndPointsInList);
  if (!pEndPointList || !numberOfEndPointsInList) {
    ZtcError(gZtcEndPointNotFound_c);
    return;
  }

  listLen = numberOfEndPointsInList * sizeofEndPoint;
  if (listLen + sizeof(clientPacketStatus_t) > sizeof(gZtcPacketToClient.structured.payload)) {
    ZtcError(gZtcTooBig_c);
    return;
  }

  /* The first location is fo rthe status. */
  gZtcPacketToClient.structured.payload[0] = gZbSuccess_c;

  /* The we must have the length to be able to display the array. */
  gZtcPacketToClient.structured.payload[sizeof(clientPacketStatus_t)] = listLen;

  /* Then the list it self. */
  FLib_MemCpy(&gZtcPacketToClient.structured.payload[sizeof(clientPacketStatus_t) + sizeof(listLen)],
              pEndPointList,
              listLen);

  gZtcPacketToClient.structured.header.len = listLen + sizeof(clientPacketStatus_t) + sizeof(zbStatus_t);
#endif                                  /* #if !gZTC_RegEpCapability #else */
}                                       /* ZtcMsgGetEndPointIdListFunc() */
#endif

/****************************************************************************/

#if gZtcIncluded_d && gICanHearYouTableCapability_d
void ZtcMsgGetICanHearYouListFunc(void) {
  ztcICanHearYouTable_t *pTable = (ztcICanHearYouTable_t *) &gZtcPacketToClient.structured.payload[sizeof(clientPacketStatus_t)];
  pTable->count = GetICanHearYouTable(pTable->table, NumberOfElements(pTable->table));
  gZtcPacketToClient.structured.header.len = sizeof(clientPacketStatus_t)
                                           + sizeof(pTable->count)
                                           + (pTable->count * sizeof(zbNwkAddr_t));
}                                       /* ZtcMsgGetICanHearYouListFunc() */
#endif

#endif /* gBeeStackIncluded_d */
/****************************************************************************/

#if gZtcIncluded_d
void ZtcMsgGetModeReqFunc(void) {
  uint8_t i;
  index_t payloadIndex = sizeof(clientPacketStatus_t);

  gZtcPacketToClient.structured.payload[payloadIndex++] = (uint8_t) gZtcCommTxBlocking;

  for (i = 0; i < mZtcSAPModeTableLen; ++i) {
    gZtcPacketToClient.structured.payload[payloadIndex++] = (uint8_t) maZtcSAPModeTable[i];
  }

  gZtcPacketToClient.structured.header.len = payloadIndex;
}                                       /* ZtcMsgGetModeReqFunc() */
#endif

/****************************************************************************/
#if gBeeStackIncluded_d

#if gZtcIncluded_d
void ZtcMsgGetNumberOfEndPointsFunc(void) {
  gZtcPacketToClient.structured.payload[sizeof(clientPacketStatus_t)] = AF_NumOfEndPoints();
  gZtcPacketToClient.structured.header.len = sizeof(clientPacketStatus_t)
                                           + sizeof(gZtcPacketToClient.structured.payload[0]);
}                                       /* ZtcMsgGetNumberOfEndPointsFunc() */
#endif
#endif /* gBeeStackIncluded_d */

/****************************************************************************/

#if gZtcIncluded_d
void ZtcMsgGetNVDataSetDescReqFunc(void) {
  index_t payloadIndex = sizeof(clientPacketStatus_t);
#if !gNvStorageIncluded_d
  gZtcPacketToClient.structured.payload[payloadIndex++] = 0;
#else
#if !gArm7ExtendedNVM_d
  index_t dataSetIndex;
  NvDataItemDescription_t const *pDesc;
  uint16_t size;

  gZtcPacketToClient.structured.payload[payloadIndex++] = NumberOfElements(NvDataSetTable);

  for (dataSetIndex = 0; dataSetIndex < NumberOfElements(NvDataSetTable); ++dataSetIndex) {
    gZtcPacketToClient.structured.payload[payloadIndex++] = NvDataSetTable[dataSetIndex].dataSetID;

    pDesc = NvDataSetTable[dataSetIndex].pItemDescriptions;
    size = 0;
    while (pDesc->length) {
      size += pDesc++->length;
    }

    gZtcPacketToClient.structured.payload[payloadIndex++] = (size & 0xFF);
    gZtcPacketToClient.structured.payload[payloadIndex++] = (size >> 8);
  }
#else
  gZtcPacketToClient.structured.payload[payloadIndex++] = 0;
#endif
#endif
  gZtcPacketToClient.structured.header.len = payloadIndex;
}                                       /* ZtcMsgGetNVDataSetDescReqFunc() */
#endif

/****************************************************************************/

#if gZtcIncluded_d
void ZtcMsgGetNVPageHdrReqFunc(void) {
  index_t payloadIndex = sizeof(clientPacketStatus_t);
#if !gNvStorageIncluded_d
  gZtcPacketToClient.structured.payload[payloadIndex++] = 0;
#else
  gZtcPacketToClient.structured.payload[payloadIndex++] = 0;
#endif
  gZtcPacketToClient.structured.header.len = payloadIndex;
}                                       /* ZtcMsgGetNVPageHdrReqFunc() */
#endif

/****************************************************************************/
#if gBeeStackIncluded_d

#if gZtcIncluded_d
void ZtcMsgGetPanIDReqFunc(void) {
  FLib_MemCpy(&gZtcPacketToClient.structured.payload[sizeof(clientPacketStatus_t)],
              NlmeGetRequest(gNwkPanId_c),
              sizeof(NlmeGetRequest(gNwkPanId_c)));
  gZtcPacketToClient.structured.header.len = sizeof(clientPacketStatus_t)
                                           + sizeof(NlmeGetRequest(gNwkPanId_c));
}                                       /* ZtcMsgGetPanIDReqFunc() */
#endif

#endif /* gBeeStackIncluded_d */
/****************************************************************************/

/* Set various Ztc operating mode flags:
 *
 *  gZtcUartTxBlocking: if TRUE, the UART output routine will block until
 *  output is finished.
 *
 *  maZtcSAPModeTable[]: One byter per group of SAP Handlers, indicating
 *  what Ztc should do when a message from one of the given SAP Handlers
 *  arrives at ZTC_TaskEventMonitor():
 *      ignore messages from that SAP Handler, or
 *      forward monitor copies of messages from that SAP Handler to the
 *          external client, or
 *      intercept messages from that SAP Handle; forward them to the external
 *      client, and prevent them from being passed to their destination
 *      BeeStack layer.
 *  Note that these modes only affect messages that are sent from the SAP
 *  Handler to Ztc. These settings have no effect on packets sent from the
 *  external client to the SAP Handler.
 *
 * *warning* This is brittle. Ztc and the client must implicitly
 * agree on the order of the mode setting flags.
 */
#if gZtcIncluded_d
void ZtcMsgModeSelectReqFunc(void) {
  uint8_t byte;
  index_t count;
  index_t dataLen = gZtcPacketFromClient.structured.header.len;
  uint8_t i;
  index_t payloadIndex = 0;

  if (dataLen > 0) {
    gZtcCommTxBlocking = !!gZtcPacketFromClient.structured.payload[payloadIndex++];
  }

  dataLen -= sizeof(gZtcPacketFromClient.structured.payload[0]);

  count = (dataLen <= mZtcSAPModeTableLen ? dataLen : mZtcSAPModeTableLen);

  for (i = 0; i < count; ++i) {
    byte = gZtcPacketFromClient.structured.payload[payloadIndex + i];
    /* If support for these SAP Handlers hse by compiled out, ignore them. */
    if (maZtcSAPModeTable[i] != gZtcSAPModeInvalid_c) {
      maZtcSAPModeTable[i] = byte;
    }
  }
}                                       /* ZtcMsgModeSelectReqFunc() */
#endif

/****************************************************************************/

#if gZtcIncluded_d
void ZtcMsgNVSaveReqFunc(void) {
#if !gNvStorageIncluded_d
  ZtcError(gZtcRequestIsDisabled_c);
#else
#if !gArm7ExtendedNVM_d
  uint8_t dataSetIndex;

  dataSetIndex = gZtcPacketFromClient.structured.payload[0];
  if (dataSetIndex > NumberOfElements(NvDataSetTable)) {
    ZtcError(gZtcTooBig_c);
    return;
  }

  NvSaveOnIdle(NvDataSetTable[dataSetIndex].dataSetID);
#else
  ZtcError(gZtcRequestIsDisabled_c);
#endif
#endif
}                                       /* ZtcMsgNVSaveReqFunc() */
#endif

/****************************************************************************/
#if gZtcIncluded_d
#if (gMsgInfo_d)
#include "msgsystem.h"
extern pools_t maMmPools[];

/******************************************************************************
* MM_DebugNumFreeBigBuffers
*
******************************************************************************/
uint8_t ZTC_DebugNumFreeBigBuffers
  (
  void
  )
{
  listHeader_t *pHead;
  uint8_t count = 0;
#ifdef __IAR_SYSTEMS_ICC__  
  AppInterrupts_PushIrqStatus();  
#endif  
  pHead = maMmPools[1].anchor.pHead;
  while(pHead != NULL)   
  {
    pHead = pHead->pNext;
    count++;
  }
#ifdef __IAR_SYSTEMS_ICC__  
  AppInterrupts_PullIrqStatus();   
#endif     
 return count;
}

/******************************************************************************
* MM_DebugNumFreeSmallBuffers
*
******************************************************************************/
uint8_t ZTC_DebugNumFreeSmallBuffers
  (
  void
  )
{
  listHeader_t *pHead;
  uint8_t count = 0;
#ifdef __IAR_SYSTEMS_ICC__  
  AppInterrupts_PushIrqStatus();  
#endif  
  pHead = maMmPools[0].anchor.pHead;
  while(pHead != NULL)    
  {
    pHead = pHead->pNext;
    count++;
  }
#ifdef __IAR_SYSTEMS_ICC__  
  AppInterrupts_PullIrqStatus();   
#endif    
  return count;
}

/******************************************************************************/
void ZtcMsgGetNumOfMsgsReqFunc(void) {
  gZtcPacketToClient.structured.payload[0] = ZTC_DebugNumFreeSmallBuffers();
  gZtcPacketToClient.structured.payload[1] = ZTC_DebugNumFreeBigBuffers();
  gZtcPacketToClient.structured.header.len = (sizeof(uint8_t) *2);
}
#endif //gMsgInfo_d


#if (gZtcOtaSupportDebug_d)
#include "OtaSupport.h"
#include "ZclOta.h"

static uint8_t mOtaSetMode = FALSE;
static bool_t mLoadInProgres = FALSE;
static uint32_t mTotalImageSize;

/****************************************************************************
 * ZtcOtapSupportStartImageReqFunc:
 *  - is called by the PC App to initiate an Image Upgrade Process. 
 * This function will inform the Host if External Memory is available or not.
 ****************************************************************************/
void ZtcOtaSupportStartImageReqFunc(void) 
{
  /* store the total image size */
  FLib_MemCpy(&mTotalImageSize, gZtcPacketFromClient.structured.payload, 4);
  mTotalImageSize = OTA2Native32(mTotalImageSize);
  gZtcPacketToClient.structured.header.len = 3*(sizeof(uint8_t)); 
  gZtcPacketToClient.structured.payload[1] = 0x01;/* version */
#if gZclOtaExternalMemorySupported_d    
  gZtcPacketToClient.structured.payload[2] = TRUE;/* External Memory Supported */
#else
  gZtcPacketToClient.structured.payload[2] = FALSE;/* External Memory Not Supported */
#endif    
  mLoadInProgres = TRUE;
  gZtcPacketToClient.structured.payload[0] = gOtaSucess_c;
}

/*****************************************************************************
* ZtcOtaSupportSetModeFunc: 
*  - will set the operating mode:  UpgradeImageOnCurrentDevice,
*   UseExternalMemoryForOtaUpdate, DoNotUseExternalMemoryForOtaUpdate;
******************************************************************************/
void ZtcOtaSupportSetModeFunc(void) 
{
  mOtaSetMode  =  gZtcPacketFromClient.structured.payload[0]; 
  gZtcPacketToClient.structured.header.len = (sizeof(uint8_t));
  if(mOtaSetMode > 2)
  {
     gZtcPacketToClient.structured.payload[0] = gOtaInvalidParam_c;
     return;
  }
  if(mOtaSetMode == gUpgradeImageOnCurrentDevice_c)
  {
#if gUpgradeImageOnCurrentDevice_d    
    /* start upgrade image on current device */
    gZtcPacketToClient.structured.payload[0] = OTA_StartImage(mTotalImageSize);
#else
   gZtcPacketToClient.structured.payload[0] = gOtaInvalidParam_c;        
#endif    
  }
  else
  {
#if gZclEnableOTAServer_d    
    /* inform application: start the ota process */
    gZtcPacketToClient.structured.payload[0] = OTAServerStartImageProcess(mOtaSetMode);
#endif    
  }
}

/****************************************************************************
 *  ZtcOtaSupportPushImageChunkReqFunc: 
 *  - is called every time a new chunk is received.
 ****************************************************************************/
void ZtcOtaSupportPushImageChunkReqFunc(void) 
{
#if gUpgradeImageOnCurrentDevice_d   ||  gZclEnableOTAServer_d
    uint8_t len = gZtcPacketFromClient.headerAndStatus.header.len;
#endif    
   // gZtcPacketToClient.structured.header.len = (sizeof(uint8_t));
    if(mLoadInProgres == FALSE)
      gZtcPacketToClient.structured.payload[0] = gOtaInvalidOperation_c; 
    if(mOtaSetMode  == gUpgradeImageOnCurrentDevice_c)
    {
#if gUpgradeImageOnCurrentDevice_d        
        gZtcPacketToClient.structured.payload[0] = OTA_PushImageChunk(&gZtcPacketFromClient.structured.payload[0],len, NULL);  
#else
        gZtcPacketToClient.structured.payload[0] = gOtaInvalidParam_c;      
#endif        
    }
    else
    {
#if gZclEnableOTAServer_d        
       gZtcPacketToClient.structured.payload[0] = ZtcOtaServerBlockReceive(&gZtcPacketFromClient.structured.payload[0], len);
#endif
    }
    gZtcPacketToClient.structured.header.len = (sizeof(uint8_t));
}

/****************************************************************************
 * ZtcOtaSupportCommitImageReqFunc:
 * - will mark the process as completed
 ****************************************************************************/
void ZtcOtaSupportCommitImageReqFunc(void) 
{
    mLoadInProgres = FALSE;
    gZtcPacketToClient.structured.header.len = (sizeof(uint8_t));
    if(mOtaSetMode  == gUpgradeImageOnCurrentDevice_c)
    {
#if gUpgradeImageOnCurrentDevice_d           
        uint32_t bitmap;
        FLib_MemCpy(&bitmap, &gZtcPacketFromClient.structured.payload[2], 4);
        gZtcPacketToClient.structured.payload[0] = OTA_CommitImage(gZtcPacketFromClient.structured.payload[0], 
                                                                OTA2Native32(bitmap));  
        OTA_SetNewImageFlag();
#else
        gZtcPacketToClient.structured.payload[0] = gOtaInvalidParam_c;    
#endif        
    }
    else
    {
		gZtcPacketToClient.structured.payload[0] = gOtaSucess_c;
    }
}

/******************************************************************************
* ZtcOtaSupportQueryNextImageRspFunc:
*   - returns a minimal set of data about the image 
******************************************************************************/
void ZtcOtaSupportQueryNextImageRspFunc(void) 
{   
#if gZclEnableOTAServer_d    
   gZtcPacketToClient.structured.header.len = (sizeof(uint8_t));
   gZtcPacketToClient.structured.payload[0] = OTAServerQueryImageRsp(&gZtcPacketFromClient.structured.payload[0]);
#endif   
}

/******************************************************************************
* ZtcOtaSupportImageNotifyFunc:
*   - returns a set of information about the new image
******************************************************************************/
void ZtcOtaSupportImageNotifyFunc(void) 
{   
#if gZclEnableOTAServer_d    
   gZtcPacketToClient.structured.header.len = (sizeof(uint8_t));  
   gZtcPacketToClient.structured.payload[0] = OTAServerImageNotify(&gZtcPacketFromClient.structured.payload[0]);
#endif   
}

/****************************************************************************
 * ZtcOtaSupportCancelImageReqFunc:
 *   - calling this function will terminate the Upgrade Process
 ****************************************************************************/
void ZtcOtaSupportCancelImageReqFunc(void) 
{
   mLoadInProgres = FALSE;
}
#endif //gZtcOtaSupportDebug_d


void ZtcGetMaxApplicationPayload()
{
  zbApsdeDataReq_t zbApsdeDataReqFromClient;
  uint8_t ofsData;
  /* We only need the fields up to the txOptions for the GetMaxApplicationPayload function */
  ofsData = sizeof(zbApsdeDataReq_t) - MbrSizeof(zbApsdeDataReq_t, pAsdu) - MbrSizeof(zbApsdeDataReq_t, txOptions)
            - MbrSizeof(zbApsdeDataReq_t, radiusCounter) - MbrSizeof(zbApsdeDataReq_t, fragmentHdr);
  FLib_MemCpy(&zbApsdeDataReqFromClient, &gZtcPacketFromClient.structured.payload[0], ofsData);
  zbApsdeDataReqFromClient.pAsdu = NULL;
  /* ofsData+2 means 2 the txOptions comes after the two bytes of the pAsdu */
  zbApsdeDataReqFromClient.txOptions = gZtcPacketFromClient.structured.payload[ofsData+2];
  gZtcPacketToClient.structured.payload[0] = GetMaxApplicationPayload(&zbApsdeDataReqFromClient);
}

#endif
/****************************************************************************/

#if gZtcIncluded_d
void ZtcMsgReadExtendedAdrReqFunc(void) {
  FLib_MemCpy( &gZtcPacketToClient.structured.payload[sizeof(clientPacketStatus_t)],
       aExtendedAddress, 8 );
 
  gZtcPacketToClient.structured.header.len = sizeof(clientPacketStatus_t) + 8;
}                                         /* ZtcMsgReadExtendedAdrReqFunc() */
#endif
#if (gZtcIncluded_d && gBeeStackIncluded_d)
void ZtcMsgReadNwkMngAddressReqFunc(void)
{
  FLib_MemCpy(&gZtcPacketToClient.structured.payload[sizeof(clientPacketStatus_t)],
              NlmeGetRequest(gNwkManagerAddr_c), 2);

  gZtcPacketToClient.structured.header.len = sizeof(clientPacketStatus_t) + 2;
}
#endif

/****************************************************************************/

/* eliminate compiler warnings */
#if gZtcIncluded_d
uint8_t AF_AppToAfdeDataReq(void) {
  return gZbSuccess_c;
}

#if gBeeStackIncluded_d
/* MSG Data Indication Callback Function Pointer */
void ZtcMsgDataCallBack(apsdeToAfMessage_t *pMsg) {
  AF_FreeDataIndicationMsg(pMsg);
}

/* Data Confirmation Callback Function Pointer */
void ZtcConfDataCallBack(apsdeToAfMessage_t *pMsg) {
  MSG_Free(pMsg);
}

void ZtcMsgRegisterEndPointFunc(void) {
#if !gZTC_RegEpCapability
  ZtcError(gZtcRequestIsDisabled_c);
#else
  bool_t endPointFound;
  index_t freeEndPoint;
  index_t i;
  zbSimpleDescriptorPackageStore_t *pScratch;
  index_t scratchLen;
  uint8_t status;
  zbMaxWindowSize_t epWindowSize;

  /* Find the first unused slot in the EndPoint table. */
   for (endPointFound = FALSE, i = 0; ((endPointFound == FALSE) && (i < NumberOfElements(maSimpleDescriptors))); i++) {
    if (!maSimpleDescriptors[i].endPoint) {
      freeEndPoint = i;
    endPointFound = TRUE;
    }
  }

  if (endPointFound == FALSE) {
    ZtcError(gZtcEndPointTableIsFull_c);
    return;
  }
  /* Convert the variable length packet format into a
   * zbSimpleDescriptorPackageStore_t. This could be more directly, here in
   * this code, but then minor changes in the typedef would require code
   * changes here. Using the copy function is slower, but more robust.
   * This is a minor abuse of the format conversion mechanism; normally the
   * copy function pointer and the format parameter value would be found via
   * the message info table and the main format table.
   */
  pScratch = (zbSimpleDescriptorPackageStore_t *) MSG_Alloc(mMaxPacketSize_c);/* changed to 128 to limit to the size of a big buffer.*/
  if (!pScratch) {
    ZtcError(gZtcEndPointTableIsFull_c);
    return;
  }

  scratchLen = ZtcMsgFromPktTwoArrayPtrs((uint8_t *) pScratch,
                                         mMaxPacketSize_c,
                                         gZtcPacketFromClient.structured.payload,
                                         gZtcPacketFromClient.structured.header.len,
                                         mFmtZTCRegisterEndPointReq_c);

  /* Get the last byte for the window size. Remember one window size per Ep */
  epWindowSize = gZtcPacketFromClient.structured.header.len -1;
  epWindowSize = gZtcPacketFromClient.structured.payload[epWindowSize];

  if (   (scratchLen == gTooBig_c)
      || (pScratch->inputClusters .cNumClusters > NumberOfElements(maInputClusterIdLists[0]))
      || (pScratch->outputClusters.cNumClusters > NumberOfElements(maOutputClusterIdLists[0]))) {
    MSG_Free(pScratch);
    ZtcError(gZtcTooBig_c);
    return;
  }

  /* Move the input and output cluster lists to more permanent places. */
  FLib_MemCpy(&maInputClusterIdLists[freeEndPoint],
              pScratch->inputClusters.pClusterList,
              pScratch->inputClusters.cNumClusters
              * sizeof(*pScratch->inputClusters.pClusterList));

  pScratch->inputClusters.pClusterList = maInputClusterIdLists[freeEndPoint];

  FLib_MemCpy(&maOutputClusterIdLists[freeEndPoint],
              pScratch->outputClusters.pClusterList,
              pScratch->outputClusters.cNumClusters
              * sizeof(*pScratch->outputClusters.pClusterList));

  pScratch->outputClusters.pClusterList = maOutputClusterIdLists[freeEndPoint];

  /* Store the simple descriptor, and set up the end point descriptor. */
  FLib_MemCpy(&maSimpleDescriptors[freeEndPoint], pScratch, sizeof(*pScratch));

  maEndPointDesc[freeEndPoint].pSimpleDesc = (zbSimpleDescriptor_t *) &maSimpleDescriptors[freeEndPoint];
  maEndPointDesc[freeEndPoint].pDataMsgCallBackFuncPtr = ZtcMsgDataCallBack;
  maEndPointDesc[freeEndPoint].pDataConfCallBackFuncPtr = ZtcConfDataCallBack;
  maEndPointDesc[freeEndPoint].maxWindowSize = epWindowSize;

  status = AF_RegisterEndPoint(&maEndPointDesc[freeEndPoint]);
  if (status != gZbSuccess_c) {
    maSimpleDescriptors[freeEndPoint].endPoint = 0;
  }

  MSG_Free(pScratch);
  ZtcMsgSetCnfStatus(status);
#endif                                  /* #if !gZTC_RegEpCapability */
}                                       /* ZtcMsgRegisterEndPointFunc() */

void ZtcMsgSetEpMaxWindowSizeFunc(void)
{
#if !gZTC_RegEpCapability
  ZtcError(gZtcRequestIsDisabled_c);
#else
  zbEndPoint_t Ep;
  zbMaxWindowSize_t  maxWindowSize;

  /* End point is always the first byte of the packet */
  Ep = gZtcPacketFromClient.structured.payload[0];

  /* Window size wil lbe always the second byte of the packet. */
  maxWindowSize = gZtcPacketFromClient.structured.payload[1];

  /* 8 is the limit for Fragmentation window size. */
  if (maxWindowSize > 0x08)
  {
    ZtcError(gZtcError_c);
    return;
  }

  /* try to set the window size if the ep exists */
  if (!AF_SetEpMaxWindowSize(Ep, maxWindowSize))
  {
    /* Wrong EP */
    ZtcError(gZtcEndPointNotFound_c);
    return;
  }

  /* Status response.! */
  ZtcMsgSetCnfStatus(gZtcSuccess_c);
#endif
}

#endif  /*  gBeeStackIncluded_d */

#endif

/****************************************************************************/

#if gZtcIncluded_d
void ZtcMsgResetCPUReqFunc(void) {
  /*PWRLib_Reset();*/
#include "crm.h"
  CRM_SoftReset();
}                                       /* ZtcMsgResetCPUReqFunc() */
#endif

/****************************************************************************/
#if gBeeStackIncluded_d

#if gZtcIncluded_d
void ZtcMsgRestartNwkReqFunc(void) {
  ZDO_Start(gStartWithOutNvm_c);
  ZtcMsgSetCnfStatus(gZtcSuccess_c);
}                                       /* ZtcMsgRestartNwkReqFunc() */
#endif

/****************************************************************************/

#if gZtcIncluded_d
/*
  Set the channel mask via ZTC.
*/
void ZtcMsgSetChannelReqFunc(void)
{
  index_t channelNumber;

  /* check range, channel must be 11 - 26 */
  channelNumber = gZtcPacketFromClient.structured.payload[0];
  if(channelNumber < gZbLowestChannel_c || channelNumber > gZbHighestChannel_c)
  {
    ZtcError(gZtcTooBig_c);
    return;
  }

  /* set the channel bit */
  BeeUtilZeroMemory(gSAS_Ram.aChannelMask, sizeof(gSAS_Ram.aChannelMask));
  BeeUtilSetIndexedBit(gSAS_Ram.aChannelMask, channelNumber);
}                                       /* ZtcMsgSetChannelReqFunc() */
#endif

/****************************************************************************/

#if gZtcIncluded_d && gICanHearYouTableCapability_d
void ZtcMsgSetICanHearYouListFunc(void)
{
  uint8_t iCount;
  ztcICanHearYouTable_t *pTable;
  uint8_t *pLqiList;

  pTable = (ztcICanHearYouTable_t *)gZtcPacketFromClient.structured.payload;
  iCount = pTable->count;
  pLqiList = (uint8_t *)(pTable->table) + (iCount * sizeof(zbNwkAddr_t));
  SetICanHearYouTable(iCount, pTable->table, pLqiList);
}                                       /* ZtcMsgSetICanHearYouListFunc() */
#endif

/****************************************************************************/

#if gZtcIncluded_d
void ZtcMsgSetPanIDReqFunc(void) {
  FLib_MemCpy(NlmeGetRequest(gNwkPanId_c),
              (void *) gZtcPacketFromClient.structured.payload,
              sizeof(NlmeGetRequest(gNwkPanId_c)));
}                                       /* ZtcMsgGetPanIDReqFunc() */
#endif

/****************************************************************************/
#if gZtcIncluded_d
void ZtcMsgStartNwkExReqFunc(void)
{
  uint8_t  *pStartMode = (uint8_t *)gZtcPacketFromClient.structured.payload;
  /*
    start modes (see R17, section 2.5.5.5)
    DDDx xxxx   - Device type
    xxxS Sxxx   - Startup set
    xxxx xCCC   - Startup control mode
  */
  ZDO_Start(pStartMode[0] | pStartMode[1] | pStartMode[2]);
  ZtcMsgSetCnfStatus(gZtcSuccess_c);
}
#endif

/****************************************************************************/
#if gZtcIncluded_d
void ZtcMsgStartNwkReqFunc(void) {
  uint8_t startMode = *(uint8_t *)gZtcPacketFromClient.structured.payload;
  uint8_t event = 0;
	if(startMode == gAssociationJoin_c)
		event = gStartWithOutNvm_c;
	else if(startMode == gOrphanJoin_c)
		event = gStartOrphanRejoinWithNvm_c;
	else if(startMode == gNwkRejoin_c)
		event = gStartNwkRejoinWithNvm_c;
	else if(startMode == gSilentRejoin_c)
		event = gStartSilentRejoinWithNvm_c;  
	else if(startMode == gSilentNwkRejoin_c)
		event = gStartSilentNwkRejoin_c;
  ZDO_Start(event);
  ZtcMsgSetCnfStatus(gZtcSuccess_c);
}                                       /* ZtcMsgStartNwkReqFunc() */
#endif

/****************************************************************************/
#if gZtcIncluded_d
void ZtcMsgStopNwkExReqFunc(void)
{
  uint8_t  *pStopMode = (uint8_t *)gZtcPacketFromClient.structured.payload;
  uint8_t  stopMask;

  /* combine over-the-serial port 4 bytes into the single mask */
  stopMask = pStopMode[0];
  if (pStopMode[1])
    stopMask |= 0x20;

  if (pStopMode[2])
    stopMask |= 0x40;

  if (pStopMode[3])
    stopMask |= 0x80;

  /* stop with the ZTC user selected mode */
  ZDO_StopEx(stopMask);
  ZtcMsgSetCnfStatus(gZtcSuccess_c);
}
#endif
/****************************************************************************/
void ZtcMsgGetSeedReqFunc(void)
{
 uint16_t seed;
 seed = GetRandomNumber(); 
 FLib_MemCpy(&gZtcPacketToClient.structured.payload[0],&seed,sizeof(uint16_t) );
 gZtcPacketToClient.structured.header.len = sizeof(uint16_t);
}
/****************************************************************************/
void ZtcMsgAddToAddressMapPermanentFunc(void)
{
 uint8_t i, status;
 i  = APS_AddToAddressMapPermanent(
                             &gZtcPacketFromClient.structured.payload[0],
                             &gZtcPacketFromClient.structured.payload[sizeof(zbIeeeAddr_t)]
                             ); 
 if(i == gAddressMapFull_c)                             
   status = gAddressMapFull_c; 
 else
   status = gZtcSuccess_c;
 ZtcMsgSetCnfStatus(status);
}
/****************************************************************************/
void ZtcMsgRemoveFromAddressMapFunc(void)
{
 APS_RemoveFromAddressMap(&gZtcPacketFromClient.structured.payload[0]); 
 ZtcMsgSetCnfStatus(gZtcSuccess_c);
}
#if gZtcIncluded_d
void ZtcMsgStopNwkReqFunc(void) {
  ZDO_Stop();
  ZtcMsgSetCnfStatus(gZtcSuccess_c);
}                                       /* ZtcMsgStopNwkReqFunc() */
#endif

#endif /* gBeeStackIncluded_d */
/****************************************************************************/

/****************************************************************************/
#if gZtcIncluded_d
/* ZtcMsgWriteExtendedAdrReqFunc() */
void ZtcMsgWriteExtendedAdrReqFunc(void)
{
  FLib_MemCpy(aExtendedAddress, gZtcPacketFromClient.structured.payload,8);
  MacPhyInit_WriteExtAddress(aExtendedAddress);
#if gBeeStackIncluded_d  
  NlmeSetRequest(gNwkIeeeAddress_c, gZtcPacketFromClient.structured.payload);
#endif
}

void ZtcMsgWriteNwkMngAddressReqFunc(void)
{
  NlmeSetRequest(gNwkManagerAddr_c, gZtcPacketFromClient.structured.payload);
}
#endif

/****************************************************************************/
#if (gSAPMessagesEnableNlme_d)
#if gZtcIncluded_d
void ZtcFreeDiscoveryTables(void) {
  NWK_FreeUpDiscoveryTables();
}                          

#endif
/****************************************************************************/

#if gZtcIncluded_d
void ZtcSetJoinFilterFlag(void) {
  gZDOJoinMode = gZtcPacketFromClient.structured.payload[0];
}                          
#endif
#endif
/****************************************************************************/

/****************************************************************************/

/* Start of the memory/GPIO handling functions                       *
 * These functions handle the direct read/write operations from and  *
 * to the microcontroller memory                                     *

These functions are, of course, called the ZTC way, using pointers to functions, picked from
the message handlers table, so do not look for their declarations. 

They decode the gZtcPacketFromClient variable and use the info from there to find
out which is the location to be read/written

*****************************************************************************/
#if gUseHwValidation_c
/*The following function reads a memory block from a given address */

void  ZtcMemoryReadBlock(void)
{
  uint8_t *addr;
  uint8_t nBytes, lcount;
  uint8_t *pIn, *pOut;


  FLib_MemCpy(&addr, gZtcPacketFromClient.structured.payload, 4);

  nBytes = gZtcPacketFromClient.structured.payload[sizeof(addr)];
  lcount = nBytes;
//Construct and send response
  pOut = addr;
  pIn = &gZtcPacketToClient.structured.payload[0];
  for (;lcount != 0; lcount--)
  {
    *pIn++ = *pOut++;
  }
  gZtcPacketToClient.structured.header.opcodeGroup = gZtcCnfOpcodeGroup_c;
  gZtcPacketToClient.structured.header.msgType = mZtcLowLevelMemoryReadBlock_c; 
  gZtcPacketToClient.structured.header.len = (uint8_t)nBytes; 
//No illegal mem locations, and no strict parm check done
}

/*********************************************************************/
/*The following function reads a byte from a given address */
void  ZtcMemoryReadByte(void)
{
  uint8_t *addr;

  FLib_MemCpy(&addr, gZtcPacketFromClient.structured.payload, 4);

//Construct and send response
  gZtcPacketToClient.structured.payload[0] = *addr;
  gZtcPacketToClient.structured.header.opcodeGroup = gZtcCnfOpcodeGroup_c;
  gZtcPacketToClient.structured.header.msgType = mZtcLowLevelMemoryReadByte_c; 
  gZtcPacketToClient.structured.header.len = sizeof(uint8_t); 
//No illegal mem locations, and no strict parm check done

}
/********************************************************************/
/*The following function reads a word from a given address */
void  ZtcMemoryReadWord(void)
{
  uint8_t *addr;
  uint8_t lcount;
  uint8_t *pIn, *pOut;


  FLib_MemCpy(&addr, gZtcPacketFromClient.structured.payload, 4);
  lcount = 2;
  pOut = addr;
  pIn = &gZtcPacketToClient.structured.payload[0];
  for (;lcount != 0; lcount--) 
      *pIn++ = *pOut++;
  gZtcPacketToClient.structured.header.opcodeGroup = gZtcCnfOpcodeGroup_c;
  gZtcPacketToClient.structured.header.msgType = mZtcLowLevelMemoryReadWord_c; 
  gZtcPacketToClient.structured.header.len = 2; 
}
/********************************************************************/
/*The following function reads a long value from a given address */
void  ZtcMemoryReadLong(void)
{
  uint8_t *addr;
  uint8_t lcount;
  uint8_t *pIn, *pOut;

  FLib_MemCpy(&addr, gZtcPacketFromClient.structured.payload, 4);
  lcount = 4;
//Construct and send response
  pOut = addr;  
  pIn = &gZtcPacketToClient.structured.payload[0];
  for (;lcount != 0; lcount--) 
    *pIn++ = *pOut++;

  gZtcPacketToClient.structured.header.opcodeGroup = gZtcCnfOpcodeGroup_c;
  gZtcPacketToClient.structured.header.msgType = mZtcLowLevelMemoryReadLong_c; 
  gZtcPacketToClient.structured.header.len = 4; 
}

//-----------------------------------------------------------------------------------
/*The following function writes a byte block from a given address */
void  ZtcMemoryWriteBlock(void)
{
  uint8_t *addr;
  uint8_t nBytes;
  uint8_t *pIn, *pOut;
  uint8_t nWritten, nRead;

  FLib_MemCpy(&addr, gZtcPacketFromClient.structured.payload, 4);

  nBytes = gZtcPacketFromClient.structured.payload[sizeof(addr)];
  pIn = &gZtcPacketFromClient.structured.payload[sizeof(addr)+1];
  pOut = addr;
  nRead = nWritten = 0;
  while (nRead++ < nBytes)
  {
  if (!(((int)pOut < 0x00400000) && (((int)pOut & 0xF0000000)!= 0x80000000)))// Write to out of RAM range or I/O space attempted
     {
      *pOut++ = *pIn++;
      nWritten++;
     }
  }
  gZtcPacketToClient.structured.header.opcodeGroup = gZtcCnfOpcodeGroup_c;
  gZtcPacketToClient.structured.header.msgType = mZtcLowLevelMemoryWriteBlock_c; 
  gZtcPacketToClient.structured.header.len = 1;
  gZtcPacketToClient.structured.payload[0] = nWritten;
}
/************************************************************************/
/*This function writes a byte into a memory location*/

void  ZtcMemoryWriteByte(void)
{
  uint8_t *addr;
  uint8_t vIn, *pOut, nWritten;


  FLib_MemCpy(&addr, gZtcPacketFromClient.structured.payload, 4);  
  vIn = gZtcPacketFromClient.structured.payload[sizeof(addr)];
  pOut = addr;
  nWritten = 0;

  if (!((int)pOut < 0x00400000) && ((((int)pOut & 0xF0000000)!= 0x80000000)))  // Write to out of RAM range or I/O space attempted
  {
    *pOut = vIn ;
    nWritten = sizeof(uint8_t); 
  }
  gZtcPacketToClient.structured.header.opcodeGroup = gZtcCnfOpcodeGroup_c;
  gZtcPacketToClient.structured.header.msgType = mZtcLowLevelMemoryWriteByte_c; 
  gZtcPacketToClient.structured.header.len = 1;
  gZtcPacketToClient.structured.payload[0] = nWritten;
}

/***************************************************************************/
/*This function writes a word into a memory location*/
void  ZtcMemoryWriteWord(void)
{
  uint8_t *addr;
  uint8_t *pIn, *pOut;
  uint8_t nWritten, nRead;

  FLib_MemCpy(&addr, gZtcPacketFromClient.structured.payload, 4);
  pIn = &gZtcPacketFromClient.structured.payload[sizeof(addr)];
  pOut = addr;
  nRead = nWritten = 0;
  while (nRead++ < sizeof(uint16_t))
  {
    if (!(((int)pOut < 0x00400000) && (((int)pOut & 0xF0000000)!= 0x80000000)))// Write to out of RAM range or I/O space attempted
    {
      *pOut++ = *pIn++;
      nWritten++;
    }
  }
  gZtcPacketToClient.structured.header.opcodeGroup = gZtcCnfOpcodeGroup_c;
  gZtcPacketToClient.structured.header.msgType = mZtcLowLevelMemoryWriteWord_c; 
  gZtcPacketToClient.structured.header.len = 1;
  gZtcPacketToClient.structured.payload[0] = nWritten;
}

/***************************************************************************/
/*This function writes a long value into a memory location*/

void  ZtcMemoryWriteLong(void)
{
  uint8_t *addr;
  uint8_t *pIn, *pOut;
  uint8_t nWritten, nRead;

  FLib_MemCpy(&addr, gZtcPacketFromClient.structured.payload, 4);
  pIn = &gZtcPacketFromClient.structured.payload[sizeof(addr)];
  pOut = addr;
  nRead = nWritten = 0;
  while (nRead++ < sizeof(uint32_t))
  {
    if (!(((int)pOut < 0x00400000) && (((int)pOut & 0xF0000000)!= 0x80000000)))// Write to out of RAM range or I/O space attempted
    {
      *pOut++ = *pIn++;
      nWritten++;
    }
  }
  gZtcPacketToClient.structured.header.opcodeGroup = gZtcCnfOpcodeGroup_c;
  gZtcPacketToClient.structured.header.msgType = mZtcLowLevelMemoryWriteLong_c; 
  gZtcPacketToClient.structured.header.len = 1;
  gZtcPacketToClient.structured.payload[0] = nWritten;
}

/****************************************************************************/
/*ZtcPing simply echoes back the payload*/

void  ZtcPing(void)
{
  gZtcPacketToClient.structured.header.opcodeGroup = gZtcCnfOpcodeGroup_c;
  gZtcPacketToClient.structured.header.msgType = mZtcLowLevelPing_c; 
  gZtcPacketToClient.structured.header.len = gZtcPacketFromClient.structured.header.len;
  FLib_MemCpy(gZtcPacketToClient.structured.payload, gZtcPacketFromClient.structured.payload,
              gZtcPacketFromClient.structured.header.len );

}
#endif  /* gUseHwValidation_c */

/* Permissions Configuration Table */
#if (gZtcIncluded_d && (gStandardSecurity_d || gHighSecurity_d) && gApsMaxEntriesForPermissionsTable_c)

extern index_t GetPermissionsTable(permissionsTable_t * pDstTable);
extern void SetPermissionsTable(index_t entryCounter, uint8_t * payload);
extern uint8_t RemoveDeviceFromPermissionsTable(zbIeeeAddr_t aDevAddr);
extern void RemoveAllFromPermissionsTable(void);
extern uint8_t AddDeviceToPermissionsTable(uint8_t * aDevAddr, permissionsFlags_t permissionsCategory);

void ZtcMsgSetPermissionsTableFunc(void) {
  SetPermissionsTable(gZtcPacketFromClient.structured.payload[0], &(gZtcPacketFromClient.structured.payload[1]));
}                                       /*  */

void ZtcMsgGetPermissionsTableFunc(void) {
  ztcPermissionsTable_t *pTable = (ztcPermissionsTable_t *) &gZtcPacketToClient.structured.payload[sizeof(clientPacketStatus_t)];
  pTable->count = GetPermissionsTable(pTable->table);
  gZtcPacketToClient.structured.header.len = sizeof(clientPacketStatus_t)
                                           + sizeof(pTable->count)
                                           + (pTable->count * sizeof(permissionsTable_t));                                           
}                                       /*  */

void ZtcMsgRemoveFromPermissionsTableFunc(void)
{
  if (gZtcPacketFromClient.structured.payload[0])
    RemoveAllFromPermissionsTable();
  else
    gZtcPacketToClient.structured.payload[0] = RemoveDeviceFromPermissionsTable(&gZtcPacketFromClient.structured.payload[1]);
  gZtcPacketToClient.structured.header.len = sizeof(clientPacketStatus_t); 
}                                       /*  */

void ZtcMsgAddDeviceToPermissionsTableFunc(void)
{
  gZtcPacketToClient.structured.payload[0] = AddDeviceToPermissionsTable(&(gZtcPacketFromClient.structured.payload[0]), gZtcPacketFromClient.structured.payload[sizeof(zbIeeeAddr_t)]);
  gZtcPacketToClient.structured.header.len = sizeof(clientPacketStatus_t); 
}                                       /*  */

#endif /* (gZtcIncluded_d && (gStandardSecurity_d || gHighSecurity_d) && gApsMaxEntriesForPermissionsTable_c) */
