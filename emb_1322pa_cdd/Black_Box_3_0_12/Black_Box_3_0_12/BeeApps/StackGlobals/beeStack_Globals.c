/******************************************************************************
* This file contains the declarations for different tables used in BeeStack.
*
* Copyright (c) 2008, Freescale, Inc.  All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from  Freescale Semiconductor.
*
******************************************************************************/
#ifndef __IAR_SYSTEMS_ICC__
#ifdef MC13226Included_d
    #undef MC13226Included_d
#endif    
    #define MC13226Included_d 0
#endif
#include "EmbeddedTypes.h"
#include "zigbee.h"
#include "BeeStackConfiguration.h"
#include "Beestack_Globals.h"
#include "ZdoNwkInterface.h"
#include "FunctionLib.h"
#include "NV_Data.h"
#include "AppZdoInterface.h"
#include "BeeStackInterface.h"
#include "BeeStackParameters.h"
#include "TS_Interface.h"
#include "nwkcommon.h"
#include "ApsDataInterface.h"
#include "ZdoApsInterface.h"


/******************************************************************************
*******************************************************************************
* Private Macros
*******************************************************************************
******************************************************************************/

/******************************************************************************
*******************************************************************************
* Private Prototypes
*******************************************************************************
******************************************************************************/

/******************************************************************************
*******************************************************************************
* Private type Definations
*******************************************************************************
******************************************************************************/

/******************************************************************************
*******************************************************************************
* Public memory declarations
*******************************************************************************
******************************************************************************/
#ifdef gHostApp_d
uint8_t        aExtendedAddress[8] = { 0xff, 0xff,0xff,0xff, 0xff, 0xff,0xff,0xff };
/* Denotes the current Software Version */
const uint8_t gaSoftwareVersion[ 2 ] = { 0x03,0x00 };
uint8_t giFragmentedPreAcked;
#endif /* gHostApp_d */

#if MC13226Included_d
/* Commissioning start up Attributes */
extern const zbCommissioningAttributes_t gSAS_Rom;
/* this is the working set in RAM */
extern zbCommissioningAttributes_t gSAS_Ram;
/* points to the commissioning SAS. Points to NULL if no commissioing SAS */
zbCommissioningAttributes_t *gpSAS_Commissioning = NULL;
/* the NVM set is stored in NVM. See gaNvNwkDataSet in NV_Data.c */
extern beeStackConfigParams_t  gBeeStackConfig;

#else   /*  MC13226Included_d */

const zbCommissioningAttributes_t gSAS_Rom =
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

/* this is the working set in RAM */
zbCommissioningAttributes_t gSAS_Ram;

/* points to the commissioning SAS. Points to NULL if no commissioing SAS */
zbCommissioningAttributes_t *gpSAS_Commissioning = NULL;
  
/* the NVM set is stored in NVM. See gaNvNwkDataSet in NV_Data.c */


beeStackConfigParams_t  gBeeStackConfig =
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
#if( gEndDevCapability_d || gComboDeviceCapability_d)
  mDefaultValueOfAuthenticationPollTimeOut_c,               // uint16_t gAuthenticationPollTimeOut;
  mDefaultValueOfConfirmationPollTimeOut_c,                 // uint16_t gConfirmationPollTimeOut;
#endif
  mDefaultValueOfAuthTimeOutDuration_c,                     // uint16_t gAuthenticationTimeOut;
  FALSE,//mDefaultValueOfNwkKeyPreconfigured_c,                     // bool_t gNwkKeyPreconfigured;
  mDefaultValueOfNwkSecurityLevel_c,                        // uint8_t gNwkSecurityLevel;
  mDefaultValueOfNwkSecureAllFrames_c,                      // bool_t gNwkSecureAllFrames;
  0xFF,                      //  zbKeyType_t
  mDefaultValueOfLpmStatus_c                                // bool_t lpmStatus;
};

#endif  /*  MC13226Included_d */


#if gUser_Desc_rsp_d || gUser_Desc_conf_d
zbUserDescriptorPkg_t gUserDesc =
{
  0x09,
  'F','r','e','e','s','c','a','l','e',' ',' ',' ',' ',' ',' ',' '
};
#endif

#if gComplex_Desc_rsp_d
configComplexDescriptor_t gComplexDesc =
{
  mDefaultValueOfComplexDescFieldCount_c ,
  mDefaultValueOfComplexDescLangCharSet_c,
  mDefaultValueOfComplexDescManufactureName_c,
  mDefaultValueOfComplexDescModelName_c,
  mDefaultValueOfComplexDescSerialNumber_c,
  mDefaultValueOfComplexDescDeviceUrl_c,
  mDefaultValueOfComplexDescIcon_c,
  mDefaultValueOfComplexDescIconUrl_c
};
#endif

/***************************** For Inter-Pan *********************************/
const uint8_t gInterPanCommunicationEnabled = gInterPanCommunicationEnabled_c;

#if gInterPanCommunicationEnabled_c
/*NOTE: DO NOT CHANGE THESE POINTERS */
const InterPanFuncInd_t pInterPanProcessIndication  = InterPan_ProcessDataIndication;
const InterPanFuncCnf_t pInterPanProcessConfirm     = InterPan_ProcessConfirm;
#else
const InterPanFuncInd_t pInterPanProcessIndication  = NULL;
const InterPanFuncCnf_t pInterPanProcessConfirm     = NULL;
#endif
/***************************** For Pan Id Condflict **************************/

#if gConflictResolutionEnabled_d && (( gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d) && gNwkPanIdConflict_d)
const PIDConflictDetect_t pPIDDetection           = ActualDetectPanIdConflict;
const PIDConflictResol_t  pPIDResolution          = ActualProcessPanIdConflictDetected;
#else
const PIDConflictDetect_t pPIDDetection           = NULL;
const PIDConflictResol_t  pPIDResolution          = NULL;
#endif


#if MC13226Included_d
/***************************** For NWK Layer *********************************/

/* Which ZigBee device type is this node (only combo devices can change type) */
#if gCoordinatorCapability_d
  #define mZbDeviceType_c gCoordinator_c
#endif

#if gRouterCapability_d
  #define mZbDeviceType_c gRouter_c
#endif

#if gEndDevCapability_d
  #define mZbDeviceType_c gEndDevice_c
#endif

#if gComboDeviceCapability_d
  #define mZbDeviceType_c gInvalidDeviceType_c
#endif


const nvmNwkData_t gDefaultNwkData = {
    { mDefaultValueOfExtendedAddress_c },
    { mDefaultValueOfMacCapFlags_c },
    { mZbDeviceType_c },
    { mDefaultLogicalChannel_c },
    { mDefaultParentShortAddress_c },   // aParentShortAddress
    { mDefaultParentLongAddress_c },    // aParentLongAddress
    { 0x00 },                           // deviceDepth
    { 0x00 },                           // nwkUpdateId;
    { mDefaultValueOfPermitJoinDuration_c } // permitJoining
  };

extern nvmNwkData_t gNwkData;

extern zbCounter_t gNibSequenceNumber;

/* Current state of network state machine */
uint8_t gNwkState;

uint16_t gNibTxTotal=0;

uint16_t gNibTxTotalFailures=0;

const uint16_t gNibPassiveAckTimeout = gNwkInfobasePassiveAckTimeout_c;

const bool_t gNibUseTreeRouting = mNwkUseTreeRouting_c;

uint16_t gNibTransactionPersistenceTime = gNwkTransPersistenceTime_c;

/*PurgeTime is : nwkPersistenceTime * SuperFrame  * 1/aBaseSuperFrameDuration */
extern uint16_t gNwkPurgeTimeout;

extern uint8_t gNibMaxBroadcastRetries ;

