/******************************************************************************
* BeeStackConfiguration.h
*
* This is the public header file to configure the table sizes used in
* ZigBee stack and to configure the beestack using the compile time
* configurable parameters.
*
* Copyright 2008 (c), Freescale, Inc.  All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from  Freescale Semiconductor.
*
******************************************************************************/
#ifndef _BEESTACK_CONFIGURATION_H_
#define _BEESTACK_CONFIGURATION_H_



#include "BeeStackFunctionality.h"

#ifndef TRUE
#define TRUE    1
#endif

#ifndef FALSE
#define FALSE   0
#endif

/******************************************************************************
*******************************************************************************
* Public macros
*******************************************************************************
******************************************************************************/

/***********************************************

  These macros build specific libraries (Secure, ZC, ZR, ZED, etc..)
  Do NOT change them. Used to build BeeStack/BeeKit environment.

************************************************/

/*
  Default Device type

  Depending on the Device Type either of One compiler time option needs to be Selected 
  This options depend on the type of device selected in compile line and described in 
  BeeStackFunctionality.h
*/
#ifndef gCoordinatorCapability_d
#define gCoordinatorCapability_d  FALSE
#endif
#ifndef gRouterCapability_d
#define gRouterCapability_d  FALSE
#endif
#ifndef gEndDevCapability_d
#define gEndDevCapability_d  FALSE
#endif
#ifndef gComboDeviceCapability_d
#define gComboDeviceCapability_d FALSE
#endif

/* Defaults to standard security (Stack Profile 0x01) */
#ifndef gStandardSecurity_d
#define gStandardSecurity_d  TRUE
#endif

/* High Security Capability (Stack Profile 0x02) */
#ifndef gHighSecurity_d
#define gHighSecurity_d  FALSE
#endif

/* Default Routing Capability (RN+ refers to mesh, RN- is tree only) */
/* This compiler time option needs to be selected if the Mesh Routing is to be used */
#ifndef gRnplusCapability_d
#define gRnplusCapability_d  FALSE
#endif


/***********************************************

  From here forward the MACROs may be set by the user.
  OK to change from defaults.

************************************************/


/*--------------------------- For ZCL --------------------*/
 /* Enable attribute validation */
#ifndef gAddValidationFuncPtrToClusterDef_c
#define gAddValidationFuncPtrToClusterDef_c TRUE
#endif


/*--------------------------- For Inter-Pan Transmission --------------------*/
#ifndef gInterPanCommunicationEnabled_c
#define gInterPanCommunicationEnabled_c  FALSE
#endif

/*--------------------------- For NWK layer ---------------------------------*/

/*
  Enables manufacturer specific profile ID. If this flag is set to FALSE then 
  gAppStackProfile_c will take its value depending on gZigBeeProIncluded_d
  in BeeStackCalculation.h. If this flag is TRUE then gAppStackProfile_c 
  will keep the value set down below.
*/
#ifndef gMSPstackProfileEnabled_d
#define gMSPstackProfileEnabled_d    FALSE
#endif

/*
  Enables Stack Profile 2 for the application also know as ZigBee Pro. Some 
  application configuration values depend on this flag see BeestackCalculation.h.
  For library configuration see BeeStackFunctionality.h
*/  
#ifndef gZigbeeProIncluded_d
#define gZigbeeProIncluded_d TRUE
#endif

/*
  Stack profile value over the air. If gMSPstackProfileEnabled_d is FALSE 
  then its value depends on gZigBeeProIncluded_d, and will be changed in
  BeeStackCalculation.h. If gMSPstackProfileEnabled_d is TRUE the value 
  won't be changed.  
*/
#ifndef gAppStackProfile_c
#define gAppStackProfile_c    2
#endif

/*
  Number Of Outstanding Broadcasts that are supported. More than one outstanding 
  broadcast can be supported at the network layer only when passive acknowledgement and 
  retry is compiled out. Devices using the ZigBee stack profile must set: Broadcast 
  Transaction Table size: 9 (minimum)

  Range:   1 - 255
  Default: 9
*/
#ifndef gMaxBroadcastTransactionTableEntries_c
#define gMaxBroadcastTransactionTableEntries_c  9
#endif


/*
  Network Passive Ack Timeout(As Per Specification).  Devices using the ZigBee
  stack profile must set: nwkPassiveAckTimeout = 0.5 secs (maximum)

  Range:   1 - 0x3ffff (4 minutes)
  Default: 500
*/
#ifndef gNwkInfobasePassiveAckTimeout_c
#define gNwkInfobasePassiveAckTimeout_c         500
#endif

/* 
  Time duration in seconds that a broadcast message needs to encompass the entire 
  network.

  Devices using the ZigBee stack profile must set: nwkBroadcastDeliveryTime = 9 secs

  Range:   1 - 0x3ffff (4 minutes)
  Default: 9000
*/
#ifndef gNwkInfobaseBroadcastDeliveryTime_c
#define gNwkInfobaseBroadcastDeliveryTime_c     9000
#endif

/* 
  Time duration is time in ticks that a neighbor table entry for an end device needs to wait
  to be expired. This feature is for both stack profiles. In order to control hours, days
  and months see gNwkAgingTickScale_c, here below.

  Range:  0 .. 255 (ticks)
  Default: 5
*/
#ifndef gNwkAgingTimeForEndDevice_c
#define gNwkAgingTimeForEndDevice_c     0x05
#endif

