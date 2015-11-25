/************************************************************************************
* This header file is provided as part of the interface to the Freescale 802.15.4
* MAC and PHY layer.
*
* The file covers the interface to the Application Support Package (ASP).
*
* (c) Copyright 2007, Freescale, Inc.  All rights reserved.
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
************************************************************************************/

#ifndef _APP_ASP_INTERFACE_H_
#define _APP_ASP_INTERFACE_H_

/************************************************************************************
*************************************************************************************
* Includes
*************************************************************************************
************************************************************************************/

#include "EmbeddedTypes.h"
#include "FunctionalityDefines.h"

/************************************************************************************
*************************************************************************************
* Public types
*************************************************************************************
************************************************************************************/

  // valid values for Asp_SetPowerLevel()
enum {
  gAspPowerLevel_m30dBm,    /* -30  dBm (hex: 0x00) */
  gAspPowerLevel_m28dBm,    /* -28  dBm (hex: 0x01) */
  gAspPowerLevel_m27dBm,    /* -27  dBm (hex: 0x02) */
  gAspPowerLevel_m26dBm,    /* -26  dBm (hex: 0x03) */
  gAspPowerLevel_m24dBm,    /* -24  dBm (hex: 0x04) */
  gAspPowerLevel_m21dBm,    /* -21  dBm (hex: 0x05) */
  gAspPowerLevel_m19dBm,    /* -19  dBm (hex: 0x06) */
  gAspPowerLevel_m17dBm,    /* -17  dBm (hex: 0x07) */
  gAspPowerLevel_m16dBm,    /* -16  dBm (hex: 0x08) */
  gAspPowerLevel_m15dBm,    /* -15  dBm (hex: 0x09) */
  gAspPowerLevel_m11dBm,    /* -11  dBm (hex: 0x0A) */
  gAspPowerLevel_m10dBm,    /* -10  dBm (hex: 0x0B) */
  gAspPowerLevel_m4d5dBm,   /* -4.5 dBm (hex: 0x0C) */
  gAspPowerLevel_m3dBm,     /* -3   dBm (hex: 0x0D) */
  gAspPowerLevel_m1d5dBm,   /* -1.5 dBm (hex: 0x0E) */
  gAspPowerLevel_m1dBm,     /* -1   dBm (hex: 0x0F) */
  gAspPowerLevel_1d7dBm,    /* 1.7  dBm (hex: 0x10) */
  gAspPowerLevel_3dBm,      /* 3    dBm (hex: 0x11) */
  gAspPowerLevel_4d5dBm     /* 4.5  dBm (hex: 0x12) */
};

#define gAspPowerLevel_m30dBm_c  (uint8_t)gAspPowerLevel_m30dBm
#define gAspPowerLevel_m28dBm_c  (uint8_t)gAspPowerLevel_m28dBm
#define gAspPowerLevel_m27dBm_c  (uint8_t)gAspPowerLevel_m27dBm
#define gAspPowerLevel_m26dBm_c  (uint8_t)gAspPowerLevel_m26dBm
#define gAspPowerLevel_m24dBm_c  (uint8_t)gAspPowerLevel_m24dBm
#define gAspPowerLevel_m21dBm_c  (uint8_t)gAspPowerLevel_m21dBm
#define gAspPowerLevel_m19dBm_c  (uint8_t)gAspPowerLevel_m19dBm
#define gAspPowerLevel_m17dBm_c  (uint8_t)gAspPowerLevel_m17dBm
#define gAspPowerLevel_m16dBm_c  (uint8_t)gAspPowerLevel_m16dBm
#define gAspPowerLevel_m15dBm_c  (uint8_t)gAspPowerLevel_m15dBm
#define gAspPowerLevel_m11dBm_c  (uint8_t)gAspPowerLevel_m11dBm
#define gAspPowerLevel_m10dBm_c  (uint8_t)gAspPowerLevel_m10dBm
#define gAspPowerLevel_m4d5dBm_c (uint8_t)gAspPowerLevel_m4d5dBm
#define gAspPowerLevel_m3dBm_c   (uint8_t)gAspPowerLevel_m3dBm
#define gAspPowerLevel_m1d5dBm_c (uint8_t)gAspPowerLevel_m1d5dBm
#define gAspPowerLevel_m1dBm_c   (uint8_t)gAspPowerLevel_m1dBm
#define gAspPowerLevel_1d7dBm_c  (uint8_t)gAspPowerLevel_1d7dBm
#define gAspPowerLevel_3dBm_c    (uint8_t)gAspPowerLevel_3dBm
#define gAspPowerLevel_4d5dBm_c  (uint8_t)gAspPowerLevel_4d5dBm

