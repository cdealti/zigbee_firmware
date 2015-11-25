/******************************************************************************
* ZclSE.c
*
* This source file describes Clusters defined in the ZigBee Cluster Library,
* General document. Some or all of these
*
* Copyright (c) 2007, Freescale, Inc. All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
******************************************************************************/
#include "EmbeddedTypes.h"
#include "zigbee.h"
#include "FunctionLib.h"
#include "BeeStackConfiguration.h"
#include "BeeStack_Globals.h"
#include "AppAfInterface.h"
#include "AfApsInterface.h"
#include "BeeStackInterface.h"
#include "ZdoApsInterface.h"
#include "HaProfile.h"
#include "ZCLGeneral.h"
#include "zcl.h"
#include "SEProfile.h"
#include "zclSE.h"
#include "display.h"
#include "eccapi.h"
#include "Led.h"
#include "ZdoApsInterface.h"
#include "ASL_ZdpInterface.h"
#include "ApsMgmtInterface.h"
#include "beeapp.h"
/******************************************************************************
*******************************************************************************
* Private macros
*******************************************************************************
******************************************************************************/



/******************************************************************************
*******************************************************************************
* Private prototypes
*******************************************************************************
******************************************************************************/
int ECC_GetRandomDataFunc(unsigned char *buffer, unsigned long sz);
int ECC_HashFunc(unsigned char *digest, unsigned long sz, unsigned char *data);

static uint8_t FindEmptyEntryInEventsTable(void);
static void AddNewEntry(zclLdCtrl_EventsTableEntry_t *pDst, zclCmdDmndRspLdCtrl_LdCtrlEvtReq_t *pSrc, afAddrInfo_t *pAddrInfo);
static void CheckForSuccessiveEvents(uint8_t msgIndex);
static uint8_t ZCL_ScheduleEvents(afAddrInfo_t *pAddrInfo, zclCmdDmndRspLdCtrl_LdCtrlEvtReq_t *pMsg);
static zbStatus_t ZCL_ProcessCancelLdCtrlEvt(zbApsdeDataIndication_t *pIndication);
static zbStatus_t ZCL_ProcessCancelAllLdCtrlEvtst(zbApsdeDataIndication_t * pIndication);
static zbStatus_t ZCL_ProcessLdCtrlEvt(zbApsdeDataIndication_t *pIndication);
static zbStatus_t ZCL_ProcessGetScheduledEvtsReq(addrInfoType_t *pAddrInfo, zclCmdDmndRspLdCtrl_GetScheduledEvts_t * pGetScheduledEvts);
static void LdCtrlEvtTimerCallback(tmrTimerID_t timerID);
static void LdCtrlJitterTimerCallBack(tmrTimerID_t timerID);

/* struct for message cluster response */
static void MsgDisplayTimerCallBack(tmrTimerID_t tmrID);
static msgAddrInfo_t MsgResponseSourceAddr;
tmrTimerID_t gMsgDisplayTimerID;
zclMsgLastMessage_t gMsgLastMessage = {0xFF};

/* prepayment cluster */
static zbStatus_t zclPrepayment_Server_SupplyStatusRsp(zclPrepayment_SupplyStatRsp_t *pReq);
static zbStatus_t ZCL_SendSupplyStatusRsp(zbApsdeDataIndication_t *pIndication);
static zbStatus_t ZCL_ProcessChangeSupplyReq(zbApsdeDataIndication_t *pIndication);
void PrepaymentTimerCallBack(tmrTimerID_t tmrID);
tmrTimerID_t gPrepaymentTimerID;
uint8_t mProposedSupplyStatus;

/******************************************************************************
*******************************************************************************
* Private type definitions
*******************************************************************************
******************************************************************************/

#define gUpdateConsumption_c  10

/******************************************************************************
*******************************************************************************
* Public memory definitions
*******************************************************************************
******************************************************************************/
// esp table
ESPRegTable_t RegistrationTable[RegistrationTableEntries_c] = {0};
addrInfoType_t mAddrInfo;
extern uint8_t appEndPoint;
/************** KEY ESTABLISHMENT CLUSTER ***************************************/
/*****************************************************************************/
/*Variables used by KeyEstab cluster                                         */
/*****************************************************************************/
uint8_t KeyEstabState = KeyEstab_InitState_c;
IdentifyCert_t *pOppositeImplicitCert = NULL;
KeyEstab_KeyData_t *KeyData = NULL;
tmrTimerID_t KeyEstabTimerId = gTmrInvalidTimerID_c;
uint8_t EphemeralDataGenerateTime;
uint8_t ConfirmKeyGenerateTime;

#if gZclKeyEstabDebugTimer_d
tmrTimerID_t KeyEstabDebugTimerId = gTmrInvalidTimerID_c;
zbApsdeDataIndication_t *pKeyEstabDataIndication  = NULL;
#endif

zclAttrKeyEstabServerAttrsRAM_t gZclAttrKeyEstabServerAttrs = { 
{0x01,0x00}
};


const zclAttrDef_t gZclKeyEstabServerAttrDef[] = {
/* Server attributes */
  { gZclAttrKeyEstabSecSuite_c,gZclDataTypeEnum16_c, gZclAttrFlagsCommon_c | gZclAttrFlagsRdOnly_c, sizeof(uint16_t),(void *) &gZclAttrKeyEstabServerAttrs.SecuritySuite },
};

const zclAttrDefList_t gZclKeyEstabServerAttrDefList = {
  NumberOfElements(gZclKeyEstabServerAttrDef),
  gZclKeyEstabServerAttrDef
};

/************** MESSAGE CLUSTER  ******************************************/


/*
There is NO attributes defined for the Messaging cluster.

*/


/************** PREPAYMENT CLUSTER  ******************************************/
const zclAttrDef_t gaZclPrepaymentServerAttrDef[] = {
    {gZclAttrSmplPrepaymentPaymentControl_c,				gZclDataTypeBitmap8_c,	gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,	sizeof(uint8_t),			(void*)MbrOfs(sePrepaymentAttrRAM_t,	paymentCtrl)}
#if gASL_ZclPrepayment_Optionals_d                                   
    ,{gZclAttrSmplPrepaymentCreditRemaining_c,				gZclDataTypeUint32_c,	gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,	sizeof(uint32_t),			(void*)MbrOfs(sePrepaymentAttrRAM_t,	creditRemaining)}
    ,{gZclAttrSmplPrepaymentEnergyCreditRemaining_c,		gZclDataTypeInt32_c,	gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,	sizeof(uint32_t),			(void*)MbrOfs(sePrepaymentAttrRAM_t,	emergencyCreditRemaining)}
    ,{gZclAttrSmplPrepaymentCreditStatus_c,					gZclDataTypeBitmap8_c,	gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,	sizeof(uint8_t),			(void*)MbrOfs(sePrepaymentAttrRAM_t,	creditStatus)}
#if gASL_ZclPrepayment_TopUpOptionals_d                          
    ,{gZclAttrSmplPrepaymentTopUpDateTime1_c,				gZclDataTypeUTCTime_c,	gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,	sizeof(uint32_t),			(void*)MbrOfs(sePrepaymentAttrRAM_t,	topUpTime1)}
    ,{gZclAttrSmplPrepaymentTopUpAmount1_c,					gZclDataTypeUint48_c,	gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,	sizeof(sePrepaymentTopUp_t),(void*)MbrOfs(sePrepaymentAttrRAM_t,	topUpAmount1)}
    ,{gZclAttrSmplPrepaymentOriginatingDevice1_c,			gZclDataTypeEnum8_c,	gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,	sizeof(uint8_t),			(void*)MbrOfs(sePrepaymentAttrRAM_t,	topUpOriginatingDevice1)}
    ,{gZclAttrSmplPrepaymentTopUpDateTime2_c,				gZclDataTypeUTCTime_c,	gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,	sizeof(uint32_t),			(void*)MbrOfs(sePrepaymentAttrRAM_t,	topUpTime2)}
    ,{gZclAttrSmplPrepaymentTopUpAmount2_c,					gZclDataTypeUint48_c,	gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,	sizeof(sePrepaymentTopUp_t),(void*)MbrOfs(sePrepaymentAttrRAM_t,	topUpAmount2)}
    ,{gZclAttrSmplPrepaymentOriginatingDevice2_c,			gZclDataTypeEnum8_c,	gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,	sizeof(uint8_t),			(void*)MbrOfs(sePrepaymentAttrRAM_t,	topUpOriginatingDevice2)}
    ,{gZclAttrSmplPrepaymentTopUpDateTime3_c,				gZclDataTypeUTCTime_c,	gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,	sizeof(uint32_t),			(void*)MbrOfs(sePrepaymentAttrRAM_t,	topUpTime3)}
    ,{gZclAttrSmplPrepaymentTopUpAmount3_c,					gZclDataTypeUint48_c,	gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,	sizeof(sePrepaymentTopUp_t),(void*)MbrOfs(sePrepaymentAttrRAM_t,	topUpAmount3)}
    ,{gZclAttrSmplPrepaymentOriginatingDevice3_c,			gZclDataTypeEnum8_c,	gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,	sizeof(uint8_t),			(void*)MbrOfs(sePrepaymentAttrRAM_t,	topUpOriginatingDevice3)}
    ,{gZclAttrSmplPrepaymentTopUpDateTime4_c,				gZclDataTypeUTCTime_c,	gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,	sizeof(uint32_t),			(void*)MbrOfs(sePrepaymentAttrRAM_t,	topUpTime4)}
    ,{gZclAttrSmplPrepaymentTopUpAmount4_c,					gZclDataTypeUint48_c,	gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,	sizeof(sePrepaymentTopUp_t),(void*)MbrOfs(sePrepaymentAttrRAM_t,	topUpAmount4)}
    ,{gZclAttrSmplPrepaymentOriginatingDevice4_c,			gZclDataTypeEnum8_c,	gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,	sizeof(uint8_t),			(void*)MbrOfs(sePrepaymentAttrRAM_t,	topUpOriginatingDevice4)}
    ,{gZclAttrSmplPrepaymentTopUpDateTime5_c,				gZclDataTypeUTCTime_c,	gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,	sizeof(uint32_t),			(void*)MbrOfs(sePrepaymentAttrRAM_t,	topUpTime5)}
    ,{gZclAttrSmplPrepaymentTopUpAmount5_c,					gZclDataTypeUint48_c,	gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,	sizeof(sePrepaymentTopUp_t),(void*)MbrOfs(sePrepaymentAttrRAM_t,	topUpAmount5)}
    ,{gZclAttrSmplPrepaymentOriginatingDevice5_c,			gZclDataTypeEnum8_c,	gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,	sizeof(uint8_t),			(void*)MbrOfs(sePrepaymentAttrRAM_t,	topUpOriginatingDevice5)}
#endif /*gASL_ZclPrepayment_TopUpOptionals_d*/
#if gASL_ZclPrepayment_DebtOptionals_d	
    ,{gZclAttrSmplPrepaymentFuelDebtRemaining_c,			gZclDataTypeUint48_c,	gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,	sizeof(sePrepaymentDebt_t),	(void*)MbrOfs(sePrepaymentAttrRAM_t,	fuelDebtRemaining)}
    ,{gZclAttrSmplPrepaymentFuelDebtRecoveryRate_c,		    gZclDataTypeUint32_c,	gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,	sizeof(uint32_t),			(void*)MbrOfs(sePrepaymentAttrRAM_t,	fuelDebtRecoveryRate)}
    ,{gZclAttrSmplPrepaymentFuelDebtRecoveryPeryod_c,		gZclDataTypeEnum8_c,	gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,	sizeof(uint8_t),			(void*)MbrOfs(sePrepaymentAttrRAM_t,	fuelDebtRecoveryPeriod)}
    ,{gZclAttrSmplPrepaymentNonFuelDebtRemaining_c,			gZclDataTypeUint48_c,	gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,	sizeof(sePrepaymentDebt_t),	(void*)MbrOfs(sePrepaymentAttrRAM_t,	nonFuelDebtRemaining)}
    ,{gZclAttrSmplPrepaymentNonFuelDebtRecoveryRate_c,		gZclDataTypeUint32_c,	gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,	sizeof(uint32_t),			(void*)MbrOfs(sePrepaymentAttrRAM_t,	nonFuelDebtRecoveryRate)}
    ,{gZclAttrSmplPrepaymentNonFuelDebtRecoveryPeriod_c,    gZclDataTypeEnum8_c,	gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,	sizeof(uint8_t),			(void*)MbrOfs(sePrepaymentAttrRAM_t,	nonFuelDebtRecoveryPeriod)}
#endif /*gASL_ZclPrepayment_DebtOptionals_d*/        	                                                                                                            	
#if gASL_ZclPrepayment_SupplyOptionals_d             	                                                                                                            	
    ,{gZclAttrSmplPrepaymentProposedChangeProviderID_c,		gZclDataTypeUint32_c,	gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,	sizeof(uint32_t),			(void*)MbrOfs(sePrepaymentAttrRAM_t,	proposedChangeProviderId)}
    ,{gZclAttrSmplPrepaymentProposedChangeImplTime_c,		gZclDataTypeUTCTime_c,	gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,	sizeof(uint32_t),			(void*)MbrOfs(sePrepaymentAttrRAM_t,	proposedChangeImplementationTime)}
    ,{gZclAttrSmplPrepaymentProposedChangeSupplyStatus_c,	gZclDataTypeEnum8_c,	gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,	sizeof(uint8_t),			(void*)MbrOfs(sePrepaymentAttrRAM_t,	proposedChangeSupplyStatus)}
    ,{gZclAttrSmplPrepaymentDelayedSuplyIntValueRem_c,		gZclDataTypeUint16_c,	gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,	sizeof(uint16_t),			(void*)MbrOfs(sePrepaymentAttrRAM_t,	delayedSupplyIntValueRemaining)}
    ,{gZclAttrSmplPrepaymentDelayedSupplyIntValType_c,		gZclDataTypeEnum8_c,	gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c,	sizeof(uint8_t),			(void*)MbrOfs(sePrepaymentAttrRAM_t,	delayedSupplyIntValueType)}
#endif /*gASL_ZclPrepayment_SupplyOptionals_d*/      	
#endif /*gASL_ZclPrepayment_Optionals_d*/
};

const zclAttrDefList_t gZclPrepaymentServerAttrDefList = {
  NumberOfElements(gaZclPrepaymentServerAttrDef),
  gaZclPrepaymentServerAttrDef
};


/************** DEMAND RESPONSE LOAD CONTROL CLUSTER  ******************************************/
/*Note there is no Server attributes ???*/

const zclAttrDef_t gaZclDRLCClientServerAttrDef[] = {
/* Server attributes */
  
  /* Client attributes */
  { gZclAttrUtilityGroup_c,   gZclDataTypeUint8_c, gZclAttrFlagsInRAM_c, sizeof(uint8_t), (void *)MbrOfs(ZCLDRLCClientServerAttrsRAM_t, UtilityGroup) },
  { gZclAttrStartRandomizeMin_c,     gZclDataTypeUint8_c, gZclAttrFlagsInRAM_c, sizeof(uint8_t), (void *)MbrOfs(ZCLDRLCClientServerAttrsRAM_t, StartRandomizeMin)  },
  { gZclAttrStopRandomizeMin_c, gZclDataTypeUint8_c, gZclAttrFlagsInRAM_c, sizeof(uint8_t), (void *)MbrOfs(ZCLDRLCClientServerAttrsRAM_t, StopRandomizeMin) },
  { gZclAttrDevCls_c, gZclDataTypeUint16_c, gZclAttrFlagsInRAM_c | gZclAttrFlagsRdOnly_c, sizeof(uint16_t), (void *)MbrOfs(ZCLDRLCClientServerAttrsRAM_t, DevCls) }

};

const zclAttrDefList_t gZclDRLCClientServerClusterAttrDefList = {
  NumberOfElements(gaZclDRLCClientServerAttrDef),
  gaZclDRLCClientServerAttrDef
};


#if gInterPanCommunicationEnabled_c
/*  Handle the InterPan messages  */
pfnInterPanIndication_t pfnInterPanServerInd = NULL;
pfnInterPanIndication_t pfnInterPanClientInd = NULL;
#endif

/* Timers ID for DmndRspLdCtrl  */
tmrTimerID_t gLdLdCtrlTimerID, jitterTimerID;
/*Events Table Information*/
zclLdCtrl_EventsTableEntry_t gaEventsTable[gNumOfEventsTableEntry_c];
zclLdCtrl_EventsTableEntry_t gaServerEventsTable[gNumOfServerEventsTableEntry_c];
uint32_t mGetLdCtlEvtStartTime;
uint8_t mGetNumOfLdCtlEvts;
uint8_t mIndexLdCtl;



/********************************************************************************/

/******************************************************************************
*******************************************************************************
* Private memory declarations
*******************************************************************************
******************************************************************************/

/*Used to keep the new message(event)*/
static zclCmdDmndRspLdCtrl_LdCtrlEvtReq_t mNewLDCtrlEvt;
static afAddrInfo_t mNewAddrInfo;
static uint8_t mNewEntryIndex = 0xff, mReportStatusEntryIndex, mGetRandomFlag = TRUE;
static uint8_t mAcceptVoluntaryLdCrtlEvt = TRUE; /* Default Voluntary Load Control events are accepted */

/******************************************************************************
*******************************************************************************
* Public functions
*******************************************************************************
******************************************************************************/

/*****************************************************************************/

/******************************************************************************/

/*WARNING -remove this function*/
void SE_Init(void)
{
  
  
  /* ZCL Time Cluster starts at 01 Jan 2000 00:00:00 GMT, hence -946702800 offset to unix Epox 01 Jan 1970 00:00:00 GMT */
#define mTimeClusterTimeEpoxOffset_c 946702800
  
  /* 28 August 2008 12:00:00 GMT */
#define mDefaultValueOfTimeClusterTime_c 1219924800 - mTimeClusterTimeEpoxOffset_c
  
  /* Time Zone GMT+1 - Aarhus, Denmark */
#define mDefaultValueOfTimeClusterTimeZone_c 3600
  /* Start daylight savings 2008 for Aarhus, Denmark 30 March 2008 */
#define mDefaultValueOfTimeClusterDstStart_c 1206849600 - mTimeClusterTimeEpoxOffset_c
  /* End daylight savings 2008 for Aarhus, Denmark 26 October 2008 */
#define mDefaultValueOfTimeClusterDstEnd_c 1224993600 - mTimeClusterTimeEpoxOffset_c
  /* Daylight Saving + 1h */
#define mDefaultValueOfClusterDstShift_c 3600
  /* Valid Until Time - mDefaultValueOfTimeClusterTime_c + aprox 5 years */
#define mDefaultValueOfTimeClusterValidUntilTime_c mDefaultValueOfTimeClusterTime_c + 31536000
  
  {
    ZCLTimeConf_t defaultTimeConf;
    defaultTimeConf.Time = mDefaultValueOfTimeClusterTime_c;
    defaultTimeConf.TimeZone = mDefaultValueOfTimeClusterTimeZone_c;
    defaultTimeConf.DstStart = mDefaultValueOfTimeClusterDstStart_c;
    defaultTimeConf.DstEnd = mDefaultValueOfTimeClusterDstEnd_c;
    defaultTimeConf.DstShift = mDefaultValueOfClusterDstShift_c;
    defaultTimeConf.ValidUntilTime = mDefaultValueOfTimeClusterValidUntilTime_c;
    
    ZCL_TimeInit(&defaultTimeConf);
  }
  
#if gFragmentationCapability_d 
  {
    uint8_t aIncomingTransferSize[2] ={ gSEMaxIncomingTransferSize };
    FLib_MemCpy(&gBeeStackConfig.maxTransferSize[0], &aIncomingTransferSize[0], 2);
    ApsmeSetRequest(gApsMaxWindowSize_c, gSEMaxWindowSize_c);        // window can be 1 - 8
    ApsmeSetRequest(gApsInterframeDelay_c, gSEInterframeDelay_c);    // interframe delay can be 1 - 255ms
    ApsmeSetRequest(gApsMaxFragmentLength_c, gSEMaxFragmentLength_c);  // max len can be 1 - ApsmeGetMaxAsduLength()
  }
#endif /* #if gFragmentationCapability_d  */  
  
}

