/******************************************************************************
* ZtcMsgTypeInfoTbl.h
* Ztc is a replacement for the existing ZigBee Test Client (ZTC) module.
*
* Copyright (c) 2008, Freescale, Inc.  All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from  Freescale Semiconductor.
*
******************************************************************************/

/* Table of information about message IDs, by SAP Handler.
 * Note that the Test Tool uses the term "opcode" where the internal
 * BeeStack code mostly uses "message id."
 */

/* The purpose of these tables is to keep various declarations and defintions
 * in perfect sync, without requiring maintenence of different declarations.
 * All information needed by Ztc about the MsgTypes supported by each SAP
 * Handler is kept here.
 *
 * The code should never be aware of which message type it is working with.
 * There should never be logic of the form:
 *
 *      "if this is message type X,
 *      then do Y"
 *
 * All of the knobs that can be tweaked to describe message types should be
 * in this file. The code should instead contain logic of the form:
 *
 *      "if the message type info table entry for this message type says X,
 *      then do Y".
 *
 * Making a change to the way the code deals with different message types
 * should require only changes in this file, unless a completely new kind
 * of handling is needed. In that case, this file should be changed to provide
 * a means of triggering the new behavior of the code.
 *
 * There is one special case: Ztc configuration commands from the Test Tool
 * look just like other packets, with an opcode group and a message id. The
 * code intercepts them, and does not use a normal message type info table
 * for that opcode group.
 *  
 * Some of the symbols and values in this table must remain as they are,
 * for compabilility with external tools, with the rest of the Beestack
 * code, and with the (binary only) MAC layer library.
 */

/* IMPORTANT NOTE: The order of the entries in these tables is not significant.
 * Entries can be added, removed or moved freely. The code must work if the
 * order of the entries in the tables is changed.
 */

/****************************************************************************/

/* There is one set of *Tbl() macros for each SAP Handler.
 * The macro format is:
 *  Column 1    msgType: The integer value of the message type. Must be
 *                compatible with library code and external clients.
 *  Column 2    cnfType: For request message types, the integer value of the
 *                corresponding confirmation. For confirmations, ignored.
 *  Column 3    flags: Various flag bits.
 *  Column 4    len: The size of the struct used by the SAP Handler to access
 *                the message. Note that this does not include the msgType_t
 *                that preceeds the data.
 *  Column 5    format: Used to select a message format translator when copying
 *                the message data.
 */

/* The name in the comment at the end of each row (if any) is from the Test */
/* Tool xml file for the given opcode group and opcode. */

#ifndef NwkMcpsTbl
#define NwkMcpsTbl(msgType, cnfType, flags, len, format)
#endif

/* gNwkMCPS_SAPHandlerId_c = 0x87, NWK_MCPS_SapHandler(). gaZtcNwkMcpsMsgTypeTable[] */
NwkMcpsTbl(0x00, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                                      gZtcMsgFmtMacDataReq_c         ) /* MacData.Request  */
NwkMcpsTbl(0x01, gUnused_c,   gZtcMsgFlagAsync_c,   sizeof(mcpsPurgeReq_t),                         gZtcMsgFmtSimple_c             ) /* MacPurge.Request */
#undef  NwkMcpsTbl

#ifndef McpsNwkTbl
#define McpsNwkTbl(msgType, cnfType, flags, len, format)
#endif

/* gMCPSNwkSAPHandlerId_c = 0x86, MCPS_NWK_SapHandler(). gaZtcMcpsNwkMsgTypeTable[] */
McpsNwkTbl(0x00, gUnused_c,   gZtcMsgFlagAsync_c,   sizeof(mcpsDataCnf_t),                          gZtcMsgFmtSimple_c             ) /* MacData.Confirm      */
McpsNwkTbl(0x01, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                                      gZtcMsgFmtMacDataInd_c         ) /* MacData.Indication   */
McpsNwkTbl(0x02, gUnused_c,   gZtcMsgFlagAsync_c,   sizeof(mcpsPurgeCnf_t),                         gZtcMsgFmtSimple_c             ) /* MacPurge.Confirm     */
McpsNwkTbl(0x03, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                                      gZtcMsgFmtMcpsPromInd_c        ) /* Promiscuous.Ind      */
#undef McpsNwkTbl

#ifndef NwkMlmeTbl
#define NwkMlmeTbl(msgType, cnfType, flags, len, format)
#endif

/* gNwkMLME_SAPHandlerId_c = 0x85, NWK_MLME_SapHandler(). gaZtcNwkMlmeMsgTypeTable[] */
NwkMlmeTbl(0x00, 0x01,        gZtcMsgFlagAsync_c,   sizeof(mlmeAssociateReq_t),                     gZtcMsgFmtSimple_c             ) /* MacAssociate.Request          */
NwkMlmeTbl(0x01, gUnused_c,   gZtcMsgFlagAsync_c,   sizeof(mlmeAssociateRes_t),                     gZtcMsgFmtSimple_c             ) /* MacAssociate.Response         */
NwkMlmeTbl(0x02, 0x03,        gZtcMsgFlagAsync_c,   sizeof(mlmeDisassociateReq_t),                  gZtcMsgFmtSimple_c             ) /* MacDisassociate.Request       */
#if gZtcMacGetSetPIBCapability_d
NwkMlmeTbl(0x03, 0x05,        gZtcMsgFlagSync_c,    gUnused_c,                                      gZtcMsgFmtMacGetReq_c          ) /* MacGetPIBAttribute.Request    */
#endif
NwkMlmeTbl(0x04, 0x07,        gZtcMsgFlagAsync_c,   sizeof(mlmeGtsReq_t),                           gZtcMsgFmtSimple_c             ) /* MacGTS.Request                */
NwkMlmeTbl(0x05, gUnused_c,   gZtcMsgFlagAsync_c,   sizeof(mlmeOrphanRes_t),                        gZtcMsgFmtSimple_c             ) /* MacOrphan.Response            */
NwkMlmeTbl(0x06, 0x09,        gZtcMsgFlagSync_c,    sizeof(mlmeResetReq_t),                         gZtcMsgFmtSimple_c             ) /* MacReset.Request              */
NwkMlmeTbl(0x07, 0x0A,        gZtcMsgFlagAsync_c,   sizeof(mlmeRxEnableReq_t),                      gZtcMsgFmtSimple_c             ) /* MacRxEnable.Request           */
NwkMlmeTbl(0x08, 0x0B,        gZtcMsgFlagAsync_c,   sizeof(mlmeScanReq_t),                          gZtcMsgFmtMacScanReq_c         ) /* MacScan.Request               */
#if gZtcMacGetSetPIBCapability_d
NwkMlmeTbl(0x09, 0x0D,        gZtcMsgFlagSync_c,    gUnused_c,                                      gZtcMsgFmtMacSetReq_c          ) /* MacSetPIBAttribute.Request    */
#endif
NwkMlmeTbl(0x0A, 0x0E,        gZtcMsgFlagAsync_c,   sizeof(mlmeStartReq_t),                         gZtcMsgFmtSimple_c             ) /* MacStart.Request              */
NwkMlmeTbl(0x0B, gUnused_c,   gZtcMsgFlagAsync_c,   sizeof(mlmeSyncReq_t),                          gZtcMsgFmtSimple_c             ) /* MacSync.Request               */
NwkMlmeTbl(0x0C, 0x10,        gZtcMsgFlagAsync_c,   sizeof(mlmePollReq_t),                          gZtcMsgFmtSimple_c             ) /* MacPoll.Request               */
#undef NwkMlmeTbl

#ifndef MlmeNwkTbl
#define MlmeNwkTbl(msgType, cnfType, flags, len, format)
#endif

/* gMLMENwkSAPHandlerId_c = 0x84, MLME_NWK_SapHandler(). gaZtcMlmeNwkMsgTypeTable[] */
MlmeNwkTbl(0x00, gUnused_c,   gZtcMsgFlagAsync_c,   sizeof(nwkAssociateInd_t),                      gZtcMsgFmtSimple_c             ) /* MacAssociate.Indication       */
MlmeNwkTbl(0x01, gUnused_c,   gZtcMsgFlagAsync_c,   sizeof(nwkAssociateCnf_t),                      gZtcMsgFmtSimple_c             ) /* MacAssociate.Confirm          */
MlmeNwkTbl(0x02, gUnused_c,   gZtcMsgFlagAsync_c,   sizeof(nwkDisassociateInd_t),                   gZtcMsgFmtSimple_c             ) /* MacDisassociate.Indication    */
MlmeNwkTbl(0x03, gUnused_c,   gZtcMsgFlagAsync_c,   sizeof(nwkDisassociateCnf_t),                   gZtcMsgFmtSimple_c             ) /* MacDisassociate.Confirm       */
MlmeNwkTbl(0x04, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                                      gZtcMsgFmtMacBeaconNotifyInd_c ) /* MacBeaconNotify.Indication    */
MlmeNwkTbl(0x06, gUnused_c,   gZtcMsgFlagAsync_c,   sizeof(nwkGtsInd_t),                            gZtcMsgFmtSimple_c             ) /* MacGTS.Indication             */
MlmeNwkTbl(0x07, gUnused_c,   gZtcMsgFlagAsync_c,   sizeof(nwkGtsCnf_t),                            gZtcMsgFmtSimple_c             ) /* MacGTS.Confirm                */
MlmeNwkTbl(0x08, gUnused_c,   gZtcMsgFlagAsync_c,   sizeof(nwkOrphanInd_t),                         gZtcMsgFmtSimple_c             ) /* MacOrphan.Indication          */
MlmeNwkTbl(0x0A, gUnused_c,   gZtcMsgFlagAsync_c,   sizeof(nwkRxEnableCnf_t),                       gZtcMsgFmtSimple_c             ) /* MacRxEnable.Confirm           */
MlmeNwkTbl(0x0B, gUnused_c,   gZtcMsgFlagAsync_c,   sizeof(nwkScanCnf_t),                           gZtcMsgFmtMacScanCnf_c         ) /* MacScan.Confirm               */
MlmeNwkTbl(0x0C, gUnused_c,   gZtcMsgFlagAsync_c,   sizeof(nwkCommStatusInd_t),                     gZtcMsgFmtSimple_c             ) /* MacCommStatus.Indication      */
MlmeNwkTbl(0x0E, gUnused_c,   gZtcMsgFlagAsync_c,   sizeof(nwkStartCnf_t),                          gZtcMsgFmtSimple_c             ) /* MacStart.Confirm              */
MlmeNwkTbl(0x0F, gUnused_c,   gZtcMsgFlagAsync_c,   sizeof(nwkSyncLossInd_t),                       gZtcMsgFmtSimple_c             ) /* MacSyncLoss.Indication        */
MlmeNwkTbl(0x10, gUnused_c,   gZtcMsgFlagAsync_c,   sizeof(nwkPollCnf_t),                           gZtcMsgFmtSimple_c             ) /* MacPoll.Confirm               */
MlmeNwkTbl(0x11, gUnused_c,   gZtcMsgFlagAsync_c,   sizeof(nwkErrorCnf_t),                          gZtcMsgFmtSimple_c             ) /* MacInputError.Confirm         */
MlmeNwkTbl(0x12, gUnused_c,   gZtcMsgFlagAsync_c,   sizeof(nwkBeaconStartInd_t),                    gZtcMsgFmtSimple_c             ) /* MacBeaconStart.Indication     */
MlmeNwkTbl(0x13, gUnused_c,   gZtcMsgFlagAsync_c,   sizeof(nwkMaintenanceScanCnf_t),                gZtcMsgFmtSimple_c             ) /* MacMaintenanceScan.Confirm    */
MlmeNwkTbl(0x14, gUnused_c,   gZtcMsgFlagAsync_c,   sizeof(nwkPollNotifyInd_t),                     gZtcMsgFmtSimple_c             ) /* MacPollNotifyIndication.Indication */

#undef MlmeNwkTbl

#ifndef NwkAspTbl
#define NwkAspTbl(msgType, cnfType, flags, len, format)
#endif

/* gNwkASP_SAPHandlerId_c = 0x95, APP_ASP_SapHandler(). gaZtcNwkAspMsgTypeTable[] */

