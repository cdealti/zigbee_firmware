/*****************************************************************************
* This is Main entry file for ZDO.
*
* (c) Copyright 2008, Freescale, Inc. All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
******************************************************************************/

#include "PublicConst.h"
#include "ZdoApsInterface.h"
#include "BeeStackUtil.h"
#include "ZdoNwkManager.h"
#include "NwkCommon.h"
#include "NVM_interface.h"
#include "BeeStackConfiguration.h"
#include "ZdoMain.h"
#include "ZdoCommon.h"
#include "ZdoBindManager.h"
#if !gArm7ExtendedNVM_d
#include "NV_Data.h"
#endif
#include "BeeStackParameters.h"
#include "FunctionLib.h"

#if gStandardSecurity_d || gHighSecurity_d
#include "ZdoSecurityManager.h"
#include "ApsMgmtInterface.h"
#endif

#include "ZtcInterface.h"

#include "ZDOStateMachineHandler.h"
#include "ASL_ZdpInterface.h"
#include "PWR_Interface.h"


/******************************************************************************
*******************************************************************************
* Private Macros
*******************************************************************************
******************************************************************************/

/*None*/
/******************************************************************************
*******************************************************************************
* Private Prototypes
*******************************************************************************
******************************************************************************/
#if gStandardSecurity_d || gHighSecurity_d
/*
  Global and private prototypes for security goes here.
*/
#if ( gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d )
extern index_t PacketsOnHoldToThisAddress(zbNwkAddr_t aDestinationAddress);
#endif
#if gApsLinkKeySecurity_d
extern bool_t SecurityTypeCheck( zbIeeeAddr_t aIeeeAddress );
#endif


#if gTrustCenter_d || gComboDeviceCapability_d
/*
  Global and private memory declarations for security, exclusive for the
  Trust Center goes here.
*/
void ZDO_TrustCenterStateMachine
(
  event_t events
);

void ZDO_SendAuthenticationToTrustCenter
(
  bool_t allow
);


extern neighborTable_t* IsMyChild ( zbAddrMode_t addressMode, uint8_t *pAddress );
extern void ResetNeighborTableEntry ( neighborTable_t *pNeighborTableEntry );
#else
#define ZDO_TrustCenterStateMachine(events)
#define ZDO_SecGetDeviceKeyType(aDeviceAddress)  gStandardNetworkKey_c
#endif

#endif

/******************************************************************************
*******************************************************************************
* Private type definitions
*******************************************************************************
******************************************************************************/

/* None */

/******************************************************************************
*******************************************************************************
* Private Memory Declarations
*******************************************************************************
******************************************************************************/

anchor_t mNlmeZdoInputQueue;
anchor_t mAppZdpInputQueue;


#if gStandardSecurity_d || gHighSecurity_d
/*
  Global and private memory declarations for security goes here.
*/

#if gTrustCenter_d || gComboDeviceCapability_d
/*
  Global and private memory declarations for security, exclusive for the
  Trust Center goes here.
*/

/*
  The State holder for the Trust center state machine.
*/
static uint8_t mTrustCenterState;

/*
  This two pointers are use to handle the queues fro the trust center state machine.
*/
static nlmeZdoMessage_t          *pJoinMsg = NULL;
static zbApsmeUpdateDeviceInd_t  *pUpdateMsg = NULL;

#endif
#endif

#if gStandardSecurity_d || gHighSecurity_d
msgQueue_t mApsmeZdoInputQueue;
#endif

/******************************************************************************
*******************************************************************************
* Public Memory
*******************************************************************************
******************************************************************************/

/* counter for stack sleep... app must manage its own. Makes it so reset allows sleep properly */
zbCounter_t gZdoAllowToSleepCounter=0;


/* Various ZDO timers. See ZdoCommon.h for definitions. */
tmrTimerID_t gZdoTimerIDs[8];

#if gStandardSecurity_d || gHighSecurity_d
/*
  Global and public memory declrations for security goes here.
*/

#if gTrustCenter_d || gComboDeviceCapability_d
/*
  Global and public memory declarations for security, exclusive for the
  Trust Center goes here.
*/
msgQueue_t gJoinIndicationQueue;
msgQueue_t gUpdateDeviceQueue;
#endif
#endif


/******************************************************************************
*******************************************************************************
* Public Functions
*******************************************************************************
*******************************************************************************/

/************************************************************************************
  Called once upon init. Initializes the task.
************************************************************************************/
void TS_ZdoTaskInit( void )
{
  uint8_t i;

  /* allocate the ZDO timers */
  for(i=0; i<NumberOfElements(gZdoTimerIDs); ++i)
  {
    gZdoTimerIDs[i] = TMR_AllocateTimer();
  }

  // setup/clear all the queues
  ZdoReset();
}

