/******************************************************************************
* BeeAppInit.c
*
* Initialization common to all applications. The very start of the program,
* main(), is found here.
*
* Copyright (c) 2008, Freescale, Inc. All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor
.
*******************************************************************************/
#ifndef __IAR_SYSTEMS_ICC__
#ifdef MC13226Included_d
    #undef MC13226Included_d
#endif    
    #define MC13226Included_d 0
#endif

#include "EmbeddedTypes.h"
#include "BeestackFunctionality.h"

#include "BeeStackInterface.h"

#if gBeeStackIncluded_d
#include "BeeStackInit.h"
#include "BeeStackUtil.h"
#include "ZdoApsInterface.h"
#include "BeeStackConfiguration.h"
#include "ZdoMain.h"
#include "BeeApp.h"

#endif /* gBeeStackIncluded_d */

#include "MsgSystem.h"
#include "Uart_Interface.h"
#include "IIC_Interface.h"
#include "SPI_Interface.h"

#ifndef gHostApp_d
  #include "AppAspInterface.h"
#endif

#include "NwkMacInterface.h"
#include "ZtcInterface.h"
#include "NVM_Interface.h"
#include "BeeAppInit.h"
#include "PublicConst.h"
#include "Led.h"
#ifdef __IAR_SYSTEMS_ICC__
#include "Interrupt.h"
#endif
#include "TS_Interface.h"
#include "BeeApp.h"
#include "EndPointConfig.h"
#include "TMR_Interface.h"
#include "ZdoNwkInterface.h"

/* For the Nlme_Sync_req */
#include "ASL_ZdpInterface.h"
#include "BeeStackInit.h"
#if gLpmIncluded_d 
#include "pwr_interface.h"
#include "pwr_configuration.h"
#endif
#include "nwkcommon.h"
/* For Platform_Init() */
#include "PlatformInit.h"

#ifdef __IAR_SYSTEMS_ICC__
#include "AppMacInterrupt.h"
#endif

#if MC13226Included_d
#include "ZdpManager.h"
#include "EndPointConfig.h"
#include "ZbAppInterface.h"
#endif

#include "BeeStack_Globals.h"

#ifdef gHostApp_d
#include "ZtcHandler.h"
#endif

#ifndef __IAR_SYSTEMS_ICC__
#ifdef gUseBootloader_d
#if gNvHalIncluded_d  && gUseBootloader_d
#include "OtaSupport.h"
#endif
#endif
#endif
/******************************************************************************
*******************************************************************************
* Private macros
*******************************************************************************
******************************************************************************/

#define mMsgTypeForMSGInd                       0xFC

/******************************************************************************
*******************************************************************************
* Public prototypes
*******************************************************************************
******************************************************************************/

void APP_ZDPJoinPermitReq(uint8_t);
void BeeAppTask(event_t events);
void BeeAppInit(void);
void AppResetApplicationQueues(void);
/******************************************************************************
*******************************************************************************
* Private prototypes
*******************************************************************************
******************************************************************************/

void Include_Symbols(void);

/******************************************************************************
*******************************************************************************
* Public memory declarations
*******************************************************************************
******************************************************************************/

anchor_t gAppDataConfirmQueue;
anchor_t gAppDataIndicationQueue;
anchor_t gInterPanAppDataConfirmQueue;
anchor_t gInterPanAppDataIndicationQueue;
tsTaskID_t gAppTaskID;

#if (gLpmIncluded_d || gComboDeviceCapability_d)
uint16_t PollTimeoutBackup; /*Stores orginal pollrate during binding.*/
#endif 	

#if MC13226Included_d
extern  zbSimpleDescriptor_t BroadcastEp;
extern  endPointDesc_t broadcastEndPointDesc;
extern bool_t gIsANwkUpdateCommandRequest;
extern bool_t gIsNwkUpdateCmdIndication;
extern uint8_t gRouteRequestId;
#endif

/******************************************************************************
*******************************************************************************
* Public functions
*******************************************************************************
******************************************************************************/

/*****************************************************************************
* Permit Join Request through ZDO
*****************************************************************************/


/*****************************************************************************
* Callback for Idle timer
*
*****************************************************************************/
#define gIdleTaskNVIntervalEvent_c  ( 1 << 0 )
void IdleTaskNvTimerCallback(tmrTimerID_t timerID) {
  (void) timerID;
  TS_SendEvent(gIdleTaskID, gIdleTaskNVIntervalEvent_c);
}

