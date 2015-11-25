/************************************************************************************
* This is the public header file for the Debug module. 
*
*
* (c) Copyright 2004, Freescale, Inc.  All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
************************************************************************************/

#ifndef _DEBUG_H_
#define _DEBUG_H_

//#include "PlatformToMacPhyDbgConfig.h"
//#include "PortConfig.h"


/************************************************************************************
*************************************************************************************
* Public type definitions
*************************************************************************************
************************************************************************************/
#if 0
  // Used with Dbg_SetLed(), and Dbg_SetPin()
  // for switching LED's and port pins.
enum {
  gDbgOn_c,
  gDbgOff_c,
  gDbgToggle_c,
};

  // Used with Dbg_SetLed() for selecting one or
  // more LEDS to be turned on/off or toggled.
  // Multiple LEDs can be switched by bitwise ORing
  // the gDbgLed*_c constants.

enum {
  gDbgLed1_c   = LED1_PIN,
  gDbgLed2_c   = LED2_PIN,
  gDbgLed3_c   = LED3_PIN,
  gDbgLed4_c   = LED4_PIN,
  gDbgLedAll_c = LED_MASK
};

  // Used with Dbg_SetPin() for selecting one or more
  // port pins to be turned on/off or toggled.
  // Multiple pins can be switched by bitwise ORing
  // the gDbgPin*_c constants.

enum {
  gDbgPin0_c = PORT1_PIN,
  gDbgPin1_c = PORT2_PIN,
  gDbgPin2_c = PORT3_PIN,
  gDbgPin3_c = PORT4_PIN
};


/************************************************************************************
*************************************************************************************
* Public macro definitions
*************************************************************************************
************************************************************************************/

#if defined(ENABLE_LEDS)

/************************************************************************************
* Turn on, off or toggle one or more LEDs
*   led:   gDbgLedAll_c, gDbgDbgLed1_c - gDbgDbgLed4_c
*   state: gDbgOff_c, gDbgOn_c, gDbgToggle_c 
*
* Interface assumptions:
*   ENABLE_LEDS must be defined
*   
* Return value:
*   None
* 
* Revision history:
*   date      Author    Comments
*   ------    ------    --------
*   250505    BPPED1    Created
* 
************************************************************************************/
#define Dbg_SetLed(led, state)    \
{                                 \
  if((state) == gDbgOn_c)         \
    LED_PORT &= ~(led); /*ON*/    \
  else if((state) == gDbgOff_c)   \
    LED_PORT |= (led);  /*OFF*/   \
  else                            \
    LED_PORT ^= (led);  /*TOGGLE*/\
}


/************************************************************************************
* Return state of LEDs. 
*
* Interface assumptions:
*   ENABLE_LEDS must be defined
*   
* Return value:
*   Bitwise ORed combination of gDbgDbgLed1_c, gDbgDbgLed2_c, gDbgDbgLed3_c,
*   and gDbgDbgLed4_c.
* 
* Revision history:
*   date      Author    Comments
*   ------    ------    --------
*   250505    BPPED1    Created
* 
************************************************************************************/
#define Dbg_GetLed(led) ((~LED_PORT) & (led))

#else

#define Dbg_SetLed(led, state)
#define Dbg_GetLed(led)

#endif // ENABLE_LEDS


#if defined(ENABLE_PORT_PINS)

/************************************************************************************
* Configure port pins for MAC/PHY debugging purposes. Can be used for e.g. timing
* analysis using a scope.
*
* Interface assumptions:
*   None
*   
* Return value:
*   None
* 
* Revision history:
*   date      Author    Comments
*   ------    ------    --------
*   250505    BPPED1    Created
* 
************************************************************************************/
#define Dbg_EnablePins()  ((PTADD) |= (PORT_PINS_MASK))


