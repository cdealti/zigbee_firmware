


/*========================================== REQUESTS ==========================================*/

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

	OUT: The size in bytes of the request payload.
*/
#define NWK_addr_req()  sizeof(zbNwkAddrRequest_t)
#else
#define NWK_addr_req()  gZero_c
#endif

#if gIEEE_addr_req_d
/*-------------------- IEEE_addr_req --------------------
	2.4.3.1.2 IEEE_addr_req. (ClusterID=0x0001)

	The IEEE_addr_req is generated from a Local Device wishing to inquire as to the
	64 bit IEEE address of the Remote Device based on their known 16 bit address.
	The destination addressing on this command shall be unicast.

	OUT: The size in bytes of the request payload.
*/
#define IEEE_addr_req()  sizeof(zbIeeeAddrRequest_t)
#else
#define IEEE_addr_req()  gZero_c
#endif

#if gNode_Desc_req_d
/*-------------------- Node_Desc_req --------------------
	2.4.3.1.3 Node_Desc_req. (ClusterID=0x0002)

	The Node_Desc_req command is generated from a local device wishing to inquire
	as to the node descriptor of a remote device. This command shall be unicast either
	to the remote device itself or to an alternative device that contains the discovery
	information of the remote device.

	OUT: The size in bytes of the request payload.
*/
#define Node_Desc_req()  sizeof(zbNodeDescriptorRequest_t)
#else
#define Node_Desc_req()  gZero_c
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

	OUT: The size in bytes of the request payload.
*/
#define Power_Desc_req()  sizeof(zbPowerDescriptorRequest_t)
#else
#define Power_Desc_req()  gZero_c
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

	OUT: The size in bytes of the request payload.
*/
#define Simple_Desc_req()  sizeof(zbSimpleDescriptorRequest_t)
#else
#define Simple_Desc_req()  gZero_c
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

	OUT: The size in bytes of the request payload.
*/
#define Extended_Simple_Desc_req()  sizeof(zbExtSimpleDescriptorRequest_t)
#else
#define Extended_Simple_Desc_req()  gZero_c
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

	OUT: The size in bytes of the request payload.
*/
#define Active_EP_req()  sizeof(zbActiveEpRequest_t)
#else
#define Active_EP_req()  gZero_c
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
#define Extended_Active_EP_req()  sizeof(zbExtActiveEpRequest_t)
#else
#define Extended_Active_EP_req()  gZero_c
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

	IN: The buffer where the request will be generated.
	IN: The package with the request data.
	IN: The Address of the destination node.

	OUT: The Size of the payload for the request.
*/
zbSize_t Match_Desc_req
(
	uint8_t *pkgPayload,                           /* IN: The buffer where the request will be generated. */
	zbMatchDescriptorRequest_t *pMessageComingIn,  /* IN: The package with the request data. */
	zbNwkAddr_t aDestAddr                          /* IN: The Address of the destination node. */
);
#else
#define Match_Desc_req(pkgPayload, pMessageComingIn, aDestAddr)  gZero_c
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

	OUT: The size in bytes of the request payload.
*/
#define Complex_Desc_req()  sizeof(zbComplexDescriptorRequest_t)
#else
#define Complex_Desc_req()  gZero_c
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

	OUT: The size in bytes of the request payload.
*/
#define User_Desc_req()  sizeof(zbUserDescriptorRequest_t)
#else
#define User_Desc_req()  gZero_c
#endif

#if gDiscovery_Cache_req_d
/*-------------------- Discovery_Cache_req --------------------
	2.4.3.1.10 Discovery_Cache_req. (ClusterID=0x0012)

	The Discovery_Cache_req is provided to enable devices on the network to locate
	a Primary Discovery Cache device on the network. The destination addressing on
	this primitive shall be broadcast to all RxOnWhenIdle devices.

	OUT: The size in bytes of the request payload.
*/
#define Discovery_Cache_req()  sizeof(zbDiscoveryCacheRequest_t)
#else
#define Discovery_Cache_req()  gZero_c
#endif

