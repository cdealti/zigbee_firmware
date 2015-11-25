/******************************************************************************
* This is a global header file for the APS/ZDO interface.
*
* Author(s): Drew Gislason, Masaru Natsu
*
* (c) Copyright 2005-2008, Freescale, Inc.  All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
******************************************************************************/

#ifndef _ZDO_APS_INTERFACE_H_
#define _ZDO_APS_INTERFACE_H_

#ifndef __IAR_SYSTEMS_ICC__
#ifdef MC13226Included_d
    #undef MC13226Included_d
#endif    
    #define MC13226Included_d 0
#endif
#include "AppZdoInterface.h"
#include "BeeStack_Globals.h"
#include "BeeStackInterface.h"

/******************************************************************************
*******************************************************************************
* Public Macros
*******************************************************************************
******************************************************************************/

/* Security Challenge Domain Patameters */
#define gMaxChallengeLength_c    128
#define gMinChallengeLength_c    128


// Indicates the state machine should go on to the next state, as some event has happened
// See gZdoStartMode and gZdoStopMode;
#define gZDO_StateEventOccurred_c		( 1<<0 )


// various ZDO events (that move from one state to another
typedef uint8_t ZdoEvent_t;
enum {
  gZdoEvent_Unused_c,             // indicates event has been processed

  // general events for any state
  gZdoEvent_Timeout_c,            // everall operation has timed out
  gZdoEvent_BetweenDiscoveryTime_c, // between discovery attempts

  // events for initial state
  gZdoEvent_Start_c,              // time to start the network (only valid on InitialState)

  // events for discover network state
  gZdoEvent_StartingDiscovery_c,  // startup up the discovery state
  gZdoEvent_EnergyScan_c,         // ZC only, do energy scan
  gZdoEvent_EnergyScanComplete_c, // scan is complete 
  gZdoEvent_NextDiscovery_c,      // do the next network discovery (aka active scan)
  gZdoEvent_DiscoveryFailure_c,
  gZdoEvent_DiscoverySuccess_c,

  // events for forming state
  gZdoEvent_Forming_c,            // forming the network
  gZdoEvent_FormationFailure_c,
  gZdoEvent_FormationSuccess_c,
  gZdoEvent_StartRouterSuccess_c, // used for both forming and joining, turns on the radio
  gZdoEvent_StartRouterFailure_c,

  // events for joining state
  gZdoEvent_Joining_c,            // joining network
  gZdoEvent_JoinSuccess_c,
  gZdoEvent_JoinFailure_c,

  // events for authenticating state
  gZdoEvent_AuthenticateStart_c,    // start authenticating process
  gZdoEvent_AuthenticationFailure_c,
  gZdoEvent_AuthenticationSuccess_c,
  gZdoEvent_Authenticate_c,       // authenticating on network
  gZdoEvent_LeaveNetwork_c,       // leaving network (from the top)
  gZdoEvent_SelfLeaveSuccess_c,   // leaving network (from the bottom or ZTC)
  gZdoEvent_ChildLeaveSuccess_c,
  gZdoEvent_ManagementCommand_c,
  gZdoEvent_ManagementResponse_c,
  gZdoEvent_Stop_c,                   // time to stop or leave (see gZdoStopMode)
  gZdoEvent_LeaveSuccess_c,
  gZdoEvent_StopSuccess_c,            // done stopping, back to initial state
  gZdoEvent_StartSuccess_c,           // started (happens once on form or join)
  gZdoEvent_FindAndRejoinNetwork_c,   // for FA, finding new channel
  gZdoEvent_NetworkRejoin_c,          /* Rejoin without discovering the network. */
};

extern ZdoEvent_t gZdoEvent;
#define ZDO_GetEvent()  ( gZdoEvent )
void ZDO_SendEvent(ZdoEvent_t event);

/*
  start modes (see R17, section 2.5.5.5)
  DDDx xxxx   - Device type
  xxxS Sxxx   - Startup set
  xxxx xCCC   - Startup control mode
*/
typedef uint8_t ZdoStartMode_t;

/* choose one of these, or leave field 0 to use previous setting (ignored if not a combo device) */
#define gZdoStartMode_Zc_c     0xc0 // start as ZC (combo device only)
#define gZdoStartMode_Zr_c     0x80 // start as ZR (combo device only)
#define gZdoStartMode_Zed_c    0x20 // start as ZED (combo device only)
#define gZdoStartMode_ZedRx_c  0x60 // start as RxOnIdle=TRUE ZED (combo device only)
#define gZdoStartMode_ComboMask_c 0xe0  // mask for above fields

/* choose one of these */
#define gZdoStartMode_NvmSet_c  0x00  // copy NVM set (if any) to working set, then start. If no NVM, use ROM set.
#define gZdoStartMode_RomSet_c  0x08  // copy ROM set to working set (factory defaults), then start
#define gZdoStartMode_RamSet_c  0x10  // use working startup set in RAM.
#define gZdoStartMode_SasSet_c  0x18  // copy commissioning cluster set to working set, then start. If not valid, use NVM set.
#define gZdoStartMode_SetMask_c 0x18  // mask for above fields

/* choose one of these (see also NLME-JOIN.request and zbNwkJoinMode_t in ZigBee.h) */
/* the order of these must match NLME-JOIN.request */
#define gZdoStartMode_Associate_c     0x00 // (default) use association (ZR, ZED only), or form (ZC)
#define gZdoStartMode_OrphanRejoin_c  0x01 // FS specific: use orphan rejoin (ZR, ZED only)
#define gZdoStartMode_NwkRejoin_c     0x02 // use NWK rejoin (ZR, ZED only)
#define gZdoStartMode_FindAndRejoin_c 0x03 // valid for ZR, ZED only, search for network on this and 
                                           // other channels, then silent join
#define gZdoStartMode_SilentStart_c   0x04 // already part of the network (no form/join needed)
#define gZdoStartMode_StartMask_c     0x07 // mask for above fields

// form (ZC) or associate join (if ZR or ZED), using NVM if available
#define gZdoStartMode_Default_c       (gZdoStartMode_Associate_c | gZdoStartMode_NvmSet_c)

