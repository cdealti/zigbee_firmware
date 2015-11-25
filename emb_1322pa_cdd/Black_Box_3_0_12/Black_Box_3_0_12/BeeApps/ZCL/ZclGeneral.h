/****************************************************************************
* ZclGeneral.h
*
* This source file describes Clusters defined in the ZigBee Cluster Library,
* General document. Some or all of these
*
* Copyright (c) 2007, Freescale, Inc. All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
* [R1] - 053520r16ZB_HA_PTG-Home-Automation-Profile.pdf
* [R2] - 075123r00ZB_AFG-ZigBee_Cluster_Library_Specification.pdf
*
******************************************************************************/
#ifndef _ZCL_GENERAL_H
#define _ZCL_GENERAL_H

#include "ZclOptions.h"
#include "AfApsInterface.h"
#include "AppAfInterface.h"
#include "BeeStackInterface.h"
#include "ZCL.h"


/******************************************************************************
*******************************************************************************
* Public macros and data types definitions.
*******************************************************************************
******************************************************************************/
/*The Level Min and Max values representes the range, in which the Level Control,
  this values are application dependant.
  For the Dimmable Light application:
    When the Level is set to 0, the light shall be turned fully off. When the level is set to 254, the light
  shall be turned on to the maximum level possible for the device.
  */
#define gZclLevelMinValue        0x00  /* 0 */
#define gZclLevelMaxValue        0xFF              
#define gZclLevelNumberOfLevels  0x03
#define gZclLevelDelta  (uint8_t)((gZclLevelMaxValue - gZclLevelMinValue + 1)/gZclLevelNumberOfLevels)

/******************************************
	Basic Cluster
*******************************************/

#if ( TRUE == gBigEndian_c )

/* basic cluster attribute IDs (see ZclGeneral.c for values) */
#define gZclAttrBasic_ZCLVersionId_c               0x0000 /* M - ZCLVersion */
#define gZclAttrBasic_ApplicationVersionId_c       0x0100 /* O - ApplicationVersion */
#define gZclAttrBasic_StackVersionId_c             0x0200 /* O - StackVersion */
#define gZclAttrBasic_HWVersionId_c                0x0300 /* O - HWVersion */
#define gZclAttrBasic_ManufacturerNameId_c         0x0400 /* O - ManufacturerName */
#define gZclAttrBasic_ModelIdentifierId_c          0x0500 /* O - ModelIdentifier */
#define gZclAttrBasic_DateCodeId_c                 0x0600 /* O - DateCode */
#define gZclAttrBasic_PowerSourceId_c              0x0700 /* M - PowerSource */
#define gZclAttrBasic_LocationDescriptionId_c      0x1000 /* O - LocationDescription */
#define gZclAttrBasic_PhysicalEnvironmentId_c      0x1100 /* O - PhysicalEnvironment */
#define gZclAttrBasic_DeviceEnabledId_c            0x1200 /* M - DeviceEnabled Boolean */
#define gZclAttrBasic_AlarmMaskId_c                0x1300 /* O - AlarmMask */
#else
/* basic cluster attribute IDs (see ZclGeneral.c for values) */
#define gZclAttrBasic_ZCLVersionId_c               0x0000 /* M - ZCLVersion */
#define gZclAttrBasic_ApplicationVersionId_c       0x0001 /* O - ApplicationVersion */
#define gZclAttrBasic_StackVersionId_c             0x0002 /* O - StackVersion */
#define gZclAttrBasic_HWVersionId_c                0x0003 /* O - HWVersion */
#define gZclAttrBasic_ManufacturerNameId_c         0x0004 /* O - ManufacturerName */
#define gZclAttrBasic_ModelIdentifierId_c          0x0005 /* O - ModelIdentifier */
#define gZclAttrBasic_DateCodeId_c                 0x0006 /* O - DateCode */
#define gZclAttrBasic_PowerSourceId_c              0x0007 /* M - PowerSource */
#define gZclAttrBasic_LocationDescriptionId_c      0x0010 /* O - LocationDescription */
#define gZclAttrBasic_PhysicalEnvironmentId_c      0x0011 /* O - PhysicalEnvironment */
#define gZclAttrBasic_DeviceEnabledId_c            0x0012 /* M - DeviceEnabled Boolean */
#define gZclAttrBasic_AlarmMaskId_c                0x0013 /* O - AlarmMask */

#endif /* #if ( TRUE == gBigEndian_c ) */

/* power source (8-bit value) */
#define gZclAttrBasicPowerSource_Unknown_c     0x00 /* Unknown */
#define gZclAttrBasicPowerSource_Mains_c       0x01 /* Mains (single phase) */
#define gZclAttrBasicPowerSource_Mains3p_c     0x02 /* Mains (3 phase) */
#define gZclAttrBasicPowerSource_Battery_c     0x03 /* Battery */
#define gZclAttrBasicPowerSource_DC_c          0x04 /* DC source */
#define gZclAttrBasicPowerSource_Emergency1_c  0x05 /* Emergency mains constantly powered */
#define gZclAttrBasicPowerSource_Emergency2_c  0x06 /* Emergency mains and transfer switch */

/* enabled attribute values */
#define gZclDeviceDisabled_c 0x00
#define gZclDeviceEnabled_c  0x01

/* 16-bytes in RAM for location description */
#define gZclAttrBasic_LocationDescriptionLen_c 16

/* Physical Environment  */
#define gZclAttrBasic_PhysicalEnvironment  gZclAttrBasicPhylEnvSpecifiedEnvironment

#define gZclAttrBasicPhyEnvUnspecifiedEnvironment 0x00
#define gZclAttrBasicPhylEnvSpecifiedEnvironment  0x01

/* basic cluster commands */
#define gZclCmdBasic_Reset_c                   0x00 /* O - Reset to Factory Defaults */
#ifdef __IAR_SYSTEMS_ICC__
#pragma pack(1)
#endif
/* reset system to factory defaults */
typedef struct zclBasicResetReq_tag
{
  afAddrInfo_t      addrInfo;   /* IN: source/dst address, cluster, etc... */
  /* no payload, just the command */
} zclBasicResetReq_t;

/* list of attributes in RAM for this cluster */
typedef struct zclBasicAttrsRAM_tag
{
  zclStr16_t LocationDescription;  /* 0x0010 LocationDescription, str16, "" RdWr - O */
  uint8_t PhysicalEnvironment;    /* 0x0011 PhysicalEnvironment, 0x00, RdWr - O */
  uint8_t DeviceEnabled ;         /* 0x0012 DeviceEnabled Boolean, 0x01, RdWr - M */
  uint8_t AlarmMask;              /* 0x0013 AlarmMask, 0x00, RdWr - O */
} zclBasicAttrsRAM_t;

/* list of attributes in ROM for this cluster */
typedef struct zclBasicAttrsROM_tag
{
  uint8_t    ZCLVersion;           /* 0x0000 ZCLVersion - 0x01 - RdOnly - M */
  uint8_t    ApplicationVersion;   /* 0x0001 ApplicationVersion - 0x00 - RdOnly - O */
  uint8_t    StackVersion;         /* 0x0002 StackVersion - 0x00 - RdOnly - O */
  uint8_t    HWVersion;            /* 0x0003 HWVersion - 0x00 - RdOnly - O */
  zclStr32_t ManufacturerName;   /* 0x0004 ManufacturerName - "Freescale", 32 - RdOnly - O */
  zclStr32_t ModelIdentifier;    /* 0x0005 ModelIdentifier - "On/Off Light", 32 - RdOnly - O */
  zclStr16_t DateCode;           /* 0x0006 DateCode - "20081404", 16, - RdOnly - O */
  uint8_t    iPowerSource;       /* 0x0007 PowerSource - 0x01(mains) - M */
} zclBasicAttrsROM_t;

/**********************************************************************
	Power Configuration Cluster (ZigBee Cluster Library Chapter 3.3)
**********************************************************************/

/* Power Configuration Attributes Sets */
#if ( TRUE == gBigEndian_c )
#define gZclAttrPwrConfigMainsInformationSet_c    0x0000    /* Mains Information attributes Set */
#define gZclAttrPwrConfigMainsSettingsSet_c       0x0100    /* Mains Settings attributes Set */
#define gZclAttrPwrConfigBatteryInformationSet_c  0x0200    /* Battery Information attributes Set */
#define gZclAttrPwrConfigBatterySettingsSet_c     0x0300    /* Battery Settings attributes Set */
#else
#define gZclAttrPwrConfigMainsInformationSet_c    0x0000    /* Mains Information attributes Set */
#define gZclAttrPwrConfigMainsSettingsSet_c       0x0001    /* Mains Settings attributes Set */
#define gZclAttrPwrConfigBatteryInformationSet_c  0x0002    /* Battery Information attributes Set */
#define gZclAttrPwrConfigBatterySettingsSet_c     0x0003    /* Battery Settings attributes Set */
#endif /* #if ( TRUE == gBigEndian_c ) */

/* 3.3.2.2.1 Mains Information Attributes Set */
#if ( TRUE == gBigEndian_c )
#define gZclAttrPwrConfigMainsInfMainsVoltage_c    0x0000    /* O - Mains Voltage */
#define gZclAttrPwrConfigMainsInfMainsFrequency_c  0x0100    /* O - Mains Frequency */
#else
#define gZclAttrPwrConfigMainsInfMainsVoltage_c    0x0000    /* O - Mains Voltage */
#define gZclAttrPwrConfigMainsInfMainsFrequency_c  0x0001    /* O - Mains Frequency */
#endif /* #if ( TRUE == gBigEndian_c ) */

                                 
/* 3.3.2.2.2 Mains Settings Attributes Set */
#if ( TRUE == gBigEndian_c )
#define gZclAttrPwrConfigMainsStgMainsAlarmMask_c             0x1000    /* O - Mains Information attributes Set */
#define gZclAttrPwrConfigMainsStgMainsVoltageMinThreshold_c   0x1100    /* O - Mains Settings attributes Set */
#define gZclAttrPwrConfigMainsStgMainsVoltageMaxThreshold_c   0x1200    /* O - Battery Information attributes Set */
#define gZclAttrPwrConfigMainsStgMainsVoltageDwellTripPoint_c 0x1300    /* O - Battery Settings attributes Set */
#else
#define gZclAttrPwrConfigMainsStgMainsAlarmMask_c             0x0010    /* O - Mains Information attributes Set */
#define gZclAttrPwrConfigMainsStgMainsVoltageMinThreshold_c   0x0011    /* O - Mains Settings attributes Set */
#define gZclAttrPwrConfigMainsStgMainsVoltageMaxThreshold_c   0x0012    /* O - Battery Information attributes Set */
#define gZclAttrPwrConfigMainsStgMainsVoltageDwellTripPoint_c 0x0013    /* O - Battery Settings attributes Set */
#endif /* #if ( TRUE == gBigEndian_c ) */

/* Mains Alarm Mask */

typedef struct gZclAlarmMask_tag
{
    unsigned MainsVoltage2Low   :1;    /* Mains voltage too Low */                             
    unsigned MainsVoltage2High  :1;    /* Mains voltage too High */                              
    unsigned Reserved           :6;    /* Reserved */                                    
}gZclAlarmMask_t; 

/* 3.3.2.2.3 Battery Information Attributes Set */
#if ( TRUE == gBigEndian_c )
#define gZclAttrPwrConfigBatteryInfBatteryVoltage_c             0x2000    /* O - Battery Voltage */
#else
#define gZclAttrPwrConfigBatteryInfBatteryVoltage_c             0x0020    /* O - Battery Voltage */
#endif /* #if ( TRUE == gBigEndian_c ) */


/* 3.3.2.2.4 Battery Settings Attributes Set */
#if ( TRUE == gBigEndian_c )
#define gZclAttrPwrConfigBatteryStgBatteryManufacturer_c          0x3000    /* O - Battery Manufacturer */
#define gZclAttrPwrConfigBatteryStgBatterySize_c                  0x3100    /* O - Battery Size */
#define gZclAttrPwrConfigBatteryStgBatteryAHrRating_c             0x3200    /* O - Battery A/Hr Rating */
#define gZclAttrPwrConfigBatteryStgBatteryQuantity_c              0x3300    /* O - Battery Quantity */
#define gZclAttrPwrConfigBatteryStgBatteryRatedVoltage_c          0x3400    /* O - Battery Rated Voltage */
#define gZclAttrPwrConfigBatteryStgBatteryAlarmMask_c             0x3500    /* O - Battery Alarm Mask */
#define gZclAttrPwrConfigBatteryStgBatteryVoltageMinThreshold_c   0x3600    /* O - Battery Voltage Min Threshold */
#else
#define gZclAttrPwrConfigBatteryStgBatteryManufacturer_c          0x0030    /* O - Battery Manufacturer */
#define gZclAttrPwrConfigBatteryStgBatterySize_c                  0x0031    /* O - Battery Size */
#define gZclAttrPwrConfigBatteryStgBatteryAHrRating_c             0x0032    /* O - Battery A/Hr Rating */
#define gZclAttrPwrConfigBatteryStgBatteryQuantity_c              0x0033    /* O - Battery Quantity */
#define gZclAttrPwrConfigBatteryStgBatteryRatedVoltage_c          0x0034    /* O - Battery Rated Voltage */
#define gZclAttrPwrConfigBatteryStgBatteryAlarmMask_c             0x0035    /* O - Battery Alarm Mask */
#define gZclAttrPwrConfigBatteryStgBatteryVoltageMinThreshold_c   0x0036    /* O - Battery Voltage Min Threshold */
#endif /* #if ( TRUE == gBigEndian_c ) */

/* Battery size */
enum
{
   gBatterySize_NoBattery = 0x00,       /* No Battery */
   gBatterySize_BuiltIn,                /* Built In */
   gBatterySize_Other,                  /* Other */
   gBatterySize_AA,                     /* AA */
   gBatterySize_AAA,                    /* AAA */
   gBatterySize_C,                      /* C */
   gBatterySize_D,                      /* D */
                                        /* 0x07-0xFE Reserved */
   gBatterySize_Unknown = 0xFF,         /* Unknown */                                    
};

/* Battery Alarm Mask */
enum
{
   gBatteryAlarmMask_BatteryVoltage2Low = 0,       /* Battery voltage too low */
                                                   /* 1 - 7 Reserved */                                    
};
/* The comand use for this cluster are the read/write attributes */
#ifdef __IAR_SYSTEMS_ICC__
#pragma pack(1)
#endif
typedef struct zclPowerCfgAttrsRAM_tag
{
  uint16_t    mainsVoltage; 
  uint8_t     mainsFrequency;
  uint8_t     mainsAlarmMask;
  uint16_t    mainsVoltageMinThreshold;
  uint16_t    mainsVoltageMaxThreshold;
  uint16_t    mainsVoltageDwellTripPoint;
  uint8_t     batteryVoltage;
  zclStr16_t  batteryMfg;
  uint8_t     batterySize;
  uint16_t    batteryAHrRating;
  uint8_t     batteryQuantity;
  uint8_t     batteryRatedVoltage;
  uint8_t     batteryAlarmMask;
  uint8_t     batteryVoltageMinThreshold;
} zclPowerCfgAttrsRAM_t;


/*******************************************************************************
    Device Temperature Configuration Cluster (ZigBee Cluster Library Chapter 3.4)
*******************************************************************************/

typedef uint8_t  SumElem_t[3];

/* Device Temperature Configuration Attributes Sets */
#if ( TRUE == gBigEndian_c )
#define gZclAttrTempCfgTempInformationSet_c    0x0000    /* Device Temperature Information Set */
#define gZclAttrTempCfgTempSettingsSet_c       0x0100    /* Device Temperature Settings Set */
                                                         /*0x002-0xFFF - reserved*/
#else
#define gZclAttrTempCfgTempInformationSet_c    0x0000    /* Device Temperature Information Set */
#define gZclAttrTempCfgTempSettingsSet_c       0x0001    /* Device Temperature Settings Set */
                                                         /*0x002-0xFFF - reserved*/
#endif /* #if ( TRUE == gBigEndian_c ) */

#if ( TRUE == gBigEndian_c )
/* 3.4.2.2.1 Device temperature information attributes */
#define gZclAttrTempCfgTempInfCurrentTemp_c             0x0000    /* M - Current temperature */
#define gZclAttrTempCfgTempInfMinTempExperienced_c      0x0100    /* O - MinTempExperienced */
#define gZclAttrTempCfgTempInfMaxTempExperienced_c      0x0200    /* O - MaxTempExperienced */
#define gZclAttrTempCfgTempInfOverTempTotalDwell_c      0x0300    /* O - OverTempTotalDwell */
/* 3.4.2.2.2 Device temperature settings attributes */
#define gZclAttrTempCfgTempStgDeviceTempAlarmMask_c     0x1000    /* O - DeviceTempAlarmMask */
#define gZclAttrTempCfgTempStgLowTempThreshold_c        0x1100    /* O - LowTempThreshold */
#define gZclAttrTempCfgTempStgHighTempThreshold_c       0x1200    /* O - HighTempThreshold*/
#define gZclAttrTempCfgTempStgLowTempDwellTripPoint_c   0x1300    /* O - LowTempDwellTripPoint*/
#define gZclAttrTempCfgTempStgHighTempDwellTripPoint_c  0x1400    /* O - HighTempDwellTripPoint*/
#else
/* 3.4.2.2.1 Device temperature information attributes */
#define gZclAttrTempCfgTempInfCurrentTemp_c             0x0000    /* M - Current temperature */
#define gZclAttrTempCfgTempInfMinTempExperienced_c      0x0001    /* O - MinTempExperienced */
#define gZclAttrTempCfgTempInfMaxTempExperienced_c      0x0002    /* O - MaxTempExperienced */
#define gZclAttrTempCfgTempInfOverTempTotalDwell_c      0x0003    /* O - OverTempTotalDwell */
/* 3.4.2.2.2 Device temperature settings attributes */
#define gZclAttrTempCfgTempStgDeviceTempAlarmMask_c     0x0010    /* O - DeviceTempAlarmMask */
#define gZclAttrTempCfgTempStgLowTempThreshold_c        0x0011    /* O - LowTempThreshold */
#define gZclAttrTempCfgTempStgHighTempThreshold_c       0x0012    /* O - HighTempThreshold*/
#define gZclAttrTempCfgTempStgLowTempDwellTripPoint_c   0x0013    /* O - LowTempDwellTripPoint*/
#define gZclAttrTempCfgTempStgHighTempDwellTripPoint_c  0x0014    /* O - HighTempDwellTripPoint*/
#endif 

