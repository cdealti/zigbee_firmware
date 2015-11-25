/*****************************************************************************
*                           
* (c) Copyright 2008, Freescale, Inc.  All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
******************************************************************************/
#include "ApplicationConf.h"
#include "ADC_Interface.h"
#include "Gpio_Interface.h"
#include "EmbeddedTypes.h"
#include "TS_Interface.h"
#include "TMR_Interface.h"
#include "FunctionLib.h"
#include "PublicConst.h"
#include "NVM_Interface.h"
#include "Interrupt.h"
#include "ADC_Interface.h"
#include "PWR_interface.h"
#include "AppToMC1322xADC.h"
#include "AppToPlatformConfig.h"

/************************************************************************************
*************************************************************************************
* Private macros
*************************************************************************************
************************************************************************************/

/* ADC constants */

/* Period between consecutive channel samplings */
#define mTimeBetweenSamples_c       6999 /* 2ms */

#define mAdcChannelAcceleratorX_c   gAdcChan2_c
#define mAdcChannelAcceleratorY_c   gAdcChan3_c
#define mAdcChannelAcceleratorZ_c   gAdcChan4_c
#define mAdcPressureSensor_c        gAdcChan5_c
#define mAdcTemperatureSensor_c     gAdcChan6_c

/* GPIO pins */
#define mGpioPinAcceleratorX_c      (1 << 0)  /* GPIO_32 */
#define mGpioPinAcceleratorY_c      (1 << 1)  /* GPIO_33 */
#define mGpioPinAcceleratorZ_c      (1 << 2)  /* GPIO_34 */
#define mGpioPinTemperatureSensor_c (1 << 3)  /* GPIO_35 */
#define mGpioPinPressureSensor_c    (1 << 4)  /* GPIO_36 */
#define mAdc2VRefH_c                (1 << 6)  /* GPIO_38 */
#define mAdc2VRefL_c                (1 << 7)  /* GPIO_39 */
#define mSleepMask_c                (1 << 8)  /* GPIO_8  */
#define mSleepPort_c                gGpioPort0_c
#define mGpioPinSleep_c             gGpioPin8_c 
#define mAdcChannelsPort_c          gGpioPort1_c

#define mPortMask_c                 (mGpioPinAcceleratorX_c | mGpioPinAcceleratorY_c | mGpioPinAcceleratorZ_c | \
                                     mGpioPinTemperatureSensor_c | mGpioPinPressureSensor_c)

#define mAdcChannels_c              (gAccelerometerEnable_d << mAdcChannelAcceleratorX_c ) | (gAccelerometerEnable_d << mAdcChannelAcceleratorY_c ) | \
                                    (gAccelerometerEnable_d << mAdcChannelAcceleratorZ_c ) | (gPressureSensorEnable_d << mAdcPressureSensor_c ) | \
                                    (gTempSensorEnable_d << mAdcTemperatureSensor_c )   | (gBatteryLevelSensorEnable_d << gAdcBatt_c)

/* degrees in Celsius to add/substract to compenstate for self heating etc. */
#define mTemperatureCompensation_c 2 

/* LM61B ZERO degrees celsius offset in milliVolt */
#define mTempSensormilliVoltOffset_c 600

/* LM61B milliVolt per 1 degree celsius */
#define mMilliVoltPerDegree_c 10

/* Voltage reference for battery level measurement */
#define mVRef_c 1.2


/************************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
************************************************************************************/


#if (gTempSensorEnable_d | gPressureSensorEnable_d | gBatteryLevelSensorEnable_d)
static float   mADCRef = 3.3;  /* default ADC voltage reference is 3.3V*/ 
#endif

EvSensorReadCallback SensorReadCallbackEvent = NULL;


/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
************************************************************************************/

int16_t gAccelerometerData[3] = {0, 0, 0};
int16_t gPressureData = 0;
int8_t  gTemperatureData = 0;
int8_t  gBatteryLevelData = 0;

/************************************************************************************
*************************************************************************************
* Private prototypes
*************************************************************************************
************************************************************************************/
#if (gTempSensorEnable_d | gAccelerometerEnable_d | gPressureSensorEnable_d | gBatteryLevelSensorEnable_d)
void App_AdcFifoCallback(void);
#endif