void ZDO_TimerStop(void)
{
  uint8_t i;

  /* allocate the ZDO timers */
  for(i=0; i<NumberOfElements(gZdoTimerIDs); ++i)
  {
    TMR_StopTimer(gZdoTimerIDs[i]);
  }
}

/************************************************************************************
  Reset may be called at any time (not just at startup).
  Clears the state back to off-the network and cleans up all the queues.
************************************************************************************/
void ZdoReset( void )
{
  /* clear all the events in ZdoTask */
  TS_ClearEvent(gZdoTaskID_c, 0xFFFF );

  /* Free pending messages from zdo queue */
  List_ClearAnchor(&mNlmeZdoInputQueue);  /* NLME_ZDO_SapHandler */
  List_ClearAnchor(&mAppZdpInputQueue);   /* APP_ZDP_SapHandler */

  /* reset */
  while(gZdoAllowToSleepCounter) {
    ZDO_AllowDeviceToSleep();
  }

#if gStandardSecurity_d || gHighSecurity_d
  List_ClearAnchor(&mApsmeZdoInputQueue);
#if gTrustCenter_d || gComboDeviceCapability_d
  List_ClearAnchor(&gJoinIndicationQueue);
  List_ClearAnchor(&gUpdateDeviceQueue);
#endif
#endif

  ZDO_TimerStop();
}

