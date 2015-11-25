/******************************************************************************
* ZclGeneral.c
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
******************************************************************************/
#ifndef __IAR_SYSTEMS_ICC__
#ifdef MC13226Included_d
    #undef MC13226Included_d
#endif    
    #define MC13226Included_d 0
#endif
#include "EmbeddedTypes.h"
#include "zigbee.h"
#include "FunctionLib.h"
#include "BeeStackConfiguration.h"
#include "BeeStack_Globals.h"
#include "BeeStackParameters.h"
#include "AppAfInterface.h"
#include "AfApsInterface.h"
#include "BeeStackInterface.h"
#include "TMR_Interface.h"
#include "ZdoApsInterface.h"
#include "ApsMgmtInterface.h"
#include "BeeAppInit.h"
#include "BeeCommon.h"
#include "HaProfile.h"
#include "ASL_UserInterface.h"
#include "BeeApp.h"
#include "ZCL.h"
#include "ZdpManager.h"
#include "ZclClosures.h"
#include "zclSensing.h"
#include "zclOta.h"
/******************************************************************************
*******************************************************************************
* Private macros
*******************************************************************************
******************************************************************************/

/* Basic Cluster Attribute Defaults, not for setting by the OEM */
/* See ZclOptions.h for the list of OEM settable attributes */
#define gHaAttrBasicZCLVersion_c          0x01
#define gHaAttrBasic_StackVersion_c       0x02
#define gZclLevel_OnOffCmd_c              0x01
#define gZclLevel_NotOnOffCmd_c           0x00
#define gZclLevel_UndefinedOnLevel_c      0xFF
#define gZclGroup_NameSupport_c           0x00  /* Group Names not supported */
#if gZclIncludeSceneName_d
#define gZclScene_NameSupport_c           0x01  /* Scene Names supported */
#else
#define gZclScene_NameSupport_c           0x00 /* Scene Names not supported */
#endif
#define DummyEndPoint                     0xFF
#define gpGroupTable gaApsGroupTable

bool_t gShadeDevice = FALSE;
gZclShadeStatus_t   statusShadeCfg = {0,0,0,0,0};
bool_t gShadeDeviceActive = FALSE;
/******************************************************************************
*******************************************************************************
* Private prototypes
*******************************************************************************
******************************************************************************/
zclSceneTableEntry_t *ZCL_AllocateScene(afDeviceDef_t *pDevice, zclCmdScene_StoreScene_t *pSceneId);
zclSceneTableEntry_t *ZCL_FindScene(afDeviceDef_t *pDevice, zclCmdScene_RecallScene_t *pSceneId);
zbStatus_t ZCL_GroupAddGroupRsp(zbApsdeDataIndication_t *pIndication,zclCmdGroup_AddGroup_t*command);
zbStatus_t ZCL_GroupAddGroupIfIdentify(zbApsdeDataIndication_t *pIndication,zclCmdGroup_AddGroupIfIdentifying_t*command);
zbStatus_t ZCL_GroupRemoveGroupRsp(zbApsdeDataIndication_t *pIndication,zclCmdGroup_RemoveGroup_t*command);
zbStatus_t ZCL_GroupRemoveAllGroups(zbApsdeDataIndication_t *pIndication,zclCmdGroup_RemoveAllGroups_t*command);
zclStatus_t ZCL_GroupViewGroupRsp(zbApsdeDataIndication_t *pIndication,zclCmdGroup_ViewGroup_t* command);
zclStatus_t ZCL_GroupGetGroupMembershipRsp(zbApsdeDataIndication_t *pIndication,zclCmdGroup_GetGroupMembership_t* command);
zbStatus_t ZCL_AddScene(zclSceneTableEntry_t *pScene, zclCmdScene_AddScene_t *pAddSceneReq, uint8_t indicationLen);
zbStatus_t ZCL_ViewScene(zclSceneTableEntry_t *pScene,zbApsdeDataIndication_t *pIndication,afDeviceDef_t *pDevice);
void ZCL_GetSceneMembership(afDeviceDef_t *pDevice, zbGroupId_t aGroupId,zbApsdeDataIndication_t *pIndication);
zbStatus_t ZCL_LevelControlMoveToLevel(zclCmdLevelControl_MoveToLevel_t * pReq, bool_t withOnOff);
zbStatus_t ZCL_LevelControlMove(zclCmdLevelControl_Move_t * pReq, bool_t withOnOff);
zclStatus_t ZCL_LevelControlStep(zclCmdLevelControl_Step_t *pReq, bool_t withOnOff);
zclStatus_t ZCL_LevelControlStop(void);
/******************************************************************************
*******************************************************************************
* Private type definitions
*******************************************************************************
******************************************************************************/

/* for handling OTA group commands */
typedef union zclAnyGroupReq_tag {
  zbApsmeAddGroupReq_t          addGroup;
  zbApsmeRemoveGroupReq_t       removeGroup;
  zbApsmeRemoveAllGroupsReq_t   removeAllGroups;
} zclAnyGroupReq_t;


/******************************************************************************
*******************************************************************************
* Public memory definitions
*******************************************************************************
******************************************************************************/
#if MC13226Included_d
    extern index_t gApsMaxBindingEntries;
    extern uint8_t gApsMaxGroups;
#else
    extern const index_t gApsMaxBindingEntries;
    extern const uint8_t gApsMaxGroups;
#endif

tmrTimerID_t occupancyTimer;
bool_t gSetAvailableOccupancy = FALSE;
/******************************************************************************
*******************************************************************************
* Private memory declarations
*******************************************************************************
******************************************************************************/

/******************************
  Basic Cluster Data
  See ZCL Specification Section 3.2
*******************************/
/* Manufacturer, Icon#, DeviceID, Device #, Reserved */
#if gZclClusterOptionals_d
const char gszZclBasicAttrMfgName[] = gszZclAttrBasic_MfgName_c;
const char gszZclBasicAttrModel[] = gszZclAttrBasic_Model_c;
const char gszZclBasicAttrDateCode[] = gszZclAttrBasic_DateCode_c;
#endif

// zclStr16_t  gZclBasicAttr_Location;
// uint8_t     gZclBasicAttr_PhysicalEnvironment;
// uint8_t     gZclBasicAttr_AlarmMask;
zclCommonAttr_t  gZclCommonAttr = {
#if gZclClusterOptionals_d  
  0x09,
  'F','r','e','e','s','c','a','l','e',' ',' ',' ',' ',' ',' ',' ',
  gZclAttrBasic_PhysicalEnvironment,
  0x00,
#endif  
  0x01
};

uint16_t gZclIdentifyTime;

/* Basic Cluster Attribute Definitions */
const zclAttrDef_t gaZclBasicClusterAttrDef[] = {
  { gZclAttrBasic_ZCLVersionId_c,          gZclDataTypeUint8_c, gZclAttrFlagsInLine_c, sizeof(uint8_t), { (void *)gHaAttrBasicZCLVersion_c } },
#if gZclClusterOptionals_d
  { gZclAttrBasic_ApplicationVersionId_c,  gZclDataTypeUint8_c, gZclAttrFlagsInLine_c, sizeof(uint8_t), { (void *)gZclAttrBasic_ApplicationVersion_c}  },
  { gZclAttrBasic_StackVersionId_c,        gZclDataTypeUint8_c, gZclAttrFlagsInLine_c, sizeof(uint8_t), { (void *)gHaAttrBasic_StackVersion_c } },
  { gZclAttrBasic_HWVersionId_c,           gZclDataTypeUint8_c, gZclAttrFlagsInLine_c, sizeof(uint8_t), { (void *)gZclAttrBasic_HWVersion_c } },
  { gZclAttrBasic_ManufacturerNameId_c,    gZclDataTypeStr_c,   gZclAttrFlagsInROM_c,  sizeof(gszZclBasicAttrMfgName)-1, { (void *)gszZclBasicAttrMfgName}  },
  { gZclAttrBasic_ModelIdentifierId_c,     gZclDataTypeStr_c,   gZclAttrFlagsInROM_c,  sizeof(gszZclBasicAttrModel)-1, { (void *)gszZclBasicAttrModel } },
  { gZclAttrBasic_DateCodeId_c,            gZclDataTypeStr_c,   gZclAttrFlagsInROM_c,  sizeof(gszZclBasicAttrDateCode)-1, { (void *)gszZclBasicAttrDateCode } },
#endif
  { gZclAttrBasic_PowerSourceId_c,         gZclDataTypeEnum8_c, gZclAttrFlagsInLine_c, sizeof(uint8_t), { (void *)gZclAttrBasicPowerSource_Mains_c } },
#if gZclClusterOptionals_d
  { gZclAttrBasic_LocationDescriptionId_c, gZclDataTypeStr_c, gZclAttrFlagsCommon_c, sizeof(zclStr16_t), { &gZclCommonAttr.basicLocation } },
  { gZclAttrBasic_PhysicalEnvironmentId_c, gZclDataTypeEnum8_c, gZclAttrFlagsCommon_c, sizeof(uint8_t), { &gZclCommonAttr.basicPhysicalEnvironment } },
#endif
  { gZclAttrBasic_DeviceEnabledId_c,       gZclDataTypeBool_c, gZclAttrFlagsCommon_c, sizeof(uint8_t), { &gZclCommonAttr.basicDeviceEnabled } }
#if gZclClusterOptionals_d
  , { gZclAttrBasic_AlarmMaskId_c,         gZclDataTypeBitmap8_c, gZclAttrFlagsCommon_c, sizeof(uint8_t), { &gZclCommonAttr.basicAlarmMask } }
#endif
};

const zclAttrDefList_t gZclBasicClusterAttrDefList = {
  NumberOfElements(gaZclBasicClusterAttrDef),
  gaZclBasicClusterAttrDef
};

/* Basic Cluster Attribute Definitions */
const zclAttrDef_t gaZclBasicClusterMirrorAttrDef[] = {
  { gZclAttrBasic_ZCLVersionId_c,          gZclDataTypeUint8_c, gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(uint8_t),       (void *)MbrOfs(seESIMirrorBasicData_t, ZCLVersion) },
  { gZclAttrBasic_PowerSourceId_c,         gZclDataTypeEnum8_c, gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(uint8_t),       (void *)MbrOfs(seESIMirrorBasicData_t, iPowerSource) },
};

const zclAttrDefList_t gZclBasicClusterMirrorAttrDefList = {
  NumberOfElements(gaZclBasicClusterMirrorAttrDef),
  gaZclBasicClusterMirrorAttrDef
};

/******************************
  Power Configuration Cluster Data
  See ZCL Specification Section 3.3
*******************************/
#if gZclClusterOptionals_d
/* Power Configuration Cluster Attribute Definitions */

zclPowerCfgAttrsRAM_t gZclPowerCfgAttrs =
{
  0x0000, /*gZclAttrPwrConfigMainsInfMainsVoltage_c*/
  0x00, /*gZclAttrPwrConfigMainsInfMainsFrequency_c*/
  0x00, /*gZclAttrPwrConfigMainsStgMainsAlarmMask_c*/
  0x0000, /*gZclAttrPwrConfigMainsStgMainsVoltageMinThreshold_c*/
  0x0000, /*gZclAttrPwrConfigMainsStgMainsVoltageMaxThreshold_c*/
  0x0000, /*gZclAttrPwrConfigMainsStgMainsVoltageDwellTripPoint_c*/
  0x00, /*gZclAttrPwrConfigBatteryInfBatteryVoltage_c*/
  0x09, 'F','r','e','e','s','c','a','l','e',' ',' ',' ',' ',' ',' ',' ', /*gZclAttrPwrConfigBatteryStgBatteryManufacturer_c*/
  0x00, /*gZclAttrPwrConfigBatteryStgBatterySize_c*/
  0x0000, /*gZclAttrPwrConfigBatteryStgBatteryAHrRating_c*/ 
  0x00, /*gZclAttrPwrConfigBatteryStgBatteryQuantity_c*/
  0x00, /*gZclAttrPwrConfigBatteryStgBatteryRatedVoltage_c*/
  0x00, /*gZclAttrPwrConfigBatteryStgBatteryAlarmMask_c*/
  0x00, /*gZclAttrPwrConfigBatteryStgBatteryVoltageMinThreshold_c*/
};

const zclAttrDef_t gaZclPowerCfgClusterAttrDef[] = {
   //mains information
  { gZclAttrPwrConfigMainsInfMainsVoltage_c,    gZclDataTypeUint16_c, gZclAttrFlagsCommon_c ,  sizeof(uint16_t), (void *)&gZclPowerCfgAttrs.mainsVoltage},
  { gZclAttrPwrConfigMainsInfMainsFrequency_c,  gZclDataTypeUint8_c,  gZclAttrFlagsCommon_c ,    sizeof(uint8_t),(void *)&gZclPowerCfgAttrs.mainsFrequency},
  //mains settings
  { gZclAttrPwrConfigMainsStgMainsAlarmMask_c,             gZclDataTypeBitmap8_c,  gZclAttrFlagsCommon_c,    sizeof(uint8_t), (void *)&gZclPowerCfgAttrs.mainsAlarmMask},
  { gZclAttrPwrConfigMainsStgMainsVoltageMinThreshold_c,   gZclDataTypeUint16_c,   gZclAttrFlagsCommon_c,    sizeof(uint16_t),(void *)&gZclPowerCfgAttrs.mainsVoltageMinThreshold},
  { gZclAttrPwrConfigMainsStgMainsVoltageMaxThreshold_c,   gZclDataTypeUint16_c,   gZclAttrFlagsCommon_c,    sizeof(uint16_t),(void *)&gZclPowerCfgAttrs.mainsVoltageMaxThreshold},
  { gZclAttrPwrConfigMainsStgMainsVoltageDwellTripPoint_c, gZclDataTypeUint16_c,   gZclAttrFlagsCommon_c,    sizeof(uint16_t),(void *)&gZclPowerCfgAttrs.mainsVoltageDwellTripPoint},
  //battery information
  { gZclAttrPwrConfigBatteryInfBatteryVoltage_c,    gZclDataTypeUint8_c,  gZclAttrFlagsCommon_c ,    sizeof(uint8_t), (void *)&gZclPowerCfgAttrs.batteryVoltage},
  //battery settings
  { gZclAttrPwrConfigBatteryStgBatteryManufacturer_c,       gZclDataTypeStr_c,     gZclAttrFlagsCommon_c,   sizeof(zclStr16_t), (void *)&gZclPowerCfgAttrs.batteryMfg},
  { gZclAttrPwrConfigBatteryStgBatterySize_c,               gZclDataTypeEnum8_c,   gZclAttrFlagsCommon_c,   sizeof(uint8_t),    (void *)&gZclPowerCfgAttrs.batterySize},
  { gZclAttrPwrConfigBatteryStgBatteryAHrRating_c,          gZclDataTypeUint16_c,  gZclAttrFlagsCommon_c,   sizeof(uint16_t),   (void *)&gZclPowerCfgAttrs.batteryAHrRating},
  { gZclAttrPwrConfigBatteryStgBatteryQuantity_c,           gZclDataTypeUint8_c,   gZclAttrFlagsCommon_c,   sizeof(uint8_t),    (void *)&gZclPowerCfgAttrs.batteryQuantity},
  { gZclAttrPwrConfigBatteryStgBatteryRatedVoltage_c,       gZclDataTypeUint8_c,   gZclAttrFlagsCommon_c,   sizeof(uint8_t),    (void *)&gZclPowerCfgAttrs.batteryRatedVoltage},
  { gZclAttrPwrConfigBatteryStgBatteryAlarmMask_c,          gZclDataTypeBitmap8_c, gZclAttrFlagsCommon_c,   sizeof(uint8_t),    (void *)&gZclPowerCfgAttrs.batteryAlarmMask},
  { gZclAttrPwrConfigBatteryStgBatteryVoltageMinThreshold_c,gZclDataTypeUint8_c,   gZclAttrFlagsCommon_c,   sizeof(uint8_t),    (void *)&gZclPowerCfgAttrs.batteryVoltageMinThreshold} 

};

const zclAttrDefList_t gZclPowerCfgClusterAttrDefList = {
  NumberOfElements(gaZclPowerCfgClusterAttrDef),
  gaZclPowerCfgClusterAttrDef
};
#endif  

/******************************
  Device Temperature Configuration Cluster Data
  See ZCL Specification Section 3.4
*******************************/

/* Device Temperature Configuration Cluster Attribute Definitions */
const zclAttrDef_t gaZclTemperatureCfgClusterAttrDef[] = {
  //device temperature information 
  { gZclAttrTempCfgTempInfCurrentTemp_c, gZclDataTypeInt16_c, gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(int16_t), (void *)MbrOfs(zclTempCfgAttrsRAM_t, currentTemp)}
#if gZclClusterOptionals_d
  , { gZclAttrTempCfgTempInfMinTempExperienced_c, gZclDataTypeInt16_c, gZclAttrFlagsInRAM_c, sizeof(int16_t), (void *)MbrOfs(zclTempCfgAttrsRAM_t, minTempExperienced)},
  { gZclAttrTempCfgTempInfMaxTempExperienced_c,   gZclDataTypeInt16_c, gZclAttrFlagsInRAM_c, sizeof(int16_t), (void *)MbrOfs(zclTempCfgAttrsRAM_t, maxTempExperienced)},
  { gZclAttrTempCfgTempInfOverTempTotalDwell_c,   gZclDataTypeInt16_c, gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(int16_t), (void *)MbrOfs(zclTempCfgAttrsRAM_t, overTempTotalDwell)},
#endif  
  //device temperature settings
#if gZclClusterOptionals_d
  { gZclAttrTempCfgTempStgDeviceTempAlarmMask_c,    gZclDataTypeBitmap8_c, gZclAttrFlagsInRAM_c, sizeof(uint8_t),   (void *)MbrOfs(zclTempCfgAttrsRAM_t, deviceTempAlarmMask)},
  { gZclAttrTempCfgTempStgLowTempThreshold_c,       gZclDataTypeInt16_c,   gZclAttrFlagsInRAM_c, sizeof(int16_t),   (void *)MbrOfs(zclTempCfgAttrsRAM_t, lowTempThreshold)},
  { gZclAttrTempCfgTempStgHighTempThreshold_c,      gZclDataTypeInt16_c,   gZclAttrFlagsInRAM_c, sizeof(int16_t),   (void *)MbrOfs(zclTempCfgAttrsRAM_t, highTempThreshold)},
  { gZclAttrTempCfgTempStgLowTempDwellTripPoint_c,  gZclDataTypeUint24_c,  gZclAttrFlagsInRAM_c, sizeof(SumElem_t), (void *)MbrOfs(zclTempCfgAttrsRAM_t, lowTempDwellTripPoint)},
  { gZclAttrTempCfgTempStgHighTempDwellTripPoint_c, gZclDataTypeUint24_c,  gZclAttrFlagsInRAM_c, sizeof(SumElem_t), (void *)MbrOfs(zclTempCfgAttrsRAM_t, highTempDwellTripPoint)}
#endif 
};

const zclAttrDefList_t gZclTemperatureCfgClusterAttrDefList = {
  NumberOfElements(gaZclTemperatureCfgClusterAttrDef),
  gaZclTemperatureCfgClusterAttrDef
};

/******************************
  Identify Cluster Data
  See ZCL Specification Section 3.5
*******************************/

extern uint8_t gZclIdentifyTimerID;

/* note: only 1 identify at a time per node: this is to reduce # of timers required in BeeStack */
zbEndPoint_t gZclIdentifyEndPoint;

const zclAttrDef_t gaZclIdentifyClusterAttrDef[] = {
  { gZclAttrIdentify_TimeId_c,  gZclDataTypeUint16_c, gZclAttrFlagsCommon_c, sizeof(uint16_t), &gZclIdentifyTime },
};

const zclAttrDefList_t gZclIdentifyClusterAttrDefList = {
  NumberOfElements(gaZclIdentifyClusterAttrDef),
  gaZclIdentifyClusterAttrDef
};

/******************************
  Groups Cluster Data
  The groups cluster is concerned with management of the group table on a device.
  See ZCL Specification Section 3.6
*******************************/


const zclAttrDef_t gaZclGroupClusterAttrDef[] = {
  { gZclAttrGroup_NameSupport_c, gZclDataTypeBitmap8_c, gZclAttrFlagsInLine_c, sizeof(uint8_t), { (void *)gZclGroup_NameSupport_c } }
};

const zclAttrDefList_t gaZclGroupClusterAttrDefList = {
  NumberOfElements(gaZclGroupClusterAttrDef),
  gaZclGroupClusterAttrDef
};


/******************************
  Scenes Cluster Data
  See ZCL Specification Section 3.7
*******************************/

