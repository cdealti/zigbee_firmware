/*****************************************************************************
* Implementation file for ZTC access to 11073 OEP functionality
*
*
* Copyright (c) 2010, Freescale, Inc. All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
*
*****************************************************************************/

#include "HcZtc.h"


/******************************************************************************
*******************************************************************************
* Private type definitions
*******************************************************************************
******************************************************************************/


/******************************************************************************
*******************************************************************************
* Private Prototypes
*******************************************************************************
******************************************************************************/

/******************************************************************************
*******************************************************************************
* Private Memory Declarations
*******************************************************************************
******************************************************************************/

const Oem11073Ztc_OpCodeIdMapEntry_t Oem11073Ztc_OpCodeIdApduChoiceMap[] = { 
  { gAarq_d, gHcZtcOpcode_Aarq_d },
  { gAare_d, gHcZtcOpcode_Aare_d},
  { gRlrq_d, gHcZtcOpcode_Rlrq_d },
  { gRlre_d, gHcZtcOpcode_Rlre_d},
  { gAbrt_d, gHcZtcOpcode_Abrt_d },
  { gPrst_d, gHcZtcOpcode_Prst_d },  
};

const Oem11073Ztc_OpCodeIdMapEntry_t Oem11073Ztc_OpCodeIdPrstChoiceMap[] = { 
  { gRoivCmipEventReport_d, gHcZtcOpcode_RoivCmipEventReport_d },
  { gRoivCmipConfirmedEventReport_d, gHcZtcOpcode_RoivCmipConfirmedEventReport_d},  
  { gRoivCmipGet_d, gHcZtcOpcode_RoivCmipGet_d},
  { gRoivCmipSet_d, gHcZtcOpcode_RoivCmipSet_d },
  { gRoivCmipConfirmedSet_d, gHcZtcOpcode_RoivCmipConfirmedSet_d},  
  { gRoivCmipAction_d, gHcZtcOpcode_RoivCmipAction_d},
  { gRoivCmipConfirmedAction_d, gHcZtcOpcode_RoivCmipConfirmedAction_d },
  { gRorsCmipConfirmedEventReport_d, gHcZtcOpcode_RorsCmipConfirmedEventReport_d},  
  { gRorsCmipGet_d, gHcZtcOpcode_RorsCmipGet_d},
  { gRorsCmipConfirmedSet_d, gHcZtcOpcode_RorsCmipConfirmedSet_d },
  { gRorsCmipConfirmedAction_d, gHcZtcOpcode_RorsCmipConfirmedAction_d},  
  { gRoer_d, gHcZtcOpcode_Roer_d},
  { gRorj_d, gHcZtcOpcode_Rorj_d }
       
};

/******************************************************************************
*******************************************************************************
* Public memory declarations
*******************************************************************************
******************************************************************************/


/******************************************************************************
*******************************************************************************
* Private Functions
*******************************************************************************
******************************************************************************/
  
  
/*****************************************************************************
  GetMsgZtcOpCode
  Returns an opcode id for healtcare frames by using arrays that map the
  2 byte choice ids inside the frames to 1-byte opcode ids
*****************************************************************************/  
uint8_t GetMsgZtcOpCode(oepGenericApdu_t *pApdu) {

  int i;
  oepMsgType_t msgType = GetOepMsgType(pApdu);
  
  if (msgType == msgType_ObjCfgMsg)
    return (uint8_t)pApdu->choice & 0xFF;
  
  if (msgType == msgType_PrstMsg) {

    prstGeneric_t *pPrstFrame = (prstGeneric_t *)&pApdu->choice;    
    
    for (i = 0; i < NumberOfElements(Oem11073Ztc_OpCodeIdPrstChoiceMap); i++)
        /* check the choice id inside the Presentation Frame */
        if (Oem11073Ztc_OpCodeIdPrstChoiceMap[i].oemFrameId == pPrstFrame->prstChoice)
          /* return corresponding opcodeId from the map in case of a match */
          return Oem11073Ztc_OpCodeIdPrstChoiceMap[i].opCodeId;
  }
  
  /* iterate apdu choice-opcode map */
  for (i = 0; i < NumberOfElements(Oem11073Ztc_OpCodeIdApduChoiceMap); i++)
    /* check the choice id inside the apdu */
    if (Oem11073Ztc_OpCodeIdApduChoiceMap[i].oemFrameId == pApdu->choice)
      /* return corresponding opcodeId from the map in case of a match */
      return Oem11073Ztc_OpCodeIdApduChoiceMap[i].opCodeId;  
      
  return gHcZtcOpcode_OepInvalid_d;
  

}

