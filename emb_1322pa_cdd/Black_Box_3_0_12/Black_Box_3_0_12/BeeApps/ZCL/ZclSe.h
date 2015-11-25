/******************************************************************************
* ZclSE.h
*
* Types, definitions and prototypes for the ZigBee SE implementition.
*
* Copyright (c) 2006, Freescale, Inc.  All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from  Freescale Semiconductor.
*
******************************************************************************/
#ifndef _ZCLSE_H
#define _ZCLSE_H

/* header files needed by home automation */
#include "EmbeddedTypes.h"
#include "AfApsInterface.h"
#include "ZCL.h"
#include "SEProfile.h"
/******************************************************************************
*******************************************************************************
* Public macros and data types definitions.
*******************************************************************************
******************************************************************************/
/*Note all these variables should be allocted in msg system*/
/*total size of memory needed : 
42 22 21 22 21 16 16 22 2 = 205 bytes.
*/

/* ZCL SE Price cluster events */
#define gzclEvtHandleGetScheduledPrices_c (1 << 0)
#define gzclEvtHandleClientPrices_c (1 << 1)
#define gzclEvtHandlePublishPriceUpdate_c (1<< 2)
#define gzclEvtHandleGetScheduledLdCtlEvts_c (1 << 3)
#define gzclEvtHandleGetBlockPeriods_c (1 << 4)
#define gzclEvtHandlePublishBlockPeriodUpdate_c (1 << 5)
#define gzclEvtHandleGetConversionFactor_c (1 << 6)
#define gzclEvtHandleGetCalorificValue_c (1 << 7)
#define gzclEvtHandleClientBlockPeriod_c (1 << 8)
#define gzclEvtHandleConversionFactorPeriod_c (1 << 9)
#define gzclEvtHandleCalorificValuePeriod_c (1 << 10)
#define gzclEvtHandleGetBillingPeriod_c   (1 << 11)
#define gzclEvtHandlePublishBillingPeriodUpdate_c (1 << 12)
#define gzclEvtHandleClientBillingPeriod_c   (1 << 13)
#define gzclEvtHandleGetConsolidatedBill_c   (1 << 14)
#define gzclEvtHandlePublishConsolidatedBillUpdate_c (1 << 15)
#define gzclEvtHandleClientConsolidatedBill_c   ((uint32_t)1 << 16)
#define gzclEvtHandlePublishCO2ValueUpdate_c ((uint32_t)1 << 17)
#define gzclEvtHandleGetCO2Value_c ((uint32_t)1 << 18)
#define gzclEvtHandleGetTariffInformation_c ((uint32_t)1 << 19)
#define gzclEvtHandlePublishTariffInformationUpdate_c ((uint32_t)1 << 20)
#define gzclEvtHandleGetPriceMatrix_c ((uint32_t)1 << 21)
#define gzclEvtHandleGetBlockThresholds_c ((uint32_t)1 << 22)
#define gzclEvtHandlePublishCPPEvent_c ((uint32_t)1 << 23)

typedef struct ESPRegTable_tag{
EspRegisterDevice_t DevInfo;
uint8_t             DevStatus;
}ESPRegTable_t;

#define RegTable_DevStatusKeyEstabComplete_c 0x02
#define RegTable_DevStatusIntialState_c 0x01
#define REgTable_InvalidIndex_c 0xff

typedef struct KeyEstab_KeyData_tag
{
  uint8_t ephemeralPrivateKey[gZclCmdKeyEstab_PrivateKeySize_c];
  uint8_t u_ephemeralPublicKey[gZclCmdKeyEstab_CompressedPubKeySize_c];
  uint8_t v_ephemeralPublicKey[gZclCmdKeyEstab_CompressedPubKeySize_c];
  uint8_t SharedSecret[gZclCmdKeyEstab_SharedSecretSize_c];
  uint8_t MACU[gZclCmdKeyEstab_AesMMOHashSize_c]; 
  uint8_t MACV[gZclCmdKeyEstab_AesMMOHashSize_c]; 
  uint8_t mackey[gZclCmdKeyEstab_SharedSecretSize_c]; /*only 16 bytes is used, but must be same length as Shared secret*/  
} KeyEstab_KeyData_t;

/*key estab state machine states */
#define KeyEstab_InitState_c             0x00
#define KeyEstab_KeyEstabInitatedState_c 0x01
#define KeyEstab_EphemeralState_c        0x02
#define KeyEstab_ConfirmDataState_c      0x03

zbStatus_t ZCL_KeyEstabClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev);
zbStatus_t ZCL_KeyEstabClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev);
zbStatus_t ZCL_MsgClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev);
zbStatus_t ZCL_MsgClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev);
zbStatus_t ZCL_InterPanMsgClusterClient(zbInterPanDataIndication_t *pIndication, afDeviceDef_t *pDev);
zbStatus_t ZCL_InterPanMsgClusterServer(zbInterPanDataIndication_t *pIndication, afDeviceDef_t *pDev);

