/*****************************************************************************
* UART / SCI / USB / Serial Port implementation for ARM7
*
* (c) Copyright 2006, Freescale, Inc. All rights reserved.
*
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
*****************************************************************************/

/* Note: This code is size optimized for the case where only one SCI port is
 * enabled. If both ports are enabled, it might be worthwhile to create
 * functions that take a port number as a parameter.
 */
#include "Platform.h"
#include "Uart_Interface.h"
#include "Uart.h"
#include "UartLowLevel.h"
#include "Interrupt.h"
#include "TS_Interface.h"

#define FN_MASK  3UL
#define FN_ALT  1

void Uart1_TxIsr(UartWriteCallbackArgs_t* args);
void Uart2_TxIsr(UartWriteCallbackArgs_t* args);
void Uart1_RxIsr(UartReadCallbackArgs_t* args);
void Uart2_RxIsr(UartReadCallbackArgs_t* args);

void GpioUart1Init(void);
void GpioUart2Init(void);

#if gBeeStackIncluded_d
#include "BeeStack_Globals.h"
#endif



/*****************************************************************************
******************************************************************************
* Public memory definitions
******************************************************************************
*****************************************************************************/

/* if neither UART enabled, then no code or data */
#if gUart1_Enabled_d || gUart2_Enabled_d

/* id for Uart task */
tsTaskID_t gUartTaskId;

/* Each port can store one unget byte. */
#if gUart1_Enabled_d
static bool_t uart1_ungetFlag = FALSE;
static unsigned char uart1_ungetByte;
bool_t Uart1_TxInProgress = FALSE;
#endif

#if gUart2_Enabled_d
static bool_t uart2_ungetFlag = FALSE;
static unsigned char uart2_ungetByte;
bool_t Uart2_TxInProgress = FALSE;
#endif

/* The transmit code keeps a circular list of buffers to be sent. */
/* Each SCI port has it's own list. */
typedef struct SciTxBufRef_tag {
  unsigned char const *pBuf;
  void (*pfCallBack)(unsigned char const *pBuf);
} SciTxBufRef_t;

/* The leading index is the next position to store a buffer reference. */
/* The trailing index is the buffer currently being transmitted. */
/* Using a separate table for the buffer lengths reduces code size. */
#if gUart1_Enabled_d
static SciTxBufRef_t maSci1TxBufRefTable[gUart_TransmitBuffers_c];
static index_t maSci1TxBufLenTable[gUart_TransmitBuffers_c];
static index_t mSci1TxBufRefLeadingIndex;   /* Post-increment. */
static index_t mSci1TxBufRefTrailingIndex;  /* Post-increment. */

/*Callback table and index pointers*/
static SciTxBufRef_t maSci1TxCallbackTable[gUart_TransmitBuffers_c];
static index_t maSci1TxCallbackLeadingIndex = 0; 
static index_t maSci1TxCallbackTrailingIndex = 0; 
#endif

#if gUart2_Enabled_d
static SciTxBufRef_t maSci2TxBufRefTable[gUart_TransmitBuffers_c];
static index_t maSci2TxBufLenTable[gUart_TransmitBuffers_c];
static index_t mSci2TxBufRefLeadingIndex;   /* Post-increment. */
static index_t mSci2TxBufRefTrailingIndex;  /* Post-increment. */
/*Callback table and index pointers*/
static SciTxBufRef_t maSci2TxCallbackTable[gUart_TransmitBuffers_c];
static index_t maSci2TxCallbackLeadingIndex = 0; 
static index_t maSci2TxCallbackTrailingIndex = 0; 
#endif

/* Each port has a small local circular Rx buffer to store data until the */
/* application can retrieve it. In addition to the leading and trailing */
/* indexes, the Rx code keeps a count of the number of bytes in the buffer. */
#if gUart1_Enabled_d
static unsigned char maSci1RxBuf[gUart_ReceiveBufferSize_c];
static void (*pfSci1RxCallBack)(void);
#endif

