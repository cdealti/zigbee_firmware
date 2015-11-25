/******************************************************************************
* This module contains the application interface to BeeStack, including
* interactions with all ZigBee layers that are exposed to the application.
* Callbacks will come into apps indication handler.
*
* Copyright (c) 2008, Freescale, Inc.  All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from  Freescale Semiconductor.
*
*******************************************************************************/
#ifndef _BEESTACK_INTERFACE_H_
#define _BEESTACK_INTERFACE_H_
#ifndef __IAR_SYSTEMS_ICC__
#ifdef MC13226Included_d
    #undef MC13226Included_d
#endif    
    #define MC13226Included_d 0
#endif
#include "BeeStackFunctionality.h"
#include "BeeStackConfiguration.h"
#include "BeeStackParameters.h"

#include "MsgSystem.h"
#include "Zigbee.h"
#include "NwkCommon.h"
#include "AfApsInterface.h"
#include "ZdoNwkInterface.h"

/* some generic modules need to know */
#ifndef gBeeStackIncluded_d
#define gBeeStackIncluded_d 1
#endif

/******************************************************************************
*******************************************************************************
* Public types
*******************************************************************************
******************************************************************************/

/*************
  APP
**************/
#if gFrequencyAgilityCapability_d && (gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d)
extern uint8_t gChannelMasterState;
extern tsTaskID_t gFATaskId;
#endif

/*************
  General Types (cross layer)
**************/
#define  gRunning_c            0
#define  gScanning_c           1
#define  gProcessScanInfo_c    2

#define gApsmeReqType_c 0x01
#define gNLMEReqType_c  0x02

/*************
  APS
**************/


/*************
  ZDO
**************/

/******************************************************************************
*******************************************************************************
* Private functions
*******************************************************************************
******************************************************************************/

/*************
  General Functions (cross layer)
**************/

/* is this endpoint a member of this group? */
bool_t ApsGroupIsMemberOfEndpoint(zbGroupId_t aGroupId, zbEndPoint_t endPoint);

extern void SetIBReqExternal(
            uint8_t iBReqType,   
						uint8_t attrId,
						uint8_t index,
						uint8_t *pValue
						); 


/******************************************************************************
*******************************************************************************
* Public functions
*******************************************************************************
******************************************************************************/

/*************
  General Macros (cross layer)
**************/
#if gFrequencyAgilityCapability_d && (gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d)
/*
  The 3 possible reasons why a scan gets initiated.
*/
#define  gScanReasonUserInitiated_c     0x01
#define  gScanReasonTxFailureCount_c    0x02
#define  gScanReasonMaxErrorReports_c   0x03
#endif


#if gFrequencyAgilityCapability_d && (gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d)
#define FADeviceChangeState(state) gFA_DeviceState = state
#define FADeviceGetState() gFA_DeviceState
#else
#define FADeviceChangeState(state)
#define FADeviceGetState()0xFF
#endif

/*
  Statemachine state definition
*/
#if gFrequencyAgilityCapability_d && (gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d)
/*
  S1: The state machine just gets started.
*/
#define gInitState_c  0x01

/*
  S2: The node is going to do a local energy scan.
*/
#define gLocalEnergyScan_c  0x02

/*
  S3: The node needs to command to the neighbors to start an energy scan.
*/
#define gNeighborEnergyScan_c  0x03

/*
  S4: The node will build the report to be send out to the application.
*/
#define gReportToTheApplication_c  0x04

/*
  S5: The procedure to change the surrent channel just gets started.
*/
#define gChangeChannelStart_c  0x05

/*
  S6: The node actually has change the channel now and it will go back to the starting state.
*/
#define gChangeChannelEnd_c  0x06

/*
  Void state, used during the Self change, to avoid doing the request multiple times.
*/
#define gSelfChannelChange_c  0xFF


/********************************************
  Event definition for the FA state machine.
*********************************************/
/*
  Ev1: A change cahnnel event arrive.
*/
#define  gChangeChannel_c    (1<<0)

/*
  Ev2: the maximum number of error reposrts has beeng reach.
*/
#define  gMaxErrorReports_c  (1<<1)

/*
  Ev3: The number maximum of failures on the trasnmisition has been reach.
*/
#define  gMaxTransmitionsFailure_c  (1<<2)

/*
  Ev4: The node is going to perform a local energy scan.
*/
#define gInitiateEnergyScan_c  (1<<3)

/*
  Ev5: The local results for the local energy scan just arrive.
*/
#define gLocalScanArrive_c  (1<<4)

/*
  Ev6: The Waiting time for the neighbor scans has expire.
*/
#define gNeighborScanTimeout_c  (1<<5)

/*
  Ev7: The time to wait for the broadcastto be deliver has expired.
*/
#define gDeliveryTimeOutExpires_c  (1<<6)

/*
  Ev8: The change to a new change has being confirmed.
*/
#define gChannelChangeComfirm_c  (1<<7)

/*
  Ev9: The Channel master is ready to report to the app the state of the curretn change.
*/
#define gBuildAndSendReport_c  (1<<8)

/*
  Regular message to the states to avoid unwanted execution.
*/
#define gExecuteState_c  (1<<9)
#endif


/*************
  APS
**************/

/*
  APSME-BIND.request (synchronous)

  Returns
  gZbSuccess_t          if worked
  gZbIllegalDevice_t    if the short or long address not valid
  gZbIllegalRequest_t   if device not on a network
  gZbTableFull_t        if the table is full
  gZbNotSupported_t     if binding not supported
*/
zbStatus_t ApsmeBindRequest(zbApsmeBindReq_t* pBindReq);

/*
  APSME-UNBIND.request (synchronous)

  Returns
  gZbSuccess_t          if worked
  gZbIllegalDevice_t    if the short or long address not valid
  gZbIllegalRequest_t   if device not on a network
  gZbTableFull_t        if the table is full
  gZbNotSupported_t     if binding not supported
*/
zbStatus_t ApsmeUnbindRequest(zbApsmeUnbindReq_t* pUnbindReq);

