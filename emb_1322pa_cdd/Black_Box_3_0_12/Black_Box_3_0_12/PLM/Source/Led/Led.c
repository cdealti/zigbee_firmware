/************************************************************************************
* LedDriver.c
*
* This header file is for LED Driver Interface.
*
* Author(s):  B06061 
*
* Copyright (c) 2007, Freescale, Inc.  All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale.
*
Revision history:
   Date                     Author                                    Comments
   ----------               ----------------------                    -------
   12.04.2007                B06061                                 Modified
************************************************************************************/
#include "TMR_Interface.h"
#include "Led.h"

#if (gLEDSupported_d)

/******************************************************************************
*******************************************************************************
* Public Macros
*******************************************************************************
******************************************************************************/

/*None*/

/******************************************************************************
*******************************************************************************
* Private prototypes
*******************************************************************************
******************************************************************************/
static void LED_FlashTimeout(uint8_t timerId);
#if gLEDBlipEnabled_d
static void LED_DecrementBlip(void);
#endif

/******************************************************************************
*******************************************************************************
* Private type definitions
*******************************************************************************
******************************************************************************/

/******************************************************************************
*******************************************************************************
* Private Memory Declarations
*******************************************************************************
******************************************************************************/

static bool_t mfLedInSerialMode;      /* doing serial lights */
static uint8_t mLedFlashingLEDs = 0;  /* flashing LEDs */
static tmrTimerID_t mLEDTimerID = gTmrInvalidTimerID_c;
#if gLEDBlipEnabled_d
static uint8_t mLedBlips[4];          /* enough for 4 LEDs to blip once */
#endif

/******************************************************************************
*******************************************************************************
* Public functions
*******************************************************************************
******************************************************************************/

/******************************************************************************
* LED_Init
*
* Initialize the LED system.
*******************************************************************************/
void LED_Init(void)
{
  /* Create and init all LEDs */
  InitLed();
  
  /* allocate a timer for use in flashing LEDs */
  mLEDTimerID = TMR_AllocateTimer();
  TurnOffLeds();
}


/******************************************************************************
* Set_Pins
*
* Set a PIN with Direction Out, this PIN can be used as a LED.
*******************************************************************************/
void Set_Pins( LED_t LEDNr )
{
  if (LEDNr & LED1){
    Gpio_SetPinFunction(LED1_PIN, gGpioNormalMode_c);
    Gpio_SetPinReadSource(LED1_PIN, gGpioPinReadReg_c);
    Gpio_SetPinDir(LED1_PIN, gGpioDirOut_c);
    Gpio_SetPinData(LED1_PIN, LED_RESET);
  }
  if (LEDNr & LED2){
    Gpio_SetPinFunction(LED2_PIN, gGpioNormalMode_c);
    Gpio_SetPinReadSource(LED2_PIN, gGpioPinReadReg_c);
    Gpio_SetPinDir(LED2_PIN, gGpioDirOut_c);
    Gpio_SetPinData(LED2_PIN, LED_RESET);
  }
  if (LEDNr & LED3){
    Gpio_SetPinFunction(LED3_PIN, gGpioNormalMode_c);
    Gpio_SetPinReadSource(LED3_PIN, gGpioPinReadReg_c);
    Gpio_SetPinDir(LED3_PIN, gGpioDirOut_c);
    Gpio_SetPinData(LED3_PIN, LED_RESET);
  }
  if (LEDNr & LED4){
    Gpio_SetPinFunction(LED4_PIN, gGpioNormalMode_c);
    Gpio_SetPinReadSource(LED4_PIN, gGpioPinReadReg_c);
    Gpio_SetPinDir(LED4_PIN, gGpioDirOut_c);
    Gpio_SetPinData(LED4_PIN, LED_RESET);
  }
}

