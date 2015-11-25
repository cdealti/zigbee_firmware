/******************************************************************************
* This is a source file that handles ZDP primitives
*
* (c) Copyright 2008, Freescale, Inc. All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
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

#include "ZdpManager.h"
#include "BeeStackUtil.h"
#include "PWR_Interface.h"
#include "ZdpUtils.h"
#include "BeeStackParameters.h"
#include "ASL_ZdpInterface.h"
#include "ZdoSecurityManager.h"


#if gNWK_addr_rsp_d || gIEEE_addr_rsp_d
#define mSingleDevResp    0x00
#define mExtendedDevResp  0x01
#endif

#if gMgmt_NWK_Disc_rsp_d
#define gHalfByte_c 4
#endif

#if gMgmt_Direct_Join_rsp_d
extern zbNwkAddr_t  aSrcAddress;
#endif

extern bool_t gIsABroadcast;
extern bool_t  mbIsNotFree;

extern zbCounter_t  gZdpSequence;
extern zbCounter_t  gZdpResponseCounter;

#if (gReplace_Device_rsp_d || gRemove_Bkup_Bind_Entry_rsp_d || gBackup_Bind_Table_rsp_d || gStore_Bkup_Bind_Entry_rsp_d || gRecover_Bind_Table_rsp_d || gBind_Register_rsp_d || gMgmt_Bind_rsp_d)
extern zbApsmeBindEntry_t  gaBindingCacheTable[gMaximumBindingCacheTableList_c];
extern zbSize_t  gBindingCacheTableCounter;
#endif

/* ZigBee 1.1 Required response structure (system Server Dsicovery Response) */
#if gSystem_Server_Discovery_rsp_d || gStore_Bkup_Bind_Entry_rsp_d || gRemove_Bkup_Bind_Entry_rsp_d || gBackup_Bind_Table_rsp_d || gRecover_Bind_Table_rsp_d || gBackup_Source_Bind_rsp_d || gRecover_Source_Bind_rsp_d || gReplace_Device_rsp_d
zbSystemServerDiscoveryStore_t  mSystemServerDiscResponse[gMaximumSystemServerciesResponses_c]= {0};
index_t  mSystemServerDiscoveryResponsesCounter = 0;
#endif

#if gBkup_Discovery_cache_d || gDiscovery_store_rsp_d || gMgmt_Cache_rsp_d || gRemove_node_cache_rsp_d || gFind_node_cache_rsp_d|| gActive_EP_store_rsp_d || gPower_Desc_store_rsp_d || gNode_Desc_store_rsp_d || gSimple_Desc_store_rsp_d
extern discoveryStoreTable_t  gaDiscoveryStoreTable[gMaximumDiscoveryStoreTableSize_c];
extern zbCounter_t  mDiscoveryStoreTableEntrys;

#define Zdp_CacheTableNwkAddr(pAddr)  (pAddr)->aNwkAddress
#define Zdp_CacheTableIeeeAddr(pAddr)  (pAddr)->aIeeeAddress
#define Zdp_CacheTableEntryAddr(index)  (&gaDiscoveryStoreTable[(index)])
#define Zdp_CacheTableEntrySize(index)  (sizeof(gaDiscoveryStoreTable[(index)]))
#endif

#if gBind_Register_rsp_d || gRecover_Source_Bind_rsp_d || gBackup_Source_Bind_rsp_d || (gBindCapability_d && gBind_rsp_d)
extern zbIeeeAddr_t  aDevicesHoldingItsOwnBindingInfo[gMaximumDevicesHoldingBindingInfo_c];
extern zbCounter_t  gDeviceHoldingItsOwnBindingInfo_Counter;
#endif

#if (gBindCapability_d && (gBind_rsp_d || gUnbind_rsp_d)) || gReplace_Device_rsp_d
extern zbEventProcessQueue_t  maEventProcessQueue[gMaximumEventsToProcess_c];
#endif

#if gMgmt_NWK_Disc_rsp_d
extern index_t  gIndex;
extern bool_t  gMgmtNwkDiscReq;
extern index_t  gStartIndexforNwkDisc;
#endif

#if gMgmt_Leave_rsp_d
extern bool_t gMemoryFreedByApplication;
extern zbNwkAddr_t  mgmtLeaveResponseInfo;
#endif

#if gMatch_Desc_rsp_d || ((gCoordinatorCapability_d || gComboDeviceCapability_d )&& gBindCapability_d && gEnd_Device_Bind_rsp_d)
extern zbClusterId_t gaZdpMatchedOutCluster[];
extern index_t giMatchIndex;
#endif

#if ((gStandardSecurity_d || gHighSecurity_d) && gApsMaxEntriesForPermissionsTable_c)
extern bool_t CommandHasPermission
(
  zbNwkAddr_t aSrcAddr,
  permissionsFlags_t  permissionsCategory
);
#endif /* ((gStandardSecurity_d || gHighSecurity_d) && gApsMaxEntriesForPermissionsTable_c) */

#if gMgmt_Bind_rsp_d
zbSize_t  ZDP_MgmtBindFillupClusters
(
  zbApsmeBindEntry_t  *pListEntry,
  uint8_t  *pNumOfEntries,
  uint8_t *pBindingTableListCount,
  apsBindingTable_Ptr_t  *pPtr,
  uint8_t iClusterIndex
);

#if MC13226Included_d
    extern uint8_t gApsMaxClusterList;
#else
    extern const uint8_t gApsMaxClusterList;
#endif
#endif


#if gNode_Desc_rsp_d || gBind_Register_rsp_d
/*-------------------- gaServeMask --------------------
	Table 2.29 Server Mask Bit Assignments

	Bit number:
		0->Primary Trust Center.
		1->Backup Trust Center.
		2->Primary Binding table cache.
		3->Backup bindin table cache.
		4->Primary Discovery Cache.
		5->Backup Discovery Cache.
		6-15->Reserved.
*/
extern zbServerMask_t  gaServerMask;
#endif


/*========================================== UTILS ==========================================*/

#if gNWK_addr_rsp_d || gIEEE_addr_rsp_d
/*-------------------- NWK_addr_And_IEEE_addr_rsp --------------------
	This function is ZDP internal.

	Generates the response to the NWK_addr_req or IEEE_addr_req command.

	IN: The package with the request information.
	IN/OUT: The buffer where the request data will be filled in.
	IN: The Id of the request coming in.

	OUT: The size in bytes of the request payload.
*/
zbSize_t NWK_addr_And_IEEE_addr_rsp
(
	uint8_t  *pMessageComingIn,              /* IN: The package with the request information. */
	zbAddressResponse_t  *pMessageGoingOut,  /* IN/OUT: The buffer where the request data will be filled in. */
	zbMsgId_t  clusterId                     /* IN: The Id of the request coming in. */
)
{
	uint8_t  requestType;
	index_t  startIndex;
	zbCounter_t  cIndex;
	neighborTable_t  *pNeighborTable;
#if gDiscovery_store_rsp_d || gBkup_Discovery_cache_d
	uint8_t  auxIndex;
	discoveryStoreTable_t  *pPtr;
#endif

	if( clusterId==gNWK_addr_req_c )
	{
		requestType = ((zbNwkAddrRequest_t *)pMessageComingIn)->requestType;
		startIndex = ((zbNwkAddrRequest_t *)pMessageComingIn)->startIndex;
	}
	else
	{
		requestType = ((zbIeeeAddrRequest_t *)pMessageComingIn)->requestType;
		startIndex = ((zbIeeeAddrRequest_t *)pMessageComingIn)->startIndex;
	}
	/* build up the response */
	Copy2Bytes(pMessageGoingOut->devResponse.singleDevResp.aNwkAddrRemoteDev, NlmeGetRequest(gNwkShortAddress_c));
	Copy8Bytes(pMessageGoingOut->devResponse.singleDevResp.aIeeeAddrRemoteDev, NlmeGetRequest(gNwkIeeeAddress_c));
	if (requestType != mSingleDevResp && requestType != mExtendedDevResp)
	{
		/* if its not a single device or extended device then is an invalid request type */
			pMessageGoingOut->status = gZdoInvalidRequestType_c;
			return (sizeof(pMessageGoingOut->status)+sizeof(zbSingleDevResp_t));
	}
	/* by default we have an answer to send back, this state may change during the response build */
	/* The outgoing buffer is filled with zeros, so pMessageGoingOut->status = gZbSuccess_c; is no longer needed */
	/* We need to know if the request is an IEEE address request or a NWK address request for us or some one else */
	if (IsSelfIeeeAddress(pMessageComingIn) || ((clusterId == gIEEE_addr_req_c) && (IsSelfNwkAddress(pMessageComingIn))))
	{
		/* If it is a request for us and is for a single device answer then */
		if ( requestType == mSingleDevResp )  /*Request Type*/
		{
			/* build up the response and send back the size of the payload */
			return (sizeof(pMessageGoingOut->status) + sizeof(zbSingleDevResp_t));
		}
		/* if it is a request for us and it is a extended device answer then */
		else if( requestType == mExtendedDevResp )  /*Request Type*/
		{
			/* We must start form the index provided in the request */
			cIndex = startIndex;  /*StartIndex*/
			/* Build up the response */
			pMessageGoingOut->devResponse.extendedDevResp.startIndex = startIndex;
			pMessageGoingOut->devResponse.extendedDevResp.numAssocDev = 0x00;
			/* we need to find out how is our children so we take little sneak peak into the neighborTable */
			for( ; cIndex < gMaxNeighborTableEntries; cIndex++ )
			{
				pNeighborTable=NlmeGetRequestTableEntry(gNwkNeighborTable_c, cIndex);
				/* Once we get the info from the neighbor table we need to find out how is our child */
				if(pNeighborTable->deviceProperty.bits.relationship == gNeighborIsChild_c)
				{
					/* If yhe device form the neighbor table is our child, we must copy the short address into the list of adresses in the response */
					Copy2Bytes(pMessageGoingOut->devResponse.extendedDevResp.aNwkAddrAssocDevList[pMessageGoingOut->devResponse.extendedDevResp.numAssocDev],
										pNeighborTable->aNetworkAddress);
					/* The number of devices assosiated to us get's incremented */
					pMessageGoingOut->devResponse.extendedDevResp.numAssocDev++;
				}
			}
			/* If we did not get any device as our child the response must not include the start index and the list of Association Devices */
			if (!pMessageGoingOut->devResponse.extendedDevResp.numAssocDev)
				return (MbrOfs(zbAddressResponse_t, devResponse.extendedDevResp.startIndex));
			else
				/* If the response includes at leat one device as our child we must include de start index and the list with the devices */
				return ( MbrOfs(zbAddressResponse_t, devResponse.extendedDevResp.aNwkAddrAssocDevList[0]) + 
								( pMessageGoingOut->devResponse.extendedDevResp.numAssocDev * sizeof(zbNwkAddr_t)));
		}
	}
	else
	{
#if gDiscovery_store_rsp_d || gBkup_Discovery_cache_d
		if (ZdoGetPrimaryDiscoveryCacheBitMask(gaServerMask) || ZdoGetBackupDiscoveryCacheBitMask(gaServerMask))
		{
			if (clusterId == gIEEE_addr_req_c)
				auxIndex = ZDP_SearchNodeInCache(((zbIeeeAddrRequest_t *)pMessageComingIn)->aNwkAddrOfInterest, NULL, zbNwkMode);
			else
				auxIndex = ZDP_SearchNodeInCache(NULL,((zbNwkAddrRequest_t *)pMessageComingIn)->aIeeeAddr, zbNwkMode);

			pPtr = Zdp_CacheTableEntryAddr(auxIndex);
			if (auxIndex == gZbSuccess_c)
			{
				Copy2Bytes(pMessageGoingOut->devResponse.singleDevResp.aNwkAddrRemoteDev, Zdp_CacheTableNwkAddr(pPtr));
				Copy8Bytes(pMessageGoingOut->devResponse.singleDevResp.aIeeeAddrRemoteDev, Zdp_CacheTableIeeeAddr(pPtr));
				return (sizeof(pMessageGoingOut->status)+sizeof(zbSingleDevResp_t));
			}
		}
#endif
		/* If the request is to answer on behalf of other node we must search for it in our neighbor table */
		for(cIndex = 0 ; cIndex < gMaxNeighborTableEntries; cIndex++ )
		{
			pNeighborTable=NlmeGetRequestTableEntry(gNwkNeighborTable_c, cIndex);
			/* Once we get the info from the neighbor table we need to find out how is our child */
			if( gNeighborIsChild_c == pNeighborTable->deviceProperty.bits.relationship )
			{
				if (IsEqual8Bytes(pMessageComingIn, pNeighborTable->aExtendedAddress) ||
				((clusterId == gIEEE_addr_req_c) && (IsEqual2Bytes(pMessageComingIn, pNeighborTable->aNetworkAddress))))
				{
					Copy2Bytes(pMessageGoingOut->devResponse.singleDevResp.aNwkAddrRemoteDev, pNeighborTable->aNetworkAddress);
					Copy8Bytes(pMessageGoingOut->devResponse.singleDevResp.aIeeeAddrRemoteDev, pNeighborTable->aExtendedAddress);
					return (sizeof(pMessageGoingOut->status)+sizeof(zbSingleDevResp_t));
				}
			}
		}
		/* If we didn't find the device and the request was unicast we generate a DEVICE_NOT_FOUND response */
		if (!gIsABroadcast) 
		{
			pMessageGoingOut->status = gZdoDeviceNotFound_c;
			return (sizeof(pMessageGoingOut->status)+sizeof(zbSingleDevResp_t));
		}		
	}
	/* The defualt answer of a payload in zero won't send anything back over the air */
	return  gZero_c;
}
#endif

