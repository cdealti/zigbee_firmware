/******************************************************************************
  ZdoStateMachineHandler.c

  (c) Copyright 2008, Freescale, Inc. All rights reserved.

  No part of this document must be reproduced in any form - including copied,
  transcribed, printed or by any electronic means - without specific written
  permission from Freescale Semiconductor.

  This module contains the ZDO state machine that starts and stops the network. It includes 
  calling on the security module for authentication, and temporarily suspending the running 
  state for remote commands (such as Mgmt_NWK_Disc_req).

  See ZDO_StateMachine():

                           +--------------------+
   +---------------------->| gZdoInitialState_c |
   |                       +--------------------+
   |                                  |
   |                                  v
   |                   +--------------------------------+
   |     +------------>| gZdoDiscoveringNetworksState_c |<--------------------+
   |     |             +--------------------------------+                     |
   |     |           (ZC, Zx) |                      | (ZR, ZED, Zx only)     |
   |     |                    v                      v                        |
   |     |      +--------------------+      +--------------------+            |
   |     +------| gZdoFormingState_c |      | gZdoJoiningState_c |------------+
   |            +--------------------+      +--------------------+            |
   |                   |                               | (ZR, ZED, Zx only)   |
   |                   |                               v                      |
   |                   |                  +---------------------------------+ |
   |                   +------------------| gZdoDeviceAuthenticationState_c |-+
   |                   |                  +---------------------------------+
   |                   v
   |          +--------------------+      +---------------------------+
   |          | gZdoRunningState_c |<---->| gZdoRemoteCommandsState_c |
   |          +--------------------+      +---------------------------+
   |                   |
   |                   v
   |         +--------------------+
   |         | gZdoLeavingState_c | (may send messages over the air... can only happen when on network)
   |         +--------------------+
   |                   |
   |                   v
   |        +---------------------+
   +--------| gZdoStoppingState_c | (local only... can happen any time)
            +---------------------+

  1. The "gZdoInitialState_c" state is where ZDO always starts (this is off the network)
  2. The "gZdoDiscoveringNetworksState_c" is where it finds networks. The network layer keeps a 
     copy of the energy scan (ZC only) and list of other ZigBee/15.4 devices. The ZDO layer frees
     this memory when finished. Discovery will continue for a certain amount of time, or when a 
     certain # of tries have been attempted.
  3. The "gZdoFormingState_c" is for ZC and Zx (in coordinator mode) devices. If forming fails,
     it goes back to discovery state. 
  4. The "gZdoJoiningState_c" is for ZR, ZED and Zx (in router/end-device modes) devices. If 
     joining fails it goes back to discovery state.
  5. The "gZdoDeviceAuthenticationState_c" is for security ZRs and ZEDs. If this state fails, 
     goes back to network discovery state.
  6. The "gZdoRunningState_c" indicates that the device is running and can now send/receive
     packets.
  7. The "gZdoRemoteCommandsState_c" is used for remote devices causing this device to perform
     things such as a scan for networks, or changing channels. Once complete, this goes back to
     running state.
  8. The "gZdoLeavingState_c" means this device is cleanly leaving the network, which will allow
     other devices in the network to clean up their tables automatically. A ZR or ZC may also tell
     its children to leave.
  9. The "gZdoStoppingState_c" means this device is stopping. This cannot fail. The device stops
     and alternately clears out NVM.

  Each state has a number of sub-states caused by "events". See the header for each state to see
  what these are.

  The overall ZDO process, when ZDO_Start() is called by the application begins with a network 
  discovery [ND] attempt. Once the best selection of beacons are gathered, then a form (ZC) or join 
  (ZR, ZED) attempt is made. If that fails, that starts over again, up to a user selected length of 
  time and # of attempts, whichever comes first. See the figure below:

  +------------------------------ (T) ------------------------------------+
  |<--------[SA]---------->|                                              |
  [ES][AS](B)[AS](B)[AS][FJ]...(RI)...[ES][AS](B)[AS](B)[AS][FJ]...(RI)....
  |                        |
  +----------- [ND] -------+

  The properties that define these various settable # of attempts and timeouts, as well as the 
  figure key, are shown below:

  (T)  = Total Timeout, mDefaultValueOfFormationAttemptsTimeOut_c(ZC), 
         mDefaultValueOfDiscoveryAttemptsTimeOut_c(ZR,ZED) (time units: sec), Internal
         configure option.
  [ES] = The amount of time to spend on the energy Scan (ZC only), gScanDuration_c
         (802.15.4 MAC exponential wait), internal configuration option.
  [AS] = The amount of time to spend on each active Scan (all device types), gScanDuration_c
         (802.15.4 MAC exponential wait), internal configuration option.
  [SA] = The amount of scans to do on each Network Discovery/Formation attempt, this
         values can be set and access through the SAS and commissioning cluster. (0x01 - 0x0FF)
  (B)  = Time Between Active Scans, mDefaultValueOfNwkTimeBwnScans_c (time uints: ms), the
         amount of time before starting the next Active scan, settable through the SAS and
         commissioning cluster.
  [FJ] = Form or join attempt. If this fails, will wait the rejoin interval. If it works,
         it goes to authenticating/running state. See [ND], one [FJ] per each [ND]
  (RI) = Rejoin interval, mDefaultValueOfRejoinInterval_c (time units: sec), settable through the SAS and
         commissioning cluster.
  [ND] = Network Discovery/Forming attempt. This will occur 'N' number of times, where 'N' is 
         mDefaultValueOfNwkFormationAttempts_c or mDefaultValueOfNwkDiscoveryAttempts_c

  See [R17], Table 1.48: Config_NWK_Scan_Attempts [ZCL Commissioning Cluster], ScanAttempts
      NlmeGetRequest(gNwkScanAttempts_c)

  mDefaultValueOfNwkScanAttempts_c describes how many times to discover networks before attempting 
  to form or join. Trying multiple times helps in a noisy or dense network environment.

******************************************************************************/
#ifndef __IAR_SYSTEMS_ICC__
#ifdef MC13226Included_d
    #undef MC13226Included_d
#endif    
    #define MC13226Included_d 0
#endif
#include "ZdoApsInterface.h"
#include "ZdoNwkManager.h"
#include "TMR_Interface.h"
#include "NwkCommon.h"
#include "PWR_Interface.h"
#include "NVM_interface.h"
#if !gArm7ExtendedNVM_d
#include "NV_Data.h"
#endif
#include "BeeStackConfiguration.h"
#include "ZdoMain.h"
#include "BeeStackUtil.h"
#include "ZdoCommon.h"
#include "ZDOStateMachineHandler.h"
#include "ZdoNwkInterface.h"
#include "ZdoSecurityManager.h"
#include "BeeStackParameters.h"
#include "BeeCommon.h"
#include "ZdpUtils.h"

/******************************************************************************
*******************************************************************************
* Private Macros
*******************************************************************************
******************************************************************************/

/* State Selected depending on device type (old state machine only) */
#if ( gCoordinatorCapability_d )
#define mZDOCoordinatorState gZdoState
#define ZDOCoordinatorChangeState( state ) mZDOCoordinatorState = state
#elif ( gRouterCapability_d )
#define mZDORouterState gZdoState
#define ZDORouterChangeState( state ) mZDORouterState = state
#else
#define mZDOEndDeviceState gZdoState
#define ZDOEndDeviceChangeState( state ) mZDOEndDeviceState = state
#endif


#define mMinLimitSignedChar  -127
#define mTimeOutForRetries    0x00
#define mResetRetries         -1

/******************************************************************************
*******************************************************************************
* Private Prototypes
*******************************************************************************
******************************************************************************/

void APS_InitCounter(void);
#if gArm7ExtendedNVM_d
bool_t ZDO_NvmRestoreAllData(void);
#else
void ZDO_NvmRestoreAllData(void);
#endif

void ZDO_StateMachine(void);

void ZDO_StateMachineTimerCallBack(uint8_t timerId);

bool_t ZDO_ValidateNwkData(void);

bool_t ZDO_IsValidParentInfo(void);

void ZDO_SetLogicalChannel(void);


#if ((gStandardSecurity_d || gHighSecurity_d) && (gRouterCapability_d || gEndDevCapability_d || gComboDeviceCapability_d) )
void ZDO_StateMachineAuthenticatingDevice(void);
#endif

/* any device can leave the network */
void ZdoNwkMng_GenerateNlmeLeaveRequest(void);

/* energy scan - possible on ZC, ZR and Zx devices */
#if ( gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d)
void ZdoNwkMng_EnergyScanRequest
(
  zbChannels_t  aChannelList,
  uint8_t  duration
);
#endif

#if (gStandardSecurity_d || gHighSecurity_d)
extern void SSP_NwkResetSecurityMaterials(void);
#endif

void ZDO_CheckForNwkRejoin(zbNwkJoinMode_t gZDOJoinMode);

/*
  Helper function to set the right server mask in the case of combo devices.
  The fact is that polling End Devices can not be any type of server.
*/
void ZDO_ServerMaskInit(void);

/* Sends the reset request to the APS layer. */
void ZDO_ApsReset(bool_t fFullReset, apsmeMessage_t *pApsmeMsg);


/******************************************************************************
*******************************************************************************
* Private type definitions
*******************************************************************************
******************************************************************************/

/*None*/


/******************************************************************************
*******************************************************************************
* Private Memory Declarations
*******************************************************************************
******************************************************************************/

/* Formation and Rejoining (orphaning) and joining (discoveryRetries) cannot all happen at the same time */
/* so use the same variable */
#define mcFormationRetries mcDiscoveryRetries
#define mcOrphanScanRetries mcDiscoveryRetries

/* used for both forming and joining in new state machine */
static uint8_t mcDiscoveryRetries;  /* a network discovery results in a list of networks */

