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
#include "EmbeddedTypes.h"
#include "ZigBee.h"
#include "BeestackConfiguration.h"
#include "BeeCommon.h"

#include "ZdpManager.h"
#include "FunctionLib.h"
#include "ZdpUtils.h"
#include "NwkCommon.h"

#include "ZdpResponses.h"

#include "ASL_ZdpInterface.h"

#if gDiscovery_Cache_rsp_d || gNode_Desc_store_req_d || gPower_Desc_store_req_d || gActive_EP_store_req_d || gSimple_Desc_store_req_d
extern bool_t  gDiscoveryCacheResponse;
#endif

#if gDiscovery_Cache_req_d || gNode_Desc_store_req_d || gPower_Desc_store_req_d || gActive_EP_store_req_d || gSimple_Desc_store_req_d
/* ZigBee 1.1 Discovery Chache table, Store the address of Discovery Cache Succcess server */
extern zdpRecordDiscCacheSuccess_t  gDiscoveryCacheSuccess;
#endif

#if ((gStandardSecurity_d || gHighSecurity_d) && gApsMaxEntriesForPermissionsTable_c)
extern bool_t CommandHasPermission
(
  zbNwkAddr_t aSrcAddr,
  permissionsFlags_t  permissionsCategory
);
#endif /* ((gStandardSecurity_d || gHighSecurity_d) && gApsMaxEntriesForPermissionsTable_c) */

/*========================================== REQUESTS ==========================================*/

/*==============================================================================================
	==================== 2.4.3.1 Device and Service Discovery Client Services ====================
	==============================================================================================*/
#if gMatch_Desc_req_d
/*-------------------- Match_Desc_req --------------------
	2.4.3.1.7 Match_Desc_req. (ClusterID=0x0006)

	The Match_Desc_req command is generated from a local device wishing to find
	remote devices supporting a specific simple descriptor match criterion. This
	command shall either be broadcast to all RxOnWhenIdle devices or unicast. If the
	command is unicast, it shall be directed either to the remote device itself or to an
	alternative device that contains the discovery information of the remote device.
	The local device shall generate the Match_Desc_req command using the format
	illustrated in Table 2.47. The NWKAddrOfInterest field shall contain the
	broadcast to all RxOnWhenIdle devices network address (0xfffd), if the command
	is to be broadcast, or the network address of the remote device for which the
	match is required.

	IN: The buffer where the request will be generated.
	IN: The package with the request data.
	IN: The Address of the destination node.

	OUT: The Size of the payload for the request.
*/
zbSize_t Match_Desc_req
(
  uint8_t *pkgPayload,                           /* IN: The buffer where the request will be generated. */
  zbMatchDescriptorRequestPtr_t *pMessageComingIn,  /* IN: The package with the request data. */
  zbNwkAddr_t aDestAddr                          /* IN: The Address of the destination node. */
)
{
  zbSize_t  payloadLength;

  FLib_MemCpy(pkgPayload , pMessageComingIn, MbrOfs(zbMatchDescriptorRequestPtr_t, inputClusterList));

  payloadLength = MbrOfs(zbMatchDescriptorRequestPtr_t, inputClusterList) +
                  ZDP_CopyClusterListPairToFrame(pkgPayload + MbrOfs(zbMatchDescriptorRequestPtr_t, inputClusterList), 
                  (void *)&pMessageComingIn->inputClusterList);

  Zdp_GenerateDataReq(gMatch_Desc_req_c, aDestAddr, (afToApsdeMessage_t *)(pkgPayload - SapHandlerDataStructureOffSet), payloadLength );
  return gZdpAlreadyHanlded_c;
}
#endif