#if gInterPanCommunicationEnabled_c
/******************************************************************************/
void ZCL_RegisterInterPanClient(pfnInterPanIndication_t pFunc)
{
  pfnInterPanClientInd = pFunc;
}

/******************************************************************************/

void ZCL_RegisterInterPanServer(pfnInterPanIndication_t pFunc)
{
  pfnInterPanServerInd = pFunc;
}
#endif /* #if gInterPanCommunicationEnabled_c */


/******************************************************************************/
void ZCL_MsgInit(void)
{

  zbNwkAddr_t ESPAddr = {0x00,0x00};
  /*Set default msg cluster response addr.*/
  MsgResponseSourceAddr.msgAddrInfo.addrInfo.dstAddrMode = 0x02;
  Copy2Bytes(MsgResponseSourceAddr.msgAddrInfo.addrInfo.dstAddr.aNwkAddr, ESPAddr);
//  MsgResponseSourceAddr.dstEndPoint=gSendingNwkData.endPoint;
  Set2Bytes(MsgResponseSourceAddr.msgAddrInfo.addrInfo.aClusterId, gZclClusterMsg_c);
  MsgResponseSourceAddr.msgAddrInfo.addrInfo.srcEndPoint=appEndPoint;
  MsgResponseSourceAddr.msgAddrInfo.addrInfo.txOptions=afTxOptionsDefault_c;
  MsgResponseSourceAddr.msgAddrInfo.addrInfo.radiusCounter=afDefaultRadius_c;
  MsgResponseSourceAddr.msgControl = 0x00;
  gMsgDisplayTimerID = TMR_AllocateTimer();
  TMR_StartSecondTimer(gMsgDisplayTimerID, 1, MsgDisplayTimerCallBack);
  
}

/******************************************************************************/

void ZCL_LdCtrlClientInit(void)
{
  
  gLdLdCtrlTimerID = TMR_AllocateTimer();
  jitterTimerID = TMR_AllocateTimer();
} 


/******************************************************************************/




#if gInterPanCommunicationEnabled_c
/******************************************************************************/
/* Handle ALL Inter Pan Server\Client Indications (filter after cluster ID)   */
/******************************************************************************/
zbStatus_t ZCL_InterPanClusterServer
(
	zbInterPanDataIndication_t *pIndication, /* IN: */
	afDeviceDef_t *pDev                /* IN: */
) 
{
(void) pIndication;
(void) pDev;
  
#if(gASL_ZclPrice_InterPanPublishPriceRsp_d )    
if(IsEqual2BytesInt(pIndication->aClusterId, gZclClusterPrice_c))
 return ZCL_InterPanPriceClusterServer(pIndication, pDev); 
#endif

if(IsEqual2BytesInt(pIndication->aClusterId, gZclClusterMsg_c))
 return ZCL_InterPanMsgClusterServer(pIndication, pDev); 
   
/* if user uses Inter Pan with other cluster, add the Cluster ID filter here*/
   
 return gZclUnsupportedClusterCommand_c;
}

/******************************************************************************/
zbStatus_t ZCL_InterPanClusterClient
(
	zbInterPanDataIndication_t *pIndication, /* IN: */
	afDeviceDef_t *pDev                /* IN: */
) 
{
(void) pIndication;
(void) pDev;

#if(gASL_ZclPrice_InterPanGetCurrPriceReq_d )  
if(IsEqual2BytesInt(pIndication->aClusterId, gZclClusterPrice_c))
 return ZCL_InterPanPriceClusterClient(pIndication, pDev); 
#endif

 
if(IsEqual2BytesInt(pIndication->aClusterId, gZclClusterMsg_c))
 return ZCL_InterPanMsgClusterClient(pIndication, pDev); 

   
/* if user uses Inter Pan with other cluster, add the Cluster ID filter here*/
   
 return gZclUnsupportedClusterCommand_c;
}
#endif /* #if gInterPanCommunicationEnabled_c */


/******************************************************************************/
/* get address ready for InterPan reply */
void PrepareInterPanForReply(InterPanAddrInfo_t *pAdrrDest, zbInterPanDataIndication_t *pIndication)
{
  
  pAdrrDest->srcAddrMode = pIndication->srcAddrMode;
  pAdrrDest->dstAddrMode = pIndication->srcAddrMode;
  FLib_MemCpy(pAdrrDest->dstPanId, pIndication->srcPanId, sizeof(zbPanId_t));
  FLib_MemCpy(pAdrrDest->dstAddr.aIeeeAddr, pIndication->aSrcAddr.aIeeeAddr, sizeof(zbIeeeAddr_t));
  FLib_MemCpy(pAdrrDest->aProfileId, pIndication->aProfileId, sizeof(zbProfileId_t));
  FLib_MemCpy(pAdrrDest->aClusterId, pIndication->aClusterId, sizeof(zbClusterId_t));
}


/******************************************************************************/
/******************************************************************************/
/* The LC Event command is generated in response to receiving a Get Scheduled Events
command */
static zbStatus_t ZCL_SendLdCtlEvt(addrInfoType_t *pAddrInfo,zclLdCtrl_EventsTableEntry_t * pMsg) 
{
  zclDmndRspLdCtrl_LdCtrlEvtReq_t req;
  
  FLib_MemCpy(&req.addrInfo, pAddrInfo, sizeof(afAddrInfo_t));	
  req.zclTransactionId =  gZclTransactionId++;
  FLib_MemCpy(&req.cmdFrame, (uint8_t *)pMsg + MbrOfs(zclLdCtrl_EventsTableEntry_t, cmdFrame), sizeof(zclCmdDmndRspLdCtrl_LdCtrlEvtReq_t));
  return ZclDmndRspLdCtrl_LdCtrlEvtReq(&req);
}


/******************************************************************************/
void ZCL_HandleGetScheduledLdCtlEvts(void)
{
  uint32_t  startTime, nextStartTime = 0x00000000;
  uint8_t status;
  
  if(mIndexLdCtl < gNumOfServerEventsTableEntry_c)
  {
    /* Check if the entry is used and if there are more scheduled events to be send*/
    if((gaServerEventsTable[mIndexLdCtl].EntryStatus != 0x00) && mGetNumOfLdCtlEvts)
    {
      startTime = OTA2Native32(gaServerEventsTable[mIndexLdCtl].cmdFrame.StartTime);
      
      if (((mIndexLdCtl + 1) < gNumOfServerEventsTableEntry_c) && (gaServerEventsTable[mIndexLdCtl + 1].EntryStatus != 0x00))
        nextStartTime = OTA2Native32(gaServerEventsTable[mIndexLdCtl + 1].cmdFrame.StartTime);
      
      if ((mGetLdCtlEvtStartTime <= startTime) ||
          ((mGetLdCtlEvtStartTime > startTime) && (mGetLdCtlEvtStartTime < nextStartTime)))
      {
        /* Send This Load Control Event */
       	status = ZCL_SendLdCtlEvt(&mAddrInfo, &gaServerEventsTable[mIndexLdCtl]);
        if(status == gZclSuccess_c)
      	{
          --mGetNumOfLdCtlEvts;
      	}
        
      }
      /* GO and send the next Event */
      mIndexLdCtl++;
      TS_SendEvent(gZclTaskId, gzclEvtHandleGetScheduledLdCtlEvts_c);
    }
    
  }
}

/******************************************************************************/
static zbStatus_t ZCL_ProcessGetScheduledEvtsReq
(
addrInfoType_t *pAddrInfo, 
zclCmdDmndRspLdCtrl_GetScheduledEvts_t * pGetScheduledEvts
)
{
  FLib_MemCpy(&mAddrInfo, pAddrInfo, sizeof(addrInfoType_t));

  mGetLdCtlEvtStartTime = OTA2Native32(pGetScheduledEvts->EvtStartTime);
  mGetNumOfLdCtlEvts = pGetScheduledEvts->NumOfEvts;
  if (!mGetNumOfLdCtlEvts)
  {
    mGetNumOfLdCtlEvts = gNumOfServerEventsTableEntry_c;
    mGetLdCtlEvtStartTime = 0x00000000; /*all information*/
  }
  mIndexLdCtl = 0;
  TS_SendEvent(gZclTaskId, gzclEvtHandleGetScheduledLdCtlEvts_c);
  return gZclSuccess_c;
}
/*****************************************************************************/
/*****************************************************************************/

/******************************************************************************/
/* Create and Send the Report Event Status OTA...                             */ 
/*****************************************************************************/
zbStatus_t ZCL_SendReportEvtStatus
(
afAddrInfo_t *pAddrInfo, 
zclCmdDmndRspLdCtrl_LdCtrlEvtReq_t *pMsg, 
uint8_t eventStatus, 
bool_t invalidValueFlag /* if TRUE sent RES with invalid values for fields */
)
{
  zclDmndRspLdCtrl_ReportEvtStatus_t req;
  zclCmdDmndRspLdCtrl_ReportEvtStatus_t *pCmdFrame;
  uint32_t currentTime;
  Signature_t signature;
  
  pCmdFrame = &(req.cmdFrame);
  /* Prepare the RES */
  FLib_MemCpy(&req, pAddrInfo, sizeof(afAddrInfo_t));
  req.zclTransactionId = gZclTransactionId++;
  FLib_MemCpy(pCmdFrame->IssuerEvtID, pMsg->IssuerEvtID, sizeof(SEEvtId_t));
  pCmdFrame->EvtStatus = eventStatus; 
  /* here get the currentTime  */
  currentTime = ZCL_GetUTCTime();

  currentTime = Native2OTA32(currentTime);
  pCmdFrame->EvtStatusTime = currentTime;
  if(!invalidValueFlag)
  {
    pCmdFrame->CritLevApplied = pMsg->CritLev;
    FLib_MemCpy(&(pCmdFrame->CoolTempSetPntApplied), 
                &(pMsg->CoolingTempSetPoint), (2 + sizeof(int8_t) + 2*sizeof(LCDRSetPoint_t)) );
    
  }
  else
  {
    /* set invalid values */
    pCmdFrame->CritLevApplied = 0x00; 
    pCmdFrame->CoolTempSetPntApplied = Native2OTA16(gZclCmdDmndRspLdCtrl_OptionalTempSetPoint_c); 	
    pCmdFrame->HeatTempSetPntApplied = Native2OTA16(gZclCmdDmndRspLdCtrl_OptionalTempSetPoint_c);
    pCmdFrame->AverageLdAdjustmentPercentage = gZclCmdDmndRspLdCtrl_OptionalAverageLdAdjustmentPercentage_c;
    pCmdFrame->DutyCycle = gZclCmdDmndRspLdCtrl_OptionalDutyCycle_c;
    pCmdFrame->EvtCtrl =0x00;
  }
  pCmdFrame->SignatureType = gSELCDR_SignatureType_c;
  ZCL_ApplyECDSASign((uint8_t*)pCmdFrame, (sizeof(zclCmdDmndRspLdCtrl_ReportEvtStatus_t)-sizeof(Signature_t)), (uint8_t*)&signature);
  FLib_MemCpy(&(pCmdFrame->Signature[0]), &signature, sizeof(Signature_t));
  /* Send the RES over the air */
 return ZclDmndRspLdCtrl_ReportEvtStatus(&req);
}

/******************************************************************************/
/* Find an Event in the Events Table...                                                                                   */
uint8_t FindEventInEventsTable(uint8_t *pEvtId)
{
  uint8_t index; 
  zclLdCtrl_EventsTableEntry_t *pEntry = &gaEventsTable[0];
  for(index = 0; index < gNumOfEventsTableEntry_c; index++)
  {  
    /* if not used go to next entry*/  
    if(((zclLdCtrl_EventsTableEntry_t *)(pEntry+index))->EntryStatus == gEntryUsed_c)
    {
      if(FLib_MemCmp(((zclLdCtrl_EventsTableEntry_t *)(pEntry+index))->cmdFrame.IssuerEvtID, pEvtId, sizeof(SEEvtId_t)))
        return index; 
    }
  }
  return 0xFF; 
  
}   

/******************************************************************************/
/* An Entry is empty if the EntryStatus is NOT Used or NotSupersededClass is 0x0000 (all class bits was overlapped)*/
static uint8_t FindEmptyEntryInEventsTable(void)
{
  uint8_t index;
  zclLdCtrl_EventsTableEntry_t *pEntry;
  for(index = 0; index < gNumOfEventsTableEntry_c; index++)
  {  
    pEntry = &gaEventsTable[index];
    /* if not used go to next entry*/  
    if(pEntry->EntryStatus == gEntryNotUsed_c)
      return index;
    else /*if class is superseded, use that entry*/
      if (pEntry->NotSupersededClass == 0x0000 &&
          pEntry->SupersededTime == 0x00000000 )
        return index;
  }
        
  /*if all entries are used and the class bits wasn't overlaped, we have NO free entry in table*/
  /* keep the same entries */
  for(index = 0; index < gNumOfEventsTableEntry_c; index++)
  { 
    pEntry = &gaEventsTable[index];
    if(pEntry->EntryStatus == gEntryUsed_c)
    {
      pEntry->NotSupersededClass = 0xffff;
      pEntry->SupersededTime = 0xffffffff;
    }
  }
  return 0xff;
}  

/******************************************************************************/
/* An Entry is empty if the EntryStatus is NOT Used*/
static uint8_t FindEmptyEntryInServerEventsTable(void)
{
  uint8_t index;
  zclLdCtrl_EventsTableEntry_t *pEntry;
  for(index = 0; index < gNumOfServerEventsTableEntry_c; index++)
  {  
    pEntry = &gaServerEventsTable[index];
    /* if not used go to next entry*/  
    if(pEntry->EntryStatus == gEntryNotUsed_c)
      return index;
   }
  return 0xff;
}  

/******************************************************************************/
/* Add a new Event in the EventsTable */
static void AddNewEntry
(
zclLdCtrl_EventsTableEntry_t *pDst, 
zclCmdDmndRspLdCtrl_LdCtrlEvtReq_t *pSrc,
afAddrInfo_t *pAddrInfo
)
{
  FLib_MemCpy(&(pDst->addrInfo), pAddrInfo, sizeof(afAddrInfo_t));
  FLib_MemCpy(&(pDst->cmdFrame), pSrc, sizeof(zclCmdDmndRspLdCtrl_LdCtrlEvtReq_t));
  pDst->CurrentStatus = gSELCDR_LdCtrlEvtCode_CmdRcvd_c;
  pDst->NextStatus = gSELCDR_LdCtrlEvtCode_Started_c;
  pDst->IsSuccessiveEvent = FALSE;
  /*Set  fields as beeing invalid... not used yet*/
  BeeUtilSetToF(&(pDst->CancelTime), (2*sizeof(ZCLTime_t) + 2*sizeof(LCDRDevCls_t) +1));
  pDst->EntryStatus = gEntryUsed_c;
}

/******************************************************************************/
static void CheckForSuccessiveEvents(uint8_t msgIndex)
{
  zclCmdDmndRspLdCtrl_LdCtrlEvtReq_t *pEvent;
  zclLdCtrl_EventsTableEntry_t *pEntry,*pMsg;
  uint32_t msgStartTime, entryStartTime ;
  uint32_t msgStopTime, entryStopTime;
  uint16_t msgDuration, entryDuration; 
  uint8_t index;
  /* point to message */
  pMsg = &gaEventsTable[msgIndex];
  msgDuration = OTA2Native16(pMsg->cmdFrame.DurationInMinutes);
  msgStartTime = OTA2Native32(pMsg->cmdFrame.StartTime);
  msgStopTime = msgStartTime + ((uint32_t)msgDuration * 60);
  for(index = 0; index < gNumOfEventsTableEntry_c; index++)
  {  
    pEntry = &gaEventsTable[index];
    /* if not used go to next entry*/  
    if((pEntry->EntryStatus == gEntryNotUsed_c)||(index == msgIndex) )
      continue;
    /* point to entry */
    pEvent = (zclCmdDmndRspLdCtrl_LdCtrlEvtReq_t *)&pEntry->cmdFrame;
    /* check if the class is overlaping */
    if (!(pEvent->DevGroupClass & pMsg->cmdFrame.DevGroupClass))
      continue;	
    /* get the entry timing */
    entryDuration = OTA2Native16(pEvent->DurationInMinutes);
    entryStartTime = OTA2Native32(pEvent->StartTime);
    entryStopTime = entryStartTime + ((uint32_t)entryDuration * 60);
    /*Check if the new event is a succesive event */
    if ((msgStartTime == entryStopTime)||(entryStartTime == msgStopTime))
    {
      if(!pEntry->IsSuccessiveEvent)
        pEntry->IsSuccessiveEvent = TRUE;
      if(!pMsg->IsSuccessiveEvent)
        pMsg->IsSuccessiveEvent = TRUE;
    } 
  }/* end for(...) */
  
}


/******************************************************************************/
/* Accepting voluntary load control events */
void ZCL_AcceptVoluntaryLdCrtlEvt(bool_t flag)
{
  mAcceptVoluntaryLdCrtlEvt = flag;
}


/******************************************************************************/
/* Update the load contol table of events...
Check if the event can be handled(to set it having received status). 
An event is considered received if it is added in the EventsTable[], superseding or Not one or more events.
the received event should be filtered (Class, Utility) and the overlapping rules should be checked
*/ 
/*****************************************************************************/
static uint8_t ZCL_ScheduleEvents(afAddrInfo_t *pAddrInfo, zclCmdDmndRspLdCtrl_LdCtrlEvtReq_t *pMsg)
{
  uint8_t status = gZclSuccess_c;
  uint8_t utilityGroup;
  uint16_t devGroupClass, entryClass;
  uint32_t currentTime;
  uint32_t msgStartTime, entryStartTime ;
  uint32_t msgStopTime, entryStopTime, OTATime;
  uint16_t msgDuration, entryDuration;  
  zclCmdDmndRspLdCtrl_LdCtrlEvtReq_t *pEvent;
  zclLdCtrl_EventsTableEntry_t *pEntry;
  uint8_t index;
  //uint8_t attrData[4];
  uint8_t attrLen;
  
  /* the previous received event is still not added in the Table Entry */
  if (mNewEntryIndex != 0xff)
    return status;
  (void)ZCL_GetAttribute(pAddrInfo->srcEndPoint,pAddrInfo->aClusterId, gZclAttrUtilityGroup_c, &utilityGroup, &attrLen);
  (void)ZCL_GetAttribute(pAddrInfo->srcEndPoint,pAddrInfo->aClusterId, gZclAttrDevCls_c, &devGroupClass, &attrLen);
  
  /* Filter the device Class and Utility (if no match, no RES is send)*/
  pMsg->DevGroupClass = pMsg->DevGroupClass & devGroupClass;
  if((!pMsg->DevGroupClass) || ((pMsg->UtilityGroup != utilityGroup) && (pMsg->UtilityGroup != 0)))
    return gZclInvalidField_c; /* NO RES is send */

  /* Check the criticality Level */
  if((pMsg->CritLev > gZclCmdDmndRspLdCtrl_MaxCritLev_c )||
     (pMsg->CritLev >= gZclSECritLev_Green_c && pMsg->CritLev <= gZclSECritLev_5_c && !mAcceptVoluntaryLdCrtlEvt))
       return gZclInvalidField_c; /* NO RES is send */
  
  /* Check for duplicate event */
  if(FindEventInEventsTable((uint8_t*)&(pMsg->IssuerEvtID)) != 0xff)
    return gZclDuplicateExists_c; 
  
  /* Check event with the End Time in the past (Event Status set to 0xFB) */
  msgDuration = OTA2Native16(pMsg->DurationInMinutes);
  msgStartTime = OTA2Native32(pMsg->StartTime);
  
 /* here get the currentTime  */ 
  currentTime = ZCL_GetUTCTime();
  /*Event is started now???*/
  if(msgStartTime == 0x00000000)
  {
    /*Set the start time for the new event(keep the current time) */
    msgStartTime = currentTime;
    /*set it OTA value */
    OTATime = Native2OTA32(currentTime);
    pMsg->StartTime = OTATime;
  }
  msgStopTime = msgStartTime + ((uint32_t)msgDuration * 60);
  /* Event had expired?? */
  if (msgStopTime <= currentTime)
  {
    status = ZCL_SendReportEvtStatus(pAddrInfo, pMsg, gSELCDR_LdCtrlEvtCode_EvtHadExpired_c, FALSE);
    return status; 
  }
  
  /* Check the overlapping rules for scheduled and not executing events. */
  /* After the Report Event Status command is successfully sent, the End Device can remove the
  previous event schedule.(  Event can be received??...Can be added in the Events Table??... table is not full??).
  No RES will be send if table is full )
  */
  for(index = 0; index < gNumOfEventsTableEntry_c; index++)
  {  
    pEntry = &gaEventsTable[index];
    /* if not used go to next entry*/ 
    if(pEntry->EntryStatus == gEntryNotUsed_c)
      continue;
    /* point to entry */
    pEvent = (zclCmdDmndRspLdCtrl_LdCtrlEvtReq_t *)&pEntry->cmdFrame;
    /* check if the class is overlaping */
    entryClass = pEvent->DevGroupClass; 
    if (!(entryClass & pMsg->DevGroupClass))
      continue; 
    /* get the entry timing */
    entryDuration = OTA2Native16(pEvent->DurationInMinutes);
    entryStartTime = OTA2Native32(pEvent->StartTime);
    entryStopTime = entryStartTime + ((uint32_t)entryDuration * 60);
    
    /*Check if the new event and the entry event are overlaping */
    if ((msgStartTime >= entryStopTime)||(entryStartTime >= msgStopTime))
      continue;
    
    /* Here the entry of the table is Overlapped */
    /* overlap only the specified class */
    /* supersede the original event strictly for that device class */
    entryClass = entryClass & (entryClass^pMsg->DevGroupClass);
    /*if NotSupersededClass is 0x0000, all the class bits was overlapped (event will be superseded)*/
    pEntry->NotSupersededClass = entryClass;
    
    /* resolve the case when the running event is superseded with a event that will start later*/
    if((entryStartTime <= currentTime) && (msgStartTime > currentTime))
      OTATime = Native2OTA32(msgStartTime);
    else
      OTATime = 0x00000000;
    pEntry->SupersededTime =OTATime;
    
  } /* end for(index = 0; index...*/
  
  /* Find an empty entry to schedule it */  
  mNewEntryIndex = FindEmptyEntryInEventsTable();
  if(mNewEntryIndex != 0xff)
  { /* keep the new event info */
    FLib_MemCpy(&mNewLDCtrlEvt, pMsg, sizeof(zclCmdDmndRspLdCtrl_LdCtrlEvtReq_t));
    FLib_MemCpy(&mNewAddrInfo,pAddrInfo, sizeof(mNewAddrInfo));
    /* Send RES with Receiving Status, but the event is not added in the Events Table yet */
    status = ZCL_SendReportEvtStatus(&mNewAddrInfo, &mNewLDCtrlEvt, gSELCDR_LdCtrlEvtCode_CmdRcvd_c, FALSE);
  }
  else	
    return gZclFailure_c; /* event can't be load (or added in table) */
  
  
  if(status == gZclSuccess_c)
    TS_SendEvent(gZclTaskId, gZclEvtHandleLdCtrl_c);  
  return status;
}

