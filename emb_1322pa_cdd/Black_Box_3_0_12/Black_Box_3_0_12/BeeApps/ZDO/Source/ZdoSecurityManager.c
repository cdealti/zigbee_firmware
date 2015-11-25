/*****************************************************************************
* Security Manager file takes care of all the security related ,
* functionality of ZDO.
*
* (c) Copyright 2005, Freescale, Inc. All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
******************************************************************************/
#ifndef __IAR_SYSTEMS_ICC__
#ifdef MC13226Included_d
    #undef MC13226Included_d
#endif    
    #define MC13226Included_d 0
#endif
#include "PublicConst.h"
#include "AppZdoInterface.h"
#include "BeeStackUtil.h"
#include "BeeStackInterface.h"
#include "BeeStack_Globals.h"
#include "NwkCommon.h"
#include "TMR_interface.h"
#include "ZdoApsInterface.h"
#include "BeeStackConfiguration.h"
#include "ZdpManager.h"
#include "NVM_interface.h"
#if !gArm7ExtendedNVM_d
#include "Nv_Data.h"
#endif
#include "ZdoCommon.h"
#include "ZdoSecurityManager.h"
#include "ZDOStateMachineHandler.h"
#if gStandardSecurity_d || gHighSecurity_d
#include "ZdoNwkManager.h"
#include "ZdoMain.h"
#endif
#include "ApsMgmtInterface.h"
#include "ASL_ZdpInterface.h"
#include "BeeStackParameters.h"
#include "zdputils.h"


/******************************************************************************
*******************************************************************************
* Public Memory
*******************************************************************************
*******************************************************************************/
#if gHighSecurity_d
extern zbEntityAuthentication_t  *gapEntityAuthenticationMaterial[];
#endif

#if gApsLinkKeySecurity_d
extern bool_t SecurityTypeCheck( zbIeeeAddr_t aIeeeAddress );
#endif

#if (gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d) && gRnplusCapability_d
extern void RemoveRouteEntry( uint8_t *pShortAddress );
#else
#define RemoveRouteEntry(pShortAddress)
#endif

extern bool_t gKeepNeighborInformation;

/******************************************************************************
*******************************************************************************
* Private Macros
*******************************************************************************
*******************************************************************************/


/******************************************************************************
*******************************************************************************
* Private Prototypes for standard security only.
*******************************************************************************
******************************************************************************/

/************************************************************************************
* Takes any APSME message send to ZDO and pass it up to the application, does not process
* it or analize it just apss it.
*
* Interface assumptions:
*   The parameter pApsmeInd is not a null pointer.
*
* Return value:
*   NONE.
*
* Revison history:
*   Date   Author  Comments
*  ------  ------  --------
*  060408    MN    Created
************************************************************************************/
void ZDO_SecSendIndicationToApp
(
  void *pApsmeInd
);

#if gStandardSecurity_d || gHighSecurity_d

zbStatus_t ZDO_SecProcessRemoveDeviceInd
(
  zbApsmeRemoveDeviceInd_t  *pRemoveDeviceInd
);

zbStatus_t ZDO_SecProcessNwkTrasnportKeyInd
(
  zbTransportKeyData_t  *pKeyData,
  zbKeyType_t         keyType
);

zbStatus_t ZDO_APSME_Establish_Key_response
(
  zbApsmeEstablishKeyInd_t  *pEstablishInd
);

/************************************************************************************
* If the establish key confirm came with a status equal success then free the memory
* used for the state machine.
*
* Interface assumptions:
*   The parameter pApsmeCnf is not a null pointer.
*
* Return value:
*   The status of the operation.
*
* Revison history:
*   Date   Author  Comments
*  ------  ------  --------
*  230408    MN    Created
************************************************************************************/
zbStatus_t ZDO_SecProcessEstablishKeyCnf
(
  zbApsmeCnf_t  *pApsmeCnf
);

zbStatus_t ZDO_SecProcessLinkOrMastreKeyInd
(
  zbApsmeTransportKeyInd_t  *pKeyIndication
);

#if gTrustCenter_d || gComboDeviceCapability_d
zbStatus_t ZDO_SecProcessUpdateDeviceInd
(
  zbApsmeUpdateDeviceInd_t  *pUpdateDeviceInd
);
#else
#define ZDO_SecProcessUpdateDeviceInd(pUpdateDeviceInd)  gZbSuccess_c
#endif

#if gTrustCenter_d || gComboDeviceCapability_d
zbStatus_t ZDO_SecProcessRequestKeyInd
(
  zbApsmeRequestKeyInd_t  *pRequestKeyInd
);
#else
#define ZDO_SecProcessRequestKeyInd(pRequestKeyInd)  gZbSuccess_c;
#endif

#if gApsLinkKeySecurity_d
void ZDO_SecSendDataKey
(
  zbTransportKeyData_t  *pKeyData,
  zbApsmeRequestKeyInd_t  *pReqKeyIndication
);
#else
#define ZDO_SecSendDataKey(pKeyData, pReqKeyIndication)
#endif


#define ZDO_SecProcessSwitchKeyInd(seqNumber)  SSP_NwkSwitchToAlternativeKey(seqNumber)


/* else gStandardSecurity_d */
/* #if gStandardSecurity_d */
#endif

#if (gStandardSecurity_d || gHighSecurity_d) && gApsLinkKeySecurity_d
zbStatus_t ZDO_APSME_Tunnel_req
(
  uint8_t  tunnelReqId,
  void  *pReqData
);
#endif

#if gStandardSecurity_d || gHighSecurity_d
void ZDO_SecRegisterTCData(void);
#else
#define ZDO_SecRegisterTCData()
#endif



/******************************************************************************
*******************************************************************************
* Private prototypes for high security only.
*******************************************************************************
******************************************************************************/
#if gHighSecurity_d
zbStatus_t ZDO_SecProcessAuthenticateInd
(
  zbApsmeAuthenticateInd_t  *pAuthenticateInd
);

zbStatus_t ZDO_SecProcessAuthenticateCnf
(
  zbApsmeCnf_t  *pAuthenticateCnf
);

#else
#define ZDO_SecProcessAuthenticateInd(pAuthenticateInd) 0
#define ZDO_SecProcessAuthenticateCnf(pAuthenticateCnf) 0
/*#if gHighSecurity_d*/
#endif


#if gStandardSecurity_d || gHighSecurity_d
/************************************************************************************
* Verify the neighbor table entry, and If the neighbor table entry corresponding to
* SenderAddress has a relationship field with value 0x01 (child), that field shall
* be set to the value 0x05 (unauthenticated child).
*
* Interface assumptions:
*   The parameter aSourceAddress is a valid 802.15.4 IEEE Address.
*   The parameter relationShip is either 0x05 or 0x01.
*
* Return value:
*   The previous value of the relationship.
*
************************************************************************************/
extern void SSP_NwkSetRelationship
(
  zbIeeeAddr_t  aSourceAddress,  /* IN: The address of the remote device (Source address). */
  uint8_t  relationShip          /* IN: The relationship Id to be set in the NT. */
);
#endif

#if (gStandardSecurity_d || gHighSecurity_d) && gApsMaxEntriesForPermissionsTable_c
extern bool_t CommandHasPermission
(
  zbNwkAddr_t aSrcAddr,
  permissionsFlags_t  permissionsCategory
);
#endif

#if gRnplusCapability_d
#if gStandardSecurity_d || gHighSecurity_d
#if gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d
extern void RemoveRouteEntry(uint8_t *pShortAddress);
#endif
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


/******************************************************************************
*******************************************************************************
* Public Memory definitions
*******************************************************************************
*******************************************************************************/

#if gStandardSecurity_d || gHighSecurity_d
/*
  Global and public memory declrations for security goes here.
*/

#if gTrustCenter_d || gComboDeviceCapability_d
/*
  Global and public memory declarations for security, exclusive for the
  Trust Center goes here.
*/
extern msgQueue_t gUpdateDeviceQueue;
#endif
#endif


/******************************************************************************
*******************************************************************************
* Public Functions
*******************************************************************************
******************************************************************************/
/*
  This function adds an IEEE address to the exclusion List
*/
#if (gStandardSecurity_d || gHighSecurity_d) && (gTrustCenter_d || gComboDeviceCapability_d) && gDefaultValueOfMaxEntriesForExclusionTable_c
bool_t Zdo_AddToExclusionList
(
  uint8_t *pIeeeAddress
)
{
  index_t cIndex;
  index_t freeIndex = 0xff; /* assumes < 0xff entries in exclusion table */

  /* add item to exclusion list */
  for (cIndex = 0; cIndex < gExclusionMax; cIndex++)
  {
    /* find a free slot */
    if(freeIndex == 0xff && Cmp8BytesToZero(gaExclusionTable[cIndex]))
    {
      freeIndex = cIndex;
    }

    /* already in table */
    if(IsEqual8Bytes(gaExclusionTable[cIndex],pIeeeAddress))
    {
      return TRUE;
    }
  }  /* End of For loop */

  /* no free slot, give up */
  if(freeIndex == 0xff)
  {
    return FALSE;
  }

  /* copy it in and return true */
  Copy8Bytes(gaExclusionTable[freeIndex],pIeeeAddress);
  return TRUE;
}

/*
  This fucntion Removes a device from the exclusion list using the IEEE address.
*/
bool_t Zdo_RemoveFromExclusionList
(
  uint8_t *pIeeeAddress
)
{
  uint8_t  i;

  /* Search into the whole Exclusion list for the device to be removed */
  for (i = 0; i < gExclusionMax; i++)
  {
    if (IsEqual8Bytes(gaExclusionTable[i],pIeeeAddress))
    {
      BeeUtilZeroMemory(gaExclusionTable[i], sizeof(zbIeeeAddr_t));
      return TRUE;
    }
  }

  return FALSE;
}
#endif


