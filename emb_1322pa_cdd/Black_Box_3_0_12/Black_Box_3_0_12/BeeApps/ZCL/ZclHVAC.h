/******************************************************************************
* ZclHVAC.c
*
* This module contains code that handles ZigBee Cluster Library commands and
* clusters for HVAC (Heating/Ventilation/Air-Conditioning).
*
* Copyright (c) 2007, Freescale, Inc.  All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
* Documents used in this specification:
* [R1] - 053520r16ZB_HA_PTG-Home-Automation-Profile.pdf
* [R2] - 075123r00ZB_AFG-ZigBee_Cluster_Library_Specification.pdf
*******************************************************************************/
#ifndef _ZCL_HVAC_H
#define _ZCL_HVAC_H

#include "AfApsInterface.h"
#include "AppAfInterface.h"
#include "BeeStackInterface.h"
#include "ZCL.h"


/******************************************************************************
*******************************************************************************
* Public macros and data types definitions.
*******************************************************************************
******************************************************************************/

/******************************************
	Thermostat Cluster
*******************************************/
#define gHaThermostatWeeklyScheduleEntries_d 2
#if (TRUE == gBigEndian_c)
/* 6.3.2.2.1 Thermostat information cluster attributes */
#define gZclAttrThermostat_LocalTemperatureId_c             0x0000 /* M - Local Temperature */
#define gZclAttrThermostat_OutdoorTemperatureId_c           0x0100 /* M - Outdoor Temperature */
#define gZclAttrThermostat_OccupancyId_c                    0x0200 /* O - Occupancy */
#define gZclAttrThermostat_AbsMinHeatSetpointLimitId_c      0x0300 /* O - AbsMinHeatSetpointLimit */
#define gZclAttrThermostat_AbsMaxHeatSetpointLimitId_c      0x0400 /* O - AbsMaxHeatSetpointLimit */
#define gZclAttrThermostat_AbsMinCoolSetpointLimitId_c      0x0500 /* O - AbsMinCoolSetpointLimit */
#define gZclAttrThermostat_AbsMaxCoolSetpointLimitId_c      0x0600 /* O - AbsMaxCoolSetpointLimit */
#define gZclAttrThermostat_PICoolingDemandId_c              0x0700 /* O - PI Cooling Demand */
#define gZclAttrThermostat_PIHeatingDemandId_c              0x0800 /* O - PI Heating Demand */

/* 6.3.2.2.2 Thermostat settings attributes */
#define gZclAttrThermostat_LocalTemperatureCalibrationId_c  0x1000 /* O - Local Temperature Calibration */
#define gZclAttrThermostat_OccupiedCoolingSetpointId_c      0x1100 /* M - Occupied Cooling Setpoint */
#define gZclAttrThermostat_OccupiedHeatingSetpointId_c      0x1200 /* M - Occupied Heating Setpoint */
#define gZclAttrThermostat_UnoccupiedCoolingSetpointId_c    0x1300 /* O - Unoccupied Cooling Setpoint */
#define gZclAttrThermostat_UnoccupiedHeatingSetpointId_c    0x1400 /* O - Unoccupied Heating Setpoint */
#define gZclAttrThermostat_MinHeatSetpointLimitId_c         0x1500 /* M - MinHeatSetpointLimit */
#define gZclAttrThermostat_MaxHeatSetpointLimitId_c         0x1600 /* M - MaxHeatSetpointLimit */
#define gZclAttrThermostat_MinCoolSetpointLimitId_c         0x1700 /* M - MinCoolSetpointLimit */
#define gZclAttrThermostat_MaxCoolSetpointLimitId_c         0x1800 /* M - MaxCoolSetpointLimit */
#define gZclAttrThermostat_MinSetpointDeadBandId_c          0x1900 /* M - MinSetpointDeadBand */
#define gZclAttrThermostat_RemoteSensingId_c                0x1a00 /* O - Remote Sensing */
#define gZclAttrThermostat_ControlSequenceOfOperationId_c   0x1b00 /* M - Control Sequence of Operation */
#define gZclAttrThermostat_SystemModeId_c                   0x1c00 /* M - System mode */
#define gZclAttrThermostat_AlarmMaskId_c                    0x1d00 /* O - Alarm Mask */
#define gZclAttrThermostat_StartOfWeek_c                    0x1e00 /* O - Start of Week */
#define gZclAttrThermostat_NumberOfWeeklyTransitions_c      0x1f00 /* O - Number of Weekly Transitions */

