/******************************************************************************
* Data type definition for 11073-20601 Optimized Exchange Protocol
*
* Copyright (c) 2009, Freescale, Inc.  All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
******************************************************************************/

#ifndef _OEP_TYPES_INTERFACE_H
#define _OEP_TYPES_INTERFACE_H

#include "EmbeddedTypes.h"

/******************************************************************************
*******************************************************************************
* Public macros
*******************************************************************************
******************************************************************************/


/******************************************************************************
*******************************************************************************
* Public prototypes
*******************************************************************************
******************************************************************************/


/******************************************************************************
*******************************************************************************
* Public type definitions
*******************************************************************************
******************************************************************************/

#ifdef __IAR_SYSTEMS_ICC__
#pragma pack(1)
#endif

typedef struct any_tag 
{ 
  uint16_t length; 
  uint8_t  value[1];    /* first element of the array */ 
} any_t; 
 
typedef uint16_t oidType_t; 
 
typedef uint16_t privateOid_t; 
 
typedef uint16_t oepHandle_t; 
 
typedef uint16_t instNumber_t; 
 
typedef uint16_t nomPartition_t; 

#define          gNomPartUnspec_d           0 
#define          gNomPartObj_d              1 
#define          gNomPartMetric_d           2 
#define          gNomPartAlert_d            3 
#define          gNomPartDim_d              4 
#define          gNomPartVattr_d            5 
#define          gNomPartPgrp_d             6 
#define          gNomPartSites_d            7 
#define          gNomPartInfrastruct_d      8 
#define          gNomPartFef_d              9 
#define          gNomPartEcgExtn_d         10 
#define          gNomPartPhdDm_d          128 
#define          gNomPartPhdHf_d          129 
#define          gNomPartPhdAi_d          130 
#define          gNomPartRetCode_d        255 
#define          gNomPartExtNom_d         256 
#define          gNomPartPriv_d          1024 

typedef struct oepType_tag 
{ 
  nomPartition_t partition; 
  oidType_t code; 
} oepType_t; 
 
typedef struct ava_tag 
{ 
  oidType_t attributeId; 
  any_t attributeValue; 
} ava_t; 
 
typedef struct attributeList_tag 
{ 
  uint16_t count; 
  uint16_t length; 
  ava_t value[1];    /* first element of the array */ 
} attributeList_t;

typedef struct attributeIdList_tag 
{ 
  uint16_t count; 
  uint16_t length; 
  oidType_t value[1];    /* first element of the array */ 
} attributeIdList_t; 
 
typedef uint32_t oepFloat_t; 
 
typedef uint16_t oepSfloat_t; 
 
typedef uint32_t relativeTime_t; 
 
typedef struct highResRelativeTime_tag 
{ 
  uint8_t value[8]; 
} highResRelativeTime_t; 
 
typedef struct absoluteTimeAdjust_tag 
{ 
  uint8_t value[6]; 
} absoluteTimeAdjust_t; 
 
typedef struct absoluteTime_tag 
{ 
  uint8_t century; 
  uint8_t year; 
  uint8_t month; 
  uint8_t day; 
  uint8_t hour; 
  uint8_t minute; 
  uint8_t second; 
  uint8_t sec_fractions; 
} absoluteTime_t;

typedef uint16_t operationalState_t; 

#define          gOperationalStateDisabled_d             0 
#define          gOperationalStateEnabled_d              1 
#define          gOperationalStateNotAvailable_d         2

 
typedef struct octetString_tag
{ 
  uint16_t length; 
  uint8_t  value[1];    /* first element of the array */ 
} octetString_t; 
 

typedef struct systemModel_tag
{ 
  uint16_t manufacturerLen;
  uint8_t  *manufacturer; 
  uint16_t modelNumberLen; 
  uint8_t  *modelNumber; 
} systemModel_t; 
 
typedef struct prodSpecEntry_tag 
{ 
  uint16_t specType; 

#define    gProdSpecUunspecified_d  0 
#define    gProdSpecSerialNumber_d  1 
#define    gProdSpecPartNumber_d    2 
#define    gProdSpecHwRev_d         3 
#define    gProdSpecSwRev_d         4 
#define    gProdSpecFwRev_d         5 
#define    gProdSpecProtocolRev_d   6 
#define    gProdSpecGmnd_d          7 

  octetString_t prodSpec; 
} prodSpecEntry_t; 
 
typedef struct productionSpec_tag 
{ 
  uint16_t count; 
  uint16_t length; 
  prodSpecEntry_t value[1];  /* first element of the array */ 
} productionSpec_t; 

 
typedef uint16_t powerStatus_t; 

#define          gPowerStatusOnMains_d          0x8000 
#define          gPowerStatusOnBattery_d        0x4000 
#define          gPowerStatusChargingFull_d     0x0080 
#define          gPowerStatusChargingTrickle_d  0x0040 
#define          gPowerStatusChargingOff_d      0x0020 

typedef struct batMeasure_tag 
{ 
  oepFloat_t value; 
  oidType_t unit; 
} batMeasure_t; 
 
typedef uint16_t measurementStatus_t; 

#if ( TRUE == gBigEndian_c )

#define          gMeasureStatInvalid_d            0x8000 
#define          gMeasureStatQuestionable_d       0x4000 
#define          gMeasureStatNotAvailable_d       0x2000 
#define          gMeasureStatCalibrationOngoing_d 0x1000 
#define          gMeasureStatTestData_d           0x0800 
#define          gMeasureStatDemoData_d           0x0400 
#define          gMeasureStatValidatedData_d      0x0080 
#define          gMeasureStatEarlyIndication_d    0x0040 
#define          gMeasureStatOngoing_d            0x0020 
#else
#define          gMeasureStatInvalid_d            0x0080 
#define          gMeasureStatQuestionable_d       0x0040 
#define          gMeasureStatNotAvailable_d       0x0020 
#define          gMeasureStatCalibrationOngoing_d 0x0010 
#define          gMeasureStatTestData_d           0x0008 
#define          gMeasureStatDemoData_d           0x0004 
#define          gMeasureStatValidatedData_d      0x8000 
#define          gMeasureStatEarlyIndication_d    0x4000 
#define          gMeasureStatOngoing_d            0x2000 
#endif
typedef struct nuObsValue_tag
{ 
  oidType_t metricId; 
  measurementStatus_t state; 
  oidType_t unitCode; 
  oepFloat_t value; 
} nuObsValue_t; 
 
typedef struct nuObsValueCmp_tag 
{ 
  uint16_t count; 
  uint16_t length; 
  nuObsValue_t value[1];    /* first element of the array */ 
} nuObsValueCmp_t; 
 
