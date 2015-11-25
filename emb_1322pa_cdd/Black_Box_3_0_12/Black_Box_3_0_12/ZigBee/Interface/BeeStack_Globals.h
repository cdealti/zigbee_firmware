/******************************************************************************
* This public header file provides structure definition for different table
* structure whose size can be configured by the application. To configure the
* the table size the user can use BeeStackConfiguration.h file
*
* Copyright (c) 2008, Freescale, Inc.  All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*******************************************************************************/

#ifndef _BEESTACK_GLOBALS_H_
#define _BEESTACK_GLOBALS_H_
#ifndef __IAR_SYSTEMS_ICC__
#ifdef MC13226Included_d
    #undef MC13226Included_d
#endif    
    #define MC13226Included_d 0
#endif
#include "EmbeddedTypes.h"
#include "BeeStackFunctionality.h"
#include "BeeStackConfiguration.h"
#include "zigbee.h"
#include "BeeCommon.h"
#include "MsgSystem.h"
#include "TS_Interface.h"
#include "TMR_Interface.h"
#include "AfApsInterface.h"
#include "NwkMacInterface.h"


/******************************************************************************
*******************************************************************************
* Public macros
*******************************************************************************
******************************************************************************/
/* Number of Channel available (do not redefined 802.15.4 standard) */
#define gNumOfChannels_c                  16

/* ZDO NVM address offset for Pan ID */
#define gPanIdNVMOffset_c                 60

/*
  Default Value of Beacon Order. Not used presently in ZigBee. Set to 0x0F (no
  beacons) always.
*/
#ifndef mDefaultValueOfBeaconOrder_c
#define mDefaultValueOfBeaconOrder_c    0x0F
#endif

#define   dummyDescCapField   0x00

#if gNwkMulticastCapability_d
 #define mUseMultiCast_c mDefaultValueOfNwkUseMulticast_c
 #define mUseMultiCastForApsGroupDataRequest_c mDefaultUseMultiCastForApsGroupDataRequest_c
#else
 #define mUseMultiCast_c FALSE
 #define mUseMultiCastForApsGroupDataRequest_c FALSE
#endif
/*******************************************************************************
********************************************************************************
* Public prototypes
********************************************************************************
*******************************************************************************/

/*******************************************************************************
* This function is invoked when the Mac layer sends Active scan
* confirmation. Here a logical channel to form a Network is selected
* The criteria for Selecting the Channel is the first one with Zero
* networks or the one containing the least number of PAN's"
*
* Interface assumptions:  This function is called only if ED Scan Confirm is
* obtained from the MAC.
* Return value:  void
*
* Effects on global data:
*  gSelectedLogicalChannel  - This will select the Channel based on Active Scan
*                             confirm
******************************************************************************/
void SelectLogicalChannel
  (
  const nwkMessage_t *pMsg, /* IN - Pointer to the Active Scan Confirm */
  uint8_t* pScanChannels,
  uint8_t* pSelectedLogicalChannel
  );

/***************************************************************************
* Using the already collected information onthe discovery process and the
* Energy scan, gathers the information and choose which channel is the one
* with less networks on it, using the channel list as a reference and then
* returns the channel number (11 .. 26).
*
* Interface assumptions:
*   The parameter pChannelList is not a null pointer.
*
* Return value:
*   The number id of the channel with less network available (11 .. 26).
****************************************************************************/
uint8_t SelectChannelFromTables
(
  uint8_t  *pChannelList  /* IN: The list of channel to be consider during the forming. */
);



/******************************************************************************
* This fuction is used to Select the PanId for the device
* that wants to form a Network when Upper layer specifies NULL as PanId.
*
* Interface assumptions:
*   None    Called only when the Pan Id specified by Upper layer is NULL.
* Return value:   void
*
* Effects on global data:
*   gaNibPanId Pan Id is generated.
*******************************************************************************/
bool_t SelectPanId
  (
  const nwkMessage_t *pMsg,  /* IN/OUT -Pointer to Active Scan Confirm Paramters */
  uint8_t SelectedLogicalChannel,
  zbPanId_t aPanId
  );

/***************************************************************************
* Using the Pan Id provided in the formation request, search on the
* discovery tables to see if the given pan Id is not on conflict. Or if the
* Given Pan Id is not a valid one then generate one and check for conflicts.
*
* Interface assumptions:
*   The parameter aPanId is a valid memory space and an input and output
*   parameter.
*   The parameter channel is avalid cahnnel number and is an input parameter
*   only.
*
* Return value:
*   TRUE if there is a conflict. FALSE other wise.
****************************************************************************/
bool_t SelectPanIDFromTables
(
  zbPanId_t  aPanId,  /* IN/OUT */
  uint8_t  channel    /* IN */
);

/***************************************************************************
* Generates a random and valid ZigBee PanId.
*
* Interface assumptions:
*   The parameter aPanId is a valid memory location. aPanI is an output
*   parameter.
*
* Return value:
*   NONE.
****************************************************************************/
void getRandomPanId
(
  zbPanId_t  aPanId  /* OUT: This parameter is used as an output. */
);



/**********************************************************************************
*
* Cheks if in a particular channel exists another network using the same Extended
* Pan Id we are tryig to use to form our network.
* Returns True if there is a confilct and False if there
*
**********************************************************************************/
bool_t CheckForExtendedPanIdConflict
  (
  uint8_t logicalChannel /* IN - Log.Channel for determining PanId */
  );

/****************************************************************************/
index_t SearchForSuitableParentToJoin ( void );


/******************************************************************************
 Init the Address Map Table (fill it with zeros).
*******************************************************************************/
void AddrMap_InitTable(void); 

/******************************************************************************
 Get an entry from the Address Map Table.
*******************************************************************************/
uint8_t AddrMap_GetTableEntry(uint8_t index, void *pEntry);

/******************************************************************************
  Search IEEE address OR short address in the Address Map Table.
*******************************************************************************/
uint8_t AddrMap_SearchTableEntry(zbIeeeAddr_t   *pIeeeAddr,
                                 zbNwkAddr_t    *pNwkAddr, 
                                 void           *pEntry);
                                 
/******************************************************************************
 Set an entry in the Address Map Table.
*******************************************************************************/
uint8_t AddrMap_SetTableEntry(uint8_t index, void *pEntry);

/******************************************************************************
 Remove an entry from the Address Map Table.
*******************************************************************************/
uint8_t AddrMap_RemoveEntry(uint8_t index);

 /******************************************************************************
 Init the Device Key Pair Set table (fill it with zeros).
*******************************************************************************/
void DevKeyPairSet_InitTable(void);

/******************************************************************************
 Get an entry from the Device Key Pair Set table.
*******************************************************************************/
uint8_t DevKeyPairSet_GetTableEntry(uint8_t index, void *pEntry);

/******************************************************************************
 Set an entry in the evice Key Pair Set table.
*******************************************************************************/
uint8_t DevKeyPairSet_SetTableEntry(uint8_t index, void *pEntry);

/******************************************************************************
 Remove an entry from the Device Key Pair Set table.
*******************************************************************************/
uint8_t DevKeyPairSet_RemoveEntry(uint8_t index);

/******************************************************************************
 Init the Key Set table (fill it with zeros).
*******************************************************************************/
void KeySet_InitTable(void);