const zclAttrDef_t gaZclSceneClusterAttrDef[] = {
  { gZclAttrScene_SceneCount_c,       gZclDataTypeUint8_c,   gZclAttrFlagsInSceneData_c | gZclAttrFlagsInRAM_c, sizeof(uint8_t), (void *)MbrOfs(zclSceneAttrs_t, sceneCount)   },
  { gZclAttrScene_CurrentScene_c,     gZclDataTypeUint8_c,   gZclAttrFlagsInSceneData_c | gZclAttrFlagsInRAM_c, sizeof(uint8_t), (void *)MbrOfs(zclSceneAttrs_t, currentScene) },
  { gZclAttrScene_CurrentGroup_c,     gZclDataTypeUint16_c,  gZclAttrFlagsInSceneData_c | gZclAttrFlagsInRAM_c, sizeof(uint16_t),(void *)MbrOfs(zclSceneAttrs_t, currentGroup) },
  { gZclAttrScene_SceneValid_c,       gZclDataTypeBool_c,    gZclAttrFlagsInSceneData_c | gZclAttrFlagsInRAM_c, sizeof(uint8_t),  (void *)MbrOfs(zclSceneAttrs_t, sceneValid) },
  { gZclAttrScene_NameSupport_c,      gZclDataTypeBitmap8_c, gZclAttrFlagsInLine_c, sizeof(uint8_t), { (void *)gZclScene_NameSupport_c } }
#if gZclClusterOptionals_d
  , { gZclAttrScene_LastConfiguredBy_c, gZclDataTypeIeeeAddr_c, gZclAttrFlagsInSceneData_c | gZclAttrFlagsInRAM_c, sizeof(zbIeeeAddr_t),  (void *)MbrOfs(zclSceneAttrs_t, lastConfiguredBy) }
#endif
};

const zclAttrDefList_t gZclSceneClusterAttrDefList = {
  NumberOfElements(gaZclSceneClusterAttrDef),
  gaZclSceneClusterAttrDef
};

/******************************
  On/Off Cluster Data
  See ZCL Specification Section 3.8
*******************************/

/* used by OnOffLight, DimmingLight, and any other device that supports the OnOff cluster server */
const zclAttrDef_t gaZclOnOffClusterAttrDef[] = {
  { gZclAttrOnOff_OnOffId_c, gZclDataTypeBool_c, gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c| gZclAttrFlagsReportable_c | gZclAttrFlagsInSceneTable_c, sizeof(uint8_t), (void *)MbrOfs(zclOnOffAttrsRAM_t, onOff) }
};

const zclAttrDefList_t gZclOnOffClusterAttrDefList = {
  NumberOfElements(gaZclOnOffClusterAttrDef),
  gaZclOnOffClusterAttrDef
};

/******************************
  On/Off Switch Configure Cluster Data
  See ZCL Specification Section 3.9
*******************************/
/* used by OnOffSwitch,Configuration Tool , and any other device that supports the OnOff Switch Configure cluster server */
const zclAttrDef_t gaZclOnOffSwitchConfigureClusterAttrDef[] = {
  { gZclAttrOnOffSwitchCfg_SwitchType_c, gZclDataTypeEnum8_c, gZclAttrFlagsInRAM_c, sizeof(uint8_t), (void *)MbrOfs(zclOnOffSwitchCfgAttrsRAM_t, SwitchType) },
  { gZclAttrOnOffSwitchCfg_SwitchAction_c, gZclDataTypeEnum8_c, gZclAttrFlagsInRAM_c, sizeof(uint8_t), (void *)MbrOfs(zclOnOffSwitchCfgAttrsRAM_t, SwitchAction) }
};

const zclAttrDefList_t gZclOnOffSwitchConfigureClusterAttrDefList = {    
  NumberOfElements(gaZclOnOffSwitchConfigureClusterAttrDef),
  gaZclOnOffSwitchConfigureClusterAttrDef
};

/******************************
  Alarms Cluster Data
  See ZCL Specification Section 3.11
*******************************/
const zclAttrDef_t gaZclAlarmsClusterAttrDef[] = {
  { gZclAlarmInformation_AlarmCount_c, gZclDataTypeUint16_c, gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(uint16_t), (void *)MbrOfs(zclAlarmsAttrsRAM_t, count) },
};

const zclAttrDefList_t gZclAlarmsClusterAttrDefList = {    
  NumberOfElements(gaZclAlarmsClusterAttrDef),
  gaZclAlarmsClusterAttrDef
};


/******************************
  Level Control Cluster Data
  See ZCL Specification Section 3.10
*******************************/
uint8_t             gZclLevel_TransitionTimerID;   /* used for the Level control cluster commands*/
zclLevelValue_t     gZclCurrentLevelTmp = gZclLevel_off;
zclLevelValue_t     gZclNewCurrentLevel = gZclLevel_off;
zclLevelValue_t     gZclLevel_diference = 0;
zclLevelValue_t     gZclLevel_LastCurrentLevel=0;
zclCmd_t            gZclLevel_OnOffState = gZclCmdOnOff_Off_c;
uint16_t            gZclLevel_TimeBetweenChanges=0;
uint8_t             gZclLevel_MoveMode=0;
bool_t              gZclLevel_OnOff=FALSE;
bool_t              gZclLevel_DimmingStopReport=FALSE;
uint8_t             gZclLevel_Step=1;

zbEndPoint_t        gZcl_ep;
tmrTimeInMilliseconds_t gZclLevel_RemainingTimeInMilliseconds=0;
uint8_t       gZclLevelCmd = gZclLevel_NotOnOffCmd_c;


/* Level Cluster Attribute Definitions */
const zclAttrDef_t gaZclLevelCtrlClusterAttrDef[] = {
   { gZclAttrLevelControl_CurrentLevelId_c,        gZclDataTypeUint8_c, gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c|gZclAttrFlagsReportable_c |gZclAttrFlagsInSceneTable_c, sizeof(uint8_t), (void*)MbrOfs(zclLevelCtrlAttrsRAM_t,currentLevel)}
#if gZclClusterOptionals_d
  ,{ gZclAttrLevelControl_RemainingTimeId_c,       gZclDataTypeUint16_c, gZclAttrFlagsInRAM_c| gZclAttrFlagsRdOnly_c, sizeof(zbLevelCtrlTime_t),(void*)MbrOfs(zclLevelCtrlAttrsRAM_t,remainingTime)  }
  ,{ gZclAttrLevelControl_OnOffTransitionTimeId_c, gZclDataTypeUint16_c, gZclAttrFlagsInRAM_c, sizeof(zbLevelCtrlTime_t), (void*)MbrOfs(zclLevelCtrlAttrsRAM_t,onOffTransitionTime) }
  ,{ gZclAttrLevelControl_OnLevelId_c,             gZclDataTypeUint8_c, gZclAttrFlagsInRAM_c, sizeof(uint8_t), (void*)MbrOfs(zclLevelCtrlAttrsRAM_t,onLevel) }
#endif /* optionals */
};

const zclAttrDefList_t gZclLevelCtrlClusterAttrDefList = {
  NumberOfElements(gaZclLevelCtrlClusterAttrDef),
  gaZclLevelCtrlClusterAttrDef
};

/******************************************************************************
*******************************************************************************
* Public functions
*******************************************************************************
******************************************************************************/

/******************************
  Basic Cluster
  See ZCL Specification Section 3.2
*******************************/

/*****************************************************************************
* ZCL_ResetDevice
*
* Resets all attributes on this endpoint to their defaults.
* Cannot fail.
*****************************************************************************/
void ZCL_ResetDevice
  (
  afDeviceDef_t *pDevice  /* IN: device definition to reset */
  )
{
  if (pDevice->pData != NULL) {   
 //   *(uint8_t *)pDevice->pReportList = 0;   /* clear the reporting bits */
  }
}

/*****************************************************************************
* ZCL_BasicClusterServer
*
* Server for all the Basic Cluster commands.
* Response generator to a command request, the server will take the
* request and process it.
*****************************************************************************/
zbStatus_t ZCL_BasicClusterServer
(
	zbApsdeDataIndication_t *pIndication,
	afDeviceDef_t *pDevice
)
{
	/* avoid compiler warnings */
	(void)pIndication;
	(void)pDevice;

	return gZclUnsupportedClusterCommand_c;
}

/******************************
  Power Configuration Cluster
  See ZCL Specification Section 3.3
*******************************/
zbStatus_t ZCL_PowerCfgClusterServer
(
      zbApsdeDataIndication_t *pIndication, 
      afDeviceDef_t *pDevice
)
{
      (void) pIndication;
      (void) pDevice;
      
      return gZclUnsupportedClusterCommand_c;
}

/******************************
  Temperature Configuration Cluster
  See ZCL Specification Section 3.4
*******************************/
zbStatus_t ZCL_TempCfgClusterServer
(
      zbApsdeDataIndication_t *pIndication, 
      afDeviceDef_t *pDevice
)
{
      (void) pIndication;
      (void) pDevice;
      
     return gZclUnsupportedClusterCommand_c;
}

/******************************
  Binary Input(Basic) Cluster
  See ZCL Specification Section 3.4
*******************************/
zbStatus_t ZCL_BinaryInputClusterServer
(
      zbApsdeDataIndication_t *pIndication, 
      afDeviceDef_t *pDevice
)
{
      (void) pIndication;
      (void) pDevice;
      
     return gZclUnsupportedClusterCommand_c;
}

/******************************
  Identify Cluster
  See ZCL Specification Section 3.5
*******************************/


/******************************************************************************
* Identify Cluster Client, receives the  group cluster responses.
*
* IN: pIndication   pointer to indication. Must be non-null
* IN: pDevice       pointer to device definition. Must be non-null.
*
* Returns
*   gZbSuccess_c    worked
*   gZclUnsupportedClusterCommand_c   not supported response
*
******************************************************************************/
zbStatus_t ZCL_IdentifyClusterClient
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
  
  if (command == gZclCmdIdentifyQueryRsp_c)
    return gZbSuccess_c;
  else
    return gZclUnsupportedClusterCommand_c;
}


/*****************************************************************************
* ZCL_IdentifyClusterServer
*
* See ZCL Specification Section 3.5
* Response generator to a command request, the server will take the
* request and process it.
*****************************************************************************/
zbStatus_t ZCL_IdentifyClusterServer
(
	zbApsdeDataIndication_t *pIndication, /* IN: */
	afDeviceDef_t *pDevice                /* IN: */
)
{
	zclFrame_t *pFrame;
	uint16_t time;

	/* avoid compiler warnings */
	(void)pDevice;

	/* check command */
	pFrame = (void *)(pIndication->pAsdu);

  switch (pFrame->command) {
    case gZclCmdIdentify_c:
      ZCL_SetIdentifyMode(pIndication->dstEndPoint, (*((uint16_t *)(pFrame + 1))));
      return gZclSuccessDefaultRsp_c; 
    case gZclCmdIdentifyQuery_c:
	    /* respond back to caller if in identify mode */
	    if(gZclIdentifyTime) {
		    time = gZclIdentifyTime;
        pFrame->command = gZclCmdIdentifyQueryRsp_c;
		    return ZCL_Reply(pIndication, sizeof(time), &time);
	    }    
      return gZclSuccessDefaultRsp_c;
    default:
      return gZclUnsupportedClusterCommand_c;
    
  }
}

/*****************************************************************************
* ZCL_EnterIdentify
*
* Local command for entering identify mode for a period of time.
*****************************************************************************/
void ZCL_IdentifyTimer( uint8_t timerId )
{
	uint16_t time;

	/* avoid compiler warning */
	(void)timerId;

	/* decrement the time */
	if(gZclIdentifyTime && gZclIdentifyTime != 0xffff)
	{
		time = OTA2Native16(gZclIdentifyTime);
		--time;
		gZclIdentifyTime = Native2OTA16(time);
		if(!gZclIdentifyTime)
		{
			TMR_StopTimer(gZclIdentifyTimerID);
                        BeeAppUpdateDevice(gZclIdentifyEndPoint, gIdentifyOff_c, 0, 0, NULL);
		}
	}
}

/*****************************************************************************
* ZCL_SetIdentifyMode
*
* Local command for entering identify mode for a period of time. Set to 0xffff
* for forever. Set to 0x0000 to turn identify off.
*****************************************************************************/
void ZCL_SetIdentifyMode
(
	zbEndPoint_t endPoint,
	uint16_t time /* IN: in seconds */
)
{
	uint8_t event;

	/* set up identify mode */
	gZclIdentifyEndPoint = endPoint;
	gZclIdentifyTime = time;

	/* turn identify on */
	if(time)
	{
		event = gIdentifyOn_c;
       		TMR_StartTimer(gZclIdentifyTimerID, gTmrLowPowerIntervalMillisTimer_c, 1000 , ZCL_IdentifyTimer);
	}
	/* turn identify off */
	else
	{
		event = gIdentifyOff_c;
		TMR_StopTimer(gZclIdentifyTimerID);
	}

	/* let UI know we're now in/out of identify mode */
        BeeAppUpdateDevice(endPoint, event, 0, 0, NULL);
}

/*****************************************************************************
* ZCL_IdentifyTimeLeft
*
* Local command for determining time left in identify mode
*****************************************************************************/
uint16_t ZCL_IdentifyTimeLeft
(
	zbEndPoint_t endPoint
)
{
	(void)endPoint; /* avoid compiler warnings */
	return gZclIdentifyTime;
}

/******************************
  Groups Cluster
  See ZCL Specification Section 3.6
*******************************/
/******************************************************************************
* Groups Cluster Client, receives the  group cluster responses.
*
* IN: pIndication   pointer to indication. Must be non-null
* IN: pDevice       pointer to device definition. Must be non-null.
*
* Returns
*   gZbSuccess_c    worked
*   gZclUnsupportedClusterCommand_c   not supported response
*
******************************************************************************/
zbStatus_t ZCL_GroupClusterClient
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
    case gZclCmdGroup_AddGroupRsp_c:
    case gZclCmdGroup_ViewGroupRsp_c:
    case gZclCmdGroup_GetGroupMembershipRsp_c:
    case gZclCmdGroup_RemoveGroupRsp_c:
      return gZbSuccess_c;
  default:
      return gZclUnsupportedClusterCommand_c;
  }
}

/******************************************************************************
* Groups Cluster Server, receives the  group cluster command and creates the 
* correspondant APSME request.
*
* IN: pIndication   pointer to indication. Must be non-null
* IN: pDevice       pointer to device definition. Must be non-null.
*
* Returns
*   gZbSuccess_c    worked
*   gZdoNoMatch_c   Couldn't find the group
*
******************************************************************************/
zbStatus_t ZCL_GroupClusterServer
  (
  zbApsdeDataIndication_t *pIndication, 
  afDeviceDef_t *pDevice
  )
{
  zclFrame_t *pFrame;
  zclCmd_t command;
  zbStatus_t status = gZbSuccess_c;
  zbStatus_t (*pGroupFunc)(zbApsdeDataIndication_t *pIndication, void *command);  
  pGroupFunc = NULL;
  
  /* prevent compiler warning */
  (void)pDevice;

	pFrame = (void *)pIndication->pAsdu;

  /* handle the command */
  command = pFrame->command;
  
  switch(command) {
    /* add group if identifying */
  	case gZclCmdGroup_AddGroupIfIdentifying_c:
      pGroupFunc = (zbStatus_t(*)(zbApsdeDataIndication_t *pIndication, void *command))  ZCL_GroupAddGroupIfIdentify;  
      status = gZclSuccessDefaultRsp_c;
      break;

    /* add group to endpoint */
    case gZclCmdGroup_AddGroup_c:
      pGroupFunc = (zbStatus_t(*)(zbApsdeDataIndication_t *pIndication, void *command)) ZCL_GroupAddGroupRsp;
      break;

    /* view the group */
    case gZclCmdGroup_ViewGroup_c:
      pGroupFunc = (zbStatus_t(*)(zbApsdeDataIndication_t *pIndication, void *command)) ZCL_GroupViewGroupRsp;
      break;
    /* check what groups this endpoint belongs to */
  	case gZclCmdGroup_GetGroupMembership_c:
  	  pGroupFunc = (zbStatus_t(*)(zbApsdeDataIndication_t *pIndication, void *command)) ZCL_GroupGetGroupMembershipRsp;
	  	break;

    /* remove the group */
    case gZclCmdGroup_RemoveGroup_c:
      pGroupFunc = (zbStatus_t(*)(zbApsdeDataIndication_t *pIndication, void *command)) ZCL_GroupRemoveGroupRsp;
      break;

    /* remove all groups from the endpoint */
    case gZclCmdGroup_RemoveAllGroups_c:
      pGroupFunc = (zbStatus_t(*)(zbApsdeDataIndication_t *pIndication, void *command)) ZCL_GroupRemoveAllGroups;
      status = gZclSuccessDefaultRsp_c;
      break;
      
    default:
      return gZclUnsupportedClusterCommand_c;
    }
    if (pGroupFunc)
      (void)pGroupFunc(pIndication, (void *)(pFrame + 1));

  return status;
}


zbStatus_t ZCL_GroupAddGroupRsp
(
  zbApsdeDataIndication_t *pIndication,
  zclCmdGroup_AddGroup_t*command
)
{
	zclCmdGroup_AddGroupRsp_t Resp;
	uint8_t iPayloadLen;
	zbApsmeAddGroupReq_t  ApsmeAddGroup;


	Copy2Bytes(ApsmeAddGroup.aGroupId,command->aGroupId);
	Copy2Bytes(Resp.aGroupId,command->aGroupId);
	ApsmeAddGroup.endPoint = pIndication->dstEndPoint;

        if(ApsGroupIsMemberOfEndpoint(Resp.aGroupId,ApsmeAddGroup.endPoint))
        {
          Resp.status = gZclDuplicateExists_c;
        } else
        {
	  Resp.status = (zclStatus_t)APSME_AddGroupRequest(&ApsmeAddGroup);
	  if(gApsTableFull_c == Resp.status)
	    Resp.status = gZclInsufficientSpace_c;

        }
	iPayloadLen = sizeof(zclCmdGroup_AddGroupRsp_t);
	
	return ZCL_Reply(pIndication, iPayloadLen, &Resp);
}

zbStatus_t ZCL_GroupAddGroupIfIdentify
(
  zbApsdeDataIndication_t *pIndication,
  zclCmdGroup_AddGroupIfIdentifying_t*command
)
{
  if(gZclIdentifyTime) {
  	zbApsmeAddGroupReq_t  ApsmeAddGroup;
    Copy2Bytes(ApsmeAddGroup.aGroupId,command->aGroupId);
    ApsmeAddGroup.endPoint = pIndication->dstEndPoint;
  	return APSME_AddGroupRequest(&ApsmeAddGroup);
  }
  return gZbSuccess_c;
}

zbStatus_t ZCL_GroupRemoveGroupRsp
(
  zbApsdeDataIndication_t *pIndication,
  zclCmdGroup_RemoveGroup_t*command
)
{
  zclCmdGroup_RemoveGroupRsp_t Resp;
  zbApsmeRemoveGroupReq_t  ApsmeRemoveGroup;
  uint8_t iPayloadLen;

  Copy2Bytes(ApsmeRemoveGroup.aGroupId,command->aGroupId);
	ApsmeRemoveGroup.endPoint = pIndication->dstEndPoint;
  Resp.status = APSME_RemoveGroupRequest(&ApsmeRemoveGroup);
  if((gApsInvalidGroup_c == Resp.status) || (gApsInvalidParameter_c == Resp.status))
    Resp.status = gZclNotFound_c;  
  Copy2Bytes(Resp.aGroupId,command->aGroupId);

  iPayloadLen = sizeof(Resp);
  return ZCL_Reply(pIndication, iPayloadLen, &Resp);
}

zbStatus_t ZCL_GroupRemoveAllGroups
(
  zbApsdeDataIndication_t *pIndication,
  zclCmdGroup_RemoveAllGroups_t*command
)
{
  zbApsmeRemoveAllGroupsReq_t  ApsmeRemoveAllGroups;

  (void) command; /*To avoid compile errors*/
  ApsmeRemoveAllGroups.endPoint = pIndication->dstEndPoint;

  return APSME_RemoveAllGroupsRequest(&ApsmeRemoveAllGroups);
}

/*Find Group*/
zclStatus_t ZCL_FindGroup
(
  zbGroupId_t  aGroupId
)
{
	zclStatus_t  status;
	zbGroupId_t  InvalidGroup={0xF8,0xFF};
	int i;

	status = gZclInvalidValue_c;  /*In case the group to be search is 0x0000 or 0xFFF7*/	
	if (!IsEqual2Bytes(aGroupId,InvalidGroup) && !Cmp2BytesToZero(aGroupId))
	{
		status = gZclNotFound_c;
		for(i = 0; i < gApsMaxGroups; ++i)
		{
			if (!IsEqual2Bytes(gpGroupTable[i].aGroupId,aGroupId))
				/*If group is no the one we are looking then continue to next one in the list */
				continue;

			/*Only in case that the group is found then stop the loop*/
			status = gZbSuccess_c;
			break;
			}
		}
	return status;
}

/*
  Client for View Group, sends the packet OTA
*/

zclStatus_t ZCL_GroupViewGroupRsp
(
	zbApsdeDataIndication_t *pIndication,
	zclCmdGroup_ViewGroup_t*command
)
{
	zclCmdGroup_ViewGroupRsp_t Resp;
	uint8_t iPayloadLen;

	Resp.status = ZCL_FindGroup(command->aGroupId);
	/*
	The String Names are optionals in Ha, for size reasons in BeeStack are not supported
	instead a NULL empty string is send out all the time.
	*/
	Resp.szName[0] = 0x00;
	iPayloadLen = sizeof(zclCmdGroup_ViewGroupRsp_t);

	Copy2Bytes(Resp.aGroupId, command->aGroupId);

	return ZCL_Reply(pIndication, iPayloadLen, &Resp);
}

