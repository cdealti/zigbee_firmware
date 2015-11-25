/************************************************************************************
* This header file is provided as part of the interface to the Freescale 802.15.4
* MAC and PHY layer.
*
* The file covers the interface to the MLME and MCPS.
*
* (c) Copyright 2007, Freescale, Inc.  All rights reserved.
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
************************************************************************************/

#ifndef _NWK_MAC_INTERFACE_H_
#define _NWK_MAC_INTERFACE_H_

#include "EmbeddedTypes.h"
//#include "AppToMacPhyConfig.h"
#include "TS_Interface.h"
#include "TMR_Interface.h"
#include "ZigBee.h"
#include "ZtcInterface.h"
/************************************************************************************
*************************************************************************************
* Public type definitions
*************************************************************************************
************************************************************************************/


#define aMaxScanResults         10  
#define aScanResultsPerBlock    5   // Must be at least 2 - align with gMmBlockSize3_c


//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
// Events to the MLME main task when running with task scheduler
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------

enum {
  gMacEventNwkMlmeMsgQueued_c = 1 << 0, /* A message from NWK to MLME was queued */
  gMacEventNwkMcpsMsgQueued_c = 1 << 1, /* A message from NWK to MCPS was queued */
  gMacEventMemMlmeMsgQueued_c = 1 << 2, /* A message from MEM to MLME was queued */
  gMacEventMemAspMsgQueued_c  = 1 << 3, /* A message from MEM to ASP was queued  */
  gMacEventPanIdConflict_c    = 1 << 4, /* SEQ signals that a PAN Id Conflict has occured */
  gMacEventGtsActionRequired_c= 1 << 5, /* MEM or MLME that a Gts Action is required in the main task */
  gMacEventBeaconCntrUpdated_c= 1 << 6  /* MEM indicates that the beacon counter has been updated */
};


//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
// General constants used in MAC primitives
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------

  // Address modes used by mlmeAssociateReq_t, mlmePollReq_t, mcpsDataReq_t, 
  // nwkCommStatusInd_t, mcpsDataInd_t, and panDescriptor_t
enum {
  gAddrModeNone_c  = 0,
  gAddrModeShort_c = 2,
  gAddrModeLong_c  = 3,
  gScanModeFastED_c   = 4
};

  // Logical channel used by mlmeAssociateReq_t, mlmeStartReq_t
  // mlmeSyncReq_t, and panDescriptor_t.
enum {
  gLogicalChannel11_c = 11,
  gLogicalChannel12_c = 12,
  gLogicalChannel13_c = 13,
  gLogicalChannel14_c = 14,
  gLogicalChannel15_c = 15,
  gLogicalChannel16_c = 16,
  gLogicalChannel17_c = 17,
  gLogicalChannel18_c = 18,
  gLogicalChannel19_c = 19,
  gLogicalChannel20_c = 20,
  gLogicalChannel21_c = 21,
  gLogicalChannel22_c = 22,
  gLogicalChannel23_c = 23,
  gLogicalChannel24_c = 24,
  gLogicalChannel25_c = 25,
  gLogicalChannel26_c = 26
};

  // Used for the parameters securityUse, and 
  // securityEnable in relevant primitives.
enum {
  gSecurityDisabled_c = 0,
  gSecurityEnabled_c  = 1
};

  // The capability info is an array of 8 bits. The following constants are
  // the bits that can be set or examined in the capability info field.
enum {
  gCapInfoAltPanCoord_c = 1<<0, // Unit has the capability of becoming a PAN coordinator
  gCapInfoDeviceFfd_c   = 1<<1, // Unit is an FFD.
  gCapInfoPowerMains_c  = 1<<2, // Unit is mains-powered, and not battery-powered.
  gCapInfoRxWhenIdle_c  = 1<<3, // Receiver is on when unit is idle.
  gCapInfoSecurity_c    = 1<<6, // Unit can send/receive secured MAC frames.
  gCapInfoAllocAddr_c   = 1<<7  // Ask coordinator to allocate short address during association.
};

  // Scan Modes
enum {
  gScanModeED_c       = 0,
  gScanModeActive_c   = 1,
  gScanModePassive_c  = 2,
  gScanModeOrphan_c   = 3
};

  // Superframe specification bit fields
enum {
  gSuperFrameSpecLsbBO_c            = 0x0F,
  gSuperFrameSpecLsbSO_c            = 0xF0,
  gSuperFrameSpecMsbFinalCapSlot_c  = 0x0F,
  gSuperFrameSpecMsbBattlifeExt_c   = 0x10,
  gSuperFrameSpecMsbReserved_c      = 0x20,
  gSuperFrameSpecMsbPanCoord_c      = 0x40,
  gSuperFrameSpecMsbAssocPermit_c   = 0x80
};

  // TX option bit fields