static uint8_t mcScanRetries;       /* each network discovery may involve 1 or more active scans */


/******************************************************************************
*******************************************************************************
* Public Memory
*******************************************************************************
******************************************************************************/
#if gArm7ExtendedNVM_d
extern NvDataSetID_t const allDataSets[gNvNumberOfDataSets_c];
#endif

/* ZDO state machine globals */

#if MC13226Included_d
    extern volatile ZdoState_t  gZdoState;
#else
    ZdoState_t      gZdoState = gZdoInitialState_c;
#endif


ZdoEvent_t      gZdoEvent;
ZdoStartMode_t  gZdoStartMode;
ZdoStopMode_t   gZdoStopMode;
static bool_t   mDiscoveryFirstTime;  // first time

#if gEndDevCapability_d || gComboDeviceCapability_d
uint16_t        gZdoOrgPollRate;  /* original poll rate */
#endif


/* informs application layer which join mode (NLME-JOIN modes) */
zbNwkJoinMode_t gZDOJoinMode = gAssociationJoin_c;

/* Keeps track of the current rejoin, to always remeber if ti was Seucre or not. */
bool_t  gZDO_SecureRejoin = FALSE;


/* another layer wants us to rejoin after leaving... */

#if ((gStandardSecurity_d || gHighSecurity_d)&& gEndDevCapability_d)
uint16_t  previousPollRate;
#endif

// running state can be looked up by device type (ZC=0, ZR=1, ZED=2)
static uint8_t maZdo2AppEvents[] = { gZDOToAppMgmtZCRunning_c, gZDOToAppMgmtZRRunning_c, gZDOToAppMgmtZEDRunning_c };
static ZdoState_t maZdoRunning[] = { gZdoCoordinatorRunningState_c, gZdoRouterRunningState_c, gZdoEndDeviceRunningState_c };

#if gComboDeviceCapability_d
extern zbServerMask_t gaServerMask;
#endif

/******************************************************************************
*******************************************************************************
* Public Functions
*******************************************************************************
******************************************************************************/

/*
  Initialize the ZDO state machine task. Nothing to do.
*/
void TS_ZdoStateMachineTaskInit ( void )
{ 
}

/*
  State machine for ZC, ZR or ZED.

  This state machine starts up and stops the network, including calling on the security 
  layer to authenticate the device.
*/
void TS_ZdoStateMachineTask
(
  event_t events  /*IN: events sent to ZDO State Machine */
)
{
  /* current state and pending event are contained in gZdoState and gZdoEvent respectively */
  if(events & gZDO_StateEventOccurred_c)
    ZDO_StateMachine();
}


/******************************************************************************
*******************************************************************************
* Private Functions
*******************************************************************************
******************************************************************************/

/*
  Is this device in the running state (ZC, ZR, ZED?)
*/
bool_t ZDO_IsRunningState(void)
{
#if gCoordinatorCapability_d
  return (ZDO_GetState() == gZdoCoordinatorRunningState_c);
#elif gRouterCapability_d
  return (ZDO_GetState() == gZdoRouterRunningState_c);
#elif gEndDevCapability_d
  return (ZDO_GetState() == gZdoEndDeviceRunningState_c);
/* combo device */
#else
  return (ZDO_GetState() == gZdoCoordinatorRunningState_c) ||
         (ZDO_GetState() == gZdoRouterRunningState_c) ||
         (ZDO_GetState() == gZdoEndDeviceRunningState_c ||
         (ZDO_GetState() == gZdoRunningState_c));
#endif
}

bool_t ZDO_IsReadyForPackets(void)
{
  return ((ZDO_GetState() == gZdoDeviceAuthenticationState_c) ||
          (ZDO_GetState() == gZdoDeviceWaitingForKeyState_c) ||
           ZDO_IsRunningState());
}

#if gArm7ExtendedNVM_d
/*
  Restore all the data - using ARM7 reconfigured NVM component

  Important: gNvNwkAndApsSecDataSet_ID_c must be restored after the gNvNwkDataSet_ID_c
*/
bool_t ZDO_NvmRestoreAllData(void)
{
  uint8_t i;

  for(i=0; i<gNvNumberOfDataSets_c; i++)
  {
    if(!NvRestoreDataSet(allDataSets[i]))
    {
      return FALSE;
    }
  }
  
  return TRUE;
}
#else  /* gArm7ExtendedNVM_d FALSE */
/*
  Restore all the data - using standard NVM component
*/
void ZDO_NvmRestoreAllData(void)
{
  (void)NvRestoreDataSet(gNvDataSet_Nwk_ID_c);
  (void)NvRestoreDataSet(gNvDataSet_App_ID_c);
  (void)NvRestoreDataSet(gNvDataSet_Aps_Sec_ID_c);
#if gStandardSecurity_d || gHighSecurity_d
  (void)NvRestoreDataSet(gNvDataSet_Sec_ID_c);
#endif
}
#endif /* gArm7ExtendedNVM_d */


/*
  Set an event for the ZDO state machine.

  Note: the state machine can only handle one event at a time. 
  If more are needed concurrently, then multiple event bits should be used.
*/
void ZDO_SendEvent(ZdoEvent_t event)
{
  gZdoEvent = event;
  TS_SendEvent( gZdoStateMachineTaskID_c, gZDO_StateEventOccurred_c );
}

/*
  Start the network. Has no effect if the network is already started, unless it is in 
  running state. If in running state, then the network can be restarted (perhaps on a new
  channel).
*/
void ZDO_StartEx(ZdoStartMode_t startMode)
{
  /* No matter what if a nwk rejoin is sent we must start from initial state. */
  if ((ZDO_GetRejoinMode(startMode) == gZdoStartMode_NwkRejoin_c) ||
      (ZDO_GetRejoinMode(startMode) == gZdoStartMode_OrphanRejoin_c))
  {
    ZDO_SetState(gZdoInitialState_c);
  }

  /* only has effect in initial state */
  if(ZDO_GetState() != gZdoInitialState_c)
    return;

  /* start up the state machine */
  gZdoStartMode = startMode;

  /* needed for some of the other stack routes (global dependancy) */
  gZDOJoinMode = ZDO_GetStartMode();

  ZDO_SendEvent(gZdoEvent_Start_c);
}

/*
  For compatibility with old ZDO_Start.
*/
void ZDO_Start(ZdoStartMode_t startMode)
{
  ZDO_StartEx(startMode);
}

/*
  Stop the network. This may be called at any time (regardless of state).
*/
void ZDO_StopEx(ZdoStopMode_t stopMode)
{
  /* go to the stopping state (which may go to the leaving state) */
  ZDO_SetState(gZdoStoppingState_c);

  /* set stop mode and tell ZDO to stop network */
  gZdoStopMode = stopMode;
  ZDO_SendEvent(gZdoEvent_Stop_c);
}

/*
  Leave the network now, no waiting.
*/
void ZDO_Stop(void)
{
  ZDO_StopEx(gZdoStopMode_Stop_c);
}

/*
  Leave the network, announcing first we are leaving.
*/
void ZDO_Leave(void)
{
  ZDO_StopEx(gZdoStopMode_Announce_c | gZdoStopMode_ResetTables_c | gZdoStopMode_ResetNvm_c);
}
  
