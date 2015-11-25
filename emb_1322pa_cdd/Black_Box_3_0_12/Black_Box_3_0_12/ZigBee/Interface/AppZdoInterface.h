/*****************************************************************************
* This is a global header file for Application / ZDP Interface
*
* (c) Copyright 2008, Freescale, Inc. All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
******************************************************************************/
#ifndef _APP_ZDO_INTERFACE_H_
#define _APP_ZDO_INTERFACE_H_

#include "AppAfInterface.h"
#include "ZdoNwkInterface.h"
#include "BeeStack_Globals.h"




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
/* None */

/******************************************************************************
*******************************************************************************
* Public type definitions
*******************************************************************************
******************************************************************************/

/******************************
 * Defines for ZDO Tables and sizes
 ******************************/

#ifndef  gMaximumActiveEPStoredList_c  /* This macro helps to store the number of active endpoints for each Discovery requet field, can be adjust or chege to handle dinamic sotrage */
#define  gMaximumActiveEPStoredList_c      3  
#endif

#ifndef  gMaximumSimpleDescriptorList_c  /* This macro helps to store the number of simple descriptors for each Active EP in the Discovery request field, can be adjust or chege to handle dinamic sotrage and there is one or each Active EP */
#define  gMaximumSimpleDescriptorList_c      3 
#endif

#ifndef  gMaximumDiscoveryStoreTableSize_c  /* This macro helps to set the right size for the storing table in the Discovery Cache Device */
#define  gMaximumDiscoveryStoreTableSize_c      2
#endif

#ifndef  gMaximumInClusterStore_c /* updated August 24 2006 */
#define  gMaximumInClusterStore_c      6
#endif

#ifndef  gMaximumOutClusterStore_c /* updated August 24 2006 */
#define  gMaximumOutClusterStore_c      6
#endif

#ifndef  gMaximumDevicesHoldingBindingInfo_c
#define  gMaximumDevicesHoldingBindingInfo_c     3
#endif

#ifndef  gMaximumSystemServerciesResponses_c
#define  gMaximumSystemServerciesResponses_c     6
#endif

#ifndef  gMaximumBackupSourceBindingList_c
#define  gMaximumBackupSourceBindingList_c     2
#endif



/* ZDO Messages IDs in alphabetic order */
enum
{
	gActive_EP_store_req_c          = 0x19,
	gActive_EP_store_rsp_c          = 0x99,

	gActive_EP_req_c                = 0x05,
	gActive_EP_rsp_c                = 0x85,

	gBackup_Bind_Table_req_c        = 0x27,
	gBackup_Bind_Table_rsp_c        = 0xA7,

	gBackup_Source_Bind_req_c       = 0x29,
	gBackup_Source_Bind_rsp_c       = 0xA9,

	gBeeStackSaveReq_c              = 0xE4,

	gBind_Register_req_c            = 0x23,
	gBind_Register_rsp_c            = 0xA3,

	gBind_req_c                     = 0x21,
	gBind_rsp_c                     = 0xA1,

	gComplex_Desc_req_c             = 0x10,
	gComplex_Desc_rsp_c             = 0x90,

	gDiscovery_Cache_req_c          = 0x12,
	gDiscovery_Cache_rsp_c          = 0x92,

	gDiscoveryConf_c                = 0x41,
	gDiscovery_store_req_c          = 0x16,
	gDiscovery_store_rsp_c          = 0x96,
	
	gExtended_Active_EP_req_c       = 0x1E,
	gExtended_Active_EP_rsp_c       = 0x9E,

	gDevice_annce_c                 = 0x13,

	gEnd_Device_Bind_req_c          = 0x20,
	gEnd_Device_Bind_rsp_c          = 0xA0,
	
	gExtended_Simple_Desc_req_c     = 0x1D,
	gExtended_Simple_Desc_rsp_c     = 0x9D,

	gFind_node_cache_req_c          = 0x1C,
	gFind_node_cache_rsp_c          = 0x9C,

	gIEEE_addr_req_c                = 0x01,
	gIEEE_addr_rsp_c                = 0x81,

	gMatch_Desc_req_c               = 0x06,
	gMatch_Desc_rsp_c               = 0x86,

