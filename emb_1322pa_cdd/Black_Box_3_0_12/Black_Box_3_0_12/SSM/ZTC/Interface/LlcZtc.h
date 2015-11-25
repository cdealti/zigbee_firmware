/******************************************************************************
* LlcZtc.h
* This module contains a fresh implementation of the ZigBee Test Client (ZTC).
*
* Copyright (c) 2006, Freescale, Inc.  All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from  Freescale Semiconductor.
*
* This file contains declarations for the ASP functions
*
******************************************************************************/
#ifndef _LlcZtc_h
#define _LlcZtc_h

/****************************************************************************/
/* ASP types */

typedef uint8_t AppLlcMsgType_t;

#define gllcMsgTypeSetLedReq_c             0x00
#define gllcMsgTypeStartDACwaveReq_c       0x01 /* Start 1KHz SIN Wave with DAC */
#define gllcMsgTypeStopDACwaveReq_c        0x02 /* Stop  1KHz SIN Wave with DAC */
#define gllcMsgTypeStartPWMwaveReq_c       0x03 /* Start 1KHz SIN Wave with PWM */
#define gllcMsgTypeStopPWMwaveReq_c        0x04 /* Stop  1KHz SIN Wave with PWM */
#define gllcMsgTypeStartPWMReq_c           0x05 /* Start PWM generation with variable duty */
#define gllcMsgTypeStopPWMReq_c            0x06 /* Stop  PWM generation */
#define gllcMsgTypeReadSensorReq_c         0x07 /* Read Sensor (ADC) */
#define gllcMsgTypeSetASStateReq_c         0x08 /* Set accelerometer sleep state */
#define gllcMsgTypeReadSwReq_c             0x09 /* Read switches states */
#define gllcMsgTypeCrystalTrimmReq_c       0x0a /* Read switches states */
#define gllcMsgTypeDisplayChessBoardReq_c  0x0b /* Display Chess Board */
#define gllcMsgTypeStartReceiverReq_c      0x0c /* Start receiver */
#define gllcMsgTypeGetReceivedPacketsReq_c 0x0d /* Get the number of received packets */
#define gllcMsgTypeStartTransmitterReq_c   0x0e /* Start transmitter */
#define gllcMsgTypeTransmitReq_c           0x0f /* Transmit Packets */
#define gllcMsgTypeTransmitConfirm_c       0x10 /* Transmission Finished */
#define gllcMsgTypeSetupJTAGNEXUSPinsReq_c 0x11 /* Setup JATG and NEXUS pins to input */
#define gllcMsgTypeReadJTAGNEXUSPinsReq_c  0x12 /* Read JATG and NEXUS pins to input */
#define gllcMsgTypeStart12MhzPwmReq_c      0x13 /* Start 12MHz PWM generation on TMR1 */
#define gllcMsgTypeStop12MhzPwmReq_c       0x14 /* Stop 12MHz PWM generation on TMR1 */
#define gllcMsgSetupGpioPinsReq_c          0x15 /* Setup GPIO pins */

typedef enum {
  gAccelSensor_c = 0,
  gPressureSensor_c,
  gTempSensor_c,
  gAudioSensor_c
} SensorType_t;
  
typedef enum chessSquareSize_tag{
  gLlcSize1Pixel_c  =  1,
  gLlcSize2Pixel_c  =  2,
  gLlcSize4Pixel_c  =  4,
  gLlcSize8Pixel_c  =  8,
  gLlcSize16Pixel_c = 16,
  gLlcSize32Pixel_c = 32,
  gLlcSize64Pixel_c = 64  
}chessSquareSize_t;

#pragma pack(1)

typedef struct llcSetLedReq_tag {  /* LlcSetLed.Request               */
  bool_t led1on;
  bool_t led2on;  
  bool_t led3on;  
  bool_t led4on;  
} llcSetLedReq_t;

typedef struct llcSetVolumeReq_tag {  /* LlcSetLed.Request             */
  uint8_t volume;  
} llcSetVolumeReq_t;