zbStatus_t ZCL_GroupGetGroupMembershipRsp
(
	zbApsdeDataIndication_t *pIndication,
	zclCmdGroup_GetGroupMembership_t*command
)
{
	zbIndex_t   iGroupEntry;
	zclCmdGroup_GetGroupMembershipRsp_t   *pResp;
	zbGroupTable_t *pPtr;
	afToApsdeMessage_t *pMsg;

	zbSize_t iPayloadLen;
	uint8_t limit;
	zbCounter_t  count;
	zbCounter_t  NumOfGroups = 0;


	pMsg = AF_MsgAlloc();
	if (!pMsg)
		return gZbNoMem_c;

	BeeUtilZeroMemory(pMsg, gMaxRxTxDataLength_c);
	pResp = (zclCmdGroup_GetGroupMembershipRsp_t *)(((uint8_t *)pMsg) + gAsduOffset_c + sizeof(zclFrame_t));

	count = 0;
	limit = (!(command->count))? 0:1;
	for (; command->count >= limit; command->count--)
	{
		for(iGroupEntry=0; iGroupEntry<gApsMaxGroups; ++iGroupEntry)
		{
			pPtr = &gpGroupTable[iGroupEntry];

			if (!Cmp2BytesToZero(pPtr->aGroupId)
             && ((!limit) || (IsEqual2Bytes(command->aGroupId[count], pPtr->aGroupId))))
			{
				// if the group matches the group table entry
				Copy2Bytes(pResp->aGroupId[pResp->count], pPtr->aGroupId);
				pResp->count++;
            NumOfGroups++;
			}			
		}

		if (!limit)
      {
			break;
      }
      else
      {
         count++;
      }
	}		

	pResp->capacity = gApsMaxGroups - NumOfGroups;

	iPayloadLen = MbrOfs(zclCmdGroup_GetGroupMembershipRsp_t,aGroupId[0])+(pResp->count * sizeof(zbClusterId_t));

	return ZCL_ReplyNoCopy(pIndication, iPayloadLen, pMsg);
}

/******************************
  Scenes Cluster
  See ZCL Specification Section 3.7
*******************************/
/******************************************************************************
* Scene Cluster Client, receives the  group cluster responses.
*
* IN: pIndication   pointer to indication. Must be non-null
* IN: pDevice       pointer to device definition. Must be non-null.
*
* Returns
*   gZbSuccess_c    worked
*   gZclUnsupportedClusterCommand_c   not supported response
*
******************************************************************************/
zbStatus_t ZCL_SceneClusterClient
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
/* responses to above commands */
  case gZclCmdScene_AddSceneRsp_c:
  case gZclCmdScene_ViewSceneRsp_c:
  case gZclCmdScene_RemoveSceneRsp_c:
  case gZclCmdScene_RemoveAllScenesRsp_c:
  case gZclCmdScene_StoreSceneRsp_c:
  case gZclCmdScene_GetSceneMembershipRsp_c:
    return gZbSuccess_c;
  default:
    return gZclUnsupportedClusterCommand_c;
  }
}
/*****************************************************************************
* ZCL_SceneClusterServer
*
* Handles scene server commands, including add and remove scene, store scene,
* etc...
*
* IN: pIndication   pointer to indication. Must be non-null
* IN: pDevice       pointer to device definition. Must be non-null.
*
* Returns gZbSuccess_c      if worked.
* Returns gZclNotFound_c    if scene not found.
*****************************************************************************/
zbStatus_t ZCL_SceneClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice)
{
  zclFrame_t      *pFrame;
  zclSceneTableEntry_t *pScene;          /* scene table entry */
  zclSceneReq_t   *pSceneReq;
  uint8_t         *pData;
  zclSceneAttrs_t *pSceneData;
  zclCmdScene_AddSceneRsp_t sceneRsp; /* generic short response (status, group, sceneID) */
  uint8_t         payloadLen;
  bool_t          fNoReply = FALSE;
  zbStatus_t      status = gZbSuccess_c;
  
#if  gZclClusterOptionals_d 
  uint8_t         ieeeAddress[8];
  uint8_t         *pSrcLongAddress =  APS_GetIeeeAddress(pIndication->aSrcAddr, ieeeAddress);
  bool_t          bModifyConfiguratorAddr = FALSE;  
#endif


  /* get ptr to device data for this endpoint */
  pData = pDevice->pData;
  pSceneData = (zclSceneAttrs_t*) pDevice->pSceneData;      

  /* get ptr to ZCL frame */
  pFrame = (void *)pIndication->pAsdu;

  /* get a pointer to the scene (add, view, restore, etc..) request portion of the zcl frame */
  pSceneReq = (zclSceneReq_t *)(pFrame + 1);

  /* create common reply */
  sceneRsp.status = gZbSuccess_c;
  FLib_MemCpy(&sceneRsp.aGroupId, pSceneReq, sizeof(sceneRsp) - sizeof(sceneRsp.status));
  payloadLen = sizeof(sceneRsp);
  
  
  if (pSceneData)
  {
    /* many commands need to know if the scene exists */
    pScene = ZCL_FindScene(pDevice, (void *)(pSceneReq));  
    if (ZCL_FindGroup(pSceneReq->addScene.aGroupId)!= gZbSuccess_c)
    {
      sceneRsp.status = gZclInvalidField_c;
      if (gZclCmdScene_RemoveAllScenes_c == pFrame->command)
        payloadLen -= sizeof(zclSceneId_t); /* don't send the sceneId for RemoveAllScenesCmd */  
    }
    else{
      /* handle the scene command */
      switch(pFrame->command)
      {
        /* Add a scene */
        case gZclCmdScene_AddScene_c:
    
          /* add the scene (or get the slot if it already exists) */
          pScene = ZCL_AllocateScene(pDevice, (void *)(pSceneReq));
    
          /* no room to add the scene */
          if(!pScene)
            sceneRsp.status = gZclInsufficientSpace_c;
    
          /* add the scene (similar to store scene) */
          else {          
            sceneRsp.status = ZCL_AddScene(pScene, &pSceneReq->addScene, pIndication->asduLength - sizeof(zclFrame_t));
            ZdoNwkMng_SaveToNvm(zdoNvmObject_ZclData_c);
            #if  gZclClusterOptionals_d   
            if (gZbSuccess_c == status) 
              bModifyConfiguratorAddr = TRUE;  
            #endif
          }
          break;
    
        /* View scene */
        case gZclCmdScene_ViewScene_c:
          if (pScene)
          {
            sceneRsp.status = ZCL_ViewScene(pScene, pIndication, pDevice);
          } else
          { 
            sceneRsp.status = gZclNotFound_c;
          }

          /* reply already sent if no error */
          if(sceneRsp.status == gZbSuccess_c)
            fNoReply = TRUE;
          break;
    
        /* Remove scene */
        case gZclCmdScene_RemoveScene_c:
    
          /* found, remove it */
          if(pScene) {
            BeeUtilZeroMemory(pScene, sizeof(zclSceneTableEntry_t));
            --(pSceneData->sceneCount);
            ZdoNwkMng_SaveToNvm(zdoNvmObject_ZclData_c);
          }
          else
            sceneRsp.status = gZclNotFound_c;
          break;
    
        /* Remove all scenes */
        case gZclCmdScene_RemoveAllScenes_c:             
          /* reset all the scenes with the specified group id */
          ZCL_ResetScenes(pDevice, sceneRsp.aGroupId);
          /* send back shortened response */
          payloadLen -= sizeof(zclSceneId_t);
          break;
    
        /* Store scene */
        case gZclCmdScene_StoreScene_c:
    
          /* (re)allocate the scene for storage */
          pScene = ZCL_AllocateScene(pDevice, (void *)pSceneReq);
    
          /* store current data to the scene */
          if(pScene) {
            /* copy the attributes to store */
            HA_StoreScene(pIndication->dstEndPoint, pScene);
            ZdoNwkMng_SaveToNvm(zdoNvmObject_ZclData_c);
            #if  gZclClusterOptionals_d   
            bModifyConfiguratorAddr = TRUE;  
            #endif
          }
          else {
            sceneRsp.status = gZclInsufficientSpace_c;
          }
          break;
    
        /* recall scene */
        case gZclCmdScene_RecallScene_c:
    
          /* recall scene data */
          if(pScene) {
            HA_RecallScene(pIndication->dstEndPoint, pData, pScene);
            pSceneData->sceneValid = gZclSceneValid_c;
            pSceneData->currentScene = pScene->sceneId;
            Copy2Bytes(pSceneData->currentGroup, pScene->aGroupId);
          }
  
          status = gZclSuccessDefaultRsp_c;
          fNoReply = TRUE;
          break;
    
        /* get scene membership */
        case gZclCmdScene_GetSceneMembership_c:
          ZCL_GetSceneMembership(pDevice, pSceneReq->getSceneMembership.aGroupId, pIndication);
          fNoReply = TRUE;
          break;
    
        /* command not supported on this cluster */
        default:
          status = gZclUnsupportedClusterCommand_c;
        }
      }
      
    #if  gZclClusterOptionals_d   
      /* If IEEE address of the source was not found set on 0xFFFFFFFFFFFF */    
      if (bModifyConfiguratorAddr) 
      {      
        if (pSrcLongAddress)
          FLib_MemCpy(&pSceneData->lastConfiguredBy, pSrcLongAddress, sizeof(zbIeeeAddr_t));
        else
          FLib_MemSet(&pSceneData->lastConfiguredBy, 0xFF, sizeof(zbIeeeAddr_t));
      }      
    #endif
  }  
  else
    /* pSceneData not available */
     status = gZclUnsupportedClusterCommand_c;  

  /* send a standard scene response */
  if(!pIndication->fWasBroadcast && (pIndication->dstAddrMode != gZbAddrModeGroup_c) && !fNoReply) {
    return ZCL_Reply(pIndication, payloadLen, &sceneRsp);
  }

  /* worked */
  return status;
}


/*****************************************************************************
* ZCL_ResetScenes
*
* Resets all scenes for the specified device that belong to groupId
*
*****************************************************************************/
void ZCL_ResetScenes(afDeviceDef_t *pDevice, zbGroupId_t groupId)
{  
  zclSceneAttrs_t *pSceneData = (zclSceneAttrs_t*) pDevice->pSceneData;
  zclSceneTableEntry_t *pSceneTableEntry;
  index_t i;
   
  for(i=0; i<gHaMaxScenes_c; ++i)
  {
    pSceneTableEntry = (zclSceneTableEntry_t *) ((uint8_t*)pDevice->pSceneData + 
                                                 sizeof(zclSceneAttrs_t) + 
                                                 i*pSceneData->sceneTableEntrySize);
    
    if (IsEqual2Bytes(groupId, pSceneTableEntry->aGroupId))
    {
      BeeUtilZeroMemory(pSceneTableEntry, sizeof(zclSceneTableEntry_t));        
      --(pSceneData->sceneCount);
    }    
  }
      
  
  ZdoNwkMng_SaveToNvm(zdoNvmObject_ZclData_c);
}

/*****************************************************************************
* ZCL_AddScene
*
* Internal add scene, when this device recieves and add-scene over the air.
*
* Returns status (worked or failed).
*****************************************************************************/
zbStatus_t ZCL_AddScene(zclSceneTableEntry_t *pScene, zclCmdScene_AddScene_t *pAddSceneReq, uint8_t indicationLen)
{
  uint8_t NameLen;
  uint8_t *pAddSceneData;
  zclSceneOtaData_t *pClusterData;
  uint8_t len;
  uint8_t *pData;

  len = MbrOfs(zclCmdScene_AddScene_t, szSceneName[0]);

  /* copy up through the name */
  FLib_MemCpy(pScene, pAddSceneReq, len);

  /* find the start of the cluster data (after the name) */
  pAddSceneData = (uint8_t *)&pAddSceneReq->szSceneName;
  NameLen = (*pAddSceneData);
#if gZclIncludeSceneName_d
  FLib_MemCpy(pScene->szSceneName, pAddSceneReq->szSceneName, NameLen+1);
#endif

  pData = (uint8_t*)(pAddSceneData + (NameLen+1));

  len +=(NameLen+1);
  
  /* no data to copy */
  if(indicationLen < len)
    return gZclMalformedCommand_c;
  
  
  NameLen = indicationLen - len;

/* Here starts the copying process */
  while(NameLen) 
  {
    pClusterData = (zclSceneOtaData_t*)pData;
    
    len = 3;
    
    //if(NameLen < sizeof(zclSceneOtaData_t) || pClusterData->length > NameLen)
    //  return gZclMalformedCommand_c;

     if (pClusterData->length > 0)
     {       
       if (HA_AddScene(pClusterData,pScene) != gZbSuccess_c)
         return gZclMalformedCommand_c;
       else
         len += pClusterData->length;
     }     
     /* get to the next extended field */ 
     pData += len;
     NameLen -= len;      
  }
  ZdoNwkMng_SaveToNvm(zdoNvmObject_ZclData_c);
  return gZbSuccess_c;
  
}

/*****************************************************************************
* ZCL_ViewScene
*
* Creates the payload for view scene command.
*****************************************************************************/
zbStatus_t ZCL_ViewScene
  (
  zclSceneTableEntry_t *pScene,        /* IN */
  zbApsdeDataIndication_t *pIndication, /* IN */
  afDeviceDef_t *pDevice  /* IN */
  )
{
  afToApsdeMessage_t *pMsg;
  uint8_t *pPayload;
  uint8_t payloadLen;

(void)pDevice;/*To avoid Compiler errors*/
  /* allocate space for the message */
  pMsg = AF_MsgAlloc();
  if(!pMsg)
    return gZclNoMem_c;

  /* build the reply */
  pPayload = ((uint8_t *)pMsg) + gAsduOffset_c + sizeof(zclFrame_t);

  /* copy in fixed portion of scene */
  payloadLen = MbrOfs(zclCmdScene_AddScene_t, szSceneName);
  FLib_MemCpy(pPayload + 1, pScene, payloadLen);

  /* put in status at beginning */
  pPayload[0] = gZbSuccess_c;
  ++payloadLen;

#if gZclIncludeSceneName_d
  /* copy in string */
  FLib_MemCpy(pPayload + payloadLen, pScene->szSceneName, 1 + *pScene->szSceneName);
  payloadLen += 1 + *pScene->szSceneName;
#else
  /* empty string */
  pPayload[payloadLen++] = 0;
#endif

  /* copy in cluster data */
  payloadLen += HA_ViewSceneData((void *)(pPayload + payloadLen), pScene);

  /* send payload over the air */
  return ZCL_ReplyNoCopy(pIndication, payloadLen, pMsg);
}

/*****************************************************************************
* ZCL_GetSceneMembership
*
* Creates the payload for get scene membership command.
*****************************************************************************/
void ZCL_GetSceneMembership
  (
  afDeviceDef_t *pDevice,
  zbGroupId_t aGroupId,                 /* IN */
  zbApsdeDataIndication_t *pIndication  /* IN */
  )
{
  afToApsdeMessage_t *pMsg;
  uint8_t payloadLen;
  uint8_t i;
  uint8_t count;    /* # of scenes found that match this group */
  zclCmdScene_GetSceneMembershipRsp_t *pRsp;
  
  zclSceneAttrs_t *pSceneAttrs;
  zclSceneTableEntry_t *pSceneTableEntry;

  /* allocate space for the message */
  pMsg = AF_MsgAlloc();
  if(!pMsg)
    return;   /* can't send response, no buffers */
  
 
  pSceneAttrs = (zclSceneAttrs_t*) pDevice->pSceneData;  

  /* fill in the response */
  pRsp = (void *)(((uint8_t *)pMsg) + gAsduOffset_c + sizeof(zclFrame_t));

  /* remember how many scenes are available */
  pRsp->status =gZbSuccess_c;
  Copy2Bytes(pRsp->aGroupId, aGroupId);
  pRsp->capacity = gHaMaxScenes_c - pSceneAttrs->sceneCount;

  /* show any that match this group */
  count = 0;
  for(i=0; i<gHaMaxScenes_c; ++i)
  {
    pSceneTableEntry = (zclSceneTableEntry_t *) ((uint8_t*)pDevice->pSceneData + sizeof(zclSceneAttrs_t) + i*pSceneAttrs->sceneTableEntrySize);
    if(IsEqual2Bytes(pSceneTableEntry->aGroupId, aGroupId)) {
      pRsp->scenes[count] = pSceneTableEntry->sceneId;
      ++count;
    }
  }

  if(!count) {
    pRsp->status = gZclInvalidField_c;
    payloadLen = MbrOfs(zclCmdScene_GetSceneMembershipRsp_t,sceneCount);
  }
  else {
    payloadLen = MbrOfs(zclCmdScene_GetSceneMembershipRsp_t, scenes) + count;
    pRsp->sceneCount = count;
  }

  /* send payload over the air */
  (void)ZCL_ReplyNoCopy(pIndication, payloadLen, pMsg);
}

/*****************************************************************************
* ZCL_AllocateScene
*
* Allocate a scene entry. Used for storing and recalling a scene.
*
* Returns NULL if not found, or pointer to entry in scene table if found.
*****************************************************************************/
zclSceneTableEntry_t *ZCL_AllocateScene(afDeviceDef_t *pDevice, zclCmdScene_StoreScene_t *pSceneId)
{
  index_t i;
  zclSceneTableEntry_t *pSceneTableEntry;
  zclSceneAttrs_t* pSceneAttrs = (zclSceneAttrs_t*) pDevice->pSceneData;


  /* invalid scene, not found */
  pSceneTableEntry = ZCL_FindScene(pDevice, (void *)pSceneId);
  if(pSceneTableEntry)
    return pSceneTableEntry;

  
  for(i=0; i<gHaMaxScenes_c; ++i)
  {
    pSceneTableEntry = (zclSceneTableEntry_t *) ((uint8_t*)pDevice->pSceneData + sizeof(zclSceneAttrs_t) + i*pSceneAttrs->sceneTableEntrySize);
    if(!pSceneTableEntry->sceneId)
    {
      FLib_MemCpy(pSceneTableEntry, pSceneId, sizeof(*pSceneId));
      ++(pSceneAttrs->sceneCount);
      return pSceneTableEntry;
    }
  }

  /* not found */
  return NULL;
}

/*****************************************************************************
* ZCL_FindScene
*
* Find a scene based on group id and scene id
*
* Returns NULL if not found, or pointer to entry in scene table if found.
*****************************************************************************/
zclSceneTableEntry_t *ZCL_FindScene(afDeviceDef_t *pDevice, zclCmdScene_RecallScene_t *pSceneId)
{
  index_t i;
  zclSceneTableEntry_t *pSceneTableEntry;
  zclSceneAttrs_t* pSceneAttrs = (zclSceneAttrs_t*) pDevice->pSceneData;

  /* invalid scene, not found */
  if(!pSceneId->sceneId)
    return NULL;

  for(i=0; i<gHaMaxScenes_c; ++i)
  {
    pSceneTableEntry = (zclSceneTableEntry_t *) ((uint8_t*)pDevice->pSceneData + sizeof(zclSceneAttrs_t) + i*pSceneAttrs->sceneTableEntrySize);

    if ( FLib_MemCmp((uint8_t*) pSceneId, (uint8_t*) pSceneTableEntry, sizeof(zclCmdScene_RecallScene_t)))          
      return pSceneTableEntry;
  }

  /* not found */
  return NULL;
}


/******************************
  On/Off Cluster
  See ZCL Specification Section 3.8
*******************************/

/*****************************************************************************
* ZCL_OnOffClusterServer
*
* Handle On/Off Cluster
*
* Returns gZbSuccess_t if worked.
*****************************************************************************/
zbStatus_t ZCL_OnOffClusterServer
  (
  zbApsdeDataIndication_t *pIndication, /* IN: MUST be set to the indication */
  afDeviceDef_t *pDevice    /* IN: MUST be set to the endpoint's device data */
  )
{
  zclCmd_t command;
  uint8_t onOffAttr;
  uint8_t event;
  zbEndPoint_t ep;
  zbClusterId_t aClusterId;
  zbStatus_t status = gZclSuccessDefaultRsp_c;
  
  /* not used in this function */
  (void)pDevice;
  
  /* get the attribute */
  ep = pIndication->dstEndPoint;
  Copy2Bytes(aClusterId, pIndication->aClusterId);
  (void)ZCL_GetAttribute(ep, aClusterId, gZclAttrOnOff_OnOffId_c, &onOffAttr, NULL);

  /* determine what to do based on the event */
  event = gZclUI_NoEvent_c;
  command = ((zclFrame_t *)pIndication->pAsdu)->command;
  
  if(command == gZclCmdOnOff_Toggle_c) {
    command = onOffAttr ? gZclCmdOnOff_Off_c : gZclCmdOnOff_On_c;
  }
  
  switch (command) {
    case gZclCmdOnOff_Off_c:
      event = gZclUI_Off_c;
      onOffAttr = 0;  /* off */
      break;
    case gZclCmdOnOff_On_c:
      event = gZclUI_On_c;
      onOffAttr = 1;  /* on */
      break;
    default:
      status = gZclUnsupportedClusterCommand_c;      
      break;           
  }


  /* sets the attribute and will report if needed */
  (void)ZCL_SetAttribute(ep, aClusterId, gZclAttrOnOff_OnOffId_c, &onOffAttr);

  /* send the event to the app */
  BeeAppUpdateDevice(ep, event, 0, 0, NULL);
  /* worked */
  return status;
}