enum {
  gTxOptsAck_c      = 1<<0,
  gTxOptsGts_c      = 1<<1,
  gTxOptsIndirect_c = 1<<2,
  gTxOptsSecurity_c = 1<<3
};

  // GTS Characteristics field masks
enum {
  gGtsLength_c    = 0x0F, // Number of superframe slots requested for the GTS
  gGtsDirection_c = 0x10, // Clr: Transmit slot only, Set: Receive slot only.
  gGtsType_c      = 0x20  // Clr: Deallocate GTS slot, Set: Allocate GTS slot.
};

  // Security Suites
enum {
  // Security suite 0 - no security operations are performed.
  gSecSuiteNoSecurity_c   = 0x00,
  // Security suite 1-7 - various levels of security.
  gSecSuiteAesCtr_c       = 0x01,
  gSecSuiteAesCcm128_c    = 0x02,
  gSecSuiteAesCcm64_c     = 0x03,
  gSecSuiteAesCcm32_c     = 0x04,
  gSecSuiteAesCbcMac128_c = 0x05,
  gSecSuiteAesCbcMac64_c  = 0x06,
  gSecSuiteAesCbcMac32_c  = 0x07,
  // The value 0x08 signifies that no security suite is used
  gSecSuiteNone_c         = 0x08
};

  // Beacon, and Superframe orders (T = (960*(2^Order))*16E-6) 
enum {
  gOrder000s015ms_c = 0,  //  15.36ms
  gOrder000s031ms_c = 1,  //  30.72ms
  gOrder000s061ms_c = 2,  //  61.44ms
  gOrder000s123ms_c = 3,  // 122.88ms
  gOrder000s246ms_c = 4,  // 245.76ms
  gOrder000s492ms_c = 5,  // 491.52ms
  gOrder000s983ms_c = 6,  // 0.98304sec
  gOrder001s966ms_c = 7,  // 1.96608sec
  gOrder003s932ms_c = 8,  // 3.93216sec
  gOrder007s864ms_c = 9,  // 7.86432sec
  gOrder015s729ms_c = 10, // 15.72864sec
  gOrder031s457ms_c = 11, // 31.45728sec
  gOrder062s915ms_c = 12, // 1min, 2.91456sec
  gOrder125s829ms_c = 13, // 2min, 5.82912sec
  gOrder251s658ms_c = 14, // 4min, 11.65824sec
  gOrderNone_c      = 15  // No beacon
};

  // MAC PIB attributes
enum {
    // Vendor specific attributes
  gMPibRole_c                           = 0x20,
  gMPibLogicalChannel_c                 = 0x21,
  gMPibTreemodeStartTime_c              = 0x22,
  gMPibPanIdConflictDetection_c         = 0x23,
  gMPibBeaconResponseDenied_c           = 0x24,  
  gMPibNBSuperFrameInterval_c           = 0x25,
  gMPibBeaconResponseLQIThreshold_c     = 0x27,
    // General MAC PIB attributes
  gMPibAckWaitDuration_c                = 0x40,
  gMPibAssociationPermit_c              = 0x41,
  gMPibAutoRequest_c                    = 0x42,
  gMPibBattLifeExt_c                    = 0x43,
  gMPibBattLifeExtPeriods_c             = 0x44,
  gMPibBeaconPayload_c                  = 0x45,	
  gMPibBeaconPayloadLength_c            = 0x46,
  gMPibBeaconOrder_c                    = 0x47,
  gMPibBeaconTxTime_c                   = 0x48,
  gMPibBsn_c                            = 0x49,
  gMPibCoordExtendedAddress_c           = 0x4A,
  gMPibCoordShortAddress_c              = 0x4B,
  gMPibDsn_c                            = 0x4C,
  gMPibGtsPermit_c                      = 0x4D,
  gMPibMaxCsmaBackoffs_c                = 0x4E,
  gMPibMinBe_c                          = 0x4F,
  gMPibPanId_c                          = 0x50,
  gMPibPromiscuousMode_c                = 0x51,
  gMPibRxOnWhenIdle_c                   = 0x52,
  gMPibShortAddress_c                   = 0x53,
  gMPibSuperFrameOrder_c                = 0x54,
  gMPibTransactionPersistenceTime_c     = 0x55,
    // Security specific MAC PIB attributes
  gMPibAclEntryDescriptorSet_c          = 0x70,
  gMPibAclEntryDescriptorSetSize_c      = 0x71,
  gMPibDefaultSecurity_c                = 0x72,
  gMPibDefaultSecurityMaterialLength_c  = 0x73,
  gMPibDefaultSecurityMaterial_c        = 0x74,
  gMPibDefaultSecuritySuite_c           = 0x75,
  gMPibSecurityMode_c                   = 0x76,
    // Vendor specific. Used for selecting current
    // entry in ACL descriptor table.
  gMPibAclEntryCurrent_c                = 0x77,
    // The following attributes are mapped to elements 
    // in the gMPibAclEntryDescriptorSet_c attribute.
  gMPibAclEntryExtAddress_c             = 0x78,
  gMPibAclEntryShortAddress_c           = 0x79,
  gMPibAclEntryPanId_c                  = 0x7A,
  gMPibAclEntrySecurityMaterialLength_c = 0x7B,
  gMPibAclEntrySecurityMaterial_c       = 0x7C,
  gMPibAclEntrySecuritySuite_c          = 0x7D
};

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
// Primitives in the MLME to NWK direction 
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
enum {
  gNwkAssociateInd_c,
  gNwkAssociateCnf_c,
  gNwkDisassociateInd_c,
  gNwkDisassociateCnf_c,
  gNwkBeaconNotifyInd_c,
  gNwkGetCnf_c,
  gNwkGtsInd_c,
  gNwkGtsCnf_c,
  gNwkOrphanInd_c,
  gNwkResetCnf_c,
  gNwkRxEnableCnf_c,
  gNwkScanCnf_c,
  gNwkCommStatusInd_c,
  gNwkSetCnf_c,
  gNwkStartCnf_c,
  gNwkSyncLossInd_c,
  gNwkPollCnf_c,
  gNwkErrorCnf_c,
  gNwkBeaconStartInd_c,           // Proprietart primitive : Beacon start indication
  gNwkMaintenanceScanCnf_c,       // Proprietary primitive : Tree mode maintenance scan confirm
  gNwkPollNotifyInd_c,            // Proprietary primitive : Poll notify indication
  gNwkMaxPrimitives_c
};
typedef uint8_t primMlmeToNwk_t;

