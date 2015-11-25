/******************************************************************
* This header file contains the defination for different constants
* used in NWK layer
*
*
* (c) Copyright 2005, Freescale, Inc. All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
**********************************************************************/
#ifndef _NWK_COMMON_H_
#define _NWK_COMMON_H_

#include "BeeStackInterface.h"
#include "TMR_Interface.h"

/******************************************************************************
*******************************************************************************
* Public memory declarations
*******************************************************************************
******************************************************************************/

/******************************************************************************
*******************************************************************************
* Public macros
*******************************************************************************
******************************************************************************/

/* Zigbee SapHandler events */

/* NWK SapHandlers events */
#define gAPS_NLDE_c (1<<0)
#define gMCPS_NWK_c (1<<1)
#define gMLME_NWK_c (1<<2)
#define gZDO_NLME_c (1<<3)

/* NWK timeout events */
/* List of timer ids used by the network layer */
#define gRREQJitterTimerEvent_c         (1<<4)
#define gRouteDiscoveryTimerEvent_c     (1<<5)
#define gBroadcastDeliveryTimerEvent_c  (1<<6)
#define gBroadcastJitterTimerEvent_c    (1<<7)
#define gPermitJoinTimerEvent_c         (1<<8)
#define gLeaveTimerEvent_c              (1<<9)
#define gMacPurgeTimer_c 								(1<<11)
#define gEndDevicePollTimeOut_c         (1<<13)
#define gUnicastNextEvent_c             (1<<14)
#define gNwkSendLinkStatus_c            (1<<15)


#define gNwkEntryNotFound_c 0xFF
#define gAllTypes_c   0xFF
#define gRoutersAndCoordinator_c  0xFE


#define gParameterIgnored_c   0x00

/* neighborTable linkStatus values*/
#define gInCommunication_c    0x00
#define gLostCommunication_c  0x01

/* Reset the Sec bit in the NWK Hdr */
#define DisableSecuityInNwkHeader(aFrameControl) ( aFrameControl[ 1 ] &= ~gSecurity_c )

#define IsASecureFrame(aFrameControl) ( aFrameControl[ 1 ] & gSecurity_c )

#define IsAMulticastFrame(aFrameControl) ( aFrameControl[ 1 ] & gMulticastFlag_c )

#define IsAMulticastRouteRequestFrame(commandOptions) ( commandOptions & gRouteReqMutlicast_c )

#define IsAManyToOneRouteRequestFrame(commandOptions) ( commandOptions & gManyToOneMask_c )

#define IsASourceRouteDataFrame(aFrameControl) ( aFrameControl[ 1 ] & gSourceRoute_c )

#define IsDstIeeeAddressPresent(aFrameControl) ( aFrameControl[ 1 ] & gDstIeeeAddrPresent_c)

#define IsSrcIeeeAddressPresent(aFrameControl) ( aFrameControl[ 1 ] & gSrcIeeeAddrPresent_c)

#define IsOriginatorIEEEAddrPresent(commandOptions)(commandOptions & gRouteReqOriginatorIEEEAddr)

#define IsResponderIEEEAddrPresent(commandOptions)(commandOptions & gRouteReqResponderIEEEAddr)

#define IncrementNwkSequenceNumber()  gNibSequenceNumber++

#define IncrementNwkTxTotal()         gNibTxTotal++

#define IncrementNwkUpdateId()        gNwkData.nwkUpdateId++

#define ResetTxFailureCounterPerChild(iNteIndex) gaNeighborTable[iNteIndex].transmitFailure = 0


void IncrementNwkTxTotalFailures( void );
void ResetTxCounters(void);

extern zbCounter_t gMsduHandle;
zbCounter_t NWK_AllocateHandle( void );

/* extract the frame type from the nwk header */
#define GetFrameType(pNpdu) (pNpdu->nwkHr.aFrameControl[ 0 ] & gFrameType_c)

/* based on the given frame control determines if it is a data frame or not*/
#define IsADataFrame(pNpduFrame) ((pNpduFrame->nwkHr.aFrameControl[ 0 ] & gFrameType_c) == gDataFrame_c)

/* based on the given frame control determines if it is a command frame or not*/
#define IsACommandFrame(pNpduFrame) ((pNpduFrame->nwkHr.aFrameControl[ 0 ] & gFrameType_c) == gCommandFrame_c)

