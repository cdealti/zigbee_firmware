/*****************************************************************************
* This  file is used by the Application to configure BeeStack. Normally set
* through BeeKit (called properties), but can be edited manually. A new export
* from BeeKit will overwrite this file.
*
* (c) Copyright 2008, Freescale, Inc. All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
*****************************************************************************/

#ifndef _APPLICATION_CONFIG_H_
#define _APPLICATION_CONFIG_H_



#include "EmbeddedTypes.h"


/*****************************************************************************
******************************************************************************
* Public macros
******************************************************************************
*****************************************************************************/

/*
  The default channel list defines which channels to scan when forming or joining a 
  network. Default = 0x02000000 = channel 25. The channel list is a bitmap, where each 
  bit describes a channel (for example bit 12 corresponds to channel 12). Any combination 
  of channels can be included. ZigBee supports channels 11-26.

  The default channel list may also be set over-the-air using the Startup Attribute Set 
  Cluster, or ZDP Mgmt_NWK_Update_req.

  3  2    2    2    1    1    0    0    0
  1  8    4    0    6    2    8    4    0
  0000 0000 0000 0000 0000 1000 0000 0000 = 0x00000800 = channel 11
  0000 0100 0000 0000 0000 0000 0000 0000 = 0x04000000 = channel 26
  0000 0010 0000 0000 0000 0000 0000 0000 = 0x02000000 = channel 25 (default)
  0000 0111 1111 1111 1111 1000 0000 0000 = 0x07fff800 = all channels 11-26
  0000 0000 1000 0000 0001 0000 0000 0000 = 0x00801000 = channels 23 and 12

  Default: 0x02000000
*/
#ifndef mDefaultValueOfChannel_c
  #define mDefaultValueOfChannel_c         0x00004000
#endif

/*
  The full channel list defines which channels to scan when forming or joining a network if the 
  preferred (mDefaultValueOfChannel_c) didn't work. The channel list is a bitmap, where each bit 
  describes a channel (for example bit 12 corresponds to channel 12). Any combination of channels 
  can be included. ZigBee supports channels 11-26.

  Set to 0x00000000 to indicate no full channel list should be used (will continued to use 
  preferred channel list).

  3  2    2    2    1    1    0    0    0
  1  8    4    0    6    2    8    4    0
  0000 0111 1111 1111 1111 1000 0000 0000 = 0x07fff800 = all channels 11-26
  0000 0000 1000 0000 0000 1000 0000 0000 = 0x00800800 = channels 23 and 11

  Default: 0x00000000 (better for demos and lab work)
  ZigBee Default: 0x07fff800 (all channels)
*/
#ifndef gFullChannelList_c
  #define gFullChannelList_c         0
#endif

/*
  PAN ID used to form or join the network. ZigBee PAN IDs range from 0 - 0xfffe
  (0x00,0x00, 0xff,0xfe). Set to 0xffff (0xff,0xff) to indicate pick a random
  PAN ID when forming, or pick any PAN ID to join when joining.

  This field is little endian. For example, PAN ID 0xefff is 0xff,0xfe

  Range:   0x0000 - 0xfffe, or 0xffff
  Default: 0x1aaa (0xaa,0x1a)
*/
#ifndef mDefaultValueOfPanId_c
  #define mDefaultValueOfPanId_c				   0xaa,0x1a
#endif

/*
  This property describes the extended address for this node. The extended address, also 
  called IEEE address, long address or 64-bit MAC addres, is a world-wide unique 
  identifier. Google "IEEE OUI" for a discussion of this address, and to purchase a range 
  of addresses from IEEE. The MAC address MUST be unique per node.

  Set to a specific MAC address, or use all 00s for a random MAC address. The random MAC 
  address works for trade show demos and in-lab work but must not be used for shipping 
  product.

  This, as with all multi-byte fields, is little endian:
  For example MAC (aka IEEE or extended) address 0x0050c237b0010202 would be 
  0x02,0x02,0x01,0xb0,0x37,0xc2,0x50,0x00

  Default: 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00

  Note: if extended PAN ID is set, it is recommended to set the PAN ID to 0xffff to avoid 
  PAN ID conflicts.
*/
#ifndef mDefaultValueOfExtendedAddress_c
  #define mDefaultValueOfExtendedAddress_c		0xFE,0xCA,0x00,0x00,0x00,0x00,0xFE,0xCA
#endif

/*
  This property allows nodes to be more specific about what network they form or join. If 
  set to non-zero, a node will search for this specific extended PAN ID (which often 
  defaults to the MAC address of the ZC).

  If set to all 0x00s, then the node will join any extended PAN and use the short 16-bit 
  PAN ID instead, defined by property mDefaultValueOfPanId_c.

  This field is little endian.

  Default: 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
*/
#ifndef mDefaultNwkExtendedPANID_c
  #define mDefaultNwkExtendedPANID_c 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
#endif

#ifndef mDefultApsUseExtendedPANID_c
#define mDefultApsUseExtendedPANID_c 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
#endif

/*
  ZigBee End-Device (ZED) Poll rate, in milliseconds, during authentication (part of the 
  joining process). This is usually faster than the default poll rate. Not relevant to ZC 
  or ZR devices.

  Range:   0x0000000a - 0x0003ffff (about 4 minutes)
  Default: 0x000000c8 (200ms)
*/
#ifndef mDefaultValueOfAuthenticationPollTimeOut_c
#define mDefaultValueOfAuthenticationPollTimeOut_c  200
#endif

