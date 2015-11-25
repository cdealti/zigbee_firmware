/******************************************************************************
* This is the source file which implements the channel PAN ID selection logic.
*
* Copyright (c) 2008, Freescale, Inc.  All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
*******************************************************************************/
#ifndef __IAR_SYSTEMS_ICC__
#ifdef MC13226Included_d
    #undef MC13226Included_d
#endif    
    #define MC13226Included_d 0
#endif
#include "EmbeddedTypes.h"
#include "Zigbee.h"
#include "Beestack_Globals.h"
#include "ZdoNwkInterface.h"
#include "FunctionLib.h"
#include "NV_Data.h"
#include "BeeStackUtil.h"
#include "BeeStackConfiguration.h"
#include "BeeStackInterface.h"
#include "NwkMacInterface.h"
#include "nwkcommon.h"
#include "BeeCommon.h"
#include "ZdoApsInterface.h"
#include "ZdoStateMachineHandler.h"
#include "ASL_UserInterface.h"
#include "ZdpManager.h"
#include "ASL_ZdpInterface.h"
#include "ZdoCommon.h"
#if (defined(gHostApp_d) || defined(gBlackBox_d))  
#include "ZtcInterface.h"
#endif
/******************************************************************************
*******************************************************************************
* Private Macros
*******************************************************************************
******************************************************************************/

zbIeeeAddr_t aTmpIeeeAddr;
zbNwkAddr_t  tmpNwkAddress;

/* Tells the criteria used to search suitable router to join */
#define mExtendedPanId_c        0x00
#define mNwkPanId_c             0x01
#define mAnyPanId_c             0x02

#define gRouterCapacity_c       0x04

#define gEndDeviceCapacity_c    0x80

#define gDeviceDepthMask_c      0x78

#define gZigbeeProStackProfile  0x02

#define gZigbeeStackProfile1    0x01

#define mNotFound               0xFF

#define IsStackProfileCompatible( pPayload ) (( gNwkProtocolId == pPayload->protocolId ) && \
                                              ((( pPayload->Info & 0x0F ) == NlmeGetRequest(gNwkStackProfile_c))|| \
                                               ((( pPayload->Info & 0x0F ) == gZigbeeProStackProfile || ( pPayload->Info & 0x0F ) == gZigbeeStackProfile1) &&  NlmeGetRequest(gNwkStackProfile_c))))

/*  
  0 = no security
  1 = no encryption, 32-bit MIC (no encryption, but authentication)
  2 = no encryption, 64-bit MIC  
  3 = no encryption, 128-bit MIC 
  4 = encryption, no MIC (encrypted but no authentication)
  5 = encryption, 32-bit MIC (default) 
  6 = encryption, 64-bit MIC
  7 = encryption, 128-bit MIC 
*/
#if ((mDefaultValueOfNwkSecurityLevel_c == 0) || (mDefaultValueOfNwkSecurityLevel_c == 4))
  #define mMICSize_c    0
#elif ((mDefaultValueOfNwkSecurityLevel_c == 1) || (mDefaultValueOfNwkSecurityLevel_c == 5))
  #define mMICSize_c    4
#elif ((mDefaultValueOfNwkSecurityLevel_c == 2) || (mDefaultValueOfNwkSecurityLevel_c == 6))
  #define mMICSize_c    8
#else
  #define mMICSize_c    16
#endif

/******************************************************************************
*******************************************************************************
* Private Prototypes
*******************************************************************************
******************************************************************************/
#if gFrequencyAgilityCapability_d && (gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d)
extern void  NWK_FRAMEHANDLE_SetBeaconPayload
(
  void
);
#endif

void ResetSourceRouteTable(void);

index_t GetFreeEntryInSourceRouteTable(void);

sourceRouteTable_t* NwkRetrieveSourceRoute(uint8_t * pDestinationAddress);

void NwkStoreSourceRoute(uint8_t * pDestinationAddress, routeRecord_t * pRouteRecord);

void FreeSourceRouteEntry(index_t iIndex );

void NwkStartOrStopNwkConcentratorDiscoveryTime( bool_t startOrStop );

/******************************************************************************
* This function is used to aging the route and source route table entries
******************************************************************************/
#if ( gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d ) && gRnplusCapability_d
/* This function start the aging of the route and source route tables*/
void StartRouteAndSourceRouteTable( void );

/* This function read both route and source route table entries and decrement the aging field value. */
void AgingRouteAndSourceTableEntries( void );

/* This is the custom call back timeout for the route and source route table aging timer*/
void CustomAgingRouteAndSourceTableTimeOutCallBack(tmrTimerID_t timerId );

/******************************************************************************
* This function is called by the network layer when the Routing table
* is full. Default behaviour is not to free a route entry.
* The example function finds an expired entry and passes the index
* to the network layer. A counter is used to avoid the same entry is not freed
* every time.
* Note the entry must be marked as gActive_c for it to able to get freed.
*
******************************************************************************/
index_t ExpireAndGetEntryInRouteTable(void);
#endif

#if gFrequencyAgilityCapability_d && (gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d)

void FA_ReportAnalysis
(
  zbEnergyValue_t  *pEnergyValues,
  zbChannels_t  ScannedChannels,
  uint8_t  scannedIndex
);

uint32_t ScanDurationToMilliseconds
(
  uint8_t scanDuration
);

void FA_SetPrivateChannelList(uint32_t *pChannelList);

#endif

/*
  Security exclusive functions.
*/
#if (gStandardSecurity_d || gHighSecurity_d) && (gTrustCenter_d || gComboDeviceCapability_d)
extern void ZDO_SendAuthenticationToTrustCenter
(
  bool_t allow
);

#else
#define ZDO_SendAuthenticationToTrustCenter(allow)
#endif

#if (gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d) && gRnplusCapability_d
extern void RemoveRouteEntry( uint8_t *pShortAddress );
void CleanUpPacketsOnHoldTable(zbNwkAddr_t aDestinationAddress);
#else
#define RemoveRouteEntry(pShortAddress)
#endif

/******************************************************************************
*******************************************************************************
* Private Type Definations
*******************************************************************************
******************************************************************************/
/*None*/
/******************************************************************************
*******************************************************************************
* Private Memory Decleration
*******************************************************************************
******************************************************************************/

/******************************************************************************
*******************************************************************************
* Global variables
*******************************************************************************
******************************************************************************/
/********************* HighRamNwkConcentrator **********************/
#if gConcentratorFlag_d

  const uint8_t gMaxSourceRouteEntries  = gNwkInfobaseMaxSourceRouteTableEntry_c;

  sourceRouteTable_t gaSourceRouteTable[gNwkInfobaseMaxSourceRouteTableEntry_c];
   
  const NwkResetSrcRouteTable_t          pNwkResetSrcRouteTable           = ResetSourceRouteTable;
  const NwkGetFreeEntryInSrcRouteTable_t pNwkGetFreeEntryInSrcRouteTable  = GetFreeEntryInSourceRouteTable;
  const NwkRetrieveSrcRoute_t            pNwkRetrieveSrcRoute             = NwkRetrieveSourceRoute;
  const NwkStoreSrcRoute_t               pNwkStoreSrcRoute                = NwkStoreSourceRoute;
  const NwkStopOrStartCtorDiscTime_t     pNwkStopOrStartCtorDiscTime      = NwkStartOrStopNwkConcentratorDiscoveryTime;
  const uint8_t gNibNwkMaxSourceRoute = gNwkMaxHopsInSourceRoute_c; 
  
#else
 
  const NwkRetrieveSrcRoute_t pNwkRetrieveSrcRoute    = NULL;
  const NwkStoreSrcRoute_t    pNwkStoreSrcRoute       = NULL;
  const NwkResetSrcRouteTable_t          pNwkResetSrcRouteTable           = NULL;
  const NwkGetFreeEntryInSrcRouteTable_t pNwkGetFreeEntryInSrcRouteTable  = NULL;
  const NwkStopOrStartCtorDiscTime_t     pNwkStopOrStartCtorDiscTime      = NULL;
  sourceRouteTable_t gaSourceRouteTable[1];
  const uint8_t gMaxSourceRouteEntries  = 1;
  const uint8_t gNibNwkMaxSourceRoute = gZero_c; 
#endif

/* Variables use for Frequency agility */
#if gFrequencyAgilityCapability_d && (gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d)
uint8_t  gFA_DeviceState = gRunning_c;

/*
  This variable tells the current state of the nwkmanager state machine.
*/
uint8_t gChannelMasterState;

/*
  The Id of the task that will handle the Channel Master State machine.
*/
tsTaskID_t gFATaskId;

/*
  The amount of incoming report errors to keep track
*/
uint8_t mErrorReportsReceived;

/*
  The amount of Scan reports received during the run of the state machine.
*/
uint8_t mNumScansReceived;

/*
  The amount of node doing Scans, this are the routers request to do the scan.
*/
uint8_t mNumScansSent;

/*
  The timer to be use during the error report processing.
*/
tmrTimerID_t  mErrorReportTimer;

/*
  FA Generic millisecond timer.
*/
tmrTimerID_t  mGenericFATimer;

/*
  Update notify report timer (minute timer).
*/
tmrTimerID_t  gFA_NotifyTimer;

/*
  Keeps track of the scannign reason in the state machone.
*/
uint8_t  mScanReason;

/*
  Initialize all channels with the middle energy value.
*/
uint8_t mChannels[] = {0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F};

/*
  ScanCount is a counter to keep track of the number of scan requests and reports that
  needs to be done.
*/
uint8_t gScanCount = 0;

/*
  pointer to the ScanChannelList need it for the energy scan.
*/
zbChannels_t gaScanChannelList;

/*
  For the devices that need to send reports back to the Nwk Mngr, we need to
  keep around the scanduration.
*/
uint8_t  mFAScanDuration;
#endif


/*
  This variable is set to true when a device of the network detects a PanId conflict 
  after it joined the network.
*/
#if MC13226Included_d
    extern bool_t gNwkPanIdConflictDetected;
#else
    bool_t gNwkPanIdConflictDetected;
#endif

#if gFrequencyAgilityCapability_d
/*
  In the case of sending the Update request to the nwk layer keep the buffer around.
*/
extern bool_t mbIsNotFree;
#endif

/* Tells what kind of joining the device is attempting to do */
extern uint8_t gZDOJoinMode;
extern bool_t gIsABroadcast;


/******************************************************************************
*******************************************************************************
* Public Functions
*******************************************************************************
******************************************************************************/



#if gStandardSecurity_d || gHighSecurity_d
extern zbKeyType_t ZDO_SecGetDeviceKeyType
(
  zbIeeeAddr_t aDeviceAddress
);
#endif  /* gStandardSecurity_d || gHighSecurity_d */

extern uint8_t * APS_GetNwkAddress
(
  uint8_t * pNwkAddr, 
  uint8_t   aWereToCpyNwkAddr[]
);

extern uint8_t * APS_GetIeeeAddress
(
  uint8_t * pNwkAddr,
  uint8_t   aWereToCpyExtAddr[]
);

/*
  GetMaxApplicationPayload

  Returns the maximum application payload (ASDU) given the data request.  
*/
uint8_t GetMaxApplicationPayload
  (
  zbApsdeDataReq_t *pDataReq /* IN: potential data request */
  )
{
  /* Maximum Packet size */
  #define mPhyLen_c         127
  
  /* Default header sizes */
  #define mMacHdrSize_c     11 /* |FC(2) | seqNo(1) | dstPan(2) | dstAddr(2) | srcAddr(2) | fcs(2) */
  #define mNwkHdrSize_c     8  /* |FC(2) | dstAddr(2) | srcAddr(2) | radius(1) | seqNo(1) */
  #define mApsHdrSize_c     8  /* |FC(1) | dstEp(1) | clusId(2) | profId(2) | srcEp(1) | counter(1) */

  /* Optional extra overheads */
  #define mNwkSrcRouteSize_c 18 /* Six directions of overhead in source route as default */  
  #define mNwkAuxSize_c     (14+mMICSize_c) /* |SC(1) | frameCtr(4) | srcAddr(8) | keySeq(1) | MIC(4) */
  #define mApsAuxSize_c     (5+mMICSize_c) /* |SC(1) | frameCtr(4) | MIC(4) */
  #define mApsFragSize_c    2  /* Overhead for fragmentation */
  #define mApsGroupSize_c   1  /* Overhead for groups */

  uint8_t maxPayloadLen;
  uint8_t * aNwkAddr=NULL;
  uint8_t * aIeeeAddr=NULL;
  
#ifndef gHostApp_d  
  uint8_t aNwkAddrLocalCpy[2], aExtAddr[8];
#endif  

#if ((gStandardSecurity_d || gHighSecurity_d) && gApsLinkKeySecurity_d)
  zbKeyType_t pKeyType=0xFF;
  (void)pKeyType;
#endif  /* ((gStandardSecurity_d || gHighSecurity_d) && gApsLinkKeySecurity_d) */

  (void)aNwkAddr;
  (void)aIeeeAddr;
  

  /* assume 100 bytes payload max */
  maxPayloadLen = mPhyLen_c - (mMacHdrSize_c + mNwkHdrSize_c + mApsHdrSize_c);
 #ifndef gHostApp_d
  /* reduce for various options (if specified) */
  if(pDataReq)
  {
    /* both group and indirect (which may have groups) are smaller */
    if(pDataReq->dstAddrMode <= gZbAddrModeGroup_c)
      maxPayloadLen -= mApsGroupSize_c;   /* group overhead is 1 */
    else 
    {      
      if (pDataReq->dstAddrMode == gZbAddrMode16Bit_c) 
      {        
        aNwkAddr = pDataReq->dstAddr.aNwkAddr;
        aIeeeAddr = APS_GetIeeeAddress(aNwkAddr, aExtAddr);
      }
      else 
      {        
        aIeeeAddr = pDataReq->dstAddr.aIeeeAddr;
        aNwkAddr = APS_GetNwkAddress(aIeeeAddr, aNwkAddrLocalCpy);
      }
    }
                
    if ((pDataReq->txOptions & gApsTxOptionFragmentationRequested_c) && (pDataReq->dstAddrMode > gZbAddrModeGroup_c))
      maxPayloadLen -= mApsFragSize_c;   /* up to 2 bytes for fragmentation header */

  /* If we are in ZigBee Pro and RnPlus is activated */
#if (gZigbeeProIncluded_d && gRnplusCapability_d)
  /* If is a unicast we need to verify if there is a source route */
    if (pDataReq->dstAddrMode > gZbAddrModeGroup_c) 
    {      
  /* A source route is only present if we are a High Ram Concentrator */
#if (gConcentratorFlag_d)
      /* Verify if a source route is going to be needed */
      if (aNwkAddr)
        maxPayloadLen -= NwkGetSourceRouteSubframeSize(aNwkAddr);    
      else
        maxPayloadLen -= mNwkSrcRouteSize_c; /* If we didn't find the Nwk Address, consider the worst case */
#endif  /* (gConcentratorFlag_d) */
    }
#endif  /* (gZigbeeProIncluded_d && gRnplusCapability_d) */

#if gStandardSecurity_d || gHighSecurity_d
    /* if security is enabled for this packet, subtract the NWK AUX size */
    
    
    if(!(pDataReq->txOptions & gApsTxOptionNoSecurity_c))
    {
		if (NlmeGetRequest(gNwkSecureAllFrames_c))
		{
		  maxPayloadLen -= mNwkAuxSize_c;
		}
    	if((pDataReq->txOptions & gApsTxOptionSecEnabled_c))
    	{
#if gApsLinkKeySecurity_d
		  /*  If we are using the active network key, and the nwkSecureAllFrames attribute
			  of the NIB is TRUE, the APS layer will not apply security */
		  if (!((pDataReq->txOptions & gApsTxOptionUseNwkKey_c) && (NlmeGetRequest(gNwkSecureAllFrames_c))))
		  {
			/* We need the Ieee address to verify if there is a linkkey with the destination device */
			if (aIeeeAddr)
			{
			  pKeyType = ZDO_SecGetDeviceKeyType(aIeeeAddr);
			  if ((pKeyType == gApplicationLinkKey_c) || (pKeyType == gTrustCenterLinkKey_c))
				maxPayloadLen -= mApsAuxSize_c;
			}
		  }
#endif  /* gApsLinkKeySecurity_d */
		}
    }
#endif  /* gStandardSecurity_d || gHighSecurity_d */
  }
  else
#else
(void)pDataReq;  
#endif  /*end gHostApp_d*/
  {
  /* Unknown what the data request is, so choose a "safe" size */
#if gFragmentationCapability_d
    maxPayloadLen -= mApsFragSize_c;   /* worst case is fragmentation */
#endif

#if (gZigbeeProIncluded_d)
#if gRouterCapability_d || gCoordinatorCapability_d || gComboDeviceCapability_d
#if gComboDeviceCapability_d
    if (NlmeGetRequest(gDevType_c) != gEndDevice_c)
#endif
    {
      maxPayloadLen -= mNwkSrcRouteSize_c;
    }
#endif
#endif  /* (gZigbeeProIncluded_d) */
    
#if gStandardSecurity_d || gHighSecurity_d
    /* normal case for stack profile 0x01, residential security */
    maxPayloadLen -= mNwkAuxSize_c;
#if gApsLinkKeySecurity_d
    maxPayloadLen -= mApsAuxSize_c;
#endif  /* gApsLinkKeySecurity_d */
#endif  /* gStandardSecurity_d || gHighSecurity_d */
  }

  /* return maximum size of payload */
  return maxPayloadLen;
}

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
)
{
  uint8_t i;

  uint8_t aNetworkCount[gNumOfChannels_c];  /* On per each channel. */

  /* Initialize at the highest value to track the channel with less amount of networks. */
  uint8_t iMinNetworkCount = 0xFF;

  /*
    This will have values 3,2,1,0 as we traverse through for each Channel,
    8 Channels are bit mapped in 1 Byte. This helps to walk throught the
    4 byte channels list.
  */
  uint8_t iIndex = 0x03;
  uint8_t iChannelIndex; 

  /*
    This is used to Right Shift and mask as we traverse each Channel,
    0x04 is for Channel 26
  */
  uint8_t bitMask = 0x04;

  uint8_t iListIndex = (gNumOfChannels_c -1);

  /* Leave on Zero the unused channels. */
  BeeUtilZeroMemory(aNetworkCount, sizeof(aNetworkCount));

  /*
    Fills each channel position with the amount of networks existing on that particular
    channel.
  */
  for (i = 0; i < gMaxDiscoveryTableEntries; i++)
  {
    if (IsBroadcastAddress(gpDiscoveryTable[i].aNetworkAddress))
    {
      continue;
    }

    /*
      Transform the channel number (11 to 26) into an array index (0 to 15).
      And keep track of the amount of networks per channel.
    */
    aNetworkCount[gpExtendedDiscoveryTable[i].logicalChannel - 11]++;
  }

  for (i = 0; i < gNumOfChannels_c; i++)
  {
    /*
      pChannelList represents the Channels Specified by Upper layer (e.g Formation Request)
      Check if channel was scanned.
    */
    if ( pChannelList[ iIndex ] & bitMask )
    {
      if (aNetworkCount[iListIndex - i] <= iMinNetworkCount)
      {
        iMinNetworkCount = aNetworkCount[iListIndex - i];
        iChannelIndex = (iListIndex - i);
      }
    }
    /* Shift the mask one bit to the right in order to process the next channel. */
    bitMask >>= 1;

    /* Check if 1 Byte (set of 8 channels ) is over */
    if( 0x00 == bitMask )
    {
      /* This is used to Right Shift and mask as we traverse each Channel,
      Reinitialise it to 0x80 for next channel  */
      bitMask = 0x80;
      iIndex--;
    }
  }

  /* Return the value of the selected channel (11 .. 26). */
  return (iChannelIndex + 11);
}