typedef struct sampleType_tag 
{ 
  uint8_t sampleSize; 
  uint8_t significantBits; 
} sampleType_t; 
#if ( TRUE == gBigEndian_c )
#define SAMPLE_TYPE_SIGNIFICANT_BITS_SIGNED_SAMPLES 255 
#else
#define SAMPLE_TYPE_SIGNIFICANT_BITS_SIGNED_SAMPLES 65280 
#endif
typedef uint16_t saFlags_t; 
#if ( TRUE == gBigEndian_c )
#define         gSmoothCurve_d          0x8000 
#define         gDelayedCurve_d         0x4000 
#define         gStaticScale_d          0x2000 
#define         gExtValRange_d          0x1000 
#else
#define         gSmoothCurve_d          0x0080 
#define         gDelayedCurve_d         0x0040 
#define         gStaticScale_d          0x0020 
#define         gExtValRange_d          0x0010 
#endif
typedef struct saSpec_tag 
{ 
  uint16_t arraySize; 
  sampleType_t sampleType; 
  saFlags_t flags; 
} saSpec_t; 

typedef struct scaleRangeSpec8_tag
{ 
  oepFloat_t lowerAbsoluteValue; 
  oepFloat_t upperAbsoluteValue; 
  uint8_t lowerScaledValue; 
  uint8_t upperScaledValue; 
} scaleRangeSpec8_t; 
 
typedef struct scaleRangeSpec16_tag 
{ 
  oepFloat_t lowerAbsoluteValue; 
  oepFloat_t upperAbsoluteValue; 
  uint16_t lowerScaled_value; 
  uint16_t upperScaled_value; 
} scaleRangeSpec16_t; 
 
typedef struct scaleRangeSpec32_tag 
{ 
  oepFloat_t lower_absoluteValue; 
  oepFloat_t upper_absoluteValue; 
  uint32_t lowerScaledValue; 
  uint32_t upperScaledValue; 
} scaleRangeSpec32_t; 

 
typedef struct enumVal_tag 
{ 
  uint16_t choice; 
  uint16_t length; 
#if ( TRUE == gBigEndian_c )
  #define         gObjIdChosen_d             0x0001 
  #define         gTextStringChosen_d        0x0002 
  #define         gBitStrChosen_d            0x0010 
#else
  #define         gObjIdChosen_d             0x0100 
  #define         gTextStringChosen_d        0x0200 
  #define         gBitStrChosen_d            0x1000 
#endif
  union { 
    oidType_t enumObjId; 
    octetString_t enumTextString; 
    uint32_t enumBitStr;
  } u;   
} enumVal_t; 
 
typedef struct enumObsValue_tag
{ 
  oidType_t metricId; 
  measurementStatus_t state; 
  enumVal_t value; 
} enumObsValue_t; 
 
typedef struct attrValMapEntry_tag 
{ 
  oidType_t attributeId; 
  uint16_t attributeLen; 
} attrValMapEntry_t; 
 
typedef struct attrValMap_tag 
{ 
  uint16_t count; 
  uint16_t length; 
  attrValMapEntry_t value[1];  /* first element of the array */
} attrValMap_t;  
  
 
typedef struct handleAttrValMapEntry_tag 
{ 
   oepHandle_t objHandle; 
   attrValMap_t attrValMap; 
} handleAttrValMapEntry_t; 
 
typedef uint16_t confirmMode_t; 
#if ( TRUE == gBigEndian_c )
#define          gUnconfirmed_d                0x0000 
#define          gConfirmed_d                  0x0001 
#else
#define          gUnconfirmed_d                0x0000 
#define          gConfirmed_d                  0x0100 
#endif
 
typedef struct handleAttrValMap_tag 
{ 
  uint16_t count; 
  uint16_t length; 
  handleAttrValMapEntry_t value[1];  /* first element of the array */ 
} handleAttrValMap_t; 

 
typedef uint16_t stoSampleAlg_t; 
#if ( TRUE == gBigEndian_c )
#define          gSampleAlgNos_d                0x0000 
#define          gSampleAlgMovingAverage_d      0x0001 
#define          gSampleAlgRecursive_d          0x0002 
#define          gSampleAlgMinPick_d            0x0003 
#define          gSampleAlgMaxPick_d            0x0004 
#define          gSampleAlgMedian_d             0x0005 
#define          gSampleAlgTrended_d            0x0200 
#define          gSampleAlgNoDownSampling_d     0x0400 
#else
#define          gSampleAlgNos_d                0x0000 
#define          gSampleAlgMovingAverage_d      0x0100 
#define          gSampleAlgRecursive_d          0x0200 
#define          gSampleAlgMinPick_d            0x0300 
#define          gSampleAlgMaxPick_d            0x0400 
#define          gSampleAlgMedian_d             0x0500 
#define          gSampleAlgTrended_d            0x0002 
#define          gSampleAlgNoDownSampling_d     0x0004 
#endif
typedef struct setTimeInvoke_tag 
{ 
  absoluteTime_t dateTime; 
  oepFloat_t accuracy; 
} setTimeInvoke_t; 
 
typedef struct segmIdList_tag 
{ 
  uint16_t count; 
  uint16_t length; 
  instNumber_t value[1];    /* first element of the array */ 
} segmIdList_t; 
 
typedef struct absTimeRange_tag 
{ 
  absoluteTime_t fromTime; 
  absoluteTime_t toTime; 
} absTimeRange_t; 
 
typedef struct segmentInfo_tag 
{ 
  instNumber_t segInstNo; 
  attributeList_t segInfo; 
} segmentInfo_t; 

 
typedef struct segmentInfoList_tag 
{ 
  uint16_t count; 
  uint16_t length; 
  segmentInfo_t value[1];   /* first element of the array */ 
} segmentInfoList_t; 
 
typedef struct segmSelection_tag 
{ 
  uint16_t choice; 
  uint16_t length; 
#if ( TRUE == gBigEndian_c )
#define         gAllSegmentsChosen_d       0x0001 
#define         gSegmIdListChosen_d        0x0002 
#define         gAbsTimeRangeChosen_d      0x0003 
#else
#define         gAllSegmentsChosen_d       0x0100 
#define         gSegmIdListChosen_d        0x0200 
#define         gAbsTimeRangeChosen_d      0x0300 
#endif
  union { 
    uint16_t all_segments; 
    segmIdList_t segm_id_list; 
    absTimeRange_t abs_time_range; 
  } u; 
} segmSelection_t;
 
typedef uint16_t pmStoreCapab_t; 
#if ( TRUE == gBigEndian_c )
#define          gPmStoreVarNoOfSegm_d          0x8000 
#define          gPmStoreEpiSegmEntries_d       0x0800 
#define          gPmStorePeriSegmEntries_d      0x0400 
#define          gPmStoreAbsTimeSelect_d        0x0200 
#define          gPmStoreClearSegmByListSup_d   0x0100 
#define          gPmStoreClearSegmByTimeSup_d   0x0080 
#define          gPmStoreClearSegmRemove_d      0x0040 
#define          gPmStoreMultiPerson_d          0x0008 
#else
#define          gPmStoreVarNoOfSegm_d          0x0080 
#define          gPmStoreEpiSegmEntries_d       0x0008 
#define          gPmStorePeriSegmEntries_d      0x0004 
#define          gPmStoreAbsTimeSelect_d        0x0002 
#define          gPmStoreClearSegmByListSup_d   0x0001 
#define          gPmStoreClearSegmByTimeSup_d   0x8000 
#define          gPmStoreClearSegmRemove_d      0x4000 
#define          gPmStoreMultiPerson_d          0x0800 
#endif
typedef uint16_t segmEntryHeader_t; 
#if ( TRUE == gBigEndian_c )
#define          gSegmElemHdrAbsoluteTime_d           0x8000 
#define          gSegmElemHdrRelativeTime_d           0x4000 
#define          gSegmElemHdrHiresRelativeTime_d      0x2000 
#else
#define          gSegmElemHdrAbsoluteTime_d           0x0080 
#define          gSegmElemHdrRelativeTime_d           0x0040 
#define          gSegmElemHdrHiresRelativeTime_d      0x0020 
#endif
typedef struct segmEntryElem_tag 
{ 
  oidType_t      classId; 
  oepType_t      metricType; 
  oepHandle_t    handle; 
  attrValMap_t   attrValMap; 
} segmEntryElem_t; 
 
