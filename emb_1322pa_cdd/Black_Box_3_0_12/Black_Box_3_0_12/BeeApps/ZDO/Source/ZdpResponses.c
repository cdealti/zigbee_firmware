/******************************************************************************
* This is a source file that handles ZDP primitives
*
* (c) Copyright 2008, Freescale, Inc. All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
******************************************************************************/
#include "EmbeddedTypes.h"
#include "Zigbee.h"
#include "BeestackConfiguration.h"
#include "BeeCommon.h"

#include "ZdpManager.h"
#include "FunctionLib.h"
#include "ZdpUtils.h"
#include "NwkCommon.h"
#include "AfZdoInterface.h"
#include "ZDOStateMachineHandler.h"
#include "ZdpResponses.h"
#include "ZdoNwkManager.h"
#include "ZdoNwkInterface.h"
#include "BeeStackParameters.h"

#if (gCoordinatorCapability_d || gComboDeviceCapability_d) && gBindCapability_d && gEnd_Device_Bind_rsp_d
#include "ZdoBindManager.h"
#endif


#define mMinEndPoint 0
#define mMaxEndPoint 0xFE


/******************************************************************************
*******************************************************************************
* Public functions
*******************************************************************************
******************************************************************************/

#if ((gStandardSecurity_d || gHighSecurity_d) && gApsMaxEntriesForPermissionsTable_c)
extern bool_t CommandHasPermission
(
  zbNwkAddr_t aSrcAddr,
  permissionsFlags_t  permissionsCategory
);
#endif

#if gMgmt_Direct_Join_rsp_d
extern uint8_t ZDP_APP_SapHandler
(
  zdpToAppMessage_t *pMsg
);
#endif


/* NONE */

/**********************************************************************
***********************************************************************
* Private Memory Declarations
***********************************************************************
***********************************************************************/
extern bool_t  mbIsNotFree;

#if gMgmt_Leave_rsp_d
zbMgmtOptions_t gMgmtSelfLeaveMask;

zbNwkAddr_t  mgmtLeaveResponseInfo;
#endif

#if gMgmt_NWK_Disc_rsp_d
index_t  gIndex = 0;
bool_t  gMgmtNwkDiscReq = FALSE;
index_t  gStartIndexforNwkDisc;
#endif

#if gMgmt_NWK_Disc_rsp_d
strAddrClusterId_t  strAddrClusterId[MaxArray];
#endif

#if gMgmt_Lqi_rsp_d && ( gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d)

/* This offset differs from the one defined in BeeStack_Globals.h because the
format of the neighbor table that it is been sent over the air is different to
the neighbor table used internally */
#define mReltnShipBitOffset_c  4 

#endif

#if gMgmt_Direct_Join_rsp_d
zbNwkAddr_t  aSrcAddress = {0xff,0xff};
#endif

#if (gDiscovery_store_rsp_d || gRemove_node_cache_rsp_d || gMgmt_Cache_rsp_d)
zbCounter_t  mDiscoveryStoreTableEntrys = 0;
#endif

#if (gBindCapability_d && (gBind_rsp_d || gUnbind_rsp_d)) || gReplace_Device_rsp_d || gBind_Register_rsp_d
zbEventProcessQueue_t  maEventProcessQueue[gMaximumEventsToProcess_c] = {0};
#endif

extern bool_t gIsABroadcast;

#if gBkup_Discovery_cache_d || gDiscovery_store_rsp_d || gMgmt_Cache_rsp_d || gRemove_node_cache_rsp_d || gFind_node_cache_rsp_d|| gActive_EP_store_rsp_d || gPower_Desc_store_rsp_d || gNode_Desc_store_rsp_d || gSimple_Desc_store_rsp_d
discoveryStoreTable_t  gaDiscoveryStoreTable[gMaximumDiscoveryStoreTableSize_c] = {0};
#endif

#if gBind_Register_rsp_d || gRecover_Source_Bind_rsp_d || gBackup_Source_Bind_rsp_d || (gBindCapability_d && gBind_rsp_d)
zbIeeeAddr_t  aDevicesHoldingItsOwnBindingInfo[gMaximumDevicesHoldingBindingInfo_c] = {0};
zbCounter_t  gDeviceHoldingItsOwnBindingInfo_Counter = 0;
#endif

#if (gReplace_Device_rsp_d || gRemove_Bkup_Bind_Entry_rsp_d || gBackup_Bind_Table_rsp_d || gStore_Bkup_Bind_Entry_rsp_d || gRecover_Bind_Table_rsp_d || gBind_Register_rsp_d || gMgmt_Bind_rsp_d)
zbApsmeBindEntry_t  gaBindingCacheTable[gMaximumBindingCacheTableList_c] = {0};
zbSize_t  gBindingCacheTableCounter;
#endif

#if (gCoordinatorCapability_d || gComboDeviceCapability_d) && gBindCapability_d && gEnd_Device_Bind_rsp_d
extern bool_t gZdoInEndDeviceBind;
#endif

/*-------------------- gaServeMask --------------------
	Table 2.29 Server Mask Bit Assignments

	Bit number:
    0->Primary Trust Center.
    1->Backup Trust Center.
    2->Primary Binding table cache.
    3->Backup bindin table cache.
    4->Primary Discovery Cache.
    5->Backup Discovery Cache.
    6->Network Manager.
    7-15->Reserved.
*/
zbServerMask_t  gaServerMask = {gServerMask_c};


/*========================================== RESPONSES ==========================================*/

/*==============================================================================================
	=============== 2.4.4.1 Device and Service Discovery Server Service Primitives ===============
	==============================================================================================*/
#if gSimple_Desc_rsp_d
/*-------------------- Simple_Desc_rsp --------------------
	2.4.4.1.5 Simple_Desc_rsp. (ClusterID=0x8004)

	The Simple_Desc_rsp is generated by a remote device in response to a
	Simple_Desc_req directed to the remote device. This command shall be unicast to
	the originator of the Simple_Desc_req command.

	IN: The package with the request information.
	IN/OUT: The buffer where the response data will be filled in.

	OUT: The size in bytes of the request payload.
*/
zbSize_t Simple_Desc_rsp
(
  zbSimpleDescriptorRequest_t  *pMessageComingIn,  /* IN: The package with the request information. */
  zbSimpleDescriptorResponse_t  *pMessageGoingOut  /* IN/OUT: The buffer where the response data will be filled in. */
)
{
  zbSimpleDescriptor_t  *pSimpleDesc;
  uint8_t  *pPtr;

#if (gSimple_Desc_store_rsp_d && ( gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d))
  index_t  cIndex;
  index_t  auxIndex;
  discoveryStoreTable_t  *pStore;
  cSimpleDescriptor_t  *pList;
#endif

  if (!(IsSelfNwkAddress(pMessageComingIn->aNwkAddrOfInterest)))
  {
#if (gSimple_Desc_store_rsp_d && ( gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d))
#if gComboDeviceCapability_d
    if( NlmeGetRequest(gDevType_c) != gEndDevice_c )
#endif
    {
      cIndex = ZDP_SearchNodeInCache(pMessageComingIn->aNwkAddrOfInterest, NULL, zbNullMode);
      if (cIndex == gZdoNoEntry_c)
      {
        pMessageGoingOut->status = gZdoDeviceNotFound_c;
      }
      else
      {
        pStore = &gaDiscoveryStoreTable[cIndex];
        for (auxIndex = 0; auxIndex < gMaximumSimpleDescriptorList_c;auxIndex++)
        {
          pList = &pStore->simpleDescriptorList[auxIndex];
          if (Cmp2BytesToZero(pList->aAppProfId))
          {
            pMessageGoingOut->status = gZdoNoDescriptor_c;
            break;
          }
          if (pList->endPoint != pMessageComingIn->endPoint)
          {
            continue;
          }
          pMessageGoingOut->length = MbrOfs(zbSimpleDescriptorPackageResponse_t, inClusterList.aClusterList[0]);
          FLib_MemCpy(&pMessageGoingOut->simpleDescriptor, pList, pMessageGoingOut->length);

          FLib_MemCpy(pMessageGoingOut->simpleDescriptor.inClusterList.aClusterList, 
                      pList->appInClusterList, 
                      (pMessageGoingOut->simpleDescriptor.inClusterList.cNumClusters * sizeof(zbClusterId_t)));

          pMessageGoingOut->length += (pMessageGoingOut->simpleDescriptor.inClusterList.cNumClusters * sizeof(zbClusterId_t));
          pPtr = (uint8_t *)((uint8_t *)&(pMessageGoingOut->simpleDescriptor) + pMessageGoingOut->length);
          pPtr[0] = pList->appNumOutClusters;
          pMessageGoingOut->length += MbrSizeof(cSimpleDescriptor_t, appNumOutClusters);

          FLib_MemCpy(&pPtr[1],pList->appOutClusterList,(pList->appNumOutClusters * sizeof(zbClusterId_t)));
          pMessageGoingOut->length += (pList->appNumOutClusters * sizeof(zbClusterId_t));

          /* The ouotgoing bugger is filled with Zeros, so pMessageGoingOut->status = gZbSuccess_c; is no longer needed */
          Copy2Bytes(pMessageGoingOut->aNwkAddrOfInterest, pStore->aNwkAddress);
          return (MbrOfs(zbSimpleDescriptorResponse_t, simpleDescriptor) + pMessageGoingOut->length);
        }
      }
    }/* if( NlmeGetRequest(gDevType_c) != gEndDevice_c ) */
#endif

   /* If we reach this point it means not child has stored anything on the cache or we dont have a cache, any way is invalid. */
   pMessageGoingOut->status = gZdoInvalidRequestType_c;
    Copy2Bytes(pMessageGoingOut->aNwkAddrOfInterest, pMessageComingIn->aNwkAddrOfInterest);
    return (MbrOfs(zbSimpleDescriptorResponse_t, simpleDescriptor));
  }

  /* The ouotgoing bugger is filled with Zeros, so pMessageGoingOut->status = gZbSuccess_c; is no longer needed */

  /* Copy the Addres of the device */
  Copy2Bytes(pMessageGoingOut->aNwkAddrOfInterest, pMessageComingIn->aNwkAddrOfInterest);

  /*check if it is a valid endpoint*/
  if ( pMessageComingIn->endPoint == mMinEndPoint || pMessageComingIn->endPoint > mMaxEndPoint ) 
  {
    pMessageGoingOut->status = gZdoInvalidEndPoint_c;
    return (MbrOfs(zbSimpleDescriptorResponse_t, simpleDescriptor));
  }

  /* The simple Descriptor Pointer for this particualr EndPoint */
  pSimpleDesc = AF_FindEndPointDesc( pMessageComingIn->endPoint );

  /*If the endpoint is not valid, function returns NULL pointer*/
  if( pSimpleDesc == NULL)
  {
    pMessageGoingOut->status = gZdoNotActive_c;
    return (MbrOfs(zbSimpleDescriptorResponse_t, simpleDescriptor));
  }

  pMessageGoingOut->length = MbrOfs(zbSimpleDescriptorPackageResponse_t, inClusterList.aClusterList[0]);

  FLib_MemCpy(&pMessageGoingOut->simpleDescriptor,pSimpleDesc, pMessageGoingOut->length);

  FLib_MemCpy(pMessageGoingOut->simpleDescriptor.inClusterList.aClusterList,
              pSimpleDesc->pAppInClusterList,
              (pMessageGoingOut->simpleDescriptor.inClusterList.cNumClusters * sizeof(zbClusterId_t)));

  pMessageGoingOut->length += (pMessageGoingOut->simpleDescriptor.inClusterList.cNumClusters * sizeof(zbClusterId_t));

  pPtr = (uint8_t *)((uint8_t *)&(pMessageGoingOut->simpleDescriptor) + pMessageGoingOut->length);

  pPtr[0] = pSimpleDesc->appNumOutClusters;
  pMessageGoingOut->length += MbrSizeof(cSimpleDescriptor_t, appNumOutClusters);

  FLib_MemCpy(&pPtr[1],pSimpleDesc->pAppOutClusterList,(pSimpleDesc->appNumOutClusters * sizeof(zbClusterId_t)));
  pMessageGoingOut->length += (pSimpleDesc->appNumOutClusters * sizeof(zbClusterId_t));

  return (MbrOfs(zbSimpleDescriptorResponse_t, simpleDescriptor) + pMessageGoingOut->length);
}
#endif

