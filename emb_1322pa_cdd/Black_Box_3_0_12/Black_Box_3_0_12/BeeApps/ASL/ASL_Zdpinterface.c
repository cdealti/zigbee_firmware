/*****************************************************************************
* ZigBee Application.
*
* (c) Copyright 2005, Freescale, Inc. All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
*****************************************************************************/
#include "EmbeddedTypes.h"
#include "ZigBee.h"
#include "Beecommon.h"
#include "BeeStack_Globals.h"
#include "BeeStackConfiguration.h"
#include "AppZdoInterface.h"
#include "ZtcInterface.h"
#if gBeeStackIncluded_d
#include "ZdoApsInterface.h"
#include "ZdoMain.h"
#endif
#include "ZDOStateMachineHandler.h"
#include "ZdpManager.h"
#include "EndPointConfig.h"
#include "ZclOptions.h"
#include "BeeAppInit.h"
#include "ASL_ZdpInterface.h"
#include "ASL_UserInterface.h"
#ifdef gHostApp_d
#include "ZtcHandler.h"
#endif
#if (gZclEnableOTAClient_d)
#include "ZclOta.h"
#endif
/******************************************************************************
*******************************************************************************
* Private macros
*******************************************************************************
******************************************************************************/

/******************************************************************************
*******************************************************************************
* Public prototypes
*******************************************************************************
******************************************************************************/

/******************************************************************************
*******************************************************************************
* Private prototypes
*******************************************************************************
******************************************************************************/

/******************************************************************************
*******************************************************************************
* Private type definitions
*******************************************************************************
******************************************************************************/
/* None */
/******************************************************************************
*******************************************************************************
* Private memory declarations
*******************************************************************************
******************************************************************************/

/******************************************************************************
*******************************************************************************
* Public memory declarations
*******************************************************************************
******************************************************************************/
#if( gEndDevCapability_d || gComboDeviceCapability_d)
//zbCounter_t gFailureCounter = 0;
#endif

extern ZDPCallBack_t gpZdpAppCallBackPtr;
extern zbCounter_t gZdpSequence;
extern zbCounter_t gZdpResponseCounter;

/******************************************************************************
*******************************************************************************
* Public functions
*******************************************************************************
******************************************************************************/

#if gBeeStackIncluded_d
/*------------------- ZDP_APP_SapHandler --------------------
	Handles messages coming from ZDP to the Application

	IN: pMsg Message from ZDP to app
*/
uint8_t ZDP_APP_SapHandler(zdpToAppMessage_t *pMsg) {
#ifndef gHostApp_d
  /* For Host application , all received SAP messages are forward 
     to Host(if Host uart communication is enabled) by ZTC */
  ZTC_TaskEventMonitor( gZDPAppSAPHandlerId_c, ( uint8_t* ) pMsg,gZbSuccess_c );
#endif
  /* discover confirm complete, free the descriptor */
  if (pMsg->msgType == gNlmeNetworkDiscoveryConfirm_c)
  {
    if (pMsg->msgData.networkDiscoveryConf.pNetworkDescriptor != NULL)
    {
      MSG_Free( pMsg->msgData.networkDiscoveryConf.pNetworkDescriptor );
    }
  }
#ifndef gHostApp_d
  /* ZEDs must handle the poll error confirm, if reaches  */
#if gEndDevCapability_d || gComboDeviceCapability_d
#if gComboDeviceCapability_d
  if(NlmeGetRequest(gDevType_c) == gEndDevice_c)
#endif
  {
    if (pMsg->msgType == gSyncConf_c)
    {
      /*
        Due to ZEDs only talks or communicates through the parent, we concider each failing
        Tx as a failure to communicate with the parent, so we reuse the counter for FA as
        a code saving, because ZED do not participate in FA procedures directly. After an
        specific limit, the parent is considered as lost, and the zed must do a Nwk Rejoin.
      */
      if(pMsg->msgData.syncConf.status != gZbSuccess_c)
      {
        IncrementNwkTxTotalFailures();
        if (NlmeGetRequest(gNwkTxTotalFailures_c) >= NlmeGetRequest(gNwkLinkRetryThreshold_c))
        {

          if (ZDO_IsRunningState())
          {
            /* For Tx failures Rejoin must be done with the RAM info.  */
            ZDO_ProcessDeviceNwkRejoin();

            /* Save on NVM that we are orphans */
          }

          /* Reset the counter. */
          ResetTxCounters();
        }
      } 
      else
      {
        ResetTxCounters();
      }
    }
  }/* Combo device */
#endif( gEndDevCapability_d  || gComboDeviceCapability_d )
#endif /* gHostApp_d */

  /* Send the message up to the application */
  if( gpZdpAppCallBackPtr )
  {
#ifdef gHostApp_d
/* This allowed to send messages over the Uart */
  extern bool_t mMsgOriginIsZtc;
  mMsgOriginIsZtc = FALSE;
#endif  
    gpZdpAppCallBackPtr( pMsg, gZdpResponseCounter ); 
  }

  /* application will not see this message */
  else
  {
    /*
      If the call back is not registed and the message is a Energy scan confirm we
      should free the internal list.
    */
    if (pMsg->msgType == gNlmeEnergyScanConfirm_c)
      MSG_Free(pMsg->msgData.energyScanConf.resList.pEnergyDetectList);

    /*
      WARNING: This line must not be removed.
      Free the message received form the lower layers.
    */
    MSG_Free( pMsg );
  }

  return gZbSuccess_c;
}
#endif


/*==============================================================================================
	======================== Device and Service Discovery Client Services ========================
	==============================================================================================*/

/*-------------------- ASL_NWK_addr_req --------------------
	ClusterID=0x0000.

	This functions genertes a ZDP NWK_addr_req, and pass it to the
	ZDO layer thru the APP_ZDP_SapHandler function.

	Is generated from a Local Device wishing to inquire as to the
	16 bit address of the Remote Device based on its known IEEE address. The
	destination addressing on this command shall be broadcast to all RxOnWhenIdle
	devices.

	NOTE: The memory allocated for the message will be freed byt he lower layers.
*/
void ASL_NWK_addr_req
(
	zbCounter_t  *pSequenceNumber,  /* IN: The sequence number used to send the request. */
	zbNwkAddr_t  aDestAddress,      /* IN: The destination address where to send the request. */
	zbIeeeAddr_t  aIeeeAddr,        /* IN: The IEEE address to be matched by the Remote Device */
	uint8_t  requestType,           /* IN: Request type for this command: 0x00 – Single device response
																																				0x01 – Extended response
																																				0x02-0xFF – reserved */
	index_t  startIndex             /* IN: If the Request type for this command is Extended response,
																				the StartIndex provides the starting index for the requested
																				elements of the associated devices list */
)
{
#if gNWK_addr_req_d
	appToZdpMessage_t *pAppZdoMsg;
	pAppZdoMsg = MSG_Alloc( MbrOfs(appToZdpMessage_t, msgData) + sizeof(zbNwkAddrRequest_t));

	if (pAppZdoMsg == NULL)
		return;

	if (pSequenceNumber != NULL)
		pSequenceNumber[0] = gZdpSequence;

	pAppZdoMsg->msgType = gNWK_addr_req_c;
	Copy2Bytes(pAppZdoMsg->aDestAddr, aDestAddress);
	Copy8Bytes(pAppZdoMsg->msgData.nwkAddressReq.aIeeeAddr, aIeeeAddr);
	pAppZdoMsg->msgData.nwkAddressReq.requestType = requestType;
	pAppZdoMsg->msgData.nwkAddressReq.startIndex = startIndex;

	if(APP_ZDP_SapHandler ( pAppZdoMsg ))
	{
		/* Insert code here to handle error  */
	}
#else
(void)pSequenceNumber;
(void)aDestAddress;
(void)aIeeeAddr;
(void)requestType;
(void)startIndex;
#endif
}


#if gIEEE_addr_req_d
void ASL_IEEE_addr_req
(
	zbCounter_t  *pSequenceNumber,
	zbNwkAddr_t  aDestAddress,
	zbNwkAddr_t aNwkAddrOfInterest,
	uint8_t  requestType,
	index_t  startIndex
)
{
	appToZdpMessage_t *pAppZdoMsg;
	pAppZdoMsg = MSG_Alloc( MbrOfs(appToZdpMessage_t, msgData) + sizeof(zbIeeeAddrRequest_t));

	if (pAppZdoMsg == NULL)
		return;

	if (pSequenceNumber != NULL)
		pSequenceNumber[0] = gZdpSequence;

	pAppZdoMsg->msgType = gIEEE_addr_req_c;
	Copy2Bytes(pAppZdoMsg->aDestAddr, aDestAddress);
	Copy2Bytes(pAppZdoMsg->msgData.extAddressReq.aNwkAddrOfInterest, aNwkAddrOfInterest);
	pAppZdoMsg->msgData.extAddressReq.requestType = requestType;
	pAppZdoMsg->msgData.extAddressReq.startIndex = startIndex;

	if(APP_ZDP_SapHandler ( pAppZdoMsg ))
	{
		/* Insert code here to handle error  */
	}
}
#endif