/*
  Cannot fail. Always retrieves a startup commissioning set from somewhere. The commissioning
  set determines PAN ID, extended PAN ID, various join timings, etc...

  1. If SAS, copies from current SAS to RAM, then done. An invalid SAS results in NVM set.
  2. If NVM, copies from NVM to RAM, then done. An invalid NVM set results in a ROM set.
  3. If RAM, then the set is already there (done)
  4. If ROM (factory defaults), copies from ROM to RAM then done

  NOTE: Security materials should not be reseted if the commissioning set is NVM or Woking set.
*/
void ZDO_RetrieveProperCommissingSet(ZdoStartMode_t iStartupSet)
{
  /*
    If user chose the commissioning set, then copy from commissioning set to RAM
    If Commissioning set not available, copy from NVM to RAM
    If no NVM, then copies from ROM to RAM
  */
  if(iStartupSet == gZdoStartMode_SasSet_c)
  {
    if(ASL_GetCommissioningSAS())
    {
      zbCommissioningAttributes_t *pCommissioningSas;
      FLib_MemCpy(&gSAS_Ram, ASL_GetCommissioningSAS(), sizeof(gSAS_Ram));
#if gStandardSecurity_d || gHighSecurity_d
      SSP_NwkResetSecurityMaterials();
#if gApsLinkKeySecurity_d
      SSP_ApsResetSecurityMaterial();
#endif
#endif

      /*
        Since the commissioning extended PanId is save in the NwkExtended PanID 
        which is the Extended PanId of the Nwk to which the device is already joined,
        or it already belongs to. And in the case of Form or Join the ApsExtPanId 
        is used.
      */
      /* Zdo state machine is incomplete according to section 2.5.5.5.6.2, the designated coordintar flag is never checked. */
      pCommissioningSas = ASL_GetCommissioningSAS();
      if (pCommissioningSas->startupControl == gStartupControl_NwkRejoin_c)
      {
        //ApsmeSetRequest(gApsUseExtendedPANID_c,NlmeGetRequest(gNwkExtendedPanId_c));
        //BeeUtilZeroMemory(NlmeGetRequest(gNwkExtendedPanId_c), sizeof(zbIeeeAddr_t));
        ZDO_SetStartMode(gZdoStartMode_NwkRejoin_c);
      }

      if (pCommissioningSas->startupControl == gStartupControl_Form_c)
      {
        /*
          Write attributes command writes into Nwk Exteneded Pan Id attribute by default.
          So, we just need to be sure to clean the APS extended pan id to avoid rejoin.
        */
        BeeUtilZeroMemory(ApsmeGetRequest(gApsUseExtendedPANID_c), sizeof(zbIeeeAddr_t));
        ZDO_SetStartMode(gZdoStartMode_Associate_c);
      }
#if (gDefaultValueOfNwkStackProfile_c == 0x02)
      if (pCommissioningSas->startupControl == gStartupControl_SilentStart_c)
      {
        /*
          Write attributes command writes into Nwk Exteneded Pan Id attribute by default.
          So, we just need to be sure to clean the APS extended pan id to avoid rejoin.
        */
        ZDO_SetStartMode(gZdoStartMode_SilentStart_c);
      }
#endif
      return;
    }

    /* No SA.S, choose NVM instead */
    iStartupSet = gZdoStartMode_NvmSet_c;
  }

  /*
    If NVM then copies from NVM to RAM, then done
    If NVM not enabled, then uses ROM set
  */
  if(iStartupSet == gZdoStartMode_NvmSet_c)
  {
#if gArm7ExtendedNVM_d  /* using ARM7 reconfigured NVM component */  
#if gNvStorageIncluded_d
    if (ZDO_NvmRestoreAllData())
    {
#if gStandardSecurity_d || gHighSecurity_d
      RestoreIncomingFrameCounter();
#if gComboDeviceCapability_d
      ZDO_SecSetTrustCenterOn();
#endif
#endif
      return;
    }
#endif
#else  /* gArm7ExtendedNVM_d FALSE */
#if gNvStorageIncluded_d
    if (NvRestoreDataSet(gNvDataSet_Nwk_ID_c) && NvRestoreDataSet(gNvDataSet_App_ID_c) && NvRestoreDataSet(gNvDataSet_Aps_Sec_ID_c))
    {
#if gStandardSecurity_d || gHighSecurity_d
      RestoreIncomingFrameCounter();
#if gComboDeviceCapability_d
      ZDO_SecSetTrustCenterOn();
#endif
      if (NvRestoreDataSet(gNvDataSet_Sec_ID_c))
#endif
      return;
    }
#endif
#endif /* gArm7ExtendedNVM_d */
    /*
      no NVM available, use RAM set instead assuming that the ram set has all the
      modifications done at run time.
    */
    iStartupSet = gZdoStartMode_RamSet_c;

    /* force to associate mode if no NVM available */
    ZDO_SetStartMode(gZdoStartMode_Associate_c);
  }

  /* User chose ROM set (or it was chosen because other choices weren't available) */
  if (iStartupSet == gZdoStartMode_RomSet_c)
  {
    FLib_MemCpy(&gSAS_Ram, (void *)(&gSAS_Rom), sizeof(gSAS_Ram));
    /*
      for ROM or RAM set, make sure to set up the security materials from what's currently
      in the RAM SAS
    */
  }

  /*
    If the device is doing a Network Rejoin and the use of insecure rejoin is not set to
    TRUE the device should not reset the securityu material.
    Setting the gApsUseInsecureRejoin_c to FALSE means that the rejoin must be done using
    the current security material set.
  */
  if (ZDO_GetStartMode() == gZdoStartMode_NwkRejoin_c && !ApsmeGetRequest(gApsUseInsecureJoin_c))
  {
    /* Keep the existing security material. */
    return;
  }

  /* Reset the security material with the proper set.! */
#if gStandardSecurity_d || gHighSecurity_d
  SSP_NwkResetSecurityMaterials();
#if gApsLinkKeySecurity_d
  SSP_ApsResetSecurityMaterial();
#endif
#endif

}

#if gNvStorageIncluded_d
#if gStandardSecurity_d || gHighSecurity_d
void RestoreIncomingFrameCounter(void)
{
    // we just restored pointer gIncomeFrameCounterSet. Point it to gaIncomingFrameCounterSetx.
    gNwkData.aNwkSecurityMaterialSet[0].pIncomingFrameCounterSet = gaIncomingFrameCounterSet1;
    gNwkData.aNwkSecurityMaterialSet[1].pIncomingFrameCounterSet = gaIncomingFrameCounterSet2;
}
#endif
#endif

/*
  Helper function. Returns TRUE if the current mode is OK. Returns FALSE if it should associate.

  1. If globals don't look valid, don't allow anything but associate (or form)
  1. If ZC and silent join, then return FALSE to assume associate. 
  2. If ZR or ZED, then must have valid parent address, else return FALSE to associate.
*/
bool_t ZDO_IsValidStartMode(ZdoStartMode_t startMode)
{
  /* it's always valid to associate */
  if(startMode == gZdoStartMode_Associate_c)
    return TRUE;

  /* various silent/rejoin nodes only valid if globals are valid (set previously) */
  if (startMode == gZdoStartMode_SilentStart_c)
  {
    if (!ZDO_ValidateNwkData())
    {
      // If we do not have all the necessary information to perform a silen start try to start with NwkRejoin
      
      if (!Cmp8BytesToZero(ApsmeGetRequest(gApsUseExtendedPANID_c)))
      {
        /*
          If the nNwk Extended Pan Id is not present BUT the APS extended Pan Id is, then,
          do a Nwk Rejoin.
        */
        startMode=gZdoStartMode_NwkRejoin_c;
        gZDOJoinMode = gZdoStartMode_NwkRejoin_c;         
        ZDO_SetStartMode(gZdoStartMode_NwkRejoin_c);
      }
    }
  }


  /* various silent/rejoin nodes only valid if globals are valid (set previously) */
  if (startMode == gZdoStartMode_NwkRejoin_c)
  {
    /* For network rejoin the NwkPanId and Short address are not needed. */
    if (!IsValidExtendedPanId(ApsmeGetRequest(gApsUseExtendedPANID_c)))
    {
      return FALSE;
    }
  }
  /* WE are not associating or rejoining so we need to know if the current nwk info is a valid one. */
  else if (!ZDO_ValidateNwkData())
  {
    return FALSE;
  }

  /* silent start is valid for ZC if stack globals OK */
  if(NlmeGetRequest(gDevType_c) == gCoordinator_c)
  {
    /* only associate and silent are legal for ZCs */
    if(startMode != gZdoStartMode_SilentStart_c)
      return FALSE;
    return TRUE;
  }

  /* SP2 Router and coordinators are allow to do silent start with no parent. */
  if (NlmeGetRequest(gDevType_c) != gEndDevice_c)
  {
    /* Stack profile = 0x02 is ZigBee Pro stack profile. */
    if (NlmeGetRequest(gNwkStackProfile_c) == 0x02)
    {
      /* No need to check for parent */
      return TRUE;
    }
  }

  /* ZRs and ZEDs must have a valid parent to rejoin,unless is an Orphan rejoin and silent start. */
  else if ((ZDO_GetStartMode() != gZdoStartMode_OrphanRejoin_c))
  {
    /* Asking for the parent address and forcing the rejoin does not affect the ZC. */
    if (!ZDO_IsValidParentInfo())
    {
      /* If the node does not has a valid nwk data for the parent node, we should force the rejoin. */
      gZDOJoinMode = gZdoStartMode_NwkRejoin_c;
      ZDO_SetStartMode(gZdoStartMode_NwkRejoin_c);
    }
  }

  /* Any device.! */
 return TRUE;
}

/* Validates the Nwk and IEEE of the entry 0 in the NT. */
bool_t ZDO_IsValidParentInfo(void)
{
  neighborTable_t *pNTE = gaNeighborTable;

  return (IsValidNwkUnicastAddr(pNTE->aNetworkAddress) && IsValidExtendedAddress(pNTE->aExtendedAddress));
}

/* Validates the basic Nwk values. */
bool_t ZDO_ValidateNwkData(void)
{
  return (IsValidExtendedPanId(NlmeGetRequest(gNwkExtendedPanId_c)) &&
          IsValidPanId(NlmeGetRequest(gNwkPanId_c)) &&
          IsValidNwkUnicastAddr(NlmeGetRequest(gNwkShortAddress_c)) &&
          IsValidLogicalChannel(NlmeGetRequest(gNwkLogicalChannel_c)));
  /*
    There is no need to check if the device is on the network or not, the validation
    happens on the initial state of the zdo state machine and this state only takes control
    when the device leaves the network or gets stoped, in any case the device is not on the
    network.
  */
}