extern bool_t AF_SetEpMaxWindowSize(zbEndPoint_t Ep, zbMaxWindowSize_t maxWindowSize);
extern zbMaxWindowSize_t AF_GetEpMaxWindowSize(zbEndPoint_t Ep);


/******************************************************************************
*******************************************************************************
* Public Macros
*******************************************************************************
******************************************************************************/

/*************
  APP
**************/

/*
  This macros are use in the channel master state machine.
*/
#if gFrequencyAgilityCapability_d && (gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d)
#define  FA_SetNwkManagerState(state)  (gChannelMasterState = (state))
#define  FA_GetNwkManagerState()  gChannelMasterState
#else
#define  FA_SetNwkManagerState(state)
#define  FA_GetNwkManagerState()
#endif



/*************
  ZDO
**************/

/*************
  APS
**************/

/*
  zbApsCounter_t ApsmeGetCounter(void);

  This counter is used on APSDE-DATA.confirm to uniquely identify a confirm of
  a data request. Use if it's possible to have multiple data requests in progress 
  at any given time.
*/
#define ApsmeGetCounter() (giApsCounter)

#define ApsGetConfirmId() (giApsConfirmId)

#define APS_IncConfirmId() (++giApsConfirmId)
/* Offset used to indicate where the ASDU need to be copy to */
#if gZigbeeProIncluded_d
#define gAsduOffset_c 0x3D
#else
#ifdef __IAR_SYSTEMS_ICC__
/*
asdu needed to fit:
mcpsDataReq_t (29bytes) pointers in arm are 4bytes long
+ nwkHeader_t(8bytes) + zbApsAckFragmentedFrame_t(11bytes)
= 0x30  
*/
#if MC13226Included_d
#define gAsduOffset_c 0x3D
#else
#define gAsduOffset_c 0x31
#endif
#else
/*
asdu needed to fit:
mcpsDataReq_t (27bytes) pointers in hcs08 are 2bytes long
+ nwkHeader_t(8bytes) + zbApsAckFragmentedFrame_t(11bytes)
= 0x2E  
*/
#define gAsduOffset_c 0x2f
#endif
#endif

#define ApsmeGetAsduOffset()  gAsduOffset_c

/* Get the maximum length of ASDU, based on the MSDU length and ASDU Offset*/
#define gAsduMaxLen_c                   (gMaxRxTxDataLength_c - gAsduOffset_c)
#define ApsmeGetMaxAsduLength(pDataReq) GetMaxApplicationPayload(pDataReq)

/* only useful for fragmented messages */
#define ApsmeGetMaxFragBufferSize()  (gMaxRxTxDataLength_c - sizeof(zbTxFragmentedHdr_t))


/* 
  returns the maximum length of the APS application payload. Pass NULL for the "normal" case
  pass an actual zbApsdeDataReq_t for a more accurate maximum
*/
uint8_t GetMaxApplicationPayload
  (
  zbApsdeDataReq_t *pDataReq     /* IN: potential data request */
  );

/* current APS counter */
extern zbApsCounter_t  giApsCounter;

/* confirm id */
extern zbApsConfirmId_t  giApsConfirmId;

extern beeStackParameters_t gBeeStackParameters;


/*
  APS Information Base (AIB)

  The BeeStack AIB is accessible through the following four functions:

  ApsmeGetRequest(attributeId)
  ApsmeGetRequestTableEntry(attributeId,index)
  ApsmeSetRequest(attributeId, pValue)
  ApsmeSetRequestTableEntry(attributeId,index,pValue)

  ApsmeGetRequest() and ApsmeSetRequest() are both for dealing with "simple" types, such 
  as integers or booleans. ApsmeGetRequestTableEntry() and ApsmeSetRequestTableEntry() are
  used for table types, such as the Binding Table.

  The attribute ID is the same as the "Attribute" field found in table 2.24 in the ZigBee 
  R17 specification, with the addition of the Freescale naming convention. For example, 
  "apsBindingTable" becomes "gApsBindingTable_c".

  Some examples:
  if(ApsmeGetRequest(gApsDesignatedCoordinator_c))
  {
    // do stuff for coordinators
  }

  apsBindingTable_t *pBindEntry;
  pBindEntry = ApsmeGetRequestTableEntry(gApsBindingTable_c, 0)
  pBindEntry->dstEndPoint = 0x05; // change destination endpoint
  ApsmeSetRequestTableEntry(gApsBindingTable_c, 0, pBindEntry);

  For simple types, the type reflects what is in the ZigBee specification. For table 
  types, the internal BeeStack type is referenced. To get pure ZigBee types, use the ZDP 
  Management functions, for example ASL_Mgmt_Bind_req(). The tables are all sparse (that 
  is, any of the entries may be valid, for example the first and last, and any entry may 
  be invalid or free, for example all the middle ones).

  All the AIB entries are listed below. In the chart, the ZigBee Name is the name found 
  in table 2.24. The R/W is whether the AIB entry is read/write or read only, the S/P/T 
  says whether the entry is Simple (like an int), Pointer (like an IEEE address), or 
  Table (like Binding or Groups).

  ZigBee Name               R/W S/P/T   Type                 Limit or Default
  -----------------         --- -----   -----------------    ----------------
  apsBindingTable           R/W   T     apsBindingTable_t    ApsmeGetMaxBindingEntries()
  apsDesignatedCoordinator  R/W   S     bool_t               gApsDesignatedCoordinatorDefault_c
  apsChannelMask            R/W   S     zbChannels_t         mDefaultValueOfChannel_c
  apsUseExtendedPANID       R/W   S     zbIeeeAddr_t         mDefaultNwkExtendedPANID_c
  apsGroupTable             R/W   T     zbGroupTable_t       ApsmeGetMaxGroupEntries()
  apsNonmemberRadius        R/W   S     uint8_t              gApsNonMemberRadiusDefault_c
  apsUseInsecureJoin        R/W   S     bool_t               gApsUseInsecureJoinDefault_c
  apsInterframeDelay        R/W   S     uint8_t              gApsInterframeDelayDefault_c
  apsLastChannelEnergy      R/W   S     zbChannelEnergy_t    0
  apsChannelTimer           R/W   S     zbHourTimer_t        gApsChannelTimerDefault_c
*/

