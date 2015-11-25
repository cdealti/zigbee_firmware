/*****************************************************************************
* All the timer module public interface are described in this header file.
*
* (c) Copyright 2008, Freescale, Inc. All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*****************************************************************************/

#ifndef _TMR_INTERFACE_H
#define _TMR_INTERFACE_H

#include "EmbeddedTypes.h"
#include "TS_Interface.h"

/*****************************************************************************
******************************************************************************
* Public macros
******************************************************************************
*****************************************************************************/

/* Enable Low Power Capability. */
#ifndef gTMR_EnableLowPowerTimers_d
#define gTMR_EnableLowPowerTimers_d    TRUE         
#endif

/* Select the hardware timer */
#define  gTmrNumber_d  gTmr0_c

/* Select the prescaler and the Clock source for timer */
#define gTmrPrimaryClkDefault_c gTmrPrimaryClkDiv128_c
#define gTmrPrescaler_c (uint32_t)128
/*KHz*/
#define gTmrDefaultClkSourceKhz_c SYSTEM_CLOCK

/* Timer task priority. Must be the highest priority task. 0xFF is reserved */
/* is reserved as an invalid task priority. */
#define gTsTimerTaskPriority_c  0xFE

/* Number of timers needed by the application. */
#ifndef gTmrApplicationTimers_c
#define gTmrApplicationTimers_c 7
#endif

/* Number of timers needed by the protocol stack. */
#ifndef gTmrStackTimers_c
#define gTmrStackTimers_c  35
#endif

/* Total number of timers. */
#ifndef gTmrTotalTimers_c
#define gTmrTotalTimers_c   (gTmrApplicationTimers_c + gTmrStackTimers_c)
#endif

/* Unit-of-time macros, for documentation. */

#define TmrMilliseconds(n)  ((tmrTimeInMilliseconds_t)(n))
#define TmrSeconds(n)       ((tmrTimeInMilliseconds_t)(TmrMilliseconds(n) * 1000))
#define TmrMinutes(n)       ((tmrTimeInMilliseconds_t)(TmrSeconds(n) * 60))



/*****************************************************************************
******************************************************************************
* Public type definitions
******************************************************************************
*****************************************************************************/

/* Times are specified in milliseconds. */
typedef uint32_t tmrTimeInMilliseconds_t;

/* times are specified in minutes, allows up to 45.5 days */
typedef uint16_t tmrTimeInMinutes_t;      /* limited to 0xffff minutes (40 days) */

/* times are specified in seconds */
typedef uint16_t tmrTimeInSeconds_t;
/* Timer handle. */
typedef uint8_t tmrTimerID_t;

/* Reserve one timer id as an invalid value. */
#define gTmrInvalidTimerID_c  0xFF

/* Timer behavior flags. */
typedef uint8_t tmrTimerType_t;
#define gTmrSingleShotTimer_c   0x01
#define gTmrIntervalTimer_c     0x02
#define gTmrSetMinuteTimer_c    0x04
#define gTmrSetSecondTimer_c    0x08
#define gTmrLowPowerTimer_c     0x10

/* Used the following timer type */
/* to start a minute/second timer without low power capability (only single shot) */
#define gTmrMinuteTimer_c       ( gTmrSetMinuteTimer_c )
#define gTmrSecondTimer_c       ( gTmrSetSecondTimer_c )
/* Used the following timer type */
/* to start a minute/second/millisecond timer with low power capability */
#define gTmrLowPowerMinuteTimer_c   ( gTmrMinuteTimer_c | gTmrLowPowerTimer_c )
#define gTmrLowPowerSecondTimer_c   ( gTmrSecondTimer_c | gTmrLowPowerTimer_c )
#define gTmrLowPowerSingleShotMillisTimer_c   ( gTmrSingleShotTimer_c | gTmrLowPowerTimer_c )
#define gTmrLowPowerIntervalMillisTimer_c     ( gTmrIntervalTimer_c | gTmrLowPowerTimer_c )

/* Timer callback function. */
typedef void (*pfTmrCallBack_t)(tmrTimerID_t);

/*****************************************************************************
******************************************************************************
* Public prototypes
******************************************************************************
*****************************************************************************/

/* Reserve a timer. Returns the timer number that should be passed to other */
/* TMR_xxx() API functions. Returns gTmrInvalidTimerID_c if there no timers */
/* available. Timers are reserved until they are freed by TMR_FreeTimer()*/
extern tmrTimerID_t TMR_AllocateTimer(void);

/* Reserve a minute timer. */
/* Returns gTmrInvalidTimerID_c if there are no timers availabled. */
#define TMR_AllocateMinuteTimer() TMR_AllocateTimer()

/* Reserve a second timer. */
/* Returns gTmrInvalidTimerID_c if there are no timers availabled. */
#define TMR_AllocateSecondTimer() TMR_AllocateTimer() 