/******************************************************************************
*******************************************************************************
* APSME Request generation (Standard and High security)
*******************************************************************************
******************************************************************************/
#if gStandardSecurity_d || gHighSecurity_d
/************************************************************************************
* 4.4.3.1 APSME-TRANSPORT-KEY.request
* The APSME-TRANSPORT-KEY.request primitive is used for transporting a key to
* another device.
*
* 4.4.3.1.1 Semantics of the Service Primitive This primitive shall provide the
* following interface:
*   APSME-TRANSPORT-KEY.request {
*                               DestAddress,
*                               KeyType,
*                               TransportKeyData
*                               }
*
* The ZDO on an initiator device shall generate this primitive when it requires
* a key to be transported to a responder device.
*
* The receipt of an APSME-TRANSPORT-KEY.request primitive shall cause the APSME to
* create a transport-key command packet (see 053474r17 sub-clause 4.4.9.2).
*
* Interface assumptions:
*   The incoming parameter pTransportKeyData is not a null pointer.
*   The KeyType has a value between 0x00 and 0x04.
*   The parameter aDestinationAddress is a valid 64-bit address.
*
* Return value:
*   NONE.
*
************************************************************************************/
void ZDO_APSME_Transport_Key_request
(
  zbIeeeAddr_t           aDestinationAddress,
  zbKeyType_t            keyType,
  zbTransportKeyData_t  *pTransportKeyData
)
{
  apsmeMessage_t  *pZdoApsmeMsg;
  zbTransportKeyData_t  *pKeyData;

  /*
    At this point ther eis no nedd to allocate a big buffers this request will be
    trasnformed into a a command to go OTA, so just the exact amount of memory is needed.
  */
  pZdoApsmeMsg = MSG_Alloc(MbrSizeof(apsmeMessage_t, msgType) + sizeof(zbApsmeTransportKeyReq_t));

  /*
    Like always, if the memory is not available we have nothing to do.
    I love my obvious  comments :-P
  */
  if (!pZdoApsmeMsg)
    return;

  pKeyData = (zbTransportKeyData_t  *)&pZdoApsmeMsg->msgData.transportKeyReq.TransportKeyData;

  /*
    Do you want an obvious comment? well here is it: Fill the payload.
  */
  pZdoApsmeMsg->msgType = gApsmeTransportKeyReq_c;
  pZdoApsmeMsg->msgData.transportKeyReq.KeyType = keyType;
  Copy8Bytes(pZdoApsmeMsg->msgData.transportKeyReq.aDestinationAddress, aDestinationAddress),

  /* This cover the 3 possible types of key to be transported. */
  FLib_MemCpy(pKeyData, pTransportKeyData, sizeof(zbTransportKeyData_t));

  /*
    APSME will generate the propper OTA command.
  */
  if (ZDO_APSME_SapHandler(pZdoApsmeMsg))
  {
    /* Catch the error here. */
  }

}

void ZDO_APSME_Update_Device_request
(
  zbIeeeAddr_t  aExtendedAddress,
  zbNwkAddr_t   aShortAddress,
  uint8_t       rejoin,
  bool_t        secureJoin
)
{
  apsmeMessage_t  *pZdoApsmeMsg;

  /*
    At this point there is no need to allocate a big buffers this request will be
    trasnformed into a a command to go OTA, so just the exact amount of memory is needed.
  */
  pZdoApsmeMsg = MSG_Alloc(MbrSizeof(apsmeMessage_t, msgType) + sizeof(zbApsmeUpdateDeviceReq_t));

  /*
    Like always, if the memory is not available we have nothing to do.
    I love my obvious  comments :-P
  */
  if (!pZdoApsmeMsg)
    return;

  /* NOTE: this must reflect what the indication shows. */
  (void)secureJoin;

  /* Every single Apsme comman has its own Id. */
  pZdoApsmeMsg->msgType = gApsmeUpdateDeviceReq_c;

  /*
    Fill the payload .. fill the payload .. FILL IT!!
  */
  Copy2Bytes(pZdoApsmeMsg->msgData.updateDeviceReq.aDeviceShortAddress, aShortAddress);
  Copy8Bytes(pZdoApsmeMsg->msgData.updateDeviceReq.aDeviceAddress, aExtendedAddress);

  /*
    NOTE: This is NOT the right way to fill the update, must be fix when the new Join
    indication gets ready.
  */
  pZdoApsmeMsg->msgData.updateDeviceReq.status = rejoin;

  /*
    Always send any update device to the trust center, who ever he is.
  */
  Copy8Bytes(pZdoApsmeMsg->msgData.updateDeviceReq.aDestAddress,
             ApsmeGetRequest(gApsTrustCenterAddress_c));

  /*
    APSME will generate the propper OTA command.
  */
  if (ZDO_APSME_SapHandler(pZdoApsmeMsg))
  {
    /* Catch the error here. */
  }
}

void ZDO_APSME_Switch_Key_request
(
  zbIeeeAddr_t      aDestAddress,
  zbKeySeqNumber_t  sequenceNumber
)
{
  apsmeMessage_t  *pZdoApsmeMsg;

  /*
    At this point there is no need to allocate a big buffers this request will be
    trasnformed into a a command to go OTA, so just the exact amount of memory is needed.
  */
  pZdoApsmeMsg = MSG_Alloc(MbrSizeof(apsmeMessage_t, msgType) + sizeof(zbApsmeSwitchKeyReq_t));

  /*
    Like always, if the memory is not available we have nothing to do.
    I love my obvious  comments :-P
  */
  if (!pZdoApsmeMsg)
    return;

  /* We need our request payload, so lest fill it. */
  pZdoApsmeMsg->msgType = gApsmeSwitchKeyReq_c;
  Copy8Bytes(pZdoApsmeMsg->msgData.switchKeyReq.aDestAddress, aDestAddress);
  pZdoApsmeMsg->msgData.switchKeyReq.keySeqNumber = sequenceNumber;

  /*
    APSME will generate the propper OTA command.
  */
  if (ZDO_APSME_SapHandler(pZdoApsmeMsg))
  {
    /* Catch the error here. */
  }
}

void ZDO_APSME_Remove_Device_request
(
  zbIeeeAddr_t  aParentAddress,
  zbIeeeAddr_t  aChildAddress
)
{
  apsmeMessage_t  *pZdoApsmeMsg;

  /*
    At this point there is no need to allocate a big buffers this request will be
    trasnformed into a a command to go OTA, so just the exact amount of memory is needed.
  */
  pZdoApsmeMsg = MSG_Alloc(MbrSizeof(apsmeMessage_t, msgType) + sizeof(zbApsmeRemoveDeviceReq_t));

  /*
    Like always, if the memory is not available we have nothing to do.
    I love my obvious  comments :-P
  */
  if (!pZdoApsmeMsg)
    return;

  pZdoApsmeMsg->msgType = gApsmeRequestKeyReq_c;
  Copy8Bytes(pZdoApsmeMsg->msgData.removeDeviceReq.aChildAddress, aChildAddress);
  Copy8Bytes(pZdoApsmeMsg->msgData.removeDeviceReq.aParentAddress, aParentAddress);

  /*
    APSME will generate the propper OTA command.
  */
  if (ZDO_APSME_SapHandler(pZdoApsmeMsg))
  {
    /* Catch the error here. */
  }
}
#endif

/******************************************************************************
*******************************************************************************
* APSME Request generation (High only)
*******************************************************************************
******************************************************************************/
#if gHighSecurity_d
/*******************************************************************************
* ZDO_APSME_Authenticate_request (053474r17ZB section 4.4.8.1.2)
*
* The ZDO on an initiator or responder device will generate this primitive when 
* it needs to initiate or respond to entity authentication. If the ZDO is 
* responding to an APSME-AUTHENTICATE.indication primitive, it will set the 
* RandomChallenge parameter to the corresponding RandomChallenge parameter in 
* the indication. The 16-octet random challenge originally received from the
* initiator. This parameter is only valid if the Action parameter is equal to 
* RESPOND_ACCEPT.
*
* Revison history:
*   Date   Author  Comments
*  ------  ------  --------
*  180408    BN    Created
*******************************************************************************/
zbStatus_t ZDO_APSME_Authenticate_request
(
  zbIeeeAddr_t  aPartnerAddress,
  zbAction_t action,
  uint8_t *pRandomChallenge
)
{
  apsmeMessage_t  *pZdoApsmeMsg;

  /*
    At this point there is no need to allocate a big buffers this request will be
    trasnformed into a a command to go OTA, so just the exact amount of memory is needed.
  */
  pZdoApsmeMsg = MSG_Alloc(MbrSizeof(apsmeMessage_t, msgType) + sizeof(zbApsmeAuthenticateReq_t));

  /*
    Like always, if the memory is not available we have nothing to do.
    I love my obvious  comments :-P
  */
  if (!pZdoApsmeMsg)
    return gZbFailed_c;

  /*Set the message type to a APSM_AuthenticateRequest*/
  pZdoApsmeMsg->msgType = gApsmeAuthenticateReq_c;

  /* Copy the Parner Addres with the one received in the argument. */
  Copy8Bytes(pZdoApsmeMsg->msgData.authenticateReq.aPartnerAddress, aPartnerAddress);

  /*
    Set the acction to be done.
  */
  pZdoApsmeMsg->msgData.authenticateReq.action = action;

  /*
    Copy the RandomChallenge from the argument. This parameter is only valid if the
    Action parameter is equal to RESPOND_ACCEPT.
  */
  Copy16Bytes(pZdoApsmeMsg->msgData.authenticateReq.aRandomChanllenge, pRandomChallenge);

  /*
    APSME will generate the propper OTA command.
  */
  if (ZDO_APSME_SapHandler(pZdoApsmeMsg))
  {
    /* Catch the error here. */
  }

  return gZbSuccess_c;
}

#endif

