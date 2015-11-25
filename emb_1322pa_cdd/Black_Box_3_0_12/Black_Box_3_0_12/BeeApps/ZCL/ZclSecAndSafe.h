/******************************************************************************
* ZclSecAndSafe.h
*
* This module contains code that handles ZigBee Cluster Library commands and
* clusters for the Security and safety clusters.
*
* Copyright (c) 2009, Freescale, Inc.  All rights reserved.
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

#ifndef _ZCL_SECANDSAFE_H
#define _ZCL_SECANDSAFE_H

#include "ZclOptions.h"
#include "AfApsInterface.h"
#include "AppAfInterface.h"
#include "BeeStackInterface.h"
#include "EmbeddedTypes.h"
#include "ZCL.h"
#include "ZCLGeneral.h"

/******************************************************************************
*******************************************************************************
* Public macros and data types definitions.
*******************************************************************************
******************************************************************************/


#if ( TRUE == gBigEndian_c )
#define gZclClusterIASZone_c      0x0005    
#define gZclClusterIASACE_c       0x0105  
#define gZclClusterIASWD_c        0x0205  
#else /* #if ( TRUE == gBigEndian_c ) */
#define gZclClusterIASZone_c      0x0500  
#define gZclClusterIASACE_c       0x0501  
#define gZclClusterIASWD_c        0x0502 
#endif



/********************************************************
	IAS Zone Cluster (ZigBee Cluster Library Chapter 8.2)
********************************************************/

#if ( TRUE == gBigEndian_c )
#define gZclAttrIASZoneZoneInformationSet_c   0x0000    /* Zone Information attributes Set */
#define gZclAttrIASZoneZoneSettingsSet_c      0x0100    /* Zone Settings attributes Set */
#else
#define gZclAttrIASZoneZoneInformationSet_c   0x0000    /* Zone Information attributes Set */
#define gZclAttrIASZoneZoneSettingsSet_c      0x0001    /* Zone Settings attributes Set */
#endif /* #if ( TRUE == gBigEndian_c ) */

/* 8.2.2.2.1 Zone Information Attributes Set */
#if ( TRUE == gBigEndian_c )
#define gZclAttrZoneInformationZoneStateSet_c     0x0000    /* M - Zone State attributes */
#define gZclAttrZoneInformationZoneTypeSet_c      0x0100    /* M - Zone Type attributes */
#define gZclAttrZoneInformationZoneStatusSet_c    0x0200    /* M - Zone Status attributes */
#else
#define gZclAttrZoneInformationZoneStateSet_c     0x0000    /* M - Zone State attributes */
#define gZclAttrZoneInformationZoneTypeSet_c      0x0001    /* M - Zone Type attributes */
#define gZclAttrZoneInformationZoneStatusSet_c    0x0002    /* M - Zone Status attributes */
#endif /* #if ( TRUE == gBigEndian_c ) */


/* 8.2.2.2.1.1 Zone State Attributes */
enum
{
   gZclZoneState_NotEnrolled_c = 0x00,       /* Not Enrolled */
   gZclZoneState_Enrolled_c,                 /* Enrolled */
                                             /* 0x02-0xFF Reserved */
};

/* 8.2.2.2.1.2 Zone Type Attributes */

#if ( TRUE == gBigEndian_c )
#define gZclZoneType_StandardCIE_c               0x0000   /* Standard CIE */
#define gZclZoneType_MotionSensor_c              0x0D00   /* Motion Sensor */
#define gZclZoneType_ContactSwitch_c             0x1500   /* Contact Switch */
#define gZclZoneType_FireSensor_c                0x2800   /* Fire Sensor */
#define gZclZoneType_WaterSensor_c               0x2A00   /* Water Sensor */
#define gZclZoneType_GasSensor_c                 0x2B00   /* Gas Sensor */
#define gZclZoneType_PersonalEmergencyDevice_c   0x2C00   /* Personal Emergency Device */
#define gZclZoneType_VibrationMovement_c         0x2D00   /* Vibration/Movement Sensor */
#define gZclZoneType_RemoteControl_c             0x0F01   /* Remote Control */
#define gZclZoneType_KeyFob_c                    0x1501   /* Key fob */
#define gZclZoneType_Keypad_c                    0x1D02   /* Keypad */
#define gZclZoneType_StandardWarning_c           0x2502   /* Standard Warning Device */
#define gZclZoneType_InvalidZone_c               0xFFFF   /* Invalid Zone Type */
                                                          /* Other Values < 0x7FFF are Reserved */
                                                          /* 0x8000-0xFFFE Reserved for manufacturer 
                                                              specific types */
