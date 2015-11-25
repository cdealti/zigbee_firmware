/******************************************************************************
* ZtcInterface.h
* "ZigBee Test Client": an optional tool to interface between a PC-hosted
* client and the SAP Handlers. Also provides a general hook for
* communication to/from other embedded code, and some extra utility functions.
*
* Copyright (c) 2008, Freescale, Inc.  All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from  Freescale Semiconductor.
*
******************************************************************************/

#ifndef _ZtcInterface_h
#define _ZtcInterface_h

#ifndef gZtcIncluded_d
#define gZtcIncluded_d  TRUE
#endif

#ifndef gOverrides_d
#define gOverrides_d    FALSE
#endif

#if gOverrides_d
#include "BeeStackOverrides.h"
#endif

#ifndef gMacStandAlone_d
#define gMacStandAlone_d            FALSE
#endif

#include "TS_Interface.h"

#define gCfgSCIInterface_d               0
#define gCfgI2CInterface_d               1

/* Define the default interface for the ZTC application */
#ifndef gZtcCommInterfaceType_d
  #define gZtcCommInterfaceType_d     gCfgSCIInterface_d
#endif 

#if (gZtcCommInterfaceType_d == gCfgSCIInterface_d)
#define gSCIInterface_d  TRUE
#define gI2CInterface_d FALSE
#else
#define gI2CInterface_d  TRUE
#define gSCIInterface_d FALSE
#endif


#if (gI2CInterface_d && gSCIInterface_d)
  #error Only one of I2C and SCI interfaces must be active !!!
#endif


/* Select UART as default interface if none is specified */
#if ((0 == gI2CInterface_d) && (0 == gSCIInterface_d))
  #undef gSCIInterface_d
  #define gSCIInterface_d TRUE
#endif



/* Verify if UART or IIC components are enabled */

#if gSCIInterface_d
#include "Uart_Interface.h"

#if (gZtcIncluded_d && gSCIInterface_d && (!gUart1_Enabled_d) && (!gUart2_Enabled_d))
#error UART module is not enabled !!!
#endif

#endif

#if gI2CInterface_d

#include "IIC_Interface.h"

#if (gZtcIncluded_d && gI2CInterface_d && (!gIIC_Enabled_d) )
#error I2C module is not enabled !!!
#endif

#endif



/* Ztc allows external tools, such as the Test Tool, to monitor messages
 * (requests and responses) passing through the SAP Handlers between stack
 * layers, and to inject messages for testing.
 *
 * There are historical and compatibility constraints on this code.
 * Some identifier spellings and constants must remain unchanged, for
 * compatiblity with extenal code, such as the Test Tool, and with
 * binary-only libraries, such as the MAC layer.
 *
 * It must be possible to use the MAC and other BeeStack libraries with or
 * without the Ztc features without recompiling those libraries. Ztc thus
 * comes in two versions: one that supports full functionality, and another
 * that consists only of non-functional stubs sufficient to satisfy the
 * linker symbols required by the rest of BeeStack. This header file must
 * be valid regardless of which version (functional or stub) of Ztc is used.
 */

/* Ztc sends and receives messages to and from SAP Handlers, but the
 * normal information flow is between stack layers, with SAP Handlers used
 * as go-betweens. Ztc must faithfully mimic the normal stack-layer-to-SAP-
 * Handler and SAP-Handler-to-stack-layer communication.
 *
 * Messages are either requests or responses to requests (aka confirmations).
 * Some requests can be satisfied immediately by the SAP Handler that receives
 * them; these are called "synchronous". To respond to a synchronous request,
 * a SAP Handler generally returns a status code as it's function value.
 * Some requests require more data than just a status return. For these,
 * the SAP Handler will modify the original request message, adding additional
 * data to it before returning.
 *
 * Requests that must be sent over the air to other ZigBee devices are called
 * "asynchronous". A response to an async request may come back at some
 * unpredictable future time, via the opposite SAP Handler.
 */