#if gNode_Desc_req_d
void ASL_Node_Desc_req
(
	zbCounter_t  *pSequenceNumber,
	zbNwkAddr_t  aDestAddress
)
{
	appToZdpMessage_t *pAppZdoMsg;
	pAppZdoMsg = MSG_Alloc( MbrOfs(appToZdpMessage_t, msgData) + sizeof(zbNodeDescriptorRequest_t));

	if (pAppZdoMsg == NULL)
		return;

	if (pSequenceNumber != NULL)
		pSequenceNumber[0] = gZdpSequence;

	pAppZdoMsg->msgType = gNode_Desc_req_c;
	Copy2Bytes(pAppZdoMsg->aDestAddr, aDestAddress);
	Copy2Bytes(pAppZdoMsg->msgData.nodeDescriptorReq.aNwkAddrOfInterest, aDestAddress);

	if(APP_ZDP_SapHandler ( pAppZdoMsg ))
	{
		/* Insert code here to handle error  */
	}
}
#endif

#if gPower_Desc_req_d
void ASL_Power_Desc_req
(
	zbCounter_t  *pSequenceNumber,
	zbNwkAddr_t  aDestAddress
)
{
	appToZdpMessage_t *pAppZdoMsg;
	pAppZdoMsg = MSG_Alloc( MbrOfs(appToZdpMessage_t, msgData) + sizeof(zbPowerDescriptorRequest_t));

	if (pAppZdoMsg == NULL)
		return;

	if (pSequenceNumber != NULL)
		pSequenceNumber[0] = gZdpSequence;

	pAppZdoMsg->msgType = gPower_Desc_req_c;
	Copy2Bytes(pAppZdoMsg->aDestAddr, aDestAddress);
	Copy2Bytes(pAppZdoMsg->msgData.powDescriptorReq.aNwkAddrOfInterest, aDestAddress);

	if(APP_ZDP_SapHandler ( pAppZdoMsg ))
	{
		/* Insert code here to handle error  */
	}
}
#endif

#if gExtended_Simple_Desc_req_d
void ASL_Extended_Simple_Desc_req
(
	zbCounter_t  *pSequenceNumber,
	zbNwkAddr_t  aDestAddress,
	zbEndPoint_t  endPoint,
	index_t startIndex
)
{
	appToZdpMessage_t *pAppZdoMsg;
	pAppZdoMsg = MSG_Alloc( MbrOfs(appToZdpMessage_t, msgData) + sizeof(zbExtSimpleDescriptorRequest_t));

	if (pAppZdoMsg == NULL)
		return;

	if (pSequenceNumber != NULL)
		pSequenceNumber[0] = gZdpSequence;

	pAppZdoMsg->msgType = gExtended_Simple_Desc_req_c;
	Copy2Bytes(pAppZdoMsg->aDestAddr, aDestAddress);
	Copy2Bytes(pAppZdoMsg->msgData.extendedSimpleDescriptorReq.aNwkAddrOfInterest, aDestAddress);
	pAppZdoMsg->msgData.extendedSimpleDescriptorReq.endPoint = endPoint;
	pAppZdoMsg->msgData.extendedSimpleDescriptorReq.startIndex = startIndex;

	if(APP_ZDP_SapHandler ( pAppZdoMsg ))
	{
		/* Insert code here to handle error  */
	}
}
#endif

#if gSimple_Desc_req_d
void ASL_Simple_Desc_req
(
	zbCounter_t  *pSequenceNumber,
	zbNwkAddr_t  aDestAddress,
	zbEndPoint_t  endPoint
)
{
	appToZdpMessage_t *pAppZdoMsg;
	pAppZdoMsg = MSG_Alloc( MbrOfs(appToZdpMessage_t, msgData) + sizeof(zbSimpleDescriptorRequest_t));

	if (pAppZdoMsg == NULL)
		return;

	if (pSequenceNumber != NULL)
		pSequenceNumber[0] = gZdpSequence;

	pAppZdoMsg->msgType = gSimple_Desc_req_c;
	Copy2Bytes(pAppZdoMsg->aDestAddr, aDestAddress);
	Copy2Bytes(pAppZdoMsg->msgData.simpleDescriptorReq.aNwkAddrOfInterest, aDestAddress);
	pAppZdoMsg->msgData.simpleDescriptorReq.endPoint = endPoint;

	if(APP_ZDP_SapHandler ( pAppZdoMsg ))
	{
		/* Insert code here to handle error  */
	}
}
#endif

#if gActive_EP_req_d
void ASL_Active_EP_req
(
	zbCounter_t  *pSequenceNumber,
	zbNwkAddr_t  aDestAddress
)
{
	appToZdpMessage_t *pAppZdoMsg;
	pAppZdoMsg = MSG_Alloc( MbrOfs(appToZdpMessage_t, msgData) + sizeof(zbActiveEpRequest_t));

	if (pAppZdoMsg == NULL)
		return;

	if (pSequenceNumber != NULL)
		pSequenceNumber[0] = gZdpSequence;

	pAppZdoMsg->msgType = gActive_EP_req_c;
	Copy2Bytes(pAppZdoMsg->aDestAddr, aDestAddress);
	Copy2Bytes(pAppZdoMsg->msgData.activeEpReq.aNwkAddrOfInterest, aDestAddress);

	if(APP_ZDP_SapHandler ( pAppZdoMsg ))
	{
		/* Insert code here to handle error  */
	}
}
#endif

#if gExtended_Active_EP_req_d
void ASL_Extended_Active_EP_req
(
	zbCounter_t  *pSequenceNumber,
	zbNwkAddr_t  aDestAddress,
	index_t startIndex
)
{
	appToZdpMessage_t *pAppZdoMsg;
	pAppZdoMsg = MSG_Alloc( MbrOfs(appToZdpMessage_t, msgData) + sizeof(zbExtActiveEpRequest_t));

	if (pAppZdoMsg == NULL)
		return;

	if (pSequenceNumber != NULL)
		pSequenceNumber[0] = gZdpSequence;

	pAppZdoMsg->msgType = gExtended_Active_EP_req_c;
	Copy2Bytes(pAppZdoMsg->aDestAddr, aDestAddress);
	Copy2Bytes(pAppZdoMsg->msgData.extendedActiveEpReq.aNwkAddrOfInterest, aDestAddress);
  pAppZdoMsg->msgData.extendedActiveEpReq.startIndex = startIndex;
  
	if(APP_ZDP_SapHandler ( pAppZdoMsg ))
	{
		/* Insert code here to handle error  */
	}
}
#endif

#if gMatch_Desc_req_d
void ASL_MatchDescriptor_req
(
  zbCounter_t  *pSequenceNumber,
  zbNwkAddr_t  aDestAddress,
  zbSimpleDescriptor_t  *pSimpleDescriptor
)
{
  appToZdpMessage_t  *pAppZdoMsg;
  zbSize_t iSizeOfSimpleDescriptor;

  /* allocate a message for building the request */
  pAppZdoMsg = AF_MsgAlloc();
  if (!pAppZdoMsg)
    return;

  if (pSequenceNumber)
    pSequenceNumber[0] = gZdpSequence;

  pAppZdoMsg->msgType = gMatch_Desc_req_c;
  Copy2Bytes(pAppZdoMsg->aDestAddr, aDestAddress);

  Copy2Bytes(pAppZdoMsg->msgData.matchDescriptorRequest.aNwkAddrOfInterest,aDestAddress);
  Copy2Bytes(pAppZdoMsg->msgData.matchDescriptorRequest.aProfileId, pSimpleDescriptor->aAppProfId);

  /* The size of both counters ... */
  iSizeOfSimpleDescriptor = sizeof(pSimpleDescriptor->appNumInClusters) * 2;
  iSizeOfSimpleDescriptor = (pSimpleDescriptor->appNumInClusters * sizeof(zbClusterId_t)) +  /* The amount of clusters in the input list. */
                            (pSimpleDescriptor->appNumOutClusters * sizeof(zbClusterId_t)) + /* The amnount of clusters in the output list. */
                            (sizeof(pSimpleDescriptor->pAppInClusterList) * 2);               /* The sise of both pionters, input list pointer and out put list poiunter. */
  /* Get all the info into the packet.! */
  FLib_MemCpy(&pAppZdoMsg->msgData.matchDescriptorRequest.cNumClusters, &pSimpleDescriptor->appNumInClusters, iSizeOfSimpleDescriptor);

  if(APP_ZDP_SapHandler(pAppZdoMsg))
  {
    /* insert code here to handle error  */
  }
}
#endif

#if gComplex_Desc_req_d
void ASL_Complex_Desc_req
(
	zbCounter_t  *pSequenceNumber,
	zbNwkAddr_t  aDestAddress
)
{
	appToZdpMessage_t *pAppZdoMsg;
	pAppZdoMsg = MSG_Alloc( MbrOfs(appToZdpMessage_t, msgData) + sizeof(zbComplexDescriptorRequest_t));

	if (pAppZdoMsg == NULL)
		return;

	if (pSequenceNumber != NULL)
		pSequenceNumber[0] = gZdpSequence;

	pAppZdoMsg->msgType = gComplex_Desc_req_c;
	Copy2Bytes(pAppZdoMsg->aDestAddr, aDestAddress);
	Copy2Bytes(pAppZdoMsg->msgData.complexDescReq.aNwkAddrOfInterest, aDestAddress);

	if(APP_ZDP_SapHandler ( pAppZdoMsg ))
	{
		/* Insert code here to handle error  */
	}
}
#endif