/* DeviceTemp Alarm Mask */

typedef struct gZclTempAlarmMask_tag
{
    unsigned MainsVoltage2Low   :1;    /* Device temp too Low */                             
    unsigned MainsVoltage2High  :1;    /* Device temp too High */                              
    unsigned Reserved           :6;    /* Reserved */                                    
}gZclTempAlarmMask_t; 

/* The comand use for this cluster are the read/write attributes */
#ifdef __IAR_SYSTEMS_ICC__
#pragma pack(1)
#endif
typedef struct zclTempCfgAttrsRAM_tag
{
  int16_t     currentTemp; 
  int16_t     minTempExperienced;
  int16_t     maxTempExperienced;
  uint16_t    overTempTotalDwell;
  uint8_t     deviceTempAlarmMask;
  uint16_t    lowTempThreshold;
  uint16_t    highTempThreshold;
  SumElem_t   lowTempDwellTripPoint;
  SumElem_t   highTempDwellTripPoint;
} zclTempCfgAttrsRAM_t;


/******************************************
	Identify Cluster
*******************************************/
/* identify attribute IDs (see HaProfile.c for values */
#define gZclAttrIdentify_TimeId_c  0x0000  /* time left (secs) in identify mode */

typedef struct zclIdentifyAttrsRAM_tag
{
  uint16_t  time;     /* in seconds */
} zclIdentifyAttrsRAM_t;

/* identify cluster commands */
#define gZclCmdIdentify_c          0x00  /* identify command */
#define gZclCmdIdentifyQuery_c     0x01  /* devices will respond if in identify mode */
#define gZclCmdIdentifyQueryRsp_c  0x00  /* response to query command */

/* note: identify query command has no payload */

typedef struct zclCmdIdentifyCmd_tag
{
  uint16_t  iTimeIdentify;
} zclCmdIdentify_t;

typedef struct zclIdentifyReq_tag
{
  afAddrInfo_t         addrInfo;  /* IN: source/dst address, cluster, etc... */
  zclCmdIdentify_t     cmdFrame;  /* IN: command frame */
} zclIdentifyReq_t;

typedef struct zclIdentifyQueryReq_tag
{
  afAddrInfo_t         addrInfo;  /* IN: source/dst address, cluster, etc... */
} zclIdentifyQueryReq_t;


/* identify query command response */
typedef struct zclCmdIdentifyQueryRsp_tag
{
  uint16_t  iTimeLeft;          /* in seconds */
} zclCmdIdentifyQueryRsp_t;

/******************************************
	Group Cluster
*******************************************/

/* Group Attributes */
#define gZclAttrGroup_NameSupport_c             0x0000 /* rdOnly M - Group Name Supported */

/* 3.6.2 Group Commands */
#define gZclCmdGroup_AddGroup_c                 0x00 /* M - Add group */
#define gZclCmdGroup_ViewGroup_c                0x01 /* M - View group */
#define gZclCmdGroup_GetGroupMembership_c       0x02 /* M - Get group membership */
#define gZclCmdGroup_RemoveGroup_c              0x03 /* M - Remove group */
#define gZclCmdGroup_RemoveAllGroups_c          0x04 /* M - Remove all groups */
#define gZclCmdGroup_AddGroupIfIdentifying_c    0x05 /* M - Add group if identifying */

/* 3.6.2.2.3 Add Group command */
typedef struct zclCmdGroup_AddGroup_tag
{
  zbGroupId_t aGroupId;   /* group ID*/
  char szGroupName[1];    /* up to 16-bytes, string with group name */
} zclCmdGroup_AddGroup_t;

/* 3.6.2.2.4 View Group command */
typedef struct zclCmdGroup_ViewGroup_tag
{
  zbGroupId_t aGroupId;  /* group ID*/
} zclCmdGroup_ViewGroup_t;

/* 3.6.2.2.5 Get group membership command */
typedef struct zclCmdGroup_GetGroupMembership_tag
{
  uint8_t     count;        /* count of following list */
  zbGroupId_t aGroupId[1];  /* group ID*/
} zclCmdGroup_GetGroupMembership_t;

/* 3.6.2.2.6 Remove Group command */
typedef struct zclCmdGroup_RemoveGroup_tag
{
  zbGroupId_t aGroupId;  /* group ID*/
} zclCmdGroup_RemoveGroup_t;

/* 3.6.2.2.7 Remove All Groups command */
typedef struct zclCmdGroup_RemoveAllGroups_tag
{
  zclCmd_t     command;  /* remove all groups command */
} zclCmdGroup_RemoveAllGroups_t;

/* 3.6.2.2.8 Add Group If Identifying command */
typedef struct zclCmdGroup_AddGroupIfIdentifying_tag
{
  zbGroupId_t aGroupId;  /* group ID*/
  char szGroupName[1];   /* string with group name */
} zclCmdGroup_AddGroupIfIdentifying_t;

/* add endpoint to a group */
typedef struct zclGroupAddGroupReq_tag
{
  afAddrInfo_t            addrInfo;  /* IN: source/dst address, cluster, etc... */
  zclCmdGroup_AddGroup_t  cmdFrame;  /* IN: command frame */
} zclGroupAddGroupReq_t;

/* View Group Request */
typedef struct zclGroupViewGroupReq_tag
{
  afAddrInfo_t             addrInfo;   /* IN: source/dst address, cluster, etc... */
  zclCmdGroup_ViewGroup_t  cmdFrame;   /* IN: command frame */
} zclGroupViewGroupReq_t;

/* Get Group Membership */
typedef struct zclGroupGetGroupMembershipReq_tag
{
  afAddrInfo_t                      addrInfo;   /* IN: source/dst address, cluster, etc... */
  zclCmdGroup_GetGroupMembership_t  cmdFrame;   /* IN: command frame */
} zclGroupGetGroupMembershipReq_t;

/* Remove Group Request */
typedef struct zclGroupRemoveGroupReq_tag
{
  afAddrInfo_t              addrInfo;   /* IN: source/dst address, cluster, etc... */
  zclCmdGroup_RemoveGroup_t cmdFrame;   /* IN: command frame */
} zclGroupRemoveGroupReq_t;

/* Remove All Groups Request */
typedef struct zclGroupRemoveAllGroupsReq_tag
{
  afAddrInfo_t          addrInfo;   /* IN: source/dst address, cluster, etc... */
  /* remove all groups has no payload */
} zclGroupRemoveAllGroupsReq_t;

/* Add Group If Identifying */
typedef struct zclGroupAddGroupIfIdentifyingReq_tag
{
  afAddrInfo_t          addrInfo;                 /* IN: */
  zclCmdGroup_AddGroupIfIdentifying_t  cmdFrame;  /* IN: command frame */
} zclGroupAddGroupIfIdentifyingReq_t;

/* responses to above commands */
#define gZclCmdGroup_AddGroupRsp_c                       0x00 /* M - AddGroup */
#define gZclCmdGroup_ViewGroupRsp_c                      0x01 /* M - ViewGroup */
#define gZclCmdGroup_GetGroupMembershipRsp_c             0x02 /* M - GetGroupMembership */
#define gZclCmdGroup_RemoveGroupRsp_c                    0x03 /* M - RemoveGroup */

/* 3.6.2.3.1 Add Group Response command */
typedef struct zclCmdGroup_AddGroupRsp_tag
{
  zclStatus_t    status;
  zbGroupId_t   aGroupId;
} zclCmdGroup_AddGroupRsp_t;

/* 3.6.2.3.2 View Group Response command */
typedef struct zclCmdGroup_ViewGroupRsp_tag
{
  zclStatus_t    status;
  zbGroupId_t   aGroupId;
  char          szName[1];
} zclCmdGroup_ViewGroupRsp_t; 

/* 0x00 - 0xfd is the # of groups available */
typedef uint8_t haCapacity_t;
#define gZclCapacityNotFull_c   0xfe /* at least 1 entry available */
#define gZclCapacityFull_c         0xff

/* 3.6.2.3.3 Get Group Membership Response command */
typedef struct zclCmdGroup_GetGroupMembershipRsp_tag
{
  haCapacity_t  capacity;     /* # of groups total */
  uint8_t       count;        /*  */
  zbGroupId_t   aGroupId[1];
} zclCmdGroup_GetGroupMembershipRsp_t;

/* 3.6.2.3.4 Remove Group Response command */
typedef struct zclCmdGroup_RemoveGroupRsp_tag
{
  zclStatus_t    status;
  zbGroupId_t   aGroupId;
} zclCmdGroup_RemoveGroupRsp_t;



/******************************************
	Scenes Cluster
*******************************************/
#if ( TRUE == gBigEndian_c )
/* scene attributes */
#define gZclAttrScene_SceneCount_c         0x0000 /* rdOnly M - SceneCount */
#define gZclAttrScene_CurrentScene_c       0x0100 /* rdOnly M - CurrentScene */
#define gZclAttrScene_CurrentGroup_c       0x0200 /* rdOnly M - CurrentGroup */
#define gZclAttrScene_SceneValid_c         0x0300 /* rdOnly M - SceneValid */
#define gZclAttrScene_NameSupport_c        0x0400 /* rdOnly M - Scene Name Supported */
#define gZclAttrScene_LastConfiguredBy_c   0x0500 /* rdOnly O - LastConfiguredBy IEEE Address */
#else
#define gZclAttrScene_SceneCount_c         0x0000 /* rdOnly M - SceneCount */
#define gZclAttrScene_CurrentScene_c       0x0001 /* rdOnly M - CurrentScene */
#define gZclAttrScene_CurrentGroup_c       0x0002 /* rdOnly M - CurrentGroup */
#define gZclAttrScene_SceneValid_c         0x0003 /* rdOnly M - SceneValid */
#define gZclAttrScene_NameSupport_c        0x0004 /* rdOnly M - Scene Name Supported */
#define gZclAttrScene_LastConfiguredBy_c   0x0005 /* rdOnly O - LastConfiguredBy IEEE Address */
#endif /* #if ( TRUE == gBigEndian_c ) */

/* scene commands */
#define gZclCmdScene_AddScene_c            0x00 /* M - AddScene */
#define gZclCmdScene_ViewScene_c           0x01 /* M - ViewScene */
#define gZclCmdScene_RemoveScene_c         0x02 /* M - RemoveScene */
#define gZclCmdScene_RemoveAllScenes_c     0x03 /* M - RemoveAllScenes */
#define gZclCmdScene_StoreScene_c          0x04 /* M - StoreScene */
#define gZclCmdScene_RecallScene_c         0x05 /* M - RecallScene */
#define gZclCmdScene_GetSceneMembership_c  0x06 /* M - GetSceneMembership */

/* scene data for each cluster */
typedef struct zclSceneOtaData_tag
{
  zclClusterId_t    clusterId;
  uint8_t           length;
  uint8_t           aData[1];
} zclSceneOtaData_t;

/* 3.7.2.4.1 Add Scene command */
typedef struct zclCmdScene_AddScene_tag
{
  zbGroupId_t       aGroupId;         /* group ID */
  zclSceneId_t      sceneId;          /* scene ID */
  uint16_t          transitionTime;   /* in seconds */
  char              szSceneName[17];  /* length-encoded string */
  zclSceneOtaData_t aData[1];         /* variable scene length data */
} zclCmdScene_AddScene_t;

/* 3.7.2.4.2 View Scene command */
typedef struct zclCmdScene_ViewScene_tag
{
  zbGroupId_t       aGroupId;         /* group ID */
  zclSceneId_t      sceneId;          /* scene ID */
} zclCmdScene_ViewScene_t;


/* 3.7.2.4.3 Remove Scene command */
typedef struct zclCmdScene_RemoveScene_tag
{
  zbGroupId_t       aGroupId;         /* group ID */
  zclSceneId_t      sceneId;          /* scene ID */
} zclCmdScene_RemoveScene_t;

/* 3.7.2.4.4 Remove All Scenes command */
/* removes all scenes on this group */
typedef struct zclCmdScene_RemoveAllScenes_tag
{
  zbGroupId_t       aGroupId;         /* group ID */
} zclCmdScene_RemoveAllScenes_t;

/* 3.7.2.4.5 Store Scene command */
/* stores current state into a scene variable */
typedef struct zclCmdScene_StoreScene_tag
{
  zbGroupId_t       aGroupId;         /* group ID */
  zclSceneId_t      sceneId;          /* scene ID */
} zclCmdScene_StoreScene_t;

/* 3.7.2.4.6 Recall Scene command */
/* recall a stored or added scene */
typedef struct zclCmdScene_RecallScene_tag
{
  zbGroupId_t       aGroupId;         /* group ID */
  zclSceneId_t      sceneId;          /* scene ID */
} zclCmdScene_RecallScene_t;


/* 3.7.2.4.7 Get Scene Membership command */
typedef struct zclCmdScene_GetSceneMembership_tag
{
  zbGroupId_t       aGroupId;         /* group ID */
} zclCmdScene_GetSceneMembership_t;

/* add scene request */
typedef struct zclSceneAddSceneReq_tag
{
  afAddrInfo_t            addrInfo;   /* IN: */
  zclCmdScene_AddScene_t  cmdFrame;   /* IN: command frame */
} zclSceneAddSceneReq_t;

/* view scene request */
typedef struct zclSceneViewSceneReq_tag
{
  afAddrInfo_t            addrInfo;   /* IN: */
  zclCmdScene_ViewScene_t  cmdFrame;   /* IN: command frame */
} zclSceneViewSceneReq_t;

/* remove scene request */
typedef struct zclSceneRemoveSceneReq_tag
{
  afAddrInfo_t            addrInfo;     /* IN: */
  zclCmdScene_RemoveScene_t  cmdFrame;   /* IN: command frame */
} zclSceneRemoveSceneReq_t;

/* remove all scenes request */
typedef struct zclSceneRemoveAllScenesReq_tag
{
  afAddrInfo_t                  addrInfo;   /* IN: */
  zclCmdScene_RemoveAllScenes_t  cmdFrame;   /* IN: command frame */
} zclSceneRemoveAllScenesReq_t;

/* store scene request */
typedef struct zclSceneStoreSceneReq_tag
{
  afAddrInfo_t             addrInfo;   /* IN: */
  zclCmdScene_StoreScene_t  cmdFrame;   /* IN: command frame */
} zclSceneStoreSceneReq_t;

/* recall scene request */
typedef struct zclSceneRecallSceneReq_tag
{
  afAddrInfo_t              addrInfo;   /* IN: */
  zclCmdScene_RecallScene_t  cmdFrame;   /* IN: command frame */
} zclSceneRecallSceneReq_t;

/* get scene membership request */
typedef struct zclSceneGetSceneMembershipReq_tag
{
  afAddrInfo_t                    addrInfo;   /* IN: */
  zclCmdScene_GetSceneMembership_t cmdFrame;   /* IN: command frame */
} zclSceneGetSceneMembershipReq_t;

/* combination of all scene commands (so a single pointer can be used) */
typedef union zclSceneReq_tag {
  zclCmdScene_AddScene_t            addScene;
  zclCmdScene_ViewScene_t           viewScene;
  zclCmdScene_RemoveScene_t         removeScene;
  zclCmdScene_RemoveAllScenes_t     removeAllScenes;
  zclCmdScene_StoreScene_t          storeScene;
  zclCmdScene_RecallScene_t         recallScene;
  zclCmdScene_GetSceneMembership_t  getSceneMembership;
} zclSceneReq_t;

/* responses to above commands */
#define gZclCmdScene_AddSceneRsp_c           0x00 /* M - AddSceneRsp */
#define gZclCmdScene_ViewSceneRsp_c          0x01 /* ViewSceneRsp */
#define gZclCmdScene_RemoveSceneRsp_c        0x02 /* RemoveSceneRsp */
#define gZclCmdScene_RemoveAllScenesRsp_c    0x03 /* RemoveAllScenesRsp */
#define gZclCmdScene_StoreSceneRsp_c         0x04 /* StoreSceneRsp */
/* no response for recall scene */
#define gZclCmdScene_GetSceneMembershipRsp_c 0x06 /* GetSceneMembershipRsp */

/* 3.7.2.5.1 Add Scene Response command */
typedef struct zclCmdScene_AddSceneRsp_tag
{
  zclStatus_t   status;
  zbGroupId_t   aGroupId;
  zclSceneId_t  sceneId;
} zclCmdScene_AddSceneRsp_t;