	gMgmt_Bind_req_c                = 0x33,
	gMgmt_Bind_rsp_c                = 0xB3,

	gMgmt_Cache_req_c               = 0x37,
	gMgmt_Cache_rsp_c               = 0xB7,

	gMgmt_Direct_Join_req_c         = 0x35,
	gMgmt_Direct_Join_rsp_c         = 0xB5,

	gMgmt_Leave_req_c               = 0x34,
	gMgmt_Leave_rsp_c               = 0xB4,

	gMgmt_Lqi_req_c                 = 0x31,
	gMgmt_Lqi_rsp_c                 = 0xB1,

	gMgmt_NWK_Disc_req_c            = 0x30,
	gMgmt_NWK_Disc_rsp_c            = 0xB0,

	gMgmt_Permit_Joining_req_c      = 0x36,
	gMgmt_Permit_Joining_rsp_c      = 0xB6,

	gMgmt_Rtg_req_c                 = 0x32,
	gMgmt_Rtg_rsp_c                 = 0xB2,

  /* 053474r17ZB - Sec. 2.4.3.3.9 Mgmt_NWK_Update_req */
  gMgmt_NWK_Update_req_c          = 0x38,

  /* 053474r17ZB - Sec. 2.4.4.3.9 Mgmt_NWK_Update_notify */
  gMgmt_NWK_Update_notify_c       = 0xB8,

	gNode_Desc_req_c                = 0x02,
	gNode_Desc_rsp_c                = 0x82,

	gNode_Desc_store_req_c          = 0x17,
	gNode_Desc_store_rsp_c          = 0x97,

	gNWK_addr_req_c                 = 0x00,
	gNWK_addr_rsp_c                 = 0x80,

	gPermitJoinReq_c                = 0x79,
	gPermitjoinConf_c               = 0xF9,

	gPower_Desc_req_c               = 0x03,
	gPower_Desc_rsp_c               = 0x83,

	gPower_Desc_store_req_c         = 0x18,
	gPower_Desc_store_rsp_c         = 0x98,

	gRecover_Bind_Table_req_c       = 0x28,
	gRecover_Bind_Table_rsp_c       = 0xA8,

	gRecover_Source_Bind_req_c      = 0x2A,
	gRecover_Source_Bind_rsp_c      = 0xAA,

	gRemove_Bkup_Bind_Entry_req_c   = 0x26,
	gRemove_Bkup_Bind_Entry_rsp_c   = 0xA6,

	gRemove_node_cache_req_c        = 0x1B,
	gRemove_node_cache_rsp_c        = 0x9B,

	gReplace_Device_req_c           = 0x24,
	gReplace_Device_rsp_c           = 0xA4,

	gRestartNwkReq_c                = 0x16,

	gSimple_Desc_req_c              = 0x04,
	gSimple_Desc_rsp_c              = 0x84,

	gSimple_Desc_store_req_c        = 0x1A,
	gSimple_Desc_store_rsp_c        = 0x9A,

	gStartNwkReq_c                  = 0x18,

	gStopNwkReq_c                   = 0x1A,

	gStore_Bkup_Bind_Entry_req_c    = 0x25,
	gStore_Bkup_Bind_Entry_rsp_c    = 0xA5,

	gSyncConf_c                     = 0x4C,
	gSyncInd_c                      = 0x4B,

	gSystem_Server_Discovery_req_c  = 0x15,
	gSystem_Server_Discovery_rsp_c  = 0x95,

	gUnbind_req_c                   = 0x22,
	gUnbind_rsp_c                   = 0xA2,

	gUser_Desc_conf_c               = 0x94,
	gUser_Desc_req_c                = 0x11,

	gUserDescResp_c                 = 0x91,
	gUser_Desc_set_c                = 0x14,

	gZdoApsmeAddGroupCnf_c          = 0xEB,
	gZdoApsmeAddGroupReq_c          = 0xE8,

	gZdoApsmeEstKeyConf_c           = 0xD5,
	gZdoApsmeEstKeyInd_c            = 0xD6,
	gZdoApsmeEstKeyReq_c            = 0xCF,
	gZdoApsmeEstKeyResp_c           = 0xD0,