/*
  After a ZigBee End-Device (ZED) recieves data from it's parent, how quickly does it 
  poll the parent again for data? This makes it easy for a ZED to receive multiple 
  packets quickly. Value in milliseconds.

  Range:   0x0000000a - 0x0003ffff (about 4 minutes)
  Default: 0x000000c8 (200ms)
*/
#ifndef mDefaultValueOfConfirmationPollTimeOut_c
#define mDefaultValueOfConfirmationPollTimeOut_c  200
#endif

/*
  How long should ZigBee End-Device (ZED) wait to receive a key from its parent before 
  assuming the join failed authentication?

  Default: 0x00001388 (5000 ms = 5 seconds)
*/
#ifndef mDefaultValueOfAuthTimeOutDuration_c
#define mDefaultValueOfAuthTimeOutDuration_c     5000
#endif

/*
  How quickly does a ZigBee End-Device (ZED) poll its parent under normal conditions. 
  This poll rate might increase when joining, or sending/receiving fragmented packets.

  See also: mDefaultValueOfAuthenticationPollTimeOut_c
            mDefaultValueOfConfirmationPollTimeOut_c
            gApsFragmentationPollTimeOut_c
  
  Use 0xFA0 for 4 seconds, 0xBB8 for 3 seconds, 0x3E8 for 1 second.

  Range:   0x0000000a - 0x0003ffff (about 4 minutes)
  Default: 0x00000BB8 (3 seconds)
*/
#ifndef mDefaultValueOfIndirectPollRate_c
#define mDefaultValueOfIndirectPollRate_c            3000
#endif

/*
  Set RxOnWhenIdle TRUE for mains-powered (non-sleeping) end-devices. Set this
  FALSE for sleeping end-devices. When FALSE, end-devices will poll their
  parent for messages. See mDefaultValueOfIndirectPollRate_c for the polling timeout.

  Default: FALSE
*/
#ifndef gRxOnWhenIdle_d
#define gRxOnWhenIdle_d           FALSE
#endif

/*
  The value to use as Nwk key, in standard or high security, if the value is set to all
  zeros, then, the stack will consider is self as not-perconfigured and the trust center
  will generate one randomly.
  If the value set is differnt from all zeros, then, the stack will consider it self as
  preconfigured and use the specified value as the preconfigured key.

  Default: 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x89,0x67,0x45,0x23,0x01,0xef,0xcd,0xab
*/
#ifndef mDefaultValueOfNetworkKey_c
#define mDefaultValueOfNetworkKey_c  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
#endif

/* Default value of Network Active Key Sequence Number. Always start at 0. */
#ifndef mDefaultValueOfNwkActiveKeySeqNumber_c
#define mDefaultValueOfNwkActiveKeySeqNumber_c    0x00
#endif

/*
  Default trust center master key. If set, this key will be used as the trust center master
  key. If mDefaultValueOfTrustCenterLinkKey_c is set the value of this field will not be
  used.

  An all zeros field means that the master key is not specified, and it will be generated
  randomly, also means that the first thing the TC willdo is send the new generated key
  to the joining device.
*/
#ifndef mDefaultValueOfTrustCenterMasterKey_c
#define mDefaultValueOfTrustCenterMasterKey_c  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
#endif

/*
  Default trust center link key. If set, this will be the link with the trust center. If
  this parameters is set to somethign different than zero, then the master key is no needed.

  An all zeros field here means that the SKKE procedure will be used to generate a TC link key.
*/
#ifndef mDefaultValueOfTrustCenterLinkKey_c
#define mDefaultValueOfTrustCenterLinkKey_c  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
#endif

/*
  The default key type to answer to a Request key when answering 0x02, which means
  Application Master key or application link key.

  0x02 = gApplicationMasterKey_c
  0x03 = gApplicationLinkKey_c

  Default: 0x02.
*/
#ifndef mDefaultValueOfApplicationKeyType_c
  #define mDefaultValueOfApplicationKeyType_c  2
#endif

/*
  Default value of the Trust Center IEEE address.
  Can be set when the TC is know but it will be changed by the transport key.
*/
#ifndef mDefaultValueOfTrustCenterLongAddress_c
  #define mDefaultValueOfTrustCenterLongAddress_c  0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa
#endif

/*
  The default period of time a device will wait for an expected security protocol frame
  (in milliseconds).
*/
#ifndef mDefaultApsSecurityTimeOutPeriod_c
  #define mDefaultApsSecurityTimeOutPeriod_c  4488
#endif

/*
  This property allows non-secure (NWK laye secure packets) APS data packets on a secure
  network. If this option is true, the network is not ZigBee compliant (and is not truly
  a secure network). Has no effect if security is disabled.

  Default: FALSE
*/
#ifndef gAllowNonSecure_d
  #define gAllowNonSecure_d     FALSE
#endif

/*
  The Ieee filter allows a router or coordinator to refuse to allow other nodes to join if the
  IEEE address does not match the filter criteria. Set both the gIeeeFilterMask_c and 
  gIeeeFilterValue_c properties. Little endian.
  
  Set gIeeeFilterMask_c to all 0x00s to disable the filter (default).
  Set to 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff to filter on OUI (top 3 bytes).
  
  See ValidateIeeeAddress() in AppStackImpl.c
*/
#ifndef gIeeeFilterMask_c
#define gIeeeFilterMask_c       0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
#endif

