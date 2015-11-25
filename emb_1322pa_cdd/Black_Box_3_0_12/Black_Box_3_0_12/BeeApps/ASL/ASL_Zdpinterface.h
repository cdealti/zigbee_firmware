/******************************************************************************
* This file interfaces application with ZTC module.
*
* (c) Copyright 2008, Freescale, Inc. All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
******************************************************************************/
#ifndef _ASL_ZDP_INTERFACE_H_
#define _ASL_ZDP_INTERFACE_H_

#include "TS_Interface.h"
#include "AppZdoInterface.h"

/******************************************************************************
*******************************************************************************
* Public macros
*******************************************************************************
******************************************************************************/

/******************************************************************************
*******************************************************************************
* Public prototypes
*******************************************************************************
******************************************************************************/

#if gBeeStackIncluded_d
uint8_t ZDP_APP_SapHandler
(
	zdpToAppMessage_t *pMsg
);
#else
#define ZDP_APP_SapHandler(pMsg)  MSG_Free(pMsg)
#endif

void APP_ZDP_PermitJoinRequest
(
	uint8_t  permit /* IN: Duration */  
);

/*==============================================================================================
	==================== 2.4.3.1 Device and Service Discovery Client Services ====================
	==============================================================================================*/
/*-------------------- ASL_NWK_addr_req --------------------
	ClusterID=0x0000.

	This functions genertes a ZDP NWK_addr_req, and pass it to the 
	ZDO layer thru the APP_ZDP_SapHandler function.

	Is generated from a Local Device wishing to inquire as to the
	16 bit address of the Remote Device based on its known IEEE address. The
	destination addressing on this command shall be broadcast to all RxOnWhenIdle
	devices.

	NOTE: The address allocated for the message will be freed byt he lower layers.
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
);

#if gIEEE_addr_req_d
void ASL_IEEE_addr_req
(
	zbCounter_t  *pSequenceNumber,
	zbNwkAddr_t  aDestAddress,
	zbNwkAddr_t aNwkAddrOfInterest,
	uint8_t  requestType,
	index_t  startIndex
);
#else
#define ASL_IEEE_addr_req(pSequenceNumber,aDestAddress,aNwkAddrOfInterest,requestType,startIndex)
#endif

#if gNode_Desc_req_d
void ASL_Node_Desc_req
(
	zbCounter_t  *pSequenceNumber,
	zbNwkAddr_t  aDestAddress
);
#else
#define ASL_Node_Desc_req(pSequenceNumber,aDestAddress)
#endif

#if gPower_Desc_req_d
void ASL_Power_Desc_req
(
	zbCounter_t  *pSequenceNumber,
	zbNwkAddr_t  aDestAddress
);
#else
#define ASL_Power_Desc_req(pSequenceNumber,aDestAddress)
#endif

#if gSimple_Desc_req_d
void ASL_Simple_Desc_req
(
	zbCounter_t  *pSequenceNumber,
	zbNwkAddr_t  aDestAddress,
	zbEndPoint_t  endPoint
);
#else
#define ASL_Simple_Desc_req(pSequenceNumber,aDestAddress,endPointDesc)
#endif

#if gExtended_Simple_Desc_req_d
void ASL_Extended_Simple_Desc_req
(
	zbCounter_t  *pSequenceNumber,
	zbNwkAddr_t  aDestAddress,
	zbEndPoint_t  endPoint,
	index_t startIndex
);
#else
#define ASL_Extended_Simple_Desc_req(pSequenceNumber,aDestAddress,endPointDesc,startIndex)
#endif

#if gActive_EP_req_d
void ASL_Active_EP_req
(
	zbCounter_t  *pSequenceNumber,
	zbNwkAddr_t  aDestAddress
);
#else
#define ASL_Active_EP_req(pSequenceNumber,aDestAddress)
#endif

#if gExtended_Active_EP_req_d
void ASL_Extended_Active_EP_req
(
	zbCounter_t  *pSequenceNumber,
	zbNwkAddr_t  aDestAddress,
	index_t startIndex
);
#else
#define ASL_Extended_Active_EP_req(pSequenceNumber,aDestAddress,startIndex)
#endif

#if gMatch_Desc_req_d
void ASL_MatchDescriptor_req
(
	zbCounter_t  *pSequenceNumber,
	zbNwkAddr_t  aDestAddress,
	zbSimpleDescriptor_t  *pSimpleDescriptor
);
#else
#define ASL_MatchDescriptor_req(pSequenceNumber, aDestAddress, pSimpleDescriptor)
#endif

#if gComplex_Desc_req_d
void ASL_Complex_Desc_req
(
	zbCounter_t  *pSequenceNumber,
	zbNwkAddr_t  aDestAddress
);
#else
#define ASL_Complex_Desc_req(pSequenceNumber, aDestAddress)
#endif

#if gUser_Desc_req_d
void ASL_User_Desc_req
(
	zbCounter_t  *pSequenceNumber,
	zbNwkAddr_t  aDestAddress
);
#else
#define ASL_User_Desc_req(pSequenceNumber, aDestAddress)
#endif

#if gDiscovery_Cache_req_d
void ASL_Discovery_Cache_req
(
	zbCounter_t  *pSequenceNumber,
	zbNwkAddr_t  aDestAddress,
	zbNwkAddr_t  aNwkAddrOfInterest,
	zbIeeeAddr_t  aIEEEAddrOfInterest
);
#else
#define ASL_Discovery_Cache_req(pSequenceNumber, aDestAddress, aNwkAddrOfInterest, aIEEEAddrOfInterest)
#endif

#if gDevice_annce_d
void ASL_Device_annce
(
	zbCounter_t  *pSequenceNumber,
	zbNwkAddr_t  aDestAddress,
	zbNwkAddr_t   aNwkAddress,
	zbIeeeAddr_t  aIeeeAddress,
	macCapabilityInfo_t  capability
);
#else
#define ASL_Device_annce(pSequenceNumber, aDestAddress, aNwkAddress, aIeeeAddress, capability)
#endif

#if gUser_Desc_set_d
void ASL_User_Desc_set
(
	zbCounter_t  *pSequenceNumber,
	zbNwkAddr_t  aDestAddress,
	zbSize_t  length,
	zbUserDescriptor_t  aUserDescription
);
#else
#define ASL_User_Desc_set(pSequenceNumber, aDestAddress, length, aUserDescription)
#endif

#if gSystem_Server_Discovery_req_d
void ASL_System_Server_Discovery_req
(
	zbCounter_t  *pSequenceNumber,
	zbNwkAddr_t  aDestAddress,
	zbServerMask_t aServerMask
);
#else
#define ASL_System_Server_Discovery_req(pSequenceNumber,aDestAddress,aServerMask)
#endif

#if gDiscovery_store_req_d
void ASL_Discovery_store_req
(
	zbCounter_t  *pSequenceNumber,
	zbNwkAddr_t  aDestAddress,
	zbDiscoveryStoreRequest_t  *pDiscoveryStore
);
#else
#define ASL_Discovery_store_req(pSequenceNumber, aDestAddress, pDiscoveryStore)
#endif

#if gNode_Desc_store_req_d
void ASL_Node_Desc_store_req
(
	zbCounter_t  *pSequenceNumber,
	zbNwkAddr_t  aDestAddress,
	zbNodeDescriptor_t  *pNodeDescriptor
);
#else
#define ASL_Node_Desc_store_req(pSequenceNumber, aDestAddress, pNodeDescriptor)
#endif

#if gPower_Desc_store_req_d
void ASL_Power_Desc_store_req
(
	zbCounter_t  *pSequenceNumber,
	zbNwkAddr_t  aDestAddress,
	zbPowerDescriptor_t  *pPowerDescriptor
);
#else
#define ASL_Power_Desc_store_req(pSequenceNumber, aDestAddress, pPowerDescriptor)
#endif

#if gActive_EP_store_req_d
void ASL_Active_EP_store_req
(
	zbCounter_t  *pSequenceNumber,
	zbNwkAddr_t  aDestAddress,
	zbCounter_t  activeEPcount,
	zbEndPoint_t  *pActiveEPList
);
#else
#define ASL_Active_EP_store_req(pSequenceNumber, aDestAddress, activeEPcount, pActiveEPList)
#endif

#if gSimple_Desc_store_req_d
void ASL_Simple_Desc_store_req
(
	zbCounter_t  *pSequenceNumber,
	zbNwkAddr_t  aDestAddress,
	zbSimpleDescriptorStoreRequest_t  *pSimpleDescStore
);
#else
#define ASL_Simple_Desc_store_req(pSequenceNumber, aDestAddress, pSimpleDescStore)
#endif

#if gRemove_node_cache_req_d
void ASL_Remove_node_cache_req
(
	zbCounter_t  *pSequenceNumber,
	zbNwkAddr_t  aDestAddress,
	zbNwkAddr_t  aNwkAddress,
	zbIeeeAddr_t  aIeeeAddress
);
#else
#define ASL_Remove_node_cache_req(pSequenceNumber, aDestAddress, aNwkAddress, aIeeeAddress)
#endif

#if gFind_node_cache_req_d
void ASL_Find_node_cache_req
(
	zbCounter_t  *pSequenceNumber,
	zbNwkAddr_t  aDestAddress,
	zbNwkAddr_t  aNwkAddress,
	zbIeeeAddr_t  aIeeeAddress
);
#else
#define ASL_Find_node_cache_req(pSequenceNumber, aDestAddress, aNwkAddress, aIeeeAddress)
#endif


#if gEnd_Device_Bind_req_d
void ASL_EndDeviceBindRequest
(
	zbCounter_t  *pSequenceNumber,
	zbNwkAddr_t  aDestAddress,
	zbSimpleDescriptor_t  *pSimpleDescriptor
);
#else
#define ASL_EndDeviceBindRequest(pSequenceNumber,aDestAddress,SimpleDescriptor)
#endif

#if gBind_req_d || gUnbind_req_d
void APP_ZDP_BindUnbindRequest
(
	zbCounter_t  *pSequenceNumber,
	zbNwkAddr_t  aDestAddress,
	zbMsgId_t  BindUnbind,
	zbBindUnbindRequest_t  *pBindUnBindRequest
);
#else
#define APP_ZDP_BindUnbindRequest(pSequenceNumber,aDestAddress,BindUnbind, pBindUnBindRequest)
#endif

#if gBind_Register_req_d
void ASL_Bind_Register_req
(
	zbCounter_t  *pSequenceNumber,
	zbNwkAddr_t  aDestAddress,
	zbIeeeAddr_t  aNodeAddress
);
#else
#define ASL_Bind_Register_req(pSequenceNumber, aDestAddress, aNodeAddress)
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
);
#else
#define ASL_Replace_Device_req(pSequenceNumber, aDestAddress, aOldAddress, oldEndPoint, aNewAddress, newEndPoint)
#endif

#if gStore_Bkup_Bind_Entry_req_d
void ASL_Store_Bkup_Bind_Entry_req
(
	zbCounter_t  *pSequenceNumber,
	zbNwkAddr_t  aDestAddress,
	zbStoreBkupBindEntryRequest_t  *pStoreBkupEntry
);
#else
#define ASL_Store_Bkup_Bind_Entry_req(pSequenceNumber, aDestAddress, pStoreBkupEntry)
#endif

#if gRemove_Bkup_Bind_Entry_req_d
void ASL_Remove_Bkup_Bind_Entry_req
(
	zbCounter_t  *pSequenceNumber,
	zbNwkAddr_t  aDestAddress,
	zbRemoveBackupBindEntryRequest_t  *pRemoveBkupEntry
);
#else
#define ASL_Remove_Bkup_Bind_Entry_req(pSequenceNumber, aDestAddress, pRemoveBkupEntry)
#endif

#if gBackup_Bind_Table_req_d
void ASL_Backup_Bind_Table_req
(
	zbCounter_t  *pSequenceNumber,
	zbNwkAddr_t  aDestAddress,
	zbBackupBindTableRequest_t  *pBackupBindTable
);
#else
#define ASL_Backup_Bind_Table_req(pSequenceNumber, aDestAddress, pBackupBindTable)
#endif

#if gRecover_Bind_Table_req_d
void ASL_Recover_Bind_Table_req
(
	zbCounter_t  *pSequenceNumber,
	zbNwkAddr_t  aDestAddress,
	index_t  index
);
#else
#define ASL_Recover_Bind_Table_req(pSequenceNumber, aDestAddress, index)
#endif

#if gBackup_Source_Bind_req_d
void ASL_Backup_Source_Bind_req
(
	zbCounter_t  *pSequenceNumber,
	zbNwkAddr_t  aDestAddress,
	zbBackupSourceBindRequest_t  *pBkupSourceBindTable
);
#else
#define ASL_Backup_Source_Bind_req(pSequenceNumber, aDestAddress, pBkupSourceBindTable)
#endif

#if gRecover_Source_Bind_req_d
void ASL_Recover_Source_Bind_req
(
	zbCounter_t  *pSequenceNumber,
	zbNwkAddr_t  aDestAddress,
	index_t  index
);
#else
#define ASL_Recover_Source_Bind_req(pSequenceNumber, aDestAddress, index)
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
);
#else
#define ASL_Mgmt_NWK_Disc_req(pSequenceNumber, aDestAddress, aScanChannel, scanDuration, startIndex)
#endif

#if gMgmt_Lqi_req_d
void ASL_Mgmt_Lqi_req
(
	zbCounter_t  *pSequenceNumber,
	zbNwkAddr_t  aDestAddress,
	index_t  index
);
#else
#define ASL_Mgmt_Lqi_req(pSequenceNumber, aDestAddress, index)
#endif

#if gMgmt_Rtg_req_d
void ASL_Mgmt_Rtg_req
(
	zbCounter_t  *pSequenceNumber,
	zbNwkAddr_t  aDestAddress,
	index_t  index
);
#else
#define ASL_Mgmt_Rtg_req(pSequenceNumber, aDestAddress, index)
#endif

#if gMgmt_Bind_req_d
void ASL_Mgmt_Bind_req
(
	zbCounter_t  *pSequenceNumber,
	zbNwkAddr_t  aDestAddress,
	index_t  index
);
#else
#define ASL_Mgmt_Bind_req(pSequenceNumber, aDestAddress, index)
#endif

#if gMgmt_Leave_req_d
void ASL_Mgmt_Leave_req
(
	zbCounter_t  *pSequenceNumber,
	zbNwkAddr_t  aDestAddress,
	zbIeeeAddr_t aDeviceAddres,
	zbMgmtOptions_t  mgmtOptions
);
#else
#define ASL_Mgmt_Leave_req(pSequenceNumber, aDestAddress, aDeviceAddres, mgmtOptions)
#endif

#if gMgmt_Direct_Join_req_d
void ASL_Mgmt_Direct_Join_req
(
  zbCounter_t  *pSequenceNumber,
  zbNwkAddr_t  aDestinationAddress,
  zbIeeeAddr_t  aDeviceAddres,
  uint8_t       capabilityInfo
);
#else
#define ASL_Mgmt_Direct_Join_req(pSequenceNumber, aDestinationAddress, aDeviceAddres, capabilityInfo)
#endif

#if gMgmt_Permit_Joining_req_d
void ASL_Mgmt_Permit_Joining_req
(
	zbCounter_t  *pSequenceNumber,
	zbNwkAddr_t  aDestAddress,
	zbCounter_t  permitDuration,
	uint8_t  TC_Significance
);
#else
#define ASL_Mgmt_Permit_Joining_req(pSequenceNumber, aDestAddress, permitDuration, TC_Significance)
#endif

#if gMgmt_Cache_req_d
void ASL_Mgmt_Cache_Req
(
	zbCounter_t  *pSequenceNumber,
	zbNwkAddr_t  aDestAddress,
	index_t  index
);
#else
#define ASL_Mgmt_Cache_Req(pSequenceNumber, aDestAddress, index)
#endif

void ASL_Nlme_Sync_req
(
	bool_t track
);


#if (gMgmt_NWK_Update_req_d && ( gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d))&& (gNetworkManagerCapability_d || gComboDeviceCapability_d)
void ASL_Mgmt_NWK_Update_req
(
  zbNwkAddr_t  aDestAddress,
  zbChannels_t  aChannelList,
  uint8_t       iScanDuration,
  uintn8_t      iScanCount
);
#else
#define ASL_Mgmt_NWK_Update_req(aDestAddress,aChannelList,iScanDuration,iScanCount)
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
  zbEnergyValue_t*  paEnergyVslues,
  zbStatus_t    status
);
#else
#define ASL_Mgmt_NWK_Update_Notify(aDestAddress, aScannedChannels, iTotalTransmissions, \
                         iTransmissionFailures, iScannedChannelListCount,paEnergyVslues, status)
#endif
/*gMgmt_NWK_Update_notify_d*/