#if gUser_Desc_req_d
void ASL_User_Desc_req
(
	zbCounter_t  *pSequenceNumber,
	zbNwkAddr_t  aDestAddress
)
{
	appToZdpMessage_t *pAppZdoMsg;
	pAppZdoMsg = MSG_Alloc( MbrOfs(appToZdpMessage_t, msgData) + sizeof(zbUserDescriptorRequest_t));

	if (pAppZdoMsg == NULL)
		return;

	if (pSequenceNumber != NULL)
		pSequenceNumber[0] = gZdpSequence;

	pAppZdoMsg->msgType = gUser_Desc_req_c;
	Copy2Bytes(pAppZdoMsg->aDestAddr, aDestAddress);
	Copy2Bytes(pAppZdoMsg->msgData.complexDescReq.aNwkAddrOfInterest, aDestAddress);

	if(APP_ZDP_SapHandler ( pAppZdoMsg ))
	{
		/* Insert code here to handle error  */
	}
}
#endif

#if gDiscovery_Cache_req_d
void ASL_Discovery_Cache_req
(
	zbCounter_t  *pSequenceNumber,
	zbNwkAddr_t  aDestAddress,
	zbNwkAddr_t  aNwkAddrOfInterest,
	zbIeeeAddr_t  aIEEEAddrOfInterest
)
{
	appToZdpMessage_t *pAppZdoMsg;
	pAppZdoMsg = MSG_Alloc( MbrOfs(appToZdpMessage_t, msgData) + sizeof(zbDiscoveryCacheRequest_t));

	if (pAppZdoMsg == NULL)
		return;

	if (pSequenceNumber != NULL)
		pSequenceNumber[0] = gZdpSequence;

	pAppZdoMsg->msgType = gDiscovery_Cache_req_c;
	Copy2Bytes(pAppZdoMsg->aDestAddr, aDestAddress);

	Copy2Bytes(pAppZdoMsg->msgData.discoveryCacheReq.aNwkAddress, aNwkAddrOfInterest);
	Copy8Bytes(pAppZdoMsg->msgData.discoveryCacheReq.aIeeeAddress, aIEEEAddrOfInterest);

	if(APP_ZDP_SapHandler ( pAppZdoMsg ))
	{
		/* Insert code here to handle error  */
	}
}
#endif

#if  gDevice_annce_d
void ASL_Device_annce
(
	zbCounter_t  *pSequenceNumber,
	zbNwkAddr_t  aDestAddress,
	zbNwkAddr_t   aNwkAddress,
	zbIeeeAddr_t  aIeeeAddress,
	macCapabilityInfo_t  capability
)
{
	appToZdpMessage_t *pAppZdoMsg;
	pAppZdoMsg = MSG_Alloc( MbrOfs(appToZdpMessage_t, msgData) + sizeof(zbEndDeviceAnnounce_t));

	if (pAppZdoMsg == NULL)
		return;

	if (pSequenceNumber != NULL)
		pSequenceNumber[0] = gZdpSequence;

	pAppZdoMsg->msgType = gDevice_annce_c;
	Copy2Bytes(pAppZdoMsg->aDestAddr, aDestAddress);

	Copy2Bytes(pAppZdoMsg->msgData.endDeviceAnnce.aNwkAddress, aNwkAddress);
	Copy8Bytes(pAppZdoMsg->msgData.endDeviceAnnce.aIeeeAddress,aIeeeAddress);
	pAppZdoMsg->msgData.endDeviceAnnce.capability = capability;

	if(APP_ZDP_SapHandler ( pAppZdoMsg ))
	{
		/* Insert code here to handle error  */
	}
}
#endif

#if gUser_Desc_set_d
void ASL_User_Desc_set
(
	zbCounter_t  *pSequenceNumber,
	zbNwkAddr_t  aDestAddress,
	zbSize_t  length,
	zbUserDescriptor_t  aUserDescription
)
{
		appToZdpMessage_t *pAppZdoMsg;
	pAppZdoMsg = MSG_Alloc( MbrOfs(appToZdpMessage_t, msgData) + sizeof(zbUserDescriptorSet_t));

	if (pAppZdoMsg == NULL)
		return;

	if (pSequenceNumber != NULL)
		pSequenceNumber[0] = gZdpSequence;

	pAppZdoMsg->msgType = gUser_Desc_set_c;
	Copy2Bytes(pAppZdoMsg->aDestAddr, aDestAddress);

	Copy2Bytes(pAppZdoMsg->msgData.userDescSet.aNwkAddrOfInterest, aDestAddress);
	pAppZdoMsg->msgData.userDescSet.descriptor.length = length;
	FLib_MemCpy(pAppZdoMsg->msgData.userDescSet.descriptor.aUserDescription,
							aUserDescription,
							sizeof(zbUserDescriptor_t));

	if(APP_ZDP_SapHandler ( pAppZdoMsg ))
	{
		/* Insert code here to handle error  */
	}
}
#endif

#if gSystem_Server_Discovery_req_d
void ASL_System_Server_Discovery_req
(
	zbCounter_t  *pSequenceNumber,
	zbNwkAddr_t  aDestAddress,
	zbServerMask_t  aServerMask
)
{
	appToZdpMessage_t *pAppZdoMsg;
	pAppZdoMsg = MSG_Alloc( MbrOfs(appToZdpMessage_t, msgData) + sizeof(zbSystemServerDiscoveryRequest_t));

	if (pAppZdoMsg == NULL)
		return;

	if (pSequenceNumber != NULL)
		pSequenceNumber[0] = gZdpSequence;

	pAppZdoMsg->msgType = gSystem_Server_Discovery_req_c;
	Copy2Bytes(pAppZdoMsg->aDestAddr, aDestAddress);
	Copy2Bytes(pAppZdoMsg->msgData.systemServerDiscReq.aServerMask, aServerMask);

	if(APP_ZDP_SapHandler ( pAppZdoMsg ))
	{
		/* Insert code here to handle error  */
	}
}
#endif

#if gDiscovery_store_req_d
void ASL_Discovery_store_req
(
	zbCounter_t  *pSequenceNumber,
	zbNwkAddr_t  aDestAddress,
	zbDiscoveryStoreRequest_t  *pDiscoveryStore
)
{
	appToZdpMessage_t *pAppZdoMsg;
	pAppZdoMsg = MSG_Alloc( MbrOfs(appToZdpMessage_t, msgData) +
													MbrOfs(zbDiscoveryStoreRequest_t,simpleDescriptorList[0]) +
													pDiscoveryStore->simpleDescriptorCount);

	if (pAppZdoMsg == NULL)
		return;

	if (pSequenceNumber != NULL)
		pSequenceNumber[0] = gZdpSequence;

	pAppZdoMsg->msgType = gDiscovery_store_req_c;
	Copy2Bytes(pAppZdoMsg->aDestAddr, aDestAddress);
	FLib_MemCpy(&pAppZdoMsg->msgData.discoveryStoreReq,
							pDiscoveryStore,
							MbrOfs(zbDiscoveryStoreRequest_t,simpleDescriptorList[0]));

	FLib_MemCpy(pAppZdoMsg->msgData.discoveryStoreReq.simpleDescriptorList,
							pDiscoveryStore->simpleDescriptorList,
							pDiscoveryStore->simpleDescriptorCount);

	if(APP_ZDP_SapHandler ( pAppZdoMsg ))
	{
		/* Insert code here to handle error  */
	}
}
#endif

#if gNode_Desc_store_req_d
void ASL_Node_Desc_store_req
(
	zbCounter_t  *pSequenceNumber,
	zbNwkAddr_t  aDestAddress,
	zbNodeDescriptor_t  *pNodeDescriptor
)
{
	appToZdpMessage_t *pAppZdoMsg;
	pAppZdoMsg = MSG_Alloc( MbrOfs(appToZdpMessage_t, msgData) + sizeof(zbNodeDescriptorStoreRequest_t));

	if (pAppZdoMsg == NULL)
		return;

	if (pSequenceNumber != NULL)
		pSequenceNumber[0] = gZdpSequence;

	pAppZdoMsg->msgType = gNode_Desc_store_req_c;
	Copy2Bytes(pAppZdoMsg->aDestAddr, aDestAddress);
	FLib_MemCpy(&pAppZdoMsg->msgData.nodeDescriptorStoreReq,
							pNodeDescriptor,
							sizeof(zbNodeDescriptorStoreRequest_t));

	if(APP_ZDP_SapHandler ( pAppZdoMsg ))
	{
		/* Insert code here to handle error  */
	}
}
#endif

#if gPower_Desc_store_req_d
void ASL_Power_Desc_store_req
(
	zbCounter_t  *pSequenceNumber,
	zbNwkAddr_t  aDestAddress,
	zbPowerDescriptor_t  *pPowerDescriptor
)
{
	appToZdpMessage_t *pAppZdoMsg;
	pAppZdoMsg = MSG_Alloc( MbrOfs(appToZdpMessage_t, msgData) + sizeof(zbPowerDescriptorStoreRequest_t));

	if (pAppZdoMsg == NULL)
		return;

	if (pSequenceNumber != NULL)
		pSequenceNumber[0] = gZdpSequence;

	pAppZdoMsg->msgType = gPower_Desc_store_req_c;
	Copy2Bytes(pAppZdoMsg->aDestAddr, aDestAddress);
	FLib_MemCpy(&pAppZdoMsg->msgData.powerDescriptorStoreReq,
							pPowerDescriptor,
							sizeof(zbPowerDescriptorStoreRequest_t));

	if(APP_ZDP_SapHandler ( pAppZdoMsg ))
	{
		/* Insert code here to handle error  */
	}
}
#endif

