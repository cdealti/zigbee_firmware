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
#include "BeeStackConfiguration.h"

#include "ZdpManager.h"
#include "ZbAppInterface.h"
#include "ASL_ZdpInterface.h"
#include "BeeStackUtil.h"

#include "ZdpRequests.h"
#include "ZdpResponses.h"
#include "ZdpUtils.h"

#if (gCoordinatorCapability_d || gComboDeviceCapability_d) && gBindCapability_d && gEnd_Device_Bind_rsp_d
#include "ZdoBindManager.h"
#endif

#include "ZDOStateMachineHandler.h"
#include "beeapp.h"
/******************************************************************************
*******************************************************************************
* Private Macros
*******************************************************************************
******************************************************************************/
/*
  The internal counter for each ZDP message, they are mainly neede to keep
  sync of the request and responses.
*/
zbCounter_t  gZdpSequence=0x02;
zbCounter_t  gZdpResponseCounter;

#if gFrequencyAgilityCapability_d && (gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d)
zbApsConfirmId_t gFa_ApsmeConfirmId;
#endif

/**********************************************************************
***********************************************************************
* Private Prototypes
***********************************************************************
***********************************************************************/

extern void *AF_MsgAlloc(void);

/**********************************************************************
***********************************************************************
* Private type definitions
***********************************************************************
***********************************************************************/

/**********************************************************************
***********************************************************************
* Private Memory Declarations
***********************************************************************
***********************************************************************/

bool_t  mbIsNotFree = TRUE;

#if (gCoordinatorCapability_d || gComboDeviceCapability_d)&& gBindCapability_d && gEnd_Device_Bind_rsp_d
extern bool_t gZdoInEndDeviceBind;
#endif

#if gDiscovery_Cache_req_d || gNode_Desc_store_req_d || gPower_Desc_store_req_d || gActive_EP_store_req_d || gSimple_Desc_store_req_d
/* ZigBee 1.1 Discovery Chache table, Store the address of Discovery Cache Succcess server */
zdpRecordDiscCacheSuccess_t  gDiscoveryCacheSuccess = {0};
#endif

#if gDiscovery_Cache_rsp_d || gNode_Desc_store_req_d || gPower_Desc_store_req_d || gActive_EP_store_req_d || gSimple_Desc_store_req_d
bool_t  gDiscoveryCacheResponse = FALSE;
#endif

#if gFind_node_cache_req_d
zdpRecordFindNodeSuccess_t  gFindNodeCacheSuccess = {0};
#endif

bool_t gIsABroadcast = FALSE;

#if gBind_req_d && gBind_Register_rsp_d
extern zbEventProcessQueue_t  maEventProcessQueue[];
#endif

#if gMgmt_Leave_rsp_d
/* used for NLME-LEAVE and ZdpLeave commands for keeping a state */
extern zbMgmtOptions_t gMgmtSelfLeaveMask;
#endif

#if gFrequencyAgilityCapability_d && (gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d)
extern tmrTimerID_t gFA_NotifyTimer;
extern uint8_t gScanCount;
#endif

/**********************************************************************
***********************************************************************
* Public Memory
***********************************************************************
***********************************************************************/
/*
	The pointer used to register the callback function used by the application to get the response form ZDP.
*/
ZDPCallBack_t  gpZdpAppCallBackPtr = NULL;

/* Simple Discriptor for 0x00(ZDP) description*/
zbSimpleDescriptor_t gZdpEp = 
{
	0x00,                        /* End Point (1Byte) */
	0x00,0x00,                   /* Device Description (2Bytes) */
	0x00,0x00,                   /* Profile ID (2Bytes) */
	gAppDeviceVersionAndFlag_c,  /* AppDeviceVersionAndFlag (1Byte) */
	0x00,                        /* NumOfInputClusters(1Byte) */
	NULL,                        /* PointerToInputClusterList (1Byte) */
	0x00,                        /* NumOfOutputClusters (1Byte) */
	NULL                         /* PointerToOutputClusterList (1Byte) */
};