/* not available to ZEDs */
#if ( gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d)
void ZdoNwkMng_EnergyScanRequest
(
  zbChannels_t  aChannelList,
  uint8_t  duration
);
#else
#define ZdoNwkMng__EnergyScanRequest(aChannelList,duration)
#endif

#if gFrequencyAgilityCapability_d 
void ASL_ChangeChannel(uint8_t  channelNumber);
#else
#define ASL_ChangeChannel(channelNumber)
#endif

#if gConcentratorFlag_d
void ASL_SendRouteDiscoveryManyToOne(
  uint8_t iRadius, 
  bool_t noRouteCacheFlag
 );
#else
#define ASL_SendRouteDiscoveryManyToOne(iRadius, noRouteCacheFlag);
#endif
/******************************************************************************
*******************************************************************************
* Public type definitions
*******************************************************************************
******************************************************************************/

/******************************************************************************
*******************************************************************************
* Public memory declarations
*******************************************************************************
******************************************************************************/
#if gBeeStackIncluded_d

/* Contains the TaskID of the main application task. */
extern tsTaskID_t gAppTaskID;

/******************************************************************************
*******************************************************************************
* Public functions
*******************************************************************************
******************************************************************************/

#endif

void ASL_ZdoCallBack(zdpToAppMessage_t *MsgFromZDP,zbCounter_t MsgCounter);

#endif  /*  _ASL_ZDP_INTERFACE_H_  */
