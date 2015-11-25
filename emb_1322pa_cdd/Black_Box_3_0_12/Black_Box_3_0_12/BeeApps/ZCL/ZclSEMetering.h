#include "EmbeddedTypes.h"
#include "AfApsInterface.h"
#include "ZCL.h"
#include "SEProfile.h"

/*Meter cluster definitions */
#ifndef gSmplMetFormat_c
#define gSmplMetFormat_c  0x28
#endif

#ifndef gProfIntrvPeriod_c
#define gProfIntrvPeriod_c              7  
#endif

#ifndef gMaxNumberOfPeriodsDelivered_c
#define gMaxNumberOfPeriodsDelivered_c  5
#endif

#ifndef gASL_ZclSmplMet_MaxFastPollInterval_d
#define gASL_ZclSmplMet_MaxFastPollInterval_d 900//15min
#endif
#define gUpdateConsumption_c  10

#define gLastValue_c 2
#define gCurrentValue_c 0

#if(gProfIntrvPeriod_c == gZclSEProfIntrvPeriod_Daily_c)
#define gTimerValue_c 1440
#elif(gProfIntrvPeriod_c == gZclSEProfIntrvPeriod_60mins_c)
#define gTimerValue_c 60
#elif(gProfIntrvPeriod_c == gZclSEProfIntrvPeriod_30mins_c)  
#define gTimerValue_c 30
#elif(gProfIntrvPeriod_c == gZclSEProfIntrvPeriod_15mins_c)    
#define gTimerValue_c 15
#elif(gProfIntrvPeriod_c == gZclSEProfIntrvPeriod_10mins_c)      
#define gTimerValue_c 10
#elif(gProfIntrvPeriod_c == gZclSEProfIntrvPeriod_7dot5mins_c)
#define gTimerValue_c 450
#elif(gProfIntrvPeriod_c == gZclSEProfIntrvPeriod_5mins_c)
#define gTimerValue_c 300
#elif(gProfIntrvPeriod_c == gZclSEProfIntrvPeriod_2dot5mins_c)  
#define gTimerValue_c 150
#else 
#define gTimerValue_c 0
#endif

#ifndef gMetDefaultUpdatePeriod_c
#define gMetDefaultUpdatePeriod_c  0x1E
#endif

/* Command IDs for the Simple Metering Client*/
#define gZclCmdSmplMet_GetProfReq_c              0x00 /* O */
#define gZclCmdSmplMet_RequestMirrorRsp_c        0x01 /* O */
#define gZclCmdSmplMet_MirrorRemovedRsp_c        0x02 /* O */
#define gZclCmdSmplMet_ReqFastPollModeReq_c      0x03 /* O */
#define gZclCmdSmplMet_GetSnapshot_Req_c         0x04 /* O */


/* Command IDs for the Simple Metering Server */
#define gZclCmdSmplMet_GetProfRsp_c              0x00 /* O */
#define gZclCmdSmplMet_RequestMirrorReq_c        0x01 /* O */
#define gZclCmdSmplMet_RemoveMirrorReq_c         0x02 /* O */
#define gZclCmdSmplMet_ReqFastPollModeRsp_c      0x03 /* O */

/* Status Field Values for zclCmdSimpleMetering_GetProfileResponse */
#define gSMGetProfRsp_SuccessStatus_c                   0x00 
#define gSMGetProfRsp_UndefIntrvChannelStatus_c         0x01 
#define gSMGetProfRsp_IntrvChannelNotSupportedStatus_c  0x02 
#define gSMGetProfRsp_InvalidEndTimeStatus_c            0x03 
#define gSMGetProfRsp_MorePeriodsRequestedStatus_c      0x04 
#define gSMGetProfRsp_NoIntrvsAvailableStatus_c         0x05

#ifdef __IAR_SYSTEMS_ICC__
#pragma pack(1)
#endif

/* Get Profile Response command payload */
typedef struct zclCmdSmplMet_GetProfRsp_tag
{
  ZCLTime_t EndTime;
  uint8_t   Status;
  uint8_t   ProfIntrvPeriod;
  uint8_t   NumOfPeriodsDlvrd;
  IntrvForProfiling_t Intrvs[1];  
} zclCmdSmplMet_GetProfRsp_t;

