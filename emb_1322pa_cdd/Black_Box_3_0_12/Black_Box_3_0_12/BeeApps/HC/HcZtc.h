/******************************************************************************
* Header file for ZTC access to 11073 OEP functionality
*
* Copyright (c) 2010, Freescale, Inc.  All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
* 
*
******************************************************************************/


#ifndef _HC_ZTC_H
#define _HC_ZTC_H

#include "EmbeddedTypes.h"
#include "Oep11073.h"
#include "OepTypes.h"
#include "ZtcInterface.h"

/******************************************************************************
*******************************************************************************
* Public macros
*******************************************************************************
******************************************************************************/


#define oepStatus_Success                             0x00
#define oepStatus_Failed                              0xFE


#define oepObjCfgChoice_GetObjectsReq                 0xF580
#define oepObjCfgChoice_SetNewHandleReq               0xF581
#define oepObjCfgChoice_GetObjectDescriptorReq        0xF582
#define oepObjCfgChoice_GetAttributeReq               0xF583
#define oepObjCfgChoice_SetAttributeReq               0xF584
#define oepObjCfgChoice_SetAttributeEnabledReq        0xF585
#define oepAssocChoice_StartOepAssocRec               0xF586
#define oepReportChoice_SendDataReportReq             0xF587
#define oepAssocChoice_SendAssocReleaseReq            0xF588
#define oepAssocChoice_SendGetMdsReq                  0xF589
#define oepAssocChoice_TransportConnectInd            0xF590
#define oepAssocChoice_TransportDisconnectInd         0xF591

#define oepObjCfgChoice_GetObjectsRsp                 0xF5B0
#define oepObjCfgChoice_SetNewHandleCnf               0xF5B1
#define oepObjCfgChoice_GetObjectDescriptorRsp        0xF5B2
#define oepObjCfgChoice_GetAttributeRsp               0xF5B3
#define oepObjCfgChoice_SetAttributeCnf               0xF5B4
#define oepObjCfgChoice_SetAttributeEnabledCnf        0xF5B5
#define oepObjCfgChoice_DeviceStateChangedEvt         0xF5C0

#define oepChoiceMarker_StartOfCfgRxCmds              oepObjCfgChoice_GetObjectsReq
#define oepChoiceMarker_EndOfCfgRxCmds                oepAssocChoice_TransportDisconnectInd

#define oepChoiceMarker_StartOfCfgTxCmds              oepObjCfgChoice_GetObjectsRsp
#define oepChoiceMarker_EndOfCfgTxCmds                oepObjCfgChoice_DeviceStateChangedEvt



#define gHcZtcObjCfgChoice_HeaderByte                 0xF5

#define gHcZtcOpcode_Aarq_d                           0xE2
#define gHcZtcOpcode_Aare_d                           0xE3 
#define gHcZtcOpcode_Rlrq_d                           0xE4 
#define gHcZtcOpcode_Rlre_d                           0xE5 
#define gHcZtcOpcode_Abrt_d                           0xE6 
#define gHcZtcOpcode_Prst_d                           0xE7 
#define gHcZtcOpcode_RoivCmipEventReport_d            0x10 
#define gHcZtcOpcode_RoivCmipConfirmedEventReport_d   0x11 
#define gHcZtcOpcode_RoivCmipGet_d                    0x13 
#define gHcZtcOpcode_RoivCmipSet_d                    0x14 
#define gHcZtcOpcode_RoivCmipConfirmedSet_d           0x15 
#define gHcZtcOpcode_RoivCmipAction_d                 0x16 
#define gHcZtcOpcode_RoivCmipConfirmedAction_d        0x17 
#define gHcZtcOpcode_RorsCmipConfirmedEventReport_d   0x21 
#define gHcZtcOpcode_RorsCmipGet_d                    0x23 
#define gHcZtcOpcode_RorsCmipConfirmedSet_d           0x25 
#define gHcZtcOpcode_RorsCmipConfirmedAction_d        0x27 
#define gHcZtcOpcode_Roer_d                           0x30 
#define gHcZtcOpcode_Rorj_d                           0x40 
#define gHcZtcOpcode_FragmentContinuation_d           0xFD
#define gHcZtcOpcode_OepInvalid_d                     0xFE