/******************************************************************************
*******************************************************************************
* ZDO indication process (Standard and High security)
*******************************************************************************
******************************************************************************/
#if gStandardSecurity_d || gHighSecurity_d
/************************************************************************************
* This function receives each indication refered from APSME (APS Commands), the stack's
* default behaviour to any of the possible indication is also taked care here.
* Each and every single indication is passed up to next higher layer (The application),
* and is responsability of the application to free the memory allocated for each indication.
* The mechanism used to deliver the indication to the app is the ZDP_APP_SapHandler, if the
* curretn application does not has any callback registered the sap handler will free the
* buffer.
*
* Interface assumptions:
*   The incoming parameter pApsmeInd is not a null pointer.
*
* Return value:
*   The status of the porcessed indication.
************************************************************************************/
zbStatus_t ZDO_SecProcessApsmeIndication
(
  zbApsmeZdoIndication_t *pApsmeInd  /* IN: The indication packet comming from the APS layer. */
)
{
  /*
    By default ZDO asumes that will be unable to handle the indication packet.
  */
  zbStatus_t  status = gZbFailed_c;
  uint8_t *pTCNwkAddress, aNwkAddr[2];

  zbApsmeTransportKeyInd_t  *pTrasnportKeyInd;

  /*
    Process each indication separated from the others.
  */
  switch (pApsmeInd->indicationID)
  {
    /*
      Process any transport key indication.
    */
    case gApsmeTransportKeyInd_c:

      /* Fast access to the momory, avoid shifting and casting. */
      pTrasnportKeyInd = (zbApsmeTransportKeyInd_t *)&pApsmeInd->indicationData;

      /* Update the TC long an short address if already know it. */
      ApsmeSetRequest(gApsTrustCenterAddress_c, pTrasnportKeyInd->aSrcAddress);
      pTCNwkAddress = APS_GetNwkAddress(pTrasnportKeyInd->aSrcAddress, aNwkAddr);
      if (pTCNwkAddress)
      {
        /* Update the TC nwk address if we know it other wise keep the default one.! */
        ApsmeSetRequest(gApsTrustCenterNwkAddress_c, pTCNwkAddress);
      }

      switch(pTrasnportKeyInd->keyType)
      {
        /*
          The incomming indication is a Standard Network Key or a High security key,
          the procedure is the same.
        */
        case gStandardNetworkKey_c:
        case gHighSecurityNetworkKey_c:
          status = ZDO_SecProcessNwkTrasnportKeyInd((void *)&pTrasnportKeyInd->keyData,
                                                    pTrasnportKeyInd->keyType);
          break;

        /*
          Any type of link key uses the same procedure, the only difference is the key type.
        */
        case gTrustCenterLinkKey_c:
        case gApplicationLinkKey_c:
        /*
          For any Master key start by defualt a SKKE procedure with the other device
          involved. The TC master key gets registered in the APS module.
        */
        case gTrustCenterMasterKey_c:
        case gApplicationMasterKey_c:
          ZDO_SecProcessLinkOrMastreKeyInd(pTrasnportKeyInd);
          break;

      }
      break;

    /*
      Process the update device indication, all of them are defer to the TC state machine
      (See ZdoMain.c)
    */
    case gApsmeUpdateDeviceInd_c:
      status = ZDO_SecProcessUpdateDeviceInd((void *)&pApsmeInd->indicationData);
      break;

    /*
      The switch key command and indication only afects the Nwk key (High or Standard).
    */
    case gApsmeSwitchKeyInd_c:
      status = ZDO_SecProcessSwitchKeyInd(pApsmeInd->indicationData.zbZdoSwitchKeyInd.keySeqNumber);
      break;

    /*
      If the TC commands the exclussion of any device using the remove device command,
      then, it will be process here.
    */
    case gApsmeRemoveDeviceInd_c:
      status = ZDO_SecProcessRemoveDeviceInd((void *)&pApsmeInd->indicationData);
      break;

    /*
      If any devie needs any type of key, this is the place where those keys got served.
    */
    case gApsmeRequestKeyInd_c:
      status = ZDO_SecProcessRequestKeyInd((void *)&pApsmeInd->indicationData);
      break;

    /*
      Verifies that the current establish method is SKKE and generate a SKKE Command.
    */
    case gApsmeEstablishKeyInd_c:
      status = ZDO_APSME_Establish_Key_response((void *)&pApsmeInd->indicationData);
      break;

    case gApsmeAuthenticateInd_c:
#if gHighSecurity_d
      status = ZDO_SecProcessAuthenticateInd((void *)&pApsmeInd->indicationData);
#endif
      break;

    case gApsmeAuthenticateCnf_c:
#if gHighSecurity_d
      status = ZDO_SecProcessAuthenticateCnf((void *)&pApsmeInd->indicationData);
#endif
      break;

     /*
       The Establish key confirm handles the any confirm status for the SKKE procedure,
       also, it administrates the time outs for the SKKE state machine.
     */
     case gApsmeEstablishKeyCnf_c:
       status = ZDO_SecProcessEstablishKeyCnf((void *)&pApsmeInd->indicationData);
       break;
#if 0
     case gApsmeTransportKeyCnf_c:
      /* Do stuff here. */
      break;

     case gApsmeTunnelCnf_c:
      /* Do stuff here. */
      break;
#endif      
  }  /* End of switch */

  /*
    No matter what, we should pass the indication up to the app.
  */
  ZDO_SecSendIndicationToApp(pApsmeInd);

  /*
    ZDO is done with the current indications
  */
  return status;
}
#endif


#if gStandardSecurity_d || gHighSecurity_d
zbStatus_t ZDO_SecProcessLinkOrMastreKeyInd
(
  zbApsmeTransportKeyInd_t  *pKeyIndication
)
{
#if gApsLinkKeySecurity_d
  uint8_t  *pRemoteDevAddress = NULL;
  zbTransportKeyData_t  keyData;
  zbStatus_t  status;

  if ((pKeyIndication->keyType == gApplicationLinkKey_c) || (pKeyIndication->keyType == gApplicationMasterKey_c))
  {

    pRemoteDevAddress = (void *)pKeyIndication->keyData.applicationKey.aParentAddress;
  }
  /*
    This function can only handle two types of key.
  */
  else
  {
    return gZbFailed_c;
  }

  Copy16Bytes(keyData.ApplicationKey.aKey, pKeyIndication->keyData.applicationKey.aApplicationKey);
  Copy8Bytes(keyData.ApplicationKey.aParentAddress, pKeyIndication->keyData.applicationKey.aParentAddress);

  status = SSP_RegisterKeyData(pKeyIndication->keyType, &keyData);

  if (pKeyIndication->keyType == gApplicationMasterKey_c)
  {
    if ((ZDO_GetState() != gZdoDeviceAuthenticationState_c) &&
        (ZDO_GetState() != gZdoDeviceWaitingForKeyState_c))
    {
      if (pKeyIndication->keyData.applicationKey.initiatorFlag)
      {
        status = ZDO_APSME_Establish_Key_request(pRemoteDevAddress, FALSE,
                                                 NlmeGetRequest(gNwkIeeeAddress_c),
                                                 gSKKE_Method_c);
      }
    }
  }

  return (status);
#else
  (void)pKeyIndication;
   return gZbFailed_c;
#endif
}

/*
*/
void ZDO_SecRegisterLinkOrMastreKeyInd
(
  zbApsmeTransportKeyInd_t  *pKeyIndication
)
{
#if gApsLinkKeySecurity_d
  zbTransportKeyData_t  keyData;

  if ((pKeyIndication->keyType == gTrustCenterLinkKey_c) || (pKeyIndication->keyType == gTrustCenterMasterKey_c))
  {
    Copy16Bytes(keyData.ApplicationKey.aKey, pKeyIndication->keyData.trustCenterKey.aTrustCenterKey);
    Copy8Bytes(keyData.ApplicationKey.aParentAddress, pKeyIndication->aSrcAddress);
    ApsmeSetRequest(gApsTrustCenterAddress_c, pKeyIndication->aSrcAddress);
  }
  else
  {
    return ;
  }

  if (pKeyIndication->keyType == gTrustCenterMasterKey_c)
  {
    /*
      WE may be receiving the TC master key at joining time we need tohave it on the
      Address map table for further trasnformations.
    */
    (void)APS_AddToAddressMap(ApsmeGetRequest(gApsTrustCenterAddress_c),
                              ApsmeGetRequest(gApsTrustCenterNwkAddress_c));
  }

  (void)SSP_RegisterKeyData(pKeyIndication->keyType, &keyData);
#else
  (void)pKeyIndication;
#endif
}

#endif


#if gStandardSecurity_d || gHighSecurity_d
/************************************************************************************
* Process 5 of the 6 possible status code on an Update Device Indication, this only
* work on the trust center side. This function is coordinated with the trust center
* state machine.
*
* Interface assumptions:
*   The parameter pUpdateIndication is not a null pointer.
*
* Return value:
*   NONE.
************************************************************************************/
#if gTrustCenter_d || gComboDeviceCapability_d
void ZDO_SecProcessUpdateDeviceIndication
(
  zbApsmeUpdateDeviceInd_t *pUpdateIndication
)
{
#if gComboDeviceCapability_d
  if (!gTrustCenter)
  {
    return;
  }
#endif
  /*
    The process of the update device status must be done here for the trust center.
  */
  switch (pUpdateIndication->status)
  {
    /*
      Status = 0x02, this indication only arrives when a node has left the network.
    */
    case gDeviceLeft_c:
      /* Forget about the leaving device. */
      /* Setting the last parameter to TRUE will erase APS secure material */
      ZDO_SecClearDeviceData(pUpdateIndication->aDeviceShortAddress, pUpdateIndication->aDeviceAddress, TRUE);

      /* Eliminate the device from the current list. */
      APS_RemoveFromAddressMap(pUpdateIndication->aDeviceAddress);

      /* The process on the Trust center state machine must end. */
      TS_SendEvent(gZdoTaskID_c, gDone_c);

      break;

    case gStandardDeviceUnsecuredRejoin_c:
    case gHighSecurityDeviceUnsecuredRejoin_c:
    case gStandardDeviceSecuredReJoin_c:
    case gHighSecurityDeviceSecuredReJoin_c:
      /* Allow devices to rejoin */
      ZDO_SendAuthenticationToTrustCenter(TRUE);
      break;

    default:
      /* The process on the Trust center state machine must end. */
      TS_SendEvent(gZdoTaskID_c, gDone_c);
      break;
  }
}
#endif
#endif

#if gStandardSecurity_d || gHighSecurity_d
void ZDO_SecClearDeviceData(zbNwkAddr_t aNwkAddr, zbIeeeAddr_t aIEEEAddr, bool_t fullErase)
{
  if(gKeepNeighborInformation)
  {
    return;
  }
#if gRnplusCapability_d
#if gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d
#if gComboDeviceCapability_d
  if (NlmeGetRequest(gDevType_c) != gEndDevice_c)
#endif
  {
    /* Remove the old routes to the joining device */
    RemoveRouteEntry(aNwkAddr);
  }
#endif
#endif

  /* If we got any comunication with this device erase it ALL.!! */
  SSP_NwkResetDeviceFrameCounter(aIEEEAddr);

  if (fullErase)
  {
    /*
      If the device has link key support we must be sure to eliminate the Application
      key information.
    */
#if gApsLinkKeySecurity_d
    APS_RemoveSecurityMaterialEntry(aIEEEAddr);
#endif
  }

  (void)aNwkAddr;
  gSecClearDeviceDataEvent = onLostChildBroadcast_c;
}
#endif

