/******************************************************************************
* ZclSETunneling.h
*
* This module contains code that handles ZigBee Smart Energy Tunneling
functionality and commands.
*
* Copyright (c) 2011, Freescale, Inc.  All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
*******************************************************************************/

#define gZclSETunnelMaxDataHandlingCapacity_c 2
#define gZclSETunnelRxBufferSize_c  200 // also used for maximum incoming transfer size
#define gZclSETunnelNumberOfSupportedProtocolIDs_c  1
#define gZclSETunnelNumberOfProtocolIDsInSTPR_c		16

#if gBigEndian_c
#define gZclAttrTunneling_CloseTunnelTimeout_c   0x1E00
#else
#define gZclAttrTunneling_CloseTunnelTimeout_c   0x001E
#endif

// ProtocolIDs
#define DLMS_COSEM    0x00
#define IEC61107      0x01
#define ANSI_C12      0x02
#define M_BUS         0x03
#define SML           0x04
#define ClimateTalk   0x05
#define TestProtocol  0xC7

// Manufacturer codes
#define gZclSETunnel_NoManufacturerCode_c	0xFFFF

// Tunnel IDs
#define gZclSETunnel_TunnelFail_c    0xFFFF

#define gZclCluster_High_Tunneling_c  0x07
#define gZclCluster_Low_Tunneling_c   0x04

// Tunnel status
#define gZclSETunnel_Success_c                    0x00
#define gZclSETunnel_Busy_c                       0x01
#define gZclSETunnel_NoMoreTunnelIDs_c            0x02
#define gZclSETunnel_ProtocolNotSupported_c       0x03
#define gZclSETunnel_FlowControlNotSupported_c    0x04
/* not in the spec but sent when no appropiate status was available */
#define gZclSETunnel_GeneralFailiure_c            0xf0

// Tunnel entry status
#define gZclSETunnel_TTE_Free_c                         0x00
#define gZclSETunnel_TTE_AwaitingTunnelRequestRsp_c     0x01
#define gZclSETunnel_TTE_Active_c                       0x02

// Transfer data status
#define gZclSETunnel_NoSuchTunnel_c				  0x00
#define gZclSETunnel_WrongDevice_c				  0x01
#define gZclSETunnel_DataOverflow_c				  0x02
#define gZclSETunnel_NoError_c                                    0xFD

// ZCL SE Tunneling status
#define gZclSETunnel_TableFull_c                  0xfe
#define gZclSETunnel_Fail_c                       0xff

#define gMaxApsDataSize 68

/*----------Client commands---------*/
#define gZclCmdSETunneling_Client_RequestTunnelReq_c              0x00 /* M */
#define gZclCmdSETunneling_Client_CloseTunnelReq_c                0x01 /* M */
#define gZclCmdSETunneling_Client_TransferData_c                  0x02 /* M */
#define gZclCmdSETunneling_Client_TransferDataError_c          	  0x03 /* M */
#define gZclCmdSETunneling_Client_AckTransferDataRsp_c            0x04 /* O */
#define gZclCmdSETunneling_Client_ReadyDataReq_c                  0x05 /* O */
#define gZclCmdSETunneling_Client_GetSupportedTunnelProtocols_c   0x06 /* O */
/*----------Server commands---------*/
#define gZclCmdSETunneling_Server_RequestTunnelRsp_c              0x00 /* M */
#define gZclCmdSETunneling_Server_TransferData_c                  0x01 /* M */
#define gZclCmdSETunneling_Server_TransferDataError_c             0x02 /* M */
#define gZclCmdSETunneling_Server_AckTransferDataRsp_c            0x03 /* O */
#define gZclCmdSETunneling_Server_ReadyDataReq_c                  0x04 /* O */
#define gZclCmdSETunneling_Server_SupportedTunnelProtocolsRsp_c	  0x05 /* O */