/* based on agive contrl frame determine if the packet is valid */
#define IsNotAZigBeePacket(pNpduFrame) ((pNpduFrame->nwkHr.aFrameControl[0] & gPacketTypeMask_c) == gReservedFrame_c)

extern uint8_t iHtteIndexActualEntry;

/* Event for EA */ 
#define gNWK_ZDO_EA_Start_c     (1<<15)

/* EA Queue where will be stored both address of the device to authenticate ( ZDO y NWK )*/
extern anchor_t gNwkEaReuDataQueue;


/* Network State type definition */
typedef uint8_t nwkState_t;

/* Network layer states */
#define gOffTheNetwork_c    0x00
#define gOnTheNetwork_c     0x01


/******************************************************************************
*******************************************************************************
* Public prototypes
*******************************************************************************
******************************************************************************/
/******************************************************************************
* This function gets the pointer to the nwkBeaconNotifyInd_t structure.
* The NeighborTable is updated with the information obtained from the
* panDescriptor of BeaconIndication.
*
* Interface assumptions:
*   1)rxonwhenidle == TRUE for devices sending the beacon for which beacon
*   indication is received.
*   2)BeaconIndication will be obtained only if the Beacon contains the
*     BeaconPayload.
*
* Return value:
*   void
*
* Effects on global data:
*   None
*
*
******************************************************************************/

void ParseBeaconNotifyIndication
  (
  nwkBeaconNotifyInd_t *pBeaconIndication/*IN -Ptr to BeaconIndication sent
                                               by Mac*/
  );

#if( gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d )

/****************************************************************************
* Set the PIB association permit flag as well as the NIB permit join flag
* and save new values into nvm
*****************************************************************************/ 
uint8_t SetPermitJoinAttribute(uint8_t permitDuration);


/****************************************************************************
* This function removes all records associated to the leaving device from our
* neighbor table, route table, etc.
*****************************************************************************/ 
void NWK_RemoveChildRecords(zbIeeeAddr_t aLeavingIeeeAddress);


#if( gNwkPanIdConflict_d )
/******************************************************************************
* This function gets the pointer to the nwkBeaconNotifyInd_t structure.
* This fucntion only compare the pan identifier and extended pan identifier 
* values of the beacon with the device receiver to detect a pan identifier conflict.
*
* Interface assumptions:
*   1)rxonwhenidle == TRUE for devices sending the beacon for which beacon
*   indication is received.
*   2)BeaconIndication will be obtained only if the Beacon contains the
*     BeaconPayload.
*
* Return value:
*   void
*
* Effects on global data:
*   None
*
*
******************************************************************************/
void ParseBeaconNotifyIndicationAndDetectPanIdConflict
  (
  nwkBeaconNotifyInd_t *pBeaconIndication  /*IN: Ptr to BeaconIndication sent by Mac */
  );

extern void ActualDetectPanIdConflict(nwkBeaconNotifyInd_t * pBeaconIndication);

extern void ActualProcessPanIdConflictDetected(void);

#endif( gNwkPanIdConflict_d )


#if( gZigbeeProIncluded_d && gRnplusCapability_d )
extern uint8_t NwkGetSourceRouteSubframeSize(uint8_t *pDstAddress);
#endif

#endif( gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d ) 

/******************************************************************************
* This function resets the Nwk and Mac layer
******************************************************************************/
void ResetNwkAndMacLayer ( bool_t fFullReset );

/****************************************************************************
* Look for an empty entry in Discovery Tables
*****************************************************************************/ 
index_t GetFreeEntryInDiscoveryTable
( 
  zbNwkAddr_t aNwkAddr,
  zbPanId_t aPanId, 
  uint8_t channel,
  uint8_t stackProfile,
  bool_t reuse     /* indicate that it will be consider as empty any device at capacity */
);

/****************************************************************************
* This function frees allocated memory used by discovery taqbles, it should
* be called once the discovery tables are not needed any more
*****************************************************************************/ 
void NWK_FreeUpDiscoveryTables( void );