/******************************************************************************
* ZCL_GenericReqNoData
* 
* Standard request when there is no payload.
* Frame cotnrol disables default response from receiver.
* 
* Returns 
*   gZbSuccess_c if worked
******************************************************************************/
zbStatus_t ZCL_GenericReqNoData(afAddrInfo_t *pAddrInfo, zclCmd_t command)
{
  uint8_t payloadLen;
  afToApsdeMessage_t *pMsg;

  /* create the frame and copy in payload */
  payloadLen = 0;
  pMsg = ZCL_CreateFrame(pAddrInfo,command,gZclFrameControl_FrameTypeSpecific | gZclFrameControl_DisableDefaultRsp,
    NULL, &payloadLen,NULL);
  if(!pMsg)
    return gZclNoMem_c;

  /* send the frame to the destination */
  return ZCL_DataRequestNoCopy(pAddrInfo, payloadLen, pMsg);
}

zbStatus_t ZCL_GenericReqNoDataServer(afAddrInfo_t *pAddrInfo, zclCmd_t command)
{
  uint8_t payloadLen;
  afToApsdeMessage_t *pMsg;

  /* create the frame and copy in payload */
  payloadLen = 0;
  pMsg = ZCL_CreateFrame(pAddrInfo, command,gZclFrameControl_FrameTypeSpecific |gZclFrameControl_DirectionRsp | gZclFrameControl_DisableDefaultRsp,
    NULL, &payloadLen,NULL);
  if(!pMsg)
    return gZclNoMem_c;

  /* send the frame to the destination */
  return ZCL_DataRequestNoCopy(pAddrInfo, payloadLen, pMsg);
}


/******************************
  On/Off Switch Configuration Cluster
  See ZCL Specification Section 3.9
*******************************/
zbStatus_t ZCL_OnOffSwitchClusterServer 
(
  zbApsdeDataIndication_t *pIndication,   /* IN: must be non-null */
  afDeviceDef_t *pDevice                  /* IN: must be non-null */
)
{
  (void) pIndication;
  (void) pDevice;
   return gZbSuccess_c;
}

/******************************
  Level Control Cluster
  See ZCL Specification Section 3.10
*******************************/

/*****************************************************************************
* ZCL_LevelOnOffClusterServer
*
* A slight variation of the On/Off Cluster, includes remembering level when
* turning on/off.
*
* Returns gZbSuccess_t if worked.
*****************************************************************************/
zbStatus_t ZCL_LevelOnOffClusterServer
  (
  zbApsdeDataIndication_t *pIndication, /* IN: MUST be set to the indication */
  afDeviceDef_t *pDevice    /* IN: MUST be set to the endpoint's device data */
  )
{
  zclCmd_t command;
  uint8_t onOffAttr;
  zbClusterId_t           aClusterShadeId;

#if gZclClusterOptionals_d  
  uint8_t onlevel;
#endif  
  zbEndPoint_t ep;
  zbClusterId_t aClusterId;
  zbClusterId_t aLevelClusterId;
  zclCmdLevelControl_MoveToLevel_t Req;
  zbStatus_t status = gZclSuccessDefaultRsp_c;
  Set2Bytes(aClusterShadeId, gZclClusterShadeCfg_c);
  /* not used in this function */
  (void)pDevice;
   if(gShadeDevice == TRUE)
   {
     gShadeDeviceActive = TRUE; 
    (void)ZCL_GetAttribute(gZcl_ep, aClusterShadeId, gZclAttrShadeCfgInfStatus_c, &statusShadeCfg, NULL);
   }
  gZcl_ep = pIndication->dstEndPoint;
  gZclLevelCmd = gZclLevel_OnOffCmd_c;

  /* level control cluster */
  Set2Bytes(aClusterId,gZclClusterOnOff_c);
  Set2Bytes(aLevelClusterId, gZclClusterLevelControl_c);
  
  /* get the attribute */
  ep = pIndication->dstEndPoint;
  (void)ZCL_GetAttribute(ep, aClusterId, gZclAttrOnOff_OnOffId_c, &onOffAttr, NULL);

  /* determine what to do based on the event */
  command = ((zclFrame_t *)pIndication->pAsdu)->command;

  if(command == gZclCmdOnOff_Toggle_c) {
    command = onOffAttr ? gZclCmdOnOff_Off_c : gZclCmdOnOff_On_c;
  }

  /* Keep a copy of current level*/
#if gZclClusterOptionals_d
      (void)ZCL_GetAttribute(ep, aLevelClusterId, gZclAttrLevelControl_OnLevelId_c, &onlevel, NULL);
      if (onlevel == gZclLevel_UndefinedOnLevel_c) {
        
        (void)ZCL_GetAttribute(ep, aLevelClusterId, gZclAttrLevelControl_CurrentLevelId_c, &gZclLevel_LastCurrentLevel, NULL);      
      }
      else {       
        gZclLevel_LastCurrentLevel = gZclLevelMinValue;
      }
#else      
      (void)ZCL_GetAttribute(ep, aLevelClusterId, gZclAttrLevelControl_CurrentLevelId_c, &gZclLevel_LastCurrentLevel, NULL);      
#endif
  
  
  
  switch (command) {
  
    case gZclCmdOnOff_Off_c:
      if (onOffAttr == gZclCmdOnOff_Off_c)
      {
        uint8_t displayStatus = 1;
        BeeAppUpdateDevice(gZcl_ep, gZclUI_GoToLevel_c, 0, 0, (void*) &displayStatus);
        return status;
      }
      /* Fill the request for the off case */
      Req.level = gZclLevelMinValue; /* Set to the minimum level suported */    
      break;
  
    case gZclCmdOnOff_On_c:
      if (onOffAttr == gZclCmdOnOff_Off_c) {
    	  onOffAttr = gZclLevelMinValue;
  	    (void)ZCL_SetAttribute(gZcl_ep, aLevelClusterId, gZclAttrLevelControl_CurrentLevelId_c,&onOffAttr);
      }
     /* Fill the request for the on case */
#if gZclClusterOptionals_d
      (void)ZCL_GetAttribute(ep, aLevelClusterId, gZclAttrLevelControl_OnLevelId_c, &onOffAttr, NULL);
      if (onOffAttr == gZclLevel_UndefinedOnLevel_c)
        Req.level = gZclLevel_LastCurrentLevel; /* Set to the stored value */    
      else
        Req.level = onOffAttr;                  /* Set to the OnLevel value */          
#else
      Req.level = gZclLevel_LastCurrentLevel;   /* Set to the stored value */    
#endif    
      break;

    default:
      return gZclUnsupportedClusterCommand_c;      
  }
  
  Req.transitionTime = 0xFFFF;   /* Set to 0xFFFF to use the OnOffTransition time */
  
  /* sets the attribute and will report if needed */
  gZclLevel_OnOffState = command;
  (void)ZCL_LevelControlMoveToLevel(&Req, TRUE);
  /* worked */
  return status;
}


/******************************************************************************
* Level Control Cluster Server, receives the level control commands for things
* like a dimmable light.
*
* IN: pIndication   pointer to indication. Must be non-null
* IN: pDevice       pointer to device definition. Must be non-null.
*
* Returns
*   gZbSuccess_c    worked
******************************************************************************/
zbStatus_t ZCL_LevelControlClusterServer
  (
  zbApsdeDataIndication_t *pIndication,   /* IN: must be non-null */
  afDeviceDef_t *pDevice                  /* IN: must be non-null */
  ) 
{ 
  zclFrame_t *pFrame;
  zclLevelControlCmd_t Command; 
  zbStatus_t status = gZclSuccessDefaultRsp_c;
  bool_t withOnOff = FALSE;
  zbClusterId_t           aClusterShadeId;
  Set2Bytes(aClusterShadeId, gZclClusterShadeCfg_c);
  (void)pDevice;

  gZclLevelCmd = gZclLevel_NotOnOffCmd_c;

  /* ZCL frame */
  pFrame = (zclFrame_t*)pIndication->pAsdu;
  gZcl_ep = pIndication->dstEndPoint;

  /* make local copy of command (might be move to level, step, etc...) */
  FLib_MemCpy(&Command,(pFrame + 1), sizeof(Command));
  if(gShadeDevice == TRUE)
  {
     gShadeDeviceActive = TRUE; 
    (void)ZCL_GetAttribute(gZcl_ep, aClusterShadeId, gZclAttrShadeCfgInfStatus_c, &statusShadeCfg, NULL);
  }
  /* handle the LevelControl commands */
  switch(pFrame->command)
  {
    /* Move to Level Commands */
    case gZclCmdLevelControl_MoveToLevelOnOff_c:
      withOnOff = TRUE;
    case gZclCmdLevelControl_MoveToLevel_c:        
      /* set on level and current level */
      (void)ZCL_LevelControlMoveToLevel(&Command.MoveToLevelCmd, withOnOff);
      break;

    /* Move commands */
    case gZclCmdLevelControl_MoveOnOff_c:    
      withOnOff = TRUE;    
    case gZclCmdLevelControl_Move_c:
      (void)ZCL_LevelControlMove(&Command.MoveCmd, withOnOff);
      break;

    /* Step Commands */
    case gZclCmdLevelControl_StepOnOff_c:
      withOnOff = TRUE;    
    case gZclCmdLevelControl_Step_c:
      (void)ZCL_LevelControlStep(&Command.StepCmd, withOnOff);
      break;

    /* Stop Commands */
    case gZclCmdLevelControl_StopOnOff_c:
    case gZclCmdLevelControl_Stop_c:
      (void)ZCL_LevelControlStop();
      break;      

    /* command not supported on this cluster */
    default:
      return gZclUnsupportedClusterCommand_c;
  }
  return status;
}


/*****************************************************************************
* ZCL_LevelControlTimer Fuction
*****************************************************************************/

void ZCL_LevelControlTimer( uint8_t timerId )
{
  zbClusterId_t           aLevelClusterId;
  zbClusterId_t           aClusterId;
  zbClusterId_t           aClusterShadeId;
  uint8_t                 OnOffStatus; 
  bool_t                  updateOnOffStatus = FALSE;
  (void) timerId;
  
  Set2Bytes(aLevelClusterId, gZclClusterLevelControl_c);
  Set2Bytes(aClusterId, gZclClusterOnOff_c);
  Set2Bytes(aClusterShadeId, gZclClusterShadeCfg_c);
  
  (void)ZCL_GetAttribute(gZcl_ep, aClusterId, gZclAttrOnOff_OnOffId_c, &OnOffStatus,NULL);
  (void)ZCL_GetAttribute(gZcl_ep, aLevelClusterId, gZclAttrLevelControl_CurrentLevelId_c, &gZclCurrentLevelTmp, NULL);
  gZclLevel_DimmingStopReport = FALSE;
 
  /* Update the CurrentLevel Attribute */
  if (gZclLevel_diference >= gZclLevel_Step)      
  {
    gZclLevel_diference -= gZclLevel_Step;
    if (zclMoveMode_Down == gZclLevel_MoveMode)
    {
      if(gShadeDevice == TRUE){
          statusShadeCfg.ShadeDirection = 0;
          statusShadeCfg.ShadeForwardDir = 0;
      }
        gZclCurrentLevelTmp -=gZclLevel_Step;
    }
    else
    {
      if(gShadeDevice == TRUE){
          statusShadeCfg.ShadeDirection = 1;
          statusShadeCfg.ShadeForwardDir = 1;
      }
        gZclCurrentLevelTmp +=gZclLevel_Step;
    }
  }
  else
  {
      gZclLevel_diference = 0;
      gZclCurrentLevelTmp = gZclNewCurrentLevel;
  }
  (void)ZCL_SetAttribute(gZcl_ep, aLevelClusterId, gZclAttrLevelControl_CurrentLevelId_c,&gZclCurrentLevelTmp);
  if (0 == gZclLevel_diference)
    gZclLevel_DimmingStopReport = TRUE;

  
  /*Recalculate the remaining time, substracting the time Between Changes*/
  if (gZclLevel_RemainingTimeInMilliseconds)
  {
    if ((gZclLevel_diference == 0) &&(gZclLevel_RemainingTimeInMilliseconds > gZclLevel_TimeBetweenChanges))
      gZclLevel_TimeBetweenChanges = (uint16_t)gZclLevel_RemainingTimeInMilliseconds;
    else
      gZclLevel_RemainingTimeInMilliseconds -= gZclLevel_TimeBetweenChanges;               
  }
   
    #if gZclClusterOptionals_d 
    {
      uint16_t remainingTime =  Native2OTA16((uint16_t) (gZclLevel_RemainingTimeInMilliseconds / 100));    
      (void)ZCL_SetAttribute(gZcl_ep, aLevelClusterId, gZclAttrLevelControl_RemainingTimeId_c,&remainingTime);
    }
    #endif

  
  if(gZclLevel_diference) 
  {
    /* We didn't get to the specified level, so restart the timer */
    TMR_StartSingleShotTimer(gZclLevel_TransitionTimerID,(tmrTimeInMilliseconds_t)gZclLevel_TimeBetweenChanges, ZCL_LevelControlTimer);
  }
  else 
  {
    updateOnOffStatus = TRUE;
    
    #if gZclClusterOptionals_d 
    {
    uint16_t remainingTime = 0;
   (void)ZCL_SetAttribute(gZcl_ep, aLevelClusterId, gZclAttrLevelControl_RemainingTimeId_c,&remainingTime);
    }
    #endif
    
    
    if (gZclLevelCmd == gZclLevel_OnOffCmd_c) 
    {
      /* There was an OnOff command, update the OnOff attribute */
      (void)ZCL_SetAttribute(gZcl_ep, aClusterId, gZclAttrOnOff_OnOffId_c, &gZclLevel_OnOffState);
      #if gZclClusterOptionals_d				        
      (void)ZCL_GetAttribute(gZcl_ep, aLevelClusterId, gZclAttrLevelControl_OnLevelId_c, &OnOffStatus, NULL);        
      if (OnOffStatus == gZclLevel_UndefinedOnLevel_c)
      #endif
        (void)ZCL_SetAttribute(gZcl_ep, aLevelClusterId, gZclAttrLevelControl_CurrentLevelId_c, &gZclLevel_LastCurrentLevel);
    }
    else if (gZclLevel_OnOff)
    {         
      /* There was an Move or MoveToLevel with OnOff option, so update the OnOff attribute, based on the current level */   
      if (gZclCurrentLevelTmp > gZclLevelMinValue)
        OnOffStatus = gZclCmdOnOff_On_c;
      else
        OnOffStatus = gZclCmdOnOff_Off_c;
      (void)ZCL_SetAttribute(gZcl_ep, aClusterId, gZclAttrOnOff_OnOffId_c, &OnOffStatus);
      if(gShadeDevice == TRUE)
         gShadeDeviceActive = FALSE;           
    }
  }
  BeeAppUpdateDevice(gZcl_ep, gZclUI_GoToLevel_c, 0, 0, (void*) &updateOnOffStatus);
}


/*****************************************************************************
* ZCL_LevelControlMoveToLevel Fuction
*
* On receipt of this command, a device shall move from its current level to the value given in the Level
* field. The meaning of ‘level’ is device dependent – e.g. for a light it may mean brightness level.
* The movement shall be continuous, i.e. not a step function, and the time taken to move to the new level
* shall be equal to the Transition time field, in seconds.
*
* Returns gZbSuccess_t if worked.
*****************************************************************************/
zbStatus_t ZCL_LevelControlMoveToLevel
(
  zclCmdLevelControl_MoveToLevel_t * pReq,
  bool_t withOnOff
)
{
	zbClusterId_t           aLevelClusterId;
	uint16_t                TransitionTime = 0;
	uint16_t                OnOffTransitionTime = 0;
	Set2Bytes(aLevelClusterId, gZclClusterLevelControl_c);

	(void)ZCL_GetAttribute(gZcl_ep, aLevelClusterId, gZclAttrLevelControl_CurrentLevelId_c, &gZclCurrentLevelTmp, NULL);
	TransitionTime = OTA2Native16(pReq->transitionTime);
	if (TransitionTime == 0xFFFF){
        #if gZclClusterOptionals_d
          (void) ZCL_GetAttribute(gZcl_ep, aLevelClusterId, gZclAttrLevelControl_OnOffTransitionTimeId_c, &OnOffTransitionTime, NULL);
        #endif            
          TransitionTime = OnOffTransitionTime; 
        }
        gZclLevel_MoveMode = zclMoveMode_Down;
        gZclLevel_OnOff = withOnOff;
        gZclLevel_diference = gZclCurrentLevelTmp- pReq->level;	
        if (gZclCurrentLevelTmp <= pReq->level) {
          gZclLevel_MoveMode = zclMoveMode_Up;
          gZclLevel_diference = pReq->level - gZclCurrentLevelTmp;
        }
	gZclLevel_Step = 1;
	gZclNewCurrentLevel = pReq->level;
	
	gZclLevel_RemainingTimeInMilliseconds = (tmrTimeInMilliseconds_t)(TransitionTime *100);	
  #if gZclClusterOptionals_d 
  {
  uint16_t remainingTime =  Native2OTA16((uint16_t) (gZclLevel_RemainingTimeInMilliseconds / 100));
    
	(void)ZCL_SetAttribute(gZcl_ep, aLevelClusterId, gZclAttrLevelControl_RemainingTimeId_c,&remainingTime);
  }
  #endif		
	
	
	gZclLevel_TimeBetweenChanges = (uint16_t)(gZclLevel_RemainingTimeInMilliseconds/gZclLevel_diference);
	TMR_StartSingleShotTimer(gZclLevel_TransitionTimerID,gZclLevel_TimeBetweenChanges, ZCL_LevelControlTimer);
	return gZbSuccess_c;
}

/*****************************************************************************
* ZCL_LevelControlMove Fuction
* On receipt of this command, a device shall move from its current level in an up 
* or down direction in a continuous fashion, as detailed by the mode:

Up:      Increase the device’s level at the rate given in the Rate field. If the level
         reaches the maximum allowed for the device, stop. If the device is currently
         powered off, do not power it on.
Up with OnOff: If the device requires powering on, do so, then proceed as for the Up mode.
Down:    Decrease the device’s level at the rate given in the Rate field. If the level
         reaches the minimum allowed for the device, stop. If the device is currently
         powered off, do not power it on.
Down with OnOff: Decrease the device’s level at the rate given in the Rate field. If the level
                 reaches the minimum allowed for the device, stop, then, if the device can be
                 powered off, do so.
*
* The Rate field specifies the rate of movement in steps per second. A step is a change 
* in the device’s level of one unit.
*
* Returns gZbSuccess_t if worked.
*****************************************************************************/
zbStatus_t ZCL_LevelControlMove 
(
  zclCmdLevelControl_Move_t * pReq,
  bool_t withOnOff
)
{
  zbClusterId_t           aLevelClusterId;
  zbClusterId_t           aOnOffClusterId;
  zclLevelValue_t         CurrentLevel;
  uint8_t                 OnOffStatus;
  
  Set2Bytes(aLevelClusterId, gZclClusterLevelControl_c);
  Set2Bytes(aOnOffClusterId, gZclClusterOnOff_c);
  
  (void)ZCL_GetAttribute(gZcl_ep, aLevelClusterId, gZclAttrLevelControl_CurrentLevelId_c, &CurrentLevel, NULL);
  (void)ZCL_GetAttribute(gZcl_ep, aOnOffClusterId, gZclAttrOnOff_OnOffId_c,&OnOffStatus, NULL);
  
  gZclLevel_MoveMode =pReq->moveMode;
  gZclLevel_OnOff = withOnOff;
  gZclLevel_Step = 1;
  
  if ((gZclLevel_MoveMode == zclMoveMode_Up) && withOnOff && (!OnOffStatus)){
    OnOffStatus = 1;
    (void)ZCL_SetAttribute(gZcl_ep, aOnOffClusterId, gZclAttrOnOff_OnOffId_c,&OnOffStatus);
  }
  
  switch (gZclLevel_MoveMode){
  case zclMoveMode_Up:
          gZclLevel_diference = gZclLevel_high- CurrentLevel;
          gZclNewCurrentLevel = gZclLevel_high;
          break;
  case zclMoveMode_Down:
          gZclLevel_diference = CurrentLevel - gZclLevel_off;
          gZclNewCurrentLevel = gZclLevel_off;
          break;
  }
  gZclLevel_TimeBetweenChanges = (uint16_t)(1000/pReq->rate);
  gZclLevel_RemainingTimeInMilliseconds = (tmrTimeInMilliseconds_t)(gZclLevel_TimeBetweenChanges*gZclLevel_diference);
  
  #if gZclClusterOptionals_d 
  {
  uint16_t remainingTime =  Native2OTA16((uint16_t) (gZclLevel_RemainingTimeInMilliseconds / 100));
    
	(void)ZCL_SetAttribute(gZcl_ep, aLevelClusterId, gZclAttrLevelControl_RemainingTimeId_c,&remainingTime);
  }
  #endif	
  
  TMR_StartSingleShotTimer(gZclLevel_TransitionTimerID,gZclLevel_TimeBetweenChanges, ZCL_LevelControlTimer);
  
  return gZbSuccess_c;
}

