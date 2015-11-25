/******************************************************************************
* Declarations used interally by Ztc.
*
* Copyright (c) 2008, Freescale, Inc.  All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from  Freescale Semiconductor.
*
* This file contains various declarations and definitions which are shared
* between different STC source files, and which should not be available to
* code outside of Ztc.
*
******************************************************************************/

#ifdef __IAR_SYSTEMS_ICC__ 
#include <stddef.h>
#endif /* __IAR_SYSTEMS_ICC__ */

#ifndef _ZtcPrivate_h
#define _ZtcPrivate_h

#include "FunctionalityDefines.h"
#include "ZCLoptions.h"

/******************************************************************************
*******************************************************************************
* Private macros
*******************************************************************************
******************************************************************************/

#ifndef gZtcDebug_d
#define gZtcDebug_d     FALSE
#endif

#ifndef gMsgInfo_d
#define gMsgInfo_d      FALSE
#endif

#if (gUpgradeImageOnCurrentDevice_d)||(gZclEnableOTAServer_d)
#define gZtcOtaSupportDebug_d    TRUE
#else
#define gZtcOtaSupportDebug_d    FALSE
#endif

/* Do the format coverters need the length-code-and-pointer format? */
#ifndef mZtcLenCodeAndPtrFormat_d
#if gZtcIncluded_d
#define mZtcLenCodeAndPtrFormat_d   (gZtcMacGetSetPIBCapability_d && gSAPMessagesEnableMlme_d)
#else
#define mZtcLenCodeAndPtrFormat_d   FALSE
#endif                                  /* #if gZtcIncluded_d */
#endif                                  /* #ifndef mZtcLenCodeAndPtrFormat_d */

/* Mark formats where the correct value is not yet certain. */
#define gZtcMsgFmtTODO_c    gZtcMsgFmtSimple_c

#define sizeofPointer ((index_t) sizeof(unsigned char *))

#if ( gBigEndian_c == TRUE)
 
 /* for converting between native and over-the-air format */
 #define OTA2Native16(iData) Swap2Bytes(iData)
 #define Native2OTA16(iData) Swap2Bytes(iData)
 
 /* for converting 32 bits between native and Over-The-Air format */
 #define OTA2Native32(iData) Swap4Bytes(iData)
 #define Native2OTA32(iData) Swap4Bytes(iData)
 
#else 
 
 /* no conversion is needed between native and over-the-air format */
 #define OTA2Native16(iData) (iData)
 #define Native2OTA16(iData) (iData)
 
 /* no conversion is needed for between native and Over-The-Air format */
 #define OTA2Native32(iData) (iData)
 #define Native2OTA32(iData) (iData)
 
#endif /* ( gBigEndian_c == TRUE) */


/* Number of elements in an array. */
#ifndef NumberOfElements
#define NumberOfElements(array)             ((sizeof(array) / (sizeof(array[0]))))
#endif

/* Offset of a member within a struct. */



/* Offset of a member within a struct. */
#ifdef __IAR_SYSTEMS_ICC__ /* IAR requires its own definition */
  #ifndef MbrOfs
  #define MbrOfs(type, member)                offsetof(type, member)
  #endif  
#else /* __IAR_SYSTEMS_ICC__ */
  #ifndef MbrOfs
  #define MbrOfs(type, member)                ((int) (&((type *) 0)->member))
  #endif
#endif /* __IAR_SYSTEMS_ICC__ */


/* Size of a member of a struct. */
#ifndef MbrSizeof
#define MbrSizeof(type, member)             (sizeof(((type *) 0)->member))
#endif

/* Access the element of a given structure */
#ifndef MbrOfStruct
#define MbrOfStruct(type, member)           (((type *) 0)->member)
#endif

/* struct "type" contains a field "member", which is a pointer. Return the */
/* size of the data type pointed to by member, i.e. sizeof(*type.member). */
#ifndef IndirectMbrSizeof
#define IndirectMbrSizeof(type, member)     (sizeof(*((type *) 0)->member))
#endif

/* Used as a placeholder for integer constants whose correct value is not yet */
/* known. */
#ifndef gTODO_c
#define gTODO_c     0
#endif

/* Used for integer constants whose correct value is known to be zero. */
#ifndef gZero_c
#define gZero_c     0
#endif

