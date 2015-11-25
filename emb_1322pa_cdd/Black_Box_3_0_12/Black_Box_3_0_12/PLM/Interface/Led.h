/************************************************************************************
* Led.h
*
* This header file is for LED Driver Interface.
*
* Author(s):  B06061 
*
* Copyright (c) 2007, Freescale, Inc.  All rights reserved.
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale.
*
Revision history:
   Date                     Author                        Comments
   ----------               ----------------------      -------
   12.04.2007                  B06061                      Modified
************************************************************************************/
#ifndef _LED_H_
#define _LED_H_

#include "AppToPlatformConfig.h"
#include "EmbeddedTypes.h"
#include "GPIO_Interface.h"

/******************************************************************************
*******************************************************************************
* Public Macros
*******************************************************************************
******************************************************************************/


/* are LEDs supported */
#define gLEDSupported_d     FALSE

/* is blip support in LED */
#ifndef gLEDBlipEnabled_d
 #define gLEDBlipEnabled_d  FALSE
#endif


/* possible LED values for LED_SetLed() */
typedef uint8_t LED_t;

#define LED1          0x01  /* LED1 is defined as bit nr 1 */
#define LED2          0x02  /* LED2 is defined as bit nr 2 */
#define LED3          0x04  /* LED3 is defined as bit nr 3 */
#define LED4          0x08  /* LED4 is defined as bit nr 4 */
#define LED_ALL       0x0F  /* turn on/off all LEDs */

#define LED1_FLASH    0x10  /* LED1_Flashing is defined as bit nr 4 + 1 */
#define LED2_FLASH    0x20  /* LED2_Flashing is defined as bit nr 4 + 2 */
#define LED3_FLASH    0x40  /* LED3_Flashing is defined as bit nr 4 + 3 */
#define LED4_FLASH    0x80  /* LED4_Flashing is defined as bit nr 4 + 4 */
#define LED_FLASH_ALL 0xF0  /* flash all LEDs */


/* note: all LEDs can operate independantly */
typedef uint8_t LedState_t; /* possible LED int states for LEDs */

enum /* possible LED states for LEDs */
{
  gLedFlashing_c,         /* flash at a fixed rate */
  gLedStopFlashing_c,     /* same as gLedOff_c */
  gLedBlip_c,             /* just like flashing, but blinks only once */
  gLedOn_c,               /* on solid */
  gLedOff_c,              /* off solid */
  gLedToggle_c            /* toggle state */
};



#if (gLEDSupported_d)                     /* If LED's are suported by the target then we can use the LED driver. If LED's are not supported, blank function will be used instead.*/
  #include "Led_MC1322x.h"


/* Define LED pin mapping for driver*/
#define LED1_PIN LED1_PIN_TARGET        /* LED1_PIN is defined as LED1 for the actual target that is in use */
#define LED2_PIN LED2_PIN_TARGET        /* LED2_PIN is defined as LED2 for the actual target that is in use */
#define LED3_PIN LED3_PIN_TARGET        /* LED3_PIN is defined as LED3 for the actual target that is in use */
#define LED4_PIN LED4_PIN_TARGET        /* LED4_PIN is defined as LED4 for the actual target that is in use */

/******************************************************************************
*******************************************************************************
* Public Macros
*******************************************************************************
******************************************************************************/

/* Interval time for toggle LED which is used for flashing LED (0 - 262140ms) */
#ifndef mLEDInterval_c
  #define mLEDInterval_c  100           /* The interval that is uset for flashing LED's */
#endif

/*   Macros with functionality to LED   */
#define Led1On()            (void)Gpio_SetPinData(LED1_PIN, LED_SET)     /* Set      LED1 to ON  state */
#define Led1Off()           (void)Gpio_SetPinData(LED1_PIN, LED_RESET)   /* Set      LED1 to OFF state */
#define Led1Toggle()        (void)Gpio_TogglePin (LED1_PIN)              /* Toggle   LED1 state */
#define GetLed1()           LED_GetLed( LED1 )                           /* Get      LED1 state */

