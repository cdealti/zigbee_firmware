/*****************************************************************************
* This is a header file for Zdp Primitives
*
* (c) Copyright 2005, Freescale, Inc. All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
******************************************************************************/

#ifndef _ZdpManager_H_
#define _ZdpManager_H_

#include "Zigbee.h"
#include "AppAfInterface.h"
#include "AppZdoInterface.h"
#include "AfApsInterface.h"

/**********************************************************************
***********************************************************************
* Public Macros
***********************************************************************
***********************************************************************/
#define ResponseClusterId 0x80
#define MaxArray 0x03
#define SapHandlerDataStructureOffSet    ApsmeGetAsduOffset()

#define SetMgmtNwkDiscReq()  (gMgmtNwkDiscReq = TRUE)
#define SetstartIndexforNwkDisc(param)  ( gStartIndexforNwkDisc = ( param ) )
#define GetStartIndexforNwkDisc()      gStartIndexforNwkDisc
#define IncrementIndex( param )  ( param ) = ( ( ( param ) <  MaxArray )? ( param )++: 0 )
#define DecrementIndex( param )  ( param ) = ( ( ( param ) > 0 )? ( param )--: 0 )
#define StoreAddress( index, strAddress )  ( Copy2Bytes( ( strAddrClusterId[ ( index ) ].address ), ( strAddress ) ) )
#define GetClusterId( index )  (strAddrClusterId[(index)].clusterId)
#define SetClusterId(index, cluster)  ((strAddrClusterId[(index)].clusterId) = (cluster))
#define GetAddress(index)  (strAddrClusterId[(index)].address)


#define mTxOptions        0x00
#define mTxOptionResSecurity 0x03
#define mTxOptionComSecurity 0x01


#define gZdpAlreadyHanlded_c  0xff

/*
	This defines are ZDP internal and ZDP_StateMachine exclusive.
*/
#define ZDP_SetSequenceId(index,sequenceId)  ( ( maEventProcessQueue[ ( index ) ].sequenceNumber ) = ( sequenceId ) )
#define ZDP_SetOriginalSequenceNumber(index,sequenceId)  ( ( maEventProcessQueue[ ( index ) ].sourceSeqNumber) = ( sequenceId ) )
#define ZDP_SetOriginatorAddress(index,address)  ( Copy2Bytes( ( maEventProcessQueue[ ( index ) ].sourceAddress) , ( address ) ) )
#define ZDP_GetOriginatorAddress(index)  ( maEventProcessQueue[ ( index ) ].sourceAddress )
#define ZDP_GetOriginalSequenceNumber(index)  ( maEventProcessQueue[ ( index ) ].sourceSeqNumber )
#define ZDP_SetMachineId(index,MachineId)  ( ( maEventProcessQueue[ ( index ) ].machineId ) = ( MachineId ) )
#define ZDP_SetResponseSeqNum(index)  ( gZdpResponseCounter = ( index ) )


/*
	use srcEndPoint of 0 to indicate a free binding entry.
*/
#define ZdpBindIsFree(bindEntry)  (!(bindEntry).srcEndPoint)
#define ZdpBindSetFree(bindEntry)  ((bindEntry).srcEndPoint = 0)

#define ZdpIsGroupBind(zbBindReq) ((zbBindReq).dstAddrMode == zbGroupMode)
#define ZdpIsGroupBindInt(apsBindEntry) ((apsBindEntry).dstEndPoint == gApsBindAddrModeGroup_c)

/*
	The server mask and its services interface.
*/
extern zbServerMask_t gaServerMask;
#define ZDP_GetServerMaskServices(serverMask)  ( *( (uint16_t *)gaServerMask) & *( (uint16_t *)( serverMask )) )
#define ZDP_GetServiceOn(service)  (*( (uint16_t *)gaServerMask) & (service))

#if gBind_Register_rsp_d || gRecover_Source_Bind_rsp_d || gBackup_Source_Bind_rsp_d || (gBindCapability_d && gBind_rsp_d)
/*
	The interface to use the table of devices holding its own information 
*/
#define ZDP_GetIEEEAddressFormHoldingBindingInfoTable(index)  ( aDevicesHoldingItsOwnBindingInfo[ ( index ) ] )
#endif

