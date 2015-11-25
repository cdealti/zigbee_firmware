/************************************************************************************
* This header file is for Keyboard Driver Interface.
*
* Copyright (c) 2006, Freescale, Inc.  All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
*****************************************************************************/

#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_
#include "EmbeddedTypes.h"
#include "TS_Interface.h"
#include "Keyboard.h"

/* Timer task priority must be the highest priority task. 0xFF 
     is reserved is reserved as an invalid task priority. */
/* The switch task has a lower priority */
#define gTsSwitchTaskPriority_c  0x1F

/* Enabled keyboard support (or not) */
#ifndef gKeyBoardSupported_d
  #define gKeyBoardSupported_d       TRUE
#endif /* gKeyBoardSupported_d */

/* Enabled joystick  support (or not) */
#ifndef gJoystickSupported_d
  #define gJoystickSupported_d       FALSE
#endif /* gJoystickSupported_d */

#if gTargetMC1322xLPB
 #define gMapJoystickCenterKeyOnSW2   TRUE
#else
 #define gMapJoystickCenterKeyOnSW2   FALSE
#endif

#if gKeyBoardSupported_d || gJoystickSupported_d
  #define gSwitchModule_d TRUE
#else
  #define gSwitchModule_d FALSE
#endif /* gKeyBoardSupported_d || gJoystickSupported_d */

/* Constant for a keypress. A short key will be returned after this # of millisecond if pressed */
/* make sure this constant is long enough for debounce time (default 50 milliseconds) */
#define gKeyScanInterval_c 50

/* A long key is if the key is held down for at least this many of the keyScanInterval */
/* default 20*50 = 1000 milliseconds or 1 second for long key */
#define gLongKeyIterations_c 20



/*****************************************************************************
******************************************************************************
* Public macros
******************************************************************************
*****************************************************************************/

/* Each key delivered to the callback function is of this type (see the following enum) */
typedef uint8_t key_event_t;

/* Which key code is given to the callback function */
enum {
  gKBD_EventSW1_c = 1,
  gKBD_EventSW2_c,
  gKBD_EventSW3_c,
  gKBD_EventSW4_c,
  gKBD_EventLongSW1_c,
  gKBD_EventLongSW2_c,
  gKBD_EventLongSW3_c,
  gKBD_EventLongSW4_c
};

/* Which key code is given to the callback function if the joystick 
     functionality is used*/
enum {
  gJoystick_Event1_c = 1,
  gJoystick_Event2_c,
  gJoystick_Event3_c,
  gJoystick_Event4_c,
  gJoystick_Event1Long_c,
  gJoystick_Event2Long_c,
  gJoystick_Event3Long_c,
  gJoystick_Event4Long_c,
  gJoystick_CenterKey_c,
  gJoystick_CenterKeyLong_c
};


/* Callback function prototype */
typedef void (*KBDFunction_t) ( uint8_t events );

/*****************************************************************************
******************************************************************************
* Public type definitions
******************************************************************************
*****************************************************************************/


/*****************************************************************************
******************************************************************************
* Public prototypes
******************************************************************************
*****************************************************************************/


/******************************************************************************
* This function generates a callback function
*
* Interface assumptions:
*
* Return value:
* None
*
******************************************************************************/
extern void KBD_Init
  (
  KBDFunction_t pfCallBackAdr /* IN: Pointer to callback function */
  );

/* The CRM callback to handle the external interrupt;
     Also is called when the MCU is waked up from sleep mode*/
extern void CrmKBDIsr(void);

/******************************************************************************
* Keyboard Enable/Disable key scanning timer. 
* Used for Low Power mode
*
* Parameters:
*   bool_t enable: TRUE  - The Key Scan is enabled
*                  FALSE - The Key Scan is disabled
******************************************************************************/
extern void KBD_EnableDisableKeyScan(bool_t enable);

/* Main switch task. Process timer events in non-interrupt context. */
extern void KBD_Task(event_t events);
#endif /* _KEYBOARD_H_ */
