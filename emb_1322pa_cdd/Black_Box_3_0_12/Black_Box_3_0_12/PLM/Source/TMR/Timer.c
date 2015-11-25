/*****************************************************************************
* Timer implementation.
*
* Copyright (c) 2007, Freescale, Inc. All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
*****************************************************************************/
#include "EmbeddedTypes.h"
#include "TS_Interface.h"
#include "TMR_Interface.h"
#include "Timer.h"
#include "TMR.h"
#include "Platform.h"
#include "Interrupt.h"

/*****************************************************************************
******************************************************************************
* Private macros
******************************************************************************
*****************************************************************************/

/* Number of elements in an array. */
#define NumberOfElements(array)   ((sizeof(array) / (sizeof(array[0]))))

/* Timer status. If none of these flags are on, the timer is not allocated.
* For allocated timers, exactly one of these flags will be set.
* mTmrStatusActive_c   Timer has been started and has not yet expired.
* mTmrStatusReady_c    TMR_StartTimer() has been called for this timer, but
*                      the timer task has not yet actually started it. The
*                      timer is considered to be active.
* mTmrStatusInactive_c Timer is allocated, but is not active.
*/
#define mTmrStatusFree_c        0
#define mTmrStatusActive_c      0x20
#define mTmrStatusReady_c       0x40
#define mTmrStatusInactive_c    0x80

#define mTimerStatusMask_c      (  mTmrStatusActive_c \
                                   | mTmrStatusReady_c \
                                   | mTmrStatusInactive_c)

/* If all bits in the status byte are zero, the timer is free. */
#define TMR_IsTimerAllocated(timerID)   (maTmrTimerStatusTable[(timerID)])
#define TMR_MarkTimerFree(timerID)       maTmrTimerStatusTable[(timerID)] = 0
#define IsLowPowerTimer(type)           ((type) & gTmrLowPowerTimer_c)

#define mTimerType_c            ( gTmrSingleShotTimer_c \
                                 | gTmrSetSecondTimer_c \
                                 | gTmrSetMinuteTimer_c \
                                 | gTmrIntervalTimer_c \
                                 | gTmrLowPowerTimer_c )


/* TMR_Task() event flag. Only one event is needed. */
#define mTMR_Event_c    ( 1 << 0 )

/*****************************************************************************
******************************************************************************
* Private type definitions
******************************************************************************
*****************************************************************************/

/* One entry in the main timer table.
* intervalInTicks      The timer's original duration, in ticks. Used to reset
*                            intervnal timers.
*
*remainingTicks      Number of ticks until the timer expire.
*

*/
typedef struct tmrTimerTableEntry_tag {
  tmrTimerTicks32_t intervalInTicks;
  tmrTimerTicks32_t remainingTicks;
  pfTmrCallBack_t pfCallBack;
} tmrTimerTableEntry_t;

/* The status and type are bitfields, to save RAM. This costs some code */
/* space, though. */
typedef uint8_t tmrTimerStatus_t;

typedef uint8_t tmrStatus_t;

/*****************************************************************************
******************************************************************************
* Private prototypes
******************************************************************************
*****************************************************************************/
static tmrStatus_t TMR_GetTimerStatus(tmrTimerID_t timerID);
static void TMR_SetTimerStatus(tmrTimerID_t timerID, tmrStatus_t status);
static tmrTimerType_t TMR_GetTimerType(tmrTimerID_t timerID);
static void TMR_SetTimerType(tmrTimerID_t timerID, tmrTimerType_t type);
static bool_t TMR_SetReg(TmrNumber_t tmrNumber);
/* Convert milliseconds to ticks*/
static tmrTimerTicks32_t TmrTicksFromMilliseconds(tmrTimeInMilliseconds_t milliseconds);

/* Call Backs for timer module to handle interrupts*/
void TmrCompEvCallBack(TmrNumber_t tmrNumber);