#define gMaximumEventsToProcess_c  3
#define gNotActive_c  0x00
#define gInformDevices_c  0x01

#define ZdoPrimaryTrustCenter_c  0x01
#define ZdoBackupTrustCenter_c   0x02
#define ZdoPrimaryBindingTableCache_c  0x04
#define ZdoBackupBindingTableCache_c   0x08
#define ZdoPrimaryDiscoveryCache_c     0x10
#define ZdoBackupDiscoveryCache_c      0x20
#define ZdoNetworkManager_c            0x40

#define ZdoSetPrimaryTrustCenterOn(aServerMask)        (((aServerMask)[0]) |= ZdoPrimaryTrustCenter_c)
#define ZdoSetBackupTrustCenterOn(aServerMask)         (((aServerMask)[0]) |= ZdoBackupTrustCenter_c)
#define ZdoSetPrimaryBindingTableCacheOn(aServerMask)  (((aServerMask)[0]) |= ZdoPrimaryBindingTableCache_c)
#define ZdoSetBackupBindingTableCacheOn(aServerMask)   (((aServerMask)[0]) |= ZdoBackupBindingTableCache_c)
#define ZdoSetPrimaryDiscoveryCacheOn(aServerMask)  (((aServerMask)[0]) |= ZdoPrimaryDiscoveryCache_c)
#define ZdoSetBackupDiscoveryCacheOn(aServerMask)   (((aServerMask)[0]) |= ZdoBackupDiscoveryCache_c)
#if ((gCoordinatorCapability_d) || (gRouterCapability_d) || (gComboDeviceCapability_d))
#define ZdoSetNwkManagerOn(aServerMask)               (((aServerMask)[0]) |= ZdoNetworkManager_c)
#else
#define ZdoSetNwkManagerOn(aServerMask)
#endif

#define ZdoSetPrimaryTrustCenterOff(aServerMask)  (((aServerMask)[0]) &= 0xfe)
#define ZdoSetBackupTrustCenterOff(aServerMask)  (((aServerMask)[0]) &= 0xfd)
#define ZdoSetPrimaryBindingTableCacheOff(aServerMask)  (((aServerMask)[0]) &= 0xfb)
#define ZdoSetBackupBindingTableCacheOff(aServerMask)  (((aServerMask)[0]) &= 0xf7)
#define ZdoSetPrimaryDiscoveryCacheOff(aServerMask)  (((aServerMask)[0]) &= 0xef)
#define ZdoSetBackupDiscoveryCacheOff(aServerMask)  (((aServerMask)[0]) &= 0xdf)
#if ((gCoordinatorCapability_d) || (gRouterCapability_d) || (gComboDeviceCapability_d))
#define ZdoSetNwkManagerOff(aServerMask)             (((aServerMask)[0]) &= 0xbf)
#else
#define ZdoSetNwkManagerOff(aServerMask)
#endif

#define ZdoGetPrimaryTrustCenterBitMask(aServerMask)  (((aServerMask)[0]) & ZdoPrimaryTrustCenter_c)
#define ZdoGetBackupTrustCenterBitMask(aServerMask)  (((aServerMask)[0]) & ZdoBackupTrustCenter_c)
#define ZdoGetPrimaryBindingTableCacheBitMask(aServerMask)  (((aServerMask)[0]) & ZdoPrimaryBindingTableCache_c)
#define ZdoGetBackupBindingTableCacheBitMask(aServerMask)  (((aServerMask)[0]) & ZdoBackupBindingTableCache_c)
#define ZdoGetPrimaryDiscoveryCacheBitMask(aServerMask)  (((aServerMask)[0]) & ZdoPrimaryDiscoveryCache_c)
#define ZdoGetBackupDiscoveryCacheBitMask(aServerMask)  (((aServerMask)[0]) & ZdoBackupDiscoveryCache_c)
#define ZdoGetNwkManagerBitMask(aServerMask)            (((aServerMask)[0]) & ZdoNetworkManager_c)

#define EndDeviceBit  0x02    /* 0000 0010 - This means that is something else but enddevice */
#define IsNotEndDeviceBitOn(bitMask)  (!((bitMask) & EndDeviceBit))