/* Server attributes*/
#define gZclAttrSETunnelingCloseTunnelTimeout_c         0x0000

#ifdef __IAR_SYSTEMS_ICC__
#pragma pack(1)
#endif

typedef struct zclCmdSETunneling_RequestTunnelReq_tag
{
  uint8_t protocolID;
  uint16_t manufacturerCode;
  bool_t flowControlSupport;
  uint16_t maximumIncomingtransferSize;
} zclCmdSETunneling_RequestTunnelReq_t;


typedef struct  zclSETunneling_RequestTunnelReq_tag
{  
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdSETunneling_RequestTunnelReq_t cmdFrame;
} zclSETunneling_RequestTunnelReq_t;

typedef struct zclCmdSETunneling_CloseTunnelReq_tag
{
  uint16_t tunnelId;
}zclCmdSETunneling_CloseTunnelReq_t;

typedef struct zclSETunneling_CloseTunnelReq_tag
{
  afAddrInfo_t addrInfo;
  uint8_t zclTransactionId;
  zclCmdSETunneling_CloseTunnelReq_t cmdFrame;
}zclSETunneling_CloseTunnelReq_t;

typedef struct zclCmdSETunneling_TransferDataReq_tag
{
  uint16_t            tunnelId;
  zclLongOctetStr_t   data;
}zclCmdSETunneling_TransferDataReq_t;

typedef struct zclSETunneling_TransferDataReq_tag
{
  bool_t client;
  zclCmdSETunneling_TransferDataReq_t   cmdFrame;
}zclSETunneling_TransferDataReq_t;

typedef struct zclSETunneling_ZTCTransferDataReq_tag
{
  bool_t      client;
  uint16_t    tunnelId;
}zclSETunneling_ZTCTransferDataReq_t;

typedef struct zclSETunneling_ZTCLoadFragment_tag
{
  uint16_t          index;
  uint8_t           length;
  uint8_t           data[1];
}zclSETunneling_ZTCLoadFragment_t;

typedef struct zclCmdSETunneling_TransferDataErrorReq_tag
{
  uint16_t tunnelId;
  uint8_t transferDataStatus;
}zclCmdSETunneling_TransferDataErrorReq_t;

typedef struct zclSETunneling_TransferDataErrorReq_tag
{
  afAddrInfo_t                          addrInfo;
  uint8_t                               zclTransactionId;
  zclCmdSETunneling_TransferDataErrorReq_t   cmdFrame;
}zclSETunneling_TransferDataErrorReq_t;

typedef struct zclCmdSETunneling_GetSupportedTunnelProtocolsReq_tag
{
  uint8_t	protocolOffset;
}zclCmdSETunneling_GetSupportedTunnelProtocolsReq_t;

typedef struct zclSETunneling_GetSupportedTunnelProtocolsReq_tag
{
  afAddrInfo_t                          addrInfo;
  uint8_t                               zclTransactionId;
  zclCmdSETunneling_GetSupportedTunnelProtocolsReq_t	cmdFrame;
}zclSETunneling_GetSupportedTunnelProtocolsReq_t;

typedef struct zclCmdSETunneling_AckTransferDataRsp_tag
{
  uint16_t tunnelId;
  uint16_t numberOfBytesLeft;
}zclCmdSETunneling_AckTransferDataRsp_t;

typedef struct zclSETunneling_AckTransferDataRsp_tag
{
  afAddrInfo_t                          addrInfo;
  uint8_t                               zclTransactionId;
  zclCmdSETunneling_AckTransferDataRsp_t   cmdFrame;
}zclSETunneling_AckTransferDataRsp_t;

typedef struct zclCmdSETunneling_ReadyDataReq_tag
{
  uint16_t tunnelId;
  uint16_t numberOfBytesLeft;
}zclCmdSETunneling_ReadyDataReq_t;