NwkAspTbl(aspMsgTypeGetTimeReq_c,         aspMsgTypeGetTimeReq_c,         gZtcMsgFlagSync_c,    sizeof(aspGetTimeReq_t),                         gZtcMsgFmtSimple_c         ) /* AspGetTime.Request           */
NwkAspTbl(aspMsgTypeWakeReq_c,            aspMsgTypeWakeReq_c,            gZtcMsgFlagSync_c,    sizeof(aspWakeReq_t),                            gZtcMsgFmtSimple_c         ) /* AspWake.Request              */
NwkAspTbl(aspMsgTypeGetMacStateReq_c,     aspMsgTypeGetMacStateCnf_c,     gZtcMsgFlagSync_c,    sizeof(aspGetMacStateReq_t),                     gZtcMsgFmtSimple_c         ) /* AspGetMacState.Request       */
NwkAspTbl(aspMsgTypeDozeReq_c,            aspMsgTypeDozeCnf_c,            gZtcMsgFlagSync_c,    sizeof(aspDozeReq_t),                            gZtcMsgFmtSimple_c         ) /* AspDoze.Request              */
NwkAspTbl(aspMsgTypeAutoDozeReq_c,        aspMsgTypeAutoDozeCnf_c,        gZtcMsgFlagSync_c,    sizeof(aspAutoDozeReq_t),                        gZtcMsgFmtSimple_c         ) /* AspAutoDoze.Request          */
NwkAspTbl(aspMsgTypeSetMinDozeTimeReq_c,  aspMsgTypeSetMinDozeTimeReq_c,  gZtcMsgFlagSync_c,    sizeof(aspSetMinDozeTimeReq_t),                  gZtcMsgFmtSimple_c         ) /* AspSetMinDozeTime.Request    */
NwkAspTbl(aspMsgTypeAcomaReq_c,           aspMsgTypeAcomaReq_c,           gZtcMsgFlagSync_c,    sizeof(aspAcomaReq_t),                           gZtcMsgFmtSimple_c         ) /* AspAcome.Request             */
NwkAspTbl(aspMsgTypeHibernateReq_c,       aspMsgTypeHibernateReq_c,       gZtcMsgFlagSync_c,    sizeof(aspHibernateReq_t),                       gZtcMsgFmtSimple_c         ) /* AspHibernate.Request         */
NwkAspTbl(aspMsgTypeClkoReq_c,            aspMsgTypeClkoReq_c,            gZtcMsgFlagSync_c,    sizeof(aspClkoReq_t),                            gZtcMsgFmtSimple_c         ) /* AspClko.Request              */
NwkAspTbl(aspMsgTypeTrimReq_c,            aspMsgTypeTrimReq_c,            gZtcMsgFlagSync_c,    sizeof(aspTrimReq_t),                            gZtcMsgFmtSimple_c         ) /* AspTrim.Request              */
NwkAspTbl(aspMsgTypeDdrReq_c,             aspMsgTypeDdrReq_c,             gZtcMsgFlagSync_c,    sizeof(aspDdrReq_t),                             gZtcMsgFmtSimple_c         ) /* AspDdr.Request               */
NwkAspTbl(aspMsgTypePortReq_c,            aspMsgTypePortReq_c,            gZtcMsgFlagSync_c,    sizeof(aspPortReq_t),                            gZtcMsgFmtSimple_c         ) /* AspPort.Request              */
NwkAspTbl(aspMsgTypeEventReq_c,           aspMsgTypeEventReq_c,           gZtcMsgFlagSync_c,    sizeof(aspEventReq_t),                           gZtcMsgFmtSimple_c         ) /* AspEvent.Request             */
NwkAspTbl(aspMsgTypeGetInactiveTimeReq_c, aspMsgTypeGetInactiveTimeCnf_c, gZtcMsgFlagSync_c,    sizeof(aspGetInactiveTimeReq_t),                 gZtcMsgFmtSimple_c         ) /* AspGetInactiveTime.Request   */
NwkAspTbl(aspMsgTypeSetNotifyReq_c,       aspMsgTypeSetNotifyReq_c,       gZtcMsgFlagSync_c,    sizeof(aspSetNotifyReq_t),                       gZtcMsgFmtSimple_c         ) /* AspSetNotify.Request         */
NwkAspTbl(aspMsgTypeSetPowerLevel_c,      aspMsgTypeSetPowerLevel_c,      gZtcMsgFlagSync_c,    sizeof(aspSetPowerLevelReq_t),                   gZtcMsgFmtSimple_c         ) /* AspSetPowerLevel.Request     */
NwkAspTbl(aspMsgTypeGetPowerLevel_c,      aspMsgTypeGetPowerLevel_c,      gZtcMsgFlagSync_c,    sizeof(aspGetPowerLevelReq_t),                   gZtcMsgFmtSimple_c         ) /* AspGetPowerLevel.Request     */
NwkAspTbl(aspMsgTypeTelecTest_c,          aspMsgTypeTelecTest_c,          gZtcMsgFlagSync_c,    sizeof(aspTelecTest_t),                          gZtcMsgFmtSimple_c         ) /* AspTelecTest.Request         */
NwkAspTbl(aspMsgTypeTelecSetFreq_c,       aspMsgTypeTelecSetFreq_c,       gZtcMsgFlagSync_c,    sizeof(aspTelecsetFreq_t),                       gZtcMsgFmtSimple_c         ) /* AspTelecSetFreq.Request      */
NwkAspTbl(aspMsgTypeTelecSendRawData_c,   aspMsgTypeTelecSendRawData_c,   gZtcMsgFlagSync_c,    sizeof(aspTelecSendRawData_t),                   gZtcMsgFmtSimple_c         ) /* AspTelecSendRawData.Request  */
#undef NwkAspTbl

#ifndef NwkLlcTbl
#define NwkLlcTbl(msgType, cnfType, flags, len, format)
#endif

/* gNwkLLC_SAPHandlerId_c = 0xB0, APP_LLC_SapHandler(). gaZtcNwkLlcMsgTypeTable[] */

NwkLlcTbl(gllcMsgTypeSetLedReq_c,             gllcMsgTypeSetLedReq_c,             gZtcMsgFlagSync_c,  sizeof(llcSetLedReq_t),             gZtcMsgFmtSimple_c) /* LlcSetLed.Request             */
NwkLlcTbl(gllcMsgTypeStartDACwaveReq_c,       gllcMsgTypeStartDACwaveReq_c,       gZtcMsgFlagSync_c,  sizeof(llcSetVolumeReq_t),          gZtcMsgFmtSimple_c) /* LlcsSartDACWave.Request       */
NwkLlcTbl(gllcMsgTypeStopDACwaveReq_c,        gllcMsgTypeStopDACwaveReq_c,        gZtcMsgFlagSync_c,  0,                                  gZtcMsgFmtSimple_c) /* LlcsStopDACWave.Request       */
NwkLlcTbl(gllcMsgTypeStartPWMwaveReq_c,       gllcMsgTypeStartPWMwaveReq_c,       gZtcMsgFlagSync_c,  sizeof(llcSetVolumeReq_t),          gZtcMsgFmtSimple_c) /* LlcsSartPWMWave.Request       */
NwkLlcTbl(gllcMsgTypeStopPWMwaveReq_c,        gllcMsgTypeStopPWMwaveReq_c,        gZtcMsgFlagSync_c,  0,                                  gZtcMsgFmtSimple_c) /* LlcsStopPWMWave.Request       */
NwkLlcTbl(gllcMsgTypeStartPWMReq_c,           gllcMsgTypeStartPWMReq_c,           gZtcMsgFlagSync_c,  sizeof(llcSetPwmDutyReq_t),         gZtcMsgFmtSimple_c) /* LlcsSartPWM.Request           */
NwkLlcTbl(gllcMsgTypeStopPWMReq_c,            gllcMsgTypeStopPWMReq_c,            gZtcMsgFlagSync_c,  0,                                  gZtcMsgFmtSimple_c) /* LlcsStopPWM.Request           */
NwkLlcTbl(gllcMsgTypeReadSensorReq_c,         gllcMsgTypeReadSensorReq_c,         gZtcMsgFlagSync_c,  sizeof(llcReadSensorReq_t),         gZtcMsgFmtSimple_c) /* LlcReadSensor.Request         */
NwkLlcTbl(gllcMsgTypeSetASStateReq_c,         gllcMsgTypeSetASStateReq_c,         gZtcMsgFlagSync_c,  sizeof(llcSetASStateReq_t),         gZtcMsgFmtSimple_c) /* LlcSetPSState.Request         */
NwkLlcTbl(gllcMsgTypeReadSwReq_c,             gllcMsgTypeReadSwReq_c,             gZtcMsgFlagSync_c,  sizeof(llcReadSwReq_t),             gZtcMsgFmtSimple_c) /* LlcReadSw.Request             */
NwkLlcTbl(gllcMsgTypeCrystalTrimmReq_c,       gllcMsgTypeCrystalTrimmReq_c,       gZtcMsgFlagSync_c,  sizeof(llcCrystalTrimmReq_t),       gZtcMsgFmtSimple_c) /* LlcCrystalTrimm.Request       */
NwkLlcTbl(gllcMsgTypeDisplayChessBoardReq_c,  gllcMsgTypeDisplayChessBoardReq_c,  gZtcMsgFlagSync_c,  sizeof(llcDisplayChessBoardReq_t),  gZtcMsgFmtSimple_c) /* LlcDisplayChessBoard.Request  */
NwkLlcTbl(gllcMsgTypeStartReceiverReq_c,      gllcMsgTypeStartReceiverReq_c,      gZtcMsgFlagSync_c,  sizeof(llcStartReceiverReq_t),      gZtcMsgFmtSimple_c) /* LlcStartReceive.Request       */
NwkLlcTbl(gllcMsgTypeGetReceivedPacketsReq_c, gllcMsgTypeGetReceivedPacketsReq_c, gZtcMsgFlagSync_c,  sizeof(llcGetReceivedPacketsReq_t), gZtcMsgFmtSimple_c) /* LlcGetReceivedPackets.Request */
NwkLlcTbl(gllcMsgTypeStartTransmitterReq_c,   gllcMsgTypeStartTransmitterReq_c,   gZtcMsgFlagSync_c,  sizeof(llcStartTransmitterReq_t),   gZtcMsgFmtSimple_c) /* LlcStartTransmitter.Request   */
NwkLlcTbl(gllcMsgTypeTransmitReq_c,           gllcMsgTypeTransmitReq_c,           gZtcMsgFlagSync_c,  sizeof(llcTransmitReq_t),           gZtcMsgFmtSimple_c) /* LlcTransmit.Request           */
NwkLlcTbl(gllcMsgTypeTransmitConfirm_c,       gllcMsgTypeTransmitConfirm_c,       gZtcMsgFlagAsync_c, 1,                                  gZtcMsgFmtSimple_c) /* LlcTransmit.Confirm           */
NwkLlcTbl(gllcMsgTypeSetupJTAGNEXUSPinsReq_c, gllcMsgTypeSetupJTAGNEXUSPinsReq_c, gZtcMsgFlagSync_c,  sizeof(llcSetupJTAGNEXUSPinsReq_t), gZtcMsgFmtSimple_c) /* LlcSetupJtagNexusPins.Request */
NwkLlcTbl(gllcMsgTypeReadJTAGNEXUSPinsReq_c,  gllcMsgTypeReadJTAGNEXUSPinsReq_c,  gZtcMsgFlagSync_c,  sizeof(llcReadJTAGNEXUSPinsReq_t),  gZtcMsgFmtSimple_c) /* LlcReadJtagNexusPins.Request  */
NwkLlcTbl(gllcMsgTypeStart12MhzPwmReq_c,      gllcMsgTypeStart12MhzPwmReq_c,      gZtcMsgFlagSync_c,  0,                                  gZtcMsgFmtSimple_c) /* LlcStart12MHZ.Request         */
NwkLlcTbl(gllcMsgTypeStop12MhzPwmReq_c,       gllcMsgTypeStop12MhzPwmReq_c,       gZtcMsgFlagSync_c,  0,                                  gZtcMsgFmtSimple_c) /* LlcStop12MHZ.Request          */
NwkLlcTbl(gllcMsgSetupGpioPinsReq_c,          gllcMsgSetupGpioPinsReq_c,          gZtcMsgFlagSync_c,  sizeof(llcSetupGpioPinsReq_t),      gZtcMsgFmtSimple_c) /* LlcSetupGPIOPins.Request      */
#undef NwkLlcTbl

#ifndef AspNwkTbl
#define AspNwkTbl(msgType, cnfType, flags, len, format)
#endif