/************************************************************************************
* Handles various ZDO state machines (Binding, Trust Center, etc), and the
* ZDO/NLME SAP and APP/ZDP SAP.
************************************************************************************/
void TS_ZdoTask
(
  event_t events  /* IN: The bit mask for the supported events in this task. */
)
{
  /*
    A Message coming form the Nwk layer to ZDO.
  */
  nlmeZdoMessage_t *pZdoMsg;

  /*
    A message coming the application to the ZDO.
  */
  appToZdpMessage_t *pAppMsg;

  /*
    Any secure command from the ASP or the SSP module.
  */
  zbApsmeZdoIndication_t *pApsmeMsg;

#if (gHighSecurity_d && ( gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d))
  /*
    Entity Authentication Request Message, send by the Nwk Layer.
  */
  zbEAReqData_t *pNwkEAReqMsg;

  /*
    Index used to get buffered messages waiting for the EA process to be done.
  */
  uintn8_t                 iIndex;
#endif

#if ( gCoordinatorCapability_d || gComboDeviceCapability_d )&& gBindCapability_d && gEnd_Device_Bind_rsp_d
#if gComboDeviceCapability_d
    if( NlmeGetRequest(gDevType_c) == gCoordinator_c )
#endif
    {
      /*
        Process the EndDevice Bind time out on the binding state machine (APS module).
      */
      if( events & gEndDevBindTimeOut_c )
      {
        ZdpEndDeviceBindStateMachine();
      }
    }
#endif

  /*
    Any Message from the Nwk Layer (NLME_ZDO_SapHandler).
  */
  if( events & gNLME_ZDO_c )
  {
    /*
      Just catch the incoming message to be process.
    */
    pZdoMsg = MSG_DeQueue( &mNlmeZdoInputQueue );

    /*
      Verify that theincoming message is not an error sent from the network layer.
    */
    if(pZdoMsg->msgType != gNwkError_c)
    {
      /*
        Process the commands/message on a different module.
      */
      ZDO_NwkManager( pZdoMsg );

      /*
        Each layer has the responsability to free the memory, in this case any Nwk
        command from the network layer. But if ZDO or the application need it for
        a while, do not free it yet.
      */
      if (!gMemoryFreedByApplication)
        MSG_Free(pZdoMsg);
    }
  }

  /*
    Any message from the application (APP_ZDP_SapHandler).
  */
  if( events & gAPP_ZDP_c )
  {

    /*
      Catch the incoming message, it should be on the queue.
    */
    pAppMsg = MSG_DeQueue( &mAppZdpInputQueue );
    if( pAppMsg ){

      switch(pAppMsg->msgType)
      {
        case gZdoNwkSyncReq_c:
          /*
            Use the same buffer to generate the Nlme message.
          */
          pAppMsg->msgType = gNlmeSyncRequest_c;

          /*
            Shift the paylod to the left, this is to eliminate the DstAddress field
            from the message, because the Nwk layer dont has it.
          */
          FLib_MemInPlaceCpy(pAppMsg->aDestAddr,
                             &pAppMsg->msgData.nlmeSyncReq,
                             sizeof(pAppMsg->msgData.nlmeSyncReq));

          /*
            Send the message to network layer.
          */
          ( void ) ZDO_NLME_SapHandler((zdoNlmeMessage_t *)pAppMsg);
          break;

        /*
          When the application needs to the permit join On/Off
        */
        case gPermitJoinReq_c:
          /*
            Use the same buffer to generate the Nlme message.
          */
          pAppMsg->msgType = gNlmePermitJoiningRequest_c;

          /*
            Shift the paylod to the left, this is to eliminate the DstAddress field
            from the message, because the Nwk layer dont has it.
          */
          FLib_MemInPlaceCpy(pAppMsg->aDestAddr,
                             &pAppMsg->msgData.permitJoinReq,
                             sizeof(pAppMsg->msgData.permitJoinReq));

          /*
            Send the message to network layer.
          */
          ( void ) ZDO_NLME_SapHandler((zdoNlmeMessage_t *)pAppMsg);
          break;

        /*
          If the command Id does not get processed before send it to ZDP.
        */
        default:
            /*
              The default is to send the message from the app to the ZDP module.
              The request can be to our self or to some one else in the network,
              For ZDP there is no difference.
            */
            if (ZDO_IsRunningState())
            {
              /* Process the command request */
              Zdp_RequestGenerator(pAppMsg->msgType, &pAppMsg->msgData,pAppMsg->aDestAddr);
            }
            else
            {
              /* Let the application know about the current state of ZDO */
              Zdo_GenerateEventZdo2App(gZDOToAppMgmtOffTheNetwork_c);
            }

            /*
              Each layer has the responsability to free the memory, in this case any ZDP
              command from the application.
            */
            MSG_Free(pAppMsg);
            break;
      }
    }
  }

  if( events & gAPSME_ZDO_c )
  {
#if gStandardSecurity_d || gHighSecurity_d
    if (!ZDO_IsReadyForPackets())
    {
      TS_SendEvent(gZdoTaskID_c, gAPSME_ZDO_c);
      return;
    }

    pApsmeMsg = MSG_DeQueue( &mApsmeZdoInputQueue );
    /*
      If we needed to use the returning error code from the function to handler
      the error code, can be done here.
    */
    if (ZDO_SecProcessApsmeIndication((void *)pApsmeMsg))
    {
      /*
        Catch Error Here.!
      */
    }
#else
    pApsmeMsg = NULL;
#endif

    /*
      Each layer has the responsability to free the memory, in this case any APS
      command from the APS layer.
    */
    if (pApsmeMsg)
      MSG_Free(pApsmeMsg);
  }

  if (events & gNWK_ZDO_EA_Start_c)
  {
#if (gHighSecurity_d && ( gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d))
#if gComboDeviceCapability_d
    if( NlmeGetRequest(gDevType_c) != gEndDevice_c )
#endif
    {
      /*
        Get the message from the queue.
      */
      pNwkEAReqMsg = MSG_DeQueue( &gNwkEaReuDataQueue );
      /*
        Sent the Authenticate request to the Zdo so that it can start the EA process.
      */
      if (ZDO_APSME_Authenticate_request(pNwkEAReqMsg->aSrcIEEEAddress,gINITIATE_c, NULL))
      {
        /*
          In case that the Entity Authentication process failed, check if there 
          message pending to be send to the Device and free it.
        */
        iIndex = PacketsOnHoldToThisAddress(pNwkEAReqMsg->aSourceAddress);

        /*
          If a data is buffered, free it since the EA process failed.
        */
        if( iIndex != gNwkEntryNotFound_c )
        {
           gaPacketsOnHoldTable[iIndex].status = mPohtFreeEntry_c;
           MSG_Free((uint8_t *)gaPacketsOnHoldTable[iIndex].pPacketOnHold-1);
        }
      }

      /*
        Each layer has the responsability to free the memory, in this case any APS
        command from the APS layer.
      */
      if (pNwkEAReqMsg)
      {
        MSG_Free(pNwkEAReqMsg);
      }
    }
#endif
  }

#if gStandardSecurity_d || gHighSecurity_d
  /*
    In any case lets visit the state machine.!
  */
  ZDO_TrustCenterStateMachine(events);
#endif

/*If any message pending post event again to run again*/
#if gStandardSecurity_d || gHighSecurity_d
    if(MSG_Pending(&mApsmeZdoInputQueue)){
      TS_SendEvent(gZdoTaskID_c, gAPSME_ZDO_c);
    }
#endif

  if(MSG_Pending(&mAppZdpInputQueue)){
    TS_SendEvent(gZdoTaskID_c, gAPP_ZDP_c);
  }

  if(MSG_Pending(&mNlmeZdoInputQueue)){
    TS_SendEvent(gZdoTaskID_c, gNLME_ZDO_c);
  }
}

