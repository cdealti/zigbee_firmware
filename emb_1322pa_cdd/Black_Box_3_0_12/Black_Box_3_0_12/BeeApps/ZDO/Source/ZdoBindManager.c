/*****************************************************************************
* ZdoBindManager.c
*
* Copyright (c) 2008, Freescale, Inc. All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
* This module contains End-Device-Bind, a function that matches endpoints.
* Two or more nodes send data to the coordinator to match clusters/endpoints.
*
* Note: only binds the first matching cluster. Only sends the unbind/bind
* commands to the side(s) with matching output cluster(s).
*
******************************************************************************/
#include "EmbeddedTypes.h"
#include "zigbee.h"

#include "PublicConst.h"
#include "BeeStack_Globals.h"
#include "ZdoApsInterface.h"
#include "BeeCommon.h"
#include "BeeStackUtil.h"
#include "BeeStackConfiguration.h"
#include "ZdoCommon.h"
#include "TMR_Interface.h"
#include "ZdoBindManager.h"
#include "ZdpManager.h"
#include "ZdoMain.h"
#include "NV_Data.h"
#include "Beestackinterface.h"
#include "Applicationconf.h"
#include "ApsMgmtInterface.h"

/******************************************************************************
*******************************************************************************
* Private Macros
*******************************************************************************
******************************************************************************/

/* various end-device bind constants */
#define mEdbMaxEntries_c      2             /* maximum EDB nodes at once */
#define mEdbMaxClusters_c     16            /* 16 total clusters in both input/output list */
#define mEdbTickInterval_c    100           /* 1/10 second between ticks */
#define mEdbTicksToExpire_c   (mDefaultValueOfEndDeviceBindTimeOut_c/mEdbTickInterval_c)  /* wait EDB up to xx seconds */
#define mEdbTicksNoExpire_c   0xffff
#define mEdbTicksForReply_c   (uint8_t) ((gNwkPurgeTimeout + 1500)/mEdbTickInterval_c)   /* wait 1.5 seconds + NwkPersistenceTime for reply */
#define mEdbMaxClusterMatch_c  (8 +1)


/* indicates table is full */
#define mEdbTableFull_c         gMaxIndex_c
#define mEdbNotInTable_c        gMaxIndex_c

/* is this entry valid? */
#define EdbIsValid(i) (maEdbTable[(i)].state)


/******************************************************************************
*******************************************************************************
* Private Types
*******************************************************************************
******************************************************************************/

/* various states for this EDB call, numeric not bit mask */
typedef uint8_t zdoEdbState_t;
enum
{
  mEdbStateUnused_c = 0,          /* must be 0 */
  mEdbStateReady_c = 1,           /* ready (not waiting for anything yet) */
  mEdbStateAllocatingUnbind_c,    /* trying to allocate memory for unbind request */
  mEdbStateWaitingForUnbindRsp_c, /* sent out unbind call, waiting for response */
  mEdbStateAllocatingBind_c,      /* trying to allocate memory for bind request */
  mEdbStateWaitingForBindRsp_c,   /* send out bind call, waiting for response */
  mEdbStateWaitingForStatus_c,    /* waiting to send status (failed or success) */
  mEdbStateSuccess_c,             /* waiting to send out success msg */
  mEdbStateDone_c                 /* bind has completed */
};

/* how many ticks left before quitting end-device-bind? */
typedef uint32_t zdoEdbTicksLeft_t;
#ifdef __IAR_SYSTEMS_ICC__
#pragma pack(1)
#endif
/* internal structure for end-device-bind */
typedef struct zdoEdbTable_tag
{
  zdoEdbState_t            state;           /* current state of this entry (also direction) */
  zbStatus_t               status;          /* what status should this report? */
  uint8_t                  iTransactionId;  /* record transaction ID */
  uint8_t                  iTimeout;        /* how long to wait before this next state time's out? */
  index_t                  iMatchIndex;     /* does the other side match? Record the index in the EDB table that matches */
  zbClusterId_t            aMatchCluster[mEdbMaxClusterMatch_c];   /* what cluster matched? */
  zbEndDeviceBindRequest_t request;         /* copy of bind request */
  zbClusterId_t            aClusterList[mEdbMaxClusters_c];  /* input and output clusters */
} zdoEdbTable_t;