#if gActive_EP_store_req_d
void ASL_Active_EP_store_req
(
	zbCounter_t  *pSequenceNumber,
	zbNwkAddr_t  aDestAddress,
	zbCounter_t  activeEPcount,
	zbEndPoint_t  *pActiveEPList
)
{
	appToZdpMessage_t *pAppZdoMsg;
	pAppZdoMsg = MSG_Alloc( MbrOfs(appToZdpMessage_t, msgData) +
													MbrOfs(zbActiveEPStoreRequest_t, activeEPList[0]) + activeEPcount);

	if (pAppZdoMsg == NULL)
		return;

	if (pSequenceNumber != NULL)
		pSequenceNumber[0] = gZdpSequence;

	pAppZdoMsg->msgType = gActive_EP_store_req_c;
	Copy2Bytes(pAppZdoMsg->aDestAddr, aDestAddress);
	pAppZdoMsg->msgData.activeEPStoreReq.activeEPCount = activeEPcount;
	FLib_MemCpy(pAppZdoMsg->msgData.activeEPStoreReq.activeEPList,pActiveEPList,activeEPcount);

	if(APP_ZDP_SapHandler ( pAppZdoMsg ))
	{
		/* Insert code here to handle error  */
	}
}
#endif

#if gSimple_Desc_store_req_d
void ASL_Simple_Desc_store_req
(
	zbCounter_t  *pSequenceNumber,
	zbNwkAddr_t  aDestAddress,
	zbSimpleDescriptorStoreRequest_t  *pSimpleDescStore
)
{
	appToZdpMessage_t *pAppZdoMsg;
	uint8_t  *pPtr;
	pAppZdoMsg = MSG_Alloc( MbrOfs(appToZdpMessage_t, msgData) +
													MbrOfs(zbSimpleDescriptorStoreRequest_t, simpleDescriptor.inputClusters) +
													pSimpleDescStore->simpleDescriptor.inputClusters.cNumClusters * sizeof(zbClusterId_t) +
													pSimpleDescStore->simpleDescriptor.outputClusters.cNumClusters * sizeof(zbClusterId_t));

	if (pAppZdoMsg == NULL)
		return;

	if (pSequenceNumber != NULL)
		pSequenceNumber[0] = gZdpSequence;

	pAppZdoMsg->msgType = gSimple_Desc_store_req_c;
	Copy2Bytes(pAppZdoMsg->aDestAddr, aDestAddress);
	FLib_MemCpy(&pAppZdoMsg->msgData.simpleDescriptorStoreReq,
							pSimpleDescStore,
							MbrOfs(zbSimpleDescriptorStoreRequest_t, simpleDescriptor.inputClusters));
	pAppZdoMsg->msgData.simpleDescriptorStoreReq.simpleDescriptor.inputClusters.cNumClusters = pSimpleDescStore->simpleDescriptor.inputClusters.cNumClusters;
	pAppZdoMsg->msgData.simpleDescriptorStoreReq.simpleDescriptor.outputClusters.cNumClusters = pSimpleDescStore->simpleDescriptor.outputClusters.cNumClusters;
	pPtr = (uint8_t *)(((uint8_t *)&pAppZdoMsg->msgData.simpleDescriptorStoreReq) + sizeof(zbSimpleDescriptorStoreRequest_t));

	FLib_MemCpy(pPtr, pSimpleDescStore->simpleDescriptor.inputClusters.pClusterList,
							pSimpleDescStore->simpleDescriptor.inputClusters.cNumClusters * sizeof(zbClusterId_t));
	pAppZdoMsg->msgData.simpleDescriptorStoreReq.simpleDescriptor.inputClusters.pClusterList = (zbClusterId_t *)pPtr;
	pPtr += (pSimpleDescStore->simpleDescriptor.inputClusters.cNumClusters * sizeof(zbClusterId_t));
	FLib_MemCpy(pPtr, pSimpleDescStore->simpleDescriptor.outputClusters.pClusterList,
							pSimpleDescStore->simpleDescriptor.outputClusters.cNumClusters * sizeof(zbClusterId_t));
	pAppZdoMsg->msgData.simpleDescriptorStoreReq.simpleDescriptor.outputClusters.pClusterList = (zbClusterId_t *)pPtr;

	if(APP_ZDP_SapHandler ( pAppZdoMsg ))
	{
		/* Insert code here to handle error  */
	}
}
#endif

#if gRemove_node_cache_req_d
void ASL_Remove_node_cache_req
(
	zbCounter_t  *pSequenceNumber,
	zbNwkAddr_t  aDestAddress,
	zbNwkAddr_t  aNwkAddress,
	zbIeeeAddr_t  aIeeeAddress
)
{
	appToZdpMessage_t *pAppZdoMsg;
	pAppZdoMsg = MSG_Alloc( MbrOfs(appToZdpMessage_t, msgData) + sizeof(zbRemoveNodeCacheRequest_t));

	if (pAppZdoMsg == NULL)
		return;

	if (pSequenceNumber != NULL)
		pSequenceNumber[0] = gZdpSequence;

	pAppZdoMsg->msgType = gRemove_node_cache_req_c;
	Copy2Bytes(pAppZdoMsg->aDestAddr, aDestAddress);

	Copy2Bytes(pAppZdoMsg->msgData.removeNodeCacheReq.aNwkAddress, aNwkAddress);
	Copy8Bytes(pAppZdoMsg->msgData.removeNodeCacheReq.aIeeeAddress, aIeeeAddress);

	if(APP_ZDP_SapHandler ( pAppZdoMsg ))
	{
		/* Insert code here to handle error  */
	}
}
#endif

#if gFind_node_cache_req_d
void ASL_Find_node_cache_req
(
	zbCounter_t  *pSequenceNumber,
	zbNwkAddr_t  aDestAddress,
	zbNwkAddr_t  aNwkAddress,
	zbIeeeAddr_t  aIeeeAddress
)
{
	appToZdpMessage_t *pAppZdoMsg;
	pAppZdoMsg = MSG_Alloc( MbrOfs(appToZdpMessage_t, msgData) + sizeof(zbFindNodeCacheRequest_t));

	if (pAppZdoMsg == NULL)
		return;

	if (pSequenceNumber != NULL)
		pSequenceNumber[0] = gZdpSequence;

	pAppZdoMsg->msgType = gFind_node_cache_req_c;
	Copy2Bytes(pAppZdoMsg->aDestAddr, aDestAddress);

	Copy2Bytes(pAppZdoMsg->msgData.findNodeCacheReq.aNwkAddrOfInterest, aNwkAddress);
	Copy8Bytes(pAppZdoMsg->msgData.findNodeCacheReq.aIeeeAddrOfInterest, aIeeeAddress);

	if(APP_ZDP_SapHandler ( pAppZdoMsg ))
	{
		/* Insert code here to handle error  */
	}
}
#endif

/*==============================================================================================
	======== End Device Bind, Bind, Unbind and Bind Management Client Services Primitives ========
	==============================================================================================*/
#if gEnd_Device_Bind_req_d
void ASL_EndDeviceBindRequest
(
	zbCounter_t  *pSequenceNumber,
	zbNwkAddr_t  aDestAddress,
	zbSimpleDescriptor_t *pSimpleDescriptor
)
{
	appToZdpMessage_t *pAppZdoMsg;
	uint8_t * tmp_ptr;

	/* assign memory to the EndDeviceBindRequest	messenge (special case)*/
	pAppZdoMsg = MSG_Alloc( MbrOfs(appToZdpMessage_t, msgData) +
													sizeof(zbEndDeviceBindRequest_t)+
													((pSimpleDescriptor->appNumInClusters*sizeof(zbClusterId_t))+
													(pSimpleDescriptor->appNumOutClusters*sizeof(zbClusterId_t))));

	if (pAppZdoMsg == NULL)
		return;

	if( pSequenceNumber != NULL)
		pSequenceNumber[0] = gZdpSequence;

	pAppZdoMsg->msgType = gEnd_Device_Bind_req_c; /* EndDeviceBindRequest type message */
	Copy2Bytes(pAppZdoMsg->aDestAddr, aDestAddress);

	Copy2Bytes(pAppZdoMsg->msgData.endDeviceBindReq.aBindingTarget,NlmeGetRequest(gNwkShortAddress_c));
	Copy2Bytes(pAppZdoMsg->msgData.endDeviceBindReq.aProfileId, pSimpleDescriptor->aAppProfId);
	Copy8Bytes(pAppZdoMsg->msgData.endDeviceBindReq.aSrcIeeeAddress,NlmeGetRequest(gNwkIeeeAddress_c));
	pAppZdoMsg->msgData.endDeviceBindReq.srcEndPoint = pSimpleDescriptor->endPoint;
	pAppZdoMsg->msgData.endDeviceBindReq.inputClusterList.cNumClusters = pSimpleDescriptor->appNumInClusters;
	pAppZdoMsg->msgData.endDeviceBindReq.outputClusterList.cNumClusters = pSimpleDescriptor->appNumOutClusters;

	/* Copying the InClusterList to tmp_ptr, just the list not the counters */
	tmp_ptr = (uint8_t *)pAppZdoMsg + sizeof(zbMsgId_t)+sizeof(zbNwkAddr_t)+sizeof(zbEndDeviceBindRequest_t);

	FLib_MemCpy( tmp_ptr, pSimpleDescriptor->pAppInClusterList,(pSimpleDescriptor->appNumInClusters * sizeof(zbClusterId_t)));
	pAppZdoMsg->msgData.endDeviceBindReq.inputClusterList.pClusterList = (zbClusterId_t *) tmp_ptr;

	/* Copying OutClusterList to tmp_ptr, just the list not the counters */
	tmp_ptr= (uint8_t *) pAppZdoMsg->msgData.endDeviceBindReq.inputClusterList.pClusterList + (pSimpleDescriptor->appNumInClusters * sizeof(zbClusterId_t));
	FLib_MemCpy( tmp_ptr, pSimpleDescriptor->pAppOutClusterList, (pSimpleDescriptor->appNumOutClusters*sizeof(zbClusterId_t)));
	pAppZdoMsg->msgData.endDeviceBindReq.outputClusterList.pClusterList = (zbClusterId_t*)tmp_ptr;

	if(APP_ZDP_SapHandler( pAppZdoMsg ))
	{
		/* Insert code here to handle error  */
	}
}
#endif