#if gStandardSecurity_d || gHighSecurity_d
/************************************************************************************
* Generates the Establish key response packet to APSME, in order to process the
* current key establishment procedure.
*
* Interface assumptions:
*   The parameter pApsmeCnf is not a null pointer.
*
* Return value:
*   The status of the operation.
*
* Revison history:
*   Date   Author  Comments
*  ------  ------  --------
*  180408    MN    Created
************************************************************************************/
zbStatus_t ZDO_APSME_Establish_Key_response
(
  zbApsmeEstablishKeyInd_t  *pEstablishInd
)
{
#if gSKKESupported_d
  /*
    The pointer to allocate the memory to be sent out through the
    ZDO_APSME Sap Handler.
  */
  apsmeMessage_t  *pZdoApsmeMsg;
  
  /*
    Verify for authorization in the permissions table.
  */
#if gApsMaxEntriesForPermissionsTable_c
  if (!CommandHasPermission(pEstablishInd->aInitiatorAddress, gSecuritySettingsPermission_c))
  {
    return gZdpNotAuthorized_c;
  }
#endif /* (gApsMaxEntriesForPermissionsTable_c) */


  pZdoApsmeMsg = MSG_Alloc(MbrSizeof(apsmeMessage_t, msgType) + sizeof(zbApsmeEstablisKeyRsp_t));

  /*
    If the memory is not available there is nothing to do.
  */
  if (!pZdoApsmeMsg)
    return gZbFailed_c;

  /*
    Build the Establish Key response so the local device can start with the SKKE porcedure.
    (if we got a SKKE1 indication, then we are the responder.!)
  */
  pZdoApsmeMsg->msgType = gApsmeEstablishKeyRsp_c;
  Copy8Bytes(pZdoApsmeMsg->msgData.establishKeyRsp.aInitiatorAddress,
             pEstablishInd->aInitiatorAddress);

  /*
    If the establish key method requested from the initiator is other than SKKE fail to
    accept.
  */
  pZdoApsmeMsg->msgData.establishKeyRsp.accept = (pEstablishInd->keyEstablishmentMethod)? FALSE: TRUE;

  if (ZDO_APSME_SapHandler(pZdoApsmeMsg))
  {
    /* Catch Error here.! */
  }
  return gZbSuccess_c;
#else
  (void)pEstablishInd->keyEstablishmentMethod;
  return gZbFailed_c;
#endif
}
#endif

#if gStandardSecurity_d || gHighSecurity_d
/************************************************************************************
* 053474r17 - Sec. 4.4.2.1 APSME-ESTABLISH-KEY.request
* The APSME-ESTABLISH-KEY request primitive is used for initiating a keyestablishment
* protocol. This primitive can be used when there is a need to securely communicate
* with another device.
* One device will act as an initiator device, and the other device will act as the
* responder. The initiator shall start the key-establishment protocol by issuing:
*
* - The APSME-ESTABLISH-KEY.request with parameters indicating the address of the
*   responder device.
*
* - An indication of which key-establishment protocol to use (currently SKKE).
*
* Interface assumptions:
*   The parameters aResponderAddress and aParentAddress are valid 64-bit addresses.
*   The parameter useParent is a boolena value (TRUE | FALSE).
*   The parameter keyEstablishmentMethod has a value from 0x00 to 0xff.
*
* Return value:
*   The status of the ZDO request.
************************************************************************************/
zbStatus_t ZDO_APSME_Establish_Key_request
(
  zbIeeeAddr_t  aResponderAddress,
  bool_t        useParent,
  zbIeeeAddr_t  aParentAddress,
  uint8_t       keyEstablishmentMethod
)
{
#if gSKKESupported_d
  apsmeMessage_t  *pZdoApsmeMsg;
  zbApsmeEstablishKeyReq_t *pEstablishReq;

  pZdoApsmeMsg = MSG_Alloc(MbrSizeof(apsmeMessage_t, msgType) +
                           sizeof(zbApsmeEstablishKeyReq_t));

  if (!pZdoApsmeMsg)
  {
    return gZbFailed_c;
  }

  pZdoApsmeMsg->msgType = gApsmeEstablishKeyReq_c;
  pEstablishReq = &pZdoApsmeMsg->msgData.establishKeyReq;
  Copy8Bytes(pEstablishReq->aResponderAddress, aResponderAddress);
  pEstablishReq->keyEstablishmentMethod = keyEstablishmentMethod;
  pEstablishReq->useParent = useParent;

  if (pEstablishReq->useParent)
  {
    Copy8Bytes(pEstablishReq->aParentAddress, aParentAddress);
  }

  if (ZDO_APSME_SapHandler(pZdoApsmeMsg))
  {
    /* Catch Error here.! */
  }

  return gZbSuccess_c;
#else
  (void)aResponderAddress;
  (void)useParent;
  (void)aParentAddress;
  (void)keyEstablishmentMethod;
  return gZbSuccess_c;
#endif
}
#endif

/******************************************************************************
*******************************************************************************
* ZDO Confirm process
*******************************************************************************
******************************************************************************/
#if gStandardSecurity_d || gHighSecurity_d
/************************************************************************************
* If the establish key confirm came with a status equal success then free the memory
* used for the state machine.
*
* Interface assumptions:
*   The parameter pApsmeCnf is not a null pointer.
*
* Return value:
*   The status of the operation.
*
* Revison history:
*   Date   Author  Comments
*  ------  ------  --------
*  230408    MN    Created
************************************************************************************/
zbStatus_t ZDO_SecProcessEstablishKeyCnf
(
  zbApsmeCnf_t  *pApsmeCnf
)
{
#if gSKKESupported_d
  /*
    The Skke state machine data need to be acced either succes or failure.
  */
  zbKeyEstablish_t  *pSkkeData;

#if gTrustCenter_d || gComboDeviceCapability_d
  zbTransportKeyData_t  *pTransportKeyData;
  zbNwkSecurityMaterialSet_t  *pSecurityMaterial;
  zbApsmeTransportKeyReq_t  keyReq;
#endif

  pSkkeData = SSP_ApsRetriveStateMachineEntry(pApsmeCnf->aAddress);

  if (!pSkkeData)
  {
    /*
      The entry was not found, either the node is not there or other problem, nothing
      to do.!
    */
    return gZbFailed_c;
  }

#if gTrustCenter_d || gComboDeviceCapability_d

#if gComboDeviceCapability_d
  if (!gTrustCenter)
  {
    /*
      On error or success the state machine entry is no longer needed.
    */
    SSP_ApsRemoveStateMachineEntry(pApsmeCnf->aAddress);

    return gZbSuccess_c;
  }
#endif

  /*
    If the current device is the trustcenter and the confirmation cameup with a status
    of succes then send teh transport key to the new device.
  */
  if (pSkkeData->sendTrasnportKey && (pApsmeCnf->status == gZbSuccess_c))
  {
    /*
      Get the current Nwk secure material set. Always deliver the fresh and active data.
    */
    pSecurityMaterial = SSP_NwkGetSecurityMaterialSet(NlmeGetRequest(gNwkActiveKeySeqNumber_c));

    pTransportKeyData = &keyReq.TransportKeyData;

    if (pSecurityMaterial)
    {
      if (NlmeGetRequest(gNwkPreconfiguredKey_c))
      {
        BeeUtilZeroMemory(pTransportKeyData->NetworkKey.aNetworkKey, sizeof(zbAESKey_t));
      }
      else
      {
        Copy16Bytes(pTransportKeyData->NetworkKey.aNetworkKey, pSecurityMaterial->nwkKey);
      }
      pTransportKeyData->NetworkKey.useParent = pSkkeData->useParent;
      if (pSkkeData->useParent)
      {
        Copy8Bytes(pTransportKeyData->NetworkKey.aParentAddress, pSkkeData->aResponderPartnerAddress);
      }
      pTransportKeyData->NetworkKey.keySeqNumber = pSecurityMaterial->keySeqNumber;

      keyReq.KeyType = pSecurityMaterial->keyType;
      Copy8Bytes(keyReq.aDestinationAddress, pSkkeData->aRemoteDeviceAddress);
      FLib_MemCpy(&keyReq.TransportKeyData, pTransportKeyData, sizeof(keyReq.TransportKeyData));

      /*
        ZDO Must send the APSME trasnport key request to be deliver to the newly joined
        device.
      */
      if (pSkkeData->useParent)
      {
        (void)ZDO_APSME_Tunnel_req(gApsmeTransportKeyReq_c, &keyReq);
      }
      else
      {
        ZDO_APSME_Transport_Key_request(keyReq.aDestinationAddress,
                                        keyReq.KeyType,
                                        &keyReq.TransportKeyData);
      }
    }
    pSecurityMaterial = NULL;
  }
#endif

  /*
    On error or success the state machine entry is no longer needed.
  */
  SSP_ApsRemoveStateMachineEntry(pApsmeCnf->aAddress);

  /* Only on a success we keep the key information. */
  if (pApsmeCnf->status == gZbSuccess_c)
  {
    ZdoNwkMng_SaveToNvm(zdoNvmObject_ApsLinkKeySet_c);
  }
  else
  {
    if (!ZDO_IsRunningState())
    {
      ZDO_StopEx(gZdoStopMode_StopAndRestart_c);
    }
  }

  return gZbSuccess_c;
#else
  (void)pApsmeCnf;
  return gZbFailed_c;
#endif
}
#endif

/******************************************************************************
*******************************************************************************
* Utility function porcess
*******************************************************************************
******************************************************************************/