typedef struct segmEntryElemList_tag 
{ 
  uint16_t count; 
  uint16_t length; 
  segmEntryElem_t value[1];  /* first element of the array */ 
} segmEntryElemList_t; 
 
typedef struct pmSegmentEntryMap_tag 
{ 
  segmEntryHeader_t segmEntryHeader; 
  segmEntryElemList_t segmEntryElemList; 
} pmSegmentEntryMap_t; 
 
typedef struct segmElemStaticAttrEntry_tag 
{ 
oidType_t        classId; 
oepType_t        metricType; 
attributeList_t  attributeList; 
} segmElemStaticAttrEntry_t; 
 
 
typedef struct pmSegmElemStaticAttrList_tag 
{ 
  uint16_t count; 
  uint16_t length; 
  segmElemStaticAttrEntry_t value[1]; /* first element of the array */ 
} pmSegmElemStaticAttrList_t; 
 
typedef struct trigSegmDataXferReq_tag 
{ 
  instNumber_t     segInstNo; 
} trigSegmDataXferReq_t; 
 
typedef uint16_t trigSegmXferRsp_t; 
#if ( TRUE == gBigEndian_c )
#define gTsxrSucessfull_d           0 
#define gTsxrFailNoSuchSegment_d    1 
#define gTsxrFailSegmTryLater_d     2 
#define gTsxrFailSegmEmpty_d        3 
#define gTsxrFailOther_d          512 
#else
#define gTsxrSucessfull_d           0 
#define gTsxrFailNoSuchSegment_d    256 
#define gTsxrFailSegmTryLater_d     512 
#define gTsxrFailSegmEmpty_d        768 
#define gTsxrFailOther_d          2 
#endif
 
typedef struct trigSegmDataXferRsp_tag 
{ 
  instNumber_t            segInstNo; 
  trigSegmXferRsp_t       trigSegmXferRsp; 
} trigSegmDataXferRsp_t; 
 
typedef uint16_t segmEvtStatus_t; 
#if ( TRUE == gBigEndian_c )
#define gSevtstaFirstEntry_d             0x8000 
#define gSevtstaLastEntry_d              0x4000 
#define gSevtstaAgentAbort_d             0x0800 
#define gSevtstaManagerConfirm_d         0x0080 
#define gSevtstaManagerAbort_d           0x0008 
#else
#define gSevtstaFirstEntry_d             0x0080 
#define gSevtstaLastEntry_d              0x0040 
#define gSevtstaAgentAbort_d             0x0008 
#define gSevtstaManagerConfirm_d         0x8000 
#define gSevtstaManagerAbort_d           0x0800 
#endif
typedef struct segmDataEventDescr_tag       
{ 
  instNumber_t            segmInstance; 
  uint32_t                segmEvtEntryIndex; 
  uint32_t                segmEvtEntryCount; 
  segmEvtStatus_t         segmEvtStatus; 
} segmDataEventDescr_t; 
 
typedef struct segmentDataEvent_tag 
{ 
  segmDataEventDescr_t    segmDataEventDescr; 
  octetString_t           segmDataEventEntries; 
} segmentDataEvent_t; 
 
typedef struct segmentDataResult_tag 
{ 
  segmDataEventDescr_t      segmDataEventDescr; 
} segmentDataResult_t; 
 
typedef uint16_t segmStatType_t; 
#if ( TRUE == gBigEndian_c )
#define gSegmStatTypeMinimum_d   1 
#define gSegmStatTymeMaximum_d   2 
#define gSegmStatTypeAverage_d   3 
#else
#define gSegmStatTypeMinimum_d   256 
#define gSegmStatTymeMaximum_d   512 
#define gSegmStatTypeAverage_d   768 
#endif
 
typedef struct segmentStatisticEntry_tag 
{ 
  segmStatType_t          segmStatType; 
  octetString_t           segmStatEntry; 
} segmentStatisticEntry_t; 
 
typedef struct segmentStatistics_tag 
{ 
  uint16_t count; 
  uint16_t length; 
  segmentStatisticEntry_t value[1];  /* first element of the array */ 
} segmentStatistics_t; 
 
typedef struct observationScan_tag 
{ 
  oepHandle_t objHandle; 
  attributeList_t attributes; 
} observationScan_t; 

typedef oidType_t timeProtocolId_t; 
 
typedef uint32_t associationVersion_t; 
#if ( TRUE == gBigEndian_c )
#define          gAssociationVersion1_d       0x80000000 
#else
#define          gAssociationVersion1_d       0x00000080 
#endif
typedef uint32_t protocolVersion_t; 
#if ( TRUE == gBigEndian_c )
#define          gProtocolVersion1_d          0x80000000 
#else
#define          gProtocolVersion1_d          0x00000080
#endif
typedef uint16_t encodingRules_t; 
#if ( TRUE == gBigEndian_c )
#define          gEncodingMDER_d      0x8000 
#define          gEncodingXER_d       0x4000 
#define          gEncodingPER_d       0x2000 
#else
#define          gEncodingMDER_d      0x0080 
#define          gEncodingXER_d       0x0040 
#define          gEncodingPER_d       0x0020 
#endif
typedef struct uuidIdent_tag 
{ 
  uint8_t value[16]; 
} uuidIdent_t; 
 
typedef uint16_t dataProtoId_t; 
#if ( TRUE == gBigEndian_c )
#define          gDataProtoId_20601_d         20601 
#define          gDataProtoIdExternal_d       65535 
#define          gAssociationVersion_d        0x80000000 
#else
#define          gDataProtoId_20601_d         31056 
#define          gDataProtoIdExternal_d       65535 
#define          gAssociationVersion_d        0x00000080 
#endif

typedef struct dataProto_tag 
{ 
  dataProtoId_t dataProtoId; 
  any_t dataProtoInfo; 
} dataProto_t; 
 
typedef struct dataProtoList_tag 
{ 
  uint16_t count; 
  uint16_t length; 
  dataProto_t value[1];  /* first element of the array */ 
} dataProtoList_t; 

typedef struct aarqApdu_tag
{ 
  associationVersion_t assocVersion; 
  dataProtoList_t dataProtoList; 
} aarqApdu_t; 
 
