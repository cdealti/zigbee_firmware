/******************************************************************************
* ZTC message type table definitions and related code.
*
* Copyright (c) 2008, Freescale, Inc.  All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from  Freescale Semiconductor.
*
******************************************************************************/

/* Define the tables that describe the Ztc-relevant properties associated
 * with the various Message Ids, and the utility functions used to find
 * things in those tables.
 *
 * There is one table per SAP Handler; eachtable contains information about
 * all of the Message Ids recognized by that SAP Handler.
 *
 * All Message Id infomation (well, almost all) is actually collected in
 * ZtcMsgTypeInfoTbl.h, with the information about a given Message Id on a
 * single line, within a macro. This file defines those macros to select
 * the desired part of the Message Id information, and then #include's
 * the tbl file.
 *
 * The tables here are never directly accesed by their names. The SAP Handler
 * info table contains a pointer to the appropriate Message Id information
 * table for each SAP Handler.
 */

#include "EmbeddedTypes.h"
#if gBeeStackIncluded_d
#include "ZigBee.h"
#include "BeeStackFunctionality.h"
#include "BeeStackConfiguration.h"
#endif

#include "ZtcInterface.h"

#if gZtcIncluded_d

#if gBeeStackIncluded_d
#include "BeeCommon.h"
#endif

#include "AppAspInterface.h"
#include "ZtcSAPHandlerInfo.h"
#if gBeeStackIncluded_d
#include "AppZdoInterface.h"
#include "NwkMacInterface.h"
#include "ZdoApsInterface.h"
#include "AppZdoInterface.h"
#include "AfApsInterface.h"
#include "NwkCommon.h"
#endif
#include "AspZtc.h"
#include "LlcZtc.h"


#include "ZtcSAPHandlerInfo.h"
#include "ZtcPrivate.h"
#include "ZtcClientCommunication.h"
#include "ZtcMsgTypeInfo.h"
#include "ZtcCopyData.h"

/******************************************************************************
*******************************************************************************
* Public memory definitions
*******************************************************************************
******************************************************************************/

/* Note that for each table, there is also a table length index_t. The code
 * needs some way to know how many rows are in each table, so the main SAP
 * Handler table contains a pointer to this length variable. This costs
 * one index_t for the length variable plus one (index_t *) for the pointer
 * to it, per SAP Handler. Fortunately, there aren't very many SAP Handlers.
 *
 * There are other ways to make the lengths of the message info tables
 * available for use in the SAP Handler table that would not require the
 * pointers, but they involve some C language and/or pre-processor tricks
 * that are not obvious, and may not work in all compilers.
 */

/* opcode group 0x87, NWK_MCPS_SapHandler() */
#if gSAPMessagesEnableMcps_d
#define NwkMcpsTbl(msgType, cnfType, flags, len, format) \
    {msgType, cnfType, len, (flags | format)},
ztcMsgTypeInfo_t const gaZtcNwkMcpsMsgTypeTable[] = {
#include "ZtcMsgTypeInfoTbl.h"
};
index_t const gZtcNwkMcpsMsgTypeTableLen = NumberOfElements(gaZtcNwkMcpsMsgTypeTable);
#endif

/* opcode group 0x86, MCPS_NWK_SapHandler() */
#if gSAPMessagesEnableMcps_d
#define McpsNwkTbl(msgType, cnfType, flags, len, format) \
    {msgType, cnfType, len, (flags | format)},
ztcMsgTypeInfo_t const gaZtcMcpsNwkMsgTypeTable[] = {
#include "ZtcMsgTypeInfoTbl.h"
};
index_t const gZtcMcpsNwkMsgTypeTableLen = NumberOfElements(gaZtcMcpsNwkMsgTypeTable);
#endif

/* opcode group_c = 0x85, NWK_MLME_SapHandler() */
#if gSAPMessagesEnableMlme_d
#define NwkMlmeTbl(msgType, cnfType, flags, len, format) \
    {msgType, cnfType, len, (flags | format)},
ztcMsgTypeInfo_t const gaZtcNwkMlmeMsgTypeTable[] = {
#include "ZtcMsgTypeInfoTbl.h"
};
index_t const gZtcNwkMlmeMsgTypeTableLen = NumberOfElements(gaZtcNwkMlmeMsgTypeTable);
#endif

