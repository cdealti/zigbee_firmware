/******************************************************************************
* ZclHVAC.c
*
* This module contains code that handles ZigBee Cluster Library commands and
* clusters for HVAC (Heating/Ventilation/Air-Conditioning).
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

#include "ZCLHvac.h"
#include "HaProfile.h"

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
  Pump Configuration and Control Cluster Data
  See ZCL Specification Section 6.2
*******************************/

/******************************
  Thermostat Cluster Data
  See ZCL Specification Section 6.3
*******************************/

haThermostatWeeklyScheduleRAM_t gHaThermostatWeeklyScheduleData;

const zclAttrDef_t gaZclThermostatClusterAttrDef[] = {
   /*Attributes of the Thermostat Information attribute set */
  { gZclAttrThermostat_LocalTemperatureId_c,         gZclDataTypeInt16_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsReportable_c,sizeof(int16_t),(void *) MbrOfs(zclThermostatAttrsRAM_t,LocalTemperature)},
#if gZclClusterOptionals_d
  { gZclAttrThermostat_OutdoorTemperatureId_c,       gZclDataTypeInt16_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,sizeof(int16_t),(void *) MbrOfs(zclThermostatAttrsRAM_t,OutdoorTemperature)},
  { gZclAttrThermostat_OccupancyId_c,                gZclDataTypeBitmap8_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,sizeof(uint8_t),(void *) MbrOfs(zclThermostatAttrsRAM_t,Occupancy)},
  { gZclAttrThermostat_AbsMinHeatSetpointLimitId_c,  gZclDataTypeInt16_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,sizeof(int16_t),(void *) MbrOfs(zclThermostatAttrsRAM_t,AbsMinHeatSetpointLimit)},
  { gZclAttrThermostat_AbsMaxHeatSetpointLimitId_c,  gZclDataTypeInt16_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,sizeof(int16_t),(void *) MbrOfs(zclThermostatAttrsRAM_t,AbsMaxHeatSetpointLimit)},
  { gZclAttrThermostat_AbsMinCoolSetpointLimitId_c,  gZclDataTypeInt16_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,sizeof(int16_t),(void *) MbrOfs(zclThermostatAttrsRAM_t,AbsMinCoolSetpointLimit)},
  { gZclAttrThermostat_AbsMaxCoolSetpointLimitId_c,  gZclDataTypeInt16_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,sizeof(int16_t),(void *) MbrOfs(zclThermostatAttrsRAM_t,AbsMaxCoolSetpointLimit)},
  { gZclAttrThermostat_PICoolingDemandId_c,          gZclDataTypeUint8_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c|gZclAttrFlagsReportable_c,sizeof(uint8_t),(void *) MbrOfs(zclThermostatAttrsRAM_t,PICoolingDemand)},
  { gZclAttrThermostat_PIHeatingDemandId_c,          gZclDataTypeUint8_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c|gZclAttrFlagsReportable_c,sizeof(uint8_t),(void *) MbrOfs(zclThermostatAttrsRAM_t,PIHeatingDemand)},
#endif
    /*Attributes o 1 f the Thermostat settings attribute set */
  {gZclAttrThermostat_OccupiedCoolingSetpointId_c,   gZclDataTypeInt16_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsReportable_c, sizeof(int16_t),(void *) MbrOfs(zclThermostatAttrsRAM_t,OccupiedCoolingSetpoint)},
  {gZclAttrThermostat_OccupiedHeatingSetpointId_c,   gZclDataTypeInt16_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsReportable_c, sizeof(int16_t),(void *) MbrOfs(zclThermostatAttrsRAM_t,OccupiedHeatingSetpoint)},
  {gZclAttrThermostat_MinHeatSetpointLimitId_c,      gZclDataTypeInt16_c,gZclAttrFlagsInRAM_c, sizeof(int16_t),(void *) MbrOfs(zclThermostatAttrsRAM_t,MinHeatSetpointLimit)},
  {gZclAttrThermostat_MaxHeatSetpointLimitId_c,      gZclDataTypeInt16_c,gZclAttrFlagsInRAM_c, sizeof(int16_t),(void *) MbrOfs(zclThermostatAttrsRAM_t,MaxHeatSetpointLimit)},
  {gZclAttrThermostat_MinCoolSetpointLimitId_c,      gZclDataTypeInt16_c,gZclAttrFlagsInRAM_c, sizeof(int16_t),(void *) MbrOfs(zclThermostatAttrsRAM_t,MinCoolSetpointLimit)},
  { gZclAttrThermostat_MaxCoolSetpointLimitId_c,     gZclDataTypeInt16_c,gZclAttrFlagsInRAM_c, sizeof(int16_t),(void *) MbrOfs(zclThermostatAttrsRAM_t,MaxCoolSetpointLimit)},
  { gZclAttrThermostat_MinSetpointDeadBandId_c,      gZclDataTypeInt8_c,gZclAttrFlagsInRAM_c, sizeof(int8_t),(void *) MbrOfs(zclThermostatAttrsRAM_t,MinSetpointDeadBand)},
  { gZclAttrThermostat_ControlSequenceOfOperationId_c,     gZclDataTypeEnum8_c,gZclAttrFlagsInRAM_c,sizeof(uint8_t),(void *) MbrOfs(zclThermostatAttrsRAM_t,ControlSequenceOfOperation)},
  { gZclAttrThermostat_SystemModeId_c,gZclDataTypeEnum8_c, gZclAttrFlagsInRAM_c|gZclAttrFlagsReportable_c,sizeof(uint8_t),(void *) MbrOfs(zclThermostatAttrsRAM_t,SystemMode)}
#if gZclClusterOptionals_d
  ,{gZclAttrThermostat_LocalTemperatureCalibrationId_c,   gZclDataTypeInt8_c,gZclAttrFlagsInRAM_c, sizeof(int8_t),(void *) MbrOfs(zclThermostatAttrsRAM_t,LocalTemperatureCalibration)},
  {gZclAttrThermostat_UnoccupiedCoolingSetpointId_c,      gZclDataTypeInt16_c,gZclAttrFlagsInRAM_c, sizeof(int16_t),(void *) MbrOfs(zclThermostatAttrsRAM_t,UnoccupiedCoolingSetpoint)},
  {gZclAttrThermostat_UnoccupiedHeatingSetpointId_c,      gZclDataTypeInt16_c,gZclAttrFlagsInRAM_c, sizeof(int16_t),(void *) MbrOfs(zclThermostatAttrsRAM_t,UnoccupiedHeatingSetpoint)},
  { gZclAttrThermostat_RemoteSensingId_c,                 gZclDataTypeBitmap8_c,gZclAttrFlagsInRAM_c, sizeof(uint8_t),(void *) MbrOfs(zclThermostatAttrsRAM_t,Remotesensing)},
  { gZclAttrThermostat_AlarmMaskId_c,                     gZclDataTypeBitmap8_c,gZclAttrFlagsInRAM_c, sizeof(uint8_t),(void *) MbrOfs(zclThermostatAttrsRAM_t,AlarmMask)},
  { gZclAttrThermostat_StartOfWeek_c,                     gZclDataTypeEnum8_c,gZclAttrFlagsInRAM_c, sizeof(uint8_t),(void *) MbrOfs(zclThermostatAttrsRAM_t,StartOfWeek)},
  { gZclAttrThermostat_NumberOfWeeklyTransitions_c,       gZclDataTypeUint8_c,gZclAttrFlagsInRAM_c, sizeof(uint8_t),(void *) MbrOfs(zclThermostatAttrsRAM_t,NumberOfWeeklyTransitions)},
#endif
};

