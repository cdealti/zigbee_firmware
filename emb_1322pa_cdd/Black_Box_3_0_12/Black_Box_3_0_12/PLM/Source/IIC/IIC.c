/************************************************************************************
* This file contains the implementation of the BlackBox communication over I2C
*
* (c) Copyright 2008, Freescale, Inc.  All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
************************************************************************************/
#include "EmbeddedTypes.h"
#include "FunctionLib.h"
#include "MsgSystem.h"
#include "TS_Interface.h"
#include "PublicConst.h"

#include "IIC_Interface.h"
#include "IIC.h"
#include "Platform.h"
#include "Interrupt.h"
#include "GPIO_Interface.h"

/******************************************************************************
*******************************************************************************
* Private macros
*******************************************************************************
******************************************************************************/

#define MODULE_ENABLE         I2C_CTRL_MEN
#define INTERRUPT_ENABLE      I2C_CTRL_MIEN
#define BROADCAST_ENABLE      I2C_CTRL_BCST
#define CLOCK_ENABLE          I2C_CKER_CKEN
#define TRANSFER_COMPLETE     I2C_STAT_MCF
#define ADDRESSED_AS_SLAVE    I2C_STAT_MAAS
#define BUS_BUSY              I2C_STAT_MBB
#define ARBITRATION_LOST      I2C_STAT_MAL
#define SLAVE_TRANSMIT        I2C_STAT_SRW
#define MODULE_INT_FLAG       I2C_STAT_MIF
#define RX_ACK                I2C_STAT_RXAK

/* I2C registers bit mask definitions*/

/* Masks for ADR register*/
#define I2C_ADR_MASK    0xFE
#define I2C_ADR_ADDR    0xFE
#define I2C_ADR_ADDR7   0x80
#define I2C_ADR_ADDR6   0x40
#define I2C_ADR_ADDR5   0x20
#define I2C_ADR_ADDR4   0x10
#define I2C_ADR_ADDR3   0x08
#define I2C_ADR_ADDR2   0x04
#define I2C_ADR_ADDR1   0x02

/* Masks for FDR register*/
#define I2C_FDIV_MASK   0x3F
#define I2C_FDIV_FDIV   0x3F
#define I2C_FDIV_FDR5   0x20
#define I2C_FDIV_FDR4   0x10
#define I2C_FDIV_FDR3   0x08
#define I2C_FDIV_FDR2   0x04
#define I2C_FDIV_FDR1   0x02
#define I2C_FDIV_FDR0   0x01

/* Masks for CTRL register*/
#define I2C_CTRL_MASK   0xFD
#define I2C_CTRL_MEN    0x80
#define I2C_CTRL_MIEN   0x40
#define I2C_CTRL_MSTA   0x20
#define I2C_CTRL_MTX    0x10
#define I2C_CTRL_TXAK   0x08
#define I2C_CTRL_RSTA   0x04
#define I2C_CTRL_BCST   0x01

/* Masks for STAT register */
#define I2C_STAT_MASK   0xFF
#define I2C_STAT_MCF    0x80
#define I2C_STAT_MAAS   0x40
#define I2C_STAT_MBB    0x20
#define I2C_STAT_MAL    0x10
#define I2C_STAT_BCSTM  0x08
#define I2C_STAT_SRW    0x04
#define I2C_STAT_MIF    0x02
#define I2C_STAT_RXAK   0x01

/* Masks for DATA register*/
#define I2C_DATA_MASK   0xFF
#define I2C_DATA_D7     0x80
#define I2C_DATA_D6     0x40
#define I2C_DATA_D5     0x20
#define I2C_DATA_D4     0x10
#define I2C_DATA_D3     0x08
#define I2C_DATA_D2     0x04
#define I2C_DATA_D1     0x02
#define I2C_DATA_D0     0x01
#define I2C_DATA_D      0xFF

/* Masks for DFSRR register*/
#define I2C_DFILT_MASK  0x3F
#define I2C_DFILT_N5    0x20
#define I2C_DFILT_N4    0x10
#define I2C_DFILT_N3    0x08
#define I2C_DFILT_N2    0x04
#define I2C_DFILT_N1    0x02
#define I2C_DFILT_N0    0x01
#define I2C_DFILT_N     0x3F