/*****************************************************************************
******************************************************************************
* Private memory declarations
******************************************************************************
*****************************************************************************/
static tmrTimerTicks16_t previousTimeInTicks;
/* Count to maximum (0xffff - 2*4ms(in ticks)), to be sure that the currentTimeInTicks 
will never roll over previousTimeInTicks in the TMR_Task();
A task have to be executed at most in 4ms */
static uint16_t mMaxToCountDown_c; 
/* The TMR_Task()event will not be issued faster than 4ms*/
static uint32_t mTicksFor4ms;
/* The source clock in Khz*/
static uint32_t mClkSourceKhz = gTmrDefaultClkSourceKhz_c;
/* Main timer table. All allocated timers are stored here. A timer's ID */
/* is it's index in this table. */
static tmrTimerTableEntry_t maTmrTimerTable[gTmrTotalTimers_c];

/* Making the single-byte-per-timer status table a separate array saves */
/* a bit of code space. If an entry is == 0, the timer is not allocated. */
static tmrStatus_t maTmrTimerStatusTable[gTmrTotalTimers_c];

/* Number of Active timers (without low power capability)
   the MCU can not enter low power if numberOfActiveTimers!=0 */
static uint8_t numberOfActiveTimers = 0;
/* Number of low power active timer 
   The MCU can enter in low power if more low power timers are active */
static uint8_t numberOfLowPowerActiveTimers = 0;

#define IncrementActiveTimerNumber(type)  (((type) & gTmrLowPowerTimer_c) \
                                          ?(++numberOfLowPowerActiveTimers) \
                                          :(++numberOfActiveTimers) )                                   
#define DecrementActiveTimerNumber(type)  (((type) & gTmrLowPowerTimer_c) \
                                          ?(--numberOfLowPowerActiveTimers) \
                                          :(--numberOfActiveTimers) ) 

/* No other code should ever post an event to the timer task. */
static tsTaskID_t mTimerTaskID;

/*****************************************************************************
******************************************************************************
* Public functions
******************************************************************************
*****************************************************************************/

/* Initialize the timer module. */
void TMR_Init(void)
{
  
  TmrInit();
  TmrEnable(gTmrNumber_d);  /*enable timer*/
  TMR_SetReg(gTmrNumber_d);
  /*Create the timer task */
  mTimerTaskID = TS_CreateTask(gTsTimerTaskPriority_c, TMR_Task);
  /* Count to maximum (0xffff - 2*4ms(in ticks)), to be sure that the currentTimeInTicks 
  will never roll over previousTimeInTicks in the TMR_Task() */
  mMaxToCountDown_c = 0xFFFF - TmrTicksFromMilliseconds(8); 
  /* The TMR_Task()event will not be issued faster than 4ms*/
  mTicksFor4ms = TmrTicksFromMilliseconds(4);
  
}

/****************************************************************************/
void TMR_NotifyClkChanged(uint32_t clkKhz)
{
  mClkSourceKhz = clkKhz;
  /* Clock was changed, so calculate again  mMaxToCountDown_c.
  Count to maximum (0xffff - 2*4ms(in ticks)), to be sure that the currentTimeInTicks 
  will never roll over previousTimeInTicks in the TMR_Task() */
  mMaxToCountDown_c = 0xFFFF - TmrTicksFromMilliseconds(8); 
  /* The TMR_Task()event will not be issued faster than 4ms*/
  mTicksFor4ms = TmrTicksFromMilliseconds(4);  
}
/****************************************************************************/

/* Allocate a timer.
* Returns the timer id, or gTmrInvalidTimerID_c if the timer table is full.
*/

tmrTimerID_t TMR_AllocateTimer
(
void
)
{
  uint32_t i;
  
  for (i = 0; i < NumberOfElements(maTmrTimerTable); ++i) {
    if (!TMR_IsTimerAllocated(i)) {
      TMR_SetTimerStatus(i, mTmrStatusInactive_c);
      return i;
    }
  }
  
  return gTmrInvalidTimerID_c;
}                                       /* TMR_AllocateTimer() */

/*****************************************************************************/

/* Return true if there are no active timers; return FALSE otherwise. */
bool_t TMR_AreAllTimersOff
(
void
)
{
  return !numberOfActiveTimers;
}                                       /* TMR_AreAllTimersOff() */

/****************************************************************************/

/* Free any timer. Harmless if the timer is already free. */
void TMR_FreeTimer
(
tmrTimerID_t timerID
)
{
  TMR_StopTimer(timerID);
  TMR_MarkTimerFree(timerID);
}                                       /* TMR_FreeTimer() */

/****************************************************************************/