/******************************************************************************/
/* Process the Cancel Load Control Event command                                                            */ 
/*****************************************************************************/
static zbStatus_t ZCL_ProcessCancelLdCtrlEvt(zbApsdeDataIndication_t *pIndication)
{

  uint8_t status = gZclSuccess_c;
  zclCmdDmndRspLdCtrl_CancelLdCtrlEvtReq_t *pMsg;
  zclLdCtrl_EventsTableEntry_t *pEntry;
  uint8_t index;
  afAddrInfo_t addrInfo;
  uint32_t msgCancelTime, entryStopTime, currentTime;
  uint16_t duration;
  uint8_t utilityGroup;
  uint8_t attrLen;

  /* get address ready for reply */
  AF_PrepareForReply(&addrInfo, pIndication); 
  pMsg = (zclCmdDmndRspLdCtrl_CancelLdCtrlEvtReq_t *)((uint8_t*)pIndication->pAsdu +sizeof(zclFrame_t));
  (void)ZCL_GetAttribute(addrInfo.srcEndPoint,addrInfo.aClusterId, gZclAttrUtilityGroup_c, &utilityGroup, &attrLen);

  if((pMsg->UtilityGroup != utilityGroup) && (pMsg->UtilityGroup != 0))
  {
    /* send RES (InvalidCancelCmdDefault) with invalid values*/
    status = ZCL_SendReportEvtStatus(&addrInfo,(zclCmdDmndRspLdCtrl_LdCtrlEvtReq_t *)pMsg, gSELCDR_LdCtrlEvtCode_EvtInvalidCancelCmdDefault_c, TRUE);
    return status; 
  }
  /* Find the entry table event */
  index = FindEventInEventsTable(pMsg->IssuerEvtID);
  if(index == 0xff)
  {
    /* send RES (undef status ) with invalid values*/
    status = ZCL_SendReportEvtStatus(&addrInfo,(zclCmdDmndRspLdCtrl_LdCtrlEvtReq_t *)pMsg, gSELCDR_LdCtrlEvtCode_EvtUndef_c, TRUE);
    return status; 
  }
  else
  {
  pEntry = &gaEventsTable[index];
  pMsg->DevGroupClass &= pEntry->cmdFrame.DevGroupClass;
  }
  
  /* class is not matching; take NO action */
  if(!pMsg->DevGroupClass)
    return status;
  
  msgCancelTime = OTA2Native32(pMsg->EffectiveTime);
  /* here get the currentTime */
  currentTime = ZCL_GetUTCTime();
  
  /*Event is Canceled Now???*/
  if(msgCancelTime == 0x00000000)
  {
    msgCancelTime = currentTime;
    currentTime = Native2OTA32(currentTime);
    pMsg->EffectiveTime = currentTime;
  }
  
  entryStopTime = OTA2Native32(pEntry->cmdFrame.StartTime);
  duration = OTA2Native16(pEntry->cmdFrame.DurationInMinutes);
  entryStopTime = entryStopTime + ((uint32_t)duration * 60);
  
  if(entryStopTime <= msgCancelTime)
  {
    /* send RES (invalid effective time) with invalid values */
    status = ZCL_SendReportEvtStatus(&addrInfo,(zclCmdDmndRspLdCtrl_LdCtrlEvtReq_t *)pMsg, gSELCDR_LdCtrlEvtCode_EvtInvalidEffectiveTime_c, TRUE);
	return status; 
  }
  /* Set the fields information and let the ZCL_HandleScheduledEventNow() to  handle it */
  pEntry->CancelCtrl = pMsg->CancelCtrl; /* Set the Cancel Control */
  pEntry->CancelClass = pMsg->DevGroupClass; /*for what class the event is canceled*/  
  pEntry->CancelTime = pMsg->EffectiveTime; /* set cancel time */
  
  if (status == gZclSuccess_c)
       TS_SendEvent(gZclTaskId, gZclEvtHandleLdCtrl_c);  
  
  return status;
  
}



/******************************************************************************/
/* Process the Cancel All Load Control Event command...                                */ 
/*****************************************************************************/
static zbStatus_t ZCL_ProcessCancelAllLdCtrlEvtst(zbApsdeDataIndication_t * pIndication)
{
  
  uint8_t status;
  zclCmdDmndRspLdCtrl_CancelAllLdCtrlEvtsReq_t *pMsg;
  zclLdCtrl_EventsTableEntry_t *pEntry;
  uint8_t index;
  
  pMsg = (zclCmdDmndRspLdCtrl_CancelAllLdCtrlEvtsReq_t *)((uint8_t*)pIndication->pAsdu +sizeof(zclFrame_t));
  status = gZclFailure_c;
  for(index = 0; index < gNumOfEventsTableEntry_c; index++)
  {  
    pEntry = &gaEventsTable[index];
    /* if not used go to next entry*/  
    if(pEntry->EntryStatus == gEntryUsed_c)
    {
      pEntry->CancelCtrl = pMsg->CancelCtrl; 
      pEntry->CancelClass = 0xffff; /*invalid... not used */ 
      pEntry->CancelTime = 0x00000000; /* cancel now */
      status = gZclSuccess_c; 
    }
  }
  
  if (status == gZclSuccess_c)
        TS_SendEvent(gZclTaskId, gZclEvtHandleLdCtrl_c);  
  
  return status;

}

/******************************************************************************/
/*  Timer Callback that handles the scheduled Events */ 
static void LdCtrlEvtTimerCallback(tmrTimerID_t timerID)
{
   (void) timerID;
   TS_SendEvent(gZclTaskId, gZclEvtHandleLdCtrl_c);  
}


/******************************************************************************/
/* LdCtrlJitterTimerCallBack()     */ 
/*****************************************************************************/
void LdCtrlJitterTimerCallBack(tmrTimerID_t timerID)
{
  zclLdCtrl_EventsTableEntry_t *pEntry;
  uint8_t status, previousEntryStatus;
  
  pEntry = (zclLdCtrl_EventsTableEntry_t*)&gaEventsTable[mReportStatusEntryIndex];
  if (pEntry->EntryStatus == gEntryNotUsed_c)
    return;
  
  /*save the previuos status */
  previousEntryStatus = pEntry->CurrentStatus; 
  
  /* Check if the event is already started and an "OptOUT" status was sent */
  if((previousEntryStatus == gSELCDR_LdCtrlEvtCode_UserHaveToChooseOptOut_c)&&
     ((pEntry->NextStatus == gSELCDR_LdCtrlEvtCode_Completed_c)||
      (pEntry->NextStatus == gSELCDR_LdCtrlEvtCode_EvtCompletedWithNoUser_c)) )
    pEntry->NextStatus = gSELCDR_LdCtrlEvtCode_EvtPrtlCompletedWithUserOptOut_c;
  /* check if an "OptIn" status was sent previous
  and the event was started or already had been started */
  if((previousEntryStatus == gSELCDR_LdCtrlEvtCode_UserHaveToChooseOptIn_c)&&
     (pEntry->NextStatus == gSELCDR_LdCtrlEvtCode_Completed_c))
    pEntry->NextStatus = gSELCDR_LdCtrlEvtCode_EvtPrtlCompletedWithUserOptIn_c;
  
  /*check if the event was started and before it was sent an "OptOut" status (to set the next status EvtCompletedWithNoUser) */
  if((previousEntryStatus == gSELCDR_LdCtrlEvtCode_UserHaveToChooseOptOut_c)&&
     (pEntry->NextStatus == gSELCDR_LdCtrlEvtCode_Started_c))
    pEntry->NextStatus = gSELCDR_LdCtrlEvtCode_EvtCompletedWithNoUser_c;
  
  status = ZCL_SendReportEvtStatus((afAddrInfo_t *)pEntry, &pEntry->cmdFrame, pEntry->NextStatus, FALSE);
  if (status == gZclSuccess_c)
  {
    /* set the current status (the Report Evt Status was sent)*/
    pEntry->CurrentStatus = pEntry->NextStatus;
    
    /* Call the BeeAppUpdateDevice */
    /* user should check the CurrentStatus of the Event */
    BeeAppUpdateDevice(((afAddrInfo_t *)pEntry)->srcEndPoint, gZclUI_LdCtrlEvt_c, 0, ((afAddrInfo_t *)pEntry)->aClusterId, pEntry); 
    
    if(pEntry->CurrentStatus == gSELCDR_LdCtrlEvtCode_Started_c)
      pEntry->NextStatus = gSELCDR_LdCtrlEvtCode_Completed_c;
    else
      if((pEntry->CurrentStatus == gSELCDR_LdCtrlEvtCode_Completed_c)||
         (pEntry->CurrentStatus == gSELCDR_LdCtrlEvtCode_EvtCompletedWithNoUser_c)||
           (pEntry->CurrentStatus == gSELCDR_LdCtrlEvtCode_EvtPrtlCompletedWithUserOptIn_c)||
             (pEntry->CurrentStatus == gSELCDR_LdCtrlEvtCode_EvtPrtlCompletedWithUserOptOut_c)||
               (pEntry->CurrentStatus == gSELCDR_LdCtrlEvtCode_EvtCancelled_c)||
                 (pEntry->CurrentStatus == gSELCDR_LdCtrlEvtCode_EvtSuperseded_c) )
        pEntry->EntryStatus = gEntryNotUsed_c;
    
    /* check if an "OptIn" status was sent previous
    and the event was started or already had been started */
    if((previousEntryStatus == gSELCDR_LdCtrlEvtCode_UserHaveToChooseOptIn_c)&&
       (pEntry->CurrentStatus == gSELCDR_LdCtrlEvtCode_Started_c) )
      pEntry->NextStatus = gSELCDR_LdCtrlEvtCode_EvtPrtlCompletedWithUserOptIn_c;
    
  }/* end  if (status == gZclSuccess_c) */
  else
  {
    /* Try again later */
    TMR_StartSingleShotTimer (timerID, 10, LdCtrlJitterTimerCallBack);
    return;
  }
  /* Handle the next Load Control Event*/             
  TS_SendEvent(gZclTaskId, gZclEvtHandleLdCtrl_c); 
}

/******************************************************************************/
/* Handle the Scheduled events(find the nerest and shortest event that has to ocur...   )          */ 
/*****************************************************************************/
void ZCL_HandleReportEventsStatus(void)
{
  uint16_t randomTime;
  /* The timer should be allocated */
  if(!jitterTimerID)
    return;
  
  if(!TMR_IsTimerActive(jitterTimerID))
  { 
    /* Delayed the Report Status Event after a random delay between 0 and 5 seconds, to avoid a potential storm of
    packets; this is done if a timer can be allocated */ 
    /* Get a random time and send the Report Status Event */
    randomTime = (uint16_t)GetRandomRange(10, 50);
    TMR_StartSingleShotTimer(jitterTimerID, randomTime, LdCtrlJitterTimerCallBack); 
  }
  else
  {
    /* handle next time */
    TS_SendEvent(gZclTaskId, gZclEvtHandleLdCtrl_c); 
  }
}

/******************************************************************************/
/* Handle the Scheduled events(find the nerest and shortest event that has to ocur...   )          */ 
/*****************************************************************************/

void ZCL_HandleScheduledEventNow(void)
{
  static uint8_t startRandomize, stopRandomize;
  uint32_t currentTime;
  uint16_t cancelClass, notSupersededClass;
  uint32_t entryStartTime, entryStopTime, cancelTime, minTiming = 0xffffffff, supersededTime;
  uint16_t entryDuration;  
  zclCmdDmndRspLdCtrl_LdCtrlEvtReq_t *pEvent;
  zclLdCtrl_EventsTableEntry_t *pEntry;
  uint8_t noEntryFlag = FALSE;
  uint8_t index, randomTime;
  uint8_t attrLen;
  
  /* If a new entry was received, add it in the table if the entry is not used; 
     if it is used then supersede it first  */
  if(mNewEntryIndex != 0xff)
  {
    zclLdCtrl_EventsTableEntry_t *pNewEntry = &gaEventsTable[mNewEntryIndex];
    if (pNewEntry->EntryStatus == gEntryNotUsed_c)
    {
      /* add the new event and check it for succesive events */
      AddNewEntry(pNewEntry, &mNewLDCtrlEvt, &mNewAddrInfo);
      CheckForSuccessiveEvents(mNewEntryIndex);
      /* Call the App to signal that an Load Control Event was received. The user should check the Current Status */
      BeeAppUpdateDevice(mNewAddrInfo.srcEndPoint, gZclUI_LdCtrlEvt_c, 0, mNewAddrInfo.aClusterId, pNewEntry); 
      /* if a new event is received get the randomize timing */
      mGetRandomFlag = TRUE;
      /* New events can be received */
      mNewEntryIndex = 0xff;
      /* Handle the next Load Control Event*/             
      TS_SendEvent(gZclTaskId, gZclEvtHandleLdCtrl_c); 
      return;

    }
    else
    {
      /*all Class bits are overlapped; supersede the entry */
      pNewEntry->NextStatus = gSELCDR_LdCtrlEvtCode_EvtSuperseded_c;
      mReportStatusEntryIndex =mNewEntryIndex;
      /* send the RES */
      TS_SendEvent(gZclTaskId, gzclEvtHandleReportEventsStatus_c);
      return;
    }
    
  }
    /*
    Randomization:
    - maintain its current state in the random period...
    - Check Effective End Time overlaps the Effective Start Time of a Successive Event, the Effective Start Time takes precedence
    - Devices shall not artificially create a gap between Successive Events
    ( use Start < Stop Randomization to prevent this)
    - It is permissible to have gaps when events are not Successive Events or Overlapping Events
    */
    if(mGetRandomFlag)
    {
      mGetRandomFlag = FALSE;
      (void)ZCL_GetAttribute(mNewAddrInfo.srcEndPoint, mNewAddrInfo.aClusterId, gZclAttrStartRandomizeMin_c, &startRandomize, &attrLen);
      (void)ZCL_GetAttribute(mNewAddrInfo.srcEndPoint, mNewAddrInfo.aClusterId, gZclAttrStopRandomizeMin_c, &stopRandomize, &attrLen);
      if(startRandomize)
        startRandomize = GetRandomRange(0, startRandomize);
      if(stopRandomize)
        stopRandomize = GetRandomRange(0, stopRandomize);
    }
  
    /* here get the currentTime  */ 
    currentTime = ZCL_GetUTCTime();
    for(index = 0; index < gNumOfEventsTableEntry_c; index++)
    { 
      pEntry = &gaEventsTable[index];
      /* if not used go to next entry*/  
      if(pEntry->EntryStatus == gEntryNotUsed_c)
        continue;
	  /* there are entries in table(not all entries had expired) */
      noEntryFlag = TRUE;
      /* point to event */
      pEvent = (zclCmdDmndRspLdCtrl_LdCtrlEvtReq_t *)&pEntry->cmdFrame;

/* Handle the supersed time */
      /*get the NotSupersededClass */
      notSupersededClass = pEntry->NotSupersededClass;
      /* Event is superseded???*/
      if(notSupersededClass != 0xffff)
      {
        /*All class bits are superseded???*/
        if(notSupersededClass == 0x0000)
        { 
          supersededTime = OTA2Native32(pEntry->SupersededTime);
          if(supersededTime <= currentTime)
          {
            /*all Class bits are overlapped; supersede the entry */
            pEntry->NextStatus = gSELCDR_LdCtrlEvtCode_EvtSuperseded_c;
            mReportStatusEntryIndex =index;
            /* send the RES */
            TS_SendEvent(gZclTaskId, gzclEvtHandleReportEventsStatus_c);
            return;
          }
        }          
        else 
        {
          /*Keep the entry with the class bits that was not superseded */  
          pEntry->cmdFrame.DevGroupClass = notSupersededClass;
          pEntry->NotSupersededClass = 0xffff;
        }  
      }

/* Handle the cancel time */
      /* get the timing */
      cancelTime = OTA2Native32(pEntry->CancelTime);
      /* Event is Canceled??? */
      if (cancelTime != 0xffffffff)
      {
        /*Is it using randomization???*/
        if(pEntry->CancelCtrl & gCancelCtrlEndRandomization_c)
          randomTime = stopRandomize;
        else
          randomTime = 0;
        /* Check if the cmd have to execute */
        cancelTime = cancelTime + (60 * (uint32_t)randomTime); /*random time is in minutes*/
        if(cancelTime <= currentTime)
        {
          cancelClass= pEntry->CancelClass;
          /*Cancel only the specified class*/
          notSupersededClass = pEvent->DevGroupClass & (cancelClass^pEvent->DevGroupClass);
          /*check if the Class is specified (CancelAll cmd don't specifies the class)*/
          if((cancelClass != 0xffff) && notSupersededClass)
          {           
            /*Keep the entry that wasn't Overlapped */
            pEvent->DevGroupClass = notSupersededClass;
            /*Set Cancel fields as beeing invalid... not used yet*/
            BeeUtilSetToF(&(pEntry->CancelTime), (sizeof(ZCLTime_t) + sizeof(LCDRDevCls_t) +1));
          }
          else
          {
            mReportStatusEntryIndex =index;
            pEntry->NextStatus = gSELCDR_LdCtrlEvtCode_EvtCancelled_c;
            TS_SendEvent(gZclTaskId, gzclEvtHandleReportEventsStatus_c);
            return;
          }          
        }/* if(cancelTime  <= currentTime)...*/
        else /* else      if(cancelTime <= currentTime)...*/
        {
          if( minTiming > cancelTime)
            minTiming = cancelTime;
        } 
      }/* if (cancelTime != 0xffffffff)...*/

/* Handle the start time and stop time */
      entryDuration = OTA2Native16(pEvent->DurationInMinutes);
      entryStartTime = OTA2Native32(pEvent->StartTime);
      entryStopTime = entryStartTime + ((uint32_t)entryDuration * 60);
      
      /*Is it using randomization???*/
      if(pEvent->EvtCtrl & 0x01)
      {
        /* If it is an succesive event, use the shortest random time for entryStartTime (to avoid gaps)*/
        if(pEntry->IsSuccessiveEvent)
          randomTime = (startRandomize <= stopRandomize)?startRandomize:stopRandomize;
        else
          randomTime = startRandomize;
      }
      else
        randomTime = 0;
      entryStartTime = entryStartTime + (60 * (uint32_t)randomTime); /*random time is in minutes*/
      
      if(pEvent->EvtCtrl & 0x02)
        randomTime = stopRandomize;
      else
        randomTime = 0;    
      entryStopTime = entryStopTime + (60 * (uint32_t)randomTime); /*random time is in minutes*/
      /* Check if the cmd have to execute */
      if( ((entryStartTime <= currentTime)&&(pEntry->NextStatus == gSELCDR_LdCtrlEvtCode_Started_c))||
         (entryStopTime <= currentTime) )
      {
        /* Send the RES */
        mReportStatusEntryIndex = index;
        TS_SendEvent(gZclTaskId, gzclEvtHandleReportEventsStatus_c);
        return; 
      }
      
      if(entryStartTime > currentTime)
      {
        if( minTiming > entryStartTime)
          minTiming = entryStartTime;
      }
      else
        if(entryStopTime > currentTime)
        {
          if( minTiming > entryStopTime)
            minTiming = entryStopTime;
        } 
    } /* end  for(index = 0; index < gNumOfEventsTableEntry_c; */
    
    /* Get Random Timing... next time when ZCL_HandleScheduledEventNow() is called  */
    mGetRandomFlag = TRUE;
    if(!noEntryFlag || (minTiming == 0xffffffff))
      TMR_StopSecondTimer (gLdLdCtrlTimerID);
    else{
      TMR_StartSecondTimer (gLdLdCtrlTimerID, (uint16_t)(minTiming - currentTime), LdCtrlEvtTimerCallback);
    }
}