#pragma pack(1)
  // PAN descriptor. Used in Scan Confirm,
  // and Beacon Notfiy Indication.
typedef struct panDescriptor_tag {
  uint8_t  coordAddress[8];
  uint8_t  coordPanId[2];
  uint8_t  coordAddrMode;
  uint8_t  logicalChannel;
  bool_t   securityUse;
  uint8_t  aclEntry;
  bool_t   securityFailure;
  uint8_t  superFrameSpec[2];
  bool_t   gtsPermit;
  uint8_t  linkQuality;
  uint8_t  timeStamp[3];
} panDescriptor_t;

typedef struct msgTxFragmentedHdr_tag {
  uint8_t iMsgType;
  struct msgTxFragmentedHdr_tag * pNextDataBlock;
  uint8_t iDataSize;
} msgTxFragmentedHdr_t;

/* note: make sure this structure matches zbApsdeDataReq_t (+ payload) */
typedef struct msgApsdeDataReq_tag {
	uint8_t dstAddrMode;
	uint8_t dstAddr[8];
	uint8_t dstEndPoint;
	uint8_t aProfileId[2];
	uint8_t aClusterId[2];
	uint8_t srcEndPoint;
	uint8_t asduLength;
	uint8_t *pAsdu;
	uint8_t txOptions;
	uint8_t radiusCounter;
        zbApsConfirmId_t  confirmId;     
        msgTxFragmentedHdr_t fragmentHdr;
  uint8_t payload[8];
} msgApsdeDataReq_t;

  // Security material used in ACL entries
typedef struct securityMaterial_tag {
  uint8_t key[16];
  uint8_t frameCounter[4];
  uint8_t keySeqCounter;
  uint8_t optFrameCounter[4];
  uint8_t optKeySeqCounter;
} securityMaterial_t;


  // ACL entry descriptor. Can be used for e.g. MLME-GET/SET operations.
typedef struct aclEntryDescriptor_tag {
  uint8_t aclExtendedAddress[8];      // 64 bit addr of the device in this ACL entry
  uint8_t aclShortAddress[2];         // 16 bit addr of the device in this ACL entry
  uint8_t aclPanId[2];                // PAN ID of the device in this ACL entry
  uint8_t aclSecurityMaterialLength;  // Number of bytes in 'aclSecurityMaterial' (<=26)
  uint8_t aclSecurityMaterial[sizeof(securityMaterial_t)]; // Key for protecting frames
  uint8_t aclSecuritySuite;           // Security suite used for the device in this ACL entry.
} aclEntryDescriptor_t;


  // Type: gNwkAssociateInd_c