/* gASPNwkSAPHandlerId_c = 0x94, ASP_APP_SapHandler(). gaZtcAspNwkMsgTypeTable[] */
/* For sync confirms, the request entry contains all needed info. */
/* These are only here as documentation. */
AspNwkTbl(0x00, 0x00,        gZtcMsgFlagSync_c,    0,                       gZtcMsgFmtSimple_c             ) /* AspGetTime.Confirm            */
AspNwkTbl(0x01, gUnused_c,   gZtcMsgFlagAsync_c,   sizeof(appWakeInd_t),    gZtcMsgFmtSimple_c             ) /* AspWake.Indication            */ 
AspNwkTbl(0x02, gUnused_c,   gZtcMsgFlagAsync_c,   sizeof(appIdleInd_t),    gZtcMsgFmtSimple_c             ) /* AspIdle.Indication            */ 
AspNwkTbl(0x03, gUnused_c,   gZtcMsgFlagAsync_c,   sizeof(appInactiveInd_t),gZtcMsgFmtSimple_c             ) /* AspInactive.Indication        */ 
AspNwkTbl(0x04, gUnused_c,   gZtcMsgFlagAsync_c,   sizeof(appEventInd_t),   gZtcMsgFmtSimple_c             ) /* AspEvent.Indication           */ 
AspNwkTbl(0x05, 0x05,        gZtcMsgFlagSync_c,    0,                       gZtcMsgFmtSimple_c             ) /* AspAcome.Confirm              */
AspNwkTbl(0x06, 0x06,        gZtcMsgFlagSync_c,    0,                       gZtcMsgFmtSimple_c             ) /* AspHibernate.Confirm          */
AspNwkTbl(0x07, 0x07,        gZtcMsgFlagSync_c,    0,                       gZtcMsgFmtSimple_c             ) /* AspWake.Confirm               */
AspNwkTbl(0x08, 0x08,        gZtcMsgFlagSync_c,    0,                       gZtcMsgFmtSimple_c             ) /* AspEvent.Confirm              */ 
AspNwkTbl(0x09, 0x09,        gZtcMsgFlagSync_c,    0,                       gZtcMsgFmtSimple_c             ) /* AspClko.Confirm               */
AspNwkTbl(0x0A, 0x0A,        gZtcMsgFlagSync_c,    0,                       gZtcMsgFmtSimple_c             ) /* AspTrim.Confirm               */
AspNwkTbl(0x0B, 0x0B,        gZtcMsgFlagSync_c,    0,                       gZtcMsgFmtSimple_c             ) /* AspDdr.Confirm                */
AspNwkTbl(0x0C, 0x0C,        gZtcMsgFlagSync_c,    0,                       gZtcMsgFmtSimple_c             ) /* AspPort.Confirm               */
AspNwkTbl(0x0D, 0x0D,        gZtcMsgFlagSync_c,    0,                       gZtcMsgFmtSimple_c             ) /* AspSetMinDozeTime.Confirm     */
AspNwkTbl(0x0E, 0x0E,        gZtcMsgFlagSync_c,    0,                       gZtcMsgFmtSimple_c             ) /* AspSetNotify.Confirm          */ 
AspNwkTbl(0x0F, 0x0F,        gZtcMsgFlagSync_c,    0,                       gZtcMsgFmtSimple_c             ) /* AspSetPowerLevel.Confirm      */ 
AspNwkTbl(0x10, 0x10,        gZtcMsgFlagSync_c,    0,                       gZtcMsgFmtSimple_c             ) /* AspTelecTest.Confirm          */ 
AspNwkTbl(0x11, 0x11,        gZtcMsgFlagSync_c,    0,                       gZtcMsgFmtSimple_c             ) /* AspTelecSetFreq.Confirm       */ 
AspNwkTbl(0x12, 0x01,        gZtcMsgFlagSync_c,    0,                       gZtcMsgFmtSimple_c             ) /* AspGetInactiveTime.Confirm    */
AspNwkTbl(0x13, 0x02,        gZtcMsgFlagSync_c,    0,                       gZtcMsgFmtSimple_c             ) /* AspGetMacState.Confirm        */
AspNwkTbl(0x14, 0x03,        gZtcMsgFlagSync_c,    0,                       gZtcMsgFmtSimple_c             ) /* AspDoze.Confirm               */
AspNwkTbl(0x15, 0x04,        gZtcMsgFlagSync_c,    0,                       gZtcMsgFmtSimple_c             ) /* AspAutoDoze.Confirm           */
AspNwkTbl(0x16, 0x16,        gZtcMsgFlagSync_c,    0,                       gZtcMsgFmtSimple_c             ) /* AspTelecSendRawData.Confirm   */
AspNwkTbl(0x1F, 0x1F,        gZtcMsgFlagSync_c,    0,                       gZtcMsgFmtSimple_c             ) /* AspGetPowerLevel.Confirm      */
 
#undef AspNwkTbl


#ifndef ApsNldeTbl
#define ApsNldeTbl(msgType, cnfType, flags, len, format)
#endif

/* gAPS_NLDE_SAPHandlerId_c = 0x9A, APS_NLDE_SapHandler(). gaZtcApsNldeMsgTypeTable[] */
ApsNldeTbl(0x33, 0x3F,        gZtcMsgFlagAsync_c,   gUnused_c,                                      gZtcMsgFmtNldeDataReq_c        ) /* NLDE-DATA.Request             */
#undef ApsNldeTbl

#ifndef NldeApsTbl
#define NldeApsTbl(msgType, cnfType, flags, len, format)
#endif

/* gNLDE_APS_SAPHandlerId_c = 0x9B, NLDE_APS_SapHandler(). gaZtcNldeApsMsgTypeTable[] */
NldeApsTbl(0x3F, 0x33,        gZtcMsgFlagAsync_c,   sizeof(nldeDataCnf_t),                          gZtcMsgFmtSimple_c             ) /* NLDE-DATA.Confirm             */
NldeApsTbl(0x40, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                                      gZtcMsgFmtNldeDataInd_c        ) /* NLDE-DATA.Indication          */
#undef NldeApsTbl

#ifndef ZdoNlmeTbl
#define ZdoNlmeTbl(msgType, cnfType, flags, len, format)
#endif

/* gZDO_NLME_SAPHandlerId_c = 0x96, ZDO_NLME_SapHandler(). gaZtcZdoNlmeMsgTypeTable[] */
ZdoNlmeTbl(0x34, 0x41,        gZtcMsgFlagAsync_c,   sizeof(nlmeNetworkDiscoveryReq_t),              gZtcMsgFmtSimple_c             ) /* NLME-NETWORK-DISCOVERY.Request  */
ZdoNlmeTbl(0x35, 0x42,        gZtcMsgFlagAsync_c,   sizeof(nlmeNetworkFormationReq_t),              gZtcMsgFmtSimple_c             ) /* NLME-NETWORK-FORMATION.Request  */
ZdoNlmeTbl(0x36, 0x43,        gZtcMsgFlagAsync_c,   sizeof(nlmePermitJoiningReq_t),                 gZtcMsgFmtSimple_c             ) /* NLME-PERMIT-JOINING.Request     */
ZdoNlmeTbl(0x37, 0x44,        gZtcMsgFlagAsync_c,   sizeof(nlmeStartRouterReq_t),                   gZtcMsgFmtSimple_c             ) /* NLME-START-ROUTER.Request       */
ZdoNlmeTbl(0x38, 0x46,        gZtcMsgFlagAsync_c,   sizeof(nlmeJoinReq_t),                          gZtcMsgFmtSimple_c             ) /* NLME-JOIN.Request               */
ZdoNlmeTbl(0x39, 0x47,        gZtcMsgFlagAsync_c,   sizeof(nlmeDirectJoinReq_t),                    gZtcMsgFmtSimple_c             ) /* NLME-DIRECT-JOIN.Request        */
ZdoNlmeTbl(0x3A, 0x48,        gZtcMsgFlagAsync_c,   sizeof(nlmeLeaveReq_t),                         gZtcMsgFmtSimple_c             ) /* NLME-LEAVE.Request              */
ZdoNlmeTbl(0x3B, 0x4A,        gZtcMsgFlagSync_c,    gZero_c,                                        gZtcMsgFmtSimple_c             ) /* NLME-RESET.Request              */
ZdoNlmeTbl(0x3C, 0x4C,        gZtcMsgFlagAsync_c,   sizeof(nlmeSyncReq_t),                          gZtcMsgFmtSimple_c             ) /* NLME-SYNC.Request               */
ZdoNlmeTbl(0x4F, 0x50,        gZtcMsgFlagAsync_c,   sizeof(nlmeRouteDiscoveryReq_t),                gZtcMsgFmtSimple_c             ) /* NLME-ROUTE-DISCOVERY.Req        */
ZdoNlmeTbl(0x54, 0x55,        gZtcMsgFlagAsync_c,   sizeof(nlmeEnergyScanReq_t),                    gZtcMsgFmtSimple_c             ) /* NLME-Energy-Scan.Req            */
ZdoNlmeTbl(0x57, 0x57,        gZtcMsgFlagSync_c,    sizeof(nlmeSetChannelReq_t),                    gZtcMsgFmtSimple_c             ) /* NLME-RESET.Request              */
ZdoNlmeTbl(0x67, 0x68,        gZtcMsgFlagAsync_c,   sizeof(networkStatusCommandReq_t),              gZtcMsgFmtSimple_c             ) /* NLME-ROUTE-DISCOVERY.Req        */
ZdoNlmeTbl(0x71, 0x72,        gZtcMsgFlagAsync_c,   sizeof(nwkConcDiscTimeReq_t),                   gZtcMsgFmtSimple_c             ) /* Custom to stop or start the nwkConcDiscTime        */
#undef ZdoNlmeTbl

#ifndef NlmeZdoTbl
#define NlmeZdoTbl(msgType, cnfType, flags, len, format)
#endif

/* gNLME_ZDO_SAPHandlerId_c = 0x97, NLME_ZDO_SapHandler(). gaZtcNlmeZdoMsgTypeTable[] */
NlmeZdoTbl(0x41, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                                      gZtcMsgFmtNlmeNwkDiscCnf_c     ) /* NLME-NETWORK-DISCOVERY.Confirm          */
NlmeZdoTbl(0x42, gUnused_c,   gZtcMsgFlagAsync_c,   sizeof(nlmeNetworkFormationConf_t),             gZtcMsgFmtSimple_c             ) /* NLME-NETWORK-FORMATION.Confirm          */
NlmeZdoTbl(0x43, gUnused_c,   gZtcMsgFlagAsync_c,   sizeof(nlmePermitJoiningConf_t),                gZtcMsgFmtSimple_c             ) /* NLME-PERMIT-JOINING.Confirm             */
NlmeZdoTbl(0x44, gUnused_c,   gZtcMsgFlagAsync_c,   sizeof(nlmeStartRouterConf_t),                  gZtcMsgFmtSimple_c             ) /* NLME-START-ROUTER.Confirm               */
NlmeZdoTbl(0x45, gUnused_c,   gZtcMsgFlagAsync_c,   sizeof(nlmeJoinIndication_t),                   gZtcMsgFmtSimple_c             ) /* NLME-JOIN.Indication                    */
NlmeZdoTbl(0x46, gUnused_c,   gZtcMsgFlagAsync_c,   sizeof(nlmeJoinConf_t),                         gZtcMsgFmtSimple_c             ) /* NLME-JOIN.Confirm                       */
NlmeZdoTbl(0x47, gUnused_c,   gZtcMsgFlagAsync_c,   sizeof(nlmeDirectJoinConf_t),                   gZtcMsgFmtSimple_c             ) /* NLME-DIRECT-JOIN.Confirm                */
NlmeZdoTbl(0x48, gUnused_c,   gZtcMsgFlagAsync_c,   sizeof(nlmeLeaveConf_t),                        gZtcMsgFmtSimple_c             ) /* NLME-LEAVE.Confirm                      */
NlmeZdoTbl(0x49, gUnused_c,   gZtcMsgFlagAsync_c,   sizeof(nlmeLeaveIndication_t),                  gZtcMsgFmtSimple_c             ) /* NLME-LEAVE.Indication                   */
NlmeZdoTbl(0x4A, gUnused_c,   gZtcMsgFlagAsync_c,   sizeof(nlmeResetConf_t),                        gZtcMsgFmtSimple_c             ) /* NLME-RESET.Confirm                      */
NlmeZdoTbl(0x4B, gUnused_c,   gZtcMsgFlagAsync_c,   gZero_c,                                        gZtcMsgFmtSimple_c             ) /* NLME-SYNC.Indication                    */
NlmeZdoTbl(0x4C, gUnused_c,   gZtcMsgFlagAsync_c,   sizeof(nlmeSyncConf_t),                         gZtcMsgFmtSimple_c             ) /* NLME-SYNC.Confirm                       */
NlmeZdoTbl(0x50, gUnused_c,   gZtcMsgFlagAsync_c,   sizeof(nlmeRouteDiscoveryConf_t),               gZtcMsgFmtSimple_c             ) /* NLME-ROUTE-DISCOVERY.Confirm            */
NlmeZdoTbl(0x51, gUnused_c,   gZtcMsgFlagAsync_c,   sizeof(nlmeNwkStatusIndication_t),              gZtcMsgFmtSimple_c             ) /* NLME-NWK-STATUS.Indication              */
NlmeZdoTbl(0x55, gUnused_c,   gZtcMsgFlagAsync_c,   sizeof(nlmeEnergyScanCnf_t),                    gZtcMsgFmtMacScanCnf_c         ) /* NLME-Energy-Scan Confirm                */
NlmeZdoTbl(0x68, gUnused_c,   gZtcMsgFlagAsync_c,   sizeof(networkStatusCommandCnf_t),              gZtcMsgFmtSimple_c             ) /* Network Status Confirm                */
NlmeZdoTbl(0x70, gUnused_c,   gZtcMsgFlagAsync_c,   sizeof(zbStatus_t),                             gZtcMsgFmtSimple_c             ) /*  */
NlmeZdoTbl(0x58, gUnused_c,   gZtcMsgFlagAsync_c,   sizeof(nlmeNwkTxReport_t),                      gZtcMsgFmtSimple_c             ) /* NLME-NWK-Tx_report */
NlmeZdoTbl(0x72, gUnused_c,   gZtcMsgFlagAsync_c,   sizeof(nwkConcDiscTimeCnf_t),                   gZtcMsgFmtSimple_c             ) /*  */
#undef NlmeZdoTbl

