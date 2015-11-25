/******************************************************************************
* SEProfile.h
*
* Types, definitions and prototypes for the ZigBee SE Profile.
*
* Copyright (c) 2006, Freescale, Inc.  All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from  Freescale Semiconductor.
*
******************************************************************************/
/*
Acronyms:

DemandResponseLoadControl... DmndRspLdCtrl
LoadControl...LdCtrl
SimpleMetering...SmplMet
KeyEstablishmentSecuritySuite ...KeyEstabSecSuite
KeyEstablishment...KeyEstab
DeviceClass...DevCls
CriticalityLevel...CritLev
UtilityDefined...UtilDef
Criticality or Critical...Crit; 
Messaging or Message...Msg
CoolingTemperatureSetPointApplied... CoolTempSetPntApplied(HeatTempSetPntApplied)
Simple....Smpl;       Load...Ld;        Demand...Dmnd;          Device...Dev;
Level...Lev;          Response...Rsp;   Control.......Ctrl;     Received...Rcvd;
Event....Evt;         Rejected...Rjctd  Delivered...Dlvrd       Summation...Summ
Metering...Met        Profile...Prof    Partial...Prtl          Interval...Intrv
Undefined...Undef     Defined...Def     Consumption...Consmp    Current...Curr
Command...Cmd         Anonymous... Anon Confirmation...Conf;
Alternate..... Alt    Register....Rgstr Number....Num           Information...Info
*/

#ifndef _SEPROFILE_H
#define _SEPROFILE_H

/* header files needed by home automation */
#include "EmbeddedTypes.h"
#include "AfApsInterface.h"
#include "ZclOptions.h"
#include "ZCL.h"
#include "ZCLGeneral.h"
#include "ZclHVAC.h"
#include "ZCLSensing.h"

/******************************************************************************
*******************************************************************************
* Public macros & type definitions
*******************************************************************************
******************************************************************************/
/*Include if Certicom ECC library is included*/
#define gEccIncluded_d FALSE



/*Wait time set in terminate key establishment commands*/
#ifndef gKeyEstab_DefaultWaitTime_c
#define gKeyEstab_DefaultWaitTime_c 5
#endif 

#ifndef gKeyEstab_DefaultEphemeralDataGenerateTime_c
#define gKeyEstab_DefaultEphemeralDataGenerateTime_c 0x03
#endif

#ifndef gKeyEstab_DefaultConfirmKeyGenerateTime_c
#define gKeyEstab_DefaultConfirmKeyGenerateTime_c 0x03
#endif

#ifndef  gKeyEstab_ConfirmKeyMessageTimeout_c
#define gKeyEstab_ConfirmKeyMessageTimeout_c 3
#endif

#ifndef gKeyEstab_EphemeralDataMessageTimeout_c
#define gKeyEstab_EphemeralDataMessageTimeout_c 3
#endif

#ifndef gKeyEstab_MinimumTimeout_c
#define gKeyEstab_MinimumTimeout_c 0x05
#endif

/* The poll rate used by an end device during ECC procedure*/
#ifndef gKeyEstab_EndDevicePollRate_c
#define gKeyEstab_EndDevicePollRate_c 1000
#endif
/* Number of entries in  DRLC event Table*/
#ifndef gNumOfEventsTableEntry_c
#define gNumOfEventsTableEntry_c 4 
#endif 

/* Number of entries stored on the DRLC Server */
#ifndef gNumOfServerEventsTableEntry_c
#define gNumOfServerEventsTableEntry_c 2
#endif

/*Size of ESP registation table */
#ifndef RegistrationTableEntries_c
#define RegistrationTableEntries_c 2
#endif 

/* Number of InterPan addr used for Price Update(for Non-SE devices) */
#ifndef gNumOfInterPanAddr_c
#define gNumOfInterPanAddr_c 1
#endif
/* Number of addr information used for Price Update(for Se devices) */
#ifndef gNumOfPriceAddr_c
#define gNumOfPriceAddr_c 4
#endif

/* Number of Prices stored in the Server (min. value is 5) */
#ifndef gNumofServerPrices_c
#define gNumofServerPrices_c 2
#endif

/* Number of Prices Handled by the Client(min. value is 2) */
#ifndef gNumofClientPrices_c
#define gNumofClientPrices_c 2
#endif

/* Number of Prices stored in the Server (min. value is 2) */
#ifndef gNumofServerBlockPeriods_c
#define gNumofServerBlockPeriods_c 2
#endif

/* Number of Prices Handled by the Client(min. value is 2) */
#ifndef gNumofClientBlockPeriods_c
#define gNumofClientBlockPeriods_c 2
#endif

/* Number of Price Matrix stored in the Server (min. value is 2) */
#ifndef gNumofServerPriceMatrix_c
#define gNumofServerPriceMatrix_c 2
#endif

/* Number of Price Matrix Handled by the Client(min. value is 2) */
#ifndef gNumofClientPriceMatrix_c
#define gNumofClientPriceMatrix_c 2
#endif

/* Number of Block Thresholds stored in the Server (min. value is 2) */
#ifndef gNumofServerBlockThresholds_c
#define gNumofServerBlockThresholds_c 2
#endif

/* Number of Block Thresholds Handled by the Client(min. value is 2) */
#ifndef gNumofClientBlockThresholds_c
#define gNumofClientBlockThresholds_c 2
#endif

/* Number of Conversion Factor stored in the Server (min. value is 2) */
#ifndef gNumofServerConversionFactors_c
#define gNumofServerConversionFactors_c 2
#endif

/* Number of Conversion Factor Handled by the Client(min. value is 2) */
#ifndef gNumofClientConversionFactors_c
#define gNumofClientConversionFactors_c 2
#endif

/* Number of CO2 Value stored in the Server (min. value is 3) */
#ifndef gNumofServerCO2Value_c
#define gNumofServerCO2Value_c 3
#endif

/* Number of CO2 Value Handled by the Client(min. value is 2) */
#ifndef gNumofClientCO2Value_c
#define gNumofClientCO2Value_c 2
#endif

/* Number of Calorific Value stored in the Server (min. value is 2) */
#ifndef gNumofServerCalorificValue_c
#define gNumofServerCalorificValue_c 2
#endif

/* Number of Calorific Value Handled by the Client(min. value is 2) */
#ifndef gNumofClientCalorificValue_c
#define gNumofClientCalorificValue_c 2
#endif

/* Number of Tariff Information stored in the Server (min. value is 2) */
#ifndef gNumofServerTariffInformation_c
#define gNumofServerTariffInformation_c 2
#endif

/* Number of Tariff Information stored on the Client (min. value is 2) */
#ifndef gNumofClientTariffInformation_c
#define gNumofClientTariffInformation_c 2
#endif

/* Number of Billing Periods stored in the Server (min. value is 2) */
#ifndef gNumofServerBillingPeriods_c
#define gNumofServerBillingPeriods_c 2
#endif

/* Number of Billing Periods Handled by the Client(min. value is 2) */
#ifndef gNumofClientBillingPeriods_c
#define gNumofClientBillingPeriods_c 2
#endif

/* Number of CPP Events stored in the Server*/
#ifndef gNumofServerCPPEvents_c
#define gNumofServerCPPEvents_c 2
#endif

/* Number of CPP Events Handled by the Client */
#ifndef gNumofClientCPPEvents_c
#define gNumofClientCPPEvents_c 2
#endif

/* Number of Consolidated Bill stored in the Server (min. value is 5) */
#ifndef gNumofServerConsolidatedBill_c
#define gNumofServerConsolidatedBill_c 5
#endif

/* Number of Consolidated Bill Handled by the Client(min. value is 2) */
#ifndef gNumofClientConsolidatedBill_c
#define gNumofClientConsolidatedBill_c 2
#endif

/* When responding to a request via
the Inter-PAN SAP, the Publish Price command should be broadcast to the PAN of
the requester after a random delay between 0 and 0.5 seconds, to avoid a potential
broadcast storm of packets */
#ifndef gInterPanMaxJitterTime_c
#define gInterPanMaxJitterTime_c 500
#endif

/* Maximum incoming transfer size that that can be supported with fragmentation using SE(128 bytes)*/
#ifndef gSEMaxIncomingTransferSize
#define gSEMaxIncomingTransferSize  0x80,0x00  
#endif
                                                 
/* Fragmenation window size, can be 1 - 8 */
#ifndef gSEMaxWindowSize_c
#define gSEMaxWindowSize_c          1        
#endif

/*Fragmentation interframe delay, can be 1 - 255ms */
#ifndef gSEInterframeDelay_c
#define gSEInterframeDelay_c        50    
#endif

/* Fragmentation maximum length, can be 1 - ApsmeGetMaxAsduLength() */
#ifndef gSEMaxFragmentLength_c
#define gSEMaxFragmentLength_c      64   
#endif



/* Smart Energy Profile - little endian (0x0109) */
#if gBigEndian_c
#define gSEProfId_c	0x0901      /* 0x0109, little endian order */
#else
#define gSEProfId_c	0x0109      
#endif

#define gSEProfIdb_c	0x09,0x01   /* in byte form */




/******************************************
device definitions 
*******************************************/

typedef uint16_t SEDevId_t;

#if gBigEndian_c
/* Generic */
#define gSERangeExtender_c            0x0800  /* Range Extender */

/* SE */
#define gSEEnergyServicePortal_c      0x0005 /* Energy Service Portal */
#define gSEMeter_c                    0x0105 /* Meter */
#define gInPremiseDisplay_c           0x0205 /* In-Premise Display */
#define gPCT_c                        0x0305 /* Programmable Controller thermostat */
#define gLdCtrlDevice_c               0x0405 /* Load Control */
#define gSmartAppliance_c             0x0605 /* Smart Appliance */
#define gPrepaymentTerminal_c         0x0705 /* Prepayment Terminal */

#else /* #if gBigEndian_c */
/* Generic */
#define gSERangeExtender_c            0x0008  /* Range Extender */

/* SE */
#define gSEEnergyServicePortal_c      0x0500 /* Energy Service Portal */
#define gSEMeter_c                    0x0501 /* Meter */
#define gInPremiseDisplay_c           0x0502 /* In-Premise Display */
#define gPCT_c                        0x0503 /* Programmable Controller thermostat */
#define gLdCtrlDevice_c               0x0504 /* Load Control */
#define gSmartAppliance_c             0x0506 /* Smart Appliance */
#define gPrepaymentTerminal_c         0x0507 /* Prepayment Terminal */
#endif /* #if gBigEndian_c */

/* devices in byte form */
#define gSERangeExtenderb_c            0x08,0x00  /* Range Extender */

/* SE */
#define gSEEnergyServicePortalb_c      0x00,0x05 /* Energy Service Portal */
#define gSEMeterb_c                    0x01,0x05 /* Meter */
#define gInPremiseDisplayb_c           0x02,0x05 /* In-Premise Display */
#define gPCTb_c                        0x03,0x05 /* Programmable Controller thermostat */
#define gLdCtrlDeviceb_c               0x04,0x05 /* Load Control  */
#define gSmartApplianceb_c             0x06,0x05 /* Smart Appliance */
#define gPrepaymentTerminalb_c         0x07,0x05 /* Prepayment Terminal */




/**********************************************
Stuff that should be added to ZCL.h
***********************************************/
#if gBigEndian_c
/*SE cluster ID's*/
/* cluster IDs (for endpoints... for now) */
#define gZclClusterPrice_c                       0x0007
#define gZclClusterDmndRspLdCtrl_c               0x0107
#define gZclClusterSmplMet_c                     0x0207
#define gZclClusterMsg_c                         0x0307
#define gZclClusterSETunneling_c                 0x0407
#define gZclClusterPrepayment_c                  0x0507
#define gKeyEstabCluster_c                       0x0008
#else /* #if gBigEndian_c */
#define gZclClusterPrice_c                       0x0700
#define gZclClusterDmndRspLdCtrl_c               0x0701
#define gZclClusterSmplMet_c                     0x0702
#define gZclClusterMsg_c                         0x0703
#define gZclClusterSETunneling_c                 0x0704
#define gZclClusterPrepayment_c                  0x0705
#define gKeyEstabCluster_c                       0x0800
#endif /* #if gBigEndian_c  */

#define gaZclClusterPrice_c                       0x00,0x07
#define gaZclClusterDmndRspLdCtrl_c               0x01,0x07
#define gaZclClusterSmplMet_c                     0x02,0x07
#define gaZclClusterMsg_c                         0x03,0x07
#define gaZclClusterSETunneling_c                 0x04,0x07
#define gaZclClusterPrepayment_c                  0x05,0x07
#define gaKeyEstabCluster_c                       0x00,0x08

#ifdef __IAR_SYSTEMS_ICC__
#pragma pack(1)
#endif

typedef uint8_t BlockThreshold_t[6];
typedef uint8_t Duration24_t[3];
typedef uint8_t Duration16_t[2];
typedef uint8_t Multiplier24_t[3];
typedef uint8_t Divisor24_t[3];
typedef uint8_t CommodityCharge_t[4];
typedef uint8_t ConversionFactor_t[4];
typedef uint8_t CalorificValue_t[4];
typedef uint8_t BlockPeriodDuration_t[3];

typedef struct zclStr12_tag
{
  uint8_t length;
  uint8_t aStr[11];
} zclStr12_t;

typedef struct zclStr32Oct_tag
{
  uint8_t length;
  uint8_t aStr[31];
} zclStr32Oct_t;

typedef struct zclStr24Oct_tag
{
  uint8_t length;
  uint8_t aStr[23];
} zclStr24Oct_t;

typedef struct zclStr16Oct_tag
{
  uint8_t length;
  uint8_t aStr[15];
} zclStr16Oct_t;
typedef struct BlockPeriodPrice_tag
{
  ZCLTime_t StartofBlockPeriod;
  Duration24_t BlockPeriodDuration;
  Multiplier24_t ThresholdMultiplier;
  Divisor24_t ThresholdDivisor;
} BlockPeriodPrice_t;

typedef struct CommodityTypePrice_tag
{
#if gASL_ZclPrice_CommodityTypeNumber_d >= 1  
  uint8_t CommodityType;
#endif  
#if gASL_ZclPrice_CommodityTypeNumber_d >= 2  
  CommodityCharge_t StandingCharge;
#endif  
#if gASL_ZclPrice_CommodityTypeNumber_d >= 3  
  ConversionFactor_t ConversionFactor;
#endif  
#if gASL_ZclPrice_CommodityTypeNumber_d >= 4  
  uint8_t  ConversionFactorTrlDigit;
#endif  
#if gASL_ZclPrice_CommodityTypeNumber_d >= 5  
  CalorificValue_t CalorificValue;
#endif  
#if gASL_ZclPrice_CommodityTypeNumber_d >= 6  
  uint8_t CalorificValueUnit;
#endif  
#if gASL_ZclPrice_CommodityTypeNumber_d >= 7  
  uint8_t CalorificValueTrlDigit;
#endif  
} CommodityTypePrice_t;

typedef uint8_t SecuritySuite_t[2];

typedef union addrInfoType_tag
{
  afAddrInfo_t addrInfo; 
  InterPanAddrInfo_t addrInterPan;
}addrInfoType_t;

typedef struct msgAddrInfo_tag
{
  addrInfoType_t msgAddrInfo; 
  uint8_t msgControl;
}msgAddrInfo_t;

typedef struct storedInterPanAddrInfo_tag
{
InterPanAddrInfo_t addrInfo;
uint8_t entryStatus;
}storedInterPanAddrInfo_t;

typedef struct storedPriceAddrInfo_tag
{
zbIeeeAddr_t iEEEaddr;
afAddrInfo_t addrInfo;
uint8_t      entryStatus;
}storedPriceAddrInfo_t;

typedef struct EspRegisterDevice_tag 
{
  zbIeeeAddr_t Addr;
  uint8_t InstallCodeLength;  /*Install code length includes CRC also*/
  uint8_t InstallCode[18]; /*18 bytes is the maximum length */
} EspRegisterDevice_t;

typedef struct EspDeRegisterDevice_tag 
{
  zbIeeeAddr_t Addr;
} EspDeRegisterDevice_t;
/*Install code length includes CRC also*/
#define gZclSEInstallCode48bit_c  (6+2)
#define gZclSEInstallCode64bit_c  (8+2)
#define gZclSEInstallCode96bit_c  (12+2)
#define gZclSEInstallCode128bit_c (16+2)

typedef zbStatus_t ESPRegisterDevFunc(EspRegisterDevice_t *devinfo);
typedef zbStatus_t ESPDeRegisterDevFunc(EspDeRegisterDevice_t *devinfo);

typedef struct zclSEGenericRxCmd_tag
{  
  void *pSECmd; /* point to received SE Cmd (point in first fragment ) */
  zbRxFragmentedHdr_t *pRxFrag; /* used when a fragmented message is received;  */
}zclSEGenericRxCmd_t;
/**********************************************
Load Control - Demand Response cluster definitions
***********************************************/


#define gZclCmdDmndRspLdCtrl_LdCtrlEvtReq_c                 0x00 /* M */
#define gZclCmdDmndRspLdCtrl_CancelLdCtrlEvtReq_c           0x01 /* M */
#define gZclCmdDmndRspLdCtrl_CancelAllLdCtrlEvtsReq_c       0x02 /* M */

typedef uint8_t MsgId_t[4];
typedef uint8_t SEEvtId_t[4];
typedef uint16_t LCDRDevCls_t;
typedef uint16_t Duration_t;
typedef uint16_t LCDRSetPoint_t;
typedef uint8_t IntrvForProfiling_t[3];
typedef uint8_t Pressure_t[6];
typedef uint8_t Demmand_t[3];
typedef uint8_t Consmp[3];
typedef uint8_t ProviderID_t[4];
typedef uint8_t Currency_t[2];
typedef uint8_t Price_t[4];
typedef uint8_t Signature_t[42];
typedef uint8_t Summ_t[6];
typedef uint8_t Charge_t[4];
typedef uint8_t BlockInf_t[4];
typedef uint8_t Bill_t[4];
typedef uint8_t CO2Value_t[4];
typedef uint8_t StandingCharge_t[4];
typedef uint8_t BlockThresholdMask_t[2];
typedef uint8_t BlockThresholdMultiplier_t[3];
typedef uint8_t BlockThresholdDivisor_t[3];

/* Max and min values for Command Fields Range for Load Control*/
#define gZclCmdDmndRspLdCtrl_MaxDurationInMinutes_c    1440  // One day 
#define gZclCmdDmndRspLdCtrl_MaxCritLev_c               0x0F
#define gZclCmdDmndRspLdCtrl_MaxTempSetPoint_c          0x7FFF
#define gZclCmdDmndRspLdCtrl_MaxTempOffset_c            0xFE
#define gZclCmdDmndRspLdCtrl_MinAverageLdAdjustmentPercentage_c            (-100)
#define gZclCmdDmndRspLdCtrl_MaxAverageLdAdjustmentPercentage_c            100
#define gZclCmdDmndRspLdCtrl_MaxDutyCycle_c            100


/* Optional values for Command Fields Range for Load Control*/
#define gZclCmdDmndRspLdCtrl_OptionalTempSetPoint_c          0x8000
#define gZclCmdDmndRspLdCtrl_OptionalTempOffset_c            0xFF
#define gZclCmdDmndRspLdCtrl_OptionalAverageLdAdjustmentPercentage_c    (-128)
#define gZclCmdDmndRspLdCtrl_OptionalDutyCycle_c            0xFF


/* Load Control Event command payload */
typedef struct zclCmdDmndRspLdCtrl_LdCtrlEvtReq_tag
{
  SEEvtId_t         IssuerEvtID;
  LCDRDevCls_t      DevGroupClass;
  uint8_t           UtilityGroup;
  ZCLTime_t         StartTime;
  Duration_t        DurationInMinutes;
  uint8_t           CritLev;
  uint8_t           CoolingTempOffset;
  uint8_t           HeatingTempOffset;
  LCDRSetPoint_t    CoolingTempSetPoint;
  LCDRSetPoint_t    HeatingTempSetPoint;
  int8_t            AverageLdAdjustmentPercentage;
  uint8_t           DutyCycle;
  uint8_t           EvtCtrl;
} zclCmdDmndRspLdCtrl_LdCtrlEvtReq_t;

/* Load Control Event command request */
typedef struct zclDmndRspLdCtrl_LdCtrlEvtReq_tag
{
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdDmndRspLdCtrl_LdCtrlEvtReq_t cmdFrame;
} zclDmndRspLdCtrl_LdCtrlEvtReq_t;

typedef struct zclLdCtrl_EventsTableEntry_tag
{
  afAddrInfo_t addrInfo;    /*where to send the Reports Event Status for this event */
  zclCmdDmndRspLdCtrl_LdCtrlEvtReq_t cmdFrame; /* keep the Event */
  uint8_t           CurrentStatus;      /* the current event status; can be modify by the user */ 
  uint8_t           NextStatus;         /* keep track of next status which depend of current status (almost all the time ) */
  uint8_t           IsSuccessiveEvent;  /* TRUE - is a "back to back" event; else FAlSE */ 
  ZCLTime_t         CancelTime; /* When the event should be canceled; 0Xffffffff meens event not canceled; apply the random time stop if needed */
  uint8_t           CancelCtrl;   /* to check if the cancelling on CancelTime use the randomization information from cmdFrame */
  LCDRDevCls_t      CancelClass; /* 0xffff meens not used */
  LCDRDevCls_t      NotSupersededClass; /* used for supressing events; 0xffff - not used yet; 0x0000 - event is seperseded */
  ZCLTime_t         SupersededTime;
  uint8_t           EntryStatus; /* the tabel entry is in use (0xFF) or not (0x00) */
}zclLdCtrl_EventsTableEntry_t;


/*Entry used or not */
#define gEntryUsed_c 0xff
#define gEntryNotUsed_c 0x00

/* Device Class Field BitMap/Encoding */
#define gZclSELCDR_HVAC_DevCls_c             0x01,0x00
#define gZclSELCDR_StripHeat_DevCls_c        0x02,0x00
#define gZclSELCDR_WaterHeater_DevCls_c      0x04,0x00
#define gZclSELCDR_PoolPump_DevCls_c         0x08,0x00
#define gZclSELCDR_SmartAppliance_DevCls_c   0x10,0x00
#define gZclSELCDR_IrrigationPump_DevCls_c   0x20,0x00
#define gZclSELCDR_ManagedLds_DevCls_c       0x40,0x00
#define gZclSELCDR_SmplMiscLds_DevCls_c      0x80,0x00
#define gZclSELCDR_ExteriorLighting_DevCls_c 0x00,0x01
#define gZclSELCDR_InteriorLighting_DevCls_c 0x00,0x02
#define gZclSELCDR_ElectricVehicle_DevCls_c  0x00,0x04
#define gZclSELCDR_Generation_DevCls_c       0x00,0x08

/*Criticality levels*/
#define gZclSECritLev_Reserved_c 0x00
#define gZclSECritLev_Green_c    0x01
#define gZclSECritLev_1_c        0x02
#define gZclSECritLev_2_c        0x03
#define gZclSECritLev_3_c        0x04
#define gZclSECritLev_4_c        0x05
#define gZclSECritLev_5_c        0x06
#define gZclSECritLev_Emergency_c     0x07
#define gZclSECritLev_PlannedOutage_c 0x08
#define gZclSECritLev_ServiceDisconnect 0x09
#define gZclSECritLev_UtilDef1_c 0x0a
#define gZclSECritLev_UtilDef2_c 0x0b
#define gZclSECritLev_UtilDef3_c 0x0c
#define gZclSECritLev_UtilDef4_c 0x0d
#define gZclSECritLev_UtilDef5_c 0x0e
#define gZclSECritLev_UtilDef6_c 0x0f

/* Cancel Load Control Event command payload */
typedef struct zclCmdDmndRspLdCtrl_CancelLdCtrlEvtReq_tag
{
  SEEvtId_t         IssuerEvtID;
  LCDRDevCls_t      DevGroupClass;
  uint8_t           UtilityGroup; 
  uint8_t           CancelCtrl; 
  ZCLTime_t         EffectiveTime;  
} zclCmdDmndRspLdCtrl_CancelLdCtrlEvtReq_t;

/* Cancel Load Control Event command request */
typedef struct zclDmndRspLdCtrl_CancelLdCtrlEvtReq_tag
{
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdDmndRspLdCtrl_CancelLdCtrlEvtReq_t cmdFrame;
} zclDmndRspLdCtrl_CancelLdCtrlEvtReq_t;

/* Cancel Control */
#define gCancelCtrlOverideRandomization_c 0x00 /* A value of Zero (0) indicates that randomization is overridden and the Evt
                                                   should be terminated immediately at the Effective Time. */
#define gCancelCtrlEndRandomization_c 0x01 /* A value of One (1) indicates the Evt should end using randomization settings in
                                                 the original Evt */

/* Cancel All Load Control Events command payload */
typedef struct zclCmdDmndRspLdCtrl_CancelAllLdCtrlEvtsReq_tag
{
  uint8_t           CancelCtrl; 
} zclCmdDmndRspLdCtrl_CancelAllLdCtrlEvtsReq_t;

/* Cancel All Load Control Events command request */
typedef struct zclDmndRspLdCtrl_CancelAllLdCtrlEvtsReq_tag
{
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdDmndRspLdCtrl_CancelAllLdCtrlEvtsReq_t cmdFrame;
} zclDmndRspLdCtrl_CancelAllLdCtrlEvtsReq_t;


/*Generated Command IDs for the Demand Response
  and Load Control Client*/
#define gZclCmdDmndRspLdCtrl_ReportEvtStatus_c           0x00 /* M */
#define gZclCmdDmndRspLdCtrl_GetScheduledEvts_c          0x01 /* M */

/* Report Event Status command payload */
typedef struct zclCmdDmndRspLdCtrl_ReportEvtStatus_tag
{
  SEEvtId_t        IssuerEvtID;
  uint8_t          EvtStatus;
  ZCLTime_t        EvtStatusTime;
  uint8_t          CritLevApplied;
  LCDRSetPoint_t   CoolTempSetPntApplied;
  LCDRSetPoint_t   HeatTempSetPntApplied;
  int8_t           AverageLdAdjustmentPercentage;
  uint8_t          DutyCycle;
  uint8_t          EvtCtrl; 
  uint8_t          SignatureType;
  Signature_t      Signature;
} zclCmdDmndRspLdCtrl_ReportEvtStatus_t;

/* Report Event Status command request */
typedef struct zclDmndRspLdCtrl_ReportEvtStatus_tag
{
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdDmndRspLdCtrl_ReportEvtStatus_t cmdFrame;
} zclDmndRspLdCtrl_ReportEvtStatus_t;

typedef struct zclCmdDmndRspLdCtrl_GetScheduledEvts_tag
{
ZCLTime_t        EvtStartTime;
uint8_t          NumOfEvts;
}zclCmdDmndRspLdCtrl_GetScheduledEvts_t;

typedef struct zclDmndRspLdCtrl_GetScheduledEvts_tag
{
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdDmndRspLdCtrl_GetScheduledEvts_t cmdFrame;
} zclDmndRspLdCtrl_GetScheduledEvts_t;

#define gSELCDR_SignatureType_c 0x01

/* Event Status Field Values */
#define gSELCDR_LdCtrlEvtCode_CmdRcvd_c                     0x01
#define gSELCDR_LdCtrlEvtCode_Started_c                     0x02
#define gSELCDR_LdCtrlEvtCode_Completed_c                   0x03
#define gSELCDR_LdCtrlEvtCode_UserHaveToChooseOptOut_c      0x04
#define gSELCDR_LdCtrlEvtCode_UserHaveToChooseOptIn_c       0x05
#define gSELCDR_LdCtrlEvtCode_EvtCancelled_c                0x06
#define gSELCDR_LdCtrlEvtCode_EvtSuperseded_c                0x07
#define gSELCDR_LdCtrlEvtCode_EvtPrtlCompletedWithUserOptOut_c   0x08
#define gSELCDR_LdCtrlEvtCode_EvtPrtlCompletedWithUserOptIn_c    0x09
#define gSELCDR_LdCtrlEvtCode_EvtCompletedWithNoUser_c           0x0A
#define gSELCDR_LdCtrlEvtCode_EvtInvalidCancelCmdDefault_c       0xF8
#define gSELCDR_LdCtrlEvtCode_EvtInvalidEffectiveTime_c          0xF9
#define gSELCDR_LdCtrlEvtCode_EvtReserved_c                      0xFA
#define gSELCDR_LdCtrlEvtCode_EvtHadExpired_c                    0xFB
#define gSELCDR_LdCtrlEvtCode_EvtUndef_c                         0xFD
#define gSELCDR_LdCtrlEvtCode_LdCtrlEvtCmdRjctd_c                0xFE

/* Server cluster attributes */
#if gBigEndian_c
/*Demand Response Client Cluster Attributes*/
#define gZclAttrUtilityGroup_c          0x0000         /*M*/
#define gZclAttrStartRandomizeMin_c     0x0100         /*M*/
#define gZclAttrStopRandomizeMin_c      0x0200         /*M*/
#define gZclAttrDevCls_c                0x0300         /*M*/
#else/* #if gBigEndian_c */
#define gZclAttrUtilityGroup_c          0x0000         /*M*/
#define gZclAttrStartRandomizeMin_c     0x0001         /*M*/
#define gZclAttrStopRandomizeMin_c      0x0002         /*M*/
#define gZclAttrDevCls_c                0x0003         /*M*/
#endif /* #if gBigEndian_c */

/**********************************************
Simple Metering cluster definitions
***********************************************/
#if gBigEndian_c
#define gZclAttrSmplMetReadInfoSet_c                    0x0000
#define gZclAttrSmplMetTOUInfoSet_c                     0x0001
#define gZclAttrSmplMetMeterStatusSet_c                 0x0002
#define gZclAttrSmplMetConsmpFormatSet_c                0x0003
#define gZclAttrSmplMetESPHistoricalConsmpSet_c         0x0004
#define gZclAttrSmplMetLdProfConfigurationSet_c         0x0005
#define gZclAttrSmplMetSupplyLimitSet_c                 0x0006
#define gZclAttrSmplMetBlockInformationSetDlvrd_c       0x0007
#define gZclAttrSmplMetAlarmsSet_c                      0x0008
#define gZclAttrSmplMetBlockInformationSetRcvd_c        0x0009
#define gZclAttrSmplMetBillingSet_c                     0x000A
#else /* #if gBigEndian_c */
#define gZclAttrSmplMetReadInfoSet_c                    0x0000
#define gZclAttrSmplMetTOUInfoSet_c                     0x0100
#define gZclAttrSmplMetMeterStatusSet_c                 0x0200
#define gZclAttrSmplMetConsmpFormatSet_c                0x0300
#define gZclAttrSmplMetESPHistoricalConsmpSet_c         0x0400
#define gZclAttrSmplMetLdProfConfigurationSet_c         0x0500
#define gZclAttrSmplMetSupplyLimitSet_c                 0x0600
#define gZclAttrSmplMetBlockInformationSetDlvrd_c       0x0700
#define gZclAttrSmplMetAlarmsSet_c                      0x0800
#define gZclAttrSmplMetBlockInformationSetRcvd_c        0x0900
#define gZclAttrSmplMetBillingSet_c                     0x0A00
#endif /* #if gBigEndian_c */

#if gBigEndian_c
/*Reading Information Set*/
#define gZclAttrSmplMetRISCurrSummDlvrd_c                       (gZclAttrSmplMetReadInfoSet_c | 0x0000) /*M*/
#define gZclAttrSmplMetRISCurrSummRcvd_c                        (gZclAttrSmplMetReadInfoSet_c | 0x0100)
#define gZclAttrSmplMetRISCurrMaxDmndDlvrd_c                    (gZclAttrSmplMetReadInfoSet_c | 0x0200)
#define gZclAttrSmplMetRISCurrMaxDmndRcvd_c                     (gZclAttrSmplMetReadInfoSet_c | 0x0300)
#define gZclAttrSmplMetRISDFTSumm_c                             (gZclAttrSmplMetReadInfoSet_c | 0x0400)
#define gZclAttrSmplMetRISDailyFreezeTime_c                     (gZclAttrSmplMetReadInfoSet_c | 0x0500)
#define gZclAttrSmplMetRISPowerFactor_c                         (gZclAttrSmplMetReadInfoSet_c | 0x0600)
#define gZclAttrSmplMetRISReadingSnapShotTime_c                 (gZclAttrSmplMetReadInfoSet_c | 0x0700)
#define gZclAttrSmplMetRISCurrMaxDmndDlvrdTimer_c               (gZclAttrSmplMetReadInfoSet_c | 0x0800)
#define gZclAttrSmplMetRISCurrMaxDmndRcvdTime_c                 (gZclAttrSmplMetReadInfoSet_c | 0x0900)
#define gZclAttrSmplMetRISDefaultUpdatePeriod_c                 (gZclAttrSmplMetReadInfoSet_c | 0x0A00)
#define gZclAttrSmplMetRISFastPollUpdatePeriod_c                (gZclAttrSmplMetReadInfoSet_c | 0x0B00)
#define gZclAttrSmplMetRISCurrBlockPeriodConsumptionDelivered_c (gZclAttrSmplMetReadInfoSet_c | 0x0C00)
#define gZclAttrSmplMetRISDailyConsumptionTarget_c              (gZclAttrSmplMetReadInfoSet_c | 0x0D00)
#define gZclAttrSmplMetRISCurrBlock_c                           (gZclAttrSmplMetReadInfoSet_c | 0x0E00)
#define gZclAttrSmplMetRISProfileIntPeriod_c                    (gZclAttrSmplMetReadInfoSet_c | 0x0F00)
#define gZclAttrSmplMetRISIntReadReportingPeriod_c              (gZclAttrSmplMetReadInfoSet_c | 0x1000)
#define gZclAttrSmplMetRISPresetReadingTime_c                   (gZclAttrSmplMetReadInfoSet_c | 0x1100)
#define gZclAttrSmplMetRISVolumePerReport_c                     (gZclAttrSmplMetReadInfoSet_c | 0x1200)
#define gZclAttrSmplMetRISFlowRestriction_c                     (gZclAttrSmplMetReadInfoSet_c | 0x1300)
#define gZclAttrSmplMetRISSupplyStatus_c                        (gZclAttrSmplMetReadInfoSet_c | 0x1400)
#define gZclAttrSmplMetRISCurrInletEnergyCarrierSummation_c     (gZclAttrSmplMetReadInfoSet_c | 0x1500)
#define gZclAttrSmplMetRISCurrOutletEnergyCarrierSummation_c    (gZclAttrSmplMetReadInfoSet_c | 0x1600)
#define gZclAttrSmplMetRISInletTemp_c                           (gZclAttrSmplMetReadInfoSet_c | 0x1700)
#define gZclAttrSmplMetRISOutletTemp_c                          (gZclAttrSmplMetReadInfoSet_c | 0x1800)
#define gZclAttrSmplMetRISCtrlTemp_c                            (gZclAttrSmplMetReadInfoSet_c | 0x1900)
#define gZclAttrSmplMetRISCurrInletEnergyCarrierDemand_c        (gZclAttrSmplMetReadInfoSet_c | 0x1A00)
#define gZclAttrSmplMetRISCurrOutletEnergyCarrierDemand_c       (gZclAttrSmplMetReadInfoSet_c | 0x1B00)