typedef uint16_t associateResult_t; 
#if ( TRUE == gBigEndian_c )
#define          gAccepted_d                          0 
#define          gRejectedPermanent_d                 1 
#define          gRejectedTransient_d                 2 
#define          gAcceptedUnknownConfig_d             3 
#define          gRejectedNoCommonProtocol_d          4 
#define          gRejectedNoCommonParameter_d         5 
#define          gRejectedUnknown_d                   6 
#define          gRejectedUnauthorized_d              7 
#define          gRejectedUnsupportedAssocVersion_d   8 
#else
#define          gAccepted_d                          0 
#define          gRejectedPermanent_d                 256 
#define          gRejectedTransient_d                 512 
#define          gAcceptedUnknownConfig_d             768 
#define          gRejectedNoCommonProtocol_d          1024 
#define          gRejectedNoCommonParameter_d         1280 
#define          gRejectedUnknown_d                   1536 
#define          gRejectedUnauthorized_d              1792 
#define          gRejectedUnsupportedAssocVersion_d   2048 
#endif
typedef struct aareApdu_tag 
{ 
  associateResult_t result; 
  dataProto_t selectedDataProto; 
} aareApdu_t; 
 
typedef uint16_t releaseRequestReason_t; 
#if ( TRUE == gBigEndian_c )
#define          gReleaseRequestReasonNormal_d        0 
#else
#define          gReleaseRequestReasonNormal_d        0 
#endif
typedef struct rlrqApdu_tag 
{ 
  releaseRequestReason_t reason; 
} rlrqApdu_t; 
 
typedef uint16_t releaseResponseReason_t; 
#if ( TRUE == gBigEndian_c )
#define          gReleaseResponseReasonNormal_d        0 
#else
#define          gReleaseResponseReasonNormal_d        0 
#endif
typedef struct rlreApdu_tag 
{ 
  releaseResponseReason_t reason; 
} rlreApdu_t; 
 
typedef uint16_t abortReason_t; 
#if ( TRUE == gBigEndian_c )
#define          gAbortReasonUndefined_d               0 
#define          gAbortReasonBufferOverflow_d          1 
#define          gAbortReasonResponseTimeout_d         2 
#define          gAbortReasonConfigurationTimeout_d    3 
#else
#define          gAbortReasonUndefined_d               0 
#define          gAbortReasonBufferOverflow_d          256 
#define          gAbortReasonResponseTimeout_d         512 
#define          gAbortReasonConfigurationTimeout_d    768 
#endif
 
typedef struct abrtApdu_tag 
{ 
  abortReason_t reason; 
} abrtApdu_t; 
 
typedef octetString_t prstApdu_t; 
 
typedef uint16_t invokeId_t; 
 
typedef struct eventReportArgumentSimple_tag 
{ 
  oepHandle_t objHandle; 
  relativeTime_t eventTime; 
  oidType_t eventType; 
  any_t eventInfo; 
} eventReportArgumentSimple_t; 
 
typedef struct getArgumentSimple_tag
{ 
  oepHandle_t obj_handle; 
  attributeIdList_t attributeIdList; 
} getArgumentSimple_t; 
 
typedef uint16_t modifyOperator_t; 
#if ( TRUE == gBigEndian_c )
#define          gReplace_d                0
#define          gAddValues_d              1
#define          gRemoveValues_d           2
#define          gSetToDefault_d           3
#else
#define          gReplace_d                0
#define          gAddValues_d              256
#define          gRemoveValues_d           512
#define          gSetToDefault_d           768
#endif
typedef struct attributeModEntry_tag 
{ 
  modifyOperator_t modifyOperator; 
  ava_t attribute; 
} attributeModEntry_t; 
 
typedef struct modificationList_tag 
{ 
  uint16_t count; 
  uint16_t length; 
  attributeModEntry_t value[1];  /* first element of the array */ 
} modificationList_t; 
 
typedef struct setArgumentSimple_tag 
{ 
  oepHandle_t objHandle; 
  modificationList_t modificationList; 
} setArgumentSimple_t; 
 
typedef struct actionArgumentSimple_tag 
{ 
  oepHandle_t objHandle; 
  oidType_t actionType; 
  any_t actionInfoArgs; 
} actionArgumentSimple_t; 
 
typedef struct eventReportResultSimple_tag 
{ 
  oepHandle_t objHandle; 
  relativeTime_t currentTime; 
  oidType_t eventType; 
  any_t eventReplyInfo; 
} eventReportResultSimple_t; 
  
typedef struct getResultSimple_tag 
{ 
  oepHandle_t objHandle; 
  attributeList_t attributeList; 
} getResultSimple_t; 
 
typedef struct typeVer_tag 
{ 
  oidType_t type; 
  uint16_t version; 
} typeVer_t; 
 
typedef struct typeVerList_tag 
{ 
  uint16_t count; 
  uint16_t length; 
  typeVer_t *value;    /* first element of the array */ 
} typeVerList_t; 
 
typedef struct setResultSimple_tag 
{ 
  oepHandle_t obj_handle; 
  attributeList_t attribute_list; 
} setResultSimple_t;

 
typedef struct actionResultSimple_tag 
{ 
  oepHandle_t objHandle; 
  oidType_t actionType; 
  any_t actionInfoArgs; 
} actionResultSimple_t; 
 
typedef uint16_t oepError_t; 
#if ( TRUE == gBigEndian_c )
#define          gErrorNoSuchObjectInstance_d  1 
#define          gErrorAccessDenied_d          2 
#define          gErrorNoSuchAction_d          9 
#define          gErrorInvalidObjectInstance_d 17 
#define          gErrorProtocolValidation_d    23 
#define          gErrorNotAllowedByObject_d    24 
#define          gErrorActionTimedOut_d        25 
#define          gErrorActionAborted_d         26 
#else
#define          gErrorNoSuchObjectInstance_d  256 
#define          gErrorAccessDenied_d          512 
#define          gErrorNoSuchAction_d          2304 
#define          gErrorInvalidObjectInstance_d 4352 
#define          gErrorProtocolValidation_d    5888 
#define          gErrorNotAllowedByObject_d    6144 
#define          gErrorActionTimedOut_d        6400 
#define          gErrorActionAborted_d         6656 
#endif
 
typedef struct errorResult_tag 
{ 
  oepError_t errorValue; 
  any_t parameter; 
} errorResult_t; 
 