/*****************************************************************************
* IdleTask
*
* Called by the kernel when no other task has an event to process.
*
*****************************************************************************/
void IdleTask(event_t events)
{
#if gNvStorageIncluded_d
  static tmrTimerID_t gIdleTaskNVTimerID = gTmrInvalidTimerID_c;
  uint8_t timerJitter;
#endif

#if gTsDebug_d
  static uint32_t TsIdleTaskCounter = 0;
  ++TsIdleTaskCounter;
#endif

  (void)events; /* remove compiler warning */

  /* There are some UART errors that are hard to clear in the UART */
  /* ISRs, and the UART driver does not have a task to clear them */
  /* in non-interrupt context. */
  Uart_ClearErrors();

#ifndef __IAR_SYSTEMS_ICC__
#ifdef gUseBootloader_d
#if gNvHalIncluded_d  &&  gUseBootloader_d
  OTA_WriteNewImageFlashFlags();
#endif
#endif
#endif

#if gNvStorageIncluded_d
  /* The idle task does not have a task init function. */
  if ( gIdleTaskNVTimerID == gTmrInvalidTimerID_c ) {
    gIdleTaskNVTimerID = TMR_AllocateTimer();
  }

  /* Process NV Storage save-on-idle and save-on-count requests. */
  NvIdle();

  /* Process NV Storage save-on-interval requests.
   * If the timer has triggered, call NvTimerTick(TRUE); otherwise call
   * NvTimerTick(FALSE). If the argument is true, NvTimerTick() will
   * decrement the data sets' timer tick counters.
   * Regardless of the argument it receives, NvTimerTick() will return TRUE
   * if any of the data sets' timer counters have not yet counted down to
   * zero. In that case, the timer should be running.
   * If NvTimerTick() returns FALSE, all of the data sets' tick counters
   * have reached zero, and the timer can be turned off.
   * Use a single shot timer, instead of an interval timer, and add some
   * random jitter to the interval, to avoid resonance with other timed
   * events (e.g. ZED polling its parent).
   */
   

  if (NvTimerTick(!!(events & gIdleTaskNVIntervalEvent_c))) {
    if (!TMR_IsTimerActive(gIdleTaskNVTimerID)) {
      timerJitter = GetRandomRange(0, 255);
      TMR_StartSingleShotTimer(gIdleTaskNVTimerID,
                               TmrSeconds(1) + timerJitter - 128,
                               IdleTaskNvTimerCallback);
    }
  }

  /* If NV Storage wrote to flash, it will have called NvOperationStart() */
  /* one or more times. NvOperationEnd() will undo whatever the start call */
  /* did, and is harmless if NvOperationStart() was not called. */
  NvOperationEnd();
#endif

#if (gLpmIncluded_d || gComboDeviceCapability_d)
#if gComboDeviceCapability_d
  if(gLpmIncluded)
#endif
  {
#ifndef gHostApp_d  
#if cPWR_UsePowerDownMode    
    /* Low power management */
    if (gBeeStackConfig.lpmStatus && (PWR_CheckIfDeviceCanGoToSleep()))
    {
      (void)PWR_EnterLowPower();
    }
#endif    
#endif
  }
#endif
}                                       /* IdleTask() */