const zclAttrDefList_t gZclThermostatClusterAttrDefList = {
  NumberOfElements(gaZclThermostatClusterAttrDef),
  gaZclThermostatClusterAttrDef
};


/******************************
  Fan Control Cluster Data
  See ZCL Specification Section 6.4
*******************************/
zclFanControlAttrs_t gZclFanControlAttrs;

const zclAttrDef_t gaZclFanControlClusterAttrDef[] = {
  { gZclAttrFanControl_FanModeId_c,gZclDataTypeEnum8_c,gZclAttrFlagsInRAM_c,sizeof(uint8_t), (void *) MbrOfs(zclFanControlAttrs_t, FanMode)},
  { gZclAttrFanControl_FanModeSequence_c,gZclDataTypeEnum8_c,gZclAttrFlagsInRAM_c,sizeof(uint8_t), (void *) MbrOfs(zclFanControlAttrs_t, FanModeSequence)}
  
};

const zclAttrDefList_t gZclFanControlClusterAttrDefList = {
  NumberOfElements(gaZclFanControlClusterAttrDef),
  gaZclFanControlClusterAttrDef
};

/******************************
  Dehumidification Cluster Data
  See ZCL Specification Section 6.5
*******************************/

/******************************
  Thermostat User Interface Configuration Cluster Data
  See ZCL Specification Section 6.6
*******************************/
const zclAttrDef_t gaZclThermostatUICfgClusterAttrDef[] = {
  {gZclAttrThermostatUserInterface_TempDisplayModeId_c,gZclDataTypeEnum8_c,gZclAttrFlagsInRAM_c,sizeof(uint8_t),(void*) MbrOfs(zclThermostatUICfgAttrsRAM_t,DisplayMode)},
  {gZclAttrThermostatUserInterface_KeypadLockoutId_c,gZclDataTypeEnum8_c,gZclAttrFlagsInRAM_c,sizeof(uint8_t),(void*) MbrOfs(zclThermostatUICfgAttrsRAM_t,KeyPadLockout)}
};