#define gHcZtcOpcode_GetObjectsReq_d                  0x80
#define gHcZtcOpcode_SetNewHandleReq_d                0x81
#define gHcZtcOpcode_GetObjectDescriptorReq_d         0x82
#define gHcZtcOpcode_GetAttributeReq_d                0x83
#define gHcZtcOpcode_SetAttributeReq_d                0x84
#define gHcZtcOpcode_SetAttributeEnabledReq_d         0x85
#define gHcZtcOpcode_StartOepAssocReq_d               0x86
#define gHcZtcOpcode_SendDataReportReq_d              0x87
#define gHcZtcOpcode_SendAssocReleaseReq_d            0x88
#define gHcZtcOpcode_SendGetMdsReq_d                  0x89

#define gHcZtcOpcode_GetObjectsRsp_d                  0xB0
#define gHcZtcOpcode_SetNewHandleCnf_d                0xB1
#define gHcZtcOpcode_GetObjectDescriptorRsp_d         0xB2
#define gHcZtcOpcode_GetAttributeRsp_d                0xB3
#define gHcZtcOpcode_SetAttributeCnf_d                0xB4
#define gHcZtcOpcode_SetAttributeEnabledCnf_d         0xB5
#define gHcZtcOpcode_DeviceStateChangedEvt_d          0xC0

#define gHcZtcOpcodeMarker_StartOfCfgRxCmds           gHcZtcOpcode_GetObjectsReq_d
#define gHcZtcOpcodeMarker_EndOfCfgRxCmds             gHcZtcOpcode_SendGetMdsReq_d

#define gHcZtcOpcodeMarker_StartOfCfgTxCmds           gHcZtcOpcode_GetObjectsRsp_d
#define gHcZtcOpcodeMarker_EndOfCfgTxCmds             gHcZtcOpcode_DeviceStateChangedEvt_d

#define mSapHandlerMsgTotalLen_c    144


/******************************************************************************
*******************************************************************************
* Public prototypes
*******************************************************************************
******************************************************************************/


/******************************************************************************
*******************************************************************************
* Public type definitions
*******************************************************************************
******************************************************************************/

typedef uint8_t   oepStatus_t;


#ifdef __IAR_SYSTEMS_ICC__
#pragma pack(1)
#endif


typedef struct Oem11073ZtcFrame_tag {
 ztcMsgType_t opCodeId;
 zclApdu_t *pApdu;
} Oem11073ZtcFrame_t;

typedef struct Oem11073Ztc_OpCodeIdMapEntry_tag {
  uint16_t oemFrameId;
  ztcMsgType_t opCodeId;
} Oem11073Ztc_OpCodeIdMapEntry_t;


typedef enum {
  oepAttributeEnabled_False,
  oepAttributeEnabled_True
} oepAttributeEnabled_t;


typedef struct prstGeneric_tag
{
  uint16_t apduChoice; 
  uint16_t apduLength; 
  uint16_t prstLength; 
  invokeId_t invokeId;  
  uint16_t prstChoice;      
} prstGeneric_t;


typedef struct oepGenericFrame_tag
{
  uint16_t choice; 
  uint16_t length; 
  uint8_t  data[1];
} oepGenericFrame_t;

typedef struct hcZtcMessage_tag {
  uint8_t msgType;
  uint8_t msgLen;
  uint8_t data[1];
} hcZtcMessage_t;

typedef struct hcZtcObjCfgFrame_tag {
  uint8_t nextFragmentPresent;
  zbEndPoint_t endPoint;
  uint8_t objCfgFrame[1];
} hcZtcObjCfgFrame_t;

typedef struct hcZtcFrame_tag {
  uint8_t nextFragmentPresent;
  zbNwkAddr_t aSrcDstAddr;
	zbEndPoint_t srcEndPoint; 
  zbEndPoint_t dstEndPoint; 
  uint8_t apdu[1];
} hcZtcFrame_t;
 
 typedef struct hcZtcFragmentContinuationFrame_tag {
  uint8_t nextFragmentPresent;
  uint8_t fragmentData[1];
} hcZtcFragmentContinuationFrame_t;

typedef struct oepGetObjectsRsp_tag {
  uint8_t  objCount;
  uint8_t  objHandlesTypesLists[1];
} oepGetObjectsRsp_t;

typedef struct oepSetNewHandleReq_tag {
  oepHandle_t oldHandle;
  oepHandle_t newHandle; 
} oepSetNewHandleReq_t;

