#include "EmbeddedTypes.h"
#include "zigbee.h"

#include "ZclSEMetering.h"
#include "SEProfile.h"
#include "AppAfInterface.h"
#include "EndpointConfig.h"
#include "ASL_ZdpInterface.h"
#include "zcl.h"

static Consmp consumptionValue;
static Summ_t LastRISCurrSummDlvrd;
static uint8_t ProfIntrvHead;
static uint8_t ProfIntrvTail;
static zclGetProfEntry_t ProfileIntervalTable[gMaxNumberOfPeriodsDelivered_c];
tmrTimerID_t gUpdatePowerConsumptionTimerID;

static zbStatus_t ZCL_SmplMet_ProcessReqFastPollEvt(zbApsdeDataIndication_t *pIndication);
static void FastPollTimerCallBack(tmrTimerID_t tmrID);
void GetProfileTestTimerCallBack(tmrTimerID_t tmrID);
static Consmp *CalculateConsumptionFrom6ByteArray(uint8_t *pNew, uint8_t *pOld);
static void Add8ByteTo6ByteArray(uint8_t value, uint8_t *pSumm_t);
static void UpdatePowerConsumptionTimerCallBack(tmrTimerID_t tmrID);
static void ZCL_BuildGetProfResponse(uint8_t IntrvChannel, ZCLTime_t EndTime, uint8_t NumberOfPeriods, zclCmdSmplMet_GetProfRsp_t *pProfRsp);
static zbStatus_t ZclSmplMet_ProcessGetSnapshotReq(zbApsdeDataIndication_t *pIndication);

static ZCLTime_t mcFastPollEndTime = 0;
static uint16_t mcFastPollRemainingTime = 0;
static uint16_t mcTimeInFastPollMode = 0;
tmrTimerID_t gFastPollTimerID;
bool_t gAcceptFastPoll = TRUE;
tmrTimerID_t gGetProfileTestTimerID;

#if gZclMirroring_d && gNum_MirrorEndpoints_c
zclMet_MirroringTable_t gMirroringTable[gNum_MirrorEndpoints_c];
#endif

extern zbEndPoint_t appEndPoint;

const zclAttrDef_t gaZclSmplMetServerAttrDef[] = {

/* Mandatory SummIntformationSet */ 
  { gZclAttrSmplMetRISCurrSummDlvrd_c, gZclDataTypeUint48_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c | gZclAttrFlagsReportable_c | gZclAttrFlagsAsynchronous_c, sizeof(Summ_t), (void *)MbrOfs(ZCLSmplMetServerAttrsRAM_t, RISCurrSummDlvrd) },
#if gASL_ZclMet_Optionals_d
  { gZclAttrSmplMetRISCurrSummRcvd_c,                          gZclDataTypeUint48_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,sizeof(Summ_t),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,RISCurrSummRcvd)},
  { gZclAttrSmplMetRISCurrMaxDmndDlvrd_c,                      gZclDataTypeUint48_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,sizeof(Summ_t),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,RISCurrSummDlvrd)},
  { gZclAttrSmplMetRISCurrMaxDmndRcvd_c,                       gZclDataTypeUint48_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,sizeof(Summ_t),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,RISCurrMaxDmndRcvd)},
  { gZclAttrSmplMetRISDFTSumm_c,                               gZclDataTypeUint48_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,sizeof(Summ_t),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,RISDFTSumm)},
  { gZclAttrSmplMetRISDailyFreezeTime_c,                       gZclDataTypeUint16_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,sizeof(uint16_t),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,RISDailyFreezeTime)},
  { gZclAttrSmplMetRISPowerFactor_c,                           gZclDataTypeInt8_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,sizeof(uint8_t),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,RISPowerFactor)},
  { gZclAttrSmplMetRISReadingSnapShotTime_c,                   gZclDataTypeUTCTime_c, gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,sizeof(ZCLTime_t),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,RISReadingSnapShotTime)},
  { gZclAttrSmplMetRISCurrMaxDmndDlvrdTimer_c,                 gZclDataTypeUTCTime_c, gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,sizeof(ZCLTime_t),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,RISCurrMaxDmndDlvrdTimer)},
  { gZclAttrSmplMetRISCurrMaxDmndRcvdTime_c,                   gZclDataTypeUTCTime_c, gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,sizeof(ZCLTime_t),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,RISCurrMaxDmndRcvdTime)},
  { gZclAttrSmplMetRISDefaultUpdatePeriod_c,                   gZclDataTypeUint8_c, gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,sizeof(uint8_t),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,RISDefaultUpdatePeriod)},
  { gZclAttrSmplMetRISFastPollUpdatePeriod_c,                  gZclDataTypeUint8_c, gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,sizeof(uint8_t),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,RISFastPollUpdatePeriod)},
  { gZclAttrSmplMetRISCurrBlockPeriodConsumptionDelivered_c,   gZclDataTypeUint48_c, gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,sizeof(Summ_t),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,RISCurrBlockPeriodConsumptionDelivered)},
  { gZclAttrSmplMetRISDailyConsumptionTarget_c,                gZclDataTypeUint24_c, gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,sizeof(Consmp),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,RISDailyConsumptionTarget)},
  { gZclAttrSmplMetRISCurrBlock_c,                             gZclDataTypeEnum8_c, gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,sizeof(uint8_t),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,RISCurrBlock)},
  { gZclAttrSmplMetRISProfileIntPeriod_c,                      gZclDataTypeEnum8_c, gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,sizeof(uint8_t),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,RISProfileIntPeriod)},
  { gZclAttrSmplMetRISIntReadReportingPeriod_c,                gZclDataTypeUint16_c, gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,sizeof(uint16_t),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,RISIntReadReportingPeriod)},
  { gZclAttrSmplMetRISPresetReadingTime_c,                     gZclDataTypeUint16_c, gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,sizeof(uint16_t),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,RISPresetReadingTime)},
  { gZclAttrSmplMetRISVolumePerReport_c,                       gZclDataTypeUint16_c, gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,sizeof(uint16_t),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,RISVolumePerReport)},
  { gZclAttrSmplMetRISFlowRestriction_c,                       gZclDataTypeUint8_c, gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,sizeof(uint8_t),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,RISFlowRestrictio)},
  { gZclAttrSmplMetRISSupplyStatus_c,                          gZclDataTypeBitmap8_c, gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,sizeof(uint8_t),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,RISSupplyStatus)},
  { gZclAttrSmplMetRISCurrInletEnergyCarrierSummation_c,       gZclDataTypeUint48_c, gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,sizeof(Summ_t),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,RISCurrInletEnergyCarrierSummation)},
  { gZclAttrSmplMetRISCurrOutletEnergyCarrierSummation_c,      gZclDataTypeUint48_c, gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,sizeof(Summ_t),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,RISCurrOutletEnergyCarrierSummation)},
  { gZclAttrSmplMetRISInletTemp_c,   gZclDataTypeInt16_c,      gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,sizeof(int16_t),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,RISInletTemp)},
  { gZclAttrSmplMetRISOutletTemp_c,   gZclDataTypeInt16_c,     gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,sizeof(int16_t),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,RISOutletTemp)},
  { gZclAttrSmplMetRISCtrlTemp_c,   gZclDataTypeInt16_c,       gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,sizeof(int16_t),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,RISCtrlTemp)},
  { gZclAttrSmplMetRISCurrInletEnergyCarrierDemand_c,          gZclDataTypeInt24_c, gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,sizeof(Consmp),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,RISCurrInletEnergyCarrierDemand)},
  { gZclAttrSmplMetRISCurrOutletEnergyCarrierDemand_c,         gZclDataTypeInt24_c, gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,sizeof(Consmp),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,RISCurrOutletEnergyCarrierDemand)},
#endif
   /*Attributes of the Simple Metering TOU Information attribute set */
#if gASL_ZclMet_Optionals_d

#if gASL_ZclSE_TiersNumber_d >= 1
  {gZclAttrSmplMetTOUCurrTier1SummDlvrd_c,    gZclDataTypeUint48_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,sizeof(Summ_t),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,TOUCurrTierSumm[0])},
  {gZclAttrSmplMetTOUCurrTier1SummRcvd_c,     gZclDataTypeUint48_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,sizeof(Summ_t),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,TOUCurrTierSumm[1])},
#endif
#if gASL_ZclSE_TiersNumber_d >= 2
  {gZclAttrSmplMetTOUCurrTier2SummDlvrd_c,    gZclDataTypeUint48_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,sizeof(Summ_t),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,TOUCurrTierSumm[2])},
  {gZclAttrSmplMetTOUCurrTier2SummRcvd_c,     gZclDataTypeUint48_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,sizeof(Summ_t),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,TOUCurrTierSumm[3])},
#endif
#if gASL_ZclSE_TiersNumber_d >= 3
  {gZclAttrSmplMetTOUCurrTier3SummDlvrd_c,    gZclDataTypeUint48_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,sizeof(Summ_t),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,TOUCurrTierSumm[4])},
  {gZclAttrSmplMetTOUCurrTier3SummRcvd_c,     gZclDataTypeUint48_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,sizeof(Summ_t),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,TOUCurrTierSumm[5])},
#endif
#if gASL_ZclSE_TiersNumber_d >= 4
  {gZclAttrSmplMetTOUCurrTier4SummDlvrd_c,    gZclDataTypeUint48_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,sizeof(Summ_t),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,TOUCurrTierSumm[6])},
  {gZclAttrSmplMetTOUCurrTier4SummRcvd_c,     gZclDataTypeUint48_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,sizeof(Summ_t),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,TOUCurrTierSumm[7])},
#endif
#if gASL_ZclSE_TiersNumber_d >= 5
  {gZclAttrSmplMetTOUCurrTier5SummDlvrd_c,    gZclDataTypeUint48_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,sizeof(Summ_t),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,TOUCurrTierSumm[8])},
  {gZclAttrSmplMetTOUCurrTier5SummRcvd_c,     gZclDataTypeUint48_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,sizeof(Summ_t),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,TOUCurrTierSumm[9])},
#endif
#if gASL_ZclSE_TiersNumber_d >= 6
  {gZclAttrSmplMetTOUCurrTier6SummDlvrd_c,    gZclDataTypeUint48_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,sizeof(Summ_t),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,TOUCurrTierSumm[10])},
  {gZclAttrSmplMetTOUCurrTier6SummRcvd_c,     gZclDataTypeUint48_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,sizeof(Summ_t),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,TOUCurrTierSumm[11])},
#endif
#if gASL_ZclSE_TiersNumber_d >= 7
  {gZclAttrSmplMetTOUCurrTier7SummDlvrd_c,    gZclDataTypeUint48_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,sizeof(Summ_t),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,TOUCurrTierSumm[12])},
  {gZclAttrSmplMetTOUCurrTier7SummRcvd_c,     gZclDataTypeUint48_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,sizeof(Summ_t),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,TOUCurrTierSumm[13])},
#endif
#if gASL_ZclSE_TiersNumber_d >= 8
  {gZclAttrSmplMetTOUCurrTier8SummDlvrd_c,    gZclDataTypeUint48_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,sizeof(Summ_t),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,TOUCurrTierSumm[14])},
  {gZclAttrSmplMetTOUCurrTier8SummRcvd_c,     gZclDataTypeUint48_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,sizeof(Summ_t),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,TOUCurrTierSumm[15])},
#endif
#if gASL_ZclSE_TiersNumber_d >= 9
  {gZclAttrSmplMetTOUCurrTier9SummDlvrd_c,    gZclDataTypeUint48_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,sizeof(Summ_t),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,TOUCurrTierSumm[16])},
  {gZclAttrSmplMetTOUCurrTier9SummRcvd_c,     gZclDataTypeUint48_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,sizeof(Summ_t),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,TOUCurrTierSumm[17])},
#endif
#if gASL_ZclSE_TiersNumber_d >= 10
  {gZclAttrSmplMetTOUCurrTier10SummDlvrd_c,   gZclDataTypeUint48_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,sizeof(Summ_t),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,TOUCurrTierSumm[18])},
  {gZclAttrSmplMetTOUCurrTier10SummRcvd_c,    gZclDataTypeUint48_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,sizeof(Summ_t),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,TOUCurrTierSumm[19])},
#endif
#if gASL_ZclSE_TiersNumber_d >= 11
  {gZclAttrSmplMetTOUCurrTier11SummDlvrd_c,   gZclDataTypeUint48_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,sizeof(Summ_t),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,TOUCurrTierSumm[20])},
  {gZclAttrSmplMetTOUCurrTier11SummRcvd_c,    gZclDataTypeUint48_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,sizeof(Summ_t),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,TOUCurrTierSumm[21])},
#endif
#if gASL_ZclSE_TiersNumber_d >= 12
  {gZclAttrSmplMetTOUCurrTier12SummDlvrd_c,   gZclDataTypeUint48_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,sizeof(Summ_t),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,TOUCurrTierSumm[22])},
  {gZclAttrSmplMetTOUCurrTier12SummRcvd_c,    gZclDataTypeUint48_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,sizeof(Summ_t),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,TOUCurrTierSumm[23])},
#endif
#if gASL_ZclSE_TiersNumber_d >= 13
  {gZclAttrSmplMetTOUCurrTier13SummDlvrd_c,   gZclDataTypeUint48_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,sizeof(Summ_t),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,TOUCurrTierSumm[24])},
  {gZclAttrSmplMetTOUCurrTier13SummRcvd_c,    gZclDataTypeUint48_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,sizeof(Summ_t),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,TOUCurrTierSumm[25])},
#endif
#if gASL_ZclSE_TiersNumber_d >= 14
  {gZclAttrSmplMetTOUCurrTier14SummDlvrd_c,   gZclDataTypeUint48_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,sizeof(Summ_t),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,TOUCurrTierSumm[26])},
  {gZclAttrSmplMetTOUCurrTier14SummRcvd_c,    gZclDataTypeUint48_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,sizeof(Summ_t),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,TOUCurrTierSumm[27])},
