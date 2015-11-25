/******************************************************************************
* ZTC data format conversion routines.
*
* Copyright (c) 2008, Freescale, Inc.  All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
* Ztc needs to copy external packet client to/from SAP Handler message data.
* The sometimes the formats are just a length + data (simple format), but
* sometimes they are more complicated, involving variable length arrays
* and/or pointers to data.
*
* *warning* In general, no effort is made to verify the correctness or
* internal consistency of the data.
******************************************************************************/

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
#include "MsgSystem.h"
#include "FunctionLib.h"
#include "FunctionalityDefines.h"
#include "PublicConst.h"

#include "AppAspInterface.h"
#if gBeeStackIncluded_d
#include "AppZdoInterface.h"
#include "ZdoNwkInterface.h"
#include "ZdoApsInterface.h"
#include "AfApsInterface.h"
#include "NwkCommon.h"
#include "ZdpManager.h"
#ifdef gHcGenericApp_d
#if gHcGenericApp_d
#include "HcZtc.h"
#endif
#endif
#endif

#include "AspZtc.h"
#include "LlcZtc.h"

#include "ZtcPrivate.h"
#include "ZtcClientCommunication.h"
#include "ZtcMsgTypeInfo.h"
#include "ZtcSAPHandlerInfo.h"
#include "ZtcMacPIB.h"
#include "ZtcCopyData.h"

/******************************************************************************
*******************************************************************************
* Private macros
*******************************************************************************
******************************************************************************/
/* 26 channels bit mapped into 32 bits,ie 4 Bytes(0-3) */
#define ByteOffset              0x03
/* mask everything except 3rd */
#define MaskAllExceptThirdByte  0x04
#define ShiftRightByOne         0x01

/******************************************************************************
*******************************************************************************
* Private type definitions
*******************************************************************************
******************************************************************************/

/******************************************************************************
*******************************************************************************
* Private memory declarations
*******************************************************************************
******************************************************************************/
#if gBeeStackIncluded_d

/* Define the table used by the header-and-data data format. */
#define FmtHdrAndData(name, hdrLen, offsetToCount, itemLen) \
  {hdrLen, offsetToCount, itemLen},

ztcFmtHdrAndData_t const maZtcFmtHdrAndDataTable[] = {
#include "ZtcCopyDataTbl.h"
};
#endif /* gBeeStackIncluded_d */

#if gSAPMessagesEnableMlme_d || gSAPMessagesEnableNlme_d
static uint8_t maScanChannels[ 4 ];    /* Buffer to hold Scanned Channels */
#endif

/****************************************************************************/

/* Tables related to the length-code-and-pointer data format.
 *
 * Each entry in the main table for this format contains a pointer
 * to a subtable that specifies the data lengths associated with
 * particular values of the length code field.
 */

/* Used for MacSetPIBAttribute.Request (85 09), aka MLME-SET.request, */
/*      and MacGetPIBAttribute.Confirm (84 05), aka MLME-GET.confirm. */
#if mZtcLenCodeAndPtrFormat_d
#define CodeAndLenMacSetReq(index, len) \
  {index, len},
ztcCodeAndLen_t const maZtcMacSetDataLengthsTable[] = {
#include "ZtcCopyDataTbl.h"
};

#define FmtLenCodeAndPtr(name, hdrLen, offsetToPtr, offsetToCount, \
                         offsetToLenCode, tableOfDataLengths, offsetToMsgData) \
  {hdrLen, offsetToPtr, offsetToCount, offsetToLenCode, tableOfDataLengths, \
   NumberOfElements(tableOfDataLengths), offsetToMsgData},

ztcFmtLenCodeAndPtr_t const maZtcFmtLenCodeAndPtrTable[] = {
#include "ZtcCopyDataTbl.h"
};
#endif                                  /* #if mZtcLenCodeAndPtrFormat_d */

/****************************************************************************/

/* Define the table for the two-array-pointers data format. */
#define FmtTwoArrayPtrs(name, hdrLen, \
                        offsetToPtr1, offsetToCount1, array1ElementLen, \
                        offsetToMsgData,                                \
                        offsetToPtr2, offsetToCount2, array2ElementLen) \
  {hdrLen, \
   offsetToPtr1, offsetToCount1, array1ElementLen, offsetToMsgData, \
   offsetToPtr2, offsetToCount2, array2ElementLen},

ztcFmtTwoArrayPtrs_t const maZtcFmtTwoArrayPtrsTable[] = {
#include "ZtcCopyDataTbl.h"
};

/****************************************************************************/

/* Instead of pointers to functions, the main format table saves space by */
/* storing indexes into tables of functions. Declare the indexes. */
#define PktFromMsgFunction(index, pFunction)  pFunction,
pZtcMsgFromPkt_t const maPktFromMsgFunctionTable [] = {
#include "ZtcCopyDataTbl.h"
};

#define MsgFromPktFunction(index, pFunction)  pFunction,
pZtcMsgFromPkt_t const maMsgFromPktFunctionTable[] = {
#include "ZtcCopyDataTbl.h"
};

/****************************************************************************/

/* Define the main data format table.
 *
 * Each entry in the main message type info table contains an index to this
 *table.
 *
 * Each entry in this table contains the pointers-to-functions and indexes-to-
 * parameter tables used by one format.
 */
#define Fmt(name, pktFromMsgFuncIndex, pktFromMsgParamIndex, \
            msgFromPktFuncIndex, msgFromPktParamIndex)             \
  {ZtcSetFmtFuncIndexes(pktFromMsgFuncIndex, msgFromPktFuncIndex), \
   pktFromMsgParamIndex, msgFromPktParamIndex},
ztcFmtInfo_t const maZtcFmtInfoTable[] = {
#include "ZtcCopyDataTbl.h"
};

/******************************************************************************
*******************************************************************************
* Public functions
*******************************************************************************
******************************************************************************/

/* The source packet consists of a fixed length header followed immediately
 * by a variable number of fixed size data items. The header contains a field
 * that specifies the number of items.
 * Copy the source header + data to the destination unchanged.
 *
 * Source packet:
 *  | H | D |
 *    ^   ^
 *    |   +-- Variable length data.
 *    +------ Header, including:
 *                a field specifying the length (in bytes) of the data.
 *
 * Destination message:
 *  Same as source.
 */
#if gBeeStackIncluded_d

index_t ZtcMsgFromPktHdrAndData
  (
  uint8_t *pMsg,
  index_t msgLen,
  uint8_t *pPkt,
  index_t pktLen,
  index_t fmtParametersIndex
  )
{
  ztcFmtHdrAndData_t const *pFmtInfo;
  index_t totalDstLen;                  /* Risky on 8 bit CPUs. */

  (void) pktLen;

  /* Note that the information in maZtcFmtTwoArrayPtrsTable[] describes */
  /* the message header. The layout of the packet header must be inferred. */
  pFmtInfo = &(maZtcFmtHdrAndDataTable[fmtParametersIndex]);
  totalDstLen = pFmtInfo->hdrLen
                + (pPkt[pFmtInfo->offsetToCount] * pFmtInfo->itemLen);

  if (totalDstLen > msgLen) {
    return gTooBig_c;
  }

  FLib_MemCpy(pMsg, (void *) pPkt, totalDstLen);
  return totalDstLen;
}                                       /* ZtcMsgFromPktHdrAndData() */

#endif /* gBeeStackIncluded_d */
/****************************************************************************/

