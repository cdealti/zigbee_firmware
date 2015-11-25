/************************************************************************************
* Kaibab Buzzer routines
*
* Author(s):
*   
*
* (c) Copyright 2007, Freescale, Inc.  All rights reserved.
*
* Freescale Confidential Proprietary
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale.
*
* Last Inspected:
* Last Tested:
************************************************************************************/
#include "EmbeddedTypes.h"
#include "AppToPlatformConfig.h"
#include "Sound.h"
#include "Timer.h"
#include "I2C_Interface.h"
#include "GPIO_Interface.h"
#if gHaveBuzzer_c
/************************************************************************************
*************************************************************************************
* Private macros
*************************************************************************************
************************************************************************************/
#define mBeepFreqinKHz_c  4
#define mToggleCompareValue_c  (SYSTEM_CLOCK/(mBeepFreqinKHz_c * 2))
#define  mSLAVE_DEV_ADDRESS_c  0x50
#define  mVREG_CMD_c           0x11
#define  gGpioBeepTimer_c     ((GpioPin_t)((uint8_t)gGpioPin8_c + (uint8_t)gBeepTimer_c))
/************************************************************************************
*************************************************************************************
* Private type definitions

*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Private prototypes
*************************************************************************************
************************************************************************************/

static void BuzzerTimerCallBack(TmrNumber_t tmrNumber);
static void Llc_VolumeInit(void);
static void Llc_VolumeSet(uint8_t volume);
/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
************************************************************************************/
uint32_t mMsCounter;
uint32_t mBeepDuration;

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/

bool_t BuzzerInit(void)
{
  TmrConfig_t tmrConfig;
  TmrStatusCtrl_t tmrStatusCtrl;
  TmrComparatorStatusCtrl_t tmrComparatorStatusCtrl;
  /* Enable hw timer 1 */
  TmrEnable(gBeepTimer_c);
  /* Don't stat the timer yet */ 
  if (gTmrErrNoError_c != TmrSetMode(gBeepTimer_c, gTmrNoOperation_c)) /*set timer mode no operation*/
  {
    return FALSE;
  }
  /* Register the callback executed when an interrupt occur */
  if(gTmrErrNoError_c != TmrSetCallbackFunction(gBeepTimer_c, gTmrComp1Event_c, BuzzerTimerCallBack))
  {
    return FALSE;
  }
  tmrStatusCtrl.uintValue = 0x0000;
  tmrStatusCtrl.bitFields.OEN = 1;    //output enable
  if (gTmrErrNoError_c != TmrSetStatusControl(gBeepTimer_c, &tmrStatusCtrl))
  {
    return FALSE;
  }  
  tmrComparatorStatusCtrl.uintValue = 0x0000;
  tmrComparatorStatusCtrl.bitFields.TCF1EN = TRUE; /* Enable Compare 1 interrupt */
  if (gTmrErrNoError_c != TmrSetCompStatusControl(gBeepTimer_c, &tmrComparatorStatusCtrl))
  {
    return FALSE;
  }
  tmrConfig.tmrOutputMode = gTmrToggleOF_c; 
  tmrConfig.tmrCoInit = FALSE;  /*co-chanel counter/timers can not force a re-initialization of this counter/timer*/
  tmrConfig.tmrCntDir = FALSE;  /*count-up*/
  tmrConfig.tmrCntLen = TRUE;  /*count until compare*/
  tmrConfig.tmrCntOnce = FALSE;   /*count repeatedly*/
  tmrConfig.tmrSecondaryCntSrc = gTmrSecondaryCnt0Input_c;    /*secondary count source not needed*/
  tmrConfig.tmrPrimaryCntSrc = gTmrPrimaryClkDiv1_c;    /*primary count source is IP BUS clock divide by 1 prescaler*/
  if (gTmrErrNoError_c != TmrSetConfig(gBeepTimer_c, &tmrConfig))  /*set timer configuration */
  {
    return FALSE;
  }
  /* Config timer to raise interrupts each 0.1 ms */
  SetComp1Val(gBeepTimer_c, mToggleCompareValue_c);
  /* Config timer to start from 0 after compare event */
  SetLoadVal(gBeepTimer_c, 0);
  I2c_Init();
  Gpio_SetPinFunction(gGpioBeepTimer_c, gGpioAlternate1Mode_c); 
  Gpio_SetPinFunction(gGpioPin12_c, gGpioAlternate1Mode_c);
  Gpio_SetPinFunction(gGpioPin13_c, gGpioAlternate1Mode_c);
  return TRUE;
}

/***********************************************************************************/

bool_t BuzzerBeep (void) 
{
  if( gBeepDurationInMs_c == 0 )
    return FALSE;
  if (gTmrErrNoError_c != TmrSetMode(gBeepTimer_c, gTmrNoOperation_c)) /*set timer mode no operation*/
  {
    return FALSE;
  }
  mMsCounter = 0;
  mBeepDuration = gBeepDurationInMs_c;
  SetCntrVal(gBeepTimer_c, 0) ; /*clear counter*/
  Llc_VolumeInit();
  Llc_VolumeSet(gBeepVolumeInPrc_c);   
  if (gTmrErrNoError_c != TmrSetMode(gBeepTimer_c, gTmrCntRiseEdgPriSrc_c))
  {
    return FALSE;
  }
  return TRUE;
}

/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/

static void BuzzerTimerCallBack(TmrNumber_t tmrNumber)
{
  mMsCounter++;
  if(mMsCounter >= mBeepFreqinKHz_c<<1)
  {
    mMsCounter = 0;
    mBeepDuration --;
    if(mBeepDuration == 0)
    {
      TmrSetMode(gBeepTimer_c, gTmrNoOperation_c);    //stop timer  
    }
  }
}

/***********************************************************************************/

static void Llc_VolumeInit(void)
{
  I2cConfig_t i2cConfig;
  I2c_Enable();
  i2cConfig.i2cBroadcastEn = FALSE;
  i2cConfig.freqDivider   = 0x20;
  i2cConfig.saplingRate   = 0x01;
  i2cConfig.slaveAddress   = 0x00;
  i2cConfig.i2cInterruptEn = FALSE; 
  I2c_SetConfig(&i2cConfig);
}

/***********************************************************************************/

static void Llc_VolumeSet(uint8_t volume)
{
  uint8_t buffer[2];
  if(volume > (uint8_t)100)
  {
    volume = 100; /* Limit volume to 100% */
  }
  /* Scale down the volume value to 0 - 31 range */
  volume = (volume * 31) / 100;
  buffer[0] = mVREG_CMD_c;
  buffer[1] = (volume & 0xFF) << 3;
  I2c_SendData(mSLAVE_DEV_ADDRESS_c, &buffer[0], 2, gI2cMstrReleaseBus_c);
}

#endif 
 
 
