/******************************************************************************
* ZTC routines to handle the ZTC <--> external client protocol.
*
* (c) Copyright 2008, Freescale, Inc. All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
******************************************************************************/

#include "EmbeddedTypes.h"

#if gBeeStackIncluded_d
#include "ZigBee.h"
#include "BeeStackFunctionality.h"
#include "BeeStackConfiguration.h"
#endif

#include "ZtcInterface.h"


#if gZtcIncluded_d

#include "FunctionLib.h"
#if gBeeStackIncluded_d
#include "BeeCommon.h"
#endif

#include "ZtcPrivate.h"
#include "ZtcClientCommunication.h"

/******************************************************************************
*******************************************************************************
* Private macros
*******************************************************************************
******************************************************************************/

/* Expected value of the start of frame delimiter */
#define mStxValue_c     ((unsigned char) 0x02)

#define mMinValidPacketLen_c    (sizeof(clientPacketHdr_t) + sizeof(clientPacketChecksum_t))

#if gSCIInterface_d

#define getByteFromRxBuffer UartX_GetByteFromRxBuffer
#define transmit UartX_Transmit
#define isTxActive UartX_IsTxActive

#elif gI2CInterface_d

#define getByteFromRxBuffer IIC_GetByteFromRxBuffer
#define transmit IIC_Transmit
#define isTxActive IIC_IsTxActive

#else

#endif

/******************************************************************************
*******************************************************************************
* Private type definitions
*******************************************************************************
******************************************************************************/
typedef uint8_t clientPacketChecksum_t;

/******************************************************************************
*******************************************************************************
* Private function prototypes
*******************************************************************************
******************************************************************************/
static void ZtcComm_ZtcTransmitOneByte(uint8_t value);
static void ZtcComm_WritePacketCallback(unsigned char const *pBuf);
static uint8_t ZtcComm_ZtcCheckPacketFromClient(void);
static void ZtcComm_ZtcCheckNextBytes(void);

#if gSCIInterface_d
static void ZtcComm_RxCallback(void);
#elif gI2CInterface_d
static void ZtcComm_RxCallback(uint16_t transfBytesNo, uint16_t status);
#endif


/******************************************************************************
*******************************************************************************
* Private memory declarations
*******************************************************************************
******************************************************************************/


/******************************************************************************
*******************************************************************************
* Public memory declarations
*******************************************************************************
******************************************************************************/
/* Flag storing if the ZTC to client transmit buffer is busy */
volatile bool_t bZtcPacketToClientIsBusy;
/* UART packet received from the external client, minus the UART envelope. */
clientPacket_t gZtcPacketFromClient;

/* UART packet to be sent to external client, minus the UART envelope. */
clientPacket_t  gZtcPacketToClient;
/* Flag storing if the start of a frame was received on the serial interface */
bool_t          bStartOfFrameSeen = FALSE;
/* Number of received bytes over the serial interface. Does not count the STX */
index_t         bytesReceived     = 0;       

/* If true, ZtcWritePacketToClient() will block after enabling the UART Tx */
/* interrupt, until the UART driver's output buffer is empty again. */
#if (gZtcDebug_d == TRUE)
  bool_t gZtcCommTxBlocking = TRUE;
#else
  bool_t gZtcCommTxBlocking = FALSE;
#endif

/******************************************************************************
*******************************************************************************
* Public functions
*******************************************************************************
******************************************************************************/

  
/*****************************************************************************
* ZtcComm_BufferIsEmpty
*
* Checks the buffer status
******************************************************************************/  
bool_t ZtcComm_BufferIsEmpty(void)
{
#if gSCIInterface_d
  return (0 == UartX_RxBufferByteCount());
#elif gI2CInterface_d
  return (0 == IIC_RxBufferByteCount());
#endif  
}


/*****************************************************************************
* ZtcComm_WritePacketToClient
*
* Writes a packet over the serial interface
******************************************************************************/
void ZtcComm_WritePacketToClient(uint8_t length) 
{
  bZtcPacketToClientIsBusy = TRUE;  
  
  /* Send packet to client */
  ZtcComm_ZtcTransmitBuffer(gZtcPacketToClient.raw, length,  ZtcComm_WritePacketCallback);
}


/*****************************************************************************
* ZtcComm_TransmitBufferBlock
*
* Send an arbitrary buffer to the external client. Block until the entire 
* buffer has been sent, regardless of the state of the gZtcCommTxBlocking 
* flag.
******************************************************************************/
void ZtcComm_TransmitBufferBlock(uint8_t *pSrc, uint8_t length) 
{
  ZtcComm_ZtcTransmitBuffer(pSrc, length, NULL);
  /* Wait until the previous transmissions completes */
  while (isTxActive());
}