/******************************************************************************
 Get an entry(a key) from the Key Set table.
*******************************************************************************/
uint8_t KeySet_GetTableEntry(uint8_t index, void *pKey);

/******************************************************************************
 Set an entry (a key) in the Key Set table.
*******************************************************************************/
uint8_t KeySet_SetTableEntry(uint8_t index, void *pKey);

/******************************************************************************
 Remove an entry (a key) from the Key Set table.
*******************************************************************************/
uint8_t KeySet_RemoveEntry(uint8_t index);

/******************************************************************************
  Search a key in the Key Set table.
*******************************************************************************/
uint8_t KeySet_SearchTableEntry(zbAESKey_t *pKey);


/****************************************************************************
 Public type definitions
*****************************************************************************/
/****************************************************************************/

/*******************************************************************************
********************************************************************************
* APS definitions
********************************************************************************
*******************************************************************************/
/**** Fragmentation Capability Types ***********************************************/
/* type returned by retry queue */
typedef index_t apsTxIndex_t;
#define gApsTxTableFull_c gMaxIndex_c
#define gApsTxEntryNotFound_c  gMaxIndex_c

/* define a set of states */
typedef uint8_t apsTxState_t;
enum {
  gApsTxState_Unused = 0,               /* entry not used */
  gApsTxState_WaitingToTx_c = 1,        /* have big buffer, waiting to Tx (waiting for lock) */
  gApsTxState_WaitingToAllocMsg_c,      /* ready to retry, waiting for buffer */
  gApsTxState_WaitingForNldeConfirm_c,  /* sent, waiting for confirm */
  gApsTxState_GotNldeConfirm_c,         /* got the NLDE confirm */
  gApsTxState_WaitingForAck_c,          /* sent, waiting for APS ACK (indication) */
  gApsTxState_WaitingInterframeDelay_c, /* fragmentation: waiting to send next block this window */
  gApsTxState_PrepareNextBlock_c,       /* fragmentation: create next OTA fragment */
  gApsTxState_GotAck_c,                 /* got APS ACK (indication) */
  gApsTxState_GotAckTimeout_c,          /* ACK timeout */
  gApsTxState_SendConfirm_c,            /* got confirm, waiting to alloc/send it */
  gApsTxState_FindNextBindEntry_c,      /* done with last entry, look for next bind entry */
  gApsTxState_WaitingForIndirect_c,     /* need to wait between indirect msgs */
  gApsTxState_InitFrag_c,               /* initialize the fragmentation packets */
  gApsTxState_Done_c,                   /* done with entry, clean it up */
  gApsTxState_SendNwkAddrReq_c,
  gApsTxState_SearchingNwkAddr_c,        /* searching Nwk address request for an invalid binding*/
  gApsTxState_NwkAddrRspTimeOut_c
};

#ifdef __IAR_SYSTEMS_ICC__
#pragma pack(1)
#endif
/* define a set of states */
typedef uint8_t apsRxState_t;
enum {
  gApsRxState_Unused_c =0,          /* not in use */
  gApsRxState_GotFirstBlock_c,      /* received first block */
  gApsRxState_WaitingBetweenAcks_c, /* waiting for another block after we sent an ACK (long wait) */
  gApsRxState_WaitingForBlock_c,    /* waiting for another block in this window (short wait) */
  gApsRxState_GotBlock_c,           /* got a block. Time to move on to a new window? */
  gApsRxState_SendAck_c,            /* time to send ACK to Tx side */
  gApsRxState_SendIndication_c      /* time to send indication to app */
};
#ifdef __IAR_SYSTEMS_ICC__
#pragma pack()
#endif

typedef uint8_t apsTxFlags_t;
#define gApsTxFlags_Fragmented_c 0x01   /* fragmented transmission */
#define gApsTxFlags_Indirect_c   0x02   /* indirect transmission */


/* Type to know if a continue is necessary after calling a fragmentation state */
typedef uint8_t apsTxResult_t;
#define gApsTxResult_DontContinue_c   FALSE
#define gApsTxResult_Continue_c       TRUE

typedef uint8_t frmCapAndReqStatus;
#define frmEnabledAndReq  0x03
#define frmEnabledAndNotReq 0x02
#define frmDisabledAndReq 0x01
#define frmDisabledAndNotReq 0x00

/*=================================================================================
===================================================================================
   Network Layer Tables
===================================================================================   
=================================================================================*/
#ifdef __IAR_SYSTEMS_ICC__
#pragma pack(1)
#endif
typedef struct rteProperties_tag
  {
#ifdef __IAR_SYSTEMS_ICC__    
    /*The status of the route. See Table 3.52 for values (Zigbee spec r17)*/
    uint8_t status              :3;
    /*A flag indicating that the destination indicated by this address does not store source routes.*/
    uint8_t noRouteCache        :1;
    /*A flag indicating that the destination is a concentrator that issued a many-to-one route request.*/
    uint8_t manyToOne           :1;
    /*A flag indicating that a route record command frame should be sent to the destination prior to the next data packet.*/
    uint8_t routeRecordRequired :1;
    /*A flag indicating that the destination address is a Group ID.*/
    uint8_t groupIdFlag         :1;
    /* This bit will be used to control the sent of the route record to a network concentrator */
    uint8_t AllAlreadySentItsRouteRecord  :1;
#else
    /*The status of the route. See Table 3.52 for values (Zigbee spec r17)*/
    unsigned int status              :3;
    /*A flag indicating that the destination indicated by this address does not store source routes.*/
    unsigned int noRouteCache        :1;
    /*A flag indicating that the destination is a concentrator that issued a many-to-one route request.*/
    unsigned int manyToOne           :1;
    /*A flag indicating that a route record command frame should be sent to the destination prior to the next data packet.*/
    unsigned int routeRecordRequired :1;
    /*A flag indicating that the destination address is a Group ID.*/
    unsigned int groupIdFlag         :1;
    /* This bit will be used to control the sent of the route record to a network concentrator */
    unsigned int AllAlreadySentItsRouteRecord  :1;
#endif    
  } rteProperties_t;
#ifdef __IAR_SYSTEMS_ICC__
#pragma pack()
#endif
/**** Backup Nwk Info ************************************************************/
/* This structure saves the nwk information in case mac association fails and */ typedef struct nwkDataBackup_tag{
  /*The 16-bit PAN identifier of the neighboring device.*/
  zbPanId_t aPanId;

    /*The 64-bit Extended PAN identifier of the neighboring device.*/
  zbIeeeAddr_t aExtendedPanId;

  /*The 16-bit network address of the neighboring device.*/
  zbNwkAddr_t aNetworkAddress;
}nwkDataBackup_t;


/**** Routing table **************************************************************/
/* Stucture for Route table Elements ( r13 - Table 3.46 ) */
typedef struct routeTable_tag
{
  /*Destination Address for which the route is discovered*/
  zbNwkAddr_t aDestinationAddress;

  /*Next hope Address to the discovered Destination*/
  zbNwkAddr_t aNextHopAddress;

  rteProperties_t properties;

  /* Used to reuse the route table entry */
  uint8_t age;

} routeTable_t;