/* These following macros use the given attributeId to call the apropriated macro */
/* Get a simple attribute from the AIB (eg max address map entries) */
/* This gets a copy or a pointer, depending on type of AIB item */
#define ApsmeGetRequest(attributeId) ApsmeGetRequest_##attributeId()

/* Gets the first entry from table (makes a copy into the pEntry) */
#define ApsmeGetToPtrRequest(attributeId, pEntry) ApsmeGetToPtrRequest_##attributeId(pEntry)

/* Get an entry from an AIB table attribute (eg address map) */
/* Note: this gets a pointer to the entry, but does not make a copy */
#define ApsmeGetRequestTableEntry(attributeId,index) ApsmeGetRequestTableEntry_##attributeId(index)

/* Get an entry from an AIB table attribute (this makes a copy into the pEntry)*/
#define ApsmeGetToPtrRequestTableEntry(attributeId,index, pEntry) ApsmeGetToPtrRequestTableEntry_##attributeId(index, pEntry)

/* Set a simple attribute in the AIB */
#define ApsmeSetRequest(attributeId, pValue) ApsmeSetRequest_##attributeId(pValue)

/* Set an entry in an AIB table */
#define ApsmeSetRequestTableEntry(attributeId,index,pValue) ApsmeSetRequest_##attributeId(index,pValue)

#if MC13226Included_d
    extern index_t gApsMaxBindingEntries;
    extern uint8_t gApsMaxGroups;
#else
    extern const index_t gApsMaxBindingEntries;
    extern const uint8_t gApsMaxGroups;
#endif

/* apsBindingTable macros */
#define ApsmeGetRequest_gApsBindingTable_c(index)				  &gpBindingTable[index]
#define ApsmeSetRequest_gApsBindingTable_c(index, pValue)	FLib_MemCpy(&gpBindingTable[index], pValue, sizeof(gpBindingTable[index]))

#define ApsmeGetMaxBindingEntries() gApsMaxBindingEntries

/* apsDesignatedCoordinator macros */
#define ApsmeGetRequest_gApsDesignatedCoordinator_c()       gBeeStackParameters.gfApsDesignatedCoordinator
#define ApsmeSetRequest_gApsDesignatedCoordinator_c(iValue) (gBeeStackParameters.gfApsDesignatedCoordinator = iValue)

/* UseInsecureJoin macros */
#define ApsmeGetRequest_gApsUseInsecureJoin_c()        gSAS_Ram.fUseInsecureJoin
#define ApsmeSetRequest_gApsUseInsecureJoin_c(iValue)  (gSAS_Ram.fUseInsecureJoin = (iValue))

/* apsChannelMask macros */
#define ApsmeGetRequest_gApsChannelMask_c()             gSAS_Ram.aChannelMask
#define ApsmeSetRequest_gApsChannelMask_c(pValue)       FLib_MemCpy(gSAS_Ram.aChannelMask, pValue, sizeof(gSAS_Ram.aChannelMask)); \
                                                        SetIBReqExternal(gApsmeReqType_c, gApsChannelMask_c, 0, pValue)


/* apsUseExtendedPANID macros */
#define ApsmeGetRequest_gApsUseExtendedPANID_c()        gSAS_Ram.aApsUseExtendedPanId
#define ApsmeSetRequest_gApsUseExtendedPANID_c(pValue)  FLib_MemCpy(gSAS_Ram.aApsUseExtendedPanId, pValue, sizeof(gSAS_Ram.aApsUseExtendedPanId))

/* apsGroupTable macros */
#define ApsmeGetRequest_gApsGroupTable_c(index)					&gpGroupTable[index]
#define ApsmeSetRequest_gApsGroupTable_c(index, pValue)	FLib_MemCpy(&gpGroupTable[index], pValue, sizeof(zbGroupTable_t))

#define ApsmeGetMaxGroupEntries() gApsMaxGroups

/* apsNonmemberRadius macros */
#define ApsmeGetRequest_gApsNonmemberRadius_c()         gBeeStackParameters.gApsNonmemberRadius
#define ApsmeSetRequest_gApsNonmemberRadius_c(mRadius)  (gBeeStackParameters.gApsNonmemberRadius = mRadius)

/* apsAddressMap macros (not in ZigBee spec) */
#define ApsmeGetToPtrRequest_gApsAddressMap_c(index, pEntry)   (void)AddrMap_GetTableEntry(index, pEntry)

#define ApsmeSetRequest_gApsAddressMap_c(index, pValue)        (void)AddrMap_SetTableEntry(index, pValue); \
                                                               SetIBReqExternal(gApsmeReqType_c,gApsAddressMap_c, index, pValue)

/* apsMaxWindowSize macros (Fragmentation) */
#define ApsmeGetRequest_gApsMaxWindowSize_c()               gBeeStackParameters.gApsMaxWindowSize

#define ApsmeSetRequest_gApsMaxWindowSize_c(mWinSize)       (gBeeStackParameters.gApsMaxWindowSize = (mWinSize)); \
                                                            SetIBReqExternal(gApsmeReqType_c,gApsMaxWindowSize_c, 0, &gBeeStackParameters.gApsMaxWindowSize)

/* apsMaxWindowSize per end point macros (Fragmentation) */
#define ApsmeGetRequest_gApsMaxEpWindowSize_c(Ep)             AF_GetEpMaxWindowSize(Ep)
#define ApsmeSetRequest_gApsMaxEpWindowSize_c(Ep,mWinSize)    AF_SetEpMaxWindowSize(Ep, mWinSize)