/*
  The Ieee filter allows a router or coordinator to refuse to allow other nodes to join if the
  IEEE address does not match the filter criteria. Set both the gIeeeFilterMask_c and 
  gIeeeFilterValue_c properties. Little endian.

  To filter based on OUI 0x123456, set the filter mask and value as follows:
  
  gIeeeFilterMask_c:   0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff
  gIeeeFilterValue_c:  0x00, 0x00, 0x00, 0x00, 0x00, 0x56, 0x34, 0x12
  
  See ValidateIeeeAddress() in AppStackImpl.c
*/
#ifndef gIeeeFilterValue_c
#define gIeeeFilterValue_c      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
#endif


/*
  How long before end-device-bind times out? End-device-bind is enabled (only
  on ZigBee coordinators - ZC) by enabling both gEnd_Device_Bind_rsp_d and
  gBindCapability_d. Examples:

  0x7530 = 30000 = 30 seconds (default)
  0x2710 = 10000 = 10 seconds

  Range:   0x0000000a - 0x0003ffff
  Default: 0x00002710 (10000ms = 10 seconds)
*/
#ifndef mDefaultValueOfEndDeviceBindTimeOut_c
  #define mDefaultValueOfEndDeviceBindTimeOut_c   30000
#endif


/*
  Security level, as defined by the ZigBee specification, table 4.38. Note: to be 
  compliant, choose security level 5. All others do not confirm to ZigBee profiles.
  ZigBee always uses a 128-bit key for encryption.

  0 = no security
  1 = no encryption, 32-bit MIC (no encryption, but authentication)
  2 = no encryption, 64-bit MIC  
  3 = no encryption, 128-bit MIC 
  4 = encryption, no MIC (encrypted but no authentication)
  5 = encryption, 32-bit MIC (default) 
  6 = encryption, 64-bit MIC
  7 = encryption, 128-bit MIC 

  Default: 5
*/
#ifndef mDefaultValueOfNwkSecurityLevel_c
  #define mDefaultValueOfNwkSecurityLevel_c   5
#endif

/*
  Default value of permit join (ZC and ZR devices only). 

  0xff:       Always a child can join this Device
  0x00:       Device is not allowing any child to join
  0x01-0xfe:  Device is allowing the child to join for the specified value in seconds

  Default: 0xff
*/
#ifndef mDefaultValueOfPermitJoinDuration_c
  #define mDefaultValueOfPermitJoinDuration_c   0xFF
#endif

/* 
  Allows an application to decide if ZigBee Pro networks use multicast (NWK layer groups) 
  or groupcast (APS layer groups). Has no effect on ZigBee 2007 (non-Pro) networks. The 
  application may alternatively use:
  
  NlmeSetRequest(gNwkUseMulticast_c, FALSE) 

  to set this at run-time.
  
  TRUE = multicast occurs at the network layer.
  FALSE= groupcast occurs at the APS layer and using the APS header.

  Default:  TRUE
*/
#ifndef mDefaultValueOfNwkUseMulticast_c
  #define mDefaultValueOfNwkUseMulticast_c  TRUE
#endif

/* 
  Allows an application to decide if ZigBee Pro networks that has multicast (NWK layer groups)
  enabled to send out groupcast packets instead of multicasts. This allows for processing of multicast
 frames in the network layer but not utilizing them for its own groupcasts (they will be regular nwk broadcasts).
  
  TRUE= to use multicast groups instead of groupcast.
  FALSE = to use groupcast instead of multicast groups
  
  Only applicaple if mDefaultValueOfNwkUseMulticast_c is TRUE and ZigBee Pro is used.

  Default:  TRUE
*/
#ifndef mDefaultUseMultiCastForApsGroupDataRequest_c
  #define mDefaultUseMultiCastForApsGroupDataRequest_c  TRUE
#endif



/*
  Node descritor field.
  Set to 0x40 to indicate 2.4GHz band. Do not change.
  Note: does not affect stack, only what is reported by the ZDP Node Descriptor.

  Default: 0x40
*/
#ifndef mDefaultValueOfApsFlagsAndFreqBand_c
  #define mDefaultValueOfApsFlagsAndFreqBand_c        0x40
#endif

/*
  The manufacturer code is a 16-bit number obtained by each OEM in the ZigBee Alliance to 
  uniquely identify that manufacture. This value is returned in response to a Node 
  Descriptor Request.

  Note: does not affect stack, only what is reported by the ZDP Node Descriptor.

  Default: 0x210 (0x10, 0x02)  
*/
#ifndef mDefaultValueOfManfCodeFlags_c
  #define mDefaultValueOfManfCodeFlags_c            0x10,0x02
#endif

/*
  Indicates the power source when a node receives the power descriptor (ZDP command).
  Note: does not affect the stack, only what is reported by the power descriptor.

  0 = mains powered
  1 = rechargeable battery
  2 = disposable battery

  Default: 0
*/
#ifndef gPowerSource_d
#define gPowerSource_d          0
#endif


/*

  This value indicates how large of a asdu (application payload, including fragmentation) 
  can be received by this node. 
  Note: does not affect the stack, only what is reported by the power descriptor.

  Range:   0x0000 - 0x7fff
  Default: 0x003c (60 bytes)
*/
#ifndef mDefaultValueOfMaxTransferSize_c
  #define mDefaultValueOfMaxTransferSize_c            0x3c,0x00