/* Masks for CKE register*/
#define I2C_CKER_MASK   0x01
#define I2C_CKER_CKEN   0x01

#define I2cBusBusy()              (I2C.Status & BUS_BUSY)
#define I2cSendStart()            I2C.Control |= I2C_CTRL_MSTA
#define I2cSendStop()             I2C.Control &= ~I2C_CTRL_MSTA
#define I2cSendRepStart()         I2C.Control |= I2C_CTRL_RSTA
#define I2cArbitrationLost()      (I2C.Status & ARBITRATION_LOST)
#define I2cALClear()              I2C.Status &= ~ARBITRATION_LOST
#define I2cMIFClear()             I2C.Status &= ~MODULE_INT_FLAG
#define I2cWaitForMCF()           while(!(I2C.Status & TRANSFER_COMPLETE)){}
#define I2cWaitForMIF()           while(!(I2C.Status & MODULE_INT_FLAG)){}
#define I2cSendAck(ackStatus)     (ackStatus == gI2cAckResponse_c) ? (I2C.Control &= ~I2C_CTRL_TXAK) : (I2C.Control |= I2C_CTRL_TXAK)
#define I2cReceivedAck()           (!(I2C.Status & RX_ACK))
#define I2cSendByte(byteData)     I2C.Data = (byteData)
#define I2cReceiveByte(byteData)  *(byteData) = I2C.Data



/******************************************************************************
*******************************************************************************
* Private type definitions
*******************************************************************************
******************************************************************************/


/******************************************************************************
*******************************************************************************
* Private Prototypes
******************************************************************************/
#if gIIC_Enabled_d
static bool_t IIC_InitQueue(IIC_CQueue_t *pQueue);
static bool_t IIC_PushData(IIC_CQueue_t *pQueue, unsigned char data);
static bool_t IIC_PopData(IIC_CQueue_t *pQueue, unsigned char *pData);
#endif
/******************************************************************************
*******************************************************************************
* Private Memory Declarations
*******************************************************************************
******************************************************************************/
#if gIIC_Enabled_d

I2cModuleEx_t mI2cModuleEx;

static uint8_t internalRxBuffer[gIIC_QueueSz_c];

/*****************************************************************************
*   IIC_InitQueue
*
 * Description: Initialize the IIC RX queue
 * Params: [IN] pQueue - pointer to queue
 * Return: TRUE if the pointer is valid, FALSE otherwise
******************************************************************************/

static bool_t IIC_InitQueue(IIC_CQueue_t *pQueue)
{
  if(NULL == pQueue)
  {
      return FALSE;
  }

  pQueue->Head = 0;
  pQueue->Tail = 0;
  pQueue->EntriesCount = 0;

  return TRUE;
}


 /*****************************************************************************
*   IIC_PushData
*
* Description: Add a new element to the RX queue
* Params: [IN] pQueue - pointer to queue
*         [IN] data - data to be added  
* Return: TRUE if the push operation succeeded, FALSE otherwise
******************************************************************************/
static bool_t IIC_PushData(IIC_CQueue_t *pQueue, unsigned char data)
{   
    if(NULL == pQueue || NULL == pQueue->pData)
    {
        return FALSE;
    }
    
    if((pQueue->Tail == pQueue->Head) && (pQueue->EntriesCount > 0))
    {
	/* increment the head (read index) */
	pQueue->Head = (pQueue->Head + 1) & ((unsigned char) (gIIC_QueueSz_c - 1));
    }

    /* Add the item to queue */
    pQueue->pData[pQueue->Tail] = data;

    /* Reset the tail when it reach gIIC_QueueSz_c */
    pQueue->Tail = (pQueue->Tail + 1) % ((unsigned char) (gIIC_QueueSz_c));

    /* Increment the entries count */
     if(pQueue->EntriesCount < (unsigned char) (gIIC_QueueSz_c)) pQueue->EntriesCount++;

    return TRUE;
}
 
 /*****************************************************************************
*   IIC_PopData
*
* Description: Retrieves the head element from the RX queue
* Params: [IN] pQueue - pointer to queue
*         [OUT] pData - pointer to the location where data shall be placed
* Return: TRUE if the pop operation succeeded, FALSE otherwise
******************************************************************************/
 