/**********************************************************************
***********************************************************************
* Public Functions
***********************************************************************
***********************************************************************/

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
)
{
	if (pZdpAppCallBackPtr == NULL)
		return;
	
	gpZdpAppCallBackPtr = pZdpAppCallBackPtr;
}

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
)
{
  /*
    This initialization is very important, please do not erase. Is need it to free
    the memory in a default case.
  */
  zbSize_t  payloadLength = 0;
  uint8_t  *pkgPayload;

  /* Memory is freed by the end application */
  zdpToAppMessage_t  *pMsgIn = AF_MsgAlloc();

  if (!pMsgIn)
    return;

  /************************** PLEASE DO NOT REMOVE **************************
    The ZDO/ZDP mechanism relays in the fact that the Memory Buffer is
    cleaned up (Filled with the Value 0x00 for each byte), this function
    call is very important for the stability of ZDP.
    gMaxRxTxDataLength_c is the allocated size of AF_MsgAlloc.*/
  BeeUtilZeroMemory(pMsgIn, gMaxRxTxDataLength_c);
  /************************** PLEASE DO NOT REMOVE **************************/

  pkgPayload = (uint8_t *)((uint8_t *)pMsgIn + SapHandlerDataStructureOffSet);

  switch (clusterId)
  {
/*==============================================================================================
  ==================== 2.4.3.1 Device and Service Discovery Client Services ====================
  ==============================================================================================*/
#if gNWK_addr_req_d
  /*-------------------- NWK_addr_req --------------------
    2.4.3.1.1 NWK_addr_req. (ClusterID=0x0000)

    The NWK_addr_req is generated from a Local Device wishing to inquire as to the
    16 bit address of the Remote Device based on its known IEEE address. The
    destination addressing on this command shall be broadcast to all RxOnWhenIdle
    devices.
    For future upgrade ability, the destination addressing may be permitted to be
    unicast. This would permit directing the message to a well-known destination that
    supports centralized or agent-based device discovery.
  */
    case gNWK_addr_req_c:
      payloadLength = NWK_addr_req();
    break;
#endif

#if gIEEE_addr_req_d
  /*-------------------- IEEE_addr_req --------------------
    2.4.3.1.2 IEEE_addr_req. (ClusterID=0x0001)

    The IEEE_addr_req is generated from a Local Device wishing to inquire as to the
    64 bit IEEE address of the Remote Device based on their known 16 bit address.
    The destination addressing on this command shall be unicast.
  */
    case gIEEE_addr_req_c:
      payloadLength = IEEE_addr_req();
    break;
#endif

#if gNode_Desc_req_d
  /*-------------------- Node_Desc_req --------------------
    2.4.3.1.3 Node_Desc_req. (ClusterID=0x0002)

    The Node_Desc_req command is generated from a local device wishing to inquire
    as to the node descriptor of a remote device. This command shall be unicast either
    to the remote device itself or to an alternative device that contains the discovery
    information of the remote device.
  */
    case gNode_Desc_req_c:
      payloadLength = Node_Desc_req();
    break;
#endif

#if gPower_Desc_req_d
  /*-------------------- Power_Desc_req --------------------
    2.4.3.1.4 Power_Desc_req. (ClusterID=0x0003)

    The Power_Desc_req command is generated from a local device wishing to
    inquire as to the power descriptor of a remote device. This command shall be
    unicast either to the remote device itself or to an alternative device that contains
    the discovery information of the remote device.
    The local device shall generate the Power_Desc_req command using the format
    illustrated in Table 2.44. The NWKAddrOfInterest field shall contain the network
    address of the remote device for which the power descriptor is required.
  */
    case gPower_Desc_req_c:
      payloadLength = Power_Desc_req();
    break;
#endif

#if gSimple_Desc_req_d
	/*-------------------- Simple_Desc_req --------------------
    2.4.3.1.5 Simple_Desc_req. (ClusterID=0x0004)

    The Simple_Desc_req command is generated from a local device wishing to
    inquire as to the simple descriptor of a remote device on a specified endpoint. This
    command shall be unicast either to the remote device itself or to an alternative
    device that contains the discovery information of the remote device.
    The local device shall generate the Simple_Desc_req command using the format
    illustrated in Table 2.45. The NWKAddrOfInterest field shall contain the network
    address of the remote device for which the simple descriptor is required and the
    endpoint field shall contain the endpoint identifier from which to obtain the
    required simple descriptor.
  */
    case gSimple_Desc_req_c:
      payloadLength = Simple_Desc_req();
    break;
#endif

#if gExtended_Simple_Desc_req_d
  /*-------------------- Extended_Simple_Desc_req --------------------
    2.4.3.1.21 Extended_Simple_Desc_req. (ClusterID=0x001d)

    The Extended_Simple_Desc_req command is generated from a local device
    wishing to inquire as to the simple descriptor of a remote device on a specified
    endpoint. This command shall be unicast either to the remote device itself or to an
    alternative device that contains the discovery information of the remote device.
    The Extended_Simple_Desc_req is intended for use with devices which employ a
    larger number of application input or output clusters than can be described by the
    Simple_Desc_req.The NWKAddrOfInterest field shall contain
    the network address of the remote device for which the simple descriptor is
    required and the endpoint field shall contain the endpoint identifier from which to
    obtain the required simple descriptor. The StartIndex is the first entry requested in
    the Application Input Cluster List and Application Output Cluster List sequence
    within the resulting response.
  */
    case gExtended_Simple_Desc_req_c:
      payloadLength = Extended_Simple_Desc_req();
    break;
#endif

#if gActive_EP_req_d
  /*-------------------- Active_EP_req --------------------
    2.4.3.1.6 Active_EP_req. (ClusterID=0x0005)

    The Active_EP_req command is generated from a local device wishing to acquire
    the list of endpoints on a remote device with simple descriptors. This command
    shall be unicast either to the remote device itself or to an alternative device that
    contains the discovery information of the remote device.
    The local device shall generate the Active_EP_req command using the format
    illustrated in Table 2.46. The NWKAddrOfInterest field shall contain the network
    address of the remote device for which the active endpoint list is required.
  */
    case gActive_EP_req_c:
      payloadLength = Active_EP_req();
    break;
#endif

#if gExtended_Active_EP_req_d
  /*-------------------- Extended_Active_EP_req --------------------
    2.4.3.1.22 Extended_Active_EP_req. (ClusterID=0x001E)

    The Extended_Active_EP_req command is generated from a local device wishing
    to acquire the list of endpoints on a remote device with simple descriptors. This
    command shall be unicast either to the remote device itself or to an alternative
    device that contains the discovery information of the remote device. The
    Extended_Active_EP_req is used for devices which support more active
    endpoints than can be returned by a single Active_EP_req.
    The local device shall generate the Extended_Active_EP_req command using the
    format illustrated in Table 2.65. The NWKAddrOfInterest field shall contain the
    network address of the remote device for which the active endpoint list is
    required.
  */
    case gExtended_Active_EP_req_c:
      payloadLength = Extended_Active_EP_req();
    break;
#endif

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
  */
    case gMatch_Desc_req_c:
      payloadLength = Match_Desc_req(pkgPayload, (zbMatchDescriptorRequest_t *)pMsg, aDestAddr);
    break;
#endif

#if gComplex_Desc_req_d
  /*-------------------- Complex_Desc_req --------------------
    2.4.3.1.8 Complex_Desc_req. (ClusterID=0x0010)

    The Complex_Desc_req command is generated from a local device wishing to
    inquire as to the complex descriptor of a remote device. This command shall be
    unicast either to the remote device itself or to an alternative device that contains
    the discovery information of the remote device.
    The local device shall generate the Complex_Desc_req command using the
    format illustrated in Table 2.48. The NWKAddrOfInterest field shall contain the
    network address of the remote device for which the complex descriptor is
    required.
  */
    case gComplex_Desc_req_c:
      payloadLength = Complex_Desc_req();
    break;
#endif

#if gUser_Desc_req_d
  /*-------------------- User_Desc_req --------------------
    2.4.3.1.9 User_Desc_req. (ClusterID=0x0011)

    The User_Desc_req command is generated from a local device wishing to inquire
    the user descriptor of a remote device. This command shall be unicast either
    to the remote device itself or to an alternative device that contains the discovery
    information of the remote device.
    The local device shall generate the User_Desc_req command using the format
    illustrated in Table 2.49. The NWKAddrOfInterest field shall contain the network
    address of the remote device for which the user descriptor is required.
  */
    case gUser_Desc_req_c:
      payloadLength = User_Desc_req();
    break;
#endif

#if gDiscovery_Cache_req_d
  /*-------------------- Discovery_Cache_req --------------------
    2.4.3.1.10 Discovery_Cache_req. (ClusterID=0x0012)

    The Discovery_Cache_req is provided to enable devices on the network to locate
    a Primary Discovery Cache device on the network. The destination addressing on
    this primitive shall be broadcast to all RxOnWhenIdle devices.
  */
    case gDiscovery_Cache_req_c:
      payloadLength = Discovery_Cache_req();
    break;
#endif

#if gDevice_annce_d
  /*-------------------- Device_annce --------------------
    2.4.3.1.11.1 Device_annce (ClusterID=0x0013)
    The Device_annce is provided to enable ZigBee devices on the network to notify
    other ZigBee devices that the device has joined or re-joined the network,
    identifying the device.s 64-bit IEEE address and new 16-bit NWK address, and
    informing the Remote Devices of the capability of the ZigBee device
  */
    case gDevice_annce_c:
      payloadLength = Device_annce();
    break;
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
  */
    case gUser_Desc_set_c:
      payloadLength = User_Desc_set(pkgPayload, (zbUserDescriptorSet_t  *)pMsg, aDestAddr);
    break;
#endif

#if gSystem_Server_Discovery_req_d
  /*-------------------- System_Server_Discovery_req --------------------
    2.4.3.1.13 System_Server_Discovery_req. (ClusterID=0x0015)

    The System_Server_Discovery_req is generated from a Local Device wishing to
    discover the location of a particular system server or servers as indicated by the
    ServerMask parameter. The destination addressing on this request is ‘broadcast to
    all RxOnWhenIdle devices’.
  */
    case gSystem_Server_Discovery_req_c:
      payloadLength = System_Server_Discovery_req();
    break;
#endif

#if gDiscovery_store_req_d
  /*-------------------- Discovery_store_req --------------------
    2.4.3.1.14 Discovery_store_req. (ClusterID=0x0016)

    The Discovery_store_req is provided to enable ZigBee end devices on the
    network to request storage of their discovery cache information on a Primary
    Discovery Cache device. Included in the request is the amount of storage space
    the Local Device requires.
  */
    case gDiscovery_store_req_c:
      payloadLength = Discovery_store_req(pMsg);
    break;
#endif

#if gNode_Desc_store_req_d
  /*-------------------- Node_Desc_store_req --------------------
    2.4.3.1.15 Node_Desc_store_req. (ClusterID=0x0017)

    The Node_Desc_store_req is provided to enable ZigBee end devices on the
    network to request storage of their Node Descriptor on a Primary Discovery
    Cache device which has previously received a SUCCESS status from a
    Discovery_store_req to the same Primary Discovery Cache device. Included in
    this request is the Node Descriptor the Local Device wishes to cache.
  */
    case gNode_Desc_store_req_c:
      payloadLength = Node_Desc_store_req(aDestAddr);
    break;
#endif

#if gPower_Desc_store_req_d
  /*-------------------- Power_Desc_store_req --------------------
    2.4.3.1.16 Power_Desc_store_req. (ClusterID=0x0018)

    The Power_Desc_store_req is provided to enable ZigBee end devices on the
    network to request storage of their Power Descriptor on a Primary Discovery
    Cache device which has previously received a SUCCESS status from a
    Discovery_store_req to the same Primary Discovery Cache device. Included in
    this request is the Power Descriptor the Local Device wishes to cache.
  */
    case gPower_Desc_store_req_c:
      payloadLength = Power_Desc_store_req(aDestAddr);
    break;
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
  */
    case gActive_EP_store_req_c:
      payloadLength = Active_EP_store_req(aDestAddr,(zbActiveEPStoreRequest_t *)pMsg);
    break;
#endif

#if gSimple_Desc_store_req_d
  /*------------------- Simple_Desc_store_req --------------------
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
  */
    case gSimple_Desc_store_req_c:
      payloadLength = Simple_Desc_store_req(pkgPayload, (zbSimpleDescriptorStoreRequest_t *)pMsg, aDestAddr);
    break;
#endif

#if gRemove_node_cache_req_d
  /*-------------------- Remove_node_cache_req --------------------
    2.4.3.1.19 Remove_node_cache_req. (ClusterID=0x001b)

    The Remove_node_cache_req is provided to enable ZigBee devices on the
    network to request removal of discovery cache information for a specified ZigBee
    end device from a Primary Discovery Cache device. The effect of a successful
    Remove_node_cache_req is to undo a previously successful Discovery_store_req
    and additionally remove any cache information stored on behalf of the specified
    ZigBee end device on the Primary Discovery Cache device.
  */
    case gRemove_node_cache_req_c:
      payloadLength = Remove_node_cache_req();
    break;
#endif

#if gFind_node_cache_req_d
  /*-------------------- Find_node_cache_req --------------------
    2.4.3.1.20 Find_node_cache_req. (ClusterID=0x001c)

    The Find_node_cache_req is provided to enable ZigBee devices on the network to
    broadcast to all RxOnWhenIdle devices a request to find a device on the network
    that holds discovery information for the device of interest, as specified in the
    request parameters. The effect of a successful Find_node_cache_req is to have the
    Primary Discovery Cache device, holding discovery information for the device of
    interest, unicast a Find_node_cache_rsp back to the Local Device. Note that, like
    the NWK_addr_req, only the device meeting this criteria shall respond to the
    request generated by Find_node_cache_req.
  */
    case gFind_node_cache_req_c:
      payloadLength = Find_node_cache_req();
    break;
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
  */
    case gEnd_Device_Bind_req_c:
      payloadLength = End_Device_Bind_req(pkgPayload, (zbEndDeviceBindRequest_t *)pMsg, aDestAddr);
    break;
#endif

#if gBind_req_d
  /*-------------------- Bind_req --------------------
    2.4.3.2.2 Bind_req. (ClusterID=0x0021)

    The Bind_req is generated from a Local Device wishing to create a Binding Table
    entry for the source and destination addresses contained as parameters. The
    destination addressing on this command shall be unicast only and the destination
    address shall be that of a Primary binding table cache or to the SrcAddress itself.
  */
    case gBind_req_c:
      payloadLength = Bind_req(pMsg);
    break;
#endif

#if gUnbind_req_d
  /*-------------------- Unbind_req --------------------
    2.4.3.2.3 Unbind_req. (ClusterID=0x0022)

    The Unbind_req is generated from a Local Device wishing to remove a Binding
    Table entry for the source and destination addresses contained as parameters.
    The destination addressing on this command shall be unicast only and the destination
    address must be that of the a Primary binding table cache or the SrcAddress.
  */
    case gUnbind_req_c:
      payloadLength = Unbind_req(pMsg);
    break;
#endif

#if gBind_Register_req_d
  /*-------------------- Bind_Register_req --------------------
    2.4.3.2.4 Bind_Register_req. (ClusterID=0x0023)

    The Bind_Register_req is generated from a Local Device and sent to a primary
    binding table cache device to register that the local device wishes to hold its own
    binding table entries. The destination addressing mode for this request is unicast.
  */
    case gBind_Register_req_c:
      payloadLength = Bind_Register_req();
    break;
#endif

#if gReplace_Device_req_d
  /*-------------------- Replace_Device_req --------------------
    2.4.3.2.5 Replace_Device_req. (ClusterID=0x0024)

    The Replace_Device_req is intended for use by a special device such as a
    Commissioning tool and is sent to a primary binding table cache device to change
    all binding table entries which match OldAddress and OldEndpoint as specified.
    (Note that OldEndpoint = 0 has special meaning and signifies that only the
    address needs to be matched. The endpoint in the binding table will not be
    changed in this case and so NewEndpoint is ignored.) The processing changes all
    binding table entries for which the source address is the same as OldAddress and,
    for which the source endpoint is the same as OldEndpoint. It shall also change
    all binding table entries which have the destination address the same as OldAddress
    and, the destination endpoint the same as OldEndpoint. The destination addressing mode
    for this request is unicast.
  */
    case gReplace_Device_req_c:
      payloadLength = Replace_Device_req();
    break;
#endif

#if gStore_Bkup_Bind_Entry_req_d
  /*-------------------- Store_Bkup_Bind_Entry_req --------------------
    2.4.3.2.6 Store_Bkup_Bind_Entry_req. (ClusterID=0x0025)

    The Store_Bkup_Bind_Entry_req is generated from a local primary binding table
    cache and sent to a remote backup binding table cache device to request backup
    storage of the entry. It will be generated whenever a new binding table entry has
    been created by the primary binding table cache. The destination addressing mode
    for this request is unicast.
  */
    case gStore_Bkup_Bind_Entry_req_c:
      payloadLength = Store_Bkup_Bind_Entry_req((zbStoreBkupBindEntryRequest_t *)pMsg);
    break;
#endif

#if gRemove_Bkup_Bind_Entry_req_d
  /*-------------------- Remove_Bkup_Bind_Entry_req --------------------
    2.4.3.2.7 Remove_Bkup_Bind_Entry_req. (ClusterID=0x0026)

    The Remove_Bkup_Bind_Entry_req is generated from a local primary binding
    table cache and sent to a remote backup binding table cache device to request
    removal of the entry from backup storage. It will be generated whenever a binding
    table entry has been unbound by the primary binding table cache. The destination
    addressing mode for this request is unicast.
  */
    case gRemove_Bkup_Bind_Entry_req_c:
      payloadLength = Remove_Bkup_Bind_Entry_req((zbRemoveBackupBindEntryRequest_t *)pMsg);
    break;
#endif

#if gBackup_Bind_Table_req_d
  /*-------------------- Backup_Bind_Table_req --------------------
    2.4.3.2.8 Backup_Bind_Table_req. (ClusterID=0x0027)

    The Backup_Bind_Table_req is generated from a local primary binding table
    cache and sent to the remote backup binding table cache device to request backup
    storage of its entire binding table. The destination addressing mode for this
    request is unicast.
  */
    case gBackup_Bind_Table_req_c:
      payloadLength = Backup_Bind_Table_req((zbBackupBindTableRequest_t *)pMsg);
    break;
#endif

#if gRecover_Bind_Table_req_d
  /*-------------------- Recover_Bind_Table_req --------------------
    2.4.3.2.9 Recover_Bind_Table_req. (ClusterID=0x0028)

    The Recover_Bind_Table_req is generated from a local primary binding table
    cache and sent to a remote backup binding table cache device when it wants a
    complete restore of the binding table. The destination addressing mode for this
    request is unicast.
  */
    case gRecover_Bind_Table_req_c:
      payloadLength = Recover_Bind_Table_req();
    break;
#endif

#if gBackup_Source_Bind_req_d
  /*-------------------- Backup_Source_Bind_req --------------------
    2.4.3.2.10 Backup_Source_Bind_req. (ClusterID=0x0029)

    The Backup_Source_Bind_req is generated from a local primary binding table
    cache and sent to a remote backup binding table cache device to request backup
    storage of its entire source table. The destination addressing mode for this request
    is unicast.
  */
    case gBackup_Source_Bind_req_c:
      payloadLength = Backup_Source_Bind_req((zbBackupSourceBindRequest_t *)pMsg);
    break;
#endif

#if gRecover_Source_Bind_req_d
  /*-------------------- Recover_Source_Bind_req --------------------
    2.4.3.2.11 Recover_Source_Bind_req. (ClusterID=0x002a)

    The Recover_Source_Bind_req is generated from a local primary binding table
    cache and sent to the remote backup binding table cache device when it wants a
    complete restore of the source bind table. The destination addressing mode for
    this request is unicast.
  */
    case gRecover_Source_Bind_req_c:
      payloadLength = Recover_Source_Bind_req();
    break;
#endif

/*==============================================================================================
  ========================= 2.4.3.3 Network Management Client Services =========================
  ==============================================================================================*/
#if gMgmt_NWK_Disc_req_d
  /*-------------------- Mgmt_NWK_Disc_req --------------------
    2.4.3.3.1 Mgmt_NWK_Disc_req. (ClusterID=0x0030)

    The Mgmt_NWK_Disc_req is generated from a Local Device requesting that the
    Remote Device execute a Scan to report back networks in the vicinity of the Local
    Device. The destination addressing on this command shall be unicast.
  */
    case gMgmt_NWK_Disc_req_c:
      payloadLength = Mgmt_NWK_Disc_req();
    break;
#endif

#if gMgmt_Lqi_req_d
  /*-------------------- Mgmt_Lqi_req --------------------
    2.4.3.3.2 Mgmt_Lqi_req. (ClusterID=0x0031)

    The Mgmt_Lqi_req is generated from a Local Device wishing to obtain a
    neighbor list for the Remote Device along with associated LQI values to each
    neighbor. The destination addressing on this command shall be unicast only and
    the destination address must be that of a ZigBee Coordinator or ZigBee Router.
  */
    case gMgmt_Lqi_req_c:
      payloadLength = Mgmt_Lqi_req();
    break;
#endif

#if gMgmt_Rtg_req_d
  /*-------------------- Mgmt_Rtg_req --------------------
    2.4.3.3.3 Mgmt_Rtg_req. (ClusterID=0x0032)

    The Mgmt_Rtg_req is generated from a Local Device wishing to retrieve the
    contents of the Routing Table from the Remote Device. The destination
    addressing on this command shall be unicast only and the destination address
    must be that of the ZigBee Router or ZigBee Coordinator.
  */
    case gMgmt_Rtg_req_c:
      payloadLength = Mgmt_Rtg_req();
    break;
#endif

#if gMgmt_Bind_req_d
  /*-------------------- Mgmt_Bind_req --------------------
    2.4.3.3.4 Mgmt_Bind_req. (ClusterID=0x0033)

    The Mgmt_Bind_req is generated from a Local Device wishing to retrieve the
    contents of the Binding Table from the Remote Device. The destination
    addressing on this command shall be unicast only and the destination address
    must be that of a Primary binding table cache or source device holding its own
    binding table.
  */
    case gMgmt_Bind_req_c:
      payloadLength = Mgmt_Bind_req();
    break;
#endif

#if gMgmt_Leave_req_d
  /*-------------------- Mgmt_Leave_req --------------------
    2.4.3.3.5 Mgmt_Leave_req. (ClusterID=0x0034)

    The Mgmt_Leave_req is generated from a Local Device requesting that a Remote
    Device leave the network or to request that another device leave the network. The
    Mgmt_Leave_req is generated by a management application which directs the
    request to a Remote Device where the NLME-LEAVE.request is to be executed
    using the parameter supplied by Mgmt_Leave_req.
  */
    case gMgmt_Leave_req_c:
      payloadLength = Mgmt_Leave_req();
    break;
#endif

#if gMgmt_Direct_Join_req_d
  /*-------------------- Mgmt_Direct_Join_req --------------------
    2.4.3.3.6 Mgmt_Direct_Join_req. (ClusterID=0x0035)

    The Mgmt_Direct_Join_req is generated from a Local Device requesting that a
    Remote Device permit a device designated by DeviceAddress to join the network
    directly. The Mgmt_Direct_Join_req is generated by a management application
    which directs the request to a Remote Device where the NLME-DIRECTJOIN.
    request is to be executed using the parameter supplied by
    Mgmt_Direct_Join_req.
  */
    case gMgmt_Direct_Join_req_c:
      payloadLength = Mgmt_Direct_Join_req();
    break;
#endif


#if gMgmt_Permit_Joining_req_d
  /*-------------------- Mgmt_Permit_Joining_req --------------------
    2.4.3.3.7 Mgmt_Permit_Joining_req. (ClusterID=0x0036)

    The Mgmt_Permit_Joining_req is generated from a Local Device requesting that
    a remote device or devices allow or disallow association. The
    Mgmt_Permit_Joining_req is generated by a management application or
    commissioning tool which directs the request to a remote device(s) where the
    NLME-PERMIT-JOINING.request is executed using the PermitDuration
    parameter supplied by Mgmt_Permit_Joining_req. Additionally, if the remote
    device is the Trust Center and TC_Significance is set to 1, the trust center
    authentication policy will be affected. The addressing may be unicast or
    ‘broadcast to all RxOnWhenIdle devices’.
  */
    case gMgmt_Permit_Joining_req_c:
      payloadLength = Mgmt_Permit_Joining_req((void *)pMsg,aDestAddr);
    break;
#endif


#if gMgmt_Cache_req_d
  /*-------------------- Mgmt_Cache_req --------------------
    2.4.3.3.8 Mgmt_Cache_req. (ClusterID=0x0037)

    The Mgmt_Cache_req is provided to enable ZigBee devices on the network to
    retrieve a list of ZigBee End Devices registered with a Primary Discovery Cache
    device. The destination addressing on this primitive shall be unicast.
  */
    case gMgmt_Cache_req_c:
      payloadLength = Mgmt_Cache_req();
    break;
#endif

#if gMgmt_NWK_Update_req_d
  /*-------------------- Mgmt_NWK_Update_req --------------------
    2.4.3.3.9 Mgmt_NWK_Update_req. (ClusterID=0x0038)

    This command is provided to allow updating of network configuration parameters
    or to request information from devices on network conditions in the local
    operating environment. The destination addressing on this primitive shall be
    unicast or broadcast to all devices for which macRxOnWhenIdle = TRUE.
  */
    case gMgmt_NWK_Update_req_c:
      payloadLength = Mgmt_NWK_Update_req((zbMgmtNwkUpdateRequest_t *) pMsg,aDestAddr);
    break;
#endif

#if gMgmt_NWK_Update_notify_d
  /*-------------------- Mgmt_NWK_Update_req --------------------
    2.4.4.3.9 Mgmt_NWK_Update_notify. (ClusterID=0x8038)

    The Mgmt_NWK_Update_notify is provided to enable ZigBee devices to report
    the condition on local channels to a network manager. The scanned channel list is
    the report of channels scanned and it is followed by a list of records, one for each
    channel scanned, each record including one byte of the energy level measured
    during the scan, or 0xff if there is too much interference on this channel.
    When sent in response to a Mgmt_NWK_Update_req command the status field shall 
    represent the status of the request. When sent unsolicited the status field
    shall be set to SUCCESS.
  */
    case gMgmt_NWK_Update_notify_c:
      payloadLength = Mgmt_NWK_Update_notify((zbMgmtNwkUpdateNotify_t *) pMsg);
    break;
#endif

  }

  /*
    Avoid processing if the payload was already handled for any function.
  */
  if (payloadLength == gZdpAlreadyHanlded_c)
    return;

  /*
    They are some request that generate their own address request, before they used to
    respond Zero paylod, making the defualt to be free and some buffer issues with
    broadcast and stuff, to let the lower layers handle the memory, we respond one
    impossible payload size, 0xff.
  */
  /* sends the request to the remote device, The default case */
  if (payloadLength)
  {
    FLib_MemCpy(pkgPayload,pMsg,payloadLength);
    Zdp_GenerateDataReq( clusterId , aDestAddr, (afToApsdeMessage_t *)pMsgIn , payloadLength );
    return;
  }

  /*
    To avoid problems with the request who allocate memory and return zero paylod, if
    that fails, free the memory every time the payload is zero.
  */
  /* can't handle it (defaults to 0 length) */
  MSG_Free(pMsgIn);
}

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
)
{
  /* To send the response to the upper layer */
  zdpToAppMessage_t  *pMsgIn = AF_MsgAlloc();

  /* remove compiler warning */
#if (!gDiscovery_Cache_req_d) && \
    (!gSystem_Server_Discovery_req_d) && \
    (!gRemove_node_cache_req_d)
    (void)aSrcAddr;
#endif
  /* prevent compiler warnings */
#if !((gCoordinatorCapability_d || gComboDeviceCapability_d) && gBindCapability_d && gEnd_Device_Bind_rsp_d)
    (void)payloadLength;
#endif
  if(!pMsgIn)
    return;

  /************************** PLEASE DO NOT REMOVE **************************
    The ZDO/ZDP mechanism relays in the fact that the Memory Buffer is
    cleaned up (Filled with the Value 0x00 for each byte), this function
    call is very important for the stability of ZDP.
    gMaxRxTxDataLength_c is the allocated size of AF_MsgAlloc.*/
  BeeUtilZeroMemory(pMsgIn, gMaxRxTxDataLength_c);
  /************************** PLEASE DO NOT REMOVE **************************/

/* Test code */
  switch (clusterId)
  {
#if gIEEE_addr_req_d || gNWK_addr_req_d
    /* for IEEE address and NWK address response, add to address map */
    case gNWK_addr_rsp_c:
    case gIEEE_addr_rsp_c:
      /* both responses are the same structure */
      if (((zbAddressResponse_t *)pMsg)->status == gZbSuccess_c)
      {
        /* when getting an IEEE/nwk address request, add to address map so we know short/long addr */
        /* NOTE: Need to handle when address map is full */
        (void)Zdp_UpdateBlockAddressMap(
        ((zbAddressResponse_t *)pMsg)->devResponse.singleDevResp.aNwkAddrRemoteDev,
        ((zbAddressResponse_t *)pMsg)->devResponse.singleDevResp.aIeeeAddrRemoteDev);
#if gBindCapability_d && (gBind_rsp_d || gUnbind_rsp_d)
        /*
          The reposne can be for one of the states machines but at this point we do not know anything about any 
          sequence number or previous stored information so we check the current runing states mchines,
          we can send the information as many sttes machines needed.
          This flag also tell us if theres a machjine runing and the type of the machine.
        */
        if (ZDP_GetIndexMachineProcessQueue(gBind_req_c, gZdpResponseCounter) != gZdoNoEntry_c)
        {
          /* This network address request was orginated by the Bind process the application does not need to know about it */
          MSG_Free(pMsgIn);
          ZDP_StateMachine(gBind_req_c,gZdpResponseCounter);
          /* Skip the rest of the function */
          return;
        }
#endif
      }
    break;
#endif

#if gDiscovery_Cache_req_d
    case gDiscovery_Cache_rsp_c:
      if (!gDiscoveryCacheResponse)
      {
        if (((zbDiscoveryCacheResponse_t *)pMsg)->status == gZbSuccess_c)
        {
          Copy2Bytes(gDiscoveryCacheSuccess.aNwkAddress, aSrcAddr);
          gDiscoveryCacheResponse = TRUE;
        }
        else
          gDiscoveryCacheResponse = FALSE;
      }
    break;
#endif

#if gSystem_Server_Discovery_req_d
    case gSystem_Server_Discovery_rsp_c:
      Zdp_UpdateSystemServerDiscoveryResponseTable(aSrcAddr, ((zbSystemServerDiscoveryResponse_t *)pMsg)->aServerMask);
    break;
#endif

#if gFind_node_cache_req_d
    case gFind_node_cache_rsp_c:
      Copy2Bytes(gFindNodeCacheSuccess.aCacheNwkAddress, ((zbFindNodeCacheResponse_t *)pMsg)->aCacheNwkAddr);
      Copy2Bytes(gFindNodeCacheSuccess.aNwkAddress, ((zbFindNodeCacheResponse_t *)pMsg)->aNwkAddrOfInterest);
      Copy8Bytes(gFindNodeCacheSuccess.aIeeeAddress, ((zbFindNodeCacheResponse_t *)pMsg)->aIeeeAddrOfInterest);
    break;
#endif

#if gBind_Register_req_d
    case gBind_Register_rsp_c:
      if (((zbBindRegisterResponse_t *)pMsg)->status == gZbSuccess_c)
      {
        APS_ClearBindingTable();
        (void)Zdp_UpDateBindingTableInfo(((zbBindRegisterResponse_t *)pMsg));
      }
    break;
#endif

#if gBind_req_d
    case gBind_rsp_c:
#if gBind_Register_rsp_d
      if (ZDP_GetIndexMachineProcessQueue(gInformDevices_c, gZdpResponseCounter) != gZdoNoEntry_c)
      {
        /* This network address request was orginated by the Bind process the application does not need to know about it */
        MSG_Free(pMsgIn);
        Zdp_UpdateBindResponseStatus(ZDP_GetIndexMachineProcessQueue(gInformDevices_c, gZdpResponseCounter),
                                    ((zbBindUnbindResponse_t *)pMsg)->status);
        ZDP_StateMachine(gInformDevices_c,gZdpResponseCounter);
        /* Skip the rest of the function */
        return;
      }
#endif
    break;
#endif

#if gUnbind_req_d
    case gUnbind_rsp_c:
#if gUnbind_rsp_d  && gBindCapability_d
      if (ZDP_GetIndexMachineProcessQueue(gReplace_Device_req_c, gZdpResponseCounter) != gZdoNoEntry_c)
      {
        MSG_Free(pMsgIn);
        ZDP_StateMachine(gReplace_Device_req_c,gZdpResponseCounter);
        return;
      }
#endif
    break;
    case gRemove_Bkup_Bind_Entry_rsp_c:
#if gRemove_Bkup_Bind_Entry_req_d
      if (ZDP_GetIndexMachineProcessQueue(gRemove_Bkup_Bind_Entry_req_c, gZdpResponseCounter) != gZdoNoEntry_c)
      {
        MSG_Free(pMsgIn);
        ZDP_StateMachine(gRemove_Bkup_Bind_Entry_req_c,gZdpResponseCounter);
        return;
      }
#endif
    break;
#endif

#if gMgmt_NWK_Update_notify_d
  /*----------------- Mgmt_NWK_Update_notify ------------------
    2.4.4.3.9 Mgmt_NWK_Update_notify. (ClusterID=0x8038)

    The Mgmt_NWK_Update_notify is provided to enable ZigBee devices to report
    the condition on local channels to a network manager. The scanned channel list is
    the report of channels scanned and it is followed by a list of records, one for each
    channel scanned, each record including one byte of the energy level measured
    during the scan, or 0xff if there is too much interference on this channel.
    When sent in response to a Mgmt_NWK_Update_req command the status field
    shall represent the status of the request. When sent unsolicited the status field
    shall be set to SUCCESS.
  */
    case gMgmt_NWK_Update_notify_c:
      /*
        Add code to process this message inside the function. If not, then,
        message will be pass directly to the application.
      */
      FA_Process_Mgmt_NWK_Update_notify((zbMgmtNwkUpdateNotify_t *)pMsg);
    break;
#endif

    default:
    break;
  }

#if (gCoordinatorCapability_d || gComboDeviceCapability_d) && gBindCapability_d && gEnd_Device_Bind_rsp_d

#if gComboDeviceCapability_d
  if ((gZdoInEndDeviceBind && (clusterId != gEnd_Device_Bind_rsp_c)) &&
      (NlmeGetRequest(gDevType_c) == gCoordinator_c))
#else
  if (gZdoInEndDeviceBind && (clusterId != gEnd_Device_Bind_rsp_c))
#endif
  {
    /* free memory as this response is NOT sent to the application */
    MSG_Free(pMsgIn);
    /* check response to see if it worked */
    if(clusterId == gBind_rsp_c || clusterId == gUnbind_rsp_c)
    {
      ZdpEndDeviceBindGotResponse(aSrcAddr, *((zbStatus_t *)pMsg));
    }
  }
  else
#endif
  {
    /* send the message to the application */
    pMsgIn->msgType = clusterId;
    FLib_MemCpy(&pMsgIn->msgData,pMsg,payloadLength);
    (void)ZDP_APP_SapHandler(pMsgIn);
  }
}

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
  zbMsgId_t  clusterId,   /* IN: The ID of the particular request to be build (the 8bit version of the clusterID of the ZDO Command). */
  void  *pMsg,            /* IN: The data whit which the request will be build. */
  zbNwkAddr_t  aSrcAddr   /* IN: The address from where the request is coming from */
)
{
  zbSize_t  payloadLength = 0;
  uint8_t  *pkgPayload;

  /*memory is freed by the end application*/
  zdpToAppMessage_t  *pMsgIn = AF_MsgAlloc();
  if (pMsgIn == NULL)
    return;

  mbIsNotFree = TRUE;

  /************************** PLEASE DO NOT REMOVE **************************
    The ZDO/ZDP mechanism relays in the fact that the Memory Buffer is
    cleaned up (Filled with the Value 0x00 for each byte), this function
    call is very important for the stability of ZDP.
    gMaxRxTxDataLength_c is the allocated size of AF_MsgAlloc. */
  BeeUtilZeroMemory(pMsgIn, gMaxRxTxDataLength_c);
  /************************** PLEASE DO NOT REMOVE **************************/

  pkgPayload = (uint8_t *)((uint8_t *)pMsgIn + SapHandlerDataStructureOffSet);

  switch (clusterId)
  {
/*==============================================================================================
  =============== 2.4.4.1 Device and Service Discovery Server Service Primitives ===============
  ==============================================================================================*/
#if gNWK_addr_rsp_d
  /*-------------------- NWK_addr_rsp --------------------
    2.4.4.1.1 NWK_addr_rsp. (ClusterID=0x8000)

    The NWK_addr_rsp is generated from a Remote Device receiving a broadcast
    NWK_addr_req who detect a match of the IEEEAddr parameter with their own
    IEEE address or the IEEE address held in a local discovery cache. The destination
    addressing on this command is unicast.
  */
    case gNWK_addr_req_c:
      payloadLength = NWK_addr_rsp(pMsg,pkgPayload,clusterId);
    break;
#endif

#if gIEEE_addr_rsp_d
  /*-------------------- IEEE_addr_rsp --------------------
    2.4.4.1.2 IEEE_addr_rsp. (ClusterID=0x8001)

    The IEEE_addr_rsp is generated from Remote Devices in response to the unicast
    IEEE_addr_req inquiring as to the 64 bit IEEE address of the Remote Device. The
    destination addressing on this command shall be unicast.
  */
    case gIEEE_addr_req_c:
      payloadLength = IEEE_addr_rsp(pMsg,pkgPayload,clusterId);
    break;
#endif

#if gNode_Desc_rsp_d
  /*-------------------- Node_Desc_rsp --------------------
    2.4.4.1.3 Node_Desc_rsp. (ClusterID=0x8002)

    The Node_Desc_rsp is generated by a remote device in response to a
    Node_Desc_req directed to the remote device. This command shall be unicast to
    the originator of the Node_Desc_req command.
  */
    case gNode_Desc_req_c:
      payloadLength = Node_Desc_rsp(pMsg, pkgPayload, clusterId);
    break;
#endif

#if gPower_Desc_rsp_d
  /*-------------------- Power_Desc_rsp --------------------
    2.4.4.1.4 Power_Desc_rsp. (ClusterID=0x8003)

    The Power_Desc_rsp is generated by a remote device in response to a
    Power_Desc_req directed to the remote device. This command shall be unicast to
    the originator of the Power_Desc_req command.
  */
    case gPower_Desc_req_c:
      payloadLength = Power_Desc_rsp(pMsg, pkgPayload, clusterId);
    break;
#endif

#if gSimple_Desc_rsp_d
  /*-------------------- Simple_Desc_rsp --------------------
    2.4.4.1.5 Simple_Desc_rsp. (ClusterID=0x8004)

    The Simple_Desc_rsp is generated by a remote device in response to a
    Simple_Desc_req directed to the remote device. This command shall be unicast to
    the originator of the Simple_Desc_req command.
  */
    case gSimple_Desc_req_c:
      payloadLength = Simple_Desc_rsp((zbSimpleDescriptorRequest_t *)pMsg, (zbSimpleDescriptorResponse_t *)pkgPayload);
    break;
#endif

#if gExtended_Simple_Desc_rsp_d
  /*-------------------- Extended_Simple_Desc_rsp --------------------
    2.4.4.1.20 Extended_Simple_Desc_rsp. (ClusterID=0x801D)

    The Extended_Simple_Desc_rsp is generated by a remote device in response to an
    Extended_Simple_Desc_req directed to the remote device. This command shall
    be unicast to the originator of the Extended_Simple_Desc_req command.
  */
    case gExtended_Simple_Desc_req_c:
      payloadLength = Extended_Simple_Desc_rsp((zbExtSimpleDescriptorRequest_t *)pMsg, (zbExtSimpleDescriptorResponse_t *)pkgPayload);
    break;
#endif

#if gActive_EP_rsp_d
  /*-------------------- Active_EP_rsp --------------------
    2.4.4.1.6 Active_EP_rsp. (ClusterID=0x8005)

    The Active_EP_rsp is generated by a remote device in response to an
    Active_EP_req directed to the remote device. This command shall be unicast to
    the originator of the Active_EP_req command.
  */
    case gActive_EP_req_c:
      payloadLength = Active_EP_rsp((zbActiveEpResponse_t*)pkgPayload,((zbActiveEpRequest_t *)pMsg)->aNwkAddrOfInterest);
    break;
#endif

#if gExtended_Active_EP_rsp_d
  /*-------------------- Extended_Active_EP_rsp --------------------
    2.4.4.1.21 Extended_Active_EP_rsp. (ClusterID=0x801E)

    The Extended_Active_EP_rsp is generated by a remote device in response to an
    Extended_Active_EP_req directed to the remote device. This command shall be
    unicast to the originator of the Extended_Active_EP_req command.
  */
    case gExtended_Active_EP_req_c:
      payloadLength = Extended_Active_EP_rsp((zbExtActiveEpResponse_t*)pkgPayload,((zbExtActiveEpRequest_t *)pMsg)->aNwkAddrOfInterest,
                                             ((zbExtActiveEpRequest_t *)pMsg)->startIndex);
    break;
#endif

#if gMatch_Desc_rsp_d
  /*-------------------- Match_Desc_rsp --------------------
    2.4.4.1.7 Match_Desc_rsp. (ClusterID=0x8006)

    The Match_Desc_rsp is generated by a remote device in response to a
    Match_Desc_req either broadcast or directed to the remote device. This command
    shall be unicast to the originator of the Match_Desc_req command.
  */
    case gMatch_Desc_req_c:
      payloadLength = Match_Desc_rsp((zbMatchDescriptorRequest_t *)pMsg, (zbMatchDescriptorResponse_t*)pkgPayload);
    break;
#endif

#if gComplex_Desc_rsp_d
  /*-------------------- Complex_Desc_rsp --------------------
    2.4.4.1.8 Complex_Desc_rsp. (ClusterID=0x8010)

    The Complex_Desc_rsp is generated by a remote device in response to a
    Complex_Desc_req directed to the remote device. This command shall be unicast
    to the originator of the Complex_Desc_req command.
  */
    case gComplex_Desc_req_c:
      payloadLength = Complex_Desc_rsp((zbComplexDescriptorRequest_t *)pMsg,(zbComplexDescriptorResponse_t *)pkgPayload);
    break;
#endif

#if gUser_Desc_rsp_d
  /*-------------------- User_Desc_rsp --------------------
    2.4.4.1.9 User_Desc_rsp. (ClusterID=0x8011)

    The User_Desc_rsp is generated by a remote device in response to a
    User_Desc_req directed to the remote device. This command shall be unicast to
    the originator of the User_Desc_req command.
  */
    case gUser_Desc_req_c:
      payloadLength = User_Desc_rsp(pMsg, pkgPayload, clusterId);
    break;
#endif

#if gDiscovery_Cache_rsp_d || (!gDiscovery_Cache_rsp_d)
  /*-------------------- Discovery_Cache_rsp --------------------
    2.4.4.1.12 Discovery_Cache_rsp. (ClusterID=0x8012)

    The Discovery_Cache_rsp is generated by Primary Discovery Cache devices
    receiving the Discovery_Cache_req. Remote Devices which are not Primary
    Discovery Cache devices (as designated in its Node Descriptor) should not
    respond to the Discovery_Cache_req command.
  */
    case gDiscovery_Cache_req_c:
      payloadLength = Discovery_Cache_rsp();
    break;
#endif

#if gUser_Desc_conf_d
  /*-------------------- User_Desc_conf --------------------
    2.4.4.1.11 User_Desc_conf. (ClusterID=0x8014)

    The User_Desc_conf is generated by a remote device in response to a
    User_Desc_set directed to the remote device. This command shall be unicast to
    the originator of the User_Desc_set command.
  */
    case gUser_Desc_set_c:
      payloadLength = User_Desc_conf((zbUserDescriptorSet_t *)pMsg, (zbUserDescriptorConfirm_t *)pkgPayload);
    break;
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
  */
    case gSystem_Server_Discovery_req_c:
      payloadLength = System_Server_Discovery_rsp((zbSystemServerDiscoveryResponse_t *)pkgPayload,
                                                  ((zbSystemServerDiscoveryRequest_t *)pMsg)->aServerMask);
    break;
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
  */
    case gDiscovery_store_req_c:
      payloadLength = Discovery_store_rsp((zbDiscoveryStoreResponse_t *)pkgPayload,
                                          ((zbDiscoveryStoreRequest_t *)pMsg)->aNwkAddress,
                                          ((zbDiscoveryStoreRequest_t *)pMsg)->aIeeeAddress);
    break;
#endif

#if gNode_Desc_store_rsp_d
  /*-------------------- Node_Desc_store_rsp --------------------
    2.4.4.1.14 Node_Desc_store_rsp. (ClusterID=0x8017)

    The Node_store_rsp is provided to notify a Local Device of the request status
    from a Primary Discovery Cache device. Included in the response is a status code
    to notify the Local Device whether the request is successful (the Primary Cache
    Device has space to store the discovery cache data for the Local Device), whether
    the request is not supported (meaning the Remote Device is not a Primary
    Discovery Cache device) or whether insufficient space exists.
  */
    case gNode_Desc_store_req_c:
      payloadLength = Node_Desc_store_rsp(pkgPayload,
                                          (gNodeDescriptorSize + sizeof(zbServerMask_t)),
                                          (uint8_t *)&((zbNodeDescriptorStoreRequest_t *)pMsg)->nodeDescriptor,
                                          ((zbNodeDescriptorStoreRequest_t *)pMsg)->aNwkAddress);
    break;
#endif

#if gPower_Desc_store_rsp_d
  /*-------------------- Power_Desc_store_rsp --------------------
    2.4.4.1.15 Power_Desc_store_rsp. (ClusterID=0x8018)

    The Power_Desc_store_rsp is provided to notify a Local Device of the request
    status from a Primary Discovery Cache device. Included in the response is a status
    code to notify the Local Device whether the request is successful (the Primary
    Cache Device has space to store the discovery cache data for the Local Device),
    whether the request is not supported (meaning the Remote Device is not a Primary
    Discovery Cache device) or whether insufficient space exists.
  */
    case gPower_Desc_store_req_c:
      payloadLength = Power_Desc_store_rsp(pkgPayload, gPowerDescriptorSize, 
                                          (uint8_t *)&((zbPowerDescriptorStoreRequest_t *)pMsg)->powerDescriptor, 
                                          ((zbPowerDescriptorStoreRequest_t *)pMsg)->aNwkAddress);
    break;
#endif

#if gActive_EP_store_rsp_d
  /*-------------------- Active_EP_store_rsp --------------------
    2.4.4.1.16 Active_EP_store_rsp. (ClusterID=0x8019)

    The Active_EP_store_rsp is provided to notify a Local Device of the request
    status from a Primary Discovery Cache device. Included in the response is a status
    code to notify the Local Device whether the request is successful (the Primary
    Cache Device has space to store the discovery cache data for the Local Device),
    whether the request is not supported (meaning the Remote Device is not a Primary
    Discovery Cache device) or whether insufficient space exists.
  */
    case gActive_EP_store_req_c:
      payloadLength = Active_EP_store_rsp(pkgPayload,
                                          ((zbActiveEPStoreRequest_t *)pMsg)->activeEPCount,
                                          (uint8_t *)((zbActiveEPStoreRequest_t *)pMsg)->activeEPList, 
                                          ((zbActiveEPStoreRequest_t *)pMsg)->aNwkAddress);
    break;
#endif

#if gSimple_Desc_store_rsp_d
  /*-------------------- Simple_Desc_store_rsp --------------------
    2.4.4.1.17 Simple_Desc_store_rsp. (ClusterID=0x801a)

    The Simple_Desc_store_rsp is provided to notify a Local Device of the request
    status from a Primary Discovery Cache device. Included in the response is a status
    code to notify the Local Device whether the request is successful (the Primary
    Cache Device has space to store the discovery cache data for the Local Device),
    whether the request is not supported (meaning the Remote Device is not a Primary
    Discovery Cache device) or whether insufficient space exists.
  */
    case gSimple_Desc_store_req_c:
      payloadLength = MbrOfs(zbSimpleDescriptorStoreRequest_t, simpleDescriptor.inputClusters)+
                      MbrSizeof(zbSimpleDescriptorStoreRequest_t, simpleDescriptor.inputClusters.cNumClusters)+
                      MbrSizeof(zbSimpleDescriptorStoreRequest_t, simpleDescriptor.outputClusters.cNumClusters)+
                      ((zbSimpleDescriptorStoreRequest_t *)pMsg)->simpleDescriptor.inputClusters.cNumClusters * sizeof(zbClusterId_t)+
                      ((zbSimpleDescriptorStoreRequest_t *)pMsg)->simpleDescriptor.outputClusters.cNumClusters * sizeof(zbClusterId_t);
      payloadLength = Simple_Desc_store_rsp(pkgPayload,
                                            payloadLength,
                                            (uint8_t *)&((zbSimpleDescriptorStoreRequest_t *)pMsg)->simpleDescriptor, 
                                            ((zbSimpleDescriptorStoreRequest_t *)pMsg)->aNwkAddress);
    break;
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
  */
    case gRemove_node_cache_req_c:
      payloadLength = Remove_node_cache_rsp(((zbRemoveNodeCacheRequest_t*)pMsg),(zbRemoveNodeCacheResponse_t*)pkgPayload);
    break;
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
  */
    case gFind_node_cache_req_c:
      payloadLength = Find_node_cache_rsp((zbFindNodeCacheResponse_t *)pkgPayload,
                                          ((zbFindNodeCacheRequest_t *)pMsg)->aNwkAddrOfInterest,
                                          ((zbFindNodeCacheRequest_t *)pMsg)->aIeeeAddrOfInterest);
    break;
#endif

/*==============================================================================================
  =========== 2.4.4.2 End Device Bind, Bind, Unbind Bind Management Server Services ============
  ==============================================================================================*/
#if gEnd_Device_Bind_rsp_d && (gCoordinatorCapability_d || gComboDeviceCapability_d) && gBindCapability_d
  /*-------------------- End_Device_Bind_rsp --------------------
    2.4.4.2.1 End_Device_Bind_rsp. (ClusterID=0x8020)

    The End_Device_Bind_rsp is generated by the ZigBee Coordinator in response to
    an End_Device_Bind_req and contains the status of the request. This command
    shall be unicast to each device involved in the bind attempt, using the
    acknowledged data service.
  */
    case gEnd_Device_Bind_req_c:
      payloadLength = End_Device_Bind_rsp(pMsg, (zbStatus_t *)pkgPayload);
    break;
#endif

#if gBind_rsp_d && gBindCapability_d
  /*-------------------- Bind_rsp --------------------
    2.4.4.2.2 Bind_rsp. (ClusterID=0x8021)

    The Bind_rsp is generated in response to a Bind_req. If the Bind_req is processed
    and the Binding Table entry committed on the Remote Device, a Status of
    SUCCESS is returned. If the Remote Device is not the a Primary binding table
    cache or the SrcAddress, a Status of NOT_SUPPORTED is returned. The
    supplied endpoint shall be checked to determine whether it falls within the
    specified range. If it does not, a Status of INVALID_EP shall be returned.11 If the
    Remote Device is the Primary binding table cache or SrcAddress but does not
    have Binding Table resources for the request, a Status of TABLE_FULL is
    returned.
  */
    case gBind_req_c:
      payloadLength = Bind_rsp((zbStatus_t *)pkgPayload,((zbBindUnbindRequest_t *)pMsg),aSrcAddr);
    break;
#endif

#if gUnbind_rsp_d && gBindCapability_d
  /*-------------------- Unbind_rsp --------------------
    2.4.4.2.3 Unbind_rsp. (ClusterID=0x8022)

    The Unbind_rsp is generated in response to an Unbind_req. If the Unbind_req is
    processed and the corresponding Binding Table entry is removed from the Remote
    Device, a Status of SUCCESS is returned. If the Remote Device is not the ZigBee
    Coordinator or the SrcAddress, a Status of NOT_SUPPORTED is returned. The
    supplied endpoint shall be checked to determine whether it falls within the
    specified range. If it does not, a Status of INVALID_EP shall be returned.12 If the
    Remote Device is the ZigBee Coordinator or SrcAddress but does not have a
    Binding Table entry corresponding to the parameters received in the request, a
    Status of NO_ENTRY is returned.
  */
  case gUnbind_req_c:
      payloadLength = Unbind_rsp((zbStatus_t *)pkgPayload,((zbBindUnbindRequest_t *)pMsg),aSrcAddr);
  break;
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
  */
    case gBind_Register_req_c:
      payloadLength = Bind_Register_rsp((zbBindRegisterResponse_t *)pkgPayload,
                                        ((zbBindRegisterRequest_t *)pMsg)->aIeeeAddress,
                                        aSrcAddr);
    break;
#endif

#if gReplace_Device_rsp_d
  /*-------------------- Replace_Device_rsp --------------------
    2.4.4.2.5 Replace_Device_rsp. (ClusterID=0x8024)

    The Replace_Device_rsp is generated from a primary binding table cache device
    in response to a Replace_Device_req and contains the status of the request. This
    command shall be unicast to the requesting device. If the device receiving the
    Replace_Device_req is not a primary binding table cache a Status of
    NOT_SUPPORTED is returned. The primary binding table cache shall search its
    binding table for entries whose source address and source endpoint, or whose
    destination address and destination endpoint match OldAddress and OldEndpoint,
    as described in the text for Replace_Device_req. It shall change these entries to
    have NewAddress and possibly NewEndpoint. It shall then return a response of
    SUCCESS.
  */
    case gReplace_Device_req_c:
      payloadLength = Replace_Device_rsp((zbReplaceDeviceResponse_t *)pkgPayload,
                                        (zbReplaceDeviceRequest_t *)pMsg,
                                        aSrcAddr);
    break;
#endif

#if gStore_Bkup_Bind_Entry_rsp_d
  /*-------------------- Store_Bkup_Bind_Entry_rsp --------------------
    2.4.4.2.6 Store_Bkup_Bind_Entry_rsp. (ClusterID=0x8025)

    The Store_Bkup_Bind_Entry_rsp is generated from a backup binding table cache
    device in response to a Store_Bkup_Bind_Entry_req from a primary binding table
    cache and contains the status of the request. This command shall be unicast to the
    requesting device.
  */
    case gStore_Bkup_Bind_Entry_req_c:
      payloadLength = Store_Bkup_Bind_Entry_rsp((zbStoreBkupBindEntryResponse_t *)pkgPayload,
                                                (zbStoreBkupBindEntryRequest_t *)pMsg,
                                                aSrcAddr);
    break;
#endif

#if gRemove_Bkup_Bind_Entry_rsp_d
  /*-------------------- Remove_Bkup_Bind_Entry_rsp --------------------
    2.4.4.2.7 Remove_Bkup_Bind_Entry_rsp. (ClusterID=0x8026)

    The Remove_Bkup_Bind_Entry_rsp is generated from a backup binding table
    cache device in response to a Remove_Bkup_Bind_Entry_req from the primary
    binding table cache and contains the status of the request. This command shall be
    unicast to the requesting device.
  */
    case gRemove_Bkup_Bind_Entry_req_c:
      payloadLength = Remove_Bkup_Bind_Entry_rsp((zbRemoveBackupBindEntryResponse_t *)pkgPayload,
                                                (zbRemoveBackupBindEntryRequest_t *)pMsg,
                                                aSrcAddr);
    break;
#endif

#if gBackup_Bind_Table_rsp_d
  /*-------------------- Backup_Bind_Table_rsp --------------------
    2.4.4.2.8 Backup_Bind_Table_rsp. (ClusterID=0x8027)

    The Backup_Bind_Table_rsp is generated from a backup binding table cache
    device in response to a Backup_Bind_Table_req from a primary binding table
    cache and contains the status of the request. This command shall be unicast to the
    requesting device.
  */
    case gBackup_Bind_Table_req_c:
      payloadLength = Backup_Bind_Table_rsp((zbBackupBindTableRequest_t*)pMsg,
                                            (zbBackupBindTableResponse_t*)pkgPayload,
                                             aSrcAddr);
    break;
#endif

#if gRecover_Bind_Table_rsp_d
  /*-------------------- Recover_Bind_Table_rsp --------------------
    2.4.4.2.9 Recover_Bind_Table_rsp. (ClusterID=0x8028)

    The Recover_Bind_Table_rsp is generated from a backup binding table cache
    device in response to a Recover_Bind_Table_req from a primary binding table
    cache and contains the status of the request. This command shall be unicast to the
    requesting device.
  */
    case gRecover_Bind_Table_req_c:
      payloadLength = Recover_Bind_Table_rsp((zbRecoverBindTableRequest_t *)pMsg,
                                             (zbRecoverBindTableResponse_t *)pkgPayload,
                                             aSrcAddr);
    break;
#endif

#if gBackup_Source_Bind_rsp_d
  /*-------------------- Backup_Source_Bind_rsp --------------------
    2.4.4.2.10 Backup_Source_Bind_rsp. (ClusterID=0x8029)

    The Backup_Source_Bind_rsp is generated from a backup binding table cache
    device in response to a Backup_Source_Bind_req from a primary binding table
    cache and contains the status of the request. This command shall be unicast to the
    requesting device.
  */
    case gBackup_Source_Bind_req_c:
      payloadLength = Backup_Source_Bind_rsp(((zbBackupSourceBindRequest_t*)pMsg),
                                            (zbBackupSourceBindResponse_t*)pkgPayload,
                                            aSrcAddr);
    break;
#endif

#if gRecover_Source_Bind_rsp_d
  /*-------------------- Recover_Source_Bind_rsp --------------------
    2.4.4.2.11 Recover_Source_Bind_rsp. (ClusterID=0x802a)

    The Recover_Source_Bind_rsp is generated from a backup binding table cache
    device in response to a Recover_Source_Bind_req from a primary binding table
    cache and contains the status of the request. This command shall be unicast to the
    requesting device.
  */
    case gRecover_Source_Bind_req_c:
      payloadLength = Recover_Source_Bind_rsp((zbRecoverSourceBindRequest_t *)pMsg,
                                              (zbRecoverSourceBindResponse_t *)pkgPayload,
                                              aSrcAddr);
    break;
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
  */
    case gMgmt_NWK_Disc_req_c:
      payloadLength = Mgmt_NWK_Disc_rsp((zbMgmtNwkDiscoveryRequest_t *)pMsg,(zdoNlmeMessage_t *)pMsgIn,aSrcAddr);
    break;
#endif

#if gMgmt_Lqi_rsp_d && ( gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d)
  /*-------------------- Mgmt_Lqi_rsp --------------------
    2.4.4.3.2 Mgmt_Lqi_rsp. (ClusterID=0x8031)

    The Mgmt_Lqi_rsp is generated in response to an Mgmt_Lqi_req. If this
    management command is not supported, a status of NOT_SUPPORTED shall be
    returned and all parameter fields after the Status field shall be omitted.
  */
    case gMgmt_Lqi_req_c:
      payloadLength = Mgmt_Lqi_rsp((zbMgmtLqiRequest_t *)pMsg, (zbMgmtLqiResponse_t *)pkgPayload);
    break;
#endif

#if gMgmt_Rtg_rsp_d
#if gRnplusCapability_d && (gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d)
  /*-------------------- Mgmt_Rtg_rsp --------------------
    2.4.4.3.3 Mgmt_Rtg_rsp. (ClusterID=0x8032)

    The Mgmt_Rtg_rsp is generated in response to an Mgmt_Rtg_req. If this
    management command is not supported, a status of NOT_SUPPORTED shall be
    returned and all parameter fields after the Status field shall be omitted.
  */
    case gMgmt_Rtg_req_c:
        payloadLength = Mgmt_Rtg_rsp((zbMgmtRtgRequest_t *)pMsg, (zbMgmtRtgResponse_t *)pkgPayload);
    break;
#endif
#endif

#if gMgmt_Bind_rsp_d
  /*-------------------- Mgmt_Bind_rsp --------------------
    2.4.4.3.4 Mgmt_Bind_rsp. (ClusterID=0x8033)

    The Mgmt_Bind_rsp is generated in response to a Mgmt_Bind_req. If this
    management command is not supported, a status of NOT_SUPPORTED shall be
    returned and all parameter fields after the Status field shall be omitted.
  */
    case gMgmt_Bind_req_c:
      payloadLength = Mgmt_Bind_rsp((zbMgmtBindRequest_t  *)pMsg,(zbMgmtBindResponse_t  *)pkgPayload);
    break;
#endif

#if gMgmt_Leave_rsp_d
  /*-------------------- Mgmt_Leave_rsp --------------------
    2.4.4.3.5 Mgmt_Leave_rsp. (ClusterID=0x8034)

    The Mgmt_Leave_rsp is generated in response to a Mgmt_Leave_req. If this
    management command is not supported, a status of NOT_SUPPORTED shall be
    returned.
  */
  case gMgmt_Leave_req_c:
    payloadLength = Mgmt_Leave_rsp((zbMgmtLeaveRequest_t  *)pMsg, (zbMgmtLeaveResponse_t *)pkgPayload, aSrcAddr);
  break;
#endif

#if gMgmt_Direct_Join_rsp_d && ( gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d )
  /*-------------------- Mgmt_Direct_Join_rsp --------------------
    2.4.4.3.6 Mgmt_Direct_Join_rsp. (ClusterID=0x8035)

    The Mgmt_Direct_Join_rsp is generated in response to a Mgmt_Direct_Join_req.
    If this management command is not supported, a status of NOT_SUPPORTED
    shall be returned.
  */
  case gMgmt_Direct_Join_req_c:
    payloadLength = Mgmt_Direct_Join_rsp(pMsg, aSrcAddr);
  break;
#endif

#if gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d
  /*-------------------- Mgmt_Permit_Joining_rsp --------------------
    2.4.4.3.7 Mgmt_Permit_Joining_rsp. (ClusterID=0x8036)

    The Mgmt_Permit_Joining_rsp is generated in response to a unicast
    Mgmt_Permit_Joining_req. In the description which follows, note that no
    response shall be sent if the Mgmt_Permit_Joining_req was received as a
    broadcast to all routers.
  */
    case gMgmt_Permit_Joining_req_c:
        payloadLength = Mgmt_Permit_Joining_rsp((zbMgmtPermitJoiningRequest_t *)pMsg,
                                               (zbMgmtPermitJoiningResponse_t *)pkgPayload, aSrcAddr);
    break;
#endif

#if gMgmt_Cache_rsp_d
  /*-------------------- Mgmt_Cache_rsp --------------------
    2.4.4.3.8 Mgmt_Cache_rsp. (ClusterID=0x8037)

    The Mgmt_Cache_rsp is generated in response to an Mgmt_Cache_req. If this
    management command is not Supported or the Remote Device is not a Primary
    Cache Device, a status of NOT_SUPPORTED shall be returned and all parameter
    fields after the Status field shall be omitted.
  */
    case gMgmt_Cache_req_c:
      payloadLength = Mgmt_Cache_rsp((zbMgmtCacheResponse_t *)pkgPayload,
                                    ((zbMgmtCacheRequest_t *)pMsg)->startIndex);
    break;
#endif

#if (gMgmt_NWK_Update_notify_d || (gEndDevCapability_d || gComboDeviceCapability_d) && gFrequencyAgilityCapability_d))
  /*----------------- Mgmt_NWK_Update_notify ------------------
    2.4.4.3.9 Mgmt_NWK_Update_notify. (ClusterID=0x8038)

    The Mgmt_NWK_Update_notify is provided to enable ZigBee devices to report
    the condition on local channels to a network manager. The scanned channel list is
    the report of channels scanned and it is followed by a list of records, one for each
    channel scanned, each record including one byte of the energy level measured
    during the scan, or 0xff if there is too much interference on this channel.
    When sent in response to a Mgmt_NWK_Update_req command the status field
    shall represent the status of the request. When sent unsolicited the status field
    shall be set to SUCCESS.
  */
    case gMgmt_NWK_Update_req_c:
      payloadLength = FA_Process_Mgmt_NWK_Update_request((zbMgmtNwkUpdateRequest_t*)pMsg,
                                                          pMsgIn,
                                                          aSrcAddr);
    break;
#endif

  /*-------------------- Device_Annce --------------------
    2.4.3.1.11.1 Device_annce (ClusterID=0x0013)
    The Device_annce is provided to enable ZigBee devices on the network to notify
    other ZigBee devices that the device has joined or re-joined the network,
    identifying the device.s 64-bit IEEE address and new 16-bit NWK address, and
    informing the Remote Devices of the capability of the ZigBee device
  */
    case gDevice_annce_c:
      payloadLength = Device_Annce((zbEndDeviceAnnounce_t *)pMsg, aSrcAddr);
    break;

    /* If the command is no supported under the current configuration then, return a Not supported status */
    default:
      /* only status is sent to the remote device in response */
      payloadLength = sizeof(zbStatus_t);
      pkgPayload[0] = gZdoNotSupported_c;
    break;
  }
  /* If there is no payload to send over the air, drop the package and free the memory */
  if (!payloadLength)
  {
    /*
      this part of code is generalized and comes here when response frame is not created
      for a particular request. If the memory has not being free befor free it.
    */
    if (mbIsNotFree)
      MSG_Free( pMsgIn );

    /* Set as not freed the memory */
    mbIsNotFree = TRUE;
    return;  /* Avoid sending anything over the air */
  }
  /* The default case is send the package over the air calling a wraper function to do the job */
  Zdp_GenerateDataReq( clusterId | ResponseClusterId , aSrcAddr, (afToApsdeMessage_t *)pMsgIn, payloadLength );
}