#if gUart2_Enabled_d
static unsigned char maSci2RxBuf[gUart_ReceiveBufferSize_c];
static void (*pfSci2RxCallBack)(void);
#endif

/*****************************************************************************
******************************************************************************
* Private memory definitions
******************************************************************************
*****************************************************************************/

/* Keep track of the high water mark in the Rx circular buffer. */
#if gUart_Debug_d
  #if gUart1_Enabled_d
  static index_t uart1RxBufHighWaterMark = 0;
  #endif /* gUart1_Enabled_d */

  #if gUart2_Enabled_d
  static index_t uart2RxBufHighWaterMark = 0;
  #endif /* gUart2_Enabled_d */
#endif /* gUart_Debug_d */

/*****************************************************************************
******************************************************************************
* Public functions
******************************************************************************
*****************************************************************************/

void Uart_ClearErrors(void) {

#if gUart1_Enabled_d
  UartClearErrors(UART_1);
#endif

#if gUart2_Enabled_d
  UartClearErrors(UART_2);
#endif

}                                       /* Uart_ClearErrors() */


/*****************************************************************************/

/* Retrieve a byte from the driver's Rx buffer and store it at *pDst. */
/* Return TRUE if a byte was retrieved; FALSE if the Rx buffer is empty. */
bool_t Uart1_GetByteFromRxBuffer(unsigned char *pDst) {
#if !gUart1_Enabled_d
  (void) pDst;
  return 0;
#else  

  if (uart1_ungetFlag) {
    uart1_ungetFlag = FALSE;
    *pDst = uart1_ungetByte;
    return TRUE;
  } else  
  return UartGetByteFromRxBuffer(UART_1, pDst);
  
#endif
}                                       /* Uart1_GetBytesFromRxBuffer() */

/**************************************/

bool_t Uart2_GetByteFromRxBuffer(unsigned char *pDst) {
#if !gUart2_Enabled_d
  (void) pDst;
  return 0;
#else
  // unsigned char scic2;

  if (uart2_ungetFlag) {
    uart2_ungetFlag = FALSE;
    *pDst = uart2_ungetByte;
    return TRUE;
  } else
  return UartGetByteFromRxBuffer(UART_2, pDst);
  
#endif
}                                       /* Uart2_GetBytesFromRxBuffer() */

/*****************************************************************************/

/* Return TRUE if there is data still not sent. */
bool_t Uart1_IsTxActive(void) {
#if !gUart1_Enabled_d
  return FALSE;
#else
  return ((Uart1_TxInProgress) || (UART1_REGS_P->Utxcon != 32));
#endif
}                                            /* Uart1_IsTxActive() */      

/**************************************/
/* Return TRUE if there is data still not sent. */
bool_t Uart2_IsTxActive(void) {
#if !gUart2_Enabled_d
  return FALSE;
#else
  return ((Uart2_TxInProgress) || (UART2_REGS_P->Utxcon != 32));
#endif
}                                            /* Uart2_IsTxActive() */

/*****************************************************************************/

