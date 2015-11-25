/*****************************************************************************
* UART / SCI / USB / Serial Port declarations.
*
* (c) Copyright 2006, Freescale, Inc. All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
*****************************************************************************/

#ifndef _Uart_h
#define _Uart_h

/*****************************************************************************
******************************************************************************
* Private macros
******************************************************************************
*****************************************************************************/

/* TODO: Move this to Platform.h */
#define PLATFORM_CLOCK 24000

/* if neither UART enabled, then no code or data */
#if gUart1_Enabled_d || gUart2_Enabled_d
  /* id for Uart task */
extern  tsTaskID_t gUartTaskId;
#endif

/* Number of elements in an array. */
#ifndef NumberOfElements
#define NumberOfElements(array)     ((sizeof(array) / (sizeof(array[0]))))
#endif

#endif