#ifndef AfApsdeTbl
#define AfApsdeTbl(msgType, cnfType, flags, len, format)
#endif

/* gAF_APSDE_SAPHandlerId_c = 0x9C, AF_APSDE_SapHandler(). gaZtcAfApsdeMsgTypeTable[] */
AfApsdeTbl(0x00, 0x00,        gZtcMsgFlagAsync_c,   gUnused_c,                                      gZtcMsgFmtApsdeDataReq_c       ) /* APSDE-DATA.Request            */
#undef AfApsdeTbl

#ifndef ApsdeAfTbl
#define ApsdeAfTbl(msgType, cnfType, flags, len, format)
#endif

/* gAPSDE_AF_SAPHandlerId_c = 0x9D, APSDE_AF_SapHandler(). gaZtcApsdeAfMsgTypeTable[] */
ApsdeAfTbl(0x00, 0x00,        gZtcMsgFlagAsync_c,   sizeof(zbApsdeDataConfirm_t),                   gZtcMsgFmtSimple_c             ) /* APSDE-DATA.Confirm            */
ApsdeAfTbl(0x01, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                                      gZtcMsgFmtApsdeDataInd_c       ) /* APSDE-DATA.Indication         */
ApsdeAfTbl(0x03, gUnused_c,   gZtcMsgFlagAsync_c,   sizeof(zbApsConfirmId_t),                       gZtcMsgFmtSimple_c             ) /* Confirm ID. */
#undef ApsdeAfTbl

#ifndef AppAfdeTbl
#define AppAfdeTbl(msgType, cnfType, flags, len, format)
#endif

/* gAppAFDE_SAPHandlerId_c = 0x9F, AF_AppToAfdeDataReq(). gaZtcAppAfdeMsgTypeTable[] */
AppAfdeTbl(0x00, gUnused_c,   gZtcMsgFlagAsync_c,   gTODO_c,                                        gZtcMsgFmtTODO_c               ) /* gZTCRegEp_c                ZTC MsgID 117 */
AppAfdeTbl(0x01, gUnused_c,   gZtcMsgFlagAsync_c,   gTODO_c,                                        gZtcMsgFmtTODO_c               ) /* gZTCDeRegEp_c              ZTC MsgID 118 */
AppAfdeTbl(0x02, gUnused_c,   gZtcMsgFlagAsync_c,   gTODO_c,                                        gZtcMsgFmtTODO_c               ) /* gZTC_KVPDataReq_c          ZTC MsgID 119 */
AppAfdeTbl(0x03, gUnused_c,   gZtcMsgFlagAsync_c,   gTODO_c,                                        gZtcMsgFmtTODO_c               ) /* gZTCMsgDataReq_c           ZTC MsgID 120 */
AppAfdeTbl(0x04, gUnused_c,   gZtcMsgFlagAsync_c,   gTODO_c,                                        gZtcMsgFmtTODO_c               ) /* gZTC_AFResetReq_c          ZTC MsgID 121 */
AppAfdeTbl(0x05, gUnused_c,   gZtcMsgFlagAsync_c,   gTODO_c,                                        gZtcMsgFmtTODO_c               ) /* gZTC_AF_KVPAggregation_c   ZTC MsgID 122 */
AppAfdeTbl(0x06, gUnused_c,   gZtcMsgFlagAsync_c,   gTODO_c,                                        gZtcMsgFmtTODO_c               ) /* gZTC_AFMsgAggregation_c    ZTC MsgID 123 */
#undef AppAfdeTbl

#ifndef AfdeAppTbl
#define AfdeAppTbl(msgType, cnfType, flags, len, format)
#endif

/* gAFDEAppSAPHandlerId_c = 0x9E, AF_AppToAfdeDataReq(). gaZtcAppAfdeMsgTypeTable[] */
AfdeAppTbl(0x00, gUnused_c,   gZtcMsgFlagAsync_c,   sizeof(zbApsdeDataConfirm_t),                   gZtcMsgFmtSimple_c             ) /* gZTCRegEp_c                ZTC MsgID 117 */
AfdeAppTbl(0x01, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                                      gZtcMsgFmtApsdeDataInd_c       ) /* gZTCDeRegEp_c              ZTC MsgID 118 */
#undef AfdeAppTbl

#if gBeeStackIncluded_d == 1
#ifndef ZdoApsmeTbl
#define ZdoApsmeTbl(msgType, cnfType, flags, len, format)
#endif
/* gZDO_APSME_SAPHandlerId_c = 0x99, ZDO_APSME_SapHandler(). gaZtcZdoApsmeMsgTypeTable[] */
ZdoApsmeTbl(0x00, 0x07,        gZtcMsgFlagSync_c,    sizeof(zbApsmeBindReq_t),                      gZtcMsgFmtSimple_c             ) /* APSME-BIND.Request              */
ZdoApsmeTbl(0x09, 0x08,        gZtcMsgFlagSync_c,    sizeof(zbApsmeBindReq_t),                      gZtcMsgFmtSimple_c             ) /* APSME-UNBIND.Request            */
ZdoApsmeTbl(0x0B, 0x0B,        gZtcMsgFlagSync_c,    gZero_c,                                       gZtcMsgFmtSimple_c             ) /* APSME-RESET.Request             */
ZdoApsmeTbl(0xCF, 0xD5,        gZtcMsgFlagAsync_c,   sizeof(zbApsmeEstablishKeyReq_t),              gZtcMsgFmtSimple_c             ) /* APSME-ESTABLISH-KEY.Request     */
ZdoApsmeTbl(0xD0, gUnused_c,   gZtcMsgFlagAsync_c,   sizeof(zbApsmeEstablisKeyRsp_t),               gZtcMsgFmtSimple_c             ) /* APSME-ESTABLISH-KEY.response    */
ZdoApsmeTbl(0xD1, 0xC0,        gZtcMsgFlagAsync_c,   sizeof(zbApsmeTransportKeyReq_t),              gZtcMsgFmtSimple_c             ) /* APSME-TRANSPORT-KEY.Request     */
ZdoApsmeTbl(0xD3, gUnused_c,   gZtcMsgFlagAsync_c,   sizeof(zbApsmeRequestKeyReq_t),                gZtcMsgFmtSimple_c             ) /* APSME-REQUEST-KEY.Request       */
ZdoApsmeTbl(0xD4, gUnused_c,   gZtcMsgFlagAsync_c,   sizeof(zbApsmeSwitchKeyReq_t),                 gZtcMsgFmtSimple_c             ) /* APSME-SWITCH-KEY.Request        */
ZdoApsmeTbl(0xE8, 0xE8,        gZtcMsgFlagSync_c,    sizeof(zbApsmeAddGroupReq_t),                  gZtcMsgFmtSimple_c             ) /* APSME-ADD-GROUP.Request         */
ZdoApsmeTbl(0xE9, 0xE9,        gZtcMsgFlagSync_c,    sizeof(zbApsmeRemoveGroupReq_t),               gZtcMsgFmtSimple_c             ) /* APSME-REMOVE-GROUP.Request      */
ZdoApsmeTbl(0xEA, 0xEA,        gZtcMsgFlagSync_c,    sizeof(zbApsmeRemoveAllGroupsReq_t),           gZtcMsgFmtSimple_c             ) /* APSME-REMOVE-ALL-GROUPS.Request */
ZdoApsmeTbl(0xF3, gUnused_c,   gZtcMsgFlagAsync_c,   sizeof(zbApsmeUpdateDeviceReq_t),              gZtcMsgFmtSimple_c             ) /* APSME-UPDATE-DEVICE.Request     */
ZdoApsmeTbl(0xF4, gUnused_c,   gZtcMsgFlagAsync_c,   sizeof(zbApsmeRemoveDeviceReq_t),              gZtcMsgFmtSimple_c             ) /* APSME-REMOVE-DEVICE.Request     */
ZdoApsmeTbl(0xF5, 0xF7,        gZtcMsgFlagAsync_c,   sizeof(zbApsmeAuthenticateReq_t),              gZtcMsgFmtSimple_c             ) /* APSME-AUTHENTICATE.Request      */
#undef ZdoApsmeTbl
#endif

/* gAPSME_ZDO_SAPHandlerId_c = 0x98, APSME_ZDO_SapHandler(). gaZtcApsmeZdoMsgTypeTable[] */
#ifndef ApsmeZdoTbl
#define ApsmeZdoTbl(msgType, cnfType, flags, len, format)
#endif
ApsmeZdoTbl(0xD6, gUnused_c,   gZtcMsgFlagAsync_c,   sizeof(zbApsmeEstablishKeyInd_t),                gZtcMsgFmtSimple_c             ) /* APSME-ESTABLISH-KEY.Indication   */
ApsmeZdoTbl(0xD8, gUnused_c,   gZtcMsgFlagAsync_c,   sizeof(zbApsmeRemoveDeviceInd_t),                gZtcMsgFmtSimple_c             ) /* APSME-REMOVE-DEVICE.Indication   */
ApsmeZdoTbl(0xD9, gUnused_c,   gZtcMsgFlagAsync_c,   sizeof(zbApsmeRequestKeyInd_t),                  gZtcMsgFmtSimple_c             ) /* APSME-REQUEST-KEY.Indication     */
ApsmeZdoTbl(0xF1, gUnused_c,   gZtcMsgFlagAsync_c,   sizeof(zbApsmeSwitchKeyInd_t),                   gZtcMsgFmtSimple_c             ) /* APSME-SWITCH-KEY.Indication      */
ApsmeZdoTbl(0xF2, gUnused_c,   gZtcMsgFlagAsync_c,   sizeof(zbApsmeTransportKeyInd_t),                gZtcMsgFmtSimple_c             ) /* APSME-TRANSPORT-KEY.Indication   */
ApsmeZdoTbl(0xD7, gUnused_c,   gZtcMsgFlagAsync_c,   sizeof(zbApsmeUpdateDeviceInd_t),                gZtcMsgFmtSimple_c             ) /* APSME-UPDATE-DEVICE.Indication   */
ApsmeZdoTbl(0x0B, 0x0B,        gZtcMsgFlagAsync_c,   gZero_c,                                         gZtcMsgFmtSimple_c             ) /* APSME-RESET.Confirm              */
ApsmeZdoTbl(0xD5, gUnused_c,   gZtcMsgFlagAsync_c,   sizeof(zbApsmeCnf_t),                            gZtcMsgFmtSimple_c             ) /* APSME-ESTABLISH-KEY.Confirm      */
ApsmeZdoTbl(0xF6, gUnused_c,   gZtcMsgFlagAsync_c,   sizeof(zbApsmeAuthenticateInd_t),                gZtcMsgFmtSimple_c             ) /* APSME-AUTHENTICADTE.Indication   */
ApsmeZdoTbl(0xF7, gUnused_c,   gZtcMsgFlagAsync_c,   sizeof(zbApsmeCnf_t),                            gZtcMsgFmtSimple_c             ) /* APSME-AUTHENTICADTE.confirm      */
ApsmeZdoTbl(0xC0, gUnused_c,   gZtcMsgFlagAsync_c,   sizeof(zbApsmeCnf_t),                            gZtcMsgFmtSimple_c             ) /* APSME-TRANSPORT-KEY.Confirm      */
ApsmeZdoTbl(0xCC, gUnused_c,   gZtcMsgFlagAsync_c,   sizeof(zbStatus_t),                              gZtcMsgFmtSimple_c             )
#undef ApsmeZdoTbl