/* Initialize the serial port(s) and this module's local data. */
void Uart_ModuleInit(void) {
  
  uint8_t res;
  UartConfig_t pConfig;
  UartCallbackFunctions_t pCallback;
  
#if gUart1_Enabled_d

  //initialize GPIOs for UART1 and UART2 
  GpioUart1Init();  

  //configure the uart parameters 
  pConfig.UartParity = gUartParityNone_c;
  pConfig.UartStopBits = gUartStopBits1_c;
  pConfig.UartBaudrate = gUartDefaultBaud_c;
  pConfig.UartFlowControlEnabled = gUart1_EnableHWFlowControl_d;
  pConfig.UartRTSActiveHigh = FALSE;

  //initialize the uart
  res = UartOpen(UART_1,PLATFORM_CLOCK);  
  res |= UartSetConfig(UART_1, &pConfig);  

  UartSetCTSThreshold(UART_1, gUart_RxFlowControlSkew_d);

  //configure the Uart Rx and Tx Threshold
  res |= UartSetTransmitterThreshold(UART_1, 5);
  res |= UartSetReceiverThreshold(UART_1, 5);

  //set pCallback functions
  pCallback.pfUartWriteCallback = Uart1_TxIsr;
  pCallback.pfUartReadCallback = Uart1_RxIsr;

  res |= UartSetCallbackFunctions(UART_1, &pCallback);
  /*set Half Flow Control*/
  #if (gUart1_EnableHalfFlowControl_d == TRUE)
    UartSetHalfFlowControl(UART_1, TRUE);
  #endif

  //mount the interrupts corresponding to UART driver
  IntAssignHandler(gUart1Int_c, (IntHandlerFunc_t)UartIsr1);
  ITC_SetPriority(gUart1Int_c, gItcNormalPriority_c);
  //enable the interrupts corresponding to UART driver
  ITC_EnableInterrupt(gUart1Int_c); 
  
  UartReadData(UART_1, maSci1RxBuf, sizeof(maSci1RxBuf), TRUE);       
#endif

#if gUart2_Enabled_d

  //initialize GPIOs for UART1 and UART2
  GpioUart2Init();  

  //configure the uart parameters
  pConfig.UartParity = gUartParityNone_c;
  pConfig.UartStopBits = gUartStopBits1_c;
  pConfig.UartBaudrate = gUartDefaultBaud_c;
  pConfig.UartFlowControlEnabled = gUart2_EnableHWFlowControl_d;
  pConfig.UartRTSActiveHigh = FALSE;
  
  //initialize the uart
  UartOpen(UART_2,PLATFORM_CLOCK);

  UartSetConfig(UART_2,&pConfig);
  UartSetCTSThreshold(UART_2, gUart_RxFlowControlSkew_d);

  //configure the Uart Rx and Tx Threshold
  UartSetTransmitterThreshold(UART_2,5);
  UartSetReceiverThreshold(UART_2,5);

  //set pCallback functions
  pCallback.pfUartWriteCallback = Uart2_TxIsr;
  pCallback.pfUartReadCallback = Uart2_RxIsr;
  UartSetCallbackFunctions(UART_2,&pCallback);

  /*set Half Flow Control*/
  #if (gUart2_EnableHalfFlowControl_d == TRUE)
    UartSetHalfFlowControl(UART_2, TRUE);
  #endif
    //mount the interrupts corresponding to UART driver
  IntAssignHandler(gUart2Int_c, (IntHandlerFunc_t)UartIsr2);
  ITC_SetPriority(gUart2Int_c, gItcNormalPriority_c);
  //enable the interrupts corresponding to UART driver
  ITC_EnableInterrupt(gUart2Int_c);
  
  UartReadData(UART_2, maSci2RxBuf, sizeof(maSci2RxBuf), TRUE);

#endif /* gUart2_Enabled_d */
  
  Uart_TaskInit();
}                                       /* Uart_ModuleInit() */

/*****************************************************************************/

/* Rx ISR. If there is room, store the byte in the Rx circular buffer. */
#if gUart1_Enabled_d
void Uart1_RxIsr(UartReadCallbackArgs_t* args) {
  
  /* Receive register full and no errors */
  TS_SendEvent(gUartTaskId, gRxSci1Event_c);  
}                                       /* Uart1_RxIsr() */
#endif /** gUart1_Enabled_d */

/**************************************/

#if gUart2_Enabled_d
void Uart2_RxIsr(UartReadCallbackArgs_t* args) {

/* Receive register full and no errors */
  TS_SendEvent(gUartTaskId, gRxSci2Event_c);  
  
}                                       /* Uart2_RxIsr() */
#endif /* gUart2_Enabled_d */

/*****************************************************************************/