#ifdef __IAR_SYSTEMS_ICC__
#pragma pack()
#endif
/* for Bind/Unbind call */
#define mEdbUnbind_c  FALSE
#define mEdbBind_c    TRUE

/******************************************************************************
*******************************************************************************
* Private Prototypes
*******************************************************************************
******************************************************************************/

bool_t EdbMatchClusters(index_t iEdbIndex);
void EdbStartupBind(index_t iSrcEdbIndex, index_t iDstEdbIndex);
void EdbTimeOutCallBack( uint8_t iTimerId );
index_t EdbAlloc(zbEndDeviceBindRequest_t *pReqFrame);
void EdbFree(index_t iEdbIndex);
bool_t EbdSendBindUnBind(bool_t fBind, index_t iEdbIndex);
index_t EdbFindEntry(zbNwkAddr_t aNwkAddr);
bool_t EdbSendResponse(index_t i);
bool_t EdbSendTimeoutResponse(void);
bool_t EdbIsAllDone(void);

/******************************************************************************
*******************************************************************************
* Public Memory
*******************************************************************************
******************************************************************************/

zbClusterId_t gaZdpMatchedOutCluster[mEdbMaxClusterMatch_c];
index_t giMatchIndex = 0x00;

/******************************************************************************
*******************************************************************************
* Private Memory
*******************************************************************************
******************************************************************************/

#if (gCoordinatorCapability_d || gComboDeviceCapability_d) && gBindCapability_d && gEnd_Device_Bind_rsp_d

bool_t gZdoInEndDeviceBind; /* # of nodes that have requested EDB */
index_t mEdbCurrentIndex;   /* only processes one side at a time (code reduction) */

bool_t mEdbError;
zdoEdbTicksLeft_t mEdbTicksLeft;            /* time left in end-device-bind mode */
zdoEdbTable_t maEdbTable[mEdbMaxEntries_c]; /* each entry contains a bind-state-machine */

#endif  /* ((gCoordinatorCapability_d) && (gBindCapability_d) && (gEnd_Device_Bind_rsp_d)) */



/******************************************************************************
*******************************************************************************
* Public Functions
*******************************************************************************
******************************************************************************/

#if (gCoordinatorCapability_d || gComboDeviceCapability_d) && gBindCapability_d && gEnd_Device_Bind_rsp_d

/*
  ZdpEndDeviceBindReset

  Reset the end-device bind state machine and structures.
*/
void ZdpEndDeviceBindReset(void)
{
  /* stop the timer */
  TMR_StopTimer(gEndDevBindTimerID);

  /* no longer in EDB, so ZDO will not send it's bind responses here.. */
  gZdoInEndDeviceBind = mEdbError = FALSE;
  mEdbTicksLeft = mEdbCurrentIndex = 0;

  /* done, clear everything */
  BeeUtilZeroMemory(maEdbTable, sizeof(maEdbTable));
}