static bool_t IIC_PopData(IIC_CQueue_t *pQueue, unsigned char *pData)
{    
    if( (NULL == pQueue) || (NULL == pQueue->pData)  || (pQueue->EntriesCount <= 0))
    {
        return FALSE;
    }
    
    *pData = pQueue->pData[pQueue->Head];

    /* Reset the head when it reach gIIC_QueueSz_c */
    pQueue->Head = (pQueue->Head + 1) % ((unsigned char) (gIIC_QueueSz_c ));

    /* Decrement the entries count */
    pQueue->EntriesCount--;

    return TRUE;
}

#endif
/******************************************************************************
*******************************************************************************
* Public memory declarations
*******************************************************************************
******************************************************************************/

/******************************************************************************
*******************************************************************************
* Public Functions
*******************************************************************************
******************************************************************************/

/*****************************************************************************
*   IIC_ModuleInit
*
*   Initialize the IIC module
******************************************************************************/
void IIC_ModuleInit()
{  
#if gIIC_Enabled_d     
  /* Configure the pin to be used for signaling the master host that data for read is available.
     Default configuration is 'No Data for Read', meaning GPIO is set to high */ 
  Gpio_EnPinPullup(gTxDataAvailablePin_c, TRUE);
  Gpio_SetPinDir(gTxDataAvailablePin_c, gGpioDirOut_c);
  Gpio_SetPinData(gTxDataAvailablePin_c, gGpioPinStateHigh_c);
       
  mI2cModuleEx.Rxi2cCallback = NULL;
  mI2cModuleEx.Txi2cCallback = NULL;
  
  mI2cModuleEx.i2cStatus = I2C_DEFAULT_STATUS;
  mI2cModuleEx.sendBuff = 0;
  mI2cModuleEx.sentBytesNo = 0;
  mI2cModuleEx.sendBuffLength = 0;
  mI2cModuleEx.recvBuffLength = 0;

  I2C.ClockEn = CLOCK_ENABLE;
  I2C.Address = I2C.Address & ~I2C_ADR_MASK;
  I2C.FreqDiv = I2C.FreqDiv & ~I2C_FDIV_MASK;
  I2C.Control = I2C.Control & ~I2C_CTRL_MASK;
  I2C.Status = 0x00;
  I2C.Data = 0x00;
  I2C.DigitalFilter = I2C.DigitalFilter & ~I2C_DFILT_MASK;
  I2C.ClockEn = I2C.ClockEn & ~I2C_CKER_MASK;
  

  Gpio_SetPinFunction(gGpioPin12_c, gGpioAlternate1Mode_c);
  Gpio_SetPinFunction(gGpioPin13_c, gGpioAlternate1Mode_c);
  
  /* Register our own interrupt handler for the I2C peripheral */  
  IntAssignHandler(gI2cInt_c, (IntHandlerFunc_t)&IIC_IsrEx);
  ITC_SetPriority(gI2cInt_c, gItcFastPriority_c);
  ITC_EnableInterrupt(gI2cInt_c);
  IntEnableFIQ();
  /* Enable I2C module */  
  IIC_EnableEx();
  
  /* Configure I2C hardware peripheral */ 
  IIC_SetSlaveAddress(gI2CDefaultSlaveAddress_c);
  
  /* Prepare the Rx module of the I2C to receive a maximum length packet */    
  IIC_InitQueue(&mI2cModuleEx.recvQueue);  
  IIC_ReceiveDataEx(0x00, &internalRxBuffer[0], gIIC_QueueSz_c - 1, gI2cSlvTransfer_c);   
#endif
}


/*****************************************************************************
*   IIC_SetSlaveAddress
*
*   Set the slave address
******************************************************************************/
void IIC_SetSlaveAddress(uint8_t address)
{ 
#if !gIIC_Enabled_d  
  (void) address;
#else  
  I2cConfig_t i2cConfig;
  
  i2cConfig.i2cInterruptEn = TRUE;
  i2cConfig.i2cBroadcastEn = FALSE;  
  i2cConfig.slaveAddress = address;
  i2cConfig.freqDivider = I2cGetDFSRVal();
  i2cConfig.saplingRate = I2cGetDFSRVal();
  IIC_SetConfigEx(&i2cConfig);
#endif  
}