#endif
#if gASL_ZclSE_TiersNumber_d >= 15
  {gZclAttrSmplMetTOUCurrTier15SummDlvrd_c,   gZclDataTypeUint48_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,sizeof(Summ_t),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,TOUCurrTierSumm[28])},
  {gZclAttrSmplMetTOUCurrTier15SummRcvd_c,    gZclDataTypeUint48_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,sizeof(Summ_t),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,TOUCurrTierSumm[29])},
#endif

#endif

/* Mandatory Meter Status Set */
  { gZclAttrSmplMetMSStatus_c,                 gZclDataTypeBitmap8_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c | gZclAttrFlagsReportable_c, sizeof(uint8_t), (void *)MbrOfs(ZCLSmplMetServerAttrsRAM_t, MSStatus)  },
#if gASL_ZclMet_Optionals_d 
  { gZclAttrSmplMetMSSRemainingBatteryLife_c,  gZclDataTypeUint8_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint8_t), (void *)MbrOfs(ZCLSmplMetServerAttrsRAM_t, RemainingBatteryLife)  },
  { gZclAttrSmplMetMSSHoursInOperation_c,      gZclDataTypeUint24_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(Consmp), (void *)MbrOfs(ZCLSmplMetServerAttrsRAM_t, HoursInOperation)  },
  { gZclAttrSmplMetMSSHoursInFault_c,          gZclDataTypeUint24_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(Consmp), (void *)MbrOfs(ZCLSmplMetServerAttrsRAM_t, HoursInFault)  },
#endif

/* Mandatory Consmp Formating Set */
  { gZclAttrSmplMetCFSUnitofMeasure_c,          gZclDataTypeEnum8_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c | gZclAttrFlagsReportable_c, sizeof(uint8_t), (void *)MbrOfs(ZCLSmplMetServerAttrsRAM_t, SmplMetCFSUnitofMeasure) },
#if gASL_ZclMet_Optionals_d  
  {gZclAttrSmplMetCFSMultiplier_c,              gZclDataTypeUint24_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Consmp),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,Mult)},
  {gZclAttrSmplMetCFSDivisor_c,                 gZclDataTypeUint24_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Consmp),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,Div)},
#endif  
  {gZclAttrSmplMetCFSSummFormat_c,              gZclDataTypeBitmap8_c,gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c | gZclAttrFlagsReportable_c, sizeof(uint8_t), (void *)MbrOfs(ZCLSmplMetServerAttrsRAM_t, CFSSummFormat) },    
#if gASL_ZclMet_Optionals_d    
  {gZclAttrSmplMetCFSDmndFormat_c,              gZclDataTypeBitmap8_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(uint8_t),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,DmndFormat)},
  {gZclAttrSmplMetCFSHistoricalConsmpFormat_c,  gZclDataTypeBitmap8_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(uint8_t),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,HistoricalConsmpFormat)},
#endif  
  {gZclAttrSmplMetCFSMetDevType_c,              gZclDataTypeBitmap8_c,gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c | gZclAttrFlagsReportable_c, sizeof(uint8_t), (void *)MbrOfs(ZCLSmplMetServerAttrsRAM_t, CFSMetDevType) },   
#if gASL_ZclMet_Optionals_d
  {gZclAttrSmplMetCFSSiteID_c,                            gZclDataTypeOctetStr_c,gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(zclStr32Oct_t), (void *)MbrOfs(ZCLSmplMetServerAttrsRAM_t, CFSSiteID) },
  {gZclAttrSmplMetCFSMeterSerialNumber_c,                 gZclDataTypeOctetStr_c,gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(zclStr24Oct_t), (void *)MbrOfs(ZCLSmplMetServerAttrsRAM_t, CFSMeterSerialNumber) },
  {gZclAttrSmplMetCFSEnergyCarrierUnitOfMeasure_c,        gZclDataTypeEnum8_c,gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint8_t), (void *)MbrOfs(ZCLSmplMetServerAttrsRAM_t, CFSEnergyCarrierUnitOfMeasure) },   
  {gZclAttrSmplMetCFSEnergyCarrierSummationFormatting_c,  gZclDataTypeBitmap8_c,gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint8_t), (void *)MbrOfs(ZCLSmplMetServerAttrsRAM_t, CFSEnergyCarrierSummationFormatting) },   
  {gZclAttrSmplMetCFSEnergyCarrierDemandFormatting_c,     gZclDataTypeBitmap8_c,gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint8_t), (void *)MbrOfs(ZCLSmplMetServerAttrsRAM_t, CFSEnergyCarrierDemandFormatting) },   
  {gZclAttrSmplMetCFSTempUnitOfMeasure_c,                 gZclDataTypeEnum8_c,gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint8_t), (void *)MbrOfs(ZCLSmplMetServerAttrsRAM_t, CFSTempUnitOfMeasure) },   
  {gZclAttrSmplMetCFSTempFormatting_c,                    gZclDataTypeBitmap8_c,gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint8_t), (void *)MbrOfs(ZCLSmplMetServerAttrsRAM_t, CFSTempFormatting) },   
   /*Attributes of the Simple Metering Meter ESP Historical attribute set */  
  {gZclAttrSmplMetEHCInstantaneousDmnd_c,               gZclDataTypeInt24_c, gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Consmp),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,InstantaneousDmnd)},
  {gZclAttrSmplMetEHCCurrDayConsmpDlvrd_c,              gZclDataTypeUint24_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Consmp),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrDayConsmpDlvrd)},
  {gZclAttrSmplMetEHCCurrDayConsmpRcvd_c,               gZclDataTypeUint24_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Consmp),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrDayConsmpRcvd)},
  {gZclAttrSmplMetEHCPreviousDayConsmpDlvrd_c,          gZclDataTypeUint24_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Consmp),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,PreviousDayConsmpDlvrd)},
  {gZclAttrSmplMetEHCPreviousDayConsmpRcvd_c,           gZclDataTypeUint24_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Consmp),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,PreviousDayConsmpRcvd)},
  {gZclAttrSmplMetEHCCurrPrtlProfIntrvStartTimeDlvrd_c, gZclDataTypeUTCTime_c, gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(ZCLTime_t),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrPartialProfileIntrvStartTimeDlvrd)},
  {gZclAttrSmplMetEHCCurrPrtlProfIntrvStartTimeRcvd_c,  gZclDataTypeUTCTime_c, gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(ZCLTime_t),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrPartialProfileIntrvStartTimeRcvd)},
  {gZclAttrSmplMetEHCCurrPrtlProfIntrvValueDlvrd_c,     gZclDataTypeUint24_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(IntrvForProfiling_t),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrPartialProfileIntrvValueDlvrd)},
  {gZclAttrSmplMetEHCCurrPrtlProfIntrvValueRcvd_c,      gZclDataTypeUint24_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(IntrvForProfiling_t),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrPartialProfileIntrvValueRcvd)},

  {gZclAttrSmplMetEHCCurrDayMaxPressure_c,              gZclDataTypeUint48_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Pressure_t),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrDayMaxPressure)},
  {gZclAttrSmplMetEHCCurrDayMinPressure_c,              gZclDataTypeUint48_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Pressure_t),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrDayMinPressure)},
  {gZclAttrSmplMetEHCPrevDayMaxPressure_c,              gZclDataTypeUint48_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Pressure_t),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,PrevDayMaxPressure)},
  {gZclAttrSmplMetEHCPrevDayMinPressure_c,              gZclDataTypeUint48_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Pressure_t),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,PrevDayMinPressure)},
  {gZclAttrSmplMetEHCCurrDayMaxDemand_c,                gZclDataTypeInt24_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Demmand_t),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrDayMaxDemand)},
  {gZclAttrSmplMetEHCPrevDayMaxDemand_c,                gZclDataTypeInt24_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Demmand_t),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,PrevDayMaxDemand)},
  {gZclAttrSmplMetEHCCurrMonthMaxDemand_c,              gZclDataTypeInt24_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Demmand_t),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrMonthMaxDemand)},
  {gZclAttrSmplMetEHCCurrYearMaxDemand_c,               gZclDataTypeInt24_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Demmand_t),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrYearMaxDemand)},
  {gZclAttrSmplMetEHCCurrDayMaxEnergyCarrierDemand_c,   gZclDataTypeInt24_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Demmand_t),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrDayMaxEnergyCarrierDemand)},
  {gZclAttrSmplMetEHCPrevDayMaxEnergyCarrierDemand_c,   gZclDataTypeInt24_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Demmand_t),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,PrevDayMaxEnergyCarrierDemand)},
  {gZclAttrSmplMetEHCCurrMonthMaxEnergyCarrierDemand_c, gZclDataTypeInt24_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Demmand_t),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrMonthMaxEnergyCarrierDemand)},
  {gZclAttrSmplMetEHCCurrMonthMinEnergyCarrierDemand_c, gZclDataTypeInt24_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Demmand_t),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrMonthMinEnergyCarrierDemand)},
  {gZclAttrSmplMetEHCCurrYearMaxEnergyCarrierDemand_c,  gZclDataTypeInt24_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Demmand_t),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrYearMaxEnergyCarrierDemand)},
  {gZclAttrSmplMetEHCCurrYearMinEnergyCarrierDemand_c,  gZclDataTypeInt24_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Demmand_t),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrYearMinEnergyCarrierDemand)},
  /*Attributes of the Simple Metering Meter Load Profile Configuration attribute set */  
  {gZclAttrSmplMetLPCMaxNumOfPeriodsDlvrd_c,      gZclDataTypeUint8_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(uint8_t),(void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,MaxNumberOfPeriodsDlvrd)},
  /*Attributes of the Simple Metering Alarm attribute set */  
  {gZclAttrSmplMetASGenericAlarmMask_c,                 gZclDataTypeBitmap16_c,gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint16_t), (void *)MbrOfs(ZCLSmplMetServerAttrsRAM_t, ASGenericAlarmMask) },   
  {gZclAttrSmplMetASElectricityAlarmMask_c,             gZclDataTypeBitmap16_c,gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint16_t), (void *)MbrOfs(ZCLSmplMetServerAttrsRAM_t, ASElectricityAlarmMask) },     
  {gZclAttrSmplMetASGenericFlowPressureAlarmMask_c,     gZclDataTypeBitmap16_c,gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint16_t), (void *)MbrOfs(ZCLSmplMetServerAttrsRAM_t, ASGenericFlowPressureAlarmMask) },     
  {gZclAttrSmplMetASWaterSpecificAlarmMask_c,           gZclDataTypeBitmap16_c,gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint16_t), (void *)MbrOfs(ZCLSmplMetServerAttrsRAM_t, ASWaterSpecificAlarmMask) },     
  {gZclAttrSmplMetASHeatAndCoolingSpecificAlarmMask_c,  gZclDataTypeBitmap16_c,gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint16_t), (void *)MbrOfs(ZCLSmplMetServerAttrsRAM_t, ASHeatAndCoolingSpecificAlarmMask) },     
  {gZclAttrSmplMetASGasSpecificAlarmMask_c,             gZclDataTypeBitmap16_c,gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint16_t), (void *)MbrOfs(ZCLSmplMetServerAttrsRAM_t, ASGasSpecificAlarmMask) },
  /*Attributes of the Simple Metering Supply Limit Attribute Set */ 
  {gZclAttrSmplMetSLSCurrDmndDlvrd_c,                 gZclDataTypeUint24_c,gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(Consmp), (void *)MbrOfs(ZCLSmplMetServerAttrsRAM_t, CurrDmndDlvrd) },   
  {gZclAttrSmplMetSLSDmndLimit_c,                     gZclDataTypeUint24_c,gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(Consmp), (void *)MbrOfs(ZCLSmplMetServerAttrsRAM_t, DmndLimit) },     
  {gZclAttrSmplMetSLSDmndIntegrationPeriod_c,         gZclDataTypeUint8_c,gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint8_t), (void *)MbrOfs(ZCLSmplMetServerAttrsRAM_t, DmndIntegrationPeriod) },     
  {gZclAttrSmplMetSLSNumOfDmndSubIntrvs_c,            gZclDataTypeUint8_c,gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint8_t), (void *)MbrOfs(ZCLSmplMetServerAttrsRAM_t, NumOfDmndSubIntrvs) },
  /*Attributes of the Simple Metering Block Information Delivered Attribute Set */
#if gASL_ZclMet_BISSetNumberOfTiersDlvr_d >= 1
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 1
  {gZclAttrSmplMetBISCurrNoTierBlock1SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[0][0])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 2
  {gZclAttrSmplMetBISCurrNoTierBlock2SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[0][1])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 3
  {gZclAttrSmplMetBISCurrNoTierBlock3SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[0][2])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 4
  {gZclAttrSmplMetBISCurrNoTierBlock4SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[0][3])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 5
  {gZclAttrSmplMetBISCurrNoTierBlock5SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[0][4])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 6
  {gZclAttrSmplMetBISCurrNoTierBlock6SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[0][5])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 7
  {gZclAttrSmplMetBISCurrNoTierBlock7SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[0][6])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 8
  {gZclAttrSmplMetBISCurrNoTierBlock8SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[0][7])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 9
  {gZclAttrSmplMetBISCurrNoTierBlock9SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[0][8])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 10
  {gZclAttrSmplMetBISCurrNoTierBlock10SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[0][9])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 11
  {gZclAttrSmplMetBISCurrNoTierBlock11SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[0][10])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 12
  {gZclAttrSmplMetBISCurrNoTierBlock12SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[0][11])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 13
  {gZclAttrSmplMetBISCurrNoTierBlock13SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[0][12])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 14
  {gZclAttrSmplMetBISCurrNoTierBlock14SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[0][13])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 15
  {gZclAttrSmplMetBISCurrNoTierBlock15SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[0][14])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 16
  {gZclAttrSmplMetBISCurrNoTierBlock16SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[0][15])},