/* 
  Time duration is time in ticks that a neighbor table entry for a router needs to wait
  to be expired. This feature is only for SP1 for SP2 see gNetworkRouterAgeLimit_c.
  In order to control hours, days and months see gNwkAgingTickScale_c, here below.

  Range:  0 .. 255 (ticks)
  Default: 10
*/
#ifndef gNwkAgingTimeForRouter_c
#define gNwkAgingTimeForRouter_c     0x0A
#endif

/* 
  Time duration is time in ticks that a neighbor table entry for a sibling router needs to wait
  to be expired. This feature is only for SP1.
  In order to control hours, days and months see gNwkAgingTickScale_c, here below.

  Range:  0 .. 255 (ticks)
  Default: 5
*/
#ifndef gNwkAgingTimeForSiblingRouter_c
#define gNwkAgingTimeForSiblingRouter_c     2
#endif

/*
  Scale in minutes to use in order to expand the time in ticks for aginig the Neighbor Table
  entries.

  To have a resolution one-to-one, this is, one ticket per minute, a value of one in the
  scale is needed. Meaning that every minute one tick will be decreased until the tick
  counter reaches zero.

  Examples:
  For (Routers) gNwkAgingTimeForRouter_c 10, with Scale set to 1, means that the NT for
  routers will be decreased one tick every minute, and will expire once the tick counter
  reaches zero.

  For (Routers) gNwkAgingTimeForRouter_c 10, with Scale set to 2, means that the NT for
  routers will be decreased one tick every two minute, and will expire once the tick
  counter (10), reaches zero.

  For (End Devices) gNwkAgingTimeForEndDevice_c 5, with Scale set to 70, means that the NT for
  routers will be decreased one tick every 70 minutes, and will expire once the tick
  counter (5), reaches zero.

  To reach 45 days (month and a half), on routers or end devices, we need Aging time set
  to 255 (ticks) and an Aging scale of 255 minutes, meaning that the NT will be decreased
  one tick every 255 minutes until the the tick counter (255) reaches Zero.

    (255ticks * 255minutes) = 65025 ticks/minutes.
    65025/60 = 1083.75hours.
    1083.75hours/24 = 45.15days.

  Range:  0 .. 255 minutes
  Default: 1
*/
#ifndef gNwkAgingTickScale_c
#define gNwkAgingTickScale_c      0x01
#endif

/* 
  Time duration in minutes that a route and source route table entry needs to wait
  to be expired. Set it to 0, it means never expire routes on time.
  
  Range:  1  -  255 ( 4 hrs )
  Default: 10
*/
#ifndef gMinutesToExpireRoute_c
#define gMinutesToExpireRoute_c     0x0A
#endif


/*
  Network Maximum number of Broadcast retries. Devices using the ZigBee stack
  profile must set: nwkMaxBroadcastRetries = 2

  Range:   1 - 7
  Default: 0x02
*/
#ifndef gNwkInfobaseMaxBroadcastRetries_c
#define gNwkInfobaseMaxBroadcastRetries_c       2
#endif

/*
  This value is used to purge the data after the below specified duration.
  Do not change.

  Default: 0x01F4 The value here yields a time of 7.68 secs for aBaseSuperframeDuration
  using the 2.4Ghz symbol rate
*/
#ifndef gNwkTransPersistenceTime_c
#define gNwkTransPersistenceTime_c              0x01F4
#endif

/* 
  For a Router, Max Neighbor Table Entries Supported

  Neighbors are parents, children and siblings. Only those within hearing range are 
  neighbors. If the network is dense (many nodes in hearing range), then a higher 
  neighbor value is useful.

  Stored in NVM. See NV_Data.c, paNvDataSetTable[][]. If increasing the size, make sure the
  data will still fit in the flash page.

  Range:   1 - 255
  Default: 25
*/
#ifndef gRouterNwkInfobaseMaxNeighborTableEntry_c
#define gRouterNwkInfobaseMaxNeighborTableEntry_c      25
#endif

/* 
  For a Coordinator, Max Neighbor Table Entries Supported

  Neighbors are parents, children and siblings. Only those within hearing range are 
  neighbors. If the network is dense (many nodes in hearing range), then a higher 
  neighbor value is useful.

  Stored in NVM. See NV_Data.c, paNvDataSetTable[][]. If increasing the size, make sure the
  data will still fit in the flash page.

  Range:   1 - 255
  Default: 24
*/
#ifndef gCoordinatorNwkInfobaseMaxNeighborTableEntry_c
#define gCoordinatorNwkInfobaseMaxNeighborTableEntry_c  25
#endif

/*
  For a ZED, Max Neighbor Table Entries Supported

  Neighbors for a ZED are potential parents. Only those within hearing range are 
  neighbors. If the network is dense (many nodes in hearing range), then a higher 
  neighbor value is useful.

  Stored in NVM. See NV_Data.c, paNvDataSetTable[][]. If increasing the size, make sure the
  data will still fit in the flash page.
  
  Default: 6
*/
#ifndef gEndDevNwkInfobaseMaxNeighborTableEntry_c
#define gEndDevNwkInfobaseMaxNeighborTableEntry_c       2
#endif