#if gExtended_Simple_Desc_rsp_d
	/*-------------------- Extended Simple_Desc_rsp --------------------
		2.4.4.1.20 Extended_Simple_Desc_rsp. (ClusterID=0x801D)

	The Extended_Simple_Desc_rsp is generated by a remote device in response to an
  Extended_Simple_Desc_req directed to the remote device. This command shall
  be unicast to the originator of the Extended_Simple_Desc_req command.

	IN: The package with the request information.
	IN/OUT: The buffer where the response data will be filled in.

	OUT: The size in bytes of the request payload.
*/
zbSize_t Extended_Simple_Desc_rsp
(
	zbExtSimpleDescriptorRequest_t  *pMessageComingIn,  /* IN: The package with the request information. */
	zbExtSimpleDescriptorResponse_t  *pMessageGoingOut  /* IN/OUT: The buffer where the response data will be filled in. */
)
{
	zbSimpleDescriptor_t  *pSimpleDesc;
	uint16_t numOfClustersToCopy = 0;
	uint8_t numOfInClusterToCopy = 0;
	uint8_t numOfOutClusterToCopy = 0;
	uint8_t maxClusterListPayload;

#if gSimple_Desc_store_rsp_d
	index_t  cIndex;
	index_t  auxIndex;
	discoveryStoreTable_t  *pStore;
	cSimpleDescriptor_t  *pList;
#endif

	/* Fill the start Index and the End point */
  pMessageGoingOut->startIndex = pMessageComingIn->startIndex;
	pMessageGoingOut->endPoint = pMessageComingIn->endPoint;
	
	if (!(IsSelfNwkAddress(pMessageComingIn->aNwkAddrOfInterest)))
	{
#if gSimple_Desc_store_rsp_d
		cIndex = ZDP_SearchNodeInCache(pMessageComingIn->aNwkAddrOfInterest, NULL, zbNullMode);
		if (cIndex == gZdoNoEntry_c)
		{
			pMessageGoingOut->status = gZdoDeviceNotFound_c;
		}
		else
		{
			pStore = &gaDiscoveryStoreTable[cIndex];
			for (auxIndex = 0; auxIndex < gMaximumSimpleDescriptorList_c;auxIndex++)
			{
				pList = &pStore->simpleDescriptorList[auxIndex];
				if (Cmp2BytesToZero(pList->aAppProfId))
				{
					pMessageGoingOut->status = gZdoNoDescriptor_c;
					break;
				}
				if (pList->endPoint != pMessageComingIn->endPoint)
				{
					continue;
				}
				/* Fill the number of app input and output clusters from the Desc. of the endpoint*/
				pMessageGoingOut->appNumInClusters = pList->appNumInClusters;
				pMessageGoingOut->appNumOutClusters = pList->appNumOutClusters;

        /* Get the total clusters */
        numOfClustersToCopy = (uint16_t)pList->appNumInClusters + (uint16_t)pList->appNumOutClusters;
        /* Calc. the maxim app clusters list payload (in bytes)  */
        maxClusterListPayload = ZdoMaximumPayLoadSize() - MbrOfs(zbExtSimpleDescriptorResponse_t,appClusterList);
        /* Calc. the maxim app clusters list (in clusters number) */
        maxClusterListPayload = maxClusterListPayload/sizeof(zbClusterId_t);
        /* Calc. the number of clusters that need to be copied */
        if(pMessageComingIn->startIndex < numOfClustersToCopy)
        {
          numOfClustersToCopy = numOfClustersToCopy - (uint16_t)pMessageComingIn->startIndex;
         
          /* Check if there are too many clusters that need to be copied in the payload???*/
          if(numOfClustersToCopy > maxClusterListPayload) 
            numOfClustersToCopy = maxClusterListPayload; 
         
          /* Calc. the input and output clusters number to be copied*/
          if(pMessageComingIn->startIndex <  pList->appNumInClusters) 
            {
              numOfInClusterToCopy = pList->appNumInClusters - pMessageComingIn->startIndex; 
              numOfOutClusterToCopy = numOfClustersToCopy - numOfInClusterToCopy;
            } 
            else {
            /* There are only output clusters */
              numOfOutClusterToCopy =  (uint8_t)numOfClustersToCopy;  
            }
         } 
        else{
          numOfClustersToCopy = 0;
        }
              
        if(numOfInClusterToCopy) 
         { 
           FLib_MemCpy( &pMessageGoingOut->appClusterList[0],
                        &pList->appInClusterList[pMessageComingIn->startIndex],
                        (numOfInClusterToCopy*sizeof(zbClusterId_t)) );
           if(numOfOutClusterToCopy)             
             /* Copy the remaining pList->appOutClusterList */
             FLib_MemCpy( &pMessageGoingOut->appClusterList[numOfInClusterToCopy],
                          &pList->appOutClusterList[0],
                         (numOfOutClusterToCopy * sizeof(zbClusterId_t)) );
                   
         }
         else 
         {
          if(numOfOutClusterToCopy)
             FLib_MemCpy( &pMessageGoingOut->appClusterList[0],
                          &pList->appOutClusterList[(pMessageComingIn->startIndex - pList->appNumInClusters)],
                          (numOfOutClusterToCopy * sizeof(zbClusterId_t)) );
         }
        
        /* The outgoing bugger is filled with Zeros, so the pMessageGoingOut->appClusterList is not needed to complete
           with empty data if no match is found or conditions above are FALSE*/                 
        /* The ouotgoing bugger is filled with Zeros, so pMessageGoingOut->status = gZbSuccess_c; is no longer needed */
        Copy2Bytes(pMessageGoingOut->aNwkAddrOfInterest, pStore->aNwkAddress);
        return (MbrOfs(zbExtSimpleDescriptorResponse_t, appClusterList) 
                        + (numOfClustersToCopy * sizeof(zbClusterId_t)) );
			}/* end for..*/ 
		} /*end else if (!(IsSelfNwkAddress(...*/
		
#else
		pMessageGoingOut->status = gZdoInvalidRequestType_c;
#endif
		Copy2Bytes(pMessageGoingOut->aNwkAddrOfInterest, pMessageComingIn->aNwkAddrOfInterest);
		return (MbrOfs(zbExtSimpleDescriptorResponse_t, appClusterList));
	}

	/* The ouotgoing bugger is filled with Zeros, so pMessageGoingOut->status = gZbSuccess_c; is no longer needed */

	/* Copy the Addres of the device */
	Copy2Bytes(pMessageGoingOut->aNwkAddrOfInterest, pMessageComingIn->aNwkAddrOfInterest);

	/*check if it is a valid endpoint*/
	if ( pMessageComingIn->endPoint == mMinEndPoint || pMessageComingIn->endPoint > mMaxEndPoint ) 
	{
		pMessageGoingOut->status = gZdoInvalidEndPoint_c;
		return (MbrOfs(zbExtSimpleDescriptorResponse_t, appClusterList));
	}

	/* The simple Descriptor Pointer for this particualr EndPoint */
	pSimpleDesc = AF_FindEndPointDesc( pMessageComingIn->endPoint );

	/*If the endpoint is not valid, function returns NULL pointer*/
	if( pSimpleDesc == NULL)
	{
		pMessageGoingOut->status = gZdoNotActive_c;
		return (MbrOfs(zbExtSimpleDescriptorResponse_t, appClusterList));
	}
  
  /* Fill the number of app input and output clusters from the Desc. of the endpoint*/
  pMessageGoingOut->appNumInClusters = pSimpleDesc->appNumInClusters;
  pMessageGoingOut->appNumOutClusters = pSimpleDesc->appNumOutClusters;
	
  /* Get the total clusters */
  numOfClustersToCopy = (uint16_t)pSimpleDesc->appNumInClusters + (uint16_t)pSimpleDesc->appNumOutClusters;
  /* Calc. the maxim app clusters list payload (in bytes)  */
  maxClusterListPayload = ZdoMaximumPayLoadSize() - MbrOfs(zbExtSimpleDescriptorResponse_t,appClusterList);
  /* Calc. the maxim app clusters list (in clusters number) */
  maxClusterListPayload = maxClusterListPayload/sizeof(zbClusterId_t);
  /* Calc. the number of clusters that need to be copied */
  if(pMessageComingIn->startIndex < numOfClustersToCopy)
  {
    numOfClustersToCopy = numOfClustersToCopy - (uint16_t)pMessageComingIn->startIndex;
   
    /* Check if there are too many clusters that need to be copied in the payload???*/
    if(numOfClustersToCopy > maxClusterListPayload) 
      numOfClustersToCopy = maxClusterListPayload; 
   
    /* Calc. the input and output clusters number to be copied*/
    if(pMessageComingIn->startIndex <  pSimpleDesc->appNumInClusters) 
      {
        numOfInClusterToCopy = pSimpleDesc->appNumInClusters - pMessageComingIn->startIndex; 
        numOfOutClusterToCopy = numOfClustersToCopy - numOfInClusterToCopy;
      } 
      else {
      /* There are only output clusters */
        numOfOutClusterToCopy =  (uint8_t)numOfClustersToCopy;  
      }
  }
  else{
    numOfClustersToCopy = 0;
  }
  
  if(numOfInClusterToCopy) 
   { 
     FLib_MemCpy( &pMessageGoingOut->appClusterList[0],
                  (uint8_t*)(pSimpleDesc->pAppInClusterList + (sizeof(zbClusterId_t)*pMessageComingIn->startIndex)),
                  (numOfInClusterToCopy*sizeof(zbClusterId_t)) );
     if(numOfOutClusterToCopy)             
       /* Copy the remaining pSimpleDesc->appOutClusterList */
       FLib_MemCpy( &pMessageGoingOut->appClusterList[numOfInClusterToCopy],
                    pSimpleDesc->pAppOutClusterList,
                   (numOfOutClusterToCopy * sizeof(zbClusterId_t)) );
             
   }
   else 
   {
    if(numOfOutClusterToCopy)
       FLib_MemCpy( &pMessageGoingOut->appClusterList[0],
                    (uint8_t*)(pSimpleDesc->pAppOutClusterList 
                    + sizeof(zbClusterId_t)*(pMessageComingIn->startIndex - pSimpleDesc->appNumInClusters) ),
                    (numOfOutClusterToCopy * sizeof(zbClusterId_t)) );
   }

	return (MbrOfs(zbExtSimpleDescriptorResponse_t, appClusterList) + (numOfClustersToCopy*sizeof(zbClusterId_t)));

}
#endif


#if gActive_EP_rsp_d
/*-------------------- Active_EP_rsp --------------------
	2.4.4.1.6 Active_EP_rsp. (ClusterID=0x8005)

	The Active_EP_rsp is generated by a remote device in response to an
	Active_EP_req directed to the remote device. This command shall be unicast to
	the originator of the Active_EP_req command.

	IN/OUT: The buffer where the response data will be filled in.
	IN: The address comingin the request package.

	OUT: The size in bytes of the request payload.
*/
zbSize_t Active_EP_rsp
(
	zbActiveEpResponse_t *pMessageGoingOut,  /* IN/OUT: The buffer where the response data will be filled in. */
	zbNwkAddr_t  aNwkAddress                 /* IN: The address comingin the request package. */
)
{
	uint8_t  dataLength = 0;
	uint8_t *pEndPointList;
#if (gActive_EP_store_rsp_d && ( gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d))
	index_t  cIndex;
	discoveryStoreTable_t  *pPtr;
#endif

	Copy2Bytes( pMessageGoingOut->aNwkAddrOfInterest , aNwkAddress);
	dataLength = MbrOfs(zbActiveEpResponse_t, pActiveEpList[0]);
	if (IsSelfNwkAddress(aNwkAddress))
	{
		/* The Outgoing buffer is set all to Zeros, so pMessageGoingOut->status = gZbSuccess_c; is no longer needed */
		pEndPointList = AF_EndPointsList(&pMessageGoingOut->activeEpCount);

		FLib_MemCpy(pMessageGoingOut->pActiveEpList,pEndPointList, pMessageGoingOut->activeEpCount);
	}
	else
	{
#if (gActive_EP_store_rsp_d && ( gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d))
#if gComboDeviceCapability_d
    if( NlmeGetRequest(gDevType_c) != gEndDevice_c )
#endif
    {
  		cIndex = ZDP_SearchNodeInCache(aNwkAddress, NULL, zbNullMode);
  		pPtr = &gaDiscoveryStoreTable[cIndex];
  		if (cIndex == gZdoNoEntry_c)
  		{
  			pMessageGoingOut->status = gZdoDeviceNotFound_c;
  			/* The Outgoing buffer is filled with Zeros, so pMessageGoingOut->activeEpCount = 0; is no longer needed */
  			return (dataLength);
  		}

  		if (!pPtr->activeEPCount)
  		{
  			/* If the address is set but the ActiveEP list is empty respond no descriptor.See 2.4.4.1.6.1 When Generated */
  			pMessageGoingOut->status = gZdoNoDescriptor_c;
  			return (dataLength);
  		}

  		/* The Outgoing buffer is filled with Zeros, so pMessageGoingOut->status = gZbSuccess_c; is no longer needed */
  		FLib_MemCpy(pMessageGoingOut->pActiveEpList, pPtr->activeEPList, pPtr->activeEPCount);
  		pMessageGoingOut->activeEpCount = pPtr->activeEPCount;
    }
#endif

#if gEndDevCapability_d || gComboDeviceCapability_d
#if gComboDeviceCapability_d
    if( NlmeGetRequest(gDevType_c) == gEndDevice_c )
#endif
    {
      /* The outgoing buffer is set all to Zeros, so only the status field in the response nedd to be changed */
		  pMessageGoingOut->status = gZdoInvalidRequestType_c;
		  return (dataLength);
    }
#endif
	}
	return (dataLength + pMessageGoingOut->activeEpCount);
}
#endif