	gZdoApsmeRemoveAllGroupsCnf_c   = 0xED,
	gZdoApsmeRemoveAllGroupsReq_c   = 0xEA,

	gZdoApsmeRemoveDevInd_c         = 0xD8,
	gZdoApsmeRemoveDeviceReq_c      = 0xF4,
	gZdoApsmeRemoveDevCnf_c         = 0xC3,

	gZdoApsmeRemoveGroupCnf_c       = 0xEC,
	gZdoApsmeRemoveGroupReq_c       = 0xE9,

	gZdoApsmeReqKeyInd_c            = 0xD9,
	gZdoApsmeRequestKeyReq_c        = 0xD3,
	gZdoApsmeReqKeyCnf_c            = 0xC2,

	gZdoApsmeSwitchKeyInd_c         = 0xF1,
	gZdoApsmeSwitchKeyReq_c         = 0xD4,
	gZdoApsmeSwitchKeyCnf_c         = 0xC1,

	gZdoApsmeTranspKeyInd_c         = 0xF2,
	gZdoApsmeTranspKeyReq_c         = 0xD1,
	gZdoApsmeTranspKeyCnf_c         = 0xC0,

	gZdoApsmeUpdateDevInd_c         = 0xD7,
	gZdoApsmeUpdateDevReq_c         = 0xF3,
	gZdoApsmeUpdateDevCnf_c         = 0xC4,

	gZdoApsmeAuthenticateReq_c      = 0xF5,
	gZdoApsmeAuthenticateInd_c      = 0xF6,
	gZdoApsmeAuthenticateConf_c     = 0xF7,

	gZdoApsmeEaInitMacData_c        = 0xF8,
	gZdoApsmeEaRspMacData_c         = 0xF9,

	gZdoApsmeRedirectReq_c          = 0xFA,

	gZdoApsmeTunnelReq_c            = 0xFB,
	gZdoApsmeTunnelCnf_c            = 0xC5,

	gZdoNwkDirectJoinReq_c          = 0x39,

	gZdoNwkDiscoveryReq_c           = 0x50,
	gZdoNwkSyncReq_c                = 0x3C,
	gzdo2AppEventInd_c              = 0xE6,
	gZdoApsmeProcessSecureFrame_c   = 0xCC,
/* FA specific Id */
  gChannelMasterReport_c          = 0xFF
};
#ifdef __IAR_SYSTEMS_ICC__
#pragma pack(1)
#endif
/******************
  Application
*******************/
/* FA Specific Structure. */
typedef struct sChannelMasterStatistics_tag
{
  uint8_t reason;
  uint8_t numErrorReports;
  uint8_t numScansRequest;
  uint8_t numScansReceived;
  uint8_t aChannels[16];
}sChannelMasterStatistics_t;

typedef struct routingTableList_tag
{
  /*Destination Address for which the route is discovered*/
  zbNwkAddr_t aDestinationAddress;
  /*Status of Route Discovered*/
  rteProperties_t properties;
  /*Next hope Address to the discovered Destination*/
  zbNwkAddr_t aNextHopAddress;
}routingTableList_t;

typedef struct zbMgmtRtgResponse_tag
{
	zbStatus_t  status;
	zbCounter_t  routingTableEntries;
	index_t  startIndex;
	zbCounter_t  routingTableListCount;
	routingTableList_t    routingTableList[gNwkInfobaseMaxRouteTableEntry_c];
}zbMgmtRtgResponse_t;

/*********************/
typedef struct zbSystemServerDiscoveryStore_tag
{
	zbNwkAddr_t  aNwkAddress;
	zbServerMask_t  aServerMask;
}zbSystemServerDiscoveryStore_t;

/***********************************/
typedef struct cSimpleDescriptor_tag
{
	/*End point ID */
	zbEndPoint_t	endPoint;
	/*Application Profile ID*/
	zbProfileId_t  aAppProfId;
	/*Appliacation Device ID*/
	uint8_t  aAppDeviceId[ 2 ];
	/*Application Device Version And APS Flag*/
	uint8_t  appDevVerAndFlag;
	/*Number of Input Cluster ID Supported by the End Point*/
	zbCounter_t  appNumInClusters;
	/*Place Holder for the list of Input Cluster ID*/
	zbClusterId_t  appInClusterList[9];
	/*Number of Output Cluster ID Supported by the End Point*/
	zbCounter_t  appNumOutClusters;
	/*Place Holder for the list of Output Cluster ID*/
	zbClusterId_t  appOutClusterList[9];
}cSimpleDescriptor_t;

