/******************************************************************************
* BeeCommon.h
*
* This module contains all of the types and functions used across modules in
* BeeStack. These are not "public" interface functions. For those, see
* BeeStackInterface.h
*
* Copyright (c) 2008, Freescale, Inc. All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
******************************************************************************/
#ifndef __IAR_SYSTEMS_ICC__
#ifdef MC13226Included_d
    #undef MC13226Included_d
#endif    
    #define MC13226Included_d 0
#endif

#ifdef __IAR_SYSTEMS_ICC__ 
#include <stddef.h>
#endif /* __IAR_SYSTEMS_ICC__ */

#ifndef _BeeCommon_h
#define _BeeCommon_h

#include "EmbeddedTypes.h"
#include "zigbee.h"
#include "MsgSystem.h"
#include "BeeStackConfiguration.h"

/******************************************************************************
*******************************************************************************
* Public macros
*******************************************************************************
******************************************************************************/
#if ( gBigEndian_c == TRUE)

	/* for converting between native and over-the-air format */
	#define OTA2Native16(iData) Swap2Bytes(iData)
	#define Native2OTA16(iData) Swap2Bytes(iData)

	/* for converting 32 bits between native and Over-The-Air format */
	#define OTA2Native32(iData) Swap4Bytes(iData)
	#define Native2OTA32(iData) Swap4Bytes(iData)

        #define OTA2Native64(pData) Swap8Bytes(pData)
        #define Native2OTA64(pData) Swap8Bytes(pData)

#else /* ( gBigEndian_c == TRUE) */

	/* no conversion is needed between native and over-the-air format */
	#define OTA2Native16(iData) (iData)
	#define Native2OTA16(iData) (iData)

	/* no conversion is needed for between native and Over-The-Air format */
	#define OTA2Native32(iData) (iData)
	#define Native2OTA32(iData) (iData)

        #define OTA2Native64(pData) (pData)
        #define Native2OTA64(pData) (pData)

#endif /* ( gBigEndian_c == TRUE) */

#if ( gBigEndian_c != TRUE)

	/* for converting between native and over-the-air format */
	#define HcOTA2Native16(iData) Swap2Bytes(iData)
	#define Native2HcOTA16(iData) Swap2Bytes(iData)

	/* for converting 32 bits between native and Over-The-Air format */
	#define HcOTA2Native32(iData) Swap4Bytes(iData)
	#define Native2HcOTA32(iData) Swap4Bytes(iData)

        #define HcOTA2Native64(pData) Swap8Bytes(pData)
        #define Native2HcOTA64(pData) Swap8Bytes(pData)

#else /* ( gBigEndian_c != TRUE) */

	/* no conversion is needed between native and over-the-air format */
	#define HcOTA2Native16(iData) (iData)
	#define Native2HcOTA16(iData) (iData)

	/* no conversion is needed for between native and Over-The-Air format */
	#define HcOTA2Native32(iData) (iData)
	#define Native2HcOTA32(iData) (iData)

        #define HcOTA2Native64(pData) (pData)
        #define Native2HcOTA64(pData) (pData)

#endif /* ( gBigEndian_c != TRUE) */



// convert from OTA (little endian) array format to native int format and back
typedef uint8_t   TwoByteArray_t[2];
typedef uint8_t   ThreeByteArray_t[3];
typedef uint8_t   FourByteArray_t[4];
uint16_t GetNative16BitInt(TwoByteArray_t aArray);
uint32_t GetNative32BitInt(FourByteArray_t aArray);
uint32_t GetNative32BitIntFrom3ByteArray(ThreeByteArray_t aArray);
void SetNative16BitInt(TwoByteArray_t aArray, uint16_t iInt16);
void SetNative32BitInt(FourByteArray_t aArray, uint32_t iInt32);

/* Number of elements in an array. */
#define NumberOfElements(array)             ((sizeof(array) / (sizeof(array[0]))))

/* Offset of a member within a struct. */
#ifdef __IAR_SYSTEMS_ICC__ /* IAR requires its own definition */
  #define MbrOfs(type, member)                offsetof(type, member)
#else /* __IAR_SYSTEMS_ICC__ */
  #define MbrOfs(type, member)                ((int) ( &( (type *) 0 )->member ) )