typedef struct oepSetNewHandleCnf_tag {
  oepStatus_t status;
} oepSetNewHandleCnf_t;

typedef struct oepGetObjectDescriptorReq_tag {
  oepHandle_t objHandle;
} oepGetObjectDescriptorReq_t;

typedef struct oepGetObjectDescriptorRsp_tag {
  oepHandle_t objHandle;
  uint8_t attrCount;
  uint8_t attrTypeList[1];
} oepGetObjectDescriptorRsp_t;

typedef struct oepGetAttributeReq_tag {
  oepHandle_t objHandle;
  oidType_t attrType;
} oepGetAttributeReq_t;

typedef struct oepGetAttributeRsp_tag {
  oepHandle_t objHandle;
  oidType_t attrType;
  oepStatus_t status;
  uint8_t valueLen;
  uint8_t value[1];
} oepGetAttributeRsp_t;

typedef struct oepSetAttributeReq_tag {
  oepHandle_t objHandle;
  oidType_t attrType;
  uint8_t  valueLen;
  uint8_t  value[1];
} oepSetAttributeReq_t;

typedef struct oepSetAttributeCnf_tag {
  oepHandle_t objHandle;
  oidType_t attrType;
  oepStatus_t status;
} oepSetAttributeCnf_t;


typedef struct oepSetAttributeEnabledReq_tag {
  oepHandle_t objHandle;
  oidType_t attrType;
  oepAttributeEnabled_t isEnabled;
} oepSetAttributeEnabledReq_t;

typedef struct oepSetAttributeEnabledCnf_tag {
  oepHandle_t objHandle;
  oidType_t attrType;
  oepStatus_t status;
} oepSetAttributeEnabledCnf_t;

typedef struct hcZtcSendDataReport_tag {
  uint16_t invokeId;
  uint16_t dataReqId;
  uint16_t scanReportNo;
} hcZtcSendDataReport_t;

typedef struct hcZtcSendAssocReleaseReq_tag {
  uint8_t dstAddr[2];
  uint8_t  dstEndPoint;
  uint16_t reason;
} hcZtcSendAssocReleaseReq_t;

typedef struct hcZtcSendGetMdsRequest_tag {
  uint8_t dstAddr[2];
  uint8_t  dstEndPoint;
  uint16_t invokeId;
} hcZtcSendGetMdsRequest_t;

typedef struct oepObjCfgPayload_tag {
  uint16_t apduLen;
  uint16_t choice;
  union {
    // hcZtcGetObjectsReq_t          getObjectReq;
    oepGetObjectsRsp_t          getObjectRsp;
    oepSetNewHandleReq_t        setNewHandleReq;
    oepSetNewHandleCnf_t        setNewHandleCnf;
    oepGetObjectDescriptorReq_t getObjectDescriptorReq;
    oepGetObjectDescriptorRsp_t getObjectDescriptorRsp;
    oepGetAttributeReq_t        getAttributeReq;
    oepGetAttributeRsp_t        getAttributeRsp;
    oepSetAttributeReq_t        setAttributeReq;
    oepSetAttributeCnf_t        setAttributeCnf;
    oepSetAttributeEnabledReq_t setAttributeEnabledReq;
    oepSetAttributeEnabledCnf_t setAttributeEnabledCnf;
    
  } frame;
} oepObjCfgFrame_t;


typedef enum {
  msgType_ObjCfgMsg,
  msgType_PrstMsg,
  msgType_General,
} oepMsgType_t;


#ifdef __IAR_SYSTEMS_ICC__
#pragma pack()
#endif

/******************************************************************************
*******************************************************************************
* Public memory declarations
*******************************************************************************
******************************************************************************/


/******************************************************************************
*******************************************************************************
* Public functions
*******************************************************************************
******************************************************************************/
uint8_t AppHcZtc_SAPHandler(hcZtcMessage_t *ztcMessage);
uint8_t HcAppZtc_SAPHandler(hcZtcMessage_t *ztcMessage);
void HcOutgoingZtcTaskEventMonitor(OepOutgoingMessage_t *message);
void HcIncomingZtcTaskEventMonitor(OepFragmentedApdu_t *message);
oepMsgType_t GetOepMsgType(oepGenericApdu_t *pApdu);
#endif /*_HC_ZTC_H */