/* 3.7.2.5.2 View Scene Response command */
typedef struct zclCmdScene_ViewSceneRsp_tag
{
  zbGroupId_t       aGroupId;           /* group ID */
  zclSceneId_t      sceneId;           /* scene ID */
  char              szSceneName[17];    /* scene name */
  uint16_t          transitionTime;     /* in seconds */
  zclSceneOtaData_t aData[1];          /* variable scene length data */
} zclCmdScene_ViewSceneRsp_t;

/* 3.7.2.5.3 Remove Scene Response command */
typedef struct zclCmdScene_RemoveSceneRsp_tag
{
  zclStatus_t   status;
  zbGroupId_t   aGroupId;
  zclSceneId_t  sceneId;
} zclCmdScene_RemoveSceneRsp_t;

/* 3.7.2.5.4 Remove All Scenes Response command */
typedef struct zclCmdScene_RemoveAllScenesRsp_tag
{
  zclStatus_t   status;
  zbGroupId_t   aGroupId;
} zclCmdScene_RemoveAllScenesRsp_t;

/* 3.7.2.5.5 Store Scene Response command */
typedef struct zclCmdScene_StoreSceneRsp_tag
{
  zclStatus_t   status;
  zbGroupId_t   aGroupId;
  zclSceneId_t  sceneId;
} zclCmdScene_StoreSceneRsp_t;

/* 3.7.2.5.6 Get Scene Membership Response command */
typedef struct zclCmdScene_GetSceneMembershipRsp_tag
{
  zclStatus_t   status;          /* status */
  haCapacity_t  capacity;         /* capacity for scenes */
  zbGroupId_t   aGroupId;         /* group */
  uint8_t       sceneCount;       /* scenes that belong to the group */
  zclSceneId_t  scenes[1];       /* list of scenes */
} zclCmdScene_GetSceneMembershipRsp_t;


typedef union zclSceneRsp_tag {
  zclCmdScene_AddSceneRsp_t           addSceneRsp;
  zclCmdScene_ViewSceneRsp_t          viewSceneRsp;
  zclCmdScene_RemoveSceneRsp_t        removeSceneRsp;
  zclCmdScene_RemoveAllScenesRsp_t    removeAllScenesRsp;
  zclCmdScene_StoreSceneRsp_t         storeSceneRsp;
  zclCmdScene_GetSceneMembershipRsp_t getSceneMembershipRsp;
} zclSceneRsp_t;



/* Scene data structures */

/* Application specific scene structure always "inherits" the zclSceneTable structure */

/* Thermostat scene data */
typedef struct zclThermostatSceneTableEntry_tag
{
  /* General scene data */
  zclSceneTableEntry_t sceneTableEntry;

  /* Thermostat specific scene data */
  int16_t occupiedCoolingSetpoint;
  int16_t occupiedHeatingSetpoint;
  uint8_t systemMode;
} zclThermostatSceneTableEntry_t;

typedef struct zclThermostatSceneData_tag
{
  zclSceneAttrs_t scene;  
  zclThermostatSceneTableEntry_t thermostatSceneTable[gHaMaxScenes_c];
} zclThermostatSceneData_t;


/* OnOff Light scene data */
typedef struct zclOnOffLightSceneTableEntry_tag
{
  /* General scene data */
  zclSceneTableEntry_t sceneTableEntry;

  /* On/Off Light specific scene data */
  uint8_t onOff;
} zclOnOffLightSceneTableEntry_t;

typedef struct zclOnOffLightSceneData_tag
{
  zclSceneAttrs_t scene;
  zclOnOffLightSceneTableEntry_t onOffLightSceneTable[gHaMaxScenes_c];
} zclOnOffLightSceneData_t;

/* DoorLock scene data */
typedef struct zclDoorLockSceneTableEntry_tag
{
  /* General scene data */
  zclSceneTableEntry_t sceneTableEntry;

  /*Door Lock specific scene data */
  uint8_t lockState;
} zclDoorLockSceneTableEntry_t;

typedef struct zclDoorLockSceneData_tag
{
  zclSceneAttrs_t scene;
  zclDoorLockSceneTableEntry_t doorLockSceneTable[gHaMaxScenes_c];
} zclDoorLockSceneData_t;

/* Dimmable Light (no OnOff feature) scene data */
typedef struct zclDimmableLightSceneTableEntry_tag
{
  /* General scene data */
  zclSceneTableEntry_t sceneTableEntry;

  /* Dimmable Light specific scene data */
  uint8_t currentLevel;  
} zclDimmableLightSceneTableEntry_t;

typedef struct zclDimmableLightSceneData_tag
{
  zclSceneAttrs_t scene;
  zclDimmableLightSceneTableEntry_t dimmableLightSceneTable[gHaMaxScenes_c];
} zclDimmableLightSceneData_t;


/* Dimmable Light with OnOff scene data */
typedef struct zclDimmableLightOnOffSceneTableEntry_tag
{
  /* General scene data */
  zclSceneTableEntry_t sceneTableEntry;

  /* Dimmable Light specific scene data */
  uint8_t onOff;
  uint8_t currentLevel;  
} zclDimmableLightOnOffSceneTableEntry_t;

typedef struct zclDimmableLightOnOffSceneData_tag
{
  zclSceneAttrs_t scene;
  zclDimmableLightOnOffSceneTableEntry_t onOffSceneTable[gHaMaxScenes_c];
} zclDimmableLightOnOffSceneData_t;

/* Shade with OnOff scene data */
typedef struct zclShadeOnOffSceneTableEntry_tag
{
  /* General scene data */
  zclSceneTableEntry_t sceneTableEntry;

  /* Shade specific scene data */
  uint8_t onOff;
  uint8_t currentLevel;  
} zclShadeOnOffSceneTableEntry_t;

typedef struct zclShadeOnOffSceneData_tag
{
  zclSceneAttrs_t scene;
  zclDimmableLightOnOffSceneTableEntry_t onOffSceneTable[gHaMaxScenes_c];
} zclShadeOnOffSceneData_t;

/******************************************
	On/Off Cluster
*******************************************/
/* on/off cluster attributes */
#define gZclAttrOnOff_OnOffId_c    0x0000  /* M - is the device on or off? */

typedef struct zclOnOffAttrsRAM_tag
{
  uint8_t  onOff[zclReportableCopies_c];
} zclOnOffAttrsRAM_t;

/* on/off cluster commands */
typedef struct zclCmdOnOffSetState_tag
{
  zclCmd_t command;                /* on/off commands */
} zclCmdOnOffSetState_t;
typedef zclCmd_t zclOnOffCmd_t;
#define gZclCmdOnOff_Off_c         0x00    /* M - turn device off */
#define gZclCmdOnOff_On_c          0x01    /* M - turn device on */
#define gZclCmdOnOff_Toggle_c      0x02    /* M - toggle device */

/* note: on/off/toggle commands have no payload */
typedef struct zclOnOffReq_tag
{
  afAddrInfo_t    addrInfo;     /* IN: source/dst address, cluster, etc... */
  zclOnOffCmd_t   command;      /* IN: on, off, toggle */
} zclOnOffReq_t;

/* sends on/off/toggle command */
typedef struct zclOnOffSetState_tag
{
  afAddrInfo_t            addrInfo;   /* IN: */
  zclCmdOnOffSetState_t    cmdFrame;  /* IN: handles all 3 commands, on/off/toggle */
} zclOnOffSetState_t;

/******************************************
	On/Off Switch Configuration Cluster
*******************************************/
#if ( TRUE == gBigEndian_c )
/* on/off switch configuration cluster information attributes */
#define gZclAttrOnOffSwitchCfg_SwitchType_c               0x0000  /* M - SwitchType */ 
/* on/off switch configurarion cluster setting attributes */
#define gZclAttrOnOffSwitchCfg_SwitchAction_c             0x1000 /* M - Switch action */
#else
/* on/off switch configuration cluster information attributes */
#define gZclAttrOnOffSwitchCfg_SwitchType_c               0x0000  /* M - SwitchType */ 
/* on/off switch configurarion cluster setting attributes */
#define gZclAttrOnOffSwitchCfg_SwitchAction_c             0x0010 /* M - Switch action */
#endif /* #if ( TRUE == gBigEndian_c ) */

typedef uint8_t  geZclSwitchType_t;
/* on/off switch configuration Switch type attr values */
enum {
    geZclSwitchType_ToggleSw_c = 0x00, /* Toggle Switch Type */
    geZclSwitchType_Momentary_c =0x01/* Momentary Switch type */
};

typedef uint8_t  geZclSwitchAction_t;
/* on/off switch configuration Switch Acction Attr values */
enum {
     geZclSwitchAction_Off_c = 0x00,
     geZclSwitchAction_On_c = 0x01,
     geZclSwitchAction_Toggle_c = 0x02
};

typedef struct zclOnOffSwitchCfgAttrsRAM_tag{
   geZclSwitchType_t     SwitchType;    /* Switch type is an enumeration of the diferent switch types */
   geZclSwitchAction_t   SwitchAction;  /* Switch Action is an enumeration of the diferent switch action*/
} zclOnOffSwitchCfgAttrsRAM_t;

/* on/off switch configuration cluster commands */

/* No commands are generated by the client, except those 
    to read and write the attributes of the server. */

/******************************************
	Level Control Cluster
*******************************************/
#if ( TRUE == gBigEndian_c )
/* level control cluster attributes */
#define gZclAttrLevelControl_CurrentLevelId_c        0x0000 /* M - CurrentLevel */
#define gZclAttrLevelControl_RemainingTimeId_c       0x0100 /* O - RemainingTime */
#define gZclAttrLevelControl_OnOffTransitionTimeId_c 0x1000 /* O - OnOffTransitionTime */
#define gZclAttrLevelControl_OnLevelId_c             0x1100 /* O - OnLevel */
#else
/* level control cluster attributes */
#define gZclAttrLevelControl_CurrentLevelId_c        0x0000 /* M - CurrentLevel */
#define gZclAttrLevelControl_RemainingTimeId_c       0x0001 /* O - RemainingTime */
#define gZclAttrLevelControl_OnOffTransitionTimeId_c 0x0010 /* O - OnOffTransitionTime */
#define gZclAttrLevelControl_OnLevelId_c             0x0011 /* O - OnLevel */
#endif /* #if ( TRUE == gBigEndian_c ) */

/* level control cluster commands */
#define gZclCmdLevelControl_MoveToLevel_c        0x00 /* M - Move to Level */
#define gZclCmdLevelControl_Move_c               0x01 /* M - Move */
#define gZclCmdLevelControl_Step_c               0x02 /* M - Step */
#define gZclCmdLevelControl_Stop_c               0x03 /* M - Stop */
#define gZclCmdLevelControl_MoveToLevelOnOff_c   0x04 /* M - Move to Level (with On/Off) */
#define gZclCmdLevelControl_MoveOnOff_c          0x05 /* M - Move (with On/Off) */
#define gZclCmdLevelControl_StepOnOff_c          0x06 /* M - Step (with On/Off) */
#define gZclCmdLevelControl_StopOnOff_c          0x07 /* M - Stop (same as above, repeated for symmetry) */

typedef uint8_t zclMoveMode_t;
#define zclMoveMode_Up           0x00 /* Up */
#define zclMoveMode_Down         0x01 /* Down */

/* 3.10.2.3.1/3.10.2.3.5 Move to Level Commands */
typedef struct zclCmdLevelControl_MoveToLevel_tag
{
  uint8_t  level;
  uint16_t transitionTime; /* in seconds */
} zclCmdLevelControl_MoveToLevel_t;

/* 3.10.2.3.2/3.10.2.3.6 Move Commands */
typedef struct zclCmdLevelControl_Move_tag
{
  zclMoveMode_t moveMode;
  uint8_t       rate;   /* in steps per second */
} zclCmdLevelControl_Move_t;

/* 3.10.2.3.3/3.10.2.3.7 Step Commands */
typedef struct zclCmdLevelControl_Step_tag
{
  zclMoveMode_t   stepMode;
  uint8_t         stepSize;         /* amount to step */
  uint16_t        transitionTime;   /* time in 1/10th second */
} zclCmdLevelControl_Step_t;

/* 3.10.2.3.4/3.10.2.3.8 Stop Commands */
typedef struct zclCmdLevelControl_Stop_tag
{
  uint8_t stopCmd;                  
} zclCmdLevelControl_Stop_t;        /* dummy struct, no actual payload */

/* level control move to level requests */
typedef struct zclLevelControlMoveToLevel_tag
{
  afAddrInfo_t                      addrInfo;  /* IN: */
  zclCmdLevelControl_MoveToLevel_t   cmdFrame;  /* command frame */
} zclLevelControlMoveToLevel_t;

/* level control move requests */
typedef struct zclLevelControlMove_tag
{
  afAddrInfo_t               addrInfo;  /* IN: */
  zclCmdLevelControl_Move_t   cmdFrame;  /* IN: command frame */
} zclLevelControlMove_t;

/* level control move requests */
typedef struct zclLevelControlStep_tag
{
  afAddrInfo_t               addrInfo;  /* IN: */
  zclCmdLevelControl_Step_t  cmdFrame;  /* IN: command frame */
} zclLevelControlStep_t;


/* level control stop requests */
typedef struct zclLevelControlStop_tag
{
  afAddrInfo_t               addrInfo;  /* IN: */
  zclCmdLevelControl_Stop_t  cmdFrame;
} zclLevelControlStop_t;


typedef union zclLevelControlReq_tag 
{
  zclLevelControlMoveToLevel_t moveToLevelReq;
  zclLevelControlMove_t moveReq;
  zclLevelControlStep_t stepReq;
  zclLevelControlStop_t stopReq;
  zclLevelControlMoveToLevel_t moveToLevelOnOffReq;
  zclLevelControlMove_t moveOnOffReq;
  zclLevelControlStep_t stepOnOffReq;
  zclLevelControlStop_t stopOnOffReq;
} zclLevelControlReq_t; 

typedef union zclLevelControlCmd_tag 
{
  zclCmdLevelControl_MoveToLevel_t MoveToLevelCmd;
  zclCmdLevelControl_Move_t MoveCmd;
  zclCmdLevelControl_Step_t StepCmd;
  zclCmdLevelControl_Stop_t StopCmd;
} zclLevelControlCmd_t; 


typedef uint16_t zbLevelCtrlTime_t;

/* Level values */
typedef uint8_t zclLevelValue_t;
#define  gZclLevel_off    0x00
#define  gZclLevel_LowestPossible   (gZclLevel_off + 1)
#define  gZclLevel_low    gZclLevelDelta - 1
#define  gZclLevel_medium (gZclLevelDelta*2) - 1
#define  gZclLevel_high   gZclLevelMaxValue
#define  gZclLevel_on     gZclLevelMaxValue

typedef struct zclLevelCtrlAttrsRAM_tag
{
  zclLevelValue_t     currentLevel[zclReportableCopies_c];   /* Current level off the light */
#if gZclClusterOptionals_d
  zbLevelCtrlTime_t   remainingTime;  /* Remaining time to complete the current command 1/10ths of a sec */
  zbLevelCtrlTime_t   onOffTransitionTime; /* Time taken to move to of from the target level when switched on of off */
  zclLevelValue_t     onLevel;        /* current level is set to this value when turn on the switch */
#endif
} zclLevelCtrlAttrsRAM_t;

/*************************************************************************
*               Commissioning cluster definitions
*              (064699r12  Tables 5, 12, 13 and 14)
**************************************************************************/
#if (gBigEndian_c)
/* Attributes Identifier for Startup Parameters  attribute set */
#define gZclAttrCommissioning_ShortAddressID_c                 0x0000
#define gZclAttrCommissioning_ExtendedPANIdID_c                0x0100
#define gZclAttrCommissioning_PanIdID_c                        0x0200
#define gZclAttrCommissioning_ChannelMaskID_c                  0x0300
#define gZclAttrCommissioning_ProtocolVersionID_c              0x0400
#define gZclAttrCommissioning_StackProfileID_c                 0x0500
#define gZclAttrCommissioning_StartupControlID_c               0x0600
#define gZclAttrCommissioning_TrustCenterAddressID_c           0x1000
#define gZclAttrCommissioning_TrustCenterMasterKeyID_c         0x1100
#define gZclAttrCommissioning_NetworkKeyID_c                   0x1200
#define gZclAttrCommissioning_UseInsecureJoinID_c              0x1300
#define gZclAttrCommissioning_PreconfiguredLinkKeyID_c         0x1400
#define gZclAttrCommissioning_NetworkKeySeqNumID_c             0x1500
#define gZclAttrCommissioning_NetworkKeyTypeID_c               0x1600
#define gZclAttrCommissioning_NetworkManagerAddressID_c        0x1700

/* Attributes Identifier for Join Parameters attribute set*/
#define gZclAttrCommissioning_ScanAttemptsID_c                 0x2000
#define gZclAttrCommissioning_TimeBetweenScansID_c             0x2100
#define gZclAttrCommissioning_RejoinIntervalID_c               0x2200
#define gZclAttrCommissioning_MaxRejoinIntervalID_c            0x2300

/* Attributes Identifier for End Device Parameters attribute set*/
#define gZclAttrCommissioning_IndirectPollRateID_c             0x3000
#define gZclAttrCommissioning_ParentRetryThreshoID_c           0x3100
                                                            
/* Attributes Identifier for Concentrator Parameters  attribute set*/
#define gZclAttrCommissioning_ConcentratorFlagID_c             0x4000
#define gZclAttrCommissioning_ConcentratorRadiusID_c           0x4100
#define gZclAttrCommissioning_ConcentratorDiscoveryTimeID_c    0x4200

