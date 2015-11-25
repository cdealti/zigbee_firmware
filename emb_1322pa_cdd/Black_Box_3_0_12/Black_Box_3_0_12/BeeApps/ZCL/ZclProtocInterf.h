/******************************************************************************
* ZclProtocInterf.c
*
* This module contains code that handles ZigBee Cluster Library commands and
* clusters for Protocol Interface Clusters
*
* Copyright (c) 2009, Freescale, Inc.  All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
* Documents used in this specification:
* [R1] - 075360r07ZB_AFG-PHHC-Profile.pdf
* [R2] - 075123r00ZB_AFG-ZigBee_Cluster_Library_Specification.pdf
*******************************************************************************/
#ifndef _ZCL_PROTOCINTERF_H
#define _ZCL_PROTOCINTERF_H

#include "ZclOptions.h"
#include "AfApsInterface.h"
#include "AppAfInterface.h"
#include "BeeStackInterface.h"
#include "EmbeddedTypes.h"
#include "ZCL.h"
#include "BeeApp.h"
#include "EndPointConfig.h"
//#include "Oep11073.h"


/******************************************************************************
*******************************************************************************
* Public macros and data types definitions.
*******************************************************************************
******************************************************************************/


/*************************************************************
  Cluster definitions
**************************************************************/

#define gZclGtMaxProtocolAddress      9
#define gZcl11073MaxAPDU              50


#if ( TRUE == gBigEndian_c )
  #define gZclClusterGeneralTunnel_c   0x0600
  #define gZclCluster11073Tunnel_c     0x1406
#else
  #define gZclClusterGeneralTunnel_c   0x0006
  #define gZclCluster11073Tunnel_c     0x0614
#endif




/*************************************************************
	Generic Tunnel Cluster (ZigBee Cluster Library Chapter 9.2)
**************************************************************/

#if ( TRUE == gBigEndian_c )
#define gZclAttrGTMaxInTrsSize_c   0x0100    /* M - Maximum IN transfer Size attributes Set */
#define gZclAttrGTMaxOutTrsSize_c  0x0200    /* M - Maximum OUT transfer Size attributes Set */
#define gZclAttrGTProtoAddr_c      0x0300    /* M - Protocol Address attributes Set */
#else
#define gZclAttrGTMaxInTrsSize_c   0x0001    /* M - Maximum IN transfer Size attributes Set */
#define gZclAttrGTMaxOutTrsSize_c  0x0002    /* M - Maximum OUT transfer Size attributes Set */
#define gZclAttrGTProtoAddr_c      0x0003    /* M - Protocol Address attributes Set */
#endif /* #if ( TRUE == gBigEndian_c ) */


#define gZclCmdRxGTMpa_c                 0x00 /* M - Match Protocol Address */  
#define gZclCmdTxGTMpaRes_c              0x00 /* M - Match Protocol Address Response */  
#define gZclCmdTxGTAdvertiseProtoAddr_c  0x01 /* M - Advertise Protocol Address */  


#ifdef __IAR_SYSTEMS_ICC__
#pragma pack(1)
#endif


typedef struct zclProtocolAddress_tag
{
  uint8_t      AddrLen;
  uint8_t      ProtocolAddress[gZclGtMaxProtocolAddress];
} zclProtocolAddress_t;

typedef struct zclCmdGtMpa_tag 
{
  afAddrInfo_t addrInfo;
  zclProtocolAddress_t protocolAddress;
}zclCmdGtMpa_t;


typedef struct zclCmdGtMpaRes_tag 
{
  afAddrInfo_t addrInfo;
}zclCmdGtMpaRes_t;


typedef struct zclCmdGtAdvertiseProtoAddr_tag 
{
  afAddrInfo_t addrInfo;
  zclProtocolAddress_t protocolAddress;
} zclCmdGtAdvertiseProtoAddr_t;


/*************************************************************
	11703 Protocol Tunnel Cluster (Health Care Profile Annex A.1)
**************************************************************/

#if ( TRUE == gBigEndian_c )
#define gZclAttr11703ProtocTunnelDeviceIdList_c    0x0000  /* M - Device ID List */
#define gZclAttr11703ProtocTunnelManagerTarget_c   0x0100
#define gZclAttr11703ProtocTunnelManagerEndpoint_c 0x0200
#define gZclAttr11703ProtocTunnelConnected_c       0x0300
#define gZclAttr11703ProtocTunnelPreemptible_c     0x0400
#define gZclAttr11703ProtocTunnelIdleTimeout_c     0x0500
#else
#define gZclAttr11703ProtocTunnelDeviceIdList_c    0x0000  /* M - Device ID List */
#define gZclAttr11703ProtocTunnelManagerTarget_c   0x0001
#define gZclAttr11703ProtocTunnelManagerEndpoint_c 0x0002
#define gZclAttr11703ProtocTunnelConnected_c       0x0003
#define gZclAttr11703ProtocTunnelPreemptible_c     0x0004
#define gZclAttr11703ProtocTunnelIdleTimeout_c     0x0005
#endif /* #if ( TRUE == gBigEndian_c ) */