#endif

/*
  Indicates if this devices is mains powered or not in Node Descriptor
  Note: does not affect the stack, only what is reported by the power descriptor.

  0x00 - not mains powered
  0x10 - mains powered

  Default: 0x10
*/
#ifndef mDefaultValueOfCurrModeAndAvailSources_c
  #define mDefaultValueOfCurrModeAndAvailSources_c  	0x10
#endif

/*
  The lower 8-bits of the Node Power Descriptor.
  Note: does not affect the stack, only what is reported by the power descriptor.

  0x00 - Constant (mains powered)
  0x01 - Rechargable battery
  0x02 - Disposable battery

  - ORed with -

  0x00 - Battery at critical
  0x40 - battery at 33%
  0x80 - battery at 66%
  0xC0 - battery at 100%

  Default: 0xc1
*/
#ifndef mDefaultValueOfCurrPowerSourceAndLevel_c
  #define mDefaultValueOfCurrPowerSourceAndLevel_c    0xC1
#endif

/*
  Sets the default behavior of the stack wehn the node does a Leave, when the flag is set
  to TRUE, and the value for Nwk Extended Pan Id is differnte than all Zeros value, it
  will do a silent nwk rejoin if rejoin is required. Other wise it will do a Nwk Rejoin.

  Defualt: TRUE
*/
#ifndef gZdoSmartRejoin_c
#define gZdoSmartRejoin_c  TRUE
#endif

/*
  The amount of active scans and/or energy scans to perform on each network discovery attempt

  See [R17], Table 1.48 :Config_NWK_Scan_Attempts
      [ZCL Commissioning Cluster], ScanAttempts
      NlmeGetRequest(gNwkScanAttempts_c)

  Range:   0x01 - 0xff
  Default: 0x03
*/
#ifndef mDefaultValueOfNwkScanAttempts_c
  #define mDefaultValueOfNwkScanAttempts_c        3
#endif

/*
  Milliseconds between two scans when forming or joining a network. This is not the time between
  network discovery, but the time between active scans during a single network discovery.

  See [R17] :Config_NWK_Time_btwn_Scans
      [ZCL Commissioning Cluster], TimeBetweenScans
      NlmeGetRequest(gNwkTimeBtwnScans_c)

  Range:   0x00001 - 0xffff (about 1 minute)
  Default: 0x0064 (100 = 0.1 seconds)
  REF. doc 064699r12ZB Commissioning cluster section 6.2.2.2.2
*/
#ifndef mDefaultValueOfNwkTimeBwnScans_c
  #define mDefaultValueOfNwkTimeBwnScans_c            100
#endif


/*
  How often in seconds a ZED will try to rejoin the network after it has left it. This is
  also used for ZCs and ZRs to determine how long to wait between form/join attempts.

  This is the period to retry to join the network, if for some reason it left the network.

  See [R17] :Config_Rejoin_Interval
      [ZCL Commissioning Cluster], RejoinInterval
      NlmeGetRequest(gRejoinInterval_c)

  Range:   0x0000 - 0xFFFF (less than MaxRejoinInterval)
  Default: 0x0002 (better for demos and lab work)
  ZigBee Default: 0x003c (60 seconds)
*/
#ifndef mDefaultValueOfRejoinInterval_c
  #define mDefaultValueOfRejoinInterval_c 2
#endif

/*
  Upper bound on Rejoin Interval (OrphanScanAttemptsTimeOut), in seconds.
  This is the maximum value that mDefaultValueOfRejoinInterval_c can take.

  See [R17] :Config_MaxRejoinInterval
      [ZCL Commissioning Cluster], MaxRejoinInterval
      NlmeGetRequest(gMaxRejoinInterval_c)

  Range:   0x0000 - 0xFFFF
  Default: 0x0e10
*/
#ifndef mDefaultValueOfMaxRejoinInterval_c
  #define mDefaultValueOfMaxRejoinInterval_c 3600
#endif

/*
  How many times to attempt to discovery a network for joining? The ZDO layer will give up
  attempting to join by either the # of attempts or the discovery timeout, whichever comes
  first. See also mDefaultValueOfDiscoveryAttemptsTimeOut_c.

  Valid for ZRs and ZEDs.

  Set to 0x00 to mean try forever.

  Range:   0x00 - 0xff attempts
  Default: 0x00
*/
#ifndef mDefaultValueOfNwkDiscoveryAttempts_c
  #define mDefaultValueOfNwkDiscoveryAttempts_c   0
#endif

/*

  How long to keep trying to join a network? That is, how many total seconds before giving up? Will 
  give up based on time or time of # attemps, whichever comes first.

  See also mDefaultValueOfNwkDiscoveryAttempts_c.

  Valid for ZRs and ZEDs.

  Set to 0x0000 to mean try forever. 

  Range:   0x0000 - 0xffff seconds (about 18.2 hours)
  Default: 0x0000
*/
#ifndef mDefaultValueOfDiscoveryAttemptsTimeOut_c
  #define mDefaultValueOfDiscoveryAttemptsTimeOut_c		0
#endif


/*
  How many times to scan until a suitable free channel/PAN ID is found. 0x00 is forever.

  Valid for ZCs only.

  Range:   0x00 - 0xff
  Default: 0x00
*/
#ifndef mDefaultValueOfNwkFormationAttempts_c
  #define mDefaultValueOfNwkFormationAttempts_c   0