typedef struct nwkAssociateInd_tag {
  uint8_t  deviceAddress[8];
  bool_t   securityUse;
  uint8_t  AclEntry;
  uint8_t  capabilityInfo;
} nwkAssociateInd_t;

  // Type: gNwkAssociateCnf_c
typedef struct nwkAssociateCnf_tag {
  uint8_t  assocShortAddress[2];
  uint8_t  status;
} nwkAssociateCnf_t;

  // Type: gNwkDisassociateInd_c
typedef struct nwkDisassociateInd_tag {
  uint8_t  deviceAddress[8];
  bool_t   securityUse;
  uint8_t  aclEntry;
  uint8_t  disassociateReason;
} nwkDisassociateInd_t;

  // Type: gNwkDisassociateCnf_c
typedef struct nwkDisassociateCnf_tag {
  uint8_t  status;
} nwkDisassociateCnf_t;

  // Type: gNwkBeaconNotifyInd_c,
typedef struct nwkBeaconNotifyInd_tag {
  uint8_t  bsn;
  uint8_t  pendAddrSpec;
  uint8_t  sduLength;
  uint8_t  *pAddrList;   // Points to address list in buffer
  panDescriptor_t *pPanDescriptor;  // Points to pan descripter in buffer
  uint8_t  *pSdu;        // Points to beacon payload in buffer
  void     *pBufferRoot; // Upper layer must free this buffer before freeing the indication message,
                         // E.g. MSG_Free(pBeaconInd->pBufferRoot); MSG_Free(pBeaconInd);
} nwkBeaconNotifyInd_t;

// Type: gNwkPollNotifyInd_c,
typedef struct nwkPollNotifyInd_tag {
  uint8_t  srcAddrMode;
	uint8_t  srcAddr[8];
	uint8_t  srcPanId[2];
} nwkPollNotifyInd_t;

  // Type: gNwkGetCnf_c - not used,
typedef struct nwkGetCnf_tag {
  uint8_t  status;
  uint8_t  pibAttribute;
  uint8_t  *pibAttributeValue;
} nwkGetCnf_t;

  // Type: gNwkGtsInd_c,
typedef struct nwkGtsInd_tag {
  uint8_t  devAddress[2];
  bool_t   securityUse;
  uint8_t  AclEntry;
  uint8_t  gtsCharacteristics;
} nwkGtsInd_t;

  // Type: gNwkGtsCnf_c,
typedef struct nwkGtsCnf_tag {
  uint8_t  status;
  uint8_t  gtsCharacteristics;
} nwkGtsCnf_t;

  // Type: gNwkOrphanInd_c,
typedef struct nwkOrphanInd_tag {
  uint8_t  orphanAddress[8];
  bool_t   securityUse;
  uint8_t  AclEntry;
} nwkOrphanInd_t;

  // Type: gNwkResetCnf_c - not used,
typedef struct nwkResetCnf_tag {
  uint8_t  status;
} nwkResetCnf_t;

  // Type: gNwkRxEnableCnf_c,
typedef struct nwkRxEnableCnf_tag {
  uint8_t  status;
} nwkRxEnableCnf_t;


struct panDescriptorBlock_tag {  
  panDescriptor_t descriptorList[aScanResultsPerBlock];
  uint8_t descriptorCount;
  struct panDescriptorBlock_tag *pNext;  
};

typedef struct panDescriptorBlock_tag panDescriptorBlock_t;
  


  // Type: gNwkScanCnf_c,
typedef struct nwkScanCnf_tag {
  uint8_t  status;
  uint8_t  scanType;
  uint8_t  resultListSize;
  uint8_t  unscannedChannels[4];
  union {  
    uint8_t *pEnergyDetectList;                  // pointer to 16 byte static buffer    
    panDescriptorBlock_t *pPanDescriptorBlocks; // this one must be freed by the upper layer    
  } resList;
} nwkScanCnf_t;

  // Type: gNwkCommStatusInd_c,
typedef struct nwkCommStatusInd_tag {
  uint8_t  srcAddress[8];
  uint8_t  panId[2];
  uint8_t  srcAddrMode;
  uint8_t  destAddress[8];
  uint8_t  destAddrMode;
  uint8_t  status;
} nwkCommStatusInd_t;

  // Type: gNwkSetCnf_c - not used, 
typedef struct nwkSetCnf_tag {
  uint8_t  status;
  uint8_t  pibAttribute;
} nwkSetCnf_t;

  // Type: gNwkStartCnf_c,
typedef struct nwkStartCnf_tag {
  uint8_t  status;
} nwkStartCnf_t;

  // Type: gNwkSyncLossInd_c,
