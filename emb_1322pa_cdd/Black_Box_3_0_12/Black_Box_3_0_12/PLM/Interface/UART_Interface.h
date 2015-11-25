/*****************************************************************************
* UART / SCI / USB / Serial Port driver declarations.
* 
* This driver supports both SCI1 and SCI2 on the ARM7, each of which can be
* enabled or disabled independantly. On the EVB, SRB and NCB boards, the USB
* (SCI2) port is used. 
*
* Copyright (c) 2007, Freescale, Inc. All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
*****************************************************************************/

/* On the receive side, this module keeps a small circular buffer, and calls
 * a callback function whenever a byte is received. The application can
 * retrieve bytes from the buffer at it's convenience, as long as it does so
 * before the driver's buffer fills up.
 *
 * On the transmit side, this module keeps a list of buffers to be
 * transmitted, and calls an application callback function when the entire
 * buffer has been sent. The application is responsible for ensuring that the
 * buffer is available until the callback function is called.
 *
 * If both SCI ports are enabled, each has it's own Rx circular buffer and
 * list of Tx buffers.
 */

#ifndef _Uart_Interface_h_
#define _Uart_Interface_h_

#include "Platform.h"
#include "EmbeddedTypes.h"
#include "TS_Interface.h"

/*****************************************************************************
******************************************************************************
* Public macros
******************************************************************************
*****************************************************************************/

/* Default setting for debugging code (includes high-water mark checking). */
#ifndef gUart_Debug_d
#define gUart_Debug_d       FALSE
#endif

/*****************************************************************************/

/* The ARM has two SCI/UART hardware interfaces. Define the one(s) in use. */
  
#ifndef gUart1_Enabled_d
  #define gUart1_Enabled_d    TRUE
#endif /* gUart1_Enabled_d */

#ifndef gUart2_Enabled_d
  #define gUart2_Enabled_d    FALSE
#endif /* gUart2_Enabled_d */


/* events for uart task */
#define gRxSci1Event_c  (1<<0)
#define gRxSci2Event_c  (1<<1)
#define gTxSci1Event_c  (1<<2)
#define gTxSci2Event_c  (1<<3)


/* Which port is used by default? */
/* If a default port is not defined */
#ifndef gUart_PortDefault_d
  #if (gUart2_Enabled_d == TRUE)
    #define gUart_PortDefault_d     2
  #else
    #define gUart_PortDefault_d     1
  #endif /* gUart2_Enabled_d */
#endif /* gUart_PortDefault_d */

/*****************************************************************************/

/* Use hardware flow control? */

#ifndef gUart1_EnableHWFlowControl_d
  #define gUart1_EnableHWFlowControl_d     FALSE
#endif /* gUart1_EnableHWFlowControl_d */

#ifndef gUart1_EnableHalfFlowControl_d
  #define gUart1_EnableHalfFlowControl_d    TRUE
#endif /* gUart1_EnableHalfFlowControl_d */

#ifndef gUart2_EnableHalfFlowControl_d
  #define gUart2_EnableHalfFlowControl_d    FALSE
#endif /* gUart2_EnableHalfFlowControl_d */

#ifndef gUart2_EnableHWFlowControl_d
  #define gUart2_EnableHWFlowControl_d     FALSE
#endif /* gUart2_EnableHWFlowControl_d */

/*****************************************************************************/

/* Tuning definitions. */

/* Number of entries in the transmit-buffers-in-waiting list. */
#ifndef gUart_TransmitBuffers_c
  #define gUart_TransmitBuffers_c     3
#endif /* gUart_TransmitBuffers_c */

/* Size of the driver's Rx circular buffer. These buffers are used to */
/* hold received bytes until the application can retrieve them via the */
/* UartX_GetBytesFromRxBuffer() functions, and are not otherwise accessable */
/* from outside the driver. The size does not need to be a power of two. */
#ifndef gUart_ReceiveBufferSize_c
  #define gUart_ReceiveBufferSize_c   32
#endif /* gUart_ReceiveBufferSize_c */

/* If flow control is used, there is a delay before telling the far side */
/* to stop and the far side actually stopping. When there are SKEW bytes */
/* remaining in the driver's Rx buffer, tell the far side to stop */
/* transmitting. */
#ifndef gUart_RxFlowControlSkew_d
#define gUart_RxFlowControlSkew_d   8
#endif