/* apsInterframeDelay macros (Fragmentation) */
#define ApsmeGetRequest_gApsInterframeDelay_c()             gBeeStackParameters.gApsInterframeDelay
#define ApsmeSetRequest_gApsInterframeDelay_c(mFrameDelay)  (gBeeStackParameters.gApsInterframeDelay = mFrameDelay); \
                                                            SetIBReqExternal(gApsmeReqType_c, gApsInterframeDelay_c, 0, &gBeeStackParameters.gApsInterframeDelay);


/* apsInterframeDelay macros (Fragmentation) */
#define ApsmeGetRequest_gApsMaxFragmentLength_c()           gBeeStackParameters.gApsMaxFragmentLength
#define ApsmeSetRequest_gApsMaxFragmentLength_c(mMaxLen)    (gBeeStackParameters.gApsMaxFragmentLength = mMaxLen);\
                                                            SetIBReqExternal(gApsmeReqType_c, gApsMaxFragmentLength_c, 0, &gBeeStackParameters.gApsMaxFragmentLength);
/* apsChannelFailureRate macros */
#define ApsmeGetRequest_gApsChannelFailureRate_c()          gApsChannelFailureRate
#define ApsmeSetRequest_gApsChannelFailureRate_c(mValue)    (gApsChannelFailureRate = mValue)

/* apsChannelTimer macros */
#define ApsmeGetRequest_gApsChannelTimer_c()                gApsChannelTimer
void ApsSetCountDownTimer(uint8_t iHours);
#define ApsmeSetRequest_gApsChannelTimer_c(mValue)          ApsSetCountDownTimer(mValue)

/* APS Security Macros */

/* Trust center Address macros */
#define ApsmeGetRequest_gApsTrustCenterAddress_c()                gSAS_Ram.aTrustCenterAddress
#define ApsmeSetRequest_gApsTrustCenterAddress_c(pIeeeAddress)    Copy8Bytes(gSAS_Ram.aTrustCenterAddress,(pIeeeAddress))

/* Trust center short address mactoss */
#define ApsmeGetRequest_gApsTrustCenterNwkAddress_c()             gBeeStackParameters.apsTrustCenterNwkAddress
#define ApsmeSetRequest_gApsTrustCenterNwkAddress_c(pNwkAddress)  Copy2Bytes(gBeeStackParameters.apsTrustCenterNwkAddress,(pNwkAddress))

/* Security Aps Timeout macross */
#define ApsmeGetRequest_gApsSecurityTimeOutPeriod_c()             gBeeStackParameters.apsSecurityTimeOutPeriod
#define ApsmeSetRequest_gApsSecurityTimeOutPeriod_c(iTimeout)     gBeeStackParameters.apsSecurityTimeOutPeriod = (iTimeout)

/* Device key pair set macross(this make a copy) */
#define ApsmeGetToPtrRequest_gApsDeviceKeyPairSet_c(pEntry)                  DevKeyPairSet_GetTableEntry(0, pEntry)
#define ApsmeGetToPtrRequestTableEntry_gApsDeviceKeyPairSet_c(index, pEntry) DevKeyPairSet_GetTableEntry(index, pEntry)
#define ApsmeGetRequest_gApsDeviceKeyPairCount_c()                giApsDeviceKeyPairCount

/* Trust center Master key macross */
#define ApsmeGetRequest_gApsTrustCenterMasterKey_c()              gSAS_Ram.aTrustCenterMasterKey
#define ApsmeSetRequest_gApsTrustCenterMasterKey_c(pKey)          Copy16Bytes((void *)gSAS_Ram.aTrustCenterMasterKey, (void *)pKey)

/* Trust center Link key macross */
#define ApsmeGetRequest_gApsTrustCenterLinkKey_c()                gSAS_Ram.aPreconfiguredTrustCenterLinkKey
#define ApsmeSetRequest_gApsTrustCenterLinkKey_c(pKey)            Copy16Bytes((void *)gSAS_Ram.aPreconfiguredTrustCenterLinkKey,(void *) pKey)

#define ApsmeGetRequest_gApsDefaultTCKeyType_c()                  gBeeStackConfig.defaultPreconfigureTCKey
#define ApsmeSetRequest_gApsDefaultTCKeyType_c(keyType)           (gBeeStackConfig.defaultPreconfigureTCKey = (keyType))



/*************
  NWK
**************/

/* addresses used for broadcasting... can be use by applications or internal stack code */
extern const zbNwkAddr_t gaBroadcastAddress;   /* 0xffff broadcast addressing */
extern const zbNwkAddr_t gaBroadcastZCnZR;     /* 0xfffc routers/coordinator */
extern const zbNwkAddr_t gaBroadcastRxOnIdle;  /* 0xfffd only devices with RxOnIdle=TRUE */


/* Do not modify this variables directly, use the macros defined below */

#if MC13226Included_d
    extern uint8_t	gNibMaxBroadcastRetries;
    extern uint8_t	gNibMaxDepth;
    extern uint8_t	gNibMaxRouter;
    extern uint32_t	gNibNetworkBroadcastDeliveryTime;
    extern uint8_t	gNibSymLink;
    extern uint8_t    gNibUniqueAddr;
    extern uint8_t    gNwkProtocolId;
    extern uint8_t    gIncomingFrameCounterSetLimit;
#else
    extern const uint8_t	gNibMaxBroadcastRetries;
    extern const uint8_t	gNibMaxDepth;
    extern const uint8_t	gNibMaxRouter;
    extern const uint32_t	gNibNetworkBroadcastDeliveryTime;
    extern const uint8_t	gNibSymLink;
    extern const uint8_t    gNibUniqueAddr;
    extern const uint8_t    gNwkProtocolId;
    extern const uint8_t    gIncomingFrameCounterSetLimit;