/* opcode group 0x84, MLME_NWK_SapHandler() */
#if gSAPMessagesEnableMlme_d
#define MlmeNwkTbl(msgType, cnfType, flags, len, format) \
    {msgType, cnfType, len, (flags | format)},
ztcMsgTypeInfo_t const gaZtcMlmeNwkMsgTypeTable[] = {
#include "ZtcMsgTypeInfoTbl.h"
};
index_t const gZtcMlmeNwkMsgTypeTableLen = NumberOfElements(gaZtcMlmeNwkMsgTypeTable);
#endif

/* opcode group_c = 0x95, APP_ASP_SapHandler() */
#if gSAPMessagesEnableAsp_d
#define NwkAspTbl(msgType, cnfType, flags, len, format) \
    {msgType, cnfType, len, (flags | format)},
ztcMsgTypeInfo_t const gaZtcNwkAspMsgTypeTable[] = {
#include "ZtcMsgTypeInfoTbl.h"
};
index_t const gZtcNwkAspMsgTypeTableLen = NumberOfElements(gaZtcNwkAspMsgTypeTable);
#endif

/* opcode group_c = 0xB0, APP_LLC_SapHandler() */
#if gSAPMessagesEnableLlc_d
#define NwkLlcTbl(msgType, cnfType, flags, len, format) \
    {msgType, cnfType, len, (flags | format)},
ztcMsgTypeInfo_t const gaZtcNwkLlcMsgTypeTable[] = {
#include "ZtcMsgTypeInfoTbl.h"
};
index_t const gZtcNwkLlcMsgTypeTableLen = NumberOfElements(gaZtcNwkLlcMsgTypeTable);
#endif

/* opcode group 0x94, ASP_APP_SapHandler() */
#if gSAPMessagesEnableAsp_d
#define AspNwkTbl(msgType, cnfType, flags, len, format) \
    {msgType, cnfType, len, (flags | format)},
ztcMsgTypeInfo_t const gaZtcAspNwkMsgTypeTable[] = {
#include "ZtcMsgTypeInfoTbl.h"
};
index_t const gZtcAspNwkMsgTypeTableLen = NumberOfElements(gaZtcAspNwkMsgTypeTable);
#endif

/* opcode group_c = 0x9A, APS_NLDE_SapHandler() */
#if gBeeStackIncluded_d && gSAPMessagesEnableNlde_d
#define ApsNldeTbl(msgType, cnfType, flags, len, format) \
    {msgType, cnfType, len, (flags | format)},
ztcMsgTypeInfo_t const gaZtcApsNldeMsgTypeTable[] = {
#include "ZtcMsgTypeInfoTbl.h"
};
index_t const gZtcApsNldeMsgTypeTableLen = NumberOfElements(gaZtcApsNldeMsgTypeTable);
#endif

/* opcode group_c = 0x9B, NLDE_APS_SapHandler() */
#if gBeeStackIncluded_d && gSAPMessagesEnableNlde_d
#define NldeApsTbl(msgType, cnfType, flags, len, format) \
    {msgType, cnfType, len, (flags | format)},
ztcMsgTypeInfo_t const gaZtcNldeApsMsgTypeTable[] = {
#include "ZtcMsgTypeInfoTbl.h"
};
index_t const gZtcNldeApsMsgTypeTableLen = NumberOfElements(gaZtcNldeApsMsgTypeTable);
#endif

/* opcode group_c = 0x96, ZDO_NLME_SapHandler() */
#if gBeeStackIncluded_d && gSAPMessagesEnableNlme_d
#define ZdoNlmeTbl(msgType, cnfType, flags, len, format) \
    {msgType, cnfType, len, (flags | format)},
ztcMsgTypeInfo_t const gaZtcZdoNlmeMsgTypeTable[] = {
#include "ZtcMsgTypeInfoTbl.h"
};
index_t const gZtcZdoNlmeMsgTypeTableLen = NumberOfElements(gaZtcZdoNlmeMsgTypeTable);
#endif

/* opcode group_c = 0x97, NLME_ZDO_SapHandler() */
#if gBeeStackIncluded_d && gSAPMessagesEnableNlme_d
#define NlmeZdoTbl(msgType, cnfType, flags, len, format) \
    {msgType, cnfType, len, (flags | format)},
