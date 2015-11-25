/******************************************************************************
* Ztc access to the NWK/APS Information Base
*
* Copyright (c) 2008, Freescale, Inc.  All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
******************************************************************************/

#include "EmbeddedTypes.h"
#if gBeeStackIncluded_d
#include "ZigBee.h"
#include "BeeStackFunctionality.h"
#include "BeeStackConfiguration.h"
#endif

#include "ZtcInterface.h"

#if gZtcIncluded_d

#include "FunctionLib.h"
#if gBeeStackIncluded_d
#include "BeeStackInterface.h"
#include "BeeStack_Globals.h"

#include "AfApsInterface.h"
#include "ZdoApsInterface.h"
#include "ApsMgmtInterface.h"
#include "BeeStackParameters.h"
#endif

#include "ZtcPrivate.h"
#include "ZtcClientCommunication.h"
#include "ZtcIBManagement.h"

/******************************************************************************
*******************************************************************************
* Private macros
*******************************************************************************
******************************************************************************/

/* determine which table to search */
#define mRomTable_c 0x00
#define mRamTable_c 0x01

/******************************************************************************
*******************************************************************************
* Private prototypes
*******************************************************************************
******************************************************************************/

#if gBeeStackIncluded_d

ztcIBData_t const *ZtcIBLookupEntry(zbIBAttributeId_t iId);

/******************************************************************************
*******************************************************************************
* Private type definitions
*******************************************************************************
******************************************************************************/

typedef uint8_t (*pZtcIBFunc_t) (uint8_t index, void *pData);

typedef struct ztcIBAccessTbl_tag
{
 zbIBAttributeId_t id;
 pZtcIBFunc_t pSetTblEntry;
 pZtcIBFunc_t pGetTblEntry; 
}ztcIBAccessTbl_t;

/******************************************************************************
*******************************************************************************
* Public memory definitions
*******************************************************************************
******************************************************************************/

//extern  uint8_t gDeviceType;

/******************************************************************************
*******************************************************************************
* Private memory declarations
*******************************************************************************
******************************************************************************/
/* Table of functions to access the IB tables; */
ztcIBAccessTbl_t const maZtcIBAccessFuncTbl[] =
{
 { gApsAddressMap_c, AddrMap_SetTableEntry, AddrMap_GetTableEntry }
#if gApsLinkKeySecurity_d 
 ,{ gApsDeviceKeyPairSet_c, DevKeyPairSet_SetTableEntry, DevKeyPairSet_GetTableEntry}
#endif
};