/*****************************************************************************
* ZCL_LevelControlStep Fuction
* On receipt of this command, a device shall move from its current level in an up or down 
direction by the number of levels specified, as detailed by the mode:

Up:  Increase the device’s level by the number of units indicated in the Amount
     field. If the level is already at the maximum allowed for the device, then do
     nothing. If the value specified in the Amount field would cause the
     maximum value to be exceeded, then move to the maximum value using the
     full transition time. If the device is currently powered off, do not power it
     on.
On with OnOff: If the device requires powering on, do so, and then proceed as for the Up
               mode.
Down: Decrease the device’s level by the number of units indicated in the Amount
      field. If the level is already at the minimum allowed for the device, then do
      nothing. If the value specified in the Amount field would cause the
      minimum value to be exceeded, then move to the minimum value using the
      full transition time. If the device is currently powered off, do not power it
      on.
Down with OnOff:  Carry out the Down action. If the new level is at or below the minimum
                  allowed for the device, and the device can be powered off, then do so.
*
* The Transition time field specifies the time, in 1/10ths of a second, the time that shall be taken to
* perform the step. A step is a change in the device’s level by the number of units specified in the
* Amount field.
*
* Returns gZbSuccess_t if worked.
*****************************************************************************/
/* level control step request */
zclStatus_t ZCL_LevelControlStep
(
  zclCmdLevelControl_Step_t *pReq,
  bool_t withOnOff
)
{
	zbClusterId_t           aLevelClusterId;
	zbClusterId_t           aOnOffClusterId;
	uint16_t                TransitionTime = 0;
	zclLevelValue_t         CurrentLevel;
	uint8_t                 OnOffStatus;

	Set2Bytes(aLevelClusterId, gZclClusterLevelControl_c);
	Set2Bytes(aOnOffClusterId, gZclClusterOnOff_c);

	(void)ZCL_GetAttribute(gZcl_ep, aLevelClusterId, gZclAttrLevelControl_CurrentLevelId_c, &CurrentLevel, NULL);
	(void)ZCL_GetAttribute(gZcl_ep, aOnOffClusterId, gZclAttrOnOff_OnOffId_c,&OnOffStatus, NULL);

	TransitionTime = OTA2Native16(pReq->transitionTime);
	gZclLevel_diference = pReq->stepSize;
	gZclLevel_Step = 1;
	gZclLevel_MoveMode = pReq->stepMode;
    gZclLevel_OnOff = withOnOff;	
	if ((gZclLevel_MoveMode == zclMoveMode_Up) && withOnOff && (!OnOffStatus)){
		OnOffStatus = 1;
		(void)ZCL_SetAttribute(gZcl_ep, aOnOffClusterId, gZclAttrOnOff_OnOffId_c,&OnOffStatus);
	}
	if (OnOffStatus){
		switch (gZclLevel_MoveMode){
			case zclMoveMode_Up:
				gZclNewCurrentLevel = CurrentLevel + pReq->stepSize;
				if ((gZclLevel_high - CurrentLevel) < pReq->stepSize){
					gZclNewCurrentLevel = gZclLevel_high;
					gZclLevel_diference = gZclLevel_high - CurrentLevel;
				}
				break;
			case zclMoveMode_Down:
				gZclNewCurrentLevel = CurrentLevel - pReq->stepSize;
				if ((CurrentLevel - gZclLevel_off) < pReq->stepSize){
					gZclNewCurrentLevel = gZclLevel_off;
					gZclLevel_diference = CurrentLevel - gZclLevel_off;
				}
				break;
			}
		gZclLevel_Step = 1;
		gZclLevel_RemainingTimeInMilliseconds = (tmrTimeInMilliseconds_t)(TransitionTime*100);
		#if gZclClusterOptionals_d 
    {
      uint16_t remainingTime =  Native2OTA16((uint16_t) (gZclLevel_RemainingTimeInMilliseconds / 100));          
  	  (void)ZCL_SetAttribute(gZcl_ep, aLevelClusterId, gZclAttrLevelControl_RemainingTimeId_c,&remainingTime);
    }
    #endif		
		gZclLevel_TimeBetweenChanges = (uint16_t)(gZclLevel_RemainingTimeInMilliseconds/gZclLevel_diference);
		TMR_StartSingleShotTimer(gZclLevel_TransitionTimerID,gZclLevel_TimeBetweenChanges, ZCL_LevelControlTimer);
		}
	return gZbSuccess_c;
}

/*****************************************************************************
* ZCL_LevelControlStop Fuction
* On receipt of this command, a device shall stop any level control actions
* (move, move to level, step) that are in progress 
******************************************************************************/

zclStatus_t ZCL_LevelControlStop(void) 
{
  TMR_StopTimer(gZclLevel_TransitionTimerID);
  return gZbSuccess_c;  
}

/******************************
  Alarms Cluster
  See ZCL Specification Section 3.11
*******************************/


/**/
zbStatus_t ZCL_AlarmsClusterClient
(
zbApsdeDataIndication_t *pIndication, 
afDeviceDef_t *pDev
)
{
    zclFrame_t *pFrame;
    zclCmd_t command;

    /* prevent compiler warning */
    (void)pDev;
    pFrame = (void *)pIndication->pAsdu;
    /* handle the command */
    command = pFrame->command;  
    switch(command)
    {
        case gAlarmClusterTxCommandID_Alarm_c:
        case gAlarmClusterTxCommandID_GetAlarmResponse_c:

          /* These should be passed up to a host*/
          return gZbSuccess_c;
        default:
          return gZclUnsupportedClusterCommand_c;
    }
} 

extern gZclAlarmTable_t gAlarmsTable[MaxAlarmsPermitted];

zbStatus_t ZCL_AlarmsClusterServer
(
zbApsdeDataIndication_t *pIndication, 
afDeviceDef_t *pDev
)
{
    zclFrame_t *pFrame;
    zclCmd_t command = 0;
    zclCmdAlarmInformation_ResetAlarm_t CmdResetAlarm;
    zclAlarms_GetAlarmResponse_t *pGetAlarmResponse;
    zbClusterId_t aClusterId;
    uint8_t  i=0, min=0;
    uint16_t count = 0;
    uint32_t minTimeStamp = 0;
    /* prevent compiler warning */
    (void)pDev;
    pFrame = (void *)pIndication->pAsdu;
    Copy2Bytes(aClusterId, pIndication->aClusterId);
    /* handle the command */
    (void)ZCL_GetAttribute( pIndication->dstEndPoint, aClusterId, gZclAlarmInformation_AlarmCount_c, &count, NULL);
    count = OTA2Native16(count);
    command = pFrame->command;  
    switch(command)
    {
        case gAlarmClusterRxCommandID_ResetAlarm_c:
          FLib_MemCpy(&CmdResetAlarm ,(pFrame + 1), sizeof(zclCmdAlarmInformation_ResetAlarm_t));  
          ResetAlarm(CmdResetAlarm, pIndication->dstEndPoint);
          return gZbSuccess_c;
        case gAlarmClusterRxCommandID_ResetAllAlarms_c:
          ResetAllAlarm(pIndication->dstEndPoint);
          return gZbSuccess_c;
        case gAlarmClusterRxCommandID_GetAlarm_c:
          pGetAlarmResponse = MSG_Alloc(sizeof(zclCmdAlarmInformation_ResetAlarm_t)); 
          pGetAlarmResponse->addrInfo.dstAddrMode = gZbAddrMode16Bit_c;
          Copy2Bytes(pGetAlarmResponse->addrInfo.dstAddr.aNwkAddr, pIndication->aSrcAddr); 
          pGetAlarmResponse->addrInfo.dstEndPoint = pIndication->dstEndPoint;
          pGetAlarmResponse->addrInfo.srcEndPoint = pIndication->srcEndPoint;
          pGetAlarmResponse->addrInfo.txOptions = 0;
          pGetAlarmResponse->addrInfo.radiusCounter = afDefaultRadius_c;
          if(count > 1)
          {
            //minTimeStamp = gAlarmsTable[0].TimeStamp;
            FLib_MemCpy(&minTimeStamp, &gAlarmsTable[0].TimeStamp, 4);  
            for(i=0; i<(count-1); i++)
              if(minTimeStamp > gAlarmsTable[i+1].TimeStamp)
              {
            	min = i+1;  
                //minTimeStamp = gAlarmsTable[min].TimeStamp;
            	FLib_MemCpy(&minTimeStamp, &gAlarmsTable[i+1].TimeStamp, 4);       
              }
          }
          else
            min = 0;        
          pGetAlarmResponse->cmdFrame.AlarmCode = gAlarmsTable[min].AlarmCode;
          Copy2Bytes(&pGetAlarmResponse->cmdFrame.ClusterID, &gAlarmsTable[min].ClusterID);
          FLib_MemCpy(&pGetAlarmResponse->cmdFrame.TimeStamp, &gAlarmsTable[min].TimeStamp, 4);   
          pGetAlarmResponse->cmdFrame.Status = (count == 0x00)?STATUS_NOT_FOUND_c:STATUS_SUCCESS_c;
		  /* update alarm table */
          if(count > 0)
            for(i=min;i<count-1;i++)
        	//gAlarmsTable[i] = gAlarmsTable[i+1];
        	FLib_MemCpy(&gAlarmsTable[i], &gAlarmsTable[i+1], sizeof(gZclAlarmTable_t)); 
	   /*update alarm count attribut*/
	  count--;
	  count = Native2OTA16(count);
	  (void)ZCL_SetAttribute( pIndication->dstEndPoint, aClusterId, gZclAlarmInformation_AlarmCount_c, &count);			
          (void)Alarms_GetAlarmResponse(pGetAlarmResponse);   
           return gZbSuccess_c;
        case gAlarmClusterRxCommandID_ResetAlarmLog_c:
          count = 0;
          (void)ZCL_SetAttribute( pIndication->dstEndPoint, aClusterId, gZclAlarmInformation_AlarmCount_c, &count);
          return gZbSuccess_c;
        default:
          return gZclUnsupportedClusterCommand_c;
    }
}

zbStatus_t Alarms_ResetAlarm
(
zclAlarmInformation_ResetAlarm_t *pReq
)
{
    pReq->zclTransactionId = gZclTransactionId++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterALarms_c);	
    return ZCL_SendClientReqSeqPassed(gAlarmClusterRxCommandID_ResetAlarm_c, sizeof(zclCmdAlarmInformation_ResetAlarm_t), (zclGenericReq_t *)pReq);

}

void ResetAlarm(zclCmdAlarmInformation_ResetAlarm_t CmdResetAlarm, uint8_t endpoint)
{
#if gASL_ZclMet_Optionals_d  ||  gZclEnableThermostat_c 
#if gASL_ZclMet_Optionals_d  
   zbClusterId_t aClusterIdZone = {gaZclClusterSmplMet_c};
   uint16_t attrId = {gZclAttrSmplMetASGenericAlarmMask_c};
   uint16_t alarmMask;
#else   
  zbClusterId_t aClusterIdZone = {gaZclClusterThermostat_c};
  uint16_t attrId = {gZclAttrThermostat_AlarmMaskId_c};
  uint8_t alarmMask;
#endif  
  if(FLib_Cmp2Bytes(&CmdResetAlarm.ClusterID, &aClusterIdZone[0]) == TRUE){
    
    (void)ZCL_GetAttribute(endpoint, aClusterIdZone, attrId, &alarmMask, NULL);
     alarmMask = (~(1<<CmdResetAlarm.AlarmCode)) & alarmMask;
    (void)ZCL_SetAttribute(endpoint, aClusterIdZone, attrId, &alarmMask);      
  }
#endif  
}

zbStatus_t Alarms_ResetAllAlarms
(
zclAlarmInformation_NoPayload_t *pReq
)
{
    pReq->zclTransactionId = gZclTransactionId++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterALarms_c);	
    return ZCL_SendClientReqSeqPassed(gAlarmClusterRxCommandID_ResetAllAlarms_c, 0, (zclGenericReq_t *)pReq);

}

zbStatus_t Alarms_ResetAlarmLog
(
zclAlarmInformation_NoPayload_t *pReq
)
{
    pReq->zclTransactionId = gZclTransactionId++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterALarms_c);	
    return ZCL_SendClientReqSeqPassed(gAlarmClusterRxCommandID_ResetAlarmLog_c, 0, (zclGenericReq_t *)pReq);
}

zbStatus_t Alarms_GetAlarm
(
zclAlarmInformation_NoPayload_t *pReq
)
{
    pReq->zclTransactionId = gZclTransactionId++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterALarms_c);	
    return ZCL_SendClientReqSeqPassed(gAlarmClusterRxCommandID_GetAlarm_c, 0, (zclGenericReq_t *)pReq);

}

void ResetAllAlarm(uint8_t endpoint)
{
#if gASL_ZclMet_Optionals_d  ||  gZclEnableThermostat_c 
#if gASL_ZclMet_Optionals_d  
   zbClusterId_t aClusterIdZone[2] = {gaZclClusterALarms_c, gaZclClusterSmplMet_c};
   uint16_t attrId = {gZclAttrSmplMetASGenericAlarmMask_c};
   uint16_t valueAttr = 0;
#else     
   zbClusterId_t aClusterIdZone[2] = {gaZclClusterALarms_c, gaZclClusterThermostat_c};
   uint16_t attrId = {gZclAttrThermostat_AlarmMaskId_c};
   uint8_t  valueAttr = 0;
#endif    
   uint16_t count = 0, i=0;
   bool_t testedCluster = FALSE;

   (void)ZCL_GetAttribute(endpoint, aClusterIdZone[0], gZclAlarmInformation_AlarmCount_c, &count, NULL);
   count = Native2OTA16(count);
   for(i=0; i< count; i++){
    if(FLib_Cmp2Bytes(&gAlarmsTable[i].ClusterID,&aClusterIdZone[1]) == TRUE)
      if(testedCluster == FALSE){
         valueAttr = 0;
        (void)ZCL_SetAttribute(endpoint, aClusterIdZone[1], attrId, &valueAttr);  
         testedCluster = TRUE;
      }
   }  
#endif  
}


zbStatus_t Alarms_Alarm
(
zclAlarmInformation_Alarm_t *pReq
)
{
    pReq->zclTransactionId = gZclTransactionId++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterALarms_c);	
    return ZCL_SendServerReqSeqPassed(gAlarmClusterTxCommandID_Alarm_c, sizeof(zclCmdAlarmInformation_Alarm_t), (zclGenericReq_t *)pReq);

}


zbStatus_t Alarms_GetAlarmResponse
(
  zclAlarms_GetAlarmResponse_t *pReq
)
{
    pReq->zclTransactionId = gZclTransactionId++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterALarms_c);	
    return ZCL_SendServerRspSeqPassed(gAlarmClusterTxCommandID_GetAlarmResponse_c,sizeof(zclCmdAlarmInformation_GetAlarmResponse_t),(zclGenericReq_t *)pReq);
}

/*check the alarm mask bit to be set*/
uint8_t TestAlarmStatus(uint8_t index, uint8_t endPoint)
{
#if gASL_ZclMet_Optionals_d  
   zbClusterId_t aClusterIdZone = {gaZclClusterSmplMet_c};
   uint16_t alarmMask = 0x00;
   uint16_t attrId = {gZclAttrSmplMetASGenericAlarmMask_c};
#else   
   zbClusterId_t aClusterIdZone = {gaZclClusterThermostat_c};
   uint8_t alarmMask = 0x00;
   uint16_t attrId = {gZclAttrThermostat_AlarmMaskId_c};
#endif
   if(IsEqual2BytesInt(aClusterIdZone, gAlarmsTable[index].ClusterID) == TRUE)
   {    
      (void)ZCL_GetAttribute(endPoint, aClusterIdZone, attrId, &alarmMask, NULL);        
      if((alarmMask & (1<<gAlarmsTable[index].AlarmCode)) >> gAlarmsTable[index].AlarmCode == 1)
       return STATUS_SUCCESS_c;
   }
   return STATUS_NOT_FOUND_c;
}

/******************************
  Time Cluster
  See ZCL Specification (075123r02) Section 3.12
*******************************/
ZCLTimeServerAttrsRAM_t gTimeAttrsData = {
/*server*/
0x00000000,
(zclTimeStatusMaster | zclTimeStatusMasterZoneDst),
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
/* client */
};

ZCLTimeServerAttrsRAM_t gTimeAttrsData;

tmrTimerID_t zclTimeSecondTimer;


/* note 
- by setting zclTimeStatusMaster and zclTimeStatusMasterZoneDst all Time attributes becomes read only
ajustmets to the time and time zones must be done from the backend.
*/

const zclAttrDef_t gaZclTimeServerAttrDef[] = {
  { gZclAttrTime_c,           gZclDataTypeUTCTime_c,  (gZclAttrFlagsCommon_c | gZclAttrFlagsRdOnly_c), sizeof(uint32_t),  (void *)&gTimeAttrsData.Time },
  { gZclAttrTimeStatus_c,     gZclDataTypeBitmap8_c,  (gZclAttrFlagsCommon_c | gZclAttrFlagsRdOnly_c), sizeof(uint8_t),   (void *)&gTimeAttrsData.TimeStatus },
  { gZclAttrTimeZone_c,       gZclDataTypeInt32_c,    (gZclAttrFlagsCommon_c | gZclAttrFlagsRdOnly_c), sizeof(int32_t),   (void *)&gTimeAttrsData.TimeZone },
  { gZclAttrDstStart_c,       gZclDataTypeUint32_c,   (gZclAttrFlagsCommon_c | gZclAttrFlagsRdOnly_c), sizeof(uint32_t),  (void *)&gTimeAttrsData.DstStart },
  { gZclAttrDstEnd_c,         gZclDataTypeUint32_c,   (gZclAttrFlagsCommon_c | gZclAttrFlagsRdOnly_c), sizeof(uint32_t),  (void *)&gTimeAttrsData.DstEnd },
  { gZclAttrDstShift_c,       gZclDataTypeInt32_c,    (gZclAttrFlagsCommon_c | gZclAttrFlagsRdOnly_c), sizeof(int32_t),   (void *)&gTimeAttrsData.DstShift },
  { gZclAttrStandardTime_c,   gZclDataTypeUint32_c,   (gZclAttrFlagsCommon_c | gZclAttrFlagsRdOnly_c), sizeof(uint32_t),  (void *)&gTimeAttrsData.StandardTime },
  { gZclAttrLocalTime_c,      gZclDataTypeUint32_c,   (gZclAttrFlagsCommon_c | gZclAttrFlagsRdOnly_c), sizeof(uint32_t),  (void *)&gTimeAttrsData.LocalTime },
  { gZclAttrLastSetTime_c,    gZclDataTypeUTCTime_c,  (gZclAttrFlagsCommon_c | gZclAttrFlagsRdOnly_c), sizeof(uint32_t),  (void *)&gTimeAttrsData.LastSetTime},
  { gZclAttrValidUntilTime_c, gZclDataTypeUTCTime_c,  (gZclAttrFlagsCommon_c),                         sizeof(uint32_t),  (void *)&gTimeAttrsData.ValidUntilTime}
};

const zclAttrDefList_t gZcltimeServerClusterAttrDefList = {
  NumberOfElements(gaZclTimeServerAttrDef),
  gaZclTimeServerAttrDef
};

/*Update the Zcl Time Cluster UTC Time - shall be called every second*/
void ZCL_TimeUpdate(void)
{
  /* swap endianess OTA 2 native*/
  uint32_t nativeTime = OTA2Native32(gTimeAttrsData.Time); 
  int32_t nativeTimeZone = OTA2Native32(gTimeAttrsData.TimeZone); 
  uint32_t nativeDstStart = OTA2Native32(gTimeAttrsData.DstStart); 
  uint32_t nativeDstEnd = OTA2Native32(gTimeAttrsData.DstEnd); 
  uint32_t nativeDstShift = OTA2Native32(gTimeAttrsData.DstShift); 
  uint32_t nativeLocalTime = OTA2Native32(gTimeAttrsData.LocalTime); 
  uint32_t nativeStandardTime = OTA2Native32(gTimeAttrsData.StandardTime); 

  
  /* do calculations*/
  nativeTime++;
  
  /*Standard Time = Time + TimeZone*/
  nativeStandardTime = nativeTime + nativeTimeZone; 
  
  /*
    Local Time = Standard Time + DstShift (if DstStart <= Time <= DstEnd)
    Local Time = Standard Time (if Time < DstStart or Time > DstEnd)
  */
  if ((nativeDstStart <= nativeTime) && (nativeTime <= nativeDstEnd))
    nativeLocalTime = nativeStandardTime + nativeDstShift; 
  else
    nativeLocalTime = nativeStandardTime; 

  
  /*save and swap back to OTA endianness */
  gTimeAttrsData.Time = Native2OTA32(nativeTime);
  gTimeAttrsData.LocalTime = Native2OTA32(nativeLocalTime); 
  gTimeAttrsData.StandardTime = Native2OTA32(nativeStandardTime); 
}


void ZCL_TimeSecondTimerCallback(tmrTimerID_t timerid)
{
(void) timerid;
  /* restart timer */
  TMR_StartSecondTimer (zclTimeSecondTimer, 1, ZCL_TimeSecondTimerCallback);

  /* update the Zcl Time cluster*/ 
  ZCL_TimeUpdate();
}

/*Initialize the Time Cluster  - note input must be little endian*/
void ZCL_TimeInit(ZCLTimeConf_t *pTimeConf)
{
  /*Initialize the Time cluster atributes */
  gTimeAttrsData.Time = pTimeConf->Time;
  gTimeAttrsData.TimeZone = pTimeConf->TimeZone;
  gTimeAttrsData.DstStart = pTimeConf->DstStart;
  gTimeAttrsData.DstEnd = pTimeConf->DstEnd;
  gTimeAttrsData.DstShift = pTimeConf->DstShift;
  gTimeAttrsData.LastSetTime = pTimeConf->Time;
  gTimeAttrsData.ValidUntilTime = pTimeConf->ValidUntilTime;

  /* start the timer */
  zclTimeSecondTimer =  TMR_AllocateTimer();
  TMR_StartSecondTimer (zclTimeSecondTimer, 1, ZCL_TimeSecondTimerCallback);
}