/*Call Back function when the interrupt occurs*/
void TmrCompEvCallBack(TmrNumber_t tmrNumber)
{
  
  TS_SendEvent(mTimerTaskID, mTMR_Event_c);
  
}

/*****************************************************************************/

/* Return TRUE if the timer is active. */
bool_t TMR_IsTimerActive
(
tmrTimerID_t timerID
)
{
  return TMR_GetTimerStatus(timerID) == mTmrStatusActive_c;
}                                       /* TMR_IsTimerActive() */

/* Return TRUE if the timer is ready - The timer is considered to be active*/
bool_t TMR_IsTimerReady
(
tmrTimerID_t timerID
)
{
  return TMR_GetTimerStatus(timerID) == mTmrStatusReady_c;
}                                       /* TMR_IsTimerReady() */

/****************************************************************************/

/****************************************************************************
*
* Start a timer (BeeStack or application). When the timer goes off, call
* the callback function in non-interrupt context.
*
*****************************************************************************/


void TMR_StartTimer
(
tmrTimerID_t timerID,                       /* IN: */
tmrTimerType_t timerType,                   /* IN: */
tmrTimeInMilliseconds_t timeInMilliseconds, /* IN: */
void (*pfTimerCallBack)(tmrTimerID_t)       /* IN: */
)
{
  tmrTimerTicks32_t intervalInTicks;
  
  /* Stopping an already stopped timer is harmless. */
  TMR_StopTimer(timerID);
  
  intervalInTicks = TmrTicksFromMilliseconds(timeInMilliseconds);
  if (!intervalInTicks) {
    intervalInTicks = 1;
  }
  
  TMR_SetTimerType(timerID, timerType);
  maTmrTimerTable[timerID].intervalInTicks = intervalInTicks;
  maTmrTimerTable[timerID].remainingTicks = intervalInTicks;
  maTmrTimerTable[timerID].pfCallBack = pfTimerCallBack;
 
  /* Enable timer, the timer task will do the rest of the work. */
  TMR_EnableTimer(timerID);
}                                       /* TMR_StartTimer() */

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
void TMR_StartLowPowerTimer
(
tmrTimerID_t timerId,
tmrTimerType_t timerType,
uint32_t timeIn,
void (*pfTmrCallBack)(tmrTimerID_t)
) 
{
#if(gTMR_EnableLowPowerTimers_d)
  TMR_StartTimer(timerId, timerType | gTmrLowPowerTimer_c, timeIn, pfTmrCallBack);
#else
(void)timerId;
(void)timerType;
(void)timeIn;
(void)pfTmrCallBack;
#endif
}


/* Start a minute timer. */
/* Note: this is a single shot timer. There are no interval minute timers. */
void TMR_StartMinuteTimer
(
tmrTimerID_t timerId, 
tmrTimeInMinutes_t timeInMinutes, 
void (*pfTmrCallBack)(tmrTimerID_t)
)
{
TMR_StartTimer(timerId, gTmrMinuteTimer_c, TmrMinutes(timeInMinutes), pfTmrCallBack);
}
  
/* Start a second timer. */
/* Note: this is a single shot timer. There are no interval second timers. */
void TMR_StartSecondTimer
(
tmrTimerID_t timerId, 
tmrTimeInSeconds_t timeInSeconds, 
void (*pfTmrCallBack)(tmrTimerID_t)
) 
{
TMR_StartTimer(timerId, gTmrSecondTimer_c, TmrSeconds(timeInSeconds), pfTmrCallBack);
}
 
/****************************************************************************/

void TMR_StartIntervalTimer
(
tmrTimerID_t timerID,
tmrTimeInMilliseconds_t timeInMilliseconds,
void (*pfTimerCallBack)(tmrTimerID_t)
)
{
  TMR_StartTimer(timerID, gTmrIntervalTimer_c, timeInMilliseconds, pfTimerCallBack);
}

void TMR_StartSingleShotTimer
(
tmrTimerID_t timerID,
tmrTimeInMilliseconds_t timeInMilliseconds,
void (*pfTimerCallBack)(tmrTimerID_t)
)
{
  TMR_StartTimer(timerID, gTmrSingleShotTimer_c, timeInMilliseconds, pfTimerCallBack);
}

/****************************************************************************/

