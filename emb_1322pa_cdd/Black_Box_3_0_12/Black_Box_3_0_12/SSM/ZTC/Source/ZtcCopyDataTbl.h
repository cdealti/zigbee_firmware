/*****************************************************************************
* Tables of information about Ztc data formats.
*
* Copyright (c) 2008, Freescale, Inc.  All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from  Freescale Semiconductor.
*
******************************************************************************/

/* The purpose of these tables is to keep various declarations and defintions
 * in perfect sync, without requiring maintenence of different declarations.
 *
 * The code should never be aware of which data format it is working with.
 * There should never be logic of the form:
 *
 *      "if this is SAP Handler X (or message type X)
 *      then do Y"
 *
 * All of the knobs that can be tweaked to describe data formats should be
 * in this file. The code should instead contain logic of the form:
 *
 *      "if the data format info table entry for this data format says X,
 *      then do Y".
 *
 * Making a change to the way the code deals with different data formats
 * should require only changes in this file, unless a completely new kind
 * of handling is needed. In that case, this file should be changed to provide
 * a means of triggering the new behavior of the code.
 */

/* IMPORTANT NOTE: The order of the entries in these tables is not significant.
 * Entries can be added, removed or moved freely. The code must work if the
 * order of the entries in the tables is changed.
 */

/* All SAP Handlers receive a pointer to a message, which the SAP Handler
 * interprets as a pointer to a structure type. The first byte of the
 * message is always the message type. All of the structure types contain
 * a union of other structure types for access to the rest of the data in
 * the message.
 * The APP_ZDP_SapHandler(), unlike all other SAP Handlers, has an additional
 * data item between the message type and the union.
 * These macros compenstate for that extra field.
 */
#define AppZdpSize(type) \
  (sizeof(type) + MbrOfs(appToZdpMessage_t, msgData) - MbrSizeof(appToZdpMessage_t, msgType))

#define AppZdpOfs(type, member) \
  ((MbrOfs(appToZdpMessage_t, msgData) - MbrSizeof(appToZdpMessage_t, msgType)) + MbrOfs(type, member))

/****************************************************************************/

/* Before defining the main table, define the format parameter subtables.
 * Some of the format copy routines can handle a number of specific formats,
 * with parameters describing the details. Each format copy routine knows
 * about its parameter table; all that is needed here is to enter the actual
 * paremeters into the right tables, and provide a table index that the copy
 * routine can use to find the appropriate row.
 *
 * Not all copy functions need (or have) parameter tables.
 */

/* Parameter table for packets and messages that contain a header plus a
 * variable number of data items. The number of items is specified in a
 * field in the header.
 *
 * For packet-from-message and message-from-packet, copy the entire source
 * (header plus data) to the destination.
 *
 * The macro format is:
 *  Column 1    name: Name of index, for use in the Fmt() table.
 *  Column 2    hdrLen: Length of the header.
 *  Column 3    offsetToCount: Offset in the header to the number of
 *                items of data.
 *  Column 4    itemLen: length of each data item.
 *
 * Used to define ztcFmtHdrAndData[].
 *
 * This format is similar to FmtPtrToArray, but there is no pointer.
 */
#if gBeeStackIncluded_d

#ifndef FmtHdrAndData
#define FmtHdrAndData(name, hdrLen, offsetToCount, itemLen)
#endif


/* Used for ZDP-NWK_addr.response (A0 80) and and ZDP-IEEE_addr.response
 * (A0 81). Note that zbAddressResponse_t has a union, devResponse, with two
 * variants, each of a different length. The zbSingleDecResp_t variant is a
 * simple format; the zbExterndedDevResp_t is a header-and-data format. The
 * zbAddressResponse_t struct does not provide any way to determine which
 * variant is being used. BeeStack interally know it by context, but Ztc
 * doesn't have that information. By convention, BeeStack internally always
 * uses the longer, variable length zbExtendedDevResp_t variant, with unused
 * fields set to zero. This convention allows Ztc to always treat the message
 * as a header-and-data format.
 */

FmtHdrAndData(mFmtNwkAddrResp_c,                                                                            /* name             */ \
              (sizeof(zbAddressResponse_t)                                                                                         \
               - MbrSizeof(zbAddressResponse_t, devResponse.extendedDevResp.aNwkAddrAssocDevList)),         /* hdrLen           */ \
              (MbrOfs(zbAddressResponse_t, devResponse.extendedDevResp.numAssocDev)),                       /* offsetToCount    */ \
              (MbrSizeof(zbAddressResponse_t, devResponse.extendedDevResp.aNwkAddrAssocDevList[0])))        /* itemLen          */

/* Used for ZDP-SimpleDescriptor.Response (A0 84). */
FmtHdrAndData(mFmtZdpSimpleDescResp_c,                                                  /* name             */ \
              (sizeof(zbSimpleDescriptorResponse_t)                                                            \
               - MbrSizeof(zbSimpleDescriptorResponse_t, simpleDescriptor)),            /* hdrLen           */ \
              (MbrOfs(zbSimpleDescriptorResponse_t, length)),                           /* offsetToCount    */ \
              (sizeof(uint8_t)))                                                        /* itemLen          */

/* Used for ZDP-Active_EP_rsp.response (A0 85). */
FmtHdrAndData(mFmtZdpActiveEPResp_c,                                                    /* name             */ \
              (sizeof(zbActiveEpResponse_t)                                                                    \
               - MbrSizeof(zbActiveEpResponse_t, pActiveEpList)),                       /* hdrLen           */ \
              (MbrOfs(zbActiveEpResponse_t, activeEpCount)),                            /* offsetToCount    */ \
              (MbrSizeof(zbActiveEpResponse_t, pActiveEpList[0])))                      /* itemLen          */

/* Used for ZDP-Match_Desc_resp.response (A0 86). */
FmtHdrAndData(mFmtZdpMatchDescResp_c,                                                                       /* name             */ \
              (sizeof(zbMatchDescriptorResponse_t) - MbrSizeof(zbMatchDescriptorResponse_t, matchList)),    /* hdrLen           */ \
              (MbrOfs(zbMatchDescriptorResponse_t, matchLength)),                                           /* offsetToCount    */ \
              (MbrSizeof(zbMatchDescriptorResponse_t, matchList[0])))                                       /* itemLen          */

/* Used for ZDP-Complex_Desc_rsp (A0 90). */
FmtHdrAndData(mFmtZdpComplexDescResp_c,                                                 /* name             */ \
              (sizeof(zbComplexDescriptorResponse_t)                                                           \
               - MbrSizeof(zbComplexDescriptorResponse_t, complexDescrip)),             /* hdrLen           */ \
              (MbrOfs(zbComplexDescriptorResponse_t, length)),                          /* offsetToCount    */ \
              (sizeof(zbSize_t)))                                                       /* itemLen          */

/* Used for ZDP-UserDescriptor.Response (A0 91). */
FmtHdrAndData(mFmtZdpUserDescResp_c,                                                    /* name             */ \
              (sizeof(zbUserDescriptorResponse_t)                                                              \
               - MbrSizeof(zbUserDescriptorResponse_t, descriptor.aUserDescription)),   /* hdrLen           */ \
              (MbrOfs(zbUserDescriptorResponse_t, descriptor.length)),                  /* offsetToCount    */ \
              (sizeof(uint8_t)))                                                        /* itemLen          */

/* Used for ZDP-Bind_Register.response (A0 A3). */
/* *warning* aBindingTableListCount is declared as 16 bits, but the */
/* BeeStack code only ever uses the first byte. */
FmtHdrAndData(mFmtZdpBindRegResp_c,                                                     /* name             */ \
              (sizeof(zbBindRegisterResponse_t)                                                                \
               - MbrSizeof(zbBindRegisterResponse_t, aBindingTableList)),               /* hdrLen           */ \
              (MbrOfs(zbBindRegisterResponse_t, aBindingTableListCount)),               /* offsetToCount    */ \
              (MbrSizeof(zbBindRegisterResponse_t, aBindingTableList[0])))              /* itemLen          */

/* Used for ZDP-Recover_Bind_Table.response (A0 A8). */
/* *warning* BindingTableListCount is declared as 16 bits, but the */
/* BeeStack code only ever uses the first byte. */
FmtHdrAndData(mFmtZdpRecoverBindTblResp_c,                                              /* name             */ \
              (sizeof(zbRecoverBindTableResponse_t)                                                            \
               - MbrSizeof(zbRecoverBindTableResponse_t, aBindingTableList)),           /* hdrLen           */ \
              (MbrOfs(zbRecoverBindTableResponse_t, aBindingTableListCount)),           /* offsetToCount    */ \
              (MbrSizeof(zbRecoverBindTableResponse_t, aBindingTableList[0])))          /* itemLen          */

/* Used for ZDP-Recover_Source_Bind.response (A0 AA). */
/* *warning* SourceTableListCount is declared as 16 bits, but the */
/* BeeStack code only ever uses the first byte. */
FmtHdrAndData(mFmtZdpRecoverSrcBindResp_c,                                              /* name             */ \
              (sizeof(zbRecoverSourceBindResponse_t)                                                           \
               - MbrSizeof(zbRecoverSourceBindResponse_t, SourceTableList)),            /* hdrLen           */ \
              (MbrOfs(zbRecoverSourceBindResponse_t, SourceTableListCount)),            /* offsetToCount    */ \
              (MbrSizeof(zbRecoverSourceBindResponse_t, SourceTableList[0])))           /* itemLen          */

