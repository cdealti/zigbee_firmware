/*****************************************************************************
* Non-volatile storage module local implementation.
*
* Copyright (c) 2006, Freescale, Inc. All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
*****************************************************************************/

#include "EmbeddedTypes.h"
#include "TS_Interface.h"
#include "NV_FlashHAL.h"
#include "NV_Flash.h"
#include "Crm.h"

#if !gArm7ExtendedNVM_d

/*****************************************************************************
******************************************************************************
* Private macros
******************************************************************************
*****************************************************************************/

/* Size of a member of a struct. */
#ifndef MbrSizeof
#define MbrSizeof(type, member)     (sizeof(((type *) 0)->member))
#endif

/* There must be at least one more flash page than there are data sets, to */
/* allow for one copy of each data set plus a page for a new copy of one */
/* data set. */

#if gNvNumberOfRawPages_c <= gNvNumberOfDataSets_c
#error
#endif

/* this macro must reflect the bit used for the idle task defined in BeeApp.c */
#define gIdleTaskNVIntervalEvent_c  ( 1 << 0 )
#define gMaxPageOffset_c  ( sizeof(NvStructuredSectorHeader_t) + gNvNumberOfRawPages_c * sizeof(NvRawPage_t))
#define gLastPageOffset_c ( sizeof(NvStructuredSectorHeader_t) + (gNvNumberOfRawPages_c - 1) * sizeof(NvRawPage_t))
#define gUninitAddress     0xffffffff
#define NvOpenSector(nvRawSectorAddress, sequenceNumber)     NvOpen_Validate_Sector( nvRawSectorAddress , sequenceNumber, TRUE )
#define NvValidateSector(nvRawSectorAddress, sequenceNumber) NvOpen_Validate_Sector( nvRawSectorAddress , sequenceNumber, FALSE )
#define g_CopyBufferSize_c  64

/*****************************************************************************
******************************************************************************
* Private prototypes
******************************************************************************
*****************************************************************************/
#if gNvStorageIncluded_d
void PowerUpNVM(void);
void PowerDownNVM(void);
static index_t NvDataSetIndexFromID(NvDataSetID_t dataSetID);
static bool_t NvIsValidPage( NvRawPageAddress_t nvRawPageAddress , NvDataSetID_t *pDataSetID );
static bool_t NvIsValidSector (  NvRawSectorAddress_t nvRawSectorAddress  );
static bool_t NvSaveDataSetLowLevel(  index_t dataSetIndex , NvRawPageAddress_t pageAddress  );
static void NvSaveDataSet( index_t dataSetIndex );
static void NvGetFirstBlankPageInfo(NvRawPageAddress_t nvRawSectorAddress, NvRawPageAddress_t* pBlankPage, index_t* pBlankPageIndex);
static NvSectorSequenceNumber_t NvGetSectorSequenceNumber( NvRawSectorAddress_t nvRawSectorAddress );
static bool_t NvCopyDataSet  (  NvRawPageAddress_t sourcePage , NvRawPageAddress_t destPage  );
static bool_t NvOpen_Validate_Sector(NvRawSectorAddress_t nvRawSectorAddress, NvSectorSequenceNumber_t sequenceNumber, bool_t open );
static NvSectorSequenceNumber_t NvLastSectorSequence ( NvSectorSequenceNumber_t sectorSeqA,NvSectorSequenceNumber_t sectorSeqB );
static void NvGetSectorPageInfo ( NvRawSectorAddress_t nvRawSectorAddress, NvSectPageInfo_t* pSectPageInfo );
static bool_t  NvCopyTheRemainsDataSets ( NvSectPageInfo_t* oldSectPageInfo, NvSectPageInfo_t* newSectPageInfo, NvRawPageAddress_t firstBlankPageAddress,index_t firstBlankPageIndex );
#endif

#if gNvSelfTest_d
static void NvSelfTest(void);
#endif

/*****************************************************************************
******************************************************************************
* Private type definitions
******************************************************************************
*****************************************************************************/

/* One entry per data set. */
typedef struct NvDataSetInfo_tag {
  bool_t saveNextInterval;
  NvSaveInterval_t ticksToNextSave;
  NvSaveCounter_t countsToNextSave;
} NvDataSetInfo_t;


/*****************************************************************************
******************************************************************************
* Private memory declarations
******************************************************************************
*****************************************************************************/

#if gNvStorageIncluded_d
/* Table of dirty flags, one per dataset in NvDataSetDescriptionTable[]. */
static NvDataSetInfo_t maNvDirtyFlags[gNvNumberOfDataSets_c];

/* Minimum number of calls to NvTimerTick() between saves of a given dataset. */
NvSaveInterval_t gNvMinimumTicksBetweenSaves = gNvMinimumTicksBetweenSaves_c;
/* Minimum number of calls to NvSaveOnIdle() between saves of a given dataset. */
static NvSaveCounter_t gNvCountsBetweenSaves = gNvCountsBetweenSaves_c;

/* If this counter is != 0, do not save to NV Storage. */
static uint8_t mNvCriticalSectionFlag = 0;
#endif                                  /* #if gNvStorageIncluded_d */

/* Scratch data sets used by the internal unit test. */


/*****************************************************************************
******************************************************************************
* Public functions
******************************************************************************
*****************************************************************************/

/* There may be operations that take place over extended times that must
 * be atomic from the point of view of NV Storage saves. These routines
 * increment/decrement a counter; when the counter is non-zero, no NV
 * saves will be done. Note that this does not affect NV restores.
 */

void NvClearCriticalSection(void) {
#if gNvStorageIncluded_d
  if(mNvCriticalSectionFlag)  /* dg - in case of set/clear mismatch */
    --mNvCriticalSectionFlag;
#endif /* gNvStorageIncluded_d */
}

void NvSetCriticalSection(void) {
#if gNvStorageIncluded_d
  ++mNvCriticalSectionFlag;
#endif /* gNvStorageIncluded_d */
}


/****************************************************************************/

/* Called from the idle task to process save-on-next-idle and save-on-count. */
void NvIdle(void)
{
#if gNvStorageIncluded_d
  index_t i;

  if (mNvCriticalSectionFlag) {
    return;
  }
   
  for (i = 0; i < gNvNumberOfDataSets_c; ++i) {
    if (!maNvDirtyFlags[i].countsToNextSave) {
      NvSaveDataSet(i);
    }
  }
#endif
}                                       /* NvIdle() */

/****************************************************************************/

/* Return TRUE if the given data set is dirty. */
bool_t NvIsDataSetDirty
  (
  NvDataSetID_t dataSetID
  )
{
#if !gNvStorageIncluded_d
  (void) dataSetID;
  return FALSE;
#else
  index_t dataSetIndex = NvDataSetIndexFromID(dataSetID);
  return (    maNvDirtyFlags[dataSetIndex].saveNextInterval
          || (maNvDirtyFlags[dataSetIndex].countsToNextSave != gNvCountsBetweenSaves_c));
#endif                                  /* #if !gNvStorageIncluded_d #else */
}                                       /* NvIsDataSetDirty() */

/****************************************************************************/

/* Call this once, before calling any other NV function.
 *
 * Any page that does not contain a valid, recognized data set is erased, in
 * preparation for later reuse.
 *
 * It is unlikely, but possible, that there could be more than one copy of one
 * or more data sets in NV storage. Normally the page containing the older
 * version is erased after the new version is written, but a reset at just
 * the wrong time could prevent the erasure.
 *
 * Having more than one copy of any data set is 1) unnecessary, 2) uses space
 * that may be needed for future writes, and 3) would complicate other NV code.
 * This function scans NV storage for alternate versions of the same data set,
 * and if it finds one, erases the older copy (or copies).
 */
void NvModuleInit(void)
{
#if gNvStorageIncluded_d
#if gNvDebug_d
  NvDataItemDescription_t const *pDataItemDescriptions;
  index_t dataSetIndex;
  NvSize_t dataSetSize;
#endif
  index_t i;
  index_t sectorIndex;
  NvRawSectorAddress_t      oldSectAddress , newSectAddress;
  NvSectorSequenceNumber_t  oldSectSeq , newSectSeq;
  NvSectPageInfo_t oldSectPageInfo ,  newSectPageInfo;
  NvRawPageAddress_t firstBlankPageAddress;
  index_t firstBlankPageIndex;
  
#if gNvDebug_d
  /* The CodeWarrior HCS08 compiler complains about constant == constant. */
  uint16_t NvMaxDataSetSize = gNvMaxDataSetSize_c;
#endif
  /* Power up the NVM */
  PowerUpNVM();
  /* initialize flash layer */
  NvHalInit();
  
  /* No data set starts dirty. */
  for (i = 0; i < gNvNumberOfDataSets_c; ++i)
  {
    maNvDirtyFlags[i].saveNextInterval = FALSE;
    maNvDirtyFlags[i].countsToNextSave = gNvCountsBetweenSaves;
  }
  
  /* Scan for and erase duplicate copies of data sets. This code is very */
  /* slow (N^2), but it only runs once, and N is small. Data set IDs are */
  /* arbitrary values, so its necessary to search through the list. */
  
  /* Visit every raw page. If it isn't a valid data set, and it isn't clean, */
  /* erase it. */
  
  newSectAddress = gUninitAddress;
  for (sectorIndex = 0; sectorIndex < gNvNumberOfRawSectors_c; ++sectorIndex)
  {
    
    if (!NvIsValidSector(maNvRawSectorAddressTable[sectorIndex])) 
    {
      if( !NvHalIsBlank(maNvRawSectorAddressTable[sectorIndex] , 0 , sizeof(NvRawSector_t)) )
      {
        NvHalEraseSector(maNvRawSectorAddressTable[sectorIndex]);
      }
    } 
    else
    {
      if(newSectAddress != gUninitAddress )
      {
        newSectSeq = NvGetSectorSequenceNumber(newSectAddress);
        oldSectSeq =   NvGetSectorSequenceNumber(maNvRawSectorAddressTable[sectorIndex]);
        if(oldSectSeq ==  NvLastSectorSequence ( newSectSeq ,oldSectSeq )  )
        {
          oldSectAddress = newSectAddress ;
          newSectAddress = maNvRawSectorAddressTable[sectorIndex];
        }
        else
        {
          oldSectAddress = maNvRawSectorAddressTable[sectorIndex];
        }
        NvGetSectorPageInfo ( newSectAddress, &newSectPageInfo );
        NvGetSectorPageInfo ( oldSectAddress, &oldSectPageInfo );
        NvGetFirstBlankPageInfo ( newSectAddress , &firstBlankPageAddress, &firstBlankPageIndex );
        if( NvCopyTheRemainsDataSets(  &oldSectPageInfo , &newSectPageInfo, firstBlankPageAddress,firstBlankPageIndex))
        {
          // copy of te rest of data sets failed
          NvHalEraseSector(newSectAddress);
          newSectAddress = oldSectAddress;
        }
        else
        {
          NvHalEraseSector(oldSectAddress);
        }
      }
      else // if(newSectAddress != gUninitAddress )
      {
        newSectAddress = maNvRawSectorAddressTable[sectorIndex];
      }
    }                                   /* if (!NvIsValidDataSet(... else */
  }
  
  
  
  
  /* for (pageIndex = 0; ... */
  
#if gNvDebug_d
  /* NVM_Interface.h defines the size of the client area of a page as an */
  /* unjustified immediate integer constant. Make sure that its correct. */
  /* Assert if it isn't. */
  if (NvMaxDataSetSize != MbrSizeof(NvStructuredPage_t, clientData)) {
    for (;;)
    { }
  }
  
  /* Verify that all of the data sets defined by the client code will */
  /* fit into the NV storage page size. Doing this once here simplifies */
  /* other code. */
  for (dataSetIndex = 0; dataSetIndex < gNvNumberOfDataSets_c; ++dataSetIndex) {
    pDataItemDescriptions = NvDataSetTable[dataSetIndex].pItemDescriptions;
    if (!pDataItemDescriptions) {
      continue;
    }
    dataSetSize = 0;
    
    while (pDataItemDescriptions->length) {
      dataSetSize += pDataItemDescriptions->length;
      ++pDataItemDescriptions;
    }
    
    /* Assert if the data set is too big. */
    if (dataSetSize > MbrSizeof(NvStructuredPage_t, clientData)) {
      for (;;)
      { }
    }
  }
#endif                                  /* #if gNvDebug_d */
  
  /* Power down the NVM */
  PowerDownNVM();
  
#if gNvSelfTest_d
  NvSelfTest();
#endif
#endif                                  /* #if gNvStorageIncluded_d */
}                                       /* NvModuleInit() */