/******************************************************************************
* Only included if ZigBee Coordinator (ZC)
* Select logical channel to form network
******************************************************************************/
void SelectLogicalChannel
(
  const nwkMessage_t *pMsg, /* IN - Pointer to the ED Scan Confirm */
  uint8_t* pScanChannels,/* IN - List of Channels obtained after ED Scan */ 
  uint8_t* pSelectedLogicalChannel/* IN/OUT- To be updated after finding
                                             least number of Nwks */     
)
{
  uint8_t panDescListSize = pMsg->msgData.scanCnf.resultListSize;
  uint8_t cCounter = 0x00;   /* Counter used for Num. of Pan Descriptors */
  uint8_t iChannelIndex  = 0x00; /* Index used for Network Count */
  uint8_t logicalChannel = 0x00; 	 /* Denotes Logical Channel */
  uint8_t minNetworkCount = 0xFF;	 /* Used to store the number of Nwks */
  uint8_t networkCount[ 16 ];  /* Denotes Num of Networks in each Channel */
  uint8_t iIndex = 0x00;	 /* Used to Scan through Channels */  
  uint8_t bitMask = 0x08; /* This is used to Left Shift and mask as we traverse 
                           each Channel ,0x08 is for Channel 11 */
  panDescriptorBlock_t *block;
  
  /* No need to check for Memory fail because it is done by the calling layer */
  panDescriptor_t *pPanDescList;
  block = pMsg->msgData.scanCnf.resList.pPanDescriptorBlocks;   
  pPanDescList = block->descriptorList;

  BeeUtilZeroMemory(networkCount, gNumOfChannels_c);/* Initialise to Zero */
  
  /* To find the Num of Networks specified in the Selected Channels */
   /*  Process the Active scan confirm. The NLME shall review
       the list of returned PAN descriptors and find the first
       channel with the lowest number of existing networks, 
       favoring a channel with no detected networks */ 
  for ( cCounter = 0x00; cCounter<panDescListSize; cCounter++) 
  {
    if (cCounter >= aScanResultsPerBlock)
    {        
      pPanDescList = block->pNext->descriptorList;     
      logicalChannel = pPanDescList[ cCounter - aScanResultsPerBlock ].logicalChannel;
    }
    else      
    {        
      logicalChannel = pPanDescList[ cCounter ].logicalChannel;
    }
    networkCount[ logicalChannel - gLogicalChannel11_c ]++;          
  }
  /* Shifting the bitMask bit towards left to Select Each of the Channels
  	 and update the Selected Logical Channel */
  /* 26 channels spread over 4 Bytes as Bit maps, Valid Channel Starts 
     from 11 to 26 
     Hence the iIndex will vary from 1 to 4 */ 
  for( iIndex = 1; iIndex < 4; iIndex++ )
  {
  	while( bitMask > 0x00 ) /* Shifting will take it to 0 after few iterations */
  	{
  		if( pScanChannels[ iIndex ] & ( bitMask )) {  	
  			if ( networkCount[ iChannelIndex ] < minNetworkCount ) 
  			{
  			  /* Make the Min Nwk Count as the one with Least Nwk Found */
  				minNetworkCount = networkCount[ iChannelIndex ];
  				*pSelectedLogicalChannel = iChannelIndex  + gLogicalChannel11_c;
  				if ( 0x00 == minNetworkCount ) {
  				  break;
  				}
  		  }/* If loop Ends */			
  		} /* 	if( pScanChannels[ iIndex ] & ( bitMask )) ENDS */
  		iChannelIndex ++;	 /* This is done in order to get the correct Channel
  		Offset */
  		bitMask <<= 1;
  	}
  	/* Check if 1 Byte (set of 8 channels ) is over */
    if ( 0x00 == bitMask ) {
      bitMask = 0x01; 		 /* Reinitialise for the Next Set of Channels */
    }
  }
}

/****************************************************************************/
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
)
{
  aPanId[0] = GetRandomRange(0x00, gaPIDUpperLimit[1]);
  aPanId[1] = GetRandomRange(0x00, gaPIDUpperLimit[0]);
}

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
)
{
  bool_t  useRandom;

  bool_t  panIdConflict = FALSE;

  uint8_t i;

  /* Is a the given Pan Id a valid one? or we need to generate one? */
  useRandom = (IsEqual2BytesInt(aPanId, 0xFFFF))? TRUE : FALSE;

  if (useRandom)
  {
    getRandomPanId(aPanId);
  }

  while (!panIdConflict)
  {
    /*  */
    for (i = 0; i < gMaxDiscoveryTableEntries; i++)
    {
      /* Skip the invalid entries. */
      if (IsBroadcastAddress(gpDiscoveryTable[i].aNetworkAddress))
      {
        continue;
      }

      if (IsEqual2Bytes(aPanId, gpExtendedDiscoveryTable[i].aPanId) &&
          channel == gpExtendedDiscoveryTable[i].logicalChannel)
      {
        panIdConflict = TRUE;
        break; /* Break only the for loop. */
      }
    }  /* End of For loop */

    /* Check if the PanId conflict was found and if the Pan Id is random. */
    if (panIdConflict && useRandom)
    {
      /*
        Ok, we got a random panId with conflict well..lets just pick another one a
        keep moving.
      */
      panIdConflict = FALSE;
      getRandomPanId(aPanId);
    }
    else
    {
      /* Ok, no conflicts or conflicts but no ra */
      break;
    }
  }  /* End of While loop */

  return panIdConflict;
}

/****************************************************************************
* Only on a ZigBee Coordinator (ZC)
* Select a random PAN ID if one not given by user.
*
* Returns TRUE if there is a PAN ID conflict.
*****************************************************************************/
bool_t SelectPanId
(
  const nwkMessage_t *pMsg,       /* IN -Pointer to Active Scan Confirm Paramters */
  uint8_t selectedLogicalChannel, /* IN - Log.Channel for determining PanId */
  uint8_t* pPanId								  /* IN/OUT - Pointer to the PanId */
)
{
  /* Get the Pan Descriptor List Size */
  uint8_t panDescListSize   = pMsg->msgData.scanCnf.resultListSize;
  uint8_t iIndex;
  uint8_t panIdConflictDetected;
  extern bool_t gUseRandomPanId;

  /* Point to the Pan Descriptors obtained */
  panDescriptorBlock_t *block;
  
  /* No need to check for Memory fail because it is done by the calling layer */
  panDescriptor_t *pPanDesc;
  block = pMsg->msgData.scanCnf.resList.pPanDescriptorBlocks;     


  /* if there is another network in this channel with our extendedpanid there is a conflict */
  if(CheckForExtendedPanIdConflict(selectedLogicalChannel))
    return TRUE;

  /* select random pan ID */
  if(gUseRandomPanId) {
    getRandomPanId(pPanId);
  }

  /* PAN ID given by user */
  else {
    Copy2Bytes(pPanId,NlmeGetRequest(gNwkPanId_c));
  }

  /* Search for a PanId that does not conflict with the networks 
     existing in the 'selectedLogicalChannel' */  
  do {

    panIdConflictDetected = 0; 

    /* Scan through the Pan Desc. Found from the scan */
    for(iIndex = 0; iIndex<panDescListSize; iIndex++)
    {
    if (iIndex >= aScanResultsPerBlock)
    {        
      pPanDesc = &block->pNext->descriptorList[ iIndex - aScanResultsPerBlock ];     
    }
    else      
    {        
      pPanDesc = &block->descriptorList[ iIndex ];     
    }

      /* Check if the Channels are same */
      if( selectedLogicalChannel == pPanDesc->logicalChannel ) { 

        /* Check if the Pan ID is same */     
        if( IsEqual2Bytes(pPanId, pPanDesc->coordPanId )) {
          ++panIdConflictDetected;
          break;
        }
      }
    } /* for loop Ends */

    /* a PAN ID was provided and there is a conflict */
    if(!gUseRandomPanId) {
#if gSamePanIdOk_c
      panIdConflictDetected = FALSE;
#endif
      return panIdConflictDetected;
    }

    /* pick another PAN ID */
    getRandomPanId(pPanId);

  } while (panIdConflictDetected);
  
  return panIdConflictDetected;
}

/**********************************************************************************
*
* Cheks if in a particular channel exists another network using the same Extended
* Pan Id we are tryig to use to form our network.
* Returns True if there is a confilct and False if there is none
*
**********************************************************************************/
bool_t CheckForExtendedPanIdConflict
(
  uint8_t logicalChannel /* IN - Log.Channel for determining PanId */
)
{
  /* Index to traverse through the neighbor table*/
  uint8_t iIndex = 0;

	for(iIndex = 0; iIndex < gMaxDiscoveryTableEntries; iIndex++) {

      if(IsEqual8Bytes( NlmeGetRequest(gNwkExtendedPanId_c),
                        gpExtendedDiscoveryTable[iIndex].aExtendedPanId ) &&
                        logicalChannel == gpExtendedDiscoveryTable[iIndex].logicalChannel )
        return TRUE;
  }
  return FALSE;
}

/*
  Calculates the Link cost based on the following Table
  ===================
  | LQI    |LinkCost|
  ===================
  |>75     |    1   |
  -------------------
  |50-75   |    3   |
  -------------------
  |<50     |    7   |
  -------------------                                                    

  053474r17 Zigbee Spec
  Link cost calculation explanation 3.6.3.1 Routing Cost
*/
zbLinkCost_t LinkCostCalculator(uint8_t iLqi)
{
  if(iLqi > 75)
  {
    return 1;
  }
#ifdef __IAR_SYSTEMS_ICC__
  else if(iLqi > 30)
#else
  else if(iLqi > 50)
#endif
  {
    return 3;
  }
  /*
    The change is need to report onthe link status an expired parent, when a parent gets
    expired, we set the LQI to zero and the outgoing cost to zero, so inorder to report a
    link cost of zero we must set it here.

    See 075035r5, test 13.5 and test 13.4.
  */
  if (!iLqi)
  {
    return 0;
  }

  return 7;
}