/* Messages sent to or received from external clients look like:
 *
 *  [STX][Opcode Group][MsgType][Len][Message][FCS]
 *
 * The Opcode Group uniquely specifies which SAP Handler the message should
 * be forwarded to. This value is used only by Ztc and the external client;
 * it is stripped from the messages before the message is forwarded to the
 * SAP Handler.
 *
 * The MsgType is used by the SAP Handlers to recognize which request/response
 * is contained by the message.
 *
 * The Opcode Group and MsgType together uniquely identify the purpose and
 * format of the message.
 */

/* Messages sent to or received from SAP Handlers look like:
 *
 *  [MsgType][Message]
 *
 * SAP Handlers know nothing about Opcode Groups. For historical reasons,
 * they instead use a comparable Internal SAP Id to identifier the SAP Handler
 * that is sending the message. The Internal SAP Id is given to Ztc as a
 * function parameter. Ztc must translate the Internal SAP Id to an Opcode
 * Group and add it to the message that is sent to the external client.
 */

/* Ztc uses sizeof() and comparable macros frequently, with *very* few magic
 * constants. In the interests of efficiency on small CPUs, and getting it
 * done in a reasonable time, it is not bulletproof. It should be able to
 * handle occasional thrown rocks (e.g. many kinds of changes in typedefs
 * in the rest of BeeStack) pretty well, requiring only a recompile and
 * possibly adjustments to table entries.
 */

/******************************************************************************
*******************************************************************************
* Public macros
*******************************************************************************
******************************************************************************/

/* Turning this on is only useful if gSAPMessagesEnableMlme_d is also on. */
#ifndef gZtcMacGetSetPIBCapability_d
#define gZtcMacGetSetPIBCapability_d    TRUE
#endif

#ifndef gZTC_RegEpCapability
/* Do not support ZTC register end point messages. */
#define gZTC_RegEpCapability        FALSE
#endif

#ifndef gZtcErrorReporting_d
#define gZtcErrorReporting_d        FALSE
#endif

#ifndef gZtcCommands_d
#define gZtcCommands_d  TRUE
#endif

/* Enable or disable compilation of message tables for various groups */
/* of SAP Handlers. These defines are for the same groups that the */
/* Test Tool uses for setting modes. */
#ifndef gSAPMessagesEnableMcps_d
#define gSAPMessagesEnableMcps_d    TRUE
#endif

/* Turning this off also turns gZtcMacGetSetPIBCapability_d off. */
#ifndef gSAPMessagesEnableMlme_d
#define gSAPMessagesEnableMlme_d    TRUE
#endif

#ifndef gSAPMessagesEnableAsp_d
#define gSAPMessagesEnableAsp_d     FALSE
#endif

#ifndef gSAPMessagesEnableLlc_d
#define gSAPMessagesEnableLlc_d     FALSE
#endif

#ifndef gSAPMessagesEnableNlde_d
#define gSAPMessagesEnableNlde_d    TRUE
#endif

#ifndef gSAPMessagesEnableNlme_d
#define gSAPMessagesEnableNlme_d    TRUE
#endif

#ifndef gSAPMessagesEnableApsde_d
#define gSAPMessagesEnableApsde_d   TRUE
#endif

#ifndef gSAPMessagesEnableAfde_d
#define gSAPMessagesEnableAfde_d    TRUE
#endif

#ifndef gSAPMessagesEnableApsme_d
#define gSAPMessagesEnableApsme_d   TRUE
#endif

#ifndef gSAPMessagesEnableZdp_d
#define gSAPMessagesEnableZdp_d     TRUE
#endif

/*Enable or Disable Intra Pan pan messages through ZTC
 Note that InterPanCapability must also be enabled */
#ifndef gSAPMessagesEnableInterPan_d
#define gSAPMessagesEnableInterPan_d FALSE
#endif

/*  If gSAPMessagesDisableApsdeEndpoint0Monitoring_d is set to TRUE the apdse data request, 
    data indications and confirms sent and received on endpoint 0x0 is NOT monitored by ZTC.
    This mode is reduces traffic to host process and is ideal for a Black box type application.*/
#ifndef gSAPMessagesDisableApsdeEndpoint0Monitoring_d
#define gSAPMessagesDisableApsdeEndpoint0Monitoring_d FALSE
#endif