#if gUser_Desc_set_d
/*-------------------- User_Desc_set --------------------
	2.4.3.1.12 User_Desc_set. (ClusterID=0x0014)

	The User_Desc_set command is generated from a local device wishing to
	configure the user descriptor on a remote device. This command shall be unicast
	either to the remote device itself or to an alternative device that contains the
	discovery information of the remote device.
	The local device shall generate the User_Desc_set command using the format
	illustrated in Table 2.52. The NWKAddrOfInterest field shall contain the network
	address of the remote device for which the user descriptor is to be configured and
	the UserDescription field shall contain the ASCII character string that is to be
	configured in the user descriptor.

	IN: The buffer where the request data will be filled in.
	IN: The package with the request information.
	IN: The destination address where the message will be send.
*/
zbSize_t User_Desc_set
(
	uint8_t  *pkgPayload,                      /* IN: The buffer where the request data will be filled in. */
	zbUserDescriptorSet_t  *pMessageComingIn,  /* IN: The package with the request information. */
	zbNwkAddr_t  aDestAddr                     /* IN: The destination address where the message will be send. */
)
{
	zbSize_t  payloadLength;

	FLib_MemSet(pkgPayload, 0x20, sizeof(zbUserDescriptorSet_t));
	payloadLength = (MbrOfs(zbUserDescriptorSet_t, descriptor.aUserDescription[0]) + pMessageComingIn->descriptor.length);
	FLib_MemCpy(pkgPayload, pMessageComingIn,payloadLength);
	Zdp_GenerateDataReq( gUser_Desc_set_c, aDestAddr, (afToApsdeMessage_t *)(pkgPayload -SapHandlerDataStructureOffSet), sizeof(zbUserDescriptorSet_t) );
	return gZdpAlreadyHanlded_c;
}
#endif

#if gNode_Desc_store_req_d
/*-------------------- Node_Desc_store_req --------------------
	2.4.3.1.15 Node_Desc_store_req. (ClusterID=0x0017)

	The Node_Desc_store_req is provided to enable ZigBee end devices on the
	network to request storage of their Node Descriptor on a Primary Discovery
	Cache device which has previously received a SUCCESS status from a
	Discovery_store_req to the same Primary Discovery Cache device. Included in
	this request is the Node Descriptor the Local Device wishes to cache.

	IN/OUT: The destination address where to send the request.

	OUT: The size in bytes of the request payload.
*/
zbStatus_t Node_Desc_store_req
(
	uint8_t *pAddress  /* IN/OUT: The destination address where to send the request. */
)
{
	if (gDiscoveryCacheResponse)
			Copy2Bytes(pAddress, gDiscoveryCacheSuccess.aNwkAddress);

	return sizeof(zbNodeDescriptorStoreRequest_t);
}
#endif

#if gPower_Desc_store_req_d
/*-------------------- Power_Desc_store_req --------------------
	2.4.3.1.16 Power_Desc_store_req. (ClusterID=0x0018)

	The Power_Desc_store_req is provided to enable ZigBee end devices on the
	network to request storage of their Power Descriptor on a Primary Discovery
	Cache device which has previously received a SUCCESS status from a
	Discovery_store_req to the same Primary Discovery Cache device. Included in
	this request is the Power Descriptor the Local Device wishes to cache.

	IN/OUT: The destination address where to send the request.

	OUT: The size in bytes of the request payload.
*/
zbSize_t Power_Desc_store_req
(
	zbNwkAddr_t  aNwkAddress  /* IN/OUT: The destination address where to send the request. */
)
{
	if (gDiscoveryCacheResponse)
				Copy2Bytes(aNwkAddress, gDiscoveryCacheSuccess.aNwkAddress);
	
	return sizeof(zbPowerDescriptorStoreRequest_t);
}
#endif

#if gActive_EP_store_req_d
/*-------------------- Active_EP_store_req --------------------
	2.4.3.1.17 Active_EP_store_req. (ClusterID=0x0019)

	The Active_EP_store_req is provided to enable ZigBee end devices on the
	network to request storage of their list of Active Endpoints on a Primary
	Discovery Cache device which has previously received a SUCCESS status from a
	Discovery_store_req to the same Primary Discovery Cache device. Included in
	this request is the count of Active Endpoints the Local Device wishes to cache and
	the endpoint list itself.

	IN/OUT: The destination address where to send the request.
	IN: The package with the request information.

	OUT: The size in bytes of the request payload.
*/
zbSize_t Active_EP_store_req
(
	zbNwkAddr_t  aAddress,                       /* IN/OUT: The destination address where to send the request. */
	zbActiveEPStoreRequest_t  *pMessageGoingOut  /* IN: The package with the request information. */
)
{
	if (gDiscoveryCacheResponse)
				Copy2Bytes(aAddress, gDiscoveryCacheSuccess.aNwkAddress);

	return (MbrOfs(zbActiveEPStoreRequest_t, activeEPList[0]) + pMessageGoingOut->activeEPCount);
}
#endif

