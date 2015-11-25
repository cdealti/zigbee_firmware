/******************************************************************************
* ZclSEPrice.h
*
* Types, definitions and prototypes for the ZigBee SE Price Cluster implementation.
*
* Copyright (c) 2012, Freescale, Inc.  All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from  Freescale Semiconductor.
*
******************************************************************************/
#ifndef _ZCLSEPRICE_H
#define _ZCLSEPRICE_H

/* header files needed by home automation */
#include "EmbeddedTypes.h"
#include "AfApsInterface.h"
#include "ZCL.h"
#include "SEProfile.h"
/******************************************************************************
*******************************************************************************
* Public macros and data types definitions.
*******************************************************************************
******************************************************************************/
/*Note all these variables should be allocted in msg system*/
/*total size of memory needed : 
42 22 21 22 21 16 16 22 2 = 205 bytes.
*/

zbStatus_t ZCL_PriceClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev);
zbStatus_t ZCL_PriceClusterClient(zbApsdeDataIndication_t *pIndication,	afDeviceDef_t *pDev );
zbStatus_t zclPrice_GetCurrPriceReq(zclPrice_GetCurrPriceReq_t *pReq);
zbStatus_t zclPrice_GetScheduledPricesReq(zclPrice_GetScheduledPricesReq_t *pReq);
zbStatus_t zclPrice_GetBlockPeriodsReq(zclPrice_GetBlockPeriodsReq_t *pReq);
zbStatus_t zclPrice_GetBillingPeriodReq(zclPrice_GetBillingPeriodReq_t *pReq);
zbStatus_t zclPrice_GetPriceMatrixReq(zclPrice_GetPriceMatrixReq_t *pReq);
zbStatus_t zclPrice_GetBlockThresholdsReq(zclPrice_GetBlockThresholdsReq_t *pReq);
zbStatus_t zclPrice_GetCO2ValueReq(zclPrice_GetCO2ValueReq_t *pReq);
zbStatus_t zclPrice_GetTariffInformationReq(zclPrice_GetTariffInformationReq_t *pReq);
zbStatus_t zclPrice_GetConsolidatedBillReq(zclPrice_GetConsolidatedBillReq_t *pReq);
zbStatus_t zclPrice_CPPEventRsp(zclPrice_CPPEventRsp_t *pReq);
zbStatus_t zclPrice_GetCalorificValueReq(zclPrice_GetCalorificValueReq_t *pReq);
zbStatus_t zclPrice_GetConversionFactorReq(zclPrice_GetConversionFactorReq_t *pReq);
zbStatus_t zclPrice_PublishPriceRsp(zclPrice_PublishPriceRsp_t *pReq);
zbStatus_t zclPrice_PublishBlockPeriodRsp(zclPrice_PublishBlockPeriodRsp_t *pReq);
zbStatus_t zclPrice_PublishConversionFactorRsp(zclPrice_PublishConversionFactorRsp_t *pReq);
zbStatus_t zclPrice_PublishCalorificValueRsp(zclPrice_PublishCalorificValueRsp_t *pReq);
zbStatus_t zclPrice_PublishBillingPeriodRsp(zclPrice_PublishBillingPeriodRsp_t *pReq);
zbStatus_t zclPrice_PublishPriceMatrixRsp(zclPrice_PublishPriceMatrixRsp_t *pReq);
zbStatus_t zclPrice_PublishBlockThresholdsRsp(zclPrice_PublishBlockThresholdsRsp_t *pReq);
zbStatus_t zclPrice_PublishCO2ValueRsp(zclPrice_PublishCO2ValueRsp_t *pReq);
zbStatus_t zclPrice_PublishTariffInformationRsp(zclPrice_PublishTariffInformationRsp_t *pReq);
zbStatus_t zclPrice_PublishConsolidatedBillRsp(zclPrice_PublishConsolidatedBillRsp_t *pReq);
zbStatus_t zclPrice_PublishCPPEventRsp(zclPrice_PublishCPPEventRsp_t *pReq);
zbStatus_t zclPrice_InterPanGetCurrPriceReq(zclPrice_InterPanGetCurrPriceReq_t *pReq);
zbStatus_t zclPrice_InterPanGetScheduledPricesReq(zclPrice_InterPanGetScheduledPricesReq_t *pReq);
zbStatus_t zclPrice_InterPanPublishPriceRsp(zclPrice_InterPanPublishPriceRsp_t *pReq);
uint8_t FindServerTariffInformationEntryByIssuerId(SEEvtId_t *pIssuerId);
uint8_t FindClientTariffInformationEntryByIssuerId(SEEvtId_t *pIssuerId);

static uint8_t AddBlockPeriodsInTable
(
publishBlockPeriodEntry_t *pTable,
uint8_t len,
zclCmdPrice_PublishBlockPeriodRsp_t *pMsg
);
static uint8_t AddTariffInformationInTable
(
publishTariffInformationEntry_t *pTable,
uint8_t len, 
zclCmdPrice_PublishTariffInformationRsp_t *pMsg
);
static uint8_t AddBillingPeriodsInTable
(
publishBillingPeriodEntry_t *pTable,
uint8_t len,
zclCmdPrice_PublishBillingPeriodRsp_t *pMsg
);

