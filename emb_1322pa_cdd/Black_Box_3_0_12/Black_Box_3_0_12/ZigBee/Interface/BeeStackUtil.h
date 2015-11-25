/******************************************************************************
* This is the interface header file which provides utility function prototype
* to application for storing restoreing ZigBee tables to NVM.
*
* (c) Copyright 2005, Freescale, Inc. All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
******************************************************************************/

#ifndef _BEESTACK_UTILITY_H_
#define _BEESTACK_UTILITY_H_

/******************************************************************************
*******************************************************************************
* Private macros
*******************************************************************************
******************************************************************************/

#define gCapInfoRcvrOnIdle_c 0x08

/******************************************************************************
*******************************************************************************
* Public macros
*******************************************************************************
******************************************************************************/

extern beeStackConfigParams_t gBeeStackConfig;

#ifndef gHostApp_d
#define BeestackGetRandomNum(Var) CM_GetRandomBackoff(Var)
#endif
/*****************************************************************************
******************************************************************************
* Public prototypes
******************************************************************************
*****************************************************************************/

/* Turn the 802.15.4 receiver on or off. */
extern void BUtl_SetReceiverOn(void);
extern void BUtl_SetReceiverOff(void);

/******************************************************************************
*******************************************************************************
* Public type definitions
*******************************************************************************
******************************************************************************/

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

extern uint16_t CM_GetRandomBackoff
  (
  uint8_t be
  );

/***************************************************************************
* This function is called from Zdo_InitZdoPtc Function.This function
* generates the extended address using the Random function generator,User
* is suggested not to use ths function.
*
* Effects on global data:
*		aExtendedAddress[8] is set to the generated extended address value
*
****************************************************************************/

void BUtl_CreateExtendedAddress
(
  void
);

/****************************************************************************
* This function is used to set the MAC PIB attribute.
*
* Return value: uint8_t : success or failure
*
* Effects on Global Data: MAC PIBN
*
******************************************************************************/

uint8_t SetPibAttributeValue
(
  uint8_t pibAttribute,         /* IN: Attribute Id */
  uint8_t* pibAttributeValue    /* IN: OUT: Pointer where the retrieved value is set */
);

#endif /* _BEESTACK_COMMON_UTILITY_H_ */