/* Get Profile Response request */
typedef struct zclSmplMet_GetProfRsp_tag
{
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdSmplMet_GetProfRsp_t cmdFrame;
} zclSmplMet_GetProfRsp_t;

typedef struct zclGetProfEntry_tag
{
  uint8_t IntrvChannel;
  uint32_t endTime;
  Consmp Intrv;
} zclGetProfEntry_t;

typedef struct zclCmdSmplMet_ReqFastPollModeRsp_tag
{
  uint8_t appliedUpdatePeriod;
  ZCLTime_t EndTime;
}zclCmdSmplMet_ReqFastPollModeRsp_t;

typedef struct zclSmplMet_ReqFastPollModeRsp_tag
{
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdSmplMet_ReqFastPollModeRsp_t cmdFrame;
}zclSmplMet_ReqFastPollModeRsp_t;

/* Interval Channel Values for zclCmdSimpleMetering_GetProfileReq */
#define gIntrvChannel_ConsmpDlvrd_c 0x00 
#define gIntrvChannel_ConsmpRcvd_c  0x01

typedef struct zclCmdSmplMet_GetProfReq_tag
{
  uint8_t   IntrvChannel;
  ZCLTime_t EndTime;  
  uint8_t   NumOfPeriods;
} zclCmdSmplMet_GetProfReq_t;

typedef struct zclSmplMet_GetProfReq_tag
{
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdSmplMet_GetProfReq_t cmdFrame;
} zclSmplMet_GetProfReq_t;

/* Mirroring commands*/

typedef struct zclMet_RequestMirrorReq_tag
{
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
} zclMet_RequestMirrorReq_t;

typedef struct zclMet_RemoveMirrorReq_tag
{
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
} zclMet_RemoveMirrorReq_t;

typedef struct zclCmdMet_RequestMirrorRsp_tag
{
  uint16_t   EndPointID;
} zclCmdMet_RequestMirrorRsp_t;

typedef struct zclMet_RequestMirrorRsp_tag
{
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdMet_RequestMirrorRsp_t cmdFrame;
} zclMet_RequestMirrorRsp_t;

typedef struct zclCmdMet_MirrorRemovedRsp_tag
{
  uint16_t   EndPointID;
} zclCmdMet_MirrorRemovedRsp_t;

typedef struct zclMet_MirrorRemovedRsp_tag
{
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdMet_MirrorRemovedRsp_t cmdFrame;
} zclMet_MirrorRemovedRsp_t;

/* Reqest Fast Poll Mode command payload */
typedef struct zclSmplMet_FastPollModeReq_tag
{
  uint8_t FastPollUpdatePeriod; /* In seconds */
  uint8_t Duration; /* In minutes */
} zclSmplMet_FastPollModeReq_t;

typedef struct zclSmplMet_RequestMirrorRsp_tag
{
  zbEndPoint_t EndPointID;
} zclSmplMet_RequestMirrorRsp_t;

typedef struct zclSmplMet_MirrorRemovedRsp_tag
{
  zbEndPoint_t EndPointID;
} zclSmplMet_MirrorRemovedRsp_t;

typedef struct zclCmdSmplMet_ReqFastPollModeReq_tag
{
  uint8_t updatePeriod;
  uint8_t duration;
}zclCmdSmplMet_ReqFastPollModeReq_t;

typedef struct zclSmplMet_ReqFastPollModeReq_tag
{
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdSmplMet_ReqFastPollModeReq_t cmdFrame;
}zclSmplMet_ReqFastPollModeReq_t;

typedef struct zclCmdSmplMet_GetSnapshotReq_tag
{
  ZCLTime_t   startTime;
  uint8_t     numberOfSnapshots;
  uint16_t    snapshotCause;
}zclCmdSmplMet_GetSnapshotReq_t;

typedef struct zclSmplMet_GetSnapshotReq_tag
{
  afAddrInfo_t                    addrInfo;
  uint8_t                         zclTransactionId;
  zclCmdSmplMet_GetSnapshotReq_t    cmdFrame;
}zclSmplMet_GetSnapshotReq_t;