/* Stop a timer. Does not free the timer; does not call the timer's callback
* function.
*
* Harmless if the timer is already inactive.
*/
void TMR_StopTimer
(
tmrTimerID_t timerID
)
{
  tmrStatus_t status;
  unsigned int saveInt;
  saveInt = IntDisableAll();
  status = TMR_GetTimerStatus(timerID);
  if (   status == mTmrStatusActive_c
      || status == mTmrStatusReady_c) {
        TMR_SetTimerStatus(timerID, mTmrStatusInactive_c);
        DecrementActiveTimerNumber(TMR_GetTimerType(timerID));
        /* if no sw active timers are enabled, */
        /* call the TMR_Task() to countdown the ticks and stop the hw timer*/
    		if (!numberOfActiveTimers && !numberOfLowPowerActiveTimers) 
           TS_SendEvent(mTimerTaskID, mTMR_Event_c);
      }
  IntRestoreAll(saveInt);    
}                                       /* TMR_StopTimer() */

/*****************************************************************************/

/*****************************************************************************
* Timer task. Called by the kernel when the timer ISR posts a timer event.
******************************************************************************/
void TMR_Task
(
event_t events
)
{
  static bool_t timerHardwareIsRunning = FALSE;
  tmrTimerTicks16_t nextInterruptTime;
  pfTmrCallBack_t pfCallBack;
  tmrTimerTicks16_t currentTimeInTicks;
  tmrTimerStatus_t status;
  tmrTimerTicks16_t ticksSinceLastHere, ticksdiff; 
  uint8_t timerID;
  unsigned int saveInt;
  tmrTimerType_t timerType;
  (void)events;
  
  TmrReadValue(gTmrNumber_d,&currentTimeInTicks);
  /* calculate difference between current and previous.  */
  ticksSinceLastHere = (currentTimeInTicks - previousTimeInTicks);
  /* remember for next time */
  previousTimeInTicks = currentTimeInTicks;
  
  for (timerID = 0; timerID < NumberOfElements(maTmrTimerTable); ++timerID) {
    saveInt = IntDisableAll();
    status = TMR_GetTimerStatus(timerID);
    /* If TMR_StartTimer() has been called for this timer, start it's count */
    /* down as of now. */
    if (status == mTmrStatusReady_c) {
      TMR_SetTimerStatus(timerID, mTmrStatusActive_c);
      IntRestoreAll(saveInt);
      continue;
    }
    IntRestoreAll(saveInt);
    
    /* Ignore any timer that is not active. */
    if (status != mTmrStatusActive_c) {
      continue;
    }
    
    /* This timer is active. Decrement it's countdown.. */
    if (maTmrTimerTable[timerID].remainingTicks > ticksSinceLastHere) {
      maTmrTimerTable[timerID].remainingTicks -= ticksSinceLastHere;
      continue;
    }
    
    timerType = TMR_GetTimerType(timerID);
    /* If this is an interval timer, restart it. Otherwise, mark it as inactive. */
    if ( (timerType & gTmrSingleShotTimer_c) ||
         (timerType & gTmrSetMinuteTimer_c) ||
         (timerType & gTmrSetSecondTimer_c)  ) {
      TMR_StopTimer(timerID);
    } else {
      maTmrTimerTable[timerID].remainingTicks = maTmrTimerTable[timerID].intervalInTicks;
    }
    /* This timer has expired. */
    pfCallBack = maTmrTimerTable[timerID].pfCallBack;
    /*Call callback if it is not NULL
    This is done after the timer got updated,
    in case the timer gets stopped or restarted in the callback*/
    if (pfCallBack) {
      pfCallBack(timerID);
    }
    
  }  /* for (timerID = 0; timerID < ... */
  
  /* Find the shortest active timer. */
  nextInterruptTime = mMaxToCountDown_c;
  for (timerID = 0; timerID < NumberOfElements(maTmrTimerTable); ++timerID) {
    if (TMR_GetTimerStatus(timerID) == mTmrStatusActive_c) {
      if (nextInterruptTime > maTmrTimerTable[timerID].remainingTicks) {
        nextInterruptTime = maTmrTimerTable[timerID].remainingTicks;
      }
    }
  }
  
  /* Check to be sure that the timer is not programmed in the past */    
  saveInt = IntDisableAll();
  TmrReadValue(gTmrNumber_d,&ticksdiff);
  /* Number of ticks to be here */
  ticksdiff = (uint16_t)(ticksdiff - currentTimeInTicks); 
   /* Next ticks to count already expired?? */
  if(ticksdiff >= nextInterruptTime)
  {  
    /* Is assumed that a task has to be executed in 4ms...
       so if the ticks already expired enter in TMR_Task() after 4ms*/
    nextInterruptTime = ticksdiff + mTicksFor4ms;
  } 
  else 
  {
    /* Time reference is 4ms...
       so be sure that won't be loaded in Cmp Reg. less that 4ms in ticks 
    */
     if((nextInterruptTime - ticksdiff) < mTicksFor4ms) 
     {
       nextInterruptTime = ticksdiff + mTicksFor4ms;
     }
  
  }
  /* Update the compare register */
  nextInterruptTime += currentTimeInTicks;
  SetComp1Val(gTmrNumber_d, nextInterruptTime);
  IntRestoreAll(saveInt);
  
  if (!numberOfActiveTimers && !numberOfLowPowerActiveTimers)
  {
    TmrStopTimerHardware();
    timerHardwareIsRunning = FALSE;
  } 
  else 
    if (!timerHardwareIsRunning) 
    {
      TmrStartTimerHardware();
      timerHardwareIsRunning = TRUE;
    }
}                                       /* TMR_Task() */