/*****************************************************************************
*   I2c_EnableEx 
*
*   Enable hardware peripheral
******************************************************************************/
I2cErr_t IIC_EnableEx(void)
{
#if !gIIC_Enabled_d  
  return gI2cErrModuleIsDis_c;
#else  
  if (mI2cModuleEx.i2cStatus & I2C_ON)
    return gI2cErrModuleIsEn_c;

  I2C.ClockEn = CLOCK_ENABLE; /* enable module's clock */
  I2C.Control |= MODULE_ENABLE; /* enable I2C module */
  mI2cModuleEx.i2cStatus &= ~I2C_MODULE_BUSY; /* I2C module is  idle */
  mI2cModuleEx.i2cStatus |= I2C_ON; /* I2C module is ON */
  return gI2cErrNoError_c;
#endif    
}

/*****************************************************************************
*   I2c_SetConfigEx 
*
*   Set hardware register with a given configuration
******************************************************************************/
I2cErr_t IIC_SetConfigEx(I2cConfig_t* pI2cConfig)
{
#if !gIIC_Enabled_d  
  (void) pI2cConfig;
  return gI2cErrModuleIsDis_c;
#else  
  if (!(mI2cModuleEx.i2cStatus & I2C_ON))
    return gI2cErrModuleIsDis_c;
  if (NULL == pI2cConfig)
    return gI2cErrNullPointer_c;

  I2C.Address = (I2C.Address & ~I2C_ADR_MASK) | (pI2cConfig->slaveAddress & I2C_ADR_MASK);
  I2C.FreqDiv = (I2C.FreqDiv & ~I2C_FDIV_MASK) | (pI2cConfig->freqDivider & I2C_FDIV_MASK);
  I2C.DigitalFilter = (I2C.DigitalFilter & ~I2C_DFILT_MASK) | (pI2cConfig->saplingRate & I2C_DFILT_MASK);

  if (FALSE == pI2cConfig->i2cInterruptEn)
  {
    I2C.Control &= ~INTERRUPT_ENABLE;
  }
  else
  {
    I2C.Control |= INTERRUPT_ENABLE;
  }
  
  if (FALSE == pI2cConfig->i2cBroadcastEn)
  {
    I2C.Control &= ~BROADCAST_ENABLE;
  }
  else
  {
    I2C.Control |= BROADCAST_ENABLE;
  }
  return gI2cErrNoError_c;
#endif
}

/*****************************************************************************
*   I2c_ReceiveDataEx 
*
*   Set the buffer where to receive a specific length of bytes depending on transfer
*   mode that was selected (slave or master)
******************************************************************************/
I2cErr_t IIC_ReceiveDataEx(uint8_t slaveDestAddr, uint8_t* i2cBuffData, uint16_t dataLength, I2cTransferMode_t transferMode)
{
#if !gIIC_Enabled_d  
  (void) slaveDestAddr;
  (void) dataLength;
  (void) transferMode;  
  return gI2cErrModuleIsDis_c;
#else    
  I2cErr_t i2cErr = gI2cErrNoError_c;

  if (!(mI2cModuleEx.i2cStatus & I2C_ON))
    return gI2cErrModuleIsDis_c;
  if (NULL == i2cBuffData)
    return gI2cErrNullPointer_c;  
  if ((!(I2C.Control & INTERRUPT_ENABLE)) && (transferMode == gI2cSlvTransfer_c))
    return gI2cErrInvalidOp_c;

  mI2cModuleEx.recvQueue.pData  = i2cBuffData;
  mI2cModuleEx.recvBuffLength   = dataLength;
  mI2cModuleEx.i2cStatus        |= I2C_SLV_RECEIVE_REQ;
  mI2cModuleEx.i2cStatus        |= I2C_MODULE_BUSY; /* I2C module is busy */

  return i2cErr;
#endif  
}