#if gExtended_Active_EP_rsp_d
/*-------------------- Extended_Active_EP_rsp --------------------
  2.4.4.1.21 Extended_Active_EP_rsp. (ClusterID=0x801E)

	The Extended_Active_EP_rsp is generated by a remote device in response to an
  Extended_Active_EP_req directed to the remote device. This command shall be
  unicast to the originator of the Extended_Active_EP_req command.
	

	IN/OUT: The buffer where the response data will be filled in.
	IN: The address comingin the request package.

	OUT: The size in bytes of the request payload.
*/
zbSize_t Extended_Active_EP_rsp
(
	zbExtActiveEpResponse_t *pMessageGoingOut,  /* IN/OUT: The buffer where the response data will be filled in. */
	zbNwkAddr_t  aNwkAddress,                 /* IN: The address comingin the request package. */
  zbIndex_t startIndex
)
{
	uint8_t  dataLength = 0;
	uint8_t *pEndPointList;
	uint8_t maxEPListPayload;
	uint8_t numOfEndPointToCopy = 0;
#if gActive_EP_store_rsp_d
	index_t  cIndex;
	discoveryStoreTable_t  *pPtr;
#endif

	Copy2Bytes( pMessageGoingOut->aNwkAddrOfInterest , aNwkAddress);
	pMessageGoingOut->startIndex = startIndex;
	dataLength = MbrOfs(zbExtActiveEpResponse_t, pActiveEpList[0]);
	if (IsSelfNwkAddress(aNwkAddress))
	{
		/* The Outgoing buffer is set all to Zeros, so pMessageGoingOut->status = gZbSuccess_c; is no longer needed */
		pEndPointList = AF_EndPointsList(&pMessageGoingOut->activeEpCount);
		if (startIndex < pMessageGoingOut->activeEpCount) 
		{
  		  
  		numOfEndPointToCopy = (pMessageGoingOut->activeEpCount - startIndex);
  		/* Calc. the maxim EndPoint list payload (in bytes)  */
      maxEPListPayload = ZdoMaximumPayLoadSize() - dataLength;
      if(numOfEndPointToCopy > maxEPListPayload)
         numOfEndPointToCopy = maxEPListPayload;
  		FLib_MemCpy(pMessageGoingOut->pActiveEpList, (pEndPointList+startIndex), numOfEndPointToCopy );
    }
	}
	else
	{
#if gActive_EP_store_rsp_d
		cIndex = ZDP_SearchNodeInCache(aNwkAddress, NULL, zbNullMode);
		pPtr = &gaDiscoveryStoreTable[cIndex];
		if (cIndex == gZdoNoEntry_c)
		{
			pMessageGoingOut->status = gZdoDeviceNotFound_c;
			/* The Outgoing buffer is filled with Zeros, so pMessageGoingOut->activeEpCount = 0; is no longer needed */
			return (dataLength);
		}

		if (!(pPtr->activeEPCount))
		{
			/* If the address is set but the ActiveEP list is empty respond no descriptor.See 2.4.4.1.6.1 When Generated */
			pMessageGoingOut->status = gZdoNoDescriptor_c;
			return (dataLength);
		}
    pMessageGoingOut->activeEpCount = pPtr->activeEPCount;
    if (startIndex < pMessageGoingOut->activeEpCount) 
		{
      numOfEndPointToCopy = (pMessageGoingOut->activeEpCount - startIndex);
  		/* Calc. the maxim EndPoint list payload (in bytes)  */
      maxEPListPayload = ZdoMaximumPayLoadSize() - dataLength;
      if(numOfEndPointToCopy > maxEPListPayload)
         numOfEndPointToCopy = maxEPListPayload;
  		/* The Outgoing buffer is filled with Zeros, so pMessageGoingOut->status = gZbSuccess_c; is no longer needed */
  		FLib_MemCpy(pMessageGoingOut->pActiveEpList, (pPtr->activeEPList+startIndex), numOfEndPointToCopy);
		}
		
#else
		/* The outgoing buffer is set all to Zeros, so only the status field in the response nedd to be changed */
		pMessageGoingOut->status = gZdoInvalidRequestType_c;
		return (dataLength);
#endif
	}
	return (dataLength + numOfEndPointToCopy);
}
#endif

#if gMatch_Desc_rsp_d
/*-------------------- Match_Desc_rsp --------------------
	2.4.4.1.7 Match_Desc_rsp. (ClusterID=0x8006)

	The Match_Desc_rsp is generated by a remote device in response to a
	Match_Desc_req either broadcast or directed to the remote device. This command
	shall be unicast to the originator of the Match_Desc_req command.

	IN: The package with the request information.
	IN/OUT: The buffer where the response data will be filled in.

	OUT: The size in bytes of the request payload.
*/
zbSize_t Match_Desc_rsp
(
  zbMatchDescriptorRequest_t *pMessageComingIn,  /* IN: The package with the request information. */
  zbMatchDescriptorResponse_t *pMessageGoingOut  /* IN/OUT: The buffer where the response data will be filled in. */
)
{
  zbSimpleDescriptor_t  *pSimpledesc;
  zbEndPoint_t  *pEndPointList;
  zbCounter_t  numOfActiveEndPoints;
  index_t  cIndex;
  zbPtrClusterList_t  outputClusterList;
  zbCounter_t *pPtr;

  mbIsNotFree = TRUE;

  pPtr = (zbCounter_t *)&pMessageComingIn->aClusterList[pMessageComingIn->cNumClusters];
  outputClusterList.cNumClusters = pPtr[0];
  pPtr++;
  outputClusterList.pClusterList = (zbClusterId_t *)pPtr;

  if (!IsEqual2Bytes(pMessageComingIn->aNwkAddrOfInterest, gaBroadcastRxOnIdle) &&
      !IsSelfNwkAddress(pMessageComingIn->aNwkAddrOfInterest))
  {
#if gEndDevCapability_d || gComboDeviceCapability_d
#if gComboDeviceCapability_d
    if (NlmeGetRequest(gDevType_c) == gEndDevice_c)
#endif
    pMessageGoingOut->status = gZdoInvalidRequestType_c;
#else
    pMessageGoingOut->status = gZdoNoDescriptor_c;
#endif
    if(gIsABroadcast)
    {
      gIsABroadcast = FALSE;
      return gZero_c;
    }
    return MbrOfs(zbMatchDescriptorResponse_t, matchList[0]);
  }

  /* We are responding so we set our own address into the packet. */
  Copy2Bytes(pMessageGoingOut->aNwkAddrOfInterest, NlmeGetRequest(gNwkShortAddress_c));

  pEndPointList = AF_EndPointsList( &numOfActiveEndPoints );
  if (!pEndPointList)
  {
    /* We got no EP registered, the request can not be answered. */
    pMessageGoingOut->status = gZdoInvalidRequestType_c;
    return MbrOfs(zbMatchDescriptorResponse_t, matchList[0]);
  }

  for( cIndex=0; cIndex < numOfActiveEndPoints ; cIndex++ )
  {
    /* Get each EP one at the time in the search for the profile. */
    pSimpledesc = AF_FindEndPointDesc( pEndPointList[cIndex]);
    if (!IsEqual2Bytes(pSimpledesc->aAppProfId, pMessageComingIn->aProfileId))
    {
      /* Sorry Wrong profile Id move into the next EP on the list. */
      continue;
    }
    /* At least one of the list must have at least a cluster, and must be a match on any side. */
    if (((pMessageComingIn->cNumClusters != gZero_c)||(outputClusterList.cNumClusters != gZero_c)) &&
/*        
        ((Zdp_MatchClusters(pMessageComingIn->cNumClusters, pSimpledesc->appNumOutClusters, pMessageComingIn->aClusterList, (zbClusterId_t *)pSimpledesc->pAppOutClusterList,FALSE))||
        (Zdp_MatchClusters(pSimpledesc->appNumInClusters, outputClusterList.cNumClusters, (zbClusterId_t *)pSimpledesc->pAppInClusterList, outputClusterList.pClusterList, FALSE))))
*/
        ((Zdp_MatchClusters(pMessageComingIn->cNumClusters, pSimpledesc->appNumInClusters, pMessageComingIn->aClusterList, (zbClusterId_t *)pSimpledesc->pAppInClusterList,FALSE))||
        (Zdp_MatchClusters(pSimpledesc->appNumOutClusters, outputClusterList.cNumClusters, (zbClusterId_t *)pSimpledesc->pAppOutClusterList, outputClusterList.pClusterList, FALSE))))
        
    {
      pMessageGoingOut->matchList[pMessageGoingOut->matchLength] = pEndPointList[cIndex];
      pMessageGoingOut->matchLength++;
    }
  }

  /* We got a match send it back. */
  if (pMessageGoingOut->matchLength)
  {
    return (MbrOfs(zbMatchDescriptorResponse_t, matchList[0])+ pMessageGoingOut->matchLength);
  }
  
  /* No match found send the default answer with any  list. */
  pMessageGoingOut->status = gZdoNoMatch_c;
  return MbrOfs(zbMatchDescriptorResponse_t, matchList[0]);
}
#endif

#if gSystem_Server_Discovery_rsp_d
/*-------------------- System_Server_Discovery_rsp --------------------
	2.4.4.1.10 System_Server_Discovery_rsp. (ClusterID=0x8015)

	The System_Server_Discovery_rsp is generated from Remote Devices on receipt
	of a System_Server_Discovery_req primitive if the parameter matches the Server
	Mask field in its node descriptor. (If there is no match, the
	System_Server_Discovery_req shall be ignored and no response given). Matching
	is performed by masking the ServerMask parameter of the
	System_Server_Discovery_req with the Server Mask field in the node descriptor.
	This command shall be unicast to the device which sent
	System_Server_Discovery_req with Acknowledge request set in TxOptions. The
	parameter ServerMask contains the bits in the parameter of the request which
	match the server mask in the node descriptor.

	IN/OUT: The buffer where the response data will be filled in.
	IN The service mask to match.

	OUT: The size in bytes of the request payload.
*/
zbSize_t System_Server_Discovery_rsp
(
	zbSystemServerDiscoveryResponse_t  *pMessageGoingOut,  /* IN/OUT: The buffer where the response data will be filled in. */
	zbServerMask_t  aServerMask                            /* IN The service mask to match. */
)
{
	/* Here we fillup the response */
	Set2Bytes(pMessageGoingOut->aServerMask, ZDP_GetServerMaskServices(aServerMask));
	if (pMessageGoingOut->aServerMask)
	{
		/* The outgoing buffer is filled all with Zeros, so pMessageGoingOut->status=gZbSuccess_c; is not longer needed */
		return(sizeof(zbSystemServerDiscoveryResponse_t));
	}
	return (0); /* with the pyload in 0 we drop the message (the messages is ignored) -mn */
}
#endif

