/******************************************************************************
* ZclClosures.c
*
* This module contains code that handles ZigBee Cluster Library commands for the
* closures
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
#ifndef _ZCL_CLOSURES_H
#define _ZCL_CLOSURES_H

#include "EmbeddedTypes.h"
#include "zigbee.h"
#include "FunctionLib.h"
#include "BeeStackConfiguration.h"
#include "BeeStack_Globals.h"
#include "AppAfInterface.h"
#include "AfApsInterface.h"
#include "BeeStackInterface.h"



/******************************************************************************
*******************************************************************************
* Private macros
*******************************************************************************
******************************************************************************/

/******************************************
	Shade Configuration Cluster
*******************************************/

/* Shade Configuration Configuration Attributes Sets */
#if ( TRUE == gBigEndian_c )
#define gZclAttrShadeCfgShadeInformationSet_c       0x0000    /* Shade Information Set */
#define gZclAttrShadeCfgShadeSettingsSet_c          0x0100    /* Shade Settings Set */
                                                              /*0x002-0xFFF - reserved*/
#else
#define gZclAttrShadeCfgShadeInformationSet_c       0x0000    /* Shade Information Set */
#define gZclAttrShadeCfgShadeSettingsSet_c          0x0001    /* Shade Settings Set */
                                                              /*0x002-0xFFF - reserved*/
#endif /* #if ( TRUE == gBigEndian_c ) */

#if ( TRUE == gBigEndian_c )
/* 7.2.2.2.1 Shade information attributes */
#define gZclAttrShadeCfgInfPhysicalClosedLimit_c    0x0000    /* O - PhysicalClosedLimit */
#define gZclAttrShadeCfgInfMotorStepSize_c          0x0100    /* O - MotorStepSize*/
#define gZclAttrShadeCfgInfStatus_c                 0x0200    /* M - Status*/
/* 7.2.2.2.2 Shade settings attributes */
#define gZclAttrShadeCfgStgClosedLimit_c            0x1000    /* M - ClosedLimit */
#define gZclAttrShadeCfgStgMode_c                   0x1100    /* M - Mode */
#else
/* 7.2.2.2.1 Shade information attributes */
#define gZclAttrShadeCfgInfPhysicalClosedLimit_c    0x0000    /* O - PhysicalClosedLimit */
#define gZclAttrShadeCfgInfMotorStepSize_c          0x0001    /* O - MotorStepSize*/
#define gZclAttrShadeCfgInfStatus_c                 0x0002    /* M - Status*/
/* 7.2.2.2.2 Shade settings attributes */
#define gZclAttrShadeCfgStgClosedLimit_c            0x0010    /* M - ClosedLimit */
#define  gZclAttrShadeCfgStgMode_c                  0x0011    /* M - Mode */
#endif 

/* Shade Configuration Status */
typedef struct gZclShadeStatus_tag
{
    unsigned ShadeOperational   :1;    /* Shade Operational (0- No, 1-Yes) - Read only */                             
    unsigned ShadeAdjusting     :1;    /* Shade Adjusting (0- No, 1-Yes) - Read only */    
    unsigned ShadeDirection     :1;    /* Shade Direction (0- closing, 1-opening) - Read only */ 
    unsigned ShadeForwardDir    :1;    /* Shade Forward Direction of motor (0- closing, 1-opening) - Read/Write */ 
    unsigned Reserved           :4;    /* Reserved */                                    
}gZclShadeStatus_t; 

/* Shade configuration Mode */
enum
{
   gModeNormal = 0x00,       /* Shade - Normal Mode */
   gModeConfigure            /* Shade - Configure Mode */  
                             /* 0x02 - 0xfe - reserved*/ 
};

/* The comand use for this cluster are the read/write attributes */
#ifdef __IAR_SYSTEMS_ICC__
#pragma pack(1)
#endif
typedef struct zclShadeCfgAttrsRAM_tag
{
#if gZclClusterOptionals_d  
  uint16_t    physicalClosedLimit; 
  uint8_t     motorStepSize;
#endif
  uint8_t     status;
  uint16_t    closedLimit;
  uint8_t     mode;
} zclShadeCfgAttrsRAM_t;


/******************************************
	Door Lock Cluster
*******************************************/
/* Door Lock Attributes Sets */