#if gDevice_annce_d
/*-------------------- Device_annce --------------------
    2.4.3.1.11.1 Device_annce (ClusterID=0x0013)
    The Device_annce is provided to enable ZigBee devices on the network to notify
    other ZigBee devices that the device has joined or re-joined the network,
    identifying the device.s 64-bit IEEE address and new 16-bit NWK address, and
    informing the Remote Devices of the capability of the ZigBee device

	OUT: The size in bytes of the request payload.
*/
#define Device_annce()  sizeof(zbEndDeviceAnnounce_t)
#else
#define Device_annce() gZero_c
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

	OUT: The size in bytes of the request payload.
*/
zbSize_t User_Desc_set
(
	uint8_t  *pkgPayload,                      /* IN: The buffer where the request data will be filled in. */
	zbUserDescriptorSet_t  *pMessageComingIn,  /* IN: The package with the request information. */
	zbNwkAddr_t  aDestAddr                     /* IN: The destination address where the message will be send. */
);
#else
#define User_Desc_set(pkgPayload, pMessageComingIn, aDestAddr)  gZero_c
#endif

#if gSystem_Server_Discovery_req_d
/*-------------------- System_Server_Discovery_req --------------------
	2.4.3.1.13 System_Server_Discovery_req. (ClusterID=0x0015)

	The System_Server_Discovery_req is generated from a Local Device wishing to
	discover the location of a particular system server or servers as indicated by the
	ServerMask parameter. The destination addressing on this request is ‘broadcast to
	all RxOnWhenIdle devices’.

	OUT: The size in bytes of the request payload.
*/
#define System_Server_Discovery_req()  sizeof(zbSystemServerDiscoveryRequest_t)
#else
#define System_Server_Discovery_req()  gZero_c
#endif

#if gDiscovery_store_req_d
/*-------------------- Discovery_store_req --------------------
	2.4.3.1.14 Discovery_store_req. (ClusterID=0x0016)

	The Discovery_store_req is provided to enable ZigBee end devices on the
	network to request storage of their discovery cache information on a Primary
	Discovery Cache device. Included in the request is the amount of storage space
	the Local Device requires.

	IN: The incoming request package.

	OUT: The size in bytes of the outgoing payload.
*/
#define Discovery_store_req(pMessageComingIn)  (MbrOfs(zbDiscoveryStoreRequest_t, simpleDescriptorList[0]) + ((zbDiscoveryStoreRequest_t *)pMessageComingIn)->simpleDescriptorCount)  /* IN: The incoming request package. */
#else
#define Discovery_store_req(pMessageComingIn)  gZero_c
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
);
#else
#define Node_Desc_store_req(pAddress)  gZero_c
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
);
#else
#define Power_Desc_store_req(aNwkAddress)  gZero_c
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
);
#else
#define Active_EP_store_req(aAddress, pMessageGoingOut)  gZero_c
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
);
#else
#define Simple_Desc_store_req(pkgPayload, pMessageComingIn, aDestAddr)  gZero_c
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

	OUT: The size in bytes of the request payload.
*/
#define Remove_node_cache_req()  sizeof(zbRemoveNodeCacheRequest_t)
#else
#define Remove_node_cache_req()  gZero_c
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

	OUT: The size in bytes of the request payload.
*/
#define Find_node_cache_req()  sizeof(zbFindNodeCacheRequest_t)
#else
#define Find_node_cache_req()  gZero_c
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
);
#else
#define End_Device_Bind_req(pkgPayload, pMessageComingIn, aDestAddr)  gZero_c
#endif

#if gBind_req_d
/*-------------------- Bind_req --------------------
	2.4.3.2.2 Bind_req. (ClusterID=0x0021)

	The Bind_req is generated from a Local Device wishing to create a Binding Table
	entry for the source and destination addresses contained as parameters. The
	destination addressing on this command shall be unicast only and the destination
	address shall be that of a Primary binding table cache or to the SrcAddress itself.

	IN: The package with the request information.

	OUT: The size in bytes of the request payload.
*/
#define Bind_req(pMessageComingIn)  Bind_Unbind_req((zbBindUnbindRequest_t *)pMessageComingIn)
#else
#define Bind_req(pMessageComingIn)  gZero_c
#endif

