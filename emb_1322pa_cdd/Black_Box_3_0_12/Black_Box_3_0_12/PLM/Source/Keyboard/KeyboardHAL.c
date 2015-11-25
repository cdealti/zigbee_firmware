/************************************************************************************
* Source file for keyboard driver.
*
* Copyright (c) 2006, Freescale, Inc. All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
* This keyboard driver has the concept of short keys and long keys. A long key is
* press and hold on a key. 
*
* The keyboard handling logic can only understand one key at a time (pressing
* two keys at once will indicate only the first key).
*
******************************************************************************/
#include "KeyboardHAL.h"
#include "Keyboard.h"


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
* Private memory declarations
*******************************************************************************
******************************************************************************/


/******************************************************************************
*******************************************************************************
* Public functions
*******************************************************************************/
#if gSwitchModule_d  
/*Init the GPIO as input, normal functin mode, pull up enable and read from pads */
void KbGpioInit(void)
{
  /* Configure the the Switch Pins */
  
  /* Set as input*/
  (void)Gpio_SetPortDir(gKbGpioPort_c, gKbGpioAllZeros_c, gKbGpioMask_c);
  /* Pullup enable and select pullup*/
  (void)Gpio_WrPortSetting(gKbGpioPort_c, gGpioPullUpEnAttr_c, gKbGpioAllOnes_c, gKbGpioMask_c);
  (void)Gpio_WrPortSetting(gKbGpioPort_c, gGpioPullUpSelAttr_c, gKbGpioAllOnes_c, gKbGpioMask_c);
  /*Read from pads*/
  (void)Gpio_WrPortSetting(gKbGpioPort_c, gGpioInputDataSelAttr_c, gKbGpioAllZeros_c, gKbGpioMask_c); 
  
  Gpio_SetPinDir(gCenterKey_c, gGpioDirIn_c);
  Gpio_EnPinPullup(gCenterKey_c, TRUE);
  Gpio_SelectPinPullup(gCenterKey_c, gGpioPinPullup_c);
  Gpio_SetPinReadSource(gCenterKey_c, gGpioPinReadPad_c);
}

/* Enable ext. interrupt,  low level sense, wake up polarity to be negative edge,
enable interrupts for all keys and register the Crm callback to handle interrupts */
void KbCrmInit(void)
{
  crmWuCtrl_t WuCtrl;
  
  /* Enable ext. interrupt,  low level sense, wake up polarity to be negative edge, enable interrupts for all keys*/
  WuCtrl.wuSource = gExtWu_c;
  WuCtrl.KBI_WU_EN = gKbiWuEN_c;
  WuCtrl.KBI_WU_EVENT = gKbiWuEVENT_c;
  WuCtrl.KBI_WU_POL = gKbiWuPOL_c;
  WuCtrl.KBI_INT_EN = gKbiIntEN_c;
  CRM_WuCntl(&WuCtrl);
  
  /*Register call back*/
  CRM_RegisterISR(gCrmKB4WuEvent_c,CrmKBDIsr);
  CRM_RegisterISR(gCrmKB5WuEvent_c,CrmKBDIsr);
  CRM_RegisterISR(gCrmKB6WuEvent_c,CrmKBDIsr);
  CRM_RegisterISR(gCrmKB7WuEvent_c,CrmKBDIsr);
}


/* Read the port and shift it with 26 for better use */
uint8_t KbReadPins(void)
{
  uint32_t portData;
  (void)Gpio_GetPortData(gKbGpioPort_c, &portData);
  return (uint8_t)(portData >> 26);
}

/*Read the joystick's center key pin*/
uint8_t KbReadCenterKey(void) 
{
  GpioPinState_t pioPinState;
  
  Gpio_GetPinData (gCenterKey_c,&pioPinState);
  
  return pioPinState;
  
}
#endif

/******************************************************************************
*******************************************************************************
* Private functions
*******************************************************************************
******************************************************************************/
