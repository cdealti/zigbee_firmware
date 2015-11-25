/************************************************************************************
*
* (c) Copyright 2007, Freescale, Inc.  All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale.
*
* Last Inspected:
* Last Tested:
************************************************************************************/
#ifndef _PLATFORM_INTERRUPTS_H_
#define _PLATFORM_INTERRUPTS_H_

#include "AppMacInterrupt.h"

extern void AppInterruptsInit(void);
extern void EnableMacaInt(void);
extern void DisableMacaInt(void);
extern unsigned int IsMacaIntEnabled(void);
extern unsigned int IsMacaIntAsserted(void);


#endif
