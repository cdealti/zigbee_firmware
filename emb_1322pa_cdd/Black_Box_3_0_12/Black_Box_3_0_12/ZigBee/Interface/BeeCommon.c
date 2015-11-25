/******************************************************************************
* This public Source file is for the functions that are common in the stack subsystems. 
*
* (c) Copyright 2008, Freescale, Inc. All rights reserved.
*
* Freescale Semiconductor Confidential Proprietary
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale.
*
******************************************************************************/
#include "EmbeddedTypes.h"
#include "ZdoApsInterface.h"
#include "AppZdoInterface.h"
//#include "AppAspInterface.h"
#include "AfApsInterface.h"
#include "BeeStackUtil.h"
#include "ZdoNwkInterface.h"
#if !gArm7ExtendedNVM_d
#include "NV_Data.h"
#endif
#include "MsgSystem.h"  
#include "PublicConst.h"
#include "NVM_Interface.h"

#include "BeeStack_Globals.h"

#ifdef __IAR_SYSTEMS_ICC__
#if gRomBuild_d
#include "ROM_Patching.h"
#endif /* gRomBuild_d */
#endif /* __IAR_SYSTEMS_ICC__ */

/******************************************************************************
*******************************************************************************
* Private Macros
*******************************************************************************
******************************************************************************/

/*None*/

/******************************************************************************
*******************************************************************************
* Private Protypes
*******************************************************************************
******************************************************************************/

/*None*/

/******************************************************************************
*******************************************************************************
* Private type Definations
*******************************************************************************
******************************************************************************/

/*None*/

/******************************************************************************
*******************************************************************************
* Public memory Declarations
*******************************************************************************
******************************************************************************/

/******************************************************************************
*******************************************************************************
* Private Memory Declerations
*******************************************************************************
******************************************************************************/


uint16_t  gRandom;

/******************************************************************************
*******************************************************************************
* Public Functions
*******************************************************************************
******************************************************************************/
/*
  Return a native 32-bit value from an OTA 4-byte array.
*/
uint32_t GetNative32BitInt(FourByteArray_t aArray)
{
#if  ( gBigEndian_c == TRUE)
  return OTA2Native16(*(uint16_t *)aArray);  
#else  
  uint16_t localVal;
  localVal = aArray[3];
  localVal = (localVal << 0x08) | aArray[2];
  localVal = (localVal << 0x08) | aArray[1];
  localVal = (localVal << 0x08) | aArray[0];
  return  localVal;
#endif    
}

/*
  Set an OTA 4-byte array from a native 32-bit value.
*/
void SetNative32BitInt(FourByteArray_t aArray, uint32_t iInt32)
{
  iInt32 = Native2OTA32(iInt32);
  FLib_MemCpy(aArray, &iInt32, sizeof(iInt32));
}

/*
  Return a native 32-bit value from an OTA 3-byte array.
*/
uint32_t GetNative32BitIntFrom3ByteArray(ThreeByteArray_t aArray)
{
#if  ( gBigEndian_c == TRUE)
  FourByteArray_t localArray;
  FLib_MemCpy(localArray, aArray, sizeof(ThreeByteArray_t));
  localArray[3] = 0;
  return OTA2Native32(*(uint32_t *)localArray);  
#else  
  uint32_t localVal = 0;
  localVal = (localVal << 0x08) | aArray[2];
  localVal = (localVal << 0x08) | aArray[1];
  localVal = (localVal << 0x08) | aArray[0];
  return  localVal;
#endif    
}


#ifndef __IAR_SYSTEMS_ICC__ 
/* Is this a valid address to send to? */
bool_t IsValidNwkAddr(zbNwkAddr_t aNwkAddr)
{
  return (IsValidNwkUnicastAddr(aNwkAddr) || IsBroadcastAddress(aNwkAddr));
}
#endif /* __IAR_SYSTEMS_ICC__ */



#if !(gBigEndian_c)


void Inc4Bytes(uint8_t* aVal1, uint32_t Val2)
{
uint32_t temp;
temp = (uint32_t)TwoBytesToUint16(aVal1);
temp |= (((uint32_t)TwoBytesToUint16(((uint8_t*)aVal1)+2))<<16);
temp += Val2;
FLib_MemCpy(aVal1,(void*)&temp,4);
}
#endif /* #if (gBigEndian_c)*/



/* Are all the bytes equal in this two arrays? */
bool_t Cmp16Bytes(zbAESKey_t aAESKey1, zbAESKey_t aAESKey2)
{
  return FLib_MemCmp(aAESKey1, aAESKey2, sizeof(zbAESKey_t));
}

/* Is the given buffer filled with zeros? */
bool_t CmpToZero(void *pPtr, zbSize_t size)
{
  return BeeUtilArrayIsFilledWith(pPtr, 0x00, size);
}

/*
  FillWithZero

  Fill with 0s: DEPRECATED
*/
//void FillWithZero(void *pBuffer, uint8_t size)
//{
//  FLib_MemSet(pBuffer, 0, size);
//}

/*
  Swap2Bytes

  Used to convert between over-the-air (OTA) format (little endian) and native 
  format (big endian on HCS08).
*/
#if ( gBigEndian_c == FALSE)

#else
// HCS08 version
uint16_t Swap2Bytes(uint16_t iOldValue)
{
  uint16_t iValue;

  iValue = ((uint8_t *)&iOldValue)[1] << 8;
  iValue += ((uint8_t *)&iOldValue)[0];

  return iValue;
}
#endif

/*
  Swap2BytesArray

  Used to convert between over-the-air (OTA) format (little endian) and native 
  format (big endian on HCS08).
*/

#ifndef __IAR_SYSTEMS_ICC__ 
/*
  FLib_MemChr

  Look through array for this byte.
*/

uint8_t *FLib_MemChr(uint8_t *pArray, uint8_t iValue, uint8_t iLen)
{
  index_t i;
  for(i=0; i<iLen; ++i) {
    if(pArray[i] == iValue)
      return &(pArray[i]);
  }
  return NULL;
}
#endif /* __IAR_SYSTEMS_ICC__  */

/*
  BeeUtilBitToIndex

  Determine first bit set in a bit array. length is in bytes.

  Returns 0 if not found (so assumes bit 0 set).
*/
uint8_t BeeUtilBitToIndex(uint8_t *pBitArray, index_t iLen)
{
  index_t i;
  uint8_t iBitThisByte;

  for(i=0; i<iLen; ++i) {
    for(iBitThisByte=0; iBitThisByte < 8; ++iBitThisByte ) {
      if((pBitArray[i]) & (1 << iBitThisByte))
        return ((i << 3) + iBitThisByte);
    }
  }
  return 0;
}
/*
  SeedRandomNumber

  Seeds the random number generator.

  This function handles both platforms QE (HSC08) and Kaibab (ARM 7)
*/
#ifndef __IAR_SYSTEMS_ICC__
extern uint16_t CM_RandomBackoff;
#endif