/*---------------------- Zdo_CnfCallBack ----------------------
  Zdo_CnfCallBack 

  It is called every time the lower layers send a data confirm

  IN: Frame received from NLLE

  OUT: NONE
*/
void Zdo_CnfCallBack(apsdeToAfMessage_t *pMsg)
{

#if gMgmt_Leave_rsp_d
  /*
    If the management leave request was requesting us to leave and we already
    sent the management leave response then leave.
  */
  if (gMgmtSelfLeaveMask & zbMgmtOptionSelfLeave_c)
  {
    /*
      If the mask was active then that mean that the local node is the one leving the
      mask does not need to beactive any more.
    */
    gMgmtSelfLeaveMask &= ~(zbMgmtOptionSelfLeave_c);

    /*
      This command announce the leaving by defautl. And is on the announcing where the
      remove children and the rejoin gets process.
    */
    ZDO_StopEx(gZdoStopMode_Announce_c);
  }
#endif

  /* If the notify fails, we need to try it 15 smitues later until success. */
#if gFrequencyAgilityCapability_d && (gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d)
#if gComboDeviceCapability_d
  if( NlmeGetRequest(gDevType_c) != gEndDevice_c)
#endif
  {
    if ((gFa_ApsmeConfirmId) && (pMsg->msgData.dataConfirm.confirmId == gFa_ApsmeConfirmId))
    {
      if (pMsg->msgData.dataConfirm.status == gZbSuccess_c)
      {
          /* If the status is success then resest the counters and values.*/
          gFa_ApsmeConfirmId = 0;
          if ((!ZdoGetNwkManagerBitMask(gaServerMask))&& (!gScanCount)) //((!gScanCount) || (gScanCount!=0xff)))
          {
            ResetTxCounters();
            TMR_StartMinuteTimer(gFA_NotifyTimer, gTimeBeforeNextNwkUpdateNotify_c, FA_TimeoutHandler);
          }
      }
      else
      {
        /* 
          If the message was failure or success and the scancount is 0 then start
          the timer, so we do not sent another NwkUpdateNotify till this time has 
          expire, to avoid reporting to often.
        */
        TMR_StartMinuteTimer(gFA_NotifyTimer, gTimeBeforeNextNwkUpdateNotify_c, FA_TimeoutHandler);
      }
    }
  }
#endif

  MSG_Free(pMsg);
}

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
)
{
  zbApsdeDataIndication_t *pIndication;
  uint8_t  clusterId;

  /* pointer to data indication */
  pIndication = &(pMsgIn->msgData.dataIndication);

  gIsABroadcast = pIndication->fWasBroadcast;

  /* convert from 16-bit cluster to 8-bit message ID */
  clusterId = pIndication->aClusterId[0];
  if(pIndication->aClusterId[1])
    clusterId |= ResponseClusterId;

  /* ZDO counter is first byte of payload */
  gZdpResponseCounter = pIndication->pAsdu[0];

  /* Ignore ZdoTransactionNumber for now, that's why data pointer incremented by the size of it */
  /* Ask if the Id of the message coming over the air is a response */

  if (clusterId & ResponseClusterId)
  {
    /* Send the response to take cre of it and send the information back to the application */
    Zdp_ResponseHandler(clusterId, pIndication->pAsdu + sizeof(gZdpResponseCounter),
                       (pIndication->asduLength - sizeof(gZdpResponseCounter)),pIndication->aSrcAddr);
  }
  else
  {
    if(clusterId == gDevice_annce_c)
    {
      /* To send the response to the upper layer*/
      zdpToAppMessage_t  *pMsg = AF_MsgAlloc();
      BeeUtilZeroMemory(pMsg, gMaxRxTxDataLength_c);
      /* send the message to the application*/
      pMsg->msgType = clusterId;
      FLib_MemCpy(&pMsg->msgData,pIndication->pAsdu + sizeof(gZdpResponseCounter),pIndication->asduLength);
      (void)ZDP_APP_SapHandler(pMsg);
    }
    /* A request has come over the air we need to generate a response if it is supported */
    Zdp_ResponseGenerator(clusterId, pIndication->pAsdu + sizeof(gZdpResponseCounter), pIndication->aSrcAddr);
  }

  /* Free msg after processing */
  MSG_Free(pMsgIn);
}

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
)
{
  pMessageGoingOut->msgType = gApsdeDataReqMsgType_d;
  pMessageGoingOut->msgData.dataReq.dstAddrMode = gZbAddrMode16Bit_c;
  Copy2Bytes(pMessageGoingOut->msgData.dataReq.dstAddr.aNwkAddr, address);

  /*
    - Simple Discriptor is 0x00 for (ZDP) description, pMessageGoingOut->msgData.dataReq.dstEndPoint = 0x00.
    - Copy profile ID is 0x0000 for Endpoint 0x00 (ZDO), Set2Bytes(pMessageGoingOut->msgData.dataReq.aProfileId, 0x0000).
    - This is an Endpoint-to-Endpoint communication so it is ZDO-to-ZDO communication,
      the src Endpoint must be Zero: pMessageGoingOut->msgData.dataReq.srcEndPoint = 0x00.
  */

  pMessageGoingOut->msgData.dataReq.aClusterId[0] = clusterId & 0x7f; /* lower byte */
  pMessageGoingOut->msgData.dataReq.aClusterId[1] = clusterId & ResponseClusterId; /* upper bit set on response */

  /* Added ZdoTransactionSequenceNumber for now, that's why data pointer incremented by 0, section 2.4.2.8 Figure2.16*/
  pMessageGoingOut->msgData.dataReq.asduLength = payloadLength + sizeof(gZdpSequence);
  pMessageGoingOut->msgData.dataReq.pAsdu = ((uint8_t *)pMessageGoingOut + (SapHandlerDataStructureOffSet-sizeof(gZdpSequence)));

  /* If it is not a response increase the transaction ID (aka. ZDP sequence number) */
  if (!(clusterId & ResponseClusterId))
  {
    pMessageGoingOut->msgData.dataReq.pAsdu[0] = gZdpSequence++;
  }
  else
  {
    /* if it is a response use the same sequence number which it came in */
    pMessageGoingOut->msgData.dataReq.pAsdu[0] = gZdpResponseCounter;
  }

  pMessageGoingOut->msgData.dataReq.txOptions = zdoTxOptionsDefault_c;

  /*
    FA, have special needs for its code. If the Update request is unicast it should
    request for an ASP Ack.
  */
  if ((clusterId == gMgmt_NWK_Update_req_c) && IsValidNwkUnicastAddr(address))
    pMessageGoingOut->msgData.dataReq.txOptions |= gApsTxOptionAckTx_c;

  /*
    Set the ACK bit for all the responses, it does not hurts anything and is on for the
    ones that need it.
  */
  if ((clusterId & ResponseClusterId) && gIsABroadcast)
    pMessageGoingOut->msgData.dataReq.txOptions |= gApsTxOptionAckTx_c;

  /*
    If the node is reporting an error condition it may need to send the notify many times,
    depending if the confirm was success full or not.
  */
  if (clusterId == gMgmt_NWK_Update_notify_c)
  {
    pMessageGoingOut->msgData.dataReq.txOptions |= gApsTxOptionAckTx_c;

#if gFrequencyAgilityCapability_d && (gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d)
  /* If frequency agility is enable then save the ApsmeConfirmId to check on the apsack */
  gFa_ApsmeConfirmId = ApsGetConfirmId();
#endif
  }

  pMessageGoingOut->msgData.dataReq.radiusCounter = gDefaultRadiusCounter;

  /* Each packet going through the AF_APSDE_SapHandler must have a valid APS Confirm ID. */
  pMessageGoingOut->msgData.dataReq.confirmId = ApsGetConfirmId();

  APS_IncConfirmId();

  /*
    Clear the broadcast global, make suer the value is unique per message. The use of this
    function is attomic and does not depends on events so, it must be safe to clear it here.
  */
  gIsABroadcast = FALSE;

  /* call on APSDE to send the ZDO command or response */
  (void)AF_APSDE_SapHandler(pMessageGoingOut);
}

/******************************************************************************
*******************************************************************************
* Private functions
*******************************************************************************
******************************************************************************/

/**********************************************************************
***********************************************************************
* Private Debug Stuff
***********************************************************************
***********************************************************************/

/* None */