/* The packet contains a header and some number of fixed length array elements.
 * The length of the array elements is specified indirectly by a length code
 * in the packet header.
 *
 * There are some messages that use the length code approach, but that have
 * either a simgle data value instead of an array, and/or that have no pointer
 * (the data is immediately after the header). The secondary format table,
 * maZtcFmtLenCodeAndPtrTable[], may contain special values (-1) for the
 * offset-to-number-of-elements and/or offset-to-pointer fields to indicate
 * these exceptions.
 *
 * Copy the packet header to the message header. Look up the length code
 * in the code-and-lengths table. Copy the data from the packet to the
 * message, at the message offset specified by offsetToMsgData. Store a
 * pointer to the data at the offsetToPtr offset in the message header.
 *
 * Source packet:
 *  | F | A | L |
 *    ^   ^   ^
 *    |   |   +-- Last part of header.
 *    |   +------ Variable length array.
 *    +---------- First part of header, including:
 *                a length code field, and
 *                a count of the number of elements in the array.
 *
 * Destination message:
 *  | F | P | L | G | A |
 *    ^   ^   ^   ^
 *    |   |   |   |   +-- Variable length array.
 *    |   |   |   +------ Gap between pointer and data. May be any length.
 *    |   |   +---------- Last part of header.
 *    |   +-------------- Pointer to "A".
 *    +------------------ First part of header.
 */
#if mZtcLenCodeAndPtrFormat_d
index_t ZtcMsgFromPktLenCodeAndPtr
  (
  uint8_t *pMsg,
  index_t msgLen,
  uint8_t *pPkt,
  index_t pktLen,
  index_t fmtParametersIndex
  )
{
  index_t arrayLen;                     /* Risky on an 8 bit CPU. */
  index_t code;
  index_t elementLen;
  ztcFmtLenCodeAndPtr_t fmtInfo;
  index_t hdrTotalLen;
  index_t hdrFirstPartLen;
  index_t i;
  index_t numberOfArrayElements;
  index_t offsetToCount;
  index_t offsetToMsgArray;
  ztcCodeAndLen_t const *pCodeAndLenTable;
  index_t totalDstLen;                  /* Risky on an 8 bit CPU. */
  uint32_t auxArrayAddress;
  
  (void) pktLen;
  

  /* Note that the information in maZtcFmtLenCodeAndPtrTable[] describes */
  /* the message header. The layout of the packet header must be inferred. */
  FLib_MemCpy(&fmtInfo,
              (void *) &(maZtcFmtLenCodeAndPtrTable[fmtParametersIndex]),
              sizeof(fmtInfo));

  pCodeAndLenTable = fmtInfo.pCodeAndLenTable;
  code = pPkt[fmtInfo.offsetToLenCode];
  for (elementLen = (index_t) gMinusOne_c, i = 0; i < fmtInfo.codeAndLenTableLen; i++) {
    if (code == pCodeAndLenTable[i].code) {
      elementLen = pCodeAndLenTable[i].len;
    }
  }

  /* If the number of elements entry in the secondary format table is -1, */
  /* this format actually only has one array element, not a variable number */
  /* of them. */
  offsetToCount = fmtInfo.offsetToCount;
  numberOfArrayElements = 1;
  if (offsetToCount != (index_t) gMinusOne_c) {
    numberOfArrayElements = pPkt[offsetToCount];
  }

  offsetToMsgArray = fmtInfo.offsetToMsgData;
  arrayLen = elementLen * numberOfArrayElements;
  totalDstLen = offsetToMsgArray + arrayLen;

  if (elementLen == (index_t) gMinusOne_c) {
    ZtcError(gZtcUnknownPIB_c);
    return gTooBig_c;
  }

  if (totalDstLen > msgLen) {
    return gTooBig_c;
  }

  hdrTotalLen = fmtInfo.hdrLen;
  hdrFirstPartLen = fmtInfo.offsetToPtr;

  if (hdrFirstPartLen == (index_t) gMinusOne_c) {
    /* The header does not contain a pointer. */
    hdrFirstPartLen = hdrTotalLen;
  } else {
    /* Write the pointer. */
    /*  *((uint8_t **)(pMsg + hdrFirstPartLen)) = pMsg + offsetToMsgArray; */
    
    auxArrayAddress = (uint32_t)(pMsg + offsetToMsgArray);
    FLib_MemCpy(pMsg + hdrFirstPartLen, &auxArrayAddress, sizeofPointer);
    
    /* Copy the second part of the header. */
    FLib_MemCpy(pMsg + hdrFirstPartLen + sizeofPointer,
                (void *) (pPkt + hdrFirstPartLen),
                hdrTotalLen - hdrFirstPartLen - sizeofPointer);
  }                                     /* if (hdrFirstPartLen == ... */

  /* Copy the first part of the header, not including the pointer. */
  FLib_MemCpy(pMsg, (void *) pPkt, hdrFirstPartLen);

  /* Copy the array. */
  FLib_MemCpy(pMsg + offsetToMsgArray,
              (void *) (pPkt + hdrFirstPartLen),
              (index_t) arrayLen);

  return totalDstLen;
}                                       /* ZtcMsgFromLenCodeAndPtr() */
#endif                                  /* #if mZtcLenCodeAndPtrFormat_d */

/****************************************************************************/

/* Placeholder function to fill in the Fmt() table slot for message formats
 * that don't require message-from-packet copying.
 */
index_t ZtcMsgFromPktUnused
  (
  uint8_t *pMsg,
  index_t msgLen,
  uint8_t *pPkt,
  index_t pktLen,
  index_t fmtParametersIndex
  )
{
  /* Keep the compiler happy. */
  (void) pMsg;
  (void) msgLen;
  (void) pPkt;
  (void) pktLen;
  (void) fmtParametersIndex;

  return gTooBig_c;
}                                       /* ZtcMsgFromPktUnused() */

/****************************************************************************/

/* Copy functions for Extended Simple Descriptor Resp.
 * The Extended Simple Descriptor Resp. cmd. doesn't have the Length field for DATA,
 * so the lenght of the packet that need to be copy is calculated and depends of
 * Start Index, appNumInClusters,appNumOutClusters and the number of bytes that 
 * can be set in one req.
 */
/****************************************************************************/

#if gExtended_Simple_Desc_rsp_d
index_t ZtcMsgFromPktExtSimpleDescriptorResp
(
  uint8_t *pMsg,
  index_t msgLen,
  uint8_t *pPkt,
  index_t pktLen,
  index_t fmtParametersIndex
  ) 
{
  zbExtSimpleDescriptorResponse_t *pPktResp;
  uint16_t numClustersToCopy = 0;
  uint8_t maxClusterListPayload;
  uint8_t pktLength;
  uint8_t result;
  
  (void)msgLen;
  (void)pktLen;
  pPktResp = (zbExtSimpleDescriptorResponse_t *)pPkt;
  /* Total number of clusters*/
  numClustersToCopy = (uint16_t)pPktResp->appNumInClusters +(uint16_t)pPktResp->appNumOutClusters;
  /* How many clusters need to be copy? */
  if (numClustersToCopy > pPktResp->startIndex)
  	numClustersToCopy = numClustersToCopy - (uint16_t)pPktResp->startIndex;
  else
  	numClustersToCopy = 0;
  /* How many bytes can be load in the app clusters list */
  maxClusterListPayload = ZdoMaximumPayLoadSize() - MbrOfs(zbExtSimpleDescriptorResponse_t,appClusterList);
   /* Calc. the maxim app clusters list (in clusters number) */
  maxClusterListPayload = maxClusterListPayload/sizeof(zbClusterId_t);
  /* Check if the DATA can be loaded in the ZDP payload*/
  if (numClustersToCopy > maxClusterListPayload)
  	numClustersToCopy = maxClusterListPayload;
 
  pktLength =MbrOfs (zbExtSimpleDescriptorResponse_t, appClusterList) + (numClustersToCopy*sizeof(zbClusterId_t));

  result = ZtcMsgFromPktSimple(pMsg,pktLength,pPkt,pktLength,fmtParametersIndex);
  return result;
}


