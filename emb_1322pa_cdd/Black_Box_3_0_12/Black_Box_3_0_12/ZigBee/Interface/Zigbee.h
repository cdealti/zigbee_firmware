/******************************************************************************
* This module contains zigbee type definitions. Any structure that goes over
* the air, or is used as interface parameters.
*
* No implementation specific structures are defined here.
*
* Copyright (c) 2008, Freescale, Inc.  All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
* [R1] - ZigBee Specification 053474r11
*
******************************************************************************/
#ifndef _ZIGBEE_H_
#define _ZIGBEE_H_

/******************************************
 * Type definitions
 ******************************************/

typedef uint8_t   zbPanId_t[2];       /* defines a single ZigBee network (part of 802.15.4) */
typedef uint8_t   zbIeeeAddr_t[8];    /* long address. Also used for extended PAN ID */
typedef uint8_t   zbNwkAddr_t[2];     /* short address. All ZigBee data packets use short addr */
typedef uint8_t   zbClusterId_t[2];   /* APS level data. Determines ZDO and app commands */
typedef uint8_t   zbClusterId8_t;     /* ZDO */
typedef uint8_t   zbGroupId_t[2];     /* APS-level group addressing */
typedef uint8_t   zbProfileId_t[2];   /* Cluster IDs are defined within a profile */
typedef uint8_t   zbDeviceId_t[2];    /* Device IDs are defined within a profile */
typedef uint8_t   zbSceneId_t;
typedef uint8_t   zbChannelEnergy_t;  /* channel energy, vendor specific */
typedef uint32_t  zbTime_t;           /* Tx/Rx time stamp */
typedef uint8_t   zbUserDescriptor_t[16];
typedef uint8_t   zbServerMask_t[2];
typedef uint8_t   zbListElement_t;
typedef uint8_t   zbCounter_t;
typedef uint8_t   zbCounter16_t[2];
typedef uint8_t   zbIndex16_t[2];
typedef uint16_t  zbuCounter16_t;
typedef uint8_t   zbSize_t;
typedef uint8_t   zbIndex_t;
typedef uint8_t   zbEnergyValue_t;
typedef uint8_t   zbKeyTableIndex_t;
typedef uint8_t   zbMsgId_t;


/* address modes, from an APSDE-DATA.request standpoint. See ZB Spec Table 2.2 */
typedef uint8_t zbAddrMode_t;
#define gZbAddrModeIndirect_c     0x00  /* use binding table */
#define gZbAddrModeGroup_c        0x01  /* group addressing */
#define gZbAddrMode16Bit_c        0x02  /* direct 16-bit addressing */
#define gZbAddrMode64Bit_c        0x03  /* direct 64-bit addressing */

/* 
  APS Reset types (zbResetType_t):
  gApsmeResetSimpleAib_c 
    Pending messages in NLDE, APSDE and APSME Sap Handlers queues
    Aps Tx table
    Sets up APS default values
    Aps already heard it table
    Residential network key
    Aps counter
  gApsmeResetBindings_c 
    Binding Table
  gApsmeResetAddrMap_c
    Address Map
  gApsmeResetSecurity_c 
    Reset link keys and APS security material
  gApsmeResetGroups_c 
    Groups Table
  gApsmeResetAllAib_c  
    Everything listed before
*/
typedef uint8_t zbResetType_t;
#define gApsmeResetSimpleAib_c  (1<<0)
#define gApsmeResetBindings_c   (1<<1)
#define gApsmeResetAddrMap_c    (1<<2)
#define gApsmeResetSecurity_c   (1<<3)
#define gApsmeResetGroups_c     (1<<4)
#define gApsmeResetAllAib_c     (0xff)

/* valid group IDs range between 0x0001 - 0xfff7 */
#define gZbGroupIdLimitLow_c      0x0001
#define gZbGroupIdLimitHigh_c     0xfff7

/* 32 bit mask of posssible channels (only bits 11-26 valid). */
/* used for scanning a variety of channels on which to form or join */
typedef uint8_t zbChannels_t[4];  
#define gZbLowestChannel_c      11
#define gZbHighestChannel_c     26

/*--- Begin ZigBee Confirm Status Values -----------------------------------------------*/
typedef uint8_t zbStatus_t;

/* general status */
#define gZbSuccess_c                0x00 /* A request has been executed successfully */
#define gZbFailed_c                 0x01 /* general failure */
#define gZbNoMem_c                  0x02 /* not enough memory */
#define gZbTooLarge_c               0x03 /* size of payload too large */
#define gZbNotOnNetwork_c           0x04 /* command cannot be used at this time: not on network */
#define gZbPartialSuccess_c         0x05 /* indicates 1 or more success, 1 or more failure on indirect Tx */
#define gZbRejoinTimeOut_c          0x06     
#define gZbBrocastAlreadyExist_c    0x07
#define gZbMulticastMsgIsSending_c  0x08
#define gZbBusy_c                   0x09 /* cannot process request at this time (e.g data request is busy) */
/* 053474r17 Table 2.26 APS Sub-layer Status Values. */
#define gApsAsduTooLong_c           0xa0 /* A transmit request failed since the ASDU is too large and fragmentation is not supported. */
#define gApsDefragDeferred_c        0xa1 /* A received fragmented frame could not be defragmented at the current time. */
#define gApsDefragUnsupported_c     0xa2 /* A received fragmented frame could not be defragmented since the device does not support fragmentation. */
#define gApsIllegalRequest_c        0xa3 /* A parameter value was out of range. */
#define gApsInvalidBinding_c        0xa4 /* An APSME-UNBIND.request failed due to requested binding link not existing */
#define gApsInvalidGroup_c          0xa5 /* An APSME-REMOVE-GROUP.request has been issued with a not existing group identifier*/
#define gApsInvalidParameter_c      0xa6 /* A parameter value was invalid or out of range*/
#define gApsNoAck_c                 0xa7 /* An APSDE-DATA.request failed due to ack not received*/
#define gApsNoBoundDevice_c         0xa8 /* Data request with addr mode 0x00 failed since device has no boundings*/
#define gApsNoShortAddress_c        0xa9 /* Data request with addr mode 0x03 failed due to no short address in addr map*/
#define gApsNotSupported_c          0xaa /* The request feature (binding, fragmentation) is not supported */ 
#define gApsSecuredLinkKey_c        0xab /* An ASDU was received secured using a link key*/
#define gApsSecuredNwkKey_c         0xac /* An ASDU was received secured using a nwk key*/
#define gApsSecurityFail_c          0xad /* Security processing error during APSDE-DATA.request */
#define gApsTableFull_c             0xae /* BIND of ADD-GROUP request fail when bind or group tables are full */
#define gApsUnsecured_c             0xaf /* sdu received without security*/
#define gApsUnsupportedAttribute_c  0xb0 /* apsme get and set request were issued with unknown attribute id */

#define gZbInvalidBinding_c         0xa0 /* invalid binding */
#define gZbNoBoundDevice_c          0xa1 /* no bound device */
#define gZbSecurityFail_c           0xa2 /* security fail */
#define gZbNoAck_c                  0xa3 /* never recieved an ACK */
#define gZbInvalidEndpoint_c        0xa4 /* Endpoint must be between 0x01-0xf0 */
#define gZbNoMatch_c                0xa5 /* no matching group or binding */
#define gZbIllegalDevice_c          0xa7 /* illegal device */

/* 053474r17 Table 3.57 NWK Layer Status Values (Continued) */
#define gZbTableFull_c              0xb1 /* table full */
#define gZbInvalidParameter_c       0xc1 /* An invalid or out-of-range parameter has been passed to a primitive from the next higher layer */
#define gZbInvalidRequest_c         0xc2 /* The next higher layer has issued a request that is invalid or cannot be executed given the current state of the NWK layer */
#define gZbNotPermitted_c           0xc3 /* An NLME-JOIN.request has been disallowed STARTUP_FAILURE 0xc4 An NLME-NETWORK-FORMATION.request has failed to start a network */
#define gZbStartupFailure_c         0xc4
#define gZbAlreadyPresent_c         0xc5 /* A device with the address supplied to the NLMEDIRECT-JOIN.request is already present in the neighbor table of the device on which the NLMEDIRECT-JOIN.request was issued */
#define gZbSyncFailure_c            0xc6 /* Used to indicate that an NLME-SYNC.request has failed at the MAC layer NEIGHBOR_TABLE_FULL 0xc7 An NLME-JOIN-DIRECTLY.request has failed because there is no more room in the neighbor table */
#define gZbUnknownDevice_c          0xc8
#define gZbNeighborTableFull_c      0xc7 /* An NLME-JOIN-DIRECTLY.request has failed because there is no more room in the neighbor table. */
#define gZbUnsupportedAttribute_c   0xc9 /* An NLME-GET.request or NLME-SET.request has been issued with an unknown attribute identifier */
#define gZbNoNetworks               0xca /* An NLME-JOIN.request has been issued in an environment where no networks are detectable */
#define gZbLeaveUnconfirmed_c       0xcb /* A device failed to confirm its departure from the network */
#define gZbMaxFrmCntr_c             0xcc /* Security processing has been attempted on an outgoing frame, and has failed because the frame counter has reached its maximum value */
#define gZbNoKey_c                  0xcd /* Security processing has been attempted on an outgoing frame, and has failed because no key was available with which to process it */
#define gZbBadCcmOutput_c           0xce /* Security processing has been attempted on an outgoing frame, and has failed because security engine produced erroneous output */
#define gZbNoRoutingCapacity_c      0xcf /* An attempt to discover a route has failed due to a lack of routing table or discovery table capacity */
#define gZbRouteDiscoveryFailed_c   0xd0 /* An attempt to discover a route has failed due to a reason other than a lack of routing capacity */
#define gZbRouteError_c             0xd1 /* An NLDE-DATA.request has failed due to a routing failure on the sending device */
#define gZbBtTableFull_c            0xd2 /* An attempt to send a broadcast frame or member mode multicast has failed due to the fact that there is no room in the BTT */
#define gZbFrameNotBuffered_c       0xd3 /* A non-member mode multicast frame was discarded pending route discovery */
#define gZbSourceRouteExpided_c     0xdc /* Freescale unique. See Nlme-Network-Status.Indication */
#define gZbRouteExpided_c           0xdd /* Freescale unique. See Nlme-Network-Status.Indication */
#define gZbNeighborExpired_c        0xde /* Freescale Unique. See Nlme-Network-Status.Indication */
#define gZbDstNotAuthenticated_c    0xdf /* Report to APS that the destination devices is not authenticated. */
/* 053474r17 Table 2.137 ZDP Enumerations Description. */
/********** 0x01 - 0x7F RESERVED. **********/
#define gZdoInvalidRequestType_c  0x80 /* The supplied request type was invalid.*/
#define gZdoDeviceNotFound_c      0x81 /* The requested device did not exist on a device following a child descriptor request to a parent. */
#define gZdoInvalidEndPoint_c     0x82 /* The supplied endpoint was equal to 0x00 or between 0xf1 and 0xff. */
#define gZdoNotActive_c           0x83 /* The requested endpoint is not described by a simple descriptor. */
#define gZdoNotSupported_c        0x84 /* The requested optional feature is not supported on the target device. */
#define gZdoTimeOut_c             0x85 /* A timeout has occurred with the requested operation. */
#define gZdoNoMatch_c             0x86 /* The end device bind request was unsuccessful due to a failure to match any suitable clusters. */
/********** 0x87 RESERVED **********/
#define gZdoNoEntry_c             0x88 /* The unbind request was unsuccessful due to the coordinator or source device not having an entry in its binding table to unbind. */
#define gZdoNoDescriptor_c        0x89 /* A child descriptor was not available following a discovery request to a parent. */
#define gZdoInsufficientSpace_c   0x8a /* The device does not have storage space to support the requested operation */
#define gZdoNotPermited_c         0x8b /* The device is not in the proper state to support the requested operation */
#define gZdoTableFull_c           0x8c /* The device does not have table space to support the operation */
#define gZdpNotAuthorized_c       0x8d /* The permissions configuration table on the target indicates that the request is not authorized from this device. */
#define gZdoSetAddrMapFailure_c   0x8e  /* Couldn't add to the address map, Reserve. */

/*--- End ZigBee Confirm Status Values -------------------------------------------------*/


/******************************************************************************
* NWK Layer
******************************************************************************/

/* Network layer Constants ( r17 - Table 3.43 ) */
#define  gNibNwkCoordinatorCapability_c   0x01
#define  gNibNwkDiscoveryRetryLimit_c		  0x03
#define  gNibNwkMaxDepth_c							  0x07
#define  gNibNwkHeaderOverhead_c				  0x08
#define  gNibNwkRepairThreshold_c				  0x03
#define  gNibNwkWaitBeforeValidation_c    0x500
#define  gNibNwkRouteDiscoveryTime_c      0x2710
#define  gNibNwkMaxBroadcastJitter_c      0x40
#define  gNibNwkInitialRREQRetries_c	    0x03
#define  gNibNwkRREQRetries_c             0x02
#define  gNibNwkRREQRetryInterval_c       0x20 /* it is 0xfe in slots of 8 milisecs */
#define  gNibNwkMinRREQJitter_c	          0x01 /* deviation from spec */
#define  gNibNwkMaxRREQJitter_c           0x10 /* deviation from spec */

/* lookup type and IDs for NIB, AIB, etc.. (includes both ZigBee and BeeStack IDs) */
typedef uint8_t zbIBAttributeId_t;

/* Network IB Attributes. ( r17 - Table 3.44 ) */
#define  gNwkSequenceNumber_c									0x81	/* Read Only */
#define  gNwkPassiveAckTimeout_c							0x82	/* Read Only */
#define  gNwkMaxBroadcastRetries_c						0x83	/* Read Only */
#define  gNwkMaxChildren_c  									0x84	/* Read Only */
#define  gNwkMaxDepth_c												0x85	/* Read Only */
#define  gNwkMaxRouter_c											0x86	/* Read Only */
#define  gNwkNeighborTable_c 									0x87	/* Read & Write */
#define  gNwkNetworkBroadcastDeliveryTime_c		0x88	/* Read Only */
#define  gNwkReportConstantCost_c							0x89	/* Read Only */
#define  gNwkRouteDiscoveryRetriesPermitted_c	0x8A	/* Read Only */
#define  gNwkRouteTable_c 										0x8B	/* Read & Write */
#define  gNwkSymLink_c 												0x8E	/* Read Only */
#define  gNwkCapabilityInformation_c 					0x8F	/* Read & Write */
#define  gNwkAddrAlloc_c             					0x90	/* Read Only */
#define  gNwkUseTreeRouting_c	      					0x91	/* Read Only */
#define  gNwkManagerAddr_c			    					0x92	/* Zigbee Pro */
#define  gNwkMaxSourceRoute_c        					0x93	/* Zigbee Pro */
#define  gNwkUpdateId_c              					0x94	/* Zigbee Pro */
#define  gNwkTransactionPersistenceTime_c			0x95	/* Read Only */
#define  gNwkShortAddress_c										0x96	/* Read Only */
#define  gNwkStackProfile_c										0x97	/* Read Only */
#define  gNwkBroadcastTransactionTable_c			0x98	/* Read Only */
#define  gNwkGroupIDTable_c										0x99	/* Zigbee Pro */
#define  gNwkExtendedPanId_c									0x9A	/* Read Only */
#define  gNwkUseMulticast_c 									0x9B
#define  gNwkRouteRecordTable_c								0x9C
#define  gNwkIsConcentrator_c 								0x9D
#define  gNwkConcentratorRadius_c							0x9E
#define  gNwkConcentratorDiscoveryTime_c      0x9F
#define  gNwkSecurityLevel_c									0xA0
#define  gNwkSecurityMaterialSet_c						0xA1
#define  gNwkActiveKeySeqNumber_c							0xA2
#define  gNwkAllFresh_c												0xA3
#define  gNwkKeyType_c                        0xA4
#define  gNwkSecureAllFrames_c								0xA5
#define  gNwkPreconfiguredKey_c               0xAF  /* Read Only */
#define  gNwkLinkStatusPeriod_c								0xA6
#define  gNwkRouterAgeLimit_c 								0xA7
#define  gNwkUniqueAddr_c      								0xA8
#define  gNwknwkAddressMap_c   								0xA9
#define  gNwkTimeStamp_c 								      0x8C
#define  gNwkPanId_c           								0x80  /* Read & Write */
#define  gNwkTxTotal_c   								      0x8D
#define  gNwkIncomingFrameCounterSetLimit_c   0x8E  /* Read Only */

/* BeeStack specific attributes */
#define  gNwkTxTotalFailures_c					      0xD0
#define  gNwkIeeeAddress_c                    0xD1  /* Read & Write */
#define  gParentShortAddress_c                0xD2
#define  gNwkLogicalChannel_c                 0xD3
#define  gNwkPermitJoiningFlag_c              0xD4
#define  gNwkDeviceDepth_c                    0xD6
#define  gNwkSecurityMaterialSetCurrrent_c    0xD7
#define  gNwkIndirectPollRate_c               0xD8
#define  gConfirmationPollTimeOut_c           0xD9
#define  gDevType_c                           0xDA  /* Read Only */
#define  gSoftwareVersion_c                   0xDB  /* Read Only */
#define  gNwkProtocolVersion_c                0xDC  /* Read Only */
#define  gNoOfDevInRouteTable_c               0xDD  /* Read Only */
#define  gParentLongAddress_c                 0xDE
#define  gAuthenticationPollTimeOut_c         0xDF  /* Read & Write */
#define  gNwkState_c                          0xE0
#define  gNwkScanAttempts_c                   0xE1  
#define  gNwkTimeBtwnScans_c                  0xE2  /* see Table 2.148 Config_NWK_Time_Btwn_Scans */
#define  gRejoinInterval_c                    0xE3
#define  gMaxRejoinInterval_c                 0xE4
#define  gNwkLinkRetryThreshold_c             0xE5
#define  gSASNwkKey_c                         0xE6  /* Read & Write */
#define  gSASTrustCenterMasterKey_c           0xE7  /* Read & Write */
#define  gSASTrustCenterLinkKey_c             0xE8  /* Read & Write */
#define  gNwkUseInsecureJoin_c                0xF0

/* AIB Security Attributes ( r17 - Table 4.36 ) */
/* The current set of 64 bit IEEE to 16 bit NWK address maps */
#define  gApsDeviceKeyPairSet_c               0xaa
#define  gApsTrustCenterAddress_c             0xab
#define  gApsSecurityTimeOutPeriod_c          0xac
#define  gApsDeviceKeyPairCount_c             0xad
#define  gApsTrustCenterNwkAddress_c          0xaf  /* unique to FS AIB */
#define  gApsTrustCenterMasterKey_c           0xb0  /* unique to FS AIB */
#define  gApsTrustCenterLinkKey_c             0xb1  /* unique to FS AIB */
#define  gApsDefaultTCKeyType_c               0xb2  /* unique to FS AIB */