/* Used for integer constants whose correct value is known to be -1. */
#ifndef gMinusOne_c
#define gMinusOne_c (-1)
#endif

/* Used as a placeholder for an integer constant that is never actually */
/* used, but which must be present. */
#ifndef gUnused_c
#define gUnused_c   0
#endif

/* Used for pointer constants whose correct value is known to be null. */
#ifndef gNull_c
#define gNull_c     NULL
#endif

#ifndef gNumOfChannels_c
#define gNumOfChannels_c                  16
#endif

#ifndef gZbSuccess_c
#define gZbSuccess_c 0x00
#endif

#ifndef gZbInvalidRequest_c
/* The next higher layer has issued a request that is invalid 
or cannot be executed given the current state of the NWK layer */
#define gZbInvalidRequest_c       0xc2 
#endif

#ifndef gUseHwValidation_c
#define gUseHwValidation_c FALSE
#endif

/******************************************************************************
*******************************************************************************
* Private type definitions
*******************************************************************************
******************************************************************************/

/* Status values returned by Ztc configuration packets. */
/* These values must be coordinated with the external client. */
#define gZtcSuccess_c                   0
#define gZtcUnknownPIB_c                0xF2
#define gZtcAppMsgTooBig_c              0xF3
#define gZtcOutOfMessages_c             0xF4
#define gZtcEndPointTableIsFull_c       0xF5
#define gZtcEndPointNotFound_c          0xF6
#define gZtcUnknownOpcodeGroup_c        0xF7
#define gZtcOpcodeGroupIsDisabled_c     0xF8
#define gZtcDebugPrintFailed_c          0xF9
#define gZtcReadOnly_c                  0xFA
#define gZtcUnknownIBIdentifier_c       0xFB
#define gZtcRequestIsDisabled_c         0xFC
#define gZtcUnknownOpcode_c             0xFD
#define gZtcTooBig_c                    0xFE
#define gZtcError_c                     0xFF    /* General catchall error. */

/******************************************************************************
*******************************************************************************
* Private prototypes
*******************************************************************************
******************************************************************************/

/* Send an error packet to the external client. */
#if gZtcErrorReporting_d
extern void ZtcError(uint8_t status);
#else
#define ZtcError(status)
#endif

/* Concatenate data to, or send, a debug message to the external client. */
#if gZtcDebug_d
void ZtcPrint(bool_t readyToSend, void const *pSrc, index_t len);
#else
#define ZtcPrint(readyToSend, pSrc, len)
#endif

/******************************************************************************
*******************************************************************************
* Public macros
*******************************************************************************
******************************************************************************/

/* Address -1 is generally reserved by hardware, and not used by software. */
#define gZtcIsDisabledOpcodeGroup_c     ((ztcSAPHandlerInfo_t *) -1)

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/

extern uint16_t Swap2Bytes(uint16_t iOldValue);

/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
************************************************************************************/

extern uint8_t aExtendedAddress[8];

/******************************************************************************
*******************************************************************************
* Public type definitions
*******************************************************************************
******************************************************************************/

/* From Ztc's point of view, all SAP Handlers look alike. */
/* This prototype must match the definitions of the SAP Handler functions. */
typedef uint8_t (*pfnZtcSapHandler_t)(void *);

/****************************************************************************/

/* Define the names of the formats. Used to link rows in the message type */
/* info tables to the main format table (ztcFmtInfoTable[]). */

#define Fmt(name, packetFromMsgFunc, packetFromMsgIndex, \
                  msgFromPacketFunc, msgFromPacketIndex) \
  name,

enum {                                  /* For ztcMsgFmt_t. */
#include "ZtcCopyDataTbl.h"
};
typedef uint8_t ztcMsgFmt_t;

/****************************************************************************/