/*****************************************************************************
  ZCL_SetOptStatusOnEvent

  Set "Opt In" or "Opt Out" Status for an event Id.
*****************************************************************************/
uint8_t ZCL_SetOptStatusOnEvent(uint8_t *pEvtId, uint8_t optStatus) 
{
  uint8_t index;
  afAddrInfo_t *pAddrInfo;
  
  if(!pEvtId) 
  {
    return 0xff;
  }
  
  index = FindEventInEventsTable(pEvtId);
  if(index == 0xff) { 
    return 0xff;
  }
  
  pAddrInfo =  (afAddrInfo_t *)(&gaEventsTable[index]);
  /* Set the CurrentStatus */
  gaEventsTable[index].CurrentStatus =  optStatus;
  (void)ZCL_SendReportEvtStatus(pAddrInfo, (zclCmdDmndRspLdCtrl_LdCtrlEvtReq_t *)&(gaEventsTable[index].cmdFrame), optStatus, FALSE);
  BeeAppUpdateDevice(pAddrInfo->srcEndPoint, gZclUI_LdCtrlEvt_c, 0, pAddrInfo->aClusterId, &gaEventsTable[index]);
  return index;
}

/*****************************************************************************
  ZCL_DeleteLdCtrlEvent()

  Set "Opt In" or "Opt Out" Status for an event Id.
*****************************************************************************/
uint8_t ZCL_DeleteLdCtrlEvent(uint8_t *pEvtId) 
{
  uint8_t index;
  
  if(!pEvtId) 
  {
    return 0xff;
  }
  
  index = FindEventInEventsTable(pEvtId);
  if(index == 0xff) { 
    return 0xff;
  }
  
  gaEventsTable[index].EntryStatus = gEntryNotUsed_c;
  return index;
}

/******************************************************************************/
/* Process the Received Load Control Event....                                */ 
/*****************************************************************************/
static zbStatus_t ZCL_ProcessLdCtrlEvt(zbApsdeDataIndication_t *pIndication)
{
  uint8_t status = gZclSuccess_c;
  zclCmdDmndRspLdCtrl_LdCtrlEvtReq_t *pMsg;
  afAddrInfo_t addrInfo;
  uint16_t durationInMinutes;
  uint16_t cooling, heating;
  
  /* get the load control event request */
  pMsg = (zclCmdDmndRspLdCtrl_LdCtrlEvtReq_t *)((uint8_t*)pIndication->pAsdu + sizeof(zclFrame_t));
  /* get address ready for reply */
  AF_PrepareForReply(&addrInfo, pIndication); 
  /* check the fields ranges */
  durationInMinutes = OTA2Native16(pMsg->DurationInMinutes);
  if( durationInMinutes > gZclCmdDmndRspLdCtrl_MaxDurationInMinutes_c  )
    status = gZclInvalidField_c;
  
  cooling = OTA2Native16(pMsg->CoolingTempSetPoint);
  heating = OTA2Native16(pMsg->HeatingTempSetPoint); 
  if( (cooling != gZclCmdDmndRspLdCtrl_OptionalTempSetPoint_c) &&
     (heating != gZclCmdDmndRspLdCtrl_OptionalTempSetPoint_c) &&
       (pMsg->CoolingTempOffset != gZclCmdDmndRspLdCtrl_OptionalTempOffset_c) &&
         (pMsg->HeatingTempOffset != gZclCmdDmndRspLdCtrl_OptionalTempOffset_c) )
  {
    if(cooling > gZclCmdDmndRspLdCtrl_MaxTempSetPoint_c||
       heating > gZclCmdDmndRspLdCtrl_MaxTempSetPoint_c )
      status = gZclInvalidField_c;
  }
  else
  {
    if( (cooling == 0x8000 &&  heating != 0x8000)||
       (cooling != 0x8000 &&  heating == 0x8000)||
         (pMsg->CoolingTempOffset != 0xff && pMsg->HeatingTempOffset == 0xff)||
           (pMsg->CoolingTempOffset == 0xff && pMsg->HeatingTempOffset != 0xff))
      status = gZclInvalidField_c;
  }
  
  if( (pMsg->AverageLdAdjustmentPercentage > gZclCmdDmndRspLdCtrl_MaxAverageLdAdjustmentPercentage_c ||
       pMsg->AverageLdAdjustmentPercentage < gZclCmdDmndRspLdCtrl_MinAverageLdAdjustmentPercentage_c )&&
     pMsg->AverageLdAdjustmentPercentage != gZclCmdDmndRspLdCtrl_OptionalAverageLdAdjustmentPercentage_c )
    status = gZclInvalidField_c;
  
  if( pMsg->DutyCycle > gZclCmdDmndRspLdCtrl_MaxDutyCycle_c &&
     pMsg->DutyCycle != gZclCmdDmndRspLdCtrl_OptionalDutyCycle_c )
    status = gZclInvalidField_c;
  
  if(status == gZclInvalidField_c)
  {
    /* Send the Event status rejected */ 
    status = ZCL_SendReportEvtStatus(&addrInfo, pMsg, gSELCDR_LdCtrlEvtCode_LdCtrlEvtCmdRjctd_c, FALSE);  
  }
  else
  {
    status = ZCL_ScheduleEvents(&addrInfo, pMsg); 
  }
  return status;
}

/******************************************************************************/
void ZCL_DeleteServerScheduledEvents(void)
{
  uint8_t i;
  for(i = 0; i < gNumOfServerEventsTableEntry_c; i++)
    gaServerEventsTable[i].EntryStatus = 0x00;
}

/******************************************************************************/
/* Store Load Control information received from the Provider 
     The server side doesn't keep track of the  status, only stores the 
     received events and take care that Nested and overlapping  commands not to occur*/
zbStatus_t ZCL_ScheduleServerLdCtrlEvents (zclCmdDmndRspLdCtrl_LdCtrlEvtReq_t *pMsg)
{
  uint8_t i;
  uint32_t currentTime, startTime,  stopTime;
  uint32_t msgStartTime, msgStopTime; 
  uint16_t duration;

  /* here get the currentTime */
  currentTime = ZCL_GetUTCTime();
  msgStartTime = OTA2Native32(pMsg->StartTime);
  if((msgStartTime == 0x00000000) || (msgStartTime == 0xffffffff))
    msgStartTime = currentTime;
  duration = OTA2Native16(pMsg->DurationInMinutes);
  msgStopTime = msgStartTime + (60*(uint32_t)duration);
  
  //if(msgStopTime <= currentTime)
  //	return status;
  
  /* Nested and overlapping Load Control Event commands are not allowed */
  for(i = 0; i < gNumOfServerEventsTableEntry_c; i++)
  {
    if(gaServerEventsTable[i].EntryStatus == 0x00)
      continue;
    /* Get the timing */
    startTime = OTA2Native32(gaServerEventsTable[i].cmdFrame.StartTime);
    duration = OTA2Native16(gaServerEventsTable[i].cmdFrame.DurationInMinutes);
    stopTime = startTime + (60*(uint32_t)duration);
    if ((msgStartTime >= stopTime)||(startTime >= msgStopTime))
      continue;
    return gZclFailure_c;
  }
  
  mNewEntryIndex = FindEmptyEntryInServerEventsTable();
  
  if(mNewEntryIndex != 0xff)
  {
    /* add the new event and check it for succesive events */
    zclLdCtrl_EventsTableEntry_t *pNewEntry = &gaServerEventsTable[mNewEntryIndex];
    AddNewEntry(pNewEntry, pMsg, &mNewAddrInfo);
  }
              
  if(mNewEntryIndex == 0xff)
    return gZclFailure_c;
  return gZclSuccess_c;
} 



/* Procesing Client side Cluster for Demand Response and Load Control Event */
zbStatus_t ZCL_DmndRspLdCtrlClusterClient
(
zbApsdeDataIndication_t *pIndication, /* IN: */
afDeviceDef_t *pDev                /* IN: */
) 
{
  zclCmd_t Cmd;
  zbStatus_t status = gZclSuccessDefaultRsp_c;
  (void) pDev;
  /* Get the cmd and the SE message */
  Cmd = ((zclFrame_t *)pIndication->pAsdu)->command;
  switch(Cmd) {
  case gZclCmdDmndRspLdCtrl_LdCtrlEvtReq_c:
    status = ZCL_ProcessLdCtrlEvt(pIndication);
    if (status == gZclInvalidField_c) {      
    /* If there is a invalid field in the request, the request will be ignored and no
    response sent */
      status = gZclSuccess_c; 
    }
    break;
    
  case gZclCmdDmndRspLdCtrl_CancelLdCtrlEvtReq_c:
     status = ZCL_ProcessCancelLdCtrlEvt(pIndication);
    break;  
    
  case gZclCmdDmndRspLdCtrl_CancelAllLdCtrlEvtsReq_c:
     status = ZCL_ProcessCancelAllLdCtrlEvtst(pIndication);
    
    break;
    
  default:
    status = gZclUnsupportedClusterCommand_c;
    break;
    
  }
  
  return status;
  
}

/* Procesing Server side Cluster for Demand Response and Load Control Event */
zbStatus_t ZCL_DmndRspLdCtrlClusterServer
(
zbApsdeDataIndication_t *pIndication, /* IN: */
afDeviceDef_t *pDev                /* IN: */
) 
{
  zclCmd_t Cmd;
  addrInfoType_t replyAddrInfo;
  zbStatus_t status = gZclSuccessDefaultRsp_c;
  zclCmdDmndRspLdCtrl_GetScheduledEvts_t * pGetScheduledLdCtlEvt;
  
  (void) pDev;
  /* Get the cmd and the SE message */
  Cmd = ((zclFrame_t *)pIndication->pAsdu)->command;
  /* get address ready for reply */
  AF_PrepareForReply((afAddrInfo_t*)&replyAddrInfo, pIndication);
  switch(Cmd) {
  case gZclCmdDmndRspLdCtrl_GetScheduledEvts_c:
    pGetScheduledLdCtlEvt = (zclCmdDmndRspLdCtrl_GetScheduledEvts_t *)((uint8_t*)pIndication->pAsdu + sizeof(zclFrame_t));
    status = ZCL_ProcessGetScheduledEvtsReq(&replyAddrInfo, pGetScheduledLdCtlEvt);
    break;
  case gZclCmdDmndRspLdCtrl_ReportEvtStatus_c:
    status = gZclSuccess_c;
    break;
  default:
    status = gZclUnsupportedClusterCommand_c;
    break;
    
  }
  
  return status;
}

/*****************************************************************************/
zbStatus_t ZCL_MsgClusterClient
(
	zbApsdeDataIndication_t *pIndication, /* IN: */
	afDeviceDef_t *pDev                /* IN: */
) 
{
  zclCmd_t Cmd;
  zbStatus_t status = gZclSuccessDefaultRsp_c;

 (void) pDev;

 Cmd = ((zclFrame_t *)pIndication->pAsdu)->command;
 /*Both Display message and Cancel message commands are passed up to the application
   so it can take appropiate action on the display.
 */
 switch(Cmd) {
  case gZclCmdMsg_DisplayMsgReq_c: 
    {
      afAddrInfo_t addrInfo;    
      zclCmdMsg_DisplayMsgReq_t *pReq;
      zclSEGenericRxCmd_t rxCmd;
      /* prepare for response in the address info (back to sender) */
      AF_PrepareForReply(&addrInfo, pIndication);
      FLib_MemCpy(&(MsgResponseSourceAddr.msgAddrInfo), &addrInfo, sizeof(afAddrInfo_t));
      pReq = (zclCmdMsg_DisplayMsgReq_t *) ((pIndication->pAsdu) + sizeof(zclFrame_t)); 
      rxCmd.pSECmd = (void*)pReq;
      if (pIndication->fragmentHdr.pData)
        rxCmd.pRxFrag = &pIndication->fragmentHdr;
      else
        rxCmd.pRxFrag = NULL;
      MsgResponseSourceAddr.msgControl = pReq->MsgCtrl;
      BeeAppUpdateDevice(pIndication->dstEndPoint, gZclUI_MsgDisplayMessageReceived_c, 0, 0, &rxCmd);
    }  
  break;  
 case gZclCmdMsg_CancelMsgReq_c:
  {
    zclCmdMsg_CancelMsgReq_t *pReq = (zclCmdMsg_CancelMsgReq_t *) (pIndication->pAsdu + sizeof(zclFrame_t));  
    BeeAppUpdateDevice(pIndication->dstEndPoint, gZclUI_MsgCancelMessageReceived_c, 0, 0, pReq);
  }  
  break;   
  default:
    status = gZclUnsupportedClusterCommand_c;
  break;  
 }
 return status;
}

zbStatus_t ZclMsg_SendLastMessage(afAddrInfo_t* pAddrInfo)
{
  zclDisplayMsgReq_t *pReq;
  zbStatus_t status;
  ZCLTime_t startTime, endTime, currentTime = ZCL_GetUTCTime();
  Duration_t duration;
  
  startTime = OTA2Native32(gMsgLastMessage.EffectiveStartTime);
  duration = OTA2Native16(gMsgLastMessage.DurationInMinutes);
  endTime = startTime + (uint32_t)(duration * 60);
  if ((duration != 0xFFFF) && (currentTime > endTime))
  {
    gMsgLastMessage.Status = 0xFF;
    return gZclNotFound_c;
  } 
  
  pReq = MSG_Alloc(sizeof(zclDisplayMsgReq_t) + sizeof(zclStr32_t));
  if(!pReq)
    return gZbNoMem_c;
  
  /* Add application payload*/
  FLib_MemCpy(&pReq->addrInfo, pAddrInfo, sizeof(afAddrInfo_t));	
  pReq->zclTransactionId =  gZclTransactionId++;
  
  FLib_MemCpy(&pReq->cmdFrame.MsgID, &gMsgLastMessage.MsgID, sizeof(MsgId_t));
  pReq->cmdFrame.MsgCtrl = gMsgLastMessage.MsgCtrl;
  FLib_MemCpy(&pReq->cmdFrame.StartTime, &gMsgLastMessage.StartTime, sizeof(ZCLTime_t));
  
  if ((gMsgLastMessage.StartTime == 0) && (duration != 0xFFFF))
  {
    duration -= (Duration_t)((currentTime - startTime) / 60 ); 
    duration = OTA2Native16(duration);
  }
  FLib_MemCpy(&pReq->cmdFrame.DurationInMinutes, &duration, sizeof(Duration_t));
  
  /* Functionality is limited to a message of maximum 32 characters*/
  pReq->cmdFrame.length = gMsgLastMessage.Msg.length;
  FLib_MemCpy(&pReq->cmdFrame.msgData, &gMsgLastMessage.Msg.aStr, sizeof(zclStr32_t) - 1);  
  
  /* Send request OTA*/
  status = ZclMsg_DisplayMsgReq(pReq);
  MSG_Free(pReq);
  return status;
}

/*****************************************************************************/
zbStatus_t ZCL_MsgClusterServer
(
	zbApsdeDataIndication_t *pIndication, /* IN: */
	afDeviceDef_t *pDev                /* IN: */
) 
{
  zclCmd_t Cmd;
  zbStatus_t status = gZclSuccessDefaultRsp_c;

 (void) pDev;

 Cmd = ((zclFrame_t *)pIndication->pAsdu)->command;

 switch(Cmd) {
  case gZclCmdMsg_GetLastMsgReq_c: 
  {
    zclGetLastMsgReq_t *pReq;
    pReq = (zclGetLastMsgReq_t *) ((pIndication->pAsdu) + sizeof(zclFrame_t));  
    BeeAppUpdateDevice(pIndication->dstEndPoint, gZclUI_MsgGetLastMessageReceived_c, 0, 0, pReq);
    
    if (gMsgLastMessage.Status == 0xFF)
      return gZclNotFound_c;
    else
    {
      afAddrInfo_t replyAddrInfo;
      AF_PrepareForReply((afAddrInfo_t*)&replyAddrInfo, pIndication);
      status = ZclMsg_SendLastMessage(&replyAddrInfo);
    }
  }  
  break;  
 case gZclCmdMsg_MsgConfReq_c:
  {
    zclCmdMsg_MsgConfReq_t *pReq = (zclCmdMsg_MsgConfReq_t *) (pIndication->pAsdu + sizeof(zclFrame_t));  
    BeeAppUpdateDevice(pIndication->dstEndPoint, gZclUI_MsgMessageConfirmReceived_c, 0, 0, pReq);
  }  
  break;   
  default:
    status = gZclUnsupportedClusterCommand_c;
  break;  
 }
 return status;
}

/******************************************************************************/
/* Handle Message Inter Pan Server\Client Indications                           */ 
/******************************************************************************/
zbStatus_t ZCL_InterPanMsgClusterClient
(
	zbInterPanDataIndication_t *pIndication, /* IN: */
	afDeviceDef_t *pDev                /* IN: */
) 
{
  zclCmd_t Cmd;
  zbStatus_t status = gZclSuccessDefaultRsp_c;
 (void) pDev;

 Cmd = ((zclFrame_t *)pIndication->pAsdu)->command;
 /*Both Display message and Cancel message commands are passed up to the application
   so it can take appropiate action on the display.
 */
 switch(Cmd) {
  case gZclCmdMsg_DisplayMsgReq_c: 
  {
      InterPanAddrInfo_t addrInfo;    
      zclCmdMsg_DisplayMsgReq_t *pReq;
      zclSEGenericRxCmd_t rxCmd;
      /* prepare for response in the address info (back to sender) */
      PrepareInterPanForReply(&addrInfo, pIndication);
      FLib_MemCpy(&(MsgResponseSourceAddr.msgAddrInfo), &addrInfo, sizeof(InterPanAddrInfo_t));
      pReq = (zclCmdMsg_DisplayMsgReq_t *) ((pIndication->pAsdu) + sizeof(zclFrame_t));  
      rxCmd.pSECmd = (void*)pReq;
      rxCmd.pRxFrag = NULL;
      MsgResponseSourceAddr.msgControl = (pReq->MsgCtrl | 0x02); // set msg conf as beeing InterPan
      BeeAppUpdateDevice(0, gZclUI_MsgDisplayMessageReceived_c, 0, 0, &rxCmd);
  }  
  break;  
 case gZclCmdMsg_CancelMsgReq_c:
  {
    zclCmdMsg_CancelMsgReq_t *pReq = (zclCmdMsg_CancelMsgReq_t *) (pIndication->pAsdu + sizeof(zclFrame_t));  
    BeeAppUpdateDevice(0, gZclUI_MsgCancelMessageReceived_c, 0, 0, pReq);
  }  
  break;   
  default:
    status = gZclUnsupportedClusterCommand_c;
  break;  
 }
 return status;
}

