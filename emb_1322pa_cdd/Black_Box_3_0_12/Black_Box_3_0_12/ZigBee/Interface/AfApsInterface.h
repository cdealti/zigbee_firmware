/******************************************************************************
* Interface definition for AF and APS
*
* Copyright (c) 2008, Freescale, Inc.  All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
******************************************************************************/

#ifndef _AF_APS_INTERFACE_H
#define _AF_APS_INTERFACE_H

/******************************************************************************
*******************************************************************************
* Public macros
*******************************************************************************
******************************************************************************/

/* Address modes used by zbApsdeDataReq_t ( Table 2.2 r09 )*/
enum {
	/* Indirect data request, Device Address and Endpoint fields are ignored.*/
  gApsAddrNotPresent_c				= 0x00,		  
  /*  16-bit group address for DstAddress and DstEndpoint not present */
  gApsGroupAddrPresent_c				= 0x01,
  /* = Direct data request with extended address of destination Device */
  gApsShortAndEPAddrPresent_c		= 0x02,
  /* = Direct data request with extended address of destination Device */
  gApsExtAndEPAddrPresent_c		= 0x03
};


/*-----------------------------------------------------------------------------
-------------------------------------------------------------------------------
   Tx Option used by zbApsdeDataReq_t
-------------------------------------------------------------------------------
-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
-------------------------------------------------------------------------------
   AF to APS Data Primitive
-------------------------------------------------------------------------------
-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
-------------------------------------------------------------------------------
  APS to AF Data Primitive 
-------------------------------------------------------------------------------
-----------------------------------------------------------------------------*/

/******************************************************************************
*******************************************************************************
* Public prototypes
*******************************************************************************
******************************************************************************/



/******************************************************************************
*******************************************************************************
* Public type definitions
*******************************************************************************
******************************************************************************/

typedef uint8_t apsAddrMode_t;
typedef uint8_t apsTxOption_t;

/* message types in the AF_APSDE_SapHandler, downward direction */
typedef uint8_t primAfToApsde_t;
#define gApsdeDataReqMsgType_d      0
#define gApsdeDataReqFragMsgType_d  1 /* pseudo message: framented blocks sent to SAP */

#ifdef __IAR_SYSTEMS_ICC__
#pragma pack(1)
#endif
/* AF to APSDE message */
typedef struct afApsdeMessage_tag {
  primAfToApsde_t msgType;
  struct {
    zbApsdeDataReq_t  dataReq;
  } msgData;
} afToApsdeMessage_t;


/* message types in the APSDE_AF_SapHandler, upward direction */
typedef uint8_t primApsdeToAf_t;
#define gApsdeDataCnfMsgType_d         0
#define gApsdeDataIndMsgType_d         1 /* fragmented and normal indications come through here */
#define gApsdeDataIndFragBlkMsgType_d  2 /* pseudo message: fragmented blocks sent to ZTC */
#define gApsdeConfirmIdMsgType_d       3

typedef struct apsdeToAfMessage_tag
{
  primApsdeToAf_t msgType;
  union
  {
    zbApsdeDataConfirm_t    dataConfirm;
    zbApsdeDataIndication_t dataIndication;
    zbApsConfirmId_t        confirmId;
  } msgData;
} apsdeToAfMessage_t;
#ifdef __IAR_SYSTEMS_ICC__
#pragma pack()
#endif
/* MSG Data Indication Callback Function Pointer */
typedef void ( *dataMsgCallback_t )( apsdeToAfMessage_t * );

/*Data Confirmation Callback Function Pointer*/
typedef void ( *dataConfCallback_t )( apsdeToAfMessage_t * );


/******************************************************************************
*******************************************************************************
* Public memory declarations
*******************************************************************************
******************************************************************************/


/******************************************************************************
*******************************************************************************
* Public functions
*******************************************************************************
******************************************************************************/

/* data communication from the application to the ZigBee Stack (and back) */
uint8_t AF_APSDE_SapHandler( afToApsdeMessage_t *pMsg );
uint8_t APSDE_AF_SapHandler( apsdeToAfMessage_t *pMsg);

#endif /*_AF_APS_INTERFACE_H */
