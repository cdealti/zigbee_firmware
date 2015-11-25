
/******************************************************************************
* This is a header file for Zdo Network Manager
*
*
* (c) Copyright 2005, Freescale, Inc. All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
*******************************************************************************/

#ifndef _ZdoNwkManager_h_
#define _ZdoNwkManager_h_

#include "ZdpManager.h"
/******************************************************************************
*******************************************************************************
* Public Macros
*******************************************************************************
******************************************************************************/
#define gDeviceDisabled_c 0x00;
#define gDeviceEnabled_c 0x01;

/******************************************************************************
*******************************************************************************
* Public Prototypes
*******************************************************************************
******************************************************************************/

#if gStandardSecurity_d || gHighSecurity_d
/************************************************************************************
* 053474r17 - Sec. 4.4.2.1 APSME-ESTABLISH-KEY.request
* The APSME-ESTABLISH-KEY request primitive is used for initiating a keyestablishment
* protocol. This primitive can be used when there is a need to securely communicate
* with another device.
* One device will act as an initiator device, and the other device will act as the
* responder. The initiator shall start the key-establishment protocol by issuing:
*
* - The APSME-ESTABLISH-KEY.request with parameters indicating the address of the
*   responder device.
*
* - An indication of which key-establishment protocol to use (currently SKKE).
*
* Interface assumptions:
*   The parameters aResponderAddress and aParentAddress are valid 64-bit addresses.
*   The parameter useParent is a boolena value (TRUE | FALSE).
*   The parameter keyEstablishmentMethod has a value from 0x00 to 0xff.
*
* Return value:
*   The status of the ZDO request.
************************************************************************************/
zbStatus_t ZDO_APSME_Establish_Key_request
(
  zbIeeeAddr_t  aResponderAddress,
  bool_t        useParent,
  zbIeeeAddr_t  aParentAddress,
  uint8_t       keyEstablishmentMethod
);
#else
#define ZDO_APSME_Establish_Key_request(aResponderAddress, useParent, aParentAddress, keyEstablishmentMethod) 0x00
#endif

#if gHighSecurity_d
/*******************************************************************************
* ZDO_APSME_Authenticate_request (053474r17ZB section 4.4.8.1.2)
*
* The ZDO on an initiator or responder device will generate this primitive when 
* it needs to initiate or respond to entity authentication. If the ZDO is 
* responding to an APSME-AUTHENTICATE.indication primitive, it will set the 
* RandomChallenge parameter to the corresponding RandomChallenge parameter in 
* the indication. The 16-octet random challenge originally received from the
* initiator. This parameter is only valid if the Action parameter is equal to 
* RESPOND_ACCEPT.
*
* Revison history:
*   Date   Author  Comments
*  ------  ------  --------
*  180408    BN    Created
*******************************************************************************/
zbStatus_t ZDO_APSME_Authenticate_request
(
  zbIeeeAddr_t  aPartnerAddress,
  zbAction_t action,
  uint8_t *pRandomChallenge
);
#else
#define ZDO_APSME_Authenticate_request(aPartnerAddress, action, pRandomChallenge)  0x00
#endif

/******************************************************************************
*******************************************************************************
* Public type definitions
*******************************************************************************
******************************************************************************/

/* NONE */

/******************************************************************************
*******************************************************************************
* Public Memory Declarations
*******************************************************************************
******************************************************************************/
//#pragma CONST_SEG BOOTLOADER_ZDO_NV_DATA

//pextern const beeStackConfigParams_t gBeeStackDefaultConfig;  

//#pragma CONST_SEG DEFAULT

extern uint8_t gCoordinatorBeaconOrder_c;
extern uint8_t gIndex;
extern bool_t  gMgmtNwkDiscReq;
extern bool_t  gOrphanScanTimerStatus;
extern bool_t  gInitialTry;
extern bool_t  gMemoryFreedByApplication;

/******************************************************************************
*******************************************************************************
* Public functions
*******************************************************************************
******************************************************************************/

/******************************************************************************
* This function in turn calls the other network primitives present in the
* network layer and also passes the parameters required for those primitives 
*
* Interface assumptions:
*   The Network layer discovery,formation,join,permitjoin and start router 
* requests are invoked from Zdo.The parameters for these requests are passed
* from this layer.
* The Zdp primitive requests and responses are handled in ZdpManager module
* 
* return value:
*   None 
*
******************************************************************************/
void ZDO_NwkManager
  (
  nlmeZdoMessage_t *pZdoMsg
  );

/******************************************************************************
* This function passes the Network Discovery Request parameters to the
* zdo network message queue
*
* Interface assumptions:
*   None
*   
* return value:
*   None 
*   
* Effects on global data:
*	  None
*
******************************************************************************/
void ZdoNwkMng_GenerateNwkDiscoveryReq
  (
  void
  );
  
/******************************************************************************
* This function passes the Network Formation Request parameters to the
* zdo network message queue
*
* Interface assumptions:
*   None
*   
* return value:
*   None 
*   
* Effects on global data:
*		None
*
******************************************************************************/
  
void ZdoNwkMng_GenerateNetworkFormation
  (
  void
  );

/******************************************************************************
* Calls the NWK layer with an NLME-JOIN.request.
*
* Interface assumptions:
*   Assumes a scan is complete and SearchForSuitableParentToJoin() can be called.
*   
* return value:
*   comes back on join.response
*   
* Effects on global data:
*		None
*
******************************************************************************/
void ZdoNwkMng_GenerateNwkJoinRequest ( zbNwkJoinMode_t joinMode );


/******************************************************************************
* This function passes the Network Start Router Request parameters to the
* zdo network message queue
*
* Interface assumptions:
*   None
*   
* return value:
*   None 
*   
* Effects on global data:
*		None
*
******************************************************************************/
  
void ZdoNwkMng_GenerateStartRouterReq
  (
  void
  );


/******************************************************************************
* This function is called when the time to establish a Nwk key expires.This funtion
* makes the device leave the Nwk.
*
* Interface assumptions:
*   None
*
* Return Value:
*   None 
*
* Effects on global data:
*   None
*
******************************************************************************/
#if gStandardSecurity_d || gHighSecurity_d
void ZDO_SecEstablishKeyTimeExpired
(
  void
);
#else
#define ZDO_SecEstablishKeyTimeExpired()
#endif

#if gHighSecurity_d
void ZDO_StartLinkStatus();
#endif

void ZdoNwkMng_GenerateNlmeLeaveRequest
(
  void
);

#if (gStandardSecurity_d || gHighSecurity_d) && !gZigbeeProIncluded_d
void ZdoNwk_ClearSiblingOnNeighborTable(ZdoEvent_t startMode);
#else
#define ZdoNwk_ClearSiblingOnNeighborTable(startMode)
#endif


/******************************************************************************
*******************************************************************************
* Interface Macro definitions
*******************************************************************************
******************************************************************************/

#endif _ZdoNwkManager_h_