/* AIB Attributes ( r17 - Table 2.24 ) */
#define gApsAddressMap_c       				        0xc0  /* unique to FS AIB */
#define gApsBindingTable_c     				        0xc1
#define gApsDesignatedCoordinator_c           0xc2
#define gApsChannelMask_c                     0xc3
#define gApsUseExtendedPANID_c                0xc4
#define gApsGroupTable_c       				        0xc5
#define gApsNonmemberRadius_c                 0xc6
#define gApsUseInsecureJoin_c                 0xc8
#define gApsInterframeDelay_c                 0xc9
#define gApsLastChannelEnergy_c               0xca
#define gApsLastChannelFailureRate_c          0xcb
#define gApsChannelTimer_c                    0xcc
#define gApsMaxWindowSize_c       				    0xcd  /* max fragmentation window size, FS unique AIB entry */
#define gApsMaxFragmentLength_c               0xce  /* max size of each fragment OTA, FS unique AIB entry */


/* 053474r17 Table 3.42 Status Code for Network Status Command Frame */
typedef uint8_t nwkStatusCode_t;
#define gZbNwkNoRouteAvailable_c					  0x00
#define gZbNwkTreeLinkFailure_c	  				  0x01
#define gZbNwkNonTreeLinkFailure_c				  0x02
#define gZbNwkLowBatteryLevel_c						  0x03
#define gZbNwkNoRoutingCapacity_c					  0x04
#define gZbNwkNoIndirectCapacity_c				  0x05
#define gZbNwkIndirectTransactionExpiry_c	  0x06
#define gZbNwkTargetDeviceUnavailable_c		  0x07
#define gZbNwkTargetAddressUnallocated_c	  0x08
#define gZbNwkParentLinkFailure_c					  0x09
#define gZbNwkValidateRoute_c							  0x0a
#define gZbNwkSourceRouteFailure_c				  0x0b
#define gZbNwkManyToOneRouteFailure_c		    0x0c
#define gZbNwkAddressConflict_c             0x0d
#define gZbNwkVerifyAddresses_c             0x0e
#define gZbNwkPANIdentifierUpdate_c         0x0f
#define gZbNwkNetworkAddressUpdate_c        0x10
#define gZbNwkBadFrameCounter_c             0x11
#define gZbNwkBadKeySequenceNumber_c        0x12

/* Zigbee spec 053474r17
   PAN identifier conflict uses in:  

|--------- Network Report Command ----------|
|   Report Command |                        |
|Identifier Value  |    Report Type         |
|-------------------------------------------|
|        0x00      | PAN identifier conflict|
|     0x01 - 0x07  | Reserved               |
---------------------------------------------


|--------- Network Update command ----------|
|   Report Command |                        |
|Identifier Value  |    Report Type         |
|-------------------------------------------|
|        0x00      | PAN identifier conflict|
|     0x01 - 0x07  | Reserved               |
---------------------------------------------
 */
#define gPanIdentifierConflict            0x00

/* MAC capability sub fields
  Bit 0: Alternative PAN Coordinator Capabilty
  Bit 1: RouterOrEndDevice, 1:Router, 0: EndDevice
  Bit 2: PowerSource, 1: Mains POwered, 0: Battery Operated
  Bit 3: RxOnWhenIdle
  Bit 4: Reserved
  Bit 5: Reserved
  Bit 6: MAC Security Capability
  Bit 7: Allocate Address  */
typedef uint8_t macCapabilityInfo_t;

/* Capability Information Bit-fields ( r13 - Table 3.17 ) */
#define gAlternatePancoordinator_c        0x01
#define gDeviceType_c                     0x02    /* set of FFD, clear if RFD */
#define gPowerSource_c                    0x04    /* set if mains, clear if battery */
#define gReceiverOnwhenIdle_c             0x08
#define gSecurityCapability_c             0x40
#define gAllocateAddress_c                0x80

/*Redefinition of gNlmeRouteDiscoveryReq_c  in NWKCommon.h*/
#define gNlmeRouteDiscReq_c               0x4F

/* See NLME-JOIN.request, Table 3.17, R17 */
/* see also ZdoStartMode_t. These must be same order. */
typedef uint8_t zbNwkJoinMode_t;
/* If the device is requesting to join a network through association */
#define gAssociationJoin_c                0x00
/* If the device is joining directly or rejoining the network using the orphaning procedure */
#define gOrphanJoin_c                     0x01
/* If the device is joining the network using the NWK rejoining procedure */
#define gNwkRejoin_c                      0x02
/* rejoin, but search channels first */
#define gSilentNwkRejoin_c                0x03
/* If the devivce is joining directly without sending any message (freescale only) */
#define gSilentRejoin_c                   0x04

/*-- Network Layer Frames -------------------------------------------------------*/

/*
  NWK Frame Control Field
*/
typedef uint8_t zbNwkFrameControl_t[2];

/* General NPDU Frame Format ( r13 - Seccion 3.4.1 )*/
/* Frame Control SubFields */
#define gMulticastFlag_c            0x01
#define gSourceRoute_c              0x04
#define gFrameType_c       		    0x03
#define gDstIeeeAddrPresent_c       0x08
#define gDiscoverRouteMask_c        0xC0
#define gSrcIeeeAddrPresent_c       0x10
#define gProtocolVersionMask_c      0x0C
#define gProtocolVersionShift_c     0x02
#define gSecurity_c                 0x02


#define gMulticastNonmemberRadiusMask     0x1C
#define gMulticastMemberModeMask          0x03
#define gMulticastMaxNonMemberRadiusMask  0xE0

#define gMulticastNonmemberMode           0x00
#define gMulticastMemberMode              0x01

/* Values of the Frame Type Sub-field ( r13 - Table 3.35 )*/
typedef uint8_t nwkFrameType_t;
#define gDataFrame_c		            0x00
#define gCommandFrame_c             0x01
#define gReservedFrame_c            0x02
#define gInterPanFrame_c            0x03
#define gPacketTypeMask_c           0x03

/* Values of the Discover Route Sub-field ( r13 - Table 3.36 )*/
typedef uint8_t nwkDiscoverRoute_t;
#define gZbSuppressRouteDiscovery_c	0x00
#define	gZbEnableRouteDiscovery_c		0x01
#define	gZbForceRouteDiscovery_c		0x02

/* Multicast Control Field ( r13 - Section 3.4.1.8 )*/
typedef uint8_t zbMulticastControl_t;

#ifdef __IAR_SYSTEMS_ICC__
#pragma pack(1)
#endif
typedef struct nwkHeader_tag
{
  zbNwkFrameControl_t aFrameControl;
  zbNwkAddr_t aDestinationAddress;
  zbNwkAddr_t aSourceAddress;
  uint8_t   radius;
  uint8_t   sequenceNumber;
}nwkHeader_t;

typedef struct npduFrame_tag
{
	nwkHeader_t  nwkHr;
	uint8_t  pNwkPayload[1];
}npduFrame_t;

/* Struct used to store the device EA information */
typedef struct zbEAReqData_tag{
  zbNwkAddr_t   aSourceAddress;
  zbIeeeAddr_t  aSrcIEEEAddress;
}zbEAReqData_t;
/*-- ZDO to NWK Management primitives -------------------------------------------*/

/* Network Discovery  */
typedef struct nlmeNetworkDiscoveryReq_tag
{
  zbChannels_t aScanChannels;
	uint8_t	scanDuration;
}nlmeNetworkDiscoveryReq_t;


/* Network Formation */
typedef struct nlmeNetworkFormationReq_tag
{
  zbChannels_t aScanChannels;
  uint8_t scanDuration;
  uint8_t beaconOrder;
  uint8_t superframeOrder;
  zbPanId_t aPanId;
  bool_t  batteryLifeExtension;
}nlmeNetworkFormationReq_t;


/* Permit Joining */
typedef struct nlmePermitJoiningReq_tag
{
  uint8_t  permitDuration;
}nlmePermitJoiningReq_t;


/* Start Router */
typedef struct nlmeStartRouterReq_tag
{
  uint8_t   beaconOrder;
  uint8_t   superframeOrder;
	bool_t    batteryLifeExtension;
}nlmeStartRouterReq_t;


/* Join */
typedef struct nlmeJoinReq_tag
{
  zbIeeeAddr_t aExtendedPanId;
  bool_t   joinAsRouter;
  zbNwkJoinMode_t  rejoinNetwork;
  zbChannels_t aScanChannels;
  uint8_t  scanDuration;
  uint8_t  powerSource;
  uint8_t  rxOnWhenIdle;

  /*
    ZigBee 053474r17 - Table 3.17 NLME-JOIN.request Parameters
    Name: SecurityEnalbe.
    Type: Boolean
    Range: True or Flase.
    Description: If the value of RejoinNetwork is 0x02 and
                 this is TRUE than the device will try to
                 rejoin securely.
                 Otherwise, this is set to FALSE.
  */
  bool_t  securityEnable;
}nlmeJoinReq_t;

 /* Direct Join */
typedef struct nlmeDirectJoinReq_tag
{
  zbIeeeAddr_t aDeviceAddress;
  uint8_t capabilityInformation;
}nlmeDirectJoinReq_t;

/* NLME-LEAVE.request Parameters ( r17 - Table 3.22 ) */
typedef struct nlmeLeaveReq_tag
{
	zbIeeeAddr_t  aDeviceAddress;     /* set to 0x0000000000000000 for self leave */
	bool_t	removeChildren;
	bool_t	rejoin;
	bool_t  reuseAddress;
	bool_t  silent;
}nlmeLeaveReq_t;

/* Sync */
typedef struct nlmeSyncReq_tag
{
  bool_t track;
}nlmeSyncReq_t;

/* Get,Set,Reset structure definitions */
typedef struct nlmeGetReq_tag
{
  uint8_t  nibAttributeLength;
  uint8_t  nibAttribute;
  uint8_t  *pNibAttributeValue;
}nlmeGetReq_t;

typedef struct nlmeGetNibAttr_tag
{
  uint8_t nibAttributeId;
  uint8_t aNibValue[2];
}nlmeGetNibAttr_t;

typedef struct nlmeSetReq_tag
{
  uint8_t  nibAttribute;
  uint8_t  nibAttributeLength;
  uint8_t  aNibAttributeValue[1];
}nlmeSetReq_t;

typedef struct nlmeChagePollTimer_tag
{
  uint16_t  pollTimeInterval;
}nlmeChagePollTimer_t;

typedef struct nlmeResetReq_tag
{
  uint8_t   fFullReset;
} nlmeResetReq_t;


/*
  The next higher layer use it to initiate route discovery.
  See NLME-ROUTE-DISCOVERY.request, ZigBee Spec Section 3.3.13.1
*/
typedef struct nlmeRouteDiscoveryReq_tag {
  zbAddrMode_t dstAddrMode;
	zbNwkAddr_t aDstAddr;
  uint8_t radius;
  bool_t noRouteCache;
} nlmeRouteDiscoveryReq_t;

/*
  To inform the next higher layer about the results of an attempt to initiate route
  discovery. See NLME-ROUTE-DISCOVERY.Confirm, 053474r17 Section 3.2.2.32
*/
typedef struct nlmeRouteDiscoveryConf_tag{
	zbStatus_t status;
  nwkStatusCode_t networkStatusCode;
} nlmeRouteDiscoveryConf_t;

/*
  Request to perform a energy Scan request. used by Frequency agility.
  ZED devices are not capable of Energy Scan.
*/
typedef struct nlmeEnergyScanReq_tag
{
  zbChannels_t aScanChannels;
  uint8_t scanDuration;
}nlmeEnergyScanReq_t;

/*
nlme energy Scan confirm. parameters are the same as the Energy Scan confirm for the
802.15.4 MACPHY. See 802.15.4 reference manuel for details

*/ 
typedef struct nlmeEnergyScanCnf_tag {
  uint8_t  status;
  uint8_t  scanType;
  uint8_t  resultListSize;
  uint8_t  unscannedChannels[4];
  union {  
    uint8_t *pEnergyDetectList;                  // pointer to 16 byte static buffer
  } resList;
} nlmeEnergyScanCnf_t;

/*
Nlme request for changing the Logical channel.
0x19 = channel 25. Note this request is a sync. request.
Return type will be "errorStatus".

*/
typedef struct nlmeSetChannelReq_tag
{
  uint8_t  Channel;
}nlmeSetChannelReq_t;

/*-- NWK to ZDO Management primitives -------------------------------------------*/

typedef struct networkDesc_tag
{
	uint8_t  aPanId[2];
	uint8_t  aExtendedPanId[8];
	uint8_t  logicalChannel;
	uint8_t  stackProfile;
	uint8_t  zigBeeVersion;
	uint8_t  beaconOrder;
	uint8_t  superframeOrder;
	bool_t   permitJoining;
}networkDesc_t;


  /* Network Discovery Conf */
typedef struct nlmeNetworkDiscoveryConf_tag
{
  uint8_t 			networkCount;
  networkDesc_t *pNetworkDescriptor;
  uint8_t       status;
}nlmeNetworkDiscoveryConf_t;


  /* Network Formation Conf */
typedef struct nlmeNetworkFormationConf_tag
{
  uint8_t status;
}nlmeNetworkFormationConf_t;


  /* Permit Joining Conf */
typedef struct nlmePermitJoiningConf_tag
{
  uint8_t status;
}nlmePermitJoiningConf_t;


  /* Start Router Conf */
typedef struct nlmeStartRouterConf_tag
{
  uint8_t status;
}nlmeStartRouterConf_t;


/* nlme Join Indication */
typedef struct nlmeJoinIndication_tag
{
  zbNwkAddr_t		aShortAddress;
  zbIeeeAddr_t  aExtendedAddress;
  uint8_t       capabilityInformation;
  uint8_t       rejoinNetwork;
  bool_t        secureRejoin;
}nlmeJoinIndication_t;

  /* nlme Join Conf */
typedef struct nlmeJoinConf_tag
{
  uint8_t aShotAddress[2];
  zbPanId_t aPanId;
  uint8_t havenetworkkey;
  uint8_t status;
}nlmeJoinConf_t;

  /* nlme Direct Join Conf */
typedef struct nlmeDirectJoinConf_tag
{
  uint8_t aDeviceAddress[8];
  uint8_t status;
}nlmeDirectJoinConf_t;

  /* nlme Leave Indication */
typedef struct nlmeLeaveIndication_tag
{
  zbIeeeAddr_t  aDeviceAddress;
  bool_t rejoin;
}nlmeLeaveIndication_t;

  /* nlme Leave Conf */
typedef struct nlmeLeaveConf_tag
{
  uint8_t aDeviceAddress[8];
  uint8_t status;
}nlmeLeaveConf_t;

  /* nlme Reset Conf */
typedef struct nlmeResetConf_tag
{
  uint8_t status;
}nlmeResetConf_t;

  /* nlme Sync Conf */
typedef struct nlmeSyncConf_tag
{
  uint8_t status;
}nlmeSyncConf_t;

  /* nlme Get Conf */
typedef struct nlmeGetConf_tag
{
  uint8_t  status;
  uint8_t  nibAttribute;
	uint8_t  nibAttributeLength;
  uint8_t  pNibAttributeValue[1];
}nlmeGetConf_t;

  /* nlme Set Conf */
typedef struct nlmeSetConf_tag
{
	uint8_t  status;
  uint8_t  nibAttribute;
}nlmeSetConf_t;

/* 053474r17 Table 3.2 NLDE-DATA.request Parameters */
typedef struct nldeDataReq_tag
{
  zbAddrMode_t dstAddrMode;
  zbNwkAddr_t  aDestAddress;
  uint8_t      nsduLength;
  uint8_t      nsduHandle;
  zbCounter_t  radius;
  zbCounter_t  nonmemberRadius;
  bool_t			 discoverRoute;
  bool_t			 securityEnable;
	uint8_t  		 *pNsdu;
}nldeDataReq_t;

/* 053474r17 Table 3.4 NLDE-DATA.indication Parameters */
typedef struct nldeDataIndication_tag
{
	zbAddrMode_t  dstAddrMode;
  zbNwkAddr_t   aDstAddress;
  zbNwkAddr_t   aSrcAddress;
  uint8_t       nsduLength;
  uint8_t       *pNsdu;
  uint8_t       linkQuality;
  uint16_t      rxTime;
  bool_t        securityUse;
}nldeDataIndication_t;

/* 053474r17 Table 3.33 NLME-NWK-STATUS.indication Parameters */
typedef struct nlmeNwkStatusIndication_tag
{
	zbStatus_t   statusCode;
  zbNwkAddr_t  aNetworkAddress;
}nlmeNwkStatusIndication_t;

/*This Structure is used to report to ZDO the TxAttempts and TxFailures*/
typedef struct nlmeNwkTxReport_tag
{
  uint16_t  TxAttempts;
  uint16_t  TxFailures;
}nlmeNwkTxReport_t;

/* r17 - Section 3.6.7 NWK Information in the MAC Beacons */
typedef struct beaconPayloadData_tag
{
  uint8_t protocolId;
  uint8_t Info;       /* Includes Stack Profile and nwkCProtocolVersion */  
  /*DeviceInfo
    Bit 0-1 --> Reserved
    Bit 2   --> RouterCapacity
    Bit3-6  --> DeviceDepth
    Bit7    --> EndDeviceCapacity
  */
  uint8_t DeviceInfo; 
  zbIeeeAddr_t aNwkExtPANId; /* network extended PAN Id */
  uint8_t aTxOffset[3];
  uint8_t nwkUpdateId;
} beaconPayloadData_t;

/**********************************************************************************
* Permitted values for neighbor table fields
* r17 - Table 3.48 Neighbor Table Entry Format Format
**********************************************************************************/

/* deviceType (neighbor table subfield) permitted values */
typedef uint8_t zbDeviceType_t;
#define gCoordinator_c          0x00
#define gRouter_c               0x01
#define gEndDevice_c            0x02
#define gInvalidDeviceType_c    0xFF

/* rxOnWhenIdle (neighbor table subfield) permitted values */
#define gRxOff_c                0x00
#define gRxOn_c                 0x01

/* relationship (neighbor table subfield) permitted values */
typedef uint8_t zbRelationshipType_t;
#define gNeighborIsParent_c        0x00
#define gNeighborIsChild_c         0x01
#define gNeighborIsSibling_c       0x02
#define gNone_c                    0x03
#define gPreviousChild_c           0x04
#define gUnAuthenticatedChild_c    0x05
#define gNeighborIsExpiredChild_c  0x06