/*****************************************************************************/
#if gSystemEventEnabled_d
void Zdo_GenerateEventZdo2App( uint8_t event )
{
	zdpToAppMessage_t *pMsg;

	pMsg = MSG_Alloc( sizeof( zbMsgId_t) + sizeof(zdo2AppEvent_t) );
	if(pMsg == NULL)
		return;

	pMsg->msgType = gzdo2AppEventInd_c;
	pMsg->msgData.zdo2AppEvent =  event;
	(void)ZDP_APP_SapHandler(pMsg);
}
#endif

/*****************************************************************************/
uint8_t NLME_ZDO_SapHandler
  (
  nlmeZdoMessage_t *pMsg /* IN - pointer to msg sent from Nwk to ZDO*/
  )
{
  /* Put the incoming message in the queue */
  MSG_Queue(&mNlmeZdoInputQueue, pMsg);
  TS_SendEvent(gZdoTaskID_c,gNLME_ZDO_c);
  ZTC_TaskEventMonitor(gNLME_ZDO_SAPHandlerId_c, (uint8_t *) pMsg, gZbSuccess_c);
  return gZbSuccess_c;
}

/****************************************************************************/
uint8_t APP_ZDP_SapHandler
  (
  appToZdpMessage_t *pMsg /* IN - pointer to msg sent from APP to ZDP*/
  )
{
  if (pMsg) {
    /* Put the incoming message in the queue */
    MSG_Queue(&mAppZdpInputQueue, pMsg);
    TS_SendEvent(gZdoTaskID_c, gAPP_ZDP_c);
    ZTC_TaskEventMonitor(gAppZDP_SAPHandlerId_c, (uint8_t *) pMsg, gZbSuccess_c);
    return gZbSuccess_c;
  }

  return gInvalidParameter_c;
}

/****************************************************************************/
#if gStandardSecurity_d || gHighSecurity_d
/*
  APS is passing up a data indication or confirm to ZDO.
  Only used for security type functions.

  IN: pMsg
*/
uint8_t APSME_ZDO_SapHandler(zbApsmeZdoIndication_t *pMsg) {
  MSG_Queue(&mApsmeZdoInputQueue, pMsg);
  TS_SendEvent(gZdoTaskID_c, gAPSME_ZDO_c);
  ZTC_TaskEventMonitor(gAPSME_ZDO_SAPHandlerId_c, (uint8_t *) pMsg, gZbSuccess_c);
  return gZbSuccess_c;
}
#endif


/******************************************************************************
*******************************************************************************
* Private Functions
*******************************************************************************
******************************************************************************/