/*****************************************************************************
* Initialization function for the ADC module.
*
* Interface assumptions: None
*
* Return value: None
*
*****************************************************************************/
#if (gTempSensorEnable_d | gAccelerometerEnable_d | gPressureSensorEnable_d | gBatteryLevelSensorEnable_d)
void App_AdcInit(EvSensorReadCallback callback)
{
  AdcConfig_t adcConfig;
  AdcConvCtrl_t adcConvCtrl; 
  AdcFifoStatus_t fifoStatus;
  AdcFifoData_t adcFifoData;
  uint8_t  adcFifoLevel;
  
  /* init GPIOs for ADC */
  Gpio_SetPortFunction(mAdcChannelsPort_c, gGpioAlternate1Mode_c, mPortMask_c );
  Gpio_SetPortFunction(gGpioPort1_c, gGpioAlternate1Mode_c, (mAdc2VRefH_c | mAdc2VRefL_c));
  Gpio_SetPortFunction(mSleepPort_c, gGpioNormalMode_c, mSleepMask_c );
  Gpio_SetPinReadSource(mGpioPinSleep_c, gGpioPinReadReg_c );
  Gpio_SetPinDir(mGpioPinSleep_c, gGpioDirOut_c);
  Gpio_SetPinData(mGpioPinSleep_c, gGpioPinStateHigh_c);

  /* Initialize the ADC */
  Adc_Init();

  /* Assign ADC interrupt handle */
  IntAssignHandler(gAdcInt_c, (IntHandlerFunc_t)&Adc_Isr);
  ITC_SetPriority(gAdcInt_c, gItcNormalPriority_c);
  ITC_EnableInterrupt(gAdcInt_c);  
  Adc_DefaultConfig(adcConfig, SYSTEM_CLOCK);
  adcConfig.adcFifoIrqEn = TRUE;
  
  Adc_SetConfig(&adcConfig);
  
  /* Clear FIFO data */
  do {
    Adc_GetFifoStatus(&fifoStatus, &adcFifoLevel);
    if(gAdcFifoEmpty_c != fifoStatus)     
      Adc_ReadFifoData(&adcFifoData);
  }
  while(gAdcFifoEmpty_c != fifoStatus);
  
  /* Set callback functions */
  Adc_SetCallback(gAdcCompEvent_c, NULL);
  Adc_SetCallback(gAdcSeq1event_c, NULL);
  Adc_SetCallback(gAdcSeq2event_c, NULL);
  Adc_SetCallback(gAdcFifoEvent_c, &App_AdcFifoCallback);
  
  /* Set FIFO threshold */
  Adc_SetFifoCtrl(mAdcFifoThreshold_c);
  
  /* Set conversion control */
  adcConvCtrl.adcTmrOn = TRUE;
  adcConvCtrl.adcSeqIrqEn = FALSE;
  adcConvCtrl.adcChannels = mAdcChannels_c;
  adcConvCtrl.adcTmBtwSamples = mTimeBetweenSamples_c;
  adcConvCtrl.adcSeqMode = gAdcSeqOnTmrEv_c;
  adcConvCtrl.adcRefVoltage = gAdcBatteryRefVoltage_c;
  Adc_SetConvCtrl(gAdcPrimary_c, &adcConvCtrl);
  
  SensorReadCallbackEvent = callback;
}


/*****************************************************************************
* This function is called when the ADC channels acquisition is done.

* Interface assumptions: The ADC is initialized and running
*
* Return value: None
*
*****************************************************************************/
void App_AdcFifoCallback(void)
{
  #if (gPressureSensorEnable_d)
    uint16_t adcPressureData;
  #endif
  #if (gTempSensorEnable_d)
    uint16_t adcTempData;
  #endif
  

  /* stop ADC */
  Adc_TurnOff();
  #if (gAccelerometerEnable_d == TRUE)  
    gAccelerometerData[0] = App_ConvertAccelerometerData(((uint16_t)Adc_FifoData() & 0x0FFF));
    gAccelerometerData[1] = App_ConvertAccelerometerData(((uint16_t)Adc_FifoData() & 0x0FFF));
    gAccelerometerData[2] = App_ConvertAccelerometerData(((uint16_t)Adc_FifoData() & 0x0FFF));
  #endif
  #if (gPressureSensorEnable_d == TRUE)
    adcPressureData  = (uint16_t) (Adc_FifoData() & 0x0FFF);
  #endif
  #if (gTempSensorEnable_d == TRUE)
    adcTempData      = (uint16_t) (Adc_FifoData() & 0x0FFF);
  #endif
  #if (gBatteryLevelSensorEnable_d == TRUE)
    gBatteryLevelData    = App_ConvertBaterryLevel((uint16_t)Adc_FifoData() & 0x0FFF);      
  #endif
  #if (gPressureSensorEnable_d == TRUE)
    gPressureData        = App_ConvertPressureData(adcPressureData);
  #endif
  #if (gTempSensorEnable_d == TRUE)
    gTemperatureData     = App_ConvertTemperature(adcTempData);
  #endif

  if (SensorReadCallbackEvent != NULL)
    SensorReadCallbackEvent();
}

#endif /* (gTempSensorEnable_d | gAccelerometerEnable_d | gPressureSensorEnable_d | gBatteryLevelSensorEnable_d) */
/*****************************************************************************
* Converts to Celsius degrees
*
* Interface assumptions: None
*
* Return value: Temperature in Celsius degrees
*
*****************************************************************************/
#if (gTempSensorEnable_d == TRUE)
int8_t App_ConvertTemperature(uint16_t val)
{
  uint32_t milliVolts;
  
  milliVolts = (uint32_t) (1000 * (float) (val * mADCRef / 0x0FFF));
  return((int8_t)((milliVolts - mTempSensormilliVoltOffset_c) / mMilliVoltPerDegree_c) - mTemperatureCompensation_c);
}
#endif


/*****************************************************************************
* Returns the battery level
*
* Interface assumptions: None
*
* Return value: Battery level in Volts
*
*****************************************************************************/
#if (gBatteryLevelSensorEnable_d == TRUE)
uint8_t App_ConvertBaterryLevel(uint16_t val)
{
  mADCRef = (float) ((mVRef_c * (float)0x0FFF) / val);
  return (uint8_t) (mADCRef  * 10);
}
#endif

/*****************************************************************************
* Converts the accelerometer data (X, Y or Z)
*
* Interface assumptions: None
*
* Return value: the converted accelerometer data
*
*****************************************************************************/
#if (gAccelerometerEnable_d == TRUE)
uint8_t App_ConvertAccelerometerData(uint16_t val)
{
  return ((uint8_t) (val >> 4));
}
#endif

/*****************************************************************************
* Converts the accelerometer data (X, Y or Z)
*
* Interface assumptions: None
*
* Return value: the converted accelerometer data
*
*****************************************************************************/
#if (gPressureSensorEnable_d == TRUE)
uint8_t App_ConvertPressureData(uint8_t val)
{
  return (uint8_t) ((val - (0.04 * mADCRef)) / (0.09 * mADCRef));
}
#endif