/* Set a pointer to the function to be called whenever a byte is received. */
/* If the pointer is set to NULL, turn off the Rx interrupt and clear the Rx */
/* buffer. */
void Uart1_SetRxCallBack(void (*pfCallBack)(void)) {
#if (gUart1_Enabled_d == FALSE)
  (void) pfCallBack;
#else  /* gUart1_Enabled_d */
  pfSci1RxCallBack = pfCallBack;
  UartOpenCloseTransceiver(UART_1, BIT_RX_EN, NULL != pfSci1RxCallBack);  
#endif  /* gUart1_Enabled_d */
}      /* Uart1_SetRxCallback() */

/**************************************/

void Uart2_SetRxCallBack(void (*pfCallBack)(void)) {
#if (gUart2_Enabled_d == FALSE)
  (void) pfCallBack;
#else
  pfSci2RxCallBack = pfCallBack;  
  UartOpenCloseTransceiver(UART_2, BIT_RX_EN, NULL != pfSci2RxCallBack);  
#endif   /* gUart2_Enabled_d */
}                                       /* Uart2_SetRxCallback() */

/*****************************************************************************/

/* Set the UART baud rate. */
void Uart1_SetBaud(UartBaudRate_t baudRate) {  
#if (gUart1_Enabled_d == FALSE)
  (void) baudRate;
#else  
  UartConfig_t Cfg;
  bool_t readStopped = FALSE;
  register uint32_t i;

  UartErr_t err = UartGetStatus(UART_1);  
  if(err != gUartErrUartNotOpen_c)
  {  
   for (i = 0; i < 2; i++)
   {  
      if(err == gUartErrReadOngoing_c) 
      {  
        UartCancelReadData(UART_1);
        readStopped = TRUE;
      }
      else
      {  
       if(err == gUartErrWriteOngoing_c)
       {  
         UartCancelWriteData(UART_1);
       }  
       else
       {
        break;
       }
      }
      /* UartGetStatus is returning one error code at a time. Read back the status to check again */
      err = UartGetStatus(UART_1);
    }
  }  
  
  UartGetConfig(UART_1, &Cfg);
  Cfg.UartBaudrate = baudRate;
  UartSetConfig(UART_1, &Cfg);  
    
  if(TRUE == readStopped)
  {
    UartReadData(UART_1, maSci1RxBuf, sizeof(maSci1RxBuf), TRUE);
  }    
#endif /* gUart1_Enabled_d */
}                                       /* Uart1_SetBaud() */

/**************************************/

void Uart2_SetBaud(UartBaudRate_t baudRate) {
#if (gUart2_Enabled_d == FALSE)
  (void) baudRate;
#else
  UartConfig_t Cfg;
  bool_t readStopped = FALSE;
  register uint32_t i;

  UartErr_t err = UartGetStatus(UART_2);  
  if(err != gUartErrUartNotOpen_c)
  {  
   for (i = 0; i < 2; i++)
   {  
      if(err == gUartErrReadOngoing_c) 
      {  
        UartCancelReadData(UART_2);
        readStopped = TRUE;
      }
      else
      {  
       if(err == gUartErrWriteOngoing_c)
       {  
         UartCancelWriteData(UART_2);
       }  
       else
       {
        break;
       }
      }
      /* UartGetStatus is returning one error code at a time. Read back the status to check again */
      err = UartGetStatus(UART_2);
    }
  }  
  
  UartGetConfig(UART_2, &Cfg);
  Cfg.UartBaudrate = baudRate;
  UartSetConfig(UART_2, &Cfg);  
    
  if(TRUE == readStopped)
  {
    UartReadData(UART_2, maSci2RxBuf, sizeof(maSci2RxBuf), TRUE);
  } 
#endif /* gUart2_Enabled_d */
}                                       /* Uart2_SetBaud() */

/*****************************************************************************/

