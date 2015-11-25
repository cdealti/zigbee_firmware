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
#ifndef _KEYBOARDHAL_H_
#define _KEYBOARDHAL_H_

#include "GPIO_Interface.h"
#include "Crm.h"


/*****************************************************************************
******************************************************************************
* Public macros
******************************************************************************
*****************************************************************************/



#include "Keyboard_MC1322x.h"


#if(gKbGpioPort_c != gGpioPort0_c)
 #error "Error: Pin configuration not supported for KBI"
#endif

#define gKbGpioMask_c      0x3C000000
#define gKbGpioAllZeros_c  0x00000000
#define gKbGpioAllOnes_c   0xFFFFFFFF


/* Enable Wake up for all 4 keys*/
#define gKbiWuEN_c 0x0f
/*Select egde sense for all  keys*/
#define gKbiWuEVENT_c  0x0f
/*Select wake up polarity to be negativ edge*/
#define gKbiWuPOL_c  0x00
/*enable interrupts for all keys */
#define gKbiIntEN_c  0x0f


#if (gKbGpioPinSwitch1_c < gGpioPin26_c) || (gKbGpioPinSwitch1_c > gGpioPin29_c)
 #error "Error: Pin configuration not supported for Switch 1"
#endif

#if (gKbGpioPinSwitch2_c < gGpioPin26_c) || (gKbGpioPinSwitch2_c > gGpioPin29_c)
 #error "Error: Pin configuration not supported for Switch 2"
#endif

#if (gKbGpioPinSwitch3_c < gGpioPin26_c) || (gKbGpioPinSwitch3_c > gGpioPin29_c)
 #error "Error: Pin configuration not supported for Switch 3"
#endif

#if (gKbGpioPinSwitch4_c < gGpioPin26_c) || (gKbGpioPinSwitch4_c > gGpioPin29_c)
 #error "Error: Pin configuration not supported for Switch 4"
#endif

#define gSWITCH1_MASK_c    1 << (gKbGpioPinSwitch1_c - gGpioPin26_c)
#define gSWITCH2_MASK_c    1 << (gKbGpioPinSwitch2_c - gGpioPin26_c)
#define gSWITCH3_MASK_c    1 << (gKbGpioPinSwitch3_c - gGpioPin26_c)
#define gSWITCH4_MASK_c    1 << (gKbGpioPinSwitch4_c - gGpioPin26_c)
#define gSWITCH_MASK_c (gSWITCH1_MASK_c | gSWITCH2_MASK_c | gSWITCH3_MASK_c | gSWITCH4_MASK_c)

/* Read the port and shift it for better detection of key pressed*/
#define SwitchReadPins KbReadPins()

/* Get the switches state:
     1 - switch is pressed
     0 - switch is released
*/
#define SwitchGet ((SwitchReadPins & gSWITCH1_MASK_c) ^ gSWITCH1_MASK_c) |\
                  ((SwitchReadPins & gSWITCH2_MASK_c) ^ gSWITCH2_MASK_c) |\
                  ((SwitchReadPins & gSWITCH3_MASK_c) ^ gSWITCH3_MASK_c) |\
                  ((SwitchReadPins & gSWITCH4_MASK_c) ^ gSWITCH4_MASK_c)
/*Enable and disable one/more kye interrupt*/
#define KbEnableIrq(SWITCH_MASK) CRM_WU_CNTL.extWuIEn |= SWITCH_MASK 
#define KbDisableIrq(SWITCH_MASK) CRM_WU_CNTL.extWuIEn  &= ~SWITCH_MASK

/*Enable and Disable all external interrupts*/
#define KbDisableAllIrq() KbDisableIrq(gSWITCH_MASK_c) 
#define KbEnableAllIrq() KbEnableIrq (gSWITCH_MASK_c)


/*****************************************************************************
******************************************************************************
* Public type definitions
******************************************************************************
*****************************************************************************/

/*Init the GPIO as input, normal functin mode, pull up enable and read from pads */
extern void KbGpioInit(void);

/* Enable ext. interrupt,  low level sense, wake up polarity to be negative edge,
    enable interrupts for all keys and register the Crm callback to handle interrupts*/
extern void KbCrmInit(void);

/* Read the port and shift it with 26 for better use */
extern uint8_t KbReadPins(void);

/*Read the joystick's center key pin*/
extern uint8_t KbReadCenterKey(void);

/*****************************************************************************
******************************************************************************
* Public prototypes
******************************************************************************
*****************************************************************************/
#endif