index_t ZtcPktFromMsgExtSimpleDescriptorResp
(  
uint8_t *pPkt,
index_t pktLen,
uint8_t *pMsg,
index_t msgLen,
index_t fmtParametersIndex
) 
{
  zbExtSimpleDescriptorResponse_t *pMsgResp;
  uint16_t numClustersToCopy = 0;
  uint8_t maxClusterListPayload;
  uint8_t msgLength;
  uint8_t result;
  
  (void)msgLen;
  (void)pktLen;
  pMsgResp = (zbExtSimpleDescriptorResponse_t *)pMsg;
  /* Total number of clusters*/
  numClustersToCopy = (uint16_t)pMsgResp->appNumInClusters +(uint16_t)pMsgResp->appNumOutClusters;
  /* How many clusters need to be copy? */
  if (numClustersToCopy > pMsgResp->startIndex)
  	numClustersToCopy = numClustersToCopy - (uint16_t)pMsgResp->startIndex;
  else
  	numClustersToCopy = 0;
  /* How many bytes can be loaded in the app clusters list */
  maxClusterListPayload = ZdoMaximumPayLoadSize() - MbrOfs(zbExtSimpleDescriptorResponse_t,appClusterList);
  /* Calc. the maxim app clusters list (in clusters number) that can be loaded in Zdp payload */
  maxClusterListPayload = maxClusterListPayload/sizeof(zbClusterId_t);
  if (numClustersToCopy > maxClusterListPayload)
  	numClustersToCopy = maxClusterListPayload;

  msgLength =MbrOfs (zbExtSimpleDescriptorResponse_t, appClusterList) + (numClustersToCopy*sizeof(zbClusterId_t));

  result = ZtcPktFromMsgSimple(pPkt,msgLength,pMsg,msgLength,fmtParametersIndex);
  return result;
}
#else
index_t ZtcMsgFromPktExtSimpleDescriptorResp
(
  uint8_t *pMsg,
  index_t msgLen,
  uint8_t *pPkt,
  index_t pktLen,
  index_t fmtParametersIndex
) 
{

  (void)pMsg;
  (void)msgLen;
  (void)pPkt;
  (void)pktLen;
  (void)fmtParametersIndex;
   return gTooBig_c;
}

index_t ZtcPktFromMsgExtSimpleDescriptorResp
(  
uint8_t *pPkt,
index_t pktLen,
uint8_t *pMsg,
index_t msgLen,
index_t fmtParametersIndex
) 
{
  (void)pPkt;
  (void)pktLen;
  (void)pMsg;
  (void)msgLen;
  (void)fmtParametersIndex;
  return gTooBig_c;
}

#endif /* #if gExtended_Simple_Desc_rsp_d */

/****************************************************************************/

/* Copy functions for Extended Active Ep Resp.
 * The Extended Active Ep Resp cmd. doesn't have the Length field for DATA,
 * so the lenght of the packet that need to be copy is calculated and depends of
 * Start Index, activeEpCount and the number of bytes that 
 * can be set in one req.
 */
/****************************************************************************/

#if gExtended_Active_EP_rsp_d
index_t ZtcMsgFromPktExtActiveEPResp
(
  uint8_t *pMsg,
  index_t msgLen,
  uint8_t *pPkt,
  index_t pktLen,
  index_t fmtParametersIndex
  ) 
{
  zbExtActiveEpResponse_t *pPktResp;
  uint8_t numOfActiveEpToCopy = 0;
  uint8_t maxActiveEpListPayload;
  uint8_t pktLength;
  uint8_t result;
  
  (void)msgLen;
  (void)pktLen;
  pPktResp = (zbExtActiveEpResponse_t *)pPkt;
  /* Total number of active Ep*/
  numOfActiveEpToCopy = pPktResp->activeEpCount ;
  /* How many Ep need to be copy? */
  if (numOfActiveEpToCopy > pPktResp->startIndex)
  	numOfActiveEpToCopy = numOfActiveEpToCopy - pPktResp->startIndex;
  else
  	numOfActiveEpToCopy = 0;
  /* How many bytes can be load in the Active Ep list  */
  maxActiveEpListPayload = ZdoMaximumPayLoadSize() - MbrOfs(zbExtActiveEpResponse_t, pActiveEpList[0]);
  /* Check if the DATA can be loaded in the ZDP payload*/
  if (numOfActiveEpToCopy > maxActiveEpListPayload)
  	numOfActiveEpToCopy = maxActiveEpListPayload;
 
  pktLength =MbrOfs (zbExtActiveEpResponse_t, pActiveEpList[0]) + numOfActiveEpToCopy;

  result = ZtcMsgFromPktSimple(pMsg,pktLength,pPkt,pktLength,fmtParametersIndex);
  return result;
}


index_t ZtcPktFromMsgExtActiveEPResp
(  
uint8_t *pPkt,
index_t pktLen,
uint8_t *pMsg,
index_t msgLen,
index_t fmtParametersIndex
) 
{
  zbExtActiveEpResponse_t *pMsgResp;
  uint8_t numOfActiveEpToCopy = 0;
  uint8_t maxActiveEpListPayload;
  uint8_t msgLength;
  uint8_t result;
  
  (void)msgLen;
  (void)pktLen;
  pMsgResp = (zbExtActiveEpResponse_t *)pMsg;
  /* Total number of active Ep*/
  numOfActiveEpToCopy = pMsgResp->activeEpCount;
  /* How many clusters need to be copy? */
  if (numOfActiveEpToCopy > pMsgResp->startIndex)
  	numOfActiveEpToCopy = numOfActiveEpToCopy - pMsgResp->startIndex;
  else
  	numOfActiveEpToCopy = 0;
  /* How many bytes can be loaded in the active Ep list */
  maxActiveEpListPayload = ZdoMaximumPayLoadSize() - MbrOfs(zbExtActiveEpResponse_t, pActiveEpList[0]);
  /* Check if the DATA can be loaded in the ZDP payload*/
  if (numOfActiveEpToCopy > maxActiveEpListPayload)
  	numOfActiveEpToCopy = maxActiveEpListPayload;

  msgLength = MbrOfs(zbExtActiveEpResponse_t, pActiveEpList[0]) + numOfActiveEpToCopy;

  result = ZtcPktFromMsgSimple(pPkt,msgLength,pMsg,msgLength,fmtParametersIndex);
  return result;
}
#else
index_t ZtcPktFromMsgExtActiveEPResp
(  
uint8_t *pPkt,
index_t pktLen,
uint8_t *pMsg,
index_t msgLen,
index_t fmtParametersIndex
)
{
  (void)pPkt;
  (void)pktLen;
  (void)pMsg;
  (void)msgLen;
  (void)fmtParametersIndex;
  return gTooBig_c;
}

index_t ZtcMsgFromPktExtActiveEPResp
(
  uint8_t *pMsg,
  index_t msgLen,
  uint8_t *pPkt,
  index_t pktLen,
  index_t fmtParametersIndex
) 
{
  (void)pMsg;
  (void)msgLen;
  (void)pPkt;
  (void)pktLen;
  (void)fmtParametersIndex;
    return gTooBig_c;
 }

#endif /* #if gExtended_Active_EP_rsp_d */


/****************************************************************************/