#if gBind_req_d || gUnbind_req_d
void APP_ZDP_BindUnbindRequest
(
	zbCounter_t  *pSequenceNumber,
	zbNwkAddr_t  aDestAddress,
	zbMsgId_t  BindUnbind,
	zbBindUnbindRequest_t  *pBindUnBindRequest
)
{
	appToZdpMessage_t *pAppZdoMsg;
	pAppZdoMsg = MSG_Alloc( MbrOfs(appToZdpMessage_t, msgData) + sizeof(zbBindUnbindRequest_t));

	if( pAppZdoMsg == NULL )
		return;

	if (pSequenceNumber != NULL)
		pSequenceNumber[0] = gZdpSequence;

	pAppZdoMsg->msgType = BindUnbind;
	Copy2Bytes(pAppZdoMsg->aDestAddr,aDestAddress);

	/* Filling up self IEEEAddress */
	Copy8Bytes(pAppZdoMsg->msgData.bindReq.aSrcAddress,pBindUnBindRequest->aSrcAddress);
	pAppZdoMsg->msgData.bindReq.srcEndPoint = pBindUnBindRequest->srcEndPoint;
	/* Filling up self ClusterID */
	Copy2Bytes(pAppZdoMsg->msgData.bindReq.aClusterId,pBindUnBindRequest->aClusterId);
	/* Filling up self use indirect address mode to read binding table */
	pAppZdoMsg->msgData.bindReq.addressMode = pBindUnBindRequest->addressMode;
	if (pAppZdoMsg->msgData.bindReq.addressMode == zbGroupMode)
		Copy2Bytes(pAppZdoMsg->msgData.bindReq.destData.groupMode.aDstaddress,pBindUnBindRequest->destData.groupMode.aDstaddress);
	else
	{
		Copy8Bytes(pAppZdoMsg->msgData.bindReq.destData.extendedMode.aDstAddress, pBindUnBindRequest->destData.extendedMode.aDstAddress);
		pAppZdoMsg->msgData.bindReq.destData.extendedMode.dstEndPoint = pBindUnBindRequest->destData.extendedMode.dstEndPoint;
	}
	if(APP_ZDP_SapHandler( pAppZdoMsg ))
	{
		/* Insert code here to handle error */
	}
}
#endif

#if gBind_Register_req_d
void ASL_Bind_Register_req
(
	zbCounter_t  *pSequenceNumber,
	zbNwkAddr_t  aDestAddress,
	zbIeeeAddr_t  aNodeAddress
)
{
	appToZdpMessage_t *pAppZdoMsg;
	pAppZdoMsg = MSG_Alloc( MbrOfs(appToZdpMessage_t, msgData) + sizeof(zbBindRegisterRequest_t));

	if( pAppZdoMsg == NULL )
		return;

	if (pSequenceNumber != NULL)
		pSequenceNumber[0] = gZdpSequence;

	pAppZdoMsg->msgType = gBind_Register_req_c;
	Copy2Bytes(pAppZdoMsg->aDestAddr,aDestAddress);

	Copy2Bytes(pAppZdoMsg->msgData.bindRegisterReq.aIeeeAddress, aNodeAddress);

	if(APP_ZDP_SapHandler( pAppZdoMsg ))
	{
		/* Insert code here to handle error */
	}
}
#endif

#if gReplace_Device_req_d
void ASL_Replace_Device_req
(
	zbCounter_t  *pSequenceNumber,
	zbNwkAddr_t  aDestAddress,
	zbNwkAddr_t  aOldAddress,
	zbEndPoint_t  oldEndPoint,
	zbNwkAddr_t  aNewAddress,
	zbEndPoint_t  newEndPoint
)
{
	appToZdpMessage_t *pAppZdoMsg;
	pAppZdoMsg = MSG_Alloc( MbrOfs(appToZdpMessage_t, msgData) + sizeof(zbReplaceDeviceRequest_t));

	if( pAppZdoMsg == NULL )
		return;

	if (pSequenceNumber != NULL)
		pSequenceNumber[0] = gZdpSequence;

	pAppZdoMsg->msgType = gReplace_Device_req_c;
	Copy2Bytes(pAppZdoMsg->aDestAddr,aDestAddress);

	Copy2Bytes(pAppZdoMsg->msgData.replaceDeviceReq.aOldAddress, aOldAddress);
	Copy2Bytes(pAppZdoMsg->msgData.replaceDeviceReq.aNewAddress, aNewAddress);

	pAppZdoMsg->msgData.replaceDeviceReq.oldEndPoint = oldEndPoint;
	pAppZdoMsg->msgData.replaceDeviceReq.newEndPoint = newEndPoint;

	if(APP_ZDP_SapHandler( pAppZdoMsg ))
	{
		/* Insert code here to handle error */
	}
}
#endif

#if gStore_Bkup_Bind_Entry_req_d
void ASL_Store_Bkup_Bind_Entry_req
(
	zbCounter_t  *pSequenceNumber,
	zbNwkAddr_t  aDestAddress,
	zbStoreBkupBindEntryRequest_t  *pStoreBkupEntry
)
{
	appToZdpMessage_t *pAppZdoMsg;
	pAppZdoMsg = MSG_Alloc( MbrOfs(appToZdpMessage_t, msgData) + sizeof(zbStoreBkupBindEntryRequest_t));

	if( pAppZdoMsg == NULL )
		return;

	if (pSequenceNumber != NULL)
		pSequenceNumber[0] = gZdpSequence;

	pAppZdoMsg->msgType = gStore_Bkup_Bind_Entry_req_c;
	Copy2Bytes(pAppZdoMsg->aDestAddr,aDestAddress);

	FLib_MemCpy(&pAppZdoMsg->msgData.storeBackupBindingEntryReq, pStoreBkupEntry, sizeof(zbStoreBkupBindEntryRequest_t));

	if(APP_ZDP_SapHandler( pAppZdoMsg ))
	{
		/* Insert code here to handle error */
	}
}
#endif

#if gRemove_Bkup_Bind_Entry_req_d
void ASL_Remove_Bkup_Bind_Entry_req
(
	zbCounter_t  *pSequenceNumber,
	zbNwkAddr_t  aDestAddress,
	zbRemoveBackupBindEntryRequest_t  *pRemoveBkupEntry
)
{
	appToZdpMessage_t *pAppZdoMsg;
	pAppZdoMsg = MSG_Alloc( MbrOfs(appToZdpMessage_t, msgData) + sizeof(zbRemoveBackupBindEntryRequest_t));

	if( pAppZdoMsg == NULL )
		return;

	if (pSequenceNumber != NULL)
		pSequenceNumber[0] = gZdpSequence;

	pAppZdoMsg->msgType = gRemove_Bkup_Bind_Entry_req_c;
	Copy2Bytes(pAppZdoMsg->aDestAddr,aDestAddress);

	FLib_MemCpy(&pAppZdoMsg->msgData.removeBackupBindingEntryReq, pRemoveBkupEntry, sizeof(zbRemoveBackupBindEntryRequest_t));

	if(APP_ZDP_SapHandler( pAppZdoMsg ))
	{
		/* Insert code here to handle error */
	}
}
#endif