/*****************************************************************************
******************************************************************************/

/* Enable or disable the timer tmrID
   If enable = TRUE timer is active
   Else timer is inactive 
*/
void TMR_EnableTimer(tmrTimerID_t tmrID)
{    
  unsigned int saveInt;
  saveInt = IntDisableAll();
  if (TMR_GetTimerStatus(tmrID) == mTmrStatusInactive_c)
  {      
    IncrementActiveTimerNumber(TMR_GetTimerType(tmrID));    
    TMR_SetTimerStatus(tmrID, mTmrStatusReady_c);
    TS_SendEvent(mTimerTaskID, mTMR_Event_c);
  }  
 IntRestoreAll(saveInt);
}


/*****************************************************************************/

/*
This function is called by Low Power module; Also this function stops the 
harware timer.
Return: time in millisecond that wasn't counted before entering in sleep  
*/
uint16_t TMR_NotCountedTicksBeforeSleep(void)
{
#if (gTMR_EnableLowPowerTimers_d)  
  uint16_t currentTimeInTicks;
  
  if (!numberOfLowPowerActiveTimers)
    return 0;

  TmrReadValue(gTmrNumber_d,&currentTimeInTicks);
  TmrStopTimerHardware();

 /* The hw timer is stopped but keep timerHardwareIsRunning = TRUE...*/ 
 /* The Lpm timers are considered as being in running mode, so that  */
 /* not to start the hw timer if a TMR event occurs (this shouldn't happen) */ 
     
  return  (uint16_t)(currentTimeInTicks - previousTimeInTicks);
#else  
   return 0;
#endif /* #if (gTMR_EnableLowPowerTimers_d) */   
} 

/*****************************************************************************/

      
/* This function is called by the Low Power Module */
/* each time the MCU wakes up */ 
/* The function make an approximate sync. the active low power timers. */                              
void TMR_SyncLpmTimers(uint32_t sleepDurationTmrTicks)
{
#if (gTMR_EnableLowPowerTimers_d) 
 index_t  timerID;
 tmrTimerType_t timerType;

 /* Check if there are low power active timer */
 if (!numberOfLowPowerActiveTimers)
    return;          

 /* For each timer, detect the timer type and count down the spent duration in sleep */  
 for (timerID = 0; timerID < NumberOfElements(maTmrTimerTable); ++timerID) 
 {

  /* Detect the timer type and count down the spent duration in sleep */
  timerType = TMR_GetTimerType(timerID);
  
  /* Sync. only the low power timers that are active */
  if ( (TMR_GetTimerStatus(timerID) == mTmrStatusActive_c)
        && (IsLowPowerTimer(timerType)) ) 
  {
           /* Timer expired when MCU was in sleep mode??? */
           if( maTmrTimerTable[timerID].remainingTicks > sleepDurationTmrTicks) 
           {
             maTmrTimerTable[timerID].remainingTicks -= sleepDurationTmrTicks;
 
           } 
           else 
           {
             maTmrTimerTable[timerID].remainingTicks = 0;           
           }
        
   }

 }/* end for (timerID = 0;.... */

 TmrStartTimerHardware();
 TmrReadValue(gTmrNumber_d, &previousTimeInTicks);

 TS_SendEvent(mTimerTaskID, mTMR_Event_c);
#else
 (void)sleepDurationTmrTicks;
#endif /* #if (gTMR_EnableLowPowerTimers_d) */ 
}