/* don't make the payload larger than APSDE-DATA.request can handle */
#define ZdoMaximumPayLoadSize()    (ApsmeGetMaxAsduLength(0))
#define ZdpMaximumEntriesInPayLoad(header, size)  ((uint8_t)(ZdoMaximumPayLoadSize()-(uint8_t)(header)) / (uint8_t)(size))

/* Defines to set the bits in the Mgmt_Leave command */
#define ZdpSetRemoveChildrenBitOn(mgmtOptions)  ((mgmtOptions) |= zbMgmtOptionRemoveChildren_c)
#define ZdpSetRejoinBitOn(mgmtOptions)  ((mgmtOptions) |= zbMgmtOptionReJoin_c)
#define ZdpSetRemoveChildrenBitOff(mgmtOptions)  ((mgmtOptions) &= 0xfe)
#define ZdpSetRejoinBitOff(mgmtOptions)  ((mgmtOptions) &= 0xfd)
#define ZdpGetRemoveChildrenBit(mgmtOptions)  ((mgmtOptions) & zbMgmtOptionRemoveChildren_c)
#define ZdpGetRejoinBit(mgmtOptions)  ((mgmtOptions) & zbMgmtOptionReJoin_c)

#define ZdpMgmtLeaveRemoveChildrenFlag(OptionByte)  ( ( ZdpGetRemoveChildrenBit( (OptionByte) ) ) ? TRUE: FALSE )
#define ZdpMgmtLeaveRejoinFlag(OptionByte)  ( ( ZdpGetRejoinBit( (OptionByte) )) ? TRUE: FALSE )

#if gPower_Desc_rsp_d
#define Zdp_PowerSourceLevel()  ( ( ( (uint8_t)PWRLib_LVD_ReportLevel() )<< 4) )
#else
#define Zdp_PowerSourceLevel()  0x00
#endif

#if gBind_req_d && gBind_Register_rsp_d
#define Zdp_UpdateBindResponseStatus(index, status)  (*((uint8_t *)&(maEventProcessQueue[(index)].data.bindingInfo)) = (status))
#endif

/**********************************************************************
***********************************************************************
* Public Prototypes
***********************************************************************
***********************************************************************/


/**********************************************************************
***********************************************************************
* Public type definitions
***********************************************************************
***********************************************************************/
typedef  uint8_t  zbMachineId_t;

typedef  struct  strAddrClusterId_tag
{
	zbMsgId_t  clusterId;
	zbNwkAddr_t  address;
}strAddrClusterId_t;

/*
	Data types used in the ZDP State Machine this union hold all the datatypes to store in the queue
*/
typedef struct zbReplaceDevice_tag
{
	zbReplaceDeviceRequest_t  replaceData;
	index_t  index;
}zbReplaceDevice_t;

typedef union zbEventQueueData_tag
{
	zbBindUnbindRequest_t  bindingInfo;
	zbReplaceDevice_t  replaceInfo;
}zbEventQueueData_t;

typedef struct zbEventProcessQueue_tag
{
	zbMachineId_t  machineId;
	zbCounter_t  sequenceNumber;
	zbCounter_t  sourceSeqNumber;
	zbNwkAddr_t  sourceAddress;
	zbEventQueueData_t  data;
}zbEventProcessQueue_t;

/*
	This prototype defines a pointer to a callback function with the answer of the ZDP commands.
*/
typedef void (* ZDPCallBack_t)(zdpToAppMessage_t *, zbCounter_t);

/**********************************************************************
***********************************************************************
* Public Memory Declarations
***********************************************************************
***********************************************************************/
extern bool_t gMgmtNwkDiscReq;
extern bool_t gEndDeviceBindComplete;
extern bool_t gLocalDevInvokedRes;
extern uint8_t giClusterIndex;
extern zbSimpleDescriptor_t gZdpEp;
extern uint8_t gStartIndexforNwkDisc;
extern strAddrClusterId_t strAddrClusterId[MaxArray];
#if gMgmt_Leave_rsp_d
extern zbMgmtOptions_t gMgmtSelfLeaveMask;
#endif
/******************************************************************************
*******************************************************************************
* Public functions
*******************************************************************************
******************************************************************************/