/* Begin transmitting size bytes of data from *pBuffer. */
/* Returns FALSE if there are no more slots in the buffer reference table. */
bool_t Uart1_Transmit(unsigned char const *pBuf, index_t bufLen, void (*pfCallBack)(unsigned char const *pBuf)) {
#if (gUart1_Enabled_d == FALSE)
  (void) pBuf;
  (void) bufLen;
  (void) pfCallBack;
  return FALSE;
#else
  /* Handle empty buffers. */
  if (!bufLen) {
    (*pfCallBack)(pBuf);
    return TRUE;
  }

  /* Room for one more? */
  if (maSci1TxBufLenTable[mSci1TxBufRefLeadingIndex]) {
    return FALSE;
    
  }

  maSci1TxBufRefTable[mSci1TxBufRefLeadingIndex].pBuf = pBuf;
  maSci1TxBufRefTable[mSci1TxBufRefLeadingIndex].pfCallBack = pfCallBack;
  /* This has to be last, in case the Tx ISR finishes with the previous */
  /* buffer while this function is executing. */
  maSci1TxBufLenTable[mSci1TxBufRefLeadingIndex] = bufLen;

  if (++mSci1TxBufRefLeadingIndex >= NumberOfElements(maSci1TxBufRefTable)) 
  {
      mSci1TxBufRefLeadingIndex = 0;
  }  
  
  if(!Uart1_TxInProgress) 
  {
     /* No buffer is pending for transmission */
    
     /* Enable the transmit interrupt. Harmless if the interrupt is already */
     /* enabled. */
     Uart1_TxInProgress = TRUE;
     UartOpenTransmitter(UART_1);
     UartWriteData(UART_1, (uint8_t*) pBuf, bufLen);     
  } 


  return TRUE;
#endif /* gUart1_Enabled_d */
}                                       /* Uart1_Transmit() */

/**************************************/

bool_t Uart2_Transmit(unsigned char const *pBuf, index_t bufLen, void (*pfCallBack)(unsigned char const *pBuf)) {
#if (gUart2_Enabled_d == FALSE)
  (void) pBuf;
  (void) bufLen;
  (void) pfCallBack;
  return FALSE;
#else
  /* Handle empty buffers. */
  if (!bufLen) {
    (*pfCallBack)(pBuf);
    return TRUE;
  }

  /* Room for one more? */
  if (maSci2TxBufLenTable[mSci2TxBufRefLeadingIndex]) {
    return FALSE;
  }

  maSci2TxBufRefTable[mSci2TxBufRefLeadingIndex].pBuf = pBuf;
  maSci2TxBufRefTable[mSci2TxBufRefLeadingIndex].pfCallBack = pfCallBack;
  /* This has to be last, in case the Tx ISR finishes with the previous */
  /* buffer while this function is executing. */
  maSci2TxBufLenTable[mSci2TxBufRefLeadingIndex] = bufLen;

  if (++mSci2TxBufRefLeadingIndex >= NumberOfElements(maSci2TxBufRefTable)) 
  {
    mSci2TxBufRefLeadingIndex = 0;
  }

  if(!Uart2_TxInProgress)
  {
   /* No buffer is pending for transmission */ 
    
   /* Enable the transmit interrupt. Harmless if the interrupt is already */
   /* enabled. */
   Uart2_TxInProgress = TRUE; 
   UartOpenTransmitter(UART_2);    
   UartWriteData(UART_2, (uint8_t*) pBuf, bufLen);
  }
  
  return TRUE;
#endif  /* gUart2_Enabled_d */
}                                       /* Uart2_Transmit() */

/*****************************************************************************/