/************************************************************************************
* Turn on, off or toggle one or more port pins.
*   ports: gDbgPin0_c - gDbgPin3_c
*   state: gDbgOff_c, gDbgOn_c, gDbgToggle_c 
*
* Interface assumptions:
*   ENABLE_PORT_PINS must be defined
*   
* Return value:
*   None
* 
* Revision history:
*   date      Author    Comments
*   ------    ------    --------
*   250505    BPPED1    Created
* 
************************************************************************************/
#define Dbg_SetPin(ports, state)                      \
{                                                     \
  if(state == gDbgOn_c) PORT_PINS |= (ports);         \
  else if(state == gDbgOff_c) PORT_PINS &= ~(ports);  \
  else PORT_PINS ^= (ports);                          \
}


/************************************************************************************
* Return state of port pins.
*
* Interface assumptions:
*   ENABLE_PORT_PINS must be defined
*   
* Return value:
*   Bitwise ORed combination of gDbgPin0_c, gDbgPin1_c, gDbgPin2_c, and gDbgPin3_c.
* 
* Revision history:
*   date      Author    Comments
*   ------    ------    --------
*   250505    BPPED1    Created
* 
************************************************************************************/
#define Dbg_GetPin(ports) (PORT_PINS & (ports))

#else

#define Dbg_EnablePins()
#define Dbg_SetPin(state, ports)
#define Dbg_GetPin(ports)

#endif // ENABLE_PORT_PINS


#if defined(ENABLE_ASSERTS)

/************************************************************************************
* This macro will stop the HCS08 CPU if 'regexp' is FALSE. A debug environment
* must be setup on the host PC that enables the breakpoint functionality of this
* function. Otherwise, the CPU will simply reset.
*   
* Interface assumptions:
*   None
*   
* Return value:
*   None
* 
* Revision history:
*   date      Author    Comments
*   ------    ------    --------
*   200104    BPPED1    Created
* 
************************************************************************************/
#define Dbg_Assert(regexp)  \
{                           \
  if((regexp)==FALSE)       \
    asm BGND;               \
}

#else

#define Dbg_Assert(regexp)

#endif // ENABLE_ASSERTS


/************************************************************************************
* The Dbg_SetLed(), Dbg_SetPin(), and Dbg_Assert() macros will result in INFORMATION
* messages from the compiler. To hide the messages the following two pragmas are used
* whenever the debug.h header file is included, and one or more of the debug features
* are enabled.
*
* Interface assumptions:
*   None
*   
* Return value:
*   None
* 
* Revision history:
*   date      Author    Comments
*   ------    ------    --------
*   250505    BPPED1    Created
* 
************************************************************************************/
#if defined(ENABLE_ASSERTS) || defined(ENABLE_LEDS) ||defined(ENABLE_PORT_PINS)
  #pragma MESSAGE DISABLE C4000 // Remove "Always true warning"
  #pragma MESSAGE DISABLE C4001 // Remove "Always false warning"
#endif // ENABLE_ASSERTS || ENABLE_LEDS || ENABLE_PORT_PINS


#if (defined(ENABLE_LEDS) || defined(ENABLE_PORT_PINS))
/************************************************************************************
* Initialize debug related io ports.
*
* Interface assumptions:
*   None
*   
* Return value:
*   None
* 
* Revision history:
*   date      Author    Comments
*   ------    ------    --------
*   250504    BPPED1    Created
* 
************************************************************************************/
#define Dbg_Init()                      \
{                                       \
  Dbg_EnablePins();                     \
  Dbg_SetLed(gDbgLedAll_c, gDbgOff_c);  \
}

#else

#define Dbg_Init()

#endif // (ENABLE_LEDS || ENABLE_PORT_PINS)


/************************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/

/************************************************************************************
* Target printf function (should do nothing for targets)
*
* Interface assumptions:
*   None
*   
* Return value:
*   None
* 
* Revision history:
*   date      Author    Comments
*   ------    ------    --------
*   250504    BPPED1    Created
* 
************************************************************************************/
#define Dbg_Printf(pString)                        
#define Dbg_Printf1(pString, arg1)             
#define Dbg_Printf2(pString, arg1, arg2)       
#define Dbg_Printf3(pString, arg1, arg2, arg3) 

#endif /* 0 */

#endif /* _DEBUG_H_ */