/*
  ZdpEndDeviceBindStateMachine

  Checks the state machine.
*/
void ZdpEndDeviceBindStateMachine(void)
{
  bool_t fBind = FALSE;
  zdoEdbTable_t  *pPtr;

  /* if state machine is done, clear out entire end-device-bind table */
  if (!mEdbTicksLeft)
  {
    /* go through and send time-out response to all nodes that didn't get a response */
    if(EdbSendTimeoutResponse())
      ZdpEndDeviceBindReset();
    return;
  }

  /* a little less time is left */
  else
  {
    --mEdbTicksLeft;
  }

  /* go through list, looking for valid entries that need attention */
  pPtr = &maEdbTable[mEdbCurrentIndex];

  /* move on to next state */
  switch(pPtr->state)
  {
    /* trying to send out a packet */
    case mEdbStateAllocatingBind_c:
      fBind = TRUE;
      /* fall through to next case... */

    case mEdbStateAllocatingUnbind_c:
      if (EbdSendBindUnBind(fBind, mEdbCurrentIndex))
      {
        ++pPtr->state;  /* on to next state: waiting for reply */
        pPtr->iTimeout = mEdbTicksForReply_c;
      }
      break;

    /* waiting for unbind or bind. */
    case mEdbStateWaitingForUnbindRsp_c:
    case mEdbStateWaitingForBindRsp_c:
      /* Nothing to do. */
      break;

    /* waiting to send out success or other status */
    case mEdbStateWaitingForStatus_c:
      if(EdbSendResponse(mEdbCurrentIndex))
      {
        /* if all entries are done, then stop state machine */
        ++mEdbCurrentIndex;
        if(mEdbError || mEdbCurrentIndex >= mEdbMaxEntries_c)
        {
          ZdpEndDeviceBindReset();
          return;
        }
      }
      break;

    /* nothing to do, just wait */
    default:
      break;
  } /* done switch */
}

/*
  EdbInvalidEndPoint

  Check if someone is trying to bind to ZDO or another reserved endpoint.
*/
bool_t EdbInvalidEndPoint(index_t iEbdIndex)
{
  zbEndPoint_t endPoint;
  zdoEdbTable_t  *pPtr;

  pPtr = &maEdbTable[iEbdIndex];
  endPoint = pPtr->request.srcEndPoint;

  if(endPoint == gZbZdpEndPoint_c || endPoint == gZbBroadcastEndPoint_c) {
    pPtr->status = gZdoInvalidEndPoint_c;
    pPtr->state = mEdbStateWaitingForStatus_c;
    return TRUE;  /* endpoint invalid */
  }
  return FALSE; /* endpoint valid */
}

/*
  ZdpEndDeviceBindGotRequest

  Every time we receive an end-device bind request, call this routine. Will
  queue the end-device bind request for processing.

  Note: the pRequest is really in over-the-air format, which can't be
  represented well in C, so we use the zbEndDeviceBindRequest_t type to
  approximate it. The two cluster lists are variable length arrays.

  OTA format:
  [aBindingTarget][aSrcIeeeAddress][srcEndPoint][aProfileId]
    [inClusters][inClusterList][outclusters][outClusterList]

  If the queue is full or we're out of memory, the request is dropped.

  Always returns 0 (no bytes to reply at this time).
*/
zbSize_t ZdpEndDeviceBindGotRequest(zbEndDeviceBindRequest_t *pReqFrame)
{
  index_t iEbdIndex;
  zdoEdbTable_t  *pPtr;

  /* if this is the first End-Device-Bind request, start up the engine */
  if(!gZdoInEndDeviceBind)
  {
    /* if we can't allocate the request, throw it out */
    iEbdIndex = EdbAlloc(pReqFrame);

    if(iEbdIndex == mEdbTableFull_c)
    {
      return 0;
    }

    /* if invalid endpoint, send back invalid endpoint error. */
    if(EdbInvalidEndPoint(iEbdIndex))
    {
      mEdbError = TRUE;
    }

    /* start up the interval timer to feed the state machine */
    mEdbTicksLeft = mEdbTicksToExpire_c;
    TMR_StartIntervalTimer(gEndDevBindTimerID, mEdbTickInterval_c, EdbTimeOutCallBack);

    /* indicate we are in binding state */
    ++gZdoInEndDeviceBind;
  }

  /* check for a match with the first EDB.request */
  else if(!mEdbError && gZdoInEndDeviceBind < mEdbMaxEntries_c)
  {

    /* allocate request. if we can't, throw it out */
    iEbdIndex = EdbAlloc(pReqFrame);
    if(iEbdIndex == mEdbTableFull_c)
      return 0;

    /* if bogus endpoint, return back no match on original */
    if(EdbInvalidEndPoint(iEbdIndex)) {
      maEdbTable[0].status = gZdoNoMatch_c;
      maEdbTable[0].state = mEdbStateWaitingForStatus_c;
      return 0;
    }

    /* if request doesn't match, throw it out */
    /* will start up the unbind/bind procedure if found an output match */
    if(!EdbMatchClusters(iEbdIndex))
    {
      for(iEbdIndex=0; iEbdIndex < mEdbMaxEntries_c; iEbdIndex++)
      {
        pPtr = &maEdbTable[iEbdIndex];
        pPtr->status = gZdoNoMatch_c;
        pPtr->state = mEdbStateWaitingForStatus_c;
      }
      return 0;
    }

    /* one more binding entry */
    ++gZdoInEndDeviceBind;
  }

  return 0;
}