zbStatus_t ZCL_DmndRspLdCtrlClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev); 
/* Procesing Client side Cluster for Demand Response and Load Control Event */
zbStatus_t ZCL_DmndRspLdCtrlClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev); 


zbStatus_t ZclMsg_DisplayMsgReq(zclDisplayMsgReq_t *pReq);
zbStatus_t ZclMsg_CancelMsgReq(zclCancelMsgReq_t *pReq);
zbStatus_t ZclMsg_GetLastMsgReq(zclGetLastMsgReq_t *pReq);
zbStatus_t ZclMsg_MsgConf(zclMsgConfReq_t *pReq);
zbStatus_t ZclMsg_InterPanDisplayMsgReq(zclInterPanDisplayMsgReq_t *pReq);
zbStatus_t ZclMsg_InterPanCancelMsgReq(zclInterPanCancelMsgReq_t *pReq);
zbStatus_t ZclMsg_InterPanGetLastMsgReq(zclInterPanGetLastMsgReq_t *pReq);
zbStatus_t ZclMsg_InterPanMsgConf(zclInterPanMsgConfReq_t *pReq);
void ZclMsg_StoreMessage(zclDisplayMsgReq_t *pReq);

zbStatus_t ZclDmndRspLdCtrl_ReportEvtStatus(zclDmndRspLdCtrl_ReportEvtStatus_t *pReq);
zbStatus_t ZclDmndRspLdCtrl_LdCtrlEvtReq(zclDmndRspLdCtrl_LdCtrlEvtReq_t *pReq);
zbStatus_t ZclDmndRspLdCtrl_CancelLdCtrlEvtReq(zclDmndRspLdCtrl_CancelLdCtrlEvtReq_t *pReq);
zbStatus_t ZclDmndRspLdCtrl_CancelAllLdCtrlEvtReq(zclDmndRspLdCtrl_CancelAllLdCtrlEvtsReq_t *pReq);
zbStatus_t ZclDmndRspLdCtrl_GetScheduledEvtsReq(zclDmndRspLdCtrl_GetScheduledEvts_t *pReq);
zbStatus_t ZCL_InterPanClusterServer(zbInterPanDataIndication_t *pIndication,afDeviceDef_t *pDev);
zbStatus_t ZCL_InterPanClusterClient(zbInterPanDataIndication_t *pIndication, afDeviceDef_t *pDev); 
zbStatus_t ZCL_InterPanPriceClusterServer(zbInterPanDataIndication_t *pIndication,afDeviceDef_t *pDev);
zbStatus_t ZCL_InterPanPriceClusterClient(zbInterPanDataIndication_t *pIndication, afDeviceDef_t *pDev); 

zbStatus_t ZCL_SETunnelingClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev);
zbStatus_t ZCL_SETunnelingServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev);

zbStatus_t ZCL_PrepaymentClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev);
zbStatus_t ZCL_PrepaymentServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev);
zbStatus_t zclPrepayment_Client_SelAvailEmergCreditReq(zclPrepayment_SelAvailEmergCreditReq_t *pReq);
zbStatus_t zclPrepayment_Client_ChangeSupplyReq(zclPrepayment_ChangeSupplyReq_t *pReq);

zbStatus_t zclKeyEstab_InitKeyEstabReq(ZclKeyEstab_InitKeyEstabReq_t *pReq);
zbStatus_t zclKeyEstab_EphemeralDataReq(ZclKeyEstab_EphemeralDataReq_t *pReq);
zbStatus_t zclKeyEstab_ConfirmKeyDataReq(ZclKeyEstab_ConfirmKeyDataReq_t *pReq);
zbStatus_t zclKeyEstab_TerminateKeyEstabServer(ZclKeyEstab_TerminateKeyEstabServer_t *pReq);
zbStatus_t zclKeyEstab_InitKeyEstabRsp(ZclKeyEstab_InitKeyEstabRsp_t *pReq);
zbStatus_t zclKeyEstab_EphemeralDataRsp(ZclKeyEstab_EphemeralDataRsp_t *pReq);
zbStatus_t zclKeyEstab_ConfirmKeyDataRsp(ZclKeyEstab_ConfirmKeyDataRsp_t *pReq);
zbStatus_t zclKeyEstab_TerminateKeyEstabClient(ZclKeyEstab_TerminateKeyEstabClient_t *pReq);
zbStatus_t zclKeyEstab_SetSecurityMaterial(ZclKeyEstab_SetSecurityMaterial_t *pReq);