#endif


extern uint8_t        gNwkState;
extern const uint8_t	gaSoftwareVersion[2];
extern const uint16_t	gNibPassiveAckTimeout;
extern const uint8_t	gNibMaxChildren;
extern const uint8_t	gNibReportConstantCost;
extern uint8_t	      gNibRouteDiscoveryRetriesPermitted;
extern const uint8_t	gNibAddrAlloc;
extern const bool_t		gNibUseTreeRouting;
extern uint16_t	      gNibTransactionPersistenceTime;
extern uint8_t        aExtendedAddress[8];
extern uint16_t       gNibTxTotal;
extern uint16_t       gNibTxTotalFailures;
extern bool_t         gNibNwkUseMulticast;
extern bool_t         gDeviceAlreadySentRouteRecord;
extern uint8_t        gNibNwkLinkStatusPeriod;
extern const uint8_t  gNibNwkMaxSourceRoute;


/*
  The following macros covert a NIB ID to the data attached to it (both for get and set).
  Some macros return a value (8, 16 or 32-bit), others return an array of bytes.
  All multi-byte arrays are stored little endian (same as over-the-air format).
  The type returned by the get is the same type used for the set.

  For example:
    NlmeGetRequest(gNwkShortAddress_c)
    converts to NlmeGetRequest_gNwkShortAddress_c()
    which converts to gSAS_Ram.aShortAddress

  Usage example:
    bool_t IsPanID1234(void)
    {
      zbPanId_t  aPan1234 = { 0x34, 0x12 };// PAN IDs are stored as array of 2 bytes, little endian

      return IsEqual2Bytes(aPan1234, NlmeGetRequest(gNwkPanId_c));
    }
*/
#define NlmeGetRequest(attributeId) NlmeGetRequest_##attributeId()

/* Get an entry from an AIB table attribute (eg address map) */
#define NlmeGetRequestTableEntry(attributeId,index) NlmeGetRequest_##attributeId(index)

/* Set a NIB entry value */
#define NlmeSetRequest(attributeId, pValue) NlmeSetRequest_##attributeId(pValue)

/* Set a specific table value*/
#define NlmeSetRequestTableEntry(attributeId,index,pValue) NlmeSetRequest_##attributeId(index,pValue)


/* Macros to GET NIB attributes values (r17, table 3.44) */
#define NlmeGetRequest_gNwkUniqueAddr_c() 				    gNibUniqueAddr

#define NlmeGetRequest_gNwkSequenceNumber_c() 				gNibSequenceNumber

#define NlmeGetRequest_gNwkPassiveAckTimeout_c()			gNibPassiveAckTimeout

#define NlmeGetRequest_gNwkMaxBroadcastRetries_c()		gNibMaxBroadcastRetries

#define NlmeGetRequest_gNwkMaxChildren_c()						gNibMaxChildren

#define NlmeGetRequest_gNwkMaxDepth_c()								gNibMaxDepth

#define NlmeGetRequest_gNwkMaxRouter_c()							gNibMaxRouter

#define NlmeGetRequest_gNwkMaxSourceRoute_c()         gNibNwkMaxSourceRoute

#define NlmeGetRequest_gNwkNeighborTable_c(index)			&gaNeighborTable[index]

#define NlmeGetRequest_gNwkNetworkBroadcastDeliveryTime_c() gNibNetworkBroadcastDeliveryTime

#define NlmeGetRequest_gNwkReportConstantCost_c()			gNibReportConstantCost

#define NlmeGetRequest_gNwkRouteDiscoveryRetriesPermitted_c() gNibRouteDiscoveryRetriesPermitted

#define NlmeGetRequest_gNwkRouteTable_c(index) 				&gaRouteTable[index]

#define NlmeGetRequest_gNwkSymLink_c()								gNibSymLink

#define NlmeGetRequest_gNwkCapabilityInformation_c()	gNwkData.capabilityInformation

#define NlmeGetRequest_gNwkAddrAlloc_c()				      gNibAddrAlloc

#define NlmeGetRequest_gNwkUpdateId_c()				        gNwkData.nwkUpdateId

#define NlmeGetRequest_gNwkUseTreeRouting_c()					gNibUseTreeRouting

#define NlmeGetRequest_gNwkTransactionPersistenceTime_c()	gNibTransactionPersistenceTime

#define NlmeGetRequest_gNwkShortAddress_c()						gSAS_Ram.aShortAddress

#define NlmeGetRequest_gNwkStackProfile_c()						gSAS_Ram.stackProfile

#define NlmeGetRequest_gNwkProtocolVersion_c()				gSAS_Ram.protocolVersion

#define NlmeGetRequest_gNwkExtendedPanId_c()					gSAS_Ram.aNwkExtendedPanId

#define NlmeGetRequest_gNwkTxTotal_c() 				        gNibTxTotal

#define NlmeGetRequest_gNwkTxTotalFailures_c()        gNibTxTotalFailures

#define NlmeGetRequest_gNwkPanId_c()							    gSAS_Ram.aPanId

#define NlmeGetRequest_gNwkIeeeAddress_c()					  aExtendedAddress

#define NlmeGetRequest_gParentShortAddress_c()			  gNwkData.aParentShortAddress

#define NlmeGetRequest_gParentLongAddress_c()         gNwkData.aParentLongAddress

#define NlmeGetRequest_gNwkLogicalChannel_c()				  gNwkData.logicalChannel

#define NlmeGetRequest_gNwkDeviceDepth_c()            gNwkData.deviceDepth

/* NIB Id: 0xa0 */
#if gStandardSecurity_d || gHighSecurity_d
#define NlmeGetRequest_gNwkSecurityLevel_c()          (gNwkData.nwkSecurityLevel)
#else
#define NlmeGetRequest_gNwkSecurityLevel_c()          (0)
#endif