/************************************************************************************
* Calculates the size of the Nwk Header Frame using the Frame control field from
* the NPDU. 
*
* Frame Control Field bits, see 053474r17 - Sec. 3.3.1.1
* Bits:
* 0-1: Frame type
* 2-5: Protocol version
* 6-7: Discover route
*   8: Multicast flag
*   9: Security
*  10: Source Route
*  11: Destination IEEE Address
*  12: Source IEEE Address
* 13-15: Reserved
*
* Interface assumptions:
* The incoming parameter pFrameControl is a not null pointer, and has a valid Frame
* control information.
*
* Return value:
* The size in bytes of the current Nwk header (NPDU) frame.
************************************************************************************/
uint8_t NWK_GetNwkHeaderSize( nwkHeader_t *pNwkHeader );


/******************************************************************************
*******************************************************************************
* Public type definitions
*******************************************************************************
******************************************************************************/
  /* NONE */

/* Nwk Msg types*/
#define gNldeDataRequest_c                0x33
#define gNlmeNetworkDiscoveryRequest_c    0x34
#define gNlmeNetworkFormationRequest_c	  0x35
#define gNlmePermitJoiningRequest_c		    0x36
#define gNlmeStartRouterRequest_c		      0x37
#define gNlmeJoinRequest_c		            0x38
#define gNlmeDirectJoinRequest_c		      0x39
#define gNlmeLeaveRequest_c	              0x3A
#define gNlmeResetRequest_c	              0x3B
#define gNlmeSyncRequest_c		            0x3C
#define gNlmeGetRequest_c                 0x3D
#define gNlmeSetRequest_c		              0x3E
#define gNldeDataConfirm_c                0x3F
#define gNldeDataIndication_c		          0x40
#define gNlmeNetworkDiscoveryConfirm_c		0x41
#define gNlmeNetworkFormationConfirm_c		0x42
#define gNlmePermitJoiningConfirm_c 		  0x43
#define gNlmeStartRouterConfirm_c         0x44
#define gNlmeJoinIndication_c             0x45
#define gNlmeJoinConfirm_c                0x46
#define gNlmeDirectJoinConfirm_c          0x47
#define gNlmeLeaveConfirm_c               0x48
#define gNlmeLeaveIndication_c            0x49
#define gNlmeResetConfirm_c               0x4A
#define gNlmeSyncIndication_c             0x4B
#define gNlmeSyncConfirm_c                0x4C
#define gNlmeGetConfirm_c                 0x4D
#define gNlmeSetConfirm_c                 0x4E
#define gNlmeRouteDiscoveryReq_c	        0x4F
#define gNlmeRouteDiscoveryCnf_c          0x50
#define gNlmeNetworkStatusInd_c           0x51
#define gNlmeNetworkStatusRequest_c       0x67
#define gNlmeNetworkStatusCnf_c           0x68
#define gToggleConcentratorDiscTimeReq_c  0x71
#define gToggleConcentratorDiscTimeCnf_c  0x72

/* Vendor Specific Attibute*/
#define gNlmeChangePollTimeInterval_c     0x53
#define gNlmeEnergyScanRequest_c          0x54
#define gNlmeEnergyScanConfirm_c          0x55
#define gNlmeSetChannelRequest_c          0x56
#define gNlmeSetChannelConf_c             0x57

/*The gNlmeTxReport_c is mainly used and added for the Frequency Agility*/
#define gNlmeTxReport_c                   0x58

/* Data.request and Data.Indication */
#define gNwkSourceRouteDataRequest_c      0x60
#define gNwkSourceRouteDataIndication_c   0x61

#define gNwkError_c                       0x66

/* Nwk Security confirm code */
#define gNwkProcessFrameConf_c            0x70

/* Error codes */
enum
{
  errorNoError,
  errorWrongConfirm,
  errorNotSuccessful,
  errorNoMessage,
  errorAllocFailed,
  errorInvalidParameter,
  errorNoScanResults,
  errorJoinFailed,
  errorNoNetworks,
  LeaveOperationComplete,
  CoordLeavingNwkComplete
};


typedef enum tNetworkEnumDescriptor
{
  gNwkRouteRepairUnderway_c           =0xCF,
}tNetworkEnumDescriptor;