void ZCL_SetUTCTime(uint32_t time)
{
  gTimeAttrsData.Time = Native2OTA32(time);
  gTimeAttrsData.LastSetTime = Native2OTA32(time);
}

uint32_t ZCL_GetUTCTime(void)
{
  return OTA2Native32(gTimeAttrsData.Time);
}

void ZCL_SetTimeZone(int32_t timeZone)
{
  gTimeAttrsData.TimeZone = Native2OTA32(timeZone);
}

void ZCL_SetTimeDst(uint32_t DstStart, uint32_t DstEnd, int32_t DstShift)
{
  gTimeAttrsData.DstStart = Native2OTA32(DstStart);
  gTimeAttrsData.DstEnd = Native2OTA32(DstEnd);
  gTimeAttrsData.DstShift = Native2OTA32(DstShift);
}

/******************************
  RSSI Cluster
  See ZCL Specification Section 3.13
*******************************/

zbStatus_t ZCL_RSSILocationClusterClient
(
zbApsdeDataIndication_t *pIndication, 
afDeviceDef_t *pDev
)
{
    zclFrame_t *pFrame;
    zclCmd_t command;

    /* prevent compiler warning */
    (void)pDev;
    pFrame = (void *)pIndication->pAsdu;
    /* handle the command */
    command = pFrame->command;  
    switch(command)
    {
        case gZclCmdTxRSSI_DeviceConfigurationResponse_c:
        case gZclCmdTxRSSI_LocationDataResponse_c:
        case gZclCmdTxRSSI_LocationDataNotification_c:
        case gZclCmdTxRSSI_CompactLocationDataNotification_c:
        case gZclCmdTxRSSI_RSSIping_c:
          return gZbSuccess_c;
        default:
          return gZclUnsupportedClusterCommand_c;
    }
}



zbStatus_t ZCL_RSSILocationClusterServer
(
zbApsdeDataIndication_t *pIndication, 
afDeviceDef_t *pDev
)
{
   zclFrame_t *pFrame;
    zclCmd_t command;

    /* prevent compiler warning */
    (void)pDev;
    pFrame = (void *)pIndication->pAsdu;
    /* handle the command */
    command = pFrame->command;  
    switch(command)
    {
        case gZclCmdRxRSSI_SetAbsoluteLocation_c:
        case gZclCmdRxRSSI_SetDeviceConfiguration_c:
        case gZclCmdRxRSSI_GetDeviceConfiguration_c:
        case gZclCmdRxRSSI_GetLocationData_c:

          /* These should be passed up to a host*/
          return gZbSuccess_c;
        default:
          return gZclUnsupportedClusterCommand_c;
    }
}


zbStatus_t RSSILocation_SetAbsoluteLocation
(
zclCmdRSSI_AbsoluteLocation_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterRssi_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxRSSI_SetAbsoluteLocation_c,0,(zclGenericReq_t *)pReq);
}


zbStatus_t RSSILocation_SetDeviceConfiguration
(zclCmdRSSI_SetDeviceConfiguration_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterRssi_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxRSSI_SetDeviceConfiguration_c,0,(zclGenericReq_t *)pReq);
}


zbStatus_t RSSILocation_GetDeviceConfiguration
(
zclCmdRSSI_GetDeviceConfiguration_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterRssi_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxRSSI_GetDeviceConfiguration_c,0,(zclGenericReq_t *)pReq);
}


zbStatus_t RSSILocation_GetLocationData
(
zclCmdRSSI_GetLocationData_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterRssi_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxRSSI_GetLocationData_c,0,(zclGenericReq_t *)pReq);
}


zbStatus_t RSSILocation_DeviceConfigurationResponse
(
zclCmdRSSI_DeviceConfigurationResponse_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterRssi_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdTxRSSI_DeviceConfigurationResponse_c,0,(zclGenericReq_t *)pReq);
}



zbStatus_t RSSILocation_LocationDataResponse
(
zclCmdRSSI_LocationDataResponse_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterRssi_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdTxRSSI_LocationDataResponse_c,0,(zclGenericReq_t *)pReq);
}


zbStatus_t RSSILocation_LocationDataNotification
(
zclCmdRSSI_LocationDataNotification_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterRssi_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdTxRSSI_LocationDataNotification_c,0,(zclGenericReq_t *)pReq);
}


zbStatus_t RSSILocation_CompactLocationDataNotification
(
zclCmdRSSI_CompactLocationDataNotification_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterRssi_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdTxRSSI_CompactLocationDataNotification_c,0,(zclGenericReq_t *)pReq);
}


zbStatus_t RSSILocation_RSSIping
(
zclCmdRSSI_RSSIping_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterRssi_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdTxRSSI_RSSIping_c,0,(zclGenericReq_t *)pReq);
}


/******************************
  Binary Input(Basic)Cluster
  See ZCL Specification  Section 3.14.4
*******************************/

/* Binary Input(basic) Cluster Attribute Definitions */
const zclAttrDef_t gaZclBinaryInputClusterAttrDef[] = {
#if gZclClusterOptionals_d
  { gZclAttrBinaryInputActiveText_c,   gZclDataTypeStr_c, gZclAttrFlagsInRAM_c, sizeof(zclStr16_t), (void *)MbrOfs(zclBinaryInputAttrsRAM_t, activeText)},
  { gZclAttrBinaryInputDescription_c,  gZclDataTypeStr_c, gZclAttrFlagsInRAM_c, sizeof(zclStr16_t), (void *)MbrOfs(zclBinaryInputAttrsRAM_t, description)},
  { gZclAttrBinaryInputInactiveText_c, gZclDataTypeStr_c, gZclAttrFlagsInRAM_c, sizeof(zclStr16_t), (void *)MbrOfs(zclBinaryInputAttrsRAM_t, inactiveText)},
#endif 
  { gZclAttrBinaryInputOutOfService_c, gZclDataTypeBool_c , gZclAttrFlagsInRAM_c, sizeof(uint8_t), (void *)MbrOfs(zclBinaryInputAttrsRAM_t, outOfService)},
#if gZclClusterOptionals_d
  { gZclAttrBinaryInputPolarity_c, gZclDataTypeEnum8_c , gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint8_t), (void *)MbrOfs(zclBinaryInputAttrsRAM_t, polarity)},
#endif 
  { gZclAttrBinaryInputPresentValue_c, gZclDataTypeBool_c , gZclAttrFlagsInRAM_c| gZclAttrFlagsReportable_c, sizeof(uint8_t), (void *)MbrOfs(zclBinaryInputAttrsRAM_t,presentValue)},
#if gZclClusterOptionals_d
  { gZclAttrBinaryInputReliability_c, gZclDataTypeEnum8_c , gZclAttrFlagsInRAM_c , sizeof(uint8_t), (void *)MbrOfs(zclBinaryInputAttrsRAM_t,reliability)},
#endif 
  { gZclAttrBinaryInputStatusFlags_c, gZclDataTypeBitmap8_c , gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c | gZclAttrFlagsReportable_c, sizeof(uint8_t), (void *)MbrOfs(zclBinaryInputAttrsRAM_t, statusFlags)}
#if gZclClusterOptionals_d
  , { gZclAttrBinaryInputApplicationType_c, gZclDataTypeUint32_c  , gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c , sizeof(uint8_t), (void *)MbrOfs(zclBinaryInputAttrsRAM_t, applicationType)}
#endif 
};

const zclAttrDefList_t gZclBinaryInputClusterAttrDefList = {
  NumberOfElements(gaZclBinaryInputClusterAttrDef),
  gaZclBinaryInputClusterAttrDef
};

/******************************
  Commissioning Cluster
  See Commisioning cluser Specification 064699r12
*******************************/
tmrTimerID_t CommisioningTimer;
void LeaveTimerCallback(tmrTimerID_t timerid);
void StartTimerCallback(tmrTimerID_t timerid);
static uint8_t RestartDelay;
static uint16_t RestartJitter;
static bool_t RestartFromAttributeSet;
#include "TMR_Interface.h"
#include "ZdoApsInterface.h"

zbCommissioningAttributes_t gCommisioningServerAttrsData = {
/* initialize like ROM set..*/
  /*** Startup Attribute Set (064699r12, section 6.2.2.1) ***/
  {mDefaultNwkShortAddress_c},        /* x shortAddress (default 0xff,0xff) */
  {mDefaultNwkExtendedPANID_c},       /* x nwkExtendedPANId */
  {mDefultApsUseExtendedPANID_c},     /* x apsUSeExtendedPANId */
  {mDefaultValueOfPanId_c},           /* x panId */
  { (uint8_t)(mDefaultValueOfChannel_c & 0xff),
    (uint8_t)((mDefaultValueOfChannel_c>>8) & 0xff),
    (uint8_t)((mDefaultValueOfChannel_c>>16) & 0xff),
    (uint8_t)((mDefaultValueOfChannel_c>>24) &0xff)
  },                                  /* x channelMask */
  {mNwkProtocolVersion_c},            /* x protocolVersion, always 0x02=ZigBee 2006, 2007 */
  {gDefaultValueOfNwkStackProfile_c}, /* x stackProfile 0x01 or 0x02 */
  {gStartupControl_Associate_c},      /* startupControl */
  {mDefaultValueOfTrustCenterLongAddress_c},  /* x trustCenterAddress */
  {mDefaultValueOfTrustCenterMasterKey_c},  /* trustCenterMasterKey */
  {mDefaultValueOfNetworkKey_c},      /* x networkKey */
  {gApsUseInsecureJoinDefault_c},     /* x useInsecureJoin */
  {mDefaultValueOfTrustCenterLinkKey_c},  /* preconfiguredLinkKey (w/ trust center) */
  {mDefaultValueOfNwkActiveKeySeqNumber_c}, /* x networkKeySeqNum */
  {mDefaultValueOfNwkKeyType_c},      /* x networkKeyType */
  {gNwkManagerShortAddr_c},           /* x networkManagerAddress, little endian */

  /*** Join Parameters Attribute Set (064699r12, section 6.2.2.2) ***/
  {mDefaultValueOfNwkScanAttempts_c}, /* x # of scan attempts */
  { (mDefaultValueOfNwkTimeBwnScans_c & 0xff),
    (mDefaultValueOfNwkTimeBwnScans_c >> 8)
  },                                  /* x time between scans(ms) */
  {(mDefaultValueOfRejoinInterval_c&0xff),
   (mDefaultValueOfRejoinInterval_c>>8)
  },                                  /* x rejoin interval (sec) */
  {(mDefaultValueOfMaxRejoinInterval_c & 0xff),
   (mDefaultValueOfMaxRejoinInterval_c >> 8)
  },                                  /* x maxRejoinInterval (sec) */

  /*** End-Device Parameters Attribute Set (064699r12, section 6.2.2.3) ***/
  {(mDefaultValueOfIndirectPollRate_c & 0xff),
   (mDefaultValueOfIndirectPollRate_c >> 8)
  },                                  /* x indirectPollRate(ms) */
  {gMaxNwkLinkRetryThreshold_c},      /* x parentRetryThreshold */

  /*** Concentrator Parameters Attribute Set (064699r12, section 6.2.2.4) ***/
  {gConcentratorFlag_d},              /* x concentratorFlag */
  {gConcentratorRadius_c},            /* x concentratorRadius */
  {gConcentratorDiscoveryTime_c},     /* x concentratorDiscoveryTime */
};

const zclAttrDef_t gaZclCommisioningServerAttrDef[] = {
/* Server attributes */
  { gZclAttrCommissioning_ShortAddressID_c,              gZclDataTypeUint16_c, gZclAttrFlagsInRAM_c| gZclAttrFlagsRdOnly_c, sizeof(uint16_t), (void *)MbrOfs(zbCommissioningAttributes_t, aShortAddress) },
  { gZclAttrCommissioning_ExtendedPANIdID_c,             gZclDataTypeIeeeAddr_c,gZclAttrFlagsInRAM_c, sizeof(zbIeeeAddr_t), (void *)MbrOfs(zbCommissioningAttributes_t, aNwkExtendedPanId) },
  { gZclAttrCommissioning_PanIdID_c,                     gZclDataTypeUint16_c, gZclAttrFlagsInRAM_c| gZclAttrFlagsRdOnly_c, sizeof(uint16_t), (void *)MbrOfs(zbCommissioningAttributes_t, aPanId) },
  { gZclAttrCommissioning_ChannelMaskID_c,               gZclDataTypeBitmap32_c, gZclAttrFlagsInRAM_c, sizeof(zbChannels_t), (void *)MbrOfs(zbCommissioningAttributes_t, aChannelMask) },
  { gZclAttrCommissioning_ProtocolVersionID_c,           gZclDataTypeUint8_c, gZclAttrFlagsInRAM_c| gZclAttrFlagsRdOnly_c, sizeof(uint8_t), (void *)MbrOfs(zbCommissioningAttributes_t, protocolVersion) },
  { gZclAttrCommissioning_StackProfileID_c,              gZclDataTypeUint8_c, gZclAttrFlagsInRAM_c| gZclAttrFlagsRdOnly_c, sizeof(uint8_t), (void *)MbrOfs(zbCommissioningAttributes_t, stackProfile) },
  { gZclAttrCommissioning_StartupControlID_c,            gZclDataTypeEnum8_c, gZclAttrFlagsInRAM_c, sizeof(uint8_t), (void *)MbrOfs(zbCommissioningAttributes_t, startupControl) },
  { gZclAttrCommissioning_TrustCenterAddressID_c,        gZclDataTypeIeeeAddr_c, gZclAttrFlagsInRAM_c, sizeof(zbIeeeAddr_t), (void *)MbrOfs(zbCommissioningAttributes_t, aTrustCenterAddress) },
  { gZclAttrCommissioning_TrustCenterMasterKeyID_c,      gZclDataTypeSecurityKey_c, gZclAttrFlagsInRAM_c| gZclAttrFlagsRdOnly_c, sizeof(zbAESKey_t), (void *)MbrOfs(zbCommissioningAttributes_t, aTrustCenterMasterKey) },
  { gZclAttrCommissioning_NetworkKeyID_c,                gZclDataTypeSecurityKey_c, gZclAttrFlagsInRAM_c, sizeof(zbAESKey_t), (void *)MbrOfs(zbCommissioningAttributes_t, aNetworkKey) },
  { gZclAttrCommissioning_UseInsecureJoinID_c,           gZclDataTypeUint8_c, gZclAttrFlagsInRAM_c, sizeof(uint8_t), (void *)MbrOfs(zbCommissioningAttributes_t, fUseInsecureJoin) },
  { gZclAttrCommissioning_PreconfiguredLinkKeyID_c,      gZclDataTypeSecurityKey_c, gZclAttrFlagsInRAM_c, sizeof(zbAESKey_t), (void *)MbrOfs(zbCommissioningAttributes_t, aPreconfiguredTrustCenterLinkKey) },
  { gZclAttrCommissioning_NetworkKeySeqNumID_c,          gZclDataTypeUint8_c, gZclAttrFlagsInRAM_c, sizeof(uint8_t), (void *)MbrOfs(zbCommissioningAttributes_t, activeNetworkKeySeqNum) },
  { gZclAttrCommissioning_NetworkKeyTypeID_c,            gZclDataTypeUint8_c, gZclAttrFlagsInRAM_c, sizeof(uint8_t), (void *)MbrOfs(zbCommissioningAttributes_t, networkKeyType) },
  { gZclAttrCommissioning_NetworkManagerAddressID_c,     gZclDataTypeUint16_c, gZclAttrFlagsInRAM_c| gZclAttrFlagsRdOnly_c, sizeof(uint16_t), (void *)MbrOfs(zbCommissioningAttributes_t, aNetworkManagerAddress) },
  { gZclAttrCommissioning_ScanAttemptsID_c,              gZclDataTypeUint8_c, gZclAttrFlagsInRAM_c| gZclAttrFlagsRdOnly_c, sizeof(uint8_t), (void *)MbrOfs(zbCommissioningAttributes_t, scanAttempts) },
  { gZclAttrCommissioning_TimeBetweenScansID_c,          gZclDataTypeUint16_c, gZclAttrFlagsInRAM_c| gZclAttrFlagsRdOnly_c, sizeof(uint16_t), (void *)MbrOfs(zbCommissioningAttributes_t, aTimeBetweenScans) },
  { gZclAttrCommissioning_RejoinIntervalID_c,            gZclDataTypeUint16_c, gZclAttrFlagsInRAM_c| gZclAttrFlagsRdOnly_c, sizeof(uint16_t), (void *)MbrOfs(zbCommissioningAttributes_t, aRejoinInterval) },
  { gZclAttrCommissioning_MaxRejoinIntervalID_c,         gZclDataTypeUint16_c, gZclAttrFlagsInRAM_c| gZclAttrFlagsRdOnly_c, sizeof(uint16_t), (void *)MbrOfs(zbCommissioningAttributes_t, aMaxRejoinInterval) },
  { gZclAttrCommissioning_IndirectPollRateID_c,          gZclDataTypeUint16_c, gZclAttrFlagsInRAM_c| gZclAttrFlagsRdOnly_c, sizeof(uint16_t), (void *)MbrOfs(zbCommissioningAttributes_t, aIndirectPollRate) },
  { gZclAttrCommissioning_ParentRetryThreshoID_c,        gZclDataTypeUint8_c, gZclAttrFlagsInRAM_c| gZclAttrFlagsRdOnly_c, sizeof(uint8_t), (void *)MbrOfs(zbCommissioningAttributes_t, parentLinkRetryThreshold) },
  { gZclAttrCommissioning_ConcentratorFlagID_c,          gZclDataTypeBool_c, gZclAttrFlagsInRAM_c| gZclAttrFlagsRdOnly_c, sizeof(bool_t), (void *)MbrOfs(zbCommissioningAttributes_t, fConcentratorFlag) },
  { gZclAttrCommissioning_ConcentratorRadiusID_c,        gZclDataTypeUint8_c, gZclAttrFlagsInRAM_c| gZclAttrFlagsRdOnly_c, sizeof(uint8_t), (void *)MbrOfs(zbCommissioningAttributes_t, concentratorRadius) },
  { gZclAttrCommissioning_ConcentratorDiscoveryTimeID_c, gZclDataTypeUint8_c, gZclAttrFlagsInRAM_c| gZclAttrFlagsRdOnly_c, sizeof(uint8_t), (void *)MbrOfs(zbCommissioningAttributes_t, concentratorDiscoveryTime) }
};

const zclAttrDefList_t gZclCommissioningServerClusterAttrDefList = {
  NumberOfElements(gaZclCommisioningServerAttrDef),
  gaZclCommisioningServerAttrDef
};



ZdoStartMode_t   gStartUpMode_DeviceType = 0;

/*

  This cluster allows over-the-air updates of key commissioning values such as PAN ID, extended PAN 
  ID and preconfigured security keys. It can also restart the remote node. This allows a 
  commissioning tool to set up a ZigBee node on a "commissioning" network, then tell the node to go
  join the "final" network.
*/
zbStatus_t ZCL_CommisioningClusterServer
(
  zbApsdeDataIndication_t *pIndication,   /* IN: must be non-null */
  afDeviceDef_t *pDevice                  /* IN: must be non-null */
)
{
  zclFrame_t *pFrame;
  zclCommissioningCmd_t *Command; 
  zbStatus_t status = gZclSuccess_c;
  
  (void)pDevice;

  /* ZCL frame */
  pFrame = (zclFrame_t*)pIndication->pAsdu;
  gZcl_ep = pIndication->dstEndPoint;
  Command = (zclCommissioningCmd_t *) (&(pFrame->command) + sizeof(uint8_t));


  switch (pFrame->command)
  {
    case gZclCmdCommissiong_RestartDeviceRequest_c:
      if((Command->RestartDeviceCmd.Options & gCommisioningClusterRestartDeviceRequestOptions_StartUpModeSubField_Mask_c) ==
          gCommisioningClusterRestartDeviceRequestOptions_ModeSubField_RestartUsingCurrentStartupParameters_c) 
      {
       /*
         Consistency check is done on:
         Start up control (form network) if device is not capable of being a coordinator
         Start up control (silent start) if stack profile is 1.
         Pan ID is checked whether it is within range
       */

#if (!(gComboDeviceCapability_d || gCoordinatorCapability_d))
        /* Form network startup only allowed for coordinator or combo device */
        if(gCommisioningServerAttrsData.startupControl == gStartupControl_Form_c) {
          status = gZclInconsistentStatupState_c;
        }
#endif
        // Check that PAN ID is set to 0xFFFF or below 0xFFFE
        if ((gCommisioningServerAttrsData.aPanId[0] != 0xFF) && (gCommisioningServerAttrsData.aPanId[1] != 0xFF))
        {
          if(gCommisioningServerAttrsData.aPanId[0] > gaPIDUpperLimit[0]) 
          {
            status = gZclInconsistentStatupState_c;
          }
        }

        if (gCommisioningServerAttrsData.startupControl == gStartupControl_SilentStart_c)
        {
#if (gDefaultValueOfNwkStackProfile_c == 0x01)
          /* consistency check for stack profile 1 */
          status = gZclInconsistentStatupState_c;
#else
          status = gZclSuccess_c;
#endif
        }

        RestartFromAttributeSet = TRUE;
      } else

        if((Command->RestartDeviceCmd.Options & gCommisioningClusterRestartDeviceRequestOptions_StartUpModeSubField_Mask_c) ==
            gCommisioningClusterRestartDeviceRequestOptions_ModeSubField_RestartUsingCurrentStackParameter_c) 
        {
          RestartFromAttributeSet = FALSE;
        }
        else
        {
          // invalid command, return error code.
          status = gZclUnsupportedClusterCommand_c;
        }

        /*
          if the device is a combo then determine the Type of Devices for the
          start mode.
        */
        gStartUpMode_DeviceType = gCommisioningServerAttrsData.startupControl == gStartupControl_Form_c ? gZdoStartMode_Zc_c:gZdoStartMode_Zr_c;

        /* Set up start delay and just and initiate leave */
        RestartDelay = Command->RestartDeviceCmd.Delay;
        RestartJitter = GetRandomRange(0,Command->RestartDeviceCmd.Jitter) * 80;
        CommisioningTimer = TMR_AllocateTimer();
        status = gZclInconsistentStatupState_c;
        if (CommisioningTimer != gTmrInvalidTimerID_c)
        {
          /*Start timer.with +200 milliseconds so the ZCL response is sent before leave is initiated.*/
          TMR_StartSingleShotTimer(CommisioningTimer,200,LeaveTimerCallback);
          status = gZclSuccess_c;
        }

        if (status != gZclSuccess_c) 
        {
          (void) ZCL_Reply(pIndication, sizeof(status), &status);
          return gZclSuccess_c;
        }
    break;

    case gZclCmdCommissiong_SaveStartupParametersRequest_c:
      // Optional - not supported.
      status = gZclUnsupportedClusterCommand_c;
    break;

    case gZclCmdCommissiong_RestoreStartupParametersRequest_c:
      // Optional - not supported.
      status = gZclUnsupportedClusterCommand_c;
    break;

    case gZclCmdCommissiong_ResetStartupParametersRequest_c:
      if ((Command->ResetStartupParameterCmd.Options & gCommisioningClusterResetStartupParametersRequestOptions_ResetCurrentSubField_Mask_c) ||
          (Command->ResetStartupParameterCmd.Options & gCommisioningClusterResetStartupParametersRequestOptions_ResetAllSubField_Mask_c)) 
      {
        /*Copy set from ROM to ram */ 
        FLib_MemCpy(&gCommisioningServerAttrsData, (void *) &gSAS_Rom, sizeof(gSAS_Rom));
      }
      else
        if (Command->ResetStartupParameterCmd.Options & gCommisioningClusterResetStartupParametersRequestOptions_EraseIndexSubField_Mask_c) 
        {
          /* any other combination return error */  
          status = gZclUnsupportedClusterCommand_c; 
        }
    break;
    
  default:
    return gZclUnsupportedClusterCommand_c;
  }

  (void)ZCL_Reply(pIndication, sizeof(status), &status);
  return gZclSuccess_c;
}