/*-------------------- BeeUtilZeroMemory --------------------
	This function is public.
	
	This function cleans a geiven buffer specified by the parameter pPtr,
	the amount of byte specified by the parameter bufferSize.
	Usign this functions is possible to avoid repeating several times load
	of a function fo rthe same propouse.

	IN: The poointer to the buffer to be cleaned.
	IN: The amount of byte to clean.

	OUT: NONE
*/
void BeeUtilZeroMemory
(
	void  *pPtr,          /* IN: The poointer to the buffer to be cleaned. */
	zbSize_t  bufferSize  /* IN: The amount of byte to clean. */
);

/*-------------------- Zdp_AppRegisterCallBack --------------------
	This function is public.

	This functions registers the callback funtion to pass the ZDP response to the Application,
	the parameters are a pointer to response package (zdpToAppMessage_t  *) and the second one
	is the ZDP sequence number, in this aproach the application always get the sequence number
	that can be used to match the request.

	IN: The function which will receive the response form ZDP commands.

	OUT: NONE
*/
void Zdp_AppRegisterCallBack
(
	ZDPCallBack_t  pZdpAppCallBackPtr  /* IN: The function which will receive the response form ZDP commands. */
);

/*---------------------- Zdo_CnfCallBack ----------------------
  Zdo_CnfCallBack 

  It is called every time the lower layers send a data confirm

	IN: Frame received from NLLE

	OUT: NONE
*/
void Zdo_CnfCallBack(apsdeToAfMessage_t *pMsg);

/*-------------------- Zdo_MsgDataCallBack --------------------
	Zdo_MsgDataCallBack

	Everything that comes over the air ends up here into this function, before
	going into Zdp_ResponseHandler or Zdp_ResponseGenerator. At the end Frees the
	message.

	IN: Frame received from NLLE

	OUT: NONE
*/
void Zdo_MsgDataCallBack
(
	apsdeToAfMessage_t *pMsgIn /*IN: Frame received from NLLE*/
);

/*-------------------- Zdp_RequestGenerator --------------------
	This function is ZDP internal

	Every time that a request cames form the application this function is called.
	The propouse of this function 	is to build the package of the given reuqest 
	and send it to the lower layers.

	IN: The ID of the particular request ot be build, (the 8bit version of the clusterID of the ZDO Command).
	IN: The data whit which the request will be build.
	IN: The address of the device which orginitates the request.
	IN: The address of the device which the package is going to.

	OUT: NONE
*/
void Zdp_RequestGenerator
(
	zbMsgId_t  clusterId,    /* IN: The ID of the particular request ot be build, (the 8bit version of the clusterID of the ZDO Command). */
	void  *pMsg,             /* IN: The data whit which the request will be build. */
	zbNwkAddr_t  aDestAddr   /* IN: The address of the device which orginitates the request. */
);

/*-------------------- Zdp_ResponseGenerator --------------------
	This function is ZDP internal.

	This functions is in charge of generate a package in response of every 
	ZDO request that is received in our node if it is supported, this function 
	handles even the request that came from our node, build the response and then 
	pass it to the lower layers.

	IN: The ID of the request that needs to response (ZDO service), (the 8bit version of the clusterID of the ZDO Command).
	IN: The data that contains the info of the request to be answered.
	IN: The Nwk Address of the node that sends the request to us (incluiding our self).

	OUT: NONE
*/
void Zdp_ResponseGenerator
(
	zbMsgId_t  clusterId,   /* IN: The ID of the particular request ot be build (the 8bit version of the clusterID of the ZDO Command). */
	void  *pMsg,            /* IN: The data whit which the request will be build. */
	zbNwkAddr_t  aSrcAddr   /* IN: The address from where the request is coming from */
);

/*-------------------- Zdp_ResponseHandler --------------------
	This funtion is ZDP internal.

	This function is called every time that a response is received, the response can be
	sended from a diferent node or our self, After processing the information, we send 
	the response back to the Application.

	IN: The ID of the response coming in, form the air or from our own device (the 8bit version of the clusterID of the ZDO Command).
	IN: The data of the response received and which will be sended to the application.
	IN: The size in bytes of the payload coming in to the handler.
	IN: The source address from where the response came.

	OUT: NONE
*/
void Zdp_ResponseHandler
(
	zbMsgId_t  clusterId,     /* IN: The response ID that is coming into our node (the 8bit version of the clusterID of the ZDO Command). */
	void  *pMsg,              /* IN: The data of the response received and which will be sended to the application. */
	zbSize_t  payloadLength,  /* IN: The size in bytes of the payload coming in to the handler. */
	zbNwkAddr_t  aSrcAddr     /* IN: The source address from where the response came. */
);