const zclAttrDefList_t gZclThermostatUICfgClusterAttrDefList = {
  NumberOfElements(gaZclThermostatUICfgClusterAttrDef),
  gaZclThermostatUICfgClusterAttrDef
};

/******************************************************************************
*******************************************************************************
* Public functions
*******************************************************************************
******************************************************************************/

/******************************
  Pump Configuration and Control Cluster
  See ZCL Specification Section 6.2
*******************************/

/******************************
  Thermostat Cluster
  See ZCL Specification Section 6.3
*******************************/

/* Server Sent Commands */

zbStatus_t ZCL_CurrentWeeklyScheduleRsp
(
  zclThermostat_CurrentWeeklySchedule_t *pReq
)
{
  uint8_t cmdSize = sizeof(zclCmdThermostat_CurrentWeeklySchedule_t) + (((pReq->cmdFrame.NumberOfTransactionsForSequence) - 1) * sizeof(transitionSetPoint_t));
  return ZCL_SendServerRspSeqPassed(gZclCmdThermostat_CurrentWeeklySchedule_c, cmdSize, (zclGenericReq_t *)pReq);
}

/* Client Sent Commands */

zbStatus_t ZCL_ThermostatSetpointRaiseLowerReq
(
  zclThermostat_SetpointRaiseLower_t *pReq
)
{
  uint8_t iPayloadLen;
  afToApsdeMessage_t *pMsg;
  iPayloadLen = sizeof(zclCmdThermostat_SetpointRaiseLower_t);
  pMsg = ZCL_CreateFrame(&(pReq->addrInfo), gZclCmdThermostat_SetpointRaiseLower_c,gZclFrameControl_FrameTypeSpecific,
                NULL, &iPayloadLen,&(pReq->cmdFrame));
  if(!pMsg)
    return gZclNoMem_c;
  
 return ZCL_DataRequestNoCopy(&(pReq->addrInfo), iPayloadLen,pMsg);
}

zbStatus_t ZCL_ThermostatSetWeeklyScheduleReq
(
  zclThermostat_SetWeeklySchedule_t *pReq
)
{
  uint8_t cmdSize = sizeof(zclCmdThermostat_SetWeeklySchedule_t) + (((pReq->cmdFrame.NumberOfTransactionsForSequence) - 1) * sizeof(transitionSetPoint_t));
  return ZCL_SendClientReqSeqPassed(gZclCmdThermostat_SetWeeklySchedule_c, cmdSize, (zclGenericReq_t *)pReq);
}

