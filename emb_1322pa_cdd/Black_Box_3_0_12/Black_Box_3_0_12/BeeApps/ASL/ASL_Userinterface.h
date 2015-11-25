/******************************************************************************
* This file  contain the User interface general functions for the applications.
*
* (c) Copyright 2008, Freescale, Inc. All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
******************************************************************************/

#ifndef _ASL_USER_INTERFACE_H_
#define _ASL_USER_INTERFACE_H_

#include "Keyboard.h"
#include "Display.h"
#include "Led.h"
#include "AppZdoInterface.h"
#include "BeeAppInit.h"
#include "BeeApp.h"
#include "ASL_ZCLInterface.h"
#include "SEProfile.h"

/******************************************************************************
*******************************************************************************
* Public macros
*******************************************************************************
******************************************************************************/

#ifndef gTargetMC13213NCB
#define gTargetMC13213NCB 0
#endif  

#ifndef gTargetQE128EVB
#define gTargetQE128EVB 0    
#endif

#ifndef gTargetMC1322xLPB
#define gTargetMC1322xLPB 0
#endif

#ifndef gTargetMC1322xNCB
#define gTargetMC1322xNCB 0
#endif


#define ASL_SetLed(LEDNr,state) ASL_AppSetLed(LEDNr,state)

#ifndef gMatchDescEnable_d
   #define gMatchDescEnable_d  0
#endif

/* If target is MC1322x Low Power Node (which has only 2 switches and 2 LEDs)
 * invert short SW2-short SW3 and LED2-LED3 functionality in Config Mode so that
 * ED binding/match and binding status are still available from the UI; SW3 and
 * LED3 are not actually used on the board; SW2 is also not availble while LP
 * board is in application mode and low power mode is enabled */
#if gTargetMC1322xLPB
#define BINDING_LED LED2
#define PERMIT_JOIN_LED LED3
#define BINDING_SW gKBD_EventSW2_c
#define PERMIT_JOIN_SW gKBD_EventSW3_c
#else
#define BINDING_LED LED3
#define PERMIT_JOIN_LED LED2
#define BINDING_SW gKBD_EventSW3_c
#define PERMIT_JOIN_SW gKBD_EventSW2_c
#endif      


#define PermitJoinOn 0xFF
#define PermitJoinOff 0x00

#define dLed1 0x01
#define dLed2 0x02
#define dLed3 0x04
#define dLed4 0x08
#define dLowestChannelVal 11
#define dMaxChannelVal    26


/* Maximum message length supported by the LCD display */
#if gTargetMC1322xNCB
#define gMaxRcvdMsgToDisplay  32
#else
#define gMaxRcvdMsgToDisplay  16
#endif


#if  (gZclEnableOTAProgressReport_d == TRUE) 
extern uint8_t gOtaProgressReportStatus;
#endif

/******************************************************************************
*******************************************************************************
* Public prototypes
*******************************************************************************
******************************************************************************/

void ASL_InitUserInterface(char *pApplicationName);
void ASL_TurnOffAllLeds(void);
void ASL_DisplayChangeToCurrentMode(uint8_t DeviceMode);
void ASL_UpdateDevice( zbEndPoint_t ep, SystemEvents_t event);
void ASL_HandleKeys(key_event_t event);
void ASL_ChangeUserInterfaceModeTo(UIMode_t DeviceMode);
void ASL_AppSetLed(LED_t LEDNr,LedState_t state);
void ASL_ToggleLed(LED_t LEDNr);
void ASL_ConfigSetLed(LED_t LEDNr,LedState_t state);
void ASL_DisplayTemperature(uint8_t TypeOfTemperature,int16_t Temperature, uint8_t displayModeFlag, uint8_t HeatCoolUnitStatus);
void ASL_DisplayAppSettingsLeds(void);
void ASL_AppSettingsSetLed(LED_t LEDNr,LedState_t state);
void ASL_PrintLdCtrlEvtStatus(uint8_t status);
void ASL_PrintLdCtrlEvt(uint8_t fieldNr, zclCmdDmndRspLdCtrl_LdCtrlEvtReq_t *pEvt);
void ASL_PrintUTCTime(void);
void ASL_PrintMessage(uint8_t *aStr);
void ASL_PrintField(uint8_t fieldNr, void *pData);
void ASL_PrintPriceEvtStatus(uint8_t status);