/*****************************************************************************
*   I2c_SendDataEx 
*
*   Set the buffer from where to transmit a specific length of bytes depending on transfer
*   mode that was selected (slave or master)
******************************************************************************/
I2cErr_t IIC_SendDataEx(uint8_t slaveDestAddr, uint8_t* i2cBuffData, uint16_t dataLength, I2cTransferMode_t transferMode)
{
#if !gIIC_Enabled_d  
  (void) slaveDestAddr;
  (void) i2cBuffData;
  (void) dataLength;
  (void) transferMode;  
  return gI2cErrModuleIsDis_c;
#else      
  I2cErr_t i2cErr = gI2cErrNoError_c;
  
  if (!(mI2cModuleEx.i2cStatus & I2C_ON))
    return gI2cErrModuleIsDis_c;
  if (NULL == i2cBuffData)
    return gI2cErrNullPointer_c;
  if ((!(I2C.Control & INTERRUPT_ENABLE)) && (transferMode == gI2cSlvTransfer_c))
    return gI2cErrInvalidOp_c;

  mI2cModuleEx.sendBuff       = i2cBuffData;
  mI2cModuleEx.sendBuffLength = dataLength;
  mI2cModuleEx.sentBytesNo    = 0;
  mI2cModuleEx.i2cStatus      |= I2C_SLV_TRANSMIT_REQ;
  mI2cModuleEx.i2cStatus      |= I2C_MODULE_BUSY; /* I2C module is busy */

  return i2cErr;
#endif  
}

/*****************************************************************************
*   IIC_SetRxCallBack
*
*   Register a callback in order to inform an upper layer that a rx/tx operation has been complete.
******************************************************************************/
void IIC_SetRxCallBack(I2cCallbackFunction_t pfCallBack)
{
#if !gIIC_Enabled_d  
  (void) pfCallBack;
#else      
  mI2cModuleEx.Rxi2cCallback = pfCallBack;
  mI2cModuleEx.Txi2cCallback = NULL;
#endif  
}

/*****************************************************************************
*   I2cDummyReadEx 
*
*   Perform a dummy read on i2c data register in order to generate clock on SLK line.
******************************************************************************/
void IIC_DummyReadEx(void)
{
#if gIIC_Enabled_d    
  register uint8_t dummy;
  
  I2C.Control &= ~I2C_CTRL_MTX; /* clear I2CCR[MTX] = set receive mode */
  dummy = I2C.Data; /* dummy read */
  (void)dummy;
#endif  
}

/*****************************************************************************
*   I2cEndSlaveTransmitEx 
*
*   End a slave transmission .
******************************************************************************/
void IIC_EndSlaveTransmitEx(void)
{
#if gIIC_Enabled_d    
  mI2cModuleEx.i2cStatus &= ~I2C_MODULE_BUSY; /* I2C slave transmit operation is complete */
  if (NULL != mI2cModuleEx.Txi2cCallback)
  {
    mI2cModuleEx.Txi2cCallback(mI2cModuleEx.sentBytesNo, mI2cModuleEx.i2cStatus); /* call the callback function */
    mI2cModuleEx.i2cStatus &= ~I2C_SLV_TRANSMIT_REQ;
    mI2cModuleEx.i2cStatus &= ~I2C_ADDR_AS_SLAVE;
  }
#endif  
}

/*****************************************************************************
*   I2cSlaveAddrPhaseEx 
*
*   Check to see if the slave is addressed for read or write by master
******************************************************************************/
void IIC_SlaveAddrPhaseEx(void)
{
#if gIIC_Enabled_d    
  I2C.Control |= I2C_CTRL_TXAK; /* implicit send NACK for next bytes */
  mI2cModuleEx.i2cStatus |= I2C_MODULE_BUSY; /* I2C module is busy */
  mI2cModuleEx.i2cStatus |= I2C_ADDR_AS_SLAVE;
  if (I2C.Status & SLAVE_TRANSMIT){ /* slave TRANSMIT (master reading from slave) */
    I2C.Control |= I2C_CTRL_MTX; /* set IRCCR[MTX] */
    mI2cModuleEx.i2cStatus |= I2C_TRANSMIT; /* I2C module transmit status */
    I2C.Data = *(mI2cModuleEx.sendBuff + mI2cModuleEx.sentBytesNo); /* WRITE data */
    if(mI2cModuleEx.sendBuffLength){
      mI2cModuleEx.sentBytesNo++;
      mI2cModuleEx.sendBuffLength--;
    }
  } /* end slave transmit */
  else{ /* slave RECEIVE (master writing to slave) */
    if (1 < mI2cModuleEx.recvBuffLength){
      I2C.Control &= ~I2C_CTRL_TXAK;  /* if more than 1 byte to receive set ACK response */
    }
    mI2cModuleEx.i2cStatus &= ~I2C_TRANSMIT; /* I2C module receive status */
    IIC_DummyReadEx();
  } /* end slave receive */
#endif  
}

