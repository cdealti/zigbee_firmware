/******************************************************************************
* Declarations for ZTC routines that handle the ZTC <--> external client protocol.
*
* Copyright (c) 2011, Freescale, Inc.  All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from  Freescale Semiconductor.
*
* Declarations relating to the communication path between Ztc and a external
* (UART based) client, such as the Test Tool.
*
******************************************************************************/

#ifndef _ZtcClientCommunication_h
#define _ZtcClientCommunication_h

#include "UART_Interface.h"
#include "ZCLoptions.h"
/******************************************************************************
*******************************************************************************
* Public macros
*******************************************************************************
******************************************************************************/

/* used to extend the maximum number of PAN Descriptors returned by Scan Confirm*/
/* gZtcExtendedScanConfirmSupport_c = TRUE -->  maximum number = aMaxScanResults */
/* gZtcExtendedScanConfirmSupport_c = FALSE --> maximum number = aScanResultsPerBlock */
#define gZtcExtendedScanConfirmSupport_c FALSE

#if (gZtcExtendedScanConfirmSupport_c == TRUE)||(gZclEnableOTAServer_d == TRUE)
/* maximum size of a packet send to Client is 255 */
/* maximum payload included in that packet is 245 */
#define gZtcPacketPayloadLen_c  245
#else
/* size of a big buffer */
#define gZtcPacketPayloadLen_c  gMaxRxTxDataLength_c
#endif /* (gZtcExtendedScanConfirmSupport_c == TRUE) */

/******************************************************************************
*******************************************************************************
* Public type definitions
*******************************************************************************
******************************************************************************/
typedef enum 
{
  packetIsValid,
  packetIsTooShort,
  framingError
} ztcPacketStatus_t;


/* Format of packets exchanged between the external client and Ztc. */
/* This excludes the UART wrapper (a leading STX byte and a trailing FCS). */
typedef uint8_t clientPacketStatus_t;

typedef struct clientPacketHdr_tag {
  ztcOpcodeGroup_t opcodeGroup;
  ztcMsgType_t msgType;
  index_t len;                          /* Actual length of payload[] */
} clientPacketHdr_t;

/* The terminal checksum is actually stored at payload[len]. The checksum */
/* field insures that there is always space for it, even if the payload */
/* is full. */
typedef struct clientPacketStructured_tag {
  clientPacketHdr_t header;
  uint8_t payload[gZtcPacketPayloadLen_c];
  uint8_t checksum;
} clientPacketStructured_t;

typedef union clientPacket_tag {
  /* The entire packet as unformatted data. */
  uint8_t raw[sizeof(clientPacketStructured_t)];

  /* The packet as header + payload. */
  clientPacketStructured_t structured;

  /* A minimal packet with only a status value as a payload. */
  struct {                              /* The packet as header + payload. */
    clientPacketHdr_t header;
    uint8_t status;
  } headerAndStatus;
} clientPacket_t;

/* Used for configuring the communication(Uart, I2C or SPI) */
typedef struct BeeAppBlackBoxConfig_tag{
  UartBaudRate_t BaudRate;
  uint8_t        I2CSlaveId;
  uint8_t		 SPIBaudRate; 
} BeeAppBlackBoxConfig_t;

#define BlackBoxDefaultBaudrate_d Baudrate_115200
#define BlackBoxDefaultI2CSlaveAddr_d 0x76
#define BlackBoxDefaultSPIBaudrate gSPI_BaudRate_100000_c
/******************************************************************************
*******************************************************************************
* Public prototypes
*******************************************************************************
******************************************************************************/

/* Initializes the Ztc communication on either SCI, I2C, or SPI */
void   ZtcComm_Init(void);

/* Checks if the internal buffer is empty */
bool_t ZtcComm_BufferIsEmpty(void);

/* Send gZtcPacketToClient to the external client. If gZtcCommTxBlocking
   is TRUE, blocks until the entire packet has been sent.
   Regardless of the state of the gZtcCommTxBlocking flag, sets the global
   mZtcPacketToClientIsBusy to TRUE until the entire packet has been sent. */
void ZtcComm_WritePacketToClient(uint8_t length);

/* Send an arbitrary buffer to the external client. Block until the entire 
   buffer has been sent, regardless of the state of the gZtcCommTxBlocking flag. */
void ZtcComm_TransmitBufferBlock(uint8_t *pSrc, uint8_t length);

/* Checks if the received packet is valid */
ztcPacketStatus_t  ZtcComm_CheckPacket(void); 

/* Reads and interprets the incoming packet */
bool_t ZtcComm_ZtcReadPacketFromClient(void); 


/* Transmits the ZTC packet */
void   ZtcComm_ZtcTransmitBuffer(uint8_t *  pSrc,
                                 uint8_t   length,
                                 void (*pfCallBack)(unsigned char const *pBuf)
                                );

/******************************************************************************
*******************************************************************************
* Public memory declarations
*******************************************************************************
******************************************************************************/
/* TRUE if gZtcPacketToClient is being written to the external client.
   FALSE if it is available to be rewritten. */
extern volatile bool_t  bZtcPacketToClientIsBusy;
/* UART packet received from the external client, minus the UART envelope. */
extern clientPacket_t gZtcPacketFromClient;

/* UART packet to be sent to external client, minus the UART envelope. */
extern clientPacket_t   gZtcPacketToClient;
/* If true, ZtcWritePacketToClient() will block after enabling the UART Tx
   interrupt, until the UART driver's output buffer is empty again. */
extern bool_t           gZtcCommTxBlocking;

/* Used for configuring the communication(Uart, I2C or SPI) */
#ifdef __IAR_SYSTEMS_ICC__
extern const   BeeAppBlackBoxConfig_t BlackBoxConfig;
#else
extern volatile const   BeeAppBlackBoxConfig_t BlackBoxConfig;
#endif
#endif /* _ZtcClientCommunication_h */