/* Used for ZDP-Mgmt_Nwk_Disc.Response (A0 B0). */
FmtHdrAndData(mFmtZdpMgmtNwkDiscResp_c,                                                 /* name             */ \
              (sizeof(zbMgmtNwkDiscResponse_t)                                                                 \
               - MbrSizeof(zbMgmtNwkDiscResponse_t, nwkList)),                          /* hdrLen           */ \
              (MbrOfs(zbMgmtNwkDiscResponse_t, nwkListCount)),                          /* offsetToCount    */ \
              (MbrSizeof(zbMgmtNwkDiscResponse_t, nwkList[0])))                         /* itemLen          */

/* Used for ZDP-Mgmt_Lqi.Response (A0 B1). */
FmtHdrAndData(mFmtZdpMgmtLqiResp_c,                                                     /* name             */ \
              (sizeof(zbMgmtLqiResponse_t)                                                                     \
               - MbrSizeof(zbMgmtLqiResponse_t, neighbourTableList)),                   /* hdrLen           */ \
              (MbrOfs(zbMgmtLqiResponse_t, neighbourTableListCount)),                   /* offsetToCount    */ \
              (MbrSizeof(zbMgmtLqiResponse_t, neighbourTableList[0])))                  /* itemLen          */

/* Used for ZDP-Mgmt_Rtg.Response (A0 B2). */
FmtHdrAndData(mFmtZdpMgmtRtgResp_c,                                                     /* name             */ \
              (sizeof(zbMgmtRtgResponse_t)                                                                     \
               - MbrSizeof(zbMgmtRtgResponse_t, routingTableList)),                     /* hdrLen           */ \
              (MbrOfs(zbMgmtRtgResponse_t, routingTableListCount)),                     /* offsetToCount    */ \
              (MbrSizeof(zbMgmtRtgResponse_t, routingTableList[0])))                    /* itemLen          */

/* Used for ZDP-Mgmt_Bind.Response (A0 B3). */
FmtHdrAndData(mFmtZdpMgmtBindResp_c,                                                    /* name             */ \
              (sizeof(zbMgmtBindResponse_t)                                                                    \
               - MbrSizeof(zbMgmtBindResponse_t, aBindingTableList)),                   /* hdrLen           */ \
              (MbrOfs(zbMgmtBindResponse_t, bindingTableListCount)),                    /* offsetToCount    */ \
              (MbrSizeof(zbMgmtBindResponse_t, aBindingTableList[0])))                  /* itemLen          */

/* Used for ZDP-Mgmt_Cache.response (A0 B7). */
FmtHdrAndData(mFmtZdpMgmtCacheResp_c,                                                   /* name             */ \
              (sizeof(zbMgmtCacheResponse_t)                                                                   \
               - MbrSizeof(zbMgmtCacheResponse_t, discoveryCacheList)),                 /* hdrLen           */ \
              (MbrOfs(zbMgmtCacheResponse_t, discoveryCacheListCount)),                 /* offsetToCount    */ \
              (MbrSizeof(zbMgmtCacheResponse_t, discoveryCacheList[0])))                /* itemLen          */

/* Used for ZDP-NwkUpdate.notify (A0 B8). */
FmtHdrAndData(mFmtZdpMgmtNwkUpdateNotify_c,                                                /* name             */ \
              (sizeof(zbMgmtNwkUpdateNotify_t)                                                                   \
               - MbrSizeof(zbMgmtNwkUpdateNotify_t, aEnergyValues)),                       /* hdrLen           */ \
              (MbrOfs(zbMgmtNwkUpdateNotify_t, ScannedChannelsListCount)),                 /* offsetToCount    */ \
              (MbrSizeof(zbMgmtNwkUpdateNotify_t, aEnergyValues[0])))                      /* itemLen          */

/* Used for ZDP-Discovery_Store.Request (A2 16). */
FmtHdrAndData(mFmtZdpDiscStoreReq_c,                                                    /* name             */ \
              (AppZdpSize(zbDiscoveryStoreRequest_t)                                                           \
               - MbrSizeof(zbDiscoveryStoreRequest_t, simpleDescriptorList)),           /* hdrLen           */ \
              (AppZdpOfs(zbDiscoveryStoreRequest_t, simpleDescriptorCount)),            /* offsetToCount    */ \
              (MbrSizeof(zbDiscoveryStoreRequest_t, simpleDescriptorList[0])))          /* itemLen          */

/* Used for ZDP-Active_EP_Store.Request (A2 19). */
FmtHdrAndData(mFmtZdpActiveEPStoreReq_c,                                                /* name             */ \
              (AppZdpSize(zbActiveEPStoreRequest_t)                                                            \
               - MbrSizeof(zbActiveEPStoreRequest_t, activeEPList)),                    /* hdrLen           */ \
              (AppZdpOfs(zbActiveEPStoreRequest_t, activeEPCount)),                     /* offsetToCount    */ \
              (MbrSizeof(zbActiveEPStoreRequest_t, activeEPList[0])))                   /* itemLen          */

/* Used for ZDP-Backup_Bind_Table.Request (A2 27). */
/* *warning* BindingTableListCount is declared as 16 bits, but the */
/* BeeStack code only ever uses the first byte. */
FmtHdrAndData(mFmtZdpBkupBindTblReq_c,                                                  /* name             */ \
              (AppZdpSize(zbBackupBindTableRequest_t)                                                          \
               - MbrSizeof(zbBackupBindTableRequest_t, BindingTableList)),              /* hdrLen           */ \
              (AppZdpOfs(zbBackupBindTableRequest_t, BindingTableListCount)),           /* offsetToCount    */ \
              (MbrSizeof(zbBackupBindTableRequest_t, BindingTableList[0])))             /* itemLen          */

/* Used for ZDP-Backup_Source_Bind.Request (A2 29). */
/* *warning* SourceTableListCount is declared as 16 bits, but the */
/* BeeStack code only ever uses the first byte. */
FmtHdrAndData(mFmtZdpBkupSrcBindReq_c,                                                  /* name             */ \
              (AppZdpSize(zbBackupSourceBindRequest_t)                                                         \
               - MbrSizeof(zbBackupSourceBindRequest_t, SourceTableList)),              /* hdrLen           */ \
              (AppZdpOfs(zbBackupSourceBindRequest_t, SourceTableListCount)),           /* offsetToCount    */ \
              (MbrSizeof(zbBackupSourceBindRequest_t, SourceTableList[0])))             /* itemLen          */


FmtHdrAndData(mFmtHcFrame_c,                                                  /* name             */ \
              0,                                                             /* hdrLen           */ \
               0,                           /* offsetToCount    */ \
              (sizeof(uint8_t)))                                                        /* itemLen  */

#endif /* gBeeStackIncluded_d */
#undef FmtHdrAndData

/****************************************************************************/

/* Tables of length-index-value-to-length, used by the FmtLenCodeAndPtr()
 * macros below.
 *
 * The macro format is:
 *  Column 1    code: The content of the code value specified by
 *                offsetToLenCode in the FmtLenCodeAndPtr() table below.
 *  Column 2    len: The length of the data associated with this
 *                index value.
 *
 * *warning* This format is only used for get/set messages, but the entire
 * get/set mechanism in BeeStack is probably going to be redesigned.
 */

#if mZtcLenCodeAndPtrFormat_d

#ifndef CodeAndLenMacSetReq
#define CodeAndLenMacSetReq(index, len)
#endif

/* Used for MacGetPIBAttribute.Request (85 03), aka MLME-GET.request */
/*      and MacSetPIBAttribute.Request (85 09), aka MLME-SET.request */
/* Defines maZtcMacSetDataLengthsTable[]. */
CodeAndLenMacSetReq(gMPibAckWaitDuration_c,                 MbrSizeof(macPib_t, mPIBimacAckWaitDuration))
CodeAndLenMacSetReq(gMPibAssociationPermit_c,               MbrSizeof(macPib_t, mPIBimacAssociationPermit))
CodeAndLenMacSetReq(gMPibAutoRequest_c,                     MbrSizeof(macPib_t, mPIBimacAutoRequest))
CodeAndLenMacSetReq(gMPibBattLifeExt_c,                     MbrSizeof(macPib_t, mPIBimacBattLifeExt))
CodeAndLenMacSetReq(gMPibBattLifeExtPeriods_c,              MbrSizeof(macPib_t, mPIBimacBattLifeExtPeriods))
CodeAndLenMacSetReq(gMPibBeaconPayload_c,                   MbrSizeof(macPib_t, mPIBnmacBeaconPayload))
CodeAndLenMacSetReq(gMPibBeaconPayloadLength_c,             MbrSizeof(macPib_t, mPIBimacBeaconPayloadLength))
CodeAndLenMacSetReq(gMPibBeaconOrder_c,                     MbrSizeof(macPib_t, mPIBimacBeaconOrder))
CodeAndLenMacSetReq(gMPibBeaconTxTime_c,                    MbrSizeof(macPib_t, mPIBlmacBeaconTxTime))
CodeAndLenMacSetReq(gMPibBsn_c,                             MbrSizeof(macPib_t, mPIBimacBsn))
CodeAndLenMacSetReq(gMPibCoordExtendedAddress_c,            MbrSizeof(macPib_t, mPIBnmacCoordExtendedAddress))
CodeAndLenMacSetReq(gMPibCoordShortAddress_c,               MbrSizeof(macPib_t, mPIBsmacCoordShortAddress))
CodeAndLenMacSetReq(gMPibDsn_c,                             MbrSizeof(macPib_t, mPIBimacDsn))
CodeAndLenMacSetReq(gMPibGtsPermit_c,                       MbrSizeof(macPib_t, mPIBimacGtsPermit))
CodeAndLenMacSetReq(gMPibMaxCsmaBackoffs_c,                 MbrSizeof(macPib_t, mPIBimacMaxCsmaBackoffs))
CodeAndLenMacSetReq(gMPibMinBe_c,                           MbrSizeof(macPib_t, mPIBimacMinBe))
CodeAndLenMacSetReq(gMPibPanId_c,                           MbrSizeof(macPib_t, mPIBsmacPanId))
CodeAndLenMacSetReq(gMPibPromiscuousMode_c,                 MbrSizeof(macPib_t, mPIBimacPromiscuousMode))
CodeAndLenMacSetReq(gMPibRxOnWhenIdle_c,                    MbrSizeof(macPib_t, mPIBimacRxOnWhenIdle))
CodeAndLenMacSetReq(gMPibShortAddress_c,                    MbrSizeof(macPib_t, mPIBsmacShortAddress))
CodeAndLenMacSetReq(gMPibSuperFrameOrder_c,                 MbrSizeof(macPib_t, mPIBimacSuperFrameOrder))
CodeAndLenMacSetReq(gMPibTransactionPersistenceTime_c,      MbrSizeof(macPib_t, mPIBsmacTransactionPersistenceTime))

