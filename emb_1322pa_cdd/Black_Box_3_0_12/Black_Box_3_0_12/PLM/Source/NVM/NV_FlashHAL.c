/*****************************************************************************
* Implementation of the non-volatile storage module flash controller HAL.
*
* (c) Copyright 2006, Freescale, Inc. All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
*****************************************************************************/

/* Unsigned char is sometimes used in this module instead of uint8_t. The
 * C standard guarantees that sizeof(char) == 1, and that this is the most
 * basic unit of memory. That guarantee is sometimes valuable when working
 * with memory hardware.
 */

/* Power supply variations can make both reading an writing flash unreliable,
 * but there is no means available to this code to monitor power.
 */

#include "EmbeddedTypes.h"
#include "NVM.h"
#include "NV_FlashHAL.h"
#include "Platform.h"

#if !gArm7ExtendedNVM_d

/*****************************************************************************
******************************************************************************
* Private macros
******************************************************************************
*****************************************************************************/

/*****************************************************************************
******************************************************************************
* Private prototypes
******************************************************************************
*****************************************************************************/


/*****************************************************************************
******************************************************************************
* Private type definitions
******************************************************************************
*****************************************************************************/

/*****************************************************************************
******************************************************************************
* Private memory declarations
******************************************************************************
*****************************************************************************/


/* Define the NV storage pages. */

/* To support one data set, there must be at least two pages available. */
#if gNvStorageIncluded_d
#define gNvSector0Address_c 0x1E000
#define gNvSector1Address_c 0x1D000
#endif



/*****************************************************************************
******************************************************************************
* Public memory definitions
******************************************************************************
*****************************************************************************/



#if gNvStorageIncluded_d
NvRawSectorAddress_t const maNvRawSectorAddressTable[ gNvNumberOfRawSectors_c] = {
                                                                                   gNvSector0Address_c,
                                                                                   gNvSector1Address_c
                                                                                 };
#endif                                  /* #if gNvStorageIncluded_d */
/*****************************************************************************
******************************************************************************
* Public functions
******************************************************************************
*****************************************************************************/

/* Erase a flash page, given an index in the NV storage page table
 * (maNvRawPageAddressTable[]).
 *
*/


void NvHalEraseSector
  (
  NvRawSectorAddress_t NvRawSectorAddress
  )
{
#if !gNvStorageIncluded_d
  ( void ) NvRawSectorAddress;
#else
  
  uint32_t sector_bitfield;
  index_t retries = mNvFlashCmdRetries_c;
  sector_bitfield = 1<<(NvRawSectorAddress>>12);  
  
      while ( retries-- ) {

      if(gNvmErrNoError_c == NVM_Erase(gNvmInterface_c, gNvmType_SST_c , sector_bitfield))
         break;
      
    }                                   
                                       
#endif                                  /* #if !gNvStorageIncluded_d #else */
}
/*****************************************************************************/



void NvHalInit ( void )
{
#if gNvStorageIncluded_d
if(gNvmInterface_c == gNvmExternalInterface_c)
 {
 	
 GPIO.FuncSel0 &= (~0xFF00); 
 GPIO.FuncSel0 |= 0x5500;
 }  
#endif                                  /* #if gNvStorageIncluded_d */
}
/****************************************************************************/

/* Read data from a specified offset within a given page of NV storage. */
void NvHalRead
  (
  NvRawPageAddress_t nvRawPageAddress,
  NvSize_t srcPageOffset,
  unsigned char *pDst,
  NvSize_t dstLen
  )
{
#if !gNvStorageIncluded_d
  ( void ) nvRawPageAddress;
  ( void ) srcPageOffset;
  ( void ) pDst;
  ( void ) dstLen;
#else

  NVM_Read(gNvmInterface_c , gNvmType_SST_c, (void *)pDst , nvRawPageAddress + srcPageOffset, (uint32_t)dstLen);

  
  
#endif                                  /* #if !gNvStorageIncluded_d #else */
}                                       /* NvHalRead() */

/****************************************************************************/

/* Write data to a specified offset within a given page of NV storage.
 * The caller must insure that a full page of data is written, across
 * more than one call to NvWrite() if necessary.
 * Since this routine only writes partial pages, it does not do retries.
 * Return TRUE if the write succeeded;
 *        FALSE if the flash controller reported an error.
 */



