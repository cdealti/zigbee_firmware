/*****************************************************************************
* Declarations for the NV storage module.
*
* (c) Copyright 2006, Freescale, Inc. All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
*****************************************************************************/

#ifndef _NV_FLASH_H_
#define _NV_FLASH_H_

#include "EmbeddedTypes.h"
#include "NVM_Interface.h"
#include "NV_FlashHAL.h"

/*****************************************************************************
******************************************************************************
* Public macros
******************************************************************************
*****************************************************************************/

#if !gArm7ExtendedNVM_d

#define gNvNumberOfRawPages_c 8
/*****************************************************************************
******************************************************************************
* Public type declarations
******************************************************************************
*****************************************************************************/


/* It is possible for there to be more than one copy of a given data set */
/* stored in NV. A sequence number is used to recognize the most recent */
/* version. */

typedef uint8_t NvSectorSequenceNumber_t;


/* A small struct is written as both the first and last data in each page, */
/* to identify the contents of the page, and to ensure that the entire */
/* contents of the page have been written correctly. */

/* A magic number is used to recognize NV pages that have been written to */
/* by this code vs. pages that have been never been written or that have */
/* been written as part of initial flash programming during manufacture. */
/* This should be something improbable; not 0x00, 0xFF, or a simple */
/* repeating bit pattern. */
/* Note that this magic number can also serve to identify versions of */
/* the NV storage code. If the NvStructuredPage_t typedef changes, change */
/* this number. */

typedef uint8_t NvMagicNumber_t;

#define mNvMagicNumber_c    ((NvMagicNumber_t) 0x27)


#pragma pack(push,current)
#pragma pack(1)

typedef struct NvStructuredSectorHeader_tag 
{
  NvMagicNumber_t magicNumber;
  NvSectorSequenceNumber_t sequenceNumber;
} NvStructuredSectorHeader_t;

typedef struct NvStructuredPageHeader_tag 
{
  NvMagicNumber_t magicNumber;
  NvDataSetID_t dataSetID;
} NvStructuredPageHeader_t;

/* One page in flash. */
/* NVM_Interface.h defines the sizeof() the clientData field as a magic */
/* integer (gNvMaxDataSetSize_c). If it changes here, be sure to change it */
/* there, too. */
typedef struct NvStructuredPage_tag {
  NvStructuredPageHeader_t header;
  unsigned char clientData[   sizeof(NvRawPage_t)
                            - sizeof(NvStructuredPageHeader_t)
                            - sizeof(NvStructuredPageHeader_t)];
  NvStructuredPageHeader_t trailer;
} NvStructuredPage_t;

typedef struct NvStructuredSector_tag {
  NvStructuredSectorHeader_t header;
  unsigned char clientData[   sizeof(NvRawSector_t)
                            - sizeof(NvStructuredSectorHeader_t)
                            - sizeof(NvStructuredSectorHeader_t)];
  NvStructuredSectorHeader_t trailer;
} NvStructuredSector_t;
#pragma pack(pop,current)
typedef NvRawPageAddress_t NvSectPageInfo_t[ gNvNumberOfDataSets_c ];
/*****************************************************************************
******************************************************************************
* Public prototypes
******************************************************************************
*****************************************************************************/

/* Most of the NV storage module's public functions are declared in */
/* NVM_Interface.h. */

/* This function is special; it is called by the early startup code (crt0 */
/* and/or PlatformInit) to try to find a data structure in NV storage that */
/* contains hardware initialization parameters. At that early point, very */
/* little of the regular system initialization has been done. In particular, */
/* neither NvModuleInit() nor NvHalInit() has been called, and RAM based */
/* data has not been initialized. */
/* This function should ONLY be called by the system startup code. */
extern void const *pNvPrimitiveSearch
  (
  unsigned char *pDelimiterString,
  index_t delimiterStringLen,
  NvSize_t totalStructLen
  );

/*****************************************************************************
*****************************************************************************/
#else /* #if !gArm7ExtendedNVM_d */
/*****************************************************************************
*****************************************************************************/

/*****************************************************************************
******************************************************************************
* Public type declarations
******************************************************************************
*****************************************************************************/


/* It is possible for there to be more than one copy of a given data set */
/* stored in NV. A sequence number is used to recognize the most recent */
/* version. */

typedef uint8_t NvSectorSequenceNumber_t;


/* A small struct is written as both the first and last data in each page, */
/* to identify the contents of the page, and to ensure that the entire */
/* contents of the page have been written correctly. */

/* A magic number is used to recognize NV pages that have been written to */
/* by this code vs. pages that have been never been written or that have */
/* been written as part of initial flash programming during manufacture. */
/* This should be something improbable; not 0x00, 0xFF, or a simple */
/* repeating bit pattern. */
/* Note that this magic number can also serve to identify versions of */
/* the NV storage code. If the NvStructuredPage_t typedef changes, change */
/* this number. */

typedef uint8_t NvMagicNumber_t;

#define mNvMagicNumber_c    ((NvMagicNumber_t) 0x27)


#pragma pack(push,current)
#pragma pack(1)

typedef struct NvStructuredSectorHeader_tag 
{
  NvMagicNumber_t magicNumber;
  NvSectorSequenceNumber_t sequenceNumber;
} NvStructuredSectorHeader_t;

typedef struct NvStructuredPageHeader_tag 
{
  NvMagicNumber_t magicNumber;
  NvDataSetID_t dataSetID;
} NvStructuredPageHeader_t;


typedef struct NvStructuredSector_tag {
  NvStructuredSectorHeader_t header;
  unsigned char clientData[   sizeof(NvRawSector_t)
                            - sizeof(NvStructuredSectorHeader_t)
                            - sizeof(NvStructuredSectorHeader_t)];
  NvStructuredSectorHeader_t trailer;
} NvStructuredSector_t;
#pragma pack(pop,current)
typedef NvRawPageAddress_t NvSectPageInfo_t[ gNvNumberOfDataSets_c ];
/*****************************************************************************
******************************************************************************
* Public prototypes
******************************************************************************
*****************************************************************************/

#endif /* #if !gArm7ExtendedNVM_d */

#endif /*_NV_FLASH_H_*/