typedef struct zclCmdSmplMet_GetSnapshotRsp_tag
{
  SEEvtId_t           issuerEventId;
  ZCLTime_t           snapshotTime;
  uint8_t             commandIndex;
  uint16_t            snapshotCause;
  uint8_t             snapshotPayloadType;
}zclCmdSmplMet_GetSnapshotRsp_t;

typedef struct zclSmplMet_GetSnapshotRsp_tag
{
  afAddrInfo_t                      addrInfo;
  uint8_t                           zclTransactionId;
  zclCmdSmplMet_GetSnapshotRsp_t   cmdFrame;
}zclSmplMet_GetSnapshotRsp_t;

typedef struct zclSmplMet_CurrSummDlvrdRcvd_tag
{
  Summ_t    currentSummationDelivered;
  Summ_t    currentSummationReceived;
}zclSmplMet_CurrSummDlvrdRcvd_t;

typedef struct zclSmplMet_TouInfSet_tag
{
  uint8_t   nrOfTiersInUse;
  Summ_t    tierSummation[1];
}zclSmplMet_TouInfSet_t;

typedef struct zclSmplMet_BlockInfSet_tag
{
  uint8_t       nrOfTiersAndBlockThresholdsInUse;
  BlockInf_t    tierBlock[1];
}zclSmplMet_BlockInfSet_t;

typedef struct zclSmplMet_Snapshot_tag
{
  zclSmplMet_CurrSummDlvrdRcvd_t    currSummDlvrRcvd;
  zclSmplMet_TouInfSet_t            *touInfDlvrSet;
  zclSmplMet_TouInfSet_t            *touInfRcvdSet;
  zclSmplMet_BlockInfSet_t          *blockInfDlvrdSet;
  zclSmplMet_BlockInfSet_t          *blockInfRcvdSet;
}zclSmplMet_Snapshot_t;

typedef struct zclMet_MirroringTable_tag
{
  index_t       endPointListIdx;
  zbNwkAddr_t   aNwkAddress;
  bool_t        isActive;
}zclMet_MirroringTable_t;

zbStatus_t ZclSmplMet_ClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev);
zbStatus_t ZclSmplMet_ClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice);

zbStatus_t ZclSmplMet_GetProfReq(zclSmplMet_GetProfReq_t *pReq);
zbStatus_t ZclMet_RemoveMirrorReq(zclMet_RemoveMirrorReq_t *pReq);
zbStatus_t ZclMet_RequestMirrorReq(zclMet_RequestMirrorReq_t *pReq);
zbStatus_t ZclMet_RequestMirrorRsp(zclMet_RequestMirrorRsp_t *pRsp);
zbStatus_t ZclMet_MirrorRemovedRsp(zclMet_MirrorRemovedRsp_t *pRsp);
zbStatus_t ZclSmplMet_ReqFastPollModeReq(zclSmplMet_ReqFastPollModeReq_t *pReq);
zbStatus_t ZclSmplMet_ReqFastPollModeRsp(zclSmplMet_ReqFastPollModeRsp_t *pReq);
zbStatus_t ZclSmplMet_AcceptFastPollModeReq(bool_t acceptFastPollReq);
uint16_t ZclSmplMet_GetFastPollRemainingTime();
zbStatus_t ZclSmplMet_GetSnapshotReq(zclSmplMet_GetSnapshotReq_t *pReq);
#if gZclMirroring_d
zbEndPoint_t ZclMet_CreateAndRegisterMirrorEndpoint(zbNwkAddr_t aNwkAddress);
void ZclMet_ConfigureReportingToMirror(zbApsdeDataIndication_t* pIndication, zbEndPoint_t endPoint);
void ZCL_InitMirroring(void);
zclMet_MirroringTable_t* getMirroringTableEntry(zbNwkAddr_t aNwkAddress, zbEndPoint_t endPoint);
zclMet_MirroringTable_t* getFreeMirroringTableEntry(void);
#endif

/* Timers ID for Simple Metering */
extern tmrTimerID_t gGetProfileTestTimerID;

zbStatus_t ZclSmplMet_GetProfRsp(zclSmplMet_GetProfRsp_t *pReq);
void UpdatePowerConsumption(void);
void ZCL_SmplMet_MeterInit(void);