typedef uint16_t rorjProblem_t; 
#if ( TRUE == gBigEndian_c )
#define          gRorjUnrecognizedApdu_d      0 
#define          gRorjBadlyStructuredApdu_d   2 
#define          gRorjUnrecognizedOperation_d 101 
#define          gRorjResourceLimitation_d    103 
#define          gRorjUnexpectedError_d       303 
#else
#define          gRorjUnrecognizedApdu_d      0 
#define          gRorjBadlyStructuredApdu_d   512 
#define          gRorjUnrecognizedOperation_d 25856 
#define          gRorjResourceLimitation_d    26368 
#define          gRorjUnexpectedError_d       12033 
#endif
typedef struct rejectResult_tag 
{ 
  rorjProblem_t problem; 
} rejectResult_t; 

 
typedef struct dataApdu_tag 
{ 
  invokeId_t invokeId; 
  struct 
  { 
    uint16_t choice; 
    uint16_t length; 
#if ( TRUE == gBigEndian_c )
#define         gRoivCmipEventReport_d            0x0100 
#define         gRoivCmipConfirmedEventReport_d   0x0101 
#define         gRoivCmipGet_d                    0x0103 
#define         gRoivCmipSet_d                    0x0104 
#define         gRoivCmipConfirmedSet_d           0x0105 
#define         gRoivCmipAction_d                 0x0106 
#define         gRoivCmipConfirmedAction_d        0x0107 
#define         gRorsCmipConfirmedEventReport_d   0x0201 
#define         gRorsCmipGet_d                    0x0203 
#define         gRorsCmipConfirmedSet_d           0x0205 
#define         gRorsCmipConfirmedAction_d        0x0207 
#define         gRoer_d                           0x0300 
#define         gRorj_d                           0x0400 
#else
#define         gRoivCmipEventReport_d            0x0001 
#define         gRoivCmipConfirmedEventReport_d   0x0101 
#define         gRoivCmipGet_d                    0x0301 
#define         gRoivCmipSet_d                    0x0401 
#define         gRoivCmipConfirmedSet_d           0x0501 
#define         gRoivCmipAction_d                 0x0601 
#define         gRoivCmipConfirmedAction_d        0x0701 
#define         gRorsCmipConfirmedEventReport_d   0x0102 
#define         gRorsCmipGet_d                    0x0302 
#define         gRorsCmipConfirmedSet_d           0x0502 
#define         gRorsCmipConfirmedAction_d        0x0702 
#define         gRoer_d                           0x0003 
#define         gRorj_d                           0x0004 
#endif
    union { 
      eventReportArgumentSimple_t roivCmipEventReport; 
      eventReportArgumentSimple_t roivCmipConfirmedEventReport; 
      getArgumentSimple_t roivCmipGet; 
      setArgumentSimple_t roivCmipSet; 
      setArgumentSimple_t roivCmipConfirmedSet; 
      actionArgumentSimple_t roivCmipAction; 
      actionArgumentSimple_t roivCmipConfirmedAction; 
      eventReportResultSimple_t rorsCmipConfirmedEventReport; 
      getResultSimple_t rorsCmipGet; 
      setResultSimple_t rorsCmipConfirmedSet; 
      actionResultSimple_t rorsCmipConfirmedAction; 
      errorResult_t roer; 
      rejectResult_t rorj; 
    } u; 
  } choice; 
} dataApdu_t; 
 
 
 
typedef uint32_t nomenclatureVersion_t; 
#if ( TRUE == gBigEndian_c )
#define          gNomVersion1_d            0x80000000 
#else
#define          gNomVersion1_d            0x00000080
#endif
typedef uint32_t functionalUnits_t; 
#if ( TRUE == gBigEndian_c )
#define          gFunUnitsUnidirectional_d  0x80000000 
#define          gFunUnitsHaveTestCap_d     0x40000000 
#define          gFunUnitsCreateTestAssoc_d 0x20000000 
#else
#define          gFunUnitsUnidirectional_d  0x00000080 
#define          gFunUnitsHaveTestCap_d     0x00000040
#define          gFunUnitsCreateTestAssoc_d 0x00000020 
#endif
typedef uint32_t systemType_t; 
#if ( TRUE == gBigEndian_c )
#define          gSysTypeManager_d        0x80000000 
#define          gSysTypeAgent_d          0x00800000 
#else
#define          gSysTypeManager_d        0x00000080
#define          gSysTypeAgent_d          0x00008000
#endif
typedef uint16_t configId_t; 
#if ( TRUE == gBigEndian_c )
#define          gManagerConfigResponse_d             0x0000 
#define          gStandardConfigStart_d               0x0001 
#define          gStandardConfigEnd_d                 0x3FFF 
#define          gExtendedConfigStart_d               0x4000 
#define          gExtendedConfigEnd_d                 0x7FFF 
#define          gReservedStart_d                     0x8000 
#define          gReservedEnd_d                       0xFFFF 
#else
#define          gManagerConfigResponse_d             0x0000 
#define          gStandardConfigStart_d               0x0100 
#define          gStandardConfigEnd_d                 0xff3f 
#define          gExtendedConfigStart_d               0x0040 
#define          gExtendedConfigEnd_d                 0xff7f 
#define          gReservedStart_d                     0x0080 
#define          gReservedEnd_d                       0xffff 
#endif
typedef uint16_t dataReqModeFlags_t; 
#if ( TRUE == gBigEndian_c )
#define          gDataReqFlagStop_d                 0x8000 
#define          gDataReqFlagScopeAll_d             0x0800 
#define          gDataReqFlagScopeClass_d           0x0400 
#define          gDataReqFlagScopeHandle_d          0x0200 
#define          gDataReqFlagSingleRsp_d            0x0080 
#define          gDataReqFlagTimePeriod_d           0x0040 
#define          gDataReqFlagTimeNoLimit_d          0x0020 
#define          gDataReqFlagPersonId_d             0x0010 
#define          gDataReqFlagInitAgent_d            0x0001 
#else
#define          gDataReqFlagStop_d                 0x0080 
#define          gDataReqFlagScopeAll_d             0x0008 
#define          gDataReqFlagScopeClass_d           0x0004 
#define          gDataReqFlagScopeHandle_d          0x0002 
#define          gDataReqFlagSingleRsp_d            0x8000 
#define          gDataReqFlagTimePeriod_d           0x4000 
#define          gDataReqFlagTimeNoLimit_d          0x2000 
#define          gDataReqFlagPersonId_d             0x1000 
#define          gDataReqFlagInitAgent_d            0x0100 
#endif

typedef struct dataReqModeCapab_tag 
{ 
  dataReqModeFlags_t dataReqModeFlags; 
  uint8_t dataReqInitAgentCount; 
  uint8_t dataReqInitManagerCount; 
} dataReqModeCapab_t; 
 
 
typedef struct phdAssociationInformation_tag 
{ 
  protocolVersion_t protocolVersion; 
  encodingRules_t encodingRules; 
  nomenclatureVersion_t nomenclatureVersion; 
  functionalUnits_t functionalUnits; 
  systemType_t systemType; 
  octetString_t systemId; 
  uint16_t devConfigId; 
  dataReqModeCapab_t dataReqModeCapab; 
  attributeList_t optionList; 
} phdAssociationInformation_t; 
 