/******************************************************************************
*******************************************************************************
* Public Functions
*******************************************************************************
******************************************************************************/


/*****************************************************************************
  AppHcZtc_SAPHandler
  Called from ZTC to make a request to the 11073 healthcare layer
*****************************************************************************/

uint8_t AppHcZtc_SAPHandler(hcZtcMessage_t *ztcMessage) {
  
  OepFragmentedApdu_t* pOepFragmentedApdu = (OepFragmentedApdu_t *)ztcMessage;
  
  uint8_t dataLenToCopy;
  uint8_t dstOffset = MbrOfs(OepFragmentedApdu_t, fragment) + MbrOfs(OepApduFragment_t, data);
  uint8_t srcOffset = MbrOfs(hcZtcMessage_t, data);
  
  if (ztcMessage->msgType >= gHcZtcOpcodeMarker_StartOfCfgRxCmds &&
      ztcMessage->msgType <= gHcZtcOpcodeMarker_EndOfCfgRxCmds) {
    
    oepObjCfgFrame_t* pZtcFrame;
    hcZtcObjCfgFrame_t* pZtcObjCfgFrame;
    
    dstOffset += MbrOfs(oepObjCfgFrame_t, frame);
    srcOffset += MbrOfs(hcZtcObjCfgFrame_t, objCfgFrame);
    dataLenToCopy = ztcMessage->msgLen - MbrSizeof(hcZtcObjCfgFrame_t, endPoint)
                    - MbrSizeof(hcZtcFrame_t, nextFragmentPresent);
    
    /* ZHCTODO support fragmentation */
    FLib_MemInPlaceCpy((uint8_t *)ztcMessage + dstOffset, (uint8_t *)ztcMessage + srcOffset, dataLenToCopy);
    pZtcFrame = (oepObjCfgFrame_t *)pOepFragmentedApdu->fragment.data;
    
    pZtcFrame->choice = (gHcZtcObjCfgChoice_HeaderByte << 8) | ztcMessage->msgType;
    pZtcFrame->apduLen = MbrSizeof(oepObjCfgFrame_t, choice) + dataLenToCopy;
    
    pOepFragmentedApdu->fragment.len = pZtcFrame->apduLen + MbrSizeof(oepObjCfgFrame_t, apduLen);
        
    pZtcObjCfgFrame = (hcZtcObjCfgFrame_t *)ztcMessage->data;
    pOepFragmentedApdu->dstEndPoint = pZtcObjCfgFrame->endPoint;
    
    /* ZHCTODO support fragmentation */ 
    pOepFragmentedApdu->fragment.nextFragment = NULL;
  } 
  else
  {
    /* ZHCTODO association and prst frames */ 
    
  }
  
  ZCL_Oep_SAPHandler(pOepFragmentedApdu);
  
  return gZbSuccess_c;
}


/*****************************************************************************
  HcAppZtc_SAPHandler
  Called from ZTC to simulate a reply from the 11073 healthcare layer
*****************************************************************************/

uint8_t HcAppZtc_SAPHandler(hcZtcMessage_t *ztcMessage) {
  (void) ztcMessage;
  
  return gZbSuccess_c;
}