zbStatus_t ZCL_ThermostatGetWeeklyScheduleReq
(
  zclThermostat_GetWeeklySchedule_t *pReq
)
{
  return ZCL_SendClientReqSeqPassed(gZclCmdThermostat_GetWeeklySchedule_c, sizeof(zclCmdThermostat_GetWeeklySchedule_t), (zclGenericReq_t *)pReq);
}

zbStatus_t ZCL_ThermostatClearWeeklyScheduleReq
(
  zclThermostat_ClearWeeklySchedule_t *pReq
)
{
  return ZCL_SendClientReqSeqPassed(gZclCmdThermostat_ClearWeeklySchedule_c, 0, (zclGenericReq_t *)pReq);
}


void ZCL_SetThermostatAttribute(zbEndPoint_t endPoint, zbClusterId_t aClusterId,zclAttrId_t attrId, int16_t attrValue) {
  int16_t Temp16;
  Temp16 = Native2OTA16(attrValue);
  (void) ZCL_SetAttribute(endPoint,aClusterId,attrId,&Temp16);
}

/*****************************************************************************
* ZCL_ThermostatSetpointRaiseLower Fuction
*
* Returns gZbSuccess_t if worked.
*****************************************************************************/
zclStatus_t ZCL_ThermostatSetpointRaiseLower
(
  zbEndPoint_t endPoint,
  zclCmdThermostat_SetpointRaiseLower_t* pReq
)
{
  int16_t OccupiedCoolingSetpoint;
  int16_t OccupiedHeatingSetpoint;
  int16_t TemperatureCoolSetpoint=0;
  int16_t TemperatureHeatSetpoint =0;
  int16_t  SetpointDiference;
  uint8_t Attrlen;
  int16_t MinHeatSetpointLimit;
  int16_t MaxHeatSetpointLimit;
  int16_t MinCoolSetpointLimit;
  int16_t MaxCoolSetpointLimit;
  int16_t Temp16;
  zbClusterId_t aClusterId;
  Set2Bytes(aClusterId, gZclClusterThermostat_c);

/* Get the data from the attributes to do the operations */
/*   ZCL_GetAttribute(zbEndPoint_t ep, zbClusterId_t aClusterId, zclAttrId_t attrId, void * pAttrData, uint8_t * pAttrLen) */
(void) ZCL_GetAttribute(endPoint,aClusterId,gZclAttrThermostat_OccupiedCoolingSetpointId_c,&Temp16, &Attrlen);
 OccupiedCoolingSetpoint = OTA2Native16(Temp16);

(void) ZCL_GetAttribute(endPoint,aClusterId,gZclAttrThermostat_OccupiedHeatingSetpointId_c,&Temp16, &Attrlen);
 OccupiedHeatingSetpoint = OTA2Native16(Temp16);
 
(void) ZCL_GetAttribute(endPoint,aClusterId,gZclAttrThermostat_MinCoolSetpointLimitId_c,&Temp16, &Attrlen);
 MinCoolSetpointLimit = OTA2Native16(Temp16);
 
(void) ZCL_GetAttribute(endPoint,aClusterId,gZclAttrThermostat_MinHeatSetpointLimitId_c,&Temp16, &Attrlen);
 MinHeatSetpointLimit = OTA2Native16(Temp16);
 
(void) ZCL_GetAttribute(endPoint,aClusterId,gZclAttrThermostat_MaxCoolSetpointLimitId_c,&Temp16, &Attrlen);
 MaxCoolSetpointLimit = OTA2Native16(Temp16);
 
(void) ZCL_GetAttribute(endPoint,aClusterId,gZclAttrThermostat_MaxHeatSetpointLimitId_c,&Temp16, &Attrlen);
 MaxHeatSetpointLimit = OTA2Native16(Temp16);

  TemperatureCoolSetpoint = (OccupiedCoolingSetpoint);
  TemperatureHeatSetpoint = (OccupiedHeatingSetpoint);
  SetpointDiference =TemperatureCoolSetpoint - TemperatureHeatSetpoint;
  switch(pReq->Mode){
    case (gThermostatModeHeat_c):
         TemperatureHeatSetpoint += (int16_t)(pReq->Amount);
      break;
    case (gThermostatModeCool_c):
         TemperatureCoolSetpoint += (int16_t)(pReq->Amount);
     break;
    case (gThermostatModeBoth_c):
         TemperatureHeatSetpoint += (int16_t)(pReq->Amount);
         TemperatureCoolSetpoint += (int16_t)(pReq->Amount);
      break;
  }/*Switch (pReq-> Mode)*/

         if (pReq->Amount < 0){
             if (TemperatureHeatSetpoint < MinHeatSetpointLimit){
                 TemperatureHeatSetpoint = MinHeatSetpointLimit;
                 TemperatureCoolSetpoint = TemperatureHeatSetpoint + SetpointDiference;
                 }
             if (TemperatureCoolSetpoint < MinCoolSetpointLimit){
                 TemperatureCoolSetpoint = MinCoolSetpointLimit;
                 TemperatureHeatSetpoint = TemperatureCoolSetpoint - SetpointDiference;
                 }
             }
         else{
             if (TemperatureHeatSetpoint > MaxHeatSetpointLimit){
                 TemperatureHeatSetpoint = MaxHeatSetpointLimit;
                 TemperatureCoolSetpoint = TemperatureHeatSetpoint + SetpointDiference;
                 }
             if (TemperatureCoolSetpoint > MaxCoolSetpointLimit){
                 TemperatureCoolSetpoint = MaxCoolSetpointLimit;
                 TemperatureHeatSetpoint = TemperatureCoolSetpoint - SetpointDiference;
                 }
              }

  OccupiedCoolingSetpoint = TemperatureCoolSetpoint;
  OccupiedHeatingSetpoint = TemperatureHeatSetpoint;

/* write the new recalculated values on the cluster attributes */
  ZCL_SetThermostatAttribute(endPoint,  aClusterId,gZclAttrThermostat_OccupiedCoolingSetpointId_c, OccupiedCoolingSetpoint);
  ZCL_SetThermostatAttribute(endPoint,  aClusterId,gZclAttrThermostat_OccupiedHeatingSetpointId_c, OccupiedHeatingSetpoint);
  ZCL_SetThermostatAttribute(endPoint,  aClusterId,gZclAttrThermostat_MinCoolSetpointLimitId_c, MinCoolSetpointLimit);
  ZCL_SetThermostatAttribute(endPoint,  aClusterId,gZclAttrThermostat_MinHeatSetpointLimitId_c, MinHeatSetpointLimit);
  ZCL_SetThermostatAttribute(endPoint,  aClusterId,gZclAttrThermostat_MaxCoolSetpointLimitId_c, MaxCoolSetpointLimit);
  ZCL_SetThermostatAttribute(endPoint,  aClusterId,gZclAttrThermostat_MaxHeatSetpointLimitId_c, MaxHeatSetpointLimit);

  return gZbSuccess_c;
}