/* Free a minute timer. Safe to call even if the timer is running. */
#define TMR_FreeMinuteTimer(timerID) TMR_FreeTimer(timerID)
/* Free a second timer. Safe to call even if the timer is running. */
#define TMR_FreeSecondTimer(timerID) TMR_FreeTimer(timerID)
/* Return true if there are no active non-low power timers. Used by power management. */
extern bool_t TMR_AreAllTimersOff(void);

/* Free a timer. Safe to call even if the timer is running. */
extern void TMR_FreeTimer(tmrTimerID_t timerID);

/* Initialize the timer code. */
extern void TMR_Init(void);

/* Return TRUE if the timer is running; FALSE otherwise. */
extern bool_t TMR_IsTimerActive(tmrTimerID_t timerID);

/* Return TRUE if the timer started; FALSE otherwise. */
extern bool_t TMR_IsTimerReady(tmrTimerID_t timerID);

/* Start a timer. When the timer goes off, call the callback function */
/* in non-interrupt context. If the timer is running when this function */
/* is called, it will be stopped and restarted. */
extern void TMR_StartTimer
  (
  tmrTimerID_t timerId,
  tmrTimerType_t timerType,
  tmrTimeInMilliseconds_t timeInMilliseconds,
  void (*pfTmrCallBack)(tmrTimerID_t)
  );

/* Start a low power timer. When the timer goes off, call the callback function */
/* in non-interrupt context. If the timer is running when this function */
/* is called, it will be stopped and restarted. */
/* Start the timer with the following timer types:
   gTmrLowPowerMinuteTimer_c
   gTmrLowPowerSecondTimer_c
   gTmrLowPowerSingleShotMillisTimer_c
   gTmrLowPowerIntervalMillisTimer_c
*/
/* The MCU can enter in low power if there are only active low power timers */
extern void TMR_StartLowPowerTimer
  (
  tmrTimerID_t timerId,
  tmrTimerType_t timerType,
  uint32_t timeIn,
  void (*pfTmrCallBack)(tmrTimerID_t)
  );

/* Shortcut version of TMR_StartTimer(), with one less parameter to pass. */
extern void TMR_StartIntervalTimer
  (
  tmrTimerID_t timerId,
  tmrTimeInMilliseconds_t timeInMilliseconds,
  void (*pfTmrCallBack)(tmrTimerID_t)
  );

/* Shortcut version of TMR_StartTimer(), with one less parameter to pass. */
extern void TMR_StartSingleShotTimer
  (
  tmrTimerID_t timerId,
  tmrTimeInMilliseconds_t timeInMilliseconds,
  void (*pfTmrCallBack)(tmrTimerID_t)
  );

/* Start a minute timer.  */
/* Note: this is a single shot timer. There are no interval minute timers. */
extern void TMR_StartMinuteTimer
  (
  tmrTimerID_t timerId, 
  tmrTimeInMinutes_t timeInMinutes, 
  void (*pfTmrCallBack)(tmrTimerID_t)
  );
  
/* Start a second timer.  */
/* Note: this is a single shot timer. There are no interval second timers. */
extern void TMR_StartSecondTimer
  (
  tmrTimerID_t timerId, 
  tmrTimeInSeconds_t timeInSeconds, 
  void (*pfTmrCallBack)(tmrTimerID_t)
  ); 
/* Stop a timer. Does not trigger the timer, i.e. does not call the timer's */
/* callback function. If the timer has expired, but the callback function has */
/* not yet been called, this function will prevent the timer's callback from */
/* being called. Does not free the timer. Safe to call anytime, regardless of */
/* the state of the timer. */
extern void TMR_StopTimer(tmrTimerID_t timerID);

/* This function is called by the Low Power Module */
/* each time the MCU wakes up */ 
/* The function make an approximate sync. of all active low power timers */
extern void TMR_SyncLpmTimers(uint32_t sleepDurationTmrTicks);

/*
This function is called by Low Power module; Also this function stops the 
harware timer.
Return: time in millisecond that wasn't counted before entering in sleep  
*/
uint16_t TMR_NotCountedTicksBeforeSleep(void);

/* stop a timer started by TMR_StartMinuteTimer() */
#define TMR_StopMinuteTimer(timerID)  TMR_StopTimer(timerID)  

/* stop a timer started by TMR_StartSecondTimer() */
#define TMR_StopSecondTimer(timerID)  TMR_StopTimer(timerID) 

/* Main timer task. Process timer events in non-interrupt context. */
extern void TMR_Task(event_t events);

/*The default IP Bus Clock for TMR module is 24000Khz;
    Notify the TMR module that the IP Bus Clk was changed for saving power by CRM module; 
    The clkKhz parameter is in Khz*/
extern void TMR_NotifyClkChanged(uint32_t clkKhz);


/* Enable or disable the timer tmrID */
/* If enable = TRUE timer is active */
/* Else timer is inactive */
extern void TMR_EnableTimer(tmrTimerID_t tmrID);

#endif
