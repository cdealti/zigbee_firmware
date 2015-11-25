/******************************************************************************
* ZclSensing.h
*
* Types, definitions and prototypes for the
* measurement and sensing Domain of clusters.
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
#ifndef _ZCL_SENSING_H
#define _ZCL_SENSING_H

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

/******************************************
	Temperature Measurement Cluster
*******************************************/

/* 4.4.2.2 Temperature Measurement Information cluster attributes */
#ifdef __IAR_SYSTEMS_ICC__
#define gZclAttrTemperatureMeasurement_MeasuredValueId_c        0x0000  /* M - Measured Value */
#define gZclAttrTemperatureMeasurement_MinMeasuredValuedId_c    0x0001  /* M - MinMeasuredValue*/
#define gZclAttrTemperatureMeasurement_MaxMeasuredValuedId_c    0x0002  /* M - MaxMeasuredValued */
#define gZclAttrTemperatureMeasurement_ToleranceId_c            0x0003 /* M - Tolerance */
#else
#define gZclAttrTemperatureMeasurement_MeasuredValueId_c        0x0000  /* M - Measured Value */
#define gZclAttrTemperatureMeasurement_MinMeasuredValuedId_c    0x0100  /* M - MinMeasuredValue*/
#define gZclAttrTemperatureMeasurement_MaxMeasuredValuedId_c    0x0200  /* M - MaxMeasuredValued */
#define gZclAttrTemperatureMeasurement_ToleranceId_c            0x0300 /* M - Tolerance */
#endif

/* The comand use for this cluster are the read/write attributes, and reporting attributes */
#ifdef __IAR_SYSTEMS_ICC__
#pragma pack(1)
#endif
typedef struct zclTemperatureMeasurementAttrs_tag
{
  int16_t      MeasuredValue[zclReportableCopies_c] ;      /* M - Measured Value */
  int16_t      MinMeasuredValue;   /* M - Minimal Measured Value */
  int16_t      MaxMeasuredValue;   /* M - Maximal Measured Value */
#if gZclClusterOptionals_d
  uint16_t  Tolerance[zclReportableCopies_c];                    /* O - Tolerance */
#endif
} zclTemperatureMeasurementAttrs_t;

/******************************************
	Occupancy Sensing Cluster
*******************************************/
/* 4.8.2.2 Attributes of the occupancy sensor information attribute set */
#ifdef __IAR_SYSTEMS_ICC__
#define gZclAttrOccupancySensing_OccupancyId_c                             0x0000 /* M - Occupancy */
#define gZclAttrOccupancySensing_OccupancySensorTypeId_c                   0x0001 /* M - Occupancy Sensor Type */
#define gZclAttrOccupancySensing_PIROccupiedToUnoccupiedDelayId_c          0x0010 /* O - PIR Sensor configuration Occupied to Unoccupied Delay */
#define gZclAttrOccupancySensing_PIRUnoccupiedToOccupiedDelayId_c          0x0011 /* O - PIR Sensor configuration Unoccupied to Occupied Delay */
#define gZclAttrOccupancySensing_PIRUnoccupiedToOccupiedThresholdId_c      0x0012 /* O - PIR Sensor configuration Unoccupied to Occupied Threshold */
#define gZclAttrOccupancySensing_UltrasonicOccupiedToUnoccupiedDelayId_c   0x0020 /* O - Ultrasonic Sensor configuration Occupied to Unoccupied Delay */
#define gZclAttrOccupancySensing_UltrasonicUnoccupiedToOccupiedDelayId_c   0x0021 /* O - Ultrasonic Sensor configuration Unoccupied to Occupied Delay */
#define gZclAttrOccupancySensing_UltrasonicUnoccupiedToOccupiedThresholdId_c 0x0022 /* O - Ultrasonic Sensor configuration Unoccupied to Occupied Threshold */
#else
#define gZclAttrOccupancySensing_OccupancyId_c                             0x0000 /* M - Occupancy */
#define gZclAttrOccupancySensing_OccupancySensorTypeId_c                   0x0100 /* M - Occupancy Sensor Type */
#define gZclAttrOccupancySensing_PIROccupiedToUnoccupiedDelayId_c          0x1000 /* O - PIR Sensor configuration Occupied to Unoccupied Delay */
#define gZclAttrOccupancySensing_PIRUnoccupiedToOccupiedDelayId_c          0x1100 /* O - PIR Sensor configuration Unoccupied to Occupied Delay */
#define gZclAttrOccupancySensing_PIRUnoccupiedToOccupiedThresholdId_c      0x1200 /* O - PIR Sensor configuration Unoccupied to Occupied Threshold */
#define gZclAttrOccupancySensing_UltrasonicOccupiedToUnoccupiedDelayId_c   0x2000 /* O - Ultrasonic Sensor configuration Occupied to Unoccupied Delay */
#define gZclAttrOccupancySensing_UltrasonicUnoccupiedToOccupiedDelayId_c   0x2100 /* O - Ultrasonic Sensor configuration Unoccupied to Occupied Delay */
#define gZclAttrOccupancySensing_UltrasonicUnoccupiedToOccupiedThresholdId_c 0x2200 /* O - Ultrasonic Sensor configuration Unoccupied to Occupied Threshold */
#endif