#endif

/*
  How long to keep trying to join a network? That is, how many total seconds before giving up? Will 
  give up based on time or time of # attemps, whichever comes first.

  See also mDefaultValueOfNwkFormationAttempts_c.
  
  Valid for ZCs only.

  Set to 0x0000 to mean try forever. 

  Range:   0x0000 - 0xffff seconds (about 18.2 hours)
  Default: 0x0000
*/
#ifndef mDefaultValueOfFormationAttemptsTimeOut_c
  #define mDefaultValueOfFormationAttemptsTimeOut_c   0
#endif

/*
  How many times to scan when attempting to rejoin a network. 0x00 is forever.

  Valid for ZRs and ZEDs

  Range:   0x00 - 0xff
  Default: 0x00
*/
#ifndef mDefaultValueOfNwkOrphanScanAttempts_c
  #define mDefaultValueOfNwkOrphanScanAttempts_c  0
#endif 

/*
  How long to keep trying to rejoin a network? That is, how many total seconds before giving up? Will 
  give up based on time or time of # attemps, whichever comes first.

  See also mDefaultValueOfNwkOrphanScanAttempts_c.

  Set to 0x0000 to mean try forever. 

  Range:   0x0000 - 0xffff seconds (about 18.2 hours)
  Default: 0x0000
*/
#ifndef mDefaultValueOfOrphanScanAttemptsTimeOut_c
  #define mDefaultValueOfOrphanScanAttemptsTimeOut_c	0
#endif


/*
  Should the addresses in the neighbor table be reused if a device fails to report in?
  This is normally false to allow for moving ZEDs to move from parent to parent within
  the network.

  Default: FALSE
*/
#ifndef mDefaultReuseAddressPolicy_c
  #define mDefaultReuseAddressPolicy_c				    FALSE
#endif

/*
  
  Value for Battery Life Extention.

  Default: FALSE
*/
#ifndef mDefaultValueOfBatteryLifeExtension_c
  #define mDefaultValueOfBatteryLifeExtension_c 	FALSE
#endif

/*
  nwkSecureAllFrames in the NIB. If so, then frames are decrypted and re-encrypted per 
  hop (which allows authentication). If not, then frames are only secured at either end. 
  Must be TRUE to be ZigBee compatible.

  Default: TRUE
*/
#ifndef mDefaultValueOfNwkSecureAllFrames_c
#define mDefaultValueOfNwkSecureAllFrames_c			TRUE
#endif

/*
  Is Low Power Managment enabled or not? Only relevant to ZED devices. Enable for battery 
  operated systems that will go into low power.

  Default: FALSE
*/
#ifndef mDefaultValueOfLpmStatus_c
  #define mDefaultValueOfLpmStatus_c  		      FALSE
#endif

/* 
  If this is set to 0x00, the NWK layer shall calculate link cost from all
  neighbor nodes using the LQI values reported by the MAC layer. Otherwise it
  shall report a constant value.

  Range:   0x00 - 0x07
  Default: 0x00
*/
#ifndef mNwkRptConstantCost_c
#define  mNwkRptConstantCost_c									0
#endif

/* 
  The number of retries allowed after an unsuccessful route request.

  Range:   0x01 - 0x07
  Default: 0x03
*/
#ifndef mNwkRouteDiscRetriesPermitted_c
#define  mNwkRouteDiscRetriesPermitted_c        3
#endif

/* 
  A flag that determines whether the NWK layer should assume the ability to use tree 
  routing. Stack profile 0x01 (aka ZigBee 2007) uses Tree. Stack profile 0x02 (aka ZigBee 
  Pro) does not.

  TRUE  assume the ability to use tree routing (Stack profile 0x01)
  FALSE never use hierarchical routing (Stack profile 0x02)
*/
#ifndef mNwkUseTreeRouting_c
#define mNwkUseTreeRouting_c                  TRUE
#endif

/* 
  Only relevant if mNwkAddressAlloc_c is FALSE.
  
  The next network address that will be assigned to a device requesting
  association. This value shall be incremented by gDefaultNwkAddressIncrement_c
  every time an address is assigned. Little endian.

  Range:   0x0000 - 0xfff0
  Default: 0x0000 (0x00,0x00)
*/
#ifndef mDefaultNwkNextAddress_c
#define mDefaultNwkNextAddress_c              0x00,0x00
#endif

/* 
  Only relevant if mNwkAddressAlloc_c is FALSE.
  
  The size of remaining block of addresses to be assigned. This value will
  be decremented by 1 every time an address is assigned. When this attribute
  has a value of 0, no more associations may be accepted.

  Range:   0x0000 - 0xfff0
  Default: 0x0000 (0x00,0x00)
*/
#ifndef mDefaultNwkAvailableAddress_c
#define mDefaultNwkAvailableAddress_c         0x00,0x00
#endif

/*
  Only relevant if mNwkAddressAlloc_c is FALSE.

  The amount by which gDefaultNwkNextAddress_c is incremented each time an
  address is assigned.

  Range:   0x0001 - 0xfff0
  Default: 0x0001 (0x01,0x00)
*/
#ifndef mDefaultNwkAddressIncrement_c
#define mDefaultNwkAddressIncrement_c         0x01,0x00
#endif