const uint8_t gNibReportConstantCost = mNwkRptConstantCost_c;

uint8_t gNibRouteDiscoveryRetriesPermitted = mNwkRouteDiscRetriesPermitted_c;

extern bool_t gNibSymLink ;

const uint8_t gNibAddrAlloc = mNwkAddressAlloc_c;

extern uint8_t gNibUniqueAddr;

zbNwkAddr_t gNibNextAddress = {mDefaultNwkNextAddress_c};

zbNwkAddr_t gNibAvailableAddresses = {mDefaultNwkAvailableAddress_c};

zbNwkAddr_t gNibAddressIncrement={mDefaultNwkAddressIncrement_c};

const uint8_t gScanDuration = gScanDuration_c;

/* Age limit in time for end devices using SP1*/
const uint8_t gNwkAgingTimeForEndDevice = gNwkAgingTimeForEndDevice_c;

/* Age limit in time for router devices using SP1*/
const uint8_t gNwkAgingTimeForRouter = gNwkAgingTimeForRouter_c;

/* Age limit in time for sibling router devices using SP1*/
const uint8_t gNwkAgingTimeForSiblingRouter = gNwkAgingTimeForSiblingRouter_c;

/* Scale used to define the resolution for expiring the entries on the NT */
uint8_t gNwkAgingTickScale = gNwkAgingTickScale_c;

/* Age limit in time to expire route and source route table entries*/
const uint8_t gMinutesToExpireRoute = gMinutesToExpireRoute_c;

bool_t gDeviceAlreadySentRouteRecord = FALSE;
extern bool_t gActAsZed;

/* This global varible is used to change the PID upper Limit */
/*const*/ zbPanId_t gaPIDUpperLimit = {0xFE, 0xFF};

#if gNwkSymLinkCapability_d
/* 053474r17 ZigBee Spec.
   The time in seconds between link status command frames. 
   Default 0x0f */
uint8_t gNibNwkLinkStatusPeriod = gNetworkLinkStatusPeriod_c;

const uint8_t gNwkLinkStatusMultipleFramesPeriod = gNwkLinkStatusMultipleFramesPeriod_c;
#endif

const uint8_t gNibNwkRouterAgeLimit =  gNetworkRouterAgeLimit_c;

#if gComboDeviceCapability_d
bool_t gLpmIncluded = FALSE;
#endif

/* Don't touch this line */
extern bool_t gNibNwkUseMulticast;

bool_t gNwkUseMulticastForApsGroupDataRequest;

extern uint8_t gMulticastMaxNonMemberRadius_c;

/* Energy Threshhold used when performing ED Scan */
const uint8_t  gNwkEnergyLevelThresHold = gNwkEnergyLevelThresHold_d;

/*  Default Transmit Failure Counter, indicates how many times have to fail a 
  transmition to a specific device before a route repair is initiated */
  
/* This variable is used to set the protocol ID uesd in Beacon Payload, This
  can be changed by the user with a proper value.
  Note:Two devices with diffrent protocol Id will never join*/
extern uint8_t gNwkProtocolId;

/* This variable is used for setting the Radius Counter parameter to the
   default value which is 2 times the maximun depth of a network */
extern zbCounter_t gDefaultRadiusCounter;

/* This variable is used to hold the max children, that a ZC/ZR can accept.
  Note: This has to be the same or greater than Nwk Max Routers */
const uint8_t gNibMaxChildren=gNwkMaximumChildren_c;

/* This variable is used to hold the max routers, that a ZC/ZR can accept.
  Note: This has to be the same or greater than Nwk Max Children */
extern uint8_t gNibMaxRouter;

/* This variable is used to hold the max depth at which the device can be present
  Note: The ZR at Last depth cannot Accept children */
extern uint8_t gNibMaxDepth;

/* Time duration in seconds that a broadcast message needs to encompass the
  entire network. Devices using the ZigBee stack profile must set: 
  nwkBroadcastDeliveryTime = 3 secs */
/*const*/ 
extern uint32_t gNibNetworkBroadcastDeliveryTime;

/* This is the variable that contains our neighbors information */
neighborTable_t gaNeighborTable[gNwkInfobaseMaxNeighborTableEntry_c];

/* This variable tells the number of neighbour table entries suppoted by the
  network layer, this can be changed at compile time by assigning a proper
	value to gNwkInfobaseMaxNeighborTableEntry_c macro*/
extern uint8_t gMaxNeighborTableEntries;


/* 
  This variable contains sorted the indexes of the neighbor table in ascending order.
  This array only is used when NwkSymLink is TRUE and is used by Link Status Command 
  to build the link status information list. This array has the same length of the gaNeighborTable 
*/
#if gNwkSymLinkCapability_d && (( gCoordinatorCapability_d || gRouterCapability_d ) || gComboDeviceCapability_d)
uint8_t gaNeighborTableAscendantSorted[gNwkInfobaseMaxNeighborTableEntry_c];
#endif


/* This table will exist only if a discovery network has been performed 
  previously and it last only during joining or forming a network */
neighborTable_t *gpDiscoveryTable = NULL;
extendedNeighborTable_t *gpExtendedDiscoveryTable = NULL;


/* This value will change depending on the size of memory allocated for
  gpDiscoveryTable and gpExtendedDiscoveryTable */
index_t gMaxDiscoveryTableEntries=0;


/* Number Of Outstanding Broadcasts that are supported. More than one
  outstanding broadcast can be supported at the network layer only when
  passive acknowledgement and retry is compiled out */
extern uint8_t gMaxBroadcastTransactionTableEntries;

/* this table keeps the pointer to the messages that are waiting to be routed.
  waiting for a route discovery or device authentication */
packetsOnHoldTable_t gaPacketsOnHoldTable[gPacketsOnHoldTableSize_c];
const uint8_t gPacketsOnHoldTableSize = gPacketsOnHoldTableSize_c;

/* this table keeps track of every unicast data request sent to the NLL */
handleTrackingTable_t gaHandleTrackingTable[gHandleTrackingTableSize_c];
const uint8_t gHandleTrackingTableSize = gHandleTrackingTableSize_c;

/* keeps track of each rejoin reuqest until copleted. */
nwkRejoinEntry_t *gpNwkRejoinStateMachine;
/* The buffer used is as big as a small bufffer. */
const uint8_t gNwkRejoinStateMachineSize = gNwkRejoinStateMachineSize_c;
/* The size of the buffer divided by the amount of entries. */
const uint8_t gNwkRejoinStateMachineAmountOfEntries = gNwkRejoinStateMachineAmountOfEntries_c;
/* The amount of time in millisecodn to exppire the Nwk Rejoin state machine. */
const tmrTimeInMilliseconds_t  gNwkRejoinExpirationTime = gNwkRejoinExpirationTime_c;

/* indicates how many entries of the handleTrackingTable are occupied by sleeping devices */
zbCounter_t gHttIndirectEntries;
  
/* Sets the maximum number of failures to tolarate in the network layer before
  it send a report up. FA module. */
const uint8_t gMaxNumberOfTxAttempts = (gMaxNumberOfTxAttempts_c+1);

const uint8_t gHttMaxIndirectEntries = gHttMaxIndirectEntries_c;

const uint8_t gNwkHighRamConcentrator = gNwkHighRamConcentrator_d;

#if gStandardSecurity_d || gHighSecurity_d
zbIncomingFrameCounterSet_t  gaIncomingFrameCounterSet1[gNwkInfobaseMaxNeighborTableEntry_c];
zbIncomingFrameCounterSet_t  gaIncomingFrameCounterSet2[gNwkInfobaseMaxNeighborTableEntry_c];
extern uint8_t                gIncomingFrameCounterSetLimit ;