/*
  ZdpEndDeviceBindGotResponse

  Got a response from a bind or unbind request. Move on to the next state.
*/
void ZdpEndDeviceBindGotResponse(zbNwkAddr_t aSrcAddr, zbStatus_t status)
{
  index_t iEdbIndex;
  bool_t  fSuccess = FALSE;
  zdoEdbTable_t  *pPtr;
  uint16_t iClusterIndex;

  /* dump the response if we didn't know about it */
  iEdbIndex = EdbFindEntry(aSrcAddr);
  if(iEdbIndex == mEdbNotInTable_c)
    return;

  pPtr = &maEdbTable[iEdbIndex];

  Copy2Bytes(&iClusterIndex, &pPtr->aMatchCluster[0]);
  /* if done with unbind, move on to binding (perhaps) */
  if(pPtr->state == mEdbStateWaitingForUnbindRsp_c)
  {
    /* only if no entry, continue to to binding. if error, or success (unbound), then don't bind. */
    /* this allows a toggle. will do a bind, then an unbind. */
    if(status == gZdoNoEntry_c)
      pPtr->state = mEdbStateAllocatingBind_c;  /* send out bind command */
    else
    {
      --iClusterIndex;
      if (!iClusterIndex)
      {
        fSuccess = TRUE;
      }
      else
      {
        pPtr->state = mEdbStateAllocatingUnbind_c;
      }
    }
  }

  /* if waiting for a bind and we got the result, send success response */
  if(pPtr->state == mEdbStateWaitingForBindRsp_c)
  {
    --iClusterIndex;
    if (!iClusterIndex)
    {
      fSuccess = TRUE;
    }
    else
    {
      pPtr->state = mEdbStateAllocatingUnbind_c;
    }
  }

  Copy2Bytes(&pPtr->aMatchCluster[0], &iClusterIndex);

  /* bind or unbind was successful, inform both nodes */
  if(fSuccess)
  {
    pPtr->state = mEdbStateWaitingForStatus_c;

    /* only send the success to the other side if it's not in the middle of binding itself */
    pPtr = &maEdbTable[pPtr->iMatchIndex];
    if(pPtr->state == mEdbStateReady_c)
    {
      pPtr->state = mEdbStateWaitingForStatus_c;
    }
  }
}

/******************************************************************************
*******************************************************************************
* Private Functions
*******************************************************************************
******************************************************************************/

/*
  Look for this particular entry in the table.
*/
index_t EdbFindEntry(zbNwkAddr_t aNwkAddr)
{
  index_t i;
  zdoEdbTable_t  *pPtr;
  for(i=0; i < mEdbMaxEntries_c; ++i)
  {
    pPtr = &maEdbTable[i];
    if(EdbIsValid(i) && IsEqual2Bytes(aNwkAddr, pPtr->request.aBindingTarget))
      return i;
  }
  return mEdbNotInTable_c;
}