/* The message consists of a header that contains two pointers to arrays.
 * Each array is described by a pointer to the data, plus a count of the
 * number of elements in the array. The sizes of the elements in each array
 * are constant, but the size of the elements of the first array is is
 * generally different than the size of the elements of the second array.
 *
 * The header has a first part (before the first array pointer), a second
 * part (between the first and second array pointers), and a third part
 * (after the second array pointer).
 *
 * This format is also used if there is only one array and pointer, instead
 * of two.
 *
 * The packet format is similar to the message, but the pointers are replaced
 * by the data of the arrays.
 *
 * Copy the data to a fixed offset relative to the beginning of the message
 * (at ztcFmtPtrToArray_t.offsetToMsgData). Copy the struct from the packet
 * to the message, inserting the pointers to the destination arrays at the
 * needed places.
 *
 * *warning* The number-of-elements fields *must* occur earlier in the
 * header than the corresponding pointers.
 *
 * The source packet looks like:
 *
 *  | F | 1 | M | 2 | L |
 *    ^   ^   ^   ^   ^
 *    :   :   :   :   +---- Third part of header.
 *    :   :   :   +-------- Contents of second array.
 *    :   :   +------------ Second part of header.
 *    :   +---------------- Contents of first array.
 *    +-------------------- First part of header.
 *
 * The field that specifies the number of element in the first array
 * must be in the first part of the header.
 * The field that specifies the number of elements in the second array
 * may be in either the first or second parts of the header.
 *
 * The destination message looks like:
 *
 *  | F | P | M | Q | L | G | 1 | 2 |
 *    ^   ^   ^   ^   ^   ^   ^   ^
 *    :   :   :   :   :   :   :   +---- Second array data.
 *    :   :   :   :   :   :   +-------- First array data.
 *    :   :   :   :   :   +------------ Gap.
 *    :   :   :   :   +---------------- Third part of header.
 *    :   :   :   +-------------------- Pointer to second array.
 *    :   :   +------------------------ Second part of header.
 *    :   +---------------------------- Pointer to first array.
 *    +-------------------------------- First part of header.
 *
 */
index_t ZtcMsgFromPktTwoArrayPtrs
  (
  uint8_t *pMsg,                        /* Pointer to message payload. */
  index_t msgLen,                       /* Length  of message payload. */
  uint8_t *pPkt,                        /* Pointer to packet  payload. */
  index_t pktLen,                       /* Length  of packet  payload. */
  index_t fmtParametersIndex
  )
{
  ztcFmtTwoArrayPtrs_t fmtInfo;

  /* The first, second, and third parts of the message and packet headers */
  /* are identical. The arrays are identical, although they are at different */
  /* offsets in the message and packet formats. */
  index_t hdrFirstPartLen;
  index_t hdrSecondPartLen;
  index_t hdrThirdPartLen;
  index_t array1Len;
  index_t array2Len;

  index_t offsetToMsgHdrSecondPart;
  index_t offsetToMsgHdrThirdPart;
  index_t offsetToMsgArray1;
  index_t offsetToMsgArray2;

  index_t offsetToMsgPtr2;

  index_t offsetToPktNumberOfArray2Elements;

  index_t offsetToPktHdrSecondPart;
  index_t offsetToPktHdrThirdPart;
  index_t offsetToPktArray2;

  index_t totalDstLen;
  uint32_t auxArrayAddress;

  (void) pktLen;

  /* Note that the information in maZtcFmtTwoArrayPtrsTable[] describes */
  /* the message header. The layout of the packet header must be inferred. */
  FLib_MemCpy(&fmtInfo,
              (void *) &(maZtcFmtTwoArrayPtrsTable[fmtParametersIndex]),
              sizeof(fmtInfo));

  hdrFirstPartLen = fmtInfo.offsetToPtr1;

  array1Len = pPkt[fmtInfo.offsetToNumberOfArray1Elements]
              * fmtInfo.sizeofArray1Element;

  /* In the message header, the second part of the header begins after the */
  /* first pointer. */
  offsetToMsgHdrSecondPart = hdrFirstPartLen + sizeofPointer;

  /* The packet header does not contain the pointers. The second part of */
  /* the packet header begins after the first array. */
  offsetToPktHdrSecondPart = hdrFirstPartLen + array1Len;

  /* In the message header, the first array is stored at a fixed offset */
  /* relative to the beginning of the header. */
  offsetToMsgArray1 = fmtInfo.offsetToMsgData;

  /* If there are two pointers, the second one cannot be at offset 0. */
  offsetToMsgPtr2 = fmtInfo.offsetToPtr2;
  if (!offsetToMsgPtr2) {
    /* There is only one array. */

    /* The second part of the header is the last part of the header. */
    hdrSecondPartLen = fmtInfo.hdrLen - offsetToMsgHdrSecondPart;
    
    /* because the first pointer is replaced by the first array, from the*/
    /* total length of the packet (totalDstLen) must be subtracted sizeofPointer bytes */
    totalDstLen = offsetToMsgArray1 + array1Len - sizeofPointer;
  } else {
    /* There are two arrays. */

    /* The second part of the header ends at the second pointer. */
    hdrSecondPartLen = offsetToMsgPtr2 - offsetToMsgHdrSecondPart;

    /* In the packet, the first pointer is replaced by the first array, */
    /* shifting the position of the second part of the header. If the */
    /* number-of-elements field for the second array is in the second part */
    /* of the header, allow for that displacement. */
    offsetToPktNumberOfArray2Elements = fmtInfo.offsetToNumberOfArray2Elements;
    if (offsetToPktNumberOfArray2Elements >= offsetToMsgHdrSecondPart) {
      offsetToPktNumberOfArray2Elements += array1Len - sizeofPointer;
    }

    /* The message header contains a pointer to the first array. The packet */
    /* header does not contain the array pointers. */
    array2Len = pPkt[offsetToPktNumberOfArray2Elements]
              * fmtInfo.sizeofArray2Element;

    offsetToMsgArray2 = offsetToMsgArray1 + array1Len;
    /* because the first pointer is replaced by the first array, from the*/
    /* total length of the packet (totalDstLen) must be subtracted sizeofPointer bytes */

      totalDstLen       = offsetToMsgArray2 + array2Len - (2*sizeofPointer);
  }

  if (totalDstLen > msgLen) {
    return gTooBig_c;
  }

  /* Copy the first part of the header, up to the pointer. */
  FLib_MemCpy(pMsg, (void *) pPkt, hdrFirstPartLen);

  /* Write the first array pointer. */
 /*  *((uint8_t **)(pMsg + hdrFirstPartLen)) = (pMsg + offsetToMsgArray1); */

    auxArrayAddress = (uint32_t)(pMsg + offsetToMsgArray1);
    FLib_MemCpy(pMsg + hdrFirstPartLen, &auxArrayAddress, sizeofPointer);  

  /* Copy the second part of the header. */
  FLib_MemCpy((pMsg + offsetToMsgHdrSecondPart),
              (void *) (pPkt + offsetToPktHdrSecondPart),
              hdrSecondPartLen);

  /* If the elements of the second array are zero length, there is no */
  /* second array. */
  if (fmtInfo.sizeofArray2Element) {
    offsetToPktArray2         = offsetToPktHdrSecondPart + hdrSecondPartLen;
    offsetToMsgHdrThirdPart   = offsetToMsgPtr2 + sizeofPointer;
    offsetToPktHdrThirdPart   = offsetToPktArray2 + array2Len;
    hdrThirdPartLen           = fmtInfo.hdrLen - offsetToMsgPtr2 - sizeofPointer;

    /* Write the second array pointer. */
    /*
    *((uint8_t **)(pMsg + offsetToMsgPtr2)) = (pMsg + offsetToMsgArray2);
    */
    auxArrayAddress = (uint32_t)(pMsg + offsetToMsgArray2);
    FLib_MemCpy(pMsg + offsetToMsgPtr2, &auxArrayAddress, sizeofPointer);  

    /* Copy the third part of the header. */
    FLib_MemCpy((pMsg + offsetToMsgHdrThirdPart),
                (void *) (pPkt + offsetToPktHdrThirdPart),
                hdrThirdPartLen);

    /* Copy the second array. */
    FLib_MemCpy((pMsg + offsetToMsgArray2), (void *) (pPkt + offsetToPktArray2), array2Len);
  }

  /* Copy the first array. */
  FLib_MemCpy((pMsg + offsetToMsgArray1), (void *) (pPkt + hdrFirstPartLen  ), array1Len);

  return totalDstLen;
}                                       /* ZtcMsgFromPktTwoArrayPtrs() */