ztcIBData_t const maZtcIBData[] = {
  /* Network Information Base Identifiers. r17 - Table 3.41 NWK IB Attributes. */
  {gNwkSequenceNumber_c,                    mZtcIBRO, sizeof(gNibSequenceNumber),                    mIBScalar,                              (void *) &gNibSequenceNumber},
  {gNwkPassiveAckTimeout_c,                 mZtcIBRO, sizeof(gNibPassiveAckTimeout),                 mIBScalar,                              (void *) &gNibPassiveAckTimeout},
  {gNwkMaxBroadcastRetries_c,               mZtcIBRO, sizeof(gNibMaxBroadcastRetries),               mIBScalar,                              (void *) &gNibMaxBroadcastRetries},
  {gNwkMaxChildren_c,                       mZtcIBRO, sizeof(gNibMaxChildren),                       mIBScalar,                              (void *) &gNibMaxChildren},
  {gNwkMaxDepth_c,                          mZtcIBRO, sizeof(gNibMaxDepth),                          mIBScalar,                              (void *) &gNibMaxDepth},
  {gNwkMaxRouter_c,                         mZtcIBRO, sizeof(gNibMaxRouter),                         mIBScalar,                              (void *) &gNibMaxRouter},
  {gNwkNetworkBroadcastDeliveryTime_c,      mZtcIBRO, sizeof(gNibNetworkBroadcastDeliveryTime),      mIBScalar,                              (void *) &gNibNetworkBroadcastDeliveryTime},
  {gNwkReportConstantCost_c,                mZtcIBRO, sizeof(gNibReportConstantCost),                mIBScalar,                              (void *) &gNibReportConstantCost},
  {gNwkRouteDiscoveryRetriesPermitted_c,    mZtcIBRO, sizeof(gNibRouteDiscoveryRetriesPermitted),    mIBScalar,                              (void *) &gNibRouteDiscoveryRetriesPermitted},
  {gNwkSymLink_c,                           mZtcIBRO, sizeof(gNibSymLink),                           mIBScalar,                              (void *) &gNibSymLink},
  {gNwkCapabilityInformation_c,             mZtcIBRW, sizeof(NlmeGetRequest(gNwkCapabilityInformation_c)), mIBScalar,                        (void *) &NlmeGetRequest(gNwkCapabilityInformation_c)},
  {gNwkAddrAlloc_c,                         mZtcIBRO, sizeof(gNibAddrAlloc),                         mIBScalar,                              (void *) &gNibAddrAlloc},
  {gNwkUseTreeRouting_c,                    mZtcIBRO, sizeof(gNibUseTreeRouting),                    mIBScalar,                              (void *) &gNibUseTreeRouting},
  {gNwkManagerAddr_c,                       mZtcIBRW, sizeof(zbNwkAddr_t),                           mIBScalar,                              NlmeGetRequest(gNwkManagerAddr_c)},
  {gNwkTransactionPersistenceTime_c,        mZtcIBRO, sizeof(gNibTransactionPersistenceTime),        mIBScalar,                              (void *) &gNibTransactionPersistenceTime},

/* 
  053474r17 Zigbee Spec.
  Table #3.44  NIB Attributes
  Read-Only in ZigBee, Read-Write in ZigBee Pro 
*/
#if gZigbeeProIncluded_d
  {gNwkShortAddress_c,                      mZtcIBRW, sizeof(NlmeGetRequest(gNwkShortAddress_c)),    mIBScalar,                              (void *) NlmeGetRequest(gNwkShortAddress_c)},
#else
  {gNwkShortAddress_c,                      mZtcIBRW, sizeof(NlmeGetRequest(gNwkShortAddress_c)),    mIBScalar,                              (void *) NlmeGetRequest(gNwkShortAddress_c)},
#endif
  
  {gNwkStackProfile_c,                      mZtcIBRO, sizeof(gSAS_Ram.stackProfile),                 mIBScalar,                              (void *) &gSAS_Ram.stackProfile},
  {gNwkProtocolVersion_c,                   mZtcIBRO, sizeof(gSAS_Ram.protocolVersion),              mIBScalar,                              (void *) &gSAS_Ram.protocolVersion},
  {gNwkExtendedPanId_c,                     mZtcIBRW, sizeof(NlmeGetRequest(gNwkExtendedPanId_c)),   mIBScalar,                              (void *) NlmeGetRequest(gNwkExtendedPanId_c)},

#if (gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d)
#if gRnplusCapability_d
  {gNwkRouteTable_c,                        mZtcIBRW, sizeof(routeTable_t),                          gNwkInfobaseMaxRouteTableEntry_c,       ((void *) gaRouteTable)},
#endif
  {gNwkNeighborTable_c,                     mZtcIBRW, sizeof(neighborTable_t),                       gNwkInfobaseMaxNeighborTableEntry_c,    ((void *) gaNeighborTable)},
#if gNwkSymLinkCapability_d
  {gNwkLinkStatusPeriod_c,                  mZtcIBRW, sizeof(NlmeGetRequest(gNwkLinkStatusPeriod_c)),               mIBScalar,                              (void *) &NlmeGetRequest(gNwkLinkStatusPeriod_c)},
#endif
#endif

    /* BeeStack implementation attributes */
  {gNwkPanId_c,                             mZtcIBRW, sizeof(gSAS_Ram.aPanId),                       mIBScalar,                              (void *) gSAS_Ram.aPanId},
  {gDevType_c,                              mZtcIBRO, sizeof(gNwkData.deviceType),                   mIBScalar,                              (void *) &gNwkData.deviceType},
  {gSoftwareVersion_c,                      mZtcIBRO, sizeof(gaSoftwareVersion),                     mIBScalar,                              (void *) &gaSoftwareVersion},

#if (gEndDevCapability_d || gComboDeviceCapability_d )
  {gNwkIndirectPollRate_c,                  mZtcIBRW, sizeof(gSAS_Ram.aIndirectPollRate),            mIBScalar,                              (void *) gSAS_Ram.aIndirectPollRate},
  {gConfirmationPollTimeOut_c,              mZtcIBRW, sizeof(gBeeStackConfig.gConfirmationPollTimeOut), mIBScalar,                           (void *) &gBeeStackConfig.gConfirmationPollTimeOut},
#endif

#if gStandardSecurity_d || gHighSecurity_d
  {gNwkSecurityLevel_c,                     mZtcIBRW, sizeof(NlmeGetRequest(gNwkSecurityLevel_c)),            mIBScalar,           (void *) &NlmeGetRequest(gNwkSecurityLevel_c)},
  {gNwkSecurityMaterialSet_c,               mZtcIBRW, sizeof(NlmeGetRequest(gNwkSecurityMaterialSet_c)),      2,                   (void *) NlmeGetRequest(gNwkSecurityMaterialSet_c)},
  {gSASNwkKey_c,                            mZtcIBRW, sizeof(zbAESKey_t),                                     mIBScalar,           (void *) gSAS_Ram.aNetworkKey},
  {gNwkActiveKeySeqNumber_c,                mZtcIBRW, sizeof(NlmeGetRequest(gNwkActiveKeySeqNumber_c)),       mIBScalar,           (void *) &NlmeGetRequest(gNwkActiveKeySeqNumber_c)},
  {gNwkAllFresh_c,                          mZtcIBRO, sizeof(NlmeGetRequest(gNwkAllFresh_c)),                 mIBScalar,           (void *) &NlmeGetRequest(gNwkAllFresh_c)},
  {gNwkSecureAllFrames_c,                   mZtcIBRO, sizeof(NlmeGetRequest(gNwkSecureAllFrames_c)),          mIBScalar,           (void *) &NlmeGetRequest(gNwkSecureAllFrames_c)},
  {gApsTrustCenterAddress_c,                mZtcIBRW, sizeof(ApsmeGetRequest(gApsTrustCenterAddress_c)),      mIBScalar,           (void *) &ApsmeGetRequest(gApsTrustCenterAddress_c)},
  {gApsSecurityTimeOutPeriod_c,             mZtcIBRW, sizeof(ApsmeGetRequest(gApsSecurityTimeOutPeriod_c)),   mIBScalar,           (void *) &ApsmeGetRequest(gApsSecurityTimeOutPeriod_c)},
#if gApsLinkKeySecurity_d
  {gApsDeviceKeyPairSet_c,                  mZtcIBRWUseFunc, sizeof(zbApsDeviceKeyPairSet_t),                       gApsMaxSecureMaterialEntries_c,   ((void *) gaApsDeviceKeyPairSet)},
#endif
#endif

  /* APS Information Base Identifiers. r17 - Table 2.24 */
  {gApsAddressMap_c,             mZtcIBRWUseFunc, sizeof(zbAddressMap_t),                             gApsMaxAddrMapEntries_c,           ((void *)( gaApsAddressMap ))},
#if gBindCapability_d
  {gApsBindingTable_c,           mZtcIBRW, sizeof(apsBindingTable_t),                               gMaximumApsBindingTableEntries_c,  ((void *)( gaApsBindingTable ))},
#endif
  {gApsDesignatedCoordinator_c,  mZtcIBRW, sizeof(gBeeStackParameters.gfApsDesignatedCoordinator),  mIBScalar,                         ((void *)(&gBeeStackParameters.gfApsDesignatedCoordinator)) },
  {gApsChannelMask_c,            mZtcIBRW, sizeof(gSAS_Ram.aChannelMask),                           mIBScalar,                         ((void *)(gSAS_Ram.aChannelMask)) },
  {gApsUseExtendedPANID_c,       mZtcIBRW, sizeof(gSAS_Ram.aApsUseExtendedPanId),                         mIBScalar,                         ((void *)((gSAS_Ram.aApsUseExtendedPanId))) },
  {gApsGroupTable_c,             mZtcIBRW, (sizeof(zbGroupId_t) + gEndPointsMaskSizeInBytes_c),     gApsMaxGroups_c,                   (( void *)(gaApsGroupTable))},
  {gApsNonmemberRadius_c,        mZtcIBRW, sizeof(gBeeStackParameters.gApsNonmemberRadius),         mIBScalar,                         ((void *)(&gBeeStackParameters.gApsNonmemberRadius)) },
  {gApsUseInsecureJoin_c,        mZtcIBRW, sizeof(gSAS_Ram.fUseInsecureJoin),                       mIBScalar,                         ((void *)(&gSAS_Ram.fUseInsecureJoin)) },
#if (gStandardSecurity_d || gHighSecurity_d) && gApsLinkKeySecurity_d
  {gSASTrustCenterMasterKey_c,   mZtcIBRW, sizeof(zbAESKey_t),                                      mIBScalar,                         (void *) gSAS_Ram.aTrustCenterMasterKey},
  {gSASTrustCenterLinkKey_c,     mZtcIBRW, sizeof(zbAESKey_t),                                      mIBScalar,                         (void *) gSAS_Ram.aPreconfiguredTrustCenterLinkKey},
#endif
#if gFragmentationCapability_d
  {gApsInterframeDelay_c,        mZtcIBRW, sizeof(gBeeStackParameters.gApsInterframeDelay),         mIBScalar,                         ((void *)(&gBeeStackParameters.gApsInterframeDelay)) },
  {gApsMaxWindowSize_c,          mZtcIBRW, sizeof(gBeeStackParameters.gApsMaxWindowSize),           mIBScalar,                         ((void *)(&gBeeStackParameters.gApsMaxWindowSize)) },
  {gApsMaxFragmentLength_c,      mZtcIBRW, sizeof(gBeeStackParameters.gApsMaxFragmentLength),       mIBScalar,                         ((void *)(&gBeeStackParameters.gApsMaxFragmentLength)) },
#endif
  {gApsLastChannelEnergy_c,      mZtcIBRO, sizeof(gApsLastChannelEnergy),                           mIBScalar,                         ((void *)(&gApsLastChannelEnergy)) },
  {gApsLastChannelFailureRate_c, mZtcIBRO, sizeof(gApsChannelFailureRate),                          mIBScalar,                         ((void *)(&gApsChannelFailureRate)) },
  {gApsChannelTimer_c,           mZtcIBRO, sizeof(gApsChannelTimer),                                mIBScalar,                         ((void *)(&gApsChannelTimer)) }

#if gNwkMulticastCapability_d
  ,{gNwkUseMulticast_c,          mZtcIBRW, sizeof(gNibNwkUseMulticast),                             mIBScalar,                          (void *) &NlmeGetRequest(gNwkUseMulticast_c)}
#endif

#if gNwkManyToOneCapability_d
, {gNwkIsConcentrator_c,             mZtcIBRW, sizeof(NlmeGetRequest(gNwkIsConcentrator_c)),             mIBScalar,                     (void *) &NlmeGetRequest(gNwkIsConcentrator_c)}
, {gNwkConcentratorRadius_c,         mZtcIBRW, sizeof(NlmeGetRequest(gNwkConcentratorRadius_c)),         mIBScalar,                     (void *) &NlmeGetRequest(gNwkConcentratorRadius_c)}
, {gNwkConcentratorDiscoveryTime_c,  mZtcIBRW, sizeof(NlmeGetRequest(gNwkConcentratorDiscoveryTime_c)),  mIBScalar,                     (void *) &NlmeGetRequest(gNwkConcentratorDiscoveryTime_c)}
#endif
, {gNwkMaxSourceRoute_c,  mZtcIBRW, sizeof(NlmeGetRequest(gNwkMaxSourceRoute_c)),             mIBScalar,                     (void *) &NlmeGetRequest(gNwkMaxSourceRoute_c)},  
  {gNwkLogicalChannel_c,  mZtcIBRW, sizeof(uint8_t), mIBScalar , (void *)&gNwkData.logicalChannel     }
};