#if gSecurityCapability_d

CodeAndLenMacSetReq(gMPibAclEntryDescriptorSet_c,           MbrSizeof(macPib_t, gpPIBaclEntryDescriptorSet))
CodeAndLenMacSetReq(gMPibAclEntryDescriptorSetSize_c,       MbrSizeof(macPib_t, mPIBimacAclEntryDescriptorSetSize))
CodeAndLenMacSetReq(gMPibDefaultSecurity_c,                 MbrSizeof(macPib_t, mPIBimacDefaultSecurity))
CodeAndLenMacSetReq(gMPibDefaultSecurityMaterialLength_c,   MbrSizeof(macPib_t, mPIBimacDefaultSecurityMaterialLength))
CodeAndLenMacSetReq(gMPibDefaultSecurityMaterial_c,         MbrSizeof(macPib_t, mPIBnmacDefaultSecurityMaterial))
CodeAndLenMacSetReq(gMPibDefaultSecuritySuite_c,            MbrSizeof(macPib_t, mPIBimacDefaultSecuritySuite))
CodeAndLenMacSetReq(gMPibSecurityMode_c,                    MbrSizeof(macPib_t, mPIBimacSecurityMode))
/* Vendor specific. Used for selecting current */
/* entry in ACL descriptor table.              */
CodeAndLenMacSetReq(gMPibAclEntryCurrent_c,                 MbrSizeof(macPib_t, mPIBimacAclEntryCurrent))
/* The following attributes are mapped to elements */
/* in the gMPibAclEntryDescriptorSet_c attribute.  */
CodeAndLenMacSetReq(gMPibAclEntryExtAddress_c,              MbrSizeof(aclEntryDescriptor_t, aclExtendedAddress))
CodeAndLenMacSetReq(gMPibAclEntryShortAddress_c,            MbrSizeof(aclEntryDescriptor_t, aclShortAddress))
CodeAndLenMacSetReq(gMPibAclEntryPanId_c,                   MbrSizeof(aclEntryDescriptor_t, aclPanId))
CodeAndLenMacSetReq(gMPibAclEntrySecurityMaterialLength_c,  MbrSizeof(aclEntryDescriptor_t, aclSecurityMaterialLength))
CodeAndLenMacSetReq(gMPibAclEntrySecurityMaterial_c,        MbrSizeof(aclEntryDescriptor_t, aclSecurityMaterial))
CodeAndLenMacSetReq(gMPibAclEntrySecuritySuite_c,           MbrSizeof(aclEntryDescriptor_t, aclSecuritySuite))

#endif                                  /* #if gSecurityCapability_d */

CodeAndLenMacSetReq(gMPibRole_c,                            MbrSizeof(macPib_t, mPIBimacRole))
CodeAndLenMacSetReq(gMPibLogicalChannel_c,                  MbrSizeof(macPib_t, mPIBimacLogicalChannel))
CodeAndLenMacSetReq(gMPibTreemodeStartTime_c,               MbrSizeof(macPib_t, mPIBlmacTreemodeStartTime))
CodeAndLenMacSetReq(gMPibPanIdConflictDetection_c,          MbrSizeof(macPib_t, mPIBimacPanIdConflictDetection))
CodeAndLenMacSetReq(gMPibBeaconResponseDenied_c,            MbrSizeof(macPib_t, mPIBimacBeaconResponseDenied))
CodeAndLenMacSetReq(gMPibNBSuperFrameInterval_c,            MbrSizeof(macPib_t, mPIBimacNBSuperFrameInterval))
CodeAndLenMacSetReq(gMPibBeaconPayloadLength_c,             MbrSizeof(macPib_t, mPIBimacBeaconPayloadLengthVendor))

#undef CodeAndLenMacSetReq
#endif                                  /* #if mZtcLenCodeAndPtrFormat_d */

/***************************************
 * Parameter table for packets and messages that contain a pointer, a count
 * of array elements, and an index to a table of array element lengths.
 *
 * For packet-from-message, copy the message header to the packet. Use the
 * length code to find the length of each element in the array, multiply
 * by the number of elements, and store the length in the packet header
 * as a 16 bit value, replacing the message header's pointer. Copy the data
 * to immediately after the packet header. Note that the Test Tool is not
 * capable of doing the count * element-length arithmetic to find the length
 * of the data, so Ztc must do that for it. The Test Tool ignores the count
 * field in the packet header.
 *
 * For message-from-packet, copy the packet header to the message header. Use
 * the packet header's code field to look up the length of the array elements.
 * Copy the data from just after the end of the packet header to the offset
 * in the message specified by the offsetToMsgData field in this table. Store a
 * pointer to the data in the message header, at the offsetToPtr location.
 *
 * For both, the length of the array elements is found by using the value at
 * the offsetToLenCode as an index in the tableOfDataLengths.
 *
 * The macro format is:
 *  Column 1    name: Name of index, for use in the Fmt() table.
 *  Column 2    hdrLen: Total length of the header that contains the pointer.
 *  Column 3    offsetToPtr: For packet-from-message, the offset in the
 *                packet header of a 16 bit data length field. For
 *                message-from-packet, the offset in the message header
 *                of a pointer to the data. If this value is -1, there is
 *                no pointer; the data immediately follows the header.
 *  Column 4    offsetToCount: offset to a count of the number of elements
 *                in the data array. If this is == -1, then there is no
 *                count field; the count is 1.
 *  Column 5    offsetToLenCode: Offset in the header to the array element
 *                length code.
 *  Column 6    tableOfDataLengths: Pointer to the table of array element
 *                lengths.
 *  Column 7    offsetToMsgData: When copying an array message-from-packet,
 *                this is the offset to the destination array from the
 *                beginning of the destination message. Ignored for
 *                packet-from-message copies.
 *
 * This macro is used to create maZtcLenCodeAndPtrTable[].
 */

#if mZtcLenCodeAndPtrFormat_d

#ifndef FmtLenCodeAndPtr
#define FmtLenCodeAndPtr(name, hdrLen, offsetToPtr, offsetToCount, \
                         offsetToLenCode, tableOfDataLengths, offsetToMsgData)
#endif

/* Used for MacGetPIBAttribute.Request (85 03), aka MLME-GET.confirm.
 * This is a synchronous request, so the offsetToMsgData can be at the end of the
 * message header.
 * Note that MacGetPIBAttribute.Request (84 04) and
 *           MacSetPIBAttribute.Request (85 09)
 * use the same data lengths table.
 */
FmtLenCodeAndPtr(mFmtMacGetReq_c,                               /* name                 */ \
                 (sizeof(mlmeGetReq_t)),                        /* hdrLen               */ \
                 (MbrOfs(mlmeGetReq_t, pibAttributeValue)),     /* offsetToPtr          */ \
                 (uint8_t)gMinusOne_c,                          /* offsetToCount        */ \
                 (MbrOfs(mlmeGetReq_t, pibAttribute)),          /* offsetToLenCode      */ \
                 maZtcMacSetDataLengthsTable,                   /* tableOfDataLengths   */ \
                 (sizeof(mlmeGetReq_t)))                        /* offsetToMsgData      */

/* Used for MacSetPIBAttribute.Request (85 09), aka MLME-SET.request.
 * This is a synchronous request, so the offsetToMsgData can be at the end of the
 * message header.
 * Note that MacGetPIBAttribute.Request (84 04) and
 *           MacSetPIBAttribute.Request (85 09)
 * use the same data lengths table.
 */
FmtLenCodeAndPtr(mFmtMacSetReq_c,                               /* name                 */ \
                 (sizeof(mlmeSetReq_t)),                        /* hdrLen               */ \
                 (MbrOfs(mlmeSetReq_t, pibAttributeValue)),     /* offsetToPtr          */ \
                 (uint8_t)gMinusOne_c,                          /* offsetToCount        */ \
                 (MbrOfs(mlmeSetReq_t, pibAttribute)),          /* offsetToLenCode      */ \
                 maZtcMacSetDataLengthsTable,                   /* tableOfDataLengths   */ \
                 (sizeof(mlmeSetReq_t)))                        /* offsetToMsgData      */

#undef FmtLenCodeAndPtr
#endif                                  /* #if mZtcLenCodeAndPtrFormat_d */

/****************************************************************************/