#else
#define gZclZoneType_StandardCIE_c               0x0000   /* Standard CIE */
#define gZclZoneType_MotionSensor_c              0x000D   /* Motion Sensor */
#define gZclZoneType_ContactSwitch_c             0x0015   /* Contact Switch */
#define gZclZoneType_FireSensor_c                0x0028   /* Fire Sensor */
#define gZclZoneType_WaterSensor_c               0x002A   /* Water Sensor */
#define gZclZoneType_GasSensor_c                 0x002B   /* Gas Sensor */
#define gZclZoneType_PersonalEmergencyDevice_c   0x002C   /* Personal Emergency Device */
#define gZclZoneType_VibrationMovement_c         0x002D   /* Vibration/Movement Sensor */
#define gZclZoneType_RemoteControl_c             0x010F   /* Remote Control */
#define gZclZoneType_KeyFob_c                    0x0115   /* Key fob */
#define gZclZoneType_Keypad_c                    0x021D   /* Keypad */
#define gZclZoneType_StandardWarning_c           0x0225   /* Standard Warning Device */
#define gZclZoneType_InvalidZone_c               0xFFFF   /* Invalid Zone Type */
                                                          /* Other Values < 0x7FFF are Reserved */
                                                          /* 0x8000-0xFFFE Reserved for manufacturer 
                                                              specific types */
#endif /* #if ( TRUE == gBigEndian_c ) */


/* 8.2.2.2.2 Zone Settings Attributes Set */
#if ( TRUE == gBigEndian_c )
#define gZclAttrZoneSettingsIASCIEAddress_c     0x1000    /* M - IAS CIE Address attributes */
#else
#define gZclAttrZoneSettingsIASCIEAddress_c     0x0010    /* M - IAS CIE Address attributes */
#endif /* #if ( TRUE == gBigEndian_c ) */   }


/* 8.2.2.2.1.3 Zone Status Attributes */

#ifdef __IAR_SYSTEMS_ICC__
#pragma pack(1)
#endif

typedef struct gZclZoneStatus_tag
{
    unsigned Alarm1             :1;                                 
    unsigned Alarm2             :1;                                 
    unsigned Tamper             :1;                                 
    unsigned Battery            :1;                                
    unsigned SupervisionReports :1;                     
    unsigned RestoreReports     :1;                         
    unsigned Trouble            :1;                                
    unsigned ACmains            :1;
    uint8_t Reserved;                                       
}gZclZoneStatus_t;                   
  
#define ZONESTATUS_ALARMED         1
#define ZONESTATUS_NOT_ALARMED     0
#define ZONESTATUS_TAMPERED        1
#define ZONESTATUS_NOT_TAMPERED    0
#define ZONESTATUS_LOW_BATTERY     1
#define ZONESTATUS_BATTERY_OK      0
#define ZONESTATUS_REPORTS         1
#define ZONESTATUS_NOT_REPORT      0
#define ZONESTATUS_FAILURE         1
#define ZONESTATUS_OK              0
                                         


typedef uint16_t IAS_CIE_Address_t;

/* 8.2.2.3 IAS Zone server cluster Commands Received */

#define gZclCmdRxIASZone_ZoneEnrollResponse_c     0x00    /* M - Zone Enroll Response */

/* payload format for Zone EnrollResponse Command */
typedef struct zclCmdIASZone_ZoneEnrollResponse_tag 
{
 uint8_t  EnrollResponseCode;
 uint8_t  ZoneID;
}zclCmdIASZone_ZoneEnrollResponse_t; 