#if gStandardSecurity_d ||gHighSecurity_d
#if gTrustCenter_d || gComboDeviceCapability_d
/************************************************************************************
* Get the status of the security procedure, as well as the unsecure payload when the
* secure level requires it. If the NWK layer has a frame, consisting of a NwkHeader,
* nwkAuxiliary header and payload, which needs to be authenticated and/or unsecure
* the payload (nwkSecurityLevel > 0), this module willprocess the full Nwk Frame
* and return the frame for further processing.
*
* Interface assumptions:
*   The events parameter is a valid 16-bit mask.
*
* Return value:
*   NONE.
*
* Revison history:
*   Date   Author  Comments
*  ------  ------  --------
*  120208    MN    Created
************************************************************************************/
void ZDO_TrustCenterStateMachine
(
  event_t events  /* IN: The bit mask with the event bits that this state
                         machine supports. */
)
{
  nlmeJoinIndication_t  *pJoinIndication = NULL;
  zbKeyType_t  apsKey = gStandardNetworkKey_c;

  /*
    If the state machine is running, in which state is it?
  */
  switch (mTrustCenterState)
  {
    /*
      The state machine is not running, is ready to process either the join
      indication or the update device.
    */
    case gIdleState_c:
      /*
        The joining device is attached to the trust center, process it as a child.
      */
      if (events & gJoinIndicationArrive_c)
      {
        /*
          Get the information from the joining queue (IEEE Addr, Nwk Addr and etc).
        */
        pJoinMsg = MSG_DeQueue(&gJoinIndicationQueue);

        /*
          Dequeue fails for any reason, no more process to do.
        */
        if (!pJoinMsg)
          break;

        pJoinIndication = &pJoinMsg->msgData.joinIndication;

        /*
          Verify is is on a Exclusion list or what ever the applicaiton needs to do
          with it and wait for an answer.
        */
        AppAuthenticateDevice(pJoinIndication->aExtendedAddress);

        /*
          Put the state machine to wait until it knows what to do with the current
          indication.
        */
        mTrustCenterState = gWaitForProcessingState_c;
        break;
      }

      /*
        The joining device is attached to some one else on the netwrok but we need
        to authenticate it.
      */
      if (events & gUpdateDeviceIndication_c)
      {
        /*
          Get the infor from the queue so the state machine can process the update device.
        */
        pUpdateMsg = MSG_DeQueue(&gUpdateDeviceQueue);

        /*
          Dequeue fails for any reason, no more process to do.
        */
        if (!pUpdateMsg)
          break;

        if ((pUpdateMsg->status == gStandardDeviceUnsecuredJoin_c) || (pUpdateMsg->status == gHighSecurityDeviceUnsecuredJoin_c))
        {
          /*
            Verify is is on a Exclusion list or what ever the applicaiton needs to do
            with it and wait for an answer.
          */
          AppAuthenticateDevice(pUpdateMsg->aDeviceAddress);
        }
        else
        {
          /*
            Is not a joining Device it must be something else.
          */
          ZDO_SecProcessUpdateDeviceIndication(pUpdateMsg);
        }

        /*
          Put the state machine to wait until it knows what to do with the current
          indication.
        */
        mTrustCenterState = gWaitForProcessingState_c;
        break;
      }
      break;

    case gWaitForProcessingState_c:
      /*
        The state machine is waiting, for an authentication event.
      */

      /*
        If the devices was authenticated with success, then...
      */
      if (events & gDeviceAllow_c)
      {
        /*
          We are processing a Joining inidication (we can only process a joining or an
          Update device at the time, but not both).
        */
        if (pJoinMsg)
        {
          pJoinIndication = &pJoinMsg->msgData.joinIndication;

#if gHighSecurity_d
          /* The node is standard security and tries to join a High security, can't do. */
          if (!(pJoinIndication->capabilityInformation & gSecurityCapability_c))
          {
            mTrustCenterState = gDoneState_c;
            TS_SendEvent(gZdoTaskID_c, gDone_c);
            return;
          }
#endif

          /*
            The trust center needs to keep a list of the devices in the network,
            we use the Address Map for it. For Aps Link keys this function adds the
            Device into the security material.
          */
          ZDO_SecAddDeviceInfo(pJoinIndication->aExtendedAddress, pJoinIndication->aShortAddress);

          /* The device is join and authenticated, let it have the key. */
          apsKey = ZDO_SecGetDeviceKeyType(pJoinIndication->aExtendedAddress);

          /* Here is where and how to do the HA joining into a High Security network. */
#if gApsLinkKeySecurity_d
          if (SecurityTypeCheck(pJoinIndication->aExtendedAddress))
          {
            ZDO_SecSendTransportKey(pJoinIndication, TRUE);
          }
          else
#endif

          if (apsKey == gTrustCenterMasterKey_c)
          {
            /*
              Register the Skke state machine right here in roder to know if a transport
              key will be needed at the end of the process.
            */
#if gSKKESupported_d
            (void)SSP_ApsRegisterSKKEStateMachine(pJoinIndication->aExtendedAddress,
                                                  NlmeGetRequest(gNwkIeeeAddress_c),
                                                  NlmeGetRequest(gNwkIeeeAddress_c),
                                                  FALSE,
                                                  TRUE);
#endif
            (void)ZDO_APSME_Establish_Key_request(pJoinIndication->aExtendedAddress,
                                                  FALSE,
                                                  NlmeGetRequest(gNwkIeeeAddress_c),
                                                  gSKKE_Method_c);
          }
          else if (apsKey == 0xff)
          {
            /*
              We need to provide the Master key to the joining device.
            */
            ZDO_SecSendTCMasterKey(pJoinIndication, TRUE);

            /* Change the rejoin status to something invalid to avoid re-erasing the joining device. */
            pJoinIndication->rejoinNetwork = gReserved_c;

            /* Let the state machine handle the join indication on the next round. */
            TS_SendEvent(gZdoTaskID_c, gDeviceAllow_c);
          }
          /* The defualt case, we just sent the Trasnport key with the NWK key. */
          else if (apsKey != gNotInAddressMap_c)
          {
            if((pJoinIndication->rejoinNetwork == gAssociationJoin_c)
               || ((pJoinIndication->rejoinNetwork == gNwkRejoin_c) && (!(pJoinIndication->secureRejoin)) && (ApsmeGetRequest(gApsUseInsecureJoin_c))))
            {
              ZDO_SecSendTransportKey(pJoinIndication, TRUE);
            }
          }
        }

        /* The device was authenticated with succes and it is an update device request. */
        else if (pUpdateMsg)
        {
          /* For any rejoin type */
          if ((pUpdateMsg->status == gHighSecurityDeviceSecuredReJoin_c) ||
              (pUpdateMsg->status == gStandardDeviceSecuredReJoin_c) ||
              (pUpdateMsg->status == gHighSecurityDeviceUnsecuredRejoin_c) ||
              (pUpdateMsg->status == gStandardDeviceUnsecuredRejoin_c))
          {
            /*
              Got an update device if the device was mi child then update the relation ship,
              if it is  a Zed then remove it.
            */
            neighborTable_t *pNT = IsMyChild(gZbAddrMode64Bit_c, pUpdateMsg->aDeviceAddress);
            if (pNT != NULL)
            {
              pNT->deviceProperty.bits.relationship = gNeighborIsSibling_c;
              if (pNT->deviceProperty.bits.deviceType == gEndDevice_c)
                ResetNeighborTableEntry(pNT);
            }
          }

#if gHighSecurity_d
          /* The node is standard security and tries to join a High security, can't do. */
          if ((pUpdateMsg->status == gStandardDeviceUnsecuredJoin_c) ||
              (pUpdateMsg->status == gStandardDeviceUnsecuredRejoin_c))
          {
            mTrustCenterState = gDoneState_c;
            TS_SendEvent(gZdoTaskID_c, gDone_c);
            return;
          }
#endif

          /*
            If the device is trying to do an unsecure-rejoin, just send the trasnport key
            after being authenticated.
          */
          if ((pUpdateMsg->status == gHighSecurityDeviceUnsecuredRejoin_c) ||
              (pUpdateMsg->status == gStandardDeviceUnsecuredRejoin_c))
          {
            /* Clear secure data of the rejoining device. */
            /* Setting the last parameter to TRUE will erase APS secure material */
            ZDO_SecClearDeviceData(pUpdateMsg->aDeviceShortAddress, pUpdateMsg->aDeviceAddress, FALSE);

            /* Send the current and active transport key using the parent. */
            //if (ApsmeGetRequest(gApsUseInsecureJoin_c))
            {
              APS_ResetDeviceCounters(pUpdateMsg->aDeviceAddress);
              ZDO_SecSendTransportKey(pUpdateMsg, FALSE);
            }

            /* The TC state machine is no longer needed. */
            mTrustCenterState = gDoneState_c;
            TS_SendEvent(gZdoTaskID_c, gDone_c);
            return;
          }

          /* if the device is doing association start from scratch. */
          if ((pUpdateMsg->status == gHighSecurityDeviceUnsecuredJoin_c) || (pUpdateMsg->status == gStandardDeviceUnsecuredJoin_c))
          {
            /* Clear secure data of the associating device. */
            if (ApsmeGetRequest(gApsDefaultTCKeyType_c) == gTrustCenterLinkKey_c)
            {
              APS_ResetDeviceCounters(pUpdateMsg->aDeviceAddress);
            }
            else
            {
              /* Remove it completely */
#if gApsLinkKeySecurity_d
              APS_RemoveSecurityMaterialEntry(pUpdateMsg->aDeviceAddress);
#endif
            }
            /* Setting the last parameter to TRUE will erase APS secure material */
            ZDO_SecClearDeviceData(pUpdateMsg->aDeviceShortAddress, pUpdateMsg->aDeviceAddress, FALSE);
          }

          /* If the rejoin is secure dont send the Transport key. */
          if ((pUpdateMsg->status == gHighSecurityDeviceSecuredReJoin_c) ||
              (pUpdateMsg->status == gStandardDeviceSecuredReJoin_c))
          {
            /* The TC state machine is no longer needed. */
            mTrustCenterState = gDoneState_c;
            TS_SendEvent(gZdoTaskID_c, gDone_c);
            return;
          }

          /*
            The trust center needs to keep a list of the devices in the network, we use
            the Address Map for it. For Aps Link keys this function adds the Device into
            the security material.
          */
          ZDO_SecAddDeviceInfo(pUpdateMsg->aDeviceAddress, pUpdateMsg->aDeviceShortAddress);

          /* The device is join and authenticated, let it have the key. */
          apsKey = ZDO_SecGetDeviceKeyType(pUpdateMsg->aDeviceAddress);

          /* Here is where and how to do the HA joining to a High Security network. */
#if gApsLinkKeySecurity_d
          if (SecurityTypeCheck(pUpdateMsg->aDeviceAddress))
          {
            ZDO_SecSendTransportKey(pUpdateMsg, FALSE);
          }
          else
#endif
          /* Do we have a Master key? */
          if (apsKey == gTrustCenterMasterKey_c)
          {
            /*
              Register the Skke state machine right here in roder to know if a transport
              key will be needed at the end of the process.
            */
#if gSKKESupported_d
            (void)SSP_ApsRegisterSKKEStateMachine(pUpdateMsg->aDeviceAddress,
                                                  NlmeGetRequest(gNwkIeeeAddress_c),
                                                  pUpdateMsg->aSrcAddress,
                                                  TRUE,
                                                  TRUE);
#endif
            (void)ZDO_APSME_Establish_Key_request(pUpdateMsg->aDeviceAddress,
                                                  TRUE,
                                                  pUpdateMsg->aSrcAddress,
                                                  gSKKE_Method_c);
          }

          /* We have nothing? */
          else if (apsKey == 0xff)
          {
            /* We need to provide the Master key to the joining device. */
            ZDO_SecSendTCMasterKey(pUpdateMsg, FALSE);

            /* Change the status to avoid re-erasing the node in process */
            pUpdateMsg->status = gReserved_c;

            /* Let the state machine handle the join indication on the next round. */
            TS_SendEvent(gZdoTaskID_c, gDeviceAllow_c);
          }
          /* The defualt case, we just sent the Trasnport key with the NWK key. */
          else if (apsKey != gNotInAddressMap_c)
          {
            if((pUpdateMsg->status == gStandardDeviceUnsecuredJoin_c)
               || ((pUpdateMsg->status == gStandardDeviceUnsecuredRejoin_c) && (ApsmeGetRequest(gApsUseInsecureJoin_c))))
            {
              ZDO_SecSendTransportKey(pUpdateMsg, FALSE);
            }
          }
        }

        /* If the authentication fails or not, we are done with this packet */
        if (apsKey != 0xff)
        {
          /* Here is where we command that all buffers used get freed. */
          mTrustCenterState = gDoneState_c;
          TS_SendEvent(gZdoTaskID_c, gDone_c);
        }
      }

      if (events & gDeviceForbiden_c)
      {
        /* Send the Remove device to the parent. */

        /*
          If the authentication fails or not, we are done with this packet.
        */
        mTrustCenterState = gDoneState_c;
        TS_SendEvent(gZdoTaskID_c, gDone_c);
      }

      if (events & gDone_c)
      {
        /*
          If the authentication fails or not, we are done with this packet.
        */
        mTrustCenterState = gDoneState_c;
        TS_SendEvent(gZdoTaskID_c, gDone_c);
      }
      break;

    case gDoneState_c:
      /*
        If the incoming message was a Join Indication, free it.
      */
      if (pJoinMsg)
      {
         /*
          Do not free it here let it go the Application.
          After the trust center truly process the join indication and the trasnport key
          has been sent then inform the application of this join.
        */
        (void)ZDP_APP_SapHandler((void *)pJoinMsg);
        pJoinIndication = NULL;
        pJoinMsg = NULL;
      }

      /*
        If the incoming message was a Update Device Indication, free it.
      */
      if (pUpdateMsg)
      {
        /*
          The update device can be freed here, the application knows abou this through
          ZDOSecurityManager.
        */
        MSG_Free(pUpdateMsg);
        pUpdateMsg = NULL;
      }

      /*
        Let the State machine be ready to porcess the next event.
      */
      mTrustCenterState = gIdleState_c;

      /*
        If the trust center has more join indications to process, send the event,
        and run the state machine again.
      */
      if (MSG_Pending(&gJoinIndicationQueue))
        TS_SendEvent(gZdoTaskID_c, gJoinIndicationArrive_c);

      /*
        If the trust center has more update device indications to process, send the event,
        and run the state machine again.
      */
      if (MSG_Pending(&gUpdateDeviceQueue))
        TS_SendEvent(gZdoTaskID_c, gUpdateDeviceIndication_c);

      break;
  }
}

