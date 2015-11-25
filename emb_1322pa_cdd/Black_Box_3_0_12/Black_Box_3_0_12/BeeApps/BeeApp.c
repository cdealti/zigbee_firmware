/*****************************************************************************
* BeeApp.c
*
*  BlackBox Application
*
* Copyright (c) 2009, Freescale, Inc. All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
*
*****************************************************************************/
#include "EmbeddedTypes.h"
#include "ZigBee.h"
#include "BeeStack_Globals.h"
#include "BeeStackConfiguration.h"
#include "BeeStackParameters.h"
#include "AppZdoInterface.h"
#include "TS_Interface.h"
#include "TMR_Interface.h"
#include "AppAfInterface.h"
#include "FunctionLib.h"
#include "PublicConst.h"
#include "keyboard.h"
#include "Display.h"
#include "Led.h"
#include "Sound.h"
#include "EndPointConfig.h"
#include "BeeApp.h"
#include "BeeAppInit.h"
#include "NVM_Interface.h"
#include "BeeStackInterface.h"
#include "ZdpManager.h"
#include "ZDOStateMachineHandler.h"
#include "ZdoApsInterface.h"
#include "ASL_ZdpInterface.h"
#include "ASL_UserInterface.h"
#include "ZTCInterface.h"
#include "ZtcClientCommunication.h"
#include "Uart_Interface.h"
#if gI2CInterface_d
#include "IIC_Interface.h"
#endif
#if gSPIInterface_d
#include "SPI_Interface.h"
#endif


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
void BeeAppTask(event_t events);
void BeeAppDataIndication(void);
void BeeAppDataConfirm(void);
#if gInterPanCommunicationEnabled_c
void BeeAppInterPanDataConfirm(void);
void BeeAppInterPanDataIndication(void);
#endif 
void BeeAppZdpCallBack(zdpToAppMessage_t *pMsg, zbCounter_t counter);
void BeeAppCallBack(tmrTimerID_t timerId);
void BlackBoxReceiveZtcMessage(ZTCMessage_t* pMsg);

extern const zbCommissioningAttributes_t gSAS_Rom;


/******************************************************************************
*******************************************************************************
* Private Memory Declarations
*******************************************************************************
******************************************************************************/

/******************************************************************************
*******************************************************************************
* Public memory declarations
*******************************************************************************
******************************************************************************/
extern anchor_t gAppDataConfirmQueue;  /* data confirm queue */
extern anchor_t gAppDataIndicationQueue;   /* data indication queue */

/* This data set contains app layer variables to be preserved across resets */
NvDataItemDescription_t const gaNvAppDataSet[] = {
  gAPS_DATA_SET_FOR_NVM,    /* APS layer NVM data */
  /* insert any user data for NVM here.... */
  {NULL, 0}       /* Required end-of-table marker. */
};

zbCommissioningAttributes_t gBlackBoxAttrsData = {
/* initialize like ROM set..*/
  /*** Startup Attribute Set (064699r12, section 6.2.2.1) ***/
  {mDefaultNwkShortAddress_c},        /* x shortAddress (default 0xff,0xff) */
  {mDefaultNwkExtendedPANID_c},       /* x nwkExtendedPANId */
  {mDefultApsUseExtendedPANID_c},     /* x apsUSeExtendedPANId */
  {mDefaultValueOfPanId_c},           /* x panId */
  { (uint8_t)(mDefaultValueOfChannel_c & 0xff),
    (uint8_t)((mDefaultValueOfChannel_c>>8) & 0xff),
    (uint8_t)((mDefaultValueOfChannel_c>>16) & 0xff),
    (uint8_t)((mDefaultValueOfChannel_c>>24) &0xff)
  },                                  /* x channelMask */
  {mNwkProtocolVersion_c},            /* x protocolVersion, always 0x02=ZigBee 2006, 2007 */
  {gDefaultValueOfNwkStackProfile_c}, /* x stackProfile 0x01 or 0x02 */
  {gStartupControl_Associate_c},      /* startupControl */
  {mDefaultValueOfTrustCenterLongAddress_c},  /* x trustCenterAddress */
  {mDefaultValueOfTrustCenterMasterKey_c},  /* trustCenterMasterKey */
  {mDefaultValueOfNetworkKey_c},      /* x networkKey */
  {gApsUseInsecureJoinDefault_c},     /* x useInsecureJoin */
  {mDefaultValueOfTrustCenterLinkKey_c},  /* preconfiguredLinkKey (w/ trust center) */
  {mDefaultValueOfNwkActiveKeySeqNumber_c}, /* x networkKeySeqNum */
  {mDefaultValueOfNwkKeyType_c},      /* x networkKeyType */
  {gNwkManagerShortAddr_c},           /* x networkManagerAddress, little endian */

  /*** Join Parameters Attribute Set (064699r12, section 6.2.2.2) ***/
  {mDefaultValueOfNwkScanAttempts_c}, /* x # of scan attempts */
  { (mDefaultValueOfNwkTimeBwnScans_c & 0xff),
    (mDefaultValueOfNwkTimeBwnScans_c >> 8)
  },                                  /* x time between scans(ms) */
  {(mDefaultValueOfRejoinInterval_c&0xff),
   (mDefaultValueOfRejoinInterval_c>>8)
  },                                  /* x rejoin interval (sec) */
  {(mDefaultValueOfMaxRejoinInterval_c & 0xff),
   (mDefaultValueOfMaxRejoinInterval_c >> 8)
  },                                  /* x maxRejoinInterval (sec) */

  /*** End-Device Parameters Attribute Set (064699r12, section 6.2.2.3) ***/
  {(mDefaultValueOfIndirectPollRate_c & 0xff),
   (mDefaultValueOfIndirectPollRate_c >> 8)
  },                                  /* x indirectPollRate(ms) */
  {gMaxNwkLinkRetryThreshold_c},      /* x parentRetryThreshold */

  /*** Concentrator Parameters Attribute Set (064699r12, section 6.2.2.4) ***/
  {gConcentratorFlag_d},              /* x concentratorFlag */
  {gConcentratorRadius_c},            /* x concentratorRadius */
  {gConcentratorDiscoveryTime_c},     /* x concentratorDiscoveryTime */
};