/*****************************************************************************
* ZtcComm_CheckPacket
*
* Checks if the received packet is valid. 
*****************************************************************************/
ztcPacketStatus_t ZtcComm_CheckPacket(void) 
{
  index_t                 i;
  uint8_t                payloadLength;
  clientPacketChecksum_t  checksum = 0;

  if (bytesReceived < mMinValidPacketLen_c) 
  {
    /* Too short to be valid. */
    return packetIsTooShort;
  }

  if (bytesReceived >= sizeof(gZtcPacketFromClient)) 
  {
    return framingError;
  }

  /* 
   * The packet's payloadLength field does not count:
   *  - STX               1 byte
   *  - opcodeGroup       1 byte
   *  - msgType           1 byte
   *  - payloadLength     2 byte
   *  - checksum          1 byte
   */
  payloadLength = gZtcPacketFromClient.structured.header.len;
  if (bytesReceived < payloadLength + sizeof(clientPacketHdr_t) + sizeof(checksum)) 
  {
    return packetIsTooShort;
  }

  /* 
   * If the length appears to be too long, it might be because the external
   * client is sending a packet that is too long, or the sync is lost with the external client. 
   * Assuming that the sync is lost.
   */
  if (payloadLength > sizeof(gZtcPacketFromClient.structured.payload)) 
  {
    return framingError;
  }

  /* If the length looks right, make sure that the checksum is correct. */
  if (bytesReceived == payloadLength + sizeof(clientPacketHdr_t) + sizeof(checksum)) 
  {
    for (checksum = 0, i = 0; i < payloadLength + sizeof(clientPacketHdr_t); ++i) 
    {
      checksum ^= gZtcPacketFromClient.raw[i];
    }
    if (checksum == gZtcPacketFromClient.structured.payload[payloadLength]) 
    {
      return packetIsValid;
    }
  }
  return framingError;
}  


/*****************************************************************************
* ZtcComm_Init()
*
* Initializes the ZTC to work on either SCI or I2C
*
*****************************************************************************/
void ZtcComm_Init()
{
#if gSCIInterface_d
  UartX_SetRxCallBack(ZtcComm_RxCallback);
#endif


#if gI2CInterface_d
  IIC_SetRxCallBack(ZtcComm_RxCallback);      
#endif
}

/*****************************************************************************
* ZtcComm_ZtcReadPacketFromClient
*
* Read data from the UART. If the UART ISR has received a complete packet
* (STX, packet header, and FCS), copy it to the Ztc's global
* gZtcPacketFromClient buffer and return true. Otherwise return false.
* This routine is called each time a byte is received from the UART.
* Client packets consist of a UART envelope enclosing a variable length
* clientPacket_t.
* The UART envelope is a leading sync (STX) byte and a trailing FCS.
******************************************************************************/
bool_t ZtcComm_ZtcReadPacketFromClient(void) 
{
  #define mNumOfZtcReadPktCallsWithRxBufEmpty_c 0xff
  uint8_t byte;
  uint8_t status;

  static uint8_t ztcReadPktCallsWithRxBufEmpty = mNumOfZtcReadPktCallsWithRxBufEmpty_c;
  /* If the gZtcPacketFromClient buffer has bytes that are still not processed, 
     and no bytes are coming over the Uart , start processing the bytes starting from 
     next STX*/

  if (ZtcComm_BufferIsEmpty() && bytesReceived)
  {
   if(!(--ztcReadPktCallsWithRxBufEmpty)) 
   {
    ztcReadPktCallsWithRxBufEmpty = mNumOfZtcReadPktCallsWithRxBufEmpty_c;
    ZtcComm_ZtcCheckNextBytes(); /* Get next bytes stating from next STX*/
    status = ZtcComm_ZtcCheckPacketFromClient();
    if (status == packetIsValid) 
      return TRUE;
   }
  }
  else
  {
   ztcReadPktCallsWithRxBufEmpty = mNumOfZtcReadPktCallsWithRxBufEmpty_c; 
  }
  
  while (getByteFromRxBuffer(&byte) ) 
	{
    if (!bStartOfFrameSeen) 
		{
		  /* Don't store the STX in the buffer. */
      bytesReceived     = 0;
      bStartOfFrameSeen = (byte == mStxValue_c);
      break;                            
    }

    gZtcPacketFromClient.raw[bytesReceived++] = byte;
 
    status = ZtcComm_ZtcCheckPacketFromClient();
    if (status == packetIsValid) 
      return TRUE;
                 
  }                    
  return FALSE;        
} 


