/*****************************************************************************
* Kernel / task handling public API
*
* (c) Copyright 2006, Freescale, Inc. All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
*****************************************************************************/

#ifndef _TS_INTERFACE_H_
#define _TS_INTERFACE_H_

#include "EmbeddedTypes.h"

/*****************************************************************************
******************************************************************************
* Public macro definitions
******************************************************************************
*****************************************************************************/

/* Maximum number of tasks in the task table. */
#define gTsMaxTasks_c   15

#ifndef gTsDebug_d
  #define gTsDebug_d  FALSE                   /* Enable  kernel debug code. */
#endif

/*****************************************************************************
******************************************************************************
* Public type definitions
******************************************************************************
*****************************************************************************/

/* One bit per event. */
typedef uint16_t event_t;

/* Unique task identifier numbers. */
/* tsTaskID_t ==  0 is reserved for the idle task. */
/* tsTaskID_t == -1 is reserved as an invalid value. */
typedef index_t tsTaskID_t;
#define gTsIdleTaskID_c     (( tsTaskID_t )  0 )
#define gTsInvalidTaskID_c  (( tsTaskID_t ) -1 )

/* Task priority. */
/* tsTaskPriority_t ==  0 is reserved for the idle task. */
/* tsTaskPriority_t == -1 is reserved as an invalid value. */
typedef uint8_t tsTaskPriority_t;
#define gTsIdleTaskPriority_c       (( tsTaskPriority_t )  0 )
#define gTsInvalidTaskPriority_c    (( tsTaskPriority_t ) -1 )

/* Task event handler function. */
typedef void ( *pfTsTaskEventHandler_t )( event_t );

/* The idle task, and only the idle task, is added to the task table */
/* by TS_Init(). The IdleTask() function must be provided by code */
/* external to the kernel. It is normally used for power management, */
/* non-volatile storage management, and other, similar background */
/* capabilities. */
extern void IdleTask( event_t );

/*****************************************************************************
******************************************************************************
* Public memory declarations
******************************************************************************
*****************************************************************************/

/* Store the TaskID of the idle task in a global, so the MAC layer can */
/* find it. */
extern tsTaskID_t gIdleTaskID;
/* The MAC TaskID must also be known by the MAC. */
extern tsTaskID_t gMacTaskID_c;

/*****************************************************************************
******************************************************************************
* Public Prototypes
******************************************************************************
*****************************************************************************/

/*****************************************************************************
 * TS_ClearEvent
 *
 * Remove events from a task's event flags.
 *****************************************************************************/
void TS_ClearEvent
  (
  tsTaskID_t taskID,                    /* IN: Which task. */
  event_t events                        /* IN: Which event(s) to clear. */
  );

/*****************************************************************************
 * TS_CreateTask
 *
 * Add a task to the kernel's task table. Returns a task ID that can be passed
 * to TS_SendEvent() to identify the task. If the task table is full, returns
 * gTsInvalidTaskID_c.
 *
 * taskPriority == 0 is reserved for the idle task, and must never be specified
 * for any other task. TS_CreateTask() does not check for this.
 *
 * Note that TS_CreateTask() does not prevent a given event handler function
 * pointer from being added more than once to the task table.
 *
 * Note that if TS_CreateTask() is called with a taskPriority that is the
 * same as the priority of a task that is already in the task table, the
 * two tasks will end up in adjacent slots in the table. Which one is
 * called first by the scheduler is not specified.
 *****************************************************************************/
tsTaskID_t TS_CreateTask
  (
  tsTaskPriority_t taskPriority,                /* IN: Priority of new task. */
  pfTsTaskEventHandler_t pfTaskEventHandler     /* IN: Self explanatory. */
  );

/*****************************************************************************
 * TS_DestroyTask
 *
 * Remove a task from the kernel's task table.
 *****************************************************************************/
void TS_DestroyTask
  (
  tsTaskID_t taskId                     /* IN: Which task to destroy. */
  );

/*****************************************************************************
 * TS_Init()
 *
 * Initialize the kernel.
 *****************************************************************************/
void TS_Init( void );

/*****************************************************************************
 * TS_PendingEvents
 *
 * Checks for pending events for all the existing tasks. Returns true if there
 * is at least one pending event for at least one task.
 *****************************************************************************/
bool_t TS_PendingEvents( void );

/*****************************************************************************
 * TS_Scheduler
 *
 * Kernel main loop. Never returns once it is started.
 *****************************************************************************/
void TS_Scheduler( void );

/*****************************************************************************
 * TS_SendEvent
 *
 * Add events to a task's event flags.
 *****************************************************************************/
void TS_SendEvent
  (
  tsTaskID_t taskID,                    /* IN: Which task to send the event to. */
  event_t events                        /* IN: Event flag(s) to send. */
  );

#endif/* #ifndef _TS_INTERFACE_H_ */