typedef struct zclIASZone_ZoneEnrollResponse_tag 
{
   afAddrInfo_t addrInfo;
   uint8_t zclTransactionId;
   zclCmdIASZone_ZoneEnrollResponse_t cmdFrame;
}zclIASZone_ZoneEnrollResponse_t; 

/* Enroll Response Code Enumeration*/
enum
{
   gEnrollResponseCode_Succes_c = 0x00,   /* Success */
   gEnrollResponseCode_NotSupported_c,    /* Not Supported */
   gEnrollResponseCode_NoEnrollPermit_c,  /* No Enroll Permit */
   gEnrollResponseCode_TooManyZones_c,    /* Too Many Zones */
                                          /* 0x04-0xFE Reserved */
};

/* 8.2.2.4 IAS Zone server cluster Commands Generated */

#define gZclCmdTxIASZone_ZoneStatusChange_c     0x00    /* M - Zone Status Change Notification */
#define gZclCmdTxIASZone_ZoneEnrollRequest_c    0x01    /* M - Zone Enroll Request */

/* payload format for Zone Status Change Notification Command */
typedef struct zclCmdIASZone_ZoneStatusChange_tag 
{
 uint16_t   ZoneStatus;
 uint8_t    ExtendedStatus;
}zclCmdIASZone_ZoneStatusChange_t; 

typedef struct zclIASZone_ZoneStatusChange_tag 
{
   afAddrInfo_t addrInfo;
   uint8_t zclTransactionId;
   zclCmdIASZone_ZoneStatusChange_t cmdFrame;
}zclIASZone_ZoneStatusChange_t;
/* for IASCIE Device - client - */
extern zclCmdIASZone_ZoneStatusChange_t gChageNotif;

/* payload format for Zone Enroll Request Command */
typedef struct zclCmdIASZone_ZoneEnrollRequest_tag
{
 uint16_t   ZoneType;
 uint16_t   ManufacturerCode;
}zclCmdIASZone_ZoneEnrollRequest_t;

typedef struct zclIASZone_ZoneEnrollRequest_tag 
{
   afAddrInfo_t addrInfo;
   uint8_t zclTransactionId;
   zclCmdIASZone_ZoneEnrollRequest_t cmdFrame;
}zclIASZone_ZoneEnrollRequest_t;

#ifdef __IAR_SYSTEMS_ICC__
#pragma pack(1)
#endif
typedef struct zclIASZoneAttrsRAM_tag
{
  uint8_t         zoneState; 
  uint16_t        zoneType;
  uint16_t        zoneStatus;
  IEEEaddress_t   IASCIEAddress;
} zclIASZoneAttrsRAM_t;

typedef struct zclIASSetAttrValue_tag
{
  uint8_t      ep;     /* endpoint*/
  uint16_t     valueAttr;
  uint16_t     attrID;
} zclIASSetAttrValue_t;

#if ( TRUE == gBigEndian_c )
#define Mask_IASZone_Alarm1               0x0100
#define Mask_IASZone_Alarm2               0x0200
#define Mask_IASZone_SupervisionReports   0x1000
#else
#define Mask_IASZone_Alarm1               0x0001
#define Mask_IASZone_Alarm2               0x0002
#define Mask_IASZone_SupervisionReports   0x0010
#endif


/******************************************************
	IAS ACE Cluster (ZigBee Cluster Library Chapter 8.3)
******************************************************/

/* 8.3.2.3 Zone Table */
typedef struct gZclZoneTable_tag
{            
    uint8_t          ZoneID; 
    uint16_t         ZoneType;
    IEEEaddress_t    ZoneAddress;                                      
}gZclZoneTable_t;

#define MaxZones 0x0A
extern uint8_t gIndexTable;/* store the number of devices that are in the gtableZone */
extern gZclZoneTable_t gTableZone[MaxZones];

/* 8.3.2.4 IAS ACE server cluster Commands Received */