/*
  Handler for the state machine intial state. This handles a variety of tasks before 
  moving on to the next state, including:

  Initial state is guaranteed to be off the network (must have got here from reset or stop).
*/
void ZDO_StateMachineInitialState(void)
{
#if gComboDeviceCapability_d
  uint8_t iCapInfo;
  uint8_t iDeviceType;
#endif

  /* should only get here once */
  if(ZDO_GetEvent() != gZdoEvent_Start_c)
    return;

  /* let application know the device is being initialized (uses a small buffer) */
  Zdo_GenerateEventZdo2App ( gZDOToAppMgmtInitDevice_c );

  /* re-initialize APS counter each time */
  APS_InitCounter();

  /* set up globals for device type on combo */
#if gComboDeviceCapability_d
  if(ZDO_GetPreferredDevType())
  {
    /* set the device type in RAM */
    iCapInfo = NlmeGetRequest(gNwkCapabilityInformation_c);
    ApsmeSetRequest(gApsDesignatedCoordinator_c, FALSE);
    switch(ZDO_GetPreferredDevType())
    {
      case gZdoStartMode_Zc_c:
        iCapInfo |= (gReceiverOnwhenIdle_c | gDeviceType_c);
        NlmeSetRequest(gDevType_c, gCoordinator_c);
        NlmeSetRequest(gNwkCapabilityInformation_c, &iCapInfo);
        ApsmeSetRequest(gApsDesignatedCoordinator_c, TRUE);
      break;
      case gZdoStartMode_Zr_c:
        iCapInfo |= (gReceiverOnwhenIdle_c | gDeviceType_c);
        NlmeSetRequest(gDevType_c, gRouter_c);
        NlmeSetRequest(gNwkCapabilityInformation_c, &iCapInfo);
      break;
      case gZdoStartMode_Zed_c:
        iCapInfo &= ~(gReceiverOnwhenIdle_c | gDeviceType_c);
        NlmeSetRequest(gDevType_c, gEndDevice_c);
        NlmeSetRequest(gNwkCapabilityInformation_c, &iCapInfo);
      break;
      case gZdoStartMode_ZedRx_c:
        iCapInfo &= ~(gDeviceType_c);
        iCapInfo |= (gReceiverOnwhenIdle_c);
        NlmeSetRequest(gDevType_c, gEndDevice_c);
        NlmeSetRequest(gNwkCapabilityInformation_c, &iCapInfo);
      break;
    }

    /* Catch the user device type */
    iDeviceType = NlmeGetRequest(gDevType_c);
  }
#endif

  /* Set the Aps designated coordinator value for non combo devices. */
#if gCoordinatorCapability_d
  ApsmeSetRequest(gApsDesignatedCoordinator_c, TRUE);
#elif (gRouterCapability_d || gEndDevCapability_d)
  ApsmeSetRequest(gApsDesignatedCoordinator_c, FALSE);
#endif

  /* retrieves the commissioning set from RAM, ROM, NVM or Commissioning Cluster, as requested by user */
  ZDO_RetrieveProperCommissingSet(ZDO_GetStartupSet());
  
  /* save the original value of the gApsUseInsecureJoin_c aib */
  gZDO_SecureRejoin = ApsmeGetRequest(gApsUseInsecureJoin_c);

  /* determine how to start the network (silent join, associate, etc..) */
  gZDOJoinMode = ZDO_GetStartMode();

#if gComboDeviceCapability_d
  /* Check in the case of invalid data */
  if (NlmeGetRequest(gDevType_c) == gInvalidDeviceType_c)
  {
    /* The values can be over writen  */
    NlmeSetRequest(gDevType_c, iDeviceType);
    NlmeSetRequest(gNwkCapabilityInformation_c, &iCapInfo);
  }
#endif

#if gStandardSecurity_d || gHighSecurity_d
#if gComboDeviceCapability_d
  /* Set the trust center flag According to the device type. */
  ZDO_SecSetTrustCenterOn();
#endif
#endif

  /* record initial poll rate: we'll need to get back to this when we changes to faster rates */
  /* note: this means poll rate must be set up in NIB before calling ZDO_Start() */
#if gEndDevCapability_d || gComboDeviceCapability_d
 #if gComboDeviceCapability_d
  if(NlmeGetRequest(gDevType_c) == gEndDevice_c)
 #endif
  {
    /* ZED: remember original polling rate */
    gZdoOrgPollRate = NlmeGetRequest(gNwkIndirectPollRate_c);

    /* ZED: keep the device awake during joining process */
    ZDO_DisallowDeviceToSleep();
  }
#endif

  /* convert extended address of all 0xff to random address and sync with MAC */
  BUtl_CreateExtendedAddress();

  /* Force the nwk rejoin if solicited */
  if (ZDO_GetJoinMode() == gZdoStartMode_NwkRejoin_c)
  {
    if (!Cmp8BytesToZero(NlmeGetRequest(gNwkExtendedPanId_c)))
    {
      ApsmeSetRequest(gApsUseExtendedPANID_c,NlmeGetRequest(gNwkExtendedPanId_c));
    }
    BeeUtilZeroMemory(NlmeGetRequest(gNwkExtendedPanId_c), sizeof(zbIeeeAddr_t));
  }

  /* Freescale's specific and special strat mode overrides the predefined byt ZigBee. */
  if (ZDO_GetJoinMode() != gZdoStartMode_SilentStart_c)
  {
    /* 053474r17 Section 2.5.5.5.6.2 Startup Procedure */
    if (!Cmp8BytesToZero(NlmeGetRequest(gNwkExtendedPanId_c)) && !ApsmeGetRequest(gApsDesignatedCoordinator_c))
    {
      /* If the Nwk Extended Pan Id is present the node must do a Silent NWK Rejoin. */
      gZDOJoinMode = gZdoStartMode_FindAndRejoin_c;
    }
    else if (!Cmp8BytesToZero(ApsmeGetRequest(gApsUseExtendedPANID_c)))
    {
      /*
        If the nNwk Extended Pan Id is not present BUT the APS extended Pan Id is, then,
        do a Nwk Rejoin.
      */
      gZDOJoinMode = gZdoStartMode_NwkRejoin_c;
    }
  }
 else
    ZDO_SetLogicalChannel();

  /*
    Join Mode can be change during the retrive of the commissioning set or in the next
    validation so it must be use in the rest of the function.
  */
  /* check if all network globals are set correctly */
  if(!ZDO_IsValidStartMode(gZDOJoinMode))
  {
    /* force to associate mode if invalid PAN, extended PAN or short address (or NWK not started) */
    gZDOJoinMode = gZdoStartMode_Associate_c;
    ZDO_SetStartMode(gZdoStartMode_Associate_c);
    /*
      There is no need to check if the device is on the network or not, the validation
      happens on the initial state of the zdo state machine and this state only takes control
      when the device leaves the network or gets stoped, in any case the device is not on the
      network.

      Remember the Nwk State is NOT SAVED on NVM.
    */
  }

  /********************************************************************
    Sepcify the association mode to be use, Rejoin, Association, etc.
  *********************************************************************/
  /*
     we don't know how many messages were sent since last save to NVM, so increment 
     the outgoing security counter. Only has effect if security enabled and the network
     was previously running (for example, if restored from NVM).
  */
#if gStandardSecurity_d || gHighSecurity_d
  /*
    if we were on the network or not before (rejoining/restarting) and we're not
    associating from scratch...
  */
  if((gZDOJoinMode != gZdoStartMode_Associate_c) && (ZDO_GetStartupSet() == gZdoStartMode_NvmSet_c))
  {
    /* Any start mode other than association assumes that the device was on the network before. */
    ZDO_SecUpdateSecureCounters();
    ZdoNwkMng_SaveToNvm(zdoNvmObject_SecureSet_c);
  }
#endif

  /* move on to discovery state (all device types if associating) */
  if(gZDOJoinMode == gZdoStartMode_Associate_c)
  {
    ZDO_SetState(gZdoDiscoveringNetworksState_c);
    ZDO_SendEvent(gZdoEvent_StartingDiscovery_c);
  }

  /* silent starting */
  else if(gZDOJoinMode == gZdoStartMode_SilentStart_c)
  {
    if(NlmeGetRequest(gDevType_c) == gCoordinator_c)
    {
      ZDO_SetState(gZdoFormingState_c);
      ZDO_SendEvent(gZdoEvent_Forming_c);
    }
    else
    {
      ZDO_SetState(gZdoJoiningState_c);
      ZDO_SendEvent(gZdoEvent_Joining_c);
    }
  }

  /* The node is on initial state (Battery off, or physical restart) and rejoin happens */
  else if (gZDOJoinMode == gZdoStartMode_NwkRejoin_c)
  {
    /* Bring all possible data back, and do the rejoin, the node need to discover netwroks. */
    ZDO_SetState(gZdoDiscoveringNetworksState_c);
    ZDO_SendEvent(gZdoEvent_StartingDiscovery_c);
  }

  else if (gZDOJoinMode == gZdoStartMode_FindAndRejoin_c)
  {
    /* Bring all possible data back, and do the rejoin, the node need to discover netwroks. */
    ZDO_SetState(gZdoDiscoveringNetworksState_c);
    ZDO_SendEvent(gZdoEvent_StartingDiscovery_c);
  }

  /* Orphan joining on ZR, ZED */
  /* Orphan join don't need to discover networks (or, if they do, that's done in NWK layer) */
  else
  {
    ZDO_SetState(gZdoJoiningState_c);
    ZDO_SendEvent(gZdoEvent_Joining_c);
  }
}

