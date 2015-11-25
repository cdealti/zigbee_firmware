/******************************************************************************
* ZclOptions.h
*
* Include this file after BeeOptions.h
*
* (c) Copyright 2008, Freescale, Inc.  All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from  Freescale Semiconductor.
*
* Documents used in this specification:
* [R1] - 053520r16ZB_HA_PTG-Home-Automation-Profile.pdf
* [R2] - 075123r00ZB_AFG-ZigBee_Cluster_Library_Specification.pdf
******************************************************************************/
#ifndef _ZCLOPTIONS_H
#define _ZCLOPTIONS_H

/* default 16 scenes per device */
#ifndef gHaMaxScenes_c
#define gHaMaxScenes_c    2
#endif

/* The maximum size in bytes for the storable scene */
/* The Scene data array:
              -> For the OnOff scenes it only needs 1 byte (0x01).
              -> For the Dimmer light it only needs 11 bytes (0x0B).
              -> For the Thermostat it needs the gHaMaxSceneSize_c
                 which is 45 bytes (0x2D).
*/
#ifndef gHaMaxSceneSize_c
#define gHaMaxSceneSize_c    45
#endif

/* Enable optionals to enable all optional clusters/attributes */
#ifndef gZclClusterOptionals_d
#define gZclClusterOptionals_d    FALSE
#endif

/* Enable to include scene name in scene table */
#ifndef gZclIncludeSceneName_d
#define gZclIncludeSceneName_d    FALSE
#endif

/* Foundation commands */


/* enable reporting of attributes (saves about 1.4K ROM) */
#ifndef gZclEnableReporting_c
#define gZclEnableReporting_c     FALSE
#endif

/* enable long attr types Reporting */
#ifndef gZclEnableOver32BitAttrsReporting_c
#define gZclEnableOver32BitAttrsReporting_c     FALSE
#endif

/* enable sending configure reporting from ztc */
#ifndef gASL_ZclZtcConfigureReporting_d
#define gASL_ZclZtcConfigureReporting_d     TRUE
#endif
/* enable long string types handling */
#ifndef gZclEnableLongStringTypes_c
#define gZclEnableLongStringTypes_c     FALSE
#endif


/* enable Discover Attributes request */
#ifndef gZclDiscoverAttrReq_d
#define gZclDiscoverAttrReq_d   FALSE
#endif

/* enable reply to Discover Attributes */
#ifndef gZclDiscoverAttrRsp_d
#define gZclDiscoverAttrRsp_d   FALSE
#endif

/* enable for Partition Cluster */
#ifndef gZclEnablePartition_d
#define gZclEnablePartition_d    FALSE
#endif


/* enable for Over the Air  (OTA)  Upgrade Cluster Server */
#ifndef gZclEnableOTAServer_d
#define gZclEnableOTAServer_d    FALSE
#endif

/* enable Metering Cluster Server */
#ifndef gZclEnableMeteringServer_d
#define gZclEnableMeteringServer_d    TRUE
#endif

/* enable for Over the Air  (OTA)  Upgrade Cluster Client */
#ifndef gZclEnableOTAClient_d
#define gZclEnableOTAClient_d    FALSE
#endif
/* enable Discovery Server Process after ota client device joined */
#ifndef gZclOTADiscoveryServerProcess_d
#define gZclOTADiscoveryServerProcess_d    TRUE
#endif

/* Upgrade Image On Current Device */
#ifndef gUpgradeImageOnCurrentDevice_d
 #define gUpgradeImageOnCurrentDevice_d        FALSE   
#endif

/* enable only for client application the LEDs progress display */
#if (gZclEnableOTAClient_d==TRUE)
#define gZclEnableOTAProgressReport_d    TRUE
#else
#define gZclEnableOTAProgressReport_d    FALSE
#endif

/*enable ota progress report by sending data to external device/App*/
#ifndef gZclEnableOTAProgressReportToExternalApp_d
#define gZclEnableOTAProgressReportToExternalApp_d    TRUE
#endif

