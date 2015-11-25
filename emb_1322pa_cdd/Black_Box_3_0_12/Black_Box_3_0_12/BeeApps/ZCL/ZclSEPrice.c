/******************************************************************************
* ZclSEPrice.c
*
* This source file describes the Price Cluster defined in the ZigBee SE
* specification.
* 
*
* Copyright (c) 2012, Freescale, Inc. All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
******************************************************************************/
#include "EmbeddedTypes.h"
#include "zigbee.h"
#include "FunctionLib.h"
#include "BeeStackConfiguration.h"
#include "BeeStack_Globals.h"
#include "AppAfInterface.h"
#include "AfApsInterface.h"
#include "BeeStackInterface.h"
#include "ZdoApsInterface.h"
#include "HaProfile.h"
#include "ZCLGeneral.h"
#include "zcl.h"
#include "SEProfile.h"
#include "zclSEPrice.h"
#include "zclSE.h"
#include "display.h"
#include "eccapi.h"
#include "Led.h"
#include "ZdoApsInterface.h"
#include "ASL_ZdpInterface.h"
#include "ApsMgmtInterface.h"
#include "beeapp.h"
/******************************************************************************
*******************************************************************************
* Private macros
*******************************************************************************
******************************************************************************/



/******************************************************************************
*******************************************************************************
* Private prototypes
*******************************************************************************
******************************************************************************/

/* Price cluster */
static zbStatus_t ZCL_ProcessClientPublishPrice(zclCmdPrice_PublishPriceRsp_t *pMsg);
#if gASL_ZclPrice_Optionals_d
static zbStatus_t ZCL_ProcessClientPublishBlockPeriod(zclCmdPrice_PublishBlockPeriodRsp_t *pMsg);
#endif 
#if gASL_ZclPrice_Optionals_d && gASL_ZclSE_Use_NextGenFeatures_d
static zbStatus_t ZCL_ProcessClientPublishConversionFactor(zclCmdPrice_PublishConversionFactorRsp_t *pMsg);
static zbStatus_t ZCL_ProcessClientPublishCalorificValue(zclCmdPrice_PublishCalorificValueRsp_t *pMsg);
#endif
static zbStatus_t ZCL_ProcessGetCurrPriceReq
(
addrInfoType_t *pAddrInfo,
zclCmdPrice_GetCurrPriceReq_t * pGetCurrPrice,
bool_t IsInterPanFlag
);
#if gASL_ZclPrice_Optionals_d
static zbStatus_t ZCL_ProcessGetScheduledPricesReq
(
addrInfoType_t *pAddrInfo, 
zclCmdPrice_GetScheduledPricesReq_t * pGetScheduledPrice, 
bool_t IsInterPanFlag
);
static zbStatus_t ZCL_ProcessGetBlockPeriodsReq
(
addrInfoType_t *pAddrInfo, 
zclCmdPrice_GetBlockPeriodsReq_t * pGetBlockPeriods
);
#endif
#if gASL_ZclPrice_Optionals_d && gASL_ZclSE_Use_NextGenFeatures_d
static zbStatus_t ZCL_ProcessGetConversioFactorReq
(
addrInfoType_t *pAddrInfo, 
zclCmdPrice_GetConversionFactorReq_t * pGetConversionFactor
);
static zbStatus_t ZCL_ProcessGetCalorificValueReq
(
addrInfoType_t *pAddrInfo, 
zclCmdPrice_GetCalorificValueReq_t * pGetCalorificValue
);
#endif
#if gASL_ZclSE_Use_NextGenFeatures_d
static zbStatus_t ZCL_ProcessGetCO2ValueReq
(
addrInfoType_t *pAddrInfo, 
zclCmdPrice_GetCO2ValueReq_t * pGetCO2Value
);

static zbStatus_t ZCL_ProcessGetTariffInformationReq
(
addrInfoType_t *pAddrInfo, 
zclCmdPrice_GetTariffInformationReq_t * pGetTariffInformation
);

static zbStatus_t ZCL_ProcessGetPriceMatrixReq
(
addrInfoType_t *pAddrInfo, 
zclCmdPrice_GetPriceMatrixReq_t * pGetPriceMatrix
);

static zbStatus_t ZCL_ProcessGetBlockThresholdsReq
(
addrInfoType_t *pAddrInfo, 
zclCmdPrice_GetBlockThresholdsReq_t * pGetBlockThresholds
);

static zbStatus_t ZCL_ProcessGetBillingPeriodReq
(
addrInfoType_t *pAddrInfo, 
zclCmdPrice_GetBillingPeriodReq_t * pGetBillingPeriod
);

static zbStatus_t ZCL_ProcessCPPEventRsp
(
addrInfoType_t *pAddrInfo, 
zclCmdPrice_CPPEventRsp_t * pCPPEvent
);

static zbStatus_t ZCL_ProcessGetConsolidatedBillReq
(
addrInfoType_t *pAddrInfo, 
zclCmdPrice_GetConsolidatedBillReq_t * pGetConsolidatedBill
);
#endif
#if gASL_ZclPrice_Optionals_d
static zbStatus_t ZCL_SendPublishBlockPeriod(addrInfoType_t *pAddrInfo, publishBlockPeriodEntry_t *pMsg);
#endif
static zbStatus_t ZCL_SendPublishPrice(addrInfoType_t *pAddrInfo, publishPriceEntry_t *pMsg, bool_t IsInterPanFlag);
static zbStatus_t ZCL_PriceAck(zclPrice_PriceAck_t *pReq);
static zbStatus_t ZCL_SendPriceAck(zbApsdeDataIndication_t *pIndication);
#if gASL_ZclPrice_Optionals_d && gASL_ZclSE_Use_NextGenFeatures_d
static zbStatus_t ZCL_SendPublishConversionFactor(addrInfoType_t *pAddrInfo, publishConversionFactorEntry_t * pMsg);
static zbStatus_t ZCL_SendPublishCalorificValue(addrInfoType_t *pAddrInfo, publishCalorificValueEntry_t * pMsg);
#endif
static uint8_t AddPriceInTable(publishPriceEntry_t *pTable, uint8_t len, zclCmdPrice_PublishPriceRsp_t *pMsg);
static uint8_t CheckForPriceUpdate(zclCmdPrice_PublishPriceRsp_t *pMsg, publishPriceEntry_t *pTable, uint8_t len);
#if gASL_ZclPrice_Optionals_d
static uint8_t CheckForBlockPeriodUpdate(zclCmdPrice_PublishBlockPeriodRsp_t *pMsg, publishBlockPeriodEntry_t *pTable, uint8_t len);
#endif
#if gASL_ZclSE_Use_NextGenFeatures_d
static zbStatus_t ZCL_SendPublishCO2Value(addrInfoType_t *pAddrInfo, publishCO2ValueEntry_t * pMsg);
static zbStatus_t ZCL_SendPublishTariffInformation(addrInfoType_t *pAddrInfo, publishTariffInformationEntry_t * pMsg);
static zbStatus_t ZCL_SendPublishPriceMatrix(addrInfoType_t *pAddrInfo, publishPriceMatrixEntry_t * pMsg);
static zbStatus_t ZCL_SendPublishBlockThresholds(addrInfoType_t *pAddrInfo, publishBlockThresholdsEntry_t * pMsg);
static uint8_t CheckForBillingPeriodUpdate(zclCmdPrice_PublishBillingPeriodRsp_t *pMsg, publishBillingPeriodEntry_t *pTable, uint8_t len);
static zbStatus_t ZCL_SendPublishBillingPeriod(addrInfoType_t *pAddrInfo, publishBillingPeriodEntry_t * pMsg);
static uint8_t CheckForConsolidatedBillUpdate(zclCmdPrice_PublishConsolidatedBillRsp_t *pMsg, publishConsolidatedBillEntry_t *pTable, uint8_t len);
static zbStatus_t ZCL_SendPublishConsolidatedBill(addrInfoType_t *pAddrInfo, publishConsolidatedBillEntry_t * pMsg);
static zbStatus_t ZCL_SendPublishCPPEvent(addrInfoType_t *pAddrInfo, publishCPPEventEntry_t * pMsg);
#endif

static afAddrInfo_t PriceCPPEvtRspSourceAddr;

static void TimerClientPriceCallBack(tmrTimerID_t timerID);
static zbStatus_t ZCL_ProcessGetCurrPriceReq
(
addrInfoType_t *pAddrInfo,
zclCmdPrice_GetCurrPriceReq_t * pGetCurrPrice,
bool_t IsInterPanFlag
);

#if gInterPanCommunicationEnabled_c
static zbStatus_t ZCL_SendInterPriceAck(zbInterPanDataIndication_t *pIndication);
static zbStatus_t ZCL_InterPriceAck(zclPrice_InterPriceAck_t *pReq);
static void StoreInterPanAddr(InterPanAddrInfo_t *pAddrInfo);
static void InterPanJitterTimerCallBack(tmrTimerID_t timerID);
#endif /* #if gInterPanCommunicationEnabled_c */

/******************************************************************************
*******************************************************************************
* Private type definitions
*******************************************************************************
******************************************************************************/



/******************************************************************************
*******************************************************************************
* Public memory definitions
*******************************************************************************
******************************************************************************/



/************** PRICE CLUSTER  ******************************************/

const zclAttrDef_t gaZclPriceClientAttrDef[] = {
  {gZclAttrClientPrice_PriceIncreaseRandomizeMinutes_c, gZclDataTypeUint8_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint8_t), (void*)MbrOfs(sePriceClientAttrRAM_t, PriceIncreaseRandomizeMinutes) }
  ,{gZclAttrClientPrice_PriceDecreaseRandomizeMinutes_c, gZclDataTypeUint8_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint8_t), (void*)MbrOfs(sePriceClientAttrRAM_t, PriceDecreaseRandomizeMinutes) }
  ,{gZclAttrClientPrice_CommodityType_c, gZclDataTypeEnum8_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint8_t), (void*)MbrOfs(sePriceClientAttrRAM_t, CommodityType) }
};

const zclAttrDefList_t gZclPriceClientAttrDefList = {
  NumberOfElements(gaZclPriceClientAttrDef),
  gaZclPriceClientAttrDef
};

#if gASL_ZclPrice_Optionals_d
const zclAttrDef_t gaZclPriceServerAttrDef[] = {

 
 //// 8 ---------- ---------- ---------- ---------- ---------- ---------- ---------- ---------- ---------- ---------- 
#if gASL_ZclSE_TiersNumber_d >= 1
	{gZclAttrPrice_Tier1PriceLabel_c, gZclDataTypeOctetStr_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(zclStr12_t), (void*)MbrOfs(sePriceAttrRAM_t, TierPriceLabel[0]) }
#endif			
#if gASL_ZclSE_TiersNumber_d >= 2
	,{gZclAttrPrice_Tier2PriceLabel_c, gZclDataTypeOctetStr_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(zclStr12_t), (void*)MbrOfs(sePriceAttrRAM_t, TierPriceLabel[1]) }
#endif			
#if gASL_ZclSE_TiersNumber_d >= 3
	,{gZclAttrPrice_Tier3PriceLabel_c, gZclDataTypeOctetStr_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(zclStr12_t), (void*)MbrOfs(sePriceAttrRAM_t, TierPriceLabel[2]) }
#endif			
#if gASL_ZclSE_TiersNumber_d >= 4
	,{gZclAttrPrice_Tier4PriceLabel_c, gZclDataTypeOctetStr_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(zclStr12_t), (void*)MbrOfs(sePriceAttrRAM_t, TierPriceLabel[3]) }
#endif			
#if gASL_ZclSE_TiersNumber_d >= 5
	,{gZclAttrPrice_Tier5PriceLabel_c, gZclDataTypeOctetStr_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(zclStr12_t), (void*)MbrOfs(sePriceAttrRAM_t, TierPriceLabel[4]) }
#endif			
#if gASL_ZclSE_TiersNumber_d >= 6
	,{gZclAttrPrice_Tier6PriceLabel_c, gZclDataTypeOctetStr_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(zclStr12_t), (void*)MbrOfs(sePriceAttrRAM_t, TierPriceLabel[5]) }
#endif			
#if gASL_ZclSE_TiersNumber_d >= 7
	,{gZclAttrPrice_Tier7PriceLabel_c, gZclDataTypeOctetStr_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(zclStr12_t), (void*)MbrOfs(sePriceAttrRAM_t, TierPriceLabel[6]) }
#endif			
#if gASL_ZclSE_TiersNumber_d >= 8
	,{gZclAttrPrice_Tier8PriceLabel_c, gZclDataTypeOctetStr_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(zclStr12_t), (void*)MbrOfs(sePriceAttrRAM_t, TierPriceLabel[7]) }
#endif			
#if gASL_ZclSE_TiersNumber_d >= 9
	,{gZclAttrPrice_Tier9PriceLabel_c, gZclDataTypeOctetStr_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(zclStr12_t), (void*)MbrOfs(sePriceAttrRAM_t, TierPriceLabel[8]) }
#endif			
#if gASL_ZclSE_TiersNumber_d >= 10
	,{gZclAttrPrice_Tier10PriceLabel_c, gZclDataTypeOctetStr_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(zclStr12_t), (void*)MbrOfs(sePriceAttrRAM_t, TierPriceLabel[9]) }
#endif			
#if gASL_ZclSE_TiersNumber_d >= 11
	,{gZclAttrPrice_Tier11PriceLabel_c, gZclDataTypeOctetStr_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(zclStr12_t), (void*)MbrOfs(sePriceAttrRAM_t, TierPriceLabel[10]) }
#endif			
#if gASL_ZclSE_TiersNumber_d >= 12
	,{gZclAttrPrice_Tier12PriceLabel_c, gZclDataTypeOctetStr_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(zclStr12_t), (void*)MbrOfs(sePriceAttrRAM_t, TierPriceLabel[11]) }
#endif			
#if gASL_ZclSE_TiersNumber_d >= 13
	,{gZclAttrPrice_Tier13PriceLabel_c, gZclDataTypeOctetStr_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(zclStr12_t), (void*)MbrOfs(sePriceAttrRAM_t, TierPriceLabel[12]) }
#endif			
#if gASL_ZclSE_TiersNumber_d >= 14
	,{gZclAttrPrice_Tier14PriceLabel_c, gZclDataTypeOctetStr_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(zclStr12_t), (void*)MbrOfs(sePriceAttrRAM_t, TierPriceLabel[13]) }
#endif			
#if gASL_ZclSE_TiersNumber_d >= 15
	,{gZclAttrPrice_Tier15PriceLabel_c, gZclDataTypeOctetStr_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(zclStr12_t), (void*)MbrOfs(sePriceAttrRAM_t, TierPriceLabel[14]) }
#endif			


#if gASL_ZclPrice_BlockThresholdNumber_d >= 1
	,{gZclAttrPrice_Block1Threshold_c, gZclDataTypeInt48_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(BlockThreshold_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockThresholdPrice[0]) }
#endif			
#if gASL_ZclPrice_BlockThresholdNumber_d >= 2
	,{gZclAttrPrice_Block2Threshold_c, gZclDataTypeInt48_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(BlockThreshold_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockThresholdPrice[1]) }
#endif			
#if gASL_ZclPrice_BlockThresholdNumber_d >= 3
	,{gZclAttrPrice_Block3Threshold_c, gZclDataTypeInt48_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(BlockThreshold_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockThresholdPrice[2]) }
#endif			
#if gASL_ZclPrice_BlockThresholdNumber_d >= 4
	,{gZclAttrPrice_Block4Threshold_c, gZclDataTypeInt48_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(BlockThreshold_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockThresholdPrice[3]) }
#endif			
#if gASL_ZclPrice_BlockThresholdNumber_d >= 5
	,{gZclAttrPrice_Block5Threshold_c, gZclDataTypeInt48_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(BlockThreshold_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockThresholdPrice[4]) }
#endif			
#if gASL_ZclPrice_BlockThresholdNumber_d >= 6
	,{gZclAttrPrice_Block6Threshold_c, gZclDataTypeInt48_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(BlockThreshold_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockThresholdPrice[5]) }
#endif			
#if gASL_ZclPrice_BlockThresholdNumber_d >= 7
	,{gZclAttrPrice_Block7Threshold_c, gZclDataTypeInt48_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(BlockThreshold_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockThresholdPrice[6]) }
#endif			
#if gASL_ZclPrice_BlockThresholdNumber_d >= 8
	,{gZclAttrPrice_Block8Threshold_c, gZclDataTypeInt48_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(BlockThreshold_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockThresholdPrice[7]) }
#endif			
#if gASL_ZclPrice_BlockThresholdNumber_d >= 9
	,{gZclAttrPrice_Block9Threshold_c, gZclDataTypeInt48_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(BlockThreshold_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockThresholdPrice[8]) }
#endif			
#if gASL_ZclPrice_BlockThresholdNumber_d >= 10
	,{gZclAttrPrice_Block10Threshold_c, gZclDataTypeInt48_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(BlockThreshold_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockThresholdPrice[9]) }
#endif			
#if gASL_ZclPrice_BlockThresholdNumber_d >= 11
	,{gZclAttrPrice_Block11Threshold_c, gZclDataTypeInt48_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(BlockThreshold_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockThresholdPrice[10]) }
#endif			
#if gASL_ZclPrice_BlockThresholdNumber_d >= 12
	,{gZclAttrPrice_Block12Threshold_c, gZclDataTypeInt48_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(BlockThreshold_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockThresholdPrice[11]) }
#endif			
#if gASL_ZclPrice_BlockThresholdNumber_d >= 13
	,{gZclAttrPrice_Block13Threshold_c, gZclDataTypeInt48_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(BlockThreshold_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockThresholdPrice[12]) }
#endif			
#if gASL_ZclPrice_BlockThresholdNumber_d >= 14
	,{gZclAttrPrice_Block14Threshold_c, gZclDataTypeInt48_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(BlockThreshold_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockThresholdPrice[13]) }
#endif			
#if gASL_ZclPrice_BlockThresholdNumber_d >= 15
	,{gZclAttrPrice_Block15Threshold_c, gZclDataTypeInt48_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(BlockThreshold_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockThresholdPrice[14]) }
#endif			


#if gASL_ZclPrice_BlockPeriodNumber_d >= 1
	,{gZclAttrPrice_StartofBlockPeriod_c, gZclDataTypeUTCTime_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(ZCLTime_t), (void*)(MbrOfs(sePriceAttrRAM_t, BlockPeriodPrice) + MbrOfs(BlockPeriodPrice_t, StartofBlockPeriod)) }
#endif			
#if gASL_ZclPrice_BlockPeriodNumber_d >= 2
	,{gZclAttrPrice_BlockPeriodDuration_c, gZclDataTypeUint24_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(Duration24_t), (void*)(MbrOfs(sePriceAttrRAM_t, BlockPeriodPrice) + MbrOfs(BlockPeriodPrice_t, BlockPeriodDuration)) }
#endif
#if gASL_ZclPrice_BlockPeriodNumber_d >= 3
	,{gZclAttrPrice_ThresholdMultiplier_c, gZclDataTypeUint24_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(Multiplier24_t), (void*)(MbrOfs(sePriceAttrRAM_t, BlockPeriodPrice) + MbrOfs(BlockPeriodPrice_t, ThresholdMultiplier)) }
#endif
#if gASL_ZclPrice_BlockPeriodNumber_d >= 4
	,{gZclAttrPrice_ThresholdDivisor_c, gZclDataTypeUint24_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(Divisor24_t), (void*)(MbrOfs(sePriceAttrRAM_t, BlockPeriodPrice) + MbrOfs(BlockPeriodPrice_t, ThresholdDivisor)) }
#endif

#if gASL_ZclPrice_CommodityTypeNumber_d >= 1
	,{gZclAttrPrice_CommodityType_c, gZclDataTypeEnum8_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint8_t), (void*)(MbrOfs(sePriceAttrRAM_t, CommodityTypePrice) + MbrOfs(CommodityTypePrice_t, CommodityType)) }
#endif
#if gASL_ZclPrice_CommodityTypeNumber_d >= 2
	,{gZclAttrPrice_StandingCharge_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(CommodityCharge_t), (void*)(MbrOfs(sePriceAttrRAM_t, CommodityTypePrice) + MbrOfs(CommodityTypePrice_t, StandingCharge)) }
#endif   
#if gASL_ZclPrice_CommodityTypeNumber_d >= 3
	,{gZclAttrPrice_ConversionFactor_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(ConversionFactor_t), (void*)(MbrOfs(sePriceAttrRAM_t, CommodityTypePrice) + MbrOfs(CommodityTypePrice_t, ConversionFactor)) }
#endif   
#if gASL_ZclPrice_CommodityTypeNumber_d >= 4
	,{gZclAttrPrice_ConversionFactorTrlDigit_c, gZclDataTypeBitmap8_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint8_t), (void*)(MbrOfs(sePriceAttrRAM_t, CommodityTypePrice) + MbrOfs(CommodityTypePrice_t, ConversionFactorTrlDigit)) }
#endif 
#if gASL_ZclPrice_CommodityTypeNumber_d >= 5
	,{gZclAttrPrice_CalorificValue_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(CalorificValue_t), (void*)(MbrOfs(sePriceAttrRAM_t, CommodityTypePrice) + MbrOfs(CommodityTypePrice_t, CalorificValue)) }
#endif  
#if gASL_ZclPrice_CommodityTypeNumber_d >= 6
	,{gZclAttrPrice_CalorificValueUnit_c, gZclDataTypeEnum8_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint8_t), (void*)(MbrOfs(sePriceAttrRAM_t, CommodityTypePrice) + MbrOfs(CommodityTypePrice_t, CalorificValueUnit)) }
#endif  
#if gASL_ZclPrice_CommodityTypeNumber_d >= 7
	,{gZclAttrPrice_CalorificValueTrlDigit_c, gZclDataTypeBitmap8_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint8_t), (void*)(MbrOfs(sePriceAttrRAM_t, CommodityTypePrice) + MbrOfs(CommodityTypePrice_t, CalorificValueTrlDigit)) }