/* Parameter table for packets and messages that contain two pointers, each
 * pointing to some number of fixed length data items, plus counts of the
 * numbers of items.
 *
 * This is the same as FmtPtrToArray(), but with two length-and-pointers to
 * two arrays.
 *
 * The message consists of a header that contains two pointers to arrays of
 * fixed length elements, plus two counts of the number of array elements.
 * The array data is somewhere else.
 *
 * The packet format is similar to the message, but the pointers are replaced
 * by the array data.
 *
 * For packet-from-message, copy the source message to the destination packet,
 * replacing each pointer with the data it points to.
 *
 * For message-from-packet, copy the data from the locations of the pointers
 * in the header to a location in the message specified by the offsetToMsgData.
 * fields in this table, then copy the source header to the destination
 * message, adding pointers to the data in the offsetToPtr[1|2} locations.
 *
 * For both directions, the length of the data == the number of array elements
 * specified in the count fields of the source times the fixed length of each
 * element from this table.
 *
 * The macro format is:
 *  Column  1   name: Name of index, for use in the Fmt() table.
 *  Column  2   hdrLen: Total length of the header that contains the pointer.
 *  Column  3   offsetToPtr1: Offset in message to the pointer to the first
 *                array.
 *  Column  4   offsetToCount1: Offset in message to the count of items in
 *                the first array.
 *  Column  5   arrayElementLen1: Length of each array item in the first
 *                array.
 *  Column  6   offsetToMsgData: When copying an array message-from-packet,
 *                this is the offset to the first destination array from the
 *                beginning of the destination message. Ignored for
 *                packet-from-message copies.
 *  Column  7   offsetToPtr2: Offset in message to the pointer to the second
 *                array.
 *  Column  8   offsetToCount2: Offset in message to the count of items in
 *                the second array.
 *  Column  9   arrayElementLen2: Length of each array item.
 *
 * The header offset values describe the typedef used by the message.
 *
 * Note that there is no offsetToMsgData2. In message-from-packet, the second
 * array is copied to immediately after the first.
 *
 * Used to define ztcFmtTwoArrayPtrs[].
 */

#ifndef FmtTwoArrayPtrs
#define FmtTwoArrayPtrs(name, hdrLen, \
                        offsetToPtr1, offsetToCount1, array1ElementLen, offsetToMsgData, \
                        offsetToPtr2, offsetToCount2, array2ElementLen)
#endif

/* Used for ZDP-Match_Desc_req.Request (A2 06). */
#if gBeeStackIncluded_d
FmtTwoArrayPtrs(mFmtZdpMatchDescReq_c,                                                              /* name             */ \
               (AppZdpSize(zbMatchDescriptorRequestPtr_t)),                                           /* hdrLen           */ \
               (AppZdpOfs( zbMatchDescriptorRequestPtr_t, inputClusterList.pClusterList)),            /* offsetToPtr1     */ \
               (AppZdpOfs( zbMatchDescriptorRequestPtr_t, inputClusterList.cNumClusters)),            /* offsetToCount1   */ \
               (IndirectMbrSizeof(zbMatchDescriptorRequestPtr_t, inputClusterList.pClusterList)),     /* array1ElementLen */ \
               (AppZdpSize(zbMatchDescriptorRequestPtr_t)),                                           /* offsetToMsgData  */ \
               (AppZdpOfs( zbMatchDescriptorRequestPtr_t, outputClusterList.pClusterList)),           /* offsetToPtr2     */ \
               (AppZdpOfs( zbMatchDescriptorRequestPtr_t, outputClusterList.cNumClusters)),           /* offsetToCount2   */ \
               (IndirectMbrSizeof(zbMatchDescriptorRequestPtr_t, outputClusterList.pClusterList)))    /* array2ElementLen */


/* Used for ZDP-End_Device_Bind.Request (A2 20). */
FmtTwoArrayPtrs(mFmtZdpEndDeviceBindReq_c,                                                          /* name             */ \
                (AppZdpSize(zbEndDeviceBindRequest_t)),                                             /* hdrLen           */ \
                (AppZdpOfs( zbEndDeviceBindRequest_t, inputClusterList.pClusterList)),              /* offsetToPtr1     */ \
                (AppZdpOfs( zbEndDeviceBindRequest_t, inputClusterList.cNumClusters)),              /* offsetToCount1   */ \
                (IndirectMbrSizeof(zbEndDeviceBindRequest_t, inputClusterList.pClusterList)),       /* array1ElementLen */ \
                (AppZdpSize(zbEndDeviceBindRequest_t)),                                             /* offsetToMsgData  */ \
                (AppZdpOfs( zbEndDeviceBindRequest_t, outputClusterList.pClusterList)),             /* offsetToPtr2     */ \
                (AppZdpOfs( zbEndDeviceBindRequest_t, outputClusterList.cNumClusters)),             /* offsetToCount2   */ \
                (IndirectMbrSizeof(zbEndDeviceBindRequest_t, outputClusterList.pClusterList)))      /* array2ElementLen */

/* Used for ZDP-Simple_Desc_Store.Request (A2 1A). */
FmtTwoArrayPtrs(mFmtZdpSimpleDescStoreReq_c,                                                                            /* name             */ \
                (AppZdpSize(zbSimpleDescriptorStoreRequest_t)),                                                         /* hdrLen           */ \
                (AppZdpOfs( zbSimpleDescriptorStoreRequest_t, simpleDescriptor.inputClusters.pClusterList)),            /* offsetToPtr1     */ \
                (AppZdpOfs( zbSimpleDescriptorStoreRequest_t, simpleDescriptor.inputClusters.cNumClusters)),            /* offsetToCount1   */ \
                (IndirectMbrSizeof(zbSimpleDescriptorStoreRequest_t, simpleDescriptor.inputClusters.pClusterList)),     /* array1ElementLen */ \
                (AppZdpSize(zbSimpleDescriptorStoreRequest_t)),                                                         /* offsetToMsgData  */ \
                (AppZdpOfs( zbSimpleDescriptorStoreRequest_t, simpleDescriptor.outputClusters.pClusterList)),           /* offsetToPtr2     */ \
                (AppZdpOfs( zbSimpleDescriptorStoreRequest_t, simpleDescriptor.outputClusters.cNumClusters)),           /* offsetToCount2   */ \
                (IndirectMbrSizeof(zbSimpleDescriptorStoreRequest_t, simpleDescriptor.outputClusters.pClusterList)))    /* array2ElementLen */

/* Used for ZTC-RegisterEndPoint.Request (A3 0B) and */
/*          ZTC-GetEndPointDescription   (A3 0D). */
/* Since these are ZTC utilities, not normal SAP Handler messages, this */
/* table entry is not referenced by the usual format table (the Fmt() macro */
/* entries). Instead, it is named directly by ZtcMsgRegisterEndPoint() */
/* and ZtcMsgGetEndPointDescription(). */
FmtTwoArrayPtrs(mFmtZTCRegisterEndPointReq_c,                                                       /* name             */ \
                (sizeof(zbSimpleDescriptorPackageStore_t)),                                         /* hdrLen           */ \
                (MbrOfs( zbSimpleDescriptorPackageStore_t, inputClusters.pClusterList)),            /* offsetToPtr1     */ \
                (MbrOfs( zbSimpleDescriptorPackageStore_t, inputClusters.cNumClusters)),            /* offsetToCount1   */ \
                (IndirectMbrSizeof(zbSimpleDescriptorPackageStore_t, inputClusters.pClusterList)),  /* array1ElementLen */ \
                (sizeof(zbSimpleDescriptorPackageStore_t)),                                         /* offsetToMsgData  */ \
                (MbrOfs( zbSimpleDescriptorPackageStore_t, outputClusters.pClusterList)),           /* offsetToPtr2     */ \
                (MbrOfs( zbSimpleDescriptorPackageStore_t, outputClusters.cNumClusters)),           /* offsetToCount2   */ \
                (IndirectMbrSizeof(zbSimpleDescriptorPackageStore_t, outputClusters.pClusterList))) /* array2ElementLen */

#endif /* gBeeStackIncluded_d */
/* Use for MacData.Indication (86 01). */
FmtTwoArrayPtrs(mFmtMacDataInd_c,                                                       /* name             */ \
                (sizeof(mcpsDataInd_t)),                                                /* hdrLen           */ \
                (MbrOfs(mcpsDataInd_t, pMsdu)),                                         /* offsetToPtr      */ \
                (MbrOfs(mcpsDataInd_t, msduLength)),                                    /* offsetToCount    */ \
                (MbrSizeof(mcpsDataInd_t, msduLength)),                                 /* arrayElementLen  */ \
                (sizeof(mcpsDataInd_t)),                                                /* offsetToMsgData  */ \
                gZero_c,                                                                /* offsetToPtr2     */ \
                gZero_c,                                                                /* offsetToCount2   */ \
                gZero_c)                                                                /* array2ElementLen */

/* Used for Promiscuous.Ind (86 03). */
FmtTwoArrayPtrs(mFmtMcpsPromInd_c,                                                      /* name             */ \
                (sizeof(mcpsPromInd_t)),                                                /* hdrLen           */ \
                (MbrOfs(mcpsPromInd_t, pMsdu)),                                         /* offsetToPtr      */ \
                (MbrOfs(mcpsPromInd_t, msduLength)),                                    /* offsetToCount    */ \
                (IndirectMbrSizeof(mcpsPromInd_t, pMsdu)),                              /* arrayElementLen  */ \
                (sizeof(mcpsPromInd_t)),                                                /* offsetToMsgData  */ \
                gZero_c,                                                                /* offsetToPtr2     */ \
                gZero_c,                                                                /* offsetToCount2   */ \
                gZero_c)                                                                /* array2ElementLen */

/* Used for MacData.Request (87 00). */
FmtTwoArrayPtrs(mFmtMacDataReq_c,                                                       /* name             */ \
                (sizeof(mcpsDataReq_t)),                                                /* hdrLen           */ \
                (MbrOfs(mcpsDataReq_t, pMsdu)),                                         /* offsetToPtr      */ \
                (MbrOfs(mcpsDataReq_t, msduLength)),                                    /* offsetToCount    */ \
                (IndirectMbrSizeof(mcpsDataReq_t, pMsdu)),                              /* arrayElementLen  */ \
                (sizeof(mcpsDataReq_t)),                                                /* offsetToMsgData  */ \
                gZero_c,                                                                /* offsetToPtr2     */ \
                gZero_c,                                                                /* offsetToCount2   */ \
                gZero_c)                                                                /* array2ElementLen */