#if gStandardSecurity_d || gHighSecurity_d
/************************************************************************************
* Receives the information from a join indication or an update device command and
* generates a transport key request with that information.
* This function is internal and only to be use by the trust center, is not the one to
* be use by the app.
* This function only generates Nwk keys. And is only used during the authentication
* procedure.
*
* Interface assumptions:
*   The parameter pNodeInfo is not a null pointer.
*   The parameter isChild is either TRUE or FALSE.
*
* Return value:
*   NONE.
*
************************************************************************************/
#if gTrustCenter_d || gComboDeviceCapability_d
void ZDO_SecSendTransportKey
(
  void *pNodeInfo,
  bool_t isChild
)
{
  /*
    The function does not know if the trasnport key to be send out
    will get the info from an Update device indication or a joining
    indication, so we need to have a way to access the info in both cases.
  */
  nlmeJoinIndication_t *pJoinIndication = NULL;
  zbApsmeUpdateDeviceInd_t  *pUpdateIndication = NULL;

  /*
    The pointer to a llocate the memory to be sent out through the
    ZDO_APSME Sap Handler.
  */
  apsmeMessage_t  *pZdoApsmeMsg;

  /*
    The actual APSME command to be send out in this particular function.
  */
  zbApsmeTransportKeyReq_t  *pApsmeTransportKey;

  zbNwkSecurityMaterialSet_t  *pSecurityMaterial;
  zbTransportKeyData_t  *pKeyData;

#if gComboDeviceCapability_d
  if (!gTrustCenter)
  {
    return;
  }
#endif

  /*
    The need memory for the command is the command data plus the size
    in bytes for the command Id.
  */
  pZdoApsmeMsg = MSG_Alloc(MbrSizeof(apsmeMessage_t, msgType) +
                           sizeof(zbApsmeTransportKeyReq_t));

  /*
    If for any reason the trust center is not able to generate the command,
    stop the security procedure and fail to authenticate the device.
  */
  if (!pZdoApsmeMsg)
    return;

  /*
    Get the current and active data.
  */
  pSecurityMaterial = SSP_NwkGetSecurityMaterialSet(NlmeGetRequest(gNwkActiveKeySeqNumber_c));

  /*
    Fail if the secure material can not be obtained.
  */
  if (!pSecurityMaterial)
    return;

  /*
    This function alway will generate a single command, a transport key.
  */
  pZdoApsmeMsg->msgType = gApsmeTransportKeyReq_c;

  /*
    Get a fast memory access to the place in memory where the message will be build.
  */
  pApsmeTransportKey = &(pZdoApsmeMsg->msgData.transportKeyReq);

  /*
    We do not know which trasnport key will be.
  */
  pKeyData = &pApsmeTransportKey->TransportKeyData;

  pApsmeTransportKey->KeyType = pSecurityMaterial->keyType;

  pKeyData->NetworkKey.keySeqNumber = pSecurityMaterial->keySeqNumber;

  if (NlmeGetRequest(gNwkPreconfiguredKey_c))
    BeeUtilZeroMemory(pKeyData->NetworkKey.aNetworkKey, sizeof(zbAESKey_t));
  else
    Copy16Bytes(pKeyData->NetworkKey.aNetworkKey, pSecurityMaterial->nwkKey);

  /*
    Catching the indication info to generate the transport key.
  */
  if (isChild)
  {
    pJoinIndication = (nlmeJoinIndication_t *)pNodeInfo;
    Copy8Bytes(pApsmeTransportKey->aDestinationAddress,pJoinIndication->aExtendedAddress);
    pKeyData->NetworkKey.useParent = FALSE;
  }
  else
  {
    pUpdateIndication = (zbApsmeUpdateDeviceInd_t *)pNodeInfo;
    Copy8Bytes(pApsmeTransportKey->aDestinationAddress, pUpdateIndication->aDeviceAddress);
    pKeyData->NetworkKey.useParent = TRUE;
    Copy8Bytes(pKeyData->NetworkKey.aParentAddress, pUpdateIndication->aSrcAddress);
#if gApsLinkKeySecurity_d
    if (!SecurityTypeCheck(pUpdateIndication->aDeviceAddress))
    {
      if (SSP_ApsIsLinkEstablished(pApsmeTransportKey->aDestinationAddress))
      {
        (void)ZDO_APSME_Tunnel_req(gApsmeTransportKeyReq_c, &(pZdoApsmeMsg->msgData.transportKeyReq));
        MSG_Free(pZdoApsmeMsg);
        return;
      }
    }
#endif
  }

  /*
    Send the command out to the APSME Module to be processed.
  */
  (void)ZDO_APSME_SapHandler(pZdoApsmeMsg);
}
#endif


#if gApsLinkKeySecurity_d
void ZDO_SecSendTCMasterKey
(
  void *pNodeInfo,
  bool_t isChild
)
{

#if gTrustCenter_d || gComboDeviceCapability_d
  nlmeJoinIndication_t *pJoinIndication;
  zbApsmeUpdateDeviceInd_t *pUpdateIndication;
  uint8_t  index;
  zbApsDeviceKeyPairSet_t devKeyPairSetEntry;
  uint8_t  *pDstAddress;
#if gSKKESupported_d  
  uint8_t  *pParentAddress;
  zbTransportKeyData_t  keyData;
#endif  

#if gComboDeviceCapability_d
  if (!gTrustCenter)
  {
    return;
  }
#endif

  if (isChild)
  {
    pJoinIndication = (nlmeJoinIndication_t *)pNodeInfo;
    pDstAddress = pJoinIndication->aExtendedAddress;
    
  }
  else
  {
    pUpdateIndication = (zbApsmeUpdateDeviceInd_t *)pNodeInfo;
    pDstAddress = pUpdateIndication->aDeviceAddress;
  }

  index = SSP_ApsGetIndexFromIEEEaddress(pDstAddress);

  if (index == gNotInAddressMap_c)
  {
    return;
  }

  (void)DevKeyPairSet_GetTableEntry(index, &devKeyPairSetEntry);
#if gSKKESupported_d
  devKeyPairSetEntry.keyType = gTrustCenterMasterKey_c;
  (void)KeySet_GetTableEntry(devKeyPairSetEntry.iKey, keyData.TrustCenterKey.aKey);

  pParentAddress = (isChild)? NlmeGetRequest(gNwkIeeeAddress_c): pUpdateIndication->aSrcAddress;

  Copy8Bytes(keyData.TrustCenterKey.aParentAddress, pParentAddress);

  ZDO_APSME_Transport_Key_request(pDstAddress, gTrustCenterMasterKey_c, &keyData);
#else
  devKeyPairSetEntry.keyType = gNoApsKey_c;
#endif
 (void)DevKeyPairSet_SetTableEntry(index, &devKeyPairSetEntry);
#else
  (void)isChild;
  (void)pNodeInfo;
#endif

}
#endif

zbStatus_t ZDO_APSME_Tunnel_req
(
  uint8_t  tunnelReqId,
  void *pReqData
)
{
#if (gStandardSecurity_d || gHighSecurity_d) && gApsLinkKeySecurity_d
#if gTrustCenter_d || gComboDeviceCapability_d
  apsmeMessage_t  *pZdoApsmeMsg;

  zbSize_t  size;
  zbSize_t  reqSize;

  zbApsmeTunnelReq_t  *pTunnelReq;

#if gComboDeviceCapability_d
  if (!gTrustCenter)
  {
    return gZbSuccess_c;
  }
#endif

  size = MbrSizeof(apsmeMessage_t, msgType);

  size += MbrOfs(zbApsmeTunnelReq_t, msgData);

  switch (tunnelReqId)
  {
    case gApsmeTransportKeyReq_c:
      reqSize = sizeof(zbApsmeTransportKeyReq_t);
      break;
  }

  pZdoApsmeMsg = MSG_Alloc(size + reqSize);

  if (!pZdoApsmeMsg)
  {
    return gZbFailed_c;
  }

  pZdoApsmeMsg->msgType = gApsmeTunnelReq_c;
  pTunnelReq = &pZdoApsmeMsg->msgData.tunnelReq;

  pTunnelReq->msgType = tunnelReqId;
  FLib_MemCpy(&pTunnelReq->msgData, pReqData, reqSize);

  return ZDO_APSME_SapHandler(pZdoApsmeMsg);
#else
  (void)tunnelReqId;
  (void)pReqData;
  return gZbFailed_c;
#endif
#else
  (void)tunnelReqId;
  (void)pReqData;
  return gZbFailed_c;
#endif
}

/************************************************************************************
* Takes any incomming update device indication and let it be preocess by the trust
* center state machine.
* Only the trust center device handles the update device indication.
*
* Interface assumptions:
*   The parameter pUpdateDeviceInd is not a null pointer.
*
* Return value:
*   Teh status for this operation.
*
* Revison history:
*   Date   Author  Comments
*  ------  ------  --------
*  060208    MN    Created
************************************************************************************/
#if gTrustCenter_d || gComboDeviceCapability_d
zbStatus_t  ZDO_SecProcessUpdateDeviceInd
(
  zbApsmeUpdateDeviceInd_t  *pUpdateDeviceInd
)
{
  zbApsmeUpdateDeviceInd_t  *pUpdateDevice;

#if gComboDeviceCapability_d
  if (!gTrustCenter)
  {
    return gZbFailed_c;
  }
#endif

  /*
    ZDO Will free any incoming indication, so we need to copy this one to process it later
    on the state machine.
  */
  pUpdateDevice = MSG_Alloc(sizeof(zbApsmeUpdateDeviceInd_t));

  if (!pUpdateDevice)
    return gZbFailed_c;

  FLib_MemCpy(pUpdateDevice, pUpdateDeviceInd, sizeof(zbApsmeUpdateDeviceInd_t));

  /*
    Send this through the State Machine.!!
  */
  MSG_Queue(&gUpdateDeviceQueue, pUpdateDevice);
  TS_SendEvent(gZdoTaskID_c, gUpdateDeviceIndication_c);
  return gZbSuccess_c;
}
#endif