/*
  How many active routes can this node sustain? Includes both routes originated from this 
  node and routes passing through this node. Routes to the same destination node count as
  one route.

  Stored in NVM. See NV_Data.c, paNvDataSetTable[][]. If increasing the size, make sure the
  data will still fit in the flash page.

  Range:   1 - 255
  Default: 10 (For SP2)
*/
#ifndef gNwkInfobaseMaxRouteTableEntry_c
#define gNwkInfobaseMaxRouteTableEntry_c              10
#endif

/*
  How many simultaneous route discoveries can this node support?


  Range:   1 - 255
  Default: 8
*/
#ifndef gNwkRoutingMaxRouteDiscoveyTableEntry_c
#define gNwkRoutingMaxRouteDiscoveyTableEntry_c         4
#endif

/* 
  How many route record entries can this node sustain?.  
  The set of paths that represent the route in order from the concentrator to the
  destination address.
*/

#ifndef gNwkInfobaseMaxSourceRouteTableEntry_c
#define gNwkInfobaseMaxSourceRouteTableEntry_c          4
#endif

/* 
  The maximun number of hops in a source route.

  Range:   0x00 - 0xff
  Default: 0x0C  
*/

#ifndef gNwkMaxHopsInSourceRoute_c
#define gNwkMaxHopsInSourceRoute_c           8
#endif

/*
  Can this node be a concentrator (aka gateway)? Only available on Stack Profile 0x02 
  (ZigBee Pro).

  Default: FALSE
*/
#ifndef gConcentratorFlag_d
#define gConcentratorFlag_d                       TRUE
#endif

/*
  Allows the storing/retrieving source routes. If disabled, the concentrator 
  does not need a table and the store/retrieve functions for source routes in 
  AppStackImpl.c can be disabled
  
  Default: FALSE
  Range: bool
*/
#ifndef gNwkHighRamConcentrator_d
#define gNwkHighRamConcentrator_d                 FALSE
#endif

/*
  Max Handle Tracking Table Entries

  How many simultaneous messages from the higher layer can the NWK layer support? 
  Includes ZDP commands, APS commands and data.

  Range:   1 - 255
  Default: 10
*/
#ifndef gHandleTrackingTableSize_c
#define gHandleTrackingTableSize_c                      10
#endif


/*
  Max Packets On Hold Table Entries

  How many simultaneous messages the network layer can hold until they get routed. 

  Range:   1 - 255
  Default: 2
*/
#ifndef gPacketsOnHoldTableSize_c
#define gPacketsOnHoldTableSize_c                       2
#endif



/* 
  Max number of indirect messages that can be in the HTT

  How many messages will be sent to the MAC for polling ZigBee End-Devices? Do not make 
  this larger than the amount of big buffers - 3.

  Range: 1 - 255
  Default: 4
*/
#ifndef gHttMaxIndirectEntries_c
#define gHttMaxIndirectEntries_c                        2
#endif

/* 
  Maximum children each router (including ZC) can keep track of. To determine 
  number of end-devices, subtract MaxRouters.

  Default 20 (0x14) in stack profile 1
*/
#ifndef gNwkMaximumChildren_c
#define gNwkMaximumChildren_c     20
#endif

/*
  Maximum depth from the ZigBee Coordinator (ZC) in number of hops. This also 
  limits the default network diameter to 2 * maxDepth.

  Default 15 in stack profile 2
*/
#ifndef gNwkMaximumDepth_c
#define gNwkMaximumDepth_c        15
#endif

/*
  Maximum number of routers each router (including ZC) can keep track of. 
  
  Default 6 in Stack Profile 1
*/
#ifndef gNwkMaximumRouters_c
#define gNwkMaximumRouters_c      6
#endif

/* 

  Number of transmission failures before TxFailureReport is issued to the next higher 
  layer.
  
  Used for the Frequency Agility.

  Range:   0x01 - 0xff
  Default: 0x14
*/
#ifndef gMaxNumberOfTxAttempts_c
#define gMaxNumberOfTxAttempts_c  20
#endif

/* 
  Default Transmit Failure Counter, indicates how many times have to fail a 
  transmition to a specific device before a route repair is initiated, or a ZED rejoins.

  Range:   0x01 - 0xff
  Default: 0x02
*/
#ifndef gMaxNwkLinkRetryThreshold_c
#define gMaxNwkLinkRetryThreshold_c 2
#endif

/*
  It is allowed in the ZigBee spec to form the same PAN ID, as long as the extended
  PAN ID is different. Disallow this by default.

  Default: FALSE
*/
#ifndef gSamePanIdOk_c
#define gSamePanIdOk_c  FALSE
#endif

/* 
  Enables the "I can hear you" table, which allows nodes to artificially ignore incoming 
  packets from nodes not in their list. Allows for ZigBee route discovery in a table-top 
  scenario such as a trade show or test bench. See "gaICanHearYouTable" in 
  BeeStackGlobals.c or set through the ZigBee Test Client (gZtcIncluded_d in ZtcInterface.h).

  Default: TRUE  
*/
#ifndef gICanHearYouTableCapability_d
#define gICanHearYouTableCapability_d   FALSE
#endif

/* 
  Limits which nodes the NWK layer can hear. Use to analyze multi-hop behavior 
  of the network. Default 4.

  Note: does not affect the MAC layer, only NWK layer and higher.

  Range:   1 - 255
  Default: 4
*/
#ifndef gDefaultValueOfMaxEntriesForICanHearYouTable_c
#define gDefaultValueOfMaxEntriesForICanHearYouTable_c  4
#endif