/* Number of bytes left in the Rx buffer when hardware flow control is */
/* deasserted. */
#ifndef gUart_RxFlowControlResume_d
#define gUart_RxFlowControlResume_d 8
#endif

/*
#if gUart_RxFlowControlResume_d >= (gUart_ReceiveBufferSize_c - gUart_RxFlowControlSkew_d)
#error Deassert flow control before it is asserted?
#endif
*/

/*****************************************************************************/

typedef uint32_t UartBaudRate_t;

#define gUARTBaudRate1200_c     ((UartBaudRate_t) 1200)
#define gUARTBaudRate2400_c     ((UartBaudRate_t) 2400)
#define gUARTBaudRate4800_c     ((UartBaudRate_t) 4800)
#define gUARTBaudRate9600_c     ((UartBaudRate_t) 9600)
#define gUARTBaudRate19200_c    ((UartBaudRate_t) 19200)
#define gUARTBaudRate38400_c    ((UartBaudRate_t) 38400)
#define gUARTBaudRate57600_c    ((UartBaudRate_t) 57600)
#define gUARTBaudRate115200_c   ((UartBaudRate_t) 115200)   /* Might not work for all clients */
#define gUARTBaudRateCustom_c   ((UartBaudRate_t) 230400)

#define Baudrate_1200     gUARTBaudRate1200_c
#define Baudrate_2400     gUARTBaudRate2400_c
#define Baudrate_4800     gUARTBaudRate4800_c
#define Baudrate_9600     gUARTBaudRate9600_c
#define Baudrate_19200    gUARTBaudRate19200_c
#define Baudrate_38400    gUARTBaudRate38400_c
#define Baudrate_57600    gUARTBaudRate57600_c
#define Baudrate_115200   gUARTBaudRate115200_c
#define Baudrate_Custom   gUARTBaudRateCustom_c


/* Default baud rate. */
#ifndef gUartDefaultBaud_c
#define gUartDefaultBaud_c Baudrate_38400
#endif /* gUartDefaultBaud_c */


/*****************************************************************************
******************************************************************************
* Public prototypes
******************************************************************************
*****************************************************************************/

#if (gUart1_Enabled_d == FALSE) && (gUart2_Enabled_d == FALSE)

/* stub functions if disabled */
#define Uart_ModuleInit()
#define Uart_ModuleUninit()
#define Uart_ClearErrors()
#define Uart1_SetBaud(baudRate)
#define Uart2_SetBaud(baudRate)
#define Uart1_Transmit(pBuf, bufLen, pfCallBack) FALSE
#define Uart2_Transmit(pBuf, bufLen, pfCallBack) FALSE
#define Uart1_IsTxActive() FALSE
#define Uart2_IsTxActive() FALSE
#define Uart1_SetRxCallBack(pfCallBack)
#define Uart2_SetRxCallBack(pfCallBack)
#define Uart1_GetByteFromRxBuffer(pDst) FALSE
#define Uart2_GetByteFromRxBuffer(pDst) FALSE
#define Uart1_UngetByte(byte)
#define Uart2_UngetByte(byte)
#define Uart1_RxBufferByteCount FALSE
#define Uart2_RxBufferByteCount FALSE

#else /* (gUart1_Enabled_d == FALSE) && (gUart2_Enabled_d == FALSE) */

#if (gUart1_Enabled_d == TRUE)
extern bool_t Uart1_TxCompleteFlag;
extern index_t mSci1RxBufferByteCount;      /* # of bytes in buffer. */
#endif

#if (gUart2_Enabled_d == TRUE)
extern bool_t Uart2_TxCompleteFlag;
extern index_t mSci2RxBufferByteCount;      /* # of bytes in buffer. */
#endif

/* Initialize the UART module, including whichever ports are enabled. */
extern void Uart_ModuleInit(void);

/* Shut down the UART module. */
extern void Uart_ModuleUninit(void);

/* Clear any error flags seen by the UART driver. In rare conditions, an */
/* error flag can be set without triggering an error interrupt, and will */
/* prevent Rx and/or Tx interrupts from occuring. The most likely cause */
/* is a breakpoint set during debugging, when a UART port is active. */
/* Calling this function occasionaly allows the UART code to recover */
/* from these errors. */
extern void Uart_ClearErrors(void);