/****************************************************************************/

/* The packet contains a fixed length header, which is copied unchanged to
 * the message.
 *
 * Source packet:
 *  | H |
 *
 * Destination message:
 *  | H |
 */
index_t ZtcMsgFromPktSimple
  (
  uint8_t *pMsg,
  index_t msgLen,
  uint8_t *pPkt,
  index_t pktLen,
  index_t fmtParametersIndex
  )
{
  (void) fmtParametersIndex;          /* Keep the compiler happy. */

  if (pktLen > msgLen) {
    return gTooBig_c;
  }

  FLib_MemCpy(pMsg, (void *) pPkt, pktLen);
  return pktLen;
}                                       /* ZtcMsgFromPktSimple() */

/****************************************************************************/

/* The source message consists of a fixed length header followed immediately
 * by a variable number of fixed length data items. The header contains a
 * field that specifies the number of items.
 * Copy the source header + data to the destination unchanged.
 * Except for the function prototype, this is identical to
 * ZtcMsgFromPktHdrAndData().
 *
 * Source message:
 *  | H | D |
 *    ^   ^
 *    |   +-- variable length data
 *    +------ header, including length (in bytes) of data
 *
 * Destination packet:
 *  Same as source.
 *
 */
#if gBeeStackIncluded_d

index_t ZtcPktFromMsgHdrAndData
  (
  uint8_t *pPkt,
  index_t pktLen,
  uint8_t *pMsg,
  index_t msgLen,
  index_t fmtParametersIndex
  )
{
  ztcFmtHdrAndData_t const *pFmtInfo;
  index_t totalDstLen;                  /* Risky on an 8 but CPU. */

  (void) msgLen;

  /* Note that the information in maZtcFmtTwoArrayPtrsTable[] describes */
  /* the message header. The layout of the packet header must be inferred. */
  pFmtInfo = &(maZtcFmtHdrAndDataTable[fmtParametersIndex]);

  totalDstLen = pFmtInfo->hdrLen
              + (pMsg[pFmtInfo->offsetToCount] * pFmtInfo->itemLen);

  if (totalDstLen > pktLen) {
    return gTooBig_c;
  }

  FLib_MemCpy(pPkt, (void *) pMsg, totalDstLen);

  return totalDstLen;
}                                       /* ZtcPktFromMsgHdrAndData() */

#endif /* gBeeStackIncluded_d */
/****************************************************************************/

/* The message contains a header and data. The header contains a pointer to
 * an array of fixed length elements, and the number of elements, and a code
 * value that can be looked up ina table to get the length of the elements.
 *
 * There are some messages that use the length code approach, but that have
 * either a simgle data value instead of an array, and/or that have no pointer
 * (the data is immediately after the header). The secondary format table,
 * maZtcFmtLenCodeAndPtrTable[], may contain special values (-1) for the
 * offset-to-number-of-elements and/or offset-to-pointer fields to indicate
 * these exceptions.
 *
 * In packet-from-message copies using the format, the field in the message
 * header that contains a pointer is replaced by a length field in the
 * packet header immediately followed by the array data.
 *
 * Copy the message header to the packet header. Look up the code in the
 * code-and-length table. THe Test Tool cannot do arithmetic, so it
 * cannot multiply the number of elements times the size of the elements to
 * find the total data length. Do the multiplication, and store the result
 * as a 16 bit value in the location where the message header's pointer is.
 * Copy the data pointed to by the message header to immediately after the
 * packet header.
 *
 * The offsetToMsgData field in the format parameter table is ignored in
 * packet-from-message copies.
 *
 * Source message:
 *  | F | P | S | G | A |
 *    ^   ^   ^   ^   ^
 *    |   |   |   |   +-- Variable length array.
 *    |   |   |   +------ Gap between pointer and data. May be any length.
 *    |   |   +---------- Second (and last) part of header.
 *    |   +-------------- Pointer to "A".
 *    +------------------ First part of header.
 *
 * Destination packet:
 *  | F | L | A | S |
 *    ^   ^    ^   ^
 *    |   |   |   +-- Second (and last) part of header.
 *    |   |   +------ Variable length array.
 *    |   +---------- Length of the array in bytes (16 bits).
 *    +-------------- First part of header, including:
 *                    a length code field, and
 *                    a count of the number of elements in the array.
 */
#if mZtcLenCodeAndPtrFormat_d
index_t ZtcPktFromMsgLenCodeAndPtr
  (
  uint8_t *pPkt,
  index_t pktLen,
  uint8_t *pMsg,
  index_t msgLen,
  index_t fmtParametersIndex
  )
{
  index_t arrayLen;                     /* Risky on an 8 bit CPU. */
  index_t code;
  ztcFmtLenCodeAndPtr_t fmtInfo;
  index_t elementLen;
  index_t hdrTotalLen;
  index_t hdrFirstPartLen;
  index_t i;
  index_t numberOfArrayElements;
  index_t offsetToCount;
  ztcCodeAndLen_t const *pCodeAndLenTable;
  /* uint8_t const *pMsgData; */
  uint8_t *pMsgData;
  index_t totalDstLen;                  /* Risky on an 8 bit CPU. */

  (void) msgLen;

  /* Note that the information in maZtcFmtTwoArrayPtrsTable[] describes */
  /* the message header. The layout of the packet header must be inferred. */
  FLib_MemCpy(&fmtInfo,
              (void *) &(maZtcFmtLenCodeAndPtrTable[fmtParametersIndex]),
              sizeof(fmtInfo));
  hdrTotalLen = fmtInfo.hdrLen;

  pCodeAndLenTable = fmtInfo.pCodeAndLenTable;
  code = pMsg[fmtInfo.offsetToLenCode];
  for (elementLen = (index_t) gMinusOne_c, i = 0; i < fmtInfo.codeAndLenTableLen; i++) {
    if (code == pCodeAndLenTable[i].code) {
      elementLen = pCodeAndLenTable[i].len;
    }
  }

  /* If the number of elements entry in the secondary format table is -1, */
  /* this format actually only has one array element, not a variable number */
  /* of them. */
  offsetToCount = fmtInfo.offsetToCount;
  numberOfArrayElements = 1;
  if (offsetToCount != (index_t) gMinusOne_c) {
    numberOfArrayElements = pMsg[offsetToCount];
  }
  arrayLen = elementLen * numberOfArrayElements;
  totalDstLen = hdrTotalLen + arrayLen;

  if (elementLen == (index_t) gMinusOne_c) {
    ZtcError(gZtcUnknownPIB_c);
  }

  if (totalDstLen > pktLen) {
    return gTooBig_c;
  }

  hdrFirstPartLen = fmtInfo.offsetToPtr;
  if (hdrFirstPartLen == (index_t) gMinusOne_c) {
    /* There is no pointer / length field. */
    hdrFirstPartLen = hdrTotalLen;
    pMsgData = pMsg + hdrTotalLen;
  } else {
    /* Store the length of the array. */
    /* *warning* This is brittle; it really should be a named typedef */
    /* instead of uint16_t. In the message this field is a pointer; in */
    /* the packet, its a length. Should be a union. Also has to agree */
    /* with the Test Tool. */
    uint16_t val = Swap2Bytes((uint16_t)arrayLen); 
    FLib_MemCpy((pPkt + hdrFirstPartLen), &val, 2);
    /* Copy the second part of the header. */
    FLib_MemCpy(pPkt + hdrFirstPartLen + sizeofPointer + arrayLen,
                (void *) (pMsg + hdrFirstPartLen + sizeofPointer),
                hdrTotalLen - hdrFirstPartLen - sizeofPointer);

    /* pMsgData = *((uint8_t **) (pMsg + hdrFirstPartLen));  */
    pMsgData = pMsg + hdrFirstPartLen+sizeofPointer;
  }

  /* Copy the first part of the header. */
  FLib_MemCpy(pPkt, (void *) pMsg, hdrFirstPartLen);

  /* Copy the data. */
  /* FLib_MemCpy(pPkt + hdrTotalLen, (void *) pMsgData, arrayLen); */
  FLib_MemCpy(pPkt + hdrTotalLen - 2, (void *) pMsgData, arrayLen);

  return totalDstLen;
}                                       /* ZtcPktFromMsgLenCodeAndPtr() */
#endif                                  /* #if mZtcLenCodeAndPtrFormat_d */