/****************************************************************************/

/* Search for a specific data structure in NV storage, delimited by a known
 * string at the beginning and end of the struct, and a known structure length.
 * The length includes both strings. The "strings" contain arbitrary bytes;
 * they are not assumed to be null-terminated C strings.
 *
 * This function is only indended for use by by the early startup code (crt0
 * and PlatformInit), which needs to find a struct containing hardware
 * initialization values.
 *
 * At that point in the startup process, the stack is available, but no data
 * in RAM has been initialized yet. ONLY const data (in ROM) is dependable.
 * NvModuleInit() has not been called yet to initialize this module. Be very
 * careful about calling local functions.
 * NvHalInit() has not been called yet. Do not call HAL functions, directly
 * or indirectly.
 *
 * Return a pointer to the NV storage copy of the data if found.
 * Return NULL if not found.
 */
//void const *pNvPrimitiveSearch
uint32_t NvPrimitiveSearch
  (
  unsigned char *pDelimiterString,
  index_t delimiterStringLen,
  NvSize_t totalStructLen
  )
{
#if !gNvStorageIncluded_d
  (void) pDelimiterString;
  (void) delimiterStringLen;
  (void) totalStructLen;
  return 0;
#else
  NvSize_t lastStartingOffset;
  index_t pageIndex;
  NvSize_t pageOffset;
  index_t sectorIndex;
  NvSectPageInfo_t sectPageInfo;
  NvRawPageAddress_t pageAddress;
  /* If the first string isn't found by lastStartingOffset, the struct */
  /* is not present in the page. */
  lastStartingOffset = sizeof(NvRawPage_t)
                     - sizeof(NvStructuredPageHeader_t)     /* Trailer. */
                     - totalStructLen;
  /* Power up the NVM */
  PowerUpNVM();
  /* Scan the NV storage pages. Invalid pages are ignored. */
    for( sectorIndex=0; sectorIndex < gNvNumberOfRawSectors_c;sectorIndex++ )
       {
        if(NvIsValidSector(maNvRawSectorAddressTable[sectorIndex]))
         {
          break;
         }
       }
    if(sectorIndex == gNvNumberOfRawSectors_c)
      {
       PowerDownNVM();
       return 0;
      }
    NvGetSectorPageInfo ( maNvRawSectorAddressTable[sectorIndex], &sectPageInfo );
    
    for(pageIndex = 0; pageIndex < gNvNumberOfDataSets_c; pageIndex++)
      {
         if(sectPageInfo[pageIndex] != gUninitAddress)
         {
            pageAddress = sectPageInfo[pageIndex];
            for (pageOffset = sizeof(NvStructuredPageHeader_t); pageOffset <= lastStartingOffset;  ++pageOffset)
               {               	

               if (NvHalVerify(pageAddress,pageOffset,pDelimiterString,delimiterStringLen )
                && NvHalVerify(pageAddress,pageOffset+totalStructLen - delimiterStringLen,pDelimiterString,delimiterStringLen ))
                   {
                       /* Power down the NVM */
                     PowerDownNVM();
                     return pageAddress + pageOffset;
          
                   }                               /* if (NvHalVerify(pageIndex,pag ... */
               }                                   /* for (pageOffset = sizeof ... */
           
         }
      }
    
  
  /* Power down the NVM */
  PowerDownNVM();
  return 0;
#endif                                  /* #if !gNvStorageIncluded_d #else */
}                                       /* pNvPrimitiveSearch() */

                                       /* NvRestoreDataSet() */

/****************************************************************************/

/* Copy the most recent version of a data set from NV storage to RAM. */
/* Note that the copy will succeed if a valid copy of the data set is found */
/* in NV storage, regardless of the state of the data set's dirty flag. */
/* Return TRUE if the copy is successful. */

bool_t NvRestoreDataSet
  (
  NvDataSetID_t dataSetID
 )
{
#if !gNvStorageIncluded_d
  (void) dataSetID;
  return FALSE;
#else
  NvDataItemDescription_t const *pDataItemDescriptions;
  index_t dataSetIndex;
  index_t sectorIndex;
  NvRawSectorAddress_t sectorAddress;
  NvSectPageInfo_t sectPageInfo;
  NvSize_t pageOffset;
  /* Power up the NVM */
  PowerUpNVM();
  /* Find the data set description. */
  dataSetIndex = NvDataSetIndexFromID(dataSetID);
  
  for( sectorIndex=0; sectorIndex < gNvNumberOfRawSectors_c;sectorIndex++ )
       {
       sectorAddress = maNvRawSectorAddressTable[sectorIndex];  
       if(NvIsValidSector(sectorAddress))
         {
          break;
         }
       }
  if(sectorIndex == gNvNumberOfRawSectors_c)
  {
    // there are no valid sectors
    PowerDownNVM(); 
    return FALSE;
  }

  NvGetSectorPageInfo(sectorAddress , &sectPageInfo);
  if(sectPageInfo[dataSetIndex] == gUninitAddress)
  {
    // the page does not exist in the valid sector
    PowerDownNVM(); 
    return FALSE;
  }
  pDataItemDescriptions = NvDataSetTable[dataSetIndex].pItemDescriptions;

  /* Start reading just after the page header. */
  pageOffset = sizeof(NvStructuredPageHeader_t);

  /* Copy data from the NV storage page to the destination data set. */
  while (pDataItemDescriptions->length)
  {
    NvHalRead(sectPageInfo[dataSetIndex], pageOffset, (uint8_t*)pDataItemDescriptions->pointer, pDataItemDescriptions->length);
    pageOffset += pDataItemDescriptions->length;
    ++pDataItemDescriptions;
  }

  maNvDirtyFlags[dataSetIndex].saveNextInterval = FALSE;
  maNvDirtyFlags[dataSetIndex].countsToNextSave = gNvCountsBetweenSaves_c;
  /* Power down the NVM */
  PowerDownNVM();
  return TRUE;
#endif                                  /* #if !gNvStorageIncluded_d #else */
}
/****************************************************************************/

/* Save the data set on the next call to NvIdle(). */
/* Use the save-on-count mechanism. */
void NvSaveOnIdle
  (
  NvDataSetID_t dataSetID
 )
{
#if !gNvStorageIncluded_d
  (void) dataSetID;
#else
  maNvDirtyFlags[NvDataSetIndexFromID(dataSetID)].countsToNextSave = 0;
#endif
}

/****************************************************************************/

void NvSaveOnInterval
  (
  NvDataSetID_t dataSetID
 )
{
#if !gNvStorageIncluded_d
  (void) dataSetID;
#else
  index_t dataSetIndex = NvDataSetIndexFromID(dataSetID);

  if (!maNvDirtyFlags[dataSetIndex].saveNextInterval) {
    maNvDirtyFlags[dataSetIndex].ticksToNextSave  = gNvMinimumTicksBetweenSaves_c;
    maNvDirtyFlags[dataSetIndex].saveNextInterval = TRUE;
    TS_SendEvent(gIdleTaskID, gIdleTaskNVIntervalEvent_c);  
  }
#endif
}                                       /* NvSaveOnInterval() */

/****************************************************************************/

/* Decrement the counter. Once it reaches 0, the next call to NvIdle() will */
/* save the data set. */
void NvSaveOnCount
  (
  NvDataSetID_t dataSetID
  )
{
#if !gNvStorageIncluded_d
  (void) dataSetID;
#else
  index_t dataSetIndex = NvDataSetIndexFromID(dataSetID);

  if (maNvDirtyFlags[dataSetIndex].countsToNextSave) {
    --maNvDirtyFlags[dataSetIndex].countsToNextSave;
  }
#endif
}                                       /* NvSaveOnCount() */

/****************************************************************************/

/* Set the timer used by NvSaveOnInterval(). Takes effect after the next */
/* save. */
void NvSetMinimumTicksBetweenSaves
  (
  NvSaveInterval_t newInterval
 )
{
#if !gNvStorageIncluded_d
  (void) newInterval;
#else
  gNvMinimumTicksBetweenSaves = newInterval;
#endif
}                                       /* NvSetMinimumTicksBetweenSaves() */

/****************************************************************************/

/* Set the counter trigger value used by NvSaveOnCount(). Takes effect */
/* after the next save. */
void NvSetCountsBetweenSaves
  (
  NvSaveCounter_t newCounter
 )
{
#if !gNvStorageIncluded_d
  (void) newCounter;
#else
  gNvCountsBetweenSaves = newCounter;
#endif
}                                       /* NvSetCountsBetweenSaves() */

/****************************************************************************/

/* Called from the idle task to process save-on-interval requests. */
/* Returns FALSE if the timer tick counters for all data sets have reached */
/* zero. In this case, the timer can be turned off. */
/* Returns TRUE if any of the data sets' timer tick counters have not yet */
/* counted down to zero. In this case, the timer should be active. */
bool_t NvTimerTick(bool_t countTick)
{
#if !gNvStorageIncluded_d
  (void) countTick;
  return FALSE;
#else
  index_t i;
  bool_t fTicksLeft = FALSE;
  NvDataSetInfo_t *pDirtyFlags;

  if (countTick) {
    pDirtyFlags = maNvDirtyFlags;
    for (i = 0; i < gNvNumberOfDataSets_c; ++i) {
      
      if(pDirtyFlags->ticksToNextSave)
        --(pDirtyFlags->ticksToNextSave);
      if(pDirtyFlags->ticksToNextSave)
        fTicksLeft = TRUE;

      if (pDirtyFlags->saveNextInterval
          && !pDirtyFlags->ticksToNextSave
          && !mNvCriticalSectionFlag) {
        NvSaveDataSet(i);
      }

      ++pDirtyFlags;
    }
  }

  return fTicksLeft;
#endif
}                                       /* NvTimerTick() */

/*****************************************************************************
******************************************************************************
* Private functions
******************************************************************************
*****************************************************************************/
/*****************************************************************************
*  PowerUpNVM - Turns on the NVM voltage regulator
******************************************************************************/

void PowerUpNVM(void)
{
  crmVRegCntl_t VRegCntl;

  VRegCntl.vReg = g1P8VReg_c;
  VRegCntl.cntl.vReg1P8VEn =1;
  CRM_VRegCntl(&VRegCntl);
  while(!CRM_VRegIsReady(V_REG_MASK_1P8V)){};
}

/*****************************************************************************
*  PowerDownNVM - Turns off the NVM voltage regulator
******************************************************************************/
void PowerDownNVM(void)
{
  crmVRegCntl_t VRegCntl;

  VRegCntl.vReg = g1P8VReg_c;
  VRegCntl.cntl.vReg1P8VEn =0;
  CRM_VRegCntl(&VRegCntl);
}


/****************************************************************************/

/* Given a data set ID, return it's index in the NvDataSetTable[].
 * Data set IDs are arbitrary values, so they have to be searched for.
 */
#if gNvStorageIncluded_d
static index_t NvDataSetIndexFromID
  (
  NvDataSetID_t dataSetID
 )
{
  index_t i;

  for (i = 0; i < gNvNumberOfDataSets_c; ++i) {
    if (NvDataSetTable[i].pItemDescriptions
         && (NvDataSetTable[i].dataSetID == dataSetID)) {
      return i;
    }
  }

  /* Can't find it. Must be a programming error in the caller. There's */
  /* no good way to handle errors, but at least this makes it obvious */
  /* during debugging. */
  for (;;) { }
}                                       /* NvDataSetIndexFromID() */
#endif                                  /* #if gNvStorageIncluded_d */


/****************************************************************************/

/* Examine an NV storage page. Check the magic number, check that the header
 * and trailer match, and check that the data set ID is recognized by the
 * application's table of data set descriptions.
 * Return TRUE if it looks legit; FALSE otherwise.
 */