/*enable ECDSA Signature Procedure for Client OTA. 
 * If this define is enabled it is necessary to have an ECC library*/
#ifndef gZclEnableOTAClientECCLibrary_d
#define gZclEnableOTAClientECCLibrary_d    FALSE
#endif

/*enable for Server OTA the possibility to use external memory in the OTA process*/
#ifndef gZclOtaExternalMemorySupported_d
#define gZclOtaExternalMemorySupported_d    FALSE
#endif

/*enable for Binary Input Cluster*/
#ifndef gZclEnableBinaryInput_c
#define gZclEnableBinaryInput_c  FALSE
#endif

/*enable for Occupancy Sensor Cluster*/
#ifndef gZclEnableOccupancySensor_c
#define gZclEnableOccupancySensor_c   FALSE
#endif

/*enable for Thermostat Cluster*/
#ifndef gZclEnableThermostat_c 
#define gZclEnableThermostat_c  FALSE
#endif




/***************************************
  Simple Metering Cluster
****************************************/

#ifndef gZclFastPollMode_d
#define gZclFastPollMode_d FALSE
#endif

#ifndef gZclFastPollUpdatePeriod_d
#define gZclFastPollUpdatePeriod_d 5
#endif

#ifndef gZclMirroring_d
#define gZclMirroring_d FALSE
#endif

#ifndef gZclMirroringDebug_d
#define gZclMirroringDebug_d FALSE
#endif

/***************************************
  Key Establishment Cluster
****************************************/

#ifndef gZclAcceptLongCert_d
#define gZclAcceptLongCert_d  TRUE
#endif

#ifndef gZclKeyEstabDebugMode_d
#define gZclKeyEstabDebugMode_d FALSE
#endif

#ifndef gZclKeyEstabDebugTimer_d
#define gZclKeyEstabDebugTimer_d FALSE
#endif

#ifndef gZclKeyEstabDebugTimeInterval_d
#define gZclKeyEstabDebugTimeInterval_d 240000
#endif

#ifndef gZclKeyEstabDebugInhibitInitRsp_d
#define gZclKeyEstabDebugInhibitInitRsp_d FALSE
#endif

#ifndef gZclSeSecurityCheck_d
#define gZclSeSecurityCheck_d FALSE
#endif
/***************************************
  Basic Cluster Defaults
****************************************/

/* Basic Cluster Attribute Defaults, for setting by the OEM */

/* SW version of the application (set by OEM) */
#ifndef gZclAttrBasic_ApplicationVersion_c
#define gZclAttrBasic_ApplicationVersion_c  0x01
#endif

/* HW version of the board (set by OEM) */
#ifndef gZclAttrBasic_HWVersion_c
#define gZclAttrBasic_HWVersion_c           0x03
#endif

/* Power Source - 0x01 = mains, see gZclAttrBasicPowerSource_Mains_c */
#ifndef gZclAttrBasic_PowerSource_c
#define gZclAttrBasic_PowerSource_c         0x01
#endif

/* up to 32 characters, manufacturers name */
#ifndef gszZclAttrBasic_MfgName_c           
#define gszZclAttrBasic_MfgName_c           "Freescale"
#endif

/* up to 32 characters, model ID (any set of characters) */
#ifndef gszZclAttrBasic_Model_c             
#define gszZclAttrBasic_Model_c             "0001"
#endif

/* up to 16 characters (date code, must be in yyyymmdd format */
#ifndef gszZclAttrBasic_DateCode_c          
#define gszZclAttrBasic_DateCode_c          "20070128"
#endif

/*--------------------------------------- ZCL Definitions ----------------------------------------*/

/* Identify Cluster */
#ifndef gASL_ZclIdentifyQueryReq_d
#define gASL_ZclIdentifyQueryReq_d    TRUE
#endif