void ZDO_SendAuthenticationToTrustCenter
(
  bool_t allow
)
{
  if (allow)
  {
    TS_SendEvent(gZdoTaskID_c, gDeviceAllow_c);
  }
  else
  {
    TS_SendEvent(gZdoTaskID_c, gDeviceForbiden_c);
  }
}

/************************************************************************************
* Register a device Long address and Short address into the address map, and for the
* Link key security, also add it to the Device key Set.
*
* The function only works for the trust center.
*
* Interface assumptions:
*   The parameter aExtAddr is a valid 64-bit address.
*   The parameter aNwkAddr is a valid 16-bit address.
*
* Return value:
*   NONE.
*
* Revison history:
*   Date   Author  Comments
*  ------  ------  --------
*  210408    MN    Created
************************************************************************************/
void ZDO_SecAddDeviceInfo
(
  zbIeeeAddr_t  aExtAddr,  /* IN: The long address of the device to register. */
  zbNwkAddr_t   aNwkAddr   /* IN: The short address of the device to register. */
)
{
#if gApsLinkKeySecurity_d
  uint8_t index;
  uint8_t i;
  uint8_t free = gNotInAddressMap_c;
  uint8_t arraySize;
  zbApsDeviceKeyPairSet_t  devKeyPairSetEntry;

  /*
    First of all we need its information on the address map.
  */
  index = APS_AddToAddressMapPermanent(aExtAddr, aNwkAddr);

  /*
    If the trust center is unable to store the device into the Address map there is
    nothing to do.!
  */
  if (index == gAddressMapFull_c)
  {
    return;
  }

  /*
    Get a pointer to the current APS SEcurity materila set, and calculate the amount of
    entries that it has.
  */
  arraySize = ApsmeGetRequest(gApsDeviceKeyPairCount_c);
  free = gNotInAddressMap_c;
  for (i = 0;i < arraySize; i++)
  {
    (void)DevKeyPairSet_GetTableEntry(i, &devKeyPairSetEntry);
    if (devKeyPairSetEntry.iDeviceAddress == gNotInAddressMap_c)
    {
      /*
        if we found a free entry and we haven't marked as free do it and keep going.!
      */
      if (free == gNotInAddressMap_c)
      {
        /*
          Catch the free entry, cadidate to be hold teh device info.
        */
        free = i;
      }
      /*
        Next loop iteration please.!
      */
      continue;
    }

    /*
      If the security material is alrady register leave the function.
    */
    if (devKeyPairSetEntry.iDeviceAddress == index)
    {
      return;
    }
  }

  /*
    The security material set is full, we will register the deviec into the address map,
    but not in the security material set.
  */
  if (free == gNotInAddressMap_c)
  {
    return;
  }

  /*
    The security material set only know where i n the address map is the device.
  */
  (void)DevKeyPairSet_GetTableEntry(free, &devKeyPairSetEntry);
  devKeyPairSetEntry.iDeviceAddress = index;
  (void)DevKeyPairSet_SetTableEntry(free, &devKeyPairSetEntry);
  /* Save to NVM eache device we have added. */
  ZdoNwkMng_SaveToNvm(zdoNvmObject_ApsLinkKeySet_c);

/* #else gApsLinkKeySecurity_d */
#else

  /*
    For other security model that does not requires Link keys add the node into the
    address map only.
  */
  (void)APS_AddToAddressMapPermanent(aExtAddr, aNwkAddr);
/* #endif gApsLinkKeySecurity_d */
#endif
}