#if gUnbind_req_d
/*-------------------- Unbind_req --------------------
	2.4.3.2.3 Unbind_req. (ClusterID=0x0022)

	The Unbind_req is generated from a Local Device wishing to remove a Binding
	Table entry for the source and destination addresses contained as parameters.
	The destination addressing on this command shall be unicast only and the destination
	address must be that of the a Primary binding table cache or the SrcAddress.

	IN: The package with the request information.

	OUT: The size in bytes of the request payload.
*/
#define Unbind_req(pMessageComingIn)  Bind_Unbind_req((zbBindUnbindRequest_t *)pMessageComingIn)
#else
#define Unbind_req(pMessageComingIn)  gZero_c
#endif

#if gBind_Register_req_d
/*-------------------- Bind_Register_req --------------------
	2.4.3.2.4 Bind_Register_req. (ClusterID=0x0023)

	The Bind_Register_req is generated from a Local Device and sent to a primary
	binding table cache device to register that the local device wishes to hold its own
	binding table entries. The destination addressing mode for this request is unicast.

	OUT: The size in bytes of the request payload.
*/
#define Bind_Register_req()  sizeof(zbBindRegisterRequest_t)
#else
#define Bind_Register_req()  gZero_c
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
#define Replace_Device_req()  sizeof(zbReplaceDeviceRequest_t)
#else
#define Replace_Device_req()  gZero_c
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
);
#else
#define Store_Bkup_Bind_Entry_req(pMessageComingIn)  gZero_c
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
);
#else
#define Remove_Bkup_Bind_Entry_req(pMessageComingIn)  gZero_c
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
);
#else
#define Backup_Bind_Table_req(pMessageComingIn)  gZero_c
#endif

#if gRecover_Bind_Table_req_d
/*-------------------- Recover_Bind_Table_req --------------------
	2.4.3.2.9 Recover_Bind_Table_req. (ClusterID=0x0028)

	The Recover_Bind_Table_req is generated from a local primary binding table
	cache and sent to a remote backup binding table cache device when it wants a
	complete restore of the binding table. The destination addressing mode for this
	request is unicast.

	OUT: The size in bytes of the request payload.
*/
#define Recover_Bind_Table_req()  sizeof(zbRecoverBindTableRequest_t)
#else
#define Recover_Bind_Table_req()  gZero_c
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
);
#else
#define Backup_Source_Bind_req(pMessageComingIn)  gZero_c
#endif

#if gRecover_Source_Bind_req_d
/*-------------------- Recover_Source_Bind_req --------------------
	2.4.3.2.11 Recover_Source_Bind_req. (ClusterID=0x002a)

	The Recover_Source_Bind_req is generated from a local primary binding table
	cache and sent to the remote backup binding table cache device when it wants a
	complete restore of the source bind table. The destination addressing mode for
	this request is unicast.

	OUT: The size in bytes of the request payload.
*/
#define Recover_Source_Bind_req()  sizeof(zbRecoverSourceBindRequest_t)
#else
#define Recover_Source_Bind_req()  gZero_c
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

	OUT: The size in bytes of the request payload.
*/
#define Mgmt_NWK_Disc_req()  sizeof(zbMgmtNwkDiscoveryRequest_t)
#else
#define Mgmt_NWK_Disc_req()  gZero_c
#endif

#if gMgmt_Lqi_req_d
/*-------------------- Mgmt_Lqi_req --------------------
	2.4.3.3.2 Mgmt_Lqi_req. (ClusterID=0x0031)

	The Mgmt_Lqi_req is generated from a Local Device wishing to obtain a
	neighbor list for the Remote Device along with associated LQI values to each
	neighbor. The destination addressing on this command shall be unicast only and
	the destination address must be that of a ZigBee Coordinator or ZigBee Router.

	OUT: The size in bytes of the request payload.
*/
#define Mgmt_Lqi_req()  sizeof(zbMgmtLqiRequest_t)
#else
#define Mgmt_Lqi_req()  gZero_c
#endif