#endif
        
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 1
	,{gZclAttrPrice_NoTierBlock1Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[0]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 2
	,{gZclAttrPrice_NoTierBlock2Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[1]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 3
	,{gZclAttrPrice_NoTierBlock3Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[2]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 4
	,{gZclAttrPrice_NoTierBlock4Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[3]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 5
	,{gZclAttrPrice_NoTierBlock5Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[4]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 6
	,{gZclAttrPrice_NoTierBlock6Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[5]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 7
	,{gZclAttrPrice_NoTierBlock7Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[6]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 8
	,{gZclAttrPrice_NoTierBlock8Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[7]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 9
	,{gZclAttrPrice_NoTierBlock9Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[8]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 10
	,{gZclAttrPrice_NoTierBlock10Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[9]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 11
	,{gZclAttrPrice_NoTierBlock11Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[10]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 12
	,{gZclAttrPrice_NoTierBlock12Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[11]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 13
	,{gZclAttrPrice_NoTierBlock13Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[12]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 14
	,{gZclAttrPrice_NoTierBlock14Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[13]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 15
	,{gZclAttrPrice_NoTierBlock15Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[14]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 16
	,{gZclAttrPrice_NoTierBlock16Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[15]) }
#endif			

#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 17
	,{gZclAttrPrice_Tier1Block1Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[16]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 18
	,{gZclAttrPrice_Tier1Block2Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[17]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 19
	,{gZclAttrPrice_Tier1Block3Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[18]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 20
	,{gZclAttrPrice_Tier1Block4Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[19]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 21
	,{gZclAttrPrice_Tier1Block5Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[20]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 22
	,{gZclAttrPrice_Tier1Block6Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[21]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 23
	,{gZclAttrPrice_Tier1Block7Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[22]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 4
	,{gZclAttrPrice_Tier1Block8Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[23]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 25
	,{gZclAttrPrice_Tier1Block9Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[24]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 26
	,{gZclAttrPrice_Tier1Block10Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[25]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 27
	,{gZclAttrPrice_Tier1Block11Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[26]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 28
	,{gZclAttrPrice_Tier1Block12Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[27]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 29
	,{gZclAttrPrice_Tier1Block13Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[28]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 30
	,{gZclAttrPrice_Tier1Block14Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[29]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 31
	,{gZclAttrPrice_Tier1Block15Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[30]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 32
	,{gZclAttrPrice_Tier1Block16Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[31]) }
#endif			

#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 33
	,{gZclAttrPrice_Tier2Block1Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[32]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 34
	,{gZclAttrPrice_Tier2Block2Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[33]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 35
	,{gZclAttrPrice_Tier2Block3Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[34]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 36
	,{gZclAttrPrice_Tier2Block4Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[35]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 37
	,{gZclAttrPrice_Tier2Block5Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[36]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 38
	,{gZclAttrPrice_Tier2Block6Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[37]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 39
	,{gZclAttrPrice_Tier2Block7Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[38]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 40
	,{gZclAttrPrice_Tier2Block8Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[39]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 41
	,{gZclAttrPrice_Tier2Block9Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[40]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 42
	,{gZclAttrPrice_Tier2Block10Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[41]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 43
	,{gZclAttrPrice_Tier2Block11Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[42]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 44
	,{gZclAttrPrice_Tier2Block12Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[43]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 45
	,{gZclAttrPrice_Tier2Block13Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[44]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 46
	,{gZclAttrPrice_Tier2Block14Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[45]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 47
	,{gZclAttrPrice_Tier2Block15Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[46]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 48
	,{gZclAttrPrice_Tier2Block16Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[47]) }
#endif			

#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 49
	,{gZclAttrPrice_Tier3Block1Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[48]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 50
	,{gZclAttrPrice_Tier3Block2Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[49]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 51
	,{gZclAttrPrice_Tier3Block3Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[50]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 52
	,{gZclAttrPrice_Tier3Block4Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[51]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 53
	,{gZclAttrPrice_Tier3Block5Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[52]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 54
	,{gZclAttrPrice_Tier3Block6Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[53]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 55
	,{gZclAttrPrice_Tier3Block7Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[54]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 56
	,{gZclAttrPrice_Tier3Block8Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[55]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 57
	,{gZclAttrPrice_Tier3Block9Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[56]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 58
	,{gZclAttrPrice_Tier3Block10Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[57]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 59
	,{gZclAttrPrice_Tier3Block11Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[58]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 60
	,{gZclAttrPrice_Tier3Block12Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[59]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 61
	,{gZclAttrPrice_Tier3Block13Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[60]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 62
	,{gZclAttrPrice_Tier3Block14Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[61]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 63
	,{gZclAttrPrice_Tier3Block15Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[62]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 64
	,{gZclAttrPrice_Tier3Block16Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[63]) }
#endif			

#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 65
	,{gZclAttrPrice_Tier4Block1Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[64]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 66
	,{gZclAttrPrice_Tier4Block2Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[65]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 67
	,{gZclAttrPrice_Tier4Block3Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[66]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 68
	,{gZclAttrPrice_Tier4Block4Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[67]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 69
	,{gZclAttrPrice_Tier4Block5Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[68]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 70
	,{gZclAttrPrice_Tier4Block6Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[69]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 71
	,{gZclAttrPrice_Tier4Block7Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[70]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 72
	,{gZclAttrPrice_Tier4Block8Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[71]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 73
	,{gZclAttrPrice_Tier4Block9Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[72]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 74
	,{gZclAttrPrice_Tier4Block10Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[73]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 75
	,{gZclAttrPrice_Tier4Block11Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[74]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 76
	,{gZclAttrPrice_Tier4Block12Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[75]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 77
	,{gZclAttrPrice_Tier4Block13Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[76]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 78
	,{gZclAttrPrice_Tier4Block14Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[77]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 79
	,{gZclAttrPrice_Tier4Block15Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[78]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 80
	,{gZclAttrPrice_Tier4Block16Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[79]) }
#endif			

#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 81
	,{gZclAttrPrice_Tier5Block1Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[80]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 82
	,{gZclAttrPrice_Tier5Block2Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[81]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 83
	,{gZclAttrPrice_Tier5Block3Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[82]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 84
	,{gZclAttrPrice_Tier5Block4Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[83]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 85
	,{gZclAttrPrice_Tier5Block5Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[84]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 86
	,{gZclAttrPrice_Tier5Block6Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[85]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 87
	,{gZclAttrPrice_Tier5Block7Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[86]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 88
	,{gZclAttrPrice_Tier5Block8Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[87]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 89
	,{gZclAttrPrice_Tier5Block9Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[88]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 90
	,{gZclAttrPrice_Tier5Block10Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[89]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 91
	,{gZclAttrPrice_Tier5Block11Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[90]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 92
	,{gZclAttrPrice_Tier5Block12Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[91]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 93
	,{gZclAttrPrice_Tier5Block13Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[92]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 94
	,{gZclAttrPrice_Tier5Block14Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[93]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 95
	,{gZclAttrPrice_Tier5Block15Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[94]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 96
	,{gZclAttrPrice_Tier5Block16Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[95]) }
#endif			

#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 97
	,{gZclAttrPrice_Tier6Block1Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[96]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 98
	,{gZclAttrPrice_Tier6Block2Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[97]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 99
	,{gZclAttrPrice_Tier6Block3Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[98]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 100
	,{gZclAttrPrice_Tier6Block4Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[99]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 101
	,{gZclAttrPrice_Tier6Block5Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[100]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 102
	,{gZclAttrPrice_Tier6Block6Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[101]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 103
	,{gZclAttrPrice_Tier6Block7Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[102]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 104
	,{gZclAttrPrice_Tier6Block8Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[103]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 105
	,{gZclAttrPrice_Tier6Block9Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[104]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 106
	,{gZclAttrPrice_Tier6Block10Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[105]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 107
	,{gZclAttrPrice_Tier6Block11Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[106]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 108
	,{gZclAttrPrice_Tier6Block12Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[107]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 109
	,{gZclAttrPrice_Tier6Block13Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[108]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 110
	,{gZclAttrPrice_Tier6Block14Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[109]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 111
	,{gZclAttrPrice_Tier6Block15Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[110]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 112
	,{gZclAttrPrice_Tier6Block16Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[111]) }
#endif			

#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 113
	,{gZclAttrPrice_Tier7Block1Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[112]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 114
	,{gZclAttrPrice_Tier7Block2Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[113]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 115
	,{gZclAttrPrice_Tier7Block3Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[114]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 116
	,{gZclAttrPrice_Tier7Block4Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[115]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 117
	,{gZclAttrPrice_Tier7Block5Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[116]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 118
	,{gZclAttrPrice_Tier7Block6Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[117]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 119
	,{gZclAttrPrice_Tier7Block7Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[118]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 120
	,{gZclAttrPrice_Tier7Block8Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[119]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 121
	,{gZclAttrPrice_Tier7Block9Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[120]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 122
	,{gZclAttrPrice_Tier7Block10Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[121]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 123
	,{gZclAttrPrice_Tier7Block11Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[122]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 124
	,{gZclAttrPrice_Tier7Block12Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[123]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 125
	,{gZclAttrPrice_Tier7Block13Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[124]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 126
	,{gZclAttrPrice_Tier7Block14Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[125]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 127
	,{gZclAttrPrice_Tier7Block15Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[126]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 128
	,{gZclAttrPrice_Tier7Block16Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[127]) }
#endif			

#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 129
	,{gZclAttrPrice_Tier8Block1Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[128]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 130
	,{gZclAttrPrice_Tier8Block2Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[129]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 131
	,{gZclAttrPrice_Tier8Block3Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[130]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 132
	,{gZclAttrPrice_Tier8Block4Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[131]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 133
	,{gZclAttrPrice_Tier8Block5Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[132]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 134
	,{gZclAttrPrice_Tier8Block6Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[133]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 135
	,{gZclAttrPrice_Tier8Block7Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[134]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 136
	,{gZclAttrPrice_Tier8Block8Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[135]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 137
	,{gZclAttrPrice_Tier8Block9Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[136]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 138
	,{gZclAttrPrice_Tier8Block10Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[137]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 139
	,{gZclAttrPrice_Tier8Block11Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[138]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 140
	,{gZclAttrPrice_Tier8Block12Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[139]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 141
	,{gZclAttrPrice_Tier8Block13Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[140]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 142
	,{gZclAttrPrice_Tier8Block14Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[141]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 143
	,{gZclAttrPrice_Tier8Block15Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[142]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 144
	,{gZclAttrPrice_Tier8Block16Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[143]) }
#endif			

#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 145
	,{gZclAttrPrice_Tier9Block1Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[144]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 146
	,{gZclAttrPrice_Tier9Block2Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[145]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 147
	,{gZclAttrPrice_Tier9Block3Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[146]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 148
	,{gZclAttrPrice_Tier9Block4Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[147]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 149
	,{gZclAttrPrice_Tier9Block5Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[148]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 150
	,{gZclAttrPrice_Tier9Block6Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[149]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 151
	,{gZclAttrPrice_Tier9Block7Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[150]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 152
	,{gZclAttrPrice_Tier9Block8Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[151]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 153
	,{gZclAttrPrice_Tier9Block9Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[152]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 154
	,{gZclAttrPrice_Tier9Block10Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[153]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 155
	,{gZclAttrPrice_Tier9Block11Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[154]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 156
	,{gZclAttrPrice_Tier9Block12Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[155]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 157
	,{gZclAttrPrice_Tier9Block13Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[156]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 158
	,{gZclAttrPrice_Tier9Block14Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[157]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 159
	,{gZclAttrPrice_Tier9Block15Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[158]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 160
	,{gZclAttrPrice_Tier9Block16Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[159]) }
#endif			

#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 161
	,{gZclAttrPrice_Tier10Block1Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[160]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 162
	,{gZclAttrPrice_Tier10Block2Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[161]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 163
	,{gZclAttrPrice_Tier10Block3Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[162]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 164
	,{gZclAttrPrice_Tier10Block4Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[163]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 165
	,{gZclAttrPrice_Tier10Block5Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[164]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 166
	,{gZclAttrPrice_Tier10Block6Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[165]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 167
	,{gZclAttrPrice_Tier10Block7Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[166]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 168
	,{gZclAttrPrice_Tier10Block8Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[167]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 169
	,{gZclAttrPrice_Tier10Block9Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[168]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 170
	,{gZclAttrPrice_Tier10Block10Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[169]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 171
	,{gZclAttrPrice_Tier10Block11Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[170]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 172
	,{gZclAttrPrice_Tier10Block12Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[171]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 173
	,{gZclAttrPrice_Tier10Block13Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[172]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 174
	,{gZclAttrPrice_Tier10Block14Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[173]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 175
	,{gZclAttrPrice_Tier10Block15Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[174]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 176
	,{gZclAttrPrice_Tier10Block16Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[175]) }
#endif			

#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 177
	,{gZclAttrPrice_Tier11Block1Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[176]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 178
	,{gZclAttrPrice_Tier11Block2Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[177]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 179
	,{gZclAttrPrice_Tier11Block3Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[178]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 180
	,{gZclAttrPrice_Tier11Block4Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[179]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 181
	,{gZclAttrPrice_Tier11Block5Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[180]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 182
	,{gZclAttrPrice_Tier11Block6Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[181]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 183
	,{gZclAttrPrice_Tier11Block7Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[182]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 184
	,{gZclAttrPrice_Tier11Block8Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[183]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 185
	,{gZclAttrPrice_Tier11Block9Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[184]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 186
	,{gZclAttrPrice_Tier11Block10Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[185]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 187
	,{gZclAttrPrice_Tier11Block11Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[186]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 188
	,{gZclAttrPrice_Tier11Block12Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[187]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 189
	,{gZclAttrPrice_Tier11Block13Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[188]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 190
	,{gZclAttrPrice_Tier11Block14Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[189]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 191
	,{gZclAttrPrice_Tier11Block15Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[190]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 192
	,{gZclAttrPrice_Tier11Block16Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[191]) }
#endif			

#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 193
	,{gZclAttrPrice_Tier12Block1Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[192]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 194
	,{gZclAttrPrice_Tier12Block2Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[193]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 195
	,{gZclAttrPrice_Tier12Block3Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[194]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 196
	,{gZclAttrPrice_Tier12Block4Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[195]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 197
	,{gZclAttrPrice_Tier12Block5Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[196]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 198
	,{gZclAttrPrice_Tier12Block6Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[197]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 199
	,{gZclAttrPrice_Tier12Block7Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[198]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 200
	,{gZclAttrPrice_Tier12Block8Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[199]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 201
	,{gZclAttrPrice_Tier12Block9Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[200]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 202
	,{gZclAttrPrice_Tier12Block10Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[201]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 203
	,{gZclAttrPrice_Tier12Block11Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[202]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 204
	,{gZclAttrPrice_Tier12Block12Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[203]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 205
	,{gZclAttrPrice_Tier12Block13Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[204]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 206
	,{gZclAttrPrice_Tier12Block14Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[205]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 207
	,{gZclAttrPrice_Tier12Block15Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[206]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 208
	,{gZclAttrPrice_Tier12Block16Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[207]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 209
	,{gZclAttrPrice_Tier13Block1Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[208]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 210
	,{gZclAttrPrice_Tier13Block2Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[209]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 211
	,{gZclAttrPrice_Tier13Block3Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[210]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 212
	,{gZclAttrPrice_Tier13Block4Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[211]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 213
	,{gZclAttrPrice_Tier13Block5Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[212]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 214
	,{gZclAttrPrice_Tier13Block6Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[213]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 215
	,{gZclAttrPrice_Tier13Block7Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[214]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 216
	,{gZclAttrPrice_Tier13Block8Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[215]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 217
	,{gZclAttrPrice_Tier13Block9Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[216]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 218
	,{gZclAttrPrice_Tier13Block10Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[217]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 219
	,{gZclAttrPrice_Tier13Block11Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[218]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 220
	,{gZclAttrPrice_Tier13Block12Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[219]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 221
	,{gZclAttrPrice_Tier13Block13Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[220]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 222
	,{gZclAttrPrice_Tier13Block14Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[221]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 223
	,{gZclAttrPrice_Tier13Block15Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[222]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 224
	,{gZclAttrPrice_Tier13Block16Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[223]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 225
	,{gZclAttrPrice_Tier14Block1Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[224]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 226
	,{gZclAttrPrice_Tier14Block2Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[225]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 227
	,{gZclAttrPrice_Tier14Block3Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[226]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 228
	,{gZclAttrPrice_Tier14Block4Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[227]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 229
	,{gZclAttrPrice_Tier14Block5Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[228]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 230
	,{gZclAttrPrice_Tier14Block6Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[229]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 231
	,{gZclAttrPrice_Tier14Block7Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[230]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 232
	,{gZclAttrPrice_Tier14Block8Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[231]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 233
	,{gZclAttrPrice_Tier14Block9Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[232]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 234
	,{gZclAttrPrice_Tier14Block10Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[233]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 235
	,{gZclAttrPrice_Tier14Block11Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[234]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 236
	,{gZclAttrPrice_Tier14Block12Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[235]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 237
	,{gZclAttrPrice_Tier14Block13Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[236]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 238
	,{gZclAttrPrice_Tier14Block14Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[237]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 239
	,{gZclAttrPrice_Tier14Block15Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[238]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 240
	,{gZclAttrPrice_Tier14Block16Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[239]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 241
	,{gZclAttrPrice_Tier15Block1Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[240]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 242
	,{gZclAttrPrice_Tier15Block2Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[241]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 243
	,{gZclAttrPrice_Tier15Block3Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[242]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 244
	,{gZclAttrPrice_Tier15Block4Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[243]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 245
	,{gZclAttrPrice_Tier15Block5Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[244]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 246
	,{gZclAttrPrice_Tier15Block6Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[245]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 247
	,{gZclAttrPrice_Tier15Block7Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[246]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 248
	,{gZclAttrPrice_Tier15Block8Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[247]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 249
	,{gZclAttrPrice_Tier15Block9Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[248]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 250
	,{gZclAttrPrice_Tier15Block10Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[249]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 251
	,{gZclAttrPrice_Tier15Block11Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[250]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 252
	,{gZclAttrPrice_Tier15Block12Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[251]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 253
	,{gZclAttrPrice_Tier15Block13Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[252]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 254
	,{gZclAttrPrice_Tier15Block14Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[253]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 255
	,{gZclAttrPrice_Tier15Block15Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[254]) }
#endif			
#if gASL_ZclPrice_BlockPriceInfoNumber_d >= 256
	,{gZclAttrPrice_Tier15Block16Price_c, gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint32_t), (void*)MbrOfs(sePriceAttrRAM_t, BlockPriceInfoPrice[255]) }
#endif
};

const zclAttrDefList_t gZclPriceServerAttrDefList = {
  NumberOfElements(gaZclPriceServerAttrDef),
  gaZclPriceServerAttrDef
};
#endif

/* TimerId for Price Client */
tmrTimerID_t gPriceClientTimerID;
/* TimerId for Block Period */
#if gASL_ZclPrice_Optionals_d
tmrTimerID_t gBlockPeriodClientTimerID;
#endif
#if gASL_ZclSE_Use_NextGenFeatures_d
tmrTimerID_t  gBillingPeriodClientTimerID;
tmrTimerID_t  gConsolidatedBillClientTimerID;
#endif
publishPriceEntry_t gaClientPriceTable[gNumofClientPrices_c];
publishPriceEntry_t gaServerPriceTable[gNumofServerPrices_c];
#if gASL_ZclPrice_Optionals_d
publishBlockPeriodEntry_t gaClientBlockPeriodsTable[gNumofClientBlockPeriods_c];
publishBlockPeriodEntry_t gaServerBlockPeriodsTable[gNumofServerBlockPeriods_c];
#endif
#if gASL_ZclPrice_Optionals_d && gASL_ZclSE_Use_NextGenFeatures_d
publishConversionFactorEntry_t gaClientConversionFactorTable[gNumofClientConversionFactors_c];
publishConversionFactorEntry_t gaServerConversionFactorTable[gNumofServerConversionFactors_c];
publishCalorificValueEntry_t gaClientCalorificValueTable[gNumofClientCalorificValue_c];
publishCalorificValueEntry_t gaServerCalorificValueTable[gNumofServerCalorificValue_c];
#endif
#if gASL_ZclSE_Use_NextGenFeatures_d
publishCO2ValueEntry_t gaClientCO2ValueTable[gNumofClientCO2Value_c];
publishCO2ValueEntry_t gaServerCO2ValueTable[gNumofServerCO2Value_c];
publishPriceMatrixEntry_t gaClientPriceMatrixTable[gNumofClientPriceMatrix_c];
publishPriceMatrixEntry_t gaServerPriceMatrixTable[gNumofServerPriceMatrix_c];
publishBlockThresholdsEntry_t gaClientBlockThresholdsTable[gNumofClientBlockThresholds_c];
publishBlockThresholdsEntry_t gaServerBlockThresholdsTable[gNumofServerBlockThresholds_c];
publishBillingPeriodEntry_t gaClientBillingPeriodTable[gNumofClientBillingPeriods_c];
publishBillingPeriodEntry_t gaServerBillingPeriodTable[gNumofServerBillingPeriods_c];
publishCPPEventEntry_t gaClientCPPEventTable[gNumofClientCPPEvents_c];
publishCPPEventEntry_t gaServerCPPEventTable[gNumofServerCPPEvents_c];
publishConsolidatedBillEntry_t gaClientConsolidatedBillTable[gNumofClientConsolidatedBill_c];
publishConsolidatedBillEntry_t gaServerConsolidatedBillTable[gNumofServerConsolidatedBill_c];
publishTariffInformationEntry_t gaClientTariffInformationTable[gNumofClientTariffInformation_c];
publishTariffInformationEntry_t gaServerTariffInformationTable[gNumofServerTariffInformation_c];
#endif
index_t mUpdatePriceIndex;
#if gASL_ZclPrice_Optionals_d
index_t mUpdateBlockPeriodIndex;
#endif
index_t mRegIndex , mInterPanIndex;
#if gASL_ZclSE_Use_NextGenFeatures_d
index_t mUpdateConsolidatedBillIndex;
index_t mUpdateCPPEventIndex;
index_t mUpdateBillingPeriodIndex;
index_t mUpdateCO2ValueIndex;
index_t mUpdateTariffInformationIndex;
#endif
#if gASL_ZclPrice_Optionals_d && gASL_ZclSE_Use_NextGenFeatures_d
index_t mUpdateConversionFactorIndex;
index_t mUpdateCalorificValueIndex;
#endif
addrInfoType_t mAddrPriceInfo; 

uint32_t mGetPriceStartTime;
uint8_t mGetNumOfPriceEvts;
bool_t mIsInterPanFlag;
uint8_t mIndex;
//Save indexs that need to be sent after a GetBlockPeriodRequest
#if gASL_ZclPrice_Optionals_d 
getBlockPeriodEntryIndex_t mGetBlockPeriodReqIndexs;
#endif
#if gASL_ZclPrice_Optionals_d && gASL_ZclSE_Use_NextGenFeatures_d
getConversionFactorEntryIndex_t mGetConversionFactorReqIndexs;
getCalorificValueEntryIndex_t mGetCalorificValueReqIndexs;
#endif
#if gASL_ZclSE_Use_NextGenFeatures_d
getPriceMatrixEntryIndex_t mGetPriceMatrixReqEvtsIdx;
getBlockThresholdsEntryIndex_t mGetBlockThresholdsReqEvtsIdx;
getCO2ValueEntryIndex_t mGetCO2ValueReqEvtsIdx;
getBillingPeriodEntryIndex_t mGetBillingPeriodReqEvtsIdx;
getConsolidatedBillEntryIndex_t mGetConsolidatedBillReqEvtsIdx;
getTariffInformationEntryIndex_t mGetTariffInformationReqEvtsIdx;
#endif
storedInterPanAddrInfo_t gaInterPanAddrTable[gNumOfInterPanAddr_c];
storedPriceAddrInfo_t gaPriceAddrTable[gNumOfPriceAddr_c];

/********************************************************************************/

/******************************************************************************
*******************************************************************************
* Private memory declarations
*******************************************************************************
******************************************************************************/

/*Used to keep the new message(event)*/
static uint32_t mPriceClusterEvt = 0x00;

/******************************************************************************
*******************************************************************************
* Public functions
*******************************************************************************
******************************************************************************/

afAddrInfo_t *ZCL_GetPriceCPPEventSrcAddrForResponse(void)
{
  return &PriceCPPEvtRspSourceAddr;
}

void ZCL_PriceClientInit(void)
{
  gPriceClientTimerID =  TMR_AllocateTimer();
#if gASL_ZclPrice_Optionals_d
  gBlockPeriodClientTimerID = TMR_AllocateTimer();
#endif
#if gASL_ZclSE_Use_NextGenFeatures_d
  gBillingPeriodClientTimerID = TMR_AllocateTimer();
  gConsolidatedBillClientTimerID = TMR_AllocateTimer();
#endif
} 	

/******************************************************************************/
/******************************************************************************/
/* The Publish Price command is generated in response to receiving a Get Current
Price command , a Get Scheduled Prices command , or when an update to the pricing information is available
from the commodity provider. */
static zbStatus_t ZCL_SendPublishPrice(addrInfoType_t *pAddrInfo, publishPriceEntry_t * pMsg, bool_t IsInterPanFlag) 
{
  zclPrice_PublishPriceRsp_t req;
  #if gInterPanCommunicationEnabled_c
  zclPrice_InterPanPublishPriceRsp_t interPanReq;
  #endif   
  uint32_t currentTime, stopTime, newDuration;
  
  /* Get the current time */
  currentTime = ZCL_GetUTCTime();
  if(((pMsg->Price.StartTime) == 0x00000000) &&  (pMsg->Price.DurationInMinutes != 0xFFFF))
  {
    stopTime = OTA2Native32(pMsg->EffectiveStartTime) + ( 60 *  (uint32_t)(OTA2Native16(pMsg->Price.DurationInMinutes)));
    newDuration = stopTime - currentTime;
  }
  
  if(!IsInterPanFlag)
  {
    FLib_MemCpy(&req.addrInfo, pAddrInfo, sizeof(afAddrInfo_t));	
    req.zclTransactionId =  gZclTransactionId++;
    FLib_MemCpy(&req.cmdFrame, pMsg, sizeof(zclCmdPrice_PublishPriceRsp_t));
    req.cmdFrame.CurrTime = currentTime;
    if(((pMsg->Price.StartTime) == 0x00000000) &&  (pMsg->Price.DurationInMinutes != 0xFFFF))
    {
      uint32_t rem = newDuration % 60;
      req.cmdFrame.DurationInMinutes = Native2OTA16((uint16_t)(newDuration / 60 + ((rem > 30)?1:0)));
    }
         
    return zclPrice_PublishPriceRsp(&req);
  }  
#if gInterPanCommunicationEnabled_c
  else	
  {
    FLib_MemCpy(&interPanReq.addrInfo, pAddrInfo, sizeof(InterPanAddrInfo_t));	
    interPanReq.zclTransactionId =  gZclTransactionId++;
    FLib_MemCpy(&interPanReq.cmdFrame, pMsg, sizeof(zclCmdPrice_PublishPriceRsp_t));
    interPanReq.cmdFrame.CurrTime = currentTime;
    if(((pMsg->Price.StartTime) == 0x00000000) &&  (pMsg->Price.DurationInMinutes != 0xFFFF))
    {
      uint32_t rem = newDuration % 30;
      req.cmdFrame.DurationInMinutes = Native2OTA16((uint16_t)(newDuration / 60 + (rem > 30)?1:0));
    }
    return zclPrice_InterPanPublishPriceRsp(&interPanReq);		
  } 
#else
return FALSE;
#endif     
}

/******************************************************************************/
/* The Publish Block Period command is generated in response to receiving a Get Block Periods
command or when an update to the block period information is available
from the commodity provider. */
#if gASL_ZclPrice_Optionals_d
static zbStatus_t ZCL_SendPublishBlockPeriod(addrInfoType_t *pAddrInfo, publishBlockPeriodEntry_t * pMsg) 
{
  zclPrice_PublishBlockPeriodRsp_t req;    
  zclCmdPrice_PublishBlockPeriodRsp_t cmdFrame;
  uint32_t currentTime, stopTime, newDuration, duration;
  
  currentTime = ZCL_GetUTCTime();
  
  if((pMsg->blockPeriod.BlockPeriodStartTime) == 0x00000000)
  {
    // get the duration value
    duration = GetNative32BitIntFrom3ByteArray(pMsg->blockPeriod.BlockPeriodDurationInMinutes);
    stopTime = Native2OTA32(pMsg->EffectiveStartTime) + ( 60 *  duration);
    newDuration = (stopTime - currentTime) / 60;
  }
  
  FLib_MemCpy(&cmdFrame.ProviderID, &pMsg->blockPeriod.ProviderID, sizeof(ProviderID_t));
  FLib_MemCpy(&cmdFrame.IssuerEvtID, &pMsg->blockPeriod.IssuerEvtID, sizeof(SEEvtId_t));
  cmdFrame.BlockPeriodStartTime = pMsg->blockPeriod.BlockPeriodStartTime;
  if((pMsg->blockPeriod.BlockPeriodStartTime) == 0x00000000)
  {
    FLib_MemCpy(&cmdFrame.BlockPeriodDurationInMinutes, &newDuration, sizeof(BlockPeriodDuration_t));
  }
  else
  {
    FLib_MemCpy(&cmdFrame.BlockPeriodDurationInMinutes, &pMsg->blockPeriod.BlockPeriodDurationInMinutes, sizeof(BlockPeriodDuration_t));
  }
  cmdFrame.NrOfPriceTiersNrOfBlockThresholds = pMsg->blockPeriod.NrOfPriceTiersNrOfBlockThresholds;
  cmdFrame.BlockPeriodControl = pMsg->blockPeriod.BlockPeriodControl;
 
  FLib_MemCpy(&req.addrInfo, pAddrInfo, sizeof(afAddrInfo_t));
  req.zclTransactionId =  gZclTransactionId++;
  FLib_MemCpy(&req.cmdFrame, &cmdFrame, sizeof(zclCmdPrice_PublishBlockPeriodRsp_t));
  
  return zclPrice_PublishBlockPeriodRsp(&req);
}
#endif
/******************************************************************************/
static zbStatus_t ZCL_PriceAck(zclPrice_PriceAck_t *pReq)
{
  return ZCL_SendClientRspSeqPassed(gZclCmdPrice_PriceAck_c, sizeof(zclCmdPrice_PriceAck_t),(zclGenericReq_t *)pReq);
}

static zbStatus_t ZCL_InterPriceAck(zclPrice_InterPriceAck_t *pReq)
{
  return ZCL_SendInterPanClientRspSeqPassed(gZclCmdPrice_PriceAck_c, sizeof(zclCmdPrice_PriceAck_t),(zclGenericReq_t *)pReq);
}

/******************************************************************************/

static zbStatus_t ZCL_SendPriceAck(zbApsdeDataIndication_t *pIndication)
{
  uint32_t currentTime;
  zclPrice_PriceAck_t priceAck;
  zclCmdPrice_PublishPriceRsp_t * pMsg;
  
  pMsg = (zclCmdPrice_PublishPriceRsp_t*)((uint8_t*)pIndication->pAsdu + sizeof(zclFrame_t));
  AF_PrepareForReply(&priceAck.addrInfo, pIndication);
  priceAck.zclTransactionId = ((zclFrame_t *)pIndication->pAsdu)->transactionId;
  FLib_MemCpy(&priceAck.cmdFrame.ProviderID, pMsg->ProviderID, sizeof(ProviderID_t));
  FLib_MemCpy(&priceAck.cmdFrame.IssuerEvt, pMsg->IssuerEvt, sizeof(SEEvtId_t));
  currentTime = ZCL_GetUTCTime();
  currentTime = Native2OTA32(currentTime);
  priceAck.cmdFrame.PriceAckTime = currentTime;
  priceAck.cmdFrame.PriceControl = pMsg->PriceControl;
  
  return ZCL_PriceAck(&priceAck);
}

/******************************************************************************/

static zbStatus_t ZCL_SendInterPriceAck(zbInterPanDataIndication_t *pIndication)
{
  uint32_t currentTime;
  zclPrice_InterPriceAck_t priceAck;
  zclCmdPrice_PublishPriceRsp_t * pMsg;
  
  pMsg = (zclCmdPrice_PublishPriceRsp_t*)((uint8_t*)pIndication->pAsdu + sizeof(zclFrame_t));
  PrepareInterPanForReply((InterPanAddrInfo_t *)&priceAck.addrInfo, pIndication);
  priceAck.zclTransactionId = ((zclFrame_t *)pIndication->pAsdu)->transactionId;
  FLib_MemCpy(&priceAck.cmdFrame.ProviderID, pMsg->ProviderID, sizeof(ProviderID_t));
  FLib_MemCpy(&priceAck.cmdFrame.IssuerEvt, pMsg->IssuerEvt, sizeof(SEEvtId_t));
  currentTime = ZCL_GetUTCTime();
  currentTime = Native2OTA32(currentTime);
  priceAck.cmdFrame.PriceAckTime = currentTime;
  priceAck.cmdFrame.PriceControl = pMsg->PriceControl;
  
  return ZCL_InterPriceAck(&priceAck);
}

/******************************************************************************/
/* Add Price in Table... so that to have the scheduled price in asccendent order */
static uint8_t AddPriceInTable(publishPriceEntry_t *pTable, uint8_t len, zclCmdPrice_PublishPriceRsp_t *pMsg)
{
  uint32_t startTime, msgStartTime, currentTime,stopTime;
  uint8_t i, poz = 0xff;
   /*
      The only exception to this is that if an event with a newer Issuer Event ID
      overlaps with the end of the current active price but is not yet active,
      the active price is not deleted but its duration is modified to 0xFFFF 
      (until changed) so that the active price ends when the new event begins
  */
  bool_t exceptOverlap = FALSE;
  bool_t verifyExceptOverlap = FALSE;
  /* get message time */
  msgStartTime = OTA2Native32(pMsg->StartTime);
  currentTime = ZCL_GetUTCTime();
  if(msgStartTime == 0x000000)
  {
    msgStartTime = currentTime;
  }
  
  /* keep the price in asccendent order; find the index in table where the message will be added */
  for(i = 0; i < len; i++)
  {
    if((pTable+i)->EntryStatus != 0x00)
    {
      startTime = OTA2Native32((pTable+i)->Price.StartTime);
      if(startTime == 0x000000)
      {
        startTime = OTA2Native32((pTable+i)->EffectiveStartTime);
      }
      if(((startTime > msgStartTime) && (startTime != 0xffffffff)) ||
         (msgStartTime == 0xffffffff)) 
      {
        poz = i;
        if(poz == 1)
        {
          verifyExceptOverlap = TRUE;
        }
        break;
      }
    }
    else
    {
      poz = i;
       if(poz == 1)
       {
          verifyExceptOverlap = TRUE;
       }
      break;
    }
    
  }
  if(verifyExceptOverlap)
  {
    startTime = OTA2Native32((pTable)->Price.StartTime);
    if(startTime == 0x000000)
    {
      startTime = OTA2Native32((pTable)->EffectiveStartTime);
    }
    stopTime = startTime + (uint32_t)(60 * OTA2Native16((pTable)->Price.DurationInMinutes));
    if((stopTime > msgStartTime) && (msgStartTime > currentTime))
    {
      exceptOverlap = TRUE;
    }
  }
  /* check if the table is full; return oxff */
  if(poz == 0xff)
    return poz;
  /*move the content to the left*/
  if((pTable+poz)->EntryStatus !=0x00)
    FLib_MemInPlaceCpy((pTable+poz+1),(pTable+poz),(len-poz-1)* sizeof(publishPriceEntry_t));
  
  FLib_MemCpy((pTable+poz), pMsg, sizeof(zclCmdPrice_PublishPriceRsp_t));
  
  if((pMsg->StartTime == 0x00000000)||
     (pMsg->StartTime == 0xffffffff))
  
  {
    /*  here get the currentTime  */
    currentTime = ZCL_GetUTCTime();
    currentTime = Native2OTA32(currentTime);
    (pTable+poz)->EffectiveStartTime = currentTime;
  }
  else
    (pTable+poz)->EffectiveStartTime = pMsg->StartTime ;
  if(exceptOverlap)
  {
    (pTable)->Price.DurationInMinutes = 0xFFFF;
  }
  /* new price was received */	
  startTime = OTA2Native32((pTable+i)->Price.StartTime);
  (pTable+poz)->EntryStatus = gPriceReceivedStatus_c;
  /* Call the App to signal that a Price was received; User should check EntryStatus */
  BeeAppUpdateDevice (0, gZclUI_PriceEvt_c, 0, 0, (pTable+poz));
  return poz;
}

/******************************************************************************/
/*Check and Updated a Price.
When new pricing information is provided that replaces older pricing
information for the same time period, IssuerEvt field allows devices to determine which
information is newer. It is expected that the value contained in this field is a
unique number managed by upstream servers.
Thus, newer pricing information will have a value in the Issuer Event ID field that
is larger than older pricing information.
*/
static uint8_t CheckForPriceUpdate(zclCmdPrice_PublishPriceRsp_t *pMsg, publishPriceEntry_t *pTable, uint8_t len)
{
  uint8_t i;
  uint32_t msgIssuerEvt, entryIssuerEvt;
  
  msgIssuerEvt = FourBytesToUint32(pMsg->IssuerEvt);
  msgIssuerEvt = OTA2Native32(msgIssuerEvt);
  
  for(i = 0; i < len; i++)
  {
    if((pTable+i)->EntryStatus != 0x00)
    {
      entryIssuerEvt = FourBytesToUint32(&(pTable+i)->Price.IssuerEvt);
      entryIssuerEvt = OTA2Native32(entryIssuerEvt);
      if (((pTable+i)->Price.StartTime == pMsg->StartTime &&
          FLib_MemCmp(&(pTable+i)->Price.DurationInMinutes, &pMsg->DurationInMinutes, sizeof(Duration_t))) ||
         (((pTable+i)->Price.StartTime == 0x00000000) &&  (pTable+i)->Price.DurationInMinutes < pMsg->DurationInMinutes))
        if(entryIssuerEvt < msgIssuerEvt)
        {
          FLib_MemCpy((pTable+i), pMsg, sizeof(zclCmdPrice_PublishPriceRsp_t));
          return i;
        }
        else 
          return 0xFE; /* reject it */
    }
  }
  return 0xff;
  
}

/******************************************************************************/
/* The Price timer callback that keep track of current active price */
static void TimerClientPriceCallBack(tmrTimerID_t timerID)
{
  (void) timerID;
  SendPriceClusterEvt(gzclEvtHandleClientPrices_c);
}

/******************************************************************************/
/* Handle the Client Prices signalling when the current price starts, was updated or is completed */
void ZCL_HandleClientPrices(void)
{
  uint32_t currentTime, startTime, nextTime=0x00000000, stopTime;
  uint16_t duration;
  publishPriceEntry_t *pEntry = &gaClientPriceTable[0];
  publishPriceEntry_t *pEntryNext = &gaClientPriceTable[1];
  
  /* the Price table is kept in ascendent order; check if any price is scheduled*/
  if(pEntry->EntryStatus == 0x00)
  {
    TMR_StopSecondTimer(gPriceClientTimerID);
    return;
  }
  
  /* Get the timing */
  currentTime = ZCL_GetUTCTime();
  startTime = OTA2Native32(pEntry->EffectiveStartTime);
  duration = OTA2Native16(pEntry->Price.DurationInMinutes);
  stopTime = startTime + (60*(uint32_t)duration);
  
  /* Check if the duration id 0xFFFF*/
  if(duration == 0xFFFF)
  {// verify the next gaClientPriceTable entry
     if(pEntryNext->EntryStatus != 0x00)
     {
       startTime = OTA2Native32(pEntryNext->EffectiveStartTime);
       if(startTime <= currentTime)
       {
         pEntry->EntryStatus = gPriceCompletedStatus_c; /* entry is not used anymore */
         /* Call the App to signal that a Price was completed; User should check EntryStatus */
         BeeAppUpdateDevice (0, gZclUI_PriceEvt_c, 0, 0, pEntry);
         
         /* Activate the new event*/
         FLib_MemInPlaceCpy(pEntry, (pEntry+1), (gNumofClientPrices_c-1) * sizeof(publishPriceEntry_t));
         gaClientPriceTable[1].EntryStatus = 0x00;
         duration = OTA2Native16(pEntry->Price.DurationInMinutes);
         stopTime = startTime + (60*(uint32_t)duration);
         
         pEntry->EntryStatus = gPriceStartedStatus_c;
         /* Call the App to signal that a Price was started; User should check EntryStatus */
         BeeAppUpdateDevice (0, gZclUI_PriceEvt_c, 0, 0, pEntry);
       }
     }
  }
  else
  {
  
    /* Check if the Price Event is completed */
    if(stopTime <= currentTime)
    {
      pEntry->EntryStatus = gPriceCompletedStatus_c; /* entry is not used anymore */
      /* Call the App to signal that a Price was completed; User should check EntryStatus */
      BeeAppUpdateDevice (0, gZclUI_PriceEvt_c, 0, 0, pEntry);
      pEntry->EntryStatus = 0x00;
      FLib_MemInPlaceCpy(pEntry, (pEntry+1), (gNumofClientPrices_c-1) * sizeof(publishPriceEntry_t));
      gaClientPriceTable[gNumofClientPrices_c-1].EntryStatus = 0x00;
    }
    else	
      if(startTime <= currentTime) /* check if the Price event have to be started or updated */
      {
        if((pEntry->EntryStatus == gPriceReceivedStatus_c)||
           (pEntry->EntryStatus == gPriceUpdateStatus_c))
        {
          pEntry->EntryStatus = gPriceStartedStatus_c;
          /* Call the App to signal that a Price was started; User should check EntryStatus */
          BeeAppUpdateDevice (0, gZclUI_PriceEvt_c, 0, 0, pEntry);
        }
      }
  }
    
  if(currentTime < startTime)
    nextTime = startTime - currentTime;
  else
    if(currentTime < stopTime)
    {
      if(duration == 0xFFFF)
      {
         if(pEntryNext->EntryStatus != 0x00)
         {
           nextTime = pEntryNext->EffectiveStartTime;
         }
      }
      else
      nextTime = stopTime - currentTime;
    }
  if (nextTime)
    TMR_StartSecondTimer(gPriceClientTimerID,(uint16_t)nextTime, TimerClientPriceCallBack);
  
}

/******************************************************************************/
/* Process the received Publish Price */
static zbStatus_t ZCL_ProcessClientPublishPrice(zclCmdPrice_PublishPriceRsp_t *pMsg)
{
  uint8_t updateIndex;
  zbStatus_t status = gZclFailure_c; 
  uint8_t newEntry;

  FLib_MemInPlaceCpy(&pMsg->IssuerEvt, &(pMsg->RateLabel.aStr[pMsg->RateLabel.length]), 
                     (sizeof(zclCmdPrice_PublishPriceRsp_t) - sizeof(ProviderID_t)- sizeof(zclStr12_t)));

  /* Check if it is a PriceUpdate */
  updateIndex = CheckForPriceUpdate(pMsg, (publishPriceEntry_t *)&gaClientPriceTable[0], gNumofClientPrices_c);
  /*if the Publish Price is not an update and is not rejected, add it in the table */
  if(updateIndex == 0xff)
  {
    /* Add the new Price information in the table */
    newEntry = AddPriceInTable((publishPriceEntry_t *)&gaClientPriceTable[0], gNumofClientPrices_c, pMsg);
    if (newEntry != 0xff)
      status = gZclSuccess_c; 
  }
  else
  {
    if(updateIndex != 0xfe)
    {
      /* the price was updated */
      gaClientPriceTable[updateIndex].EntryStatus = gPriceUpdateStatus_c;
      /* Call the App to signal that a Price was updated; User should check EntryStatus */
      BeeAppUpdateDevice (0, gZclUI_PriceEvt_c, 0, 0, &gaClientPriceTable[updateIndex]);
      status = gZclSuccess_c;
    }
  }
  
  if(status == gZclSuccess_c)
    SendPriceClusterEvt(gzclEvtHandleClientPrices_c);
  
  return status;
}
#if gASL_ZclPrice_Optionals_d
/******************************************************************************/
/* The Block Period timer callback that keep track of current active price */
static void TimerClientBlockPeriodCallBack(tmrTimerID_t timerID)
{
  (void) timerID;
  SendPriceClusterEvt(gzclEvtHandleClientBlockPeriod_c);
}

/******************************************************************************/
/* Process the received Publish Block Period */
static zbStatus_t ZCL_ProcessClientPublishBlockPeriod(zclCmdPrice_PublishBlockPeriodRsp_t *pMsg)
{
  uint8_t updateIndex;
  zbStatus_t status = gZclFailure_c; 
  uint8_t newEntry;

  /* Check if it is a BlockPeriod Update */
  updateIndex = CheckForBlockPeriodUpdate(pMsg, (publishBlockPeriodEntry_t *)&gaClientBlockPeriodsTable[0], gNumofClientBlockPeriods_c);
  /*if the Publish Price is not an update and is not rejected, add it in the table */
  if(updateIndex == 0xff)
  {
    /* Add the new Block Period information in the table */
    newEntry = AddBlockPeriodsInTable((publishBlockPeriodEntry_t *)&gaClientBlockPeriodsTable[0], gNumofClientBlockPeriods_c, pMsg);
    if (newEntry != 0xff)
      status = gZclSuccess_c; 
  }
  else
  {
    if(updateIndex != 0xfe)
    {
      /* the price was updated */
      gaClientBlockPeriodsTable[updateIndex].EntryStatus = gPriceUpdateStatus_c;
      /* Call the App to signal that a Price was updated; User should check EntryStatus */
      BeeAppUpdateDevice (0, gZclUI_BlockPeriodEvt_c, 0, 0, &gaClientBlockPeriodsTable[updateIndex]);
      status = gZclSuccess_c;
    }
  }
  
  if(status == gZclSuccess_c)
    SendPriceClusterEvt(gzclEvtHandleClientBlockPeriod_c);
  
  return status;
}
#endif
/******************************************************************************/
/* Handle the Client Block Period signalling when the current price starts, was updated or is completed */
#if gASL_ZclPrice_Optionals_d
void ZCL_HandleClientBlockPeriod(void)
{
  uint32_t currentTime, startTime, nextTime=0x00000000, stopTime;
  publishBlockPeriodEntry_t *pEntry = &gaClientBlockPeriodsTable[0];
  
  
  /* the Block Period table is kept in ascendent order; check if any Block Period is scheduled*/
  if(pEntry->EntryStatus == 0x00)
  {
    TMR_StopSecondTimer(gBlockPeriodClientTimerID);
    return;
  }
  
  /* Get the timing */
  currentTime = ZCL_GetUTCTime();
  startTime = OTA2Native32(pEntry->EffectiveStartTime);
  stopTime = startTime + (60 * GetNative32BitIntFrom3ByteArray(pEntry->blockPeriod.BlockPeriodDurationInMinutes));
  /* Check if the Price Event is completed */
  if(stopTime <= currentTime)
  {
    pEntry->EntryStatus = gBlockPeriodCompletedStatus_c; /* entry is not used anymore */
    /* Call the App to signal that a Price was completed; User should check EntryStatus */
    BeeAppUpdateDevice (0, gZclUI_BlockPeriodEvt_c, 0, 0, pEntry);
    pEntry->EntryStatus = 0x00;
    FLib_MemInPlaceCpy(pEntry, (pEntry+1), (gNumofClientBlockPeriods_c-1) * sizeof(publishBlockPeriodEntry_t));
    gaClientBlockPeriodsTable[gNumofClientBlockPeriods_c-1].EntryStatus = 0x00;
  }
  else
  {
    if(startTime <= currentTime) /* check if the Price event have to be started or updated */
    {
      if((pEntry->EntryStatus == gBlockPeriodReceivedStatus_c)||
         (pEntry->EntryStatus == gBlockPeriodUpdateStatus_c))
      {
        pEntry->EntryStatus = gBlockPeriodStartedStatus_c;
        /* Call the App to signal that a BlockPeriod was started; User should check EntryStatus */
        BeeAppUpdateDevice (0, gZclUI_BlockPeriodEvt_c, 0, 0, pEntry);
      }
    }
  }
  if(currentTime < startTime)
    nextTime = startTime - currentTime;
  else
    if(currentTime < stopTime)
      nextTime = stopTime - currentTime;
  if (nextTime)
    TMR_StartSecondTimer(gBlockPeriodClientTimerID,(uint16_t)nextTime, TimerClientBlockPeriodCallBack);
  
}
#endif

#if gASL_ZclPrice_Optionals_d && gASL_ZclSE_Use_NextGenFeatures_d
/******************************************************************************/
/* Process the received Publish Conversion Factor */
static zbStatus_t ZCL_ProcessClientPublishConversionFactor(zclCmdPrice_PublishConversionFactorRsp_t *pMsg)
{
  uint8_t updateIndex;
  zbStatus_t status = gZclFailure_c; 
  uint8_t newEntry;

  /* Check if it is a Conversion Factor Update */
  for(updateIndex=0;updateIndex<gNumofClientConversionFactors_c;updateIndex++)
  {
    if(gaClientConversionFactorTable[updateIndex].conversionFactor.StartTime == 
       pMsg->StartTime)
    {
      if(gaClientConversionFactorTable[updateIndex].conversionFactor.IssuerEvt >= 
         pMsg->IssuerEvt)
      {
        updateIndex = 0xFE;// reject it;
      }
      else
      {
        FLib_MemCpy(&gaClientConversionFactorTable[updateIndex], pMsg, sizeof(zclCmdPrice_PublishConversionFactorRsp_t));
      }
      break;
    }
  }
  /*if the Publish Conversion Factor is not rejected, add it in the table */
  if(updateIndex == gNumofClientConversionFactors_c)
  {
    if(updateIndex != 0xfe)
    {
      /* the Conversion Factor was updated */
      gaClientConversionFactorTable[updateIndex].EntryStatus = gPriceUpdateStatus_c;
      status = gZclSuccess_c;
    }
    else
    {
    /* Add the new Conversion Factor information in the table */
    newEntry = AddConversionFactorInTable((publishConversionFactorEntry_t *)&gaClientConversionFactorTable[0], gNumofClientConversionFactors_c, pMsg);
    if (newEntry != 0xff)
      status = gZclSuccess_c; 
    }
  }
  
  //if(status == gZclSuccess_c)
    //TS_SendEvent(gZclTaskId, gzclEvtHandleClientPrices_c);
  
  return status;
}

/******************************************************************************/
/* Process the received Publish Calorific Value */
static zbStatus_t ZCL_ProcessClientPublishCalorificValue(zclCmdPrice_PublishCalorificValueRsp_t *pMsg)
{
  uint8_t updateIndex;
  zbStatus_t status = gZclFailure_c; 
  uint8_t newEntry;

  /* Check if it is a Calorific Value Update */
  for(updateIndex=0;updateIndex<gNumofClientCalorificValue_c;updateIndex++)
  {
    if(gaClientCalorificValueTable[updateIndex].CalorificValue.StartTime == 
       pMsg->StartTime)
    {
       if(gaClientCalorificValueTable[updateIndex].CalorificValue.IssuerEvtID >= 
         pMsg->IssuerEvtID)
      {
        updateIndex = 0xFE;// reject it;
      }
      else
      {
        FLib_MemCpy(&gaClientCalorificValueTable[updateIndex], pMsg, sizeof(zclCmdPrice_PublishCalorificValueRsp_t));
      }
      break;
    }
  }
  /*if the Publish Calorific Value is not rejected, add it in the table */
  if(updateIndex == gNumofClientCalorificValue_c)
  {
    if(updateIndex != 0xfe)
    {
      /* the Calorific Value was updated */
      gaClientCalorificValueTable[updateIndex].EntryStatus = gPriceUpdateStatus_c;
      status = gZclSuccess_c;
    }
    else
    {
    /* Add the new Calorific Value information in the table */
    newEntry = AddCalorificValueInTable((publishCalorificValueEntry_t *)&gaClientCalorificValueTable[0], gNumofClientCalorificValue_c, pMsg);
    if (newEntry != 0xff)
      status = gZclSuccess_c; 
    }
  }
  
  //if(status == gZclSuccess_c)
    //TS_SendEvent(gZclTaskId, gzclEvtHandleClientPrices_c);
  
  return status;
}
#endif

/******************************************************************************/
/* Process the Get Conversion Factor Req; reply with the Publish Convertor
Factor for all currently scheduled periods*/
#if gASL_ZclPrice_Optionals_d && gASL_ZclSE_Use_NextGenFeatures_d
static zbStatus_t ZCL_ProcessGetConversioFactorReq
(
addrInfoType_t *pAddrInfo, 
zclCmdPrice_GetConversionFactorReq_t * pGetConversionFactor
)
{
  uint32_t startTime,eventStartTime;
  uint8_t nrOfEvents,i;
  bool_t eventFound;

  // coppy the address
  FLib_MemCpy(&mGetConversionFactorReqIndexs.addrInfo, pAddrInfo, sizeof(addrInfoType_t));
  // set index vector to 0x00 no index to send
  BeeUtilSetToF(&mGetConversionFactorReqIndexs.IndexVector,gNumofServerConversionFactors_c);
  mGetConversionFactorReqIndexs.Index=0x00;
  
  // get the start time
  startTime = OTA2Native32(pGetConversionFactor->StartTime);
  // get the number of events
  nrOfEvents = pGetConversionFactor->NumOfEvts;
  // if start time is 0x00000000 start time will be curent start time
  if(startTime == 0x00000000)
  {
    startTime = ZCL_GetUTCTime();
  }
  /* if number of events is 0 will return all available publish block periods 
  starting with th ecurren block in progress*/
  if(nrOfEvents == 0)
  {// set the nrOfEvents equal with gNumofServerPrices_c
    nrOfEvents=gNumofServerConversionFactors_c;
  }
  for(i = 0; i < gNumofServerConversionFactors_c; i++)
  {
    // if exits this entry
    if(gaServerConversionFactorTable[i].EntryStatus != 0x00)  
    {
      //get the eventStartTime
      eventStartTime = OTA2Native32(gaServerConversionFactorTable[i].conversionFactor.StartTime);
      if((startTime<=eventStartTime)&(nrOfEvents>0))
      {
        // make the flag true event has been found
        eventFound = TRUE;
        // add index in mGetBlockPeriodReqIndexs to send PublishBlockPeriod
        mGetConversionFactorReqIndexs.IndexVector[mGetConversionFactorReqIndexs.Index]=i;
        mGetConversionFactorReqIndexs.Index=mGetConversionFactorReqIndexs.Index+1;
        nrOfEvents=nrOfEvents-1;
      }
    }
  }
  if(eventFound == TRUE)
  {
    mGetConversionFactorReqIndexs.Index=0;
    SendPriceClusterEvt(gzclEvtHandleGetConversionFactor_c);
    return gZclSuccess_c;
  }
  else
  {
    return gZclNotFound_c;
  }
}
#endif
/******************************************************************************/
/* Process the Get Calorific Value Req; reply with the Publish Calorific
value for all currently scheduled periods*/
#if gASL_ZclPrice_Optionals_d && gASL_ZclSE_Use_NextGenFeatures_d
static zbStatus_t ZCL_ProcessGetCalorificValueReq
(
addrInfoType_t *pAddrInfo, 
zclCmdPrice_GetCalorificValueReq_t * pGetCalorificValue
)
{
  uint32_t startTime,eventStartTime;
  uint8_t nrOfEvents,i;
  bool_t eventFound;

  // coppy the address
  FLib_MemCpy(&mGetCalorificValueReqIndexs.addrInfo, pAddrInfo, sizeof(addrInfoType_t));
  // set index vector to 0x00 no index to send
  BeeUtilSetToF(&mGetCalorificValueReqIndexs.IndexVector,gNumofServerCalorificValue_c);
  mGetCalorificValueReqIndexs.Index=0x00;  
  
  // get the start time
  startTime = OTA2Native32(pGetCalorificValue->StartTime);
  // get the number of events
  nrOfEvents = pGetCalorificValue->NumOfEvts;
  // if start time is 0x00000000 start time will be curent start time
  if(startTime == 0x00000000)
  {
    startTime = ZCL_GetUTCTime();
  }
  /* if number of events is 0 will return all available publish block periods 
  starting with th ecurren block in progress*/
  if(nrOfEvents == 0)
  {// set the nrOfEvents equal with gNumofServerPrices_c
    nrOfEvents=gNumofServerCalorificValue_c;
  }
  for(i = 0; i < gNumofServerCalorificValue_c; i++)
  {
    // if exits this entry
    if(gaServerCalorificValueTable[i].EntryStatus != 0x00)  
    {
      //get the eventStartTime
      eventStartTime = OTA2Native32(gaServerCalorificValueTable[i].CalorificValue.StartTime);
      if((startTime<=eventStartTime)&(nrOfEvents>0))
      {
        // make the flag true event has been found
        eventFound = TRUE;
        // add index in mGetBlockPeriodReqIndexs to send PublishBlockPeriod
        mGetCalorificValueReqIndexs.IndexVector[mGetCalorificValueReqIndexs.Index]=i;
        mGetCalorificValueReqIndexs.Index=mGetCalorificValueReqIndexs.Index+1;
        nrOfEvents=nrOfEvents-1;
      }
    }
  }
  if(eventFound == TRUE)
  {
    mGetCalorificValueReqIndexs.Index=0;
    SendPriceClusterEvt(gzclEvtHandleGetCalorificValue_c);
    return gZclSuccess_c;
  }
  else
  {
    return gZclNotFound_c;
  }
}
/******************************************************************************/
/* The Publish Conversion Factor command is generated in response to receiving a Get Conversion Factor
command or if a new conversion factor is available. */
static zbStatus_t ZCL_SendPublishConversionFactor(addrInfoType_t *pAddrInfo, publishConversionFactorEntry_t * pMsg) 
{
  zclPrice_PublishConversionFactorRsp_t req;  
  uint32_t currentTime;
  /* Get the current time */
  currentTime = ZCL_GetUTCTime();
  currentTime = Native2OTA32(currentTime);
  FLib_MemCpy(&req.addrInfo, pAddrInfo, sizeof(afAddrInfo_t));	
  req.zclTransactionId =  gZclTransactionId++;
  FLib_MemCpy(&req.cmdFrame, pMsg, sizeof(zclCmdPrice_PublishConversionFactorRsp_t));
  return zclPrice_PublishConversionFactorRsp(&req);
}

/******************************************************************************/
/* The Publish Calorific Value command is generated in response to receiving a Get 
Calorific Value of if a new Calorific value is available.*/
static zbStatus_t ZCL_SendPublishCalorificValue(addrInfoType_t *pAddrInfo, publishCalorificValueEntry_t * pMsg) 
{
  zclPrice_PublishCalorificValueRsp_t req;
  uint32_t currentTime;
  /* Get the current time */
  currentTime = ZCL_GetUTCTime();
  currentTime = Native2OTA32(currentTime);
  FLib_MemCpy(&req.addrInfo, pAddrInfo, sizeof(afAddrInfo_t));	
  req.zclTransactionId =  gZclTransactionId++;
  FLib_MemCpy(&req.cmdFrame, pMsg, sizeof(zclCmdPrice_PublishCalorificValueRsp_t));
  return zclPrice_PublishCalorificValueRsp(&req);
}
#endif

/******************************************************************************/
/* The Publish Block Threshold command is generated in response to receiving a Get Block Threshold
command.*/
#if gASL_ZclSE_Use_NextGenFeatures_d
static zbStatus_t ZCL_SendPublishBlockThresholds(addrInfoType_t *pAddrInfo, publishBlockThresholdsEntry_t * pMsg) 
{
  
  zclPrice_PublishBlockThresholdsRsp_t *pReq;   
  uint8_t noBlockThresholdInUse, noBlocksPerFrame, tariffEntryIdx;
  zbStatus_t status;
  
   /* Find how much prices can fit into the packet */
  tariffEntryIdx = FindServerTariffInformationEntryByIssuerId(&pMsg->IssuerEvtID);
  noBlockThresholdInUse = gaServerTariffInformationTable[tariffEntryIdx].TariffInformation.NumberBlockThreshholdsUse;
  if (noBlockThresholdInUse == 0)
    return gZclFailure_c;
  
  noBlocksPerFrame = (uint8_t)(GetMaxApplicationPayload(NULL) - sizeof(zclFrame_t) - sizeof(SEEvtId_t)) / sizeof(BlockThreshold_t);
  
  if (((mGetBlockThresholdsReqEvtsIdx.CmdIndex + 1) * noBlocksPerFrame) < noBlockThresholdInUse)
    mGetBlockThresholdsReqEvtsIdx.Length = noBlocksPerFrame;
  else  
    mGetBlockThresholdsReqEvtsIdx.Length = noBlockThresholdInUse - (mGetBlockThresholdsReqEvtsIdx.CmdIndex * noBlocksPerFrame);
  
  /* Allocate buffer for OTA frame*/
  pReq = MSG_Alloc(sizeof(zclPrice_PublishBlockThresholdsRsp_t) + (mGetBlockThresholdsReqEvtsIdx.Length- 1) * sizeof(BlockThreshold_t));
  if(!pReq)
    return gZbNoMem_c;
  
  /* Add application payload*/
  FLib_MemCpy(&pReq->addrInfo, pAddrInfo, sizeof(afAddrInfo_t));	
  pReq->zclTransactionId =  gZclTransactionId++;
  FLib_MemCpy(pReq->cmdFrame.IssuerEvtID, pMsg->IssuerEvtID, sizeof(SEEvtId_t));
  pReq->cmdFrame.CommandIdx = mGetBlockThresholdsReqEvtsIdx.CmdIndex;
  FLib_MemCpy(pReq->cmdFrame.BlockThresholds[0], &pMsg->BlockThresholds[mGetBlockThresholdsReqEvtsIdx.Offset], mGetBlockThresholdsReqEvtsIdx.Length * sizeof(BlockThreshold_t));
 
  /* Send request OTA*/
  status = zclPrice_PublishBlockThresholdsRsp(pReq);
  MSG_Free(pReq);
  return status;
 
}
#endif

/******************************************************************************/
/* The Publish Price Matrix command is generated in response to receiving a Get Price matrix
command.*/
#if gASL_ZclSE_Use_NextGenFeatures_d
static zbStatus_t ZCL_SendPublishPriceMatrix(addrInfoType_t *pAddrInfo, publishPriceMatrixEntry_t * pMsg) 
{
  
  zclPrice_PublishPriceMatrixRsp_t *pReq;   
  uint8_t noPriceTiersInUse, noPriceBlocksInUse, noPricesPerFrame, tariffEntryIdx;
  zbStatus_t status;
  
   /* Find how much prices can fit into the packet */
  tariffEntryIdx = FindServerTariffInformationEntryByIssuerId(&pMsg->IssuerEvtID);
  noPriceTiersInUse = gaServerTariffInformationTable[tariffEntryIdx].TariffInformation.NumberPriceTiersUse;
  noPriceBlocksInUse = gaServerTariffInformationTable[tariffEntryIdx].TariffInformation.NumberBlockThreshholdsUse + 1;
  noPricesPerFrame = (uint8_t)(GetMaxApplicationPayload(NULL) - sizeof(zclFrame_t) - sizeof(SEEvtId_t) - 1) / sizeof(Price_t);
  
  if (((mGetPriceMatrixReqEvtsIdx.CmdIndex + 1) * noPricesPerFrame) < (noPriceTiersInUse * noPriceBlocksInUse))
    mGetPriceMatrixReqEvtsIdx.Length = noPricesPerFrame;
  else  
    mGetPriceMatrixReqEvtsIdx.Length = (noPriceTiersInUse * noPriceBlocksInUse) - (mGetPriceMatrixReqEvtsIdx.CmdIndex * noPricesPerFrame);
  
  /* Allocate buffer for OTA frame*/
  pReq = MSG_Alloc(sizeof(zclPrice_PublishPriceMatrixRsp_t) + (mGetPriceMatrixReqEvtsIdx.Length- 1) * sizeof(Price_t));
  if(!pReq)
    return gZbNoMem_c;
  
  /* Add application payload*/
  FLib_MemCpy(&pReq->addrInfo, pAddrInfo, sizeof(afAddrInfo_t));	
  pReq->zclTransactionId =  gZclTransactionId++;
  FLib_MemCpy(pReq->cmdFrame.IssuerEvtID, pMsg->IssuerEvtID, sizeof(SEEvtId_t));
  pReq->cmdFrame.CommandIdx = mGetPriceMatrixReqEvtsIdx.CmdIndex;
  FLib_MemCpy(pReq->cmdFrame.TierBlockPrice[0], &pMsg->TierBlockPrice[mGetPriceMatrixReqEvtsIdx.Offset], mGetPriceMatrixReqEvtsIdx.Length * sizeof(Price_t));
 
  /* Send request OTA*/
  status = zclPrice_PublishPriceMatrixRsp(pReq);
  MSG_Free(pReq);
  return status;
 
}
#endif

/******************************************************************************/
/* The Publish Tariff Information command is generated in response to receiving a Get Tariff Information
command or when an update to the Tariff Information is available
from the commodity provider. */
#if gASL_ZclSE_Use_NextGenFeatures_d
static zbStatus_t ZCL_SendPublishTariffInformation(addrInfoType_t *pAddrInfo, publishTariffInformationEntry_t * pMsg) 
{
  zclPrice_PublishTariffInformationRsp_t req;    
  FLib_MemCpy(&req.addrInfo, pAddrInfo, sizeof(afAddrInfo_t));	
  req.zclTransactionId =  gZclTransactionId++;
  FLib_MemCpy(&req.cmdFrame, pMsg, sizeof(zclCmdPrice_PublishTariffInformationRsp_t));
  return zclPrice_PublishTariffInformationRsp(&req);
}
#endif


/******************************************************************************/
/* The Publish CO2 Value command is generated in response to receiving a Get CO2 Value
command or when an update to the CO2 Value information is available
from the commodity provider. */
#if gASL_ZclSE_Use_NextGenFeatures_d
static zbStatus_t ZCL_SendPublishCO2Value(addrInfoType_t *pAddrInfo, publishCO2ValueEntry_t * pMsg) 
{
  zclPrice_PublishCO2ValueRsp_t req;    
  FLib_MemCpy(&req.addrInfo, pAddrInfo, sizeof(afAddrInfo_t));	
  req.zclTransactionId =  gZclTransactionId++;
  FLib_MemCpy(&req.cmdFrame, pMsg, sizeof(zclCmdPrice_PublishCO2ValueRsp_t));
  return zclPrice_PublishCO2ValueRsp(&req);
}
#endif


/******************************************************************************/
/* The Publish CPP Event command is generated in response to receiving a Get CPP Event
command or when an update to the billing period information is available
from the commodity provider. */
#if gASL_ZclSE_Use_NextGenFeatures_d
static zbStatus_t ZCL_SendPublishCPPEvent(addrInfoType_t *pAddrInfo, publishCPPEventEntry_t * pMsg) 
{
  zclPrice_PublishCPPEventRsp_t req;    
  zclCmdPrice_PublishCPPEventRsp_t cmdFrame;
  uint32_t currentTime, stopTime, newDuration;
  uint16_t duration;
 
  currentTime = ZCL_GetUTCTime();
  
  FLib_MemCpy(&cmdFrame.IssuerEvtID, &pMsg->CPPEvent.IssuerEvtID, sizeof(SEEvtId_t));
  cmdFrame.CPPEventStartTime = pMsg->CPPEvent.CPPEventStartTime;
  if((pMsg->CPPEvent.CPPEventStartTime) == 0x00000000)
  {
    /* Get the duration value */

    duration = GetNative16BitInt(pMsg->CPPEvent.CPPEventDurationInMinutes);
    stopTime = Native2OTA32(pMsg->EffectiveStartTime) + ( 60 *  duration);
    newDuration = (stopTime - currentTime) / 60;
    FLib_MemCpy(&cmdFrame.CPPEventDurationInMinutes, &newDuration, sizeof(Duration16_t));
  }
  else
  {
    FLib_MemCpy(&cmdFrame.CPPEventDurationInMinutes, &pMsg->CPPEvent.CPPEventDurationInMinutes, sizeof(Duration16_t));
  }

  cmdFrame.TariffType = pMsg->CPPEvent.TariffType;
  cmdFrame.CPPPriceTier = pMsg->CPPEvent.CPPPriceTier;
  cmdFrame.CPPAuth = pMsg->CPPEvent.CPPAuth;
 
  FLib_MemCpy(&req.addrInfo, pAddrInfo, sizeof(afAddrInfo_t));
  req.zclTransactionId = gZclTransactionId++;
  FLib_MemCpy(&req.cmdFrame, &cmdFrame, sizeof(zclCmdPrice_PublishCPPEventRsp_t));
  
  return zclPrice_PublishCPPEventRsp(&req);
}
#endif

/******************************************************************************/
/* The Publish Billing Period command is generated in response to receiving a Get Billing Period
command or when an update to the billing period information is available
from the commodity provider. */
#if gASL_ZclSE_Use_NextGenFeatures_d
static zbStatus_t ZCL_SendPublishBillingPeriod(addrInfoType_t *pAddrInfo, publishBillingPeriodEntry_t * pMsg) 
{
  zclPrice_PublishBillingPeriodRsp_t req;    
  zclCmdPrice_PublishBillingPeriodRsp_t cmdFrame;
  uint32_t currentTime, stopTime, newDuration, duration;
  
  currentTime = ZCL_GetUTCTime();
  
  FLib_MemCpy(&cmdFrame.IssuerEvtID, &pMsg->BillingPeriod.IssuerEvtID, sizeof(SEEvtId_t));
  cmdFrame.BillingPeriodStartTime = pMsg->BillingPeriod.BillingPeriodStartTime;
  if((pMsg->BillingPeriod.BillingPeriodStartTime) == 0x00000000)
  {
    /* Get the duration value */
    duration = GetNative32BitIntFrom3ByteArray(pMsg->BillingPeriod.BillingPeriodDurationInMinutes);
    stopTime = Native2OTA32(pMsg->EffectiveStartTime) + ( 60 *  duration);
    newDuration = (stopTime - currentTime) / 60;
    FLib_MemCpy(&cmdFrame.BillingPeriodDurationInMinutes, &newDuration, sizeof(Duration24_t));
  }
  else
  {
    FLib_MemCpy(&cmdFrame.BillingPeriodDurationInMinutes, &pMsg->BillingPeriod.BillingPeriodDurationInMinutes, sizeof(Duration24_t));
  }
  cmdFrame.TariffType = pMsg->BillingPeriod.TariffType;
 
  FLib_MemCpy(&req.addrInfo, pAddrInfo, sizeof(afAddrInfo_t));
  req.zclTransactionId =  gZclTransactionId++;
  FLib_MemCpy(&req.cmdFrame, &cmdFrame, sizeof(zclCmdPrice_PublishBillingPeriodRsp_t));
  
  return zclPrice_PublishBillingPeriodRsp(&req);
}
#endif

/******************************************************************************/
/* The Publish Consolidated Bill command is generated in response to receiving a Get COnsolidated Bill
command or when an update to the billing period information is available
from the commodity provider. */
#if gASL_ZclSE_Use_NextGenFeatures_d
static zbStatus_t ZCL_SendPublishConsolidatedBill(addrInfoType_t *pAddrInfo, publishConsolidatedBillEntry_t * pMsg) 
{
  zclPrice_PublishConsolidatedBillRsp_t req;    
  zclCmdPrice_PublishConsolidatedBillRsp_t cmdFrame;
  uint32_t currentTime, stopTime, newDuration,duration;
  
  currentTime = ZCL_GetUTCTime();
  
  FLib_MemCpy(&cmdFrame.IssuerEvtID, &pMsg->ConsolidatedBill.IssuerEvtID, sizeof(SEEvtId_t));
  cmdFrame.BillingPeriodStartTime = pMsg->ConsolidatedBill.BillingPeriodStartTime;
  if((pMsg->ConsolidatedBill.BillingPeriodStartTime) == 0x00000000)
  {
    /* Get the duration value */
    duration = GetNative32BitIntFrom3ByteArray(pMsg->ConsolidatedBill.BillingPeriodDurationInMinutes);
    stopTime = Native2OTA32(pMsg->EffectiveStartTime) + ( 60 *  duration);
    newDuration = (stopTime - currentTime) / 60;
    FLib_MemCpy(&cmdFrame.BillingPeriodDurationInMinutes, &newDuration, sizeof(Duration24_t));
  }
  else
  {
    FLib_MemCpy(&cmdFrame.BillingPeriodDurationInMinutes, &pMsg->ConsolidatedBill.BillingPeriodDurationInMinutes, sizeof(Duration24_t));
  }
  
  cmdFrame.TariffType = pMsg->ConsolidatedBill.TariffType;
  FLib_MemCpy(&cmdFrame.ConsolidatedBill, &pMsg->ConsolidatedBill.ConsolidatedBill, sizeof(Bill_t));
  FLib_MemCpy(&cmdFrame.Currency, &pMsg->ConsolidatedBill.Currency, sizeof(Currency_t));
  cmdFrame.BillTrailingDigit = pMsg->ConsolidatedBill.BillTrailingDigit;
 
  FLib_MemCpy(&req.addrInfo, pAddrInfo, sizeof(afAddrInfo_t));
  req.zclTransactionId =  gZclTransactionId++;
  FLib_MemCpy(&req.cmdFrame, &cmdFrame, sizeof(zclCmdPrice_PublishConsolidatedBillRsp_t));
  
  return zclPrice_PublishConsolidatedBillRsp(&req);
}
#endif

#if gInterPanCommunicationEnabled_c
/******************************************************************************/
static void StoreInterPanAddr(InterPanAddrInfo_t *pAddrInfo)
{
  uint8_t i, newEntry = 0xff;
  
  for(i = 0; i < gNumOfInterPanAddr_c; i++)
  {
    /* If PanID already exist in the table, overwrite the existing entry*/
    if(gaInterPanAddrTable[i].entryStatus != 0x00 &&
       FLib_MemCmp(&(pAddrInfo->dstPanId), &(gaInterPanAddrTable[i].addrInfo.dstPanId), sizeof(zbPanId_t)) )
    {
      FLib_MemCpy(&gaInterPanAddrTable[i], pAddrInfo, sizeof(InterPanAddrInfo_t));
      return;
    }
    if(newEntry == 0xff && 
       gaInterPanAddrTable[i].entryStatus ==0x00)
      newEntry = i;
  }
  
  if(newEntry != 0xff)
  {
    FLib_MemCpy(&gaInterPanAddrTable[newEntry], pAddrInfo, sizeof(InterPanAddrInfo_t));
    gaInterPanAddrTable[newEntry].entryStatus = 0xff;
  } 
}
#endif

/******************************************************************************/
static void RegisterDevForPrices(afAddrInfo_t *pAddrInfo)
{
  uint8_t *pIEEE, aExtAddr[8];
  uint8_t i, newEntry = 0xff;
  
  pIEEE = APS_GetIeeeAddress((uint8_t*)&pAddrInfo->dstAddr.aNwkAddr, aExtAddr);
  if(!pIEEE)
    return;
  
  for(i = 0; i < gNumOfPriceAddr_c; i++)
  {
    if(gaPriceAddrTable[i].entryStatus != 0x00 &&
       Cmp8Bytes(pIEEE, (uint8_t*)&gaPriceAddrTable[i].iEEEaddr))
    {
      FLib_MemCpy(&gaPriceAddrTable[i].addrInfo, pAddrInfo,sizeof(afAddrInfo_t));
      return;
    }
    if(newEntry == 0xff && 
       gaPriceAddrTable[i].entryStatus == 0x00)
      newEntry = i;
  }
  
  if(newEntry != 0xff)
  {
    FLib_MemCpy(&gaPriceAddrTable[newEntry].iEEEaddr, pIEEE, 8);
    FLib_MemCpy(&gaPriceAddrTable[newEntry].addrInfo, pAddrInfo, sizeof(afAddrInfo_t));
    gaPriceAddrTable[newEntry].entryStatus = 0xff;
  } 
  
}

#if gInterPanCommunicationEnabled_c
/******************************************************************************/
static void InterPanJitterTimerCallBack(tmrTimerID_t timerID)
{
  uint8_t status;

  (void)timerID;
  gaInterPanAddrTable[mInterPanIndex].addrInfo.dstAddrMode = gAddrModeShort_c; 
  BeeUtilSetToF(&gaInterPanAddrTable[mInterPanIndex].addrInfo.dstAddr, sizeof(zbNwkAddr_t)); ///broadcast address 
  status = ZCL_SendPublishPrice((addrInfoType_t *)&gaInterPanAddrTable[mInterPanIndex],
                                &gaServerPriceTable[mUpdatePriceIndex], TRUE);
  if(status == gZclSuccess_c)
    ++mInterPanIndex;
  SendPriceClusterEvt(gzclEvtHandlePublishPriceUpdate_c);
}
#endif /* #if gInterPanCommunicationEnabled_c */

/******************************************************************************/
/* Handle the Price Update */
void ZCL_HandlePublishPriceUpdate(void)
{
  uint8_t status;
  
  /* Check the Price addr table for SE device and send the publish price */
  if(mRegIndex < gNumOfPriceAddr_c)
  {
    if(gaPriceAddrTable[mRegIndex].entryStatus)
    {
      status = ZCL_SendPublishPrice((addrInfoType_t *)&gaPriceAddrTable[mRegIndex].addrInfo,
                                    &gaServerPriceTable[mUpdatePriceIndex], FALSE);
      if(status == gZclSuccess_c)
	       ++mRegIndex;
      SendPriceClusterEvt(gzclEvtHandlePublishPriceUpdate_c);
      return;
      
    }
    
  }
#if gInterPanCommunicationEnabled_c  
{ 
  static tmrTimerID_t interPanTimerID = 0x00;
  uint16_t randomTime;
    
  /* Check the InterPan Price addr table for non-Se device and send the publish price */  
  if(mInterPanIndex < gNumOfInterPanAddr_c)
  {
    if(gaInterPanAddrTable[mInterPanIndex].entryStatus != 0x00)
    { 
      if(!interPanTimerID)
        interPanTimerID = TMR_AllocateTimer();
      if(interPanTimerID)
      {  
        randomTime = (uint16_t)gInterPanMaxJitterTime_c/10;
        randomTime = (uint16_t)GetRandomRange(0, (uint8_t)randomTime) * 10;
        TMR_StartSingleShotTimer (interPanTimerID, randomTime, InterPanJitterTimerCallBack);
      } 
    }
    
  }
}  
#endif /* #if gInterPanCommunicationEnabled_c */
}


/******************************************************************************/
/* Process the Get Current Price Req; reply with the active scheduled price */
static zbStatus_t ZCL_ProcessGetCurrPriceReq
(
addrInfoType_t *pAddrInfo,
zclCmdPrice_GetCurrPriceReq_t * pGetCurrPrice,
bool_t IsInterPanFlag)
{
  uint8_t i;
  uint32_t currentTime, startTime, stopTime;
  uint16_t duration;
  
  pGetCurrPrice->CmdOptions = pGetCurrPrice->CmdOptions & gGetCurrPrice_RequestorRxOnWhenIdle_c;
  
  if(pGetCurrPrice->CmdOptions)
  {
    if(IsInterPanFlag)
    {
#if gInterPanCommunicationEnabled_c       
      //Store InterPanAddr 
      StoreInterPanAddr((InterPanAddrInfo_t *)pAddrInfo);
#else
      return gZclFailure_c;
#endif /* #if gInterPanCommunicationEnabled_c  */
      
    }
    else
    {
      //set the device as being registered having the Rx on Idle(to send Price update) 
      RegisterDevForPrices((afAddrInfo_t *)pAddrInfo);
    }
  }
   /* here get the currentTime */
  currentTime = ZCL_GetUTCTime();
  /* reply with the active scheduled price */
  for(i = 0; i < gNumofServerPrices_c; i++)
  {
    if(gaServerPriceTable[i].EntryStatus != 0x00)
    {
      startTime = OTA2Native32(gaServerPriceTable[i].Price.StartTime);
      duration = OTA2Native16(gaServerPriceTable[i].Price.DurationInMinutes);
      stopTime = startTime + ((uint32_t)duration*60);
      if((startTime == 0x00000000) || (startTime == 0xFFFFFFFF) ||
         ((startTime <= currentTime) && (currentTime <= stopTime)))
	        return ZCL_SendPublishPrice(pAddrInfo, &gaServerPriceTable[i], IsInterPanFlag);	
    }
  }
  
  return gZclNotFound_c;  
}


/******************************************************************************/
#if gASL_ZclPrice_Optionals_d 
void ZCL_HandleGetScheduledPrices(void)
{
  uint32_t  startTime;
  uint8_t status;
  
  if(mIndex < gNumofServerPrices_c)
  {
    /* Check if the entry is used and if there are more scheduled price to be send*/
    if((gaServerPriceTable[mIndex].EntryStatus != 0x00) && mGetNumOfPriceEvts)
    {
      startTime = OTA2Native32(gaServerPriceTable[mIndex].EffectiveStartTime);
      if(mGetPriceStartTime <= startTime)
      {
        /* Send This Pubish Price */
       	status = ZCL_SendPublishPrice(&mAddrPriceInfo, &gaServerPriceTable[mIndex], mIsInterPanFlag);
      	if(status == gZclSuccess_c)
      	{
          --mGetNumOfPriceEvts;
      	}
        
      }
      /* GO and send the next price */
      mIndex++;
      SendPriceClusterEvt(gzclEvtHandleGetScheduledPrices_c);
    }
    
  }
}
#endif
/******************************************************************************/
#if gASL_ZclPrice_Optionals_d 
static zbStatus_t ZCL_ProcessGetScheduledPricesReq
(
addrInfoType_t *pAddrInfo, 
zclCmdPrice_GetScheduledPricesReq_t * pGetScheduledPrice, 
bool_t IsInterPanFlag)
{
  FLib_MemCpy(&mAddrPriceInfo, pAddrInfo, sizeof(addrInfoType_t));

  mGetPriceStartTime = OTA2Native32(pGetScheduledPrice->StartTime);
  mGetNumOfPriceEvts = pGetScheduledPrice->NumOfEvts;
  if (!mGetNumOfPriceEvts)
  {
    mGetNumOfPriceEvts = gNumofServerPrices_c;
    mGetPriceStartTime = 0x00000000; /*all price information*/
  }
  mIsInterPanFlag = IsInterPanFlag;
  mIndex = 0;
  SendPriceClusterEvt(gzclEvtHandleGetScheduledPrices_c);
  return gZclSuccess_c;
} 
#endif

/******************************************************************************/
#if gASL_ZclSE_Use_NextGenFeatures_d 
/* Find an Event in the Events Table...  */
uint8_t FindNextBillingPeriodEvent(void)
{
  uint8_t i; 
  ZCLTime_t eventStartTime, eventStopTime;
  uint32_t eventDuration;
  
  for(i = mGetBillingPeriodReqEvtsIdx.Index; i < gNumofServerBillingPeriods_c; i++)
  {
    /* If this entry exists */
    if(gaServerBillingPeriodTable[i].EntryStatus != 0x00)  
    {
      if ((mGetBillingPeriodReqEvtsIdx.tariffType != 0xFF) &&
          (gaServerBillingPeriodTable[i].BillingPeriod.TariffType != mGetBillingPeriodReqEvtsIdx.tariffType))
      {
        continue;
      }
      
      /* Get start time */
      eventStartTime = OTA2Native32(gaServerBillingPeriodTable[i].BillingPeriod.BillingPeriodStartTime);
      if(eventStartTime == 0x00000000)
      {
        eventStartTime = OTA2Native32(gaServerBillingPeriodTable[i].EffectiveStartTime);
      }
      /* Get the eventDuration */
      eventDuration = GetNative32BitIntFrom3ByteArray(gaServerBillingPeriodTable[i].BillingPeriod.BillingPeriodDurationInMinutes);
      
      /* Get the eventStopTime */
      eventStopTime = eventStartTime + ((uint32_t)eventDuration * 60);
      /* Check if entry satisfies condition*/
      if((mGetBillingPeriodReqEvtsIdx.startTime <= eventStopTime))
      {
        return i;
      }
    }
  }
  return 0xFF; 
}   
#endif

/******************************************************************************/
#if gASL_ZclSE_Use_NextGenFeatures_d 
/* Find an Event in the Events Table...  */
uint8_t FindNextTariffInformationEvent(void)
{
  uint8_t i; 
  ZCLTime_t eventStartTime;
  
  for(i = mGetTariffInformationReqEvtsIdx.Index; i < gNumofServerTariffInformation_c; i++)
  {
    /* If this entry exists */
    if(gaServerTariffInformationTable[i].EntryStatus != 0x00)  
    {
      if ((mGetTariffInformationReqEvtsIdx.tariffType != 0xFF) &&
          (gaServerTariffInformationTable[i].TariffInformation.TariffType != mGetTariffInformationReqEvtsIdx.tariffType))
      {
        continue;
      }
      
      /* Get start time */
      eventStartTime = OTA2Native32(gaServerTariffInformationTable[i].TariffInformation.StartTime);
      if(eventStartTime == 0x00000000)
      {
        eventStartTime = OTA2Native32(gaServerTariffInformationTable[i].EffectiveStartTime);
      }
      
      /* Check if entry satisfies condition */
      if((mGetTariffInformationReqEvtsIdx.startTime > eventStartTime))
      {
        /* Check if it is the active tariff */
        if (((i + 1) == gNumofServerTariffInformation_c) || 
          (gaServerTariffInformationTable[i+1].EffectiveStartTime > mGetTariffInformationReqEvtsIdx.startTime) ||
           gaServerTariffInformationTable[i+1].EntryStatus == gEntryNotUsed_c )              
          return i;
      }
      else
      {
        /* It is a scheduled tariff */
        return i;
      }
    }
  }
  return 0xFF; 
}   
#endif

/******************************************************************************/
#if gASL_ZclSE_Use_NextGenFeatures_d 
/* Find an Event in the Events Table...  */
uint8_t FindNextCO2ValueEvent(void)
{
  uint8_t i; 
  ZCLTime_t eventStartTime;
  
  for(i = mGetCO2ValueReqEvtsIdx.Index; i < gNumofServerCO2Value_c; i++)
  {
    /* If this entry exists */
    if(gaServerCO2ValueTable[i].EntryStatus != 0x00)  
    {
      if ((mGetCO2ValueReqEvtsIdx.tariffType != 0xFF) &&
          (gaServerCO2ValueTable[i].CO2Value.TariffType != mGetCO2ValueReqEvtsIdx.tariffType))
      {
        continue;
      }
      
      /* Get start time */
      eventStartTime = OTA2Native32(gaServerCO2ValueTable[i].CO2Value.StartTime);
      if(eventStartTime == 0x00000000)
      {
        eventStartTime = OTA2Native32(gaServerCO2ValueTable[i].EffectiveStartTime);
      }
      
      /* Check if entry satisfies condition*/
      if((mGetCO2ValueReqEvtsIdx.startTime >= eventStartTime))
      {
        return i;
      }
    }
  }
  return 0xFF; 
}   
#endif

/******************************************************************************/
#if gASL_ZclSE_Use_NextGenFeatures_d 
/* Find an Event in the Events Table...  */
uint8_t FindNextConsolidatedBillEvent(void)
{
  uint8_t i; 
  ZCLTime_t eventStartTime, eventStopTime;
  uint32_t eventDuration;
  
  for(i = mGetConsolidatedBillReqEvtsIdx.Index; i < gNumofServerConsolidatedBill_c; i++)
  {
    /* If this entry exists */
    if(gaServerConsolidatedBillTable[i].EntryStatus != 0x00)  
    {
      if ((mGetConsolidatedBillReqEvtsIdx.tariffType != 0xFF) &&
          (gaServerConsolidatedBillTable[i].ConsolidatedBill.TariffType != mGetConsolidatedBillReqEvtsIdx.tariffType))
      {
        continue;
      }
      
      /* Get start time */
      eventStartTime = OTA2Native32(gaServerConsolidatedBillTable[i].ConsolidatedBill.BillingPeriodStartTime);
      if(eventStartTime == 0x00000000)
      {
        eventStartTime = OTA2Native32(gaServerConsolidatedBillTable[i].EffectiveStartTime);
      }
      /* Get the eventDuration */
      eventDuration = GetNative32BitIntFrom3ByteArray(gaServerConsolidatedBillTable[i].ConsolidatedBill.BillingPeriodDurationInMinutes);
      
      /* Get the eventStopTime */
      eventStopTime = eventStartTime + (eventDuration * 60);
      /* Check if entry satisfies condition*/
      if((mGetConsolidatedBillReqEvtsIdx.startTime <= eventStopTime))
      {
        return i;
      }
    }
  }
  return 0xFF; 
}   
#endif

/******************************************************************************/
/* Process CPP Event Response; reply with the Publish CPP Event */
#if gASL_ZclSE_Use_NextGenFeatures_d 
static zbStatus_t ZCL_ProcessCPPEventRsp
(
  addrInfoType_t *pAddrInfo, 
  zclCmdPrice_CPPEventRsp_t * pCPPEventResponse
)
{
  zbStatus_t status = gZclSuccess_c;
  uint8_t i;
  uint8_t index = 0xFF;
  
  for(i = 0; i < gNumofServerCPPEvents_c; i++)
  {    
    if(FLib_MemCmp(&gaServerCPPEventTable[i].CPPEvent.IssuerEvtID, &pCPPEventResponse->IssuerEvtID, sizeof(SEEvtId_t)))
      index = i;
  }
  
  if (index == 0xFF)
    return gZclFailure_c;
       
  /* Check the status of the Authorization*/
  if (pCPPEventResponse->CPPAuth == gPriceCPPEventAccepted)
  {
    gaServerCPPEventTable[index].CPPEvent.CPPAuth = gPriceCPPEventForced;
    status = ZCL_SendPublishCPPEvent(pAddrInfo, &gaServerCPPEventTable[index]);
  }
  return status;
}
#endif

/******************************************************************************/
/* Process the Get Billing Period Req; reply with the Publish Billing Period for all
currently scheduled periods*/
#if gASL_ZclSE_Use_NextGenFeatures_d 
static zbStatus_t ZCL_ProcessGetBillingPeriodReq
(
addrInfoType_t *pAddrInfo, 
zclCmdPrice_GetBillingPeriodReq_t * pGetBillingPeriod
)
{
  ZCLTime_t startTime;
  uint8_t entryIndex;
  
  /* Copy the address*/
  FLib_MemCpy(&mGetBillingPeriodReqEvtsIdx.addrInfo, pAddrInfo, sizeof(addrInfoType_t));
  mGetBillingPeriodReqEvtsIdx.Index = 0x00;
  mGetBillingPeriodReqEvtsIdx.numOfEvtsRemaining = pGetBillingPeriod->NumOfEvts;
  mGetBillingPeriodReqEvtsIdx.tariffType = pGetBillingPeriod->TariffType;
  
  /* Check valid value for tarif type*/
  if ((mGetBillingPeriodReqEvtsIdx.tariffType > 0x02) &&
      (mGetBillingPeriodReqEvtsIdx.tariffType != 0xFF))
  {
    return gZclFailure_c;
  }
  /* Get the start time */
  startTime = OTA2Native32(pGetBillingPeriod->StartTime);
  
  /* If start time is 0x00000000 replace with current start time */
  if(startTime == 0x00000000)
  {
    startTime = ZCL_GetUTCTime();
  }
  mGetBillingPeriodReqEvtsIdx.startTime = startTime;
  
  /* If number of events is 0 we will return all valid entries*/
  if(mGetBillingPeriodReqEvtsIdx.tariffType == 0)
  {
    mGetBillingPeriodReqEvtsIdx.numOfEvtsRemaining = gNumofServerBillingPeriods_c;
  }
      
  /* Find next valid entry */
  entryIndex = FindNextBillingPeriodEvent();
  if(entryIndex != 0xFF)
  {
    mGetBillingPeriodReqEvtsIdx.Index = entryIndex;
    SendPriceClusterEvt(gzclEvtHandleGetBillingPeriod_c);
    return gZclSuccess_c;
  }
  else
  {
    return gZclNotFound_c;
  }
}
#endif

/******************************************************************************/
/* Process the Get Consolidated Bill Req; reply with the Publish Consolidated Bill for all
currently scheduled bills*/
#if gASL_ZclSE_Use_NextGenFeatures_d 
static zbStatus_t ZCL_ProcessGetConsolidatedBillReq
(
addrInfoType_t *pAddrInfo, 
zclCmdPrice_GetConsolidatedBillReq_t * pGetConsolidatedBill
)
{
  ZCLTime_t startTime;
  uint8_t entryIndex;
  
  /* Copy the address*/
  FLib_MemCpy(&mGetConsolidatedBillReqEvtsIdx.addrInfo, pAddrInfo, sizeof(addrInfoType_t));
  mGetConsolidatedBillReqEvtsIdx.Index = 0x00;
  mGetConsolidatedBillReqEvtsIdx.numOfEvtsRemaining = pGetConsolidatedBill->NumOfEvts;
  mGetConsolidatedBillReqEvtsIdx.tariffType = pGetConsolidatedBill->TariffType;
  
  /* Check valid value for tarif type*/
  if ((mGetConsolidatedBillReqEvtsIdx.tariffType > 0x02) &&
      (mGetConsolidatedBillReqEvtsIdx.tariffType != 0xFF))
  {
    return gZclFailure_c;
  }
  /* Get the start time */
  startTime = OTA2Native32(pGetConsolidatedBill->StartTime);
  
  /* If start time is 0x00000000 replace with current start time */
  if(startTime == 0x00000000)
  {
    startTime = ZCL_GetUTCTime();
  }
  mGetConsolidatedBillReqEvtsIdx.startTime = startTime;
  
  /* If number of events is 0 we will return all valid entries*/
  if(mGetConsolidatedBillReqEvtsIdx.tariffType == 0)
  {
    mGetConsolidatedBillReqEvtsIdx.numOfEvtsRemaining = gNumofServerConsolidatedBill_c;
  }
      
  /* Find next valid entry */
  entryIndex = FindNextConsolidatedBillEvent();
  if(entryIndex != 0xFF)
  {
    mGetConsolidatedBillReqEvtsIdx.Index = entryIndex;
    SendPriceClusterEvt(gzclEvtHandleGetConsolidatedBill_c);
    return gZclSuccess_c;
  }
  else
  {
    return gZclNotFound_c;
  }
}
#endif

/******************************************************************************/
/* Process the Get Block Threshold Req; reply with the Publish BlockThreshold for all
currently stored values*/
#if gASL_ZclSE_Use_NextGenFeatures_d 
static zbStatus_t ZCL_ProcessGetBlockThresholdsReq
(
addrInfoType_t *pAddrInfo, 
zclCmdPrice_GetBlockThresholdsReq_t * pGetBlockThresholds
)
{
  uint8_t entryIdx;
  
  mGetBlockThresholdsReqEvtsIdx.Index = 0xFF;
  
  /* Find entry in Price Matrix table*/  
  for (entryIdx = 0; entryIdx < gNumofServerBlockThresholds_c; entryIdx++)
  {
    if (FLib_MemCmp(gaServerBlockThresholdsTable[entryIdx].IssuerEvtID, pGetBlockThresholds->IssuerEvtID, sizeof(SEEvtId_t)))
    {
      mGetBlockThresholdsReqEvtsIdx.Index = entryIdx;
      break;
    }
  }
  if(mGetBlockThresholdsReqEvtsIdx.Index != 0xFF)
  {
    /* This is the first command*/
    mGetBlockThresholdsReqEvtsIdx.CmdIndex = 0x00;
    /* Copy the address and event id*/
    FLib_MemCpy(&mGetBlockThresholdsReqEvtsIdx.addrInfo, pAddrInfo, sizeof(addrInfoType_t));
    /* Start at index 0 */
    mGetBlockThresholdsReqEvtsIdx.Offset = 0;
      
    SendPriceClusterEvt(gzclEvtHandleGetBlockThresholds_c);
    return gZclSuccess_c;
  }
  else
  {
    return gZclNotFound_c;
  }
}
#endif

/******************************************************************************/
/* Process the Get Price Matrix Req; reply with the Publish Price Matrix for all
currently scheduled values*/
#if gASL_ZclSE_Use_NextGenFeatures_d 
static zbStatus_t ZCL_ProcessGetPriceMatrixReq
(
addrInfoType_t *pAddrInfo, 
zclCmdPrice_GetPriceMatrixReq_t * pGetPriceMatrix
)
{
  uint8_t entryIdx;
  
  mGetPriceMatrixReqEvtsIdx.Index = 0xFF;
  
  /* Find entry in Price Matrix table*/  
  for (entryIdx = 0; entryIdx < gNumofServerPriceMatrix_c; entryIdx++)
  {
    if (FLib_MemCmp(gaServerPriceMatrixTable[entryIdx].IssuerEvtID, pGetPriceMatrix->IssuerEvtID, sizeof(SEEvtId_t)))
    {
      mGetPriceMatrixReqEvtsIdx.Index = entryIdx;
      break;
    }
  }
  if(mGetPriceMatrixReqEvtsIdx.Index != 0xFF)
  {
    /* This is the first command*/
    mGetPriceMatrixReqEvtsIdx.CmdIndex = 0x00;
    /* Copy the address and event id*/
    FLib_MemCpy(&mGetPriceMatrixReqEvtsIdx.addrInfo, pAddrInfo, sizeof(addrInfoType_t));
    /* Start at index 0 */
    mGetPriceMatrixReqEvtsIdx.Offset = 0;
      
    SendPriceClusterEvt(gzclEvtHandleGetPriceMatrix_c);
    return gZclSuccess_c;
  }
  else
  {
    return gZclNotFound_c;
  }
}
#endif

/******************************************************************************/
/* Process the Get Tariff Information Req; reply with the Publish Tariff Information for all
currently scheduled values*/
#if gASL_ZclSE_Use_NextGenFeatures_d 
static zbStatus_t ZCL_ProcessGetTariffInformationReq
(
addrInfoType_t *pAddrInfo, 
zclCmdPrice_GetTariffInformationReq_t * pGetTariffInformation
)
{
  ZCLTime_t startTime;
  uint8_t entryIndex;
  
  /* Copy the address*/
  FLib_MemCpy(&mGetTariffInformationReqEvtsIdx.addrInfo, pAddrInfo, sizeof(addrInfoType_t));
  mGetTariffInformationReqEvtsIdx.Index = 0x00;
  mGetTariffInformationReqEvtsIdx.numOfEvtsRemaining = pGetTariffInformation->NumOfEvts;
  mGetTariffInformationReqEvtsIdx.tariffType = pGetTariffInformation->TariffType;
  
  /* Check valid value for tarif type*/
  if ((mGetTariffInformationReqEvtsIdx.tariffType > 0x02) &&
      (mGetTariffInformationReqEvtsIdx.tariffType != 0xFF))
  {
    return gZclFailure_c;
  }
  /* Get the start time */
  startTime = OTA2Native32(pGetTariffInformation->StartTime);
  
  /* If start time is 0x00000000 replace with current start time */
  if(startTime == 0x00000000)
  {
    startTime = ZCL_GetUTCTime();
  }
  mGetTariffInformationReqEvtsIdx.startTime = startTime;
  
  /* If number of events is 0 we will return all valid entries*/
  if(mGetTariffInformationReqEvtsIdx.numOfEvtsRemaining == 0)
  {
    mGetTariffInformationReqEvtsIdx.numOfEvtsRemaining = gNumofServerTariffInformation_c;
  }
      
  /* Find next valid entry */
  entryIndex = FindNextTariffInformationEvent();
  if(entryIndex != 0xFF)
  {
    mGetTariffInformationReqEvtsIdx.Index = entryIndex;
    SendPriceClusterEvt(gzclEvtHandleGetTariffInformation_c);
    return gZclSuccess_c;
  }
  else
  {
    return gZclNotFound_c;
  }
}
#endif

/******************************************************************************/
/* Process the Get CO2 Value Req; reply with the Publish CO2 Value for all
currently scheduled values*/
#if gASL_ZclSE_Use_NextGenFeatures_d 
static zbStatus_t ZCL_ProcessGetCO2ValueReq
(
addrInfoType_t *pAddrInfo, 
zclCmdPrice_GetCO2ValueReq_t * pGetCO2Value
)
{
  ZCLTime_t startTime;
  uint8_t entryIndex;
  
  /* Copy the address*/
  FLib_MemCpy(&mGetCO2ValueReqEvtsIdx.addrInfo, pAddrInfo, sizeof(addrInfoType_t));
  mGetCO2ValueReqEvtsIdx.Index = 0x00;
  mGetCO2ValueReqEvtsIdx.numOfEvtsRemaining = pGetCO2Value->NumOfEvts;
  mGetCO2ValueReqEvtsIdx.tariffType = pGetCO2Value->TariffType;
  
  /* Check valid value for tarif type*/
  if ((mGetCO2ValueReqEvtsIdx.tariffType > 0x02) &&
      (mGetCO2ValueReqEvtsIdx.tariffType != 0xFF))
  {
    return gZclFailure_c;
  }
  /* Get the start time */
  startTime = OTA2Native32(pGetCO2Value->StartTime);
  
  /* If start time is 0x00000000 replace with current start time */
  if(startTime == 0x00000000)
  {
    startTime = ZCL_GetUTCTime();
  }
  mGetCO2ValueReqEvtsIdx.startTime = startTime;
  
  /* If number of events is 0 we will return all valid entries*/
  if(mGetCO2ValueReqEvtsIdx.numOfEvtsRemaining == 0)
  {
    mGetCO2ValueReqEvtsIdx.numOfEvtsRemaining = gNumofServerCO2Value_c;
  }
      
  /* Find next valid entry */
  entryIndex = FindNextCO2ValueEvent();
  if(entryIndex != 0xFF)
  {
    mGetCO2ValueReqEvtsIdx.Index = entryIndex;
    SendPriceClusterEvt(gzclEvtHandleGetCO2Value_c);
    return gZclSuccess_c;
  }
  else
  {
    return gZclNotFound_c;
  }
}
#endif

/******************************************************************************/
/* Handle the GetBillingPeriod command*/
#if gASL_ZclSE_Use_NextGenFeatures_d 
void ZCL_HandleGetBillingPeriod(void)
{
  uint8_t status;
  uint8_t entryIndex;
     
  status= ZCL_SendPublishBillingPeriod(&mGetBillingPeriodReqEvtsIdx.addrInfo
       ,&gaServerBillingPeriodTable[mGetBillingPeriodReqEvtsIdx.Index]);
  
  if(status == gZclSuccess_c)
  {
    mGetBillingPeriodReqEvtsIdx.numOfEvtsRemaining = mGetBillingPeriodReqEvtsIdx.numOfEvtsRemaining - 1;
    mGetBillingPeriodReqEvtsIdx.Index = mGetBillingPeriodReqEvtsIdx.Index + 1;
    if (mGetBillingPeriodReqEvtsIdx.numOfEvtsRemaining > 0)
    {
      /* Find next valid entry */
      entryIndex = FindNextBillingPeriodEvent();
      if(entryIndex != 0xFF)
      {
        mGetBillingPeriodReqEvtsIdx.Index = entryIndex;
        SendPriceClusterEvt(gzclEvtHandleGetBillingPeriod_c);
      }
    }
  }
}
#endif

/******************************************************************************/
/* Handle the Get BlockThreshold command*/
#if gASL_ZclSE_Use_NextGenFeatures_d 
void ZCL_HandleGetBlockThresholds(void)
{
  zbStatus_t status;
     
  status = ZCL_SendPublishBlockThresholds(&mGetBlockThresholdsReqEvtsIdx.addrInfo
       ,&gaServerBlockThresholdsTable[mGetBlockThresholdsReqEvtsIdx.Index]);
  
  if(status == gZclSuccess_c)
  {
    mGetBlockThresholdsReqEvtsIdx.CmdIndex = mGetBlockThresholdsReqEvtsIdx.CmdIndex + 1;
    mGetBlockThresholdsReqEvtsIdx.Offset = mGetBlockThresholdsReqEvtsIdx.Offset + mGetBlockThresholdsReqEvtsIdx.Length;
    if (mGetBlockThresholdsReqEvtsIdx.Offset < gaServerBlockThresholdsTable[mGetBlockThresholdsReqEvtsIdx.Index].Length)
      SendPriceClusterEvt(gzclEvtHandleGetBlockThresholds_c);
  }
}
#endif

/******************************************************************************/
/* Handle the Get Price Matrix command*/
#if gASL_ZclSE_Use_NextGenFeatures_d 
void ZCL_HandleGetPriceMatrix(void)
{
  zbStatus_t status;
     
  status = ZCL_SendPublishPriceMatrix(&mGetPriceMatrixReqEvtsIdx.addrInfo
       ,&gaServerPriceMatrixTable[mGetPriceMatrixReqEvtsIdx.Index]);
  
  if(status == gZclSuccess_c)
  {
    mGetPriceMatrixReqEvtsIdx.CmdIndex = mGetPriceMatrixReqEvtsIdx.CmdIndex + 1;
    mGetPriceMatrixReqEvtsIdx.Offset = mGetPriceMatrixReqEvtsIdx.Offset + mGetPriceMatrixReqEvtsIdx.Length;
    if (mGetPriceMatrixReqEvtsIdx.Offset < gaServerPriceMatrixTable[mGetPriceMatrixReqEvtsIdx.Index].Length)
      SendPriceClusterEvt(gzclEvtHandleGetPriceMatrix_c);
  }
}
#endif

/******************************************************************************/
/* Handle the GetTariffInformation command*/
#if gASL_ZclSE_Use_NextGenFeatures_d 
void ZCL_HandleGetTariffInformation(void)
{
  uint8_t status;
  uint8_t entryIndex;
     
  status= ZCL_SendPublishTariffInformation(&mGetTariffInformationReqEvtsIdx.addrInfo
       ,&gaServerTariffInformationTable[mGetTariffInformationReqEvtsIdx.Index]);
  
  if(status == gZclSuccess_c)
  {
    mGetTariffInformationReqEvtsIdx.numOfEvtsRemaining = mGetTariffInformationReqEvtsIdx.numOfEvtsRemaining - 1;
    mGetTariffInformationReqEvtsIdx.Index = mGetTariffInformationReqEvtsIdx.Index + 1;
    if (mGetTariffInformationReqEvtsIdx.numOfEvtsRemaining > 0)
    {
      /* Find next valid entry */
      entryIndex = FindNextTariffInformationEvent();
      if(entryIndex != 0xFF)
      {
        mGetTariffInformationReqEvtsIdx.Index = entryIndex;
        SendPriceClusterEvt(gzclEvtHandleGetTariffInformation_c);
      }
    }
  }
}
#endif

/******************************************************************************/
/* Handle the GetCO2Value command*/
#if gASL_ZclSE_Use_NextGenFeatures_d 
void ZCL_HandleGetCO2Value(void)
{
  uint8_t status;
  uint8_t entryIndex;
     
  status= ZCL_SendPublishCO2Value(&mGetCO2ValueReqEvtsIdx.addrInfo
       ,&gaServerCO2ValueTable[mGetCO2ValueReqEvtsIdx.Index]);
  
  if(status == gZclSuccess_c)
  {
    mGetCO2ValueReqEvtsIdx.numOfEvtsRemaining = mGetCO2ValueReqEvtsIdx.numOfEvtsRemaining - 1;
    mGetCO2ValueReqEvtsIdx.Index = mGetCO2ValueReqEvtsIdx.Index + 1;
    if (mGetCO2ValueReqEvtsIdx.numOfEvtsRemaining > 0)
    {
      /* Find next valid entry */
      entryIndex = FindNextCO2ValueEvent();
      if(entryIndex != 0xFF)
      {
        mGetCO2ValueReqEvtsIdx.Index = entryIndex;
        SendPriceClusterEvt(gzclEvtHandleGetCO2Value_c);
      }
    }
  }
}
#endif

/******************************************************************************/
/* Handle the GetConsolidatedBill command*/
#if gASL_ZclSE_Use_NextGenFeatures_d 
void ZCL_HandleGetConsolidatedBill(void)
{
  uint8_t status;
  uint8_t entryIndex;
     
  status= ZCL_SendPublishConsolidatedBill(&mGetConsolidatedBillReqEvtsIdx.addrInfo
       ,&gaServerConsolidatedBillTable[mGetConsolidatedBillReqEvtsIdx.Index]);
  
  if(status == gZclSuccess_c)
  {
    mGetConsolidatedBillReqEvtsIdx.numOfEvtsRemaining = mGetConsolidatedBillReqEvtsIdx.numOfEvtsRemaining - 1;
    mGetConsolidatedBillReqEvtsIdx.Index = mGetConsolidatedBillReqEvtsIdx.Index + 1;
    if (mGetConsolidatedBillReqEvtsIdx.numOfEvtsRemaining > 0)
    {
      /* Find next valid entry */
      entryIndex = FindNextConsolidatedBillEvent();
      if(entryIndex != 0xFF)
      {
        mGetConsolidatedBillReqEvtsIdx.Index = entryIndex;
        SendPriceClusterEvt(gzclEvtHandleGetConsolidatedBill_c);
      }
    }
  }
}
#endif
/*************************************************************************/

/* Process the Get Block Periods Req; reply with the Publish Block Period for all
currently scheduled periods*/
#if gASL_ZclPrice_Optionals_d 
static zbStatus_t ZCL_ProcessGetBlockPeriodsReq
(
addrInfoType_t *pAddrInfo, 
zclCmdPrice_GetBlockPeriodsReq_t * pGetBlockPeriods
)
{
  uint32_t startTime,eventStartTime,eventStopTime;
  uint8_t nrOfEvents,i;
  uint32_t eventDuration;
  bool_t eventFound;
  
  // coppy the address
  FLib_MemCpy(&mGetBlockPeriodReqIndexs.addrInfo, pAddrInfo, sizeof(addrInfoType_t));
  // set index vector to 0x00 no index to send
  BeeUtilSetToF(&mGetBlockPeriodReqIndexs.IndexVector,gNumofServerBlockPeriods_c);
  mGetBlockPeriodReqIndexs.Index=0x00;
  
  // get the start time
  startTime = OTA2Native32(pGetBlockPeriods->StartTime);
  // get the number of events
  nrOfEvents = pGetBlockPeriods->NumOfEvts;
  // if start time is 0x00000000 start time will be curent start time
  if(startTime == 0x00000000)
  {
    startTime = ZCL_GetUTCTime();
  }
  /* if number of events is 0 will return all available publish block periods 
  starting with th ecurren block in progress*/
  if(nrOfEvents == 0)
  {// set the nrOfEvents equal with gNumofServerPrices_c
    nrOfEvents=gNumofServerBlockPeriods_c;
  }
  for(i = 0; i < gNumofServerBlockPeriods_c; i++)
  {
    // if exits this entry
    if(gaServerBlockPeriodsTable[i].EntryStatus != 0x00)  
    {
      //get the eventStartTime
      eventStartTime = OTA2Native32(gaServerBlockPeriodsTable[i].blockPeriod.BlockPeriodStartTime);
      if(eventStartTime == 0x00000000)
      {
        eventStartTime = OTA2Native32(gaServerBlockPeriodsTable[i].EffectiveStartTime);
      }
      //get the eventDuration
      eventDuration = GetNative32BitIntFrom3ByteArray(gaServerBlockPeriodsTable[i].blockPeriod.BlockPeriodDurationInMinutes);
      //get the eventStopTime
      eventStopTime = eventStartTime + (60 * eventDuration);
      if((startTime <= eventStopTime) && (nrOfEvents>0))
      {
        // make the flag true event has been found
        eventFound = TRUE;
        // add index in mGetBlockPeriodReqIndexs to send PublishBlockPeriod
        mGetBlockPeriodReqIndexs.IndexVector[mGetBlockPeriodReqIndexs.Index]=i;
        mGetBlockPeriodReqIndexs.Index=mGetBlockPeriodReqIndexs.Index+1;
        nrOfEvents=nrOfEvents-1;
      }
    }
  }
  if(eventFound == TRUE)
  {
    mGetBlockPeriodReqIndexs.Index=0;
    SendPriceClusterEvt(gzclEvtHandleGetBlockPeriods_c);
    return gZclSuccess_c;
  }
  else
  {
    return gZclNotFound_c;
  }
}
#endif
/******************************************************************************/
/* Handle the GetBlockPeriods command*/
#if gASL_ZclPrice_Optionals_d 
void ZCL_HandleGetBlockPeriods(void)
{
   uint8_t status;
   
   //verify if index is in gaServerPriceTable range
   if(mGetBlockPeriodReqIndexs.Index < gNumofServerBlockPeriods_c)
   {
     //verify if entry is real 
     if(mGetBlockPeriodReqIndexs.IndexVector[mGetBlockPeriodReqIndexs.Index]!=0xFF)
     {
        status= ZCL_SendPublishBlockPeriod(&mGetBlockPeriodReqIndexs.addrInfo
             ,&gaServerBlockPeriodsTable[mGetBlockPeriodReqIndexs.IndexVector[mGetBlockPeriodReqIndexs.Index]]
             );
        if(status == gZclSuccess_c)
        {
            mGetBlockPeriodReqIndexs.Index=mGetBlockPeriodReqIndexs.Index+1;
           SendPriceClusterEvt(gzclEvtHandleGetBlockPeriods_c);
        }
     }
   }
}
#endif
/******************************************************************************/
/* Handle the GetConversionFactor command*/
#if gASL_ZclPrice_Optionals_d &&  gASL_ZclSE_Use_NextGenFeatures_d
void ZCL_HandleGetConversionFactor(void)
{
   uint8_t status;
   
   //verify if index is in gaServerPriceTable range
   if(mGetConversionFactorReqIndexs.Index < gNumofServerConversionFactors_c)
   {
     //verify if entry is real 
     if(mGetConversionFactorReqIndexs.IndexVector[mGetConversionFactorReqIndexs.Index]!=0xFF)
     {
        status= ZCL_SendPublishConversionFactor(&mGetConversionFactorReqIndexs.addrInfo
             ,&gaServerConversionFactorTable[mGetConversionFactorReqIndexs.IndexVector[mGetConversionFactorReqIndexs.Index]]
             );
        if(status == gZclSuccess_c)
        {
            mGetConversionFactorReqIndexs.Index=mGetConversionFactorReqIndexs.Index+1;
           SendPriceClusterEvt(gzclEvtHandleGetConversionFactor_c);
        }
     }
   }
}
#endif
/******************************************************************************/
/* Handle the GetCalorificValue command*/
#if gASL_ZclPrice_Optionals_d && gASL_ZclSE_Use_NextGenFeatures_d
void ZCL_HandleGetCalorificValue(void)
{
   uint8_t status;
   
   //verify if index is in gaServerPriceTable range
   if(mGetCalorificValueReqIndexs.Index < gNumofServerCalorificValue_c)
   {
     //verify if entry is real 
     if(mGetCalorificValueReqIndexs.IndexVector[mGetCalorificValueReqIndexs.Index]!=0xFF)
     {
        status= ZCL_SendPublishCalorificValue(&mGetCalorificValueReqIndexs.addrInfo
             ,&gaServerCalorificValueTable[mGetCalorificValueReqIndexs.IndexVector[mGetCalorificValueReqIndexs.Index]]
             );
        if(status == gZclSuccess_c)
        {
            mGetCalorificValueReqIndexs.Index=mGetCalorificValueReqIndexs.Index+1;
           SendPriceClusterEvt(gzclEvtHandleGetCalorificValue_c);
        }
     }
   }
}
#endif
/******************************************************************************/
/* Store Block Periods information received from the Provider 
     The server side doesn't keep track of the Block Periods status, only stores the 
     received Block Periods and take care that Nested and overlapping Block Periods commands not to occur*/
#if gASL_ZclPrice_Optionals_d
zbStatus_t ZCL_ScheduleServerBlockPeriodsEvents ( ztcCmdPrice_PublishBlockPeriodRsp_t *pMsg)
{
  uint8_t i, newEntry;
  uint32_t currentTime, startTime,  stopTime;
  uint32_t msgStartTime, msgStopTime, duration; 
  /* here get the currentTime */
  currentTime = ZCL_GetUTCTime();
  msgStartTime = OTA2Native32(pMsg->publishBlockPeriodRsp.BlockPeriodStartTime);
  if((msgStartTime == 0x00000000) || (msgStartTime == 0xffffffff))
    msgStartTime = currentTime;
  duration = GetNative32BitIntFrom3ByteArray(pMsg->publishBlockPeriodRsp.BlockPeriodDurationInMinutes);
  msgStopTime = msgStartTime + (60 * duration);
  
  //if(msgStopTime <= currentTime)
  //	return status;
  
  /* Nested and overlapping Block Periods commands are not allowed */
  for(i = 0; i < gNumofServerBlockPeriods_c; i++)
  {
    
    if(gaServerBlockPeriodsTable[i].EntryStatus == 0x00)
      continue;
    /* Get the timing */
    startTime = OTA2Native32(gaServerBlockPeriodsTable[i].EffectiveStartTime);
    stopTime = startTime + (60 * GetNative32BitIntFrom3ByteArray(pMsg->publishBlockPeriodRsp.BlockPeriodDurationInMinutes));
    if ((msgStartTime >= stopTime)||(startTime >= msgStopTime))
      continue;
    /*the price overlapp... take no action*/
    return gZclFailure_c;
  }
  
  
  newEntry = AddBlockPeriodsInTable((publishBlockPeriodEntry_t *)&gaServerBlockPeriodsTable[0], gNumofServerBlockPeriods_c, &pMsg->publishBlockPeriodRsp);
  if(newEntry == 0xff)
    return gZclFailure_c;
  if(pMsg->SendPublishBlockPeriodImmediately)
  {
     mRegIndex = 0; 
     mInterPanIndex = 0;
     mUpdateBlockPeriodIndex = newEntry;
     SendPriceClusterEvt(gzclEvtHandlePublishBlockPeriodUpdate_c);
  }
  return gZclSuccess_c;
} 
#endif

#if gASL_ZclSE_Use_NextGenFeatures_d 
/******************************************************************************/
/* Store Billing Periods information received from the Provider 
     The server side doesn't keep track of the Billing Periods status, only stores the 
     received Billing Periods and take care that Nested and overlapping Billing Periods commands not to occur*/
zbStatus_t ZCL_ScheduleServerBillingPeriodEvents ( ztcCmdPrice_PublishBillingPeriodRsp_t *pMsg)
{
  uint8_t i, newEntry;
  uint32_t currentTime, startTime,  stopTime;
  uint32_t msgStartTime, msgStopTime; 
  uint32_t duration;
  
  /* here get the currentTime */
  currentTime = ZCL_GetUTCTime();
  msgStartTime = OTA2Native32(pMsg->publishBillingPeriodRsp.BillingPeriodStartTime);
  if((msgStartTime == 0x00000000) || (msgStartTime == 0xffffffff))
    msgStartTime = currentTime;
  duration = GetNative32BitIntFrom3ByteArray(pMsg->publishBillingPeriodRsp.BillingPeriodDurationInMinutes);
  msgStopTime = msgStartTime + (60 * duration);

  //if(msgStopTime <= currentTime)
  //	return status;
  
  /* Nested and overlapping Billing Periods commands are not allowed */
  for(i = 0; i < gNumofServerBillingPeriods_c; i++)
  {    
    if(gaServerBillingPeriodTable[i].EntryStatus == 0x00)
      continue;
    /* Get the timing */
    startTime = OTA2Native32(gaServerBillingPeriodTable[i].EffectiveStartTime);
    /* Get the duration value */
    duration = GetNative32BitIntFrom3ByteArray(pMsg->publishBillingPeriodRsp.BillingPeriodDurationInMinutes);
    stopTime = startTime + (60*(uint32_t)duration);

    if ((msgStartTime >= stopTime)||(startTime >= msgStopTime))
      continue;
    /*the period overlaps... take no action*/
    return gZclFailure_c;
  }
  
  
  newEntry = AddBillingPeriodsInTable((publishBillingPeriodEntry_t *)&gaServerBillingPeriodTable[0], gNumofServerBillingPeriods_c, &pMsg->publishBillingPeriodRsp);
  if(newEntry == 0xff)
    return gZclFailure_c;
  if(pMsg->SendPublishBillingPeriodImmediately)
  {
     mRegIndex = 0; 
     mInterPanIndex = 0;
     mUpdateBillingPeriodIndex = newEntry;
     SendPriceClusterEvt(gzclEvtHandlePublishBillingPeriodUpdate_c);
  }
  return gZclSuccess_c;
} 
#endif

#if gASL_ZclSE_Use_NextGenFeatures_d 
/******************************************************************************/
/* Store Block Thresholds received from the Provider */
zbStatus_t ZCL_StoreServerBlockThresholds( ztcCmdPrice_PublishBlockThresholdsRsp_t *pMsg)
{
  uint8_t i, entryIdx = 0xFF;

  /* Search for an existing entry*/
  for(i = 0; i < gNumofServerBlockThresholds_c; i++)
  {    
    if(FLib_MemCmp(gaServerBlockThresholdsTable[i].IssuerEvtID, pMsg->IssuerEvtID, sizeof(SEEvtId_t)))
    {
      entryIdx = i;
      break;
    }
  }
  
  /*If it is a new entry find an empty slot for it*/
  if (entryIdx == 0xFF)
    for(i = 0; i < gNumofServerBlockThresholds_c; i++)
    {    
      if(gaServerBlockThresholdsTable[i].EntryStatus == gEntryNotUsed_c)
      {
        entryIdx = i;
        break;
      }
    }
  
  /* Cannot store infromation*/  
  if(entryIdx == 0xff)
    return gZclFailure_c;
  
  /* Save the information in the Price Matrix Table*/
  FLib_MemCpy(&gaServerBlockThresholdsTable[entryIdx].IssuerEvtID, pMsg->IssuerEvtID, sizeof(SEEvtId_t));
  gaServerBlockThresholdsTable[entryIdx].Length = pMsg->Offset + pMsg->Length;
  gaServerBlockThresholdsTable[entryIdx].EntryStatus = gEntryUsed_c;
  FLib_MemCpy(&gaServerBlockThresholdsTable[entryIdx].BlockThresholds[pMsg->Offset], &pMsg->BlockThresholds[0], pMsg->Length * sizeof(BlockThreshold_t));
  return gZclSuccess_c;
} 
#endif

#if gASL_ZclSE_Use_NextGenFeatures_d 
/******************************************************************************/
/* Store Block Tier Price  information received from the Provider */
zbStatus_t ZCL_StoreServerPriceMatrix( ztcCmdPrice_PublishPriceMatrixRsp_t *pMsg)
{
  uint8_t i, entryIdx = 0xFF;

  /* Search for an existing entry*/
  for(i = 0; i < gNumofServerPriceMatrix_c; i++)
  {    
    if(FLib_MemCmp(gaServerPriceMatrixTable[i].IssuerEvtID, pMsg->IssuerEvtID, sizeof(SEEvtId_t)))
    {
      entryIdx = i;
      break;
    }
  }
  
  /*If it is a new entry fin an empty slot for it*/
  if (entryIdx == 0xFF)
    for(i = 0; i < gNumofServerPriceMatrix_c; i++)
    {    
      if(gaServerPriceMatrixTable[i].EntryStatus == gEntryNotUsed_c)
      {
        entryIdx = i;
        break;
      }
    }
  
  /* Cannot store infromation*/  
  if(entryIdx == 0xff)
    return gZclFailure_c;
  
  /* Save the information in the Price Matrix Table*/
  FLib_MemCpy(&gaServerPriceMatrixTable[entryIdx].IssuerEvtID, pMsg->IssuerEvtID, sizeof(SEEvtId_t));
  gaServerPriceMatrixTable[entryIdx].Length = pMsg->Offset + pMsg->Length;
  gaServerPriceMatrixTable[entryIdx].EntryStatus = gEntryUsed_c;
  FLib_MemCpy(&gaServerPriceMatrixTable[entryIdx].TierBlockPrice[pMsg->Offset], &pMsg->TierBlockPrice[0], pMsg->Length * sizeof(Price_t));
  return gZclSuccess_c;
} 
#endif

#if gASL_ZclSE_Use_NextGenFeatures_d 
/******************************************************************************/
/* Store Tariff Information received from the Provider 
     The server side doesn't keep track of the Tariff Information status, only stores the 
     received Tariff Information*/
zbStatus_t ZCL_ScheduleServerTariffInformationEvents ( ztcCmdPrice_PublishTariffInformationRsp_t *pMsg)
{
   uint8_t i, newEntry;
  uint32_t currentTime, startTime;
  uint32_t msgStartTime; 
  /* here get the currentTime */
  currentTime = ZCL_GetUTCTime();
  msgStartTime = OTA2Native32(pMsg->publishTariffInformationRsp.StartTime);
  if((msgStartTime == 0x00000000) || (msgStartTime == 0xffffffff))
    msgStartTime = currentTime;
  
  /* Nested and overlapping Tariff Information commands are not allowed */
  for(i = 0; i < gNumofServerTariffInformation_c; i++)
  {
    
    if(gaServerTariffInformationTable[i].EntryStatus == gEntryNotUsed_c)
      continue;
    /* Get the timing */
    startTime = OTA2Native32(gaServerTariffInformationTable[i].EffectiveStartTime);
    if (msgStartTime > startTime)
      continue;
    return gZclFailure_c;
  }
    
  newEntry = AddTariffInformationInTable((publishTariffInformationEntry_t *)&gaServerTariffInformationTable[0], gNumofServerTariffInformation_c, &pMsg->publishTariffInformationRsp);
  if(newEntry == 0xff)
    return gZclFailure_c;
  if(pMsg->SendPublishImmediately)
  {
     mRegIndex = 0; 
     mInterPanIndex = 0;
     mUpdateTariffInformationIndex = newEntry;
     SendPriceClusterEvt(gzclEvtHandlePublishTariffInformationUpdate_c);
  }
  return gZclSuccess_c;
} 
#endif

#if gASL_ZclSE_Use_NextGenFeatures_d 
/******************************************************************************/
/* Store CO2 Value information received from the Provider 
     The server side doesn't keep track of the CO2 Value status, only stores the 
     received CO2 Value*/
zbStatus_t ZCL_ScheduleServerCO2ValueEvents ( ztcCmdPrice_PublishCO2ValueRsp_t *pMsg)
{
   uint8_t i, newEntry;
  uint32_t currentTime, startTime;
  uint32_t msgStartTime; 
  /* here get the currentTime */
  currentTime = ZCL_GetUTCTime();
  msgStartTime = OTA2Native32(pMsg->publishCO2ValueRsp.StartTime);
  if((msgStartTime == 0x00000000) || (msgStartTime == 0xffffffff))
    msgStartTime = currentTime;
  
  /* Nested and overlapping CO2 Value commands are not allowed */
  for(i = 0; i < gNumofServerCO2Value_c; i++)
  {
    
    if(gaServerCO2ValueTable[i].EntryStatus == gEntryNotUsed_c)
      continue;
    /* Get the timing */
    startTime = OTA2Native32(gaServerCO2ValueTable[i].EffectiveStartTime);
    if (msgStartTime > startTime)
      continue;
    /*the price overlapp... take no action*/
    return gZclFailure_c;
  }
  
  
  newEntry = AddCO2ValueInTable((publishCO2ValueEntry_t *)&gaServerCO2ValueTable[0], gNumofServerCO2Value_c, &pMsg->publishCO2ValueRsp);
  if(newEntry == 0xff)
    return gZclFailure_c;
  if(pMsg->SendPublishCO2ValueImmediately)
  {
     mRegIndex = 0; 
     mInterPanIndex = 0;
     mUpdateCO2ValueIndex = newEntry;
     SendPriceClusterEvt(gzclEvtHandlePublishCO2ValueUpdate_c);
  }
  return gZclSuccess_c;
 
} 
#endif

#if gASL_ZclSE_Use_NextGenFeatures_d 
/******************************************************************************/
/* Store Consolidated Bill information received from the Provider 
     The server side doesn't keep track of the Consolidated Bill status, only stores the 
     received Consolidated Bill and take care that Nested and overlapping Consolidated Bill commands not to occur*/
zbStatus_t ZCL_ScheduleServerConsolidatedBillEvents ( ztcCmdPrice_PublishConsolidatedBillRsp_t *pMsg)
{
  uint8_t i, newEntry;
  uint32_t currentTime, startTime,  stopTime;
  uint32_t msgStartTime, msgStopTime; 
  uint32_t duration;

  /* here get the currentTime */
  currentTime = ZCL_GetUTCTime();
  msgStartTime = OTA2Native32(pMsg->publishConsolidatedBillRsp.BillingPeriodStartTime);
  if((msgStartTime == 0x00000000) || (msgStartTime == 0xffffffff))
    msgStartTime = currentTime;
  
  duration = GetNative32BitIntFrom3ByteArray(pMsg->publishConsolidatedBillRsp.BillingPeriodDurationInMinutes);
  msgStopTime = msgStartTime + (60 * duration);
  
  //if(msgStopTime <= currentTime)
  //	return status;
  
  /* Nested and overlapping Billing Periods commands are not allowed */
  for(i = 0; i < gNumofServerConsolidatedBill_c; i++)
  {    
    if(gaServerConsolidatedBillTable[i].EntryStatus == 0x00)
      continue;
    /* Get the timing */
    startTime = OTA2Native32(gaServerConsolidatedBillTable[i].EffectiveStartTime);
    // get the duration value
    duration = GetNative32BitIntFrom3ByteArray(pMsg->publishConsolidatedBillRsp.BillingPeriodDurationInMinutes);
    stopTime = startTime + (60 * duration);

    if ((msgStartTime >= stopTime)||(startTime >= msgStopTime))
      continue;
    /*the period overlaps... take no action*/
    return gZclFailure_c;
  }
  
  newEntry = AddConsolidatedBillsInTable((publishConsolidatedBillEntry_t *)&gaServerConsolidatedBillTable[0], gNumofServerConsolidatedBill_c, &pMsg->publishConsolidatedBillRsp);
  if(newEntry == 0xff)
    return gZclFailure_c;
  if(pMsg->SendPublishConsolidatedBillImmediately)
  {
     mRegIndex = 0; 
     mInterPanIndex = 0;
     mUpdateConsolidatedBillIndex = newEntry;
     SendPriceClusterEvt(gzclEvtHandlePublishConsolidatedBillUpdate_c);
  }
  return gZclSuccess_c;
} 
#endif

#if gASL_ZclSE_Use_NextGenFeatures_d 
zbStatus_t ZCL_ScheduleServerCPPEvents(zclCmdPrice_PublishCPPEventRsp_t *pMsg)
{
  uint8_t i, newEntry;
  uint32_t currentTime, startTime,  stopTime;
  uint32_t msgStartTime, msgStopTime; 
  uint16_t duration;
  
  /* here get the currentTime */
  currentTime = ZCL_GetUTCTime();
  msgStartTime = OTA2Native32(pMsg->CPPEventStartTime);
  if((msgStartTime == 0x00000000) || (msgStartTime == 0xffffffff))
    msgStartTime = currentTime;
  duration = GetNative16BitInt(pMsg->CPPEventDurationInMinutes);
  msgStopTime = msgStartTime + (60 * (uint32_t)duration);
  
  /* Nested and overlapping CPP Events commands are not allowed */
  for(i = 0; i < gNumofServerCPPEvents_c; i++)
  {    
    if(gaServerCPPEventTable[i].EntryStatus == 0x00)
      continue;
    /* Get the timing */
    startTime = OTA2Native32(gaServerCPPEventTable[i].EffectiveStartTime);
    // get the duration value
    duration = GetNative16BitInt(pMsg->CPPEventDurationInMinutes);
    stopTime = startTime + (60 * (uint32_t)duration);
    if ((msgStartTime >= stopTime)||(startTime >= msgStopTime))
      continue;
    /*the period overlaps... take no action*/
    return gZclFailure_c;
  }
  newEntry = AddCPPEventsInTable((publishCPPEventEntry_t *)&gaServerCPPEventTable[0], gNumofServerCPPEvents_c, pMsg);
  if(newEntry == 0xff)
    return gZclFailure_c;
  
  /* Prepare to send publish command*/
  mRegIndex = 0; 
  mUpdateCPPEventIndex = newEntry;
  SendPriceClusterEvt(gzclEvtHandlePublishCPPEvent_c);
  
  return gZclSuccess_c;
} 
#endif



/******************************************************************************/
#if gASL_ZclPrice_Optionals_d
void ZCL_DeleteScheduleServerBlockPeriods(void)
{
  uint8_t i;
  for(i = 0; i < gNumofServerBlockPeriods_c; i++)
    gaServerBlockPeriodsTable[i].EntryStatus = 0x00;
}

/******************************************************************************/
/* Add Block Periods in Table... so that to have the scheduled price in asccendent order */
static uint8_t AddBlockPeriodsInTable(publishBlockPeriodEntry_t *pTable, uint8_t len, zclCmdPrice_PublishBlockPeriodRsp_t *pMsg)
{
  uint32_t startTime, msgStartTime, currentTime;
  uint8_t i, poz = 0xff;
  /* get message time */
  msgStartTime = OTA2Native32(pMsg->BlockPeriodStartTime);
  currentTime = ZCL_GetUTCTime();
  if(msgStartTime == 0x000000)
  {
    msgStartTime = currentTime;
  }
  
  /* keep the Block Periods in asccendent order; find the index in table where the message will be added */
  for(i = 0; i < len; i++)
  {
    if((pTable+i)->EntryStatus != 0x00)
    {
      startTime = OTA2Native32((pTable+i)->blockPeriod.BlockPeriodStartTime);
      if(startTime == 0x000000)
      {
        startTime = OTA2Native32((pTable+i)->EffectiveStartTime);
      }
      if(((startTime > msgStartTime) && (startTime != 0xffffffff)) ||
         (msgStartTime == 0xffffffff)) 
      {
        poz = i;
        break;
      }
    }
    else
    {
      poz = i;
      break;
    }
  }
  /* check if the table is full; return oxff */
  if(poz == 0xff)
    return poz;
  /*move the content to the left*/
  if((pTable+poz)->EntryStatus !=0x00)
    FLib_MemInPlaceCpy((pTable+poz+1),(pTable+poz),(len-poz-1)* sizeof(publishBlockPeriodEntry_t));
  
  FLib_MemCpy((pTable+poz), pMsg, sizeof(zclCmdPrice_PublishBlockPeriodRsp_t));
  
  if((pMsg->BlockPeriodStartTime == 0x00000000)||
     (pMsg->BlockPeriodStartTime == 0xffffffff))
  
  {
    /*  here get the currentTime  */
    currentTime = ZCL_GetUTCTime();
    currentTime = Native2OTA32(currentTime);
    (pTable+poz)->EffectiveStartTime = currentTime;
  }
  else
    (pTable+poz)->EffectiveStartTime = pMsg->BlockPeriodStartTime ;
  /* new price was received */	
  (pTable+poz)->EntryStatus = gPriceReceivedStatus_c;
  /* Call the App to signal that a Price was received; User should check EntryStatus */
  BeeAppUpdateDevice (0, gZclUI_BlockPeriodEvt_c, 0, 0, (pTable+poz));
  return poz;
}

/******************************************************************************/
/*Update a Block Period from the Provider.
    In the case of an update to the
Block Period information from the commodity provider, the Publish Block Period command
should be unicast to all individually registered devices implementing the Price
Cluster on the ZigBee Smart Energy network. */
zbStatus_t ZCL_UpdateServerBlockPeriodEvents(zclCmdPrice_PublishBlockPeriodRsp_t *pMsg)
{
  uint8_t updateIndex;
  
  updateIndex = CheckForBlockPeriodUpdate(pMsg, (publishBlockPeriodEntry_t *)&gaServerBlockPeriodsTable[0], gNumofServerBlockPeriods_c);
  /* the Block Period is updated??? */
  if (updateIndex < 0xFE)
  {
    /* Send the Publish Prices to all SE registered devices */
    mRegIndex = 0; 
    mInterPanIndex = 0;
    mUpdateBlockPeriodIndex = updateIndex;
    SendPriceClusterEvt(gzclEvtHandlePublishBlockPeriodUpdate_c);
    return gZclSuccess_c;
  }
  else
    return gZclFailure_c;
}

/******************************************************************************/
/*Check and Updated a Block Period.
When new block period information is provided that replaces older block period
information for the same time period, IssuerEvt field allows devices to determine which
information is newer. It is expected that the value contained in this field is a
unique number managed by upstream servers.
Thus, newer pricing information will have a value in the Issuer Event ID field that
is larger than older pricing information.
*/
static uint8_t CheckForBlockPeriodUpdate(zclCmdPrice_PublishBlockPeriodRsp_t *pMsg, publishBlockPeriodEntry_t *pTable, uint8_t len)
{
  uint8_t i;
  uint32_t msgIssuerEvt, entryIssuerEvt;
  
  msgIssuerEvt = FourBytesToUint32(pMsg->IssuerEvtID);
  msgIssuerEvt = OTA2Native32(msgIssuerEvt);
  
  for(i = 0; i < len; i++)
  {
    if((pTable+i)->EntryStatus != 0x00)
    {
      entryIssuerEvt = FourBytesToUint32(&(pTable+i)->blockPeriod.IssuerEvtID);
      entryIssuerEvt = OTA2Native32(entryIssuerEvt);
      if ((pTable+i)->blockPeriod.BlockPeriodStartTime == pMsg->BlockPeriodStartTime &&
          FLib_MemCmp(&(pTable+i)->blockPeriod.BlockPeriodDurationInMinutes, &pMsg->BlockPeriodDurationInMinutes, sizeof(BlockPeriodDuration_t)))
        if(entryIssuerEvt < msgIssuerEvt)
        {
          FLib_MemCpy((pTable+i), pMsg, sizeof(zclCmdPrice_PublishBlockPeriodRsp_t));
          return i;
        }
        else 
          return 0xFE; /* reject it */
    }
  }
  return 0xff;
}
#endif
/******************************************************************************/
/* Handle the Price Cluster Event */
void ZCL_HandleSEPriceClusterEvt(void)
{
  uint32_t events = mPriceClusterEvt;  

#if gASL_ZclPrice_PublishPriceRsp_d
  
#if gASL_ZclPrice_Optionals_d 
  if (events & gzclEvtHandleGetScheduledPrices_c)
    ZCL_HandleGetScheduledPrices();
#endif
  
  if(events & gzclEvtHandlePublishPriceUpdate_c)
  	ZCL_HandlePublishPriceUpdate();
#else
  (void)events;
#endif

#if gASL_ZclPrice_PublishConversionFactorRsp_d
#if gASL_ZclPrice_Optionals_d 
  if(events & gzclEvtHandleGetConversionFactor_c)
        ZCL_HandleGetConversionFactor();
#endif
#endif

#if gASL_ZclPrice_PublishCalorificValueRsp_d
#if gASL_ZclPrice_Optionals_d 
  if(events & gzclEvtHandleGetCalorificValue_c)
        ZCL_HandleGetCalorificValue();
#endif
#endif
  
#if gASL_ZclPrice_PublishBlockPeriodsRsp_d
#if gASL_ZclPrice_Optionals_d 
  if(events & gzclEvtHandleGetBlockPeriods_c)
        ZCL_HandleGetBlockPeriods();
  if(events & gzclEvtHandlePublishBlockPeriodUpdate_c)
        ZCL_HandleBlockPeriodUpdate();
#endif
#endif
  
#if gASL_ZclPrice_GetCurrPriceReq_d || gASL_ZclPrice_GetSheduledPricesReq_d   
  if (events & gzclEvtHandleClientPrices_c)
    ZCL_HandleClientPrices();
#endif
  
#if gASL_ZclPrice_Optionals_d  
#if gASL_ZclPrice_GetBlockPeriodsReq_d
   if (events & gzclEvtHandleClientBlockPeriod_c)
    ZCL_HandleClientBlockPeriod();
#endif
#endif

#if gASL_ZclPrice_Optionals_d  
#if gASL_ZclPrice_PublishConversionFactorRsp_d
   if (events & gzclEvtHandleConversionFactorPeriod_c)
    ZCL_HandleConversionFactorUpdate();
#endif
#endif
   
#if gASL_ZclPrice_Optionals_d  
#if gASL_ZclPrice_PublishCalorificValueRsp_d
   if (events & gzclEvtHandleCalorificValuePeriod_c)
    ZCL_HandleCalorificValueUpdate();
#endif
#endif
   
#if gASL_ZclSE_Use_NextGenFeatures_d
#if gASL_ZclPrice_PublishBillingPeriodRsp_d
  if(events & gzclEvtHandleGetBillingPeriod_c)
    ZCL_HandleGetBillingPeriod();
  if(events & gzclEvtHandlePublishBillingPeriodUpdate_c)
    ZCL_HandleBillingPeriodUpdate();
#endif  
#endif   
  
#if gASL_ZclSE_Use_NextGenFeatures_d 
#if gASL_ZclPrice_PublishCPPEventRsp_d  
  if(events & gzclEvtHandlePublishCPPEvent_c)
    ZCL_HandleCPPEventUpdate();
#endif  
#endif  
   
#if gASL_ZclSE_Use_NextGenFeatures_d  
#if gASL_ZclPrice_GetBillingPeriodReq_d
  if (events & gzclEvtHandleClientBillingPeriod_c)
    ZCL_HandleClientBillingPeriod();
#endif
#endif   

#if gASL_ZclSE_Use_NextGenFeatures_d
#if gASL_ZclPrice_PublishPriceMatrixRsp_d  
  if(events & gzclEvtHandleGetPriceMatrix_c)
    ZCL_HandleGetPriceMatrix();
#endif  
#endif  

#if gASL_ZclSE_Use_NextGenFeatures_d
#if gASL_ZclPrice_PublishBlockThresholdsRsp_d  
  if(events & gzclEvtHandleGetBlockThresholds_c)
    ZCL_HandleGetBlockThresholds();
#endif  
#endif  

#if gASL_ZclSE_Use_NextGenFeatures_d
#if gASL_ZclPrice_PublishCO2ValueRsp_d  
  if(events & gzclEvtHandleGetCO2Value_c)
    ZCL_HandleGetCO2Value();
  if(events & gzclEvtHandlePublishCO2ValueUpdate_c)
    ZCL_HandleCO2ValueUpdate();
#endif  
#endif  

#if gASL_ZclSE_Use_NextGenFeatures_d
#if gASL_ZclPrice_PublishTariffInformationRsp_d  
  if(events & gzclEvtHandleGetTariffInformation_c)
    ZCL_HandleGetTariffInformation();
  if(events & gzclEvtHandlePublishTariffInformationUpdate_c)
    ZCL_HandleTariffInformationUpdate();
#endif  
#endif  

#if gASL_ZclSE_Use_NextGenFeatures_d 
#if gASL_ZclPrice_PublishConsolidatedBillRsp_d  
  if(events & gzclEvtHandleGetConsolidatedBill_c)
    ZCL_HandleGetConsolidatedBill();
  if(events & gzclEvtHandlePublishConsolidatedBillUpdate_c)
    ZCL_HandleConsolidatedBillUpdate();
#endif 
#endif  
   
#if gASL_ZclSE_Use_NextGenFeatures_d  
#if gASL_ZclPrice_GetConsolidatedBillReq_d
  if (events & gzclEvtHandleClientConsolidatedBill_c)
    ZCL_HandleClientConsolidatedBill();
#endif
#endif   
    
}

/******************************************************************************/
/* Send the Price Cluster Event to the TS */
void SendPriceClusterEvt(uint32_t evtId)
{
  mPriceClusterEvt = evtId;
  TS_SendEvent(gZclTaskId, gzclEvtHandlePriceClusterEvt_c);
}

/******************************************************************************/
/* Handle the Block Period Update */
#if gASL_ZclPrice_Optionals_d
void ZCL_HandleBlockPeriodUpdate(void)
{
  uint8_t status;
  
  /* Check the Price addr table for SE device and send the publish price */
  if(mRegIndex < gNumOfPriceAddr_c)
  {
    if(gaPriceAddrTable[mRegIndex].entryStatus)
    {
      status = ZCL_SendPublishBlockPeriod((addrInfoType_t *)&gaPriceAddrTable[mRegIndex].addrInfo,
                                    &gaServerBlockPeriodsTable[mUpdateBlockPeriodIndex]);
      if(status == gZclSuccess_c)
	       ++mRegIndex;
      SendPriceClusterEvt(gzclEvtHandlePublishBlockPeriodUpdate_c);
      return;
      
    }
  }
}
#endif
#if gASL_ZclSE_Use_NextGenFeatures_d
/******************************************************************************/
/* Handle the Billing Period Update */

void ZCL_HandleBillingPeriodUpdate(void)
{
  uint8_t status;
  
  /* Check the Price addr table for SE device and send the Publish Billing Period */
  if(mRegIndex < gNumOfPriceAddr_c)
  {
    if(gaPriceAddrTable[mRegIndex].entryStatus)
    {
      status = ZCL_SendPublishBillingPeriod((addrInfoType_t *)&gaPriceAddrTable[mRegIndex].addrInfo,
                                    &gaServerBillingPeriodTable[mUpdateBillingPeriodIndex]);
      if(status == gZclSuccess_c)
	       ++mRegIndex;
      SendPriceClusterEvt(gzclEvtHandlePublishBillingPeriodUpdate_c);
      return;
      
    }
  }
}
#endif 

#if gASL_ZclSE_Use_NextGenFeatures_d
void ZCL_HandleCPPEventUpdate(void)
{
  uint8_t status;
  
  /* Check the Price addr table for SE device and send the Publish CPP Event */
  if(mRegIndex < gNumOfPriceAddr_c)
  {
    if(gaPriceAddrTable[mRegIndex].entryStatus)
    {
      status = ZCL_SendPublishCPPEvent((addrInfoType_t *)&gaPriceAddrTable[mRegIndex].addrInfo,
                                    &gaServerCPPEventTable[mUpdateCPPEventIndex]);
      if(status == gZclSuccess_c)
	       ++mRegIndex;
      SendPriceClusterEvt(gzclEvtHandlePublishCPPEvent_c);
      return;
      
    }
  }
}
#endif 

#if gASL_ZclSE_Use_NextGenFeatures_d
/******************************************************************************/
/* Handle the CO2 Value Update */

void ZCL_HandleCO2ValueUpdate(void)
{
  uint8_t status;
  
  /* Check the Price addr table for SE device and send the Publish CO2 Value */
  if(mRegIndex < gNumOfPriceAddr_c)
  {
    if(gaPriceAddrTable[mRegIndex].entryStatus)
    {
      status = ZCL_SendPublishCO2Value((addrInfoType_t *)&gaPriceAddrTable[mRegIndex].addrInfo,
                                    &gaServerCO2ValueTable[mUpdateCO2ValueIndex]);
      if(status == gZclSuccess_c)
	       ++mRegIndex;
      SendPriceClusterEvt(gzclEvtHandlePublishCO2ValueUpdate_c);
      return;
      
    }
  }
}
#endif 

#if gASL_ZclSE_Use_NextGenFeatures_d
/******************************************************************************/
/* Handle the Tariff Information Update */

void ZCL_HandleTariffInformationUpdate(void)
{
  uint8_t status;
  
  /* Check the Price addr table for SE device and send the Publish Tariff Information */
  if(mRegIndex < gNumOfPriceAddr_c)
  {
    if(gaPriceAddrTable[mRegIndex].entryStatus)
    {
      status = ZCL_SendPublishTariffInformation((addrInfoType_t *)&gaPriceAddrTable[mRegIndex].addrInfo,
                                    &gaServerTariffInformationTable[mUpdateTariffInformationIndex]);
      if(status == gZclSuccess_c)
	       ++mRegIndex;
      SendPriceClusterEvt(gzclEvtHandlePublishTariffInformationUpdate_c);
      return;
      
    }
  }
}
#endif 

#if gASL_ZclSE_Use_NextGenFeatures_d
/******************************************************************************/
/* Handle the Billing Period Update */

void ZCL_HandleConsolidatedBillUpdate(void)
{
  uint8_t status;
  
  /* Check the Price addr table for SE device and send the Publish Billing Period */
  if(mRegIndex < gNumOfPriceAddr_c)
  {
    if(gaPriceAddrTable[mRegIndex].entryStatus)
    {
      status = ZCL_SendPublishConsolidatedBill((addrInfoType_t *)&gaPriceAddrTable[mRegIndex].addrInfo,
                                    &gaServerConsolidatedBillTable[mUpdateConsolidatedBillIndex]);
      if(status == gZclSuccess_c)
	       ++mRegIndex;
      SendPriceClusterEvt(gzclEvtHandlePublishConsolidatedBillUpdate_c);
      return;
      
    }
  }
}
#endif 

/******************************************************************************/
/* Handle the Conversion Factor Update */
#if gASL_ZclPrice_Optionals_d && gASL_ZclSE_Use_NextGenFeatures_d 
void ZCL_HandleConversionFactorUpdate(void)
{
  uint8_t status;
  
  /* Check the Price addr table for SE device and send the publish price */
  if(mRegIndex < gNumOfPriceAddr_c)
  {
    if(gaPriceAddrTable[mRegIndex].entryStatus)
    {
      status = ZCL_SendPublishConversionFactor((addrInfoType_t *)&gaPriceAddrTable[mRegIndex].addrInfo,
                                    &gaServerConversionFactorTable[mUpdateConversionFactorIndex]);
      if(status == gZclSuccess_c)
	       ++mRegIndex;
      SendPriceClusterEvt(gzclEvtHandleConversionFactorPeriod_c);
      return;
      
    }
  }
}
#endif
/******************************************************************************/
/* Handle the Calorific Value Update */
#if gASL_ZclPrice_Optionals_d && gASL_ZclSE_Use_NextGenFeatures_d 
void ZCL_HandleCalorificValueUpdate(void)
{
  uint8_t status;
  
  /* Check the Price addr table for SE device and send the publish price */
  if(mRegIndex < gNumOfPriceAddr_c)
  {
    if(gaPriceAddrTable[mRegIndex].entryStatus)
    {
      status = ZCL_SendPublishCalorificValue((addrInfoType_t *)&gaPriceAddrTable[mRegIndex].addrInfo,
                                    &gaServerCalorificValueTable[mUpdateCalorificValueIndex]);
      if(status == gZclSuccess_c)
	       ++mRegIndex;
      SendPriceClusterEvt(gzclEvtHandleCalorificValuePeriod_c);
      return;
      
    }
  }
}
#endif
/******************************************************************************/
/* Store Calorific Value information received from the Provider 
     The server side doesn't keep track of the Calorific Value status, only stores the 
     received Calorific Value and take care that Nested and overlapping Calorific Value commands not to occur*/
#if gASL_ZclPrice_Optionals_d && gASL_ZclSE_Use_NextGenFeatures_d 
zbStatus_t ZCL_ScheduleServerCalorificValueStore ( ztcCmdPrice_PublishCalorificValueRsp_t *pMsg)
{
  uint8_t i, newEntry;
  uint32_t currentTime, startTime;
  uint32_t msgStartTime; 
  /* here get the currentTime */
  currentTime = ZCL_GetUTCTime();
  msgStartTime = OTA2Native32(pMsg->publishCalorificValueRsp.StartTime);
  if((msgStartTime == 0x00000000) || (msgStartTime == 0xffffffff))
    msgStartTime = currentTime;
  
  /* Nested and overlapping Calorific Value commands are not allowed */
  for(i = 0; i < gNumofServerCalorificValue_c; i++)
  {
    
    if(gaServerCalorificValueTable[i].EntryStatus == 0x00)
      continue;
    /* Get the timing */
    startTime = OTA2Native32(gaServerCalorificValueTable[i].EffectiveStartTime);
    if (msgStartTime > startTime)
      continue;
    /*the price overlapp... take no action*/
    return gZclFailure_c;
  }
  
  
  newEntry = AddCalorificValueInTable((publishCalorificValueEntry_t *)&gaServerCalorificValueTable[0], gNumofServerCalorificValue_c, &pMsg->publishCalorificValueRsp);
  if(newEntry == 0xff)
    return gZclFailure_c;
  if(pMsg->SendPublishBlockPeriodImmediately)
  {
     mRegIndex = 0; 
     mInterPanIndex = 0;
     mUpdateCalorificValueIndex = newEntry;
     SendPriceClusterEvt(gzclEvtHandleCalorificValuePeriod_c);
  }
  return gZclSuccess_c;
} 

/******************************************************************************/
void ZCL_DeleteServerCalorificValueStore(void)
{
  uint8_t i;
  for(i = 0; i < gNumofServerCalorificValue_c; i++)
    gaServerCalorificValueTable[i].EntryStatus = 0x00;
}

/******************************************************************************/
/* Add Clorific Value in Table... so that to have the clorific values in asccendent order */
static uint8_t AddCalorificValueInTable(publishCalorificValueEntry_t *pTable, uint8_t len, zclCmdPrice_PublishCalorificValueRsp_t *pMsg)
{
  uint32_t startTime, msgStartTime, currentTime;
  uint8_t i, poz = 0xff;
  /* get message time */
  msgStartTime = OTA2Native32(pMsg->StartTime);
  
  /* keep the calorific values in asccendent order; find the index in table where the message will be added */
  for(i = 0; i < len; i++)
  {
    if((pTable+i)->EntryStatus != 0x00)
    {
      startTime = OTA2Native32((pTable+i)->CalorificValue.StartTime);
      if(((startTime > msgStartTime) && (startTime != 0xffffffff)) ||
         (msgStartTime == 0xffffffff)) 
      {
        poz = i;
        break;
      }
    }
    else
    {
      poz = i;
      break;
    }
  }
  /* check if the table is full; return oxff */
  if(poz == 0xff)
    return poz;
  /*move the content to the left*/
  if((pTable+poz)->EntryStatus !=0x00)
    FLib_MemInPlaceCpy((pTable+poz+1),(pTable+poz),(len-poz-1)* sizeof(publishCalorificValueEntry_t));
  
  FLib_MemCpy((pTable+poz), pMsg, sizeof(zclCmdPrice_PublishCalorificValueRsp_t));
  
  if((pMsg->StartTime == 0x00000000)||
     (pMsg->StartTime == 0xffffffff))
  
  {
    /*  here get the currentTime  */
    currentTime = ZCL_GetUTCTime();
    currentTime = Native2OTA32(currentTime);
    (pTable+poz)->EffectiveStartTime = currentTime;
  }
  else
    (pTable+poz)->EffectiveStartTime = pMsg->StartTime ;
  /* new price was received */	
  (pTable+poz)->EntryStatus = gPriceReceivedStatus_c;
  /* Call the App to signal that a Price was received; User should check EntryStatus */
  //de continuat   BeeAppUpdateDevice (0, gZclUI_PriceEvt_c, 0, 0, (pTable+poz));
  return poz;
}

/******************************************************************************/
/* Store Conversion Factor information received from the Provider 
     The server side doesn't keep track of the Conversion Factors status, only stores the 
     received Conversion Factors and take care that Nested and overlapping Conversion Factors commands not to occur*/
zbStatus_t ZCL_ScheduleServerConversionFactorStore ( ztcCmdPrice_PublishConversionFactorRsp_t *pMsg)
{
  uint8_t i, newEntry;
  uint32_t currentTime, startTime;
  uint32_t msgStartTime; 
  /* here get the currentTime */
  currentTime = ZCL_GetUTCTime();
  msgStartTime = OTA2Native32(pMsg->publishConversionFactorRsp.StartTime);
  if((msgStartTime == 0x00000000) || (msgStartTime == 0xffffffff))
    msgStartTime = currentTime;
  
  /* Nested and overlapping Block Periods commands are not allowed */
  for(i = 0; i < gNumofServerConversionFactors_c; i++)
  {
    
    if(gaServerConversionFactorTable[i].EntryStatus == 0x00)
      continue;
    /* Get the timing */
    startTime = OTA2Native32(gaServerConversionFactorTable[i].EffectiveStartTime);
    if (msgStartTime > startTime)
      continue;
    /*the price overlapp... take no action*/
    return gZclFailure_c;
  }
  
  
  newEntry = AddConversionFactorInTable((publishConversionFactorEntry_t *)&gaServerConversionFactorTable[0], gNumofServerConversionFactors_c, &pMsg->publishConversionFactorRsp);
  if(newEntry == 0xff)
    return gZclFailure_c;
  if(pMsg->SendPublishBlockPeriodImmediately)
  {
     mRegIndex = 0; 
     mInterPanIndex = 0;
     mUpdateConversionFactorIndex = newEntry;
     SendPriceClusterEvt(gzclEvtHandleConversionFactorPeriod_c);
  }
  return gZclSuccess_c;
} 

/******************************************************************************/
void ZCL_DeleteServerConversionFactor(void)
{
  uint8_t i;
  for(i = 0; i < gNumofServerConversionFactors_c; i++)
    gaServerConversionFactorTable[i].EntryStatus = 0x00;
}

/******************************************************************************/
/* Add Conversion Factor in Table... so that to have the Conversion Factor in asccendent order */
static uint8_t AddConversionFactorInTable(publishConversionFactorEntry_t *pTable, uint8_t len, zclCmdPrice_PublishConversionFactorRsp_t *pMsg)
{
  uint32_t startTime, msgStartTime, currentTime;
  uint8_t i, poz = 0xff;
  /* get message time */
  msgStartTime = OTA2Native32(pMsg->StartTime);
  
  /* keep the Conversion Factor in asccendent order; find the index in table where the message will be added */
  for(i = 0; i < len; i++)
  {
    if((pTable+i)->EntryStatus != 0x00)
    {
      startTime = OTA2Native32((pTable+i)->conversionFactor.StartTime);
      if(((startTime > msgStartTime) && (startTime != 0xffffffff)) ||
         (msgStartTime == 0xffffffff)) 
      {
        poz = i;
        break;
      }
    }
    else
    {
      poz = i;
      break;
    }
  }
  /* check if the table is full; return oxff */
  if(poz == 0xff)
    return poz;
  /*move the content to the left*/
  if((pTable+poz)->EntryStatus !=0x00)
    FLib_MemInPlaceCpy((pTable+poz+1),(pTable+poz),(len-poz-1)* sizeof(publishConversionFactorEntry_t));
  
  FLib_MemCpy((pTable+poz), pMsg, sizeof(zclCmdPrice_PublishConversionFactorRsp_t));
  
  if((pMsg->StartTime == 0x00000000)||
     (pMsg->StartTime == 0xffffffff))
  
  {
    /*  here get the currentTime  */
    currentTime = ZCL_GetUTCTime();
    currentTime = Native2OTA32(currentTime);
    (pTable+poz)->EffectiveStartTime = currentTime;
  }
  else
    (pTable+poz)->EffectiveStartTime = pMsg->StartTime ;
  /* new price was received */	
  (pTable+poz)->EntryStatus = gPriceReceivedStatus_c;
  /* Call the App to signal that a Price was received; User should check EntryStatus */
  //de continuat   BeeAppUpdateDevice (0, gZclUI_PriceEvt_c, 0, 0, (pTable+poz));
  return poz;
}
#endif

#if gASL_ZclSE_Use_NextGenFeatures_d

/******************************************************************************/
/* Add Billing Periods in Table... so that to have the scheduled price in asccendent order */
static uint8_t AddBillingPeriodsInTable(publishBillingPeriodEntry_t *pTable, uint8_t len, zclCmdPrice_PublishBillingPeriodRsp_t *pMsg)
{
  uint32_t startTime, msgStartTime, currentTime;
  uint8_t i, poz = 0xff;
  /* get message time */
  msgStartTime = OTA2Native32(pMsg->BillingPeriodStartTime);
  currentTime = ZCL_GetUTCTime();
  if(msgStartTime == 0x000000)
  {
    msgStartTime = currentTime;
  }
  
  /* keep the Billing Periods in ascendent order; find the index in table where the message will be added */
  for(i = 0; i < len; i++)
  {
    if((pTable+i)->EntryStatus != 0x00)
    {
      startTime = OTA2Native32((pTable+i)->BillingPeriod.BillingPeriodStartTime);
      if(startTime == 0x000000)
      {
        startTime = OTA2Native32((pTable+i)->EffectiveStartTime);
      }
      if(((startTime > msgStartTime) && (startTime != 0xffffffff)) ||
         (msgStartTime == 0xffffffff)) 
      {
        poz = i;
        break;
      }
    }
    else
    {
      poz = i;
      break;
    }
  }
  /* check if the table is full; return oxff */
  if(poz == 0xff)
    return poz;
  /*move the content to the left*/
  if((pTable+poz)->EntryStatus !=0x00)
    FLib_MemInPlaceCpy((pTable+poz+1),(pTable+poz),(len-poz-1)* sizeof(publishBillingPeriodEntry_t));
  
  FLib_MemCpy((pTable+poz), pMsg, sizeof(zclCmdPrice_PublishBillingPeriodRsp_t));
  
  if((pMsg->BillingPeriodStartTime == 0x00000000)||
     (pMsg->BillingPeriodStartTime == 0xffffffff))
  
  {
    /*  here get the currentTime  */
    currentTime = ZCL_GetUTCTime();
    currentTime = Native2OTA32(currentTime);
    (pTable+poz)->EffectiveStartTime = currentTime;
  }
  else
    (pTable+poz)->EffectiveStartTime = pMsg->BillingPeriodStartTime ;
  /* new billing period was received */	
  (pTable+poz)->EntryStatus = gPriceReceivedStatus_c;
  /* Call the App to signal that a Block period was received; User should check EntryStatus */
  BeeAppUpdateDevice (0, gZclUI_BillingPeriodEvt_c, 0, 0, (pTable+poz));
  return poz;
}

/******************************************************************************/
/* The Billing Period timer callback that keep track of current active billing period */
static void TimerClientBillingPeriodCallBack(tmrTimerID_t timerID)
{
  (void) timerID;
   SendPriceClusterEvt(gzclEvtHandleClientBillingPeriod_c);
}

/******************************************************************************/
/* Process the received Publish Billing Period */
static zbStatus_t ZCL_ProcessClientPublishBillingPeriod(zclCmdPrice_PublishBillingPeriodRsp_t *pMsg)
{
  uint8_t updateIndex;
  zbStatus_t status = gZclFailure_c; 
  uint8_t newEntry;

  /* Check if it is a BillingPeriod Update */
  updateIndex = CheckForBillingPeriodUpdate(pMsg, (publishBillingPeriodEntry_t *)&gaClientBillingPeriodTable[0], gNumofClientBillingPeriods_c);
  /*if the Publish Price is not an update and is not rejected, add it in the table */
  if(updateIndex == 0xff)
  {
    /* Add the new Block Period information in the table */
    newEntry = AddBillingPeriodsInTable((publishBillingPeriodEntry_t *)&gaClientBillingPeriodTable[0], gNumofClientBillingPeriods_c, pMsg);
    if (newEntry != 0xff)
      status = gZclSuccess_c; 
  }
  else
  {
    if(updateIndex != 0xfe)
    {
      /* the price was updated */
      gaClientBillingPeriodTable[updateIndex].EntryStatus = gPriceUpdateStatus_c;
      /* Call the App to signal that a Price was updated; User should check EntryStatus */
      BeeAppUpdateDevice (0, gZclUI_BillingPeriodEvt_c, 0, 0, &gaClientBillingPeriodTable[updateIndex]);
      status = gZclSuccess_c;
    }
  }
  
  if(status == gZclSuccess_c)
    SendPriceClusterEvt(gzclEvtHandleClientBillingPeriod_c);
  
  return status;
}
#endif

#if gASL_ZclSE_Use_NextGenFeatures_d
/******************************************************************************/
/* Add CO2 Value in Table... so that to have the scheduled price in ascendent order */
static uint8_t AddCO2ValueInTable(publishCO2ValueEntry_t *pTable, uint8_t len, zclCmdPrice_PublishCO2ValueRsp_t *pMsg)
{
  uint32_t startTime, msgStartTime, currentTime;
  uint8_t i, poz = 0xff;
  /* get message time */
  msgStartTime = OTA2Native32(pMsg->StartTime);
  currentTime = ZCL_GetUTCTime();
  if(msgStartTime == 0x000000)
  {
    msgStartTime = currentTime;
  }
  
  /* keep the CO2 Value in ascendent order; find the index in table where the message will be added */
  for(i = 0; i < len; i++)
  {
    if((pTable+i)->EntryStatus != 0x00)
    {
      startTime = OTA2Native32((pTable+i)->CO2Value.StartTime);
      if(startTime == 0x000000)
      {
        startTime = OTA2Native32((pTable+i)->EffectiveStartTime);
      }
      if(((startTime > msgStartTime) && (startTime != 0xffffffff)) ||
         (msgStartTime == 0xffffffff)) 
      {
        poz = i;
        break;
      }
    }
    else
    {
      poz = i;
      break;
    }
  }
  /* check if the table is full; return oxff */
  if(poz == 0xff)
    return poz;
  /*move the content to the left*/
  if((pTable+poz)->EntryStatus !=0x00)
    FLib_MemInPlaceCpy((pTable+poz+1),(pTable+poz),(len-poz-1)* sizeof(publishCO2ValueEntry_t));
  
  FLib_MemCpy((pTable+poz), pMsg, sizeof(zclCmdPrice_PublishCO2ValueRsp_t));
  
  if((pMsg->StartTime == 0x00000000)||
     (pMsg->StartTime == 0xffffffff))
  
  {
    /*  here get the currentTime  */
    currentTime = ZCL_GetUTCTime();
    currentTime = Native2OTA32(currentTime);
    (pTable+poz)->EffectiveStartTime = currentTime;
  }
  else
    (pTable+poz)->EffectiveStartTime = pMsg->StartTime ;
  /* new billing period was received */	
  (pTable+poz)->EntryStatus = gPriceReceivedStatus_c;
  /* Call the App to signal that a Block period was received; User should check EntryStatus */
  BeeAppUpdateDevice (0, gZclUI_CO2ValueEvt_c, 0, 0, (pTable+poz));
  return poz;
}
#endif

#if gASL_ZclSE_Use_NextGenFeatures_d
/******************************************************************************/
/* Add Tariff Information in Table... so that to have the scheduled price in ascendent order */
static uint8_t AddTariffInformationInTable(publishTariffInformationEntry_t *pTable, uint8_t len, zclCmdPrice_PublishTariffInformationRsp_t *pMsg)
{
  uint32_t startTime, msgStartTime, currentTime;
  uint8_t i, poz = 0xff;
  /* get message time */
  msgStartTime = OTA2Native32(pMsg->StartTime);
  currentTime = ZCL_GetUTCTime();
  if(msgStartTime == 0x000000)
  {
    msgStartTime = currentTime;
  }
  
  /* keep the Tariff Information in ascendent order; find the index in table where the message will be added */
  for(i = 0; i < len; i++)
  {
    if((pTable+i)->EntryStatus != 0x00)
    {
      startTime = OTA2Native32((pTable+i)->TariffInformation.StartTime);
      if(startTime == 0x000000)
      {
        startTime = OTA2Native32((pTable+i)->EffectiveStartTime);
      }
      if(((startTime > msgStartTime) && (startTime != 0xffffffff)) ||
         (msgStartTime == 0xffffffff)) 
      {
        poz = i;
        break;
      }
    }
    else
    {
      poz = i;
      break;
    }
  }
  /* check if the table is full; return oxff */
  if(poz == 0xff)
    return poz;
  /*move the content to the left*/
  if((pTable+poz)->EntryStatus !=0x00)
    FLib_MemInPlaceCpy((pTable+poz+1),(pTable+poz),(len-poz-1)* sizeof(publishTariffInformationEntry_t));
  
  FLib_MemCpy((pTable+poz), pMsg, sizeof(zclCmdPrice_PublishTariffInformationRsp_t));
  
  if((pMsg->StartTime == 0x00000000)||
     (pMsg->StartTime == 0xffffffff))
  
  {
    /*  here get the currentTime  */
    currentTime = ZCL_GetUTCTime();
    currentTime = Native2OTA32(currentTime);
    (pTable+poz)->EffectiveStartTime = currentTime;
  }
  else
    (pTable+poz)->EffectiveStartTime = pMsg->StartTime ;
  /* new billing period was received */	
  (pTable+poz)->EntryStatus = gPriceReceivedStatus_c;
  /* Call the App to signal that a Tariff Information was received; User should check EntryStatus */
  BeeAppUpdateDevice (0, gZclUI_TariffInformationEvt_c, 0, 0, (pTable+poz));
  return poz;
}
#endif

#if gASL_ZclSE_Use_NextGenFeatures_d
uint8_t FindServerTariffInformationEntryByIssuerId(SEEvtId_t *pIssuerId)
{
  uint8_t i;
  
   /* Search for an existing entry*/
  for(i = 0; i < gNumofServerTariffInformation_c; i++)
  {    
    if(FLib_MemCmp(gaServerTariffInformationTable[i].TariffInformation.IssuerTariffID, pIssuerId, sizeof(SEEvtId_t)))
    {
      return i;
    }
  }
  return 0xFF;
}
#endif


#if gASL_ZclSE_Use_NextGenFeatures_d
uint8_t FindClientTariffInformationEntryByIssuerId(SEEvtId_t *pIssuerId)
{
  uint8_t i;
  
   /* Search for an existing entry*/
  for(i = 0; i < gNumofClientTariffInformation_c; i++)
  {    
    if(FLib_MemCmp(gaClientTariffInformationTable[i].TariffInformation.IssuerTariffID, pIssuerId, sizeof(SEEvtId_t)))
    {
      return i;
    }
  }
  return 0xFF;
}
#endif

#if gASL_ZclSE_Use_NextGenFeatures_d
/******************************************************************************/
/* Process the received Publish Block Thresholds */
static zbStatus_t ZCL_ProcessClientPublishBlockThresholds(zclCmdPrice_PublishBlockThresholdsRsp_t *pMsg)
{
  uint8_t entryIdx = 0xFF;
  uint8_t i, expectedNoBlockThresholds, noBlockThresholdsInUse, noBlockThresholdsPerFrame, tariffEntryIdx; 
  
   /* Search for an existing entry*/
  for(i = 0; i < gNumofClientBlockThresholds_c; i++)
  {    
    if(FLib_MemCmp(gaClientBlockThresholdsTable[i].IssuerEvtID, pMsg->IssuerEvtID, sizeof(SEEvtId_t)))
    {
      entryIdx = i;
      break;
    }
  }
  
  /*If it is a new entry find an empty slot for it*/
  if (entryIdx == 0xFF)
    for(i = 0; i < gNumofClientBlockThresholds_c; i++)
    {    
      if(gaClientBlockThresholdsTable[i].EntryStatus == gEntryNotUsed_c)
      {
        entryIdx = i;
        break;
      }
    }
  
  /* Cannot store infromation*/  
  if(entryIdx == 0xff)
    return gZclFailure_c;
  
  tariffEntryIdx = FindClientTariffInformationEntryByIssuerId(&gaClientBlockThresholdsTable[entryIdx].IssuerEvtID);
  noBlockThresholdsInUse = gaClientTariffInformationTable[tariffEntryIdx].TariffInformation.NumberBlockThreshholdsUse;
  noBlockThresholdsPerFrame = (uint8_t)(GetMaxApplicationPayload(NULL) - sizeof(zclFrame_t) - sizeof(SEEvtId_t) - 1) / sizeof(BlockThreshold_t);
  
  if (((pMsg->CommandIdx + 1) * noBlockThresholdsPerFrame) < noBlockThresholdsInUse)
    expectedNoBlockThresholds = noBlockThresholdsPerFrame;
  else  
    expectedNoBlockThresholds = noBlockThresholdsInUse - (pMsg->CommandIdx * noBlockThresholdsPerFrame);  
  
  /* Save the information in the Price Matrix Table*/
  FLib_MemCpy(&gaClientBlockThresholdsTable[entryIdx].IssuerEvtID, pMsg->IssuerEvtID, sizeof(SEEvtId_t));
  gaClientBlockThresholdsTable[entryIdx].Length = (pMsg->CommandIdx * noBlockThresholdsPerFrame) + noBlockThresholdsPerFrame;
  gaClientBlockThresholdsTable[entryIdx].EntryStatus = gEntryUsed_c;
  FLib_MemCpy(&gaClientBlockThresholdsTable[entryIdx].BlockThresholds[pMsg->CommandIdx * noBlockThresholdsPerFrame], &pMsg->BlockThresholds[0], expectedNoBlockThresholds);
  
  return gZclSuccess_c;
}
#endif

#if gASL_ZclSE_Use_NextGenFeatures_d
/******************************************************************************/
/* Process the received Publish Price Matrix */
static zbStatus_t ZCL_ProcessClientPublishPriceMatrix(zclCmdPrice_PublishPriceMatrixRsp_t *pMsg)
{
  uint8_t entryIdx = 0xFF;
  uint8_t i, expectedNoPrices, noPriceTiersInUse, noPriceBlocksInUse, noPricesPerFrame, tariffEntryIdx; 
  
   /* Search for an existing entry*/
  for(i = 0; i < gNumofClientPriceMatrix_c; i++)
  {    
    if(FLib_MemCmp(gaClientPriceMatrixTable[i].IssuerEvtID, pMsg->IssuerEvtID, sizeof(SEEvtId_t)))
    {
      entryIdx = i;
      break;
    }
  }
  
  /*If it is a new entry find an empty slot for it*/
  if (entryIdx == 0xFF)
    for(i = 0; i < gNumofClientPriceMatrix_c; i++)
    {    
      if(gaClientPriceMatrixTable[i].EntryStatus == gEntryNotUsed_c)
      {
        entryIdx = i;
        break;
      }
    }
  
  /* Cannot store infromation*/  
  if(entryIdx == 0xff)
    return gZclFailure_c;
  
  tariffEntryIdx = FindClientTariffInformationEntryByIssuerId(&gaClientPriceMatrixTable[entryIdx].IssuerEvtID);
  noPriceTiersInUse = gaClientTariffInformationTable[tariffEntryIdx].TariffInformation.NumberPriceTiersUse;
  noPriceBlocksInUse = gaClientTariffInformationTable[tariffEntryIdx].TariffInformation.NumberBlockThreshholdsUse + 1;
  noPricesPerFrame = (uint8_t)(GetMaxApplicationPayload(NULL) - sizeof(zclFrame_t) - sizeof(SEEvtId_t) - 1) / sizeof(Price_t);
  
  if (((pMsg->CommandIdx + 1) * noPricesPerFrame) < (noPriceTiersInUse * noPriceBlocksInUse))
    expectedNoPrices = noPricesPerFrame;
  else  
    expectedNoPrices = (noPriceTiersInUse * noPriceBlocksInUse) - (pMsg->CommandIdx * noPricesPerFrame);  
  
  /* Save the information in the Price Matrix Table*/
  FLib_MemCpy(&gaClientPriceMatrixTable[entryIdx].IssuerEvtID, pMsg->IssuerEvtID, sizeof(SEEvtId_t));
  gaClientPriceMatrixTable[entryIdx].Length = (pMsg->CommandIdx * noPricesPerFrame) + expectedNoPrices;
  gaClientPriceMatrixTable[entryIdx].EntryStatus = gEntryUsed_c;
  FLib_MemCpy(&gaClientBlockThresholdsTable[entryIdx].BlockThresholds[pMsg->CommandIdx * noPricesPerFrame], &pMsg->TierBlockPrice[0], expectedNoPrices);
  
  return gZclSuccess_c;
}
#endif

#if gASL_ZclSE_Use_NextGenFeatures_d
/******************************************************************************/
/* Process the received Publish CO2 Value */
static zbStatus_t ZCL_ProcessClientPublishCO2Value(zclCmdPrice_PublishCO2ValueRsp_t *pMsg)
{
  uint8_t updateIndex;
  zbStatus_t status = gZclFailure_c; 
  uint8_t newEntry;

   uint32_t msgIssuerId, entryIssuerId;
  
  msgIssuerId = GetNative32BitInt(pMsg->IssuerEvtID);
    /* Check if it is a CO2 Value Update */
  for(updateIndex=0; updateIndex < gNumofClientCO2Value_c; updateIndex++)
  {
    if(FLib_MemCmp(&gaClientCO2ValueTable[updateIndex].CO2Value.StartTime, &pMsg->StartTime, sizeof(ZCLTime_t))
       && (gaClientCO2ValueTable[updateIndex].EntryStatus != gEntryNotUsed_c))
    {
      entryIssuerId = GetNative32BitInt(gaClientCO2ValueTable[updateIndex].CO2Value.IssuerEvtID);
      if(entryIssuerId > msgIssuerId)
      {
        updateIndex = 0xFE;// reject it;
      }
      else
      {
        FLib_MemCpy(&gaClientCO2ValueTable[updateIndex], pMsg, sizeof(zclCmdPrice_PublishCO2ValueRsp_t));
      }
      break;
    }
  }
  /*if the Publish CO2 value is not rejected, add it in the table */
  
  if(updateIndex != 0xfe)
  {
    if(updateIndex == gNumofClientCO2Value_c)
    {
      /* Add the new CO2 value in the table */
      newEntry = AddCO2ValueInTable((publishCO2ValueEntry_t *)&gaClientCO2ValueTable[0], gNumofClientCO2Value_c, pMsg);
      if (newEntry != 0xff)
      status = gZclSuccess_c; 
    }
    else
    {
      /* the CO2 value was updated */
      gaClientCO2ValueTable[updateIndex].EntryStatus = gPriceUpdateStatus_c;
      status = gZclSuccess_c;
    }
  }
  
  //if(status == gZclSuccess_c)
    //TS_SendEvent(gZclTaskId, gzclEvtHandleClientPrices_c);
  
  return status;
}
#endif

#if gASL_ZclSE_Use_NextGenFeatures_d
/******************************************************************************/
/* Process the received Publish Tariff Information */
static zbStatus_t ZCL_ProcessClientPublishTariffInformation(zclCmdPrice_PublishTariffInformationRsp_t *pMsg)
{
  uint8_t updateIndex;
  zbStatus_t status = gZclFailure_c; 
  uint8_t newEntry;
  uint32_t msgIssuerId, entryIssuerId;
  
  msgIssuerId = GetNative32BitInt(pMsg->IssuerTariffID);
    /* Check if it is a Tariff Information Update */
  for(updateIndex=0; updateIndex < gNumofClientTariffInformation_c; updateIndex++)
  {
    if(FLib_MemCmp(&gaClientTariffInformationTable[updateIndex].TariffInformation.StartTime, &pMsg->StartTime, sizeof(ZCLTime_t))
       && (gaClientTariffInformationTable[updateIndex].EntryStatus != gEntryNotUsed_c))
    {
      entryIssuerId = GetNative32BitInt(gaClientTariffInformationTable[updateIndex].TariffInformation.IssuerTariffID);
      if(entryIssuerId > msgIssuerId)
      {
        updateIndex = 0xFE;// reject it;
      }
      else
      {
        FLib_MemCpy(&gaClientTariffInformationTable[updateIndex], pMsg, sizeof(zclCmdPrice_PublishTariffInformationRsp_t));
      }
      break;
    }
  }
  /*if the Publish Tariff Information is not rejected, add it in the table */
  
  if(updateIndex != 0xfe)
  {
    if(updateIndex == gNumofClientTariffInformation_c)
    {
      /* Add the new Tariff information in the table */
      newEntry = AddTariffInformationInTable((publishTariffInformationEntry_t *)&gaClientTariffInformationTable[0], gNumofClientTariffInformation_c, pMsg);
      if (newEntry != 0xff)
      status = gZclSuccess_c; 
    }
    else
    {
      /* the Tariff Information was updated */
      gaClientTariffInformationTable[updateIndex].EntryStatus = gPriceUpdateStatus_c;
      status = gZclSuccess_c;
    }
  }
  
  //if(status == gZclSuccess_c)
    //TS_SendEvent(gZclTaskId, gzclEvtHandleTariffInformation_c);
  
  return status;
}
#endif

#if gASL_ZclSE_Use_NextGenFeatures_d
static uint8_t CheckForCPPEventUpdate(zclCmdPrice_PublishCPPEventRsp_t *pMsg, publishCPPEventEntry_t *pTable, uint8_t len)
{
  uint8_t i;
  uint32_t msgIssuerEvt, entryIssuerEvt;
  
  msgIssuerEvt = FourBytesToUint32(pMsg->IssuerEvtID);
  msgIssuerEvt = OTA2Native32(msgIssuerEvt);
  
  for(i = 0; i < len; i++)
  {
    if((pTable+i)->EntryStatus != 0x00)
    {
      entryIssuerEvt = FourBytesToUint32(&(pTable+i)->CPPEvent.IssuerEvtID);
      entryIssuerEvt = OTA2Native32(entryIssuerEvt);
      if (FLib_MemCmp(&(pTable+i)->CPPEvent.IssuerEvtID, &pMsg->IssuerEvtID, sizeof(SEEvtId_t)))
        FLib_MemCpy((pTable+i), pMsg, sizeof(zclCmdPrice_PublishCPPEventRsp_t));
        return i;
    }
  }
  return 0xff;
}
#endif

#if gASL_ZclSE_Use_NextGenFeatures_d
/******************************************************************************/
/* Add CPP Event in Table... so that to have the scheduled events in asccendent order */
static uint8_t AddCPPEventsInTable(publishCPPEventEntry_t *pTable, uint8_t len, zclCmdPrice_PublishCPPEventRsp_t *pMsg)
{
  uint32_t startTime, msgStartTime, currentTime;
  uint8_t i, poz = 0xff;
  /* get message time */
  msgStartTime = OTA2Native32(pMsg->CPPEventStartTime);
  currentTime = ZCL_GetUTCTime();
  if(msgStartTime == 0x000000)
  {
    msgStartTime = currentTime;
  }
  
  /* keep the events in ascendent order; find the index in table where the message will be added */
  for(i = 0; i < len; i++)
  {
    if((pTable+i)->EntryStatus != 0x00)
    {
      startTime = OTA2Native32((pTable+i)->CPPEvent.CPPEventStartTime);
      if(startTime == 0x000000)
      {
        startTime = OTA2Native32((pTable+i)->EffectiveStartTime);
      }
      if(((startTime > msgStartTime) && (startTime != 0xffffffff)) ||
         (msgStartTime == 0xffffffff)) 
      {
        poz = i;
        break;
      }
    }
    else
    {
      poz = i;
      break;
    }
  }
  /* check if the table is full; return oxff */
  if(poz == 0xff)
    return poz;
  /*move the content to the left*/
  if((pTable+poz)->EntryStatus !=0x00)
    FLib_MemInPlaceCpy((pTable+poz+1),(pTable+poz),(len-poz-1)* sizeof(publishCPPEventEntry_t));
  
  FLib_MemCpy((pTable+poz), pMsg, sizeof(zclCmdPrice_PublishCPPEventRsp_t));
  
  if((pMsg->CPPEventStartTime == 0x00000000)||
     (pMsg->CPPEventStartTime == 0xffffffff))
  
  {
    /*  here get the currentTime  */
    currentTime = ZCL_GetUTCTime();
    currentTime = Native2OTA32(currentTime);
    (pTable+poz)->EffectiveStartTime = currentTime;
  }
  else
    (pTable+poz)->EffectiveStartTime = pMsg->CPPEventStartTime ;
  /* new event was received */	
  (pTable+poz)->EntryStatus = gPriceReceivedStatus_c;
  return poz;
}
#endif

#if gASL_ZclSE_Use_NextGenFeatures_d
/******************************************************************************/
/* Process the received Publish CPP Event */
static zbStatus_t ZCL_ProcessClientPublishCPPEvent(zclCmdPrice_PublishCPPEventRsp_t *pMsg)
{
  uint8_t updateIndex;
  zbStatus_t status = gZclFailure_c; 

  /* Check if it is a CPP Event Update */
  updateIndex = CheckForCPPEventUpdate(pMsg, (publishCPPEventEntry_t *)&gaClientCPPEventTable[0], gNumofClientCPPEvents_c);
  /*if the Publish Price is not an update and is not rejected, add it in the table */
  if(updateIndex == 0xff)
  {
    /* Add the new CPP Event in the table */
    updateIndex = AddCPPEventsInTable((publishCPPEventEntry_t *)&gaClientCPPEventTable[0], gNumofClientCPPEvents_c, pMsg);
    if (updateIndex != 0xff)
      status = gZclSuccess_c;
  }
  else
  {
    if(updateIndex != 0xfe)
    {
      FLib_MemCpy(&gaClientCPPEventTable[updateIndex], pMsg, sizeof(zclCmdPrice_PublishCPPEventRsp_t));
      status = gZclSuccess_c;
    }
}
  
  if ((status == gZclSuccess_c) && gaClientCPPEventTable[updateIndex].CPPEvent.CPPAuth != gPriceCPPEventForced)
    BeeAppUpdateDevice (0, gZclUI_CPPEvt_c, 0, 0, &updateIndex);
  
  return status;
}
#endif

/******************************************************************************/
/*Check and Updated a BillingPeriod.
When new BillingPeriod information is provided that replaces older block period
information for the same time period, IssuerEvt field allows devices to determine which
information is newer. It is expected that the value contained in this field is a
unique number managed by upstream servers.
Thus, newer BillingPeriod information will have a value in the Issuer Event ID field that
is larger than older pricing information.
*/
#if gASL_ZclSE_Use_NextGenFeatures_d
static uint8_t CheckForBillingPeriodUpdate(zclCmdPrice_PublishBillingPeriodRsp_t *pMsg, publishBillingPeriodEntry_t *pTable, uint8_t len)
{
  uint8_t i;
  uint32_t msgIssuerEvt, entryIssuerEvt;
  
  msgIssuerEvt = FourBytesToUint32(pMsg->IssuerEvtID);
  msgIssuerEvt = OTA2Native32(msgIssuerEvt);
  
  for(i = 0; i < len; i++)
  {
    if((pTable+i)->EntryStatus != 0x00)
    {
      entryIssuerEvt = FourBytesToUint32(&(pTable+i)->BillingPeriod.IssuerEvtID);
      entryIssuerEvt = OTA2Native32(entryIssuerEvt);
      if ((pTable+i)->BillingPeriod.BillingPeriodStartTime == pMsg->BillingPeriodStartTime &&
          FLib_MemCmp(&(pTable+i)->BillingPeriod.BillingPeriodDurationInMinutes, &pMsg->BillingPeriodDurationInMinutes, sizeof(Duration24_t)))
        if(entryIssuerEvt < msgIssuerEvt)
        {
          FLib_MemCpy((pTable+i), pMsg, sizeof(zclCmdPrice_PublishBillingPeriodRsp_t));
          return i;
        }
        else 
          return 0xFE; /* reject it */
    }
  }
  return 0xff;
}
#endif

/******************************************************************************/
/* Handle the Client Billing Period signalling when the current Billing starts, was updated or is completed */
#if gASL_ZclSE_Use_NextGenFeatures_d
void ZCL_HandleClientBillingPeriod(void)
{
  uint32_t currentTime, startTime, nextTime = 0x00000000, stopTime;
  uint32_t duration;
  publishBillingPeriodEntry_t *pEntry = &gaClientBillingPeriodTable[0];
  
  /* the Billing Period table is kept in ascendent order; check if any Billing Period is scheduled*/
  if(pEntry->EntryStatus == 0x00)
  {
    TMR_StopSecondTimer(gBillingPeriodClientTimerID);
    return;
  }
  
  /* Get the timing */
  currentTime = ZCL_GetUTCTime();
  startTime = OTA2Native32(pEntry->EffectiveStartTime);
  duration = GetNative32BitIntFrom3ByteArray(pEntry->BillingPeriod.BillingPeriodDurationInMinutes);
  stopTime = startTime + (60 * duration);
  /* Check if the Price Event is completed */
  if(stopTime <= currentTime)
  {
    pEntry->EntryStatus = gPriceCompletedStatus_c; /* entry is not used anymore */
    /* Call the App to signal that a Price was completed; User should check EntryStatus */
    BeeAppUpdateDevice (0, gZclUI_BillingPeriodEvt_c, 0, 0, pEntry);
    pEntry->EntryStatus = 0x00;
    FLib_MemInPlaceCpy(pEntry, (pEntry+1), (gNumofClientBillingPeriods_c-1) * sizeof(publishBillingPeriodEntry_t));
    gaClientBillingPeriodTable[gNumofClientBillingPeriods_c-1].EntryStatus = 0x00;
  }
  else
  {
    if(startTime <= currentTime) /* check if the Price event have to be started or updated */
    {
      if((pEntry->EntryStatus == gPriceReceivedStatus_c)||
         (pEntry->EntryStatus == gPriceUpdateStatus_c))
      {
        pEntry->EntryStatus = gPriceStartedStatus_c;
        /* Call the App to signal that a BillingPeriod was started; User should check EntryStatus */
        BeeAppUpdateDevice (0, gZclUI_BillingPeriodEvt_c, 0, 0, pEntry);
      }
    }
  }

  if(currentTime < startTime)
    nextTime = startTime - currentTime;
  else
    if(currentTime < stopTime)
      nextTime = stopTime - currentTime;
  if (nextTime)
    TMR_StartSecondTimer(gBillingPeriodClientTimerID,(uint16_t)nextTime, TimerClientBillingPeriodCallBack);
  
}
#endif

#if gASL_ZclSE_Use_NextGenFeatures_d
/******************************************************************************/
/* Add Consolidated Bill in Table... so that to have the scheduled consolidated bill in asccendent order */
static uint8_t AddConsolidatedBillsInTable(publishConsolidatedBillEntry_t *pTable, uint8_t len, zclCmdPrice_PublishConsolidatedBillRsp_t *pMsg)
{
  uint32_t startTime, msgStartTime, currentTime;
  uint8_t i, poz = 0xff;
  /* get message time */
  msgStartTime = OTA2Native32(pMsg->BillingPeriodStartTime);
  currentTime = ZCL_GetUTCTime();
  if(msgStartTime == 0x000000)
  {
    msgStartTime = currentTime;
  }
  
  /* keep the Consolidated Bill in ascendent order; find the index in table where the message will be added */
  for(i = 0; i < len; i++)
  {
    if((pTable+i)->EntryStatus != 0x00)
    {
      startTime = OTA2Native32((pTable+i)->ConsolidatedBill.BillingPeriodStartTime);
      if(startTime == 0x000000)
      {
        startTime = OTA2Native32((pTable+i)->EffectiveStartTime);
      }
      if(((startTime > msgStartTime) && (startTime != 0xffffffff)) ||
         (msgStartTime == 0xffffffff)) 
      {
        poz = i;
        break;
      }
    }
    else
    {
      poz = i;
      break;
    }
  }
  /* check if the table is full; return oxff */
  if(poz == 0xff)
    return poz;
  /*move the content to the left*/
  if((pTable+poz)->EntryStatus !=0x00)
    FLib_MemInPlaceCpy((pTable+poz+1),(pTable+poz),(len-poz-1)* sizeof(publishConsolidatedBillEntry_t));
  
  FLib_MemCpy((pTable+poz), pMsg, sizeof(zclCmdPrice_PublishConsolidatedBillRsp_t));
  
  if((pMsg->BillingPeriodStartTime == 0x00000000)||
     (pMsg->BillingPeriodStartTime == 0xffffffff))
  
  {
    /*  here get the currentTime  */
    currentTime = ZCL_GetUTCTime();
    currentTime = Native2OTA32(currentTime);
    (pTable+poz)->EffectiveStartTime = currentTime;
  }
  else
    (pTable+poz)->EffectiveStartTime = pMsg->BillingPeriodStartTime ;
  /* new billing period was received */	
  (pTable+poz)->EntryStatus = gPriceReceivedStatus_c;
  /* Call the App to signal that a Consolidated Bill was received; User should check EntryStatus */
  BeeAppUpdateDevice (0, gZclUI_ConsolidatedBillEvt_c, 0, 0, (pTable+poz));
  return poz;
}

/******************************************************************************/
/* The Consolidated Bill timer callback that keep track of current active consolidated bill */
static void TimerClientConsolidatedBillCallBack(tmrTimerID_t timerID)
{
  (void) timerID;
   SendPriceClusterEvt(gzclEvtHandleClientConsolidatedBill_c);
}

/******************************************************************************/
/* Process the received Publish Consolidated Bill */
static zbStatus_t ZCL_ProcessClientPublishConsolidatedBill(zclCmdPrice_PublishConsolidatedBillRsp_t *pMsg)
{
  uint8_t updateIndex;
  zbStatus_t status = gZclFailure_c; 
  uint8_t newEntry;

  /* Check if it is a Consolidated Bill Update */
  updateIndex = CheckForConsolidatedBillUpdate(pMsg, (publishConsolidatedBillEntry_t *)&gaClientConsolidatedBillTable[0], gNumofClientConsolidatedBill_c);
  /*if the Publish Price is not an update and is not rejected, add it in the table */
  if(updateIndex == 0xff)
  {
    /* Add the new Consolidated Bill information in the table */
    newEntry = AddConsolidatedBillsInTable((publishConsolidatedBillEntry_t *)&gaClientConsolidatedBillTable[0], gNumofClientConsolidatedBill_c, pMsg);
    if (newEntry != 0xff)
      status = gZclSuccess_c; 
  }
  else
  {
    if(updateIndex != 0xfe)
    {
      /* the price was updated */
      gaClientConsolidatedBillTable[updateIndex].EntryStatus = gPriceUpdateStatus_c;
      /* Call the App to signal that a Consolidated Bill was updated; User should check EntryStatus */
      BeeAppUpdateDevice (0, gZclUI_ConsolidatedBillEvt_c, 0, 0, &gaClientConsolidatedBillTable[updateIndex]);
      status = gZclSuccess_c;
    }
  }
  
  if(status == gZclSuccess_c)
    SendPriceClusterEvt(gzclEvtHandleClientConsolidatedBill_c);
  
  return status;
}
#endif

/******************************************************************************/
/*Check and Updated a Consolidated Bill.
When new Consolidated Bill information is provided that replaces older block period
information for the same time period, IssuerEvt field allows devices to determine which
information is newer. It is expected that the value contained in this field is a
unique number managed by upstream servers.
Thus, newer Consolidated Bill information will have a value in the Issuer Event ID field that
is larger than older pricing information.
*/
#if gASL_ZclSE_Use_NextGenFeatures_d
static uint8_t CheckForConsolidatedBillUpdate(zclCmdPrice_PublishConsolidatedBillRsp_t *pMsg, publishConsolidatedBillEntry_t *pTable, uint8_t len)
{
  uint8_t i;
  uint32_t msgIssuerEvt, entryIssuerEvt;
  
  msgIssuerEvt = FourBytesToUint32(pMsg->IssuerEvtID);
  msgIssuerEvt = OTA2Native32(msgIssuerEvt);
  
  for(i = 0; i < len; i++)
  {
    if((pTable+i)->EntryStatus != 0x00)
    {
      entryIssuerEvt = FourBytesToUint32(&(pTable+i)->ConsolidatedBill.IssuerEvtID);
      entryIssuerEvt = OTA2Native32(entryIssuerEvt);
      if ((pTable+i)->ConsolidatedBill.BillingPeriodStartTime == pMsg->BillingPeriodStartTime &&
          FLib_MemCmp(&(pTable+i)->ConsolidatedBill.BillingPeriodDurationInMinutes, &pMsg->BillingPeriodDurationInMinutes, sizeof(Duration24_t)))
        if(entryIssuerEvt < msgIssuerEvt)
        {
          FLib_MemCpy((pTable+i), pMsg, sizeof(zclCmdPrice_PublishConsolidatedBillRsp_t));
          return i;
        }
        else 
          return 0xFE; /* reject it */
    }
  }
  return 0xff;
}
#endif

/******************************************************************************/
/* Handle the Client Consolidated Bill signalling when the current Billing starts, was updated or is completed */
#if gASL_ZclSE_Use_NextGenFeatures_d
void ZCL_HandleClientConsolidatedBill(void)
{
  uint32_t currentTime, startTime, nextTime = 0x00000000, stopTime;
  uint32_t duration;
  publishConsolidatedBillEntry_t *pEntry = &gaClientConsolidatedBillTable[0];
  
  /* the Billing Period table is kept in ascendent order; check if any Billing Period is scheduled*/
  if(pEntry->EntryStatus == 0x00)
  {
    TMR_StopSecondTimer(gConsolidatedBillClientTimerID);
    return;
  }
  
  /* Get the timing */
  currentTime = ZCL_GetUTCTime();
  startTime = OTA2Native32(pEntry->EffectiveStartTime);
  duration = GetNative32BitIntFrom3ByteArray(pEntry->ConsolidatedBill.BillingPeriodDurationInMinutes);
  stopTime = startTime + (60 * duration);
  /* Check if the Price Event is completed */
  if(stopTime <= currentTime)
  {
    pEntry->EntryStatus = gPriceCompletedStatus_c; /* entry is not used anymore */
    /* Call the App to signal that a Price was completed; User should check EntryStatus */
    BeeAppUpdateDevice (0, gZclUI_ConsolidatedBillEvt_c, 0, 0, pEntry);
    pEntry->EntryStatus = 0x00;
    FLib_MemInPlaceCpy(pEntry, (pEntry+1), (gNumofClientConsolidatedBill_c-1) * sizeof(publishConsolidatedBillEntry_t));
    gaClientConsolidatedBillTable[gNumofClientConsolidatedBill_c-1].EntryStatus = 0x00;
  }
  else
  {
    if(startTime <= currentTime) /* check if the Price event have to be started or updated */
    {
      if((pEntry->EntryStatus == gPriceReceivedStatus_c)||
         (pEntry->EntryStatus == gPriceUpdateStatus_c))
      {
        pEntry->EntryStatus = gPriceStartedStatus_c;
        /* Call the App to signal that a ConsolidatedBill was started; User should check EntryStatus */
        BeeAppUpdateDevice (0, gZclUI_ConsolidatedBillEvt_c, 0, 0, pEntry);
      }
    }
  }

  if(currentTime < startTime)
    nextTime = startTime - currentTime;
  else
    if(currentTime < stopTime)
      nextTime = stopTime - currentTime;
  if (nextTime)
    TMR_StartSecondTimer(gConsolidatedBillClientTimerID,(uint16_t)nextTime, TimerClientConsolidatedBillCallBack);
  
}
#endif

/******************************************************************************/
/******************************************************************************/
/* Handle PRICE Inter Pan Server\Client Indications                           */ 
/******************************************************************************/
zbStatus_t ZCL_InterPanPriceClusterClient
(
	zbInterPanDataIndication_t *pIndication, /* IN: */
	afDeviceDef_t *pDev                /* IN: */
) 
{
 zclCmd_t *pCmd;
zbStatus_t status = gZclSuccess_c;

(void)pDev;
pCmd = (zclCmd_t *)&(((zclFrame_t *)pIndication->pAsdu)->command);
if(*pCmd == gZclCmdPrice_PublishPriceRsp_c)
{
    status = ZCL_ProcessClientPublishPrice((zclCmdPrice_PublishPriceRsp_t *)(pCmd+sizeof(zclCmd_t)));
    if(status != gZclSuccess_c)
    {
      return status;
    }
    else
    {
      return ZCL_SendInterPriceAck(pIndication);
    }
}

 return gZclUnsupportedClusterCommand_c;
}

/******************************************************************************/
zbStatus_t ZCL_InterPanPriceClusterServer
(
zbInterPanDataIndication_t *pIndication, /* IN: */
afDeviceDef_t *pDev                /* IN: */
) 
{
  
  zclCmd_t Cmd;
  zbStatus_t status = gZclSuccessDefaultRsp_c;
  addrInfoType_t replyAddrInfo;
  zclCmdPrice_GetCurrPriceReq_t *pGetCurrPrice;
#if gASL_ZclPrice_Optionals_d
  zclCmdPrice_GetScheduledPricesReq_t *pGetScheduledPrice;
#endif
  (void) pDev;
  /* Get the cmd and the SE message */
  Cmd = ((zclFrame_t *)pIndication->pAsdu)->command;
  /* get address ready for reply */
  PrepareInterPanForReply((InterPanAddrInfo_t *)&replyAddrInfo, pIndication); 
  switch(Cmd) {
  case gZclCmdPrice_GetCurrPriceReq_c:
    pGetCurrPrice = (zclCmdPrice_GetCurrPriceReq_t *)((uint8_t*)pIndication->pAsdu + sizeof(zclFrame_t));
    status = ZCL_ProcessGetCurrPriceReq(&replyAddrInfo, pGetCurrPrice, TRUE);
    
    break;
#if gASL_ZclPrice_Optionals_d     
  case gZclCmdPrice_GetScheduledPricesReq_c:
    pGetScheduledPrice = (zclCmdPrice_GetScheduledPricesReq_t *)((uint8_t*)pIndication->pAsdu + sizeof(zclFrame_t));
    status = ZCL_ProcessGetScheduledPricesReq(&replyAddrInfo, pGetScheduledPrice, TRUE);
    break;  
#endif
  case gZclCmdPrice_PriceAck_c:
    status = gZclSuccess_c;
    break;
    
  default:
    status = gZclUnsupportedClusterCommand_c;
    break;
    
  }
  
  return status;
  
}


/******************************************************************************/
void ZCL_DeleteClientPrice(uint8_t *pEvtId)
{
  uint8_t i;
  for(i = 0; i < gNumofClientPrices_c; i++)
  {
    if (gaClientPriceTable[i].EntryStatus != 0x00)
    {
      if(FLib_MemCmp(pEvtId, gaClientPriceTable[i].Price.IssuerEvt, 4))
      {
        gaClientPriceTable[i].EntryStatus = 0x00;
        return;
      }
      
    }  
  }
}

/******************************************************************************/
void ZCL_DeleteServerScheduledPrices(void)
{
  uint8_t i;
  for(i = 0; i < gNumofServerPrices_c; i++)
    gaServerPriceTable[i].EntryStatus = 0x00;
}

/******************************************************************************/
/*Update a price from the Provider.
    In the case of an update to the
pricing information from the commodity provider, the Publish Price command
should be unicast to all individually registered devices implementing the Price
Cluster on the ZigBee Smart Energy network. When responding to a request via
the Inter-PAN SAP, the Publish Price command should be broadcast to the PAN of
the requester after a random delay between 0 and 0.5 seconds, to avoid a potential
broadcast storm of packets.*/
zbStatus_t ZCL_UpdateServerPriceEvents( zclCmdPrice_PublishPriceRsp_t *pMsg)
{
  uint8_t updateIndex;
  
  updateIndex = CheckForPriceUpdate(pMsg, (publishPriceEntry_t *)&gaServerPriceTable[0], gNumofServerPrices_c);
  /* the Price is updated??? */
  if (updateIndex < 0xFE)
  {
    /* Send the Publish Prices to all SE or Non-SE registered devices */
    mRegIndex = 0; 
    mInterPanIndex = 0;
    mUpdatePriceIndex = updateIndex;
    SendPriceClusterEvt(gzclEvtHandlePublishPriceUpdate_c);
    return gZclSuccess_c;
  }
  else
    return gZclFailure_c;
}

/******************************************************************************/
/* Store Price information received from the Provider 
     The server side doesn't keep track of the price status, only stores the 
     received prices and take care that Nested and overlapping Publish Price commands not to occur*/
zbStatus_t ZCL_ScheduleServerPriceEvents ( ztcCmdPrice_PublishPriceRsp_t *pMsg)
{
  uint8_t i, newEntry;
  uint32_t currentTime, startTime,  stopTime;
  uint32_t msgStartTime, msgStopTime; 
  uint16_t duration;
  //0 if no overlapp exception, 1- gNumofServerPrices_c if a overlapp exception occurs
  bool_t overlapException = 0;
  
  
  /* here get the currentTime */
  currentTime = ZCL_GetUTCTime();
  msgStartTime = OTA2Native32(pMsg->publishPriceRsp.StartTime);
  if((msgStartTime == 0x00000000) || (msgStartTime == 0xffffffff))
    msgStartTime = currentTime;
  duration = OTA2Native16(pMsg->publishPriceRsp.DurationInMinutes);
  msgStopTime = msgStartTime + (60 * (uint32_t)duration);
  
  //if(msgStopTime <= currentTime)
  //	return status;
  
  /* Nested and overlapping Publish Price commands are not allowed */
  for(i = 0; i < gNumofServerPrices_c; i++)
  {
    if(gaServerPriceTable[i].EntryStatus == 0x00)
      continue;
    /* Get the timing */
    startTime = OTA2Native32(gaServerPriceTable[i].EffectiveStartTime);
    duration = OTA2Native16(gaServerPriceTable[i].Price.DurationInMinutes);
    stopTime = startTime + (60 * (uint32_t)duration);
    if ((msgStartTime >= stopTime)||(startTime >= msgStopTime))
    {
      continue;
    }else
    {
      /*
      The only exception to this is that if an event with a newer Issuer Event ID
      overlaps with the end of the current active price but is not yet active,
      the active price is not deleted but its duration is modified to 0xFFFF 
      (until changed) so that the active price ends when the new event begins
      */
      if((startTime <= currentTime)&&(stopTime>currentTime)&&(currentTime <= msgStartTime))
      {
        overlapException = i+1;
        break;
      }
      /*the price overlapp... take no action*/
      return gZclFailure_c;
    }
  }
  
  if((overlapException != 0) && (currentTime == msgStartTime))
    return ZCL_UpdateServerPriceEvents(&pMsg->publishPriceRsp);
  
  newEntry = AddPriceInTable((publishPriceEntry_t *)&gaServerPriceTable[0], gNumofServerPrices_c, &pMsg->publishPriceRsp);
  if(newEntry == 0xff)
    return gZclFailure_c;
  else
  {
    if(overlapException != 0)
    {
      gaServerPriceTable[overlapException-1].Price.DurationInMinutes = 0xFFFF;  
    }
  }
  if(pMsg->SendPublishBlockPeriodImmediately)
  {
     mRegIndex = 0; 
     mInterPanIndex = 0;
     mUpdatePriceIndex = newEntry;
     SendPriceClusterEvt(gzclEvtHandlePublishPriceUpdate_c);
  }
  return gZclSuccess_c;
} 

/******************************************************************************/
zbStatus_t ZCL_PriceClusterServer
(
     zbApsdeDataIndication_t *pIndication, /* IN: */
     afDeviceDef_t *pDev                /* IN: */
) 
{
  zclCmd_t Cmd;
  zbStatus_t status = gZclSuccessDefaultRsp_c;
  addrInfoType_t replyAddrInfo;
  zclCmdPrice_GetCurrPriceReq_t *pGetCurrPrice;
#if gASL_ZclPrice_Optionals_d
  zclCmdPrice_GetScheduledPricesReq_t *pGetScheduledPrice;
  zclCmdPrice_GetBlockPeriodsReq_t *pGetBlockPeriods;
#endif
#if gASL_ZclPrice_Optionals_d && gASL_ZclSE_Use_NextGenFeatures_d
  zclCmdPrice_GetConversionFactorReq_t *pGetConversionFactor;
  zclCmdPrice_GetCalorificValueReq_t *pGetCalorificValue;
#endif
#if gASL_ZclSE_Use_NextGenFeatures_d
  zclCmdPrice_GetPriceMatrixReq_t *pGetPriceMatrix;
  zclCmdPrice_GetBlockThresholdsReq_t *pGetBlockThresholds;
  zclCmdPrice_GetCO2ValueReq_t *pGetCO2Value;
  zclCmdPrice_GetBillingPeriodReq_t *pGetBillingPeriod;    
  zclCmdPrice_CPPEventRsp_t *pCPPEventRsp;  
  zclCmdPrice_GetConsolidatedBillReq_t *pGetConsolidatedBill;  
  zclCmdPrice_GetTariffInformationReq_t *pGetTariffInformation;
#endif
  (void) pDev;
  /* Get the cmd and the SE message */
  Cmd = ((zclFrame_t *)pIndication->pAsdu)->command;
  /* get address ready for reply */
  AF_PrepareForReply((afAddrInfo_t*)&replyAddrInfo, pIndication); 
  switch(Cmd) {
  case gZclCmdPrice_GetCurrPriceReq_c:
    pGetCurrPrice = (zclCmdPrice_GetCurrPriceReq_t *)((uint8_t*)pIndication->pAsdu + sizeof(zclFrame_t));
    status = ZCL_ProcessGetCurrPriceReq(&replyAddrInfo, pGetCurrPrice, FALSE);
    break;
    
#if gASL_ZclPrice_Optionals_d   
  case gZclCmdPrice_GetScheduledPricesReq_c:
    pGetScheduledPrice = (zclCmdPrice_GetScheduledPricesReq_t *)((uint8_t*)pIndication->pAsdu + sizeof(zclFrame_t));
    status = ZCL_ProcessGetScheduledPricesReq(&replyAddrInfo, pGetScheduledPrice, FALSE);
    break;
#endif    
    
#if gASL_ZclSE_Use_NextGenFeatures_d
  case gZclCmdPrice_GetBillingPeriod_c:
    pGetBillingPeriod = (zclCmdPrice_GetBillingPeriodReq_t *)((uint8_t*)pIndication->pAsdu + sizeof(zclFrame_t));
    status = ZCL_ProcessGetBillingPeriodReq(&replyAddrInfo, pGetBillingPeriod);
    break;
#endif

#if gASL_ZclSE_Use_NextGenFeatures_d
  case gZclCmdPrice_GetTariffInformation_c:
    pGetTariffInformation = (zclCmdPrice_GetTariffInformationReq_t *)((uint8_t*)pIndication->pAsdu + sizeof(zclFrame_t));
    status = ZCL_ProcessGetTariffInformationReq(&replyAddrInfo, pGetTariffInformation);
    break;
#endif
    
#if gASL_ZclSE_Use_NextGenFeatures_d
  case gZclCmdPrice_GetConsolidatedBill_c:
    pGetConsolidatedBill = (zclCmdPrice_GetConsolidatedBillReq_t *)((uint8_t*)pIndication->pAsdu + sizeof(zclFrame_t));
    status = ZCL_ProcessGetConsolidatedBillReq(&replyAddrInfo, pGetConsolidatedBill);
    break;
#endif
    
#if gASL_ZclSE_Use_NextGenFeatures_d
  case gZclCmdPrice_GetCPPEventResponse_c:
    pCPPEventRsp = (zclCmdPrice_CPPEventRsp_t *)((uint8_t*)pIndication->pAsdu + sizeof(zclFrame_t));
    status = ZCL_ProcessCPPEventRsp(&replyAddrInfo, pCPPEventRsp);
    break;
#endif
    
#if gASL_ZclSE_Use_NextGenFeatures_d
  case gZclCmdPrice_GetCO2Value_c:
    pGetCO2Value = (zclCmdPrice_GetCO2ValueReq_t *)((uint8_t*)pIndication->pAsdu + sizeof(zclFrame_t));
    status = ZCL_ProcessGetCO2ValueReq(&replyAddrInfo, pGetCO2Value);
    break;
#endif

#if gASL_ZclSE_Use_NextGenFeatures_d
  case gZclCmdPrice_GetPriceMatrix_c:
    pGetPriceMatrix = (zclCmdPrice_GetPriceMatrixReq_t *)((uint8_t*)pIndication->pAsdu + sizeof(zclFrame_t));
    status = ZCL_ProcessGetPriceMatrixReq(&replyAddrInfo, pGetPriceMatrix);
    break;
#endif
    
#if gASL_ZclSE_Use_NextGenFeatures_d
  case gZclCmdPrice_GetBlockThresholds_c:
    pGetBlockThresholds = (zclCmdPrice_GetBlockThresholdsReq_t *)((uint8_t*)pIndication->pAsdu + sizeof(zclFrame_t));
    status = ZCL_ProcessGetBlockThresholdsReq(&replyAddrInfo, pGetBlockThresholds);
    break;
#endif
    
  case gZclCmdPrice_PriceAck_c:
    status = gZclSuccess_c;
    break;
    
#if gASL_ZclPrice_Optionals_d     
  case gZclCmdPrice_GetBlockPeriods_c:
    pGetBlockPeriods = (zclCmdPrice_GetBlockPeriodsReq_t *)((uint8_t*)pIndication->pAsdu + sizeof(zclFrame_t));
    status = ZCL_ProcessGetBlockPeriodsReq(&replyAddrInfo, pGetBlockPeriods);
    break;
#endif

#if gASL_ZclPrice_Optionals_d && gASL_ZclSE_Use_NextGenFeatures_d    
  case gZclCmdPrice_GetConversionFactor_c:
    pGetConversionFactor = (zclCmdPrice_GetConversionFactorReq_t *)((uint8_t*)pIndication->pAsdu + sizeof(zclFrame_t));
    status = ZCL_ProcessGetConversioFactorReq(&replyAddrInfo, pGetConversionFactor);
    break;
  
  case gZclCmdPrice_GetCalorificValue_c:
    pGetCalorificValue = (zclCmdPrice_GetCalorificValueReq_t *)((uint8_t*)pIndication->pAsdu + sizeof(zclFrame_t));
    status = ZCL_ProcessGetCalorificValueReq(&replyAddrInfo, pGetCalorificValue);
    break;
#endif
    
    
  default:
    status = gZclUnsupportedClusterCommand_c;
    break;
    
  }
  
  return status;
}
  
/*****************************************************************************/

zbStatus_t ZCL_PriceClusterClient
(
     zbApsdeDataIndication_t *pIndication, /* IN: */
     afDeviceDef_t *pDev                /* IN: */
) 
{
  zclCmd_t Cmd;
  zbStatus_t status;
  
  (void)pDev;
  Cmd = (((zclFrame_t *)pIndication->pAsdu)->command);
  switch(Cmd) {
    case gZclCmdPrice_PublishPriceRsp_c:
      status =  ZCL_ProcessClientPublishPrice((zclCmdPrice_PublishPriceRsp_t *)(((uint8_t *)pIndication->pAsdu
                                                                                 + sizeof(zclFrame_t))));
      if(status == gZclSuccess_c)
      {//verify if PriceAck is required
        if(((((zclCmdPrice_PublishPriceRsp_t *)(((uint8_t *)pIndication->pAsdu + sizeof(zclFrame_t))))->PriceControl)
            & 0x01)==0x01)
        {
          status =  ZCL_SendPriceAck(pIndication);
        }
      }
      status = gZclSuccess_c;
      break;
#if gASL_ZclPrice_Optionals_d
    case gZclCmdPrice_PublishBlockPeriodRsp_c:
      status =  ZCL_ProcessClientPublishBlockPeriod((zclCmdPrice_PublishBlockPeriodRsp_t *)(((uint8_t *)pIndication->pAsdu
                                                                                 + sizeof(zclFrame_t))));
      if(status == gZclSuccess_c)
      {//verify if PriceAck is required
        if(((((zclCmdPrice_PublishBlockPeriodRsp_t *)(((uint8_t *)pIndication->pAsdu + sizeof(zclFrame_t))))->BlockPeriodControl)
            & 0x01)==0x01)
        {
          status =  ZCL_SendPriceAck(pIndication);
        }
      }
      status = gZclSuccess_c;
      break;
#endif
#if gASL_ZclPrice_Optionals_d &&  gASL_ZclSE_Use_NextGenFeatures_d     
    case gZclCmdPrice_PublishConversionFactorRsp_c:
      status = ZCL_ProcessClientPublishConversionFactor((zclCmdPrice_PublishConversionFactorRsp_t *)(((uint8_t *)pIndication->pAsdu
                                                                                 + sizeof(zclFrame_t))));
      break;
    
    case gZclCmdPrice_PublishCalorificValueRsp_c:
      status = ZCL_ProcessClientPublishCalorificValue((zclCmdPrice_PublishCalorificValueRsp_t *)(((uint8_t *)pIndication->pAsdu
                                                                                 + sizeof(zclFrame_t))));
      break;
#endif
      
#if gASL_ZclSE_Use_NextGenFeatures_d
  case gZclCmdPrice_PublishCO2ValueRsp_c:
    status = ZCL_ProcessClientPublishCO2Value((zclCmdPrice_PublishCO2ValueRsp_t *)(((uint8_t *)pIndication->pAsdu
                                                                                 + sizeof(zclFrame_t))));
    break;
#endif
    
#if gASL_ZclSE_Use_NextGenFeatures_d
  case gZclCmdPrice_PublishTariffInformationRsp_c:
    status = ZCL_ProcessClientPublishTariffInformation((zclCmdPrice_PublishTariffInformationRsp_t *)(((uint8_t *)pIndication->pAsdu
                                                                                 + sizeof(zclFrame_t))));
    break;
#endif
    
#if gASL_ZclSE_Use_NextGenFeatures_d
  case gZclCmdPrice_PublishPriceMatrixRsp_c:
    status = ZCL_ProcessClientPublishPriceMatrix((zclCmdPrice_PublishPriceMatrixRsp_t *)(((uint8_t *)pIndication->pAsdu
                                                                                 + sizeof(zclFrame_t))));
    break;
#endif
    
#if gASL_ZclSE_Use_NextGenFeatures_d
  case gZclCmdPrice_PublishBlockThresholdsRsp_c:
    status = ZCL_ProcessClientPublishBlockThresholds((zclCmdPrice_PublishBlockThresholdsRsp_t *)(((uint8_t *)pIndication->pAsdu
                                                                                 + sizeof(zclFrame_t))));
    break;
#endif
    
#if gASL_ZclSE_Use_NextGenFeatures_d
  case gZclCmdPrice_PublishBillingPeriodRsp_c:
    status = ZCL_ProcessClientPublishBillingPeriod((zclCmdPrice_PublishBillingPeriodRsp_t *)(((uint8_t *)pIndication->pAsdu
                                                                                 + sizeof(zclFrame_t))));
    break;
#endif
    
#if gASL_ZclSE_Use_NextGenFeatures_d
  case gZclCmdPrice_PublishConsolidatedBillRsp_c:
    status = ZCL_ProcessClientPublishConsolidatedBill((zclCmdPrice_PublishConsolidatedBillRsp_t *)(((uint8_t *)pIndication->pAsdu
                                                                                 + sizeof(zclFrame_t))));
    break;
#endif
#if gASL_ZclSE_Use_NextGenFeatures_d
  case gZclCmdPrice_PublishCPPEventRsp_c:
    {
      afAddrInfo_t addrInfo;    
      status = ZCL_ProcessClientPublishCPPEvent((zclCmdPrice_PublishCPPEventRsp_t *)(((uint8_t *)pIndication->pAsdu
                                                                                 + sizeof(zclFrame_t))));
      /* prepare for response in the address info (back to sender) */
      AF_PrepareForReply(&addrInfo, pIndication);
      FLib_MemCpy(&PriceCPPEvtRspSourceAddr, &addrInfo, sizeof(afAddrInfo_t));
    }
    break;
#endif
    
    default: 
      status = gZclUnsupportedClusterCommand_c;
      break;
  }
  
  return status;
}

/*-------- Price cluster commands ---------*/
zbStatus_t zclPrice_GetCurrPriceReq
(
zclPrice_GetCurrPriceReq_t *pReq
)
{
  return ZCL_SendClientReqSeqPassed(gZclCmdPrice_GetCurrPriceReq_c, sizeof(zclCmdPrice_GetCurrPriceReq_t),(zclGenericReq_t *)pReq);	
}


zbStatus_t zclPrice_GetScheduledPricesReq
(
zclPrice_GetScheduledPricesReq_t *pReq
)
{ 
  
  return ZCL_SendClientReqSeqPassed(gZclCmdPrice_GetScheduledPricesReq_c, sizeof(zclCmdPrice_GetScheduledPricesReq_t),(zclGenericReq_t *)pReq);	
}

zbStatus_t zclPrice_GetBlockPeriodsReq
(
zclPrice_GetBlockPeriodsReq_t *pReq
)
{
  return ZCL_SendClientReqSeqPassed(gZclCmdPrice_GetBlockPeriods_c, sizeof(zclCmdPrice_GetBlockPeriodsReq_t),(zclGenericReq_t *)pReq);	
}

zbStatus_t zclPrice_PublishPriceRsp
(
zclPrice_PublishPriceRsp_t *pReq
)
{
  uint8_t *pSrc,*pDst;
  uint8_t length;
  /*NOTE - depending on whether the Rate Label has a fixed length this function needs to package
  the telegram.
  The Stucture allows for 12 bytes of text, which is a size of 12 including the length.(11 byte of data)     
  aka
  copy from currentTime to rest of telegram to the location of Ratelabel+sizeof(length)+length.
  
  */
  
  pSrc  = (uint8_t *) &pReq->cmdFrame.IssuerEvt[0];
  pDst  = &pReq->cmdFrame.RateLabel.aStr[pReq->cmdFrame.RateLabel.length];  
  /*1 is the length byte*/
  length = sizeof(zclCmdPrice_PublishPriceRsp_t) - (sizeof(ProviderID_t) + sizeof(uint8_t) + pReq->cmdFrame.RateLabel.length);  
  FLib_MemInPlaceCpy(pDst, pSrc, length);
  length = sizeof(zclCmdPrice_PublishPriceRsp_t) - (11-pReq->cmdFrame.RateLabel.length);
  
  return ZCL_SendServerRspSeqPassed(gZclCmdPrice_PublishPriceRsp_c, length,(zclGenericReq_t *)pReq);	
}


zbStatus_t zclPrice_PublishBlockPeriodRsp
(
zclPrice_PublishBlockPeriodRsp_t *pReq
)
{
  return ZCL_SendServerRspSeqPassed(gZclCmdPrice_PublishBlockPeriodRsp_c,  sizeof(zclCmdPrice_PublishBlockPeriodRsp_t),(zclGenericReq_t *)pReq);	
}

zbStatus_t zclPrice_PublishConversionFactorRsp
(
zclPrice_PublishConversionFactorRsp_t *pReq
)
{
  return ZCL_SendServerRspSeqPassed(gZclCmdPrice_PublishConversionFactorRsp_c,  sizeof(zclCmdPrice_PublishConversionFactorRsp_t),(zclGenericReq_t *)pReq);	
}

zbStatus_t zclPrice_PublishCalorificValueRsp
(
zclPrice_PublishCalorificValueRsp_t *pReq
)
{
  return ZCL_SendServerRspSeqPassed(gZclCmdPrice_PublishCalorificValueRsp_c,  sizeof(zclCmdPrice_PublishCalorificValueRsp_t),(zclGenericReq_t *)pReq);	
}

zbStatus_t zclPrice_GetCalorificValueReq
(
zclPrice_GetCalorificValueReq_t *pReq
)
{
  return ZCL_SendClientReqSeqPassed(gZclCmdPrice_GetCalorificValue_c,  sizeof(zclCmdPrice_GetCalorificValueReq_t),(zclGenericReq_t *)pReq);	
}

zbStatus_t zclPrice_GetConversionFactorReq
(
zclPrice_GetConversionFactorReq_t *pReq
)
{
  return ZCL_SendClientReqSeqPassed(gZclCmdPrice_GetConversionFactor_c,  sizeof(zclCmdPrice_GetConversionFactorReq_t),(zclGenericReq_t *)pReq);	
}

zbStatus_t zclPrice_PublishTariffInformationRsp
(
zclPrice_PublishTariffInformationRsp_t *pReq
)
{
  uint8_t *pSrc,*pDst;
  uint8_t length;
  /*NOTE - depending on whether the Rate Label has a fixed length this function needs to package
  the telegram.
  The Stucture allows for 12 bytes of text, which is a size of 12 including the length.(11 byte of data)     
  aka
  copy from currentTime to rest of telegram to the location of Ratelabel+sizeof(length)+length.
  
  */
  
  pSrc  = (uint8_t *) &pReq->cmdFrame.NumberPriceTiersUse;
  pDst  = &pReq->cmdFrame.TariffLabel.aStr[pReq->cmdFrame.TariffLabel.length];  
  /*1 is the length byte*/
  length = sizeof(zclCmdPrice_PublishTariffInformationRsp_t) - MbrOfs(zclCmdPrice_PublishTariffInformationRsp_t, TariffLabel)
          - sizeof(uint8_t) - pReq->cmdFrame.TariffLabel.length;  
  FLib_MemInPlaceCpy(pDst, pSrc, length);
  length = sizeof(zclCmdPrice_PublishTariffInformationRsp_t) - (11-pReq->cmdFrame.TariffLabel.length);
  
  return ZCL_SendServerRspSeqPassed(gZclCmdPrice_PublishTariffInformationRsp_c, length,(zclGenericReq_t *)pReq);	
}

zbStatus_t zclPrice_GetBillingPeriodReq
(
zclPrice_GetBillingPeriodReq_t *pReq
)
{
  return ZCL_SendClientReqSeqPassed(gZclCmdPrice_GetBillingPeriod_c, sizeof(zclCmdPrice_GetBillingPeriodReq_t),(zclGenericReq_t *)pReq);	
}

zbStatus_t zclPrice_PublishBillingPeriodRsp
(
zclPrice_PublishBillingPeriodRsp_t *pReq
)
{
  return ZCL_SendServerRspSeqPassed(gZclCmdPrice_PublishBillingPeriodRsp_c,  sizeof(zclCmdPrice_PublishBillingPeriodRsp_t),(zclGenericReq_t *)pReq);	
}

zbStatus_t zclPrice_GetPriceMatrixReq
(
zclPrice_GetPriceMatrixReq_t *pReq
)
{
  return ZCL_SendClientReqSeqPassed(gZclCmdPrice_GetPriceMatrix_c, sizeof(zclCmdPrice_GetPriceMatrixReq_t),(zclGenericReq_t *)pReq);	
}

#if gASL_ZclSE_Use_NextGenFeatures_d
zbStatus_t zclPrice_PublishPriceMatrixRsp
(
zclPrice_PublishPriceMatrixRsp_t *pReq
)
{
  return ZCL_SendServerRspSeqPassed(gZclCmdPrice_PublishPriceMatrixRsp_c,  sizeof(zclCmdPrice_PublishPriceMatrixRsp_t) + 
                                    (mGetPriceMatrixReqEvtsIdx.Length - 1) * sizeof(Price_t) ,(zclGenericReq_t *)pReq);	
}
#endif

zbStatus_t zclPrice_GetBlockThresholdsReq
(
zclPrice_GetBlockThresholdsReq_t *pReq
)
{
  return ZCL_SendClientReqSeqPassed(gZclCmdPrice_GetBlockThresholds_c, sizeof(zclCmdPrice_GetBlockThresholdsReq_t),(zclGenericReq_t *)pReq);	
}

#if gASL_ZclSE_Use_NextGenFeatures_d
zbStatus_t zclPrice_PublishBlockThresholdsRsp
(
zclPrice_PublishBlockThresholdsRsp_t *pReq
)
{
  return ZCL_SendServerRspSeqPassed(gZclCmdPrice_PublishBlockThresholdsRsp_c,  sizeof(zclCmdPrice_PublishBlockThresholdsRsp_t) + 
                                    (mGetBlockThresholdsReqEvtsIdx.Length - 1) * sizeof(BlockThreshold_t) ,(zclGenericReq_t *)pReq);	
}
#endif

zbStatus_t zclPrice_GetCO2ValueReq
(
zclPrice_GetCO2ValueReq_t *pReq
)
{
  return ZCL_SendClientReqSeqPassed(gZclCmdPrice_GetCO2Value_c, sizeof(zclCmdPrice_GetCO2ValueReq_t),(zclGenericReq_t *)pReq);	
}

zbStatus_t zclPrice_PublishCO2ValueRsp
(
zclPrice_PublishCO2ValueRsp_t *pReq
)
{
  return ZCL_SendServerRspSeqPassed(gZclCmdPrice_PublishCO2ValueRsp_c,  sizeof(zclCmdPrice_PublishCO2ValueRsp_t),(zclGenericReq_t *)pReq);	
}

zbStatus_t zclPrice_GetTariffInformationReq
(
zclPrice_GetTariffInformationReq_t *pReq
)
{
  return ZCL_SendClientReqSeqPassed(gZclCmdPrice_GetTariffInformation_c, sizeof(zclCmdPrice_GetTariffInformationReq_t),(zclGenericReq_t *)pReq);	
}

zbStatus_t zclPrice_GetConsolidatedBillReq
(
zclPrice_GetConsolidatedBillReq_t *pReq
)
{
  return ZCL_SendClientReqSeqPassed(gZclCmdPrice_GetConsolidatedBill_c, sizeof(zclCmdPrice_GetConsolidatedBillReq_t),(zclGenericReq_t *)pReq);	
}

zbStatus_t zclPrice_PublishConsolidatedBillRsp
(
zclPrice_PublishConsolidatedBillRsp_t *pReq
)
{
  return ZCL_SendServerRspSeqPassed(gZclCmdPrice_PublishConsolidatedBillRsp_c,  sizeof(zclCmdPrice_PublishConsolidatedBillRsp_t),(zclGenericReq_t *)pReq);	
}

zbStatus_t zclPrice_CPPEventRsp
(
zclPrice_CPPEventRsp_t *pRsp
)
{
  return ZCL_SendClientReqSeqPassed(gZclCmdPrice_GetCPPEventResponse_c, sizeof(zclCmdPrice_CPPEventRsp_t),(zclGenericReq_t *)pRsp);	
}

zbStatus_t zclPrice_PublishCPPEventRsp
(
zclPrice_PublishCPPEventRsp_t *pReq
)
{
  return ZCL_SendServerRspSeqPassed(gZclCmdPrice_PublishCPPEventRsp_c,  sizeof(zclCmdPrice_PublishCPPEventRsp_t),(zclGenericReq_t *)pReq);	
}

#if gInterPanCommunicationEnabled_c

zbStatus_t zclPrice_InterPanGetCurrPriceReq
(
zclPrice_InterPanGetCurrPriceReq_t *pReq
)
{
  
  return ZCL_SendInterPanClientReqSeqPassed(gZclCmdPrice_GetCurrPriceReq_c, sizeof(zclCmdPrice_GetCurrPriceReq_t), pReq);	
}


zbStatus_t zclPrice_InterPanGetScheduledPricesReq
(
zclPrice_InterPanGetScheduledPricesReq_t *pReq
)
{ 
  
  return ZCL_SendInterPanClientReqSeqPassed(gZclCmdPrice_GetScheduledPricesReq_c, sizeof(zclCmdPrice_GetScheduledPricesReq_t), pReq);	
}


zbStatus_t zclPrice_InterPanPublishPriceRsp
(
zclPrice_InterPanPublishPriceRsp_t *pReq
)
{
  uint8_t *pSrc,*pDst;
  uint8_t length;
  /*NOTE - depending on whether the Rate Label has a fixed length this function needs to package
  the telegram.
  The Stucture allows for 12 bytes of text, which is a size of 12 including the length.(11 byte of data)     
  aka
  copy from currentTime to rest of telegram to the location of Ratelabel+sizeof(length)+length.
  
  */
  pSrc  = (uint8_t *) &pReq->cmdFrame.IssuerEvt[0];
  pDst  = &pReq->cmdFrame.RateLabel.aStr[pReq->cmdFrame.RateLabel.length];  
  /*1 is the length byte*/
  length = sizeof(zclCmdPrice_PublishPriceRsp_t) - (sizeof(ProviderID_t) + sizeof(uint8_t) + pReq->cmdFrame.RateLabel.length);  
  FLib_MemInPlaceCpy(pDst, pSrc, length);
  length = sizeof(zclCmdPrice_PublishPriceRsp_t) - (11-pReq->cmdFrame.RateLabel.length);
  return ZCL_SendInterPanServerRspSeqPassed(gZclCmdPrice_PublishPriceRsp_c, length, pReq);
}

#endif /* #if gInterPanCommunicationEnabled_c */