#ifndef gASL_ZclIdentifyReq_d
#define gASL_ZclIdentifyReq_d   TRUE 
#endif

/* Groups Cluster */
#ifndef gASL_ZclGroupAddGroupReq_d
#define gASL_ZclGroupAddGroupReq_d    TRUE
#endif

#ifndef gASL_ZclGroupViewGroupReq_d
#define gASL_ZclGroupViewGroupReq_d    TRUE
#endif

#ifndef gASL_ZclGetGroupMembershipReq_d
#define gASL_ZclGetGroupMembershipReq_d    TRUE
#endif

#ifndef gASL_ZclRemoveGroupReq_d
#define gASL_ZclRemoveGroupReq_d    TRUE
#endif

#ifndef gASL_ZclRemoveAllGroupsReq_d
#define gASL_ZclRemoveAllGroupsReq_d    TRUE
#endif

#ifndef gASL_ZclStoreSceneReq_d
#define gASL_ZclStoreSceneReq_d    TRUE
#endif

#ifndef gASL_ZclRecallSceneReq_d
#define gASL_ZclRecallSceneReq_d    TRUE
#endif

#ifndef gASL_ZclGetSceneMembershipReq_d
#define gASL_ZclGetSceneMembershipReq_d    TRUE
#endif

#ifndef gASL_ZclOnOffReq_d
#define gASL_ZclOnOffReq_d    TRUE
#endif

/* Thermostat cluster commands */

#ifndef gASL_ZclSetWeeklyScheduleReq
#define gASL_ZclSetWeeklyScheduleReq    FALSE
#endif

#ifndef gASL_ZclGetWeeklyScheduleReq
#define gASL_ZclGetWeeklyScheduleReq    FALSE
#endif

#ifndef gASL_ZclClearWeeklyScheduleReq
#define gASL_ZclClearWeeklyScheduleReq    FALSE
#endif

#ifndef gASL_ZclSetpointRaiseLowerReq
#define gASL_ZclSetpointRaiseLowerReq    FALSE
#endif

#ifndef gASL_ZclDoorLockReq_d
#define gASL_ZclDoorLockReq_d    FALSE
#endif

#ifndef gASL_ZclLevelControlReq_d
#define gASL_ZclLevelControlReq_d    TRUE
#endif

#ifndef gASL_ZclSceneAddSceneReq_d
#define gASL_ZclSceneAddSceneReq_d    TRUE
#endif

#ifndef gASL_ZclViewSceneReq_d
#define gASL_ZclViewSceneReq_d    TRUE
#endif

#ifndef gASL_ZclRemoveSceneReq_d
#define gASL_ZclRemoveSceneReq_d    TRUE
#endif

#ifndef gASL_ZclRemoveAllScenesReq_d
#define gASL_ZclRemoveAllScenesReq_d    TRUE
#endif

#ifndef gASL_ZclStoreSceneHandler_d
#define gASL_ZclStoreSceneHandler_d    TRUE
#endif

#ifndef gASL_ZclAddGroupHandler_d
#define gASL_ZclAddGroupHandler_d    TRUE
#endif

#ifndef gASL_ZclCommissioningRestartDeviceRequest_d
#define gASL_ZclCommissioningRestartDeviceRequest_d FALSE
#endif

#ifndef gASL_ZclCommissioningSaveStartupParametersRequest_d
#define gASL_ZclCommissioningSaveStartupParametersRequest_d FALSE
#endif

#ifndef gASL_ZclCommissioningRestoreStartupParametersRequest_d
#define gASL_ZclCommissioningRestoreStartupParametersRequest_d FALSE
#endif

#ifndef gASL_ZclCommissioningResetStartupParametersRequest_d
#define gASL_ZclCommissioningResetStartupParametersRequest_d FALSE
#endif

/* Alarms cluster commands */

#ifndef gASL_ZclAlarms_ResetAlarm_d
#define gASL_ZclAlarms_ResetAlarm_d    FALSE
#endif

