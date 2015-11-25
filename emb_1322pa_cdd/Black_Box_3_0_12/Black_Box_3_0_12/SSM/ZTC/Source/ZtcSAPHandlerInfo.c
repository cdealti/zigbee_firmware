/******************************************************************************
* Data and functions related to information about SAP Handlers.
*
* Copyright (c) 2008, Freescale, Inc.  All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from  Freescale Semiconductor.
*
* This file contains definitions of read-only data used by Ztc.
* Due to their size, the opcode description tables are in a separate file.
*
******************************************************************************/

#include "EmbeddedTypes.h"
#if gBeeStackIncluded_d
#include "ZigBee.h"
#include "BeeStackFunctionality.h"
#include "BeeStackConfiguration.h"
#endif

#include "ZtcInterface.h"


#if gMacStandAlone_d
#include "Mapp.h"
#endif


#if gZtcIncluded_d
#if gBeeStackIncluded_d
#include "BeeCommon.h"
#endif


#include "AppAspInterface.h"
#if gBeeStackIncluded_d
#include "AppZdoInterface.h"
#include "NwkMacInterface.h"
#include "AfApsInterface.h"
#include "ZdoApsInterface.h"
#include "AppAfInterface.h"
#include "NwkCommon.h"
#include "ZdoMain.h"
#include "ApsMgmtInterface.h"
#include "ApsDataInterface.h"
#endif /* gBeeStackIncluded_d */
#include "AspZtc.h"
#include "LlcZtc.h"
#include "Display.h"

#include "ZtcPrivate.h"
#include "ZtcClientCommunication.h"
#include "ZtcMsgTypeInfo.h"
#include "ZtcSAPHandlerInfo.h"
#include "ZtcCopyData.h"
#include "Telec.h"
#ifdef gHcGenericApp_d
#if gHcGenericApp_d
#include "HcZtc.h"
#endif
#endif

/******************************************************************************
*******************************************************************************
* Public memory definitions
*******************************************************************************
******************************************************************************/

/* Master table of information about SAP Handlers. */
#define Sap(opcodeGroup, converse, intSAPIdName, \
            pSAPFunc, modeTableIndex, msgQueue, msgEvent, pTaskID, \
            msgTypeTable, msgTypeTableLen) \
    extern ztcMsgTypeInfo_t const msgTypeTable[]; \
    extern index_t const msgTypeTableLen;
#include "ZtcSAPHandlerInfoTbl.h"

#if (gZtcMacGetSetPIBCapability_d || \
     gSAPMessagesEnableMcps_d     || \
     gSAPMessagesEnableMlme_d     || \
     gSAPMessagesEnableAsp_d      || \
     gZtcErrorReporting_d         || \
     gSAPMessagesEnableNlde_d     || \
     gSAPMessagesEnableNlme_d     || \
     gSAPMessagesEnableApsde_d    || \
     gSAPMessagesEnableAfde_d     || \
     gSAPMessagesEnableApsme_d    || \
		gSAPMessagesEnableZdp_d || \
		gSAPMessagesEnableHc_d)
#define Sap(opcodeGroup, converse, intSAPIdName, \
            pSAPFunc, modeTableIndex, msgQueue, msgEvent, pTaskID, \
            msgTypeTable, msgTypeTableLen) \
  {opcodeGroup, converse, intSAPIdName, pSAPFunc, modeTableIndex, \
   msgQueue, msgEvent, pTaskID, msgTypeTable, &msgTypeTableLen},
ztcSAPHandlerInfo_t const gaZtcSAPHandlerInfo[] = {
#include "ZtcSAPHandlerInfoTbl.h"
};
#else
#define Sap(opcodeGroup, converse, intSAPIdName, \
            pSAPFunc, modeTableIndex, msgQueue, msgEvent, pTaskID, \
            msgTypeTable, msgTypeTableLen) \
  {opcodeGroup, converse, intSAPIdName, pSAPFunc, modeTableIndex, \
   msgQueue, msgEvent, pTaskID, msgTypeTable, &msgTypeTableLen},
ztcSAPHandlerInfo_t const gaZtcSAPHandlerInfo[1] = {0xFF, 0xFF, 0xFF, NULL, 0xFF, NULL, 0xFF, NULL, NULL, NULL};
#endif
/****************************************************************************/

/* Current SAP Handler modes. One byte per SAP Handler. There are only three */
/* modes, so 2 bits would be enough, but there aren't very many SAP Handlers. */
#define SapMode(name, index, defaultMode)   defaultMode,