/******************************************************************************/
zbStatus_t ZCL_InterPanMsgClusterServer
(
zbInterPanDataIndication_t *pIndication, /* IN: */
afDeviceDef_t *pDev                /* IN: */
) 
{
  zclCmd_t Cmd;
  zbStatus_t status = gZclSuccessDefaultRsp_c;

 (void) pDev;

 Cmd = ((zclFrame_t *)pIndication->pAsdu)->command;

 switch(Cmd) {
  case gZclCmdMsg_GetLastMsgReq_c: 
  {
    zclInterPanGetLastMsgReq_t *pReq;
    pReq = (zclInterPanGetLastMsgReq_t *) ((pIndication->pAsdu) + sizeof(zclFrame_t));  
    BeeAppUpdateDevice(0, gZclUI_MsgGetLastMessageReceived_c, 0, 0, pReq);
  }  
  break;  
 case gZclCmdMsg_MsgConfReq_c:
  {
    zclCmdMsg_MsgConfReq_t *pReq = (zclCmdMsg_MsgConfReq_t *) (pIndication->pAsdu + sizeof(zclFrame_t));  
    BeeAppUpdateDevice(0, gZclUI_MsgMessageConfirmReceived_c, 0, 0, pReq);
  }  
  break;   
  default:
    status = gZclUnsupportedClusterCommand_c;
  break;  
 }
 return status;
  
}

/*****************************************************************************/
static void MsgDisplayTimerCallBack(tmrTimerID_t tmrID)
{
   BeeAppUpdateDevice(0, gZclUI_MsgUpdateTimeInDisplay_c, 0, 0, NULL); 
   TMR_StartSecondTimer(gMsgDisplayTimerID, 1, MsgDisplayTimerCallBack);
   (void)tmrID; /* Unused parameter. */
}


/*****************************************************************************/
msgAddrInfo_t *ZCL_GetMsgSourceaddrForResponse(void)
{
  return &MsgResponseSourceAddr;
}

/*****************************************************************************/
/* Free memory used by key estab cluster                                     */
/*****************************************************************************/
void KeyEstabFreeMemory(void)
{
  if(pOppositeImplicitCert)
  {
    MSG_Free(pOppositeImplicitCert);
    pOppositeImplicitCert = NULL;
  }
  if(KeyData)
  {
    MSG_Free(KeyData);
    KeyData = NULL;
  }
  
}

#if gZclKeyEstabDebugTimer_d
void KeyEstabDebugTimerCallBack(tmrTimerID_t tmrID)
{
  zclCmd_t TransactionId;
  
  (void) tmrID;
  TMR_FreeTimer(tmrID);
  TransactionId = ((zclFrame_t *)pKeyEstabDataIndication->pAsdu)->transactionId; 
  
  switch(KeyEstabState)
  {  
  case KeyEstab_KeyEstabInitatedState_c:
    (void) TransactionId;
    ZCL_SendEphemeralDataReq(KeyData->u_ephemeralPublicKey, pKeyEstabDataIndication);
    KeyEstabState = KeyEstab_EphemeralState_c;
    MSG_Free(pKeyEstabDataIndication);
    break;
  case KeyEstab_EphemeralState_c:
#if gCoordinatorCapability_d
    ZCL_SendEphemeralDataRsp(KeyData->v_ephemeralPublicKey, pKeyEstabDataIndication, TransactionId);
    KeyEstabState = KeyEstab_ConfirmDataState_c;
    MSG_Free(pKeyEstabDataIndication);
#else    
    ZCL_SendConfirmKeyDataReq(KeyData->MACU, pKeyEstabDataIndication);
    KeyEstabState = KeyEstab_ConfirmDataState_c;
    MSG_Free(pKeyEstabDataIndication);
#endif    
    break;
  case KeyEstab_ConfirmDataState_c:
#if gCoordinatorCapability_d    
    ZCL_SendConfirmKeyDataRsp(KeyData->MACV, pKeyEstabDataIndication, TransactionId);
    KeyEstabState = KeyEstab_InitState_c;
    MSG_Free(pKeyEstabDataIndication);
    KeyEstabFreeMemory();
#endif
    break;
  default:
    break;    
  }
}
#endif

/*****************************************************************************/
/* Timout timer callback function                                            */
/* If timeout triggered this function will reset state machine and clean up  */
/*****************************************************************************/
void KeyEstabTimeoutCallback(tmrTimerID_t tmrid)
{
  (void) tmrid;
  TMR_FreeTimer(tmrid); 
  if (KeyEstabState != KeyEstab_InitState_c) 
  {    
    KeyEstabState = KeyEstab_InitState_c;
    KeyEstabFreeMemory();
    BeeAppUpdateDevice(0, gZclUI_KeyEstabTimeout_c, 0, 0, NULL);
/* Change the poll rate to its initial value after ECC procedure*/    
#if gEndDevCapability_d || gComboDeviceCapability_d
#if gComboDeviceCapability_d
    if(NlmeGetRequest(gDevType_c) == gEndDevice_c)
#endif
    {
      (void)ZDO_NLME_ChangePollRate(gZdoOrgPollRate);
    }
#endif     
  }    
}

/*****************************************************************************/
/* Stops and frees timer used for key estab timeout                          */
/*****************************************************************************/
void KeyEstabStopTimeout(void)
{
    TMR_StopTimer(KeyEstabTimerId);
    TMR_FreeTimer(KeyEstabTimerId); 
    KeyEstabTimerId = gTmrInvalidTimerID_c;
/* Change the poll rate to its initial value after ECC procedure*/    
#if gEndDevCapability_d || gComboDeviceCapability_d
#if gComboDeviceCapability_d
    if(NlmeGetRequest(gDevType_c) == gEndDevice_c)
#endif
    {
      (void)ZDO_NLME_ChangePollRate(gZdoOrgPollRate);
    }
#endif  
}

/*****************************************************************************/
/* Resets timeout timer so timeout period is started again                   */
/*****************************************************************************/
void ResetKeyEstabTimeout(void)
{
 // use the biggest timeout value reported, but minimum 2 seconds.
  uint8_t timeout = FLib_GetMax(EphemeralDataGenerateTime, ConfirmKeyGenerateTime );
  timeout = FLib_GetMax(gKeyEstab_MinimumTimeout_c , timeout );
  
  TMR_StartSingleShotTimer(KeyEstabTimerId, TmrSeconds(timeout), KeyEstabTimeoutCallback);
}

/*****************************************************************************/
/* Initializes (allocates timer and start timeout timer                      */
/*****************************************************************************/
void InitAndStartKeyEstabTimeout(void)
{
  KeyEstabTimerId = TMR_AllocateTimer();
  ResetKeyEstabTimeout();
}
/*****************************************************************************/
/* Allocates memory used during a key establishment session                  */
/*****************************************************************************/
bool_t KeyEstabAllocateMemory(void)
{
  KeyData = MSG_Alloc(sizeof(KeyEstab_KeyData_t));
  if (KeyData)
  {
    BeeUtilZeroMemory(KeyData, sizeof(KeyEstab_KeyData_t));
    pOppositeImplicitCert = MSG_Alloc(sizeof(IdentifyCert_t));
    if (pOppositeImplicitCert)
      return TRUE;
  }
  // memory allocation failed - free memory
  KeyEstabFreeMemory();
  return FALSE;  
}
/*****************************************************************************/
/* Initializes Key estab. state machine and allocates memory needed          */
/*****************************************************************************/
bool_t InitKeyEstabStateMachine(void) {
  if(KeyEstabState == KeyEstab_InitState_c) 
  {
    if( KeyEstabAllocateMemory()) {      
      KeyEstabState = KeyEstab_KeyEstabInitatedState_c;
      return TRUE;
    }    
  }
 return FALSE;  
}
/*****************************************************************************/
/* Sends terminate key estab to server and resets statemachine               */
/*****************************************************************************/
void ZCL_TerminateKeyEstabServer(zbApsdeDataIndication_t *pIndication, uint8_t ErrorCode,bool_t ResetStateMachine)
{  
  ZCL_SendTerminateKeyEstabServerReq(ErrorCode, gKeyEstab_DefaultWaitTime_c,pIndication);
  KeyEstabStopTimeout();
  if (ResetStateMachine) 
  {    
    KeyEstabState = KeyEstab_InitState_c;
    KeyEstabFreeMemory();
    BeeAppUpdateDevice(pIndication->dstEndPoint, gZclUI_KeyEstabFailed_c, 0, 0, pIndication);
  }  
}
/*****************************************************************************/
/* Sends terminate key estab to client and resets statemachine               */
/*****************************************************************************/
void ZCL_TerminateKeyEstabClient(zbApsdeDataIndication_t *pIndication, uint8_t ErrorCode,bool_t ResetStateMachine) 
{

  ZCL_SendTerminateKeyEstabClientReq(ErrorCode, gKeyEstab_DefaultWaitTime_c,pIndication,((zclFrame_t *)pIndication->pAsdu)->transactionId);
  KeyEstabStopTimeout();
  if (ResetStateMachine) 
  {    
    KeyEstabState = KeyEstab_InitState_c;
    KeyEstabFreeMemory();
    BeeAppUpdateDevice(pIndication->dstEndPoint, gZclUI_KeyEstabFailed_c, 0, 0, pIndication);    
  }
}
/*****************************************************************************/
/* calculates key, MACU, MAC for the server/responder side                   */
/*****************************************************************************/
void GenerateKeyMACUMACVResponder(void)
{       // Generate MAC key and MAC data  
        SSP_HashKeyDerivationFunction(KeyData->SharedSecret,gZclCmdKeyEstab_SharedSecretSize_c, KeyData->mackey);
        //MACtag 1, initator Ieee, responer ieee, Initiator Ep pub key, responder ep pub key, shared secret Z, output.
        SSP_HashGenerateSecretTag(2,(uint8_t*) pOppositeImplicitCert->Subject,(uint8_t*) DeviceImplicitCert.Subject, KeyData->u_ephemeralPublicKey,gZclCmdKeyEstab_CompressedPubKeySize_c,KeyData->v_ephemeralPublicKey,gZclCmdKeyEstab_CompressedPubKeySize_c,KeyData->SharedSecret,KeyData->MACU,FALSE, NULL);
        //MACtag 2, responder Ieee, initiator ieee, responder Ep pub key, initiator ep pub key, shared secret Z, output.
        SSP_HashGenerateSecretTag(3,(uint8_t*) DeviceImplicitCert.Subject, (uint8_t*)pOppositeImplicitCert->Subject, KeyData->v_ephemeralPublicKey,gZclCmdKeyEstab_CompressedPubKeySize_c,KeyData->u_ephemeralPublicKey,gZclCmdKeyEstab_CompressedPubKeySize_c,KeyData->SharedSecret,KeyData->MACV,FALSE, NULL);          
        //MACtag 1, initator Ieee, responer ieee, Initiator Ep pub key, responder ep pub key, shared secret Z, output.

}
/*****************************************************************************/
/* calculates key, MACU, MAC for the client/initiator side                   */
/*****************************************************************************/
void GenerateKeyMACUMACVInitiator(void)
{
// Generate MAC key and MAC data  
        SSP_HashKeyDerivationFunction(KeyData->SharedSecret, gZclCmdKeyEstab_SharedSecretSize_c, KeyData->mackey);
        //MACtag 1, initator Ieee, responer ieee, Initiator Ep pub key, responder ep pub key, shared secret Z, output.
        SSP_HashGenerateSecretTag(2,(uint8_t*) DeviceImplicitCert.Subject,(uint8_t*) pOppositeImplicitCert->Subject, KeyData->u_ephemeralPublicKey,gZclCmdKeyEstab_CompressedPubKeySize_c,KeyData->v_ephemeralPublicKey,gZclCmdKeyEstab_CompressedPubKeySize_c,KeyData->SharedSecret,KeyData->MACU, FALSE, NULL);
        //MACtag 2, responder Ieee, initiator ieee, responder Ep pub key, initiator ep pub key, shared secret Z, output.
        SSP_HashGenerateSecretTag(3,(uint8_t*) pOppositeImplicitCert->Subject,(uint8_t*) DeviceImplicitCert.Subject,KeyData->v_ephemeralPublicKey,gZclCmdKeyEstab_CompressedPubKeySize_c,KeyData->u_ephemeralPublicKey,gZclCmdKeyEstab_CompressedPubKeySize_c,KeyData->SharedSecret,KeyData->MACV, FALSE, NULL);                
}
/*****************************************************************************/
/* Check if the sender and the length of the InitKeyEstabReq is correct      */
/*****************************************************************************/
bool_t ValidateKeyEstabMessage(zbApsdeDataIndication_t *pIndication, IdentifyCert_t *pCert) 
{
zbNwkAddr_t aSrcAddr; 
zbIeeeAddr_t aCertIeee;
#if gComboDeviceCapability_d || gCoordinatorCapability_d
zbNwkAddr_t aCertNwk;
#endif
uint8_t asduLen=pIndication->asduLength; 
FLib_MemCpyReverseOrder(aCertIeee,pCert->Subject,sizeof(zbIeeeAddr_t));
Copy2Bytes(aSrcAddr,pIndication->aSrcAddr);
#if gComboDeviceCapability_d 
  if (((NlmeGetRequest(gDevType_c) != gCoordinator_c) && (Cmp2BytesToZero(aSrcAddr) && IsEqual8Bytes(aCertIeee, ApsmeGetRequest(gApsTrustCenterAddress_c))))||((NlmeGetRequest(gDevType_c) == gCoordinator_c) && IsEqual2Bytes(APS_GetNwkAddress(aCertIeee,aCertNwk),aSrcAddr)))
#elif gCoordinatorCapability_d
  if(IsEqual2Bytes(APS_GetNwkAddress(aCertIeee,aCertNwk),aSrcAddr))
#else
  if(Cmp2BytesToZero(aSrcAddr) && IsEqual8Bytes(aCertIeee, ApsmeGetRequest(gApsTrustCenterAddress_c)))
#endif 
  {
#if gZclAcceptLongCert_d
    if((asduLen>=sizeof(ZclCmdKeyEstab_InitKeyEstabReq_t)+sizeof(zclFrame_t)))
#else
    if((asduLen==sizeof(ZclCmdKeyEstab_InitKeyEstabReq_t)+sizeof(zclFrame_t)))
#endif
      return TRUE;
  } 
  return FALSE; 
} 