#endif /* __IAR_SYSTEMS_ICC__ */

/* Size of a member of a struct. */
#define MbrSizeof(type, member)             (sizeof(((type *) 0)->member))

/* Access the element of a given structure */
#define MbrOfStruct(type, member)           (((type *) 0)->member)

/* struct "type" contains a field "member", which is a pointer. Return the */
/* size of the data type pointed to by member, i.e. sizeof(*type.member). */
#define IndirectMbrSizeof(type, member)     (sizeof(*((type *) 0)->member))

/* Used as a placeholder for integer constants whose correct value is not yet */
/* known. */
#define gTODO_c     0

/* Used for integer constants whose correct value is known to be zero. */
#define gZero_c     0

/* Used for integer constants whose correct value is known to be -1. */
#define gMinusOne_c (-1)

/* Used as a placeholder for an integer constant that is never actually */
/* used, but which must be present. */
#define gUnused_c   0

/* Used for pointer constants whose correct value is known to be null. */
#define gNull_c     NULL

/******************************************************************************
*******************************************************************************
* Public type definitions
*******************************************************************************
******************************************************************************/

/*****************************************************************************
******************************************************************************
* Public prototypes
******************************************************************************
*****************************************************************************/

/*
  Frees the buffer when it has a message Id  just before the address of the give pointer.
*/
void FreeRootBuffer(void *pMsg);

/* Is this id in group table? */
extern bool_t ApsIsGroupInGroupTable(zbGroupId_t aGroupId);
/* is this ieee address ourselves? */
bool_t IsSelfIeeeAddress(zbIeeeAddr_t aIeeeAddr);

/* is this nwk address ourselves? */
bool_t IsSelfNwkAddress(zbNwkAddr_t aNwkAddr);

#ifndef __IAR_SYSTEMS_ICC__   
/* Is this one of the broadcast addresses? */
bool_t IsBroadcastAddress(zbNwkAddr_t aNwkAddr);

/* is this a valid nwk addr for unicasting to? */
bool_t IsValidNwkUnicastAddr(zbNwkAddr_t aNwkAddr);

/* is this a incomplete binding nwk addr 0xFFFE ? */
bool_t IsIncompleteBindNwkAddr(zbNwkAddr_t aNwkAddr);

/* is this a valid nwk addr to send to? */
bool_t IsValidNwkAddr(zbNwkAddr_t aNwkAddr);

/* is this a valid PAN ID? */
//bool_t IsValidPanId(zbPanId_t aPanId);

/* is this a valid extended PAN ID? */
bool_t IsValidExtendedPanId(zbIeeeAddr_t aIeeeAddr);
#else
#if MC13226Included_d
/* Is this one of the broadcast addresses? */
extern bool_t IsBroadcastAddress(zbNwkAddr_t aNwkAddr);
/* is this a valid nwk addr for unicasting to? */
extern bool_t IsValidNwkUnicastAddr(zbNwkAddr_t aNwkAddr);
/* is this a incomplete binding nwk addr 0xFFFE ? */
extern bool_t IsIncompleteBindNwkAddr(zbNwkAddr_t aNwkAddr);
/* is this a valid nwk addr to send to? */
extern bool_t IsValidNwkAddr(zbNwkAddr_t aNwkAddr);
/* is this a valid PAN ID? */
//bool_t IsValidPanId(zbPanId_t aPanId);
/* is this a valid extended PAN ID? */
extern bool_t IsValidExtendedPanId(zbIeeeAddr_t aIeeeAddr);
#else
/* Is this one of the broadcast addresses? */
extern bool_t ROM_IsBroadcastAddress(zbNwkAddr_t aNwkAddr);
#define IsBroadcastAddress ROM_IsBroadcastAddress
/* is this a valid nwk addr for unicasting to? */
extern bool_t ROM_IsValidNwkUnicastAddr(zbNwkAddr_t aNwkAddr);
#define IsValidNwkUnicastAddr ROM_IsValidNwkUnicastAddr
/* is this a incomplete binding nwk addr 0xFFFE ? */
extern bool_t ROM_IsIncompleteBindNwkAddr(zbNwkAddr_t aNwkAddr);
#define IsIncompleteBindNwkAddr ROM_IsIncompleteBindNwkAddr
/* is this a valid nwk addr to send to? */
extern bool_t ROM_IsValidNwkAddr(zbNwkAddr_t aNwkAddr);
#define IsValidNwkAddr ROM_IsValidNwkAddr
/* is this a valid PAN ID? */
//bool_t IsValidPanId(zbPanId_t aPanId);
/* is this a valid extended PAN ID? */
extern bool_t ROM_IsValidExtendedPanId(zbIeeeAddr_t aIeeeAddr);
#define IsValidExtendedPanId ROM_IsValidExtendedPanId
#endif
#endif


