/******************************************************************************
* HaProfile.h
*
* Types, definitions and prototypes for the ZigBee Home Automation Profile.
*
* Copyright (c) 2008, Freescale, Inc.  All rights reserved.
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
#ifndef _HAPROFILE_H
#define _HAPROFILE_H

/* header files needed by home automation */
#include "EmbeddedTypes.h"
#include "AfApsInterface.h"
#include "ZclOptions.h"
#include "ZCL.h"
#include "ZCLGeneral.h"
#include "ZclClosures.h"
#include "ZclHVAC.h"
#include "ZCLSensing.h"
#include "ZCLSecAndSafe.h"
#include "SEProfile.h"

/******************************************************************************
*******************************************************************************
* Public macros & type definitions
*******************************************************************************
******************************************************************************/

/* Home Automation Profile - little endian (0x0104) */
#define gHaProfileId_c	0x0401      /* 0x0104, little endian order */
#define gHaProfileIdb_c	0x04,0x01   /* in byte form */


/******************************************
	device definitions 
*******************************************/

typedef uint16_t haDeviceId_t;

/* Generic */
#define gHaOnOffSwitch_c              0x0000  /* On/Off Switch */
#define gHaLevelControlSwitch_c       0x0100  /* Level Control Switch */
#define gHaOnOffOutput_c              0x0200  /* On/Off Output */
#define gHaLevelControllableOutput_c  0x0300  /* Level Controllable Output */
#define gHaSceneSelector_c            0x0400  /* Scene Selector */
#define gHaConfigurationTool_c        0x0500  /* Configuration Tool */
#define gHaGenericRemoteControl_c     0x0600  /* Generic Remote Control */
#define gHaCombinedInterface_c        0x0700  /* Combined Interface */
#define gHaRangeExtender_c            0x0800  /* Range Extender */
#define gHaMainsPowerOutlet_c         0x0900  /* Mains Power Outlet */
#define gHaDoorLock_c                 0x0A00  /* Door Lock */
#define gHaDoorLockController_c       0x0B00  /* Door Lock Controller */

/* Lighting */
#define gHaOnOffLight_c               0x0001  /* On/Off Light */
#define gHaDimmableLight_c            0x0101  /* Dimmable Light */
#define gHaColorDimmableLight_c       0x0201  /* Color Dimmable Light */
#define gHaOnOffLightSwitch_c         0x0301  /* On/Off Light Switch */
#define gHaDimmerSwitch_c             0x0401  /* Dimmer Switch */
#define gHaColorDimmerSwitch_c        0x0501  /* Color Dimmer Switch */
#define gHaLightSensor_c              0x0601  /* Light Sensor */
#define gHaOccupancySensor_c          0x0701  /* Occupancy Sensor */

/* Closures */
#define gHaShade_c                    0x0002  /* Shade */
#define gHaShadeController_c          0x0102  /* Shade Controller */

/* Freescale defined devices */
#define gHaDoorSensor_c               0x1002  /* door sensor */
#define gHaWindowSensor_c             0x1102  /* window sensor */

/* HVAC */
#define gHaHeatingCoolingUnit_c       0x0003  /* Heating/Cooling Unit */
#define gHaThermostat_c               0x0103  /* Thermostat */
#define gHaTemperatureSensor_c        0x0203  /* Temperature Sensor */
#define gHaPump_c                     0x0303  /* Pump */
#define gHaPumpController_c           0x0403  /* Pump Controller */
#define gHaPressureSensor_c           0x0503  /* Pressure Sensor */
#define gHaFlowSensor_c               0x0603  /* Flow Sensor */

/* Intruder Alarm Systems */
#define gHaIasControlIndicatingEquipment_c  0x0004  /* IAS Control and Indicating Equipment */
#define gHaIasAncillaryControlEquipment_c   0x0104  /* IAS Ancillary Control Equipment */
#define gHaIasZone_c                  0x0204  /* IAS Zone */
#define gHaIasWarningDevice_c         0x0304  /* IAS Warning Device */