#ifndef AppZdpTbl
#define AppZdpTbl(msgType, cnfType, flags, len, format)
#endif

#define AppZdpSize(type)  (sizeof(type) + MbrOfs(appToZdpMessage_t, msgData) - MbrSizeof(appToZdpMessage_t, msgType))

/* gAppZDP_SAPHandlerId_c = 0xA2, APP_ZDP_SapHandler(). gaZtcAppZdpMsgTypeTable[] */
AppZdpTbl(0x00, gUnused_c,   gZtcMsgFlagAsync_c,   AppZdpSize(zbNwkAddrRequest_t),                  gZtcMsgFmtSimple_c             ) /* ZDP-NWK_addr_req.Request             */
AppZdpTbl(0x01, gUnused_c,   gZtcMsgFlagAsync_c,   AppZdpSize(zbIeeeAddrRequest_t),                 gZtcMsgFmtSimple_c             ) /* ZDP-IEEE_addr_req.Request            */
AppZdpTbl(0x02, gUnused_c,   gZtcMsgFlagAsync_c,   AppZdpSize(zbNodeDescriptorRequest_t),           gZtcMsgFmtSimple_c             ) /* ZDP-NodeDescriptor.Request           */
AppZdpTbl(0x03, gUnused_c,   gZtcMsgFlagAsync_c,   AppZdpSize(zbPowerDescriptorRequest_t),          gZtcMsgFmtSimple_c             ) /* ZDP-PowerDescriptor.Request          */
AppZdpTbl(0x04, gUnused_c,   gZtcMsgFlagAsync_c,   AppZdpSize(zbSimpleDescriptorRequest_t),         gZtcMsgFmtSimple_c             ) /* ZDP-SimpleDescriptor.Request         */
AppZdpTbl(0x05, gUnused_c,   gZtcMsgFlagAsync_c,   AppZdpSize(zbActiveEpRequest_t),                 gZtcMsgFmtSimple_c             ) /* ZDP-Active_EP_req.Request            */
AppZdpTbl(0x06, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                                       gZtcMsgFmtZdpMatchDescReq_c    ) /* ZDP-Match_Desc_req.Request           */
AppZdpTbl(0x10, gUnused_c,   gZtcMsgFlagAsync_c,   AppZdpSize(zbComplexDescriptorRequest_t),        gZtcMsgFmtSimple_c             ) /* ZDP-Complex_Desc_req                 */
AppZdpTbl(0x11, gUnused_c,   gZtcMsgFlagAsync_c,   AppZdpSize(zbUserDescriptorRequest_t),           gZtcMsgFmtSimple_c             ) /* ZDP-User_Desc_req.Request            */
AppZdpTbl(0x12, gUnused_c,   gZtcMsgFlagAsync_c,   AppZdpSize(zbDiscoveryCacheRequest_t),           gZtcMsgFmtSimple_c             ) /* ZDP-Discovery_Cache.request          */
AppZdpTbl(0x13, gUnused_c,   gZtcMsgFlagAsync_c,   AppZdpSize(zbEndDeviceAnnounce_t),               gZtcMsgFmtSimple_c             ) /* ZDP-EndDeviceAnnounce.Request        */
AppZdpTbl(0x14, gUnused_c,   gZtcMsgFlagAsync_c,   AppZdpSize(zbUserDescriptorSet_t),               gZtcMsgFmtSimple_c             ) /* ZDP-User_Desc_set                    */
AppZdpTbl(0x15, gUnused_c,   gZtcMsgFlagAsync_c,   AppZdpSize(zbSystemServerDiscoveryRequest_t),    gZtcMsgFmtSimple_c             ) /* ZDP-System_Server_Discovery.Request  */
AppZdpTbl(0x16, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                                       gZtcMsgFmtZdpDiscStoreReq_c    ) /* ZDP-Discovery_Store.Request          */
AppZdpTbl(0x17, gUnused_c,   gZtcMsgFlagAsync_c,   AppZdpSize(zbNodeDescriptorStoreRequest_t),      gZtcMsgFmtSimple_c             ) /* ZDP-Node_Desc_Store.Request          */
AppZdpTbl(0x18, gUnused_c,   gZtcMsgFlagAsync_c,   AppZdpSize(zbPowerDescriptorStoreRequest_t),     gZtcMsgFmtSimple_c             ) /* ZDP-Power_Desc_Store.Request         */
AppZdpTbl(0x19, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                                       gZtcMsgFmtActiveEPStoreReq_c   ) /* ZDP-Active_EP_Store.Request          */
AppZdpTbl(0x1A, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                                       gZtcMsgFmtSimpleDescStoreReq_c ) /* ZDP-Simple_Desc_Store.Request        */
AppZdpTbl(0x1B, gUnused_c,   gZtcMsgFlagAsync_c,   AppZdpSize(zbRemoveNodeCacheRequest_t),          gZtcMsgFmtSimple_c             ) /* ZDP-Remove_Node_Cache.Request        */
AppZdpTbl(0x1C, gUnused_c,   gZtcMsgFlagAsync_c,   AppZdpSize(zbFindNodeCacheRequest_t),            gZtcMsgFmtSimple_c             ) /* ZDP-Find_Node_Cache.Request          */
AppZdpTbl(0x1D, gUnused_c,   gZtcMsgFlagAsync_c,   AppZdpSize(zbExtSimpleDescriptorRequest_t),      gZtcMsgFmtSimple_c             ) /* ZDP-ExtendedSimpleDescriptor.Request */
AppZdpTbl(0x1E, gUnused_c,   gZtcMsgFlagAsync_c,   AppZdpSize(zbExtActiveEpRequest_t),              gZtcMsgFmtSimple_c             ) /* ZDP-ExtendedActive_EP_req.Request    */
AppZdpTbl(0x20, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                                       gZtcMsgFmtZdpEndDeviceBindReq_c) /* ZDP-End_Device_Bind.Request          */
AppZdpTbl(0x21, gUnused_c,   gZtcMsgFlagAsync_c,   AppZdpSize(zbBindUnbindRequest_t),               gZtcMsgFmtSimple_c             ) /* ZDP-BIND.Request                     */
AppZdpTbl(0x22, gUnused_c,   gZtcMsgFlagAsync_c,   AppZdpSize(zbBindUnbindRequest_t),               gZtcMsgFmtSimple_c             ) /* ZDP-UNBIND.Request                   */
AppZdpTbl(0x23, gUnused_c,   gZtcMsgFlagAsync_c,   AppZdpSize(zbBindRegisterRequest_t),             gZtcMsgFmtSimple_c             ) /* ZDP-Bind_Register.Request            */
AppZdpTbl(0x24, gUnused_c,   gZtcMsgFlagAsync_c,   AppZdpSize(zbReplaceDeviceRequest_t),            gZtcMsgFmtSimple_c             ) /* ZDP-Replace_Device.Request           */
AppZdpTbl(0x25, gUnused_c,   gZtcMsgFlagAsync_c,   AppZdpSize(zbStoreBkupBindEntryRequest_t),       gZtcMsgFmtSimple_c             ) /* ZDP-Store_Bkup_Bind_entry.Request    */
AppZdpTbl(0x26, gUnused_c,   gZtcMsgFlagAsync_c,   AppZdpSize(zbRemoveBackupBindEntryRequest_t),    gZtcMsgFmtSimple_c             ) /* ZDP-Remove_Bkup_Bind_Entry.Request   */
AppZdpTbl(0x27, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                                       gZtcMsgFmtBkupBindTblReq_c     ) /* ZDP-Backup_Bind_Table.Request        */
AppZdpTbl(0x28, gUnused_c,   gZtcMsgFlagAsync_c,   AppZdpSize(zbRecoverBindTableRequest_t),         gZtcMsgFmtSimple_c             ) /* ZDP-Recover_Bind_Table.Request       */
AppZdpTbl(0x29, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                                       gZtcMsgFmtBkupSrcBindReq_c     ) /* ZDP-Backup_Source_Bind.Request       */
AppZdpTbl(0x2A, gUnused_c,   gZtcMsgFlagAsync_c,   AppZdpSize(zbRecoverSourceBindRequest_t),        gZtcMsgFmtSimple_c             ) /* ZDP-Recover_Source_Bind.Request      */
AppZdpTbl(0x30, gUnused_c,   gZtcMsgFlagAsync_c,   AppZdpSize(zbMgmtNwkDiscoveryRequest_t),         gZtcMsgFmtSimple_c             ) /* ZDP-Mgmt_Nwk_Disc.Request            */
AppZdpTbl(0x31, gUnused_c,   gZtcMsgFlagAsync_c,   AppZdpSize(zbMgmtLqiRequest_t),                  gZtcMsgFmtSimple_c             ) /* ZDP-Mgmt_Lqi.Request                 */
AppZdpTbl(0x32, gUnused_c,   gZtcMsgFlagAsync_c,   AppZdpSize(zbMgmtRtgRequest_t),                  gZtcMsgFmtSimple_c             ) /* ZDP-Mgmt_Rtg.Request                 */
AppZdpTbl(0x33, gUnused_c,   gZtcMsgFlagAsync_c,   AppZdpSize(zbMgmtBindRequest_t),                 gZtcMsgFmtSimple_c             ) /* ZDP-Mgmt_Bind.Request                */
AppZdpTbl(0x34, gUnused_c,   gZtcMsgFlagAsync_c,   AppZdpSize(zbMgmtLeaveRequest_t),                gZtcMsgFmtSimple_c             ) /* ZDP-Mgmt_Leave.Request               */
AppZdpTbl(0x35, gUnused_c,   gZtcMsgFlagAsync_c,   AppZdpSize(zbMgmtDirectJoinRequest_t),           gZtcMsgFmtSimple_c             ) /* ZDP-Mgmt_Direct_Join.Request         */
AppZdpTbl(0x36, gUnused_c,   gZtcMsgFlagAsync_c,   AppZdpSize(zbMgmtPermitJoiningRequest_t),        gZtcMsgFmtSimple_c             ) /* ZDP-Mgmt_Permit_Joining.Request      */
AppZdpTbl(0x37, gUnused_c,   gZtcMsgFlagAsync_c,   AppZdpSize(zbMgmtCacheRequest_t),                gZtcMsgFmtSimple_c             ) /* ZDP-Mgmt_Cache.Request               */
AppZdpTbl(0x39, gUnused_c,   gZtcMsgFlagAsync_c,   AppZdpSize(zbMgmtDirectJoinRequest_t),           gZtcMsgFmtSimple_c             ) /* ZDP-NLME-DIRECT-JOIN.Request         */
AppZdpTbl(0x38, gUnused_c,   gZtcMsgFlagAsync_c,   AppZdpSize(zbMgmtNwkUpdateRequest_t),            gZtcMsgFmtSimple_c             ) /* ZDP-Mgmt_Nwk_Update.Request         */
AppZdpTbl(0xB8, gUnused_c,   gZtcMsgFlagAsync_c,   sizeof(zbMgmtNwkUpdateNotify_t),                 gZtcMsgFmtZdpMgmtNwkUpdateNotifyRequest_c ) /* ZDP-MgmtNwkUpdateNotify.notify    */
AppZdpTbl(0x3C, 0x4C,        gZtcMsgFlagAsync_c,   sizeof(zdpNlmeSyncReq_t),                        gZtcMsgFmtSimple_c             ) /* ZDP-NLME-SYNC.Request                */
//AppZdpTbl(0x50, 0x41,        gZtcMsgFlagAsync_c,   sizeof(zdoNlmeNetworkDiscoveryReq_t),            gZtcMsgFmtSimple_c             ) /* ZDP-NLME-NETWORK-DISCOVERY.Request   */
AppZdpTbl(0x79, gUnused_c,   gZtcMsgFlagAsync_c,   sizeof(nlmePermitJoiningReq_t),                   gZtcMsgFmtSimple_c             ) /* ZDP-NLME-NETWORK-DISCOVERY.Request   */
//AppZdpTbl(0xCF, gUnused_c,   gZtcMsgFlagAsync_c,   AppZdpSize(zdoApsmeEstKeyReq_t),                 gZtcMsgFmtSimple_c             ) /* ZDP-APSME-ESTABLISH-KEY.Request      */
//AppZdpTbl(0xD1, gUnused_c,   gZtcMsgFlagAsync_c,   AppZdpSize(zdoApsmeTranspKeyReq_t),              gZtcMsgFmtSimple_c             ) /* ZDP-APSME-TRANSPORT-KEY.Request      */
//AppZdpTbl(0xD3, gUnused_c,   gZtcMsgFlagAsync_c,   AppZdpSize(zdoApsmeRequestKeyReq_t),             gZtcMsgFmtSimple_c             ) /* ZDP-APSME-REQUEST-KEY.Request        */
//AppZdpTbl(0xD4, gUnused_c,   gZtcMsgFlagAsync_c,   AppZdpSize(zdoApsmeSwitchKeyReq_t),              gZtcMsgFmtSimple_c             ) /* ZDP-APSME-SWITCH-KEY.Request         */
//AppZdpTbl(0xF3, gUnused_c,   gZtcMsgFlagAsync_c,   AppZdpSize(zdoApsmeUpdateDevReq_t),              gZtcMsgFmtSimple_c             ) /* ZDP-APSME-UPDATE-DEVICE.Request      */
//AppZdpTbl(0xF4, gUnused_c,   gZtcMsgFlagAsync_c,   AppZdpSize(zdoApsmeRemoveDevReq_t),              gZtcMsgFmtSimple_c             ) /* ZDP-APSME-REMOVE-DEVICE.Request      */