/***********************************************************************************
* Checks if the end-device-bind entry matches clusters with the original entry.
*
* Interface assumption:
*   The parameter iEdbIndex is avlid index in the table used for the EDB state machine
*   usually is a two entry table.
*
* Retturns value:
*   TRUE if it does, FALSE if not. Fills in
************************************************************************************/
bool_t EdbMatchClusters
(
  index_t iEdbIndex
)
{
  bool_t  fFoundMatch = FALSE;
  zdoEdbTable_t  *pPtr;

  /*  */
  pPtr = &maEdbTable[iEdbIndex];

  /* first, check if profiles match. If not, then no match */
  if (!IsEqual2Bytes(maEdbTable[0].request.aProfileId, pPtr->request.aProfileId))
    return fFoundMatch;

  /*
    check if the output clusters from entry 0 match to the input clusters of the second
    entry. The entry 0 is the first device that sent the EndDeviceBindRequest.
  */
  if (Zdp_MatchClusters(pPtr->request.inputClusterList.cNumClusters,
      maEdbTable[0].request.outputClusterList.cNumClusters,
      pPtr->request.inputClusterList.pClusterList,
      maEdbTable[0].request.outputClusterList.pClusterList,
      TRUE))
  {
    EdbStartupBind(0, iEdbIndex);
    fFoundMatch = TRUE;
  }

  /*
    check if the input clusters from entry 0 match to the output clusters of the second
    entry. The entry 0 is the first device that sent the EndDeviceBindRequest.
  */
  if (Zdp_MatchClusters(maEdbTable[0].request.inputClusterList.cNumClusters,
      pPtr->request.outputClusterList.cNumClusters,
      maEdbTable[0].request.inputClusterList.pClusterList,
      pPtr->request.outputClusterList.pClusterList,TRUE))
  {
    EdbStartupBind(iEdbIndex, 0);

    /* no output match on entry 0, make sure to move the state of entry 0 to waiting to send reply */
    if (!fFoundMatch)
      maEdbTable[0].state = mEdbStateWaitingForStatus_c;

    fFoundMatch = TRUE;

  }

  return fFoundMatch;
}

/*
  EdbStartupBind

  Startup a binding entry.
*/
void EdbStartupBind(index_t iSrcEdbIndex, index_t iDstEdbIndex)
{
  zdoEdbTable_t  *pPtr = &maEdbTable[iSrcEdbIndex];

  /* don't start up if already in the process of binding for this entry */
  if (pPtr->state != mEdbStateReady_c)
  {
    return;
  }

  /* record the output cluster index for building the binding frame */
  pPtr->iMatchIndex = iDstEdbIndex;
  FLib_MemCpy(&pPtr->aMatchCluster[1], &gaZdpMatchedOutCluster,(sizeof(zbClusterId_t) * giMatchIndex));

  /* The first entry has the amount of matches. */
  Set2Bytes(&pPtr->aMatchCluster[0], giMatchIndex);
  giMatchIndex = 0x00;

  /* note: gaZdpMatchedOutCluster was set when Zdp_MatchClusters() is called */

  /* tell state machine to allocate and send the bind */
  pPtr->state = mEdbStateAllocatingUnbind_c;
}