/* 
  Default # of entries in exclusion table. Used to exclude nodes in a secure network.

  Range:   1 - 255
  Default: 1
*/
#ifndef gDefaultValueOfMaxEntriesForExclusionTable_c
#define gDefaultValueOfMaxEntriesForExclusionTable_c    1
#endif

/*
  Defines how big is goin tobe the buffer to be used on the Nwk rejoin response state machine.
  the state machine uses one buffer to control all the entries, each entries represent a
  uinque set on the state machine, each entry is as big as the size of nwkRejoinEntry_t.

  Range:    2 - 127 bytes.
  default:  As big as a small buffer.
*/
#ifndef gNwkRejoinStateMachineSize_c
#define gNwkRejoinStateMachineSize_c  (sizeof(block1sizeOrigin_t))
#endif

/*
  Define the amount of entries possible to handle in the rejoin response state machine, it
  will depend on the size of the buffer to beused in the state machine.
  The default size for the buffer is sizeof(block1sizeOrigin_t), which is ~30bytes for the
  Qe128 and ~34 bytes for the kaibab, and the sizeof each entry on the state machine is
  sizeof(nwkRejoinEntry_t), which is 2 bytes, this gives you a state machinie capable of
  handling up to ~15 rejoin responses at the same time.

  Range:   1 - ~63. Depends on the size of the buffer.
  Default: (gNwkRejoinStateMachineSize_c / sizeof(nwkRejoinEntry_t))
*/
#ifndef gNwkRejoinStateMachineAmountOfEntries_c
#define gNwkRejoinStateMachineAmountOfEntries_c  (gNwkRejoinStateMachineSize_c / sizeof(nwkRejoinEntry_t))
#endif

/*
  Default amount of time to before expiring the rejoin response state amchine and all its
  contents. This time defines the amount of time that the last rejoin reposne arriving into
  the state will have until the confrim of it gets into the stack.
  This time should be something reasonable, a shor time will make the that the state machine
  don't process any rejoin response and a long time will make the state machine to keep the
  buffer allocated for too long.

  Range:    1000 - 65535ms
  Default:  2000ms
*/
#ifndef gNwkRejoinExpirationTime_c
#define gNwkRejoinExpirationTime_c  2000
#endif


/*--------------------------- For APS layer ---------------------------------*/

/* 
  APS Security (Optional Stack profile 0x01 and 0x02)

  Default: FALSE
*/
#ifndef gApsLinkKeySecurity_d
#define gApsLinkKeySecurity_d  FALSE
#endif

/*
  APS SKKE activated or not, the use of SKKE help to establish link keys between the nodes
  through over the iar commands (Optional Stack profile 0x01 and 0x02).

  Default: FALSE
*/
#ifndef gSKKESupported_d
#define gSKKESupported_d  FALSE
#endif

/*
  Is binding capability enabled? Set to FALSE if not using Binding. Set to TRUE otherwise.

  Default: TRUE
*/
#ifndef gBindCapability_d
#define gBindCapability_d  TRUE
#endif

/*
  Does APS Support ACK (acknowledgements)? Always set to TRUE for all stack profiles. 
  Changing this will make the stack non-ZigBee compliant.

  Default: TRUE
*/
#ifndef gApsAckCapability_d
#define gApsAckCapability_d  TRUE
#endif

/*
  Frequency Agility Capability
  
  This Flag determine if the frequency agility functionality is included or not.The Zdp commands 
  for the frequency agility should also be included:
  Mgmt_NWK_Update_notify/req

  Default: TRUE
*/
#ifndef gFrequencyAgilityCapability_d
#define gFrequencyAgilityCapability_d  FALSE
#endif

/*
  Network Manager Capability
  
  This Flag determine if the Node is a NwkManager or not , the default is FALSE 
  so that the user needs to added in the compile line.
  Default: FALSE  
  IMPORTANT: This capability is only mandatory for ZC so it is se to TRUE for 
  that target in BeeStackCalculation.h
*/

#ifndef gNetworkManagerCapability_d
#define gNetworkManagerCapability_d  FALSE
#endif

/*
  Fragmentation Capability

  Fragmentation allows the use of AF_DataRequestFragmented(), which can send acknowledged 
  packets longer than will fit in a single packet. Up to 255 fragments may be sent, each
  of which can be up to 80 bytes long, depending on ApsmeGetMaxAsduLength().

  Default: FALSE
*/
#ifndef gFragmentationCapability_d
#define gFragmentationCapability_d  FALSE
#endif

/*
  Conflict Resolution Capabilty
  
  Conflict Resolution allows to the device to detect a Pan Id conflict with other networks
  in  the range and resolve it sending a network report to the network manager to generate 
  a new Pan Id and update it in all devices present in the network.

  Note: This capability only controls the detection part, the Nwk Manager will still be able
  react or resolve the conflict if a report is recevied. In other words, this configuration
  option only works on application level and to control the detection of a PanId Conflict,
  not the resolution of it.

  Default: TRUE
*/
#ifndef gConflictResolutionEnabled_d
#define gConflictResolutionEnabled_d TRUE
#endif