extern ZdoEvent_t gZdoStartMode;
#define ZDO_GetStartMode()            (gZdoStartMode & gZdoStartMode_StartMask_c)
#define ZDO_SetStartMode(mode)        (gZdoStartMode = (gZdoStartMode & (~gZdoStartMode_StartMask_c)) | (mode))
#define ZDO_GetPreferredDevType()     (gZdoStartMode & gZdoStartMode_ComboMask_c)
#define ZDO_SetPreferredDevType(type) (gZdoStartMode = (gZdoStartMode & (~gZdoStartMode_ComboMask_c)) | (type))
#define ZDO_GetStartupSet()           (gZdoStartMode & gZdoStartMode_SetMask_c)
#define ZDO_SetStartupSet(set)        (gZdoStartMode = (gZdoStartMode & (~gZdoStartMode_SetMask_c)) | (set))
#define ZDO_GetRejoinMode(mode)       ((mode) & gZdoStartMode_StartMask_c)

/* start the network (does nothing if network already started or in progress) */
void ZDO_Start(ZdoStartMode_t startMode);

/* this can be used after a node has joined the network to determine what mode it joined with. */
/* not stored in NVM, so not valid after system has reset */
/* see zbNwkJoinMode_t in ZigBee.h for possible join modes */
#define ZDO_GetJoinMode() gZDOJoinMode
extern zbNwkJoinMode_t gZDOJoinMode;

/*
  stop mode bit mask for ZDO_StopEx()

  xxxx xxxA   announce (leave) before stopping
  XXXX XXCX   remove children if announce leave is active.
  Nxxx xxxx   reset NVM
  xRxx xxxx   restart after stopping
  xxBx xxxx   don't reset binding and group tables
*/
typedef uint8_t ZdoStopMode_t;
/* choose of these */
#define gZdoStopMode_Stop_c             0x00    /* stops immediately */
#define gZdoStopMode_Announce_c         0x01    /* (A)does an NLME-LEAVE (OTA) before stopping */
#define gZdoStopMode_RemoveChildren_c   0x02    /* (C)remove all your children when Announce stop is active. */
#define gZdoStopMode_ResetTables_c      0x20    /* (B)resets binding/group tables/channel, etc... back to ROM settings */
#define gZdoStopMode_StopAndRestart_c   0x40    /* (R)Restarts the node after stopping. */
#define gZdoStopMode_ResetNvm_c         0x80    /* (N)simply writes to NVM after stopping */

extern ZdoStopMode_t gZdoStopMode;

/* or chose the default */
#define gZdoStopMode_Default_c      gZdoStopMode_Announce_c    /* stop after announcing */

/*  */

/* stop the network, allowing user to chose extra modes compared to the old stop and leave */
void ZDO_StopEx(ZdoStopMode_t stopMode);

/* stop the network. Can be restarted in silent start mode. */
void ZDO_Stop(void);

/* leave network announcing that we're leaving. Can't be restarted in silent mode. */
void ZDO_Leave(void);

/* ZDO has fully stopped the network (all done) */
#define ZDO_Stopped() (gZdoState == gZdoState_InitialState_c)

/* ZDO has fully started the network (all done) */
#define ZDO_Started() (gZdoState == gZdoState_RunningState_c)

/* start and stop macros supported for backward compatibility */
#define gStartWithOutNvm_c                ( gZdoStartMode_RamSet_c )
#define gStartAssociationRejoinWithNvm_c  ( gZdoStartMode_Associate_c | gZdoStartMode_NvmSet_c )
#define gStartOrphanRejoinWithNvm_c			  ( gZdoStartMode_OrphanRejoin_c | gZdoStartMode_NvmSet_c )
#define gStartNwkRejoinWithNvm_c				  ( gZdoStartMode_NwkRejoin_c | gZdoStartMode_NvmSet_c )
#define gStartSilentRejoinWithNvm_c			  ( gZdoStartMode_SilentStart_c | gZdoStartMode_NvmSet_c )
#define gStartSilentRejoinWithOutNvm_c    ( gZdoStartMode_SilentStart_c )
#define gStartSilentNwkRejoin_c   			  ( gZdoStartMode_FindAndRejoin_c )
#define gStop_c                           ( gZdoStopMode_Stop_c )
#define gAnnceStop_c                      ( gZdoStopMode_Announce_c )


/* States for the ZDO state machine */
enum
{
  gZdoInitialState_c,
  gZdoDiscoveringNetworksState_c,
  gZdoFormingState_c,               /* (forming) only valid for ZC */
  gZdoJoiningState_c,               /* (joining) only valid for ZR, ZED */
  gZdoOrphanJoiningState_c,         /* (orphan joining) only valid for ZR, ZED */
  gZdoCoordinatorRunningState_c,    /* only valid for ZC */
  gZdoRouterRunningState_c,         /* only valid for ZR */
  gZdoEndDeviceRunningState_c,      /* only valid for ZED */
  gZdoRunningState_c,               /* device is running (any device type) */
  gZdoDeviceAuthenticationState_c,  /* only valid for ZR, ZED */
  gZdoRemoteCommandsState_c,
  gZdoDeviceWaitingForKeyState_c,   /* Added for LinkKeys */
  gZdoLeavingState_c,               /* telling others we are leaving */
  gZdoStoppingState_c               /* stopping (and resetting) */
};
typedef uint8_t ZdoState_t;
#if MC13226Included_d
    extern volatile ZdoState_t gZdoState;
#else
    extern ZdoState_t gZdoState;
#endif


#define ZDO_GetState()  ( gZdoState )
#define ZDO_SetState(state) ( gZdoState = (state) )
bool_t ZDO_IsRunningState(void);
bool_t ZDO_IsReadyForPackets(void);


typedef uint8_t zdoNvmObject_t;
// save on interval
#define zdoNvmObject_RoutingTable_c     0x01
#define zdoNvmObject_NeighborTable_c    0x02
#define zdoNvmObject_AddrMap_c          0x03
#define zdoNvmObject_GroupTable_c       0x04
#define zdoNvmObject_BindingTable_c     0x05
#define zdoNvmObject_ApsLinkKeySet_c    0x06  // APS device key pair set.
#define zdoNvmObject_NwkData_c          0x07  // PIB, permitjoining, etc..
#define zdoNvmObject_ZclData_c          0x08
// save on idle
#define zdoNvmObject_All_c              0x09
// save on count
#define zdoNvmObject_SecurityCounters_c 0x0A
#define zdoNvmObject_SecureSet_c        0x0B
#define zdoNvmObject_AtomicSave_c       0x0C  // All data set, no waiting.
#define zdoNvmObject_AddrMapPermanent_c 0x0D


