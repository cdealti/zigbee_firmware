/******************************************************************************
* Declarations for ZTC message type table definitions and related code.
*
* Copyright (c) 2008, Freescale, Inc.  All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from  Freescale Semiconductor.
*
* Most of the read-only data used by Ztc is in a different file. The MsgType
* tables are large enough that they get their own file, and their own header.
*
******************************************************************************/

#ifndef _ZtcMsgTypeInfo_h
#define _ZtcMsgTypeInfo_h

/******************************************************************************
*******************************************************************************
* Public type definitions
*******************************************************************************
******************************************************************************/

/* Fields in the ztcMsgFlagAndFormat_t field. */
#define gZtcMsgFlagMask_c       0x80
#define gZtcMsgFormatMask_c     ( ~gZtcMsgFlagMask_c )

/* Distinguish synchronous from asynchronous messages. */
#define gZtcMsgFlagSync_c       0x00
#define gZtcMsgFlagAsync_c      0x80
#define gZtcMsgFlagToDo_c       ( gZtcMsgFlagAsync_c )

#define ZtcMsgGetFlg( v )   ( v & gZtcMsgFlagMask_c )
#define ZtcMsgGetFmt( v )   ( v & gZtcMsgFormatMask_c )

typedef uint8_t ztcMsgFlagAndFormat_t;

/* When Ztc receives a message, it looks up the external SAP Id in the
 * master SAP Handler table. The entry for a SAP Handler contains a pointer
 * to another table of message descriptions by MsgType.
 *
 * Note that there is no end of table indication. The SAP Handler table
 * entry that points to a given message info table entry has a pointer to
 * a variable that contains the number of elements in the message info
 * table.
 *
 * There is a single one-bit flag that distinguishes synchronous from
 * asynchronous messages. To save space, this flag and the message
 * format are combined into a single field. Since not all compilers
 * generate efficient, or even correct, code for bit fields, the
 * composition/decomposition is done via the masks and macros above.
 */
typedef struct ztcMsgTypeInfo_tag {
  ztcMsgType_t msgType;
  ztcMsgType_t cnfType;
  index_t len;
  ztcMsgFlagAndFormat_t flagAndFormat;
} ztcMsgTypeInfo_t;

/****************************************************************************/

/* Ztc configuration messages are handled as a special case, and use
 * a different message type info structure.
 */
typedef void ( *pZtcConfigFunc_t )( void );

typedef struct ztcInternalMsgTypeInfo_tag {
  ztcMsgType_t msgType;
  pZtcConfigFunc_t pConfigFunc;
} ztcInternalMsgTypeInfo_t;

extern ztcInternalMsgTypeInfo_t const gaZtcInternalMsgTypeInfoTable[];
extern index_t const gZtcInternalMsgTypeInfoTableLen;

/******************************************************************************
*******************************************************************************
* Public prototypes
*******************************************************************************
******************************************************************************/

const ztcMsgTypeInfo_t *pZtcMsgTypeInfoFromMsgType( const ztcMsgTypeInfo_t *pMsgTypeTable,
                                                    const index_t msgTypeInfoLen,
                                                    const ztcMsgType_t msgType );

/* Declare the functions that handle Ztc configuration messages. */
#define ZtcMsgTbl( msgType, configFunc ) \
  extern void configFunc( void );
#include "ZtcMsgTypeInfoTbl.h"

#endif                                  /* #ifndef _ZtcMsgTypeInfo_h */