/****************************************************************************/

/* See the comments for ZtcMsgFromPktTwoArrayPtrs().
 *
 * The source message looks like:
 *
 *  | F | P | M | Q | L | G | 1 | 2 |
 *    ^   ^   ^   ^   ^   ^   ^   ^
 *    :   :   :   :   :   :   :   +---- Second array data.
 *    :   :   :   :   :   :   +-------- First array data.
 *    :   :   :   :   :   +------------ Gap.
 *    :   :   :   :   +---------------- Third part of header.
 *    :   :   :   +-------------------- Pointer to second array.
 *    :   :   +------------------------ Second part of header.
 *    :   +---------------------------- Pointer to first array.
 *    +-------------------------------- First part of header.
 *
 * The field that specifies the number of element in the first array
 * must be in the first part of the header.
 * The field that specifies the number of elements in the second array
 * may be in either the first or second parts of the header.
 *
 * The destination packet looks like:
 *
 *  | F | 1 | M | 2 | L |
 *    ^   ^   ^   ^   ^
 *    :   :   :   :   +---- Third part of header.
 *    :   :   :   +-------- Contents of second array.
 *    :   :   +------------ Second part of header.
 *    :   +---------------- Contents of first array
 *    +-------------------- First part of header.
 *
 * The field that specifies the number of elements in the second array may
 * be in either the first or second part of the header.
 */
index_t ZtcPktFromMsgTwoArrayPtrs
  (
  uint8_t *pPkt,
  index_t pktLen,
  uint8_t *pMsg,
  index_t msgLen,
  index_t fmtParametersIndex
  )
{
  ztcFmtTwoArrayPtrs_t fmtInfo;

  index_t array1Len;                    /* Risky on an 8 bit CPU. */
  index_t array2Len;                    /* Risky on an 8 bit CPU. */

  /* The first, second, and third parts of the msg and pkt headers are */
  /* identical. */
  index_t hdrLen;
  index_t hdrSecondPartLen;
  index_t hdrThirdPartLen;

  index_t offsetToMsgHdrPtr1;
  index_t offsetToMsgHdrSecondPart;
  index_t offsetToMsgHdrPtr2;
  index_t offsetToMsgHdrThirdPart;

  uint8_t *pPktArray1;
  uint8_t *pPktHdrSecondPart;
  uint8_t *pPktArray2;
  uint8_t *pPktHdrThirdPart;

/*  uint8_t **pMsgHdrPtrToArray1; */
/*  uint8_t **pMsgHdrPtrToArray2; */

  uint8_t *pMsgArray1;
  uint8_t *pMsgArray2;
  index_t totalDstLen;                  /* Risky on an 8 bit CPU. */

  (void) msgLen;

  /* Note that the information in maZtcFmtTwoArrayPtrsTable[] describes */
  /* the message header. The layout of the packet header must be inferred. */
  FLib_MemCpy(&fmtInfo,
              (void *) &(maZtcFmtTwoArrayPtrsTable[fmtParametersIndex]),
              sizeof(fmtInfo));

  array1Len = pMsg[fmtInfo.offsetToNumberOfArray1Elements]
            * fmtInfo.sizeofArray1Element;

  /* If there is only one array, sizeofArray2Element will be == 0, and so */
  /* array2Len will be == 0. */
  array2Len = pMsg[fmtInfo.offsetToNumberOfArray2Elements]
            * fmtInfo.sizeofArray2Element;

  hdrLen                    = fmtInfo.hdrLen;
  offsetToMsgHdrPtr1        = fmtInfo.offsetToPtr1;

  /* If there is only one array, there is no second pointer, and the header */
  /* only has two parts (first and second, with no third part). Use the total */
  /* size of the header to compute the length of the second part. */
  offsetToMsgHdrPtr2        = fmtInfo.offsetToPtr2;
  if (!offsetToMsgHdrPtr2) {
    offsetToMsgHdrPtr2 = hdrLen;
  }

  offsetToMsgHdrSecondPart  = offsetToMsgHdrPtr1 + sizeofPointer;
  hdrSecondPartLen          = offsetToMsgHdrPtr2 - offsetToMsgHdrSecondPart;

  pPktArray1                = pPkt + offsetToMsgHdrPtr1;
  pPktHdrSecondPart         = pPktArray1 + array1Len;
/*
  pMsgHdrPtrToArray1        = (uint8_t **) (pMsg + offsetToMsgHdrPtr1);

  pMsgArray1                = *pMsgHdrPtrToArray1;
 */

  FLib_MemCpy(&pMsgArray1, pMsg + offsetToMsgHdrPtr1, sizeofPointer);

  /* The message version has either one or two pointers. The packet version */
  /* has no pointer (the arrays take the place of the pointers). */
  totalDstLen               = hdrLen + array1Len - sizeofPointer;
  if (fmtInfo.sizeofArray2Element) {
    totalDstLen += array2Len - sizeofPointer;   /* Allow for the 2nd array. */
  }

  if (totalDstLen > pktLen) {
    return gTooBig_c;
  }

  /* Copy the beginning of the header. */
  FLib_MemCpy(pPkt, (void *) pMsg, offsetToMsgHdrPtr1);

  /* Copy the data for the first array. */
  FLib_MemCpy(pPktArray1, (void *) pMsgArray1, array1Len);

  /* Copy the second of the header, between the pointers. */
  FLib_MemCpy(pPktHdrSecondPart,
              (void *) (pMsg + offsetToMsgHdrSecondPart),
              hdrSecondPartLen);

  /* If the elements of the second array are zero length, there is no */
  /* second array. */
  if (fmtInfo.sizeofArray2Element) {
    offsetToMsgHdrThirdPart   = offsetToMsgHdrPtr2 + sizeofPointer;
    hdrThirdPartLen           = hdrLen - offsetToMsgHdrThirdPart;

    pPktArray2                = pPktHdrSecondPart + hdrSecondPartLen;
    pPktHdrThirdPart          = pPktArray2 + array2Len;
    /*
    pMsgHdrPtrToArray2        = (uint8_t **) (pMsg + offsetToMsgHdrPtr2);
    pMsgArray2                = *pMsgHdrPtrToArray2;
    */
    FLib_MemCpy(&pMsgArray2, pMsg + offsetToMsgHdrPtr2, sizeofPointer);

    /* Copy the data for the second array. */
    FLib_MemCpy(pPktArray2, (void *) pMsgArray2, array2Len);

    /* Copy the remainder of the header. */
    FLib_MemCpy(pPktHdrThirdPart,
                (void *) (pMsg + offsetToMsgHdrThirdPart),
                hdrThirdPartLen);
  }

  return totalDstLen;
}                                       /* ZtcPktFromMsgTwoArrayPtrs() */