/*
  The 16-bit address that the device uses to communicate with the PAN. This attribute 
  reflects the value of the MAC PIB attribute macShortAddress any changes made by the 
  higher layer will be reflected in the MAC PIB attribute value as well. Set to 0xff,0xff 
  to indicate the node will receive its address from its parent. Set only if silent
  joining.
  
  Set as a byte array, little endian (e.g. 0x1234 is 0x34,0x12).

  Default: 0xffff
  Range:   0x0000 - 0xfff1
*/
#ifndef mDefaultNwkShortAddress_c
  #define mDefaultNwkShortAddress_c             0xFF,0xFF
#endif

/*
  The version of the ZigBee protocol currently in use by the NWK layer.
  Must be 0x02. Do not change.

  Default: 0x02
*/
#ifndef mNwkProtocolVersion_c
  #define mNwkProtocolVersion_c 0x02
#endif

/* 
  The default value for the logical channel. Set to 0x00 to allow the node to pick a 
  channel from the channel list. Used only if silent joining.

  Range:   0x00, 11-26
  Default: 0x00
*/
#ifndef mDefaultLogicalChannel_c
  #define mDefaultLogicalChannel_c 0x00
#endif

/* 
  The default value of current device's parent address, little endian. Set to 0xffff to 
  find parent. Set to 0x0000 - 0xfff0 to set to a specific parent.

  Range:   0x0000 - 0xfff0
  Default: 0xffff (0xff,0xff)
*/
#ifndef mDefaultParentShortAddress_c
#define mDefaultParentShortAddress_c 0xff,0xff
#endif

/* 
  The default value of current device's parent long address, little endian.
  A default of all 0xffs means not defined until the device joins the network.
  Set to anything else to allow the node to choose a particular parent by
  preference.

  Range:   Any valid MAC (IEEE) address
  Default: 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff
*/
#ifndef mDefaultParentLongAddress_c
#define mDefaultParentLongAddress_c 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff
#endif


/*
  This is the default value for field count.This macro indiactes how many
  fields are included in the complex descriptor
*/
#ifndef mDefaultValueOfComplexDescFieldCount_c
#define mDefaultValueOfComplexDescFieldCount_c      0x07
#endif

/*
  This macro gives the default values for Language code and charecter set in Complex 
  descriptor.This is of 3 bytes length.The first byte says the XML code for language And 
  character set, followed by the language code and character set identifier.

  Note: does not change stack behavior, only the fields in the Complex Descriptor.

  Default: 0x01, 0x41, 0x41, 0x00
*/
#ifndef mDefaultValueOfComplexDescLangCharSet_c
#define mDefaultValueOfComplexDescLangCharSet_c     0x01,0x45,0x4E,0x00
#endif

/*
  default value of manufacturer name in complex descriptor.

  The first byte gives the XML tag code, number of ASCII characters and the actual value 
  Note: does not change stack behavior, only the fields in the Complex Descriptor.

  Default: 0x02,0x04,0x42,0x42,0x42,0x42
*/
#ifndef mDefaultValueOfComplexDescManufactureName_c
#define mDefaultValueOfComplexDescManufactureName_c 0x02,0x04,0x42,0x42,0x42,0x42
#endif

/*
  default value of model name in complex descriptor.

  The first byte gives the XML tag code, number of ASCII characters and the actual value 
  Note: does not change stack behavior, only the fields in the Complex Descriptor.

  Default: 0x03,0x04,0x43,0x43,0x43,0x43
*/
#ifndef mDefaultValueOfComplexDescModelName_c
#define mDefaultValueOfComplexDescModelName_c       0x03,0x04,0x43,0x43,0x43,0x43
#endif

/*
  default value of serial number in complex descriptor.
  
  The first byte gives the XML tag code, number of ASCII characters and the actual value
  Note: does not change stack behavior, only the fields in the Complex Descriptor.

  Default: 0x04,0x04,0x31,0x32,0x33,0x34
*/
#ifndef mDefaultValueOfComplexDescSerialNumber_c
#define mDefaultValueOfComplexDescSerialNumber_c    0x04,0x04,0x31,0x32,0x33,0x34
#endif

/*
  default value of device URL in complex descriptor.
  
  The first byte gives the XML tag code, number of ASCII characters and the actual value
  Note: does not change stack behavior, only the fields in the Complex Descriptor.

  Default: 0x05,0x0F,0x32,0x35,0x35,0x2E,0x32,0x35,0x35,0x2E,0x32,0x35,0x35,0x2E,0x32,0x35,0x35
*/
#ifndef mDefaultValueOfComplexDescDeviceUrl_c
#define mDefaultValueOfComplexDescDeviceUrl_c       0x05,0x0F,0x32,0x35,0x35,0x2E,0x32,0x35,0x35,0x2E,0x32,0x35,0x35,0x2E,0x32,0x35,0x35
#endif

/*
  default value of descriptor Icon in complex descriptor.
  
  The first byte gives the XML tag code, number of ASCII characters and the actual value
  Note: does not change stack behavior, only the fields in the Complex Descriptor.

  Default: 0x06,0x02,0xdd,0xdd
*/
#ifndef mDefaultValueOfComplexDescIcon_c
#define mDefaultValueOfComplexDescIcon_c            0x06,0x02,0xdd,0xdd
#endif