/******************************************************************************
*******************************************************************************
* Public macros
*******************************************************************************
******************************************************************************/

#if gArm7ExtendedNVM_d
extern bool_t ZDO_NvmRestoreAllData(void);
#else
extern void ZDO_NvmRestoreAllData(void);
#endif

/************************************************************************************
* Builds a set of 16 octets made of random values.
*
* Interface assumptions:
* The parameter pChallenge is a not null pointer.
*
* Return value:
* NONE.
*
* Revison history:
* Date Author Comments
* ------ ------ --------
* 180408 MN Created
************************************************************************************/
extern void SSP_ChallengeGenPrimitive
(
uint8_t *pChallenge
);


/************************************************************************************
* Given an IEEE address, search in the whole array of state machines for an entry
* holding that particular IEEE address as the remote device, the remote device field
* is the only one unique.
*
* Interface assumptions:
*   The incoming parameter pRemoteAddress is not a null pointer.
*
* Return value:
*   The memory address of the current state machine data.
*
* Revison history:
*   Date   Author  Comments
*  ------  ------  --------
*  210408    MN    Created
************************************************************************************/
extern zbKeyEstablish_t  *SSP_ApsRetriveStateMachineEntry
(
  uint8_t  *pRemoteAddress
);

/************************************************************************************
* Given an IEEE address, search in the whole array of state machines for an entry
* holding that particular IEEE address and when it finds it delete it.
*
* Interface assumptions:
*   The incoming parameter pRemoteAddress is not a null pointer.
*
* Return value:
*   NONE.
*
* Revison history:
*   Date   Author  Comments
*  ------  ------  --------
*  210408    MN    Created
************************************************************************************/
extern void SSP_ApsRemoveStateMachineEntry
(
  uint8_t  *pRemoteAddress
);

/************************************************************************************
* Takes the full array of state machines used for SKKE, and one by one frees each of
* the available entries.
*
* Interface assumptions:
*   NONE.
*
* Return value:
*   NONE.
*
* Revison history:
*   Date   Author  Comments
*  ------  ------  --------
*  210408    MN    Created
************************************************************************************/
extern void SSP_ApsResetSKKEStateMachine
(
  void
);


extern zbKeyEstablish_t *SSP_ApsRegisterSKKEStateMachine
(
  zbIeeeAddr_t  aResponderAddress,
  zbIeeeAddr_t  aInitiatorAddress,
  zbIeeeAddr_t  aParentAddress,
  bool_t  useParent,
  bool_t  sendTrasnportKey
);

void ZdoNwkMng_SaveToNvm(zdoNvmObject_t iNvmObject);


/******************************************************************************
*******************************************************************************
* Public type definitions
*******************************************************************************
******************************************************************************/


/* used by ZDO_APSME_SapHandler() */
enum {
  gApsmeBindReq_c,                                                /* 0x00 */
  gApsmeGetReq_c                = 0x03,                           /* 0x03 */
  gApsmeSetReq_c                = 0x06,                           /* 0x06 */
  gApsmeUnbindReq_c             = 0x09,                           /* 0x09 */
  gApsmeReset_c                 = 0x0b,                           /* 0x0b */
  gApsmeAddGroupReq_c           = gZdoApsmeAddGroupReq_c,         /* 0x10 */
  gApsmeRemoveGroupReq_c        = gZdoApsmeRemoveGroupReq_c,      /* 0x11 */
  gApsmeRemoveAllGroupsReq_c    = gZdoApsmeRemoveAllGroupsReq_c,  /* 0x12 */

  gApsmeEstablishKeyReq_c       = gZdoApsmeEstKeyReq_c,           /* 0xcf */
  gApsmeEstablishKeyRsp_c       = gZdoApsmeEstKeyResp_c,          /* 0xd0 */
  gApsmeTransportKeyReq_c       = gZdoApsmeTranspKeyReq_c,        /* 0xd1 */
  gApsmeRequestKeyReq_c         = gZdoApsmeRequestKeyReq_c,       /* 0xd3 */
  gApsmeSwitchKeyReq_c          = gZdoApsmeSwitchKeyReq_c,        /* 0xd4 */
  gApsmeRemoveDeviceReq_c       = gZdoApsmeRemoveDeviceReq_c,     /* 0xf4 */
  gApsmeUpdateDeviceReq_c       = gZdoApsmeUpdateDevReq_c,        /* 0xf3 */
  gApsmeAuthenticateReq_c       = gZdoApsmeAuthenticateReq_c,     /* 0xf5*/
  gApsmeRedirectReq_c           = gZdoApsmeRedirectReq_c,         /* 0xfa */
  gApsmeTunnelReq_c             = gZdoApsmeTunnelReq_c,           /* 0xfb */
  gApsmeEaInitMacData_c         = gZdoApsmeEaInitMacData_c,       /* 0xf8 */
  gApsmeEaRspMacData_c          = gZdoApsmeEaRspMacData_c         /* 0xf9 */
};

typedef uint8_t primZdoToApsme_t;

#ifdef __IAR_SYSTEMS_ICC__
#pragma pack(1)
#endif

typedef union apsmePayload_tag
{
  zbApsmeSKKECommand_t  skke;
  zbApsmeTransportKeyCommand_t  transportKey;
  zbApsmeMacDataCommand_t         macData;
  zbApsmeTunnelCommand_t   tunnel;
}apsmePayload_t;

typedef struct zbApsmeRedirectCmdReq_tag
{
  zbApsCommandID_t  commandId;
  zbIeeeAddr_t      aDestAddress;
  uint8_t           payloadSize;
  apsmePayload_t    payload;
}zbApsmeRedirectCmdReq_t;

typedef union msgReq_tag
{
  zbApsmeTransportKeyReq_t  transportKeyReq;
}msgReq_t;

typedef struct zbApsmeTunnelReq_tag
{
  primZdoToApsme_t msgType;
  msgReq_t  msgData;
}zbApsmeTunnelReq_t;

