/*****************************************************************************
* IIC Serial Port driver declarations.
* 
* This driver supports the IIC on the HCS08.
*
* Copyright (c) 2008, Freescale, Inc. All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
*****************************************************************************/

#ifndef _IIC_Interface_h_
#define _IIC_Interface_h_

#include "EmbeddedTypes.h"
#include "AppToPlatformConfig.h"
#include "I2C_Interface.h"
#include "TS_Interface.h"

/*****************************************************************************
******************************************************************************
* Public macros
******************************************************************************
*****************************************************************************/

/* Enable or disable the IIC module */
#ifndef gIIC_Enabled_d
#define gIIC_Enabled_d    FALSE
#endif

/* Events for IIC task */
#define gRxIIcEvent_c   (1<<2)
#define gTxIIcEvent_c   (1<<3)

/* Number of entries in the transmit-buffers-in-waiting list */
#ifndef gIIC_TransmitBuffers_c
#define gIIC_TransmitBuffers_c      3
#endif

/* Size of the driver's Rx circular buffer. This buffers is used to
    hold received bytes until the application can retrieve them via the
    IIcX_GetBytesFromRxBuffer() functions, and are not otherwise accessable
    from outside the driver. The size does not need to be a power of two. */
#ifndef gIIC_ReceiveBufferSize_c
#define gIIC_ReceiveBufferSize_c    32
#endif

#ifndef gIIC_TransmitBuferSize_c
#define gIIC_TransmitBufferSize_c   32
#endif

/* Given MCU bus speed of 16MHz and IIC bit rate of 400kbps.
    Master IIC Frequency Divider is 0b00001011  =  0x0B */
#define gIIC_FrequencyDivider       0x0B

#ifndef gI2CDefaultSlaveAddress_c
#define gI2CDefaultSlaveAddress_c   0xEC
#endif


/*
  The GPIO pin used to signal the master tha there's data available
*/
#define gTxDataAvailablePin_c   gGpioPin1_c

/*****************************************************************************
******************************************************************************
* Public memory declarations
******************************************************************************
*****************************************************************************/
/* Number of bytes Rx in buffer */
extern index_t mIIcRxBufferByteCount;


/*****************************************************************************
******************************************************************************
* Public prototypes
******************************************************************************
*****************************************************************************/


/* Initialize the IIC module */
extern void     IIC_ModuleInit(void);
/* Set the slave address */
extern void       IIC_SetSlaveAddress(uint8_t address);
/* Shut down the IIC module */
extern void     IIC_ModuleUninit(void);
/* Set the receive side callback function */
extern void     IIC_SetRxCallBack(I2cCallbackFunction_t pfCallBack);
/* Retrieve one byte from the driver's Rx buffer and store it at *pDst.
    Return TRUE if a byte was retrieved; FALSE if the Rx buffer is empty */
extern bool_t   IIC_GetByteFromRxBuffer(unsigned char *pDst);
/* Transmit bufLen bytes of data from pBuffer over a port. Call *pfCallBack() */
/* when the entire buffer has been sent. Returns FALSE if there are no */
/* more available Tx buffer slots, TRUE otherwise. The caller must ensure */
/* that the buffer remains available until the call back function is called. */
/* pfCallBack must not be NULL. */
/* The callback function will be called in interrupt context, so it should */
/* be kept very short. */
extern bool_t IIC_Transmit(uint8_t * pSrc, uint16_t length, void (*pfCallBack)(unsigned char const *pBuf));
/* Checks if Tx process is still running */
extern bool_t   IIC_IsTxActive(void); 
/* The call of this function will trigger the dropping of data arrived over I2C interface 
    until the node is addressed again */
extern void     IIC_QuitCurrentRxSession (void);
/* IIC module main task */
extern void     IIC_Task(event_t events);
/* Depending on bool parameter master will be signaled that need to read data from slave */
extern void IIC_TxDataAvailable(bool_t bIsAvailable);

extern uint32_t IIC_RxBufferByteCount(void);


#endif    /* _IIC_Interface_h_ */