/* This table is used by the Dicovery Cache Device only, is not a request or  a response */
typedef struct discoveryStoreTable_tag  /* mn */
{
	zbNwkAddr_t  aNwkAddress;  /* The network address of the device */
	zbIeeeAddr_t  aIeeeAddress;  /* The extended address of the device */
	zbPowerDescriptor_t  powerDescriptor;  /* The power descriptor of the node requesting the storage */
	zbNodeDescriptor_t  nodeDescriptor;  /* the node descriptor  of the node requesting the storeage */
	zbCounter_t  activeEPCount; /* how many active EP the node has */
	zbEndPoint_t  activeEPList[gMaximumActiveEPStoredList_c]; /* The list of the active EP, this field will be send from the node, the one who ask for storage space */
	zbCounter_t  simpleDescriptorCount; /* the number of simple desciptors, is one simple descriptor for each active endpoint */
	cSimpleDescriptor_t  simpleDescriptorList[gMaximumSimpleDescriptorList_c];  /*Thelist of simpledescriptors comming from the node */
}discoveryStoreTable_t;


/***********************************
* ZigBee 1.1 Spec r09 page 98
************************************/

/* Add a more explicit name into the structure */
typedef struct holdBindingInfo_tag
{
	zbIeeeAddr_t  IeeeAddress;
}holdBindingInfo_t;

/*********************/
/*Security */
/********************/
typedef struct zdoApsmeEstKeyReq_tag
{
	uint8_t  aRespLongAddr[8];
	bool_t  useParent;
	uint8_t  aRespParentLongAddr[8];
	uint8_t  keyEstablishmentMethod;

}zdoApsmeEstKeyReq_t;


typedef struct zdoApsmeEstKeyResp_tag
{
  uint8_t   aInitLongAddr[8];
  bool_t    accept;

}zdoApsmeEstKeyResp_t;

/**********************/

 typedef struct tcMasterKeyMat_tag
{
	uint8_t aTCMasterKey[16];
	uint8_t aDestLongAddr[8];
	uint8_t aParentLongAddr[8];

}tcMasterKeyMat_t;

typedef struct nwkKeyMat_tag{
  uint8_t aNetworkKey[16];
  uint8_t keySeqNum;
	uint8_t aDestLongAddr[8];
	bool_t  useParent;
	uint8_t aParentLongAddr[8];
	bool_t  bSentAfterAuth;
}nwkKeyMat_t;

typedef struct appKeyMat_tag{
	uint8_t aKey[16];
	uint8_t aPartnerLongAddr[8];
	bool_t  initiator;
	uint8_t aDestLongAddr[8];
}appKeyMat_t;


typedef struct zdoApsmeTranspKeyReq_tag{
  uint8_t keyType;
  union{
   	tcMasterKeyMat_t       tcMkMat;
	  nwkKeyMat_t            nkMat;
	  appKeyMat_t            mklkMat;
  }transportKeyData;
}zdoApsmeTranspKeyReq_t;

typedef struct zdoApsmeSwitchKeyReq_tag{
  uint8_t aDestLongAddr[8];
  uint8_t keySeqNumber;
}zdoApsmeSwitchKeyReq_t;

typedef struct zdoApsmeRequestKeyReq_tag{
  uint8_t aDestLongAddr[8];
  uint8_t keyType;
  uint8_t aPartnerLongAddr[8];
}zdoApsmeRequestKeyReq_t;

typedef struct zdoApsmeRemoveDevReq_tag{
  uint8_t aParentLongAddr[8];
  uint8_t aChildLongAddr[8];
}zdoApsmeRemoveDevReq_t;

typedef struct zdoApsmeUpdateDevReq_tag{
  uint8_t aDestLongAddr[8];
  uint8_t aDeviceLongAddr[8];
  uint8_t aDeviceShortAddr[2];
  uint8_t status;
}zdoApsmeUpdateDevReq_t;