#if gNvStorageIncluded_d
static bool_t NvIsValidPage
  (
  NvRawPageAddress_t nvRawPageAddress , NvDataSetID_t *pDataSetID
  )
{
  bool_t legitDataSetID;
  index_t i;
  NvStructuredPageHeader_t pageHeader;
  
  NvHalRead(nvRawPageAddress,0,(unsigned char*)&pageHeader,sizeof(NvStructuredPageHeader_t));
   

  /* Is the data set ID legit? */
  for (legitDataSetID = FALSE, i = 0; i < gNvNumberOfDataSets_c; ++i) 
    {
    if(NvDataSetTable[i].dataSetID && (NvDataSetTable[i].dataSetID == pageHeader.dataSetID)) 
      {
      legitDataSetID = TRUE;
      *pDataSetID = pageHeader.dataSetID;
      break;
      }
    }

  /* Do the header and trailer match? */
  /* Is the magic number legit? */
  if (legitDataSetID
       && NvHalVerify( nvRawPageAddress , sizeof(NvRawPage_t)- sizeof(NvStructuredPageHeader_t),(unsigned char*)&pageHeader,sizeof(NvStructuredPageHeader_t))
  
 
       && (pageHeader.magicNumber == mNvMagicNumber_c)) {
    return TRUE;
  }

  return FALSE;
}                                       /* NvIsValidDataSet() */
#endif                                  /* #if gNvStorageIncluded_d */
/****************************************************************************/

/* Examine an NV sector. Check the magic number, check that the header
 * and trailer match
 * Return TRUE if it looks legit; FALSE otherwise.
 */

/********************************************************/
#if gNvStorageIncluded_d
static bool_t NvIsValidSector
  (
  NvRawSectorAddress_t nvRawSectorAddress
  )
{

  NvStructuredSectorHeader_t sectorHeader;
  
  NvHalRead(nvRawSectorAddress,0,(unsigned char*)&sectorHeader,sizeof(NvStructuredSectorHeader_t));
  
  /* Do the header and trailer match? */
  /* Is the magic number legit? */
  if (NvHalVerify( nvRawSectorAddress , sizeof(NvRawSector_t)- sizeof(NvStructuredSectorHeader_t),(unsigned char*)&sectorHeader,sizeof(NvStructuredSectorHeader_t))
  
 
       && (sectorHeader.magicNumber == mNvMagicNumber_c)) {
    return TRUE;
  }

  return FALSE;
}                                       /* NvIsValidDataSet() */
#endif                                  /* #if gNvStorageIncluded_d */


/********************************************************/
#if gNvStorageIncluded_d
static bool_t NvOpen_Validate_Sector(NvRawSectorAddress_t nvRawSectorAddress, NvSectorSequenceNumber_t sequenceNumber, bool_t open )
  {
  NvStructuredSectorHeader_t sectorHeader;
  NvSize_t pageOffset;
  sectorHeader.magicNumber = mNvMagicNumber_c;
  sectorHeader.sequenceNumber = sequenceNumber;
  pageOffset = open ? 0 : ( sizeof(NvRawSector_t) - sizeof(NvStructuredSectorHeader_t) );
  if( NvHalWrite( nvRawSectorAddress , pageOffset , (uint8_t*)&sectorHeader , sizeof(NvStructuredSectorHeader_t)))
   {
    return FALSE;  
   }
  return TRUE;
  }                                       /* NvIsValidDataSet() */
#endif                                  /* #if gNvStorageIncluded_d */

  /****************************************************************************/

/* Examine an NV sector. Check the magic number, check that the header
 * and trailer match
 * Return TRUE if it looks legit; FALSE otherwise.
 */


#if gNvStorageIncluded_d

static void NvGetFirstBlankPageInfo
  (
  NvRawSectorAddress_t nvRawSectorAddress, NvRawPageAddress_t* pBlankPage, index_t* pBlankPageIndex 
  )
{
  NvRawPageAddress_t pageAddress;
  index_t pageIndex;
  *pBlankPage = gMaxPageOffset_c + nvRawSectorAddress;
  pageAddress  =   gLastPageOffset_c + nvRawSectorAddress;
  *pBlankPageIndex = pageIndex  = gNvNumberOfRawPages_c;
  
  while(pageIndex --)
  {
    if( NvHalIsBlank ( pageAddress, 0 , sizeof(NvRawPage_t)))
      {
      *pBlankPage = pageAddress;
      *pBlankPageIndex = pageIndex;
      }
    else
      {
        return;
      }
    pageAddress -= sizeof(NvRawPage_t);
  }  

}                                      
#endif                                  /* #if gNvStorageIncluded_d */
  /****************************************************************************/

/* Examine an NV sector. Check the magic number, check that the header
 * and trailer match
 * Return TRUE if it looks legit; FALSE otherwise.
 */


#if gNvStorageIncluded_d
      
/*typedef struct NvSectInfo_tag {
  NvSectorSequenceNumber_t sequenceNumber;
  NvRawPageAddress_t pageAddress[gNvNumberOfDataSets_c];
} NvSectInfo_t;
 typedef NvRawPageAddress_t NvSectPageInfo_t[ gNvNumberOfDataSets_c ];     
      */

static void NvGetSectorPageInfo
  (
  NvRawSectorAddress_t nvRawSectorAddress, NvSectPageInfo_t* pSectPageInfo
  )
{
  
  NvRawPageAddress_t pageAddress;
  index_t pageIndex;
  NvDataSetID_t dataSetID;
  for(pageIndex = 0; pageIndex < gNvNumberOfDataSets_c; pageIndex++)
  {
    (*pSectPageInfo)[pageIndex] = (NvRawPageAddress_t)gUninitAddress;
  }
  
 
  for(pageIndex = 0 ,pageAddress  = nvRawSectorAddress + sizeof(NvStructuredSectorHeader_t); pageIndex < gNvNumberOfRawPages_c ; pageIndex++ , pageAddress += sizeof(NvRawPage_t ))
  {
    
        if( NvIsValidPage( pageAddress , &dataSetID ))
          {
            (*pSectPageInfo)[NvDataSetIndexFromID(dataSetID)] = pageAddress;
          }
          
  }

}                                      
#endif                                  /* #if gNvStorageIncluded_d */      
            
  /****************************************************************************/

/* Examine an NV sector. Check the magic number, check that the header
 * and trailer match
 * Return TRUE if it looks legit; FALSE otherwise.
 */


#if gNvStorageIncluded_d

static NvSectorSequenceNumber_t NvGetSectorSequenceNumber
  (
  NvRawSectorAddress_t nvRawSectorAddress
  )
{
  NvSectorSequenceNumber_t sectorSequenceNumber;
  NvHalRead( nvRawSectorAddress,  sizeof(NvMagicNumber_t) , (uint8_t *)&sectorSequenceNumber, sizeof(sectorSequenceNumber));
  return sectorSequenceNumber;
}                                      
#endif                                  /* #if gNvStorageIncluded_d */                  
  /****************************************************************************/

/* Examine an NV sector. Check the magic number, check that the header
 * and trailer match
 * Return TRUE if it looks legit; FALSE otherwise.
 */


#if gNvStorageIncluded_d

static NvSectorSequenceNumber_t NvLastSectorSequence
(
NvSectorSequenceNumber_t sectorSeqA,NvSectorSequenceNumber_t sectorSeqB
)
{
  NvSectorSequenceNumber_t seqDiff;
  seqDiff = sectorSeqA - sectorSeqB;
  return (seqDiff == 1) ? sectorSeqA : sectorSeqB ;
}                                      
#endif                                  /* #if gNvStorageIncluded_d */                  
            
/****************************************************************************/

/* Save the contents of a data set, regardless of the state of the set's
 * dirty flag. Always writes a full page.
 *
 * Retry if the HAL reports an error. There isn't anything that the caller
 * can do about errors, so reporting one would be pointless.
 *
 * The flash controller reports errors that it detects, but power supply
 * fluctuations may still cause undetected errors. Its tempting to compute
 * some kind of checksum, and verify it after write, and also when a data
 * set is restored. This would complicate the code somewhat, and may not
 * be a common enough problem to be worth the extra code space.
 */
#if gNvStorageIncluded_d
static bool_t NvSaveDataSetLowLevel
  (
  index_t dataSetIndex , NvRawPageAddress_t pageAddress
  )
{
  NvDataItemDescription_t const *pDataItemDescriptions;
  NvDataSetID_t dataSetID;
  static NvStructuredPageHeader_t pageHeader;
  NvSize_t pageOffset;
  
  dataSetID = NvDataSetTable[dataSetIndex].dataSetID;

  /* Set up the page header/trailer. */
  pageHeader.magicNumber = mNvMagicNumber_c;
  pageHeader.dataSetID = dataSetID;
  pDataItemDescriptions = NvDataSetTable[dataSetIndex].pItemDescriptions;
  pageOffset = 0;

    /* Write the page header to the beginning of the page. */
   if( FALSE == NvHalWrite( pageAddress,  pageOffset,  (uint8_t*) &pageHeader,  sizeof(pageHeader)) )
     {
       return FALSE;
     }
   pageOffset += sizeof(pageHeader);

    /* Write the client data to the page. Note that the sizes of all of the */
    /* data sets in NvDataSetTable[] were validated in NvModuleInit(), so it */
    /* isn't necesary to check it here. */
    while ( pDataItemDescriptions->length) 
      {
       if( FALSE == NvHalWrite( pageAddress, pageOffset, (uint8_t*)(pDataItemDescriptions->pointer), pDataItemDescriptions->length))
         {
           return FALSE;
         }
       pageOffset += pDataItemDescriptions->length;
      ++pDataItemDescriptions;
      }

    /* Write the terminal copy of the header. If this works, we're done. */
    pageOffset =   sizeof(NvRawPage_t) - sizeof(NvStructuredPageHeader_t);
    if( FALSE == NvHalWrite( pageAddress, pageOffset, (uint8_t *) &pageHeader, sizeof(pageHeader)) )
      {
       return FALSE;
      }
    return TRUE;
}                                       /* NvSaveDataSetLowLevel() */


#endif                                  /* #if !gNvStorageIncluded_d */

/****************************************************************************/

/* Save the contents of a data set, regardless of the state of the set's
 * dirty flag. Always writes a full page.
 *
 * Retry if the HAL reports an error. There isn't anything that the caller
 * can do about errors, so reporting one would be pointless.
 *
 * The flash controller reports errors that it detects, but power supply
 * fluctuations may still cause undetected errors. Its tempting to compute
 * some kind of checksum, and verify it after write, and also when a data
 * set is restored. This would complicate the code somewhat, and may not
 * be a common enough problem to be worth the extra code space.
 */