ztcSAPMode_t maZtcSAPModeTable[] = {    /* Cannot be const. */
#include "ZtcSAPHandlerInfoTbl.h"
};
index_t const mZtcSAPModeTableLen = NumberOfElements(maZtcSAPModeTable);

/******************************************************************************
*******************************************************************************
* Private memory declarations
*******************************************************************************
******************************************************************************/

#define DisabledSap(opcodeGroup) opcodeGroup,

/* Used by pZtcSAPInfoFromOpcodeGroup() to recognized opcode groups */
/* that have been compiled out, to improve error messages. */
#if gZtcErrorReporting_d
ztcOpcodeGroup_t const maDisabledOpcodeGroups[] = {
#include "ZtcSAPHandlerInfoTbl.h"
  gZtcInvalidOpcodeGroup_c              /* End of table mark. */
};
#endif

/******************************************************************************
*******************************************************************************
* Public functions
*******************************************************************************
******************************************************************************/

/* Given an internal SAP Handler id, return a pointer to the corresponding
 * entry in the SAP Handler info table. If the internal SAP Handler id is
 * not found in the table, return NULL.
 */
ztcSAPHandlerInfo_t const *pZtcSAPInfoFromIntSAPId
  (
  ztcIntSAPId_t const intSAPId
  )
{
  index_t i;

  for (i = 0; i < NumberOfElements(gaZtcSAPHandlerInfo); ++i) {
    if (gaZtcSAPHandlerInfo[i].intSAPId == intSAPId) {
      return &(gaZtcSAPHandlerInfo[i]);
    }
  }

  return NULL;
}                                       /* pZtcSAPInfoFromIntSAPId() */

/****************************************************************************/

/* Given an opcode group, return a pointer to the corresponding entry in the
 * SAP Handler info table. If the opcode group is not found in the table,
 * return NULL. If the opcode group is found in the disables table, return
 * 0x0001.
 */
ztcSAPHandlerInfo_t const *pZtcSAPInfoFromOpcodeGroup
  (
  ztcOpcodeGroup_t const opcodeGroup
  )
{
  index_t i;

  for (i = 0; i < NumberOfElements(gaZtcSAPHandlerInfo); ++i) {
    if (gaZtcSAPHandlerInfo[i].opcodeGroup == opcodeGroup) {
      return &(gaZtcSAPHandlerInfo[i]);
    }
  }

#if gZtcErrorReporting_d
  i = 0;
  while (maDisabledOpcodeGroups[ i ] != gZtcSAPModeInvalid_c) {
    if (maDisabledOpcodeGroups[ i++ ] == opcodeGroup) {
      return gZtcIsDisabledOpcodeGroup_c;
    }
  }
#endif

  return NULL;
}                                       /* pZtcSAPInfoFromOpcodeGroup() */