#if gUser_Desc_conf_d
/*-------------------- User_Desc_conf --------------------
	2.4.4.1.11 User_Desc_conf. (ClusterID=0x8014)

	The User_Desc_conf is generated by a remote device in response to a
	User_Desc_set directed to the remote device. This command shall be unicast to
	the originator of the User_Desc_set command.

	IN: The package with the request information.
	IN/OUT: The buffer where the response data will be filled in.

	OUT: The size in bytes of the request payload.
*/
zbSize_t User_Desc_conf
(
	zbUserDescriptorSet_t  *pMessageComingIn,     /* IN: The package with the request information. */
	zbUserDescriptorConfirm_t  *pMessageGoingOut  /* IN/OUT: The buffer where the response data will be filled in. */
)
{
	/* The outgoing buffer is filled with zeros so pMessageGoingOut->status = gZbSuccess_c; is no loger needed */
	/* reseting to white spaces */
	FLib_MemSet(&gUserDesc,' ', sizeof(gUserDesc));
	/* including the length of the user descriptor is copied here */
	FLib_MemCpy(&gUserDesc,&pMessageComingIn->descriptor, sizeof(pMessageComingIn->descriptor) );
	Copy2Bytes(pMessageGoingOut->aNwkAddrOfInterest, NlmeGetRequest(gNwkShortAddress_c));
	return (sizeof( zbUserDescriptorConfirm_t ));
}
#endif

#if gDiscovery_Cache_rsp_d
/*-------------------- Discovery_Cache_rsp --------------------
	2.4.4.1.12 Discovery_Cache_rsp. (ClusterID=0x8012)

	The Discovery_Cache_rsp is generated by Primary Discovery Cache devices
	receiving the Discovery_Cache_req. Remote Devices which are not Primary
	Discovery Cache devices (as designated in its Node Descriptor) should not
	respond to the Discovery_Cache_req command.

	IN/OUT: NONE.

	OUT: The size in bytes of the request payload.
*/
zbSize_t Discovery_Cache_rsp
(
	void  /* IN/OUT: NONE. */
)
{
	/* The outgoing buffer is filled with Zeros, so the pMessageGoingOut->status = gZbSuccess_c; is not longer needed */
	if (ZdoGetPrimaryDiscoveryCacheBitMask(gaServerMask))
		return(sizeof(zbDiscoveryCacheResponse_t));

	return 0; /* Zero mean Not supported in the ZDP default case */
}
#endif

#if gDiscovery_store_rsp_d
/*-------------------- Discovery_store_rsp --------------------
	2.4.4.1.13 Discovery_store_rsp. (ClusterID=0x8016)

	The Discovery_store_rsp is provided to notify a Local Device of the request status
	from a Primary Discovery Cache device. Included in the response is a status code
	to notify the Local Device whether the request is successful (the Primary Cache
	Device has space to store the discovery cache data for the Local Device), whether
	the request is unsupported (meaning the Remote Device is not a Primary
	Discovery Cache device) or whether insufficient space exists.

	IN/OUT: The buffer where the response data will be filled in.
	IN: The short address of the requesting device.
	IN: The MAC address of the requesting device.

	OUT: The size in bytes of the request payload.
*/
zbSize_t Discovery_store_rsp
(
	zbDiscoveryStoreResponse_t  *pMessageGoingOut,  /* IN/OUT: The buffer where the response data will be filled in. */
	zbNwkAddr_t  aNwkAddress,                       /* IN: The short address of the requesting device. */
	zbIeeeAddr_t  aIeeeAddress                      /* IN: The MAC address of the requesting device. */
)
{
	if (!ZdoGetPrimaryDiscoveryCacheBitMask(gaServerMask))
	{
		pMessageGoingOut->status = gZdoNotSupported_c;
		return (sizeof(zbDiscoveryStoreResponse_t));
	}

	pMessageGoingOut->status = UpdateDiscoveryStoreTable(aNwkAddress, aIeeeAddress);
	return (sizeof(zbDiscoveryStoreResponse_t));
}
#endif

#if gRemove_node_cache_rsp_d
/*-------------------- Remove_node_cache_rsp --------------------
	2.4.4.1.18 Remove_node_cache_rsp. (ClusterID=0x801b)

	The Remove_node_cache_rsp is provided to notify a Local Device of the request
	status from a Primary Discovery Cache device. Included in the response is a status
	code to notify the Local Device whether the request is successful (the Primary
	Cache Device has removed the discovery cache data for the indicated device of
	interest) or whether the request is not supported (meaning the Remote Device is
	not a Primary Discovery Cache device).

	IN/OUT: The buffer where the response will be generated.
	IN: The package with the request coming in.

	OUT: The size in bytes of the response payload.
*/
zbSize_t Remove_node_cache_rsp
(
	zbRemoveNodeCacheRequest_t  *pMessageComingIn,  /* IN/OUT: The buffer where the response will be generated. */
	zbRemoveNodeCacheResponse_t  *pMessageGoingOut  /* IN: The package with the request coming in. */
)
{
	index_t  cIndex;

	/*
		If it is not a Primary Discovery Cache device, the
		Remote Device shall return a status of NOT_SUPPORTED.
	*/
	if (!ZdoGetPrimaryDiscoveryCacheBitMask(gaServerMask))
	{
		pMessageGoingOut->status = gZdoNotSupported_c;
		return (MbrSizeof(zbRemoveNodeCacheResponse_t, status));
	}

	cIndex = ZDP_SearchNodeInCache(pMessageComingIn->aNwkAddress, pMessageComingIn->aIeeeAddress,zbNwkAndIeeeMode);
	if (cIndex == gZdoNoEntry_c)
	{
		pMessageGoingOut->status = gZdoNotPermited_c;
		return (MbrSizeof(zbRemoveNodeCacheResponse_t, status));
	}

	BeeUtilZeroMemory(&gaDiscoveryStoreTable[cIndex], sizeof(discoveryStoreTable_t));
	mDiscoveryStoreTableEntrys--;

	return sizeof(zbRemoveNodeCacheResponse_t);
}
#endif

#if gFind_node_cache_rsp_d
/*-------------------- Find_node_cache_rsp --------------------
	2.4.4.1.19 Find_node_cache_rsp. (ClusterID=0x801c)

	The Find_node_cache_rsp is provided to notify a Local Device of the successful
	discovery of the Primary Discovery Cache device for the given NWKAddr and
	IEEEAddr fields supplied in the request, or to signify that the device of interest is
	capable of responding to discovery requests. The Find_node_cache_ rsp shall be
	generated only by Primary Discovery Cache devices holding discovery
	information for the NWKAddr and IEEEAddr in the request or the device of
	interest itself and all other Remote Devices shall not supply a response.

	IN/OUT: The buffer where the response data will be filled in.
	IN: The NWK Address for the device of interest.
	IN: The IEEE address for the device of interest.

	OUT: The size in bytes of the response payload.
*/
zbSize_t Find_node_cache_rsp
(
	zbFindNodeCacheResponse_t  *pMessageGoingOut,  /* IN/OUT: The buffer where the response data will be filled in. */
	zbNwkAddr_t  aNwkAddress,                      /* IN: The NWK Address for the device of interest. */
	zbIeeeAddr_t  aIeeeAddress                     /* IN: The IEEE address for the device of interest. */
)
{
	index_t  cIndex;
	discoveryStoreTable_t  *pPtr;

	if (!ZdoGetPrimaryDiscoveryCacheBitMask(gaServerMask))
		return 0; /* Returning Zero ZDP will use the default case which is Not Supported */

	if ( IsSelfNwkAddress(aNwkAddress) && IsSelfIeeeAddress(aIeeeAddress))
	{
		/* Here we fillup the response */ /*Packing up the response -mn*/
		Copy2Bytes(pMessageGoingOut->aCacheNwkAddr, aNwkAddress);/* Give the networ address (Short address) of the Device Cache -mn */
		Copy8Bytes(pMessageGoingOut->aIeeeAddrOfInterest, aIeeeAddress); /* this data must be geted from the Device cache table */
		Copy2Bytes(pMessageGoingOut->aNwkAddrOfInterest, aNwkAddress); /* this data must be goted from the Device cache table */
		return(sizeof(zbFindNodeCacheResponse_t));/* Set the porper payload Lenght for the response message */
	}

	cIndex = ZDP_SearchNodeInCache(aNwkAddress, aIeeeAddress,zbNwkAndIeeeMode);
	if (cIndex == gZdoNoEntry_c)
		return 0; /* Returning Zero ZDP will use the default case which is Not Supported */

	pPtr = &gaDiscoveryStoreTable[cIndex];
	Copy2Bytes(pMessageGoingOut->aCacheNwkAddr, NlmeGetRequest(gNwkShortAddress_c));/* Give the networ address (Short address) of the Device Cache -mn */
	Copy8Bytes(pMessageGoingOut->aIeeeAddrOfInterest, pPtr->aIeeeAddress); /* this data must be geted from the Device cache table */
	Copy2Bytes(pMessageGoingOut->aNwkAddrOfInterest, pPtr->aNwkAddress); /* this data must be goted from the Device cache table */
	return(sizeof(zbFindNodeCacheResponse_t));/* Set the porper payload Lenght for the response message */
}
#endif

#if gBind_Register_rsp_d
/*-------------------- Bind_Register_rsp --------------------
  2.4.4.2.4 Bind_Register_rsp. (ClusterID=0x8023)

  The Bind_Register_rsp is generated from a primary binding table cache device in
  response to a Bind_Register_req and contains the status of the request. This
  command shall be unicast to the requesting device.

  If the device receiving the Bind_Register_req is not a primary binding table cache
  a Status of NOT_SUPPORTED is returned. If its list of devices which choose to
  store their own binding table entries is full a status of TABLE_FULL is returned.
  In these error cases, BindingTableEntries and BindingTableListCount shall be
  zero and BindingTableList shall be empty. A Status of SUCCESS indicates that
  the requesting device has been successfully registered.

  IN/OUT: The buffer where the response data will be filled in.
  IN: The IEEE address of the node to be searched in the binding cache table.
  IN: The short address of the node soliciting the binding info.

  OUT: The size in bytes of the response payload.
*/
zbSize_t Bind_Register_rsp
(
  zbBindRegisterResponse_t  *pMessageGoingOut,  /* IN/OUT: The buffer where the response data will be filled in. */
  zbIeeeAddr_t  aIeeeAddr,                      /* IN: The IEEE address of the node to be searched in the binding cache table. */
  zbNwkAddr_t  aNwkAddress                      /* IN: The short address of the node soliciting the binding info. */
)
{
  zbSize_t  listSize;

  pMessageGoingOut->status = gZbSuccess_c;

  if(!ZdoGetPrimaryBindingTableCacheBitMask(gaServerMask))
  {
    pMessageGoingOut->status = gZdoNotSupported_c;
    return(MbrOfs(zbBindRegisterResponse_t, aBindingTableList[0]));
  }

  /*Try to Add the Source Address to the aDevicesHoldingItsOwnBindingInfo table */
  pMessageGoingOut->status = ZDP_RecordSourceBindAddress(aIeeeAddr);
  if(pMessageGoingOut->status == gZdoTableFull_c)
    return(MbrOfs(zbBindRegisterResponse_t, aBindingTableList[0]));

  Set2Bytes(pMessageGoingOut->aBindingTableEntrys, 
            ZDP_GetBindingCacheEntryListByAddress(aIeeeAddr,(uint8_t *)pMessageGoingOut->aBindingTableList,&listSize));
  if (!TwoBytesToUint16(pMessageGoingOut->aBindingTableEntrys))
  {
    pMessageGoingOut->status = gZdoDeviceNotFound_c;
    return(MbrOfs(zbBindRegisterResponse_t, aBindingTableList[0]));
  }
  else if (TwoBytesToUint16(pMessageGoingOut->aBindingTableEntrys) >= 
           ZdpMaximumEntriesInPayLoad(MbrOfs(zbBindRegisterResponse_t, aBindingTableList[0]),sizeof(zbApsmeBindEntry_t)))
  {
    Set2Bytes(pMessageGoingOut->aBindingTableListCount,
              ZdpMaximumEntriesInPayLoad(MbrOfs(zbBindRegisterResponse_t, aBindingTableList[0]),sizeof(zbApsmeBindEntry_t)));
  }
  else
    Copy2Bytes(pMessageGoingOut->aBindingTableListCount,pMessageGoingOut->aBindingTableEntrys);

#if (gBigEndian_c)  
  Swap2BytesArray(pMessageGoingOut->aBindingTableEntrys);
  Swap2BytesArray(pMessageGoingOut->aBindingTableListCount);
#endif  

  if (Zdp_UpdateBlockAddressMap(aNwkAddress, aIeeeAddr) == gZbSuccess_c)
    return (MbrOfs(zbBindRegisterResponse_t, aBindingTableList[0]) + listSize);
  else
  {
    pMessageGoingOut->status = gZdoTableFull_c;
    return(MbrOfs(zbBindRegisterResponse_t, aBindingTableList[0]));
  }
}
#endif