void HcIncomingZtcTaskEventMonitor(OepFragmentedApdu_t *message) {
  
  hcZtcMessage_t* pMsgFromSAPHandler;
  
  /* pCurrentFragment points initially to the fragment in OepFragmentedApdu_t */   
  OepApduFragment_t* pCurrentFragment = &message->fragment;
  
  /* cleaner access to the 11073 apdu */   
  oepGenericApdu_t *pApdu = (oepGenericApdu_t *)pCurrentFragment->data;
  
  
  /* first fragment flag is initially set to True */   
  uint8_t firstFragment = TRUE;

  /* iterate the fragments */   
  do {
  
    uint8_t srcOffset, dstOffset, dataCopyLen;
  
    /* allocate a new buffer as the ZTC frames are quite different from the actual
       11073 frames */   
    pMsgFromSAPHandler = (hcZtcMessage_t *) MSG_Alloc(mSapHandlerMsgTotalLen_c);    

    /* set default values for mem copy offsets and length */
    
    /* by default copy data beginning with the choice member in the apdu */      
    srcOffset = MbrOfs(oepGenericApdu_t, choice);
    
    /* by default copy data to the apdu member in the ztc frame*/          
    dstOffset = MbrOfs(hcZtcFrame_t, apdu);    
    
    /* data copy length adjusted to omit apduLen field */              
    dataCopyLen = pCurrentFragment->len - MbrOfs(oepGenericApdu_t, choice);
    
    if (firstFragment) {
      oepMsgType_t oepMsgType; 
    
      firstFragment = FALSE;
    
      /* is it a cfg, prst, other kind of frame? */ 
      oepMsgType = GetOepMsgType(pApdu);
    
      /* get ZTC opcode id for the frame */ 
      pMsgFromSAPHandler->msgType = GetMsgZtcOpCode(pApdu);
      
      /* frame is an object configuration frame */ 
      if (oepMsgType == msgType_ObjCfgMsg) {
      
        /* msgLen is the length of the ZTC payload; this includes the actual
           apdu payload, without the choice and apduLen as well as the src/dstEndPoint
           and nextFragmentPresent bytes */
        pMsgFromSAPHandler->msgLen = (uint8_t)(pCurrentFragment->len - 
                                     MbrOfs(oepGenericApdu_t, payload) +
                                     MbrOfs(hcZtcObjCfgFrame_t, objCfgFrame));
                                     
        /* adjust source offset for data copy to not include choice */ 
        srcOffset = MbrOfs(oepGenericApdu_t, payload);
        
        /* adjust dst offset for data copy as srcAddress and an endpoint are no longer included */
        dstOffset = MbrOfs(hcZtcObjCfgFrame_t, objCfgFrame);
        
        /* adjust data length for data copy as choice is no longer included */
        dataCopyLen -= MbrSizeof(oepGenericApdu_t, choice);
        
        /* set destination endpoint as second byte in the ZTC
           packet based on frame direction */
        pMsgFromSAPHandler->data[1] = message->dstEndPoint;
                                     
      }
      else {
        /* Aarq, Aars, Prst, etc type of frame */
        hcZtcFrame_t* pZtcFrameGeneric = (hcZtcFrame_t *)pMsgFromSAPHandler->data;
        
        /* msgLen is the length of the ZTC payload; this includes the actual
           apdu payload, without the apduLen as well as the addressing information
           and nextFragmentPresent bytes */
        pMsgFromSAPHandler->msgLen = pCurrentFragment->len -
                                     MbrOfs(oepGenericApdu_t, choice) +
                                     MbrOfs(hcZtcFrame_t, apdu);
                                     
        /* copy addressing info */
        Copy2Bytes(pZtcFrameGeneric->aSrcDstAddr, message->aSrcAddr);
        pZtcFrameGeneric->dstEndPoint = message->dstEndPoint;
        pZtcFrameGeneric->srcEndPoint = message->srcEndPoint;
      }
    }
    else 
    {
      /* this is not the first fragment; */
      
      /* set data copy and copy size accordingly */ 
      srcOffset = 0;
      dstOffset = 1;
      dataCopyLen = pCurrentFragment->len;
      
      pApdu =  (oepGenericApdu_t *)pCurrentFragment->data;
      
      /* set fragment continuation opcode */
      pMsgFromSAPHandler->msgType = gHcZtcOpcode_FragmentContinuation_d;
      
        /* msgLen includes the actualapdu payload and the nextFragmentPresent byte */      
      pMsgFromSAPHandler->msgLen = pCurrentFragment->len +
                                   MbrSizeof(hcZtcFrame_t, nextFragmentPresent);
    }
    
    /* nextFragmentPresent */
    pMsgFromSAPHandler->data[0] = FALSE;
    
    /* set nextFramgentPresent byte in ZTC frame to 1 if there is
       another fragment to be processed */
    if (pCurrentFragment->nextFragment != NULL)
      pMsgFromSAPHandler->data[0] = TRUE;
    
    /* do the fragment iteration */
    pCurrentFragment = pCurrentFragment->nextFragment;

    /* copy payload data */
    FLib_MemCpy((uint8_t*)pMsgFromSAPHandler->data + dstOffset,
                (uint8_t*)pApdu + srcOffset,
                dataCopyLen);
    
    /* send message to ZTC */
#ifndef gHostApp_d        
    ZTC_TaskEventMonitor(gAppHc_SAPHandlerId_c, (uint8_t *)pMsgFromSAPHandler, gZbSuccess_c);
#else
    ZTC_TaskEventMonitor(gpHostAppUart, gAppHc_SAPHandlerId_c, (uint8_t *)pMsgFromSAPHandler, gZbSuccess_c);
#endif     
    /* free the message if ZTC hasn't already done that */
    if (pMsgFromSAPHandler)
      MSG_Free(pMsgFromSAPHandler);
    
  } while (pCurrentFragment != NULL);
}