#endif
#endif
#if gASL_ZclMet_BISSetNumberOfTiersDlvr_d >= 2
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 1
  {gZclAttrSmplMetBISCurrNoTier1Block1SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[1][0])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 2
  {gZclAttrSmplMetBISCurrNoTier1Block2SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[1][1])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 3
  {gZclAttrSmplMetBISCurrNoTier1Block3SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[1][2])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 4
  {gZclAttrSmplMetBISCurrNoTier1Block4SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[1][3])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 5
  {gZclAttrSmplMetBISCurrNoTier1Block5SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[1][4])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 6
  {gZclAttrSmplMetBISCurrNoTier1Block6SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[1][5])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 7
  {gZclAttrSmplMetBISCurrNoTier1Block7SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[1][6])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 8
  {gZclAttrSmplMetBISCurrNoTier1Block8SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[1][7])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 9
  {gZclAttrSmplMetBISCurrNoTier1Block9SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[1][8])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 10
  {gZclAttrSmplMetBISCurrNoTier1Block10SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[1][9])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 11
  {gZclAttrSmplMetBISCurrNoTier1Block11SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[1][10])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 12
  {gZclAttrSmplMetBISCurrNoTier1Block12SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[1][11])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 13
  {gZclAttrSmplMetBISCurrNoTier1Block13SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[1][12])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 14
  {gZclAttrSmplMetBISCurrNoTier1Block14SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[1][13])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 15
  {gZclAttrSmplMetBISCurrNoTier1Block15SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[1][14])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 16
  {gZclAttrSmplMetBISCurrNoTier1Block16SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[1][15])},
#endif
#endif
#if gASL_ZclMet_BISSetNumberOfTiersDlvr_d >= 3
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 1
  {gZclAttrSmplMetBISCurrNoTier2Block1SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[2][0])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 2
  {gZclAttrSmplMetBISCurrNoTier2Block2SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[2][1])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 3
  {gZclAttrSmplMetBISCurrNoTier2Block3SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[2][2])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 4
  {gZclAttrSmplMetBISCurrNoTier2Block4SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[2][3])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 5
  {gZclAttrSmplMetBISCurrNoTier2Block5SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[2][4])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 6
  {gZclAttrSmplMetBISCurrNoTier2Block6SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[2][5])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 7
  {gZclAttrSmplMetBISCurrNoTier2Block7SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[2][6])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 8
  {gZclAttrSmplMetBISCurrNoTier2Block8SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[2][7])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 9
  {gZclAttrSmplMetBISCurrNoTier2Block9SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[2][8])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 10
  {gZclAttrSmplMetBISCurrNoTier2Block10SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[2][9])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 11
  {gZclAttrSmplMetBISCurrNoTier2Block11SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[2][10])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 12
  {gZclAttrSmplMetBISCurrNoTier2Block12SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[2][11])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 13
  {gZclAttrSmplMetBISCurrNoTier2Block13SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[2][12])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 14
  {gZclAttrSmplMetBISCurrNoTier2Block14SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[2][13])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 15
  {gZclAttrSmplMetBISCurrNoTier2Block15SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[2][14])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 16
  {gZclAttrSmplMetBISCurrNoTier2Block16SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[2][15])},
#endif
#endif
#if gASL_ZclMet_BISSetNumberOfTiersDlvr_d >= 4
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 1
  {gZclAttrSmplMetBISCurrNoTier3Block1SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[3][0])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 2
  {gZclAttrSmplMetBISCurrNoTier3Block2SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[3][1])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 3
  {gZclAttrSmplMetBISCurrNoTier3Block3SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[3][2])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 4
  {gZclAttrSmplMetBISCurrNoTier3Block4SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[3][3])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 5
  {gZclAttrSmplMetBISCurrNoTier3Block5SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[3][4])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 6
  {gZclAttrSmplMetBISCurrNoTier3Block6SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[3][5])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 7
  {gZclAttrSmplMetBISCurrNoTier3Block7SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[3][6])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 8
  {gZclAttrSmplMetBISCurrNoTier3Block8SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[3][7])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 9
  {gZclAttrSmplMetBISCurrNoTier3Block9SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[3][8])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 10
  {gZclAttrSmplMetBISCurrNoTier3Block10SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[3][9])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 11
  {gZclAttrSmplMetBISCurrNoTier3Block11SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[3][10])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 12
  {gZclAttrSmplMetBISCurrNoTier3Block12SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[3][11])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 13
  {gZclAttrSmplMetBISCurrNoTier3Block13SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[3][12])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 14
  {gZclAttrSmplMetBISCurrNoTier3Block14SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[3][13])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 15
  {gZclAttrSmplMetBISCurrNoTier3Block15SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[3][14])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 16
  {gZclAttrSmplMetBISCurrNoTier3Block16SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[3][15])},
#endif
#endif
#if gASL_ZclMet_BISSetNumberOfTiersDlvr_d >= 5
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 1
  {gZclAttrSmplMetBISCurrNoTier4Block1SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[4][0])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 2
  {gZclAttrSmplMetBISCurrNoTier4Block2SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[4][1])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 3
  {gZclAttrSmplMetBISCurrNoTier4Block3SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[4][2])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 4
  {gZclAttrSmplMetBISCurrNoTier4Block4SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[4][3])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 5
  {gZclAttrSmplMetBISCurrNoTier4Block5SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[4][4])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 6
  {gZclAttrSmplMetBISCurrNoTier4Block6SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[4][5])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 7
  {gZclAttrSmplMetBISCurrNoTier4Block7SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[4][6])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 8
  {gZclAttrSmplMetBISCurrNoTier4Block8SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[4][7])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 9
  {gZclAttrSmplMetBISCurrNoTier4Block9SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[4][8])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 10
  {gZclAttrSmplMetBISCurrNoTier4Block10SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[4][9])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 11
  {gZclAttrSmplMetBISCurrNoTier4Block11SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[4][10])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 12
  {gZclAttrSmplMetBISCurrNoTier4Block12SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[4][11])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 13
  {gZclAttrSmplMetBISCurrNoTier4Block13SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[4][12])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 14
  {gZclAttrSmplMetBISCurrNoTier4Block14SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[4][13])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 15
  {gZclAttrSmplMetBISCurrNoTier4Block15SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[4][14])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 16
  {gZclAttrSmplMetBISCurrNoTier4Block16SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[4][15])},
#endif
#endif
#if gASL_ZclMet_BISSetNumberOfTiersDlvr_d >= 6
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 1
  {gZclAttrSmplMetBISCurrNoTier5Block1SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[5][0])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 2
  {gZclAttrSmplMetBISCurrNoTier5Block2SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[5][1])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 3
  {gZclAttrSmplMetBISCurrNoTier5Block3SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[5][2])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 4
  {gZclAttrSmplMetBISCurrNoTier5Block4SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[5][3])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 5
  {gZclAttrSmplMetBISCurrNoTier5Block5SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[5][4])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 6
  {gZclAttrSmplMetBISCurrNoTier5Block6SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[5][5])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 7
  {gZclAttrSmplMetBISCurrNoTier5Block7SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[5][6])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 8
  {gZclAttrSmplMetBISCurrNoTier5Block8SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[5][7])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 9
  {gZclAttrSmplMetBISCurrNoTier5Block9SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[5][8])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 10
  {gZclAttrSmplMetBISCurrNoTier5Block10SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[5][9])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 11
  {gZclAttrSmplMetBISCurrNoTier5Block11SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[5][10])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 12
  {gZclAttrSmplMetBISCurrNoTier5Block12SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[5][11])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 13
  {gZclAttrSmplMetBISCurrNoTier5Block13SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[5][12])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 14
  {gZclAttrSmplMetBISCurrNoTier5Block14SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[5][13])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 15
  {gZclAttrSmplMetBISCurrNoTier5Block15SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[5][14])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 16
  {gZclAttrSmplMetBISCurrNoTier5Block16SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[5][15])},
#endif
#endif
#if gASL_ZclMet_BISSetNumberOfTiersDlvr_d >= 7
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 1
  {gZclAttrSmplMetBISCurrNoTier6Block1SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[6][0])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 2
  {gZclAttrSmplMetBISCurrNoTier6Block2SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[6][1])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 3
  {gZclAttrSmplMetBISCurrNoTier6Block3SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[6][2])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 4
  {gZclAttrSmplMetBISCurrNoTier6Block4SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[6][3])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 5
  {gZclAttrSmplMetBISCurrNoTier6Block5SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[6][4])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 6
  {gZclAttrSmplMetBISCurrNoTier6Block6SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[6][5])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 7
  {gZclAttrSmplMetBISCurrNoTier6Block7SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[6][6])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 8
  {gZclAttrSmplMetBISCurrNoTier6Block8SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[6][7])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 9
  {gZclAttrSmplMetBISCurrNoTier6Block9SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[6][8])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 10
  {gZclAttrSmplMetBISCurrNoTier6Block10SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[6][9])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 11
  {gZclAttrSmplMetBISCurrNoTier6Block11SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[6][10])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 12
  {gZclAttrSmplMetBISCurrNoTier6Block12SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[6][11])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 13
  {gZclAttrSmplMetBISCurrNoTier6Block13SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[6][12])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 14
  {gZclAttrSmplMetBISCurrNoTier6Block14SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[6][13])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 15
  {gZclAttrSmplMetBISCurrNoTier6Block15SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[6][14])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 16
  {gZclAttrSmplMetBISCurrNoTier6Block16SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[6][15])},
#endif
#endif
#if gASL_ZclMet_BISSetNumberOfTiersDlvr_d >= 8
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 1
  {gZclAttrSmplMetBISCurrNoTier7Block1SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[7][0])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 2
  {gZclAttrSmplMetBISCurrNoTier7Block2SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[7][1])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 3
  {gZclAttrSmplMetBISCurrNoTier7Block3SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[7][2])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 4
  {gZclAttrSmplMetBISCurrNoTier7Block4SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[7][3])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 5
  {gZclAttrSmplMetBISCurrNoTier7Block5SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[7][4])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 6
  {gZclAttrSmplMetBISCurrNoTier7Block6SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[7][5])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 7
  {gZclAttrSmplMetBISCurrNoTier7Block7SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[7][6])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 8
  {gZclAttrSmplMetBISCurrNoTier7Block8SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[7][7])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 9
  {gZclAttrSmplMetBISCurrNoTier7Block9SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[7][8])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 10
  {gZclAttrSmplMetBISCurrNoTier7Block10SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[7][9])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 11
  {gZclAttrSmplMetBISCurrNoTier7Block11SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[7][10])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 12
  {gZclAttrSmplMetBISCurrNoTier7Block12SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[7][11])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 13
  {gZclAttrSmplMetBISCurrNoTier7Block13SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[7][12])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 14
  {gZclAttrSmplMetBISCurrNoTier7Block14SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[7][13])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 15
  {gZclAttrSmplMetBISCurrNoTier7Block15SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[7][14])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 16
  {gZclAttrSmplMetBISCurrNoTier7Block16SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[7][15])},
#endif
#endif
#if gASL_ZclMet_BISSetNumberOfTiersDlvr_d >= 9
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 1
  {gZclAttrSmplMetBISCurrNoTier8Block1SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[8][0])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 2
  {gZclAttrSmplMetBISCurrNoTier8Block2SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[8][1])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 3
  {gZclAttrSmplMetBISCurrNoTier8Block3SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[8][2])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 4
  {gZclAttrSmplMetBISCurrNoTier8Block4SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[8][3])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 5
  {gZclAttrSmplMetBISCurrNoTier8Block5SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[8][4])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 6
  {gZclAttrSmplMetBISCurrNoTier8Block6SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[8][5])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 7
  {gZclAttrSmplMetBISCurrNoTier8Block7SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[8][6])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 8
  {gZclAttrSmplMetBISCurrNoTier8Block8SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[8][7])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 9
  {gZclAttrSmplMetBISCurrNoTier8Block9SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[8][8])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 10
  {gZclAttrSmplMetBISCurrNoTier8Block10SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[8][9])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 11
  {gZclAttrSmplMetBISCurrNoTier8Block11SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[8][10])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 12
  {gZclAttrSmplMetBISCurrNoTier8Block12SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[8][11])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 13
  {gZclAttrSmplMetBISCurrNoTier8Block13SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[8][12])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 14
  {gZclAttrSmplMetBISCurrNoTier8Block14SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[8][13])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 15
  {gZclAttrSmplMetBISCurrNoTier8Block15SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[8][14])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 16
  {gZclAttrSmplMetBISCurrNoTier8Block16SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[8][15])},
#endif
#endif
#if gASL_ZclMet_BISSetNumberOfTiersDlvr_d >= 10
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 1
  {gZclAttrSmplMetBISCurrNoTier9Block1SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[9][0])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 2
  {gZclAttrSmplMetBISCurrNoTier9Block2SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[9][1])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 3
  {gZclAttrSmplMetBISCurrNoTier9Block3SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[9][2])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 4
  {gZclAttrSmplMetBISCurrNoTier9Block4SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[9][3])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 5
  {gZclAttrSmplMetBISCurrNoTier9Block5SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[9][4])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 6
  {gZclAttrSmplMetBISCurrNoTier9Block6SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[9][5])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 7
  {gZclAttrSmplMetBISCurrNoTier9Block7SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[9][6])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 8
  {gZclAttrSmplMetBISCurrNoTier9Block8SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[9][7])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 9
  {gZclAttrSmplMetBISCurrNoTier9Block9SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[9][8])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 10
  {gZclAttrSmplMetBISCurrNoTier9Block10SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[9][9])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 11
  {gZclAttrSmplMetBISCurrNoTier9Block11SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[9][10])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 12
  {gZclAttrSmplMetBISCurrNoTier9Block12SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[9][11])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 13
  {gZclAttrSmplMetBISCurrNoTier9Block13SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[9][12])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 14
  {gZclAttrSmplMetBISCurrNoTier9Block14SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[9][13])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 15
  {gZclAttrSmplMetBISCurrNoTier9Block15SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[9][14])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 16
  {gZclAttrSmplMetBISCurrNoTier9Block16SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[9][15])},
