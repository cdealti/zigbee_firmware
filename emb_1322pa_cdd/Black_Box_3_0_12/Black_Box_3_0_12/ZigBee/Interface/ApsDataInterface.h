/******************************************************************************
* This file provides interface methods for data service module of APS
*
* Copyright (c) 2007, Freescale, Inc.  All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from  Freescale Semiconductor.
*******************************************************************************/
#ifndef _APS_DATA_INTERFACE_H_
#define _APS_DATA_INTERFACE_H_

/* needed for binding type */
#include "ApsMgmtInterface.h"

/******************************************************************************
*******************************************************************************
* Public macros
*******************************************************************************
******************************************************************************/

/*APS SapHandlers events*/
#define gAF_APSDE_c  (1<<0)
#define gNLDE_APS_c  (1<<1)
#define gZDO_APSME_c (1<<2)
#define gApsTxStateMachine_c (1<<3)


/******************************************************************************
*******************************************************************************
* Public prototypes
*******************************************************************************
******************************************************************************/

/* None */

/******************************************************************************
*******************************************************************************
* Public type definitions
*******************************************************************************
******************************************************************************/
typedef struct apsRxFragCtrlSettings_tag
{
  uint8_t iRxWindowStart;
  uint8_t iRxApsMaxWindowSize;
  uint8_t iRxTotalBlocks;
  uint8_t iRxFragmentationGiveUpCounter;
  uint8_t iRxAckBitField;
}apsRxFragCtrlSettings_t;

typedef struct apsTxFragCtrlSettings_tag
{
  uint8_t iTxWindowStart;
  uint8_t iTxApsMaxWindowSize;
  zbApsCounter_t iTxBlockCounter;
  zbApsCounter_t  iTxTotalBlocks;
  uint16_t iTxTotalLength;
}apsTxFragCtrlSettings_t;

/* Used for APS transmit (Tx) state machine. */
/* See also gApsMaxDataHandlingCapacity_c in BeeStackConfiguration.h */
typedef struct apsTxTable_tag
{
  apsTxState_t        state;          /* what state are we in? */
  apsTxFlags_t        flags;          /* what type of transmission is this? */
  uint8_t             cRetries;       /* # of left retries for ACK */
  zbApsCounter_t      orgApsCounter;  /* remember the original APS counter */ 
  zbApsConfirmId_t    apsConfirmId;   /* confirm Id */
  uint8_t             nsduHandle;     /* handle received from NWK layer */
  zbApsCounter_t      apsCounter;     /* APS counter for this transmission */
  zbStatus_t          status;         /* remember error status */
#if (gBindCapability_d)
  bindIndex_t         iBindIndex;     /* last found binding index */
#endif
  zbExtHdr_t          iAckBitField;   /* for fragmentation */
  zbApsdeDataReq_t    *pDataReq;      /* ptr to a "packed" copy of the original data request */
  afToApsdeMessage_t  *pMsg;          /* ptr to current msg, possibly built in Tx state machine */
} apsTxTable_t;


/* Fragmentation Rx state machine variables. all pointers are NULL if unused */
typedef struct apsRxFragmentationTable_tag
{
  apsRxState_t        state;          /* what state are we in? init*/
  uint8_t             keyUsed;        /* remember key used for security init*/
  uint8_t             blockNumber;    /* the block # currently being processed */
  zbApsCounter_t      apsCounter;     /* counter ID of this transaction */
  zbNwkAddr_t         aSrcAddr;       /* source addr for this transaction */  
  zbExtHdr_t          iAckBitField;   /* receive ACK bits */
  apsdeToAfMessage_t *pDataIndMsg;    /* ptr to data indication we're building */
  nldeApsMessage_t   *pMsg;           /* pointer to new message from NWK layer */
  nldeDataIndication_t *pOrgNldeReq;  /* pointer to original nlde-data.req from NWK layer, in packed form, for ACKs */
  tmrTimerID_t iRxTimerId;            /* timer id used to wait the next block*/
  apsRxFragCtrlSettings_t apsRxFragCtrlSettings;
} apsRxFragmentationTable_t;
/******************************************************************************
*******************************************************************************
* Public memory declarations
*******************************************************************************
******************************************************************************/

extern anchor_t gApsdeQueue;
extern anchor_t gNldeQueue;

/* for APS Tx State Machine */
extern apsTxTable_t gaApsTxTable[];
extern apsTxFragCtrlSettings_t apsTxFragCtrlSettings[];
#if gApsAckCapability_d
extern tmrTimerID_t gaApsTxTimerIDTable[];
#endif
extern uint8_t gMaxApsTxTableEntries;

/* for APS Rx Fragmentation State Machine */
extern apsRxFragmentationTable_t gApsRxFragmentationTable[];
extern uint8_t gApsRxFragmentationCapacity;
/******************************************************************************
*******************************************************************************
* Public functions
*******************************************************************************
******************************************************************************/

/* SAP handler to for APSDE-DATA.request */  
uint8_t AF_APSDE_SapHandler(afToApsdeMessage_t *pMsg);
  
/* SAP handler for APSDE-DATA.confirm and APSDE-DATA.indication */
uint8_t NLDE_APS_SapHandler(nldeApsMessage_t *pMsg);
      
/* actually processes the APSDE-DATA.request */
zbStatus_t APSDE_DATA_request(afToApsdeMessage_t * pMsg);

/* actually processes the APSDE-DATA.indication and APSDE-DATA.confirm */
void APS_ProcessNldeMessage(nldeApsMessage_t* pMsg);

/* called when an ACK times out, 
  or when time to send to the next binding entry on indirect data requests */
void APS_TxStateMachine(void);

/* initialize the retry */
void APS_ResetTxTable(void);

/* resets the fragmentation RxState machine */
void APS_FreeRxStateMachine(index_t iIndexRxTable);

/* free the entire linked list of Tx fragments, except the head. Use FragHdr as input. */
void FreeSubsequentFragments
  (
  zbTxFragmentedHdr_t *pHead  /* IN: header for linked list of fragments */
  );

/* free the entire linked list of Tx fragments, except the head. Use dataReq as input */
void FreeTxFragments
  (
  zbApsdeDataReq_t *pDataReq /* IN: data request with linked list at end */
  );

/* free linked list of Rx fragments, */  
void FreeRxFragments
  (
  zbRxFragmentedHdr_t * pFirstBuffer
  );

uint16_t GetLengthOfAllFragments
  (
  zbTxFragmentedHdr_t *pHead
  );

uint16_t AF_GetTotalLength
  (
  zbApsdeDataIndication_t *pIndication
  );


#endif /* _APS_DATA_INTERFACE_H_ */