/* NIB Id: 0xa1 */
#define NlmeGetRequest_gNwkSecurityMaterialSet_c()    (gNwkData.aNwkSecurityMaterialSet)

/* NIB Id: 0xa2 */
#define NlmeGetRequest_gNwkActiveKeySeqNumber_c()     (gSAS_Ram.activeNetworkKeySeqNum)

#define NlmeGetRequest_gNwkKeyType_c()                (gSAS_Ram.networkKeyType)

/* NIB Id: 0xa3 */
#define NlmeGetRequest_gNwkAllFresh_c()               (gNwkData.nwkAllFresh)

/* NIB Id: 0xa5 */
#define NlmeGetRequest_gNwkSecureAllFrames_c()        (gNwkData.nwkSecureAllFrames)

/* FS:SAS? */
#define NlmeGetRequest_gNwkPreconfiguredKey_c()       (gBeeStackConfig.gNwkKeyPreconfigured)

#define NlmeGetRequest_gNwkIncomingFrameCounterSetLimit_c()  (gIncomingFrameCounterSetLimit)

#define NlmeGetRequest_gConfirmationPollTimeOut_c()	  gBeeStackConfig.gConfirmationPollTimeOut

#define NlmeGetRequest_gAuthenticationPollTimeOut_c() gBeeStackConfig.gAuthenticationPollTimeOut

#define NlmeGetRequest_gDevType_c()									  gNwkData.deviceType

#define NlmeGetRequest_gSoftwareVersion_c()					  gaSoftwareVersion

#define NlmeGetRequest_gNwkState_c()	                gNwkState

#define NlmeGetRequest_gNwkManagerAddr_c()            gSAS_Ram.aNetworkManagerAddress

#define NlmeGetRequest_gNwkPermitJoiningFlag_c()      gNwkData.permitJoining

#define NlmeGetRequest_gNwkUseMulticast_c()           gNibNwkUseMulticast

/* is this node a concentrator? */
#define NlmeGetRequest_gNwkIsConcentrator_c()         gSAS_Ram.fConcentratorFlag

/* radius of many-to-one route discovery */
#define NlmeGetRequest_gNwkConcentratorRadius_c()     gSAS_Ram.concentratorRadius

/* time in seconds to do another many-to-one route request. 0x00=app will do it */
#define NlmeGetRequest_gNwkConcentratorDiscoveryTime_c() gSAS_Ram.concentratorDiscoveryTime

/* see R17 Tablw 2.148 Config_NWK_Scan_Attempts */
#define NlmeGetRequest_gNwkScanAttempts_c()           gSAS_Ram.scanAttempts  // moved to gSAS_Ram

/* time between scans, see R17 Table 2.148 Config_NWK_Time_Btwn_Scans */
#define NlmeGetRequest_gNwkTimeBtwnScans_c()          GetNative16BitInt(gSAS_Ram.aTimeBetweenScans)

/* time between rejoining when lost contact with network */
#define NlmeGetRequest_gRejoinInterval_c()            GetNative16BitInt(gSAS_Ram.aRejoinInterval)

#define NlmeGetRequest_gMaxRejoinInterval_c()         GetNative16BitInt(gSAS_Ram.aMaxRejoinInterval)

/* poll rate of ZED */
#define NlmeGetRequest_gNwkIndirectPollRate_c()       GetNative16BitInt(gSAS_Ram.aIndirectPollRate)

/* used both for parent retry threshold (ZEDs) and link retry threshold (ZR, ZC) */
#define NlmeGetRequest_gNwkLinkRetryThreshold_c()     gSAS_Ram.parentLinkRetryThreshold

/* allow insecure association after a nwk rejoin has failed */
#define NlmeGetRequest_gNwkUseInsecureJoin_c()        gSAS_Ram.fUseInsecureJoin

#define NlmeGetRequest_RouteRecordWasSent()           gDeviceAlreadySentRouteRecord

#define NlmeGetRequest_gNwkLinkStatusPeriod_c()       gNibNwkLinkStatusPeriod


#define NlmeSetRequest_gNwkLinkStatusPeriod_c(value)  gNibNwkLinkStatusPeriod = value

#define NlmeSetRequest_gNwkMaxSourceRoute_c(value)    gNibNwkMaxSourceRoute = value

#define NlmeSetRequest_RouteRecordWasSent(value)      gDeviceAlreadySentRouteRecord = value
/* FS:SAS? allow over-the-air choice of ZC, ZR, ZED? */				
#define NlmeSetRequest_gNwkCapabilityInformation_c(pValue) gNwkData.capabilityInformation = *pValue

#define NlmeSetRequest_gNwkPanId_c(pValue)	          Copy2Bytes(gSAS_Ram.aPanId, pValue); \
                                                      SetIBReqExternal(gNLMEReqType_c,gNwkPanId_c, 0, pValue)

#define NlmeSetRequest_gNwkShortAddress_c(pValue)     Copy2Bytes(gSAS_Ram.aShortAddress, pValue); \
                                                      SetIBReqExternal(gNLMEReqType_c, gNwkShortAddress_c, 0, pValue)

#define NlmeSetRequest_gNwkExtendedPanId_c(pValue)	  Copy8Bytes(gSAS_Ram.aNwkExtendedPanId, pValue); \
                                                      SetIBReqExternal(gNLMEReqType_c, gNwkExtendedPanId_c, 0, pValue)

#define NlmeSetRequest_gParentShortAddress_c(pValue)	Copy2Bytes(gNwkData.aParentShortAddress, pValue)

/* FS:SAS? - for direct join */
#define NlmeSetRequest_gParentLongAddress_c(pValue)   Copy8Bytes(gNwkData.aParentLongAddress, pValue)

#define NlmeSetRequest_gNwkIeeeAddress_c(pValue)	    Copy8Bytes(aExtendedAddress, pValue)