/*
  EbdSendBindUnBind

  Sends the bind/unbind command over the air. Makes a message and calls on ZDP.

  Returns TRUE if sent, FALSE if failed.
*/
bool_t EbdSendBindUnBind(bool_t fBind, index_t iEdbIndex)
{
  appToZdpMessage_t *pMsg;
  zbBindUnbindRequest_t bindRequest;
  zdoEdbTable_t  *pPtr = &maEdbTable[iEdbIndex];
  uint8_t iOtherIndex = iEdbIndex ? 0 : 1;
  uint16_t  iClusterMatch;

  /* allocate the message */
  pMsg = MSG_AllocType(appToZdpMessage_t);
  if(!pMsg)
    return FALSE;

  /* fill in the message (note: takes advantage of bind/unbind being same payload) */
  pMsg->msgType = fBind ? gBind_req_c : gUnbind_req_c;
  Copy2Bytes(pMsg->aDestAddr, pPtr->request.aBindingTarget);

  /* fill in the request */
  Copy2Bytes(&iClusterMatch,&pPtr->aMatchCluster[0]);
  Copy8Bytes(bindRequest.aSrcAddress, pPtr->request.aSrcIeeeAddress);
  Copy2Bytes(bindRequest.aClusterId, pPtr->aMatchCluster[iClusterMatch]);
  bindRequest.addressMode = gZbAddrMode64Bit_c;
  bindRequest.srcEndPoint = pPtr->request.srcEndPoint;
  Copy8Bytes(bindRequest.destData.extendedMode.aDstAddress, maEdbTable[iOtherIndex].request.aSrcIeeeAddress);
  bindRequest.destData.extendedMode.dstEndPoint = maEdbTable[iOtherIndex].request.srcEndPoint;

  /* copy the request */
  FLib_MemCpy(&pMsg->msgData.bindReq, &bindRequest, sizeof(bindRequest));

  /* call ZDP to send the message out */
  (void)APP_ZDP_SapHandler( pMsg );
  return TRUE;
}

/*
  EdbTimeOutCallBack

  Called once per End-Device-Bind "tick".
*/
void EdbTimeOutCallBack( uint8_t iTimerId )
{
  /* appease compiler (prevent warning) */
  (void)iTimerId;

  /* a tick has happened, call the task, ends up in ZdoEdbStateMachine() */
  TS_SendEvent(gZdoTaskID_c,gEndDevBindTimeOut_c);
}

/*
  EdbFree

  Free an entry. Cannot fail.
*/
void EdbFree(index_t iEdbIndex)
{
  maEdbTable[iEdbIndex].state = 0;
}

/***********************************************************************************
* Allocate entry for the End-Device-Bind request. Copies the edb structure into
* memory. Note: Bind request comes in over-the-air (OTA) form.
* It has an array big enough ant the ned of the structure where it hold the two lists
* Inpunt and output clusters 
*
* Interface assumption:
*   The parameter pReqFrame is not a null pointer.
*
* Return value:
*   0-n if worked, mEdbTableFull_c if failed.
************************************************************************************/
index_t EdbAlloc(zbEndDeviceBindRequest_t *pReqFrame)
{
  index_t iEdbIndex;
  index_t i;
  index_t iClusterIndex;
  index_t iNumClusters;   /* # clusters to copy */
  zbClusterList_t *pSrcClusterList;    /* from the OTA msg */
  zbPtrClusterList_t *pDstClusterList; /* goes into the table... */
  zdoEdbTable_t  *pPtr;

  /* look for a free entry in end-device-bind table */
  for(iEdbIndex=0; iEdbIndex < mEdbMaxEntries_c; ++iEdbIndex)
  {
    /* free entry */
    if(!EdbIsValid(iEdbIndex))
      break;
  }

  /* too many entries, give up */
  if(iEdbIndex == mEdbMaxEntries_c)
    return mEdbTableFull_c;

  pPtr = &maEdbTable[iEdbIndex];

  /* copy the fixed part of the bind request */
  FLib_MemCpy(&(pPtr->request), pReqFrame, MbrOfs(zbEndDeviceBindRequest_t,inputClusterList));

  /* transaction ID is 1 byte before payload */
  pPtr->iTransactionId = *(((uint8_t *)pReqFrame) - sizeof(zbMsgId_t));

  /* copy the 2 cluster lists */
  pSrcClusterList = (zbClusterList_t *)(&(pReqFrame->inputClusterList));
  pDstClusterList = (&(pPtr->request.inputClusterList));
  iClusterIndex = 0;
  for(i=0; i < 2; i++)
  {
    pDstClusterList = pDstClusterList + i;

    /* get # of clusters */
    iNumClusters = pSrcClusterList->cNumClusters;
    pDstClusterList->cNumClusters = iNumClusters;

    /* get ptr to clusters in array */
    pDstClusterList->pClusterList = &(pPtr->aClusterList[iClusterIndex]);

    /* copy clusters (but not if too large) */
    iClusterIndex += iNumClusters;
    if(iClusterIndex > mEdbMaxClusters_c)
    {
      return mEdbTableFull_c;
    }
    FLib_MemCpy(pDstClusterList->pClusterList, pSrcClusterList->aClusterList,
        iNumClusters * sizeof(zbClusterId_t));

    /* move pointer to next cluster list */
    pSrcClusterList = (void *)(((uint8_t *)pSrcClusterList) + sizeof(pSrcClusterList->cNumClusters) +
      iNumClusters * sizeof(zbClusterId_t));
  }

  /* indicate ready to use (waiting for another match) */
  pPtr->state = mEdbStateReady_c;
  pPtr->status = gZbSuccess_c;

  /* return which entry we got */
  return iEdbIndex;
}