#endif
#endif
#if gASL_ZclMet_BISSetNumberOfTiersDlvr_d >= 11
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 1
  {gZclAttrSmplMetBISCurrNoTier10Block1SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[10][0])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 2
  {gZclAttrSmplMetBISCurrNoTier10Block2SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[10][1])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 3
  {gZclAttrSmplMetBISCurrNoTier10Block3SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[10][2])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 4
  {gZclAttrSmplMetBISCurrNoTier10Block4SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[10][3])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 5
  {gZclAttrSmplMetBISCurrNoTier10Block5SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[10][4])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 6
  {gZclAttrSmplMetBISCurrNoTier10Block6SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[10][5])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 7
  {gZclAttrSmplMetBISCurrNoTier10Block7SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[10][6])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 8
  {gZclAttrSmplMetBISCurrNoTier10Block8SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[10][7])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 9
  {gZclAttrSmplMetBISCurrNoTier10Block9SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[10][8])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 10
  {gZclAttrSmplMetBISCurrNoTier10Block10SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[10][9])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 11
  {gZclAttrSmplMetBISCurrNoTier10Block11SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[10][10])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 12
  {gZclAttrSmplMetBISCurrNoTier10Block12SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[10][11])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 13
  {gZclAttrSmplMetBISCurrNoTier10Block13SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[10][12])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 14
  {gZclAttrSmplMetBISCurrNoTier10Block14SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[10][13])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 15
  {gZclAttrSmplMetBISCurrNoTier10Block15SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[10][14])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 16
  {gZclAttrSmplMetBISCurrNoTier10Block16SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[10][15])},
#endif
#endif
#if gASL_ZclMet_BISSetNumberOfTiersDlvr_d >= 12
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 1
  {gZclAttrSmplMetBISCurrNoTier11Block1SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[12][0])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 2
  {gZclAttrSmplMetBISCurrNoTier11Block2SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[12][1])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 3
  {gZclAttrSmplMetBISCurrNoTier11Block3SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[12][2])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 4
  {gZclAttrSmplMetBISCurrNoTier11Block4SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[12][3])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 5
  {gZclAttrSmplMetBISCurrNoTier11Block5SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[12][4])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 6
  {gZclAttrSmplMetBISCurrNoTier11Block6SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[12][5])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 7
  {gZclAttrSmplMetBISCurrNoTier11Block7SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[12][6])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 8
  {gZclAttrSmplMetBISCurrNoTier11Block8SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[12][7])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 9
  {gZclAttrSmplMetBISCurrNoTier11Block9SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[12][8])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 10
  {gZclAttrSmplMetBISCurrNoTier11Block10SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[12][9])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 11
  {gZclAttrSmplMetBISCurrNoTier11Block11SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[12][10])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 12
  {gZclAttrSmplMetBISCurrNoTier11Block12SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[12][11])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 13
  {gZclAttrSmplMetBISCurrNoTier11Block13SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[12][12])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 14
  {gZclAttrSmplMetBISCurrNoTier11Block14SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[12][13])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 15
  {gZclAttrSmplMetBISCurrNoTier11Block15SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[12][14])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 16
  {gZclAttrSmplMetBISCurrNoTier11Block16SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[12][15])},
#endif
#endif
#if gASL_ZclMet_BISSetNumberOfTiersDlvr_d >= 13
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 1
  {gZclAttrSmplMetBISCurrNoTier12Block1SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[13][0])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 2
  {gZclAttrSmplMetBISCurrNoTier12Block2SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[13][1])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 3
  {gZclAttrSmplMetBISCurrNoTier12Block3SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[13][2])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 4
  {gZclAttrSmplMetBISCurrNoTier12Block4SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[13][3])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 5
  {gZclAttrSmplMetBISCurrNoTier12Block5SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[13][4])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 6
  {gZclAttrSmplMetBISCurrNoTier12Block6SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[13][5])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 7
  {gZclAttrSmplMetBISCurrNoTier12Block7SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[13][6])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 8
  {gZclAttrSmplMetBISCurrNoTier12Block8SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[13][7])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 9
  {gZclAttrSmplMetBISCurrNoTier12Block9SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[13][8])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 10
  {gZclAttrSmplMetBISCurrNoTier12Block10SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[13][9])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 11
  {gZclAttrSmplMetBISCurrNoTier12Block11SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[13][10])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 12
  {gZclAttrSmplMetBISCurrNoTier12Block12SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[13][11])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 13
  {gZclAttrSmplMetBISCurrNoTier12Block13SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[13][12])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 14
  {gZclAttrSmplMetBISCurrNoTier12Block14SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[13][13])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 15
  {gZclAttrSmplMetBISCurrNoTier12Block15SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[13][14])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 16
  {gZclAttrSmplMetBISCurrNoTier12Block16SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[13][15])},
#endif
#endif
#if gASL_ZclMet_BISSetNumberOfTiersDlvr_d >= 14
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 1
  {gZclAttrSmplMetBISCurrNoTier13Block1SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[14][0])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 2
  {gZclAttrSmplMetBISCurrNoTier13Block2SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[14][1])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 3
  {gZclAttrSmplMetBISCurrNoTier13Block3SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[14][2])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 4
  {gZclAttrSmplMetBISCurrNoTier13Block4SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[14][3])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 5
  {gZclAttrSmplMetBISCurrNoTier13Block5SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[14][4])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 6
  {gZclAttrSmplMetBISCurrNoTier13Block6SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[14][5])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 7
  {gZclAttrSmplMetBISCurrNoTier13Block7SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[14][6])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 8
  {gZclAttrSmplMetBISCurrNoTier13Block8SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[14][7])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 9
  {gZclAttrSmplMetBISCurrNoTier13Block9SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[14][8])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 10
  {gZclAttrSmplMetBISCurrNoTier13Block10SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[14][9])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 11
  {gZclAttrSmplMetBISCurrNoTier13Block11SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[14][10])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 12
  {gZclAttrSmplMetBISCurrNoTier13Block12SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[14][11])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 13
  {gZclAttrSmplMetBISCurrNoTier13Block13SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[14][12])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 14
  {gZclAttrSmplMetBISCurrNoTier13Block14SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[14][13])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 15
  {gZclAttrSmplMetBISCurrNoTier13Block15SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[14][14])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 16
  {gZclAttrSmplMetBISCurrNoTier13Block16SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[14][15])},
#endif
#endif
#if gASL_ZclMet_BISSetNumberOfTiersDlvr_d >= 15
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 1
  {gZclAttrSmplMetBISCurrNoTier14Block1SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[15][0])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 2
  {gZclAttrSmplMetBISCurrNoTier14Block2SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[15][1])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 3
  {gZclAttrSmplMetBISCurrNoTier14Block3SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[15][2])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 4
  {gZclAttrSmplMetBISCurrNoTier14Block4SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[15][3])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 5
  {gZclAttrSmplMetBISCurrNoTier14Block5SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[15][4])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 6
  {gZclAttrSmplMetBISCurrNoTier14Block6SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[15][5])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 7
  {gZclAttrSmplMetBISCurrNoTier14Block7SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[15][6])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 8
  {gZclAttrSmplMetBISCurrNoTier14Block8SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[15][7])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 9
  {gZclAttrSmplMetBISCurrNoTier14Block9SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[15][8])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 10
  {gZclAttrSmplMetBISCurrNoTier14Block10SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[15][9])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 11
  {gZclAttrSmplMetBISCurrNoTier14Block11SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[15][10])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 12
  {gZclAttrSmplMetBISCurrNoTier14Block12SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[15][11])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 13
  {gZclAttrSmplMetBISCurrNoTier14Block13SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[15][12])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 14
  {gZclAttrSmplMetBISCurrNoTier14Block14SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[15][13])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 15
  {gZclAttrSmplMetBISCurrNoTier14Block15SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[15][14])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 16
  {gZclAttrSmplMetBISCurrNoTier14Block16SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[15][15])},
#endif
#endif
#if gASL_ZclMet_BISSetNumberOfTiersDlvr_d >= 16
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 1
  {gZclAttrSmplMetBISCurrNoTier15Block1SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[0][0])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 2
  {gZclAttrSmplMetBISCurrNoTier15Block2SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[0][1])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 3
  {gZclAttrSmplMetBISCurrNoTier15Block3SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[0][2])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 4
  {gZclAttrSmplMetBISCurrNoTier15Block4SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[0][3])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 5
  {gZclAttrSmplMetBISCurrNoTier15Block5SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[0][4])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 6
  {gZclAttrSmplMetBISCurrNoTier15Block6SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[0][5])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 7
  {gZclAttrSmplMetBISCurrNoTier15Block7SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[0][6])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 8
  {gZclAttrSmplMetBISCurrNoTier15Block8SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[0][7])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 9
  {gZclAttrSmplMetBISCurrNoTier15Block9SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[0][8])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 10
  {gZclAttrSmplMetBISCurrNoTier15Block10SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[0][9])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 11
  {gZclAttrSmplMetBISCurrNoTier15Block11SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[0][10])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 12
  {gZclAttrSmplMetBISCurrNoTier15Block12SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[0][11])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 13
  {gZclAttrSmplMetBISCurrNoTier15Block13SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[0][12])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 14
  {gZclAttrSmplMetBISCurrNoTier15Block14SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[0][13])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 15
  {gZclAttrSmplMetBISCurrNoTier15Block15SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[0][14])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 16
  {gZclAttrSmplMetBISCurrNoTier15Block16SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummDlvrd[0][15])},
#endif
#endif
/*Attributes of the Simple Metering Block Information Received Attribute Set */
#if gASL_ZclMet_BISSetNumberOfTiersRcvd_d >= 1
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 1
  {gZclAttrSmplMetBISCurrNoTierBlock1SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[0][0])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 2
  {gZclAttrSmplMetBISCurrNoTierBlock2SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[0][1])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 3
  {gZclAttrSmplMetBISCurrNoTierBlock3SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[0][2])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 4
  {gZclAttrSmplMetBISCurrNoTierBlock4SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[0][3])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 5
  {gZclAttrSmplMetBISCurrNoTierBlock5SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[0][4])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 6
  {gZclAttrSmplMetBISCurrNoTierBlock6SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[0][5])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 7
  {gZclAttrSmplMetBISCurrNoTierBlock7SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[0][6])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 8
  {gZclAttrSmplMetBISCurrNoTierBlock8SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[0][7])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 9
  {gZclAttrSmplMetBISCurrNoTierBlock9SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[0][8])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 10
  {gZclAttrSmplMetBISCurrNoTierBlock10SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[0][9])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 11
  {gZclAttrSmplMetBISCurrNoTierBlock11SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[0][10])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 12
  {gZclAttrSmplMetBISCurrNoTierBlock12SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[0][11])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 13
  {gZclAttrSmplMetBISCurrNoTierBlock13SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[0][12])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 14
  {gZclAttrSmplMetBISCurrNoTierBlock14SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[0][13])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 15
  {gZclAttrSmplMetBISCurrNoTierBlock15SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[0][14])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 16
  {gZclAttrSmplMetBISCurrNoTierBlock16SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[0][15])},
#endif
#endif
#if gASL_ZclMet_BISSetNumberOfTiersRcvd_d >= 2
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 1
  {gZclAttrSmplMetBISCurrNoTier1Block1SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[1][0])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 2
  {gZclAttrSmplMetBISCurrNoTier1Block2SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[1][1])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 3
  {gZclAttrSmplMetBISCurrNoTier1Block3SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[1][2])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 4
  {gZclAttrSmplMetBISCurrNoTier1Block4SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[1][3])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 5
  {gZclAttrSmplMetBISCurrNoTier1Block5SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[1][4])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 6
  {gZclAttrSmplMetBISCurrNoTier1Block6SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[1][5])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 7
  {gZclAttrSmplMetBISCurrNoTier1Block7SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[1][6])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 8
  {gZclAttrSmplMetBISCurrNoTier1Block8SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[1][7])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 9
  {gZclAttrSmplMetBISCurrNoTier1Block9SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[1][8])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 10
  {gZclAttrSmplMetBISCurrNoTier1Block10SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[1][9])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 11
  {gZclAttrSmplMetBISCurrNoTier1Block11SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[1][10])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 12
  {gZclAttrSmplMetBISCurrNoTier1Block12SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[1][11])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 13
  {gZclAttrSmplMetBISCurrNoTier1Block13SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[1][12])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 14
  {gZclAttrSmplMetBISCurrNoTier1Block14SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[1][13])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 15
  {gZclAttrSmplMetBISCurrNoTier1Block15SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[1][14])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 16
  {gZclAttrSmplMetBISCurrNoTier1Block16SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[1][15])},
#endif
#endif
#if gASL_ZclMet_BISSetNumberOfTiersRcvd_d >= 3
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 1
  {gZclAttrSmplMetBISCurrNoTier2Block1SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[2][0])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 2
  {gZclAttrSmplMetBISCurrNoTier2Block2SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[2][1])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 3
  {gZclAttrSmplMetBISCurrNoTier2Block3SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[2][2])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 4
  {gZclAttrSmplMetBISCurrNoTier2Block4SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[2][3])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 5
  {gZclAttrSmplMetBISCurrNoTier2Block5SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[2][4])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 6
  {gZclAttrSmplMetBISCurrNoTier2Block6SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[2][5])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 7
  {gZclAttrSmplMetBISCurrNoTier2Block7SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[2][6])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 8
  {gZclAttrSmplMetBISCurrNoTier2Block8SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[2][7])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 9
  {gZclAttrSmplMetBISCurrNoTier2Block9SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[2][8])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 10
  {gZclAttrSmplMetBISCurrNoTier2Block10SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[2][9])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 11
  {gZclAttrSmplMetBISCurrNoTier2Block11SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[2][10])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 12
  {gZclAttrSmplMetBISCurrNoTier2Block12SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[2][11])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 13
  {gZclAttrSmplMetBISCurrNoTier2Block13SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[2][12])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 14
  {gZclAttrSmplMetBISCurrNoTier2Block14SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[2][13])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 15
  {gZclAttrSmplMetBISCurrNoTier2Block15SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[2][14])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 16
  {gZclAttrSmplMetBISCurrNoTier2Block16SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[2][15])},