/****************************************************************************/

/* The packet contains a fixed length header, which is copied unchanged to
 * the message.
 *
 * Source message
 *  | H |
 *
 * Destination packet:
 *  | H |
 */
index_t ZtcPktFromMsgSimple
  (
  uint8_t *pPkt,
  index_t pktLen,
  uint8_t *pMsg,
  index_t msgLen,
  index_t fmtParametersIndex
  )
{
  (void) fmtParametersIndex;

  if (pktLen < msgLen) {
    return gTooBig_c;
  }

  FLib_MemCpy(pPkt, (void *) pMsg, msgLen);
  return msgLen;
}                                       /* ZtcPktFromMsgSimple() */

/****************************************************************************/

/* Placeholder function to fill in the Fmt() table slot for message formats
 * that don't require packet-from-message copying.
 */
index_t ZtcPktFromMsgUnused
  (
  uint8_t *pPkt,
  index_t pktLen,
  uint8_t *pMsg,
  index_t msgLen,
  index_t fmtParametersIndex
  )
{
  /* Keep the compiler happy. */
  (void) pPkt;
  (void) pktLen;
  (void) pMsg;
  (void) msgLen;
  (void) fmtParametersIndex;

  return gTooBig_c;
}                                       /* ZtcPktFromMsgUnused() */


/****************************************************************************/
/* Custom function for copying MAC BeaconNotifyIndication from MLME message
 * to ZTC packet. It is needed because the BeaconNotifyInd frame does not
 * have a format that can be applied to other data copy functions. 
 *
 * Source message:
 *  | BSN | PendAddrSpec | sduLength | *pAddrList | *pPanDescriptor | pSdu |
 *
 * Destination packet:
 *  | BSN | PendAddrSpec | sduLength | AddrList | PanDescriptor | pSdu | 
 *
 */
#if gSAPMessagesEnableMlme_d
index_t ZtcPktFromMsgMacBeaconNotifyInd
  (
  uint8_t *pPkt,
  index_t pktLen,
  uint8_t *pMsg,
  index_t msgLen,
  index_t fmtParametersIndex
  )
{
  index_t currentLen;
  index_t totalDstLen;  
  index_t addrLen;

  /* Apply a beacon notify structure pointer to the message buffer. */
  nwkBeaconNotifyInd_t *pMsgBeaconNotifyInd = (nwkBeaconNotifyInd_t *)pMsg;
  uint8_t pendAddrSpec = pMsgBeaconNotifyInd->pendAddrSpec;
  uint8_t sduLength = pMsgBeaconNotifyInd->sduLength;
  
  /* Get the short and extended address counts from the PendAddrSpec field. */
  /* 802.15.4 IEEE standard 7.2.2.1.6 */
  index_t shortAddrCnt = pendAddrSpec & 0x07;
  index_t extAddrCnt = (pendAddrSpec & 0x70) >> 4;
    
  (void) msgLen;
  (void) fmtParametersIndex;  
  
  /* Copy BSN */
  pPkt[0] = pMsgBeaconNotifyInd->bsn;
  /* Copy PendAddrSpec */
  pPkt[1] = pendAddrSpec;  
  /* Copy SDU length */
  pPkt[2] = sduLength;  

  /* Get the total length in bytes of the address list  */
  addrLen = (shortAddrCnt << 1) + (extAddrCnt << 3);

  /* Get total length of the ZTC packet  */
  totalDstLen = 3 + addrLen + sizeof(panDescriptor_t) + sduLength;
  
  if (totalDstLen > pktLen) {
    return gTooBig_c;
  }  

  /* Copy address list */
  FLib_MemCpy(&pPkt[3], pMsgBeaconNotifyInd->pAddrList, addrLen);  
  
  currentLen = addrLen + 3;
  
  /* Copy PanDescriptor from the pointer in Beacon Notify Structure */
  FLib_MemCpy(&pPkt[currentLen], pMsgBeaconNotifyInd->pPanDescriptor, 
              sizeof(panDescriptor_t));
  
  currentLen += sizeof(panDescriptor_t);
  
  /* Copy SDU */
  FLib_MemCpy(&pPkt[currentLen], pMsgBeaconNotifyInd->pSdu, sduLength);
  
  return totalDstLen;  
}                                       /* ZtcPktFromMsgMacBeaconNotifyInd() */
#endif                                  /* #if gSAPMessagesEnableMlme_d     */


/****************************************************************************/
/* The function saves the values specified in the channel list for each MAC 
 * Scan Request given from ZTC in the static maScanChannels so that the 
 * subsequent MAC Scan Confirm message from ZTC can be built correctly for 
 * an Energy Detect Scan Type. For this type of scan confirm, the list of 
 * energies should only contain the values given for the channels specifed
 * in the scan request, so we save the list of channels and retrieve it in
 * ZtcPktFromMsgMacScanCnf when building the list for the scan confirm.
 * The function acts a proxy for ZtcMsgFromPktSimple as its only role is 
 * to save the channel list and does not do actual data copying from pkt to
 * msg.
 */
#if gSAPMessagesEnableMlme_d
index_t ZtcMsgFromPktMacScanReq
(
  uint8_t *pMsg,
  index_t msgLen,
  uint8_t *pPkt,
  index_t pktLen,
  index_t fmtParametersIndex
  )
{
  mlmeScanReq_t *pPktScanReq;
  index_t result;
  pPktScanReq = (mlmeScanReq_t *)pPkt;
  
  /* Saving channel list in MAC Scan Request */
  maScanChannels[ 0 ] = pPktScanReq->scanChannels[ 0 ];
  maScanChannels[ 1 ] = pPktScanReq->scanChannels[ 1 ];
  maScanChannels[ 2 ] = pPktScanReq->scanChannels[ 2 ];
  maScanChannels[ 3 ] = pPktScanReq->scanChannels[ 3 ];
  
  /* Redirect packet to simple data copy function */
  result = ZtcMsgFromPktSimple(pMsg, msgLen, pPkt, pktLen, fmtParametersIndex);
  return result;
}
#endif                                  /* #if gSAPMessagesEnableMlme_d */

#if gSAPMessagesEnableMlme_d
index_t ZtcPktFromMsgMacScanReq
(
  uint8_t *pMsg,
  index_t msgLen,
  uint8_t *pPkt,
  index_t pktLen,
  index_t fmtParametersIndex
  )
{
  mlmeScanReq_t *pPktScanReq;
  index_t result;
  pPktScanReq = (mlmeScanReq_t *)pPkt;
 
  /* Saving channel list in MAC Scan Request */
  maScanChannels[ 0 ] = pPktScanReq->scanChannels[ 0 ];
  maScanChannels[ 1 ] = pPktScanReq->scanChannels[ 1 ];
  maScanChannels[ 2 ] = pPktScanReq->scanChannels[ 2 ];
  maScanChannels[ 3 ] = pPktScanReq->scanChannels[ 3 ];
  
  /* Redirect packet to simple data copy function */
  result = ZtcPktFromMsgSimple(pMsg, msgLen, pPkt, pktLen, fmtParametersIndex);
  return result;
}
#endif                                  /* #if gSAPMessagesEnableMlme_d */