#define Led2On()            (void)Gpio_SetPinData(LED2_PIN, LED_SET)     /* Set      LED2 to ON  state */
#define Led2Off()           (void)Gpio_SetPinData(LED2_PIN, LED_RESET)   /* Set      LED2 to OFF state */
#define Led2Toggle()        (void)Gpio_TogglePin (LED2_PIN)              /* Toggle   LED2 state */
#define GetLed2()           LED_GetLed( LED2 )                           /* Get      LED2 state */

#define Led3On()            (void)Gpio_SetPinData(LED3_PIN, LED_SET)     /* Set      LED3 to ON  state */
#define Led3Off()           (void)Gpio_SetPinData(LED3_PIN, LED_RESET)   /* Set      LED3 to OFF state */
#define Led3Toggle()        (void)Gpio_TogglePin (LED3_PIN)              /* Toggle   LED3 state */
#define GetLed3()           LED_GetLed( LED3 )                           /* Get      LED3 state */

#define Led4On()            (void)Gpio_SetPinData(LED4_PIN, LED_SET)     /* Set      LED4 to ON  state */
#define Led4Off()           (void)Gpio_SetPinData(LED4_PIN, LED_RESET)   /* Set      LED4 to OFF state */
#define Led4Toggle()        (void)Gpio_TogglePin (LED4_PIN)              /* Toggle   LED4 state */
#define GetLed4()           LED_GetLed( LED4 )                           /* Get      LED4 state */

/* Turn off LEDs */
#define TurnOffLeds()       LED_TurnOffAllLeds()                    /* Turn OFF All LEDs */
/* Turn on LEDs */
#define TurnOnLeds()        LED_TurnOnAllLeds()                     /* Turn ON  All LEDs */

/* serial flashing */
#define SerialFlashing()     LED_StartSerialFlash()                 /* Start Serial flashing   */
#define StopSerialFlashing() LED_ExitSerialFlash()                  /* Stop  Serial Flashing for LEDs  and close the timers*/

/* Flashing LED */
#define Led1Flashing()       LED_StartFlash(LED1)                  /* Start Flashing LED1 */
#define Led2Flashing()       LED_StartFlash(LED2)                  /* Start Flashing LED2 */
#define Led3Flashing()       LED_StartFlash(LED3)                  /* Start Flashing LED3 */
#define Led4Flashing()       LED_StartFlash(LED4)                  /* Start Flashing LED4 */
                        
#define StopLed1Flashing()   LED_StopFlash(LED1)                   /* Stop  Flashing LED1 */
#define StopLed2Flashing()   LED_StopFlash(LED2)                   /* Stop  Flashing LED2 */
#define StopLed3Flashing()   LED_StopFlash(LED3)                   /* Stop  Flashing LED3 */
#define StopLed4Flashing()   LED_StopFlash(LED4)                   /* Stop  Flashing LED4 */

#define InitLed()            Set_Pins(LED_ALL);                    /*Set settings and state for all LEDs */

/******************************************************************************
*******************************************************************************
* Public prototypes
*******************************************************************************
******************************************************************************/

void LED_Init(void);                                      /* Initializing LEDs state */

void LED_TurnOffLed(LED_t LEDNr);                         /* Turn OFF LED state with number LEDNr */
void LED_TurnOnLed(LED_t LEDNr);                          /* Turn ON  LED state with number LEDNr */
void LED_ToggleLed(LED_t LEDNr);                          /* Toggle   LED state with number LEDNr */

void LED_StartFlash(LED_t LEDNr);                         /* Start Flashing LED with number LEDNr */
void LED_StopFlash (LED_t LEDNr);                         /* Stop  Flashing LED with number LEDNr */

void LED_StartSerialFlash(void);                          /* Start Serial Flashing for All LEDs */
void LED_StopFlashingAllLeds(void);                       /* Stop Serial Flashing for All LEDs */
void LED_ExitSerialFlash(void);                           /* Stop Serial Flashing for LEDs  and close the timers*/

void LED_TurnOffAllLeds(void);                            /* Turn OFF state for All LEDs */
void LED_TurnOnAllLeds(void);                             /* Turn ON  state for All LEDs */