/* Set the defualt transmision options for SSP. */
#if gApsLinkKeySecurity_d
/* default to APS layer security. */
/*const*/ extern uint8_t gTxOptionsDefault;
#else
/* default to no APS layer security (both ZigBee and ZigBee Pro) */
extern uint8_t gTxOptionsDefault;
#endif

extern bool_t gTrustCenter;


#endif

#if ( !gNwkBroadcastPassiveAckRetryCapability_d )

extern uint8_t gBroadcastDeliveryInterval_c;
extern uint8_t gBroadcastJitterInterval_c;

  /* This value determines the number of ticks before a broadcast transaction entry expires
  in the BroadcastTransactionTable. Changed to constant gNwkInfobaseBroadcastDeliveryTime_c*/
  extern uint8_t gBroadcastDeliveryNumTicks;

  /* Broadcast Transaction Table, size of this table is decided by the number
  of broadcast requests that handled by the network layer simultaneously */
  broadcastTransactionTable_t gaBroadcastTransactionTable[gMaxBroadcastTransactionTableEntries_c ];

#else

#if gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d
  /* Broadcast Transaction Table, size of this table is decided by the number
      of broadcast requests that handled by the network layer simultaneously */
  broadcastTransactionTable_t gaBroadcastTransactionTable[ 1 ];
#endif

#endif ( !gNwkBroadcastPassiveAckRetryCapability_d )

#if gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d
#if gRnplusCapability_d
  routeTable_t gaRouteTable[gNwkInfobaseMaxRouteTableEntry_c];
#endif
#endif

/*This variable hold the value of Maximum route table Entries supported, this
  can be changed by the user at compile time by updating
  gNwkInfobaseMaxRouteTableEntry_c macro by proper value */
const uint8_t gMaxRouteTableEntries = gNwkInfobaseMaxRouteTableEntry_c;

/* Route Discovery Table Entries, to update the number of route Discovery
  table entries, the value of gNwkRoutingMaxRouteDiscoveyTableEntry_c macro
  needs to be updated */
routeDiscoveryTable_t gaRouteDiscoveryTable[gNwkRoutingMaxRouteDiscoveyTableEntry_c ];

/*This variable hold the value of Number of route Discovery Entries
  supported, this can be changed by the user at compile time by updating
  gNwkRoutingMaxRouteDiscoveyTableEntry_c macro by proper value */
const uint8_t gMaxRouteDiscoveryTableEntries = gNwkRoutingMaxRouteDiscoveyTableEntry_c;

const uint8_t gRouteRequestRetryInterval = 8;

const uint8_t gRouteDiscoveryExpirationInterval = 150;

const uint8_t gRouteDiscoveryExpirationNumTicks = gNibNwkRouteDiscoveryTime_c / 150 /* = gRouteDiscoveryExpirationInterval*/;

#if gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d
#if gStaticAddressingCapability_d
/* =======================
      STATIC ADDRESSING
   ======================= */
  /* Home Control Lighhting and Test Profile */
  /*Default values of CSkip Table to Assign the Static addresses*/
#include "CSkipCalc.h"

#endif /* #endif for Static Addr */
#endif

const uint8_t gDefaultRetriesStochasticAddrLocalDevice = gDefaultRetriesStochasticAddrLocalDevice_c;

/***************************** For APS Layer *********************************/

/*
  APS Information Base (AIB) ID=0xc0, AddressMap table

  The address map maps IEEE (8-byte) addresses to ZigBee 2-byte zbNwkAddr_t. Particularly 
  important for end-devices which may change address or in ZigBee Pro where an address 
  conflict may be detected. Used by the binding table below.
*/
zbAddressMap_t gaApsAddressMap[gApsMaxAddrMapEntries_c];
extern uint8_t gApsMaxAddressMapEntries;
uint8_t gaApsAddressMapBitMask[gApsAddressMapBitMaskInBits_c];
extern addrMapIndex_t gGlobalAddressMapCounter;

/*
  APS Information Base (AIB) ID=0xc1, Binding Table

  The binding table contains a list of "bindings" from a single source endpoint to 
  endpoints on other nodes or groups. Use zbAddrMode_t gZbAddrModeIndirect_c in the 
  AF_DataRequest() to instruct BeeStack to use the binding table on the data request.

  This is the "internal" structure for a binding table. For the ZigBee-style, use the
  ZDP commands.
*/
apsBindingTable_t gaApsBindingTable[gMaximumApsBindingTableEntries_c];
extern uint8_t gApsMaxBindingEntries;
extern uint8_t gApsMaxClusterList;

/* GroupTable */
zbGroupTable_t gaApsGroupTable[gApsMaxGroups_c];
extern uint8_t gApsMaxGroups;


extern uint8_t gEpMaskSize;


/* specifies the # of retries APS uses */
extern uint8_t gApsMaxRetries;

/* specifies the wait duration on APS ACKs before retrying */
/*const*/ extern uint16_t gApsAckWaitDuration;

/* specifies the wait for a nwk address response to complete binding information */
/*const*/ extern uint32_t gAps64BitAddressResolutionTimeout;

const zbChannels_t gaFullChannelList = 
{
  (uint8_t)(gFullChannelList_c & 0xff),
  (uint8_t)((gFullChannelList_c>>8) & 0xff),
  (uint8_t)((gFullChannelList_c>>16) & 0xff),
  (uint8_t)((gFullChannelList_c>>24) &0xff)
};

/* The latest percentage of transmission network transmission failures for the 
   previous channel just before a channel change (in percentage of failed
   transmissions to the total number of transmissions attempted)*/
uint8_t        gApsChannelFailureRate;

/* The energy measurement for the channel energy scan performed on the previous 
   channel just before a channel change*/
uint8_t        gApsLastChannelEnergy;

/*
  See Table 2.24, AIB
  A countdown timer (in hours) indicating the time to the next permitted
  frequency agility channel change. A value of NULL indicates the channel has not 
  been changed previously
  NOTE: preserve across resets.
*/
extern uint8_t        gApsChannelTimer;
uint8_t        gApsChannelTimerDefault = gApsChannelTimerDefault_c;

/* APS TxState machine (how many data requests are possible at one time. Must be at least 2. */
apsTxTable_t gaApsTxTable[ gApsMaxDataHandlingCapacity_c ];

apsTxFragCtrlSettings_t apsTxFragCtrlSettings[ gApsMaxDataHandlingCapacity_c ];

tmrTimerID_t gaApsTxTimerIDTable[ gApsMaxDataHandlingCapacity_c ];

extern uint8_t gMaxApsTxTableEntries;

apsRxFragmentationTable_t  gApsRxFragmentationTable[gApsRxFragmentationCapacity_c];

const uint8_t gMaxApsRxFragmentationTableEntries = gApsRxFragmentationCapacity_c;



/*****************************************************************************************
                                      APS SECURITY
******************************************************************************************/
/* The security parameters that are variable length tables. */
/*
  Security specific parameters.
*/
/*
  053474r17 Table 4.36
  Name: apsTrustCenterAddress
  Type: Device Address
  Range: Any valid 64-bt address
  Default: --
  AIB Id: 0xab
*/
/* saved in NVM */
zbApsDeviceKeyPairSet_t gaApsDeviceKeyPairSet[gApsMaxSecureMaterialEntries_c];
extern uint8_t giApsDeviceKeyPairCount;
zbAESKey_t  gaApsKeySet[gApsMaxLinkKeys_c];
extern zbKeyTableIndex_t gApsKeySetCount;

/* NOT saved in NVM */
zbKeyEstablish_t  *apSKKEMaterial[gDefaulEntriesInSKKEStateMachine_c];
extern uint8_t  gSKKEStateMachineSize;