/* A.1.2.3 11073 Protocol Tunnel  cluster Commands Received */
#define gZclCmdRx11073_TransferAPDU_c                0x00    /* M - Transfer APDU */  
#define gZclCmdRx11073_ConnectRequest_c            0x01    /* M - Connect request */
#define gZclCmdRx11073_DisconnectRequest_c         0x02    /* M - Disconnect request */
#define gZclCmdRx11073_ConnectStatusNotif_c        0x03    /* M - Connect status notification */


typedef struct zclApdu_tag
{
  uint16_t apduLen;
  uint8_t  apdu[1];  
}
zclApdu_t;



/* Transfer APDU command */
typedef struct zclCmd11073_TransferApdu_tag 
{
  afAddrInfo_t addrInfo;
  zclApdu_t    apdu;
}
zclCmd11073_TransferApdu_t;

/* Transfer APDU command */
typedef struct zclCmd11073_TransferApduPtr_tag 
{
  afAddrInfo_t addrInfo;
  zclApdu_t*   apdu;
}
zclCmd11073_TransferApduPtr_t;


typedef uint8_t zclConnectControl_t;

#define zclCmd11073_PreemtibleMask 0x01;

typedef struct  zclCmd11073_ConnectRequestParams_tag {
  zclConnectControl_t connectControl;
  uint16_t idleTimeout;
  uint8_t ieeeAddress[8];
  uint8_t managerEndpoint;
} zclCmd11073_ConnectRequestParams_t;

typedef struct  zclCmd11073_ConnectRequest_tag {
  afAddrInfo_t addrInfo;
  zclCmd11073_ConnectRequestParams_t params;
} zclCmd11073_ConnectRequest_t;

typedef struct  zclCmd11073_DisconnectRequest_tag {
  afAddrInfo_t addrInfo;
  uint8_t ieeeAddress[8];
} zclCmd11073_DisconnectRequest_t;

typedef enum  {
  zcl11073Status_Disconnected,
  zcl11073Status_Connected,
  zcl11073Status_NotAuthorized,
  zcl11073Status_ReconnectRequest,
  zcl11073Status_AlreadyConnected
} zclCmd11073_ConnectStatus_t;

typedef struct  zclCmd11073_ConnectStatusNotif_tag {
  afAddrInfo_t addrInfo;
  zclCmd11073_ConnectStatus_t connectStatus;
} zclCmd11073_ConnectStatusNotif_t;

typedef struct zclGenericAppAttrRAM_tag
{
  uint16_t maxInTrsSize;
  uint16_t maxOutTrsSize;
  uint8_t  protocolAddress[gZclGtMaxProtocolAddress];
  uint8_t  deviceIDList[5];
  uint8_t  managerTarget[8];
  uint8_t  managerEndpoint;
  bool_t   connected;
  bool_t   preemtible;
  uint16_t idleTimeout;
  tmrTimerID_t idleTimer;
} zclGenericAppAttrRAM_t;


#ifdef __IAR_SYSTEMS_ICC__
#pragma pack()
#endif


/* Application events */
enum
{
  gHcEventApduRcvd_c = 60
};


/******************************************************************************
*******************************************************************************
* Public functions prototypes
*******************************************************************************
******************************************************************************/

extern const zclAttrDefList_t gGtClusterAttrDefList;
extern const zclAttrDef_t gaGtClusterAttrDef[];
extern const zclAttrDef_t ga11073TunnelClusterAttrDef[];
extern const zclAttrDefList_t g11073TunnelClusterAttrDefList;

zbStatus_t ZCL_GenericTunnelClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev);
zbStatus_t ZCL_GenericTunnelClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev);
zbStatus_t ZCL_11073TunnelClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev);
zbStatus_t ZCL_11073TunnelClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev);

zbStatus_t IEEE11073ProtocolTunnel_GetIEEE11073MessageProcStatus(void);
zbStatus_t GenericTunnel_MatchProtocolAddress (zclCmdGtMpa_t *pReq);
zbStatus_t GenericTunnel_MatchProtocolAddressResponse ( zclCmdGtMpaRes_t *pReq);
zbStatus_t GenericTunnel_AdvertiseProtocolAddress (zclCmdGtAdvertiseProtoAddr_t *pReq);
zbStatus_t IEEE11073ProtocolTunnel_TransferApdu (zclCmd11073_TransferApdu_t *pReq);
zbStatus_t IEEE11073ProtocolTunnel_TransferApduPtr (zclCmd11073_TransferApduPtr_t *pReq);
zbStatus_t IEEE11073ProtocolTunnel_ConnectRequest (zclCmd11073_ConnectRequest_t *pReq);
zbStatus_t IEEE11073ProtocolTunnel_DisconnectRequest (zclCmd11073_DisconnectRequest_t *pReq);
zbStatus_t IEEE11073ProtocolTunnel_ConnectStatusNotif (zclCmd11073_ConnectStatusNotif_t *pReq);
void ZCL_11073Init(void);
zbStatus_t IEEE11073ProtocolTunnel_InternalDisconnectReq(zclCmd11073_ConnectStatusNotif_t* statusNotif);
#if gZclEnablePartition_d  
zbStatus_t IEEE11073ProtocolTunnel_SetPartitionThreshold(uint8_t threshold);
#endif

#endif