#endif
#endif
#if gASL_ZclMet_BISSetNumberOfTiersRcvd_d >= 4
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 1
  {gZclAttrSmplMetBISCurrNoTier3Block1SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[3][0])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 2
  {gZclAttrSmplMetBISCurrNoTier3Block2SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[3][1])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 3
  {gZclAttrSmplMetBISCurrNoTier3Block3SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[3][2])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 4
  {gZclAttrSmplMetBISCurrNoTier3Block4SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[3][3])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 5
  {gZclAttrSmplMetBISCurrNoTier3Block5SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[3][4])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 6
  {gZclAttrSmplMetBISCurrNoTier3Block6SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[3][5])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 7
  {gZclAttrSmplMetBISCurrNoTier3Block7SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[3][6])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 8
  {gZclAttrSmplMetBISCurrNoTier3Block8SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[3][7])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 9
  {gZclAttrSmplMetBISCurrNoTier3Block9SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[3][8])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 10
  {gZclAttrSmplMetBISCurrNoTier3Block10SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[3][9])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 11
  {gZclAttrSmplMetBISCurrNoTier3Block11SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[3][10])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 12
  {gZclAttrSmplMetBISCurrNoTier3Block12SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[3][11])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 13
  {gZclAttrSmplMetBISCurrNoTier3Block13SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[3][12])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 14
  {gZclAttrSmplMetBISCurrNoTier3Block14SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[3][13])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 15
  {gZclAttrSmplMetBISCurrNoTier3Block15SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[3][14])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 16
  {gZclAttrSmplMetBISCurrNoTier3Block16SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[3][15])},
#endif
#endif
#if gASL_ZclMet_BISSetNumberOfTiersRcvd_d >= 5
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 1
  {gZclAttrSmplMetBISCurrNoTier4Block1SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[4][0])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 2
  {gZclAttrSmplMetBISCurrNoTier4Block2SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[4][1])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 3
  {gZclAttrSmplMetBISCurrNoTier4Block3SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[4][2])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 4
  {gZclAttrSmplMetBISCurrNoTier4Block4SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[4][3])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 5
  {gZclAttrSmplMetBISCurrNoTier4Block5SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[4][4])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 6
  {gZclAttrSmplMetBISCurrNoTier4Block6SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[4][5])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 7
  {gZclAttrSmplMetBISCurrNoTier4Block7SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[4][6])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 8
  {gZclAttrSmplMetBISCurrNoTier4Block8SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[4][7])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 9
  {gZclAttrSmplMetBISCurrNoTier4Block9SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[4][8])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 10
  {gZclAttrSmplMetBISCurrNoTier4Block10SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[4][9])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 11
  {gZclAttrSmplMetBISCurrNoTier4Block11SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[4][10])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 12
  {gZclAttrSmplMetBISCurrNoTier4Block12SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[4][11])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 13
  {gZclAttrSmplMetBISCurrNoTier4Block13SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[4][12])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 14
  {gZclAttrSmplMetBISCurrNoTier4Block14SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[4][13])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 15
  {gZclAttrSmplMetBISCurrNoTier4Block15SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[4][14])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 16
  {gZclAttrSmplMetBISCurrNoTier4Block16SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[4][15])},
#endif
#endif
#if gASL_ZclMet_BISSetNumberOfTiersRcvd_d >= 6
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 1
  {gZclAttrSmplMetBISCurrNoTier5Block1SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[5][0])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 2
  {gZclAttrSmplMetBISCurrNoTier5Block2SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[5][1])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 3
  {gZclAttrSmplMetBISCurrNoTier5Block3SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[5][2])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 4
  {gZclAttrSmplMetBISCurrNoTier5Block4SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[5][3])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 5
  {gZclAttrSmplMetBISCurrNoTier5Block5SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[5][4])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 6
  {gZclAttrSmplMetBISCurrNoTier5Block6SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[5][5])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 7
  {gZclAttrSmplMetBISCurrNoTier5Block7SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[5][6])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 8
  {gZclAttrSmplMetBISCurrNoTier5Block8SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[5][7])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 9
  {gZclAttrSmplMetBISCurrNoTier5Block9SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[5][8])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 10
  {gZclAttrSmplMetBISCurrNoTier5Block10SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[5][9])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 11
  {gZclAttrSmplMetBISCurrNoTier5Block11SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[5][10])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 12
  {gZclAttrSmplMetBISCurrNoTier5Block12SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[5][11])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 13
  {gZclAttrSmplMetBISCurrNoTier5Block13SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[5][12])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 14
  {gZclAttrSmplMetBISCurrNoTier5Block14SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[5][13])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 15
  {gZclAttrSmplMetBISCurrNoTier5Block15SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[5][14])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 16
  {gZclAttrSmplMetBISCurrNoTier5Block16SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[5][15])},
#endif
#endif
#if gASL_ZclMet_BISSetNumberOfTiersRcvd_d >= 7
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 1
  {gZclAttrSmplMetBISCurrNoTier6Block1SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[6][0])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 2
  {gZclAttrSmplMetBISCurrNoTier6Block2SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[6][1])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 3
  {gZclAttrSmplMetBISCurrNoTier6Block3SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[6][2])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 4
  {gZclAttrSmplMetBISCurrNoTier6Block4SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[6][3])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 5
  {gZclAttrSmplMetBISCurrNoTier6Block5SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[6][4])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 6
  {gZclAttrSmplMetBISCurrNoTier6Block6SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[6][5])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 7
  {gZclAttrSmplMetBISCurrNoTier6Block7SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[6][6])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 8
  {gZclAttrSmplMetBISCurrNoTier6Block8SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[6][7])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 9
  {gZclAttrSmplMetBISCurrNoTier6Block9SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[6][8])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 10
  {gZclAttrSmplMetBISCurrNoTier6Block10SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[6][9])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 11
  {gZclAttrSmplMetBISCurrNoTier6Block11SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[6][10])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 12
  {gZclAttrSmplMetBISCurrNoTier6Block12SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[6][11])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 13
  {gZclAttrSmplMetBISCurrNoTier6Block13SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[6][12])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 14
  {gZclAttrSmplMetBISCurrNoTier6Block14SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[6][13])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 15
  {gZclAttrSmplMetBISCurrNoTier6Block15SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[6][14])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 16
  {gZclAttrSmplMetBISCurrNoTier6Block16SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[6][15])},
#endif
#endif
#if gASL_ZclMet_BISSetNumberOfTiersRcvd_d >= 8
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 1
  {gZclAttrSmplMetBISCurrNoTier7Block1SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[7][0])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 2
  {gZclAttrSmplMetBISCurrNoTier7Block2SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[7][1])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 3
  {gZclAttrSmplMetBISCurrNoTier7Block3SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[7][2])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 4
  {gZclAttrSmplMetBISCurrNoTier7Block4SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[7][3])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 5
  {gZclAttrSmplMetBISCurrNoTier7Block5SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[7][4])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 6
  {gZclAttrSmplMetBISCurrNoTier7Block6SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[7][5])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 7
  {gZclAttrSmplMetBISCurrNoTier7Block7SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[7][6])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 8
  {gZclAttrSmplMetBISCurrNoTier7Block8SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[7][7])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 9
  {gZclAttrSmplMetBISCurrNoTier7Block9SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[7][8])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 10
  {gZclAttrSmplMetBISCurrNoTier7Block10SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[7][9])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 11
  {gZclAttrSmplMetBISCurrNoTier7Block11SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[7][10])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 12
  {gZclAttrSmplMetBISCurrNoTier7Block12SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[7][11])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 13
  {gZclAttrSmplMetBISCurrNoTier7Block13SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[7][12])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 14
  {gZclAttrSmplMetBISCurrNoTier7Block14SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[7][13])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 15
  {gZclAttrSmplMetBISCurrNoTier7Block15SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[7][14])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 16
  {gZclAttrSmplMetBISCurrNoTier7Block16SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[7][15])},
#endif
#endif
#if gASL_ZclMet_BISSetNumberOfTiersRcvd_d >= 9
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 1
  {gZclAttrSmplMetBISCurrNoTier8Block1SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[8][0])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 2
  {gZclAttrSmplMetBISCurrNoTier8Block2SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[8][1])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 3
  {gZclAttrSmplMetBISCurrNoTier8Block3SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[8][2])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 4
  {gZclAttrSmplMetBISCurrNoTier8Block4SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[8][3])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 5
  {gZclAttrSmplMetBISCurrNoTier8Block5SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[8][4])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 6
  {gZclAttrSmplMetBISCurrNoTier8Block6SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[8][5])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 7
  {gZclAttrSmplMetBISCurrNoTier8Block7SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[8][6])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 8
  {gZclAttrSmplMetBISCurrNoTier8Block8SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[8][7])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 9
  {gZclAttrSmplMetBISCurrNoTier8Block9SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[8][8])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 10
  {gZclAttrSmplMetBISCurrNoTier8Block10SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[8][9])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 11
  {gZclAttrSmplMetBISCurrNoTier8Block11SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[8][10])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 12
  {gZclAttrSmplMetBISCurrNoTier8Block12SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[8][11])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 13
  {gZclAttrSmplMetBISCurrNoTier8Block13SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[8][12])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 14
  {gZclAttrSmplMetBISCurrNoTier8Block14SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[8][13])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 15
  {gZclAttrSmplMetBISCurrNoTier8Block15SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[8][14])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 16
  {gZclAttrSmplMetBISCurrNoTier8Block16SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[8][15])},
#endif
#endif
#if gASL_ZclMet_BISSetNumberOfTiersRcvd_d >= 10
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 1
  {gZclAttrSmplMetBISCurrNoTier9Block1SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[9][0])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 2
  {gZclAttrSmplMetBISCurrNoTier9Block2SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[9][1])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 3
  {gZclAttrSmplMetBISCurrNoTier9Block3SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[9][2])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 4
  {gZclAttrSmplMetBISCurrNoTier9Block4SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[9][3])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 5
  {gZclAttrSmplMetBISCurrNoTier9Block5SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[9][4])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 6
  {gZclAttrSmplMetBISCurrNoTier9Block6SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[9][5])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 7
  {gZclAttrSmplMetBISCurrNoTier9Block7SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[9][6])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 8
  {gZclAttrSmplMetBISCurrNoTier9Block8SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[9][7])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 9
  {gZclAttrSmplMetBISCurrNoTier9Block9SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[9][8])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 10
  {gZclAttrSmplMetBISCurrNoTier9Block10SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[9][9])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 11
  {gZclAttrSmplMetBISCurrNoTier9Block11SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[9][10])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 12
  {gZclAttrSmplMetBISCurrNoTier9Block12SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[9][11])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 13
  {gZclAttrSmplMetBISCurrNoTier9Block13SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[9][12])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 14
  {gZclAttrSmplMetBISCurrNoTier9Block14SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[9][13])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 15
  {gZclAttrSmplMetBISCurrNoTier9Block15SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[9][14])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 16
  {gZclAttrSmplMetBISCurrNoTier9Block16SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[9][15])},
#endif
#endif
#if gASL_ZclMet_BISSetNumberOfTiersRcvd_d >= 11
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 1
  {gZclAttrSmplMetBISCurrNoTier10Block1SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[10][0])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 2
  {gZclAttrSmplMetBISCurrNoTier10Block2SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[10][1])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 3
  {gZclAttrSmplMetBISCurrNoTier10Block3SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[10][2])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 4
  {gZclAttrSmplMetBISCurrNoTier10Block4SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[10][3])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 5
  {gZclAttrSmplMetBISCurrNoTier10Block5SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[10][4])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 6
  {gZclAttrSmplMetBISCurrNoTier10Block6SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[10][5])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 7
  {gZclAttrSmplMetBISCurrNoTier10Block7SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[10][6])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 8
  {gZclAttrSmplMetBISCurrNoTier10Block8SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[10][7])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 9
  {gZclAttrSmplMetBISCurrNoTier10Block9SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[10][8])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 10
  {gZclAttrSmplMetBISCurrNoTier10Block10SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[10][9])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 11
  {gZclAttrSmplMetBISCurrNoTier10Block11SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[10][10])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 12
  {gZclAttrSmplMetBISCurrNoTier10Block12SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[10][11])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 13
  {gZclAttrSmplMetBISCurrNoTier10Block13SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[10][12])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 14
  {gZclAttrSmplMetBISCurrNoTier10Block14SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[10][13])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 15
  {gZclAttrSmplMetBISCurrNoTier10Block15SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[10][14])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 16
  {gZclAttrSmplMetBISCurrNoTier10Block16SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[10][15])},