/* Set the baud rate. */
extern void Uart1_SetBaud(UartBaudRate_t baudRate);
extern void Uart2_SetBaud(UartBaudRate_t baudRate);

/* Transmit bufLen bytes of data from pBuffer over a port. Call *pfCallBack() */
/* when the entire buffer has been sent. Returns FALSE if there are no */
/* more available Tx buffer slots, TRUE otherwise. The caller must ensure */
/* that the buffer remains available until the call back function is called. */
/* pfCallBack must not be NULL. */
/* The callback function will be called in interrupt context, so it should */
/* be kept very short. */
extern bool_t Uart1_Transmit(unsigned char const *pBuf, index_t bufLen, void (*pfCallBack)(unsigned char const *pBuf));
extern bool_t Uart2_Transmit(unsigned char const *pBuf, index_t bufLen, void (*pfCallBack)(unsigned char const *pBuf));

/* Return TRUE if there are any bytes in transmit buffers that have not */
/* yet been sent. Return FALSE otherwise. */
extern bool_t Uart1_IsTxActive(void);
extern bool_t Uart2_IsTxActive(void);

/* Set the receive side callback function. This function will be called */
/* whenever a byte is received by the driver. The callback's bytesWaiting */
/* parameter is the number of bytes available in the driver's Rx buffer. */
/* The callback function will be called in interrupt context, so it should */
/* be kept very short. */
/* If the callback pointer is set to NULL, the Rx interrupt will be turned */
/* off, and all data in the driver's Rx buffer will be discarded. */
extern void Uart1_SetRxCallBack(void (*pfCallBack)(void));
extern void Uart2_SetRxCallBack(void (*pfCallBack)(void));

/* Retrieve one byte from the driver's Rx buffer and store it at *pDst. */
/* Return TRUE if a byte was retrieved; FALSE if the Rx buffer is empty. */
extern bool_t Uart1_GetByteFromRxBuffer(unsigned char *pDst);
extern bool_t Uart2_GetByteFromRxBuffer(unsigned char *pDst);

/* If a caller reads one too many characters, it can unget it. Calling */
/* the unget routine will store the byte to be read by the next call to */
/* UartX_GetByteFromRxBuffer(), and will also call the Rx call back. */
extern void Uart1_UngetByte(unsigned char byte);
extern void Uart2_UngetByte(unsigned char byte);


/* Get the number of unread bytes from the UART queue */
extern uint32_t Uart1_RxBufferByteCount(void);
extern uint32_t Uart2_RxBufferByteCount(void);

/* Declare the default API. If gUart_PortDefault_d is not defined, there is */
/* no default API. */
#if gUart_PortDefault_d == 1
  #define UartX_Transmit              Uart1_Transmit
  #define UartX_TransmitAndBlock      Uart1_TransmitAndBlock
  #define UartX_IsTxActive            Uart1_IsTxActive
  #define UartX_SetRxCallBack         Uart1_SetRxCallBack
  #define UartX_GetRxBytesWaiting     Uart1_GetRxBytesWaiting
  #define UartX_GetByteFromRxBuffer   Uart1_GetByteFromRxBuffer
  #define UartX_SetBaud               Uart1_SetBaud
  #define UartX_UngetByte             Uart1_UngetByte
  #define UartX_TxCompleteFlag        Uart1_TxCompleteFlag
  #define UartX_RxBufferByteCount     Uart1_RxBufferByteCount
#endif

#if gUart_PortDefault_d == 2
  #define UartX_Transmit              Uart2_Transmit
  #define UartX_TransmitAndBlock      Uart2_TransmitAndBlock
  #define UartX_IsTxActive            Uart2_IsTxActive
  #define UartX_SetRxCallBack         Uart2_SetRxCallBack
  #define UartX_GetRxBytesWaiting     Uart2_GetRxBytesWaiting
  #define UartX_GetByteFromRxBuffer   Uart2_GetByteFromRxBuffer
  #define UartX_SetBaud               Uart2_SetBaud
  #define UartX_UngetByte             Uart2_UngetByte
  #define UartX_TxCompleteFlag        Uart2_TxCompleteFlag
  #define UartX_RxBufferByteCount     Uart2_RxBufferByteCount
#endif

#endif /* (gUart1_Enabled_d == FALSE) && (gUart2_Enabled_d == FALSE) */

  void Uart_Task(event_t events);
  void Uart_TaskInit(void);

#endif /* _Uart_Interface_h_ */
