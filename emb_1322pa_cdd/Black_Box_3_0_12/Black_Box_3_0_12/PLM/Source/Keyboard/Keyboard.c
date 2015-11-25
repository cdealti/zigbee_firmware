/************************************************************************************
* Source file for keyboard driver.
*
* Copyright (c) 2007, Freescale, Inc. All rights reserved.
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
#include "EmbeddedTypes.h"
#include "TMR_Interface.h"
#include "KeyboardHAL.h"
#include "Keyboard.h"

#if (gSwitchModule_d == TRUE)

/******************************************************************************
*******************************************************************************
* Private macros
*******************************************************************************
******************************************************************************/

/* Keyboard State */
enum {
  mStateKeyIdle,        /* coming in for first time */
  mStateKeyDetected,    /* got a key, waiting to see if it's a long key */
  mStateKeyCheckRelease  /* got the long key, waiting for the release to go back to idle */
};

/* The Switch task has a single event*/
#define mEventSW_c 0x01
#define mNoKey_c 0xFF


/******************************************************************************
*******************************************************************************
* Private prototypes
*******************************************************************************
******************************************************************************/
static void KeyScan(uint8_t timerId);
static uint8_t KeySwitchPortGet(void);

/******************************************************************************
*******************************************************************************
* Private type definitions
*******************************************************************************
******************************************************************************/
/*None*/

/******************************************************************************
*******************************************************************************
* Private memory declarations
*******************************************************************************
******************************************************************************/
static uint8_t mKeyState = mStateKeyIdle;
static uint8_t mSwitch_SCAN;
static uint8_t mLongKeyCount;
#if (gJoystickSupported_d == TRUE) 
 static bool_t  mPollTimerState = TRUE;
#endif 

#if gJoystickSupported_d
static uint8_t mLongCenterKeyCount;
#endif /* gJoystickSupported_d */

static KBDFunction_t mpfKeyFunction;
static uint8_t mKeysStillPressed = 0;
tmrTimerID_t mKeyScanTimerID = gTmrInvalidTimerID_c;

/* No other code should ever post an event to the timer task. */
static tsTaskID_t mSwTaskID;


/******************************************************************************
*******************************************************************************
* Public functions
*******************************************************************************/

/******************************************************************************
* Initialize the keyboard handling. Works on on Freescale demo boards.
*******************************************************************************/
void KBD_Init
(
KBDFunction_t pfCallBackAdr /* IN: Pointer to callback function */
)
{
  /* timer is used to determine short or long key press */
  mKeyScanTimerID = TMR_AllocateTimer();
  
  /* where to send the data */
  mpfKeyFunction = pfCallBackAdr;
  /* Init all the Gpio pins for keyboard*/
  KbGpioInit();
  /*Init the CRM module for external interrupt capability*/
  KbCrmInit();
  /* Create a task for switches */
  mSwTaskID = TS_CreateTask(gTsSwitchTaskPriority_c, KBD_Task);
  
  /*Start the timer to detect the center key for joystick*/
#if gJoystickSupported_d
  TMR_StartIntervalTimer(mKeyScanTimerID, gKeyScanInterval_c, KeyScan);  
#endif /* gJoystickSupported_d */
  
}

/******************************************************************************
* Keyboard ISR. 
******************************************************************************/
void CrmKBDIsr(void)
{
  /*Disable all external interrupts and send the SW event to handle the key detection*/
  KbDisableAllIrq();
  TS_SendEvent(mSwTaskID, mEventSW_c);
  
}

/******************************************************************************
* Keyboard Enable/Disable key scanning timer. 
* Used for Low Power mode
*
* Parameters:
*   bool_t enable: TRUE  - The Key Scan is enabled
*                  FALSE - The Key Scan is disabled
******************************************************************************/
void KBD_EnableDisableKeyScan(bool_t enable)
{

 if(mKeyScanTimerID != gTmrInvalidTimerID_c)
 {  
  if(enable == TRUE) 
  {     
    TMR_EnableTimer(mKeyScanTimerID);   
  }
  else
  {
    if(CRM_WU_CNTL.extWuIEn  == gSWITCH_MASK_c)
    {
     /* disable scan timer here only if the KBD interrupts are all enabled.
      * With this timer disabled, the low power mode can be enetered. 
      * with the KBD interrupts disabled, the callback won't be called at the exist from low power
      */
     TMR_StopTimer(mKeyScanTimerID);
    }
  }
 }
 #if (gJoystickSupported_d == TRUE) 
  mPollTimerState = enable;
 #endif 
}

/******************************************************************************
*******************************************************************************
* Private functions
*******************************************************************************
******************************************************************************/

/******************************************************************************
* Keyboard task executed when an interrupt occur
******************************************************************************/

void KBD_Task(event_t events)
{
  
  mKeyState = mStateKeyIdle;
  /*Start the timer;*/
  TMR_StartIntervalTimer(mKeyScanTimerID, gKeyScanInterval_c, KeyScan);  
}


/******************************************************************************
* Called to get state of keyboard
******************************************************************************/
static uint8_t KeySwitchPortGet
(
void
)
{
  return (uint8_t)SwitchGet;
}