/* TOU Information Set */
#define gZclAttrSmplMetTOUCurrTier1SummDlvrd_c  (gZclAttrSmplMetTOUInfoSet_c | 0x0000)
#define gZclAttrSmplMetTOUCurrTier1SummRcvd_c   (gZclAttrSmplMetTOUInfoSet_c | 0x0100)
#define gZclAttrSmplMetTOUCurrTier2SummDlvrd_c  (gZclAttrSmplMetTOUInfoSet_c | 0x0200)
#define gZclAttrSmplMetTOUCurrTier2SummRcvd_c   (gZclAttrSmplMetTOUInfoSet_c | 0x0300)
#define gZclAttrSmplMetTOUCurrTier3SummDlvrd_c  (gZclAttrSmplMetTOUInfoSet_c | 0x0400)
#define gZclAttrSmplMetTOUCurrTier3SummRcvd_c   (gZclAttrSmplMetTOUInfoSet_c | 0x0500)
#define gZclAttrSmplMetTOUCurrTier4SummDlvrd_c  (gZclAttrSmplMetTOUInfoSet_c | 0x0600)
#define gZclAttrSmplMetTOUCurrTier4SummRcvd_c   (gZclAttrSmplMetTOUInfoSet_c | 0x0700)
#define gZclAttrSmplMetTOUCurrTier5SummDlvrd_c  (gZclAttrSmplMetTOUInfoSet_c | 0x0800)
#define gZclAttrSmplMetTOUCurrTier5SummRcvd_c   (gZclAttrSmplMetTOUInfoSet_c | 0x0900)
#define gZclAttrSmplMetTOUCurrTier6SummDlvrd_c  (gZclAttrSmplMetTOUInfoSet_c | 0x0A00)
#define gZclAttrSmplMetTOUCurrTier6SummRcvd_c   (gZclAttrSmplMetTOUInfoSet_c | 0x0B00)
#define gZclAttrSmplMetTOUCurrTier7SummDlvrd_c  (gZclAttrSmplMetTOUInfoSet_c | 0x0C00)
#define gZclAttrSmplMetTOUCurrTier7SummRcvd_c   (gZclAttrSmplMetTOUInfoSet_c | 0x0D00)
#define gZclAttrSmplMetTOUCurrTier8SummDlvrd_c  (gZclAttrSmplMetTOUInfoSet_c | 0x0E00)
#define gZclAttrSmplMetTOUCurrTier8SummRcvd_c   (gZclAttrSmplMetTOUInfoSet_c | 0x0F00)
#define gZclAttrSmplMetTOUCurrTier9SummDlvrd_c  (gZclAttrSmplMetTOUInfoSet_c | 0x1000)
#define gZclAttrSmplMetTOUCurrTier9SummRcvd_c   (gZclAttrSmplMetTOUInfoSet_c | 0x1100)
#define gZclAttrSmplMetTOUCurrTier10SummDlvrd_c (gZclAttrSmplMetTOUInfoSet_c | 0x1200)
#define gZclAttrSmplMetTOUCurrTier10SummRcvd_c  (gZclAttrSmplMetTOUInfoSet_c | 0x1300)
#define gZclAttrSmplMetTOUCurrTier11SummDlvrd_c (gZclAttrSmplMetTOUInfoSet_c | 0x1400)
#define gZclAttrSmplMetTOUCurrTier11SummRcvd_c  (gZclAttrSmplMetTOUInfoSet_c | 0x1500)
#define gZclAttrSmplMetTOUCurrTier12SummDlvrd_c (gZclAttrSmplMetTOUInfoSet_c | 0x1600)
#define gZclAttrSmplMetTOUCurrTier12SummRcvd_c  (gZclAttrSmplMetTOUInfoSet_c | 0x1700)
#define gZclAttrSmplMetTOUCurrTier13SummDlvrd_c (gZclAttrSmplMetTOUInfoSet_c | 0x1800)
#define gZclAttrSmplMetTOUCurrTier13SummRcvd_c  (gZclAttrSmplMetTOUInfoSet_c | 0x1900)
#define gZclAttrSmplMetTOUCurrTier14SummDlvrd_c (gZclAttrSmplMetTOUInfoSet_c | 0x1A00)
#define gZclAttrSmplMetTOUCurrTier14SummRcvd_c  (gZclAttrSmplMetTOUInfoSet_c | 0x1B00)
#define gZclAttrSmplMetTOUCurrTier15SummDlvrd_c (gZclAttrSmplMetTOUInfoSet_c | 0x1C00)
#define gZclAttrSmplMetTOUCurrTier15SummRcvd_c  (gZclAttrSmplMetTOUInfoSet_c | 0x1D00)

/*Consumption Formating Set*/
#define gZclAttrSmplMetCFSUnitofMeasure_c                    (gZclAttrSmplMetConsmpFormatSet_c | 0x0000)  /* M */
#define gZclAttrSmplMetCFSMultiplier_c                       (gZclAttrSmplMetConsmpFormatSet_c | 0x0100)
#define gZclAttrSmplMetCFSDivisor_c                          (gZclAttrSmplMetConsmpFormatSet_c | 0x0200)
#define gZclAttrSmplMetCFSSummFormat_c                       (gZclAttrSmplMetConsmpFormatSet_c | 0x0300)  /* M */
#define gZclAttrSmplMetCFSDmndFormat_c                       (gZclAttrSmplMetConsmpFormatSet_c | 0x0400)
#define gZclAttrSmplMetCFSHistoricalConsmpFormat_c           (gZclAttrSmplMetConsmpFormatSet_c | 0x0500)
#define gZclAttrSmplMetCFSMetDevType_c                       (gZclAttrSmplMetConsmpFormatSet_c | 0x0600) /* M */
#define gZclAttrSmplMetCFSSiteID_c                           (gZclAttrSmplMetConsmpFormatSet_c | 0x0700) 
#define gZclAttrSmplMetCFSMeterSerialNumber_c                (gZclAttrSmplMetConsmpFormatSet_c | 0x0800) 
#define gZclAttrSmplMetCFSEnergyCarrierUnitOfMeasure_c       (gZclAttrSmplMetConsmpFormatSet_c | 0x0900)
#define gZclAttrSmplMetCFSEnergyCarrierSummationFormatting_c (gZclAttrSmplMetConsmpFormatSet_c | 0x0A00)
#define gZclAttrSmplMetCFSEnergyCarrierDemandFormatting_c    (gZclAttrSmplMetConsmpFormatSet_c | 0x0B00)
#define gZclAttrSmplMetCFSTempUnitOfMeasure_c                (gZclAttrSmplMetConsmpFormatSet_c | 0x0C00)
#define gZclAttrSmplMetCFSTempFormatting_c                   (gZclAttrSmplMetConsmpFormatSet_c | 0x0D00)

/* ESP Historical Consumption */
#define gZclAttrSmplMetEHCInstantaneousDmnd_c                (gZclAttrSmplMetESPHistoricalConsmpSet_c | 0x0000)
#define gZclAttrSmplMetEHCCurrDayConsmpDlvrd_c               (gZclAttrSmplMetESPHistoricalConsmpSet_c | 0x0100)
#define gZclAttrSmplMetEHCCurrDayConsmpRcvd_c                (gZclAttrSmplMetESPHistoricalConsmpSet_c | 0x0200)
#define gZclAttrSmplMetEHCPreviousDayConsmpDlvrd_c           (gZclAttrSmplMetESPHistoricalConsmpSet_c | 0x0300)
#define gZclAttrSmplMetEHCPreviousDayConsmpRcvd_c            (gZclAttrSmplMetESPHistoricalConsmpSet_c | 0x0400)
#define gZclAttrSmplMetEHCCurrPrtlProfIntrvStartTimeDlvrd_c  (gZclAttrSmplMetESPHistoricalConsmpSet_c | 0x0500)
#define gZclAttrSmplMetEHCCurrPrtlProfIntrvStartTimeRcvd_c   (gZclAttrSmplMetESPHistoricalConsmpSet_c | 0x0600)
#define gZclAttrSmplMetEHCCurrPrtlProfIntrvValueDlvrd_c      (gZclAttrSmplMetESPHistoricalConsmpSet_c | 0x0700)
#define gZclAttrSmplMetEHCCurrPrtlProfIntrvValueRcvd_c       (gZclAttrSmplMetESPHistoricalConsmpSet_c | 0x0800)
#define gZclAttrSmplMetEHCCurrDayMaxPressure_c               (gZclAttrSmplMetESPHistoricalConsmpSet_c | 0x0900)
#define gZclAttrSmplMetEHCCurrDayMinPressure_c               (gZclAttrSmplMetESPHistoricalConsmpSet_c | 0x0A00)
#define gZclAttrSmplMetEHCPrevDayMaxPressure_c               (gZclAttrSmplMetESPHistoricalConsmpSet_c | 0x0B00)
#define gZclAttrSmplMetEHCPrevDayMinPressure_c               (gZclAttrSmplMetESPHistoricalConsmpSet_c | 0x0C00)
#define gZclAttrSmplMetEHCCurrDayMaxDemand_c                 (gZclAttrSmplMetESPHistoricalConsmpSet_c | 0x0D00)
#define gZclAttrSmplMetEHCPrevDayMaxDemand_c                 (gZclAttrSmplMetESPHistoricalConsmpSet_c | 0x0E00)
#define gZclAttrSmplMetEHCCurrMonthMaxDemand_c               (gZclAttrSmplMetESPHistoricalConsmpSet_c | 0x0F00)
#define gZclAttrSmplMetEHCCurrYearMaxDemand_c                (gZclAttrSmplMetESPHistoricalConsmpSet_c | 0x1000)
#define gZclAttrSmplMetEHCCurrDayMaxEnergyCarrierDemand_c    (gZclAttrSmplMetESPHistoricalConsmpSet_c | 0x1100)
#define gZclAttrSmplMetEHCPrevDayMaxEnergyCarrierDemand_c    (gZclAttrSmplMetESPHistoricalConsmpSet_c | 0x1200)
#define gZclAttrSmplMetEHCCurrMonthMaxEnergyCarrierDemand_c  (gZclAttrSmplMetESPHistoricalConsmpSet_c | 0x1300)
#define gZclAttrSmplMetEHCCurrMonthMinEnergyCarrierDemand_c  (gZclAttrSmplMetESPHistoricalConsmpSet_c | 0x1400)
#define gZclAttrSmplMetEHCCurrYearMaxEnergyCarrierDemand_c   (gZclAttrSmplMetESPHistoricalConsmpSet_c | 0x1500)
#define gZclAttrSmplMetEHCCurrYearMinEnergyCarrierDemand_c   (gZclAttrSmplMetESPHistoricalConsmpSet_c | 0x1600)

/*Meter Status Set*/
#define gZclAttrSmplMetMSStatus_c               (gZclAttrSmplMetMeterStatusSet_c | 0x0000)  /* M */
#define gZclAttrSmplMetMSSRemainingBatteryLife_c (gZclAttrSmplMetMeterStatusSet_c | 0x0100)  
#define gZclAttrSmplMetMSSHoursInOperation_c    (gZclAttrSmplMetMeterStatusSet_c | 0x0200)  
#define gZclAttrSmplMetMSSHoursInFault_c        (gZclAttrSmplMetMeterStatusSet_c | 0x0300)  

/*Load Profile Configuration */
#define gZclAttrSmplMetLPCMaxNumOfPeriodsDlvrd_c       (gZclAttrSmplMetLdProfConfigurationSet_c | 0x0000)

/* Supply Limit Attribute Set */
#define gZclAttrSmplMetSLSCurrDmndDlvrd_c              (gZclAttrSmplMetSupplyLimitSet_c | 0x0000)
#define gZclAttrSmplMetSLSDmndLimit_c                  (gZclAttrSmplMetSupplyLimitSet_c | 0x0100)
#define gZclAttrSmplMetSLSDmndIntegrationPeriod_c      (gZclAttrSmplMetSupplyLimitSet_c | 0x0200)
#define gZclAttrSmplMetSLSNumOfDmndSubIntrvs_c         (gZclAttrSmplMetSupplyLimitSet_c | 0x0300)

/* Block Information Attribute Set Delivered*/
#define gZclAttrSmplMetBISCurrNoTierBlock1SummationDelivered_c            (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0000)
#define gZclAttrSmplMetBISCurrNoTierBlock2SummationDelivered_c            (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0100)
#define gZclAttrSmplMetBISCurrNoTierBlock3SummationDelivered_c            (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0200)
#define gZclAttrSmplMetBISCurrNoTierBlock4SummationDelivered_c            (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0300)
#define gZclAttrSmplMetBISCurrNoTierBlock5SummationDelivered_c            (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0400)
#define gZclAttrSmplMetBISCurrNoTierBlock6SummationDelivered_c            (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0500)
#define gZclAttrSmplMetBISCurrNoTierBlock7SummationDelivered_c            (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0600)
#define gZclAttrSmplMetBISCurrNoTierBlock8SummationDelivered_c            (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0700)
#define gZclAttrSmplMetBISCurrNoTierBlock9SummationDelivered_c            (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0800)
#define gZclAttrSmplMetBISCurrNoTierBlock10SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0900)
#define gZclAttrSmplMetBISCurrNoTierBlock11SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0A00)
#define gZclAttrSmplMetBISCurrNoTierBlock12SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0B00)
#define gZclAttrSmplMetBISCurrNoTierBlock13SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0C00)
#define gZclAttrSmplMetBISCurrNoTierBlock14SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0D00)
#define gZclAttrSmplMetBISCurrNoTierBlock15SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0E00)
#define gZclAttrSmplMetBISCurrNoTierBlock16SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0F00)
#define gZclAttrSmplMetBISCurrNoTier1Block1SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x1000)
#define gZclAttrSmplMetBISCurrNoTier1Block2SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x1100)
#define gZclAttrSmplMetBISCurrNoTier1Block3SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x1200)
#define gZclAttrSmplMetBISCurrNoTier1Block4SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x1300)
#define gZclAttrSmplMetBISCurrNoTier1Block5SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x1400)
#define gZclAttrSmplMetBISCurrNoTier1Block6SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x1500)
#define gZclAttrSmplMetBISCurrNoTier1Block7SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x1600)
#define gZclAttrSmplMetBISCurrNoTier1Block8SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x1700)
#define gZclAttrSmplMetBISCurrNoTier1Block9SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x1800)
#define gZclAttrSmplMetBISCurrNoTier1Block10SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x1900)
#define gZclAttrSmplMetBISCurrNoTier1Block11SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x1A00)
#define gZclAttrSmplMetBISCurrNoTier1Block12SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x1B00)
#define gZclAttrSmplMetBISCurrNoTier1Block13SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x1C00)
#define gZclAttrSmplMetBISCurrNoTier1Block14SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x1D00)
#define gZclAttrSmplMetBISCurrNoTier1Block15SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x1E00)
#define gZclAttrSmplMetBISCurrNoTier1Block16SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x1F00)
#define gZclAttrSmplMetBISCurrNoTier2Block1SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x2000)
#define gZclAttrSmplMetBISCurrNoTier2Block2SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x2100)
#define gZclAttrSmplMetBISCurrNoTier2Block3SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x2200)
#define gZclAttrSmplMetBISCurrNoTier2Block4SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x2300)
#define gZclAttrSmplMetBISCurrNoTier2Block5SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x2400)
#define gZclAttrSmplMetBISCurrNoTier2Block6SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x2500)
#define gZclAttrSmplMetBISCurrNoTier2Block7SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x2600)
#define gZclAttrSmplMetBISCurrNoTier2Block8SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x2700)
#define gZclAttrSmplMetBISCurrNoTier2Block9SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x2800)
#define gZclAttrSmplMetBISCurrNoTier2Block10SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x2900)
#define gZclAttrSmplMetBISCurrNoTier2Block11SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x2A00)
#define gZclAttrSmplMetBISCurrNoTier2Block12SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x2B00)
#define gZclAttrSmplMetBISCurrNoTier2Block13SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x2C00)
#define gZclAttrSmplMetBISCurrNoTier2Block14SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x2D00)
#define gZclAttrSmplMetBISCurrNoTier2Block15SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x2E00)
#define gZclAttrSmplMetBISCurrNoTier2Block16SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x2F00)
#define gZclAttrSmplMetBISCurrNoTier3Block1SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x3000)
#define gZclAttrSmplMetBISCurrNoTier3Block2SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x3100)
#define gZclAttrSmplMetBISCurrNoTier3Block3SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x3200)
#define gZclAttrSmplMetBISCurrNoTier3Block4SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x3300)
#define gZclAttrSmplMetBISCurrNoTier3Block5SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x3400)
#define gZclAttrSmplMetBISCurrNoTier3Block6SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x3500)
#define gZclAttrSmplMetBISCurrNoTier3Block7SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x3600)
#define gZclAttrSmplMetBISCurrNoTier3Block8SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x3700)
#define gZclAttrSmplMetBISCurrNoTier3Block9SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x3800)
#define gZclAttrSmplMetBISCurrNoTier3Block10SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x3900)
#define gZclAttrSmplMetBISCurrNoTier3Block11SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x3A00)
#define gZclAttrSmplMetBISCurrNoTier3Block12SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x3B00)
#define gZclAttrSmplMetBISCurrNoTier3Block13SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x3C00)
#define gZclAttrSmplMetBISCurrNoTier3Block14SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x3D00)
#define gZclAttrSmplMetBISCurrNoTier3Block15SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x3E00)
#define gZclAttrSmplMetBISCurrNoTier3Block16SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x3F00)
#define gZclAttrSmplMetBISCurrNoTier4Block1SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x4000)
#define gZclAttrSmplMetBISCurrNoTier4Block2SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x4100)
#define gZclAttrSmplMetBISCurrNoTier4Block3SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x4200)
#define gZclAttrSmplMetBISCurrNoTier4Block4SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x4300)
#define gZclAttrSmplMetBISCurrNoTier4Block5SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x4400)
#define gZclAttrSmplMetBISCurrNoTier4Block6SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x4500)
#define gZclAttrSmplMetBISCurrNoTier4Block7SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x4600)
#define gZclAttrSmplMetBISCurrNoTier4Block8SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x4700)
#define gZclAttrSmplMetBISCurrNoTier4Block9SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x4800)
#define gZclAttrSmplMetBISCurrNoTier4Block10SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x4900)
#define gZclAttrSmplMetBISCurrNoTier4Block11SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x4A00)
#define gZclAttrSmplMetBISCurrNoTier4Block12SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x4B00)
#define gZclAttrSmplMetBISCurrNoTier4Block13SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x4C00)
#define gZclAttrSmplMetBISCurrNoTier4Block14SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x4D00)
#define gZclAttrSmplMetBISCurrNoTier4Block15SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x4E00)
#define gZclAttrSmplMetBISCurrNoTier4Block16SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x4F00)
#define gZclAttrSmplMetBISCurrNoTier5Block1SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x5000)
#define gZclAttrSmplMetBISCurrNoTier5Block2SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x5100)
#define gZclAttrSmplMetBISCurrNoTier5Block3SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x5200)
#define gZclAttrSmplMetBISCurrNoTier5Block4SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x5300)
#define gZclAttrSmplMetBISCurrNoTier5Block5SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x5400)
#define gZclAttrSmplMetBISCurrNoTier5Block6SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x5500)
#define gZclAttrSmplMetBISCurrNoTier5Block7SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x5600)
#define gZclAttrSmplMetBISCurrNoTier5Block8SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x5700)
#define gZclAttrSmplMetBISCurrNoTier5Block9SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x5800)
#define gZclAttrSmplMetBISCurrNoTier5Block10SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x5900)
#define gZclAttrSmplMetBISCurrNoTier5Block11SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x5A00)
#define gZclAttrSmplMetBISCurrNoTier5Block12SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x5B00)
#define gZclAttrSmplMetBISCurrNoTier5Block13SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x5C00)
#define gZclAttrSmplMetBISCurrNoTier5Block14SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x5D00)
#define gZclAttrSmplMetBISCurrNoTier5Block15SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x5E00)
#define gZclAttrSmplMetBISCurrNoTier5Block16SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x5F00)
#define gZclAttrSmplMetBISCurrNoTier6Block1SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x6000)
#define gZclAttrSmplMetBISCurrNoTier6Block2SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x6100)
#define gZclAttrSmplMetBISCurrNoTier6Block3SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x6200)
#define gZclAttrSmplMetBISCurrNoTier6Block4SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x6300)
#define gZclAttrSmplMetBISCurrNoTier6Block5SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x6400)
#define gZclAttrSmplMetBISCurrNoTier6Block6SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x6500)
#define gZclAttrSmplMetBISCurrNoTier6Block7SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x6600)
#define gZclAttrSmplMetBISCurrNoTier6Block8SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x6700)
#define gZclAttrSmplMetBISCurrNoTier6Block9SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x6800)
#define gZclAttrSmplMetBISCurrNoTier6Block10SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x6900)
#define gZclAttrSmplMetBISCurrNoTier6Block11SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x6A00)
#define gZclAttrSmplMetBISCurrNoTier6Block12SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x6B00)
#define gZclAttrSmplMetBISCurrNoTier6Block13SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x6C00)
#define gZclAttrSmplMetBISCurrNoTier6Block14SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x6D00)
#define gZclAttrSmplMetBISCurrNoTier6Block15SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x6E00)
#define gZclAttrSmplMetBISCurrNoTier6Block16SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x6F00)
#define gZclAttrSmplMetBISCurrNoTier7Block1SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x7000)
#define gZclAttrSmplMetBISCurrNoTier7Block2SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x7100)
#define gZclAttrSmplMetBISCurrNoTier7Block3SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x7200)
#define gZclAttrSmplMetBISCurrNoTier7Block4SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x7300)
#define gZclAttrSmplMetBISCurrNoTier7Block5SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x7400)
#define gZclAttrSmplMetBISCurrNoTier7Block6SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x7500)
#define gZclAttrSmplMetBISCurrNoTier7Block7SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x7600)
#define gZclAttrSmplMetBISCurrNoTier7Block8SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x7700)
#define gZclAttrSmplMetBISCurrNoTier7Block9SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x7800)
#define gZclAttrSmplMetBISCurrNoTier7Block10SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x7900)
#define gZclAttrSmplMetBISCurrNoTier7Block11SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x7A00)
#define gZclAttrSmplMetBISCurrNoTier7Block12SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x7B00)
#define gZclAttrSmplMetBISCurrNoTier7Block13SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x7C00)
#define gZclAttrSmplMetBISCurrNoTier7Block14SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x7D00)
#define gZclAttrSmplMetBISCurrNoTier7Block15SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x7E00)
#define gZclAttrSmplMetBISCurrNoTier7Block16SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x7F00)
#define gZclAttrSmplMetBISCurrNoTier8Block1SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x8000)
#define gZclAttrSmplMetBISCurrNoTier8Block2SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x8100)
#define gZclAttrSmplMetBISCurrNoTier8Block3SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x8200)
#define gZclAttrSmplMetBISCurrNoTier8Block4SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x8300)
#define gZclAttrSmplMetBISCurrNoTier8Block5SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x8400)
#define gZclAttrSmplMetBISCurrNoTier8Block6SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x8500)
#define gZclAttrSmplMetBISCurrNoTier8Block7SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x8600)
#define gZclAttrSmplMetBISCurrNoTier8Block8SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x8700)
#define gZclAttrSmplMetBISCurrNoTier8Block9SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x8800)
#define gZclAttrSmplMetBISCurrNoTier8Block10SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x8900)
#define gZclAttrSmplMetBISCurrNoTier8Block11SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x8A00)
#define gZclAttrSmplMetBISCurrNoTier8Block12SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x8B00)
#define gZclAttrSmplMetBISCurrNoTier8Block13SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x8C00)
#define gZclAttrSmplMetBISCurrNoTier8Block14SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x8D00)
#define gZclAttrSmplMetBISCurrNoTier8Block15SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x8E00)
#define gZclAttrSmplMetBISCurrNoTier8Block16SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x8F00)
#define gZclAttrSmplMetBISCurrNoTier9Block1SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x9000)
#define gZclAttrSmplMetBISCurrNoTier9Block2SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x9100)
#define gZclAttrSmplMetBISCurrNoTier9Block3SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x9200)
#define gZclAttrSmplMetBISCurrNoTier9Block4SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x9300)
#define gZclAttrSmplMetBISCurrNoTier9Block5SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x9400)
#define gZclAttrSmplMetBISCurrNoTier9Block6SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x9500)
#define gZclAttrSmplMetBISCurrNoTier9Block7SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x9600)
#define gZclAttrSmplMetBISCurrNoTier9Block8SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x9700)
#define gZclAttrSmplMetBISCurrNoTier9Block9SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x9800)
#define gZclAttrSmplMetBISCurrNoTier9Block10SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x9900)
#define gZclAttrSmplMetBISCurrNoTier9Block11SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x9A00)
#define gZclAttrSmplMetBISCurrNoTier9Block12SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x9B00)
#define gZclAttrSmplMetBISCurrNoTier9Block13SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x9C00)
#define gZclAttrSmplMetBISCurrNoTier9Block14SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x9D00)
#define gZclAttrSmplMetBISCurrNoTier9Block15SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x9E00)
#define gZclAttrSmplMetBISCurrNoTier9Block16SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x9F00)
#define gZclAttrSmplMetBISCurrNoTier10Block1SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xA000)
#define gZclAttrSmplMetBISCurrNoTier10Block2SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xA100)
#define gZclAttrSmplMetBISCurrNoTier10Block3SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xA200)
#define gZclAttrSmplMetBISCurrNoTier10Block4SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xA300)
#define gZclAttrSmplMetBISCurrNoTier10Block5SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xA400)
#define gZclAttrSmplMetBISCurrNoTier10Block6SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xA500)
#define gZclAttrSmplMetBISCurrNoTier10Block7SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xA600)
#define gZclAttrSmplMetBISCurrNoTier10Block8SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xA700)
#define gZclAttrSmplMetBISCurrNoTier10Block9SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xA800)
#define gZclAttrSmplMetBISCurrNoTier10Block10SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xA900)
#define gZclAttrSmplMetBISCurrNoTier10Block11SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xAA00)
#define gZclAttrSmplMetBISCurrNoTier10Block12SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xAB00)
#define gZclAttrSmplMetBISCurrNoTier10Block13SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xAC00)
#define gZclAttrSmplMetBISCurrNoTier10Block14SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xAD00)
#define gZclAttrSmplMetBISCurrNoTier10Block15SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xAE00)
#define gZclAttrSmplMetBISCurrNoTier10Block16SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xAF00)
#define gZclAttrSmplMetBISCurrNoTier11Block1SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xB000)
#define gZclAttrSmplMetBISCurrNoTier11Block2SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xB100)
#define gZclAttrSmplMetBISCurrNoTier11Block3SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xB200)
#define gZclAttrSmplMetBISCurrNoTier11Block4SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xB300)
#define gZclAttrSmplMetBISCurrNoTier11Block5SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xB400)
#define gZclAttrSmplMetBISCurrNoTier11Block6SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xB500)
#define gZclAttrSmplMetBISCurrNoTier11Block7SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xB600)
#define gZclAttrSmplMetBISCurrNoTier11Block8SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xB700)
#define gZclAttrSmplMetBISCurrNoTier11Block9SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xB800)
#define gZclAttrSmplMetBISCurrNoTier11Block10SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xB900)
#define gZclAttrSmplMetBISCurrNoTier11Block11SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xBA00)
#define gZclAttrSmplMetBISCurrNoTier11Block12SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xBB00)
#define gZclAttrSmplMetBISCurrNoTier11Block13SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xBC00)
#define gZclAttrSmplMetBISCurrNoTier11Block14SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xBD00)
#define gZclAttrSmplMetBISCurrNoTier11Block15SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xBE00)
#define gZclAttrSmplMetBISCurrNoTier11Block16SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xBF00)
#define gZclAttrSmplMetBISCurrNoTier12Block1SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xC000)
#define gZclAttrSmplMetBISCurrNoTier12Block2SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xC100)
#define gZclAttrSmplMetBISCurrNoTier12Block3SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xC200)
#define gZclAttrSmplMetBISCurrNoTier12Block4SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xC300)
#define gZclAttrSmplMetBISCurrNoTier12Block5SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xC400)
#define gZclAttrSmplMetBISCurrNoTier12Block6SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xC500)
#define gZclAttrSmplMetBISCurrNoTier12Block7SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xC600)
#define gZclAttrSmplMetBISCurrNoTier12Block8SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xC700)
#define gZclAttrSmplMetBISCurrNoTier12Block9SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xC800)
#define gZclAttrSmplMetBISCurrNoTier12Block10SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xC900)
#define gZclAttrSmplMetBISCurrNoTier12Block11SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xCA00)
#define gZclAttrSmplMetBISCurrNoTier12Block12SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xCB00)
#define gZclAttrSmplMetBISCurrNoTier12Block13SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xCC00)
#define gZclAttrSmplMetBISCurrNoTier12Block14SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xCD00)
#define gZclAttrSmplMetBISCurrNoTier12Block15SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xCE00)
#define gZclAttrSmplMetBISCurrNoTier12Block16SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xCF00)
#define gZclAttrSmplMetBISCurrNoTier13Block1SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xD000)
#define gZclAttrSmplMetBISCurrNoTier13Block2SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xD100)
#define gZclAttrSmplMetBISCurrNoTier13Block3SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xD200)
#define gZclAttrSmplMetBISCurrNoTier13Block4SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xD300)
#define gZclAttrSmplMetBISCurrNoTier13Block5SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xD400)
#define gZclAttrSmplMetBISCurrNoTier13Block6SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xD500)
#define gZclAttrSmplMetBISCurrNoTier13Block7SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xD600)
#define gZclAttrSmplMetBISCurrNoTier13Block8SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xD700)
#define gZclAttrSmplMetBISCurrNoTier13Block9SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xD800)
#define gZclAttrSmplMetBISCurrNoTier13Block10SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xD900)
#define gZclAttrSmplMetBISCurrNoTier13Block11SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xDA00)
#define gZclAttrSmplMetBISCurrNoTier13Block12SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xDB00)
#define gZclAttrSmplMetBISCurrNoTier13Block13SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xDC00)
#define gZclAttrSmplMetBISCurrNoTier13Block14SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xDD00)
#define gZclAttrSmplMetBISCurrNoTier13Block15SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xDE00)
#define gZclAttrSmplMetBISCurrNoTier13Block16SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xDF00)
#define gZclAttrSmplMetBISCurrNoTier14Block1SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xE000)
#define gZclAttrSmplMetBISCurrNoTier14Block2SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xE100)
#define gZclAttrSmplMetBISCurrNoTier14Block3SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xE200)
#define gZclAttrSmplMetBISCurrNoTier14Block4SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xE300)
#define gZclAttrSmplMetBISCurrNoTier14Block5SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xE400)
#define gZclAttrSmplMetBISCurrNoTier14Block6SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xE500)
#define gZclAttrSmplMetBISCurrNoTier14Block7SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xE600)
#define gZclAttrSmplMetBISCurrNoTier14Block8SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xE700)
#define gZclAttrSmplMetBISCurrNoTier14Block9SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xE800)
#define gZclAttrSmplMetBISCurrNoTier14Block10SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xE900)
#define gZclAttrSmplMetBISCurrNoTier14Block11SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xEA00)
#define gZclAttrSmplMetBISCurrNoTier14Block12SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xEB00)
#define gZclAttrSmplMetBISCurrNoTier14Block13SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xEC00)
#define gZclAttrSmplMetBISCurrNoTier14Block14SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xED00)
#define gZclAttrSmplMetBISCurrNoTier14Block15SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xEE00)
#define gZclAttrSmplMetBISCurrNoTier14Block16SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xEF00)
#define gZclAttrSmplMetBISCurrNoTier15Block1SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xF000)
#define gZclAttrSmplMetBISCurrNoTier15Block2SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xF100)
#define gZclAttrSmplMetBISCurrNoTier15Block3SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xF200)
#define gZclAttrSmplMetBISCurrNoTier15Block4SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xF300)
#define gZclAttrSmplMetBISCurrNoTier15Block5SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xF400)
#define gZclAttrSmplMetBISCurrNoTier15Block6SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xF500)
#define gZclAttrSmplMetBISCurrNoTier15Block7SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xF600)
#define gZclAttrSmplMetBISCurrNoTier15Block8SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xF700)
#define gZclAttrSmplMetBISCurrNoTier15Block9SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xF800)
#define gZclAttrSmplMetBISCurrNoTier15Block10SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xF900)
#define gZclAttrSmplMetBISCurrNoTier15Block11SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xFA00)
#define gZclAttrSmplMetBISCurrNoTier15Block12SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xFB00)
#define gZclAttrSmplMetBISCurrNoTier15Block13SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xFC00)
#define gZclAttrSmplMetBISCurrNoTier15Block14SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xFD00)
#define gZclAttrSmplMetBISCurrNoTier15Block15SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xFE00)
#define gZclAttrSmplMetBISCurrNoTier15Block16SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0xFF00)