/*-------------------- Zdp_GenerateDataReq --------------------
	This function is ZDP internal

	This function creates the frame to be passed to the APS layer, for each request and
	response to be send out from ZDO/ZDP.

	IN: The ID for the request or the response to seand out (8 bits)
	IN: The Short addres of the destiantion for the command to be send out.
	IN: The pointer to the whole frame including the payload in which the package will be build.
	IN: The size in byte of the payload (reuqest/response or ZDP command) to be send.

	OUT: NONE
*/
void Zdp_GenerateDataReq
(
	zbMsgId_t  clusterId ,                  /* IN: The ID for the request or the response to seand out (8 bits) */
	zbNwkAddr_t  address,                   /* IN: The Short addres of the destiantion for the command to be send out. */
	afToApsdeMessage_t  *pMessageGoingOut,  /* IN: The pointer to the whole frame including the payload in which the package will be build. */
	zbSize_t  payloadLength                 /* IN: The size in byte of the payload (reuqest/response or ZDP command) to be send. */
);

/*-------------------- Zdp_UpdateBlockAddressMap --------------------
	This function is a Global util function

	Calls the APS_AddTOAddressMap funtion and translates the returning status
	to a ZDO status value, this is handy when an answers depends of the 
	returning status.
	Updates the adress map table.

	IN: The netowork address of the device of interest.
	IN: The IEEE address of the device of interest.

	OUT: A ZDO Status of the APS_AddToAddressMap action.
*/
zbStatus_t Zdp_UpdateBlockAddressMap
(
	zbNwkAddr_t  aNwkAddress,   /* IN: The netowork address of the device of interest. */
	zbIeeeAddr_t  aIeeeAddress  /* IN: The IEEE address of the device of interest. */
);
/******************************************************************************
*  This function handles response frame for management network discovery
*
* Interface assumptions:   
*   													  
*
* Return value: void 
*   
******************************************************************************/
 
 
#if gMgmt_NWK_Disc_rsp_d
void  Zdp_Mgmt_Send_Nwk_disc_conf
(
	nlmeZdoMessage_t *pZdoMsg
);
#else
#define Zdp_Mgmt_Send_Nwk_disc_conf(pZdoMsg)  (void)pZdoMsg;
#endif

#if gMgmt_Leave_rsp_d
/*-------------------- Zdp_Mgmt_Send_leave_conf --------------------
	This fucntion sends the response back to a Mgmt_Leave_Req.

	The function receives a message from the network layer, get the status
	from the confirm, and frees the mesge then allocates another buffer (bigger)
	to send the response back, the buffer needs to be big enough to build in
	all the headers from the lower layers.

	IN: Message received from Nwk Layer.

	OUT: NONE.
*/
void Zdp_Mgmt_Send_leave_conf
(
	nlmeZdoMessage_t *pZdoMsg  /* IN: Message received from Nwk Layer. */
);
#else
#define Zdp_Mgmt_Send_leave_conf(pZdoMsg)  (void)pZdoMsg;
#endif

#if gMatch_Desc_rsp_d || ((gCoordinatorCapability_d || gComboDeviceCapability_d)&& gBindCapability_d && gEnd_Device_Bind_rsp_d)
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
bool_t Zdp_MatchClusters
(
	zbCounter_t  cInClusters,   /* IN: count of input clusters */
	zbCounter_t  cOutClusters,  /* IN: count of output clusters */
	zbClusterId_t  *aInList,    /* IN: input cluster list */
	zbClusterId_t  *aOutList,   /* IN: output cluster list */
	bool_t  fUseArrayForMatch
);
#endif

#if gMgmt_NWK_Disc_rsp_d
extern void NWK_FreeUpDiscoveryTables( void );
#endif

#if gFrequencyAgilityCapability_d && (gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d)
extern void FA_TimeoutHandler(tmrTimerID_t tmrId);
#endif

#endif /* _ZdpManager_H_ */
