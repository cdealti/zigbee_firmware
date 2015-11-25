/*****************************************************************************
* NV storage module interface declarations.
*
* (c) Copyright 2006, Freescale, Inc. All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
*****************************************************************************/

#ifndef _NVM_INTERFACE_H
#define _NVM_INTERFACE_H

#include "NV_Data.h"



#ifndef gNvStorageIncluded_d
#define gNvStorageIncluded_d    FALSE       /* enable/disable NV storage module. */
#endif

/* Switch internal debugging code on/off. */
#ifndef gNvDebug_d
#define gNvDebug_d              0       /* Turn on/off NV storage debugging. */
#endif

#ifndef gNvSelfTest_d
#define gNvSelfTest_d           0       /* Turn on/off NV storage self test. */
#endif

/* Which progamming mode should NV storage used: */
/* 1 -> Byte-at-a-time is slower, but it uses less RAM. */
/* 0 -> Use burst mode flash programming. */
#ifndef gNvByteAtATimeProgramming_d
#define gNvByteAtATimeProgramming_d   0     /* Use burst mode flash programming. */
#endif

/* Make sure to turn off the macros fro the targets that do not have it ON. */
#ifndef gStandardSecurity_d
#define gStandardSecurity_d 0
#endif

#ifndef gHighSecurity_d
#define gHighSecurity_d 0
#endif


/* *IMPORTANT*
 *
 * There is some manual coordination required between the NV storage code and
 * the application code that uses it.
 *
 * On the application side, NV storage is organized into "data sets". Each
 * data set is described by a table of pointers and lengths of data in the
 * set. The length of the table is variable; the last entry is recognized
 * by having a NULL pointer and a zero length.
 *
 * The NV storage functions define a struct, NvStructuredPage_t, that contains
 * some overhead information plus a client data area. The size of the client
 * data area is based on the page size provided by the NV storage HAL.
 *
 * A single application data set must not be larger than the client area of
 * an NvStructuredPage_t.
 *
 * The data sets used by the application code are defined at compile time
 * in NV_Data.c, in NvDataSetTable[]. The size of that array must be defined
 * in this file, to make it available to the NV storage code.
 *
 * The NV storage HAL must have at least one more page available
 * to it than the number of data sets defined by the application. Generally,
 * this requires coordinating the NV HAL and linker commands to reserve
 * flash memory pages. The details are handled in the NV storage code.
 *
 * Some compiler preprocessors have enough access to the compiler's symbol
 * table to be able to take the sizeof() a C struct. Some don't, making
 * #if tests of the number of data sets vs. the number of page, and of
 * the sizes of the data set, impossible. This module defines both values
 * as magic constants. The actual values are determined by hardware
 * requirements and the linker command file, which is not available to
 * C code.
 *
 * Since these contsants cannot be reliably checked at compile time, the NV
 * module checks them at run time. If the debugging compile flag is turned
 * on in the NV code, the NV module initialization function will assert
 * (infinite loop) if:
 *      1) There is not at least one more NV storage page available then
 *      there are data sets, or
 *      2) If any data set is larger than the client data area of an NV
 *      storage page.
 */

/*****************************************************************************
******************************************************************************
* Public macros
******************************************************************************
*****************************************************************************/

/* Number of data sets defined by the application code.
 * If gNvDebug_d is set, this constant is checked at runtime against the
 * number of available NV storage pages. There must be at least one more
 * page reserved for NV storage than there are data sets, so this number
 * should be correct, and small.
 * To change the number of data sets and/or NV storage pages:
 *      Set gNvNumberOfDataSets_c in NVM_Interface.h, and
 *      Make sure the number of entries in the NvDataSetTable[] array in
 *          NV_Data.c matches the value of gNvNumberOfDataSets_c. and
 *      Set NvNumberOfRawPages_c in NV_FlashHAL.h to >= gNvNumberOfDataSets_c + 1
  */

#if !gArm7ExtendedNVM_d

#if gStandardSecurity_d || gHighSecurity_d
#define gNvNumberOfDataSets_c   4
#else
#define gNvNumberOfDataSets_c   3
#endif

/* Size of the client area of an NV storage page. Will be verified correct */
/* at run time in the NV storage code if it's debug flag is turned on. */
/* Otherwise, this is just an unjustified, unreliable magic constant. */
/* 512 - 4 bytes each for header & trailer */
#define gNvMaxDataSetSize_c     504

/* Default minimum-timer-ticks-between-dataset-saves, in seconds. */
/* See NvSaveOnInterval(). This is used for all data sets. */
typedef uint16_t NvSaveInterval_t;
#ifndef gNvMinimumTicksBetweenSaves_c
#define gNvMinimumTicksBetweenSaves_c   4
#endif

/* Default number-of-calls-to-NvSaveOnCount-between-dataset-saves. */
/* See NvSaveOnCount(). This is used for all data sets. */
typedef uint16_t NvSaveCounter_t;
#ifndef gNvCountsBetweenSaves_c
#define gNvCountsBetweenSaves_c         256
#endif