#define NlmeSetRequest_gNwkLogicalChannel_c(pValue)   gNwkData.logicalChannel = *(pValue); \
                                                      SetIBReqExternal(gNLMEReqType_c, gNwkLogicalChannel_c, 0, pValue)

#define NlmeSetRequest_gNwkDeviceDepth_c(pValue)      gNwkData.deviceDepth = pValue

/* FS:SAS? combine to one "fast" ZED timeout? */
#define NlmeSetRequest_gConfirmationPollTimeOut_c(pValue) gBeeStackConfig.gConfirmationPollTimeOut = *(pValue)

/* FS:SAS? */
#define NlmeSetRequest_gAuthenticationPollTimeOut_c(pValue) gBeeStackConfig.gAuthenticationPollTimeOut = *(pValue)

#define NlmeSetRequest_gNwkUseMulticast_c(iValue)     gNibNwkUseMulticast = iValue


/* NIB Id: 0xa0 */
#if gStandardSecurity_d || gHighSecurity_d
#define NlmeSetRequest_gNwkSecurityLevel_c(value)     (gNwkData.nwkSecurityLevel = (value))
#else
#define NlmeSetRequest_gNwkSecurityLevel_c(value)
#endif

#define NlmeSetRequest_gNwkTxTotal_c(value)                 (gNibTxTotal = (value))

#define NlmeSetRequest_gNwkTxTotalFailures_c(value)         (gNibTxTotalFailures = (value))


/* NIB Id: 0xa1 */
#define NlmeSetRequest_gNwkSecurityMaterialSet_c(value) (FLib_MemCpy(gNwkData.aNwkSecurityMaterialSet, value, sizeof(zbNwkSecurityMaterialSet_t))

/* NIB Id: 0xa2 */
#define NlmeSetRequest_gNwkActiveKeySeqNumber_c(value)  (gSAS_Ram.activeNetworkKeySeqNum = (value))

#define NlmeSetRequest_gNwkKeyType_c(value)             (gSAS_Ram.networkKeyType = (value))

/* NIB Id: 0xa3 */
#define NlmeSetRequest_gNwkAllFresh_c(value)            (gNwkData.nwkAllFresh = (value))

/* NIB Id: 0xa5 */
#define NlmeSetRequest_gNwkSecureAllFrames_c(value)     (gNwkData.nwkSecureAllFrames = (value))

#define NlmeSetRequest_gNwkPreconfiguredKey_c(value)    (gBeeStackConfig.gNwkKeyPreconfigured = (value))

#define NlmeSetRequest_gNwkUpdateId_c(pValue)           (gNwkData.nwkUpdateId = *(pValue))

#define NlmeSetRequest_gNwkManagerAddr_c(pValue)        Copy2Bytes(gSAS_Ram.aNetworkManagerAddress, pValue)

#define NlmeSetRequest_gNwkState_c(value)               gNwkState = (value)

#define NlmeSetRequest_gDevType_c(value)                gNwkData.deviceType = (value)

#define NlmeSetRequest_gNwkIsConcentrator_c(fValue)     (gSAS_Ram.fConcentratorFlag = (fValue))

#define NlmeSetRequest_gNwkConcentratorRadius_c(value)  (gSAS_Ram.concentratorRadius = (value))

#define NlmeSetRequest_gNwkConcentratorDiscoveryTime_c(value) (gSAS_Ram.concentratorDiscoveryTime = (value))

#define NlmeSetRequest_gNwkScanAttempts_c(value)        (gSAS_Ram.scanAttempts = (value))

/* time between scans */
#define NlmeSetRequest_gNwkTimeBtwnScans_c(value)       SetNative16BitInt(gSAS_Ram.aTimeBetweenScans, value)

/* time between rejoin attempts */
#define NlmeSetRequest_gRejoinInterval_c(value)         SetNative16BitInt(gSAS_Ram.aRejoinInterval, value)

/* maximum rejoin interval (if variable) */
#define NlmeSetRequest_gMaxRejoinInterval_c(value)      SetNative16BitInt(gSAS_Ram.aMaxRejoinInterval, value)

/* time between polls the a ZEDs parent in ms */

#define NlmeSetRequest_gNwkIndirectPollRate_c(value)    SetNative16BitInt(gSAS_Ram.aIndirectPollRate, value); \
                                                        SetIBReqExternal(gNLMEReqType_c, gNwkIndirectPollRate_c, 0, (uint8_t*)&gSAS_Ram.aIndirectPollRate)
 
#define NlmeSetRequest_gNwkLinkRetryThreshold_c(value)  (gSAS_Ram.parentLinkRetryThreshold = (value))

/* Allow assocation join if nwk rejoin fails */
#define NlmeSetRequest_gNwkUseInsecureJoin_c(value)     (gSAS_Ram.fUseInsecureJoin = (value))

/* note: no error checking */
#define NlmeSetRequest_gNwkNeighborTable_c(index,pValue) \
					FLib_MemCpy(&gaNeighborTable[index], pValue, sizeof(neighborTable_t))

/* note: no error checking */
#define NlmeSetRequest_gNwkRouteTable_c(index,pValue) \
					FLib_MemCpy(&gaRouteTable[index], pValue, sizeof(routeTable_t))

/* macros to access the subfields of the mac capability information attribute */
#define IsLocalDeviceReceiverOnWhenIdle() \
					(NlmeGetRequest(gNwkCapabilityInformation_c) & gReceiverOnwhenIdle_c) 

/* TRUE for both ZC and ZR */
#define IsLocalDeviceTypeARouter() \
					(NlmeGetRequest(gNwkCapabilityInformation_c) & gDeviceType_c)

/* 053474r17 Zigbee Spec, secc. 3.6.1.4.1.1
   The link quality for frames received from this device is such that a link cost of
   at most 3 is produced when calculated as described in sub-clause 3.6.3.1 
*/
#define maxLinkCost 0x03