typedef struct zdoEstablishKeyInd_tag{
  uint8_t aInitLongAddr[8];
  uint8_t keyEstMethod;
}zdoEstablishKeyInd_t;

typedef struct zdoEstablishKeyConf_tag{
  uint8_t aPartnerLongAddr[8];
  uint8_t status;
}zdoEstablishKeyConf_t;

typedef struct zdoRemoveDeviceInd_tag{
 uint8_t aSrcLongAddr[8];
 uint8_t aChildLongAddr[8];
}zdoRemoveDeviceInd_t;

typedef struct zdoRequestKeyInd_tag{
  uint8_t aSrcLongAddr[8];
  uint8_t keyType;
  uint8_t aPartnerLongAddr[8];
}zdoRequestKeyInd_t;

typedef struct zdoSwitchKeyInd_tag{
  uint8_t aSrcLongAddr[8];
  uint8_t keySeqNum;
}zdoSwitchKeyInd_t;


typedef struct nwkKeyDataInd_tag{
   uint8_t aNetworkKey[16];
   uint8_t keySeqNum;
}nwkKeyDataInd_t;



 typedef struct masterLinkKeyInd_tag{
	uint8_t aKey[16];
	uint8_t aPartnerLongAddr[8];
	bool_t  initiator;
 }masterLinkKeyInd_t;

typedef struct  zdoTransportKeyInd_tag{
  uint8_t aSrcLongAddr[8];
  uint8_t keyType;
  union{
  uint8_t             tcMasterKey[16];
  nwkKeyDataInd_t     nkData;
  masterLinkKeyInd_t  mlKeyData;
  }keyData;
}zdoTransportKeyInd_t;


typedef struct zdoUpdateDeviceInd_tag{
 uint8_t aSrcLongAddr[8];
 uint8_t aDeviceLongAddr[8];
 uint8_t aDeviceShortAddr[2];
 uint8_t status;
}zdoUpdateDeviceInd_t;

/********************************************
NOTE: Few Commands do not have struct defined
********************************************/

/*********************/
/*Network */
/********************/

typedef struct zdoNwkDiscoveryReq_tag
{
	uint8_t		aScanChannels[4];
	uint8_t		scanDuration;

}zdoNwkDiscoveryReq_t;

typedef struct zdoNwkDiscoveryConf_tag
{
	uint8_t		networkCount;
	uint8_t		status;

}zdoNwkDiscoveryConf_t;

/*********************/

typedef struct zdoNwkDirectJoinReq_tag
{
  uint8_t aDeviceAddr[8];
  uint8_t capabilityInfo;

}zdoNwkDirectJoinReq_t ;

typedef struct zdoNwkPermitJoinReq_tag
{
  uint8_t  permitDuration;

}zdoNwkPermitJoinReq_t ;



typedef struct zdoNwkDirectJoinConf_tag
{
  uint8_t aDeviceAddr[8];
  uint8_t status;

}zdoNwkDirectJoinConf_t;


/*********************/

typedef struct zdoNwkSyncReq_tag
{
    bool_t track;

}zdoNwkSyncReq_t;


typedef struct zdoNwkSyncInd_tag
{
   /* NOTE: not according to Spec */
   uint8_t syncLossInd;

}zdoNwkSyncInd_t;


typedef struct ZdoNwkSyncConf_tag
{
	uint8_t		status;

}ZdoNwkSyncConf_t;

typedef struct zdpNlmeSyncReq_tag
{
  bool_t track;
}zdpNlmeSyncReq_t;

typedef struct zdoNlmeNetworkDiscoveryReq_tag
{
  uint8_t	aScanChannels[4];
	uint8_t	scanDuration;
}zdoNlmeNetworkDiscoveryReq_t;

/*********************/