/**** RouteDiscoveryTable ********************************************************/
/* Structure of route discovery table. ( r13 - Table 3.48 ) */
typedef struct routeDiscoveryTable_tag
{
  /* A sequence number for a route request command frame that is incremented
  each time a device initiates a route request.*/
  uint8_t  routeRequestId;

  /*The 16-bit network address of the route request’s initiator.*/
  zbNwkAddr_t aSourceAddress;

  /*The 16-bit network address of the device that has sent the most recent
  lowest cost route request command frame corresponding to this entry’s
  Route request identifier and Source address. This field is used to determine
  the path that an eventual route reply command frame should follow.*/
  zbNwkAddr_t aSenderAddress;

  /*
    ZigBee Spec. 053474r17
    3.4.1.2 NWK Header Fields 
    The source IEEE address sub-field of the frame control field shall be set to 1
    and the source IEEE address field of the NWK header shall be present and shall
    contain the 64-bit IEEE address of the originator of the frame.
  */
  zbIeeeAddr_t aIEEESrcAddr;

  /* Destination IEEE address in command options */
  zbIeeeAddr_t aIEEEDstAddrCmdOptions;
  
	/*The accumulated path cost from source of the route request to the current
  device.*/
	uint8_t  forwardCost;

	/*The accumulated path cost from the current device to the destination
  device.*/
  uint8_t  residualCost;

	/*A countdown timer indicating the number of ticks until route discovery expires.*/
  uint8_t  expirationTicks;

	/*A countdown timer indicating the number of ticks until route discovery is retransmited */
  uint8_t  retriesTicks;

	/* Holds the number of retries pending for this route request */
  uint8_t  retriesLeft;

	index_t indexInRouteTable;

	uint8_t sequenceNumber;

	uint8_t radius;

} routeDiscoveryTable_t;

/************* Source Route Table  ***********************************************/
/* Structure for source route table 
   ZigBee Spec. 053474r17
   Table 3.45 Route Record Table Entry Format
*/ 
typedef struct sourceRouteTable_tag{

  /* The destination network address for this route record. */
  zbNwkAddr_t aDestinationAddress;

  /* The count of relay nodes from concentrator to the destination.*/
  uint8_t relayCount;

  /* Age field. Is settable option */
  uint8_t age;

  /* The set of network addresses that represent the route  from  concentrator to destination.*/
  RouteRecordRelayList_t *path;

} sourceRouteTable_t;

/**** Broadcast Transaction Buffer ***********************************************/
typedef struct broadcastBuffer_tag
{
  /* Holds the length of the stored NPDU */
  uint8_t npduLength;

  /* Holds the pointer to the stored NPDU */
  npduFrame_t *pNpdu;

}broadcastBuffer_t;


/**** Broadcast Transaction Table ************************************************/
typedef struct broadcastTransactionTable_tag
{

  /* Holds the Source Short Address of the broadcast transaction */
  zbNwkAddr_t aSourceAddress;

  /* Holds the Sequence number of the broadcast transaction */
  uint8_t sequenceNumber;

	/* Holds the handle to identify mac confirmation for this broadcast instance*/
	uint8_t msduHandle;

	bool_t confirmPending;

  /* number of ticks to expire this broadcast transaction */
  uint8_t deliveryTicks;

#if gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d || MC13226Included_d

  /* Holds the status whether broadcast is in progress or not */
  uint8_t retriesTicks;

	/* Holds the number of retries pending for this broadcast transaction */
  uint8_t retriesLeft;

	/* Data needed for routers or coordinator */
	broadcastBuffer_t  bufferedData;

#endif

}broadcastTransactionTable_t;

#ifdef __IAR_SYSTEMS_ICC__
#pragma pack(1)
#endif
/*********************************************************************************/
typedef union {
  uint8_t allBits;
  struct {
#ifdef __IAR_SYSTEMS_ICC__    
    /*
      deviceType 
         0b00 -- ZigBee Coordinator.
         0b01 -- ZigBee Router.
         0b10 -- ZigBee EndDevice.
         0b11 -- Device Mask.
    */
    uint8_t deviceType     :2;
    /**/
    uint8_t rxOnWhenIdle   :1;

    /*
      (0) = 0b000 - Is a parent
      (1) = 0b001 - Is a children
      (2) = 0b010 - Is a silbing
      (3) = 0b011 - Is not related.
      (4) = 0b100 - Is previous child
      (5) = 0b101 - Is un-authenticated
      (6) = 0b110 - Is an expired children.
    */
    uint8_t relationship   :3;
    /**/
    uint8_t linkStatus     :1;
    /**/
    uint8_t reuseAddress   :1;
#else
     /*
      deviceType 
         0b00 -- ZigBee Coordinator.
         0b01 -- ZigBee Router.
         0b10 -- ZigBee EndDevice.
         0b11 -- Device Mask.
    */
    unsigned int deviceType     :2;
    /**/
    unsigned int rxOnWhenIdle   :1;

    /*
      (0) = 0b000 - Is a parent
      (1) = 0b001 - Is a children
      (2) = 0b010 - Is a silbing
      (3) = 0b011 - Is not related.
      (4) = 0b100 - Is previous child
      (5) = 0b101 - Is un-authenticated
      (6) = 0b110 - Is an expired children.
    */
    unsigned int relationship   :3;
    /**/
    unsigned int linkStatus     :1;
    /**/
    unsigned int reuseAddress   :1;
#endif    
  } bits;
} nteProperties_t;

#define gZbDeviceTypeMask_c  (0b11)

/**** Neighbor Table *************************************************************/
/*This structure holds the information of the Additional Neighbour Table
	r13 - Table 3.44 Additional Neighbor Table Fields */
typedef struct neighborTable_tag{

  /*64-bit IEEE address that is unique to every device.*/
  zbIeeeAddr_t aExtendedAddress;

  /*The 16-bit network address of the neighboring device.*/
  zbNwkAddr_t aNetworkAddress;

  /* a set of sub-fields associated to this entry */
  nteProperties_t deviceProperty;

  /*A value indicating if previous transmissions to the device were
  successful or not. Higher values indicate more failures.*/
  uint8_t transmitFailure;

  /*The estimated link quality for RF transmissions from this device.*/
  uint8_t lqi;

  /* 053474r17 ZigBee Spec. 
     Sec. Table 3.48 Neighbor Table Entry Format
     The cost of an outgoing link as measured by the neighbor. A valueof 0 indicates no 
     outgoing cost is available.

     This field is mandatory if nwkSymLink = TRUE.*/
  zbCounter_t outgoingCost;

    /* 053474r17 ZigBee Spec. 
     Sec. Table 3.48 Neighbor Table Entry Format
     The number of nwkLinkStatusPeriod intervals since a link status command was received. 

     This field is mandatory if nwkSymLink = TRUE.*/
  zbCounter_t age;

#if ( gBeaconSupportCapability_d )

  /*The time, in symbols, at which the last beacon frame was received from
  the neighbor. This value is equal to the timestamp taken when the beacon
  frame was received,*/
  uint8_t aIncomingBeaconTimeStamp[3];

  /*The transmission time difference,in symbols, between the neighbor’s
  beacon and its parent’s beacon. This difference may be subtracted from the
  corresponding incoming beacon timestamp to calculate the beacon transmission
  time of the neighbor’s parent.*/
  uint8_t aBeaconTransmissionTimeOffset[3];

#endif /* ( gBeaconSupportCapability_d ) */

}neighborTable_t;