#ifndef gASL_ZclAlarms_ResetAllAlarms_d
#define gASL_ZclAlarms_ResetAllAlarms_d    FALSE
#endif

#ifndef gASL_ZclAlarms_Alarm_d
#define gASL_ZclAlarms_Alarm_d    FALSE
#endif

#ifndef gASL_ZclAlarms_GetAlarm_d
#define gASL_ZclAlarms_GetAlarm_d    FALSE
#endif

/* IAS Zone Cluster */
#ifndef gASL_ZclIASZoneReq_d
#define gASL_ZclIASZoneReq_d    FALSE
#endif

/* IAS ACE Cluster */
#ifndef gASL_ZclIASACEReq_d
#define gASL_ZclIASACEReq_d    FALSE
#endif

/* IAS WD Cluster */
#ifndef gASL_ZclIASWDReq_d
#define gASL_ZclIASWDReq_d    FALSE
#endif

/* SE commands and responses */

#ifndef gASL_ZclDisplayMsgReq_d
#define gASL_ZclDisplayMsgReq_d FALSE
#endif

#ifndef gASL_ZclCancelMsgReq_d
#define gASL_ZclCancelMsgReq_d FALSE
#endif

#ifndef gASL_ZclGetLastMsgReq_d
#define gASL_ZclGetLastMsgReq_d FALSE
#endif

#ifndef gASL_ZclMsgConfReq_d
#define gASL_ZclMsgConfReq_d FALSE
#endif

#ifndef gASL_ZclInterPanDisplayMsgReq_d
#define gASL_ZclInterPanDisplayMsgReq_d FALSE
#endif

#ifndef gASL_ZclInterPanCancelMsgReq_d
#define gASL_ZclInterPanCancelMsgReq_d FALSE
#endif

#ifndef gASL_ZclInterPanGetLastMsgReq_d
#define gASL_ZclInterPanGetLastMsgReq_d FALSE
#endif

#ifndef gASL_ZclInterPanMsgConfReq_d
#define gASL_ZclInterPanMsgConfReq_d FALSE
#endif

#ifndef gASL_ZclSmplMet_GetProfReq_d
#define gASL_ZclSmplMet_GetProfReq_d FALSE
#endif

#ifndef gASL_ZclSmplMet_GetProfRsp_d
#define gASL_ZclSmplMet_GetProfRsp_d FALSE
#endif

#ifndef gASL_ZclSmplMet_FastPollModeReq_d
#define gASL_ZclSmplMet_FastPollModeReq_d FALSE
#endif

#ifndef gASL_ZclSmplMet_FastPollModeRsp_d
#define gASL_ZclSmplMet_FastPollModeRsp_d FALSE
#endif

#ifndef gASL_ZclSmplMet_AcceptFastPollModeReq_d
#define gASL_ZclSmplMet_AcceptFastPollModeReq_d FALSE
#endif

#ifndef gASL_ZclSmplMet_GetSnapshotReq_d
#define gASL_ZclSmplMet_GetSnapshotReq_d FALSE
#endif

#ifndef gASL_ZclMet_RequestMirrorReq_d
#define gASL_ZclMet_RequestMirrorReq_d FALSE
#endif

#ifndef gASL_ZclMet_RemoveMirrorReq_d
#define gASL_ZclMet_RemoveMirrorReq_d FALSE
#endif

#ifndef gASL_ZclDmndRspLdCtrl_ReportEvtStatus_d
#define gASL_ZclDmndRspLdCtrl_ReportEvtStatus_d FALSE
#endif

#ifndef gASL_ZclDmndRspLdCtrl_GetScheduledEvtsReq_d
#define gASL_ZclDmndRspLdCtrl_GetScheduledEvtsReq_d FALSE
#endif

#ifndef gASL_ZclDmndRspLdCtrl_ScheduledServerEvts_d
#define gASL_ZclDmndRspLdCtrl_ScheduledServerEvts_d FALSE
#endif