#ifndef gNvIncrementAfterRestore_c
#define gNvIncrementAfterRestore_c      gNvCountsBetweenSaves_c
#endif

/*****************************************************************************
*****************************************************************************/
#else /* #if !gArm7ExtendedNVM_d */
/*****************************************************************************
*****************************************************************************/

#ifndef gNvNumberOfDataSetTypes_c
#define gNvNumberOfDataSetTypes_c   3
#endif

#ifndef gNvNumberOfDataSets1_c
#define gNvNumberOfDataSets1_c   2
#endif

#ifndef gNvNumberOfDataSets2_c
#define gNvNumberOfDataSets2_c   2
#endif

#ifndef gNvNumberOfDataSets3_c
#define gNvNumberOfDataSets3_c   2
#endif

#if (gNvNumberOfDataSetTypes_c  == 1)
 #define gNvNumberOfDataSets_c   gNvNumberOfDataSets1_c
#elif(gNvNumberOfDataSetTypes_c  == 2)
 #define gNvNumberOfDataSets_c   (gNvNumberOfDataSets1_c + gNvNumberOfDataSets2_c)
#elif(gNvNumberOfDataSetTypes_c  == 3)
 #define gNvNumberOfDataSets_c   (gNvNumberOfDataSets1_c + gNvNumberOfDataSets2_c + gNvNumberOfDataSets3_c)
#elif
 #error No more than 3 data set type can be defined
#endif
#if( gNvNumberOfDataSets_c > 254 )  
 #error Number of data sets overflow
#endif

/* Size of the client area of an NV storage page. Will be verified correct */
/* at run time in the NV storage code if it's debug flag is turned on. */
/* Otherwise, this is just an unjustified, unreliable magic constant. */
/* 512 - 4 bytes each for header & trailer */
#define gNvMaxDataSetSize_type1_c     504
#define gNvMaxDataSetSize_type2_c     504
#define gNvMaxDataSetSize_type3_c     1016

#define gNvNumberOfRawSectors1_c   2
#define gNvNumberOfRawSectors2_c   2
#define gNvNumberOfRawSectors3_c   2

#define gNvMaxNumberOfRawSectors_c   7

/* Default minimum-timer-ticks-between-dataset-saves, in seconds. */
/* See NvSaveOnInterval(). This is used for all data sets. */
typedef uint16_t NvSaveInterval_t;
#ifndef gNvMinimumTicksBetweenSaves_c
#define gNvMinimumTicksBetweenSaves_c   4
#endif

/* Default number-of-calls-to-NvSaveOnCount-between-dataset-saves. */
/* See NvSaveOnCount(). This is used for all data sets. */
typedef uint16_t NvSaveCounter_t;
#ifndef gNvCountsBetweenSaves_c
#define gNvCountsBetweenSaves_c         256
#endif

#ifndef gNvIncrementAfterRestore_c
#define gNvIncrementAfterRestore_c      gNvCountsBetweenSaves_c
#endif

#endif

/****************************************************************************/

/* The client may require notification when an NV operation is going to be */
/* performed. NV operations can be slow, and the client may want to prepare */
/* for the long wait. */
extern void BUtl_SetReceiverOff(void);
extern void BUtl_SetReceiverOn(void);
#define NvOperationStart()  BUtl_SetReceiverOff()
#define NvOperationEnd()    BUtl_SetReceiverOn()

/*****************************************************************************
******************************************************************************
* Public type declarations
******************************************************************************
*****************************************************************************/

/* There may be zero or more separate NV data sets; each set has its own */
/* definition for it's contents. A small integer is used to recognize which */
/* data set is currently contained in a given NV page. */

typedef uint8_t NvDataSetID_t;

#define gNvInvalidDataSetID_c   ((NvDataSetID_t) -1)

/* Length of one item in a data set. */
typedef uint16_t NvDataItemLength_t;

/* One entry in a data set description. */
typedef struct NvDataItemDescription_tag {
  void *pointer;
  NvDataItemLength_t length;
} NvDataItemDescription_t;

/* A description of a complete data set. */
/* *warning* A dataset must not be larger than the data field of an NvStructuredPage_t. */
/* dataSetID is used to identify a particular data set, both in the NV */
/* storage function arguments and in the NV storage pages. */
/* pItemDescriptions points to a variable size array of item descriptions. */
/* The end of the table is recognized by an entry with a NULL pointer and */
/* a zero size. */
typedef struct NvDataSetDescription_tag {
  NvDataSetID_t dataSetID;
  NvDataItemDescription_t const *pItemDescriptions;
} NvDataSetDescription_t;

/* The table of data set descriptions. The content of the table is defined */
/* by the application code. */

#if !gArm7ExtendedNVM_d
extern NvDataSetDescription_t const NvDataSetTable[gNvNumberOfDataSets_c];