#if gSimple_Desc_store_req_d
/*-------------------- Simple_Desc_store_req --------------------
	2.4.3.1.18 Simple_Desc_store_req. (ClusterID=0x001a)

	The Simple_desc_store_req is provided to enable ZigBee end devices on the
	network to request storage of their list of Simple Descriptors on a Primary
	Discovery Cache device which has previously received a SUCCESS status from a
	Discovery_store_req to the same Primary Discovery Cache device. Note that each
	Simple Descriptor for every active endpoint on the Local Device must be
	individually uploaded to the Primary Discovery Cache device via this command
	to enable cached discovery. Included in this request is the length of the Simple
	Descriptor the Local Device wishes to cache and the Simple Descriptor itself. The
	endpoint is a field within the Simple Descriptor and is accessed by the Remote
	Device to manage the discovery cache information for the Local Device.

	IN/OUT: The buffer where the request data will be filled in.
	IN: The package with the request information.
	IN: The destination address where to send the request.

	OUT: The size in bytes of the request payload.
*/
zbSize_t Simple_Desc_store_req
(
	uint8_t *pkgPayload,                                 /* IN/OUT: The buffer where the request data will be filled in. */
	zbSimpleDescriptorStoreRequest_t *pMessageComingIn,  /* IN: The package with the request information. */
	zbNwkAddr_t aDestAddr                                /* IN: The destination address where to send the request. */
)
{
	zbSize_t  payloadLength;

	if (gDiscoveryCacheResponse)
			Copy2Bytes(aDestAddr, gDiscoveryCacheSuccess.aNwkAddress);

	FLib_MemCpy(pkgPayload, pMessageComingIn,MbrOfs(zbSimpleDescriptorStoreRequest_t, simpleDescriptor.inputClusters));

	payloadLength = MbrOfs(zbSimpleDescriptorStoreRequest_t, simpleDescriptor.inputClusters) + 
									ZDP_CopyClusterListPairToFrame(pkgPayload + MbrOfs(zbSimpleDescriptorStoreRequest_t, simpleDescriptor.inputClusters), 
									&(pMessageComingIn->simpleDescriptor.inputClusters));

	Zdp_GenerateDataReq(gSimple_Desc_store_req_c, aDestAddr, (afToApsdeMessage_t *)(pkgPayload - SapHandlerDataStructureOffSet) , payloadLength );

	return gZdpAlreadyHanlded_c;
}
#endif

/*==============================================================================================
	==== 2.4.3.2 End Device Bind, Bind, Unbind and Bind Management Client Services Primitives ====
	==============================================================================================*/
#if gEnd_Device_Bind_req_d
/*-------------------- End_Device_Bind_req --------------------
	2.4.3.2.1 End_Device_Bind_req. (ClusterID=0x0020)

	The End_Device_Bind_req is generated from a Local Device wishing to perform
	End Device Bind with a Remote Device. The End_Device_Bind_req is generated,
	typically based on some user action like a button press. The destination addressing
	on this command shall be unicast and the destination address shall be that of the
	ZigBee Coordinator.

	IN/OUT: The buffer where the request data will be filled in.
	IN: The package with the request information.
	IN: The destination address where to send the request.

	OUT: The size in bytes of the request payload.
*/
zbSize_t End_Device_Bind_req
(
	uint8_t  *pkgPayload,                        /* IN/OUT: The buffer where the request data will be filled in. */
	zbEndDeviceBindRequest_t *pMessageComingIn,  /* IN: The package with the request information. */
	zbNwkAddr_t aDestAddr                        /* IN: The destination address where to send the request. */
)
{
	zbSize_t  payloadLength;

	FLib_MemCpy(pkgPayload, pMessageComingIn, MbrOfs(zbEndDeviceBindRequest_t, inputClusterList));
	payloadLength = MbrOfs(zbEndDeviceBindRequest_t, inputClusterList) + 
									ZDP_CopyClusterListPairToFrame(pkgPayload + MbrOfs(zbEndDeviceBindRequest_t, inputClusterList), 
									&pMessageComingIn->inputClusterList);

	Zdp_GenerateDataReq( gEnd_Device_Bind_req_c, aDestAddr, (afToApsdeMessage_t *)(pkgPayload - SapHandlerDataStructureOffSet) , payloadLength );

	return gZdpAlreadyHanlded_c;
}
#endif