bool_t IsValidPanId(zbPanId_t aPanId);


bool_t IsValidLogicalChannel(uint8_t iChannel);

#define IsValidExtendedAddress(myAddr) IsValidExtendedPanId(myAddr)

#ifndef __IAR_SYSTEMS_ICC__ 
/* set a large array of memory (larger than FLib_memset can handle */
void BeeUtilLargeMemSet(void *pBuffer, uint8_t value, uint16_t iCount);

/* same as BeeUtilLargeMemSet(), but with 1 less parameter */
void BeeUtilLargeZeroMemory(void *pBuffer, uint16_t iCount);
#else
#if MC13226Included_d
/* set a large array of memory (larger than FLib_memset can handle */
extern void BeeUtilLargeMemSet(void *pBuffer, uint8_t value, uint16_t iCount);
/* same as BeeUtilLargeMemSet(), but with 1 less parameter */
extern void BeeUtilLargeZeroMemory(void *pBuffer, uint16_t iCount);
#else
/* set a large array of memory (larger than FLib_memset can handle */
extern void ROM_BeeUtilLargeMemSet(void *pBuffer, uint8_t value, uint16_t iCount);
#define BeeUtilLargeMemSet ROM_BeeUtilLargeMemSet
/* same as BeeUtilLargeMemSet(), but with 1 less parameter */
extern void ROM_BeeUtilLargeZeroMemory(void *pBuffer, uint16_t iCount);
#define BeeUtilLargeZeroMemory ROM_BeeUtilLargeZeroMemory
#endif
#endif

/*copies a 16 bit variable to a location with the bytes swapped
  Note needed for big endian
*/
#if !(gBigEndian_c)
void Set2Bytes(void *ptr, uint16_t val);

bool_t IsEqual2BytesInt(void *ptr, uint16_t val);
#endif
/*
  Clears a buffer to zero
*/
#ifndef __IAR_SYSTEMS_ICC__ 
void BeeUtilZeroMemory
(
	void  *pPtr,          /* IN: The poointer to the buffer to be cleaned. */
	zbSize_t  bufferSize  /* IN: The amount of byte to clean. */
);
#else
#if MC13226Included_d
extern void BeeUtilZeroMemory
(
	void  *pPtr,          /* IN: The poointer to the buffer to be cleaned. */
	zbSize_t  bufferSize  /* IN: The amount of byte to clean. */
);
#else
extern void ROM_BeeUtilZeroMemory
(
	void  *pPtr,          /* IN: The poointer to the buffer to be cleaned. */
	zbSize_t  bufferSize  /* IN: The amount of byte to clean. */
);
#define BeeUtilZeroMemory ROM_BeeUtilZeroMemory
#endif
#endif


void BeeUtilSetToF
(
  void  *pPtr,          /* IN: The poointer to the buffer to be cleaned. */
  zbSize_t  bufferSize  /* IN: The amount of byte to clean. */
);

/* copies 2 ieee (long) addresses. for code size savings */
void Copy8Bytes(zbIeeeAddr_t aIeeeAddr1,zbIeeeAddr_t aIeeeAddr2);

/* copies 16 bytes always. */
void Copy16Bytes(void *pDst, void *pSrc);

/* are the two addresses equal? */
bool_t IsEqual8Bytes(zbIeeeAddr_t aIeeeAddr1, zbIeeeAddr_t aIeeeAddr2);

/* fill ieee (long) address with 0s */
void Fill8BytesToZero(zbIeeeAddr_t aIeeeAddr1);

/* fill any length buffer with 0s */
void FillWithZero(void *pBuffer, uint8_t size);