/*
  Time to discover networks, or to handle results of a network discovery. All devices discover 
  networks if associating.

  Events for this state include: 
  gZdoEvent_StartingDiscovery_c     starting the discovery process (occurs once after leaving initial state)
  gZdoEvent_NextDiscovery_c         go on to next discovery in state machine. Occurs for retires.
  gZdoEvent_DiscoverySuccess_c      discovery found 1 or more beacons
  gZdoEvent_DiscoveryFailure_c      discovery did not find any beacons
  gZdoEvent_EnergyScan_c            ZC only, do energy scan
  gZdoEvent_EnergyScanComplete_c    ZC only, scan is complete 
  gZDO_TimeoutBetweenScan_c         timeout between scans is over
*/
void ZDO_StateMachineDiscoveringNetworks(void)
{
  tmrTimeInSeconds_t iTimeout;

  /*-------------------------------------------------------------------------*/
  if(ZDO_GetEvent() == gZdoEvent_StartingDiscovery_c)
  {
    /* Let the application know that the node is in discovery mode. */
    Zdo_GenerateEventZdo2App(gZDOToAppMgmtNwkDiscovery_c);

#if gCoordinatorCapability_d || gComboDeviceCapability_d
#if gComboDeviceCapability_d
    /* discovery machine behaves differently for ZC vs. ZR */
    if(NlmeGetRequest(gDevType_c) == gCoordinator_c)
#endif
    {
      /* # of attempts depends on formation attempts */
      mcDiscoveryRetries = gBeeStackConfig.gNwkFormationAttempts;
      iTimeout = gBeeStackConfig.gFormationAttemptsTimeOut;

      /* start doing an energy scan */
      ZDO_SendEvent(gZdoEvent_EnergyScan_c);
    }
#endif

#if gComboDeviceCapability_d
    else
#endif
#if (!gCoordinatorCapability_d )
    {
      /* how many times to discover networks for joining purposes? */
      mcDiscoveryRetries = gBeeStackConfig.gNwkDiscoveryAttempts;
      iTimeout = gBeeStackConfig.gDiscoveryAttemptsTimeOut;
      ZDO_SendEvent(gZdoEvent_NextDiscovery_c);
    }
#endif

    /* this is an overall timeout (independant of retries and scan times) */
    if(iTimeout)
    {
      /* if this timer timers out while in discovery state, will go to stopped state */
      TMR_StartSecondTimer( gZdoDiscoveryAttemptTimerID, iTimeout, ZDO_StateMachineTimerCallBack );
    }

    /* prepare for discovery */
    mDiscoveryFirstTime = TRUE;
    mcScanRetries = NlmeGetRequest(gNwkScanAttempts_c);
  }

#if gCoordinatorCapability_d || gComboDeviceCapability_d
  /*-------------------------------------------------------------------------*/
  /* only ZCs need an energy scan during formation discovery */
  else if(ZDO_GetEvent() == gZdoEvent_EnergyScan_c)
  {
    ZdoNwkMng_EnergyScanRequest(ApsmeGetRequest(gApsChannelMask_c),gScanDuration_c);
  }

  /* done with scan */
  else if(ZDO_GetEvent() == gZdoEvent_EnergyScanComplete_c)
  {
    ZDO_SendEvent(gZdoEvent_NextDiscovery_c);
  }
#endif

  /*-------------------------------------------------------------------------*/
  /* results of a network discovery came back from NWK layer */
  else if(ZDO_GetEvent() == gZdoEvent_DiscoveryFailure_c || ZDO_GetEvent() == gZdoEvent_DiscoverySuccess_c)
  {
    /*
      OK, network discovery is done, now use that info to try to form (ZC) or join (ZR, ZED)
      the network on success only.
    */
    mcScanRetries--;    
    if (mcScanRetries == 0)
    {
#if gCoordinatorCapability_d || gComboDeviceCapability_d
#if gComboDeviceCapability_d
      if (NlmeGetRequest(gDevType_c) == gCoordinator_c)
#endif
      {
        /*
          After scanning Coordinator devices will jump to forming no matter what, so on
          Silent Nwk Rejoin or network rejoin will do a forming any way.
        */
        ZDO_SetState(gZdoFormingState_c);
        ZDO_SendEvent(gZdoEvent_Forming_c);
      }
#endif
#if gComboDeviceCapability_d
      else  //if(NlmeGetRequest(gDevType_c) == gCoordinator_c)
#endif
#if !gCoordinatorCapability_d
      {
        //if (ZDO_GetEvent() == gZdoEvent_DiscoverySuccess_c)
        {
          /* Any other device after a scan will go to joining, no matter the startup mode. */
          ZDO_SetState(gZdoJoiningState_c);
          ZDO_SendEvent(gZdoEvent_Joining_c);
        }
      }
#endif
      /* Stop the discovery timer here to avoid the time out. */
      TMR_StopTimer(gZdoDiscoveryAttemptTimerID);
      return;
    }

    /* start the between scans timer when done will generate the gZdoEvent_NextDiscovery_c event */
    TMR_StartSingleShotTimer(gTimeBetScansTimerID,
                             NlmeGetRequest(gNwkTimeBtwnScans_c),
                             ZDO_StateMachineTimerCallBack );
  }

  /* time between scans done. Check if we need to scan again, or move on to joining */
  else if( ZDO_GetEvent() == gZdoEvent_NextDiscovery_c )
  {
    /* mcDiscoveryRetries == 0 means try forever, otherwise try for that # of retries */
    if( mcDiscoveryRetries )
    {
      /* don't decrement the first time, as we don't want to stop if only set to 1 attempt */
      if(mDiscoveryFirstTime) 
      {        
        mDiscoveryFirstTime = FALSE;
      }
      else 
      { /*only decrement the nwk discovery counter, if scan attempts are done.*/
        if (mcScanRetries == NlmeGetRequest(gNwkScanAttempts_c)) 
        {        
          mcDiscoveryRetries--;      
          if( !mcDiscoveryRetries )
          {
            /* tried that # of times to discover a network. Give up and stop. */
            TMR_StopTimer( gZdoDiscoveryAttemptTimerID );
            ZDO_Stop();
            return;
          }
        }      
      }
    }

    /* do the NLME-DISCOVER-NETWORKS.request */
    ZdoNwkMng_GenerateNwkDiscoveryReq();
  }

  /*-------------------------------------------------------------------------*/
  /* total state machine timed out, see if there any valid parents to form or join */
  else if( ZDO_GetEvent() == gZdoEvent_Timeout_c )
  {
    ZDO_Stop();
  }
}

/*
  Helper function. This is the common code between forming and joining. Used by both the forming and the joining states.

  1. If forming/joining is successful, will move on to starting the router (ZC or ZR)
  2. If starting is successful, then will move on to running state.
  3. If either is not successful, will move back to discovery state, which may try again or stop, depending on user settings.
*/
void ZDO_StateStartRouterCheck(void)
{
  switch(ZDO_GetEvent())
  {
    /* success on form or join, move on to starting the ZC/ZR */
#if ( gRouterCapability_d || gCoordinatorCapability_d ) || (gComboDeviceCapability_d) 
    case gZdoEvent_FormationSuccess_c:
    case gZdoEvent_JoinSuccess_c:
#if ( gRouterCapability_d || gCoordinatorCapability_d || gComboDeviceCapability_d)
#if ( gComboDeviceCapability_d )
      if(NlmeGetRequest(gDevType_c) != gEndDevice_c)
#endif
      { 
        ZdoNwkMng_GenerateStartRouterReq();
      }
#endif
    break;
#endif
    /* failed on form, join, or start go back to discovery state, and try again */
    case gZdoEvent_FormationFailure_c:
    case gZdoEvent_JoinFailure_c:
    case gZdoEvent_StartRouterFailure_c:

    /* Sleepy ZEDs can go to sleep while waiting for next discovery */
#if gEndDevCapability_d || gComboDeviceCapability_d
#if gComboDeviceCapability_d
      if ( NlmeGetRequest( gDevType_c ) == gEndDevice_c )
#endif
      {
        if(!IsLocalDeviceReceiverOnWhenIdle())
        {
          ZDO_AllowDeviceToSleep();
          (void)ZDO_NLME_ChangePollRate(gZdoOrgPollRate);
        }
      }
#endif

      /*
        Upper bound on Rejoin Interval (OrphanScanAttemptsTimeOut), in seconds.
        This is the maximum value that mDefaultValueOfRejoinInterval_c can take.
      */
      if (NlmeGetRequest(gRejoinInterval_c) > NlmeGetRequest(gMaxRejoinInterval_c))
      {
        NlmeSetRequest(gRejoinInterval_c, NlmeGetRequest(gMaxRejoinInterval_c));
      }
      /*if the last discovery attempt was just made, then dont start timer, but just signal the discovery event so that the discovery network 
      state can handle the appropiate action without waiting for the rejoin timer.
      */
      if (mcDiscoveryRetries == 1) {
        ZDO_SendEvent(gZdoEvent_BetweenDiscoveryTime_c);      
      } else
      /* start up the in-between timer */
      TMR_StartSecondTimer( gZdoFormationAttemptTimerID, NlmeGetRequest(gRejoinInterval_c), ZDO_StateMachineTimerCallBack );
    break;

    /* done waiting, discover again */
    case gZdoEvent_BetweenDiscoveryTime_c:

      /* copy in full channel list, since preferred one failed... */
      if(!BeeUtilArrayIsFilledWith((uint8_t *)gaFullChannelList, 0, sizeof(gaFullChannelList)))
        ApsmeSetRequest(gApsChannelMask_c, (void *)gaFullChannelList);

      /* sleepy ZEDs should go back to slow polling rate for discovery */      
#if gEndDevCapability_d || gComboDeviceCapability_d
#if gComboDeviceCapability_d
      if ( NlmeGetRequest( gDevType_c ) == gEndDevice_c )
#endif
      {
        if ( !IsLocalDeviceReceiverOnWhenIdle() )
        {
          ZDO_DisallowDeviceToSleep();
        }
      }
#endif

      /* prepare for discovery */  
      mcScanRetries = NlmeGetRequest(gNwkScanAttempts_c);

      /* go back to discovery */
      ZDO_SetState(gZdoDiscoveringNetworksState_c);
      ZDO_SendEvent(gZdoEvent_NextDiscovery_c);
    break;

    /* nothing to do on default */
    default:
    break;
  }

}

#if (gCoordinatorCapability_d || gComboDeviceCapability_d)
/*
  Forming the network (ZC, Combo)

  Possible events for this state:
  gZdoEvent_Forming_c:             attempt formimg
  gZdoEvent_FormationSuccess_c:    success, finish forming
  gZdoEvent_FormationFailure_c:    failed, try again if any retries left. Otherwise, fail it.
  gZdoEvent_StartRouterSuccess_c:  used for both forming and joining, turns on the radio
  gZdoEvent_StartRouterFailure_c:  failed to start the ZC
  gZdoEvent_Timeout_c:             wasn't completed in time, give up
*/
void ZDO_StateMachineForming(void)
{
  switch(ZDO_GetEvent())
  {
    /*If Network Formation event is received call the network formation request*/
    case gZdoEvent_Forming_c:
      Zdo_GenerateEventZdo2App ( gZDOToAppMgmtZCStarting_c );

      if(ZDO_GetStartMode() == gZdoStartMode_Associate_c)
        ZdoNwkMng_GenerateNetworkFormation();
      else
        ZDO_SendEvent(gZdoEvent_FormationSuccess_c);
    break;

    /* everything successful, go on to running state */
    case gZdoEvent_StartRouterSuccess_c:
      /* done, don't timeout anymore */
      TMR_StopTimer(gZdoDiscoveryAttemptTimerID);
    
      /* move to running state */
      ZDO_SetState(gZdoRunningState_c);
      ZDO_SendEvent(gZdoEvent_StartSuccess_c);
    break;

    /* handle the events common to the forming and joining state */
    default:
      ZDO_StateStartRouterCheck();
    break;
  }

}
#endif