/* Alarm Attribute Set */
#define gZclAttrSmplMetASGenericAlarmMask_c                  (gZclAttrSmplMetAlarmsSet_c | 0x0000)
#define gZclAttrSmplMetASElectricityAlarmMask_c              (gZclAttrSmplMetAlarmsSet_c | 0x0100)
#define gZclAttrSmplMetASGenericFlowPressureAlarmMask_c      (gZclAttrSmplMetAlarmsSet_c | 0x0200)
#define gZclAttrSmplMetASWaterSpecificAlarmMask_c            (gZclAttrSmplMetAlarmsSet_c | 0x0300)
#define gZclAttrSmplMetASHeatAndCoolingSpecificAlarmMask_c   (gZclAttrSmplMetAlarmsSet_c | 0x0400)
#define gZclAttrSmplMetASGasSpecificAlarmMask_c              (gZclAttrSmplMetAlarmsSet_c | 0x0500)

/* Block Information Attribute Set Received*/
#define gZclAttrSmplMetBISCurrNoTierBlock1SummationReceived_c            (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0000)
#define gZclAttrSmplMetBISCurrNoTierBlock2SummationReceived_c            (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0100)
#define gZclAttrSmplMetBISCurrNoTierBlock3SummationReceived_c            (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0200)
#define gZclAttrSmplMetBISCurrNoTierBlock4SummationReceived_c            (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0300)
#define gZclAttrSmplMetBISCurrNoTierBlock5SummationReceived_c            (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0400)
#define gZclAttrSmplMetBISCurrNoTierBlock6SummationReceived_c            (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0500)
#define gZclAttrSmplMetBISCurrNoTierBlock7SummationReceived_c            (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0600)
#define gZclAttrSmplMetBISCurrNoTierBlock8SummationReceived_c            (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0700)
#define gZclAttrSmplMetBISCurrNoTierBlock9SummationReceived_c            (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0800)
#define gZclAttrSmplMetBISCurrNoTierBlock10SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0900)
#define gZclAttrSmplMetBISCurrNoTierBlock11SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0A00)
#define gZclAttrSmplMetBISCurrNoTierBlock12SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0B00)
#define gZclAttrSmplMetBISCurrNoTierBlock13SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0C00)
#define gZclAttrSmplMetBISCurrNoTierBlock14SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0D00)
#define gZclAttrSmplMetBISCurrNoTierBlock15SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0E00)
#define gZclAttrSmplMetBISCurrNoTierBlock16SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0F00)
#define gZclAttrSmplMetBISCurrNoTier1Block1SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x1000)
#define gZclAttrSmplMetBISCurrNoTier1Block2SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x1100)
#define gZclAttrSmplMetBISCurrNoTier1Block3SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x1200)
#define gZclAttrSmplMetBISCurrNoTier1Block4SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x1300)
#define gZclAttrSmplMetBISCurrNoTier1Block5SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x1400)
#define gZclAttrSmplMetBISCurrNoTier1Block6SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x1500)
#define gZclAttrSmplMetBISCurrNoTier1Block7SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x1600)
#define gZclAttrSmplMetBISCurrNoTier1Block8SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x1700)
#define gZclAttrSmplMetBISCurrNoTier1Block9SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x1800)
#define gZclAttrSmplMetBISCurrNoTier1Block10SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x1900)
#define gZclAttrSmplMetBISCurrNoTier1Block11SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x1A00)
#define gZclAttrSmplMetBISCurrNoTier1Block12SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x1B00)
#define gZclAttrSmplMetBISCurrNoTier1Block13SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x1C00)
#define gZclAttrSmplMetBISCurrNoTier1Block14SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x1D00)
#define gZclAttrSmplMetBISCurrNoTier1Block15SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x1E00)
#define gZclAttrSmplMetBISCurrNoTier1Block16SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x1F00)
#define gZclAttrSmplMetBISCurrNoTier2Block1SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x2000)
#define gZclAttrSmplMetBISCurrNoTier2Block2SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x2100)
#define gZclAttrSmplMetBISCurrNoTier2Block3SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x2200)
#define gZclAttrSmplMetBISCurrNoTier2Block4SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x2300)
#define gZclAttrSmplMetBISCurrNoTier2Block5SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x2400)
#define gZclAttrSmplMetBISCurrNoTier2Block6SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x2500)
#define gZclAttrSmplMetBISCurrNoTier2Block7SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x2600)
#define gZclAttrSmplMetBISCurrNoTier2Block8SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x2700)
#define gZclAttrSmplMetBISCurrNoTier2Block9SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x2800)
#define gZclAttrSmplMetBISCurrNoTier2Block10SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x2900)
#define gZclAttrSmplMetBISCurrNoTier2Block11SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x2A00)
#define gZclAttrSmplMetBISCurrNoTier2Block12SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x2B00)
#define gZclAttrSmplMetBISCurrNoTier2Block13SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x2C00)
#define gZclAttrSmplMetBISCurrNoTier2Block14SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x2D00)
#define gZclAttrSmplMetBISCurrNoTier2Block15SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x2E00)
#define gZclAttrSmplMetBISCurrNoTier2Block16SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x2F00)
#define gZclAttrSmplMetBISCurrNoTier3Block1SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x3000)
#define gZclAttrSmplMetBISCurrNoTier3Block2SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x3100)
#define gZclAttrSmplMetBISCurrNoTier3Block3SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x3200)
#define gZclAttrSmplMetBISCurrNoTier3Block4SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x3300)
#define gZclAttrSmplMetBISCurrNoTier3Block5SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x3400)
#define gZclAttrSmplMetBISCurrNoTier3Block6SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x3500)
#define gZclAttrSmplMetBISCurrNoTier3Block7SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x3600)
#define gZclAttrSmplMetBISCurrNoTier3Block8SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x3700)
#define gZclAttrSmplMetBISCurrNoTier3Block9SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x3800)
#define gZclAttrSmplMetBISCurrNoTier3Block10SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x3900)
#define gZclAttrSmplMetBISCurrNoTier3Block11SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x3A00)
#define gZclAttrSmplMetBISCurrNoTier3Block12SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x3B00)
#define gZclAttrSmplMetBISCurrNoTier3Block13SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x3C00)
#define gZclAttrSmplMetBISCurrNoTier3Block14SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x3D00)
#define gZclAttrSmplMetBISCurrNoTier3Block15SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x3E00)
#define gZclAttrSmplMetBISCurrNoTier3Block16SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x3F00)
#define gZclAttrSmplMetBISCurrNoTier4Block1SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x4000)
#define gZclAttrSmplMetBISCurrNoTier4Block2SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x4100)
#define gZclAttrSmplMetBISCurrNoTier4Block3SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x4200)
#define gZclAttrSmplMetBISCurrNoTier4Block4SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x4300)
#define gZclAttrSmplMetBISCurrNoTier4Block5SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x4400)
#define gZclAttrSmplMetBISCurrNoTier4Block6SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x4500)
#define gZclAttrSmplMetBISCurrNoTier4Block7SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x4600)
#define gZclAttrSmplMetBISCurrNoTier4Block8SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x4700)
#define gZclAttrSmplMetBISCurrNoTier4Block9SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x4800)
#define gZclAttrSmplMetBISCurrNoTier4Block10SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x4900)
#define gZclAttrSmplMetBISCurrNoTier4Block11SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x4A00)
#define gZclAttrSmplMetBISCurrNoTier4Block12SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x4B00)
#define gZclAttrSmplMetBISCurrNoTier4Block13SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x4C00)
#define gZclAttrSmplMetBISCurrNoTier4Block14SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x4D00)
#define gZclAttrSmplMetBISCurrNoTier4Block15SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x4E00)
#define gZclAttrSmplMetBISCurrNoTier4Block16SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x4F00)
#define gZclAttrSmplMetBISCurrNoTier5Block1SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x5000)
#define gZclAttrSmplMetBISCurrNoTier5Block2SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x5100)
#define gZclAttrSmplMetBISCurrNoTier5Block3SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x5200)
#define gZclAttrSmplMetBISCurrNoTier5Block4SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x5300)
#define gZclAttrSmplMetBISCurrNoTier5Block5SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x5400)
#define gZclAttrSmplMetBISCurrNoTier5Block6SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x5500)
#define gZclAttrSmplMetBISCurrNoTier5Block7SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x5600)
#define gZclAttrSmplMetBISCurrNoTier5Block8SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x5700)
#define gZclAttrSmplMetBISCurrNoTier5Block9SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x5800)
#define gZclAttrSmplMetBISCurrNoTier5Block10SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x5900)
#define gZclAttrSmplMetBISCurrNoTier5Block11SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x5A00)
#define gZclAttrSmplMetBISCurrNoTier5Block12SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x5B00)
#define gZclAttrSmplMetBISCurrNoTier5Block13SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x5C00)
#define gZclAttrSmplMetBISCurrNoTier5Block14SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x5D00)
#define gZclAttrSmplMetBISCurrNoTier5Block15SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x5E00)
#define gZclAttrSmplMetBISCurrNoTier5Block16SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x5F00)
#define gZclAttrSmplMetBISCurrNoTier6Block1SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x6000)
#define gZclAttrSmplMetBISCurrNoTier6Block2SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x6100)
#define gZclAttrSmplMetBISCurrNoTier6Block3SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x6200)
#define gZclAttrSmplMetBISCurrNoTier6Block4SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x6300)
#define gZclAttrSmplMetBISCurrNoTier6Block5SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x6400)
#define gZclAttrSmplMetBISCurrNoTier6Block6SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x6500)
#define gZclAttrSmplMetBISCurrNoTier6Block7SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x6600)
#define gZclAttrSmplMetBISCurrNoTier6Block8SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x6700)
#define gZclAttrSmplMetBISCurrNoTier6Block9SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x6800)
#define gZclAttrSmplMetBISCurrNoTier6Block10SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x6900)
#define gZclAttrSmplMetBISCurrNoTier6Block11SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x6A00)
#define gZclAttrSmplMetBISCurrNoTier6Block12SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x6B00)
#define gZclAttrSmplMetBISCurrNoTier6Block13SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x6C00)
#define gZclAttrSmplMetBISCurrNoTier6Block14SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x6D00)
#define gZclAttrSmplMetBISCurrNoTier6Block15SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x6E00)
#define gZclAttrSmplMetBISCurrNoTier6Block16SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x6F00)
#define gZclAttrSmplMetBISCurrNoTier7Block1SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x7000)
#define gZclAttrSmplMetBISCurrNoTier7Block2SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x7100)
#define gZclAttrSmplMetBISCurrNoTier7Block3SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x7200)
#define gZclAttrSmplMetBISCurrNoTier7Block4SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x7300)
#define gZclAttrSmplMetBISCurrNoTier7Block5SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x7400)
#define gZclAttrSmplMetBISCurrNoTier7Block6SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x7500)
#define gZclAttrSmplMetBISCurrNoTier7Block7SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x7600)
#define gZclAttrSmplMetBISCurrNoTier7Block8SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x7700)
#define gZclAttrSmplMetBISCurrNoTier7Block9SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x7800)
#define gZclAttrSmplMetBISCurrNoTier7Block10SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x7900)
#define gZclAttrSmplMetBISCurrNoTier7Block11SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x7A00)
#define gZclAttrSmplMetBISCurrNoTier7Block12SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x7B00)
#define gZclAttrSmplMetBISCurrNoTier7Block13SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x7C00)
#define gZclAttrSmplMetBISCurrNoTier7Block14SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x7D00)
#define gZclAttrSmplMetBISCurrNoTier7Block15SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x7E00)
#define gZclAttrSmplMetBISCurrNoTier7Block16SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x7F00)
#define gZclAttrSmplMetBISCurrNoTier8Block1SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x8000)
#define gZclAttrSmplMetBISCurrNoTier8Block2SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x8100)
#define gZclAttrSmplMetBISCurrNoTier8Block3SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x8200)
#define gZclAttrSmplMetBISCurrNoTier8Block4SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x8300)
#define gZclAttrSmplMetBISCurrNoTier8Block5SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x8400)
#define gZclAttrSmplMetBISCurrNoTier8Block6SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x8500)
#define gZclAttrSmplMetBISCurrNoTier8Block7SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x8600)
#define gZclAttrSmplMetBISCurrNoTier8Block8SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x8700)
#define gZclAttrSmplMetBISCurrNoTier8Block9SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x8800)
#define gZclAttrSmplMetBISCurrNoTier8Block10SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x8900)
#define gZclAttrSmplMetBISCurrNoTier8Block11SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x8A00)
#define gZclAttrSmplMetBISCurrNoTier8Block12SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x8B00)
#define gZclAttrSmplMetBISCurrNoTier8Block13SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x8C00)
#define gZclAttrSmplMetBISCurrNoTier8Block14SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x8D00)
#define gZclAttrSmplMetBISCurrNoTier8Block15SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x8E00)
#define gZclAttrSmplMetBISCurrNoTier8Block16SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x8F00)
#define gZclAttrSmplMetBISCurrNoTier9Block1SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x9000)
#define gZclAttrSmplMetBISCurrNoTier9Block2SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x9100)
#define gZclAttrSmplMetBISCurrNoTier9Block3SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x9200)
#define gZclAttrSmplMetBISCurrNoTier9Block4SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x9300)
#define gZclAttrSmplMetBISCurrNoTier9Block5SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x9400)
#define gZclAttrSmplMetBISCurrNoTier9Block6SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x9500)
#define gZclAttrSmplMetBISCurrNoTier9Block7SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x9600)
#define gZclAttrSmplMetBISCurrNoTier9Block8SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x9700)
#define gZclAttrSmplMetBISCurrNoTier9Block9SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x9800)
#define gZclAttrSmplMetBISCurrNoTier9Block10SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x9900)
#define gZclAttrSmplMetBISCurrNoTier9Block11SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x9A00)
#define gZclAttrSmplMetBISCurrNoTier9Block12SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x9B00)
#define gZclAttrSmplMetBISCurrNoTier9Block13SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x9C00)
#define gZclAttrSmplMetBISCurrNoTier9Block14SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x9D00)
#define gZclAttrSmplMetBISCurrNoTier9Block15SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x9E00)
#define gZclAttrSmplMetBISCurrNoTier9Block16SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x9F00)
#define gZclAttrSmplMetBISCurrNoTier10Block1SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xA000)
#define gZclAttrSmplMetBISCurrNoTier10Block2SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xA100)
#define gZclAttrSmplMetBISCurrNoTier10Block3SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xA200)
#define gZclAttrSmplMetBISCurrNoTier10Block4SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xA300)
#define gZclAttrSmplMetBISCurrNoTier10Block5SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xA400)
#define gZclAttrSmplMetBISCurrNoTier10Block6SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xA500)
#define gZclAttrSmplMetBISCurrNoTier10Block7SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xA600)
#define gZclAttrSmplMetBISCurrNoTier10Block8SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xA700)
#define gZclAttrSmplMetBISCurrNoTier10Block9SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xA800)
#define gZclAttrSmplMetBISCurrNoTier10Block10SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xA900)
#define gZclAttrSmplMetBISCurrNoTier10Block11SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xAA00)
#define gZclAttrSmplMetBISCurrNoTier10Block12SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xAB00)
#define gZclAttrSmplMetBISCurrNoTier10Block13SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xAC00)
#define gZclAttrSmplMetBISCurrNoTier10Block14SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xAD00)
#define gZclAttrSmplMetBISCurrNoTier10Block15SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xAE00)
#define gZclAttrSmplMetBISCurrNoTier10Block16SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xAF00)
#define gZclAttrSmplMetBISCurrNoTier11Block1SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xB000)
#define gZclAttrSmplMetBISCurrNoTier11Block2SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xB100)
#define gZclAttrSmplMetBISCurrNoTier11Block3SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xB200)
#define gZclAttrSmplMetBISCurrNoTier11Block4SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xB300)
#define gZclAttrSmplMetBISCurrNoTier11Block5SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xB400)
#define gZclAttrSmplMetBISCurrNoTier11Block6SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xB500)
#define gZclAttrSmplMetBISCurrNoTier11Block7SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xB600)
#define gZclAttrSmplMetBISCurrNoTier11Block8SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xB700)
#define gZclAttrSmplMetBISCurrNoTier11Block9SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xB800)
#define gZclAttrSmplMetBISCurrNoTier11Block10SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xB900)
#define gZclAttrSmplMetBISCurrNoTier11Block11SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xBA00)
#define gZclAttrSmplMetBISCurrNoTier11Block12SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xBB00)
#define gZclAttrSmplMetBISCurrNoTier11Block13SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xBC00)
#define gZclAttrSmplMetBISCurrNoTier11Block14SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xBD00)
#define gZclAttrSmplMetBISCurrNoTier11Block15SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xBE00)
#define gZclAttrSmplMetBISCurrNoTier11Block16SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xBF00)
#define gZclAttrSmplMetBISCurrNoTier12Block1SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xC000)
#define gZclAttrSmplMetBISCurrNoTier12Block2SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xC100)
#define gZclAttrSmplMetBISCurrNoTier12Block3SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xC200)
#define gZclAttrSmplMetBISCurrNoTier12Block4SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xC300)
#define gZclAttrSmplMetBISCurrNoTier12Block5SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xC400)
#define gZclAttrSmplMetBISCurrNoTier12Block6SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xC500)
#define gZclAttrSmplMetBISCurrNoTier12Block7SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xC600)
#define gZclAttrSmplMetBISCurrNoTier12Block8SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xC700)
#define gZclAttrSmplMetBISCurrNoTier12Block9SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xC800)
#define gZclAttrSmplMetBISCurrNoTier12Block10SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xC900)
#define gZclAttrSmplMetBISCurrNoTier12Block11SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xCA00)
#define gZclAttrSmplMetBISCurrNoTier12Block12SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xCB00)
#define gZclAttrSmplMetBISCurrNoTier12Block13SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xCC00)
#define gZclAttrSmplMetBISCurrNoTier12Block14SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xCD00)
#define gZclAttrSmplMetBISCurrNoTier12Block15SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xCE00)
#define gZclAttrSmplMetBISCurrNoTier12Block16SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xCF00)
#define gZclAttrSmplMetBISCurrNoTier13Block1SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xD000)
#define gZclAttrSmplMetBISCurrNoTier13Block2SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xD100)
#define gZclAttrSmplMetBISCurrNoTier13Block3SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xD200)
#define gZclAttrSmplMetBISCurrNoTier13Block4SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xD300)
#define gZclAttrSmplMetBISCurrNoTier13Block5SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xD400)
#define gZclAttrSmplMetBISCurrNoTier13Block6SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xD500)
#define gZclAttrSmplMetBISCurrNoTier13Block7SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xD600)
#define gZclAttrSmplMetBISCurrNoTier13Block8SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xD700)
#define gZclAttrSmplMetBISCurrNoTier13Block9SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xD800)
#define gZclAttrSmplMetBISCurrNoTier13Block10SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xD900)
#define gZclAttrSmplMetBISCurrNoTier13Block11SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xDA00)
#define gZclAttrSmplMetBISCurrNoTier13Block12SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xDB00)
#define gZclAttrSmplMetBISCurrNoTier13Block13SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xDC00)
#define gZclAttrSmplMetBISCurrNoTier13Block14SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xDD00)
#define gZclAttrSmplMetBISCurrNoTier13Block15SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xDE00)
#define gZclAttrSmplMetBISCurrNoTier13Block16SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xDF00)
#define gZclAttrSmplMetBISCurrNoTier14Block1SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xE000)
#define gZclAttrSmplMetBISCurrNoTier14Block2SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xE100)
#define gZclAttrSmplMetBISCurrNoTier14Block3SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xE200)
#define gZclAttrSmplMetBISCurrNoTier14Block4SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xE300)
#define gZclAttrSmplMetBISCurrNoTier14Block5SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xE400)
#define gZclAttrSmplMetBISCurrNoTier14Block6SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xE500)
#define gZclAttrSmplMetBISCurrNoTier14Block7SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xE600)
#define gZclAttrSmplMetBISCurrNoTier14Block8SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xE700)
#define gZclAttrSmplMetBISCurrNoTier14Block9SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xE800)
#define gZclAttrSmplMetBISCurrNoTier14Block10SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xE900)
#define gZclAttrSmplMetBISCurrNoTier14Block11SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xEA00)
#define gZclAttrSmplMetBISCurrNoTier14Block12SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xEB00)
#define gZclAttrSmplMetBISCurrNoTier14Block13SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xEC00)
#define gZclAttrSmplMetBISCurrNoTier14Block14SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xED00)
#define gZclAttrSmplMetBISCurrNoTier14Block15SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xEE00)
#define gZclAttrSmplMetBISCurrNoTier14Block16SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xEF00)
#define gZclAttrSmplMetBISCurrNoTier15Block1SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xF000)
#define gZclAttrSmplMetBISCurrNoTier15Block2SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xF100)
#define gZclAttrSmplMetBISCurrNoTier15Block3SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xF200)
#define gZclAttrSmplMetBISCurrNoTier15Block4SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xF300)
#define gZclAttrSmplMetBISCurrNoTier15Block5SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xF400)
#define gZclAttrSmplMetBISCurrNoTier15Block6SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xF500)
#define gZclAttrSmplMetBISCurrNoTier15Block7SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xF600)
#define gZclAttrSmplMetBISCurrNoTier15Block8SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xF700)
#define gZclAttrSmplMetBISCurrNoTier15Block9SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xF800)
#define gZclAttrSmplMetBISCurrNoTier15Block10SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xF900)
#define gZclAttrSmplMetBISCurrNoTier15Block11SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xFA00)
#define gZclAttrSmplMetBISCurrNoTier15Block12SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xFB00)
#define gZclAttrSmplMetBISCurrNoTier15Block13SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xFC00)
#define gZclAttrSmplMetBISCurrNoTier15Block14SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xFD00)
#define gZclAttrSmplMetBISCurrNoTier15Block15SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xFE00)
#define gZclAttrSmplMetBISCurrNoTier15Block16SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0xFF00)

/* Billing Attribute Set */
#define gZclAttrSmplMetBillingBillToDate_c                               (gZclAttrSmplMetBillingSet_c | 0x0000)
#define gZclAttrSmplMetBillingBillToDateTimeStamp_c                      (gZclAttrSmplMetBillingSet_c | 0x0100)
#define gZclAttrSmplMetBillingProjectedBill_c                            (gZclAttrSmplMetBillingSet_c | 0x0200)
#define gZclAttrSmplMetBillingProjectedBillTimeStamp_c                   (gZclAttrSmplMetBillingSet_c | 0x0300)

#else /* #if gBigEndian_c */

/*ReadInformationSet*/
#define gZclAttrSmplMetRISCurrSummDlvrd_c                       (gZclAttrSmplMetReadInfoSet_c | 0x0000) /*M*/
#define gZclAttrSmplMetRISCurrSummRcvd_c                        (gZclAttrSmplMetReadInfoSet_c | 0x0001)
#define gZclAttrSmplMetRISCurrMaxDmndDlvrd_c                    (gZclAttrSmplMetReadInfoSet_c | 0x0002)
#define gZclAttrSmplMetRISCurrMaxDmndRcvd_c                     (gZclAttrSmplMetReadInfoSet_c | 0x0003)
#define gZclAttrSmplMetRISDFTSumm_c                             (gZclAttrSmplMetReadInfoSet_c | 0x0004)
#define gZclAttrSmplMetRISDailyFreezeTime_c                     (gZclAttrSmplMetReadInfoSet_c | 0x0005)
#define gZclAttrSmplMetRISPowerFactor_c                         (gZclAttrSmplMetReadInfoSet_c | 0x0006)
#define gZclAttrSmplMetRISReadingSnapShotTime_c                 (gZclAttrSmplMetReadInfoSet_c | 0x0007)
#define gZclAttrSmplMetRISCurrMaxDmndDlvrdTimer_c               (gZclAttrSmplMetReadInfoSet_c | 0x0008)
#define gZclAttrSmplMetRISCurrMaxDmndRcvdTime_c                 (gZclAttrSmplMetReadInfoSet_c | 0x0009)
#define gZclAttrSmplMetRISDefaultUpdatePeriod_c                 (gZclAttrSmplMetReadInfoSet_c | 0x000A)
#define gZclAttrSmplMetRISFastPollUpdatePeriod_c                (gZclAttrSmplMetReadInfoSet_c | 0x000B)
#define gZclAttrSmplMetRISCurrBlockPeriodConsumptionDelivered_c (gZclAttrSmplMetReadInfoSet_c | 0x000C)
#define gZclAttrSmplMetRISDailyConsumptionTarget_c              (gZclAttrSmplMetReadInfoSet_c | 0x000D)
#define gZclAttrSmplMetRISCurrBlock_c                           (gZclAttrSmplMetReadInfoSet_c | 0x000E)
#define gZclAttrSmplMetRISProfileIntPeriod_c                    (gZclAttrSmplMetReadInfoSet_c | 0x000F)
#define gZclAttrSmplMetRISIntReadReportingPeriod_c              (gZclAttrSmplMetReadInfoSet_c | 0x0010)
#define gZclAttrSmplMetRISPresetReadingTime_c                   (gZclAttrSmplMetReadInfoSet_c | 0x0011)
#define gZclAttrSmplMetRISVolumePerReport_c                     (gZclAttrSmplMetReadInfoSet_c | 0x0012)
#define gZclAttrSmplMetRISFlowRestriction_c                     (gZclAttrSmplMetReadInfoSet_c | 0x0013)
#define gZclAttrSmplMetRISSupplyStatus_c                        (gZclAttrSmplMetReadInfoSet_c | 0x0014)
#define gZclAttrSmplMetRISCurrInletEnergyCarrierSummation_c     (gZclAttrSmplMetReadInfoSet_c | 0x0015)
#define gZclAttrSmplMetRISCurrOutletEnergyCarrierSummation_c    (gZclAttrSmplMetReadInfoSet_c | 0x0016)
#define gZclAttrSmplMetRISInletTemp_c                           (gZclAttrSmplMetReadInfoSet_c | 0x0017)
#define gZclAttrSmplMetRISOutletTemp_c                          (gZclAttrSmplMetReadInfoSet_c | 0x0018)
#define gZclAttrSmplMetRISCtrlTemp_c                            (gZclAttrSmplMetReadInfoSet_c | 0x0019)
#define gZclAttrSmplMetRISCurrInletEnergyCarrierDemand_c        (gZclAttrSmplMetReadInfoSet_c | 0x001A)
#define gZclAttrSmplMetRISCurrOutletEnergyCarrierDemand_c       (gZclAttrSmplMetReadInfoSet_c | 0x001B)

/* Summation TOU Information Set */
#define gZclAttrSmplMetTOUCurrTier1SummDlvrd_c  (gZclAttrSmplMetTOUInfoSet_c | 0x0000)
#define gZclAttrSmplMetTOUCurrTier1SummRcvd_c   (gZclAttrSmplMetTOUInfoSet_c | 0x0001)
#define gZclAttrSmplMetTOUCurrTier2SummDlvrd_c  (gZclAttrSmplMetTOUInfoSet_c | 0x0002)
#define gZclAttrSmplMetTOUCurrTier2SummRcvd_c   (gZclAttrSmplMetTOUInfoSet_c | 0x0003)
#define gZclAttrSmplMetTOUCurrTier3SummDlvrd_c  (gZclAttrSmplMetTOUInfoSet_c | 0x0004)
#define gZclAttrSmplMetTOUCurrTier3SummRcvd_c   (gZclAttrSmplMetTOUInfoSet_c | 0x0005)
#define gZclAttrSmplMetTOUCurrTier4SummDlvrd_c  (gZclAttrSmplMetTOUInfoSet_c | 0x0006)
#define gZclAttrSmplMetTOUCurrTier4SummRcvd_c   (gZclAttrSmplMetTOUInfoSet_c | 0x0007)
#define gZclAttrSmplMetTOUCurrTier5SummDlvrd_c  (gZclAttrSmplMetTOUInfoSet_c | 0x0008)
#define gZclAttrSmplMetTOUCurrTier5SummRcvd_c   (gZclAttrSmplMetTOUInfoSet_c | 0x0009)
#define gZclAttrSmplMetTOUCurrTier6SummDlvrd_c  (gZclAttrSmplMetTOUInfoSet_c | 0x000A)
#define gZclAttrSmplMetTOUCurrTier6SummRcvd_c   (gZclAttrSmplMetTOUInfoSet_c | 0x000B)
#define gZclAttrSmplMetTOUCurrTier7SummDlvrd_c  (gZclAttrSmplMetTOUInfoSet_c | 0x000C)
#define gZclAttrSmplMetTOUCurrTier7SummRcvd_c   (gZclAttrSmplMetTOUInfoSet_c | 0x000D)
#define gZclAttrSmplMetTOUCurrTier8SummDlvrd_c  (gZclAttrSmplMetTOUInfoSet_c | 0x000E)
#define gZclAttrSmplMetTOUCurrTier8SummRcvd_c   (gZclAttrSmplMetTOUInfoSet_c | 0x000F)
#define gZclAttrSmplMetTOUCurrTier9SummDlvrd_c  (gZclAttrSmplMetTOUInfoSet_c | 0x0010)
#define gZclAttrSmplMetTOUCurrTier9SummRcvd_c   (gZclAttrSmplMetTOUInfoSet_c | 0x0011)
#define gZclAttrSmplMetTOUCurrTier10SummDlvrd_c (gZclAttrSmplMetTOUInfoSet_c | 0x0012)
#define gZclAttrSmplMetTOUCurrTier10SummRcvd_c  (gZclAttrSmplMetTOUInfoSet_c | 0x0013)
#define gZclAttrSmplMetTOUCurrTier11SummDlvrd_c (gZclAttrSmplMetTOUInfoSet_c | 0x0014)
#define gZclAttrSmplMetTOUCurrTier11SummRcvd_c  (gZclAttrSmplMetTOUInfoSet_c | 0x0015)
#define gZclAttrSmplMetTOUCurrTier12SummDlvrd_c (gZclAttrSmplMetTOUInfoSet_c | 0x0016)
#define gZclAttrSmplMetTOUCurrTier12SummRcvd_c  (gZclAttrSmplMetTOUInfoSet_c | 0x0017)
#define gZclAttrSmplMetTOUCurrTier13SummDlvrd_c (gZclAttrSmplMetTOUInfoSet_c | 0x0018)
#define gZclAttrSmplMetTOUCurrTier13SummRcvd_c  (gZclAttrSmplMetTOUInfoSet_c | 0x0019)
#define gZclAttrSmplMetTOUCurrTier14SummDlvrd_c (gZclAttrSmplMetTOUInfoSet_c | 0x001A)
#define gZclAttrSmplMetTOUCurrTier14SummRcvd_c  (gZclAttrSmplMetTOUInfoSet_c | 0x001B)
#define gZclAttrSmplMetTOUCurrTier15SummDlvrd_c (gZclAttrSmplMetTOUInfoSet_c | 0x001C)
#define gZclAttrSmplMetTOUCurrTier15SummRcvd_c  (gZclAttrSmplMetTOUInfoSet_c | 0x001D)

/*Consumption Formating Set*/
#define gZclAttrSmplMetCFSUnitofMeasure_c                    (gZclAttrSmplMetConsmpFormatSet_c | 0x0000)  /* M */
#define gZclAttrSmplMetCFSMultiplier_c                       (gZclAttrSmplMetConsmpFormatSet_c | 0x0001)
#define gZclAttrSmplMetCFSDivisor_c                          (gZclAttrSmplMetConsmpFormatSet_c | 0x0002)
#define gZclAttrSmplMetCFSSummFormat_c                       (gZclAttrSmplMetConsmpFormatSet_c | 0x0003)  /* M */
#define gZclAttrSmplMetCFSDmndFormat_c                       (gZclAttrSmplMetConsmpFormatSet_c | 0x0004)
#define gZclAttrSmplMetCFSHistoricalConsmpFormat_c           (gZclAttrSmplMetConsmpFormatSet_c | 0x0005)
#define gZclAttrSmplMetCFSMetDevType_c                       (gZclAttrSmplMetConsmpFormatSet_c | 0x0006) /* M */
#define gZclAttrSmplMetCFSSiteID_c                           (gZclAttrSmplMetConsmpFormatSet_c | 0x0007) 
#define gZclAttrSmplMetCFSMeterSerialNumber_c                (gZclAttrSmplMetConsmpFormatSet_c | 0x0008) 
#define gZclAttrSmplMetCFSEnergyCarrierUnitOfMeasure_c       (gZclAttrSmplMetConsmpFormatSet_c | 0x0009)
#define gZclAttrSmplMetCFSEnergyCarrierSummationFormatting_c (gZclAttrSmplMetConsmpFormatSet_c | 0x000A)
#define gZclAttrSmplMetCFSEnergyCarrierDemandFormatting_c    (gZclAttrSmplMetConsmpFormatSet_c | 0x000B)
#define gZclAttrSmplMetCFSTempUnitOfMeasure_c                (gZclAttrSmplMetConsmpFormatSet_c | 0x000C)
#define gZclAttrSmplMetCFSTempFormatting_c                   (gZclAttrSmplMetConsmpFormatSet_c | 0x000D)