/*****************************************************************************
* ZCL_ThermostatClearWeeklySchedule Fuction
*
* Interface assumptions: pData is not null.
*****************************************************************************/
zclStatus_t ZCL_ThermostatClearWeeklySchedule(haThermostatWeeklyScheduleRAM_t *pData)
{
  index_t i, j;
  /* clear gHaThermostatWeeklyScheduleData */
  for(i=0; i<8; i++)
  {
    for(j=0; j<gHaThermostatWeeklyScheduleEntries_d; j++)
    {
      /* an entry is not in use if transitionTime is ffff */
      pData->WeeklySetPoints[i][j].TransitionTime = 0xffff;
    }
  }
  return gZclSuccess_c;
}

/*****************************************************************************
* ZCL_ProcessSetWeeklySchedule Fuction
*
* Returns gZclSuccessDefaultRsp_c if worked.
*****************************************************************************/
zbStatus_t ZCL_ProcessSetWeeklySchedule
(
  zbApsdeDataIndication_t *pIndication
)
{
  uint8_t i, j, k;
  uint8_t daysUpdated = 0x00;
  zbStatus_t status = gZclSuccessDefaultRsp_c;
  zclCmdThermostat_SetWeeklySchedule_t *pMsg;
  
  /* get the set Weekly Schedule command */
  pMsg = (zclCmdThermostat_SetWeeklySchedule_t *)((uint8_t*)pIndication->pAsdu + sizeof(zclFrame_t));
  
  /* go through all the entries in the set weekly schedule request */
  for(i=0; i<pMsg->NumberOfTransactionsForSequence; i++)
  {
    /* go through all the days for the current entry */
    for(j=0; j<8; j++)
    {
      /* check if the current entry applies to the current day */
      if(pMsg->WeeklySetPointSchedule[i].TransitionDayOfWeek & (1 << j))
      {
        /* check if an entry has allready been added for the current day,
        if not erase all entries for the current day */
        if(!(daysUpdated & j))
        {
          /* go through all the entries for this day */
          for(k=0; k<gHaThermostatWeeklyScheduleEntries_d; k++)
          {
            /* mark all entries as invalid */
            gHaThermostatWeeklyScheduleData.WeeklySetPoints[j][k].TransitionTime = 0xffff;
          }
          /* mark the current day as updated */
          daysUpdated = daysUpdated | j;
        }
        /* go through all the entries for this day */
        for(k=0; k<gHaThermostatWeeklyScheduleEntries_d; k++)
        {
          /* look for a free entry */
          if(gHaThermostatWeeklyScheduleData.WeeklySetPoints[j][k].TransitionTime == 0xffff)
          {
            /* add the info to the current entry */
            FLib_MemCpy(&gHaThermostatWeeklyScheduleData.WeeklySetPoints[j][k],
                        &pMsg->WeeklySetPointSchedule[i].SetPoint, sizeof(thermostatSetPoint_t));
            break;
          }
          if(k==gHaThermostatWeeklyScheduleEntries_d)
          {
            /* no free entry was found, set staus to insuffiecient space */
            status = gZclInsufficientSpace_c;
          }
        }
      }
    }
  }
  return status;
}
/*****************************************************************************
* ZCL_ProcessGetWeeklySchedule Fuction
*
* Returns gZclSuccessDefaultRsp_c if worked.
*****************************************************************************/
zbStatus_t ZCL_ProcessGetWeeklySchedule
(
  zbApsdeDataIndication_t *pIndication
)
{
  zbStatus_t status = gZclSuccessDefaultRsp_c;
  uint8_t day, entryNumber;
  uint8_t numberOfTransitions = 0;
  zclThermostat_CurrentWeeklySchedule_t *pCurrentSchedule;
  zclCmdThermostat_GetWeeklySchedule_t *pMsg;
  
  pCurrentSchedule = AF_MsgAlloc();
  if(!pCurrentSchedule)
  {
    return gZclNoMem_c; 
  }
  /* get the get Weekly Schedule command */
  pMsg = (zclCmdThermostat_GetWeeklySchedule_t *)((uint8_t*)pIndication->pAsdu + sizeof(zclFrame_t));
  /* build the response */
  AF_PrepareForReply(&pCurrentSchedule->addrInfo, pIndication);
  pCurrentSchedule->zclTransactionId = ((zclFrame_t *)pIndication->pAsdu)->transactionId;
  /* go through all the days of the week */
  for(day=0; day < 8; day++)
  {
    /* Check what days must be returned in the Current Weekly Schedule command */
    if(pMsg->DaysToReturn & (1 << day))
    {
      /* go through all the entries for the current day in the gHaThermostatWeeklyScheduleData */
      for(entryNumber=0; entryNumber<gHaThermostatWeeklyScheduleEntries_d; entryNumber++)
      {
        /* if the current entry is valid add it to the response */
        if(gHaThermostatWeeklyScheduleData.WeeklySetPoints[day][entryNumber].TransitionTime != 0xffff)
        {
          pCurrentSchedule->cmdFrame.WeeklySetPointSchedule[numberOfTransitions].TransitionDayOfWeek = day;
          FLib_MemCpy(&pCurrentSchedule->cmdFrame.WeeklySetPointSchedule[numberOfTransitions].SetPoint,
                      &gHaThermostatWeeklyScheduleData.WeeklySetPoints[day][entryNumber],
                      sizeof(thermostatSetPoint_t));
          numberOfTransitions++;
        }
        /* Maximum 10 entries in one response */
        if(numberOfTransitions == 10)
        {
          /* We send a response with 10 entries and build another response if more entries are requested */
          pCurrentSchedule->cmdFrame.NumberOfTransactionsForSequence = numberOfTransitions;
          status = ZCL_CurrentWeeklyScheduleRsp(pCurrentSchedule);
          if(status != gZbSuccess_c)
          {
            return status;
          }
          /* Reset numberOfTransitions for the next response */
          numberOfTransitions = 0;
        }
      }
    }
  }
  if(numberOfTransitions)
  {
    pCurrentSchedule->cmdFrame.NumberOfTransactionsForSequence = numberOfTransitions;
    status = ZCL_CurrentWeeklyScheduleRsp(pCurrentSchedule);
  }
  MSG_Free(pCurrentSchedule);
  return status;
}
/*****************************************************************************
* ZCL_ThermostatValidationFunction
*
* Validation function for thermostat attributes
*****************************************************************************/
#if gAddValidationFuncPtrToClusterDef_c