/*
  Use this on the security functions to avoid having #if in the lib.
*/
#if gSKKESupported_d
extern uint8_t  gSkkeCommand;
#endif

#if gApsLinkKeySecurity_d
extern const uint8_t mMICLength[8];
#endif

/*****************************************************************************************/

#if gHighSecurity_d
/*
    This variable is an array of pointers, each entry keeps the state machine values for
    a Entity authentication process. 
*/
  zbEntityAuthentication_t  *gapEntityAuthenticationMaterial[gDefaultEntriesInEAStateMachine_c];
  uint8_t  gEAMaterialSize = gDefaultEntriesInEAStateMachine_c;
#endif


/*
  Set the default AIB variables.
*/
void SetupAIBDefaults(void)
{
  zbChannels_t channelMask;

  /* window size (1-8)  */
  ApsmeSetRequest(gApsMaxWindowSize_c, gApsWindowSizeDefault_c);

  /* APS Fragmentation: 0-255, delay in ms between frames */
  ApsmeSetRequest(gApsInterframeDelay_c, gApsInterframeDelayDefault_c);

  /* APS fragmentation, maximum size of fragment for each OTA frame */
  ApsmeSetRequest(gApsMaxFragmentLength_c, gApsMaxFragmentLengthDefault_c);

  /* List of allowed channels for the node to work in, used specially for freq.agility */
  channelMask[0] = (( mDefaultValueOfChannel_c ) & 0xFF );
  channelMask[1] = (( mDefaultValueOfChannel_c >>  8 ) & 0xFF );
  channelMask[2] = (( mDefaultValueOfChannel_c >> 16 ) & 0xFF );
  channelMask[3] = (( mDefaultValueOfChannel_c >> 24 ) & 0xFF );
  ApsmeSetRequest(gApsChannelMask_c, channelMask);

  /* for multicast */
  ApsmeSetRequest(gApsNonmemberRadius_c, gApsNonMemberRadiusDefault_c);

  /* will this node boot as a ZC? */
  ApsmeSetRequest(gApsDesignatedCoordinator_c, gApsDesignatedCoordinatorDefault_c);

  /* join securly or not */
  ApsmeSetRequest(gApsUseInsecureJoin_c, gApsUseInsecureJoinDefault_c);
}

/***************************** For AF Layer **********************************/

/* # of application endpoints, set in EndPointConfig.h, + 2means that we are including the ZDO and the Broadcst Endpoint */
extern uint8_t gNoOfEndPoints;

/* array of pointers to endpoint descriptions, set at runtime */
/* allow 2 extra ptrs for ZDP and broadcast endpoints */
endPointPtrArray_t gaEndPointDesc[ gNumberOfEndPoints_c + 2 ];

/********************************* For BeeStack NVM **************************/

/* this structure defines APS stuff that needs to be stored in NVM */
extern beeStackParameters_t gBeeStackParameters;


#if gStandardSecurity_d || gHighSecurity_d

const uint8_t gAllowNonSecurePacketsInSecureMode = gAllowNonSecure_d;

#endif


/* duplicate rejection table */
zbApsDuplicateRejectionTable_t gaApsDuplicateRejectionTable[gApscMinDuplicationRejectionTableSize_c];
extern index_t giApsDuplicationRejectionTableSize ;

/* array of active endpoints, + 2 means that the array will include the ZDO endpoint and the Bradcast Endpoint */
zbEndPoint_t gaActiveEndPointList[gNumberOfEndPoints_c + 2];

#else

/***************************** For NWK Layer *********************************/

/* Which ZigBee device type is this node (only combo devices can change type) */
#if gCoordinatorCapability_d
  #define mZbDeviceType_c gCoordinator_c
#endif

#if gRouterCapability_d
  #define mZbDeviceType_c gRouter_c
#endif

#if gEndDevCapability_d
  #define mZbDeviceType_c gEndDevice_c
#endif

#if gComboDeviceCapability_d
  #define mZbDeviceType_c gInvalidDeviceType_c
#endif


const nvmNwkData_t gDefaultNwkData = {
    { mDefaultValueOfExtendedAddress_c },
    { mDefaultValueOfMacCapFlags_c },
    { mZbDeviceType_c },
    { mDefaultLogicalChannel_c },
    { mDefaultParentShortAddress_c },   // aParentShortAddress
    { mDefaultParentLongAddress_c },    // aParentLongAddress
    { 0x00 },                           // deviceDepth
    { 0x00 },                           // nwkUpdateId;
    { mDefaultValueOfPermitJoinDuration_c } // permitJoining
  };

nvmNwkData_t gNwkData;

zbCounter_t gNibSequenceNumber;

/* Current state of network state machine */
uint8_t gNwkState;

uint16_t gNibTxTotal=0;

uint16_t gNibTxTotalFailures=0;

const uint16_t gNibPassiveAckTimeout = gNwkInfobasePassiveAckTimeout_c;

const bool_t gNibUseTreeRouting = mNwkUseTreeRouting_c;

uint16_t gNibTransactionPersistenceTime = gNwkTransPersistenceTime_c;

/*PurgeTime is : nwkPersistenceTime * SuperFrame  * 1/aBaseSuperFrameDuration */
uint16_t gNwkPurgeTimeout = gNwkTransPersistenceTime_c * gSuperFrameSpecLsbBO_c * 1.024;

const uint8_t gNibMaxBroadcastRetries = gNwkInfobaseMaxBroadcastRetries_c;

const uint8_t gNibReportConstantCost = mNwkRptConstantCost_c;

uint8_t gNibRouteDiscoveryRetriesPermitted = mNwkRouteDiscRetriesPermitted_c;

const bool_t gNibSymLink = mNwkSymmetryLink_c;

const uint8_t gNibAddrAlloc = mNwkAddressAlloc_c;

const uint8_t gNibUniqueAddr = mNwkUniqueAddr_c;

zbNwkAddr_t gNibNextAddress = {mDefaultNwkNextAddress_c};

zbNwkAddr_t gNibAvailableAddresses = {mDefaultNwkAvailableAddress_c};

zbNwkAddr_t gNibAddressIncrement={mDefaultNwkAddressIncrement_c};

const uint8_t gScanDuration = gScanDuration_c;

/* Age limit in time for end devices using SP1*/
const uint8_t gNwkAgingTimeForEndDevice = gNwkAgingTimeForEndDevice_c;

/* Age limit in time for router devices using SP1*/
const uint8_t gNwkAgingTimeForRouter = gNwkAgingTimeForRouter_c;

/* Age limit in time for sibling router devices using SP1*/
const uint8_t gNwkAgingTimeForSiblingRouter = gNwkAgingTimeForSiblingRouter_c;

/* Scale used to define the resolution for expiring the entries on the NT */
uint8_t gNwkAgingTickScale = gNwkAgingTickScale_c;

/* Age limit in time to expire route and source route table entries*/
const uint8_t gMinutesToExpireRoute = gMinutesToExpireRoute_c;

bool_t gDeviceAlreadySentRouteRecord = FALSE;
bool_t gActAsZed = FALSE;

/* This global varible is used to change the PID upper Limit */
const zbPanId_t gaPIDUpperLimit = {0xFE, 0xFF};

#if gNwkSymLinkCapability_d
/* 053474r17 ZigBee Spec.
   The time in seconds between link status command frames. 
   Default 0x0f */
uint8_t gNibNwkLinkStatusPeriod = gNetworkLinkStatusPeriod_c;

const uint8_t gNwkLinkStatusMultipleFramesPeriod = gNwkLinkStatusMultipleFramesPeriod_c;
#endif