#ifdef __IAR_SYSTEMS_ICC__
const BeeAppBlackBoxConfig_t BlackBoxConfig =
{
    BlackBoxDefaultBaudrate_d,
  
  #if gI2CInterface_d
    BlackBoxDefaultI2CSlaveAddr_d,
  #else
    0x00,
  #endif
  
  #if gSPIInterface_d
    BlackBoxDefaultSPIBaudrate
  #else
    0x00
  #endif
};
#pragma required=BlackBoxConfig
#else
volatile const BeeAppBlackBoxConfig_t BlackBoxConfig =
{
  BlackBoxDefaultBaudrate_d,
  
  #if gI2CInterface_d
    BlackBoxDefaultI2CSlaveAddr_d,
  #else
    0x00,
  #endif
  
  #if gSPIInterface_d
    BlackBoxDefaultSPIBaudrate
  #else
    0x00
  #endif
};
#endif

/******************************************************************************
*******************************************************************************
* Public Functions
*******************************************************************************
******************************************************************************/

#if (gMC1323xMatrixKBD_d == 1)
void BeeAppHandleKeysMC1323xMatrixKBD
( 
  uint8_t events, 
  uint8_t pressedKey 
)
{
  if (events == gKBD_EventLong_c)
    pressedKey +=  gKBD_EventLongSW1_c - gKBD_EventSW1_c;
  BeeAppHandleKeys(pressedKey);
}
#endif

/*****************************************************************************
* BeeAppInit
*
* Initializes the application
*
* Initialization function for the App Task. This is called during
* initialization and should contain any application specific initialization
* (ie. hardware initialization/setup, table initialization, power up
* notification.
*****************************************************************************/
#ifdef __IAR_SYSTEMS_ICC__
#pragma optimize=low
#endif
void BeeAppInit
  (
  void
  )
{
#if gNum_EndPoints_c != 0
  index_t i;
#endif  

  /* initialize LED driver */
  LED_Init();

  /* register to get keyboard input */
#if (gMC1323xMatrixKBD_d == 1)
  KBD_Init(BeeAppHandleKeysMC1323xMatrixKBD);
#else  
  KBD_Init(BeeAppHandleKeys);
#endif  

  /* initialize LCD (NCB only) */
  LCD_Init(); 

  /* Set baudrate */
#if (gUart1_Enabled_d == TRUE) || (gUart2_Enabled_d == TRUE)
  UartX_SetBaud(BlackBoxConfig.BaudRate);
#endif 

  /* Set I2C slave address */
#if gI2CInterface_d   
  (void)IIC_SetSlaveAddress(BlackBoxConfig.I2CSlaveId);
#endif  
  
  /* register to get BeeApp responses */
  ZTC_RegisterAppInterfaceToTestClient(&BlackBoxReceiveZtcMessage);

  /* flash LED1 to indicate not on network */
  LED_TurnOffAllLeds();
  LED_SetLed(LED1, gLedFlashing_c);

  /* register the application endpoint(s), so we receive callbacks */
#if gNum_EndPoints_c != 0  
  for(i=0; i<gNum_EndPoints_c; ++i) {
    (void)AF_RegisterEndPoint(endPointList[i].pEndpointDesc);
  }

#endif  

ASL_SetCommissioningSAS(&gBlackBoxAttrsData);
Zdp_AppRegisterCallBack(BeeAppZdpCallBack);
#if gFrequencyAgilityCapability_d && (!gEndDevCapability_d && !gComboDeviceCapability_d)
  /*
    The combo device should init the FA State Machine when it joins or forms the
    NWK when it gets to the Running State not in here.
  */
  FA_StateMachineInit();
#endif  

}