/* Legacy default 0dBm value */
#define gAspPowerLevel_0dBm_c    gAspPowerLevel_m1dBm_c

  // Valid values for aspSetNotifyReq_t->notifications.
enum {
  gAspNotifyNone_c,         // No notifications about beacon state
  gAspNotifyIdle_c,         // Notify about remaining time in Idle portion of CAP
  gAspNotifyInactive_c,     // Notify about remaining time in inactive portion of superframe
  gAspNotifyIdleInactive_c, // Notify about remaining time in Idle portion of CAP, and inactive portion of superframe
  gAspNotifyLastEntry_c     // Don't use! 
};

  // Valid values for aspTelecTestReq_t->mode
enum {
  gTestForceIdle_c=0  ,
  gTestPulseTxPrbs9_c,
  gTestContinuousRx_c,
  gTestContinuousTxMod_c,
  gTestContinuousTxNoMod_c
};

  // GetMacState status codes
enum {
  gAspMacStateIdle_c = 0,
  gAspMacStateBusy_c,
  gAspMacStateNotEmpty_c
};

  // Valid values for Asp_ConfigureRFCtlSignals signalType
typedef enum {
  gAspRfSignalANT1_c,
  gAspRfSignalANT2_c,
  gAspRfSignalTXON_c,
  gAspRfSignalRXON_c,
  gAspRfSignalMax_c
}AspRfSignalType_t;

  // Valid values for Asp_ConfigureRFCtlSignals signalFunction
typedef enum {
  gAspRfSignalFunctionGPIO_c,
  gAspRfSignalFunction1_c,
  gAspRfSignalFunction2_c,
  gAspRfSignalFunctionMax_c
}AspRfSignalFunction_t;

//-----------------------------------------------------------------------------------
//     Messages from ASP to application
//-----------------------------------------------------------------------------------
enum {
  gAspErrorCfm_c,       
  gAspAppWakeInd_c,
  gAspAppIdleInd_c,
  gAspAppInactiveInd_c,
  gAspAppEventInd_c,
  gAspMaxPrimitives_c
};

#pragma pack(1)
  // Type: gAspAppWakeInd_c
typedef struct appWakeInd_tag {
  uint8_t status;
} appWakeInd_t;

  // Type: gAspAppIdleInd_c
typedef struct appIdleInd_tag {
  uint8_t timeRemaining[3];
} appIdleInd_t;

  // Type: gAspAppInactiveInd_c
typedef struct appInactiveInd_tag {
  uint8_t timeRemaining[3];
} appInactiveInd_t;

  // Type: gAspAppEventInd_c
typedef struct appEventInd_tag {
  uint8_t dummy; // This primitive has no parameters.
} appEventInd_t;


  // Type: gAspErrorCnf_c
typedef struct appErrorCfm_tag {
  uint8_t  status;
} appErrorCfm_t;


  // ASP to application message
typedef struct aspToAppMsg_tag {
  uint8_t msgType;
  union {
    appErrorCfm_t           appErrorCfm;
    appWakeInd_t            appWakeInd;
    appIdleInd_t            appIdleInd;
    appInactiveInd_t        appInactiveInd;
    appEventInd_t           appEventInd;
  } msgData;
} aspToAppMsg_t;

#pragma pack()

/************************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
************************************************************************************/

/************************************************************************************
* Requests the current value of the transceiver internal event timer.
*   
* Interface assumptions:
*   time - pointer to a  zbClock32_t variable, where the time value will be stored 
*   
* Return value:
*   None
* 
************************************************************************************/
void    Asp_GetTimeReq(zbClock32_t *time);