#if gRouterCapability_d || gEndDevCapability_d || gComboDeviceCapability_d
/**********************************************************************************
* Valid for ZR, ZED or Combo devices
*
* Look for a router (ZC or ZR) to join. Return the index in the discover table for
* the potential parent. Return 0XFF if there were no match found. The criteria used 
* is as follows:
*
* 1. First check the extended pan id in the nib, if it's different than zero the 
*    algoritm will try to find a suitable router based on that extended pan id.
* 2. If extended pan id is equal to zero check the nwk pan id in the nib, if it's
*    diffferent than 0xFFFF the algoritm will try to find a suitable router based
*    on the nwk pan id.
* 3. If nwk pan id is equal to 0xFFFF (actually, anything 0x4000-0xffff) then the 
*    algoritm will try to find a suitable router withot paying attention to either 
*    extended or nwk pan id.
*
* Note: also takes into a count the permit joining flag, link cost, maxDepth and 
* router or end device capacity.
*
**********************************************************************************/
index_t SearchForSuitableParentToJoin ( void )
{
  /* Index to traverse through the neighbor table*/
  uint8_t iIndex = 0;

	/* To check whether the link cost in valid range */
  uint8_t linkCost = 0;

  /* The MaxDepth that can be possible is 0xff*/
  uint8_t minDepth=0xff;
	
  /* Index in neighbor table once the sutible coordinator is selected */
  uint8_t minimumDepthIndex =0;
	
  /* by default, search will be based on Extended Pan Id*/
  uint8_t lookFor;
	
  /* flag used to indicated wheter or not a particular coord is suitable*/
  bool_t panIdMatches=FALSE;

  /* value of the Highest NwkUpdateId to join to */
  uint8_t highestNwkUpdateId = NlmeGetRequest(gNwkUpdateId_c);

  /* flag used to indicate whether or not a coord has been found*/
  bool_t coordinatorFound = FALSE;

  /*Ptr to a Discovery Table entry containing Vendor specific ,Mandatory and
  Optional Attributes*/
  neighborTable_t *pDTE;

  /* Pointer to an Extended Discovery Table Entry */
  extendedNeighborTable_t *pEDTE;

  /* The right extended PAN ID to use. */
  uint8_t *pExtendedPanId = NULL;

  /*
    If the device is a combo device acting like coordinator should not proceed
    processing further.
  */
#if gComboDeviceCapability_d
  if ((NlmeGetRequest(gDevType_c) == gCoordinator_c))
  {
    return gNwkEntryNotFound_c;
  }
#endif


  /* if discovery tables are not even allocated return gNwkEntryNotFound_c(0xFF) */
  if(!gpDiscoveryTable)
    return gNwkEntryNotFound_c;

  /* By default use Network Extended PAN Id. */
  lookFor = mExtendedPanId_c;

  /* Se if the stack has a valid Nwk Extended PAN Id. If so, use it to match the desired Network. */
  if (!Cmp8BytesToZero(NlmeGetRequest(gNwkExtendedPanId_c)))
  {
    pExtendedPanId = NlmeGetRequest(gNwkExtendedPanId_c);
  }
  else if (!Cmp8BytesToZero(ApsmeGetRequest(gApsUseExtendedPANID_c)))
  {
    /*
      Ok, We don't have a Nwk Extended Pan Id, we must then have an APS extended Pan Id,
      lets try to use it (we may be doing rejoin, See section 2.5.5.5.6.2 053474r17.).
    */
    pExtendedPanId = ApsmeGetRequest(gApsUseExtendedPANID_c);
  }
  /* if it's not a valid extended Pan Id, check if the nwk Pan Id is a valid one */
  else if (TwoBytesToUint16(NlmeGetRequest(gNwkPanId_c)) == 0xffff)
  {
    /* if the nwk Pan Id is also invalid, search will be based on any pan id. */
    lookFor = mAnyPanId_c;
  }
  else
  {
    /* If nwk Pan Id is a valid one, search will be base on it */
    lookFor = mNwkPanId_c;
  }

  /* Concider all of the cases found.! */
  for (iIndex = 0; iIndex < gMaxDiscoveryTableEntries; iIndex++)
  {
    panIdMatches=FALSE; // Reset PanId match flag
    pDTE = &( gpDiscoveryTable [ iIndex ] );
    pEDTE = &( gpExtendedDiscoveryTable [ iIndex ] );

    linkCost = LinkCostCalculator ( pDTE->lqi );

    if (lookFor == mExtendedPanId_c)
    {
      /* Compare the extended PAN Id's fond to the selected one (Nwk or APS). */
      if( IsEqual8Bytes( pEDTE->aExtendedPanId, pExtendedPanId))
        panIdMatches = TRUE;
    }
    else if (lookFor == mNwkPanId_c)
    {
      /* We don't have extended Pan Id, lets try the configured Short Pan Id. */
      if(IsEqual2Bytes(pEDTE->aPanId,NlmeGetRequest(gNwkPanId_c)))
        panIdMatches = TRUE;
    }
    else if (lookFor == mAnyPanId_c)
    {
      /* We dont have an Extended Pan Id or a short Pan Id, lets try Any short Pan Id. */
      panIdMatches =TRUE;
    }

    if ((panIdMatches) && ( NWK_DeviceIsPotentialParent(pEDTE->joiningFeatures) ) && ( linkCost <= maxLinkCost ))
    {
      if( NWK_DevAllowsAssociation( pEDTE->joiningFeatures) || gZDOJoinMode == gNwkRejoin_c || gZDOJoinMode == gSilentNwkRejoin_c )
      {
        if ((pEDTE->nwkUpdateId >= highestNwkUpdateId) || ((highestNwkUpdateId == 0xFF)&&(pEDTE->nwkUpdateId < 0x05)))
        {
          highestNwkUpdateId = pEDTE->nwkUpdateId;

          /* At this point the link costis below 3 which makes it acceptable, lets see ti it wasnt our parent before. */
          if (IsEqual2Bytes(pDTE->aNetworkAddress, NlmeGetRequest(gParentShortAddress_c)))
          {
            /* Ok, it was our parent before and is between the acceptable limits, use this guy. */
            return iIndex;
          }

          /* to choose a potential parent at the minimum depth. */
          if( pEDTE->depth < minDepth )
          {
            minimumDepthIndex = iIndex;
            minDepth = pEDTE->depth;
            coordinatorFound = TRUE;
            panIdMatches = FALSE;  /* reset flag */
          }
        }
      }
    }
  } /* for (iIndex = 0;... */

  if( coordinatorFound )
  {
    return minimumDepthIndex;
  }

  /* Giveup, theres no one out there.! */
  return gNwkEntryNotFound_c;
}
#endif


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
)
{
  index_t iIndex=0;

  /* Pointer to the DiscoveryTable entry containing Vendor specific ,Mandatory and
  Optional Attributes*/
  neighborTable_t *pDTE;

  /* Pointer to the Extended Discovery Table Entry containing information needed
  to form or join a network */
  extendedNeighborTable_t *pEDTE;

  /* look for an available entry for the beacon notify sender */
  for(iIndex=0; iIndex < gMaxDiscoveryTableEntries; iIndex++) {

 	  pDTE = &( gpDiscoveryTable [ iIndex ] );
    pEDTE = &( gpExtendedDiscoveryTable [ iIndex ] );

    if(reuse)
      if( !NWK_DeviceIsPotentialParent(pEDTE->joiningFeatures) ||
          !NWK_DevAllowsAssociation( pEDTE->joiningFeatures) )
        return iIndex;

    /* if an empty entry has been found return index. */
		if( IsEqual2BytesInt( pDTE->aNetworkAddress, 0xFFFF ) ||
			/* or if we got already a becacon notify from the sender reuse the entry */
			( IsEqual2Bytes( pEDTE->aPanId, aPanId) &&
				IsEqual2Bytes(pDTE->aNetworkAddress, aNwkAddr ) &&
				pEDTE->logicalChannel == channel &&
				((pEDTE->joiningFeatures & 0xF0)>>4) == stackProfile ) ) {
      return iIndex;
    }
  }
  return mNotFound;
}

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
******************************************************************************/
void ParseBeaconNotifyIndication
  (
  nwkBeaconNotifyInd_t *pBeaconIndication  /*IN: Ptr to BeaconIndication sent by Mac */
  )
{
  index_t iIndex=0;

  /* Pointer to the DiscoveryTable entry containing Vendor specific ,Mandatory and
  Optional Attributes*/
  neighborTable_t *pDTE;

  /* Pointer to the Extended Discovery Table Entry containing information needed
  to form or join a network */
  extendedNeighborTable_t *pEDTE;

  /* Pointer to the pan descriptor contained in the beacon indication */
  panDescriptor_t *pPanDescriptor = pBeaconIndication->pPanDescriptor;

  /* Pointer to the beacon payload contained in the beacon indication */
  beaconPayloadData_t *pPayload = ( beaconPayloadData_t * )pBeaconIndication->pSdu;

 /* if packet is null */
  if(!pPayload || !pPanDescriptor)
    return;

  /* Check if beacon indication has a compatible protocol id and stack profile, if not ignore the beacon */
  if( !IsStackProfileCompatible( pPayload ) )
    return;

  /* look for an empty slot */
  iIndex = GetFreeEntryInDiscoveryTable(pPanDescriptor->coordAddress,
                                        pPanDescriptor->coordPanId,
                                        pPanDescriptor->logicalChannel,
                                        pPayload->Info & 0x0F, //stack profile
                                        FALSE);

  /* if there is no empty slots then looks for an entry with less priority that can be replace */
  if(iIndex ==  mNotFound) 
  {
    iIndex = GetFreeEntryInDiscoveryTable(pPanDescriptor->coordAddress,
                                          pPanDescriptor->coordPanId,
                                          pPanDescriptor->logicalChannel,
                                          pPayload->Info & 0x0F, //stack profile
                                          TRUE);
  }

  /* if there is no room for another entry then discard the beacon */
  if(iIndex ==  mNotFound)
    return;
  
	/* if an empty entry has been found add new data to that entry, else ignore the beacon.
	to check if an entry is empty check extended address, this never will be zero if entry is being used */

	pDTE = &( gpDiscoveryTable [ iIndex ] );
	pEDTE = &( gpExtendedDiscoveryTable [ iIndex ] );


  /* initialize transmit failures counter to zero */
  pDTE->transmitFailure=0;

  /* initialize device property field*/
  pDTE->deviceProperty.allBits = 0;

	/*update the Discovery Table Entry with the Nwk Address of the Router (aka coord in MAC speak) */
	Copy2Bytes(pDTE->aNetworkAddress, pPanDescriptor->coordAddress);

	/*Update the Extended Discovery Table Entry with the PanId of the Coordinator */
	Copy2Bytes(pEDTE->aPanId,pPanDescriptor->coordPanId);

	/*Update the Extended Discovery Table Entry with the Extended PanId contained in payload */
	Copy8Bytes(pEDTE->aExtendedPanId, pPayload->aNwkExtPANId) ;

	/*Update the Extended Discovery Table Entry with the logical channel*/
  pEDTE->logicalChannel = pPanDescriptor->logicalChannel;

#if ( gBeaconSupportCapability_d )
  /* Update the txOffset of the potential parent */
  FLib_MemCpy( pDTE->aBeaconTransmissionTimeOffset, pPayload->aTxOffset, 3 );

  /* Update the SuperFrame Specifications */
  pEDTE->beaconOrder = ( pPanDescriptor->superFrameSpec[ 0 ] ) & gSuperFrameSpecLsbBO_c;

#endif ( gBeaconSupportCapability_d )

  /* Beacons can be received only from a Zigbee Coordinator or from a Zigbee Router
  and hence based on the Pan Coordinator Bit set the deviceType to Coordinator or Router */
  pDTE->deviceProperty.bits.deviceType = ( ( ( pPanDescriptor->superFrameSpec[ 1 ] ) & gSuperFrameSpecMsbPanCoord_c)? gCoordinator_c : gRouter_c );

  /* Update Neighbor Table entry with the LinkQuality obtained from Mac */
  pDTE->lqi = pPanDescriptor->linkQuality;

  /* Assumption: There is no way to receive the rxonwhenidle while performing
  discovery and  the assumption is that any device sending the beacon will
  have its receiver turned on */
  pDTE->deviceProperty.bits.rxOnWhenIdle = gRxOn_c;

  /* If the Info about the device is already present in the neighbor table then do not
  update the relationship info else	set it to none */

  if( pDTE->deviceProperty.bits.relationship != gNeighborIsParent_c && pDTE->deviceProperty.bits.relationship != gNeighborIsChild_c )
  {
    pDTE->deviceProperty.bits.relationship = gNone_c;
  }

  pEDTE->depth = ( ( pPayload->DeviceInfo & gDeviceDepthMask_c ) >> 3 ) ;

  pEDTE->nwkUpdateId = pPayload->nwkUpdateId;

  /* Update the Permit Join subfield for this entry */
  pEDTE->joiningFeatures = 0 | ( ( ( pPanDescriptor->superFrameSpec[1] ) & gSuperFrameSpecMsbAssocPermit_c ) >> 7  );

	/* If End Device capacity or Router capacity (respectively) is True then the device is a
  potential parent, so update potential parent subfield */
	if( ( IsLocalDeviceTypeARouter() && ( pPayload->DeviceInfo & gRouterCapacity_c ) ) ||
      ( !IsLocalDeviceTypeARouter() && ( pPayload->DeviceInfo & gEndDeviceCapacity_c ) ) ) {
    pEDTE->joiningFeatures |= gMaskPotentialParent_c;
  }
  else
  {
    pEDTE->joiningFeatures &=  ~gMaskPotentialParent_c;
  }

  /* Update stack profile subfield for this entry */
  pEDTE->joiningFeatures |= ( ( pPayload->Info & 0x0F ) << 4 );

  return;
}


#if gNwkPanIdConflict_d
#if gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d
/*************************************************************************************************/
void ParseBeaconNotifyIndicationAndDetectPanIdConflict
(
  nwkBeaconNotifyInd_t *pBeaconIndication  /*IN: Ptr to BeaconIndication sent by Mac */
)
{
  /* Pointer to the pan descriptor contained in the beacon indication */
  panDescriptor_t *pPanDescriptor = pBeaconIndication->pPanDescriptor;

  /* Pointer to the beacon payload contained in the beacon indication */
  beaconPayloadData_t *pPayload = ( beaconPayloadData_t * )pBeaconIndication->pSdu;

  /* if packet is null */
  if(!pPayload || !pPanDescriptor)
    return;
 
  /* Check if beacon indication has a compatible protocol id and stack profile, if not ignore the beacon */
  if( !IsStackProfileCompatible( pPayload ) )
    return;

#if gCorruptBeaconDetection_c
  /* check if reserved bits are zero*/
  if(pPayload->DeviceInfo & gDeviceInfoReservedBits_c)
    return;
  
  /* verify tx Offset is all Fs*/
  if((pPayload->aTxOffset[0] != 0xFF) || (pPayload->aTxOffset[1] != 0xFF) || (pPayload->aTxOffset[2] != 0xFF))
    return;
#endif
  
/* This Pan Identifier detection isn't realized when the network is forming. This detection is realized 
   during the life of the device on the network. In BeeStack 2006 already exists an implementation to 
   detect a PanId conflict when the network is forming.
   */ 

  if( !Cmp8BytesToZero(NlmeGetRequest(gNwkExtendedPanId_c)) && 
      ( IsEqual2Bytes( pPanDescriptor->coordPanId ,NlmeGetRequest( gNwkPanId_c ) ) &&
      !IsEqual8Bytes( pPayload->aNwkExtPANId, NlmeGetRequest(gNwkExtendedPanId_c) ) ) ){

    /* If it is detected then the device has to create a PandId list but it first has to do a 
       MLME-SCAN.request of type scan ACTIVE. To can to do this scan the network already is formed and the
       device is ready and running on the network.

       gNwkPanIdConflictDetected indicate to the Network Discovery state 
       that it has to create a network report information pan id list 
    */
    if (ZDO_IsRunningState())
      gNwkPanIdConflictDetected = TRUE;
  }
}
#endif
#endif
/******************************************************************************
* This function is called by the network layer when a ZeD device increments
* its parent transmit failure counter.
* The function initiates a network rejoin if the transmit failure is met.
* 
* NOTE! only the transmit failure limit (gMaxDataTransmitFailure_c) should
* be changed, or alternativly the ZDO start command can be removed if a rejoin
* is not wanted.
******************************************************************************/
bool_t ZeDTransmitFailureCounterCheck(uint16_t FailureCounter)
{
  /* Polling devices should base their failures on the poll confirm. */
  if (!IsLocalDeviceReceiverOnWhenIdle())
    return FALSE;

  if (FailureCounter >= NlmeGetRequest(gNwkLinkRetryThreshold_c))
  {
    /* Commands the rejoin. */
#if gRouterCapability_d || gEndDevCapability_d || gComboDeviceCapability_d
    ZDO_ProcessDeviceNwkRejoin();
#endif
    return TRUE; /* counter limit reached return true */
  }

  return FALSE; /* counter limit not reached return false*/
}