#if gMgmt_Rtg_req_d
/*-------------------- Mgmt_Rtg_req --------------------
	2.4.3.3.3 Mgmt_Rtg_req. (ClusterID=0x0032)

	The Mgmt_Rtg_req is generated from a Local Device wishing to retrieve the
	contents of the Routing Table from the Remote Device. The destination
	addressing on this command shall be unicast only and the destination address
	must be that of the ZigBee Router or ZigBee Coordinator.

	OUT: The size in bytes of the request payload.
*/
#define Mgmt_Rtg_req()  sizeof(zbMgmtRtgRequest_t)
#else
#define Mgmt_Rtg_req()  gZero_c
#endif

#if gMgmt_Bind_req_d
/*-------------------- Mgmt_Bind_req --------------------
	2.4.3.3.4 Mgmt_Bind_req. (ClusterID=0x0033)

	The Mgmt_Bind_req is generated from a Local Device wishing to retrieve the
	contents of the Binding Table from the Remote Device. The destination
	addressing on this command shall be unicast only and the destination address
	must be that of a Primary binding table cache or source device holding its own
	binding table.

	OUT: The size in bytes of the request payload.
*/
#define Mgmt_Bind_req()  sizeof(zbMgmtBindRequest_t)
#else
#define Mgmt_Bind_req()  gZero_c
#endif

#if gMgmt_Leave_req_d
/*-------------------- Mgmt_Leave_req --------------------
	2.4.3.3.5 Mgmt_Leave_req. (ClusterID=0x0034)

	The Mgmt_Leave_req is generated from a Local Device requesting that a Remote
	Device leave the network or to request that another device leave the network. The
	Mgmt_Leave_req is generated by a management application which directs the
	request to a Remote Device where the NLME-LEAVE.request is to be executed
	using the parameter supplied by Mgmt_Leave_req.

	OUT: The size in bytes of the request payload.
*/
#define Mgmt_Leave_req()  sizeof(zbMgmtLeaveRequest_t)
#else
#define Mgmt_Leave_req()  gZero_c
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

  OUT: The size in bytes of the request payload.
*/
#define Mgmt_Direct_Join_req()  sizeof(zbMgmtDirectJoinRequest_t)
#else
#define Mgmt_Direct_Join_req()  gZero_c
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

	OUT: The size in bytes of the request payload.
*/
zbSize_t Mgmt_Permit_Joining_req
(
zbMgmtPermitJoiningRequest_t *pMessageComingIn,  /* IN: The package with the request information. */
zbNwkAddr_t aDestAddr                        /* IN: The destination address where to send the request. */
);
#else
#define Mgmt_Permit_Joining_req()  gZero_c
#endif

#if gMgmt_Cache_req_d
/*-------------------- Mgmt_Cache_req --------------------
	2.4.3.3.8 Mgmt_Cache_req. (ClusterID=0x0037)

	The Mgmt_Cache_req is provided to enable ZigBee devices on the network to
	retrieve a list of ZigBee End Devices registered with a Primary Discovery Cache
	device. The destination addressing on this primitive shall be unicast.

	OUT: The size in bytes of the request payload.
*/
#define Mgmt_Cache_req()  sizeof(zbMgmtCacheRequest_t)
#else
#define Mgmt_Cache_req()  gZero_c
#endif

#if (gMgmt_NWK_Update_req_d && ( gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d))&&(gNetworkManagerCapability_d || gComboDeviceCapability_d)
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
);
#else
#define Mgmt_NWK_Update_req(pRequest,aDestAddr)  gZero_c
#endif

#if (gMgmt_NWK_Update_notify_d && ( gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d))&&(!gNetworkManagerCapability_d || gComboDeviceCapability_d)
/*-------------------- Mgmt_NWK_Update_notify ----------------
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
zbSize_t Mgmt_NWK_Update_notify
(
	zbMgmtNwkUpdateNotify_t  *pRequest
);
#else
#define Mgmt_NWK_Update_notify(pRequest)  gZero_c
#endif