#else
extern NvDataSetDescription_t const* paNvDataSetTable[gNvNumberOfDataSetTypes_c];
#endif

#define gNvInvalidDataSetIndex_c    ((index_t) -1)

/*****************************************************************************
******************************************************************************
* Public prototypes
******************************************************************************
*****************************************************************************/

/* Data sets are only saved by the idle task. There is no save-immediately
 * capability.
 *
 * There are three save mechansims, each with it's own API function.
 *      1) NvSaveOnIdle(): save the next time the idle task runs. This
 *      will save the data set at least as soon as either of the other
 *      functions, and usually sooner.
 *
 *      2) NvSaveOnInterval(): save no more often than a given time
 *      interval. If it has been at least that long since the last save,
 *      this function will cause a save the next time the idle task runs.
 *
 *      3) NvSaveOnCount(): increment a counter that is associated with the
 *      data set. When that counter equals or exceeds some trigger value,
 *      save the next time the idle task runs.
 *
 * The data set will be saved to NV storage during the next execution of
 * the idle task if:
 *      NvSaveOnIdle() has been called,
 *  OR
 *      NvSaveOnInterval() has been called, and at least the specified
 *      time has past since the last save,
 *  OR
 *      NvSaveOnCount() has been called at least as often as the counter's
 *      trigger value since the last save.
 *
 * All three functions cause NvIsDataSetDirty() to return TRUE. No API is
 * provided to allow the client to know which save function or functions
 * have been called. The data set is either believed to be identical to
 * the saved copy (== not ditry) or not identical (== dirty).
 *
 * The NV code does not require a data set to have a mininum-time-between-
 * saves or a minimum-count-between-saves. A data set may have both.
 *
 * Whenever a data set is saved for any reason:
 *      it's dirty flag is cleared
 *  AND
 *      it's minimum-time-between-saves timer is restarted from 0,
 *  AND
 *      it's minimum-count-between-saves counter is set to 0.
 */
extern void NvSaveOnIdle(NvDataSetID_t dataSetID);
extern void NvSaveOnInterval(NvDataSetID_t dataSetID);
extern void NvSaveOnCount(NvDataSetID_t dataSetID);

/* Set the timer used by NvSaveOnInterval(). Takes effect after the next */
/* save. */
extern void NvSetMinimumTicksBetweenSaves(NvSaveInterval_t newInterval);

/* Set the counter trigger value used by NvSaveOnCount(). Takes effect */
/* after the next save. */
extern void NvSetCountsBetweenSaves(NvSaveCounter_t newCounter);

/* Return TRUE if the given data set is dirty. */
extern bool_t NvIsDataSetDirty(NvDataSetID_t dataSetID);

/* Initialize the NV storage module. Does not restore any data sets. */
extern void NvModuleInit(void);

/* If there is a valid copy of the data set in NV storage, restore it's */
/* data to RAM and return TRUE. Otherwise return FALSE. */
/* Set the data set's dirty flag to FALSE. */
extern bool_t NvRestoreDataSet(NvDataSetID_t dataSetID);

/* These functions should be called ONLY from the idle task. */
 
/* NvIdle() processes NvSaveOnIdle() and NvSaveOnCount() requests. */
extern void NvIdle(void);

/* NvTimerTick() processes NvSaveOnInterval() requests. */
/* If this call should count a timer tick, call with countTick TRUE. */
/* Otherwise, leave countTick FALSE. */
/* Regardless of the value of countTick, NvTimerTick() returns */
/*      TRUE if one or more of the data sets' tick counters have not yet */
/*          counted down to zero, or */
/*      FALSE if all data set tick counters have reached zero. */
/* If NvTimerTick() returns TRUE,  the timer should be on. */
/* If NvTimerTick() returns FALSE, the timer can be turned off. */
extern bool_t NvTimerTick(bool_t countTick);

/* THERE may be operations that take place over extended times that must */
/* be atomic from the point of view of NV Storage saves. These routines */
/* increment/decrement a counter; when the counter is non-zero, no NV */
/* saves will be done. Note that this does not affect NV restores. */
extern void NvClearCriticalSection(void);
extern void NvSetCriticalSection(void);
#if gArm7ExtendedNVM_d
extern NvDataSetID_t NvAddressToDataSetId(void* pAddress);
#endif
/*
#define NvSaveOnIdle(dataSetID)
#define NvSaveOnInterval(dataSetID)
#define NvSaveOnCount(dataSetID)
#define NvSetMinimumTicksBetweenSaves(newInterval)
#define NvSetCountsBetweenSaves(newCounter)
#define NvIsDataSetDirty(dataSetID) FALSE
#define NvModuleInit() 
#define NvRestoreDataSet(dataSetID) TRUE
#define NvIdle()
#define NvOperationEnd()
#define NvTimerTick(countTick) FALSE
#define NvClearCriticalSection() 
#define NvSetCriticalSection() 
*/

#endif /* _NVM_INTERFACE_H */