typedef struct nwkSyncLossInd_tag {
  uint8_t  lossReason;
} nwkSyncLossInd_t;

  // Type: gNwkPollCnf_c,
typedef struct nwkPollCnf_tag {
  uint8_t  status;
} nwkPollCnf_t;

  // Type: gNwkErrorCnf_c,
typedef struct nwkErrorCnf_tag {
  uint8_t  status;
} nwkErrorCnf_t;

  // Type: gNwkBeaconStartInd_c,
typedef struct nwkBeaconStartInd_tag {
  uint8_t  source;
} nwkBeaconStartInd_t;

  // Type: gNwkMaintenanceScanCnf_c,
typedef struct nwkMaintenanceScanCnf_tag {
  uint8_t  status;
} nwkMaintenanceScanCnf_t;

  // MLME to NWK message
typedef struct nwkMessage_tag {
  primMlmeToNwk_t msgType;
  union {
    nwkAssociateInd_t       associateInd;
    nwkAssociateCnf_t       associateCnf;
    nwkDisassociateInd_t    disassociateInd;
    nwkDisassociateCnf_t    disassociateCnf;
    nwkBeaconNotifyInd_t    beaconNotifyInd;
    nwkGetCnf_t             getCnf;         // Not used
    nwkGtsInd_t             gtsInd;
    nwkGtsCnf_t             gtsCnf;
    nwkOrphanInd_t          orphanInd;
    nwkResetCnf_t           resetCnf;       // Not used
    nwkRxEnableCnf_t        rxEnableCnf;
    nwkScanCnf_t            scanCnf;
    nwkCommStatusInd_t      commStatusInd;
    nwkSetCnf_t             setCnf;         // Not used
    nwkStartCnf_t           startCnf;
    nwkSyncLossInd_t        syncLossInd;
    nwkPollCnf_t            pollCnf;
    nwkErrorCnf_t           errorCnf;       // Test framework primitive.
    nwkBeaconStartInd_t     beaconStartInd;
    nwkMaintenanceScanCnf_t maintenanceScanCnf;
    nwkPollNotifyInd_t      pollNotifyInd;
    nlmeNetworkDiscoveryReq_t networkDiscoveryReq;
  } msgData;
} nwkMessage_t;


//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
// Primitives in the MCPS to NWK direction 
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------

  // Primitives in the MAC to NWK direction. All should be included no matter which
  // features are implemented as the primitives are used to interface to Nwk.
enum {
  gMcpsDataCnf_c,
  gMcpsDataInd_c,
  gMcpsPurgeCnf_c,
  gMcpsPromInd_c,  // Promiscuous mode "packet received indication"
  gMcpsToNwkMaxPrimitives_c
};
typedef uint8_t primMcpsToNwk_t;

  // Type: gMcpsDataCnf_c,
typedef struct mcpsDataCnf_tag {
  uint8_t  msduHandle;
  uint8_t  status;
} mcpsDataCnf_t;

  // Type: gMcpsDataInd_c,
typedef struct mcpsDataInd_tag {
  uint8_t  dstAddr[8];    // First 0/2/8 bytes is the address as defined by dstAddrMode
  uint8_t  dstPanId[2];   // 16 bit word converted to little endian byte array
  uint8_t  dstAddrMode;
  uint8_t  srcAddr[8];    // First 0/2/8 bytes is the address as defined by srcAddrMode
  uint8_t  srcPanId[2];   // 16 bit word converted to little endian byte array
  uint8_t  srcAddrMode;
  uint8_t  msduLength;    // 0-102 (101?)
  uint8_t  mpduLinkQuality;
  bool_t   securityUse; 
  uint8_t  aclEntry;
  uint8_t  *pMsdu;        // Data will start at this address inside the message.
} mcpsDataInd_t;

  // Type: gMcpsPurgeCnf_c,
typedef struct mcpsPurgeCnf_tag {
  uint8_t  msduHandle;
  uint8_t  status;
} mcpsPurgeCnf_t;

  // Type: gMcpsPromInd_c
typedef struct mcpsPromInd_tag {
  uint8_t  mpduLinkQuality;
  zbClock32_t timeStamp;
  uint8_t  msduLength;    // 5-121 (shortest frame is ACK frame. Longest frame is max length MAC frame)
  uint8_t  *pMsdu;        // Data will start at this address inside the message.
} mcpsPromInd_t;

  // MCPS to NWK message
typedef struct mcpsToNwkMessage_tag {
  primMcpsToNwk_t msgType;
  union {
    mcpsDataCnf_t   dataCnf;
    mcpsDataInd_t   dataInd;
    mcpsPurgeCnf_t  purgeCnf;
    mcpsPromInd_t   promInd;
    void            *dummyAlign;	// Used for aligning union, so that mcpsToNwkMessage_t may be cast to nwkMessage_t (in PassMacMessageUp of PTC)
  } msgData;
} mcpsToNwkMessage_t;