#if gBackup_Bind_Table_req_d
void ASL_Backup_Bind_Table_req
(
	zbCounter_t  *pSequenceNumber,
	zbNwkAddr_t  aDestAddress,
	zbBackupBindTableRequest_t  *pBackupBindTable
)
{
	appToZdpMessage_t *pAppZdoMsg;
	pAppZdoMsg = MSG_Alloc( MbrOfs(appToZdpMessage_t, msgData) +
													MbrOfs(zbBackupBindTableRequest_t, BindingTableList[0]) +
													(OTA2Native16(TwoBytesToUint16(pBackupBindTable->BindingTableListCount)) *
													MbrSizeof(zbBackupBindTableRequest_t, BindingTableList[0])));

	if( pAppZdoMsg == NULL )
		return;

	if (pSequenceNumber != NULL)
		pSequenceNumber[0] = gZdpSequence;

	pAppZdoMsg->msgType = gBackup_Bind_Table_req_c;
	Copy2Bytes(pAppZdoMsg->aDestAddr,aDestAddress);

	FLib_MemCpy(&pAppZdoMsg->msgData.backupBindTableReq,
							pBackupBindTable,
							(MbrOfs(zbBackupBindTableRequest_t, BindingTableList[0]) +
							(OTA2Native16(TwoBytesToUint16(pBackupBindTable->BindingTableListCount)) *
							MbrSizeof(zbBackupBindTableRequest_t, BindingTableList[0]))));

	if(APP_ZDP_SapHandler( pAppZdoMsg ))
	{
		/* Insert code here to handle error */
	}
}
#endif

#if gRecover_Bind_Table_req_d
void ASL_Recover_Bind_Table_req
(
	zbCounter_t  *pSequenceNumber,
	zbNwkAddr_t  aDestAddress,
	index_t  index
)
{
	appToZdpMessage_t *pAppZdoMsg;
	pAppZdoMsg = MSG_Alloc( MbrOfs(appToZdpMessage_t, msgData) + sizeof(zbRecoverBindTableRequest_t));

	if( pAppZdoMsg == NULL )
		return;

	if (pSequenceNumber != NULL)
		pSequenceNumber[0] = gZdpSequence;

	pAppZdoMsg->msgType = gRecover_Bind_Table_req_c;
	Copy2Bytes(pAppZdoMsg->aDestAddr,aDestAddress);

	Set2Bytes(pAppZdoMsg->msgData.recoverBindTableReq.startIndex, index);
#if (gBigEndian_c)       
	Swap2BytesArray(pAppZdoMsg->msgData.recoverBindTableReq.startIndex);
#endif       

	if(APP_ZDP_SapHandler( pAppZdoMsg ))
	{
		/* Insert code here to handle error */
	}
}
#endif

#if gBackup_Source_Bind_req_d
void ASL_Backup_Source_Bind_req
(
	zbCounter_t  *pSequenceNumber,
	zbNwkAddr_t  aDestAddress,
	zbBackupSourceBindRequest_t  *pBkupSourceBindTable
)
{
	appToZdpMessage_t *pAppZdoMsg;
	pAppZdoMsg = MSG_Alloc( MbrOfs(appToZdpMessage_t, msgData) +
													MbrOfs(zbBackupSourceBindRequest_t, SourceTableList[0]) +
													(TwoBytesToUint16(pBkupSourceBindTable->SourceTableListCount) *
													MbrSizeof(zbBackupSourceBindRequest_t, SourceTableList[0])));

	if( pAppZdoMsg == NULL )
		return;

	if (pSequenceNumber != NULL)
		pSequenceNumber[0] = gZdpSequence;

	pAppZdoMsg->msgType = gBackup_Source_Bind_req_c;
	Copy2Bytes(pAppZdoMsg->aDestAddr,aDestAddress);

	FLib_MemCpy(&pAppZdoMsg->msgData.backupSourceBindReq,
							pBkupSourceBindTable,
							(MbrOfs(zbBackupSourceBindRequest_t, SourceTableList[0]) +
							(TwoBytesToUint16(pBkupSourceBindTable->SourceTableListCount) *
							MbrSizeof(zbBackupSourceBindRequest_t, SourceTableList[0]))));

	if(APP_ZDP_SapHandler( pAppZdoMsg ))
	{
		/* Insert code here to handle error */
	}
}
#endif

#if gRecover_Source_Bind_req_d
void ASL_Recover_Source_Bind_req
(
	zbCounter_t  *pSequenceNumber,
	zbNwkAddr_t  aDestAddress,
	index_t  index
)
{
	appToZdpMessage_t *pAppZdoMsg;
	pAppZdoMsg = MSG_Alloc( MbrOfs(appToZdpMessage_t, msgData) + sizeof(zbRecoverSourceBindRequest_t));

	if( pAppZdoMsg == NULL )
		return;

	if (pSequenceNumber != NULL)
		pSequenceNumber[0] = gZdpSequence;

	pAppZdoMsg->msgType = gRecover_Source_Bind_req_c;
	Copy2Bytes(pAppZdoMsg->aDestAddr,aDestAddress);

	Set2Bytes(pAppZdoMsg->msgData.recoverSourceBindReq.StartIndex, index);
#if (gBigEndian_c)        
	Swap2BytesArray(pAppZdoMsg->msgData.recoverSourceBindReq.StartIndex);
#endif
	if(APP_ZDP_SapHandler( pAppZdoMsg ))
	{
		/* Insert code here to handle error */
	}
}
#endif

/*==============================================================================================
	============================= Network Management Client Services =============================
	==============================================================================================*/
#if gMgmt_NWK_Disc_req_d
void ASL_Mgmt_NWK_Disc_req
(
	zbCounter_t  *pSequenceNumber,
	zbNwkAddr_t  aDestAddress,
	zbChannels_t  aScanChannel,
	zbCounter_t  scanDuration,
	index_t  startIndex
)
{
	appToZdpMessage_t *pAppZdoMsg;
	pAppZdoMsg = MSG_Alloc( MbrOfs(appToZdpMessage_t, msgData) + sizeof(zbMgmtNwkDiscoveryRequest_t));

	if( pAppZdoMsg == NULL )
		return;

	if (pSequenceNumber != NULL)
		pSequenceNumber[0] = gZdpSequence;

	pAppZdoMsg->msgType = gMgmt_NWK_Disc_req_c;
	Copy2Bytes(pAppZdoMsg->aDestAddr,aDestAddress);

	FLib_MemCpy(pAppZdoMsg->msgData.mgmtNwkDiscReq.aScanChannel, aScanChannel, sizeof(zbChannels_t));
	pAppZdoMsg->msgData.mgmtNwkDiscReq.scanDuration = scanDuration;
	pAppZdoMsg->msgData.mgmtNwkDiscReq.startIndex = startIndex;

	if(APP_ZDP_SapHandler( pAppZdoMsg ))
	{
		/* Insert code here to handle error */
	}
}
#endif

#if gMgmt_Lqi_req_d
void ASL_Mgmt_Lqi_req
(
	zbCounter_t  *pSequenceNumber,
	zbNwkAddr_t  aDestAddress,
	index_t  index
)
{
	appToZdpMessage_t *pAppZdoMsg;
	pAppZdoMsg = MSG_Alloc( MbrOfs(appToZdpMessage_t, msgData) + sizeof(zbMgmtLqiRequest_t));

	if( pAppZdoMsg == NULL )
		return;

	if (pSequenceNumber != NULL)
		pSequenceNumber[0] = gZdpSequence;

	pAppZdoMsg->msgType = gMgmt_Lqi_req_c;
	Copy2Bytes(pAppZdoMsg->aDestAddr,aDestAddress);

	pAppZdoMsg->msgData.mgmtLqiReq.startIndex = index;

	if(APP_ZDP_SapHandler( pAppZdoMsg ))
	{
		/* Insert code here to handle error */
	}
}
#endif

#if gMgmt_Rtg_req_d
void ASL_Mgmt_Rtg_req
(
	zbCounter_t  *pSequenceNumber,
	zbNwkAddr_t  aDestAddress,
	index_t  index
)
{
	appToZdpMessage_t *pAppZdoMsg;
	pAppZdoMsg = MSG_Alloc( MbrOfs(appToZdpMessage_t, msgData) + sizeof(zbMgmtRtgRequest_t));

	if( pAppZdoMsg == NULL )
		return;

	if (pSequenceNumber != NULL)
		pSequenceNumber[0] = gZdpSequence;

	pAppZdoMsg->msgType = gMgmt_Lqi_req_c;
	Copy2Bytes(pAppZdoMsg->aDestAddr,aDestAddress);

	pAppZdoMsg->msgData.mgmtRtgReq.startIndex = index;

	if(APP_ZDP_SapHandler( pAppZdoMsg ))
	{
		/* Insert code here to handle error */
	}
}
#endif

#if gMgmt_Bind_req_d
void ASL_Mgmt_Bind_req
(
	zbCounter_t  *pSequenceNumber,
	zbNwkAddr_t  aDestAddress,
	index_t  index
)
{
	appToZdpMessage_t *pAppZdoMsg;
	pAppZdoMsg = MSG_Alloc( MbrOfs(appToZdpMessage_t, msgData) + sizeof(zbMgmtBindRequest_t));

	if( pAppZdoMsg == NULL )
		return;

	if (pSequenceNumber != NULL)
		pSequenceNumber[0] = gZdpSequence;

	pAppZdoMsg->msgType = gMgmt_Bind_req_c;
	Copy2Bytes(pAppZdoMsg->aDestAddr,aDestAddress);

	pAppZdoMsg->msgData.mgmtBindReq.startIndex = index;

	if(APP_ZDP_SapHandler( pAppZdoMsg ))
	{
		/* Insert code here to handle error */
	}
}
#endif