#if ( TRUE == gBigEndian_c )
#define gZclAttrDoorLockLockState_c           0x0000    /* M - LockState */
#define gZclAttrDoorLockLockType_c            0x0100    /* M - LockType */
#define gZclAttrDoorLockActuatorEnable_c      0x0200    /* M - ActuatorEnable */
#define gZclAttrDoorLockDoorState_c           0x0300    /* O - DoorState */
#define gZclAttrDoorLockDoorOpenEvents_c      0x0400    /* O - DoorOpenEvents */
#define gZclAttrDoorLockDoorClosedEvents_c    0x0500    /* O - DoorClosedEvents */
#define gZclAttrDoorLockDoorOpenPeriod_c      0x0600    /* O - OpenPeriod */
#else
#define gZclAttrDoorLockLockState_c           0x0000    /* M - LockState */
#define gZclAttrDoorLockLockType_c            0x0001    /* M - LockType */
#define gZclAttrDoorLockActuatorEnable_c      0x0002    /* M - ActuatorEnable */
#define gZclAttrDoorLockDoorState_c           0x0003    /* O - DoorState */
#define gZclAttrDoorLockDoorOpenEvents_c     0x0004    /* O - DoorOpenEvents */
#define gZclAttrDoorLockDoorClosedEvents_c    0x0005    /* O - DoorClosedEvents */
#define gZclAttrDoorLockDoorOpenPeriod_c      0x0006    /* O - OpenPeriod */
#endif 

/* Door LockState */
enum
{
   gDoorLockStateNotFullyLocked = 0x00,    /* LockState - Not Fully Locked */
   gDoorLockStateLocked,                   /* LockState - Locked */  
   gDoorLockStateUnlocked,                 /* LockState - Unlocked */ 
                                           /* 0x03 - 0xfe - reserved, 0xff - not defined */ 
};

/* Door LockType */
enum
{
   gDoorLockTypeDeadbolt = 0x00,          /* LockType - Deadbolt */
   gDoorLockTypeMagnetic,                 /* LockType - Magnetic */  
   gDoorLockTypeOther,                    /* LockType - Other */ 
                                          /* 0x03 - 0xfe - reserved, 0xff - not defined */ 
};

/* Door  State */
enum
{
   gDoorStateOpen = 0x00,                 /* DoorState - Open */
   gDoorStateClosed,                      /* DoorState - Closed */
   gDoorStateErrorJammed,                 /* DoorState - Error : jammed */
   gDoorStateErrorForcedOpen,             /* DoorState - Error : forced open */
   gDoorStateErrorUnspecified,            /* DoorState - Error : unspecified */
                                          /* 0x05 - 0xfe - reserved, 0xff - not defined */
};

#ifdef __IAR_SYSTEMS_ICC__
#pragma pack(1)
#endif
typedef struct zclDoorLockAttrsRAM_tag
{
  uint8_t    lockState[zclReportableCopies_c]; 
  uint8_t    lockType;
  uint8_t    actuatorEnabled;
#if gZclClusterOptionals_d  
  uint8_t    doorState[zclReportableCopies_c];
  uint32_t   doorOpenEvents;
  uint32_t   doorClosedEvents;
  uint32_t   doorOpenPeriod;  
#endif  
} zclDoorLockAttrsRAM_t;

/* Door Lock cluster commands */
typedef struct zclCmdDoorLockSetState_tag
{
  zclCmd_t command;                /* door lock commands */
} zclCmdDoorLockSetState_t;

typedef zclCmd_t zclDoorLockCmd_t;
/* 7.3.2.3 Commands Received  */
#define gZclCmdDoorLock_Lock_c         0x00    /* M - lock door */
#define gZclCmdDoorLock_Unlock_c       0x01    /* M - unlock door */
/* 7.3.2.4 Commands Generated  */
#define gZclCmdDoorLock_LockRsp_c      0x00    /* M - lock door response */
#define gZclCmdDoorLock_UnlockRsp_c    0x01    /* M - unlock door response */

/* note: lock/unlock commands have no payload */
typedef struct zclDoorLockReq_tag
{
  afAddrInfo_t    addrInfo;     /* IN: source/dst address, cluster, etc... */
  zclOnOffCmd_t   command;      /* IN: lock, unlock */
} zclDoorLockReq_t;

/* sends lock/unlock command */
typedef struct zclDoorLockSetState_tag
{
  afAddrInfo_t                addrInfo;  /* IN: */
  zclCmdDoorLockSetState_t    cmdFrame;  /* IN: handles all 2 commands, lock, unlock */
} zclDoorLockSetState_t;

typedef struct zclCmdDoorLockRsp_tag
{
  uint8_t                 statusCmd;
} zclCmdDoorLockRsp_t;


/* cluster servers */
zbStatus_t ZCL_ShadeCfgClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice);
zbStatus_t ZCL_DoorLockClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice);

/* cluster client */
zbStatus_t ZCL_ShadeCfgClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice);
zbStatus_t ZCL_DoorLockClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice);

#endif