#if gReplace_Device_rsp_d
/*-------------------- Replace_Device_rsp --------------------
	This function is ZDP internal. (ClusterID=0x8024)

	The Replace_Device_rsp is generated from a primary binding table cache device
	in response to a Replace_Device_req and contains the status of the request. This
	command shall be unicast to the requesting device.
	ZigBee 1.1 section 2.4.4.2.5

	IN/OUT: The buffer where the response will be generated.
	IN: The package with the reuest coming in.
	IN: The address of the device sending the request.

	OUT: The size in bytes of the response.
*/
zbSize_t Replace_Device_rsp
(
	zbReplaceDeviceResponse_t  *pMessageGoingOut,  /* IN/OUT: The buffer where the response will be generated. */
	zbReplaceDeviceRequest_t  *pMessageComingIn,   /* IN: The package with the reuest coming in. */
	zbNwkAddr_t  aSrcAddr                          /* IN: The address of the device sending the request. */
)
{
	index_t cIndex;
	index_t auxIndex;
	zbAddrMode_t  addressMode;
	zbNwkAddr_t  aNwkAddress;
	zbApsmeBindEntry_t  *pPtr;

	pMessageGoingOut->status = gZdoNotSupported_c;

	if (!ZdoGetPrimaryBindingTableCacheBitMask(gaServerMask) || Cmp8BytesToZero(pMessageComingIn->aOldAddress))
		return (MbrSizeof(zbReplaceDeviceResponse_t, status));

	for (auxIndex = 0; auxIndex < gMaximumBindingCacheTableList_c; auxIndex++)
	{
		cIndex = ZDP_SearchInBindCache(pMessageComingIn->aOldAddress, &addressMode, 0x00);
		pPtr = &gaBindingCacheTable[cIndex];
		if (cIndex != gZdoNoEntry_c)
		{
			if (ZDP_SearchDeviceHoldingItsOwnBindingInfo(pPtr->aSrcAddr) != gZdoNoEntry_c)
			{
                                pMessageGoingOut = ( zbReplaceDeviceResponse_t* )( ( (uint8_t *)pMessageGoingOut ) - SapHandlerDataStructureOffSet );
                                MSG_Free( pMessageGoingOut);
				mbIsNotFree = FALSE;
				pMessageGoingOut->status = ZDP_SetRegisterMachineData(gReplace_Device_req_c,aSrcAddr, pMessageComingIn);
				return gZero_c;
			}
			else
			{
				GetAddressFromService(ZdoBackupBindingTableCache_c, aNwkAddress);
				if (!IsEqual2BytesInt(aNwkAddress, 0xffff))
				{
                                        pMessageGoingOut = ( zbReplaceDeviceResponse_t* )( ( (uint8_t *)pMessageGoingOut ) - SapHandlerDataStructureOffSet );
                                        MSG_Free( pMessageGoingOut);
					mbIsNotFree = FALSE;
					pMessageGoingOut->status = ZDP_SetRegisterMachineData(gRemove_Bkup_Bind_Entry_req_c,aSrcAddr, pMessageComingIn);
					return gZero_c;
				}
				else
				{
					pPtr= &gaBindingCacheTable[auxIndex];
					if (addressMode == zbSrcAddressMode)
					{
						Copy8Bytes(pPtr->aSrcAddr, pMessageComingIn->aNewAddress);
						if (pMessageComingIn->oldEndPoint)
							pPtr->srcEndPoint = pMessageComingIn->newEndPoint;
						pMessageGoingOut->status = gZbSuccess_c;
					}
					else
					{
						Copy8Bytes(pPtr->aDstAddr, pMessageComingIn->aOldAddress);
						if (pMessageComingIn->oldEndPoint)
							pPtr->dstEndPoint= pMessageComingIn->newEndPoint;
						pMessageGoingOut->status = gZbSuccess_c;
					}
				}
			}
		}
		else
			pMessageGoingOut->status = gZdoNoEntry_c;
	}
	return (MbrSizeof(zbReplaceDeviceResponse_t, status));
}
#endif

#if gStore_Bkup_Bind_Entry_rsp_d
/*-------------------- Store_Bkup_Bind_Entry_rsp --------------------
	2.4.4.2.6 Store_Bkup_Bind_Entry_rsp. (ClusterID=0x8025)

	The Store_Bkup_Bind_Entry_rsp is generated from a backup binding table cache
	device in response to a Store_Bkup_Bind_Entry_req from a primary binding table
	cache and contains the status of the request. This command shall be unicast to the
	requesting device.

	IN/OUT: The buffer where the response will be generated.
	IN: The package with the request information.
	IN: The address of the device sending the request.

	OUT: The size in bytes of the response payload.
*/
zbSize_t Store_Bkup_Bind_Entry_rsp
(
	zbStoreBkupBindEntryResponse_t  *pMessageGoingOut,  /* IN/OUT: The buffer where the response will be generated. */
	zbStoreBkupBindEntryRequest_t  *pMessageComingIn,   /* IN: The package with the request information. */
	zbNwkAddr_t  aNwkAddress                            /* IN: The address of the device sending the request. */
)
{
	if (!ZdoGetBackupBindingTableCacheBitMask(gaServerMask))
	{
		pMessageGoingOut->status = gZdoNotSupported_c;
		return (MbrSizeof(zbStoreBkupBindEntryResponse_t, status));
	}

	if (!ZDP_MatchAddressToService(aNwkAddress,ZdoPrimaryBindingTableCache_c))
	{
		pMessageGoingOut->status = gZdoInvalidRequestType_c;
		return (MbrSizeof(zbStoreBkupBindEntryResponse_t, status));
	}

	if (ZDP_MatchBindCacheEntry((zbApsmeBindEntry_t *)pMessageComingIn, pMessageComingIn->addressMode) != gZdoNoEntry_c)
	{
		pMessageGoingOut->status = gZbSuccess_c;
		return (MbrSizeof(zbStoreBkupBindEntryResponse_t, status));
	}

	if (!ZDP_AddToBindingCacheTable((zbBindUnbindRequest_t *)pMessageComingIn))
	{
		pMessageGoingOut->status = gZdoTableFull_c;
		return (MbrSizeof(zbStoreBkupBindEntryResponse_t, status));
	}

	pMessageGoingOut->status = gZbSuccess_c;
	return (MbrSizeof(zbStoreBkupBindEntryResponse_t, status));
}
#endif

#if gRemove_Bkup_Bind_Entry_rsp_d
/*-------------------- Remove_Bkup_Bind_Entry_rsp --------------------
	2.4.4.2.7 Remove_Bkup_Bind_Entry_rsp. (ClusterID=0x8026)

	The Remove_Bkup_Bind_Entry_rsp is generated from a backup binding table
	cache device in response to a Remove_Bkup_Bind_Entry_req from the primary
	binding table cache and contains the status of the request. This command shall be
	unicast to the requesting device.

	IN/OUT: The buffer where the response will be generated.
	IN: The package with the request information.
	IN: The address of the device sending the request.

	OUT: The size in bytes of the response payload.
*/
zbSize_t Remove_Bkup_Bind_Entry_rsp
(
	zbRemoveBackupBindEntryResponse_t  *pMessageGoingOut,  /* IN/OUT: The buffer where the response will be generated. */
	zbRemoveBackupBindEntryRequest_t	*pMessageComingIn,	 /* IN: The package with the request information. */
	zbNwkAddr_t  aNwkAddress															 /* IN: The address of the device sending the request. */
)
{
	index_t  cIndex;

	if (!ZdoGetBackupBindingTableCacheBitMask(gaServerMask))
	{
		pMessageGoingOut->status = gZdoNotSupported_c;
		return (MbrSizeof(zbRemoveBackupBindEntryResponse_t, status));
	}

	if (!ZDP_MatchAddressToService(aNwkAddress,ZdoPrimaryBindingTableCache_c))
	{
		pMessageGoingOut->status = gZdoInvalidRequestType_c;
		return (MbrSizeof(zbRemoveBackupBindEntryResponse_t, status));
	}

	cIndex = ZDP_MatchBindCacheEntry((zbApsmeBindEntry_t *)pMessageComingIn, pMessageComingIn->addressMode);
	if (cIndex != gZdoNoEntry_c)
	{
		ZdpBindSetFree(gaBindingCacheTable[cIndex]);
		pMessageGoingOut->status = gZbSuccess_c;
		return (MbrSizeof(zbRemoveBackupBindEntryResponse_t, status));
	}
	else
	{
		pMessageGoingOut->status = gZdoNoEntry_c;
		return (MbrSizeof(zbRemoveBackupBindEntryResponse_t, status));
	}
}
#endif

#if gBackup_Bind_Table_rsp_d
/*-------------------- Backup_Bind_Table_rsp --------------------
	2.4.4.2.8 Backup_Bind_Table_rsp. (ClusterID=0x8027)

	The Backup_Bind_Table_rsp is generated from a backup binding table cache
	device in response to a Backup_Bind_Table_req from a primary binding table
	cache and contains the status of the request. This command shall be unicast to the
	requesting device.

	IN: The package with the request information.
	IN/OUT: The buffer where the response will be generated.
	IN: The address of the device sending the request.

	OUT: The size in bytes of the response payload.
*/
zbSize_t Backup_Bind_Table_rsp
(
	zbBackupBindTableRequest_t	*pMessageComingIn,	 /* IN: The package with the request information. */
	zbBackupBindTableResponse_t  *pMessageGoingOut,  /* IN/OUT: The buffer where the response will be generated. */
	zbNwkAddr_t  aNwkAddress												 /* IN: The address of the device sending the request. */
)
{
	uint16_t  cIndex;  /* The index in the binding table where the replace procedure will start. */
	uint16_t  limit;   /* The upper limit, this is the value until where will replace in the backup-binding-cache Table. */
	uint8_t  *pList;   /* The pointer to the list where the information use to replace the existing is. */
	zbApsmeBindEntry_t  *pPtr;

	/*
		If the device is not a backup binding table cache it shall
		return a status of NOT_SUPPORTED.
	*/
	if (!ZdoGetBackupBindingTableCacheBitMask(gaServerMask))
	{
		pMessageGoingOut->status = gZdoNotSupported_c;
		return sizeof(zbBackupBindTableResponse_t);
	}

	/*
		If the originator of the request is not recognized as a primary 
		binding table cache it shall return a status of INV_REQUESTTYPE.
	*/
	if (!ZDP_MatchAddressToService(aNwkAddress, ZdoPrimaryBindingTableCache_c))
	{
		pMessageGoingOut->status = gZdoInvalidRequestType_c;
		return sizeof(zbBackupBindTableResponse_t);
	}
#if (gBigEndian_c)  
	Swap2BytesArray(pMessageComingIn->StartIndex);
	Swap2BytesArray(pMessageComingIn->BindingTableListCount);
#endif        
        cIndex = TwoBytesToUint16(pMessageComingIn->StartIndex);

	if ((cIndex + TwoBytesToUint16(pMessageComingIn->BindingTableListCount)) > gMaximumBindingCacheTableList_c)
	{
		/*
			If this exceeds its table size it shall fill in as many entries
			as possible and return a status of TABLE_FULL and the EntryCount
			parameter will be the number of entries in the table.
		*/
		pMessageGoingOut->status = gZdoTableFull_c;
		Set2Bytes(pMessageGoingOut->entryCount,gMaximumBindingCacheTableList_c);
		/* Changing the value to OTA */
#if (gBigEndian_c)                
		Swap2BytesArray(pMessageGoingOut->entryCount);
#endif                
		limit = gMaximumBindingCacheTableList_c;
	}
	else
	{
		/*
			Otherwise it shall return a status of SUCCESS and EntryCount will be equal
			to StartIndex + BindingTableListCount from Backup_Bind_Table_req.
		*/
		pMessageGoingOut->status = gZbSuccess_c;
		Set2Bytes(pMessageGoingOut->entryCount, cIndex + TwoBytesToUint16(pMessageComingIn->BindingTableListCount));
		/* Changing the value to OTA */
#if (gBigEndian_c)                
		Swap2BytesArray(pMessageGoingOut->entryCount);
#endif                
		limit = cIndex + TwoBytesToUint16(pMessageComingIn->BindingTableListCount);
	}
	
	/*
		The backup binding table cache shall overwrite the binding entries
		in its binding table starting with StartIndex and continuing for
		BindingTableListCount entries, The procedure relays in the fact
		tht the start index is not a C index which alwasy start 0, that
		is the way that needs to be done.
	*/
	pList = (uint8_t *)pMessageComingIn->BindingTableList;
	for (;cIndex < limit;cIndex++)
	{
		pPtr = &gaBindingCacheTable[cIndex];
		if (((zbApsmeBindEntry_t *)pList)->dstAddrMode == zbGroupMode)
		{
			Fill8BytesToZero(pPtr->aDstAddr);
			FLib_MemCpy(pPtr, pList, MbrOfs(zbApsmeBindEntry_t, aDstAddr[0]) + sizeof(zbGroupId_t));
			pPtr->dstEndPoint = 0xff;
			pList += (MbrOfs(zbApsmeBindEntry_t, aDstAddr[0]) + sizeof(zbGroupId_t));
		}
		else if (((zbApsmeBindEntry_t *)pList)->dstAddrMode == zbIEEEMode)
		{
			FLib_MemCpy(pPtr, pList, sizeof(zbApsmeBindEntry_t));
			pList += sizeof(zbApsmeBindEntry_t);
		}
		gBindingCacheTableCounter++;
	}

	return (sizeof(zbBackupBindTableResponse_t));
}
#endif