#else
/* 6.3.2.2.1 Thermostat information cluster attributes */
#define gZclAttrThermostat_LocalTemperatureId_c             0x0000 /* M - Local Temperature */
#define gZclAttrThermostat_OutdoorTemperatureId_c           0x0001 /* M - Outdoor Temperature */
#define gZclAttrThermostat_OccupancyId_c                    0x0002 /* O - Occupancy */
#define gZclAttrThermostat_AbsMinHeatSetpointLimitId_c      0x0003 /* O - AbsMinHeatSetpointLimit */
#define gZclAttrThermostat_AbsMaxHeatSetpointLimitId_c      0x0004 /* O - AbsMaxHeatSetpointLimit */
#define gZclAttrThermostat_AbsMinCoolSetpointLimitId_c      0x0005 /* O - AbsMinCoolSetpointLimit */
#define gZclAttrThermostat_AbsMaxCoolSetpointLimitId_c      0x0006 /* O - AbsMaxCoolSetpointLimit */
#define gZclAttrThermostat_PICoolingDemandId_c              0x0007 /* O - PI Cooling Demand */
#define gZclAttrThermostat_PIHeatingDemandId_c              0x0008 /* O - PI Heating Demand */

/* 6.3.2.2.2 Thermostat settings attributes */
#define gZclAttrThermostat_LocalTemperatureCalibrationId_c  0x0010 /* O - Local Temperature Calibration */
#define gZclAttrThermostat_OccupiedCoolingSetpointId_c      0x0011  /* M - Occupied Cooling Setpoint */
#define gZclAttrThermostat_OccupiedHeatingSetpointId_c      0x0012 /* M - Occupied Heating Setpoint */
#define gZclAttrThermostat_UnoccupiedCoolingSetpointId_c    0x0013 /* O - Unoccupied Cooling Setpoint */
#define gZclAttrThermostat_UnoccupiedHeatingSetpointId_c    0x0014 /* O - Unoccupied Heating Setpoint */
#define gZclAttrThermostat_MinHeatSetpointLimitId_c         0x0015 /* M - MinHeatSetpointLimit */
#define gZclAttrThermostat_MaxHeatSetpointLimitId_c         0x0016 /* M - MaxHeatSetpointLimit */
#define gZclAttrThermostat_MinCoolSetpointLimitId_c         0x0017 /* M - MinCoolSetpointLimit */
#define gZclAttrThermostat_MaxCoolSetpointLimitId_c         0x0018 /* M - MaxCoolSetpointLimit */
#define gZclAttrThermostat_MinSetpointDeadBandId_c          0x0019 /* M - MinSetpointDeadBand */
#define gZclAttrThermostat_RemoteSensingId_c                0x001a /* O - Remote Sensing */
#define gZclAttrThermostat_ControlSequenceOfOperationId_c   0x001b /* M - Control Sequence of Operation */
#define gZclAttrThermostat_SystemModeId_c                   0x001c /* M - System mode */
#define gZclAttrThermostat_AlarmMaskId_c                    0x001d /* O - Alarm Mask */
#define gZclAttrThermostat_StartOfWeek_c                    0x001e /* O - Start of Week */
#define gZclAttrThermostat_NumberOfWeeklyTransitions_c      0x001f /* O - Number of Weekly Transitions */
#endif /* #if (TRUE == gBigEndian_c) */

enum{
/* 6.3.2.2.2.12 Thermostat Control Sequence of Operation Attribute Values */
   gZclControlSecuence_CoolingOnly_c = 0x00,                        /* Cooling Only */
   gZclControlSecuenceCoolingWithReheat_c = 0x01,                   /* Cooling with reheat */
   gZclControlSecuence_HeatingOnly_c = 0x02,                        /* Heating Only */
   gZclControlSecuence_HeatingWithReheat_c = 0x03,                  /* Heating with reheat */
   gZclControlSecuence_CoolingAndHeating4Pipes_c = 0x04,            /* Cooling and Heating */
   gZclControlSecuence_CoolingAndHeating4PipesWithReheat_c = 0x05   /* Cooling and Heating 4Pipes With Reheat */
};