const uint8_t gNibNwkRouterAgeLimit =  gNetworkRouterAgeLimit_c;

#if gComboDeviceCapability_d
bool_t gLpmIncluded = FALSE;
#endif
  /* 
    Default:  TRUE
    A flag determining the layer where multicast messaging occurs.
    TRUE = multicast occurs at the network layer.
    FALSE= multicast occurs at the APS layer and using the APS header.
  */
/* Don't touch this line */
bool_t gNibNwkUseMulticast = mUseMultiCast_c;
bool_t gNwkUseMulticastForApsGroupDataRequest = mDefaultUseMultiCastForApsGroupDataRequest_c;

uint8_t gMulticastMaxNonMemberRadius_c = 0x07;

/* Energy Threshhold used when performing ED Scan */
const uint8_t  gNwkEnergyLevelThresHold = gNwkEnergyLevelThresHold_d;

/*  Default Transmit Failure Counter, indicates how many times have to fail a 
  transmition to a specific device before a route repair is initiated */
  
/* This variable is used to set the protocol ID uesd in Beacon Payload, This
  can be changed by the user with a proper value.
  Note:Two devices with diffrent protocol Id will never join*/
const uint8_t gNwkProtocolId = gDefaultValueOfNwkProtocolId_c;

/* This variable is used for setting the Radius Counter parameter to the
   default value which is 2 times the maximun depth of a network */
const zbCounter_t gDefaultRadiusCounter = 2 * gNwkMaximumDepth_c;

/* This variable is used to hold the max children, that a ZC/ZR can accept.
  Note: This has to be the same or greater than Nwk Max Routers */
const uint8_t gNibMaxChildren=gNwkMaximumChildren_c;

/* This variable is used to hold the max routers, that a ZC/ZR can accept.
  Note: This has to be the same or greater than Nwk Max Children */
const uint8_t gNibMaxRouter=gNwkMaximumRouters_c;

/* This variable is used to hold the max depth at which the device can be present
  Note: The ZR at Last depth cannot Accept children */
const uint8_t gNibMaxDepth=gNwkMaximumDepth_c;

/* Time duration in seconds that a broadcast message needs to encompass the
  entire network. Devices using the ZigBee stack profile must set: 
  nwkBroadcastDeliveryTime = 3 secs */
const uint32_t gNibNetworkBroadcastDeliveryTime	= gNwkInfobaseBroadcastDeliveryTime_c;

/* This is the variable that contains our neighbors information */
neighborTable_t gaNeighborTable[gNwkInfobaseMaxNeighborTableEntry_c];

/* This variable tells the number of neighbour table entries suppoted by the
  network layer, this can be changed at compile time by assigning a proper
	value to gNwkInfobaseMaxNeighborTableEntry_c macro*/
const uint8_t gMaxNeighborTableEntries = gNwkInfobaseMaxNeighborTableEntry_c;


/* 
  This variable contains sorted the indexes of the neighbor table in ascending order.
  This array only is used when NwkSymLink is TRUE and is used by Link Status Command 
  to build the link status information list. This array has the same length of the gaNeighborTable 
*/
#if gNwkSymLinkCapability_d && (( gCoordinatorCapability_d || gRouterCapability_d ) || gComboDeviceCapability_d)
uint8_t gaNeighborTableAscendantSorted[gNwkInfobaseMaxNeighborTableEntry_c];
#endif


/* This table will exist only if a discovery network has been performed 
  previously and it last only during joining or forming a network */
neighborTable_t *gpDiscoveryTable = NULL;
extendedNeighborTable_t *gpExtendedDiscoveryTable = NULL;


/* This value will change depending on the size of memory allocated for
  gpDiscoveryTable and gpExtendedDiscoveryTable */
index_t gMaxDiscoveryTableEntries=0;


/* Number Of Outstanding Broadcasts that are supported. More than one
  outstanding broadcast can be supported at the network layer only when
  passive acknowledgement and retry is compiled out */
const uint8_t gMaxBroadcastTransactionTableEntries = gMaxBroadcastTransactionTableEntries_c;

/* this table keeps the pointer to the messages that are waiting to be routed.
  waiting for a route discovery or device authentication */
packetsOnHoldTable_t gaPacketsOnHoldTable[gPacketsOnHoldTableSize_c];
const uint8_t gPacketsOnHoldTableSize = gPacketsOnHoldTableSize_c;

/* this table keeps track of every unicast data request sent to the NLL */
handleTrackingTable_t gaHandleTrackingTable[gHandleTrackingTableSize_c];
const uint8_t gHandleTrackingTableSize = gHandleTrackingTableSize_c;

/* keeps track of each rejoin reuqest until copleted. */
nwkRejoinEntry_t *gpNwkRejoinStateMachine;
/* The buffer used is as big as a small bufffer. */
const uint8_t gNwkRejoinStateMachineSize = gNwkRejoinStateMachineSize_c;
/* The size of the buffer divided by the amount of entries. */
const uint8_t gNwkRejoinStateMachineAmountOfEntries = gNwkRejoinStateMachineAmountOfEntries_c;
/* The amount of time in millisecodn to exppire the Nwk Rejoin state machine. */
const tmrTimeInMilliseconds_t  gNwkRejoinExpirationTime = gNwkRejoinExpirationTime_c;

/* indicates how many entries of the handleTrackingTable are occupied by sleeping devices */
zbCounter_t gHttIndirectEntries;
  
/* Sets the maximum number of failures to tolarate in the network layer before
  it send a report up. FA module. */
const uint8_t gMaxNumberOfTxAttempts = (gMaxNumberOfTxAttempts_c+1);

const uint8_t gHttMaxIndirectEntries = gHttMaxIndirectEntries_c;

const uint8_t gNwkHighRamConcentrator = gNwkHighRamConcentrator_d;

#if gStandardSecurity_d || gHighSecurity_d
zbIncomingFrameCounterSet_t  gaIncomingFrameCounterSet1[gNwkInfobaseMaxNeighborTableEntry_c];
zbIncomingFrameCounterSet_t  gaIncomingFrameCounterSet2[gNwkInfobaseMaxNeighborTableEntry_c];
const uint8_t                gIncomingFrameCounterSetLimit = gNwkInfobaseMaxNeighborTableEntry_c;

/* Set the defualt transmision options for SSP. */
#if gApsLinkKeySecurity_d
/* default to APS layer security. */
const uint8_t gTxOptionsDefault = (gApsTxOptionSecEnabled_c);
#else
/* default to no APS layer security (both ZigBee and ZigBee Pro) */
const uint8_t gTxOptionsDefault = gApsTxOptionNone_c;
#endif


#if gComboDeviceCapability_d
bool_t gTrustCenter = FALSE;
#endif
#endif

#if ( !gNwkBroadcastPassiveAckRetryCapability_d )

const uint8_t gBroadcastDeliveryInterval_c = 250;
const uint8_t gBroadcastJitterInterval_c = 10;

  /* This value determines the number of ticks before a broadcast transaction entry expires
  in the BroadcastTransactionTable. Changed to constant gNwkInfobaseBroadcastDeliveryTime_c*/
  const uint8_t gBroadcastDeliveryNumTicks = ( gNwkInfobaseBroadcastDeliveryTime_c / 250 /* = gBroadcastDeliveryInterval_c */) ;

  /* Broadcast Transaction Table, size of this table is decided by the number
  of broadcast requests that handled by the network layer simultaneously */
  broadcastTransactionTable_t gaBroadcastTransactionTable[gMaxBroadcastTransactionTableEntries_c ];

#else

#if gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d
  /* Broadcast Transaction Table, size of this table is decided by the number
      of broadcast requests that handled by the network layer simultaneously */
  broadcastTransactionTable_t gaBroadcastTransactionTable[ 1 ];