#ifndef gSAPMessagesEnableHc_d
#define gSAPMessagesEnableHc_d FALSE
#endif

#if (gMacStandAlone_d ==TRUE)
#endif /* gMacStandAlone_d ==TRUE */


/******************************************************************************
*******************************************************************************
* Public type definitions
*******************************************************************************
******************************************************************************/

/* SAP Handlers are declared to return uint8_t. For Ztc, use a more */
/* abstracted type name. */
typedef uint8_t ztcSAPHandlerStatus_t;

/* Define Ztc's task event word. */
/* The UART ISR has received a byte from the external client. */
#define gDataFromTestClientToZTCEvent_c (1 << 0)

/* The Test Tool uses an Opcode Group to specify which SAP Handler a packet
 * in intended for or received from.
 */
#define gMlmeNwkOpcodeGroup_c       0x84    /* MLME_NWK_SapHandler          */
#define gNwkMlmeOpcodeGroup_c       0x85    /* NWK_MLME_SapHandler          */
#define gMcpsNwkOpcodeGroup_c       0x86    /* MCPS_NWK_SapHandler          */
#define gNwkMcpsOpcodeGroup_c       0x87    /* NWK_MCPS_SapHandler          */
#define gAspAppOpcodeGroup_c        0x94    /* ASP_APP_SapHandler           */
#define gAppAspOpcodeGroup_c        0x95    /* APP_ASP_SapHandler           */
#define gZdoNlmeOpcodeGroup_c       0x96    /* ZDO_NLME_SapHandler          */
#define gNlmeZdoOpcodeGroup_c       0x97    /* NLME_ZDO_SapHandler          */
#define gApsmeZdoOpcodeGroup_c      0x98    /* gZDO_APSME_SAPHandlerId_c    */
#define gZdoApsmeOpcodeGroup_c      0x99    /* ZDO_APSME_SapHandler         */
#define gApsNldeOpcodeGroup_c       0x9A    /* APS_NLDE_SapHandler          */
#define gNldeApsOpcodeGroup_c       0x9B    /* NLDE_APS_SapHandler          */
#define gAfApsdeOpcodeGroup_c       0x9C    /* AF_APSDE_SapHandler          */
#define gApsdeAfOpcodeGroup_c       0x9D    /* APSDE_AF_SapHandler          */
#define gAfdeAppOpcodeGroup_c       0x9E    /* Not defined.                 */
#define gAppAfdeOpcodeGroup_c       0x9F    /* AF_AppToAfdeDataReq          */
#define gZdpAppOpcodeGroup_c        0xA0    /* ZDP_APP_SapHandler           */
#define gAppZdpOpcodeGroup_c        0xA2    /* APP_ZDP_SapHandler           */
#define gZtcReqOpcodeGroup_c        0xA3    /* Ztc utility requests         */
#define gZtcCnfOpcodeGroup_c        0xA4    /* Ztc utility confirmations    */

#define gAppInterPanOpcodeGroup_c   0xA5    /* APP_InterPan_SapHandler      */
#define gInterPanAppOpcodeGroup_c   0xA6    /* InterPan_APP_SapHandler      */

#define gAppLlcOpcodeGroup_c        0xB0    /* LLC utility requests         */
#define gHcAppOpcodeGroup_c         0xB5    /* Hc_App_SapHandler           */
#define gAppHcOpcodeGroup_c         0xB6    /* App_Hc_SapHandler           */

#define gZtcInvalidOpcodeGroup_c    0xFF    /* Reserve an invalid opcode group value.   */
typedef uint8_t ztcOpcodeGroup_t;