/*-----------------------------------------------------------------------------
-------------------------------------------------------------------------------
  Different key to be transported
-------------------------------------------------------------------------------
-----------------------------------------------------------------------------*/


  /* ZDO_APSME_SapHandler() messages */
typedef struct apsmeMessage_tag {
  primZdoToApsme_t msgType;
  union {
    zbApsmeBindReq_t             bindReq;    /* 0x00 */
    zbApsmeUnbindReq_t           unbindReq;
    zbApsmeAddGroupReq_t         addGroupReq;
    zbApsmeRemoveGroupReq_t      removeGroupReq;
    zbApsmeRemoveAllGroupsReq_t  removeAllGroupsReq; /* 0x0D */
    zbApsmeTransportKeyReq_t     transportKeyReq;
    zbApsmeSwitchKeyReq_t        switchKeyReq;
    zbApsmeUpdateDeviceReq_t     updateDeviceReq;
    zbApsmeRemoveDeviceReq_t     removeDeviceReq;
    zbApsmeRequestKeyReq_t       requestKeyReq;
    zbApsmeEstablishKeyReq_t     establishKeyReq;
    zbApsmeEstablisKeyRsp_t      establishKeyRsp;
    zbApsmeRedirectCmdReq_t      redirectReq;
    zbApsmeAuthenticateReq_t     authenticateReq;
    zbResetType_t                resetTypeReq;
    zbApsmeTunnelReq_t           tunnelReq;
  } msgData;
} apsmeMessage_t;
#ifdef __IAR_SYSTEMS_ICC__
#pragma pack()
#endif

  /* ZDO_APSME_SapHandler() message types */
enum
{
  gApsmeEstablishKeyInd_c = gZdoApsmeEstKeyInd_c,
  gApsmeEstablishKeyCnf_c = gZdoApsmeEstKeyConf_c,
  gApsmeRemoveDeviceInd_c = gZdoApsmeRemoveDevInd_c,  /* 0xD8 */
  gApsmeRemoveDeviceCnf_c = gZdoApsmeRemoveDevCnf_c,
  gApsmeRequestKeyInd_c   = gZdoApsmeReqKeyInd_c,
  gApsmeRequestKeyCnf_c   = gZdoApsmeReqKeyCnf_c,
  gApsmeSwitchKeyInd_c    = gZdoApsmeSwitchKeyInd_c,
  gApsmeSwitchKeyCnf_c    = gZdoApsmeSwitchKeyCnf_c,
  gApsmeTransportKeyInd_c = gZdoApsmeTranspKeyInd_c,
  gApsmeTransportKeyCnf_c = gZdoApsmeTranspKeyCnf_c,
  gApsmeUpdateDeviceInd_c = gZdoApsmeUpdateDevInd_c,
  gApsmeUpdateDeviceCnf_c = gZdoApsmeUpdateDevCnf_c,
  gApsmeAuthenticateInd_c = gZdoApsmeAuthenticateInd_c,
  gApsmeAuthenticateCnf_c = gZdoApsmeAuthenticateConf_c,
  gApsmeAddGroupCnf_c     = gZdoApsmeAddGroupCnf_c,
  gApsmeTunnelCnf_c       = gZdoApsmeTunnelCnf_c,
  gApsmeProcessSecureFrameConf_c = gZdoApsmeProcessSecureFrame_c,
  gApsmeRemoveGroupCnf_c,
  gApsmeRemoveAllGroupsCnf_c
};



/* APS PIB attributes */

enum {
  gAPibAddressMap_c                     = 0xc0, /* zigbee */
  gAPibBindingTable_c                   = 0xc1, /* zigbee */
  gAPibAddressMapCurrent_c              = 0xc2, /* zigbee */
  gAPibBindingTableCurrent_c            = 0xc3,
  gAPibDeviceKeyPairSet_c               = 0xaa,
  gAPibTrustCenterAddress_c             = 0xab,
  gAPibSecurityTimeOutPeriod_c          = 0xac,
  gAPibDeviceKeyPairCurrent_c           = 0xad
};

enum{
  SKKEStatusSuccess          ,
  SKKEStatusInvalidParameter ,
  SKKEStatusNoMasterKey      ,
  SKKEStatusInvalidChallenge ,
  SKKEStatusInvalidSKG       ,
  SKKEStatusInvalidMAC       ,
  SKKEStatusInvalidKey 			 ,
  SKKEStatusTimeOut 				 ,
  SKKEStatusBadFrame
} ;

/******************************************************************************
*******************************************************************************
* Public memory declarations
*******************************************************************************
******************************************************************************/

#if MC13226Included_d
/* # of address map entries */
extern index_t gApsMaxAddressMapEntries;

/* # of binding entries */
#if gBindCapability_d
extern index_t gApsMaxBindingEntries;
#endif
#else
/* # of address map entries */
extern const index_t gApsMaxAddressMapEntries;

/* # of binding entries */
#if gBindCapability_d
extern const index_t gApsMaxBindingEntries;
#endif
#endif




/******************************************************************************
*******************************************************************************
* Public functions
*******************************************************************************
******************************************************************************/

/* SAP handlers between ZDO and APSME (both directions: from_to) */
uint8_t ZDO_APSME_SapHandler(apsmeMessage_t *pMsg);
uint8_t APSME_ZDO_SapHandler(zbApsmeZdoIndication_t *pMsg);



/* Entry point to the APS */
void TS_ApsTask(uint16_t events);

/* Initilizes the msg Queues managed by the APS */
void TS_ApsTaskInit(void);


/* is APS done with all of it's queues? Use for low-power */
bool_t APS_AreQueuesEmpty(void);

/************************************************************************************
* 4.4.3.1 APSME-TRANSPORT-KEY.request
* The APSME-TRANSPORT-KEY.request primitive is used for transporting a key to
* another device.
*
* 4.4.3.1.1 Semantics of the Service Primitive This primitive shall provide the
* following interface:
*   APSME-TRANSPORT-KEY.request {
*                               DestAddress,
*                               KeyType,
*                               TransportKeyData
*                               }
*
* The ZDO on an initiator device shall generate this primitive when it requires
* a key to be transported to a responder device.
*
* The receipt of an APSME-TRANSPORT-KEY.request primitive shall cause the APSME to
* create a transport-key command packet (see 053474r17 sub-clause 4.4.9.2).
*
* Interface assumptions:
*   The incoming parameter pTransportKeyData is not a null pointer.
*   The KeyType has a value between 0x00 and 0x04.
*   The parameter aDestinationAddress is a valid 64-bit address.
*
* Return value:
*   NONE.
*
************************************************************************************/
extern void ZDO_APSME_Transport_Key_request
(
  zbIeeeAddr_t           aDestinationAddress,
  zbKeyType_t            keyType,
  zbTransportKeyData_t  *pTransportKeyData
);