/* Transmit ISR. */
#if (gUart1_Enabled_d == TRUE)
void Uart1_TxIsr(UartWriteCallbackArgs_t* args) {
   void (*pfCallBack)(unsigned char const *pBuf);

  /* Finished with the current buffer */

  /* Mark this one as done, and call the callback. */
  maSci1TxBufLenTable[mSci1TxBufRefTrailingIndex] = 0;
  pfCallBack = *maSci1TxBufRefTable[mSci1TxBufRefTrailingIndex].pfCallBack;
    
  if(pfCallBack)
  {
      /*Signal uart task that we got a callback to be executed*/
      TS_SendEvent(gUartTaskId, gTxSci1Event_c);
    
      /*add callback information to callback table*/
      maSci1TxCallbackTable[maSci1TxCallbackLeadingIndex].pfCallBack = pfCallBack;
      maSci1TxCallbackTable[maSci1TxCallbackLeadingIndex].pBuf = maSci1TxBufRefTable[mSci1TxBufRefTrailingIndex].pBuf;
      /*increment and wrap around leading index*/
      if (++maSci1TxCallbackLeadingIndex >= NumberOfElements(maSci1TxBufRefTable))
      {
        maSci1TxCallbackLeadingIndex = 0;
      }    
  }      
  
  /* Increment to the next buffer. */  
  if (++mSci1TxBufRefTrailingIndex >= NumberOfElements(maSci1TxBufRefTable)) 
  {
     mSci1TxBufRefTrailingIndex = 0;
  }
    
  if(maSci1TxBufLenTable[mSci1TxBufRefTrailingIndex] != 0)
  {
     /* Write the next buffer. */       
     UartWriteData(UART_1, (uint8_t*)maSci1TxBufRefTable[mSci1TxBufRefTrailingIndex].pBuf, maSci1TxBufLenTable[mSci1TxBufRefTrailingIndex]);
  }
  else
  {
     Uart1_TxInProgress = FALSE;
  }         
}                                       /* Uart1_TxIsr() */
#endif /* gUart1_Enabled_d */

/**************************************/

#if (gUart2_Enabled_d == TRUE)
void Uart2_TxIsr(UartWriteCallbackArgs_t* args) {
  void (*pfCallBack)(unsigned char const *pBuf);

  /* Finished with the current buffer */

  /* Mark this one as done, and call the callback. */
  maSci2TxBufLenTable[mSci2TxBufRefTrailingIndex] = 0;
  pfCallBack = *maSci2TxBufRefTable[mSci2TxBufRefTrailingIndex].pfCallBack;

  if (pfCallBack)
  {
      /*Signal uart task that we got a callback to be executed*/
      TS_SendEvent(gUartTaskId, gTxSci2Event_c);
      /*add callback information to callback table*/
      maSci2TxCallbackTable[maSci2TxCallbackLeadingIndex].pfCallBack = pfCallBack;
      maSci2TxCallbackTable[maSci2TxCallbackLeadingIndex].pBuf = maSci2TxBufRefTable[mSci2TxBufRefTrailingIndex].pBuf;
      /*increment and wrap around leading index*/  
      if (++maSci2TxCallbackLeadingIndex >= NumberOfElements(maSci2TxBufRefTable))
      {
        maSci2TxCallbackLeadingIndex = 0;
      }    
  }
    
  /* Increment to the next buffer. */

  if (++mSci2TxBufRefTrailingIndex >= NumberOfElements(maSci2TxBufRefTable))
  {
      mSci2TxBufRefTrailingIndex = 0;
  }
    
  if(maSci2TxBufLenTable[mSci2TxBufRefTrailingIndex] != 0)
  {
       /* Write the next buffer. */       
       UartWriteData(UART_2, (uint8_t*)maSci2TxBufRefTable[mSci2TxBufRefTrailingIndex].pBuf, maSci2TxBufLenTable[mSci2TxBufRefTrailingIndex]);
  }
  else
  {
     Uart2_TxInProgress = FALSE;
  }  
}                                       /* Uart2_TxIsr() */
#endif /* gUart2_Enabled_d */

/*****************************************************************************/

/* Shut down the serial port(s). */
void Uart_ModuleUninit(void) {
#if gUart1_Enabled_d
  UartClose(UART_1);
#endif /* gUart1_Enabled_d */

#if gUart2_Enabled_d
  UartClose(UART_2);
#endif /* gUart2_Enabled_d */
}                                       /* Uart_ModuleUninit() */

/*****************************************************************************/

/* Unget a byte: store the byte so that it will be returned by the next call */
/* to the get byte function. Also call the Rx call back. */

