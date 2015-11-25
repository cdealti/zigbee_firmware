/******************************************************************************
* This is the header file for the Interface between NWK and ZDO.
*
* Copyright (c) 2008, Freescale, Inc. All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
******************************************************************************/

#ifndef _ZDONWKINTERFACE_H_
#define _ZDONWKINTERFACE_H_


#if ( gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d )
extern zbChannels_t gaScanChannels;
#endif

/******************************************************************************
*******************************************************************************
* Public macros
*******************************************************************************
******************************************************************************/
#ifdef __IAR_SYSTEMS_ICC__
#pragma pack(1)
#endif
/* zdo - Nlme Message */
typedef struct zdoNlmeMessage_tag
{
  uint8_t msgType;
  union
  {
    nlmeNetworkDiscoveryReq_t   networkDiscoveryReq;
    nlmeNetworkFormationReq_t   networkFormationReq;
    nlmePermitJoiningReq_t      permitJoiningReq;
    nlmeStartRouterReq_t        startRouterReq;
    nlmeJoinReq_t               joinReq;
    nlmeDirectJoinReq_t         directJoinReq;
    nlmeLeaveReq_t              leaveReq;
    nlmeSyncReq_t               syncReq;
    nlmeChagePollTimer_t        changePollTimer;
    nlmeRouteDiscoveryReq_t     routeDiscoveryReq;
    nlmeEnergyScanReq_t         EnergyScanReq; 
    nlmeSetChannelReq_t         SetChannelReq;
    nlmeAuthenticationCnf_t     AuthenticationCnf;
    nlmeResetReq_t              resetReq;
    networkStatusCommandReq_t   networkStatusReq;
    nwkConcDiscTimeReq_t        nwkConcDiscTimeReq;
  }msgData;
}zdoNlmeMessage_t;


/* nlme - Zdo Message */
typedef struct nlmeZdoMessage_tag
{
  uint8_t msgType;
  union 
  {
    nlmeNetworkDiscoveryConf_t  networkDiscoveryConf;
    nlmeNetworkFormationConf_t  networkFormationConf;
    nlmePermitJoiningConf_t     permitJoiningConf;
    nlmeStartRouterConf_t       startRouterConf;
    nlmeJoinIndication_t        joinIndication;
    nlmeJoinConf_t              joinConf;
    nlmeDirectJoinConf_t        directJoinConf;
    nlmeLeaveIndication_t       leaveIndication;
    nlmeLeaveConf_t             leaveConf;
    nlmeResetConf_t             resetConf;   /*not used*/
    nlmeSyncConf_t              syncConf;
    nlmeGetConf_t               getConf;     
    nlmeSetConf_t               setConf;  
    nlmeRouteDiscoveryConf_t    routeDiscoveryConf;
    uint8_t                     syncLossIndStatus;
    uint8_t                     errorStatus;
    nlmeEnergyScanCnf_t         EnergyScanConf;
    nlmeNwkStatusIndication_t   NetworkStatusIndication;
    nlmeNwkTxReport_t           networkTxReport;
    networkStatusCommandCnf_t   networkStatusCnf;
    nwkConcDiscTimeCnf_t        nwkConcDiscTimeCnf; 
    zbStatus_t                  securityConf;
  }msgData;
}nlmeZdoMessage_t;
#ifdef __IAR_SYSTEMS_ICC__
#pragma pack()
#endif

/* Status Values of Find Route Function Call */
enum
{
  gRouteFoundThroughRouteDiscovery_c  = 0x00,
  gRouteFoundThroughTreeRouting_c     = 0x01,
  gRouteFoundThroughMeshRouting_c     = 0x02,
  gRoutingErrorScenario_c             = 0x03,
  gRouteFoundThroughSourceRouting_c   = 0x04
};


/******************************************************************************
*******************************************************************************
* Public prototypes
*******************************************************************************
******************************************************************************/

/****************************************************************************
* This function is used to Check whether NWk Layer Queues are Empty.
*****************************************************************************/

bool_t NWK_AreNWKQueuesEmpty
(
  void
);

/****************************************************************************
* This is the Initialise NWK Layer function which intiialises the NVM Flash,
* NIB,Message Queues
*****************************************************************************/

void Nwk_Init( void );

/****************************************************************************
* This is the main function of the Network layer and this function calls 
* both the Data services and Management module
*****************************************************************************/
void NwkLayer_Main( 
  uint16_t events
);



/******************************************************************************
*******************************************************************************
* Public type definitions
*******************************************************************************
******************************************************************************/

/******************************************************************************
*******************************************************************************
* Public memory declarations
*******************************************************************************
******************************************************************************/
/* None */


/******************************************************************************
*******************************************************************************
* Public functions
*******************************************************************************
******************************************************************************/