#define gZclCmdRxIASACE_Arm_c                 0x00      /* M - Arm */
#define gZclCmdRxIASACE_Bypass_c              0x01      /* M - Bypass */
#define gZclCmdRxIASACE_Emergency_c           0x02      /* M - Emergency */
#define gZclCmdRxIASACE_Fire_c                0x03      /* M - Fire */
#define gZclCmdRxIASACE_Panic_c               0x04      /* M - Panic */
#define gZclCmdRxIASACE_GetZoneIDMap_c        0x05      /* M - Get Zone ID Map */
#define gZclCmdRxIASACE_GetZoneInformation_c  0x06      /* M - Get Zone Information */
                                                        /*0x07 -0xFF -reserved */
/* arm command payload */
typedef struct zclCmdIASACE_Arm_tag 
{
 uint8_t  ArmMode;
}zclCmdIASACE_Arm_t;

typedef struct zclIASACE_Arm_tag 
{
   afAddrInfo_t        addrInfo;
   uint8_t             zclTransactionId;
   zclCmdIASACE_Arm_t  cmdFrame;
}zclIASACE_Arm_t;

/* Arm Mode Field Enumeration*/
enum
{
   gArmMode_Disarm_c = 0x00,      /* Disarm */
   gArmMode_ArmDayHomeZone_c,     /* Arm Day/Home Zones Only */
   gArmMode_ArmNightSleepZone_c,  /* Arm Night/Sleep Zones Only */
   gArmMode_ArmAllZones_c,        /* Arm All Zones */
                                  /* 0x08-0xFF Reserved */
};

/* bypass command payload */
typedef struct zclCmdIASACE_Bypass_tag 
{
 uint8_t  NumberOfZones; 
 uint8_t  pZoneId[1];
}zclCmdIASACE_Bypass_t;

typedef struct zclIASACE_Bypass_tag 
{
   afAddrInfo_t           addrInfo;
   uint8_t                zclTransactionId;
   zclCmdIASACE_Bypass_t  cmdFrame;
}zclIASACE_Bypass_t;

/* Emergency, Fire, Panic, Get Zone ID MAp Command */
typedef struct zclIASACE_EFP_tag 
{
   afAddrInfo_t           addrInfo;
   uint8_t                zclTransactionId;
}zclIASACE_EFP_t;

/* payload for Get Zone Information Command*/
typedef struct zclCmdIASACE_GetZoneInformation_tag 
{
 uint8_t  ZoneID; 
}zclCmdIASACE_GetZoneInformation_t;

typedef struct zclIASACE_GetZoneInformation_tag 
{
   afAddrInfo_t                       addrInfo;
   uint8_t                            zclTransactionId;
   zclCmdIASACE_GetZoneInformation_t  cmdFrame;
}zclIASACE_GetZoneInformation_t;

/* 8.3.2.5 IAS ACE server cluster Commands Generated */

#define gZclCmdTxIASACE_ArmRsp_c              0x00    /* M - Arm Response */
#define gZclCmdTxIASACE_GetZoneIDMApRsp_c     0x01    /* M - Get Zone ID Map Response */
#define gZclCmdTxIASACE_GetZoneInfRsp_c       0x02    /* M - Get Zone Information Response */
                                                      /* 0x03 - 0xFF - reserved */
/* payload for Arm Response command */
typedef struct zclCmdIASACE_ArmRsp_tag 
{
 uint8_t    ArmNotification;
}zclCmdIASACE_ArmRsp_t; 

typedef struct zclIASACE_ArmRsp_tag 
{
   afAddrInfo_t           addrInfo;
   uint8_t                zclTransactionId;
   zclCmdIASACE_ArmRsp_t  cmdFrame;
}zclIASACE_ArmRsp_t; 

/* Arm Notification Enumeration*/
enum
{
   gArmNotif_AllZoneDisarm_c = 0x00,      /* All Zones Disarmed */
   gArmNotif_DayHomeZoneArmed_c,          /* Only Day/Home Zones Armed */
   gArmNotif_NightSleepZoneArmed_c,       /* Only Night/Sleep Zones Armed */
   gArmNotif_AllZonesArmed_c,             /* Arm All Zones */
                                          /* 0x04-0xFE Reserved */
};

/*payload for Get Zone ID Map Response Command*/
typedef struct zclCmdIASACE_GetZoneIDMApRsp_tag
{
 uint16_t   ZoneIDMapSection[16];
}zclCmdIASACE_GetZoneIDMApRsp_t;