typedef struct appToZdpMessage_tag{

  zbMsgId_t  msgType;
  zbNwkAddr_t  aDestAddr;
  union
  {
    zbIeeeAddrRequest_t               extAddressReq;
    zbNwkAddrRequest_t                nwkAddressReq;
    zbActiveEPStoreRequest_t          activeEPStoreReq;
    zbActiveEpRequest_t               activeEpReq;
    zbBackupBindTableRequest_t        backupBindTableReq;
    zbBackupSourceBindRequest_t       backupSourceBindReq;
    zbBindRegisterRequest_t           bindRegisterReq;
    zbBindUnbindRequest_t             bindReq;
    zbComplexDescriptorRequest_t      complexDescReq;
    zbDiscoveryCacheRequest_t         discoveryCacheReq;
    zbDiscoveryStoreRequest_t         discoveryStoreReq;
    zbExtActiveEpRequest_t            extendedActiveEpReq;
    zbEndDeviceAnnounce_t             endDeviceAnnce;
    zbEndDeviceBindRequest_t          endDeviceBindReq;
    zbExtSimpleDescriptorRequest_t    extendedSimpleDescriptorReq;
    zbFindNodeCacheRequest_t          findNodeCacheReq;
    zbMatchDescriptorRequest_t        matchDescriptorRequest;
    zbMgmtBindRequest_t               mgmtBindReq;
    zbMgmtCacheRequest_t              mgmtCacheReq;
    zbMgmtDirectJoinRequest_t         mgmtDirectJoinReq;
    zbMgmtLeaveRequest_t              mgmtLeaveReq;
    zbMgmtLqiRequest_t                mgmtLqiReq;
    zbMgmtNwkDiscoveryRequest_t       mgmtNwkDiscReq;
    zbMgmtPermitJoiningRequest_t      mgmtPermitJoiningReq;
    zbMgmtRtgRequest_t                mgmtRtgReq;
    zbMgmtNwkUpdateRequest_t          mgmtNwkUpdateRequest;
    zbMgmtNwkUpdateNotify_t           mgmtNwkUpdateNotify;
    zdpNlmeSyncReq_t                  nlmeSyncReq;
    zdoNlmeNetworkDiscoveryReq_t      nlmeNetworkDiscoveryReq;
    zbNodeDescriptorRequest_t         nodeDescriptorReq;
    zbNodeDescriptorStoreRequest_t    nodeDescriptorStoreReq;
    zbPowerDescriptorRequest_t        powDescriptorReq;
    zbPowerDescriptorStoreRequest_t   powerDescriptorStoreReq;
    zbRecoverBindTableRequest_t       recoverBindTableReq;
    zbRecoverSourceBindRequest_t      recoverSourceBindReq;
    zbRemoveBackupBindEntryRequest_t  removeBackupBindingEntryReq;
    zbRemoveNodeCacheRequest_t        removeNodeCacheReq;
    zbReplaceDeviceRequest_t          replaceDeviceReq;
    zbSimpleDescriptorRequest_t       simpleDescriptorReq;
    zbSimpleDescriptorStoreRequest_t  simpleDescriptorStoreReq;
    zbStoreBkupBindEntryRequest_t     storeBackupBindingEntryReq;
    zbSystemServerDiscoveryRequest_t  systemServerDiscReq;
    zbBindUnbindRequest_t             unbindReq;
    zbUserDescriptorRequest_t         userDescReq;
    zbUserDescriptorSet_t             userDescSet;
    zdoApsmeEstKeyReq_t               zdoApsmeEstKeyReq;
    zdoApsmeEstKeyResp_t              establishRes;
    zdoApsmeRemoveDevReq_t            removeReq;
    zdoApsmeRequestKeyReq_t           requestKeyReq;
    zdoApsmeSwitchKeyReq_t            switchKeyReq;
    zdoApsmeTranspKeyReq_t            zdoApsmeTranspKeyReq;
    zdoApsmeUpdateDevReq_t            updateDeviceReq;
    zdoNwkPermitJoinReq_t             permitJoinReq;
    zdoTransportKeyInd_t              transportKeyReq;
  }msgData;
}appToZdpMessage_t;

typedef struct appToNwkMessage_tag{

  zbMsgId_t  msgType;
  union
  {
     zdoNwkDiscoveryReq_t       zdoNwkDiscoveryReq;
     zdoNwkSyncReq_t            zdoNwkSyncReq;
     zdoNwkDirectJoinReq_t      zdoNwkDirectJoinReq;
     zdoNwkPermitJoinReq_t      zdoNwkPermitJoiningReq;
   }msgData;

}appToNwkMessage_t;