/************************************************************************************
* Selects the MC1322x Demodulator type - DCD or NCD.
*   
* Interface assumptions:
*   demDCDenable - TRUE  = DCD demodulator type requested
*                  FALSE = NCD demodulator type requested
*   
* Return value:
*   None
* 
************************************************************************************/
void    Asp_SetDemodulatorType(bool_t demDCDenable);

/************************************************************************************
* This function Enables / Disables the Complementary PA Output.
*   
* Interface assumptions:
*   enable       - TRUE  = Complementary PA Output enabled
*                  FALSE = Complementary PA Output disabled
*   
* Return value:
*   None
* 
************************************************************************************/
void Asp_EnableComplementaryPAOutput(bool_t enable);

/************************************************************************************
* This interface is used to configure the function of the RF Control signals: ANT_1, ANT_2, TX_ON, RX_ON.
*   
* Interface assumptions:
*
* Possible values for the Rf control signal parameters:
*
* signalType      function                     gpioOutput            gpioOutputHigh
* -----------------------------------------------------------------------------------------
* ANT_1 (GPIO42)  GPIO (Rf Ctl disabled)       FALSE (In)            Ignored
*                 GPIO (Rf Ctl disabled)       TRUE  (Out)           TRUE(high)/FALSE(low)
*                 Function 1 (Rf controlled)   Ignored               Ignored
*
* ANT_2 (GPIO43)  GPIO (Rf Ctl disabled)       FALSE (In)            Ignored
*                 GPIO (Rf Ctl disabled)       TRUE  (Out)           TRUE(high)/FALSE(low)
*                 Function 1 (Rf controlled)   Ignored               Ignored
*
* TX_ON (GPIO44)  GPIO (Rf Ctl disabled)       FALSE (In)            Ignored
*                 GPIO (Rf Ctl disabled)       TRUE  (Out)           TRUE(high)/FALSE(low)
*                 Function 1 (Rf controlled)   Ignored               Ignored
*                 Function 2 (Rf controlled)   Ignored               Ignored
*
* RX_ON (GPIO45)  GPIO (Rf Ctl disabled)       FALSE (In)            Ignored
*                 GPIO (Rf Ctl disabled)       TRUE  (Out)           TRUE(high)/FALSE(low)
*                 Function 1 (Rf controlled)   Ignored               Ignored
*                 Function 2 (Rf controlled)   Ignored               Ignored
*   
* -----------------------------------------------------------------------------------------
* Return value:
*   gSuccess_c,
*   gInvalidParameter_c
* 
************************************************************************************/
uint8_t Asp_ConfigureRFCtlSignals(AspRfSignalType_t signalType, AspRfSignalFunction_t function, bool_t gpioOutput, bool_t gpioOutputHigh);


/************************************************************************************
* This function Enables / Disables the Power Level Lock mode.
*
* There are some restrictions when enabling the lock mode
*     - Channel 26 cannot be selected in the power level lock mode
*     - Allowed power level values (hex): 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x0C
*   
* Interface assumptions:
*   enableLock   - TRUE  = Power Level Lock Mode enabled
*                  FALSE = Power Level Lock Mode disabled
*   
* Return value:
*   gSuccess_c,
*   gDenied_c
* 
************************************************************************************/
uint8_t Asp_SetPowerLevelLockMode(bool_t enableLock);

/************************************************************************************
* Get basic state of the MAC. The caller can use this information to determine
* if it is safe to go into one of the deep sleep modes! It checks for the following:
* The MEM, SEQ, and MLME state machines must be in idle state. Otherwise return "busy"
* The queues must be empty. Otherwise return "not empty"
* Otherwise just return "idle".
*   
* Interface assumptions:
*   None
*   
* Return value:
*   one of the three values
*     gAspMacStateBusy_c;
*    gAspMacStateNotEmpty_c;
*    gAspMacStateIdle_c;
*
************************************************************************************/
uint8_t Asp_GetMacStateReq(void);