#if gTrustCenter_d || gComboDeviceCapability_d
zbStatus_t  ZDO_SecProcessRequestKeyInd
(
  zbApsmeRequestKeyInd_t  *pRequestKeyInd
)
{
  zbTransportKeyData_t  keyData;
  zbNwkSecurityMaterialSet_t  *pNwkSecMaterialSet;
  zbKeyType_t  keyType;

#if gComboDeviceCapability_d
  if (!gTrustCenter)
  {
    return gZbSuccess_c;
  }
#endif

  /*
    The remote devices are requesting for a Nwk Key.
  */
  if (pRequestKeyInd->keyType == 0x01)
  {
    pNwkSecMaterialSet = SSP_NwkGetSecurityMaterialSet(NlmeGetRequest(gNwkActiveKeySeqNumber_c));
    if (!pNwkSecMaterialSet)
    {
      return gZbFailed_c;
    }
    keyData.NetworkKey.keySeqNumber = NlmeGetRequest(gNwkActiveKeySeqNumber_c);
    Copy16Bytes(keyData.NetworkKey.aNetworkKey,pNwkSecMaterialSet->nwkKey);
    keyData.NetworkKey.useParent = FALSE;
    keyType = pNwkSecMaterialSet->keyType;
  }

  /*
    The devices are requesting for either a APS link key or a APS Master key.
  */
  else if (pRequestKeyInd->keyType == 0x02)
  {
    ZDO_SecSendDataKey(&keyData, pRequestKeyInd);
    return gZbSuccess_c;
  }

  /*
    If the remote device is asking for any of the reserved values, do nothing.
  */
  else
  {
    return gZbFailed_c;
  }

  ZDO_APSME_Transport_Key_request(pRequestKeyInd->aSrcAddress, keyType, &keyData);
  return gZbSuccess_c;
}
#endif

#if gApsLinkKeySecurity_d
void ZDO_SecSendDataKey
(
  zbTransportKeyData_t  *pKeyData,
  zbApsmeRequestKeyInd_t  *pReqKeyIndication
)
{
#if gTrustCenter_d || gComboDeviceCapability_d
  /*
    By freescale request the stack always responds with an Application Master key.
  */
#if gComboDeviceCapability_d
  if (!gTrustCenter)
  {
    return;
  }
#endif
  Copy8Bytes(pKeyData->ApplicationKey.aParentAddress, pReqKeyIndication->aParentAddress);
  SSP_ChallengeGenPrimitive(pKeyData->ApplicationKey.aKey);
  pKeyData->ApplicationKey.Initiator = TRUE;
  ZDO_APSME_Transport_Key_request(pReqKeyIndication->aSrcAddress, mDefaultValueOfApplicationKeyType_c, pKeyData);

  pKeyData->ApplicationKey.Initiator = FALSE;
  Copy8Bytes(pKeyData->ApplicationKey.aParentAddress, pReqKeyIndication->aSrcAddress);
  ZDO_APSME_Transport_Key_request(pReqKeyIndication->aParentAddress, mDefaultValueOfApplicationKeyType_c, pKeyData);
#else
  (void)pKeyData;
  (void)pReqKeyIndication;
#endif
}
#endif

/************************************************************************************
* For any device that is not the trust center takes the trasnport key indication and
* process it (Standard or high security).
* During the the authentication porcedure, if the device is on preconfigured nwk key
* environment, the receiveing key must be the dummy key (All filled with zeros) and the
* devices just register the current sequence number for the key, but, on a non 
* preconfigured key, the devices must check that the key is not the dummy key and then
* register the key data into the local security material.
*
* Interface assumptions:
*   The parameter pKeyData is not a null pointer.
*   The parameter keyType is either 0x01 (Standard) or 0x05 (High).
*
* Return value:
*   Teh status for this operation.
*
* Revison history:
*   Date   Author  Comments
*  ------  ------  --------
*  060208    MN    Created
************************************************************************************/
zbStatus_t ZDO_SecProcessNwkTrasnportKeyInd
(
  zbTransportKeyData_t  *pKeyData,
  zbKeyType_t         keyType
)
{
#if !gTrustCenter_d || gComboDeviceCapability_d
  /*
    A boolean variale to register if the incomming key is all zeros (TRUE), or, is just
    a regular AES key (FALSE).
  */
  bool_t zeroKeyStatus;

  /*
    A pointer to the current secure Nwk security material set.
  */
  zbNwkSecurityMaterialSet_t  *pSecurityMaterialSet;

#if gComboDeviceCapability_d
  if (gTrustCenter)
  {
    return gZbSuccess_c;
  }
#endif

  /*
    The device is already authenticated, so just register the key info into the Security
    material and leave.
  */
  if(ZDO_IsRunningState())
  {
    (void)SSP_RegisterKeyData(keyType, pKeyData);
    /*
      The secure materials sets are in gNvDataSet_Nwk_ID_c.
      The key and SAS secure infromation is in gNvDataSet_App_ID_c.
      The Incomming counter are in gNvDataSet_Sec_ID_c.
    */
    ZdoNwkMng_SaveToNvm(zdoNvmObject_All_c);

    return gZbSuccess_c;
  }

  /*
    On transport nwk key indication we cover preconfigured or not precofigured, in both
    we need to know if the key is all zeros or not.
  */
  zeroKeyStatus = Cmp16BytesToZero(pKeyData->NetworkKey.aNetworkKey);

  /*
    From now and on,we need to access the current security material set.
  */
  pSecurityMaterialSet = SSP_NwkGetSecurityMaterialSet(NlmeGetRequest(gNwkActiveKeySeqNumber_c));

  /*
    If the current securityu material is not available report an error and stop processing.
  */
  if (!pSecurityMaterialSet)
  {
    return gZbFailed_c;
  }

  /*
    The process of a network key during the authentication procedure is different on a
    preconfigured vs a non-preconfigured.
  */
  if (NlmeGetRequest(gNwkPreconfiguredKey_c))
  {
    /*
      If the device is on a preconfigured Nwk key environment and the arriving key is not
      all zeros tehn we are in the wrong network.
    */
    if (!zeroKeyStatus)
    {
      return gZbFailed_c;
    }

    /*
      For a preconfigured key environment we should only register the sequence number.
    */
    pSecurityMaterialSet->keySeqNumber = pKeyData->NetworkKey.keySeqNumber;
    NlmeSetRequest(gNwkActiveKeySeqNumber_c, pKeyData->NetworkKey.keySeqNumber);
  }
  else  /* if (NlmeGetRequest(gNwkPreconfiguredKey_c)) */
  {
    /*
      This means that we are on a non-preconfigured key environment and we got a all zeros
      key, this is an erro we must leave.
    */
    if (zeroKeyStatus)
    {
      return gZbFailed_c;
    }

    /*
      We are a not precofigured key environment, register the incoming data, and, switch
      to it, so that is the cureent and active key data.
    */
    (void)SSP_RegisterKeyData(keyType, pKeyData);
    (void)SSP_NwkSwitchToAlternativeKey(pKeyData->NetworkKey.keySeqNumber);
    NlmeSetRequest(gNwkKeyType_c, keyType);

    /*
      The secure materials sets are in gNvDataSet_Nwk_ID_c.
      The key and SAS secure infromation is in gNvDataSet_App_ID_c.
      The Incomming counter are in gNvDataSet_Sec_ID_c.
    */
    ZdoNwkMng_SaveToNvm(zdoNvmObject_All_c);
  }

  /*
    If we are on a preconfigured environment then register the Trust center data with
    the security material.
  */
  ZDO_SecRegisterTCData();
  RemoveRouteEntry(ApsmeGetRequest(gApsTrustCenterNwkAddress_c));

#if 0
  {
    neighborTable_t  *pNTE;  /* Entry on NT to process. */
   /*Remove route to TC in case an old one is present*/
    RemoveRouteEntry(ApsmeGetRequest(gApsTrustCenterNwkAddress_c));

    /* We care about the IEEE address, the NWK address may change. */
    pNTE = IsInNeighborTable(gZbAddrMode64Bit_c, ApsmeGetRequest(gApsTrustCenterAddress_c));
    
    if (pNTE) {
      if (pNTE->deviceProperty.bits.relationship == gNeighborIsSibling_c) {
            
        /* We dont keep old siblings */
       ResetNeighborTableEntry(pNTE);
      }
    }
  }
#endif

#if gSKKESupported_d
  ZDO_SetState(gZdoDeviceAuthenticationState_c);
#endif

  /*
    Let the ZDO State machine know that got the transport key and we authenticated now.
  */
  if (keyType == gStandardNetworkKey_c)
  {
    ZDO_SendEvent(gZdoEvent_AuthenticationSuccess_c);
  }


  /*
    In case that the received key type is for high security, then, iniciate the Entity
    Authentication.
  */
  if(keyType == gHighSecurityNetworkKey_c)
  {
    if(!NlmeGetRequest(gNwkPreconfiguredKey_c))
    {
      /*
        This procedure is mandatory for high security.
      */
      (void)ZDO_APSME_Authenticate_request(NlmeGetRequest(gParentLongAddress_c), gINITIATE_c, NULL);
    }
    else
    {
      ZDO_SendEvent(gZdoEvent_AuthenticationSuccess_c);
    }
  }
#else
  (void)keyType;
  (void)pKeyData;
#endif
  return gZbSuccess_c;
}

/************************************************************************************
* Generates a network leave (NLME) using the address on the remove device indication.
*
* Interface assumptions:
*   The parameter pRemoveDeviceInd is not a null pointer.
*
* Return value:
*   Teh status for this operation.
*
* Revison history:
*   Date   Author  Comments
*  ------  ------  --------
*  060208    MN    Created
************************************************************************************/
zbStatus_t ZDO_SecProcessRemoveDeviceInd
(
  zbApsmeRemoveDeviceInd_t  *pRemoveDeviceInd
)
{
#if !gTrustCenter_d || gComboDeviceCapability_d
  zdoNlmeMessage_t *pZdoNlmeMsg;
  neighborTable_t  *pNeighborTableEntry;

  /*
    The size for the current command to be passed down.
  */
  uint8_t size = (sizeof(MbrSizeof(zdoNlmeMessage_t, msgType)) + sizeof(nlmeLeaveReq_t));

#if gComboDeviceCapability_d
  if (gTrustCenter)
  {
    return gZbSuccess_c;
  }
#endif

  pZdoNlmeMsg = MSG_Alloc(size);

  if (!pZdoNlmeMsg)
  {
    return gZbFailed_c;
  }

  BeeUtilZeroMemory(pZdoNlmeMsg, size);

  pZdoNlmeMsg->msgType = gNlmeLeaveRequest_c;

  Copy8Bytes(pZdoNlmeMsg->msgData.leaveReq.aDeviceAddress, pRemoveDeviceInd->aChildAddress);

  pNeighborTableEntry = IsInNeighborTable(gZbAddrMode64Bit_c, pRemoveDeviceInd->aChildAddress);

  if (pNeighborTableEntry)
  {
    /*
      The normal parameters change when the device is one our own children.
    */
    if (pNeighborTableEntry->deviceProperty.bits.relationship == gUnAuthenticatedChild_c)
    {
      pZdoNlmeMsg->msgData.leaveReq.silent = TRUE;
      pZdoNlmeMsg->msgData.leaveReq.reuseAddress = TRUE;
    }
  }

  if (ZDO_NLME_SapHandler(pZdoNlmeMsg))
  {
    /* Catch error here.! */
  }
#else
  (void)pRemoveDeviceInd;
#endif
  return gZbSuccess_c;
}