bool_t  ZCL_ThermostatValidateAttributes(zbEndPoint_t endPoint, zbClusterId_t clusterId, void *pData)
{ 
  zclCmdWriteAttrRecord_t *pRecord = (zclCmdWriteAttrRecord_t*) pData;  
  zclAttrData_t *pAttrData = (zclAttrData_t*) pRecord->aData;
  uint16_t min, max;
  uint8_t len;
  
  
  switch (pRecord->attrId)
  {    
  case gZclAttrThermostat_LocalTemperatureCalibrationId_c:
    {
      return ((pAttrData->data8 >= 0x19) && (pAttrData->data8 <= 0xE7));      
    }
  case gZclAttrThermostat_OccupiedCoolingSetpointId_c:
  case gZclAttrThermostat_UnoccupiedCoolingSetpointId_c:
    {
      (void) ZCL_GetAttribute(endPoint, clusterId, gZclAttrThermostat_MinCoolSetpointLimitId_c,&min, &len);
      (void) ZCL_GetAttribute(endPoint, clusterId, gZclAttrThermostat_MaxCoolSetpointLimitId_c,&max, &len);
      min = OTA2Native16(min);
      max = OTA2Native16(max);    
      return ((OTA2Native16(pAttrData->data16) >= min) && (OTA2Native16(pAttrData->data16) <= max));
    }  
  case gZclAttrThermostat_OccupiedHeatingSetpointId_c:  
  case gZclAttrThermostat_UnoccupiedHeatingSetpointId_c:
    {    
      (void) ZCL_GetAttribute(endPoint, clusterId, gZclAttrThermostat_MinHeatSetpointLimitId_c,&min, &len);
      (void) ZCL_GetAttribute(endPoint, clusterId, gZclAttrThermostat_MaxHeatSetpointLimitId_c,&max, &len);
      min = OTA2Native16(min);
      max = OTA2Native16(max);    
      return ((OTA2Native16(pAttrData->data16) >= min) && (OTA2Native16(pAttrData->data16) <= max));
    }    
  case gZclAttrThermostat_MinHeatSetpointLimitId_c:
  case gZclAttrThermostat_MaxHeatSetpointLimitId_c:    
  case gZclAttrThermostat_MinCoolSetpointLimitId_c:
  case gZclAttrThermostat_MaxCoolSetpointLimitId_c:
    {    
      return ((OTA2Native16(pAttrData->data16) <= 0x7FFF) || (OTA2Native16(pAttrData->data16) >= 0x954D));
    }    
  case gZclAttrThermostat_MinSetpointDeadBandId_c:
    {
      return ((pAttrData->data8 >= 0x0A) && (pAttrData->data8 <= 0x19));
    }  
  case gZclAttrThermostat_ControlSequenceOfOperationId_c:
    {
      return (pAttrData->data8 <= 0x05);
    }  
  case gZclAttrThermostat_SystemModeId_c:
    {
      return (pAttrData->data8 <= 0x02);
    }
  case gZclAttrThermostat_RemoteSensingId_c:
  case gZclAttrThermostat_AlarmMaskId_c:
    {
      return (pAttrData->data8 <= 0x07);
    }
  case gZclAttrThermostat_StartOfWeek_c:
    {
      return (pAttrData->data8 <= 0x06);
    }
  case gZclAttrThermostat_NumberOfWeeklyTransitions_c:
  default:
    return TRUE;
  }
}