/**** Extended Neighbor Table ****************************************************/
typedef struct extendedNeighborTable_tag
{
  /*The 16-bit PAN identifier of the neighboring device.*/
  zbPanId_t aPanId;

	/*The 64-bit Extended PAN identifier of the neighboring device.*/
  zbIeeeAddr_t aExtendedPanId;

  /*The logical channel on which the neighbor is operating.*/
  uint8_t logicalChannel;

  /*The tree depth of the neighbor device. A value of 0x00 indicates that
  the device is the ZigBee coordinator for the network.*/
  uint8_t depth;

  zbCounter_t nwkUpdateId;
  
#if ( gBeaconSupportCapability_d )
  /*This specifies how often the beacon is to be transmitted.*/
  uint8_t beaconOrder ;
#endif /* ( gBeaconSupportCapability_d ) */

  /*joiningFeatures is a bit field containing the following bits
  Bit 0   -> permitJoining
  Bit 1   -> potential parent
  Bit 2-3 -> not used
  Bit 4-7 -> stack profile
  Note: This is a slight deviation from spec where these attributes are one byte*/
  uint8_t joiningFeatures;

}extendedNeighborTable_t;



/* handleTrackingTable_t status codes */
typedef uint8_t httStatus_t;
#define mHttFreeEntry_c           0x00
#define mHttDataImSending_c       0x01
#define mHttDataImRouting_c       0x02
#define mHttBroadcastForRxOff_c   0x03

#define mHttSuppressRouteDiscovery     0x80
#define mHttStatusMask                 0x7F

/**** HandleTrackingTable ************************************************************/
typedef struct handleTrackingTable_tag{
  /* entry status */
  httStatus_t status;
  /* Indicate if the msg is a data frame - nldedataReq, indicat, source route.-*/
  uint8_t msgType;
  /* msduHandle awaiting Confirmation */
  zbCounter_t handle;
  /* index pointing to a neighbor table entry */
  index_t iNteIndex;
  /* Mac src addres used to control the many-to-one route error */
  zbNwkAddr_t aMacSourceAddress;
  /* Source Address of this data request - NwkHeader.aSourceAddress- */
  zbNwkAddr_t aSourceAddress;
  /* final destination address of this data request */
  zbNwkAddr_t aDestinationAddress;
} handleTrackingTable_t;


/* packetsOnHoldTable_t status codes */
typedef uint8_t pohtStatus_t;
#define mPohtFreeEntry_c          0x00
#define mPohtDataFromNhl_c        0x01
#define mPohtDataFromNll_c        0x02
#define mReadyToSend_c            0x03
/**** PacketsOnHoldTable *************************************************************/
typedef struct packetsOnHoldTable_tag{
  /* entry status */
  pohtStatus_t status;

  uint8_t msgType;

  /* short address to where the packet should be send */
  zbNwkAddr_t aDestinationAddress;

  /* packet waiting to be routed */
  uint8_t *pPacketOnHold;
  uint8_t age;
} packetsOnHoldTable_t;


/*
  This data type is used to handle each rejoin request to be send out, keeps aunique handler
  in order to respond to more than one rejoin reqwuest at the same time
*/
typedef struct nwkRejoinEntry_tag
{
  zbCounter_t  nsduHanlde;
  index_t      iIndex;
  zbNwkAddr_t  aNewAddress;
  bool_t       wasSecure;
}nwkRejoinEntry_t;

/*=================================================================================
===================================================================================
  APS Layer Tables
===================================================================================
=================================================================================*/


/* address map index type */
typedef index_t addrMapIndex_t;
#define gNotInAddressMap_c  (gMaxIndex_c-1)
#define gAddressMapFull_c   (gMaxIndex_c-1)

/* indicate destination is group, not IEEE address (flag multiplexed on dstEndPoint field) */
/* otherwise, dstEndPoint will be 0x01 - 0xfE */
#define gApsBindAddrModeGroup_c 0x00

typedef union apsBindingDst_tag
{
  addrMapIndex_t  index;
  zbGroupId_t     aGroupId;
} apsBindingDst_t;

/* BeeStack in-memory representation of a binding table entry */
typedef struct apsBindingTable_tag
{
  zbEndPoint_t    srcEndPoint;      /* always 0x01-0xf0 */
  apsBindingDst_t dstAddr;          /* either group id or address map index */
  zbEndPoint_t    dstEndPoint;      /* multiplex dst endpoint or group  */
  index_t         iClusterCount;    /* count of (packed) clusters in list */
  zbClusterId_t   aClusterList[gMaximumApsBindingTableClusters_c];   /* list of clusters bound for this destination */
}apsBindingTable_t;


/* Pointer type for the binding table to be used in the library. */
typedef struct apsBindingTable_Ptr_tag
{
  zbEndPoint_t    srcEndPoint;      /* always 0x01-0xf0 */
  apsBindingDst_t dstAddr;          /* either group id or address map index */
  zbEndPoint_t    dstEndPoint;      /* multiplex dst endpoint or group  */
  index_t         iClusterCount;    /* count of (packed) clusters in list */
  zbClusterId_t   aClusterList[1];  /* list of clusters bound for this destination */
}apsBindingTable_Ptr_t;

/*
  calculating bytes needed to represent the end points bit mask for groups 
  1+ because at least one is needed
  +2 because 0xFF and 0x00 are hardcoded in index 0 and 1 then
  gNumberOfEndPoints_c only represents application specific end points
*/

#define gEndPointsMaskSizeInBytes_c  (1 + ((gNumberOfEndPoints_c + 2)>> 3))

typedef uint8_t  zbEpMask_t[gEndPointsMaskSizeInBytes_c];

/*
  Each group entry keeps track of which endpoints belong to this group. Used
  by APSDE_DataRequest to determine if the data indication will be propogated
  to the endpoint. Used by the APSME_xxx management functions.

  Note: gaEndPointDesc describes endpoints. It's an array that contains the
  following:
  [0] = ZDP
  [1] = broadcast endpoint
  [2-n] = user endpoints by # (1-240)

  References
  [R1] sections 2.2.4.5.1 thru 2.2.4.5.6
*/
typedef struct zbGroupTable_tag
{
    zbGroupId_t aGroupId;
    uint8_t  aEpMask[gEndPointsMaskSizeInBytes_c];  /* variable length array - 1 bit per endpoint */
}zbGroupTable_t;

typedef struct zbGroupTableStackPtr_tag
{
    zbGroupId_t aGroupId;
    uint8_t  aEpMask[1];  /* dummy array entry, but is the size of gEndPointsMaskSizeInBytes_c
                             used inside the stack lib */
}zbGroupTableStackPtr_t;

/*
  defines the a bit mask (in bytes) to be used withthe address map, each bit in this mask
  represents an entry in the addess map table.
  1+ because at least we need a byte no matter if address map is only one entry long.
  (gApsMaxAddrMapEntries_c >> 3) is the amount of bytes need according to amount of entries
  in the address map table.

  (*) The smalles amount of bytes is two.
*/
#define gApsAddressMapBitMaskInBits_c  (1 + (gApsMaxAddrMapEntries_c >> 3))