/* ESP Historical Consumption */
#define gZclAttrSmplMetEHCInstantaneousDmnd_c                (gZclAttrSmplMetESPHistoricalConsmpSet_c | 0x0000)
#define gZclAttrSmplMetEHCCurrDayConsmpDlvrd_c               (gZclAttrSmplMetESPHistoricalConsmpSet_c | 0x0001)
#define gZclAttrSmplMetEHCCurrDayConsmpRcvd_c                (gZclAttrSmplMetESPHistoricalConsmpSet_c | 0x0002)
#define gZclAttrSmplMetEHCPreviousDayConsmpDlvrd_c           (gZclAttrSmplMetESPHistoricalConsmpSet_c | 0x0003)
#define gZclAttrSmplMetEHCPreviousDayConsmpRcvd_c            (gZclAttrSmplMetESPHistoricalConsmpSet_c | 0x0004)
#define gZclAttrSmplMetEHCCurrPrtlProfIntrvStartTimeDlvrd_c  (gZclAttrSmplMetESPHistoricalConsmpSet_c | 0x0005)
#define gZclAttrSmplMetEHCCurrPrtlProfIntrvStartTimeRcvd_c   (gZclAttrSmplMetESPHistoricalConsmpSet_c | 0x0006)
#define gZclAttrSmplMetEHCCurrPrtlProfIntrvValueDlvrd_c      (gZclAttrSmplMetESPHistoricalConsmpSet_c | 0x0007)
#define gZclAttrSmplMetEHCCurrPrtlProfIntrvValueRcvd_c       (gZclAttrSmplMetESPHistoricalConsmpSet_c | 0x0008)
#define gZclAttrSmplMetEHCCurrDayMaxPressure_c               (gZclAttrSmplMetESPHistoricalConsmpSet_c | 0x0009)
#define gZclAttrSmplMetEHCCurrDayMinPressure_c               (gZclAttrSmplMetESPHistoricalConsmpSet_c | 0x000A)
#define gZclAttrSmplMetEHCPrevDayMaxPressure_c               (gZclAttrSmplMetESPHistoricalConsmpSet_c | 0x000B)
#define gZclAttrSmplMetEHCPrevDayMinPressure_c               (gZclAttrSmplMetESPHistoricalConsmpSet_c | 0x000C)
#define gZclAttrSmplMetEHCCurrDayMaxDemand_c                 (gZclAttrSmplMetESPHistoricalConsmpSet_c | 0x000D)
#define gZclAttrSmplMetEHCPrevDayMaxDemand_c                 (gZclAttrSmplMetESPHistoricalConsmpSet_c | 0x000E)
#define gZclAttrSmplMetEHCCurrMonthMaxDemand_c               (gZclAttrSmplMetESPHistoricalConsmpSet_c | 0x000F)
#define gZclAttrSmplMetEHCCurrYearMaxDemand_c                (gZclAttrSmplMetESPHistoricalConsmpSet_c | 0x0010)
#define gZclAttrSmplMetEHCCurrDayMaxEnergyCarrierDemand_c    (gZclAttrSmplMetESPHistoricalConsmpSet_c | 0x0011)
#define gZclAttrSmplMetEHCPrevDayMaxEnergyCarrierDemand_c    (gZclAttrSmplMetESPHistoricalConsmpSet_c | 0x0012)
#define gZclAttrSmplMetEHCCurrMonthMaxEnergyCarrierDemand_c  (gZclAttrSmplMetESPHistoricalConsmpSet_c | 0x0013)
#define gZclAttrSmplMetEHCCurrMonthMinEnergyCarrierDemand_c  (gZclAttrSmplMetESPHistoricalConsmpSet_c | 0x0014)
#define gZclAttrSmplMetEHCCurrYearMaxEnergyCarrierDemand_c   (gZclAttrSmplMetESPHistoricalConsmpSet_c | 0x0015)
#define gZclAttrSmplMetEHCCurrYearMinEnergyCarrierDemand_c   (gZclAttrSmplMetESPHistoricalConsmpSet_c | 0x0016)

/*Meter Status Set*/
#define gZclAttrSmplMetMSStatus_c                 (gZclAttrSmplMetMeterStatusSet_c | 0x0000)  /* M */
#define gZclAttrSmplMetMSSRemainingBatteryLife_c  (gZclAttrSmplMetMeterStatusSet_c | 0x0001)  
#define gZclAttrSmplMetMSSHoursInOperation_c      (gZclAttrSmplMetMeterStatusSet_c | 0x0002)  
#define gZclAttrSmplMetMSSHoursInFault_c          (gZclAttrSmplMetMeterStatusSet_c | 0x0003)  

/*Load Profile Configuration */
#define gZclAttrSmplMetLPCMaxNumOfPeriodsDlvrd_c       (gZclAttrSmplMetLdProfConfigurationSet_c | 0x0000)

/* Supply Limit Attribute Set */
#define gZclAttrSmplMetSLSCurrDmndDlvrd_c              (gZclAttrSmplMetSupplyLimitSet_c | 0x0000)
#define gZclAttrSmplMetSLSDmndLimit_c                  (gZclAttrSmplMetSupplyLimitSet_c | 0x0001)
#define gZclAttrSmplMetSLSDmndIntegrationPeriod_c      (gZclAttrSmplMetSupplyLimitSet_c | 0x0002)
#define gZclAttrSmplMetSLSNumOfDmndSubIntrvs_c         (gZclAttrSmplMetSupplyLimitSet_c | 0x0003)

/* Block Information Attribute Set Delivered */
#define gZclAttrSmplMetBISCurrNoTierBlock1SummationDelivered_c            (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0000)
#define gZclAttrSmplMetBISCurrNoTierBlock2SummationDelivered_c            (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0001)
#define gZclAttrSmplMetBISCurrNoTierBlock3SummationDelivered_c            (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0002)
#define gZclAttrSmplMetBISCurrNoTierBlock4SummationDelivered_c            (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0003)
#define gZclAttrSmplMetBISCurrNoTierBlock5SummationDelivered_c            (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0004)
#define gZclAttrSmplMetBISCurrNoTierBlock6SummationDelivered_c            (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0005)
#define gZclAttrSmplMetBISCurrNoTierBlock7SummationDelivered_c            (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0006)
#define gZclAttrSmplMetBISCurrNoTierBlock8SummationDelivered_c            (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0007)
#define gZclAttrSmplMetBISCurrNoTierBlock9SummationDelivered_c            (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0008)
#define gZclAttrSmplMetBISCurrNoTierBlock10SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0009)
#define gZclAttrSmplMetBISCurrNoTierBlock11SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x000A)
#define gZclAttrSmplMetBISCurrNoTierBlock12SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x000B)
#define gZclAttrSmplMetBISCurrNoTierBlock13SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x000C)
#define gZclAttrSmplMetBISCurrNoTierBlock14SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x000D)
#define gZclAttrSmplMetBISCurrNoTierBlock15SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x000E)
#define gZclAttrSmplMetBISCurrNoTierBlock16SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x000F)
#define gZclAttrSmplMetBISCurrNoTier1Block1SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0010)
#define gZclAttrSmplMetBISCurrNoTier1Block2SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0011)
#define gZclAttrSmplMetBISCurrNoTier1Block3SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0012)
#define gZclAttrSmplMetBISCurrNoTier1Block4SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0013)
#define gZclAttrSmplMetBISCurrNoTier1Block5SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0014)
#define gZclAttrSmplMetBISCurrNoTier1Block6SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0015)
#define gZclAttrSmplMetBISCurrNoTier1Block7SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0016)
#define gZclAttrSmplMetBISCurrNoTier1Block8SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0017)
#define gZclAttrSmplMetBISCurrNoTier1Block9SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0018)
#define gZclAttrSmplMetBISCurrNoTier1Block10SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0019)
#define gZclAttrSmplMetBISCurrNoTier1Block11SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x001A)
#define gZclAttrSmplMetBISCurrNoTier1Block12SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x001B)
#define gZclAttrSmplMetBISCurrNoTier1Block13SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x001C)
#define gZclAttrSmplMetBISCurrNoTier1Block14SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x001D)
#define gZclAttrSmplMetBISCurrNoTier1Block15SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x001E)
#define gZclAttrSmplMetBISCurrNoTier1Block16SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x001F)
#define gZclAttrSmplMetBISCurrNoTier2Block1SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0020)
#define gZclAttrSmplMetBISCurrNoTier2Block2SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0021)
#define gZclAttrSmplMetBISCurrNoTier2Block3SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0022)
#define gZclAttrSmplMetBISCurrNoTier2Block4SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0023)
#define gZclAttrSmplMetBISCurrNoTier2Block5SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0024)
#define gZclAttrSmplMetBISCurrNoTier2Block6SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0025)
#define gZclAttrSmplMetBISCurrNoTier2Block7SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0026)
#define gZclAttrSmplMetBISCurrNoTier2Block8SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0027)
#define gZclAttrSmplMetBISCurrNoTier2Block9SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0028)
#define gZclAttrSmplMetBISCurrNoTier2Block10SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0029)
#define gZclAttrSmplMetBISCurrNoTier2Block11SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x002A)
#define gZclAttrSmplMetBISCurrNoTier2Block12SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x002B)
#define gZclAttrSmplMetBISCurrNoTier2Block13SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x002C)
#define gZclAttrSmplMetBISCurrNoTier2Block14SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x002D)
#define gZclAttrSmplMetBISCurrNoTier2Block15SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x002E)
#define gZclAttrSmplMetBISCurrNoTier2Block16SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x002F)
#define gZclAttrSmplMetBISCurrNoTier3Block1SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0030)
#define gZclAttrSmplMetBISCurrNoTier3Block2SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0031)
#define gZclAttrSmplMetBISCurrNoTier3Block3SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0032)
#define gZclAttrSmplMetBISCurrNoTier3Block4SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0033)
#define gZclAttrSmplMetBISCurrNoTier3Block5SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0034)
#define gZclAttrSmplMetBISCurrNoTier3Block6SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0035)
#define gZclAttrSmplMetBISCurrNoTier3Block7SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0036)
#define gZclAttrSmplMetBISCurrNoTier3Block8SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0037)
#define gZclAttrSmplMetBISCurrNoTier3Block9SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0038)
#define gZclAttrSmplMetBISCurrNoTier3Block10SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0039)
#define gZclAttrSmplMetBISCurrNoTier3Block11SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x003A)
#define gZclAttrSmplMetBISCurrNoTier3Block12SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x003B)
#define gZclAttrSmplMetBISCurrNoTier3Block13SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x003C)
#define gZclAttrSmplMetBISCurrNoTier3Block14SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x003D)
#define gZclAttrSmplMetBISCurrNoTier3Block15SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x003E)
#define gZclAttrSmplMetBISCurrNoTier3Block16SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x003F)
#define gZclAttrSmplMetBISCurrNoTier4Block1SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0040)
#define gZclAttrSmplMetBISCurrNoTier4Block2SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0041)
#define gZclAttrSmplMetBISCurrNoTier4Block3SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0042)
#define gZclAttrSmplMetBISCurrNoTier4Block4SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0043)
#define gZclAttrSmplMetBISCurrNoTier4Block5SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0044)
#define gZclAttrSmplMetBISCurrNoTier4Block6SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0045)
#define gZclAttrSmplMetBISCurrNoTier4Block7SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0046)
#define gZclAttrSmplMetBISCurrNoTier4Block8SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0047)
#define gZclAttrSmplMetBISCurrNoTier4Block9SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0048)
#define gZclAttrSmplMetBISCurrNoTier4Block10SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0049)
#define gZclAttrSmplMetBISCurrNoTier4Block11SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x004A)
#define gZclAttrSmplMetBISCurrNoTier4Block12SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x004B)
#define gZclAttrSmplMetBISCurrNoTier4Block13SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x004C)
#define gZclAttrSmplMetBISCurrNoTier4Block14SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x004D)
#define gZclAttrSmplMetBISCurrNoTier4Block15SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x004E)
#define gZclAttrSmplMetBISCurrNoTier4Block16SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x004F)
#define gZclAttrSmplMetBISCurrNoTier5Block1SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0050)
#define gZclAttrSmplMetBISCurrNoTier5Block2SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0051)
#define gZclAttrSmplMetBISCurrNoTier5Block3SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0052)
#define gZclAttrSmplMetBISCurrNoTier5Block4SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0053)
#define gZclAttrSmplMetBISCurrNoTier5Block5SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0054)
#define gZclAttrSmplMetBISCurrNoTier5Block6SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0055)
#define gZclAttrSmplMetBISCurrNoTier5Block7SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0056)
#define gZclAttrSmplMetBISCurrNoTier5Block8SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0057)
#define gZclAttrSmplMetBISCurrNoTier5Block9SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0058)
#define gZclAttrSmplMetBISCurrNoTier5Block10SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0059)
#define gZclAttrSmplMetBISCurrNoTier5Block11SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x005A)
#define gZclAttrSmplMetBISCurrNoTier5Block12SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x005B)
#define gZclAttrSmplMetBISCurrNoTier5Block13SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x005C)
#define gZclAttrSmplMetBISCurrNoTier5Block14SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x005D)
#define gZclAttrSmplMetBISCurrNoTier5Block15SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x005E)
#define gZclAttrSmplMetBISCurrNoTier5Block16SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x005F)
#define gZclAttrSmplMetBISCurrNoTier6Block1SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0060)
#define gZclAttrSmplMetBISCurrNoTier6Block2SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0061)
#define gZclAttrSmplMetBISCurrNoTier6Block3SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0062)
#define gZclAttrSmplMetBISCurrNoTier6Block4SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0063)
#define gZclAttrSmplMetBISCurrNoTier6Block5SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0064)
#define gZclAttrSmplMetBISCurrNoTier6Block6SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0065)
#define gZclAttrSmplMetBISCurrNoTier6Block7SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0066)
#define gZclAttrSmplMetBISCurrNoTier6Block8SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0067)
#define gZclAttrSmplMetBISCurrNoTier6Block9SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0068)
#define gZclAttrSmplMetBISCurrNoTier6Block10SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0069)
#define gZclAttrSmplMetBISCurrNoTier6Block11SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x006A)
#define gZclAttrSmplMetBISCurrNoTier6Block12SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x006B)
#define gZclAttrSmplMetBISCurrNoTier6Block13SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x006C)
#define gZclAttrSmplMetBISCurrNoTier6Block14SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x006D)
#define gZclAttrSmplMetBISCurrNoTier6Block15SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x006E)
#define gZclAttrSmplMetBISCurrNoTier6Block16SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x006F)
#define gZclAttrSmplMetBISCurrNoTier7Block1SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0070)
#define gZclAttrSmplMetBISCurrNoTier7Block2SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0071)
#define gZclAttrSmplMetBISCurrNoTier7Block3SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0072)
#define gZclAttrSmplMetBISCurrNoTier7Block4SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0073)
#define gZclAttrSmplMetBISCurrNoTier7Block5SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0074)
#define gZclAttrSmplMetBISCurrNoTier7Block6SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0075)
#define gZclAttrSmplMetBISCurrNoTier7Block7SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0076)
#define gZclAttrSmplMetBISCurrNoTier7Block8SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0077)
#define gZclAttrSmplMetBISCurrNoTier7Block9SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0078)
#define gZclAttrSmplMetBISCurrNoTier7Block10SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0079)
#define gZclAttrSmplMetBISCurrNoTier7Block11SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x007A)
#define gZclAttrSmplMetBISCurrNoTier7Block12SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x007B)
#define gZclAttrSmplMetBISCurrNoTier7Block13SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x007C)
#define gZclAttrSmplMetBISCurrNoTier7Block14SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x007D)
#define gZclAttrSmplMetBISCurrNoTier7Block15SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x007E)
#define gZclAttrSmplMetBISCurrNoTier7Block16SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x007F)
#define gZclAttrSmplMetBISCurrNoTier8Block1SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0080)
#define gZclAttrSmplMetBISCurrNoTier8Block2SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0081)
#define gZclAttrSmplMetBISCurrNoTier8Block3SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0082)
#define gZclAttrSmplMetBISCurrNoTier8Block4SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0083)
#define gZclAttrSmplMetBISCurrNoTier8Block5SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0084)
#define gZclAttrSmplMetBISCurrNoTier8Block6SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0085)
#define gZclAttrSmplMetBISCurrNoTier8Block7SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0086)
#define gZclAttrSmplMetBISCurrNoTier8Block8SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0087)
#define gZclAttrSmplMetBISCurrNoTier8Block9SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0088)
#define gZclAttrSmplMetBISCurrNoTier8Block10SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0089)
#define gZclAttrSmplMetBISCurrNoTier8Block11SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x008A)
#define gZclAttrSmplMetBISCurrNoTier8Block12SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x008B)
#define gZclAttrSmplMetBISCurrNoTier8Block13SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x008C)
#define gZclAttrSmplMetBISCurrNoTier8Block14SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x008D)
#define gZclAttrSmplMetBISCurrNoTier8Block15SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x008E)
#define gZclAttrSmplMetBISCurrNoTier8Block16SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x008F)
#define gZclAttrSmplMetBISCurrNoTier9Block1SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0090)
#define gZclAttrSmplMetBISCurrNoTier9Block2SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0091)
#define gZclAttrSmplMetBISCurrNoTier9Block3SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0092)
#define gZclAttrSmplMetBISCurrNoTier9Block4SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0093)
#define gZclAttrSmplMetBISCurrNoTier9Block5SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0094)
#define gZclAttrSmplMetBISCurrNoTier9Block6SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0095)
#define gZclAttrSmplMetBISCurrNoTier9Block7SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0096)
#define gZclAttrSmplMetBISCurrNoTier9Block8SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0097)
#define gZclAttrSmplMetBISCurrNoTier9Block9SummationDelivered_c           (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0098)
#define gZclAttrSmplMetBISCurrNoTier9Block10SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x0099)
#define gZclAttrSmplMetBISCurrNoTier9Block11SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x009A)
#define gZclAttrSmplMetBISCurrNoTier9Block12SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x009B)
#define gZclAttrSmplMetBISCurrNoTier9Block13SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x009C)
#define gZclAttrSmplMetBISCurrNoTier9Block14SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x009D)
#define gZclAttrSmplMetBISCurrNoTier9Block15SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x009E)
#define gZclAttrSmplMetBISCurrNoTier9Block16SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x009F)
#define gZclAttrSmplMetBISCurrNoTier10Block1SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00A0)
#define gZclAttrSmplMetBISCurrNoTier10Block2SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00A1)
#define gZclAttrSmplMetBISCurrNoTier10Block3SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00A2)
#define gZclAttrSmplMetBISCurrNoTier10Block4SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00A3)
#define gZclAttrSmplMetBISCurrNoTier10Block5SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00A4)
#define gZclAttrSmplMetBISCurrNoTier10Block6SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00A5)
#define gZclAttrSmplMetBISCurrNoTier10Block7SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00A6)
#define gZclAttrSmplMetBISCurrNoTier10Block8SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00A7)
#define gZclAttrSmplMetBISCurrNoTier10Block9SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00A8)
#define gZclAttrSmplMetBISCurrNoTier10Block10SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00A9)
#define gZclAttrSmplMetBISCurrNoTier10Block11SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00AA)
#define gZclAttrSmplMetBISCurrNoTier10Block12SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00AB)
#define gZclAttrSmplMetBISCurrNoTier10Block13SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00AC)
#define gZclAttrSmplMetBISCurrNoTier10Block14SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00AD)
#define gZclAttrSmplMetBISCurrNoTier10Block15SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00AE)
#define gZclAttrSmplMetBISCurrNoTier10Block16SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00AF)
#define gZclAttrSmplMetBISCurrNoTier11Block1SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00B0)
#define gZclAttrSmplMetBISCurrNoTier11Block2SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00B1)
#define gZclAttrSmplMetBISCurrNoTier11Block3SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00B2)
#define gZclAttrSmplMetBISCurrNoTier11Block4SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00B3)
#define gZclAttrSmplMetBISCurrNoTier11Block5SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00B4)
#define gZclAttrSmplMetBISCurrNoTier11Block6SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00B5)
#define gZclAttrSmplMetBISCurrNoTier11Block7SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00B6)
#define gZclAttrSmplMetBISCurrNoTier11Block8SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00B7)
#define gZclAttrSmplMetBISCurrNoTier11Block9SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00B8)
#define gZclAttrSmplMetBISCurrNoTier11Block10SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00B9)
#define gZclAttrSmplMetBISCurrNoTier11Block11SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00BA)
#define gZclAttrSmplMetBISCurrNoTier11Block12SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00BB)
#define gZclAttrSmplMetBISCurrNoTier11Block13SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00BC)
#define gZclAttrSmplMetBISCurrNoTier11Block14SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00BD)
#define gZclAttrSmplMetBISCurrNoTier11Block15SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00BE)
#define gZclAttrSmplMetBISCurrNoTier11Block16SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00BF)
#define gZclAttrSmplMetBISCurrNoTier12Block1SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00C0)
#define gZclAttrSmplMetBISCurrNoTier12Block2SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00C1)
#define gZclAttrSmplMetBISCurrNoTier12Block3SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00C2)
#define gZclAttrSmplMetBISCurrNoTier12Block4SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00C3)
#define gZclAttrSmplMetBISCurrNoTier12Block5SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00C4)
#define gZclAttrSmplMetBISCurrNoTier12Block6SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00C5)
#define gZclAttrSmplMetBISCurrNoTier12Block7SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00C6)
#define gZclAttrSmplMetBISCurrNoTier12Block8SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00C7)
#define gZclAttrSmplMetBISCurrNoTier12Block9SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00C8)
#define gZclAttrSmplMetBISCurrNoTier12Block10SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00C9)
#define gZclAttrSmplMetBISCurrNoTier12Block11SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00CA)
#define gZclAttrSmplMetBISCurrNoTier12Block12SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00CB)
#define gZclAttrSmplMetBISCurrNoTier12Block13SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00CC)
#define gZclAttrSmplMetBISCurrNoTier12Block14SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00CD)
#define gZclAttrSmplMetBISCurrNoTier12Block15SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00CE)
#define gZclAttrSmplMetBISCurrNoTier12Block16SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00CF)
#define gZclAttrSmplMetBISCurrNoTier13Block1SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00D0)
#define gZclAttrSmplMetBISCurrNoTier13Block2SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00D1)
#define gZclAttrSmplMetBISCurrNoTier13Block3SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00D2)
#define gZclAttrSmplMetBISCurrNoTier13Block4SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00D3)
#define gZclAttrSmplMetBISCurrNoTier13Block5SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00D4)
#define gZclAttrSmplMetBISCurrNoTier13Block6SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00D5)
#define gZclAttrSmplMetBISCurrNoTier13Block7SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00D6)
#define gZclAttrSmplMetBISCurrNoTier13Block8SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00D7)
#define gZclAttrSmplMetBISCurrNoTier13Block9SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00D8)
#define gZclAttrSmplMetBISCurrNoTier13Block10SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00D9)
#define gZclAttrSmplMetBISCurrNoTier13Block11SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00DA)
#define gZclAttrSmplMetBISCurrNoTier13Block12SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00DB)
#define gZclAttrSmplMetBISCurrNoTier13Block13SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00DC)
#define gZclAttrSmplMetBISCurrNoTier13Block14SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00DD)
#define gZclAttrSmplMetBISCurrNoTier13Block15SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00DE)
#define gZclAttrSmplMetBISCurrNoTier13Block16SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00DF)
#define gZclAttrSmplMetBISCurrNoTier14Block1SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00E0)
#define gZclAttrSmplMetBISCurrNoTier14Block2SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00E1)
#define gZclAttrSmplMetBISCurrNoTier14Block3SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00E2)
#define gZclAttrSmplMetBISCurrNoTier14Block4SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00E3)
#define gZclAttrSmplMetBISCurrNoTier14Block5SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00E4)
#define gZclAttrSmplMetBISCurrNoTier14Block6SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00E5)
#define gZclAttrSmplMetBISCurrNoTier14Block7SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00E6)
#define gZclAttrSmplMetBISCurrNoTier14Block8SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00E7)
#define gZclAttrSmplMetBISCurrNoTier14Block9SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00E8)
#define gZclAttrSmplMetBISCurrNoTier14Block10SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00E9)
#define gZclAttrSmplMetBISCurrNoTier14Block11SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00EA)
#define gZclAttrSmplMetBISCurrNoTier14Block12SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00EB)
#define gZclAttrSmplMetBISCurrNoTier14Block13SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00EC)
#define gZclAttrSmplMetBISCurrNoTier14Block14SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00ED)
#define gZclAttrSmplMetBISCurrNoTier14Block15SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00EE)
#define gZclAttrSmplMetBISCurrNoTier14Block16SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00EF)
#define gZclAttrSmplMetBISCurrNoTier15Block1SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00F0)
#define gZclAttrSmplMetBISCurrNoTier15Block2SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00F1)
#define gZclAttrSmplMetBISCurrNoTier15Block3SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00F2)
#define gZclAttrSmplMetBISCurrNoTier15Block4SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00F3)
#define gZclAttrSmplMetBISCurrNoTier15Block5SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00F4)
#define gZclAttrSmplMetBISCurrNoTier15Block6SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00F5)
#define gZclAttrSmplMetBISCurrNoTier15Block7SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00F6)
#define gZclAttrSmplMetBISCurrNoTier15Block8SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00F7)
#define gZclAttrSmplMetBISCurrNoTier15Block9SummationDelivered_c          (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00F8)
#define gZclAttrSmplMetBISCurrNoTier15Block10SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00F9)
#define gZclAttrSmplMetBISCurrNoTier15Block11SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00FA)
#define gZclAttrSmplMetBISCurrNoTier15Block12SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00FB)
#define gZclAttrSmplMetBISCurrNoTier15Block13SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00FC)
#define gZclAttrSmplMetBISCurrNoTier15Block14SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00FD)
#define gZclAttrSmplMetBISCurrNoTier15Block15SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00FE)
#define gZclAttrSmplMetBISCurrNoTier15Block16SummationDelivered_c         (gZclAttrSmplMetBlockInformationSetDlvrd_c | 0x00FF)

/* Alarm Attribute Set */
#define gZclAttrSmplMetASGenericAlarmMask_c                  (gZclAttrSmplMetAlarmsSet_c | 0x0000)
#define gZclAttrSmplMetASElectricityAlarmMask_c              (gZclAttrSmplMetAlarmsSet_c | 0x0001)
#define gZclAttrSmplMetASGenericFlowPressureAlarmMask_c      (gZclAttrSmplMetAlarmsSet_c | 0x0002)
#define gZclAttrSmplMetASWaterSpecificAlarmMask_c            (gZclAttrSmplMetAlarmsSet_c | 0x0003)
#define gZclAttrSmplMetASHeatAndCoolingSpecificAlarmMask_c   (gZclAttrSmplMetAlarmsSet_c | 0x0004)
#define gZclAttrSmplMetASGasSpecificAlarmMask_c              (gZclAttrSmplMetAlarmsSet_c | 0x0005)