#if gNvStorageIncluded_d
static void NvSaveDataSet
(
index_t dataSetIndex 
)
{
  index_t i;
  NvRawSectorAddress_t activeSectorAddress , oldSectorAddress ;
  NvRawPageAddress_t firstBlankPageAddress;
  bool_t sectorChange;
  bool_t sectorOpen;
  bool_t dataSaved;
  bool_t nvmError ;
  index_t sectorIndex;
  index_t firstBlankPageIndex;
  uint8_t retries;
  NvSectorSequenceNumber_t sectSeqNo;
  NvSectPageInfo_t validSectPageInfo, newSectPageInfo;
  
  PowerUpNVM();   
  retries = mNvFlashCmdRetries_c;
  while(retries--)  
  {
    nvmError = FALSE;  
    sectorChange = FALSE;
    sectorOpen = FALSE;
    activeSectorAddress = gUninitAddress;
    dataSaved = FALSE;
    // search for the valid sector
    for( sectorIndex=0; sectorIndex < gNvNumberOfRawSectors_c;sectorIndex++ )
    {
      if(NvIsValidSector(maNvRawSectorAddressTable[sectorIndex]))
      {
        activeSectorAddress = maNvRawSectorAddressTable[sectorIndex];
        NvGetFirstBlankPageInfo(activeSectorAddress, &firstBlankPageAddress, &firstBlankPageIndex);
        break;
      }
    }
    while(dataSaved == FALSE)
    {
      if(activeSectorAddress == gUninitAddress)
      {
        // There is no valid sector.Is the first time when a page is saved.  
        sectorIndex = 0;
        sectSeqNo = 0;
        activeSectorAddress = maNvRawSectorAddressTable[sectorIndex];
        firstBlankPageAddress = activeSectorAddress + sizeof(NvStructuredSectorHeader_t);
        firstBlankPageIndex = 0;
        if (nvmError =  NvOpenSector(activeSectorAddress, sectSeqNo))
        {
          break;
        }
        sectorOpen = TRUE;
      }
      else   
      {   
        if( gNvNumberOfRawPages_c == firstBlankPageIndex )
        {
          // there is no room to save all data sets  
          if(sectorChange || sectorOpen)
          {
            nvmError = TRUE;
            break;
            //after a sector change or after a sectorOpen data sets cannot be saved in the new sector.
            // must be a hardware issue. leave the loop and erase the sector again
          }
          else
          {
            
            sectSeqNo = NvGetSectorSequenceNumber(activeSectorAddress);
            oldSectorAddress = activeSectorAddress;
            NvGetSectorPageInfo(activeSectorAddress , &validSectPageInfo);
            sectSeqNo  +=  1;
            for(i=0 ; i < gNvNumberOfDataSets_c; i++ )
            {
              newSectPageInfo[i] = (NvRawPageAddress_t)gUninitAddress;
            }
            sectorIndex = ((sectorIndex + 1) < gNvNumberOfRawSectors_c)?(sectorIndex + 1):0 ;
            activeSectorAddress = maNvRawSectorAddressTable[sectorIndex];
            firstBlankPageAddress = activeSectorAddress + sizeof(NvStructuredSectorHeader_t);
            firstBlankPageIndex = 0;
            
            if (nvmError =  NvOpenSector(activeSectorAddress , sectSeqNo) )
            {
              break;
            }
            sectorChange = TRUE;
            
            // if a sector change all data sets to be saved must be saved in the new sector 
            //and after that the remains data sets must be copied from the old sector
          }
        }
      }//else from (activeSectorAddress == 0)
      if(NvSaveDataSetLowLevel(dataSetIndex,firstBlankPageAddress ))
      {
        dataSaved = TRUE; 
        if(sectorChange)
        {
          newSectPageInfo[dataSetIndex] = firstBlankPageAddress;
        }
      }  
      firstBlankPageAddress +=  sizeof(NvRawPage_t);
      firstBlankPageIndex ++;
      
    }//  while(dataSaved == FALSE))
    if((sectorChange || sectorOpen) && (nvmError == FALSE))
      {
      //the sector must be marked as a valid one(write the magic number and the sequence number at the end of the sector)
      nvmError = NvValidateSector(activeSectorAddress, sectSeqNo);
      }
    
    if(sectorChange &&(nvmError == FALSE))
    {
      // the remains data sets must be copied from the old sector
      nvmError =NvCopyTheRemainsDataSets(&validSectPageInfo, &newSectPageInfo,firstBlankPageAddress,firstBlankPageIndex);
    }

    if((sectorChange ) && (nvmError == FALSE))
    {
      NvHalEraseSector(oldSectorAddress);  
    } 
    if(nvmError)
    {
      NvHalEraseSector(activeSectorAddress);
      
      continue;  
    }
    
    maNvDirtyFlags[dataSetIndex].saveNextInterval = FALSE;
    maNvDirtyFlags[dataSetIndex].countsToNextSave = gNvCountsBetweenSaves_c;
    
    
    break;
  } 
  PowerDownNVM();
}                                       /* NvSaveDataSet() */


#endif                                  /* #if !gNvStorageIncluded_d */            
/****************************************************************************/
            
#if gNvStorageIncluded_d
static bool_t NvCopyDataSet
  (
  NvRawPageAddress_t sourcePage , NvRawPageAddress_t destPage
  )
{
  NvDataItemDescription_t const *pDataItemDescriptions;
  static NvStructuredPageHeader_t pageHeader;
  index_t dataSetIndex;
  uint8_t buffer[g_CopyBufferSize_c];
  NvSize_t length;
  NvSize_t numBytes;
  NvSize_t pageOffset;
  
  NvHalRead(sourcePage, 0, (uint8_t*)&pageHeader, sizeof(pageHeader));  
  dataSetIndex = NvDataSetIndexFromID(pageHeader.dataSetID); 
  /* Set up the page header/trailer. */
  pDataItemDescriptions = NvDataSetTable[dataSetIndex].pItemDescriptions;
  length = 0;
   while (pDataItemDescriptions->length)
   {
    length += pDataItemDescriptions->length;
    ++pDataItemDescriptions;
   }
 
  pageOffset = 0;
  if( FALSE == NvHalWrite( destPage,  pageOffset,  (uint8_t*) &pageHeader,  sizeof(pageHeader)) )
     {
       return FALSE;
     }
    
  pageOffset = sizeof(NvStructuredPageHeader_t);
  
  while(length)
  {
    numBytes = (length  <  sizeof(buffer)) ? length : sizeof(buffer);
    NvHalRead(sourcePage, pageOffset , buffer, numBytes);  
    if( FALSE == NvHalWrite( destPage,  pageOffset,  buffer, numBytes) )
     {
       return FALSE;
     }
    length -= numBytes;
    pageOffset += numBytes;
  }
   /* Write the terminal copy of the header. If this works, we're done. */
   pageOffset =   sizeof(NvRawPage_t) - sizeof(NvStructuredPageHeader_t);
    if( FALSE == NvHalWrite( destPage, pageOffset, (uint8_t *) &pageHeader, sizeof(pageHeader)) )
      {
       return FALSE;
      }
    return TRUE;
}                                       /* NvCopyDataSet() */


#endif                                  /* #if !gNvStorageIncluded_d */
            
/****************************************************************************/
            
#if gNvStorageIncluded_d
static bool_t  NvCopyTheRemainsDataSets
  (
   NvSectPageInfo_t* oldSectPageInfo, NvSectPageInfo_t *newSectPageInfo, NvRawPageAddress_t firstBlankPageAddress,index_t firstBlankPageIndex
  )
{
  index_t dataSetsToCopy;
  index_t dataSetIndex;
  for(dataSetIndex = 0 , dataSetsToCopy = 0 ; dataSetIndex < gNvNumberOfDataSets_c ; dataSetIndex++ )
    {
      if( ( (*oldSectPageInfo)[dataSetIndex] != gUninitAddress ) && ((*newSectPageInfo)[dataSetIndex] == gUninitAddress))
      {
        dataSetsToCopy++;
      }
    }
  if(!dataSetsToCopy)
    return FALSE;
  dataSetIndex = 0;
  while(dataSetsToCopy)
  {
     if((gNvNumberOfRawPages_c - firstBlankPageIndex) < dataSetsToCopy )
       return TRUE;
     if( ( (*oldSectPageInfo)[dataSetIndex] != gUninitAddress ) && ((*newSectPageInfo)[dataSetIndex] == gUninitAddress))
      {
        if( NvCopyDataSet( (*oldSectPageInfo)[dataSetIndex] , firstBlankPageAddress))
        {
          dataSetsToCopy--;
          dataSetIndex++;
        }
        firstBlankPageAddress +=  sizeof(NvRawPage_t);
        firstBlankPageIndex ++;
      }
     else
       {
        dataSetIndex++;
       }
  }
   
  return FALSE;

}                                       /* NvCopyDataSet() */


#endif                                  /* #if !gNvStorageIncluded_d */            

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

/* Size of a member of a struct. */
#ifndef MbrSizeof
#define MbrSizeof(type, member)     (sizeof(((type *) 0)->member))
#endif

#if gNvStorageIncluded_d
#define gNvSector0Address_c 0x1E000
#define gNvSector1Address_c 0x1D000
#define gNvSector2Address_c 0x1C000
#define gNvSector3Address_c 0x1B000
#define gNvSector4Address_c 0x1A000
#define gNvSector5Address_c 0x19000
#define gNvSector6Address_c 0x18000


#if (gNvNumberOfDataSetTypes_c  == 1)
 #define gNvNumberOfRawSectors_c   gNvNumberOfRawSectors1_c
#elif(gNvNumberOfDataSetTypes_c  == 2)
 #define gNvNumberOfRawSectors_c   (gNvNumberOfRawSectors1_c + gNvNumberOfRawSectors2_c)
#elif(gNvNumberOfDataSetTypes_c  == 3)
 #define gNvNumberOfRawSectors_c   (gNvNumberOfRawSectors1_c + gNvNumberOfRawSectors2_c + gNvNumberOfRawSectors3_c)
#endif

#if(gNvNumberOfRawSectors_c > gNvMaxNumberOfRawSectors_c)
#error Too many raw sectors defined
#endif

#define gNvRawSectorsBaseAddress_c 0x1E000
#define gNvRawSectorsBaseAddress1_c  gNvRawSectorsBaseAddress_c
#if( gNvNumberOfRawSectors1_c < 2)
#error There must be at least two sectors for a data set type
#endif
#if(gNvNumberOfDataSetTypes_c > 1) 
 #if( gNvNumberOfRawSectors2_c < 2)
 #error There must be at least two sectors for a data set type
 #endif
#define gNvRawSectorsBaseAddress2_c ( gNvRawSectorsBaseAddress1_c - gNvRawSectorSize_c* gNvNumberOfRawSectors1_c )
#endif
#if(gNvNumberOfDataSetTypes_c > 2) 
 #if( gNvNumberOfRawSectors3_c < 2)
 #error There must be at least two sectors for a data set type
 #endif
#define gNvRawSectorsBaseAddress3_c ( gNvRawSectorsBaseAddress2_c - gNvRawSectorSize_c* gNvNumberOfRawSectors2_c )
#endif

#define gNvRawPageSize1_c        (gNvMaxDataSetSize_type1_c + 2*sizeof(NvStructuredPageHeader_t))
#define gNvRawPageSize2_c        (gNvMaxDataSetSize_type2_c + 2*sizeof(NvStructuredPageHeader_t))
#define gNvRawPageSize3_c        (gNvMaxDataSetSize_type3_c + 2*sizeof(NvStructuredPageHeader_t))

#define gNvNumberOfRawPages1_c   (NvSize_t)((gNvRawSectorSize_c - 2*sizeof(NvStructuredSectorHeader_t))/gNvRawPageSize1_c )
#define gNvNumberOfRawPages2_c   (NvSize_t)((gNvRawSectorSize_c - 2*sizeof(NvStructuredSectorHeader_t))/gNvRawPageSize2_c )
#define gNvNumberOfRawPages3_c   (NvSize_t)((gNvRawSectorSize_c - 2*sizeof(NvStructuredSectorHeader_t))/gNvRawPageSize3_c )




/* There must be at least one more flash page than there are data sets, to */
/* allow for one copy of each data set plus a page for a new copy of one */
/* data set. */


/* this macro must reflect the bit used for the idle task defined in BeeApp.c */
#define gIdleTaskNVIntervalEvent_c  ( 1 << 0 )

#define gMaxPageOffset1_c  ( sizeof(NvStructuredSectorHeader_t) + gNvNumberOfRawPages1_c * gNvRawPageSize1_c )
#define gLastPageOffset1_c ( sizeof(NvStructuredSectorHeader_t) + (gNvNumberOfRawPages1_c - 1) * gNvRawPageSize1_c )

#define gMaxPageOffset2_c  ( sizeof(NvStructuredSectorHeader_t) + gNvNumberOfRawPages2_c * gNvRawPageSize2_c )
#define gLastPageOffset2_c ( sizeof(NvStructuredSectorHeader_t) + (gNvNumberOfRawPages2_c - 1) * gNvRawPageSize2_c )

#define gMaxPageOffset3_c  ( sizeof(NvStructuredSectorHeader_t) + gNvNumberOfRawPages3_c * gNvRawPageSize3_c )
#define gLastPageOffset3_c ( sizeof(NvStructuredSectorHeader_t) + (gNvNumberOfRawPages3_c - 1) * gNvRawPageSize3_c )

#define gUninitAddress     0xffffffff
#define NvOpenSector(nvRawSectorAddress, sequenceNumber)     NvOpen_Validate_Sector( nvRawSectorAddress , sequenceNumber, TRUE )
#define NvValidateSector(nvRawSectorAddress, sequenceNumber) NvOpen_Validate_Sector( nvRawSectorAddress , sequenceNumber, FALSE )
#define g_CopyBufferSize_c  64

#endif //gNvStorageIncluded_d




/*****************************************************************************
******************************************************************************
* Private type definitions
******************************************************************************
*****************************************************************************/

/* One entry per data set. */
typedef struct NvDataSetInfo_tag {
  bool_t saveNextInterval;
  NvSaveInterval_t ticksToNextSave;
  NvSaveCounter_t countsToNextSave;
} NvDataSetInfo_t;

typedef struct NvIndexesInfo_tag {
  index_t dirtyFlagsIndex;
  index_t dataSetTableIndex;
  index_t dataSetType;
} NvIndexesInfo_t;