/************************************************************************************
* Wake-up the transceiver from Doze/Hibernate mode. The ATTNBi pin of the transceiver must be wired to
* a MCU port pin for this primitive to function. Otherwise, it has no effect. The AttEnable, and AttDisable
* macros must be defined in Target.h. The former must set the MCU port pin to logic high, and the latter
* must set it to logic low. If the two macros are modified e.g. due to changes in the PCB layout, the PHY
* should be modified also.
*   
* Interface assumptions:
*   None
*   
* Return value:
*   None
* 
************************************************************************************/
void    Asp_WakeReq(void);

#if gAspPowerSaveCapability_d
 
 /************************************************************************************
  * Shut down the transceiver for a given amount of time in symbols. The CLKO output pin will stop providing
  * a clock signal to the CPU if clko_en is 0 (FALSE). The CLKO output pin will continue to provide a clock 
  * signal if clko_en is 1 (TRUE). The dozeDuration parameter is the maximum time in number of symbols
  * that the transceiver will be in doze mode. The transceiver can be woken up prematurely from doze mode by
  *  a signal on the ATTNBi pin. CLKO is automatically started again when transceiver leaves doze mode.
  *   
  * Interface assumptions:
  *   dozeDuration
  *   clko_en
  *   
  * Return value:
  *   None
  * 
  ************************************************************************************/
  uint8_t Asp_DozeReq(zbClock32_t *dozeDuration, uint8_t clko_en);

  /************************************************************************************
  * Automatically shut down the transceiver during idle periods. The CLKO output pin will stop providing a
  * clock signal to the CPU if clko_en is 0 (FALSE). The CLKO output pin will continue to provide a clock
  * signal if clko_en is 1 (TRUE). The autoDozeInterval parameter is a suggested period in symbols in
  * which the transceiver will be in doze mode. This interval may be overridden if doze mode is interrupted by
  * an external signal (ATTNBi pin). If the enableWakeIndication parameter is TRUE then an
  * ASP-WAKE.Indication is sent to the APP layer each time the doze interval expires. The indication can be
  * used by the APP layer to do processing. In order to enable auto doze the autoEnable parameter must be
  * TRUE. Auto doze can be disabled by sending another ASP-AUTODOZE.Request with the autoEnable
  * parameter set to FALSE. It is recommended to use the ASP-WAKE. Indication for simple processing
  * during auto doze since it will occur frequently (if enabled) and the auto doze feature is blocked during the
  * processing of the indication in the ASP_APP SAP..
  *   
  * Interface assumptions:
  *   dozeDuration
  *   clko_en
  *   
  * Return value:
  *   Standard error code
  * 
  ************************************************************************************/
  void Asp_AutoDozeReq(bool_t  autoEnable, bool_t  enableWakeIndication, zbClock32_t *autoDozeInterval, uint8_t clko_en);

  /************************************************************************************
  * The hibernate request shuts down the transceiver. The CLKO output pin will stop providing a clock signal
  * to the CPU. Only a signal on the ATTNBi pin of the transceiver or a power loss can bring the transceiver out
  * of hibernate mode. CLKO is automatically started again when transceiver leaves hibernate mode. The
  * hibernate mode is not adequate for beaconed operation. Doze mode should be used instead when
  * transceiver timers are required.
  *   
  * Interface assumptions:
  *   None
  *   
  * Return value:
  *   Standard error code
  * 
  ************************************************************************************/
  uint8_t Asp_HibernateReq(void);

  /************************************************************************************
  * Set the default minimum doze time. If the MAC cannot doze for at least the minimum doze time, then it
  * will not enter doze mode. E.g. if the doze request is issued 3ms before the end of a beacon period the MAC
  * will not enter doze mode since the default minimum doze time is 4ms. However, if the minimum doze time
  * is changed to 2ms, then the MAC will doze for 2ms, and wake up 1ms before the next beacon assuming
  * the same timing in both examples.
  *   
  * Interface assumptions:
  *   minDozeTime
  *   
  * Return value:
  *   None
  * 
  ************************************************************************************/
  void Asp_SetMinDozeTimeReq(zbClock32_t *minDozeTime);

  /************************************************************************************
  * The acoma request shuts down the transceiver. The CLKO output pin will stop providing a clock signal to
  * the CPU if clko_en is 0 (FALSE). The CLKO output pin will continue to provide a clock signal if clko_en
  * is 1 (TRUE). Only a signal on the ATTNBi pin of the transceiver or a power loss can bring the transceiver
  * out of acoma mode. CLKO is automatically started again when transceiver leaves acoma mode. The
  * acoma mode is not suited for beaconed operation. Doze mode should be used instead when transceiver
  * timers are required.
  * The main difference between acoma mode and hibernate mode is that CLKO can be generated during
  * acoma mode – this is not possible in hibernate. Note also that acoma mode does not support timer wakeup
  * (which is possible during doze mode).
  *   
  * Interface assumptions:
  *   clko_en
  *   
  * Return value:
  *   Standard error code
  * 
  ************************************************************************************/
  uint8_t Asp_AcomaReq(uint8_t clko_en);