#endif

#endif ( !gNwkBroadcastPassiveAckRetryCapability_d )

#if gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d
#if gRnplusCapability_d
  routeTable_t gaRouteTable[gNwkInfobaseMaxRouteTableEntry_c];
#endif
#endif

/*This variable hold the value of Maximum route table Entries supported, this
  can be changed by the user at compile time by updating
  gNwkInfobaseMaxRouteTableEntry_c macro by proper value */
const uint8_t gMaxRouteTableEntries = gNwkInfobaseMaxRouteTableEntry_c;

/* Route Discovery Table Entries, to update the number of route Discovery
  table entries, the value of gNwkRoutingMaxRouteDiscoveyTableEntry_c macro
  needs to be updated */
routeDiscoveryTable_t gaRouteDiscoveryTable[gNwkRoutingMaxRouteDiscoveyTableEntry_c ];

/*This variable hold the value of Number of route Discovery Entries
  supported, this can be changed by the user at compile time by updating
  gNwkRoutingMaxRouteDiscoveyTableEntry_c macro by proper value */
const uint8_t gMaxRouteDiscoveryTableEntries = gNwkRoutingMaxRouteDiscoveyTableEntry_c;

const uint8_t gRouteRequestRetryInterval = 8;

const uint8_t gRouteDiscoveryExpirationInterval = 150;

const uint8_t gRouteDiscoveryExpirationNumTicks = gNibNwkRouteDiscoveryTime_c / 150 /* = gRouteDiscoveryExpirationInterval*/;

#if gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d
#if gStaticAddressingCapability_d
/* =======================
      STATIC ADDRESSING
   ======================= */
  /* Home Control Lighhting and Test Profile */
  /*Default values of CSkip Table to Assign the Static addresses*/
#include "CSkipCalc.h"

#endif /* #endif for Static Addr */
#endif

const uint8_t gDefaultRetriesStochasticAddrLocalDevice = gDefaultRetriesStochasticAddrLocalDevice_c;

/***************************** For APS Layer *********************************/

/*
  APS Information Base (AIB) ID=0xc0, AddressMap table

  The address map maps IEEE (8-byte) addresses to ZigBee 2-byte zbNwkAddr_t. Particularly 
  important for end-devices which may change address or in ZigBee Pro where an address 
  conflict may be detected. Used by the binding table below.
*/
zbAddressMap_t gaApsAddressMap[gApsMaxAddrMapEntries_c];
const uint8_t gApsMaxAddressMapEntries = gApsMaxAddrMapEntries_c;
uint8_t gaApsAddressMapBitMask[gApsAddressMapBitMaskInBits_c];
addrMapIndex_t gGlobalAddressMapCounter;

/*
  APS Information Base (AIB) ID=0xc1, Binding Table

  The binding table contains a list of "bindings" from a single source endpoint to 
  endpoints on other nodes or groups. Use zbAddrMode_t gZbAddrModeIndirect_c in the 
  AF_DataRequest() to instruct BeeStack to use the binding table on the data request.

  This is the "internal" structure for a binding table. For the ZigBee-style, use the
  ZDP commands.
*/
apsBindingTable_t gaApsBindingTable[gMaximumApsBindingTableEntries_c];
const uint8_t gApsMaxBindingEntries = gMaximumApsBindingTableEntries_c;
const uint8_t gApsMaxClusterList = gMaximumApsBindingTableClusters_c;

/* GroupTable */
zbGroupTable_t gaApsGroupTable[gApsMaxGroups_c];
const uint8_t gApsMaxGroups = gApsMaxGroups_c;

/* You know the question Neo.... What is the matrix? */
const uint8_t gEpMaskSize = gEndPointsMaskSizeInBytes_c;


/* specifies the # of retries APS uses */
const uint8_t gApsMaxRetries = gApsMaxRetries_c;

/* specifies the wait duration on APS ACKs before retrying */
const uint16_t gApsAckWaitDuration = gApsAckWaitDuration_c;

/* specifies the wait for a nwk address response to complete binding information */
const uint32_t gAps64BitAddressResolutionTimeout = gAps64BitAddressResolutionTimeout_c;

const zbChannels_t gaFullChannelList = 
{
  (uint8_t)(gFullChannelList_c & 0xff),
  (uint8_t)((gFullChannelList_c>>8) & 0xff),
  (uint8_t)((gFullChannelList_c>>16) & 0xff),
  (uint8_t)((gFullChannelList_c>>24) &0xff)
};

/* The latest percentage of transmission network transmission failures for the 
   previous channel just before a channel change (in percentage of failed
   transmissions to the total number of transmissions attempted)*/
uint8_t        gApsChannelFailureRate;

/* The energy measurement for the channel energy scan performed on the previous 
   channel just before a channel change*/
uint8_t        gApsLastChannelEnergy;

/*
  See Table 2.24, AIB
  A countdown timer (in hours) indicating the time to the next permitted
  frequency agility channel change. A value of NULL indicates the channel has not 
  been changed previously
  NOTE: preserve across resets.
*/
uint8_t        gApsChannelTimer;
uint8_t        gApsChannelTimerDefault = gApsChannelTimerDefault_c;

/* APS TxState machine (how many data requests are possible at one time. Must be at least 2. */
apsTxTable_t gaApsTxTable[ gApsMaxDataHandlingCapacity_c ];

apsTxFragCtrlSettings_t apsTxFragCtrlSettings[ gApsMaxDataHandlingCapacity_c ];

tmrTimerID_t gaApsTxTimerIDTable[ gApsMaxDataHandlingCapacity_c ];

uint8_t gMaxApsTxTableEntries = gApsMaxDataHandlingCapacity_c;

apsRxFragmentationTable_t  gApsRxFragmentationTable[gApsRxFragmentationCapacity_c];

const uint8_t gMaxApsRxFragmentationTableEntries = gApsRxFragmentationCapacity_c;

/*****************************************************************************************
                                      APS SECURITY
******************************************************************************************/
/* The security parameters that are variable length tables. */
/*
  Security specific parameters.
*/
/*
  053474r17 Table 4.36
  Name: apsTrustCenterAddress
  Type: Device Address
  Range: Any valid 64-bt address
  Default: --
  AIB Id: 0xab
*/
/* saved in NVM */
zbApsDeviceKeyPairSet_t gaApsDeviceKeyPairSet[gApsMaxSecureMaterialEntries_c];
const uint8_t giApsDeviceKeyPairCount = gApsMaxSecureMaterialEntries_c;
zbAESKey_t  gaApsKeySet[gApsMaxLinkKeys_c];
const zbKeyTableIndex_t gApsKeySetCount = gApsMaxLinkKeys_c;

/* NOT saved in NVM */
zbKeyEstablish_t  *apSKKEMaterial[gDefaulEntriesInSKKEStateMachine_c];
uint8_t  gSKKEStateMachineSize = gDefaulEntriesInSKKEStateMachine_c;

/*
  Use this on the security functions to avoid having #if in the lib.
*/
#if gSKKESupported_d
  const uint8_t  gSkkeCommand = sizeof(zbApsmeSKKECommand_t);
#else
  const uint8_t  gSkkeCommand = 0;
#endif

#if gApsLinkKeySecurity_d
extern const uint8_t mMICLength[8];
#endif

/*****************************************************************************************/

#if gHighSecurity_d
/*
    This variable is an array of pointers, each entry keeps the state machine values for
    a Entity authentication process. 
*/
  zbEntityAuthentication_t  *gapEntityAuthenticationMaterial[gDefaultEntriesInEAStateMachine_c];
  uint8_t  gEAMaterialSize = gDefaultEntriesInEAStateMachine_c;