/******************************************************************************
* This is a SAP handler function that handles all synchronous calls from Network
* to ZDO as well as Queueing up data sent to the ZDO Layer 
*
* Return value:
*   gZbSuccess_c , gInvalidParameter_c    
*   
******************************************************************************/
uint8_t NLME_ZDO_SapHandler
  (
  nlmeZdoMessage_t *pMsg
  );

/******************************************************************************
* This is a SAP handler function that handles all synchronous calls to Network
* as well as Queueing up data sent to the Nwk Layer 
*
* Interface assumptions:
*   All synchronous calls will not be Queued in the ZdoNwkInputQueue
*
* Return value:
*   gZbSuccess_c , gInvalidParameter_c    
******************************************************************************/
uint8_t ZDO_NLME_SapHandler
  (
  zdoNlmeMessage_t *pMsg
  );

/*****************************************************************************
* This function call sets a flag to notify whether the device has received
* the network key
******************************************************************************/

/******************************************************************************
* This function updates the poll rate from the current value
******************************************************************************/
void NWK_MNGTSync_ChangePollRate( uint16_t );

/******************************************************************************
* The stack must keep track of sleep state. If the stack is reset, it must 
* restore the proper number of "allowToSleep".
*
* Interface assumptions:
*   NONE
*
* Return value:
*   NONE    
******************************************************************************/
void ZDO_AllowDeviceToSleep(void);

/******************************************************************************
* The stack must keep track of sleep state. If the stack is reset, it must 
* restore the proper number of "allowToSleep".
*
* Interface assumptions:
*   NONE
*
* Return value:
*   NONE    
******************************************************************************/
void ZDO_DisallowDeviceToSleep(void);
/******************************************************************************
* This function starts the aging timer

* 
* Interface assumptions:
*   NONE.
*
* Return value:
*   NONE.
*******************************************************************************/
void StartCustomAgingNeighborTableTimer(void);
/******************************************************************************
* This function gets controll once the aging timeout is triggered, and ages
* the ticks on the NT once per each minute defined on the aging scale.
* 
* Interface assumptions:
*   NONE.
*
* Return value:
*   NONE.
*******************************************************************************/
void CustomAgingNeighborTableTimeOutCallBack
(
  tmrTimerID_t timerId  /* IN: The Id of the timer to be expired, may be needed or not.*/
);

/******************************************************************************
* This function sets back to default the scale value, and the function is
* needed, on the library, nwk layer  portion to reset this value whe Nwk gets
* restarted. Remember library code can not use define macros from the
* application portion.
*
* Interface assumptions:
*   NONE.
*
* Return value:
*   NONE.
*******************************************************************************/
void Nwk_ResetAgingTickScale(void);


/******************************************************************************
* Ages the neighbot table entries every time the function gets controll, For
* SP2, on routers and coordinators increase the value of the field "age" by
* one tick each time, once the device reach a defined limit, this same funciton
* marks them on two fields, on relationship it is set to "None", and on Outgoing
* cost it is set to Zero.
* On SP1, routers are expired based on time, just like ZED on SP1 and SP2, but,
* instead of increasing the value of the field "age", this gets decremented one
* tick at the time per each minute on the scale unitl it reaches zero, once the
* age field reaches zero then is consider has expired the Relationship is set to
* None, and for SP1 the reuse address field is set to TRUE.
* For Parents on SP1 and SP2, the LQI is set to 0 meaning bad LQI and outgoing
* cost is also set to zero.
*
* Interface assumption:
*   The parameter deviceTypeToAging is a valid ZigBee Device type.
*
* Return value:
*   NONE.
*******************************************************************************/
extern void AgingNeighborTable(zbDeviceType_t deviceTypeToAging);

/******************************************************************************
* Force the Nwk Rejoin if the limit of the Tx Failures or poll rewquest has
* been reached.
*
* Interface assumption:
*   NONE.
*
* Return value:
*   NONE.
*******************************************************************************/
#if gRouterCapability_d || gEndDevCapability_d || gComboDeviceCapability_d
extern void ZDO_ProcessDeviceNwkRejoin(void);
#endif


#if gStandardSecurity_d
/******************************************************************************
* This function is only called for all the packets with security off on a
* secure network, Depends on a global to allow packets, the relationship of
* the sending device and out own state, Data packet must be encrypted unless
* we allow for unsecure packet or come from a node getting authenticated or
* to us when we are in the way to be authenticated.
*
* Interface assumption:
*   The parameters pNTE and pNpduare not null pointers.
*
* Return vlue:
*   Success if the packet will be allow to reach the next higher layer.
*   Failure if the packet must be freed and will not reach the next higher layer.
*
*******************************************************************************/
zbStatus_t SSP_NwkVerifyUnsecurePacket(neighborTable_t *pNTE, npduFrame_t *pNpdu);
#endif


#endif  _ZDONWKINTERFACE_H_