typedef struct zclIASACE_GetZoneIDMApRsp_tag 
{
   afAddrInfo_t                    addrInfo;
   uint8_t                         zclTransactionId;
   zclCmdIASACE_GetZoneIDMApRsp_t  cmdFrame;
}zclIASACE_GetZoneIDMApRsp_t;

/* payload for Get Zone Information Response Command */
typedef struct zclCmdIASACE_GetZoneInfRsp_tag
{
 uint8_t          ZoneID;
 uint16_t         ZoneType; 
 IEEEaddress_t    IEEEAddress;
}zclCmdIASZone_GetZoneInfRsp_t;

typedef struct zclIASACE_GetZoneInfRsp_tag 
{
   afAddrInfo_t                    addrInfo;
   uint8_t                         zclTransactionId;
   zclCmdIASZone_GetZoneInfRsp_t   cmdFrame;
}zclIASACE_GetZoneInfRsp_t;

/******************************************************
	IAS WD Cluster (ZigBee Cluster Library Chapter 8.4)
******************************************************/
extern uint16_t gDurationWarning;
extern uint16_t gStartWarningActive;

/* 8.4.2.2 IAS WD Attributes */
#if ( TRUE == gBigEndian_c )
#define gZclAttrIASWDMaxDuration_c     0x0000    /* M - Max Duration */
#else
#define gZclAttrIASWDMaxDuration_c     0x0000    /* M - Max Duration */
#endif /* #if ( TRUE == gBigEndian_c ) */


typedef struct zclIASWDAttrsRAM_tag
{
  uint16_t    maxDuration; 
} zclIASWDAttrsRAM_t;

/* 8.4.2.3 IAS WD server cluster Commands Received */
enum
{
   gZclCmdRxIASWD_StartWarning_c = 0x00,    /* M - Start Warning */
   gZclCmdRxIASWD_Squawk_c,                 /* M - Squawk */
                                            /* 0x02-0xFF Reserved */
};


typedef struct WarningModeAndStrobe_tag
{
 unsigned   WarningMode : 4;
 unsigned   Strobe : 2;
 unsigned   Reserved : 2;
}WarningModeAndStrobe_t;


/* Start Warning Payload */
typedef struct zclCmdIASWD_StartWarning_tag
{
 WarningModeAndStrobe_t   WarningModeandStrobe;
 uint16_t                 WarningDuration;
}zclCmdIASWD_StartWarning_t; 

typedef struct zclIASWD_StartWarning_tag 
{
   afAddrInfo_t addrInfo;
   uint8_t zclTransactionId;
   zclCmdIASWD_StartWarning_t cmdFrame;
}zclIASWD_StartWarning_t;

enum
{
  WarningMode_Stop = 0,     /* Stop (no warning) */
  WarningMode_Burglar,      /* Burglar */
  WarningMode_Fire,         /* Fire */
  WarningMode_Emergency,    /* Emergency */ 
                            /* 0x04-0xFF Reserved */  
};

enum
{
  Strobe_NoStrobe = 0,      /* No Strobe */
  Strobe_StrobeParallel,    /* Use Strobe in Parallel to warning */
                            /* 2-3 Reserved */
};   


/* Squawk Payload */
typedef struct zclCmdIASWD_Squawk_tag
{
 unsigned   SquawkMode : 4;
 unsigned   Strobe : 1;
 unsigned   Reserved : 1;
 unsigned   SquawkLevel : 2;
}zclCmdIASWD_Squawk_t;  

extern zclCmdIASWD_Squawk_t gParamSquawk;

typedef struct zclIASWD_Squawk_tag 
{
   afAddrInfo_t addrInfo;
   uint8_t zclTransactionId;
   zclCmdIASWD_Squawk_t cmdFrame;
}zclIASWD_Squawk_t;

enum
{
  SquawkMode_SystemArmed = 0,     /* Notification sound for "System is Armed" */
  SquawkMode_SystemDisarmed,      /* Notification sound for "System is DisArmed" */
                                  /* 0x02-0xFF Reserved */  
};   

#define NO_STROBE           0
#define STROBE_PARALLEL     1    

