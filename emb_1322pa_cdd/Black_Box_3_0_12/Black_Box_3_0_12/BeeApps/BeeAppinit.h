/******************************************************************************
* BeeAppInit.h
*
* This file interfaces application with ZTC module.
*
* Copyright (c) 2008, Freescale, Inc. All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
******************************************************************************/

#ifndef _BEEAPPINIT_H_
#define _BEEAPPINIT_H_

#include "TS_Interface.h"
#include "TMR_Interface.h"
#include "MsgSystem.h"

/******************************************************************************
*******************************************************************************
* Public macros
*******************************************************************************
******************************************************************************/

/* Set gUserInterfaceIncluded_d TRUE to include the user interface. */
#define gUserInterfaceIncluded_d TRUE

#define DummyEndPoint 0xFF

enum {
	mStateIdle_c,
	mStateZDO_init_device_c,
	mStateZDO_Coordinator_starting_c,
	mStateZDO_stopped_c,
	mStateZDO_network_discovery_c,
	mStateZDO_join_network_c,
	mStateZDO_device_running_c,
	mStateZDO_device_unauth_c,
	mStateZDO_orphan_c,
 	mStateMatchDescRequest_c,
	mStateMatchDescSuccess_c,
  	mStateBindRequest_c,
  	mStateBindSuccess_c,
  	mStateUnBindRequest_c
};

typedef uint8_t UIMode_t;

enum {
  gConfigureMode_c,
  gApplicationMode_c,
  gAppSetting_c
};

/******************************************************************************
*******************************************************************************
* Public prototypes
*******************************************************************************
******************************************************************************/

/* mIdentifyTimerID_c is used for timer ID for timer of the Identify Mode. This constanst may NOT be changed */
/* #define mIdentifyModeOnTimerID_c 0x0b// gApplicationTimerIDOffset_c+2 */
extern tmrTimerID_t mIdentifyModeOnTimerID;

/* Interval time for Identify Mode On, which is used for Waiting in Identify Mode ON (0 - 262140ms) */
#define mIdentifyModeOnInterval_c 5000

void AppStartPolling(void);
void AppStopPolling(void);

/******************************************************************************
*******************************************************************************
* Public type definitions
*******************************************************************************
******************************************************************************/
/* make sure these don't overlapp the ZDO messages */
enum {
  	gRestartZDOStatemachine_c = 0x70,
	gStartNetwork_c,
  	gLeaveNetwork_c,
  	gBind_Device_c,
  	gUnbind_Device_c,
  	gMatchDescriptorSuccess_c,
  	gMatchNotFound_c,
  	gMatchFailure_c,
  	gLeaveMatchDescriptor_c,
  	gBindingSuccess_c,
  	gUnBindingSuccess_c,
  	gBindingFailure_c,
  	gUnBindingFailure_c,
  	gPermitJoinToggle_c,
  	gIdentifyOn_c,
  	gIdentifyOff_c,
  	gIdentifyToggle_c,
  	gScanningChannels_c,
  	gSwitchTypeMomentary,
  	gSwitchTypeToggle,
  	gSwitchActionOn,
  	gSwitchActionOff,
  	gSwitchActionToggle,
  	gThermostatCelsius,
  	gThermostatFahrenheit,
    gOTAProgressReportEvent_c
};

/******************************************************************************
*******************************************************************************
* Public memory declarations
*******************************************************************************
******************************************************************************/

/* TaskID of the main application task. */
#if gBeeStackIncluded_d == 1
extern tsTaskID_t gAppTaskID;

/* indication and confirm queues */
extern anchor_t gAppDataConfirmQueue;
extern anchor_t gAppDataIndicationQueue;
/* indication and confirm queues */
extern anchor_t gInterPanAppDataConfirmQueue;
extern anchor_t gInterPanAppDataIndicationQueue;

#endif  /* gBeeStackIncluded_d */

#endif  /*  _BEEAPPINIT_H_  */