/******************************************************************************
*******************************************************************************
* Public functions
*******************************************************************************
******************************************************************************/

/* Retrieve information from the Information Base.
 *
 * On entry
 *  pRequest->iId       ID of the IB entry to get,
 *  pRequest->iIndex    Index of the first element to get, and
 *  pRequest->iEntries  Number of elements to get.
 *  pRequest->pData     Ignored
 *
 * Return
 *  pRequest->iEntries  Actual number of elements returned,
 *  pRequest->pData     Reused as the total data returned, in bytes.
 *
 * Note that this code treats everything as an array. Scalar values are
 * considered arrays that have a maximum of one element.
 */
void ZtcMsgGetIBReqFunc(void) {
#define pRequest ((getIBRequest_t *) gZtcPacketFromClient.structured.payload)
  getIBRequest_t *pResponse = ((getIBRequest_t *) &gZtcPacketToClient.structured.payload[sizeof(clientPacketStatus_t)]);

  index_t entriesReturned = pRequest->iEntries;
  index_t entrySize;
  index_t firstEntry = pRequest->iIndex;
  index_t i;
  ztcIBData_t const *pIBData;
  index_t totalDataSize;

  for (i = 0, pIBData = NULL; i < NumberOfElements(maZtcIBData); ++i) {
    if (pRequest->iId == maZtcIBData[i].id) {
      pIBData = &maZtcIBData[i];
      break;
    }
  }

  if (!pIBData) {
    ZtcError(gZtcUnknownIBIdentifier_c);
    return;
  }

  /* The Test Tool is used by humans, who make mistakes. Be forgiving. */
  if (firstEntry >= pIBData->maxEntries) {
    firstEntry = pIBData->maxEntries - 1;
  }
  if (firstEntry + entriesReturned > pIBData->maxEntries) {
    entriesReturned = pIBData->maxEntries - firstEntry;
  }
  if (!entriesReturned) {
    entriesReturned = 1;
  }

  /* points to the "To" client packet. They don't overlap. */
  FLib_MemCpy((uint8_t *) pResponse, (void *) pRequest, sizeof(getIBRequest_t));

  pResponse->iEntries = entriesReturned;
  pResponse->iEntrySize = entrySize = pIBData->entrySize;
  totalDataSize = (index_t) (entriesReturned * entrySize);
  if (totalDataSize > sizeof(gZtcPacketFromClient.structured.payload) - sizeof(getIBRequest_t)) {
    gZtcPacketToClient.structured.payload[0] = gZtcTooBig_c;
    return;
  }

  /* Note that pRequest points to the "From" client packet, and pResponse */
  /* totalDataSize is shifted because Test Tool has to receive it little endian */
  *((uint16_t *) &pResponse->pData) = totalDataSize; /* << 8; */ 
  gZtcPacketToClient.structured.header.len = sizeof(getIBRequest_t) + totalDataSize + sizeof(clientPacketStatus_t);

/*
The actual problem is the existance of an adittional field in the payload region which specifies payload length for client
 */

#define DataSizeFieldLengthInBytes 2

  if (pIBData->access == mZtcIBRWUseFunc)
  {
    ztcIBAccessTbl_t  const *pZtcIBAccessFuncTbl = NULL;
    /* Find the entry in maZtcIBAccessFuncTbl */
    for (i = 0; i < NumberOfElements(maZtcIBAccessFuncTbl); ++i) {
      if (pIBData->id == maZtcIBAccessFuncTbl[i].id) {
        pZtcIBAccessFuncTbl = &maZtcIBAccessFuncTbl[i];
        break;
      }
    }
    
    if (pZtcIBAccessFuncTbl)
    { 
      uint8_t *pEntries = ((uint8_t *) &pResponse->pData) + DataSizeFieldLengthInBytes; 
      for(i = firstEntry; i < (firstEntry+entriesReturned); i++)
      {
        (void)(pZtcIBAccessFuncTbl)->pGetTblEntry(i, pEntries);
        pEntries = pEntries + entrySize;
      }
      return;
    }
  }
  
FLib_MemCpy(((uint8_t *) &pResponse->pData) + DataSizeFieldLengthInBytes,
              (void *) (((uint8_t *) pIBData->pTable) + ((uint16_t)entrySize * firstEntry)),
              totalDataSize);
        
#undef DataSizeFieldLengthInBytes /*  As this is not final, get rid of it ASAP */
        
#undef pRequest
}                                       /* ZtcMsgGetIBReqFunc() */