/* --------------------------------------------------------------------
--------------Network Layer Command Frames Definitions-----------------
---------------------------------------------------------------------*/
typedef uint8_t gRouteReqCommandOptionValues_t;
#define gRouteReqMutlicast_c                  0x40
#define gRouteReqOriginatorIEEEAddr           0x10
#define gRouteReqResponderIEEEAddr            0x20

typedef uint8_t gManyToOneCmdOptions_t;
#define gManyToOneMask_c                      0x18
#define gIsNotManyToOne_c                     0x00
#define gIsManyTooneAndSenderSupportRRT_c     0x08
#define gIsManyTooneAndSenderNotSupportRRT_c  0x10


/* Nwk command Frame Identifiers (commandId_t)
Zigbee Spec r17, Table 3.40 NWK Command Frames */
typedef uint8_t commandId_t;
#define gRouteRequest_c       0x01
#define gRouteReply_c	        0x02
#define gNetworkStatus_c      0x03
#define gLeave_c              0x04
#define gRouteRecord_c        0x05
#define gRejoinRequest_c      0x06
#define gRejoinResponse_c     0x07
#define gLinkStatus_c         0x08
#define gNetworkReport_c      0x09
#define gNetworkUpdate_c      0x0A

#define gNpduFrame_c          0x0B

/* Rejoin Request Command Frame (r11 section 3.5.6)*/
typedef struct rejoinRequestCommand_tag
{
	commandId_t  commandFrameIdentifier;
	macCapabilityInfo_t  capabilityInformation;
}rejoinRequestCommand_t;

/* Rejoin Response Command Frame (r11 section 3.5.7)*/
typedef struct rejoinResponseCommand_tag
{
	commandId_t  commandFrameIdentifier;
	zbNwkAddr_t shortAddress;
	uint8_t  rejoinStatus;
}rejoinResponseCommand_t;

/* Rejoin Response Request */
typedef struct rejoinResponseReq_tag
{
	commandId_t  commandFrameIdentifier;
	zbNwkAddr_t shortAddress;
	uint8_t  rejoinStatus;
	zbIeeeAddr_t rejoiningDeviceIEEEaddress;
	zbIeeeAddr_t parentDeviceIEEEaddress;
}rejoinResponse_t;

/* Network Status Command Frame (r17 section 3.4.3)*/
typedef struct networkStatusCommand_tag
{
	commandId_t     commandFrameIdentifier;
	nwkStatusCode_t statusCode;
  zbNwkAddr_t     aDestinationAddress;
}networkStatusCommand_t;

/* Network Status Request from ZTC */
typedef struct networkStatusCommandReq_tag
{
	zbNwkAddr_t     aDestinationAddress;
	nwkStatusCode_t statusCode;
  zbNwkAddr_t     aNwkAddressToReport;
}networkStatusCommandReq_t;

/* Network Status Confirm */
typedef struct networkStatusCommandCnf_tag
{
	nwkStatusCode_t status;
}networkStatusCommandCnf_t;

/* This request stop or start the nwkConcentratorDiscoveryTime from ZTC */
typedef struct nwkConcDiscTimeReq_tag
{
	bool_t    StartOrStop;
}nwkConcDiscTimeReq_t;

/* stop or start the nwkConcentratorDiscoveryTime Confirm */
typedef struct nwkConcDiscTimeCnf_tag
{
	nwkStatusCode_t status;
}nwkConcDiscTimeCnf_t;

/*
  ZigBee spec. 053474r17 sec. 3.4.9
  Network Report Command 
*/
  
typedef zbPanId_t reportInformation_t;
typedef uint8_t networkReportCommandOption_t;

typedef struct networkReportCommand_tag
{
	commandId_t commandFrameIdentifier;
	networkReportCommandOption_t reportCommandOption;
	zbIeeeAddr_t EPID;
  /* the same max size that the max room size for pan descriptor in the 
     active scan*/
  reportInformation_t aReportInformationList[8];  
} networkReportCommand_t;

/*
  ZigBee spec. 053474r17 sec. 3.4.10
  Network Update Command 
*/
  
typedef struct networkUpdateCommand_tag
{
	commandId_t commandFrameIdentifier;
	networkReportCommandOption_t updateCommandOption;
	zbIeeeAddr_t EPID;
  uint8_t nwkUpdateId; 
  zbPanId_t aUpdateInformation;
}networkUpdateCommand_t;

/*
  ZigBee spec. 053474r17 sec. 3.4.8
  Link Status Command 
*/
typedef uint8_t linkStatusCommandOption_t;
typedef uint8_t statusLink_t;

typedef struct linkStatusList_tag
{
  zbNwkAddr_t shortAddress;
  statusLink_t statusLink;
} linkStatusList_t;

typedef struct linkStatusCommand_tag
{
	commandId_t commandFrameIdentifier;
	linkStatusCommandOption_t commandOption;
  linkStatusList_t aLinkStatusList[1];
} linkStatusCommand_t;


typedef zbNwkAddr_t RouteRecordRelayList_t;
typedef struct sourceRouteSubFrameField_tag
{
  uint8_t relayCount;
  uint8_t relayIndex;
  RouteRecordRelayList_t relayList[1];
}sourceRouteSubFrameField_t;

typedef struct routeRecordCommand_tag
{
  commandId_t commandFrameIdentifier;
  uint8_t relayCount;
  RouteRecordRelayList_t relayList[1]; 
}routeRecord_t;

typedef enum 
{
  onLostChildBroadcast_c = 0,
  onAssociationWithDifferentAddress_c,
  onCapabilitiesChangeRejoin_c,
  onGetNTExpiredEntry_c
}secClearDeviceDataEvent_t;
/******************************************************************************
* APS Layer
******************************************************************************/

/* ZigBee Endpoint. Endpoints available to application are 0x01 - 0xfe */
typedef uint8_t zbEndPoint_t;
#define gZbBroadcastEndPoint_c      0xff
#define gInvalidAppEndPoint_c       0xff  /* Last application endpoint is 0xfe */
#define gZbFirstAppEndPoint_c       0x01
#define gZbLastAppEndPoint_c        0xfe  /* Application endpoints may be from 0x01 - 0xfe */
#define gZbZdpEndPoint_c            0x00

/* APS transmit options. Used when calling AF_DataRequest() */
/* see also afTxOptionsDefault_c in AppAfInterface.h */
typedef uint8_t zbApsTxOption_t;
enum
{
  /* defined by ZigBee */
  gApsTxOptionNone_c         = (0),       /* no APS security (NWK layer only) */
  gApsTxOptionSecEnabled_c   = (1 << 0),  /* APS layer security enabled */
  gApsTxOptionUseNwkKey_c    = (1 << 1),  /* NWK key used for APS layer security */
  gApsTxOptionAckTx_c        = (1 << 2),  /* Use acknowledgement for transmitting frame */
  gApsTxOptionFragmentationRequested_c  =  (1<<3),  /* Fragmentation Enabled */

  /* freescale enhanced features */
  gApsTxOptionNoSecurity_c             = (1 << 5),  /* send a non-secure packet in a secure network */
  gApsTxOptionSuppressRouteDiscovery_c = (1 << 6),
  gApsTxOptionForceRouteDiscovery_c    = (1 << 7)
};

/*
  used when sending to ApsdeDataRequest()
*/
typedef union zbApsAddr_tag {
  zbIeeeAddr_t  aIeeeAddr;  /* long address (e.g. MAC address) */
  zbNwkAddr_t   aNwkAddr;   /* short address (16-bit address - may be broadcast) */
  zbGroupId_t   aGroupId;   /* group address */
} zbApsAddr_t;

/* used to uniquely identify APS frame. See giApsCounter. */
typedef uint8_t zbApsCounter_t;
typedef uint8_t zbApsConfirmId_t;

/* this header is in each fragmented MsgBuffer, both on TX and RX sides */
/* note: ZTC_FragEventMonitor() requires these two structurs start with same 2 fields */
typedef struct zbTxFragmentedHdr_tag {
  uint8_t iMsgType;              /* see primAfToApsde_t */
  struct zbTxFragmentedHdr_tag * pNextDataBlock;
  uint8_t iDataSize;
} zbTxFragmentedHdr_t;

/* this header is in each fragmented MsgBuffer, both on TX and RX sides */
/* note: ZTC_FragEventMonitor() requires these two structurs start with same 2 fields */
typedef struct zbRxFragmentedHdr_tag {
  uint8_t iMsgType;              /* see primApsdeToAf_t */
  struct zbRxFragmentedHdr_tag * pNextDataBlock;
  uint8_t iDataSize;
  uint8_t *pData;
  uint8_t iBufferNumber;
} zbRxFragmentedHdr_t;

/* used to uniquely identify Exteded Header values. See Fragmentation. */
typedef uint8_t zbExtHdr_t;


/* 
  APSDE-DATA.request parameters. See Table 2.2 in R17.
  
  ZigBee data requests are used to send data to other nodes in the network.
  Note
  
  Note: Don't change without also changing afAddrInfo_t and msgApsdeDataReq_t
*/
typedef struct zbApsdeDataReq_tag {
	zbAddrMode_t  dstAddrMode;      /* indirect, group, direct-16, direct-64 */
	zbApsAddr_t   dstAddr;          /* short address, long address or group (ignored on indirect mode) */
	zbEndPoint_t  dstEndPoint;      /* destination endpoint (ignored if group mode) */
	zbProfileId_t aProfileId;       /* application profile (either private or public) */
	zbClusterId_t aClusterId;       /* cluster identifier */
	zbEndPoint_t  srcEndPoint;      /* source endpoint */
	uint8_t       asduLength;       /* length of payload (of this fragment if fragmented) */
	uint8_t       *pAsdu;           /* pointer to payload */
	zbApsTxOption_t txOptions;      /* options on transmit (to ACK or not to ACK, that is the question... */
	uint8_t       radiusCounter;    /* radius */
  zbApsConfirmId_t  confirmId;      /* identifier for the confirm */  
  zbTxFragmentedHdr_t fragmentHdr; /* set to all 0x00s if not fragmented transaction */
} zbApsdeDataReq_t;

/* 
  ZigBee data confirm primitive. See Table 2.3 in R17.

  This is received for every AF_DataRequest(). The status indicates whether the 
  call succeeded, and depends on the input parameters to the AF_DataRequest().

  The confirm CallBack is defined by the endpoint descriptor (endPointDesc_t) 
  for the given endpoint. See EndPointConfig.c and BeeApp.c.
  
*/
typedef struct zbApsdeDataConfirm_tag
{
  zbAddrMode_t    dstAddrMode;    /* address mode */
	zbApsAddr_t     dstAddr;        /* destination address or group */
  zbEndPoint_t    dstEndPoint;    /* destination endpoint */
	zbEndPoint_t    srcEndPoint;    /* endpoint originating the msg */
  zbStatus_t      status;         /* status of confirm (worked or failed) */
  zbTime_t        txTime;         /* vendor specific time packet was transmitted (always 0) */
  zbApsConfirmId_t  confirmId;      /* identifier for the confirm */
} zbApsdeDataConfirm_t;

/* Note: NWK and link (APS) key flags are ORed together if both keys were used. */
/* Failed security packets are dropped and are not seen at the application */
typedef uint8_t zbApsSecurityStatus_t;
#define gApsSecurityStatus_Nwk_Key_c    (1<<0)   
#define gApsSecurityStatus_Link_Key_c   (1<<1)
#define gApsSecurityStatus_Unsecured_c  0

/*
  ZigBee data indication primitive.  See Table 2.4 in R17.
  
  This is received when another node has sent data to this node using 
  AF_DataRequest().
*/
typedef struct zbApsdeDataIndication_tag
{
	zbAddrMode_t    dstAddrMode;    /* address mode */
	zbNwkAddr_t     aDstAddr;       /* destination address or group */
	zbEndPoint_t    dstEndPoint;    /* destination endpoint */
	zbAddrMode_t    srcAddrMode;    /* address mode - note: always 16-bit */
	zbNwkAddr_t     aSrcAddr;       /* source address or group (never 64-bit) */
	zbEndPoint_t    srcEndPoint;    /* source endpoint */
	zbProfileId_t   aProfileId;     /* profile ID (filtered automatically by APS) */
	zbClusterId_t   aClusterId;     /* cluster ID (no filter applied to clusters) */
	uint8_t         asduLength;     /* length of payload (or this fragment if fragmented) */
	uint8_t         *pAsdu;         /* pointer to payload */
  zbStatus_t      status;/* note: status field is not presented, as any packets that fail will not become indications */
	bool_t          fWasBroadcast;  /* was the packet groupcast or broadcast? */
	zbApsSecurityStatus_t fSecurityStatus;  /* was the packet secure? At which layers? */
	uint8_t         linkQuality;    /* link quality from network layer */
  zbTime_t        rxTime;         /* vendor specific time packet was received (always 0) */
  zbRxFragmentedHdr_t fragmentHdr;  /* only present if txOptions & gApsTxOptionFragmentationRequested_c */
} zbApsdeDataIndication_t;

/*
  zbApsmeBindReqt_t

  See [R1] - See section 2.2.4.3.1 APSME-BIND.request

  Used when calling ApsmeBindRequest(), to add or update an entry in the
  binding table.
*/
typedef struct zbApsmeBindReq_tag {
  zbIeeeAddr_t  aSrcAddr;    /* long/MAC/IEEE source address */
  zbEndPoint_t  srcEndPoint; /* source endpoint to bind */
  zbClusterId_t aClusterId;  /* cluster to bind */
  zbAddrMode_t  dstAddrMode; /* desintation address mode */
  zbIeeeAddr_t  aDstAddr;    /* destination address.. may be group */
  zbEndPoint_t  dstEndPoint; /* destination endpoint (0xff for group) */
} zbApsmeBindReq_t;

/* see zbApsmeBindReq_t */
typedef zbApsmeBindReq_t zbApsmeUnbindReq_t;

/* see zbApsmeBindReq_t */
typedef zbApsmeBindReq_t zbApsmeBindEntry_t;

/* APS allows for more than 256 binding entries */
typedef uint16_t zbBindIndex_t;

/* Struction Defination for Address Map table Entry */
typedef struct zbAddressMap_tag{
  zbIeeeAddr_t aIeeeAddr; /* long/MAC/IEEE address */
  zbNwkAddr_t aNwkAddr;   /* short/NWK address */
} zbAddressMap_t;

/*
  Add a group to an endpoint. Use with APP_APSME_SapHandler.
  See APSME-ADD-GROUP.request, ZigBee Spec Section 2.2.4.5.1
*/
typedef struct zbApsmeAddGroupReq_tag {
  zbGroupId_t aGroupId;
  zbEndPoint_t endPoint;
} zbApsmeAddGroupReq_t;

/*
  Confirm for add a group to an endpoint. Use with APSME_APP_SapHandler.
  See APSME-ADD-GROUP.request, ZigBee Spec Section 2.2.4.5.2
*/
typedef struct zbApsmeAddGroupCnf_tag {
  zbStatus_t status;
  zbGroupId_t aGroupId;
  zbEndPoint_t endPoint;
} zbApsmeAddGroupCnf_t;

/*
  Remove a group from an endpoint. Use with APP_APSME_SapHandler.
  See APSME-REMOVE-GROUP.request, ZigBee Spec Section 2.2.4.5.3
*/
typedef struct zbApsmeRemoveGroupReq_tag {
  zbGroupId_t aGroupId;
  zbEndPoint_t endPoint;
} zbApsmeRemoveGroupReq_t;

/*
  Confirm for remove a group to an endpoint. Use with APSME_APP_SapHandler.
  See APSME-REMOVE-GROUP.confirm, ZigBee Spec Section 2.2.4.5.4
*/
typedef struct zbApsmeRemoveGroupCnf_tag{
  zbStatus_t status;
  zbGroupId_t aGroupId;
  zbEndPoint_t endPoint;
} zbApsmeRemoveGroupCnf_t;

/*
  Remove all groups from an endpoint. Use with APP_APSME_SapHandler.
  See APSME-REMOVE-ALL-GROUPS.request, ZigBee Spec Section 2.2.4.5.5
*/
typedef struct zbApsmeRemoveAllGroupsReq_tag{
  zbEndPoint_t endPoint;
} zbApsmeRemoveAllGroupsReq_t;

/*
  Confirm for remove all groups to an endpoint. Use with APP_APSME_SapHandler.
  See APSME-REMOVE-ALL-GROUPS.confirm, ZigBee Spec Section 2.2.4.5.6
*/
typedef struct zbApsmeRemoveAllGroupsCnf_tag{
  zbStatus_t status;
  zbEndPoint_t endPoint;
} zbApsmeRemoveAllGroupsCnf_t;

/*
  APS Frame Control Field
  See ZigBee Spec Figure 2.5

  [ Bits 0:1  ][Bits:2-3     ][Bit: 4  ][Bit:5   ][6  ][Bit:7   ]
  [ Frame type][Delivery mode][Indirect][Security][ACK][Reserved]
*/
typedef uint8_t zbApsFrameControl_t;

/* for frameType field */
#define gApsFrameControlTypeData          0x00  /* data frames */
#define gApsFrameControlTypeCommand       0x01  /* APS command frames */
#define gApsFrameControlTypeAck           0x02  /* APS ack frame */
#define gApsFrameControlTypeMask          0x03  /* for masking off type */
#define gApsFrameControlDeliveryNormal    0x00
#define gApsFrameControlDeliveryIndirect  0x04
#define gApsFrameControlDeliveryBroadcast 0x08
#define gApsFrameControlDeliveryGroup     0x0c
#define gApsFrameControlDeliveryMask      0x0c  /* for masking off delivery mode mask */
#define gApsFrameControlIndirect          0x10  /* 1=no srcEndpoint (from ZZ), 0=no dstEndpoint (to ZC) */
#define gApsFrameControlSecurity          0x20
#define gApsFrameControlNeedAck           0x40  /* do we want an ACK? */
#define gApsFrameControlExtHdrPresent     0x80  /* Is extended header present? */

/*
  Extended Header frame control types 
*/
#define gApsExtHdrFrameCtrlFragmentationNotNeeded_c 0x00
#define gApsExtHdrFrameCtrlFirstPacket_c            0x01
#define gApsExtHdrFrameCtrlNotFirstPacket_c         0x02
#define gApsExtHdrFrameCtrlFragMask_c               0x03
#define gApsExtHdrFrameCtrlReservedBits_c           0xfc

/* 
  Window acknowledgement complete
*/
#define gApsAckWindowComplete_c 0xFF

/*
  General APS frame format for a 16-bit direct mode.

  See R17 Spec, Section 2.2.5.1, Figure 2.2
*/
typedef struct zbApsFrame16Bit_tag
{
  zbApsFrameControl_t frameControl;
  zbEndPoint_t  dstEndPoint;
  zbClusterId_t aClusterId;
  zbProfileId_t aProfileId;
  zbEndPoint_t  srcEndPoint;
  zbApsCounter_t  apsCounter; /* aka transaction ID or sequence # */
} zbApsFrame16Bit_t;