void ZCL_ApplyECDSASign(uint8_t *pBufIn, uint8_t lenIn, uint8_t *pBufOut);
bool_t ZCL_InitiateKeyEstab(zbEndPoint_t DstEndpoint,zbEndPoint_t SrcEndpoint, zbNwkAddr_t DstAddr);
bool_t InitClientKeyEstab(void);
void ZCL_SendInitiatKeyEstReq(IdentifyCert_t *Cert, zbEndPoint_t DstEndpoint,zbEndPoint_t SrcEndpoint, zbNwkAddr_t DstAddr);
void ZCL_SendInitiatKeyEstRsp(IdentifyCert_t *Cert,  zbApsdeDataIndication_t *pIndication, uint8_t transactionid);
void ZCL_SendEphemeralDataReq(uint8_t *EphemeralPubKey,  zbApsdeDataIndication_t *pIndication);
void ZCL_SendEphemeralDataRsp(uint8_t *EphemeralPubKey,  zbApsdeDataIndication_t *pIndication, uint8_t transactionid);
void ZCL_SendConfirmKeyDataReq(uint8_t *MACU,  zbApsdeDataIndication_t *pIndication);
void ZCL_SendConfirmKeyDataRsp(uint8_t *MACV,  zbApsdeDataIndication_t *pIndication, uint8_t transactionid);
void ZCL_SendTerminateKeyEstabServerReq(uint8_t Status,uint8_t WaitCode,  zbApsdeDataIndication_t *pIndication);
void ZCL_SendTerminateKeyEstabClientReq(uint8_t Status, uint8_t WaitCode,  zbApsdeDataIndication_t *pIndication, uint8_t transactionid);
extern const zclAttrDef_t gZclKeyEstabServerAttrDef[];
extern const zclAttrDef_t gaZclDRLCClientServerAttrDef[]; 

#if gInterPanCommunicationEnabled_c
extern pfnInterPanIndication_t pfnInterPanServerInd;
extern pfnInterPanIndication_t pfnInterPanClientInd;
#endif

void PrepareInterPanForReply(InterPanAddrInfo_t *pAdrrDest, zbInterPanDataIndication_t *pIndication);
void SE_Init(void);
void ZCL_LdCtrlClientInit(void);
uint8_t FindEventInEventsTable(uint8_t *pEvtId);
zbStatus_t ZCL_ScheduleServerLdCtrlEvents(zclCmdDmndRspLdCtrl_LdCtrlEvtReq_t *pMsg);
void ZCL_DeleteServerScheduledEvents(void);
void ZCL_HandleScheduledEventNow(void);
void ZCL_HandleReportEventsStatus(void);
void ZCL_HandleGetScheduledLdCtlEvts(void);
uint8_t ZCL_SetOptStatusOnEvent(uint8_t *pEvtId, uint8_t optStatus); 
uint8_t ZCL_DeleteLdCtrlEvent(uint8_t *pEvtId); 
extern zbStatus_t ZCL_SendReportEvtStatus
(
afAddrInfo_t *pAddrInfo, 
zclCmdDmndRspLdCtrl_LdCtrlEvtReq_t *pMsg, 
uint8_t eventStatus, 
bool_t invalidValueFlag /* if TRUE sent RES with invalid values for fields */
);
/* Accepting voluntary load control events */
void ZCL_AcceptVoluntaryLdCrtlEvt(bool_t flag);

extern void ZCL_RegisterInterPanClient(pfnInterPanIndication_t pFunc);
extern void ZCL_RegisterInterPanServer(pfnInterPanIndication_t pFunc);
extern void PrepareInterPanForReply(InterPanAddrInfo_t *pAdrrDest, zbInterPanDataIndication_t *pIndication);

extern void GetProfileTestTimerCallBack(tmrTimerID_t tmrID);
extern void ZCL_Register_EspRegisterDeviceFunctions(ESPRegisterDevFunc *RegFunc,ESPDeRegisterDevFunc *DeRegFunc);
extern bool_t GenerateKeyFromInstallCode(uint8_t length,uint8_t* pInstallationCode, uint8_t *pKey);
extern ESPRegisterDevFunc *pSE_ESPRegFunc;
extern ESPDeRegisterDevFunc *pSE_ESPDeRegFunc;

extern zbStatus_t ZCL_ESPRegisterDevice(EspRegisterDevice_t *Devinfo);
extern zbStatus_t ZCL_ESPDeRegisterDevice(EspDeRegisterDevice_t *Devinfo);
extern index_t ZCL_AddToRegTable(EspRegisterDevice_t *Devinfo);
extern index_t ZCL_FindIeeeInRegTable(zbIeeeAddr_t aExtAddr);
extern void ZCL_ESPInit(void);
extern void ZCL_MsgInit(void);
extern void ZCL_PrepaymentServerInit(void);
extern msgAddrInfo_t *ZCL_GetMsgSourceaddrForResponse(void);
extern void ZCL_SetKeyEstabComplete(zbIeeeAddr_t aExtAddr);
extern bool_t ZCL_IsKeyEstablishmentCompleted(zbIeeeAddr_t aExtAddr);
extern bool_t ZCl_SEClusterSecuritycheck(zbApsdeDataIndication_t *pIndication);


extern zclLdCtrl_EventsTableEntry_t gaEventsTable[gNumOfEventsTableEntry_c];

extern uint16_t        gZdoOrgPollRate; 
extern ZCLTimeServerAttrsRAM_t gTimeAttrsData;

#endif /* _ZCLSE_H */