#else /* #if (gBigEndian_c) */

/* Attributes Identifier for Startup Parameters  attribute set */
#define gZclAttrCommissioning_ShortAddressID_c                 0x0000
#define gZclAttrCommissioning_ExtendedPANIdID_c                0x0001
#define gZclAttrCommissioning_PanIdID_c                        0x0002
#define gZclAttrCommissioning_ChannelMaskID_c                  0x0003
#define gZclAttrCommissioning_ProtocolVersionID_c              0x0004
#define gZclAttrCommissioning_StackProfileID_c                 0x0005
#define gZclAttrCommissioning_StartupControlID_c               0x0006
#define gZclAttrCommissioning_TrustCenterAddressID_c           0x0010
#define gZclAttrCommissioning_TrustCenterMasterKeyID_c         0x0011
#define gZclAttrCommissioning_NetworkKeyID_c                   0x0012
#define gZclAttrCommissioning_UseInsecureJoinID_c              0x0013
#define gZclAttrCommissioning_PreconfiguredLinkKeyID_c         0x0014
#define gZclAttrCommissioning_NetworkKeySeqNumID_c             0x0015
#define gZclAttrCommissioning_NetworkKeyTypeID_c               0x0016
#define gZclAttrCommissioning_NetworkManagerAddressID_c        0x0017

/* Attributes Identifier for Join Parameters attribute set*/
#define gZclAttrCommissioning_ScanAttemptsID_c                 0x0020
#define gZclAttrCommissioning_TimeBetweenScansID_c             0x0021
#define gZclAttrCommissioning_RejoinIntervalID_c               0x0022
#define gZclAttrCommissioning_MaxRejoinIntervalID_c            0x0023

/* Attributes Identifier for End Device Parameters attribute set*/
#define gZclAttrCommissioning_IndirectPollRateID_c             0x0030
#define gZclAttrCommissioning_ParentRetryThreshoID_c           0x0031
                                                            
/* Attributes Identifier for Concentrator Parameters  attribute set*/
#define gZclAttrCommissioning_ConcentratorFlagID_c             0x0040
#define gZclAttrCommissioning_ConcentratorRadiusID_c           0x0041
#define gZclAttrCommissioning_ConcentratorDiscoveryTimeID_c    0x0042

#endif /* #if (gBigEndian_c) */


/* StartupControl attribute usage  */ 
enum
{
  gIsPartofNetwork_c     = 0x00, /*the device should consider itself part of the network indicated by the ExtendedPANId */
  gFormWithExtPanId_c    = 0x01, /* form a network with extended PAN ID */
  gRejoinWithExtPanId_c  = 0x02, /* the device should rejoin the network with extended PAN ID given by the ExtendedPANId attribute */
  gStartFromScratch_c    = 0x03  /* start "from scratch" and join the network using (unsecured) MAC association */
};

/*(064699r12  Table 15 - Commands Received by the Commissioning Cluster Server)*/
#define gZclCmdCommissiong_RestartDeviceRequest_c               0x00  /*M*/
#define gZclCmdCommissiong_SaveStartupParametersRequest_c       0x01  /*O*/
#define gZclCmdCommissiong_RestoreStartupParametersRequest_c    0x02  /*O*/
#define gZclCmdCommissiong_ResetStartupParametersRequest_c      0x03  /*M*/

/*(064699r12  Table 17 - Commands Generated by the Commissioning Cluster Server)*/
#define gZclCmdCommissiong_RestartDeviceResponse_c              0x00  /*M*/
#define gZclCmdCommissiong_SaveStartupParametersResponse_c      0x01  /*M*/
#define gZclCmdCommissiong_RestoreStartupParametersResponse_c   0x02  /*M*/
#define gZclCmdCommissiong_ResetStartupParametersResponse_c     0x03  /*M*/


typedef struct zclCmdCommissiong_RestartDeviceRequest_tag
{
  uint8_t   Options;       /* Startup mode, immediate subfield*/
  uint8_t   Delay;         /* Startup delay in seconds */
  uint8_t   Jitter;        /* Random jitter range in milliseconds */
} zclCmdCommissiong_RestartDeviceRequest_t;

#define gCommisioningClusterRestartDeviceRequestOptions_StartUpModeSubField_Mask_c      0x03
#define gCommisioningClusterRestartDeviceRequestOptions_ImmediateSubField_Mask_c        0x04


#define gCommisioningClusterRestartDeviceRequestOptions_ImmediateSubField_RestartImmediately_c   0x00
#define gCommisioningClusterRestartDeviceRequestOptions_ImmediateSubField_Restartconveniently_c  0x04

#define gCommisioningClusterRestartDeviceRequestOptions_ModeSubField_RestartUsingCurrentStartupParameters_c  0x00
#define gCommisioningClusterRestartDeviceRequestOptions_ModeSubField_RestartUsingCurrentStackParameter_c     0x01


typedef struct zclCmdCommissiong_SaveStartupParametersRequest_tag
{
  uint8_t   Options;       /* Reserved */
  uint8_t   Index;         /* Startup Attribute Set index */
} zclCmdCommissiong_SaveStartupParametersRequest_t;

typedef struct zclCmdCommissiong_RestoreStartupParametersRequest_tag
{
  uint8_t   Options;       /* Reserved */
  uint8_t   Index;         /* Startup Attribute Set index */
} zclCmdCommissiong_RestoreStartupParametersRequest_t;


typedef struct zclCmdCommissiong_ResetStartupParametersRequest_tag
{
  uint8_t   Options;       /* Reserved */
  uint8_t   Index;         /* Startup Attribute Set index */
} zclCmdCommissiong_ResetStartupParametersRequest_t;

#define gCommisioningClusterResetStartupParametersRequestOptions_ResetCurrentSubField_Mask_c      0x01
#define gCommisioningClusterResetStartupParametersRequestOptions_ResetAllSubField_Mask_c          0x02
#define gCommisioningClusterResetStartupParametersRequestOptions_EraseIndexSubField_Mask_c        0x04

typedef union zclCommissioningCmd_tag 
{
  zclCmdCommissiong_RestartDeviceRequest_t RestartDeviceCmd;
  zclCmdCommissiong_SaveStartupParametersRequest_t    SaveStartupParameterCmd;
  zclCmdCommissiong_RestoreStartupParametersRequest_t RestoreStartupParameterCmd;
  zclCmdCommissiong_ResetStartupParametersRequest_t   ResetStartupParameterCmd;
} zclCommissioningCmd_t;

/*
All commands have the same type of response so there is only defined one
type def.
*/

typedef struct zclCmdCommissiong_response_tag
{
  uint8_t   status;         /* response status */
} zclCmdCommissiong_response_t;


/* Commisioning Response format:
   All responses uses the same format as a Default ZCL response defined by:
   zclCmdDefaultRsp_t
 */

typedef struct  zclCommissioningRestartDeviceReq_tag 
{  
  afAddrInfo_t addrInfo; 
  zclCmdCommissiong_RestartDeviceRequest_t cmdFrame;
} zclCommissioningRestartDeviceReq_t;

typedef struct  zclCommissioningSaveStartupParametersReq_tag 
{  
  afAddrInfo_t addrInfo; 
  zclCmdCommissiong_SaveStartupParametersRequest_t cmdFrame;
} zclCommissioningSaveStartupParametersReq_t;

typedef struct  zclCommissioningRestoreStartupParametersReq_tag 
{  
  afAddrInfo_t addrInfo; 
  zclCmdCommissiong_RestoreStartupParametersRequest_t cmdFrame;
} zclCommissioningRestoreStartupParametersReq_t;

typedef struct  zclCommissioningResetStartupParametersReq_tag 
{  
  afAddrInfo_t addrInfo; 
  zclCmdCommissiong_ResetStartupParametersRequest_t cmdFrame;
} zclCommissioningResetStartupParametersReq_t;


extern const zclAttrDefList_t gZclCommissioningServerClusterAttrDefList;
extern zbCommissioningAttributes_t gCommisioningServerAttrsData;

/*******************************************************
	Alarms Cluster (ZigBee Cluster Library Chapter 3.11)
*******************************************************/

#if ( TRUE == gBigEndian_c )
/* Alarms cluster attributes set */
#define gZclAttrAlarms_AlarmInformation_c        0x0000 /* Alarm Information */
#else
/* Alarms cluster attributes set */
#define gZclAttrAlarms_AlarmInformation_c        0x0000 /* Alarm Information */
#endif /* #if ( TRUE == gBigEndian_c ) */

/* 3.11.2.2.1 Alarm Information Attributes Set */

#if ( TRUE == gBigEndian_c )
/* alarms cluster attributes */
#define  gZclAlarmInformation_AlarmCount_c           0x0000 /* 0 - AlarmCount */
#else
/* alarms cluster attributes */
#define  gZclAlarmInformation_AlarmCount_c           0x0000 /* 0 - AlarmCount */
#endif /* #if ( TRUE == gBigEndian_c ) */

/* 3.11.2.3.1 Alarm Table */

typedef struct zclAlarmsAttrsRAM_tag
{
  uint16_t count;
}zclAlarmsAttrsRAM_t;

typedef struct gZclAlarmTable_tag
{            
    uint8_t     AlarmCode; 
    uint16_t    ClusterID;
    uint32_t    TimeStamp;                                      
}gZclAlarmTable_t; 

#define MaxAlarmsPermitted 0x03 

/* 3.11.2.4 Commands Received */
enum
{
   gAlarmClusterRxCommandID_ResetAlarm_c = 0x00,   /* M - Reset Alarm */
   gAlarmClusterRxCommandID_ResetAllAlarms_c = 0x01, /* M - Reset All alarms */
   gAlarmClusterRxCommandID_GetAlarm_c,            /* O - Get Alarm */
   gAlarmClusterRxCommandID_ResetAlarmLog_c,       /* O - Reset Alarm Log */
                                                 /* 0x04-0xFF Reserved */
};


/* Reset Alarm Payload */
typedef struct zclCmdAlarmInformation_ResetAlarm_tag 
{
   uint8_t   AlarmCode;
   uint16_t  ClusterID;
}zclCmdAlarmInformation_ResetAlarm_t; 

typedef struct zclAlarmInformation_ResetAlarm_tag 
{
   afAddrInfo_t addrInfo;
   uint8_t zclTransactionId;
   zclCmdAlarmInformation_ResetAlarm_t cmdFrame;
}zclAlarmInformation_ResetAlarm_t; 

/* 3.11.2.5 Commands Generated */
enum
{
   gAlarmClusterTxCommandID_Alarm_c = 0x00,        /* M - Alarm */
   gAlarmClusterTxCommandID_GetAlarmResponse_c,    /* O - Get Alarm Response */
                                                 /* 0x02-0xFF Reserved */
};

/* for no payload command */
typedef struct zclAlarmInformation_NoPayload_tag 
{
   afAddrInfo_t addrInfo;
   uint8_t zclTransactionId;
}zclAlarmInformation_NoPayload_t; 


/* Alarm Payload */
typedef struct zclCmdAlarmInformation_Alarm_tag 
{
 uint8_t   AlarmCode;
 uint16_t  ClusterID;
}zclCmdAlarmInformation_Alarm_t; 

typedef struct zclAlarmInformation_Alarm_tag 
{
   afAddrInfo_t addrInfo;
   uint8_t zclTransactionId;
   zclCmdAlarmInformation_Alarm_t cmdFrame;
}zclAlarmInformation_Alarm_t; 

/* Get Alarm Response Payload */
typedef struct zclCmdAlarmInformation_GetAlarmResponse_tag 
{
 uint8_t   Status;
 uint8_t   AlarmCode;
 uint16_t  ClusterID;
 uint32_t   TimeStamp;
}zclCmdAlarmInformation_GetAlarmResponse_t;

typedef struct zclAlarms_GetAlarmResponse_tag 
{
   afAddrInfo_t addrInfo;
   uint8_t zclTransactionId;
   zclCmdAlarmInformation_GetAlarmResponse_t cmdFrame;
}zclAlarms_GetAlarmResponse_t;

/******************************
  Time Cluster
  See ZCL Specification (075123r02) Section 3.12
*******************************/
#if (gBigEndian_c)

#define gZclAttrTime_c            0x0000 /* M - Time */
#define gZclAttrTimeStatus_c      0x0100 /* M - Time Status */
#define gZclAttrTimeZone_c        0x0200 /* O - Time Zone */
#define gZclAttrDstStart_c        0x0300 /* O - DST Start */
#define gZclAttrDstEnd_c          0x0400 /* O - DST End */
#define gZclAttrDstShift_c        0x0500 /* O - DST Shift */
#define gZclAttrStandardTime_c    0x0600 /* O - Standard Time */
#define gZclAttrLocalTime_c       0x0700 /* O - Local Time */
#define gZclAttrLastSetTime_c     0x0800 /* O - Last Set Time */
#define gZclAttrValidUntilTime_c  0x0900 /* O - Valid Until Time */

#else /* #if (gBigEndian_c) */

#define gZclAttrTime_c            0x0000 /* M - Time */
#define gZclAttrTimeStatus_c      0x0001 /* M - Time Status */
#define gZclAttrTimeZone_c        0x0002 /* O - Time Zone */
#define gZclAttrDstStart_c        0x0003 /* O - DST Start */
#define gZclAttrDstEnd_c          0x0004 /* O - DST End */
#define gZclAttrDstShift_c        0x0005 /* O - DST Shift */
#define gZclAttrStandardTime_c    0x0006 /* O - Standard Time */
#define gZclAttrLocalTime_c       0x0007 /* O - Local Time */
#define gZclAttrLastSetTime_c     0x0008 /* O - Last Set Time */
#define gZclAttrValidUntilTime_c  0x0009 /* O - Valid Until Time */

#endif /* #if (gBigEndian_c) */

#define zclTimeStatusMaster         0x01
#define zclTimeStatusSynchronized   0x02
#define zclTimeStatusMasterZoneDst  0x04

/* gZclDataTypeUTCTime_c - note stored as little endian.*/
typedef uint32_t ZCLTime_t;

typedef struct ZCLTimeServerAttrsRAM_tag
{
   /*Attributes of the Time attribute set */
  ZCLTime_t   Time;
  uint8_t     TimeStatus;
  int32_t     TimeZone;
  uint32_t    DstStart;
  uint32_t    DstEnd;
  int32_t     DstShift;
  uint32_t    StandardTime;
  uint32_t    LocalTime;
  ZCLTime_t   LastSetTime;
  ZCLTime_t   ValidUntilTime;
} ZCLTimeServerAttrsRAM_t;

/* structure used to set the time in the Time cluster - note all data is little endian*/
typedef struct ZCLTimeConf_tag
{
  ZCLTime_t   Time;
  int32_t     TimeZone;
  uint32_t    DstStart;
  uint32_t    DstEnd;
  int32_t     DstShift;
  ZCLTime_t   ValidUntilTime;
} ZCLTimeConf_t;


extern const zclAttrDefList_t gZcltimeServerClusterAttrDefList;

/*Initialize the Time Cluster  - note input must be little endian*/
void ZCL_TimeInit(ZCLTimeConf_t *pTimeConf);

/*Set UTC Time from the backend - native endianess */
void ZCL_SetUTCTime(uint32_t time);

/*Get UTC Time from the backend - native endianess */
uint32_t ZCL_GetUTCTime(void);

/*Set TimeZone from the backend - native endianess */
void ZCL_SetTimeZone(int32_t timeZone);
/*Set Daylight saving interval from the backend - native endianess */
void ZCL_SetTimeDst(uint32_t DstStart, uint32_t DstEnd, int32_t DstShift);

/*************************************************************
	RSSI Location Cluster (ZigBee Cluster Library Chapter 3.13)
*************************************************************/

#if gBigEndian_c
#define gZclAttrRSSILocationInformationSet_c            0x0000
#define gZclAttrRSSILocationSettingsSet_c               0x0100
#else /* #if gBigEndian_c */
#define gZclAttrRSSILocationInformationSet_c            0x0000
#define gZclAttrRSSILocationSettingsSet_c               0x0001
#endif /* #if gBigEndian_c */


/* Location Information Attribute Set */
#if (TRUE == gBigEndian_c)
#define gZclAttrLocationInformation_LocationType_c         0x0000 /*M*/
#define gZclAttrLocationInformation_LocationMethod_c       0x0100 /*M*/
#define gZclAttrLocationInformation_LocationAge_c          0x0200 /*O*/
#define gZclAttrLocationInformation_QualityMeasure_c       0x0300 /*O*/
#define gZclAttrLocationInformation_NumberOfDevices_c      0x0400 /*O*/
#else
#define gZclAttrLocationInformation_LocationType_c         0x0000 /*M*/
#define gZclAttrLocationInformation_LocationMethod_c       0x0001 /*M*/
#define gZclAttrLocationInformation_LocationAge_c          0x0002 /*O*/
#define gZclAttrLocationInformation_QualityMeasure_c       0x0003 /*O*/
#define gZclAttrLocationInformation_NumberOfDevices_c      0x0004 /*O*/
#endif /* #if (TRUE == gBigEndian_c) */

/* Location Type Attribute */

typedef struct LocationType_tag
{
 unsigned   Absolute         : 1;
 unsigned   Dimension        : 1;
 unsigned   CoordinateSystem : 2;
 unsigned   Reserved         : 4;
}LocationType_t;

#define ABSOLUTE_LOCATION   1
#define MEASURED_LOCATION   0
#define TWO_DIMENSIONAL     1
#define THREE_DIMENSIONAL   0
#define RECTANGULAR         0