#if gRecover_Bind_Table_rsp_d
/*-------------------- Recover_Bind_Table_rsp --------------------
	2.4.4.2.9 Recover_Bind_Table_rsp. (ClusterID=0x8028)

	The Recover_Bind_Table_rsp is generated from a backup binding table cache
	device in response to a Recover_Bind_Table_req from a primary binding table
	cache and contains the status of the request. This command shall be unicast to the
	requesting device.

	IN: The package with the request information.
	IN/OUT: The buffer where the response will be generated.
	IN: The address of the device sending the request.

	OUT: The size in bytes of the response payload.
*/
zbSize_t Recover_Bind_Table_rsp
(
	zbRecoverBindTableRequest_t *pMessageComingIn,   /* IN: The package with the request information. */
	zbRecoverBindTableResponse_t *pMessageGoingOut,  /* IN/OUT: The buffer where the response will be build. */
	zbNwkAddr_t  aNwkAddress                         /* IN: The short address of the node soliciting the binding info. */
)
{
	zbSize_t  listSize;
	zbIeeeAddr_t  aNullAddress = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};

	/*
		If the responding device is not a backup binding table cache it
		shall return a status of NOT_SUPPORTED.
	*/
	if (!ZdoGetBackupBindingTableCacheBitMask(gaServerMask))
	{
		pMessageGoingOut->status = gZdoNotSupported_c;
		return (MbrOfs(zbRecoverBindTableResponse_t, aBindingTableList[0]));
	}

	/*
		If the originator of the request is not recognized as a primary
		binding table cache it shall return a status of INV_REQUESTTYPE.
	*/
	if (!ZDP_MatchAddressToService(aNwkAddress, ZdoPrimaryBindingTableCache_c))
	{
		pMessageGoingOut->status = gZdoInvalidRequestType_c;
		return (MbrOfs(zbRecoverBindTableResponse_t, aBindingTableList[0]));
	}

	/*
		If StartIndex is more than the number of entries in the
		Binding table, a status of NO_ENTRY is returned.
	*/
#if (gBigEndian_c)          
	Swap2BytesArray(pMessageComingIn->startIndex);
#endif	
	if (TwoBytesToUint16(pMessageComingIn->startIndex) > gMaximumBindingCacheTableList_c)
	{
		pMessageGoingOut->status = gZdoNoEntry_c;
		return (MbrOfs(zbRecoverBindTableResponse_t, aBindingTableList[0]));
	}

	/* Passing a null address to this function means all entries in the table */
	Set2Bytes(pMessageGoingOut->aBindingTableEntries,
						ZDP_GetBindingCacheEntryListByAddress(aNullAddress,(uint8_t *)pMessageGoingOut->aBindingTableList,&listSize));
  if (!TwoBytesToUint16(pMessageGoingOut->aBindingTableEntries))
  {
		/* If there is no entries in the backup binding table respond NO_ENTRY. */
    pMessageGoingOut->status = gZdoNoEntry_c;
    return (MbrOfs(zbRecoverBindTableResponse_t, aBindingTableList[0]));
  }

  if (TwoBytesToUint16(pMessageGoingOut->aBindingTableEntries) >=
      ZdpMaximumEntriesInPayLoad((MbrOfs(zbRecoverBindTableResponse_t, aBindingTableList[0])),sizeof(zbApsmeBindEntry_t)))
    Set2Bytes(pMessageGoingOut->aBindingTableListCount,
              ZdpMaximumEntriesInPayLoad((MbrOfs(zbRecoverBindTableResponse_t, aBindingTableList[0])),sizeof(zbApsmeBindEntry_t)));
  else
    Copy2Bytes(pMessageGoingOut->aBindingTableListCount,pMessageGoingOut->aBindingTableEntries);
#if (gBigEndian_c)
  Swap2BytesArray(pMessageGoingOut->aBindingTableEntries);
  Swap2BytesArray(pMessageGoingOut->aBindingTableListCount);
#endif
  return (MbrOfs(zbRecoverBindTableResponse_t, aBindingTableList[0]) + listSize);
}
#endif

#if gBackup_Source_Bind_rsp_d
/*-------------------- Backup_Source_Bind_rsp --------------------
	2.4.4.2.10 Backup_Source_Bind_rsp. (ClusterID=0x8029)

	The Backup_Source_Bind_rsp is generated from a backup binding table cache
	device in response to a Backup_Source_Bind_req from a primary binding table
	cache and contains the status of the request. This command shall be unicast to the
	requesting device.

	IN: The package with the request information.
	IN/OUT: The buffer where the response will be generated.
	IN: The address of the device sending the request.

	OUT: The size in bytes of the response payload.
*/
zbSize_t Backup_Source_Bind_rsp
(
	zbBackupSourceBindRequest_t  *pMessageComingIn,   /* IN: The package with the request information. */
	zbBackupSourceBindResponse_t  *pMessageGoingOut,  /* IN/OUT: The buffer where the response will be build. */
	zbNwkAddr_t  aNwkAddress                          /* IN: The short address of the node soliciting the binding info. */
)
{
	zbuCounter16_t  cIndex;  /* The index in the source binding table where the replace procedure will start. */
	zbuCounter16_t  limit;   /* The upper limit, this is the value until where will replace in the backup-source-binding Table. */
	zbCounter_t  auxIndex;   /* The index inthe incoming list. */

	if (!ZdoGetBackupBindingTableCacheBitMask(gaServerMask))
	{
		pMessageGoingOut->status = gZdoNotSupported_c;
		return (sizeof(zbBackupSourceBindResponse_t));
	}

	if (!ZDP_MatchAddressToService(aNwkAddress,ZdoPrimaryBindingTableCache_c))
	{
		pMessageGoingOut->status = gZdoInvalidRequestType_c;
		return (sizeof(zbBackupSourceBindResponse_t));
	}
#if (gBigEndian_c)  
	Swap2BytesArray(pMessageComingIn->StartIndex);
	Swap2BytesArray(pMessageComingIn->SourceTableListCount);
#endif        
cIndex = TwoBytesToUint16(pMessageComingIn->StartIndex);        

	if ((cIndex + TwoBytesToUint16(pMessageComingIn->SourceTableListCount)) > gMaximumDevicesHoldingBindingInfo_c)
	{
		/*
			If this exceeds its table size it shall fill in as many entries
			as possible and return a status of TABLE_FULL.
		*/
		pMessageGoingOut->status = gZdoTableFull_c;
		limit = gMaximumBindingCacheTableList_c;
	}
	else
	{
		/*
			Otherwise it shall return a status of SUCCESS.
		*/
		pMessageGoingOut->status = gZbSuccess_c;
		limit = cIndex + TwoBytesToUint16(pMessageComingIn->SourceTableListCount);
	}
	gDeviceHoldingItsOwnBindingInfo_Counter = 0;
	for (auxIndex = 0;cIndex < limit;cIndex++,auxIndex++)
	{
		Copy8Bytes(aDevicesHoldingItsOwnBindingInfo[cIndex],pMessageComingIn->SourceTableList[auxIndex]);
		gDeviceHoldingItsOwnBindingInfo_Counter++;
	}

	return (sizeof(zbBackupSourceBindResponse_t));
}
#endif

#if gRecover_Source_Bind_rsp_d
/*-------------------- Recover_Source_Bind_rsp --------------------
	2.4.4.2.11 Recover_Source_Bind_rsp. (ClusterID=0x802a)

	The Recover_Source_Bind_rsp is generated from a backup binding table cache
	device in response to a Recover_Source_Bind_req from a primary binding table
	cache and contains the status of the request. This command shall be unicast to the
	requesting device.

	IN: The package with the request information.
	IN/OUT: The buffer where the response will be generated.
	IN: The address of the device sending the request.

	OUT: The size in bytes of the response payload.
*/
zbSize_t Recover_Source_Bind_rsp
(
	zbRecoverSourceBindRequest_t  *pMessageComingIn,   /* IN: The package with the request information. */
	zbRecoverSourceBindResponse_t  *pMessageGoingOut,  /* IN/OUT: The buffer where the response will be build. */
	zbNwkAddr_t  aNwkAddress                           /* IN: The address of the device sending the request. */
)
{
	zbuCounter16_t  cIndex;
	zbCounter_t  auxIndex;
	zbCounter_t  limit;

	if (!ZdoGetBackupBindingTableCacheBitMask(gaServerMask))
	{
		pMessageGoingOut->Status = gZdoNotSupported_c;
		return (sizeof(zbStatus_t));
	}

	if (!ZDP_MatchAddressToService(aNwkAddress,ZdoPrimaryBindingTableCache_c))
	{
		pMessageGoingOut->Status = gZdoInvalidRequestType_c;
		return (sizeof(zbStatus_t));
	}

	Copy2Bytes(pMessageGoingOut->StartIndex , pMessageComingIn->StartIndex);
#if (gBigEndian_c)	        
	Swap2BytesArray(pMessageComingIn->StartIndex);
#endif
	cIndex = TwoBytesToUint16(pMessageComingIn->StartIndex);
	if ((cIndex > gMaximumDevicesHoldingBindingInfo_c) ||
      (cIndex > gDeviceHoldingItsOwnBindingInfo_Counter))
	{
		pMessageGoingOut->Status = gZdoNoEntry_c;
		return (sizeof(zbStatus_t));
	}

	pMessageGoingOut->Status = gZbSuccess_c;
	Set2Bytes(pMessageGoingOut->SourceTableEntries, gDeviceHoldingItsOwnBindingInfo_Counter);
#if (gBigEndian_c)	
        Swap2BytesArray(pMessageGoingOut->SourceTableEntries);
#endif
	limit = (uint8_t)ZdpMaximumEntriesInPayLoad(MbrOfs(zbRecoverSourceBindResponse_t, SourceTableList[0]), sizeof(zbIeeeAddr_t));
	for (auxIndex = 0; cIndex < limit; cIndex++,auxIndex++)
	{
		if (auxIndex >= gDeviceHoldingItsOwnBindingInfo_Counter)
			break;

		if (cIndex >= gMaximumDevicesHoldingBindingInfo_c)
			break;

		Copy8Bytes(pMessageGoingOut->SourceTableList[auxIndex], aDevicesHoldingItsOwnBindingInfo[cIndex]);
	}
	Set2Bytes(pMessageGoingOut->SourceTableListCount, auxIndex);
#if (gBigEndian_c)        
	Swap2BytesArray(pMessageGoingOut->SourceTableListCount);
#endif        
	return (MbrOfs(zbRecoverSourceBindResponse_t, SourceTableList[0]) + (sizeof(zbIeeeAddr_t) * auxIndex));
}
#endif

#if gEnd_Device_Bind_rsp_d && (gCoordinatorCapability_d || gComboDeviceCapability_d)&& gBindCapability_d
/*-------------------- End_Device_Bind_rsp --------------------
  2.4.4.2.1 End_Device_Bind_rsp. (ClusterID=0x8020)

  The End_Device_Bind_rsp is generated by the ZigBee Coordinator in response to
  an End_Device_Bind_req and contains the status of the request. This command
  shall be unicast to each device involved in the bind attempt, using the
  acknowledged data service.

  IN: The package with the in-coming request.

  OUT: Zero to avoid sending the response immediately.
*/
zbSize_t End_Device_Bind_rsp
(
  zbEndDeviceBindRequest_t *pMessageComingIn,
  zbStatus_t *pStatus
)
{
  /* If the current device is not the designted coordinator it should respond not supported. */
  if (!ApsmeGetRequest(gApsDesignatedCoordinator_c))
  {
    *pStatus = gZdoNotSupported_c;
    return sizeof(zbStatus_t);
  }

  /* We are the designated coordinator we should process the request. */
  return ZdpEndDeviceBindGotRequest(pMessageComingIn);
}
#endif