/* ZR, ZED, Combo */
#if !gCoordinatorCapability_d
/*
  Joining a network (ZR, ZED, Combo)

  This node is in the joining process. After joining, it will go to authenticating (if 
  security), then to running.

  Possible events for this state:
  gZdoEvent_Joining_c:             attempt joining
  gZdoEvent_JoinSuccess_c:         success, finish joining
  gZdoEvent_JoinFailure_c:         failed, try again if any retries left. Otherwise, fail it.
  gZdoEvent_StartRouterSuccess_c:  used for both forming and joining, turns on the radio
  gZdoEvent_StartRouterFailure_c:  failed to start the ZC
  gZdoEvent_Timeout_c:             wasn't completed in time, give up

  Possible next states:
  gZdoDeviceAuthenticationState_c   joined, now authenticate in a secure network
  gZdoDiscoveringNetworksState_c    failed to join, perhaps try again
*/
void ZDO_StateMachineJoining(void)
{
  switch(ZDO_GetEvent())
  {
    /* time to try to join the network */
    case gZdoEvent_Joining_c:
      Zdo_GenerateEventZdo2App(gZDOToAppMgmtJoinNwk_c);

      /* Check if an address needs to be generated */
      ZDO_CheckForNwkRejoin(gZDOJoinMode);

      /* attempt join the network (join mode may be any of the 5 join modes) */
      ZdoNwkMng_GenerateNwkJoinRequest( gZDOJoinMode );
    break;

    /* Router is now started, go on to authenticating state */
    case gZdoEvent_StartRouterSuccess_c:
#if (gStandardSecurity_d || gHighSecurity_d) && (!gTrustCenter_d || gComboDeviceCapability_d)
#if gComboDeviceCapability_d
      if (!gTrustCenter)
#endif
      {
        ZDO_SetState(gZdoDeviceAuthenticationState_c);
        ZDO_SendEvent(gZdoEvent_AuthenticateStart_c);
      }
#else
      /* done, don't timeout anymore */
      TMR_StopTimer(gZdoDiscoveryAttemptTimerID);

      /* move to running state */
      ZDO_SetState(gZdoRunningState_c);
      ZDO_SendEvent(gZdoEvent_StartSuccess_c);
#endif
    break;

    case gZdoEvent_JoinSuccess_c:
#if (gEndDevCapability_d || gComboDeviceCapability_d)
#if gComboDeviceCapability_d
      if(NlmeGetRequest(gDevType_c) == gEndDevice_c)
#endif
      {
        /* if using security ZED needs to authenticate first */
#if gStandardSecurity_d || gHighSecurity_d
        /* Association ALWAYS go through authentication state. */
        if (ZDO_GetStartMode() == gZdoStartMode_Associate_c)
        {
          ZDO_SetState(gZdoDeviceAuthenticationState_c);
          ZDO_SendEvent(gZdoEvent_AuthenticateStart_c);
          ApsmeSetRequest(gApsUseInsecureJoin_c, gZDO_SecureRejoin);
        }
        /*
          On Nwk Rejoin only go to Authentication state if the use of insecure rejoin is
          available.
        */
        else if ((ZDO_GetStartMode() == gZdoStartMode_NwkRejoin_c) && (ApsmeGetRequest(gApsUseInsecureJoin_c)))
        {
          ZDO_SetState(gZdoDeviceAuthenticationState_c);
          ZDO_SendEvent(gZdoEvent_AuthenticateStart_c);
          ApsmeSetRequest(gApsUseInsecureJoin_c, gZDO_SecureRejoin);
        }
        /*
          If we are not associating or rejoining with the insecure rejoin set to TREU,
          then, go directly to running state.
        */
        else
        {
          /* move to running state */
          ZDO_SetState(gZdoRunningState_c);
          ZDO_SendEvent(gZdoEvent_StartSuccess_c);
        }
#else
        /* move to running state */
        ZDO_SetState(gZdoRunningState_c);
        ZDO_SendEvent(gZdoEvent_StartSuccess_c);
#endif
      }
#endif

      /* join succedded ZR should start router */
#if (gRouterCapability_d || gComboDeviceCapability_d)
#if gComboDeviceCapability_d
      if(NlmeGetRequest(gDevType_c) == gRouter_c)
#endif
      {
        ZDO_StateStartRouterCheck();
        ApsmeSetRequest(gApsUseInsecureJoin_c, gZDO_SecureRejoin);
      }
#endif
    break;

    /* total state machine timed out, see if there any valid parents to form or join */
    case gZdoEvent_Timeout_c:
      ZDO_Stop();
    break;


    default:
      /* if join failure, go back to discovering */
      ZDO_StateStartRouterCheck();
    break;
  }

}
#endif

#if gStandardSecurity_d || gHighSecurity_d
#if (gRouterCapability_d || gEndDevCapability_d || gComboDeviceCapability_d)
/*
  Authenticating.

  The device is asking the TrustCenter if it's OK to be on the network. At this stage the device 
  has joined the network, but without the proper keys and permission yet.
*/
void ZDO_StateMachineAuthenticatingDevice(void)
{
  switch(ZDO_GetEvent())
  {
    case gZdoEvent_AuthenticateStart_c:

      /* Assume any case besides Association and rejoing needs authentication. */
      if (((gZDOJoinMode != gAssociationJoin_c) && (gZDOJoinMode != gNwkRejoin_c)) ||
          ((gZDOJoinMode == gNwkRejoin_c) && !ApsmeGetRequest(gApsUseInsecureJoin_c)))
      {
        ZDO_SendEvent(gZdoEvent_AuthenticationSuccess_c);
        break;
      }

      /* Secure ZRs and ZEDs need to authenticate when associating */
#if gEndDevCapability_d || gComboDeviceCapability_d

      /* Only change the poll rate if and only if authentication is needed. */
#if gComboDeviceCapability_d
      if ( NlmeGetRequest( gDevType_c ) == gEndDevice_c )
#endif
      {
        if (!IsLocalDeviceReceiverOnWhenIdle())
        {
          /*
            Original poll rate has been saved in gZdoOrgPollRate it will be changed
            back in running state.
          */
          if (gZdoOrgPollRate > NlmeGetRequest(gAuthenticationPollTimeOut_c))
          {
            /* Sleepy ZEDs need to increase polling rate for joining */
            (void)ZDO_NLME_ChangePollRate(NlmeGetRequest(gAuthenticationPollTimeOut_c));
          }
        }
      }
#endif

#if gSKKESupported_d
      ZDO_SetState(gZdoDeviceWaitingForKeyState_c);
#else
      ZDO_SetState(gZdoDeviceAuthenticationState_c);
#endif

      /*
        Set the global authentication timer. This timer prevent the node to tries for
        ever if it is not allow in the network or somethig fails during the authentication.
      */
      TMR_StartSingleShotTimer( gZdoEstablishKeysTimerID,
                                gBeeStackConfig.gAuthenticationTimeOut,
                                ZDO_StateMachineTimerCallBack );

      /* Send an event to Application */
      Zdo_GenerateEventZdo2App ( gZDOToAppMgmtEstablishKey_c );

     /*-------- Passed Authentication -------------*/
    break;

    case gZdoEvent_AuthenticationSuccess_c:
     /* Stop the timers */
     TMR_StopTimer( gZdoEstablishKeysTimerID );
     TMR_StopTimer( gZdoDiscoveryAttemptTimerID );

     ZDO_SetState(gZdoRunningState_c);
     ZDO_SendEvent(gZdoEvent_StartSuccess_c);
    break;

    /*-------- Failed Authentication -------------*/
    case gZdoEvent_AuthenticationFailure_c:
    case gZdoEvent_Timeout_c:
      /* ZED: go back to standard polling rate */
#if gEndDevCapability_d || gComboDeviceCapability_d
#if gComboDeviceCapability_d
      if(NlmeGetRequest(gDevType_c) == gEndDevice_c)
#endif
      {
        NWK_MNGTSync_ChangePollRate(gZdoOrgPollRate);
      }
#endif
      ZDO_SecEstablishKeyTimeExpired();
    break;
  }

}
#endif  (gRouterCapability_d || gEndDevCapability_d || gComboDeviceCapability_d)
#endif (gStandardSecurity_d || gHighSecurity_d)