/******************************************************************************
*******************************************************************************
* Private functions
*******************************************************************************
******************************************************************************/
#if MC13226Included_d
void BeeStackVarsInit(void)
{
    const zbCommissioningAttributes_t gSAS_Rom_init =
    {
      /*** Startup Attribute Set (064699r12, section 6.2.2.1) ***/
      {mDefaultNwkShortAddress_c},        /* x shortAddress (default 0xff,0xff) */
      {mDefaultNwkExtendedPANID_c},       /* x nwkExtendedPANId */
      {mDefultApsUseExtendedPANID_c},     /* x apsUseExtendedPANId */
      {mDefaultValueOfPanId_c},           /* x panId */
      { (uint8_t)(mDefaultValueOfChannel_c & 0xff),
        (uint8_t)((mDefaultValueOfChannel_c>>8) & 0xff),
        (uint8_t)((mDefaultValueOfChannel_c>>16) & 0xff),
        (uint8_t)((mDefaultValueOfChannel_c>>24) &0xff)
      },                                          /* x channelMask */
      {mNwkProtocolVersion_c},                    /* x protocolVersion, always 0x02=ZigBee 2006, 2007 */
      {gAppStackProfile_c},                       /* x stackProfile 0x01 or 0x02 */
      {gStartupControl_Associate_c},              /* startupControl */
      {mDefaultValueOfTrustCenterLongAddress_c},  /* x trustCenterAddress */
      {mDefaultValueOfTrustCenterMasterKey_c},    /* trustCenterMasterKey */
      {mDefaultValueOfNetworkKey_c},              /* x networkKey */
      {gApsUseInsecureJoinDefault_c},             /* x useInsecureJoin */
      {mDefaultValueOfTrustCenterLinkKey_c},      /* preconfiguredLinkKey (w/ trust center) */
      {mDefaultValueOfNwkActiveKeySeqNumber_c},   /* x networkKeySeqNum */
      {mDefaultValueOfNwkKeyType_c},              /* x networkKeyType */
      {gNwkManagerShortAddr_c},                   /* x networkManagerAddress, little endian */

      /*** Join Parameters Attribute Set (064699r12, section 6.2.2.2) ***/
      {mDefaultValueOfNwkScanAttempts_c}, /* x # of scan attempts */
      { (mDefaultValueOfNwkTimeBwnScans_c & 0xff),
        (mDefaultValueOfNwkTimeBwnScans_c >> 8)
      },                                  /* x time between scans(ms) */
      {(mDefaultValueOfRejoinInterval_c&0xff),
       (mDefaultValueOfRejoinInterval_c>>8)
      },                                  /* x rejoin interval (sec) */
      {(mDefaultValueOfMaxRejoinInterval_c & 0xff),
       (mDefaultValueOfMaxRejoinInterval_c >> 8)
      },                                  /* x maxRejoinInterval (sec) */

      /*** End-Device Parameters Attribute Set (064699r12, section 6.2.2.3) ***/
      {(mDefaultValueOfIndirectPollRate_c & 0xff),
       (mDefaultValueOfIndirectPollRate_c >> 8)
      },                                  /* x indirectPollRate(ms) */
      {gMaxNwkLinkRetryThreshold_c},      /* x parentRetryThreshold */

      /*** Concentrator Parameters Attribute Set (064699r12, section 6.2.2.4) ***/
      {gConcentratorFlag_d},              /* x concentratorFlag */
      {gConcentratorRadius_c},            /* x concentratorRadius */
      {gConcentratorDiscoveryTime_c},     /* x concentratorDiscoveryTime */
    };

    beeStackConfigParams_t  gBeeStackConfig_init =
    {
      mDefaultValueOfBeaconOrder_c,                             // uint8_t beaconOrder;
      mDefaultValueOfBatteryLifeExtension_c,                    // bool_t batteryLifeExtn;
      mDefaultValueOfApsFlagsAndFreqBand_c,                     // uint8_t frequency band used by the radio
      mDefaultValueOfManfCodeFlags_c,                           // uint8_t array[2] default Value of Manufacturer Code Flag
      (gMaxRxTxDataLength_c - gAsduOffset_c),                   // uint8_t default value of maximum buffer size
      mDefaultValueOfMaxTransferSize_c,                         // uint8_t array[2] default Value od Maximum Transfer Size
      gServerMask_c,                                            // uint8_t array[2] the services server mask
      mDefaultValueOfMaxTransferSize_c,                         // uint8_t array[2] default Value of Maximum Outgoing Transfer Size
      dummyDescCapField,                                        // uint8_t descriptor capability field
      mDefaultValueOfCurrModeAndAvailSources_c,                 // uint8_t currModeAndAvailSources;
      mDefaultValueOfCurrPowerSourceAndLevel_c,                 // uint8_t currPowerSourceAndLevel;
      mDefaultValueOfNwkDiscoveryAttempts_c,                    // uint8_t gNwkDiscoveryAttempts;
      mDefaultValueOfNwkFormationAttempts_c,                    // uint8_t gNwkFormationAttempts;
      mDefaultValueOfNwkOrphanScanAttempts_c,                   // uint8_t gNwkOrphanScanAttempts;
      mDefaultValueOfDiscoveryAttemptsTimeOut_c,                // uint32_t gDiscoveryAttemptsTimeOut;
      mDefaultValueOfFormationAttemptsTimeOut_c,                // uint32_t gFormationAttemptsTimeOut;
      mDefaultValueOfOrphanScanAttemptsTimeOut_c,               // uint32_t gOrphanScanAttemptsTimeOut;
    //#if( gEndDevCapability_d || gComboDeviceCapability_d)
      mDefaultValueOfAuthenticationPollTimeOut_c,               // uint16_t gAuthenticationPollTimeOut;
      mDefaultValueOfConfirmationPollTimeOut_c,                 // uint16_t gConfirmationPollTimeOut;
    //#endif
      mDefaultValueOfAuthTimeOutDuration_c,                     // uint16_t gAuthenticationTimeOut;
      FALSE,//mDefaultValueOfNwkKeyPreconfigured_c,                     // bool_t gNwkKeyPreconfigured;
      mDefaultValueOfNwkSecurityLevel_c,                        // uint8_t gNwkSecurityLevel;
      mDefaultValueOfNwkSecureAllFrames_c,                      // bool_t gNwkSecureAllFrames;
      0xFF,                      //  zbKeyType_t
      mDefaultValueOfLpmStatus_c                                // bool_t lpmStatus;
    };

    const endPointDesc_t endPoint0Desc_init = {
      (zbSimpleDescriptor_t *)&gZdpEp,  /* Simple descriptor for ZDP. */
      Zdo_MsgDataCallBack,              /* Indication call back for this ep */
      Zdo_CnfCallBack,                  /* Confirm call back for this ep */
      0x00                              /* No fragmentation supported for this ep. */
    };

    const endPointDesc_t broadcastEndPointDesc_init =
    {
      (zbSimpleDescriptor_t *)&BroadcastEp,   /* simple descriptor */
      AppBroadcastMsgCallBack,                /* broadcast MSG callback */
      NULL,                                   /* no broadcast confirm callback */
      0x00                                    /* Values 1 - 8, 0 if fragmentation is not supported. */
    };

  FLib_MemCpy((void*)&gSAS_Rom, (void*)&gSAS_Rom_init, sizeof(gSAS_Rom_init));
  FLib_MemCpy((void*)&gBeeStackConfig, (void*)&gBeeStackConfig_init, sizeof(gBeeStackConfig_init));
  FLib_MemCpy((void*)&endPoint0Desc, (void*)&endPoint0Desc_init, sizeof(endPoint0Desc_init));
  FLib_MemCpy((void*)&broadcastEndPointDesc, (void*)&broadcastEndPointDesc_init, sizeof(broadcastEndPointDesc_init));

  gNibNetworkBroadcastDeliveryTime = gNwkInfobaseBroadcastDeliveryTime_c;
  gAps64BitAddressResolutionTimeout = gAps64BitAddressResolutionTimeout_c;
  gNwkPurgeTimeout = gNwkTransPersistenceTime_c * gSuperFrameSpecLsbBO_c * 1.024;
  gApsFragmentationPollRate = gApsFragmentationPollTimeOut_c;
  gApsAckWaitDuration = gApsAckWaitDuration_c;
  gDefaultRadiusCounter = 2 * gNwkMaximumDepth_c;
  gNwkProtocolId = gDefaultValueOfNwkProtocolId_c;
  gNibMaxDepth=gNwkMaximumDepth_c;
  gNibMaxRouter=gNwkMaximumRouters_c;
  gNibSymLink = mNwkSymmetryLink_c;
  gNibUniqueAddr = mNwkUniqueAddr_c;
  gNibNwkUseMulticast = mUseMultiCast_c;
#if gNwkMulticastCapability_d  
  gNwkUseMulticastForApsGroupDataRequest = mDefaultUseMultiCastForApsGroupDataRequest_c;
#endif  
  gNibMaxBroadcastRetries = gNwkInfobaseMaxBroadcastRetries_c;
#if gNwkMulticastCapability_d    
  gMulticastMaxNonMemberRadius_c = 0x07;
#endif  
#if gApsLinkKeySecurity_d
/* default to APS layer security. */
  gTxOptionsDefault = (gApsTxOptionSecEnabled_c);
#else
/* default to no APS layer security (both ZigBee and ZigBee Pro) */
  
#if gStandardSecurity_d || gHighSecurity_d
  gTxOptionsDefault = gApsTxOptionNone_c;
#endif  
#endif
  gMaxNeighborTableEntries = gNwkInfobaseMaxNeighborTableEntry_c;
  gMaxBroadcastTransactionTableEntries = gMaxBroadcastTransactionTableEntries_c;
  gActAsZed = FALSE;
  gBroadcastDeliveryNumTicks = ( gNwkInfobaseBroadcastDeliveryTime_c / 250 /* = gBroadcastDeliveryInterval_c */) ;
  gSKKEStateMachineSize = gDefaulEntriesInSKKEStateMachine_c;
  giApsDeviceKeyPairCount = gApsMaxSecureMaterialEntries_c;
  gApsKeySetCount = gApsMaxLinkKeys_c;
#if gStandardSecurity_d || gHighSecurity_d  
  gTrustCenter = gTrustCenter_d;
#endif  
  gZdoState = gZdoInitialState_c;
#if gSKKESupported_d
  gSkkeCommand = sizeof(zbApsmeSKKECommand_t);
#else
  gSkkeCommand = 0;
#endif

  gIncomingFrameCounterSetLimit = gNwkInfobaseMaxNeighborTableEntry_c;
  gBroadcastJitterInterval_c = 10;
  gBroadcastDeliveryInterval_c = 250;
  gApsMaxBindingEntries = gMaximumApsBindingTableEntries_c;
  gMaxApsTxTableEntries = gApsMaxDataHandlingCapacity_c;
  //gApsChannelTimer
  //gGlobalAddressMapCounter
  gApsMaxAddressMapEntries = gApsMaxAddrMapEntries_c;
  gEpMaskSize = gEndPointsMaskSizeInBytes_c;
  gApsMaxGroups = gApsMaxGroups_c;
  gApsMaxRetries = gApsMaxRetries_c;
  gfFragmentationEnabled = gFragmentationCapability_d;
  gApsMaxClusterList = gMaximumApsBindingTableClusters_c;
  gNoOfEndPoints = (gNumberOfEndPoints_c + 2);
  giApsDuplicationRejectionTableSize = gApscMinDuplicationRejectionTableSize_c;
  gIsANwkUpdateCommandRequest = 0;
  gIsNwkUpdateCmdIndication = 0;
  //gNewPanIdInUpdateInformation
  //gTmpSequenceNumber - No region ?
  //gTmpSrcAddr
  iHtteIndexActualEntry = 0xFF;
  //mSecurityRequired = 0
  //gNwkMaxEndDevice
  gRouteRequestId = 1;
}