ztcMsgTypeInfo_t const gaZtcNlmeZdoMsgTypeTable[] = {
#include "ZtcMsgTypeInfoTbl.h"
};
index_t const gZtcNlmeZdoMsgTypeTableLen = NumberOfElements(gaZtcNlmeZdoMsgTypeTable);
#endif

/* opcode group_c = 0x9C, AF_APSDE_SapHandler() */
#if gBeeStackIncluded_d && gSAPMessagesEnableApsde_d
#define AfApsdeTbl(msgType, cnfType, flags, len, format) \
    {msgType, cnfType, len, (flags | format)},
ztcMsgTypeInfo_t const gaZtcAfApsdeMsgTypeTable[] = {
#include "ZtcMsgTypeInfoTbl.h"
};
index_t const gZtcAfApsdeMsgTypeTableLen = NumberOfElements(gaZtcAfApsdeMsgTypeTable);
#endif

/* opcode group_c = 0x9D, APSDE_AF_SapHandler() */
#if gBeeStackIncluded_d && gSAPMessagesEnableApsde_d
#define ApsdeAfTbl(msgType, cnfType, flags, len, format) \
    {msgType, cnfType, len, (flags | format)},
ztcMsgTypeInfo_t const gaZtcApsdeAfMsgTypeTable[] = {
#include "ZtcMsgTypeInfoTbl.h"
};
index_t const gZtcApsdeAfMsgTypeTableLen = NumberOfElements(gaZtcApsdeAfMsgTypeTable);
#endif

/* opcode group_c = 0x9E, AF_AppToAfdeDataReq() */
#if gBeeStackIncluded_d && gSAPMessagesEnableAfde_d
#define AfdeAppTbl(msgType, cnfType, flags, len, format) \
    {msgType, cnfType, len, (flags | format)},
ztcMsgTypeInfo_t const gaZtcAfdeAppMsgTypeTable[] = {
#include "ZtcMsgTypeInfoTbl.h"
};
index_t const gZtcAfdeAppMsgTypeTableLen = NumberOfElements(gaZtcAfdeAppMsgTypeTable);
#endif

/* opcode group_c = 0x9F, AF_AppToAfdeDataReq() */
#if gBeeStackIncluded_d && gSAPMessagesEnableAfde_d
#define AppAfdeTbl(msgType, cnfType, flags, len, format) \
    {msgType, cnfType, len, (flags | format)},
ztcMsgTypeInfo_t const gaZtcAppAfdeMsgTypeTable[] = {
#include "ZtcMsgTypeInfoTbl.h"
};
index_t const gZtcAppAfdeMsgTypeTableLen = NumberOfElements(gaZtcAppAfdeMsgTypeTable);
#endif

/* opcode group_c = 0x99, ZDO_APSME_SapHandler() */
#if gBeeStackIncluded_d && gSAPMessagesEnableApsme_d
#define ZdoApsmeTbl(msgType, cnfType, flags, len, format) \
    {msgType, cnfType, len, (flags | format)},
ztcMsgTypeInfo_t const gaZtcZdoApsmeMsgTypeTable[] = {
#include "ZtcMsgTypeInfoTbl.h"
};
index_t const gZtcZdoApsmeMsgTypeTableLen = NumberOfElements(gaZtcZdoApsmeMsgTypeTable);
#endif

/* opcode group_c = 0x98, APSME_ZDO_SapHandler() */
#if gBeeStackIncluded_d && gSAPMessagesEnableApsme_d
#define ApsmeZdoTbl(msgType, cnfType, flags, len, format) \
    {msgType, cnfType, len, (flags | format)},
ztcMsgTypeInfo_t const gaZtcApsmeZdoMsgTypeTable[] = {
#include "ZtcMsgTypeInfoTbl.h"
};
index_t const gZtcApsmeZdoMsgTypeTableLen = NumberOfElements(gaZtcApsmeZdoMsgTypeTable);
#endif

/* opcode group_c = 0xA2, APP_ZDP_SapHandler() */
#if gBeeStackIncluded_d && gSAPMessagesEnableZdp_d
#define AppZdpTbl(msgType, cnfType, flags, len, format) \
    {msgType, cnfType, len, (flags | format)},
ztcMsgTypeInfo_t const gaZtcAppZdpMsgTypeTable[] = {
#include "ZtcMsgTypeInfoTbl.h"
};
index_t const gZtcAppZdpMsgTypeTableLen = NumberOfElements(gaZtcAppZdpMsgTypeTable);
#endif