/*****************************************************************************
******************************************************************************
* Private prototypes
******************************************************************************
*****************************************************************************/
#if gNvStorageIncluded_d
void PowerUpNVM(void);
void PowerDownNVM(void);
static void NvDataSetIndexesFromID(NvDataSetID_t dataSetID, NvIndexesInfo_t* pNvIndexesInfo);
static void NvDataSetIndexesFromIndex(index_t dataSetIndex,  NvIndexesInfo_t* pNvIndexesInfo);
static bool_t NvIsValidPage( NvRawPageAddress_t nvRawPageAddress , NvDataSetID_t *pDataSetID, index_t dataSetType );
static bool_t NvIsValidSector (  NvRawSectorAddress_t nvRawSectorAddress  );
static bool_t NvSaveDataSetLowLevel(  index_t dataSetIndex , NvRawPageAddress_t pageAddress  );
static void NvSaveDataSet( index_t dataSetIndex );
static void NvGetFirstBlankPageInfo(NvRawPageAddress_t nvRawSectorAddress, NvRawPageAddress_t* pBlankPage, NvSize_t* pBlankPageIndex, index_t dataSetType);
static NvSectorSequenceNumber_t NvGetSectorSequenceNumber( NvRawSectorAddress_t nvRawSectorAddress );
static bool_t NvCopyDataSet  (  NvRawPageAddress_t sourcePage , NvRawPageAddress_t destPage  );
static bool_t NvOpen_Validate_Sector(NvRawSectorAddress_t nvRawSectorAddress, NvSectorSequenceNumber_t sequenceNumber, bool_t open );
static NvSectorSequenceNumber_t NvLastSectorSequence ( NvSectorSequenceNumber_t sectorSeqA,NvSectorSequenceNumber_t sectorSeqB );
static void NvGetSectorPageInfo ( NvRawSectorAddress_t nvRawSectorAddress, NvSectPageInfo_t* pSectPageInfo,index_t dataSetType );
static bool_t  NvCopyTheRemainsDataSets ( NvSectPageInfo_t* oldSectPageInfo, NvSectPageInfo_t* newSectPageInfo, NvRawPageAddress_t firstBlankPageAddress,NvSize_t firstBlankPageIndex , index_t dataSetType);
#endif

#if gNvSelfTest_d
static void NvSelfTest(void);
#endif


/*****************************************************************************
******************************************************************************
* Private memory declarations
******************************************************************************
*****************************************************************************/
#if gNvStorageIncluded_d
 NvRawSectorAddress_t static maNvRawSectorAddressTable1[ gNvNumberOfRawSectors1_c];
 #if(gNvNumberOfDataSetTypes_c > 1) 
 NvRawSectorAddress_t static maNvRawSectorAddressTable2[ gNvNumberOfRawSectors2_c];
 #endif
 #if(gNvNumberOfDataSetTypes_c > 2) 
 NvRawSectorAddress_t static maNvRawSectorAddressTable3[ gNvNumberOfRawSectors3_c];
 #endif
 NvRawSectorAddress_t const * paNvRawSectorAddressTable[gNvNumberOfDataSetTypes_c]={
                                                                                      maNvRawSectorAddressTable1
  #if(gNvNumberOfDataSetTypes_c > 1)                                                                                        
                                                                                      ,maNvRawSectorAddressTable2
  #endif                                                                                        
  #if(gNvNumberOfDataSetTypes_c > 2)                                                                                                                                                                                
                                                                                      ,maNvRawSectorAddressTable3
  #endif                                                                                     
                                                                                    };
uint32_t const gaMaxPageOffset[gNvNumberOfDataSetTypes_c]={
                                                          gMaxPageOffset1_c
#if(gNvNumberOfDataSetTypes_c > 1)                                                            
                                                         ,gMaxPageOffset2_c
#endif
#if(gNvNumberOfDataSetTypes_c > 2)                                                           
                                                         ,gMaxPageOffset3_c  
#endif                                                           
                                                         };
uint32_t const gaLastPageOffset[gNvNumberOfDataSetTypes_c] = {
                                                             gLastPageOffset1_c
#if(gNvNumberOfDataSetTypes_c > 1)                                                              
                                                            ,gLastPageOffset2_c
#endif                                                              
#if(gNvNumberOfDataSetTypes_c > 2)                                                              
                                                            ,gLastPageOffset3_c  
#endif                                                              
                                                            };
uint8_t const gaNvNumberOfDataSets[gNvNumberOfDataSetTypes_c] = {
                                                             gNvNumberOfDataSets1_c
#if(gNvNumberOfDataSetTypes_c > 1)                                                              
                                                            ,gNvNumberOfDataSets2_c
#endif                                                              
#if(gNvNumberOfDataSetTypes_c > 2)                                                              
                                                            ,gNvNumberOfDataSets3_c  
#endif                                                              
                                                            };

uint8_t const gaNvNumberOfRawSectors[gNvNumberOfDataSetTypes_c] = {
                                                             gNvNumberOfRawSectors1_c
#if(gNvNumberOfDataSetTypes_c > 1)                                                              
                                                            ,gNvNumberOfRawSectors2_c
#endif                                                              
#if(gNvNumberOfDataSetTypes_c > 2)                                                              
                                                            ,gNvNumberOfRawSectors3_c  
#endif
                                                            };


NvSize_t const gaNvRawPageSize[gNvNumberOfDataSetTypes_c] = {
                                                             gNvRawPageSize1_c
#if(gNvNumberOfDataSetTypes_c > 1)                                                              
                                                            ,gNvRawPageSize2_c
#endif                                                              
#if(gNvNumberOfDataSetTypes_c > 2)                                                              
                                                            ,gNvRawPageSize3_c 
#endif
                                                            };
#if gNvDebug_d
 NvSize_t const gaNvClientPageSize[gNvNumberOfDataSetTypes_c] = {
                                                             gNvMaxDataSetSize_type1_c
 #if(gNvNumberOfDataSetTypes_c > 1)                                                              
                                                            ,gNvMaxDataSetSize_type2_c
 #endif                                                              
 #if(gNvNumberOfDataSetTypes_c > 2)                                                              
                                                            ,gNvMaxDataSetSize_type3_c 
 #endif
                                                            };
#endif


NvSize_t const gaNvNumberOfRawPages[gNvNumberOfDataSetTypes_c] = {
                                                             gNvNumberOfRawPages1_c
#if(gNvNumberOfDataSetTypes_c > 1)                                                              
                                                            ,gNvNumberOfRawPages2_c
#endif                                                              
#if(gNvNumberOfDataSetTypes_c > 2)                                                              
                                                            ,gNvNumberOfRawPages3_c 
#endif
                                                            };


/* Table of dirty flags, one per dataset in NvDataSetDescriptionTable[]. */

static NvDataSetInfo_t maNvDirtyFlags[gNvNumberOfDataSets_c];


/* Minimum number of calls to NvTimerTick() between saves of a given dataset. */
NvSaveInterval_t gNvMinimumTicksBetweenSaves = gNvMinimumTicksBetweenSaves_c;
/* Minimum number of calls to NvSaveOnIdle() between saves of a given dataset. */
static NvSaveCounter_t gNvCountsBetweenSaves = gNvCountsBetweenSaves_c;

/* If this counter is != 0, do not save to NV Storage. */
static uint8_t mNvCriticalSectionFlag = 0;
#endif                                  /* #if gNvStorageIncluded_d */

/* Scratch data sets used by the internal unit test. */


/*****************************************************************************
******************************************************************************
* Public functions
******************************************************************************
*****************************************************************************/
/****************************************************************************/

/* Return TRUE if the given data set is dirty. */
NvDataSetID_t NvAddressToDataSetId
  (
  void* pAddress
  )
{
#if !gNvStorageIncluded_d
  (void)pAddress;
  return gNvInvalidDataSetID_c;
#else
  index_t   dataSetIndex;
  index_t dataSetType;
  NvDataItemDescription_t const *pDataItemDescriptions;
  for(dataSetType = 0 ; dataSetType < gNvNumberOfDataSetTypes_c; dataSetType++ )
  {
    for( dataSetIndex = 0 ; dataSetIndex < gaNvNumberOfDataSets[dataSetType] ; dataSetIndex++)    
    {
      
      pDataItemDescriptions = (paNvDataSetTable[dataSetType])[dataSetIndex].pItemDescriptions;
      while (pDataItemDescriptions->length)
      {
        if( ((uint32_t) pAddress >= (uint32_t)pDataItemDescriptions->pointer)
           &&((uint32_t)pAddress < (uint32_t)pDataItemDescriptions->pointer + pDataItemDescriptions->length )  
             )
        {
          return (paNvDataSetTable[dataSetType])[dataSetIndex].dataSetID;
        }
        
        ++pDataItemDescriptions;
      }
    }
    
  }
  return gNvInvalidDataSetID_c;
  
#endif                                  /* #if !gNvStorageIncluded_d #else */
}                                       
/* There may be operations that take place over extended times that must
 * be atomic from the point of view of NV Storage saves. These routines
 * increment/decrement a counter; when the counter is non-zero, no NV
 * saves will be done. Note that this does not affect NV restores.
 */

void NvClearCriticalSection(void) {
#if gNvStorageIncluded_d
  if(mNvCriticalSectionFlag)  /* dg - in case of set/clear mismatch */
    --mNvCriticalSectionFlag;
#endif /* gNvStorageIncluded_d */
}

void NvSetCriticalSection(void) {
#if gNvStorageIncluded_d
  ++mNvCriticalSectionFlag;
#endif /* gNvStorageIncluded_d */
}


/****************************************************************************/

/* Called from the idle task to process save-on-next-idle and save-on-count. */
void NvIdle(void)
{
#if gNvStorageIncluded_d
  index_t i;

  if (mNvCriticalSectionFlag) {
    return;
  }
   
  for (i = 0; i < gNvNumberOfDataSets_c; ++i) {
    if (!maNvDirtyFlags[i].countsToNextSave) {
      NvSaveDataSet(i);
    }
  }
#endif
}                                       /* NvIdle() */

/****************************************************************************/

/* Return TRUE if the given data set is dirty. */
bool_t NvIsDataSetDirty
  (
  NvDataSetID_t dataSetID
  )
{
#if !gNvStorageIncluded_d
  (void) dataSetID;
  return FALSE;
#else
  NvIndexesInfo_t nvIndexesInfo;
  NvDataSetIndexesFromID(dataSetID, &nvIndexesInfo);
  return (    maNvDirtyFlags[nvIndexesInfo.dirtyFlagsIndex].saveNextInterval
          || (maNvDirtyFlags[nvIndexesInfo.dirtyFlagsIndex].countsToNextSave != gNvCountsBetweenSaves_c));
#endif                                  /* #if !gNvStorageIncluded_d #else */
}                                       /* NvIsDataSetDirty() */

/****************************************************************************/

/* Call this once, before calling any other NV function.
 *
 * Any page that does not contain a valid, recognized data set is erased, in
 * preparation for later reuse.
 *
 * It is unlikely, but possible, that there could be more than one copy of one
 * or more data sets in NV storage. Normally the page containing the older
 * version is erased after the new version is written, but a reset at just
 * the wrong time could prevent the erasure.
 *
 * Having more than one copy of any data set is 1) unnecessary, 2) uses space
 * that may be needed for future writes, and 3) would complicate other NV code.
 * This function scans NV storage for alternate versions of the same data set,
 * and if it finds one, erases the older copy (or copies).
 */
