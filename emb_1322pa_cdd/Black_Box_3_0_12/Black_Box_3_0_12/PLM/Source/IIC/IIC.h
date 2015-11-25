/************************************************************************************
* This header file contains the interface for the BlackBox communication over I2C
*
* (c) Copyright 2008, Freescale, Inc.  All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
************************************************************************************/
#ifndef _I2C_H_
#define _I2C_H_

#include "I2C_Interface.h"

/******************************************************************************
*******************************************************************************
* Public Macros
*******************************************************************************
******************************************************************************/

#define gIIC_QueueSz_c  128

/******************************************************************************
*******************************************************************************
* Public type definitions
*******************************************************************************
******************************************************************************/

/* Circular queue data type definition */
typedef struct tag_CQueue
{
	uint8_t*	pData; /* pointer to queue data */
	index_t	Head;	/* read index */
	index_t	Tail;	/* write index */
	unsigned int	EntriesCount;
} IIC_CQueue_t;

/* This data type describes the internal variables used for one I2C driver instance */
typedef struct I2cModuleEx_tag{
  /* I2C status */ 
  volatile uint16_t i2cStatus;
  
  /* variable which contains in its member the callbacks functions for the I2C driver */
  I2cCallbackFunction_t Rxi2cCallback;
  I2cCallbackFunction_t Txi2cCallback;

  /* Send buffer */
  uint8_t* sendBuff;

  /* Receive queue */
  IIC_CQueue_t recvQueue;
  
  /* Send buffer length */
  volatile uint16_t sendBuffLength;
  
  /* Receive buffer length */
  volatile uint16_t recvBuffLength;

  /* Sent bytes number */
  uint16_t sentBytesNo;
  
}I2cModuleEx_t;

/******************************************************************************
*******************************************************************************
* Public Memory Declarations
*******************************************************************************
******************************************************************************/

extern I2cModuleEx_t mI2cModuleEx;

/******************************************************************************
*******************************************************************************
* Public Prototypes
*******************************************************************************
******************************************************************************/
extern void       IIC_IsrEx(void);
extern I2cErr_t   IIC_EnableEx(void);
extern void       IIC_GpioInit(void);
extern I2cErr_t   IIC_SetConfigEx(I2cConfig_t* pI2cConfig);
extern I2cErr_t   IIC_ReceiveDataEx(uint8_t slaveDestAddr, uint8_t* i2cBuffData, uint16_t dataLength, I2cTransferMode_t transferMode);
extern I2cErr_t   IIC_SendDataEx(uint8_t slaveDestAddr, uint8_t* i2cBuffData, uint16_t dataLength, I2cTransferMode_t transferMode);
extern I2cErr_t   IIC_SetCallbackFunctionEx(I2cCallbackFunction_t pRxI2cCallback, I2cCallbackFunction_t pTxI2cCallback);
extern void       IICSlaveAddrPhaseEx(void);
extern void       IICDummyReadEx(void);
extern void       IIC_RxCallBack(uint16_t transfBytesNo, uint16_t status);
extern void       IIC_TxCallBack(uint16_t transfBytesNo, uint16_t status);
extern void       IIC_SendBBoxMsg(bool_t bWaitForTxComplete);
extern bool_t     IIC_GetByteFromRxBuffer(uint8_t* byte);
extern bool_t     IIC_IsTxActive(void);
extern uint32_t   IIC_RxBufferByteCount(void);
extern void       IIC_ResetRx(void);

/**********************************************************************************/
#endif /* _I2C_H_ */