#endif  /* MC13226Included_d */

#ifdef  __IAR_SYSTEMS_ICC__

void Main(void)  {
      
#if MC13226Included_d
  BeeStackVarsInit();
#endif
  /* First init the Interrupt module */
  InterruptInit();
  /* Interrupts should already be off. This is probably redundant. */
  IntDisableAll();

  /* Set priority and handler for Crm, timer, and maca*/
  /* Init the radio and set the channel */

  Platform_Init();
  TS_Init();
  TMR_Init();
  
  Uart_ModuleInit();  
  
  IIC_ModuleInit();
  IntEnableAll();


#ifndef gHostApp_d    
  /* initialize MAC after stack */
#if MC13226Included_d  
  gMacTaskID_c = TS_CreateTask(gTsMlmeTaskPriority_c, Mlme_Main);
  Init_802_15_4(TRUE);
  BeeStackInit(); 
#else
  BeeStackInit(); 
  Init_802_15_4(TRUE);
#endif

#else
  
  BeeStackInit();
  pfMM_UpdateMsgTracking = (bool_t (*)(const void *pBlock, const bool_t Alloc))MM_UpdateMsgTracking;
  pfMM_InitMsgTracking = (void (*)(void))MM_InitMsgTracking;
  pfMM_AddMsgToTrackingArray = (void (*)(uint8_t Index, uint32_t Addr))MM_AddMsgToTrackingArray;
  
  maMacHeapROMCopy = &maMacHeap[0];
  poolInfoROMCopy = &poolInfo[0];
  maMmPoolsROMCopy = &maMmPools[0];
  pLastPoolROMCopy = pLastPool;
  pSecondLastPoolROMCopy = pSecondLastPool;
  
  MM_Init((uint8_t*)maMacHeap, poolInfo, maMmPools);
#endif  

    /* initialize all tasks */
  LED_Init();

  /*initialize the application*/
  
  gAppTaskID = TS_CreateTask(gTsAppTaskPriority_c, BeeAppTask);
#ifdef gHostApp_d
  ZtcCPUResetBlackBox();
#else
  BeeAppInit();  
#endif  

  /*All LED's are switched OFF*/
  Led1Off();
  Led2Off();
  Led3Off();
  Led4Off();
  
  Led1On();
  
#else /*__IAR_SYSTEMS_ICC__*/

#ifdef gHostApp_d
extern uint8_t maMacHeap[];
extern pools_t maMmPools[];
extern const poolInfo_t poolInfo[];
#endif
  void main(void) {
    
  /* Interrupts should already be off. This is probably redundant. */
  IrqControlLib_DisableAllIrqs();

  /* Init platform */
  Platform_Init();   
#ifndef gHostApp_d 
  Init_802_15_4();
#else
  MM_Init((uint8_t*)maMacHeap, poolInfo, maMmPools);
  Platform_Init();    // Init platform
#endif  
 
#if gBeeStackIncluded_d
  TS_Init();                            /* Init the kernel. */
  Uart_ModuleInit();
  IIC_ModuleInit();
  SPI_Init();
  IrqControlLib_EnableAllIrqs();
  BeeStackInit();
#else
  TMR_Init();
  IrqControlLib_EnableAllIrqs();
#endif

  gAppTaskID = TS_CreateTask(gTsAppTaskPriority_c, BeeAppTask);
 
#ifdef gHostApp_d
  ZtcCPUResetBlackBox();
#else
  BeeAppInit();  
#endif  


  
#endif/*__IAR_SYSTEMS_ICC__*/
  
  
  /* Start the task scheduler. Does not return. */
  TS_Scheduler();

}

/****************************************************************************/

#if !gBeeStackIncluded_d
uint8_t MLME_NWK_SapHandler
  (
  nwkMessage_t * pMsg	 /* Pointer for Mngmt functions from MAC to NWK */
  )
{
  ZTC_TaskEventMonitor(gMLMENwkSAPHandlerId_c, (uint8_t *) pMsg, gZbSuccess_c);
  MSG_Free( pMsg );
  return gZbSuccess_c;
}

/****************************************************************************/
uint8_t MCPS_NWK_SapHandler
  (
  mcpsToNwkMessage_t *pMsg  /* Pointer for Data from MAC to NWK */
  )
{
  ZTC_TaskEventMonitor(gMCPSNwkSAPHandlerId_c, (uint8_t *) pMsg, gZbSuccess_c);
  MSG_Free( pMsg );
  return gZbSuccess_c;
}


/****************************************************************************/
uint8_t ASP_APP_SapHandler
  (
  aspToAppMsg_t *pMsg		/*Pointer from ASP to APP */
  )
{
  ZTC_TaskEventMonitor(gASPNwkSAPHandlerId_c, (uint8_t *) pMsg, gZbSuccess_c);
  MSG_Free( pMsg );
  return gZbSuccess_c;
}

#endif /*  ( gBeeStackIncluded_d == 0 ) */


/***************************************************************************/
/* NOTE: NEED ALOT OF COMMENTS */
uint8_t InterPan_APP_SapHandler
(
  InterPanMessage_t *pMsg /*pointer from Intra Pan to APP*/
)
{

  zbInterPanDataIndication_t *pDataInd;
  uint8_t index;
  zbSimpleDescriptor_t *pSimpleDescriptor;
  uint8_t fMatch = FALSE;
  
  /* For Host application , all received SAP messages are forward 
     to Host(if Host uart communication is enabled) by ZTC */  
#ifndef gHostApp_d
  ZTC_TaskEventMonitor(gInterPanApp_SAPHandlerId_c, (uint8_t *) pMsg, gZbSuccess_c);
#endif
  if(pMsg->msgType == gInterPanDataCnf_c)
  {
    MSG_Queue( &gInterPanAppDataConfirmQueue, pMsg );
    TS_SendEvent(gAppTaskID, gInterPanAppEvtDataConfirm_c);
  }
  else if(pMsg->msgType == gInterPanDataInd_c)
  {
    /* filter the profile */
    pDataInd =(zbInterPanDataIndication_t *)(&pMsg->msgData.InterPandataIndication);
/* look for matching endpoint
            iIndex starts as 2 in order to ignore broadcast and ZDP endpoint. */    
    for(index = 2; index < gNoOfEndPoints ; ++index) 
    {
      /* is this a registered endpoint? */
      if(gaEndPointDesc[index].pDescription) 
      {
        /* does profile ID match? */
        pSimpleDescriptor = (zbSimpleDescriptor_t *)gaEndPointDesc[index].pDescription->pSimpleDesc;
        if(IsEqual2Bytes(pSimpleDescriptor->aAppProfId, pDataInd->aProfileId))
        {
          fMatch = TRUE;
          break;
        }
      }
    } /* end for(...) */
    if(fMatch){
      MSG_Queue( &gInterPanAppDataIndicationQueue, pMsg );
      TS_SendEvent(gAppTaskID, gInterPanAppEvtDataIndication_c);
    }
    else{
      MSG_Free(pMsg);
    }
  }

  return gZbSuccess_c;
}
/***************************************************************************/

/*
  AppMsgCallBack

  Received a message.
*/
void AppMsgCallBack(apsdeToAfMessage_t *pMsg)
{
  MSG_Queue( &gAppDataIndicationQueue, pMsg );
  TS_SendEvent(gAppTaskID, gAppEvtDataIndication_c);
}

/***************************************************************************/

/*
  AppCnfCallBack

  Received a confirm.
*/
void AppCnfCallBack(apsdeToAfMessage_t *pMsg)
{
  MSG_Queue( &gAppDataConfirmQueue, pMsg );
  TS_SendEvent(gAppTaskID, gAppEvtDataConfirm_c);
}

/*
  DeQueue (but don't free) the application message queues.
  
*/
void AppResetApplicationQueues(void)
{
  List_ClearAnchor( &gAppDataIndicationQueue );

  List_ClearAnchor( &gAppDataConfirmQueue );

  /* Also Cleat all the events to the application. */
  TS_ClearEvent(gAppTaskID, 0xFFFF);
}
/*****************************************************************************
  AF_GetEndPointDevice

  Based on an endpoint number, get the pointer to the device data. Device data
  is unique per endpoint, and only for application endpoints (not ZDO or
  Broadcast).

  Returns pointer to endpoint device or NULL if doesn't exist.
*****************************************************************************/
afDeviceDef_t *AF_GetEndPointDevice
  (
  zbEndPoint_t endPoint /* IN: endpoint # (1-240) */
  )
{
#if gNum_EndPoints_c != 0
  index_t i;
  zbSimpleDescriptor_t *pSimpleDesc;

  if(!endPoint)
    return NULL;

  for(i=0; i<gNoOfEndPoints; ++i) {
    if(endPointList[i].pEndpointDesc) {
      pSimpleDesc = endPointList[i].pEndpointDesc->pSimpleDesc;
      if(pSimpleDesc->endPoint == endPoint)
        return (afDeviceDef_t *)endPointList[i].pDevice;
    }
  }
  return NULL;
#else
  (void) endPoint;
  return NULL;
#endif  
}

/*****************************************************************************
* AF_DeviceDefToEndPoint
*
* Look through endpoint descriptors for this device definition. 
* Returns endpoint #.
*****************************************************************************/
zbEndPoint_t AF_DeviceDefToEndPoint
  (
  afDeviceDef_t *pDeviceDef
  )
{
#if gNum_EndPoints_c != 0  
  index_t i;

  for(i=0; i<gNum_EndPoints_c; ++i) {
    if(endPointList[i].pDevice == pDeviceDef)
      return endPointList[i].pEndpointDesc->pSimpleDesc->endPoint;
  }

  return gInvalidAppEndPoint_c; /* not found */
#else
  (void) pDeviceDef;
  return gInvalidAppEndPoint_c;
#endif  
  
}

/*****************************************************************************
* Get the simple descriptor for this index #
*****************************************************************************/
static zbSimpleDescriptor_t *AF_GetSimpleDescriptor
  (
  uint8_t index
  )
{  
  return (gaEndPointDesc[index].pDescription ? gaEndPointDesc[index].pDescription->pSimpleDesc : NULL);
}

/*****************************************************************************
* TS_AppBroadcastMsgCallBack
*
* Common routine called whenever a message on the broadcast endpoint (0xff)
* is received.
*
*****************************************************************************/
void AppBroadcastMsgCallBack
  (
  apsdeToAfMessage_t *pMsg  /* IN: broadcast message coming in */
  )
{
  zbSimpleDescriptor_t *pSimpleDescriptor;
  zbApsdeDataIndication_t *pIndication;
  zbEndPoint_t endPoint;
	index_t iIndex;
  apsdeToAfMessage_t *pPrevEpMsg;
  uint8_t prevEndPoint=gZbBroadcastEndPoint_c;

  /* get indication */
  pIndication = &(pMsg->msgData.dataIndication);

	/* look for matching endpoint
            iIndex starts as 2 in order to ignore broadcast and ZDP endpoint. */
	for(iIndex = 2; iIndex < gNoOfEndPoints; ++iIndex) {

	  /* for now, just try the first application endpoint */
  	pSimpleDescriptor = AF_GetSimpleDescriptor(iIndex);
  	if(!pSimpleDescriptor)
			continue;
	  endPoint = pSimpleDescriptor->endPoint;

	  /* profile filter */
  	if(!IsEqual2Bytes(pIndication->aProfileId, pSimpleDescriptor->aAppProfId))
			continue;

	  /* group filter  */
  	if(pIndication->dstAddrMode == gZbAddrModeGroup_c) {
    	if(!ApsGroupIsMemberOfEndpoint(pIndication->aDstAddr,endPoint))
				continue;
			}

    if(prevEndPoint  !=  gZbBroadcastEndPoint_c)
    {
      /* copy all message in order to queue it */
      pPrevEpMsg = AF_MsgAlloc();
      if (pPrevEpMsg)
      {
        FLib_MemCpy(pPrevEpMsg, pMsg, gMaxRxTxDataLength_c);
        pPrevEpMsg->msgData.dataIndication.pAsdu = ((uint8_t *)pMsg->msgData.dataIndication.pAsdu - (uint8_t *)pMsg) + (uint8_t *)pPrevEpMsg;
        /* Copy EndPoint number */
        pPrevEpMsg->msgData.dataIndication.dstEndPoint = prevEndPoint;    /* set endpoint to found application endpoint */
        /* tell ZTC about the message */
#ifndef gHostApp_d        
        ZTC_TaskEventMonitor(gAFDEAppSAPHandlerId_c, (uint8_t *) pPrevEpMsg, mMsgTypeForMSGInd);
#else
        ZTC_TaskEventMonitor(gpHostAppUart, gAFDEAppSAPHandlerId_c, (uint8_t *) pPrevEpMsg, mMsgTypeForMSGInd);
#endif 
        /* pass it on to the app */
        MSG_Queue( &gAppDataIndicationQueue, pPrevEpMsg);
        TS_SendEvent(gAppTaskID, gAppEvtDataIndication_c);
      }
    }
    /* found a new end point save its number to send it later */
    prevEndPoint = endPoint;
	} /* end of for loop */

	/* no endpoints matched, throw out msg */
	if(prevEndPoint == gZbBroadcastEndPoint_c)
  {
    MSG_Free(pMsg);
		return;
	}

  /* Copy EndPoint Number*/
  pIndication->dstEndPoint = prevEndPoint;    /* set endpoint to found application endpoint */
  /* tell ZTC about the message */
#ifndef gHostApp_d   
  ZTC_TaskEventMonitor(gAFDEAppSAPHandlerId_c, (uint8_t *) pMsg, mMsgTypeForMSGInd);
#else
  ZTC_TaskEventMonitor(gpHostAppUart, gAFDEAppSAPHandlerId_c, (uint8_t *) pMsg, mMsgTypeForMSGInd);
#endif
  /* pass it on to the app */
  MSG_Queue( &gAppDataIndicationQueue, pMsg );
  TS_SendEvent(gAppTaskID, gAppEvtDataIndication_c);
}

/*****************************************************************************
* AppStartPolling
* 
* This fucntion change the Poll rate to 1 sec and start the interval timer to 
* make polling every 1sec until it is stop calling function AppStopPolling.
*
******************************************************************************/
void AppStartPolling
  (
  void
  )
{  
#if (gLpmIncluded_d || gComboDeviceCapability_d)
#if gComboDeviceCapability_d
  if(gLpmIncluded)
#endif
  {
 	  uint16_t BindingPollTimeout = 1000; /* 1sec */
 	  /*Only set the polltimeout if lowpower is enabled*/
    if (gBeeStackConfig.lpmStatus == 1) {      
      PollTimeoutBackup = NlmeGetRequest(gNwkIndirectPollRate_c);
      NlmeSetRequest(gNwkIndirectPollRate_c, BindingPollTimeout);
#ifndef gHostApp_d    
 	    NWK_MNGTSync_ChangePollRate(BindingPollTimeout); 
#endif    
    }
  }
#endif
}
  
/*****************************************************************************
* AppStopPolling
* 
* This fucntion change the Poll rate to 0 so it just make polling when need it.
*
******************************************************************************/
void AppStopPolling
  (
  void
  )
{
#if (gLpmIncluded_d || gComboDeviceCapability_d)
#if gComboDeviceCapability_d
  if(gLpmIncluded)
#endif
  {
    /*Only restore orginal poll timeout if low power is enabled*/
    if (gBeeStackConfig.lpmStatus == 1) {      
     NlmeSetRequest(gNwkIndirectPollRate_c, PollTimeoutBackup);
#ifndef gHostApp_d    
     NWK_MNGTSync_ChangePollRate(NlmeGetRequest(gNwkIndirectPollRate_c));
#endif     
    }
  }
#endif
}
/******************************************************************************
*******************************************************************************
* Debugging and Unit Tests
*******************************************************************************
******************************************************************************/

#ifndef gMsgDebug_c
#define gMsgDebug_c 0
#endif 

#if gMsgDebug_c
#include "msgsystem.h"
extern pools_t maMmPools[];

uint32_t gMM_AllocCount;  /* # of times MSG_Alloc() is called */
void *gaMM_Msgs[FLib_GetMax(gTotalBigMsgs_d, gTotalSmallMsgs_d)];

/******************************************************************************
* MM_DebugFree
*
* Similar to free, but fills the memory with a free value and will call
* MM_DebugAssert() if memory is freed twice.
******************************************************************************/
void MM_DebugFree
  (
  void *msg   /* IN: previously allocated by MM_DebugAlloc() */
  )
{
  /* first check if already freed */
  if(BeeUtilArrayIsFilledWith(msg, gMemFreeValue_c, gMmBlockSize0_c))
    MM_DebugAssert();

  /* fill the buffer with known free value */
  FLib_MemSet(msg, gMemFreeValue_c, gMmBlockSize0_c);
  MM_Free(msg);
}

/******************************************************************************
* MM_DebugAlloc
*
* Allocate memory. Fills memory with known alloc value and puts in alloc
* count
******************************************************************************/
void *MM_DebugAlloc
  (
  uint8_t numBytes  /* IN */
  )
{
  uint8_t *pMsg;
  uint8_t size;

  /* increment allocation count */
  ++gMM_AllocCount;

  /* allocate the memory */
  pMsg = MM_Alloc(numBytes);
  if(!pMsg)
    MM_DebugAssert();
  else {
    /* fill memory with alloc val */
    size = gMmBlockSize0_c;
    if(numBytes > gMmBlockSize0_c)
      size = gMmBlockSize1_c;
    FLib_MemSet(pMsg, gMemAllocValue_c, size);

    /* add counter to end */
    *(uint32_t *)(pMsg[size-sizeof(uint32_t)]) = gMM_AllocCount;
  }

  return (void *)pMsg;
}

/******************************************************************************
* MM_DebugNumFreeBigBuffers
*
******************************************************************************/
uint8_t MM_DebugNumFreeBigBuffers
  (
  void
  )
{
  listHeader_t *pHead;
  uint8_t count = 0;
#ifdef __IAR_SYSTEMS_ICC__  
  AppInterrupts_PushIrqStatus();  
#endif
  pHead = maMmPools[1].anchor.pHead;
  while(pHead != NULL)   
  {
    pHead = pHead->pNext;
    count++;
  }
#ifdef __IAR_SYSTEMS_ICC__  
  AppInterrupts_PullIrqStatus();   
#endif  
 return count;
}

/******************************************************************************
* MM_DebugNumFreeSmallBuffers
*
******************************************************************************/
uint8_t MM_DebugNumFreeSmallBuffers
  (
  void
  )
{
  listHeader_t *pHead;
  uint8_t count = 0;
#ifdef __IAR_SYSTEMS_ICC__
  AppInterrupts_PushIrqStatus();  
#endif  
  pHead = maMmPools[0].anchor.pHead;
  while(pHead != NULL)    
  {
    pHead = pHead->pNext;
    count++;
  }
#ifdef __IAR_SYSTEMS_ICC__  
  AppInterrupts_PullIrqStatus();  
#endif  
  return count;
}

/******************************************************************************
* MM_DebugAssert
*
* Either couldn't allocate a buffer or freed a buffer twice. Place a breakpoint
* in the debugger here to help solve memory leaks, etc...
******************************************************************************/
void MM_DebugAssert
  (
  void
  )
{
  /* insert debugging code here... */
  TurnOnLeds();
}
#endif
/*****************************************************************************/

/* Called each time deep sleep mode is exited. */
void DeepSleepWakeupStackProc
  (
  void
  )
{
	
  TS_SendEvent(gAppTaskID, gAppEvtSyncReq_c);	
  return;
}

/******************************************************************************
* BeeAppAssert
*
* Something failed in the stack or app that can't recover. Stop here. Mostly
* used during debug phase of product. Could be used
******************************************************************************/
void BeeAppAssert
  (
  assertCode_t code /* IN: why the assert failed */
  )
{
  volatile bool_t waitForever = code; /* just set to anything non-zero */
  
  while (waitForever)
    { }
}