#if gComplex_Desc_rsp_d
/*-------------------- Complex_Desc_rsp --------------------
	2.4.4.1.8 Complex_Desc_rsp. (ClusterID=0x8010)

	The Complex_Desc_rsp is generated by a remote device in response to a
	Complex_Desc_req directed to the remote device. This command shall be unicast
	to the originator of the Complex_Desc_req command.

	IN: The package with the request information.
	IN/OUT: The buffer where the response data will be filled in.

	OUT: The size in bytes of the response payload.
*/
zbSize_t Complex_Desc_rsp
(
	zbComplexDescriptorRequest_t *pMessageComingIn,  /* IN: The package with the request information. */
	zbComplexDescriptorResponse_t *pMessageGoingOut  /* IN/OUT: The buffer where the response data will be filled in. */
)
{
	Copy2Bytes(pMessageGoingOut->aNwkAddrOfInterest,pMessageComingIn->aNwkAddrOfInterest);
	if (IsSelfNwkAddress(pMessageComingIn->aNwkAddrOfInterest))
	{
		if (gComplexDesc.fieldCount)
		{
			pMessageGoingOut->status = gZbSuccess_c;
			pMessageGoingOut->length = sizeof(zbComplexDescriptor_t);
			FLib_MemCpy( &pMessageGoingOut->complexDescrip,&gComplexDesc,sizeof(zbComplexDescriptor_t));
			return (sizeof(zbComplexDescriptorResponse_t));
		}
		else
		{
			pMessageGoingOut->status = gZdoNoDescriptor_c;
			pMessageGoingOut->length = 0;
			return (MbrOfs(zbComplexDescriptorResponse_t, complexDescrip));
		}
	}
	else
	{
		pMessageGoingOut->status = gZdoInvalidRequestType_c;
		pMessageGoingOut->length = 0;
		return (MbrOfs(zbComplexDescriptorResponse_t, complexDescrip));
	}
}
#endif

#if gNode_Desc_rsp_d || gPower_Desc_rsp_d || gUser_Desc_rsp_d
/*-------------------- Node_Desc_Power_Desc_And_User_Desc_rsp --------------------
	This function is ZDP internal.

	Genertes the response for the Node Descriptro, Power Descriptor and
	User Descriptor.

	IN: The package with the request information.
	IN/OUT: The buffer where the response data will be filled in.
	IN: The Id of the request coming in.

	OUT: The size in bytes of the request payload.
*/
zbSize_t Node_Desc_Power_Desc_And_User_Desc_rsp
(
  uint8_t  *pMessageComingIn,  /* IN: The package with the request information. */
  uint8_t  *pMessageGoingOut,  /* IN/OUT: The buffer where the response data will be filled in. */
  uint8_t  clusterId           /* IN: The Id of the request coming in. */
)
{
  uint8_t *pData;
  zbNodeDescriptor_t * pNodeDescriptor=NULL;
#if ((gPower_Desc_store_rsp_d || gNode_Desc_store_rsp_d) && ( gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d))
  index_t  cIndex;
  discoveryStoreTable_t  *pPtr;
#endif
  (void)pNodeDescriptor;


  pMessageGoingOut[0] = gZbSuccess_c;
  Copy2Bytes(&pMessageGoingOut[1],pMessageComingIn);

  pData = pMessageGoingOut + (sizeof(zbStatus_t) + sizeof(zbNwkAddr_t));

  switch(clusterId)
  {
#if gNode_Desc_rsp_d
    case gNode_Desc_req_c:
      if (IsSelfNwkAddress(((zbNodeDescriptorRequest_t *)pMessageComingIn)->aNwkAddrOfInterest))
      {
        /* Creating the Node Descriptor field in the response frame, including: */
        pNodeDescriptor = (zbNodeDescriptor_t *)pData;

        /* -> Logical Type. */
        pNodeDescriptor->logicalType = NlmeGetRequest(gDevType_c);

        /* -> Frequency band used by the radio. */
        pNodeDescriptor->apsFlagsAndFreqBand = gBeeStackConfig.apsFlagsAndFreqBand;

        /* -> MAC capability Flag. */
        pNodeDescriptor->macCapFlags = NlmeGetRequest(gNwkCapabilityInformation_c);

        /* -> Manufacturer Code Flag. */
        Copy2Bytes(pNodeDescriptor->aManfCodeFlags,gBeeStackConfig.manfCodeFlags);

        /* -> Maximum buffer Size. This is the worst case scenario. */
        pNodeDescriptor->maxBufferSize = GetMaxApplicationPayload(NULL);

        /* -> The server Mask to be sended in the request payload. */
        Copy2Bytes(pNodeDescriptor->aServerMask,gaServerMask);

        /* -> Default Value of Maximum Outgoing Transfer Size. Worst case */
        pNodeDescriptor->aMaxOutTransferSize[0] = GetMaxApplicationPayload(NULL);
        Copy2Bytes(pNodeDescriptor->aMaxTransferSize,gBeeStackConfig.maxTransferSize);

        /* -> The descriptor capability field. */
        pNodeDescriptor->descCapField = gBeeStackConfig.descCapField;
        return (sizeof( zbNodeDescriptorResponse_t ));
      }
#if (gNode_Desc_store_rsp_d && ( gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d))
      else
      {
#if gComboDeviceCapability_d
        if ((NlmeGetRequest(gDevType_c) == gEndDevice_c) && !IsLocalDeviceReceiverOnWhenIdle())
        {
          /* ZEDs aren't supoused to have Node descriptors of any kind. Only RxOnWhenIdle devices. */
          pMessageGoingOut[0] = gZdoInvalidRequestType_c;
          return (MbrOfs(zbNodeDescriptorResponse_t, nodeDescriptor));
        }
#endif
        cIndex = ZDP_SearchNodeInCache(((zbNodeDescriptorRequest_t *)pMessageComingIn)->aNwkAddrOfInterest, NULL,zbNullMode);
        pPtr = &gaDiscoveryStoreTable[cIndex];
        if (cIndex != gZdoNoEntry_c)
        {
          if (pPtr->nodeDescriptor.logicalType)
          {
            FLib_MemCpy(pData, &(pPtr->nodeDescriptor), sizeof(zbNodeDescriptorResponse_t));
            return  (sizeof( zbNodeDescriptorResponse_t ));
          }
          else
            pMessageGoingOut[0] = gZdoNoDescriptor_c;
        }
        else
          pMessageGoingOut[0] = gZdoDeviceNotFound_c;
      }
#else
      pMessageGoingOut[0] = gZdoInvalidRequestType_c;
#endif
      return (MbrOfs(zbNodeDescriptorResponse_t, nodeDescriptor));

/* iar warning of statement unreachable */
#ifndef __IAR_SYSTEMS_ICC__
    break;
#endif

#endif

#if gPower_Desc_rsp_d
    case gPower_Desc_req_c:
      if (IsSelfNwkAddress(((zbPowerDescriptorRequest_t *)pMessageComingIn)->aNwkAddrOfInterest))
      {
        /* Cretate the Power Descriptor field in the response frame */
        Copy2Bytes(pData, &gBeeStackConfig.currModeAndAvailSources);
        if(!IsLocalDeviceReceiverOnWhenIdle())
        {
          ((zbPowerDescriptor_t *)pData)->currModeAndAvailSources |= 0x01;
        }

        ((zbPowerDescriptor_t *)pData)->currPowerSourceAndLevel &= 0x0F;
        /*power source is lower nibble*/
        ((zbPowerDescriptor_t *)pData)->currPowerSourceAndLevel |= Zdp_PowerSourceLevel();
        return (sizeof( zbPowerDescriptorResponse_t ));
      }
#if (gPower_Desc_store_rsp_d && ( gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d))
      else
      {
#if gComboDeviceCapability_d
        if (NlmeGetRequest(gDevType_c) == gEndDevice_c && !IsLocalDeviceReceiverOnWhenIdle())
        {
          pMessageGoingOut[0] = gZdoInvalidRequestType_c;
          return (MbrOfs(zbPowerDescriptorResponse_t, powerDescriptor));
        }
#endif
        cIndex = ZDP_SearchNodeInCache(((zbPowerDescriptorRequest_t *)pMessageComingIn)->aNwkAddrOfInterest, NULL, zbNullMode);
        pPtr= &gaDiscoveryStoreTable[cIndex];
        if (cIndex != gZdoNoEntry_c)
        {
          if ((pPtr->powerDescriptor.currModeAndAvailSources != 0) && (pPtr->powerDescriptor.currPowerSourceAndLevel != 0))
          {
            FLib_MemCpy(pData, &(pPtr->powerDescriptor), gPowerDescriptorSize);
            return (sizeof( zbPowerDescriptorResponse_t ));
          }
          else
            pMessageGoingOut[0] = gZdoNoDescriptor_c;
        }
        else
          pMessageGoingOut[0] = gZdoDeviceNotFound_c;
      }
#else
      pMessageGoingOut[0] = gZdoInvalidRequestType_c;
#endif
      return (MbrOfs(zbPowerDescriptorResponse_t, powerDescriptor));

/* iar warning of statement unreachable */
#ifndef __IAR_SYSTEMS_ICC__
    break;
#endif

#endif

#if gUser_Desc_rsp_d
    case  gUser_Desc_req_c:
      if (IsSelfNwkAddress(((zbUserDescriptorRequest_t *)pMessageComingIn)->aNwkAddrOfInterest))
      {
        pData[0] = gUserDesc.length;
        FLib_MemCpy(pData + sizeof(gUserDesc.length), gUserDesc.aUserDescription, sizeof(gUserDesc.aUserDescription));
        return (sizeof( zbUserDescriptorResponse_t ));
      }
      else
      {
        pMessageGoingOut[0] = gZdoInvalidRequestType_c;
        return (MbrOfs(zbUserDescriptorResponse_t, descriptor));
      }
/* marked as unreachable by IAR */
#ifndef __IAR_SYSTEMS_ICC__
    break;
#endif

#endif

    default:
    break;
  }

  return gZero_c;
}
#endif

#if gNode_Desc_store_rsp_d || gPower_Desc_store_rsp_d || gActive_EP_store_rsp_d || gSimple_Desc_store_rsp_d
/*-------------------- Descriptor_Store_rsp --------------------
	This functions is ZDP internal.

	Stores the data for Node_Desc_store, Power_Desc_store, Active_EP_store
	and Simple_Desc_store.

	IN: The ID of the incoming request.
	IN/OUT: The buffer where the response data will be filled in.
	IN: The size in bytes of the descriptor to store.
	IN: The descriptor to store.
	IN: The short address of the requesting device.

	OUT: The size in bytes of the request payload.
*/
zbSize_t Descriptor_Store_rsp
(
	zbMsgId_t  clusterId,        /* IN: The ID of the incoming request. */
	uint8_t  *pMessageGoingOut,  /* IN/OUT: The buffer where the response data will be filled in. */
	zbCounter_t  count,          /* IN: The size in bytes of the descriptor to store. */
	uint8_t  *pDataToStore,      /* IN: The descriptor to store. */
	zbNwkAddr_t  aNwkAddress     /* IN: The short address of the requesting device. */
)
{
	uint8_t  cIndex;
	discoveryStoreTable_t  *pPtr;


  if (!ZdoGetPrimaryDiscoveryCacheBitMask(gaServerMask))
  {
    *(pMessageGoingOut) = gZdoNotSupported_c;
    return (sizeof(zbStatus_t));
  }

	cIndex =ZDP_SearchNodeInCache(aNwkAddress, NULL, zbSrcAddressMode);
	if (cIndex == gZdoNoEntry_c)
	{
		*(pMessageGoingOut) = gZdoNotPermited_c;
		return (sizeof(zbStatus_t));
	}
	pPtr = &gaDiscoveryStoreTable[cIndex];
	switch(clusterId)
	{
		case gSimple_Desc_store_req_c:
			if (pPtr->simpleDescriptorCount < gMaximumInClusterStore_c)
			{
				FLib_MemCpy(&pPtr->simpleDescriptorList[pPtr->simpleDescriptorCount],
										pDataToStore, 
										MbrOfs(cSimpleDescriptor_t, appNumInClusters));
				pDataToStore += MbrOfs(cSimpleDescriptor_t, appNumInClusters);
				pPtr->simpleDescriptorList[pPtr->simpleDescriptorCount].appNumInClusters = *pDataToStore;

				pDataToStore += MbrSizeof(cSimpleDescriptor_t, appNumInClusters);
				FLib_MemCpy(pPtr->simpleDescriptorList[pPtr->simpleDescriptorCount].appInClusterList,
										pDataToStore,
										pPtr->simpleDescriptorList[pPtr->simpleDescriptorCount].appNumInClusters * sizeof(zbClusterId_t));
				pDataToStore += pPtr->simpleDescriptorList[pPtr->simpleDescriptorCount].appNumInClusters * sizeof(zbClusterId_t);
				pPtr->simpleDescriptorList[pPtr->simpleDescriptorCount].appNumOutClusters = *pDataToStore;
				pDataToStore += MbrSizeof(cSimpleDescriptor_t, appNumOutClusters);
				FLib_MemCpy(pPtr->simpleDescriptorList[pPtr->simpleDescriptorCount].appOutClusterList, pDataToStore, pPtr->simpleDescriptorList[pPtr->simpleDescriptorCount].appNumOutClusters * sizeof(zbClusterId_t));
				pPtr->simpleDescriptorCount++;
			}
		break;
		case gActive_EP_store_req_c:
			if (count <= gMaximumActiveEPStoredList_c)
			{
				FLib_MemCpy(&pPtr->activeEPList, pDataToStore, count);
				pPtr->activeEPCount = count;
			}
			else
			{
				FLib_MemCpy(&pPtr->activeEPList, pDataToStore, gMaximumActiveEPStoredList_c);
				pPtr->activeEPCount = gMaximumActiveEPStoredList_c;
			}
		break;
		case gNode_Desc_store_req_c:
			FLib_MemCpy(&pPtr->nodeDescriptor,pDataToStore, count);
		break;
		case gPower_Desc_store_req_c:
			FLib_MemCpy(&pPtr->powerDescriptor,pDataToStore, count);
		break;
	}
	*(pMessageGoingOut) = gZbSuccess_c;
	return(sizeof(zbStatus_t));
}
#endif

