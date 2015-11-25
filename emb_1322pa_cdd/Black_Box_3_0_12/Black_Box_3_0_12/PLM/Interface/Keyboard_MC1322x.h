/************************************************************************************
* Led_MC1322x.h
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
   Date                     Author                                    Comments
   ----------               ----------------------                    -------
   12.04.2007               B06061                                   Modified
************************************************************************************/

#ifndef _Keyboard_MC1322x_H_
#define _Keyboard_MC1322x_H_

/******************************************************************************
*******************************************************************************
* Public Macros
*******************************************************************************
******************************************************************************/
/* BEGIN Define Key pin mapping */
/* Select the port */
#define gKbGpioPort_c           gGpioPort0_c

/* Select the pins for switch 1-4. Possible values: 26, 27, 28, 29  */
#define gKbGpioPinSwitch1_c     gGpioPin26_c
#define gKbGpioPinSwitch2_c     gGpioPin27_c
#define gKbGpioPinSwitch3_c     gGpioPin28_c
#define gKbGpioPinSwitch4_c     gGpioPin29_c

/* Center key pin for joystick */
#define gCenterKey_c     gGpioPin22_c
/* END Define Key pin mapping */






/******************************************************************************
*******************************************************************************
* Public type definitions
*******************************************************************************
******************************************************************************/
/* None */

/******************************************************************************
*******************************************************************************
* Public prototypes
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

#endif /* _Keyboard_MC1322x_H_ */