/* Location Method Attribute */

enum
{
  gLocationMethod_Lateration_c = 0,     /* Lateration */
  gLocationMethod_Signposting_c,        /* Signposting */
  gLocationMethod_RFfingerprinting_c,   /* RF Fingerprinting */
  gLocationMethod_OutOfBand_c,          /* Out of Band */
                                        /* 0x04-0x3F Reserved */
                                        /* 0x40-0xFF Reserved for manufacturer */
}; 

/* Location Settings Attribute Set */
#if (TRUE == gBigEndian_c)
#define gZclAttrLocationSettings_Coordinate1_c              0x1000 /*M*/
#define gZclAttrLocationSettings_Coordinate2_c              0x1100 /*M*/
#define gZclAttrLocationSettings_Coordinate3_c              0x1200 /*O*/
#define gZclAttrLocationSettings_Power_c                    0x1300 /*M*/
#define gZclAttrLocationSettings_PathLossExponent_c         0x1400 /*M*/
#define gZclAttrLocationSettings_ReportingPeriod_c          0x1500 /*O*/
#define gZclAttrLocationSettings_CalculationPeriodt_c       0x1600 /*O*/
#define gZclAttrLocationSettings_NumberRSSImeasurements_c   0x1700 /*M*/
#else
#define gZclAttrLocationSettings_Coordinate1_c              0x0010 /*M*/
#define gZclAttrLocationSettings_Coordinate2_c              0x0011 /*M*/
#define gZclAttrLocationSettings_Coordinate3_c              0x0012 /*O*/
#define gZclAttrLocationSettings_Power_c                    0x0013 /*M*/
#define gZclAttrLocationSettings_PathLossExponent_c         0x0014 /*M*/
#define gZclAttrLocationSettings_ReportingPeriod_c          0x0015 /*O*/
#define gZclAttrLocationSettings_CalculationPeriodt_c       0x0016 /*O*/
#define gZclAttrLocationSettings_NumberRSSImeasurements_c   0x0017 /*M*/
#endif /* #if (TRUE == gBigEndian_c) */

/* 3.13.2.3 RSSI Location server cluster Commands Received */
enum
{
  gZclCmdRxRSSI_SetAbsoluteLocation_c = 0,     /* M - Set Absolute Location */
  gZclCmdRxRSSI_SetDeviceConfiguration_c,      /* M - Set Device Configuration */
  gZclCmdRxRSSI_GetDeviceConfiguration_c,      /* M - Get Device Configuration */
  gZclCmdRxRSSI_GetLocationData_c,             /* M - Get Location Data */
                                               /* 0x04-0xFF Reserved */
}; 


typedef struct zclCmdRSSI_AbsoluteLocation_tag 
{
 int16_t  Coordinate1;
 int16_t  Coordinate2;
 int16_t  Coordinate3;
 int16_t  Power;
 uint16_t PathLossExponent;
}zclCmdRSSI_AbsoluteLocation_t; 

typedef struct zclCmdRSSI_SetDeviceConfiguration_tag 
{
 int16_t  Power;
 uint16_t PathLossExponent;
 uint16_t CalculationPeriod;
 uint16_t NumberRSSImeasurements;
 uint16_t ReportingPeriod;
}zclCmdRSSI_SetDeviceConfiguration_t;

typedef struct 
{
  uint32_t  AddressHigh;
  uint32_t  AddressLow;
}IEEEaddress_t;

typedef struct zclCmdRSSI_GetDeviceConfiguration_tag 
{
  IEEEaddress_t TargetAddress;  
}zclCmdRSSI_GetDeviceConfiguration_t;

typedef struct GetLocationData_tag
{
 unsigned   Reserved           : 3;
 unsigned   CompactResponse    : 1;
 unsigned   BroadcastResponse  : 1;
 unsigned   BroadcastIndicator : 1;
 unsigned   ReCalculate        : 1;
 unsigned   AbsoluteOnly       : 1;
}GetLocationData_t;

typedef struct zclCmdRSSI_GetLocationData_tag 
{
  GetLocationData_t   GetLocationData;
  uint8_t             NumberResponses;   
  IEEEaddress_t       TargetAddress;  
}zclCmdRSSI_GetLocationData_t;


/* 3.13.2.4 RSSI Location server cluster Commands Generated */
enum
{
  gZclCmdTxRSSI_DeviceConfigurationResponse_c = 0,     /* M - Set Absolute Location */
  gZclCmdTxRSSI_LocationDataResponse_c,                /* M - Set Device Configuration */
  gZclCmdTxRSSI_LocationDataNotification_c,            /* M - Get Device Configuration */
  gZclCmdTxRSSI_CompactLocationDataNotification_c,     /* M - Get Location Data */
  gZclCmdTxRSSI_RSSIping_c,                            /* M - Get Location Data */
                                                       /* 0x05-0xFF Reserved */
}; 
 

typedef struct zclCmdRSSI_DeviceConfigurationResponse_tag 
{
 uint8_t   Status;
 int16_t   Power;
 uint16_t  PathLossExponent;
 uint16_t  CalculationPeriod;
 uint8_t   NumberRSSImeasurements;
 uint16_t  ReportingPeriod;
}zclCmdRSSI_DeviceConfigurationResponse_t; 

typedef struct zclCmdRSSI_LocationDataResponse_tag 
{
 uint8_t          Status;
 LocationType_t   LocationType;
 int16_t          Coordinate1;
 int16_t          Coordinate2;
 int16_t          Coordinate3;
 int16_t          Power;
 uint16_t         PathLossExponent;
 uint8_t          LocationMethod;
 uint8_t          QualityMeasure;
 uint16_t         LocationAge;
}zclCmdRSSI_LocationDataResponse_t; 

typedef struct zclCmdRSSI_LocationDataNotification_tag 
{
 LocationType_t   LocationType;
 int16_t          Coordinate1;
 int16_t          Coordinate2;
 int16_t          Coordinate3;
 int16_t          Power;
 uint16_t         PathLossExponent;
 uint8_t          LocationMethod;
 uint8_t          QualityMeasure;
 uint16_t         LocationAge;
}zclCmdRSSI_LocationDataNotification_t; 

typedef struct zclCmdRSSI_CompactLocationDataNotification_tag 
{
 LocationType_t   LocationType;
 int16_t          Coordinate1;
 int16_t          Coordinate2;
 int16_t          Coordinate3;
 uint8_t          QualityMeasure;
 uint16_t         LocationAge;
}zclCmdRSSI_CompactLocationDataNotification_t; 

typedef struct zclCmdRSSI_RSSIping_tag 
{
 LocationType_t   LocationType;
}zclCmdRSSI_RSSIping_t; 

/**********************************************************************
	Binary Input (Basic) Cluster (ZigBee Cluster Library Chapter 3.14.4)
**********************************************************************/

/*Binary Input(Basic) Attributes Sets */
#if ( TRUE == gBigEndian_c )
#define gZclAttrBinaryInputActiveText_c           0x0400    /* O - Active Text */
#define gZclAttrBinaryInputDescription_c          0x1C00    /* O - Description */
#define gZclAttrBinaryInputInactiveText_c         0x2E00    /* O - Inactive Text */
#define gZclAttrBinaryInputOutOfService_c         0x5100    /* M - OutOfService */
#define gZclAttrBinaryInputPolarity_c             0x5400    /* O - Polarity */
#define gZclAttrBinaryInputPresentValue_c         0x5500    /* M - PresentValue */
#define gZclAttrBinaryInputReliability_c          0x6700    /* O - Reliability */
#define gZclAttrBinaryInputStatusFlags_c          0x6F00    /* M - StatusFlags  */
#define gZclAttrBinaryInputApplicationType_c      0x0001    /* O - ApplicationType */
#else
#define gZclAttrBinaryInputActiveText_c           0x0004    /* O - Active Text */
#define gZclAttrBinaryInputDescription_c          0x001C    /* O - Description */
#define gZclAttrBinaryInputInactiveText_c         0x002E    /* O - Inactive Text */
#define gZclAttrBinaryInputOutOfService_c         0x0051    /* M - OutOfService */
#define gZclAttrBinaryInputPolarity_c             0x0054    /* O - Polarity */
#define gZclAttrBinaryInputPresentValue_c         0x0055    /* M - PresentValue */
#define gZclAttrBinaryInputReliability_c          0x0067    /* O - Reliability */
#define gZclAttrBinaryInputStatusFlags_c          0x006F    /* M - StatusFlags  */
#define gZclAttrBinaryInputApplicationType_c      0x0100    /* O - ApplicationType */
#endif /* #if ( TRUE == gBigEndian_c ) */

/* The comand use for this cluster are the read/write attributes */
#ifdef __IAR_SYSTEMS_ICC__
#pragma pack(1)
#endif
typedef struct zclBinaryInputAttrsRAM_tag
{
#if gZclClusterOptionals_d
  zclStr16_t  activeText; 
  zclStr16_t  description; 
  zclStr16_t  inactiveText; 
  uint8_t     polarity;
  uint8_t     reliability;
  uint32_t    applicationType;
#endif
  bool_t      outOfService;
  bool_t      presentValue[zclReportableCopies_c];
  uint8_t     statusFlags[zclReportableCopies_c];
} zclBinaryInputAttrsRAM_t;

typedef struct zclBinaryInput_StatusFlags_tag
{
 unsigned   InAlarm      :1;
 unsigned   Fault        :1;
 unsigned   Overridden   :1;
 unsigned   OutOfService :1;
 unsigned   reserved     :4; 
}zclBinaryInput_StatusFlags_t;


#if gZclClusterOptionals_d
/* Reliability Attribute values */
enum{
  noFaultDetected = 0x00,
  noSensor, /*only for input cluster*/
  overRange,
  underRange,
  openLoop,
  shortedLoop,
  noOutput, /*only for input Sensor*/
  unreliableOther,
  processError,
  multiStateFault,
  configurationError 
};
#endif



/*****************************************************************
	Partitioning Cluster (Telecom Applications Profile Chapter 8.1)
******************************************************************/
#if ( TRUE == gBigEndian_c )
#define gZclAttrPartitioningMaxIncomingTransferSize_c      0x0000    /* M - Maximum Incoming Transfer Size */
#define gZclAttrPartitioningMaxOutgoingTransferSize_c      0x0100    /* M - Maximum Outgoing Transfer Size */
#define gZclAttrPartitioningPartionedFrameSize_c           0x0200    /* M - Partioned Frame Size */
#define gZclAttrPartitioningLargeFrameSize_c               0x0300    /* M - Large Frame Size */
#define gZclAttrPartitioningNumberOfACKframe_c             0x0400    /* M - Number of ACK Frame*/
#define gZclAttrPartitioningNACKtimeout_c                  0x0500    /* M - NACK Timeout */
#define gZclAttrPartitioningInterframeDelay_c              0x0600    /* M - Interframe Delay */
#define gZclAttrPartitioningNumberOfSendRetries_c          0x0700    /* M - Number of Send Retries */
#define gZclAttrPartitioningSenderTimeout_c                0x0800    /* M - Sender Timeout */
#define gZclAttrPartitioningReceiverTimeout_c              0x0900    /* M - Receiver Timeout */
#else
#define gZclAttrPartitioningMaxIncomingTransferSize_c      0x0000    /* M - Maximum Incoming Transfer Size */
#define gZclAttrPartitioningMaxOutgoingTransferSize_c      0x0001    /* M - Maximum Outgoing Transfer Size */
#define gZclAttrPartitioningPartionedFrameSize_c           0x0002    /* M - Partioned Frame Size */
#define gZclAttrPartitioningLargeFrameSize_c               0x0003    /* M - Large Frame Size */
#define gZclAttrPartitioningNumberOfACKframe_c             0x0004    /* M - Number of ACK Frame*/
#define gZclAttrPartitioningNACKtimeout_c                  0x0005    /* M - NACK Timeout */
#define gZclAttrPartitioningInterframeDelay_c              0x0006    /* M - Interframe Delay */ 
#define gZclAttrPartitioningNumberOfSendRetries_c          0x0007    /* M - Number of Send Retries */
#define gZclAttrPartitioningSenderTimeout_c                0x0008    /* M - Sender Timeout */
#define gZclAttrPartitioningReceiverTimeout_c              0x0009    /* M - Receiver Timeout */
#endif /* #if ( TRUE == gBigEndian_c ) */


/* 8.1.3.3 Partitioning cluster Commands Received */
enum
{
   gZclCmdRxPartitioning_TransferPartitionedFrame_c = 0x00, /* M - Transfer Partitioned Frame */
   gZclCmdRxPartitioning_ReadHandshakeParam_c,              /* M - Read Handshake Param */
   gZclCmdRxPartitioning_WriteHandshakeParam_c,             /* M - Write Handshake Param */
};


typedef struct FragmentationOptions_tag
{ 
  unsigned  FirstBlock       :1;
  unsigned  IndicatorLenght  :1;
  unsigned  Reserved         :6; 
} FragmentationOptions_t;

typedef struct zclCmdPartitioning_TransferPartitionedFrame_tag
{
  //¿? Variable ¿?          ZCLHeader
  FragmentationOptions_t    FragmentationOptions;
  uint16_t                  PartitionIndicator;   
  //¿? Variable ¿?          PartitionedFrame;
} zclCmdPartitioning_TransferPartitionedFrame_t;

typedef struct zclCmdPartitioning_ReadHandshakeParam_tag
{
  //¿? Variable ¿?          ZCLHeader
  uint16_t                  ClusterID;
  /* ¿?¿? Not clear how much attributes ID are needed ****
  uint16_t                  AttributeID1;
  uint16_t                  AttributeID2;   
  uint16_t                  AttributeIDn;
  */
} zclCmdPartitioning_ReadHandshakeParam_t;

typedef struct WriteAttributeRecord_tag
{
  uint16_t   AttributeID;
  uint8_t    AttributeDataType;
  //¿? Variable ¿?    AttributeData
} WriteAttributeRecord_t;

typedef struct zclCmdPartitioning_WriteHandshakeParam_tag
{
  //¿? Variable ¿?          ZCLHeader
  uint16_t                  ClusterID;
  /* ¿?¿? Not clear how much write attribute records are needed ****
  WriteAttributeRecord_t    WriteAttributeRecord1;
  WriteAttributeRecord_t    WriteAttributeRecord2;   
  WriteAttributeRecord_t    WriteAttributeRecordn;
  */
} zclCmdPartitioning_WriteHandshakeParam_t;

/* 8.1.3.4 Partitioning cluster Commands Generated */
#define gZclCmdTxPartitioning_MultipleACK_c                    0x00  /* M - Multiple ACK */
#define gZclCmdTxPartitioning_ReadHandshakeParamResponse_c     0x01  /* M - Read Handshake Param Response */


typedef struct ACKOptions_tag
{ 
  unsigned  NACKid       :1;
  unsigned  Reserved     :7; 
} ACKOptions_t;

typedef struct zclCmdPartitioning_MultipleACK_tag
{
  ACKOptions_t         ACKOptions;
  uint16_t             FirstFrameID;
  uint16_t             NACKid;   
  //¿? more NACKid¿¿¿¿¿????
} zclCmdPartitioning_MultipleACK_t;


typedef struct ReadAttributeStatusRecord_tag
{
  uint16_t   AttributeID;
  uint16_t   Status;
  uint8_t    AttributeDataType;
  //¿? Variable ¿?    AttributeData
} ReadAttributeStatusRecord_t;

typedef struct zclCmdPartitioning_ReadHandshakeParamResponse_tag
{
  //¿? Variable ¿?          ZCLHeader
  uint16_t                  PartitionedClusterID;
  /* ¿?¿? Not clear how much read attribute status records are needed ****
  ReadAttributeStatusRecord_t    ReadAttributeStatusRecord1;
  ReadAttributeStatusRecord_t    ReadAttributeStatusRecord2;   
  ReadAttributeStatusRecord_t    ReadAttributeStatusRecordn;
  */
} zclCmdPartitioning_ReadHandshakeParamResponse_t;



/**************************************************************************
	Alpha-Secure Key Establishment Cluster (Health Care Profile Annex A.2)
***************************************************************************/
#if ( TRUE == gBigEndian_c )
#define gZclAttrASKEInformationSet_c   0x0000    /* ASKE Information attributes Set */
#else
#define gZclAttrASKEInformationSet_c   0x0000    /* ASKE Information attributes Set */
#endif /* #if ( TRUE == gBigEndian_c ) */

/* ASKE Information Attributes Set */
#if ( TRUE == gBigEndian_c )
#define gZclAttrASKEInformationASKEversion_c              0x0000    /* M - ASKE version */
#define gZclAttrASKEInformationMaxSDnumber_c              0x0100    /* M - Max Security Domain number*/
#define gZclAttrASKEInformationSDlist_c                   0x0200    /* M - Security Domain List */
#define gZclAttrASKEInformationMaxKeyingMaterialSize_c    0x0300    /* M - Max size of a single KM instance*/
#define gZclAttrASKEInformationMaxRevocationListLenght_c  0x0400    /* O - Max number of devices in a single instance of a revocation list */
#define gZclAttrASKEInformationASKEtimeout_c              0x0500    /* M - ASKE timeout */
#define gZclAttrASKEInformationKeySegmentSizesupport_c    0x0600    /* M - List of Key segment sizes */
#else
#define gZclAttrASKEInformationASKEversion_c              0x0000    /* M - ASKE version */
#define gZclAttrASKEInformationMaxSDnumber_c              0x0001    /* M - Max Security Domain number*/
#define gZclAttrASKEInformationSDlist_c                   0x0002    /* M - Security Domain List */
#define gZclAttrASKEInformationMaxKeyingMaterialSize_c    0x0003    /* M - Max size of a single KM instance*/
#define gZclAttrASKEInformationMaxRevocationListLenght_c  0x0004    /* O - Max number of devices in a single instance of a revocation list */
#define gZclAttrASKEInformationASKEtimeout_c              0x0005    /* M - ASKE timeout */
#define gZclAttrASKEInformationKeySegmentSizesupport_c    0x0006    /* M - List of Key segment sizes */
#endif /* #if ( TRUE == gBigEndian_c ) */