/*****************************************************************************************
                          FREQUENCY AGILITY FUNCTIONS
*****************************************************************************************/
/*
  In this section of this file are the fucntions need it for the processing in 
  frequency agility.
*/

/************************************************************************************
* Given a 32 bit mask return a integer channel number. Or 0xFF if there is no valid
* channels. This fucntion is internal for the FA module.
*
* Interface assumptions:
*   The parameter pChannelList, is not a null pointer.
*   The parameter pChannelList, is a 32-bit mask.
*
* Return value:
*   The integer value between 11 and 26 of the first channel found in the list.
*
* Revison history:
*   Date   Author  Comments
*  ------  ------  --------
*  180208    BN    Created
************************************************************************************/
uint8_t FA_GetChannelFromList
(
  uintn32_t *pChannelList  /* IN: the 32-bits channel list, the list shall have only
                                  one valid channel. */
)
{
  /* The counter to be use during the function. */
  uint8_t i;

#ifdef __IAR_SYSTEMS_ICC__
  uint32_t cl;

  /* Transform the data to be able to use the native value */
  FLib_MemCpy(&cl, pChannelList, sizeof(uint32_t));

  /* Eliminate the not in use bits on the channel list. */
  cl = (cl >> 11);

  /* check every one of the valid channels tha zigbee can use. */
  for (i = 0; i < 16; i++)
  {
    if (cl & 1)
    {
      return (i+11);
    }

    cl = (cl >> 1);
  }

  return 0xFF;
#else
  /* Transform the data to be able to use the native value */
  *pChannelList = OTA2Native32(*pChannelList);

  /*
    Verify if the list is invalid. If the list if empty or has an invalid
    channel set, then return an error code.
  */
  if (!(*pChannelList & 0x07fff800))
    return 0xFF;

  /* Eliminate the not in use bits on the channel list. */
  *pChannelList = (*pChannelList >> 11);

  /* check every one of the valid channels tha zigbee can use. */
  for (i = 0; i < 16; i++)
  {
    if (*pChannelList & 1)
    {
      return (i+11);
    }

    *pChannelList = (*pChannelList >>1);
  }

  return 0xFF;
#endif
}


#if gFrequencyAgilityCapability_d
/*-------------------- FA_Process_Mgmt_NWK_Update_request ----------------------
  This function is available for any RxOnWhenIdle device, to process the incomming
  Mgmt_NWK_Update_Request (2.4.3.3.9 Mgmt_NWK_Update_req). 
  Upon receipt, the Remote Device shall determine from the contents of the
  ScanDuration parameter whether this request is an update to the apsChannelMask
  and nwkManagerAddr parameters, a channel change command, or a request to
  scan channels and report the results.

  IN: The package with the request information.
  IN/OUT: The buffer where the response will be build.
*/
zbSize_t FA_Process_Mgmt_NWK_Update_request
(
  zbMgmtNwkUpdateRequest_t  *pMessageComingIn,  /* IN: The package with the request information. */
  void                      *pMessageGoingOut,  /* IN/OUT: The buffer where the response will be build. */
  zbNwkAddr_t               aSrcAddrr
)
{
  /*
    Get the value to determine how the packet will intepretated.
  */
  uint8_t ScanDuration = pMessageComingIn->ScanDuration;
  uint8_t NewChannel;


  /*
    Free the big buffer being allocated by ZDP, we are not going to use it.
  */
  MSG_Free(pMessageGoingOut);

  /*
    Tell ZDP that the buffer is already free.
  */
  mbIsNotFree = FALSE;

  /*
    If the incomming request is not from the current Nwk Manager, we should ignore
    the request.
  */
  if (!IsEqual2Bytes(aSrcAddrr, NlmeGetRequest(gNwkManagerAddr_c)))
  {
    return 0;
  }

#if( gCoordinatorCapability_d || gRouterCapability_d  || gComboDeviceCapability_d )
  /*
    Requesting a Scan channels.
  */
  gScanCount = pMessageComingIn->ExtraData.ScanCount;

#if gComboDeviceCapability_d
  if ((NlmeGetRequest(gDevType_c) == gEndDevice_c)&&
      (ScanDuration < 0xfe))
  {
    return gZero_c;
  }

#endif
  if ((ScanDuration <= 0x05) && (gScanCount <= 0x05))
  {
    /*
      If the request indicates an energy scan of 0 scans, leave there is
      nothing to do.
    */
    if (!pMessageComingIn->ExtraData.ScanCount)
    {
      return gZero_c;
    }

    /*
      Since the Scan has been requested to prevent sending the notify to late 
    then stop this timer.
    */
    TMR_StopMinuteTimer(gFA_NotifyTimer);

    /*
      Catch the scan duration in the case that the command needs to be resent.
    */
    mFAScanDuration = pMessageComingIn->ScanDuration;

    /*
      Get the channellist to be use during the FA procedure.
    */
    FA_SetPrivateChannelList((void *)pMessageComingIn->aScanChannels);

    /*
      Send the request out the NWK layer.
    */
    ZdoNwkMng_EnergyScanRequest(gaScanChannelList, mFAScanDuration);
  }
  else
#endif
    /*
      Requesting a Change channel with out Energy Scan.
    */
    if (ScanDuration == 0xfe)
    {
      /*
        Send the Set Channel request to the nwk layer, through the State machine.
      */
      NlmeSetRequest(gNwkUpdateId_c, &pMessageComingIn->ExtraData.NwkManagerData.nwkUpdateId);

      /*
        We need to know which channel (number) to use, before we actually change to it.
      */
      NewChannel = FA_GetChannelFromList((uint32_t *)pMessageComingIn->aScanChannels);
#if (gEndDevCapability_d || gComboDeviceCapability_d)
#if gComboDeviceCapability_d
      if(NlmeGetRequest(gDevType_c) == gEndDevice_c)
#endif
        /*
          The ZEDs only update the update id and then, change the channel (Broadcast or unicast).
        */
        ASL_ChangeChannel(NewChannel);
#endif

#if ( gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d)
      /*
        The beacon information needs to be update as soon as possible to respond to the
        beacons request with the updated info as fast a possible.
      */
#if gComboDeviceCapability_d
      if (NlmeGetRequest(gDevType_c) != gEndDevice_c)
#endif
      {
        NWK_FRAMEHANDLE_SetBeaconPayload();

        /*
          Routers and coordinators, if the packet was unicast then, just update the update id
          and change the channel.
        */
        if( !gIsABroadcast && !IsSelfNwkAddress(aSrcAddrr) )
        {
          /*
            The ZEDs only update the update id and then, change the channel (Broadcast or unicast).
          */
          ASL_ChangeChannel(NewChannel);
        }
        else
        {
          NlmeSetRequest(gNwkLogicalChannel_c, &NewChannel);
          FA_SetNwkManagerState(gChangeChannelEnd_c);
          TMR_StartSingleShotTimer(mGenericFATimer,
                                   NlmeGetRequest(gNwkNetworkBroadcastDeliveryTime_c),
                                   FA_TimeoutHandler);
        }
      }
#endif

    }
    /*
      Requesting to change the NwkUpdateId, ApsChannelMask and the NwkManagerAdddr.
    */
    else  if (ScanDuration == 0xff)
    {
      /*
        When is 0xff, is a synchronous call, it will never leave ZDP.
      */
      NlmeSetRequest(gNwkUpdateId_c,&pMessageComingIn->ExtraData.NwkManagerData.nwkUpdateId);
      NlmeSetRequest(gNwkManagerAddr_c, pMessageComingIn->ExtraData.NwkManagerData.aNwkManagerAddr);
      ApsmeSetRequest(gApsChannelMask_c, pMessageComingIn->aScanChannels);
    }
#if ( gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d)
    /*
      If for any reason the request is invalid, we should only answer if the
      it was unicast.
    */
    else if (!gIsABroadcast)
#if gComboDeviceCapability_d
    if (NlmeGetRequest(gDevType_c) != gEndDevice_c)
#endif
    {
      /*
        Just set the right status, the rest of the payload is filled with Zeros
        by default.
      */
      gScanCount = 0xff;
      *((uint32_t *)pMessageComingIn->aScanChannels) = OTA2Native32(*((uint32_t *)pMessageComingIn->aScanChannels));
      ASL_Mgmt_NWK_Update_Notify(NlmeGetRequest(gNwkManagerAddr_c),
                                 pMessageComingIn->aScanChannels,
                                 0, 0, 0, NULL, gZdoInvalidRequestType_c);
    }
#endif
  /*
    This will force ZDP to free the message and do not send anythign to next layer.
  */
  return gZero_c;
}
#endif

#if gFrequencyAgilityCapability_d && (gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d)
/************************************************************************************
* Given a 32 bit mask representing the channel list, changes it for OTA to
* native, and set it into the local FA channel list.
*
* Interface assumptions:
*   The parameter pChannelList, if is not a null pointer and it should be a valid
*   OTA channel list.
*
* Return value:
*   NONE
*
* Revison history:
*   Date   Author  Comments
*  ------  ------  --------
*  040308    MN    Created
************************************************************************************/
void FA_SetPrivateChannelList
(
  uint32_t *pChannelList  /* IN: The list to be set as the current channel list. */
)
{
  if (!pChannelList)
  {
#ifdef __IAR_SYSTEMS_ICC__
#if gBigEndian_c
    *((uint32_t *)gaScanChannelList) = 0x07fff800;
#else
    *((uint32_t *)gaScanChannelList) = 0x00f8ff07;
#endif
#else
    *((uint32_t *)gaScanChannelList) = 0x07fff800;
#endif
  }
  else
  {
#ifdef __IAR_SYSTEMS_ICC__
    FLib_MemCpyReverseOrder(gaScanChannelList, pChannelList, sizeof(uint32_t));
#else
    *((uint32_t *)gaScanChannelList) = OTA2Native32(*((uint32_t *)pChannelList));
#endif
  }

}

/************************************************************************************
* Given a 8 bit number sets a bit into a 32 bit mask and returns the value.
*
* Interface assumptions:
*   The parameter channelNumber, is a number between 11 and 26.
*
* Return value:
*   The 32-bit mask channel list.
*
* Revison history:
*   Date   Author  Comments
*  ------  ------  --------
*  040308    MN    Updated
*  180208    BN    Created
************************************************************************************/
uint32_t FA_BuildListFromChannel
(
  uint8_t  channelNumber  /* IN: The channel number tio set a bit. */
)
{
  uint32_t  channelList=0;

  /*
    Set the right bit in to the list representing the given channel.
  */
  channelList |= ((uint32_t)1 << channelNumber);

  /*
    Change from the native format to OTA, swaping the bytes.
  */
  channelList = Native2OTA32(channelList);

  return (channelList);
}


/************************************************************************************
* This fucntion at most selects iNUmberOfRouters from the neighbor table.
*
* Interface assumptions:
*   The parameter pListOfRouters, is not a null pointer.
*   The array pointed by the parameter pListOfRouters is at least as big as the
*   number passed in the iNumberOfRouters parameter.
*
* Return value:
*   The amount of entries in the pListOfRouters array.
*   The array of Nwk Address pointed in the pListOfRouters parameter.
*
* Revison history:
*   Date   Author  Comments
*  ------  ------  --------
*  180208    BN    Created
************************************************************************************/
zbSize_t FA_GetRoutersForScanningChannels
(
  zbNwkAddr_t *pListOfRouters,  /* IN/OUT: The buffer list of routers is returned. */
  zbSize_t iNumberOfRouters     /* IN: The actual number of router extracted from the NT.*/
)
{
  neighborTable_t  *pNeighborTable;
  index_t           iIndex;
  uint8_t           iRealListSize = 0;

  /*
    If there amount of routers to get from the NT is zero, no further processign should
    be done.
  */
  if (!iNumberOfRouters)
    return iRealListSize;

  /*
    For each device in the Neighbor table....
  */
  for(iIndex = 0 ; iIndex < gNwkInfobaseMaxNeighborTableEntry_c ; iIndex++)
  {
    pNeighborTable = NlmeGetRequestTableEntry(gNwkNeighborTable_c, iIndex);

    /*
      Get only the ones with router capability to process in the FA module.
    */
    if ( pNeighborTable->deviceProperty.bits.deviceType != gRouter_c ||
        !IsValidNwkUnicastAddr(pNeighborTable->aNetworkAddress) ) {
      continue;
    }
    /*
      If we already have all the amoutn required, then we are done.
    */
    if (!iNumberOfRouters)
      break;

    /*
      Got one, one less to retrive.
    */
    iNumberOfRouters--;

    /*
      Fill the array with the current info.
    */
    Copy2Bytes(pListOfRouters[iRealListSize], pNeighborTable->aNetworkAddress);

    /*
      Report the real amount of address the deliver.
    */
    iRealListSize++;
  }

  /*
    At this point the amount of router should be at least one, if there is at
    least one in the NT.
  */
  return iRealListSize;
}

/************************************************************************************
* This function is a callback to be use for the timers needed, this timers
* handle the amount of error reports that arrive on a fixed period of time. The
* neighbor scan time out and the wait before the Nwk Manager switch channels on a
* channel change.
*
* Interface assumptions:
*   The parameter tmrId, is a previously register timer ID.
*
* Return value:
*   NONE.
*
* Revison history:
*   Date   Author  Comments
*  ------  ------  --------
*  180208    BN    Created
************************************************************************************/
void FA_TimeoutHandler
(
  tmrTimerID_t tmrId  /* IN: The timer Id to process. */
)
{
  /*
    The time in minutes to wait for the error reports to arrive into the Channel Master.
  */
  if (tmrId == mErrorReportTimer)
  {
    /*
      If the time has pass and the expected amount of error reports arrive activate
      the state machine to process into the next state.
    */
    if (mErrorReportsReceived >= gMaxIncomingErrorReports_c)
      TS_SendEvent(gFATaskId, gMaxErrorReports_c);
  }
  /*
    If the state machine is almost about to finish and the neighbor scan is done
    send the event to the state machine so it can move forward.
  */
  else if ((tmrId == mGenericFATimer) && (FA_GetNwkManagerState() == gReportToTheApplication_c))
  {
    TS_SendEvent(gFATaskId, gNeighborScanTimeout_c);
  }
  /*
    This time expires once the message has being send as a broadcast and we have done
    waiting for the Broadcast delivery time, so we jump into the final state.
  */
  else if ((tmrId == mGenericFATimer) && (FA_GetNwkManagerState() == gChangeChannelEnd_c) )
  {
    TS_SendEvent(gFATaskId, gDeliveryTimeOutExpires_c);
  }
  /*
    Timer to be set every time a NwkUpdate Notify has been sent so to avoid repoorting
    to often.Acording to the spec should be 15 min. 
  */
  else if (tmrId == gFA_NotifyTimer)
  {
    /* If the timer expire then check if we have any scans to be done */
    if (gScanCount)
      ZdoNwkMng_EnergyScanRequest(gaScanChannelList, mFAScanDuration);
  }
}