/* Define the values that the SAP Handlers use to identify themselves to */
/* ZTC_TaskEventMonitor. Its tempting to use the opcode group numbers */
/* instead of a separate list of values, but the MAC library uses it's own */
/* definitions, which are not changable. */
#define gNwkMCPS_SAPHandlerId_c     0
#define gMCPSNwkSAPHandlerId_c      1
#define gNwkMLME_SAPHandlerId_c     2
#define gMLMENwkSAPHandlerId_c      3
#define gNwkASP_SAPHandlerId_c      4
#define gASPNwkSAPHandlerId_c       5
#define gAPS_NLDE_SAPHandlerId_c    6
#define gNLDE_APS_SAPHandlerId_c    7
#define gZDO_NLME_SAPHandlerId_c    8
#define gNLME_ZDO_SAPHandlerId_c    9
#define gAF_APSDE_SAPHandlerId_c    10
#define gAPSDE_AF_SAPHandlerId_c    11
#define gAppAFDE_SAPHandlerId_c     12
#define gAFDEAppSAPHandlerId_c      13
#define gZDO_APSME_SAPHandlerId_c   14
#define gAPSME_ZDO_SAPHandlerId_c   15
#define gAppZDP_SAPHandlerId_c      16
#define gZDPAppSAPHandlerId_c       17
#define gNwkLLC_SAPHandlerId_c      18
#define gLLCNwk_SAPHandlerId_c      19
#define gInterPanApp_SAPHandlerId_c 20
#define gAppInterPanSAPHandlerId_c  21
#define gHcApp_SAPHandlerId_c       26
#define gAppHc_SAPHandlerId_c       27
typedef uint8_t ztcIntSAPId_t;

/* The Test Tool uses an "opcode" to distinguish the various
 * requests / responses / confirmations /indications recognized by each
 * SAP Handler. BeeStack internally calls this a "message type."
 * Each SAP Handler interprets the message type integers in it's own
 * context. A single message type integer value may be used to mean different
 * things by different SAP Handlers.
 */
typedef uint8_t ztcMsgType_t;

typedef struct ZTCMessage_tag {
  ztcOpcodeGroup_t opCode;
  ztcMsgType_t opCodeId;
  uint8_t length;
  uint8_t data[1];                      /* Place holder. */
} ZTCMessage_t;

/* If Ztc receives a message from a client with a opcode group that
 * Ztc does not recognize, call this function to handle the message.
 * This provides a general function hook mechanism to the rest of BeeStack.
 */
typedef void (*gpfTestClientToApplication_t)(void *pMsg);
extern gpfTestClientToApplication_t gpfTestClientToApplicationFuncPtr;

/* Used to post a byte-available-from-external-client message to Ztc. */
extern tsTaskID_t gZTCTaskID;


/******************************************************************************
*******************************************************************************
* Public memory declarations
*******************************************************************************
******************************************************************************/

/* ZTC's task identifier. */
extern tsTaskID_t gZTCTaskID;

/******************************************************************************
*******************************************************************************
* Public prototypes
*******************************************************************************
******************************************************************************/

extern void Ztc_TaskInit(void);
extern void Ztc_Task(event_t events);
extern void ZTC_TaskEventMonitor(const ztcIntSAPId_t SAPId,
                                 uint8_t *pMsg,
                                 const uint8_t status);
extern void ZTC_FragEventMonitor(const ztcIntSAPId_t SAPId, uint8_t *pMsg);

/* Called by non-Ztc code to send a message to the external client. */
extern void ZTCQueue_QueueToTestClient(uint8_t const *const pData,
                                       uint8_t const opCodeGrp,
                                       uint8_t const msgType,
                                       uint8_t const   dataLength);
                                       
#if gZtcIncluded_d
#define ZTC_RegisterAppInterfaceToTestClient(pfPointer) \
  gpfTestClientToApplicationFuncPtr = (gpfTestClientToApplication_t) pfPointer
#else
#define ZTC_RegisterAppInterfaceToTestClient(pfPointer)
#endif

/* This function set the specified attributte and send a Ztc Set(Apsme or Nlme) Request 
   over the selected Uart */
extern void ZtcMsgSetIBReq(uint8_t msgType, uint8_t attrId, uint8_t index, uint8_t *pValue);

#define ZtcApsmeSetRequest(attrId, index, pValue) ZtcMsgSetIBReq(0x21, attrId, index, pValue)
#define ZtcNlmeSetRequest(attrId, index, pValue)  ZtcMsgSetIBReq(0x23, attrId, index, pValue)



#endif                                  /* #ifndef _ZtcInterface_h */