/*
  General APS frame format for a group mode.

  See R17 Spec, Section 2.2.5.1, Figure 2.2
*/
typedef struct zbApsFrameGroup_tag
{
  zbApsFrameControl_t frameControl;
  zbGroupId_t   aGroupAddr;
  zbClusterId_t aClusterId;
  zbProfileId_t aProfileId;
  zbEndPoint_t  srcEndPoint;
  zbApsCounter_t  apsCounter; /* aka transaction ID or sequence # */
} zbApsFrameGroup_t;

/*
  Fragmentation APS frame format for a 16-bit direct mode.

  See R17 Spec, Section 2.2.5.1, Figure 2.2
*/
typedef struct zbApsFrame16BitFragmented_tag
{
  zbApsFrameControl_t frameControl;
  zbEndPoint_t  dstEndPoint;
  zbClusterId_t aClusterId;
  zbProfileId_t aProfileId;
  zbEndPoint_t  srcEndPoint;
  zbApsCounter_t  apsCounter; /* aka transaction ID or sequence # */
  zbExtHdr_t extHdrFrameControl;
  zbExtHdr_t extHdrBlockNumber;
} zbApsFrame16BitFragmented_t;

/* command frame */
typedef uint8_t zbApsCommand_t;

/*
  General APS command frame format (for both cmd ACK and commands).

  See R17 Spec, Section 2.2.5.1, Figure 2.2
*/
typedef struct zbApsFrameCmd_tag
{
  zbApsFrameControl_t frameControl;
  zbApsCounter_t      apsCounter;   /* aka transaction ID or sequence # */
} zbApsFrameCmd_t;

/*
  APS ACK Frame format

  See R17 Spec, Section 2.2.5.1, Figure 2.2
*/
typedef struct zbApsAckFrame_tag
{
  zbApsFrameControl_t frameControl;
  zbEndPoint_t  dstEndPoint;
  zbClusterId_t aClusterId;
  zbProfileId_t aProfileId;
  zbEndPoint_t  srcEndPoint;
  zbApsCounter_t  apsCounter; /* aka transaction ID or sequence # */
} zbApsAckFrame_t;

/*
  Fragmented acknowledge frame

  See R17 Spec, Section 2.2.5.1, Figure 2.2
*/
typedef struct zbApsAckFragmentedFrame_tag
{
  zbApsFrameControl_t frameControl;
  zbEndPoint_t  dstEndPoint;
  zbClusterId_t aClusterId;
  zbProfileId_t aProfileId;
  zbEndPoint_t  srcEndPoint;
  zbApsCounter_t  apsCounter; /* aka transaction ID or sequence # */
  zbExtHdr_t extHdrFrameControl;
  zbExtHdr_t extHdrBlockNumber;
  zbExtHdr_t extHdrAckBitField;
} zbApsAckFragmentedFrame_t;

/* Duplicate rejection table, as defined in [R1] 2.2..8.3.2 */
typedef struct zbApsDuplicateRejectionTable_tag
{
  zbNwkAddr_t     aSrcAddr;
  zbApsCounter_t  apsCounter;
} zbApsDuplicateRejectionTable_t;

/******************************************************************************
* ZDO Layer
******************************************************************************/

typedef  uint8_t  zbMgmtOptions_t;
#define zbMgmtOptionSelfLeave_c       0x01
#define zbMgmtOptionReJoin_c          0x80
#define zbMgmtOptionRemoveChildren_c  0x40

#define zbNullMode          0x00
#define zbGroupMode         0x01
#define zbIEEEMode          0x03
#define zbSrcAddressMode    0x04
#define zbDstAddressMode    0x05
#define zbSrcAndDstAddrMode 0x06
#define zbEntryMode         0x07
#define zbNwkAndIeeeMode    0x08
#define zbNwkMode           0x09

typedef struct zbClusterList_tag
{
	zbCounter_t   cNumClusters;
	zbClusterId_t aClusterList[1];
}zbClusterList_t;

typedef struct zbPtrClusterList_tag
{
	zbCounter_t  cNumClusters;
	zbClusterId_t  *pClusterList;
}zbPtrClusterList_t;

/*
	The ZDO request format to ask for a IEEE from a diferent node device.
*/
typedef struct zbNwkAddrRequest_tag
{
	zbIeeeAddr_t aIeeeAddr;
	uint8_t  requestType;
	zbIndex_t  startIndex;
}zbNwkAddrRequest_t;

typedef struct zbIeeeAddrRequest_tag
{
	zbNwkAddr_t  aNwkAddrOfInterest;
	uint8_t  requestType;
	zbIndex_t  startIndex;
}zbIeeeAddrRequest_t;

/********************/
typedef struct zbSingleDevResp_tag
{
	zbIeeeAddr_t  aIeeeAddrRemoteDev;
	zbNwkAddr_t  aNwkAddrRemoteDev;
}zbSingleDevResp_t;

typedef struct zbExtendedDevResp_tag
{
	zbIeeeAddr_t  aIeeeAddrRemoteDev;
	zbNwkAddr_t  aNwkAddrRemoteDev;
	zbCounter_t  numAssocDev;
	zbIndex_t  startIndex;
	zbNwkAddr_t  aNwkAddrAssocDevList[1]; /*Holds list of short address based on
																		number of associated devices*/
}zbExtendedDevResp_t;

/*
	The response for the IEEE and NWK address requests are identical, use same structure
*/
typedef struct  zbAddressResponse_tag
{
	zbStatus_t  status;
	union{
		zbSingleDevResp_t  singleDevResp;
		zbExtendedDevResp_t  extendedDevResp;
	}devResponse;
}zbAddressResponse_t;

/* Device Announce. See [R1] - Device_annce, Table 2.51 */
typedef struct zbEndDeviceAnnounce_tag
{
	zbNwkAddr_t   aNwkAddress;
	zbIeeeAddr_t  aIeeeAddress;
	macCapabilityInfo_t  capability;
} zbEndDeviceAnnounce_t;

/*
	the structure that is use to build the set command and the user descriptor response
*/
typedef struct zbUserDescriptorPkg_tag
{
	zbSize_t  length;
	zbUserDescriptor_t  aUserDescription;
}zbUserDescriptorPkg_t;

/*
	The request and response for the user dercriptor
*/
typedef struct zbUserDescriptorSet_tag
{
	zbNwkAddr_t  aNwkAddrOfInterest;
	zbUserDescriptorPkg_t  descriptor;
}zbUserDescriptorSet_t;

typedef struct zbUserDescriptorConfirm_tag
{
	zbStatus_t  status;
	zbNwkAddr_t  aNwkAddrOfInterest;
}zbUserDescriptorConfirm_t;

/*
	The User descriptor rquest and response data structure
*/
typedef struct zbUserDescriptorRequest_tag
{
	zbNwkAddr_t  aNwkAddrOfInterest;
}zbUserDescriptorRequest_t;

typedef struct zbUserDescriptorResponse_tag
{
	zbStatus_t  status;
	zbNwkAddr_t  aNwkAddrOfInterest;
	zbUserDescriptorPkg_t  descriptor;
}zbUserDescriptorResponse_t;

typedef struct zbPowerDescriptor_tag
{
  /*Current Mode and Available Source*/
  uint8_t     currModeAndAvailSources;
  /*Current Power Source And Lavel*/
  uint8_t     currPowerSourceAndLevel;
}zbPowerDescriptor_t;

typedef struct zbPowerDescriptorRequest_tag
{
	zbNwkAddr_t  aNwkAddrOfInterest;

}zbPowerDescriptorRequest_t;

typedef struct zbPowerDescriptorResponse_tag
{
	zbStatus_t  status;
	zbNwkAddr_t  aNwkAddrOfInterest;
	zbPowerDescriptor_t  powerDescriptor;
}zbPowerDescriptorResponse_t;

typedef struct zbPowerDescriptorStoreRequest_tag
{
	zbNwkAddr_t  aNwkAddress;
	zbIeeeAddr_t  aIeeeAddress;
	zbPowerDescriptor_t  powerDescriptor;
}zbPowerDescriptorStoreRequest_t;

typedef struct zbPowerDescriptorStoreResponse_tag
{
	zbStatus_t  status;
}zbPowerDescriptorStoreResponse_t;

typedef struct zbActiveEPStoreRequest_tag
{
	zbNwkAddr_t  aNwkAddress; /* The network address of the device */
	zbIeeeAddr_t  aIeeeAddress; /* The extended address of the device */
	zbCounter_t  activeEPCount; /* The size of the list that hold the active EPs */
	zbEndPoint_t  activeEPList[1];
}zbActiveEPStoreRequest_t;

typedef struct zbActiveEPStoreResponse_tag
{
	zbStatus_t  status;
}zbActiveEPStoreResponse_t;

/*===================================*/
/*Complex Descriptor Structure*/
typedef struct zbComplexDescriptor_tag
{
    /*Field Count*/
    uint8_t fieldCount;
    /*Language and Charecter Set*/
    uint8_t aLanguageAndCharSet[4];
    /*Manufacturer Name*/
    uint8_t aManufacturerName[6];
    /*Model Name*/
    uint8_t aModelName[6];
    /*Serial Number*/
    uint8_t aSerialNumber[6];
    /*Device URL*/
    uint8_t aDeviceUrl[17];
    /*ICON*/
    uint8_t aIcon[4];
    /*ICON URL*/
    uint8_t aIconUrl[9];
}zbComplexDescriptor_t;

typedef struct zbComplexDescriptorRequest_tag
{
	zbNwkAddr_t  aNwkAddrOfInterest;
}zbComplexDescriptorRequest_t;

typedef struct zbComplexDescriptorResponse_tag
{
	zbStatus_t  status;
	zbNwkAddr_t  aNwkAddrOfInterest;
	zbSize_t  length;
	zbComplexDescriptor_t  complexDescrip;
}zbComplexDescriptorResponse_t;

typedef struct zbDiscoveryStoreRequest_tag  /* mn */
{
	zbNwkAddr_t  aNwkAddress; /* The network address of the device */
	zbIeeeAddr_t  aIeeeAddress; /* The extended address of the device */
	zbSize_t  nodeDescriptorSize;  /* Device's node descriptor size this mean that the structure that handles the power descriptor data */
	zbSize_t  powerDescriptorSize; /* Device's power descriptor size, this mean the structure that handles the power descriptor data */
	zbSize_t  activeEPSize; /* How many Active EP we have, remember each active end point is one byte long -> ActiveEPSize = NumOfActiveEP *sizeof(byte) +1 for the counter*/
	zbCounter_t  simpleDescriptorCount; /* How many simple descriptors we have, remember there is one simple descriptor for each Active EP */
	zbSize_t  simpleDescriptorList[1]; /* An array with size equal to the simpleDescriptorCount, each possition in the array holds the size of the correspondig simple descriptor of the list o f simples descriptors */
}zbDiscoveryStoreRequest_t;

typedef struct zbDiscoveryStoreResponse_tag  /* mn */
{
	zbStatus_t status;  /* The status of the request */
}zbDiscoveryStoreResponse_t;

/**************************************
 * Node Descriptor and Node Descriptor Store
 * Warning: If you modify this structure need to modify also BeeStackConfigParams_tag in
 *          BeeStack_Globals.h to make them match
 **************************************/
typedef struct zbNodeDescriptor_tag
{
	/*Logical Type*/
	uint8_t		logicalType;
	/*APS Flag: Not Used and Frequency Band: Frequency band used by the radio used*/
	uint8_t		apsFlagsAndFreqBand;
	/*MAC capability Flag*/
	uint8_t		macCapFlags;
	/*Manufacturer Code Flag*/
	uint8_t		aManfCodeFlags[2];
	/*Maximum buffer Size*/
	uint8_t   maxBufferSize;
	/*Maximum Transfer Size*/
	uint8_t		aMaxTransferSize[2];
	/* The server Mask to be sended in the request payload.*/
	zbServerMask_t  aServerMask;
	/*Maximum Outgoing Transfer Size*/
	uint8_t		aMaxOutTransferSize[2];
	/* The descriptor capability field.	*/
	uint8_t  descCapField;	
}zbNodeDescriptor_t;

typedef struct zbNodeDescriptorStoreRequest_tag  /* mn */
{
	zbNwkAddr_t  aNwkAddress; /* The network address of the device */
	zbIeeeAddr_t  aIeeeAddress; /* The extended address of the device */
	zbNodeDescriptor_t  nodeDescriptor;
}zbNodeDescriptorStoreRequest_t;

typedef struct zbNodeDescriptorStoreResponse_tag  /* mn */
{
	zbStatus_t  status; /* the status of the request */
}zbNodeDescriptorStoreResponse_t;

typedef  struct  zbNodeDescriptorRequest_tag
{
	zbNwkAddr_t  aNwkAddrOfInterest;
}zbNodeDescriptorRequest_t;

typedef  struct  zbNodeDescriptorResponse_tag
{
	zbStatus_t  status;
	zbNwkAddr_t  aNwkAddrOfInterest;
	zbNodeDescriptor_t  nodeDescriptor;
}zbNodeDescriptorResponse_t;

/************************************************
 * Discovery Cache Request and Response.
 ************************************************/
typedef struct zbDiscoveryCacheRequest_tag
{
	zbNwkAddr_t  aNwkAddress;
	zbIeeeAddr_t  aIeeeAddress;
}zbDiscoveryCacheRequest_t;

typedef struct zbDiscoveryCacheResponse_tag
{
	zbStatus_t  status;
}zbDiscoveryCacheResponse_t;

/***********************************************
 * System Srvice Discovery Request and Response
 ***********************************************/
 /* This request always send it message to the 0xfffc addreas meaning the RxOnWhenIdle devices */
typedef struct zbSystemServerDiscoveryRequest_tag
{
	zbServerMask_t  aServerMask;/* Bit number: (053474r17ZB - Sec: 2.3.2.3.10 Server Mask Field)
																	0->Primary Trust Center
																	1->Backup Trust Center
																	2->Primary Binding table cache
																	3->Backup binding table cache
																	4->Primary Discovery Cache
																	5->Backup Discovery Cache
																	6->Network Manager
																	7-15->Reserved*/
}zbSystemServerDiscoveryRequest_t;

typedef struct zbSystemServerDiscoveryResponse_tag
{
	zbStatus_t  status;  /* The status of the System Dicovery Response command */
	zbServerMask_t  aServerMask;/* Bit number: (053474r17ZB - Sec: 2.3.2.3.10 Server Mask Field)
																	0->Primary Trust Center
																	1->Backup Trust Center
																	2->Primary Binding table cache
																	3->Backup binding table cache
																	4->Primary Discovery Cache
																	5->Backup Discovery Cache
																	6->Network Manager
																	7-15->Reserved*/
}zbSystemServerDiscoveryResponse_t;

/****************************************
 * Simple descriptor and Simple Descriptor Store
 *****************************************/
typedef struct zbSimpleDescriptor_tag
{
	/*End point ID */
	zbEndPoint_t  endPoint;
	/*Application Profile ID*/
	zbProfileId_t aAppProfId;
	/*Appliacation Device ID*/
	zbDeviceId_t  aAppDeviceId;
	/*Application Device Version And APS Flag*/
	uint8_t       appDevVerAndFlag;
	/*Number of Input Cluster ID Supported by the End Point*/
	zbCounter_t   appNumInClusters;
	/*Place Holder for the list of Input Cluster ID*/
	uint8_t      *pAppInClusterList;
	/*Number of Output Cluster ID Supported by the End Point*/
	zbCounter_t   appNumOutClusters;
	/*Place Holder for the list of Output Cluster ID*/
	uint8_t      *pAppOutClusterList;
}zbSimpleDescriptor_t;

typedef  struct  zbSimpleDescriptorPackageStore_tag  /* mn */
{
/*End point ID */
zbEndPoint_t  endPoint;
/*Application Profile ID*/
zbProfileId_t aAppProfId;
/*Appliacation Device ID*/
zbDeviceId_t  aAppDeviceId;
/*Application Device Version And APS Flag*/
uint8_t  appDevVerAndFlag;
/*Number of Input Cluster ID Supported by the End Point and Place Holder for the list of Input Cluster ID*/
zbPtrClusterList_t  inputClusters;
/*Number of Output Cluster ID Supported by the End Point and Place Holder for the list of Output Cluster ID*/
zbPtrClusterList_t  outputClusters;
}zbSimpleDescriptorPackageStore_t;

typedef  struct  zbSimpleDescriptorPackageResponse_tag
{
	/*End point ID */
	zbEndPoint_t  endPoint;
	/*Application Profile ID*/
	zbProfileId_t aAppProfId;
	/*Appliacation Device ID*/
	zbDeviceId_t  aAppDeviceId;
	/*Application Device Version And APS Flag*/
	uint8_t  appDevVerAndFlag;
	/*Number of Input Cluster ID Supported by the End Point*/
	zbClusterList_t  inClusterList;
	/* The output cluster willbe added here at the end of the structur to form the package (OTA) */
}zbSimpleDescriptorPackageResponse_t;

typedef struct zbSimpleDescriptorStoreRequest_tag  /* mn */
{
	zbNwkAddr_t   aNwkAddress; /* The network address of the device */
	zbIeeeAddr_t  aIeeeAddress; /* The extended address of the device */
	zbSize_t      simpleDescriptorLength; /* The size in bytes of the simple descriptor */
	zbSimpleDescriptorPackageStore_t  simpleDescriptor;  /* The simple descriptor for a particular Endpoint - Bfore was simpleDescriptorList[1] */
}zbSimpleDescriptorStoreRequest_t;

typedef  struct  zbSimpleDescriptorStoreResponse_tag  /* mn */
{
	zbStatus_t  status;  /* This status indicates success or some error code */
}zbSimpleDescriptorStoreResponse_t;

typedef struct zbRemoveNodeCacheRequest_tag
{
	zbNwkAddr_t  aNwkAddress;
	zbIeeeAddr_t  aIeeeAddress;
}zbRemoveNodeCacheRequest_t;

typedef struct zbRemoveNodeCacheResponse_tag
{
	zbStatus_t  status;
}zbRemoveNodeCacheResponse_t;

typedef struct  zbSimpleDescriptorRequest_tag
{
	zbNwkAddr_t   aNwkAddrOfInterest;
	zbEndPoint_t  endPoint;
}zbSimpleDescriptorRequest_t;

typedef struct  zbExtSimpleDescriptorRequest_tag
{
	zbNwkAddr_t   aNwkAddrOfInterest;
	zbEndPoint_t  endPoint;
	zbIndex_t     startIndex;  
}zbExtSimpleDescriptorRequest_t;

typedef struct zbSimpleDescriptorResponse_tag
{
	zbStatus_t   status;
	zbNwkAddr_t  aNwkAddrOfInterest;
	zbSize_t     length;
	zbSimpleDescriptorPackageResponse_t  simpleDescriptor;  /*Holds a simple descriptor*/
}zbSimpleDescriptorResponse_t;