/*==============================================================================================
	=========== 2.4.4.2 End Device Bind, Bind, Unbind Bind Management Server Services ============
	==============================================================================================*/
#if (gBind_rsp_d || gUnbind_rsp_d) && gBindCapability_d
/*-------------------- Bind_Unbind_rsp ---------------------
	This function is ZDP internal.

	Generates the response to the ZDO Bind or UnBind request (same command, different cluster).
	Bind Response (ClsuterId=0x8021), refer to section 2.4.4.2.2 of the ZigBee Spec.
	UnBind Response (ClusterId=0x8022), refer to the section 2.4.4.2.3 of the ZigBee Spec.

	IN: The command ID of the incoming request.
	IN/OUT: The buffer where the response data will be filled in (status only).
	IN: The package with the request information.
	IN: The short address of the requesting device.

	OUT: The size in bytes of the response payload.
*/
zbSize_t Bind_Unbind_rsp
(
	zbMsgId_t  clusterId,                      /* IN: The command ID of the incoming request. */
	zbStatus_t  *pMessageGoingOut,             /* IN/OUT: The buffer where the response data will be filled in (status only). */
	zbBindUnbindRequest_t  *pMessageComingIn,  /* IN: The package with the request information. */
	zbNwkAddr_t  SrcAddress                    /* IN: The short address of the requesting device. */
)
{
  /*
    In the case of failure, return Invalid request by default.
  */
  zbStatus_t  bindUnbindStatus = gZdoInvalidRequestType_c;
  uint8_t aNwkAddr[2];
#if gBind_Register_rsp_d  
  index_t  cIndex = gZdoNoEntry_c;
#endif
  /*
    If is not an in-node command, verify for authorization in the permissions table.
  */
#if ((gStandardSecurity_d || gHighSecurity_d) && gApsMaxEntriesForPermissionsTable_c)  
  if (!IsSelfNwkAddress(SrcAddress))
  {    
    if (!CommandHasPermission(SrcAddress, gApplicationSettingsPermission_c))
    {
      pMessageGoingOut[0] = gZdpNotAuthorized_c;
      return (sizeof(zbStatus_t));
    }
  }
#endif /* ((gStandardSecurity_d || gHighSecurity_d) && gApsMaxEntriesForPermissionsTable_c) */

  /*
    If the current bind or unbind req, has the soruce address set to the local device
    that means that the request must be processed locally there is nothing to send OTA.
  */
  if (IsSelfIeeeAddress(pMessageComingIn->aSrcAddress))
  {
    /*
      If the current request is an Unbind, just process it and send the response as
      soon as possible.
    */
    if (clusterId == gUnbind_req_c)
      bindUnbindStatus = APSME_UnbindRequest((zbApsmeUnbindReq_t *)pMessageComingIn);
    else
    {
      /*
        If the bind request is to a group address just process ti we asume that is a
        valid groups, there is no way to know or register a group.
      */
      if (pMessageComingIn->addressMode == zbGroupMode)
        bindUnbindStatus = APSME_BindRequest((zbApsmeUnbindReq_t *)pMessageComingIn);

      /*
        If is is a bind request and is a 64-bit address mode then lets see if we get to
        know the device, if it so, then bind to the device.
      */
      else if (pMessageComingIn->addressMode == zbIEEEMode)
      {
        if (APS_GetNwkAddress(pMessageComingIn->destData.extendedMode.aDstAddress, aNwkAddr) != NULL)
          bindUnbindStatus = APSME_BindRequest((zbApsmeUnbindReq_t *)pMessageComingIn);
        else
        {
          /*
            Try to start the bind state machine, if the state machine was able to start
            send zero to ZDP, so ZDP does not send a repsonse right now, it will wait
            until the state machine ends its porcess, either fails or success.
          */
          bindUnbindStatus = ZDP_SetRegisterMachineData(clusterId, SrcAddress, pMessageComingIn);
          if (bindUnbindStatus != gZbSuccess_c)
          {
            /*
              If the state machine was unable to allocate the entry, then send an error
              message as soon as possible.
            */
            bindUnbindStatus = gZdoNoEntry_c;
          }
          else
          {
            /*
              The destination node is unknow to us, we should free the big buffer allocated
              in ZDP, to avoid wating memory.
            */
            MSG_Free((uint8_t *)pMessageGoingOut - SapHandlerDataStructureOffSet);

            /*
              Let ZDP knows that the buffer is already Free, so it does not try to free again.
            */
            mbIsNotFree = FALSE;
            return gZbSuccess_c;
          }
        }
      }
    }
  }
  /*
    ZDP handles the bind or unbind request to be done as a broadcast. NOT ZIGBEE, this
    is a Freescale Requirenment.
  */
  else if (Cmp8BytesToFs(pMessageComingIn->aSrcAddress))
  {
    Copy8Bytes(pMessageComingIn->aSrcAddress, NlmeGetRequest(gNwkIeeeAddress_c));
    if (clusterId == gUnbind_req_c)
      bindUnbindStatus = APSME_UnbindRequest((zbApsmeUnbindReq_t *)pMessageComingIn);
    else
      bindUnbindStatus = APSME_BindRequest((zbApsmeUnbindReq_t *)pMessageComingIn);
  }

  /*
    The Bind register handle code.
  */
#if gBind_Register_rsp_d
  /*
    If the device is not registered to keep external binding info, it should reject the request.
  */
  else if (!ZdoGetPrimaryBindingTableCacheBitMask(gaServerMask))
  {
    bindUnbindStatus = gZdoNotSupported_c;
  }
  else
  {
    /*
      First it must check if the device info is alredy register as one of the devices
      previously backing up its binding info here, in the cache device.
    */
    cIndex = ZDP_SearchDeviceHoldingItsOwnBindingInfo(pMessageComingIn->aSrcAddress);

    /*
      The device was not found as a previous register device.
    */
    if (cIndex == gZdoNoEntry_c)
    {
      /*
        The device is not register but it was an unbind request, free the current bind
        entries with the given info.
      */
      if (clusterId == gUnbind_req_c)
      {
        bindUnbindStatus = Zdp_RemoveBindCache((zbApsmeBindReq_t *)pMessageComingIn);
      }
      else
      {
        /*
          We do not know about this device, so it means that the devices whishes to
          backup the info.
        */
        if (!ZDP_AddToBindingCacheTable(pMessageComingIn))
          bindUnbindStatus = gZdoTableFull_c;
        else
         bindUnbindStatus = gZbSuccess_c;
      }
    }
    /*
      We already have the devices registered with us, let the devices know about the
      update in the info.
    */
    else
    {
      /*
        Let all the bound devices know about the changes in the information.
      */
      MSG_Free((uint8_t *)pMessageGoingOut - SapHandlerDataStructureOffSet);

      /*
        We are going to use the state machine, so free the big buffer and let the state
        machine handle the messages. Inform ZDP that the buffer is already free.
      */
      mbIsNotFree = FALSE;

      /*
        Use the state machine to propagate the information.
      */
      bindUnbindStatus = ZDP_SetRegisterMachineData(gInformDevices_c, SrcAddress, pMessageComingIn);
      if (bindUnbindStatus == gZbSuccess_c)
        return gZbSuccess_c;
      else
        bindUnbindStatus = gZdoNoEntry_c;
    }
  }
#endif

  /*
    ZDP Needs to translate the error codes form APS, in this way the proper error code
    goes OTA, and the sniffer can see it correctly.
  */
  switch (bindUnbindStatus)
  {
    case gApsIllegalRequest_c:
      bindUnbindStatus = gZdoInvalidRequestType_c;
    break;
    case gApsInvalidBinding_c:
      bindUnbindStatus = gZdoNoEntry_c;
    break;
    case gApsBindingTableFull_c:
    case gApsTableFull_c:
      bindUnbindStatus = gZdoTableFull_c;
    break;
  }

  /*
    The bind & unbind response is only one byte, set the proper status and return the
    size of the current response.
  */
  pMessageGoingOut[0] = bindUnbindStatus;
  return (sizeof(zbStatus_t));
}
#endif

#if gMgmt_Bind_rsp_d
/*-------------------- CountEntriesInTable ------------------
	This function is ZDP internal.

	This function can search in two different tables and return
	the amount of active entries.

	IN: The table where the search will be perform.
	IN: The type of table where to search in.

	OUT: The amount of active entries foun in the table.
*/
zbCounter_t CountEntriesInTable
(
	uint8_t  *pTable,  /* IN: The table where the search will be perform. */
	bool_t  tableType  /* IN: The type of table where to search in. */
)
{
  index_t  limit;
  index_t  cIndex;
  uint8_t  iSizeToAdd;
  zbCounter_t  counter =0;

  limit = (tableType)? gMaximumBindingCacheTableList_c : gApsMaxBindingEntries;

  /* The aps table is defined in user space, it will alwyas have the right size on compile time. */
  iSizeToAdd = (tableType)? sizeof(zbApsmeBindEntry_t) : sizeof(apsBindingTable_t);

  for (cIndex = 0; cIndex < limit; cIndex++)
  {
    if ( (!(ZdpBindIsFree(*((zbApsmeBindEntry_t *)pTable)))) || (!(ZdpBindIsFree(*((apsBindingTable_t *)pTable)))))
    {
      counter++;
    }
    pTable = pTable + iSizeToAdd;
  }

  return (counter);
}
#endif

#if gMgmt_Bind_rsp_d
/*-------------------- RetriveBindingCacheIndex ------------------
	This function is ZDP internal.

	Search in the binding table or the bindin cache table and returns the
	real index inthe table based on the ocurrence of the entries expressed
	by the "requestIndex" paramater.

	IN: The ocurrence of the entries to be transformed to an index.
	IN: The type of tabe where we must look for the ocurrence.

	OUT: NONE
*/
bindIndex_t RetriveBindingCacheIndex
(
	index_t  requestIndex,  /* IN: The ocurrence of the entries to be transformed to an index. */
	bool_t  tableType       /* IN: The type of tabe where we must look for the ocurrence. */
)
{
	index_t  i;
	index_t  limit;

	if (tableType)
		limit = gMaximumBindingCacheTableList_c;
	else
		limit = gApsMaxBindingEntries;

	for (i = requestIndex; i < limit; i++)
	{
		if (tableType)
		{
			if (ZdpBindIsFree(gaBindingCacheTable[i]))
				continue;
		}
		else
		{
			if (ZdpBindIsFree(gpBindingTable[i]))
				continue;
		}

		if (i >= requestIndex)
			return i;
	}
	return gZdoNoEntry_c;
}
#endif


#if gMgmt_Bind_rsp_d
zbSize_t ZDP_MgmtBindResponse_BindTable
(
  zbIndex_t startIndex,
  zbMgmtBindResponse_t *pMgmtBindRsp
)
{
  zbIndex_t index;
  uint8_t  i;
  uint8_t iNumOfEntries;
  apsBindingTable_Ptr_t  *pPtr;
  zbApsmeBindEntry_t  *pListEntry;
  uint8_t  listSize = 0;
  uint8_t iClusterIndex;
  bindIndex_t iBindIndex;
  uint8_t totalBindEntries = gApsMaxClusterList * gApsMaxBindingEntries;

  /* See if the requested index is available or get the next one. */
  index = Zdp_GetTranslateToBindIndexAndCluster(startIndex, &iClusterIndex, &iBindIndex);

  /*
    If the request index is higher than the actual size of the table or the index found is
    empty then leave with success.
  */
  if ((startIndex > totalBindEntries) || (index == gZdoNoEntry_c))
  {
    /* Even if there is no entry to return, the status must be success (2.4.4.3.4 r17) */
    return (0);
  }

  pMgmtBindRsp->startIndex = index;

  /* Catch the maximum number of entries to include in the payload list */
  iNumOfEntries = ZdpMaximumEntriesInPayLoad(MbrOfs(zbMgmtBindResponse_t, aBindingTableList), sizeof(zbApsmeBindEntry_t));

  /*
    Walk the whole table to count the amount of valid entries and record the list using the
    requested index.
  */
  for (i = 0; i < gApsMaxBindingEntries; i++)
  {
    pPtr = (void *)&gpBindingTable[i];

    /* Only take care of valid entries. */
    if (!pPtr->srcEndPoint)
    {
      continue;
    }

    /* We found a valid entry which it correspond to an entry per cluster bound. */
    pMgmtBindRsp->bindingTableEntries += pPtr->iClusterCount;

    /*
      We are taking care of the entries beyond the requested index, and while we got then
      fill the message going out.
    */
    if ((i >= iBindIndex) && (iNumOfEntries))
    {
      pListEntry = (void *)((uint8_t *)&pMgmtBindRsp->aBindingTableList + listSize);
      listSize += ZDP_MgmtBindFillupClusters(pListEntry,&iNumOfEntries,&pMgmtBindRsp->bindingTableListCount, pPtr, iClusterIndex);
    }
  }

  return listSize;
}

bindIndex_t Zdp_GetTranslateToBindIndexAndCluster
(
  zbIndex_t startIndex,
  uint8_t *pClusterIndex,
  bindIndex_t *pBindIndex
)
{
  uint8_t i;
  apsBindingTable_Ptr_t  *pBindEntry;
  uint8_t clCount = 0;
  uint8_t bindIndexTemp = 0;
  uint8_t INeed =0;

  /*
    if startIndex is equals to zero then return the first valid bind entry and its
    clusterId as zero no matter what bind entry phisically is in an index different to zero.
  */
  if(!startIndex)
  {
    *pClusterIndex = 0;
    *pBindIndex = 0;
    return startIndex;
  }

  for (i = 0; i < gApsMaxBindingEntries; i++)
  {
    pBindEntry = (void *)&gpBindingTable[i];
    *pBindIndex = i;

    /* If the bind entry is not valid then don't process it */
    if (!pBindEntry->srcEndPoint)
    {
      continue;
    }

    clCount += pBindEntry->iClusterCount;

    if( startIndex > (clCount-1))
    {
      bindIndexTemp = i;
      *pClusterIndex = (uint8_t)((clCount-1) % gApsMaxClusterList);
      INeed = startIndex - (clCount - 1);
    }
    else
    {
      if(!INeed)
        *pClusterIndex = startIndex;
      else
        *pClusterIndex = (INeed - 1);
      return startIndex;
    }
  }

  *pBindIndex = bindIndexTemp;
  /* Only if have found something on the Bind table we will update the starting index. */
  if (clCount != 0)
  {
    startIndex = (clCount-1);
  }
  return startIndex;
}