//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
// Primitives in the NWK to MLME direction 
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------

  // Primitives in the NWK to MAC direction. These must
  // always exist even if the feature is not supported.
  // The NWK layer may have a copy of these enums, and
  // does not know if we disable some of them here.
enum {
  gMlmeAssociateReq_c,
  gMlmeAssociateRes_c,
  gMlmeDisassociateReq_c,
  gMlmeGetReq_c,
  gMlmeGtsReq_c,
  gMlmeOrphanRes_c,
  gMlmeResetReq_c,
  gMlmeRxEnableReq_c,
  gMlmeScanReq_c,
  gMlmeSetReq_c,
  gMlmeStartReq_c,
  gMlmeSyncReq_c,
  gMlmePollReq_c,
  gMlmeMaintenanceScanReq_c,    // Proprietary primitive : Tree mode maintenance scan request
  gMlmeInternalPanConflictDetected_c,
  gMlmeMaxPrimitives_c
};
typedef uint8_t primNwkToMlme_t;

  // Type: gMlmeAssociateReq_c
typedef struct mlmeAssociateReq_tag {
  uint8_t  coordAddress[8];
  uint8_t  coordPanId[2];
  uint8_t  coordAddrMode;
  uint8_t  logicalChannel;
  bool_t   securityEnable;
  uint8_t  capabilityInfo;
} mlmeAssociateReq_t;

  // Type: gMlmeAssociateRes_c
typedef struct mlmeAssociateRes_tag {
  uint8_t  deviceAddress[8];
  uint8_t  assocShortAddress[2];
  bool_t   securityEnable;
  uint8_t  status;
} mlmeAssociateRes_t;

  // Type: gMlmeDisassociateReq_c
typedef struct mlmeDisassociateReq_tag {
  uint8_t  deviceAddress[8];
  bool_t   securityEnable;
  uint8_t  disassociateReason;
} mlmeDisassociateReq_t;

  // Type: gMlmeGetReq_c,
  // Note: Only macBeaconPayloadLength number of
  // bytes is returnd if getting macBeaconPayload.
typedef struct mlmeGetReq_tag {
  uint8_t  pibAttribute;
  uint8_t  *pibAttributeValue;  // Not in spec. 
} mlmeGetReq_t;

  // Type: gMlmeGtsReq_c,
typedef struct mlmeGtsReq_tag {
  bool_t   securityEnable;
  uint8_t  gtsCharacteristics;
} mlmeGtsReq_t;

  // Type: gMlmeOrphanRes_c,
typedef struct mlmeOrphanRes_tag {
  uint8_t  orphanAddress[8];
  uint8_t  shortAddress[2];
  bool_t   securityEnable;
  bool_t   associatedMember;
} mlmeOrphanRes_t;

  // Type: gMlmeResetReq_c,
typedef struct mlmeResetReq_tag {
  bool_t   setDefaultPib;
} mlmeResetReq_t;

  // Type: gMlmeRxEnableReq_c,
typedef struct mlmeRxEnableReq_tag {
  bool_t   deferPermit;
  uint8_t  rxOnTime[3];
  uint8_t  rxOnDuration[3];
} mlmeRxEnableReq_t;

  // Type: gMlmeScanReq_c,
typedef struct mlmeScanReq_tag {
  uint8_t  scanType;
  uint8_t  scanChannels[4];
  uint8_t  scanDuration;
} mlmeScanReq_t;

  // Type: gMlmeSetReq_c,
  // Note: macBeaconPayloadLength must have been
  // set before setting macBeaconPayload.
typedef struct mlmeSetReq_tag {
  uint8_t   pibAttribute;
  uint8_t   *pibAttributeValue;
} mlmeSetReq_t;

  // Type: gMlmeStartReq_c,
typedef struct mlmeStartReq_tag {
  uint8_t  panId[2];
  uint8_t  logicalChannel;
  uint8_t  beaconOrder;
  uint8_t  superFrameOrder;
  bool_t   panCoordinator;
  bool_t   batteryLifeExt;
  bool_t   coordRealignment;
  bool_t   securityEnable;
} mlmeStartReq_t;

  // Type: gMlmeSyncReq_c,
typedef struct mlmeSyncReq_tag {
  uint8_t  logicalChannel;
  bool_t   trackBeacon;
} mlmeSyncReq_t;

  // Type: gMlmePollReq_c,