/*****************************************************************************/
static uint8_t ZtcComm_ZtcCheckPacketFromClient(void)
{
  uint8_t status = framingError;
  
  while (bStartOfFrameSeen) 
  {
    status = ZtcComm_CheckPacket();
    if (status == packetIsValid) 
  	{
      bStartOfFrameSeen = FALSE;
      bytesReceived     = 0;
      return status;
    }

    if (status == packetIsTooShort) 
      return status;

    ZtcComm_ZtcCheckNextBytes();
  } 
  return status;
}

/*****************************************************************************/
static void ZtcComm_ZtcCheckNextBytes(void)
{
  uint8_t i;
    bStartOfFrameSeen = FALSE;
    for (i = 0; i < bytesReceived; ++i) 
  	{
      if (gZtcPacketFromClient.raw[i] == mStxValue_c) 
  		{
        bytesReceived     -= (i + 1);
        FLib_MemCpy(gZtcPacketFromClient.raw, gZtcPacketFromClient.raw + i + 1, bytesReceived);
        bStartOfFrameSeen  = TRUE;
        break;
      }
    }
    if(bStartOfFrameSeen == FALSE) 
      bytesReceived     = 0;
}


/*****************************************************************************
* ZtcComm_ZtcTransmitBuffer
*
* Send an arbitrary buffer to the external client
******************************************************************************/
void ZtcComm_ZtcTransmitBuffer(
                        uint8_t *  pSrc,
                        uint8_t length,
                        void (*pfCallBack)(unsigned char const *pBuf)
                       )
{
  uint8_t checksum;
  uint8_t i;

  /* Send the start of the frame */
  ZtcComm_ZtcTransmitOneByte(mStxValue_c);

  /* Send the content of the frame */  
  if(gZtcCommTxBlocking) 
  { 
    /* Stay here until the message is placed in the UART queue. Call uart with no callback */
    while (!transmit(pSrc, (uint8_t)length, NULL));
    /* Wait for the completion of the tx operation */
    while (isTxActive());
    /* Execute callback manually */
    if (pfCallBack) 
    {
      pfCallBack(NULL);
    }
  } 
  else 
  { 
    /* Stay here until the message is placed in the UART queue */
    while (!transmit(pSrc, (uint8_t)length, pfCallBack));
  }
  
  /* Compute checkSum */
  checksum = 0;
  for (i = 0; i < length; ++i) 
  {
    checksum ^= pSrc[i];
  }

  /* Send the checkSum of the frame */
  ZtcComm_ZtcTransmitOneByte(checksum);
}


/******************************************************************************
*******************************************************************************
* Private functions
*******************************************************************************
******************************************************************************/



/*****************************************************************************
* ZtcComm_ZtcTransmitOneByte
*
* Send an arbitrary byte to the external client
******************************************************************************/
static void ZtcComm_ZtcTransmitOneByte(uint8_t value)
{
  static uint8_t  byteToTransmit;
  byteToTransmit = value;
  /* Block here until a buffer is free and the message can be placed in the UART queue */
  while (!transmit(&byteToTransmit, sizeof(uint8_t), NULL)); 
  /* If uart blocking enabled, wait for tx complete flag.*/ 
  if(gZtcCommTxBlocking) 
  { 
    /* Wait until the previous transmissions complete */
    while (isTxActive());
  }
}

/*****************************************************************************
* ZtcComm_RxCallBack
*
* Function to be called each time information is received over the interface
* with the host
*****************************************************************************/

#if gSCIInterface_d
static void ZtcComm_RxCallback(void)
{
#elif gI2CInterface_d
static void ZtcComm_RxCallback(uint16_t transfBytesNo, uint16_t status) 
{
  (void) transfBytesNo;
  (void) status;
#endif
  
  TS_SendEvent(gZTCTaskID, gDataFromTestClientToZTCEvent_c);
  
}

/*****************************************************************************
* ZtcComm_WritePacketCallBack
*
* Communication interface write operation callback
******************************************************************************/
static void ZtcComm_WritePacketCallback(unsigned char const *pBuf) 
{
  /* Avoid compiler warning */
  (void) pBuf;
  bZtcPacketToClientIsBusy = FALSE;
}

#endif