zbSize_t ZDP_MgmtBindResponse_CacheTable
(
  zbIndex_t startIndex,
  zbMgmtBindResponse_t *pMgmtBindRsp
)
{
  zbIndex_t  index;
  uint8_t iNumOfEntries;
  zbApsmeBindEntry_t  *pListEntry;
  zbApsmeBindEntry_t  *pPtr;
  uint8_t i;
  uint8_t listSize = 0;

  /* See if the requested index is available or get the next one. */
  index = RetriveBindingCacheIndex(startIndex, TRUE);

  if ((startIndex > gMaximumBindingCacheTableList_c) || (index == gZdoNoEntry_c))
  {
    /* Even if there is no entry to return, the status must be success (2.4.4.3.4 r17) */
    return (0);
  }

  /* Catch the maximum number of entries to include in the payload list */
  iNumOfEntries = ZdpMaximumEntriesInPayLoad(MbrOfs(zbMgmtBindResponse_t, aBindingTableList), sizeof(zbApsmeBindEntry_t));

  pListEntry = (void *)&pMgmtBindRsp->aBindingTableList;

  pMgmtBindRsp->startIndex = index;

  for (i = 0; i< gMaximumBindingCacheTableList_c; i++)
  {
    pPtr = (void *)&gaBindingCacheTable[i];

    /*
      In the same way as the binding table, the cahce table uses the src endpoint set to
      zero to indicate empty space.
    */
    if (!pPtr->srcEndPoint)
      continue;

    /* We found a valid entry which it correspond to an entry per cluster bound. */
    pMgmtBindRsp->bindingTableEntries ++;

    if ((i >= index) && iNumOfEntries)
    {
      /* Address group is special, and generates shorter entries. */
      if (pPtr->dstAddrMode != gZbAddrModeGroup_c)
      {
        FLib_MemCpy(pListEntry, pPtr, sizeof(zbApsmeBindEntry_t));
        pListEntry = (void *)((uint8_t *)pListEntry + sizeof(zbApsmeBindEntry_t));
        listSize += sizeof(zbApsmeBindEntry_t);
      }
      else
      {
        FLib_MemCpy(pListEntry, pPtr, MbrOfs(zbApsmeBindEntry_t, aDstAddr) + sizeof(zbGroupId_t));
        pListEntry = (void *)(MbrOfs(zbApsmeBindEntry_t, aDstAddr) + sizeof(zbGroupId_t));
        listSize += (MbrOfs(zbApsmeBindEntry_t, aDstAddr) + sizeof(zbGroupId_t));
      }
      pMgmtBindRsp->bindingTableListCount++;
      iNumOfEntries--;
    }
  }

  return listSize;
}

zbSize_t  ZDP_MgmtBindFillupClusters
(
  zbApsmeBindEntry_t  *pListEntry,
  uint8_t  *pNumOfEntries,
  uint8_t *pBindingTableListCount,
  apsBindingTable_Ptr_t  *pPtr,
  uint8_t iClusterIndex
)
{
  uint8_t i;
  uint8_t  listSize = 0;

  /* For each cluster on the Bind table entry generate an entry on Mgmt_Response.*/
  for (i = iClusterIndex; i < pPtr->iClusterCount; i++)
  {
    /* If no more entries can fit leave. */
    if (!(*pNumOfEntries))
      return listSize;

    /* For each entry with cluster fill the static data. */
    pListEntry->srcEndPoint = pPtr->srcEndPoint;
    Copy8Bytes(pListEntry->aSrcAddr, NlmeGetRequest(gNwkIeeeAddress_c));

    /* Get the right detination address mode and destination address from the local table. */
    pListEntry->dstAddrMode = gZbAddrMode64Bit_c;
    if (pPtr->dstEndPoint == gApsBindAddrModeGroup_c)
    {
      Copy2Bytes(pListEntry->aDstAddr, pPtr->dstAddr.aGroupId);
      pListEntry->dstAddrMode = gZbAddrModeGroup_c;
    }
    else
    {
      /*
        Use the previously registered Address map index to get the right IEEE address.
        NOTE: If the the bind entry exist, but the addressmap entry does not then we have
        a BIG error. Tables out of sync.
      */
      zbAddressMap_t addrMapEntry;
      (void)AddrMap_GetTableEntry(pPtr->dstAddr.index, &addrMapEntry);
      Copy8Bytes(pListEntry->aDstAddr, addrMapEntry.aIeeeAddr);

      /* Get the destination End point and one entry per cluster */
      pListEntry->dstEndPoint = pPtr->dstEndPoint;
    }

    /* Copy only one cluster per entry. */
    Copy2Bytes(pListEntry->aClusterId, pPtr->aClusterList[i]);

    /* Move the list pointer ot access the next valid entry */
    if (pPtr->dstEndPoint != gApsBindAddrModeGroup_c)
    {
      pListEntry = (void *)((uint8_t *)pListEntry + sizeof(zbApsmeBindEntry_t));
      listSize += sizeof(zbApsmeBindEntry_t);
    }
    else
    {
      listSize += MbrOfs(zbApsmeBindEntry_t,aDstAddr) + sizeof(zbClusterId_t);
      pListEntry = (void *)((uint8_t *)pListEntry + MbrOfs(zbApsmeBindEntry_t,aDstAddr) + sizeof(zbClusterId_t));
    }

    /* We got one more valid entry to send out OTA. */
    ++*pBindingTableListCount;

    /* We got one entry less to process. */
    --*pNumOfEntries;
  }

  return listSize;
}
#endif

#if gMatch_Desc_req_d || gSimple_Desc_req_d || gEnd_Device_Bind_req_d
/*-------------------- ZDP_CopyClusterListPairToFrame --------------------
	This functions is ZDP internal.

	Thi functions copies two elements of the type zbPtrClusterList_t, 
	this two elements need to be follwed one afer the other in memory or in the structure.
	It relays in the fact that only are two.

	IN: The buffer where the list must be copied in.
	IN: A pointer to first list of the zbPtrClusterList type.

	OUT: The size in bytes of the total of copied data.
*/
zbSize_t ZDP_CopyClusterListPairToFrame
(
	uint8_t  *pMessageGoingOut,            /* IN: The buffer where the list must be copied in. */
	zbPtrClusterList_t  *pClusterListPair  /* IN: A pointer to first list of the zbPtrClusterList type. */
)
{
	index_t  cIndex;
	zbCounter_t  cCount;
	zbSize_t  iSize = 0;
	uint8_t  clusterSize;

	/* copy both input and output cluster list */
	for(cIndex = 0; cIndex < 2; cIndex++)
	{
		cCount = pClusterListPair[cIndex].cNumClusters;
		clusterSize = cCount * sizeof(zbClusterId_t);
		*pMessageGoingOut++ = cCount;
		FLib_MemCpy(pMessageGoingOut, pClusterListPair[cIndex].pClusterList, clusterSize);
		pMessageGoingOut += clusterSize;
		iSize += sizeof(cCount) + clusterSize;
	}
	return (iSize);
}
#endif

#if gReplace_Device_rsp_d
/*-------------------- ZDP_SearchInBindCache --------------------
	This function is ZDP internal and Replace Device exclusive.

	Search in the binding cache table for entries in which the the Source or destination address
	or both, is equal to the address provide as aparameter, depends on the mode given will match the Source
	or the destination or both addresses.

	IN: The IEEE address to match.
	IN/OUT: The mode used to compare against the source address or the destination address.
	IN: The index where the search will start.

	OUT: The index where the entry was found or NO ENTRY if not found.
*/
index_t ZDP_SearchInBindCache
(
	zbIeeeAddr_t  matchingAddress,  /* IN: The IEEE address to match. */
	zbAddrMode_t  *addressMode,     /* IN/OUT: The mode used to compare against the source address or the destination address. */
	index_t  index                  /* IN: The index where the search will start. */
)
{
	index_t cIndex;
	zbApsmeBindEntry_t  *pPtr;

	for (cIndex = index; cIndex < gMaximumBindingCacheTableList_c; cIndex++)
	{
		pPtr = &gaBindingCacheTable[cIndex];
		if (IsEqual8Bytes(matchingAddress, pPtr->aSrcAddr))
		{
			if (addressMode != NULL)
				addressMode[0] = zbSrcAddressMode;
			return cIndex;
		}
		if (IsEqual8Bytes(matchingAddress, pPtr->aDstAddr))
		{
			if (addressMode != NULL)
				addressMode[0] = zbDstAddressMode;
			return cIndex;
		}
	}
	return gZdoNoEntry_c;
}
#endif

#if gUnbind_rsp_d && gBind_Register_rsp_d
zbStatus_t Zdp_RemoveBindCache
(
	zbApsmeBindReq_t  *pBindEntry  /* IN: The entry to match. */
)
{
	index_t  cIndex;

	cIndex = ZDP_MatchBindCacheEntry(pBindEntry,pBindEntry->dstAddrMode);

	if (cIndex == gZdoNoEntry_c)
		return gZdoNoEntry_c;

	BeeUtilZeroMemory(&gaBindingCacheTable[cIndex], sizeof(gaBindingCacheTable[cIndex]));
	return gZbSuccess_c;
}
#endif

#if gRemove_Bkup_Bind_Entry_rsp_d || gStore_Bkup_Bind_Entry_rsp_d || (gBind_Register_rsp_d && gUnbind_rsp_d)
/*-------------------- ZDP_MatchBindCacheEntry --------------------
	This function is ZDP internal.

	Search in the binding cache table one entry at the time,
	and match each one with the given entry.

	IN: The entry to match.
	IN: The Destination address mode for the given entry.

	OUT: gZdoNoEntry_c if there is no match at all, or the INDEX in the table if there is a match.
*/
index_t ZDP_MatchBindCacheEntry
(
	zbApsmeBindEntry_t  *pBindEntry,  /* IN: The entry to match. */
	zbAddrMode_t  addressMode         /* IN: The Destination address mode for the given entry. */
)
{
	index_t  cIndex;
	for (cIndex = 0; cIndex < gMaximumBindingCacheTableList_c; cIndex++)
	{
		if (addressMode == zbGroupMode)
		{
			if (FLib_MemCmp(&gaBindingCacheTable[cIndex], pBindEntry, MbrOfs(zbApsmeBindEntry_t, aDstAddr[0])+ sizeof(zbGroupId_t)))
				return cIndex;
		}
		else if (addressMode == zbIEEEMode)
		{
			if (FLib_MemCmp(&gaBindingCacheTable[cIndex], pBindEntry, sizeof(zbApsmeBindEntry_t)))
				return cIndex;
		}
	}
	return gZdoNoEntry_c;
}
#endif

#if gBind_Register_rsp_d || gRecover_Bind_Table_rsp_d
/*-------------------- ZDP_RecordSourceBindAddress --------------------
	This fucntion is ZDP Internal.

	Stores the IEEE address of the device soliciting its own binding information.
	If the address has being adde previously or is added returns SUCCESS, otherwise
	returns TABLE FULL.

	IN: The IEEE Addres to store.

	OUT: SUCCESS if the adres is already inthe table or is added, otherwise TBALE FULL.
*/
zbStatus_t ZDP_RecordSourceBindAddress
(
	zbIeeeAddr_t  aIeeeAddress  /* IN: The IEEE Addres to store. */
)
{
	index_t zeroIndex = 0xff;
	index_t cIndex;

	if (gDeviceHoldingItsOwnBindingInfo_Counter >= gMaximumDevicesHoldingBindingInfo_c)
		return gZdoTableFull_c;

/*Search into the entire  Table */
	for (cIndex = 0; cIndex< gMaximumDevicesHoldingBindingInfo_c; cIndex++)
	{
		if(IsEqual8Bytes(aDevicesHoldingItsOwnBindingInfo[cIndex], aIeeeAddress))
			return gZbSuccess_c;
		if (Cmp8BytesToZero(aDevicesHoldingItsOwnBindingInfo[cIndex]) && (zeroIndex == 0xff))
			zeroIndex = cIndex;
	}

	cIndex = zeroIndex;
	if (zeroIndex != 0xff)
	{
		Copy8Bytes(aDevicesHoldingItsOwnBindingInfo[zeroIndex], aIeeeAddress);
		gDeviceHoldingItsOwnBindingInfo_Counter++;
		return gZbSuccess_c;
	}
	else
		return gZdoTableFull_c;
}
#endif