/* Define the message types that Ztc recognizes and/or generates. */
enum {
  mZtcMsgModeSelectReq_c            = 0x00, /* ZTC-ModeSelect.Request.              */
  mZtcMsgGetModeReq_c               = 0x02, /* ZTC-GetMode.Request.                 */
  mZtcMsgAFResetReq_c               = 0x05, /* ZTC-AFReset.Request.                 */
  mZtcMsgAPSResetReq_c              = 0x06, /* ZTC-APSReset.Request.                */
  mZtcMsgAPSReadyReq_c              = 0x07, /* ZTC-SetAPSReady.Request.             */
  mZtcMsgResetCPUReq_c              = 0x08, /* ZTC-CPU_Reset.Request.               */
  mZtcMsgGetEpMaxWindowSize_c       = 0x09, /* ZTC-GetEpMAxWindowSize               */
  mZtcMsgDeregisterEndPoint_c       = 0x0A, /* ZTC-DeregisterEndPoint.Request       */
  mZtcMsgRegisterEndPoint_c         = 0x0B, /* ZTC-RegisterEndPoint.Request         */
  mZtcMsgGetNumberOfEndPoints_c     = 0x0C, /* ZTC-GetNumberOfEndPoints.Request     */
  mZtcMsgGetEndPointDescription_c   = 0x0D, /* ZTC-GetEndPointDescription.Request   */
  mZtcMsgGetEndPointIdList_c        = 0x0E, /* ZTC-GetEndPointIdList.Request        */
  mZtcMsgSetEpMaxWindowSize_c       = 0x0F, /* ZTC-SetEpMAxWindowSize               */
  mZtcMsgGetICanHearYouList_c       = 0x10, /* ZTC-GetICanHearYouList.Request       */
  mZtcMsgSetICanHearYouList_c       = 0x11, /* ZTC-SetICanHearYouList.Request       */
  mZtcMsgGetChannelReq_c            = 0x12, /* ZTC-GetChannel.Request               */
  mZtcMsgSetChannelReq_c            = 0x13, /* ZTC-SetChannel.Request               */
  mZtcMsgGetPanIDReq_c              = 0x14, /* ZTC-GetPanID.Request                 */
  mZtcMsgSetPanIDReq_c              = 0x15, /* ZTC-SetPanID.Request                 */
  mZtcMsgGetPermissionsTable_c      = 0x16, /* ZTC-GetPermissionsTable.Request      */
  mZtcMsgSetPermissionsTable_c      = 0x17, /* ZTC-SetPermissionsTable.Request      */
  mZtcMsgRemoveFromPermissionsTable_c  = 0x18, /* ZTC-RemoveFromPermissionsTable.Request    */   
  mZtcMsgAddDeviceToPermissionsTable_c  = 0x19, /* ZTC-AddDeviceToPermissionsTable.Request    */ 
  mZtcMsgApsmeGetIBReq_c            = 0x20, /* ZTC-GetIB.Request, aka APSME-GET.Request     */
  mZtcMsgApsmeSetIBReq_c            = 0x21, /* ZTC-SetIB.Request, aka APSME-SET.Request     */
  mZtcMsgNlmeGetIBReq_c             = 0x22, /* ZTC-GetIB.Request, aka NLME-GET.Request      */
  mZtcMsgNlmeSetIBReq_c             = 0x23, /* ZTC-SetIB.Request, aka NLME-SET.Request      */
  mZtcMsgGetNumOfMsgsReq_c          = 0x24, /* ZTC-Get number of msgs available request     */  
  mZtcMsgFreeDiscoveryTablesReq_c   = 0x25, /* ZTC-FreeDiscoveryTables.Request      */
  mZtcMsgSetJoinFilterFlagReq_c     = 0x26, /* ZTC-SetJoinFilterFlag.Request        */
  mZtcMsgGetMaxApplicationPayloadReq_c = 0x27, /* ZTC-GetMaxApplicationPayload.Request */
#if (gZtcOtaSupportDebug_d)
  mZtcOtaSupportSetModeReq_c                      = 0x28, /* ZTC-OTASupportSetMode.Request */
  mZtcOtaSupportStartImageReq_c                   = 0x29, /* ZTC-OTASupportStartImage.Request */
  mZtcOtaSupportPushImageChunkReq_c               = 0x2A, /* ZTC-OTASupportPushImageChunk.Request */
  mZtcOtaSupportCommitImageReq_c                  = 0x2B, /* ZTC-OTASupportCommitImage.Request */
  mZtcOtaSupportCancelImageReq_c                  = 0x2C, /* ZTC-OTASupportCancelImage.Request */
  mZtcOtaSupportImageChunkReq_c                   = 0x2F, /* ZTC-OTASupportImageChunk.Request */
#endif
#if gUseHwValidation_c
  mZtcLowLevelMemoryWriteBlock_c    = 0x30, /* ZTCRamWriteBlock    */
  mZtcLowLevelMemoryReadBlock_c     = 0x31, /* ZTCRamReadBlock    */
  mZtcLowLevelMemoryWriteByte_c     = 0x32, /* ZTCRamWriteByte    */
  mZtcLowLevelMemoryReadByte_c      = 0x33, /* ZTCRamReadByte    */
  mZtcLowLevelMemoryWriteWord_c     = 0x34, /* ZTCRamWriteWord    */
  mZtcLowLevelMemoryReadWord_c      = 0x35, /* ZTCRamReadWord    */
  mZtcLowLevelMemoryWriteLong_c     = 0x36, /* ZTCRamWriteLong  */
  mZtcLowLevelMemoryReadLong_c      = 0x37, /* ZTCRamReadLong  */
  mZtcLowLevelPing_c                = 0x38, /* ZTCPing */
  mZtcLowLevelAbelWrite_c           = 0x39, /* ZTC-AbelWrite.Request                */ 
  mZtcLowLevelAbelRead_c            = 0x3A, /* ZTC-AbelRead.Request                 */
#endif */ gUseHwValidation_c */
  mZtcMsgGetApsDeviceKeyPairSet_c   = 0x3B,  /* ZTC-GetApsDeviceKeyPairSet */
  mZtcMsgGetApsDeviceKey_c          = 0x3C,
  mZtcMsgSetApsDeviceKey_c          = 0x3D,
  mZtcMsgSetApsDeviceKeyPairSet_c   = 0x3E,
  mZtcMsgClearApsDeviceKeyPairSet_c   = 0x3F,
  