/* Block Information Attribute Set Received */
#define gZclAttrSmplMetBISCurrNoTierBlock1SummationReceived_c            (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0000)
#define gZclAttrSmplMetBISCurrNoTierBlock2SummationReceived_c            (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0001)
#define gZclAttrSmplMetBISCurrNoTierBlock3SummationReceived_c            (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0002)
#define gZclAttrSmplMetBISCurrNoTierBlock4SummationReceived_c            (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0003)
#define gZclAttrSmplMetBISCurrNoTierBlock5SummationReceived_c            (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0004)
#define gZclAttrSmplMetBISCurrNoTierBlock6SummationReceived_c            (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0005)
#define gZclAttrSmplMetBISCurrNoTierBlock7SummationReceived_c            (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0006)
#define gZclAttrSmplMetBISCurrNoTierBlock8SummationReceived_c            (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0007)
#define gZclAttrSmplMetBISCurrNoTierBlock9SummationReceived_c            (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0008)
#define gZclAttrSmplMetBISCurrNoTierBlock10SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0009)
#define gZclAttrSmplMetBISCurrNoTierBlock11SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x000A)
#define gZclAttrSmplMetBISCurrNoTierBlock12SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x000B)
#define gZclAttrSmplMetBISCurrNoTierBlock13SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x000C)
#define gZclAttrSmplMetBISCurrNoTierBlock14SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x000D)
#define gZclAttrSmplMetBISCurrNoTierBlock15SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x000E)
#define gZclAttrSmplMetBISCurrNoTierBlock16SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x000F)
#define gZclAttrSmplMetBISCurrNoTier1Block1SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0010)
#define gZclAttrSmplMetBISCurrNoTier1Block2SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0011)
#define gZclAttrSmplMetBISCurrNoTier1Block3SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0012)
#define gZclAttrSmplMetBISCurrNoTier1Block4SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0013)
#define gZclAttrSmplMetBISCurrNoTier1Block5SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0014)
#define gZclAttrSmplMetBISCurrNoTier1Block6SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0015)
#define gZclAttrSmplMetBISCurrNoTier1Block7SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0016)
#define gZclAttrSmplMetBISCurrNoTier1Block8SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0017)
#define gZclAttrSmplMetBISCurrNoTier1Block9SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0018)
#define gZclAttrSmplMetBISCurrNoTier1Block10SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0019)
#define gZclAttrSmplMetBISCurrNoTier1Block11SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x001A)
#define gZclAttrSmplMetBISCurrNoTier1Block12SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x001B)
#define gZclAttrSmplMetBISCurrNoTier1Block13SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x001C)
#define gZclAttrSmplMetBISCurrNoTier1Block14SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x001D)
#define gZclAttrSmplMetBISCurrNoTier1Block15SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x001E)
#define gZclAttrSmplMetBISCurrNoTier1Block16SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x001F)
#define gZclAttrSmplMetBISCurrNoTier2Block1SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0020)
#define gZclAttrSmplMetBISCurrNoTier2Block2SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0021)
#define gZclAttrSmplMetBISCurrNoTier2Block3SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0022)
#define gZclAttrSmplMetBISCurrNoTier2Block4SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0023)
#define gZclAttrSmplMetBISCurrNoTier2Block5SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0024)
#define gZclAttrSmplMetBISCurrNoTier2Block6SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0025)
#define gZclAttrSmplMetBISCurrNoTier2Block7SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0026)
#define gZclAttrSmplMetBISCurrNoTier2Block8SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0027)
#define gZclAttrSmplMetBISCurrNoTier2Block9SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0028)
#define gZclAttrSmplMetBISCurrNoTier2Block10SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0029)
#define gZclAttrSmplMetBISCurrNoTier2Block11SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x002A)
#define gZclAttrSmplMetBISCurrNoTier2Block12SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x002B)
#define gZclAttrSmplMetBISCurrNoTier2Block13SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x002C)
#define gZclAttrSmplMetBISCurrNoTier2Block14SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x002D)
#define gZclAttrSmplMetBISCurrNoTier2Block15SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x002E)
#define gZclAttrSmplMetBISCurrNoTier2Block16SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x002F)
#define gZclAttrSmplMetBISCurrNoTier3Block1SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0030)
#define gZclAttrSmplMetBISCurrNoTier3Block2SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0031)
#define gZclAttrSmplMetBISCurrNoTier3Block3SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0032)
#define gZclAttrSmplMetBISCurrNoTier3Block4SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0033)
#define gZclAttrSmplMetBISCurrNoTier3Block5SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0034)
#define gZclAttrSmplMetBISCurrNoTier3Block6SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0035)
#define gZclAttrSmplMetBISCurrNoTier3Block7SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0036)
#define gZclAttrSmplMetBISCurrNoTier3Block8SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0037)
#define gZclAttrSmplMetBISCurrNoTier3Block9SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0038)
#define gZclAttrSmplMetBISCurrNoTier3Block10SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0039)
#define gZclAttrSmplMetBISCurrNoTier3Block11SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x003A)
#define gZclAttrSmplMetBISCurrNoTier3Block12SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x003B)
#define gZclAttrSmplMetBISCurrNoTier3Block13SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x003C)
#define gZclAttrSmplMetBISCurrNoTier3Block14SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x003D)
#define gZclAttrSmplMetBISCurrNoTier3Block15SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x003E)
#define gZclAttrSmplMetBISCurrNoTier3Block16SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x003F)
#define gZclAttrSmplMetBISCurrNoTier4Block1SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0040)
#define gZclAttrSmplMetBISCurrNoTier4Block2SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0041)
#define gZclAttrSmplMetBISCurrNoTier4Block3SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0042)
#define gZclAttrSmplMetBISCurrNoTier4Block4SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0043)
#define gZclAttrSmplMetBISCurrNoTier4Block5SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0044)
#define gZclAttrSmplMetBISCurrNoTier4Block6SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0045)
#define gZclAttrSmplMetBISCurrNoTier4Block7SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0046)
#define gZclAttrSmplMetBISCurrNoTier4Block8SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0047)
#define gZclAttrSmplMetBISCurrNoTier4Block9SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0048)
#define gZclAttrSmplMetBISCurrNoTier4Block10SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0049)
#define gZclAttrSmplMetBISCurrNoTier4Block11SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x004A)
#define gZclAttrSmplMetBISCurrNoTier4Block12SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x004B)
#define gZclAttrSmplMetBISCurrNoTier4Block13SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x004C)
#define gZclAttrSmplMetBISCurrNoTier4Block14SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x004D)
#define gZclAttrSmplMetBISCurrNoTier4Block15SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x004E)
#define gZclAttrSmplMetBISCurrNoTier4Block16SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x004F)
#define gZclAttrSmplMetBISCurrNoTier5Block1SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0050)
#define gZclAttrSmplMetBISCurrNoTier5Block2SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0051)
#define gZclAttrSmplMetBISCurrNoTier5Block3SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0052)
#define gZclAttrSmplMetBISCurrNoTier5Block4SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0053)
#define gZclAttrSmplMetBISCurrNoTier5Block5SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0054)
#define gZclAttrSmplMetBISCurrNoTier5Block6SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0055)
#define gZclAttrSmplMetBISCurrNoTier5Block7SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0056)
#define gZclAttrSmplMetBISCurrNoTier5Block8SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0057)
#define gZclAttrSmplMetBISCurrNoTier5Block9SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0058)
#define gZclAttrSmplMetBISCurrNoTier5Block10SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0059)
#define gZclAttrSmplMetBISCurrNoTier5Block11SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x005A)
#define gZclAttrSmplMetBISCurrNoTier5Block12SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x005B)
#define gZclAttrSmplMetBISCurrNoTier5Block13SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x005C)
#define gZclAttrSmplMetBISCurrNoTier5Block14SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x005D)
#define gZclAttrSmplMetBISCurrNoTier5Block15SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x005E)
#define gZclAttrSmplMetBISCurrNoTier5Block16SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x005F)
#define gZclAttrSmplMetBISCurrNoTier6Block1SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0060)
#define gZclAttrSmplMetBISCurrNoTier6Block2SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0061)
#define gZclAttrSmplMetBISCurrNoTier6Block3SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0062)
#define gZclAttrSmplMetBISCurrNoTier6Block4SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0063)
#define gZclAttrSmplMetBISCurrNoTier6Block5SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0064)
#define gZclAttrSmplMetBISCurrNoTier6Block6SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0065)
#define gZclAttrSmplMetBISCurrNoTier6Block7SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0066)
#define gZclAttrSmplMetBISCurrNoTier6Block8SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0067)
#define gZclAttrSmplMetBISCurrNoTier6Block9SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0068)
#define gZclAttrSmplMetBISCurrNoTier6Block10SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0069)
#define gZclAttrSmplMetBISCurrNoTier6Block11SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x006A)
#define gZclAttrSmplMetBISCurrNoTier6Block12SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x006B)
#define gZclAttrSmplMetBISCurrNoTier6Block13SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x006C)
#define gZclAttrSmplMetBISCurrNoTier6Block14SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x006D)
#define gZclAttrSmplMetBISCurrNoTier6Block15SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x006E)
#define gZclAttrSmplMetBISCurrNoTier6Block16SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x006F)
#define gZclAttrSmplMetBISCurrNoTier7Block1SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0070)
#define gZclAttrSmplMetBISCurrNoTier7Block2SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0071)
#define gZclAttrSmplMetBISCurrNoTier7Block3SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0072)
#define gZclAttrSmplMetBISCurrNoTier7Block4SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0073)
#define gZclAttrSmplMetBISCurrNoTier7Block5SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0074)
#define gZclAttrSmplMetBISCurrNoTier7Block6SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0075)
#define gZclAttrSmplMetBISCurrNoTier7Block7SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0076)
#define gZclAttrSmplMetBISCurrNoTier7Block8SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0077)
#define gZclAttrSmplMetBISCurrNoTier7Block9SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0078)
#define gZclAttrSmplMetBISCurrNoTier7Block10SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0079)
#define gZclAttrSmplMetBISCurrNoTier7Block11SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x007A)
#define gZclAttrSmplMetBISCurrNoTier7Block12SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x007B)
#define gZclAttrSmplMetBISCurrNoTier7Block13SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x007C)
#define gZclAttrSmplMetBISCurrNoTier7Block14SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x007D)
#define gZclAttrSmplMetBISCurrNoTier7Block15SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x007E)
#define gZclAttrSmplMetBISCurrNoTier7Block16SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x007F)
#define gZclAttrSmplMetBISCurrNoTier8Block1SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0080)
#define gZclAttrSmplMetBISCurrNoTier8Block2SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0081)
#define gZclAttrSmplMetBISCurrNoTier8Block3SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0082)
#define gZclAttrSmplMetBISCurrNoTier8Block4SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0083)
#define gZclAttrSmplMetBISCurrNoTier8Block5SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0084)
#define gZclAttrSmplMetBISCurrNoTier8Block6SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0085)
#define gZclAttrSmplMetBISCurrNoTier8Block7SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0086)
#define gZclAttrSmplMetBISCurrNoTier8Block8SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0087)
#define gZclAttrSmplMetBISCurrNoTier8Block9SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0088)
#define gZclAttrSmplMetBISCurrNoTier8Block10SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0089)
#define gZclAttrSmplMetBISCurrNoTier8Block11SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x008A)
#define gZclAttrSmplMetBISCurrNoTier8Block12SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x008B)
#define gZclAttrSmplMetBISCurrNoTier8Block13SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x008C)
#define gZclAttrSmplMetBISCurrNoTier8Block14SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x008D)
#define gZclAttrSmplMetBISCurrNoTier8Block15SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x008E)
#define gZclAttrSmplMetBISCurrNoTier8Block16SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x008F)
#define gZclAttrSmplMetBISCurrNoTier9Block1SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0090)
#define gZclAttrSmplMetBISCurrNoTier9Block2SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0091)
#define gZclAttrSmplMetBISCurrNoTier9Block3SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0092)
#define gZclAttrSmplMetBISCurrNoTier9Block4SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0093)
#define gZclAttrSmplMetBISCurrNoTier9Block5SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0094)
#define gZclAttrSmplMetBISCurrNoTier9Block6SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0095)
#define gZclAttrSmplMetBISCurrNoTier9Block7SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0096)
#define gZclAttrSmplMetBISCurrNoTier9Block8SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0097)
#define gZclAttrSmplMetBISCurrNoTier9Block9SummationReceived_c           (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0098)
#define gZclAttrSmplMetBISCurrNoTier9Block10SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x0099)
#define gZclAttrSmplMetBISCurrNoTier9Block11SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x009A)
#define gZclAttrSmplMetBISCurrNoTier9Block12SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x009B)
#define gZclAttrSmplMetBISCurrNoTier9Block13SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x009C)
#define gZclAttrSmplMetBISCurrNoTier9Block14SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x009D)
#define gZclAttrSmplMetBISCurrNoTier9Block15SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x009E)
#define gZclAttrSmplMetBISCurrNoTier9Block16SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x009F)
#define gZclAttrSmplMetBISCurrNoTier10Block1SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00A0)
#define gZclAttrSmplMetBISCurrNoTier10Block2SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00A1)
#define gZclAttrSmplMetBISCurrNoTier10Block3SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00A2)
#define gZclAttrSmplMetBISCurrNoTier10Block4SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00A3)
#define gZclAttrSmplMetBISCurrNoTier10Block5SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00A4)
#define gZclAttrSmplMetBISCurrNoTier10Block6SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00A5)
#define gZclAttrSmplMetBISCurrNoTier10Block7SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00A6)
#define gZclAttrSmplMetBISCurrNoTier10Block8SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00A7)
#define gZclAttrSmplMetBISCurrNoTier10Block9SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00A8)
#define gZclAttrSmplMetBISCurrNoTier10Block10SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00A9)
#define gZclAttrSmplMetBISCurrNoTier10Block11SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00AA)
#define gZclAttrSmplMetBISCurrNoTier10Block12SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00AB)
#define gZclAttrSmplMetBISCurrNoTier10Block13SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00AC)
#define gZclAttrSmplMetBISCurrNoTier10Block14SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00AD)
#define gZclAttrSmplMetBISCurrNoTier10Block15SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00AE)
#define gZclAttrSmplMetBISCurrNoTier10Block16SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00AF)
#define gZclAttrSmplMetBISCurrNoTier11Block1SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00B0)
#define gZclAttrSmplMetBISCurrNoTier11Block2SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00B1)
#define gZclAttrSmplMetBISCurrNoTier11Block3SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00B2)
#define gZclAttrSmplMetBISCurrNoTier11Block4SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00B3)
#define gZclAttrSmplMetBISCurrNoTier11Block5SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00B4)
#define gZclAttrSmplMetBISCurrNoTier11Block6SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00B5)
#define gZclAttrSmplMetBISCurrNoTier11Block7SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00B6)
#define gZclAttrSmplMetBISCurrNoTier11Block8SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00B7)
#define gZclAttrSmplMetBISCurrNoTier11Block9SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00B8)
#define gZclAttrSmplMetBISCurrNoTier11Block10SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00B9)
#define gZclAttrSmplMetBISCurrNoTier11Block11SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00BA)
#define gZclAttrSmplMetBISCurrNoTier11Block12SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00BB)
#define gZclAttrSmplMetBISCurrNoTier11Block13SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00BC)
#define gZclAttrSmplMetBISCurrNoTier11Block14SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00BD)
#define gZclAttrSmplMetBISCurrNoTier11Block15SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00BE)
#define gZclAttrSmplMetBISCurrNoTier11Block16SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00BF)
#define gZclAttrSmplMetBISCurrNoTier12Block1SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00C0)
#define gZclAttrSmplMetBISCurrNoTier12Block2SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00C1)
#define gZclAttrSmplMetBISCurrNoTier12Block3SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00C2)
#define gZclAttrSmplMetBISCurrNoTier12Block4SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00C3)
#define gZclAttrSmplMetBISCurrNoTier12Block5SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00C4)
#define gZclAttrSmplMetBISCurrNoTier12Block6SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00C5)
#define gZclAttrSmplMetBISCurrNoTier12Block7SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00C6)
#define gZclAttrSmplMetBISCurrNoTier12Block8SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00C7)
#define gZclAttrSmplMetBISCurrNoTier12Block9SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00C8)
#define gZclAttrSmplMetBISCurrNoTier12Block10SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00C9)
#define gZclAttrSmplMetBISCurrNoTier12Block11SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00CA)
#define gZclAttrSmplMetBISCurrNoTier12Block12SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00CB)
#define gZclAttrSmplMetBISCurrNoTier12Block13SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00CC)
#define gZclAttrSmplMetBISCurrNoTier12Block14SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00CD)
#define gZclAttrSmplMetBISCurrNoTier12Block15SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00CE)
#define gZclAttrSmplMetBISCurrNoTier12Block16SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00CF)
#define gZclAttrSmplMetBISCurrNoTier13Block1SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00D0)
#define gZclAttrSmplMetBISCurrNoTier13Block2SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00D1)
#define gZclAttrSmplMetBISCurrNoTier13Block3SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00D2)
#define gZclAttrSmplMetBISCurrNoTier13Block4SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00D3)
#define gZclAttrSmplMetBISCurrNoTier13Block5SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00D4)
#define gZclAttrSmplMetBISCurrNoTier13Block6SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00D5)
#define gZclAttrSmplMetBISCurrNoTier13Block7SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00D6)
#define gZclAttrSmplMetBISCurrNoTier13Block8SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00D7)
#define gZclAttrSmplMetBISCurrNoTier13Block9SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00D8)
#define gZclAttrSmplMetBISCurrNoTier13Block10SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00D9)
#define gZclAttrSmplMetBISCurrNoTier13Block11SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00DA)
#define gZclAttrSmplMetBISCurrNoTier13Block12SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00DB)
#define gZclAttrSmplMetBISCurrNoTier13Block13SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00DC)
#define gZclAttrSmplMetBISCurrNoTier13Block14SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00DD)
#define gZclAttrSmplMetBISCurrNoTier13Block15SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00DE)
#define gZclAttrSmplMetBISCurrNoTier13Block16SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00DF)
#define gZclAttrSmplMetBISCurrNoTier14Block1SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00E0)
#define gZclAttrSmplMetBISCurrNoTier14Block2SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00E1)
#define gZclAttrSmplMetBISCurrNoTier14Block3SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00E2)
#define gZclAttrSmplMetBISCurrNoTier14Block4SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00E3)
#define gZclAttrSmplMetBISCurrNoTier14Block5SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00E4)
#define gZclAttrSmplMetBISCurrNoTier14Block6SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00E5)
#define gZclAttrSmplMetBISCurrNoTier14Block7SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00E6)
#define gZclAttrSmplMetBISCurrNoTier14Block8SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00E7)
#define gZclAttrSmplMetBISCurrNoTier14Block9SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00E8)
#define gZclAttrSmplMetBISCurrNoTier14Block10SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00E9)
#define gZclAttrSmplMetBISCurrNoTier14Block11SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00EA)
#define gZclAttrSmplMetBISCurrNoTier14Block12SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00EB)
#define gZclAttrSmplMetBISCurrNoTier14Block13SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00EC)
#define gZclAttrSmplMetBISCurrNoTier14Block14SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00ED)
#define gZclAttrSmplMetBISCurrNoTier14Block15SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00EE)
#define gZclAttrSmplMetBISCurrNoTier14Block16SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00EF)
#define gZclAttrSmplMetBISCurrNoTier15Block1SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00F0)
#define gZclAttrSmplMetBISCurrNoTier15Block2SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00F1)
#define gZclAttrSmplMetBISCurrNoTier15Block3SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00F2)
#define gZclAttrSmplMetBISCurrNoTier15Block4SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00F3)
#define gZclAttrSmplMetBISCurrNoTier15Block5SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00F4)
#define gZclAttrSmplMetBISCurrNoTier15Block6SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00F5)
#define gZclAttrSmplMetBISCurrNoTier15Block7SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00F6)
#define gZclAttrSmplMetBISCurrNoTier15Block8SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00F7)
#define gZclAttrSmplMetBISCurrNoTier15Block9SummationReceived_c          (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00F8)
#define gZclAttrSmplMetBISCurrNoTier15Block10SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00F9)
#define gZclAttrSmplMetBISCurrNoTier15Block11SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00FA)
#define gZclAttrSmplMetBISCurrNoTier15Block12SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00FB)
#define gZclAttrSmplMetBISCurrNoTier15Block13SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00FC)
#define gZclAttrSmplMetBISCurrNoTier15Block14SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00FD)
#define gZclAttrSmplMetBISCurrNoTier15Block15SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00FE)
#define gZclAttrSmplMetBISCurrNoTier15Block16SummationReceived_c         (gZclAttrSmplMetBlockInformationSetRcvd_c | 0x00FF)

/* Billing Attribute Set */
#define gZclAttrSmplMetBillingBillToDate_c                               (gZclAttrSmplMetBillingSet_c | 0x0000)
#define gZclAttrSmplMetBillingBillToDateTimeStamp_c                      (gZclAttrSmplMetBillingSet_c | 0x0001)
#define gZclAttrSmplMetBillingProjectedBill_c                            (gZclAttrSmplMetBillingSet_c | 0x0002)
#define gZclAttrSmplMetBillingProjectedBillTimeStamp_c                   (gZclAttrSmplMetBillingSet_c | 0x0003)

#endif /* #if gBigEndian_c */

/* Mapping of the Meter Status Attribute */
#define gZclMSStatusCheckMeter_c              0x01
#define gZclMSStatusLowBattery_c              0x02 
#define gZclMSStatusTamperDetect_c            0x04 
#define gZclMSStatusPowerFailure_c            0x08 
#define gZclMSStatusPowerQuality_c            0x10 
#define gZclMSStatusLeakDetect_c              0x20 
#define gZclMSStatusServiceDisconnectOpen_c   0x40 


/* UnitofMeasure Attribute Enumerations */
#define  gZclAttrSmplMetCFS_kWInPureBinary_c 0x00 /* kW (kilo-Watts) & kWh (kilo-WattHours) in pure Binary format.*/
#define  gZclAttrSmplMetCFS_m3InPureBinary_c 0x01 /*m3 (Cubic Meter) & m3/h (Cubic Meter per Hour) in pure Binary format.*/
#define  gZclAttrSmplMetCFS_ft3InPureBinary_c 0x02 /*ft3 (Cubic Feet) & ft3/h (Cubic Feet per Hour) in pure Binary format.*/
#define  gZclAttrSmplMetCFS_ccfInPureBinary_c 0x03 /*ccf ((100 or Centum) Cubic Feet) & ccf/h ((100 or Centum) Cubic Feet per Hour) in pure Binary format.*/
#define  gZclAttrSmplMetCFS_USglInPureBinary_c 0x04 /*US gl (US Gallons) & US gl/h (US Gallons per Hour) in pure Binary format.*/
#define  gZclAttrSmplMetCFS_IMPglInPureBinary_c 0x05 /*IMP gl (Imperial Gallons) & IMP gl/h (Imperial Gallons per Hour) in pure Binary format.*/
#define  gZclAttrSmplMetCFS_BTUsInPureBinary_c 0x06 /*BTUs & BTU/h in pure Binary format.*/
#define  gZclAttrSmplMetCFS_LitersInPureBinary_c 0x07 /*Liters & l/h (Liters per Hour) in pure Binary format*/
#define  gZclAttrSmplMetCFS_kPAGaugeInPureBinary_c 0x08 /*kPA(gauge) in pure Binary format.*/
#define  gZclAttrSmplMetCFS_kPAAbsoluteInPureBinary_c 0x09 /*kPA(absolute) in pure Binary format.*/
#define  gZclAttrSmplMetCFS_kWInBCD_c 0x80 /*kW (kilo-Watts) & kWh (kilo-WattHours) in BCD format*/
#define  gZclAttrSmplMetCFS_m3InBCD_c 0x81 /*m3 (Cubic Meter) & m3/h (Cubic Meter per Hour) in BCD format*/
#define  gZclAttrSmplMetCFS_ft3InBCD_c 0x82 /*ft3 (Cubic Feet) & ft3/h (Cubic Feet per Hour) in BCD format*/
#define  gZclAttrSmplMetCFS_ccfInBCD_c 0x83 /*ccf ((100 or Centum) Cubic Feet) & ccf/h ((100 or Centum) Cubic Feet per Hour) in BCD format*/
#define  gZclAttrSmplMetCFS_USglInBCD_c 0x84 /* US gl (US Gallons) & US gl/h (US Gallons per Hour) in BCD format*/
#define  gZclAttrSmplMetCFS_IMPglInBCD_c 0x85 /* IMP gl (Imperial Gallons) & IMP gl/h (Imperial Gallons per Hour) in BCD format */
#define  gZclAttrSmplMetCFS_BTUsInBCD_c 0x86 /* BTUs & BTU/h in BCD format */
#define  gZclAttrSmplMetCFS_LitersInBCD_c 0x87 /* Liters & l/h (Liters per Hour) in BCD format */
#define  gZclAttrSmplMetCFS_kPAGaugeInBCD_c 0x88 /* kPA(gauge) in BCD format. */
#define  gZclAttrSmplMetCFS_kPAAbsoluteInBCD_c 0x89 /* kPA(absolute) in BCD format. */

/* Summation Formatting Attribute */
#define gZclAttrSmplMetFormat_c      gSmplMetFormat_c  

/* TemperatureUnitOfMeasure Enumeration  */
#define gZclAttrSmplMetTUMKelvin_InPureBinary_c      0x00  /* K (Degrees Kelvin) in pure Binary format. */
#define gZclAttrSmplMetTUMCelsius_InPureBinary_c     0x01  /* ?C (Degrees Celsius) in pure Binary format. */ 
#define gZclAttrSmplMetTUMFahrenheit_InPureBinary_c  0x02  /* ?F (Degrees Fahrenheit) in pure Binary format. */ 
#define gZclAttrSmplMetTUMKelvin_InBCD_c             0x80  /* K (Degrees Kelvin) in BCD format. */ 
#define gZclAttrSmplMetTUMCelsius_InBCD_c            0x81  /* ?C (Degrees Celsius) in BCD format. */ 
#define gZclAttrSmplMetTUMFahrenheit_InBCD_c         0x82  /* ?F (Degrees Fahrenheit) in BCD format. */ 

/* MeteringDeviceType Attribute Enumerations */
#define  gZclAttrSmplMetCFSElectricMet_c         0x00
#define  gZclAttrSmplMetCFSGasMet_C              0x01
#define  gZclAttrSmplMetCFSWaterMet_c            0x02
#define  gZclAttrSmplMetCFSThermalMet_c          0x03
#define  gZclAttrSmplMetCFSPressureMet_c         0x04
#define  gZclAttrSmplMetCFSHeatMet_c             0x05
#define  gZclAttrSmplMetCFSCoolingMet_c          0x06
#define  gZclAttrSmplMetCFSMirroredGasMet_c      0x80
#define  gZclAttrSmplMetCFSMirroredWaterMet_c    0x81
#define  gZclAttrSmplMetCFSMirroredThermalMet_c  0x82
#define  gZclAttrSmplMetCFSMirroredPressureMet_c 0x83
#define  gZclAttrSmplMetCFSMirroredHeatMet_c     0x84
#define  gZclAttrSmplMetCFSMirroredCoolingMet_c  0x85

/* ProfileIntervalPeriod Timeframes */
#define gZclSEProfIntrvPeriod_Daily_c  0x00
#define gZclSEProfIntrvPeriod_60mins_c 0x01
#define gZclSEProfIntrvPeriod_30mins_c 0x02
#define gZclSEProfIntrvPeriod_15mins_c 0x03
#define gZclSEProfIntrvPeriod_10mins_c 0x04
#define gZclSEProfIntrvPeriod_7dot5mins_c 0x05
#define gZclSEProfIntrvPeriod_5mins_c     0x06
#define gZclSEProfIntrvPeriod_2dot5mins_c     0x07

/* Block Threshold Field Enumerations */
#define gBTFNoBlocksInUse_c                 0x00
#define gBTFNoBlock1_c                      0x01
#define gBTFNoBlock2_c                      0x02
#define gBTFNoBlock3_c                      0x03
#define gBTFNoBlock4_c                      0x04
#define gBTFNoBlock5_c                      0x05
#define gBTFNoBlock6_c                      0x06
#define gBTFNoBlock7_c                      0x07
#define gBTFNoBlock8_c                      0x08
#define gBTFNoBlock9_c                      0x09
#define gBTFNoBlock10_c                     0x0A
#define gBTFNoBlock11_c                     0x0B
#define gBTFNoBlock12_c                     0x0C
#define gBTFNoBlock13_c                     0x0D
#define gBTFNoBlock14_c                     0x0E
#define gBTFNoBlock15_c                     0x0F
#define gBTFNoBlock16_c                     0x10

/* Generic Alarm Group */
#define gGAGCheckMeter_c                     0x00 
#define gGAGLowBatery_c                      0x01 
#define gGAGTamperDetect_c                   0x02 
#define gGAGPowerFailure_c                   0x03
#define gGAGPipeEmpty_c                      0x03
#define gGAGTempSensor_c                     0x03
#define gGAGPowerQuality_c                   0x04
#define gGAGLowPressure_c                    0x04
#define gGAGBurstDetect_c                    0x04
#define gGAGLeakDetect_c                     0x05
#define gGAGServiceDisconnect_c              0x06
#define gGAGFlow_c                           0x07
#define gGAGFlowSensor_c                     0x07

/* Electricity Alarm Group */
#define gEAGLowVoltageL1_c                   0x10 
#define gEAGHighVoltageL1_c                  0x11
#define gEAGLowVoltageL2_c                   0x12 
#define gEAGHighVoltageL2_c                  0x13
#define gEAGLowVoltageL3_c                   0x14 
#define gEAGHighVoltageL3_c                  0x15
#define gEAGOverCurrentL1_c                  0x16 
#define gEAGOverCurrentL2_c                  0x17
#define gEAGOverCurrentL3_c                  0x18 
#define gEAGFrequencyTooLowL1_c              0x19
#define gEAGFrequencyTooHighL1_c             0x1A
#define gEAGFrequencyTooLowL2_c              0x1B
#define gEAGFrequencyTooHighL2_c             0x1C
#define gEAGFrequencyTooLowL3_c              0x1D
#define gEAGFrequencyTooHighL3_c             0x1E
#define gEAGGroundFault_c                    0x1F
#define gEAGElectricTamperDetect_c           0x20

/* Generic Flow/Presure Alarm Group */
#define gFPABurstDetect_c                    0x30
#define gFPAPresureTooLow_c                  0x31
#define gFPAPresureTooHigh_c                 0x32
#define gFPAFlowSensorCommError_c            0x33
#define gFPAFlowSensorMeasurementFault_c     0x34
#define gFPAFlowSensorReverseFlow_c          0x35
#define gFPAFlowSensorAirDetect_c            0x36
#define gFPAPipeEmpty_c                      0x37

/* Heat and Cooling Specific Alarm Group */
#define gWSAInletTemperatureSensorFault_c    0x50
#define gWSAOutletTemperatureSensorFault_c   0x51

/* Supply Status Attribute */
#define gSSASupplyOff_c         0x00
#define gSSASupplyOffArmed_c    0x01
#define gSSASupplyOn_c          0x02

/**********************************************
Price cluster definitions
***********************************************/

#if gBigEndian_c
#define gZclAttrClientPrice_PriceIncreaseRandomizeMinutes_c   0x0000
#define gZclAttrClientPrice_PriceDecreaseRandomizeMinutes_c   0x0100
#define gZclAttrClientPrice_CommodityType_c                   0x0200
#else
#define gZclAttrClientPrice_PriceIncreaseRandomizeMinutes_c   0x0000
#define gZclAttrClientPrice_PriceDecreaseRandomizeMinutes_c   0x0001
#define gZclAttrClientPrice_CommodityType_c                   0x0002
#endif

#if gBigEndian_c
    #define gZclAttrPrice_TierLabelSet_c 		0x0000
    #define gZclAttrPrice_BlockThresholdSet_c 	0x0001
    #define gZclAttrPrice_BlockPeriodSet_c 		0x0002
    #define gZclAttrPrice_CommodityTypeSet_c 	0x0003
    #define gZclAttrPrice_BlockPriceInfoSet_c 	0x0004
#else
    #define gZclAttrPrice_TierLabelSet_c 		0x0000
    #define gZclAttrPrice_BlockThresholdSet_c 	0x0100
    #define gZclAttrPrice_BlockPeriodSet_c 		0x0200
    #define gZclAttrPrice_CommodityTypeSet_c 	0x0300
    #define gZclAttrPrice_BlockPriceInfoSet_c 	0x0400
#endif
 
#if gBigEndian_c
	#define gZclAttrPrice_Tier1PriceLabel_c (gZclAttrPrice_TierLabelSet_c | 0x0000)
	#define gZclAttrPrice_Tier2PriceLabel_c (gZclAttrPrice_TierLabelSet_c | 0x0100)
	#define gZclAttrPrice_Tier3PriceLabel_c (gZclAttrPrice_TierLabelSet_c | 0x0200)
	#define gZclAttrPrice_Tier4PriceLabel_c (gZclAttrPrice_TierLabelSet_c | 0x0300)
	#define gZclAttrPrice_Tier5PriceLabel_c (gZclAttrPrice_TierLabelSet_c | 0x0400)
	#define gZclAttrPrice_Tier6PriceLabel_c (gZclAttrPrice_TierLabelSet_c | 0x0500)
	#define gZclAttrPrice_Tier7PriceLabel_c (gZclAttrPrice_TierLabelSet_c | 0x0600)
	#define gZclAttrPrice_Tier8PriceLabel_c (gZclAttrPrice_TierLabelSet_c | 0x0700)
	#define gZclAttrPrice_Tier9PriceLabel_c (gZclAttrPrice_TierLabelSet_c | 0x0800)
	#define gZclAttrPrice_Tier10PriceLabel_c (gZclAttrPrice_TierLabelSet_c | 0x0900)
	#define gZclAttrPrice_Tier11PriceLabel_c (gZclAttrPrice_TierLabelSet_c | 0x0A00)
	#define gZclAttrPrice_Tier12PriceLabel_c (gZclAttrPrice_TierLabelSet_c | 0x0B00)
	#define gZclAttrPrice_Tier13PriceLabel_c (gZclAttrPrice_TierLabelSet_c | 0x0C00)
	#define gZclAttrPrice_Tier14PriceLabel_c (gZclAttrPrice_TierLabelSet_c | 0x0D00)
	#define gZclAttrPrice_Tier15PriceLabel_c (gZclAttrPrice_TierLabelSet_c | 0x0E00)
#else
	#define gZclAttrPrice_Tier1PriceLabel_c (gZclAttrPrice_TierLabelSet_c | 0x0000)
	#define gZclAttrPrice_Tier2PriceLabel_c (gZclAttrPrice_TierLabelSet_c | 0x0001)
	#define gZclAttrPrice_Tier3PriceLabel_c (gZclAttrPrice_TierLabelSet_c | 0x0002)
	#define gZclAttrPrice_Tier4PriceLabel_c (gZclAttrPrice_TierLabelSet_c | 0x0003)
	#define gZclAttrPrice_Tier5PriceLabel_c (gZclAttrPrice_TierLabelSet_c | 0x0004)
	#define gZclAttrPrice_Tier6PriceLabel_c (gZclAttrPrice_TierLabelSet_c | 0x0005)
	#define gZclAttrPrice_Tier7PriceLabel_c (gZclAttrPrice_TierLabelSet_c | 0x0006)
	#define gZclAttrPrice_Tier8PriceLabel_c (gZclAttrPrice_TierLabelSet_c | 0x0007)
	#define gZclAttrPrice_Tier9PriceLabel_c (gZclAttrPrice_TierLabelSet_c | 0x0008)
	#define gZclAttrPrice_Tier10PriceLabel_c (gZclAttrPrice_TierLabelSet_c | 0x0009)
	#define gZclAttrPrice_Tier11PriceLabel_c (gZclAttrPrice_TierLabelSet_c | 0x000A)
	#define gZclAttrPrice_Tier12PriceLabel_c (gZclAttrPrice_TierLabelSet_c | 0x000B)
	#define gZclAttrPrice_Tier13PriceLabel_c (gZclAttrPrice_TierLabelSet_c | 0x000C)
	#define gZclAttrPrice_Tier14PriceLabel_c (gZclAttrPrice_TierLabelSet_c | 0x000D)
	#define gZclAttrPrice_Tier15PriceLabel_c (gZclAttrPrice_TierLabelSet_c | 0x000E)
#endif
#if gBigEndian_c
	#define gZclAttrPrice_Block1Threshold_c (gZclAttrPrice_BlockThresholdSet_c | 0x0000)
	#define gZclAttrPrice_Block2Threshold_c (gZclAttrPrice_BlockThresholdSet_c | 0x0100)
	#define gZclAttrPrice_Block3Threshold_c (gZclAttrPrice_BlockThresholdSet_c | 0x0200)
	#define gZclAttrPrice_Block4Threshold_c (gZclAttrPrice_BlockThresholdSet_c | 0x0300)
	#define gZclAttrPrice_Block5Threshold_c (gZclAttrPrice_BlockThresholdSet_c | 0x0400)
	#define gZclAttrPrice_Block6Threshold_c (gZclAttrPrice_BlockThresholdSet_c | 0x0500)
	#define gZclAttrPrice_Block7Threshold_c (gZclAttrPrice_BlockThresholdSet_c | 0x0600)
	#define gZclAttrPrice_Block8Threshold_c (gZclAttrPrice_BlockThresholdSet_c | 0x0700)
	#define gZclAttrPrice_Block9Threshold_c (gZclAttrPrice_BlockThresholdSet_c | 0x0800)
	#define gZclAttrPrice_Block10Threshold_c (gZclAttrPrice_BlockThresholdSet_c | 0x0900)
	#define gZclAttrPrice_Block11Threshold_c (gZclAttrPrice_BlockThresholdSet_c | 0x0A00)
	#define gZclAttrPrice_Block12Threshold_c (gZclAttrPrice_BlockThresholdSet_c | 0x0B00)
	#define gZclAttrPrice_Block13Threshold_c (gZclAttrPrice_BlockThresholdSet_c | 0x0C00)
	#define gZclAttrPrice_Block14Threshold_c (gZclAttrPrice_BlockThresholdSet_c | 0x0D00)
	#define gZclAttrPrice_Block15Threshold_c (gZclAttrPrice_BlockThresholdSet_c | 0x0E00)
#else
	#define gZclAttrPrice_Block1Threshold_c (gZclAttrPrice_BlockThresholdSet_c | 0x0000)
	#define gZclAttrPrice_Block2Threshold_c (gZclAttrPrice_BlockThresholdSet_c | 0x0001)
	#define gZclAttrPrice_Block3Threshold_c (gZclAttrPrice_BlockThresholdSet_c | 0x0002)
	#define gZclAttrPrice_Block4Threshold_c (gZclAttrPrice_BlockThresholdSet_c | 0x0003)
	#define gZclAttrPrice_Block5Threshold_c (gZclAttrPrice_BlockThresholdSet_c | 0x0004)
	#define gZclAttrPrice_Block6Threshold_c (gZclAttrPrice_BlockThresholdSet_c | 0x0005)
	#define gZclAttrPrice_Block7Threshold_c (gZclAttrPrice_BlockThresholdSet_c | 0x0006)
	#define gZclAttrPrice_Block8Threshold_c (gZclAttrPrice_BlockThresholdSet_c | 0x0007)
	#define gZclAttrPrice_Block9Threshold_c (gZclAttrPrice_BlockThresholdSet_c | 0x0008)
	#define gZclAttrPrice_Block10Threshold_c (gZclAttrPrice_BlockThresholdSet_c | 0x0009)
	#define gZclAttrPrice_Block11Threshold_c (gZclAttrPrice_BlockThresholdSet_c | 0x000A)
	#define gZclAttrPrice_Block12Threshold_c (gZclAttrPrice_BlockThresholdSet_c | 0x000B)
	#define gZclAttrPrice_Block13Threshold_c (gZclAttrPrice_BlockThresholdSet_c | 0x000C)
	#define gZclAttrPrice_Block14Threshold_c (gZclAttrPrice_BlockThresholdSet_c | 0x000D)
	#define gZclAttrPrice_Block15Threshold_c (gZclAttrPrice_BlockThresholdSet_c | 0x000E)
#endif
#if gBigEndian_c
	#define gZclAttrPrice_StartofBlockPeriod_c (gZclAttrPrice_BlockPeriodSet_c | 0x0000)
	#define gZclAttrPrice_BlockPeriodDuration_c (gZclAttrPrice_BlockPeriodSet_c | 0x0100)
        #define gZclAttrPrice_ThresholdMultiplier_c (gZclAttrPrice_BlockPeriodSet_c | 0x0200)
        #define gZclAttrPrice_ThresholdDivisor_c  (gZclAttrPrice_BlockPeriodSet_c | 0x0300)
#else
	#define gZclAttrPrice_StartofBlockPeriod_c (gZclAttrPrice_BlockPeriodSet_c | 0x0000)
	#define gZclAttrPrice_BlockPeriodDuration_c (gZclAttrPrice_BlockPeriodSet_c | 0x0001)
        #define gZclAttrPrice_ThresholdMultiplier_c (gZclAttrPrice_BlockPeriodSet_c | 0x0002)
        #define gZclAttrPrice_ThresholdDivisor_c  (gZclAttrPrice_BlockPeriodSet_c | 0x0003)
#endif

#if gBigEndian_c
	#define gZclAttrPrice_CommodityType_c (gZclAttrPrice_CommodityTypeSet_c | 0x0000)
        #define gZclAttrPrice_StandingCharge_c (gZclAttrPrice_CommodityTypeSet_c | 0x0100)
        #define gZclAttrPrice_ConversionFactor_c (gZclAttrPrice_CommodityTypeSet_c | 0x0200)
        #define gZclAttrPrice_ConversionFactorTrlDigit_c (gZclAttrPrice_CommodityTypeSet_c | 0x0300)
        #define gZclAttrPrice_CalorificValue_c (gZclAttrPrice_CommodityTypeSet_c | 0x0400)
        #define gZclAttrPrice_CalorificValueUnit_c (gZclAttrPrice_CommodityTypeSet_c | 0x0500)
        #define gZclAttrPrice_CalorificValueTrlDigit_c (gZclAttrPrice_CommodityTypeSet_c | 0x0600)
