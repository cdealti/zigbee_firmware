/******************************************************************************
* AspZtc.h
* This module contains a fresh implementation of the ZigBee Test Client (ZTC).
*
* Copyright (c) 2008, Freescale, Inc.  All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from  Freescale Semiconductor.
*
* This file contains declarations for the ASP functions
*
******************************************************************************/
#ifndef _AspZtc_h
#define _AspZtc_h

/****************************************************************************/
/* ASP types */

typedef uint8_t AppAspMsgType_t;
#define aspMsgTypeGetTimeReq_c          0x00
#define aspMsgTypeGetInactiveTimeReq_c  0x01
#define aspMsgTypeGetMacStateReq_c      0x02
#define aspMsgTypeDozeReq_c             0x03
#define aspMsgTypeAutoDozeReq_c         0x04
#define aspMsgTypeAcomaReq_c            0x05
#define aspMsgTypeHibernateReq_c        0x06
#define aspMsgTypeWakeReq_c             0x07
#define aspMsgTypeEventReq_c            0x08
#define aspMsgTypeClkoReq_c             0x09
#define aspMsgTypeTrimReq_c             0x0A
#define aspMsgTypeDdrReq_c              0x0B
#define aspMsgTypePortReq_c             0x0C
#define aspMsgTypeSetMinDozeTimeReq_c   0x0D
#define aspMsgTypeSetNotifyReq_c        0x0E
#define aspMsgTypeSetPowerLevel_c       0x0F
#define aspMsgTypeGetPowerLevel_c       0x1F
#define aspMsgTypeTelecTest_c           0x10
#define aspMsgTypeTelecSetFreq_c        0x11
#define aspMsgTypeGetInactiveTimeCnf_c  0x12
#define aspMsgTypeGetMacStateCnf_c      0x13
#define aspMsgTypeDozeCnf_c             0x14
#define aspMsgTypeAutoDozeCnf_c         0x15
#define aspMsgTypeTelecSendRawData_c    0x16


#pragma pack(1)

typedef struct aspGetTimeReq_tag {         /* AspGetTime.Request         */
  zbClock24_t time;
} aspGetTimeReq_t;

typedef struct aspGetInactiveTimeReq_tag { /* AspGetInactiveTime.Request */
  zbClock24_t time;
} aspGetInactiveTimeReq_t;

typedef struct aspGetMacStateReq_tag {     /* AspGetMacState.Request     */
  uint8_t placeHolder;
} aspGetMacStateReq_t;

typedef struct aspDozeReq_tag {            /* AspDoze.Request            */
  zbClock24_t dozeDuration;
  uint8_t clko_en;
} aspDozeReq_t;

typedef struct aspAutoDozeReq_tag {        /* AspAutoDoze.Request        */
  bool_t autoEnable;
  bool_t enableWakeIndication;
  zbClock24_t autoDozeInterval;
  uint8_t clko_en;
} aspAutoDozeReq_t;

typedef struct aspAcomaReq_tag {           /* AspAcoma.Request           */
  uint8_t clko_en;
} aspAcomaReq_t;

typedef struct aspHibernateReq_tag {       /* AspHibernate.Request       */
  uint8_t placeHolder;
} aspHibernateReq_t;

typedef struct aspWakeReq_tag {            /* AspWake.Request            */
  uint8_t placeHolder;
} aspWakeReq_t;

typedef struct aspEventReq_tag {           /* AspEvent.Request           */
  zbClock24_t eventTime;
} aspEventReq_t;

typedef struct aspClkoReq_tag {            /* AspClko.Request            */
  uint8_t clkoEnable;
  uint8_t clkoRate;
} aspClkoReq_t;

typedef struct aspTrimReq_tag {            /* AspTrim.Request            */
  uint8_t fineTune;
  uint8_t coarseTune;
} aspTrimReq_t;

typedef struct aspDdrReq_tag {             /* AspDdr.Request             */
  uint8_t directionMask;
} aspDdrReq_t;

typedef struct aspPortReq_tag {            /* AspPort.Request            */
  uint8_t portWrite;
  uint8_t portValue;
} aspPortReq_t;

typedef struct aspSetMinDozeTimeReq_tag {  /* AspSetMinDozeTime.Request  */
  zbClock24_t minDozeTime;
} aspSetMinDozeTimeReq_t;

typedef struct aspSetNotifyReq_tag {       /* AspSetNotify.Request       */
  uint8_t notifications;
} aspSetNotifyReq_t;

typedef struct aspSetPowerLevelReq_tag {   /* AspSetPowerLevel.Request   */
  uint8_t powerLevel;
} aspSetPowerLevelReq_t;

typedef struct aspGetPowerLevelReq_tag {   /* AspGetPowerLevel.Request   */
  uint8_t powerLevel;
} aspGetPowerLevelReq_t;

typedef struct aspTelecTest_tag {          /* AspTelecTest.Request       */
  uint8_t mode;
} aspTelecTest_t;

typedef struct aspTelecsetFreq_tag {       /* AspTelecSetFreq.Request    */
  uint8_t channel;
} aspTelecsetFreq_t;

typedef struct aspTelecSendRawData_tag {  /* AspTelecSendRawData.Request */
  uint8_t  length;
  uint8_t* dataPtr;
} aspTelecSendRawData_t;


typedef union aspReq_tag {
  aspGetTimeReq_t         aspGetTimeReq;
  aspGetInactiveTimeReq_t aspGetInactiveTimeReq;
  aspDozeReq_t            aspDozeReq;
  aspAutoDozeReq_t        aspAutoDozeReq;
  aspAcomaReq_t           aspAcomaReq;
  aspHibernateReq_t       aspHibernateReq;
  aspWakeReq_t            aspWakeReq;
  aspEventReq_t           aspEventReq;
  aspClkoReq_t            aspClkoReq;
  aspTrimReq_t            aspTrimReq;
  aspDdrReq_t             aspDdrReq;
  aspPortReq_t            aspPortReq;
  aspSetMinDozeTimeReq_t  aspSetMinDozeTimeReq;
  aspSetNotifyReq_t       aspSetNotifyReq;
  aspSetPowerLevelReq_t   aspSetPowerLevelReq;
  aspGetPowerLevelReq_t   aspGetPowerLevelReq;
  aspTelecTest_t          aspTelecTest;
  aspTelecsetFreq_t       aspTelecsetFreq;
  aspTelecSendRawData_t   aspTelecSendRawData;
} aspReq_t;


/* APP to ASP message */
typedef struct AppToAspMessage_tag {
  AppAspMsgType_t msgType;
  struct {
    aspReq_t  req;
  } msgData;
} AppToAspMessage_t;

#pragma pack()

#endif