/* devices in byte form */
#define gHaOnOffLightb_c              0x00,0x01  /* on/off light */
#define gHaDimmableLightb_c           0x01,0x01  /* Dimmable Light */
#define gHaOnOffLightSwitchb_c        0x03,0x01  /* On/Off Light Switch */
#define gHaDimmerSwitchb_c            0x04,0x01  /* Dimmer Switch */

#define gHaRangeExtenderb_c           0x06,0x00  /* Range Extender */

/* HA location types */
#define gHaLocation_None_c              0x00  /* None            */
#define gHaLocation_Atrium_c            0x01  /* Atrium          */
#define gHaLocation_Bar_c               0x02  /* Bar             */
#define gHaLocation_Courtyard_c         0x03  /* Courtyard       */
#define gHaLocation_Bathroom_c          0x04  /* Bathroom        */
#define gHaLocation_Bedroom_c           0x05  /* Bedroom         */
#define gHaLocation_BilliardRoom_c      0x06  /* BilliardRoom    */
#define gHaLocation_UtilityRoom_c       0x07  /* UtilityRoom     */
#define gHaLocation_Cellar_c            0x08  /* Cellar          */
#define gHaLocation_Closet_c            0x09  /* Closet          */
#define gHaLocation_Theater_c           0x0A  /* Theater         */
#define gHaLocation_Office_c            0x0B  /* Office          */
#define gHaLocation_Deck_c              0x0C  /* Deck            */
#define gHaLocation_Den_c               0x0D  /* Den             */
#define gHaLocation_DiningRoom_c        0x0E  /* DiningRoom      */
#define gHaLocation_ElectricalRoom_c    0x0F  /* ElectricalRoom  */
#define gHaLocation_Elevator_c          0x10  /* Elevator        */
#define gHaLocation_Entry_c             0x11  /* Entry           */
#define gHaLocation_FamilyRoom_c        0x12  /* FamilyRoom      */
#define gHaLocation_MainFloor_c         0x13  /* MainFloor       */
#define gHaLocation_Upstairs_c          0x14  /* Upstairs        */
#define gHaLocation_Downstairs_c        0x15  /* Downstairs      */
#define gHaLocation_Basement_c          0x16  /* Basement        */
#define gHaLocation_Gallery_c           0x17  /* Gallery         */
#define gHaLocation_GameRoom_c          0x18  /* GameRoom        */
#define gHaLocation_Garage_c            0x19  /* Garage          */
#define gHaLocation_Gym_c               0x1A  /* Gym             */
#define gHaLocation_Hallway_c           0x1B  /* Hallway         */
#define gHaLocation_House_c             0x1C  /* House           */
#define gHaLocation_Kitchen_c           0x1D  /* Kitchen         */
#define gHaLocation_LaundryRoom_c       0x1E  /* LaundryRoom     */
#define gHaLocation_Library_c           0x1F  /* Library         */
#define gHaLocation_MasterBedroom_c     0x20  /* MasterBedroom   */
#define gHaLocation_MudRoom_c           0x21  /* MudRoom     */
#define gHaLocation_Nursery_c           0x22  /* Nursery     */
#define gHaLocation_Pantry_c            0x23  /* Pantry      */
//#define gHaLocation_Office_c          0x24  /* Office      */
#define gHaLocation_Outside_c           0x25  /* Outside     */
#define gHaLocation_Pool_c              0x26  /* Pool      */
#define gHaLocation_Porch_c             0x27  /* Porch     */
#define gHaLocation_SewingRoom_c        0x28  /* SewingRoom      */
#define gHaLocation_SittingRoom_c       0x29  /* SittingRoom     */
#define gHaLocation_Stairway_c          0x2A  /* Stairway      */
#define gHaLocation_Yard_c              0x2B  /* Yard      */
#define gHaLocation_Attic_c             0x2C  /* Attic     */
#define gHaLocation_HotTub_c            0x2D  /* Hot Tub */
#define gHaLocation_LivingRoom_c        0x2E  /* LivingRoom      */
#define gHaLocation_Sauna_c             0x2F  /* Sauna     */
#define gHaLocation_Shop_c              0x30  /* Shop      */
#define gHaLocation_GuestBedroom_c      0x31  /* GuestBedroom      */
#define gHaLocation_GuestBath_c         0x32  /* GuestBath     */
#define gHaLocation_PowderRoom_c        0x33  /* PowderRoom      */
#define gHaLocation_BackYard_c          0x34  /* BackYard      */
#define gHaLocation_FrontYard_c         0x35  /* FrontYard     */
#define gHaLocation_Patio_c             0x36  /* Patio     */
#define gHaLocation_Driveway_c          0x37  /* Driveway      */
#define gHaLocation_SunRoom_c           0x38  /* SunRoom     */
//#define gHaLocation_LivingRoom_c      0x39  /* LivingRoom - duplicate */
#define gHaLocation_Spa_c               0x3A  /* Spa     */
#define gHaLocation_Whirlpool_c         0x3B  /* Whirlpool     */
#define gHaLocation_Shed_c              0x3C  /* Shed      */
#define gHaLocation_EquipmentStorage_c  0x3D  /* EquipmentStorage      */
#define gHaLocation_HobbyRoom_c         0x3E  /* HobbyRoom     */
#define gHaLocation_Fountain_c          0x3F  /* Fountain      */
#define gHaLocation_Pond_c              0x40  /* Pond      */
#define gHaLocation_ReceptionRoom_c     0x41  /* ReceptionRoom     */
#define gHaLocation_BreakfastRoom_c     0x42  /* BreakfastRoom     */
#define gHaLocation_Nook_c              0x43  /* Nook      */
#define gHaLocation_Garden_c            0x44  /* Garden      */
#define gHaLocation_Balcony_c           0x45  /* Balcony     */
#define gHaLocation_PanicRoom_c         0x46  /* PanicRoom     */
#define gHaLocation_Terrace_c           0x47  /* Terrace     */
#define gHaLocation_Roof_c              0x48  /* Roof      */