/******************************************************************************
* LED_GetLed
*
* Get LED state.
*
* gpioPinState      LED_SET signal        Logical LED State (ON/OFF)
*      0                  0                        1
*      0                  1                        0
*      1                  0                        0
*      1                  1                        1
* -----------------------------------------------------
* LED State (ON/OFF) = !(gpioPinState ^ LED_SET)
*******************************************************************************/
LedState_t LED_GetLed( LED_t LEDNr)                      
{
  GpioPinState_t gpioPinState;
  LED_t LEDpinsState = 0;
  int8_t ReturnSingleLED = 0;
  
  if ( (LEDNr == LED1) || (LEDNr == LED2) || (LEDNr == LED3) || (LEDNr == LED4) )
    ReturnSingleLED = 1;
  else
    ReturnSingleLED = 0;
  
  if (LEDNr & LED1)
  {
    Gpio_GetPinData(LED1_PIN, &gpioPinState);
    gpioPinState = (GpioPinState_t)(1 - (gpioPinState ^ LED_SET));
    if (ReturnSingleLED)
      return gpioPinState;
  }
  LEDpinsState |= gpioPinState << 0;
  
  if (LEDNr & LED2)
  {
    Gpio_GetPinData(LED2_PIN, &gpioPinState);
    gpioPinState = (GpioPinState_t)(1 - (gpioPinState ^ LED_SET));
    if (ReturnSingleLED)
      return gpioPinState;
  }
  LEDpinsState |= gpioPinState << 1;
  
  if (LEDNr & LED3)
  {
    Gpio_GetPinData(LED3_PIN, &gpioPinState);
    gpioPinState = (GpioPinState_t)(1 - (gpioPinState ^ LED_SET));
    if (ReturnSingleLED)
      return gpioPinState;
  }
  LEDpinsState |= gpioPinState << 2;
  
  if (LEDNr & LED4)
  {
    Gpio_GetPinData(LED4_PIN, &gpioPinState);
    gpioPinState = (GpioPinState_t)(1 - (gpioPinState ^ LED_SET));
    if (ReturnSingleLED)
      return gpioPinState;
  }
  LEDpinsState |= gpioPinState << 3;
  
  return LEDpinsState;
  
}

/******************************************************************************
* LED_ToggleLed
*
* Toggles on or more LEDs. Doesn't check/affect flashing states.
*******************************************************************************/
void LED_ToggleLed(LED_t LEDNr)
{
  if (LEDNr & LED1){    
    Led1Toggle();
  }
  if (LEDNr & LED2){    
    Led2Toggle();
  }
  if (LEDNr & LED3){    
    Led3Toggle();
  }
  if (LEDNr & LED4){    
    Led4Toggle();
  }
}

/******************************************************************************
* LED_TurnOffLed
*
* Turns off on or more LEDs. Doesn't check/affect flashing states.
*******************************************************************************/
void LED_TurnOffLed(LED_t LEDNr)
{
  if (LEDNr & LED1)
    Led1Off();
  if (LEDNr & LED2)
    Led2Off();
  if (LEDNr & LED3)
    Led3Off();
  if (LEDNr & LED4)
    Led4Off();
}

/******************************************************************************
* LED_TurnOnLed
*
* Turn on one or more LEDs. Doesn't check/affect flashing states.
*******************************************************************************/
void LED_TurnOnLed(LED_t LEDNr)
{
  if (LEDNr & LED1)
    Led1On();
  if (LEDNr & LED2)
    Led2On();
  if (LEDNr & LED3)
    Led3On();
  if (LEDNr & LED4)
    Led4On();
}

/******************************************************************************
* LED_TurnOffAllLeds
*
* Turn off all LEDs. Does not affect flashing state.
*******************************************************************************/
void LED_TurnOffAllLeds(void)
{
  LED_TurnOffLed(LED_ALL);
}

/******************************************************************************
* LED_TurnOnAllLeds
*
* Turn on all LEDs. Does not affect flashing state.
*******************************************************************************/
void LED_TurnOnAllLeds(void)
{
  LED_TurnOnLed(LED_ALL);
}

/******************************************************************************
* LED_StopFlashingAllLeds
*
* Turn off all LEDs and stop flashing.
*******************************************************************************/
void LED_StopFlashingAllLeds(void)
{
  LED_SetLed(LED_ALL, gLedOff_c);
}

/******************************************************************************
* LED_StartFlash
*
* Start one or more LEDs flashing
*******************************************************************************/
void LED_StartFlash
 (
    LED_t LEDNr /* IN: LED Number (may be an OR of the list) */
 )
{
  /* indicate which leds are flashing */
  mLedFlashingLEDs |= LEDNr;

  /* start the timer */
  TMR_StartIntervalTimer(mLEDTimerID, mLEDInterval_c, LED_FlashTimeout);
}

#if gLEDBlipEnabled_d
/******************************************************************************
* LED_StartBlip
*
* Set up for blinking one or more LEDs once. 
******************************************************************************/
void LED_StartBlip ( LED_t LEDNr )
{
  uint8_t iLedIndex;

  /* set up for blinking one or more LEDs once */
  for(iLedIndex = 0; iLedIndex < 4; ++iLedIndex) 
  {
    if(LEDNr & (1 << iLedIndex))
     mLedBlips[iLedIndex] = 2;   /* blink on, then back off */
  }

  /* start flashing */
  LED_StartFlash(LEDNr);
}
#endif

/******************************************************************************
* LED_StopFlash
*
* Stop an LED from flashing.
******************************************************************************/
void LED_StopFlash
  (
  LED_t LEDNr /* IN: LED Number. */
  )
{
  /* leave stopped LEDs in the off state */
  LED_TurnOffLed(LEDNr);

  /* stop flashing on one or more LEDs */
  mLedFlashingLEDs &= (~LEDNr);

  /* if ALL LEDs have stopped flashing, then stop timer */
  if(!mLedFlashingLEDs)
    TMR_StopTimer(mLEDTimerID);
}