/*
  TimerID declaration. Timer 0 to 4 is just for End devices and
  Timers of 2 to 10 for routers and coordinator 
*/
#define gRejoinRspTimerID                 gNwkTimerIDs[0]
#define gPollTimerID                      gNwkTimerIDs[1]
#define gLeaveTimerID                     gNwkTimerIDs[2]
#define gBroadcastJitterTimerID           gNwkTimerIDs[3]
#define gBroadcastDeliveryTimerID         gNwkTimerIDs[4]
#define gPermitJoinTimerID                gNwkTimerIDs[5]
#define gRREQJitterTimerID                gNwkTimerIDs[6]
#define gRouteDiscoveryTimerID            gNwkTimerIDs[7]
#define gLinkStatusTimerID                gNwkTimerIDs[8]
#define gLinkStatusMultipleFramesTimerID  gNwkTimerIDs[9]
#define gAgingNTEntriesTimerID            gNwkTimerIDs[10]
#define gRouteRepliesTimerID              gNwkTimerIDs[11]
#define gExpireRouteEntriesTimerID        gNwkTimerIDs[12]
#define gNwkRejoinStateMachineTimerID     gNwkTimerIDs[13]
/* Size of vector timer */
#define gMaxVectorTimerSize_c       14

/* Size of vector timer for end devices sleeping or not */
#define gVectorTimerSizeForRFD_c    5  

/* Index of vector timer for routers and coordinator */
#define gVectorTimerIndexForFFD_c   2  


extern tmrTimerID_t gNwkTimerIDs[];
extern const uint8_t gNumberOfNwkTimers;


/* Route Status Values ( r13 - Table 3.47 ) */
#define gActive_c             0x00
#define gDiscoveryUnderway_c  0x01
#define gDiscoveryFailed_c    0x02
#define gInactive_c           0x03
#define gValidationUnderway_c 0x04
/* Vendor specific */
#define gRouteTableDefaultStatusValue_c gInactive_c

/* This line was removed from NwkDataServices.c L:91 and put here. */

/* defined in 802.15.4 spec */
#define aResponseWaitTime_c	400

/* Network Report Information List size */
#define gReportInformationListSize_c 8
/*  ==============================
      NWK to APS Data primitives
    ============================== */
enum {
  gNldeDataReq_c = 51,
  gNldeDataCnf_c = 63,
  gNldeDataInd_c
};

typedef struct nldeDataCnf_tag {
  uint8_t nsduHandle;
  uint8_t status;
} nldeDataCnf_t;

/* NLDE to APS message */
typedef struct nldeApsMessage_tag {
  uint8_t msgType;
  union {
    nldeDataCnf_t dataConf;
    nldeDataIndication_t dataIndication;
  } msgData;
} nldeApsMessage_t;

#if gStandardSecurity_d || gHighSecurity_d
extern void ZDO_SecClearDeviceData(zbNwkAddr_t aNwkAddr, zbIeeeAddr_t aIEEEAddr, bool_t fullErase);
#endif

/******************************************************************************
*******************************************************************************
* Public functions
*******************************************************************************
******************************************************************************/
/*  ==============================
      InterPan to App Data primitives
    ============================== */
enum {
  gInterPanDataReq_c = 0x00,
  gInterPanDataCnf_c = 0x01,
  gInterPanDataInd_c = 0x02
};

/* IntrPan to App message */
typedef struct InterPanAppMessage_tag {
  uint8_t msgType;
  union {
    zbInterPanDataConfirm_t InterPandataConf;
    zbInterPanDataIndication_t InterPandataIndication;
  } msgData;
} InterPanMessage_t;

typedef struct AppInterPanMessage_tag {
  uint8_t msgType;
  union {
  zbInterPanDataReq_t InterPanDataReq;
  } msgData;
} AppInterPanMessage_t;

typedef struct InterPanAddrInfo_tag
{
  zbAddrMode_t  srcAddrMode;    /* indirect, group, direct-16, direct-64 */
	zbAddrMode_t  dstAddrMode;    /* indirect, group, direct-16, direct-64 */
	zbPanId_t     dstPanId;
	zbApsAddr_t   dstAddr;        /* short address, long address or group (ignored on indirect mode) */
	zbProfileId_t aProfileId;     /* application profile (either private or public) */
	zbClusterId_t aClusterId;     /* cluster identifier */
} InterPanAddrInfo_t;


extern uint8_t APP_InterPan_SapHandler(AppInterPanMessage_t *pMsg);
extern void InterPan_ProcessDataIndication(mcpsToNwkMessage_t *pMsg);
extern void InterPan_ProcessConfirm(uint8_t msduHandle, uint8_t status);
/*Note this function must be present in "application space"*/
extern uint8_t InterPan_APP_SapHandler(InterPanMessage_t *pMsg);

#endif
                           