/*
  053474r17 ZigBee Spec.
  The time in seconds between link status command frames. 
  Default 0x0f
  Range 0x00-0xff
*/
#ifndef gNetworkLinkStatusPeriod_c
#define gNetworkLinkStatusPeriod_c  15
#endif

/*
  053474r17 ZigBee Spec.
  This variable is used as limit in the routers aging for SP2. The number of missed link 
  status command frames before resetting the link costs to zero. In this case, the neighbor 
  entry is considered stale and may be reused if necessary to make room for a new neighbor.

  If nwkAddrAlloc has a value of 0x00, neighbor table entries for relatives should
  not be considered stale and reused.

  Default 0x03
  Range 0x00-0xff
*/
#ifndef gNetworkRouterAgeLimit_c 
#define gNetworkRouterAgeLimit_c   3
#endif

/*
  Time (in ms) between split link status frames if they need to span multiple packets due 
  to a large number of neighbors.

  Range: 100 - 300
  Default 100
*/
#ifndef gNwkLinkStatusMultipleFramesPeriod_c
#define gNwkLinkStatusMultipleFramesPeriod_c  100
#endif

/*
  Address map entries are used for binding to nodes that are not children or
  the parent of a node. The address map correlates the IEEE (aka 64-bit or MAC)
  address with the 16-bit NWK address for a node. Default 5.

  The address map is stored in NVM. See gAPS_DATA_SET_FOR_NVM in BeeStackParameters.h. 
  See also paNvDataSetTable[] in NV_Data.c.

  Note: APS link keys (if enabled) need at least one address map entry per link key.
  See also 

  Range:   1 - 255
  Default: 9
*/
#ifndef gApsMaxAddrMapEntries_c
#define gApsMaxAddrMapEntries_c            9
#endif

/*
  Link keys may be used to secure data in the APS payload (in addition to data at the NWK 
  payload). Link keys should be used if the data is sensitive and may not be known by 
  other nodes in the network (for example, medical data).

  Each link key requires 26 bytes of memory, and if the # of link keys grows beyond 5, then
  the NVM storage for link keys must be adjusted. See NV_Data.c.

  There must also be enough address map entries for each link key. See also
  gApsMaxAddrMapEntries_c.

  Range:   1 - Maximum number of entries in the Neighbor table.
  Default: 9
*/
#ifndef gApsMaxLinkKeys_c
#define gApsMaxLinkKeys_c                   9
#endif

/* See description above. */
#ifndef gApsMaxSecureMaterialEntries_c
#define gApsMaxSecureMaterialEntries_c     9
#endif

/*
  Number of groups in the group table. Groups are used to filter group or multi-cast 
  messages. If a message is sent in group mode, APS checks if each endpoint is a member 
  of that group and only sends the message to those endpoint that belong to the group. 
  Default 16.

  Groups are stored in NVM. See gAPS_DATA_SET_FOR_NVM in BeeStackParameters.h. See also 
  paNvDataSetTable[] in NV_Data.c.

  Range:   1 - 255
  Default: 5
*/
#ifndef gApsMaxGroups_c
#define gApsMaxGroups_c                    5
#endif

/*
  Every node is capable for source binding. Set this to 0 to remove binding
  from BeeStack.

  The binding table is stored in NVM. See gAPS_DATA_SET_FOR_NVM in BeeStackParameters.h. 
  See also paNvDataSetTable[][] in NV_Data.c.

  Range:   1 - 255
  Default: 5
*/
#ifndef gMaximumApsBindingTableEntries_c
#define gMaximumApsBindingTableEntries_c        5
#endif

/*
  Each entry on the binding table must have a list of cluster to be bound to.

  The Binding table and the list of clusters bound to, are allocate inton NVM, any change
  to the amount of clusters or binding entries will afect directly the NVM pages.

  Default: 5
*/
#ifndef gMaximumApsBindingTableClusters_c
#define gMaximumApsBindingTableClusters_c        5
#endif


/*
  Table used to keep track of previously heard APS frames so they are only sent
  to the application once (prevents duplicates). Default: 6

  Range:   1 - 255
  Default: 6
*/
#ifndef gApscMinDuplicationRejectionTableSize_c
#define gApscMinDuplicationRejectionTableSize_c   6
#endif

/*
  Set the maximum # of retries APS ACK uses (normally 3... for a total of 4 tries).

  Range:   1 - 7
  Default: 3
*/
#ifndef gApsMaxRetries_c
#define gApsMaxRetries_c    3
#endif

/*
  Adjust this wait (in ms) duration to be large enough for the expected delivery time
  for a packet, including route discover if needed.

  Formulas given by the ZigBee Spec are:
  (500 * ( 2 * 5 ) + 100)  // 5.1 seconds between tries for secure network
  (500 + ( 2 * 5 ) * 100)  // 1.5 seconds between tries
  
  Default to 1.8 seconds, as this allows enough time for route discover and reply for SP1
  but for PRO (using symmetrical links) is 2.5+ seconds.

  Range:   0 - 0x3ffff (4 minutes)
  Default: 1800 (1.8 seconds)
  
*/
#ifndef gApsAckWaitDuration_c
#define gApsAckWaitDuration_c  1800
#endif