void HcOutgoingZtcTaskEventMonitor(OepOutgoingMessage_t *message) {
  
  
  hcZtcMessage_t* pMsgFromSAPHandler;
  
  /* cleaner access to the 11073 apdu */   
  oepGenericApdu_t *pApdu = (oepGenericApdu_t *)message->pApdu;
  
  uint8_t* pSrcDataPtr;
  uint8_t* pDstDataPtr;
  
  /* first fragment flag is initially set to True */   
  uint8_t firstFragment = TRUE;
  uint16_t apduLen = message->length - MbrSizeof(oepGenericApdu_t, apduLen);
  
  uint8_t dataCopyLen;

  /* iterate the fragments */   
  do
  {
    /* allocate a new buffer as the ZTC frames are quite different from the actual
       11073 frames */   
    pMsgFromSAPHandler = (hcZtcMessage_t *) MSG_Alloc(mSapHandlerMsgTotalLen_c);    

    if (firstFragment) {
    
      oepMsgType_t oepMsgType; 
    
      firstFragment = FALSE;
      
      /* is it a cfg, prst, other kind of frame? */ 
      oepMsgType = GetOepMsgType(pApdu);
    
      /* get ZTC opcode id for the frame */ 
      pMsgFromSAPHandler->msgType = GetMsgZtcOpCode(pApdu);
      
      
      /* frame is an object configuration frame */ 
      if (oepMsgType == msgType_ObjCfgMsg) {
      
        hcZtcObjCfgFrame_t* pHcZtcObjCfgFrame = (hcZtcObjCfgFrame_t *)pMsgFromSAPHandler->data;
        
        apduLen -= MbrSizeof(oepGenericApdu_t, choice);
        
        if (apduLen > mSapHandlerMsgTotalLen_c - MbrOfs(hcZtcObjCfgFrame_t, objCfgFrame)) {
        
          dataCopyLen = mSapHandlerMsgTotalLen_c - MbrOfs(hcZtcObjCfgFrame_t, objCfgFrame);
          apduLen -= dataCopyLen;
          pHcZtcObjCfgFrame->nextFragmentPresent = TRUE;
          
        }
        else {
        
          dataCopyLen = (uint8_t)apduLen;
          apduLen = 0;
          pHcZtcObjCfgFrame->nextFragmentPresent = FALSE;
          
        }
      
        pMsgFromSAPHandler->msgLen = dataCopyLen + MbrOfs(hcZtcObjCfgFrame_t, objCfgFrame);
                                     
        pSrcDataPtr = (uint8_t *)pApdu->payload;
        
        pDstDataPtr = (uint8_t *)pHcZtcObjCfgFrame->objCfgFrame; 
        
        pHcZtcObjCfgFrame->endPoint = message->srcEndPoint;
        
      }
      else {
        /* Aarq, Aars, Prst, etc type of frame */
        hcZtcFrame_t* pZtcFrameGeneric = (hcZtcFrame_t *)pMsgFromSAPHandler->data;
        
        if (apduLen > mSapHandlerMsgTotalLen_c - MbrOfs(hcZtcFrame_t, apdu)) {
        
          dataCopyLen = mSapHandlerMsgTotalLen_c - MbrOfs(hcZtcFrame_t, apdu);
          apduLen -= dataCopyLen;
          pZtcFrameGeneric->nextFragmentPresent = TRUE;
          
        } 
        else {

          dataCopyLen = (uint8_t)apduLen;
          apduLen = 0;
          pZtcFrameGeneric->nextFragmentPresent = FALSE;
          
        }
        
        pMsgFromSAPHandler->msgLen = dataCopyLen + MbrOfs(hcZtcFrame_t, apdu);
        
        pSrcDataPtr = (uint8_t *)&pApdu->choice;
        
        pDstDataPtr = (uint8_t *)pZtcFrameGeneric->apdu;
                                
        Copy2Bytes(pZtcFrameGeneric->aSrcDstAddr, message->aDstAddr);
        pZtcFrameGeneric->dstEndPoint = message->dstEndPoint;
        pZtcFrameGeneric->srcEndPoint = message->srcEndPoint;
      }
    }
    else 
    {
      /* this is not the first fragment; */
      
      hcZtcFragmentContinuationFrame_t* pZtcFragmentCont = 
                          (hcZtcFragmentContinuationFrame_t *)pMsgFromSAPHandler->data;
                          
      /* set fragment continuation opcode */
      pMsgFromSAPHandler->msgType = gHcZtcOpcode_FragmentContinuation_d;
      
      if (apduLen > mSapHandlerMsgTotalLen_c - 
          MbrOfs(hcZtcFragmentContinuationFrame_t, fragmentData)) {
        
        dataCopyLen = mSapHandlerMsgTotalLen_c -  
                      MbrOfs(hcZtcFragmentContinuationFrame_t, fragmentData);
        apduLen -= dataCopyLen;
        pZtcFragmentCont->nextFragmentPresent = TRUE;
        
      } 
      else {

        dataCopyLen = (uint8_t)apduLen;
        apduLen = 0;
        pZtcFragmentCont->nextFragmentPresent = FALSE;
        
      }
      
      pMsgFromSAPHandler->msgLen = dataCopyLen +
                           MbrSizeof(hcZtcFragmentContinuationFrame_t, fragmentData);

      pDstDataPtr = (uint8_t *)pZtcFragmentCont->fragmentData;
    }
    
    FLib_MemCpy(pDstDataPtr, pSrcDataPtr, dataCopyLen);
    
    pSrcDataPtr += dataCopyLen;
    
    /* send message to ZTC */
#ifndef gHostApp_d        
    ZTC_TaskEventMonitor(gHcApp_SAPHandlerId_c, (uint8_t *)pMsgFromSAPHandler, gZbSuccess_c);
#else
    ZTC_TaskEventMonitor(gpHostAppUart, gHcApp_SAPHandlerId_c, (uint8_t *)pMsgFromSAPHandler, gZbSuccess_c);
#endif     

    /* free the message if ZTC hasn't already done that */
    if (pMsgFromSAPHandler)
      MSG_Free(pMsgFromSAPHandler);
    
  } while (apduLen > 0);
}

/*****************************************************************************
  GetOepMsgType
  Returns the type of apdu: object configuration, Prst or generic
*****************************************************************************/  
oepMsgType_t GetOepMsgType(oepGenericApdu_t *pApdu) {

  uint16_t choice = pApdu->choice;

  if (choice >= oepChoiceMarker_StartOfCfgRxCmds &&
      choice <= oepChoiceMarker_EndOfCfgRxCmds ||
      choice >= oepChoiceMarker_StartOfCfgTxCmds &&
      choice <= oepChoiceMarker_EndOfCfgTxCmds)
    return msgType_ObjCfgMsg;

  if (choice == gPrst_d)
    return msgType_PrstMsg;
  
  return msgType_General;
}