/*****************************************************************************
*   I2c_IsrEx 
*
*   I2C interrupt routine.
******************************************************************************/
void IIC_IsrEx(void)
{
#if gIIC_Enabled_d   
  register uint8_t i2cData;

  I2C.Status &= ~MODULE_INT_FLAG; /* clear I2CSR[MIF] */
  if (I2C.Control & I2C_CTRL_MSTA) /* MASTER mode */
  {
  }
  else /* SLAVE mode */
  {
    if (I2C.Status & ARBITRATION_LOST) /* ARBITRATION lost */
    { 
      mI2cModuleEx.i2cStatus |= I2C_ARB_LOST; /* I2C arbitration LOST status */
      mI2cModuleEx.i2cStatus &= ~I2C_MODULE_BUSY; /* I2C operation complete status */
      if ((NULL != mI2cModuleEx.Rxi2cCallback) && ((NULL != mI2cModuleEx.Txi2cCallback)))
      {
        if (mI2cModuleEx.i2cStatus & I2C_TRANSMIT)
        {
          mI2cModuleEx.Rxi2cCallback(mI2cModuleEx.sentBytesNo, mI2cModuleEx.i2cStatus); /* call the callback function */
        }
        else
        {
          mI2cModuleEx.Txi2cCallback(mI2cModuleEx.recvQueue.EntriesCount, mI2cModuleEx.i2cStatus); /* call the callback function */
        }
      }
      I2C.Status &= ~ARBITRATION_LOST; /* clear I2CSR[MAL] */
      mI2cModuleEx.i2cStatus &= ~I2C_ARB_LOST; /* clear I2C arbitration lost status */
      if (I2C.Status & ADDRESSED_AS_SLAVE) /* addressed as slave */
      { 
        IIC_SlaveAddrPhaseEx();
      } /* end addresed as slave */
    } /* end arbitration lost */
    else /* ARBITRATION is not lost */
    { 
      if (I2C.Status & ADDRESSED_AS_SLAVE) /* addressed as slave */
      { 
        IIC_SlaveAddrPhaseEx();
      } /* end addressed as slave */
      else /* not addressed as SLAVE (slave data cycle) */
      { 
        if (I2C.Control & I2C_CTRL_MTX) /* (slave) TRANSMIT mode */
        { 
          if (I2C.Status & RX_ACK) /* ACK not received */
          { 
            IIC_DummyReadEx(); /* release the bus */
            IIC_EndSlaveTransmitEx();
          } /* end ack not received */
          else /* ACK received */
          { 
            I2C.Data = *(mI2cModuleEx.sendBuff + mI2cModuleEx.sentBytesNo); /* WRITE next byte to I2CDR */
            if (mI2cModuleEx.sendBuffLength)
            {
              mI2cModuleEx.sentBytesNo++;
              mI2cModuleEx.sendBuffLength--;
            }
            else
            {
              IIC_DummyReadEx(); /* release the bus */
              IIC_EndSlaveTransmitEx();
              if (mI2cModuleEx.i2cStatus & I2C_BREAK_TRANSF)
              {
                mI2cModuleEx.i2cStatus &= ~I2C_BREAK_TRANSF;
              }
              mI2cModuleEx.sentBytesNo = 0;
            }
          } /* end ack received */
        } /* end slave transmit mode */
        else /* (slave) RECEIVE mode */
        {           
          if(mI2cModuleEx.recvQueue.EntriesCount < mI2cModuleEx.recvBuffLength)
          {
            /* READ I2CDR */
            i2cData = I2C.Data;
            /* store data to internal circular buffer */
            IIC_PushData(&mI2cModuleEx.recvQueue, i2cData);
            /* call the callback function */
            mI2cModuleEx.Rxi2cCallback(mI2cModuleEx.recvQueue.EntriesCount, mI2cModuleEx.i2cStatus);
          }       
        } /* end slave receive mode */
      } /* end not addressed as slave */
    } /* end  arbitration is not lost */
  }
#endif  
}