/* 6.3.2.2.4 Thermostat Alarm Mask attribute values */
#define gZclThermostat_InitializationFailure_c    0 /* Initialization Failure */
#define gZclThermostat_HardwareFailure_c          1 /* Hardware Failure */
#define gZclThermostat_SelfCalibrationFailure_c   2 /* Self Calibration Failure */

/* Thermostat Mode */
enum{
/* 6.3.2.2.3 Thermostat Mode Values */
   gThermostatModeHeat_c = 0x00,
   gThermostatModeCool_c = 0x01,
   gThermostatModeBoth_c = 0x02
};

/* 6.3.2.3 Thermostat Server Commands Received */
#define gZclCmdThermostat_SetpointRaiseLower_c 0x00   /* Setpoint Raise / Lower */
#define gZclCmdThermostat_SetWeeklySchedule_c 0x01    /* Set Weekly Schedule */
#define gZclCmdThermostat_GetWeeklySchedule_c 0x02    /* Get Weekly Schedule */
#define gZclCmdThermostat_ClearWeeklySchedule_c 0x03  /* Clear Weekly Schedule */

/* Thermostat Server Commands Sent */
#define gZclCmdThermostat_CurrentWeeklySchedule_c 0x00  /* Current Weekly Schedulre */

#ifdef __IAR_SYSTEMS_ICC__
#pragma pack(1)
#endif
typedef struct zclThermostatAttrsRAM_tag
{
   /*Attributes of the Thermostat Information attribute set */
  int16_t   LocalTemperature[zclReportableCopies_c];        /* Local or Outdoor temperature */
#if gZclClusterOptionals_d
  uint16_t  OutdoorTemperature;
  uint8_t   Occupancy; 
  int16_t   AbsMinHeatSetpointLimit;
  int16_t   AbsMaxHeatSetpointLimit;
  int16_t   AbsMinCoolSetpointLimit;
  int16_t   AbsMaxCoolSetpointLimit;
  uint8_t   PICoolingDemand[zclReportableCopies_c];
  uint8_t   PIHeatingDemand[zclReportableCopies_c];
#endif

    /*Attributes o 1 f the Thermostat settings attribute set */
  int16_t   OccupiedCoolingSetpoint[zclReportableCopies_c];
  int16_t   OccupiedHeatingSetpoint[zclReportableCopies_c];
  int16_t   MinHeatSetpointLimit;
  int16_t   MaxHeatSetpointLimit;
  int16_t   MinCoolSetpointLimit;
  int16_t   MaxCoolSetpointLimit;
  int8_t    MinSetpointDeadBand;	
  uint8_t   ControlSequenceOfOperation;
  uint8_t   SystemMode[zclReportableCopies_c];
	
#if gZclClusterOptionals_d
  int8_t    LocalTemperatureCalibration;
  int16_t   UnoccupiedCoolingSetpoint;
  int16_t   UnoccupiedHeatingSetpoint;
  uint8_t   Remotesensing;
  uint8_t   AlarmMask;
  uint8_t   StartOfWeek;
  uint8_t   NumberOfWeeklyTransitions;
#endif
} zclThermostatAttrsRAM_t;

typedef struct thermostatSetPoint_tag
{
  uint16_t  TransitionTime;
  int16_t   HeatSetPoint;
  int16_t   CoolSetPoint;
} thermostatSetPoint_t;

typedef struct transitionSetPoint_tag
{
  uint8_t               TransitionDayOfWeek;
  thermostatSetPoint_t  SetPoint;
} transitionSetPoint_t;

/* Data structure for the Thermostat weekly schedule, 7 days plus vacation */
typedef struct haThermostatWeeklyScheduleRAM_tag
{
  thermostatSetPoint_t WeeklySetPoints[8][gHaThermostatWeeklyScheduleEntries_d];
} haThermostatWeeklyScheduleRAM_t;