bool_t NvHalWrite
  (
  NvRawPageAddress_t nvRawPageAddress,
  NvSize_t dstPageOffset,
  unsigned char *pSource,
  NvSize_t sourceLen
  )
{
#if !gNvStorageIncluded_d
  ( void ) nvRawPageAddress;
  ( void ) dstPageOffset;
  ( void ) pSource;
  ( void ) sourceLen;
  return FALSE;
#else

 if(gNvmErrNoError_c ==  NVM_Write(gNvmInterface_c, gNvmType_SST_c ,(void *)pSource, nvRawPageAddress + dstPageOffset, (uint32_t)sourceLen))
   return TRUE;
 
 return FALSE;  

#endif                                  /* #if !gNvStorageIncluded_d #else */
}



bool_t NvHalVerify
  (
  NvRawPageAddress_t nvRawPageAddress,
  NvSize_t dstPageOffset,
  unsigned char *pSource,
  NvSize_t sourceLen
  )
{
#if !gNvStorageIncluded_d
  ( void ) nvRawPageAddress;
  ( void ) dstPageOffset;
  ( void ) pSource;
  ( void ) sourceLen;
  return FALSE;
#else
 
 if(gNvmErrNoError_c ==  NVM_Verify(gNvmInterface_c , gNvmType_SST_c ,(void *)pSource, nvRawPageAddress + dstPageOffset , (uint32_t)sourceLen))
   return TRUE;
 return FALSE;  

#endif                                  /* #if !gNvStorageIncluded_d #else */
}


bool_t NvHalIsBlank
  (
  NvRawPageAddress_t nvRawPageAddress,
  NvSize_t dstPageOffset,
  NvSize_t length
  )
{
#if !gNvStorageIncluded_d
  ( void ) nvRawPageAddress;
  ( void ) dstPageOffset;
  ( void ) length;
  return FALSE;
#else
 
 if(gNvmErrNoError_c == NVM_BlankCheck(gNvmInterface_c, gNvmType_SST_c, nvRawPageAddress + dstPageOffset, length))  
   return TRUE;
 return FALSE;  

#endif                                  /* #if !gNvStorageIncluded_d #else */
}
/****************************************************************************/



/****************************************************************************
* Giving a command to the flash controller may disturb read access to flash
* addresses. This function is copied into RAM so it can run even if flash is
* not accessible.
*
* Retries here are not a good idea, both because it would add mode code that
* would reside in scarce RAM, and because some commands cannot be retried
* without doing something else first. In particular, no byte of flash should
* be written more than once without an erase between the write commands.
*
* This function should only be called while interrupts are disabled.
*
* Returns TRUE  if the command worked;
*         FALSE if the flash controller reported an error.
*****************************************************************************/


/*****************************************************************************
*****************************************************************************/
#else /* #if !gArm7ExtendedNVM_d */
/*****************************************************************************
*****************************************************************************/


/*****************************************************************************
******************************************************************************
* Private macros
******************************************************************************
*****************************************************************************/

/*****************************************************************************
******************************************************************************
* Private prototypes
******************************************************************************
*****************************************************************************/


/*****************************************************************************
******************************************************************************
* Private type definitions
******************************************************************************
*****************************************************************************/

/*****************************************************************************
******************************************************************************
* Private memory declarations
******************************************************************************
*****************************************************************************/


/* Define the NV storage pages. */

/* To support one data set, there must be at least two pages available. */


/*****************************************************************************
******************************************************************************
* Public memory definitions
******************************************************************************
*****************************************************************************/



/*****************************************************************************
******************************************************************************
* Public functions
******************************************************************************
*****************************************************************************/

/* Erase a flash page, given an index in the NV storage page table
 * (maNvRawPageAddressTable[]).
 *
*/


void NvHalEraseSector
  (
  NvRawSectorAddress_t NvRawSectorAddress
  )
{
#if !gNvStorageIncluded_d
  ( void ) NvRawSectorAddress;
#else
  
  uint32_t sector_bitfield;
  index_t retries = mNvFlashCmdRetries_c;
  sector_bitfield = 1<<(NvRawSectorAddress>>12);
  
      while ( retries-- ) {

      if(gNvmErrNoError_c == NVM_Erase(gNvmInterface_c, gNvmType_SST_c , sector_bitfield))
         break;
      
    }                                   
                                       
#endif                                  /* #if !gNvStorageIncluded_d #else */
}
/*****************************************************************************/