/************************************************************************************
* Reset the APS security materila to get the defualts.
*
* On a trust center, establish the predefined key (Master or Application) into each
* each entry of the security material set, given the possibility to every joining
* device the chance to share at least the key needed to communicate with the trust center.
* On a regular device (Non trust center), Stores the TC information into the entry zero
* of the set.
*
* Interface assumptions:
*   NONE.
*
* Return value:
*   NONE.
*
* Revison history:
*   Date   Author  Comments
*  ------  ------  --------
*  210408    MN    Created
************************************************************************************/
extern void SSP_ApsResetSecurityMaterial
(
  void
);

/************************************************************************************
* Given an IEEE address, search in the whole array of security material for the index
* holding the device info related to the given IEEE address.
*
* Interface assumptions:
*   The incoming parameter aDeviceAddress is valid 64-bit address.
*
* Return value:
*   The index on in the security material set where the secure material related to the
    IEEE Address is hold, in the case of not findin the info the funtion will return
    gNotInAddressMap_c.
*
************************************************************************************/
extern uint8_t  SSP_ApsGetIndexFromIEEEaddress
(
  zbIeeeAddr_t  aDeviceAddress
);

/* This clear the APS security material, must be use when a node leaves. */
extern void SSP_ApsRemoveSecurityMaterialEntry
(
  uint8_t *pDeviceAddress
);

extern void SSP_ResetDeviceCounters(uintn8_t *pIEEEAddress);

extern void SSP_ApsRegisterLinkKeyData
(
  zbIeeeAddr_t  aDeviceAddress,
  zbKeyType_t  keyType,
  uint8_t  *pKey
);

extern bool_t SSP_ApsIsLinkEstablished
(
  uint8_t  *pDevAddress
);

/*******************************************************************************
* challengeIsAccepted (053474r17ZB section B.4 Challenge Validation Primitive)
*
* This function is used to check whether a challenge to be used by a scheme in 
* the standard has sufficient length (e.g., messages that are too short are 
* discarded, due to insufficient entropy).Returning TRUE or FALSE.
*
* IN:aChanllenge set of 16 bytes that has a random challenge sent by the 
*                initiator of the authenticate process.
*******************************************************************************/
#if gHighSecurity_d
extern bool_t SSP_ChallengeIsAccepted
(
  uint8_t   sizeOfChallenge
);

extern void SSP_ApsRemoveEntityAuthenticationStateMachineEntry
(
  uint8_t *pPartnerAddress
);

extern zbEntityAuthentication_t  *SSP_ApsRetriveEntityAuthenticationStateMachineEntry
(
  uint8_t  *pPartnerAddress
);

extern void SSP_ApsResetEntityAuthenticationStateMachine
(
  void
);

#endif

#if gHighSecurity_d
/************************************************************************************
* Given the 64-bit address of a device search for it in the NT, if it find it, this
* means that  we need to search for it on the security material set, if the device is
* found this mean that we know about the device previously.
*
* Interface assumptions:
*   The parameters aDeviceAddr is a valid 64-bit address.
*
* Return value:
*   TRUE if the information is previously registered on the security material set.
*   FALSE if the we do not know anything about the device.
*
************************************************************************************/
bool_t SSP_NwkGotSecurityMaterial
(
  zbIeeeAddr_t  aDeviceAddress
);
#endif

#if gStandardSecurity_d || gHighSecurity_d
/************************************************************************************
* Sets the given key data into the inactive security material set, will work for
* APS and Nwk.
*
* Interface assumptions:
*   NONE.
*
* Return value:
*   Success if the register action was completed, Failed otherwise.
************************************************************************************/
zbStatus_t  SSP_RegisterKeyData
(
  zbKeyType_t  keyType,
  zbTransportKeyData_t  *pKeyInfo
);

void ZDO_SecRegisterLinkOrMastreKeyInd
(
  zbApsmeTransportKeyInd_t  *pKeyIndication
);
#endif

/************************************************************************************
* Generate a Nlme request to set the given time in millisecond as the current poll
* rate.
*
* Interface assumptions:
*   The parameter pollTime is value grather than zero.
*
* Return value:
*   The status of the request sended to the NLME.
************************************************************************************/
zbStatus_t ZDO_NLME_ChangePollRate
(
  uint16_t  pollTime
);

/*****************************************************************************************
                                APS SECURE CODE
******************************************************************************************/

/* This clear the APS security material, must be use when a node leaves. */
void APS_RemoveSecurityMaterialEntry
(
  uint8_t *pExtendedAddress
);

void APS_ResetDeviceCounters(uint8_t *pIEEEAddress);


/************************************************************************************
* Reset the APS security materila to get the defualts.
*
* On a trust center, establish the predefined key (Master or Application) into each
* each entry of the security material set, given the possibility to every joining
* device the chance to share at least the key needed to communicate with the trust center.
* On a regular device (Non trust center), Stores the TC information into the entry zero
* of the set.
*
* Interface assumptions:
*   NONE.
*
* Return value:
*   NONE.
*
************************************************************************************/
void APS_ResetSecurityMaterial
(
  void
);

void APS_SetSecureTxOptions
(
  zbApsTxOption_t  *pTxOptions,
  zbApsCommandID_t  cmdID,
  uint8_t *pDestAddress
);