typedef uint8_t zdo2AppEvent_t;

typedef struct zdpToAppMessage_tag
{
  zbMsgId_t  msgType;

  union
  {
    zbActiveEPStoreResponse_t          activeEPStoreRsp;
    zbActiveEpResponse_t               activeEpResp;
 #if gBindCapability_d
    zbBackupBindTableResponse_t        backupBindTableResp;
    zbBackupSourceBindResponse_t       backupSourceBindResp;
    zbBindRegisterResponse_t           bindRegisterRsp;
    zbBindUnbindResponse_t             bindResp;
 #endif
    zbComplexDescriptorResponse_t      complexDescResp;
    zbDiscoveryCacheResponse_t         discoveryCacheResp;
    zbDiscoveryStoreResponse_t         discoveryStoreRsp;
    zbExtActiveEpResponse_t            extActiveEpResp;
    zbExtSimpleDescriptorResponse_t    extSimpleDescriptorResp;
    zbEndDeviceBindResponse_t          endDeviceBindResp;
    zbFindNodeCacheResponse_t          findNodeCacheRsp;
    zbMatchDescriptorResponse_t        matchDescriptorResponse;
    zbMgmtBindResponse_t               mgmtBindResp;
    zbMgmtCacheResponse_t              mgmtCacheRsp;
    zbMgmtDirectJoinResponse_t         mgmtDirectJoinResp;
    zbMgmtLeaveResponse_t              mgmtLeaveResp;
    zbMgmtLqiResponse_t                mgmtLqiResp;
    zbMgmtNwkDiscResponse_t            mgmtNwkDiscResp;
    zbMgmtPermitJoiningResponse_t      mgmtPermitJoiningResp;
    zbMgmtRtgResponse_t                mgmtRtgResp;
    zbMgmtNwkUpdateNotify_t            mgmtNwkUpdateNotify;
    nlmeDirectJoinConf_t               directJoinConf;
    nlmeNetworkDiscoveryConf_t         networkDiscoveryConf;
    nlmePermitJoiningConf_t            permitJoiningConf;
    nlmeSyncConf_t                     syncConf;
    nlmeEnergyScanCnf_t                energyScanConf;
    nlmeNwkStatusIndication_t          nlmeNwkStatusIndication;
    nlmeNwkTxReport_t                  nlmeNwkTxReport;
    zbNodeDescriptorResponse_t         nodeDescriptorResp;
    zbNodeDescriptorStoreResponse_t    nodeDescriptorStoreRsp;
    zbPowerDescriptorResponse_t        powerDescriptorResp;
    zbPowerDescriptorStoreResponse_t   powerDescriptorStoreRsp;
    zbRecoverBindTableResponse_t       recoverBindTableResp;
    zbRecoverSourceBindResponse_t      recoverSourceBindTableResp;
    zbRemoveBackupBindEntryResponse_t  removeBackupBindingEntryRsp;
    zbRemoveNodeCacheResponse_t        removeNodeCacheResp;
    zbReplaceDeviceResponse_t          replaceDeviceRsp;
    zbSimpleDescriptorResponse_t       simpleDescriptorResp;
    zbSimpleDescriptorStoreResponse_t  simpleDescriptorStoreRsp;
    zbStoreBkupBindEntryResponse_t     storeBackupBindingEntryRsp;
    zbSystemServerDiscoveryResponse_t  systemServerDiscRsp;
    zbBindUnbindResponse_t             unbindResp;
    zbUserDescriptorConfirm_t          userDescConf;
    zbUserDescriptorResponse_t         userDescResp;
    zbAddressResponse_t                extAddressResp;
    zbAddressResponse_t                nwkAddressResp;
    zdo2AppEvent_t                     zdo2AppEvent;
    zdoApsmeEstKeyResp_t               zdoApsmeEstKeyResp;
    zdoEstablishKeyConf_t              establishConf;
    zdoEstablishKeyInd_t               establishInd;
    zdoNwkSyncInd_t                    syncInd;
    zdoRemoveDeviceInd_t               removeInd;
    zdoRequestKeyInd_t                 requestKeyInd;
    zdoSwitchKeyInd_t                  switchKeyInd;
    zdoTransportKeyInd_t               transportKeyInd;
    zdoUpdateDeviceInd_t               updateDeviceInd;
#if gFrequencyAgilityCapability_d && (gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d)
    sChannelMasterStatistics_t         channelMasterReport;
#endif
  }msgData;
}zdpToAppMessage_t;