zbStatus_t ZCL_CommisioningClusterClient
(
  zbApsdeDataIndication_t *pIndication,   /* IN: must be non-null */
  afDeviceDef_t *pDevice                  /* IN: must be non-null */
)
{
  zclFrame_t *pFrame;
/*  zclCmdCommissiong_response_t *Command; */
 
  (void)pDevice;

  /* ZCL frame */
  pFrame = (zclFrame_t*)pIndication->pAsdu;
  gZcl_ep = pIndication->dstEndPoint;
/*  Command = (zclCmdCommissiong_response_t *) (&(pFrame->command) + sizeof(uint8_t));*/


  switch (pFrame->command)
  {
    case gZclCmdCommissiong_RestartDeviceResponse_c:
    case gZclCmdCommissiong_SaveStartupParametersResponse_c:
    case gZclCmdCommissiong_RestoreStartupParametersResponse_c:
    case gZclCmdCommissiong_ResetStartupParametersResponse_c:
      return gZbSuccess_c;   
    default:
      return gZclUnsupportedClusterCommand_c;      
  }
  }


/* Commissioning cluster Client commands */
#if gASL_ZclCommissioningRestartDeviceRequest_d
zbStatus_t  ZCL_Commisioning_RestartDeviceReq
(
	zclCommissioningRestartDeviceReq_t *pReq
)
{
	Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterCommissioning_c);
   return ZCL_GenericReq(gZclCmdCommissiong_RestartDeviceRequest_c, sizeof(zclCmdCommissiong_RestartDeviceRequest_t),(zclGenericReq_t *)pReq);	
}
#endif


#if gASL_ZclCommissioningSaveStartupParametersRequest_d
zbStatus_t  ZCL_Commisioning_SaveStartupParametersReq
(
	zclCommissioningSaveStartupParametersReq_t *pReq
)
{
	Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterCommissioning_c);
   return ZCL_GenericReq(gZclCmdCommissiong_SaveStartupParametersRequest_c, sizeof(zclCmdCommissiong_SaveStartupParametersRequest_t),(zclGenericReq_t *)pReq);	
}
#endif


#if gASL_ZclCommissioningRestoreStartupParametersRequest_d
zbStatus_t  ZCL_Commisioning_RestoreStartupParametersReq
(
	zclCommissioningRestoreStartupParametersReq_t *pReq
)
{
	Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterCommissioning_c);
   return ZCL_GenericReq(gZclCmdCommissiong_RestoreStartupParametersRequest_c, sizeof(zclCmdCommissiong_RestoreStartupParametersRequest_t),(zclGenericReq_t *)pReq);	
}
#endif



#if gASL_ZclCommissioningResetStartupParametersRequest_d
zbStatus_t  ZCL_Commisioning_ResetStartupParametersReq
(
	zclCommissioningResetStartupParametersReq_t *pReq
)
{
	Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterCommissioning_c);
   return ZCL_GenericReq(gZclCmdCommissiong_ResetStartupParametersRequest_c, sizeof(zclCmdCommissiong_ResetStartupParametersRequest_t),(zclGenericReq_t *)pReq);	
} 
#endif


/**************************************************************************
	Alpha-Secure Key Establishment Cluster (Health Care Profile Annex A.2)
***************************************************************************/

zbStatus_t ZCL_ASKEClusterClient
(
zbApsdeDataIndication_t *pIndication, 
afDeviceDef_t *pDev
)
{
    zclFrame_t *pFrame;
    zclCmd_t command;

    /* prevent compiler warning */
    (void)pDev;
    pFrame = (void *)pIndication->pAsdu;
    /* handle the command */
    command = pFrame->command;  
    switch(command)
    {
        case gZclCmdTxASKE_ConfigureSDResponse_c:
        case gZclCmdTxASKE_UpdateRevocationListResponse_c:
        case gZclCmdTxASKE_RemoveSDResponse_c:
        case gZclCmdTxASKE_ReadSDResponse_c:
        case gZclCmdTxASKE_InitiateASKEResponse_c:
        case gZclCmdTxASKE_ConfirmASKEkeyResponse_c:
        case gZclCmdRxASKE_TerminateASKE_c:
        case gZclCmdTxASKE_GenerateAMKResponse_c:
        case gZclCmdTxASKE_ReportRevokedNode_c:
        case gZclCmdTxASKE_RequestConfiguration_c:

          /* These should be passed up to a host*/
          return gZbSuccess_c;
        default:
          return gZclUnsupportedClusterCommand_c;
    }
}


zbStatus_t ZCL_ASKEClusterServer
(
zbApsdeDataIndication_t *pIndication, 
afDeviceDef_t *pDev
)
{
    zclFrame_t *pFrame;
    zclCmd_t command;

    /* prevent compiler warning */
    (void)pDev;
    pFrame = (void *)pIndication->pAsdu;
    /* handle the command */
    command = pFrame->command;  
    switch(command)
    {
        case gZclCmdRxASKE_ConfigureSD_c:
        case gZclCmdRxASKE_UpdateRevocationList_c:
        case gZclCmdRxASKE_RemoveSD_c:
        case gZclCmdRxASKE_ReadSD_c:
        case gZclCmdRxASKE_InitiateASKE_c:
        case gZclCmdRxASKE_ConfirmASKEkey_c:
        case gZclCmdRxASKE_TerminateASKE_c:
        case gZclCmdRxASKE_GenerateAMK_c:

          /* These should be passed up to a host*/
          return gZbSuccess_c;
        default:
          return gZclUnsupportedClusterCommand_c;
    }
}




zbStatus_t ASKE_ConfigureSD
(
zclCmdASKE_ConfigureSD_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASKE_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxASKE_ConfigureSD_c,0,(zclGenericReq_t *)pReq);
}


zbStatus_t ASKE_UpdateRevocationList
(
zclCmdASKE_UpdateRevocationList_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASKE_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxASKE_UpdateRevocationList_c,0,(zclGenericReq_t *)pReq);
}


zbStatus_t ASKE_RemoveSD
(
zclCmdASKE_RemoveSD_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASKE_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxASKE_RemoveSD_c,0,(zclGenericReq_t *)pReq);
}


zbStatus_t ASKE_ReadSD
(
zclCmdASKE_ReadSD_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASKE_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxASKE_ReadSD_c,0,(zclGenericReq_t *)pReq);
}


zbStatus_t ASKE_InitiateASKE
(
zclCmdASKE_InitiateASKE_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASKE_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxASKE_InitiateASKE_c,0,(zclGenericReq_t *)pReq);
}


zbStatus_t ASKE_ConfirmASKEkey
(
zclCmdASKE_ConfirmASKEkey_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASKE_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxASKE_ConfirmASKEkey_c,0,(zclGenericReq_t *)pReq);
}


zbStatus_t ASKE_TerminateASKE
(
zclCmdASKE_TerminateASKE_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASKE_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxASKE_TerminateASKE_c,0,(zclGenericReq_t *)pReq);
}


zbStatus_t ASKE_GenerateAMK
(
zclCmdASKE_GenerateAMK_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASKE_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxASKE_GenerateAMK_c,0,(zclGenericReq_t *)pReq);
}



zbStatus_t ASKE_ConfigureSDresponse
(
zclCmdASKE_ConfigureSDresponse_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASKE_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdTxASKE_ConfigureSDResponse_c,0,(zclGenericReq_t *)pReq);
}


zbStatus_t ASKE_UpdateRevocationListResponse
(
zclCmdASKE_UpdateRevocationListResponse_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASKE_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdTxASKE_UpdateRevocationListResponse_c,0,(zclGenericReq_t *)pReq);
}


zbStatus_t ASKE_RemoveSDresponse
(
zclCmdASKE_RemoveSDresponse_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASKE_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdTxASKE_RemoveSDResponse_c,0,(zclGenericReq_t *)pReq);
}


zbStatus_t ASKE_ReadSDresponse
(
zclCmdASKE_ReadSDresponse_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASKE_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdTxASKE_ReadSDResponse_c,0,(zclGenericReq_t *)pReq);
}


zbStatus_t ASKE_InitiateASKEresponse
(
zclCmdASKE_InitiateASKEresponse_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASKE_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdTxASKE_InitiateASKEResponse_c,0,(zclGenericReq_t *)pReq);
}


zbStatus_t ASKE_ConfirmASKEkeyResponse
(
zclCmdASKE_ConfirmASKEkeyResponse_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASKE_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdTxASKE_ConfirmASKEkeyResponse_c,0,(zclGenericReq_t *)pReq);
}



//zbStatus_t ASKE_TerminateASKE(zclCmdASKE_TerminateASKE_t *pReq){}


zbStatus_t ASKE_GenerateAMKresponse
(
zclCmdASKE_GenerateAMKresponse_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASKE_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdTxASKE_GenerateAMKResponse_c,0,(zclGenericReq_t *)pReq);
}


zbStatus_t ASKE_ReportRevokedNode
(
zclCmdASKE_ReportRevokedNode_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASKE_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdTxASKE_ReportRevokedNode_c,0,(zclGenericReq_t *)pReq);
}


zbStatus_t ASKE_RequestConfiguration
(
zclCmdASKE_RequestConfiguration_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASKE_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdTxASKE_RequestConfiguration_c,0,(zclGenericReq_t *)pReq);
}


/**************************************************************************
	Alpha-Secure Access Control Cluster (Health Care Profile Annex A.3)
***************************************************************************/

zbStatus_t ZCL_ASACClusterClient
(
zbApsdeDataIndication_t *pIndication, 
afDeviceDef_t *pDev
)
{
zclFrame_t *pFrame;
    zclCmd_t command;

    /* prevent compiler warning */
    (void)pDev;
    pFrame = (void *)pIndication->pAsdu;
    /* handle the command */
    command = pFrame->command;  
    switch(command)
    {
        case gZclCmdTxASAC_ConfigureSDresponse_c:
        case gZclCmdTxASAC_UpdateRevocationListResponse_c:
        case gZclCmdTxASAC_ConfigureACpoliciesResponse_c:
        case gZclCmdTxASAC_ReadSDresponse_c:
        case gZclCmdTxASAC_ReadACpoliciesResponse_c:
        case gZclCmdTxASAC_RemoveACresponse_c:
        case gZclCmdTxASAC_ACpropertiesResponse_c:
        case gZclCmdTxASAC_TSreport_c:
        case gZclCmdTxASAC_InitiateASACresponse_c:
        case gZclCmdTxASAC_ConfirmASACkeyResponse_c:
        case gZclCmdTxASAC_TerminateASAC_c:
        case gZclCmdTxASAC_GenerateAMKresponse_c:
        case gZclCmdTxASAC_LDCtransportResponse_c:
        case gZclCmdTxASAC_RequestConfiguration_c:

          /* These should be passed up to a host*/
          return gZbSuccess_c;
        default:
          return gZclUnsupportedClusterCommand_c;
    }
}

zbStatus_t ZCL_ASACClusterServer
(zbApsdeDataIndication_t *pIndication, 
afDeviceDef_t *pDev)
{
zclFrame_t *pFrame;
    zclCmd_t command;

    /* prevent compiler warning */
    (void)pDev;
    pFrame = (void *)pIndication->pAsdu;
    /* handle the command */
    command = pFrame->command;  
    switch(command)
    {
        case gZclCmdRxASAC_ConfigureSD_c:
        case gZclCmdRxASAC_UpdateRevocationList_c:
        case gZclCmdRxASAC_ConfigureACpolicies_c:
        case gZclCmdRxASAC_ReadSD_c:
        case gZclCmdRxASAC_ReadACpolicies_c:
        case gZclCmdRxASAC_RemoveAC_c:
        case gZclCmdRxASAC_ACpropertiesReq_c:
        case gZclCmdRxASAC_TSreportResponse_c:
        case gZclCmdRxASAC_InitiateASAC_c:
        case gZclCmdRxASAC_ConfirmASACkey_c:
        case gZclCmdRxASAC_TerminateASAC_c:
        case gZclCmdRxASAC_GenerateAMK_c:
        case gZclCmdRxASAC_LDCtransport_c:
        
          /* These should be passed up to a host*/
          return gZbSuccess_c;
        default:
          return gZclUnsupportedClusterCommand_c;
    }
}


zbStatus_t ASAC_ConfigureSD
(
  zclCmdASAC_ConfigureSD_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASAC_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxASAC_ConfigureSD_c,0,(zclGenericReq_t *)pReq);
}


zbStatus_t ASAC_UpdateRevocationList
(
  zclCmdASAC_UpdateRevocationList_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASAC_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxASAC_UpdateRevocationList_c,0,(zclGenericReq_t *)pReq);
}


zbStatus_t ASAC_ConfigureACpolicies
(
  zclCmdASAC_ConfigureACpolicies_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASAC_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxASAC_ConfigureACpolicies_c,0,(zclGenericReq_t *)pReq);
}


zbStatus_t ASAC_ReadSD
(
  zclCmdASAC_ReadSD_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASAC_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxASAC_ReadSD_c,0,(zclGenericReq_t *)pReq);
}


zbStatus_t ASAC_ReadACpolicies
(
  zclCmdASAC_ReadACpolicies_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASAC_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxASAC_ReadACpolicies_c,0,(zclGenericReq_t *)pReq);
}


zbStatus_t ASAC_RemoveAC
(
  zclCmdASAC_RemoveAC_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASAC_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxASAC_RemoveAC_c,0,(zclGenericReq_t *)pReq);
}


zbStatus_t ASAC_ACpropertiesRequest(void)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASAC_c);	
//    return ZCL_SendClientReqSeqPassed(gZclCmdRxASAC_ACpropertiesReq_c,0,(zclGenericReq_t *)pReq);
  return gZbSuccess_c;
}


zbStatus_t ASAC_TSReportResponse(void)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASAC_c);	
//    return ZCL_SendClientReqSeqPassed(gZclCmdRxASAC_TSreportResponse_c,0,(zclGenericReq_t *)pReq);
return gZbSuccess_c;
}


zbStatus_t ASAC_InitiateASAC
(
  zclCmdASAC_InitiateASAC_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASAC_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxASAC_InitiateASAC_c,0,(zclGenericReq_t *)pReq);
}


zbStatus_t ASAC_ConfirmASACkey
(
  zclCmdASAC_ConfirmASAC_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASAC_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxASAC_ConfirmASACkey_c,0,(zclGenericReq_t *)pReq);
}


zbStatus_t ASAC_TerminateASAC
(
  zclCmdASAC_TerminateASAC_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASAC_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxASAC_TerminateASAC_c,0,(zclGenericReq_t *)pReq);
}


zbStatus_t ASAC_GenerateAMK
(
  zclCmdASAC_GenerateAMK_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASAC_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxASAC_GenerateAMK_c,0,(zclGenericReq_t *)pReq);
}


zbStatus_t ASAC_LDCtransport
(
  zclCmdASAC_LDCtransport_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASAC_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxASAC_LDCtransport_c,0,(zclGenericReq_t *)pReq);
}


zbStatus_t ASAC_ConfigureSDresponse
(
  zclCmdASAC_ConfigureSDresponse_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASAC_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdTxASAC_ConfigureSDresponse_c,0,(zclGenericReq_t *)pReq);
}


zbStatus_t ASAC_UpdateRevocationListResponse
(
  zclCmdASAC_UpdateRevocationListResponse_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASAC_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdTxASAC_UpdateRevocationListResponse_c,0,(zclGenericReq_t *)pReq);
}


zbStatus_t ASAC_ConfigureACpoliciesResponse
(
  zclCmdASAC_ConfigureACpoliciesResponse_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASAC_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdTxASAC_ConfigureACpoliciesResponse_c,0,(zclGenericReq_t *)pReq);
}


zbStatus_t ASAC_ReadSDresponse
(
  zclCmdASAC_ReadSDresponse_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASAC_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdTxASAC_ReadSDresponse_c,0,(zclGenericReq_t *)pReq);
}


zbStatus_t ASAC_ReadACpoliciesResponse
(
  zclCmdASAC_ReadACpoliciesResponse_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASAC_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdTxASAC_ReadACpoliciesResponse_c,0,(zclGenericReq_t *)pReq);
}


zbStatus_t ASAC_RemoveACresponse
(
  zclCmdASAC_RemoveACresponse_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASAC_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdTxASAC_RemoveACresponse_c,0,(zclGenericReq_t *)pReq);
}


zbStatus_t ASAC_ACpropertiesResponse
(
  zclCmdASAC_ACpropertiesResponse_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASAC_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdTxASAC_ACpropertiesResponse_c,0,(zclGenericReq_t *)pReq);
}


zbStatus_t ASAC_TSReport
(
  zclCmdASAC_TSreport_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASAC_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdTxASAC_TSreport_c,0,(zclGenericReq_t *)pReq);
}


zbStatus_t ASAC_InitiateASACresponse
(
  zclCmdASAC_InitiateASACresponse_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASAC_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdTxASAC_InitiateASACresponse_c,0,(zclGenericReq_t *)pReq);
}


zbStatus_t ASAC_ConfirmASACkeyResponse
(
  zclCmdASAC_ConfirmASACkeyResponse_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASAC_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdTxASAC_ConfirmASACkeyResponse_c,0,(zclGenericReq_t *)pReq);
}


//zbStatus_t ASAC_TerminateASAC(zclCmdASAC_TerminateASAC_t *pReq){}


zbStatus_t ASAC_GenerateAMKresponse
(
  zclCmdASAC_GenerateAMKresponse_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASAC_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdTxASAC_GenerateAMKresponse_c,0,(zclGenericReq_t *)pReq);
}


zbStatus_t ASAC_LDCtransportResponse
(
  zclCmdASAC_LDCtransportResponse_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASAC_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdTxASAC_LDCtransportResponse_c,0,(zclGenericReq_t *)pReq);
}


zbStatus_t ASAC_RequestConfiguration
(
  zclCmdASAC_RequestConfiguration_t *pReq
)
{
  // pReq->zclTransactionId = gZclTransactionId++;
//    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterASAC_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdTxASAC_RequestConfiguration_c,0,(zclGenericReq_t *)pReq);
}


/*****************************************************************************
* Leave timer callback, executes the leave commands, and set up a timer to restart ZDO.
******************************************************************************/
void LeaveTimerCallback(tmrTimerID_t timerid) {

  /* Leave network, but keep the current in RAM defaults (for example, node type in a Zx Combo device) */
  ZDO_StopEx(gZdoStopMode_Announce_c | gZdoStopMode_ResetNvm_c);

  /*setup timer for rejoining, a 200 msec is added to insure that the leave command has happend*/  
  TMR_StartSingleShotTimer(timerid,((tmrTimeInMilliseconds_t)(RestartDelay) * 1000) + RestartJitter, StartTimerCallback);
}