typedef struct zbExtSimpleDescriptorResponse_tag
{
	zbStatus_t    status;
	zbNwkAddr_t   aNwkAddrOfInterest;
	zbEndPoint_t  endPoint;
	zbCounter_t   appNumInClusters;
	zbCounter_t   appNumOutClusters;
	zbIndex_t     startIndex;
	zbClusterId_t appClusterList[1];
}zbExtSimpleDescriptorResponse_t;

/*************************
 * Bind Request And Response.
 * Also Unbind Request and
 * Response
 **************************/
typedef struct extendedMode_tag
{
	zbIeeeAddr_t  aDstAddress;
	zbEndPoint_t  dstEndPoint;
}extendedMode_t;

typedef struct groupMode_tag
{
	zbGroupId_t  aDstaddress;
}groupMode_t;

typedef union zbBindModeData_tag
{
	extendedMode_t  extendedMode;
	groupMode_t     groupMode;
}zbBindModeData_t;

typedef struct zbBindUnbindRequest_tag
{
	zbIeeeAddr_t      aSrcAddress;
	zbEndPoint_t      srcEndPoint;
	zbClusterId_t     aClusterId;
	zbAddrMode_t      addressMode;
	zbBindModeData_t  destData;
}zbBindUnbindRequest_t;

typedef struct zbBindUnbindResponse_tag
{
	zbStatus_t  status;
}zbBindUnbindResponse_t;

typedef struct zbActiveEpRequest_tag
{
	zbNwkAddr_t  aNwkAddrOfInterest;

}zbActiveEpRequest_t;

typedef struct zbExtActiveEpRequest_tag
{
	zbNwkAddr_t  aNwkAddrOfInterest;
  zbIndex_t    startIndex;
}zbExtActiveEpRequest_t;

typedef struct zbActiveEpResponse_tag
{
	zbStatus_t  status;
	zbNwkAddr_t  aNwkAddrOfInterest;
	zbCounter_t   activeEpCount;
	zbEndPoint_t   pActiveEpList[1];	 /*Holds array of Active EndPoints*/
}zbActiveEpResponse_t;

typedef struct zbExtActiveEpResponse_tag
{
	zbStatus_t     status;
	zbNwkAddr_t    aNwkAddrOfInterest;
	zbCounter_t    activeEpCount;
	zbIndex_t      startIndex;
	zbEndPoint_t   pActiveEpList[1];	 /*Holds array of Active EndPoints*/
}zbExtActiveEpResponse_t;


/*
  See [R1] - 2.4.3.2.1 End_Device_Bind_req

  Allows a device or 3rd party tool match for binding using a binding toggle
  engine in the ZigBee coordinator.
*/
typedef struct zbEndDeviceBindRequest_tag
{
	zbNwkAddr_t         aBindingTarget;
  zbIeeeAddr_t        aSrcIeeeAddress;
	zbEndPoint_t        srcEndPoint;
	zbProfileId_t       aProfileId;
	zbPtrClusterList_t  inputClusterList; /* count + ptr */
	zbPtrClusterList_t  outputClusterList;
} zbEndDeviceBindRequest_t;

/*
  See [R1] - 2.4.4.2.1 End_Device_Bind_rsp

  Allows a device or 3rd party tool match for binding using a binding toggle
  engine in the ZigBee coordinator.
*/
typedef struct zbEndDeviceBindResponse_tag
{
	zbStatus_t  status;
} zbEndDeviceBindResponse_t;

/******************/
typedef struct zbMatchDescriptorRequest_tag
{
	zbNwkAddr_t         aNwkAddrOfInterest;
	zbProfileId_t       aProfileId;
	zbCounter_t  cNumClusters;
	zbClusterId_t  aClusterList[1];
	zbPtrClusterList_t  outputClusterList;
}zbMatchDescriptorRequest_t;

typedef struct zbMatchDescriptorRequestPtr_tag
{
	zbNwkAddr_t         aNwkAddrOfInterest;
	zbProfileId_t       aProfileId;
	zbPtrClusterList_t  inputClusterList;
	zbPtrClusterList_t  outputClusterList;
}zbMatchDescriptorRequestPtr_t;


typedef struct zbMatchDescriptorResponse_tag
{
	zbStatus_t  status;
	zbNwkAddr_t  aNwkAddrOfInterest;
	zbSize_t  matchLength;
	zbEndPoint_t  matchList[1]; /*Holds array of EndPoints that match the criteria (EndPoint Length = 1 byte)*/
}zbMatchDescriptorResponse_t;

typedef struct zbFindNodeCacheRequest_tag  /* mn */
{
	zbNwkAddr_t  aNwkAddrOfInterest; /* The network address of the device of interest */
	zbIeeeAddr_t  aIeeeAddrOfInterest; /* The MAC Address of the device of interest */
}zbFindNodeCacheRequest_t;

/******************************************
* This response can be given by two diferent nodes,
* the Cache device holding the information or the
* DEvice of interes it self ZigBee 1.1 r09 page 150
*******************************************/
typedef struct zbFindNodeCacheResponse_tag  /* mn */
{
	zbNwkAddr_t  aCacheNwkAddr; /* The network addres of the device Cache */
	zbNwkAddr_t  aNwkAddrOfInterest; /* The network addres of the device of interest */
	zbIeeeAddr_t  aIeeeAddrOfInterest; /* The MAC address of the device of interest */
}zbFindNodeCacheResponse_t;

/*
  See Cluster 0x0033, ZDP Management Bind Response
  See [R1] 2.4.3.3.4 Mgmt_Bind_req

  Used for getting the binding table from another node.
*/
typedef struct zbMgmtBindRequest_tag
{
	zbIndex_t  startIndex;
}zbMgmtBindRequest_t;


/*
  See Cluster 0x8033, ZDP Management Bind Response
  See [R1] 2.4.4.3.4 Mgmt_Bind_rsp

  Used for returning the binding table to a remote node.
*/
typedef struct zbMgmtBindResponse_tag
{
	zbStatus_t          status;
	zbCounter_t         bindingTableEntries;      /* total entries in binding table */
	zbIndex_t           startIndex;               /* starting index */
	zbCounter_t         bindingTableListCount;    /* count following in this packet */
	zbApsmeBindEntry_t  aBindingTableList[1]; /* binding structures */
}zbMgmtBindResponse_t;

typedef struct zbBindRegisterRequest_tag  /* ClusterId 0x0023 */
{
	zbIeeeAddr_t  aIeeeAddress;  /* The addrress of the node that whishes to hold its own information */
}zbBindRegisterRequest_t;

typedef struct zbBindResgisterResponse_tag  /* ClusterId 0x8023 */
{
	zbStatus_t  status; /* the possible status of the response: SUCCESS, NOT_SUPPORTED, TABLE_FULL*/
	zbCounter16_t  aBindingTableEntrys; /* thsi field indicates how many times we found the address send in the request */
	zbCounter16_t  aBindingTableListCount; /* no matter how many times we found it, we are only sending back this many */
	zbApsmeBindEntry_t  aBindingTableList[1];
}zbBindRegisterResponse_t;

/*
	Comments
*/
typedef struct zbReplaceDeviceRequest_tag  /* ClusterId 0x0024 */
{
	zbIeeeAddr_t  aOldAddress;  /* the old address to match and replace */
	zbEndPoint_t  oldEndPoint;  /* the old Endpoint to match and replace, only if the stored one is different of zero */
	zbIeeeAddr_t  aNewAddress;  /* The new address to store */
	zbEndPoint_t  newEndPoint;  /* the new endpoint to be stored only if the previous one is  not zero */
}zbReplaceDeviceRequest_t;

typedef struct zbReplaceDeviceResponse_tag  /* ClusterId 0x8024 */
{
	zbStatus_t  status;  /* The status of the request */
}zbReplaceDeviceResponse_t;

typedef zbBindUnbindRequest_t zbStoreBkupBindEntryRequest_t;  /* ClusterId 0x0025 */

typedef struct zbStoreBkupBindEntryResponse_tag  /* ClusterId 0x8025 */
{
	zbStatus_t  status;
}zbStoreBkupBindEntryResponse_t;

typedef zbBindUnbindRequest_t zbRemoveBackupBindEntryRequest_t;  /* ClusterId 0x26 */

typedef struct zbRemoveBackupBindEntryResponse_tag  /* ClusterId 0x8026 */
{
	zbStatus_t  status;
}zbRemoveBackupBindEntryResponse_t;

typedef struct zbRecoverBindTableRequest_tag /* ClusterID  0x0028*/
{
	zbIndex16_t  startIndex;
}zbRecoverBindTableRequest_t;

typedef struct zbRecoverBindTableResponse_tag  /* ClusterID 0x8028*/
{
	zbStatus_t  status;
	zbCounter16_t  aBindingTableEntries;
	zbIndex16_t  startIndex;
	zbCounter16_t  aBindingTableListCount;
	zbApsmeBindEntry_t  aBindingTableList[1];
}zbRecoverBindTableResponse_t;

typedef struct zbBackupSourceBindResponse_tag
{
	zbStatus_t  status;
}zbBackupSourceBindResponse_t;

typedef struct zbBackupSourceBindRequest_tag
{
	zbCounter16_t  SourceTableEntries;
	zbCounter16_t  StartIndex;
	zbCounter16_t  SourceTableListCount;
	zbIeeeAddr_t  SourceTableList[1];/*before was holdBindingInfo_t */
}zbBackupSourceBindRequest_t;

typedef struct zbRecoverSourceBindRequest_tag
{
	zbIndex16_t  StartIndex;
}zbRecoverSourceBindRequest_t;

typedef struct zbRecoverSourceBindResponse_tag
{
	zbStatus_t  Status;
	zbCounter16_t  SourceTableEntries;
	zbIndex16_t  StartIndex;
	zbCounter16_t  SourceTableListCount;
	zbIeeeAddr_t  SourceTableList[1];
}zbRecoverSourceBindResponse_t;

typedef struct zbBackupBindTableRequest_tag
{
	zbCounter16_t  BindingTableEntries;
	zbIndex16_t  StartIndex;
	zbCounter16_t  BindingTableListCount;
	zbApsmeBindEntry_t  BindingTableList[1];
}zbBackupBindTableRequest_t;

typedef struct zbBackupBindTableResponse_tag
{
	zbStatus_t  status;
	zbCounter16_t  entryCount;
}zbBackupBindTableResponse_t;

/*====================================================*/
typedef struct zbMgmtLeaveRequest_tag
{
	zbIeeeAddr_t  aDeviceAddress;
	zbMgmtOptions_t  mgmtOptions;
}zbMgmtLeaveRequest_t;

typedef struct zbMgmtLeaveResponse_tag
{
	zbStatus_t  status;
}zbMgmtLeaveResponse_t;

typedef struct  zbMgmtDiscoveryCacheList_tag
{
	zbIeeeAddr_t  aIeeeAddress;
	zbNwkAddr_t  aNwkAddress;
}zbMgmtDiscoveryCacheList_t;

typedef struct zbMgmtCacheResponse_tag  /* ClusterID 0x8037 */
{
	zbStatus_t  status;
	zbCounter_t  discoveryCacheEntries;
	zbIndex_t  startIndex;
	zbCounter_t  discoveryCacheListCount;
	zbMgmtDiscoveryCacheList_t  discoveryCacheList[1];
}zbMgmtCacheResponse_t;

typedef struct zbMgmtCacheRequest_tag  /* ClusterID 0x0037 */
{
	zbIndex_t  startIndex;
}zbMgmtCacheRequest_t;

typedef struct zbOTAFormatNetworkDescriptor_tag
{
	zbIeeeAddr_t  aExtendedPanId;
	uint8_t  logicalChannel;
	uint8_t  StackProfileAndZigBeeVersion; /*first 4 bits for Stackprofile */
	uint8_t  BeaconOrderAndSuperframeOrder;/* first 4 bits for BeaconOrder*/
	bool_t   permitJoining; /*first bit, the rest is reserved */
}zbOTAFormatNetworkDescriptor_t;

typedef struct zbMgmtNwkDiscResponse_tag
{
	zbStatus_t  status;
	zbCounter_t  nwkCount;
	zbIndex_t  startIndex;
	zbCounter_t  nwkListCount;
	zbOTAFormatNetworkDescriptor_t  nwkList[1];
}zbMgmtNwkDiscResponse_t;

typedef struct zbMgmtNwkDiscoveryRequest_tag
{
	zbChannels_t  aScanChannel;
	zbCounter_t  scanDuration;
	zbIndex_t  startIndex;
}zbMgmtNwkDiscoveryRequest_t;

typedef struct zbMgmtLqiRequest_tag
{
	zbIndex_t  startIndex;
}zbMgmtLqiRequest_t;

typedef struct zbNeighborTableList_tag
{
	zbIeeeAddr_t  aExtendedPanId;
	zbIeeeAddr_t  aExtendedAddr;
	zbNwkAddr_t  aNetworkAddr;
	uint8_t  deviceProperty;
	bool_t   permitJoining;
	uint8_t depth;
	uint8_t  lqi;
}zbNeighborTableList_t;

typedef struct zbMgmtLqiResponse_tag
{
	zbStatus_t  status;
	zbCounter_t  neighbourTableEntries;
	zbIndex_t  startIndex;
	zbCounter_t  neighbourTableListCount;
	zbNeighborTableList_t  neighbourTableList[1];
}zbMgmtLqiResponse_t;

typedef struct zbMgmtRtgRequest_tag
{
	zbIndex_t  startIndex;
}zbMgmtRtgRequest_t;

/* The structure tha that was here was moved to appzdointerface file to be ble to include the routeDiscoveryTable_t data type */


typedef struct zbMgmtDirectJoinRequest_tag
{
	zbIeeeAddr_t  aDeviceAddress;
	macCapabilityInfo_t  capabilityInformation;
}zbMgmtDirectJoinRequest_t;

typedef struct zbMgmtDirectJoinResponse_tag
{
	zbStatus_t  status;
}zbMgmtDirectJoinResponse_t;

typedef struct zbMgmtPermitJoiningResponse_tag
{
	zbStatus_t  status;
}zbMgmtPermitJoiningResponse_t;

typedef struct zbMgmtPermitJoiningRequest_tag
{
	zbCounter_t  PermitDuration;
	uint8_t  TC_Significance;
}zbMgmtPermitJoiningRequest_t;


typedef struct zbNwkManagerData_tag{
  uint8_t       nwkUpdateId;
  zbNwkAddr_t   aNwkManagerAddr;
}zbNwkManagerData_t;

typedef union zbudata_tag{
  uintn8_t      ScanCount;
  zbNwkManagerData_t NwkManagerData;
}zbudata_t;

/* 053474r17ZB - Sec. 2.4.3.3.9 Mgmt_NWK_Update_req */
typedef struct zbMgmtNwkUpdateRequest_tag
{
  zbChannels_t  aScanChannels;
  uint8_t       ScanDuration;
  zbudata_t     ExtraData;
}zbMgmtNwkUpdateRequest_t;

/* 053474r17ZB - Sec. 2.4.4.3.9 Mgmt_NWK_Update_notify */
typedef struct zbMgmtNwkUpdateNotify_tag
{
  zbStatus_t     Status;
  zbChannels_t   ScannedChannels;
  uint16_t  TotalTransmissions;
  uint16_t  TransmissionFailures;
  uint8_t    ScannedChannelsListCount;
  zbEnergyValue_t        aEnergyValues[1];
}zbMgmtNwkUpdateNotify_t;
/******************************************************************************
* Inter-Pan Layer
******************************************************************************/
/* 075356r10ZB AMI/SE profile Annex B - Inter PAN transmission mechanism */

typedef struct zbInterPanDataReq_tag {
  zbAddrMode_t  srcAddrMode;    /* indirect, group, direct-16, direct-64 */
	zbAddrMode_t  dstAddrMode;    /* indirect, group, direct-16, direct-64 */
	zbPanId_t     dstPanId;
	zbApsAddr_t   dstAddr;        /* short address, long address or group (ignored on indirect mode) */
	zbProfileId_t aProfileId;     /* application profile (either private or public) */
	zbClusterId_t aClusterId;     /* cluster identifier */
	uint8_t       asduLength;     /* length of payload */
	uint8_t       *pAsdu;         /* pointer to payload */
	uint8_t       *pAsduHandle;   /* pointer to variable where handle is written to */
} zbInterPanDataReq_t;

typedef struct zbInterPanNwkStubHeader_tag {
zbNwkFrameControl_t aFrameControl;
} zbInterPanNwkStubHeader_t;

typedef struct npduStubFrame_tag
{
	zbInterPanNwkStubHeader_t  nwkHr;
	uint8_t  pNwkPayload[1];
}npduStubFrame_t;


typedef struct zbInterPanStubApsFrameGroup_tag
{
  zbApsFrameControl_t frameControl;
  zbGroupId_t   aGroupAddr;
  zbClusterId_t aClusterId;
  zbProfileId_t aProfileId;
} zbInterPanStubApsFrameGroup_t;

typedef struct zbInterPanStubApsFrame16Bit_tag
{
  zbApsFrameControl_t frameControl;
  zbClusterId_t       aClusterId;
  zbProfileId_t       aProfileId;
} zbInterPanStubApsFrame16Bit_t;


typedef union zbInterPanApsStubHeader_tag
{
  zbInterPanStubApsFrameGroup_t pGroup;  /* send to a group */
  zbInterPanStubApsFrame16Bit_t p16Bit;  /* send to a 16-bit addr */
} zbInterPanApsStubHeader_t;


typedef struct zbInterPanDataConfirm_tag
{
  uint8_t         AsduHandle;
  zbStatus_t      status;         /* status of confirm (worked or failed) */
} zbInterPanDataConfirm_t;

typedef struct zbInterPanDataIndication_tag
{
	zbAddrMode_t    srcAddrMode;    /* address mode */
	zbPanId_t       srcPanId;
	zbApsAddr_t     aSrcAddr;       /* source address or group */
	zbAddrMode_t    destAddrMode;    /* address mode */
	zbPanId_t       dstPanId;
	zbApsAddr_t     aDstAddr;       /* source address or group */	
	zbProfileId_t   aProfileId;     /* profile ID (filtered automatically by APS) */
	zbClusterId_t   aClusterId;     /* cluster ID (no filter applied to clusters) */
	uint8_t         asduLength;     /* length of payload */
	uint8_t         *pAsdu;         /* pointer to payload */
	uint8_t         linkQuality;    /* link quality from network layer */
} zbInterPanDataIndication_t;


/*****************************************************************************************
                      SSP types and primitive types
*****************************************************************************************/

/* Type Elments of the Network Security Materila Descriptor, (aka. nwkSecurityMaterialSet) */