#ifndef gASL_ZclDmndRspLdCtrl_LdCtrlEvtReq_d
#define gASL_ZclDmndRspLdCtrl_LdCtrlEvtReq_d FALSE
#endif

#ifndef gASL_ZclDmndRspLdCtrl_CancelLdCtrlEvtReq_d
#define gASL_ZclDmndRspLdCtrl_CancelLdCtrlEvtReq_d FALSE
#endif

#ifndef gASL_ZclDmndRspLdCtrl_CancelAllLdCtrlEvtsReq_d
#define gASL_ZclDmndRspLdCtrl_CancelAllLdCtrlEvtsReq_d FALSE
#endif

#ifndef gASL_ZclPrice_GetCurrPriceReq_d
#define gASL_ZclPrice_GetCurrPriceReq_d FALSE
#endif

#ifndef gASL_ZclPrice_GetSheduledPricesReq_d
#define gASL_ZclPrice_GetSheduledPricesReq_d FALSE
#endif

#ifndef gASL_ZclPrice_GetBlockPeriodsReq_d
#define gASL_ZclPrice_GetBlockPeriodsReq_d FALSE
#endif

#ifndef gASL_ZclPrice_GetPriceMatrixReq_d
#define gASL_ZclPrice_GetPriceMatrixReq_d FALSE
#endif

#ifndef gASL_ZclPrice_GetBlockThresholdsReq_d
#define gASL_ZclPrice_GetBlockThresholdsReq_d FALSE
#endif
#ifndef gASL_ZclPrice_GetConversionFactorReq_d
#define gASL_ZclPrice_GetConversionFactorReq_d FALSE
#endif

#ifndef gASL_ZclPrice_GetCO2ValueReq_d
#define gASL_ZclPrice_GetCO2ValueReq_d FALSE
#endif

#ifndef gASL_ZclPrice_PublishCO2ValueRsp_d
#define gASL_ZclPrice_PublishCO2ValueRsp_d FALSE
#endif

#ifndef gASL_ZclPrice_GetCalorificValueReq_d
#define gASL_ZclPrice_GetCalorificValueReq_d FALSE
#endif

#ifndef gASL_ZclPrice_GetTariffInformationReq_d
#define gASL_ZclPrice_GetTariffInformationReq_d FALSE
#endif

#ifndef gASL_ZclPrice_GetBillingPeriodReq_d
#define gASL_ZclPrice_GetBillingPeriodReq_d FALSE
#endif

#ifndef gASL_ZclPrice_GetConsolidatedBillReq_d
#define gASL_ZclPrice_GetConsolidatedBillReq_d FALSE
#endif

#ifndef gASL_ZclPrice_PublishConversionFactorRsp_d
#define gASL_ZclPrice_PublishConversionFactorRsp_d FALSE
#endif

#ifndef gASL_ZclPrice_PublishCO2ValueRsp_d
#define gASL_ZclPrice_PublishCO2ValueRsp_d FALSE
#endif

#ifndef gASL_ZclPrice_PublishPriceMatrixRsp_d
#define gASL_ZclPrice_PublishPriceMatrixRsp_d FALSE
#endif

#ifndef gASL_ZclPrice_PublishBlockThresholdsRsp_d
#define gASL_ZclPrice_PublishBlockThresholdsRsp_d FALSE
#endif

#ifndef gASL_ZclPrice_PublishCalorificValueRsp_d
#define gASL_ZclPrice_PublishCalorificValueRsp_d FALSE
#endif

#ifndef gASL_ZclPrice_PublishBlockPeriodsRsp_d
#define gASL_ZclPrice_PublishBlockPeriodsRsp_d FALSE
#endif

#ifndef gASL_ZclPrice_PublishBillingPeriodRsp_d
#define gASL_ZclPrice_PublishBillingPeriodRsp_d FALSE
#endif