/*****************************************************************************/

/* Store information to the Information Base.
 *
 * On entry
 *  pRequest->iId       ID of the IB entry to set,
 *  pRequest->iIndex    Index of the first element to set, and
 *  pRequest->iEntries  Number of elements to set.
 *  pRequest->pData     Not actually a pointer; replaced by the start of the
 *                          data.
 *
 * Note that this code treats everything as an array. Scalar values are
 * considered arrays that have a maximum of one element.
 */
void ZtcMsgSetIBReqFunc(void) {
#define pRequest ((getIBRequest_t *) gZtcPacketFromClient.structured.payload)

  index_t entriesToSet = pRequest->iEntries;
  index_t entrySize;
  index_t firstEntry = pRequest->iIndex;
  index_t i;
  ztcIBData_t const *pIBData;
  index_t totalDataSize;

  for (i = 0, pIBData = NULL; i < NumberOfElements(maZtcIBData); ++i) {
    if (pRequest->iId == maZtcIBData[i].id) {
      pIBData = &maZtcIBData[i];
      break;
    }
  }

  if (!pIBData) {
    ZtcError(gZtcUnknownIBIdentifier_c);
    return;
  }

  if ((pIBData->access != mZtcIBRW) && (pIBData->access != mZtcIBRWUseFunc)) {
    gZtcPacketToClient.structured.payload[0] = gZtcReadOnly_c;
    return;
  }

  entrySize = pIBData->entrySize;
  totalDataSize = (index_t) (entriesToSet * entrySize);

  /* Check each of these conditions separately, to catch overflows. */
  if (  (firstEntry >= pIBData->maxEntries)
      || (firstEntry + entriesToSet > pIBData->maxEntries)
      || (totalDataSize > gZtcPacketFromClient.structured.header.len)) {
    gZtcPacketToClient.structured.payload[0] = gZtcTooBig_c;
    return;
  }
  
  if (pIBData->access == mZtcIBRWUseFunc)
  {

    ztcIBAccessTbl_t  const *pZtcIBAccessFuncTbl = NULL;
    /* Find the entry in maZtcIBAccessFuncTbl */
    for (i = 0; i < NumberOfElements(maZtcIBAccessFuncTbl); ++i) {
      if (pIBData->id == maZtcIBAccessFuncTbl[i].id) {
        pZtcIBAccessFuncTbl = &maZtcIBAccessFuncTbl[i];
        break;
      }
    } 
    if (pZtcIBAccessFuncTbl)
    { 
      uint8_t *pEntries = (void *) &pRequest->pData;
      for(i = firstEntry; i < (firstEntry + entriesToSet); i++)
      {
        (void)(pZtcIBAccessFuncTbl)->pSetTblEntry(i, pEntries);
        pEntries = pEntries + entrySize;
      }
      return;
    }    

  }

  FLib_MemCpy((uint8_t *) pIBData->pTable + ((uint16_t)entrySize * firstEntry),
              (void *) &pRequest->pData,
              totalDataSize);

#undef pRequest
}                                       /* ZtcMsgSetIBReqFunc() */