LedState_t  LED_GetLed( LED_t LEDNr);                     /* Get State of LEDNr*/
void    LED_SetLed( LED_t LEDNr, LedState_t  state);      /* Set LEDNr to "state" state */
void    LED_SetHex( uint8_t hexValue);                    /* Set LEDs state to show a binary Value */
void    Set_Pins  ( LED_t LEDNr);                         /* Set initial state of LED */

/* LEDs not supported */
#else

#define Led1On()                /* Set      LED1 to ON  state */
#define Led1Off()               /* Set      LED1 to OFF state */
#define Led1Toggle()            /* Toggle   LED1 state */
#define GetLed1()  0            /* Get      LED1 state */

#define Led2On()                /* Set      LED2 to ON  state */
#define Led2Off()               /* Set      LED2 to OFF state */
#define Led2Toggle()            /* Toggle   LED2 state */
#define GetLed2()  0            /* Get      LED2 state */

#define Led3On()                /* Set      LED3 to ON  state */
#define Led3Off()               /* Set      LED3 to OFF state */
#define Led3Toggle()            /* Toggle   LED3 state */
#define GetLed3()  0            /* Get      LED3 state */

#define Led4On()                /* Set      LED4 to ON  state */
#define Led4Off()               /* Set      LED4 to OFF state */
#define Led4Toggle()            /* Toggle   LED4 state */
#define GetLed4()  0            /* Get      LED4 state */

#define TurnOffLeds()           /* Turn OFF All LEDs */
#define TurnOnLeds()            /* Turn ON  All LEDs */

#define SerialFlashing()        /* Start Serial flashing   */
#define StopSerialFlashing()    /* Stop  Serial Flashing for LEDs  and close the timers */

#define Led1Flashing()          /* Start Flashing LED1 */
#define Led2Flashing()          /* Start Flashing LED2 */
#define Led3Flashing()          /* Start Flashing LED3 */
#define Led4Flashing()          /* Start Flashing LED4 */

#define StopLed1Flashing()      /* Stop Flashing LED1 */
#define StopLed2Flashing()      /* Stop  Flashing LED2 */
#define StopLed3Flashing()      /* Stop Flashing LED3 */
#define StopLed4Flashing()      /* Stop Flashing LED4 */

#define InitLed()               /*Set settings and state for all LEDs */


/* define empty prototypes */
/******************************************************************************
*******************************************************************************
* Public prototypes
*******************************************************************************
******************************************************************************/

#define LED_Init()                    /* Initializing LEDs state */

#define LED_TurnOffLed(LEDNr)         /* Turn OFF LED state with number LEDNr */
#define LED_TurnOnLed(LEDNr)          /* Turn ON  LED state with number LEDNr */
#define LED_ToggleLed(LEDNr)          /* Toggle   LED state with number LEDNr */

#define LED_StartFlash(LEDNr)         /* Start Flashing LED with number LEDNr */
#define LED_StopFlash(LEDNr)          /* Stop  Flashing LED with number LEDNr */

#define LED_StartSerialFlash()        /* Start Serial Flashing for All LEDs */
#define LED_StopFlashingAllLeds()     /* Stop  Serial Flashing for All LEDs */
#define LED_ExitSerialFlash()         /* Stop  Serial Flashing for LEDs  and closes the timers*/

#define LED_TurnOffAllLeds()          /* Turn OFF state for All LEDs */
#define LED_TurnOnAllLeds()           /* Turn ON  state for All LEDs */

#define LED_GetLed( LEDNr)        0   /* Get State of LEDNr*/                        
#define LED_SetLed(LEDNr, state)      /* Set LEDNr to "state" state */
#define LED_SetHex(hexValue)          /* Set LEDs state to show a binary Value */
#define Set_Pins( LEDNr)              /* Set initial state of LED */

#endif /* gLEDSupported_d */

/******************************************************************************
*******************************************************************************
* Public type definitions
*******************************************************************************
******************************************************************************/
/* None */

/******************************************************************************
*******************************************************************************
* Public Memory Declarations
*******************************************************************************
******************************************************************************/
/* None */

/******************************************************************************
*******************************************************************************
* Public functions
*******************************************************************************
******************************************************************************/
/* None */

#endif /* _LED_H_ */
