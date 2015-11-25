/******************************************************************************
* ZclSensing.c
*
* This module contains code that handles ZigBee Cluster Library commands for the
* measurement and sensing commands.
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
#include "EmbeddedTypes.h"
#include "zigbee.h"
#include "FunctionLib.h"
#include "BeeStackConfiguration.h"
#include "BeeStack_Globals.h"
#include "AppAfInterface.h"
#include "AfApsInterface.h"
#include "BeeStackInterface.h"
#include "ZCLSensing.h"

#include "ZclOptions.h"

/******************************************************************************
*******************************************************************************
* Private macros
*******************************************************************************
******************************************************************************/

/******************************
  Illuminance Measurement Cluster
  See ZCL Specification Section 4.2
*******************************/

/******************************
  Illuminance Level Sensing Cluster
  See ZCL Specification Section 4.3
*******************************/

/******************************
  Temperature Measurement Cluster
  See ZCL Specification Section 4.4
*******************************/
const zclAttrDef_t gaZclTemperatureMeasurementClusterAttrDef[] = {  
  {gZclAttrTemperatureMeasurement_MeasuredValueId_c,     gZclDataTypeInt16_c,gZclAttrFlagsInRAM_c | gZclAttrFlagsReportable_c,sizeof(int16_t),  (void *)MbrOfs(zclTemperatureMeasurementAttrs_t,MeasuredValue)},
  {gZclAttrTemperatureMeasurement_MinMeasuredValuedId_c, gZclDataTypeInt16_c,gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c,sizeof(int16_t),  (void *)MbrOfs(zclTemperatureMeasurementAttrs_t,MinMeasuredValue)},
  {gZclAttrTemperatureMeasurement_MaxMeasuredValuedId_c, gZclDataTypeInt16_c,gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c,sizeof(int16_t),  (void *)MbrOfs(zclTemperatureMeasurementAttrs_t,MaxMeasuredValue)}
#if gZclClusterOptionals_d
  , {gZclAttrTemperatureMeasurement_ToleranceId_c, gZclDataTypeUint16_c,gZclAttrFlagsInRAM_c | gZclAttrFlagsReportable_c,sizeof(uint16_t),  (void *)MbrOfs(zclTemperatureMeasurementAttrs_t,Tolerance)}
#endif
};

const zclAttrDefList_t gZclTemperatureMeasurementClusterAttrDefList = {
  NumberOfElements(gaZclTemperatureMeasurementClusterAttrDef),
  gaZclTemperatureMeasurementClusterAttrDef
};

/******************************
  Pressure Measurement Cluster
  See ZCL Specification Section 4.5
*******************************/

/******************************
  Flow Measurement Cluster
  See ZCL Specification Section 4.6
*******************************/

/******************************
  Relative Humidity Measurement Cluster
  See ZCL Specification Section 4.7
*******************************/

/******************************
  Occupancy Sensing Cluster
  See ZCL Specification Section 4.8
*******************************/
const zclAttrDef_t gaZclOccupancySensingClusterAttrDef[] = {  
  //occupancy sensor information set
  {gZclAttrOccupancySensing_OccupancyId_c , gZclDataTypeBitmap8_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsReportable_c | gZclAttrFlagsRdOnly_c, sizeof(uint8_t),  (void *)MbrOfs(zclOccupancySensingAttrs_t, Occupancy)},
  {gZclAttrOccupancySensing_OccupancySensorTypeId_c, gZclDataTypeEnum8_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint8_t),  (void *)MbrOfs(zclOccupancySensingAttrs_t, OccupancySensorType)}
  //PIR configuration set
#if gZclClusterOptionals_d
  , {gZclAttrOccupancySensing_PIROccupiedToUnoccupiedDelayId_c, gZclDataTypeUint16_c, gZclAttrFlagsInRAM_c ,sizeof(int16_t),  (void *)MbrOfs(zclOccupancySensingAttrs_t, PIROccupiedToUnoccupiedDelay)},
  {gZclAttrOccupancySensing_PIRUnoccupiedToOccupiedDelayId_c, gZclDataTypeUint16_c, gZclAttrFlagsInRAM_c ,sizeof(uint16_t),  (void *)MbrOfs(zclOccupancySensingAttrs_t, PIRUnoccupiedToOccupiedDelay)},
  {gZclAttrOccupancySensing_PIRUnoccupiedToOccupiedThresholdId_c, gZclDataTypeUint8_c, gZclAttrFlagsInRAM_c ,sizeof(uint8_t),  (void *)MbrOfs(zclOccupancySensingAttrs_t, PIRUnoccupiedToOccupiedThreshold)}
#endif
  //ultrasonic configuration set  
#if gZclClusterOptionals_d
  , {gZclAttrOccupancySensing_UltrasonicOccupiedToUnoccupiedDelayId_c, gZclDataTypeUint16_c,gZclAttrFlagsInRAM_c, sizeof(int16_t),  (void *)MbrOfs(zclOccupancySensingAttrs_t, UltrasonicOccupiedToUnoccupiedDelay)},
  {gZclAttrOccupancySensing_UltrasonicUnoccupiedToOccupiedDelayId_c, gZclDataTypeUint16_c,gZclAttrFlagsInRAM_c, sizeof(uint16_t),  (void *)MbrOfs(zclOccupancySensingAttrs_t, UltrasonicUnoccupiedToOccupiedDelay)},
  {gZclAttrOccupancySensing_UltrasonicUnoccupiedToOccupiedThresholdId_c, gZclDataTypeUint8_c,gZclAttrFlagsInRAM_c, sizeof(uint8_t),  (void *)MbrOfs(zclOccupancySensingAttrs_t, UltrasonicUnoccupiedToOccupiedThreshold)}
#endif
};

const zclAttrDefList_t gZclOccupancySensingClusterAttrDef = {
  NumberOfElements(gaZclOccupancySensingClusterAttrDef),
  gaZclOccupancySensingClusterAttrDef
};


/******************************************************************************
*******************************************************************************
* Public Functions
*******************************************************************************
******************************************************************************/
/* for read/write/report attribute commands */
/******************************
  Temperature Measurement Cluster
  See ZCL Specification Section 4.4
*******************************/
zbStatus_t ZCL_TemperatureMeasureClusterServer
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
  Occupancy Sensing Cluster
  See ZCL Specification Section 4.8
*******************************/
zbStatus_t ZCL_OccupancySensingClusterServer
  (
  zbApsdeDataIndication_t *pIndication, 
  afDeviceDef_t *pDevice
  )
{
    (void) pIndication;
    (void) pDevice;
    return gZbSuccess_c;
}