/* 
  Calculates the Link cost based on the following Table
  ===================
  | LQI    |LinkCost|
  ===================
  |  >75   |    1   |
  -------------------
  | 50-75  |    3   |
  -------------------
  |  <50   |    7   |
  -------------------

  053474r17 Zigbee Spec
  Link cost calculation explanation 3.6.3.1 Routing Cost
*/
typedef uint8_t zbLinkCost_t;
zbLinkCost_t LinkCostCalculator(uint8_t iLqi);

/*Masks to extract the from the extended neighbor table the subfields of joiningFeatures field */
#define gMaskPermitJoin_c       0x01    /* Bit 0  -> PermitJoin */
#define gMaskPotentialParent_c  0x02    /* Bit 1  -> PotentialParent */
#define gMaskStackProfile_c     0xE0    /* Bit 4-7-> Stack profile */


/**********************************************************************************
* Macros to access and manipulate data in the Neighbor Table
**********************************************************************************/

/* This macro gets the outgoing cost from the neighbor table entry */
#define GetOutgoingCostFromNeighborTableEntry(pNTE) \
  (pNTE->outgoingCost & 0x0F)


/* This macro gets the incoming cost from the neighbor table entry */
#define GetincomingCostFromNeighborTableEntry(pNTE)\
  (pNTE->outgoingCost & 0xF0) >> gShiftLeftByFour_c

/* This macro gets the age from the neighbor table entry */
#define GetAgeFromNeighborTableEntry(pNTE) pNTE->age

/* This macro set the age to the given neighbor table entry */
#define SetAgeFromNeighborTableEntry(pNTE, val) pNTE->age = val 
/**********************************************************************************
* Macros to access and manipulate data in the Extended Neighbor Table
**********************************************************************************/

/*This macro gets the PermitJoin attribute value from device property
containing the bit field values  */
#define NWK_DevAllowsAssociation(deviceInfo ) ( deviceInfo & gMaskPermitJoin_c )

/*This macro gets the Potential parent attribute value from device property
containing the bit field values */
#define NWK_DeviceIsPotentialParent(deviceInfo ) ( deviceInfo & gMaskPotentialParent_c )

/******************************************************************************
* This function sets the outgoing cost for a neighbor table entry
******************************************************************************/
/* This macro set the outgoing cost to the given neighbor table entry */
void SetOutgoingCostFromNeighborTableEntry(neighborTable_t *pNTE, uint8_t cost);
/******************************************************************************
* This function sets the ingoing cost for a neighbor table entry
******************************************************************************/
void SetincomingCostFromNeighborTableEntry(neighborTable_t *pNTE, uint8_t cost);

/******************************************************************************
* This function goes thru the whole neighbor table to find out how many devices
* of the given type are in the neighbor table and return the result.
******************************************************************************/
zbCounter_t NWK_MNG_GetNumberOfNeighbors( zbRelationshipType_t relationshipType, zbDeviceType_t deviceType );

/******************************************************************************
                          FREQUENCY AGILITY PROTOTYPES
  In this section of this file are the prototypes of the fucntions need it for 
  the processing in frequency agility.
******************************************************************************/
#if gFrequencyAgilityCapability_d && (gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d)
/*------------- FA_Process_Mgmt_NWK_Update_notify --------------*/
void FA_Process_Mgmt_NWK_Update_notify
(
  zbMgmtNwkUpdateNotify_t  *pMessageComingIn  /* IN: The package with the request information. */
);

void FA_StateMachine
(
  event_t events
);

void FA_StateMachineInit
(
  void
);

void FA_ProcessNlmeTxReport
(
  nlmeNwkTxReport_t  *pNlmeNwkTxReport
);

void FA_ProcessEnergyScanCnf
(
  nlmeEnergyScanCnf_t *pScanCnf
);

uint32_t FA_BuildListFromChannel
(
  uint8_t  channelNumber
);

void FA_SelectChannelAndChange
(
  void
);

uint8_t FA_GetChannelFromList
(
  uintn32_t *pChannelList  /* IN: the 32-bits channel list, the list shall have only
                                  one valid channel. */
);

#define FA_SendChangeChannelEvt() TS_SendEvent(gFATaskId, gChangeChannel_c)
#else
#define FA_Process_Mgmt_NWK_Update_notify(pMessageComingIn)
#define FA_StateMachine(events)
#define FA_StateMachineInit()
/* Reset the counter to avoid sending too many once the first report has been sent.*/
#define FA_ProcessNlmeTxReport(pNlmeNwkTxReport) ResetTxCounters();
#define FA_ProcessEnergyScanCnf(pScanCnf)
#define FA_BuildListFromChannel(channelNumber)
#define FA_SendChangeChannelEvt()
#define FA_SelectChannelAndChange()
#endif

#if gFrequencyAgilityCapability_d
/*--------------FA_Process_Mgmt_NWK_Update_request--------------*/
uint8_t FA_Process_Mgmt_NWK_Update_request
(
  zbMgmtNwkUpdateRequest_t  *pMessageComingIn,  /* IN: The package with the request information. */
  void                      *pMessageGoingOut,  /* IN/OUT: The buffer where the response will be build. */
  zbNwkAddr_t               aSrcAddrr
);
#else
#define FA_Process_Mgmt_NWK_Update_request(pMessageComingIn,pMessageGoingOut, aSrcAddrr) 0
#endif

/*gFrequencyAgilityCapability_d*/

/*****************************************************************************************
                     SECURITY SPECIFIC AND EXPOSED INTERFACES
*****************************************************************************************/
#if gStandardSecurity_d || gHighSecurity_d
#if gTrustCenter_d || gComboDeviceCapability_d
void AppAuthenticateDevice
(
  zbIeeeAddr_t  aIeeeAddress
);
#else
#define AppAuthenticateDevice(aIeeeAddress)
#endif
#endif

#endif  /* _BEESTACK_INTERFACE_H_ */