/* 6.3.2.3.1  Thermostat Setpoint Raise/Lower Command */
typedef struct zclCmdThermostat_SetpointRaiseLower_tag
{
  uint8_t                   Mode;
  int8_t                    Amount;   /* in steps per second */
} zclCmdThermostat_SetpointRaiseLower_t;

/* Setpoint Raise/Lower Request frame */
typedef struct zclThermostat_SetpointRaiseLower_tag
{
  afAddrInfo_t  addrInfo;  /* IN: */
  zclCmdThermostat_SetpointRaiseLower_t cmdFrame;  /* command frame */
} zclThermostat_SetpointRaiseLower_t;

/* Current Weekly Schedule command */
typedef struct zclCmdThermostat_CurrentWeeklySchedule_tag
{
  uint8_t               NumberOfTransactionsForSequence;
  transitionSetPoint_t  WeeklySetPointSchedule[1];
} zclCmdThermostat_CurrentWeeklySchedule_t;

/* Current Weekly Schedule frame */
typedef struct zclThermostat_CurrentWeeklySchedule_tag
{
  afAddrInfo_t  addrInfo;
  uint8_t zclTransactionId;
  zclCmdThermostat_CurrentWeeklySchedule_t cmdFrame;
} zclThermostat_CurrentWeeklySchedule_t;

/* Set Weekly Schedule command */
typedef struct zclCmdThermostat_SetWeeklySchedule_tag
{
  uint8_t               NumberOfTransactionsForSequence;
  transitionSetPoint_t  WeeklySetPointSchedule[1];
} zclCmdThermostat_SetWeeklySchedule_t;

/* Set Weekly Schedule frame */
typedef struct zclThermostat_SetWeeklySchedule_tag
{
  afAddrInfo_t  addrInfo;
  uint8_t zclTransactionId;
  zclCmdThermostat_SetWeeklySchedule_t cmdFrame;
} zclThermostat_SetWeeklySchedule_t;

/* Get Weekly Schedule command */
typedef struct zclCmdThermostat_GetWeeklySchedule_tag
{
  uint8_t DaysToReturn;
} zclCmdThermostat_GetWeeklySchedule_t;

/* Get Weekly Schedule frame */
typedef struct zclThermostat_GetWeeklySchedule_tag
{
  afAddrInfo_t  addrInfo;
  uint8_t zclTransactionId;
  zclCmdThermostat_GetWeeklySchedule_t cmdFrame;
} zclThermostat_GetWeeklySchedule_t;

/* Clear Weekly Schedule frame */
typedef struct zclThermostat_ClearWeeklySchedule_tag
{
  afAddrInfo_t  addrInfo;
  uint8_t zclTransactionId;
} zclThermostat_ClearWeeklySchedule_t;

/******************************************
	Fan Control Cluster
*******************************************/
#if (TRUE == gBigEndian_c)
/* 6.4.2.2 Attributes of the Fan Control cluster */
#define gZclAttrFanControl_FanModeId_c           0x0000 /*Fan Mode*/
#define gZclAttrFanControl_FanModeSequence_c     0x0100 /*Fan Mode Sequence*/
#else
#define gZclAttrFanControl_FanModeId_c           0x0000 /*Fan Mode*/
#define gZclAttrFanControl_FanModeSequence_c     0x0001 /*Fan Mode Sequence*/
#endif

enum{
/* 6.4.2.2.1 FanMode attribute values */
    gZcl_FanMode_Off_c    = 0x00, /* Off */
    gZcl_FanMode_Low_c    = 0x01, /* On */
    gZcl_FanMode_Medium_c = 0x02, /* Medium */
    gZcl_FanMode_High_c   = 0x03, /* High */
    gZcl_FanMode_On_c     = 0x04, /* On */
    gZcl_FanMode_Auto_c   = 0x05, /*Auto (the fan speed is self-regulated)*/
    gZcl_FanMode_Smart_c  = 0x06 /*Smart (when the heated/cooled space is occupied, the fan is always on)*/
};

