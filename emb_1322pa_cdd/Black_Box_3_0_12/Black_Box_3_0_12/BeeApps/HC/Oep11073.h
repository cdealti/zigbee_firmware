/************************************************************************************
* Oep11073.h
* IEEE 11073 Optimized Exchange Protocol header file
*
* (c) Copyright 2010, Freescale, Inc.  All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
************************************************************************************/
#ifndef _OEP11073_H_
#define _OEP11073_H_

#include "BeeStack_Globals.h"
#include "EmbeddedTypes.h"
#include "zigbee.h"
#include "zcl.h"
#include "HcProfile.h"
#include "OepClassSupport.h"


/******************************************************************************
*******************************************************************************
* Public Macros
*******************************************************************************
******************************************************************************/

/* Maximum number of supported devices */
#define gOepMaxDevices_c    1



/* Task events */
#define  evtNoEvent_c       0
#define  evtApduReceived_c  1<<1     /* Apdu received from the lower layer */  
#define  evtAssocReq_c      1<<2     /* Association request */

#define gOepFragOptions_WindowSize        1       /* window can be 1 - 8 */
#define gOepFragOptions_InterFrameDelay   50      /* interframe delay can be 1 - 255ms */
#define gOepFragOptions_MaxFragmentLength 64


/******************************************************************************
*******************************************************************************
* Public type definitions
*******************************************************************************
******************************************************************************/

typedef enum {
  oepDeviceType_Manager,
  oepDeviceType_Agent
} OepDeviceType_t;

typedef enum 
{
  /* Common to Agent and Manager */
  stateDisconnected_c,
  stateConnDissasociating_c,
  stateConnUnassociated_c,
  stateConnAssociating_c,
  stateConnAssocOperating_c,
  
  /* Agent specific */  
  stateConnAssocConfWa_c,
  stateConnAssocConfSc_c,
  
  /* Manager specific */  
  stateConnAssocConfWc_c,
  stateConnAssocConfCc_c
  
} OepAgentSmState_t;


typedef enum {
  oepStartAssocOrigin_Ztc,
  oepStartAssocOrigin_EndDeviceBind
} OepStartAssocOrigin_t;


typedef enum Oep11073CmdType_t
{

  OepCmdAssociateReq_c,
  OepCmdAssociateCnf_c
    
} Oep11073CmdType_t;


typedef void (*pfnOepInit_t)(void);



#ifdef __IAR_SYSTEMS_ICC__
#pragma pack(1)
#endif

typedef struct Oep11073Cmd_Tag 
{
  Oep11073CmdType_t cmdType;
  /*
  struct cmd
  {
  
  }
  */  
} Oep11073Cmd_t;


typedef struct Oep11073AddressInfo_Tag 
{
  zbAddrMode_t    dstAddrMode;     /* destination addressing mode */
  zbIeeeAddr_t    aDstAddr;        /* destination address  */
  zbEndPoint_t    dstEndPoint;     /* destination endpoint */
  zbEndPoint_t    srcEndPoint;     /* source endpoint      */ 
} Oep11073AddressInfo_t;

struct OepApduFragment_tag
{
  uint8_t len;                     /* length of Apdu fragment */
  struct OepApduFragment_tag *nextFragment; /* pointer to next fragment */
  uint8_t data[1];                 /* First byte of Apdu payload */  
};
typedef struct OepApduFragment_tag OepApduFragment_t;

typedef struct OepFragmentedApdu_tag 
{
  zbNwkAddr_t     aSrcAddr;        /* source address */
	zbEndPoint_t    srcEndPoint;     /* source endpoint */  	
  zbEndPoint_t    dstEndPoint;     /* destination endpoint */  	    
  OepApduFragment_t fragment;		   /* apdu fragment */
}
OepFragmentedApdu_t;

typedef struct OepOutgoingMessage_tag
{
  bool_t ztcOnly;
  zbAddrMode_t    dstAddrMode;     /* addressing mode */
	zbEndPoint_t    srcEndPoint;     /* source endpoint */  	
  zbIeeeAddr_t    aDstAddr;        /* destination address */
  zbEndPoint_t    dstEndPoint;     /* destination endpoint */  	    
  uint16_t        length;          /* apdu length */
  uint8_t*        pApdu;		       /* apdu fragment */
}
OepOutgoingMessage_t;

typedef struct OepEndpointData_tag 
{      
  zbEndPoint_t ep;
  void (*handler)(OepFragmentedApdu_t *pApdu);
}
OepEndpointData_t;


#ifdef __IAR_SYSTEMS_ICC__
#pragma pack()
#endif



/******************************************************************************
*******************************************************************************
* Public Memory Declarations
*******************************************************************************
******************************************************************************/

/******************************************************************************
*******************************************************************************
* Public Prototypes
*******************************************************************************
******************************************************************************/

void Oep11073InitManagerDevice(zbEndPoint_t ep, genericClass_t* (*pObjectArray)[]);
void Oep11073InitAgentDevice(zbEndPoint_t ep, genericClass_t* (*pObjectArray)[]);

void ZCL_Oep_SAPHandler(OepFragmentedApdu_t *FragmentedpApdu);
void Oep_ZCL_SAPHandler(OepOutgoingMessage_t *pMsg);
void OepFreeFragmentedApdu(OepFragmentedApdu_t *pApdu);
OepFragmentedApdu_t *OepGetStartOepFragment(zbEndPoint_t ep, 
                                            bool_t knownConfiguration,
                                            OepStartAssocOrigin_t origin
                                           );
OepFragmentedApdu_t *OepGetSendDataReportFragment(zbEndPoint_t ep, uint16_t invokeId, 
                                                  uint16_t dataReqId, uint16_t scanReportNo
                                                  );
OepFragmentedApdu_t *OepGetReleaseOepFragment(zbEndPoint_t ep);
OepFragmentedApdu_t *OepGetTransportConnectedFragment(zbEndPoint_t ep);
OepFragmentedApdu_t *OepGetTransportDisconnectedFragment(zbEndPoint_t ep);


/**********************************************************************************/
#endif /* _OEP11073_H_ */
