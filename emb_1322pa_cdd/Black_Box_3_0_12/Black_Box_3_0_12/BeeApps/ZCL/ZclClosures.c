/******************************************************************************
* ZclClosures.c
*
* This module contains code that handles ZigBee Cluster Library commands for the
* closures
*
* Copyright (c) 2008, Freescale, Inc.  All rights reserved.
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
#include "EmbeddedTypes.h"
#include "zigbee.h"
#include "FunctionLib.h"
#include "BeeStackConfiguration.h"
#include "BeeStack_Globals.h"
#include "AppAfInterface.h"
#include "AfApsInterface.h"
#include "BeeStackInterface.h"

#include "HaProfile.h"
#include "ZclClosures.h"

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

/******************************************************************************
*******************************************************************************
* Private memory declarations
*******************************************************************************
******************************************************************************/

/******************************
  Shade Configuration Cluster 
  See ZCL Specification Section 7.2
*******************************/
/* Shade Configuration Cluster Attribute Definitions */
const zclAttrDef_t gaZclShadeCfgClusterAttrDef[] = {
   /*Attributes of the Shade Information attribute set */
#if gZclClusterOptionals_d
  { gZclAttrShadeCfgInfPhysicalClosedLimit_c, gZclDataTypeUint16_c, gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(uint16_t),(void *) MbrOfs(zclShadeCfgAttrsRAM_t, physicalClosedLimit)},
  { gZclAttrShadeCfgInfMotorStepSize_c,       gZclDataTypeUint8_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(uint8_t),(void *) MbrOfs(zclShadeCfgAttrsRAM_t, motorStepSize)},
#endif
  { gZclAttrShadeCfgInfStatus_c, gZclDataTypeBitmap8_c, gZclAttrFlagsInRAM_c, sizeof(uint8_t),(void *) MbrOfs(zclShadeCfgAttrsRAM_t, status)},
  /*Attributes of the Shade Settings attribute set */
  { gZclAttrShadeCfgStgClosedLimit_c, gZclDataTypeUint16_c, gZclAttrFlagsInRAM_c, sizeof(uint16_t),(void *) MbrOfs(zclShadeCfgAttrsRAM_t, closedLimit)},
  { gZclAttrShadeCfgStgMode_c,        gZclDataTypeEnum8_c,  gZclAttrFlagsInRAM_c, sizeof(uint8_t), (void *) MbrOfs(zclShadeCfgAttrsRAM_t, mode)}
};

const zclAttrDefList_t gZclShadeCfgClusterAttrDefList = {
  NumberOfElements(gaZclShadeCfgClusterAttrDef),
  gaZclShadeCfgClusterAttrDef
};


/******************************
  Door Lock Cluster 
  See ZCL Specification Section 7.3
*******************************/
/* Door Lock Cluster Attribute Definitions */
const zclAttrDef_t gaZclDoorLockClusterAttrDef[] = {
  { gZclAttrDoorLockLockState_c,  gZclDataTypeEnum8_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsReportable_c | gZclAttrFlagsRdOnly_c, sizeof(uint8_t),(void *) MbrOfs(zclDoorLockAttrsRAM_t, lockState)},
  { gZclAttrDoorLockLockType_c,   gZclDataTypeEnum8_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint8_t),(void *) MbrOfs(zclDoorLockAttrsRAM_t, lockType)},
  { gZclAttrDoorLockActuatorEnable_c, gZclDataTypeBool_c,  gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint8_t), (void *) MbrOfs(zclDoorLockAttrsRAM_t, actuatorEnabled)}
#if gZclClusterOptionals_d
  , { gZclAttrDoorLockDoorState_c,   gZclDataTypeEnum8_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c | gZclAttrFlagsReportable_c, sizeof(uint8_t),(void *) MbrOfs(zclDoorLockAttrsRAM_t, doorState)},
  { gZclAttrDoorLockDoorOpenEvents_c,  gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c , sizeof( uint32_t),(void *) MbrOfs(zclDoorLockAttrsRAM_t, doorOpenEvents)},
  { gZclAttrDoorLockDoorClosedEvents_c,   gZclDataTypeUint32_c, gZclAttrFlagsInRAM_c , sizeof(uint32_t),(void *) MbrOfs(zclDoorLockAttrsRAM_t, doorClosedEvents)},
  { gZclAttrDoorLockDoorOpenPeriod_c,   gZclDataTypeUint16_c, gZclAttrFlagsInRAM_c , sizeof(uint16_t),(void *) MbrOfs(zclDoorLockAttrsRAM_t, doorOpenPeriod)}
#endif
};

const zclAttrDefList_t gZclDoorLockClusterAttrDefList = {
  NumberOfElements(gaZclDoorLockClusterAttrDef),
  gaZclDoorLockClusterAttrDef
};


/******************************
  Shade Configuration Cluster 
  See ZCL Specification Section 7.2
*******************************/
/* The comand use for this cluster are the read/write attributes */
zbStatus_t ZCL_ShadeCfgClusterServer
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

zbStatus_t ZCL_ShadeCfgClusterClient
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
  Door Lock Cluster
  See ZCL Specification Section 3.8
*******************************/