#define gCurrentASKEversion 0x00


/*
typedef struct 
{
  uint32_t  AddressHigh;
  uint32_t  AddressLow;
}IEEEaddress_t;
*/

typedef struct 
{
  uint32_t  TSLKsecurityKey1;
  uint32_t  TSLKsecurityKey2;
  uint32_t  TSLKsecurityKey3;
  uint32_t  TSLKsecurityKey4;
}TSLKsecurityKey_t;

typedef struct 
{
  IEEEaddress_t      IEEEAddress;
  TSLKsecurityKey_t  TSLKsecurityKey;
}TrustServer_t;


typedef struct ASKESecurityDomainTable_tag
{ 
  uint8_t       Index;                     /*  */
  uint16_t      SDidentifier;              /*  */
  IEEEaddress_t EPID;                      /*  */
  uint8_t       FPPorder;                  /*  */
  uint8_t       Lambda;                    /*  */
  uint16_t      KeySegmentSize;            /*  */
  //¿? Variable ¿?  Irreducible;           /*  */
  uint8_t       Entropy;                   /*  */
  uint8_t       CryptoIdentityType;        /*  */
  uint16_t      CryptoIdentity16;          /*  */
  uint16_t      KeyingMaterial;            /*  */
  uint8_t       KeyingMaterialVersion;     /*  */
  uint8_t       RevocationList;            /*  */
  TrustServer_t TrustServer;               /*  */
} ASKESecurityDomainTable_tag;             
                                           
enum                                       
{
   gCryptoIndentityType_16bitAddress_c = 0x00,       /* 16-bit Address */
   gCryptoIndentityType_64bitAddress_c,              /* 64-bit Address */
   gCryptoIndentityType_16bitIdentifier_c            /* 16-bit Identifier */
                                                     /* 0x03-0xFF Reserved */
};

/* A.2.8.5 ASKE cluster Commands Received */

enum
{
   gZclCmdRxASKE_ConfigureSD_c = 0x00,    /* O - Configure SD */
   gZclCmdRxASKE_UpdateRevocationList_c,  /* O - Update Revocation List */
   gZclCmdRxASKE_RemoveSD_c,              /* O - Remove SD */
   gZclCmdRxASKE_ReadSD_c,                /* O - Read SD */
   gZclCmdRxASKE_InitiateASKE_c,          /* O - Initiate ASKE */
   gZclCmdRxASKE_ConfirmASKEkey_c,        /* O - Confirm ASKE key */
   gZclCmdRxASKE_TerminateASKE_c,         /* O - Terminate ASKE */
   gZclCmdRxASKE_GenerateAMK_c,           /* O - Generate AMK */
                                          /* 0x08-0xFF Reserved */
};


typedef struct zclCmdASKE_ConfigureSD_tag
{
 uint16_t       Options;
 uint16_t       ALKupdateTime;
 uint16_t       SDindetifier;
 IEEEaddress_t  EPID;
 uint8_t        Index;
 TrustServer_t  TrustServer;
 uint16_t       KeyingMaterialSize;
 uint8_t        FPPorder;
 uint8_t        Lambda;
 uint16_t       KeySegmentSize;
 uint8_t        Entropy; 
 //¿? octet string ¿?  Irreducible;
 uint8_t        CryptoIdentityType;
 uint16_t       CryptoIdentity16;
 IEEEaddress_t  CryptoIdentity;
 uint16_t       KeyingMaterialOffset;
 uint8_t        KeyingMaterialVersion;
 //¿? variable ¿?  KeyingMaterial;
}zclCmdASKE_ConfigureSD_t; 

typedef struct zclCmdASKE_UpdateRevocationList_tag
{
 uint16_t       Options;
 uint16_t       SDindetifier;
 IEEEaddress_t  EPID;
 uint8_t        Index;
 uint8_t        CryptoIdentityType;
 uint8_t        NumberOfDevices;
 //¿? variable ¿?  CryptoIndentityList;
}zclCmdASKE_UpdateRevocationList_t;

enum
{
   gOptions_ClearRevocationList_c = 0,      /* Clear Revocation List */
   gOptions_AddToRevocationList_c,          /* Clear Revocation List */
   gOptions_RemoveFromRevocationList_c,     /* Clear Revocation List */
   gOptions_SendRevocationListLength_c,     /* Send Revocation List Length */
   gOptions_SendRevocationList_c,           /* Send Revocation List */
                                            /* 5-7 Reserved */
};


typedef struct zclCmdASKE_RemoveSD_tag
{
 uint16_t       Options;
 uint16_t       SDindetifier;
 IEEEaddress_t  EPID;
 uint8_t        Index;
}zclCmdASKE_RemoveSD_t;

typedef struct zclCmdASKE_ReadSD_tag
{
 uint16_t       Options;
 uint16_t       SDindetifier;
 IEEEaddress_t  EPID;
 uint8_t        Index;
 uint16_t       Offset;
}zclCmdASKE_ReadSD_t;

typedef struct zclCmdASKE_InitiateASKE_tag
{
 uint16_t       SDindetifier;
 uint8_t        ASKEversion;
 uint8_t        KeyingMaterialVersion;
 uint8_t        CryptoIdentityType;
 uint16_t       CryptoIdentity16;
 IEEEaddress_t  CryptoIdentity;
 uint8_t        EphemeralData[16];
}zclCmdASKE_InitiateASKE_t;

typedef struct zclCmdASKE_ConfirmASKEkey_tag
{
 uint8_t        HMAC[16];
}zclCmdASKE_ConfirmASKEkey_t;

typedef struct zclCmdASKE_TerminateASKE_tag
{
 uint8_t        Status;
}zclCmdASKE_TerminateASKE_t;

typedef struct zclCmdASKE_GenerateAMK_tag
{
 uint16_t       SDindetifier;
 uint8_t        CryptoIdentityType;
 uint16_t       CryptoIdentity16;
 IEEEaddress_t  CryptoIdentity;
}zclCmdASKE_GenerateAMK_t;

/* A.2.8.6 ASKE cluster Commands Generated */

enum
{
   gZclCmdTxASKE_ConfigureSDResponse_c = 0x00,    /* O - Configure SD Resposne */
   gZclCmdTxASKE_UpdateRevocationListResponse_c,  /* O - Update Revocation List Resposne */
   gZclCmdTxASKE_RemoveSDResponse_c,              /* O - Remove SD Resposne */
   gZclCmdTxASKE_ReadSDResponse_c,                /* O - Read SD Resposne */
   gZclCmdTxASKE_InitiateASKEResponse_c,          /* O - Initiate ASKE Resposne */
   gZclCmdTxASKE_ConfirmASKEkeyResponse_c,        /* O - Confirm ASKE key Resposne */
   gZclCmdTxASKE_TerminateASKE_c,                 /* O - Terminate ASKE */
   gZclCmdTxASKE_GenerateAMKResponse_c,           /* O - Generate AMK Resposne */
   gZclCmdTxASKE_ReportRevokedNode_c,             /* O - Report Revoked Node */
   gZclCmdTxASKE_RequestConfiguration_c,          /* M - Request Configuration */
                                                  /* 0x0A-0xFF Reserved */
};


typedef struct zclCmdASKE_ConfigureSDresponse_tag
{
 uint8_t    Status;
 uint8_t    Index;
}zclCmdASKE_ConfigureSDresponse_t;

typedef struct zclCmdASKE_UpdateRevocationListResponse_tag
{
 uint8_t        Status;
 uint8_t        Options;
 uint16_t       SDindetifier;
 IEEEaddress_t  EPID;
 uint8_t        TotalListLength;
 uint8_t        StartIndex;
 uint8_t        CryptoIdentityType;
 uint8_t        ListCount;
 //¿? variable ¿?  CryptoIDList;
}zclCmdASKE_UpdateRevocationListResponse_t;

typedef struct zclCmdASKE_RemoveSDresponse_tag
{
 uint8_t        Status;
}zclCmdASKE_RemoveSDresponse_t;

typedef struct zclCmdASKE_ReadSDresponse_tag
{
 uint8_t        Status;
 uint8_t        Options;
 uint16_t       SDindetifier;
 IEEEaddress_t  EPID;
 uint8_t        Index;
 TrustServer_t  TrustServer;
 uint16_t       KeyingMaterialSize;
 uint8_t        FPPorder;
 uint8_t        Lambda;
 uint16_t       KeySegmentSize;
 uint8_t        Entropy;
 //¿? octet string ¿?  Irreducible;
 uint8_t        CryptoIdentityType;
 uint16_t       CryptoIdentity16;
 IEEEaddress_t  CryptoIdentity;
 uint16_t       KeyingMaterialOffset;
 uint8_t        KeyingMaterialVersion;
 //¿? variable ¿?  KeyingMaterial;
}zclCmdASKE_ReadSDresponse_t;

typedef struct zclCmdASKE_InitiateASKEresponse_tag
{
 uint8_t        ASKEversion;
 uint8_t        CryptoIdentityType;
 uint16_t       CryptoIdentity16;
 IEEEaddress_t  CryptoIdentity;
 uint8_t        EphemeralData[16];
}zclCmdASKE_InitiateASKEresponse_t;

typedef struct zclCmdASKE_ConfirmASKEkeyResponse_tag
{
 uint8_t        HMAC[16];
}zclCmdASKE_ConfirmASKEkeyResponse_t;

typedef struct zclCmdASKE_GenerateAMKresponse_tag
{
 uint8_t        Status;
 uint8_t        CryptoIdentityType;
 uint16_t       CryptoIdentity16;
 IEEEaddress_t  CryptoIdentity;
}zclCmdASKE_GenerateAMKresponse_t;

typedef struct zclCmdASKE_ReportRevokedNode_tag
{
 uint16_t        SDidentifier;
 uint8_t        CryptoIdentityType;
 uint16_t       RevokedCryptoID16;
 IEEEaddress_t  RevokedCryptoID;
}zclCmdASKE_ReportRevokedNode_t;

typedef struct zclCmdASKE_RequestConfiguration_tag
{
 IEEEaddress_t  IEEEaddress;
 uint16_t       ShortAddress;
}zclCmdASKE_RequestConfiguration_t;

enum
{
   STATUS_SUCCESS_c = 0x00,         
   STATUS_FAILURE_c,                
   STATUS_UNSUP_CLUSTER_COMMAND_c = 0x81,  
   STATUS_INVALID_FIELD_c = 0x85,          
   STATUS_UNSUPPORTED_ATTRIBUTE_c,  
   STATUS_READ_ONLY_c = 0x88,              
   STATUS_INSUFFICIENT_SPACE_c,     
   STATUS_NOT_FOUND_c = 0x8B,              
   STATUS_DEFINED_OUT_OF_BAND_c = 0x91,    
   STATUS_INCONSISTENT_c,           
   STATUS_ACTION_DENIED_c,          
   STATUS_TIMEOUT_c,                                              
};


/**************************************************************************
	Alpha-Secure Access Control Cluster (Health Care Profile Annex A.3)
***************************************************************************/
#if ( TRUE == gBigEndian_c )
#define gZclAttrASACInformationSet_c   0x0000    /* ASAC Information attributes Set */
#else
#define gZclAttrASACInformationSet_c   0x0000    /* ASAC Information attributes Set */
#endif /* #if ( TRUE == gBigEndian_c ) */

/* ASKE Information Attributes Set */
#if ( TRUE == gBigEndian_c )
#define gZclAttrASACInformationASACversion_c                   0x0000    /* M - ASAC version */
#define gZclAttrASACInformationMaxSDnumber_c                   0x0100    /* M - Max Security Domain number*/
#define gZclAttrASACInformationMaxKeyingMaterialSize_c         0x0200    /* M - Max Keying Material Size */
#define gZclAttrASACInformationMaxPermissionsTableEntries_c    0x0300    /* M - Max Permission Table Entries */
#define gZclAttrASACInformationMaxCertificateTableEntries_c    0x0400    /* M - Max Certificate Table Entries */
#define gZclAttrASACInformationMaxRevocationListLenght_c       0x0500    /* O - Max number of devices in a single instance of a revocation list */
#define gZclAttrASACInformationASACtimeout_c                   0x0600    /* M - ASAC timeout */
#define gZclAttrASACInformationMainSDlist_c                    0x0700    /* M - Main Security Domain List */
#define gZclAttrASACInformationKeySegmentSizeSupport_c         0x0800    /* M - Key Segment Size Support*/

#else
#define gZclAttrASACInformationASACversion_c                   0x0000    /* M - ASAC version */
#define gZclAttrASACInformationMaxSDnumber_c                   0x0001    /* M - Max Security Domain number*/
#define gZclAttrASACInformationMaxKeyingMaterialSize_c         0x0002    /* M - Max Keying Material Size */
#define gZclAttrASACInformationMaxPermissionsTableEntries_c    0x0003    /* M - Max Permission Table Entries */
#define gZclAttrASACInformationMaxCertificateTableEntries_c    0x0004    /* M - Max Certificate Table Entries */
#define gZclAttrASACInformationMaxRevocationListLenght_c       0x0005    /* O - Max number of devices in a single instance of a revocation list */
#define gZclAttrASACInformationASACtimeout_c                   0x0006    /* M - ASAC timeout */
#define gZclAttrASACInformationMainSDlist_c                    0x0007    /* M - Main Security Domain List */
#define gZclAttrASACInformationKeySegmentSizeSupport_c         0x0008    /* M - Key Segment Size Support*/
#endif /* #if ( TRUE == gBigEndian_c ) */

#define gCurrentASACversion 0x00

typedef struct 
{
  uint8_t  PropertyID;
  uint8_t  PropertyType;
  //-----  PropertyValue;   // this value depends on the property type (see following enumeration)
}LightWeightDigCert_t;

enum                                       
{
   gPropertyType_IEEEaddress_c = 0x00, /* IEEE address */
   gPropertyType_ProfileID_c,          /* Profile ID = 16-bit */
   gPropertyType_DeviceID_c,           /* Device ID = 16-bit */
   gPropertyType_ServerClusters_c,     /* Server Clusters = Set of cluster ID */
   gPropertyType_ClientClusters_c,     /* Client Clusters = Set of cluster ID */
   gPropertyType_UniqueNumber_c,       /* Unique Number = 128-bit */
                                       /* 0x06-0x40 Reserved Generic */
                                       /* 0x40-0x80 Reserved Application Profile */
                                       /* 0x80-0xFF Reserved Manufacturer */
};


typedef struct 
{
  uint32_t  LDCfingerprint1;
  uint32_t  LDCfingerprint2;
  uint32_t  LDCfingerprint3;
  uint32_t  LDCfingerprint4;
}LCDfingerprint_t;

typedef struct 
{
  unsigned  DefaultPermission : 1;
  unsigned  Reserved          : 7;
}DefaultPermission_t;

typedef struct ASACSecurityDomainTable_tag
{ 
  uint8_t                Index;                      /* M - Number of the ASAC SD table entry */
  uint16_t               SDidentifier;               /* M - Unique identifier of the current SD */
  uint8_t                SDpointer;                  /* M - Pointer to the position of the SD in MASKE */
  uint8_t                SDdescriptor;               /* O - Descriptor of the features associated with teh current SD */
  uint16_t               MainSD;                     /* O - Main SD */
  IEEEaddress_t          EPID;                       /* O - EPID */
  LightWeightDigCert_t   LightWeightDigCertificate;  /* O - Light Weight Digital Certificate  */
  LCDfingerprint_t       LDCfingerprint;             /* O - LDC fingerprint */
  uint8_t                ACcryptoIdentitySize;       /* M - AC Crypto Identity Size */
  uint8_t                ACcryptoIdentity;           /* O - AC crypto Identity */ 
  uint8_t                SegmentNumber;              /* M - Segment Number */
  uint8_t                FPPorder;                   /* M - FPP order */
  uint8_t                Lambda;                     /* M - Lambda */
  uint16_t               KeySegmentSize;             /* M - Key segment size */
  uint8_t                Irreducible;                /* O - Irreducible */
  uint16_t               KeyingMaterial;             /* M - Keying Material */
  uint8_t                KeyingMaterialVersion;      /* M - Keying Material version */
  uint8_t                RevocationListSize;         /* O - Revocation List Size */
  uint8_t                RevocationList;             /* O - Revocation List */
  DefaultPermission_t    DefaultPermission;           /* O - Default Permission*/
  TrustServer_t          TrustServer;                /* O - Trust Server */
} ASACSecurityDomainTable_tag;

typedef struct 
{
  unsigned  ServerSideCommand           : 1;
  unsigned  ClientSideCommand           : 1;
  unsigned  ZCLCommand                  : 1;
  unsigned  ManufacturerSpecificCommand : 1;
  unsigned  GenericCommand              : 1;
  unsigned  ClusterSpecificCommand      : 1;
  unsigned  Reserved                    : 2;
}SpecificSel_t;

typedef struct 
{
  uint8_t        Eightbitmap;
  SpecificSel_t  SpecificSelector;
}Command_t;