#if gBind_Register_rsp_d || gRecover_Bind_Table_rsp_d
/*-------------------- ZDP_GetBindingCacheEntryListByAddress --------------------
	This function is ZDP internal.

	Used to find the binding entries where the Src Address 
	is equal to the IEEE addres received as a parameter.

	IN: The IEEE Addres to lok inthe binding table.
	IN: The pointer where the list of device will be generated.

	OUT: The size inbytes of the list of binding netrys to send back inthe response.
	OUT:Returns the amount of entries if at least one entry was found and 0 if not.
*/
zbuCounter16_t ZDP_GetBindingCacheEntryListByAddress
(
	zbIeeeAddr_t  aIeeeAddress,   /* IN: The IEEE Addres to lok inthe binding table, a broadcast address means all entries. */
	uint8_t  *pBindingEntryList,  /* IN: The pointer where the list of device will be generated. */
	zbSize_t  *listPayload        /* OUT: The size in bytes of the list of binding entries to send back in the response. */
)
{
	index_t  cIndex;
	zbuCounter16_t  entryCounter;
	zbApsmeBindEntry_t  *pPtr;

	entryCounter = 0x0000;
	listPayload[0] = 0;

	for (cIndex = 0; cIndex < gMaximumBindingCacheTableList_c; cIndex++)
	{
		pPtr = &gaBindingCacheTable[cIndex];
		if (ZdpBindIsFree(gaBindingCacheTable[cIndex]))
			continue;

		if (IsEqual8Bytes(aIeeeAddress, pPtr->aSrcAddr) || Cmp8BytesToFs(aIeeeAddress))
		{
			if (entryCounter < ZdpMaximumEntriesInPayLoad(MbrOfs(zbApsmeBindEntry_t, aDstAddr[0]), sizeof(zbApsmeBindEntry_t)))
			{
				if (pPtr->dstAddrMode == zbIEEEMode)
				{
					FLib_MemCpy(pBindingEntryList, pPtr,sizeof(zbApsmeBindEntry_t));
					pBindingEntryList += sizeof(zbApsmeBindEntry_t);
					listPayload[0] += sizeof(zbApsmeBindEntry_t);
				}
				else if (pPtr->dstAddrMode == zbGroupMode)
				{
					FLib_MemCpy(pBindingEntryList,pPtr,(MbrOfs(zbApsmeBindEntry_t, aDstAddr[0]) + sizeof(zbGroupId_t)));
					pBindingEntryList += (MbrOfs(zbApsmeBindEntry_t, aDstAddr[0]) + sizeof(zbGroupId_t));
					listPayload[0] += (MbrOfs(zbApsmeBindEntry_t, aDstAddr[0]) + sizeof(zbGroupId_t));
				}
			}
			entryCounter++;
		}
	}
	return entryCounter;
}
#endif

#if gBind_Register_rsp_d || gRecover_Bind_Table_rsp_d
/*-------------------- ZDP_SearchDeviceHoldingItsOwnBindingInfo --------------------
	This function is ZDP internal.

	This functions is part of the interface for accessing the table 
	of the devices holding its own bindig info.

	IN: The IEEE Address of the that will be serach in the table.

	OUT: The index in the table where the devices is located or the proper error code if is not found.
*/
index_t ZDP_SearchDeviceHoldingItsOwnBindingInfo
(
	zbIeeeAddr_t  aAddress  /* IN: The IEEE Address of the that will be serach inthe table. */
)
{
	index_t  cIndex;

	for (cIndex = 0;cIndex < gMaximumDevicesHoldingBindingInfo_c; cIndex++)
		if (IsEqual8Bytes(aDevicesHoldingItsOwnBindingInfo[cIndex], aAddress))
			return (cIndex);

	return gZdoNoEntry_c;
}
#endif

#if gBind_Register_rsp_d || gStore_Bkup_Bind_Entry_rsp_d || gRecover_Bind_Table_rsp_d
/*-------------------- ZDP_AddToBindingCacheTable --------------------
	This function is ZDP internal and binding cache exclusive.

	Register a bind request in the Binding cache table if and only if we are the primary cahe device
	and the bind request arrives to us and the source address is not ours.

	IN: The bind request arriving to our node.

	OUT: The status of the store process, TRUE if the store process was completed with success and FALSE otherwise.
*/
bool_t ZDP_AddToBindingCacheTable
(
	zbBindUnbindRequest_t *pBindRequest  /* IN: The bind request arriving to our node. */
)
{
	index_t  cIndex;
	zbApsmeBindEntry_t  *pPtr;

	if (gBindingCacheTableCounter >= gMaximumBindingCacheTableList_c)
		return FALSE;
	
	for (cIndex = 0; cIndex < gMaximumBindingCacheTableList_c; cIndex++)
	{
		pPtr = &gaBindingCacheTable[cIndex];
		if (!ZdpBindIsFree(gaBindingCacheTable[cIndex]))
			continue;
		else
		{
			FLib_MemCpy(pPtr,pBindRequest,MbrOfs(zbBindUnbindRequest_t, destData));
			if (pPtr->dstAddrMode == zbGroupMode)
			{
				Fill8BytesToZero(pPtr->aDstAddr);
				Copy2Bytes(pPtr->aDstAddr, pBindRequest->destData.groupMode.aDstaddress);
				pPtr->dstEndPoint = gZbBroadcastEndPoint_c;
				gBindingCacheTableCounter++;
				return TRUE;
			}
			else if (pPtr->dstAddrMode == zbIEEEMode)
			{
				Copy8Bytes(pPtr->aDstAddr, pBindRequest->destData.extendedMode.aDstAddress);
				pPtr->dstEndPoint = pBindRequest->destData.extendedMode.dstEndPoint;
				gBindingCacheTableCounter++;
				return TRUE;
			}
		}
	}
	return FALSE;
}
#endif

#if (gBindCapability_d && (gBind_rsp_d || gUnbind_rsp_d)) || gReplace_Device_rsp_d
/*-------------------- ZDP_StateMachine --------------------
	This function is ZDP internal and ZDP_StateMachine exclusive.

	This function is the Main engine to control the state machines in ZDP,
	it use a special case for every single one.

	IN: The ID for the state mchine to execute.
	IN: The sequence number that identifys each state machine runing, or to be executed.

	OUT: NONE
*/
void ZDP_StateMachine
(
	zbMachineId_t  machineId,    /* IN: The ID for the state mchine to execute. */
	zbCounter_t  sequenceNumber  /* IN: The sequence number that identifys each state machine runing, or to be executed. */
)
{
	switch(machineId)
	{
#if gReplace_Device_rsp_d
	case gReplace_Device_req_c: Process_ReplaceDevices(sequenceNumber);
	break;
	case gRemove_Bkup_Bind_Entry_req_c: Process_RemoveDevReq(sequenceNumber);
	break;
#endif
#if (gBind_rsp_d || gUnbind_rsp_d) && gBindCapability_d
		case gBind_req_c: Process_BindReq(sequenceNumber);
		break;
#endif
#if gBind_Register_rsp_d
		case gInformDevices_c: Process_SourceBindDevices(sequenceNumber);
		break;
#endif
		default: return;
		/* iar warning of statement unreachable */
                #ifndef __IAR_SYSTEMS_ICC__        
		break;
                #endif
	}
}
#endif

#if (gBindCapability_d && (gBind_rsp_d || gUnbind_rsp_d)) || gReplace_Device_rsp_d
/*-------------------- ZDP_GetIndexMachineProcessQueue --------------------
	This function is ZDP internal and ZDP_StateMachine exclusive.

	This function in multiplexed to be used in different ways,
	but the main is retriving the index of different storage places

	IN: The state machine ID for which we are going to serach the info
	IN: The sequence number to match and identify the info, this method
	can have multiple states machines of the same kind.

	OUT: The index where the information is located in the queue.
*/
index_t ZDP_GetIndexMachineProcessQueue
(
	zbMachineId_t  machineId,    /* IN: The state machine ID for which we are looking the informtion. */
	zbCounter_t  sequenceNumber  /* IN: The sequence number used as an unique ID for the stored information. */
)
{
	index_t  cIndex;
	zbEventProcessQueue_t  *pPtr;
	/* Search in the whole storage array to get the asked information */
	for (cIndex = 0; cIndex < NumberOfElements(maEventProcessQueue); cIndex++)
	{
		pPtr = &maEventProcessQueue[cIndex];
		if ((pPtr->machineId == machineId) &&
				(pPtr->sequenceNumber == sequenceNumber))
			return cIndex;
	}
	return gZdoNoEntry_c;
}

/*-------------------- ZDP_SetRegisterMachineData --------------------
	This function is ZDP internal and ZDP_StateMachine exclusive.

	Keeps specific information in memory the just the needed time,
	some state machines need to keep around some information until the state
	machine requires it.

	IN: The ID for a particular state machine.
	IN: The source address of the originator of initial request.
	IN: The particular data that needs to keep in memory.

	OUT: the status of the operation required.
*/
zbStatus_t ZDP_SetRegisterMachineData
(
	zbMachineId_t  machineId,  /* IN: The ID for a particular state machine that needs to stro information */
	zbNwkAddr_t   SrcAddr,     /* IN: The source address of the originator of initial request. */
	void  *data                /* IN: The particular data that needs to keep in memory. */
)
{
	index_t  cIndex;
	zbEventProcessQueue_t  *pPtr;
	/* Get a free entry in our storage system */
	cIndex = ZDP_GetIndexMachineProcessQueue(gNotActive_c,gNotActive_c);
	if (cIndex == gZdoNoEntry_c)
		return cIndex;
	/* the faiulure condition has pass so we have a valid index to use and we must the event */
	ZDP_SetMachineId(cIndex, machineId);
	/* This line keep the information around until the process of binding has come to and end */
	ZDP_SetOriginalSequenceNumber(cIndex, gZdpResponseCounter);
	/* We need to remeber the originator of the request to send the porper response to it */
	ZDP_SetOriginatorAddress(cIndex, SrcAddr);
	/* Depending on which procees is the data that we will kepp around */
	pPtr = &maEventProcessQueue[cIndex];
	switch(machineId)
	{
		/*
			The BindReq or Bind request after bind register state machine the need to store some data
		*/
		case gBind_req_c:
		case gInformDevices_c:
			/* Store the data form the original bind request to use it when all the information has being collected */
			FLib_MemCpy(&pPtr->data.bindingInfo,
									(zbBindUnbindRequest_t *)data,
									sizeof(zbBindUnbindRequest_t));
			/* Call the Engine to pass the information to the proper state machine. */
			ZDP_StateMachine(machineId,gZdpSequence);
		break;
		case gReplace_Device_req_c:
		case gRemove_Bkup_Bind_Entry_req_c:
			/*
				Stores the data from the original Remove_Bkup_Bind_Entry or Replace Device to use it when the state machine ends
			*/
			FLib_MemCpy(&pPtr->data.replaceInfo.replaceData,
									(zbReplaceDeviceRequest_t *)data,
									sizeof(zbReplaceDeviceRequest_t));
			pPtr->data.replaceInfo.index = 0;
			ZDP_StateMachine(machineId, gZdpSequence);
		break;

		default:
		break;
	}
	return gZbSuccess_c;
}

/*-------------------- ZDP_CleanUpStateMachineData --------------------
	This functions is ZDP interanl and ZDP_StateMachine exclusive.

	Clean up the values of a articular entry in the ZDP state machine queue,
	it uses the given index inthe queue and sets ID for the particular machine
	that stored the data and set the sequence number to one invalid, also the 
	cleans up the response sequence number.

	IN: The index in the ZDP_StateMachine queue to clean up.

	OUT: NONE
*/
void ZDP_CleanUpStateMachineData
(
	index_t cIndex  /* IN: The index in the ZDP_StateMachine queue to clean up. */
)
{
	ZDP_SetMachineId(cIndex, gNotActive_c);
	ZDP_SetSequenceId(cIndex, gNotActive_c);
	ZDP_SetResponseSeqNum(ZDP_GetOriginalSequenceNumber(cIndex));
}
#endif

#if gReplace_Device_rsp_d
void GetAddressFromService
(
	uint8_t  service,
	uint8_t *pNwkAddress
)
{
	zbSystemServerDiscoveryStore_t  *pPtr = &mSystemServerDiscResponse[3];
	switch (service)
	{
		case ZdoBackupBindingTableCache_c:
			if (!Cmp2BytesToZero(pPtr->aServerMask))
				Copy2Bytes(pNwkAddress, pPtr->aNwkAddress);
			else
				Set2Bytes(pNwkAddress, 0xffff);
		break;
		default:
			Set2Bytes(pNwkAddress, 0xffff);
		break;
	}
}

bool_t SendBindUnBindRequest
(
	index_t  cIndex,
	zbMsgId_t  msgId
)
{
	uint8_t  *pNwkAddress, aNwkAddr[2];
	zbNwkAddr_t  aNwkAddress;
	index_t i;
	zbApsmeBindEntry_t  *pPtr;

	/*
		Allocate the Memory for the out going message for this particular state machine
	*/
	zbBindUnbindRequest_t  *pMessageGoingOut = MSG_AllocType(zbBindUnbindRequest_t);

	if(pMessageGoingOut == NULL)
		return FALSE;
	/*
		Cleanup the space allocated to avoid grabge over the air
	*/
	BeeUtilZeroMemory(pMessageGoingOut, sizeof(zbBindUnbindRequest_t));
	i = maEventProcessQueue[cIndex].data.replaceInfo.index;
	pPtr = &gaBindingCacheTable[i];
	FLib_MemCpy(pMessageGoingOut, pPtr, sizeof(zbApsmeBindEntry_t));

	pNwkAddress = APS_GetNwkAddress(pPtr->aSrcAddr, aNwkAddr);
	if (pNwkAddress == NULL)
	{
		ZDP_CleanUpStateMachineData(cIndex);
		MSG_Free(pMessageGoingOut);
		return FALSE;
	}

	Zdp_RequestGenerator(msgId, pMessageGoingOut, pNwkAddress);

	GetAddressFromService(ZdoBackupBindingTableCache_c, aNwkAddress);
	if (!IsEqual2BytesInt(aNwkAddress, 0xffff))
	{
		BeeUtilZeroMemory(pMessageGoingOut, sizeof(zbBindUnbindRequest_t));
		FLib_MemCpy(pMessageGoingOut, &pPtr,sizeof(zbApsmeBindEntry_t));
		if (msgId == gBind_req_c)
		{
			Zdp_RequestGenerator(gStore_Bkup_Bind_Entry_req_c, pMessageGoingOut, aNwkAddress);
		}
		else
		{
			Zdp_RequestGenerator(gRemove_Bkup_Bind_Entry_req_c, pMessageGoingOut, aNwkAddress);
		}
	}
	MSG_Free(pMessageGoingOut);
	return TRUE;
}

