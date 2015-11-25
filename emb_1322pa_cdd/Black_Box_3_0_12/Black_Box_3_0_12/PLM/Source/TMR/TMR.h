/*****************************************************************************
* Timer module private declarations.
*
* (c) Copyright 2006, Freescale, Inc. All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
*****************************************************************************/
#ifndef _TMR_H
#define _TMR_H

#include "TMR_Interface.h"
#include "EmbeddedTypes.h"
#include "Timer.h"
#include "AppToPlatformConfig.h"

/*****************************************************************************
******************************************************************************
* Private type declarations
******************************************************************************
*****************************************************************************/

/* Time in clock ticks. */
typedef uint16_t tmrTimerTicks16_t;
typedef uint32_t tmrTimerTicks32_t;
/*****************************************************************************
******************************************************************************
* Private macros
******************************************************************************
*****************************************************************************/

/* Set to "No operation" or Rise Edge of Primary Source clk */
#define TmrStartTimerHardware() TmrSetMode(gTmrNumber_d, gTmrCntRiseEdgPriSrc_c)
#define TmrStopTimerHardware()  TmrSetMode(gTmrNumber_d, gTmrNoOperation_c)



/*****************************************************************************
******************************************************************************
* Private prototypes
******************************************************************************
*****************************************************************************/


#endif