/*****************************************************************************
  BeeAppTask

  Application task. Responds to events for the application.
*****************************************************************************/
void BeeAppTask
  (
  event_t events    /*IN: events for the application task */
  )
{
  /*
    events bits 12-15 are defined in BeeApp.h, and are generic to all apps
  */

  /* received one or more data confirms */
  if(events & gAppEvtDataConfirm_c)
    BeeAppDataConfirm();

  /* received one or more data indications */
  if(events & gAppEvtDataIndication_c)
    BeeAppDataIndication();

  if(events & gAppEvtSyncReq_c)
    ASL_Nlme_Sync_req(FALSE);  

#if gInterPanCommunicationEnabled_c
    /* received one or more data confirms */
  if(events & gInterPanAppEvtDataConfirm_c)
    BeeAppInterPanDataConfirm();

  /* received one or more data indications */
  if(events & gInterPanAppEvtDataIndication_c)
    BeeAppInterPanDataIndication();
#endif 

  /* 
    app specific events (bits 0-11) go here...
  */

}

/*****************************************************************************
  BeeAppHandleKeys

  Handles all key events for this device.

*****************************************************************************/
void BeeAppHandleKeys
  (
  key_event_t keyEvent  /*IN: Events from keyboard modul */
  )
{
 
  (void) keyEvent;
  
}

/****************************************************************************
* BeeAppDataIndication
*
* Process incoming ZigBee over-the-air messages.
*****************************************************************************/
void BeeAppDataIndication
  (
  void
  )
{
  apsdeToAfMessage_t *pMsg;
  zbApsdeDataIndication_t *pIndication;

  while(MSG_Pending(&gAppDataIndicationQueue))
  {
    /* Get a message from a queue */
    pMsg = MSG_DeQueue( &gAppDataIndicationQueue );

    /* ask ZCL to handle the frame */
    pIndication = &(pMsg->msgData.dataIndication);
    (void) pIndication;
    /*
      Note: if multiple endpoints are supported by this app, insert 
      endpoint filtering here...

      This app assumes only 1 active endpoint. APS layer has already
      filtered by endpoint and profile.

      Note: all multi-byte over-the-air fields are little endian.
      That is 0x1101 would come in byte order 0x01 0x11.
    */
    
        

    /* Free memory allocated by data indication */
    AF_FreeDataIndicationMsg(pMsg);
  }
}

/*****************************************************************************
  BeeAppDataConfirm

  Process incoming ZigBee over-the-air data confirms.
*****************************************************************************/
void BeeAppDataConfirm
  (
  void
  )
{
  apsdeToAfMessage_t *pMsg;
  zbApsdeDataConfirm_t *pConfirm;

  while(MSG_Pending(&gAppDataConfirmQueue))
  {
    /* Get a message from a queue */
    pMsg = MSG_DeQueue( &gAppDataConfirmQueue );
    pConfirm = &(pMsg->msgData.dataConfirm);

    /* Action taken when confirmation is received. */
    if( pConfirm->status == gSuccess_c )
    {
      /* successful confirm */
    }
    
    /* Free memory allocated in Call Back function */
    MSG_Free(pMsg);
  }
}

#if gInterPanCommunicationEnabled_c

/*****************************************************************************
  BeeAppInterPanDataIndication

  Process InterPan incoming ZigBee over-the-air messages.
*****************************************************************************/
void BeeAppInterPanDataIndication(void)
{
  InterPanMessage_t *pMsg;
  zbInterPanDataIndication_t *pIndication;
  zbStatus_t status = gZclMfgSpecific_c;
  (void) status;
  
  while(MSG_Pending(&gInterPanAppDataIndicationQueue))
  {
    /* Get a message from a queue */
    pMsg = MSG_DeQueue( &gInterPanAppDataIndicationQueue );

    /* ask ZCL to handle the frame */
    pIndication = &(pMsg->msgData.InterPandataIndication );
   (void) pIndication;    
    /* Handle the Indication here */
    /* Free memory allocated by data indication */
    MSG_Free(pMsg);
  }
}