/*****************************************************************************
*   I2C_GetByteFromRxBuffer 
*
*   Get a byte from internal rx buffer if any
******************************************************************************/
bool_t IIC_GetByteFromRxBuffer(uint8_t* byte)
{
#if gIIC_Enabled_d      
    *byte = 0x00;
    return IIC_PopData(&mI2cModuleEx.recvQueue, byte);
#else
  return FALSE;  
#endif  
}

/*****************************************************************************
*   I2C_IsTxActive 
*
*   Inform upper layer that there is bytes to be sent
******************************************************************************/
bool_t IIC_IsTxActive(void)
{
#if gIIC_Enabled_d
  return ((mI2cModuleEx.sendBuffLength > 0)||I2cBusBusy());
#else
  return FALSE;
#endif  
}

/*****************************************************************************
*   I2C_RxBufferByteCount 
*
*   
******************************************************************************/
uint32_t IIC_RxBufferByteCount(void)
{
#if gIIC_Enabled_d      
  return mI2cModuleEx.recvQueue.EntriesCount;
#else  
 return 0;
#endif    
 
}

/*****************************************************************************
*   I2C_ResetRx 
*
*   Reset internal RX variabiles
******************************************************************************/
void IIC_ResetRx(void)
{
#if gIIC_Enabled_d        
  IIC_InitQueue(&mI2cModuleEx.recvQueue);
#endif  
}

/*****************************************************************************
*   I2C_SendBBoxMsg 
*
*   Sends a built blackBox message over the serial communication interface
******************************************************************************/
void IIC_SendBBoxMsg(bool_t bWaitForTxComplete)
{
  (void)bWaitForTxComplete;
#if gIIC_Enabled_d        
  /* Signal to host that we have data to transmit */
  Gpio_SetPinData(gTxDataAvailablePin_c, gGpioPinStateLow_c);
#endif  
}


void ICC_ZtcTransmitOneByte(uint8_t* pByte)
{
#if !gIIC_Enabled_d      
  (void) pByte;
#else  
  /* Reset internal TX vars */
  mI2cModuleEx.sendBuff = pByte;
  mI2cModuleEx.sendBuffLength = 1;
  mI2cModuleEx.sentBytesNo = 0;

  /* Signal to the master that there is data to be sent. */
  Gpio_SetPinData(gTxDataAvailablePin_c, gGpioPinStateLow_c);
  while (IIC_IsTxActive());
#endif  
}


/*****************************************************************************
* I2C_ZtcTransmitBuffer
*
* Send an arbitrary buffer to the external client
******************************************************************************/
bool_t IIC_Transmit(uint8_t * pSrc, uint16_t length, void (*pfCallBack)(unsigned char const *pBuf))
{
#if !gIIC_Enabled_d      
  (void) pSrc;
  (void) length;
  (void) pfCallBack;
  
  return TRUE;
#else  
  /* Stay here until the message is placed in the I2C queue. Call I2C Tx with no callback */
  mI2cModuleEx.sendBuff = (uint8_t*)pSrc;
  mI2cModuleEx.sentBytesNo = 0;
  mI2cModuleEx.sendBuffLength = length;
  /* Wait for the completion of the tx operation */
  Gpio_SetPinData(gTxDataAvailablePin_c, gGpioPinStateLow_c);
  while (IIC_IsTxActive());  
  
  /* Signal to the master that there is no data to be sent. */
  Gpio_SetPinData(gTxDataAvailablePin_c, gGpioPinStateHigh_c);

  while (I2cBusBusy());

  /* Execute callback manually */
  if (pfCallBack) 
    pfCallBack(NULL);
  
  return TRUE;
#endif  
}


/******************************************************************************
*******************************************************************************
* Private Functions
*******************************************************************************
******************************************************************************/