/* is this ieee address all 0s? */
bool_t Cmp8BytesToZero(zbIeeeAddr_t aIeeeAddr1);

/* Are all the bytes equal zero in this array? */
bool_t Cmp16BytesToZero(void *pArray);

/* is this ieee address all 0xFFs? */
bool_t Cmp8BytesToFs(zbIeeeAddr_t aIeeeAddr1);

/* Are all the bytes equal in this two arrays? */
bool_t Cmp8Bytes(zbIeeeAddr_t  aIeeeAddr1, zbIeeeAddr_t aIeeeAddr2);

/* Are all the bytes equal in this two arrays? */
bool_t Cmp16Bytes(zbAESKey_t aAESKey1, zbAESKey_t aAESKey2);

/* Is the given buffer filled with zeros? */
bool_t CmpToZero(void *pPtr, zbSize_t size);


#ifndef __IAR_SYSTEMS_ICC__ 
/* swaps bytes to convert between OTA and native format for a 16-bit word */
uint16_t Swap2Bytes(uint16_t iOldValue);

/* swaps bytes to convert between OTA and native format for a 2-byte array */
void Swap2BytesArray(uint8_t *pArray);

uint32_t Swap4Bytes(uint32_t iOldValue);

void Swap8Bytes(uint8_t  *pInput);
#else
#if MC13226Included_d
/* swaps bytes to convert between OTA and native format for a 16-bit word */
extern uint16_t Swap2Bytes(uint16_t iOldValue);
/* swaps bytes to convert between OTA and native format for a 2-byte array */
extern void Swap2BytesArray(uint8_t *pArray);

extern uint32_t Swap4Bytes(uint32_t iOldValue);

extern void Swap8Bytes(uint8_t  *pInput);
#else
/* swaps bytes to convert between OTA and native format for a 16-bit word */
extern uint16_t Swap2Bytes(uint16_t iOldValue);
/* swaps bytes to convert between OTA and native format for a 2-byte array */
extern void ROM_Swap2BytesArray(uint8_t *pArray);
#define Swap2BytesArray ROM_Swap2BytesArray

extern uint32_t ROM_Swap4Bytes(uint32_t iOldValue);
#define Swap4Bytes ROM_Swap4Bytes

extern void ROM_Swap8Bytes(uint8_t  *pInput);
#define Swap8Bytes ROM_Swap8Bytes
#endif
#endif
/* Used in the securityto convert over-the-air (OTA) frame counters and native format (big endian on HCS08). */

/* look for a byte in an array of bytes */
#ifndef __IAR_SYSTEMS_ICC__ 
uint8_t *FLib_MemChr(uint8_t *pArray, uint8_t iValue, uint8_t iLen);
#else
#if MC13226Included_d
uint8_t *FLib_MemChr(uint8_t *pArray, uint8_t iValue, uint8_t iLen);
#else
uint8_t *ROM_FLib_MemChr(uint8_t *pArray, uint8_t iValue, uint8_t iLen);
#define FLib_MemChr ROM_FLib_MemChr
#endif
#endif
/* set an indexed bit (used by APS group functions) */
#ifndef __IAR_SYSTEMS_ICC__ 
void BeeUtilSetIndexedBit(uint8_t *pBitArray, index_t iBit);

/* get the bit */
bool_t BeeUtilGetIndexedBit(uint8_t *pBitArray, index_t iBit);

/* clear the bit */
uint8_t BeeUtilClearIndexedBit(uint8_t *pBitArray, index_t iBit);

/* check to see if an array is filled with a particular value */
bool_t BeeUtilArrayIsFilledWith(uint8_t *pArray, uint8_t value, index_t iLen);

/* determine first bit in a bit indexed array */ 
uint8_t BeeUtilBitToIndex(uint8_t *pBitArray, index_t iLen);
#else
#if MC13226Included_d
extern void BeeUtilSetIndexedBit(uint8_t *pBitArray, index_t iBit);
/* get the bit */
extern bool_t BeeUtilGetIndexedBit(uint8_t *pBitArray, index_t iBit);
/* clear the bit */
extern uint8_t BeeUtilClearIndexedBit(uint8_t *pBitArray, index_t iBit);