/*****************************************************************************
  BeeAppDataConfirm

  Process InterPan incoming ZigBee over-the-air data confirms.
*****************************************************************************/
void BeeAppInterPanDataConfirm
(
void
)
{
  InterPanMessage_t *pMsg;
  zbInterPanDataConfirm_t *pConfirm;
  
  while(MSG_Pending(&gInterPanAppDataConfirmQueue))
  {
    /* Get a message from a queue */
    pMsg = MSG_DeQueue( &gInterPanAppDataConfirmQueue );
    pConfirm = &(pMsg->msgData.InterPandataConf);
    
    /* Action taken when confirmation is received. */
    if( pConfirm->status != gZbSuccess_c )
    {
      /* The data wasn't delivered -- Handle error code here */
    }
    
    /* Free memory allocated in Call Back function */
    MSG_Free(pMsg);
  }
}
#endif 

/******************************************************************************
*******************************************************************************
* Private Functions
*******************************************************************************
*******************************************************************************/

/****************************************************************************
* BlackBoxReceiveZtcMessage
*
* Receives a message from ZTC (Test Tool). Used to control the BlackBox
*	application through the serial port.
*****************************************************************************/
void BlackBoxReceiveZtcMessage(ZTCMessage_t* pMsg) 
{
  zbCommissioningAttributes_t *pSAS;
  
  
  /* Check for invalid opcodes and parameters */
  if ((gBlackBoxGroup_c != pMsg->opCode) ||
      ((gBlackBoxReadSAS_c == pMsg->opCodeId) && (pMsg->data[0] > gROM_c))) 
  {    
    return;
  }

  switch(pMsg->opCodeId)
  {
    /* Overwrite SAS structure */
    case gBlackBoxWriteSAS_c:
    {
      uint8_t status = gZbSuccess_c;
      FLib_MemCpy(&gBlackBoxAttrsData, pMsg->data, sizeof(zbCommissioningAttributes_t));
      ZTCQueue_QueueToTestClient(&status, pMsg->opCode,
                                 gBlackBoxWriteSASCnf_c, sizeof(status));
    }
      break;
    
    /* Return SAS to Test Client */
    case gBlackBoxReadSAS_c:
      if (gRAM_c == pMsg->data[0])
        pSAS = &gBlackBoxAttrsData;
      else if (gROM_c == pMsg->data[0])
        pSAS = (zbCommissioningAttributes_t*)&gSAS_Rom;

      ZTCQueue_QueueToTestClient((uint8_t *)(pSAS), pMsg->opCode,
                                 gBlackBoxReadSASCnf_c, sizeof(zbCommissioningAttributes_t));
      break;
  }           
  
}



/****************************************************************************
* BeeAppZdpCallBack
*
* ZDP calls this function when it receives a response to a request.
* For example, ASL_MatchDescriptor_req will return whether it worked or not.
*****************************************************************************/
void BeeAppZdpCallBack
  (
  zdpToAppMessage_t *pMsg, 
  zbCounter_t counter
  )
{
  uint8_t event;

  (void)counter;

  /* get the event from ZDP */
  event = pMsg->msgType;
  if(event == gzdo2AppEventInd_c) /* many possible application events */
    event = pMsg->msgData.zdo2AppEvent;

  /* got a response to match descriptor */
  switch(event) {

    case gEnd_Device_Bind_rsp_c:
    
      break;
    
    case gMatch_Desc_rsp_c:
      break;

    /* network has been started */
    case gZDOToAppMgmtZCRunning_c:
    case gZDOToAppMgmtZRRunning_c:
    case gZDOToAppMgmtZEDRunning_c:
      if (appState == mStateIdle_c) {

        appState = mStateZDO_device_running_c;
        
      }
      break;

    case gNlmeTxReport_c:
      FA_ProcessNlmeTxReport(&pMsg->msgData.nlmeNwkTxReport);
      break;

    case gMgmt_NWK_Update_notify_c:
      /* Already handle in ZDP. */
      break;

    case gNlmeEnergyScanConfirm_c:
      FA_ProcessEnergyScanCnf(&pMsg->msgData.energyScanConf);
      break;

    case gChannelMasterReport_c:
      FA_SelectChannelAndChange();
      break;
  }
  if (pMsg->msgType == gNlmeEnergyScanConfirm_c)
    MSG_Free(pMsg->msgData.energyScanConf.resList.pEnergyDetectList);

  /* free the message from ZDP */
  MSG_Free(pMsg);
}



/*****************************************************************************
* BeeAppUpdateDevice
*
* Contains application specific
*
*
*****************************************************************************/
void BeeAppUpdateDevice
  (
  zbEndPoint_t endPoint,    /* IN: endpoint update happend on */
  zclUIEvent_t event,        /* IN: state to update */
  zclAttrId_t attrId, 
  zbClusterId_t clusterId, 
  void *pData
  )
{
  (void) endPoint;
  (void) event;
  (void) attrId;
  (void) clusterId;
  (void) pData;

}