bool_t SendRemoveStoreRequest
(
	index_t  cIndex,
	zbMsgId_t  msgId
)
{
	zbNwkAddr_t  aNwkAddress;
	index_t  i;

	/*
		Allocate the Memory for the out going message for this particular state machine
	*/
	zbStoreBkupBindEntryRequest_t  *pMessageGoingOut = MSG_AllocType(zbStoreBkupBindEntryRequest_t);

	if(pMessageGoingOut == NULL)
		return FALSE;
	/*
		Cleanup the space allocated to avoid grabge over the air
	*/
	BeeUtilZeroMemory(pMessageGoingOut, sizeof(zbStoreBkupBindEntryRequest_t));
	i = maEventProcessQueue[cIndex].data.replaceInfo.index;
	FLib_MemCpy(pMessageGoingOut, 
							&gaBindingCacheTable[i],sizeof(zbStoreBkupBindEntryRequest_t));
	GetAddressFromService(ZdoBackupBindingTableCache_c, aNwkAddress);
  (void)msgId; /* to avoid codewarrior message NOTE */
	MSG_Free(pMessageGoingOut);
	return TRUE;
}

void ReplaceData(index_t cIndex, zbAddrMode_t addressMode)
{
	index_t i;
	zbApsmeBindEntry_t  *pBind;
	zbEventProcessQueue_t  *pEvent;

	i = maEventProcessQueue[cIndex].data.replaceInfo.index;
	pBind = &gaBindingCacheTable[i];
	pEvent = &maEventProcessQueue[cIndex];
	if (addressMode == zbDstAddressMode)
	{
		Copy8Bytes(pBind->aDstAddr,pEvent->data.replaceInfo.replaceData.aNewAddress);
	
		if (pEvent->data.replaceInfo.replaceData.oldEndPoint)
		pBind->dstEndPoint = pEvent->data.replaceInfo.replaceData.newEndPoint;
	}
	if (addressMode == zbSrcAddressMode)
	{
		Copy8Bytes(pBind->aSrcAddr,
							pEvent->data.replaceInfo.replaceData.aNewAddress);
	
		if (pEvent->data.replaceInfo.replaceData.oldEndPoint)
		pBind->srcEndPoint = pEvent->data.replaceInfo.replaceData.newEndPoint;
	}
}

void SendBackStatus
(
	index_t cIndex,
	zbStatus_t  status
)
{
	/*
		Allocate the Memory for the out going message for this particular state machine
	*/
	zdpToAppMessage_t  *pMessageGoingOut = AF_MsgAlloc();

	if(pMessageGoingOut == NULL)
		return;
	/*
		Cleanup the space allocated to avoid grabge over the air
	*/
	/* gMaxRxTxDataLength_c is the allocated size of AF_MsgAlloc*/
	BeeUtilZeroMemory(pMessageGoingOut, gMaxRxTxDataLength_c);
	*((uint8_t *)pMessageGoingOut + SapHandlerDataStructureOffSet) = status;
	Zdp_GenerateDataReq(gReplace_Device_rsp_c, maEventProcessQueue[cIndex].sourceAddress,(afToApsdeMessage_t *)pMessageGoingOut, MbrSizeof(zbReplaceDeviceResponse_t, status));
}

void Process_ReplaceDevices
(
	zbCounter_t sequenceNumber
)
{
	index_t  cIndex;
	zbAddrMode_t addressMode;
	zbEventProcessQueue_t  *pPtr;

	cIndex = ZDP_GetIndexMachineProcessQueue(gReplace_Device_req_c, sequenceNumber);
	pPtr = &maEventProcessQueue[cIndex];
	if (cIndex != gZdoNoEntry_c)
	{
		if (!SendBindUnBindRequest(cIndex,gBind_req_c))
			SendBackStatus(cIndex,gZdoInvalidRequestType_c);

		pPtr->data.replaceInfo.index = ZDP_SearchInBindCache(pPtr->data.replaceInfo.replaceData.aOldAddress,
																												&addressMode, pPtr->data.replaceInfo.index);
		if (pPtr->data.replaceInfo.index != gZdoNoEntry_c)
		{
			if (SendBindUnBindRequest(cIndex,gUnbind_req_c))
				ReplaceData(cIndex,addressMode);
			else
				return;
		}
		else
		{
			ZDP_CleanUpStateMachineData(cIndex);
			SendBackStatus(cIndex,gZbSuccess_c);
		}
	}
	else
	{
		cIndex = ZDP_GetIndexMachineProcessQueue(gReplace_Device_req_c,gNotActive_c);
		pPtr = &maEventProcessQueue[cIndex];
		if (cIndex != gZdoNoEntry_c)
		{
			ZDP_SetSequenceId(cIndex,sequenceNumber);
			/* We Spect that at leat there is one device */
			pPtr->data.replaceInfo.index = ZDP_SearchInBindCache(pPtr->data.replaceInfo.replaceData.aOldAddress,
																													&addressMode, pPtr->data.replaceInfo.index);
			if (pPtr->data.replaceInfo.index != gZdoNoEntry_c)
			{
				if (SendBindUnBindRequest(cIndex,gUnbind_req_c))
					ReplaceData(cIndex,addressMode);
				else
					return;
			}
		}
	}
}

void Process_RemoveDevReq
(
	zbCounter_t  sequenceNumber
)
{
	index_t  cIndex;
	zbAddrMode_t  addressMode;
	zbEventProcessQueue_t  *pPtr;

	cIndex = ZDP_GetIndexMachineProcessQueue(gRemove_Bkup_Bind_Entry_req_c, sequenceNumber);
	pPtr = &maEventProcessQueue[cIndex];
	if (cIndex != gZdoNoEntry_c)
	{
		if (!SendRemoveStoreRequest(cIndex,gStore_Bkup_Bind_Entry_req_c))
			SendBackStatus(cIndex,gZdoInvalidRequestType_c);

		pPtr->data.replaceInfo.index = ZDP_SearchInBindCache(pPtr->data.replaceInfo.replaceData.aOldAddress,
																												&addressMode, pPtr->data.replaceInfo.index);
		if (pPtr->data.replaceInfo.index != gZdoNoEntry_c)
		{
			if (SendRemoveStoreRequest(cIndex,gRemove_Bkup_Bind_Entry_req_c))
				ReplaceData(cIndex,addressMode);
			else
				return;
		}
		else
		{
			ZDP_CleanUpStateMachineData(cIndex);
			SendBackStatus(cIndex,gZbSuccess_c);
		}
	}
	else
	{
		cIndex = ZDP_GetIndexMachineProcessQueue(gRemove_Bkup_Bind_Entry_req_c, gNotActive_c);
		pPtr = &maEventProcessQueue[cIndex];
		if (cIndex != gZdoNoEntry_c)
		{
			ZDP_SetSequenceId(cIndex,sequenceNumber);
			/* We Spect that at leat there is one device */
			pPtr->data.replaceInfo.index = ZDP_SearchInBindCache(pPtr->data.replaceInfo.replaceData.aOldAddress,
																													&addressMode, pPtr->data.replaceInfo.index);
			if (pPtr->data.replaceInfo.index != gZdoNoEntry_c)
			{
				if (SendRemoveStoreRequest(cIndex,gRemove_Bkup_Bind_Entry_req_c))
					ReplaceData(cIndex,addressMode);
				else
					return;
			}
		}
	}
}

#endif

#if gBind_Register_rsp_d
/*-------------------- Process_SourceBindDevices --------------------
	This fucntion is ZDP internal and ZDP_StateMachine exclusive.

	The objective of this fucntion is get the information needed to send the bind
	information to the device which is holding it's own binding information.

	IN: The sequence number for the processing message.

	OUT: NONE
*/
void Process_SourceBindDevices
(
	zbCounter_t  sequenceNumber  /* IN: The sequence number for the processing message. */
)
{
	index_t  cIndex;
	uint8_t  *pAddress, aNwkAddr[2];
	zbEventProcessQueue_t  *pPtr;
	/*
		Allocate the Memory for the out going message for this particular state machine
	*/
	zdpToAppMessage_t  *pMessageGoingOut = AF_MsgAlloc();

	if(pMessageGoingOut == NULL)
		return;
	/*
		Cleanup the space allocated to avoid grabge over the air
	*/
	/* gMaxRxTxDataLength_c is the allocated size of AF_MsgAlloc*/
	BeeUtilZeroMemory(pMessageGoingOut, gMaxRxTxDataLength_c);
	
	/*
		This function is the one that takes care of the bind state machine so it look if we are working wit a previous 
		initiated state machine.
	*/
	cIndex = ZDP_GetIndexMachineProcessQueue(gInformDevices_c, sequenceNumber);
	pPtr = &maEventProcessQueue[cIndex];
	if (cIndex != gZdoNoEntry_c)
	{
		/*
			If we are working with a previous initiated state machine, then do the bind response back.
		*/
		ZDP_CleanUpStateMachineData(cIndex);
		*((uint8_t *)pMessageGoingOut + SapHandlerDataStructureOffSet) = 
		*((uint8_t *)&(pPtr->data.bindingInfo));
		Zdp_GenerateDataReq(gBind_rsp_c, ZDP_GetOriginatorAddress(cIndex),
												(afToApsdeMessage_t *)pMessageGoingOut,
												MbrSizeof(zbBindUnbindResponse_t,status));
	}
	else
	{
		cIndex = ZDP_GetIndexMachineProcessQueue(gInformDevices_c,gNotActive_c);
		pPtr = &maEventProcessQueue[cIndex];
		if (cIndex != gZdoNoEntry_c)
		{
			ZDP_SetSequenceId(cIndex,sequenceNumber);
			FLib_MemCpy((uint8_t *)pMessageGoingOut + SapHandlerDataStructureOffSet, 
								&pPtr->data.bindingInfo,
								MbrSizeof(zbEventProcessQueue_t, data.bindingInfo));
			pAddress = APS_GetNwkAddress((uint8_t *)pPtr->data.bindingInfo.aSrcAddress, aNwkAddr);
			Zdp_GenerateDataReq(gBind_req_c, pAddress,
													(afToApsdeMessage_t *)pMessageGoingOut,
													MbrSizeof(zbEventProcessQueue_t,data.bindingInfo));
		}
		else
			MSG_Free(pMessageGoingOut);
	}
}
#endif

#if (gBind_rsp_d || gUnbind_rsp_d) && gBindCapability_d
/*-------------------- Process_BindReq --------------------
	This function is ZDP internal and Bind state machine exclusive.

	This fucntion is the main procedure for the bind state machine,
	is inserted in the ZDP_StateMachine engine as a separated state machine.

	IN: The sequence number of particular call, this seuqence number helps to unique identify
	which bind state machine are we preocessing.

	OUT: NONE
*/
void Process_BindReq
(
	zbCounter_t  sequenceNumber  /* IN: The sequence number of particular call, 
																			this seuqence number helps to unique identify
																			which bind state machine are we preocessing. */
)
{
	index_t  cIndex, cFinalStatus;
	zbEventProcessQueue_t  *pPtr;
	/*
		allocate the Memory for the out going message for this particular stae machine
	*/
	zdpToAppMessage_t  *pMessageGoingOut = AF_MsgAlloc();

	if(pMessageGoingOut == NULL)
		return;
	/*
		Cleanup the space allocated to avoid grabge over the air
	*/
	/* gMaxRxTxDataLength_c is the allocated size of AF_MsgAlloc*/
	BeeUtilZeroMemory(pMessageGoingOut, gMaxRxTxDataLength_c);
	/*
		This function is the one that takes care of the bind state machine so it look if we are working wit a previous 
		initiated state machine.
	*/
	cIndex = ZDP_GetIndexMachineProcessQueue(gBind_req_c, sequenceNumber);
	pPtr = &maEventProcessQueue[cIndex];
	if (cIndex != gZdoNoEntry_c)
	{
		/*
			If we are working with a previous initiated state machine, then do the binding
		*/

		ZDP_CleanUpStateMachineData(cIndex);
		cFinalStatus = APSME_BindRequest((zbApsmeBindReq_t *)&pPtr->data.bindingInfo);
    /*
      We need to translate the error codes form APS, in this way the proper error code
      goes OTA, and the sniffer can see it correctly.
    */
    switch (cFinalStatus)
    {
      case gApsIllegalRequest_c:
        cFinalStatus = gZdoInvalidRequestType_c;
      break;
      case gApsInvalidBinding_c:
        cFinalStatus = gZdoNoEntry_c;
      break;
      case gApsTableFull_c:
        cFinalStatus = gZdoTableFull_c;
      break;
    }
		((uint8_t *)pMessageGoingOut + SapHandlerDataStructureOffSet)[0] = cFinalStatus;
		Zdp_GenerateDataReq(gBind_rsp_c, ZDP_GetOriginatorAddress(cIndex),
												(afToApsdeMessage_t *)pMessageGoingOut,
												MbrSizeof(zbBindUnbindResponse_t,status));
	}
	else
	{
		cIndex = ZDP_GetIndexMachineProcessQueue(gBind_req_c,gNotActive_c);
		pPtr = &maEventProcessQueue[cIndex];
		if (cIndex != gZdoNoEntry_c)
		{
			ZDP_SetSequenceId(cIndex,sequenceNumber);
			FLib_MemCpy((uint8_t *)pMessageGoingOut + SapHandlerDataStructureOffSet,
									pPtr->data.bindingInfo.destData.extendedMode.aDstAddress,
									MbrSizeof(zbEventProcessQueue_t, data.bindingInfo.destData.extendedMode.aDstAddress));
			Zdp_GenerateDataReq(gNWK_addr_req_c, (uint8_t *)gaBroadcastRxOnIdle,
													(afToApsdeMessage_t *)pMessageGoingOut,
													sizeof(zbNwkAddrRequest_t));
		}
		else
			MSG_Free(pMessageGoingOut);
	}
}
#endif