/* check to see if an array is filled with a particular value */
extern bool_t BeeUtilArrayIsFilledWith(uint8_t *pArray, uint8_t value, index_t iLen);

/* determine first bit in a bit indexed array */ 
extern uint8_t BeeUtilBitToIndex(uint8_t *pBitArray, index_t iLen);
#else
extern void ROM_BeeUtilSetIndexedBit(uint8_t *pBitArray, index_t iBit);
#define BeeUtilSetIndexedBit ROM_BeeUtilSetIndexedBit
/* get the bit */
extern bool_t ROM_BeeUtilGetIndexedBit(uint8_t *pBitArray, index_t iBit);
#define BeeUtilGetIndexedBit ROM_BeeUtilGetIndexedBit
/* clear the bit */
extern uint8_t ROM_BeeUtilClearIndexedBit(uint8_t *pBitArray, index_t iBit);
#define BeeUtilClearIndexedBit ROM_BeeUtilClearIndexedBit
/* check to see if an array is filled with a particular value */
extern bool_t ROM_BeeUtilArrayIsFilledWith(uint8_t *pArray, uint8_t value, index_t iLen);
#define BeeUtilArrayIsFilledWith ROM_BeeUtilArrayIsFilledWith
/* determine first bit in a bit indexed array */ 
extern uint8_t ROM_BeeUtilBitToIndex(uint8_t *pBitArray, index_t iLen);
#define BeeUtilBitToIndex ROM_BeeUtilBitToIndex
#endif
#endif
#define UintOf(p2Bytes) (*(uint16_t *) (p2Bytes))

#ifdef __IAR_SYSTEMS_ICC__
  #ifdef gHostApp_d
  uint16_t GetRandomNumberArmHost(void);
  uint8_t GetRandomRangeArmHost(uint8_t low, uint8_t high);
  #define GetRandomNumber GetRandomNumberArmHost
  #define GetRandomRange GetRandomRangeArmHost
  #else
  uint32_t GetRandomNumber(void);
  uint8_t GetRandomRange(uint8_t low, uint8_t high);
  #endif
#else
uint16_t GetRandomNumber(void);
uint8_t GetRandomRange(uint8_t low, uint8_t high);
#endif

void SeedRandomNumber(uint16_t seed);

/* Check is a specific address is within the ICanHearYouTable, return true if it is within the table, it also
returns true if global variable gICanHearYouCounter is equeal to zero which means it can hear all addresses
returns false if gICanHearYouCounter is different than 0 and the given address was not found within the table*/
bool_t CanIHearThisShortAddress(zbNwkAddr_t aSourceAddress);
extern uint8_t giIcanHearLinkCost;

#if gICanHearYouTableCapability_d
/*
  Sets up an i-can-hear-you table, which allows for table-top routing.

  If a NwkAddr is NOT in the i-can-hear-you-table, then the incoming MCPS-DATA.indication is dropped.

  The pLqiList list must have the same count as the pAddressList. If the entry is 0, then LQI is
  not overridden for that entry. If non-zero, then that hard-coded value is used for LQI for incoming
  packets from that node.

  Set addressCounter to 0 to disable the table completely.
*/
void SetICanHearYouTable(index_t addressCounter, zbNwkAddr_t *pAddressList, uint8_t *pLqiList);

/* Receives a pointer to the destination buffer where the table is going to be
copy to, and the size of destination buffer. Returns number of table entries
copied to destination buffer. */
index_t GetICanHearYouTable(zbNwkAddr_t *pDstTable, index_t maxElementsInDstTable);
#endif
/* Free all the meesages from a given anchor */
#ifndef __IAR_SYSTEMS_ICC__ 
void DeQueueAllMessages ( anchor_t *pAnchor );
#else
#if MC13226Included_d
extern void DeQueueAllMessages ( anchor_t *pAnchor );
#else
extern void ROM_DeQueueAllMessages ( anchor_t *pAnchor );
#define DeQueueAllMessages ROM_DeQueueAllMessages
#endif
#endif

/* uint8_t zbRandom(uint8_t  seed); */

#if (gHighSecurity_d)
uint8_t SSP_EntityAuthenticationGetStateMachineEntryIndex
(
  uint8_t  *pPartnerAddress
);
#endif
#endif                                  /* #ifndef _BeeCommon_h */