/************************************************************************************
* This function is only available for the Network Manager (Channel Master), to 
* process the incomming Mgmt_NWK_Update_Notify.
*
* Upon receipt of an unsolicited Mgmt_NWK_Update_notify, the network manager
* must evaluate if a channel change is required in the network. For this there
* are several steps to follow:
* Request other interference reports using the Mgmt_NWK_Update_req command.The
* network manager may request data from randomly selected routers in the network.
* (as indicated in the Zigbee spec R17- Annex E, for the purpose of giving the
* user a simple example of how and where this is done and it can be adjusted
* as need it).
*
* Interface assumptions:
*   The parameter pMessageComingIn, is not a null pointer.
*
* Return value:
*   NONE.
*
* Revison history:
*   Date   Author  Comments
*  ------  ------  --------
*  180208    BN    Created
************************************************************************************/
void FA_Process_Mgmt_NWK_Update_notify
(
  zbMgmtNwkUpdateNotify_t  *pMessageComingIn  /* IN: The package with the request information. */
)
{
  /* If the node receiving the MgmtNwkUpdateNotify is not a Network Manager then
     then do nothing and end the function. */
  if (!ZdoGetNwkManagerBitMask(gaServerMask))
    return;

  /*
    It does not matter if the current report was requested or not. Process it anyway.
  */
  FA_ReportAnalysis(pMessageComingIn->aEnergyValues,
                    pMessageComingIn->ScannedChannels,
                    pMessageComingIn->ScannedChannelsListCount);

  /*
    If the State machine is on init, this is an unrequested update notify.
  */
  if (FA_GetNwkManagerState() == gInitState_c)
  {
    /*
      If this is the first unrequested report received, then start the timer.
    */
    if (!mErrorReportsReceived)
    {
      /*
        This is also a valid report so consider it in the incoming amount.
      */
      mErrorReportsReceived++;

      /*
        Start a timer with the maximumm amoutn of minutes to wait for the
        incomming error reports.
      */
      TMR_StartMinuteTimer(mErrorReportTimer,
                           (tmrTimeInMinutes_t)gMaxTimeoutForIncomingErrorReports_c,
                           FA_TimeoutHandler);
    }

    /*
      If this is not the first report that arrives and timer has not expire,
      but we reach the limit of reports, then, stop the timer, clear the amount
      of incoming erros and send the event to the FA State machine.
    */
    else
    {
      if ((++mErrorReportsReceived) >= gMaxIncomingErrorReports_c)
      {
        TMR_StopMinuteTimer(mErrorReportTimer);
        TS_SendEvent(gFATaskId, gMaxErrorReports_c);
      }
    }  
  }

  /*
    If the repors arrive becasue the Nwk Manager request them then, 
  */
  if (FA_GetNwkManagerState() == gNeighborEnergyScan_c)
  {
    mNumScansReceived++;
  }
}

/************************************************************************************
* Gets the amount of active channels in the apsCheannel mask and use it for the time
* out period to wait before the Nwk manager changes it's own current channel.
*
* Interface assumptions:
*   NONE.
*
* Return value:
*   NONE.
*
* Revison history:
*   Date   Author  Comments
*  ------  ------  --------
*  180208    BN    Created
************************************************************************************/
void FA_SetWaitingForReportsTimer
(
  void
)
{
  /*
    The time in millisecond for the current scanduration.
  */
  uint32_t scanMilliseconds=0;

  uint8_t i;

  uintn32_t  channelList;

  /*
    Get the current channel list, we will use it to count the amount of channels set into it.
  */
  channelList = OTA2Native32(*((uint32_t *)ApsmeGetRequest(gApsChannelMask_c)));

  /*
    Eliminate the un used bits.
  */
  channelList = (channelList >> 11);

  for (i = 0; i < 16; i++)
  {
    channelList = (channelList >> i);
    if (channelList & ((uint32_t)1))
      /*Increment scan duration for each channel.*/
      scanMilliseconds += ScanDurationToMilliseconds(gDefaultScanDuration_c);
  }

  TMR_StartSingleShotTimer(mGenericFATimer,
                          (scanMilliseconds) + gExtraTimeWindow_c,
                          FA_TimeoutHandler);
}


/************************************************************************************
* Selects an specific amount of router from the neighbor table and sends a unicast
* message to every single one, this is how the Nwk Manager requests a energy scan
* to the routers.
*
* Interface assumptions:
*   NONE.
*
* Return value:
*   NONE.
*
* Revison history:
*   Date   Author  Comments
*  ------  ------  --------
*  180208    BN    Created
************************************************************************************/
void FA_SendRequestEnergyScanRequestToRouters
(
  void
)
{
  /*
    A local list of routers availables.
  */
  zbNwkAddr_t aListOfRouters[gMinNumberOfRouters_c];

  /*
    The ideal number of router to use for the analisys.
  */
  zbSize_t iNumberOfRouters = gMinNumberOfRouters_c;
  uint8_t i;

  /*
    Get any available router to use for the energy analisys.
  */
  iNumberOfRouters = FA_GetRoutersForScanningChannels(aListOfRouters, iNumberOfRouters);

  /*
    We did not found any routers availables.
  */
  if (!iNumberOfRouters)
    return;

  mNumScansSent = iNumberOfRouters;

  /*
    For each router found send an Energy scan request to process the energy levels.
  */
  for (i = 0; i < iNumberOfRouters; i++)
  {
    ASL_Mgmt_NWK_Update_req(aListOfRouters[i],
                            gaScanChannelList,
                            gDefaultScanDuration_c,
                            gDefaultScanCount_c);
  }

}

/************************************************************************************
* Choose from the global array that keeps tract of the energy info of the cahnnels
* the less noicy one. Use a simple algorithm to select it. Asumes that the current
* channel is the best one and compares it, one to one with whole array.
*
* Interface assumptions:
*   NONE.
*
* Return value:
*   TRUE if a new channel was found. FALSE if it keep the current channel as the choosen one.
*
* Revison history:
*   Date   Author  Comments
*  ------  ------  --------
*  180208    BN    Created
************************************************************************************/
bool_t FA_SelectChannelFromList
(
  void
)
{
  uint8_t i;
  uint8_t theChosenOne;
  uint8_t theOneIndex;

  theOneIndex = (NlmeGetRequest(gNwkLogicalChannel_c)-11);
  theChosenOne = mChannels[theOneIndex];

  for (i = 0; i < 16; i++)
  {
    
    if (theChosenOne <= mChannels[i])
      continue;

    theChosenOne = mChannels[i];
    theOneIndex = i;
  }

  theOneIndex +=11;
  if (theOneIndex == NlmeGetRequest(gNwkLogicalChannel_c))
    return FALSE;

  NlmeSetRequest(gNwkLogicalChannel_c, &theOneIndex );
  return TRUE;
}

/************************************************************************************
* All channels are initialized with the middle energy value(0x7f) (most noisy
* channel has the value 0xff).
* This function updates each channel up/down based on the energy value from the
* scanned channel reported in the notify information packet.
*
* Interface assumptions:
*   The parameter pMessageComingIn, is not a null pointer.
*
* Return value:
*   NONE.
*
* Revison history:
*   Date   Author  Comments
*  ------  ------  --------
*  180208    MN    Modified
************************************************************************************/
void FA_ReportAnalysis
(
  zbEnergyValue_t  *pEnergyValues,
  zbChannels_t  ScannedChannels,
  uint8_t  scannedIndex
)
{
  uint8_t i;

  /*
    The incoming channel list on native format.
  */
  uint32_t channelList;

  /*
    Transform the data to be able to use the native value.
  */
  channelList = OTA2Native32(*((uint32_t*)ScannedChannels));

  /* Verify if the channel list is invalid.*/
  if (!(channelList & 0x07fff800))
  return;

  /*
    Eliminate the bits that are not used.
  */
  channelList = (channelList >> 11);

  /* Check every channel... update up/down the channel[] for report analysis */
  for (i = 0; i < 16; i++)
  {
    /* Check if channel was scanned */
    if (!((channelList >> i) & 1))
      continue;

    /* Verify if there are more Energy reported values  */
    if (scannedIndex)
      return;

    /*
      Update up/down the mChannels array, based on the energy value from the
      scanned channel reported by the notify information packet.
    */
    if (pEnergyValues[scannedIndex] < 0x7f)
    {
      if (pEnergyValues[scannedIndex] <= 32)
        mChannels[i]-=7;
      else if (pEnergyValues[scannedIndex] <= 64)
        mChannels[i]-=5;
      else if (pEnergyValues[scannedIndex] <= 96)
        mChannels[i]-=3;
      else
        mChannels[i]-=1;
    }
    else if(pEnergyValues[scannedIndex] <= 159)
      mChannels[i]+=1;
    else if(pEnergyValues[scannedIndex] <= 191)
      mChannels[i]+=3;
    else if(pEnergyValues[scannedIndex] <= 223)
      mChannels[i]+=5;
    else
      mChannels[i]+=7;

    /*
      Keep track of how many channels come in the report.
    */
    scannedIndex--;
  } /* for (i = 0; i < 16; i++) */

}

/************************************************************************************
* Catch every time that the nwk layr sends a Tx report,inorder to keep track of
* the Trasnmitions attempts and the transmitions failures.
*
* Interface assumptions:
*   The parameter pNlmeNwkTxReport, is not a null pointer.
*
* Return value:
*   NONE.
*
* Revison history:
*   Date   Author  Comments
*  ------  ------  --------
*  150208    BN    Created
*  180208    MN    Modified
************************************************************************************/
void FA_ProcessNlmeTxReport
(
  nlmeNwkTxReport_t  *pNlmeNwkTxReport  /* IN: The report from the Nwk Layer. */
)
{
  /*
    The current percentage of failures, of the total of transmitions attempts.
  */
  uint16_t percentageOfFailure;

  if (!ZDO_IsRunningState())
  {
    return;
  }

  /*
    If TxAttempts is the total of transmitions, then, that is our 100%.
    So using a 3 value mechanism (3 simple rule) we calculate at what
    porcentage is equal the TxFailures corresponding to the current TxAttempts.
  */
  percentageOfFailure = (uint16_t)((pNlmeNwkTxReport->TxFailures * 100)/pNlmeNwkTxReport->TxAttempts);

  /*
    If this value exceed the porcentage that is the limit of tolerance, then,
    send the proper message.
  */
  if (percentageOfFailure >= gMaxTxFailuresPercentage_c)
  {
    /*
      Let the state machine know about the current event.
    */
    TS_SendEvent(gFATaskId, gMaxTransmitionsFailure_c);
  }
}

/************************************************************************************
* Initialize all the values needed to have the FA state machine running on the
* Nwk Manager device.
*
* Interface assumptions:
*   NONE.
*
* Return value:
*   NONE.
*
* Revison history:
*   Date   Author  Comments
*  ------  ------  --------
*  150208    BN    Created
*  180208    MN    Modified
************************************************************************************/
void FA_StateMachineInit
(
  void
)
{
  /*
    Create the task to use for the State machine.
  */
  gFATaskId = TS_CreateTask(gTsFirstApplicationTaskPriority_c +1, FA_StateMachine);

  /*
    Set the state machine ready to run, before this the state machine is not working at all.
  */
  FA_SetNwkManagerState(gInitState_c);

  /*
    Clear the error globals.
  */
  mErrorReportsReceived = 0;
  mNumScansReceived = 0;
  mNumScansSent = 0;

  /*
    Get the timers needed fro the state machine.
  */
  mErrorReportTimer = TMR_AllocateMinuteTimer();

  /*
    Allocate a  multipropouse timer.
  */
  mGenericFATimer = TMR_AllocateTimer();

  /*
    Allocate the minute timer for the update notify.
  */
  gFA_NotifyTimer = TMR_AllocateMinuteTimer();

  /*
    if the current device is a coordinator set it as a netwrok manager
  */
#if gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d
  /*
    Set the bit On in the server mask of the node descriptor. But Only to the
    coordinator by default.
  */

#if (gNetworkManagerCapability_d || gComboDeviceCapability_d)
#if gComboDeviceCapability_d
  if (NlmeGetRequest(gDevType_c) == gCoordinator_c)
#endif
  {
    ZdoSetNwkManagerOn(gaServerMask);
  }
#else
  ZdoSetNwkManagerOff(gaServerMask);
#endif

#endif
}

/************************************************************************************
* Time out function used to generate a Energy scan after a fixed period of time.
* Normally is used when with the scan count to avoid memory problems or fails on
* the notify delivery.
*
* Interface assumptions:
*   The parameter Timer is a vlid timer Id.
*
* Return value:
*   NONE.
*
* Revison history:
*   Date   Author  Comments
*  ------  ------  --------
*  180208    BN    Created
************************************************************************************/
void GenerateEnergyRequest
(
  tmrTimerID_t Timer
)
{
  (void) Timer;
  ZdoNwkMng_EnergyScanRequest(gaScanChannelList,gScanDuration_c);
}

/************************************************************************************
* The function is use either by the Network manager or by any other device
* participating on the FA porcess. This function catchs every single energy scan
* confirm from the Nwk layer and process it.
*
* Interface assumptions:
*   The paramter pScanCnf is not a null pointer.
*
* Return value:
*   NONE.
*
* Revison history:
*   Date   Author  Comments
*  ------  ------  --------
*  180208    BN    Created
************************************************************************************/
void FA_ProcessEnergyScanCnf
(
  nlmeEnergyScanCnf_t *pScanCnf  /* The Nlme scan confirm. */
)
{

#if gComboDeviceCapability_d
   /* Combo device acting like ZEd should not go further. */
  if (NlmeGetRequest(gDevType_c) == gEndDevice_c)
  {
    return;
  }
#endif

  if (!ZDO_IsRunningState())
  {
    return;
  }

  /*
    If the current node is the network manager process the energy scan as a local
    energy scan and inform the state amchine about it.
  */
  if (ZdoGetNwkManagerBitMask(gaServerMask))
  {
    /*
      Only the Network manager keeps track of every energy scan report.
    */
    FA_ReportAnalysis(pScanCnf->resList.pEnergyDetectList,
                      ApsmeGetRequest(gApsChannelMask_c),
                      pScanCnf->resultListSize);

    /*
      Change the values of the Nlme counters.
    */
    ResetTxCounters();

    /*
      Let the state machine know that the local energy scan is done.
    */
    TS_SendEvent(gFATaskId, gLocalScanArrive_c);
    return;
  }

  /*
    If node is not the Network manager and the current energy scan fails,
    there is nothing to do.
  */
  if (pScanCnf->status)
    return;

  if (TMR_IsTimerActive(gFA_NotifyTimer))
    return;

  /*
    Keep track of how many energy scan are left to do, just used for frequency agility.
  */
 gScanCount -- ;

  /*
    Send a Notify Message to the NwkManager for each energy scan received, only
    if the Timer is not active, to avoid reporting errors to often.
  */
  ASL_Mgmt_NWK_Update_Notify(NlmeGetRequest(gNwkManagerAddr_c),
                             gaScanChannelList,
                             NlmeGetRequest(gNwkTxTotal_c),
                             NlmeGetRequest(gNwkTxTotalFailures_c),
                             pScanCnf->resultListSize,
                             pScanCnf->resList.pEnergyDetectList,
                             gZbSuccess_c);

  /*
    If there is more energy scans left ot do, process the next one, but not too often
    to avoid memory and delivery problems.
  */
  if (gScanCount)
  {
    TMR_StartSingleShotTimer(mGenericFATimer, 250, GenerateEnergyRequest);
    return;
  }

  FA_SetNwkManagerState(gInitState_c);
}