/*****************************************************************************
 This Function store  to the Information Base in BlackBox and Host.
 *****************************************************************************/
void ZtcMsgSetIBReq(uint8_t msgType, uint8_t attrId, uint8_t index, uint8_t *pValue) {
  
  index_t i;
  uint8_t entrySize;
  
  /* Search attribute in table */
  for (i = 0; i < NumberOfElements(maZtcIBData); ++i) {
    if (attrId == maZtcIBData[i].id) {
      break;
    }
  } 
  
  if (i == NumberOfElements(maZtcIBData)) {
    return;
  }

  entrySize = maZtcIBData[i].entrySize;
  if((maZtcIBData[i].access == mZtcIBRWUseFunc))
  {
    ztcIBAccessTbl_t  const *pZtcIBAccessFuncTbl = NULL;
    uint8_t j;
      /* Find the entry in maZtcIBAccessFuncTbl */
    for (j = 0; j < NumberOfElements(maZtcIBAccessFuncTbl); ++j) {
      if (maZtcIBData[i].id == maZtcIBAccessFuncTbl[j].id) {
        pZtcIBAccessFuncTbl = &maZtcIBAccessFuncTbl[j];
        break;
      }
    }
    if (pZtcIBAccessFuncTbl)
     (void)(pZtcIBAccessFuncTbl)->pSetTblEntry(index, pValue);
    
  }
  else
  {
      /* Set the IB attribute locally and sent it over the UART (to BlackBox) */
    FLib_MemCpy((uint8_t *) maZtcIBData[i].pTable + (uint16_t) (entrySize * index),
                pValue,
                entrySize);
  }
  gZtcPacketToClient.structured.header.opcodeGroup    = gZtcReqOpcodeGroup_c;
  gZtcPacketToClient.structured.header.msgType        = msgType;
  gZtcPacketToClient.structured.payload[0] = attrId;
  gZtcPacketToClient.structured.payload[1] = index;
  gZtcPacketToClient.structured.payload[2] = 0x01; /* Update one entry each time */
  gZtcPacketToClient.structured.payload[3] = entrySize;
  FLib_MemCpy(&gZtcPacketToClient.structured.payload[4],
              pValue,
              entrySize);
  gZtcPacketToClient.structured.header.len = 4 + entrySize;
  ZtcComm_WritePacketToClient((gZtcPacketToClient.structured.header.len +
                         sizeof(gZtcPacketToClient.structured.header)));            
}

#endif /* gBeeStackIncluded_d */
#endif                                  /* #if gZtcIncluded_d */