typedef struct manufSpecAssociationInformation_tag 
{ 
  uuidIdent_t dataProtoIdExt; 
  any_t dataProtoInfoExt; 
} manufSpecAssociationInformation_t;  

 
typedef uint16_t mdsTimeCapState_t; 
#if ( TRUE == gBigEndian_c )
#define          gMdsTimeCapabRealTimeClock_d              0x8000 
#define          gMdsTimeCapabSetClock_d                   0x4000 
#define          gMdsTimeCapabRelativeClock_d              0x2000 
#define          gMdsTimeCapabHighResRelativeTime_d        0x1000 
#define          gMdsTimeCapabSyncAbsTime_d                0x0800 
#define          gMdsTimeCapabSyncRelTime_d                0x0400 
#define          gMdsTimeCapabSyncHighResRelativeTime_d    0x0200 
#define          gMdsTimeStateAbsTimeSynced_d              0x0080 
#define          gMdsTimeStateRelTimeSynced_d              0x0040 
#define          gMdsTimeStateHighResRelTimeSynced_d       0x0020 
#define          gMdsTimeMgrSetTime_d                      0x0010 
#else
#define          gMdsTimeCapabRealTimeClock_d              0x0080 
#define          gMdsTimeCapabSetClock_d                   0x0040 
#define          gMdsTimeCapabRelativeClock_d              0x0020 
#define          gMdsTimeCapabHighResRelativeTime_d        0x0010 
#define          gMdsTimeCapabSyncAbsTime_d                0x0008 
#define          gMdsTimeCapabSyncRelTime_d                0x0004 
#define          gMdsTimeCapabSyncHighResRelativeTime_d    0x0002 
#define          gMdsTimeStateAbsTimeSynced_d              0x8000 
#define          gMdsTimeStateRelTimeSynced_d              0x4000 
#define          gMdsTimeStateHighResRelTimeSynced_d       0x2000 
#define          gMdsTimeMgrSetTime_d                      0x1000 
#endif
typedef struct mdsTimeInfo_tag 
{ 
  mdsTimeCapState_t mdsTimeCapState;  
  timeProtocolId_t timesyncProtocol; 
  relativeTime_t timeSyncAccuracy; 
  uint16_t timeResolutionAbsTime; 
  uint16_t timeResolutionRelTime; 
  uint32_t timeResolutionHighResTime; 
} mdsTimeInfo_t; 
 
typedef octetString_t enumPrintableString_t; 
 
typedef uint16_t personId_t; 
#if ( TRUE == gBigEndian_c )
#define          gUnknownPersonId_d    0xFFFF 
#else
#define          gUnknownPersonId_d    0xffff 
#endif
typedef uint16_t metricSpecSmall_t; 
//#if ( TRUE == gBigEndian_c )
#define          gMssAvailIntermittent_d         0x8000 
#define          gMssAvailStoreddata_d           0x4000 
#define          gMssUpdAperiodic_d              0x2000 
#define          gMssMsmtAperiodic_d             0x1000 
#define          gMssMsmtPhysEvId_d              0x0800 
#define          gMssMsmtBtbMetric_d             0x0400 
#define          gMssAccManagerInitiated_d       0x0080 
#define          gMssAccAgentInitiated_d         0x0040 
#define          gMssCatManual_d                 0x0008 
#define          gMssCatSetting_d                0x0004 
#define          gMssCatCaculation_d             0x0002 
/*
#else
#define          gMssAvailIntermittent_d         0x0080 
#define          gMssAvailStoreddata_d           0x0040 
#define          gMssUpdAperiodic_d              0x0020 
#define          gMssMsmtAperiodic_d             0x0010 
#define          gMssMsmtPhysEvId_d              0x0008 
#define          gMssMsmtBtbMetric_d             0x0004 
#define          gMssAccManagerInitiated_d       0x8000 
#define          gMssAccAgentInitiated_d         0x4000 
#define          gMssCatManual_d                 0x0800 
#define          gMssCatSetting_d                0x0400 
#define          gMssCatCaculation_d             0x0200 
#endif
*/
typedef struct metricStructureSmall_tag 
{ 
   uint8_t msStruct; 
#if ( TRUE == gBigEndian_c )
#define gMsStructSimple_d       0 
#define gMsStructCompound_d     1 
#define gMsStructReserved_d     2 
#define gMsStructCompoundFix_d  3 
#else
#define gMsStructSimple_d       0 
#define gMsStructCompound_d     256 
#define gMsStructReserved_d     512 
#define gMsStructCompoundFix_d  768 
#endif
} metricStructureSmall_t; 
 

typedef struct metricIdList_tag
{  
  uint16_t count;
  uint16_t length;
  oidType_t *value;  
} metricIdList_t;

 
typedef struct supplementalTypeList_tag 
{ 
  uint16_t count; 
  uint16_t length; 
  oepType_t value[1];    /* first element of the array */ 
} supplementalTypeList_t; 


typedef struct observationScanList_tag 
{ 
  uint16_t count; 
  uint16_t length; 
  observationScan_t value[1];  /* first element of the array */ 
} observationScanList_t; 

typedef struct scanReportPerVar_tag 
{ 
  uint16_t personId; 
  observationScanList_t obsScanVar; 
} scanReportPerVar_t; 
 
typedef struct scanReportPerVarList_tag 
{ 
  uint16_t count; 
  uint16_t length; 
  scanReportPerVar_t value[1];  /* first element of the array */ 
} scanReportPerVarList_t; 
 
typedef uint16_t dataReqId_t; 
#if ( TRUE == gBigEndian_c )
#define gDataReqIdManagerInitiatedMin_d 0x0000 
#define gDataReqIdManagerInitiatedMax_d 0xEFFF 
#define gDataReqIdAgentInitiated_d      0xF000 
#else
#define gDataReqIdManagerInitiatedMin_d 0x0000 
#define gDataReqIdManagerInitiatedMax_d 0xffef 
#define gDataReqIdAgentInitiated_d      0x00f0 
#endif
typedef struct scanReportInfoMPVar_tag 
{ 
  dataReqId_t dataReqId; 
  uint16_t scanReportNo; 
  scanReportPerVarList_t scanPerVar; 
} scanReportInfoMPVar_t; 
 
typedef struct observationScanFixed_tag 
{ 
  oepHandle_t objHandle; 
  octetString_t obsValData; 
} observationScanFixed_t; 
 
typedef struct observationScanFixedList_tag 
{ 
  uint16_t count; 
  uint16_t length; 
  observationScanFixed_t value[1];  /* first element of the array */ 
} observationScanFixedList_t; 
 
typedef struct scanReportPerFixed_tag 
{ 
  uint16_t personId; 
  observationScanFixedList_t obsScanFix; 
} scanReportPerFixed_t;

 typedef struct scanReportPerFixedList_t 
{ 
  uint16_t count; 
  uint16_t length; 
  scanReportPerFixed_t value[1];  /* first element of the array */ 
} scanReportPerFixedList_t; 
 
typedef struct scanReportInfoMPFixed_tag 
{ 
  dataReqId_t dataReqId; 
  uint16_t scanReportNo; 
  scanReportPerFixedList_t scanPerFixed; 
} scanReportInfoMPFixed_t; 

 
typedef struct scanReportInfoVar_tag 
{ 
  dataReqId_t dataReqId; 
  uint16_t scanReportNo; 
  observationScanList_t obsScanVar; 
} scanReportInfoVar_t; 
 
typedef struct scanReportInfoFixed_tag 
{ 
  dataReqId_t dataReqId; 
  uint16_t scanReportNo; 
  observationScanFixedList_t obsScanFixed; 
} scanReportInfoFixed_t; 
 
typedef octetString_t observationScanGrouped_t; 
 