typedef struct llcSetPwmDutyReq_tag
{
  uint8_t duty;
  uint8_t volume;
}llcSetPwmDutyReq_t;

typedef struct llcReadSensor_tag  {  /* LlcReadAdc.Request */
  SensorType_t  sensor;
  uint16_t readVal[3];
} llcReadSensorReq_t;

typedef struct llcSetASStareReq_tag {  /* LlcSetASState.Request */
  bool_t sleepState;
} llcSetASStateReq_t;

typedef struct llcReadSw_tag  {  /* LlcReadAdc.Request */
  bool_t swState[5];
} llcReadSwReq_t;

typedef struct llcCrystalTrimm_tag  
{  /* LlcReadAdc.Request */
  uint8_t courseTune; 
  uint8_t fineTune; 
} llcCrystalTrimmReq_t;

typedef struct llcDisplayChessBoardReq_tag 
{ /* Llc_DisplayChessBoard.Request */
  bool_t mode;
  bool_t backLightOn;
  chessSquareSize_t squareSize;
} llcDisplayChessBoardReq_t;

typedef struct llcStartReceiverReq_tag
{
  uint8_t  panId[2];
  uint8_t  logicalChannel;
  uint8_t  shortAddr[2];
}llcStartReceiverReq_t;

typedef struct llcGetReceivedPacketsReq_tag
{
 uint16_t packetCnt;
}llcGetReceivedPacketsReq_t;

typedef struct llcStartTransmitterReq_tag
{
  uint8_t  panId[2];
  uint8_t  logicalChannel;
  uint8_t  shortAddr[2];
}llcStartTransmitterReq_t;

typedef struct llcTransmitReq_tag
{
  uint8_t  destPanId[2];
  uint8_t  destShortAddr[2];
  uint8_t  packetCount;
  uint8_t  len;
}llcTransmitReq_t;

typedef struct llcReadJTAGNEXUSPinsReq_tag
{
  uint32_t  pins;
}llcReadJTAGNEXUSPinsReq_t;

typedef struct llcSetupJTAGNEXUSPinsReq_tag
{
  bool_t  restore;
}llcSetupJTAGNEXUSPinsReq_t;

typedef struct llcSetupGpioPinsReq_tag
{
  bool_t   readWriteNeg;
  uint8_t  gpioPort;
  uint8_t  gpioAttr;
  uint32_t gpioValue;
  uint32_t gpioMask;
}llcSetupGpioPinsReq_t;

typedef union llcReq_tag {
  llcSetLedReq_t             llcSetLedReq;  
  llcSetVolumeReq_t          llcSetVolumeReq;
  llcSetPwmDutyReq_t         llcSetPwmDutyReq;
  llcReadSensorReq_t         llcReadSensorReq;
  llcSetASStateReq_t         llcSetASStateReq;
  llcReadSwReq_t             llcReadSwReq;
  llcCrystalTrimmReq_t       llcCrystalTrimmReq;
  llcDisplayChessBoardReq_t  llcDisplayChessBoardReq;
  llcStartReceiverReq_t      llcStartReceiverReq;
  llcGetReceivedPacketsReq_t llcGetReceivedPacketsReq;
  llcStartTransmitterReq_t   llcStartTransmitterReq;
  llcTransmitReq_t           llcTransmitReq;
  llcReadJTAGNEXUSPinsReq_t  llcReadJTAGNEXUSPinsReq;
  llcSetupJTAGNEXUSPinsReq_t llcSetupJTAGNEXUSPinsReq;
  llcSetupGpioPinsReq_t      llcSetupGpioPinsReq;
}llcReq_t;

/* APP to ASP message */
typedef struct AppToLlcMessage_tag {
  AppLlcMsgType_t msgType;
  llcReq_t  req;
} AppToLlcMessage_t;

#pragma pack()

#endif  /* _LlcZtc_h */