typedef struct 
{
  unsigned  Reserved              : 4;
  unsigned  NumberOfLDCproperties : 4;
  unsigned  NumberOfSDs           : 4;
  unsigned  NumberOfIEEE          : 4;
}Options_t;

typedef struct ASACPermissionsTable_tag
{ 
  uint16_t               Profile;                /* M - Profile */
  uint8_t                EndPoint;               /* M - EndPoint */
  uint16_t               Cluster;                /* M - Cluster */
  uint16_t               Attribute;              /* M - Attribute */
  Command_t              Command;                /* M - Command */
  uint16_t               AssociatedMainSD;       /* M - Associated Main SD */
  Options_t              Options;                /* M - Options */
   //-----               RequiredLDCproperties;  /* O - Required LDC properties */
   //-----                RequiredSubSDs;        /* O - Required Sub SDs */
   //-----                RequiredIEEE;          /* O - Required IEEE */

} ASACPermissionsTable_t;


/* A.3.7.6 ASAC cluster Commands Received */

enum
{
   gZclCmdRxASAC_ConfigureSD_c = 0x00,    /* O - Configure SD */
   gZclCmdRxASAC_UpdateRevocationList_c,  /* O - Update Revocation List */
   gZclCmdRxASAC_ConfigureACpolicies_c,   /* O - Configure AC Policies */
   gZclCmdRxASAC_ReadSD_c,                /* O - Read SD */
   gZclCmdRxASAC_ReadACpolicies_c,        /* O - Read AC policies */
   gZclCmdRxASAC_RemoveAC_c,              /* O - Remove AC */
   gZclCmdRxASAC_ACpropertiesReq_c,       /* O - AC properties request */
   gZclCmdRxASAC_TSreportResponse_c,      /* O - TS report response */
   gZclCmdRxASAC_InitiateASAC_c,          /* O - Initiate ASAC */
   gZclCmdRxASAC_ConfirmASACkey_c,        /* O - Confirm ASAC key */
   gZclCmdRxASAC_TerminateASAC_c,         /* O - Terminate ASAC */
   gZclCmdRxASAC_GenerateAMK_c,           /* O - Generate AMK */
   gZclCmdRxASAC_LDCtransport_c,          /* O - LDC transport */
                                          /*     0x0D-0xFF Reserved */
};

/* Configure SD command Payload */


typedef struct 
{
 unsigned ALKUpdateSchedule    : 3;
 unsigned SDparametersPresent  : 1;
 unsigned EPIDpresent          : 1;
 unsigned IndexPresent         : 1;
 unsigned TrustServerPresent   : 1;
 unsigned NewKMstructure       : 1;
 unsigned ACcryptoIDpresent    : 1;
 unsigned PartialKM            : 1;
 unsigned CompleteKM           : 1;
 unsigned LDCpresent           : 1;
 unsigned Reserved             : 4;
}ConfigSD_Options_t;

typedef struct zclCmdASAC_ConfigureSD_tag
{
 ConfigSD_Options_t       Options;
 uint16_t       ALKupdateTime;
 uint16_t       SDindetifier;
 IEEEaddress_t  EPID;
 uint8_t        Index;
 uint8_t        SDpointer;
 uint8_t        SDdescriptor;
 uint16_t       MainSDID;
 TrustServer_t  TrustServer;
 DefaultPermission_t DefaultPermission;
 uint16_t       KeyingMaterialSize;
 uint8_t        FPPorder;
 uint8_t        Lambda;
 uint8_t        SegmentNumber; 
 uint16_t       KeySegmentSize; 
 //¿? octet string ¿?  Irreducible;
 uint8_t        ACIdentitySize;
 uint16_t       ACcryptoID;
 uint16_t       KeyingMaterialOffset;
 uint8_t        KeyingMaterialVersion;
 //¿? variable ¿?  KeyingMaterial;
 //------       LDC;
}zclCmdASAC_ConfigureSD_t;

enum                                       
{
   gALKupdate_InmmediateUpdate_c = 0,    /* Inmmediate Update all ALK from previous KM */
   gALKupdate_UpdateNextComm_c,          /* Update all ALK from previous KM on next communication */
   gALKupdate_UpdateAfterTimeElapse_c,   /* Update all ALK from previous KM after a Time elapsed*/
   gALKupdate_NoUpdate_c,                /* Do not Update all ALK until explicity instructed */
                                         /* 4-7 Reserved Manufacturer */
};

/* Update Revocation List command Payload */


typedef struct 
{
 unsigned Action        : 3;
 unsigned EPIDpresent   : 1;
 unsigned Reserved      : 4;
        
}RevListCommand_Options_t;

typedef struct zclCmdASAC_UpdateRevocationList_tag
{
 RevListCommand_Options_t    Options;
 uint16_t       SDindetifier;
 IEEEaddress_t  EPID;
 uint8_t        ListIndex;
 uint8_t        NumberOfDevices;
 //---          ACcryptoIDList;
}zclCmdASAC_UpdateRevocationList_t;

/* Configure AC Policies command Payload */

typedef struct 
{
 unsigned ProfileIDpresent   : 1;
 unsigned Endpointpresent    : 1;
 unsigned CommandPresent     : 1;
 unsigned AttributePresent   : 1;
 unsigned Reserved           : 4;
        
}ACpolicies_Options_t;

typedef struct 
{
 unsigned Reserved              : 4;
 unsigned NumberOfLDCproperties : 4;
 unsigned NumberOfSubSDs        : 4;
 unsigned NumberOfIEEE          : 4;
        
}ACpolicies_ACoptions_t;


typedef struct zclCmdASAC_ConfigureACpolicies_tag
{
 ACpolicies_Options_t    Options;
 uint16_t       Profile;
 uint8_t        Endpoint;
 uint16_t       Cluster;
 ACpolicies_ACoptions_t  ACoptions;
 uint16_t       Command;
 uint16_t       Attribute;
 uint16_t       MainSDID;
 //-----        RequiredLDCproperties;  /* O - Required LDC properties */
 //-----        RequiredSubSDs;        /* O - Required Sub SDs */
 //-----        RequiredIEEE;          /* O - Required IEEE */
}zclCmdASAC_ConfigureACpolicies_t;

/* Read SD command Payload */

typedef struct 
{

 unsigned SDidentifierPresent : 1;
 unsigned EPIDpresent         : 1;
 unsigned IndexPresent        : 1;
 unsigned ReadTS              : 1;
 unsigned ReadMainSDKMstruct  : 1;
 unsigned ReadMainSDACcryptoID: 1;
 unsigned ReadMainSDKM        : 1;
 unsigned bit7                : 1;   //missing description in HC profile for this bit (figure 57, A.3.7.6.1.4.1)
 unsigned ReadSubSDKMstruct   : 1;
 unsigned ReadSubSDACcryptoID : 1;
 unsigned ReadSubSDKM         : 1;
 unsigned Reserved            : 5;        
}ReadSD_Options_t;


typedef struct zclCmdASAC_ReadSD_tag
{
 ReadSD_Options_t  Options;
 uint16_t       SDindetifier;
 IEEEaddress_t  EPID;
 uint8_t        Index;
 uint16_t       MainSDoffset;
 uint16_t       SubSDoffset;
}zclCmdASAC_ReadSD_t;

/* Read AC Policies command Payload */

typedef struct 
{

 unsigned MainSDID          : 1;
 unsigned ProfilePresent    : 1;
 unsigned ClusterPresent    : 1;
 unsigned CommandPresent    : 1;
 unsigned AttributePresent  : 1;
 unsigned Reserved          : 3;        
}ReadACpolicies_Options_t;


typedef struct zclCmdASAC_ReadACpolicies_tag
{
 ReadACpolicies_Options_t  Options;
 uint16_t       MainSDID;
 uint16_t       Profile;
 uint16_t       Cluster;
 uint16_t       Command;
 uint16_t       Attribute;
}zclCmdASAC_ReadACpolicies_t;

/* Remove AC command Payload */

typedef struct 
{

 unsigned RemoveSD      : 1;
 unsigned RemoveSubSDs  : 1;
 unsigned RemovePolicy  : 1;
 unsigned IndexPresent  : 1;
 unsigned SDidentifier  : 1;
 unsigned EPIDpresent   : 1; 
 unsigned Reserved      : 2; 
}RemoveAC_Options_t;

typedef struct zclCmdASAC_RemoveAC_tag
{
 RemoveAC_Options_t  Options;
 uint8_t       Index;
 uint16_t       SDidentifier;
 IEEEaddress_t  EPID;
}zclCmdASAC_RemoveAC_t;

/* AC properties command Payload */

// This command has no payload


/* TS Report Response command Payload */

// This command has no payload

/* Initiate ASAC command Payload */

typedef struct 
{

 unsigned LDCfingerprintPresent      : 1; 
 unsigned Reserved                   : 7; 
}InitiateASAC_Options_t;

typedef struct zclCmdASAC_InitiateASAC_tag
{
 InitiateASAC_Options_t  Options;
 uint16_t       MainSDidentifier;
 uint8_t       ASACversion;
 //-----  InitiatorACcryptoID;
 uint8_t  InitiatorLDCfingerprint[16];
 uint8_t  InitiatorEphemeralData[16];
 uint8_t  NumberOfSubSDpointers;
 //-----  SubSDpointers;
}zclCmdASAC_InitiateASAC_t;

/* Confirm ASAC command Payload */
typedef struct zclCmdASAC_ConfirmASAC_tag
{
 uint8_t HMAC[16];
}zclCmdASAC_ConfirmASAC_t;

/* Terminate ASAC command Payload */
typedef struct zclCmdASAC_TerminateASAC_tag
{
 uint8_t Status;
}zclCmdASAC_TerminateASAC_t;


enum                                       
{
   gConfirmASACstatus_Success_c = 0x00,          /* Success */
   gConfirmASACstatus_Failure_c,                 /* Failure */
   gConfirmASACstatus_UnsupClusterCommand_c,     /* Unsup Cluster Command */
   gConfirmASACstatus_InvalidField_c,            /* Invalid Field */
   gConfirmASACstatus_UnsupportedAttribute_c,    /* Unsupported Attribute */
   gConfirmASACstatus_ReadOnly_c,                /* Read Only */ 
   gConfirmASACstatus_InsufficientSpace_c,       /* Insufficient Space */
   gConfirmASACstatus_NotFound_c,                /* Not Found */
   gConfirmASACstatus_DefinedOutOfBand_c,        /* Defined out of Band */
   gConfirmASACstatus_Inconsistent_c,            /* Inconsistent */
   gConfirmASACstatus_ActionDenied_c,            /* Action Denied */
   gConfirmASACstatus_Timeout_c,                 /* Timeout */
};

/* Generate AMK command Payload */


typedef struct 
{

 unsigned LDCfingerprintPresent      : 1; 
 unsigned Reserved                   : 7; 
}GenerateAMK_Options_t;

typedef struct zclCmdASAC_GenerateAMK_tag
{
 GenerateAMK_Options_t        Options;
 uint16_t       MainSDindetifier;
 uint8_t        ASACversion;
 //------       InitiatorACcryptoID;
 uint8_t        LDCfingerprint[16];
 uint8_t        NumberOfSubSDsPointers;
 //------       SubSDpointers;
}zclCmdASAC_GenerateAMK_t;

/* LDC Transport command Payload */

typedef struct 
{

 unsigned OwnACcryptoPresent : 1;
 unsigned OwnLDCpresent      : 1;
 unsigned RequestACcryptoID  : 1;
 unsigned RequestLDC         : 1; 
 unsigned Reserved           : 4; 
}LDCtransport_Options_t;

typedef struct zclCmdASAC_LDCtransport_tag
{
 LDCtransport_Options_t  Options;
 uint16_t       MainSD;
 //------        ACcryptoID;
 //------       LDC;
}zclCmdASAC_LDCtransport_t;


/* A.3.7.7 ASAC cluster Commands Generated */

enum
{
   gZclCmdTxASAC_ConfigureSDresponse_c = 0x00,    /* O - Configure SD response */
   gZclCmdTxASAC_UpdateRevocationListResponse_c,  /* O - Update Revocation List Response */
   gZclCmdTxASAC_ConfigureACpoliciesResponse_c,   /* O - Configure AC Policies Response */
   gZclCmdTxASAC_ReadSDresponse_c,                /* O - Read SD Response */
   gZclCmdTxASAC_ReadACpoliciesResponse_c,        /* O - Read AC policies Response */
   gZclCmdTxASAC_RemoveACresponse_c,              /* O - Remove AC Response */
   gZclCmdTxASAC_ACpropertiesResponse_c,          /* O - AC properties Response */
   gZclCmdTxASAC_TSreport_c,                      /* O - TS report */
   gZclCmdTxASAC_InitiateASACresponse_c,          /* O - Initiate ASAC Response */
   gZclCmdTxASAC_ConfirmASACkeyResponse_c,        /* O - Confirm ASAC key Response */
   gZclCmdTxASAC_TerminateASAC_c,                 /* O - Terminate ASAC */
   gZclCmdTxASAC_GenerateAMKresponse_c,           /* O - Generate AMK Response */
   gZclCmdTxASAC_LDCtransportResponse_c,          /* O - LDC transport Response */
   gZclCmdTxASAC_RequestConfiguration_c,          /* O - Request Configuration */
                                                  /*     0x0E-0xFF Reserved */
};

/* Configure SD response command Payload */


typedef struct zclCmdASAC_ConfigureSDresponse_tag
{
 uint8_t       Status;
 uint8_t       Index;
}zclCmdASAC_ConfigureSDresponse_t;

/* Update Revocation List response command Payload */

typedef struct zclCmdASAC_UpdateRevocationListResponse_tag
{
 uint8_t        Status;
 uint8_t        Options;
 uint16_t       MainSDindetifier;
 IEEEaddress_t  EPID;
 uint8_t        TotalListLenght;
 uint8_t        CryptoIdentityType;
 uint8_t        ListCount;
 //---          ACcryptoIDList;
}zclCmdASAC_UpdateRevocationListResponse_t;

/* Configure AC Policies Response command Payload */
typedef struct zclCmdASAC_ConfigureACpoliciesResponse_tag
{
 uint8_t        Status;
}zclCmdASAC_ConfigureACpoliciesResponse_t;

/* Read SD Response command Payload */

typedef struct 
{
  unsigned  SDIDpresent              : 1;
  unsigned  EPIDpresent              : 1;
  unsigned  IndexPresent             : 1;
  unsigned  SDdescriptorPresent      : 1;
  unsigned  MainSDidentifierPresent  : 1;
  unsigned  TSpresent                : 1;
  unsigned  KMstructPresent          : 1;
  unsigned  ACcryptoIDpresent        : 1;
  unsigned  KMpresent                : 1;
  unsigned  Continuation             : 1;
  unsigned  Reserved                 : 6;

}ReadSDresponse_Options_t;

typedef struct zclCmdASAC_ReadSDresponse_tag
{
 uint8_t    Status;
 ReadSDresponse_Options_t  Options;
 uint16_t       SDindetifier;
 IEEEaddress_t  EPID;
 uint8_t        Index;
 //------   SDdescriptor;  
 uint16_t       MainSDidentifier;
 TrustServer_t  TrustServer;
 DefaultPermission_t DefaultPermission;
 uint16_t       KeyingMaterialSize;
 uint8_t        FPPorder;
 uint8_t        Lambda;
 uint8_t        SegmentNumber; 
 uint16_t       KeySegmentSize; 
 //¿? octet string ¿?  Irreducible;
 uint16_t       ACcryptoID;
 uint16_t       KeyingMaterialOffset;
 uint8_t        KeyingMaterialVersion;
 //¿? variable ¿?  KeyingMaterial;
}zclCmdASAC_ReadSDresponse_t;

/* Read AC policies Response command Payload */
typedef struct 
{
  unsigned  ProfileIDpresent    : 1;
  unsigned  EndpointPresent     : 1;
  unsigned  CommandPresent      : 1;
  unsigned  AttributeIDPresent  : 1;
  unsigned  MultipleResponses   : 1;
  unsigned  Reserved            : 3;
}ReadACpoliciesResponse_Options_t;

typedef struct zclCmdASAC_ReadACpoliciesResponse_tag
{
 uint8_t    Status;
 ReadACpoliciesResponse_Options_t  Options;
 uint16_t   Profile;
 uint8_t    Endpoint;
 uint16_t   Cluster;
 uint16_t   ACoptions;
 uint16_t   Command;
 uint16_t   Attribute;
 uint16_t       MainSDID;
 //-----    RequiredLDCproperties;
 //-----    Required SubSDs;
 //-----    Required IEEE;
}zclCmdASAC_ReadACpoliciesResponse_t;

/* Remove AC Response command Payload */

typedef struct 
{
  unsigned  ProfileIDpresent    : 1;
  unsigned  EndpointPresent     : 1;
  unsigned  CommandPresent      : 1;
  unsigned  AttributeIDPresent  : 1;
  unsigned  MultipleResponses   : 1;
  unsigned  Reserved            : 3;
}RemoveACresponse_Options_t;

typedef struct zclCmdASAC_RemoveACresponse_tag
{
 uint8_t    Status;
 ReadACpoliciesResponse_Options_t  Options;
 uint16_t   Profile;
 uint8_t    Endpoint;
 uint16_t   Cluster;
 uint16_t   ACoptions;
 uint16_t   Command;
 uint16_t   Attribute;
 uint16_t   MainSDID;
 //-----    RequiredLDCproperties;
 //-----    Required SubSDs;
 //-----    Required IEEE;
}zclCmdASAC_RemoveACresponse_t;