/*
  053474r17 table 4.2
  Name: KeySeqNumber
  Type: Octet
  Range: 0x00-0xff
  Default: 0x00
*/
typedef uint8_t zbKeySeqNumber_t;

/*
  053474r17 table 4.2
  Name: OutGoingFrameCounter
  Type: Oredered set of 4 Octets
  Range: 0x00000000 - 0xffffffff
  Default: 0x00000000
*/
/* This type can defined on the ZigBee.h files, is common for APS and NWK keys.. */
typedef uint32_t zbFrameCounter_t;

/* Elements of the Incoming Frame Counter Descriptor, 053474r17 table 4.3 */
/*
  053474r17 table 4.2
  Name: IncomingFrameCounterSet
  Type: Set of incoming frame counter descriptor values, See 053474r17 table 4.3
  Range: Variable
  Default: Null set
*/
typedef struct zbIncomingFrameCounterSet_tag
{
  /*
    Name: SenderAddress
    Type: Device Address
    Range: Any valid 64-bit address
    Default: Device Specific/index on NT.
  */
  /* zbIeeeAddr_t aSenderAddress; */  /* NOTE: This is specified as a IEEE address on
                                         the spec, for size saving issues we change it
                                         to an iindex ont he Neighbor table. */
  uint8_t iSenderAddress;  /* An index from 0 - 255, that indicates where in the n*/

  /*
    Name: IncomigFrameCounter
    Type: Ordered set of 4 octets
    Range: 0x00000000 - 0xffffffff
    Default: 0x00000000
  */
  zbFrameCounter_t IncomingFrameCounter;
}zbIncomingFrameCounterSet_t;

/*
  053474r17 table 4.2
  Name: Key
  Type: Ordered set of 16 octets (128 bits key)
  Range: Variable
  Default: -
*/
typedef uint8_t zbAESKey_t[16];

/*
  053474r17 table 4.2
  Name: keyType
  Type: Octet
  Range: 0x00 - 0x05
  Default: 0x01
*/
typedef uint8_t zbKeyType_t;

/*
  Table 4.12 KeyType Parameter of the Transport-Key Primitive
*/
#define gTrustCenterMasterKey_c    0x00
#define gStandardNetworkKey_c      0x01
#define gApplicationMasterKey_c    0x02
#define gApplicationLinkKey_c      0x03
#define gTrustCenterLinkKey_c      0x04
#define gHighSecurityNetworkKey_c  0x05
#define gNoApsKey_c                0xFC
#define gTrustCenterRandomMasterkey_c  0xFF


/*
  053474r17 Table 4.28
  Name: Action
  Type: Enum
  Range: 0x00 - 0x02
  values:
         0x00: INITIATE
         0x01: RESPOND_ACCEPT
         0x02: RESPOND_REJECT
*/
typedef enum
{
  gINITIATE_c=0x00,
  gRESPOND_ACCEPT_c,
  gRESPOND_REJECT_c
}zbAction_t;


/*
  Table 4.35 KeyType Parameter of the Init Challenge Command
*/
/*
  Indicates the key is an active network key or a Link key shared between the initator and
  responder.
*/
#define  gActiveNwkKey_c  0x00
#define  gLinkKeyShared_c  0x01


/*
  Name: Key Info
  Type: Struct.
  Description: The key info type is divided in two fields: KeyType and KeySeqNumber.
*/
typedef struct zbKeyInfo_tag
{
  /*
    Name: Key Type
    Type: Integer.
    Range: 0x00 - 0xFF
    Description: The key type field shall be set to 1 when the network key is being
                 requested, shall be set to 2 when an application key is being requested,
                 and 4 when a Trust Center link key is being requested.
  */
  zbKeyType_t  keyType;

  /*
    Name: Key Sequence Type
    Type: Integer (can and can not be present).
    Range: 0x00 - 0xFF
    Description: The KeyType field shall be set to 0x00 to indicate that the shared key is
    the active network key shared between the initiator and the responder.
  */
  zbKeySeqNumber_t keySeqNumber;
}zbKeyInfo_t;


/*this type is used for the Mac Data command in high security*/
typedef uintn8_t zbMacData_t [4];
/*****************************************************************************************
                                NIB SPECIFIC DATA TYPES
*****************************************************************************************/
/* 053474r17 Table 4.1 NIB Security attibutes data types */

/*
  Some of the types here can be arange in diferent header files for now
  and until the the final design gets applyed they will be here.
  Some of them will be mark in comments if they can be move or should
  be defined on a different file.
*/

/*
  053474r17 Table 4.1
  Name: nwkSecurityLevel
  Type Octet
  Range: 0x00 - 0x07
  Default: 0x05
  NIB Id: 0xa0
*/
typedef uint8_t zbNwkSecurityLevel_t;

/*
  Suggested type definition for the nwkSecurityMaterialSet
*/
/*
  053474r17 Table 4.1
  Name: nwkSecurityLevel
  Type A set of 0, 1 or 2 nwk security material descriptor (Table 4.2)
  Range: Variable
  Default: --
  NIB Id: 0xa1
*/
typedef struct zbNwkSecurityMaterialSet_tag
{
  zbKeySeqNumber_t             keySeqNumber;
  zbFrameCounter_t             outgoingFrameCounter;
  zbAESKey_t                   nwkKey;
  zbKeyType_t                  keyType;
  zbIncomingFrameCounterSet_t  *pIncomingFrameCounterSet;
}zbNwkSecurityMaterialSet_t;

/*
  053474r17 Table 4.1
  Name:nwkActiveKeySeqNumber
  Type: Octet
  Range: 0x00 - 0xff
  Default: 0x00
  NIB Id: 0xa2
*/
typedef uint8_t zbNwkActiveKeySeqNumber_t;

/*****************************************************************************************
                            AUXILIARY FRAME SPECIFIC TYPES
*****************************************************************************************/

/*
  Security Control field for the Auxiliary Frame.
  .... .BBB: (0 - 2) Secure level.
  ...B B...: (3 - 4) Key Identifier.
  ..B. ....: (5) Extended nonce.
  XX.. ....: (6 - 7) Resrved.
*/
typedef uint8_t zbAuxSecurityControlField_t;
#define gAuxSecurityLevelMask_c  0x07

/*
  053474r17Zb - Sec. 4.5.1 Auxiliary Frame Header Format, Table 4.23
*/
/* NOTE: The  must be removed when the new secure module gets activated. */
typedef struct zbAuxHeader_tag
{
  zbAuxSecurityControlField_t  AuxSecurityControlField;
  zbFrameCounter_t             FrameCounter;
  zbIeeeAddr_t                 aSourceAddress;
  zbKeySeqNumber_t             SequenceNumber;
}zbAuxHeader_t;

/*
  053474r17Zb - Table 4.38 Security Levels Available to the NWK, and APS Layers
*/
/* Security Level Sub Field: 000 */
#define gNoSecurity_c  0x00

/* Security Level Sub Field: 001 */
#define gMIC32_c       0x01

/* Security Level Sub Field: 010 */
#define gMIC64_c       0x02

/* Security Level Sub Field: 011 */
#define gMIC128_c      0x03

/* Security Level Sub Field: 100 */
#define gENC_c         0x04

/* Security Level Sub Field: 101 */
#define gENC_MIC32_c   0x05

/* Security Level Sub Field: 110 */
#define gENC_MIC64_c   0x06

/* Security Level Sub Field: 111 */
#define gENC_MIC128_c  0x07

/*
  053474r17Zb - Key Id and Key Identifier sub field.
*/
/* Key Identifier sub-field: 0b00 */
#define gDataKey_c           0x00

/* Key Identifier sub-field: 0b01 */
#define gNwkKey_c            0x01

/* Key Identifier sub-field: 0b10 */
#define gKey_TransportKey_c  0x02

/* Key Identifier sub-field: 0b11 */
#define gKey_LoadKey_c       0x03
/*
  The Key Id that use the above values.
*/
typedef uint8_t zbKeyId_t;


/*
  SSP Nonce
*/
typedef struct zbNonce_tag
{
  zbIeeeAddr_t                 aSourceAddress;
  zbFrameCounter_t             FrameCounter;
  zbAuxSecurityControlField_t  AuxSecurityControlField;
}zbNonce_t;

/*****************************************************************************************
                              APS LINK SECURITY SPECIFICS
*****************************************************************************************/

/*
  053474r17Zb - Table 4.37 Elements of the Key-Pair Descriptor.
*/
typedef struct zbApsDeviceKeyPairSet_tag
{
  /*
    053474r17 Table 4.37
    Name: DeviceAddress
    Type: Any valid 64-bit address
    Description: Indetifies the address of the entity with which this key-pair is shared.
    Range: -
    Default: -
    Note: The type is change to be an index on the address map table, for size saving.
  */
  uint8_t  iDeviceAddress;

  /*
    053474r17 Table 4.37
    Name: MasterKey || LinkKey
    Type: Set fo 16 octets.
    Range: Variable.o
    Description:
  */
  zbKeyTableIndex_t  iKey;

  zbKeyType_t  keyType;

  /*
    053474r17 Table 4.37
    Name: LinkKey
    Type: Set of 16 octets
    Description: The actual value of the link key.
    Range: -
    Default: -
    Note: -
  */
  zbFrameCounter_t  OutgoingFrameCounter;

  /*
    053474r17 Table 4.37
    Name: LinkKey
    Type: Set of 16 octets
    Description: The actual value of the link key.
    Range: -
    Default: -
    Note: -
  */
  zbFrameCounter_t  IncomingFrameCounter;
}zbApsDeviceKeyPairSet_t;

/*****************************************************************************************
                                  APSME PRIMITIVE TYPES
*****************************************************************************************/
/*
  Table 4.12 KeyType Parameter of the Transport-Key Primitive
*/
typedef struct zbTrustCenterKeyData_tag
{
  /*
    Name: ParentAddress
    Type: Device address
    Range: Any vslid 64-bit address
    Descripiton: The extended 64-bit address of the parent of the destination
                 device given by the DestAddress parameter.
  */
  zbIeeeAddr_t  aParentAddress;

  /*
    Name: Key
    Type: Set of 16 octets
    Range: Variable
    Description: The trust center master or link key.
  */
  zbAESKey_t    aKey;
}zbTrustCenterKeyData_t;

/*
  Table 4.14 TransportKeyData Parameter for a Network Key
*/
typedef struct zbNetworkKeyData_tag
{
  /*
    Name: KeySeqNumber
    Type: Octet
    Range: 0x00 - 0xff
    Description: A sequence number assigned to a network key
                 by the Trust Center and used to distinguish
                 network keys for purposes of jey updates and
                 incoming frame security operations.
  */
  zbKeySeqNumber_t  keySeqNumber;

  /*
    Name: NetworkKey
    Type: Set of 16 octets
    Range: Variable
    Description: The network key.
  */
  zbAESKey_t        aNetworkKey;

  /*
    Name: UseParent
    Type: Boolean
    Range: TRUE | FALSE
    Description: This parameter indicates if the destination device's parent
                 shall be used to forward the key to the destination device:
                 TRUE = Use parent.
                 FALSE = Do not use parent.
  */
  bool_t            useParent;

  /*
    Name: ParentAddress.
    Type: Device address
    Range: Any valid 64-bit address
    Description: If the UseParent is TRUE, then ParentAddress parameter
                 shall contain the extended 64-bit address of the destiantion
                 device's parent device; otherwise, this parameter is not
                 used and need not to be set.
  */
  zbIeeeAddr_t      aParentAddress;

}zbNetworkKeyData_t;

/*
  Table 4.15 TransportKeyData Parameter for an Application Master or Link Key
*/
typedef struct zbApplicationKeyData_tag
{
  /*
    Name: ParentAddress
    Type: Device Address
    Range: Any Valid 64-bit address
    Description: The extended 64-bit address of the device that was also sent
                 this master key.
  */
  zbIeeeAddr_t  aParentAddress;

  /*
    Name: Initiator
    Type: Boolean
    Range: TRUE | FALSE
    Description: This parameter indicates if the destination device of this
                 master key request it.
                 TRUE = If the destination request the key.
                 FALSE = Otherwise.
  */
  bool_t        Initiator;

  /*
    Name: Key
    Type: Set of 16 octets
    Range: Variable
    Description: The master or link key (as indicated by the KeyType parameter).
  */
  zbAESKey_t    aKey;
}zbApplicationKeyData_t;

typedef union zbTransportKeyData_tag
{
  zbTrustCenterKeyData_t  TrustCenterKey;
  zbNetworkKeyData_t      NetworkKey;
  zbApplicationKeyData_t  ApplicationKey;
}zbTransportKeyData_t;

/*
  Table 4.11 APSME-TRANSPORT-KEY.request Parameters.
  NOTE: The orther of the data match the XML test tool form.!
*/
typedef struct zbApsmeTransportKeyReq_tag
{
    /*
    Name: KeyType
    Type: Integer
    Range: 0x00 - 0x06
    Description: Identifies the type of key material that should be
                 trasnported; see table 4.12
  */
  zbKeyType_t     KeyType;

  /*
    Name: DestAddress
    Type: Device address
    Range: Any valid 64-bit address
    Description: The extended 64-bit Address of the destination device.
  */
  zbIeeeAddr_t  aDestinationAddress;

  /*
    Name: TrasnportKeyData
    Type: Variable
    Range: Variable
    Description: The key being transported along with identification
                 and usage parameters. The type of this parameter
                 depends on the KeyType parameter as fallow:
                 See Table 4.12 KeyType Parameter of the Transport-Key Primitive.
  */
  zbTransportKeyData_t  TransportKeyData;
}zbApsmeTransportKeyReq_t;

/*
  053474r17 Sec. 4.4.7.1 APSME-SWITCH-KEY.request
  This primitive allows a device (for example, the Trust Center) to request that
  another device switch to a new active network key.
*/
typedef struct zbApsmeSwitchKeyReq_tag
{
  /*
    Name: DestAddress.
    Type: Device Address.
    Range: Any valid 64-bit address.
    Description: The extended 64-bit address of the device to which the switch-key
                 command is sent.
  */
  zbIeeeAddr_t      aDestAddress;

  /*
    Name: KeySeqNumber.
    Type: Octet.
    Range: 0x00 - 0xFF
    Description: A sequence number assigned to a network key by the Trust Center
                 and used to distinguish network keys.
  */
  zbKeySeqNumber_t  keySeqNumber;
}zbApsmeSwitchKeyReq_t;

/*
  053474r17 Sec. 4.4.4.1 APSME-UPDATE-DEVICE.request
  The ZDO shall issue this primitive when it wants to inform a device (for example,
  a Trust Center) that another device has a status that needs to be updated (for
  example, the device joined or left the network).
*/
typedef struct zbApsmeUpdateDeviceReq_tag
{
  /*
    Name: DestAddress.
    Type: Device Address.
    Range: Any valid 64-bit address.
    Description: The extended 64-bit address of the device that shall be sent the
                 update information.
  */
  zbIeeeAddr_t  aDestAddress;

  /*
    Name: DeviceAddress.
    Type: Device Address.
    Range: Any valid 64-bit address.
    Description: The extended 64-bit address of the device whose status is being updated.
  */
  zbIeeeAddr_t  aDeviceAddress;

  /*
    Name: DeviceShortAddress.
    Type: Network Address.
    Range: 0x0000 - 0xffff
    Description: The 16-bit network address of the device whose status is being updated.
  */
  zbNwkAddr_t  aDeviceShortAddress;

  /*
    Name: Status.
    Type: Integer.
    Range: 0x00 - 0x07
    Description: Indicates the updated status of the device given by the
                 DeviceAddress parameter:
                 0x00 = Standard device secured rejoin
                 0x01 = Standard device unsecured join
                 0x02 = Device left
                 0x03 = Standard device unsecured rejoin
                 0x04 = High security device secured rejoin
                 0x05 = High security device unsecured join
                 0x06 = Reserved
                 0x07 = High security device unsecured rejoin
  */
  uint8_t       status;
}zbApsmeUpdateDeviceReq_t;

/*
  053474r17 Sec. 4.4.5.1 APSME-REMOVE-DEVICE.request
  The ZDO of a device (for example, a Trust Center) shall issue this primitive when
  it wants to request that a parent device (for example, a router) remove one of its
  children from the network. For example, a Trust Center can use this primitive to
  remove a child device that fails to authenticate properly.
*/
typedef struct zbApsmeRemoveDeviceReq_tag
{
  /*
    Name: ParentAddress.
    Type: Device Address.
    Range: Any valid 64-bit address.
    Description: The extended 64-bit address of the device that is the parent of the
                 child device that is requested to be removed.
  */
  zbIeeeAddr_t aParentAddress;

  /*
    Name: ChildAddress.
    Type: Device Address.
    Range: Any valid 64-bit address.
    Description: The extended 64-bit address of the child device that is requested
                 to be removed.
  */
  zbIeeeAddr_t aChildAddress;
}zbApsmeRemoveDeviceReq_t;

/*
  053474r17 Sec. 4.4.6.1 APSME-REQUEST-KEY.request
  This primitive allows the ZDO to request either the active network key or a new
  end-to-end application key (master or link).
*/
typedef struct zbApsmeRequestKeyReq_tag
{
  /*
    Name: DestAddress.
    Type: Device Address.
    Range: Any valid 64-bit address.
    Description: The extended 64-bit address of the device to which the request-key
                 command should be sent.
  */
  zbIeeeAddr_t  aDestAddress;

  /*
    Name: KeyType.
    Type: Octet.
    Range: 0x00 - 0xFF.
    Description: The type of key being requested:
                                                 0x01 = Network key
                                                 0x02 = Application key
                                                 0x00 and 0x03-0xFF = Reserved
  */
  zbKeyType_t  keyType;

  /*
    Name: PartnerAddress.
    Type: Device Address.
    Range: Any valid 64-bit address.
    Description: If the KeyType parameter indicates an application key, this parameter
                 shall indicate an extended 64-bit address of a device that shall receive
                 the same key as the device requesting the key.
  */
  zbIeeeAddr_t  aPartnerAddress;
}zbApsmeRequestKeyReq_t;


typedef struct zbApsmeEstablishKeyReq_tag
{
  /*
    Name: Responder-Address.
    Type: Device Address.
    Range: Any valid 64-bit address-
    Description: The extended 64-bit address of the responder device.
  */
  zbIeeeAddr_t  aResponderAddress;

  /*
    Name: UseParent.
    Type: Boolean.
    Range: TRUE | FALSE.
    Description: This parameter indicates if the responder's parent shall be used to
                 forward messages between the initiator and responder devices:
                 TRUE: Use parent
                 FALSE: Do not use parent
  */
  bool_t        useParent;

  /*
    Name: Responder-Parent-Address
    Type: Device Address.
    Range: Any valid 64-bit address.
    Description: If UseParent is TRUE, then the ResponderParentAddress parameter shall
                 contain the extended 64-bit address of the responder.s parent device.
                 Otherwise, this parameter is not used and need not be set.
  */
  zbIeeeAddr_t  aParentAddress;

  /*
    Name: KeyEstablishment-Method.
    Type: Integer.
    Range: 0x00 - 0x03
    Description: The requested key-establishment method shall be one of the following:
                 0x00 = SKKE method
                 0x01 - 0x03 = reserved
  */
  uint8_t       keyEstablishmentMethod;
}zbApsmeEstablishKeyReq_t;