void ASL_DisplayFanSpeed(uint8_t speed);
void ASL_DisplayDutyCycle(uint8_t dutyCycle);

#ifdef gHaThermostat_d
#if gHaThermostat_d
/*****************************************************************************
* Special case for thermostat: make a reverse binding than the one created
* by enddevice bind for the temperature measurement cluster  as reporting needs
* to happen from temp sensor to thermostat
*****************************************************************************/
void CreateTempMeasurementBinding(void);
#endif //gHaThermostat_d
#endif
#if gLCDSupported_d
void ASL_LCDWriteString(char *pstr);
void ASL_DisplayAppName(void);
void ASL_PrintEvent(char *eventStr);


#ifdef gHaDimmableLight_d
#if gHaDimmableLight_d
  void ASL_DisplayLevelBarMC1322xNCB(zclLevelValue_t Level, zclCmd_t OnOffStatus);
  void ASL_DisplayLevelPercentageMC1322xNCB(zclLevelValue_t Level, zclCmd_t OnOffStatus);
#endif  /* #if gHaDimmableLight_d */
#endif  /* #ifdef gHaDimmableLight_d */

#ifdef gHaThermostat_d
#if gHaThermostat_d
  void ASL_DisplayThermostatStatusMC1322xNCB(uint8_t heatStatus, uint8_t fanStatus);
#endif  /* #if gHaThermostat_d */
#endif  /* #ifdef gHaThermostat_d */

#else   /* #if gLCDSupported_d */
  #define ASL_LCDWriteString(pstr)
  #define ASL_DisplayAppName()      /* empty function if no LCD */
#endif  /* #if gLCDSupported_d */
/******************************************************************************
*******************************************************************************
* Public type definitions
*******************************************************************************
******************************************************************************/


enum {
  gASL_FieldPriceTier_c,
  gASL_FieldLabel_c,
  gASL_FieldUom_c,
  gASL_FieldPrice_c,
  gASL_FieldDuration_c,
  gASL_FieldMessage_c,
  gASL_FieldTime_c,
  gASL_FieldLast_c
};

typedef struct ASL_DisplayStatus_tag {
	uint8_t Leds;
} ASL_DisplayStatus_t;

typedef struct ASL_SendingNwkData_tag {
	zbAddrMode_t gAddressMode;  /* indirect, direct16 or group */
	zbGroupId_t  aGroupId;
	zbSceneId_t  aSceneId;
	zbNwkAddr_t  NwkAddrOfIntrest;
  zbEndPoint_t endPoint;
} ASL_SendingNwkData_t;

enum {
  gASL_HCUOff_c,
  gASL_HCUHeatOn_c,
  gASL_HCUCoolOn_c
};

enum {
	gASL_LocalTemperature_c,
	gASL_DesireTemperature_c
};

/* all the ASL data that is restored from NVM */
#ifdef __IAR_SYSTEMS_ICC__
#pragma pack(1)
#endif
typedef struct ASL_Data_tag {
  uint8_t userInterfaceMode;
  ASL_DisplayStatus_t appModeDisplay;
  ASL_DisplayStatus_t configModeDisplay;
  ASL_DisplayStatus_t appSettingDisplay;
  uint8_t appState;
  uint8_t PermitJoinStatusFlag;
  uint8_t channelNumber;
  ASL_SendingNwkData_t sendingNwkData;
} ASL_Data_t;
#ifdef __IAR_SYSTEMS_ICC__
#pragma pack()
#endif
/* handy macros */
#define gAppModeDisplay       gAslData.appModeDisplay
#define gConfigModeDisplay    gAslData.configModeDisplay
#define gAppSettingDisplay    gAslData.appSettingDisplay
#define appState              gAslData.appState
#define PermitJoinStatusFlag  gAslData.PermitJoinStatusFlag
#define gmAslChannelNumber    gAslData.channelNumber
#define gSendingNwkData       gAslData.sendingNwkData

/* this needs to be a var so that it works ok across NVM resets */
extern  uint8_t gmUserInterfaceMode;

/******************************************************************************
*******************************************************************************
* Public memory declarations
*******************************************************************************
******************************************************************************/

extern ASL_Data_t gAslData;
extern const uint8_t gaHexValue[];
extern zbEndPoint_t appEndPoint;

#endif  /*  _ASL_USER_INTERFACE_H_  */