#ifndef gASL_ZclPrice_PublishCPPEventRsp_d
#define gASL_ZclPrice_PublishCPPEventRsp_d FALSE
#endif
#ifndef gASL_ZclPrice_PublishTariffInformationRsp_d
#define gASL_ZclPrice_PublishTariffInformationRsp_d FALSE
#endif

#ifndef gASL_ZclPrice_PublishConsolidatedBillRsp_d
#define gASL_ZclPrice_PublishConsolidatedBillRsp_d FALSE
#endif

#ifndef gASL_ZclPrice_PublishPriceRsp_d
#define gASL_ZclPrice_PublishPriceRsp_d FALSE
#endif

#ifndef gASL_ZclPrice_InterPanGetCurrPriceReq_d
#define gASL_ZclPrice_InterPanGetCurrPriceReq_d FALSE
#endif

#ifndef gASL_ZclPrice_InterPanGetSheduledPricesReq_d
#define gASL_ZclPrice_InterPanGetSheduledPricesReq_d FALSE
#endif

#ifndef gASL_ZclPrice_InterPanPublishPriceRsp_d
#define gASL_ZclPrice_InterPanPublishPriceRsp_d FALSE
#endif

/* SE Tunneling requires fragmentation to be enabled (gFragmentationCapability_d TRUE) */
#ifndef gASL_ZclSETunneling_d
#define gASL_ZclSETunneling_d FALSE
#endif

#ifndef gASL_ZclSETunnelingOptionals_d
#define gASL_ZclSETunnelingOptionals_d FALSE
#endif

#ifndef gASL_ZclZtcSETunnelingTesting_d
#define gASL_ZclZtcSETunnelingTesting_d FALSE
#endif

#define gASL_ZclZtcSETunnelingTestingBufferSize_c 128

#ifndef gASL_ZclPrepayment_d
#define gASL_ZclPrepayment_d FALSE
#endif

#ifndef gASL_ZclPrepayment_Optionals_d
#define gASL_ZclPrepayment_Optionals_d FALSE
#endif

#ifndef gASL_ZclPrepayment_TopUpOptionals_d
#define gASL_ZclPrepayment_TopUpOptionals_d FALSE
#endif

#ifndef gASL_ZclPrepayment_DebtOptionals_d
#define gASL_ZclPrepayment_DebtOptionals_d FALSE
#endif

#ifndef gASL_ZclPrepayment_SupplyOptionals_d
#define gASL_ZclPrepayment_SupplyOptionals_d FALSE
#endif
#ifndef gASL_ZclPrepayment_SelAvailEmergCreditReq_d
#define gASL_ZclPrepayment_SelAvailEmergCreditReq_d FALSE
#endif

#ifndef gASL_ZclPrepayment_ChangeSupplyReq_d
#define gASL_ZclPrepayment_ChangeSupplyReq_d FALSE
#endif

#ifndef gASL_ZclKeyEstab_InitKeyEstabReq_d
#define gASL_ZclKeyEstab_InitKeyEstabReq_d FALSE
#endif

#ifndef gASL_ZclKeyEstab_EphemeralDataReq_d
#define gASL_ZclKeyEstab_EphemeralDataReq_d FALSE
#endif

#ifndef gASL_ZclKeyEstab_ConfirmKeyDataReq_d
#define gASL_ZclKeyEstab_ConfirmKeyDataReq_d FALSE
#endif

#ifndef gASL_ZclKeyEstab_TerminateKeyEstabServer_d
#define gASL_ZclKeyEstab_TerminateKeyEstabServer_d FALSE
#endif

#ifndef gASL_ZclKeyEstab_InitKeyEstabRsp_d
#define gASL_ZclKeyEstab_InitKeyEstabRsp_d FALSE
#endif

#ifndef gASL_ZclKeyEstab_EphemeralDataRsp_d
#define gASL_ZclKeyEstab_EphemeralDataRsp_d FALSE
#endif