#if gBind_req_d || gUnbind_req_d
/*-------------------- Bind_Unbind_req --------------------
	2.4.3.2.2 Bind_req and 2.4.3.2.3 Unbind_req. (ClusterID=0x0021) - (ClusterID=0x0022)

	Retunrs the proper size in bytes for the Bind_req and Unbind_req primitives.

	IN: The request package with the message information.

	OUT: The size in bytes for the request package.
*/
zbSize_t Bind_Unbind_req
(
	zbBindUnbindRequest_t  *pMessageComingIn  /* IN: The request package with the message information. */
)
{
	if (pMessageComingIn->addressMode == zbGroupMode)
		return (MbrOfs(zbBindUnbindRequest_t, destData) + sizeof(groupMode_t));
	else
		return (MbrOfs(zbBindUnbindRequest_t, destData) + sizeof(extendedMode_t));
}
#endif

#if gStore_Bkup_Bind_Entry_req_d
/*-------------------- Store_Bkup_Bind_Entry_req --------------------
	2.4.3.2.6 Store_Bkup_Bind_Entry_req. (ClusterID=0x0025)

	The Store_Bkup_Bind_Entry_req is generated from a local primary binding table
	cache and sent to a remote backup binding table cache device to request backup
	storage of the entry. It will be generated whenever a new binding table entry has
	been created by the primary binding table cache. The destination addressing mode
	for this request is unicast.

	IN: The package with the request information.

	OUT: The size in bytes of the request payload.
*/
zbSize_t Store_Bkup_Bind_Entry_req
(
	zbStoreBkupBindEntryRequest_t *pMessageComingIn  /* IN: The package with the request information. */
)
{
	if (pMessageComingIn->addressMode == zbGroupMode)
		return (MbrOfs(zbStoreBkupBindEntryRequest_t, destData) + sizeof(groupMode_t));
	else
		return (MbrOfs(zbStoreBkupBindEntryRequest_t, destData) + sizeof(extendedMode_t));
}
#endif

#if gRemove_Bkup_Bind_Entry_req_d
/*-------------------- Remove_Bkup_Bind_Entry_req --------------------
	2.4.3.2.7 Remove_Bkup_Bind_Entry_req. (ClusterID=0x0026)

	The Remove_Bkup_Bind_Entry_req is generated from a local primary binding
	table cache and sent to a remote backup binding table cache device to request
	removal of the entry from backup storage. It will be generated whenever a binding
	table entry has been unbound by the primary binding table cache. The destination
	addressing mode for this request is unicast.

	IN: The package with the request information.

	OUT: The size in bytes of the request payload.
*/
zbSize_t Remove_Bkup_Bind_Entry_req
(
	zbRemoveBackupBindEntryRequest_t *pMessageComingIn  /* IN: The package with the request information. */
)
{
	if (pMessageComingIn->addressMode == zbGroupMode)
		return (MbrOfs(zbRemoveBackupBindEntryRequest_t, destData) + sizeof(groupMode_t));
	else
		return (MbrOfs(zbRemoveBackupBindEntryRequest_t, destData) + sizeof(extendedMode_t));
}
#endif

#if gBackup_Bind_Table_req_d
/*-------------------- Backup_Bind_Table_req --------------------
	2.4.3.2.8 Backup_Bind_Table_req. (ClusterID=0x0027)

	The Backup_Bind_Table_req is generated from a local primary binding table
	cache and sent to the remote backup binding table cache device to request backup
	storage of its entire binding table. The destination addressing mode for this
	request is unicast.

	IN: The package with the request information.

	OUT: The size in bytes of the request payload.
*/
zbSize_t Backup_Bind_Table_req
(
	zbBackupBindTableRequest_t  *pMessageComingIn  /* IN: The package with the request information. */
)
{
	zbSize_t  payloadLength;
	uint8_t  *pPtr = NULL;
	uint16_t  cIndex;
	uint16_t  limit;

	payloadLength = MbrOfs(zbBackupBindTableRequest_t, BindingTableList[0]);
	pPtr = (uint8_t *)&pMessageComingIn->BindingTableList[0];

	limit = OTA2Native16(TwoBytesToUint16(pMessageComingIn->BindingTableListCount));
	for (cIndex = 0; cIndex < limit; cIndex++)
	{
		if (((zbApsmeBindEntry_t *)pPtr)->dstAddrMode == zbGroupMode)
		{
			payloadLength += (MbrOfs(zbApsmeBindEntry_t, dstAddrMode) + sizeof(zbGroupId_t));
			pPtr += (MbrOfs(zbApsmeBindEntry_t, dstAddrMode) + sizeof(zbGroupId_t));
		}
		else
		{
			payloadLength += sizeof(zbApsmeBindEntry_t);
			pPtr += sizeof(zbApsmeBindEntry_t);
		}
	}
	return (payloadLength);
}
#endif