/*****************************************************************************
* ZCL_DoorLockClusterServer
*
* Handle Door Lock/Unlock 
*
* Returns Succes if worked.
*****************************************************************************/
zbStatus_t ZCL_DoorLockClusterServer
  (
  zbApsdeDataIndication_t *pIndication, /* IN: MUST be set to the indication */
  afDeviceDef_t *pDevice    /* IN: MUST be set to the endpoint's device data */
  )
{
  uint8_t lockUnlockAttr;
#if gZclClusterOptionals_d 
  uint8_t doorStateAttr;
  uint32_t doorEvents = 0;
#endif  
  uint8_t event;
  uint8_t status;
  zclFrame_t *pFrame;
  zbEndPoint_t ep;
  zbClusterId_t aClusterId;
 
  status = gZclFailure_c; 
  /* not used in this function */
  (void)pDevice;
  
  /* get the attribute */
  ep = pIndication->dstEndPoint;
  Copy2Bytes(aClusterId, pIndication->aClusterId);
  (void)ZCL_GetAttribute(ep, aClusterId, gZclAttrDoorLockLockState_c, &lockUnlockAttr, NULL);

  /* determine what to do based on the event */
  event = gZclUI_NoEvent_c;
  pFrame = (void *)(pIndication->pAsdu);

  switch (pFrame->command) {
    case gZclCmdDoorLock_Lock_c:
        if(lockUnlockAttr != gDoorLockStateLocked)
        {
              event = gZclUI_Lock_c;
              
              lockUnlockAttr = gDoorLockStateLocked;  /* lock */
#if gZclClusterOptionals_d
              doorStateAttr = gDoorStateClosed; /* door closed */
#endif        
              status = gZclSuccess_c;
              /* sets the attribute and will report if needed */
              (void)ZCL_SetAttribute(ep, aClusterId, gZclAttrDoorLockLockState_c, &lockUnlockAttr);
#if gZclClusterOptionals_d    
              (void)ZCL_GetAttribute(ep, aClusterId, gZclAttrDoorLockDoorClosedEvents_c , &doorEvents,NULL);
              doorEvents = OTA2Native32(doorEvents);
              doorEvents++;
              doorEvents = Native2OTA32(doorEvents);
              (void)ZCL_SetAttribute(ep, aClusterId, gZclAttrDoorLockDoorState_c, &doorStateAttr);
              (void)ZCL_SetAttribute(ep, aClusterId, gZclAttrDoorLockDoorClosedEvents_c , &doorEvents);
#endif 
              /* send the event to the app */
              BeeAppUpdateDevice(ep, event, 0, 0, NULL);
        }
        else
              status = gZclFailure_c; 
        /* respond back to caller */
        pFrame->command = gZclCmdDoorLock_LockRsp_c;
        return  ZCL_Reply(pIndication, sizeof(uint8_t), &status);
        
    case gZclCmdDoorLock_Unlock_c:
      if(lockUnlockAttr != gDoorLockStateUnlocked)
        {
              event = gZclUI_Unlock_c;
              lockUnlockAttr = gDoorLockStateUnlocked;  /* unlock */
#if gZclClusterOptionals_d
              doorStateAttr = gDoorStateOpen; /* door open */
#endif 
              status = gZclSuccess_c;
              /* sets the attribute and will report if needed */
              (void)ZCL_SetAttribute(ep, aClusterId, gZclAttrDoorLockLockState_c, &lockUnlockAttr);
#if gZclClusterOptionals_d 
              (void)ZCL_GetAttribute(ep, aClusterId, gZclAttrDoorLockDoorOpenEvents_c , &doorEvents, NULL);
              doorEvents = OTA2Native32(doorEvents);
              doorEvents++;
              doorEvents = Native2OTA32(doorEvents);
              (void)ZCL_SetAttribute(ep, aClusterId, gZclAttrDoorLockDoorState_c, &doorStateAttr);
              (void)ZCL_SetAttribute(ep, aClusterId, gZclAttrDoorLockDoorOpenEvents_c , &doorEvents);
#endif
              /* send the event to the app */
              BeeAppUpdateDevice(ep, event, 0, 0, NULL); 
        }
      else
              status = gZclFailure_c;
         /* respond back to caller */
        pFrame->command = gZclCmdDoorLock_UnlockRsp_c;
        return  ZCL_Reply(pIndication, sizeof(uint8_t), &status); 
        
    default:
      return  gZclUnsupportedClusterCommand_c;                     
  }
}


/*****************************************************************************
* ZCL_DoorLockClusterClient
*
* 
* Returns Succes if worked.
*****************************************************************************/
zbStatus_t ZCL_DoorLockClusterClient
  (
  zbApsdeDataIndication_t *pIndication, 
  afDeviceDef_t *pDevice
  )
{
  zclFrame_t *pFrame;
  zclCmdDoorLockRsp_t Command;
  uint8_t event;
 
  /* prevent compiler warning */
  (void)pDevice;
  pFrame = (void *)pIndication->pAsdu;
  FLib_MemCpy(&Command ,(pFrame + 1), sizeof(zclCmdDoorLockRsp_t));
  /* handle the command */
  switch(pFrame->command){
     case gZclCmdDoorLock_LockRsp_c:
          if(Command.statusCmd == gZclSuccess_c)
            event = gZclUI_Lock_c;
          else 
            event = gZclUI_NoEvent_c;
          BeeAppUpdateDevice(0, event, 0, 0, NULL);
          return gZbSuccess_c;
     case gZclCmdDoorLock_UnlockRsp_c:
          if(Command.statusCmd == gZclSuccess_c)
             event = gZclUI_Unlock_c;
          else 
             event = gZclUI_NoEvent_c;
          BeeAppUpdateDevice(0, event, 0, 0, NULL);
          return gZbSuccess_c;
     default:
         return  gZclUnsupportedClusterCommand_c;    
  }
}