/************************************************************************************
* Get the status of the security procedure, as well as the secure payload when the
* TxOptions requires it. If the APS layer has a frame, consisting of a header
* ApsHeader and Payload, that needs security protection and nwkSecurityLevel > 0,
* it shall apply security as descibed in this function.
*
* Interface assumptions:
*   The Aps header and the Aps payload are a contiguos set of bytes, in the way of
*   [ ApsHeader||ApsPayload ].
*   The incoming parameters pApsHeader, pApsFrameLength are not null pointers.
*   The memory buffer that holds the ApsHeader and Aps Payload is big enough to hold
*   the corresponding apsAuxiliaryFrame and MIC.
*
* Return value:
*   The status of the security procedure, the secure payload and the size of the
*   full payload after the security procedure.
*
************************************************************************************/
zbStatus_t APS_ProcessOutgoingFrame
(
  uint8_t  *pApsHeader,         /* IN: The pointer where the ApsDataRequest header is. */
  uint8_t  apsHeaderLength,     /* IN: Size in bytes of the Aps Header. */
  uint8_t  *pApsFrameLength,    /* IN/OUT: The size in bytes of the complete APS Frame
                                           [ header||payload ].*/
  zbNwkAddr_t  aDestAddress,    /* IN: The device who will receive the packet. */
  zbApsTxOption_t    txOptions, /*  */
  zbApsCommandID_t  apsCommandID
);

zbStatus_t  APS_ProcessIncomingFrames
(
  uint8_t  *pApsHeader,
  uint8_t  *pApsFrameLength,
  zbNwkAddr_t  aSrcAddress
);


void APS_RegisterLinkKeyData
(
  zbIeeeAddr_t  aDeviceAddress,
  zbKeyType_t  keyType,
  uint8_t  *pKey
);

/************************************************************************************
* 053474r17 Sec. - 4.4.2.6.1 Generating and Sending the Initial SKKE-1 Frame
* The SKKE protocol begins with the initiator device sending an SKKE-1 frame.
* The SKKE-1 command frame shall be constructed as specified in subclause
* 4.4.9.1.
*
* Interface assumptions:
*   The parameters pSKKE, pEstablishKeyReq and pDestAddress are not null pointers.
*
* Return value:
*   The size in bytes of the APSME command to be send out.
*
************************************************************************************/
uint8_t APS_SKKE1
(
  zbApsmeSKKECommand_t       *pSKKE,             /* OUT: The place in memory where the
                                                         command will be build. */
  zbApsmeEstablishKeyReq_t   *pEstablishKeyReq,  /* IN: Establish key request form ZDO. */
  uint8_t                    *pDestAddress,      /* OUT: The address of the destination
                                                         node. */
  bool_t                     *pSecurityEnable
);

/************************************************************************************
* 053474r17 - Sec. 4.4.2.4 APSME-ESTABLISH-KEY.response
* 4.4.2.4.3 Effect on Receipt.
* If the Accept parameter is TRUE, then the APSME of the responder will attempt to
* execute the key establishment protocol indicated by the KeyEstablishmentMethod
* parameter. If KeyEstablishmentMethod is equal to SKKE, the APSME shall execute the
* SKKE protocol, described in subclause 4.4.2.6.
*
* Interface assumptions:
*   The parameters pSKKE2, pEstablishKeyRsp and pDestAddress are not null pointers.
*
* Return value:
*   The size in bytes of the APSME command to be send out.
*
************************************************************************************/
uint8_t APS_SKKE2
(
  zbApsmeSKKECommand_t     *pSKKE2,
  zbApsmeEstablisKeyRsp_t  *pEstablishKeyRsp,
  uint8_t                  *pDestAddress,
  bool_t                   *pSecurityEnable
);

zbSize_t  APS_GenerateTunnelCommand
(
  zbApsmeTunnelCommand_t  *pTunnelCommand,
  zbApsmeTunnelReq_t  *pTunnelReq,
  uint8_t *pDestAddress,
  bool_t *pSecurityEnable
);

zbApsmeZdoIndication_t *APSME_SKKE_indication
(
  uint8_t                commandId,
  zbApsmeSKKECommand_t  *pSKKECmd,
  zbIeeeAddr_t           aSrcAddress
);


uint8_t APS_TunnelCommandSize(void);

zbApsmeZdoIndication_t *APSME_TUNNEL_CMMD_indication
(
  zbApsmeTunnelCommand_t  *pTunnel
);


zbSize_t SSP_GenerateTunnelCommand
(
  zbApsmeTunnelCommand_t  *pTunnelCommand,
  zbApsmeTunnelReq_t  *pTunnelReq,
  uint8_t *pDestAddress,
  bool_t *pSecurityEnable
);

void SSP_ApsSetTxOptions
(
  zbApsTxOption_t  *pTxOptions,
  zbApsCommandID_t  cmdID,
  uint8_t *pDestAddress
);

zbStatus_t SSP_ApsProcessOutgoingFrame
(
  uint8_t  *pApsHeader,         /* IN: The pointer where the ApsDataRequest header is. */
  uint8_t  apsHeaderLength,     /* IN: Size in bytes of the Aps Header. */
  uint8_t  *pApsFrameLength,    /* IN/OUT: The size in bytes of the complete APS Frame
                                           [ header||payload ].*/
  zbNwkAddr_t  aDestAddress,    /* IN: The device who will receive the packet. */
  zbApsTxOption_t    txOptions, /*  */
  zbApsCommandID_t  apsCommandID
);

zbStatus_t  SSP_ApsProcessIncomingFrames
(
  uint8_t  *pApsHeader,
  uint8_t  *pApsFrameLength,
  zbNwkAddr_t  aSrcAddress
);

zbApsmeZdoIndication_t *APSME_TUNNEL_indication
(
  zbApsmeTunnelCommand_t  *pTunnel
);

uint8_t SSP_SKKE2
(
  zbApsmeSKKECommand_t     *pSKKE2,
  zbApsmeEstablisKeyRsp_t  *pEstablishKeyRsp,
  uint8_t                  *pDestAddress,
  bool_t                   *pSecurityEnable
);

uint8_t SSP_SKKE1
(
  zbApsmeSKKECommand_t         *pSKKE,             /* OUT: The place in memory where the
                                                           command will be build. */
  zbApsmeEstablishKeyReq_t     *pEstablishKeyReq,  /* IN: Establish key request form ZDO. */
  uint8_t                      *pDestAddress,      /* OUT: The address of the destination
                                                           node. */
  bool_t                       *pSecurityEnable
);

zbApsmeZdoIndication_t *APSME_SKKE1_indication
(
  zbApsmeSKKECommand_t  *pSKKE1Cmd,
  zbIeeeAddr_t          aSrcAddress
);

zbApsmeZdoIndication_t *APSME_SKKE2_indication
(
  zbApsmeSKKECommand_t * pSKKE2Cmd
);