#else
	#define gZclAttrPrice_CommodityType_c (gZclAttrPrice_CommodityTypeSet_c | 0x0000)
        #define gZclAttrPrice_StandingCharge_c (gZclAttrPrice_CommodityTypeSet_c | 0x0001)
        #define gZclAttrPrice_ConversionFactor_c (gZclAttrPrice_CommodityTypeSet_c | 0x0002)
        #define gZclAttrPrice_ConversionFactorTrlDigit_c (gZclAttrPrice_CommodityTypeSet_c | 0x0003)
        #define gZclAttrPrice_CalorificValue_c (gZclAttrPrice_CommodityTypeSet_c | 0x0004)
        #define gZclAttrPrice_CalorificValueUnit_c (gZclAttrPrice_CommodityTypeSet_c | 0x0005)
        #define gZclAttrPrice_CalorificValueTrlDigit_c (gZclAttrPrice_CommodityTypeSet_c | 0x0006)
#endif

#if gBigEndian_c
	#define gZclAttrPrice_NoTierBlock1Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0000)
	#define gZclAttrPrice_NoTierBlock2Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0100)
	#define gZclAttrPrice_NoTierBlock3Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0200)
	#define gZclAttrPrice_NoTierBlock4Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0300)
	#define gZclAttrPrice_NoTierBlock5Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0400)
	#define gZclAttrPrice_NoTierBlock6Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0500)
	#define gZclAttrPrice_NoTierBlock7Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0600)
	#define gZclAttrPrice_NoTierBlock8Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0700)
	#define gZclAttrPrice_NoTierBlock9Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0800)
	#define gZclAttrPrice_NoTierBlock10Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0900)
	#define gZclAttrPrice_NoTierBlock11Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0A00)
	#define gZclAttrPrice_NoTierBlock12Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0B00)
	#define gZclAttrPrice_NoTierBlock13Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0C00)
	#define gZclAttrPrice_NoTierBlock14Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0D00)
	#define gZclAttrPrice_NoTierBlock15Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0E00)
	#define gZclAttrPrice_NoTierBlock16Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0F00)
	#define gZclAttrPrice_Tier1Block1Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x1000)
	#define gZclAttrPrice_Tier1Block2Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x1100)
	#define gZclAttrPrice_Tier1Block3Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x1200)
	#define gZclAttrPrice_Tier1Block4Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x1300)
	#define gZclAttrPrice_Tier1Block5Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x1400)
	#define gZclAttrPrice_Tier1Block6Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x1500)
	#define gZclAttrPrice_Tier1Block7Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x1600)
	#define gZclAttrPrice_Tier1Block8Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x1700)
	#define gZclAttrPrice_Tier1Block9Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x1800)
	#define gZclAttrPrice_Tier1Block10Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x1900)
	#define gZclAttrPrice_Tier1Block11Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x1A00)
	#define gZclAttrPrice_Tier1Block12Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x1B00)
	#define gZclAttrPrice_Tier1Block13Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x1C00)
	#define gZclAttrPrice_Tier1Block14Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x1D00)
	#define gZclAttrPrice_Tier1Block15Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x1E00)
	#define gZclAttrPrice_Tier1Block16Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x1F00)
	#define gZclAttrPrice_Tier2Block1Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x2000)
	#define gZclAttrPrice_Tier2Block2Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x2100)
	#define gZclAttrPrice_Tier2Block3Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x2200)
	#define gZclAttrPrice_Tier2Block4Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x2300)
	#define gZclAttrPrice_Tier2Block5Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x2400)
	#define gZclAttrPrice_Tier2Block6Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x2500)
	#define gZclAttrPrice_Tier2Block7Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x2600)
	#define gZclAttrPrice_Tier2Block8Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x2700)
	#define gZclAttrPrice_Tier2Block9Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x2800)
	#define gZclAttrPrice_Tier2Block10Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x2900)
	#define gZclAttrPrice_Tier2Block11Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x2A00)
	#define gZclAttrPrice_Tier2Block12Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x2B00)
	#define gZclAttrPrice_Tier2Block13Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x2C00)
	#define gZclAttrPrice_Tier2Block14Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x2D00)
	#define gZclAttrPrice_Tier2Block15Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x2E00)
	#define gZclAttrPrice_Tier2Block16Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x2F00)
	#define gZclAttrPrice_Tier3Block1Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x3000)
	#define gZclAttrPrice_Tier3Block2Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x3100)
	#define gZclAttrPrice_Tier3Block3Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x3200)
	#define gZclAttrPrice_Tier3Block4Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x3300)
	#define gZclAttrPrice_Tier3Block5Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x3400)
	#define gZclAttrPrice_Tier3Block6Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x3500)
	#define gZclAttrPrice_Tier3Block7Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x3600)
	#define gZclAttrPrice_Tier3Block8Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x3700)
	#define gZclAttrPrice_Tier3Block9Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x3800)
	#define gZclAttrPrice_Tier3Block10Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x3900)
	#define gZclAttrPrice_Tier3Block11Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x3A00)
	#define gZclAttrPrice_Tier3Block12Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x3B00)
	#define gZclAttrPrice_Tier3Block13Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x3C00)
	#define gZclAttrPrice_Tier3Block14Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x3D00)
	#define gZclAttrPrice_Tier3Block15Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x3E00)
	#define gZclAttrPrice_Tier3Block16Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x3F00)
	#define gZclAttrPrice_Tier4Block1Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x4000)
	#define gZclAttrPrice_Tier4Block2Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x4100)
	#define gZclAttrPrice_Tier4Block3Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x4200)
	#define gZclAttrPrice_Tier4Block4Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x4300)
	#define gZclAttrPrice_Tier4Block5Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x4400)
	#define gZclAttrPrice_Tier4Block6Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x4500)
	#define gZclAttrPrice_Tier4Block7Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x4600)
	#define gZclAttrPrice_Tier4Block8Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x4700)
	#define gZclAttrPrice_Tier4Block9Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x4800)
	#define gZclAttrPrice_Tier4Block10Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x4900)
	#define gZclAttrPrice_Tier4Block11Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x4A00)
	#define gZclAttrPrice_Tier4Block12Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x4B00)
	#define gZclAttrPrice_Tier4Block13Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x4C00)
	#define gZclAttrPrice_Tier4Block14Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x4D00)
	#define gZclAttrPrice_Tier4Block15Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x4E00)
	#define gZclAttrPrice_Tier4Block16Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x4F00)
	#define gZclAttrPrice_Tier5Block1Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x5000)
	#define gZclAttrPrice_Tier5Block2Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x5100)
	#define gZclAttrPrice_Tier5Block3Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x5200)
	#define gZclAttrPrice_Tier5Block4Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x5300)
	#define gZclAttrPrice_Tier5Block5Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x5400)
	#define gZclAttrPrice_Tier5Block6Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x5500)
	#define gZclAttrPrice_Tier5Block7Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x5600)
	#define gZclAttrPrice_Tier5Block8Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x5700)
	#define gZclAttrPrice_Tier5Block9Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x5800)
	#define gZclAttrPrice_Tier5Block10Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x5900)
	#define gZclAttrPrice_Tier5Block11Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x5A00)
	#define gZclAttrPrice_Tier5Block12Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x5B00)
	#define gZclAttrPrice_Tier5Block13Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x5C00)
	#define gZclAttrPrice_Tier5Block14Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x5D00)
	#define gZclAttrPrice_Tier5Block15Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x5E00)
	#define gZclAttrPrice_Tier5Block16Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x5F00)
	#define gZclAttrPrice_Tier6Block1Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x6000)
	#define gZclAttrPrice_Tier6Block2Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x6100)
	#define gZclAttrPrice_Tier6Block3Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x6200)
	#define gZclAttrPrice_Tier6Block4Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x6300)
	#define gZclAttrPrice_Tier6Block5Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x6400)
	#define gZclAttrPrice_Tier6Block6Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x6500)
	#define gZclAttrPrice_Tier6Block7Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x6600)
	#define gZclAttrPrice_Tier6Block8Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x6700)
	#define gZclAttrPrice_Tier6Block9Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x6800)
	#define gZclAttrPrice_Tier6Block10Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x6900)
	#define gZclAttrPrice_Tier6Block11Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x6A00)
	#define gZclAttrPrice_Tier6Block12Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x6B00)
	#define gZclAttrPrice_Tier6Block13Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x6C00)
	#define gZclAttrPrice_Tier6Block14Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x6D00)
	#define gZclAttrPrice_Tier6Block15Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x6E00)
	#define gZclAttrPrice_Tier6Block16Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x6F00)
	#define gZclAttrPrice_Tier7Block1Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x7000)
	#define gZclAttrPrice_Tier7Block2Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x7100)
	#define gZclAttrPrice_Tier7Block3Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x7200)
	#define gZclAttrPrice_Tier7Block4Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x7300)
	#define gZclAttrPrice_Tier7Block5Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x7400)
	#define gZclAttrPrice_Tier7Block6Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x7500)
	#define gZclAttrPrice_Tier7Block7Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x7600)
	#define gZclAttrPrice_Tier7Block8Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x7700)
	#define gZclAttrPrice_Tier7Block9Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x7800)
	#define gZclAttrPrice_Tier7Block10Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x7900)
	#define gZclAttrPrice_Tier7Block11Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x7A00)
	#define gZclAttrPrice_Tier7Block12Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x7B00)
	#define gZclAttrPrice_Tier7Block13Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x7C00)
	#define gZclAttrPrice_Tier7Block14Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x7D00)
	#define gZclAttrPrice_Tier7Block15Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x7E00)
	#define gZclAttrPrice_Tier7Block16Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x7F00)
	#define gZclAttrPrice_Tier8Block1Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x8000)
	#define gZclAttrPrice_Tier8Block2Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x8100)
	#define gZclAttrPrice_Tier8Block3Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x8200)
	#define gZclAttrPrice_Tier8Block4Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x8300)
	#define gZclAttrPrice_Tier8Block5Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x8400)
	#define gZclAttrPrice_Tier8Block6Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x8500)
	#define gZclAttrPrice_Tier8Block7Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x8600)
	#define gZclAttrPrice_Tier8Block8Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x8700)
	#define gZclAttrPrice_Tier8Block9Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x8800)
	#define gZclAttrPrice_Tier8Block10Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x8900)
	#define gZclAttrPrice_Tier8Block11Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x8A00)
	#define gZclAttrPrice_Tier8Block12Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x8B00)
	#define gZclAttrPrice_Tier8Block13Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x8C00)
	#define gZclAttrPrice_Tier8Block14Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x8D00)
	#define gZclAttrPrice_Tier8Block15Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x8E00)
	#define gZclAttrPrice_Tier8Block16Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x8F00)
	#define gZclAttrPrice_Tier9Block1Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x9000)
	#define gZclAttrPrice_Tier9Block2Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x9100)
	#define gZclAttrPrice_Tier9Block3Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x9200)
	#define gZclAttrPrice_Tier9Block4Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x9300)
	#define gZclAttrPrice_Tier9Block5Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x9400)
	#define gZclAttrPrice_Tier9Block6Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x9500)
	#define gZclAttrPrice_Tier9Block7Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x9600)
	#define gZclAttrPrice_Tier9Block8Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x9700)
	#define gZclAttrPrice_Tier9Block9Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x9800)
	#define gZclAttrPrice_Tier9Block10Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x9900)
	#define gZclAttrPrice_Tier9Block11Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x9A00)
	#define gZclAttrPrice_Tier9Block12Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x9B00)
	#define gZclAttrPrice_Tier9Block13Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x9C00)
	#define gZclAttrPrice_Tier9Block14Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x9D00)
	#define gZclAttrPrice_Tier9Block15Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x9E00)
	#define gZclAttrPrice_Tier9Block16Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x9F00)
	#define gZclAttrPrice_Tier10Block1Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xA000)
	#define gZclAttrPrice_Tier10Block2Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xA100)
	#define gZclAttrPrice_Tier10Block3Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xA200)
	#define gZclAttrPrice_Tier10Block4Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xA300)
	#define gZclAttrPrice_Tier10Block5Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xA400)
	#define gZclAttrPrice_Tier10Block6Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xA500)
	#define gZclAttrPrice_Tier10Block7Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xA600)
	#define gZclAttrPrice_Tier10Block8Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xA700)
	#define gZclAttrPrice_Tier10Block9Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xA800)
	#define gZclAttrPrice_Tier10Block10Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xA900)
	#define gZclAttrPrice_Tier10Block11Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xAA00)
	#define gZclAttrPrice_Tier10Block12Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xAB00)
	#define gZclAttrPrice_Tier10Block13Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xAC00)
	#define gZclAttrPrice_Tier10Block14Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xAD00)
	#define gZclAttrPrice_Tier10Block15Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xAE00)
	#define gZclAttrPrice_Tier10Block16Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xAF00)
	#define gZclAttrPrice_Tier11Block1Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xB000)
	#define gZclAttrPrice_Tier11Block2Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xB100)
	#define gZclAttrPrice_Tier11Block3Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xB200)
	#define gZclAttrPrice_Tier11Block4Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xB300)
	#define gZclAttrPrice_Tier11Block5Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xB400)
	#define gZclAttrPrice_Tier11Block6Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xB500)
	#define gZclAttrPrice_Tier11Block7Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xB600)
	#define gZclAttrPrice_Tier11Block8Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xB700)
	#define gZclAttrPrice_Tier11Block9Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xB800)
	#define gZclAttrPrice_Tier11Block10Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xB900)
	#define gZclAttrPrice_Tier11Block11Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xBA00)
	#define gZclAttrPrice_Tier11Block12Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xBB00)
	#define gZclAttrPrice_Tier11Block13Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xBC00)
	#define gZclAttrPrice_Tier11Block14Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xBD00)
	#define gZclAttrPrice_Tier11Block15Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xBE00)
	#define gZclAttrPrice_Tier11Block16Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xBF00)
	#define gZclAttrPrice_Tier12Block1Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xC000)
	#define gZclAttrPrice_Tier12Block2Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xC100)
	#define gZclAttrPrice_Tier12Block3Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xC200)
	#define gZclAttrPrice_Tier12Block4Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xC300)
	#define gZclAttrPrice_Tier12Block5Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xC400)
	#define gZclAttrPrice_Tier12Block6Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xC500)
	#define gZclAttrPrice_Tier12Block7Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xC600)
	#define gZclAttrPrice_Tier12Block8Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xC700)
	#define gZclAttrPrice_Tier12Block9Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xC800)
	#define gZclAttrPrice_Tier12Block10Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xC900)
	#define gZclAttrPrice_Tier12Block11Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xCA00)
	#define gZclAttrPrice_Tier12Block12Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xCB00)
	#define gZclAttrPrice_Tier12Block13Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xCC00)
	#define gZclAttrPrice_Tier12Block14Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xCD00)
	#define gZclAttrPrice_Tier12Block15Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xCE00)
	#define gZclAttrPrice_Tier12Block16Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xCF00)
	#define gZclAttrPrice_Tier13Block1Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xD000)
	#define gZclAttrPrice_Tier13Block2Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xD100)
	#define gZclAttrPrice_Tier13Block3Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xD200)
	#define gZclAttrPrice_Tier13Block4Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xD300)
	#define gZclAttrPrice_Tier13Block5Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xD400)
	#define gZclAttrPrice_Tier13Block6Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xD500)
	#define gZclAttrPrice_Tier13Block7Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xD600)
	#define gZclAttrPrice_Tier13Block8Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xD700)
	#define gZclAttrPrice_Tier13Block9Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xD800)
	#define gZclAttrPrice_Tier13Block10Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xD900)
	#define gZclAttrPrice_Tier13Block11Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xDA00)
	#define gZclAttrPrice_Tier13Block12Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xDB00)
	#define gZclAttrPrice_Tier13Block13Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xDC00)
	#define gZclAttrPrice_Tier13Block14Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xDD00)
	#define gZclAttrPrice_Tier13Block15Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xDE00)
	#define gZclAttrPrice_Tier13Block16Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xDF00)
	#define gZclAttrPrice_Tier14Block1Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xE000)
	#define gZclAttrPrice_Tier14Block2Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xE100)
	#define gZclAttrPrice_Tier14Block3Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xE200)
	#define gZclAttrPrice_Tier14Block4Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xE300)
	#define gZclAttrPrice_Tier14Block5Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xE400)
	#define gZclAttrPrice_Tier14Block6Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xE500)
	#define gZclAttrPrice_Tier14Block7Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xE600)
	#define gZclAttrPrice_Tier14Block8Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xE700)
	#define gZclAttrPrice_Tier14Block9Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xE800)
	#define gZclAttrPrice_Tier14Block10Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xE900)
	#define gZclAttrPrice_Tier14Block11Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xEA00)
	#define gZclAttrPrice_Tier14Block12Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xEB00)
	#define gZclAttrPrice_Tier14Block13Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xEC00)
	#define gZclAttrPrice_Tier14Block14Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xED00)
	#define gZclAttrPrice_Tier14Block15Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xEE00)
	#define gZclAttrPrice_Tier14Block16Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xEF00)
	#define gZclAttrPrice_Tier15Block1Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xF000)
	#define gZclAttrPrice_Tier15Block2Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xF100)
	#define gZclAttrPrice_Tier15Block3Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xF200)
	#define gZclAttrPrice_Tier15Block4Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xF300)
	#define gZclAttrPrice_Tier15Block5Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xF400)
	#define gZclAttrPrice_Tier15Block6Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xF500)
	#define gZclAttrPrice_Tier15Block7Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xF600)
	#define gZclAttrPrice_Tier15Block8Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xF700)
	#define gZclAttrPrice_Tier15Block9Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xF800)
	#define gZclAttrPrice_Tier15Block10Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xF900)
	#define gZclAttrPrice_Tier15Block11Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xFA00)
	#define gZclAttrPrice_Tier15Block12Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xFB00)
	#define gZclAttrPrice_Tier15Block13Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xFC00)
	#define gZclAttrPrice_Tier15Block14Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xFD00)
	#define gZclAttrPrice_Tier15Block15Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xFE00)
	#define gZclAttrPrice_Tier15Block16Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0xFF00)
#else
	#define gZclAttrPrice_NoTierBlock1Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0000)
	#define gZclAttrPrice_NoTierBlock2Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0001)
	#define gZclAttrPrice_NoTierBlock3Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0002)
	#define gZclAttrPrice_NoTierBlock4Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0003)
	#define gZclAttrPrice_NoTierBlock5Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0004)
	#define gZclAttrPrice_NoTierBlock6Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0005)
	#define gZclAttrPrice_NoTierBlock7Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0006)
	#define gZclAttrPrice_NoTierBlock8Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0007)
	#define gZclAttrPrice_NoTierBlock9Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0008)
	#define gZclAttrPrice_NoTierBlock10Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0009)
	#define gZclAttrPrice_NoTierBlock11Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x000A)
	#define gZclAttrPrice_NoTierBlock12Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x000B)
	#define gZclAttrPrice_NoTierBlock13Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x000C)
	#define gZclAttrPrice_NoTierBlock14Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x000D)
	#define gZclAttrPrice_NoTierBlock15Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x000E)
	#define gZclAttrPrice_NoTierBlock16Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x000F)
	#define gZclAttrPrice_Tier1Block1Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0010)
	#define gZclAttrPrice_Tier1Block2Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0011)
	#define gZclAttrPrice_Tier1Block3Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0012)
	#define gZclAttrPrice_Tier1Block4Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0013)
	#define gZclAttrPrice_Tier1Block5Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0014)
	#define gZclAttrPrice_Tier1Block6Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0015)
	#define gZclAttrPrice_Tier1Block7Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0016)
	#define gZclAttrPrice_Tier1Block8Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0017)
	#define gZclAttrPrice_Tier1Block9Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0018)
	#define gZclAttrPrice_Tier1Block10Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0019)
	#define gZclAttrPrice_Tier1Block11Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x001A)
	#define gZclAttrPrice_Tier1Block12Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x001B)
	#define gZclAttrPrice_Tier1Block13Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x001C)
	#define gZclAttrPrice_Tier1Block14Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x001D)
	#define gZclAttrPrice_Tier1Block15Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x001E)
	#define gZclAttrPrice_Tier1Block16Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x001F)
	#define gZclAttrPrice_Tier2Block1Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0020)
	#define gZclAttrPrice_Tier2Block2Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0021)
	#define gZclAttrPrice_Tier2Block3Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0022)
	#define gZclAttrPrice_Tier2Block4Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0023)
	#define gZclAttrPrice_Tier2Block5Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0024)
	#define gZclAttrPrice_Tier2Block6Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0025)
	#define gZclAttrPrice_Tier2Block7Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0026)
	#define gZclAttrPrice_Tier2Block8Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0027)
	#define gZclAttrPrice_Tier2Block9Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0028)
	#define gZclAttrPrice_Tier2Block10Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0029)
	#define gZclAttrPrice_Tier2Block11Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x002A)
	#define gZclAttrPrice_Tier2Block12Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x002B)
	#define gZclAttrPrice_Tier2Block13Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x002C)
	#define gZclAttrPrice_Tier2Block14Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x002D)
	#define gZclAttrPrice_Tier2Block15Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x002E)
	#define gZclAttrPrice_Tier2Block16Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x002F)
	#define gZclAttrPrice_Tier3Block1Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0030)
	#define gZclAttrPrice_Tier3Block2Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0031)
	#define gZclAttrPrice_Tier3Block3Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0032)
	#define gZclAttrPrice_Tier3Block4Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0033)
	#define gZclAttrPrice_Tier3Block5Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0034)
	#define gZclAttrPrice_Tier3Block6Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0035)
	#define gZclAttrPrice_Tier3Block7Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0036)
	#define gZclAttrPrice_Tier3Block8Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0037)
	#define gZclAttrPrice_Tier3Block9Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0038)
	#define gZclAttrPrice_Tier3Block10Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0039)
	#define gZclAttrPrice_Tier3Block11Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x003A)
	#define gZclAttrPrice_Tier3Block12Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x003B)
	#define gZclAttrPrice_Tier3Block13Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x003C)
	#define gZclAttrPrice_Tier3Block14Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x003D)
	#define gZclAttrPrice_Tier3Block15Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x003E)
	#define gZclAttrPrice_Tier3Block16Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x003F)
	#define gZclAttrPrice_Tier4Block1Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0040)
	#define gZclAttrPrice_Tier4Block2Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0041)
	#define gZclAttrPrice_Tier4Block3Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0042)
	#define gZclAttrPrice_Tier4Block4Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0043)
	#define gZclAttrPrice_Tier4Block5Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0044)
	#define gZclAttrPrice_Tier4Block6Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0045)
	#define gZclAttrPrice_Tier4Block7Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0046)
	#define gZclAttrPrice_Tier4Block8Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0047)
	#define gZclAttrPrice_Tier4Block9Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0048)
	#define gZclAttrPrice_Tier4Block10Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0049)
	#define gZclAttrPrice_Tier4Block11Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x004A)
	#define gZclAttrPrice_Tier4Block12Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x004B)
	#define gZclAttrPrice_Tier4Block13Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x004C)
	#define gZclAttrPrice_Tier4Block14Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x004D)
	#define gZclAttrPrice_Tier4Block15Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x004E)
	#define gZclAttrPrice_Tier4Block16Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x004F)
	#define gZclAttrPrice_Tier5Block1Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0050)
	#define gZclAttrPrice_Tier5Block2Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0051)
	#define gZclAttrPrice_Tier5Block3Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0052)
	#define gZclAttrPrice_Tier5Block4Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0053)
	#define gZclAttrPrice_Tier5Block5Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0054)
	#define gZclAttrPrice_Tier5Block6Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0055)
	#define gZclAttrPrice_Tier5Block7Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0056)
	#define gZclAttrPrice_Tier5Block8Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0057)
	#define gZclAttrPrice_Tier5Block9Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0058)
	#define gZclAttrPrice_Tier5Block10Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0059)
	#define gZclAttrPrice_Tier5Block11Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x005A)
	#define gZclAttrPrice_Tier5Block12Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x005B)
	#define gZclAttrPrice_Tier5Block13Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x005C)
	#define gZclAttrPrice_Tier5Block14Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x005D)
	#define gZclAttrPrice_Tier5Block15Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x005E)
	#define gZclAttrPrice_Tier5Block16Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x005F)
	#define gZclAttrPrice_Tier6Block1Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0060)
	#define gZclAttrPrice_Tier6Block2Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0061)
	#define gZclAttrPrice_Tier6Block3Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0062)
	#define gZclAttrPrice_Tier6Block4Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0063)
	#define gZclAttrPrice_Tier6Block5Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0064)
	#define gZclAttrPrice_Tier6Block6Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0065)
	#define gZclAttrPrice_Tier6Block7Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0066)
	#define gZclAttrPrice_Tier6Block8Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0067)
	#define gZclAttrPrice_Tier6Block9Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0068)
	#define gZclAttrPrice_Tier6Block10Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0069)
	#define gZclAttrPrice_Tier6Block11Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x006A)
	#define gZclAttrPrice_Tier6Block12Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x006B)
	#define gZclAttrPrice_Tier6Block13Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x006C)
	#define gZclAttrPrice_Tier6Block14Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x006D)
	#define gZclAttrPrice_Tier6Block15Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x006E)
	#define gZclAttrPrice_Tier6Block16Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x006F)
	#define gZclAttrPrice_Tier7Block1Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0070)
	#define gZclAttrPrice_Tier7Block2Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0071)
	#define gZclAttrPrice_Tier7Block3Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0072)
	#define gZclAttrPrice_Tier7Block4Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0073)
	#define gZclAttrPrice_Tier7Block5Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0074)
	#define gZclAttrPrice_Tier7Block6Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0075)
	#define gZclAttrPrice_Tier7Block7Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0076)
	#define gZclAttrPrice_Tier7Block8Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0077)
	#define gZclAttrPrice_Tier7Block9Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0078)
	#define gZclAttrPrice_Tier7Block10Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0079)
	#define gZclAttrPrice_Tier7Block11Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x007A)
	#define gZclAttrPrice_Tier7Block12Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x007B)
	#define gZclAttrPrice_Tier7Block13Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x007C)
	#define gZclAttrPrice_Tier7Block14Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x007D)
	#define gZclAttrPrice_Tier7Block15Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x007E)
	#define gZclAttrPrice_Tier7Block16Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x007F)
	#define gZclAttrPrice_Tier8Block1Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0080)
	#define gZclAttrPrice_Tier8Block2Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0081)
	#define gZclAttrPrice_Tier8Block3Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0082)
	#define gZclAttrPrice_Tier8Block4Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0083)
	#define gZclAttrPrice_Tier8Block5Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0084)
	#define gZclAttrPrice_Tier8Block6Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0085)
	#define gZclAttrPrice_Tier8Block7Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0086)
	#define gZclAttrPrice_Tier8Block8Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0087)
	#define gZclAttrPrice_Tier8Block9Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0088)
	#define gZclAttrPrice_Tier8Block10Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0089)
	#define gZclAttrPrice_Tier8Block11Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x008A)
	#define gZclAttrPrice_Tier8Block12Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x008B)
	#define gZclAttrPrice_Tier8Block13Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x008C)
	#define gZclAttrPrice_Tier8Block14Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x008D)
	#define gZclAttrPrice_Tier8Block15Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x008E)
	#define gZclAttrPrice_Tier8Block16Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x008F)
	#define gZclAttrPrice_Tier9Block1Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0090)
	#define gZclAttrPrice_Tier9Block2Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0091)
	#define gZclAttrPrice_Tier9Block3Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0092)
	#define gZclAttrPrice_Tier9Block4Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0093)
	#define gZclAttrPrice_Tier9Block5Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0094)
	#define gZclAttrPrice_Tier9Block6Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0095)
	#define gZclAttrPrice_Tier9Block7Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0096)
	#define gZclAttrPrice_Tier9Block8Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0097)
	#define gZclAttrPrice_Tier9Block9Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0098)
	#define gZclAttrPrice_Tier9Block10Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x0099)
	#define gZclAttrPrice_Tier9Block11Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x009A)
	#define gZclAttrPrice_Tier9Block12Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x009B)
	#define gZclAttrPrice_Tier9Block13Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x009C)
	#define gZclAttrPrice_Tier9Block14Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x009D)
	#define gZclAttrPrice_Tier9Block15Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x009E)
	#define gZclAttrPrice_Tier9Block16Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x009F)
	#define gZclAttrPrice_Tier10Block1Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00A0)
	#define gZclAttrPrice_Tier10Block2Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00A1)
	#define gZclAttrPrice_Tier10Block3Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00A2)
	#define gZclAttrPrice_Tier10Block4Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00A3)
	#define gZclAttrPrice_Tier10Block5Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00A4)
	#define gZclAttrPrice_Tier10Block6Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00A5)
	#define gZclAttrPrice_Tier10Block7Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00A6)
	#define gZclAttrPrice_Tier10Block8Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00A7)
	#define gZclAttrPrice_Tier10Block9Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00A8)
	#define gZclAttrPrice_Tier10Block10Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00A9)
	#define gZclAttrPrice_Tier10Block11Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00AA)
	#define gZclAttrPrice_Tier10Block12Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00AB)
	#define gZclAttrPrice_Tier10Block13Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00AC)
	#define gZclAttrPrice_Tier10Block14Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00AD)
	#define gZclAttrPrice_Tier10Block15Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00AE)
	#define gZclAttrPrice_Tier10Block16Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00AF)
	#define gZclAttrPrice_Tier11Block1Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00B0)
	#define gZclAttrPrice_Tier11Block2Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00B1)
	#define gZclAttrPrice_Tier11Block3Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00B2)
	#define gZclAttrPrice_Tier11Block4Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00B3)
	#define gZclAttrPrice_Tier11Block5Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00B4)
	#define gZclAttrPrice_Tier11Block6Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00B5)
	#define gZclAttrPrice_Tier11Block7Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00B6)
	#define gZclAttrPrice_Tier11Block8Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00B7)
	#define gZclAttrPrice_Tier11Block9Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00B8)
	#define gZclAttrPrice_Tier11Block10Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00B9)
	#define gZclAttrPrice_Tier11Block11Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00BA)
	#define gZclAttrPrice_Tier11Block12Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00BB)
	#define gZclAttrPrice_Tier11Block13Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00BC)
	#define gZclAttrPrice_Tier11Block14Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00BD)
	#define gZclAttrPrice_Tier11Block15Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00BE)
	#define gZclAttrPrice_Tier11Block16Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00BF)
	#define gZclAttrPrice_Tier12Block1Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00C0)
	#define gZclAttrPrice_Tier12Block2Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00C1)
	#define gZclAttrPrice_Tier12Block3Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00C2)
	#define gZclAttrPrice_Tier12Block4Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00C3)
	#define gZclAttrPrice_Tier12Block5Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00C4)
	#define gZclAttrPrice_Tier12Block6Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00C5)
	#define gZclAttrPrice_Tier12Block7Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00C6)
	#define gZclAttrPrice_Tier12Block8Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00C7)
	#define gZclAttrPrice_Tier12Block9Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00C8)
	#define gZclAttrPrice_Tier12Block10Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00C9)
	#define gZclAttrPrice_Tier12Block11Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00CA)
	#define gZclAttrPrice_Tier12Block12Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00CB)
	#define gZclAttrPrice_Tier12Block13Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00CC)
	#define gZclAttrPrice_Tier12Block14Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00CD)
	#define gZclAttrPrice_Tier12Block15Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00CE)
	#define gZclAttrPrice_Tier12Block16Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00CF)
	#define gZclAttrPrice_Tier13Block1Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00D0)
	#define gZclAttrPrice_Tier13Block2Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00D1)
	#define gZclAttrPrice_Tier13Block3Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00D2)
	#define gZclAttrPrice_Tier13Block4Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00D3)
	#define gZclAttrPrice_Tier13Block5Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00D4)
	#define gZclAttrPrice_Tier13Block6Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00D5)
	#define gZclAttrPrice_Tier13Block7Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00D6)
	#define gZclAttrPrice_Tier13Block8Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00D7)
	#define gZclAttrPrice_Tier13Block9Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00D8)
	#define gZclAttrPrice_Tier13Block10Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00D9)
	#define gZclAttrPrice_Tier13Block11Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00DA)
	#define gZclAttrPrice_Tier13Block12Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00DB)
	#define gZclAttrPrice_Tier13Block13Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00DC)
	#define gZclAttrPrice_Tier13Block14Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00DD)
	#define gZclAttrPrice_Tier13Block15Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00DE)
	#define gZclAttrPrice_Tier13Block16Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00DF)
	#define gZclAttrPrice_Tier14Block1Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00E0)
	#define gZclAttrPrice_Tier14Block2Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00E1)
	#define gZclAttrPrice_Tier14Block3Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00E2)
	#define gZclAttrPrice_Tier14Block4Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00E3)
	#define gZclAttrPrice_Tier14Block5Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00E4)
	#define gZclAttrPrice_Tier14Block6Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00E5)
	#define gZclAttrPrice_Tier14Block7Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00E6)
	#define gZclAttrPrice_Tier14Block8Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00E7)
	#define gZclAttrPrice_Tier14Block9Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00E8)
	#define gZclAttrPrice_Tier14Block10Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00E9)
	#define gZclAttrPrice_Tier14Block11Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00EA)
	#define gZclAttrPrice_Tier14Block12Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00EB)
	#define gZclAttrPrice_Tier14Block13Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00EC)
	#define gZclAttrPrice_Tier14Block14Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00ED)
	#define gZclAttrPrice_Tier14Block15Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00EE)
	#define gZclAttrPrice_Tier14Block16Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00EF)
	#define gZclAttrPrice_Tier15Block1Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00F0)
	#define gZclAttrPrice_Tier15Block2Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00F1)
	#define gZclAttrPrice_Tier15Block3Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00F2)
	#define gZclAttrPrice_Tier15Block4Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00F3)
	#define gZclAttrPrice_Tier15Block5Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00F4)
	#define gZclAttrPrice_Tier15Block6Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00F5)
	#define gZclAttrPrice_Tier15Block7Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00F6)
	#define gZclAttrPrice_Tier15Block8Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00F7)
	#define gZclAttrPrice_Tier15Block9Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00F8)
	#define gZclAttrPrice_Tier15Block10Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00F9)
	#define gZclAttrPrice_Tier15Block11Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00FA)
	#define gZclAttrPrice_Tier15Block12Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00FB)
	#define gZclAttrPrice_Tier15Block13Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00FC)
	#define gZclAttrPrice_Tier15Block14Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00FD)
	#define gZclAttrPrice_Tier15Block15Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00FE)
	#define gZclAttrPrice_Tier15Block16Price_c (gZclAttrPrice_BlockPriceInfoSet_c | 0x00FF)
#endif


#define gZclCmdPrice_GetCurrPriceReq_c                 0x00 /* M */
#define gZclCmdPrice_GetScheduledPricesReq_c           0x01 /* O */
#define gZclCmdPrice_PriceAck_c                        0x02 /* M */
#define gZclCmdPrice_GetBlockPeriods_c                 0x03 /* O */
#define gZclCmdPrice_GetConversionFactor_c             0x04 /* O */
#define gZclCmdPrice_GetCalorificValue_c               0x05 /* O */
#define gZclCmdPrice_GetTariffInformation_c            0x10 /* O / M UK */
#define gZclCmdPrice_GetPriceMatrix_c                  0x11 /* O / M UK */
#define gZclCmdPrice_GetBlockThresholds_c              0x12 /* O / M UK */
#define gZclCmdPrice_GetCO2Value_c                     0x15 /* O / M UK */
#define gZclCmdPrice_GetBillingPeriod_c                0x20 /* O / M UK */
#define gZclCmdPrice_GetConsolidatedBill_c             0x21 /* O / M UK */
#define gZclCmdPrice_GetCPPEventResponse_c             0x30 /* O / O UK */