enum {
/* 6.4.2.2.2  FanSequenceOperation attribute values*/
    gZclFanModeSequence_LowMedHigh_c = 0x00, /*  Low/Med/High  */
    gZclFanModeSequence_LowHigh_c = 0x01, /*  Low/High  */
    gZclFanModeSequence_LowMedHighAuto_c = 0x02, /*  Low/Med/High/Auto  */
    gZclFanModeSequence_LowHighAuto_c = 0x03, /*  Low/High/Auto  */
    gZclFanModeSequence_OnAuto_c = 0x04, /*  On/Auto  */
};


typedef struct zclFanControlAttrs_tag
{
  uint8_t FanMode;   /* Fan Mode is an enumeration */
  uint8_t FanModeSequence; /* Fan Mode Sequence of operation */
} zclFanControlAttrs_t;


/******************************************
	Thermostat User Interface Configuration
*******************************************/
#if (TRUE == gBigEndian_c)
/* 6.6.2.2 Attributes of the Thermostat User Interface Configuration cluster */
#define gZclAttrThermostatUserInterface_TempDisplayModeId_c    0x0000 /* Temperature display mode */
#define gZclAttrThermostatUserInterface_KeypadLockoutId_c      0x0100 /* Keypad Lockout */
#else
#define gZclAttrThermostatUserInterface_TempDisplayModeId_c    0x0000 /* Temperature display mode */
#define gZclAttrThermostatUserInterface_KeypadLockoutId_c      0x0001 /* Keypad Lockout */
#endif

enum {
/* KeypadLockout Attribute KeypadLockout attribute values */
    gZclKeypad_NoLockout_c = 0x00,
    gZclKeypad_LockoutLevel1_c = 0x01,
    gZclKeypad_LockoutLevel2_c = 0x02,
    gZclKeypad_LockoutLevel3_c = 0x03,
    gZclKeypad_LockoutLevel4_c = 0x04,
    gZclKeypad_LockoutLevel5_c = 0x05
};

/* No commands are generated by the server or recived by the client except responses to commands 
    to read and write the attributes of the server */

typedef struct zclThermostatUICfgAttrsRAM_tag
{
  uint8_t   DisplayMode;   /* Display mode is an enumeration */
  uint8_t   KeyPadLockout;
} zclThermostatUICfgAttrsRAM_t;
#ifdef __IAR_SYSTEMS_ICC__
#pragma pack()
#endif
/******************************************************************************
*******************************************************************************
* Public functions prototypes
*******************************************************************************
******************************************************************************/
zbStatus_t ZCL_ThermostatSetpointRaiseLowerReq(zclThermostat_SetpointRaiseLower_t *pReq);
zbStatus_t ZCL_ThermostatSetWeeklyScheduleReq(zclThermostat_SetWeeklySchedule_t *pReq);
zbStatus_t ZCL_ThermostatGetWeeklyScheduleReq(zclThermostat_GetWeeklySchedule_t *pReq);
zbStatus_t ZCL_ThermostatClearWeeklyScheduleReq(zclThermostat_ClearWeeklySchedule_t *pReq);
zbStatus_t ZCL_ThermostatClusterServer(zbApsdeDataIndication_t *pIndication,afDeviceDef_t *pDevice);
zbStatus_t ZCL_ThermostatClearWeeklySchedule(haThermostatWeeklyScheduleRAM_t *pData);
zbStatus_t ZCL_FanControlClusterServer(zbApsdeDataIndication_t *pIndication,afDeviceDef_t *pDevice);
zbStatus_t ZCL_ThermostatUserInterfaceCfgClusterServer(zbApsdeDataIndication_t *pIndication,afDeviceDef_t *pDevice);
#if gAddValidationFuncPtrToClusterDef_c
bool_t  ZCL_ThermostatValidateAttributes(zbEndPoint_t endPoint, zbClusterId_t clusterId, void *pAttrDef);
bool_t  ZCL_ThermostatUserCfgValidateAttributes(zbEndPoint_t endPoint, zbClusterId_t clusterId, void *pAttrDef);
#else
#define ZCL_ThermostatValidateAttributes
#define ZCL_ThermostatUserCfgValidateAttributes
#endif


#endif