/* Used for NLME-NETWORK-DISCOVERY.Confirm (97 41). */
#if gBeeStackIncluded_d
FmtTwoArrayPtrs(mFmtNlmeNetworkDiscoveryConf_c,                                         /* name             */ \
                (sizeof(nlmeNetworkDiscoveryConf_t)),                                   /* hdrLen           */ \
                (MbrOfs(nlmeNetworkDiscoveryConf_t, pNetworkDescriptor)),               /* offsetToPtr      */ \
                (MbrOfs(nlmeNetworkDiscoveryConf_t, networkCount)),                     /* offsetToCount    */ \
                (IndirectMbrSizeof(nlmeNetworkDiscoveryConf_t, pNetworkDescriptor)),    /* arrayElementLen  */ \
                (sizeof(nlmeNetworkDiscoveryConf_t)),                                   /* offsetToMsgData  */ \
                gZero_c,                                                                /* offsetToPtr2     */ \
                gZero_c,                                                                /* offsetToCount2   */ \
                gZero_c)                                                                /* array2ElementLen */

/* Used for NLDE-DATA.Request (9A 33). */
FmtTwoArrayPtrs(mFmtNldeDataReq_c,                                                      /* name             */ \
                (sizeof(nldeDataReq_t)),                                                /* hdrLen           */ \
                (MbrOfs(nldeDataReq_t, pNsdu)),                                         /* offsetToPtr      */ \
                (MbrOfs(nldeDataReq_t, nsduLength)),                                    /* offsetToCount    */ \
                (IndirectMbrSizeof(nldeDataReq_t, pNsdu)),                              /* arrayElementLen  */ \
                (sizeof(mcpsDataReq_t) + 8),                                            /* offsetToMsgData  */ \
                gZero_c,                                                                /* offsetToPtr2     */ \
                gZero_c,                                                                /* offsetToCount2   */ \
                gZero_c)                                                                /* array2ElementLen */

/* Used for NLDE-DATA.Indication (9B 40). */
FmtTwoArrayPtrs(mFmtNldeDataInd_c,                                                      /* name             */ \
                (sizeof(nldeDataIndication_t)),                                         /* hdrLen           */ \
                (MbrOfs(nldeDataIndication_t, pNsdu)),                                  /* offsetToPtr      */ \
                (MbrOfs(nldeDataIndication_t, nsduLength)),                             /* offsetToCount    */ \
                (IndirectMbrSizeof(nldeDataIndication_t, pNsdu)),                       /* arrayElementLen  */ \
                (sizeof(nldeDataIndication_t)),                                         /* offsetToMsgData  */ \
                gZero_c,                                                                /* offsetToPtr2     */ \
                gZero_c,                                                                /* offsetToCount2   */ \
                gZero_c)                                                                /* array2ElementLen */

/* Used for APSDE-DATA.Request (9C 00). */
/* *warning* The 0x2C magic number in the offsetToMsgData works. It is derived */
/* from the lengths of some other network layer headers that will be inserted */
/* between the zbApsdeDataReq_t and the data. Note that 0x2C counts from the */
/* beginning of the zbApsdeDataReq_t, not from the MsgType. */
FmtTwoArrayPtrs(mFmtApsdeDataReq_c,                                                     /* name             */ \
                (sizeof(zbApsdeDataReq_t)),                                             /* hdrLen           */ \
                (MbrOfs(zbApsdeDataReq_t, pAsdu)),                                      /* offsetToPtr      */ \
                (MbrOfs(zbApsdeDataReq_t, asduLength)),                                 /* offsetToCount    */ \
                (IndirectMbrSizeof(zbApsdeDataReq_t, pAsdu)),                           /* arrayElementLen  */ \
                0x2C+3,                                                                 /* offsetToMsgData  */ \
                gZero_c,                                                                /* offsetToPtr2     */ \
                gZero_c,                                                                /* offsetToCount2   */ \
                gZero_c)                                                                /* array2ElementLen */

/*Used for InterPan-DATA.Request (A5 01) */
FmtTwoArrayPtrs(mFmtInterPanDataReq_c,                                                       /* name             */ \
                (sizeof(zbInterPanDataReq_t) - MbrSizeof(zbInterPanDataReq_t, pAsduHandle)),                                              /* hdrLen           */ \
                (MbrOfs(zbInterPanDataReq_t, pAsdu)),                                        /* offsetToPtr      */ \
                (MbrOfs(zbInterPanDataReq_t, asduLength)),                                   /* offsetToCount    */ \
                (IndirectMbrSizeof(zbInterPanDataReq_t, pAsdu)),                             /* arrayElementLen  */ \
                0x2C+3,                                                                      /* offsetToMsgData  */ \
                gZero_c,                                                                     /* offsetToPtr2     */ \
                gZero_c,                                                                     /* offsetToCount2   */ \
                gZero_c)                                                                     /* array2ElementLen */

/*Used for InterPan-DATA.Request (A5 01) */
FmtTwoArrayPtrs(mFmtInterPanDataReqSap_c,                                               /* name             */ \
                (sizeof(zbInterPanDataReq_t)),                                          /* hdrLen           */ \
                (MbrOfs(zbInterPanDataReq_t, pAsdu)),                                   /* offsetToPtr      */ \
                (MbrOfs(zbInterPanDataReq_t, asduLength)),                              /* offsetToCount    */ \
                (IndirectMbrSizeof(zbInterPanDataReq_t, pAsdu)),                        /* arrayElementLen  */ \
                0x2C+3,                                                                 /* offsetToMsgData  */ \
                gZero_c,                                                                /* offsetToPtr2     */ \
                gZero_c,                                                                /* offsetToCount2   */ \
                gZero_c)                                                                /* array2ElementLen */


/* Used for InterPan-DATA.Indication (A6 02). */
FmtTwoArrayPtrs(mFmtInterPanDataInd_c,                                                  /* name             */ \
                (sizeof(zbInterPanDataIndication_t)),                                   /* hdrLen           */ \
                (MbrOfs(zbInterPanDataIndication_t, pAsdu)),                            /* offsetToPtr      */ \
                (MbrOfs(zbInterPanDataIndication_t, asduLength)),                       /* offsetToCount    */ \
                (IndirectMbrSizeof(zbInterPanDataIndication_t, pAsdu)),                 /* arrayElementLen  */ \
                (sizeof(zbInterPanDataIndication_t)),                                   /* offsetToMsgData  */ \
                gZero_c,                                                                /* offsetToPtr2     */ \
                gZero_c,                                                                /* offsetToCount2   */ \
                gZero_c)                                                                /* array2ElementLen */


/* Used for APSDE-DATA.Indication (9D 01). */
FmtTwoArrayPtrs(mFmtApsdeDataInd_c,                                                     /* name             */ \
                (sizeof(zbApsdeDataIndication_t)),                                      /* hdrLen           */ \
                (MbrOfs(zbApsdeDataIndication_t, pAsdu)),                               /* offsetToPtr      */ \
                (MbrOfs(zbApsdeDataIndication_t, asduLength)),                          /* offsetToCount    */ \
                (IndirectMbrSizeof(zbApsdeDataIndication_t, pAsdu)),                    /* arrayElementLen  */ \
                (sizeof(zbApsdeDataIndication_t)),                                      /* offsetToMsgData  */ \
                gZero_c,                                                                /* offsetToPtr2     */ \
                gZero_c,                                                                /* offsetToCount2   */ \
                gZero_c)                                                                /* array2ElementLen */

/* Used for AFDE_DATA.Indication (9E 02). */
FmtTwoArrayPtrs(mFmtAfdeDataInd_c,                                                      /* name             */ \
                (sizeof(zbApsdeDataIndication_t)),                                      /* hdrLen           */ \
                (MbrOfs(zbApsdeDataIndication_t, pAsdu)),                               /* offsetToPtr      */ \
                (MbrOfs(zbApsdeDataIndication_t, asduLength)),                          /* offsetToCount    */ \
                (sizeof(uint8_t)),                                                      /* arrayElementLen  */ \
                (sizeof(zbApsdeDataIndication_t)),                                      /* offsetToMsgData  */ \
                gZero_c,                                                                /* offsetToPtr2     */ \
                gZero_c,                                                                /* offsetToCount2   */ \
                gZero_c)                                                                /* array2ElementLen */

/* Used for ZDP-NLME-NETWORK-DISCOVERY.Confirm (A0 41). */
FmtTwoArrayPtrs(mFmtZdpNlmeNwkDiscCnf_c,                                                /* name             */ \
                (sizeof(nlmeNetworkDiscoveryConf_t)),                                   /* hdrLen           */ \
                (MbrOfs(nlmeNetworkDiscoveryConf_t, pNetworkDescriptor)),               /* offsetToPtr      */ \
                (MbrOfs(nlmeNetworkDiscoveryConf_t, networkCount)),                     /* offsetToCount    */ \
                (IndirectMbrSizeof(nlmeNetworkDiscoveryConf_t, pNetworkDescriptor)),    /* arrayElementLen  */ \
                (sizeof(nlmeNetworkDiscoveryConf_t)),                                   /* offsetToMsgData  */ \
                gZero_c,                                                                /* offsetToPtr2     */ \
                gZero_c,                                                                /* offsetToCount2   */ \
                gZero_c)                                                                /* array2ElementLen */

#endif /* gBeeStackIncluded_d */
#undef FmtTwoArrayPtrs

/****************************************************************************/