void Uart1_UngetByte(unsigned char byte) {
#if (gUart1_Enabled_d == FALSE)
  (void) byte;
#else
  /* If the un get flag is already true, there isn't anything we can */
  /* do about it. Not much point in checking. Its up to the called to */
  /* use this function correctly. */
  uart1_ungetFlag = TRUE;
  uart1_ungetByte = byte;

  /* Likewise, it is the caller's responsibility to not call this function */
  /* if the call back pointer is not valid. */
  if(pfSci1RxCallBack != NULL)
  {  
   pfSci1RxCallBack();
  } 

#endif /* gUart1_Enabled_d */
}                                       /* Uart1_UngetByte() */

void Uart2_UngetByte(unsigned char byte) {
#if (gUart2_Enabled_d == FALSE)
  (void) byte;
#else
  uart2_ungetFlag = TRUE;
  uart2_ungetByte = byte;
  if(pfSci2RxCallBack != NULL)
  {  
   pfSci2RxCallBack();
  } 
#endif  /* gUart2_Enabled_d */
}                                       /* Uart2_UngetByte() */




uint32_t Uart1_RxBufferByteCount() {
  return UartGetUnreadBytesNumber(UART_1);
  
}

uint32_t Uart2_RxBufferByteCount() {
  return UartGetUnreadBytesNumber(UART_2);
  
}


void Uart_Task(event_t events)
{
  void (*pfCallBack)(unsigned char const *pBuf);
  

#if gUart1_Enabled_d

  if( events & gRxSci1Event_c ) {
    if(pfSci1RxCallBack != NULL)
    {
     pfSci1RxCallBack();
    } 
  }

  if( events & gTxSci1Event_c ) {   
  
    /*Tx call back event received - run through the callback table and execute any pending
    Callbacks.
    */
    while (maSci1TxCallbackLeadingIndex != maSci1TxCallbackTrailingIndex) {    
      pfCallBack = maSci1TxCallbackTable[maSci1TxCallbackTrailingIndex].pfCallBack;

      if(pfCallBack != NULL)
      {  
       /*Call calback with buffer info as parameter*/
       (*pfCallBack)(maSci1TxCallbackTable[maSci1TxCallbackTrailingIndex].pBuf);
      } 
      
      /*increment and wrap around trailing index*/
      if (++maSci1TxCallbackTrailingIndex >= NumberOfElements(maSci1TxCallbackTable)) {
        maSci1TxCallbackTrailingIndex = 0;    
      }
    } /* while */
  } /*if events ...*/


#endif /* gUart1_Enabled_d */
  
  
#if gUart2_Enabled_d

  if( events & gRxSci2Event_c ) {
    if(pfSci2RxCallBack != NULL)
    {  
     pfSci2RxCallBack();
    } 
  } 

  if( events & gTxSci2Event_c ) {
    /*Tx call back event received - run through the callback table and execute any pending
    Callbacks.
    */
    while (maSci2TxCallbackLeadingIndex != maSci2TxCallbackTrailingIndex) {    
      pfCallBack = maSci2TxCallbackTable[maSci2TxCallbackTrailingIndex].pfCallBack;
      if(pfCallBack != NULL)
      {  
       /*Call calback with buffer info as parameter*/
       (*pfCallBack)(maSci2TxCallbackTable[maSci2TxCallbackTrailingIndex].pBuf);
      } 
      /*increment and wrap around trailing index*/     
      if (++maSci2TxCallbackTrailingIndex >= NumberOfElements(maSci2TxCallbackTable)) {
        maSci2TxCallbackTrailingIndex = 0;    
      }
    } /* while */
  } /*if events ...*/

#endif /* gUart2_Enabled_d */
  
}

#endif /* gUart1_Enabled_d || gUart2_Enabled_d */


/*
  Initialize the uart task.
*/
void Uart_TaskInit(void)
{
#if gUart1_Enabled_d || gUart2_Enabled_d
  gUartTaskId = TS_CreateTask(gTsUartTaskPriority_c, Uart_Task);
#endif /* gUart1_Enabled_d || gUart2_Enabled_d */
}