/*
  Default value of Descriptor Icon URL in complex descriptor.

  The first byte gives the XML tag code, number of ASCII characters and the actual value
  Note: does not change stack behavior, only the fields in the Complex Descriptor.

  Default: 0x07,0x07,0x30,0x2E,0x30,0x2E,0x30,0x2E,0x30
*/
#ifndef mDefaultValueOfComplexDescIconUrl_c
#define mDefaultValueOfComplexDescIconUrl_c         0x07,0x07,0x30,0x2E,0x30,0x2E,0x30,0x2E,0x30
#endif

/*
  Default value of the times in which a ZC or ZR can generate a network address for the
  device who attempt to join to it. This address doesn't exists in any entry of its local tables. 
  If a network address cannot be generated within these retries, the node gives up and will
  not join the network.

  Range:   1 - 255
  Default: 3
*/
#ifndef gDefaultRetriesStochasticAddrLocalDevice_c
#define gDefaultRetriesStochasticAddrLocalDevice_c     3
#endif

/*
  The number of times a parent will try to assign a random address to a joining child. If 
  the parent receives an address conflict this many times, it will give up and disallow 
  the new node to join.
   
  Range:   1 - 255
  Default: 3
*/
#ifndef gDefaultRetriesStochasticAddrNetworkWide_c
#define gDefaultRetriesStochasticAddrNetworkWide_c     3
#endif


/*****************************************************************************************
                                FA Configurable Options.
*****************************************************************************************/

/*
  The amount of routers to select by default from the NT to be use during the Frequency 
  Agility scanning procedure. This only has affect on the network manager node (usually 
  the ZC).

  Range:   1 - 255
  Default: 5
*/
#ifndef  gMinNumberOfRouters_c
#define  gMinNumberOfRouters_c                 5
#endif

/*
  The maximum number of reports before initiating the Frequency Agility procedure. This 
  only has affect on the network manager node (usually the ZC).

  Range:    1 - 255
  Default:  5
*/
#ifndef  gMaxIncomingErrorReports_c
#define  gMaxIncomingErrorReports_c            5
#endif

/*
  The number of minutes to wait between reports to the network manager that this node has 
  reached the maximum # of errors. This has effect on ZRs only.

  Range:   1 - 255
  Default: 120
*/
#ifndef  gMaxTimeoutForIncomingErrorReports_c
#define  gMaxTimeoutForIncomingErrorReports_c  120
#endif

/*
  The limit on the Transmitions failure tolerance. Expressed in percentage.

  Range:   1 - 100
  Default: 25
*/
#ifndef  gMaxTxFailuresPercentage_c
#define  gMaxTxFailuresPercentage_c            25
#endif

/*
  The number of minutes to wait between reports to be send to the network manager 
  when the node has reached the maximum # of errors. This has effect on ZRs only.
  According to the spec Annex E no more than 4 times in a hour.
*/

#ifndef gTimeBeforeNextNwkUpdateNotify_c
#define gTimeBeforeNextNwkUpdateNotify_c       15
#endif

/*******************************************
*             Values for AIB               *
********************************************/

/*
  See [R1] apsDesignatedCoordinator in Table 2.24, AIB
  Set to TRUE (1) if the device should become the ZigBee Coordinator on startup, FALSE if
  otherwise. This flag is only relevant for a ZigBee device that can become a router or
  coordinator on startup.

  Not currently support. Set to FALSE.
  
  Default: FALSE
*/
#ifndef gApsDesignatedCoordinatorDefault_c
#define gApsDesignatedCoordinatorDefault_c            FALSE
#endif

/*
  See [R1] apsNonMemberRadius in Table 2.24, AIB
  The value to be used for the NonmemberRadius parameter when using NWK layer multicast.
  
  This value is only relevant for ZigBee Pro (as ZigBee 2007 does not support multicast).

  Range:   0 - 7 
  Default: 2
*/
#ifndef gApsNonMemberRadiusDefault_c
#define gApsNonMemberRadiusDefault_c                  2
#endif

/*
  See [R1] apsUseInsecureJoin in Table 2.24, AIB
  A flag controlling insecure join at startup. Specifies if the rejoin process should
  be secured or unsecured.
  
  This feature is only relevant for ZigBee Pro. 

  Default: TRUE
*/
#ifndef gApsUseInsecureJoinDefault_c
#define gApsUseInsecureJoinDefault_c                  TRUE
#endif

/*
  See [R1] apsInterframeDelay in Table 2.24, AIB
  Delay (in milliseconds) between frames when transmitting fragmented packets.

  Note: only used if Fragmentation enabled (gFragmentationCapability_d) in 
  BeeStackConfiguration.h

  Range:   0 - 255
  Default: 100
*/
#ifndef gApsInterframeDelayDefault_c
#define gApsInterframeDelayDefault_c                  100
#endif

/*
  See [R1] apscMaxWindowSize in table 2.23 APS Constants

  This is the # of fragments in a single window. Fragmentation uses a sliding window 
  approach to deliverying fragmented messages.

  Note: only used if Fragmentation enabled (gFragmentationCapability_d) in 
  BeeStackConfiguration.h

  Range:   1 - 8, 0 means not capable.
  Default: 1
*/
#ifndef gApsWindowSizeDefault_c
#define gApsWindowSizeDefault_c                       1
#endif

/*
  Maximum size of each fragment over the air in fragmented APSDE-DATA.request 
  transmissions. Each fragment is the same size, with the exception of the final fragment 
  which may be short. Use ApsmeGetMaxAsduLength() macro if setting this field at 
  run-time. See also ApsmeSetRequest(gApsMaxFragmentLength_c, val)

  Note: only used if Fragmentation enabled (gFragmentationCapability_d) in 
  BeeStackConfiguration.h

  Range:   0x01 - 0x4e (depending on security mode).
  Default: 0x3c
*/
#ifndef gApsMaxFragmentLengthDefault_c
#define gApsMaxFragmentLengthDefault_c               60
#endif