/* 0x00 = SKKE method */
#define gSKKE_Method_c  0x00

/*
  053474r17 Sec. - 4.4.2.4 APSME-ESTABLISH-KEY.response
  The ZDO of the responder device shall use the APSME-ESTABLISHKEY.
  response primitive to respond to an APSME-ESTABLISH-KEY.indication
  primitive. The ZDO determines whether to continue with the key establishment
  or halt it. This decision is indicated in the Accept parameter of the
  APSMEESTABLISH-KEY.response primitive.
*/
typedef struct zbApsmeEstablisKeyRsp_tag
{
  /*
    Name: InitiatorAddress.
    Type: Device Address.
    Range: Any valid 64-bit address.
    Description: The extended 64-bit address of the device that initiated key
                 establishment.
  */
  zbIeeeAddr_t  aInitiatorAddress;

  /*
    Name: Accept.
    Type: Boolean.
    Range: TRUE | FALSE
    Description: This parameter indicates the response to an initiator’s request to
                 execute a keyestablishment protocol. The response shall be either:
                 TRUE = Accept
                 FALSE = Reject
  */
  bool_t        accept;
}zbApsmeEstablisKeyRsp_t;


/*
  053474r17 Sec. 4.4.8.1 APSME-AUTHENTICATE.request
  The ZDO on an initiator or responder device will generate this primitive when it
  needs to initiate or respond to entity authentication. If the ZDO is responding to an
  APSME-AUTHENTICATE.indication primitive, it will set the RandomChallenge
  parameter to the corresponding RandomChallenge parameter in the indication.
*/
typedef struct zbApsmeAuthenticateReq_tag
{
  /*
    Name: PartnerAddress.
    Type: Device Address.
    Range: Any valid 64-bit address.
    Description: If the KeyType parameter indicates an application key, this parameter
                 shall indicate an extended 64-bit address of a device that shall receive
                 the same key as the device requesting the key.
  */
  zbIeeeAddr_t  aPartnerAddress;

/*
  Name: Action.
  Type: Enumeration.
  Range: 0x00 - 0x02.
  Description: Indicates action required
                    0x00: Initiate
                    0x01: Respond_accept
                    0x02: Respond_Reject
*/
  zbAction_t  action;

/*
  Name: RandomChallenge.
  Type: Set of 16 octets.
  Range: -.
  Description: The 16-octet random challenge
                    originally received from the
                    initiator. This parameter is only
                    valid if the Action parameter is
                    equal to RESPOND_ACCEPT.
*/
  zbAESKey_t  aRandomChanllenge;
}zbApsmeAuthenticateReq_t;

/*****************************************************************************************
                                  APSME COMMAND TYPES
*****************************************************************************************/
/*
  Table 4.34 Command Identifier Values
*/
#define  gAPS_CMD_SKKE_1_c            0x01
#define  gAPS_CMD_SKKE_2_c            0x02
#define  gAPS_CMD_SKKE_3_c            0x03
#define  gAPS_CMD_SKKE_4_c            0x04
#define  gAPS_CMD_TRANSPORT_KEY_c     0x05
#define  gAPS_CMD_UPDATE_DEVICE_c     0x06
#define  gAPS_CMD_REMOVE_DEVICE_c     0x07
#define  gAPS_CMD_REQUEST_KEY_c       0x08
#define  gAPS_CMD_SWITCH_KEY_c        0x09
#define  gAPS_CMD_EA_INIT_CHLNG_c     0x0A
#define  gAPS_CMD_EA_RSP_CHLNG_c      0x0B
#define  gAPS_CMD_EA_INIT_MAC_DATA_c  0x0C
#define  gAPS_CMD_EA_RSP_MAC_DATA_c   0x0D
#define  gAPS_CMD_TUNNEL_c            0x0E


typedef uint8_t zbApsCommandID_t;

typedef struct zbApsmeCommandFrame_tag
{
  zbApsFrameControl_t  frameControl;
  zbApsCounter_t       apsCounter;
  zbApsCommandID_t     commandID;
}zbApsmeCommandFrame_t;

/*
  0534734r17 Sec. 4.4.9.2.3.1 Trust Center Master or Link Key Descriptor Field
  If the key type field is set to 0 or 4, the key descriptor field shall be formatted as
  shown.
*/
typedef struct zbTrustCenterKeyDescriptor_tag
{
  zbIeeeAddr_t  destinationAddr;
  zbIeeeAddr_t  sourceAddr;
}zbTrustCenterKeyDescriptor_t;

/*
  053474r17 Sec. 4.4.9.2.3.2 Network Key Descriptor Field
  If the key type field is set to 1, 5 or 6, this field shall be formatted as shown.
*/
typedef struct zbNetworkKeyDescriptor_tag
{
  zbKeySeqNumber_t  sequenceNumber;
  zbIeeeAddr_t      destinationAddr;
  zbIeeeAddr_t      sourceAddr;
}zbNetworkKeyDescriptor_t;

/*
  053474r17 Sec. 4.4.9.2.3.3 Application Master and Link Key Descriptor Field
  If the key type field is set to 2 or 3, this field shall be formatted as shown.
*/
typedef struct zbApplicationKeyDescriptor_tag
{
  zbIeeeAddr_t  parentAddr;
  bool_t        initiatorFlag;
}zbApplicationKeyDescriptor_t;

/*
  053474r17 Sec. 4.4.9.2.3 Key Descriptor Field
  This field is variable in length and shall contain the actual (unprotected) value of
  the transported key along with any relevant identification and usage parameters.
  The information in this field depends on the type of key being transported (as
  indicated by the key type field . see sub-clause 4.4.9.2.2).
*/
typedef union zbKeyDescriptor_tag
{
  zbNetworkKeyDescriptor_t      nwkKey;
  zbTrustCenterKeyDescriptor_t  trustCenterKey;
  zbApplicationKeyDescriptor_t  applicationKey;
}zbKeyDescriptor_t;

/*
  053474r17 Sec. 4.4.9.2 Transport-Key Commands
  The transport-key command frame shall be formatted as illustrated in Figure 4.8.
  The optional fields of the APS header portion of the general APS frame format
  shall not be present.
*/
typedef struct zbApsmeTransportKeyCommand_tag
{
  zbKeyType_t        keyType;
  zbAESKey_t         key;
  zbKeyDescriptor_t  keyDescriptor;
}zbApsmeTransportKeyCommand_t;

/*
  053474r17 Sec. 4.4.9.3 Update Device Commands
  The APS command frame used for device updates is specified in this clause. The
  optional fields of the APS header portion of the general APS frame format shall
  not be present.
  The update-device command frame shall be formatted as illustrated in
  Figure 4.12.
*/
typedef struct zbApsmeUpdateDeviceCommand_tag
{
  /*
    Name: DestAddress.
    Type: Device Address.
    Range: Any valid 64-bit address.
    Description: The device address field shall be the 64-bit extended address of the
                 device whose status is being updated.
  */
  zbIeeeAddr_t  deviceAddr;

  /*
    Name: DeviceShortAddress
    Type: Device Address
    Range: Any valid 16-bit address.
    Description: The device short address field shall be the 16-bit network address of
                 the device whose status is being updated.
  */
  zbNwkAddr_t   deviceShortAddr;

  /*
    Name: Status.
    Type: Integer.
    Range: 0x00 - 0x07
    Description: Indicates the updated status of the device given by the
                 DeviceAddress parameter:
                 0x00 = Standard device secured rejoin.
                 0x01 = Standard device unsecured join.
                 0x02 = Device left.
                 0x03 = Standard device unsecured rejoin.
                 0x04 = High security device secured rejoin.
                 0x05 = High security device unsecured join.
                 0x06 = Reserved.
                 0x07 = High security device unsecured rejoin.
  */
  zbStatus_t    status;
}zbApsmeUpdateDeviceCommand_t;

/*
  053474r17 Sec. 4.4.9.4 Remove Device Commands
  The APS command frame used for removing a device is specified in this clause.
  The optional fields of the APS header portion of the general APS frame format
  shall not be present.
  The remove-device command frame shall be formatted as illustrated in
  Figure 4.13.
*/
typedef struct zbApsmeRemoveDeviceCommand_tag
{
  /*
    Name: Child Address.
    Type: Device address.
    Range: Any valid 64-bit address.
    Description: The child address field shall be the 64-bit extended address of the
                 device that is requested to be removed from the network.
  */
  zbIeeeAddr_t  ChildAddr;
}zbApsmeRemoveDeviceCommand_t;

/*
  053474r17 Sec. 4.4.9.6 Switch-Key Commands
  The APS command frame used by a device for switching a key is specified in this
  clause. The optional fields of the APS header portion of the general APS frame
  format shall not be present.
*/
typedef struct zbApsmeSwitchKeyCommand_tag
{
  /*
    Name: Sequence Number.
    Type: Integer.
    Range: 0x00 - 0xFF
    Description: The sequence number field shall contain the sequence number identifying
                 the network key to be made active.
  */
  zbKeySeqNumber_t  sequenceNumber;
}zbApsmeSwitchKeyCommand_t;

/*
  053474r17 Sec. 4.4.9.5 Request-Key Commands
  The APS command frame used by a device for requesting a key is specified in this
  clause. The optional fields of the APS header portion of the general APS frame
  format shall not be present.
*/
typedef struct zbApsmeRequestKeyCommand_tag
{
  /*
    Name: Key Type
    Type: Integer.
    Range: 0x00 - 0x06
    Description: The key type field shall be set to 1 when the network key is being
                 requested, shall be set to 2 when an application key is being requested,
                 and 4 when a Trust Center link key is being requested.
  */
  zbKeyType_t  keyType;

  /*
    Name: Partner Address.
    Type: Device Address.
    Range: Any valid 64-bit address.
    Description: When the key type field is 2 (that is, an application key), the partner
                 address field shall contain the extended 64-bit address of the partner
                 device that shall be sent the key. Both the partner device and the device
                 originating the request-key command will be sent the key.
                 When the key-type field is 1 (that is, a network key), the partner
                 address field will not be present.
  */
  zbIeeeAddr_t   aPartnerAddress;
}zbApsmeRequestKeyCommand_t;



typedef struct zbApsmeSKKECommand_tag
{
  /*
    Name: Initiator address.
    Type: Device Address.
    Range: Any valid 64-bit address.
    Description: The initiator address field shall be the 64-bit extended address of the
                 device that acts as the initiator in the key-establishment protocol.
  */
  zbIeeeAddr_t  aInitiatorAddress;

  /*
    Name: Responder address.
    Type: Device Address.
    Range: Any valid 64-bit address.
    Description: The responder address field shall be the 64-bit extended address of the
                 device that acts as the responder in the key-establishment protocol.
  */
  zbIeeeAddr_t  aResponderAddress;

  /*
    Name: Data Field.
    Type: Set of 16 octets.
    Range: variable.
    Description: The content of the data field depends on the command identifier field
                 (that is, SKKE-1, SKKE-2, SKKE-3, or SKKE-4).
  */
  zbAESKey_t  aData;
}zbApsmeSKKECommand_t;

typedef struct zbApsmeChallengeCommand_tag
{
  /*
    Name: Key Info
    Type: Struct.
    Description: The key info type is divided in two fields: KeyType and KeySeqNumber.
  */
  zbKeyInfo_t  keyInfo;

/*
  Name: Initiator.
  Type: Device Address.
  Range: Any valid 64-bit address.
  Description: The initiator field shall be set to the 64-bit extended address of the device that acts
                     as the initiator of the scheme.
*/
zbIeeeAddr_t   aInitiatorAddress;

/*
  Name: Responder.
  Type: Device Address.
  Range: Any valid 64-bit address.
  Description: The responder field shall be set to the 64-bit extended address of the device that
                    acts as the responder to the scheme.
*/
zbIeeeAddr_t   aResponderAddress;

/*
  Name: Challenge.
  Type: 16 byte array.
  Description: The challenge field shall be the octet representation of the challenge QEV
                     generated by the responder during action step 2 of sub-clause B.8.2.
*/
zbAESKey_t  aChallenge;
}zbApsmeChallengeCommand_t;

typedef struct zbApsmeMacDataCommand_tag
{
/*
  Name: MAC.
  Type: Device Address.
  Range: Any valid 64-bit address.
  Description: The responder field shall be set to the 64-bit extended address of the device that
                    acts as the responder to the scheme.
*/
  zbAESKey_t  aMac;
/*
  Name: Data TYpe.
  Type: int.
  Range: 0x00-0xff.
  Description: The DataType field shall be set to 0x00 to indicate the frame counter associated
                     with the active network key.
*/
  uint8_t dataType;

/*
  Name: Data.
  Type: array of 4 bytes.
  Description: The Data field shall be octet representation of the string Text2, i.e. the frame
                     counter associated with the active network key.
*/
  uint32_t  data;

}zbApsmeMacDataCommand_t;

/*
  053474r17 Sec - 4.4.9.8 Tunnel Commands
  The APS command frame used by a device for sending a command to a device that lacks the
  current network key is specified in this clause. The optional fields of the APS header
  portion of the general APS frame format shall not be present. The tunnel-key command
  frame is sent unsecured.
*/
typedef union zbApsmeTunnelData_tag
{
  zbApsmeTransportKeyCommand_t  transportkey;
}zbApsmeTunnelData_t;

typedef struct zbApsmeTunnelCommand_tag
{
  zbIeeeAddr_t      aDestinationAddress;
  zbApsmeCommandFrame_t  commandFrame;
  zbAuxSecurityControlField_t  AuxSecurityControlField;
  zbFrameCounter_t             FrameCounter;
  zbIeeeAddr_t                 aSourceAddress;
  zbApsmeTunnelData_t  tunnelCommand;
  /*
    we asume that the tunneling command will never be use with a secure level
    different than 5, so the MIC should be adjusted depnding on the secure level.
  */
  uint8_t  tunnelMIC[4];
}zbApsmeTunnelCommand_t;


typedef union zbApsmeCommandData_tag
{
  zbApsmeTransportKeyCommand_t  transportkey;
  zbApsmeUpdateDeviceCommand_t  updateDevice;
  zbApsmeRemoveDeviceCommand_t  removeDevice;
  zbApsmeSwitchKeyCommand_t     switchKey;
  zbApsmeRequestKeyCommand_t    requestKey;
  zbApsmeSKKECommand_t          skke;
  zbApsmeChallengeCommand_t     eaChallenge;
  zbApsmeMacDataCommand_t       eaMacData;
  zbApsmeTunnelCommand_t        tunnel;
}zbApsmeCommandData_t;

typedef struct zbApsmeCommand_tag
{
  zbApsmeCommandFrame_t  commandFrame;
  zbApsmeCommandData_t   commandData;
}zbApsmeCommand_t;


/*****************************************************************************************
                                  APSME INDICATION TYPES
*****************************************************************************************/
/*
  053474r17 - Table 4.17 TransportKeyData Parameter for a Trust Center Master or Link Key.
*/
typedef struct zbApsmeTrustCenterKeyInd_tag
{
  /*
    Name: TrustCenter-MasterKey.
    Type: set of 16 octets
    Range: Variable.
    Description: The trust center master or link key.
  */
  zbAESKey_t  aTrustCenterKey;
}zbApsmeTrustCenterKeyInd_t;


/*
  053474r17 - Table 4.18 TransportKeyData Parameter for a Network Key.
*/
typedef struct zbApsmeNwkKeyInd_tag
{
  /*
    Name: KeySeqNumber.
    Type: Octet.
    Range: 0x00 - 0xFF.
    Description: A sequence number assigned to a network key by the Trust Center and
    used to distinguish; network keys for purposes of key updates and incoming frame
    security operations.
  */
  zbKeySeqNumber_t  keySeqNumber;

  /*
    Name: NetworkKey.
    Type: Set of 16 octets.
    Range: Variable.
    Description: The netwrok key.
  */
  zbAESKey_t  aNetworkKey;
}zbApsmeNwkKeyInd_t;


/*
  053474r17 - Table 4.19 TransportKeyData Parameter for an Application Master or Link Key.
*/
typedef struct zbApsmeApplicationKeyInd_tag
{
  /*
    Name: ParentAddress.
    Type: Device Address.
    Range: Any Valid 64-bit address.
    Description: The extended 64-bit address of the device that was also sent this
    master key.
  */
  zbIeeeAddr_t  aParentAddress;

  bool_t        initiatorFlag;

  /*
    Name: Key.
    Type: Set of 16 octets.
    Range: Variable.
    Description: The master or link key (as indicated by the KeyType parameter).
  */
  zbAESKey_t  aApplicationKey;
}zbApsmeApplicationKeyInd_t;

typedef union zbTransportKeyIndDescriptor_tag
{
  zbApsmeTrustCenterKeyInd_t  trustCenterKey;
  zbApsmeNwkKeyInd_t          networkKey;
  zbApsmeApplicationKeyInd_t  applicationKey;
}zbTransportKeyIndDescriptor_t;

/*
  053474r17 Sec. 4.4.3.2 APSME-TRANSPORT-KEY.indication
  The APSME-TRANSPORT-KEY.indication primitive is used to inform the ZDO of the receipt
  of keying material.
*/
typedef struct zbApsmeTransportKeyInd_tag
{
  /*
    Name: SrcAddress.
    Type: Device Address.
    Range: Any valid 64-bit address.
    Description: The extended 64-bit address of the device that is the original source
                 of the transported key.
  */
  zbIeeeAddr_t    aSrcAddress;

  /*
    Name: KeyType.
    Type: Octet.
    Range: 0x00 - 0x06
    Description: Identifies the type of key material that was be transported; 053474r17
                 see Table 4.12.
  */
  zbKeyType_t  keyType;

  /*
    Name: TrasnportKeyData.
    Type: Variable.
    Range: Variable.
    Description: The key that was transported along with identification and usage
                 parameters. The type of this parameter depends on the KeyType
                 parameter as follows:
                                      KeyType = 0x00 see 053474r17 Table 4.17
                                      KeyType = 0x01 see 053474r17 Table 4.18
                                      KeyType = 0x02 see 053474r17 Table 4.20
                                      KeyType = 0x03 see 053474r17 Table 4.20
                                      KeyType = 0x04 see 053474r17 Table 4.17
                                      KeyType = 0x05 see 053474r17 Table 4.18
  */
  zbTransportKeyIndDescriptor_t  keyData;
}zbApsmeTransportKeyInd_t;