#endif
#endif
#if gASL_ZclMet_BISSetNumberOfTiersRcvd_d >= 12
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 1
  {gZclAttrSmplMetBISCurrNoTier11Block1SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[12][0])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 2
  {gZclAttrSmplMetBISCurrNoTier11Block2SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[12][1])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 3
  {gZclAttrSmplMetBISCurrNoTier11Block3SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[12][2])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 4
  {gZclAttrSmplMetBISCurrNoTier11Block4SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[12][3])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 5
  {gZclAttrSmplMetBISCurrNoTier11Block5SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[12][4])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 6
  {gZclAttrSmplMetBISCurrNoTier11Block6SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[12][5])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 7
  {gZclAttrSmplMetBISCurrNoTier11Block7SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[12][6])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 8
  {gZclAttrSmplMetBISCurrNoTier11Block8SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[12][7])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 9
  {gZclAttrSmplMetBISCurrNoTier11Block9SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[12][8])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 10
  {gZclAttrSmplMetBISCurrNoTier11Block10SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[12][9])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 11
  {gZclAttrSmplMetBISCurrNoTier11Block11SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[12][10])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 12
  {gZclAttrSmplMetBISCurrNoTier11Block12SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[12][11])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 13
  {gZclAttrSmplMetBISCurrNoTier11Block13SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[12][12])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 14
  {gZclAttrSmplMetBISCurrNoTier11Block14SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[12][13])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 15
  {gZclAttrSmplMetBISCurrNoTier11Block15SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[12][14])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 16
  {gZclAttrSmplMetBISCurrNoTier11Block16SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[12][15])},
#endif
#endif
#if gASL_ZclMet_BISSetNumberOfTiersRcvd_d >= 13
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 1
  {gZclAttrSmplMetBISCurrNoTier12Block1SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[13][0])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 2
  {gZclAttrSmplMetBISCurrNoTier12Block2SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[13][1])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 3
  {gZclAttrSmplMetBISCurrNoTier12Block3SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[13][2])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 4
  {gZclAttrSmplMetBISCurrNoTier12Block4SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[13][3])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 5
  {gZclAttrSmplMetBISCurrNoTier12Block5SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[13][4])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 6
  {gZclAttrSmplMetBISCurrNoTier12Block6SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[13][5])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 7
  {gZclAttrSmplMetBISCurrNoTier12Block7SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[13][6])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 8
  {gZclAttrSmplMetBISCurrNoTier12Block8SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[13][7])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 9
  {gZclAttrSmplMetBISCurrNoTier12Block9SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[13][8])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 10
  {gZclAttrSmplMetBISCurrNoTier12Block10SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[13][9])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 11
  {gZclAttrSmplMetBISCurrNoTier12Block11SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[13][10])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 12
  {gZclAttrSmplMetBISCurrNoTier12Block12SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[13][11])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 13
  {gZclAttrSmplMetBISCurrNoTier12Block13SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[13][12])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 14
  {gZclAttrSmplMetBISCurrNoTier12Block14SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[13][13])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 15
  {gZclAttrSmplMetBISCurrNoTier12Block15SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[13][14])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 16
  {gZclAttrSmplMetBISCurrNoTier12Block16SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[13][15])},
#endif
#endif
#if gASL_ZclMet_BISSetNumberOfTiersRcvd_d >= 14
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 1
  {gZclAttrSmplMetBISCurrNoTier13Block1SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[14][0])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 2
  {gZclAttrSmplMetBISCurrNoTier13Block2SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[14][1])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 3
  {gZclAttrSmplMetBISCurrNoTier13Block3SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[14][2])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 4
  {gZclAttrSmplMetBISCurrNoTier13Block4SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[14][3])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 5
  {gZclAttrSmplMetBISCurrNoTier13Block5SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[14][4])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 6
  {gZclAttrSmplMetBISCurrNoTier13Block6SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[14][5])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 7
  {gZclAttrSmplMetBISCurrNoTier13Block7SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[14][6])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 8
  {gZclAttrSmplMetBISCurrNoTier13Block8SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[14][7])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 9
  {gZclAttrSmplMetBISCurrNoTier13Block9SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[14][8])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 10
  {gZclAttrSmplMetBISCurrNoTier13Block10SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[14][9])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 11
  {gZclAttrSmplMetBISCurrNoTier13Block11SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[14][10])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 12
  {gZclAttrSmplMetBISCurrNoTier13Block12SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[14][11])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 13
  {gZclAttrSmplMetBISCurrNoTier13Block13SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[14][12])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 14
  {gZclAttrSmplMetBISCurrNoTier13Block14SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[14][13])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 15
  {gZclAttrSmplMetBISCurrNoTier13Block15SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[14][14])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 16
  {gZclAttrSmplMetBISCurrNoTier13Block16SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[14][15])},
#endif
#endif
#if gASL_ZclMet_BISSetNumberOfTiersRcvd_d >= 15
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 1
  {gZclAttrSmplMetBISCurrNoTier14Block1SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[15][0])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 2
  {gZclAttrSmplMetBISCurrNoTier14Block2SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[15][1])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 3
  {gZclAttrSmplMetBISCurrNoTier14Block3SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[15][2])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 4
  {gZclAttrSmplMetBISCurrNoTier14Block4SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[15][3])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 5
  {gZclAttrSmplMetBISCurrNoTier14Block5SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[15][4])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 6
  {gZclAttrSmplMetBISCurrNoTier14Block6SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[15][5])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 7
  {gZclAttrSmplMetBISCurrNoTier14Block7SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[15][6])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 8
  {gZclAttrSmplMetBISCurrNoTier14Block8SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[15][7])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 9
  {gZclAttrSmplMetBISCurrNoTier14Block9SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[15][8])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 10
  {gZclAttrSmplMetBISCurrNoTier14Block10SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[15][9])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 11
  {gZclAttrSmplMetBISCurrNoTier14Block11SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[15][10])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 12
  {gZclAttrSmplMetBISCurrNoTier14Block12SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[15][11])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 13
  {gZclAttrSmplMetBISCurrNoTier14Block13SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[15][12])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 14
  {gZclAttrSmplMetBISCurrNoTier14Block14SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[15][13])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 15
  {gZclAttrSmplMetBISCurrNoTier14Block15SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[15][14])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 16
  {gZclAttrSmplMetBISCurrNoTier14Block16SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[15][15])},
#endif
#endif
#if gASL_ZclMet_BISSetNumberOfTiersRcvd_d >= 16
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 1
  {gZclAttrSmplMetBISCurrNoTier15Block1SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[0][0])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 2
  {gZclAttrSmplMetBISCurrNoTier15Block2SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[0][1])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 3
  {gZclAttrSmplMetBISCurrNoTier15Block3SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[0][2])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 4
  {gZclAttrSmplMetBISCurrNoTier15Block4SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[0][3])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 5
  {gZclAttrSmplMetBISCurrNoTier15Block5SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[0][4])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 6
  {gZclAttrSmplMetBISCurrNoTier15Block6SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[0][5])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 7
  {gZclAttrSmplMetBISCurrNoTier15Block7SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[0][6])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 8
  {gZclAttrSmplMetBISCurrNoTier15Block8SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[0][7])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 9
  {gZclAttrSmplMetBISCurrNoTier15Block9SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[0][8])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 10
  {gZclAttrSmplMetBISCurrNoTier15Block10SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[0][9])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 11
  {gZclAttrSmplMetBISCurrNoTier15Block11SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[0][10])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 12
  {gZclAttrSmplMetBISCurrNoTier15Block12SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[0][11])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 13
  {gZclAttrSmplMetBISCurrNoTier15Block13SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[0][12])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 14
  {gZclAttrSmplMetBISCurrNoTier15Block14SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[0][13])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 15
  {gZclAttrSmplMetBISCurrNoTier15Block15SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[0][14])},
#endif
#if gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >= 16
  {gZclAttrSmplMetBISCurrNoTier15Block16SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Summ_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,CurrNoTierBlockSummRcvd[0][15])},
#endif
#endif
  /*Attributes of the Simple Metering Billing Attribute Set */
  {gZclAttrSmplMetBillingBillToDate_c,   gZclDataTypeUint32_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Bill_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,BillToDate)},
  {gZclAttrSmplMetBillingBillToDateTimeStamp_c,   gZclDataTypeUTCTime_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Bill_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,BillToDateTimeStamp)},
  {gZclAttrSmplMetBillingProjectedBill_c,   gZclDataTypeUint32_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Bill_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,ProjectedBill)},
  {gZclAttrSmplMetBillingProjectedBillTimeStamp_c,   gZclDataTypeUTCTime_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(Bill_t), (void *) MbrOfs(ZCLSmplMetServerAttrsRAM_t,ProjectedBillTimeStamp)},
#endif
};

const zclAttrDefList_t gZclSmplMetAttrDefList = {
  NumberOfElements(gaZclSmplMetServerAttrDef),
  gaZclSmplMetServerAttrDef
};

const zclAttrDef_t gaZclSmplMetServerMirrorAttrDef[] = {
  { gZclAttrSmplMetRISCurrSummDlvrd_c,        gZclDataTypeUint48_c,  gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(Summ_t),    (void *)MbrOfs(seESIMirrorMeteringData_t, RISCurrSummDlvrd) },
  { gZclAttrSmplMetMSStatus_c,                gZclDataTypeBitmap8_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint8_t),   (void *)MbrOfs(seESIMirrorMeteringData_t, MSStatus)  },
  { gZclAttrSmplMetCFSUnitofMeasure_c,        gZclDataTypeEnum8_c,   gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint8_t),   (void *)MbrOfs(seESIMirrorMeteringData_t, SmplMetCFSUnitofMeasure) },
  {gZclAttrSmplMetCFSSummFormat_c,            gZclDataTypeBitmap8_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint8_t),   (void *)MbrOfs(seESIMirrorMeteringData_t, CFSSummFormat) },    
  {gZclAttrSmplMetCFSMetDevType_c,            gZclDataTypeBitmap8_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint8_t),   (void *)MbrOfs(seESIMirrorMeteringData_t, CFSMetDevType) },   
};

const zclAttrDefList_t gZclSmplMetMirrorAttrDefList = {
  NumberOfElements(gaZclSmplMetServerMirrorAttrDef),
  gaZclSmplMetServerMirrorAttrDef
};

/*******************************************************************************/

void ZCL_SmplMet_MeterInit(void)
{
  /*A timer is used for demo purpose to generate consumption data.*/
  gGetProfileTestTimerID = TMR_AllocateTimer();
  gUpdatePowerConsumptionTimerID = TMR_AllocateTimer();
  TMR_StartSecondTimer(gUpdatePowerConsumptionTimerID, gUpdateConsumption_c, UpdatePowerConsumptionTimerCallBack);
#if(gProfIntrvPeriod_c < 5)
  TMR_StartMinuteTimer(gGetProfileTestTimerID, gTimerValue_c, GetProfileTestTimerCallBack);
#else
  TMR_StartSecondTimer(gGetProfileTestTimerID, gTimerValue_c, GetProfileTestTimerCallBack);
#endif
    
}

/*******************************************************************************/

zbStatus_t ZclSmplMet_ClusterServer
(
	zbApsdeDataIndication_t *pIndication, /* IN: */
	afDeviceDef_t *pDev                /* IN: */
) 
{
  zclCmd_t Cmd;
  zbStatus_t status = gZclSuccessDefaultRsp_c;

  (void) pDev;
  Cmd = ((zclFrame_t *)pIndication->pAsdu)->command;
  
  switch(Cmd) {
    case gZclCmdSmplMet_GetProfReq_c: 
    {
      zclFrame_t *pFrame;
      zclSmplMet_GetProfRsp_t *pRsp;
      zclCmdSmplMet_GetProfReq_t *pCmdFrame;
      pRsp = MSG_Alloc(sizeof(zclSmplMet_GetProfRsp_t) + (gMaxNumberOfPeriodsDelivered_c*3) );
      if(pRsp)
      {
        /* prepare for response in the address info (back to sender) */
        AF_PrepareForReply(&pRsp->addrInfo, pIndication);
        pFrame = (void *)pIndication->pAsdu;
        pRsp->zclTransactionId = pFrame->transactionId;
        pCmdFrame = (void *)(pFrame + 1);
        /* Buildng the response */
        ZCL_BuildGetProfResponse(pCmdFrame->IntrvChannel, pCmdFrame->EndTime, pCmdFrame->NumOfPeriods, &pRsp->cmdFrame);
        status = gZclSuccess_c;
        (void) ZclSmplMet_GetProfRsp(pRsp);        
        
        MSG_Free(pRsp);
      }
    }
    break;  
#if gZclMirroring_d   
    
  case gZclCmdSmplMet_RequestMirrorRsp_c:
    {
      zclCmdMet_RequestMirrorRsp_t *pCmdFrame = (void *)(pIndication->pAsdu + sizeof(zclFrame_t));
#if gZclEnableReporting_c      
      if (pCmdFrame->EndPointID != 0xFFFF)    
        ZclMet_ConfigureReportingToMirror(pIndication, (uint8_t)OTA2Native16(pCmdFrame->EndPointID));
#else
      (void)pCmdFrame;      
#endif      
      status = gZbSuccess_c;
    }
    break;
    
  case gZclCmdSmplMet_RemoveMirrorReq_c:
    /* Disable Reporting */    
#if gZclEnableReporting_c
    gZclReportingSetup.fReporting = FALSE;
    TMR_StopTimer(gZclReportingTimerID);    
#endif
    status = gZbSuccess_c;    
    break;
    
#endif    
  case gZclCmdSmplMet_ReqFastPollModeReq_c:
    status = ZCL_SmplMet_ProcessReqFastPollEvt(pIndication);
    break;
  case gZclCmdSmplMet_GetSnapshot_Req_c:
    status = ZclSmplMet_ProcessGetSnapshotReq(pIndication);
    break;
  default:
    status = gZclUnsupportedClusterCommand_c;
    break;
  }
  
  return status;
}

/*******************************************************************************/