/* endif gStandardSecurity_d */
#endif

#if gHighSecurity_d

/*******************************************************************************
* ZDO_SecProcessAuthenticateInd
*
* Upon receiving the APSME-AUTHENTICATE.indication primitive, the ZDO
* will determine whether it wishes to participate in entity authentication with 
* the initiator specified by the InitiatorAddress parameter. If it does, the 
* ZDO will respond using the APSME-AUTHENTICATE.request primitive, setting the
* Action parameter to RESPOND_ACCEPT and the corresponding RandomChallenge 
* parameter in the APSME-AUTHENTICATE.request primitive
* to the RandomChallenge parameter. If it does not wish to participate in entity
* authentication with the initiator, the ZDO will respond using the APSMEAUTHENTICATE.
* request primitive, setting the Action parameter to
* RESPOND_REJECT. 053474r17ZB section 4.4.8.2.3
*
* Interface assumptions:
*   The parameter pAuthenticateInd is not a null pointer.
*
* Return value:
*   NONE.
*
* Revison history:
*   Date   Author  Comments
*  ------  ------  --------
*  180408    BN    Created
*******************************************************************************/
zbStatus_t ZDO_SecProcessAuthenticateInd
(
  zbApsmeAuthenticateInd_t  *pAuthenticateInd
)
{
  zbAction_t  action = gRESPOND_ACCEPT_c;
  zbAESKey_t        aRandomChallenge;
  zbStatus_t        status;
  uint8_t           size;
  neighborTable_t   *pNTE;

  size = sizeof(aRandomChallenge);
  /*Set the aRandomChallenge to Zeros, to avoid future trash.*/
  BeeUtilZeroMemory(aRandomChallenge,size);

  /* Check if the Random challenge sent by the initiator is accepted acording to
     the procedure 053474r17ZB section B3.2. */
  status = SSP_ChallengeIsAccepted(size);
  
  /* If the challenge is not accepted then the response is to reject the challenge*/
  if (!status)
  {
    action = gRESPOND_REJECT_c;
  }

  pNTE = IsInNeighborTable(gZbAddrMode64Bit_c, pAuthenticateInd->aInitiatorAddress);

  if (!pNTE)
  {
    action = gRESPOND_REJECT_c;
  }

  /* The aRandomChallenge for the response must be copied from the one in the 
    indication.*/
  Copy16Bytes(aRandomChallenge, pAuthenticateInd->aRandomChanllenge);

  /*Generate the Authenticate request to respond to the Authenticate Indication 
    and sent it to the lower layers.*/
  status = ZDO_APSME_Authenticate_request( pAuthenticateInd->aInitiatorAddress, 
                                           action,aRandomChallenge);

  if (status)
  {
    /* Catch error here, in case there is one. */
  }
  return status;
}

/*******************************************************************************
* ZDO_SecProcessAuthenticateCnf
* 
* Upon receiving the APSME-AUTHENTICATE.confirm primitive, the ZDO of the 
* initiator is notified of the result of its request to authenticate the responder.
* If the transfer was successful, the Status parameter will be set to SUCCESS 
* and the ZDO will learn that it shares a key with the responder and that it has
* received authenticated data from the responder. Otherwise, the Status parameter 
* will indicate the error.
* The ZDO of the responder is notified of the result of the transfer of 
* authenticated data from the initiator. If the transfer was successful, the 
* Status parameter will be set to SUCCESS and the ZDO will learn that it shares 
* a key with the initiator and that it has received authenticated data from the 
* initiator. Otherwise, the Status parameter will indicate the error.
*
* Interface assumptions:
*   The parameter pAuthenticateCnf is not a null pointer.
*
* Return value:
*   NONE.
*
* Revison history:
*   Date   Author  Comments
*  ------  ------  --------
*  180408    BN    Created
*******************************************************************************/
zbStatus_t ZDO_SecProcessAuthenticateCnf
(
  zbApsmeCnf_t  *pAuthenticateCnf
)
{
  zbEntityAuthentication_t   *pEntityAuthentication_Data;
  zdoNlmeMessage_t           *pZdoNlmeMsg;
  addrMapIndex_t i;

  pEntityAuthentication_Data = SSP_ApsRetriveEntityAuthenticationStateMachineEntry(pAuthenticateCnf->aAddress);

  if (!pEntityAuthentication_Data)
  {
    return gEA_INVALID_PARAMETER_c;
  }

  TMR_StopTimer(pEntityAuthentication_Data->EA_TimerID);
  TMR_FreeTimer(pEntityAuthentication_Data->EA_TimerID);

  /*
    This event it is just sent when the authentication process was at a joining process.
    Need to add the proper flags to know whe it is joinig or just an authentication process.
  */
  if (pAuthenticateCnf->status == gZbSuccess_c)
  {
    if (ZDO_GetState() == gZdoDeviceAuthenticationState_c)
    {
      ZDO_SendEvent(gZdoEvent_AuthenticationSuccess_c);
    }
    else if (!pEntityAuthentication_Data->isInitiator)
    {
      SSP_NwkSetRelationship(pAuthenticateCnf->aAddress, gNeighborIsChild_c);
    }

    i = APS_FindIeeeInAddressMap(pAuthenticateCnf->aAddress);
    if (i != gNotInAddressMap_c)
    {
      APS_MarkAddrMapEntryPermanent(i, TRUE);
    }
  }
  else
  {
    /*Add the proper code for the error cases.*/
  }

  /*
    Report to the Nwk layer how the authentication process with the partner addres
    went.
  */
  pZdoNlmeMsg= MSG_Alloc(sizeof(nlmeAuthenticationCnf_t) + sizeof(zbMsgId_t));

  if(pZdoNlmeMsg != NULL)
  {
    pZdoNlmeMsg->msgType = gApsmeAuthenticateCnf_c;
    Copy16Bytes(pZdoNlmeMsg->msgData.AuthenticationCnf.aAddress, pAuthenticateCnf->aAddress);
    pZdoNlmeMsg->msgData.AuthenticationCnf.status = pAuthenticateCnf->status;

    (void)ZDO_NLME_SapHandler(pZdoNlmeMsg);
  }

  /*
    No matter the status obtained in the confirm we nned to free the pEA_Material
    since the Entity Authentication process has ended for the device in the 
    incomming confirm.
  */
  SSP_ApsRemoveEntityAuthenticationStateMachineEntry(pAuthenticateCnf->aAddress);

  if (pAuthenticateCnf->status == gZbSuccess_c)
  {
    /* For authentication the counters and the neighbor table got modified so need tobe save. */
    ZdoNwkMng_SaveToNvm(zdoNvmObject_SecureSet_c);
    ZdoNwkMng_SaveToNvm(zdoNvmObject_NwkData_c);
  }

  return (pAuthenticateCnf->status);
}

#endif

/************************************************************************************
* Reset the APS security materila to get the defualts.
*
* On a trust center, establish the predefined key (Master or Application) into each
* each entry of the security material set, given the possibility to every joining
* device the chance to share at least the key needed to communicate with the trust center.
* On a regular device (Non trust center), Stores the TC information into the entry zero
* of the set.
*
* Interface assumptions:
*   NONE.
*
* Return value:
*   NONE.
*
* Revison history:
*   Date   Author  Comments
*  ------  ------  --------
*  210408    MN    Created
************************************************************************************/
void SSP_ApsResetSecurityMaterial
(
  void
)
{
#if gApsLinkKeySecurity_d
  uint8_t  *pKey = NULL;
  zbKeyType_t keyType = gTrustCenterRandomMasterkey_c;
  zbAESKey_t aTempKey;

  /* When link keys is active we need the initialization early, before the nwk layer. */
#if gComboDeviceCapability_d
  ZDO_SecSetTrustCenterOn();
#endif

#if gSKKESupported_d
  /* If the trust center master key is non zero key then is preconfigured TC master key. */
  if (!Cmp16BytesToZero(ApsmeGetRequest(gApsTrustCenterMasterKey_c)))
  {
    pKey = (uint8_t *)ApsmeGetRequest(gApsTrustCenterMasterKey_c);
    keyType = gTrustCenterMasterKey_c;
  }
#endif

  /* If the trust center link key is different than  */
  if (!Cmp16BytesToZero(ApsmeGetRequest(gApsTrustCenterLinkKey_c)))
  {
    pKey = (uint8_t *)ApsmeGetRequest(gApsTrustCenterLinkKey_c);
    keyType = gTrustCenterLinkKey_c;
  }

  /* If non of the keys were pre-configured then establish a key no matter what. */
  if (!pKey)
  {
#if gTrustCenter_d || gComboDeviceCapability_d
#if gComboDeviceCapability_d
    if (gTrustCenter)
#endif
    {
      /*
        If the device is the trust center, generate a random key tobe used as the TC
        master key.
      */
      SSP_ChallengeGenPrimitive(aTempKey);
    }
#endif

#if !gTrustCenter_d || gComboDeviceCapability_d
#if gComboDeviceCapability_d
    if (!gTrustCenter)
#endif
    {
      /* Any node that is not a trust center will celar the key to Zero. */
      BeeUtilZeroMemory(aTempKey, sizeof(zbAESKey_t));
    }
#endif
    pKey = (uint8_t *)aTempKey;
    keyType = gTrustCenterRandomMasterkey_c;
  }

  /* At this point we have a key and we can set the rest of the security material. */
  if (keyType == gTrustCenterRandomMasterkey_c)
  {
    ApsmeSetRequest(gApsTrustCenterMasterKey_c, pKey);
  }

  /* No matter what register the key type.! */
  ApsmeSetRequest(gApsDefaultTCKeyType_c, keyType);

  /* Clear the secure material. We depend on the fact that is all zeros. */
  DevKeyPairSet_InitTable();
  /* clear the key table*/
  (void)KeySet_InitTable();
  /* Access the key Table */
  (void)KeySet_SetTableEntry(0 , (zbAESKey_t *)pKey);



#if !gTrustCenter_d || gComboDeviceCapability_d
  /*
    By default we asume that the trust center address is the coordinator, until the application
    starts any discovery mechanism.
  */
#if gComboDeviceCapability_d
  if (!gTrustCenter)
#endif
  {
    Set2Bytes(ApsmeGetRequest(gApsTrustCenterNwkAddress_c), 0x0000);
  }
#endif

#if gTrustCenter_d || gComboDeviceCapability_d
#if gComboDeviceCapability_d
  if (gTrustCenter)
#endif
  {
    ApsmeSetRequest(gApsTrustCenterAddress_c, NlmeGetRequest(gNwkIeeeAddress_c));
    ApsmeSetRequest(gApsTrustCenterNwkAddress_c, NlmeGetRequest(gNwkShortAddress_c));
  }
#endif

  /* Reset the SKKE Statemachine to start form scratch all fresh and clean. */
#if gSKKESupported_d
  SSP_ApsResetSKKEStateMachine();
#endif

#endif

#if gHighSecurity_d
  SSP_ApsResetEntityAuthenticationStateMachine();
#endif

#if gHighSecurity_d || gApsLinkKeySecurity_d
  /* Establish the time out for the security material. */
  ApsmeSetRequest(gApsSecurityTimeOutPeriod_c, mDefaultApsSecurityTimeOutPeriod_c);
#endif

}