/*
  Uart1 GPIO .Initilization
*/
void GpioUart1Init(void)
{
  register uint32_t tmpReg;

  GPIO.PuSelLo |= (GPIO_UART1_RTS_bit | GPIO_UART1_RX_bit);  // Pull-up select: UP type
  GPIO.PuEnLo  |= (GPIO_UART1_RTS_bit | GPIO_UART1_RX_bit);  // Pull-up enable
        
  #if (gUart1_EnableHalfFlowControl_d == FALSE)
  tmpReg = GPIO.FuncSel0 & ~((FN_MASK << GPIO_UART1_RX_fnpos) | (FN_MASK << GPIO_UART1_TX_fnpos));
  GPIO.FuncSel0 = tmpReg | ((FN_ALT << GPIO_UART1_RX_fnpos) | (FN_ALT << GPIO_UART1_TX_fnpos));
  tmpReg = GPIO.FuncSel1 & ~((FN_MASK << GPIO_UART1_CTS_fnpos) | (FN_MASK << GPIO_UART1_RTS_fnpos));
  GPIO.FuncSel1 = tmpReg | ((FN_ALT << GPIO_UART1_CTS_fnpos) | (FN_ALT << GPIO_UART1_RTS_fnpos));
  #else
    tmpReg = GPIO.FuncSel0 & ~((FN_MASK << GPIO_UART1_RX_fnpos) | (FN_MASK << GPIO_UART1_TX_fnpos));
    GPIO.FuncSel0 = tmpReg | ((FN_ALT << GPIO_UART1_RX_fnpos) | (FN_ALT << GPIO_UART1_TX_fnpos));
    tmpReg = GPIO.FuncSel1 & ~((FN_MASK << GPIO_UART1_CTS_fnpos) | (FN_MASK << GPIO_UART1_RTS_fnpos));
    GPIO.FuncSel1 = tmpReg | (FN_ALT << GPIO_UART1_RTS_fnpos);
    GPIO.DataResetLo = GPIO_UART1_CTS_bit;  // low
    GPIO.DirSetLo = GPIO_UART1_CTS_bit;  // output
  #endif
}


/*
  Uart1 GPIO .Initilization
*/
void GpioUart2Init(void)
{
  register uint32_t tmpReg;

  GPIO.PuSelLo |= (GPIO_UART2_RTS_bit | GPIO_UART2_RX_bit);  // Pull-up select: UP type
  GPIO.PuEnLo  |= (GPIO_UART2_RTS_bit | GPIO_UART2_RX_bit);  // Pull-up enable

  #if (gUart2_EnableHalfFlowControl_d == FALSE)
  tmpReg = GPIO.FuncSel1 & ~((FN_MASK << GPIO_UART2_CTS_fnpos) | (FN_MASK << GPIO_UART2_RTS_fnpos)\
  | (FN_MASK << GPIO_UART2_RX_fnpos) | (FN_MASK << GPIO_UART2_TX_fnpos));
  GPIO.FuncSel1 = tmpReg | ((FN_ALT << GPIO_UART2_CTS_fnpos) | (FN_ALT << GPIO_UART2_RTS_fnpos)\
  | (FN_ALT << GPIO_UART2_RX_fnpos) | (FN_ALT << GPIO_UART2_TX_fnpos));
  #else
    tmpReg = GPIO.FuncSel1 & ~((FN_MASK << GPIO_UART2_CTS_fnpos) | (FN_MASK << GPIO_UART2_RTS_fnpos)\
      | (FN_MASK << GPIO_UART2_RX_fnpos) | (FN_MASK << GPIO_UART2_TX_fnpos));
    GPIO.FuncSel1 = tmpReg | ((FN_ALT << GPIO_UART2_RTS_fnpos) | (FN_ALT << GPIO_UART2_RX_fnpos)\
      | (FN_ALT << GPIO_UART2_TX_fnpos));
    GPIO.DataResetLo = GPIO_UART2_CTS_bit;  // low
    GPIO.DirSetLo = GPIO_UART2_CTS_bit; // output
  #endif
}