zbStatus_t ZclSmplMet_ClusterClient
  (
  zbApsdeDataIndication_t *pIndication, 
  afDeviceDef_t *pDevice
  )
{
  zclFrame_t *pFrame;
  zclCmd_t command;
  
  /* prevent compiler warning */
  (void)pDevice;
  pFrame = (void *)pIndication->pAsdu;
  /* handle the command */
  command = pFrame->command;
  
  switch(command) {
    case gZclCmdSmplMet_GetProfRsp_c:
      return gZbSuccess_c;
#if gZclMirroring_d      
    case gZclCmdSmplMet_RequestMirrorReq_c:
      {
        zclMet_RequestMirrorRsp_t *pRsp;
        zbEndPoint_t mirrorEp = ZclMet_CreateAndRegisterMirrorEndpoint(pIndication->aSrcAddr);

        pRsp = MSG_Alloc(sizeof(zclMet_RequestMirrorRsp_t));
        if(pRsp)
        {
          /* Prepare for response in the address info (back to sender) */
          AF_PrepareForReply(&pRsp->addrInfo, pIndication);
          pRsp->zclTransactionId = pFrame->transactionId;
          
          /* Build the response */
          if(mirrorEp != 0xFF)
            pRsp->cmdFrame.EndPointID = Native2OTA16(mirrorEp);
          else
            pRsp->cmdFrame.EndPointID = 0xFFFF;
          
          /* Send the response*/
          (void)ZclMet_RequestMirrorRsp(pRsp);          
          MSG_Free(pRsp);
          return gZbSuccess_c;
        }
      }    
    case gZclCmdSmplMet_MirrorRemovedRsp_c:
      {
        zclMet_MirrorRemovedRsp_t *pRsp;
        zclMet_MirroringTable_t* pMTE;        
        
        /* Check address for authorization*/
        pMTE = getMirroringTableEntry(pIndication->aSrcAddr, gInvalidAppEndPoint_c);
        if (!pMTE)
          return gZclNotAuthorized_c;
        
        pRsp = MSG_Alloc(sizeof(zclMet_RequestMirrorRsp_t));
        if(pRsp)
        {
          /* Prepare for response in the address info (back to sender) */
          AF_PrepareForReply(&pRsp->addrInfo, pIndication);
          pRsp->zclTransactionId = pFrame->transactionId;                    
          
          /* Build the response */
          if (AF_DeRegisterEndPoint(endPointList[1].pEndpointDesc->pSimpleDesc->endPoint) == gZbSuccess_c)
          {
            pRsp->cmdFrame.EndPointID = Native2OTA16(endPointList[1].pEndpointDesc->pSimpleDesc->endPoint);                   
            pMTE->isActive = FALSE;
          }
          else
            pRsp->cmdFrame.EndPointID = 0xFFFF;
          
          /* Send the response*/
          (void)ZclMet_MirrorRemovedRsp(pRsp);          
          MSG_Free(pRsp);
          return gZbSuccess_c;
        }
      }    
      
#endif      
#if gZclFastPollMode_d
    case gZclCmdSmplMet_ReqFastPollModeRsp_c:
      return gZbSuccess_c;
#endif
  default:
      return gZclUnsupportedClusterCommand_c;
  }
}

/*******************************************************************************/

zbStatus_t ZclSmplMet_AcceptFastPollModeReq(bool_t acceptFastPollReq)
{
  gAcceptFastPoll = acceptFastPollReq;
  return gZclSuccess_c;
}

/*******************************************************************************/

zbStatus_t ZclSmplMet_ProcessGetSnapshotReq
(
zbApsdeDataIndication_t *pIndication
)
{
  zclCmdSmplMet_GetSnapshotReq_t *pReq;
  
  pReq = (zclCmdSmplMet_GetSnapshotReq_t *) ((uint8_t*)pIndication->pAsdu + sizeof(zclFrame_t));
  (void)pReq;
  
  return gZclUnsupportedClusterCommand_c;
}

/*******************************************************************************/

static zbStatus_t ZCL_SmplMet_ProcessReqFastPollEvt
(
zbApsdeDataIndication_t *pIndication  //IN: pointer to APSDE Data Indication
)
{
#if gZclFastPollMode_d
  uint8_t temp8 = 0;
  zclSmplMet_ReqFastPollModeRsp_t response;
    
  zbStatus_t status = gZclSuccess_c;
  zclCmdSmplMet_ReqFastPollModeReq_t *pMsg;
  /* get the request fast poll mode request */
  pMsg = (zclCmdSmplMet_ReqFastPollModeReq_t *)((uint8_t*)pIndication->pAsdu + sizeof(zclFrame_t));
  
  BeeAppUpdateDevice(pIndication->dstEndPoint, gZclUI_FastPollReqRcv_c, 0, 0, &pMsg);
  
  if(!gAcceptFastPoll)
  {
    response.cmdFrame.appliedUpdatePeriod = 0;
  }
  else
  {
    /*set the values for the response*/
    if(ZCL_GetAttribute(pIndication->dstEndPoint,pIndication->aClusterId,gZclAttrSmplMetRISFastPollUpdatePeriod_c,&temp8, NULL))
    {
      return gZclFailure_c;
    }
    if(pMsg->updatePeriod <= temp8)
    {
      response.cmdFrame.appliedUpdatePeriod = temp8;
    }
    else
    {
      response.cmdFrame.appliedUpdatePeriod = pMsg->updatePeriod;
    }
    /*Calculate mcFastPollRemainingTime*/
    /*if the device hasn't allready reached the maximum time it can stay in fast poll mode, recalculate mcFastPollRemainingTime*/
    if(mcFastPollRemainingTime + mcTimeInFastPollMode < gASL_ZclSmplMet_MaxFastPollInterval_d)
    {
      /*If the device isn't in fast poll mode the timer is started*/
      if(!mcFastPollRemainingTime)
      {
        gFastPollTimerID = TMR_AllocateTimer();
        TMR_StartSecondTimer(gFastPollTimerID, 1, FastPollTimerCallBack);
      }
      /*Can the new required fast poll mode request time be included in the maximum fast poll interval time?*/
      if(pMsg->duration * 60 < gASL_ZclSmplMet_MaxFastPollInterval_d - mcTimeInFastPollMode)
      {
        /*If the new duration is higher than the fast poll remaining time update mcFastPollRemainingTime*/
        if(pMsg->duration * 60 > mcFastPollRemainingTime)
        {
          mcFastPollRemainingTime = pMsg->duration * 60;
        }/*else there is no need to update mcFastPollRemainingTime*/
      }
      else
      {
        mcFastPollRemainingTime = gASL_ZclSmplMet_MaxFastPollInterval_d - mcTimeInFastPollMode;
      }
      mcFastPollEndTime = ZCL_GetUTCTime() + (ZCLTime_t)mcFastPollRemainingTime;
    }/*else there is no need to update mcFastPollRemainingTime*/
  }
  /*mcFastPollEndTime is used instead of ZCL_GetUTCTime() + (ZCLTime_t)mcFastPollRemainingTime because in some cases
  there can be a +/-1second difference in the reported time of 2 responses that should have the same end time*/
  response.cmdFrame.EndTime = Native2OTA32(mcFastPollEndTime);
  /*build the response*/
  AF_PrepareForReply(&response.addrInfo, pIndication);
  response.zclTransactionId = ((zclFrame_t *)pIndication->pAsdu)->transactionId;
  status = ZclSmplMet_ReqFastPollModeRsp(&response);
  return status;
#else
  (void)pIndication;
  (void)mcFastPollEndTime;
  return gZclUnsupportedClusterCommand_c;
#endif
}

/*******************************************************************************/

static void FastPollTimerCallBack
(
tmrTimerID_t tmrID
)
{
  (void)tmrID;
  if(mcFastPollRemainingTime ==0)
  {
    TMR_FreeTimer(gFastPollTimerID);
    mcTimeInFastPollMode = 0;
  }
  else
  {
    mcFastPollRemainingTime--;
    mcTimeInFastPollMode++;
    TMR_StartSecondTimer(gFastPollTimerID, 1, FastPollTimerCallBack);
  }
}

/******************************************************************************
* ZclSmplMet_GetFastPollRemainingTime
*
* Returns
*   fastPollRemainingTime - Remaining time in seconds that the device must maintain
*                           fast poll mode.
*
******************************************************************************/
uint16_t ZclSmplMet_GetFastPollRemainingTime()
{
  return mcFastPollRemainingTime;
}

/*******************************************************************************/

zbStatus_t ZclSmplMet_GetProfReq
(
zclSmplMet_GetProfReq_t *pReq
)
{
  return ZCL_SendClientReqSeqPassed(gZclCmdSmplMet_GetProfReq_c, sizeof(zclCmdSmplMet_GetProfReq_t),(zclGenericReq_t *)pReq);	
}

/*******************************************************************************/

zbStatus_t ZclMet_RequestMirrorReq
(
  zclMet_RequestMirrorReq_t *pReq
)
{
  return ZCL_SendServerReqSeqPassed(gZclCmdSmplMet_RequestMirrorReq_c, 0,(zclGenericReq_t *)pReq);	
}

/*******************************************************************************/

zbStatus_t ZclMet_RemoveMirrorReq
(
  zclMet_RemoveMirrorReq_t *pReq
)
{
  return ZCL_SendServerReqSeqPassed(gZclCmdSmplMet_RemoveMirrorReq_c, 0,(zclGenericReq_t *)pReq);	
}

/*******************************************************************************/

zbStatus_t ZclMet_RequestMirrorRsp
(
  zclMet_RequestMirrorRsp_t *pRsp
)
{
  return ZCL_SendClientReqSeqPassed(gZclCmdSmplMet_RequestMirrorRsp_c, sizeof(zclCmdMet_RequestMirrorRsp_t),(zclGenericReq_t *)pRsp);	
}

/*******************************************************************************/

zbStatus_t ZclMet_MirrorRemovedRsp
(
  zclMet_MirrorRemovedRsp_t *pRsp
)
{
  return ZCL_SendClientReqSeqPassed(gZclCmdSmplMet_MirrorRemovedRsp_c, sizeof(zclCmdMet_MirrorRemovedRsp_t),(zclGenericReq_t *)pRsp);	
}

/*******************************************************************************/

zbStatus_t ZclSmplMet_ReqFastPollModeReq
(
zclSmplMet_ReqFastPollModeReq_t *pReq
)
{
  return ZCL_SendClientReqSeqPassed(gZclCmdSmplMet_ReqFastPollModeReq_c, sizeof(zclCmdSmplMet_ReqFastPollModeReq_t),(zclGenericReq_t *)pReq);	
}

/*******************************************************************************/
void ZCL_BuildGetProfResponse
(   uint8_t IntrvChannel,                 /* IN: */
    ZCLTime_t EndTime,                    /* IN:  note little endian*/
    uint8_t NumberOfPeriods,              /* IN: */
    zclCmdSmplMet_GetProfRsp_t *pProfRsp  /* OUT: */
)
{
  /* Search for entries that match end time in Profile intervals table */ 
  uint8_t *pTmpIntrvs,i,NumOfPeriodsDlvrd=0, SearchPointer;  
  uint32_t endTime;
  bool_t EntryInTable=FALSE, endTimeValid=FALSE, IntrvChannelSupported=FALSE;
  uint8_t status = gSMGetProfRsp_SuccessStatus_c;
  pTmpIntrvs = (uint8_t *)pProfRsp->Intrvs;
  endTime = OTA2Native32(EndTime);  
  if(ProfIntrvHead == 0)
      SearchPointer = gMaxNumberOfPeriodsDelivered_c;
  else
    SearchPointer = ProfIntrvHead-1;
  
  if(ProfIntrvTail > ProfIntrvHead)
    i = ProfIntrvHead + (gMaxNumberOfPeriodsDelivered_c-ProfIntrvTail)+1;
  else
    i = (ProfIntrvHead - ProfIntrvTail);
  
  if(i)
    EntryInTable = TRUE;
  
  for(; i>0 && NumOfPeriodsDlvrd < NumberOfPeriods; --i)
  {
    /* Search the profile table for entries */
    if(ProfileIntervalTable[SearchPointer].IntrvChannel == IntrvChannel)
    {
      IntrvChannelSupported=TRUE;
      /* Check if endTime is valid */
      if(ProfileIntervalTable[SearchPointer].endTime <= endTime || endTime == 0x00000000)
      {
        endTimeValid=TRUE;
        if(NumOfPeriodsDlvrd==0)
          FLib_MemCpy(&pProfRsp->EndTime, &ProfileIntervalTable[SearchPointer].endTime, sizeof(uint32_t));
        NumOfPeriodsDlvrd++;
        FLib_MemCpy(pTmpIntrvs, &ProfileIntervalTable[SearchPointer].Intrv, sizeof(Consmp));
        pTmpIntrvs += sizeof(IntrvForProfiling_t);
      }
      else
      {
        /* endTime is invalid, check next entry */
        endTimeValid=FALSE;
      }
      
    }
 //   else
 //   {
 //     IntrvChannelSupported=FALSE;
 //   }
    if(SearchPointer==0)
    {
      SearchPointer = gMaxNumberOfPeriodsDelivered_c;
    }
    --SearchPointer;
    
  }
 
  pProfRsp->ProfIntrvPeriod = gProfIntrvPeriod_c;
  pProfRsp->NumOfPeriodsDlvrd = NumOfPeriodsDlvrd;
  /* check if status field value is undefined  interval channel requested */
  if((IntrvChannel != gIntrvChannel_ConsmpDlvrd_c) && (IntrvChannel != gIntrvChannel_ConsmpRcvd_c))
     status = gSMGetProfRsp_UndefIntrvChannelStatus_c;
  /* check if status field value is interval channel not supported */
  else if(EntryInTable == TRUE && IntrvChannelSupported == FALSE)
    status = gSMGetProfRsp_IntrvChannelNotSupportedStatus_c;
   /* check if status field value is invalid end time */
  /*else if() 
    status = gSMGetProfRsp_InvalidEndTimeStatus_c;*/  
  /* check if status field value is no intervals available for the requested time */
  else if((EntryInTable == TRUE && IntrvChannelSupported==TRUE && endTimeValid==FALSE)|| 
          (EntryInTable == FALSE))
    status = gSMGetProfRsp_NoIntrvsAvailableStatus_c;
  /* check if status field value is more periods requested than can be returned */
  else if(NumberOfPeriods > NumOfPeriodsDlvrd)
    status = gSMGetProfRsp_MorePeriodsRequestedStatus_c;
  
  pProfRsp->Status=status;

}