static uint8_t AddCO2ValueInTable
(
publishCO2ValueEntry_t *pTable,
uint8_t len,
zclCmdPrice_PublishCO2ValueRsp_t *pMsg
);

static uint8_t AddConsolidatedBillsInTable
(
publishConsolidatedBillEntry_t *pTable,
uint8_t len,
zclCmdPrice_PublishConsolidatedBillRsp_t *pMsg
);

static uint8_t AddCPPEventsInTable
(
  publishCPPEventEntry_t *pTable,
  uint8_t len,
  zclCmdPrice_PublishCPPEventRsp_t *pMsg
);

static uint8_t AddCalorificValueInTable
(
publishCalorificValueEntry_t *pTable,
uint8_t len,
zclCmdPrice_PublishCalorificValueRsp_t *pMsg
);
static uint8_t AddConversionFactorInTable
(
publishConversionFactorEntry_t *pTable,
uint8_t len,
zclCmdPrice_PublishConversionFactorRsp_t *pMsg
);

#if gASL_ZclPrice_Optionals_d
void ZCL_DeleteScheduleServerBlockPeriods(void);
zbStatus_t ZCL_ScheduleServerBlockPeriodsEvents(ztcCmdPrice_PublishBlockPeriodRsp_t *pMsg);
zbStatus_t ZCL_UpdateServerBlockPeriodEvents(zclCmdPrice_PublishBlockPeriodRsp_t *pMsg);
#endif
#if gASL_ZclPrice_Optionals_d && gASL_ZclSE_Use_NextGenFeatures_d
void ZCL_DeleteServerCalorificValueStore(void);
zbStatus_t ZCL_ScheduleServerCalorificValueStore ( ztcCmdPrice_PublishCalorificValueRsp_t *pMsg);
zbStatus_t ZCL_ScheduleServerConversionFactorStore ( ztcCmdPrice_PublishConversionFactorRsp_t *pMsg);
#endif
#if gASL_ZclSE_Use_NextGenFeatures_d 
zbStatus_t ZCL_ScheduleServerTariffInformationEvents ( ztcCmdPrice_PublishTariffInformationRsp_t *pMsg);
zbStatus_t ZCL_ScheduleServerBillingPeriodEvents(ztcCmdPrice_PublishBillingPeriodRsp_t *pMsg);
zbStatus_t ZCL_ScheduleServerCO2ValueEvents(ztcCmdPrice_PublishCO2ValueRsp_t *pMsg);
zbStatus_t ZCL_StoreServerPriceMatrix(ztcCmdPrice_PublishPriceMatrixRsp_t *pMsg);
zbStatus_t ZCL_StoreServerBlockThresholds(ztcCmdPrice_PublishBlockThresholdsRsp_t *pMsg);
zbStatus_t ZCL_ScheduleServerConsolidatedBillEvents(ztcCmdPrice_PublishConsolidatedBillRsp_t *pMsg);
zbStatus_t ZCL_ScheduleServerCPPEvents(zclCmdPrice_PublishCPPEventRsp_t *pMsg);
#endif

void ZCL_HandleSEPriceClusterEvt(void);
void SendPriceClusterEvt(uint32_t evtId);

void ZCL_PriceClientInit(void);
void ZCL_DeleteClientPrice(uint8_t *pEvtId);
zbStatus_t ZCL_ScheduleServerPriceEvents(ztcCmdPrice_PublishPriceRsp_t *pMsg);
zbStatus_t ZCL_UpdateServerPriceEvents(zclCmdPrice_PublishPriceRsp_t *pMsg);
void ZCL_DeleteServerScheduledPrices(void);
void ZCL_HandleClientPrices(void);
void ZCL_HandleClientBlockPeriod(void);
void ZCL_HandleConversionFactorUpdate(void);
void ZCL_HandleCalorificValueUpdate(void);
void ZCL_HandleGetScheduledPrices(void);
void ZCL_HandlePublishPriceUpdate(void);
void ZCL_HandleBlockPeriodUpdate(void);
void ZCL_HandleGetBlockPeriods(void);

void ZCL_HandleGetBillingPeriod(void);
void ZCL_HandleClientBillingPeriod(void);
void ZCL_HandleBillingPeriodUpdate(void);
void ZCL_HandleCPPEventUpdate(void);

void ZCL_HandleCO2ValueUpdate(void);
void ZCL_HandleTariffInformationUpdate(void);

void ZCL_HandleGetConsolidatedBill(void);
void ZCL_HandleClientConsolidatedBill(void);
void ZCL_HandleConsolidatedBillUpdate(void);

void ZCL_HandleGetConversionFactor(void);
void ZCL_HandleGetCalorificValue(void);

extern afAddrInfo_t *ZCL_GetPriceCPPEventSrcAddrForResponse(void);

extern publishPriceEntry_t gaClientPriceTable[gNumofClientPrices_c];
extern publishPriceEntry_t gaServerPriceTable[gNumofServerPrices_c];
extern publishCPPEventEntry_t gaClientCPPEventTable[gNumofClientCPPEvents_c];

#endif /* _ZCLSEPRICE_H */