/*
  Synchronous calls to the ASP handler.
*/
uint8_t APP_ASP_SapHandler(AppToAspMessage_t *pMsg)
{
  uint8_t msgStatus = gZbSuccess_c;
  uint32_t auxAddrAlign;
#if gSCIInterface_d  
  uint8_t  byte;
#endif  
  switch(pMsg->msgType) {
    case aspMsgTypeGetTimeReq_c:
      Asp_GetTimeReq(&auxAddrAlign);
      pMsg->msgData.req.aspGetTimeReq.time= auxAddrAlign;
      break;

    case aspMsgTypeWakeReq_c:
      Asp_WakeReq();
      break;

    case aspMsgTypeGetMacStateReq_c:
      msgStatus = Asp_GetMacStateReq();
      break;

#if gAspPowerSaveCapability_d
    case aspMsgTypeDozeReq_c:
      msgStatus = Asp_DozeReq(&pMsg->msgData.req.aspDozeReq.dozeDuration,pMsg->msgData.req.aspDozeReq.clko_en);
      break;

    case aspMsgTypeAutoDozeReq_c:
      Asp_AutoDozeReq(pMsg->msgData.req.aspAutoDozeReq.autoEnable,
                      pMsg->msgData.req.aspAutoDozeReq.enableWakeIndication,
                      &pMsg->msgData.req.aspAutoDozeReq.autoDozeInterval,
                      pMsg->msgData.req.aspAutoDozeReq.clko_en);
      break;

    case aspMsgTypeSetMinDozeTimeReq_c:
      Asp_SetMinDozeTimeReq(&pMsg->msgData.req.aspSetMinDozeTimeReq.minDozeTime);
      break;

    case aspMsgTypeAcomaReq_c:
      msgStatus = Asp_AcomaReq(pMsg->msgData.req.aspAcomaReq.clko_en);
      break;

    case aspMsgTypeHibernateReq_c:
      msgStatus = Asp_HibernateReq();
      break;
#endif

#if gAspHwCapability_d
    case aspMsgTypeClkoReq_c:
      msgStatus = Asp_ClkoReq(pMsg->msgData.req.aspClkoReq.clkoEnable, pMsg->msgData.req.aspClkoReq.clkoRate);
      break;

    case aspMsgTypeTrimReq_c:
      Asp_TrimReq(pMsg->msgData.req.aspTrimReq.fineTune, pMsg->msgData.req.aspTrimReq.coarseTune);
      break;

    case aspMsgTypeDdrReq_c:
      Asp_DdrReq(pMsg->msgData.req.aspDdrReq.directionMask);
      break;

    case aspMsgTypePortReq_c:
      Asp_PortReq(pMsg->msgData.req.aspPortReq.portWrite, &pMsg->msgData.req.aspPortReq.portValue);
      break;
#endif

#if gAspEventCapability_d
    case aspMsgTypeEventReq_c:
      msgStatus = Asp_EventReq(&pMsg->msgData.req.aspEventReq.eventTime);
      break;
#endif

#if gBeaconedCapability_d
    case aspMsgTypeGetInactiveTimeReq_c:
      msgStatus = Asp_GetInactiveTimeReq(&pMsg->msgData.req.aspGetInactiveTimeReq.time);
      break;

    case aspMsgTypeSetNotifyReq_c:
      msgStatus = Asp_SetNotifyReq(pMsg->msgData.req.aspSetNotifyReq.notifications);
      break;
#endif


#if gAspPowerLevelCapability_d
    case aspMsgTypeSetPowerLevel_c:
      msgStatus = Asp_SetPowerLevel(pMsg->msgData.req.aspSetPowerLevelReq.powerLevel);
      break;

    case aspMsgTypeGetPowerLevel_c:
      msgStatus = Asp_GetPowerLevel();
      break;
#endif

    case aspMsgTypeTelecTest_c:
      ASP_TelecTest(pMsg->msgData.req.aspTelecTest.mode);
#if gSCIInterface_d
      if(gTestPulseTxPrbs9_c == pMsg->msgData.req.aspTelecTest.mode)
      {
        while(FALSE == UartX_GetByteFromRxBuffer(&byte))
        {
          ASP_TelecTest(pMsg->msgData.req.aspTelecTest.mode);
        }
        UartX_UngetByte(byte);
      }
#endif
      break;

    case aspMsgTypeTelecSetFreq_c:
      ASP_TelecSetFreq(pMsg->msgData.req.aspTelecsetFreq.channel);
      break;
      
    case aspMsgTypeTelecSendRawData_c:
      if((pMsg->msgData.req.aspTelecSendRawData.length >= 3) &&
         (pMsg->msgData.req.aspTelecSendRawData.length <= 125))
      {
         ASP_TelecSendRawData(pMsg->msgData.req.aspTelecSendRawData.length,
                              (uint8_t*)&pMsg->msgData.req.aspTelecSendRawData.dataPtr);
       }
       else
       {
         msgStatus = gZbInvalidRequest_c; 
       }
      break;      

    default:
      msgStatus = gZbInvalidRequest_c;
      break;

  }

  /* monitoring this task */
  ZTC_TaskEventMonitor(gNwkASP_SAPHandlerId_c, (uint8_t *)pMsg, msgStatus);
  return msgStatus;
}


/*
  Synchronous calls to the LLC handler.
*/