/*****************************************************************************/
/* Key Establishment Cluster server, only one session at a time is supported */
/*****************************************************************************/
zbStatus_t ZCL_KeyEstabClusterServer
(
zbApsdeDataIndication_t *pIndication, /* IN: */
afDeviceDef_t *pDev                /* IN: */
) 
{
  zclCmd_t Cmd;
  zclCmd_t TransactionId;
  zbStatus_t status = gZbSuccess_c;
  (void) pDev;
  
  
  Cmd = ((zclFrame_t *)pIndication->pAsdu)->command;
  TransactionId = ((zclFrame_t *)pIndication->pAsdu)->transactionId;
  switch(Cmd)
  {   
  case gZclCmdKeyEstab_InitKeyEstabReq_c:
    {
      ZclCmdKeyEstab_InitKeyEstabReq_t *pReq;
      IdentifyCert_t *pCert;
      pReq = (ZclCmdKeyEstab_InitKeyEstabReq_t *) ((pIndication->pAsdu) + sizeof(zclFrame_t));         
      pCert = (IdentifyCert_t *) pReq->IdentityIDU;
      if (KeyEstabState == KeyEstab_InitState_c)
      {
        // check whether request orginate from coordiantor, if device is not a coordinator.
        if (((NlmeGetRequest(gDevType_c) != gCoordinator_c) && (Cmp2BytesToZero(pIndication->aSrcAddr))) ||
            (NlmeGetRequest(gDevType_c) == gCoordinator_c))
        {
          // certificate validation function
          if(ValidateKeyEstabMessage(pIndication,pCert))
          {
            // validate whether the securitysuite is the correct type
            if(IsEqual2BytesInt(pReq->SecuritySuite, gKeyEstabSuite_CBKE_ECMQV_c))
            {
              // validate whether issuer ID is correct:
              if(FLib_MemCmp(pCert->Issuer, (void *)CertAuthIssuerID, sizeof(CertAuthIssuerID)))
              { 
                if (KeyEstabAllocateMemory()) {
                  FLib_MemCpy((uint8_t*) pOppositeImplicitCert,(void *)pCert, sizeof(IdentifyCert_t));
                  // Issuer ID is correct, send response back.
                  EphemeralDataGenerateTime = pReq->EphemeralDataGenerateTime;
                  ConfirmKeyGenerateTime    = pReq->ConfirmKeyGenerateTime;
                  InitAndStartKeyEstabTimeout();
#if !gZclKeyEstabDebugInhibitInitRsp_d                  
                  (void) ZCL_SendInitiatKeyEstRsp(&DeviceImplicitCert, pIndication,TransactionId);
#endif                  
                  KeyEstabState = KeyEstab_EphemeralState_c;
                }
                else
                { // Memory coult not be allocated.
                  ZCL_TerminateKeyEstabClient(pIndication, gZclCmdKeyEstab_TermNoResources_c, TRUE);
                }
              }
              else
              { // Issuer ID was not the correct, send terminate message
                ZCL_TerminateKeyEstabClient(pIndication, gZclCmdKeyEstab_TermUnknownIssuer_c,TRUE);
              }          
            }
            else
            {
              //only CBKE-ECMQV supported
              ZCL_TerminateKeyEstabClient(pIndication, gZclCmdKeyEstab_TermUnsupportedSuite_c,FALSE);          
            }
          }
          else
          {
            //invalid InitKeyEstabReq/certificate
            ZCL_TerminateKeyEstabClient(pIndication, gZclCmdKeyEstab_TermBadMessage_c, TRUE);
          }
        }
        else
        {
          // discard, message when initate message is not from coordinator. 
          ZCL_TerminateKeyEstabClient(pIndication, gZclCmdKeyEstab_TermBadMessage_c,TRUE);    
        }               
      }
      else
      {
        //Device is in the middle of key establishment with another device, send terminate message with status: no resources.
        ZCL_SendTerminateKeyEstabClientReq(gZclCmdKeyEstab_TermNoResources_c, gKeyEstab_DefaultWaitTime_c,pIndication,((zclFrame_t *)pIndication->pAsdu)->transactionId);
      }
    }
    break;      
  case gZclCmdKeyEstab_EphemeralDataReq_c:
    {
      
      if (KeyEstabState == KeyEstab_EphemeralState_c)
      {
        uint8_t aNwkAddrLocalCpy[2];
        zbIeeeAddr_t clientIeee;
        FLib_MemCpyReverseOrder(clientIeee,pOppositeImplicitCert->Subject,sizeof(zbIeeeAddr_t));
        //check if the originator of the ephemeral data request is the originator of the init key estab req
        if(IsEqual2Bytes(APS_GetNwkAddress(clientIeee,aNwkAddrLocalCpy),pIndication->aSrcAddr))
        {
          ZclCmdKeyEstab_EphemeralDataReq_t *pReq;
      
          pReq = (ZclCmdKeyEstab_EphemeralDataReq_t *) ((pIndication->pAsdu) + sizeof(zclFrame_t));         
          FLib_MemCpy(KeyData->u_ephemeralPublicKey,pReq->EphemeralDataQEU, gZclCmdKeyEstab_CompressedPubKeySize_c);
          if (ZSE_ECCGenerateKey(KeyData->ephemeralPrivateKey, KeyData->v_ephemeralPublicKey, ECC_GetRandomDataFunc, NULL, 0) == 0x00) // 0x00 = MCE_SUCCESS
          {
            ResetKeyEstabTimeout();
          }
          // Send response:
          // Generate keybits
          if(ZSE_ECCKeyBitGenerate(DevicePrivateKey, KeyData->ephemeralPrivateKey, KeyData->v_ephemeralPublicKey,
                                   (uint8_t *)pOppositeImplicitCert, KeyData->u_ephemeralPublicKey, (uint8_t *)CertAuthPubKey,KeyData->SharedSecret, ECC_HashFunc, NULL, 0)==0x00)
          {
            GenerateKeyMACUMACVResponder();
#if gZclKeyEstabDebugTimer_d
            KeyEstabDebugTimerId = TMR_AllocateTimer();
            pKeyEstabDataIndication = MSG_Alloc(sizeof(zbApsdeDataIndication_t));
            FLib_MemCpy(pKeyEstabDataIndication, pIndication, sizeof(zbApsdeDataIndication_t));
            TMR_StartTimer(KeyEstabDebugTimerId, gTmrSingleShotTimer_c, gZclKeyEstabDebugTimeInterval_d, KeyEstabDebugTimerCallBack);   
#else

            KeyEstabState = KeyEstab_ConfirmDataState_c;
            ZCL_SendEphemeralDataRsp(KeyData->v_ephemeralPublicKey, pIndication,TransactionId);
#endif            
          }
          else
          {
            ZCL_TerminateKeyEstabClient(pIndication, gZclCmdKeyEstab_TermBadMessage_c, TRUE);
          }
        }
        else
        {
          ZCL_SendTerminateKeyEstabClientReq(gZclCmdKeyEstab_TermBadMessage_c, gKeyEstab_DefaultWaitTime_c,pIndication,((zclFrame_t *)pIndication->pAsdu)->transactionId);
        }
      }
      else
      { // command is out of sequence... send terminate message
        ZCL_SendTerminateKeyEstabClientReq(gZclCmdKeyEstab_TermBadMessage_c, gKeyEstab_DefaultWaitTime_c,pIndication,((zclFrame_t *)pIndication->pAsdu)->transactionId);
      }
    }
    break;      
  case gZclCmdKeyEstab_ConfirmKeyDataReq_c:
    if (KeyEstabState == KeyEstab_ConfirmDataState_c)
    {
      uint8_t aNwkAddrLocalCpy[2];
      zbIeeeAddr_t clientIeee;
      FLib_MemCpyReverseOrder(clientIeee,pOppositeImplicitCert->Subject,sizeof(zbIeeeAddr_t));
      //check if the originator of the confirm key data request is the originator of the init key estab req
      if(IsEqual2Bytes(APS_GetNwkAddress(clientIeee,aNwkAddrLocalCpy),pIndication->aSrcAddr))
      {
        ZclCmdKeyEstab_ConfirmKeyDataReq_t *pReq;
        pReq = (ZclCmdKeyEstab_ConfirmKeyDataReq_t *) ((pIndication->pAsdu) + sizeof(zclFrame_t));         
        
        if (FLib_MemCmp(pReq->SecureMsgAuthCodeMACU, KeyData->MACU, gZclCmdKeyEstab_AesMMOHashSize_c))
        {
          uint8_t keytype = gApplicationLinkKey_c;
          zbIeeeAddr_t addr;
          KeyEstabStopTimeout();
#if gZclKeyEstabDebugTimer_d
          KeyEstabDebugTimerId = TMR_AllocateTimer();
          pKeyEstabDataIndication = MSG_Alloc(sizeof(zbApsdeDataIndication_t));
          FLib_MemCpy(pKeyEstabDataIndication, pIndication, sizeof(zbApsdeDataIndication_t));
          TMR_StartTimer(KeyEstabDebugTimerId, gTmrSingleShotTimer_c, gZclKeyEstabDebugTimeInterval_d, KeyEstabDebugTimerCallBack);   
#else          
          ZCL_SendConfirmKeyDataRsp(KeyData->MACV,pIndication,TransactionId);
          KeyEstabState = KeyEstab_InitState_c;
#endif          
          // call BeeAppDeviceJoined to tell that device has been registered.
          // install new key:
          // Ieee address in certificate is Big endian, convert to little endian before adding it.
          Copy8Bytes(addr,pOppositeImplicitCert->Subject);
          Swap8Bytes(addr);                
          if (IsEqual8Bytes(addr, ApsmeGetRequest(gApsTrustCenterAddress_c)))          
          {
            keytype = gTrustCenterLinkKey_c;
          }        
          APS_RegisterLinkKeyData(addr,keytype,(uint8_t *) KeyData->mackey);        
          BeeAppUpdateDevice(pIndication->dstEndPoint, gZclUI_KeyEstabSuccesful_c, 0, 0, addr);
#if !gZclKeyEstabDebugTimer_d          
          KeyEstabFreeMemory();
#endif          
        }
        else
        {
          // call BeeAppDeviceJoined to tell that a MACU failed to validate.
          ZCL_TerminateKeyEstabClient(pIndication, gZclCmdKeyEstab_TermBadKeyConfirm_c, TRUE);
        }
      }
      else
      {
        ZCL_SendTerminateKeyEstabClientReq(gZclCmdKeyEstab_TermBadMessage_c, gKeyEstab_DefaultWaitTime_c,pIndication,((zclFrame_t *)pIndication->pAsdu)->transactionId);
      }
    }
    else 
    {  // command is out of sequence... send terminate message
      ZCL_SendTerminateKeyEstabClientReq(gZclCmdKeyEstab_TermBadMessage_c, gKeyEstab_DefaultWaitTime_c,pIndication,((zclFrame_t *)pIndication->pAsdu)->transactionId);
    }
    break;      
  case gZclCmdKeyEstab_TerminateKeyEstabServer_c:
    {
      uint8_t aNwkAddrLocalCpy[2];
      zbIeeeAddr_t clientIeee;
      FLib_MemCpyReverseOrder(clientIeee,pOppositeImplicitCert->Subject,sizeof(zbIeeeAddr_t));
      //accept terminate key estab server only from the device involved in key estab
      if(IsEqual2Bytes(APS_GetNwkAddress(clientIeee,aNwkAddrLocalCpy),pIndication->aSrcAddr))
      {
        // call BeeAppDeviceJoined to tell that device failed to register.
        KeyEstabState = KeyEstab_InitState_c;
        KeyEstabFreeMemory();
        BeeAppUpdateDevice(pIndication->dstEndPoint, gZclUI_KeyEstabFailed_c, 0, 0, pIndication);    
      }
    }
    break;          
  default:
    status = gZclUnsupportedClusterCommand_c;
    break;
  }
  return status;
}

zbStatus_t ZCL_KeyEstabClusterClient
(
	zbApsdeDataIndication_t *pIndication, /* IN: */
	afDeviceDef_t *pDev                /* IN: */
) 
{
  zclCmd_t Cmd;
  zbStatus_t status = gZbSuccess_c;

  (void) pDev; 
  Cmd = ((zclFrame_t *)pIndication->pAsdu)->command;

  switch(Cmd)
  {   
  case gZclCmdKeyEstab_InitKeyEstabRsp_c:
    {
      ZclCmdKeyEstab_InitKeyEstabRsp_t *pReq;
      IdentifyCert_t *pCert;
      pReq = (ZclCmdKeyEstab_InitKeyEstabRsp_t *) ((pIndication->pAsdu) + sizeof(zclFrame_t));         
      pCert = (IdentifyCert_t *) pReq->IdentityIDV;
      if(KeyEstabState == KeyEstab_KeyEstabInitatedState_c)
      {
        if (ValidateKeyEstabMessage(pIndication,pCert))
        {
          // validate whether issuer ID is correct:
          if(FLib_MemCmp(pCert->Issuer, (void *)CertAuthIssuerID, sizeof(CertAuthIssuerID)) == TRUE)         
          {           
            FLib_MemCpy((void *)pOppositeImplicitCert,(void *)pCert, sizeof(IdentifyCert_t));          
            EphemeralDataGenerateTime = pReq->EphemeralDataGenerateTime;
            ConfirmKeyGenerateTime    = pReq->ConfirmKeyGenerateTime;
            // Issuer ID is correct, send ephemerial Data request back.
            if(ZSE_ECCGenerateKey(KeyData->ephemeralPrivateKey, KeyData->u_ephemeralPublicKey, ECC_GetRandomDataFunc, NULL, 0) == 0x00)//0x00 = MCE_SUCCESS
            {
              ResetKeyEstabTimeout();
              
#if gZclKeyEstabDebugTimer_d
              KeyEstabDebugTimerId = TMR_AllocateTimer();
              pKeyEstabDataIndication = MSG_Alloc(sizeof(zbApsdeDataIndication_t));
              FLib_MemCpy(pKeyEstabDataIndication, pIndication, sizeof(zbApsdeDataIndication_t));
              TMR_StartTimer(KeyEstabDebugTimerId, gTmrSingleShotTimer_c, gZclKeyEstabDebugTimeInterval_d, KeyEstabDebugTimerCallBack);   
#else
              ZCL_SendEphemeralDataReq(KeyData->u_ephemeralPublicKey,pIndication);
              KeyEstabState = KeyEstab_EphemeralState_c;
#endif              
            }                            
          }
          else
          {
            // send terminate key estab. with UNKNOWN_ISSUER status           
            ZCL_TerminateKeyEstabServer(pIndication, gZclCmdKeyEstab_TermUnknownIssuer_c, TRUE);                 
          }                  
        }
        else
        {
          // cluster is not participating in key establishment
          // send terminate key estab. with BAD_MESSAGE status
          ZCL_TerminateKeyEstabServer(pIndication, gZclCmdKeyEstab_TermBadMessage_c, TRUE);                 
        }
      }
      else
      {
        // command is out of sequence... send terminate message
        ZCL_SendTerminateKeyEstabServerReq(gZclCmdKeyEstab_TermBadMessage_c, gKeyEstab_DefaultWaitTime_c,pIndication);
      }
    }
    break;      
  case gZclCmdKeyEstab_EphemeralDataRsp_c:
    {
      if (KeyEstabState == KeyEstab_EphemeralState_c)
      {
        uint8_t aNwkAddrLocalCpy[2];
        zbIeeeAddr_t serverIeee;
        FLib_MemCpyReverseOrder(serverIeee,pOppositeImplicitCert->Subject,sizeof(zbIeeeAddr_t));
        //check that the originator of the ephemeral data response is correct
        if(IsEqual2Bytes(APS_GetNwkAddress(serverIeee,aNwkAddrLocalCpy),pIndication->aSrcAddr))
        {
          ZclCmdKeyEstab_EphemeralDataRsp_t *pReq;
          pReq = (ZclCmdKeyEstab_EphemeralDataRsp_t *) ((pIndication->pAsdu) + sizeof(zclFrame_t));
          FLib_MemCpy(KeyData->v_ephemeralPublicKey,pReq->EphemeralDataQEV, gZclCmdKeyEstab_CompressedPubKeySize_c);
          // Generate keybits 
          if (ZSE_ECCKeyBitGenerate(DevicePrivateKey, KeyData->ephemeralPrivateKey, KeyData->u_ephemeralPublicKey,
                                    (uint8_t *)pOppositeImplicitCert, KeyData->v_ephemeralPublicKey, (uint8_t *)CertAuthPubKey,KeyData->SharedSecret, ECC_HashFunc, NULL, 0)==0x00)
          {
            // Generate MAC key and MAC data  
            GenerateKeyMACUMACVInitiator();        
            ResetKeyEstabTimeout();
            
#if gZclKeyEstabDebugTimer_d
            KeyEstabDebugTimerId = TMR_AllocateTimer();
            pKeyEstabDataIndication = MSG_Alloc(sizeof(zbApsdeDataIndication_t));
            FLib_MemCpy(pKeyEstabDataIndication, pIndication, sizeof(zbApsdeDataIndication_t));
            TMR_StartTimer(KeyEstabDebugTimerId, gTmrSingleShotTimer_c, gZclKeyEstabDebugTimeInterval_d, KeyEstabDebugTimerCallBack);   
#else
            ZCL_SendConfirmKeyDataReq(KeyData->MACU,pIndication);        
            KeyEstabState = KeyEstab_ConfirmDataState_c;
#endif            
          }
          else
          {
            ZCL_TerminateKeyEstabServer(pIndication, gZclCmdKeyEstab_TermBadMessage_c, TRUE);
          }
        }
        else
        {
          //Originator of the ephemeral data response is different than the originator of the init key estab rsp
          ZCL_SendTerminateKeyEstabServerReq(gZclCmdKeyEstab_TermBadMessage_c, gKeyEstab_DefaultWaitTime_c,pIndication);
        }
      }
      else
      {
        // command is out of sequence... send terminate message
        ZCL_TerminateKeyEstabServer(pIndication, gZclCmdKeyEstab_TermBadMessage_c, TRUE);
      }
    }
    break;      
  case gZclCmdKeyEstab_ConfirmKeyDataRsp_c:
    {
      ZclCmdKeyEstab_ConfirmKeyDataRsp_t *pReq;
      pReq = (ZclCmdKeyEstab_ConfirmKeyDataRsp_t *) ((pIndication->pAsdu) + sizeof(zclFrame_t));
      if (KeyEstabState == KeyEstab_ConfirmDataState_c)
      {
        uint8_t aNwkAddrLocalCpy[2];
        zbIeeeAddr_t serverIeee;
        FLib_MemCpyReverseOrder(serverIeee,pOppositeImplicitCert->Subject,sizeof(zbIeeeAddr_t));
        //check that the originator of the confirm key data response is correct
        if(IsEqual2Bytes(APS_GetNwkAddress(serverIeee,aNwkAddrLocalCpy),pIndication->aSrcAddr))
        {
          if (FLib_MemCmp(pReq->SecureMsgAuthCodeMACV, KeyData->MACV,gZclCmdKeyEstab_AesMMOHashSize_c))
          {
            // Call BeeAppDeviceUpdate and tell that device has been registered successfully
            // install key:
            // Ieee address in certificate is Big endian, convert to little endian before adding it.
            uint8_t keytype = gApplicationLinkKey_c;
            zbIeeeAddr_t addr;
            KeyEstabStopTimeout();
            Copy8Bytes(addr,pOppositeImplicitCert->Subject);
            Swap8Bytes(addr);
            if (IsEqual8Bytes(addr, ApsmeGetRequest(gApsTrustCenterAddress_c)))          
            {
              keytype = gTrustCenterLinkKey_c;
            }        
            APS_RegisterLinkKeyData(addr,keytype,(uint8_t *) KeyData->mackey);
            BeeAppUpdateDevice(pIndication->dstEndPoint, gZclUI_KeyEstabSuccesful_c, 0, 0, addr);          
            KeyEstabFreeMemory();
            KeyEstabState = KeyEstab_InitState_c;
            /* Change the poll rate to its initial value after ECC procedure*/    
#if gEndDevCapability_d || gComboDeviceCapability_d
#if gComboDeviceCapability_d
            if(NlmeGetRequest(gDevType_c) == gEndDevice_c)
#endif
            {
              (void)ZDO_NLME_ChangePollRate(gZdoOrgPollRate);
            }
#endif 
          }
          else
          {
            ZCL_TerminateKeyEstabServer(pIndication, gZclCmdKeyEstab_TermBadKeyConfirm_c, TRUE);
          }
        }
        else
        {
          //Originator of the confirm key data response different than the originator of the init key estab rsp
          ZCL_SendTerminateKeyEstabServerReq(gZclCmdKeyEstab_TermBadMessage_c, gKeyEstab_DefaultWaitTime_c,pIndication);
        }
      }
      else
      {
        // command is out of sequence... send terminate message
        ZCL_SendTerminateKeyEstabServerReq(gZclCmdKeyEstab_TermBadMessage_c, gKeyEstab_DefaultWaitTime_c,pIndication);
      }
    }
    break;      
  case gZclCmdKeyEstab_TerminateKeyEstabClient_c:
    {
      uint8_t aNwkAddrLocalCpy[2];
      zbIeeeAddr_t serverIeee;
      FLib_MemCpyReverseOrder(serverIeee,pOppositeImplicitCert->Subject,sizeof(zbIeeeAddr_t));
      //accept terminate key estab client only in KeyEstab_KeyEstabInitatedState_c or from the device involved in key estab.
      if((KeyEstabState == KeyEstab_KeyEstabInitatedState_c) || (IsEqual2Bytes(APS_GetNwkAddress(serverIeee,aNwkAddrLocalCpy),pIndication->aSrcAddr)))
      {
#if gEndDevCapability_d || gComboDeviceCapability_d
#if gComboDeviceCapability_d
         if(NlmeGetRequest(gDevType_c) == gEndDevice_c)
#endif
          {
            (void)ZDO_NLME_ChangePollRate(gZdoOrgPollRate);
          }
#endif 
        // call BeeAppDeviceUpdate and tell that device failed to register.
        KeyEstabState = KeyEstab_InitState_c;
        KeyEstabFreeMemory();
        BeeAppUpdateDevice(pIndication->dstEndPoint, gZclUI_KeyEstabFailed_c, 0, 0, pIndication);
      }
    }
    break;          
  default:
    status = gZclUnsupportedClusterCommand_c;
    break;
  }
 return status;
}

void ZclMsg_StoreMessage(zclDisplayMsgReq_t *pReq)
{
  /* Functionality is limited to a message of maximum 32 characters*/
  gMsgLastMessage.Status = 0x00;
  FLib_MemCpy(&gMsgLastMessage.MsgID, &pReq->cmdFrame.MsgID, sizeof(MsgId_t));
  gMsgLastMessage.MsgCtrl = pReq->cmdFrame.MsgCtrl;
  FLib_MemCpy(&gMsgLastMessage.StartTime, &pReq->cmdFrame.StartTime, sizeof(ZCLTime_t));
  FLib_MemCpy(&gMsgLastMessage.DurationInMinutes, &pReq->cmdFrame.DurationInMinutes, sizeof(Duration_t));
  if (pReq->cmdFrame.StartTime == 0)
  {
    uint32_t currentTime = ZCL_GetUTCTime();
    currentTime = Native2OTA32(currentTime);
    FLib_MemCpy(&gMsgLastMessage.EffectiveStartTime, &currentTime, sizeof(ZCLTime_t));    
  }
  else
    FLib_MemCpy(&gMsgLastMessage.EffectiveStartTime, &pReq->cmdFrame.StartTime, sizeof(ZCLTime_t));    
  gMsgLastMessage.Msg.length = pReq->cmdFrame.length;
  FLib_MemCpy(&gMsgLastMessage.Msg.aStr, &pReq->cmdFrame.msgData, sizeof(zclStr32_t) - 1);  
}

/*****************************************************************************/
/*-------- Message cluster commands ---------*/
/*Message cluster client commands */
zbStatus_t ZclMsg_DisplayMsgReq
(
zclDisplayMsgReq_t *pReq
)
{
  uint8_t Length;
  /*1 is the length byte*/
  Length = (pReq->cmdFrame.length) + (sizeof(zclCmdMsg_DisplayMsgReq_t) - sizeof(uint8_t));
  (void)ZclMsg_StoreMessage(pReq);
  return ZCL_SendServerReqSeqPassed(gZclCmdMsg_DisplayMsgReq_c, Length,(zclGenericReq_t *)pReq);	
}

zbStatus_t ZclMsg_CancelMsgReq
(
zclCancelMsgReq_t *pReq
)
{
  gMsgLastMessage.Status = 0xFF;  
  return ZCL_SendServerReqSeqPassed(gZclCmdMsg_CancelMsgReq_c, sizeof(zclCmdMsg_CancelMsgReq_t),(zclGenericReq_t *)pReq);
}

/*Message cluster Server commands */
zbStatus_t ZclMsg_GetLastMsgReq
(
zclGetLastMsgReq_t *pReq
)
{
  return ZCL_SendClientReqSeqPassed(gZclCmdMsg_GetLastMsgReq_c, 0 , (zclGenericReq_t *)pReq);
}


zbStatus_t ZclMsg_MsgConf
(
zclMsgConfReq_t *pReq
)
{
  return ZCL_SendClientRspSeqPassed(gZclCmdMsg_CancelMsgReq_c, sizeof(zclCmdMsg_MsgConfReq_t),(zclGenericReq_t *)pReq);
}



