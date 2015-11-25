
/************************************************************************************
*
* (c) Copyright 2008, Freescale, Inc.  All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
************************************************************************************/

#include "Interrupt.h"
#include "ADC_Interface.h"
#include "PWR_interface.h"

/******************************************************************************
*******************************************************************************
* Public macros
*******************************************************************************
******************************************************************************/

#ifndef gTempSensorEnable_d
#define gTempSensorEnable_d         1
#endif
#define gAccelerometerEnable_d      1
#define gPressureSensorEnable_d     0
#define gBatteryLevelSensorEnable_d 0

#ifndef gTempSensorEnable_d
#if (gTargetMC1322xSRB == TRUE)
#define gTempSensorEnable_d 1
#else 
#define gTempSensorEnable_d 0
#endif
#endif

#ifndef gAccelerometerEnable_d
#if (gTargetMC1322xSRB == TRUE)
#define gAccelerometerEnable_d 1
#else 
#define gAccelerometerEnable_d 0
#endif
#endif

#ifndef gPressureSensorEnable_d
#if (gTargetMC1322xSRB == TRUE)
#define gPressureSensorEnable_d 1
#else 
#define gPressureSensorEnable_d 0
#endif
#endif

#ifndef gBatteryLevelSensorEnable_d
#define gBatteryLevelSensorEnable_d 1
#endif


#define mAdcFifoThreshold_c (gTempSensorEnable_d + (gAccelerometerEnable_d * 3) + gPressureSensorEnable_d + gBatteryLevelSensorEnable_d)


/******************************************************************************
*******************************************************************************
* Public prototypes
*******************************************************************************
******************************************************************************/

#if (gTempSensorEnable_d | gAccelerometerEnable_d | gPressureSensorEnable_d | gBatteryLevelSensorEnable_d)
typedef void (*EvSensorReadCallback)(void);
void App_AdcInit(EvSensorReadCallback callback);
#if (gTempSensorEnable_d)
int8_t App_ConvertTemperature(uint16_t val);
#endif
#if (gBatteryLevelSensorEnable_d)
uint8_t App_ConvertBaterryLevel(uint16_t val);
#endif
#if (gAccelerometerEnable_d)
uint8_t App_ConvertAccelerometerData(uint16_t val);
#endif
#if (gPressureSensorEnable_d)
uint8_t App_ConvertPressureData(uint8_t val);
#endif
#endif