bool_t  ZCL_ThermostatUserCfgValidateAttributes(zbEndPoint_t endPoint, zbClusterId_t clusterId, void *pData)
{  
  zclCmdWriteAttrRecord_t *pRecord = (zclCmdWriteAttrRecord_t*) pData;  
  zclAttrData_t *pAttrData = (zclAttrData_t*) pRecord->aData;
 (void) endPoint;
 (void) clusterId;
    
  switch (pRecord->attrId)
  {        
    case gZclAttrThermostatUserInterface_TempDisplayModeId_c:
      {
    return (pAttrData->data8 <= 0x01);        
      }
    case gZclAttrThermostatUserInterface_KeypadLockoutId_c:  
      {
    return (pAttrData->data8 <= 0x05);        
      }
  default:
    return TRUE;
  }
} 


#endif /* gZclEnableAttributeValidation_c */

/*****************************************************************************
* ZCL_ThermostatCluster Server Fuction
*
* Returns gZbSuccess_t if worked.
*****************************************************************************/
zbStatus_t ZCL_ThermostatClusterServer
  (
  zbApsdeDataIndication_t *pIndication, 
  afDeviceDef_t *pDevice
  )
{
    zclFrame_t *pFrame;
    zbStatus_t status = gZclSuccessDefaultRsp_c;
    zclCmdThermostat_SetpointRaiseLower_t Command;
    zclUIEvent_t event;
    /*
      BUGBUG: Parameter not used.
    */
    (void)pDevice;
    /* ZCL frame */
    pFrame = (zclFrame_t*)pIndication->pAsdu;

    /* handle the LevelControl commands */
    switch(pFrame->command)
    {
      /* Thermostat Setpoint Raise Lower */
    case gZclCmdThermostat_SetpointRaiseLower_c:
      FLib_MemCpy(&Command ,(pFrame + 1), sizeof(zclCmdThermostat_SetpointRaiseLower_t));
      (void)ZCL_ThermostatSetpointRaiseLower(pIndication->dstEndPoint,&Command);
      if (Command.Amount > 0)
        event = gZclUI_ThermostatRaiseSetpoint_c;
      else
        event = gZclUI_ThermostatLowerSetpoint_c;

      BeeAppUpdateDevice(pIndication->dstEndPoint, event, 0, 0, NULL);
      break;
    case gZclCmdThermostat_SetWeeklySchedule_c:
      status = ZCL_ProcessSetWeeklySchedule(pIndication);
      break;
    case gZclCmdThermostat_GetWeeklySchedule_c:
      status = ZCL_ProcessGetWeeklySchedule(pIndication);
      break;
    case gZclCmdThermostat_ClearWeeklySchedule_c:
      status = ZCL_ThermostatClearWeeklySchedule(&gHaThermostatWeeklyScheduleData);
      break;
      /* command not supported on this cluster */
    default:
      return gZclUnsupportedClusterCommand_c;
    }
    return status;
}

/******************************
  Fan Control Cluster
  See ZCL Specification Section 6.4
*******************************/
zbStatus_t ZCL_FanControlClusterServer
  (
  zbApsdeDataIndication_t *pIndication, 
  afDeviceDef_t *pDevice
  )
{
    (void) pIndication;
    (void) pDevice;
    return gZbSuccess_c;
}

/******************************
  Dehumidification Cluster
  See ZCL Specification Section 6.5
*******************************/

/******************************
  Thermostat User Interface  Configuration Cluster
  See ZCL Specification Section 6.6
*******************************/
zbStatus_t ZCL_ThermostatUserInterfaceCfgClusterServer
  (
  zbApsdeDataIndication_t *pIndication, 
  afDeviceDef_t *pDevice
  )
{
    (void) pIndication;
    (void) pDevice;
    return gZbSuccess_c;
}