#if gMgmt_Leave_req_d
void ASL_Mgmt_Leave_req
(
	zbCounter_t  *pSequenceNumber,
	zbNwkAddr_t  aDestAddress,
	zbIeeeAddr_t aDeviceAddres,
	zbMgmtOptions_t  mgmtOptions
)
{
	appToZdpMessage_t *pAppZdoMsg;
	pAppZdoMsg = MSG_Alloc( MbrOfs(appToZdpMessage_t, msgData) + sizeof(zbMgmtLeaveRequest_t));

	if( pAppZdoMsg == NULL )
		return;

	if (pSequenceNumber != NULL)
		pSequenceNumber[0] = gZdpSequence;

	pAppZdoMsg->msgType = gMgmt_Leave_req_c;
	Copy2Bytes(pAppZdoMsg->aDestAddr,aDestAddress);

	pAppZdoMsg->msgData.mgmtLeaveReq.mgmtOptions = mgmtOptions;
	Copy8Bytes(pAppZdoMsg->msgData.mgmtLeaveReq.aDeviceAddress, aDeviceAddres);

	if(APP_ZDP_SapHandler( pAppZdoMsg ))
	{
		/* Insert code here to handle error */
	}
}
#endif


#if gMgmt_Permit_Joining_req_d
void ASL_Mgmt_Permit_Joining_req
(
	zbCounter_t  *pSequenceNumber,
	zbNwkAddr_t  aDestAddress,
	zbCounter_t  permitDuration,
	uint8_t  TC_Significance
)
{
	appToZdpMessage_t *pAppZdoMsg;
	pAppZdoMsg = MSG_Alloc( MbrOfs(appToZdpMessage_t, msgData) + sizeof(zbMgmtPermitJoiningRequest_t));

	if( pAppZdoMsg == NULL )
		return;

	if (pSequenceNumber != NULL)
		pSequenceNumber[0] = gZdpSequence;

	pAppZdoMsg->msgType = gMgmt_Permit_Joining_req_c;
	Copy2Bytes(pAppZdoMsg->aDestAddr,aDestAddress);

	pAppZdoMsg->msgData.mgmtPermitJoiningReq.PermitDuration = permitDuration;
	pAppZdoMsg->msgData.mgmtPermitJoiningReq.TC_Significance = TC_Significance;

	if(APP_ZDP_SapHandler( pAppZdoMsg ))
	{
		/* Insert code here to handle error */
	}
}
#endif

#if gMgmt_Cache_req_d
void ASL_Mgmt_Cache_Req
(
	zbCounter_t  *pSequenceNumber,
	zbNwkAddr_t  aDestAddress,
	index_t  index
)
{
	appToZdpMessage_t *pAppZdoMsg;
	pAppZdoMsg = MSG_Alloc( MbrOfs(appToZdpMessage_t, msgData) + sizeof(zbMgmtCacheRequest_t));

	if( pAppZdoMsg == NULL )
		return;

	if (pSequenceNumber != NULL)
		pSequenceNumber[0] = gZdpSequence;

	pAppZdoMsg->msgType = gMgmt_Cache_req_c;
	Copy2Bytes(pAppZdoMsg->aDestAddr,aDestAddress);

	pAppZdoMsg->msgData.mgmtCacheReq.startIndex = index;

	if(APP_ZDP_SapHandler( pAppZdoMsg ))
	{
		/* Insert code here to handle error */
	}
}
#endif

#if gMgmt_Direct_Join_req_d && (gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d)
void ASL_Mgmt_Direct_Join_req
(
  zbCounter_t  *pSequenceNumber,
  zbNwkAddr_t  aDestinationAddress,
  zbIeeeAddr_t  aDeviceAddres,
  uint8_t       capabilityInfo
)
{
  /* The message to pass down to the sap handler. */
  appToZdpMessage_t *pAppZdoMsg;

  /* The pointer to be use to fill up the message. */
  zbMgmtDirectJoinRequest_t  *pDirectJoin;

#if gComboDeviceCapability_d
  if (NlmeGetRequest(gDevType_c) == gEndDevice_c)
  {
    return;
  }
#endif

  /* Allocate the message to be sended. */
  pAppZdoMsg = MSG_Alloc( MbrOfs(appToZdpMessage_t, msgData) + sizeof(zbMgmtDirectJoinRequest_t));

  if (pAppZdoMsg == NULL)
    return;

  if (pSequenceNumber != NULL)
    pSequenceNumber[0] = gZdpSequence;

  /* Set the address where the message is going to. */
  Copy2Bytes(pAppZdoMsg->aDestAddr, aDestinationAddress);

  /* Point to the plce in memory wher to fill the message. */
  pDirectJoin = (zbMgmtDirectJoinRequest_t *)&pAppZdoMsg->msgData.mgmtDirectJoinReq;

  /* The IEEE Address of the receiving device. */
  Copy8Bytes(pDirectJoin->aDeviceAddress, aDeviceAddres);

  /* The capability information to be used by the receiving device. */
  pDirectJoin->capabilityInformation = capabilityInfo;

  if(APP_ZDP_SapHandler( pAppZdoMsg ))
  {
    /* Insert code here to handle error */
  }
}
#endif

void APP_ZDP_PermitJoinRequest
(
	uint8_t  permit /* IN: Duration */
)
{
	appToZdpMessage_t *pAppZdoMsg;
	pAppZdoMsg = MSG_Alloc( MbrOfs(appToZdpMessage_t, msgData) + sizeof(zbMgmtDirectJoinRequest_t));

	if(pAppZdoMsg == NULL)
		return;

	pAppZdoMsg->msgType = gPermitJoinReq_c;
	pAppZdoMsg->msgData.permitJoinReq.permitDuration = permit;

	if(APP_ZDP_SapHandler ( pAppZdoMsg ))
	{
		/* Insert code here to handle error  */
	}
}

void ASL_Nlme_Sync_req
(
	bool_t track
)
{
	appToZdpMessage_t  *pAppZdoMsg;
	uint8_t *pPtr;
	pAppZdoMsg = MSG_Alloc(MbrOfs(appToNwkMessage_t,msgData) + sizeof(zdoNwkSyncReq_t));

	if (pAppZdoMsg == NULL)
		return;

	pAppZdoMsg->msgType = gZdoNwkSyncReq_c;
//	Set2Bytes(pAppZdoMsg->aDestAddr, 0x0000); /* address shouldn't be needed for local calls */
	pPtr = (uint8_t *)&pAppZdoMsg->msgData;
	*pPtr = track;

	if (APP_ZDP_SapHandler ( pAppZdoMsg ))
	{
		/* Insert code here to handle error  */
	}
}

#if (gMgmt_NWK_Update_req_d && ( gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d))&& (gNetworkManagerCapability_d || gComboDeviceCapability_d)
void ASL_Mgmt_NWK_Update_req
(
  zbNwkAddr_t   aDestAddress,
  zbChannels_t  aChannelList,
  uint8_t       iScanDuration,
  uintn8_t      iScanCount
)
{
  appToZdpMessage_t *pAppZdoMsg;
  zbMgmtNwkUpdateRequest_t* pmsgData;

  if (!ZdoGetNwkManagerBitMask(gaServerMask))
  {
    return;
  }


  pAppZdoMsg = MSG_Alloc( MbrOfs(appToZdpMessage_t, msgData) + sizeof(zbMgmtNwkUpdateRequest_t));

  if( pAppZdoMsg == NULL )
    return;

  pAppZdoMsg->msgType = gMgmt_NWK_Update_req_c;
  Copy2Bytes(pAppZdoMsg->aDestAddr,aDestAddress);

  pmsgData =(zbMgmtNwkUpdateRequest_t*) &pAppZdoMsg->msgData;

  FLib_MemCpy(pmsgData->aScanChannels,
              aChannelList, sizeof (zbChannels_t));
  pmsgData->ScanDuration = iScanDuration;

  if (iScanDuration <=0x05)
    pmsgData->ExtraData.ScanCount= iScanCount;

  if (iScanDuration == 0xfe ||iScanDuration ==0xff)
    pmsgData->ExtraData.NwkManagerData.nwkUpdateId=NlmeGetRequest(gNwkUpdateId_c);

  if (iScanDuration ==0xff)
    Copy2Bytes(pmsgData->ExtraData.NwkManagerData.aNwkManagerAddr,NlmeGetRequest(gNwkShortAddress_c));

  if(APP_ZDP_SapHandler( pAppZdoMsg ))
  {
    /* Insert code here to handle error */
  }
}
#endif
/*gMgmt_NWK_Update_req_d*/