#if gBackup_Source_Bind_req_d
/*-------------------- Backup_Source_Bind_req --------------------
	2.4.3.2.10 Backup_Source_Bind_req. (ClusterID=0x0029)

	The Backup_Source_Bind_req is generated from a local primary binding table
	cache and sent to a remote backup binding table cache device to request backup
	storage of its entire source table. The destination addressing mode for this request
	is unicast.

	IN: The package with the request information.

	OUT: The size in bytes of the request payload.
*/
zbSize_t Backup_Source_Bind_req
(
	zbBackupSourceBindRequest_t  *pMessageComingIn  /* IN: The package with the request information. */
)
{

	return ((MbrOfs(zbBackupSourceBindRequest_t, SourceTableList[0]) +
					(MbrSizeof(zbBackupSourceBindRequest_t, SourceTableList[0]) *
					OTA2Native16(TwoBytesToUint16(pMessageComingIn->SourceTableListCount)))));
}
#endif

/*#if (gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d)*/
#if gMgmt_Permit_Joining_req_d
/*-------------------- Mgmt_Permit_Joining_req  --------------------

	IN: The package with the request information.

	OUT: The size in bytes of the request payload.
*/
zbSize_t Mgmt_Permit_Joining_req
(
zbMgmtPermitJoiningRequest_t *pMessageComingIn, /*IN The package with the request information.  */
zbNwkAddr_t aDestAddr /* IN: The destination address where to send the request.  */
)
{
zdoNlmeMessage_t * pZdoNlmeMsg;

  /* If the destiantion address is to all awake devices, first turn it on locally and then send it OTA. */
  if(FLib_MemCmp((uint8_t *)gaBroadcastZCnZR, aDestAddr, sizeof(zbNwkAddr_t)))
  {
    /* Do the permit joinig locally as well. */
    pZdoNlmeMsg = MSG_AllocType(zdoNlmeMessage_t);

    /* If we can not set this locally is highly probably that we are not able to send packets OTA. */
    if(pZdoNlmeMsg == NULL)
      return 0;
    if (NlmeGetRequest(gDevType_c) != gEndDevice_c)
    {          
      /* Build the NLME request using the incoming info. */
      BeeUtilZeroMemory(pZdoNlmeMsg,sizeof(zdoNlmeMessage_t ));
      pZdoNlmeMsg->msgData.permitJoiningReq.permitDuration = pMessageComingIn->PermitDuration;
      pZdoNlmeMsg->msgType = gNlmePermitJoiningRequest_c;
      (void)ZDO_NLME_SapHandler(pZdoNlmeMsg);
    }
  }

  /* By returning the size ZDP send out the request. */
  return sizeof(zbMgmtPermitJoiningRequest_t);
}
#endif
/*#endif*/

#if gMgmt_Direct_Join_req_d
zbSize_t Mgmt_Direct_Join_req
(
  zbMgmtDirectJoinRequest_t *pMsgComingIn
)
{
  zdoNlmeMessage_t  *pZdoNlmeMsg;

#if ((gStandardSecurity_d || gHighSecurity_d) && gApsMaxEntriesForPermissionsTable_c)
  nlmeZdoMessage_t zdoMsg;
#endif

#if ((gStandardSecurity_d || gHighSecurity_d) && gApsMaxEntriesForPermissionsTable_c)
  if (!CommandHasPermission(NlmeGetRequest(gNwkShortAddress_c), gNetworkSettingsPermission_c))
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

    ZDO_SendEvent(gZdoEvent_ManagementCommand_c);

    (void)ZDO_NLME_SapHandler((void *)pZdoNlmeMsg);
  }

  return 0;
}
#endif