/* #endif gTrustCenter_d */
#endif

/* #endif gStandardSecurity_d || gHighSecurity_d */
#endif

/************************************************************************************
* Generate a Nlme request to set the given time in millisecond as the current poll
* rate.
*
* Interface assumptions:
*   The parameter pollTime is value grather than zero.
*
* Return value:
*   The status of the request sended to the NLME.
************************************************************************************/
zbStatus_t ZDO_NLME_ChangePollRate
(
  uint16_t  pollTime
)
{
  zdoNlmeMessage_t zdoNlmeMsg;

  zdoNlmeMsg.msgType = gNlmeChangePollTimeInterval_c;
  zdoNlmeMsg.msgData.changePollTimer.pollTimeInterval = pollTime;

  /*
    This particular Nlme request is Synchronous so the use of a local variable for the
    message will not generaste a memory problem. (Synchronous means that will not leave
    the Sap handler without doing the work).
  */

  return ZDO_NLME_SapHandler( &zdoNlmeMsg );
}

/*
  The stack must keep track of sleep state. If the stack is reset, it must restore the
  proper number of "allowToSleep".
*/
void ZDO_AllowDeviceToSleep(void)
{
  if(gZdoAllowToSleepCounter)
  {
    PWR_AllowDeviceToSleep();
    gZdoAllowToSleepCounter--;
  }
}

/*
  The stack must keep track of sleep state. If the stack is reset, it must restore the
  proper number of "allowToSleep".
*/
void ZDO_DisallowDeviceToSleep(void)
{
  PWR_DisallowDeviceToSleep();
  gZdoAllowToSleepCounter++;
}


/******************************************************************************
*******************************************************************************
* Private Debug Stuff
*******************************************************************************
******************************************************************************/

/* None */