/* opcode group 0xA0, ZDP_APP_SapHandler() */
#if gBeeStackIncluded_d && gSAPMessagesEnableZdp_d
#define ZdpAppTbl(msgType, cnfType, flags, len, format) \
    {msgType, cnfType, len, (flags | format)},
ztcMsgTypeInfo_t const gaZtcZdpAppMsgTypeTable[] = {
#include "ZtcMsgTypeInfoTbl.h"
};
index_t const gZtcZdpAppMsgTypeTableLen = NumberOfElements(gaZtcZdpAppMsgTypeTable);
#endif

/* opcode group_c = 0xA5, APP_InterPan_SapHandler() */
#if gBeeStackIncluded_d && gSAPMessagesEnableInterPan_d
#define AppInterPanTbl(msgType, cnfType, flags, len, format) \
    {msgType, cnfType, len, (flags | format)},
ztcMsgTypeInfo_t const gaZtcAppInterPanMsgTypeTable[] = {
#include "ZtcMsgTypeInfoTbl.h"
};
index_t const gZtcAppInterPanMsgTypeTableLen = NumberOfElements(gaZtcAppInterPanMsgTypeTable);
#endif

/* opcode group 0xA6, InterPan_APP_SapHandler() */
#if gBeeStackIncluded_d && gSAPMessagesEnableInterPan_d
#define InterPanAppTbl(msgType, cnfType, flags, len, format) \
    {msgType, cnfType, len, (flags | format)},
ztcMsgTypeInfo_t const gaZtcInterPanAppMsgTypeTable[] = {
#include "ZtcMsgTypeInfoTbl.h"
};
index_t const gZtcInterPanAppMsgTypeTableLen = NumberOfElements(gaZtcInterPanAppMsgTypeTable);
#endif

/* opcode group_c = 0xB6, APP_Hc_SapHandler() */
#if gBeeStackIncluded_d && gSAPMessagesEnableHc_d
#define AppHcTbl(msgType, cnfType, flags, len, format) \
    {msgType, cnfType, len, (flags | format)},
ztcMsgTypeInfo_t const gaZtcAppHcMsgTypeTable[] = {
#include "ZtcMsgTypeInfoTbl.h"
};
index_t const gZtcAppHcMsgTypeTableLen = NumberOfElements(gaZtcAppHcMsgTypeTable);
#endif

/* opcode group 0xB5, Hc_APP_SapHandler() */
#if gBeeStackIncluded_d && gSAPMessagesEnableHc_d
#define HcAppTbl(msgType, cnfType, flags, len, format) \
    {msgType, cnfType, len, (flags | format)},
ztcMsgTypeInfo_t const gaZtcHcAppMsgTypeTable[] = {
#include "ZtcMsgTypeInfoTbl.h"
};
index_t const gZtcHcAppMsgTypeTableLen = NumberOfElements(gaZtcHcAppMsgTypeTable);
#endif


/* Ztc handles all requests internally, and needs less information about
 * message types than the SAP Handlers need.
 */
#define ZtcMsgTbl(msgType, configFunc) \
  {msgType, configFunc},
ztcInternalMsgTypeInfo_t const gaZtcInternalMsgTypeInfoTable[] = {
#include "ZtcMsgTypeInfoTbl.h"
};

index_t const gZtcInternalMsgTypeInfoTableLen = NumberOfElements(gaZtcInternalMsgTypeInfoTable);



/******************************************************************************
*******************************************************************************
* Private memory declarations
*******************************************************************************
******************************************************************************/

/******************************************************************************
*******************************************************************************
* Public functions
*******************************************************************************
******************************************************************************/

/* Given a pointer to a message info table and a message type, return a */
/* pointer to the entry describing the message type. */
ztcMsgTypeInfo_t const *pZtcMsgTypeInfoFromMsgType
  (
  ztcMsgTypeInfo_t const *pMsgTypeTable,
  index_t const msgTypeInfoLen,
  ztcMsgType_t const msgType
  )
{
  index_t i;

  for (i = 0; i < msgTypeInfoLen; ++i) {
    if (pMsgTypeTable[i].msgType == msgType) {
      return &(pMsgTypeTable[i]);
    }
  }

  return NULL;
}                                       /* pZtcPtrToMsgTypeInfo() */

#endif                                  /* #if gZtcIncluded_d == 1 */