typedef struct mlmePollReq_tag {
  uint8_t  coordAddress[8];
  uint8_t  coordPanId[2];
  uint8_t  coordAddrMode;
  bool_t   securityEnable;
} mlmePollReq_t;

  // NWK to MLME message
typedef struct mlmeMessage_tag {
  primNwkToMlme_t msgType;
  union {
    mlmeAssociateReq_t    associateReq;
    mlmeAssociateRes_t    associateRes;
    mlmeDisassociateReq_t disassociateReq;
    mlmeGetReq_t          getReq;
    mlmeGtsReq_t          gtsReq;
    mlmeOrphanRes_t       orphanRes;
    mlmeResetReq_t        resetReq;
    mlmeRxEnableReq_t     rxEnableReq;
    mlmeScanReq_t         scanReq;
    mlmeSetReq_t          setReq;
    mlmeStartReq_t        startReq;
    mlmeSyncReq_t         syncReq;
    mlmePollReq_t         pollReq;
  } msgData;
} mlmeMessage_t;


//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
// Primitives in the NWK to MCPS direction 
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------

  // Primitives in the NWK to MAC direction. All should be included no matter which
  // features are implemented as the primitives are used to interface to Nwk.
enum {
  gMcpsDataReq_c,
  gMcpsPurgeReq_c,
  gNwkToMcpsMaxPrimitives_c
};
typedef uint8_t primNwkToMcps_t;

  // Type: gMcpsDataReq_c,
typedef struct mcpsDataReq_tag {
  uint8_t  dstAddr[8];  // First 0/2/8 bytes is the address as defined by dstAddrMode
  uint8_t  dstPanId[2]; // 16 bit word converted to little endian byte array
  uint8_t  dstAddrMode;
  uint8_t  srcAddr[8];  // First 0/2/8 bytes is the address as defined by srcAddrMode
  uint8_t  srcPanId[2]; // 16 bit word converted to little endian byte array
  uint8_t  srcAddrMode;
  uint8_t  msduLength;  // 0-102
  uint8_t  msduHandle; 
  uint8_t  txOptions;
  uint8_t  *pMsdu;     //  Data will start at this address
} mcpsDataReq_t;

  // Type: gMcpsPurgeReq_c,
typedef struct mcpsPurgeReq_tag {
  uint8_t  msduHandle;
} mcpsPurgeReq_t;

  // NWK to MCPS message
typedef struct nwkToMcpsMessage_tag {
  primNwkToMcps_t msgType;
  union {
    mcpsDataReq_t         dataReq;
    mcpsPurgeReq_t        purgeReq;
    void *                dummyAlign; // Used for alignment with mlmeGenericMsg_t
  } msgData;
} nwkToMcpsMessage_t;

#pragma pack()
/************************************************************************************
*************************************************************************************
* Public macros
*************************************************************************************
************************************************************************************/

/* Use this from the application when the scheduler is not enabled */
#define MlmeMain() Mlme_Main(0);

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/

/************************************************************************************
*  Macros used to control Special Transmit mode and number of tries
* 
* Revision history:
*   date      Author    Comments
*   ------    ------    --------
*   061006    PNSN001   Created
*   061123    PNSN001   Added macro to controll the number of tries
* 
************************************************************************************/
extern uint8_t aMaxFrameRetries;
extern uint8_t gLowLatencyTX;

  // Macro enabling Low Latency TX mode
#define EnterLowLatencyTXMode() { gLowLatencyTX = 1;}

  // Macro returning the state of TX
#define IsLowLatencyTXModeEnabled() (gLowLatencyTX == 1)

  // Macro disabling special TX mode
#define ExitLowLatencyTXMode()  { gLowLatencyTX = 0;}

  // Macro used to specify the number of tries to transmit a pachet
  //   the MAC wants to receive the number of re-tries, hence the decrement.
#define SetNumberOfTries(MaxFrameTries) { aMaxFrameRetries = (MaxFrameTries) - 1;}

/************************************************************************************
* MAC main function. Must be called by the NWK loop until it returns FALSE. The 
* function will examine the MAC input queues, and handle incoming/outgoing
* MAC/MCPS messages.
*   
* Interface assumptions:
*   None
*   
* Return value:
*   TRUE if there are more messages for the Mlme_Main function process.
*   FALSE otherwise.
* 
************************************************************************************/

// bool_t Mlme_Main(event_t events);
void Mlme_Main(event_t events);

/************************************************************************************
* NWK to MLME service access point provided by the MAC.
*   
* Interface assumptions:
*   pMsg must be allocated by MSG_Alloc(size) unless the primitive in question
*   is Get/Set/Reset-Request.
*   
* Return value:
*   Standard error code
* 
************************************************************************************/
uint8_t NWK_MLME_SapHandler(mlmeMessage_t *pMsg);