/*=================================================================================
===================================================================================
  AF Layer Tables
===================================================================================
=================================================================================*/

/************* For ZDO Layer ************/

/*union for address type*/

typedef union zdoAddress_tag {
  zbNwkAddr_t aShortAddress;   /* Short Address */
  zbIeeeAddr_t aLongAddress;  /* Ieee Address */
} zdoAddress_t;

/*Structure for addreass information of an End device*/
typedef struct zdoAddressInfo_tag {
  zbAddrMode_t	addressMode;
  zdoAddress_t zbAddress;
  zbEndPoint_t epAddr;    /*Endpoint Address*/
} zdoAddressInfo_t;

/* Typedef for Semiprecision data */
typedef uint16_t afSemiPrecision_t;

/*
  Max window size type, no  more than a byte according to the docuemtn 085023r05.
  Values: 1 - 8 valid window sizes.
  Zero means not supported.
  Values: 9 - 0xFF are invalid.
*/
typedef uint8_t zbMaxWindowSize_t;

/*
  endPointDesc_t

  this structure points not only to the simple descriptor (ZigBee endpoint
  structure), but also to callbacks for the indications.
*/
typedef struct endPointDesc_tag
{
  zbSimpleDescriptor_t *pSimpleDesc;
  dataMsgCallback_t  pDataMsgCallBackFuncPtr;
  dataConfCallback_t  pDataConfCallBackFuncPtr;
  /*
    apsMaxWindowSize for the Endpoint Number.
    Value of 1-8, and 0 for not supporting fragmentation.
  */
  zbMaxWindowSize_t  maxWindowSize;
} endPointDesc_t;

/* For run-time endpoints, there is an array of ptrs to the descriptions */
  typedef struct endPointPtrArray_tag {
    //const endPointDesc_t  *pDescription; /* Variable pointer to a const endPointDesc_t, the content of the pointer is constant. */
    endPointDesc_t  *pDescription;
} endPointPtrArray_t;


/*Power Descriptor Structure*/
typedef struct configPowerDescriptor_tag
{
    /*Current Mode and Available Source*/
    uint8_t     currModeAndAvailSources;
    /*Current Power Source And Lavel*/
    uint8_t     currPowerSourceAndLevel;
}configPowerDescriptor_t;

/*Simple Descriptor Structure*/
typedef struct configSimpleDescriptors_tag
{
  /*End point ID */
  uint8_t       endPoint;
  /*Application Profile ID*/
  uint8_t       aAppProfId[ 2 ];
  /*Appliacation Device ID*/
  uint8_t       aAppDeviceId[ 2 ];
  /*Application Device Version And APS Flag*/
  uint8_t       appDevVerAndFlag;
  /*Number of Input Cluster ID Supported by the End Point*/
  uint8_t       appNumInClusters;
  /*Place Holder for the list of Input Cluster ID*/
  uint8_t       aAppInClusterList[1];
  /*Number of Output Cluster ID Supported by the End Point*/
  uint8_t       appNumOutClusters;
  /*Place Holder for the list of Output Cluster ID*/
  uint8_t       aAppOutClusterList[1];
}configSimpleDescriptors_t;

/*Complex Descriptor Structure*/
typedef struct configComplexDescriptor_tag
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
}configComplexDescriptor_t;

/*User Descriptor Structure*/
typedef struct configUserDescriptor_tag
{
    /*user Descriptor*/
    zbUserDescriptor_t  aUserDescriptor;
}configUserDescriptor_t;

typedef uint8_t zbTimeType_t[2];

/* from commissioning cluster (064699r12) */
// see ZdoStartMode_t in 
#define gStartupControl_SilentStart_c   0x00  // already part of the network (no join needed)
#define gStartupControl_Form_c          0x01  // form a network with exteneded Pan Id described in the attribute "Extended Pan Id"(ZC only)
#define gStartupControl_NwkRejoin_c     0x02  // use NWK rejoin (ZR, ZED only)
#define gStartupControl_Associate_c     0x03  // use association (ZR, ZED only)

/* FS Specific Startup control modes. */
#define gStartupControl_OrphanRejoin_c  0x04  // FS specific: use orphan rejoin (ZR, ZED only)
#define gStartupControl_ChannelRejoin_c 0x05  // FS specific: use orphan rejoin (ZR, ZED only)

/* Commissioning Parameters structure, aka Startup Attribute Set or SAS (91 bytes) */
/* note: this structure is cross-layer */
typedef struct zbCommissioningAttributes_tag
{
  /*** Startup Attribute Set (064699r12, section 6.2.2.1) ***/
  zbNwkAddr_t aShortAddress;          /* 16bit network address of the device*/
  zbIeeeAddr_t aNwkExtendedPanId;     /* 64-bit extended PAN ID (nwkExtendedPanId) */
  zbIeeeAddr_t aApsUseExtendedPanId;  /* 64-bit extended PAN ID (apsUseExtendedPanId) */
  zbPanId_t aPanId;                   /* 16-bit PAN ID */
  zbChannels_t aChannelMask;          /* set of channels for */
  uint8_t protocolVersion;            /* selects the current protocol version (must be 0x02 for ZigBee 2006+) */
  uint8_t stackProfile;               /* 0x01 or 0x02 */
  uint8_t startupControl;             /* determines how certain other parameters will be used */  
  zbIeeeAddr_t aTrustCenterAddress;   /* used in security as extendedPanId */
  zbAESKey_t aTrustCenterMasterKey;   /* used during key stablishment with TC */
  zbAESKey_t aNetworkKey;             /* network key */
  bool_t fUseInsecureJoin;            /* use insecure join mechanism */
  zbAESKey_t aPreconfiguredTrustCenterLinkKey;   /* key between the device and the TC */
  zbKeySeqNumber_t activeNetworkKeySeqNum;  /* key sequence # of network key */
  zbKeyType_t networkKeyType;         /* key type for network key */
  zbNwkAddr_t aNetworkManagerAddress; /* where to find network manager (for FA) */

  /*** Join Parameters Attribute Set (064699r12, section 6.2.2.2) ***/
  uint8_t      scanAttempts;               /* # of scan attempts (default 5) */
  zbTimeType_t aTimeBetweenScans;     /* time between scans (default 100ms), little endian */
  zbTimeType_t aRejoinInterval;       /* rejoin interval in seconds, little endian */
  zbTimeType_t aMaxRejoinInterval;    /* in seconds, little endian */

  /*** End-Device Parameters Attribute Set (064699r12, section 6.2.2.3) ***/
  zbTimeType_t aIndirectPollRate;     /* poll time for messages from its parent */
  uint8_t parentLinkRetryThreshold;   /* # of missed parent access before finding new parent */

  /*** Concentrator Parameters Attribute Set (064699r12, section 6.2.2.4) ***/
  bool_t fConcentratorFlag;           /* is this a concentrator or not? */
  uint8_t concentratorRadius;         /* radius for many-to-one route discovery */
  uint8_t concentratorDiscoveryTime;  /* how often to send out many-to-one route discovery */
}zbCommissioningAttributes_t;