  mZtcMsgAddToAddressMapPermanent_c      = 0xC0,
  mZtcMsgRemoveFromAddressMap_c          = 0xC1,
  
#if (gZtcOtaSupportDebug_d)
  mZtcOtaSupportQueryImageReq_c   = 0xC2, /* ZTC-OTASupportQueryImage.Request */
  mZtcOtaSupportQueryImageRsp_c   = 0xC3, /* ZTC-OTASupportQueryImageRsp.Request */
  mZtcOtaSupportImageNotifyReq_c  = 0xC4, /* ZTC-OTASupportImageNotify.Request */
#endif
   
  mZtcMsgGetSeedReq_c               = 0xD0,
  mZtcMsgReadExtendedAdrReq_c       = 0xD2, /* ZTC-ReadExtAddr.Request              */
  mZtcMsgWriteExtendedAdrReq_c      = 0xDB, /* ZTC-WriteExtAddr.Request             */
  mZtcMsgReadNwkMngAddressReq_c     = 0xDA, /* ZTC-ReadNwkMngAddr.Request           */
  mZtcMsgWriteNwkMngAddressReq_c    = 0xAD, /* ZTC-WriteNwkMngAddr.Request          */
  mZtcMsgStopNwkReq_c               = 0xDC, /* ZTC-StopNwk.Request                  */
  mZtcMsgStartNwkReq_c              = 0xDF, /* ZTC-StartNwk.Request                 */
  mZtcMsgStartNwkExReq_c            = 0xE7, /* ZTC-StartNwkEx.Request               */
  mZtcMsgStopNwkExReq_c             = 0xE8, /* ZTC-StopNwkEx.Request                */
  mZtcMsgRestartNwkReq_c            = 0xE0, /* ZTC-RestartNwk.Request               */
  mZtcMsgNVSaveReq_c                = 0xE4, /* ZTC-NVSave.Request.                  */
  mZtcMsgGetNVDataSetDescReq_c      = 0xE5, /* ZTC-NVGetDataSetDesc.Request.        */
  mZtcMsgGetNVPageHdrReq_c          = 0xE6, /* ZTC-NVGetPageHeaders.Request.        */
  mZtcMsgError_c                    = 0xFE, /* ZTC internal error.                  */
  mZtcMsgDebugPrint_c               = 0xFF, /* printf()-style debug message.        */
};

#endif                                  /* #ifndef _ZtcPrivate_h */