/* Tables of indexes and copy functions, by copy direction. */
#ifndef PktFromMsgFunction
#define PktFromMsgFunction(index, pFunction)
#endif
#ifndef MsgFromPktFunction
#define MsgFromPktFunction(index, pFunction)
#endif

PktFromMsgFunction(mZtcPktFromMsgUnused_c,             ZtcPktFromMsgUnused)
PktFromMsgFunction(mZtcPktFromMsgSimple_c,             ZtcPktFromMsgSimple)
#if gBeeStackIncluded_d
PktFromMsgFunction(mZtcPktFromMsgHdrAndData_c,         ZtcPktFromMsgHdrAndData)
PktFromMsgFunction(mZtcPktFromMsgExtSimpleDescResp_c,            ZtcPktFromMsgExtSimpleDescriptorResp)
MsgFromPktFunction(mZtcMsgFromPktExtSimpleDescResp_c,            ZtcMsgFromPktExtSimpleDescriptorResp)
PktFromMsgFunction(mZtcPktFromMsgExtActiveEPResp_c,              ZtcPktFromMsgExtActiveEPResp)
MsgFromPktFunction(mZtcMsgFromPktExtActiveEPResp_c,              ZtcMsgFromPktExtActiveEPResp)
#endif /* gBeeStackIncluded_d */
#if gSAPMessagesEnableMlme_d || gSAPMessagesEnableNlme_d
PktFromMsgFunction(mZtcPktFromMsgMacScanCnf_c,         ZtcPktFromMsgMacScanCnf)
#endif
#if gSAPMessagesEnableMlme_d 
PktFromMsgFunction(mZtcPktFromMsgMacBeaconNotifyInd_c, ZtcPktFromMsgMacBeaconNotifyInd)
#endif
#if mZtcLenCodeAndPtrFormat_d
PktFromMsgFunction(mZtcPktFromMsgLenCodeAndPtr_c,      ZtcPktFromMsgLenCodeAndPtr)
#endif
PktFromMsgFunction(mZtcPktFromMsgTwoArrayPtrs_c,       ZtcPktFromMsgTwoArrayPtrs)

MsgFromPktFunction(mZtcMsgFromPktUnused_c,             ZtcMsgFromPktUnused)
MsgFromPktFunction(mZtcMsgFromPktSimple_c,             ZtcMsgFromPktSimple)
#if gBeeStackIncluded_d
MsgFromPktFunction(mZtcMsgFromPktHdrAndData_c,         ZtcMsgFromPktHdrAndData)
#endif /* gBeeStackIncluded_d */
#if gSAPMessagesEnableMlme_d 
MsgFromPktFunction(mZtcMsgFromPktMacScanReq_c,         ZtcMsgFromPktMacScanReq)
PktFromMsgFunction(mZtcPktFromMsgMacScanReq_c,         ZtcPktFromMsgMacScanReq)
#endif
#if mZtcLenCodeAndPtrFormat_d
MsgFromPktFunction(mZtcMsgFromPktLenCodeAndPtr_c,      ZtcMsgFromPktLenCodeAndPtr)
#endif
MsgFromPktFunction(mZtcMsgFromPktTwoArrayPtrs_c,       ZtcMsgFromPktTwoArrayPtrs)
MsgFromPktFunction(mZtcMsgFromPktApsde_c,              ZtcMsgFromPktApsde)

#if gSAPMessagesEnableInterPan_d 
MsgFromPktFunction(mZtcMsgFromPktInterPanReq_c,        ZtcMsgFromPktInterPanDataReq)
PktFromMsgFunction(mZtcPktFromMsgInterPanReq_c,        ZtcPktFromMsgInterPanDataReq)
#endif

#if gSAPMessagesEnableHc_d 


MsgFromPktFunction(mZtcMsgFromPktHc_c,     ZtcMsgFromPktHc)
PktFromMsgFunction(mZtcPktFromMsgHc_c,     ZtcPktFromMsgHc)
#endif



#undef PktFromMsgFunction
#undef MsgFromPktFunction

/****************************************************************************/

/* Main format table. Each entry in the message info tables includes an
 * enum that is used as an index into this table. Each row in this table
 * represents a named format conversion type.
 * Each table entry contains two (function pointer, index) pairs. The
 * function pointer points to the function to call to perform the copy,
 * packet-from-message or message-from-packet. The index is used by that
 * function in one of the tables above (each function knows which table
 * it uses) to find parameters for this particular copy operation.
 *
 * The macro format is:
 *  Column 1    name: Identifier naming this format.
 *  Column 2    pktFromMsgFunc: Function to use to copy client-packet-
 *                from-SAP-Handler-message.
 *  Column 3    pktFromMsgIndex: Index to the packet-from-msg copy
 *                function's parameters.
 *  Column 4    msgFromPktFunc: Function to use to copy message-from-packet.
 *  Column 3    msgFromPktIndex: Index to the msg-from-packet copy
 *                function's parameters.
 *
 * For both sync and async message types,
 *      message-from-packet copy functions are used to inject messages
 *      from the external client (the Test Tool) into the SAP Handlers.
 *
 * For async message types,
 *      packet-from-message copy functions are used for sending monitor
 *      copies of messsages that pass through SAP Handler out to the
 *      external client.
 *
 * For sync message types,
 *      packet-from-message copy function are used to synthesize
 *      confirmations or responses to requests. The request may
 *      originate from within BeeStack or from the external client
 *      via Ztc.
 *
 * Used to define ztcFmtInfoTable[].
 */

#ifndef Fmt
#define Fmt(name, pktFromMsgFuncIndex, pktFromMsgParamIndex, \
            msgFromPktFuncIndex, msgFromPktParamIndex)
#endif

/* Used for the majority of messages. The simple format does not need */
/* parameter table indexes. */
Fmt(gZtcMsgFmtSimple_c,                 /* name             */ \
    mZtcPktFromMsgSimple_c,             /* pktFromMsgFunc   */ \
    gUnused_c,                          /* pktFromMsgIndex  */ \
    mZtcMsgFromPktSimple_c,             /* msgFromPktFunc   */ \
    gUnused_c)                          /* msgFromPktIndex  */

#if gSAPMessagesEnableMlme_d    
/* Used for MacScan.Confirm (84 04).  */
Fmt(gZtcMsgFmtMacBeaconNotifyInd_c,     /* name             */ \
    mZtcPktFromMsgMacBeaconNotifyInd_c, /* pktFromMsgFunc   */ \
    gUnused_c,                          /* pktFromMsgIndex  */ \
    mZtcMsgFromPktSimple_c,             /* msgFromPktFunc   */ \
    gUnused_c)                          /* msgFromPktIndex  */

/* Used for MacScan.Request (85 08).  */
Fmt(gZtcMsgFmtMacScanReq_c,             /* name             */ \
    mZtcPktFromMsgMacScanReq_c,         /* pktFromMsgFunc   */ \
    gUnused_c,                          /* pktFromMsgIndex  */ \
    mZtcMsgFromPktMacScanReq_c,         /* msgFromPktFunc   */ \
    gUnused_c)                          /* msgFromPktIndex   */

#endif
#if gSAPMessagesEnableMlme_d || gSAPMessagesEnableNlme_d   
/* Used for MacScan.Confirm (84 0B). and for the NLME-ED-scan.Confirm    */
Fmt(gZtcMsgFmtMacScanCnf_c,             /* name             */ \
    mZtcPktFromMsgMacScanCnf_c,         /* pktFromMsgFunc   */ \
    gUnused_c,                          /* pktFromMsgIndex  */ \
    mZtcMsgFromPktSimple_c,             /* msgFromPktFunc   */ \
    gUnused_c)                          /* msgFromPktIndex  */
#endif    


/* Used for MacGetPIBAttribute.Request (85 03), aka MLME-GET.request. */
#if mZtcLenCodeAndPtrFormat_d
Fmt(gZtcMsgFmtMacGetReq_c,              /* name             */ \
    mZtcPktFromMsgLenCodeAndPtr_c,      /* pktFromMsgFunc   */ \
    mFmtMacGetReq_c,                    /* pktFromMsgIndex  */ \
    mZtcMsgFromPktLenCodeAndPtr_c,      /* msgFromPktFunc   */ \
    mFmtMacGetReq_c)                    /* msgFromPktIndex  */
#endif                                  /* #if mZtcLenCodeAndPtrFormat_d */

/* Used for MacSetPIBAttribute.Request (85 09), aka MLME-SET.request. */
#if mZtcLenCodeAndPtrFormat_d
Fmt(gZtcMsgFmtMacSetReq_c,              /* name             */ \
    mZtcPktFromMsgLenCodeAndPtr_c,      /* pktFromMsgFunc   */ \
    mFmtMacSetReq_c,                    /* pktFromMsgIndex  */ \
    mZtcMsgFromPktLenCodeAndPtr_c,      /* msgFromPktFunc   */ \
    mFmtMacSetReq_c)                    /* msgFromPktIndex  */
#endif                                  /* #if mZtcLenCodeAndPtrFormat_d */

/* Used for MacData.Indication (86 01). */
Fmt(gZtcMsgFmtMacDataInd_c,             /* name             */ \
    mZtcPktFromMsgTwoArrayPtrs_c,       /* pktFromMsgFunc   */ \
    mFmtMacDataInd_c,                   /* pktFromMsgIndex  */ \
    mZtcMsgFromPktTwoArrayPtrs_c,       /* msgFromPktFunc   */ \
    mFmtMacDataInd_c)                   /* msgFromPktIndex  */

/* Used for Promiscuous.Ind (86 03). */
Fmt(gZtcMsgFmtMcpsPromInd_c,            /* name             */ \
    mZtcPktFromMsgTwoArrayPtrs_c,       /* pktFromMsgFunc   */ \
    mFmtMcpsPromInd_c,                  /* pktFromMsgIndex  */ \
    mZtcMsgFromPktTwoArrayPtrs_c,       /* msgFromPktFunc   */ \
    mFmtMcpsPromInd_c)                  /* msgFromPktIndex  */