/*
  Wait (in ms) duration for the expected delivery time of the nwk address response 
  needed to complete the address map.
  Works for both resolving 64-bit destination addresses and incomplete (short address 
  unknown) bindings.

  Range:   0 - 0x3ffff (4 minutes)
  Default: 1500 (1.5 seconds)
*/
#ifndef gAps64BitAddressResolutionTimeout_c
#define gAps64BitAddressResolutionTimeout_c   1500
#endif

/*
  Determines the # of simultaneous messages APS can handle from higher layers (both ZDP 
  and application). If this limit is reached, the system indicates busy (gZbBusy_c).

  If increasing this value, also increase the number of timers in TMR_Interface.h.

  Range:   1 - 7
  Default: 2
*/
#ifndef gApsMaxDataHandlingCapacity_c
#define gApsMaxDataHandlingCapacity_c           2
#endif

/*
  Determines the # of simultaneous Rx fragmented messages APS can handle.
  If this limit is reached, the system indicates busy (gZbBusy_c).

  If increasing this value, also increase the number of timers in TMR_Interface.h.

  Range:   1 - 7
  Default: 2
*/
#ifndef gApsRxFragmentationCapacity_c
#define gApsRxFragmentationCapacity_c           2
#endif

/*
   Determines the # of simultaneous SKKE processes can be handle at the same 
   time with deferents devices.

  Range:   1 - as many big buffers are available after the joining process.
  Default: 5
*/
#ifndef gDefaulEntriesInSKKEStateMachine_c
#define gDefaulEntriesInSKKEStateMachine_c  5
#endif

/*
   Determines the # of simultaneous Entity Authentication can be handle at the
  same time with deferents devices.

  Range:   1 - as many big buffers are available after the joining process.
  Default: 3
*/
#ifndef gDefaultEntriesInEAStateMachine_c
#define gDefaultEntriesInEAStateMachine_c  3
#endif


/*--------------------------- For AF layer ----------------------------------*/

/*
  The number of application endpoints supported by BeeStack.

  Range: 1 - 240
  Default: 5
*/
#ifndef gNumberOfEndPoints_c
#define gNumberOfEndPoints_c                5
#endif


/*--------------------------- For SSP ---------------------------------------*/
/*********************Residential Stack Profile Only *************************/



/*****************************************************************************/



/*--------------------------- ZDO layer Definations -------------------------*/

/*
  Scan Duration. A value used to calculate the length of time to spend scanning each 
  channel. The time spent scanning each channel is (aBaseSuperframeDuration * (2n + 1)) 
  symbols, where n is the value of the ScanDuration parameter. (IEEE 802.15.4 2003 spec)

  Formula                                                                       symbols      ms
  Symbol: 62.5k symbols/sec                                                      62500       0.02
  aBaseSuperframeDuration: aBaseSlotDuration(60) * aNumSuperframeSlots(16)                  15.36

  Scan value  Duration ms   Duration sec
      0           31           0.03
      1           46           0.05
      2           77           0.08
      3           138          0.14
      4           261          0.26
      5           507          0.51
      6           998          1
      7           1,981        1.98
      8           3,948        3.95
      9           7,880        7.88
     10           15,744       15.74
     11           31,473       31.47
     12           62,930       62.93
     13           125,844      125.84
     14           251,674      251.67

  Range: 1 - 7
  Default: 3
*/
#ifndef gScanDuration_c
#define gScanDuration_c  3
#endif

/*gZDPNwkSec_d has effect only if link keys are used.
  If TRUE, ZDP commands will be sent with Tx options 0x02(nwk security).
  If FALSE, ZDP commands will be sent with Tx options 0x01(aps security).*/
#ifndef gZDPNwkSec_d
#define gZDPNwkSec_d  TRUE
#endif
/* These options must match the ZigBee Specification, not BeeStack app needs */
/* see 053474r17 (ZigBee2007, ZigBeePro specification) */

/*Compiler time option to compile out Network Address Request*/
#ifndef gNWK_addr_req_d
#define gNWK_addr_req_d  TRUE
#endif

#ifndef gNWK_addr_rsp_d  /* The command is Mandatory on the server side */
#define gNWK_addr_rsp_d  TRUE
#endif

/*Compiler time option to compile out IEEE Address Request*/
#ifndef gIEEE_addr_req_d
#define gIEEE_addr_req_d  TRUE
#endif

#ifndef gIEEE_addr_rsp_d  /* The command is Mandatory on the server side */
#define gIEEE_addr_rsp_d  TRUE
#endif

/*Compiler time option to compile out Node Descriptor Request*/
#ifndef gNode_Desc_req_d
#define gNode_Desc_req_d  TRUE
#endif

#ifndef gNode_Desc_rsp_d  /* The command is Mandatory on the server side */
#define gNode_Desc_rsp_d  TRUE
#endif

/*Compiler time option to compile out Power Descriptor Request*/
#ifndef gPower_Desc_req_d
#define gPower_Desc_req_d  FALSE
#endif

#ifndef gPower_Desc_rsp_d  /* The command is Mandatory on the server side */
#define gPower_Desc_rsp_d  TRUE
#endif

/*Compiler time option to compile out Simple Descriptor Request*/
#ifndef gSimple_Desc_req_d
#define gSimple_Desc_req_d  FALSE
#endif

#ifndef gSimple_Desc_rsp_d  /* The command is Mandatory on the server side */
#define gSimple_Desc_rsp_d  TRUE
#endif

