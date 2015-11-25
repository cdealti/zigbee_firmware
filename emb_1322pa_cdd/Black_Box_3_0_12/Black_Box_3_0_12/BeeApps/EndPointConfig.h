/************************************************************************************
* This header file is device End point registration Interface
*
* (c) Copyright 2006, Freescale, Inc.  All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
************************************************************************************/
#ifndef _ENDPOINT_CONFIG_H_
#define _ENDPOINT_CONFIG_H_

#include "BeeStack_Globals.h"
#include "BeeStackConfiguration.h"


/******************************************************************************
*******************************************************************************
* Public Macros
*******************************************************************************
******************************************************************************/
#define GetEndpoint(Number) (endPointList[Number].pEndpointList->pSimpleDesc->endPoint)

#define gNum_EndPoints_c                0

#define gTotalNum_EndPoints_c          (2 + gNum_EndPoints_c)

#define gNum_GenericEndpoints_c         0

#define gNum_HaGenericEndpoints_c       0

#define gNum_HaOnOffLightEndpoints_c    0

#define gNum_HaOnOffSwitchEndpoints_c   0

#define gNum_HaDimmableLightEndpoints_c 0

#define gNum_HaDimmerSwitchEndpoints_c  0

#define gNum_HaTempSensorEndpoints_c    0

#define gNum_HaThermostatEndpoints_c    0

#define gNum_HaConfigurationToolEndpoints_c    0

#define gNum_HaCombinedInterfaceEndpoints_c    0

#define gNum_HaRangeExtenderEndpoints_c       0

#define gNum_WirelessUARTEndpoints_c    0

#define gNum_TP2DutEndpoints_c 0

#define gNum_TP2TestDriverEndpoints_c 0

#define gNum_SeEnergyServicePortalEndpoints_c 0

#define gNum_MirrorEndpoints_c 0

#define gNum_SeInPremiseDisplayEndpoints_c 0

#define gNum_SeLoadControlDeviceEndpoints_c 0

#define gNum_SeMeteringDeviceEndpoints_c 0

#define gNum_SePCTEndpoints_c 0

#define gNum_SePrePaymentTerminalEndpoints_c 0

#define gNum_SeRangeExtenderEndpoints_c 0

#define gNum_SeSmartApplianceEndpoints_c 0



/* Application Device Version and flag values */
#if ( gUser_Desc_rsp_d == 1 )&&( gComplex_Desc_rsp_d == 1 )
/* user and complex descriptor both supported */
#define gAppDeviceVersionAndFlag_c 0x30 
#elif ( gUser_Desc_rsp_d == 1 ) 
/* user descriptor supported */
#define gAppDeviceVersionAndFlag_c 0x20
#elif ( gComplex_Desc_rsp_d == 1 )
/* complex descriptor supported */
#define gAppDeviceVersionAndFlag_c 0x10
#else
/* user and complex descriptor both not supported */
#define gAppDeviceVersionAndFlag_c 0x00
#endif




/* BEGIN Endpoint HA Options Section */
/* END Endpoint HA Options Section */

/******************************************************************************
*******************************************************************************
* Public type definitions
*******************************************************************************
******************************************************************************/
#ifdef __IAR_SYSTEMS_ICC__
#pragma pack(1)
#endif
typedef struct endPointList_tag {
  const endPointDesc_t *pEndpointDesc;  /* pointer to descriptor */
  const afDeviceDef_t *pDevice;  /* pointer to device (or profile specific structure */
} endPointList_t;
#ifdef __IAR_SYSTEMS_ICC__
#pragma pack()
#endif	
/******************************************************************************
*******************************************************************************
* Public Prototypes
*******************************************************************************
******************************************************************************/
void AppMsgCallBack( apsdeToAfMessage_t *pMsg);
void AppCnfCallBack( apsdeToAfMessage_t *pMsg );
void AppBroadcastMsgCallBack(apsdeToAfMessage_t *pMsg);
/******************************************************************************
*******************************************************************************
* Public Memory Declarations
*******************************************************************************
******************************************************************************/
#if gNum_EndPoints_c != 0
extern const endPointList_t endPointList[];
#endif
/*****************************************************************************/
#endif /* _ENDPOINT_CONFIG_H_ */