/*
  Device is now on the network, running normally. Inform the app and finish up.
*/
void ZDO_StateMachineRunningState(void)
{
  /*
    Only run this stuff first time. If we go back to running state from performing remote 
    commands (such as scan for networks), no need to get back on the network
  */
  if(ZDO_GetEvent() != gZdoEvent_StartSuccess_c)
  {
    return;
  }

  /* Sleepy ZEDs should go back to original polling rate */
#if (gEndDevCapability_d || gComboDeviceCapability_d)
#if gComboDeviceCapability_d
  if (NlmeGetRequest(gDevType_c) == gEndDevice_c)
#endif
  {

    /* if the device is sleeping then set gLpkIncluded to TRUE. Remember gLpmIncluded always is enabled for combo devices */
    if( !IsLocalDeviceReceiverOnWhenIdle() )
    {
#if (gComboDeviceCapability_d)
      gLpmIncluded = TRUE;
#endif
      ZDO_AllowDeviceToSleep();
      (void)ZDO_NLME_ChangePollRate(gZdoOrgPollRate);
    }
  }
#endif

  /* set the state to the proper running state */
  ZDO_SetState(maZdoRunning[NlmeGetRequest(gDevType_c)]);

#if gConcentratorFlag_d && (gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d)
#if gComboDeviceCapability_d
  if( NlmeGetRequest(gDevType_c) != gEndDevice_c )
#endif
  {
    /*
      Zigbee spec. 053474r17
      3.6.3.5.1 Initiation of Route Discovery
      If a device has nwkIsConcentrator equal to TRUE and there is a non-zero value in
      nwkConcentratorDiscoveryTime, the network layer should issue a route request
      command frame each nwkConcentratorDiscoveryTime.
    */
    if( NlmeGetRequest(gNwkIsConcentrator_c) && NlmeGetRequest(gNwkConcentratorDiscoveryTime_c))
      TMR_StartMinuteTimer(gNwkConcentratorDiscoveryTimerID, NlmeGetRequest(gNwkConcentratorDiscoveryTime_c), ZDO_StateMachineTimerCallBack);
  }
#endif

  /*
    If the deivce have done a NwkRejoin, then we must set the value of NwkExtendedPanId
    if it is all zeros.
  */
  if (ZDO_GetJoinMode() == gZdoStartMode_NwkRejoin_c)
  {
    if (Cmp8BytesToZero(NlmeGetRequest(gNwkExtendedPanId_c)))
    {
      NlmeSetRequest(gNwkExtendedPanId_c, ApsmeGetRequest(gApsUseExtendedPANID_c));
    }
  }
  else
  {
    /*
      Silent nwk rejoin or any other association type should update the
      ApsUseExtendedPanId field.
    */
    if (Cmp8BytesToZero(ApsmeGetRequest(gApsUseExtendedPANID_c)))
    {
      ApsmeSetRequest(gApsUseExtendedPANID_c, NlmeGetRequest(gNwkExtendedPanId_c));
    }
  }

  /* tell the world we have joined/formed */
#if (gDevice_annce_d && !gCoordinatorCapability_d)
#if gComboDeviceCapability_d
  if(NlmeGetRequest(gDevType_c) != gCoordinator_c)
#endif
  {
    /* Process the routes and  */
    ZdoNwk_ClearSiblingOnNeighborTable(ZDO_GetStartMode());

    if (ZDO_GetJoinMode() != gZdoStartMode_SilentStart_c)
    {
      /* Announce the capabilities and the device's info to everybody in range. */
      DeviceAnnounce((uint8_t *)gaBroadcastRxOnIdle);
    }

      /* We need to let know to our new neighbors that we are here. */
#if gRouterCapability_d || gComboDeviceCapability_d
    if ((NlmeGetRequest(gDevType_c) == gRouter_c) && !gActAsZed)
    {
      /* This will only have effect if the network is SP2 */
      TS_SendEvent(gNwkTaskID_c, gNwkSendLinkStatus_c);
    }
#endif
  }
#endif

#if gFrequencyAgilityCapability_d && gComboDeviceCapability_d
  /* Combo device acting like ZEd should not Init the Frequency Agility State Machine. */
  if (NlmeGetRequest(gDevType_c) != gEndDevice_c)
  {
    FA_StateMachineInit();
  }
#endif

  /* Set the right server mask. */
  ZDO_ServerMaskInit();

  /* Make sure incremented counters, or commissionning values are saved to NVM */
  ZdoNwkMng_SaveToNvm(zdoNvmObject_All_c);

  /* don't do the startup stuff twice */
  gZdoEvent = gZdoEvent_Unused_c;

  /*
    Tell application about running state, this procedure is asynchronous so it will not
    set any event it be call the application's callback and wont leave until the application
    have done executing thi event.
  */
  Zdo_GenerateEventZdo2App( maZdo2AppEvents[NlmeGetRequest(gDevType_c)] );
}

/*
  Handling a remote command, such as scanning for networks on behalf of the network manager.
  That means that temporarily, this node cannot send/receive data.
*/
void ZDO_StateMachineRemoteCommands(void)
{
  /* set the state to the proper running state */
  if (ZDO_GetEvent() == gZdoEvent_ManagementResponse_c)
  {
    /* A response was sent, we need to go back to running state. */
    ZDO_SetState(maZdoRunning[NlmeGetRequest(gDevType_c)]);
  }
}

/*
  Leaving

  Announces that this device is leaving (possibly telling it's children to leave), then 
  stops. Can be called at any time.
*/
void ZDO_StateMachineLeaving(void)
{
  /*-------------------------------------------------------------------------*/      
  if( ZDO_GetEvent() == gZdoEvent_ChildLeaveSuccess_c )
  {
    /* one child leave notification per child */
    Zdo_GenerateEventZdo2App( gZDOToAppMgmtLeavingNwk_c );
  }

  /*-------------------------------------------------------------------------*/
  else if (ZDO_GetEvent() == gZdoEvent_SelfLeaveSuccess_c || ZDO_GetEvent() == gZdoEvent_Timeout_c)
  {
    /* leave from above (APP) or below (OTA) has finished */
    ZDO_SendEvent(gZdoEvent_Stop_c);
    ZDO_SetState(gZdoStoppingState_c);
  }

  /*-------------------------------------------------------------------------*/
  else if (ZDO_GetEvent() == gZdoEvent_LeaveNetwork_c )
  {
    /*
      start a timer in case leave request fails. We still need to stop. if it succed Leave
      indication will command the ZDO_Stop and the layers, timers will be reset.
    */
    TMR_StartSingleShotTimer( gZdoLeavingTimerID,
                                5000,     // 5 seconds, enough time for all children
                                ZDO_StateMachineTimerCallBack );

    Zdo_GenerateEventZdo2App( gZDOToAppMgmtLeavingNwk_c );
    /* Leave the network after waiting some little time. */
    ZdoNwkMng_GenerateNlmeLeaveRequest();
  }
}

/*
  Stopping. Can be called at any time.
*/
void ZDO_StateMachineStopping(void)
{
  zdoNlmeMessage_t nlmeMsg;
  apsmeMessage_t apsmeMsg;
  bool_t fFullReset;

  /* ZED: go back to standard polling rate */
#if gEndDevCapability_d || gComboDeviceCapability_d
#if gComboDeviceCapability_d
  if(NlmeGetRequest(gDevType_c) == gEndDevice_c)
#endif
  {
    NWK_MNGTSync_ChangePollRate(gZdoOrgPollRate);
  }

  /* ZED: allow to sleep again (all other states but running and initial state prevent sleeping) */
  if(ZDO_GetState() != gZdoRunningState_c)
  {
    ZDO_AllowDeviceToSleep();
  }
#endif

  /*
    If the incomming stop mode is with announce then we must leave the network and
    then stop.
  */
  if (gZdoStopMode & gZdoStopMode_Announce_c)
  {
    /*
      Clearing the bit is important to avoid looping in the stop state doing the same
      action over and over again.
    */
    gZdoStopMode &= ~gZdoStopMode_Announce_c;

    /* leave the network (which will return to stopping state */
    ZDO_SetState(gZdoLeavingState_c);
    ZDO_SendEvent(gZdoEvent_LeaveNetwork_c);
    return;
  }

  /* reset zdo: Purge the message queues  */
  ZdoReset();

  fFullReset = gZdoStopMode & gZdoStopMode_ResetTables_c;

  /* reset everything */

  /* Send the reset request to APS layer. */
  ZDO_ApsReset(fFullReset, &apsmeMsg);

  /* Reset AF layer queues */
  AF_Reset();

  /* Reset App layer queues */
  AppResetApplicationQueues();

  /*
    Back to the initial state (must happen before ZDO_Start() below, and before letting
    the application know about this.
  */
  ZDO_SetState(gZdoInitialState_c);

  /* No more events for the Zdo State machine. */
  TS_ClearEvent(gZdoStateMachineTaskID_c,0xFFFF);

  /* let application know it's stopped */
  Zdo_GenerateEventZdo2App(gZDOToAppMgmtStopped_c);

  /*****************************************************************************************
  * WARNING: MAC Should be the last layer to be reset, Due that resets the memory 
  * subsystem and frees all the buffers, no function freeing buffers should be added after
  * this call, may generate double buffer freeing error. This reset layer, should be the
  * last layer to be reset in this function ALWAYS.
  ******************************************************************************************/
  /* Reset Network Layer (also resets MAC layer) */
  nlmeMsg.msgType = gNlmeResetRequest_c;
  nlmeMsg.msgData.resetReq.fFullReset = fFullReset;
  (void)ZDO_NLME_SapHandler(&nlmeMsg);
/* WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING
   WARNING          No buffer should be freed after reset the mac layer..          WARNING
   WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING */

  /* save NVM */
  if(gZdoStopMode & gZdoStopMode_ResetNvm_c)
  {
    /* Ensure to save inmediately. */
    ZdoNwkMng_SaveToNvm(zdoNvmObject_AtomicSave_c);
    /*
      Clearing the bit is important to avoid looping in the stop state doing the same
      action over and over again.
    */
    gZdoStopMode &= ~gZdoStopMode_ResetNvm_c;
  }

  /* restart if requested */
  if(gZdoStopMode & gZdoStopMode_StopAndRestart_c)
  {
    gZdoStopMode &= ~gZdoStopMode_StopAndRestart_c;
    /*
      Clearing the bit is important to avoid looping in the stop state doing the same
      action over and over again.
    */
    ZDO_Start(gZdoStartModeDefault_c);
  }
}