void NvModuleInit(void)
{
#if gNvStorageIncluded_d
#if gNvDebug_d
  NvDataItemDescription_t const *pDataItemDescriptions;
  index_t dataSetIndex;
  NvSize_t dataSetSize;
#endif
  index_t i;
  index_t sectorIndex;
  NvRawSectorAddress_t const * maNvRawSectorAddressTable;
  NvRawSectorAddress_t      oldSectAddress , newSectAddress;
  NvSectorSequenceNumber_t  oldSectSeq , newSectSeq;
  NvSectPageInfo_t oldSectPageInfo ,  newSectPageInfo;
  NvRawPageAddress_t firstBlankPageAddress;
  NvSize_t firstBlankPageIndex;
  index_t dataSetType;

  /* Power up the NVM */
  PowerUpNVM();
  /* initialize flash layer */
  NvHalInit();
  
  /* No data set starts dirty. */
  for (i = 0; i < gNvNumberOfDataSets_c; ++i)
  {
    maNvDirtyFlags[i].saveNextInterval = FALSE;
    maNvDirtyFlags[i].countsToNextSave = gNvCountsBetweenSaves;
  }
  
  // initialization of raw sectors adresses
  
  for( i=0,newSectAddress = gNvRawSectorsBaseAddress1_c  ; i < gNvNumberOfRawSectors1_c ; i++,newSectAddress -= gNvRawSectorSize_c )
  {
    maNvRawSectorAddressTable1[i] = newSectAddress; 
  }
  
#if(gNvNumberOfDataSetTypes_c > 1) 
  for( i=0,newSectAddress = gNvRawSectorsBaseAddress2_c  ; i < gNvNumberOfRawSectors2_c ; i++,newSectAddress -= gNvRawSectorSize_c )
  {
    maNvRawSectorAddressTable2[i] = newSectAddress; 
  } 
#endif
#if(gNvNumberOfDataSetTypes_c > 2) 
  for( i=0,newSectAddress = gNvRawSectorsBaseAddress3_c  ; i < gNvNumberOfRawSectors3_c ; i++,newSectAddress -= gNvRawSectorSize_c )
  {
    maNvRawSectorAddressTable3[i] = newSectAddress; 
  } 
#endif    
  
  
  /* Scan for and erase duplicate copies of data sets. This code is very */
  /* slow (N^2), but it only runs once, and N is small. Data set IDs are */
  /* arbitrary values, so its necessary to search through the list. */
  
  /* Visit every raw page. If it isn't a valid data set, and it isn't clean, */
  /* erase it. */
 
  for(dataSetType = 0; dataSetType < gNvNumberOfDataSetTypes_c; dataSetType++)
  {
    
    newSectAddress = gUninitAddress;
    maNvRawSectorAddressTable = paNvRawSectorAddressTable[dataSetType];
    for (sectorIndex = 0; sectorIndex < gaNvNumberOfRawSectors[dataSetType]; ++sectorIndex)
    {
      
      if (!NvIsValidSector(maNvRawSectorAddressTable[sectorIndex])) 
      {
        if( !NvHalIsBlank(maNvRawSectorAddressTable[sectorIndex] , 0 , sizeof(NvRawSector_t)) )
        {
          NvHalEraseSector(maNvRawSectorAddressTable[sectorIndex]);
        }
      } 
      else
      {
        if(newSectAddress != gUninitAddress )
        {
          newSectSeq = NvGetSectorSequenceNumber(newSectAddress);
          oldSectSeq =   NvGetSectorSequenceNumber(maNvRawSectorAddressTable[sectorIndex]);
          if(oldSectSeq ==  NvLastSectorSequence ( newSectSeq ,oldSectSeq )  )
          {
            oldSectAddress = newSectAddress ;
            newSectAddress = maNvRawSectorAddressTable[sectorIndex];
          }
          else
          {
            oldSectAddress = maNvRawSectorAddressTable[sectorIndex];
          }
          NvGetSectorPageInfo ( newSectAddress, &newSectPageInfo, dataSetType );
          NvGetSectorPageInfo ( oldSectAddress, &oldSectPageInfo, dataSetType );
          NvGetFirstBlankPageInfo ( newSectAddress , &firstBlankPageAddress, &firstBlankPageIndex, dataSetType );
          if( NvCopyTheRemainsDataSets(  &oldSectPageInfo , &newSectPageInfo, firstBlankPageAddress,firstBlankPageIndex,dataSetType))
          {
            // copy of te rest of data sets failed
            NvHalEraseSector(newSectAddress);
            newSectAddress = oldSectAddress;
          }
          else
          {
            NvHalEraseSector(oldSectAddress);
          }
        }
        else // if(newSectAddress != gUninitAddress )
        {
          newSectAddress = maNvRawSectorAddressTable[sectorIndex];
        }
      }                                   /* if (!NvIsValidDataSet(... else */
    }
  } 
  
  
  
  /* for (pageIndex = 0; ... */
  
#if gNvDebug_d
  /* NVM_Interface.h defines the size of the client area of a page as an */
  /* unjustified immediate integer constant. Make sure that its correct. */
  /* Assert if it isn't. */
  if((gNvNumberOfRawPages1_c < gNvNumberOfDataSets1_c) || (gNvNumberOfRawPages2_c < gNvNumberOfDataSets2_c) || (gNvNumberOfRawPages3_c < gNvNumberOfDataSets3_c))
  {
    for (;;)
    { }
  }
  
  /* Verify that all of the data sets defined by the client code will */
  /* fit into the NV storage page size. Doing this once here simplifies */
  /* other code. */
  
  for(dataSetType = 0 ; dataSetType < gNvNumberOfDataSetTypes_c; dataSetType++ )
  {
    for( dataSetIndex = 0 ; dataSetIndex < gaNvNumberOfDataSets[dataSetType] ; dataSetIndex++)    
    {
      
      pDataItemDescriptions = (paNvDataSetTable[dataSetType])[dataSetIndex].pItemDescriptions;
      dataSetSize = 0;
      while (pDataItemDescriptions->length)
      {
        dataSetSize += pDataItemDescriptions->length;
        
        ++pDataItemDescriptions;
      }
      if (dataSetSize > gaNvClientPageSize[dataSetType]) 
      {
        for (;;)
        { }
      }
    }
          
  }
  
  

#endif                                  /* #if gNvDebug_d */
  
  /* Power down the NVM */
  PowerDownNVM();
  
#if gNvSelfTest_d
  NvSelfTest();
#endif
#endif                                  /* #if gNvStorageIncluded_d */
}                                       /* NvModuleInit() */


/****************************************************************************/

/* Copy the most recent version of a data set from NV storage to RAM. */
/* Note that the copy will succeed if a valid copy of the data set is found */
/* in NV storage, regardless of the state of the data set's dirty flag. */
/* Return TRUE if the copy is successful. */

bool_t NvRestoreDataSet
  (
  NvDataSetID_t dataSetID
 )
{
#if !gNvStorageIncluded_d
  (void) dataSetID;
  return FALSE;
#else
  NvDataItemDescription_t const *pDataItemDescriptions;
  NvIndexesInfo_t nvIndexesInfo;
  index_t sectorIndex;
  NvRawSectorAddress_t sectorAddress;
  NvSectPageInfo_t sectPageInfo;
  NvSize_t pageOffset;
  /* Power up the NVM */
  PowerUpNVM();
  /* Find the data set description. */
  NvDataSetIndexesFromID(dataSetID, &nvIndexesInfo);
  for( sectorIndex=0; sectorIndex < gaNvNumberOfRawSectors[nvIndexesInfo.dataSetType] ; sectorIndex++ )
       {
       sectorAddress = (paNvRawSectorAddressTable[nvIndexesInfo.dataSetType])[sectorIndex];  
       if(NvIsValidSector(sectorAddress))
         {
          break;
         }
       }
  if(sectorIndex == gaNvNumberOfRawSectors[nvIndexesInfo.dataSetType])
  {
    // there are no valid sectors
    PowerDownNVM(); 
    return FALSE;
  }

  NvGetSectorPageInfo(sectorAddress , &sectPageInfo, nvIndexesInfo.dataSetType);
  if(sectPageInfo[nvIndexesInfo.dirtyFlagsIndex] == gUninitAddress)
  {
    // the page does not exist in the valid sector
    PowerDownNVM(); 
    return FALSE;
  }
 pDataItemDescriptions = (paNvDataSetTable[nvIndexesInfo.dataSetType])[nvIndexesInfo.dataSetTableIndex].pItemDescriptions; 

  /* Start reading just after the page header. */
  pageOffset = sizeof(NvStructuredPageHeader_t);

  /* Copy data from the NV storage page to the destination data set. */
  while (pDataItemDescriptions->length)
  {
    NvHalRead(sectPageInfo[nvIndexesInfo.dirtyFlagsIndex], pageOffset, (uint8_t*)pDataItemDescriptions->pointer, pDataItemDescriptions->length);
    pageOffset += pDataItemDescriptions->length;
    ++pDataItemDescriptions;
  }

  maNvDirtyFlags[nvIndexesInfo.dirtyFlagsIndex].saveNextInterval = FALSE;
  maNvDirtyFlags[nvIndexesInfo.dirtyFlagsIndex].countsToNextSave = gNvCountsBetweenSaves_c;
  /* Power down the NVM */
  PowerDownNVM();
  return TRUE;
#endif                                  /* #if !gNvStorageIncluded_d #else */
}
/****************************************************************************/

/* Save the data set on the next call to NvIdle(). */
/* Use the save-on-count mechanism. */
void NvSaveOnIdle
  (
  NvDataSetID_t dataSetID
 )
{
#if !gNvStorageIncluded_d
  (void) dataSetID;
#else
  NvIndexesInfo_t nvIndexesInfo;
  NvDataSetIndexesFromID(dataSetID, &nvIndexesInfo);
  maNvDirtyFlags[nvIndexesInfo.dirtyFlagsIndex].countsToNextSave = 0;
#endif
}

/****************************************************************************/

void NvSaveOnInterval
  (
  NvDataSetID_t dataSetID
 )
{
#if !gNvStorageIncluded_d
  (void) dataSetID;
#else
  NvIndexesInfo_t nvIndexesInfo;
  NvDataSetIndexesFromID(dataSetID, &nvIndexesInfo );
  if (!maNvDirtyFlags[nvIndexesInfo.dirtyFlagsIndex].saveNextInterval) {
    maNvDirtyFlags[nvIndexesInfo.dirtyFlagsIndex].ticksToNextSave  = gNvMinimumTicksBetweenSaves_c;
    maNvDirtyFlags[nvIndexesInfo.dirtyFlagsIndex].saveNextInterval = TRUE;
    TS_SendEvent(gIdleTaskID, gIdleTaskNVIntervalEvent_c);  
  }
#endif
}                                       /* NvSaveOnInterval() */

/****************************************************************************/

/* Decrement the counter. Once it reaches 0, the next call to NvIdle() will */
/* save the data set. */
void NvSaveOnCount
  (
  NvDataSetID_t dataSetID
  )
{
#if !gNvStorageIncluded_d
  (void) dataSetID;
#else
  NvIndexesInfo_t nvIndexesInfo;
  NvDataSetIndexesFromID(dataSetID, &nvIndexesInfo );
  if (maNvDirtyFlags[nvIndexesInfo.dirtyFlagsIndex].countsToNextSave) {
    --maNvDirtyFlags[nvIndexesInfo.dirtyFlagsIndex].countsToNextSave;
  }
#endif
}                                       /* NvSaveOnCount() */

/****************************************************************************/

/* Set the timer used by NvSaveOnInterval(). Takes effect after the next */
/* save. */
void NvSetMinimumTicksBetweenSaves
  (
  NvSaveInterval_t newInterval
 )
{
#if !gNvStorageIncluded_d
  (void) newInterval;
#else
  gNvMinimumTicksBetweenSaves = newInterval;
#endif
}                                       /* NvSetMinimumTicksBetweenSaves() */

/****************************************************************************/

/* Set the counter trigger value used by NvSaveOnCount(). Takes effect */
/* after the next save. */
void NvSetCountsBetweenSaves
  (
  NvSaveCounter_t newCounter
 )
{
#if !gNvStorageIncluded_d
  (void) newCounter;
#else
  gNvCountsBetweenSaves = newCounter;
#endif
}                                       /* NvSetCountsBetweenSaves() */

/****************************************************************************/

/* Called from the idle task to process save-on-interval requests. */
/* Returns FALSE if the timer tick counters for all data sets have reached */
/* zero. In this case, the timer can be turned off. */
/* Returns TRUE if any of the data sets' timer tick counters have not yet */
/* counted down to zero. In this case, the timer should be active. */
bool_t NvTimerTick(bool_t countTick)
{
#if !gNvStorageIncluded_d
  (void) countTick;
  return FALSE;
#else
  index_t i;
  bool_t fTicksLeft = FALSE;
  NvDataSetInfo_t *pDirtyFlags;

  if (countTick) {
    pDirtyFlags = maNvDirtyFlags;
    for (i = 0; i < gNvNumberOfDataSets_c; ++i) {
      
      if(pDirtyFlags->ticksToNextSave)
        --(pDirtyFlags->ticksToNextSave);
      if(pDirtyFlags->ticksToNextSave)
        fTicksLeft = TRUE;

     if (pDirtyFlags->saveNextInterval && !pDirtyFlags->ticksToNextSave)
      {
        if(!mNvCriticalSectionFlag)
        {
          NvSaveDataSet(i);
        }
        else
        {
          pDirtyFlags->countsToNextSave = 0;
        }
      }
      ++pDirtyFlags;
    }
  }

  return fTicksLeft;
#endif
}                                       /* NvTimerTick() */