/************************************************************************************
* Use the information generated during the FA process and build a specific message,
* a notify in the case of the none Nwk Manager nodes, and an event to the state
* machine in the case of the Network Manager.
*
* Interface assumptions:
*   NONE.
*
* Return value:
*   NONE.
*
* Revison history:
*   Date   Author  Comments
*  ------  ------  --------
*  180208    BN    Created
************************************************************************************/
void FA_SendReportToApplication
(
  void
)
{
  zdpToAppMessage_t  *pMsg;
  sChannelMasterStatistics_t  *pChannelReport;

#if gComboDeviceCapability_d
   /* Combo device acting like ZEd should not go further. */
  if (NlmeGetRequest(gDevType_c) == gEndDevice_c)
  {
    return;
  }
#endif

  pMsg = MSG_Alloc(sizeof(sChannelMasterStatistics_t) + sizeof(zbMsgId_t));

  if (!pMsg)
    return;

  /*
    Build the error tobe send out to the application.
  */
  pMsg->msgType = gChannelMasterReport_c;
  pChannelReport = &pMsg->msgData.channelMasterReport;

  pChannelReport->reason = mScanReason;
  pChannelReport->numErrorReports = mErrorReportsReceived;
  pChannelReport->numScansReceived = mNumScansReceived;
  pChannelReport->numScansRequest = mNumScansSent;

  FLib_MemCpy(pChannelReport->aChannels, mChannels, sizeof(mChannels));

  /*
    Use the ZDP Sap Handler to be able to send the report and see it thourgh test tool.
  */
  if (ZDP_APP_SapHandler(pMsg))
  {
    /*
      Catch Error here.
    */
  }

  /*
    Clear the FA globals
  */
  mErrorReportsReceived = 0;
  mNumScansReceived = 0;
  mNumScansSent = 0;
}

/************************************************************************************
* The state machine used by the network manager to keep track of the FA procedure
* described in 053474r17 Annex E, uses a task and events. This state machine is also
* used by the routers particiapting in the FA procedure.
*
* Interface assumptions:
*   NONE.
*
* Return value:
*   NONE.
*
* Revison history:
*   Date   Author  Comments
*  ------  ------  --------
*  180208    BN    Created
************************************************************************************/
void FA_StateMachine
(
  event_t events
)
{
/* to avoid IAR compiler warning */  
#if (gMgmt_NWK_Update_req_d && (gNetworkManagerCapability_d || gComboDeviceCapability_d))
  zbChannels_t  aChannels;
#endif  

#if gComboDeviceCapability_d
   /* Combo device acting like ZEd should not go further. */
  if (NlmeGetRequest(gDevType_c) == gEndDevice_c)
  {
    return;
  }
#endif

  switch (FA_GetNwkManagerState())
  {
    /*
      S1: The state machine is on the initial state
    */
    case gInitState_c:
      /*
        If the event Ev1 (ChangeChannel) arrives, change state to S5 (ChangeChannelStart).
      */
      if (events & gChangeChannel_c)
      {
        FA_SetNwkManagerState(gChangeChannelStart_c);
        TS_SendEvent(gFATaskId, gChangeChannel_c);
      }

      /*
        If the number maximum of error repots (Ev2) has arrive the change to S2
        (LocalEnergyScan).
      */
      if (events & gMaxErrorReports_c)
      {
        FA_SetNwkManagerState(gLocalEnergyScan_c);
        TS_SendEvent(gFATaskId, gExecuteState_c);
        mScanReason = gScanReasonMaxErrorReports_c;
      }

      /*
        If The number of maximum transmitions failures has being reached ()Ev3,
        we need to do a Local energy Scan (S2).
      */
      if (events & gMaxTransmitionsFailure_c)
      {
        FA_SetNwkManagerState(gLocalEnergyScan_c);
        TS_SendEvent(gFATaskId, gExecuteState_c);
        mScanReason = gScanReasonTxFailureCount_c;
      }

      /*
        If the application command to do an energy scan ,then, change the current
        state and proceed.
      */
      if (events & gInitiateEnergyScan_c)
      {
        FA_SetNwkManagerState(gLocalEnergyScan_c);
        TS_SendEvent(gFATaskId, gExecuteState_c);
        mScanReason = gScanReasonUserInitiated_c;
      }
      break;

    /*
      S2: Do a local energy scan.
    */
    case gLocalEnergyScan_c:
      /*
        To avoid errors, just execute the code deliberately, with an event.
      */
      if (events & gExecuteState_c)
      {
        /*
          To avoid wrong execution only change the state inside the state machine.
        */
        FA_SetNwkManagerState(gNeighborEnergyScan_c);
        FA_SetPrivateChannelList(NULL);
        gScanCount = 1;
        (void)ZdoNwkMng_EnergyScanRequest(gaScanChannelList,gDefaultScanDuration_c);
      }
      break;

    /*
      S3: Do a Neighbor energy scan.
    */
    case gNeighborEnergyScan_c:
      /*
        The incoming event is "Local Energy scan results arrive" (Ev5), we most start
        telling the neighbors to do the same.
      */
      if (events & gLocalScanArrive_c)
      {
        FA_SetNwkManagerState(gReportToTheApplication_c);
        FA_SendRequestEnergyScanRequestToRouters();
        FA_SetWaitingForReportsTimer();
      }
      break;

    /*
      S4: Build the report to be send up to the application.
    */
    case gReportToTheApplication_c:
        if (events & gNeighborScanTimeout_c)
        {
          FA_SetNwkManagerState(gInitState_c);
          FA_SendReportToApplication();
        }
      break;

    /*
      S5: The procedure to start the change of a channel gets started.
    */
    case gChangeChannelStart_c:
      if (events & gChangeChannel_c)
      {
        FA_SetNwkManagerState(gChangeChannelEnd_c);
/* to avoid warning on IAR */        
#if (gMgmt_NWK_Update_req_d &&(gNetworkManagerCapability_d || gComboDeviceCapability_d))
#if gComboDeviceCapability_d
      if ((NlmeGetRequest(gDevType_c) != gEndDevice_c)&& ZdoGetNwkManagerBitMask(gaServerMask))
#endif
        if (ZdoGetNwkManagerBitMask(gaServerMask))
        {
        *((uint32_t *)aChannels) = FA_BuildListFromChannel(NlmeGetRequest(gNwkLogicalChannel_c));
        ASL_Mgmt_NWK_Update_req((uint8_t *)gaBroadcastRxOnIdle, aChannels, 0xfe, 0);
        }
#endif
        TMR_StartSingleShotTimer(mGenericFATimer,
                                 NlmeGetRequest(gNwkNetworkBroadcastDeliveryTime_c),
                                 FA_TimeoutHandler);
      }
      break;

    /*
      S6: The final state here the onlything that we can do is change to current
      logical channel.
    */
    case gChangeChannelEnd_c:
      if (events & gDeliveryTimeOutExpires_c)
      {
        FA_SetNwkManagerState(gInitState_c);
        ASL_ChangeChannel(NlmeGetRequest(gNwkLogicalChannel_c));
      }
      break;
  }
}

/************************************************************************************
* Used byt the application to select and change to a different channel if is requiered.
*
* Interface assumptions:
*   NONE.
*
* Return value:
*   NONE.
*
* Revison history:
*   Date   Author  Comments
*  ------  ------  --------
*  180208    BN    Created
************************************************************************************/
void FA_SelectChannelAndChange
(
  void
)
{
#if gComboDeviceCapability_d
   /* Combo device acting like ZEd should not go further. */
  if (NlmeGetRequest(gDevType_c) == gEndDevice_c)
  {
    return;
  }
#endif

  if (!ZDO_IsRunningState())
  {
    return;
  }

  if (FA_SelectChannelFromList())
    FA_SendChangeChannelEvt();
}
#endif
/*gFrequencyAgilityCapability_d*/


/*-------------------- ScanDurationToMilliseconds ----------------------
Calculate the length
of time to spend scanning each
channel. The time spent scanning
each channel is (aBaseSuperframeDuration * (2^n + 1)) symbols, where n is the value of
the ScanDuration parameter(0x00-0x05)

IN: ScanDuration value between 0x00..0x05.
OUT: milliseconds corresponding for ScanDuration
*/
uint32_t ScanDurationToMilliseconds
(
  uint8_t scanDuration
)
{
/* constValue is used to calc. the time in microseconds...
   constValue = (aBaseSuperframeDuration(16*60)) * (time for a symbol(16microsec))=15360 */
#define constValue (16*60*16)

/*Verify if wrong parameter*/
if (scanDuration > 0x05)
    return 0;

/* (constValue * (2^n + 1))\1000 miliseconds */
return (  ( ((uint32_t)constValue * (uint32_t)((0x01<<scanDuration)+1)) )/1000 );
}

/*****************************************************************************************
                     SECURITY SPECIFIC AND EXPOSED APIs
*****************************************************************************************/
#if gStandardSecurity_d ||gHighSecurity_d
#if gTrustCenter_d || gComboDeviceCapability_d
void AppAuthenticateDevice
(
  zbIeeeAddr_t  aIeeeAddress
)
{
  uint8_t i;
  bool_t DeviceAllow = TRUE;

  for (i =0; i < gExclusionMax; i++)
  {
    if (IsEqual8Bytes(aIeeeAddress, gaExclusionTable[i]))
    {
      DeviceAllow = FALSE;
      break;
    }
  }

  ZDO_SendAuthenticationToTrustCenter(DeviceAllow);
}
#endif
#endif

/*
  given an Ieee Address this function will copmpare its OUI bytes against the aCompanyId, 
  which is the permitted group of addressesses in this network. it will return true if 
  the given address is valid otherwise it will return false.
*/
bool_t ValidateIeeeAddress( zbIeeeAddr_t aIeeeAddress ) 
{
  static const zbIeeeAddr_t aFilterMask = { gIeeeFilterMask_c };
  static const zbIeeeAddr_t aFilterValue = { gIeeeFilterValue_c };
  index_t i;

  /* no filter, always is valid IEEE address */
  if(Cmp8BytesToZero((uint8_t *)aFilterMask))
    return TRUE;

  /* check each byte in filter */
  for(i=0; i<sizeof(zbIeeeAddr_t); ++i)
  {
    if((aIeeeAddress[i] & aFilterMask[i]) != (aFilterValue[i] & aFilterMask[i]))
      return FALSE;
  }
  return TRUE;
}

/*****************************************************************************************
                     PERMISSIONS CONFIGURATION TABLE
*****************************************************************************************/
#if ((gStandardSecurity_d || gHighSecurity_d) && gApsMaxEntriesForPermissionsTable_c)

extern uint8_t * APS_GetIeeeAddress
(
  uint8_t * pNwkAddr,
  uint8_t   aWereToCpyExtAddr[]
);

extern zbKeyType_t ZDO_SecGetDeviceKeyType
(
  zbIeeeAddr_t aDeviceAddress
);

extern addrMapIndex_t APS_FindIeeeInAddressMap
(
  zbIeeeAddr_t aExtAddr
);

extern addrMapIndex_t APS_AddIeeeToAddressMap
(
  zbIeeeAddr_t aExtAddr
);

/**********************************************************************************
* Verify if the requesting device have authorization to carry out the incoming command, where
* aSrcAddr is the address of the requesting device, and permissionsCategory is the bit mask
* representing the incoming command.
*
* Return value:
*   Authorization flag, TRUE if the command was authorized, and FALSE otherwise.
*
**********************************************************************************/
bool_t CommandHasPermission
(
  zbNwkAddr_t aSrcAddr,
  permissionsFlags_t  permissionsCategory
)
{
  uint8_t i;
  uint8_t *pIeeeSrcAddr, aExtAddr[8];
  zbKeyType_t pKeyType=0xFF;
  
  /* If Permissions Table is in its default state authorize always the command */
  if (!gPermissionsTableCount)
    return TRUE;

  /* Obtain the Ieee address from the neighborhood table or the address map */
  pIeeeSrcAddr = APS_GetIeeeAddress(aSrcAddr, aExtAddr);
  if (!pIeeeSrcAddr)
  {
    /* We dont know anything sbout this device, fail to authorize. */
    return FALSE;
  }

  /* We need to know the key type being in, use */
  pKeyType = ZDO_SecGetDeviceKeyType(pIeeeSrcAddr);

  /* If the command is secured with the Trust Center Link Key then authorize it without checking */
  if ((pKeyType == gTrustCenterLinkKey_c) && IsEqual8Bytes(pIeeeSrcAddr, ApsmeGetRequest(gApsTrustCenterAddress_c)))
  {
    /* The packet was secured with TC link key. */
    return TRUE;
  }

  /* Look if there is an entry in the table authorizing the device to perform the command */
  for (i = 0; i < gApsMaxEntriesForPermissionsTable_c; i++)
  {
    if (!IsEqual8Bytes(pIeeeSrcAddr, gaPermissionsTable[i].authorizedDevice))
    {
      /* Skip all the entries with a different IEEE address. */
      continue;
    }

    /* Verify if a link key is required and if the device has it */
    if ((gLinkKeyRequired_c & gaPermissionsTable[i].permissionsFlags) && (pKeyType != gApplicationLinkKey_c))
    {
      return FALSE;
    }

    /* Verify if device has permission */
    if (permissionsCategory & gaPermissionsTable[i].permissionsFlags)
    {
      return TRUE;
    }

    /* Device found in the permissions table but is not authorized. */
    break;
  }

  return FALSE;
}

/**********************************************************************************
* Add a device to the Permissions Configuration Table, where aDevAddr is the IEEE
* device address, and permissionCategory is the bit mask representing the device
* permissions.
* Adding a device in this table include to adding it also to the address map.
* 
* Return values:
*   gZbSuccess_c
*   gZdoTableFull_c
*
**********************************************************************************/
uint8_t	AddDeviceToPermissionsTable
(
  zbIeeeAddr_t aDevAddr,
  permissionsFlags_t  permissionsCategory
)
{
  uint8_t i;
  int8_t iAvailable = -1;
  
  /* Look if the device is already in the table  */
  for (i = 0; i < gApsMaxEntriesForPermissionsTable_c; i++)
  {
    if (IsEqual8Bytes(&aDevAddr[0], &gaPermissionsTable[i].authorizedDevice[0]))
    {
      /* Save the entry index to update */
      iAvailable = i;
      break;
    }
    /* Save the free entry index */
    if (!(gaPermissionsTable[i].permissionsFlags & gPermissionsEntryActive))
      iAvailable = i;
  }
  if (iAvailable > -1)
  {    
    /* Copy the device IEEE address */
    FLib_MemCpy(&gaPermissionsTable[iAvailable].authorizedDevice[0], aDevAddr, sizeof(zbIeeeAddr_t));
    /* Set the device permissions */
    gaPermissionsTable[iAvailable].permissionsFlags = (permissionsCategory | gPermissionsEntryActive);
    gPermissionsTableCount++;
    return gZbSuccess_c;
  }
  return gZdoTableFull_c;
}

/**********************************************************************************
* Remove a device from the Permissions Configuration Table. This function only removes the device
* from the Permissions Configuration Table, not from the Address Map.
* 
* Return values:
*   gZbSuccess_c
*   gZdoNoEntry_c
*
**********************************************************************************/
uint8_t RemoveDeviceFromPermissionsTable
(
  zbIeeeAddr_t  aDevAddr
)
{
  uint8_t i;
  
  /* Search in table only when there are active entries */
  if (gPermissionsTableCount) 
  {  
    /* Search for the entry to delete */
    for (i = 0; i < gApsMaxEntriesForPermissionsTable_c; i++)
    {
      if (IsEqual8Bytes(aDevAddr, gaPermissionsTable[i].authorizedDevice))
      {
        /* The entry must have the Ieee Address to delete and be an active one */
        if (gaPermissionsTable[i].permissionsFlags & gPermissionsEntryActive)
        {
          BeeUtilZeroMemory(&gaPermissionsTable[i], sizeof(permissionsTable_t));
          gPermissionsTableCount--;
          return gZbSuccess_c;
        }
        return gZdoNoEntry_c;
      }
    }
  }
  return gZdoNoEntry_c;
}

