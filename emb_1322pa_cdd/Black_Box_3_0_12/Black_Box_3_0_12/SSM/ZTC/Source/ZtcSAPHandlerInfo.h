/******************************************************************************
* ZtcSAPHandlerInfo.h
* This module contains a fresh implementation of the ZigBee Test Client (ZTC).
*
* Copyright (c) 2008, Freescale, Inc.  All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from  Freescale Semiconductor.
*
* This file contains declarations of read-only data used by Ztc.
* The opcode description tables are in separate files.
*
******************************************************************************/

#ifndef _ZtcSAPHandlerInfo_h
#define _ZtcSAPHandlerInfo_h

#include "TS_Interface.h"
#include "TS_Kernel.h"
#include "ZtcPrivate.h"
#include "ZtcMsgTypeInfo.h"
#include "MsgSystem.h"

/******************************************************************************
*******************************************************************************
* Public macros
*******************************************************************************
******************************************************************************/

/******************************************************************************
*******************************************************************************
* Public type definitions
*******************************************************************************
******************************************************************************/

/* Each SAP handler can be monitored, hooked or left alone (disable)
 *
 * Note that the Test Tool defines hook mode to include monitor mode.
 *
 * The Test Tool sets / checks these modes by providing or asking for
 * a small stream of byte values. The index values into the ztcSAPModeTable[]
 * correspond to the positions in that byte stream. The index values are
 * created by the SapMode() macro, and the correspondance between index
 * values and SAP Handlers is established in the Sap() macros.
 */
#define gZtcSAPModeDisable_c    0
#define gZtcSAPModeHook_c       1
#define gZtcSAPModeMonitor_c    2
#define gZtcSAPModeInvalid_c    0xFF    /* Reserve and invalid value. */
typedef uint8_t ztcSAPMode_t;

/* Current modes for SAP Handlers, grouped by SapMode() index. */
extern ztcSAPMode_t maZtcSAPModeTable[];
extern const index_t mZtcSAPModeTableLen;   /* Number of elements in maZtcSAPModeTable[]. */

/****************************************************************************/

/* Define the index identifiers into maZtcSAPModeTable[]. */
#define SapMode( name, index, defaultMode ) \
  name,

enum {
#include "ZtcSAPHandlerInfoTbl.h"
};

/****************************************************************************/

/* Some SAP Handlers code their message types, such that a bit is used to
 * distinguish between request and response types.
 * Fields in the ztcSAPHandlerFlags_t flags field.
 */
#define gZtcCheckRespBitMask_c      0x01

#define gZtcIgnoreRespBit_c         0
#define gZtcCheckRespBit_c          ( gZtcCheckRespBitMask_c )

typedef uint8_t ztcSAPHandlerFlags_t;

/****************************************************************************/

/* Type of each entry in the main SAP Handler information table, */
/* maZtcSAPHandlerInfo[] */
typedef struct ztcSAPHandlerInfo_tag {
  ztcOpcodeGroup_t const opcodeGroup;
  ztcOpcodeGroup_t const converseOpcodeGroup;
  ztcIntSAPId_t const intSAPId;
  pfnZtcSapHandler_t const pSAPHandler;
  index_t const modeIndex;
  anchor_t *const pMsgQueue;
  uint8_t msgEvent;
  tsTaskID_t const *pTaskID;
  ztcMsgTypeInfo_t const *pMsgTypeTable;
  index_t const *pMsgTypeTableLen;
} ztcSAPHandlerInfo_t;

/******************************************************************************
*******************************************************************************
* Public prototypes
*******************************************************************************
******************************************************************************/

const ztcSAPHandlerInfo_t *pZtcSAPInfoFromOpcodeGroup( const ztcOpcodeGroup_t opcodeGroup );
const ztcSAPHandlerInfo_t *pZtcSAPInfoFromIntSAPId( const ztcIntSAPId_t intSAPId );

extern uint8_t NWK_MCPS_SapHandler();
extern uint8_t MCPS_NWK_SapHandler();
extern uint8_t NWK_MLME_SapHandler();
extern uint8_t MLME_NWK_SapHandler();
extern uint8_t APP_ASP_SapHandler();
extern uint8_t APP_LLC_SapHandler();
extern uint8_t ASP_APP_SapHandler();
extern uint8_t APS_NLDE_SapHandler();
extern uint8_t NLDE_APS_SapHandler();
extern uint8_t ZDO_NLME_SapHandler();
extern uint8_t NLME_ZDO_SapHandler();
extern uint8_t AF_APSDE_SapHandler();
extern uint8_t APSDE_AF_SapHandler();
extern uint8_t AF_AppToAfdeDataReq();
extern uint8_t ZDO_APSME_SapHandler();
extern uint8_t APSME_ZDO_SapHandler();
extern uint8_t APP_ZDP_SapHandler();
extern uint8_t ZDP_APP_SapHandler();


/******************************************************************************
*******************************************************************************
* Public memory declarations
*******************************************************************************
******************************************************************************/

/* The main SAP Handler information table. */
extern const ztcSAPHandlerInfo_t maZtcSAPHandlerInfo[];

/* The number of elements in ztcSAPHandlerInfo[]. */
extern const uint8_t mZtcSapHandlerTableLen;

/* Headers of message queues used by the SAP Handlers. */
extern anchor_t mMcpsNwkInputQueue;     /* MCPS_NWK_SapHandler */
extern anchor_t mMlmeNwkInputQueue;     /* MLME_NWK_SapHandler */
extern anchor_t mZdoNlmeInputQueue;     /* ZDO_NLME_SapHandler */
extern anchor_t mApsNldeInputQueue;     /* APS_NLDE_SapHandler */

extern anchor_t gNldeQueue;             /* NLDE_APS_SapHandler */
extern anchor_t mApsmeQueue;            /* ZDO_APSME_SapHandler */
extern anchor_t gApsdeQueue;            /* AF_APSDE_SapHandler */
extern anchor_t gApsToAfdeQ;            /* APSDE_AF_SapHandler */

extern anchor_t mNlmeZdoInputQueue;     /* NLME_ZDO_SapHandler */
extern anchor_t mApsmeZdoInputQueue;    /* APSME_ZDO_SapHandler */
extern anchor_t mAppZdpInputQueue;      /* APP_ZDP_SapHandler */


#endif                                  /* #ifndef _ZtcSAPHandlerInfo_h */