/* 4.8.2.2.1.2 Values of the OccupancySensorType attribute */
enum{
  gZclTypeofSensor_PIR_c = 0x00,               /* PIR sensor type */
  gZclTypeofSensor_Ultrasonic_c = 0x01,        /* Ultrasonic sensor type */
  gZclTypeofSensor_PirAndUltrasonic_c = 0x02   /* PIR and Utrasonic sensor type */
};

/*Cluster Commands:
	This cluster shall support attribute reporting using the Report Attributes command and according to the
    minimum and maximum reporting interval settings described in the ZCL Specification (see
    [R2]). The following attribute shall be reported:
    Occupancy.
    No commands are generated by the server or received by the client cluster, except responses to read 
    and write attribute commands, and attribute reports.
	*/

typedef struct zclOccupancySensingAttrs_tag
{
  uint8_t    Occupancy;                            /* M - Occupancy is a 8b bitmap */
  uint8_t OccupancySensorType;  /* M - Occupancy Sensor type, is an enumeration */
#if gZclClusterOptionals_d
  uint16_t PIROccupiedToUnoccupiedDelay;            /* O - PIR Sensor configuration Occupied to Unoccupied Delay */
  uint16_t PIRUnoccupiedToOccupiedDelay;            /* O - PIR Sensor configuration Unoccupied to Occupied Delay */
  uint8_t  PIRUnoccupiedToOccupiedThreshold;         /* O - PIR Sensor configuration Unoccupied to Occupied Threshold */
  uint16_t UltrasonicOccupiedToUnoccupiedDelay;     /* O - Ultrasonic Sensor configuration Occupied to Unoccupied Delay */
  uint16_t UltrasonicUnoccupiedToOccupiedDelay;     /* O - Ultrasonic Sensor configuration Unoccupied to Occupied Delay */
  uint8_t  UltrasonicUnoccupiedToOccupiedThreshold;     /* O - Ultrasonic Sensor configuration Unoccupied to Occupied Threshold */
#endif
} zclOccupancySensingAttrs_t;

typedef struct zclOccupancySetAttrValue_tag
{
  uint8_t      ep;     /* endpoint*/
  uint8_t      valueAttr;
  uint16_t     attrID;
} zclOccupancySetAttrValue_t;



#ifdef __IAR_SYSTEMS_ICC__
#pragma pack()
#endif
/******************************************************************************
*******************************************************************************
* Public functions prototypes
*******************************************************************************
******************************************************************************/
zbStatus_t ZCL_TemperatureMeasureClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice);
zbStatus_t ZCL_OccupancySensingClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice);
#endif  /* _ZCL_SENSING_H */