/**********************************************************************************
* Remove all devices from the Permissions Configuration Table and leave it in its default state.
* 
* Return value:
*
**********************************************************************************/
void RemoveAllFromPermissionsTable(void)
{
  /* Just set all Permissions Table entries to zero */
  BeeUtilZeroMemory(gaPermissionsTable, sizeof(permissionsTable_t) * gApsMaxEntriesForPermissionsTable_c);
  gPermissionsTableCount = 0;
}

/**********************************************************************************
* Get the Permissions Configuration Table.
* 
* Return value:
*
**********************************************************************************/
index_t GetPermissionsTable
(
  uint8_t * pDstTable
)
{
  uint8_t i, count=0;
  
  /* No need to search in table when there is no active entry */
  if (!gPermissionsTableCount)
    return count;
    
  /* Search in Permissions Table */
  for (i = 0; i < gApsMaxEntriesForPermissionsTable_c; i++)
  {
    /* Retrieve only the active entries */
    if (gaPermissionsTable[i].permissionsFlags & gPermissionsEntryActive)
    {
      FLib_MemCpy(&pDstTable[sizeof(permissionsTable_t) * count], &gaPermissionsTable[i], sizeof(permissionsTable_t));
      count++;
    }
  }
  /* Return the number of active entries in Permissions Table */
  return count;
}

/**********************************************************************************
* Set the Permissions Configuration Table.
* 
* Return value:
*
**********************************************************************************/
void SetPermissionsTable
(
  index_t entryCounter,
  uint8_t * payload
)
{
  /* Verification to don't overrun the array */
  if(entryCounter > gApsMaxEntriesForPermissionsTable_c)
    entryCounter = gApsMaxEntriesForPermissionsTable_c;
  
  /* Fill the non set entries with zeros */
  BeeUtilZeroMemory(&payload[(sizeof(permissionsTable_t) * entryCounter)], sizeof(permissionsTable_t) * (gApsMaxEntriesForPermissionsTable_c - entryCounter));
  
  /* Set Permissions Table */
  FLib_MemCpy(gaPermissionsTable, payload, sizeof(permissionsTable_t) * gApsMaxEntriesForPermissionsTable_c);
  gPermissionsTableCount = entryCounter;
}

#endif  /* (gZtcIncluded_d && (gStandardSecurity_d || gHighSecurity_d) && gApsMaxEntriesForPermissionsTable_c) */


/******************************************************************************
* This function looks for a free entry in the source route table.  
*
* Return: The index of that entry, if it's free, otherwise return gNwkEntryNotFound_c
*         (0xFF)
******************************************************************************/ 
index_t GetFreeEntryInSourceRouteTable( void )
{
	index_t iIndex;
	for( iIndex = 0; iIndex < gMaxSourceRouteEntries; iIndex++ ) {
		if( IsEqual2Bytes(gaSourceRouteTable[iIndex].aDestinationAddress, gaBroadcastAddress) ){
      return iIndex;
    }
  }
 return gNwkEntryNotFound_c; 
}
/******************************************************************************
* This function look for an specific entry in source route Table with specific 
* find type.
*
* Returns:  Source route entry if destination address is found, otherwise return 
*           NULL
******************************************************************************/
sourceRouteTable_t* NwkRetrieveSourceRoute(uint8_t *pDestinationAddress){

  /* Index to traverse throught the source route table*/
  uint8_t iIndex;

  /* pointer to source route table entry */
  sourceRouteTable_t*  pSourceRouteTable;

  for (iIndex = 0; iIndex < gMaxSourceRouteEntries; iIndex++) {
    pSourceRouteTable = &( gaSourceRouteTable[ iIndex ] );

    /* if address is found then return the the pointer to this entry*/
    if( IsEqual2Bytes( pSourceRouteTable->aDestinationAddress,pDestinationAddress))
		{return( pSourceRouteTable );}

  }
  return NULL;
}

/******************************************************************************
* This function creates and entry if it doesn't exists in the source route table 
* or update it if it already exist.
* 
* Return: void
******************************************************************************/ 
extern void GenerateNlmeNwkStatusIndication(zbStatus_t pNwkStatus, uint8_t * pShortAddress);

void NwkStoreSourceRoute(uint8_t* pDestinationAddress, routeRecord_t*  pRouteRecord){

  sourceRouteTable_t *pSRTE = NULL;

  index_t indexOfFreeEntryInSourceRouteTable;

  RouteRecordRelayList_t *pRouteRecordList;
  
  /* Verify if already exists an entry for that destination address */
  if( pNwkRetrieveSrcRoute )
    pSRTE = pNwkRetrieveSrcRoute(pDestinationAddress);

  
  /* If the entry doesn't exist then get a free entry */
  if(!pSRTE){

    if( pNwkGetFreeEntryInSrcRouteTable ){
      indexOfFreeEntryInSourceRouteTable = pNwkGetFreeEntryInSrcRouteTable();

      if(indexOfFreeEntryInSourceRouteTable !=  gNwkEntryNotFound_c){
        pSRTE =  &gaSourceRouteTable[ indexOfFreeEntryInSourceRouteTable ];
      }
    }
  }
  /* if the entry exists or is new then fill it */
  if(pSRTE){
    /* Free the previous information and re-fill with the new. We assume that this entry
      has the path info empty - NULL - when it is free. */
    if ( pSRTE->path )
      FreeSourceRouteEntry( (uint8_t)(pSRTE - gaSourceRouteTable) );
    
    /* First of all, we need to allocate the relay list */
    pRouteRecordList = MSG_Alloc( (pRouteRecord->relayCount << 1) );

    /* If there is no memeory then do not copy the source route entry */
    if(!pRouteRecordList){
      (void)GenerateNlmeNwkStatusIndication(gZbNoMem_c, pDestinationAddress);
      return;
    }
    /* Copy the destination address */
    Copy2Bytes(pSRTE->aDestinationAddress, pDestinationAddress);

    pSRTE->relayCount = pRouteRecord->relayCount;

    FLib_MemCpy(pRouteRecordList, pRouteRecord->relayList, (pRouteRecord->relayCount << 1) );

    /* Store the list */
    pSRTE->path = pRouteRecordList;

    /* Set the age field */
    pSRTE->age = gMinutesToExpireRoute;

  }

}

/* Clears the NIB counters */
void ResetTxCounters(void)
{
  gNibTxTotal=0;
  gNibTxTotalFailures=0;
}


/*****************************************************************************
* This function reset the source route table, on ZC and ZR 
*****************************************************************************/
void ResetSourceRouteTable(void){

  /* Index to traverse throught the source route table*/
  index_t iIndex;

  /* Set to NULL to the pointer */  
  for(iIndex = 0; iIndex < gMaxSourceRouteEntries; iIndex++){
    FreeSourceRouteEntry(iIndex );
  }
}

/*****************************************************************************
* This function free or remove a specific entry in the source route table 
*****************************************************************************/
void FreeSourceRouteEntry(index_t iIndex )
{
  BeeUtilSetToF(&gaSourceRouteTable[iIndex], sizeof( sourceRouteTable_t ) - sizeof(gaSourceRouteTable[iIndex].path));

  if( gaSourceRouteTable[iIndex].path ){
    /* Free the path list */
    MSG_Free(gaSourceRouteTable[iIndex].path );

    /* Point to NULL to be reused againg */
    gaSourceRouteTable[iIndex].path = NULL;
   }
}

#ifndef gHostApp_d
/****************************************************************************/
extern void ZDO_StateMachineTimerCallBack(uint8_t timerId);
void NwkStartOrStopNwkConcentratorDiscoveryTime(bool_t startOrStop)
{
  /* if nwkConcentratorDiscoveryTime is 0x00 it means it will be maanage by the application*/
  if( !startOrStop )
  {
    TMR_StopMinuteTimer(gNwkConcentratorDiscoveryTimerID);
  }
  else
  {
    /* Otherwise, start again the timer */
    TMR_StartMinuteTimer(gNwkConcentratorDiscoveryTimerID, NlmeGetRequest(gNwkConcentratorDiscoveryTime_c), ZDO_StateMachineTimerCallBack);
  }
} 
#endif

#if ( gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d ) && gRnplusCapability_d 
/*****************************************************************************/
void StartRouteAndSourceRouteTable( void )
{
  /* Start aging route and source route table entries if and only if the gMinutesToExpireRoute is not zero */
  if(gMinutesToExpireRoute)
  {
    /* 
      This timer function require milliseconds so you need to convert gMinutesToExpireRoute in milliseconds.
      So, gMinutesToExpireRoute * 1000 milliseconds ( = 1 second ) * 60 seconds */
    TMR_StartMinuteTimer(gExpireRouteEntriesTimerID, 1, CustomAgingRouteAndSourceTableTimeOutCallBack);
  }
}
/*****************************************************************************/
void AgingRouteAndSourceTableEntries( void )
{
  /* Index of the route table entry */  
  index_t iIndex = 0;

  /* Points to route table entry */
  routeTable_t *pRTE = NULL;

#if gConcentratorFlag_d && gNwkHighRamConcentrator_d
  /* Points to source route table entry */
  sourceRouteTable_t *pSRTE = NULL;
#endif
  /* Used to notify which device was expired */
  zbNwkAddr_t aDstNwkAddress;

  /* Used to notify or not to app that one entry has expired */
  bool_t NotifyToApp = FALSE;

  /* Indicate the expiration type */
  zbStatus_t status;

  /* ----------------- Aging Route Table ----------------- */
  for( iIndex = 0; iIndex < gMaxRouteTableEntries; iIndex++ ){

    pRTE = &gaRouteTable[iIndex];

    /* Only valid entries can be aging */
    if(pRTE && !IsEqual2Bytes(pRTE->aDestinationAddress, gaBroadcastAddress) && pRTE->properties.status != gInactive_c)
    {
      /* aging the route */
      if( pRTE->age )
      {
        pRTE->age--;
      }

      if( !pRTE->age )
      {
        /* Notify to App */
        NotifyToApp = TRUE;

        status = gZbRouteExpided_c;

        Copy2Bytes(aDstNwkAddress, pRTE->aDestinationAddress );
        
        /* Remove the source route entry */
        RemoveRouteEntry( pRTE->aDestinationAddress );
      }
    }
  }/* for.. route entries */

  /* ----------------- Aging Source Route Table ----------------- */

#if gConcentratorFlag_d && gNwkHighRamConcentrator_d
  /* 
    Particulary to aging the source route entries must be when the device is a nwk concentrator and when it has the
    high ram enabled.
  */
  for( iIndex = 0; iIndex < gMaxSourceRouteEntries; iIndex++ ){

    pSRTE = &gaSourceRouteTable[iIndex];

    /* Only valid entries can be aging */
    if( pSRTE ) 
    {
      if( !IsBroadcastAddress(pSRTE->aDestinationAddress) && pSRTE->age )
      {
        pSRTE->age--;
      }

      if( !pSRTE->age ){
  
        /* Notify to App */
        NotifyToApp = TRUE;

        status = gZbSourceRouteExpided_c;

        Copy2Bytes(aDstNwkAddress, pSRTE->aDestinationAddress);
      
        /* Remove the source route entry */
        FreeSourceRouteEntry( (uint8_t)(pSRTE - gaSourceRouteTable));
      }
    }
  }/* for.. route entries */
  
#endif

  if( NotifyToApp )
  {
    GenerateNlmeNwkStatusIndication(status, aDstNwkAddress);
  }
}
/****************************************************************************/
void CustomAgingRouteAndSourceTableTimeOutCallBack
(
  tmrTimerID_t timerId  /* IN: */
)
{
  (void)timerId;  /* to prevent compiler warnings */

  /* Aging both route and source table entries */
  AgingRouteAndSourceTableEntries();

  TMR_StartMinuteTimer(gExpireRouteEntriesTimerID, 1, CustomAgingRouteAndSourceTableTimeOutCallBack);
}

/******************************************************************************/
index_t ExpireAndGetEntryInRouteTable(void)
{
  /* Actual index */
  index_t iIndex;

  /* index of the field to compare */
  index_t iIndexCmp;

  /* index of the field to compare */
  index_t iIndexEntryExpired = gNwkEntryNotFound_c;

  /* First search for expired entry and if none is found then search the minor age */
  for( iIndex = 0; iIndex < gMaxRouteTableEntries; iIndex++ )
  { 
    if( !gaRouteTable[iIndex].age )
      return iIndex;
    
    for( iIndexCmp = 0; iIndexCmp < gMaxRouteTableEntries; iIndexCmp++ )
    {
      if( gaRouteTable[iIndex].age < gaRouteTable[iIndexCmp].age)
      {
        iIndexEntryExpired = iIndex;

      }
    }
  }
  if (iIndexEntryExpired != gNwkEntryNotFound_c)
  {
    // clean up any packets pending for this route to be resolved
    CleanUpPacketsOnHoldTable(gaRouteTable[iIndexEntryExpired].aDestinationAddress);
    // set route as inactive and return index.
    gaRouteTable[iIndexEntryExpired].properties.status = gInactive_c;  
  }
  return iIndexEntryExpired;
}
#endif ( gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d ) && gRnplusCapability_d

#ifndef gHostApp_d
/******************************************************************************
* This function starts the aging timer

* 
* Interface assumptions:
*   NONE.
*
* Return value:
*   NONE.
*******************************************************************************/
void StartCustomAgingNeighborTableTimer(void)
{
    TMR_StartMinuteTimer(gAgingNTEntriesTimerID, 1 , CustomAgingNeighborTableTimeOutCallBack); 
}
/******************************************************************************
* This function gets controll once the aging timeout is triggered, and ages
* the ticks on the NT once per each minute defined on the aging scale.
* 
* Interface assumptions:
*   NONE.
*
* Return value:
*   NONE.
*******************************************************************************/
void CustomAgingNeighborTableTimeOutCallBack
(
  tmrTimerID_t timerId  /* IN: The Id of the timer to be expired, may be needed or not.*/
)
{
  /* The timer Id is not needed, this function is linked to only one timer. */
  (void)timerId;  /* to prevent compiler warnings */

  /* Handle the resoltuin minutes. */
  if (gNwkAgingTickScale)
    --gNwkAgingTickScale;

  /* On Zero value we will eage by one, each time the resolutions minutes reach zero. */
  if (!gNwkAgingTickScale)
  {
    gNwkAgingTickScale = gNwkAgingTickScale_c;
    /* Age by one tick each ZED on NT; also includes ZR for SP1, see inside the function.. */
#if !gZigbeeProIncluded_d
    AgingNeighborTable( gRouter_c | gEndDevice_c );
#else
    AgingNeighborTable( gEndDevice_c );
#endif
  }

  /* As the timer is a single shot we need to start again*/  
  StartCustomAgingNeighborTableTimer();
}
#endif
/******************************************************************************
* This function sets back to default the scale value, and the function is
* needed, on the library, nwk layer  portion to reset this value whe Nwk gets
* restarted. Remember library code can not use define macros from the
* application portion.
*
* Interface assumptions:
*   NONE.
*
* Return value:
*   NONE.
*******************************************************************************/
void Nwk_ResetAgingTickScale(void)
{
  gNwkAgingTickScale = gNwkAgingTickScale_c;
}