/*
  EdbSendResponse

  Sends the end-device-bind response over-the-air (may be success, may not).
*/
bool_t EdbSendResponse(index_t i)
{
  uint8_t *pMsg;
  zdoEdbTable_t  *pPtr;

  /* allocate the message */
  pMsg = AF_MsgAlloc();
  if(!pMsg)
    return FALSE; /* not sent, try again later */

  pPtr = &maEdbTable[i];
  /* Zdp_GenerateDataReq relays on the fact tha the memory is filled up with Zeros (0x00) */
  /* gMaxRxTxDataLength_c is the allocated size of AF_MsgAlloc*/
  BeeUtilZeroMemory(pMsg, gMaxRxTxDataLength_c);

  /* send to originating node */
  pMsg[SapHandlerDataStructureOffSet] = pPtr->status;
  pMsg[SapHandlerDataStructureOffSet-sizeof(zbMsgId_t)] = pPtr->iTransactionId;
  Zdp_GenerateDataReq(gEnd_Device_Bind_rsp_c, pPtr->request.aBindingTarget, (afToApsdeMessage_t *)pMsg, sizeof(zbStatus_t));

  /* must be done */
  pPtr->state = mEdbStateDone_c;

  return TRUE;  /* msg sent */
}

/*******************************************************************************
  Returns TRUE if all valid entries are done.
*******************************************************************************/
bool_t EdbIsAllDone(void)
{
  index_t i;
	zdoEdbTable_t  *pPtr;

  for(i=0; i<mEdbMaxEntries_c; ++i) {
		pPtr = &maEdbTable[i];
    if(EdbIsValid(i) && pPtr->state != mEdbStateDone_c)
      return FALSE;
  }
  return TRUE; /* all done. nothing left to send */
}


/*******************************************************************************
  EdbSendTimeoutResponse

  Sends the timeout response to each node that didn't match.

  Returns TRUE if all timeouts are sent. Returns FALSE if not.
*******************************************************************************/
bool_t EdbSendTimeoutResponse(void)
{
  index_t i;
	zdoEdbTable_t  *pPtr;

  for(i=0; i<mEdbMaxEntries_c; ++i) {
		pPtr = &maEdbTable[i];
    if(EdbIsValid(i) && pPtr->state != mEdbStateDone_c) {
      /* send out status (if we can) */
      pPtr->status = gZdoTimeOut_c;
      if(!EdbSendResponse(i))
        return FALSE;   /* still waiting to send */
    }
  }

  /* no longer in end-device-bind */
  gZdoInEndDeviceBind = FALSE;

  return TRUE; /* all done. nothing left to send */
}

#endif /* gCoordinatorCapability_d && gBindCapability_d && gEnd_Device_Bind_rsp_d */

/******************************************************************************
*******************************************************************************
* Unit Tests
*******************************************************************************
******************************************************************************/

/* None */