#if gInterPanCommunicationEnabled_c 
zbStatus_t ZclMsg_InterPanDisplayMsgReq
(
zclInterPanDisplayMsgReq_t *pReq
)
{
  uint8_t Length;
  /*1 is the length byte*/
  Length = (pReq->cmdFrame.length) + (sizeof(zclCmdMsg_DisplayMsgReq_t) - sizeof(uint8_t));	
  
  return ZCL_SendInterPanServerReqSeqPassed(gZclCmdMsg_DisplayMsgReq_c, Length,pReq);	
}

zbStatus_t ZclMsg_InterPanCancelMsgReq
(
zclInterPanCancelMsgReq_t *pReq
)
{ 
  return ZCL_SendInterPanServerReqSeqPassed(gZclCmdMsg_CancelMsgReq_c, sizeof(zclCmdMsg_CancelMsgReq_t),pReq);
}

/*Message cluster Server commands */
zbStatus_t ZclMsg_InterPanGetLastMsgReq
(
zclInterPanGetLastMsgReq_t *pReq
)
{
  return ZCL_SendInterPanClientReqSeqPassed(gZclCmdMsg_GetLastMsgReq_c, 0 , pReq);
}


zbStatus_t ZclMsg_InterPanMsgConf
(
zclInterPanMsgConfReq_t *pReq
)
{
  return ZCL_SendInterPanClientRspSeqPassed(gZclCmdMsg_CancelMsgReq_c, sizeof(zclCmdMsg_MsgConfReq_t),pReq);
}
#endif /*#if gInterPanCommunicationEnabled_c */

/*-------- DRLC cluster commands ---------*/
/*DRLC cluster client commands */
zbStatus_t ZclDmndRspLdCtrl_ReportEvtStatus
(
zclDmndRspLdCtrl_ReportEvtStatus_t *pReq
)
{
  uint8_t length;
#if gASL_ZclDmndRspLdCtrl_ReportEvtStatus_d
  uint8_t entryIndex;
  
  if (pReq->cmdFrame.EvtStatus == gSELCDR_LdCtrlEvtCode_UserHaveToChooseOptOut_c ||
      pReq->cmdFrame.EvtStatus == gSELCDR_LdCtrlEvtCode_UserHaveToChooseOptIn_c)
  {
    entryIndex = FindEventInEventsTable((uint8_t *)&(pReq->cmdFrame.IssuerEvtID));
    if(entryIndex != 0xff)
      gaEventsTable[entryIndex].CurrentStatus = pReq->cmdFrame.EvtStatus;
  }
#endif  
  length = sizeof(zclCmdDmndRspLdCtrl_ReportEvtStatus_t);
  return ZCL_SendClientRspSeqPassed(gZclCmdDmndRspLdCtrl_ReportEvtStatus_c, length ,(zclGenericReq_t *)pReq); 
  
}

/*DRLC cluster Server commands */
zbStatus_t ZclDmndRspLdCtrl_LdCtrlEvtReq
(
zclDmndRspLdCtrl_LdCtrlEvtReq_t *pReq
)
{
  
  return ZCL_SendServerReqSeqPassed(gZclCmdDmndRspLdCtrl_LdCtrlEvtReq_c, sizeof(zclCmdDmndRspLdCtrl_LdCtrlEvtReq_t),(zclGenericReq_t *)pReq);	
}

zbStatus_t ZclDmndRspLdCtrl_CancelLdCtrlEvtReq
(
zclDmndRspLdCtrl_CancelLdCtrlEvtReq_t *pReq
)
{ 
  return ZCL_SendServerReqSeqPassed(gZclCmdDmndRspLdCtrl_CancelLdCtrlEvtReq_c, sizeof(zclCmdDmndRspLdCtrl_CancelLdCtrlEvtReq_t),(zclGenericReq_t *)pReq);	
}

zbStatus_t ZclDmndRspLdCtrl_CancelAllLdCtrlEvtReq
(
zclDmndRspLdCtrl_CancelAllLdCtrlEvtsReq_t *pReq
)
{
  return ZCL_SendServerReqSeqPassed(gZclCmdDmndRspLdCtrl_CancelAllLdCtrlEvtsReq_c, sizeof(zclCmdDmndRspLdCtrl_CancelAllLdCtrlEvtsReq_t),(zclGenericReq_t *)pReq);	
}

zbStatus_t ZclDmndRspLdCtrl_GetScheduledEvtsReq
(
zclDmndRspLdCtrl_GetScheduledEvts_t *pReq
)
{
  return ZCL_SendClientReqSeqPassed(gZclCmdDmndRspLdCtrl_GetScheduledEvts_c, sizeof(zclCmdDmndRspLdCtrl_GetScheduledEvts_t),(zclGenericReq_t *)pReq);	
}

/*---------------- Prepayment Cluster  ---------------------*/

void ZCL_PrepaymentServerInit(void)
{
  gPrepaymentTimerID = TMR_AllocateTimer();
}

//client indication
zbStatus_t ZCL_PrepaymentClient
(
	zbApsdeDataIndication_t *pIndication, /* IN: */
	afDeviceDef_t *pDev                /* IN: */
)
{
  zclCmd_t Cmd;
  zbStatus_t status = gZclSuccessDefaultRsp_c;

 (void) pDev;

 Cmd = ((zclFrame_t *)pIndication->pAsdu)->command;
 switch(Cmd) {
  case gZclCmdPrepayment_Server_SupplyStatRsp_c: 
  {
    return gZclSuccess_c;
  }    
  default:
    status = gZclUnsupportedClusterCommand_c;
  break;  
 }
 return status;
}

//server indication
zbStatus_t ZCL_PrepaymentServer
(
	zbApsdeDataIndication_t *pIndication, /* IN: */
	afDeviceDef_t *pDev                /* IN: */
)
{
  zclCmd_t Cmd;
  zbStatus_t status = gZclFailure_c;

  (void) pDev;

  Cmd = ((zclFrame_t *)pIndication->pAsdu)->command;
 
  switch(Cmd) {
  case gZclCmdPrepayment_Client_SelAvailEmergCreditReq_c: 
  {
    //zclCmdPrepayment_SelAvailEmergCreditReq_t *pReq;
    uint8_t creditStatus;
    afAddrInfo_t addrInfo;
    AF_PrepareForReply(&addrInfo, pIndication);
   // pReq = (zclCmdPrepayment_SelAvailEmergCreditReq_t *) ((pIndication->pAsdu) + sizeof(zclFrame_t));
    
      
    /* Update the Credit Status Attribute*/
    (void)ZCL_GetAttribute(addrInfo.srcEndPoint, addrInfo.aClusterId, gZclAttrSmplPrepaymentCreditStatus_c , &creditStatus, NULL);
    
    if (creditStatus & gSePrepaymentCreditStatusFlags_EmergencyAvailable)
    {
      creditStatus |= (gSePrepaymentCreditStatusFlags_EmergencyEnabled | gSePrepaymentCreditStatusFlags_EmergencySelected);
      (void)ZCL_SetAttribute(addrInfo.srcEndPoint, addrInfo.aClusterId, gZclAttrSmplPrepaymentCreditStatus_c , &creditStatus);
      status = gZclSuccess_c;        
    }
  }  
  break;  
  
  case gZclCmdPrepayment_Client_ChangeSupplyReq_c: 
  {
    status = ZCL_ProcessChangeSupplyReq(pIndication);  
  }
  break;  
  
  default:
  {
    status = gZclUnsupportedClusterCommand_c;
  }
  break;  
 }
 return status;
}

//client generated commands
zbStatus_t zclPrepayment_Client_SelAvailEmergCreditReq
(
zclPrepayment_SelAvailEmergCreditReq_t *pReq
)
{
  uint8_t *pSrc,*pDst;
  uint8_t length;
  uint8_t meterSerialNumberLength = pReq->cmdFrame.meterSerialNumber.length;
  uint8_t siteIdLength = pReq->cmdFrame.siteId.length;
  
  pSrc  = (uint8_t *) &pReq->cmdFrame.meterSerialNumber;
  pDst  = &pReq->cmdFrame.siteId.aStr[pReq->cmdFrame.siteId.length];  
  /*1 is the length byte*/
  length = sizeof(zclCmdPrepayment_SelAvailEmergCreditReq_t) - (sizeof(ZCLTime_t) + 2 * sizeof(uint8_t) + pReq->cmdFrame.siteId.length);  
  FLib_MemInPlaceCpy(pDst, pSrc, length);
     
  length = sizeof(zclCmdPrepayment_SelAvailEmergCreditReq_t) - (31 - siteIdLength)- (15 - meterSerialNumberLength);
  
  return ZCL_SendClientReqSeqPassed(gZclCmdPrepayment_Client_SelAvailEmergCreditReq_c, length,(zclGenericReq_t *)pReq);
}

zbStatus_t zclPrepayment_Client_ChangeSupplyReq
(
  zclPrepayment_ChangeSupplyReq_t *pReq
)
{
  uint8_t *pSrc,*pDst;
  uint8_t length;
  uint8_t meterSerialNumberLength = pReq->cmdFrame.meterSerialNumber.length;
  uint8_t siteIdLength = pReq->cmdFrame.siteId.length;
  
  pSrc  = (uint8_t *) &pReq->cmdFrame.meterSerialNumber;
  pDst  = &pReq->cmdFrame.siteId.aStr[pReq->cmdFrame.siteId.length];  
  /*1 is the length byte*/
  length = sizeof(zclCmdPrepayment_ChangeSupplyReq_t) - (sizeof(ZCLTime_t) + sizeof(ProviderID_t)+ sizeof(uint8_t) + pReq->cmdFrame.siteId.length);  
  FLib_MemInPlaceCpy(pDst, pSrc, length);
     
  pSrc  = (uint8_t *) &pReq->cmdFrame.implementationDateTime - (31 - siteIdLength);
  pDst  = &pReq->cmdFrame.meterSerialNumber.aStr[meterSerialNumberLength]- (31 - siteIdLength);  
  /*1 is the length byte*/
  length = sizeof(zclCmdPrepayment_ChangeSupplyReq_t) - (sizeof(ZCLTime_t) + sizeof(ProviderID_t)+ sizeof(uint8_t) + siteIdLength
                                                         + sizeof(uint8_t) + meterSerialNumberLength);  
  FLib_MemInPlaceCpy(pDst, pSrc, length);
  
  length = sizeof(zclCmdPrepayment_ChangeSupplyReq_t) - (31 - siteIdLength)- (15 - meterSerialNumberLength);
  
  return ZCL_SendClientReqSeqPassed(gZclCmdPrepayment_Client_ChangeSupplyReq_c, length,(zclGenericReq_t *)pReq);
}

//server generated commands
static zbStatus_t zclPrepayment_Server_SupplyStatusRsp
(
zclPrepayment_SupplyStatRsp_t *pReq
)
{
  return ZCL_SendServerRspSeqPassed(gZclCmdPrepayment_Server_SupplyStatRsp_c, sizeof(zclCmdPrepayment_SupplyStatRsp_t),(zclGenericReq_t *)pReq);
}

void PrepaymentTimerCallBack(tmrTimerID_t timerID)
{
  ZCLTime_t time = 0x00000000; 
  zbClusterId_t clusterIdPrepay = {gaZclClusterPrepayment_c};  
  zbClusterId_t clusterIdMet = {gaZclClusterSmplMet_c};  
  (void)timerID;
  
  /* Change Proposed Supply Status*/
  (void)ZCL_SetAttribute(appEndPoint, clusterIdMet, gZclAttrSmplMetRISSupplyStatus_c , &mProposedSupplyStatus);
  /* Reset Proposed Implementation Time*/
  (void)ZCL_SetAttribute(appEndPoint, clusterIdPrepay, gZclAttrSmplPrepaymentProposedChangeImplTime_c , &time);
}
static zbStatus_t ZCL_ProcessChangeSupplyReq
(
zbApsdeDataIndication_t *pIndication
)
{
  zclCmdPrepayment_ChangeSupplyReq_t  *pMsg;
  afAddrInfo_t addrInfo;
  uint8_t supplyStatus, siteIdLength, meterSerialNoLength, proposedSupplyStatus, originatorId;
  ZCLTime_t currentTime = ZCL_GetUTCTime();
  ZCLTime_t implementationDateTime, startTime = 0x00000000;
  uint16_t delayedInterruptValue;
  zbClusterId_t clusterIdMet = {gaZclClusterSmplMet_c};
  
  pMsg = (zclCmdPrepayment_ChangeSupplyReq_t*)((uint8_t*)pIndication->pAsdu + sizeof(zclFrame_t));
  siteIdLength = pMsg->siteId.length;
  meterSerialNoLength = *(&pMsg->siteId.length + siteIdLength + sizeof(uint8_t)); 
  proposedSupplyStatus = *(&pMsg->siteId.length + siteIdLength + meterSerialNoLength + 2 * sizeof(uint8_t) + sizeof(ZCLTime_t));
  FLib_MemCpy(&implementationDateTime, &pMsg->siteId.length + siteIdLength + meterSerialNoLength + 2 * sizeof(uint8_t), sizeof(ZCLTime_t));
  implementationDateTime = OTA2Native32(implementationDateTime);
  originatorId = *(&pMsg->siteId.length + siteIdLength + meterSerialNoLength + 3 * sizeof(uint8_t) + sizeof(ZCLTime_t));

  AF_PrepareForReply(&addrInfo, pIndication);
  (void)ZCL_GetAttribute(appEndPoint, clusterIdMet, gZclAttrSmplMetRISSupplyStatus_c , &supplyStatus, NULL);
  
  if (proposedSupplyStatus == supplyStatus) 
    return gZclSuccessDefaultRsp_c;
  
  startTime = implementationDateTime;
   
  if (implementationDateTime == 0x00000000)
  {
    if (proposedSupplyStatus == gSePrepaymentSupplyInterrupt_c)
    {
      (void)ZCL_GetAttribute(addrInfo.srcEndPoint, addrInfo.aClusterId, gZclAttrSmplPrepaymentDelayedSuplyIntValueRem_c , &delayedInterruptValue, NULL);
      if (delayedInterruptValue == 0)
       (void)ZCL_SetAttribute(appEndPoint, clusterIdMet, gZclAttrSmplMetRISSupplyStatus_c , &proposedSupplyStatus);
    }
    else
      (void)ZCL_SetAttribute(appEndPoint, clusterIdMet, gZclAttrSmplMetRISSupplyStatus_c , &proposedSupplyStatus);
      
  } 
  else if (implementationDateTime == 0xFFFFFFFF)
  {
    TMR_StopTimer(gPrepaymentTimerID);
    /* Reset Proposed Implementation Time*/
    (void)ZCL_SetAttribute(addrInfo.srcEndPoint, addrInfo.aClusterId, gZclAttrSmplPrepaymentProposedChangeImplTime_c , &startTime);
  }  
   
  else if (startTime < currentTime)
  {
    return gZclInvalidValue_c;
  }
  else
  {
    (void)ZCL_SetAttribute(addrInfo.srcEndPoint, addrInfo.aClusterId, gZclAttrSmplPrepaymentProposedChangeSupplyStatus_c , &proposedSupplyStatus);
    /* Set the proposed implementation time */
    (void)ZCL_SetAttribute(addrInfo.srcEndPoint, addrInfo.aClusterId, gZclAttrSmplPrepaymentProposedChangeImplTime_c , &implementationDateTime);
    /* Set the proposed provider id */
    (void)ZCL_SetAttribute(addrInfo.srcEndPoint, addrInfo.aClusterId, gZclAttrSmplPrepaymentProposedChangeProviderID_c , &pMsg->providerId);
    /* Store the proposed status*/
    mProposedSupplyStatus = proposedSupplyStatus;
    /* Set the timer for the supply status change */
    TMR_StartSecondTimer(gPrepaymentTimerID, (uint16_t)(startTime - currentTime), PrepaymentTimerCallBack);
  }
 
  /* Check if we have to send a response */
  if(originatorId & 0x08)
    return ZCL_SendSupplyStatusRsp(pIndication);
  else
    return gZclSuccess_c;
}

static zbStatus_t ZCL_SendSupplyStatusRsp
(
zbApsdeDataIndication_t *pIndication
)
{
  zclPrepayment_SupplyStatRsp_t       response;
  zclCmdPrepayment_ChangeSupplyReq_t  *pMsg;
  /* Get the request to extract the required info */
  pMsg = (zclCmdPrepayment_ChangeSupplyReq_t*)((uint8_t*)pIndication->pAsdu + sizeof(zclFrame_t));
  /* Prepare the response */
  AF_PrepareForReply(&response.addrInfo, pIndication);
  response.zclTransactionId = ((zclFrame_t *)pIndication->pAsdu)->transactionId;
  FLib_MemCpy(&response.cmdFrame.providerId, &pMsg->providerId, sizeof(ProviderID_t));
  response.cmdFrame.implementationDateTime = pMsg->implementationDateTime;
  response.cmdFrame.supplyStatus = pMsg->proposedSupplyStatus;
  
  return zclPrepayment_Server_SupplyStatusRsp(&response);
}
/*-------- key Establishment cluster commands ---------*/

void ZCL_ApplyECDSASign(uint8_t *pBufIn, uint8_t lenIn, uint8_t *pBufOut)
{
  uint8_t digest[16];
  /* Hash the pBufIn */
  BeeUtilZeroMemory(digest, 16);
  SSP_MatyasMeyerOseasHash(pBufIn, lenIn, digest);

  (void)ZSE_ECDSASign( DevicePrivateKey, &digest[0],
                ECC_GetRandomDataFunc, pBufOut, (pBufOut + 21), NULL, 0 );

}


zbStatus_t zclKeyEstab_InitKeyEstabReq
(
ZclKeyEstab_InitKeyEstabReq_t *pReq
) 
{
 	Set2Bytes(pReq->addrInfo.aClusterId, gKeyEstabCluster_c);
  return ZCL_SendClientReqSeqPassed(gZclCmdKeyEstab_InitKeyEstabReq_c, sizeof(ZclCmdKeyEstab_InitKeyEstabReq_t),(zclGenericReq_t *)pReq);
}

zbStatus_t zclKeyEstab_EphemeralDataReq
(
ZclKeyEstab_EphemeralDataReq_t *pReq
) 
{
		Set2Bytes(pReq->addrInfo.aClusterId, gKeyEstabCluster_c);
  return ZCL_SendClientReqSeqPassed(gZclCmdKeyEstab_EphemeralDataReq_c, sizeof(ZclCmdKeyEstab_EphemeralDataReq_t),(zclGenericReq_t *)pReq);
}


zbStatus_t zclKeyEstab_ConfirmKeyDataReq
(
ZclKeyEstab_ConfirmKeyDataReq_t *pReq
) 
{
  	Set2Bytes(pReq->addrInfo.aClusterId, gKeyEstabCluster_c);
  return ZCL_SendClientReqSeqPassed(gZclCmdKeyEstab_ConfirmKeyDataReq_c, sizeof(ZclCmdKeyEstab_ConfirmKeyDataReq_t),(zclGenericReq_t *)pReq);
}

zbStatus_t zclKeyEstab_TerminateKeyEstabServer
(
ZclKeyEstab_TerminateKeyEstabServer_t *pReq
) 
{
  	Set2Bytes(pReq->addrInfo.aClusterId, gKeyEstabCluster_c);
  return ZCL_SendClientReqSeqPassed(gZclCmdKeyEstab_TerminateKeyEstabServer_c, sizeof(ZclCmdKeyEstab_TerminateKeyEstabServer_t),(zclGenericReq_t *)pReq);
}

zbStatus_t zclKeyEstab_InitKeyEstabRsp
(
ZclKeyEstab_InitKeyEstabRsp_t *pReq
) 
{
  	Set2Bytes(pReq->addrInfo.aClusterId, gKeyEstabCluster_c);
  return ZCL_SendServerRspSeqPassed(gZclCmdKeyEstab_InitKeyEstabRsp_c, sizeof(ZclCmdKeyEstab_InitKeyEstabRsp_t),(zclGenericReq_t *)pReq);
}

zbStatus_t zclKeyEstab_EphemeralDataRsp
(
ZclKeyEstab_EphemeralDataRsp_t *pReq
) 
{
  	Set2Bytes(pReq->addrInfo.aClusterId, gKeyEstabCluster_c);
  return ZCL_SendServerRspSeqPassed(gZclCmdKeyEstab_EphemeralDataRsp_c, sizeof(ZclCmdKeyEstab_EphemeralDataRsp_t),(zclGenericReq_t *)pReq);
}