#undef AppZdpSize
#undef AppZdpTbl

#ifndef ZdpAppTbl
#define ZdpAppTbl(msgType, cnfType, flags, len, format)
#endif

#define ZdpAppSize(type)  (sizeof(type) + MbrOfs(zdpToAppMessage_t, msgData) - MbrSizeof(zdpToAppMessage_t, msgType))

/* gZDPAppSAPHandlerId_c = 0xA0, ZDP_APP_SapHandler(). gaZtcZdpAppMsgTypeTable[] */
//ZdpAppTbl(0x41, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                                       gZtcMsgFmtZdpNlmeNwkDiscCnf_c  ) /* ZDP-NLME-NETWORK-DISCOVERY.Confirm    */
ZdpAppTbl(0x47, gUnused_c,   gZtcMsgFlagAsync_c,   ZdpAppSize(nlmeDirectJoinConf_t),                gZtcMsgFmtSimple_c             ) /* ZDP-NLME-DIRECT-JOIN.Confirm          */
ZdpAppTbl(0x4B, gUnused_c,   gZtcMsgFlagAsync_c,   gZero_c,                                         gZtcMsgFmtSimple_c             ) /* ZDP-NLME-SYNC.Indication              */
ZdpAppTbl(0x4C, gUnused_c,   gZtcMsgFlagAsync_c,   ZdpAppSize(nlmeSyncConf_t),                      gZtcMsgFmtSimple_c             ) /* ZDP-NLME-SYNC.Confirm                 */
ZdpAppTbl(0x13, gUnused_c,   gZtcMsgFlagAsync_c,   ZdpAppSize(zbEndDeviceAnnounce_t),               gZtcMsgFmtSimple_c             ) /* ZDP-EndDeviceAnnounce.Indication      */
/* Note that ZDP-NWK_addr.response and ZDP-IEEE_addr.response use the same format. */
ZdpAppTbl(0x80, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                                       gZtcMsgFmtNwkAddrResp_c        ) /* ZDP-NWK_addr.response                 */
ZdpAppTbl(0x81, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                                       gZtcMsgFmtNwkAddrResp_c        ) /* ZDP-IEEE_addr.response                */
ZdpAppTbl(0x82, gUnused_c,   gZtcMsgFlagAsync_c,   ZdpAppSize(zbNodeDescriptorResponse_t),          gZtcMsgFmtSimple_c             ) /* ZDP-NodeDescriptor.Response           */
ZdpAppTbl(0x83, gUnused_c,   gZtcMsgFlagAsync_c,   ZdpAppSize(zbPowerDescriptorResponse_t),         gZtcMsgFmtSimple_c             ) /* ZDP-PowerDescriptor.Response          */
ZdpAppTbl(0x84, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                                       gZtcMsgFmtZdpSimpleDescResp_c  ) /* ZDP-SimpleDescriptor.Response         */
ZdpAppTbl(0x85, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                                       gZtcMsgFmtZdpActiveEPResp_c    ) /* ZDP-Active_EP_rsp.response            */
ZdpAppTbl(0x86, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                                       gZtcMsgFmtZdpMatchDescResp_c   ) /* ZDP-Match_Desc_resp.response          */
ZdpAppTbl(0x90, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                                       gZtcMsgFmtComplexDescResp_c    ) /* ZDP-Complex_Desc_rsp                  */
ZdpAppTbl(0x91, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                                       gZtcMsgFmtUserDescResp_c       ) /* ZDP-UserDescriptor.Response           */
ZdpAppTbl(0x92, gUnused_c,   gZtcMsgFlagAsync_c,   ZdpAppSize(zbDiscoveryCacheResponse_t),          gZtcMsgFmtSimple_c             ) /* ZDP-Discovery_Cache_response          */
ZdpAppTbl(0x94, gUnused_c,   gZtcMsgFlagAsync_c,   ZdpAppSize(zbUserDescriptorConfirm_t),           gZtcMsgFmtSimple_c             ) /* ZDP-User_Desc_conf                    */
ZdpAppTbl(0x95, gUnused_c,   gZtcMsgFlagAsync_c,   ZdpAppSize(zbSystemServerDiscoveryResponse_t),   gZtcMsgFmtSimple_c             ) /* ZDP-System_Server_Discovery.response  */
ZdpAppTbl(0x96, gUnused_c,   gZtcMsgFlagAsync_c,   ZdpAppSize(zbDiscoveryStoreResponse_t),          gZtcMsgFmtSimple_c             ) /* ZDP-Discovery_store.response          */
ZdpAppTbl(0x97, gUnused_c,   gZtcMsgFlagAsync_c,   ZdpAppSize(zbNodeDescriptorStoreResponse_t),     gZtcMsgFmtSimple_c             ) /* ZDP-Node_Desc_store.response          */
ZdpAppTbl(0x98, gUnused_c,   gZtcMsgFlagAsync_c,   ZdpAppSize(zbPowerDescriptorStoreResponse_t),    gZtcMsgFmtSimple_c             ) /* ZDP-Power_Desc_store.response         */
ZdpAppTbl(0x99, gUnused_c,   gZtcMsgFlagAsync_c,   ZdpAppSize(zbActiveEPStoreResponse_t),           gZtcMsgFmtSimple_c             ) /* ZDP-Active_EP_store.response          */
ZdpAppTbl(0x9A, gUnused_c,   gZtcMsgFlagAsync_c,   ZdpAppSize(zbSimpleDescriptorStoreResponse_t),   gZtcMsgFmtSimple_c             ) /* ZDP-Simple_Desc_store.response        */
ZdpAppTbl(0x9B, gUnused_c,   gZtcMsgFlagAsync_c,   ZdpAppSize(zbRemoveNodeCacheResponse_t),         gZtcMsgFmtSimple_c             ) /* ZDP-Remove_node_cache.response        */
ZdpAppTbl(0x9C, gUnused_c,   gZtcMsgFlagAsync_c,   ZdpAppSize(zbFindNodeCacheResponse_t),           gZtcMsgFmtSimple_c             ) /* ZDP-Find_node_cache.response          */
ZdpAppTbl(0x9D, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                                       gZtcMsgFmtZdpExtSimpleDescResp_c  ) /* ZDP-ExtendedSimpleDescriptor.Response */
ZdpAppTbl(0x9E, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                                       gZtcMsgFmtZdpExtActiveEPResp_c  ) /* ZDP-ExtendedActiveEP.Response */
ZdpAppTbl(0xA0, gUnused_c,   gZtcMsgFlagAsync_c,   ZdpAppSize(zbEndDeviceBindResponse_t),           gZtcMsgFmtSimple_c             ) /* ZDP-End_Device_Bind.response          */
ZdpAppTbl(0xA1, gUnused_c,   gZtcMsgFlagAsync_c,   ZdpAppSize(zbBindUnbindResponse_t),              gZtcMsgFmtSimple_c             ) /* ZDP-BIND.Resp                         */
ZdpAppTbl(0xA2, gUnused_c,   gZtcMsgFlagAsync_c,   ZdpAppSize(zbBindUnbindResponse_t),              gZtcMsgFmtSimple_c             ) /* ZDP-UNBIND.Response                   */
ZdpAppTbl(0xA3, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                                       gZtcMsgFmtZdpBindRegResp_c     ) /* ZDP-Bind_Register.response            */
ZdpAppTbl(0xA4, gUnused_c,   gZtcMsgFlagAsync_c,   ZdpAppSize(zbReplaceDeviceResponse_t),           gZtcMsgFmtSimple_c             ) /* ZDP-Replace_Device.response           */
ZdpAppTbl(0xA5, gUnused_c,   gZtcMsgFlagAsync_c,   ZdpAppSize(zbStoreBkupBindEntryResponse_t),      gZtcMsgFmtSimple_c             ) /* ZDP-Store_Bkup_Bind_entry.response    */
ZdpAppTbl(0xA6, gUnused_c,   gZtcMsgFlagAsync_c,   ZdpAppSize(zbRemoveBackupBindEntryResponse_t),   gZtcMsgFmtSimple_c             ) /* ZDP-Remove_Bkup_Bind_entry.response   */
#if gBindCapability_d
ZdpAppTbl(0xA7, gUnused_c,   gZtcMsgFlagAsync_c,   ZdpAppSize(zbBackupBindTableResponse_t),         gZtcMsgFmtSimple_c             ) /* ZDP-Backup_Bind_Table.response        */
ZdpAppTbl(0xA8, gUnused_c,   gZtcMsgFlagAsync_c,   ZdpAppSize(zbRecoverBindTableResponse_t),        gZtcMsgFmtZdpRecoverBindTblResp_c   ) /* ZDP-Recover_Bind_Table.response       */
ZdpAppTbl(0xA9, gUnused_c,   gZtcMsgFlagAsync_c,   ZdpAppSize(zbBackupSourceBindResponse_t),        gZtcMsgFmtSimple_c             ) /* ZDP-Backup_Source_Bind.response       */
#endif
ZdpAppTbl(0xAA, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                                       gZtcMsgFmtZdpRecoverSrcBindResp_c   ) /* ZDP-Recover_Source_Bind.response      */
ZdpAppTbl(0xB0, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                                       gZtcMsgFmtZdpMgmtNwkDiscResp_c ) /* ZDP-Mgmt_Nwk_Disc.Response            */
ZdpAppTbl(0xB1, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                                       gZtcMsgFmtZdpMgmtLqiResp_c     ) /* ZDP-Mgmt_Lqi.Response                 */
ZdpAppTbl(0xB2, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                                       gZtcMsgFmtZdpMgmtRtgResp_c     ) /* ZDP-Mgmt_Rtg.Response                 */
ZdpAppTbl(0xB3, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                                       gZtcMsgFmtZdpMgmtBindResp_c    ) /* ZDP-Mgmt_Bind.Response                */
ZdpAppTbl(0xB4, gUnused_c,   gZtcMsgFlagAsync_c,   ZdpAppSize(zbMgmtLeaveResponse_t),               gZtcMsgFmtSimple_c             ) /* ZDP-Mgmt_Leave.Response               */
ZdpAppTbl(0xB5, gUnused_c,   gZtcMsgFlagAsync_c,   ZdpAppSize(zbMgmtDirectJoinResponse_t),          gZtcMsgFmtSimple_c             ) /* ZDP-Mgmt_Direct_Join.Response         */
ZdpAppTbl(0xB6, gUnused_c,   gZtcMsgFlagAsync_c,   ZdpAppSize(zbMgmtPermitJoiningResponse_t),       gZtcMsgFmtSimple_c             ) /* ZDP-Mgmt_Permit_Join.response         */
ZdpAppTbl(0xB7, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                                       gZtcMsgFmtZdpMgmtCacheResp_c   ) /* ZDP-Mgmt_Cache.response               */
ZdpAppTbl(0xB8, gUnused_c,   gZtcMsgFlagAsync_c,   ZdpAppSize(zbMgmtNwkUpdateNotify_t),             gZtcMsgFmtZdpMgmtNwkUpdateNotify_c ) /* ZDP-MgmtNwkUpdateNotify.notify    */
/* Secure Indictions to the Application. */
ZdpAppTbl(0xD6, gUnused_c,   gZtcMsgFlagAsync_c,   sizeof(zbApsmeEstablishKeyInd_t),                gZtcMsgFmtSimple_c             ) /* ZDP-ESTABLISH-KEY.indication   */
ZdpAppTbl(0xD8, gUnused_c,   gZtcMsgFlagAsync_c,   sizeof(zbApsmeRemoveDeviceInd_t),                gZtcMsgFmtSimple_c             ) /* ZDP-REMOVE-DEVICE.indication   */
ZdpAppTbl(0xD9, gUnused_c,   gZtcMsgFlagAsync_c,   sizeof(zbApsmeRequestKeyInd_t),                  gZtcMsgFmtSimple_c             ) /* ZDP-REQUEST-KEY.Indication     */
ZdpAppTbl(0xF1, gUnused_c,   gZtcMsgFlagAsync_c,   sizeof(zbApsmeSwitchKeyInd_t),                   gZtcMsgFmtSimple_c             ) /* ZDP-SWITCH-KEY.Indication      */
ZdpAppTbl(0xF2, gUnused_c,   gZtcMsgFlagAsync_c,   sizeof(zbApsmeTransportKeyInd_t),                gZtcMsgFmtSimple_c             ) /* ZDP-TRANSPORT-KEY.Indication   */
ZdpAppTbl(0xD7, gUnused_c,   gZtcMsgFlagAsync_c,   sizeof(zbApsmeUpdateDeviceInd_t),                gZtcMsgFmtSimple_c             ) /* ZDP-UPDATE-DEVICE.Indication   */
ZdpAppTbl(0x0B, 0x0B,        gZtcMsgFlagAsync_c,   gZero_c,                                         gZtcMsgFmtSimple_c             ) /* ZDP-RESET.Confirm              */
ZdpAppTbl(0xD5, gUnused_c,   gZtcMsgFlagAsync_c,   sizeof(zbApsmeCnf_t),                            gZtcMsgFmtSimple_c             ) /* ZDP-ESTABLISH-KEY.Confirm      */
ZdpAppTbl(0xF6, gUnused_c,   gZtcMsgFlagAsync_c,   sizeof(zbApsmeAuthenticateInd_t),                gZtcMsgFmtSimple_c             ) /* ZDP-AUTHENTICADTE.Indication   */
ZdpAppTbl(0xF7, gUnused_c,   gZtcMsgFlagAsync_c,   sizeof(zbApsmeCnf_t),                            gZtcMsgFmtSimple_c             ) /* ZDP-AUTHENTICADTE.confirm      */
ZdpAppTbl(0xE6, gUnused_c,   gZtcMsgFlagAsync_c,   ZdpAppSize(zdo2AppEvent_t),                      gZtcMsgFmtSimple_c             ) /* ZDP-SystemEvent.Indication           */
ZdpAppTbl(0xF9, gUnused_c,   gZtcMsgFlagAsync_c,   ZdpAppSize(nlmePermitJoiningConf_t),             gZtcMsgFmtSimple_c             ) /*                */
ZdpAppTbl(0x70, gUnused_c,   gZtcMsgFlagAsync_c,   sizeof(zbStatus_t),                              gZtcMsgFmtSimple_c             )
ZdpAppTbl(0xCC, gUnused_c,   gZtcMsgFlagAsync_c,   sizeof(zbStatus_t),                              gZtcMsgFmtSimple_c             )
ZdpAppTbl(0x58, gUnused_c,   gZtcMsgFlagAsync_c,   sizeof(nlmeNwkTxReport_t),                       gZtcMsgFmtSimple_c             ) /* NLME-NWK-Tx_report */
#undef ZdpAppSize
#undef ZdpAppTbl