/* There are 4 copies of the startup attributes: ROM, RAM(working copy), NVM and commissioning cluster */
extern const zbCommissioningAttributes_t gSAS_Rom;        /* factory set */
extern zbCommissioningAttributes_t gSAS_Ram;              /* working set */
extern zbCommissioningAttributes_t *gpSAS_Commissioning;  /* commissioning cluster (optional) */
/* another copy is in NVM (copy of the gSAS_Ram) */

/* define function to set commissioning cluster SAS */
#define ASL_GetCommissioningSAS()     (gpSAS_Commissioning)
#define ASL_SetCommissioningSAS(pPtr) (gpSAS_Commissioning = (pPtr))

/*
  BeeStack Configurable Parameter Structure
  Warning: If you modify this structure from logicalType field to descCapField, need to modify
           also zbNodeDescriptor_tag in zigbee.h to make them match
*/
typedef struct beeStackConfigParams_tag
{
  /*Beacon Order*/
  uint8_t beaconOrder;                        /* ROM */

  /*Batter Life Extension is Enabled or not*/
  bool_t batteryLifeExtn;                     /* ROM */

  /*Logical Type*/
  //uint8_t logicalType;                        /* ROM */

  /* Frequency band used by the radio */
  uint8_t apsFlagsAndFreqBand;                /* ROM */

	/* The capability information */
	//uint8_t macCapFlags;
	
  /* Default Value of Manufacturer Code Flag */
  uint8_t manfCodeFlags[2];                   /* ROM */

  /* Default value of maximum buffer size */
  uint8_t maxBufferSize;                      /* ROM */

  /* default Value od Maximum Transfer Size */
  uint8_t maxTransferSize[2];                 /* ROM */

  /* The services server mask */
  uint8_t serverMask[2];

   /*Maximum Outgoing Transfer Size*/
  uint8_t	aMaxOutTransferSize[2];

  /* The descriptor capability field.	*/
  uint8_t descCapField;	

  /*Current Mode and Available Source*/
  uint8_t currModeAndAvailSources;

  /*Current Power Source And Lavel*/
  uint8_t currPowerSourceAndLevel;

  /* Number of Scan Attempts */
  uint8_t gNwkDiscoveryAttempts;              /* ROM */

  /*Nuber of Scan Attemp*/
  uint8_t gNwkFormationAttempts;              /* ROM */

  /*Nuber of Scan Attemp*/
  uint8_t gNwkOrphanScanAttempts;             /* ROM */

  /* timer value for time between the Discovery */
  tmrTimeInSeconds_t gDiscoveryAttemptsTimeOut;         /* ROM */

  /*timer value for time between the Formation*/
  tmrTimeInSeconds_t gFormationAttemptsTimeOut;         /* ROM */

  /*timer value for time between the Orphan scan*/
  tmrTimeInSeconds_t gOrphanScanAttemptsTimeOut;        /* ROM */

#if( gEndDevCapability_d || gComboDeviceCapability_d || MC13226Included_d)
  /* FS:SAS? Timer value for a device to poll for Authentication*/
  uint16_t gAuthenticationPollTimeOut;        /* ROM */

  /* FS:SAS? Timer value for a device to poll*/
  uint16_t gConfirmationPollTimeOut;          /* ROM */
#endif

  /* FS:SAS? Timeout value for authentication process*/
  uint32_t gAuthenticationTimeOut;            /* ROM */

  /* FS:SAS? Preconfigured Network key is used or not*/
  bool_t gNwkKeyPreconfigured;                /* ROM */

  /*Security Level*/
  uint8_t gNwkSecurityLevel;                  /* ROM */

  /*Network Fresh is enabled or not*/
  //bool_t gNwkAllFresh;

  /*Networ Secure all Frame is Enable d or not: Ignored in No Security*/
  bool_t gNwkSecureAllFrames;                 /* ROM */

  /*
    FS, internal parameter.
    Name: defaultPreconfigureTCKey
    Type: zbKeyType_t
    Range: 0x00 or 0x04
    Default: 0x00
  */
  zbKeyType_t  defaultPreconfigureTCKey;

  /*enable/disable of low power mode*/
	bool_t lpmStatus;

} beeStackConfigParams_t;

/* this structure defines general network data, much of it assigned at run-time */
typedef struct nvmNwkData_tag
{
  /* Extended Address of the Device */
  zbIeeeAddr_t aExtendedAddress;

  /* Capability information of this device */
  macCapabilityInfo_t capabilityInformation;

  /* is this device started as a ZC, ZR or ZED? */
  zbDeviceType_t        deviceType;

  /* Logical channel on which the current device has associated */
  uint8_t logicalChannel;

  /* Short address of current device's parent */
  zbNwkAddr_t aParentShortAddress;

  /* Long address of current device's parent */
  zbIeeeAddr_t aParentLongAddress;

  uint8_t deviceDepth;

  uint8_t nwkUpdateId;

  uint8_t permitJoining;

  /*
    NIB Security Attributes
  */
#if gStandardSecurity_d || gHighSecurity_d
  /*
    053474r17 Table 4.1
    Name: nwkSecurityLevel
    Type: Octet
    Range: 0x00 - 0x07
    Default: 0x05
    NIB Id: 0xa0
  */
  zbNwkSecurityLevel_t  nwkSecurityLevel;

  /*
    053474r17 Table 4.1
    Name: nwkSecurityMaterialSet
    Type: A set of 0, 1 or 2 nwk security material descriptor (Table 4.2)
    Range: Variable
    Default: --
    NIB Id: 0xa1
  */
  zbNwkSecurityMaterialSet_t  aNwkSecurityMaterialSet[2];

  /*
    053474r17 Table 4.1
    Name: nwkAllFresh
    Type: Boolean
    Range: TRUE | FALSE
    Default: TRUE
    NIB Id: 0xa3
  */
  bool_t nwkAllFresh;

  /*
    053474r17 Table 4.1
    Name: nwkSecureAllFrames
    Type: Boolean
    Range: TRUE | FALSE
    Default: TRUE
    NIB Id: 0xa5
  */
  bool_t nwkSecureAllFrames;
#endif
}nvmNwkData_t;

#ifdef __IAR_SYSTEMS_ICC__
#pragma pack()
#endif
/****************************************************************************/

/* Task / kernel identifiers. */

/* Reserved task priority ranges.
 * Task priority   0 is reserved by the kernel for the idle task.
 * Task priority 255 is reserved by the kernel as an invalid priority value.
 *
 * Higher number tasks run first.
 */

/* Reserve some IDs for low priority BeeStack tasks. */
#define gTsFirstBeeStackTaskPriorityLow_c   0x01
#define gTsZTCTaskPriority_c                0x02
#define gTsUartTaskPriority_c               0x03
#define gTsI2CTaskPriority_c                0x04

#ifdef __IAR_SYSTEMS_ICC__ 
#define gTsLCDTaskPriority_c                0x05
#endif



/* Reserved for application tasks. */
#define gTsFirstApplicationTaskPriority_c   0x40

/* Application main task. */
#define gTsAppTaskPriority_c                0x80