/*Compiler time option to compile out Extended Simple Descriptor Request*/
#ifndef gExtended_Simple_Desc_req_d
#define gExtended_Simple_Desc_req_d  FALSE
#endif

/* Compiler time option to compile out Extended Simple Desc response */
#ifndef gExtended_Simple_Desc_rsp_d  
#define gExtended_Simple_Desc_rsp_d  FALSE
#endif
/*Compiler time option to compile out Active End Point Request*/
#ifndef gActive_EP_req_d
#define gActive_EP_req_d  FALSE
#endif

#ifndef gActive_EP_rsp_d  /* The command is Mandatory on the server side */
#define gActive_EP_rsp_d  TRUE
#endif

/*Compiler time option to compile out Extended Active End Point Request*/
#ifndef gExtended_Active_EP_req_d
#define gExtended_Active_EP_req_d  FALSE
#endif

#ifndef gExtended_Active_EP_rsp_d 
#define gExtended_Active_EP_rsp_d  FALSE
#endif

/*Compiler time option to compile out Match Descriptor Request*/
#ifndef gMatch_Desc_req_d
#define gMatch_Desc_req_d  TRUE
#endif

#ifndef gMatch_Desc_rsp_d  /* The command is Mandatory on the server side */
#define gMatch_Desc_rsp_d  TRUE
#endif

/* Complex Discriptor Request Supported */
#ifndef gComplex_Desc_req_d
#define gComplex_Desc_req_d  FALSE
#endif

#ifndef gComplex_Desc_rsp_d
#define gComplex_Desc_rsp_d  FALSE
#endif

/* User Discriptor Request Supported */
#ifndef gUser_Desc_req_d
#define gUser_Desc_req_d  FALSE
#endif

#ifndef gUser_Desc_rsp_d
#define gUser_Desc_rsp_d  FALSE
#endif

#ifndef  gDiscovery_Cache_req_d
#define gDiscovery_Cache_req_d  FALSE
#endif

#ifndef gDiscovery_Cache_rsp_d
#define gDiscovery_Cache_rsp_d  FALSE
#endif

/* End Device Announce Supported. End device annnounce is always able 
   because it is mandatory in BeeStack 2007 SP1  */
#ifndef gDevice_annce_d
#define gDevice_annce_d  TRUE
#endif

/* User Discriptor Set Supported */
#ifndef gUser_Desc_set_d
#define gUser_Desc_set_d  FALSE
#endif

#ifndef gUser_Desc_conf_d
#define gUser_Desc_conf_d  FALSE
#endif

/* System Server Discovery Supported */
#ifndef gSystem_Server_Discovery_req_d
#define gSystem_Server_Discovery_req_d  FALSE
#endif

#ifndef gSystem_Server_Discovery_rsp_d
#define gSystem_Server_Discovery_rsp_d  FALSE
#endif

#ifndef gDiscovery_store_req_d
#define gDiscovery_store_req_d  FALSE
#endif

#ifndef gDiscovery_store_rsp_d
#define gDiscovery_store_rsp_d  FALSE
#endif

#ifndef  gNode_Desc_store_req_d
#define gNode_Desc_store_req_d  FALSE
#endif

#ifndef gNode_Desc_store_rsp_d
#define gNode_Desc_store_rsp_d  FALSE
#endif

#ifndef gPower_Desc_store_req_d
#define gPower_Desc_store_req_d  FALSE
#endif

#ifndef gPower_Desc_store_rsp_d
#define gPower_Desc_store_rsp_d  FALSE
#endif

#ifndef gActive_EP_store_req_d
#define gActive_EP_store_req_d  FALSE
#endif

#ifndef gActive_EP_store_rsp_d
#define gActive_EP_store_rsp_d  FALSE
#endif

#ifndef gSimple_Desc_store_req_d
#define gSimple_Desc_store_req_d  FALSE
#endif

#ifndef gSimple_Desc_store_rsp_d
#define gSimple_Desc_store_rsp_d  FALSE
#endif

#ifndef gRemove_node_cache_req_d
#define gRemove_node_cache_req_d  FALSE
#endif

#ifndef gRemove_node_cache_rsp_d
#define gRemove_node_cache_rsp_d  FALSE
#endif

#ifndef gFind_node_cache_req_d
#define gFind_node_cache_req_d  FALSE
#endif

#ifndef gFind_node_cache_rsp_d
#define gFind_node_cache_rsp_d  FALSE
#endif

/*
End Device Bind, Bind, Unbind and Bind Management Client Services Primitives
*/
/*End Device Bind Request Enabled*/
#ifndef gEnd_Device_Bind_req_d
#define gEnd_Device_Bind_req_d      FALSE
#endif

#ifndef gEnd_Device_Bind_rsp_d
#define gEnd_Device_Bind_rsp_d      FALSE
#endif

/*Bind Request Enabled*/
#ifndef gBind_req_d
#define gBind_req_d                TRUE
#endif

#ifndef gBind_rsp_d
#define gBind_rsp_d                TRUE
#endif

/*UnBind Request Enabled*/
#ifndef gUnbind_req_d
#define gUnbind_req_d              TRUE
#endif

#ifndef gUnbind_rsp_d
#define gUnbind_rsp_d              TRUE
#endif

#ifndef gBind_Register_req_d
#define gBind_Register_req_d         FALSE
#endif

