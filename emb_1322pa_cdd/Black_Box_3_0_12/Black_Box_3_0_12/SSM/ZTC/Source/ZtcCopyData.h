/******************************************************************************
* Declarations for the ZTC data format conversion routines.
*
* Copyright (c) 2008, Freescale, Inc.  All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from  Freescale Semiconductor.
*
******************************************************************************/

#ifndef _ZtcCopyData_h
#define _ZtcCopyData_h

#include "UART_Interface.h"
#include "FunctionalityDefines.h"

/******************************************************************************
*******************************************************************************
* Private macros
*******************************************************************************
******************************************************************************/

/******************************************************************************
*******************************************************************************
* Private type definitions
*******************************************************************************
******************************************************************************/
#if gBeeStackIncluded_d
typedef struct ztcFmtHdrAndData_tag {
  index_t hdrLen;
  index_t offsetToCount;
  index_t itemLen;
} ztcFmtHdrAndData_t;

#define FmtHdrAndData( name, hdrLen, offsetToCount, itemLen ) \
  name,

enum {
#include "ZtcCopyDataTbl.h"
};
#endif /* gBeeStackIncluded_d */

/****************************************************************************/

/* Types related to the index-to-length-plus-pointer data format. */

#if mZtcLenCodeAndPtrFormat_d
typedef struct ztcCodeAndLen_tag {
  index_t code;
  index_t len;
} ztcCodeAndLen_t;

typedef struct ztcFmtLenCodeAndPtr_tag {
  index_t hdrLen;
  index_t offsetToPtr;
  index_t offsetToCount;
  index_t offsetToLenCode;
  ztcCodeAndLen_t const *pCodeAndLenTable;
  index_t codeAndLenTableLen;
  index_t offsetToMsgData;
} ztcFmtLenCodeAndPtr_t;

#define FmtLenCodeAndPtr( name, hdrLen, offsetToPtr, offsetToCount, \
                          offsetToLenCode, tableOfDataLengths, offsetToMsgData ) \
  name,

enum {
#include "ZtcCopyDataTbl.h"
};
#endif                                  /* #if mZtcLenCodeAndPtrFormat_d */

/****************************************************************************/

typedef struct ztcFmtTwoArrayPtrs_tag {
  index_t hdrLen;

  index_t offsetToPtr1;
  index_t offsetToNumberOfArray1Elements;
  index_t sizeofArray1Element;
  index_t offsetToMsgData;

  index_t offsetToPtr2;
  index_t offsetToNumberOfArray2Elements;
  index_t sizeofArray2Element;
} ztcFmtTwoArrayPtrs_t;

#define FmtTwoArrayPtrs( name, hdrLen, \
                         offsetToPtr1, offsetToCount1, array1ElementLen, \
                         offsetToMsgData, \
                         offsetToPtr2, offsetToCount2, array2ElementLen ) \
  name,

enum {
#include "ZtcCopyDataTbl.h"
};

/******************************************************************************
*******************************************************************************
* Private prototypes
*******************************************************************************
******************************************************************************/

/******************************************************************************
*******************************************************************************
* Public macros
*******************************************************************************
******************************************************************************/

/* Return value from the copy routines if the source data is too big to copy. */
#define gTooBig_c (( uint8_t ) 0xFF )

/******************************************************************************
*******************************************************************************
* Public type definitions
*******************************************************************************
******************************************************************************/

/* pDst points to the clientPkt_t.header.statusOrData.data.
 * pSrc points to the sapHandlerMsg_t.header.data.
 * Returns the size of the destination data[].
 * These functions copy only the data[], not the header.
 */
typedef index_t ( *pZtcPktFromMsg_t )( uint8_t *pDst,
                                       index_t dstLen,
                                       uint8_t *pSrc,
                                       index_t srcLen,
                                       index_t fmtParametersIndex );

/****************************************************************************/

/* pDst points to the sapHandlerMsg_t.header.data. */
/* pSrc points to a clientPkt_t.header.statusOrData.data. */
/* srcLen is the value of the clientPkt_t.header.len field. */
/* Returns the size of the destination data[]. */
/* These functions copy only the data[], not the header. */
typedef index_t ( *pZtcMsgFromPkt_t )( uint8_t *pDst,
                                       index_t dstLen,
                                       uint8_t *pSrc,
                                       index_t srcLen,
                                       index_t fmtParametersIndex );

/****************************************************************************/

/* Instead of pointers to functions, the main format table saves space by */
/* storing indexes into tables of functions. Declare the indexes. */
#define PktFromMsgFunction( index, pFunction )  index,
enum {
#include "ZtcCopyDataTbl.h"
};

#define MsgFromPktFunction( index, pFunction )  index,
enum {
#include "ZtcCopyDataTbl.h"
};

/****************************************************************************/
/* One entry in the main format table. */
#define ZtcGetPktFromMsgFuncIndex( v )  (( v ) & 0x0F )
#define ZtcGetMsgFromPktFuncIndex( v )  (( v ) >> 4 )
#define ZtcSetFmtFuncIndexes( pktFromMsg, msgFromPkt ) \
  (( msgFromPkt << 4 ) | ( pktFromMsg & 0x0F ))

typedef struct ztcFmtInfo_tag {
  uint8_t copyFunctionIndexes;
  index_t pktFromMsgParametersIndex;
  index_t msgFromPktParametersIndex;
} ztcFmtInfo_t;

/******************************************************************************
*******************************************************************************
* Public prototypes
*******************************************************************************
******************************************************************************/

/* Use the TODO versions if the correct table entry is uncertain. */
#define ZtcPktFromMsgTODO   ZtcPktFromMsgUnused
#define ZtcMsgFromPktTODO   ZtcMsgFromPktUnused

/* Note that functions may be used in more than one table entry, causing them
 * to be declared extern more than once. This is harmless.
 */
#define PktFromMsgFunction( index, pFunction ) \
  extern index_t pFunction( uint8_t *pDst, \
                            index_t dstLen, \
                            uint8_t *pSrc, \
                            index_t srcLen, \
                            index_t fmtParametersIndex );

#define MsgFromPktFunction( index, pFunction ) \
  extern index_t pFunction( uint8_t *pDst, \
                            index_t dstLen, \
                            uint8_t *pSrc, \
                            index_t srcLen, \
                            index_t fmtParametersIndex );

#include "ZtcCopyDataTbl.h"

/******************************************************************************
*******************************************************************************
* Public memory declarations
*******************************************************************************
******************************************************************************/

extern ztcFmtInfo_t const maZtcFmtInfoTable[];
extern pZtcMsgFromPkt_t const maPktFromMsgFunctionTable[];
extern pZtcMsgFromPkt_t const maMsgFromPktFunctionTable[];

/******************************************************************************
*******************************************************************************
* Public functions
*******************************************************************************
******************************************************************************/

#endif                                  /* #ifndef _ZtcCopyData_h */