typedef struct scanReportInfoGroupedList_tag 
{ 
  uint16_t count; 
  uint16_t length; 
  observationScanGrouped_t value[1];  /* first element of the array */ 
} scanReportInfoGroupedList_t; 
 
typedef struct scanReportInfoGrouped_t 
{ 
  dataReqId_t dataReqId; 
  uint16_t scanReportNo; 
  scanReportInfoGroupedList_t obsScanGrouped; 
} scanReportInfoGrouped_t; 
 
typedef struct scanReportPerGrouped_tag 
{ 
  personId_t personId; 
  observationScanGrouped_t obsScanGrouped; 
} scanReportPerGrouped_t; 
 
typedef struct scanReportPerGroupedList_tag 
{ 
  uint16_t count; 
  uint16_t length; 
  scanReportPerGrouped_t value[1];  /* first element of the array */ 
} scanReportPerGroupedList_t; 

 
typedef struct scanReportInfoMPGrouped_tag 
{ 
  dataReqId_t dataReqId; 
  uint16_t scanReportNo; 
  scanReportPerGroupedList_t scanPerGrouped; 
} scanReportInfoMPgrouped_t; 
 
typedef struct configObject_tag 
{ 
  oidType_t objClass; 
  oepHandle_t objHandle; 
  attributeList_t attributes; 
} configObject_t; 

 
typedef struct configObjectList_tag 
{ 
  uint16_t count; 
  uint16_t length; 
  configObject_t value[1];          /* first element of the array */ 
} configObjectList_t; 
 
typedef struct configReport_tag 
{ 
  configId_t configReportId; 
  configObjectList_t configObjList; 
} configReport_t; 
 
typedef uint16_t configResult_t; 
#if ( TRUE == gBigEndian_c )
#define         gConfigAccepted_d            0x0000 
#define         gConfigUnsupported_d         0x0001 
#define         gConfigUnknown_d             0x0002 
#else
#define         gConfigAccepted_d            0x0000 
#define         gConfigUnsupported_d         0x0100 
#define         gConfigUnknown_d             0x0200 
#endif
typedef struct configReportRsp_tag 
{ 
  configId_t configReportId; 
  configResult_t configResult; 
} configReportRsp_t; 
 
typedef uint16_t dataReqMode_t; 
#if ( TRUE == gBigEndian_c )
#define          gDataReqStartStop_d            0x8000 
#define          gDataReqContinuation_d         0x4000 
#define          gDataReqScopeAll_d             0x0800 
#define          gDataReqScopeType_d            0x0400 
#define          gDataReqScopeHandle_d          0x0200 
#define          gDataReqModeSingleRsp_d        0x0080 
#define          gDataReqModeTimePeriod_d       0x0040 
#define          gDataReqModeTimeNoLimit_d      0x0020 
#define          gDataReqModeDataReqPersonId_d  0x0008 
#else
#define          gDataReqStartStop_d            0x0080 
#define          gDataReqContinuation_d         0x0040 
#define          gDataReqScopeAll_d             0x0008 
#define          gDataReqScopeType_d            0x0004 
#define          gDataReqScopeHandle_d          0x0002 
#define          gDataReqModeSingleRsp_d        0x8000 
#define          gDataReqModeTimePeriod_d       0x4000 
#define          gDataReqModeTimeNoLimit_d      0x2000 
#define          gDataReqModeDataReqPersonId_d  0x0800 
#endif
typedef struct handleList_tag 
{ 
  uint16_t count; 
  uint16_t length; 
  oepHandle_t value[1];    /* first element of the array */ 
} handleList_t; 
 
 
typedef struct dataRequest_tag 
{ 
  dataReqId_t id; 
  dataReqMode_t mode; 
  relativeTime_t time; 
  personId_t personId; 
  oidType_t dataReqClass; 
  handleList_t objHandleList; 
} dataRequest_t; 
 
typedef uint16_t dataReqResult_t; 
#if ( TRUE == gBigEndian_c )
#define          gDataReqResNoError_d                       0 
#define          gDataReqResUnspecificError_d               1 
#define          gDataReqResNoStopSupport_d                 2 
#define          gDataReqResNoScopeAllSupport_d             3 
#define          gDataReqResNoScopeClassSupport_d           4 
#define          gDataReqResNoScopeHandleSupport_d          5 
#define          gDataReqResNoModeSingleRspSupport_d        6 
#define          gDataReqResNoModeTimePeriodSupport_d       7 
#define          gDataReqResNoModeTimeNoLimitSupport_d      8 
#define          gDataReqResNoPersonIdSupport_d             9 
#define          gDataReqResUnknownPersonId_d              11 
#define          gDataReqResUnknownClass_d                 12 
#define          gDataReqResUnknoenHandle_d                13 
#define          gDataReqResUnsupportedScope_d             14 
#define          gDataReqResUnsupportedMode_d              15 
#define          gDataReqResInitManagerOverflow_d          16 
#define          gDataReqResContinuationNotSupported_d     17 
#define          gDataReqResInvalidReqId_d                 18 
#else
#define          gDataReqResNoError_d                       0 
#define          gDataReqResUnspecificError_d               256 
#define          gDataReqResNoStopSupport_d                 512 
#define          gDataReqResNoScopeAllSupport_d             768 
#define          gDataReqResNoScopeClassSupport_d           1024 
#define          gDataReqResNoScopeHandleSupport_d          1280 
#define          gDataReqResNoModeSingleRspSupport_d        1536 
#define          gDataReqResNoModeTimePeriodSupport_d       1792 
#define          gDataReqResNoModeTimeNoLimitSupport_d      2048 
#define          gDataReqResNoPersonIdSupport_d             2304 
#define          gDataReqResUnknownPersonId_d              2816 
#define          gDataReqResUnknownClass_d                 3072 
#define          gDataReqResUnknoenHandle_d                3328 
#define          gDataReqResUnsupportedScope_d             3584 
#define          gDataReqResUnsupportedMode_d              3840 
#define          gDataReqResInitManagerOverflow_d          4096 
#define          gDataReqResContinuationNotSupported_d     4352 
#define          gDataReqResInvalidReqId_d                 4608 
#endif
typedef struct dataResponse_tag 
{ 
  relativeTime_t relTimeStamp; 
  dataReqResult_t dataReqResult; 
  oidType_t eventType; 
  any_t eventInfo; 
} dataResponse_t; 

 
typedef oepFloat_t simpleNuObsValue_t; 
 
typedef struct simpleNuObsValueCmp_tag 
{ 
  uint16_t count; 
  uint16_t length; 
  simpleNuObsValue_t value[1];  /* first element of the array */ 
} simpleNuObsValueCmp_t; 
 
 
typedef struct basicValList_tag 
{ 
  uint16_t count; 
  uint16_t length; 
  oepSfloat_t *value;
} basicValList_t;


typedef oepSfloat_t basicNuObsValue_t; 
typedef basicValList_t basicNuObsValueCmp_t; 






  