/* AC properties Response command Payload */

typedef struct 
{
  unsigned  ListComplete    : 1;
  unsigned  Reserved        : 7;
}ACPropertiesResponse_Options_t;

typedef struct zclCmdASAC_ACpropertiesResponse_tag
{
 uint8_t    Status;
 ACPropertiesResponse_Options_t  Options;
 //-----    PropertiesList;
}zclCmdASAC_ACpropertiesResponse_t;

/* TS report command Payload */

typedef struct 
{
  unsigned  EPIDpresent    : 1;
  unsigned  LDC1present    : 1;
  unsigned  IEEE1present    : 1;
  unsigned  LDC2present    : 1;
  unsigned  IEEE2present    : 1;
  unsigned  Reserved       : 3;
}TSreport_Options_t;

typedef struct zclCmdASAC_TSreport_tag
{
 uint8_t    Status;
 uint8_t    Options;
 uint16_t   MainSDidentifier;
 IEEEaddress_t  EPID;
 //-----    ACcryptoID1;
 //---   LDC1; 
 IEEEaddress_t  IEEEaddress1;
 //-----    ACcryptoID2;
 //---   LDC2; 
 IEEEaddress_t  IEEEaddress2;
 //-----  MissingSubSDs;
}zclCmdASAC_TSreport_t;

enum                                       
{
   gTSreportStatus_RevokedNode_c = 0x00,    /* Revoked Node */
   gTSreportStatus_MisConfiguration_c,      /* MisConfiguration */
   gTSreportStatus_BogusTerminate_c,        /* Bogus Terminate */
   gTSreportStatus_IDconflict_c,            /* ID conflict */
   gTSreportStatus_FailedExchange_c,        /* Failed Exchange */  
};


/* Initiate ASAC Response command Payload */

typedef struct 
{

 unsigned LDCfingerprintPresent      : 1; 
 unsigned SubSDpointersPresent       : 1;
 unsigned Reserved                   : 6; 
}InitASACresponse_Options_t;

typedef struct zclCmdASAC_InitiateASACresponse_tag
{
 InitASACresponse_Options_t  Options;
 //-----  ResponderACcryptoID;
 uint8_t  ResponderLDCfingerprint[16];
 uint8_t  ResponderEphemeralData[16];
 uint8_t  NumberOfSubSDpointers;
 //-----  SubSDpointers;
}zclCmdASAC_InitiateASACresponse_t;

/* Confirm ASAC key Response command Payload */

typedef struct zclCmdASAC_ConfirmASACkeyResponse_tag
{
 uint8_t HMAC[16];
}zclCmdASAC_ConfirmASACkeyResponse_t;

/* Terminate ASAC command Payload */




/* Generate AMK Response command Payload */

typedef struct 
{

 unsigned LDCfingerprintPresent      : 1; 
 unsigned Reserved                   : 7; 
}GenAMKresponse_Options_t;

typedef struct zclCmdASAC_GenerateAMKresponse_tag
{
 uint8_t Status;
 GenAMKresponse_Options_t Options;
 //----  ResponderADcryptoID;
 uint8_t ResponderLDCfingerprint[16];
 uint8_t NumberOfSubSDpointers;
 //----  SubSDpointers;
}zclCmdASAC_GenerateAMKresponse_t;

/* LDC transport Response command Payload */

typedef struct 
{

 unsigned OwnACcryptoIDpresent   : 1; 
 unsigned OwnLDCpresent          : 1; 
 unsigned Reserved               : 6; 
}LDCtransportResponse_Options_t;

typedef struct zclCmdASAC_LDCtransportResponse_tag
{
 LDCtransportResponse_Options_t  Options;
 //----  ADcryptoID;
 //----  LDC;
}zclCmdASAC_LDCtransportResponse_t;

/* Request Configuration command Payload */

typedef struct zclCmdASAC_RequestConfiguration_tag
{
 IEEEaddress_t  IEEEaddress;
 uint16_t       ShortAddress;
}zclCmdASAC_RequestConfiguration_t;

#ifdef __IAR_SYSTEMS_ICC__
#pragma pack()
#endif


#ifdef __IAR_SYSTEMS_ICC__
#pragma pack(1)
#endif
typedef struct zclSetAttrValue_tag
{
  uint8_t         ep;             /* endpoint*/
  zbClusterId_t   clusterId;      /* cluster ID */
  uint16_t        attrID;         /* attribut ID */
  uint8_t         attrSize;       /* attribut Size */
  uint8_t         valueAttr[1];      /* value */
} zclSetAttrValue_t;

#ifdef __IAR_SYSTEMS_ICC__
#pragma pack()
#endif

/******************************************************************************
*******************************************************************************
* Public functions prototypes
*******************************************************************************
******************************************************************************/

/* ask the other node to reset the endpoint */
/*zclStatus_t ZCL_BasicResetReq(zclBasicResetReq_t *pReq);*/
#define ZCL_BasicResetReq(pReq)

/* recall scene data for light/switch, etc... */
void HA_RecallScene(zbEndPoint_t endPoint, void *pData, zclSceneTableEntry_t *pScene);
uint8_t HA_ViewSceneData(zclSceneOtaData_t *pClusterData, zclSceneTableEntry_t *pScene);
zbStatus_t HA_AddScene(zclSceneOtaData_t *pClusterData,zclSceneTableEntry_t *pScene);
void HA_StoreScene(zbEndPoint_t endPoint, zclSceneTableEntry_t *pScene);
void ZCL_ResetScenes(afDeviceDef_t *pDevice, zbGroupId_t groupId);

/* identify commands */
void ZCL_SetIdentifyMode(zbEndPoint_t endPoint, uint16_t time);

uint16_t ZCL_IdentifyTimeLeft(zbEndPoint_t endPoint);

/* Commissioning cluster commands */
#if gASL_ZclCommissioningRestartDeviceRequest_d
zbStatus_t  ZCL_Commisioning_RestartDeviceReq(zclCommissioningRestartDeviceReq_t *pReq);
#else
#define ZCL_Commisioning_RestartDeviceReq(pReq) FALSE
#endif


#if gASL_ZclCommissioningSaveStartupParametersRequest_d
zbStatus_t  ZCL_Commisioning_SaveStartupParametersReq(zclCommissioningSaveStartupParametersReq_t *pReq);
#else
#define ZCL_Commisioning_SaveStartupParametersReq(pReq) FALSE
#endif


#if gASL_ZclCommissioningRestoreStartupParametersRequest_d
zbStatus_t  ZCL_Commisioning_RestoreStartupParametersReq(zclCommissioningRestoreStartupParametersReq_t *pReq);
#else
#define ZCL_Commisioning_RestoreStartupParametersReq(pReq) FALSE
#endif

#if gASL_ZclCommissioningResetStartupParametersRequest_d
zbStatus_t  ZCL_Commisioning_ResetStartupParametersReq(zclCommissioningResetStartupParametersReq_t *pReq);
#else
#define ZCL_Commisioning_ResetStartupParametersReq(pReq) FALSE
#endif




/* cluster servers */
zbStatus_t ZCL_BasicClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice);
zbStatus_t ZCL_PowerCfgClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice);
zbStatus_t ZCL_TempCfgClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice);
zbStatus_t ZCL_BinaryInputClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice);
zbStatus_t ZCL_GroupClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice);
zbStatus_t ZCL_GroupClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice);
zbStatus_t ZCL_IdentifyClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice);
zbStatus_t ZCL_IdentifyClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice);
zbStatus_t ZCL_SceneClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice);
zbStatus_t ZCL_SceneClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice);
zbStatus_t ZCL_OnOffClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice);
zbStatus_t ZCL_LevelOnOffClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice);
zbStatus_t ZCL_LevelControlClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice);
zbStatus_t ZCL_OnOffSwitchClusterServer( zbApsdeDataIndication_t *pIndication,afDeviceDef_t *pDevice);
zbStatus_t ZCL_CommisioningClusterServer( zbApsdeDataIndication_t *pIndication,afDeviceDef_t *pDevice);
zbStatus_t ZCL_CommisioningClusterClient( zbApsdeDataIndication_t *pIndication,afDeviceDef_t *pDevice);

zbStatus_t ZCL_GenericReqNoData(afAddrInfo_t *pAddrInfo, zclCmd_t command);
zbStatus_t ZCL_GenericReqNoDataServer(afAddrInfo_t *pAddrInfo, zclCmd_t command);

/***** Alarm cluster servers and commands functions ******/
zbStatus_t ZCL_AlarmsClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev);
zbStatus_t ZCL_AlarmsClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev);

zbStatus_t Alarms_ResetAlarm(zclAlarmInformation_ResetAlarm_t *pReq);
void ResetAlarm(zclCmdAlarmInformation_ResetAlarm_t CmdResetAlarm, uint8_t endpoint);
zbStatus_t Alarms_ResetAllAlarms(zclAlarmInformation_NoPayload_t *pReq);
void ResetAllAlarm(uint8_t endpoint);
zbStatus_t Alarms_GetAlarm(zclAlarmInformation_NoPayload_t *pReq);
zbStatus_t Alarms_ResetAlarmLog(zclAlarmInformation_NoPayload_t *pReq);
zbStatus_t Alarms_Alarm(zclAlarmInformation_Alarm_t *pReq);
zbStatus_t Alarms_GetAlarmResponse(zclAlarms_GetAlarmResponse_t *pReq);
/**********************************************************/


/***** RSSI Location cluster servers and commands functions ******/
zbStatus_t ZCL_RSSILocationClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev);
zbStatus_t ZCL_RSSILocationClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev);


zbStatus_t RSSILocation_SetAbsoluteLocation(zclCmdRSSI_AbsoluteLocation_t *pReq);
zbStatus_t RSSILocation_SetDeviceConfiguration(zclCmdRSSI_SetDeviceConfiguration_t *pReq);
zbStatus_t RSSILocation_GetDeviceConfiguration(zclCmdRSSI_GetDeviceConfiguration_t *pReq);
zbStatus_t RSSILocation_GetLocationData(zclCmdRSSI_GetLocationData_t *pReq);

zbStatus_t RSSILocation_DeviceConfigurationResponse(zclCmdRSSI_DeviceConfigurationResponse_t *pReq);
zbStatus_t RSSILocation_LocationDataResponse(zclCmdRSSI_LocationDataResponse_t *pReq);
zbStatus_t RSSILocation_LocationDataNotification(zclCmdRSSI_LocationDataNotification_t *pReq);
zbStatus_t RSSILocation_CompactLocationDataNotification(zclCmdRSSI_CompactLocationDataNotification_t *pReq);
zbStatus_t RSSILocation_RSSIping(zclCmdRSSI_RSSIping_t *pReq);
/**********************************************************/

/***** Partitioning cluster servers and commands functions ******/
zbStatus_t ZCL_PartitioningClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev);
zbStatus_t ZCL_PartitioningClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev);


zbStatus_t Partitioning_TransferPartitionedFrame(zclCmdPartitioning_TransferPartitionedFrame_t *pReq);
zbStatus_t Partitioning_ReadHandshakeParam(zclCmdPartitioning_ReadHandshakeParam_t *pReq);
zbStatus_t Partitioning_WriteHandshakeParam(zclCmdPartitioning_WriteHandshakeParam_t *pReq);

zbStatus_t Partitioning_MultipleACK(zclCmdPartitioning_MultipleACK_t *pReq);
zbStatus_t Partitioning_ReadHandshakeParamResponse(zclCmdPartitioning_ReadHandshakeParamResponse_t *pReq);
/**********************************************************/

/***** ASKE cluster servers and commands functions ******/
zbStatus_t ZCL_ASKEClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev);
zbStatus_t ZCL_ASKEClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev);


zbStatus_t ASKE_ConfigureSD(zclCmdASKE_ConfigureSD_t *pReq);
zbStatus_t ASKE_UpdateRevocationList(zclCmdASKE_UpdateRevocationList_t *pReq);
zbStatus_t ASKE_RemoveSD(zclCmdASKE_RemoveSD_t *pReq);
zbStatus_t ASKE_ReadSD(zclCmdASKE_ReadSD_t *pReq);
zbStatus_t ASKE_InitiateASKE(zclCmdASKE_InitiateASKE_t *pReq);
zbStatus_t ASKE_ConfirmASKEkey(zclCmdASKE_ConfirmASKEkey_t *pReq);
zbStatus_t ASKE_TerminateASKE(zclCmdASKE_TerminateASKE_t *pReq);
zbStatus_t ASKE_GenerateAMK(zclCmdASKE_GenerateAMK_t *pReq);

zbStatus_t ASKE_ConfigureSDresponse(zclCmdASKE_ConfigureSDresponse_t *pReq);
zbStatus_t ASKE_UpdateRevocationListResponse(zclCmdASKE_UpdateRevocationListResponse_t *pReq);
zbStatus_t ASKE_RemoveSDresponse(zclCmdASKE_RemoveSDresponse_t *pReq);
zbStatus_t ASKE_ReadSDresponse(zclCmdASKE_ReadSDresponse_t *pReq);
zbStatus_t ASKE_InitiateASKEresponse(zclCmdASKE_InitiateASKEresponse_t *pReq);
zbStatus_t ASKE_ConfirmASKEkeyResponse(zclCmdASKE_ConfirmASKEkeyResponse_t *pReq);
//zbStatus_t ASKE_TerminateASKE(zclCmdASKE_TerminateASKE_t *pReq);
zbStatus_t ASKE_GenerateAMKresponse(zclCmdASKE_GenerateAMKresponse_t *pReq);
zbStatus_t ASKE_ReportRevokedNode(zclCmdASKE_ReportRevokedNode_t *pReq);
zbStatus_t ASKE_RequestConfiguration(zclCmdASKE_RequestConfiguration_t *pReq);
/**********************************************************/

/***** ASAC cluster servers and commands functions ******/
zbStatus_t ZCL_ASACClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev);
zbStatus_t ZCL_ASACClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev);


zbStatus_t ASAC_ConfigureSD(zclCmdASAC_ConfigureSD_t *pReq);
zbStatus_t ASAC_UpdateRevocationList(zclCmdASAC_UpdateRevocationList_t *pReq);
zbStatus_t ASAC_ConfigureACpolicies(zclCmdASAC_ConfigureACpolicies_t *pReq);
zbStatus_t ASAC_ReadSD(zclCmdASAC_ReadSD_t *pReq);
zbStatus_t ASAC_ReadACpolicies(zclCmdASAC_ReadACpolicies_t *pReq);
zbStatus_t ASAC_RemoveAC(zclCmdASAC_RemoveAC_t *pReq);
zbStatus_t ASAC_ACpropertiesRequest(void);
zbStatus_t ASAC_TSReportResponse(void);
zbStatus_t ASAC_InitiateASAC(zclCmdASAC_InitiateASAC_t *pReq);
zbStatus_t ASAC_ConfirmASACkey(zclCmdASAC_ConfirmASAC_t *pReq);
zbStatus_t ASAC_TerminateASAC(zclCmdASAC_TerminateASAC_t *pReq);
zbStatus_t ASAC_GenerateAMK(zclCmdASAC_GenerateAMK_t *pReq);
zbStatus_t ASAC_LDCtransport(zclCmdASAC_LDCtransport_t *pReq);

zbStatus_t ASAC_ConfigureSDresponse(zclCmdASAC_ConfigureSDresponse_t *pReq);
zbStatus_t ASAC_UpdateRevocationListResponse(zclCmdASAC_UpdateRevocationListResponse_t *pReq);
zbStatus_t ASAC_ConfigureACpoliciesResponse(zclCmdASAC_ConfigureACpoliciesResponse_t *pReq);
zbStatus_t ASAC_ReadSDresponse(zclCmdASAC_ReadSDresponse_t *pReq);
zbStatus_t ASAC_ReadACpoliciesResponse(zclCmdASAC_ReadACpoliciesResponse_t *pReq);
zbStatus_t ASAC_RemoveACresponse(zclCmdASAC_RemoveACresponse_t *pReq);
zbStatus_t ASAC_ACpropertiesResponse(zclCmdASAC_ACpropertiesResponse_t *pReq);
zbStatus_t ASAC_TSReport(zclCmdASAC_TSreport_t *pReq);
zbStatus_t ASAC_InitiateASACresponse(zclCmdASAC_InitiateASACresponse_t *pReq);
zbStatus_t ASAC_ConfirmASACkeyResponse(zclCmdASAC_ConfirmASACkeyResponse_t *pReq);
//zbStatus_t ASAC_TerminateASAC(zclCmdASAC_TerminateASAC_t *pReq);
zbStatus_t ASAC_GenerateAMKresponse(zclCmdASAC_GenerateAMKresponse_t *pReq);
zbStatus_t ASAC_LDCtransportResponse(zclCmdASAC_LDCtransportResponse_t *pReq);
zbStatus_t ASAC_RequestConfiguration(zclCmdASAC_RequestConfiguration_t *pReq);

#if gZclClusterOptionals_d
zbStatus_t ZCL_SetValueAttr(zclSetAttrValue_t *pSetAttrValueReq);
void TmrOccupancyCallBack(tmrTimerID_t tmrid);
#endif
bool_t InterpretMatchDescriptor(zbNwkAddr_t  aDestAddress, uint8_t endPoint);
/**********************************************************/
#endif /*ZCL_GENERAL_H*/
