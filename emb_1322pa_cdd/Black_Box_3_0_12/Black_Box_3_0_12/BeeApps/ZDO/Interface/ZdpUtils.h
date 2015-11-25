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
#ifndef _ZDP_UTILS_H_
#define _ZDP_UTILS_H_

#include "ZigBee.h"
#include "ZdpManager.h"
#include "ApsMgmtInterface.h"

#define gNodeDescriptorSize  (sizeof(zbNodeDescriptor_t))
#define gPowerDescriptorSize  (sizeof(zbPowerDescriptor_t))

/*========================================== UTILS ==========================================*/

#if gNWK_addr_rsp_d || gIEEE_addr_rsp_d
/*-------------------- NWK_addr_And_IEEE_addr_rsp --------------------
	This function is ZDP internal.

	Generates the response to the NWK_addr_req or IEEE_addr_req command.

	IN: The package with the request information.
	IN/OUT: The buffer where the response data will be filled in.
	IN: The Id of the request coming in.

	OUT: The size in bytes of the response payload.
*/
zbSize_t NWK_addr_And_IEEE_addr_rsp
(
	uint8_t  *pMessageComingIn,              /* IN: The package with the request information. */
	zbAddressResponse_t  *pMessageGoingOut,  /* IN/OUT: The buffer where the request data will be filled in. */
	zbMsgId_t  clusterId                     /* IN: The Id of the request coming in. */
);
#endif

#if gNode_Desc_rsp_d || gPower_Desc_rsp_d || gUser_Desc_rsp_d
/*-------------------- Node_Desc_Power_Desc_And_User_Desc_rsp --------------------
	This function is ZDP internal.

	Genertes the response for the Node Descriptro, Power Descriptor and
	User Descriptor.

	IN: The package with the request information.
	IN/OUT: The buffer where the response data will be filled in.
	IN: The Id of the request coming in.

	OUT: The size in bytes of the response payload.
*/
zbSize_t Node_Desc_Power_Desc_And_User_Desc_rsp
(
	uint8_t  *pMessageComingIn,  /* IN: The package with the request information. */
	uint8_t  *pMessageGoingOut,  /* IN/OUT: The buffer where the response data will be filled in. */
	uint8_t  clusterId           /* IN: The Id of the request coming in. */
);
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

	OUT: The size in bytes of the response payload.
*/
zbSize_t Descriptor_Store_rsp
(
	zbMsgId_t  clusterId,        /* IN: The ID of the incoming request. */
	uint8_t  *pMessageGoingOut,  /* IN/OUT: The buffer where the response data will be filled in. */
	zbCounter_t  count,          /* IN: The size in bytes of the descriptor to store. */
	uint8_t  *pDataToStore,      /* IN: The descriptor to store. */
	zbNwkAddr_t  aNwkAddress     /* IN: The short address of the requesting device. */
);
#endif

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
);
#endif

#if gBind_Register_rsp_d
/*------------------- Zdp_UpDateBindingTableInfo -------------------
	This function is ZDP internal.

	Thsi functions stores every Bind entrie coming int the BindRegister response command
	in its own source bindig table, the procedure adding binding entries by using the bind
	APS_BindReq.

	IN: The Bind Register Response package.

	OUT: The status of the store opertaion.
*/
zbStatus_t  Zdp_UpDateBindingTableInfo
(
	zbBindRegisterResponse_t  *pRegisterResponse  /* IN: The Bind Register Response package. */
);
#else
#define Zdp_UpDateBindingTableInfo(pRegisterResponse)  gZero_c
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
);
#else
#define ZDP_GetBindingCacheEntryListByAddress(aIeeeAddress, pBindingEntryList, listPayload)  gZero_c
#endif

#if gBind_Register_rsp_d
/*-------------------- ZDP_RecordSourceBindAddress -------------------
	This fucntion is ZDP Internal.

	Stores the IEEE address of the device soliciting its ownbinding information.
	If the address has being adde previously or is added returns SUCCESS, otherwise
	returns TABLE FULL.

	IN: The IEEE Addres to store.

	OUT: SUCCESS if the adres is already inthe table or is added, otherwise TBALE FULL.
*/
zbStatus_t ZDP_RecordSourceBindAddress
(
	zbIeeeAddr_t  aIeeeAddress  /* IN: The IEEE Addres to store. */
);
#else
#define ZDP_RecordSourceBindingAddress(aIeeeAddress)  gZero_c
#endif

/******************************************************************************
* This function adds an entry in the discovery store table. 
*
* Interface assumptions:
*   aNwkAddress and aIeeeAddress are not null.
*
* Return Value:
*   gZbSuccess if the entry was added successfully otherwise gZdoInsufficientSpace_c 
*
* Effects on global data:
*   None
*
******************************************************************************/
#if gDiscovery_store_rsp_d
zbStatus_t UpdateDiscoveryStoreTable
(
	zbNwkAddr_t  aNwkAddress,
	zbIeeeAddr_t  aIeeeAddress
);
#else
#define UpdateDiscoveryStoreTable(aNwkAddress,aIeeeAddress)  gZdoInsufficientSpace_c
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
);
#else
#define  Zdp_UpdateSystemServerDiscoveryResponseTable(pNwkAddress,pServerMask)  (void)pNwkAddress
#endif