/*****************************************************************************/

/* Private functions
******************************************************************************
*****************************************************************************/

/* Configure the timer registers to work in compare mode, to count repeatedly (roll over)
and register the callbacks to handle interrupts*/
bool_t TMR_SetReg(TmrNumber_t tmrNumber)
{
  
  TmrConfig_t regConfig;
  TmrStatusCtrl_t regStatusCtrl;
  TmrComparatorStatusCtrl_t regComStatusCtrl;
  
  /* Register the callback executed when an interrupt occur */
  if(gTmrErrNoError_c != TmrSetCallbackFunction(tmrNumber, gTmrComp1Event_c, &TmrCompEvCallBack))
  {
    return FALSE;
  }
  /* Don't stat the timer yet */
  if (gTmrErrNoError_c != TmrStopTimerHardware()) /*set timer mode no operation*/
  {
    return FALSE;
  }
  
  if(gTmrErrNoError_c != TmrWriteValue(tmrNumber, 0x0000))    /*clear counter*/
  {
    return FALSE;
  }  
  
  regStatusCtrl.uintValue = 0x0000;
  if (gTmrErrNoError_c != TmrSetStatusControl(tmrNumber, &regStatusCtrl))
  {
    return FALSE;
  }  
  
  regComStatusCtrl.uintValue = 0x0000;
  regComStatusCtrl.bitFields.TCF1EN = TRUE; /* Enable Compare 1 interrupt */
  if (gTmrErrNoError_c != TmrSetCompStatusControl(tmrNumber, &regComStatusCtrl))
  {
    return FALSE;
  }
  
  SetComp1Val(tmrNumber, 0x0000); /*clear comparator 1 value*/ 
  regConfig.tmrOutputMode = gTmrAssert_c; 
  regConfig.tmrCoInit = FALSE;  /*co-chanel counter/timers can not force a re-initialization of this counter/timer*/
  regConfig.tmrCntDir = FALSE;  /*count-up*/
  regConfig.tmrCntLen = FALSE;  /*rol over */
  regConfig.tmrCntOnce = FALSE;   /*count repeatedly*/
  regConfig.tmrSecondaryCntSrc = gTmrSecondaryCnt0Input_c;    /*secondary count source not needed*/
  regConfig.tmrPrimaryCntSrc = gTmrPrimaryClkDefault_c;    /*primary count source is IP BUS clock divide by 128 prescaler*/
  if (gTmrErrNoError_c != TmrSetConfig(tmrNumber, &regConfig))  /*set timer configuration */
  {
    return FALSE;
  }
  
  return TRUE;
}


/* Time from milliseconds in ticks*/
static tmrTimerTicks32_t TmrTicksFromMilliseconds(tmrTimeInMilliseconds_t milliseconds)
{
  return (milliseconds * (mClkSourceKhz/gTmrPrescaler_c));
}

static tmrStatus_t TMR_GetTimerStatus
(
tmrTimerID_t timerID
)
{
  return maTmrTimerStatusTable[timerID] & mTimerStatusMask_c;
}

static void TMR_SetTimerStatus
(
tmrTimerID_t timerID, 
tmrStatus_t status
)
{
  maTmrTimerStatusTable[timerID] = (maTmrTimerStatusTable[timerID] & ~mTimerStatusMask_c) | status;
}

/*****************************************************************************/

static tmrTimerType_t TMR_GetTimerType
(
tmrTimerID_t timerID
)
{
  return maTmrTimerStatusTable[timerID] & mTimerType_c;
}

static void TMR_SetTimerType
(
tmrTimerID_t timerID, 
tmrTimerType_t type
)
{
  maTmrTimerStatusTable[timerID] = (maTmrTimerStatusTable[timerID] & ~mTimerType_c) | type;
}