/*****************************************************************************
* Start timer callback, frees timer used, and restart ZDO
******************************************************************************/
void StartTimerCallback(tmrTimerID_t timerid) {
 
 
  if (ZDO_GetState() != gZdoInitialState_c)
  {
    /*
      if ZDO state machine has not yet left the network and moved to initial state then,
      check status again after 50 milliseconds
    */
    TMR_StartSingleShotTimer(timerid,(tmrTimeInMilliseconds_t)50,StartTimerCallback);
    return;
  }

  /*Free timer, as we do not need it anymore*/
  TMR_FreeTimer(timerid);

  /* Start restart the ZDO with the commisioning cluster settings */ 
  if (RestartFromAttributeSet)
  {
    ASL_SetCommissioningSAS(&gCommisioningServerAttrsData);
    /* Note whether rejoin or associate should be done is determined by the ZDO when a SAS set is used
    */
    ZDO_Start(gStartUpMode_DeviceType | gZdoStartMode_SasSet_c | gZdoStartMode_Associate_c);
  }
  else
  {
    ASL_SetCommissioningSAS(NULL);
    ZDO_Start(gStartUpMode_DeviceType | gZdoStartMode_RamSet_c | gZdoStartMode_Associate_c);
  }
}


/******************************************************************************
*******************************************************************************
* Private functions
*******************************************************************************
******************************************************************************/

/*****************************************************************************
* ZCL_ReplyNoCopy
*
* Used for longer replies that must be built in-place.
*****************************************************************************/
zbStatus_t ZCL_ReplyNoCopy
  (
  zbApsdeDataIndication_t *pIndication, /* IN: indication that came from other node */
  uint8_t payloadLen,       /* IN: payload length, not counting ZCL frame (3-byte hdr)  */
  afToApsdeMessage_t *pMsg  /* IN: message, with payload at gAsduOffset_c + sizeof(zclFrame_t) */
  )
{
  afAddrInfo_t addrInfo;
  uint8_t frameControl;

  /* get address ready for reply */
  AF_PrepareForReply(&addrInfo, pIndication);
  addrInfo.radiusCounter = gDefaultRadiusCounter;
  
  frameControl = ((zclFrame_t *)pIndication->pAsdu)->frameControl;
  
  /* setup the reply frame */
  ZCL_SetupFrame((void *)(((uint8_t *)pMsg) + gAsduOffset_c), (void *)pIndication->pAsdu);
  
  if (frameControl & gZclFrameControl_MfgSpecific)
    payloadLen += sizeof(zclMfgFrame_t);
  else
    payloadLen += sizeof(zclFrame_t);

  /* send it over the air */
  return ZCL_DataRequestNoCopy(&addrInfo, payloadLen, pMsg);
}


/*****************************************************************************
* ZCL_Reply
*
* Used for short replies (that do not need to be built in-place).
*****************************************************************************/
zbStatus_t ZCL_Reply
  (
  zbApsdeDataIndication_t *pIndication,   /* IN: */
  uint8_t payloadLen,                     /* IN: */
  void *pPayload                          /* IN: */
  )
{
  afToApsdeMessage_t *pMsg;

  /* allocate space for the message */
  pMsg = AF_MsgAlloc();
  if(!pMsg)
    return gZclNoMem_c;

  /* copy in payload */
  FLib_MemCpy(((uint8_t *)pMsg) + gAsduOffset_c + sizeof(zclFrame_t),
    pPayload, payloadLen);

  /* send it over the air */
  return ZCL_ReplyNoCopy(pIndication, payloadLen, pMsg);
}


uint8_t *ZCL_InterPanCreatePayload
  (
  zclCmd_t command,               /* IN: command */
  zclFrameControl_t frameControl, /* IN: frame control field */
  uint8_t *pPayloadLen,           /* IN/OUT: length of payload (then adjusted to length of asdu) */
  uint8_t *pPayload                  /* IN: payload after frame (first byte from pPayload is transactionID)*/
  )
{
  uint8_t *pMsg;
  zclFrame_t *pFrame;
  *pPayloadLen += sizeof(zclFrame_t);
  /* allocate space for the message */
  pMsg = MSG_Alloc(*pPayloadLen);
  if(!pMsg)
    return NULL;
  BeeUtilZeroMemory(pMsg, *pPayloadLen);
  /* set up the frame */
  pFrame = (zclFrame_t *)pMsg;
  pFrame->frameControl = frameControl;
  pFrame->transactionId = pPayload[0];
  pFrame->command = command;
  if(pPayload && (*pPayloadLen))
    /* copy the payload, skip over first byte which is the zcl transaction ID*/
    FLib_MemCpy((pFrame + 1), (pPayload+1), (*pPayloadLen));
  /* return ptr to msg buffer */
  return pMsg;
}

/*****************************************************************************
* ZCL_GenericReq
*
* This request works for many of the request types that don't need to parse the
* data before sending.
*
* Returns the gHaNoMem_c if not enough memory to send.
*****************************************************************************/
zclStatus_t ZCL_GenericReq(zclCmd_t command, uint8_t iPayloadLen, zclGenericReq_t *pReq)
{
  afToApsdeMessage_t *pMsg;

  /* create a ZCL frame and copy in the payload */
  pMsg = ZCL_CreateFrame(&(pReq->addrInfo),command,gZclFrameControl_FrameTypeSpecific,
    NULL, &iPayloadLen,&(pReq->cmdFrame));
  if(!pMsg)
    return gZclNoMem_c;

  /* send packet over the air */
  return ZCL_DataRequestNoCopy(&(pReq->addrInfo), iPayloadLen, pMsg);
}



/* Send Req from Client to Server with Zcl Transaction Sequence Id passed as parameter */  
zclStatus_t ZCL_SendClientReqSeqPassed(zclCmd_t command, uint8_t iPayloadLen, zclGenericReq_t *pReq)
{
  afToApsdeMessage_t *pMsg;

  /* create a ZCL frame and copy in the payload */
  pMsg = ZCL_CreateFrame(&(pReq->addrInfo), command, gZclFrameControl_FrameTypeSpecific,
         &(pReq->cmdFrame),&iPayloadLen, ((uint8_t*)&(pReq->cmdFrame)+1));
  if(!pMsg)
    return gZclNoMem_c;

  /* send packet over the air */
  return ZCL_DataRequestNoCopy(&(pReq->addrInfo), iPayloadLen, pMsg);
}

/* Send InterPan Req from Client to Server with Zcl Transaction Sequence Id passed as parameter */  
zclStatus_t ZCL_SendInterPanClientReqSeqPassed(zclCmd_t command, uint8_t iPayloadLen, void *pReq)
{
 
  uint8_t *pPayload;
  /* create a ZCL frame */
  pPayload = ZCL_InterPanCreatePayload(command, gZclFrameControl_FrameTypeSpecific ,
                                       &iPayloadLen, ((uint8_t *)pReq + sizeof(InterPanAddrInfo_t)) );
  if(!pPayload)
    return gZclNoMem_c;
  /* send packet over the air */
  return AF_InterPanDataRequest((InterPanAddrInfo_t *)pReq, iPayloadLen, pPayload, NULL);
}
/* Send Req From Server to Client with Zcl Transaction Sequence Id passed as parameter */
zclStatus_t ZCL_SendServerReqSeqPassed(zclCmd_t command, uint8_t iPayloadLen, zclGenericReq_t *pReq)
{
  afToApsdeMessage_t *pMsg;

  /* create a ZCL frame and copy in the payload */
  pMsg = ZCL_CreateFrame(&(pReq->addrInfo),command, (gZclFrameControl_FrameTypeSpecific | gZclFrameControl_DirectionRsp),
          &(pReq->cmdFrame), &iPayloadLen, ((uint8_t*)&(pReq->cmdFrame)+1));
  if(!pMsg)
    return gZclNoMem_c;
  
  /* send packet over the air */
  return ZCL_DataRequestNoCopy(&(pReq->addrInfo), iPayloadLen, pMsg);
}

/* Send InterPan Req from Server to Client with Zcl Transaction Sequence Id passed as parameter */  
zclStatus_t ZCL_SendInterPanServerReqSeqPassed(zclCmd_t command, uint8_t iPayloadLen, void *pReq)
{
 
  uint8_t *pPayload;
  /* create a ZCL frame */
  pPayload = ZCL_InterPanCreatePayload(command, (gZclFrameControl_FrameTypeSpecific | gZclFrameControl_DirectionRsp) ,
                                       &iPayloadLen, ((uint8_t *)pReq + sizeof(InterPanAddrInfo_t)) );
  if(!pPayload)
    return gZclNoMem_c;
  /* send packet over the air */
  return AF_InterPanDataRequest((InterPanAddrInfo_t *)pReq, iPayloadLen, pPayload, NULL);
}

/* Send Rsp From Server To Client with Zcl Transaction Sequence Id passed as parameter */
zclStatus_t ZCL_SendServerRspSeqPassed(zclCmd_t command, uint8_t iPayloadLen, zclGenericReq_t *pReq)
{
  afToApsdeMessage_t *pMsg;

  /* create a ZCL frame and copy in the payload */
  pMsg = ZCL_CreateFrame(&(pReq->addrInfo),command, (gZclFrameControl_FrameTypeSpecific | gZclFrameControl_DirectionRsp |gZclFrameControl_DisableDefaultRsp) ,
    &(pReq->cmdFrame), &iPayloadLen, ((uint8_t*)&(pReq->cmdFrame)+1));
  if(!pMsg)
    return gZclNoMem_c;

  /* send packet over the air */
  return ZCL_DataRequestNoCopy(&(pReq->addrInfo), iPayloadLen, pMsg);
}


/* Send InterPan Rsp From Server To Client with Zcl Transaction Sequence Id passed as parameter */
zclStatus_t ZCL_SendInterPanServerRspSeqPassed(zclCmd_t command, uint8_t iPayloadLen, void*pReq)
{
  uint8_t *pPayload;
  /* create a ZCL frame */
  pPayload = ZCL_InterPanCreatePayload(command, (gZclFrameControl_FrameTypeSpecific | gZclFrameControl_DirectionRsp |gZclFrameControl_DisableDefaultRsp) ,
        &iPayloadLen, ((uint8_t *)pReq + sizeof(InterPanAddrInfo_t)) );
  if(!pPayload)
    return gZclNoMem_c;
  /* send packet over the air */
  return AF_InterPanDataRequest((InterPanAddrInfo_t *)pReq, iPayloadLen, pPayload, NULL);
}

/* Send Rsp From Client to Server with Zcl Transaction Sequence Id passed as parameter */
zclStatus_t ZCL_SendClientRspSeqPassed(zclCmd_t command, uint8_t iPayloadLen, zclGenericReq_t *pReq)
{
  afToApsdeMessage_t *pMsg;
  
  /* create a ZCL frame and copy in the payload */
  pMsg = ZCL_CreateFrame(&(pReq->addrInfo), command,(gZclFrameControl_FrameTypeSpecific |gZclFrameControl_DisableDefaultRsp),
          &(pReq->cmdFrame), &iPayloadLen, ((uint8_t*)&(pReq->cmdFrame)+1));
  if(!pMsg)
    return gZclNoMem_c;

  /* send packet over the air */
  return ZCL_DataRequestNoCopy(&(pReq->addrInfo), iPayloadLen, pMsg);
}

/* Send InterPan Rsp From Client To Server with Zcl Transaction Sequence Id passed as parameter */
zclStatus_t ZCL_SendInterPanClientRspSeqPassed(zclCmd_t command, uint8_t iPayloadLen, void*pReq)
{
  uint8_t *pPayload;
  /* create a ZCL frame */
  pPayload = ZCL_InterPanCreatePayload(command, (gZclFrameControl_FrameTypeSpecific |gZclFrameControl_DisableDefaultRsp) ,
        &iPayloadLen, ((uint8_t *)pReq + sizeof(InterPanAddrInfo_t)) );
  if(!pPayload)
    return gZclNoMem_c;
  /* send packet over the air */
  return AF_InterPanDataRequest((InterPanAddrInfo_t *)pReq, iPayloadLen, pPayload, NULL);
}


/*****************************************************************************/
#if gZclClusterOptionals_d
zbStatus_t ZCL_SetValueAttr 
  (
  zclSetAttrValue_t *pSetAttrValueReq
  )
{
  afDeviceDef_t  *pDeviceDef;
  afClusterDef_t *pClusterDef;
  zclAttrDef_t   *pAttrDef;
  zbClusterId_t  aClusterId;
  
#if gZclEnableThermostat_c || gASL_ZclMet_Optionals_d 
  uint8_t alarmCode = 0;
#endif

#if gZclEnableThermostat_c
  uint8_t alarmMask = 0;
  uint8_t valueAttr = 0;
#endif  
  
#if gASL_ZclMet_Optionals_d 
  uint16_t alarmMask = 0;
  uint16_t valueAttr = 0;
#endif  
  
  
#if gZclEnableOccupancySensor_c    
  uint8_t sensorType, thresholdOccupancy, occupancyState;
  uint16_t delayOccupancy;
#endif  
  
  /* does the endpoint exist? (and have a device definition?) */
  pDeviceDef = AF_GetEndPointDevice(pSetAttrValueReq->ep);
  if(!pDeviceDef)
    return gZclUnsupportedAttribute_c;

  /* does the cluster exist on this endpoint? */
  pClusterDef = ZCL_FindCluster(pDeviceDef, pSetAttrValueReq->clusterId);
  if(!pClusterDef)
    return gZclUnsupportedAttribute_c;

  /* does the attribute exist? */
  pAttrDef = ZCL_FindAttr(pClusterDef, pSetAttrValueReq->attrID);
  if(!pAttrDef)
   return gZclUnsupportedAttribute_c;
  
#if gZclEnableBinaryInput_c
 /*used only for Binary input Cluster*/ 
   Set2Bytes(aClusterId, gZclClusterBinaryInput_c);
   if(FLib_Cmp2Bytes(pSetAttrValueReq->clusterId,aClusterId) == TRUE){
      if((pSetAttrValueReq->attrID == gZclAttrBinaryInputStatusFlags_c)||(pSetAttrValueReq->attrID == gZclAttrBinaryInputApplicationType_c))
        (void)ZCL_SetAttribute(pSetAttrValueReq->ep, aClusterId, pSetAttrValueReq->attrID, &(pSetAttrValueReq->valueAttr)); 
      BeeAppUpdateDevice(pSetAttrValueReq->ep, gZclUI_ChangeNotification, 0, aClusterId, NULL);
      return gZbSuccess_c;
  } 
#endif
   
#if gASL_ZclIASZoneReq_d
  Set2Bytes(aClusterId, gZclClusterSecurityZone_c);
  if(FLib_Cmp2Bytes(pSetAttrValueReq->clusterId,aClusterId) == TRUE){
      if(pSetAttrValueReq->attrID == gZclAttrZoneInformationZoneStatusSet_c)
      {
        (void)ZCL_SetAttribute(pSetAttrValueReq->ep, aClusterId, gZclAttrZoneInformationZoneStatusSet_c, &(pSetAttrValueReq->valueAttr));
        BeeAppUpdateDevice(pSetAttrValueReq->ep, gZclUI_ChangeNotification, 0, 0, NULL);
      }
       return gZbSuccess_c;
  }
#endif

#if gZclEnableOccupancySensor_c  
  Set2Bytes(aClusterId, gZclClusterOccupancy_c);
  if(FLib_Cmp2Bytes(pSetAttrValueReq->clusterId,aClusterId) == TRUE){
    if(pSetAttrValueReq->attrID == gZclAttrOccupancySensing_OccupancyId_c){  /* attribute = occupancy */
      (void)ZCL_GetAttribute(pSetAttrValueReq->ep, aClusterId, gZclAttrOccupancySensing_OccupancySensorTypeId_c, &sensorType, NULL);
      (void)ZCL_GetAttribute(pSetAttrValueReq->ep, aClusterId, gZclAttrOccupancySensing_OccupancyId_c, &occupancyState, NULL);
      if(pSetAttrValueReq->valueAttr[0] != occupancyState) //(size for occupancyState attribute = 1 byte)
      {
        gSetAvailableOccupancy = FALSE;
        occupancyTimer = TMR_AllocateTimer();
        if(sensorType == gZclTypeofSensor_PIR_c)
        {        
          (void)ZCL_GetAttribute(pSetAttrValueReq->ep, aClusterId, gZclAttrOccupancySensing_PIRUnoccupiedToOccupiedThresholdId_c, &thresholdOccupancy, NULL);
          if(occupancyState == 1) /*occupied*/
            (void)ZCL_GetAttribute(pSetAttrValueReq->ep, aClusterId, gZclAttrOccupancySensing_PIROccupiedToUnoccupiedDelayId_c, &delayOccupancy, NULL);
          else /*unoccupied*/
            (void)ZCL_GetAttribute(pSetAttrValueReq->ep, aClusterId, gZclAttrOccupancySensing_PIRUnoccupiedToOccupiedDelayId_c, &delayOccupancy, NULL);      
        }
        else
          if(sensorType == gZclTypeofSensor_Ultrasonic_c)
          {
            (void)ZCL_GetAttribute(pSetAttrValueReq->ep, aClusterId, gZclAttrOccupancySensing_UltrasonicUnoccupiedToOccupiedThresholdId_c, &thresholdOccupancy, NULL);
             if(occupancyState == 1) /*occupied*/
              (void)ZCL_GetAttribute(pSetAttrValueReq->ep, aClusterId, gZclAttrOccupancySensing_UltrasonicOccupiedToUnoccupiedDelayId_c, &delayOccupancy, NULL);
            else /*unoccupied*/
              (void)ZCL_GetAttribute(pSetAttrValueReq->ep, aClusterId, gZclAttrOccupancySensing_UltrasonicUnoccupiedToOccupiedDelayId_c, &delayOccupancy, NULL);      
        }
	delayOccupancy = OTA2Native16(delayOccupancy);
        TMR_StartSecondTimer( pSetAttrValueReq->ep, delayOccupancy, TmrOccupancyCallBack);       
      }
       return gZbSuccess_c;
    }
    else
      if(pSetAttrValueReq->attrID == gZclAttrOccupancySensing_OccupancySensorTypeId_c) 
      (void)ZCL_SetAttribute(pSetAttrValueReq->ep, aClusterId, pSetAttrValueReq->attrID, &(pSetAttrValueReq->valueAttr));
     return gZbSuccess_c;
  }
#endif
  
#if gZclEnableThermostat_c || gASL_ZclMet_Optionals_d   
#if gZclEnableThermostat_c  
  Set2Bytes(aClusterId, gZclClusterThermostat_c);
#else
  Set2Bytes(aClusterId, gZclClusterSmplMet_c); 
#endif
  if(FLib_Cmp2Bytes(pSetAttrValueReq->clusterId,aClusterId) == TRUE){
#if gZclEnableThermostat_c      
    if(pSetAttrValueReq->attrID == gZclAttrThermostat_AlarmMaskId_c){
      valueAttr = pSetAttrValueReq->valueAttr[0];
#else
    if(pSetAttrValueReq->attrID == gZclAttrSmplMetASGenericAlarmMask_c){  
      FLib_MemCpy(&valueAttr, &pSetAttrValueReq->valueAttr, pSetAttrValueReq->attrSize);
#endif          
      (void)ZCL_GetAttribute(pSetAttrValueReq->ep, aClusterId, pSetAttrValueReq->attrID, &alarmMask, NULL);   
      //if it is the same value - can't detect the correct code alarm 
      if((valueAttr != 0)&&(valueAttr != alarmMask))
      {
          alarmCode = (uint8_t)(valueAttr & (~alarmMask)); //sizeof(alarm code) = 1Byte
          if(alarmCode != 0x00)
          {
            alarmCode--;
            BeeAppUpdateDevice(pSetAttrValueReq->ep, gZclUI_AlarmGenerate_c, 0, aClusterId, &alarmCode);
          }
      }
      (void)ZCL_SetAttribute(pSetAttrValueReq->ep, aClusterId, pSetAttrValueReq->attrID, &valueAttr);      
      
     }
      return gZbSuccess_c;
  } 
#endif
    
   Copy2Bytes(aClusterId, pSetAttrValueReq->clusterId);
   return ZCL_SetAttribute(pSetAttrValueReq->ep, aClusterId, pSetAttrValueReq->attrID, &(pSetAttrValueReq->valueAttr));    
}

#if gZclEnableOccupancySensor_c  
void TmrOccupancyCallBack(tmrTimerID_t tmrid)
{
  zbClusterId_t   aClusterId = {gaZclClusterOccupancySensor_c};
  uint8_t  occupancyState = 0;
  (void) tmrid;
  gSetAvailableOccupancy = TRUE;
  (void)ZCL_GetAttribute(0x08, aClusterId, gZclAttrOccupancySensing_OccupancyId_c, &occupancyState, NULL);
  occupancyState = (occupancyState == 0x00)?0x01:0x00; 
  (void)ZCL_SetAttribute(0x08, aClusterId, gZclAttrOccupancySensing_OccupancyId_c, &occupancyState);
  TMR_FreeTimer(occupancyTimer);
}
#endif //gZclEnableOccupancySensor_c
#endif //gZclClusterOptionals_d

/*********************************************************** 
*   InterpretMatchDescriptor 
*      this fucntion return TRUE only if the MatchDescriptor Req
*      was generated by a OTA Cluster command
************************************************************/
bool_t InterpretMatchDescriptor(zbNwkAddr_t  aDestAddress, uint8_t endPoint)
{
  bool_t status = FALSE;
#if (gZclEnableOTAClient_d)  
  status = InterpretOtaMatchDescriptor(aDestAddress, endPoint);
#endif
  return status;  
}