/*
  053474r17 Sec. 4.4.4.2 APSME-UPDATE-DEVICE.indication
  The APSME shall issue this primitive to inform the ZDO that it received an
  update-device command frame.
*/
typedef struct zbApsmeUpdateDeviceInd_tag
{
  /*
    Name: SrcAddress.
    Type: Devie Address.
    Range: Any Valid 64-bit address.
    Description: The extended 64-bit address of the device originating the update-device
                 command.
  */
  zbIeeeAddr_t  aSrcAddress;

  /*
    Name: DeviceAddress.
    Type: Device Address.
    Range: Any valid 64-bit address.
    Description: The extended 64-bit address of the device whose status is being updated.
  */
  zbIeeeAddr_t  aDeviceAddress;

  /*
    Name: DeviceShortAddress.
    Type: Network Address.
    Range: 0x0000 - 0xffff.
    Description: The 16-bit network address of the device whose status is being updated.
  */
  zbNwkAddr_t   aDeviceShortAddress;

  /*
    Name: Status.
    Type: Integer.
    Range: 0x00 - 0x07.
    Description: Indicates the updated status of the device given by the DeviceAddress
                 parameter:
                           0x00 = Standard device secured rejoin
                           0x01 = Standard device unsecured join
                           0x02 = Device left
                           0x03 = Standard device unsecured rejoin
                           0x04 = High security device secured rejoin
                           0x05 = High security device unsecured join
                           0x06 = Reserved
                           0x07 = High security device unsecured rejoin
  */
  zbStatus_t    status;
}zbApsmeUpdateDeviceInd_t;

/*
  053474r17 Sec. 4.4.7.2 APSME-SWITCH-KEY.indication
  The APSME shall issue this primitive to inform the ZDO that it received a switchkey
  command frame.
*/
typedef struct zbApsmeSwitchKeyInd_tag
{
  /*
    Name: SrcAddress.
    Type: Device Address.
    Range: Any Valid 64-bit address.
    Description: The extended 64-bit address of the device that sent the
                 switch-key command.
  */
  zbIeeeAddr_t  aSrcAddress;

  /*
    Name: KeySeqnumber.
    Type: Octet.
    Range: 0x00 - 0xff.
    Description: A sequence number assigned to a network key by the Trust Center
                 and used to distinguish network keys.
  */
  zbKeySeqNumber_t  keySeqNumber;
}zbApsmeSwitchKeyInd_t;

/*
  053474r17 Sec. 4.4.5.2 APSME-REMOVE-DEVICE.indication
  The APSME shall issue this primitive to inform the ZDO that it received a
  remove-device command frame.
*/
typedef struct zbApsmeRemoveDeviceInd_tag
{
  /*
    Name: SrcAddress.
    Type Device Address.
    Range: Any valid 64-bit address.
    Description: The extended 64-bit address of the device requesting that a child device
                 be removed.
  */
  zbIeeeAddr_t  aSrcAddress;

  /*
    Name: ChildAddress.
    Type: Device Address.
    Range: Any valid 64-bit address.
    Description: The extended 64-bit address of the child device that is requested to
                 be removed.
  */
  zbIeeeAddr_t  aChildAddress;
}zbApsmeRemoveDeviceInd_t;

/*
  053473r17 Sec. 4.4.6.2 APSME-REQUEST-KEY.indication
  The APSME shall issue this primitive to inform the ZDO that it received a
  request-key command frame.
*/
typedef struct zbApsmeRequestKeyInd_tag
{
  /*
    Name: SrcAddress.
    Type: Device Address.
    Range: Any valid 64-bit address.
    Description: The extended 64-bit address of the device that sent the request-key
                 command.
  */
  zbIeeeAddr_t  aSrcAddress;

  /*
    Name: KeyType.
    Type: Octet.
    Range: 0x00 - 0xFF
    Description: The type of key being requested:
                                                  0x01 = Network key
                                                  0x02 = Application key
                                                  0x00 and 0x03-0xFF = Reserved
  */
  zbKeyType_t  keyType;

  /*
    Name: ParentAddress.
    Type: Device Address.
    Range: Any valid 64- bit address.
    Description: If the KeyType parameter indicates an application key, this parameter
                 shall indicate an extended 64-bit address of a device that shall
                 receive the same key as the device requesting the key.
  */
  zbIeeeAddr_t  aParentAddress;
}zbApsmeRequestKeyInd_t;

/*
  053474r17 Sec. 4.4.2.3 APSME-ESTABLISH-KEY.indication
  The APSME in the responder shall issue this primitive to its ZDO when it receives
  an initial key-establishment message (for example, an SKKE-1 frame) from an
  initiator.
*/
typedef struct zbApsmeEstablishKeyInd_tag
{
  /*
    Name: InitiatorAddress.
    Type: Device address.
    Range: Any valid 64-bit address.
    Description: The extended 64-bit address of the initiator device.
  */
  zbIeeeAddr_t  aInitiatorAddress;

  /*
    Name: KeyEStablishmentMethod.
    Type: Integer.
    Range: 0x00 - 0x03
    Description: The requested key-establishment method shall be one of the following:
                 0x00 = SKKE method
                 0x01-0x03 = reserved
  */
  uint8_t  keyEstablishmentMethod;
}zbApsmeEstablishKeyInd_t;


/*
  053474r17 Sec. 4.4.8.3 APSME-AUTHENTICATE.indication
  The APSME in the responder device shall issue this primitive to the ZDO when a
  request to start an entity authentication is received from an initiator.
*/
typedef struct zbApsmeAuthenticateInd_tag
{
  /*
    Name: InitiatorAddress.
    Type: Device Address.
    Range: Any valid 64- bit address.
    Description: The extended, 64-bit IEEE address of the initiator device.
  */
  zbIeeeAddr_t  aInitiatorAddress;

  /*
    Name: RandomChallenge.
    Type: Set of 16 octets.
    Range: -.
    Description: The 16-octet random challenge received from the initiator.
  */
  zbAESKey_t  aRandomChanllenge;
}zbApsmeAuthenticateInd_t;


/*
  053474r17 Sec 4.4.2.2 APSME-ESTABLISH-KEY.confirm
  The APSME in both the responder and initiator devices shall issue this primitive
  to the ZDO upon completion of a key-establishment protocol.

  053474r17 Sec 4.4.8.2 APSME-AUTHENTICATE.confirm
  The APSME of the initiator or responder shall issue this primitive to its ZDO
  upon completion of entity authentication.
*/
typedef struct zbApsmeCnf_tag
{
  /*
    Name: RandomChallenge.
    Type: Set of 16 octets.
    Range: -.
    Description: The 16-octet random challenge received from the initiator.
  */
  zbIeeeAddr_t  aAddress;

  /*
    Name: RandomChallenge.
    Type: Set of 16 octets.
    Range: -.
    Description: The 16-octet random challenge received from the initiator.
  */
  zbStatus_t status;
}zbApsmeCnf_t;


/* This type is used to sent the Entity Authentication Confirm to the Nwk Layer. */
typedef zbApsmeCnf_t nlmeAuthenticationCnf_t;

  /*
    053474r17 - Table 4.10 Mapping of Symmetric-Key Key Agreement Error Conditions to
    Status Codes.
  */
  /*
    Status Description: No errors occur.
    Status Code: SUCESS
    Value: 0x00
  */
  /* NOTE: Same as regular ZigBee Success. */

  /*
    Status Description: An invalid parameter was input to one of the key establishment
                        primitives.
    Status Code: INVALID_PARAMETER
    Value: 0x01
  */
  #define gINVALID_PARAMETER_c  0x01

  /*
    Status Description: No master key is available.
    Status Code: NO_MASTER_KEY
    Value: 0x02
  */
  #define gNO_MASTER_KEY_c  0x02

  /*
    Status Description: Challenge is invalid:
                        Initiator during action step 3 (sub-clause B.7.1)
                        Responder during action step 3 (sub-clause B.7.2)
    Status Code: INVALID_CHALLENGE
    Value: 0x03
  */
  #define gINVALID_CHALLENGE_c  0x03

  /*
    Status Description: SKG outputs invalid:
                        Initiator during action step 4 (sub-clause B.7.1)
                        Responder during action step 3 (sub-clause B.7.2)
    Status Code: INVALID_SKG
    Value:0x04
  */
  #define gINVALID_SKG_c  0x04

  /*
    Status Description: MAC transformation outputs invalid:
                        Initiator during action step 8 (sub-clause B.7.1)
                        Responder during action step 10 (sub-clause B.7.2)
    Status Code: INVALID_MAC
    Value: 0x05
  */
  #define gINVALID_MAC_c  0x05

  /*
    Status Description: Tag checking transformation outputs invalid:
                        Initiator during action step 12 (sub-clause B.7.1)
                        Responder during action step 8 (sub-clause B.7.2)
    Status Code: INVALID_KEY
    Value: 0x06
  */
  #define gINVALID_KEY_c  0x06

  /*
    Status Description: Either the initiator or responder waits for an expected
                        incoming message for time greater than the apsSecurityTimeOutPeriod
                        attribute of the AIB.
    Status Code: TIMEOUT
    Value: 0x07
  */
  #define gTIMEOUT_c  0x07

  /*
    Status Description: Either the initiator or responder receives an SKKE frame out of
                        order.
    Status Code: BAD_FRAME
    Value: 0x08
  */
   #define gBAD_FRAME_c  0x08


  /*
  053474r17 Table 4.33 Mapping of Mutual Entity Authentication Error Conditions 
                       to Status Codes.
  NOTE: Status code were complemented with aprefix EA Entity Authentication since
        some of them have the same name as the SKKE Error Conditions to Status 
        Codes, but they have diferent values, so this is to avoid conflicts.
  */

  /*
    Status Description: No errors occur.
    Status Code: SUCESS
    Value: 0x00
  */
  /* NOTE: Same as regular ZigBee Success. */

  /*
    Status Description: An invalid parameter was input to one of the key establishment
                        primitives.
    Status Code:INVALID_PARAMETER
    Value: 0x01
  */
  #define gEA_INVALID_PARAMETER_c  0x01

  /*
    Status Description: No authentication key is available.
    Status Code: NO_KEY
    Value: 0x02
  */
  #define gEA_NO_KEY_c  0x02

  /*
    Status Description: No authentication data is available.
    Status Code: NO_DATA
    Value: 0x03
  */
  #define gEA_NO_DATA_c  0x03

  /*
    Status Description: Challenge is invalid:
                          Initiator during action step 2 (sub-clause B.8.1).
                          Responder during action step 1 (sub-clause B.8.2).
    Status Code: INVALID_CHALLENGE
    Value: 0x04
  */
  #define gEA_INVALLID_CHALLENGE_c  0x04

  /*
    Status Description: MAC transformation outputs invalid:
                          Initiator during action step 4 (sub-clause B.8.1).
                          Responder during action steps 4 and 7 (sub-clause B.8.2).
    Status Code: INVALID_MAC
    Value: 0x05
  */
  #define gEA_INVALLID_MAC_c  0x05

  /*
    Status Description: Tag checking transformation outputs invalid:
                          Initiator during action step 3 (sub-clause B.8.1).
                          Responder during action step 6 (sub-clause B.8.2).
    Status Code: INVALID_KEY
    Value: 0x06
  */
  #define gEA_INVALLID_KEY_c  0x06

  /*
    Status Description: The initiator or responder waits for an expected incoming
                        message for time greater than apsSecurityTimeoutPeriod.
    Status Code: TIMEOUT
    Value: 0x07
  */
  #define gEA_TIMEOUT_c  0x07

/* THIS ISTRUCTURES BELOW MAY BELONG TO ADIFFERENT FILE */

typedef uint8_t zbApsmeIndicationID_t;
typedef uint8_t zbApsmeConfirmID_t;

typedef union zbAspmeIndication_tag
{
  zbApsmeTransportKeyInd_t  zbZdoTransportKeyInd;
  zbApsmeUpdateDeviceInd_t  zbZdoUpdateDeviceInd;
  zbApsmeSwitchKeyInd_t     zbZdoSwitchKeyInd;
  zbApsmeRemoveDeviceInd_t  zbZdoRemoveDeviceInd;
  zbApsmeRequestKeyInd_t    zbZdoRequestKeyInd;
  zbApsmeEstablishKeyInd_t  zbZdoEstablishKeyInd;
  zbApsmeAuthenticateInd_t  zbZdoAuthenticateInd;
  zbApsmeCnf_t              zbZdoApsmeCnf;
  zbStatus_t                zbApsmeSecCnf;
}zbAspmeIndication_t;

typedef struct zbApsmeZdoIndication_tag
{
  zbApsmeIndicationID_t  indicationID;
  zbAspmeIndication_t     indicationData;
}zbApsmeZdoIndication_t;

/*****************************************************************************************
                                 SKKE STATE MACHINE TYPE
*****************************************************************************************/
/*
  NOTE: This type is declared here to have it accessible to the Lib and to the APP
*/
typedef struct zbKeyEstablish_tag
{
  /*
    Name: IsInitiator.
    Type: Boolean.
    Range: TRUE or FALSE.
    Description: Marks the current information set TRUE, if the curretn device is the
                 initiator device.
  */
  bool_t  isInitiator;

  /*
    Name: State.
    Type: Integer.
    Range: 0x00 - 0x04, From 0x05 to 0xff are reserved.
    Description: The state for the current SSKE procedure.
  */
  uint8_t state;

  /*
    Name: RemoteDeviceAddress.
    Type: Device Address.
    Range: Any valid 64-bit address.
    Description: The address of the remote device. This point to the secure material
                 set, is a valid 64-bit address, the reason is because we must have the
                 device registered on the secure material set at least with a master key.
  */
  zbIeeeAddr_t  aRemoteDeviceAddress;

  /*
    Name: UseParent.
    Typye: Boolean.
    Range: TURE or FALSE
    Description: Indicates if the communication during the skke procedure will be done
                 using the prent as the middle guys for the communication, or the devices
                 shall talk directly one to each other.
  */
  bool_t useParent;

  /*
    Name: ResponderParentAddress.
    Type: Device Address.
    Range: Any valid 64-bit address.
    Description: If the use parent field is set to true, this field shall contain the
                 parent IEEE address of the responder device.
  */
  zbIeeeAddr_t  aResponderPartnerAddress;

  /*
    Name: SecretData.
    Type: A set of 16 octets.
    Range: Variable.
    Description: Also known as macTag0, Secret data used with the key, this will be
                 replaced after the key derivation porcedure.
  */
  zbAESKey_t    aSecretData;

  /*
    Name: MacTag1.
    Type: A set of 16 octets.
    Range: Variable.
    Description: A unique MAC generated byt the responder device during the SKKE porcedure.
  */
  zbAESKey_t       aMacTag1;

  /*
    Name: MacTag2.
    Type: A set of 16 octets.
    Range: Variable.
    Description: A unique MAC generated byt the Initiator device during the SKKE porcedure.
  */
  zbAESKey_t       aMacTag2;

  /*
    Name: InitChallenge
    Type: A set of 16 octets.
    Range: Variable.
    Description: The challenge sent from the initiator device.
  */
  zbAESKey_t       aInitChallenge;

  /*
    Name: RespChallenge.
    Type: A set of 16 octets.
    Range: Variable.
    Description: The challenge sent from the responder device.
  */
  zbAESKey_t       aRespChallenge;

  /*
    Name: Key.
    Type: A set of 16 octets.
    Range: Variable.
    Description: The initial key used to generated the MACTag1 and MACTag2. It canbe the
                 Master key shared between two devices. It will be replaced withthe link.
  */
  zbAESKey_t    aKey;

  /*
    Name: CommandTimer
    Type: Integer
    Range 0x0000 - 0xffff
    Description: This is the amount of millisecond to wait between SKKE commands, in order
                 to avoid keep teh memory for ever if one node dies in the process.
  */
  uint8_t  SkkeTimerID;

  /*
    Name: IsFromJoin.
    Type: Boolean.
    Range: TRUE | FALSE
    Description: Tells ZDO if at the end of the process should send a transport key or not.
  */
  bool_t  sendTrasnportKey;
}zbKeyEstablish_t;


/*****************************************************************************************
                 MUTUAL SYMETRIC-KEY ETITY AUTHENTICATION MACHINE TYPE
*****************************************************************************************/
/*
  NOTE: This type is declared here to have it accessible to the Lib and to the APP
*/
typedef struct zbEntityAuthentication_tag
{
  /*
    Name: IsInitiator.
    Type: Boolean.
    Range: TRUE or FALSE.
    Description: Marks the current information set TRUE, if the curretn device is the
                 initiator device.
  */
  bool_t  isInitiator;

  /*
    Name: State.
    Type: Integer.
    Range: 0x00 - 0x04.
    Description: The state for the current Entity Authentication procedure.
  */
  uint8_t state;

  /*
    Name: PartnerAddress.
    Type: Device Address.
    Range: Any valid 64-bit address.
    Description: Address of the devices we are trying to make the Entity 
                 Authenticaion. Could it be the Responder address for the case 
                 of the Initiator or vice versa.
  */
  zbIeeeAddr_t  aPartnerAddress;

  /*
    Name: InitChallenge
    Type: A set of 16 octets.
    Range: Variable.
    Description: The challenge sent from the initiator device.
  */
  zbAESKey_t       aInitChallenge;

  /*
    Name: RespChallenge.
    Type: A set of 16 octets.
    Range: Variable.
    Description: The challenge sent from the responder device.
  */
  zbAESKey_t       aRespChallenge;

  /*
    Name: Key.
    Type: A set of 16 octets.
    Range: Variable.
    Description: The initial key used to generated the MACTag1 and MACTag2. It
                 shall be the active network key shared between the initiator 
                 and the responder.
  */
  zbAESKey_t    aMAcKey;

  /*
    Name: MacTag1.
    Type: A set of 16 octets.
    Range: Variable.
    Description: A unique MAC generated byt the responder device during the SKKE porcedure.
  */
  zbAESKey_t       aMacTag1;

  /*
    Name: MacTag2.
    Type: A set of 16 octets.
    Range: Variable.
    Description: A unique MAC generated byt the Initiator device during the SKKE porcedure.
  */
  zbAESKey_t       aMacTag2;

  /*
    Name: CommandTimer
    Type: Integer
    Range 0x0000 - 0xffff
    Description: This is the amount of millisecond to wait during the Entity 
                 Authentication process, in order to avoid keeping the memory 
                 for ever if one node dies in the process.
  */
  uint8_t  EA_TimerID;

}zbEntityAuthentication_t;
#ifdef __IAR_SYSTEMS_ICC__
#pragma pack()
#endif
#endif  /* _ZIGBEE_H_ */