/* Reserved for high priority BeeStack tasks. */
#define gTsFirstBeeStackTaskPriorityHigh    0xC0
#define gTsZclTaskPriority_c                gTsFirstBeeStackTaskPriorityHigh
#define gTsAfTaskPriority_c                 0xC1
#define gTsApsTaskPriority_c                0xC2
#define gTsZdoTaskPriority_c                0xC3
#define gTsZdoStateMachineTaskPriority_c    0xC4
#define gTsNwkTaskPriority_c                0xC5
#define gTsMlmeTaskPriority_c               0xC6
#define gTsHighestTaskPriorityPlusOne_c     0xC7

#if gTsTimerTaskPriority_c < gTsHighestTaskPriorityPlusOne_c
#error The Timer Task must be the highest priority task.
#endif

/****************************************************************************
*****************************************************************************
* Public memory declarations
*****************************************************************************
****************************************************************************/

extern bool_t gActAsZed;


/**********************************************************/

  /* User Descriptor */
	extern zbUserDescriptorPkg_t gUserDesc;
  /* Complex Descriptor */
	extern configComplexDescriptor_t gComplexDesc;

  
#if MC13226Included_d
  /* Number of Neighbour Table Entries */
  extern uint8_t gMaxNeighborTableEntries;
  /* Number of Maximum Route Table Entries */
  extern const uint8_t gMaxRouteTableEntries;
#else
  extern const uint8_t gMaxNeighborTableEntries;
  extern const uint8_t gMaxRouteTableEntries;
#endif


  extern const uint8_t gHandleTrackingTableSize;
  extern const uint8_t gPacketsOnHoldTableSize;
  extern secClearDeviceDataEvent_t gSecClearDeviceDataEvent;

/*
  The failure tolerance for the Nwk layer transmitions.
*/
extern const uint8_t gMaxNumberOfTxAttempts;

#if gStandardSecurity_d || gHighSecurity_d
/*
  The network security material, this are set of incomming counters, we need a set for
  each Nwk key of the system.
*/
extern zbIncomingFrameCounterSet_t  gaIncomingFrameCounterSet1[];
extern zbIncomingFrameCounterSet_t  gaIncomingFrameCounterSet2[];
#if MC13226Included_d
    extern uint8_t                gIncomingFrameCounterSetLimit;
    /* Set the tx options by default. */
    extern uint8_t gTxOptionsDefault;
#else
    extern const uint8_t                gIncomingFrameCounterSetLimit;
    /* Set the tx options by default. */
    extern const uint8_t gTxOptionsDefault;
#endif

	
#if MC13226Included_d
extern bool_t gTrustCenter;
#else /* MC13226Included_d */
#if gComboDeviceCapability_d
extern bool_t gTrustCenter;
#endif  /* gComboDeviceCapability_d  */
#endif  /* MC13226Included_d  */


#endif

#if (gStandardSecurity_d || gHighSecurity_d) && (gTrustCenter_d || gComboDeviceCapability_d) && gDefaultValueOfMaxEntriesForExclusionTable_c
/* security scan exclude nodes from the network */
extern const uint8_t gExclusionMax;
extern zbIeeeAddr_t gaExclusionTable[];
#endif

    /*
      053474r17 Table 4.36
      Name: apsTrustCenterAddress
      Type: Device Address
      Range: Any valid 64-bt address
      Default: --
      AIB Id: 0xab
    */
extern zbApsDeviceKeyPairSet_t gaApsDeviceKeyPairSet[];
#if MC13226Included_d
  extern uint8_t giApsDeviceKeyPairCount;
  extern zbKeyTableIndex_t gApsKeySetCount;
#else
  extern const uint8_t giApsDeviceKeyPairCount;
  extern const zbKeyTableIndex_t gApsKeySetCount;
#endif
extern zbAESKey_t  gaApsKeySet[];

#define gSSPNoKeyFound_c (zbKeyTableIndex_t)(~((zbKeyTableIndex_t)0))


/* keeps track of each rejoin reuqest until copleted. */
extern nwkRejoinEntry_t *gpNwkRejoinStateMachine;
/* The buffer used is as big as a small bufffer. */
extern const uint8_t gNwkRejoinStateMachineSize;
/* The size of the buffer divided by the amount of entries. */
extern const uint8_t gNwkRejoinStateMachineAmountOfEntries;
/* The amount of time in millisecodn to exppire the Nwk Rejoin state machine. */
extern const tmrTimeInMilliseconds_t  gNwkRejoinExpirationTime;


/* This table will exist only if a discovery network has been performed
previously and it last only during joining or forming a network */
extern neighborTable_t *gpDiscoveryTable;
extern extendedNeighborTable_t *gpExtendedDiscoveryTable;

extern index_t gMaxDiscoveryTableEntries;

/* This variable gives the size of the SSP Table which includes  */


#define gpBindingTable gaApsBindingTable

#define gpAddressMapTable gaApsAddressMap


	/* APS and NWK Information base externs. */
	extern nvmNwkData_t           gNwkData;  
  extern const nvmNwkData_t     gDefaultNwkData;
  extern beeStackConfigParams_t gBeeStackConfig;
	extern neighborTable_t        gaNeighborTable[];

#if gNwkSymLinkCapability_d
#if( gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d ) 
  extern uint8_t                gaNeighborTableAscendantSorted[];
  extern uint8_t                gNibNwkLinkStatusPeriod;
  extern const uint8_t          gNwkLinkStatusMultipleFramesPeriod; 
#endif
#endif

extern const uint8_t          gNibNwkRouterAgeLimit;

#if gNwkMulticastCapability_d
  extern bool_t                 gNibNwkUseMulticast;
  extern bool_t                 gNwkUseMulticastForApsGroupDataRequest;
  extern uint8_t                gMulticastMaxNonMemberRadius_c; 
#endif

	extern routeTable_t           gaRouteTable[];
  extern const uint8_t          gNwkRouteTableEntryMaxAge; 
  extern handleTrackingTable_t  gaHandleTrackingTable[];
  extern packetsOnHoldTable_t   gaPacketsOnHoldTable[];
	extern uint8_t                gNibSequenceNumber;
	
	extern const uint8_t          gNibReportConstantCost;
	extern uint8_t                gNibRouteDiscoveryRetriesPermitted;

	extern const uint8_t           gNibAddrAlloc;
  extern const uint8_t          gNwkEnergyLevelThresHold;
  extern zbCounter_t            gHttIndirectEntries;
	extern uint16_t               gNwkPurgeTimeout;
  extern const uint8_t          gDefaultRetriesStochasticAddrLocalDevice;
  extern const uint8_t          gHttMaxIndirectEntries;
  extern const uint8_t          gNwkHighRamConcentrator;

  extern const zbChannels_t gaFullChannelList;
  extern uint8_t gApsLastChannelEnergy;
  extern uint8_t gApsChannelFailureRate;
  extern uint8_t gApsChannelTimer;
  extern const uint8_t gNwkAgingTimeForEndDevice;
  extern const uint8_t gScanDuration;
  extern const uint8_t gNwkAgingTimeForRouter;
  extern const uint8_t gMinutesToExpireRoute; 