#if gDiscovery_Cache_req_d || gNode_Desc_store_req_d || gPower_Desc_store_req_d || gActive_EP_store_req_d || gSimple_Desc_store_req_d
/* ZigBee 1.1 Discovery Chache table, Store the address of Discovery Cache Succcess server */
typedef struct zdpRecordDiscCacheSuccess_tag
{
	zbNwkAddr_t  aNwkAddress;
}zdpRecordDiscCacheSuccess_t;
#endif

#if gFind_node_cache_req_d
typedef struct zdpRecordFindNodeSuccess_tag
{
	zbNwkAddr_t  aCacheNwkAddress;
	zbNwkAddr_t  aNwkAddress;
	zbIeeeAddr_t  aIeeeAddress;
}zdpRecordFindNodeSuccess_t;
#endif
#ifdef __IAR_SYSTEMS_ICC__
#pragma pack()
#endif
/******************************************************************************
*******************************************************************************
* Public memory declarations
*******************************************************************************
******************************************************************************/


typedef uint8_t SystemEvents_t;
/* enumeration for system events (reported to APP and through ZTC) */
enum {

  gZDOToAppMgmtInitDevice_c         = 0x00,
  gZDOToAppMgmtNwkDiscovery_c       = 0x01,
  gZDOToAppMgmtJoinNwk_c            = 0x02, // attempting a join
  gZDOToAppMgmtZCStarting_c         = 0x03, // attempting a form
  gZDOToAppMgmtZRRunning_c          = 0x04,
  gZDOToAppMgmtZEDRunning_c         = 0x05,
  gZDOToAppMgmtLeavingNwk_c         = 0x09,
  gZDOToAppMgmtEstablishKey_c       = 0x0a,
  gZDOToAppMgmtStopped_c            = 0x0b,
  gZDOToAppMgmtOrphan_c             = 0x0c,
  gZDOToAppMgmtZCRunning_c          = 0x10,
  gZDOToAppKeyTransferInitiated_c   = 0x11,
  gZDOToAppAuthenticationStarted_c  = 0x12,
  gZDOToAppMgmtOffTheNetwork_c      = 0x13,
  gZDOToAppMgmtIdle_c               = 0x14
};

/******************************************************************************
*******************************************************************************
* Public functions
*******************************************************************************
******************************************************************************/

uint8_t APP_ZDP_SapHandler
  (
  appToZdpMessage_t *pMsg
  );



/***************************************************************************
* This is the main function of the ZDO layer and this function calls
* function that initializes the configuration attributes and the functions
* that handles the respective ZDP primitives.
* Continuous polling for end device is invoked from this function
* once the timer expiers
* Discovery request is invoked by this function once the timer for
* time between scans expires.
* This function is declared in this file in order to expose this to
* upper layer
*
* Interface assumptions:
*   Initially when the ZDO starts it takes the default values
*   of the configuration primitives from the Nvm to join the
*   Network
*
* Return value:
*   None
*
* Effects on global data:
*   mInitZdo
*   mcDiscoveryRetry
*
******************************************************************************/
void TS_ZdoTask
(
	event_t events
);

/******************************************************************************
* This a entry point in the ZDO State Machine TASK. Here All Events sent to
* the ZDO State Machine task are handled
*
* Interface assumptions:
* None
*
* The routine limitations.
* None
*
* Return value:
* None
*
* Effects on global data.
* None
*
* Source of algorithm used.
* None
*
******************************************************************************/

void TS_ZdoStateMachineTask
  (
  uint16_t events /*IN: Diffrent events Sent to ZDO State Machine*/
  );


/***************************************************************************
* This function initializes the Message queues of Network/Zdo and
* Application/Zdo
*
* Interface assumptions:
*   None
*
* return value:
*   None
*
* Effects on global data:
*
****************************************************************************/
void TS_ZdoTaskInit
  (
  void
  );

#endif _APP_ZDO_INTERFACE_H_
