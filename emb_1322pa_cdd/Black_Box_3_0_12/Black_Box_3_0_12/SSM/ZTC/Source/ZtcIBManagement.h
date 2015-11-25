/******************************************************************************
* ZtcIBManagement.c
* Access to the NWK/APS Information Base
*
* Copyright (c) 2008, Freescale, Inc.  All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
******************************************************************************/

#ifndef _ZtcIBManagement_c
#define _ZtcIBManagement_c

/******************************************************************************
*******************************************************************************
* Private type definitions
*******************************************************************************
******************************************************************************/

#if gBeeStackIncluded_d

/* These structures are defined by ZigBee, but never actually sent over the */
/* air. Used for both scalar and vector IB entries. */
typedef struct getIBRequest_tag {
  zbIBAttributeId_t iId;                /* Which IB entry is being requested. */
  index_t iIndex;                       /* Index of first entry to get. */
  index_t iEntries;                     /* # of entries to get or # returned. */
  uint8_t iEntrySize;                   /* Size of each entry returned. */
  void *pData;                          /* Ignored in req; # bytes returned. */
} getIBRequest_t;

typedef struct setIBRequest_tag {
  zbIBAttributeId_t iId;
  index_t iIndex;
  index_t iEntries;
  uint8_t iEntrySize;
  void *pData;
} setIBRequest_t;

/* A scalar can be thought of as a table with only one entry. */
/* Used for non-array entries in ztcIBData_t.iMaxRecords. */
#define mIBScalar   1

/* Mark entries in the IB table as read/write or read-only. */
/* Used in ztcIBData_t.access. */
typedef enum {
  mZtcIBRO,
  mZtcIBRW,
  mZtcIBRWUseFunc
} mZtcIBAccess_t;

/* Describe the data that the ZTC_GetIB.Request and ZTC_SetIB.Request */
/* utility messages can access. */
typedef struct ztcIBData_tag {
  zbIBAttributeId_t id;
  mZtcIBAccess_t access;
  uint8_t entrySize;
  index_t maxEntries;
  void *pTable;
} ztcIBData_t;


#endif /* gBeeStackIncluded_d */
#endif                                  /* #ifndef _ZtcIBManagement_c */