/******************************************
	HA Internal Types (implementation specific)
*******************************************/


/******************************
  OnOffLight
*******************************/

/* attributes for on/off light in RAM */
typedef struct haOnOffLightAttrsRAM_tag {
  uint8_t             reportMask[1];  /* allows up to 8 reportable endpoints in this node */
  zclOnOffAttrsRAM_t  onOffAttrs;     /* RAM attributes from the on/off cluster */
} haOnOffLightAttrRAM_t;


/******************************
  OnOffSwitch
*******************************/

typedef struct haOnOffSwitchAttrsRAM_tag {
  uint8_t  reportMask[1];  /* allows up to 8 reportable endpoints in this node  */
  uint16_t identifyTime;
  zclOnOffSwitchCfgAttrsRAM_t SwitchCfg;    /*RAM attributes from the on/off switch configure cluster*/
} haOnOffSwitchAttrRAM_t;

/******************************
  SimpleSensor
*******************************/

/* attributes for simple sensor in RAM */
typedef struct haSimpleSensorAttrsRAM_tag {
  uint8_t                   reportMask[1];        /* allows up to 8 reportable endpoints in this node */
  zclBinaryInputAttrsRAM_t  binaryInputAttrs;     /* RAM attributes from the Binary Input cluster */
} haSimpleSensorAttrRAM_t;


/******************************
  DoorLock
*******************************/

/* attributes for Door Lock in RAM */
typedef struct haDoorLockAttrsRAM_tag {
   uint8_t                 reportMask[1];     /* allows up to 8 reportable endpoints in this node */
   zclDoorLockAttrsRAM_t  doorLockAttrs;     /* RAM attributes from the Door Lock cluster */
} haDoorLockAttrRAM_t;


/******************************
  DimmerSwitch
*******************************/

typedef struct haDimmerSwitchAttrsRAM_tag {
  uint8_t  reportMask[1];  /* allows up to 8 reportable endpoints in this node  */
  uint16_t identifyTime;
  zclOnOffSwitchCfgAttrsRAM_t SwitchCfg;    /*RAM attributes from the on/off switch configure cluster*/
} haDimmerSwitchAttrRAM_t;