void NvHalInit ( void )
{
#if gNvStorageIncluded_d
if(gNvmInterface_c == gNvmExternalInterface_c)
 {
 	
 GPIO.FuncSel0 &= (~0xFF00); 
 GPIO.FuncSel0 |= 0x5500;
 SPI.Setup = 0x1;
 }  
else if(gNvmInterface_c == gNvmInternalInterface_c)
 {
 FLASH.Setup = 0x1;  
 }
#endif                                  /* #if gNvStorageIncluded_d */
}
/****************************************************************************/

/* Read data from a specified offset within a given page of NV storage. */
void NvHalRead
  (
  NvRawPageAddress_t nvRawPageAddress,
  NvSize_t srcPageOffset,
  unsigned char *pDst,
  NvSize_t dstLen
  )
{
#if !gNvStorageIncluded_d
  ( void ) nvRawPageAddress;
  ( void ) srcPageOffset;
  ( void ) pDst;
  ( void ) dstLen;
#else

  NVM_Read(gNvmInterface_c , gNvmType_SST_c, (void *)pDst , nvRawPageAddress + srcPageOffset, (uint32_t)dstLen);

  
  
#endif                                  /* #if !gNvStorageIncluded_d #else */
}                                       /* NvHalRead() */

/****************************************************************************/

/* Write data to a specified offset within a given page of NV storage.
 * The caller must insure that a full page of data is written, across
 * more than one call to NvWrite() if necessary.
 * Since this routine only writes partial pages, it does not do retries.
 * Return TRUE if the write succeeded;
 *        FALSE if the flash controller reported an error.
 */



bool_t NvHalWrite
  (
  NvRawPageAddress_t nvRawPageAddress,
  NvSize_t dstPageOffset,
  unsigned char *pSource,
  NvSize_t sourceLen
  )
{
#if !gNvStorageIncluded_d
  ( void ) nvRawPageAddress;
  ( void ) dstPageOffset;
  ( void ) pSource;
  ( void ) sourceLen;
  return FALSE;
#else

  if(gNvmErrNoError_c ==  NVM_Write(gNvmInterface_c, gNvmType_SST_c ,(void *)pSource, nvRawPageAddress + dstPageOffset, (uint32_t)sourceLen))
  {
    if(gNvmErrNoError_c ==  NVM_Verify(gNvmInterface_c , gNvmType_SST_c ,(void *)pSource, nvRawPageAddress + dstPageOffset , (uint32_t)sourceLen))
    {
      return TRUE;
    }
  }
 
 return FALSE;  

#endif                                  /* #if !gNvStorageIncluded_d #else */
}



bool_t NvHalVerify
  (
  NvRawPageAddress_t nvRawPageAddress,
  NvSize_t dstPageOffset,
  unsigned char *pSource,
  NvSize_t sourceLen
  )
{
#if !gNvStorageIncluded_d
  ( void ) nvRawPageAddress;
  ( void ) dstPageOffset;
  ( void ) pSource;
  ( void ) sourceLen;
  return FALSE;
#else
 
 if(gNvmErrNoError_c ==  NVM_Verify(gNvmInterface_c , gNvmType_SST_c ,(void *)pSource, nvRawPageAddress + dstPageOffset , (uint32_t)sourceLen))
   return TRUE;
 return FALSE;  

#endif                                  /* #if !gNvStorageIncluded_d #else */
}


bool_t NvHalIsBlank
  (
  NvRawPageAddress_t nvRawPageAddress,
  NvSize_t dstPageOffset,
  NvSize_t length
  )
{
#if !gNvStorageIncluded_d
  ( void ) nvRawPageAddress;
  ( void ) dstPageOffset;
  ( void ) length;
  return FALSE;
#else
 
 if(gNvmErrNoError_c == NVM_BlankCheck(gNvmInterface_c, gNvmType_SST_c, nvRawPageAddress + dstPageOffset, length))  
   return TRUE;
 return FALSE;  

#endif                                  /* #if !gNvStorageIncluded_d #else */
}
/****************************************************************************/

#endif /* #if !gArm7ExtendedNVM_d */