#endif // gAspPowerSaveCapability_d

#if gAspHwCapability_d
  /************************************************************************************
  *  This primitive sets and/or enables the CLKO output of the transceiver. If clkoEnable is TRUE, CLKO is
  *  made active, otherwise it is disabled. The CLKO output frequency is programmed depending upon the
  *  value contained in clkoRate per the CLKO frequency selection of the transceiver.
  *   clkoRate can be assigned the following values:
  *  0     -    16 MHz (default)
  *  1      -    8 MHz
  *  2     -    4 MHz
  *  3     -   2 MHz
  *  4     -    1 MHz
  *  5     -    62.5 KHz
  *  6     -    31.25 KHz
  *  7     -    15.625 KHz
  * Interface assumptions:
  *   clkoEnable
  *   clkoRate
  *   
  * Return value:
  *   Standard error code
  * 
  ************************************************************************************/
  uint8_t Asp_ClkoReq(bool_t  clkoEnable, uint8_t clkoRate);

  /************************************************************************************
  * This primitive sets the trim capacitor value for the clock. Upon receipt of the 
  * request, the trim values will be programmed into the XTAL_CNTL register from CRM module.
  *   
  *   fineTune   - only the lower 5 bits are significant
  *   coarseTune - only the lower 5 bits are significant
  *
  * Interface assumptions:
  *   
  * Return value:
  *   None
  * 
  ************************************************************************************/
  void    Asp_TrimReq(uint8_t fineTune, uint8_t coarseTune);

  /************************************************************************************
  * This primitive sets the GPIO data direction. GPIOs 3-7 are programmed as outputs if the respective bit in
  * mask is a logical 1, otherwise they are programmed as inputs. Bits 0,1 and 2 of mask are ignored.
  *   
  * Interface assumptions:
  *   directionMask
  *   
  * Return value:
  *   None
  * 
  ************************************************************************************/
  void    Asp_DdrReq(uint8_t directionMask);

  /************************************************************************************
  * This primitive reads or writes the GPIO data register. portWrite is a Boolean value. If TRUE, the respective
  *  bits in portValue will be programmed to the GPIO data register (only bits 3-7 are valid). portValue is 
  * a pointer to a uint8_t variable. If the function is called with the portWrite set to TRUE, than the value at the
  * address given by portValue will be written to the transceiver port. Otherwise, if the function is called with 
  * the portWrite set to FALSE, than the transceiver input port value will be copied at the address given 
  * by portValue. 
  *   
  * Interface assumptions:
  *   portValue
  *   
  * Return value:
  *   None
  * 
  ************************************************************************************/
  void Asp_PortReq(uint8_t portWrite, uint8_t *portValue);
#endif // gAspHwCapability_d

#if gAspEventCapability_d
  /************************************************************************************
  * This primitive can be used to schedule a notification for an application event. The notification is a single
  * instance event. If there is any conflict with the MAC operation a status of gInvalidParameter_c is
  * returned, otherwise a status of gSuccess_c is returned.
  * The eventTime parameter is a 3-byte little endian integer symbol time. The event time is relative to the
  * instance when the function call is made.
  *   
  * Interface assumptions:
  *   None
  *   
  * Return value:
  *   Standard error code
  * 
  ************************************************************************************/
  uint8_t Asp_EventReq(zbClock32_t *time);