#if MC13226Included_d
  extern zbPanId_t gaPIDUpperLimit;
  extern uint8_t          gNibMaxBroadcastRetries;
  extern bool_t           gNibSymLink;
  extern zbCounter_t      gDefaultRadiusCounter;  
    /* specifies the # of retries APS uses */
  extern uint8_t gApsMaxRetries;
  /* specifies the wait duration on APS ACKs before retrying */
  extern uint16_t gApsAckWaitDuration;  
  extern bool_t gfFragmentationEnabled;  
  extern uint32_t gAps64BitAddressResolutionTimeout;
  extern uint16_t gApsFragmentationPollRate;
  extern uint8_t gMaxBroadcastTransactionTableEntries;
  extern uint8_t gBroadcastDeliveryNumTicks;
  extern uint8_t  gSkkeCommand;
  extern uint8_t gBroadcastJitterInterval_c;
  extern uint8_t gBroadcastDeliveryInterval_c;
  extern uint8_t gApsMaxClusterList;
  extern index_t giApsDuplicationRejectionTableSize;
  extern uint8_t gMaxApsTxTableEntries;
  
#else
  extern const zbPanId_t gaPIDUpperLimit;
  extern const uint8_t          gNibMaxBroadcastRetries;
  extern const bool_t           gNibSymLink;
  extern const zbCounter_t      gDefaultRadiusCounter;
  /* specifies the # of retries APS uses */
  extern const uint8_t gApsMaxRetries;  
  /* specifies the wait duration on APS ACKs before retrying */
  extern const uint16_t gApsAckWaitDuration;  
  extern const bool_t gfFragmentationEnabled;  
#endif

  extern uint8_t gNwkAgingTickScale;
  extern const uint8_t gNwkAgingTimeForSiblingRouter;
#if gComboDeviceCapability_d
  extern bool_t gLpmIncluded;
#endif  

#if gStandardSecurity_d || gHighSecurity_d
  /*
    allows non-secure APS data packets on a secure network. 
    If this option is true, the network is not ZigBee compliant.
    Has no effect if security is disabled.
  */
  extern const uint8_t gAllowNonSecurePacketsInSecureMode;
  
#endif
/***************************** For Inter-Pan *********************************/
extern const uint8_t gInterPanCommunicationEnabled;
typedef void (*InterPanFunc_t) ( void *msg );
typedef void (*InterPanFuncInd_t) ( mcpsToNwkMessage_t *pMsg );
typedef void (*InterPanFuncCnf_t) (uint8_t msduHandle, uint8_t status);
extern const InterPanFuncInd_t pInterPanProcessIndication;
extern const InterPanFuncCnf_t pInterPanProcessConfirm;
/***************************** Conflict Resolution **************************/

typedef void (*PIDConflictDetect_t) ( nwkBeaconNotifyInd_t *pBeaconIndication );
typedef void (*PIDConflictResol_t) ( void );
extern const PIDConflictDetect_t pPIDDetection;
extern const PIDConflictResol_t  pPIDResolution;

/**************** HighRamConcentrator function pointers *********************/
typedef sourceRouteTable_t*  (*NwkRetrieveSrcRoute_t) ( uint8_t *pDestinationAddress );
typedef void     (*NwkStoreSrcRoute_t) (uint8_t* pDestinationAddress, routeRecord_t*  pRouteRecord);
typedef void     (*NwkResetSrcRouteTable_t) ( void );
typedef index_t  (*NwkGetFreeEntryInSrcRouteTable_t) (void);
typedef void     (*NwkStopOrStartCtorDiscTime_t)(bool_t startOrStop); 

extern const NwkRetrieveSrcRoute_t          pNwkRetrieveSrcRoute;
extern const NwkStoreSrcRoute_t             pNwkStoreSrcRoute;
extern const NwkResetSrcRouteTable_t        pNwkResetSrcRouteTable;
extern const NwkGetFreeEntryInSrcRouteTable_t pNwkGetFreeEntryInSrcRouteTable;
extern const NwkStopOrStartCtorDiscTime_t     pNwkStopOrStartCtorDiscTime;
extern const uint8_t gMaxSourceRouteEntries;
extern sourceRouteTable_t gaSourceRouteTable[];
/****************************************************************************/
/* TaskIDs. The MAC library needs to know the IDs for some of the tasks, so
 * it can set events for them. But it is supplied as a binary library, so it
 * can't use #define or enum constants, and the TaskIDs might change. TaskIDs
 * are assigned dynamically by the kernel.
 * Instead, there is integer in RAM for each task that contains that task's
 * ID. This costs some RAM, but the MAC library only needs to know the names
 * of the integers, not the TaskIDs, at link time.
 */

#define Task( taskIdGlobal, taskInitFunc, taskMainFunc, priority ) \
  extern tsTaskID_t taskIdGlobal; \
  extern void taskInitFunc( void ); \
  extern void taskMainFunc( event_t );

#include "BeeStackTasksTbl.h"

/****************************************************************************
*****************************************************************************
* Fragmentation Capability Headers
*****************************************************************************
****************************************************************************/

/* state machine that handles fragmented Tx (transmit). May be disabled at compile-time. */
apsTxResult_t APS_ActualFragmentationTxStateMachine(apsTxIndex_t iTxIndex, apsTxState_t state);
apsTxResult_t APS_StubFragmentationTxStateMachine(apsTxIndex_t iTxIndex, apsTxState_t state);
bool_t APS_FragmentationTxStateMachine(apsTxIndex_t iTxIndex, apsTxState_t txState);
zbStatus_t Aps_StubSaveFragmentedData(void);
zbStatus_t Aps_ActualSaveFragmentedData(index_t iIndexRxTable);

/* state machine that handles fragmented Rx (receive).  May be disabled at compile-time  */
void APS_ActualFragmentationRxStateMachine(void);
void APS_StubFragmentationRxStateMachine(void);
void APS_FragmentationRxStateMachine(void);

/******************************************************************************
*******************************************************************************
* Permissions Configuration Table
*******************************************************************************
******************************************************************************/

#if gApsMaxEntriesForPermissionsTable_c

/* Type representing the permissions bit mask */
typedef	uint8_t permissionsFlags_t;

/*
  Set of bit masks representing the different permission categories.
  The gLinkKeyRequired_c mask is used to verify if a link key is required to carry out the commands.
*/
#define	gPermissionsEntryInActive                 0x00
#define	gNetworkSettingsPermission_c              0x02
#define	gApplicationSettingsPermission_c          0x04
#define	gSecuritySettingsPermission_c             0x08
#define	gApplicationCommandsPermission_c          0x10
#define	gLinkKeyRequired_c                        0x20
#define	gPermissionsEntryActive                   0x40

/* Permissions Configuration Table structure */
typedef	struct permissionsTable_tag {
  zbIeeeAddr_t authorizedDevice;
  permissionsFlags_t permissionsFlags;
}permissionsTable_t;

/* Permissions Configuration Table */
extern permissionsTable_t	gaPermissionsTable[];

extern uint8_t gPermissionsTableCount;

#endif  /* ((gStandardSecurity_d || gHighSecurity_d) && gApsMaxEntriesForPermissionsTable_c) */

#if MC13226Included_d
    extern uint8_t gNoOfEndPoints;
#else
    extern const uint8_t gNoOfEndPoints;
#endif

extern endPointPtrArray_t gaEndPointDesc[];
#ifdef gHostApp_d
  extern uint8_t        aExtendedAddress[];
  extern uint8_t giFragmentedPreAcked;
#endif /* gHostApp_d */

#endif /* _BEESTACK_GLOBALS_H_ */