zbStatus_t zclKeyEstab_ConfirmKeyDataRsp
(
ZclKeyEstab_ConfirmKeyDataRsp_t *pReq
) 
{ 
  	Set2Bytes(pReq->addrInfo.aClusterId, gKeyEstabCluster_c);
  return ZCL_SendServerRspSeqPassed(gZclCmdKeyEstab_ConfirmKeyDataRsp_c, sizeof(ZclCmdKeyEstab_ConfirmKeyDataRsp_t),(zclGenericReq_t *)pReq);
}

zbStatus_t zclKeyEstab_TerminateKeyEstabClient
(
ZclKeyEstab_TerminateKeyEstabClient_t *pReq
) 
{
	Set2Bytes(pReq->addrInfo.aClusterId, gKeyEstabCluster_c);
  return ZCL_SendServerRspSeqPassed(gZclCmdKeyEstab_TerminateKeyEstabClient_c, sizeof(ZclCmdKeyEstab_TerminateKeyEstabClient_t),(zclGenericReq_t *)pReq);
}

zbStatus_t zclKeyEstab_SetSecurityMaterial
(
	ZclKeyEstab_SetSecurityMaterial_t *pReq
) 
{
	FLib_MemCpy(&DeviceImplicitCert, &pReq->deviceImplicitCert, sizeof(IdentifyCert_t));
	FLib_MemCpy(&DevicePrivateKey, &pReq->devicePrivateKey, gZclCmdKeyEstab_PrivateKeySize_c * sizeof(uint8_t));
	FLib_MemCpy(&DevicePublicKey, &pReq->devicePublicKey, gZclCmdKeyEstab_CompressedPubKeySize_c * sizeof(uint8_t));
	return gZbSuccess_c;
}
bool_t ZCL_InitiateKeyEstab(zbEndPoint_t DstEndpoint,zbEndPoint_t SrcEndpoint, zbNwkAddr_t DstAddr) {
  // Init key estab. state machine. (key establishment inprogress)
   if (InitKeyEstabStateMachine())  
   {
/* Change the poll rate to the authentication specific value for the ECC procedure*/    
#if gEndDevCapability_d || gComboDeviceCapability_d
#if gComboDeviceCapability_d
    if(NlmeGetRequest(gDevType_c) == gEndDevice_c)
#endif
    {
      (void)ZDO_NLME_ChangePollRate(gKeyEstab_EndDevicePollRate_c);
    }
#endif      
#if !gZclKeyEstabDebugMode_d
    ZCL_SendInitiatKeyEstReq(&DeviceImplicitCert, DstEndpoint, SrcEndpoint,(uint8_t*) DstAddr);
#else
    (void)DstEndpoint;
    (void)SrcEndpoint;
    (void)DstAddr;
#endif 
    InitAndStartKeyEstabTimeout();
    return TRUE;
   } else {
     return FALSE;   
   }
}

bool_t InitClientKeyEstab(void)
{
  if (InitKeyEstabStateMachine())
  {
    InitAndStartKeyEstabTimeout();
    return TRUE;
  }
  return FALSE;
}
  
void ZCL_SendInitiatKeyEstReq(IdentifyCert_t *Cert, zbEndPoint_t DstEndpoint,zbEndPoint_t SrcEndpoint, zbNwkAddr_t DstAddr) {
ZclKeyEstab_InitKeyEstabReq_t *pReq;
  
  pReq = MSG_Alloc(sizeof(ZclKeyEstab_InitKeyEstabReq_t)); 
  
  if(pReq) {
  pReq->addrInfo.dstAddrMode = gZbAddrMode16Bit_c;
  Copy2Bytes(pReq->addrInfo.dstAddr.aNwkAddr, DstAddr);
  pReq->addrInfo.dstEndPoint = DstEndpoint;
  pReq->addrInfo.srcEndPoint = SrcEndpoint;
  pReq->addrInfo.txOptions = 0;
  pReq->addrInfo.radiusCounter = afDefaultRadius_c;
  pReq->zclTransactionId = gZclTransactionId++;
	
  pReq->cmdFrame.EphemeralDataGenerateTime = gKeyEstab_DefaultEphemeralDataGenerateTime_c;
  pReq->cmdFrame.ConfirmKeyGenerateTime = gKeyEstab_DefaultConfirmKeyGenerateTime_c;
  Set2Bytes(pReq->cmdFrame.SecuritySuite,gKeyEstabSuite_CBKE_ECMQV_c);
  
  FLib_MemCpy(pReq->cmdFrame.IdentityIDU,(uint8_t *)Cert ,sizeof(IdentifyCert_t)/sizeof(uint8_t));
  
  (void) zclKeyEstab_InitKeyEstabReq(pReq);
  MSG_Free(pReq);
  }
}

void ZCL_SendEphemeralDataReq(uint8_t *EphemeralPubKey, zbApsdeDataIndication_t *pIndication) {
ZclKeyEstab_EphemeralDataReq_t *pReq;
  
  pReq = MSG_Alloc(sizeof(ZclKeyEstab_EphemeralDataReq_t)); 
  
  if(pReq) {
    AF_PrepareForReply(&pReq->addrInfo, pIndication);
	
  pReq->zclTransactionId = gZclTransactionId++;
	
  FLib_MemCpy(pReq->cmdFrame.EphemeralDataQEU,(uint8_t *)EphemeralPubKey ,gZclCmdKeyEstab_CompressedPubKeySize_c);
  
  (void) zclKeyEstab_EphemeralDataReq(pReq);
  MSG_Free(pReq);
  }
}
void ZCL_SendConfirmKeyDataReq(uint8_t *MACU, zbApsdeDataIndication_t *pIndication) {
ZclKeyEstab_ConfirmKeyDataReq_t *pReq;
  
  pReq = MSG_Alloc(sizeof(ZclKeyEstab_ConfirmKeyDataReq_t)); 
  
  if(pReq) {
    AF_PrepareForReply(&pReq->addrInfo, pIndication);
	
  pReq->zclTransactionId = gZclTransactionId++;
	
  FLib_MemCpy(pReq->cmdFrame.SecureMsgAuthCodeMACU,(uint8_t *)MACU ,gZclCmdKeyEstab_AesMMOHashSize_c);
  
  (void) zclKeyEstab_ConfirmKeyDataReq(pReq);
  MSG_Free(pReq);
  }
}

void ZCL_SendTerminateKeyEstabServerReq(uint8_t Status,uint8_t WaitCode, zbApsdeDataIndication_t *pIndication) {
ZclKeyEstab_TerminateKeyEstabServer_t *pReq;
  
  pReq = MSG_Alloc(sizeof(ZclKeyEstab_TerminateKeyEstabServer_t)); 
  
  if(pReq) {
    AF_PrepareForReply(&pReq->addrInfo, pIndication);
	
  pReq->zclTransactionId = gZclTransactionId++;
	
  pReq->cmdFrame.StatusCode  = Status;
  pReq->cmdFrame.WaitCode    = WaitCode;
  Set2Bytes(pReq->cmdFrame.SecuritySuite,gKeyEstabSuite_CBKE_ECMQV_c);  
  
  (void) zclKeyEstab_TerminateKeyEstabServer(pReq);
  MSG_Free(pReq);
  }
}




void ZCL_SendInitiatKeyEstRsp(IdentifyCert_t *Cert,  zbApsdeDataIndication_t *pIndication, uint8_t transactionid) {
ZclKeyEstab_InitKeyEstabRsp_t *pReq;
  
  pReq = MSG_Alloc(sizeof(ZclKeyEstab_InitKeyEstabReq_t)); 
  
  if(pReq) {
    AF_PrepareForReply(&pReq->addrInfo, pIndication);
	
  pReq->zclTransactionId = transactionid;
	
  pReq->cmdFrame.EphemeralDataGenerateTime = gKeyEstab_DefaultEphemeralDataGenerateTime_c;
  pReq->cmdFrame.ConfirmKeyGenerateTime = gKeyEstab_DefaultConfirmKeyGenerateTime_c;
  Set2Bytes(pReq->cmdFrame.SecuritySuite,gKeyEstabSuite_CBKE_ECMQV_c);
  
  FLib_MemCpy(pReq->cmdFrame.IdentityIDV,(uint8_t *)Cert ,sizeof(IdentifyCert_t)/sizeof(uint8_t));
  
  (void) zclKeyEstab_InitKeyEstabRsp(pReq);
  MSG_Free(pReq);
  }
}

void  ZCL_SendEphemeralDataRsp(uint8_t *EphemeralPubKey,  zbApsdeDataIndication_t *pIndication, uint8_t transactionid) {
ZclKeyEstab_EphemeralDataRsp_t *pReq;
  
  pReq = MSG_Alloc(sizeof(ZclKeyEstab_EphemeralDataRsp_t)); 
  
  if(pReq) {
    AF_PrepareForReply(&pReq->addrInfo, pIndication);
	
  pReq->zclTransactionId = transactionid;	

  FLib_MemCpy(pReq->cmdFrame.EphemeralDataQEV,(uint8_t *)EphemeralPubKey ,gZclCmdKeyEstab_CompressedPubKeySize_c);
  
  (void) zclKeyEstab_EphemeralDataRsp(pReq);
  MSG_Free(pReq);
  }
}

void  ZCL_SendConfirmKeyDataRsp(uint8_t *MACV, zbApsdeDataIndication_t *pIndication, uint8_t transactionid) {
ZclKeyEstab_ConfirmKeyDataRsp_t *pReq;
  
  pReq = MSG_Alloc(sizeof(ZclKeyEstab_ConfirmKeyDataRsp_t)); 
  
  if(pReq) {
    AF_PrepareForReply(&pReq->addrInfo, pIndication);
	
  pReq->zclTransactionId = transactionid;	

  FLib_MemCpy(pReq->cmdFrame.SecureMsgAuthCodeMACV,(uint8_t *)MACV ,gZclCmdKeyEstab_AesMMOHashSize_c);
  
  (void) zclKeyEstab_ConfirmKeyDataRsp(pReq);
  MSG_Free(pReq);
  }
}

void  ZCL_SendTerminateKeyEstabClientReq(uint8_t Status, uint8_t WaitCode,zbApsdeDataIndication_t *pIndication , uint8_t transactionid) {
ZclKeyEstab_TerminateKeyEstabClient_t *pReq;
  
  pReq = MSG_Alloc(sizeof(ZclKeyEstab_TerminateKeyEstabClient_t)); 
  
  if(pReq) {
    AF_PrepareForReply(&pReq->addrInfo, pIndication);
    	
  pReq->zclTransactionId = transactionid;	

  pReq->cmdFrame.StatusCode  = Status;
  pReq->cmdFrame.WaitCode    = WaitCode;
  Set2Bytes(pReq->cmdFrame.SecuritySuite,gKeyEstabSuite_CBKE_ECMQV_c);  

  (void) zclKeyEstab_TerminateKeyEstabClient(pReq);
  MSG_Free(pReq);
  }
}

ESPRegisterDevFunc *pSE_ESPRegFunc = NULL;
ESPDeRegisterDevFunc *pSE_ESPDeRegFunc = NULL;

void ZCL_Register_EspRegisterDeviceFunctions(ESPRegisterDevFunc *RegFunc,ESPDeRegisterDevFunc *DeRegFunc)
{
  pSE_ESPRegFunc = RegFunc;
  pSE_ESPDeRegFunc = DeRegFunc;
}


/******************************************************************************
*******************************************************************************
* Private functions
*******************************************************************************
******************************************************************************/
int ECC_GetRandomDataFunc(unsigned char *buffer, unsigned long sz)
{
        unsigned long i;
        for(i=0;i<sz;i++){
                ((unsigned char *)buffer)[i] =(uint8_t) GetRandomNumber();
        }
        return 0x00; //success
} 

int ECC_HashFunc(unsigned char *digest, unsigned long sz, unsigned char *data)
{
/* Insert to use Certicom's AES MMO function*/
/*  aesMmoHash(digest, sz, data);  */
  /*zero initialize digest before generating hash*/
  BeeUtilZeroMemory(digest,16);
  SSP_MatyasMeyerOseasHash(data, (uint8_t) sz, digest);  
  return 0x00; //success
}

/******************************************************************************/ 
uint16_t crc_reflect(uint16_t data, uint8_t data_len)
{
    uint8_t i;
    uint16_t ret;
 
    ret = 0;
    for (i = 0; i < data_len; i++)
    {
        if (data & 0x01) {
            ret = (ret << 1) | 1;
        } else {
            ret = ret << 1;
        }
        data >>= 1;
    }
    return ret;
}
 
/******************************************************************************/ 
uint16_t crc_update(uint16_t crc16, uint8_t *pInput, uint8_t length)
{
uint8_t data;
  while (length--)
  {
 data = (uint8_t)crc_reflect(*pInput++, 8); // pInput may need to be shifted 8 bits up if running code on a big endian machine.
 
    crc16 = (uint8_t)(crc16 >> 8) | (crc16 << 8);
    crc16 ^= data; // x^1
    crc16 ^= (uint8_t)(crc16 & 0xff) >> 4; // ???
    crc16 ^= (crc16 << 8) << 4;    // x^12
    crc16 ^= ((crc16 & 0xff) << 4) << 1;    // X^5
  }
  return (0xFFFF^crc_reflect(crc16, 16));
}
/******************************************************************************/
/******************************************************************************
 
Name        : GenerateKeyFromInstallCode
Input(s)    : pInstallationCode: pointer to an installation code (max 144bit incl crc16)
 
              length: length of installation key (in bytes incl CRC)
 
In/Output(s): pKey: pointer to key (or hash)
 
Description : This function computes an initial key based on an installation code
 
*******************************************************************************/
bool_t GenerateKeyFromInstallCode(uint8_t length,uint8_t* pInstallationCode, uint8_t *pKey)
{
  // 1. Validate InstallationCode (CRC16)
  // 2. Pad installation code (M) to obtain M' 
  // 3. Compute hash using AES128bit engine
  uint16_t mCrc16;
  uint16_t vCrc16 = 0xffff;
  mCrc16=(pInstallationCode[length-1]<<8 | pInstallationCode[length-2]);
  vCrc16 = crc_update(vCrc16,pInstallationCode, length-2);
  // Check CRC
  if(mCrc16!=vCrc16){
    return FALSE;
  } 
  else{
    SSP_MatyasMeyerOseasHash(pInstallationCode,length ,pKey);
    return TRUE;  
  }
}

/* place functions here that will be shared by other modules */
zbStatus_t ZCL_ESPRegisterDevice(EspRegisterDevice_t *Devinfo)
{
  zbAESKey_t key = {0};
  index_t    i;
  zbNwkAddr_t unknownAddr = {0xFF,0xFF};
  i = ZCL_AddToRegTable(Devinfo);
  if (i != 0xFF)
  {
    if (GenerateKeyFromInstallCode(RegistrationTable[i].DevInfo.InstallCodeLength,RegistrationTable[i].DevInfo.InstallCode,key)) {    
      if (APS_AddToAddressMapPermanent(Devinfo->Addr,unknownAddr) != gAddressMapFull_c)
        APS_RegisterLinkKeyData(RegistrationTable[i].DevInfo.Addr,gApplicationLinkKey_c,(uint8_t *) key);        
      else
        return gZclNoMem_c;    
    return gZclSuccess_c;  
    } else
     return gZclFailure_c;
  }
  return gZclNoMem_c;
    
}

zbStatus_t ZCL_ESPDeRegisterDevice(EspDeRegisterDevice_t *Devinfo)
{
  index_t Index;
  Index = ZCL_FindIeeeInRegTable(Devinfo->Addr);
  if (Index != 0xff)
  {
    ASL_Mgmt_Leave_req(NULL, (uint8_t *)gaBroadcastRxOnIdle,RegistrationTable[Index].DevInfo.Addr,0);
    APS_RemoveSecurityMaterialEntry(RegistrationTable[Index].DevInfo.Addr);
    APS_RemoveFromAddressMap(Devinfo->Addr);
    Fill8BytesToZero(RegistrationTable[Index].DevInfo.Addr); 
    return gZclSuccess_c;
  } else
    return gZclNoMem_c;   
}

index_t ZCL_FindIeeeInRegTable(zbIeeeAddr_t aExtAddr)
{
  index_t i;

  for(i=0; i<RegistrationTableEntries_c; ++i)
  {
    /* found the entry */
    if(IsEqual8Bytes(RegistrationTable[i].DevInfo.Addr, aExtAddr))
      return i;
  }
  return 0xFF;
}

index_t ZCL_AddToRegTable(EspRegisterDevice_t *Devinfo)
{
  index_t i;
  index_t iFree;

  /* indicate we haven't found a free one yet */
  iFree = REgTable_InvalidIndex_c;

  for(i=0; i<RegistrationTableEntries_c; ++i)
  {
    /* found the entry */
    if(IsEqual8Bytes(RegistrationTable[i].DevInfo.Addr, Devinfo->Addr))
    {
      /*Ieee address already exist in table, exist and do nothing*/
      return REgTable_InvalidIndex_c;
    }

    /* record first free entry */
    if(iFree == REgTable_InvalidIndex_c && Cmp8BytesToZero(RegistrationTable[i].DevInfo.Addr))
      iFree = i;
  }
  /* return indicating full */
  if(iFree == REgTable_InvalidIndex_c)
    return iFree;

  /* add in new entry */
  FLib_MemCpy((void*) &RegistrationTable[iFree].DevInfo, (void*)Devinfo, sizeof(EspRegisterDevice_t));
#if gEccIncluded_d == 1
  RegistrationTable[iFree].DevStatus = RegTable_DevStatusIntialState_c;
#else
  // If Ecc library is not included, then set that device has done key estab.
  RegistrationTable[iFree].DevStatus = RegTable_DevStatusKeyEstabComplete_c;  
#endif  
  return iFree;
}

void ZCL_ESPInit(void)
{
    ZCL_Register_EspRegisterDeviceFunctions(ZCL_ESPRegisterDevice, ZCL_ESPDeRegisterDevice);
}

void ZCL_SetKeyEstabComplete(zbIeeeAddr_t aExtAddr)
{
    index_t Index;
    Index = ZCL_FindIeeeInRegTable(aExtAddr); 
    if (Index != REgTable_InvalidIndex_c)
    RegistrationTable[Index].DevStatus = RegTable_DevStatusKeyEstabComplete_c;
}  

bool_t ZCL_IsKeyEstablishmentCompleted(zbIeeeAddr_t aExtAddr)
{
    index_t Index;
    Index = ZCL_FindIeeeInRegTable(aExtAddr); 
    if (Index != REgTable_InvalidIndex_c)
      return (RegistrationTable[Index].DevStatus == RegTable_DevStatusKeyEstabComplete_c);  

return FALSE;    
}

bool_t ZCl_SEClusterSecuritycheck(zbApsdeDataIndication_t *pIndication)
{
  uint16_t cluster;

   /*if packets are from our self then do not check security level */
  if (IsSelfNwkAddress(pIndication->aSrcAddr)) 
  {  
    return TRUE;
  }

  Copy2Bytes(&cluster, pIndication->aClusterId);
  /*  cluster = OTA2Native16(cluster);*/
  #if gStandardSecurity_d
  if((cluster == gZclClusterBasic_c)    ||
     (cluster == gZclClusterIdentify_c) ||
     (cluster == gKeyEstabCluster_c)) 
  {
    if((pIndication->fSecurityStatus & gApsSecurityStatus_Nwk_Key_c) == gApsSecurityStatus_Nwk_Key_c)
      return TRUE;   
    else
      return FALSE;
  }


  if((cluster == gZclClusterSmplMet_c) ||
     (cluster == gZclClusterMsg_c)      ||
     (cluster == gZclClusterPrice_c)    ||   
     (cluster == gZclClusterDmndRspLdCtrl_c) ||
     (cluster == gZclClusterTime_c)     ||
     (cluster == gZclClusterCommissioning_c))
   {
#if gApsLinkKeySecurity_d  
    if((pIndication->fSecurityStatus & gApsSecurityStatus_Link_Key_c) == gApsSecurityStatus_Link_Key_c)
      return TRUE;
    else
      return FALSE;
#else
   return TRUE;
#endif    
   }
#endif
  return TRUE; 
}

