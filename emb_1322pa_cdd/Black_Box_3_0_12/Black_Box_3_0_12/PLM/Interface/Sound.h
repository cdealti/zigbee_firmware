/* Sound.h
*
* Header file for interface to the sound driver. Includes only buzzer support for
* now.
*
* Copyright (c) 2006, Freescale, Inc.  All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
************************************************************************************/
#ifndef _SOUND_H_
#define _SOUND_H_



/* enable/disable buzzer support */
#ifndef gBuzzerSupported_d
#define gBuzzerSupported_d  FALSE
#endif


#if gBuzzerSupported_d
#define gHaveBuzzer_c  TRUE


#else
#define gHaveBuzzer_c  FALSE
#endif

/* buzzer supported this board (and turned on) */
#if gHaveBuzzer_c

#define gBeepDurationInMs_c  250
#define gBeepVolumeInPrc_c   30
#define gBeepTimer_c         gTmr3_c 
extern bool_t BuzzerInit(void); /* TmrInit() must be called before this function  */
extern bool_t BuzzerBeep (void);/* InterruptInit(), TmrInit(), 
                                   IntAssignHandler(gTmrInt_c, (IntHandlerFunc_t)TmrIsr),
                                   ITC_EnableInterrupt(gTmrInt_c),  BuzzerInit(), IntEnableIRQ()
                                   must be called before this function  */

/* buzzer not supported this board (or turned off) */
#else
#define BuzzerInit()
#define BuzzerBeep()
#endif

#endif /* _SOUND_H_ */

