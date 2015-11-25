/*****************************************************************************
* Declarations for the NV storage HAL for the HCS08.
*
* (c) Copyright 2006, Freescale, Inc. All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
*****************************************************************************/

#ifndef _NV_FLASHHAL_H_
#define _NV_FLASHHAL_H_

#include "EmbeddedTypes.h"
#include "NVM_Interface.h"
/*****************************************************************************
******************************************************************************
* Public macros
******************************************************************************
*****************************************************************************/

/* If a flash command fails, retry it a few times. If it still fails, an */
/* embedded device generally doesn't have any one to tell about the problem, */
/* so this number can be large. */

#define mNvFlashCmdRetries_c    32

/* Size of a flash memory page. */

#if !gArm7ExtendedNVM_d
#define gNvRawPageSize_c        508
#endif
#define gNvRawSectorSize_c      4096

/* Number of elements in an array. */
#ifndef NumberOfElements
#define NumberOfElements( array )   (( sizeof( array ) / ( sizeof( array[ 0 ]))))
#endif

/*****************************************************************************
******************************************************************************
* Public type declarations
******************************************************************************
*****************************************************************************/
#if !gArm7ExtendedNVM_d
typedef unsigned char NvRawPage_t[ gNvRawPageSize_c ];
#endif
typedef unsigned char NvRawSector_t[ gNvRawSectorSize_c];

typedef  uint32_t NvRawPageAddress_t;
typedef  uint32_t NvRawSectorAddress_t;


/* An integer type large enough to contain the size of a flash memory page. */
typedef uint16_t NvSize_t;



/*****************************************************************************
******************************************************************************
* Public memory declarations
*****************************************************************************
*****************************************************************************/

/* Make the number of pages available to other code.
 * This must be coordinated with  the number-
 * of-datasets definition in NV_Interface.h.
 * To change the number of data sets and/or NV storage pages:
 *      Set gNvNumberOfDataSets_c in NVM_Interface.h, and
 *      Make sure the number of entries in the NvDataSetTable[] array in
 *          NV_Data.c matches the value of gNvNumberOfDataSets_c. and
 *      Set NvNumberOfRawPages_c in NV_FlashHAL.h to >= gNvNumberOfDataSets_c + 1
 */


#if !gArm7ExtendedNVM_d
#define gNvNumberOfRawSectors_c   2
/* Make the addresses of the pages available to other code. */
extern NvRawSectorAddress_t const maNvRawSectorAddressTable[ gNvNumberOfRawSectors_c];
#endif

#define gNvmInterface_c gNvmInternalInterface_c

#define gNvInvalidSectorIndex_c   (( index_t ) -1 )

/*****************************************************************************
******************************************************************************
* Public prototypes
******************************************************************************
*****************************************************************************/

/* Erase one of the NV storage flash pages. */
void NvHalEraseSector
  (
  NvRawSectorAddress_t NvRawSectorAddress
  );

/* Init the flash controller hardware. */
void NvHalInit( void );

/* Copy data from an NV storage page. */
void NvHalRead
  (
  NvRawPageAddress_t nvRawPageAddress,
  NvSize_t srcPageOffset,
  unsigned char *pDst,
  NvSize_t dstLen
  );

/* Write data to an NV storage page. Return TRUE if successful. */
/* To write any data into flash, this function must be called to write */
/* enough bytes to fill an entire flash page, at successive page offsets. */
/* The result of doing anything else is undefined. */
bool_t NvHalWrite
  (
  NvRawPageAddress_t nvRawPageAddress,
  NvSize_t dstPageOffset,
  unsigned char *pSource,
  NvSize_t sourceLen
  );
  
  
  bool_t NvHalVerify
  (
  NvRawPageAddress_t nvRawPageAddress,
  NvSize_t dstPageOffset,
  unsigned char *pSource,
  NvSize_t sourceLen
  );

  bool_t NvHalIsBlank
  (
  NvRawPageAddress_t nvRawPageAddress,
  NvSize_t dstPageOffset,
  NvSize_t length
  ) ;

/*****************************************************************************
******************************************************************************
* Private macros
******************************************************************************
*****************************************************************************/

/* On the HCS08, the lowest numbered page in memory is reserved by the CPU, */
/* and can never be used for NV storage. */
#define gNvInvalidRawPageNumber_c       0



#endif /*_NV_FLASHHAL_H_*/