#define gZclCmdPrice_PublishPriceRsp_c                 0x00 /* M */
#define gZclCmdPrice_PublishBlockPeriodRsp_c           0x01 /* O */
#define gZclCmdPrice_PublishConversionFactorRsp_c      0x02 /* O */
#define gZclCmdPrice_PublishCalorificValueRsp_c        0x03 /* O */
#define gZclCmdPrice_PublishTariffInformationRsp_c     0x10 /* O / M UK */
#define gZclCmdPrice_PublishPriceMatrixRsp_c           0x11 /* O / M UK */
#define gZclCmdPrice_PublishBlockThresholdsRsp_c       0x12 /* O / M UK */
#define gZclCmdPrice_PublishCO2ValueRsp_c              0x15 /* O / M UK */
#define gZclCmdPrice_PublishCPPEventRsp_c              0x16 /* O / M UK */
#define gZclCmdPrice_PublishBillingPeriodRsp_c         0x20 /* O / M UK*/
#define gZclCmdPrice_PublishConsolidatedBillRsp_c      0x21 /* O / M UK*/

#define gGetCurrPrice_RequestorRxOnWhenIdle_c       0x01

#define gAltCostUnit_KgOfCO2perUnit_c 0x01 

/* Price Tier Sub-field Enumerations */
#define gPriceTierNoTierRelated_c 0x0 
#define gPriceTierTier1PriceLabel_c 0x1 
#define gPriceTierTier2PriceLabel_c 0x2 
#define gPriceTierTier3PriceLabel_c 0x3 
#define gPriceTierTier4PriceLabel_c 0x4 
#define gPriceTierTier5PriceLabel_c 0x5 
#define gPriceTierTier6PriceLabel_c 0x6 

/* Register Tier Sub-field Enumerations */
#define gRgstrTierNoTierRelated_c 0x0
#define gRgstrTierCurrTier1SummDlvrd 0x1 
#define gRgstrTierCurrTier2SummDlvrd 0x2 
#define gRgstrTierCurrTier3SummDlvrd 0x3 
#define gRgstrTierCurrTier4SummDlvrd 0x4 
#define gRgstrTierCurrTier5SummDlvrd 0x5 
#define gRgstrTierCurrTier6SummDlvrd 0x6
/* CPP Autorization Status*/
#define gPriceCPPEventPending   0x00
#define gPriceCPPEventAccepted  0x01
#define gPriceCPPEventRejected  0x02
#define gPriceCPPEventForced    0x03

typedef struct zclCmdPrice_GetCalorificValueReq_tag
{
  ZCLTime_t StartTime;
  uint8_t NumOfEvts;
} zclCmdPrice_GetCalorificValueReq_t;

typedef struct  zclPrice_GetCalorificValueReq_tag
{  
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdPrice_GetCalorificValueReq_t cmdFrame;
} zclPrice_GetCalorificValueReq_t;

typedef struct zclCmdPrice_GetConversionFactorReq_tag
{
  ZCLTime_t StartTime;
  uint8_t NumOfEvts;
} zclCmdPrice_GetConversionFactorReq_t;

typedef struct  zclPrice_GetConversionFactorReq_tag
{  
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdPrice_GetConversionFactorReq_t cmdFrame;
} zclPrice_GetConversionFactorReq_t;

typedef struct zclCmdPrice_GetBlockPeriodsReq_tag
{
  ZCLTime_t StartTime;
  uint8_t NumOfEvts;
} zclCmdPrice_GetBlockPeriodsReq_t;

typedef struct  zclPrice_GetBlockPeriodsReq_tag
{  
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdPrice_GetBlockPeriodsReq_t cmdFrame;
} zclPrice_GetBlockPeriodsReq_t;

typedef struct zclCmdPrice_GetTariffInformationReq_tag
{
  ZCLTime_t StartTime;
  uint8_t NumOfEvts;
  uint8_t TariffType; /* O */
} zclCmdPrice_GetTariffInformationReq_t;

typedef struct  zclPrice_GetTariffInformationReq_tag
{  
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdPrice_GetTariffInformationReq_t cmdFrame;
} zclPrice_GetTariffInformationReq_t;

typedef struct zclCmdPrice_GetBillingPeriodReq_tag
{
  ZCLTime_t StartTime;
  uint8_t NumOfEvts;
  uint8_t TariffType; /* O */
} zclCmdPrice_GetBillingPeriodReq_t;

typedef struct  zclPrice_GetBillingPeriodReq_tag
{  
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdPrice_GetBillingPeriodReq_t cmdFrame;
} zclPrice_GetBillingPeriodReq_t;

typedef struct zclCmdPrice_CPPEventRsp_tag
{
  SEEvtId_t   IssuerEvtID;
  uint8_t     CPPAuth;
} zclCmdPrice_CPPEventRsp_t;

typedef struct  zclPrice_CPPEventRsp_tag
{  
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdPrice_CPPEventRsp_t cmdFrame;
} zclPrice_CPPEventRsp_t;

typedef struct zclCmdPrice_GetPriceMatrixReq_tag
{
  SEEvtId_t         IssuerEvtID;
} zclCmdPrice_GetPriceMatrixReq_t;

typedef struct  zclPrice_GetPriceMatrixReq_tag
{  
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdPrice_GetPriceMatrixReq_t cmdFrame;
} zclPrice_GetPriceMatrixReq_t;

typedef struct zclCmdPrice_GetBlockThresholdsReq_tag
{
  SEEvtId_t         IssuerEvtID;
} zclCmdPrice_GetBlockThresholdsReq_t;

typedef struct  zclPrice_GetBlockThresholdsReq_tag
{  
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdPrice_GetBlockThresholdsReq_t cmdFrame;
} zclPrice_GetBlockThresholdsReq_t;

typedef struct zclCmdPrice_GetCO2ValueReq_tag
{
  ZCLTime_t StartTime;
  uint8_t NumOfEvts;
  uint8_t TariffType; /* O */
} zclCmdPrice_GetCO2ValueReq_t;

typedef struct  zclPrice_GetCO2ValueReq_tag
{  
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdPrice_GetCO2ValueReq_t cmdFrame;
} zclPrice_GetCO2ValueReq_t;

typedef struct zclCmdPrice_GetConsolidatedBillReq_tag
{
  ZCLTime_t StartTime;
  uint8_t NumOfEvts;
  uint8_t TariffType; /* O */
} zclCmdPrice_GetConsolidatedBillReq_t;

typedef struct  zclPrice_GetConsolidatedBillReq_tag
{  
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdPrice_GetConsolidatedBillReq_t cmdFrame;
} zclPrice_GetConsolidatedBillReq_t;

typedef struct zclCmdPrice_GetCurrPriceReq_tag
{
  uint8_t CmdOptions;
} zclCmdPrice_GetCurrPriceReq_t;

typedef struct  zclPrice_GetCurrPriceReq_tag
{  
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdPrice_GetCurrPriceReq_t cmdFrame;
} zclPrice_GetCurrPriceReq_t;

typedef struct  zclPrice_InterPanGetCurrPriceReq_tag
{  
  InterPanAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdPrice_GetCurrPriceReq_t cmdFrame;
} zclPrice_InterPanGetCurrPriceReq_t;

typedef struct  zclCmdPrice_GetScheduledPricesReq_tag
{  
  ZCLTime_t StartTime;
  uint8_t NumOfEvts;
} zclCmdPrice_GetScheduledPricesReq_t;

typedef struct  zclPrice_GetScheduledPricesReq_tag
{  
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdPrice_GetScheduledPricesReq_t cmdFrame;
} zclPrice_GetScheduledPricesReq_t;

typedef struct  zclPrice_InterPanGetScheduledPricesReq_tag
{  
  InterPanAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdPrice_GetScheduledPricesReq_t cmdFrame;
} zclPrice_InterPanGetScheduledPricesReq_t;

typedef struct zclCmdPrice_PublishPriceRsp_tag
{
  ProviderID_t ProviderID;
  zclStr12_t   RateLabel;
  SEEvtId_t    IssuerEvt;
  ZCLTime_t    CurrTime;
  uint8_t      UnitOfMeasure;
  Currency_t   Currency;
  uint8_t      PriceTrailingDigitAndPriceTier;
  uint8_t      NumOfPriceTiersAndRgstrTier;
  ZCLTime_t    StartTime;         
  Duration_t   DurationInMinutes;
  Price_t      Price;
  uint8_t      PriceRatio;  /* O */
  Price_t      GenerationPrice;   /* O */
  uint8_t      GenerationPriceRatio;  /* O */
  Price_t      AltCostDlvrd;  /* O */
  uint8_t      AltCostUnit;  /* O */
  uint8_t      AltCostTrailingDigit;   /* O */
  uint8_t      NumberOfBlocksThresholds;   /* O */
  uint8_t      PriceControl;   /* O */
} zclCmdPrice_PublishPriceRsp_t;

typedef struct  zclPrice_PublishPriceRsp_tag
{  
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdPrice_PublishPriceRsp_t cmdFrame;
} zclPrice_PublishPriceRsp_t;

typedef struct zclCmdPrice_PublishBlockPeriodRsp_tag
{
  ProviderID_t          ProviderID;
  SEEvtId_t             IssuerEvtID;
  ZCLTime_t             BlockPeriodStartTime;
  BlockPeriodDuration_t BlockPeriodDurationInMinutes;
  uint8_t               NrOfPriceTiersNrOfBlockThresholds;
  uint8_t               BlockPeriodControl;
} zclCmdPrice_PublishBlockPeriodRsp_t;

typedef struct  zclPrice_PublishBlockPeriodRsp_tag
{  
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdPrice_PublishBlockPeriodRsp_t cmdFrame;
} zclPrice_PublishBlockPeriodRsp_t;

typedef struct zclCmdPrice_PublishPriceMatrixRsp_tag
{
  SEEvtId_t    IssuerEvtID;
  uint8_t      CommandIdx;
  Price_t      TierBlockPrice[1]; 
} zclCmdPrice_PublishPriceMatrixRsp_t;

typedef struct  zclPrice_PublishPriceMatrixRsp_tag
{  
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdPrice_PublishPriceMatrixRsp_t cmdFrame;
} zclPrice_PublishPriceMatrixRsp_t;

typedef struct zclCmdPrice_PublishBlockThresholdsRsp_tag
{
  SEEvtId_t         IssuerEvtID;
  uint8_t           CommandIdx;
  BlockThreshold_t  BlockThresholds[1]; 
} zclCmdPrice_PublishBlockThresholdsRsp_t;

typedef struct  zclPrice_PublishBlockThresholdsRsp_tag
{  
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdPrice_PublishBlockThresholdsRsp_t cmdFrame;
} zclPrice_PublishBlockThresholdsRsp_t;

typedef struct zclCmdPrice_PublishCalorificValueRsp_tag
{
  SEEvtId_t             IssuerEvtID;
  ZCLTime_t             StartTime;
  CalorificValue_t      CalorificValue;
  uint8_t               CalorificValueUnit;
  uint8_t               CalorificValueTrailingDigit;
} zclCmdPrice_PublishCalorificValueRsp_t;

typedef struct  zclPrice_PublishCalorificValueRsp_tag
{  
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdPrice_PublishCalorificValueRsp_t cmdFrame;
} zclPrice_PublishCalorificValueRsp_t;

typedef struct zclCmdPrice_PublishConversionFactorRsp_tag
{
  SEEvtId_t             IssuerEvt;
  ZCLTime_t             StartTime;
  ConversionFactor_t    ConversionFactor;
  uint8_t               ConversionFactorTrailingDigit;
} zclCmdPrice_PublishConversionFactorRsp_t;

typedef struct  zclPrice_PublishConversionFactorRsp_tag
{  
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdPrice_PublishConversionFactorRsp_t cmdFrame;
} zclPrice_PublishConversionFactorRsp_t;


typedef struct zclCmdPrice_PublishTariffInformationRsp_tag
{
  ProviderID_t                ProviderID;
  SEEvtId_t                   IssuerTariffID;
  ZCLTime_t                   StartTime;
  uint8_t                     TariffType;
  zclStr12_t                  TariffLabel;
  uint8_t                     NumberPriceTiersUse;
  uint8_t                     NumberBlockThreshholdsUse;
  uint8_t                     UnitOfMeasure;
  Currency_t                  Currency;
  uint8_t                     PriceTrailingDigit;
  StandingCharge_t            StandingCharge;
  uint8_t                     TierBlockMode;
  BlockThresholdMask_t        BlockThresholdMask;
  BlockThresholdMultiplier_t  BlockThresholdMultiplier;
  BlockThresholdDivisor_t     BlockThresholdDivisor;
} zclCmdPrice_PublishTariffInformationRsp_t;

typedef struct  zclPrice_PublishTariffInformationRsp_tag
{  
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdPrice_PublishTariffInformationRsp_t cmdFrame;
} zclPrice_PublishTariffInformationRsp_t;

typedef struct zclCmdPrice_PublishBillingPeriodRsp_tag
{
  SEEvtId_t         IssuerEvtID;
  ZCLTime_t         BillingPeriodStartTime;
  Duration24_t      BillingPeriodDurationInMinutes;
  uint8_t           TariffType;
} zclCmdPrice_PublishBillingPeriodRsp_t;

typedef struct  zclPrice_PublishBillingPeriodRsp_tag
{  
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdPrice_PublishBillingPeriodRsp_t cmdFrame;
} zclPrice_PublishBillingPeriodRsp_t;

typedef struct zclCmdPrice_PublishCPPEventRsp_tag
{
  SEEvtId_t         IssuerEvtID;
  ZCLTime_t         CPPEventStartTime;
  Duration16_t      CPPEventDurationInMinutes;
  uint8_t           TariffType;
  uint8_t           CPPPriceTier;
  uint8_t           CPPAuth;
} zclCmdPrice_PublishCPPEventRsp_t;

typedef struct  zclPrice_PublishCPPEventRsp_tag
{  
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdPrice_PublishCPPEventRsp_t cmdFrame;
} zclPrice_PublishCPPEventRsp_t;

typedef struct zclCmdPrice_PublishCO2ValueRsp_tag
{
  SEEvtId_t         IssuerEvtID;
  ZCLTime_t         StartTime;
  uint8_t           TariffType;
  CO2Value_t        CO2Value;
  uint8_t           CO2ValueUnit;
  uint8_t           CO2ValueTrailingDigit;
} zclCmdPrice_PublishCO2ValueRsp_t;

typedef struct  zclPrice_PublishCO2ValueRsp_tag
{  
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdPrice_PublishCO2ValueRsp_t cmdFrame;
} zclPrice_PublishCO2ValueRsp_t;

typedef struct zclCmdPrice_PublishConsolidatedBillRsp_tag
{
  SEEvtId_t         IssuerEvtID;
  ZCLTime_t         BillingPeriodStartTime;
  Duration24_t      BillingPeriodDurationInMinutes;
  uint8_t           TariffType;
  Bill_t            ConsolidatedBill;
  Currency_t        Currency;
  uint8_t           BillTrailingDigit;
} zclCmdPrice_PublishConsolidatedBillRsp_t;

typedef struct  zclPrice_PublishConsolidatedBillRsp_tag
{  
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdPrice_PublishConsolidatedBillRsp_t cmdFrame;
} zclPrice_PublishConsolidatedBillRsp_t;

typedef struct zclCmdPrice_PriceAck_tag
{
  ProviderID_t ProviderID;
  SEEvtId_t    IssuerEvt;
  ZCLTime_t    PriceAckTime;
  uint8_t      PriceControl;
}zclCmdPrice_PriceAck_t;

typedef struct zclPrice_PriceAck_tag
{
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdPrice_PriceAck_t cmdFrame;
}zclPrice_PriceAck_t;

typedef struct zclPrice_InterPriceAck_tag
{
  InterPanAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdPrice_PriceAck_t cmdFrame;
}zclPrice_InterPriceAck_t;

typedef struct  zclPrice_InterPanPublishPriceRsp_tag
{  
  InterPanAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdPrice_PublishPriceRsp_t cmdFrame;
} zclPrice_InterPanPublishPriceRsp_t;

typedef struct ztcCmdPrice_PublishBlockPeriodRsp_tag
{
  zclCmdPrice_PublishBlockPeriodRsp_t publishBlockPeriodRsp;
  bool_t SendPublishBlockPeriodImmediately;
} ztcCmdPrice_PublishBlockPeriodRsp_t;

typedef struct ztcCmdPrice_PublishBillingPeriodRsp_tag
{
  zclCmdPrice_PublishBillingPeriodRsp_t publishBillingPeriodRsp;
  bool_t SendPublishBillingPeriodImmediately;
} ztcCmdPrice_PublishBillingPeriodRsp_t;

typedef struct ztcCmdPrice_PublishTariffInformationRsp_tag
{
  zclCmdPrice_PublishTariffInformationRsp_t publishTariffInformationRsp;
  bool_t SendPublishImmediately;
} ztcCmdPrice_PublishTariffInformationRsp_t;

typedef struct ztcCmdPrice_PublishCO2ValueRsp_tag
{
  zclCmdPrice_PublishCO2ValueRsp_t publishCO2ValueRsp;
  bool_t SendPublishCO2ValueImmediately;
} ztcCmdPrice_PublishCO2ValueRsp_t;

typedef struct ztcCmdPrice_PublishConsolidatedBillRsp_tag
{
  zclCmdPrice_PublishConsolidatedBillRsp_t publishConsolidatedBillRsp;
  bool_t SendPublishConsolidatedBillImmediately;
} ztcCmdPrice_PublishConsolidatedBillRsp_t;

typedef struct ztcCmdPrice_PublishConversionFactorRsp_tag
{
  zclCmdPrice_PublishConversionFactorRsp_t publishConversionFactorRsp;
  bool_t SendPublishBlockPeriodImmediately;
} ztcCmdPrice_PublishConversionFactorRsp_t;

typedef struct ztcCmdPrice_PublishPriceMatrixRsp_tag
{
  SEEvtId_t   IssuerEvtID;
  uint8_t     Offset;
  uint8_t     Length;
  Price_t     TierBlockPrice[1]; 
} ztcCmdPrice_PublishPriceMatrixRsp_t;

typedef struct ztcCmdPrice_PublishBlockThresholdsRsp_tag
{
  SEEvtId_t         IssuerEvtID;
  uint8_t           Offset;
  uint8_t           Length;
  BlockThreshold_t  BlockThresholds[1]; 
} ztcCmdPrice_PublishBlockThresholdsRsp_t;

typedef struct ztcCmdPrice_PublishCalorificValueRsp_tag
{
  zclCmdPrice_PublishCalorificValueRsp_t publishCalorificValueRsp;
  bool_t SendPublishBlockPeriodImmediately;
} ztcCmdPrice_PublishCalorificValueRsp_t;

typedef struct ztcCmdPrice_PublishPriceRsp_tag
{
  zclCmdPrice_PublishPriceRsp_t publishPriceRsp;
  bool_t SendPublishBlockPeriodImmediately;
} ztcCmdPrice_PublishPriceRsp_t;

#define gPriceReceivedStatus_c 0x01
#define gPriceStartedStatus_c 0x02
#define gPriceUpdateStatus_c 0x03
#define gPriceCompletedStatus_c 0x04

//block period client status
#define gBlockPeriodReceivedStatus_c 0x01
#define gBlockPeriodStartedStatus_c 0x02
#define gBlockPeriodUpdateStatus_c 0x03
#define gBlockPeriodCompletedStatus_c 0x04

typedef struct publishPriceEntry_tag
{  
  zclCmdPrice_PublishPriceRsp_t Price;
  ZCLTime_t EffectiveStartTime;
  uint8_t EntryStatus;
} publishPriceEntry_t;

typedef struct publishBlockPeriodEntry_tag
{  
  zclCmdPrice_PublishBlockPeriodRsp_t blockPeriod;
  ZCLTime_t EffectiveStartTime;
  uint8_t EntryStatus;
} publishBlockPeriodEntry_t;

typedef struct publishPriceMatrixEntry_tag
{
  SEEvtId_t   IssuerEvtID;
  uint8_t     Length;
  uint8_t     EntryStatus;
  Price_t     TierBlockPrice[gASL_ZclPrice_BlockPriceInfoNumber_d]; 
} publishPriceMatrixEntry_t;

typedef struct publishBlockThresholdsEntry_tag
{
  SEEvtId_t         IssuerEvtID;
  uint8_t           Length;
  uint8_t           EntryStatus;
  BlockThreshold_t  BlockThresholds[gASL_ZclPrice_BlockThresholdNumber_d]; 
} publishBlockThresholdsEntry_t;

typedef struct publishConversionFactorEntry_tag
{  
  zclCmdPrice_PublishConversionFactorRsp_t conversionFactor;
  ZCLTime_t EffectiveStartTime;
  uint8_t EntryStatus;
} publishConversionFactorEntry_t;

typedef struct publishCalorificValueEntry_tag
{  
  zclCmdPrice_PublishCalorificValueRsp_t CalorificValue;
  ZCLTime_t EffectiveStartTime;
  uint8_t EntryStatus;
} publishCalorificValueEntry_t;

typedef struct publishTariffInformationEntry_tag
{  
  zclCmdPrice_PublishTariffInformationRsp_t TariffInformation;
  ZCLTime_t EffectiveStartTime;
  uint8_t EntryStatus;
} publishTariffInformationEntry_t;

typedef struct publishBillingPeriodEntry_tag
{  
  zclCmdPrice_PublishBillingPeriodRsp_t BillingPeriod;
  ZCLTime_t EffectiveStartTime;
  uint8_t EntryStatus;
} publishBillingPeriodEntry_t;

typedef struct publishCPPEventEntry_tag
{  
  zclCmdPrice_PublishCPPEventRsp_t CPPEvent;
  ZCLTime_t EffectiveStartTime;
  uint8_t EntryStatus;
} publishCPPEventEntry_t;

typedef struct publishCO2ValueEntry_tag
{  
  zclCmdPrice_PublishCO2ValueRsp_t CO2Value;
  ZCLTime_t EffectiveStartTime;
  uint8_t EntryStatus;
} publishCO2ValueEntry_t;

typedef struct publishConsolidatedBillEntry_tag
{  
  zclCmdPrice_PublishConsolidatedBillRsp_t ConsolidatedBill;
  ZCLTime_t EffectiveStartTime;
  uint8_t EntryStatus;
} publishConsolidatedBillEntry_t;

/* save all indexs from gaServerBlockPeriodsTable that need to be sent after a 
GetBlockPeriod request*/
typedef struct getBlockPeriodPEntryIndex_tag
{  
  uint8_t Index;
  addrInfoType_t addrInfo;
  uint8_t IndexVector[gNumofServerBlockPeriods_c];
} getBlockPeriodEntryIndex_t;

/* save all indexs from gaServerConversionFactorTable that need to be sent after a 
Get Conversion Factor request*/
typedef struct getConversionFactorEntryIndex_tag
{  
  uint8_t Index;
  addrInfoType_t addrInfo;
  uint8_t IndexVector[gNumofServerConversionFactors_c];
} getConversionFactorEntryIndex_t;

/* save all indexs from gaServerCalorificValueTable that need to be sent after a 
Get Calorific Value request*/
typedef struct getCalorificValueEntryIndex_tag
{  
  uint8_t Index;
  addrInfoType_t addrInfo;
  uint8_t IndexVector[gNumofServerCalorificValue_c];
} getCalorificValueEntryIndex_t;

/* save all indexs from gaTariffInformationTable that need to be sent after a 
GetTariffInformationTable request*/
typedef struct getTariffInformationEntryIndex_tag
{  
  addrInfoType_t addrInfo; 
  uint8_t Index;
  uint8_t numOfEvtsRemaining;
  ZCLTime_t startTime;
  uint8_t tariffType;
} getTariffInformationEntryIndex_t;

/* save next index from gaServerCO2ValueTable that need to be sent after a 
Get CO2 Value request and filter reqs*/
typedef struct getCO2ValueEntryIndex_tag
{  
  addrInfoType_t addrInfo;
  uint8_t Index;
  uint8_t numOfEvtsRemaining;
  ZCLTime_t startTime;
  uint8_t tariffType;
} getCO2ValueEntryIndex_t;

/* save next prices from gaServerPriceMatrixTable that need to be sent after a 
GetPriceMatrix request and filter reqs*/
typedef struct getPriceMatrixEntryIndex_tag
{  
  addrInfoType_t  addrInfo;
  uint8_t         Index;
  uint8_t         CmdIndex;
  uint8_t         Offset;
  uint8_t         Length;
} getPriceMatrixEntryIndex_t;

/* save next information from gaServerBlockThresholdsTable that need to be sent after a 
GetBlockThresholds request and filter reqs*/
typedef struct getBlockThresholdsEntryIndex_tag
{  
  addrInfoType_t  addrInfo;
  uint8_t         Index;
  uint8_t         CmdIndex;
  uint8_t         Offset;
  uint8_t         Length;
} getBlockThresholdsEntryIndex_t;

/* save next index from gaServerBillingPeriodTable that need to be sent after a 
Get Billing Period request and filter reqs*/
typedef struct getBillingPeriodEntryIndex_tag
{  
  addrInfoType_t addrInfo;
  uint8_t Index;
  uint8_t numOfEvtsRemaining;
  ZCLTime_t startTime;
  uint8_t tariffType;
} getBillingPeriodEntryIndex_t;

/* save next index from gaServerConsolidatedBillTable that need to be sent after a 
Get Consolidated Bill request and filter reqs*/
typedef struct getConsolidatedBillEntryIndex_tag
{  
  addrInfoType_t addrInfo;
  uint8_t Index;
  uint8_t numOfEvtsRemaining;
  ZCLTime_t startTime;
  uint8_t tariffType;
} getConsolidatedBillEntryIndex_t;

/**********************************************
Prepayment cluster definitions
***********************************************/

typedef uint8_t sePrepaymentTopUp_t[6];
typedef uint8_t sePrepaymentDebt_t[6];

/* Supply Status Attribute */
#define gSePrepaymentSupplyInterrupt_c   0x00
#define gSePrepaymentSupplyArmed_c       0x01
#define gSePrepaymentSupplyRestore_c      0x02

#define gSePrepaymentCtrlFlags_DisconnectionEnabled_c   0x01
#define gSePrepaymentCtrlFlags_CreditMgmtEnabled_c      0x04
#define gSePrepaymentCtrlFlags_CreditDisplayEnabled_c   0x10
#define gSePrepaymentCtrlFlags_AccountBase_c            0x40
#define gSePrepaymentCtrlFlags_ContractorFitter_c       0x80
#define gSePrepaymentCtrlFlags_Reserved_c               0x2A

#define gSePrepaymentCreditStatusFlags_CreditOK           0x01
#define gSePrepaymentCreditStatusFlags_LowCredit          0x02
#define gSePrepaymentCreditStatusFlags_EmergencyEnabled   0x04
#define gSePrepaymentCreditStatusFlags_EmergencyAvailable 0x08
#define gSePrepaymentCreditStatusFlags_EmergencySelected  0x10
#define gSePrepaymentCreditStatusFlags_EmergencyInUse     0x20
#define gSePrepaymentCreditStatusFlags_EmergencyExhausted 0x40

#if gBigEndian_c
	#define gZclAttrSmplPrepaymentPrepaymentInfoSet_c 		0x0000
	#define gZclAttrSmplPrepaymentTopUpSet_c 				0x0001
	#define gZclAttrSmplPrepaymentDebtSet_c 				0x0002
	#define gZclAttrSmplPrepaymentSuplySet_c 				0x0003
#else
	#define gZclAttrSmplPrepaymentPrepaymentInfoSet_c 		0x0000
	#define gZclAttrSmplPrepaymentTopUpSet_c 				0x0100
	#define gZclAttrSmplPrepaymentDebtSet_c 				0x0200
	#define gZclAttrSmplPrepaymentSuplySet_c 				0x0300
#endif


#if gBigEndian_c
	#define gZclAttrSmplPrepaymentPaymentControl_c (gZclAttrSmplPrepaymentPrepaymentInfoSet_c | 0x0000)
	#define gZclAttrSmplPrepaymentCreditRemaining_c (gZclAttrSmplPrepaymentPrepaymentInfoSet_c | 0x0100)
	#define gZclAttrSmplPrepaymentEnergyCreditRemaining_c (gZclAttrSmplPrepaymentPrepaymentInfoSet_c | 0x0200)
	#define gZclAttrSmplPrepaymentCreditStatus_c (gZclAttrSmplPrepaymentPrepaymentInfoSet_c | 0x0300)
#else
	#define gZclAttrSmplPrepaymentPaymentControl_c (gZclAttrSmplPrepaymentPrepaymentInfoSet_c | 0x0000)
	#define gZclAttrSmplPrepaymentCreditRemaining_c (gZclAttrSmplPrepaymentPrepaymentInfoSet_c | 0x0001)
	#define gZclAttrSmplPrepaymentEnergyCreditRemaining_c (gZclAttrSmplPrepaymentPrepaymentInfoSet_c | 0x0002)
	#define gZclAttrSmplPrepaymentCreditStatus_c (gZclAttrSmplPrepaymentPrepaymentInfoSet_c | 0x0003)
#endif
#if gBigEndian_c
	#define gZclAttrSmplPrepaymentTopUpDateTime1_c (gZclAttrSmplPrepaymentTopUpSet_c | 0x0000)
	#define gZclAttrSmplPrepaymentTopUpAmount1_c (gZclAttrSmplPrepaymentTopUpSet_c | 0x0100)
	#define gZclAttrSmplPrepaymentOriginatingDevice1_c (gZclAttrSmplPrepaymentTopUpSet_c | 0x0200)
	#define gZclAttrSmplPrepaymentTopUpDateTime2_c (gZclAttrSmplPrepaymentTopUpSet_c | 0x1000)
	#define gZclAttrSmplPrepaymentTopUpAmount2_c (gZclAttrSmplPrepaymentTopUpSet_c | 0x1100)
	#define gZclAttrSmplPrepaymentOriginatingDevice2_c (gZclAttrSmplPrepaymentTopUpSet_c | 0x1200)
	#define gZclAttrSmplPrepaymentTopUpDateTime3_c (gZclAttrSmplPrepaymentTopUpSet_c | 0x2000)
	#define gZclAttrSmplPrepaymentTopUpAmount3_c (gZclAttrSmplPrepaymentTopUpSet_c | 0x2100)
	#define gZclAttrSmplPrepaymentOriginatingDevice3_c (gZclAttrSmplPrepaymentTopUpSet_c | 0x2200)
	#define gZclAttrSmplPrepaymentTopUpDateTime4_c (gZclAttrSmplPrepaymentTopUpSet_c | 0x3000)
	#define gZclAttrSmplPrepaymentTopUpAmount4_c (gZclAttrSmplPrepaymentTopUpSet_c | 0x3100)
	#define gZclAttrSmplPrepaymentOriginatingDevice4_c (gZclAttrSmplPrepaymentTopUpSet_c | 0x3200)
	#define gZclAttrSmplPrepaymentTopUpDateTime5_c (gZclAttrSmplPrepaymentTopUpSet_c | 0x4000)
	#define gZclAttrSmplPrepaymentTopUpAmount5_c (gZclAttrSmplPrepaymentTopUpSet_c | 0x4100)
	#define gZclAttrSmplPrepaymentOriginatingDevice5_c (gZclAttrSmplPrepaymentTopUpSet_c | 0x4200)
#else
	#define gZclAttrSmplPrepaymentTopUpDateTime1_c (gZclAttrSmplPrepaymentTopUpSet_c | 0x0000)
	#define gZclAttrSmplPrepaymentTopUpAmount1_c (gZclAttrSmplPrepaymentTopUpSet_c | 0x0001)
	#define gZclAttrSmplPrepaymentOriginatingDevice1_c (gZclAttrSmplPrepaymentTopUpSet_c | 0x0002)
	#define gZclAttrSmplPrepaymentTopUpDateTime2_c (gZclAttrSmplPrepaymentTopUpSet_c | 0x0010)
	#define gZclAttrSmplPrepaymentTopUpAmount2_c (gZclAttrSmplPrepaymentTopUpSet_c | 0x0011)
	#define gZclAttrSmplPrepaymentOriginatingDevice2_c (gZclAttrSmplPrepaymentTopUpSet_c | 0x0012)
	#define gZclAttrSmplPrepaymentTopUpDateTime3_c (gZclAttrSmplPrepaymentTopUpSet_c | 0x0020)
	#define gZclAttrSmplPrepaymentTopUpAmount3_c (gZclAttrSmplPrepaymentTopUpSet_c | 0x0021)
	#define gZclAttrSmplPrepaymentOriginatingDevice3_c (gZclAttrSmplPrepaymentTopUpSet_c | 0x0022)
	#define gZclAttrSmplPrepaymentTopUpDateTime4_c (gZclAttrSmplPrepaymentTopUpSet_c | 0x0030)
	#define gZclAttrSmplPrepaymentTopUpAmount4_c (gZclAttrSmplPrepaymentTopUpSet_c | 0x0031)
	#define gZclAttrSmplPrepaymentOriginatingDevice4_c (gZclAttrSmplPrepaymentTopUpSet_c | 0x0032)
	#define gZclAttrSmplPrepaymentTopUpDateTime5_c (gZclAttrSmplPrepaymentTopUpSet_c | 0x0040)
	#define gZclAttrSmplPrepaymentTopUpAmount5_c (gZclAttrSmplPrepaymentTopUpSet_c | 0x0041)
	#define gZclAttrSmplPrepaymentOriginatingDevice5_c (gZclAttrSmplPrepaymentTopUpSet_c | 0x0042)
#endif
#if gBigEndian_c
	#define gZclAttrSmplPrepaymentFuelDebtRemaining_c (gZclAttrSmplPrepaymentDebtSet_c | 0x0000)
	#define gZclAttrSmplPrepaymentFuelDebtRecoveryRate_c (gZclAttrSmplPrepaymentDebtSet_c | 0x0100)
	#define gZclAttrSmplPrepaymentFuelDebtRecoveryPeryod_c (gZclAttrSmplPrepaymentDebtSet_c | 0x0200)
	#define gZclAttrSmplPrepaymentNonFuelDebtRemaining_c (gZclAttrSmplPrepaymentDebtSet_c | 0x0300)
	#define gZclAttrSmplPrepaymentNonFuelDebtRecoveryRate_c (gZclAttrSmplPrepaymentDebtSet_c | 0x0400)
	#define gZclAttrSmplPrepaymentNonFuelDebtRecoveryPeriod_c (gZclAttrSmplPrepaymentDebtSet_c | 0x0500)