zbApsmeZdoIndication_t *APSME_SKKE3_indication
(
  zbApsmeSKKECommand_t * pSKKE3Cmd
);

zbApsmeZdoIndication_t *APSME_SKKE4_indication
(
  zbApsmeSKKECommand_t * pSKKE4Cmd
);

zbApsDeviceKeyPairSet_t * APS_GetSecurityMaterilaEntry
(
  uint8_t *pDeviceAddress,
  zbApsDeviceKeyPairSet_t *pWhereToCpyEntry
);

zbApsDeviceKeyPairSet_t *SSP_ApsGetSecurityMaterilaEntry
(
  uint8_t *pDeviceAddress,
  zbApsDeviceKeyPairSet_t *pWhereToCpyEntry
);

/************************************************************************************
* Matyas-Meyer-Oseas:
* The Matyas-Meyer-Oseas one-way compression function can be considered the dual
* (the opposite) of Davies-Meyer.
* It feeds each block of the message (m sub(i)) as the plain text to be encrypted.
* The output cipher text is then also XORed (^) with the same message block (m sub(i))
* to produce the next hash value (H sub(i)). The previous hash value (H sub(i-1)) is
* fed as the key to the block cipher. In the first round when there is no previous hash
* value it uses a constant pre-specified initial value (H Sub(0)).
*
* If the block cipher has different block and key sizes the hash value (H sub(i-1)) will
* have the wrong size for use as the key. The cipher might also have other special
* requirements on the key. Then the hash value is first fed through the function g( ) to
* be converted/padded to fit as key for the cipher.
*
* In mathematical notation Matyas-Meyer-Oseas can be described as:
*
* H sub(i) = E(g(H sub(i-1)) (m sub(i))) XOR (m sub(i))
*
* Note: The 'E' is what ever encryption function is used (e.g. AES).
*
* Diagram:
*                      +-------+
*       (input)        |       |     (input)
*   --- H Sub(i-1) --->|   E   |<--- m Sub()i ---
*                      |       |       |
*                      +-------+       |
*                 (output) |           |
*                          |           |
*                         XOR<---------+
*                          |
*                          v (output)
*                       H sub(i)
*
* Note: This function sub divides the provided set of octets in blocks of in the way of
*       m sub(i).
*
* Interface assumptions:
*   The parameters pInputString and pHash are not null pointer.
*
* Return value:
*   The cypher hash in the pHash parameter.
*
* Revison history:
*   Date   Author  Comments
*  ------  ------  --------
*  080408    MN    Created
************************************************************************************/
#ifndef __IAR_SYSTEMS_ICC__
extern void SSP_MatyasMeyerOseasHash
(
  uint8_t  *pInputString,  /* IN: The set of blocks of 16 bytes to cypher. */
  uint8_t   inputOctets,   /* IN: The amount of octets in the set of blocks */
  uint8_t  *pHash          /* OUT: The place where the hash will be generated. */
);
#endif

#if 0
#ifdef __IAR_SYSTEMS_ICC__
extern void SSP_AesAndXor
(
  uint8_t aBlock[16],
  uint8_t aKey[16],
  uint8_t aOutput[16]
);
#endif
#endif

#ifndef __IAR_SYSTEMS_ICC__
extern void SSP_ExorWithValue
(
  const uint8_t  *pSrc,  /* IN: The array to be exored. */
  uint8_t  *pDest,       /* OUT: The array where the xored values will be stored. */
  bool_t  value          /* IN: The value to exor with each other value of the input array. */
);
#endif

/************************************************************************************
* Perform AES-128 cryptographic operation on a data set. Optimized for 8 bit
* processors. The result will be processed in about 8000 cycles on the HCS08 platform
* Please note that for HCS08 projects the architecture definitions must locate the
* internal buffers (MY_ZEROPAGE) in lower memory space for correct linking.
*
* Interface assumptions:
*   All input/outputs are 16 bytes (128 bit).
*   Please note that the function is not re-entrant. Also, it is of course not re-
*   entrant with other functions calling this function (like SecLib_CcmStar).
*   It is allowed to point the ReturnData pointer to the same memory as either Data or
*   Key if wanted.
*
* Return value:
*   None
* 
************************************************************************************/

#ifdef __IAR_SYSTEMS_ICC__
extern void RotateBlock( uint32_t* dataIn, uint32_t* dataOut);
#endif

/************************************************************************************
* Genrates the different tag in different stages of the SKKE procedure.
*
* MagTag0:
*   Challenge1    = Initiator Challenge.
*   Challenge2    = Responder Challenge.
*   Key           = Master Key.
*   OutPut String = Secret Data.
*   Initiator:
*            U = Local Device.
*            V = Remote Device.
*   Responder:
*            U = Remote Device.
*            V = Local Device.
*
* MagTag1:
*   Challenge1    = Responder Challenge.
*   Challenge2    = Initiator Challenge.
*   Key           = Secret Data.
*   OutPut String = MagTag1.
*   Initiator:
*            U = Remote Device.
*            V = Local Device.
*   Responder:
*            U = Local Device.
*            V = Remote Device.
*
* MagTag2:
*   Challenge1    = Initiator Challenge.
*   Challenge2    = Responder Challenge.
*   Key           = Secret Data.
*   OutPut String = MagTag2.
*   Initiator:
*            U = Local Device
*            V = Remote Device
*   Responder:
*            U = Remote Device.
*            V = Local Device.
*
* Interface assumptions:
*   The input pointer are not null pointers.
*   The parameter tagId is a one of the 3 possible values (0, 1 or 2).
*
* Return value:
*   NONE.
*
************************************************************************************/
#ifndef __IAR_SYSTEMS_ICC__ 
extern void SSP_HashGenerateSecretTag
(
  uint8_t  tagId,
  uint8_t  *pU,
  uint8_t  *pV,
  uint8_t  *pChallenge1,
  uint8_t  challenge_1_Size,
  uint8_t  *pChallenge2,
  uint8_t  challenge_2_Size,
  uint8_t  *pKey,
  uint8_t  *pOutputString,
  bool_t  needSwap,
  zbFrameCounter_t  *pOptionalText
);


extern void SSP_HashKeyDerivationFunction
(
  uint8_t  *pSecretData,  /* MacKey */
  uint8_t  secretDataSize,
  uint8_t  *pKey          /* Link Key. */
);