#endif // gAspEventCapability_d

#if gBeaconedCapability_d
  /************************************************************************************
  * Request the remaining time in the inactive portion of the super frame.
  *   
  * Interface assumptions:
  *  time - pointer to a  zbClock32_t variable , where the time value will be stored 
  *   
  * Return value:
  *   Standard error code
  * 
  ************************************************************************************/
  uint8_t Asp_GetInactiveTimeReq(zbClock32_t *time);

  /************************************************************************************
  * This primitive controls the indications generated in beaconed operation. The notifications parameter can
  * be any of the following four values:
  * 1.  gAspNotifyNone_c No indications are sent to the APP layer.
  * 2.  gAspNotifyIdle_c ASP-IDLE Indication (See Section 4.2.19, “Idle Indication”) is sent.
  * 3.  gAspNotifyInactive_c ASP-INACTIVE Indication (See Section 4.2.20, “Inactive Indication”)
  * is sent.
  * 4.  gAspNotifyIdleInactive_c ASP-IDLE, and ASP-INACTIVE Indications are sent.
  * If the MAC PIB attribute macRxOnWhenIdle is set then no idle indications are sent.
  *   
  * Interface assumptions:
  *   notifications
  *   
  * Return value:
  *   Standard error code
  *
  ************************************************************************************/
  uint8_t Asp_SetNotifyReq(uint8_t notifications);
#endif // gBeaconedCapability_d

#if gAspPowerLevelCapability_d
  /************************************************************************************
  * Sets the power level of the transceiver. The powerLevel parameter can be any value between 0 (lowest transmit level), 
  * and 15 (highest transmit level). The relationship between the powerLevel parameter and the actual transmit power is not 
  * perfectly linear (or exponential). If a power level greater than 15 was specified in the Set Power Level request primitive, 
  * the returned value is gInvalidParameter_c, otherwise it is gSuccess_c.
  *
  * In order to maintain compatibility with the Abel/Toro based solutions,
  * we accept only 16 power levels (0x00 - 0x0F). MC1322x HW actually has
  * 22 power level steps, resulting in 21 unique output power levels.
  * We will only use 16 of these, depicted in the table below.
  * +---------------------+----------------+
  * | powerLevel      |Pout (dBm) |
  * +---------------------+----------------+
  * | 0x00               |      -30      |
  * | 0x01               |      -28      |
  * | 0x02               |      -27      |
  * | 0x03               |      -26      |
  * | 0x04               |      -24      |
  * | 0x05               |      -21      |
  * | 0x06               |      -19      |
  * | 0x07               |      -17      |
  * | 0x08               |      -16      |
  * | 0x09               |      -15      |
  * | 0x0A              |      -11      |
  * | 0x0B              |      -10      |
  * | 0x0C              |      -4.5     |
  * | 0x0D              |      -3        |
  * | 0x0E              |      -1.5     |
  * | 0x0F              |       -1       |
  * | 0x10 (unused) |      1.7      |
  * | 0x11 (unused) |       3        |
  * +--------------------+----------------+
  *                             
  * Interface assumptions:
  *   mAspPowerLevel
  *   
  * Return value:
  *   Standard error code
  * 
  ************************************************************************************/
  uint8_t Asp_SetPowerLevel(uint8_t powerLevel);
  
  /************************************************************************************
  * Requests the current power level of the transceiver.
  *   
  * Interface assumptions:
  *   mAspPowerLevel
  *   
  * Return value:
  *   The current power level of the transceiver.
  ************************************************************************************/
  uint8_t Asp_GetPowerLevel();
#endif // gAspPowerLevelCapability_d

/************************************************************************************
* Application layer service access point for ASP indications.
*   
* Interface assumptions:
*   None
*   
* Return value:
*   Standard error code
* 
************************************************************************************/
#if gAspCapability_d
uint8_t ASP_APP_SapHandler
  (
  aspToAppMsg_t *pMsg
  );

extern  uint8_t (*pfASP_APP_SapHandler)(aspToAppMsg_t* pMsg);
#endif //gAspCapability_d


#endif /* _APP_ASP_INTERFACE_H_ */