/******************************************************************************
* Called when a key is pressed. Determines when the key is up (lifted).
******************************************************************************/
static void KeyScan
(
uint8_t timerId
)
{
  uint8_t nowScan;
  uint8_t keyBase = 0;
  uint8_t changedScan;
  
  /* Detect the center key from joystick */
#if gJoystickSupported_d
  uint8_t centerKeyDetect;
  static uint8_t markLongPress = FALSE;
  /* Detect if a long or short press is done  */
  centerKeyDetect = mNoKey_c; 
  if( 0 == KbReadCenterKey() ) 
  {
    /*key is pressed*/
    if(mLongCenterKeyCount > gLongKeyIterations_c) 
    {   
      /*Is marked as being pressed?*/
      if(!markLongPress)
      {
        /*long keypress*/
        #if gMapJoystickCenterKeyOnSW2
         centerKeyDetect = gKBD_EventLongSW2_c;
        #else
         centerKeyDetect = gJoystick_CenterKeyLong_c;
        #endif
        /*mark the key as being press to send only an event to call back*/
        markLongPress =TRUE;
      }
    }
    else
    {
      mLongCenterKeyCount++;
    }
  }
  /*the key is not pressed*/
  else 
  { 
    /*It was a short key?*/ 
    if ((0 < mLongCenterKeyCount)&& 
        (mLongCenterKeyCount < gLongKeyIterations_c))
    {
      /* short keypress */
      #if gMapJoystickCenterKeyOnSW2
       centerKeyDetect = gKBD_EventSW2_c;
      #else
       centerKeyDetect = gJoystick_CenterKey_c;
      #endif
    }
    mLongCenterKeyCount = 0;  
    /*mark as not being pressed*/  
    markLongPress =FALSE;
  }
  /*Send the detected event*/  
  if( centerKeyDetect != mNoKey_c )
  {
    mpfKeyFunction(centerKeyDetect);
  }
#endif /* gJoystickSupported_d */
  
  /*Detect if a key is pressed or released;
  Every key that is pressed is marked (when is relesed is unmarked)*/
  switch(mKeyState) {
    
    /* got a fresh key */
  case mStateKeyIdle:
    /* Check if a fresh key is pressed and treat it (some keys can still be pressed) */
    mSwitch_SCAN = KeySwitchPortGet() & (~mKeysStillPressed);
    if ( mSwitch_SCAN )
    {   
      mKeyState = mStateKeyDetected;
      mLongKeyCount = 0;  /* assume no key */
    }
    else
    {
      /* No fresh key is pressed */
      /*check if one or more keys was released and unmark those (update mKeysStillPressed) */
      mKeyState = mStateKeyCheckRelease;
    }
    break;
    
    /* A fresh key was detected. Has the key been released or is it still being pressed? */
  case mStateKeyDetected:
    nowScan = KeySwitchPortGet()& (~mKeysStillPressed);
    if( nowScan & mSwitch_SCAN ) {
      mLongKeyCount++;
      
      if(mLongKeyCount >= gLongKeyIterations_c) {
        /*long keypress*/ 
        keyBase = gKBD_EventLongSW1_c;
      }
      
    }
    else {
      /* short keypress */
      keyBase = gKBD_EventSW1_c;
    }
    
    if(keyBase) {
      
      /* does it match a key?; mark it as being pressed */
      if(mSwitch_SCAN & gSWITCH1_MASK_c)
      {
        mKeysStillPressed |= mSwitch_SCAN;
        mpfKeyFunction(keyBase + 0);
      }
      if(mSwitch_SCAN & gSWITCH2_MASK_c)
      {
        mKeysStillPressed |= mSwitch_SCAN;
        mpfKeyFunction(keyBase + 1);
      }
      if(mSwitch_SCAN & gSWITCH3_MASK_c)
      {
        mKeysStillPressed |= mSwitch_SCAN;
        mpfKeyFunction(keyBase + 2);
      }
      if(mSwitch_SCAN & gSWITCH4_MASK_c)
      {
        mKeysStillPressed |= mSwitch_SCAN;
        mpfKeyFunction(keyBase + 3);
      }
      
      /* go and check if it was released  */
      mKeyState = mStateKeyCheckRelease;
    }
    
    break;
    
    /* check if one and more key was released;
    only the keys marked as released are treated for next time*/
  case mStateKeyCheckRelease:
    /*No key are pressed, so stop the timer*/ 
    if ( !mKeysStillPressed )
    {      
      /* Don't stop the timer for joystick to 
      detect the center key that doesn't has external interrupt capability*/            
#if (gJoystickSupported_d == FALSE)
      TMR_StopTimer(timerId);
#else
      if(mPollTimerState == FALSE)
      {
       /* Stop timer if it was requested */ 
       TMR_StopTimer(timerId);
      }      
#endif /* gJoystickSupported_d */
      
    }
    
    else
    { 
      
      /* Check if one or more keys was released and notify this; only keys that are not 
      still pressed are treated for next time*/
      nowScan = KeySwitchPortGet();
      changedScan = (nowScan & mKeysStillPressed)^mKeysStillPressed;
      if(changedScan) { 
        mKeysStillPressed &=~changedScan;
      }
    }
    /*Enable all interrupt but only the released (unmarked) keys will be treated*/
    KbEnableAllIrq();
    
    break;
  default:
    
    break;
  }
}

/******************************************************************************
*******************************************************************************
* Private Debug stuff
*******************************************************************************
******************************************************************************/
#else /* gSwitchModule_d  */

/* Dummy functions*/
void KBD_Init
(
KBDFunction_t pfCallBackAdr /* IN: Pointer to callback function */
)
{
  (void)pfCallBackAdr;
}

/* The CRM callback to handle the external interrupt;
Also is called when the MCU is waked up from sleep mode*/
void CrmKBDIsr(void){}

/* Main switch task. Process timer events in non-interrupt context. */
void KBD_Task(event_t events)
{
  (void)events;
}

#endif /* gSwitchModule_d */