/*******************************************************************************/

zbStatus_t ZclSmplMet_GetProfRsp
(
zclSmplMet_GetProfRsp_t *pReq
)
{
  uint8_t length;
  length = MbrOfs(zclCmdSmplMet_GetProfRsp_t,Intrvs) + 
    (pReq->cmdFrame.NumOfPeriodsDlvrd * sizeof(pReq->cmdFrame.Intrvs[0]));
  /* Set fragmented transmission if we cannot fit payload in frame */
  if(length > ApsmeGetMaxAsduLength(0) - sizeof(zclFrame_t))
    pReq->addrInfo.txOptions |= gApsTxOptionFragmentationRequested_c;
  return ZCL_SendServerRspSeqPassed(gZclCmdSmplMet_GetProfRsp_c, length,(zclGenericReq_t *)pReq);
}

/*******************************************************************************/

zbStatus_t ZclSmplMet_ReqFastPollModeRsp
(
zclSmplMet_ReqFastPollModeRsp_t *pReq
)
{
  return ZCL_SendServerRspSeqPassed(gZclCmdSmplMet_ReqFastPollModeRsp_c, sizeof(zclCmdSmplMet_ReqFastPollModeRsp_t),(zclGenericReq_t *)pReq);
}

/*******************************************************************************/

zbStatus_t ZclSmplMet_GetSnapshotReq
(
zclSmplMet_GetSnapshotReq_t *pReq
)
{
  return ZCL_SendClientReqSeqPassed(gZclCmdSmplMet_GetSnapshot_Req_c, sizeof(zclCmdSmplMet_GetSnapshotReq_t),(zclGenericReq_t *)pReq);	
}

/*******************************************************************************/

void GetProfileTestTimerCallBack(tmrTimerID_t tmrID)
{
  Consmp *consumption;
  Summ_t newRISCurrSummDlvrd;
  zbClusterId_t ClusterId={gaZclClusterSmplMet_c}; 
  (void)tmrID;
  
  ProfileIntervalTable[ProfIntrvHead].IntrvChannel = gIntrvChannel_ConsmpDlvrd_c;
  ProfileIntervalTable[ProfIntrvHead].endTime = ZCL_GetUTCTime();
  /* Simulate used power from last readings */
  (void)ZCL_GetAttribute(appEndPoint, ClusterId, gZclAttrSmplMetRISCurrSummDlvrd_c, &newRISCurrSummDlvrd, NULL);
  consumption = CalculateConsumptionFrom6ByteArray( (uint8_t *)&newRISCurrSummDlvrd, (uint8_t *)&LastRISCurrSummDlvrd);
  FLib_MemCpy(&LastRISCurrSummDlvrd, &newRISCurrSummDlvrd, sizeof(Summ_t));
  
  FLib_MemCpy(&ProfileIntervalTable[ProfIntrvHead++].Intrv, consumption, sizeof(Consmp));
  
  if(ProfIntrvHead >= gMaxNumberOfPeriodsDelivered_c)
    ProfIntrvHead=0;
  if(ProfIntrvHead == ProfIntrvTail)
  {
    ++ProfIntrvTail;
    if(ProfIntrvTail >= gMaxNumberOfPeriodsDelivered_c)
      ProfIntrvTail=0;
  }
  
/* Start the GetProfile test timer */
#if(gProfIntrvPeriod_c < 5)
  TMR_StartMinuteTimer(gGetProfileTestTimerID, gTimerValue_c, GetProfileTestTimerCallBack);
#else
  TMR_StartSecondTimer(gGetProfileTestTimerID, gTimerValue_c, GetProfileTestTimerCallBack);
#endif
}

/*******************************************************************************/

void UpdatePowerConsumptionTimerCallBack(tmrTimerID_t tmrID)
{
  UpdatePowerConsumption();
  (void)tmrID; /* Unused parameter. */
}

/*******************************************************************************/

void UpdatePowerConsumption(void)
{
  Summ_t newRISCurrSummDlvrd;
  uint8_t randomRange;
  uint8_t rangeIndexMax, rangeIndexMin;
  zbClusterId_t ClusterId={gaZclClusterSmplMet_c}; 
  /* This is only for demo, the intervals should be calculated by reading the meter and finding the used power from last reading */
  (void)ZCL_GetAttribute(appEndPoint, ClusterId, gZclAttrSmplMetRISCurrSummDlvrd_c, &newRISCurrSummDlvrd, NULL);  
  #if(gProfIntrvPeriod_c == gZclSEProfIntrvPeriod_Daily_c)
      rangeIndexMin=150;
      rangeIndexMax=255;
  #elif(gProfIntrvPeriod_c == gZclSEProfIntrvPeriod_60mins_c)
      rangeIndexMin=120;
      rangeIndexMax=224;
  #elif(gProfIntrvPeriod_c == gZclSEProfIntrvPeriod_30mins_c)  
      rangeIndexMin=80;
      rangeIndexMax=192;
  #elif(gProfIntrvPeriod_c == gZclSEProfIntrvPeriod_15mins_c)    
      rangeIndexMin=50;
      rangeIndexMax=160;
  #elif(gProfIntrvPeriod_c == gZclSEProfIntrvPeriod_10mins_c)      
      rangeIndexMin=30;
      rangeIndexMax=128;
  #elif(gProfIntrvPeriod_c == gZclSEProfIntrvPeriod_7dot5mins_c)
      rangeIndexMin=15;
      rangeIndexMax=96;
  #elif(gProfIntrvPeriod_c == gZclSEProfIntrvPeriod_5mins_c)
      rangeIndexMin=10;
      rangeIndexMax=64;
  #else(gProfIntrvPeriod_c == gZclSEProfIntrvPeriod_2dot5mins_c)  
      rangeIndexMin=0;
      rangeIndexMax=32;
  #endif
  
  randomRange = GetRandomRange(rangeIndexMin, rangeIndexMax);
  Add8ByteTo6ByteArray(randomRange, (uint8_t *)&newRISCurrSummDlvrd);
  (void)ZCL_SetAttribute(appEndPoint, ClusterId, gZclAttrSmplMetRISCurrSummDlvrd_c, &newRISCurrSummDlvrd);

  TMR_StartSecondTimer(gUpdatePowerConsumptionTimerID, gUpdateConsumption_c, UpdatePowerConsumptionTimerCallBack);  
}

/*****************************************************************************/

static Consmp *CalculateConsumptionFrom6ByteArray(uint8_t *pNew, uint8_t *pOld)
{
  uint32_t old32Bit, new32Bit;
  
  // OTA/ZCL Domain - Little Endian 
  FLib_MemCpy(&new32Bit, &pNew[0], sizeof(uint32_t));
  FLib_MemCpy(&old32Bit, &pOld[0], sizeof(uint32_t));
  
  new32Bit=OTA2Native32(new32Bit);
  old32Bit=OTA2Native32(old32Bit);
  
   // Native Domain - MC1322x (Little)  MC1321x,QE,HCS08 (BIG) 
  if(new32Bit >= old32Bit)
  {
    new32Bit = (new32Bit - old32Bit);
  }
  else
  {
    new32Bit = (new32Bit + old32Bit);
  }
  
  new32Bit=Native2OTA32 (new32Bit);  
   // OTA/ZCL Domain - Little Endian 
  FLib_MemCpy(&consumptionValue[0], &new32Bit, sizeof(Consmp));
  
  return &consumptionValue;
 }

/*****************************************************************************/

static void Add8ByteTo6ByteArray(uint8_t value, uint8_t *pSumm_t)
{
  uint32_t LSB2Bytes=0, tmp=0, MSB4Bytes=0;
  
  // OTA/ZCL Domain - Little Endian 
  FLib_MemCpy(&LSB2Bytes, &pSumm_t[0], sizeof(uint16_t));
  FLib_MemCpy(&MSB4Bytes, &pSumm_t[2], sizeof(uint32_t));
  
  LSB2Bytes=OTA2Native32(LSB2Bytes);
  MSB4Bytes=OTA2Native32(MSB4Bytes);
  
  // Native Domain - MC1322x (Little)  MC1321x,QE,HCS08 (BIG)
  LSB2Bytes += value;
  
  tmp = LSB2Bytes >> 16;
  
  MSB4Bytes += tmp;

  // OTA/ZCL Domain - Little Endian 
  pSumm_t[0] = (uint8_t) (LSB2Bytes);
  pSumm_t[1] = (uint8_t) (LSB2Bytes>>8);
  pSumm_t[2] = (uint8_t) (MSB4Bytes);
  pSumm_t[3] = (uint8_t) (MSB4Bytes>>8);
  pSumm_t[4] = (uint8_t) (MSB4Bytes>>16);
  pSumm_t[5] = (uint8_t) (MSB4Bytes>>24);
  
}

#if gZclMirroring_d
zbEndPoint_t ZclMet_CreateAndRegisterMirrorEndpoint(zbNwkAddr_t aNwkAddress)
{
  uint8_t phyEnvironment;
  zbClusterId_t aClusterId = {gaZclClusterBasic_c};
  zclMet_MirroringTable_t* pMTE;
  
  /* Get Physical Environment Attribute*/
  if (ZCL_GetAttribute(endPointList[0].pEndpointDesc->pSimpleDesc->endPoint, aClusterId, gZclAttrBasic_PhysicalEnvironmentId_c, &phyEnvironment, NULL) != gZbSuccess_c)
    return gInvalidAppEndPoint_c;
  
  /* Check if mirroring is supported*/
  if (phyEnvironment != gZclAttrBasicPhylEnvSpecifiedEnvironment)
    return gInvalidAppEndPoint_c;
  
  pMTE = getFreeMirroringTableEntry();
  
  pMTE->isActive = TRUE; 
  
  if ((pMTE) && (AF_RegisterEndPoint(endPointList[pMTE->endPointListIdx].pEndpointDesc) == gZbSuccess_c))
  {
    pMTE->isActive = TRUE; 
    Copy2Bytes(pMTE->aNwkAddress, aNwkAddress);
    return endPointList[pMTE->endPointListIdx].pEndpointDesc->pSimpleDesc->endPoint;
  }
  return gInvalidAppEndPoint_c;
}
#endif

void ZclMet_ConfigureReportingToMirror(zbApsdeDataIndication_t *pIndication, zbEndPoint_t mirrorEndPoint)
{ 
  zbBindUnbindRequest_t  bindRequest;
  zbAddressMap_t addrMap;
  uint8_t idx;
  
  /* Bind our endpoint to the mirror endpoint*/
  (void)AddrMap_SearchTableEntry(NULL, &pIndication->aSrcAddr, &addrMap);
  Set2Bytes(bindRequest.aClusterId, gZclClusterSmplMet_c);
  Copy8Bytes(bindRequest.aSrcAddress, NlmeGetRequest(gNwkIeeeAddress_c));
  bindRequest.srcEndPoint = pIndication->dstEndPoint;
  bindRequest.addressMode = gZbAddrMode64Bit_c;
  Copy8Bytes(bindRequest.destData.extendedMode.aDstAddress, addrMap.aIeeeAddr);
  bindRequest.destData.extendedMode.dstEndPoint = mirrorEndPoint;     
  
  APP_ZDP_BindUnbindRequest(NULL, NlmeGetRequest(gNwkShortAddress_c), gBind_req_c, &bindRequest);
  
  Set2Bytes(bindRequest.aClusterId, gZclClusterBasic_c);
  APP_ZDP_BindUnbindRequest(NULL, NlmeGetRequest(gNwkShortAddress_c), gBind_req_c, &bindRequest);
  
  /* Configure reporting*/
  
  gZclReportingSetup.fReporting = FALSE;
  gZclReportingSetup.reportTimeout = gMetDefaultUpdatePeriod_c;
  gZclReportingSetup.reportMin = gMetDefaultUpdatePeriod_c;
  gZclReportingSetup.minTimeCounter = gMetDefaultUpdatePeriod_c;

  /* Initialize the counter with the number of seconds we want to report */
  gZclReportingSetup.reportCounter = gZclReportingSetup.reportTimeout;
  ZLC_StartReportingTimer();
  
  /* Set the report mask */
  for (idx = 0; idx< gSEMeteringDeviceDef.reportCount; idx++)
    BeeUtilSetIndexedBit(gSEMeteringDeviceDef.pData, idx);

  /* Start reporting and set state */
  gZclReportingSetup.fReporting = TRUE;
}


#if gZclMirroring_d && gNum_MirrorEndpoints_c 
void ZCL_InitMirroring(void)
{
  uint8_t idx;
  for (idx = 0; idx < gNum_MirrorEndpoints_c; idx++)
  {
    gMirroringTable[idx].endPointListIdx = idx + 1;
    gMirroringTable[idx].isActive = FALSE;
  }
 }


zclMet_MirroringTable_t* getMirroringTableEntry(zbNwkAddr_t aNwkAddress, zbEndPoint_t endPoint)
{
  uint8_t idx;
  
  for (idx = 0; idx < gNum_MirrorEndpoints_c; idx++)
  {
    if (((aNwkAddress != NULL) && IsEqual2Bytes(gMirroringTable[idx].aNwkAddress, aNwkAddress)) ||
      ((endPoint != gInvalidAppEndPoint_c) && (endPointList[gMirroringTable[idx].endPointListIdx].pEndpointDesc->pSimpleDesc->endPoint == endPoint)))
      return &gMirroringTable[idx];
  }
  return NULL;
}

zclMet_MirroringTable_t* getFreeMirroringTableEntry(void)
{
  uint8_t idx;
  
  for (idx = 0; idx < gNum_MirrorEndpoints_c; idx++)
  {
    if (!(gMirroringTable[idx].isActive))
      return &gMirroringTable[idx];
  }
  return NULL;
}
#endif