/*
  The ZDO_StateMachine handles starting and stopping of the device on the network.
  It understands starting a device as a ZC, ZR or ZED, or Combo device.

  Compile switches can limit it to a particular device type. See:
  gCoordinatorCapability_d, gRouterCapability_d, gEndDevCapability_d, gComboDeviceCapability_d
*/
void ZDO_StateMachine(void)
{
  /* Leaving and stopping can happen any time, any state */
  if(ZDO_GetEvent() == gZdoEvent_SelfLeaveSuccess_c)
  {
    ZDO_SetState(gZdoLeavingState_c);
  }

  /* an event is acting on the current state */
  switch(ZDO_GetState())
  {
    /* initial state is not yet on the network */
    /* This is the only state that handles ZDO_Start() request */
    case gZdoInitialState_c:
      ZDO_StateMachineInitialState();
    break;

    /* we now have a list of networks, go do something with it */
    case gZdoDiscoveringNetworksState_c:
      ZDO_StateMachineDiscoveringNetworks();
    break;

/* ZC or Combo */
#if (gCoordinatorCapability_d || gComboDeviceCapability_d)
    /* time to form the network */
    case gZdoFormingState_c:
      ZDO_StateMachineForming();
    break;
#endif

/* ZR, ZED, Combo */
#if (!gCoordinatorCapability_d || gComboDeviceCapability_d)
    /* time to join the network */
    case gZdoJoiningState_c:
      ZDO_StateMachineJoining();
    break;

#if (gStandardSecurity_d || gHighSecurity_d)
    /* now the security object has control, will go to running or leaving state */
    case gZdoDeviceAuthenticationState_c:
      ZDO_StateMachineAuthenticatingDevice();
    break;
#endif
#endif (!gCoordinatorCapability_d || gComboDeviceCapability_d)

    /* done authenticating, now running, finish up last network setup */
    case gZdoRunningState_c:
      ZDO_StateMachineRunningState();
    break;

    /* executing a remote command (e.g. network discovery) */
    case gZdoRemoteCommandsState_c:
      ZDO_StateMachineRemoteCommands();
    break;

    /* leaving (this tells others that we're leaving) */
    case gZdoLeavingState_c:
      ZDO_StateMachineLeaving();
    break;

    /* stopping the network. May be called at any time. */
    case gZdoStoppingState_c:
      ZDO_StateMachineStopping();
    break;
  }

}

/*
  Called when something in the ZDO state machine times out.
*/
void ZDO_StateMachineTimerCallBack(uint8_t timerId)
{
  /* Discovery timer is used for the total form/join timeout (in seconds). */
  if(timerId == gZdoDiscoveryAttemptTimerID)
  {
    // If we are in the process of forming or joining and the globaltime out expires then pull the breaks.
    if (ZDO_GetState()== gZdoDiscoveringNetworksState_c)
      ZDO_Stop();
  }

  /* formation timer is used for Between Discoveries time (in seconds) */
  /* note: also used for leaving (gZdoLeavingTimerID), but not at the same time */
  if((timerId == gZdoFormationAttemptTimerID) || timerId == gZdoLeavingTimerID)
  {
    // must be leaving timed out...
    if(ZDO_GetState() == gZdoLeavingState_c)
      ZDO_SendEvent(gZdoEvent_Timeout_c);

    // must be forming/joining state...
    else
      ZDO_SendEvent(gZdoEvent_BetweenDiscoveryTime_c);
  }

  /* cannot find a parent, give up */
  if(timerId == gZdoOrphanScanAttemptTimerID)
  {
    ZDO_SendEvent(gZdoEvent_Timeout_c);
  }

  /* timed out */
  if ( timerId == gZdoEstablishKeysTimerID )
  {
    ZDO_SetState(gZdoDeviceAuthenticationState_c);
    ZDO_SendEvent(gZdoEvent_AuthenticationFailure_c);
  }

  /* done waiting between network discoveries, try next one */
  if(timerId == gTimeBetScansTimerID)
  {
    ZDO_SendEvent(gZdoEvent_NextDiscovery_c);
  }

#if gConcentratorFlag_d
  /* Is time to send an Many-to-one Nlme-Route-Discovery.request */
  if( timerId == gNwkConcentratorDiscoveryTimerID )
  {
    /* We pass the radius setted by the user. The NoRouteCache is True if we don't have memory otherwise False */
    ASL_SendRouteDiscoveryManyToOne(NlmeGetRequest(gNwkConcentratorRadius_c), !gNwkHighRamConcentrator);

    /* Start again the timer if and only if the time is not zero */
    if( NlmeGetRequest(gNwkConcentratorDiscoveryTime_c) )
    {
      TMR_StartMinuteTimer(gNwkConcentratorDiscoveryTimerID, NlmeGetRequest(gNwkConcentratorDiscoveryTime_c), ZDO_StateMachineTimerCallBack);
    }
  }
#endif
}

/*
  If the node has a preconfugred Exteneded PanId it must do a NwkRejoin and generate its
  own NWk address.
*/
void ZDO_CheckForNwkRejoin(zbNwkJoinMode_t gZdoJoinMode)
{
#if gStochasticAddressingCapability_d
  uint8_t iCapInfo;
  zbNwkAddr_t  aTempAddr = {0xFF,0xFF};
  if (gZdoJoinMode != gNwkRejoin_c)
  {
    return;
  }

  if (Cmp8BytesToZero(ApsmeGetRequest(gApsUseExtendedPANID_c)))
  {
    return;
  }

  if (IsValidNwkUnicastAddr(NlmeGetRequest(gNwkShortAddress_c)))
  {
    return;
  }

  NWK_ADDRESSASSIGN_GenerateAddressStochasticMode(aTempAddr);
  if (IsBroadcastAddress(aTempAddr))
  {
    return;
  }

  NlmeSetRequest(gNwkShortAddress_c, aTempAddr);

  /*
    053474r17 p387 lines 9 to 16
    If the device is joining this network for the first time, and the value of the
    nwkNetworkAddress attribute of its NIB has a value of 0xffff indicating that it is
    not currently joined to a network, the device shall select a 16-bit network address
    for itself and set the nwkNetworkAddress attribute to this value. The address
    should be randomly selected according to the procedures outlined in subclause
    3.6.1.7. In this case, and in any case where the nwkAddrAlloc attribute of
    the NIB has a value of 0x02 indicating stochastic addressing, the allocate address
    sub-field of the capability information field of the command payload shall be set
    to 0 indicating a self-selected address.
  */
  if( NlmeGetRequest(gNwkAddrAlloc_c) == 0x02 )
    {
        /* Get the actual device capabilities*/
        iCapInfo = NlmeGetRequest(gNwkCapabilityInformation_c);
        /* Setting to 0 the allocate address subfield */
        iCapInfo &=0x7F;
        /* Updating the device capabilities */
        NlmeSetRequest(gNwkCapabilityInformation_c, &iCapInfo);
    }
    
#else
(void)gZdoJoinMode;
#endif
}

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
void ZDO_ProcessDeviceNwkRejoin(void)
{
  if ((NlmeGetRequest(gDevType_c) != gEndDevice_c) && !gActAsZed)
  {
    return;
  }

  /* For Tx failures Rejoin must be done with the RAM info.  */
  //BeeUtilZeroMemory(NlmeGetRequest(gNwkExtendedPanId_c), sizeof(NlmeGetRequest(gNwkExtendedPanId_c)));
  ZDO_Start(gZdoStartMode_NwkRejoin_c | gZdoStartMode_RamSet_c);
}
#endif


#if gEndDevCapability_d || gComboDeviceCapability_d
void ZDO_StopPolling(void)
{
#if gComboDeviceCapability_d
  if (NlmeGetRequest(gDevType_c) != gEndDevice_c)
  {
    return;
  }
#endif
  gZdoOrgPollRate = NlmeGetRequest(gNwkIndirectPollRate_c);
  TMR_StopTimer(gPollTimerID);
}
#endif

/*
  Helper function to set the right server mask in the case of combo devices.
  The fact is that polling End Devices can not be any type of server.
*/
void ZDO_ServerMaskInit(void)
{
/*-------------------- gaServeMask --------------------
  Table 2.29 Server Mask Bit Assignments

  Bit number:
    0->Primary Trust Center.
    1->Backup Trust Center.
    2->Primary Binding table cache.
    3->Backup bindin table cache.
    4->Primary Discovery Cache.
    5->Backup Discovery Cache.
    6->Network Manager.
    7-15->Reserved.
*/

/* A polling ZED can not be a server of any kind. */
#if gComboDeviceCapability_d
  if (NlmeGetRequest(gDevType_c) == gEndDevice_c)
  {
    /* Just check for polling devices. */
    if (!IsLocalDeviceReceiverOnWhenIdle())
    {
      /* Clear the server mask. */
      BeeUtilZeroMemory(gaServerMask, sizeof(gaServerMask));
    }
  }
#endif
}

/* Sends the reset request to the APS layer. */
void ZDO_ApsReset(bool_t fFullReset, apsmeMessage_t *pApsmeMsg)
{
  pApsmeMsg->msgData.resetTypeReq = 0;
  /* reset everything */
  if (fFullReset)
  {
    pApsmeMsg->msgData.resetTypeReq = gApsmeResetAllAib_c;
    /*
      Clearing the bit is important to avoid looping in the stop state doing the same
      action over and over again.
    */
    gZdoStopMode &=  ~gZdoStopMode_ResetTables_c;
  }
  pApsmeMsg->msgType = gApsmeReset_c;
  ( void )ZDO_APSME_SapHandler( pApsmeMsg );
}

void ZDO_SetLogicalChannel(void)
{
  uint8_t iLogicalChannel=0;

  if (!NlmeGetRequest(gNwkLogicalChannel_c))
  {
    iLogicalChannel = BeeUtilBitToIndex(ApsmeGetRequest(gApsChannelMask_c),4);
    NlmeSetRequest(gNwkLogicalChannel_c,&iLogicalChannel);
  }
}


/******************************************************************************
*******************************************************************************
* Private Debug Stuff
*******************************************************************************
******************************************************************************/

/* None */