enum
{
  SquawkLevel_LowLevel = 0,     /* Low Level Sound */
  SquawkLevel_MediumLevel,      /* Medium Level Sound */
  SquawkLevel_HighLevel,        /* High Level Sound */
  SquawkLevel_VeryHighLevel,    /* Very High Level Sound */
};              

#ifdef __IAR_SYSTEMS_ICC__
#pragma pack()
#endif


/******************************************************************************
*******************************************************************************
* Public functions prototypes
*******************************************************************************
******************************************************************************/

zbStatus_t ZCL_IASZoneClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev);
zbStatus_t ZCL_IASZoneClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev);
zbStatus_t ZCL_IASACEClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev);
zbStatus_t ZCL_IASACEClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev);
zbStatus_t ZCL_IASWDClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev);
zbStatus_t ZCL_IASWDClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev);

zbStatus_t IASZone_ZoneStatusChange(zclIASZone_ZoneStatusChange_t *pReq);
void ZCL_SendStatusChangeNotification(zbEndPoint_t DstEndpoint, zbEndPoint_t SrcEndpoint, uint16_t zoneStatusChangeNotif);
zbStatus_t IASZone_ZoneEnrollRequest(zclIASZone_ZoneEnrollRequest_t *pReq);
void ZCL_SendZoneEnrollRequest(zbEndPoint_t DstEndpoint, zbEndPoint_t SrcEndpoint,  zbNwkAddr_t DstAddr);
zbStatus_t IASZone_ZoneEnrollResponse(zclIASZone_ZoneEnrollResponse_t *pReq);

    
zbStatus_t IASACE_Arm(zclIASACE_Arm_t *pReq);
void ZCL_SendArmCommand(zbEndPoint_t DstEndpoint, zbEndPoint_t SrcEndpoint, zbNwkAddr_t DstAddr, uint8_t armMode);
zbStatus_t IASACE_ArmRsp(zclIASACE_ArmRsp_t *pReq);
zbStatus_t IASACE_Bypass(zclIASACE_Bypass_t *pReq);
void ZCL_SendBypassCommand(zbEndPoint_t DstEndpoint, zbEndPoint_t SrcEndpoint, zbNwkAddr_t DstAddr, uint8_t NrOfZones, uint8_t *zoneID); 
void ZCL_SendEFPCommand(zbEndPoint_t DstEndpoint, zbEndPoint_t SrcEndpoint, zbNwkAddr_t DstAddr, uint8_t CommandEFP); 
zbStatus_t IASACE_Emergency(zclIASACE_EFP_t *pReq);
zbStatus_t IASACE_Fire(zclIASACE_EFP_t *pReq);
zbStatus_t IASACE_Panic(zclIASACE_EFP_t *pReq);
zbStatus_t IASACE_GetZoneIDMap(zclIASACE_EFP_t *pReq);
zbStatus_t IASACE_GetZoneIDMapRsp(zclIASACE_GetZoneIDMApRsp_t *pReq);
zbStatus_t IASACE_GetZoneInformation(zclIASACE_GetZoneInformation_t *pReq);
void ZCL_SendGetZoneInformationCommand(zbEndPoint_t DstEndpoint, zbEndPoint_t SrcEndpoint, zbNwkAddr_t DstAddr, uint8_t zoneId); 
zbStatus_t IASACE_GetZoneInformationRsp(zclIASACE_GetZoneInfRsp_t *pReq);

zbStatus_t IASWD_StartWarning(zclIASWD_StartWarning_t *pReq);
void ZCL_SendStartWarning(zbEndPoint_t DstEndpoint, zbEndPoint_t SrcEndpoint, zbNwkAddr_t DstAddr, WarningModeAndStrobe_t warningStrobe, uint16_t  warningDuration);
zbStatus_t IASWD_Squawk(zclIASWD_Squawk_t *pReq);
void ZCL_SendSqawk(zbEndPoint_t DstEndpoint, zbEndPoint_t SrcEndpoint, zbNwkAddr_t DstAddr, zclCmdIASWD_Squawk_t squawk); 


#endif