#ifndef AppInterPanTbl
#define AppInterPanTbl(msgType, cnfType, flags, len, format)
#endif
/* gInterPanAppSAPHandlerId_c = 0xA5, APP_InterPan_SapHandler(). gaZtcInterPanAppMsgTypeTable[] */
AppInterPanTbl(0x00, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                gZtcMsgFmtInterPanDataReq_c             ) /* InterPan-Data.Request */
#undef AppInterPanSize
#undef AppInterPanTbl

#ifndef InterPanAppTbl
#define InterPanAppTbl(msgType, cnfType, flags, len, format)
#endif

#define AppInterPanSize(type)  (sizeof(type) + MbrOfs(InterPanMessage_t, msgData) - MbrSizeof(InterPanMessage_t, msgType))

/* gInterPanAppSAPHandlerId_c = 0xA6, InterPan_ÁPP_SapHandler(). gaZtcAppInterPanMsgTypeTable[] */
InterPanAppTbl(0x01, gUnused_c,   gZtcMsgFlagAsync_c,   AppInterPanSize(zbInterPanDataConfirm_t),  gZtcMsgFmtSimple_c                 ) /* InterPan-Data.Confirm */
InterPanAppTbl(0x02, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                                 gZtcMsgFmtInterPanDataIndication_c ) /* InterPan-Data.Indication */
#undef InterPanAppSize
#undef InterPanAppTbl



#ifndef AppHcTbl
#define AppHcTbl(msgType, cnfType, flags, len, format)
#endif
/* gAppHcSAPHandlerId_c = 0xB6, APP_Hc_SapHandler(). gaZtcHcAppMsgTypeTable[] */
AppHcTbl(0x80, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                gZtcMsgFmtHc_c             ) 
AppHcTbl(0x81, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                gZtcMsgFmtHc_c             ) 
AppHcTbl(0x82, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                gZtcMsgFmtHc_c             ) 
AppHcTbl(0x83, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                gZtcMsgFmtHc_c             ) 
AppHcTbl(0x84, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                gZtcMsgFmtHc_c             ) 
AppHcTbl(0x85, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                gZtcMsgFmtHc_c             ) 
AppHcTbl(0x86, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                gZtcMsgFmtHc_c             ) 
AppHcTbl(0x87, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                gZtcMsgFmtHc_c             ) 
AppHcTbl(0x88, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                gZtcMsgFmtHc_c             ) 
AppHcTbl(0x89, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                gZtcMsgFmtHc_c             ) 
AppHcTbl(0xFD, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                gZtcMsgFmtHc_c             ) 
AppHcTbl(0xFE, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                gZtcMsgFmtHc_c             ) 
AppHcTbl(0xE2, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                gZtcMsgFmtHc_c             ) 
AppHcTbl(0xE3, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                gZtcMsgFmtHc_c             ) 
AppHcTbl(0xE4, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                gZtcMsgFmtHc_c             ) 
AppHcTbl(0xE5, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                gZtcMsgFmtHc_c             ) 
AppHcTbl(0xE6, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                gZtcMsgFmtHc_c             ) 
AppHcTbl(0xE7, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                gZtcMsgFmtHc_c             ) 
AppHcTbl(0x10, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                gZtcMsgFmtHc_c             ) 
AppHcTbl(0x11, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                gZtcMsgFmtHc_c             ) 
AppHcTbl(0x13, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                gZtcMsgFmtHc_c             ) 
AppHcTbl(0x14, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                gZtcMsgFmtHc_c             ) 
AppHcTbl(0x15, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                gZtcMsgFmtHc_c             ) 
AppHcTbl(0x16, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                gZtcMsgFmtHc_c             ) 
AppHcTbl(0x17, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                gZtcMsgFmtHc_c             ) 
AppHcTbl(0x21, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                gZtcMsgFmtHc_c             ) 
AppHcTbl(0x23, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                gZtcMsgFmtHc_c             ) 
AppHcTbl(0x25, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                gZtcMsgFmtHc_c             ) 
AppHcTbl(0x27, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                gZtcMsgFmtHc_c             ) 
AppHcTbl(0x30, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                gZtcMsgFmtHc_c             ) 
AppHcTbl(0x40, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                gZtcMsgFmtHc_c             ) 

#undef AppHcSize
#undef AppHcTbl

#ifndef HcAppTbl
#define HcAppTbl(msgType, cnfType, flags, len, format)
#endif

#define AppHcSize(type)  (sizeof(type) + MbrOfs(InterPanMessage_t, msgData) - MbrSizeof(InterPanMessage_t, msgType))

/* gHcAppSAPHandlerId_c = 0xB5, Hc_ÁPP_SapHandler(). gaZtcAppHcMsgTypeTable[] */
HcAppTbl(0xB0, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                gZtcMsgFmtHc_c             )
HcAppTbl(0xB1, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                gZtcMsgFmtHc_c             )
HcAppTbl(0xB2, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                gZtcMsgFmtHc_c             )
HcAppTbl(0xB3, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                gZtcMsgFmtHc_c             )
HcAppTbl(0xB4, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                gZtcMsgFmtHc_c             )
HcAppTbl(0xB5, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                gZtcMsgFmtHc_c             )
HcAppTbl(0xC0, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                gZtcMsgFmtHc_c             )
HcAppTbl(0xE2, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                gZtcMsgFmtHc_c             ) 
HcAppTbl(0xE3, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                gZtcMsgFmtHc_c             ) 
HcAppTbl(0xE4, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                gZtcMsgFmtHc_c             ) 
HcAppTbl(0xE5, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                gZtcMsgFmtHc_c             ) 
HcAppTbl(0xE6, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                gZtcMsgFmtHc_c             ) 
HcAppTbl(0xE7, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                gZtcMsgFmtHc_c             ) 
HcAppTbl(0x10, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                gZtcMsgFmtHc_c             ) 
HcAppTbl(0x11, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                gZtcMsgFmtHc_c             ) 
HcAppTbl(0x13, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                gZtcMsgFmtHc_c             ) 
HcAppTbl(0x14, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                gZtcMsgFmtHc_c             ) 
HcAppTbl(0x15, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                gZtcMsgFmtHc_c             ) 
HcAppTbl(0x16, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                gZtcMsgFmtHc_c             ) 
HcAppTbl(0x17, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                gZtcMsgFmtHc_c             ) 
HcAppTbl(0x21, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                gZtcMsgFmtHc_c             ) 
HcAppTbl(0x23, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                gZtcMsgFmtHc_c             ) 
HcAppTbl(0x25, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                gZtcMsgFmtHc_c             ) 
HcAppTbl(0x27, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                gZtcMsgFmtHc_c             ) 
HcAppTbl(0x30, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                gZtcMsgFmtHc_c             ) 
HcAppTbl(0x40, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                gZtcMsgFmtHc_c             ) 

HcAppTbl(0xFD, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                gZtcMsgFmtHc_c             ) 
HcAppTbl(0xFE, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                gZtcMsgFmtHc_c             ) 
// HcAppTbl(0x02, gUnused_c,   gZtcMsgFlagAsync_c,   gUnused_c,                gZtcMsgFmtHcDataIndication_c             ) /* Hc-Data.Indication */
#undef HcAppSize
#undef HcAppTbl




/* gZtcReq_HandlerId_c = 0xA3, ZtcReq_Handler().
 *
 * These messages are only exchanged between the external client and Ztc;
 * they are never sent by or sent to any other BeeStack code. No message type
 * info table is needed. The confirmations use opcode group 0xA4, and use the
 * same message types as the requests.
 *
 * This table is a different form from the others; Ztc does not need as much
 * table information about messages sent to it.
 *
 * These are all synchronous requests, handled internally by Ztc. Each of
 * them has a confirmation with the same messsage type and the converse
 * opcode group.
 *
 * The integer values of the message types must match the Test Tool xml.
 */
