/*****************************************************************************
* Header file for Zdo Security Manager
*
*
* (c) Copyright 2005, Freescale, Inc. All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
******************************************************************************/
#ifndef _ZDO_SECURITY_MANAGER_
#define _ZDO_SECURITY_MANAGER_


/**********************************************************************
***********************************************************************
* Public Macros
***********************************************************************
***********************************************************************/

/*
  Updevice device status values.
  Name: Status
  Type: Integer
  Range: 0x00 - 0x07
  Description: Indicates the updated status of the device given by the
               DeviceAddress parameter.
*/
#define gStandardDeviceSecuredReJoin_c        0x00
#define gStandardDeviceUnsecuredJoin_c        0x01
#define gDeviceLeft_c                         0x02
#define gStandardDeviceUnsecuredRejoin_c      0x03
#define gHighSecurityDeviceSecuredReJoin_c    0x04
#define gHighSecurityDeviceUnsecuredJoin_c    0x05
#define gReserved_c                           0x06
#define gHighSecurityDeviceUnsecuredRejoin_c  0x07


/**********************************************************************
***********************************************************************
* Public Prototypes
***********************************************************************
***********************************************************************/

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
);


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
);
#if gApsLinkKeySecurity_d
void ZDO_SecUpdateAPSCounters(void);
#else
#define ZDO_SecUpdateAPSCounters()
#endif

#else
#define ZDO_SecUpdateSecureCounters()
#define ZDO_SecUpdateAPSCounters()
#endif

#if gStandardSecurity_d || gHighSecurity_d
void ZDO_SecClearDeviceData(zbNwkAddr_t aNwkAddr, zbIeeeAddr_t aIEEEAddr, bool_t fullErase);
#else
#define ZDO_SecClearDeviceData(aNwkAddr, aIEEEAddr, fullErase)
#endif


/**********************************************************************
***********************************************************************
* Public type definitions
***********************************************************************
***********************************************************************/

 /* None */

/**********************************************************************
***********************************************************************
* Public Memory Declarations
***********************************************************************
***********************************************************************/



extern uint8_t gaDevRequestedToLeave[8];


/******************************************************************************
*******************************************************************************
* Public functions
*******************************************************************************
******************************************************************************/

#if gStandardSecurity_d || gHighSecurity_d
/************************************************************************************
* Sets the given key data into the inactive security material set, will work for
* APS and Nwk.
*
* Interface assumptions:
*   NONE.
*
* Return value:
*   Success if the register action was completed, Failed otherwise.
************************************************************************************/
extern zbStatus_t  SSP_RegisterKeyData
(
  zbKeyType_t  keyType,
  zbTransportKeyData_t  *pKeyInfo
);

/************************************************************************************
* Gets the current and active Nwk Security Material set.
*
* Interface assumptions:
*   NONE.
*
* Return value:
*   The address in memory where the current and active security material set is
*   located or NULL if can not be obtained.
*
* Revison history:
*   Date   Author  Comments
*  ------  ------  --------
*  060208    MN    Created
************************************************************************************/
extern zbNwkSecurityMaterialSet_t *SSP_NwkGetSecurityMaterialSet
(
  zbNwkActiveKeySeqNumber_t  KeySequenceNumber
);


/************************************************************************************
* Switch to a different security material set if the given sequence number is
* newer than the current one. And it is already registered
*
* Interface assumptions:
*   The parameter sequenceNumber is a number between 0x00 and 0xFF, And it was previously
*   regitered.
*
* Return value:
*   NONE.
*
* Revison history:
*   Date   Author  Comments
*  ------  ------  --------
*  120208    MN    Created
************************************************************************************/
extern zbStatus_t SSP_NwkSwitchToAlternativeKey
(
  zbKeySeqNumber_t sequenceNumber
);

#endif

#if (gStandardSecurity_d || gHighSecurity_d) && (gTrustCenter_d || gComboDeviceCapability_d) && gDefaultValueOfMaxEntriesForExclusionTable_c
/*
  This function adds an IEEE address to the exclusion List
*/
bool_t Zdo_AddToExclusionList
(
	uint8_t *pIeeeAddress
);

/*
  This fucntion Removes a device from the exclusion list using the IEEE address.
*/
bool_t Zdo_RemoveFromExclusionList
(
  uint8_t *pIeeeAddress
);
#endif

void ZDO_APSME_Update_Device_request
(
  zbIeeeAddr_t  aExtendedAddress,
  zbNwkAddr_t   aShortAddress,
  uint8_t       rejoin,
  bool_t        secureJoin
);


/************************************************************************************
* Given the 64-bit addres of a dievice serach for it in the NT, if it find it this
* means that the leaving device used to have direct communication with us and we must
* clear the incoming counter as soon as possible.
*
* Interface assumptions:
*   The parameters aDeviceAddr is a valid 64-bit address.
*   The function will search in the Acive material set.
*
* Return value:
*   NONE.
*
* Revison history:
*   Date   Author  Comments
*  ------  ------  --------
*  260308    MN    Created
************************************************************************************/
#if gStandardSecurity_d || gHighSecurity_d
extern void SSP_NwkResetDeviceFrameCounter
(
  zbIeeeAddr_t  aDeviceAddr  /* IN: The 64-bit address of the device to clear off. */
);
#endif


#if gStandardSecurity_d || gHighSecurity_d
zbStatus_t ZDO_SecProcessApsmeIndication
(
  zbApsmeZdoIndication_t *pApsmeInd
);
#endif


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
#if gStandardSecurity_d || gHighSecurity_d
zbKeyType_t ZDO_SecGetDeviceKeyType
(
  zbIeeeAddr_t aDeviceAddress
);
#else
#define ZDO_SecGetDeviceKeyType(aDeviceAddress)  gStandardNetworkKey_c
#endif



/*****************************************************************************************
       NEW SET OF ZDO SECURITY FUNCTIONS, REPLACE THE ONES ABOVE AFTER TESTING
*****************************************************************************************/
#if gStandardSecurity_d || gHighSecurity_d
#if gTrustCenter_d || gComboDeviceCapability_d
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
*
* Revison history:
*   Date   Author  Comments
*  ------  ------  --------
*  140208    MN    Created
************************************************************************************/
void ZDO_SecProcessUpdateDeviceIndication
(
  zbApsmeUpdateDeviceInd_t  *pUpdateIndication
);

void ZDO_SecSendTransportKey
(
  void *pNodeInfo,
  bool_t isChild
);

#if gApsLinkKeySecurity_d
void ZDO_SecSendTCMasterKey
(
  void *pNodeInfo,
  bool_t isChild
);
#else
#define ZDO_SecSendTCMasterKey(pNodeInfo, isChild)
#endif

#else
#define ZDO_SecProcessUpdateDeviceIndication(pUpdateIndication)
#define ZDO_SecSendTransportKey(pNodeInfo, isChild)
#endif
#endif

/*
  IsInNeighborTable
  This routine searches an specific IEEE Address in the neighbor table, if it is found
  return the pointer to the entry related to that address, if not found return NULL
*/
#if gStandardSecurity_d || gHighSecurity_d
extern neighborTable_t* IsInNeighborTable
(
  zbAddrMode_t addressMode,
  uint8_t *pAddress		/* IN:  */
);
#endif

#if gStandardSecurity_d || gHighSecurity_d
void ZDO_SecSetTrustCenterOn(void);
#endif


#endif