void SSP_HashKeyedHMAC
(
  uint8_t  *pInputString,
  uint8_t   inputOctets,
  uint8_t  *pKey,
  uint8_t  *pHMAC
);
#endif
/************************************************************************************
* Exors each element of the provided array as pSrc with the given value, and store it
* on the corresponding entrie of the pDest array.
*
* Interface assumptions:
*   The parameters pSrc and pDest are not null pointers.
*   The memory space pointed by pSrc and pDst are equally long (same amount of bytes).
*
* Return value:
*   The cypher hash in the pHash parameter.
*
************************************************************************************/
#ifndef __IAR_SYSTEMS_ICC__ 
extern void SSP_ExorWithValue
(
  const uint8_t  *pSrc,  /* IN: The array to be exored. */
  uint8_t  *pDest,       /* OUT: The array where the xored values will be stored. */
  bool_t  value          /* IN: The value to exor with each other value of the input array. */
);
#else
#if MC13226Included_d

extern void SSP_HashGenerateSecretTag
(
  uint8_t  tagId,
  uint8_t  *pU,
  uint8_t  *pV,
  uint8_t  *pChallenge1,
  uint8_t  challenge_1_Size,
  uint8_t  *pChallenge2,
  uint8_t  challenge_2_Size,
  uint8_t  *pKey,
  uint8_t  *pOutputString,
  bool_t  needSwap,
  zbFrameCounter_t  *pOptionalText
);

extern void SSP_HashKeyDerivationFunction
(
  uint8_t  *pSecretData,  /* MacKey */
  uint8_t  secretDataSize,
  uint8_t  *pKey          /* Link Key. */
);

extern void SSP_HashKeyedHMAC
(
  uint8_t  *pInputString,
  uint8_t   inputOctets,
  uint8_t  *pKey,
  uint8_t  *pHMAC
);

extern void SSP_MatyasMeyerOseasHash
(
  uint8_t  *pInputString,  /* IN: The set of blocks of 16 bytes to cypher. */
  uint8_t   inputOctets,   /* IN: The amount of octets in the set of blocks */
  uint8_t  *pHash          /* OUT: The place where the hash will be generated. */
);

extern void SSP_ExorWithValue
(
  const uint8_t  *pSrc,  /* IN: The array to be exored. */
  uint8_t  *pDest,       /* OUT: The array where the xored values will be stored. */
  bool_t  value          /* IN: The value to exor with each other value of the input array. */
);

#else
extern void ROM_SSP_HashGenerateSecretTag
(
  uint8_t  tagId,
  uint8_t  *pU,
  uint8_t  *pV,
  uint8_t  *pChallenge1,
  uint8_t  challenge_1_Size,
  uint8_t  *pChallenge2,
  uint8_t  challenge_2_Size,
  uint8_t  *pKey,
  uint8_t  *pOutputString,
  bool_t  needSwap,
  zbFrameCounter_t  *pOptionalText
);

#define SSP_HashGenerateSecretTag ROM_SSP_HashGenerateSecretTag

extern void ROM_SSP_HashKeyDerivationFunction
(
  uint8_t  *pSecretData,  /* MacKey */
  uint8_t  secretDataSize,
  uint8_t  *pKey          /* Link Key. */
);

#define SSP_HashKeyDerivationFunction ROM_SSP_HashKeyDerivationFunction

extern void ROM_SSP_HashKeyedHMAC
(
  uint8_t  *pInputString,
  uint8_t   inputOctets,
  uint8_t  *pKey,
  uint8_t  *pHMAC
);

#define SSP_HashKeyedHMAC ROM_SSP_HashKeyedHMAC

extern void ROM_SSP_MatyasMeyerOseasHash
(
  uint8_t  *pInputString,  /* IN: The set of blocks of 16 bytes to cypher. */
  uint8_t   inputOctets,   /* IN: The amount of octets in the set of blocks */
  uint8_t  *pHash          /* OUT: The place where the hash will be generated. */
);

#define SSP_MatyasMeyerOseasHash ROM_SSP_MatyasMeyerOseasHash

extern void ROM_SSP_ExorWithValue
(
  const uint8_t  *pSrc,  /* IN: The array to be exored. */
  uint8_t  *pDest,       /* OUT: The array where the xored values will be stored. */
  bool_t  value          /* IN: The value to exor with each other value of the input array. */
);

#define SSP_ExorWithValue ROM_SSP_ExorWithValue
#endif
#endif

bool_t ApsEncryptACK(zbIeeeAddr_t aDestAddress);

#if gStandardSecurity_d || gHighSecurity_d
#if gComboDeviceCapability_d
extern void ZDO_SecSetTrustCenterOn(void);
#endif
#endif

void CustomLinkStatusTimeOutCallBack
(
  tmrTimerID_t timerId  /* IN: */
);

/*****************************************************************************
* This function must return false, it is implemented for future enchancements
*****************************************************************************/
bool_t SecurityTypeCheck( zbIeeeAddr_t aIeeeAddress );

/*****************************************************************************
* This function returns TRUE if a SE packet that doesn't belong to the Key
* Establishment cluster is encrypted with the preconfigured TC link key.
*****************************************************************************/
bool_t SeSecurityCheck(uint8_t frameControl, uint8_t *pProfileId, uint8_t *pKey);

/***********************************************************************************
* If the process of the Incoming or outcoming frame in the secure module has a
* status different then success wqe report that to the application.
*
* Interface Assumptions:
*   The parameter 'securityStatus' is one of the possible status code used in the
*   secure module:
*      gZbSuccess_c       = 0x00
*      gZbSecurityFail_c  = 0xa2
*      gSSP_SecurityOff_c = 0x06
*
* Return value:
*   NONE.
*
* NOTE: Assume that zbMsgId_t and zbStatus_t are one byte each.
************************************************************************************/
extern void SSP_SecProcessSecureFrameConfirm
(
  uint8_t     msgId,
  zbStatus_t  securityStatus  /* IN: the confirm status code. */
);


#if gStochasticAddressingCapability_d
void NWK_ADDRESSASSIGN_GenerateAddressStochasticMode
(
  uint8_t* pShortAddrGenerated
);
#endif


#endif /* _ZDO_APS_INTERFACE_H_ */