/***********************************************************
11073-2061 speciffic
***********************************************************/
typedef struct aarqOepApdu_tag
{ 
  associationVersion_t assocVersion;  /* This must always be 0x80000000 */
  uint16_t count;       /* This must be initializat to 1 */
  uint16_t length;      /* This must be initialize to the size of the reminder of the structure */
  dataProtoId_t dataProtoId;  /* This must be initialized to gDataProtoId_20601_d = 20601 = 0x5079 */
  uint16_t lengthAssocRqInfo; /* This must be initialize to the size of the reminder of the structure - associationInformation*/ 
  /* associationInformation_t  - we can add a structure member of this type or leave it as it is */
  protocolVersion_t protocolVersion; 
  encodingRules_t encodingRules; 
  nomenclatureVersion_t nomenclatureVersion; 
  functionalUnits_t functionalUnits; 
  systemType_t systemType;
  uint16_t  systemIdLen;
  uint8_t   systemId[8]; 
  configId_t devConfigId; 
  dataReqModeCapab_t dataReqModeCapab; 
  attributeList_t optionList; 
} aarqOepApdu_t; 

typedef struct aareOepApdu_tag 
{ 
  associateResult_t result; 
  dataProtoId_t dataProtoId;  /* This must be initialized to gDataProtoId_20601_d = 20601 = 0x5079 */
  uint16_t lengthAssocReInfo; /* This must be initialize to the size of the reminder of the structure - associationInformation*/ 
  /* associationInformation_t  - we can add a structure member of this type or leave it as it is */
  protocolVersion_t protocolVersion; 
  encodingRules_t encodingRules; 
  nomenclatureVersion_t nomenclatureVersion; 
  functionalUnits_t functionalUnits; 
  systemType_t systemType; 
  uint16_t  systemIdLen;
  uint8_t   systemId[8]; 
  configId_t devConfigId; 
  dataReqModeCapab_t dataReqModeCapab; 
  attributeList_t optionList; 
} aareOepApdu_t; 

typedef struct prstOepCfgRepApdu_tag
{
  uint16_t length;          /* Length of the reminder of the structure */
  invokeId_t invokeId; 
  uint16_t choice;          /* Must be set to gRoivCmipConfirmedEventReport_d = 0x0101 */
  uint16_t lengthDataApdu;  /* Length of the reminder of the structure */ 
  oepHandle_t objHandle;    /* Must be set to 0 - MSD object */
  relativeTime_t eventTime; 
  oidType_t eventType;      /* Must be set to MDC_NOTI_CONFIG = 0x0D1C */ 
  uint16_t lengthCfgReport; /* Length of the actual config report */
  configReport_t cfgReport; /* The actual configuration report - a list of objects and atributes */ 
} prstOepCfgRepApdu_t;

typedef struct prstOepCfgRspApdu_tag
{
  uint16_t length;          /* Length of the reminder of the structure */
  invokeId_t invokeId;      /* Must match the invokeId received in the configuration report */
  uint16_t choice;          /* Must be set to gRorsCmipConfirmedEventReport_d = 0x0201 */
  uint16_t lengthDataApdu;  /* Length of the reminder of the structure */ 
  oepHandle_t objHandle;    /* Must be set to 0 - MSD object */
  relativeTime_t currentTime; 
  oidType_t eventType;        /* Must be set to MDC_NOTI_CONFIG = 0x0D1C */ 
  uint16_t lengthReply;       /* Length of the actual reply */
  configId_t configReportId;  /* Must match the configReportId received */
  configResult_t configResult;/* Actual result */ 
} prstOepCfgRspApdu_t;

typedef struct prstOepDataReport_tag
{
  uint16_t length;          // Length of the reminder of the structure
  invokeId_t invokeId;      
  uint16_t choice;          // Must be set to gRoivCmipConfirmedEventReport_d = 0x0101
  uint16_t lengthDataApdu;  // Length of the reminder of the structure
  oepHandle_t objHandle;    // Must be set to 0 - MSD object
  relativeTime_t eventTime; 
  oidType_t eventType;      // Must be set to MDC_NOTI_SCAN_REPORT_FIXED = 0x0D1D
  uint16_t lengthDataReport;// Length of the actual data report
  scanReportInfoFixed_t scanReport;  
} prstOepDataReport_t;

typedef struct prstOepDataRspApdu_tag
{
  uint16_t length;          // Length of the reminder of the structure
  invokeId_t invokeId;      // Must match the invokeId received in the data report
  uint16_t choice;          // Must be set to gRorsCmipConfirmedEventReport_d = 0x0201
  uint16_t lengthDataApdu;  // Length of the reminder of the structure
  oepHandle_t objHandle;    // Must be set to 0 - MSD object
  relativeTime_t currentTime; 
  oidType_t eventType;        // Must be set to  MDC_NOTI_SCAN_REPORT_FIXED = 0x0D1D
  uint16_t lengthReply;       // Length of the actual reply - 0
} prstOepDataRspApdu_t;

typedef struct prstOepGetFrame_tag
{
  uint16_t length;          // Length of the reminder of the structure
  invokeId_t invokeId;      // Must match the invokeId received in the data report
  uint16_t choice;          // Must be set to gRoivCmipGet_d (0x0103) or gRorsCmipGet_d (0x0203) 
  uint16_t lengthDataApdu;  // Length of the reminder of the structure
  oepHandle_t objHandle;    // Must be set to handle of the object we want to get
  attributeList_t attrList; // List of attributes to get in the request or sent in the response frame
} prstOepGetFrame_t;

typedef struct apdu_tag 
{ 
  uint16_t choice; 
  uint16_t length; 
#if ( TRUE == gBigEndian_c )
#define         gAarq_d             0xE200 
#define         gAare_d             0xE300 
#define         gRlrq_d             0xE400 
#define         gRlre_d             0xE500 
#define         gAbrt_d             0xE600 
#define         gPrst_d             0xE700 
#else
#define         gAarq_d             0x00e2 
#define         gAare_d             0x00e3 
#define         gRlrq_d             0x00e4 
#define         gRlre_d             0x00e5 
#define         gAbrt_d             0x00e6 
#define         gPrst_d             0x00e7 
#endif
  union { 
    aarqApdu_t      aarq; 
    aareApdu_t      aare; 
    rlrqApdu_t      rlrq; 
    rlreApdu_t      rlre; 
    abrtApdu_t      abrt; 
    prstApdu_t      prst; 
    /* 20601 speciffic structures*/
    aarqOepApdu_t aarqOep; 
    aareOepApdu_t aareOep; 
    prstOepCfgRepApdu_t   cfgReport;
    prstOepCfgRspApdu_t   cfgResponse;
    prstOepDataReport_t   dataReport;
    prstOepDataRspApdu_t  dataResponse;
    prstOepGetFrame_t     getFrame;
  } u; 
} apdu_t; 


 
#ifdef __IAR_SYSTEMS_ICC__
#pragma pack()
#endif

/******************************************************************************
*******************************************************************************
* Public memory declarations
*******************************************************************************
******************************************************************************/


/******************************************************************************
*******************************************************************************
* Public functions
*******************************************************************************
******************************************************************************/


#endif /*_OEP_TYPES_INTERFACE_H */