/*
  This value is used for increasing the poll rate during the reception of fragmented packets  
  Default value: 50 miliseconds

  Note: only used if Fragmentation enabled (gFragmentationCapability_d) in 
  BeeStackConfiguration.h

  Range:   0 - 0x3ffff (about 4 minutes)
  Default: 50
*/
#ifndef gApsFragmentationPollTimeOut_c
#define gApsFragmentationPollTimeOut_c               50
#endif


/*
  See [R1] apsChannelTimer in Table 2.24 AIB
  
  Timer that counts down to 0 to indicate whether it is OK to change channels. Units in 
  hours. Only relevant to the Channel Manager. Valid values to set this are 0-24. 0 Turns 
  off the channel timer and allows Frequency Agility to change channels.

  Range:   1 - 255
  Default: 24
*/
#ifndef gApsChannelTimerDefault_c
#define gApsChannelTimerDefault_c                    24
#endif

/*
  Sets the default scan duration time for frequency agility. This is an exponential 
  scale, as seen in the 802.15.4 specification.

  Range:   0x01 - 0x07
  Default: 0x05
*/
#ifndef gDefaultScanDuration_c
#define gDefaultScanDuration_c                       5
#endif

/*
  Sets the default scan count for frequency agility

  Range:   0x01 - 0xff
  Default: 0x01
*/
#ifndef gDefaultScanCount_c
#define gDefaultScanCount_c                          1
#endif

/*
  Sets the default extra time window for frequency agility beyond the scan time per channel.
  In milliseconds.

  Range: 
  Default: 2000 (2 seconds)
*/
#ifndef gExtraTimeWindow_c
#define gExtraTimeWindow_c                           2000
#endif

/*
  The short address of the network manager (where to send frequency agility reports)

  Default: 0x0000
  Range: 0x0000 - 0xfff1
*/
#ifndef gNwkManagerShortAddr_c
#define gNwkManagerShortAddr_c                       0x00,0x00
#endif

/*
  Determine the radius for many-to-one route discovery from a concentrator (gateway).
  Limited to 11 if the concentrator will use source routes (high-RAM concentrator). 

  Default: 5
  Range: 1 - 30
*/
#ifndef gConcentratorRadius_c
#define gConcentratorRadius_c                        5
#endif

/*
  Determine the amount of time (in seconds) between many-to-one route discoveries. Defaults to 0x00, 
  which means the time is managed by the application. See Table 3.44 (NIB Attributes), 
  nwkConcentratorDiscoveryTime, for details.
  
  Default: 0
  Range: 0x00 - 0xff
*/
#ifndef gConcentratorDiscoveryTime_c
#define gConcentratorDiscoveryTime_c                 0
#endif

/*
  Maximum number of entries for the Permissions Configuration Table, a zero value means that
  the Permissions Configuration Table is not available. This value must be less or equal to
  the maximum number of entries for the Address Map.

  Default: 0x00
  Range:   0x00 - gApsMaxAddrMapEntries_c
*/
#ifndef gApsMaxEntriesForPermissionsTable_c
#define gApsMaxEntriesForPermissionsTable_c          0
#endif


/*
  Stop mode to use if this node receives an over-the-air NLME-LEAVE command. See
  ZdoStopMode_t in ApsZdoInterface.h. Note: the restart bit is chosen by the
  over-the-air command, but whether to announce or reset tables is chosen by the
  application. See also ZDO_StopEx().
  Also, the stop mode determines if the changes into the tables must be saved into
  NVM or not.

  BBBx xxxx   Reset binding and group tables, node type, pan ID, etc..

  B..x xxxx - This bit controls if the data will be saved on NVM when stops after leaving
  .B.x xxxx - This bit controls if the device should re-start when stops after leaving.
  ..Bx xxxx - This bit commands if the tables (APS, NWK) will be reset to factory defualts
              when it stops after leaving, this can be used in convination with saving
              into NVM to clear NVM.

  Default: 0x80  (Don't reset tables but save on NVM when stop, Dont restart)

  NOTE: The bits marked with an 'X' are forbidden to be set by the user.
*/
#ifndef gZdoStopModeDefault_c
#define gZdoStopModeDefault_c                        0x80
#endif

/*
  Start mode to use if this node receives an over-the-air NLME-LEAVE command. See
  ZdoStopMode_t in ApsZdoInterface.h. Note: the restart bit is chosen by the
  over-the-air command, but whether to announce or reset tables is chosen by the
  application. See also ZDO_StartEx().

  DDDx xxxx   - Device type
  xxxS Sxxx   - Startup set
  xxxx xCCC   - Startup control mode

  Default: 0x02 (NVM set, gZdoStartMode_NwkRejoin_c)
*/
#ifndef gZdoStartModeDefault_c
#define gZdoStartModeDefault_c                       0x02
#endif


/*****************************************************************************
******************************************************************************
* Public type definitions
******************************************************************************
*****************************************************************************/

/*None*/

/*****************************************************************************
******************************************************************************
* Public prototypes
******************************************************************************
*****************************************************************************/

/*None*/

#endif _APPLICATION_CONFIG_H_