/*==============================================================================================
	========================= 2.4.4.3 Network Management Server Services =========================
	==============================================================================================*/
#if gMgmt_NWK_Disc_rsp_d
/*-------------------- Mgmt_NWK_Disc_rsp --------------------
	2.4.4.3.1 Mgmt_NWK_Disc_rsp. (ClusterID=0x8030)

	The Mgmt_NWK_Disc_rsp is generated in response to an
	Mgmt_NWK_Disc_req. If this management command is not supported, a status
	of NOT_SUPPORTED shall be returned and all parameter fields after the Status
	field shall be omitted. Otherwise, the Remote Device shall implement the
	following processing.

	IN: The package with the request information.
	IN/OUT: The buffer where the response will be build.

	OUT: NONE
*/
zbSize_t Mgmt_NWK_Disc_rsp
(
  zbMgmtNwkDiscoveryRequest_t  *pMessageComingIn,  /* IN: The package with the request information. */
  zdoNlmeMessage_t  *pMessageGoingOut,             /* IN/OUT: The buffer where the response will be build. */
  zbNwkAddr_t  aNwkAddress                         /* IN: The address of the device sending the request. */
)
{
  if (Zdp_IsNotValidCahnnel((void *)pMessageComingIn->aScanChannel))
  {
    ((uint8_t *)pMessageGoingOut + SapHandlerDataStructureOffSet)[0] = gZbNotPermitted_c;
    return (MbrOfs(zbMgmtNwkDiscResponse_t, nwkList));
  }
  SetClusterId(gIndex, gMgmt_NWK_Disc_req_c);
  StoreAddress( gIndex, aNwkAddress );
  SetstartIndexforNwkDisc( pMessageComingIn->startIndex );
  SetMgmtNwkDiscReq();
  pMessageGoingOut->msgType = gNlmeNetworkDiscoveryRequest_c;
  FLib_MemCpy(pMessageGoingOut->msgData.networkDiscoveryReq.aScanChannels,
  pMessageComingIn->aScanChannel,sizeof(nlmeNetworkDiscoveryReq_t));

  ZDO_SetState(gZdoRemoteCommandsState_c);

  (void)ZDO_NLME_SapHandler(pMessageGoingOut);
  mbIsNotFree = FALSE;
  return gZero_c;
}
#endif

#if gMgmt_Lqi_rsp_d && ( gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d)
/*-------------------- Mgmt_Lqi_rsp --------------------
	2.4.4.3.2 Mgmt_Lqi_rsp. (ClusterID=0x8031)

	The Mgmt_Lqi_rsp is generated in response to an Mgmt_Lqi_req. If this
	management command is not supported, a status of NOT_SUPPORTED shall be
	returned and all parameter fields after the Status field shall be omitted.

	IN: The package with the request information.
	IN/OUT: The buffer where the response will be generated.

	OUT: The size in bytes of the response payload.
*/
zbSize_t Mgmt_Lqi_rsp
(
	zbMgmtLqiRequest_t *pMessageComingIn,  /* IN: The package with the request information. */
	zbMgmtLqiResponse_t *pMessageGoingOut  /* IN/OUT: The buffer where the response will be generated. */
)
{
	index_t  cIndex = pMessageComingIn->startIndex;
	zbSize_t  dataLength = MbrOfs(zbMgmtLqiResponse_t, neighbourTableList[0]);
	neighborTable_t  *pNeighborTable;

#if gComboDeviceCapability_d
  if( NlmeGetRequest( gDevType_c) == gEndDevice_c )
    {
      pMessageGoingOut->status = gZdoNotSupported_c;
      return sizeof(zbStatus_t);
    }
#endif

  pMessageGoingOut->neighbourTableListCount = 0x00;
  pMessageGoingOut->neighbourTableEntries = 0x00;
  pMessageGoingOut->startIndex = pMessageComingIn->startIndex;
  pMessageGoingOut->neighbourTableEntries = NWK_MNG_GetNumberOfNeighbors( gAllTypes_c, gAllTypes_c ); 

  for( ; cIndex < gMaxNeighborTableEntries ; cIndex++)
  {
    if( pMessageGoingOut->neighbourTableListCount >=
        ZdpMaximumEntriesInPayLoad(MbrOfs(zbMgmtLqiResponse_t, neighbourTableList[0]),sizeof(zbNeighborTableList_t)))
    {
      break;
    }

    pNeighborTable = NlmeGetRequestTableEntry(gNwkNeighborTable_c, cIndex);
    /* if entry has no a valid relationship type, then skip the entry */
    if ( pNeighborTable->deviceProperty.bits.relationship != gNeighborIsParent_c &&
         pNeighborTable->deviceProperty.bits.relationship != gNeighborIsChild_c &&
         pNeighborTable->deviceProperty.bits.relationship != gNeighborIsSibling_c )
    {
      continue;
    }

    /* Got a good one, get all the needed info. */
    Copy8Bytes(pMessageGoingOut->neighbourTableList[pMessageGoingOut->neighbourTableListCount].aExtendedPanId, NlmeGetRequest(gNwkExtendedPanId_c));
    Copy8Bytes(pMessageGoingOut->neighbourTableList[pMessageGoingOut->neighbourTableListCount].aExtendedAddr, pNeighborTable->aExtendedAddress);
    Copy2Bytes(pMessageGoingOut->neighbourTableList[pMessageGoingOut->neighbourTableListCount].aNetworkAddr, pNeighborTable->aNetworkAddress);

    /*Here we get the device property and is implemented as the Beestack way */ 
    pMessageGoingOut->neighbourTableList[pMessageGoingOut->neighbourTableListCount].deviceProperty = pNeighborTable->deviceProperty.bits.deviceType;
		pMessageGoingOut->neighbourTableList[pMessageGoingOut->neighbourTableListCount].deviceProperty |= pNeighborTable->deviceProperty.bits.rxOnWhenIdle << 2;
		pMessageGoingOut->neighbourTableList[pMessageGoingOut->neighbourTableListCount].deviceProperty |= pNeighborTable->deviceProperty.bits.relationship << 4;
		/*The permitJoining is setting to false by default , There is no way to know that value cause the implementation  */
		pMessageGoingOut->neighbourTableList[pMessageGoingOut->neighbourTableListCount].permitJoining = FALSE;
		if (pNeighborTable->deviceProperty.bits.relationship == gNeighborIsChild_c)
			pMessageGoingOut->neighbourTableList[pMessageGoingOut->neighbourTableListCount].depth = NlmeGetRequest( gNwkDeviceDepth_c ) + 1;
		else
			pMessageGoingOut->neighbourTableList[pMessageGoingOut->neighbourTableListCount].depth = NlmeGetRequest( gNwkDeviceDepth_c ) - 1;
		pMessageGoingOut->neighbourTableList[pMessageGoingOut->neighbourTableListCount].lqi = pNeighborTable->lqi;

    dataLength += MbrSizeof(zbMgmtLqiResponse_t, neighbourTableList[0]);
		pMessageGoingOut->neighbourTableListCount++;
	}
	pMessageGoingOut->status = gZbSuccess_c;
	return dataLength;
}
#endif

#if gRnplusCapability_d && (gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d)
#if gMgmt_Rtg_rsp_d
/*-------------------- Mgmt_Rtg_rsp --------------------
	2.4.4.3.3 Mgmt_Rtg_rsp. (ClusterID=0x8032)

	The Mgmt_Rtg_rsp is generated in response to an Mgmt_Rtg_req. If this
	management command is not supported, a status of NOT_SUPPORTED shall be
	returned and all parameter fields after the Status field shall be omitted.

	IN: The package with the request information.
	IN/OUT: The buffer where the response will be generated.

	OUT: The size in bytes of the response payload.
*/
zbSize_t Mgmt_Rtg_rsp
(
  zbMgmtRtgRequest_t  *pMessageComingIn,  /* IN: The package with the request information. */
  zbMgmtRtgResponse_t  *pMessageGoingOut  /* IN/OUT: The buffer where the response will be generated. */
)
{
  index_t cIndex;
  uint8_t dataLength= MbrOfs(zbMgmtRtgResponse_t, routingTableList);
  routeTable_t *proutingTableEntry;

#if gComboDeviceCapability_d
  if( NlmeGetRequest( gDevType_c) == gEndDevice_c )
  {
    pMessageGoingOut->status = gZdoNotSupported_c;
    return sizeof(zbStatus_t);
  }
#endif

  pMessageGoingOut->startIndex = pMessageComingIn->startIndex;
  for(cIndex = 0; cIndex < gMaxRouteTableEntries; cIndex++) 
  {
    /* Get the entry from the table */
    proutingTableEntry = &gaRouteTable[cIndex];

    /* If the entry is not active skip it */
    if (proutingTableEntry->properties.status != gActive_c)
      continue;

    if (IsEqual2Bytes(proutingTableEntry->aDestinationAddress, gaBroadcastAddress))
      continue;		

    /* We have a valid entry, lets increase the counter */
    pMessageGoingOut->routingTableEntries++;

    /* If the index in the table is not in the desired range then skip */
    if( cIndex < pMessageComingIn->startIndex )
      continue;

    /* if the response fits in the payload */
    if((dataLength + sizeof(routingTableList_t)) > ZdoMaximumPayLoadSize())
    {
      break;
    }

    {
      routingTableList_t *pListEntry = &(pMessageGoingOut->routingTableList[pMessageGoingOut->routingTableListCount]);

      /* The last field need to be excluded */
      dataLength += sizeof(routingTableList_t);

      /* Copy the data obtained from the RTG table */
      Copy2Bytes(pListEntry->aDestinationAddress, proutingTableEntry->aDestinationAddress);
      Copy2Bytes(pListEntry->aNextHopAddress,proutingTableEntry->aNextHopAddress);
      pListEntry->properties = proutingTableEntry->properties;

      /* we got another entry, this counter tell us about how many entries goes over the air */
      pMessageGoingOut->routingTableListCount++;
    }
  }
  /* return the amount of bytes to be send in the response */
  return dataLength;
}
#endif
#endif

#if gMgmt_Bind_rsp_d
/*-------------------- Mgmt_Bind_rsp --------------------
	2.4.4.3.4 Mgmt_Bind_rsp. (ClusterID=0x8033)

	The Mgmt_Bind_rsp is generated in response to a Mgmt_Bind_req. If this
	management command is not supported, a status of NOT_SUPPORTED shall be
	returned and all parameter fields after the Status field shall be omitted.

	IN: The package with the request information.
	IN/OUT: The buffer where the response will be generated.

	OUT: The size in bytes of the response payload.
*/
zbSize_t Mgmt_Bind_rsp
(
  zbMgmtBindRequest_t  *pMessageComingIn,  /* IN: The package with the request information. */
  zbMgmtBindResponse_t  *pMessageGoingOut  /* IN/OUT: The buffer where the response will be generated. */
)
{
  bool_t  tableType;
  zbSize_t  payLoad;
  zbStatus_t  entryStatus;

  entryStatus = gBindCapability_d;

  /*
    If the node is not the mgmt cache device or binding capabilities are off then we have
    nothing to process.
  */
  if (!ZdoGetPrimaryBindingTableCacheBitMask(gaServerMask) && !entryStatus)
  {
    pMessageGoingOut->status = gZdoInvalidRequestType_c;
    return sizeof(pMessageGoingOut->status);
  }

  /* Get the least amount of bye to send out OTA in the response, everything but the list. */
  payLoad = MbrOfs(zbMgmtBindResponse_t, aBindingTableList[0]);

  /* TRUE if the local device is a binding cache, false other wise. */
  tableType = (ZdoGetPrimaryBindingTableCacheBitMask(gaServerMask)? TRUE : FALSE);

  if (tableType)
  {
    /* If the device is the Mgmt Bind cache process the the cache table. */
    payLoad += ZDP_MgmtBindResponse_CacheTable(pMessageComingIn->startIndex, pMessageGoingOut);
  }
  else
  {
    /* If the device is not the mgmt chce device process the local bind table if available. */
    payLoad += ZDP_MgmtBindResponse_BindTable(pMessageComingIn->startIndex, pMessageGoingOut);
  }

  /* Return the amount of bytes to send out in the response. */
  return payLoad;
}
#endif