/******************************
* DimmableLight
*******************************/

/* dimmable light structure */
typedef struct haDimmableLightAttrsRAM_tag {
  uint8_t  reportMask[1];  /* allows up to 8 reportable attributes in this node  */
  zclOnOffAttrsRAM_t      onOffAttrs;     /* RAM attributes from the on/off cluster */
  zclLevelCtrlAttrsRAM_t  levelCtrlAttrs; /* RAM attributes from the level control cluster */
} haDimmableLightAttrRAM_t;

/******************************
* Shade
*******************************/

/* shade structure */
typedef struct haShadeAttrRAM_tag {
  uint8_t  reportMask[1];  /* allows up to 8 reportable attributes in this node  */
  zclOnOffAttrsRAM_t      onOffAttrs;     /* RAM attributes from the on/off cluster */
  zclLevelCtrlAttrsRAM_t  levelCtrlAttrs; /* RAM attributes from the level control cluster */
  zclShadeCfgAttrsRAM_t   shadeCfgAttrs;  /* RAM attributes from the shade configuration cluster */
} haShadeAttrRAM_t;

/******************************
* ConsumptionAwernessDevice
******************************/

/* consumtion awerness device structure */
typedef struct haConsumptionAwernessDeviceAttrRAM_tag {
  uint8_t  reportMask[1];  /* allows up to 8 reportable endpoints in this node  */
  ZCLSmplMetServerAttrsRAM_t  SmplMet;    /*RAM attributes from the metering cluster*/
} haConsumptionAwernessDeviceAttrRAM_t;

/******************************
* Configuration Tool
*******************************/
/* 
NOTE: The clusters used in this application does not have atttributes for the client side.
typedef struct haConfigurationToolAttrsRAM_tag{

} haOnOffSwitchCfgAttrsRAM_t;
*/

/******************************
* Temperature Sensor
*******************************/
typedef struct haTemperatureSensorAttrRAM_tag {
  uint8_t                             reportMask[1];  /* allows up to 8 reportable endpoints in this node */
  zclTemperatureMeasurementAttrs_t    TempSensor;       /* temp sensor attribute*/
  zclOccupancySensingAttrs_t          OccupancySensor; /*occupancy sensing attribute*/
} haTemperatureSensorAttrRAM_t;

/******************************
* Occupancy Sensing
*******************************/
typedef struct haOccupancySensorAttrRAM_tag {
  uint8_t                             reportMask[1];  /* allows up to 8 reportable endpoints in this node */
  zclOccupancySensingAttrs_t          OccupancySensor;
} haOccupancySensorAttrRAM_t;

/******************************
* IAS Zone
*******************************/
typedef struct haIASZoneAttrRAM_tag {
  uint8_t               reportMask[1];  /* allows up to 8 reportable endpoints in this node */
  zclIASZoneAttrsRAM_t  iasZone;
} haIASZoneAttrRAM_t;

/******************************
* IAS Warning Device
*******************************/
typedef struct haIASWDAttrRAM_tag {
  uint8_t               reportMask[1];  /* allows up to 8 reportable endpoints in this node */
  zclIASWDAttrsRAM_t    iasWD;          /* IAS Warning Device Attribute*/
  zclIASZoneAttrsRAM_t  iasZone;        /* IAS Zone Attribute */
} haIASWDAttrRAM_t;

/******************************
* Thermostat
*******************************/
typedef struct haThermostatAttrRAM_tag {
  uint8_t              reportMask[1];  /* allows up to 8 reportable endpoints in this node  */
  uint16_t             identifyTime;
  zclThermostatAttrsRAM_t  Thermostat;
  zclThermostatUICfgAttrsRAM_t  UIConfigure;
  zclFanControlAttrs_t FanControl;
  zclAlarmsAttrsRAM_t count;
} haThermostatAttrRAM_t;


/******************************************************************************
*******************************************************************************
* Public prototypes
*******************************************************************************
******************************************************************************/
#endif /* _HAPROFILE_H */