#if (gMgmt_NWK_Update_req_d && ( gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d))// && gNetworkManagerCapability_d)
/*-------------------- Mgmt_NWK_Update_req --------------------
  2.4.3.3.9 Mgmt_NWK_Update_req. (ClusterID=0x0038)

  This command is provided to allow updating of network configuration parameters
  or to request information from devices on network conditions in the local
  operating environment. The destination addressing on this primitive shall be
  unicast or broadcast to all devices for which macRxOnWhenIdle = TRUE.
*/
zbSize_t Mgmt_NWK_Update_req
(
  zbMgmtNwkUpdateRequest_t  *pRequest,
  zbNwkAddr_t               aDestAddr
)
{
  zbSize_t  size = MbrOfs(zbMgmtNwkUpdateRequest_t, ExtraData);


  if (!ZdoGetNwkManagerBitMask(gaServerMask))
  {
    (void) pRequest;
    (void) aDestAddr;
    (void) size;
    return gZero_c;
  }

  if (!IsValidNwkUnicastAddr(aDestAddr) && !IsEqual2Bytes(aDestAddr,gaBroadcastRxOnIdle))
  {
    /*
      Consider it as an invalid request.
    */
    return gZero_c;
  }

  /*
    The values 0x00 and 0x05 are acording to the Table 2.87 Fields of the
    Mgmt_NWK_Update_req Command , since is a the ScanDuration is a uint8 then 
    it is not necesary to make the verification "pRequest->ScanDuration >= 0x00",
    that is why is commented.
  */

  /*
    Do a scan.
  */
  if ( pRequest->ScanDuration <= 0x05)
  {
     return (size + MbrSizeof(zbudata_t, ScanCount));
  }

  /*
    Channel channel.
  */
  else if (pRequest->ScanDuration == 0xfe)
  {
#if gFrequencyAgilityCapability_d
    /*
      This is a special case, is when the application commands the channge with out using
      the state machine.
    */
    if (ZdoGetNwkManagerBitMask(gaServerMask)&&(FA_GetNwkManagerState() == gInitState_c))
    {
      /*
        Place holder to the state machine of the Nwk Manager, to avoid hiting the same
        state many times.
      */
      FA_SetNwkManagerState(gSelfChannelChange_c);

      /*
        Send the same command to our selfs, in order to use the mechanism.
      */
      ASL_Mgmt_NWK_Update_req(NlmeGetRequest(gNwkShortAddress_c), /* To Us. */
                              pRequest->aScanChannels,  /* Same Channel list ot be use. */
                              0xfe,  /* Do a channel change. */
                              pRequest->ExtraData.NwkManagerData.nwkUpdateId);  /* No scan duration, is a channel change. */
    }

    /*
      This will make ZDP send the request to other devices.
    */
    return (size + MbrSizeof(zbNwkManagerData_t, nwkUpdateId));
#endif
  }

  /*
    Update NwkUpdateId and channel list.
  */
  else if(pRequest->ScanDuration == 0xff)
  {
    return (size + MbrSizeof(zbNwkManagerData_t, nwkUpdateId) +
            MbrSizeof(zbNwkManagerData_t, aNwkManagerAddr));
  }

  /*
    For any other value of Scanduration then it only needs the Scan Count.
  */
  return (size + MbrSizeof(zbudata_t, ScanCount));
}
#endif



#if (gMgmt_NWK_Update_notify_d && ( gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d))&&(!gNetworkManagerCapability_d || gComboDeviceCapability_d)
zbSize_t Mgmt_NWK_Update_notify
(
  zbMgmtNwkUpdateNotify_t  *pRequest
)
{
  zbSize_t  size = MbrOfs(zbMgmtNwkUpdateNotify_t, aEnergyValues);

    if (ZdoGetNwkManagerBitMask(gaServerMask))
  {
    (void) pRequest;
    (void) size;
    return gZero_c;
  }
  return (size + (pRequest->ScannedChannelsListCount /* *MbrSizeof(zbMgmtNwkUpdateNotify_t, aEnergyValues)*/));
}
#endif
/*==============================================================================================
	========================= 2.4.3.3 Network Management Client Services =========================
	==============================================================================================*/

/*                    NONE                    */