typedef struct zclSETunneling_ReadyDataReq_tag
{
  afAddrInfo_t                          addrInfo;
  uint8_t                               zclTransactionId;
  zclCmdSETunneling_ReadyDataReq_t         cmdFrame;
}zclSETunneling_ReadyDataReq_t;

typedef struct zclCmdSETunneling_RequestTunnelRsp_tag
{
  uint16_t tunnelId;
  uint8_t  tunnelStatus;
  uint16_t maximumIncomingtransferSize;
}zclCmdSETunneling_RequestTunnelRsp_t;

typedef struct zclSETunneling_RequestTunnelRsp_tag
{
  afAddrInfo_t                          addrInfo;
  uint8_t                               zclTransactionId;
  zclCmdSETunneling_RequestTunnelRsp_t  cmdFrame;
}zclSETunneling_RequestTunnelRsp_t;

typedef struct zclSETunneling_Protocol_tag
{
  uint16_t 	manufacturerCode;
  uint8_t	protocolID;
}zclSETunneling_Protocol_t;

typedef struct zclCmdSETunneling_SupportedTunnelingProtocolsRsp_tag
{
  bool_t                      protocolListComplete;
  uint8_t                     protocolCount;
  zclSETunneling_Protocol_t   protocolList[1];
}zclCmdSETunneling_SupportedTunnelingProtocolsRsp_t;

typedef struct zclSETunneling_SupportedTunnelingProtocolsRsp_tag
{
  afAddrInfo_t                          				addrInfo;
  uint8_t                               				zclTransactionId;
  zclCmdSETunneling_SupportedTunnelingProtocolsRsp_t	cmdFrame;
}zclSETunneling_SupportedTunnelingProtocolsRsp_t;

typedef struct zclSETunneling_ReadyRx_tag
{
  uint16_t tunnelID;
  bool_t client;
}zclSETunneling_ReadyRx_t;

#ifdef __IAR_SYSTEMS_ICC__
#pragma pack()
#endif

// Client commands
zbStatus_t ZCL_SETunnel_Client_RequestTunnelReq(zclSETunneling_RequestTunnelReq_t *pReq);
zbStatus_t ZCL_SETunnel_SendTunnelReq(zbNwkAddr_t *pNwkAddress, uint8_t protocolID, uint16_t manufacturerCode, bool_t flowControlSupport, uint16_t maximumIncomingtransferSize);
zbStatus_t ZCL_SETunnel_SendGetSupportedProtocols(zbNwkAddr_t *pNwkAddress);
zbStatus_t ZCL_SETunnel_Client_CloseTunnelReq(zclSETunneling_CloseTunnelReq_t *pReq);

// Server commands
zbStatus_t ZCL_SETunnel_Server_SetNextTunnelID(uint16_t tunnelID);

void ZCL_SETunnel_InitData(void);
zbStatus_t ZCL_SETunnel_ZTCTransferDataReq(zclSETunneling_ZTCTransferDataReq_t *pReq);
zbStatus_t ZCL_SETunnel_ReadyRx(zclSETunneling_ReadyRx_t *pReq);
zbStatus_t ZCL_SETunnel_ZTCLoadFragment(zclSETunneling_ZTCLoadFragment_t *pReq);

typedef struct tunnelingTable_tag
{
  /* common */
  uint16_t tunnelID;
  uint8_t protocolId;
  uint16_t manufacturerCode;
  bool_t flowControlSupport;
  uint8_t tunnelStatus;
  /* partner */
  uint16_t maximumIncomingTransferSize;
  uint16_t numberOfOctetsLeft;
  zbIeeeAddr_t partnerIeeeAddr;
  afAddrInfo_t partnerAddrInfo;
  /* local */
  tmrTimerID_t closeTunnelTmr;
  uint16_t rxDataLength;
  uint8_t *rxData;
  uint16_t txDataSent;
  uint16_t txDataLength;
  uint8_t *txData;
  /* unused*/
  uint8_t transferDataStatus;
} tunnelingTable_t;