#ifndef ZtcMsgTbl
#define ZtcMsgTbl(msgType, configFunc)
#endif
ZtcMsgTbl(mZtcMsgModeSelectReq_c,           ZtcMsgModeSelectReqFunc)            /* ZTC-ModeSelect.Request.       */
ZtcMsgTbl(mZtcMsgGetModeReq_c,              ZtcMsgGetModeReqFunc)               /* ZTC-GetMode.Request.          */
#if gUseHwValidation_c
ZtcMsgTbl(mZtcLowLevelMemoryWriteByte_c,    ZtcMemoryWriteByte)                 /* ZTC-RamWriteBytet.Request.    */
ZtcMsgTbl(mZtcLowLevelMemoryReadByte_c,     ZtcMemoryReadByte)                  /* ZTC-RamReadBytet.Request.     */
ZtcMsgTbl(mZtcLowLevelMemoryWriteWord_c,    ZtcMemoryWriteWord)                 /* ZTC-RamWriteWordt.Request.    */
ZtcMsgTbl(mZtcLowLevelMemoryReadWord_c,     ZtcMemoryReadWord)                  /* ZTC-RamReadWordt.Request.     */
ZtcMsgTbl(mZtcLowLevelMemoryWriteLong_c,    ZtcMemoryWriteLong)                 /* ZTC-RamWriteLongt.Request.    */
ZtcMsgTbl(mZtcLowLevelMemoryReadLong_c,     ZtcMemoryReadLong)                  /* ZTC-RamReadLongt.Request.     */
ZtcMsgTbl(mZtcLowLevelMemoryWriteBlock_c,   ZtcMemoryWriteBlock)                /* ZTC-RamWriteBlockt.Request.   */
ZtcMsgTbl(mZtcLowLevelMemoryReadBlock_c,    ZtcMemoryReadBlock)                 /* ZTC-RamReadBlockt.Request.    */
ZtcMsgTbl(mZtcLowLevelPing_c,               ZtcPing)                            /* ZTC-Pingt.Request.            */
#endif /* gUseHwValidation_c */

#if gBeeStackIncluded_d
ZtcMsgTbl(mZtcMsgAFResetReq_c,              ZtcMsgAFResetReqFunc)               /* ZTC-AFReset.Request.                 */
ZtcMsgTbl(mZtcMsgAPSResetReq_c,             ZtcMsgAPSResetReqFunc)              /* ZTC-APSReset.Request.                */
#endif
ZtcMsgTbl(mZtcMsgResetCPUReq_c,             ZtcMsgResetCPUReqFunc)              /* ZTC-CPU_Reset.Request.               */
#if gBeeStackIncluded_d && gZtcCommands_d
ZtcMsgTbl(mZtcMsgGetEpMaxWindowSize_c,      ZtcMsgGetEpMaxWindowSizeFunc)       /* ZTC-DeregisterEndPoint.Request       */
ZtcMsgTbl(mZtcMsgDeregisterEndPoint_c,      ZtcMsgDeregisterEndPointFunc)       /* ZTC-DeregisterEndPoint.Request       */
ZtcMsgTbl(mZtcMsgRegisterEndPoint_c,        ZtcMsgRegisterEndPointFunc)         /* ZTC-RegisterEndPoint.Request         */
ZtcMsgTbl(mZtcMsgGetNumberOfEndPoints_c,    ZtcMsgGetNumberOfEndPointsFunc)     /* ZTC-GetNumberOfEndPoints.Request     */
ZtcMsgTbl(mZtcMsgGetEndPointDescription_c,  ZtcMsgGetEndPointDescriptionFunc)   /* ZTC-GetEndPointDescription.Request   */
ZtcMsgTbl(mZtcMsgGetEndPointIdList_c,       ZtcMsgGetEndPointIdListFunc)        /* ZTC-GetEndPointIdList.Request        */
ZtcMsgTbl(mZtcMsgSetEpMaxWindowSize_c,      ZtcMsgSetEpMaxWindowSizeFunc)       /* ZTC-DeregisterEndPoint.Request       */
#if gApsLinkKeySecurity_d
ZtcMsgTbl(mZtcMsgGetApsDeviceKeyPairSet_c,  ZtcMsgGetApsDeviceKeyPairSet)
ZtcMsgTbl(mZtcMsgSetApsDeviceKeyPairSet_c,  ZtcMsgSetApsDeviceKeyPairSet)
ZtcMsgTbl(mZtcMsgClearApsDeviceKeyPairSet_c,  ZtcMsgClearApsDeviceKeyPairSet)
#endif
#if gICanHearYouTableCapability_d
ZtcMsgTbl(mZtcMsgGetICanHearYouList_c,      ZtcMsgGetICanHearYouListFunc)       /* ZTC-GetICanHearYouList.Request       */
ZtcMsgTbl(mZtcMsgSetICanHearYouList_c,      ZtcMsgSetICanHearYouListFunc)       /* ZTC-SetICanHearYouList.Request       */
#endif
ZtcMsgTbl(mZtcMsgGetChannelReq_c,           ZtcMsgGetChannelReqFunc)            /* ZTC-GetChannel.Request               */
ZtcMsgTbl(mZtcMsgSetChannelReq_c,           ZtcMsgSetChannelReqFunc)            /* ZTC-SetChannel.Request               */
ZtcMsgTbl(mZtcMsgGetPanIDReq_c,             ZtcMsgGetPanIDReqFunc)              /* ZTC-GetPanID.Request                 */
ZtcMsgTbl(mZtcMsgSetPanIDReq_c,             ZtcMsgSetPanIDReqFunc)              /* ZTC-SetPanID.Request                 */
#if (gZtcIncluded_d && (gStandardSecurity_d || gHighSecurity_d) && gApsMaxEntriesForPermissionsTable_c)
ZtcMsgTbl(mZtcMsgGetPermissionsTable_c,     ZtcMsgGetPermissionsTableFunc)      /* ZTC-GetPermissionsTable.Request      */
ZtcMsgTbl(mZtcMsgSetPermissionsTable_c,     ZtcMsgSetPermissionsTableFunc)      /* ZTC-SetPermissionsTable.Request      */
ZtcMsgTbl(mZtcMsgRemoveFromPermissionsTable_c,     ZtcMsgRemoveFromPermissionsTableFunc)    /* ZTC-RemoveFromPermissionsTable.Request      */
ZtcMsgTbl(mZtcMsgAddDeviceToPermissionsTable_c,     ZtcMsgAddDeviceToPermissionsTableFunc)    /* ZTC-RemoveFromPermissionsTable.Request      */
#endif  /* (gZtcIncluded_d && (gStandardSecurity_d || gHighSecurity_d) && gApsMaxEntriesForPermissionsTable_c) */
#endif  /* gBeeStackIncluded_d && gZtcCommands_d */
ZtcMsgTbl(mZtcMsgReadExtendedAdrReq_c,      ZtcMsgReadExtendedAdrReqFunc)       /* ZTC-ReadExtAddr.Request              */
ZtcMsgTbl(mZtcMsgWriteExtendedAdrReq_c,     ZtcMsgWriteExtendedAdrReqFunc)      /* ZTC-WriteExtAddr.Request             */
ZtcMsgTbl(mZtcMsgReadNwkMngAddressReq_c,    ZtcMsgReadNwkMngAddressReqFunc)     /* ZTC-ReadNwkMngAddr.Request           */
ZtcMsgTbl(mZtcMsgWriteNwkMngAddressReq_c,   ZtcMsgWriteNwkMngAddressReqFunc)    /* ZTC-WriteNwkMngAddr.Request          */
#if gBeeStackIncluded_d
ZtcMsgTbl(mZtcMsgStopNwkReq_c,              ZtcMsgStopNwkReqFunc)               /* ZTC-StopNwk.Request                  */
ZtcMsgTbl(mZtcMsgStartNwkReq_c,             ZtcMsgStartNwkReqFunc)              /* ZTC-StartNwk.Request                 */
ZtcMsgTbl(mZtcMsgStartNwkExReq_c,           ZtcMsgStartNwkExReqFunc)            /* ZTC-StartNwkEx.Request               */
ZtcMsgTbl(mZtcMsgStopNwkExReq_c,            ZtcMsgStopNwkExReqFunc)             /* ZTC-StopNwk.Request                  */
ZtcMsgTbl(mZtcMsgRestartNwkReq_c,           ZtcMsgRestartNwkReqFunc)            /* ZTC-RestartNwk.Request               */
ZtcMsgTbl(mZtcMsgNVSaveReq_c,               ZtcMsgNVSaveReqFunc)                /* ZTC-NVSave.Request.                  */
ZtcMsgTbl(mZtcMsgGetNVDataSetDescReq_c,     ZtcMsgGetNVDataSetDescReqFunc)      /* ZTC-NVGetDataSetDesc.Request.        */
ZtcMsgTbl(mZtcMsgGetNVPageHdrReq_c,         ZtcMsgGetNVPageHdrReqFunc)          /* ZTC-NVGetPageHeaders.Request.        */

/* Was APSME-GET.Request (99 03), APSME-GET.Confirm (98 09), */
/*     NLME=GET.Request  (96 3D), NLME-GET.Confirm  (97 4D). */
ZtcMsgTbl(mZtcMsgApsmeGetIBReq_c,          ZtcMsgGetIBReqFunc)                  /* ZTC-GetIB.Request, aka APSME-GET.Request     */
ZtcMsgTbl(mZtcMsgApsmeSetIBReq_c,          ZtcMsgSetIBReqFunc)                  /* ZTC-GetIB.Request, aka NLME-GET.Request      */

/* Was APSME-SET.Request (99 06), APSME-SET.Confirm (98 0A), */
/*     NLME-SET.Request  (96 3E), NLME-SET.Confirm  (97 4E). */
ZtcMsgTbl(mZtcMsgNlmeGetIBReq_c,           ZtcMsgGetIBReqFunc)                  /* ZTC-SetIB.Request, aka APSME-SET.Request     */
ZtcMsgTbl(mZtcMsgNlmeSetIBReq_c,           ZtcMsgSetIBReqFunc)                  /* ZTC-SetIB.Request, aka NLME-SET.Request      */
#if (gMsgInfo_d)
ZtcMsgTbl(mZtcMsgGetNumOfMsgsReq_c,        ZtcMsgGetNumOfMsgsReqFunc)           /* ZTC-SetIB.Request, aka NLME-SET.Request      */
#endif
#if (gZtcOtaSupportDebug_d)
ZtcMsgTbl(mZtcOtaSupportSetModeReq_c, ZtcOtaSupportSetModeFunc) 
ZtcMsgTbl(mZtcOtaSupportStartImageReq_c, ZtcOtaSupportStartImageReqFunc) 
ZtcMsgTbl(mZtcOtaSupportPushImageChunkReq_c, ZtcOtaSupportPushImageChunkReqFunc) 
ZtcMsgTbl(mZtcOtaSupportCommitImageReq_c, ZtcOtaSupportCommitImageReqFunc) 
ZtcMsgTbl(mZtcOtaSupportCancelImageReq_c, ZtcOtaSupportCancelImageReqFunc) 
ZtcMsgTbl(mZtcOtaSupportQueryImageRsp_c,  ZtcOtaSupportQueryNextImageRspFunc) 
ZtcMsgTbl(mZtcOtaSupportImageNotifyReq_c, ZtcOtaSupportImageNotifyFunc)
#endif
#if (gSAPMessagesEnableNlme_d)
ZtcMsgTbl(mZtcMsgFreeDiscoveryTablesReq_c, ZtcFreeDiscoveryTables)              /* ZTC-SetIB.Request, aka NLME-SET.Request      */
ZtcMsgTbl(mZtcMsgSetJoinFilterFlagReq_c,   ZtcSetJoinFilterFlag)                /* ZTC-SetIB.Request, aka NLME-SET.Request      */
#endif
ZtcMsgTbl(mZtcMsgGetMaxApplicationPayloadReq_c, ZtcGetMaxApplicationPayload)    /* ZTC-GetMaxApplicationPayload.Request         */
#endif /* gBeeStackIncluded_d */
ZtcMsgTbl(mZtcMsgGetSeedReq_c,               ZtcMsgGetSeedReqFunc)
ZtcMsgTbl(mZtcMsgAddToAddressMapPermanent_c, ZtcMsgAddToAddressMapPermanentFunc)
ZtcMsgTbl(mZtcMsgRemoveFromAddressMap_c,     ZtcMsgRemoveFromAddressMapFunc)

#undef ZtcMsgTbl