/*****************************************************************************
* This function must return false, it is implemented for future enchancements
*****************************************************************************/
bool_t SecurityTypeCheck( zbIeeeAddr_t aIeeeAddress )
{
  (void) aIeeeAddress;
  return FALSE;
}
/*****************************************************************************
* This function returns TRUE if a SE packet that doesn't belong to the Key
* Establishment cluester is encrypted with the preconfigured TC link key.
*****************************************************************************/
bool_t SeSecurityCheck
(
  uint8_t   frameControl,
  uint8_t   *pProfileId,
  uint8_t   *pKey
)
{
#if gZclSeSecurityCheck_d
  uint8_t profile[2] = {gSEProfIdb_c};
  uint8_t key[16] = {mDefaultValueOfTrustCenterLinkKey_c};
  //Check if the packet is a aps data message or aps ack.
  if(((frameControl & gApsFrameControlTypeMask)==gApsFrameControlTypeData) || ((frameControl & gApsFrameControlTypeMask) == gApsFrameControlTypeAck))
  {
    if(IsEqual2Bytes(pProfileId, &profile))
    {
      if(FLib_MemCmp(pKey, &key, sizeof(zbAESKey_t)))
      {
        return TRUE;
      }
    }
  }
#else
  (void)frameControl;
  (void)pProfileId;
  (void)pKey;
#endif
  return FALSE;
}
/*****************************************************************************/
#if gStochasticAddressingCapability_d
/*
  This function generates an stochastic short address for the device
  Params: 
  uint8_t*  pShortAddrGenerated: Variable where this function stores the short address.
*/
extern neighborTable_t* IsInNeighborTable(zbAddrMode_t addressMode, uint8_t *pAddress);
extern index_t IsInRouteTable(uint8_t * pDestinationAddressToCheck, zbAddrMode_t addrMode);
void NWK_ADDRESSASSIGN_GenerateAddressStochasticMode
(
  uint8_t* pShortAddrGenerated
)
{
  uint16_t retries = 0;
  uint16_t StochasticAddrGenerated16 = 0;

  /* used to check if the originator is one of our children */
  neighborTable_t *pNTE;

  /*
    This loop try of generates a random address for child device. 
    It retries giRetriesStochasticAddrLocalDevive.
  */
  do
  {
    /* Generate the short address */
    StochasticAddrGenerated16 = (((uint16_t)GetRandomNumber()) & 0xfff7);

    /*
      if the Stochastic address genereted is in the valid range 0x0001 and 0xFFF7 then proccess it.
    */
    if (StochasticAddrGenerated16)
    {
      /* store the address */
      Set2Bytes(pShortAddrGenerated, StochasticAddrGenerated16); 

      /*
        If I am the parent (Coordinator or Router), then I have to search the
        stochastic address generated in all NIB -neighbor and route- tables entries
        to can assign it to the device.
      */

      /* Check the Neighbor and Route table */
      pNTE = IsInNeighborTable(gZbAddrMode16Bit_c, pShortAddrGenerated);

      /*
        If the address generated doesn't exists in neighbor or route or both talbes or
        if the stochastic address generated is not my address then it must be generated again.
      */
      if ((!pNTE) 
#if gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d
#if gRnplusCapability_d
       && (IsInRouteTable(pShortAddrGenerated, gZbAddrMode16Bit_c) == gNwkEntryNotFound_c) 
#endif
#endif
      && (!IsEqual2Bytes(pShortAddrGenerated, NlmeGetRequest(gNwkShortAddress_c))) )
      {
        /* Finish the loop */
        retries = gDefaultRetriesStochasticAddrLocalDevice;
      }

      /* make it invalid */
      else
      {
        Copy2Bytes(pShortAddrGenerated, gaBroadcastAddress);
      }

      /* Increment the number of retries.*/
      ++retries;
    }

  }while(retries < gDefaultRetriesStochasticAddrLocalDevice);

}
#endif

#if gStandardSecurity_d
/******************************************************************************
* This funcion return the size of IEEE addresses contained in the nwk header.
*
* Interface assumptions: pNpdu is not null.
* 
* Return value: uint8_t - size of the IEEE addresses     
*   
* Effects on global data: NONE
* 
******************************************************************************/
extern uint8_t NwkReturnIeeeAddrLen(npduFrame_t *pNpdu);

/******************************************************************************
* This function is only called for all the packets with security off on a
* secure network, Depends on a global to allow packets, the relationship of
* the sending device and out own state, Data packet must be encrypted unless
* we allow for unsecure packet or come from a node getting authenticated or
* to us when we are in the way to be authenticated.
*
* Interface assumption:
*   The parameters pNTE and pNpduare not null pointers.
*
* Return vlue:
*   Success if the packet will be allow to reach the next higher layer.
*   Failure if the packet must be freed and will not reach the next higher layer.
*
*******************************************************************************/
zbStatus_t SSP_NwkVerifyUnsecurePacket
(
  neighborTable_t *pNTE,  /* IN: Entry on the NT where the node's info is stored, NULL if unknown. */
  npduFrame_t *pNpdu      /* IN: The payload of the incomming indication. */
)
{
  uint8_t *pCommandFrame;  /* Direct access to the command Id, if the packet is a Network command. */

  /* If we allow unsecure packets then let it go. */
  if (gAllowNonSecurePacketsInSecureMode)
    return gZbSuccess_c;  /* Success means let the packet go to the next higher layer. */

  /* If we dont know the device must some other type of packet. */
  if (pNTE)
  {
    /*
      Allow devices that aren't on our NT, just becuase thye may be ZED, from some other Depth.
      Or ZR from far way, and this may be the first time we hear then, now the next conditions
      may fial if this is not one of the allow packets.
    */
    /* If the sender is a device trying tobe authenticated,let it pass. */
    if (pNTE->deviceProperty.bits.relationship == gUnAuthenticatedChild_c)
      return gZbSuccess_c;  /* Success means let the packet go to the next higher layer. */
  }

  /* If we (local node receiving a packet) are not authenticated yet, we need to receive everything. */
  if (!ZDO_IsRunningState())
    return gZbSuccess_c;  /* Success means let the packet go to the next higher layer. */

  /*
    If it is a data frame (Not equal to command) and we don't allow unsecure packets then
    stop processing.
  */
  if (!IsACommandFrame(pNpdu))
  {
    /* This will return the security failure and will free the message. */
    return gZbSecurityFail_c;  /* Security fail means stop processing the packet and free the buffer. */
  }

  /* Get the command Id to be able to know if ti is one of the allow packets. */
  pCommandFrame = ((uint8_t *)pNpdu->pNwkPayload + NwkReturnIeeeAddrLen(pNpdu));

  /* Ok we have a command so far we just allow two command to be unsecure. Leave and rejoin (req and rsp). */
  if (pCommandFrame[0] != gRejoinRequest_c)
  {
    /* Invalid command return failure */
    return gZbSecurityFail_c;  /* Security fail means stop processing the packet and free the buffer. */
  }

  /* We mande it everyhitng is ok.. */
  return gZbSuccess_c;  /* Success means let the packet go to the next higher layer. */
}
#endif

 /******************************************************************************
 Set an IB attribute external(using UART, I2C etc)
*******************************************************************************/
void SetIBReqExternal(
            uint8_t iBReqType,   
						uint8_t attrId,
						uint8_t index,
						uint8_t *pValue
						) 
{
#if defined(gHostApp_d)
  if(iBReqType == gApsmeReqType_c)
  {
  if((attrId == gApsChannelMask_c) ||
     (attrId == gApsAddressMap_c)  ||
     (attrId == gApsMaxWindowSize_c)  ||
     (attrId == gApsInterframeDelay_c)  ||
     (attrId == gApsMaxFragmentLength_c) 
    )
      ZtcApsmeSetRequest(attrId, index, pValue);
  }
  else if(iBReqType == gNLMEReqType_c)
  {
    if((attrId == gNwkPanId_c) ||
     (attrId == gNwkShortAddress_c)  ||
     (attrId == gNwkExtendedPanId_c)  ||
     (attrId == gNwkLogicalChannel_c)  ||
     (attrId == gNwkIndirectPollRate_c) 
    )
     ZtcNlmeSetRequest(attrId, index, pValue);
  } 
#elif defined(gBlackBox_d)
  if(iBReqType == gApsmeReqType_c)
  {
  if((attrId == gApsChannelMask_c) ||
     (attrId == gApsAddressMap_c) 
    )
      ZtcApsmeSetRequest(attrId, index, pValue);
  }
  else if(iBReqType == gNLMEReqType_c)
  {
    if((attrId == gNwkPanId_c) ||
     (attrId == gNwkShortAddress_c)  ||
     (attrId == gNwkExtendedPanId_c)  ||
     (attrId == gNwkLogicalChannel_c) 
    )
     ZtcNlmeSetRequest(attrId, index, pValue);
  } 
 
#else
  (void)iBReqType; 
  (void)attrId;
  (void)index;
  (void)pValue;
#endif 
}
/******************************************************************************
 Init the Address Map Table (fill it with 0xFF).
*******************************************************************************/
void AddrMap_InitTable(void)
{
  /* Clear address map */
  BeeUtilLargeMemSet(gaApsAddressMap, 0xFF, 
                        (uint16_t)gApsMaxAddressMapEntries * sizeof(gaApsAddressMap[0]));	  
}

/******************************************************************************
 Get an entry from the Address Map Table.
*******************************************************************************/
uint8_t AddrMap_GetTableEntry(uint8_t index, void *pEntry)
{
 if((index >= gApsMaxAddressMapEntries) || !pEntry) 
  return FALSE;
 
 FLib_MemCpy(pEntry, &gpAddressMapTable[index], sizeof(zbAddressMap_t));
 return TRUE;
}

/******************************************************************************
  Search IEEE address OR short address in the Address Map Table.
*******************************************************************************/
uint8_t AddrMap_SearchTableEntry(zbIeeeAddr_t   *pIeeeAddr,
                                 zbNwkAddr_t    *pNwkAddr, 
                                 void           *pEntry)
{
  uint8_t i; 

  if(!pEntry || (!pIeeeAddr && !pNwkAddr)) 
    return 0xff;
      
  for(i = 0; i < gApsMaxAddressMapEntries; i++)
  {
    
    if( (pIeeeAddr && IsEqual8Bytes(gpAddressMapTable[i].aIeeeAddr, (uint8_t*)pIeeeAddr)) ||
        (pNwkAddr && IsEqual2Bytes(gpAddressMapTable[i].aNwkAddr, (uint8_t*)pNwkAddr)) ) 
    {
      FLib_MemCpy(pEntry, &gpAddressMapTable[i], sizeof(zbAddressMap_t));
      return i;
    } 
  }
  return 0xff;
}

 /******************************************************************************
 Set an entry in the Address Map Table.
*******************************************************************************/
uint8_t AddrMap_SetTableEntry(uint8_t index, void *pEntry)
{
  if(index >= gApsMaxAddressMapEntries) 
    return FALSE; 
  FLib_MemCpy(&gpAddressMapTable[index], pEntry, sizeof(zbAddressMap_t));
  return TRUE;
}

 /******************************************************************************
 Remove an entry from the Address Map Table.
*******************************************************************************/
uint8_t AddrMap_RemoveEntry(uint8_t index)
{
  if(index >= gApsMaxAddressMapEntries) 
    return FALSE; 
  
  BeeUtilZeroMemory(&gpAddressMapTable[index], sizeof(zbAddressMap_t));
  return TRUE;
}
 

 /******************************************************************************
 Init the Device Key Pair Set table (fill it with zeros).
*******************************************************************************/
void DevKeyPairSet_InitTable(void)
{
  uint8_t i;
  /* Clear gaApsDeviceKeyPairSet */
  BeeUtilLargeZeroMemory(gaApsDeviceKeyPairSet, 
                        (uint16_t)giApsDeviceKeyPairCount * sizeof(gaApsDeviceKeyPairSet[0]));	 
    /* Initialize each entry of the secure material. */
  for (i = 0; i < ApsmeGetRequest(gApsDeviceKeyPairCount_c); i++)
  {
    /* Device that are not registered yet.!! */
    gaApsDeviceKeyPairSet[i].iDeviceAddress = gNotInAddressMap_c;

    /* Set the default TC key. */
    gaApsDeviceKeyPairSet[i].keyType = ApsmeGetRequest(gApsDefaultTCKeyType_c);
  } 
}

 /******************************************************************************
 Get an entry from the Device Key Pair Set table.
*******************************************************************************/
uint8_t DevKeyPairSet_GetTableEntry(uint8_t index, void *pEntry)
{
 if((index >= giApsDeviceKeyPairCount) || !pEntry) 
  return FALSE;
 
 FLib_MemCpy(pEntry, &gaApsDeviceKeyPairSet[index], sizeof(zbApsDeviceKeyPairSet_t));
 return TRUE;
}


/******************************************************************************
 Set an entry in the evice Key Pair Set table.
*******************************************************************************/
uint8_t DevKeyPairSet_SetTableEntry(uint8_t index, void *pEntry)
{
  if(index >= giApsDeviceKeyPairCount) 
    return FALSE; 
  FLib_MemCpy(&gaApsDeviceKeyPairSet[index], pEntry, sizeof(zbApsDeviceKeyPairSet_t));
  return TRUE;
}

 /******************************************************************************
 Remove an entry from the Device Key Pair Set table.
*******************************************************************************/
uint8_t DevKeyPairSet_RemoveEntry(uint8_t index)
{
  if(index >= giApsDeviceKeyPairCount) 
    return FALSE; 
  
  BeeUtilZeroMemory(&gaApsDeviceKeyPairSet[index], sizeof(zbApsDeviceKeyPairSet_t));
  return TRUE;
}
 

/******************************************************************************
 Init the Key Set table (fill it with zeros).
*******************************************************************************/
void KeySet_InitTable(void)
{
  /* Clear gaApsKeySet */
  BeeUtilLargeZeroMemory(gaApsKeySet, 
                        (uint16_t)gApsKeySetCount * sizeof(gaApsKeySet[0]));	  
}

 /******************************************************************************
 Get an entry(a key) from the Key Set table.
*******************************************************************************/
uint8_t KeySet_GetTableEntry(uint8_t index, void *pKey)
{
 if((index >= gApsKeySetCount) || !pKey) 
  return FALSE;
 
 FLib_MemCpy(pKey, &gaApsKeySet[index], sizeof(zbAESKey_t));
 return TRUE;
}


 /******************************************************************************
 Set an entry (a key) in the Key Set table.
*******************************************************************************/
uint8_t KeySet_SetTableEntry(uint8_t index, void *pKey)
{
  if((index >= gApsKeySetCount) || !pKey) 
    return FALSE; 
  FLib_MemCpy(&gaApsKeySet[index], pKey, sizeof(zbAESKey_t));
  return TRUE;
}

 /******************************************************************************
 Remove an entry (a key) from the Key Set table.
*******************************************************************************/
uint8_t KeySet_RemoveEntry(uint8_t index)
{
  if(index >= gApsKeySetCount) 
    return FALSE; 
  
  BeeUtilZeroMemory(&gaApsKeySet[index], sizeof(zbAESKey_t));
  return TRUE;
}
 
 /******************************************************************************
  Search a key in the Key Set table.
*******************************************************************************/
uint8_t KeySet_SearchTableEntry(zbAESKey_t *pKey)
{
  uint8_t i; 
  
  if(!pKey)
    return 0xff;
  for(i = 0; i < gApsKeySetCount; i++)
  {
    if(FLib_MemCmp(gaApsKeySet[i], pKey, 16))
      return i;
  }
  return 0xff;
}
 
 
 /******************************************************************************
*******************************************************************************/

/******************************************************************************
*******************************************************************************
* Private Functions
*******************************************************************************
******************************************************************************/
/*None*/
/******************************************************************************
*******************************************************************************
* Private Debug Stuff
*******************************************************************************
******************************************************************************/