#endif


/*
  Set the default AIB variables.
*/
void SetupAIBDefaults(void)
{
  zbChannels_t channelMask;

  /* window size (1-8)  */
  ApsmeSetRequest(gApsMaxWindowSize_c, gApsWindowSizeDefault_c);

  /* APS Fragmentation: 0-255, delay in ms between frames */
  ApsmeSetRequest(gApsInterframeDelay_c, gApsInterframeDelayDefault_c);

  /* APS fragmentation, maximum size of fragment for each OTA frame */
  ApsmeSetRequest(gApsMaxFragmentLength_c, gApsMaxFragmentLengthDefault_c);

  /* List of allowed channels for the node to work in, used specially for freq.agility */
  channelMask[0] = (( mDefaultValueOfChannel_c ) & 0xFF );
  channelMask[1] = (( mDefaultValueOfChannel_c >>  8 ) & 0xFF );
  channelMask[2] = (( mDefaultValueOfChannel_c >> 16 ) & 0xFF );
  channelMask[3] = (( mDefaultValueOfChannel_c >> 24 ) & 0xFF );
  ApsmeSetRequest(gApsChannelMask_c, channelMask);

  /* for multicast */
  ApsmeSetRequest(gApsNonmemberRadius_c, gApsNonMemberRadiusDefault_c);

  /* will this node boot as a ZC? */
  ApsmeSetRequest(gApsDesignatedCoordinator_c, gApsDesignatedCoordinatorDefault_c);

  /* join securly or not */
  ApsmeSetRequest(gApsUseInsecureJoin_c, gApsUseInsecureJoinDefault_c);
}

/***************************** For AF Layer **********************************/

/* # of application endpoints, set in EndPointConfig.h, + 2means that we are including the ZDO and the Broadcst Endpoint */
const uint8_t gNoOfEndPoints = (gNumberOfEndPoints_c + 2);

/* array of pointers to endpoint descriptions, set at runtime */
/* allow 2 extra ptrs for ZDP and broadcast endpoints */
endPointPtrArray_t gaEndPointDesc[ gNumberOfEndPoints_c + 2 ];

/********************************* For BeeStack NVM **************************/

/* this structure defines APS stuff that needs to be stored in NVM */
beeStackParameters_t gBeeStackParameters;


#if gStandardSecurity_d || gHighSecurity_d

const uint8_t gAllowNonSecurePacketsInSecureMode = gAllowNonSecure_d;

#endif


/* duplicate rejection table */
zbApsDuplicateRejectionTable_t gaApsDuplicateRejectionTable[gApscMinDuplicationRejectionTableSize_c];
const index_t giApsDuplicationRejectionTableSize = gApscMinDuplicationRejectionTableSize_c;

/* array of active endpoints, + 2 means that the array will include the ZDO endpoint and the Bradcast Endpoint */
zbEndPoint_t gaActiveEndPointList[gNumberOfEndPoints_c + 2];

#endif

/****************************************************************************/

/* TaskIDs. The MAC library needs to know the IDs for some of the tasks, so
 * it can set events for them. But it is supplied as a binary library, so it
 * can't use #define or enum constants, and the TaskIDs might change. TaskIDs
 * are assigned dynamically by the kernel.
 * Instead, there is integer in RAM for each task that contains that task's
 * ID. This costs some RAM, but the MAC library only needs to know the names
 * of the integers, not the TaskIDs, at link time.
 */
#if MC13226Included_d
extern tsTaskID_t gApsTaskID_c;
extern tsTaskID_t gAfTaskID_c;
extern tsTaskID_t gNwkTaskID_c;

tsTaskID_t gZdoTaskID_c;
tsTaskID_t gZdoStateMachineTaskID_c;
tsTaskID_t gMacTaskID_c;
#else
#define Task( taskIdGlobal, taskInitFunc, taskMainFunc, priority ) \
  tsTaskID_t taskIdGlobal;
#include "BeeStackTasksTbl.h"
#endif



/******************************************************************************
*******************************************************************************
* Private Debug Stuff
*******************************************************************************
******************************************************************************/

#if (gStandardSecurity_d || gHighSecurity_d) && (gTrustCenter_d || gComboDeviceCapability_d) && gDefaultValueOfMaxEntriesForExclusionTable_c
/* security scan exclude nodes from the network */
const uint8_t gExclusionMax = gDefaultValueOfMaxEntriesForExclusionTable_c;
zbIeeeAddr_t gaExclusionTable[gDefaultValueOfMaxEntriesForExclusionTable_c];
#endif

#if gICanHearYouTableCapability_d
/* 
  ICanHearYou Table Entries = NWK Short Addresses that this device can hear, All packets received 
  via McpsDataIndication with MAC SourceAddress not listed in this table will be discarded by NWK 
  layer. Use for table-top demonstrations and debugging. Not saved to NVM.
*/
zbNwkAddr_t gaICanHearYouTable[gDefaultValueOfMaxEntriesForICanHearYouTable_c];
uint8_t gaICanHearYouLqi[gDefaultValueOfMaxEntriesForICanHearYouTable_c];

/* Specifies how many entries of gaICanHearYouTable are active */
uint8_t gICanHearYouCounter = 0;

/* States the limit size for i can hear you table */
const index_t gICanHearYouMax = gDefaultValueOfMaxEntriesForICanHearYouTable_c;
#endif
uint8_t giIcanHearLinkCost = 0;   // note: 0 means use real Link Cost.

/******************************************************************************
*******************************************************************************
* Fragmentation Capability 
*******************************************************************************
******************************************************************************/
#if MC13226Included_d
/* is fragmentation enabled? */
extern bool_t gfFragmentationEnabled;

#if gEndDevCapability_d || gComboDeviceCapability_d
extern uint16_t gApsFragmentationPollRate;
#endif

#else
/* is fragmentation enabled? */
const bool_t gfFragmentationEnabled = gFragmentationCapability_d;

#if gEndDevCapability_d || gComboDeviceCapability_d
const uint16_t gApsFragmentationPollRate = gApsFragmentationPollTimeOut_c;
#endif

#endif


/*
  This function is called from within the BeeStack ZigBee library to handle a fragmented 
  transmission (Tx). If fragmentation is disabled at compile-time, then only the (small) 
  stub function is used to save code.
*/
bool_t APS_FragmentationTxStateMachine(apsTxIndex_t iTxIndex, apsTxState_t txState)
{
#if(gFragmentationCapability_d)
  return (APS_ActualFragmentationTxStateMachine(iTxIndex, txState));
#else
  return (APS_StubFragmentationTxStateMachine(iTxIndex, txState));
#endif
}

/*
  This function is called from within the BeeStack ZigBee library to handle a fragmented 
  reception (Rx). If fragmentation is disabled at compile-time, then only the (small) 
  stub function is used to save code.
*/
void APS_FragmentationRxStateMachine(void)
{
#if(gFragmentationCapability_d)
  APS_ActualFragmentationRxStateMachine();
#else
  APS_StubFragmentationRxStateMachine();
#endif
}

zbStatus_t Aps_SaveFragmentedData(index_t iIndexRxTable)
{
#if(gFragmentationCapability_d)
  return Aps_ActualSaveFragmentedData(iIndexRxTable);
#else
  (void)iIndexRxTable;
  return Aps_StubSaveFragmentedData();
#endif
}



#if ((gStandardSecurity_d || gHighSecurity_d) && gApsMaxEntriesForPermissionsTable_c)
/* Permissions Configuration Table */
permissionsTable_t  gaPermissionsTable[gApsMaxEntriesForPermissionsTable_c];

uint8_t gPermissionsTableCount;