#ifndef gASL_ZclKeyEstab_ConfirmKeyDataRsp_d
#define gASL_ZclKeyEstab_ConfirmKeyDataRsp_d FALSE
#endif

#ifndef gASL_ZclKeyEstab_TerminateKeyEstabClient_d
#define gASL_ZclKeyEstab_TerminateKeyEstabClient_d FALSE
#endif

#ifndef gASL_ZclSE_RegisterDevice_d
#define gASL_ZclSE_RegisterDevice_d FALSE
#endif

#ifndef gASL_ZclSE_DeRegisterDevice_d
#define gASL_ZclSE_DeRegisterDevice_d FALSE
#endif

#ifndef gASL_ZclSE_InitTime_d
#define gASL_ZclSE_InitTime_d FALSE
#endif

/*****************************
    Optional Attributes
*****************************/

#if !gZclClusterOptionals_d
#define gASL_ZclMet_Optionals_d FALSE
#define gASL_ZclPrice_Optionals_d FALSE
#endif 

/* Cluster Configuration */

#ifndef gASL_ZclMet_Optionals_d
#define gASL_ZclMet_Optionals_d FALSE
#endif

/* Metering Block Information Attribute Set parameters */

#ifndef gASL_ZclMet_BISSetNumberOfTiersDlvr_d
#define gASL_ZclMet_BISSetNumberOfTiersDlvr_d   5
#endif

#ifndef gASL_ZclMet_BISSetNumberOfBlocksDlvr_d
#define gASL_ZclMet_BISSetNumberOfBlocksDlvr_d   6
#endif

#ifndef gASL_ZclMet_BISSetNumberOfTiersRcvd_d
#define gASL_ZclMet_BISSetNumberOfTiersRcvd_d   5
#endif

#ifndef gASL_ZclMet_BISSetNumberOfBlocksRcvd_d
#define gASL_ZclMet_BISSetNumberOfBlocksRcvd_d   6
#endif

#ifndef gASL_ZclPrice_Optionals_d
#define gASL_ZclPrice_Optionals_d FALSE
#endif

#ifndef gASL_ZclPrepayment_Optionals_d
#define gASL_ZclPrepayment_Optionals_d FALSE
#endif

#ifndef gASL_ZclSE_Use_NextGenFeatures_d
#define gASL_ZclSE_Use_NextGenFeatures_d    FALSE
#endif

/* Cluster Sets Configuration */

#ifndef gASL_ZclSE_TiersNumber_d
#define gASL_ZclSE_TiersNumber_d 5//max 15
#endif

#ifndef gASL_ZclSE_ExtendedPriceTiersNumber_d
#define gASL_ZclSE_ExtendedPriceTiersNumber_d 0//max 32
#endif

#ifndef gASL_ZclSE_BlocksNumber_d
#define gASL_ZclSE_BlocksNumber_d 6//max 16
#endif

#ifndef gASL_ZclPrice_BlockThresholdNumber_d
#define gASL_ZclPrice_BlockThresholdNumber_d 5//max 15
#endif

#ifndef gASL_ZclPrice_BlockPeriodNumber_d
#define gASL_ZclPrice_BlockPeriodNumber_d 1//max 4
#endif

#ifndef gASL_ZclPrice_BlockPriceInfoNumber_d
#define gASL_ZclPrice_BlockPriceInfoNumber_d gASL_ZclSE_BlocksNumber_d * (gASL_ZclSE_TiersNumber_d + 1) + gASL_ZclSE_ExtendedPriceTiersNumber_d
#endif

#ifndef gASL_ZclPrice_CommodityTypeNumber_d
#define gASL_ZclPrice_CommodityTypeNumber_d 7//max7
#endif
/***************************************
  Define the set of clusters used by each device
****************************************/

extern zbApsTxOption_t const gZclTxOptions;

#endif /* _ZCLOPTIONS_H */

