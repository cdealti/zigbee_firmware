/*****************************************************************************
* This is a main header file  for ZDO layer
*
* Copyright (c) 2008, Freescale, Inc. All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
******************************************************************************/

#ifndef _ZdoMain_h_
#define _ZdoMain_h_

#include "AppZdoInterface.h"
/**********************************************************************
***********************************************************************
* Public Macros
***********************************************************************
***********************************************************************/

/*ZDO/ZDP SapHandlers events*/
#define gAPSME_ZDO_c  (1<<0)
#define gNLME_ZDO_c   (1<<1)
#define gAPP_ZDP_c    (1<<2)

/*ZDO/ZDP timeout events*/
#define gTimeBetScansTimeOut_c   (1<<3)
#define gSendSecondBindResp_c    (1<<4)
#define gEndDevBindTimeOut_c     (1<<5)
#define gOrphanScanTimeOut_c     (1<<6)
#define gEstablishKeysTimeOut_c  (1<<7)
#define gEvntInitZdo_c           (1<<8)

/*
  ZDO/ZDP Security events.
*/
#define gJoinIndicationArrive_c    (1<<9)
#define gUpdateDeviceIndication_c  (1<<11)
#define gDeviceAllow_c             (1<<12)
#define gDeviceForbiden_c          (1<<13)
#define gDone_c                    (1<<14)

/*
  ZDO/ZDP Trust Center state machine states.
*/
#define gIdleState_c               0x00
#define gWaitForProcessingState_c  0x01
#define gDoneState_c               0x02

#define gZDONVMChanged    gZDONVM_Flag


/**********************************************************************
***********************************************************************
* Public Prototypes
***********************************************************************
***********************************************************************/

#if gStandardSecurity_d || gHighSecurity_d

/*
  Global and private prototypes for security goes here.
*/

#if gTrustCenter_d || gComboDeviceCapability_d
/*
  Global and private memory declarations for security, exclusive for the
  Trust Center goes here.
*/

void ZDO_SendAuthenticationToTrustCenter
(
  bool_t allow
);

/************************************************************************************
* Register a device Long address and Short address into the address map, and for the
* Link key security, also add it to the Device key Set.
*
* The function only works for the trust center.
*
* Interface assumptions:
*   The parameter aExtAddr is a valid 64-bit address.
*   The parameter aNwkAddr is a valid 16-bit address.
*
* Return value:
*   NONE.
*
* Revison history:
*   Date   Author  Comments
*  ------  ------  --------
*  210408    MN    Created
************************************************************************************/
void ZDO_SecAddDeviceInfo
(
  zbIeeeAddr_t  aExtAddr,  /* IN: The long address of the device to register. */
  zbNwkAddr_t   aNwkAddr   /* IN: The short address of the device to register. */
);

#else
#define ZDO_SendAuthenticationToTrustCenter(allow)
#endif
#endif


/***************************************************************************
* This function is called whenever an event has occured
* 
* Tells ZDO to generate a system event when ZDO state changes (e.g. joins 
* or leaves network).
****************************************************************************/
#if gSystemEventEnabled_d == 1
void Zdo_GenerateEventZdo2App
( 
SystemEvents_t event 
) ;
#else

#define Zdo_GenerateEventZdo2App( event )

#endif /*gSystemEventEnabled_d*/


/******************************************************************************
* Called whenever ZDO has an event for the app.
******************************************************************************/
uint8_t APP_ZDP_SapHandler
  ( 
  appToZdpMessage_t *pMsg /* IN - pointer to msg sent from APP to ZDP*/ 
  );


/******************************************************************************
* SAP Handler for NWK to send management service indication / confirmation
******************************************************************************/
uint8_t NLME_ZDO_SapHandler
(
nlmeZdoMessage_t *pMsg
);

/***************************************************************************
* SAP Handler for APSME to send primitives to ZDO
* This is required only if security is implemented because
* Bind, unbind and updating address map table are sync calls
******************************************************************************/
uint8_t APSME_ZDO_SapHandler
(
  zbApsmeZdoIndication_t *pMsg 
);


/*****************************************************************************
* This is the call back function provided to the timer module which gets 
* called when time out happens. This posts an event to SSP when time out 
* happnes. 
*
*
* Interface assumptions:
*
*   None
* Return value:
*   None
*
******************************************************************************/  
void ZDO_TimeOutHandler
  ( 
  uint8_t timerId /* IN : Timer Id that expired */
  );

/*
  Reset the ZDO task. May be called at any time.
*/
void ZdoReset( void );


/**********************************************************************
***********************************************************************
* Public type definitions
***********************************************************************
***********************************************************************/

/* None */

/**********************************************************************
***********************************************************************
* Public Memory Declarations
***********************************************************************
***********************************************************************/ 
extern bool_t gTimerStatus ;

/**********************************************************************
***********************************************************************
* Interface Macro Declarations
***********************************************************************
***********************************************************************/

/*None*/

#endif _ZdoMain_h_