/*****************************************************************************/
#if gDiscovery_store_rsp_d || gBkup_Discovery_cache_d
zbStatus_t UpdateDiscoveryStoreTable
(
	zbNwkAddr_t  aNwkAddress,
	zbIeeeAddr_t  aIeeeAddress
)
{
	index_t  cIndex;
	discoveryStoreTable_t  *pPtr;

	cIndex = ZDP_SearchNodeInCache(aNwkAddress,aIeeeAddress,zbIEEEMode);
	pPtr = Zdp_CacheTableEntryAddr(cIndex);
	if (cIndex != gZdoNoEntry_c)
	{
		if (IsEqual2Bytes(aNwkAddress, Zdp_CacheTableNwkAddr(pPtr)))
			return gZbSuccess_c;
		else
		{
			BeeUtilZeroMemory(pPtr, Zdp_CacheTableEntrySize(cIndex));
			Copy2Bytes(Zdp_CacheTableNwkAddr(pPtr), aNwkAddress);
			Copy8Bytes(Zdp_CacheTableIeeeAddr(pPtr), aIeeeAddress);
			return gZbSuccess_c;
		}
	}
	if (mDiscoveryStoreTableEntrys < gMaximumDiscoveryStoreTableSize_c)
	{
		for (cIndex = 0; cIndex < gMaximumDiscoveryStoreTableSize_c; cIndex++)
		{
			pPtr = Zdp_CacheTableEntryAddr(cIndex);
			if (Cmp2BytesToZero(Zdp_CacheTableNwkAddr(pPtr)) &&
					Cmp8BytesToZero(Zdp_CacheTableIeeeAddr(pPtr)))
			{
				Copy2Bytes(Zdp_CacheTableNwkAddr(pPtr), aNwkAddress);
				Copy8Bytes(Zdp_CacheTableIeeeAddr(pPtr), aIeeeAddress);
				mDiscoveryStoreTableEntrys++;
				return gZbSuccess_c;
			}
		}
	}

	return gZdoInsufficientSpace_c;
}
#endif

#if gStore_Bkup_Bind_Entry_rsp_d || gRemove_Bkup_Bind_Entry_rsp_d || gBackup_Bind_Table_rsp_d || gRecover_Bind_Table_rsp_d || gBackup_Source_Bind_rsp_d || gRecover_Source_Bind_rsp_d
/*-------------------- ZDP_MatchAddressToService ---------------------
	This fucntion is ZDP internal.

	Given a particular service and a network address search in the local table
	to match with the info from previous discovery service.

	IN: The address of the devices to match with the service.
	IN: The service that we are trying to match with.

	OUT: TRUE if there is a match, and FALSE otherwise.
*/
bool_t ZDP_MatchAddressToService
(
	zbNwkAddr_t  aNwkAddress,  /*IN : The address to match with the particular service. */
	uint8_t  serverMaskToMach  /*IN : The service to look for. */
)
{

	switch(serverMaskToMach)
	{
		case ZdoPrimaryTrustCenter_c:
			if ((Cmp2BytesToZero(mSystemServerDiscResponse[0].aServerMask)) ||
					(!IsEqual2Bytes(aNwkAddress, mSystemServerDiscResponse[0].aNwkAddress)))
			{
				return FALSE;
			}
		break;
		case ZdoBackupTrustCenter_c:
			if ((Cmp2BytesToZero(mSystemServerDiscResponse[1].aServerMask)) ||
					(!IsEqual2Bytes(aNwkAddress, mSystemServerDiscResponse[1].aNwkAddress)))
			{
				return FALSE;
			}
		break;
		case ZdoPrimaryBindingTableCache_c:
			if ((Cmp2BytesToZero(mSystemServerDiscResponse[2].aServerMask)) ||
					(!IsEqual2Bytes(aNwkAddress, mSystemServerDiscResponse[2].aNwkAddress)))
			{
				return FALSE;
			}
		break;
		case ZdoBackupBindingTableCache_c:
			if ((Cmp2BytesToZero(mSystemServerDiscResponse[3].aServerMask)) ||
					(!IsEqual2Bytes(aNwkAddress, mSystemServerDiscResponse[3].aNwkAddress)))
			{
				return FALSE;
			}
		break;
		case ZdoPrimaryDiscoveryCache_c:
			if ((Cmp2BytesToZero(mSystemServerDiscResponse[4].aServerMask)) ||
					(!IsEqual2Bytes(aNwkAddress, mSystemServerDiscResponse[4].aNwkAddress)))
			{
				return FALSE;
			}
		break;
		case ZdoBackupDiscoveryCache_c:
			if ((Cmp2BytesToZero(mSystemServerDiscResponse[5].aServerMask)) ||
					(!IsEqual2Bytes(aNwkAddress, mSystemServerDiscResponse[5].aNwkAddress)))
			{
				return FALSE;
			}
		break;
		default:
			return FALSE;
                /* marked as unreachable by IAR */
                #ifndef __IAR_SYSTEMS_ICC__ 
		break;
                #endif
	}
	return TRUE;
}
#endif

#if gDiscovery_store_rsp_d || gNode_Desc_store_rsp_d || gPower_Desc_store_rsp_d || gActive_EP_store_rsp_d || gSimple_Desc_store_rsp_d || gRemove_node_cache_rsp_d || gFind_node_cache_rsp_d
/*-------------------- ZDP_SearchNodeInCache --------------------
	This function is ZDP internal and Cache Device exclusive

	This fucntion search inthe Cache table any entrie who match the
	NWK addres or the IEEEE address or both. The mode parameter 
	determines which is going to be the criteria ti use during the
	matching process.

	IN: The network address used to match during the search and process.
	IN: The IEEE address used to match during the search and match process.
	IN: The mode used to search and match.

	OUT: NONE.
*/
zbStatus_t ZDP_SearchNodeInCache
(
	zbNwkAddr_t aNwkAddress,    /* IN: The network address used to match during the search and process. */
	zbIeeeAddr_t aIeeeAddress,  /* IN: The IEEE address used to match during the search and match process. */
	zbAddrMode_t mode           /* IN: The mode used to search and match. */
)
{
	index_t  cIndex;
	discoveryStoreTable_t *pPtr;

	for (cIndex = 0;cIndex < gMaximumDiscoveryStoreTableSize_c;cIndex++)
	{
		pPtr = &gaDiscoveryStoreTable[cIndex];
		if (mode == zbNwkAndIeeeMode)
		{
			if (IsEqual2Bytes(aNwkAddress, Zdp_CacheTableNwkAddr(pPtr)) &&
					IsEqual8Bytes(aIeeeAddress, Zdp_CacheTableIeeeAddr(pPtr)))
				return cIndex;
		}
		else if (mode == zbIEEEMode)
		{
			if (IsEqual8Bytes(aIeeeAddress, Zdp_CacheTableIeeeAddr(pPtr)))
				return cIndex;
		}
		else
		{
			if (IsEqual2Bytes(aNwkAddress, Zdp_CacheTableNwkAddr(pPtr)))
				return cIndex;
		}
	}
	/* To use the Discovery Cache Backup capability the gDiscovery_store_rsp must be on al lthe time*/
#if gBkup_Discovery_cache_d
	if (UpdateDiscoveryStoreTable(aNwkAddress, aIeeeAddress) == gZbSuccess_c)
		return gZbSuccess_c;
	else
		return gZdoInsufficientSpace_c;
#endif

#if !gBkup_Discovery_cache_d
	return gZdoNoEntry_c;
#endif
}
#endif

/*-------------------- Zdp_UpdateBlockAddressMap --------------------
	This function is a Global util function

	Calls the APS_AddTOAddressMap funtion and translates the returning status
	to a ZDO status value, this is handy when an answers depends of the 
	returning status.
	Updates the address map table.

	IN: The netowork address of the device of interest.
	IN: The IEEE address of the device of interest.

	OUT: A ZDO Status of the APS_AddToAddressMap action.
*/
zbStatus_t Zdp_UpdateBlockAddressMap
(
	zbNwkAddr_t  aNwkAddress,   /* IN: The netowork address of the device of interest. */
	zbIeeeAddr_t  aIeeeAddress  /* IN: The IEEE address of the device of interest. */
)
{
	/* add or update this item in the address map */
	if(APS_AddToAddressMap(aIeeeAddress, aNwkAddress) != gAddressMapFull_c)
		return gZbSuccess_c;

	return gZdoTableFull_c;
}

/*****************************************************************************/

#if gMgmt_NWK_Disc_rsp_d
void Zdp_Mgmt_Send_Nwk_disc_conf
(
	nlmeZdoMessage_t *pZdoMsg /*IN: Message received from Nwk Layer*/
)
{
	index_t  aIndex;
	index_t  cIndex = GetStartIndexforNwkDisc();
	/* The size of the response payload */
	zbSize_t dataLength = sizeof(zbMgmtNwkDiscResponse_t)- sizeof(zbOTAFormatNetworkDescriptor_t);
	/* The place where we will build the response */
	zbMgmtNwkDiscResponse_t *pMgmtNwkDiscResp;
	/* The network descriptor coming in the confir from NWK Layer */
	networkDesc_t *pNetworkDescriptor;

	/*memory will be freed by the lower layer*/
	zdpToAppMessage_t  *pMsg = NULL;

	/* The buffer where the response will be build */
	pMsg = AF_MsgAlloc();

	if( pMsg == NULL )
		return;

	/* Clean up the pMsg */
  /* gMaxRxTxDataLength_c is the allocated size of AF_MsgAlloc*/
	BeeUtilZeroMemory(pMsg, gMaxRxTxDataLength_c);
	/* The place where to build the payload */
	pMgmtNwkDiscResp = (zbMgmtNwkDiscResponse_t *)((uint8_t *)pMsg + SapHandlerDataStructureOffSet);
	/* The data coming form the confirm */
	pNetworkDescriptor = pZdoMsg->msgData.networkDiscoveryConf.pNetworkDescriptor; 

	/* The response status */
	pMgmtNwkDiscResp->status = pZdoMsg->msgData.networkDiscoveryConf.status;
	/* The valu of the index, where we start */
	pMgmtNwkDiscResp->startIndex = GetStartIndexforNwkDisc();
	/* The amount of entries */
	pMgmtNwkDiscResp->nwkCount = pZdoMsg->msgData.networkDiscoveryConf.networkCount;

	/* If the sttus of the confr is success then process the list of network desriptors */
	if (pMgmtNwkDiscResp->status == gZbSuccess_c)
	{
		for (aIndex = 0; cIndex <pMgmtNwkDiscResp->nwkCount; cIndex++)
		{
			/* If we can not fit any other entrie in the pay load quit */
			if ((dataLength + sizeof(zbOTAFormatNetworkDescriptor_t)) > ZdoMaximumPayLoadSize())
				break;

			/* Build each entrie of the network descriptors into the payload */
			FLib_MemCpy(pMgmtNwkDiscResp->nwkList[aIndex].aExtendedPanId, pNetworkDescriptor->aExtendedPanId, sizeof(zbIeeeAddr_t));
			pMgmtNwkDiscResp->nwkList[aIndex].logicalChannel = pNetworkDescriptor->logicalChannel;
			/* The Same byte has the Stack Profile and the ZigBee Version So: */
			/* Shift the zigbee version to the most significant bits in the byte */
			pMgmtNwkDiscResp->nwkList[aIndex].StackProfileAndZigBeeVersion = (pNetworkDescriptor->zigBeeVersion << gHalfByte_c);
			/* Fit the stack profile in the less significant bits in the byte */
			pMgmtNwkDiscResp->nwkList[aIndex].StackProfileAndZigBeeVersion |= pNetworkDescriptor->stackProfile;
			
			pMgmtNwkDiscResp->nwkList[aIndex].BeaconOrderAndSuperframeOrder = (pNetworkDescriptor->superframeOrder << gHalfByte_c);
			pMgmtNwkDiscResp->nwkList[aIndex].BeaconOrderAndSuperframeOrder |= pNetworkDescriptor->beaconOrder;
			pMgmtNwkDiscResp->nwkList[aIndex].permitJoining = pNetworkDescriptor->permitJoining;
			/* Increase the size of the payload for each descriptor included */
			dataLength += sizeof(zbOTAFormatNetworkDescriptor_t);
			aIndex++;
			pMgmtNwkDiscResp->nwkListCount++;
			pNetworkDescriptor++;
		}
	}
	/* Send the data out to next layer */
	Zdp_GenerateDataReq(GetClusterId(gIndex) | ResponseClusterId, GetAddress(gIndex), (afToApsdeMessage_t *)pMsg, dataLength);
	gIndex = ( ( ( gIndex ) <  MaxArray )? ( gIndex )++: 0 );
}
#endif

#if gMgmt_Leave_rsp_d
/*-------------------- Zdp_Mgmt_Send_leave_conf --------------------
	This fucntion sends the response back to a Mgmt_Leave_Req.

	The function receives a message from the network layer, get the status
	from the confirm, and frees the message, then allocates another buffer (bigger)
	to send the response back, the buffer needs to be big enough to build in
	all the headers from the lower layers.

	IN: Message received from Nwk Layer.

	OUT: NONE.
*/
void Zdp_Mgmt_Send_leave_conf
(
  nlmeZdoMessage_t *pZdoMsg  /* IN: Message received from Nwk Layer. */
)
{
  zbStatus_t  status;

  /*
    Response message for management leave request, the memory will be freed by the lower
    layers.
  */
  afToApsdeMessage_t *pMsg;

  /* Got the status of the leave command */
  status = pZdoMsg->msgData.leaveConf.status;

  /* Free the message and mark it a "Free" */
  MSG_Free(pZdoMsg);
  pZdoMsg = NULL;

  /* Allocates the message to send back the response */
  pMsg = AF_MsgAlloc();
  if(pMsg == NULL)
    return;

  /* Every place where the Zdp_GenerateDataReq is called the memory must be filled with Zeros */
  /* gMaxRxTxDataLength_c is the allocated size of AF_MsgAlloc*/
  BeeUtilZeroMemory(pMsg, gMaxRxTxDataLength_c); 
  /* Set the proper status in the response buffers */
  ((uint8_t *)pMsg + ApsmeGetAsduOffset())[0] = status;

  /* Send the response */
  Zdp_GenerateDataReq(gMgmt_Leave_rsp_c, mgmtLeaveResponseInfo, pMsg, sizeof(zbMgmtLeaveResponse_t));
}
#endif