#if gMgmt_Leave_rsp_d
/*-------------------- Mgmt_Leave_rsp --------------------
	2.4.4.3.5 Mgmt_Leave_rsp. (ClusterID=0x8034)

	The Mgmt_Leave_rsp is generated in response to a Mgmt_Leave_req. If this
	management command is not supported, a status of NOT_SUPPORTED shall be
	returned.

	IN: The package with the request information.
	IN/OUT: The buffer where the response will be generated.
	IN: The address of the device sending the request.

	OUT: The size in bytes of the response payload.
*/
zbSize_t Mgmt_Leave_rsp
(
  zbMgmtLeaveRequest_t *pMessageComingIn,  /* IN: The package with the request information. */
  zbMgmtLeaveResponse_t *pMessageGoingOut, /* IN/OUT: The buffer where the response will be generated. */
  zbNwkAddr_t aNwkAddress                  /* IN: The address of the device sending the request. */
)
{
  zdoNlmeMessage_t *zdoNlmeMsg;

  mbIsNotFree = FALSE;

  /*
    If is not an in-node command, verify for authorization in the permissions table.
  */
#if ((gStandardSecurity_d || gHighSecurity_d) && gApsMaxEntriesForPermissionsTable_c)
  if (!IsSelfNwkAddress(aNwkAddress))
  {
    if (!CommandHasPermission(aNwkAddress, gNetworkSettingsPermission_c))
    {
      pMessageGoingOut->status = gZdpNotAuthorized_c;
      return (sizeof(zbStatus_t));
    }
  }
#endif /* ((gStandardSecurity_d || gHighSecurity_d) && gApsMaxEntriesForPermissionsTable_c) */

  Copy2Bytes(mgmtLeaveResponseInfo,aNwkAddress);

  if (IsSelfIeeeAddress(pMessageComingIn->aDeviceAddress))
  {
    /*
      Indicates that the mgmt leave request is to ourself, so when we receive the confirm
      that the mgmt leave response has been sent over the air we leave.
    */
    gMgmtSelfLeaveMask = zbMgmtOptionSelfLeave_c;
    pMessageGoingOut->status = gZbSuccess_c;

    /* Catch any of the options for later process. */
    gMgmtSelfLeaveMask |= pMessageComingIn->mgmtOptions;  /* possible rejoin or remove children */

    /*
      Is the node it self who will be leaving, so, by returning the size of the response
      the ZDP's mechanism will send the Mgmt response back to the originator and wait for
      the confirm (Success or whatever), and then, is on the confirm where the rest of the
      option gets process.
    */
    return sizeof(zbMgmtLeaveResponse_t);
  }

  zdoNlmeMsg = (zdoNlmeMessage_t *)( ( (uint8_t *)pMessageGoingOut ) - SapHandlerDataStructureOffSet );
  BeeUtilZeroMemory(zdoNlmeMsg, sizeof(zdoNlmeMessage_t));

  /* Indicate we got a Management Leave Reguest pending to be responded */
  ZDO_SetState(gZdoRemoteCommandsState_c);

  zdoNlmeMsg->msgType = gNlmeLeaveRequest_c;
  Copy8Bytes( zdoNlmeMsg->msgData.leaveReq.aDeviceAddress, pMessageComingIn->aDeviceAddress );
  zdoNlmeMsg->msgData.leaveReq.rejoin = ZdpMgmtLeaveRejoinFlag(pMessageComingIn->mgmtOptions);
  zdoNlmeMsg->msgData.leaveReq.removeChildren = ZdpMgmtLeaveRemoveChildrenFlag(pMessageComingIn->mgmtOptions);
  zdoNlmeMsg->msgData.leaveReq.reuseAddress = mDefaultReuseAddressPolicy_c;

  (void)ZDO_NLME_SapHandler(zdoNlmeMsg);

  return gZero_c;
}
#endif

#if gMgmt_Direct_Join_rsp_d && (gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d)
/*-------------------- Mgmt_Direct_Join_rsp --------------------
  2.4.4.3.6 Mgmt_Direct_Join_rsp. (ClusterID=0x8035)

  The Mgmt_Direct_Join_rsp is generated in response to a Mgmt_Direct_Join_req.
  If this management command is not supported, a status of NOT_SUPPORTED
  shall be returned.
*/
zbSize_t Mgmt_Direct_Join_rsp
(
  zbMgmtDirectJoinRequest_t *pMsgComingIn,
  zbNwkAddr_t  aSrcAddr
)
{
  zdoNlmeMessage_t  *pZdoNlmeMsg;

#if ((gStandardSecurity_d || gHighSecurity_d) && gApsMaxEntriesForPermissionsTable_c)
  nlmeZdoMessage_t zdoMsg;
#endif

  /* 
    In this place we need to store the network source addres because if that device does not has permissions the aSrcAddress varible 
    never will change for the incoming src address and the Direct Join confirm will throw it due that address is 0xFFFF.
  */
  Copy2Bytes(aSrcAddress,aSrcAddr);

#if ((gStandardSecurity_d || gHighSecurity_d) && gApsMaxEntriesForPermissionsTable_c)
  if (!CommandHasPermission(aSrcAddr, gNetworkSettingsPermission_c))
  {
    zdoMsg.msgData.directJoinConf.status = gZdpNotAuthorized_c;
    Zdp_Mgmt_Direct_Join_conf((void *)&zdoMsg);
    return 0;
  }
#endif

  pZdoNlmeMsg = MSG_Alloc(MbrSizeof(zdoNlmeMessage_t, msgType) + sizeof(nlmeDirectJoinReq_t));

  if (pZdoNlmeMsg)
  {
    pZdoNlmeMsg->msgType = gNlmeDirectJoinRequest_c;

    FLib_MemCpy(&pZdoNlmeMsg->msgData.directJoinReq, pMsgComingIn, sizeof(nlmeDirectJoinReq_t));

    /* Set the state machine in remote command mode */
    ZDO_SetState(gZdoRemoteCommandsState_c);


    (void)ZDO_NLME_SapHandler((void *)pZdoNlmeMsg);
  }

  return 0;
}
#endif

#if gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d
#if gMgmt_Permit_Joining_rsp_d
/*-------------------- Mgmt_Permit_Joining_rsp --------------------
	2.4.4.3.7 Mgmt_Permit_Joining_rsp. (ClusterID=0x8036)

	The Mgmt_Permit_Joining_rsp is generated in response to a unicast
	Mgmt_Permit_Joining_req. In the description which follows, note that no
	response shall be sent if the Mgmt_Permit_Joining_req was received as a
	broadcast to all routers.

	IN: The package with the request information.
	IN/OUT: The buffer where the response will be generated.

	OUT: The size in bytes of the response payload.
*/
zbSize_t Mgmt_Permit_Joining_rsp
(
  zbMgmtPermitJoiningRequest_t   *pMessageComingIn,  /* IN: The package with the request information. */
  zbMgmtPermitJoiningResponse_t  *pMessageGoingOut,  /* IN/OUT: The buffer where the response will be generated. */
  zbNwkAddr_t  aNwkAddress                           /* IN: The address of the device sending the request. */
)
{
  zdoNlmeMessage_t *pZdoNwkMsg;

#if gComboDeviceCapability_d
  if( NlmeGetRequest( gDevType_c) == gEndDevice_c )
    {
      pMessageGoingOut->status = gZdoNotSupported_c;
      return sizeof(zbStatus_t);
    }
#endif

  /*
    If is not an in-node command, verify for authorization in the permissions table.
  */
#if ((gStandardSecurity_d || gHighSecurity_d) && gApsMaxEntriesForPermissionsTable_c)
  if (!IsSelfNwkAddress(aNwkAddress))
  {
    if (!CommandHasPermission(aNwkAddress, gNetworkSettingsPermission_c))
    {
      pMessageGoingOut->status = gZdpNotAuthorized_c;
      return (sizeof(zbStatus_t));
    }
  }
#else
  (void)aNwkAddress;
#endif

  pZdoNwkMsg = MSG_AllocType( zdoNlmeMessage_t );
  if(!pZdoNwkMsg)
  {
    return gZero_c;
  }

  BeeUtilZeroMemory(pZdoNwkMsg,sizeof(zdoNlmeMessage_t ));
  pZdoNwkMsg->msgData.permitJoiningReq.permitDuration = pMessageComingIn->PermitDuration;
  pZdoNwkMsg->msgType = gNlmePermitJoiningRequest_c;
  (void)ZDO_NLME_SapHandler(pZdoNwkMsg);

  if (gIsABroadcast)
  {
    gIsABroadcast = FALSE;
    return gZero_c;
  }

  pMessageGoingOut->status = gZbSuccess_c;
  return sizeof(zbStatus_t);
}
#endif
#endif

#if gMgmt_Cache_rsp_d
/*-------------------- Mgmt_Cache_rsp --------------------
	2.4.4.3.8 Mgmt_Cache_rsp. (ClusterID=0x8037)

	The Mgmt_Cache_rsp is generated in response to an Mgmt_Cache_req. If this
	management command is not Supported or the Remote Device is not a Primary
	Cache Device, a status of NOT_SUPPORTED shall be returned and all parameter
	fields after the Status field shall be omitted.

	IN: The package with the request information.
	IN: The starting Index for the requested elements of the discovery cache list.

	OUT: The size in bytes of the response payload.
*/
zbSize_t Mgmt_Cache_rsp
(
	zbMgmtCacheResponse_t  *pMessageGoingOut,  /* IN: The package with the request information. */
	zbIndex_t  startIndex                      /* IN: The starting Index for the requested elements of the discovery cache list. */
)
{
	index_t  cIndex;
	discoveryStoreTable_t  *pPtr;

	if (!ZdoGetPrimaryDiscoveryCacheBitMask(gaServerMask))
	{
		pMessageGoingOut->status = gZdoNotSupported_c;
		return (sizeof(pMessageGoingOut->status));
	}

	/* The out going buffer is filled with Zeros, so setting the status = Succes (0) is not necesary */
	pMessageGoingOut->discoveryCacheEntries = mDiscoveryStoreTableEntrys;
	pMessageGoingOut->startIndex = startIndex;

	for (cIndex = startIndex; cIndex < gMaximumDiscoveryStoreTableSize_c; cIndex++)
	{
		pPtr = &gaDiscoveryStoreTable[cIndex];
		if (Cmp8BytesToZero(pPtr->aIeeeAddress))
			continue;

		Copy8Bytes(pMessageGoingOut->discoveryCacheList[pMessageGoingOut->discoveryCacheListCount].aIeeeAddress, pPtr->aIeeeAddress);
		Copy2Bytes(pMessageGoingOut->discoveryCacheList[pMessageGoingOut->discoveryCacheListCount].aNwkAddress, pPtr->aNwkAddress);
		pMessageGoingOut->discoveryCacheListCount ++;
	}

	return (MbrOfs(zbMgmtCacheResponse_t, discoveryCacheList[0]) +
					(pMessageGoingOut->discoveryCacheListCount * sizeof(zbMgmtDiscoveryCacheList_t)));
}
#endif

/*-------------------- Device_Annce --------------------
    2.4.3.1.11.1 Device_annce (ClusterID=0x0013)
    The Device_annce is provided to enable ZigBee devices on the network to notify
    other ZigBee devices that the device has joined or re-joined the network,
    identifying the device.s 64-bit IEEE address and new 16-bit NWK address, and
    informing the Remote Devices of the capability of the ZigBee device
*/
zbSize_t Device_Annce
(
  zbEndDeviceAnnounce_t *pMsg,
  zbNwkAddr_t  aSrcAddress
)
{
  /*
    If we are using stochastic addressing we will use the fucntion 
    NwkAddressConflictDetected instead of ASP_FindIeee... condition
  */
#if (gStochasticAddressingCapability_d)
  if (NwkAddressConflictDetected(pMsg->aNwkAddress, pMsg->aIeeeAddress,FALSE))
  {
    (void)AddressConflictResolution();
  }
#else
  (void)Zdp_UpdateBlockAddressMap(pMsg->aNwkAddress, pMsg->aIeeeAddress);
#endif

  /* Update the address of the announcing device on the local tables. */
  ZDP_UpdateNTAndRT(pMsg, aSrcAddress);

  /*
    We need to let know our new neighbors that we are here.
  */
#if gHighSecurity_d && ( gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d)
  if ((NlmeGetRequest(gDevType_c)!= gEndDevice_c) && !gActAsZed)
  {
    TS_SendEvent(gNwkTaskID_c, gNwkSendLinkStatus_c);
  }
#endif

  /*
    This command does not send a response OTA, returning 0 do not respond.
  */
  return 0;
}