/************************************************************************************
* NWK to MCPS service access point provided by the MAC.
*   
* Interface assumptions:
*   pMsg must be allocated by MSG_Alloc(size).
*   
* Return value:
*   Standard error code
* 
************************************************************************************/
uint8_t NWK_MCPS_SapHandler(nwkToMcpsMessage_t *pMsg);

/************************************************************************************
* MLME to NWK service access point provided by the NWK/Application.
*   
* Interface assumptions:
*   pMsg must be deallocated by MSG_Free(pMsg).
*   
* Return value:
*   Standard error code
* 
************************************************************************************/
uint8_t MLME_NWK_SapHandler(nwkMessage_t *pMsg);
//MAC ROM pointer copy of the above item
extern  uint8_t (*pfMLME_NWK_SapHandler)(nwkMessage_t* pMsg);

/************************************************************************************
* MCPS to NWK service access point provided by the NWK/Application.
*   
* Interface assumptions:
*   pMsg must be deallocated by MSG_Free(pMsg).
*   
* Return value:
*   Standard error code
* 
************************************************************************************/
uint8_t MCPS_NWK_SapHandler(mcpsToNwkMessage_t *pMsg);
//MAC ROM pointer copy of the above item
extern  uint8_t (*pfMCPS_NWK_SapHandler)(nwkMessage_t* pMsg);

/************************************************************************************
*
* ZTC services provided by the NWK/Application.
* The ZTC interface needs to be defined, even if the upper layer does not uses it.
* The MAC ROM will call the ZTC function, so at least a stub must be provided.
*
************************************************************************************/
//Upper layer provided function
extern void ZTC_TaskEventMonitor(
                                #ifdef gHostApp_d
                                const ztcUart_t *pZtcUart, 
                                #endif                                 
                                uint8_t sapId, uint8_t* pMsg, uint8_t status );

//MAC ROM pointer copy of the above item
#ifndef gHostApp_d    
extern  void (*pfZTC_TaskEventMonitor)( uint8_t sapId, uint8_t* pMsg, uint8_t status );
#endif

/************************************************************************************
*
* Task Scheduler services.
*
************************************************************************************/
//Upper layer provided function
extern void TS_SendEvent( tsTaskID_t taskID, event_t events );
//MAC ROM pointer copy of the above item
extern  void (*pfTS_SendEvent)( tsTaskID_t taskID, event_t events );
//Uper layer provided object
extern tsTaskID_t gMacTaskID_c;
//MAC ROM copy of the above object
extern tsTaskID_t gMacTaskIDROMCopy_c;

/************************************************************************************
*
* Timer services.
*
************************************************************************************/

extern void  (*pfTMR_StartTimer)(tmrTimerID_t timerId, 
                            tmrTimerType_t timerType, 
                            tmrTimeInMilliseconds_t timeInMilliseconds,
                            void (*pfTmrCallBack)(tmrTimerID_t)
             );
extern bool_t (*pfTMR_IsTimerActive)(tmrTimerID_t timerID);
extern tmrTimerID_t (*pfTMR_AllocateTimer)(void);
extern void (*pfTMR_StopTimer)(tmrTimerID_t timerID);

/************************************************************************************
*
* Security storage provided by the NWK/Application.
* The following variables must be defined, even if the upper layer does not uses
* security. The MAC ROM is compiled with security.
*
************************************************************************************/
//Upper layer provided data
extern aclEntryDescriptor_t gPIBaclEntryDescriptorSet[];
extern uint8_t gNumAclEntryDescriptors;
//MAC ROM pointer copy of the above items
extern aclEntryDescriptor_t *gpPIBaclEntryDescriptorSetROMCopy;
extern uint8_t gNumAclEntryDescriptorsROMCopy;

/************************************************************************************
*
* Message tracking services provided by the NWK/Application.
* The message tracking interface needs to be defined, even if the upper layer does 
* not uses it. The MAC ROM will call the message tracking function, so at least a 
* stub must be provided.
*
************************************************************************************/
//Upper layer provided functions
extern bool_t MM_UpdateMsgTracking(const void *pBlock, const bool_t Alloc);
extern void   MM_InitMsgTracking(void);
extern void   MM_AddMsgToTrackingArray(uint8_t Index, uint32_t Addr);
//MAC ROM pointer copy of the above items
extern bool_t (*pfMM_UpdateMsgTracking)(const void *pBlock, const bool_t Alloc);
extern void   (*pfMM_InitMsgTracking)(void);
extern void   (*pfMM_AddMsgToTrackingArray)(uint8_t Index, uint32_t Addr);

#endif /* _NWK_MAC_INTERFACE_H_ */