#ifndef gBind_Register_rsp_d
#define gBind_Register_rsp_d         FALSE
#endif

#ifndef gReplace_Device_req_d
#define gReplace_Device_req_d        FALSE
#endif

#ifndef gReplace_Device_rsp_d
#define gReplace_Device_rsp_d        FALSE
#endif

#ifndef gStore_Bkup_Bind_Entry_req_d
#define gStore_Bkup_Bind_Entry_req_d  FALSE
#endif

#ifndef gStore_Bkup_Bind_Entry_rsp_d
#define gStore_Bkup_Bind_Entry_rsp_d  FALSE
#endif

#ifndef gRemove_Bkup_Bind_Entry_req_d
#define gRemove_Bkup_Bind_Entry_req_d  FALSE
#endif

#ifndef gRemove_Bkup_Bind_Entry_rsp_d
#define gRemove_Bkup_Bind_Entry_rsp_d  FALSE
#endif

#ifndef gBackup_Bind_Table_req_d
#define gBackup_Bind_Table_req_d      FALSE
#endif

#ifndef gBackup_Bind_Table_rsp_d
#define gBackup_Bind_Table_rsp_d      FALSE
#endif

#ifndef gRecover_Bind_Table_req_d
#define gRecover_Bind_Table_req_d     FALSE
#endif

#ifndef gRecover_Bind_Table_rsp_d
#define gRecover_Bind_Table_rsp_d     FALSE
#endif

#ifndef gBackup_Source_Bind_req_d
#define gBackup_Source_Bind_req_d     FALSE
#endif

#ifndef gBackup_Source_Bind_rsp_d
#define gBackup_Source_Bind_rsp_d     FALSE
#endif

#ifndef gRecover_Source_Bind_req_d
#define gRecover_Source_Bind_req_d    FALSE
#endif

#ifndef gRecover_Source_Bind_rsp_d
#define gRecover_Source_Bind_rsp_d    FALSE
#endif

#ifndef gMgmt_NWK_Disc_req_d
#define gMgmt_NWK_Disc_req_d  FALSE
#endif

#ifndef gMgmt_NWK_Disc_rsp_d
#define gMgmt_NWK_Disc_rsp_d  FALSE
#endif

#ifndef gMgmt_Lqi_req_d
#define gMgmt_Lqi_req_d  TRUE
#endif

#ifndef gMgmt_Lqi_rsp_d
#define gMgmt_Lqi_rsp_d  TRUE
#endif

#ifndef gMgmt_Rtg_req_d
#define gMgmt_Rtg_req_d  FALSE
#endif

#ifndef gMgmt_Rtg_rsp_d
#define gMgmt_Rtg_rsp_d  FALSE
#endif

/* Mangment Bind Request Supported */
#ifndef gMgmt_Bind_req_d
#define gMgmt_Bind_req_d  FALSE
#endif

#ifndef gMgmt_Bind_rsp_d
#define gMgmt_Bind_rsp_d  FALSE
#endif

/* Mangment Leave Request Supported */
#ifndef gMgmt_Leave_req_d
#define gMgmt_Leave_req_d  FALSE
#endif

#ifndef gMgmt_Leave_rsp_d
#define gMgmt_Leave_rsp_d  FALSE
#endif

/* Mangment Direct Join Request Supported */
#ifndef gMgmt_Direct_Join_req_d
#define gMgmt_Direct_Join_req_d  FALSE
#endif

#ifndef gMgmt_Direct_Join_rsp_d
#define gMgmt_Direct_Join_rsp_d  FALSE
#endif

#ifndef gMgmt_Permit_Joining_req_d
#define gMgmt_Permit_Joining_req_d    TRUE
#endif

#ifndef gMgmt_Permit_Joining_rsp_d
#define gMgmt_Permit_Joining_rsp_d    TRUE
#endif

#ifndef  gMgmt_Cache_req_d
#define  gMgmt_Cache_req_d      FALSE
#endif

#ifndef  gMgmt_Cache_rsp_d
#define  gMgmt_Cache_rsp_d      FALSE
#endif

/* 053474r17ZB - Sec. 2.4.3.3.9 Mgmt_NWK_Update_req */
#ifndef  gMgmt_NWK_Update_req_d
#define  gMgmt_NWK_Update_req_d  FALSE
#endif

/* 053474r17ZB - Sec. 2.4.4.3.9 Mgmt_NWK_Update_notify */
#ifndef  gMgmt_NWK_Update_notify_d
#define  gMgmt_NWK_Update_notify_d  FALSE
#endif

/* tell application about ZDO events */
#ifndef gSystemEventEnabled_d
#define gSystemEventEnabled_d       TRUE
#endif

/* Default Backup Discovery Cache Capability */
#ifndef gBkup_Discovery_cache_d
#define gBkup_Discovery_cache_d     FALSE
#endif


/*
  Energy Threshhold used when performing ED Scan. 0xFF means all noise levels are accepted

  Range:   0x01 - 0xff
  Default: 0xff
*/
#ifndef gNwkEnergyLevelThresHold_d
#define gNwkEnergyLevelThresHold_d       0xFF
#endif 

#endif /* _BEESTACK_CONFIGURATION_H_ */


/*This file does the appropiate calculations for some variants depending on the
selected values on BeeStackConfiguration.h and ApplicationConf.h*/
#include "BeeStackCalculations.h"