/*****************************************************************************
******************************************************************************
* Private functions
******************************************************************************
*****************************************************************************/
/*****************************************************************************
*  PowerUpNVM - Turns on the NVM voltage regulator
******************************************************************************/

void PowerUpNVM(void)
{
  crmVRegCntl_t VRegCntl;

  VRegCntl.vReg = g1P8VReg_c;
  VRegCntl.cntl.vReg1P8VEn =1;
  CRM_VRegCntl(&VRegCntl);
  while(!CRM_VRegIsReady(V_REG_MASK_1P8V)){};
}

/*****************************************************************************
*  PowerDownNVM - Turns off the NVM voltage regulator
******************************************************************************/
void PowerDownNVM(void)
{
  crmVRegCntl_t VRegCntl;

  VRegCntl.vReg = g1P8VReg_c;
  VRegCntl.cntl.vReg1P8VEn =0;
  CRM_VRegCntl(&VRegCntl);
}


/****************************************************************************/

/* Given a data set ID, return it's index in the NvDataSetTable[].
 * Data set IDs are arbitrary values, so they have to be searched for.
 */

#if gNvStorageIncluded_d
static void NvDataSetIndexesFromID
  (
  NvDataSetID_t dataSetID,
  NvIndexesInfo_t* pNvIndexesInfo
  )
  {
    index_t i,j,dataSetType;
    
    for (dataSetType = 0 , i = 0 , j=0; i < gNvNumberOfDataSets_c; j++,i++) 
    {
      if(j == gaNvNumberOfDataSets[dataSetType])
      {
        dataSetType++;
        j=0;
        if(dataSetType >= gNvNumberOfDataSetTypes_c)
        {
          break;
        }
      }
      
      if ((*(paNvDataSetTable[dataSetType] + j)).pItemDescriptions
          && ((*(paNvDataSetTable[dataSetType] + j)).dataSetID == dataSetID)) 
      {
        pNvIndexesInfo->dirtyFlagsIndex = i;
        pNvIndexesInfo->dataSetTableIndex = j;
        pNvIndexesInfo->dataSetType = dataSetType;       
        return ;
      }
    }
    
    /* Can't find it. Must be a programming error in the caller. There's */
    /* no good way to handle errors, but at least this makes it obvious */
    /* during debugging. */
    for (;;) { }
  }                                       /* NvDataSetIndexesFromID() */
#endif                                  /* #if gNvStorageIncluded_d */

/****************************************************************************/

/* Given a data set index, return its data set type .
 * Data set IDs are arbitrary values, so they have to be searched for.
 */
#if gNvStorageIncluded_d
static void NvDataSetIndexesFromIndex
  (
  index_t dataSetIndex,
  NvIndexesInfo_t * pNvIndexesInfo
 )
  {
    index_t i;
    pNvIndexesInfo->dirtyFlagsIndex = dataSetIndex;
    for(i=0 ; i<gNvNumberOfDataSetTypes_c ; i++)
    {
      if(dataSetIndex <gaNvNumberOfDataSets[i])
      {
        pNvIndexesInfo->dataSetType = i;
        pNvIndexesInfo->dataSetTableIndex = dataSetIndex;
        return ; 
      }
      dataSetIndex -=  gaNvNumberOfDataSets[i];
    }
    
    
    /* Can't find it. Must be a programming error in the caller. There's */
    /* no good way to handle errors, but at least this makes it obvious */
    /* during debugging. */
    for (;;) { }
  }                                       /* NvDataSetIndexesFromID() */
#endif                                  /* #if gNvStorageIncluded_d */

/****************************************************************************/

/* Examine an NV storage page. Check the magic number, check that the header
 * and trailer match, and check that the data set ID is recognized by the
 * application's table of data set descriptions.
 * Return TRUE if it looks legit; FALSE otherwise.
 */


#if gNvStorageIncluded_d
static bool_t NvIsValidPage
  (
  NvRawPageAddress_t nvRawPageAddress , NvDataSetID_t *pDataSetID, index_t dataSetType
  )
{
  bool_t legitDataSetID;
  index_t i;
  NvStructuredPageHeader_t pageHeader;
  
  NvHalRead(nvRawPageAddress,0,(unsigned char*)&pageHeader,sizeof(NvStructuredPageHeader_t));
   

  /* Is the data set ID legit? */
  for (legitDataSetID = FALSE, i = 0; i < gaNvNumberOfDataSets[dataSetType]; ++i) 
    {
    if((paNvDataSetTable[dataSetType])[i].dataSetID && ((paNvDataSetTable[dataSetType])[i].dataSetID == pageHeader.dataSetID)) 
      {
      legitDataSetID = TRUE;
      *pDataSetID = pageHeader.dataSetID;
      break;
      }
    }

  /* Do the header and trailer match? */
  /* Is the magic number legit? */
  if (legitDataSetID
       && NvHalVerify( nvRawPageAddress , gaNvRawPageSize[dataSetType]- sizeof(NvStructuredPageHeader_t),(unsigned char*)&pageHeader,sizeof(NvStructuredPageHeader_t))
  
 
       && (pageHeader.magicNumber == mNvMagicNumber_c)) {
    return TRUE;
  }

  return FALSE;
}                                       /* NvIsValidDataSet() */
#endif                                  /* #if gNvStorageIncluded_d */
/****************************************************************************/

/* Examine an NV sector. Check the magic number, check that the header
 * and trailer match
 * Return TRUE if it looks legit; FALSE otherwise.
 */

/********************************************************/
#if gNvStorageIncluded_d
static bool_t NvIsValidSector
  (
  NvRawSectorAddress_t nvRawSectorAddress
  )
{

  NvStructuredSectorHeader_t sectorHeader;
  
  NvHalRead(nvRawSectorAddress,0,(unsigned char*)&sectorHeader,sizeof(NvStructuredSectorHeader_t));
  
  /* Do the header and trailer match? */
  /* Is the magic number legit? */
  if (NvHalVerify( nvRawSectorAddress , sizeof(NvRawSector_t)- sizeof(NvStructuredSectorHeader_t),(unsigned char*)&sectorHeader,sizeof(NvStructuredSectorHeader_t))
  
 
       && (sectorHeader.magicNumber == mNvMagicNumber_c)) {
    return TRUE;
  }

  return FALSE;
}                                       /* NvIsValidDataSet() */
#endif                                  /* #if gNvStorageIncluded_d */


/********************************************************/
#if gNvStorageIncluded_d
static bool_t NvOpen_Validate_Sector(NvRawSectorAddress_t nvRawSectorAddress, NvSectorSequenceNumber_t sequenceNumber, bool_t open )
  {
  NvStructuredSectorHeader_t sectorHeader;
  NvSize_t pageOffset;
  sectorHeader.magicNumber = mNvMagicNumber_c;
  sectorHeader.sequenceNumber = sequenceNumber;
  pageOffset = open ? 0 : ( sizeof(NvRawSector_t) - sizeof(NvStructuredSectorHeader_t) );
  if( NvHalWrite( nvRawSectorAddress , pageOffset , (uint8_t*)&sectorHeader , sizeof(NvStructuredSectorHeader_t)))
   {
    return FALSE;  
   }
  return TRUE;
  }                                       /* NvIsValidDataSet() */
#endif                                  /* #if gNvStorageIncluded_d */

  /****************************************************************************/

/* Examine an NV sector. Check the magic number, check that the header
 * and trailer match
 * Return TRUE if it looks legit; FALSE otherwise.
 */


#if gNvStorageIncluded_d

static void NvGetFirstBlankPageInfo
  (
  NvRawSectorAddress_t nvRawSectorAddress, NvRawPageAddress_t* pBlankPage, NvSize_t* pBlankPageIndex, index_t dataSetType 
  )
{
  NvRawPageAddress_t pageAddress;
  NvSize_t pageIndex;
  *pBlankPage = gaMaxPageOffset[dataSetType] + nvRawSectorAddress;
  pageAddress  =   gaLastPageOffset[dataSetType] + nvRawSectorAddress;
  *pBlankPageIndex = pageIndex  = gaNvNumberOfRawPages[dataSetType];
  
  while(pageIndex --)
  {
    if( NvHalIsBlank ( pageAddress, 0 , gaNvRawPageSize[dataSetType]))
      {
      *pBlankPage = pageAddress;
      *pBlankPageIndex = pageIndex;
      }
    else
      {
        return;
      }
    pageAddress -= gaNvRawPageSize[dataSetType];
  }  

}                                      
#endif                                  /* #if gNvStorageIncluded_d */
  /****************************************************************************/

/* Examine an NV sector. Check the magic number, check that the header
 * and trailer match
 * Return TRUE if it looks legit; FALSE otherwise.
 */


#if gNvStorageIncluded_d
      
static void NvGetSectorPageInfo
  (
  NvRawSectorAddress_t nvRawSectorAddress, NvSectPageInfo_t* pSectPageInfo, index_t dataSetType
  )
{
  NvRawPageAddress_t pageAddress;
  NvSize_t pageIndex;
  NvDataSetID_t dataSetID;
  NvIndexesInfo_t nvIndexesInfo;
  for(pageIndex = 0; pageIndex < gNvNumberOfDataSets_c; pageIndex++)
  {
    (*pSectPageInfo)[pageIndex] = (NvRawPageAddress_t)gUninitAddress;
  }
  
 
  for(pageIndex = 0 ,pageAddress  = nvRawSectorAddress + sizeof(NvStructuredSectorHeader_t); pageIndex < gaNvNumberOfRawPages[dataSetType] ; pageIndex++ , pageAddress += gaNvRawPageSize[dataSetType])
  {
    
        if( NvIsValidPage( pageAddress , &dataSetID, dataSetType  ))
          {
            NvDataSetIndexesFromID(dataSetID, &nvIndexesInfo); 
            (*pSectPageInfo)[nvIndexesInfo.dirtyFlagsIndex] = pageAddress;
          }
          
  }

}                                      
#endif                                  /* #if gNvStorageIncluded_d */      
            
  /****************************************************************************/

/* Examine an NV sector. Check the magic number, check that the header
 * and trailer match
 * Return TRUE if it looks legit; FALSE otherwise.
 */


#if gNvStorageIncluded_d

static NvSectorSequenceNumber_t NvGetSectorSequenceNumber
  (
  NvRawSectorAddress_t nvRawSectorAddress
  )
{
  NvSectorSequenceNumber_t sectorSequenceNumber;
  NvHalRead( nvRawSectorAddress,  sizeof(NvMagicNumber_t) , (uint8_t *)&sectorSequenceNumber, sizeof(sectorSequenceNumber));
  return sectorSequenceNumber;
}                                      
#endif                                  /* #if gNvStorageIncluded_d */                  
  /****************************************************************************/

/* Examine an NV sector. Check the magic number, check that the header
 * and trailer match
 * Return TRUE if it looks legit; FALSE otherwise.
 */


#if gNvStorageIncluded_d

static NvSectorSequenceNumber_t NvLastSectorSequence
(
NvSectorSequenceNumber_t sectorSeqA,NvSectorSequenceNumber_t sectorSeqB
)
{
  NvSectorSequenceNumber_t seqDiff;
  seqDiff = sectorSeqA - sectorSeqB;
  return (seqDiff == 1) ? sectorSeqA : sectorSeqB ;
}                                      
#endif                                  /* #if gNvStorageIncluded_d */                  
            
/****************************************************************************/

/* Save the contents of a data set, regardless of the state of the set's
 * dirty flag. Always writes a full page.
 *
 * Retry if the HAL reports an error. There isn't anything that the caller
 * can do about errors, so reporting one would be pointless.
 *
 * The flash controller reports errors that it detects, but power supply
 * fluctuations may still cause undetected errors. Its tempting to compute
 * some kind of checksum, and verify it after write, and also when a data
 * set is restored. This would complicate the code somewhat, and may not
 * be a common enough problem to be worth the extra code space.
 */