#if gBind_req_d || gUnbind_req_d
/*-------------------- Bind_Unbind_req --------------------
	This functions is ZDP internal.

	Retunrs the proper size in bytes for the Bind_req and Unbind_req primitives.

	IN: The request package with the message information.

	OUT: The size in bytes for the request package.
*/
zbSize_t Bind_Unbind_req
(
	zbBindUnbindRequest_t  *pMessageComingIn  /* IN: The request package with the message information. */
);
#else
#define Bind_Unbind_req(pMessageComingIn)  gZero_c
#endif

#if gBind_rsp_d && gBindCapability_d
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
);
#else
#define Process_BindReq(sequenceNumber)  gZero_c
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
);
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
);
#else
#define ZDP_AddToBindingCacheTable(pBindRequest)  TRUE
#endif


#if gReplace_Device_rsp_d
/*-------------------- Process_ReplaceDevices --------------------
	This function is ZDP internal and Replace Device state machine exclusive.

	This fucntion is the main procedure for the repalce device state machine,
	is inserted in the ZDP_StateMachine engine as a separated state machine.

	IN: The sequence number of particular call, this seuqence number helps to unique identify
	which replace device state machine are we preocessing.

	OUT: NONE
*/
void Process_ReplaceDevices
(
	zbCounter_t sequenceNumber  /* IN: The sequence number of particular call, 
																			this seuqence number helps to unique identify 
																			which bind state machine are we preocessing. */
);

/******************************************************************************
* This function remove an entry of the backup bind table. 
*
* Interface assumptions:
*   NONE
*
* Return Value:
*   NONE 
*
* Effects on global data:
*   None
*
******************************************************************************/
void Process_RemoveDevReq
(
	zbCounter_t  sequenceNumber
);
#else
#define Process_ReplaceDevices(sequenceNumber)
#define Process_RemoveDevReq(sequenceNumber)
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
);
#else
#define ZDP_SearchInBindCache(matchingAddress,addressMode,index)  gZero_c
#endif

/******************************************************************************
* This function remove an entry of the Binding Cache Table. 
*
* Interface assumptions:
*   pBindEntry is not null.
*
* Return Value:
*   gZbSuccess if the entry was removed successfully otherwise gZdoNoEntry_c 
*
* Effects on global data:
*   BindingCacheTable
*
******************************************************************************/
#if gUnbind_rsp_d && gBind_Register_rsp_d
zbStatus_t Zdp_RemoveBindCache
(
	zbApsmeBindReq_t  *pBindEntry  /* IN: The entry to match. */
);
#else
#define Zdp_RemoveBindCache(pBindEntry)  gZdoNoEntry_c
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
);
#else
#define ZDP_MatchBindCacheEntry(pBindEntry, addressMode)  gZdoNoEntry_c
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
);
#endif

#if (gBindCapability_d && (gBind_rsp_d || gUnbind_rsp_d)) || gReplace_Device_rsp_d
/*-------------------- ZDP_StateMachine --------------------
	This function is ZDP internal.

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
);
#else
#define ZDP_StateMachine(machineId, sequenceNumber)
#endif

#if (gBindCapability_d && (gBind_rsp_d || gUnbind_rsp_d)) || gReplace_Device_rsp_d
/*-------------------- ZDP_SetRegisterMachineData --------------------
	This function is ZDP internal.

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
	zbNwkAddr_t  SrcAddr,      /* IN: The source address of the originator of initial request. */
	void  *data                /* IN: The particular data that needs to keep in memory. */
);

/*-------------------- ZDP_GetIndexMachineProcessQueue --------------------
	This function is ZDP Internal
	
	This function in multiplexed to be used in different ways,
	but the main is retriving the index of different storage places

	IN: The state machine ID for which we are going to serach the info
	IN: The sequence number to match and identify the info, this method
	can have multiple states machines of the same kind.

	OUT: The index where the information is.
*/
index_t ZDP_GetIndexMachineProcessQueue
(
	zbMachineId_t  machineId,    /* IN: The state machine ID for which we are looking the informtion. */
	zbCounter_t  sequenceNumber  /* IN: The sequence number used as an unique ID for the stored information. */
);
#else
#define ZDP_SetRegisterMachineData(machineId, SrcAddr, data)  gZdoNoEntry_c
#define ZDP_GetIndexMachineProcessQueue(machineId, sequenceNumber)  gZdoNoEntry_c
#endif
/*-------------------- ZDP_SearchDeviceHoldingItsOwnBindingInfo --------------------
	This function is ZDP internal.

	This functions is part of the interface for accessing the table 
	of the devices holding its own bindig info.

	IN: The IEEE Address of the that will be serach inthe table.

	OUT: The index in the table where the devices is located or the proper error code if is not found.
*/
index_t ZDP_SearchDeviceHoldingItsOwnBindingInfo
(
	zbIeeeAddr_t  aAddress
);

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
);
#else
#define ZDP_MatchAddressToService(aNwkAddress, serverMaskToMach)  FALSE
#endif