#if gMgmt_Direct_Join_rsp_d && (gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d)
void Zdp_Mgmt_Direct_Join_conf
(
  nlmeZdoMessage_t *pZdoMsg
)
{
  zbStatus_t iStatus;
  zdpToAppMessage_t  *pZdpMsg;
  afToApsdeMessage_t *pMsg;

  /* Catch the respond status. */
  iStatus = pZdoMsg->msgData.directJoinConf.status;

  /* Free this buffer, we no longer neede and we need to allocte more memory. */
  MSG_Free(pZdoMsg);

  pZdpMsg = MSG_Alloc(MbrSizeof(zdpToAppMessage_t, msgType) + sizeof(zbMgmtDirectJoinResponse_t));

  if (!pZdpMsg)
    return;

  pZdpMsg->msgType = gMgmt_Direct_Join_rsp_c;

  pZdpMsg->msgData.mgmtDirectJoinResp.status = iStatus;

  (void)ZDP_APP_SapHandler(pZdpMsg);

  if (IsBroadcastAddress(aSrcAddress))
    return;

  pMsg = AF_MsgAlloc();

  if (!pMsg)
  {
    Copy2Bytes(aSrcAddress, gaBroadcastAddress);
    return;
  }

  BeeUtilZeroMemory(pMsg, gMaxRxTxDataLength_c);

  *((uint8_t *)pMsg + ApsmeGetAsduOffset()) = iStatus;

  Zdp_GenerateDataReq(gMgmt_Direct_Join_rsp_c, aSrcAddress, pMsg, sizeof(zbMgmtDirectJoinResponse_t));
  Copy2Bytes(aSrcAddress, gaBroadcastAddress);
}
#endif


/*-------------------- Zdp_MatchClusters --------------------
	This function is ZDP internal.

	Returns TRUE if the anything in the two cluster lists match.
	Returns FALSE if not.

	Only included if either match descriptor is defined, or this is the
	coordinator supporting end-device-bind.

	This function only keep the first match, the return need to move to
	track more than one.

	IN: count of input clusters.
	IN: count of output clusters.
	IN: input cluster list.
	IN: output cluster list.
*/
#if gMatch_Desc_rsp_d || ((gCoordinatorCapability_d || gComboDeviceCapability_d) && gBindCapability_d && gEnd_Device_Bind_rsp_d)
bool_t Zdp_MatchClusters
(
  zbCounter_t  cInClusters,   /* IN: count of input clusters */
  zbCounter_t  cOutClusters,  /* IN: count of output clusters */
  zbClusterId_t  *aInList,    /* IN: input cluster list */
  zbClusterId_t  *aOutList,   /* IN: output cluster list */
  bool_t  fUseArrayForMatch
)
{
  index_t  iOutCluster;
  index_t  iInCluster;
  bool_t  status = FALSE;  /* Assume no matches were found. */

  /* check if any output clusters match any input clusters */
  for(iOutCluster=0; iOutCluster < cOutClusters; iOutCluster++)
  {
    for(iInCluster=0; iInCluster < cInClusters; iInCluster++)
    {
      if(IsEqual2Bytes(aInList[iInCluster], aOutList[iOutCluster]))
      {
        if (fUseArrayForMatch)
        {
          /* Used for binding... */
          Copy2Bytes(gaZdpMatchedOutCluster[giMatchIndex],aOutList[iOutCluster]);
          giMatchIndex++;
        }
        /* found match */
        status = TRUE;
      }
    }
  }

  /* no matches found */
  return status;
}
#endif

#if gBind_Register_rsp_d
/*------------------- Zdp_UpDateBindingTableInfo -------------------
	This function is ZDP internal.

	This functions stores every Bind entrie coming into the BindRegister response command
	in its own source bindig table, the procedure adding binding entries by using the bind
	APS_BindReq.

	IN: The Bind Register Response package.

	OUT: The status of the store opertaion.
*/
zbStatus_t Zdp_UpDateBindingTableInfo
(
	zbBindRegisterResponse_t  *pRegisterResponse  /* IN: The Bind Register Response package. */
)
{
	uint8_t  cIndex;
	zbApsmeBindEntry_t  *pBindingEntry;

	pBindingEntry = (zbApsmeBindEntry_t *)pRegisterResponse->aBindingTableList;
	for (cIndex = 0; cIndex < TwoBytes2Byte(pRegisterResponse->aBindingTableListCount); cIndex++)
	{
		if (APSME_BindRequest((zbApsmeBindReq_t *)pBindingEntry) != gZbSuccess_c)
			return gZdoTableFull_c;
		if (pBindingEntry->dstAddrMode == zbIEEEMode)
      pBindingEntry = (zbApsmeBindEntry_t*) (((uint8_t *)pBindingEntry) + sizeof(zbApsmeBindEntry_t));
		else
      pBindingEntry = (zbApsmeBindEntry_t*) (((uint8_t *)pBindingEntry) + (MbrOfs(zbApsmeBindEntry_t, aDstAddr[0]) + sizeof(zbGroupId_t)) );
	}

	return gZbSuccess_c;
}
#endif

#if gSystem_Server_Discovery_rsp_d
/*-------------------- Zdp_UpdateSystemServerDiscoveryResponseTable --------------------
	This function is ZDP internal.

	Store the service and the address of the node which provides that service,
	is one server per array entrie whit it's prticular device address.
	If the first node who answer has all the services asked then only the information
	of that particular node willbe stored in the array.

	IN: The network address of the node answering with its services.
	IN: The mask with the services bits of the answering node.
*/
void Zdp_UpdateSystemServerDiscoveryResponseTable
(
	zbNwkAddr_t  aNwkAddress,    /* IN: The network address of the node answering with its services. */
	zbServerMask_t  aServerMask  /* IN: The mask with the services bits of the answering node. */
)
{
	index_t  cIndex;
	uint16_t  bitMask = 0x4000;  /* (0100 0000 0000 0000) bitMask initialized to check the Network Manager bit (littel endian) */
  
	zbSystemServerDiscoveryStore_t *pPtr;

	if (mSystemServerDiscoveryResponsesCounter >= gMaximumSystemServerciesResponses_c)
		return ;

	if (!TwoBytesToUint16(aServerMask))
		return ;
	
/*
  If the Network Manager bit was set in the System_Server_Discovery_rsp, then the
  Remote Device's NWK address shall be set into the nwkManagerAddr of the NIB. (2.4.4.10.2)
*/
	if (TwoBytesToUint16(aServerMask) & (bitMask)) 
	{
	  NlmeSetRequest(gNwkManagerAddr_c, aNwkAddress);
	}

#if gStandardSecurity_d || gHighSecurity_d
  /* if the incomming service is from Trust cente, then, update the info. */
  if (TwoBytesToUint16(aServerMask) & 0x0100)
  {
    ApsmeSetRequest(gApsTrustCenterNwkAddress_c, aNwkAddress);
    (void)APS_AddToAddressMapPermanent(ApsmeGetRequest(gApsTrustCenterAddress_c), ApsmeGetRequest(gApsTrustCenterNwkAddress_c));
  }
#endif

#if (gBigEndian_c)
	bitMask = 0x0100;   /* Reset bitMask */
#else
  bitMask = 0x0001;   /* Reset bitMask */
#endif	
	for (cIndex=0; cIndex < gMaximumSystemServerciesResponses_c; cIndex++)
	{
		pPtr = &mSystemServerDiscResponse[cIndex];

		if (Cmp2BytesToZero(pPtr->aServerMask) &&(TwoBytesToUint16(aServerMask) & bitMask))
		{
			Set2Bytes(pPtr->aServerMask,(bitMask<<cIndex));
			Copy2Bytes(pPtr->aNwkAddress, aNwkAddress);
			mSystemServerDiscoveryResponsesCounter++;
		}
		Set2Bytes(aServerMask,(TwoBytesToUint16(aServerMask)>>1));
	}
	return ;
}
#endif

/* WARNING: Function called by the NWK in the library, it must be present as a funciton all the time. */
void DeviceAnnounce(zbNwkAddr_t  aDstAddress)
{
#if gDevice_annce_d
  ASL_Device_annce(NULL, aDstAddress,
                   NlmeGetRequest(gNwkShortAddress_c),
                   NlmeGetRequest(gNwkIeeeAddress_c),
                   NlmeGetRequest(gNwkCapabilityInformation_c));
#else
(void)aDstAddress;
#endif
}

#if gMgmt_NWK_Disc_rsp_d
bool_t Zdp_IsNotValidCahnnel(uint32_t *pChannels)
{
#ifdef __IAR_SYSTEMS_ICC__
  uint32_t  channels = 0;
  uint32_t  c1 = 0x07FFF800;

  FLib_MemCpy(&channels, pChannels, sizeof(uint32_t));

  return (channels & c1)? FALSE:TRUE;
#else
  return  ((*(uint32_t *)(pChannels)) & 0x00F8FF07)? FALSE:TRUE;
#endif
}
#endif

/*****************************************************************************************
* Updates the addess of the announcing devices in the NT and RT tables.
* Handles 3 cases:
*   1) Non existing devices.
*   2) Existing End Devices.
*   3) Existing Routers.
*
* 1) Non existing devices:
*   -If the announcing device does not exist in the NT, and it is a ZED, we don't
*    process any further, we do not included ZEDs that are not our children.
*   -If the announcing device does not exist in the NT, and it is a ZR, we check
*    if the message is in our neighborhood, by comparing the sender address with
*    address contained in the packet, if so, then we add the node as sibling.
*
* 2) Existing End Devices:
*   -If the network address of the announcing ZED is different from the one
*    already stored in out NT, then, the ZED must be some elses child now, and
*    it no longer belong into our NT, so, we remove it.
*
* 3) Existing Routers:
*   -If the network address of the announcing ZR is different form the one
*    already stored in our NT, then, we erase the old routes, we consider the
*    device a new device, now for the NT, if the device is still in our neighborhood
*    we update the info, if not, we erase the NT entry, remeber to prove that
*    a node is on the neighborhood, we check for the message to be one hop away
*    comparing the sender address with address of the de device announcing.
*
* Interface assumption:
*   The parameter pMsg is not a nul pointer. The aprameter aSrcAddress is a valid
*   unicast address.
*
* Return value:
*   None.
******************************************************************************************/
void ZDP_UpdateNTAndRT
(
  zbEndDeviceAnnounce_t *pMsg,  /* IN: The device announce message indication. */
  zbNwkAddr_t aSrcAddress       /* The address of the last hop. */
)
{
  neighborTable_t  *pNTE;  /* Entry on NT to process. */

  /* We care about the IEEE address, the NWK address may change. */
  pNTE = IsInNeighborTable(gZbAddrMode64Bit_c, pMsg->aIeeeAddress);

  /* See if the node is not on the NT. */
  if (!pNTE)
  {
    /* Nothing more to do.! Do not add sibling on Device annce. */
    return;
  }

  /* From now and on, means that the devices was found on the NT. */

/****************************** Process Any Sibling Device *******************************/

  /* Any sibling re-announcing will be removed. NOTE: ZED are never our siblings. */
  if (pNTE->deviceProperty.bits.relationship == gNeighborIsSibling_c)
  {
    /* Clear all the routes that lead to that device. */
#if gRnplusCapability_d
#if gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d
    RemoveRouteEntry(pNTE->aNetworkAddress);
#endif
#endif


    /* We dont keep old siblings */
    ResetNeighborTableEntry(pNTE);

    /* Adios amigos! */
    return;
  }


/**************************** Process the child end devices *****************************/
  if (!(pMsg->capability & gDeviceType_c))
  {
    /*
      Ok, is on our NT but the address has change, this means it must be some one elses child.
      NOTE: On SP1 the reuse of the address depends on configuration on SP2 is a long shot,
      most of the cases the node gets the same address but this may not be always TRUE,
      the worst case scenario the node will be erase and added under some other circunstances.
    */
    if (!IsEqual2Bytes(pNTE->aNetworkAddress, pMsg->aNwkAddress))
    {

      /* We don't keep other node's children. */
      ResetNeighborTableEntry(pNTE);
    }

    /* Ok, we are done, lets go!! */
    return;
  }

/****************************** Process the child routers. ******************************/

   /* If the device is a router and our child then we should not remove it. */
  /*
    If the address has change, then, update the NT and process the route table. Otherwise
    there is nothing to do.
  */
  if (IsEqual2Bytes(pNTE->aNetworkAddress, pMsg->aNwkAddress))
  {
    /* Same address is our child again, so don't process the entry. */
    return;
  }

/*****************************************************************************************
* At this point we know that the address has change but not the reason, it can be that
* an address conflict has happen and the router change its own address, or it was rejoined
* into the network to some other parent (Nwk Rejoin or Associaion).
* - Any way, is better to clean the routes.
*
* - If the device is close to use update the NT with the address an set it as sibling.
*
* - And clear the secure material, we dont know if a rejoin has happen, in tha tcase we
*   need a clear material set.
******************************************************************************************/

  /*
    Remove the old routes to the announcing device. Use Old Address for Old routes, there
    is no routes to the new address, consider the easiest case that has a new address, and
    treat it as a new device, remember its parent and next hop may be different.
  */
#if gRnplusCapability_d
#if gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d
  RemoveRouteEntry(pNTE->aNetworkAddress);
#endif
#endif


  /* If the device is still in our neighborhood we update the info, if not, we erase the info. */
  if (IsEqual2Bytes(pMsg->aNwkAddress, aSrcAddress))
  {
    /*
      Update the NT, change the relationship to sibling. Relation ship on routers is not
      so important. We dont know if the devie got an address conflict or rejoin the nwk.
      After erasing the routes and the secure material, we just change the relationship and
      update the NT with the new address.
    */
    Copy2Bytes(pNTE->aNetworkAddress, pMsg->aNwkAddress);
    pNTE->deviceProperty.bits.relationship = gNeighborIsSibling_c;
    return;
  }

  /* Erase the info, the node is too far away. */
#if gRnplusCapability_d
#if gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d
    RemoveRouteEntry(pNTE->aNetworkAddress);
#endif
#endif  
  ResetNeighborTableEntry(pNTE);

  /* Se ya! */
}