#endif /* ((gStandardSecurity_d || gHighSecurity_d) && gApsMaxEntriesForPermissionsTable_c) */

/*****************************************************************************************
                          APS SECURITY COMPILE TIME CODE.
******************************************************************************************/

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
)
{
#if gApsLinkKeySecurity_d
  SSP_ApsResetSecurityMaterial();
#endif
}

#ifndef gHostApp_d
/* This clear the APS security material, must be use when a node leaves. */
void APS_RemoveSecurityMaterialEntry
(
  uint8_t *pExtendedAddress
)
{
#if gApsLinkKeySecurity_d
  SSP_ApsRemoveSecurityMaterialEntry(pExtendedAddress); 
  /*Make sure device is removed from address map if it is not used for bindings*/
  (void)APS_RemoveDeviceFromAddressMap(pExtendedAddress);
#else
  (void)pExtendedAddress;
#endif
}
#endif

void APS_ResetDeviceCounters(uint8_t *pIEEEAddress)
{
#if gApsLinkKeySecurity_d
  SSP_ResetDeviceCounters(pIEEEAddress);
#else
  (void)pIEEEAddress;
#endif
}

uint8_t APS_TunnelCommandSize(void)
{
#if (gApsLinkKeySecurity_d && (gHighSecurity_d || gStandardSecurity_d))
  uint8_t size = 0;
  size = MbrOfs(zbApsmeTunnelCommand_t, tunnelCommand);
  /* By default we just support the Trasnport key more considerations can be added later. */
  size += sizeof(zbApsmeTransportKeyCommand_t);
  size += mMICLength[NlmeGetRequest(gNwkSecurityLevel_c)];
  return size;
#else
  return 0;
#endif

}

zbSize_t  APS_GenerateTunnelCommand
(
  zbApsmeTunnelCommand_t  *pTunnelCommand,
  zbApsmeTunnelReq_t  *pTunnelReq,
  uint8_t *pDestAddress,
  bool_t *pSecurityEnable
)
{
#if gApsLinkKeySecurity_d && (gHighSecurity_d || gStandardSecurity_d)
  return SSP_GenerateTunnelCommand(pTunnelCommand, pTunnelReq, pDestAddress, pSecurityEnable);
#else
  (void)pTunnelCommand;
  (void)pTunnelReq;
  (void)pDestAddress;
  (void)pSecurityEnable;
  return 0;
#endif
}

void APS_SetSecureTxOptions
(
  zbApsTxOption_t  *pTxOptions,
  zbApsCommandID_t  cmdID,
  uint8_t *pDestAddress
)
{
#if gApsLinkKeySecurity_d
  SSP_ApsSetTxOptions(pTxOptions, cmdID, pDestAddress);
#else
  (void)pTxOptions;
  (void)cmdID;
  (void)pDestAddress;
#endif
}

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
)
{
#if gApsLinkKeySecurity_d
  return SSP_ApsProcessOutgoingFrame(pApsHeader,
                                     apsHeaderLength,
                                     pApsFrameLength,
                                     aDestAddress,
                                     txOptions,
                                     apsCommandID);
#else
  (void)pApsHeader;
  (void)apsHeaderLength;
  (void)pApsFrameLength;
  (void)aDestAddress;
  (void)txOptions;
  (void)apsCommandID;
  return gZbSuccess_c;
#endif
}

bool_t ApsEncryptACK(zbIeeeAddr_t aDestAddress)
{
#if gStandardSecurity_d || gHighSecurity_d
#if gTrustCenter_d || gComboDeviceCapability_d

#if gComboDeviceCapability_d
  if (gTrustCenter)
#endif
  {
    uint8_t aExtAddr[8];
    if (!SecurityTypeCheck(APS_GetIeeeAddress(aDestAddress, aExtAddr)))
    {
      return TRUE;
    }
  }
#else
  (void)aDestAddress;
#endif
#else
  (void)aDestAddress;
#endif
  return FALSE;
}

zbStatus_t  APS_ProcessIncomingFrames
(
  uint8_t  *pApsHeader,
  uint8_t  *pApsFrameLength,
  zbNwkAddr_t  aSrcAddress
)
{
#if gApsLinkKeySecurity_d
  return SSP_ApsProcessIncomingFrames(pApsHeader, pApsFrameLength, aSrcAddress);
#else
  (void)pApsHeader;
  (void)pApsFrameLength;
  (void)aSrcAddress;
  return gZbSuccess_c;
#endif
}

#ifndef gHostApp_d 
void APS_RegisterLinkKeyData
(
  zbIeeeAddr_t  aDeviceAddress,
  zbKeyType_t  keyType,
  uint8_t  *pKey
)
{
#if gApsLinkKeySecurity_d
  /*
    Whe a node wishes to register a link key (either is a Master key or not)
    uses the field parent address as the device with whom is sharing the key.
  */
 
  SSP_ApsRegisterLinkKeyData(aDeviceAddress, keyType, pKey);
#else
  (void)aDeviceAddress;
  (void)keyType;
  (void)pKey;
#endif
}
#endif

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
)
{
#if gSKKESupported_d
  return  SSP_SKKE1(pSKKE,
                    pEstablishKeyReq,
                    pDestAddress,
                    pSecurityEnable);
#else
  (void)pSKKE;
  (void)pEstablishKeyReq;
  (void)pDestAddress;
  (void)pSecurityEnable;
  return 0;
#endif
}

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
)
{
#if gSKKESupported_d
  return SSP_SKKE2(pSKKE2,
                   pEstablishKeyRsp,
                   pDestAddress,
                   pSecurityEnable);
#else
  (void)pSKKE2;
  (void)pEstablishKeyRsp;
  (void)pDestAddress;
  (void)pSecurityEnable;
  return 0;
#endif
}

zbApsmeZdoIndication_t *APSME_SKKE_indication
(
  uint8_t                commandId,
  zbApsmeSKKECommand_t  *pSKKECmd,
  zbIeeeAddr_t           aSrcAddress
)
{
#if gSKKESupported_d
  zbApsmeZdoIndication_t *pZdoInd;

  switch (commandId)
  {
    case gAPS_CMD_SKKE_1_c:
      pZdoInd = APSME_SKKE1_indication((void *)pSKKECmd, aSrcAddress);
      break;

    case gAPS_CMD_SKKE_2_c:
      pZdoInd = APSME_SKKE2_indication((void *)pSKKECmd);
      break;

    case gAPS_CMD_SKKE_3_c:
      pZdoInd = APSME_SKKE3_indication((void *)pSKKECmd);
      break;

    case gAPS_CMD_SKKE_4_c:
      pZdoInd = APSME_SKKE4_indication((void *)pSKKECmd);
      break;

    default:
      pZdoInd = NULL;
      break;
  }

  return pZdoInd;
#else
  (void)commandId;
  (void)pSKKECmd;
  (void)aSrcAddress;
  return NULL;
#endif
}

zbApsmeZdoIndication_t *APSME_TUNNEL_CMMD_indication
(
  zbApsmeTunnelCommand_t  *pTunnel
)
{
#if gApsLinkKeySecurity_d
  return APSME_TUNNEL_indication((void *)pTunnel);
#else
  (void)pTunnel;
  return NULL;
#endif
}

zbApsDeviceKeyPairSet_t * APS_GetSecurityMaterilaEntry
(
  uint8_t *pDeviceAddress,
  zbApsDeviceKeyPairSet_t *pWhereToCpyEntry
)
{
#if gApsLinkKeySecurity_d
  return SSP_ApsGetSecurityMaterilaEntry(pDeviceAddress, pWhereToCpyEntry);
#else
  (void)pDeviceAddress;
  (void)pWhereToCpyEntry;
  return NULL;
#endif
}