#if gMgmt_Bind_rsp_d
/*-------------------- RetriveBindingCacheIndex ------------------
	This function is ZDP internal.

	Search in the binding table or the bindin chc table and returns the
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
);

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
);
#else
#define RetriveBindingCacheIndex(requestIndex, tableType)  gZero_c
#define CountEntriesInTable(pTable, tableType)  gZero_c
#endif

/******************************************************************************
* This function find a valid entries in the cache table corresponding to an 
* entry per cluster bound. 
*
* Interface assumptions:
*   pMgmtBindRsp is not null.
*
* Return Value:
*   gZbSuccess or the list size 
*
* Effects on global data:
*   NONE
*
******************************************************************************/
#if gMgmt_Bind_rsp_d
zbSize_t ZDP_MgmtBindResponse_CacheTable
(
  zbIndex_t startIndex,
  zbMgmtBindResponse_t *pMgmtBindRsp
);
/******************************************************************************
* This function find a valid entries in the Bind Table corresponding to an 
* entry per cluster bound. 
*
* Interface assumptions:
*   pMgmtBindRsp is not null.
*
* Return Value:
*   gZbSuccess or the list size 
*
* Effects on global data:
*   NONE
*
******************************************************************************/
zbSize_t ZDP_MgmtBindResponse_BindTable
(
  zbIndex_t startIndex,
  zbMgmtBindResponse_t *pMgmtBindRsp
);
/******************************************************************************
* This function is used to get the bind and cluste index. 
*
* Interface assumptions:
*   NONE
*
* Return Value:
*   bind start index. 
*
* Effects on global data:
*   NONE
*
******************************************************************************/
bindIndex_t Zdp_GetTranslateToBindIndexAndCluster
(
  zbIndex_t startIndex,
  uint8_t *pClusterIndex,
  bindIndex_t *pBindIndex
);
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
);
#else
#define ZDP_SearchNodeInCache(aNwkAddress, aIeeeAddress, mode)  gZdoNoEntry_c
#endif

#if gReplace_Device_rsp_d
void GetAddressFromService
(
	uint8_t  service,
	uint8_t  *pNwkAddress
);
#else
#define GetAddressFromService(service, pNwkAddress)
#endif

/***********************************************************************
************************************************************************
* Public Prototype definitions
************************************************************************
************************************************************************/
#if gMgmt_Bind_rsp_d
/*-------------------- ConvertToOtaBindEntry --------------------
	This function is ZDP internal.

	Converts from native format to over-the-air format on a single binding table 
	entry.

	IN: Internal binding table entry.
	IN/OUT: OTA binding entry format.
*/
void ConvertToOtaBindEntry
(
	zbApsmeBindEntry_t* pBindReq,  /* IN/OUT: OTA binding entry format. */
	apsBindingTable_t *pEntry      /* IN: Internal binding table entry. */
);
#endif

/******************************************************************************
* This function is used to announce the device information of the new joined  device. 
*
* Interface assumptions:
*   aDstAddress is not null
*
* Return Value:
*   None 
*
* Effects on global data:
*   None
*
******************************************************************************/
void DeviceAnnounce(zbNwkAddr_t  aDstAddress);

/******************************************************************************
* This function takes the nlmeZdoMessage_t message and generates              a
* Mgmt-Direct-Join.rsp message to notify to the App Layer and also send the 
* Mgmt-Direct-Join.rsp message through AF_APSDE sap hander.
*
* Interface assumptions:
*   pZdoMsg is not null
*
* Return Value:
*   None 
*
* Effects on global data:
*   None
*
******************************************************************************/
#if gMgmt_Direct_Join_rsp_d && (gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d)
void Zdp_Mgmt_Direct_Join_conf
(
  nlmeZdoMessage_t *pZdoMsg
);
#else
#define Zdp_Mgmt_Direct_Join_conf(pZdoMsg)
#endif

/******************************************************************************
* This function compare a channel given against the channel list and verify if 
* that channel is valid.
*
* Interface assumptions:
*   pChannels is not null
*
* Return Value:
*   FALSE is the channel is valid otherwise TRUE 
*
* Effects on global data:
*   None
*
******************************************************************************/
#if gMgmt_NWK_Disc_rsp_d
bool_t Zdp_IsNotValidCahnnel(uint32_t *pChannels);
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
);

extern neighborTable_t* IsInNeighborTable(zbAddrMode_t addressMode, uint8_t *pAddress);

extern index_t GetFreeEntryInNeighborTable(uint8_t *pAddress,zbAddrMode_t  addressMode, bool_t selectSibling);

/*********************************************************************************/
extern void ResetNeighborTableEntry(neighborTable_t *pNeighborTableEntry);

#if gRnplusCapability_d
#if gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d
extern void RemoveRouteEntry(uint8_t *pShortAddress);
extern void ResetRouteTable ( void );
#endif
#endif


#endif  /*  */