#if gSAPMessagesEnableInterPan_d
index_t ZtcPktFromMsgInterPanDataReq
(
  uint8_t *pMsg,
  index_t msgLen,
  uint8_t *pPkt,
  index_t pktLen,
  index_t fmtParametersIndex
  )
{
  index_t result;
  
  /* Redirect packet to simple data copy function, but insure that the pasduhandle is not copied */
  msgLen = msgLen - MbrSizeof(zbInterPanDataReq_t, pAsduHandle);
  result = ZtcPktFromMsgTwoArrayPtrs(pMsg, msgLen, pPkt, pktLen, fmtParametersIndex);
  return result;
}
#endif                                  /* #if gSAPMessagesEnableInterPan_d */

#if gSAPMessagesEnableInterPan_d
index_t ZtcMsgFromPktInterPanDataReq
(
  uint8_t *pMsg,
  index_t msgLen,
  uint8_t *pPkt,
  index_t pktLen,
  index_t fmtParametersIndex
  )
{
  zbInterPanDataReq_t *pPktInterPanDataReq;
  index_t result;  
  pPktInterPanDataReq = (zbInterPanDataReq_t*)pMsg;
  
  result = ZtcMsgFromPktTwoArrayPtrs(pMsg, msgLen, pPkt, pktLen, fmtParametersIndex);
  /*Make sure that the msdu handle pointer is set to NULL*/
  pPktInterPanDataReq->pAsduHandle = NULL;  
  
  return result;
}
#endif                                  /* #if gSAPMessagesEnableInterPan_d */



#if gSAPMessagesEnableHc_d
index_t ZtcMsgFromPktHc
  (
  uint8_t *pMsg,
  index_t msgLen,
  uint8_t *pPkt,
  index_t pktLen,
  index_t fmtParametersIndex
  )
{
  (void) fmtParametersIndex;

  if (pktLen + 1> msgLen) {
    return gTooBig_c;
  }

  pMsg[0] = pktLen;

  FLib_MemCpy((uint8_t *)pMsg + 1, (void *) pPkt, pktLen);
  
  return (pktLen + 1);
}                                       /* ZtcMsgFromPktHc() */
#endif                                  /* #if gSAPMessagesEnableHc_d */


#if gSAPMessagesEnableHc_d
index_t ZtcPktFromMsgHc
  (
  uint8_t *pPkt,
  index_t pktLen,
  uint8_t *pMsg,
  index_t msgLen,
  index_t fmtParametersIndex
  )
{
  (void) fmtParametersIndex;
  (void) pktLen;
  (void) msgLen;

  FLib_MemCpy(pPkt, (uint8_t *)pMsg + 1, pMsg[0]);
  
  return pMsg[0];
}                                       /* ZtcPktFromMsgHc() */
#endif                                  /* #if gSAPMessagesEnableHc_d */


/****************************************************************************/
/* Custom function for copying MAC Scan Confirmation from MLME message
 * to ZTC packet. It is needed because it uses the stored maScanChannels values
 * in order to identify which of the channels in the Energy Detect List
 * was returned in the confirmation message. 
 *
 */
#if gSAPMessagesEnableMlme_d || gSAPMessagesEnableNlme_d
index_t ZtcPktFromMsgMacScanCnf
  (
  uint8_t *pPkt,
  index_t pktLen,
  uint8_t *pMsg,
  index_t msgLen,
  index_t fmtParametersIndex
  )
{
  index_t totalDstLen;                  /* Risky on 8 bit CPUs. */
  index_t scanType;
  index_t elementLen;
  index_t arrayLen;
  index_t hdrLen;
  nwkScanCnf_t *pMsgScanCnf;
  uint8_t cChannelCounter = 0x00;  
  uint8_t iIndex = ByteOffset;  
  uint8_t bitMask = MaskAllExceptThirdByte;  
  
  (void) msgLen;
  (void) fmtParametersIndex;  

  /* Apply a Scan Confirmation structure pointer to the message buffer. */
  pMsgScanCnf = (nwkScanCnf_t *)pMsg;
  
  /* Find out fixed number of bytes to the variable-sized list */
  hdrLen = MbrOfs(nwkScanCnf_t, resList);
  scanType = pMsgScanCnf->scanType;

  /* Get the element lenght of the result list which is 1 for ED */
  /*  and orphan scan and 22 for Active and Passive Scan types */
  elementLen = IndirectMbrSizeof(nwkScanCnf_t, resList.pEnergyDetectList);
  if ( scanType == gScanModeActive_c || scanType == gScanModePassive_c )
    elementLen = sizeof(panDescriptor_t);
  
  /* arrayLen holds the size of the variable result list */
  
  #if gZtcExtendedScanConfirmSupport_c
    arrayLen = pMsgScanCnf->resultListSize * elementLen;
  #else
    if(( scanType == gScanModeED_c)||
      (( scanType == gScanModeActive_c || scanType == gScanModePassive_c )&&
       (pMsgScanCnf->resultListSize <= aScanResultsPerBlock)))
    {
       arrayLen = pMsgScanCnf->resultListSize * elementLen;
    }    
    else
    {
       arrayLen = elementLen*aScanResultsPerBlock;  

    }
  #endif

  totalDstLen = hdrLen + arrayLen;
  
  if (totalDstLen > pktLen) {
    return gTooBig_c;
  }  

  /* copy the bytes from before the variable result list */  
  FLib_MemCpy(pPkt, (void *) pMsg, hdrLen );
  
  /* if the MAC scan was an energy detect, we retrieve the channels */  
  /* scanned from maScanChannels and copy only those energy values */  
  /* from the global buffer to the ZTC packet */
  if (scanType == gScanModeED_c) {
    uint8_t *pEdList = pMsgScanCnf->resList.pEnergyDetectList;
    uint8_t *pCurrentArrayIdx = pPkt + totalDstLen - 1;
  
    for( cChannelCounter = gNumOfChannels_c; cChannelCounter>0; cChannelCounter-- )
    {   
      if ( maScanChannels[ iIndex ] & bitMask ) {
        *pCurrentArrayIdx = pEdList[ cChannelCounter -1 ];
        pCurrentArrayIdx--;
      }
      
      bitMask >>= ShiftRightByOne;
      /* Check if 1 Byte (set of 8 channels ) is over */
      if( 0x00 == bitMask ) {
        /* This is used to Right Shift and mask as we traverse each Channel,
            Reinitialise it to 0x80 for next channel  */
        bitMask = 0x80;
        iIndex--;
      }
    }
  } 
  /* any other kind of MAC scan */  
  else 
  {            
    if(pMsgScanCnf->resultListSize <= aScanResultsPerBlock) 
    {
       FLib_MemCpy(pPkt + hdrLen, 
                  (void *) (&pMsgScanCnf->resList.pPanDescriptorBlocks->descriptorList), 
                  arrayLen);                                    
    } 
    else
    {  
       FLib_MemCpy(pPkt + hdrLen, 
                  (void *) (&pMsgScanCnf->resList.pPanDescriptorBlocks->descriptorList), 
                  (elementLen*aScanResultsPerBlock)); 
       #if gZtcExtendedScanConfirmSupport_c                  
          FLib_MemCpy(pPkt + hdrLen + elementLen*aScanResultsPerBlock + 1, 
                     (void *) (&pMsgScanCnf->resList.pPanDescriptorBlocks->pNext->descriptorList), 
                     (elementLen *(pMsgScanCnf->resultListSize-aScanResultsPerBlock)));                 
       #endif                
    }
      
                   
  }
  return totalDstLen;  
}                                       /* ZtcPktFromMsgMacScanCnf() */
#endif                                  /* #if gSAPMessagesEnableMlme_d */

#endif                                  /* #if gZtcIncluded_d == 1 */