#if gSAPMessagesEnableLlc_d
uint8_t APP_LLC_SapHandler(AppToLlcMessage_t *pMsg)
{
  uint8_t msgStatus = gZbSuccess_c;

  switch(pMsg->msgType) 
  {   
    case gllcMsgTypeSetLedReq_c:
      Llc_SetLedReq(pMsg->req.llcSetLedReq.led1on, pMsg->req.llcSetLedReq.led2on, pMsg->req.llcSetLedReq.led3on, pMsg->req.llcSetLedReq.led4on);
    break;
      
    case gllcMsgTypeStartDACwaveReq_c:
      Llc_Start1kHzDACSinWaveReq(pMsg->req.llcSetVolumeReq.volume);
    break; 
      
    case gllcMsgTypeStopDACwaveReq_c:
      Llc_StopDACSinWaveReq();
    break;
      
    case gllcMsgTypeStartPWMwaveReq_c:
      if(FALSE == Llc_Start1kHzPWMSinWaveReq(pMsg->req.llcSetVolumeReq.volume)){
        msgStatus = gZbInvalidRequest_c;
      }
    break;
         
    case gllcMsgTypeStopPWMwaveReq_c:
      Llc_StopPWMSinWaveReq();
    break;  
      
    case gllcMsgTypeStartPWMReq_c:
      if (FALSE == Llc_Start1KHzPWMReq(pMsg->req.llcSetPwmDutyReq.duty, pMsg->req.llcSetPwmDutyReq.volume)){
        msgStatus = gZbInvalidRequest_c;
      }
    break;  
          
    case gllcMsgTypeStopPWMReq_c:
      Llc_Stop1KHzPWMReq();
    break;  
      
    case gllcMsgTypeReadSensorReq_c:
      Llc_SensorReadReq(pMsg->req.llcReadSensorReq.sensor, pMsg->req.llcReadSensorReq.readVal);
    break;
      
    case gllcMsgTypeSetASStateReq_c:
      Llc_SetASStateReq(pMsg->req.llcSetASStateReq.sleepState);
    break;
      
    case gllcMsgTypeReadSwReq_c:
      Llc_ReadSwReq(pMsg->req.llcReadSwReq.swState);
    break;    
    
    case gllcMsgTypeCrystalTrimmReq_c:
      Llc_CrystalTrimmReq(pMsg->req.llcCrystalTrimmReq.courseTune,pMsg->req.llcCrystalTrimmReq.fineTune);
    break; 
    
    case gllcMsgTypeDisplayChessBoardReq_c:
      LCD_Init();
      Llc_DisplayChessBoardReq(pMsg->req.llcDisplayChessBoardReq.mode,  pMsg->req.llcDisplayChessBoardReq.backLightOn, pMsg->req.llcDisplayChessBoardReq.squareSize);
    break;  

    case gllcMsgTypeStartReceiverReq_c:
      if(TRUE == Llc_StartReceiver(pMsg->req.llcStartReceiverReq.panId, 
                            pMsg->req.llcStartReceiverReq.logicalChannel, 
                            pMsg->req.llcStartReceiverReq.shortAddr)
         )
      {
        msgStatus = gZbSuccess_c; 
      }
      else
      {
        msgStatus = gZbInvalidRequest_c;
      }
    break;
    
    case gllcMsgTypeGetReceivedPacketsReq_c:
      pMsg->req.llcGetReceivedPacketsReq.packetCnt = Llc_GetReceivedPackets();
    break;
    
    case gllcMsgTypeStartTransmitterReq_c:
      if(TRUE == Llc_StartTransmitter(pMsg->req.llcStartTransmitterReq.panId, 
                               pMsg->req.llcStartTransmitterReq.logicalChannel, 
                               pMsg->req.llcStartTransmitterReq.shortAddr)
       )
      {
        msgStatus = gZbSuccess_c; 
      }
      else
      {
        msgStatus = gZbInvalidRequest_c;
      }             
    break;
    
    case gllcMsgTypeTransmitReq_c:
      if(TRUE == Llc_TransmitPackets(pMsg->req.llcTransmitReq.destPanId,
                              pMsg->req.llcTransmitReq.destShortAddr,
                              pMsg->req.llcTransmitReq.packetCount,
                              pMsg->req.llcTransmitReq.len))
      {
        msgStatus = gZbSuccess_c; 
      }
      else
      {
        msgStatus = gZbInvalidRequest_c; 
      }
    break;
    
    case gllcMsgTypeSetupJTAGNEXUSPinsReq_c:
      Llc_SetupJTAGNEXUSPinsReq_c(pMsg->req.llcSetupJTAGNEXUSPinsReq.restore);
    break;
    
    case gllcMsgTypeReadJTAGNEXUSPinsReq_c:
      pMsg->req.llcReadJTAGNEXUSPinsReq.pins = Llc_ReadJTAGNEXUSPinsReq_c();
    break;
    
    case gllcMsgTypeStart12MhzPwmReq_c:
      Llc_Start12MhzPWMReq();
    break;
    
    case gllcMsgTypeStop12MhzPwmReq_c:
      Llc_Stop12MhzPWMReq();
    break;
    
    case gllcMsgSetupGpioPinsReq_c:
      if (TRUE == Llc_SetupGpioPinsReq(&(pMsg->req.llcSetupGpioPinsReq))) 
      {
        msgStatus = gZbSuccess_c;
      }
      else 
      {
        msgStatus = gZbInvalidRequest_c;
      }
    break;
  } /* end switch*/

  /* monitoring this task */
  ZTC_TaskEventMonitor(gNwkLLC_SAPHandlerId_c, (uint8_t *)pMsg, msgStatus);
  return msgStatus;  
}
#endif  /* #if gSAPMessagesEnableLlc_d */


#endif                                  /* #if gZtcIncluded_d == 1 */
 