/******************************************************************************
* LED_StartSerialFlash
*
* Serial flashing lights
******************************************************************************/
void LED_StartSerialFlash(void)
{
  /* indicate going into flashing mode (must be done first) */
  LED_StartFlash(0);

  /* set up for serial lights */
  LED_TurnOffAllLeds();
  Led1On();
  mLedFlashingLEDs = LED1 | LED2; /* toggle these to on first flash */

  /* indicate in serial flash mode */
  mfLedInSerialMode = TRUE;
}

/******************************************************************************
* LED_DecrementBlink
*
* Decrements the blink count.
******************************************************************************/
void LED_SetHex
  (
  uint8_t hexValue
  )
{
  LED_SetLed(LED_ALL, gLedOff_c);
  LED_SetLed(hexValue, gLedOn_c);
}

/******************************************************************************
* LED_SetLed
*
* This fuction can set the Led state in: On, Off, Toggle, Flashing or StopFlash
*******************************************************************************/
void LED_SetLed
  (
  LED_t LEDNr,    /* IN: mask of LEDs to change */
  LedState_t state
  )
{
  /* turning off flashing same as off state */
  if(state == gLedStopFlashing_c)
    state = gLedOff_c;

  /* turn off serial lights if in serial mode */
  LED_ExitSerialFlash();

  /* flash LED */
  if(state == gLedFlashing_c)
    LED_StartFlash(LEDNr);

#if gLEDBlipEnabled_d
  /* blink LEDs */
  else if(state == gLedBlip_c)
    LED_StartBlip(LEDNr);
#endif

  /* On, Off or Toggle Led*/
  else {

    /* on, off or toggle will stop flashing on the LED. Also exits serial mode */
    if(mfLedInSerialMode || (mLedFlashingLEDs & LEDNr))
      LED_StopFlash(LEDNr);

    /* Select the operation to be done on the port */
    if(state == gLedOn_c)
      LED_TurnOnLed(LEDNr);
    if(state == gLedOff_c)
      LED_TurnOffLed(LEDNr);
    if(state == gLedToggle_c)
      LED_ToggleLed(LEDNr);
  }
}


/******************************************************************************
*******************************************************************************
* Private functions
*******************************************************************************
******************************************************************************/

/******************************************************************************
* LED_ExitSerialFlash
*
* Stop serial flashing mode. Leaves all LEDs off. Only exits if actually in 
* serial mode.
*******************************************************************************/
void LED_ExitSerialFlash(void)
{
  if(mfLedInSerialMode) 
  {
    mLedFlashingLEDs = 0;   /* no LEDs are currently flashing */
    LED_TurnOffAllLeds();
    TMR_StopTimer(mLEDTimerID);
    mfLedInSerialMode = FALSE;
  }
}

#if gLEDBlipEnabled_d
/******************************************************************************
* LED_DecrementBlip
*
* Decrements the blink count.
******************************************************************************/
static void LED_DecrementBlip(void)
{
  uint8_t iLedIndex;

  for(iLedIndex = 0; iLedIndex < 4; ++iLedIndex) 
  {
    if(mLedBlips[iLedIndex]) 
    {
      --mLedBlips[iLedIndex];
      if(!mLedBlips[iLedIndex])
        mLedFlashingLEDs &= ~(1 << iLedIndex);
    }
  }

  /* done, stop the timer */
  if(!mLedFlashingLEDs)
    TMR_StopTimer(mLEDTimerID);
}
#endif

/******************************************************************************
* LED_FlashTimeout
*
* Flash the selected LED(s).
******************************************************************************/
static void LED_FlashTimeout
  (
  uint8_t timerId /* IN: TimerID. */
  )
{
  (void)timerId;  /* prevent compiler warning */

  if(mLedFlashingLEDs & LED1)
    Led1Toggle();
  if(mLedFlashingLEDs & LED2)
    Led2Toggle();
  if(mLedFlashingLEDs & LED3)
    Led3Toggle();
  if(mLedFlashingLEDs & LED4)
    Led4Toggle();

#if gLEDBlipEnabled_d
 /* decrement blips */
  LED_DecrementBlip();
#endif

 /* if serial lights, move on to next light */
  if(mfLedInSerialMode)
  {
    mLedFlashingLEDs = mLedFlashingLEDs << 1;
    if(mLedFlashingLEDs & (LED4 << 1))  /* wrap around */
    {
      mLedFlashingLEDs &= LED_ALL;
      mLedFlashingLEDs |= LED1;
    }
  }
}


/******************************************************************************
*******************************************************************************
* Unit Testing
*******************************************************************************
******************************************************************************/

/* None */

#endif /* gLEDSupported_d */