#else
	#define gZclAttrSmplPrepaymentFuelDebtRemaining_c (gZclAttrSmplPrepaymentDebtSet_c | 0x0000)
	#define gZclAttrSmplPrepaymentFuelDebtRecoveryRate_c (gZclAttrSmplPrepaymentDebtSet_c | 0x0001)
	#define gZclAttrSmplPrepaymentFuelDebtRecoveryPeryod_c (gZclAttrSmplPrepaymentDebtSet_c | 0x0002)
	#define gZclAttrSmplPrepaymentNonFuelDebtRemaining_c (gZclAttrSmplPrepaymentDebtSet_c | 0x0003)
	#define gZclAttrSmplPrepaymentNonFuelDebtRecoveryRate_c (gZclAttrSmplPrepaymentDebtSet_c | 0x0004)
	#define gZclAttrSmplPrepaymentNonFuelDebtRecoveryPeriod_c (gZclAttrSmplPrepaymentDebtSet_c | 0x0005)
#endif
#if gBigEndian_c
	#define gZclAttrSmplPrepaymentProposedChangeProviderID_c (gZclAttrSmplPrepaymentSuplySet_c | 0x0000)
	#define gZclAttrSmplPrepaymentProposedChangeImplTime_c (gZclAttrSmplPrepaymentSuplySet_c | 0x0100)
	#define gZclAttrSmplPrepaymentProposedChangeSupplyStatus_c (gZclAttrSmplPrepaymentSuplySet_c | 0x0200)
	#define gZclAttrSmplPrepaymentDelayedSuplyIntValueRem_c (gZclAttrSmplPrepaymentSuplySet_c | 0x0300)
	#define gZclAttrSmplPrepaymentDelayedSupplyIntValType_c (gZclAttrSmplPrepaymentSuplySet_c | 0x0400)
#else
	#define gZclAttrSmplPrepaymentProposedChangeProviderID_c (gZclAttrSmplPrepaymentSuplySet_c | 0x0000)
	#define gZclAttrSmplPrepaymentProposedChangeImplTime_c (gZclAttrSmplPrepaymentSuplySet_c | 0x0001)
	#define gZclAttrSmplPrepaymentProposedChangeSupplyStatus_c (gZclAttrSmplPrepaymentSuplySet_c | 0x0002)
	#define gZclAttrSmplPrepaymentDelayedSuplyIntValueRem_c (gZclAttrSmplPrepaymentSuplySet_c | 0x0003)
	#define gZclAttrSmplPrepaymentDelayedSupplyIntValType_c (gZclAttrSmplPrepaymentSuplySet_c | 0x0004)
#endif


/*----------Server commands---------*/
#define gZclCmdPrepayment_Server_SupplyStatRsp_c              0x00 /* O */

/*----------Client commands---------*/
#define gZclCmdPrepayment_Client_SelAvailEmergCreditReq_c       0x00 /* O */
#define gZclCmdPrepayment_Client_ChangeSupplyReq_c              0x01 /* O */

typedef struct zclCmdPrepayment_SupplyStatRsp_tag
{
  ProviderID_t    providerId;
  ZCLTime_t       implementationDateTime;
  uint8_t         supplyStatus;
}zclCmdPrepayment_SupplyStatRsp_t;

typedef struct zclPrepayment_SupplyStatRsp_tag
{
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdPrepayment_SupplyStatRsp_t cmdFrame;
}zclPrepayment_SupplyStatRsp_t;

typedef struct zclCmdPrepayment_SelAvailEmergCreditReq_tag
{
  ZCLTime_t       cmdDateTime;
  uint8_t         originatingDevice;
  zclStr32Oct_t   siteId;
  zclStr16Oct_t   meterSerialNumber;
}zclCmdPrepayment_SelAvailEmergCreditReq_t;

typedef struct zclPrepayment_SelAvailEmergCreditReq_tag
{
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdPrepayment_SelAvailEmergCreditReq_t cmdFrame;
}zclPrepayment_SelAvailEmergCreditReq_t;

typedef struct zclCmdPrepayment_ChangeSupplyReq_tag
{
  ProviderID_t    providerId;
  ZCLTime_t       reqDateTime;
  zclStr32Oct_t   siteId;
  zclStr16Oct_t   meterSerialNumber;
  ZCLTime_t       implementationDateTime;
  uint8_t         proposedSupplyStatus;
  uint8_t         originatorId;
}zclCmdPrepayment_ChangeSupplyReq_t;

typedef struct zclPrepayment_ChangeSupplyReq_tag
{
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdPrepayment_ChangeSupplyReq_t cmdFrame;
}zclPrepayment_ChangeSupplyReq_t;

/**********************************************
Key Establishment cluster definitions
***********************************************/
#define gZclAttrKeyEstabInfoSet_c    0x0000

/* Value of the KeyEstabSuite Attribute */

#if gBigEndian_c
#define gKeyEstabSuite_CBKE_ECMQV_c 0x0100      /* 0x0001, little endian order */
#else
#define gKeyEstabSuite_CBKE_ECMQV_c 0x0001
#endif


/* information set */
#define gZclAttrKeyEstabSecSuite_c                   (gZclAttrKeyEstabInfoSet_c | 0x0000) /*M*/


typedef struct IdentityCert_tag 
{
  uint8_t PublicReconstrKey[22];
  zbIeeeAddr_t Subject;
  uint8_t Issuer[8];
  uint8_t ProfileAttributeData[10];
} IdentifyCert_t;

#define gZclCmdKeyEstab_CompressedPubKeySize_c    22
#define gZclCmdKeyEstab_UncompressedPubKeySize_c  43
#define gZclCmdKeyEstab_PrivateKeySize_c          21
#define gZclCmdKeyEstab_CertSize_c                sizeof(IdentifyCert_t)
#define gZclCmdKeyEstab_SharedSecretSize_c        21
#define gZclCmdKeyEstab_PointOrderSize_c          21
#define gZclCmdKeyEstab_AesMMOHashSize_c          16 


/*command Ids for client commands (send)*/
#define gZclCmdKeyEstab_InitKeyEstabReq_c         0x00
#define gZclCmdKeyEstab_EphemeralDataReq_c        0x01
#define gZclCmdKeyEstab_ConfirmKeyDataReq_c       0x02
#define gZclCmdKeyEstab_TerminateKeyEstabServer_c 0x03

/*command Ids for server commands (send)*/
#define gZclCmdKeyEstab_InitKeyEstabRsp_c         0x00
#define gZclCmdKeyEstab_EphemeralDataRsp_c        0x01
#define gZclCmdKeyEstab_ConfirmKeyDataRsp_c       0x02
#define gZclCmdKeyEstab_TerminateKeyEstabClient_c 0x03

/*status values for the Terminate Key Establishment command (client and server)*/
#define gZclCmdKeyEstab_TermUnknownIssuer_c     0x01
#define gZclCmdKeyEstab_TermBadKeyConfirm_c     0x02
#define gZclCmdKeyEstab_TermBadMessage_c        0x03
#define gZclCmdKeyEstab_TermNoResources_c       0x04
#define gZclCmdKeyEstab_TermUnsupportedSuite_c  0x05

/*status values for the Initiate Key Establishment response*/
#define gZclCmdKeyEstab_InitSuccess_c           0x00
#define gZclCmdKeyEstab_InitBadCertificate_c    0x01
#define gZclCmdKeyEstab_InitBadMessage_c        0x02
#define gZclCmdKeyEstab_Timeout_c               0x03



/*Client commands*/
typedef struct ZclCmdKeyEstab_InitKeyEstabReq_tag {
  SecuritySuite_t SecuritySuite;
  uint8_t EphemeralDataGenerateTime;
  uint8_t ConfirmKeyGenerateTime;
  uint8_t IdentityIDU[gZclCmdKeyEstab_CertSize_c];  
} ZclCmdKeyEstab_InitKeyEstabReq_t;

typedef struct ZclCmdKeyEstab_EphemeralDataReq_tag {
  uint8_t EphemeralDataQEU[gZclCmdKeyEstab_CompressedPubKeySize_c];
} ZclCmdKeyEstab_EphemeralDataReq_t;


typedef struct ZclCmdKeyEstab_ConfirmKeyDataReq_tag {
  uint8_t SecureMsgAuthCodeMACU[gZclCmdKeyEstab_AesMMOHashSize_c];
} ZclCmdKeyEstab_ConfirmKeyDataReq_t;

typedef struct ZclCmdKeyEstab_TerminateKeyEstabServer_tag {
  uint8_t StatusCode;
  uint8_t WaitCode;
  SecuritySuite_t SecuritySuite;  
} ZclCmdKeyEstab_TerminateKeyEstabServer_t;


/*Server commmands*/
typedef struct ZclCmdKeyEstab_InitKeyEstabRsp_tag {
  SecuritySuite_t SecuritySuite;
  uint8_t EphemeralDataGenerateTime;
  uint8_t ConfirmKeyGenerateTime;
  uint8_t IdentityIDV[gZclCmdKeyEstab_CertSize_c];  
} ZclCmdKeyEstab_InitKeyEstabRsp_t;

typedef struct ZclCmdKeyEstab_EphemeralDataRsp_tag {
  uint8_t EphemeralDataQEV[gZclCmdKeyEstab_CompressedPubKeySize_c];
} ZclCmdKeyEstab_EphemeralDataRsp_t;

typedef struct ZclCmdKeyEstab_ConfirmKeyDataRsp_tag {
  uint8_t SecureMsgAuthCodeMACV[gZclCmdKeyEstab_AesMMOHashSize_c];
} ZclCmdKeyEstab_ConfirmKeyDataRsp_t;

typedef struct ZclCmdKeyEstab_TerminateKeyEstabClient_tag {
  uint8_t StatusCode;
  uint8_t WaitCode;
  SecuritySuite_t SecuritySuite;  
} ZclCmdKeyEstab_TerminateKeyEstabClient_t;

/*--------------------------------------*/

typedef struct  ZclKeyEstab_InitKeyEstabReq_tag 
{  
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  ZclCmdKeyEstab_InitKeyEstabReq_t cmdFrame;
} ZclKeyEstab_InitKeyEstabReq_t;

typedef struct  ZclKeyEstab_EphemeralDataReq_tag 
{  
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  ZclCmdKeyEstab_EphemeralDataReq_t cmdFrame;
} ZclKeyEstab_EphemeralDataReq_t;

typedef struct  ZclKeyEstab_ConfirmKeyDataReq_tag 
{  
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  ZclCmdKeyEstab_ConfirmKeyDataReq_t cmdFrame;
} ZclKeyEstab_ConfirmKeyDataReq_t;

typedef struct  ZclKeyEstab_TerminateKeyEstabServer_tag 
{  
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  ZclCmdKeyEstab_TerminateKeyEstabServer_t cmdFrame;
} ZclKeyEstab_TerminateKeyEstabServer_t;

typedef struct  ZclKeyEstab_InitKeyEstabRsp_tag 
{  
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  ZclCmdKeyEstab_InitKeyEstabRsp_t cmdFrame;
} ZclKeyEstab_InitKeyEstabRsp_t;

typedef struct  ZclKeyEstab_EphemeralDataRsp_tag 
{  
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  ZclCmdKeyEstab_EphemeralDataRsp_t cmdFrame;
} ZclKeyEstab_EphemeralDataRsp_t;

typedef struct  ZclKeyEstab_ConfirmKeyDataRsp_tag 
{  
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  ZclCmdKeyEstab_ConfirmKeyDataRsp_t cmdFrame;
} ZclKeyEstab_ConfirmKeyDataRsp_t;


typedef struct  ZclKeyEstab_TerminateKeyEstabClient_tag 
{  
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  ZclCmdKeyEstab_TerminateKeyEstabClient_t cmdFrame;
} ZclKeyEstab_TerminateKeyEstabClient_t;

typedef struct ZclKeyEstab_SetSecurityMaterial_tag
{
  IdentifyCert_t deviceImplicitCert;
  uint8_t devicePrivateKey[gZclCmdKeyEstab_PrivateKeySize_c];
  uint8_t devicePublicKey[gZclCmdKeyEstab_CompressedPubKeySize_c];
}ZclKeyEstab_SetSecurityMaterial_t;

typedef struct ZclKeyEstab_InitiateKeyEstab_tag
{
  zbEndPoint_t dstEndpoint;
  zbEndPoint_t srcEndpoint;
  zbNwkAddr_t dstAddr;
}ZclKeyEstab_InitiateKeyEstab_t;

/**********************************************
Messaging cluster definitions
***********************************************/

#define gZclCmdMsg_DisplayMsgReq_c              0x00 /* M */
#define gZclCmdMsg_CancelMsgReq_c               0x01 /* M */

typedef struct zclCmdMsg_DisplayMsgReq_tag
{
  MsgId_t       MsgID;
  uint8_t       MsgCtrl;
  ZCLTime_t     StartTime;
  Duration_t    DurationInMinutes;
  //zclStr32_t    Msg;
  uint8_t     length;
  uint8_t     msgData[1];
} zclCmdMsg_DisplayMsgReq_t;

typedef struct zclMsgLastMessage_tag
{
  uint8_t       Status;
  MsgId_t       MsgID;
  uint8_t       MsgCtrl;
  ZCLTime_t     StartTime;
  Duration_t    DurationInMinutes;
  ZCLTime_t     EffectiveStartTime;
  /* Message is limited to a message of maximum 32 characters*/
  zclStr32_t    Msg;
} zclMsgLastMessage_t;
typedef struct  zclDisplayMsgReq_tag 
{  
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdMsg_DisplayMsgReq_t cmdFrame;
} zclDisplayMsgReq_t;

typedef struct  zclInterPanDisplayMsgReq_tag 
{  
  InterPanAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdMsg_DisplayMsgReq_t cmdFrame;
} zclInterPanDisplayMsgReq_t;

#define gZclSEMsgCtrl_SendMsgThroughNormalCmdFunctionToClient_c   0x00 
#define gZclSEMsgCtrl_AdditionallyPassMsgOntoTheAnonDataCluster_c 0x01
#define gZclSEMsgCtrl_OnlySendMsgThroughTheAnonDataCluster_c      0x02

#define gZclSEMsgCtrl_MsgUrgency_Low_c       0x00 
#define gZclSEMsgCtrl_MsgUrgency_Medium_c    0x04 
#define gZclSEMsgCtrl_MsgUrgency_High_c      0x08 
#define gZclSEMsgCtrl_MsgUrgency_Crit_c      0x0C 

#define gZclSEMsgCtrl_MsgConfNotRequired_c      0x00 
#define gZclSEMsgCtrl_MsgConfRequired_c         0x80 



typedef struct zclCmdMsg_CancelMsgReq_tag
{
  MsgId_t       MsgID;
  uint8_t       MsgCtrl;
} zclCmdMsg_CancelMsgReq_t;

typedef struct  zclCancelMsgReq_tag 
{  
  afAddrInfo_t addrInfo; 
  uint8_t zclTransactionId;
  zclCmdMsg_CancelMsgReq_t cmdFrame;
} zclCancelMsgReq_t;

typedef struct  zclInterPanCancelMsgReq_tag 
{  
  InterPanAddrInfo_t addrInfo; 
  uint8_t zclTransactionId;
  zclCmdMsg_CancelMsgReq_t cmdFrame;
} zclInterPanCancelMsgReq_t;

#define gZclCmdMsg_GetLastMsgReq_c              0x00 /* M */
#define gZclCmdMsg_MsgConfReq_c         0x01 /* M */

typedef struct zclCmdMsg_MsgConfReq_tag
{
  MsgId_t   MsgID;
  ZCLTime_t ConfTime;  
} zclCmdMsg_MsgConfReq_t;

typedef struct zclGetLastMsgReq_tag
{
  afAddrInfo_t         addrInfo;  /* IN: source/dst address, cluster, etc... */
  uint8_t zclTransactionId;
} zclGetLastMsgReq_t;

typedef struct zclInterPanGetLastMsgReq_tag
{
  InterPanAddrInfo_t         addrInfo;  /* IN: source/dst address, cluster, etc... */
  uint8_t zclTransactionId;
} zclInterPanGetLastMsgReq_t;

typedef struct  zclMsgConfReq_tag 
{  
  afAddrInfo_t addrInfo; 
  uint8_t zclTransactionId;
  zclCmdMsg_MsgConfReq_t cmdFrame;
} zclMsgConfReq_t;

typedef struct  zclInterPanMsgConfReq_tag 
{  
  InterPanAddrInfo_t addrInfo; 
  uint8_t zclTransactionId;
  zclCmdMsg_MsgConfReq_t cmdFrame;
} zclInterPanMsgConfReq_t;


/******************************************************************************
*******************************************************************************
* Internal Attributes types def
*******************************************************************************
******************************************************************************/
typedef struct zclAttrKeyEstabServerAttrsRAM_tag
{
  SecuritySuite_t SecuritySuite;
}zclAttrKeyEstabServerAttrsRAM_t;


typedef struct ZCLDRLCClientServerAttrsRAM_tag {
  uint8_t UtilityGroup;
  uint8_t StartRandomizeMin;
  uint8_t StopRandomizeMin;  
  LCDRDevCls_t DevCls;
} ZCLDRLCClientServerAttrsRAM_t;

typedef struct ZCLSmplMetServerAttrsRAM_tag
{
   /*Attributes of the Reading information attribute set */
  Summ_t   RISCurrSummDlvrd[zclReportableCopies_c];
#if gASL_ZclMet_Optionals_d
  Summ_t    RISCurrSummRcvd; 
  Summ_t    RISCurrMaxDmndDlvrd;
  Summ_t    RISCurrMaxDmndRcvd;
  Summ_t    RISDFTSumm;
  uint16_t    RISDailyFreezeTime;
  int8_t    RISPowerFactor;
  ZCLTime_t RISReadingSnapShotTime;
  ZCLTime_t RISCurrMaxDmndDlvrdTimer;
  ZCLTime_t RISCurrMaxDmndRcvdTime;
  uint8_t   RISDefaultUpdatePeriod;
  uint8_t   RISFastPollUpdatePeriod;
  Summ_t    RISCurrBlockPeriodConsumptionDelivered;
  Consmp    RISDailyConsumptionTarget;
  uint8_t   RISCurrBlock;
  uint8_t   RISProfileIntPeriod;
  uint16_t  RISIntReadReportingPeriod;
  uint16_t  RISPresetReadingTime;
  uint16_t  RISVolumePerReport;
  uint8_t   RISFlowRestrictio;
  uint8_t   RISSupplyStatus;
  Summ_t    RISCurrInletEnergyCarrierSummation;
  Summ_t    RISCurrOutletEnergyCarrierSummation;
  int16_t   RISInletTemp;
  int16_t   RISOutletTemp;
  int16_t   RISCtrlTemp;
  Consmp    RISCurrInletEnergyCarrierDemand;
  Consmp    RISCurrOutletEnergyCarrierDemand;

  /*Attributes of the TOU Information attribute set */
#if gASL_ZclSE_TiersNumber_d >=1  
  Summ_t  TOUCurrTierSumm[2 * gASL_ZclSE_TiersNumber_d];
#endif
  
#endif
  
  /*Attributes of the Meter Status attribute set */
  uint8_t MSStatus[zclReportableCopies_c];
#if gASL_ZclMet_Optionals_d
  uint8_t RemainingBatteryLife;
  Consmp  HoursInOperation;
  Consmp  HoursInFault;
#endif
  
  /*Attributes of the Formatting attribute set */
  uint8_t   SmplMetCFSUnitofMeasure[zclReportableCopies_c];
#if gASL_ZclMet_Optionals_d 
  Consmp  Mult;
  Consmp  Div;
#endif  
  uint8_t   CFSSummFormat[zclReportableCopies_c];
#if gASL_ZclMet_Optionals_d 
  uint8_t   DmndFormat;
  uint8_t   HistoricalConsmpFormat;
#endif  
  uint8_t   CFSMetDevType[zclReportableCopies_c];
#if gASL_ZclMet_Optionals_d
  zclStr32Oct_t CFSSiteID;
  zclStr24Oct_t CFSMeterSerialNumber;
  uint8_t CFSEnergyCarrierUnitOfMeasure;
  uint8_t CFSEnergyCarrierSummationFormatting;
  uint8_t CFSEnergyCarrierDemandFormatting;
  uint8_t CFSTempUnitOfMeasure;
  uint8_t CFSTempFormatting;
#endif

/*Attributes of the ESP Historical Consumption */	
#if gASL_ZclMet_Optionals_d
  Consmp  InstantaneousDmnd;
  Consmp  CurrDayConsmpDlvrd;
  Consmp  CurrDayConsmpRcvd;
  Consmp  PreviousDayConsmpDlvrd;
  Consmp  PreviousDayConsmpRcvd;
  ZCLTime_t CurrPartialProfileIntrvStartTimeDlvrd;
  ZCLTime_t CurrPartialProfileIntrvStartTimeRcvd;
  IntrvForProfiling_t  CurrPartialProfileIntrvValueDlvrd;
  IntrvForProfiling_t  CurrPartialProfileIntrvValueRcvd;
  Pressure_t CurrDayMaxPressure;
  Pressure_t CurrDayMinPressure;
  Pressure_t PrevDayMaxPressure;
  Pressure_t PrevDayMinPressure;
  Demmand_t CurrDayMaxDemand;
  Demmand_t PrevDayMaxDemand;
  Demmand_t CurrMonthMaxDemand;
  Demmand_t CurrYearMaxDemand;
  Demmand_t CurrDayMaxEnergyCarrierDemand;
  Demmand_t PrevDayMaxEnergyCarrierDemand;
  Demmand_t CurrMonthMaxEnergyCarrierDemand;
  Demmand_t CurrMonthMinEnergyCarrierDemand;
  Demmand_t CurrYearMaxEnergyCarrierDemand;
  Demmand_t CurrYearMinEnergyCarrierDemand;

/*Attributes of the Simple Metering Meter Load Profile Configuration attribute set */  
  uint8_t MaxNumberOfPeriodsDlvrd;
  
/*Attributes of the Simple Metering Alarm attribute set */  
  uint16_t ASGenericAlarmMask;
  uint16_t ASElectricityAlarmMask;
  uint16_t ASGenericFlowPressureAlarmMask;
  uint16_t ASWaterSpecificAlarmMask;
  uint16_t ASHeatAndCoolingSpecificAlarmMask;
  uint16_t ASGasSpecificAlarmMask;
  
/*Attributes of the Simple Metering Supply Limit Attribute Set */ 
  Consmp  CurrDmndDlvrd;
  Consmp  DmndLimit;
  uint8_t DmndIntegrationPeriod;
  uint8_t NumOfDmndSubIntrvs;

/*Attributes of the Simple Metering Block Information Delivered Attribute Set */
  
#if gASL_ZclMet_BISSetNumberOfTiersDlvr_d >= 1 && gASL_ZclMet_BISSetNumberOfBlocksDlvr_d >= 1
  Summ_t    CurrNoTierBlockSummDlvrd[gASL_ZclMet_BISSetNumberOfTiersDlvr_d][gASL_ZclMet_BISSetNumberOfBlocksDlvr_d];
#endif

/*Attributes of the Simple Metering Block Information Delivered Attribute Set */
#if gASL_ZclMet_BISSetNumberOfTiersRcvd_d >= 1 && gASL_ZclMet_BISSetNumberOfBlocksRcvd_d >=1
  Summ_t    CurrNoTierBlockSummRcvd[gASL_ZclMet_BISSetNumberOfTiersRcvd_d][gASL_ZclMet_BISSetNumberOfBlocksRcvd_d];
#endif
  
/*Attributes of the Simple Metering Billing Attribute Set */
  Bill_t    BillToDate;
  uint32_t  BillToDateTimeStamp;
  Bill_t    ProjectedBill;
  uint32_t  ProjectedBillTimeStamp;
#endif
} ZCLSmplMetServerAttrsRAM_t;

typedef struct sePriceClientAttrRAM_tag{
  uint8_t PriceIncreaseRandomizeMinutes;
  uint8_t PriceDecreaseRandomizeMinutes;
  uint8_t CommodityType;
}sePriceClientAttrRAM_t;

#if gASL_ZclPrice_Optionals_d
typedef struct sePriceAttrRAM_tag {
  #if gASL_ZclSE_TiersNumber_d > 0
		zclStr12_t TierPriceLabel[gASL_ZclSE_TiersNumber_d];
	#endif
                
	#if gASL_ZclPrice_BlockThresholdNumber_d > 0
		BlockThreshold_t BlockThresholdPrice[gASL_ZclPrice_BlockThresholdNumber_d];
	#endif
                
	#if gASL_ZclPrice_BlockPeriodNumber_d > 0
		BlockPeriodPrice_t BlockPeriodPrice;
	#endif
                
        #if  gASL_ZclPrice_CommodityTypeNumber_d>0
		CommodityTypePrice_t CommodityTypePrice;
        #endif        
	#if gASL_ZclPrice_BlockPriceInfoNumber_d > 0
		uint32_t BlockPriceInfoPrice[gASL_ZclPrice_BlockPriceInfoNumber_d];
	#endif
} sePriceAttrRAM_t;
#endif

#if gASL_ZclSETunneling_d
typedef struct seTunnelingAttrRAM_tag {
  uint16_t            closeTunnelTimeout;
} seTunnelingAttrRAM_t;
#endif

typedef struct sePrepaymentAttrRAM_tag{
  uint8_t             paymentCtrl;
#if gASL_ZclPrepayment_Optionals_d
  uint32_t            creditRemaining;
  uint32_t            emergencyCreditRemaining;  
  uint8_t             creditStatus;
#if gASL_ZclPrepayment_TopUpOptionals_d
  ZCLTime_t           topUpTime1;
  sePrepaymentTopUp_t topUpAmount1;
  uint8_t             topUpOriginatingDevice1;
  ZCLTime_t           topUpTime2;
  sePrepaymentTopUp_t topUpAmount2;
  uint8_t             topUpOriginatingDevice2;
  ZCLTime_t           topUpTime3;
  sePrepaymentTopUp_t topUpAmount3;
  uint8_t             topUpOriginatingDevice3;
  ZCLTime_t           topUpTime4;
  sePrepaymentTopUp_t topUpAmount4;
  uint8_t             topUpOriginatingDevice4;
  ZCLTime_t           topUpTime5;
  sePrepaymentTopUp_t topUpAmount5;
  uint8_t             topUpOriginatingDevice5;
#endif  /* gASL_ZclPrepayment_TopUpOptionals_d */
#if gASL_ZclPrepayment_DebtOptionals_d
  sePrepaymentDebt_t   fuelDebtRemaining;
  uint32_t             fuelDebtRecoveryRate;
  uint8_t              fuelDebtRecoveryPeriod;
  sePrepaymentDebt_t   nonFuelDebtRemaining;
  uint32_t             nonFuelDebtRecoveryRate;
  uint8_t              nonFuelDebtRecoveryPeriod;
#endif  /* gASL_ZclPrepayment_DebtOptionals_d */
#if gASL_ZclPrepayment_SupplyOptionals_d
  uint32_t             proposedChangeProviderId;
  ZCLTime_t            proposedChangeImplementationTime;
  uint8_t              proposedChangeSupplyStatus;
  uint16_t             delayedSupplyIntValueRemaining;
  uint8_t              delayedSupplyIntValueType;
#endif  /* gASL_ZclPrepayment_SupplyOptionals_d */
#endif  /* gASL_ZclPrepayment_Optionals_d */
} sePrepaymentAttrRAM_t;

typedef struct seMeterAttrRAM_tag {
  uint8_t              reportMask[1];  /* allows up to 8 reportable endpoints in this node  */
  uint16_t             identifyTime;
  ZCLSmplMetServerAttrsRAM_t  SmplMet;
}seSmplMetAttrRAM_t;

typedef struct seESIMirrorBasicData_t {
  uint8_t    ZCLVersion;           /* 0x0000 ZCLVersion - 0x01 - RdOnly - M */
#if gZclClusterOptionals_d
  uint8_t    ApplicationVersion;   /* 0x0001 ApplicationVersion - 0x00 - RdOnly - O */
  uint8_t    HWVersion;            /* 0x0003 HWVersion - 0x00 - RdOnly - O */
  zclStr32_t ManufacturerName;   /* 0x0004 ManufacturerName - "Freescale", 32 - RdOnly - O */
  zclStr32_t ModelIdentifier;    /* 0x0005 ModelIdentifier - "On/Off Light", 32 - RdOnly - O */
#endif  
  uint8_t    iPowerSource;       /* 0x0007 PowerSource - 0x01(mains) - M */  
} seESIMirrorBasicData_t;

typedef struct seESIMirrorMeteringData_t {
  Summ_t     RISCurrSummDlvrd;
#if gZclClusterOptionals_d  
  Summ_t    RISDFTSumm;
  uint16_t    RISDailyFreezeTime;
  ZCLTime_t RISReadingSnapShotTime;
#endif  
  uint8_t MSStatus;
  uint8_t   SmplMetCFSUnitofMeasure;
#if gZclClusterOptionals_d    
  Consmp  Mult;
  Consmp  Div;
#endif  
  uint8_t   CFSSummFormat;
  uint8_t   CFSMetDevType;  
#if gZclClusterOptionals_d    
  uint8_t MaxNumberOfPeriodsDlvrd;
#endif  
} seESIMirrorMeteringData_t;

typedef struct seESIMirrorData_t {
  seESIMirrorBasicData_t BasicData;
  seESIMirrorMeteringData_t MeteringData;
} seESIMirrorData_t;

typedef struct seESPData_tag {
#if gZclEnableMeteringServer_d   
  seSmplMetAttrRAM_t MeterAttr;
#endif
#if gASL_ZclPrice_Optionals_d
  sePriceAttrRAM_t PriceAttr;
#endif
#if gASL_ZclSETunneling_d
  seTunnelingAttrRAM_t SETunnelingAttr;
#endif
#if gASL_ZclPrepayment_d
  sePrepaymentAttrRAM_t SEPrepaymentAttr;
#endif
} seESPData_t;

typedef struct seLCDData_tag{
  ZCLDRLCClientServerAttrsRAM_t DRLCAttr;
#if gZclClusterOptionals_d  
  sePriceClientAttrRAM_t        PriceAttr;
#endif  
#if gASL_ZclSETunneling_d
  seTunnelingAttrRAM_t SETunnelingAttr;
#endif
}seLCDData_t;

#if gZclClusterOptionals_d  
typedef struct seIPDData_tag{

  ZCLDRLCClientServerAttrsRAM_t DRLCAttr;
  sePriceClientAttrRAM_t        PriceAttr;
#if gASL_ZclSETunneling_d
  seTunnelingAttrRAM_t SETunnelingAttr;
#endif
}seIPDData_t;
#endif
typedef struct seMTRData_tag {
  seSmplMetAttrRAM_t MeterAttr;
#if gZclClusterOptionals_d
  sePriceClientAttrRAM_t PriceAttr;
#endif    
#if gASL_ZclSETunneling_d
  seTunnelingAttrRAM_t TunnelingAttr;
#endif
#if gASL_ZclPrepayment_d
  sePrepaymentAttrRAM_t PrepaymentAttr;
#endif
#if gZclClusterOptionals_d
  zclAlarmsAttrsRAM_t alarmsAttr;
#endif
} seMTRData_t;
typedef struct sePCTData_tag{
  ZCLDRLCClientServerAttrsRAM_t DRLCAttr;
#if gZclClusterOptionals_d  
  sePriceClientAttrRAM_t        PriceAttr;
#endif  
#if gASL_ZclSETunneling_d
  seTunnelingAttrRAM_t SETunnelingAttr;
#endif
}sePCTData_t;

typedef struct sePrepayTermData_tag{
#if gZclClusterOptionals_d
  ZCLDRLCClientServerAttrsRAM_t DRLCAttr;
#endif   
  sePriceClientAttrRAM_t        PriceAttr;
#if gASL_ZclSETunneling_d
  seTunnelingAttrRAM_t SETunnelingAttr;
#endif
#if gASL_ZclPrepayment_d
  sePrepaymentAttrRAM_t PrepaymentAttr;
#endif  
}sePrepayTermData_t;

typedef struct seSmartApplianceData_tag{
#if gZclClusterOptionals_d
  ZCLDRLCClientServerAttrsRAM_t DRLCAttr;
#endif   
  sePriceClientAttrRAM_t        PriceAttr;
#if gASL_ZclSETunneling_d
  seTunnelingAttrRAM_t SETunnelingAttr;
#endif
}seSmartApplianceData_t;

#ifdef __IAR_SYSTEMS_ICC__
#pragma pack()
#endif
/******************************************************************************
*******************************************************************************
* Public prototypes
*******************************************************************************
******************************************************************************/

extern const zclAttrDefList_t gZclKeyEstabServerAttrDefList;
extern const zclAttrDefList_t gZclSmplMetAttrDefList;
extern const zclAttrDefList_t gZclSmplMetMirrorAttrDefList;
extern const zclAttrDefList_t gZclDRLCClientServerClusterAttrDefList;
extern const zclAttrDefList_t gZclPriceServerAttrDefList;
extern const zclAttrDefList_t gZclPriceClientAttrDefList;
extern const zclAttrDefList_t gZclSETunnelingServerAttrDefList;
extern const zclAttrDefList_t gZclPrepaymentServerAttrDefList;

extern IdentifyCert_t DeviceImplicitCert;
extern uint8_t DevicePrivateKey[gZclCmdKeyEstab_PrivateKeySize_c];
extern uint8_t DevicePublicKey[gZclCmdKeyEstab_CompressedPubKeySize_c];
extern const uint8_t CertAuthPubKey[gZclCmdKeyEstab_CompressedPubKeySize_c];
extern const uint8_t CertAuthIssuerID[8];

#endif 
/* _SEPROFILE_H */