/* Used for MacData.Request (87 00). */
Fmt(gZtcMsgFmtMacDataReq_c,             /* name             */ \
    mZtcPktFromMsgTwoArrayPtrs_c,       /* pktFromMsgFunc   */ \
    mFmtMacDataReq_c,                   /* pktFromMsgIndex  */ \
    mZtcMsgFromPktTwoArrayPtrs_c,       /* msgFromPktFunc   */ \
    mFmtMacDataReq_c)                   /* msgFromPktIndex  */

#if gBeeStackIncluded_d
/* Used for NLME-NETWORK-DISCOVERY.Confirm (97 41). */
Fmt(gZtcMsgFmtNlmeNwkDiscCnf_c,         /* name             */ \
    mZtcPktFromMsgTwoArrayPtrs_c,       /* pktFromMsgFunc   */ \
    mFmtNlmeNetworkDiscoveryConf_c,     /* pktFromMsgIndex  */ \
    mZtcMsgFromPktTwoArrayPtrs_c,       /* msgFromPktFunc   */ \
    mFmtNlmeNetworkDiscoveryConf_c)     /* msgFromPktIndex  */

/* Used for NLDE_DATA.Request (9A 33) */
Fmt(gZtcMsgFmtNldeDataReq_c,            /* name             */ \
    mZtcPktFromMsgTwoArrayPtrs_c,       /* pktFromMsgFunc   */ \
    mFmtNldeDataReq_c,                  /* pktFromMsgIndex  */ \
    mZtcMsgFromPktTwoArrayPtrs_c,       /* msgFromPktFunc   */ \
    mFmtNldeDataReq_c)                  /* msgFromPktIndex  */

/* Used for NLDE-DATA.Indication (9B 40). */
Fmt(gZtcMsgFmtNldeDataInd_c,            /* name             */ \
    mZtcPktFromMsgTwoArrayPtrs_c,       /* pktFromMsgFunc   */ \
    mFmtNldeDataInd_c,                  /* pktFromMsgIndex  */ \
    mZtcMsgFromPktTwoArrayPtrs_c,       /* msgFromPktFunc   */ \
    mFmtNldeDataInd_c)                  /* msgFromPktIndex  */

/* Used for APSDE-DATA.Request (9C 00). */
Fmt(gZtcMsgFmtApsdeDataReq_c,           /* name             */ \
    mZtcPktFromMsgTwoArrayPtrs_c,       /* pktFromMsgFunc   */ \
    mFmtApsdeDataReq_c,                 /* pktFromMsgIndex  */ \
    mZtcMsgFromPktApsde_c,              /* msgFromPktFunc   */ \
    mFmtApsdeDataReq_c)                 /* msgFromPktIndex  */

/* Used for APSDE-DATA.Indication (9D 01). */
Fmt(gZtcMsgFmtApsdeDataInd_c,           /* name             */ \
    mZtcPktFromMsgTwoArrayPtrs_c,       /* pktFromMsgFunc   */ \
    mFmtApsdeDataInd_c,                 /* pktFromMsgIndex  */ \
    mZtcMsgFromPktTwoArrayPtrs_c,       /* msgFromPktFunc   */ \
    mFmtApsdeDataInd_c)                 /* msgFromPktIndex  */
#if gSAPMessagesEnableInterPan_d
/* Used for InterPan-DATA.Request (A5 00). */
Fmt(gZtcMsgFmtInterPanDataReq_c,        /* name             */ \
    mZtcPktFromMsgInterPanReq_c,        /* pktFromMsgFunc   */ \
    mFmtInterPanDataReq_c,              /* pktFromMsgIndex  */ \
    mZtcMsgFromPktInterPanReq_c,        /* msgFromPktFunc   */ \
    mFmtInterPanDataReqSap_c)           /* msgFromPktIndex  */

/* Used for InterPan-DATA.Indication (A6 02). */
Fmt(gZtcMsgFmtInterPanDataIndication_c, /* name             */ \
    mZtcPktFromMsgTwoArrayPtrs_c,       /* pktFromMsgFunc   */ \
    mFmtInterPanDataInd_c,              /* pktFromMsgIndex  */ \
    mZtcMsgFromPktTwoArrayPtrs_c,       /* msgFromPktFunc   */ \
    mFmtInterPanDataInd_c)              /* msgFromPktIndex  */

#endif
/* Used for AFDE_DATA.Indication (9E 02). */
Fmt(gZtcMsgFmtAfdeDataInd_c,            /* name             */ \
    mZtcPktFromMsgTwoArrayPtrs_c,       /* pktFromMsgFunc   */ \
    mFmtAfdeDataInd_c,                  /* pktFromMsgIndex  */ \
    mZtcMsgFromPktTwoArrayPtrs_c,       /* msgFromPktFunc   */ \
    mFmtAfdeDataInd_c)                  /* msgFromPktIndex  */

/* Used for ZDP-NLME-NETWORK-DISCOVERY.Confirm (A0 41). */
Fmt(gZtcMsgFmtZdpNlmeNwkDiscCnf_c,      /* name             */ \
    mZtcPktFromMsgTwoArrayPtrs_c,       /* pktFromMsgFunc   */ \
    mFmtZdpNlmeNwkDiscCnf_c,            /* pktFromMsgIndex  */ \
    mZtcMsgFromPktUnused_c,             /* msgFromPktFunc   */ \
    gUnused_c)                          /* msgFromPktIndex  */

/* Used for ZDP-NWK_addr.response (A0 80) and ZDP-IEEE_addr.response (A0 81). */
Fmt(gZtcMsgFmtNwkAddrResp_c,            /* name             */ \
    mZtcPktFromMsgHdrAndData_c,         /* pktFromMsgFunc   */ \
    mFmtNwkAddrResp_c,                  /* pktFromMsgIndex  */ \
    mZtcMsgFromPktHdrAndData_c,         /* msgFromPktFunc   */ \
    mFmtNwkAddrResp_c)                  /* msgFromPktIndex  */

/* Used for ZDP-SimpleDescriptor.Response (A0 84). */
Fmt(gZtcMsgFmtZdpSimpleDescResp_c,      /* name             */ \
    mZtcPktFromMsgHdrAndData_c,         /* pktFromMsgFunc   */ \
    mFmtZdpSimpleDescResp_c,            /* pktFromMsgIndex  */ \
    mZtcMsgFromPktHdrAndData_c,         /* msgFromPktFunc   */ \
    mFmtZdpSimpleDescResp_c)            /* msgFromPktIndex  */
    
Fmt(gZtcMsgFmtZdpExtSimpleDescResp_c,   /* name             */ \
    mZtcPktFromMsgExtSimpleDescResp_c,  /* pktFromMsgFunc   */ \
    gUnused_c,                          /* pktFromMsgIndex  */ \
    mZtcMsgFromPktExtSimpleDescResp_c,  /* msgFromPktFunc   */ \
    gUnused_c)                          /* msgFromPktIndex  */
    
Fmt(gZtcMsgFmtZdpExtActiveEPResp_c,     /* name             */ \
    mZtcPktFromMsgExtActiveEPResp_c,    /* pktFromMsgFunc   */ \
    gUnused_c,                          /* pktFromMsgIndex  */ \
    mZtcMsgFromPktExtActiveEPResp_c,     /* msgFromPktFunc   */ \
    gUnused_c)      

/* Used for ZDP-Active_EP_rsp.response (A0 85). */
Fmt(gZtcMsgFmtZdpActiveEPResp_c,        /* name             */ \
    mZtcPktFromMsgHdrAndData_c,         /* pktFromMsgFunc   */ \
    mFmtZdpActiveEPResp_c,              /* pktFromMsgIndex  */ \
    mZtcMsgFromPktHdrAndData_c,         /* msgFromPktFunc   */ \
    mFmtZdpActiveEPResp_c)              /* msgFromPktIndex  */

/* Used for ZDP-Match_Desc_resp.response (A0 86). */
Fmt(gZtcMsgFmtZdpMatchDescResp_c,       /* name             */ \
    mZtcPktFromMsgHdrAndData_c,         /* pktFromMsgFunc   */ \
    mFmtZdpMatchDescResp_c,             /* pktFromMsgIndex  */ \
    mZtcMsgFromPktHdrAndData_c,         /* msgFromPktFunc   */ \
    mFmtZdpMatchDescResp_c)             /* msgFromPktIndex  */

/* Used for ZDP-Complex_Desc_rsp (A0 90). */
Fmt(gZtcMsgFmtComplexDescResp_c,        /* name             */ \
    mZtcPktFromMsgHdrAndData_c,         /* pktFromMsgFunc   */ \
    mFmtZdpComplexDescResp_c,           /* pktFromMsgIndex  */ \
    mZtcMsgFromPktHdrAndData_c,         /* msgFromPktFunc   */ \
    mFmtZdpComplexDescResp_c)           /* msgFromPktIndex  */

/* Used for ZDP-UserDescriptor.Response (A0 91). */
Fmt(gZtcMsgFmtUserDescResp_c,           /* name             */ \
    mZtcPktFromMsgHdrAndData_c,         /* pktFromMsgFunc   */ \
    mFmtZdpUserDescResp_c,              /* pktFromMsgIndex  */ \
    mZtcMsgFromPktHdrAndData_c,         /* msgFromPktFunc   */ \
    mFmtZdpUserDescResp_c)              /* msgFromPktIndex  */