#if gNvStorageIncluded_d
static bool_t NvSaveDataSetLowLevel
  (
  index_t dataSetIndex , NvRawPageAddress_t pageAddress
  )
{
  NvDataItemDescription_t const *pDataItemDescriptions;
  NvDataSetID_t dataSetID;
  static NvStructuredPageHeader_t pageHeader;
  NvSize_t pageOffset;
  NvIndexesInfo_t nvIndexesInfo;
  NvDataSetIndexesFromIndex(dataSetIndex, &nvIndexesInfo);
  dataSetID = (paNvDataSetTable[nvIndexesInfo.dataSetType])[nvIndexesInfo.dataSetTableIndex].dataSetID;

  /* Set up the page header/trailer. */
  pageHeader.magicNumber = mNvMagicNumber_c;
  pageHeader.dataSetID = dataSetID;
  pDataItemDescriptions = (paNvDataSetTable[nvIndexesInfo.dataSetType])[nvIndexesInfo.dataSetTableIndex].pItemDescriptions;
  pageOffset = 0;

    /* Write the page header to the beginning of the page. */
   if( FALSE == NvHalWrite( pageAddress,  pageOffset,  (uint8_t*) &pageHeader,  sizeof(pageHeader)) )
     {
       return FALSE;
     }
   pageOffset += sizeof(pageHeader);

    /* Write the client data to the page. Note that the sizes of all of the */
    /* data sets in NvDataSetTable[] were validated in NvModuleInit(), so it */
    /* isn't necesary to check it here. */
    while ( pDataItemDescriptions->length) 
      {
       if( FALSE == NvHalWrite( pageAddress, pageOffset, (uint8_t*)(pDataItemDescriptions->pointer), pDataItemDescriptions->length))
         {
           return FALSE;
         }
       pageOffset += pDataItemDescriptions->length;
      ++pDataItemDescriptions;
      }

    /* Write the terminal copy of the header. If this works, we're done. */
    pageOffset = gaNvRawPageSize[nvIndexesInfo.dataSetType] - sizeof(NvStructuredPageHeader_t);
    
    if( FALSE == NvHalWrite( pageAddress, pageOffset, (uint8_t *) &pageHeader, sizeof(pageHeader)) )
      {
       return FALSE;
      }
    return TRUE;
}                                       /* NvSaveDataSetLowLevel() */


#endif                                  /* #if !gNvStorageIncluded_d */

/****************************************************************************/

/* Save the contents of a data set, regardless of the state of the set's
 * dirty flag. Always writes a full page.
 *
 * Retry if the HAL reports an error. There isn't anything that the caller
 * can do about errors, so reporting one would be pointless.
 *
 * The flash controller reports errors that it detects, but power supply
 * fluctuations may still cause undetected errors. Its tempting to compute
 * some kind of checksum, and verify it after write, and also when a data
 * set is restored. This would complicate the code somewhat, and may not
 * be a common enough problem to be worth the extra code space.
 */
#if gNvStorageIncluded_d
static void NvSaveDataSet
(
index_t dataSetIndex 
)
{
  index_t i;
  NvRawSectorAddress_t activeSectorAddress , oldSectorAddress ;
  NvRawPageAddress_t firstBlankPageAddress;
  bool_t sectorChange;
  bool_t sectorOpen;
  bool_t dataSaved;
  bool_t nvmError ;
  index_t sectorIndex;
  NvSize_t firstBlankPageIndex;
  uint8_t retries;
  NvSectorSequenceNumber_t sectSeqNo;
  NvSectPageInfo_t validSectPageInfo, newSectPageInfo;
  NvIndexesInfo_t nvIndexesInfo;
  NvRawSectorAddress_t const * maNvRawSectorAddressTable;
  
  PowerUpNVM();   
  NvDataSetIndexesFromIndex(dataSetIndex, &nvIndexesInfo);
  maNvRawSectorAddressTable = paNvRawSectorAddressTable[nvIndexesInfo.dataSetType];
  retries = mNvFlashCmdRetries_c;
  while(retries--)  
  {
    nvmError = FALSE;  
    sectorChange = FALSE;
    sectorOpen = FALSE;
    activeSectorAddress = gUninitAddress;
    dataSaved = FALSE;
    // search for the valid sector
    for( sectorIndex=0; sectorIndex < gaNvNumberOfRawSectors[nvIndexesInfo.dataSetType];sectorIndex++ )
    {
      if(NvIsValidSector(maNvRawSectorAddressTable[sectorIndex]))
      {
        activeSectorAddress = maNvRawSectorAddressTable[sectorIndex];
        NvGetFirstBlankPageInfo(activeSectorAddress, &firstBlankPageAddress, &firstBlankPageIndex,nvIndexesInfo.dataSetType);
        break;
      }
    }
    while(dataSaved == FALSE)
    {
      if(activeSectorAddress == gUninitAddress)
      {
        // There is no valid sector.Is the first time when a page is saved.  
        sectorIndex = 0;
        sectSeqNo = 0;
        activeSectorAddress = maNvRawSectorAddressTable[sectorIndex];
        firstBlankPageAddress = activeSectorAddress + sizeof(NvStructuredSectorHeader_t);
        firstBlankPageIndex = 0;
        if (nvmError =  NvOpenSector(activeSectorAddress, sectSeqNo))
        {
          break;
        }
        sectorOpen = TRUE;
      }
      else   
      {
        if( gaNvNumberOfRawPages[nvIndexesInfo.dataSetType] == firstBlankPageIndex )
        {
          // there is no room to save all data sets  
          if(sectorChange || sectorOpen)
          {
            nvmError = TRUE;
            break;
            //after a sector change or after a sectorOpen data sets cannot be saved in the new sector.
            // must be a hardware issue. leave the loop and erase the sector again
          }
          else
          {
            
            sectSeqNo = NvGetSectorSequenceNumber(activeSectorAddress);
            oldSectorAddress = activeSectorAddress;
            NvGetSectorPageInfo(activeSectorAddress , &validSectPageInfo, nvIndexesInfo.dataSetType);
            sectSeqNo  +=  1;
            for(i=0 ; i < gNvNumberOfDataSets_c; i++ )
            {
              newSectPageInfo[i] = (NvRawPageAddress_t)gUninitAddress;
            }
            sectorIndex = ((sectorIndex + 1) < gaNvNumberOfRawSectors[nvIndexesInfo.dataSetType])?(sectorIndex + 1):0 ;
            activeSectorAddress = maNvRawSectorAddressTable[sectorIndex];
            firstBlankPageAddress = activeSectorAddress + sizeof(NvStructuredSectorHeader_t);
            firstBlankPageIndex = 0;
            
            if (nvmError =  NvOpenSector(activeSectorAddress , sectSeqNo) )
            {
              break;
            }
            sectorChange = TRUE;
            
            // if a sector change all data sets to be saved must be saved in the new sector 
            //and after that the remains data sets must be copied from the old sector
          }
        }
      }//else from (activeSectorAddress == 0)
      if(NvSaveDataSetLowLevel(dataSetIndex,firstBlankPageAddress ))
      {
        dataSaved = TRUE; 
        if(sectorChange)
        {
          newSectPageInfo[dataSetIndex] = firstBlankPageAddress;
        }
      }  
      firstBlankPageAddress +=  gaNvRawPageSize[nvIndexesInfo.dataSetType];
      firstBlankPageIndex ++;
      
    }//  while(dataSaved == FALSE))
    if((sectorChange || sectorOpen) && (nvmError == FALSE))
      {
      //the sector must be marked as a valid one(write the magic number and the sequence number at the end of the sector)
      nvmError = NvValidateSector(activeSectorAddress, sectSeqNo);
      }
    
    if(sectorChange &&(nvmError == FALSE))
    {
      // the remains data sets must be copied from the old sector
      nvmError =NvCopyTheRemainsDataSets(&validSectPageInfo, &newSectPageInfo,firstBlankPageAddress,firstBlankPageIndex,nvIndexesInfo.dataSetType);
    }

    if((sectorChange ) && (nvmError == FALSE))
    {
      NvHalEraseSector(oldSectorAddress);  
    } 
    if(nvmError)
    {
      NvHalEraseSector(activeSectorAddress);
      
      continue;  
    }
    
    maNvDirtyFlags[dataSetIndex].saveNextInterval = FALSE;
    maNvDirtyFlags[dataSetIndex].countsToNextSave = gNvCountsBetweenSaves_c;
    
    
    break;
  } 
  PowerDownNVM();
}                                       /* NvSaveDataSet() */


#endif                                  /* #if !gNvStorageIncluded_d */            
/****************************************************************************/
            
#if gNvStorageIncluded_d
static bool_t NvCopyDataSet
  (
  NvRawPageAddress_t sourcePage , NvRawPageAddress_t destPage 
  )
{
  NvDataItemDescription_t const *pDataItemDescriptions;
  static NvStructuredPageHeader_t pageHeader;
  uint8_t buffer[g_CopyBufferSize_c];
  NvSize_t length;
  NvSize_t numBytes;
  NvSize_t pageOffset;
  NvIndexesInfo_t nvIndexesInfo;
  NvHalRead(sourcePage, 0, (uint8_t*)&pageHeader, sizeof(pageHeader));  
  NvDataSetIndexesFromID(pageHeader.dataSetID, &nvIndexesInfo);
    /* Set up the page header/trailer. */
  pDataItemDescriptions = (paNvDataSetTable[nvIndexesInfo.dataSetType])[nvIndexesInfo.dataSetTableIndex].pItemDescriptions;
  
  length = 0;
   while (pDataItemDescriptions->length)
   {
    length += pDataItemDescriptions->length;
    ++pDataItemDescriptions;
   }
 
  pageOffset = 0;
  if( FALSE == NvHalWrite( destPage,  pageOffset,  (uint8_t*) &pageHeader,  sizeof(pageHeader)) )
     {
       return FALSE;
     }
    
  pageOffset = sizeof(NvStructuredPageHeader_t);
  
  while(length)
  {
    numBytes = (length  <  sizeof(buffer)) ? length : sizeof(buffer);
    NvHalRead(sourcePage, pageOffset , buffer, numBytes);  
    if( FALSE == NvHalWrite( destPage,  pageOffset,  buffer, numBytes) )
     {
       return FALSE;
     }
    length -= numBytes;
    pageOffset += numBytes;
  }
   /* Write the terminal copy of the header. If this works, we're done. */
    pageOffset =   gaNvRawPageSize[nvIndexesInfo.dataSetType]  - sizeof(NvStructuredPageHeader_t);
    if( FALSE == NvHalWrite( destPage, pageOffset, (uint8_t *) &pageHeader, sizeof(pageHeader)) )
      {
       return FALSE;
      }
    return TRUE;
}                                       /* NvCopyDataSet() */


#endif                                  /* #if !gNvStorageIncluded_d */
            
/****************************************************************************/
            
#if gNvStorageIncluded_d
static bool_t  NvCopyTheRemainsDataSets
  (
   NvSectPageInfo_t* oldSectPageInfo, NvSectPageInfo_t *newSectPageInfo, NvRawPageAddress_t firstBlankPageAddress,NvSize_t firstBlankPageIndex, index_t dataSetType
  )
{
  index_t dataSetsToCopy;
  index_t dataSetIndex;
  for(dataSetIndex = 0 , dataSetsToCopy = 0 ; dataSetIndex < gNvNumberOfDataSets_c ; dataSetIndex++ )
    {
      if( ( (*oldSectPageInfo)[dataSetIndex] != gUninitAddress ) && ((*newSectPageInfo)[dataSetIndex] == gUninitAddress))
      {
        dataSetsToCopy++;
      }
    }
  if(!dataSetsToCopy)
    return FALSE;
  dataSetIndex = 0;
  while(dataSetsToCopy)
  {
     if((gaNvNumberOfRawPages[dataSetType] - firstBlankPageIndex) < dataSetsToCopy )
       return TRUE;
     if( ( (*oldSectPageInfo)[dataSetIndex] != gUninitAddress ) && ((*newSectPageInfo)[dataSetIndex] == gUninitAddress))
      {
        if( NvCopyDataSet( (*oldSectPageInfo)[dataSetIndex] , firstBlankPageAddress))
        {
          dataSetsToCopy--;
          dataSetIndex++;
        }
        firstBlankPageAddress +=  gaNvRawPageSize[dataSetType];
        firstBlankPageIndex ++;
      }
     else
       {
        dataSetIndex++;
       }
  }
   
  return FALSE;

}                                       /* NvCopyDataSet() */


#endif                                  /* #if !gNvStorageIncluded_d */            

#endif /* #if !gArm7ExtendedNVM_d */
    