#if (gMgmt_NWK_Update_notify_d && ( gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d))&&(!gNetworkManagerCapability_d || gComboDeviceCapability_d)
void ASL_Mgmt_NWK_Update_Notify
(
  zbNwkAddr_t  aDestAddress,
  zbChannels_t  aScannedChannels,
  uint16_t      iTotalTransmissions,
  uint16_t      iTransmissionFailures,
  uint8_t       iScannedChannelListCount,
  zbEnergyValue_t  *paEnergyVslues,
  zbStatus_t    status
)
{
  appToZdpMessage_t *pAppZdoMsg;
  zbMgmtNwkUpdateNotify_t *pmsgData;

#ifndef __IAR_SYSTEMS_ICC__
  uint32_t  *pList;
#endif

  if (ZdoGetNwkManagerBitMask(gaServerMask))
  {
#ifndef __IAR_SYSTEMS_ICC__
    (void) pList;
#endif
    return;
  }

  pAppZdoMsg = MSG_Alloc(MbrOfs(appToZdpMessage_t, msgData) +
                         sizeof(zbMgmtNwkUpdateNotify_t) +
                         iScannedChannelListCount);

  if( pAppZdoMsg == NULL )
    return;

  pAppZdoMsg->msgType = gMgmt_NWK_Update_notify_c;
  Copy2Bytes(pAppZdoMsg->aDestAddr,aDestAddress);

  pmsgData = (zbMgmtNwkUpdateNotify_t*)&pAppZdoMsg->msgData;
  pmsgData->Status = status;

#ifdef __IAR_SYSTEMS_ICC__
  FLib_MemCpyReverseOrder((uint8_t*)&pmsgData->ScannedChannels,(uint8_t*)aScannedChannels, sizeof(zbChannels_t));
#else
  FLib_MemCpy((uint8_t*)&pmsgData->ScannedChannels,(uint8_t*)aScannedChannels, sizeof(zbChannels_t));
  pList = (uint32_t *)pmsgData->ScannedChannels;
  *pList = Native2OTA32(*pList);
#endif

  pmsgData->TotalTransmissions = Native2OTA16(iTotalTransmissions);
  pmsgData->TransmissionFailures = Native2OTA16(iTransmissionFailures);
  pmsgData->ScannedChannelsListCount = iScannedChannelListCount;

  if (paEnergyVslues)
  {
    FLib_MemCpy((uint8_t*)&pmsgData->aEnergyValues,paEnergyVslues,
               iScannedChannelListCount /* * MbrSizeof(zbMgmtNwkUpdateNotify_t, aEnergyValues)*/);
  }

  if(APP_ZDP_SapHandler( pAppZdoMsg ))
  {
    /* Insert code here to handle error */
  }
}
#endif
/*gMgmt_NWK_Update_notify_d*/

#if gFrequencyAgilityCapability_d 
void ASL_ChangeChannel
(
  uint8_t  channelNumber
)
{
  zdoNlmeMessage_t  *pZdoMsg;

  pZdoMsg = MSG_Alloc(sizeof(nlmeSetChannelReq_t) + sizeof(zbMsgId_t));

  if (!pZdoMsg)
    return;

  pZdoMsg->msgType = gNlmeSetChannelRequest_c;
  pZdoMsg->msgData.SetChannelReq.Channel = channelNumber;

  if (ZDO_NLME_SapHandler(pZdoMsg))
  {
    /*
      Handle error here.
    */
  }
}

#endif
/*gFrequencyAgilityCapability_d*/
/************************************************************************************
* Send a Many-To-One Nlme-Route-Discovery.req to all routers 0xFFFC, pass the confirm
* to the application using the ZDP_APP Sap handler. This function only require the 
* nwkConcentratorRadius and the NoRouteCache value.
*
* Interface assumptions:
*   The parameter iRadius, is the nwkConcentratorRadius.
*   
*
* Return value:
*   NONE.
*
************************************************************************************/
#if gConcentratorFlag_d
void ASL_SendRouteDiscoveryManyToOne
(
  uint8_t iRadius,
  bool_t  noRouteCacheFlag
)
{
  zdoNlmeMessage_t  *pZdoMsg;

  pZdoMsg = MSG_Alloc( sizeof( nlmeRouteDiscoveryReq_t )  + sizeof(zbMsgId_t));

  if (!pZdoMsg) 
    return;

  NlmeSetRequest(gNwkConcentratorRadius_c, iRadius);

  pZdoMsg->msgType = gNlmeRouteDiscReq_c;
  /* Many-to-one destination address mode is 0x00 */
	pZdoMsg->msgData.routeDiscoveryReq.dstAddrMode =	gZbAddrModeIndirect_c;
	Copy2Bytes(pZdoMsg->msgData.routeDiscoveryReq.aDstAddr,	gaBroadcastZCnZR);
  pZdoMsg->msgData.routeDiscoveryReq.noRouteCache = noRouteCacheFlag; 

    if (ZDO_NLME_SapHandler(pZdoMsg))
  {
    /*
      Handle error here.
    */
  }
}


      
#endif

/*****************************************************************************
* ASL_ZdoCallBack
*
* Default key handling. Handles all config mode keys.
*****************************************************************************/

void ASL_ZdoCallBack
  (
  zdpToAppMessage_t *MsgFromZDP,
  zbCounter_t MsgCounter
  )
{
  uint8_t event;

  (void)MsgCounter; /* eliminate compiler warnings */

  /* return the event */
  event = MsgFromZDP->msgType;

  /* if event indication, message type is the zdo2AppEvent */
  if(event == gzdo2AppEventInd_c) {
    event = MsgFromZDP->msgData.zdo2AppEvent;
#ifdef gHcGenericApp_d    
#if gHcGenericApp_d 
    if(event == gZDOToAppMgmtZCRunning_c || event == gZDOToAppMgmtZRRunning_c ||
       event == gZDOToAppMgmtZEDRunning_c)  {
      {
        zbClusterId_t  ClusterId={gaZclClusterGeneralTunnel_c};
        uint8_t  ProtocolAddress[sizeof(uint8_t) + sizeof(zbIeeeAddr_t)];
        ProtocolAddress[0] = sizeof(zbIeeeAddr_t);
        FLib_MemCpy(&ProtocolAddress[1], aExtendedAddress, sizeof(zbIeeeAddr_t));
        Swap8Bytes(&ProtocolAddress[1]);
        (void)ZCL_SetAttribute(endPointList[0].pEndpointDesc->pSimpleDesc->endPoint,
              ClusterId, gZclAttrGTProtoAddr_c, ProtocolAddress);    
      }      
    }
#endif
#endif 
#ifdef gHostApp_d
    if(event == gZDOToAppMgmtZCRunning_c || event == gZDOToAppMgmtZRRunning_c ||
       event == gZDOToAppMgmtZEDRunning_c)  
      {
        ZdoSaveAllDataSetsLocal();
      }
#endif 
  
  }

  if(event == gEnd_Device_Bind_rsp_c) {
    event = gBindingSuccess_c;
    gSendingNwkData.endPoint = MsgFromZDP->msgData.matchDescriptorResponse.matchList[0];
    if(MsgFromZDP->msgData.endDeviceBindResp.status != gZbSuccess_c )
      event = gBindingFailure_c;
#ifdef gHaThermostat_d    
#if gHaThermostat_d    
    else 
      CreateTempMeasurementBinding();
#endif     
#endif
   }

#if gMatch_Desc_req_d
  if (event == gMatch_Desc_rsp_c ) {
    if (MsgFromZDP->msgData.matchDescriptorResponse.status == gZbSuccess_c)
    {
      event = gMatchDescriptorSuccess_c;
      gSendingNwkData.NwkAddrOfIntrest[0] = MsgFromZDP->msgData.matchDescriptorResponse.aNwkAddrOfInterest[0];
      gSendingNwkData.NwkAddrOfIntrest[1] = MsgFromZDP->msgData.matchDescriptorResponse.aNwkAddrOfInterest[1];
      gSendingNwkData.endPoint = MsgFromZDP->msgData.matchDescriptorResponse.matchList[0];
    }
    else if (MsgFromZDP->msgData.matchDescriptorResponse.status == gZdoDeviceNotFound_c)
      event = gMatchNotFound_c;
    else
      event = gMatchFailure_c;
  }
#endif

#if gIEEE_addr_req_d
  if (event == gIEEE_addr_rsp_c ) {
    if (MsgFromZDP->msgData.extAddressResp.status == gZbSuccess_c)
    {
#if (gZclEnableOTAClient_d)
      InterpretOtaIEEEAddrReq(MsgFromZDP->msgData.extAddressResp.devResponse.singleDevResp.aIeeeAddrRemoteDev);	
#endif    	
    }
    else
    {
    	// TO DO
    }
  }
#endif  
  
  
  if (event == gNlmeTxReport_c)
  {
    FA_ProcessNlmeTxReport(&MsgFromZDP->msgData.nlmeNwkTxReport);
  }
  if (event == gMgmt_NWK_Update_notify_c)
  {/* Already handle in ZDP. */}

  if (event == gNlmeEnergyScanConfirm_c)
  {
    FA_ProcessEnergyScanCnf(&MsgFromZDP->msgData.energyScanConf);
  }
  if (event == gChannelMasterReport_c)
  {
      FA_SelectChannelAndChange();
  }

  /* free the message, we're done */
  
  if (MsgFromZDP->msgType == gNlmeEnergyScanConfirm_c)
  {
    MSG_Free(MsgFromZDP->msgData.energyScanConf.resList.pEnergyDetectList);
  }
  
  MSG_Free(MsgFromZDP);

  /* display the results of the event */
 // ASL_UpdateDevice(appEndPoint, event);
 BeeAppUpdateDevice(appEndPoint, event, 0, NULL, NULL); 
}
/******************************************************************************
*******************************************************************************
* Private functions
*******************************************************************************
******************************************************************************/


/******************************************************************************
*******************************************************************************
* Private Debug stuff
*******************************************************************************
******************************************************************************/