/* Used for ZDP-Bind_Register.response (A0 A3). */
Fmt(gZtcMsgFmtZdpBindRegResp_c,         /* name             */ \
    mZtcPktFromMsgHdrAndData_c,         /* pktFromMsgFunc   */ \
    mFmtZdpBindRegResp_c,               /* pktFromMsgIndex  */ \
    mZtcMsgFromPktHdrAndData_c,         /* msgFromPktFunc   */ \
    mFmtZdpBindRegResp_c)               /* msgFromPktIndex  */

/* Used for ZDP-Recover_Bind_Table.response (A0 A8). */
Fmt(gZtcMsgFmtZdpRecoverBindTblResp_c,  /* name             */ \
    mZtcPktFromMsgHdrAndData_c,         /* pktFromMsgFunc   */ \
    mFmtZdpRecoverBindTblResp_c,        /* pktFromMsgIndex  */ \
    mZtcMsgFromPktHdrAndData_c,         /* msgFromPktFunc   */ \
    mFmtZdpRecoverBindTblResp_c)        /* msgFromPktIndex  */

/* Used for ZDP-Recover_Source_Bind.response (A0 AA). */
Fmt(gZtcMsgFmtZdpRecoverSrcBindResp_c,  /* name             */ \
    mZtcPktFromMsgHdrAndData_c,         /* pktFromMsgFunc   */ \
    mFmtZdpRecoverSrcBindResp_c,        /* pktFromMsgIndex  */ \
    mZtcMsgFromPktHdrAndData_c,         /* msgFromPktFunc   */ \
    mFmtZdpRecoverSrcBindResp_c)        /* msgFromPktIndex  */

/* Used for ZDP-Mgmt_Nwk_Disc.Response (A0 B0). */
Fmt(gZtcMsgFmtZdpMgmtNwkDiscResp_c,     /* name             */ \
    mZtcPktFromMsgHdrAndData_c,         /* pktFromMsgFunc   */ \
    mFmtZdpMgmtNwkDiscResp_c,           /* pktFromMsgIndex  */ \
    mZtcMsgFromPktHdrAndData_c,         /* msgFromPktFunc   */ \
    mFmtZdpMgmtNwkDiscResp_c)           /* msgFromPktIndex  */

/* Used for ZDP-Mgmt_Lqi.Response (A0 B1). */
Fmt(gZtcMsgFmtZdpMgmtLqiResp_c,         /* name             */ \
    mZtcPktFromMsgHdrAndData_c,         /* pktFromMsgFunc   */ \
    mFmtZdpMgmtLqiResp_c,               /* pktFromMsgIndex  */ \
    mZtcMsgFromPktHdrAndData_c,         /* msgFromPktFunc   */ \
    mFmtZdpMgmtLqiResp_c)               /* msgFromPktIndex  */

/* Used for ZDP-Mgmt_Rtg.Response (A0 B2). */
Fmt(gZtcMsgFmtZdpMgmtRtgResp_c,         /* name             */ \
    mZtcPktFromMsgHdrAndData_c,         /* pktFromMsgFunc   */ \
    mFmtZdpMgmtRtgResp_c,               /* pktFromMsgIndex  */ \
    mZtcMsgFromPktHdrAndData_c,         /* msgFromPktFunc   */ \
    mFmtZdpMgmtRtgResp_c)               /* msgFromPktIndex  */

/* Used for ZDP-Mgmt_Bind.Response (A0 B3). */
Fmt(gZtcMsgFmtZdpMgmtBindResp_c,        /* name             */ \
    mZtcPktFromMsgHdrAndData_c,         /* pktFromMsgFunc   */ \
    mFmtZdpMgmtBindResp_c,              /* pktFromMsgIndex  */ \
    mZtcMsgFromPktHdrAndData_c,         /* msgFromPktFunc   */ \
    mFmtZdpMgmtBindResp_c)              /* msgFromPktIndex  */

/* Used for ZDP-Mgmt_Cache.response (A0 B7). */
Fmt(gZtcMsgFmtZdpMgmtCacheResp_c,       /* name             */ \
    mZtcPktFromMsgHdrAndData_c,         /* pktFromMsgFunc   */ \
    mFmtZdpMgmtCacheResp_c,             /* pktFromMsgIndex  */ \
    mZtcMsgFromPktHdrAndData_c,         /* msgFromPktFunc   */ \
    mFmtZdpMgmtCacheResp_c)             /* msgFromPktIndex  */

/* Used for ZDP-Mgmt_NwkUpdate.Notify (A0 B8). */
Fmt(gZtcMsgFmtZdpMgmtNwkUpdateNotify_c, /* name             */ \
    mZtcPktFromMsgHdrAndData_c,         /* pktFromMsgFunc   */ \
    mFmtZdpMgmtNwkUpdateNotify_c,       /* pktFromMsgIndex  */ \
    mZtcMsgFromPktHdrAndData_c,         /* msgFromPktFunc   */ \
    mFmtZdpMgmtNwkUpdateNotify_c)       /* msgFromPktIndex  */

/* Used for ZDP-Match_Desc_req.Request (A2 06). */
Fmt(gZtcMsgFmtZdpMatchDescReq_c,        /* name             */ \
    mZtcPktFromMsgTwoArrayPtrs_c,       /* pktFromMsgFunc   */ \
    mFmtZdpMatchDescReq_c,              /* pktFromMsgIndex  */ \
    mZtcMsgFromPktTwoArrayPtrs_c,       /* msgFromPktFunc   */ \
    mFmtZdpMatchDescReq_c)              /* msgFromPktIndex  */

/* Used for ZDP-Discovery_Store.Request (A2 16). */
Fmt(gZtcMsgFmtZdpDiscStoreReq_c,        /* name             */ \
    mZtcPktFromMsgHdrAndData_c,         /* pktFromMsgFunc   */ \
    mFmtZdpDiscStoreReq_c,              /* pktFromMsgIndex  */ \
    mZtcMsgFromPktHdrAndData_c,         /* msgFromPktFunc   */ \
    mFmtZdpDiscStoreReq_c)              /* msgFromPktIndex  */

/* Used for ZDP-Active_EP_Store.Request (A2 19). */
Fmt(gZtcMsgFmtActiveEPStoreReq_c,       /* name             */ \
    mZtcPktFromMsgHdrAndData_c,         /* pktFromMsgFunc   */ \
    mFmtZdpActiveEPStoreReq_c,          /* pktFromMsgIndex  */ \
    mZtcMsgFromPktHdrAndData_c,         /* msgFromPktFunc   */ \
    mFmtZdpActiveEPStoreReq_c)          /* msgFromPktIndex  */

/* Used for ZDP-Simple_Desc_Store.Request (A2 1A). */
Fmt(gZtcMsgFmtSimpleDescStoreReq_c,     /* name             */ \
    mZtcPktFromMsgTwoArrayPtrs_c,       /* pktFromMsgFunc   */ \
    mFmtZdpSimpleDescStoreReq_c,        /* pktFromMsgIndex  */ \
    mZtcMsgFromPktTwoArrayPtrs_c,       /* msgFromPktFunc   */ \
    mFmtZdpSimpleDescStoreReq_c)        /* msgFromPktIndex  */

/* Used for ZDP-End_Device_Bind.Request (A2 20). */
Fmt(gZtcMsgFmtZdpEndDeviceBindReq_c,    /* name             */ \
    mZtcPktFromMsgTwoArrayPtrs_c,       /* pktFromMsgFunc   */ \
    mFmtZdpEndDeviceBindReq_c,          /* pktFromMsgIndex  */ \
    mZtcMsgFromPktTwoArrayPtrs_c,       /* msgFromPktFunc   */ \
    mFmtZdpEndDeviceBindReq_c)          /* msgFromPktIndex  */

/* Used for ZDP-Backup_Bind_Table.Request (A2 27). */
Fmt(gZtcMsgFmtBkupBindTblReq_c,         /* name             */ \
    mZtcPktFromMsgHdrAndData_c,         /* pktFromMsgFunc   */ \
    mFmtZdpBkupBindTblReq_c,            /* pktFromMsgIndex  */ \
    mZtcMsgFromPktHdrAndData_c,         /* msgFromPktFunc   */ \
    mFmtZdpBkupBindTblReq_c)            /* msgFromPktIndex  */

/* Used for ZDP-Backup_Source_Bind.Request (A2 29). */
Fmt(gZtcMsgFmtBkupSrcBindReq_c,         /* name             */ \
    mZtcPktFromMsgHdrAndData_c,         /* pktFromMsgFunc   */ \
    mFmtZdpBkupSrcBindReq_c,            /* pktFromMsgIndex  */ \
    mZtcMsgFromPktHdrAndData_c,         /* msgFromPktFunc   */ \
    mFmtZdpBkupSrcBindReq_c)            /* msgFromPktIndex  */

Fmt(gZtcMsgFmtZdpMgmtNwkUpdateNotifyRequest_c,/* name             */ \
    mZtcPktFromMsgHdrAndData_c,               /* pktFromMsgFunc   */ \
    mFmtZdpMgmtNwkUpdateNotify_c,             /* pktFromMsgIndex  */ \
    mZtcMsgFromPktHdrAndData_c,               /* msgFromPktFunc   */ \
    mFmtZdpMgmtNwkUpdateNotify_c)             /* msgFromPktIndex  */


    
#if gSAPMessagesEnableHc_d

 
Fmt(gZtcMsgFmtHc_c,       /* name             */ \
    mZtcPktFromMsgHc_c,   /* pktFromMsgFunc   */ \
    gUnused_c,        /* pktFromMsgIndex  */ \
    mZtcMsgFromPktHc_c,   /* msgFromPktFunc   */ \
    gUnused_c)        /* msgFromPktIndex  */  
      


    

#endif


#endif /* gBeeStackIncluded_d */
#undef Fmt
#undef AppZdpSize
#undef AppZdpOfs