#if gStandardSecurity_d || gHighSecurity_d
/************************************************************************************
* After restoring from NVM, increase the outgoing frame counters to avoid loosing
* The comunication with the neighbors.
*
* Interface assumptions:
*   NONE.
*
* Return value:
*   NONE.
************************************************************************************/
void ZDO_SecUpdateSecureCounters
(
  void
)
{
  zbNwkSecurityMaterialSet_t  *pSecurityMaterial;

  pSecurityMaterial = SSP_NwkGetSecurityMaterialSet(NlmeGetRequest(gNwkActiveKeySeqNumber_c));

  /* Only update if the current active sequence number is a valid one. */
  if (pSecurityMaterial)
  {
    /* Nwk outgoing counter needs to be increased to avoid get out of sync. */
    pSecurityMaterial->outgoingFrameCounter += gNvIncrementAfterRestore_c;
  }

  /* Each APS outgoing counter needs update too. */
  ZDO_SecUpdateAPSCounters();
}

#if gApsLinkKeySecurity_d
void ZDO_SecUpdateAPSCounters(void)
{
  uint8_t i;  /* Loop counter. */
  zbApsDeviceKeyPairSet_t devKeyPairSetEntry;

  /* Start from the beggining and walk the whole list */

  /* AMke sure to update each entry of the array. */
  for (i = 0; i < ApsmeGetRequest(gApsDeviceKeyPairCount_c); i++)
  {
    (void)DevKeyPairSet_GetTableEntry(i, &devKeyPairSetEntry);
    /* Skip the unused ones if theres any. */
    if (devKeyPairSetEntry.iDeviceAddress == gNotInAddressMap_c)
    {
      continue;
    }

    /* Keep counter ahead. */
    devKeyPairSetEntry.OutgoingFrameCounter += gNvIncrementAfterRestore_c;
    (void)DevKeyPairSet_SetTableEntry(i, &devKeyPairSetEntry);
  }
}
#endif

#endif

#if gStandardSecurity_d || gHighSecurity_d
/************************************************************************************
* Given a device IEEE address get the key type pre configured into the device's
* secutiy material.
* If APS link keys are off, then it will return the current key type for the newk key.
* If APS link is On, it will return the type of the current link key.
*
* Interface assumptions:
*   The paramneter aDeviceAddress is a valid 64-bit address.
*
* Return value:
*   NONE.
*
************************************************************************************/
zbKeyType_t ZDO_SecGetDeviceKeyType
(
  zbIeeeAddr_t aDeviceAddress
)
{
#if gApsLinkKeySecurity_d
  uint8_t  index;
  zbApsDeviceKeyPairSet_t devKeyPairSetEntry;

  index = SSP_ApsGetIndexFromIEEEaddress(aDeviceAddress);

  if (index == gNotInAddressMap_c)
  {
    return (index);
  }

  (void)DevKeyPairSet_GetTableEntry(index, &devKeyPairSetEntry);

  return (devKeyPairSetEntry.keyType);
#else
  zbNwkSecurityMaterialSet_t *pSecurityMaterial;
  pSecurityMaterial = SSP_NwkGetSecurityMaterialSet(NlmeGetRequest(gNwkActiveKeySeqNumber_c));

  if (!pSecurityMaterial)
  {
    return (gNotInAddressMap_c);
  }

  (void)aDeviceAddress;
  return (pSecurityMaterial->keyType);
#endif
}
#endif

#if gStandardSecurity_d || gHighSecurity_d
void ZDO_SecRegisterTCData(void)
{
#if (!gTrustCenter_d || gComboDeviceCapability_d) && gApsLinkKeySecurity_d

#if gComboDeviceCapability_d
  if (gTrustCenter)
  {
    return;
  }
#endif
  /*
    For pre-configured link keys only.
  */
  if (ApsmeGetRequest(gApsDefaultTCKeyType_c) == gTrustCenterLinkKey_c)
  {
    zbAESKey_t aAESkey;
    zbApsDeviceKeyPairSet_t devKeyPairSetEntry;
    (void)APS_AddToAddressMapPermanent(ApsmeGetRequest(gApsTrustCenterAddress_c), ApsmeGetRequest(gApsTrustCenterNwkAddress_c));
    (void)DevKeyPairSet_GetTableEntry(0, &devKeyPairSetEntry);
    (void)KeySet_GetTableEntry(devKeyPairSetEntry.iKey, &aAESkey);
    APS_RegisterLinkKeyData(ApsmeGetRequest(gApsTrustCenterAddress_c), gTrustCenterLinkKey_c, aAESkey);
  }
#endif
}
#endif

#if gStandardSecurity_d || gHighSecurity_d
void ZDO_SecSetTrustCenterOn(void)
{
#if MC13226Included_d
  gTrustCenter = FALSE;
#else /* MC13226Included_d */
#if gComboDeviceCapability_d
  gTrustCenter = FALSE;
#endif  /* gComboDeviceCapability_d  */
#endif  /* MC13226Included_d  */
  if (NlmeGetRequest(gDevType_c) == gCoordinator_c)
  {
#if MC13226Included_d
   gTrustCenter = TRUE;
#else /* MC13226Included_d */
#if gComboDeviceCapability_d
   gTrustCenter = TRUE;
#endif  /* gComboDeviceCapability_d  */
#endif  /* MC13226Included_d  */
   ZdoSetPrimaryTrustCenterOn(gaServerMask); /* Trust center bit enable. */ /* Trust center bit enable. */
  }
}
#endif

#if gStandardSecurity_d || gHighSecurity_d
bool_t ZdoSec_NwkCheckSequenceNumber(zbKeySeqNumber_t iCurrent, zbKeySeqNumber_t iIncoming)
{
  uint8_t  in = (uint8_t)iIncoming; /* The incoming sequence number should be treated as an unsigned number. */
  int8_t  curr = (int8_t)iCurrent;  /* The current sequence number should be treated as a signed number. */
  int16_t diff;

  /* Verify that the difference between the sequence numbers is not to big. */
  diff = (int16_t)(in - curr);

  /* See if the difference is no bigger than 127 (unsigned) the half of the size of the current type. */
  if (diff > 0x7F)
  {
    return FALSE;  /* Reject the incoming key */
  }

  return TRUE;  /* Accept the incoming key */
}
#endif

/***********************************************************************************
* If the process of the Incoming or outcoming frame in the secure module has a
* status different then success wqe report that to the application.
*
* Interface Assumptions:
*   The parameter 'securityStatus' is one of the possible status code used in the
*   secure module:
*      gZbSuccess_c       = 0x00
*      gZbSecurityFail_c  = 0xa2
*      gSSP_SecurityOff_c = 0x06
*
* Return value:
*   NONE.
*
* NOTE: Assume that zbMsgId_t and zbStatus_t are one byte each.
************************************************************************************/
void SSP_SecProcessSecureFrameConfirm
(
  uint8_t     msgId,
  zbStatus_t  securityStatus  /* IN: the confirm status code. */
)
{
#if gStandardSecurity_d || gHighSecurity_d
  uint8_t *pMsg;

  pMsg = MSG_Alloc(sizeof(msgId) + sizeof(securityStatus));

  if (!pMsg)
  {
    return;
  }

  pMsg[0] = msgId;
  pMsg[1] = securityStatus;

  /* If the message was sent with success or not we don't care */
  if (msgId == gNwkProcessFrameConf_c)
  {
    (void)NLME_ZDO_SapHandler((void *)pMsg);
  }
  else if (msgId == gApsmeProcessSecureFrameConf_c)
  {
    (void)APSME_ZDO_SapHandler((void *)pMsg);
  }
#else
(void)msgId;
(void)securityStatus;
#endif
}


/************************************************************************************
* Takes any APSME message send to ZDO and pass it up to the application, does not process
* it or analize it just apss it.
*
* Interface assumptions:
*   The parameter pApsmeInd is not a null pointer.
*
* Return value:
*   NONE.
*
* Revison history:
*   Date   Author  Comments
*  ------  ------  --------
*  060408    MN    Created
************************************************************************************/
void ZDO_SecSendIndicationToApp
(
  void *pApsmeInd
)
{
  zbApsmeZdoIndication_t  *pAppInd;

  pAppInd = MSG_Alloc(sizeof(zbApsmeZdoIndication_t));

  if (!pAppInd)
    return;

  FLib_MemCpy(pAppInd, pApsmeInd, sizeof(zbApsmeZdoIndication_t));

  /*
    We do not pay attention to any error, for now.!!
  */
  (void)ZDP_APP_SapHandler((void *)pAppInd);
}

/*****************************************************************************/

/*******************************************************************************
********************************************************************************
* Private Debug Stuff
********************************************************************************
*******************************************************************************/

/* None */ 
