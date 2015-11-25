/*****************************************************************************
* ZigBee Application.
*
* Copyright (c) 2007, Freescale, Inc. All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
*****************************************************************************/
#include "EmbeddedTypes.h"
#include "ZigBee.h"
#include "BeeAppInit.h"
#include "AppAfInterface.h"
#include "EndPointConfig.h"
#include "AppZdoInterface.h"
#include "ZdpManager.h"
#include "ZDOStateMachineHandler.h"
#include "BeeStackInterface.h"
#include "ASL_ZdpInterface.h"
#include "ASL_UserInterface.h"
#include "ASL_ZCLInterface.h"
#include "ApsMgmtInterface.h"
#include "keyboard.h"
#include "Sound.h"
#include "TMR_Interface.h"
#include "led.h"
#include "beeapp.h"
#include "BeeStackInit.h"
#if gLpmIncluded_d 
#include "PWR_Interface.h"
#endif
#include "ZclSe.h"
#ifdef gHcGenericApp_d    
#if gHcGenericApp_d    
#include "Oep11073.h"
#include "ZclProtocInterf.h"
#endif
#endif

#if gZclEnableOTAClient_d 
#include "ZclOta.h"
#endif

/******************************************************************************
*******************************************************************************
* Private macros
*******************************************************************************
******************************************************************************/

/*Temperature limits for turning on the Leds on the temperature display*/
#define Limit0   -500 
#define Limit1   1000
#define Limit2   2000
#define Limit3   3000
#define Limit4   4000

#define mSecDay_c ((uint32_t)24 * 60 * 60)
#define mPrintStringEvStatus_c TRUE
#define mStartLine  3


/******************************************************************************
*******************************************************************************
* Public prototypes
*******************************************************************************
******************************************************************************/

/******************************************************************************
*******************************************************************************
* Private prototypes
*******************************************************************************
******************************************************************************/
void ASL_TurnOnLed(LedState_t Led);
void ASL_TurnOffLed(LedState_t Led);
void ASL_FlashLed(LedState_t Led);
void ASL_StopFlashLed(LedState_t Led);
void ZdoStoppedAction(void);
#if (gMC1323xMatrixKBD_d == 1)
void HandleKeysMC1323xMatrixKBD( uint8_t events, uint8_t pressedKey );
#endif
void BeeAppHandleKeys( key_event_t events );
void ASL_ChangeLedsStateOnMode(LED_t LEDNr,LedState_t state, UIMode_t mode);

#if gLCDSupported_d
void ASL_DisplayLine2OnLCD(void);
void ASL_DisplayAppName(void);
void ASL_Dec2Str(char *pStr,uint16_t decValue,index_t digits);
#else
#define ASL_DisplayLine2OnLCD()   /* empty function if no LCD */
#define ASL_DisplayAppName()      /* empty function if no LCD */
#define ASL_Dec2Str(pStr,decValue,digits)
#endif

/******************************************************************************
*******************************************************************************
* Private type definitions
*******************************************************************************
******************************************************************************/
/* None */

/******************************************************************************
*******************************************************************************
* Private memory declarations
*******************************************************************************
******************************************************************************/
const uint8_t gsASL_LeaveNetwork[] = "Leave network";
#if (gCoordinatorCapability_d)
  const uint8_t gsASL_StartingNwk[] = "Starting Network";
#else
  const uint8_t gsASL_StartingNwk[] = "Joining Network";
#endif

/* LCD Strings use for displaing diferente messages */
const uint8_t gsASL_ChannelSelect[]=      "Select channel";
const uint8_t gsASL_Running[]=            "Running Device";
const uint8_t gsASL_PermitJoinEnabled[]=  "Permit Join (E)";
const uint8_t gsASL_PermitJoinDisabled[]= "Permit Join (D)";
#ifndef SmartEnergyApplication_d
const uint8_t gsASL_Binding[]=            "Binding";
const uint8_t gsASL_BindingFail[]=        "Binding Fail";
const uint8_t gsASL_BindingSuccess[]=     "Binding Success";
const uint8_t gsASL_UnBinding[]=          "UnBinding";
const uint8_t gsASL_UnBindingFail[]=      "UnBinding Fail";
const uint8_t gsASL_UnBindingSuccess[]=   "UnBinding Success";
const uint8_t gsASL_RemoveBind[]=         "Remove Binding";
#endif
const uint8_t gsASL_ResetNode[]=          "ResetNode";
const uint8_t gsASL_IdentifyEnabled[]=    "Identify Enabled";
const uint8_t gsASL_IdentifyDisabled[]=   "Identify Disabled";
#if gMatch_Desc_req_d
#ifndef SmartEnergyApplication_d
const uint8_t gsASL_Matching[]=           "Matching";
const uint8_t gsASL_MatchFound[]=         "Match Found";
const uint8_t gsASL_MatchFail[]=          "Match Fail";
const uint8_t gsASL_MatchNotFound[]=      "No Match Found";
#endif
#endif
#ifndef gHaCombinedInterface_d
#ifndef SmartEnergyApplication_d
const uint8_t gsASL_SwitchTypeMomentary[]="Momentary SW";
const uint8_t gsASL_SwitchTypeToggle[]=   "Toggle SW";
const uint8_t gsASL_SwitchActionOn[]=     "On Action";
const uint8_t gsASL_SwitchActionOff[]=    "Off Action";
const uint8_t gsASL_SwitchActionToggle[]= "Toggle Action";
const uint8_t gsASL_DispModeCelsius[]=    "Celsius  ";
const uint8_t gsASL_DispModeFahrenheit[]= "Fahrnheit";
#endif
#endif


uint8_t *gpszAslAppName;
const uint8_t gszAslCfgMode[] = "Cfg";
const uint8_t gszAslAppMode[] = "App";
const uint8_t gszAslNoNwkAddr[] = "----";


#if (gLCDSupported_d == TRUE)

const uint8_t gProgressBarBitmapPixels[]=
{
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* Empty                 */
  0xFF, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, /* Left empty progress   */
  0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, /* Middle empty progress */
  0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0xFF, /* Right empty progress  */
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF  /* Filled progress       */
};
char gLevelLcdString[16] = "Level =        ";
uint8_t gProgressBarIndex[11];

#ifdef gHaThermostat_d
const uint8_t gFanIconBitmapPixels[]=
{
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0x00 - Empty         */
  0x00, 0x00, 0x00, 0x00, 0xFE, 0xFE, 0xFC, 0xF8, /* 0x01 - Fan 1-2       */
  0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0x02 - Fan 1-3       */
  0x00, 0x18, 0x1C, 0x1E, 0x1F, 0x1F, 0x1F, 0x1F, /* 0x03 - Fan 2-1       */
  0x1E, 0x9E, 0xDC, 0xF8, 0xEF, 0x3F, 0x77, 0xF3, /* 0x04 - Fan 2-2       */
  0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0x70, 0x30, /* 0x05 - Fan 2-3       */
  0x1E, 0x3F, 0x7F, 0xFF, 0xFF, 0x00, 0x00, 0x00, /* 0x06 - Fan 3-2       */
  0x00, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, /* 0x07 - Fan 3-3       */
  0x00, 0x00, 0x02, 0x06, 0x0E, 0x1E, 0x3E, 0x7E, /* 0x08 - ArrowDown 2-1 */
  0xFE, 0x7E, 0x3E, 0x1E, 0x0E, 0x06, 0x02, 0x00, /* 0x09 - ArrowDown 2-2 */
  0x00, 0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, /* 0x0A - ArrowUp   2-1 */
  0xFE, 0xFC, 0xF8, 0xF0, 0xE0, 0xC0, 0x80, 0x00, /* 0x0B - ArrowUp   2-2 */
  
  0x00, 0x00, 0x00, 0xE0, 0xC0, 0xC0, 0xF0, 0x00, /* 0x0C - Snow 1-1 */
  0x08, 0x0C, 0x18, 0xFE, 0xFE, 0x10, 0x18, 0x0C, /* 0x0D - Snow 1-2 */  
  0x00, 0xF0, 0x90, 0xC0, 0xC0, 0x40, 0x00, 0x00, /* 0x0E - Snow 1-3 */  
  0x00, 0x00, 0x00, 0xB6, 0xB2, 0xE3, 0xE1, 0x63, /* 0x0F - Snow 2-1 */    
  0x63, 0x32, 0x36, 0xFF, 0xFF, 0x1C, 0x36, 0x36, /* 0x10 - Snow 2-2 */    
  0xE3, 0xE1, 0xE3, 0xA2, 0xB6, 0x02, 0x00, 0x00, /* 0x11 - Snow 2-3 */    
  0x00, 0x00, 0x01, 0x03, 0x01, 0x00, 0x07, 0x00, /* 0x12 - Snow 3-1 */    
  0x00, 0x18, 0x0C, 0x37, 0x3F, 0x04, 0x0C, 0x18, /* 0x13 - Snow 3-2 */    
  0x01, 0x07, 0x00, 0x01, 0x01, 0x01, 0x00, 0x00, /* 0x14 - Snow 3-3 */
  
  0x00, 0x00, 0x0C, 0x1C, 0x38, 0x70, 0xE0, 0x40, /* 0x15 - Sun 1-1 */
  0x00, 0x80, 0x80, 0xDF, 0xDF, 0x80, 0x80, 0x00, /* 0x16 - Sun 1-2 */  
  0x00, 0xC0, 0xE0, 0x70, 0x38, 0x1C, 0x0C, 0x00, /* 0x17 - Sun 1-3 */  
  0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x18, 0x7E, /* 0x18 - Sun 2-1 */    
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, /* 0x19 - Sun 2-2 */    
  0xFE, 0x3C, 0x00, 0x18, 0x18, 0x18, 0x18, 0x18, /* 0x1A - Sun 2-3 */    
  0x00, 0x00, 0x30, 0x38, 0x1C, 0x0E, 0x07, 0x02, /* 0x1B - Sun 3-1 */    
  0x00, 0x01, 0x03, 0xFB, 0xFB, 0x03, 0x01, 0x00, /* 0x1C - Sun 3-2 */    
  0x00, 0x02, 0x07, 0x0E, 0x1C, 0x38, 0x30, 0x00, /* 0x1D - Sun 3-3 */  
  
  0x00, 0x04, 0x04, 0x08, 0x08, 0x10, 0x10, 0x10, /* 0x1E - Wave 1 2-1 */
  0x10, 0x08, 0x08, 0x08, 0x04, 0x04, 0x02, 0x02, /* 0x1F - Wave 1 2-2 */
  0x02, 0x02, 0x02, 0x02, 0x04, 0x04, 0x08, 0x08, /* 0x20 - Wave 1 2-3 */  
  
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, /* 0x21 - Wave 2 1-2 */
  0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, /* 0x22 - Wave 2 1-3 */  
  0x00, 0x21, 0x21, 0x42, 0x42, 0x84, 0x84, 0x84, /* 0x23 - Wave 2 2-1 */    
  0x84, 0x42, 0x42, 0x42, 0x21, 0x21, 0x10, 0x10, /* 0x24 - Wave 2 2-2 */      
  0x10, 0x10, 0x10, 0x10, 0x21, 0x21, 0x42, 0x42, /* 0x25 - Wave 2 2-3 */        
  
  0x00, 0x40, 0x40, 0x80, 0x80, 0x00, 0x00, 0x00, /* 0x26 - Wave 3 1-1 */
  0x00, 0x80, 0x80, 0x80, 0x40, 0x40, 0x20, 0x20, /* 0x27 - Wave 3 1-2 */  
  0x20, 0x20, 0x20, 0x20, 0x40, 0x40, 0x80, 0x80, /* 0x28 - Wave 3 1-3 */    
  0x00, 0x08, 0x08, 0x10, 0x10, 0x21, 0x21, 0x21, /* 0x29 - Wave 3 2-1 */      
  0x21, 0x10, 0x10, 0x10, 0x08, 0x08, 0x84, 0x84, /* 0x2A - Wave 3 2-2 */          
  0x84, 0x84, 0x84, 0x84, 0x08, 0x08, 0x10, 0x10, /* 0x2B - Wave 3 2-2 */            
  0x00, 0x01, 0x01, 0x02, 0x02, 0x04, 0x04, 0x04, /* 0x2C - Wave 3 3-1 */            
  0x04, 0x02, 0x02, 0x02, 0x01, 0x01, 0x00, 0x00, /* 0x2D - Wave 3 3-2 */            
  0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x02, 0x02  /* 0x2E - Wave 3 3-3 */              
};

#define gFanIconY_d 0
const uint8_t gFanLine1[] = {0x00, 0x00, 0x01, 0x02, 0x00};
const uint8_t gFanLine2[] = {0x00, 0x03, 0x04, 0x05, 0x00};
const uint8_t gFanLine3[] = {0x00, 0x00, 0x06, 0x07, 0x00};

#define gHeatStatusIconY_d 5
const uint8_t gThermostatOffLine1[] = {0x00, 0x00, 0x00, 0x00};
const uint8_t gThermostatOffLine2[] = {0x00, 0x00, 0x00, 0x00};
const uint8_t gThermostatOffLine3[] = {0x00, 0x00, 0x00, 0x00};

const uint8_t gThermostatHeatLine1[] = {0x15, 0x16, 0x17, 0x00};
const uint8_t gThermostatHeatLine2[] = {0x18, 0x19, 0x1A, 0x00};
const uint8_t gThermostatHeatLine3[] = {0x1B, 0x1C, 0x1D, 0x00};

const uint8_t gThermostatCoolLine1[] = {0x0C, 0x0D, 0x0E, 0x00};
const uint8_t gThermostatCoolLine2[] = {0x0F, 0x10, 0x11, 0x00};
const uint8_t gThermostatCoolLine3[] = {0x12, 0x13, 0x14, 0x00};

#define gFanStatusIconY_d 9
const uint8_t gWave1Line1[] = {0x00, 0x00, 0x00, 0x00};
const uint8_t gWave1Line2[] = {0x1E, 0x1F, 0x20, 0x00};
const uint8_t gWave1Line3[] = {0x00, 0x00, 0x00, 0x00};

const uint8_t gWave2Line1[] = {0x00, 0x21, 0x22, 0x00};
const uint8_t gWave2Line2[] = {0x23, 0x24, 0x25, 0x00};
const uint8_t gWave2Line3[] = {0x00, 0x00, 0x00, 0x00};

const uint8_t gWave3Line1[] = {0x26, 0x27, 0x28, 0x00};
const uint8_t gWave3Line2[] = {0x29, 0x2A, 0x2B, 0x00};
const uint8_t gWave3Line3[] = {0x2C, 0x2D, 0x2E, 0x00};

uint8_t gThermostatLine1[13];
uint8_t gThermostatLine2[13];
uint8_t gThermostatLine3[13];
#endif /* gHaThermostat_d */

#endif /* gLCDSupported_d == TRUE */

/******************************************************************************
*******************************************************************************
* Public memory declarations
*******************************************************************************
******************************************************************************/

/* Variables to keep track of the status of the Display on the AppMode or ConfigMode */
// ASL_DisplayStatus_t gAppModeDisplay;
// ASL_DisplayStatus_t gConfigModeDisplay;
// uint8_t gmUserInterfaceMode;

ASL_Data_t gAslData;
uint8_t gmUserInterfaceMode;

/******************************************************************************
*******************************************************************************
* Public functions
*******************************************************************************
******************************************************************************/

/******************************************************************************
* Initialize the user interface for the Application Support Library
******************************************************************************/
void ASL_InitUserInterface
  (
  char *pApplicationName
  )
{
  ASL_SendingNwkData_t mSendingNwkDataInit ={gZbAddrModeIndirect_c,{BeeKitGroupDefault_d},BeeKitSceneDefault_d,{0x00,0x00}};

  /* initialize the global variables */
  PermitJoinStatusFlag = 0xff;
  FLib_MemCpy(&gSendingNwkData,&mSendingNwkDataInit,sizeof(mSendingNwkDataInit));
  
  /* initialize LEDs if available (all boards) */
  LED_Init();

  /* initialize keyboard if available (all boards) */
#if (gMC1323xMatrixKBD_d == 1)
  KBD_Init(HandleKeysMC1323xMatrixKBD);
#else  
  KBD_Init(BeeAppHandleKeys);
#endif  

  /* initialize LCD if available (NCB only) */
  LCD_Init();

  /* initialize sound and beep if enabled (SRC & NCB only) */
  BuzzerInit();
  BuzzerBeep();

  /* register to receive ZDO and ZDP notifications */
  Zdp_AppRegisterCallBack (ASL_ZdoCallBack);

  /* initialize ZigBee Cluster Library (always after registering endpoints) */
  ASL_ZclInit();

  /* start out in config mode */
  ASL_ChangeUserInterfaceModeTo(gConfigureMode_c);

	/* indicate not on network */
  ASL_ConfigSetLed(LED1,gLedFlashing_c);

  /* write both node type and application name */
  gpszAslAppName = (uint8_t*)pApplicationName;
  ASL_DisplayAppName();
  ASL_DisplayLine2OnLCD();

  /* Get the statemachine for FA into start state. */
#if gFrequencyAgilityCapability_d && (!gEndDevCapability_d && !gComboDeviceCapability_d)
  /*
    The combo device should init the FA State Machine when it joins or forms the
    NWK when it gets to the Running State not in here.
  */
  FA_StateMachineInit();
#endif  
}

/******************************************************************************
* Write a string to the LCD line 1, but only for application mode.
******************************************************************************/
#if gLCDSupported_d
void ASL_LCDWriteString
  (
  char *pstr
  )
{
  if (gmUserInterfaceMode == gApplicationMode_c)
    LCD_WriteString(1, (uint8_t*) pstr);
  (void)pstr; /* remove compiler warning in case no LCD */
}
#endif

/*****************************************************************************
* ASL_TurnOffAllLeds
*
* Set LED to on/off or flashing
*****************************************************************************/
void ASL_ToggleLed
  (
  LED_t LEDNr  
  )
{
  LED_ToggleLed(LEDNr);
  (void) LEDNr;
}

/*****************************************************************************
* ASL_TurnOffAllLeds
*
* Set LED to on/off or flashing
*****************************************************************************/
void ASL_TurnOffAllLeds
  (
  void
  )
{
  /* turn off all LEDs, both in display and in the logical display */
  LED_SetLed(LED_ALL, gLedOff_c);
  if(gmUserInterfaceMode == gConfigureMode_c)
    gConfigModeDisplay.Leds = 0;
  else
    gAppModeDisplay.Leds = 0;
}

/*****************************************************************************
* ASL_SetLed
*
* Set LED to on/off or flashing
*****************************************************************************/
void ASL_SerialLeds
  (
  void
  )
{
  LED_StartSerialFlash();
}

/*****************************************************************************
* ASL_UserSetLed
*
* Set LED to on/off or flashing in the Application Mode
*****************************************************************************/
void ASL_AppSetLed
  (
  LED_t LEDNr,
  LedState_t state
  )
{
  if (gmUserInterfaceMode == gApplicationMode_c)
     LED_SetLed(LEDNr, state);

  ASL_ChangeLedsStateOnMode(LEDNr,state,gApplicationMode_c);
}

/*****************************************************************************/
void ASL_DisplayAppSettingsLeds(void)
{
  LED_SetLed(LED_FLASH_ALL, gLedStopFlashing_c);
  LED_SetLed(LED_ALL, gLedOff_c);
  LED_SetLed(gAppSettingDisplay.Leds, gLedOn_c); /**keep this order... on first, toggle second */
  LED_SetLed((uint8_t)(gAppSettingDisplay.Leds>>4), gLedFlashing_c );
}
/*****************************************************************************
* ASL_AppSettingsSetLed
*
* Set LED to on/off or flashing in the Application Mode or for Application Settings 
*****************************************************************************/
void ASL_AppSettingsSetLed
  (
  LED_t LEDNr,
  LedState_t state
  )
{
  if ((gmUserInterfaceMode == gApplicationMode_c)||
      (gmUserInterfaceMode == gAppSetting_c))
  {
    ASL_ChangeLedsStateOnMode(LEDNr,state,gAppSetting_c);
    ASL_DisplayAppSettingsLeds();
  }
}

/*****************************************************************************
* ASL_ChangeUserInterfaceModeTo
*****************************************************************************/
void ASL_ChangeUserInterfaceModeTo
  (
  UIMode_t DeviceMode
  )
{
    LED_StopFlashingAllLeds();
    gmUserInterfaceMode = DeviceMode;
#if gTargetMC1322xLPB   
    /* if target is MC1322x Low Power Node and low Power Mode is enabled,
     * restart Key Scan timer when the device is in configuration mode
     * so that SW2 can be used on the board and disable it in application mode
     * so that the node can sleep */

    if (1 == gBeeStackConfig.lpmStatus)
    {
      if (gConfigureMode_c == DeviceMode)
      {
        PWR_DisallowDeviceToSleep();
        KBD_EnableDisableKeyScan(TRUE);
      }
      else 
      {
        KBD_EnableDisableKeyScan(FALSE);        
        PWR_AllowDeviceToSleep();        
      }
    }
#endif      
}

/*****************************************************************************
* ASL_ChangeUserInterfaceModeTo
*****************************************************************************/
void ASL_DisplayChangeToCurrentMode
  (
  UIMode_t DeviceMode
  )
{
#if (gLEDSupported_d)
  ASL_DisplayStatus_t *pCurrentMode;
  uint8_t Leds;
#endif
  /* Clean the Display */
  LCD_ClearDisplay();
  LED_SetLed(LED_ALL,gLedOff_c);
  ASL_DisplayAppName();

  /* change to new mode on display and LEDs */
  if (DeviceMode == gApplicationMode_c) {
		ASL_DisplayLine2OnLCD();
#if (gLEDSupported_d)  
    pCurrentMode = &gAppModeDisplay;
#endif
   	}
  else{
    if (DeviceMode == gConfigureMode_c)
    {
		ASL_DisplayLine2OnLCD();
#if (gLEDSupported_d)		
	  pCurrentMode = &gConfigModeDisplay;
#endif
    }
#if (gLEDSupported_d)
    else
      pCurrentMode = &gAppSettingDisplay;
#endif
   	}
#if (gLEDSupported_d)
  Leds = pCurrentMode->Leds;
  if (Leds & LED1_FLASH) {
    LED_SetLed(LED1,gLedFlashing_c);
    }
  else {
    if (Leds & LED1)
      LED_SetLed(LED1,gLedOn_c);
    }
  if (Leds & LED2_FLASH) {
    LED_SetLed(LED2,gLedFlashing_c);
    }
  else {
    if (Leds & LED2)
      LED_SetLed(LED2,gLedOn_c);
    }
  if (Leds & LED3_FLASH) {
    LED_SetLed(LED3,gLedFlashing_c);
    }
  else {
    if (Leds & LED3)
      LED_SetLed(LED3,gLedOn_c);
    }
  if (Leds & LED4_FLASH) {
    LED_SetLed(LED4,gLedFlashing_c);
    }
  else {
    if (Leds & LED4)
      LED_SetLed(LED4,gLedOn_c);
    }
#endif    
}

/*****************************************************************************
* ASL_UpdateDevice
*
* Update the visual display on the device. Also updates the state (for example
* idle or running). Used by ZCL and by ASL to update the display.
*****************************************************************************/
void ASL_UpdateDevice
  (
  zbEndPoint_t ep,  /* IN: endpoint update happend on */
  SystemEvents_t event     /* IN: state to update */
  )
{
  (void)ep;

  /* handle the UI event (blink LED, etc...) */
  switch (event) {

    /* Formed/joined network */
    case gZDOToAppMgmtZCRunning_c:
    case gZDOToAppMgmtZRRunning_c:
    case gZDOToAppMgmtZEDRunning_c:
      appState = mStateZDO_device_running_c;
      ASL_ConfigSetLed(LED1,gLedStopFlashing_c);
      ASL_ConfigSetLed(LED1,gLedOn_c);
#if gZclEnableOTAClient_d       
#if gZclOTADiscoveryServerProcess_d     
#if gNum_EndPoints_c != 0   
      {
    	zclInitiateOtaProcess_t initServerDiscovery = {0, 0};
    	initServerDiscovery.isServer = FALSE;
    	initServerDiscovery.clientInit.endPoint = endPointList[0].pEndpointDesc->pSimpleDesc->endPoint;
		(void)OTA_InitiateOtaClusterProcess(&initServerDiscovery);
      }
#endif	//gNum_EndPoints_c
#endif  //gZclOTADiscoveryServerProcess_d     
#endif	//gZclEnableOTAClient_d	
#if gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d

#if gComboDeviceCapability_d
      if (NlmeGetRequest(gDevType_c) == gEndDevice_c)
      {
        break;
      }
#endif

      if (NlmeGetRequest(gNwkPermitJoiningFlag_c))
      {
        PermitJoinStatusFlag = PermitJoinOn;
         ASL_ConfigSetLed(PERMIT_JOIN_LED,gLedOn_c);
      }
      else
        PermitJoinStatusFlag = PermitJoinOff;
#endif
      LCD_WriteString(1,(uint8_t *)gsASL_Running);
      ASL_DisplayLine2OnLCD();
      break;

    /* Leave the Network event*/
    case gZDOToAppMgmtStopped_c:
    case gLeaveNetwork_c:
      if ((appState == mStateZDO_device_running_c ) ||(appState == mStateBindSuccess_c))
        ZdoStoppedAction();
      break;

    case gStartNetwork_c:
      ASL_SerialLeds();
      LCD_WriteString(1,(uint8_t *)gsASL_StartingNwk);
      break;
      
#if  (gZclEnableOTAProgressReport_d == TRUE)      
    case gOTAProgressReportEvent_c:
      switch(gOtaProgressReportStatus)
      {
      case otaStartProgress_c:  
        ASL_SetLed(LED2, gLedToggle_c);
        ASL_SetLed(LED3, gLedOff_c);
        ASL_SetLed(LED4, gLedOff_c);
        break;
      case otaProgress33_c:
        ASL_SetLed(LED2, gLedOn_c);
        ASL_SetLed(LED3, gLedToggle_c);
        ASL_SetLed(LED4, gLedOff_c);
        break;
      case otaProgress66_c:
        ASL_SetLed(LED2, gLedOn_c);
        ASL_SetLed(LED3, gLedOn_c);
        ASL_SetLed(LED4, gLedToggle_c);
        break;  
      case otaProgress100_c:
        ASL_SetLed(LED2, gLedOn_c);
        ASL_SetLed(LED3, gLedOn_c);
        ASL_SetLed(LED4, gLedOn_c);
        break;   
      }
      break;
#endif      
#ifndef SmartEnergyApplication_d
    /* Binding Request event */
    case gBind_Device_c:
      if (appState != mStateIdle_c ){	
	  	AppStartPolling();
      appState = mStateBindRequest_c;
      ASL_ConfigSetLed(BINDING_LED,gLedFlashing_c);
      LCD_WriteString(1,(uint8_t *)gsASL_Binding);
	    }
      break;

    /* Binding Success event */
    case gBindingSuccess_c:
      if (appState == mStateBindRequest_c){
		    AppStopPolling();
        appState = mStateBindSuccess_c;
        ASL_ConfigSetLed(BINDING_LED,gLedStopFlashing_c);
        ASL_ConfigSetLed(BINDING_LED,gLedOn_c);
        LCD_WriteString(1,(uint8_t *)gsASL_BindingSuccess);
        gSendingNwkData.gAddressMode = gZbAddrModeIndirect_c;
        gSendingNwkData.endPoint = 0x00;
      }
   	  break;

    /* Unbinding worked */
    case gUnBindingSuccess_c:
		  AppStopPolling();
      appState = mStateZDO_device_running_c;
      ASL_ConfigSetLed(BINDING_LED,gLedOff_c);
      /* "UnBinding success" */
      LCD_WriteString(1,(uint8_t *)gsASL_UnBindingSuccess);
      break;

    /* Binding Failure Event */
    case gBindingFailure_c:
      if (appState == mStateBindRequest_c){
	    AppStopPolling();
          appState = mStateZDO_device_running_c;
          ASL_ConfigSetLed(BINDING_LED,gLedStopFlashing_c);
          ASL_ConfigSetLed(BINDING_LED,gLedOff_c);
          /*"Binding fault" */
          LCD_WriteString(1,(uint8_t *)gsASL_BindingFail);
          }
      break;

    /* unbind failed */
    case gUnBindingFailure_c:
      appState = mStateZDO_device_running_c;
      ASL_ConfigSetLed(BINDING_LED,gLedOn_c);
      /* "UnBinding fault" */
      LCD_WriteString(1,(uint8_t *)gsASL_UnBindingFail);
      break;

#if gMatch_Desc_req_d
    case gMatch_Desc_req_c:
      if (appState != mStateIdle_c){
        appState = mStateMatchDescRequest_c;
        ASL_ConfigSetLed(BINDING_LED,gLedFlashing_c);
        LCD_WriteString(1,(uint8_t *)gsASL_Matching);
      }
      break;

    case gMatchDescriptorSuccess_c:
      if (appState == mStateMatchDescRequest_c){
        appState = mStateMatchDescSuccess_c;
        ASL_ConfigSetLed(BINDING_LED,gLedStopFlashing_c);
        ASL_ConfigSetLed(BINDING_LED,gLedOn_c);
        gSendingNwkData.gAddressMode = gZbAddrMode16Bit_c;
	      /* Match Found */
        LCD_WriteString(1,(uint8_t *)gsASL_MatchFound);
      }
   	  break;

    case gMatchNotFound_c:
      if (appState == mStateMatchDescRequest_c){
        appState = mStateZDO_device_running_c;
        ASL_ConfigSetLed(BINDING_LED,gLedStopFlashing_c);
        ASL_ConfigSetLed(BINDING_LED,gLedOff_c);
        /* Match Descriptor Not Found  */
        LCD_WriteString(1,(uint8_t *)gsASL_MatchNotFound);
      }
      break;

    case gMatchFailure_c:
      if (appState == mStateMatchDescRequest_c){
        appState = mStateZDO_device_running_c;
        ASL_ConfigSetLed(BINDING_LED,gLedStopFlashing_c);
        ASL_ConfigSetLed(BINDING_LED,gLedOff_c);
        /* Match Descriptor fault */
        LCD_WriteString(1,(uint8_t *)gsASL_MatchFail);
      }
      break;
#endif      
#endif /*!smart energy application*/
    /* permit join toggle */
    case gPermitJoinToggle_c:
#if gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d


#if gComboDeviceCapability_d
      if (NlmeGetRequest(gDevType_c) == gEndDevice_c)
      {
        break;
      }
#endif

      if (appState == mStateZDO_device_running_c ) {
        if(!PermitJoinStatusFlag) {
          LCD_WriteString( 1,(uint8_t *)gsASL_PermitJoinDisabled );
          ASL_ConfigSetLed(PERMIT_JOIN_LED,gLedOff_c);
        }
        else {
          LCD_WriteString( 1,(uint8_t *)gsASL_PermitJoinEnabled);
          ASL_ConfigSetLed(PERMIT_JOIN_LED,gLedOn_c);
        }
      }
#endif
      break;

    case gScanningChannels_c:
      if (appState == mStateIdle_c) {
        LCD_WriteString(1,(uint8_t *)gsASL_ChannelSelect);
        ASL_DisplayLine2OnLCD();
        LED_TurnOffAllLeds();
        LED_SetLed((gmAslChannelNumber - dLowestChannelVal), gLedOn_c);
      }
      break;

    /* Identify on */
    case gIdentifyOn_c:
      if (appState != mStateIdle_c ) {
        LCD_WriteString( 1,(uint8_t *)gsASL_IdentifyEnabled);
        ASL_SetLed(LED3,gLedFlashing_c);
      }
      break;

    /* Idenitfy off */
    case gIdentifyOff_c:
      if(appState != mStateIdle_c) {
        LCD_WriteString( 1,(uint8_t *)gsASL_IdentifyDisabled);
        ASL_SetLed(LED3,gLedStopFlashing_c);
        ASL_SetLed(LED3,gLedOff_c);
      }
      break;
#if gLCDSupported_d
#ifndef gHaCombinedInterface_d
#ifndef gHaOnOffLight_d
#ifndef SmartEnergyApplication_d
    case gSwitchTypeMomentary:
      if(appState != mStateIdle_c) {
        LCD_WriteString( 1,(uint8_t *)gsASL_SwitchTypeMomentary);
      }
      break;

    case gSwitchTypeToggle:
      if(appState != mStateIdle_c) {
        LCD_WriteString( 1,(uint8_t *)gsASL_SwitchTypeToggle);
      }
      break;

    case gSwitchActionOn:
      if(appState != mStateIdle_c) {
        LCD_WriteString( 1,(uint8_t *)gsASL_SwitchActionOn);
      }
      break;

    case gSwitchActionOff:
      if(appState != mStateIdle_c) {
        LCD_WriteString( 1,(uint8_t *)gsASL_SwitchActionOff);
      }
      break;

    case gSwitchActionToggle:
      if(appState != mStateIdle_c) {
        LCD_WriteString( 1,(uint8_t *)gsASL_SwitchActionToggle);
      }
      break;

    case gThermostatFahrenheit:
      if(appState != mStateIdle_c) {
        LCD_WriteString( 1,(uint8_t *)gsASL_DispModeFahrenheit);
      }
      break;

    case gThermostatCelsius:
      if(appState != mStateIdle_c) {
        LCD_WriteString( 1,(uint8_t *)gsASL_DispModeCelsius);
      }
      break;
#endif
#endif     
#endif      
#endif      
  } /* end of switch */
}

/*****************************************************************************
* ASL_HandleKeys
*
* Default key handling. Handles all config mode keys.
*****************************************************************************/
void ASL_HandleKeys
  (
  key_event_t events  /*IN: Events from keyboard modul */
  )
{
  uint16_t time;
#ifndef SmartEnergyApplication_d        
#if !defined(gBlackBoxApp_d) && !defined(gBlackBox_d) 
  #if !gMatchDescEnable_d && gEnd_Device_Bind_req_d
  zbNwkAddr_t  aDestAddress = {0x00,0x00};
  #endif
#endif  
#endif 
  //zbNwkAddr_t  aBroadcast = {0xfd,0xff};

  /* common application mode switches */
  if (gmUserInterfaceMode == gApplicationMode_c) {
    switch (events){
      case gKBD_EventSW3_c:
        /* identify Mode for 10 Seconds */
        if(!ZCL_IdentifyTimeLeft(appEndPoint))
          time = gIdentifyTimeSecs_d;
        else
          time = 0;
        ZCL_SetIdentifyMode(appEndPoint, Native2OTA16(time));
        break;
      case gKBD_EventLongSW1_c:
          /* Change the User Interface Mode to Configmode */
          /* Change the User Interface Display to the current  Mode */
        ASL_ChangeUserInterfaceModeTo(gConfigureMode_c);
        ASL_DisplayChangeToCurrentMode(gmUserInterfaceMode);
        break;
      case gKBD_EventSW4_c: /* Recalls the data for Scene*/
#if gASL_ZclRecallSceneReq_d      
#ifndef SmartEnergyApplication_d        
        ASL_LCDWriteString( "Recall Scene" );
        ASL_ZclRecallSceneUI(gSendingNwkData.aGroupId, gSendingNwkData.aSceneId);
#endif        
#endif        
        break;

      /* Sends an Add Group to the Devices by air */
      case gKBD_EventLongSW3_c:
#if gASL_ZclGroupAddGroupReq_d
#ifndef SmartEnergyApplication_d        
        ASL_LCDWriteString("Add Group" );
        ASL_SetLed(LED3,gLedOn_c);
        if (ASL_ZclAddGroupIfIdentifyUI(gASL_Zcl_InitState_c,gSendingNwkData.aGroupId)!=gZbSuccess_c)
          ASL_SetLed(LED3,gLedOff_c);
        else{
          gSendingNwkData.gAddressMode = gZbAddrModeGroup_c;
					gSendingNwkData.endPoint = 0xff;
					/* save app data into nvm */
          ZdoNwkMng_SaveToNvm(zdoNvmObject_ZclData_c);
          }
#endif        
#endif
       break;

      /* Set up the data for Scene to the SW4*/
      case gKBD_EventLongSW4_c:
#if gASL_ZclStoreSceneReq_d
#ifndef SmartEnergyApplication_d        
        ASL_LCDWriteString("Store Scene" );
        ASL_SetLed(LED4,gLedOn_c);
        if(ASL_ZclStoreSceneUI(gASL_Zcl_InitState_c, gSendingNwkData.aGroupId, gSendingNwkData.aSceneId) != gZbSuccess_c)
          ASL_SetLed(LED4,gLedOff_c);
#endif        
#endif        
       break;
      }
    }

  /* config mode configures the node, including joining the network */
  else {    
    switch ( events ) {

    /* SW1 joins with NVM. That is, after a reset, it will be back on the network */
    case gKBD_EventSW1_c:
      if (appState == mStateIdle_c) {
        ASL_UpdateDevice(appEndPoint,gStartNetwork_c);

        /* ZDO_Start:
        gStartAssociationRejoinWithNvm_c
        gStartOrphanRejoinWithNvm_c
        gStartNwkRejoinWithNvm_c
        gStartSilentRejoinWithNvm_c*/
        ZDO_Start(gStartSilentRejoinWithNvm_c);
      	}
      break;
    case PERMIT_JOIN_SW:        /* SW3 on MC1322x-LPN, SW2 on other boards */
      if (appState != mStateIdle_c) {
#if gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d

#if gComboDeviceCapability_d
      if (NlmeGetRequest(gDevType_c) == gEndDevice_c)
      {
        break;
      }
#endif

      if(PermitJoinStatusFlag)
        PermitJoinStatusFlag = PermitJoinOff;
      else
        PermitJoinStatusFlag = PermitJoinOn;
      ASL_UpdateDevice(DummyEndPoint,gPermitJoinToggle_c);
      APP_ZDP_PermitJoinRequest(PermitJoinStatusFlag);
#endif
      }
      break;

    case BINDING_SW:        /* SW2 on MC1322x-LPN, SW3 on other boards */
#ifndef SmartEnergyApplication_d 
#if !defined(gBlackBoxApp_d) && !defined(gBlackBox_d)     
      #if gMatchDescEnable_d
	  if (appState != mStateIdle_c){
        ASL_UpdateDevice(appEndPoint,gMatch_Desc_req_d);
	    ASL_MatchDescriptor_req( NULL, gaBroadcastRxOnIdle, AF_FindEndPointDesc(appEndPoint));
	  	}
	  #else
      if (appState != mStateIdle_c) {
        ASL_UpdateDevice(appEndPoint,gBind_Device_c);       
        ASL_EndDeviceBindRequest(NULL,aDestAddress,endPointList[0].pEndpointDesc->pSimpleDesc);
      }
      #endif
#endif      
#endif	  
      break;

    /* cfg.SW4 - Walk through all the channels (only when idle) */
    case gKBD_EventSW4_c:

      /* only allow selecting if not on network */
      if(appState == mStateIdle_c) {
        zbChannels_t aChannelMask = {0x00, 0x00, 0x00, 0x00};
        LED_SetLed(LED1,gLedOff_c);

        /* set the logical channel */
        if(!gmAslChannelNumber)
          gmAslChannelNumber = NlmeGetRequest(gNwkLogicalChannel_c);
        ++gmAslChannelNumber;
        if(gmAslChannelNumber < dLowestChannelVal || gmAslChannelNumber > dMaxChannelVal)
            gmAslChannelNumber = dLowestChannelVal;

        /* turn it into a bit mask */
        BeeUtilSetIndexedBit(aChannelMask, gmAslChannelNumber);
        ApsmeSetRequest(gApsChannelMask_c, aChannelMask);
        
        NlmeSetRequest(gNwkLogicalChannel_c, &gmAslChannelNumber);   
    
      }

      /* display current channel on LEDs (0x0=channel 11, 0x1=channel 12, 0xf=channel 26) */
      ASL_UpdateDevice(DummyEndPoint,gScanningChannels_c);
      break;

    case gKBD_EventLongSW1_c:
      ASL_ChangeUserInterfaceModeTo(gApplicationMode_c);
      ASL_DisplayChangeToCurrentMode(gmUserInterfaceMode);
      break;

    /* cfg.LSW2 - Leave the network */
    case gKBD_EventLongSW2_c:
      if (appState != mStateIdle_c){
          ASL_TurnOffAllLeds();
          ASL_ConfigSetLed(LED1, gLedFlashing_c);
	        ASL_DisplayAppName();
          ZDO_Leave();
          appState = mStateIdle_c;
  	  }
      break;

    /* cfg.LSW3 - clear binding table */
    case gKBD_EventLongSW3_c:
#ifndef SmartEnergyApplication_d      
	    if (appState == mStateBindSuccess_c){
  #if gBindCapability_d
                  APS_ClearBindingTable();
  #endif              
		    ASL_UpdateDevice(appEndPoint,gUnBindingSuccess_c);
	    }
	    else
	   	  ASL_UpdateDevice(appEndPoint,gUnBindingFailure_c);
#endif            
      break;

    /* start network without NVM */
    case gKBD_EventLongSW4_c:
      if (appState == mStateIdle_c){
	      ASL_UpdateDevice(appEndPoint,gStartNetwork_c);
        ZDO_Start(gStartWithOutNvm_c);
      }
      //LCD_WriteString(2,(uint8_t *)gsASL_ResetNode);
      break;
    } /* end of switch */

  } /* end of if */
}

/*****************************************************************************
* Common code used by both thermostat and temp sensor.
* Only linked in if Thermostat or temp sensor endpoint is used.
*****************************************************************************/
void ASL_DisplayTemperature
(
uint8_t TypeOfTemperature,
int16_t Temperature,
uint8_t displayModeFlag,
uint8_t HeatCoolUnitStatus
)
{
#if gLCDSupported_d 
  char aString[] = "DT=             ";
  int16_t NewTemp=0;
  uint8_t NewTempLength;
   
  if (TypeOfTemperature == gASL_LocalTemperature_c)
     aString[0] = 'L';     
  
  if (displayModeFlag == gZclDisplayMode_TempCelsius_c){
  	NewTemp = Temperature/100;          /* The Real temperatures is = Temperature/100 */
	  aString[6]='C';
	  NewTempLength=2;
  }
  else
  {
    NewTemp = ((9*Temperature)/500)+32; /* Converts the displayed temperature into Farenheit */
    aString[7]='F';
    NewTempLength=3;
	}
  
  if (Temperature < 0)
  {
		aString[3] = '-';
		ASL_Dec2Str(&aString[4],(-1*(NewTemp)), NewTempLength);
  }
  else
		ASL_Dec2Str(&aString[3],NewTemp, NewTempLength);

  switch (HeatCoolUnitStatus){
    case gASL_HCUHeatOn_c:
	    FLib_MemCpy(&aString[9],"HeatOn",sizeof("HeatOn"));
		break;
    case gASL_HCUCoolOn_c:
        FLib_MemCpy(&aString[9],"CoolOn",sizeof("CoolOn"));
		break;
  	}

  #if gTargetMC1322xNCB
    ASL_LCDWriteString(aString);
  #else
    LCD_WriteString(2, (uint8_t*) aString);
  #endif  
#endif


  /*Turn OFF all Led's in app mode*/
  	ASL_SetLed(LED2,gLedOff_c);
  	ASL_SetLed(LED3,gLedOff_c);
  	ASL_SetLed(LED4,gLedOff_c);


  if (Temperature <= Limit0)
  	ASL_SetLed(LED2,gLedFlashing_c);
                   
  if (Temperature >= Limit1)
  	ASL_SetLed(LED2, gLedOn_c);

  if (Temperature >= Limit2)
  	ASL_SetLed(LED3, gLedOn_c);
  
  if (Temperature >= Limit3)
  	ASL_SetLed(LED4, gLedOn_c);
  
  
  if (Temperature >= Limit4)
  	ASL_SetLed(LED4,gLedFlashing_c);
  

  (void) TypeOfTemperature;
  (void) displayModeFlag;
  (void) HeatCoolUnitStatus;
  
}
/******************************************************************************
 ASL_DisplayDutyCycle
*******************************************************************************/
void ASL_DisplayDutyCycle(uint8_t dutyCycle)
{
  
#if gLCDSupported_d 
  char pString[] = "DutyCycle=   %ON";
  ASL_Dec2Str(&pString[10], dutyCycle, 3);
#if gTargetMC1322xNCB  
  ASL_LCDWriteString(pString);
#else
  LCD_WriteString(2, (uint8_t*) pString);
#endif  
#else
  (void) dutyCycle;
#endif
  
 // ASL_DisplayFanSpeed(dutyCycle/25);
  
}
/******************************************************************************
 ASL_DisplayFanSpeed
*******************************************************************************/
void ASL_DisplayFanSpeed(uint8_t speed)
{
  uint8_t led = 1, i;
#if gLCDSupported_d 
  char pString[] = "Speed=    (   %)";  
  ASL_Dec2Str(&pString[7], speed, 1);
  ASL_Dec2Str(&pString[11], 25 * speed, 3);
#if gTargetMC1322xNCB
  ASL_LCDWriteString(pString);
#else
  LCD_WriteString(2, (uint8_t*) pString);
#endif  
#endif

  /* Turn OFF all Led's in app mode*/
  ASL_SetLed(LED1,gLedOff_c);
  ASL_SetLed(LED2,gLedOff_c);
  ASL_SetLed(LED3,gLedOff_c);
  ASL_SetLed(LED4,gLedOff_c);
  for(i = 0; i < speed; i++)
    ASL_SetLed(((led<<i)), gLedOn_c);
  
}
/******************************************************************************
*******************************************************************************
* Private functions
*******************************************************************************
******************************************************************************/

/*****************************************************************************
* Changes state of LEDs to state for this mode
*****************************************************************************/
void ASL_ChangeLedsStateOnMode
(
LED_t LEDNr,
LedState_t state,
UIMode_t mode
)
{
 ASL_DisplayStatus_t *pModeDisplay;

  if (mode == gApplicationMode_c)
    pModeDisplay = &gAppModeDisplay;
  else
    if (mode == gConfigureMode_c)
       pModeDisplay = &gConfigModeDisplay;
    else
       pModeDisplay = &gAppSettingDisplay;

/* set logical state of LEDs */
  if ((state == gLedOn_c) || (state == gLedFlashing_c)){
    pModeDisplay->Leds |= LEDNr;
    /* In case that the LED is set to Flashing mode, the Led variable of the Current Mode is set too
    0000 0001 => LED1 on, 0001 0001 => LED1 On/flashing */
    if (state == gLedFlashing_c)
      pModeDisplay->Leds |= (LEDNr << 4);
    }

  if ((state == gLedOff_c) || (state == gLedStopFlashing_c)){
    pModeDisplay->Leds  &= ~LEDNr;
	/* In case that the LED i was set to Flashing mode, then to turn of the Led variable of the Current Mode is set too
	     0000 0001 => LED1 on, 0001 0001 => LED1 On/flashing */
  if (state == gLedStopFlashing_c)
    pModeDisplay->Leds &= ~(LEDNr << 4);
  }
  
   if (state == gLedToggle_c)
   {
     if((pModeDisplay->Leds & LEDNr) == 0x00)
      pModeDisplay->Leds |= LEDNr;
     else
      pModeDisplay->Leds &= ~LEDNr ;
   }
  
}
/*****************************************************************************
* ASL_ConfigSetLed
*
* Set LED to on/off or flashing in the Config Mode
*****************************************************************************/
void ASL_ConfigSetLed
  (
  LED_t LEDNr,
  LedState_t state
  )
{
  /* set the LEDs for the proper mode */
  if (gmUserInterfaceMode == gConfigureMode_c)
    /* physically set the LEDs */
    LED_SetLed(LEDNr,state);

  ASL_ChangeLedsStateOnMode(LEDNr,state,gConfigureMode_c);
}

/*****************************************************************************
* zdoStopAction
*****************************************************************************/
void ZdoStoppedAction(void)
{
  LCD_WriteString(1,(void *)gsASL_LeaveNetwork);
  LCD_WriteString(2,"SW1 Restarts");
  appState = mStateIdle_c;

  ASL_ChangeUserInterfaceModeTo(gConfigureMode_c);
  ASL_DisplayLine2OnLCD();
  ASL_TurnOffAllLeds();
  ASL_ConfigSetLed(LED1,gLedFlashing_c);
}

#ifdef gHaThermostat_d
#if gHaThermostat_d
/*****************************************************************************
* Special case for thermostat: make a reverse binding than the one created
* by enddevice bind for the temperature measurement cluster  as reporting needs
* to happen from temp sensor to thermostat
*****************************************************************************/
#ifndef gHostApp_d
void CreateTempMeasurementBinding(void) 
{
  uint8_t i, j;
  uint8_t tempCluster[] = { gaZclClusterTemperatureSensor_c };
  apsBindingTable_Ptr_t  *pBindEntry;
  zbBindUnbindRequest_t bindRequest;
  zbAddressMap_t addrMap;
  uint8_t aNwkAddrLocalCpy[2];
  
  for (i = 0; i < gApsMaxBindingEntries; i++)
  {
    pBindEntry = (void *)&gpBindingTable[i];

    /* If the bind entry is not valid then don't process it */
    if (!pBindEntry->srcEndPoint)
    {
      continue;
    }
    
    for (j = 0; j < pBindEntry->iClusterCount; j++) {
      if (IsEqual2Bytes(pBindEntry->aClusterList[j], tempCluster)) {
         (void)AddrMap_GetTableEntry(pBindEntry->dstAddr.index, &addrMap);
         Copy2Bytes(bindRequest.aClusterId, tempCluster);
         Copy8Bytes(bindRequest.aSrcAddress, addrMap.aIeeeAddr);
         bindRequest.srcEndPoint = pBindEntry->dstEndPoint;
         bindRequest.addressMode = gZbAddrMode64Bit_c;
         Copy8Bytes(bindRequest.destData.extendedMode.aDstAddress, NlmeGetRequest(gNwkIeeeAddress_c));
         bindRequest.destData.extendedMode.dstEndPoint = pBindEntry->srcEndPoint;     
         
         APP_ZDP_BindUnbindRequest(NULL,  APS_GetNwkAddress(addrMap.aIeeeAddr,aNwkAddrLocalCpy), gBind_req_c, &bindRequest);        
         
         break;
      }
    }
  }
}
#else

extern zbBindUnbindRequest_t storedBindReq;
void CreateTempMeasurementBinding(void) 
{
  uint8_t tempCluster[] = { gaZclClusterTemperatureSensor_c };
  zbBindUnbindRequest_t bindRequest;
  uint8_t aWhereToCpyNwkAddr[2];

  Copy2Bytes(bindRequest.aClusterId, tempCluster);
  Copy8Bytes(bindRequest.aSrcAddress, storedBindReq.destData.extendedMode.aDstAddress);
  bindRequest.srcEndPoint = storedBindReq.destData.extendedMode.dstEndPoint;
  bindRequest.addressMode = gZbAddrMode64Bit_c;
  Copy8Bytes(bindRequest.destData.extendedMode.aDstAddress, NlmeGetRequest(gNwkIeeeAddress_c));
  bindRequest.destData.extendedMode.dstEndPoint = storedBindReq.srcEndPoint;     

  APP_ZDP_BindUnbindRequest(NULL, 
                            APS_GetNwkAddress(storedBindReq.destData.extendedMode.aDstAddress, aWhereToCpyNwkAddr), 
                            gBind_req_c,
                            &bindRequest);        


}

#endif /* gHostApp_d */

#endif
#endif




/******************************************************************************
*******************************************************************************
* Private Debug stuff
*******************************************************************************
******************************************************************************/
#if gLCDSupported_d

/*******************************************************************************
* Converts from a hex number into a 4 digit string (does not terminate).
*******************************************************************************/
void ASL_Hex2Str
  (
  char *pStr,
  uint16_t hexValue
  )
{
  index_t digits;

  digits = 4;
  while(digits)
  {
    --digits;
    pStr[digits] = gaHexValue[hexValue & 0xf];
    hexValue = hexValue >> 4;
  }
}

/*******************************************************************************
* Converts from decimal number into a string (does not terminate)
*******************************************************************************/
void ASL_Dec2Str
  (
  char *pStr,
  uint16_t decValue,
  index_t digits
  )
{
  if (digits == 0)
    digits = 2;

  while(digits)
  {
    --digits;
    pStr[digits] = gaHexValue[decValue % 10];
    decValue /= 10;
  }
}

/********************************************************************************
  ASL_DisplayLine2OnLCD

  This function will display the common line2 on the LCD screen (for those
  nodes with LCDs). Line2 includes Keyboard mode (Config mode or Application
  mode), PAN ID (may be 0xffff if not yet on a PAN), Channel, and Nwk (short)
  address, or ---- if not yet on a PAN.

  0123456789012345
  Cfg 1AAA 26 ----
  App 1AAA 11 0001

********************************************************************************/
void ASL_DisplayLine2OnLCD
  (
  void
  )
{
  char aString[17];
  zbNwkAddr_t aNwkAddr;
  zbPanId_t aPanId;
  uint8_t channel;

  /* start with all spaces */
  FLib_MemSet(aString, ' ', sizeof(aString));
  aString[sizeof(aString)-1] = 0;

  /* current mode */
  FLib_MemCpy(aString, gmUserInterfaceMode == gConfigureMode_c ?
    (void *)gszAslCfgMode : (void *)gszAslAppMode, sizeof(gszAslCfgMode)-1);

  /* copy in PAN ID */
  Copy2Bytes(aPanId, NlmeGetRequest(gNwkPanId_c));
  ASL_Hex2Str(&aString[4], OTA2Native16(*((uint16_t *)aPanId)));

  /* copy in channel, first look in NIB */
  /* then in our button-pressed channel */
  /* then convert hard-coded channel */
  channel = gmAslChannelNumber;
  if(!channel)
    channel = NlmeGetRequest(gNwkLogicalChannel_c);
  if(!channel)
    channel = BeeUtilBitToIndex(ApsmeGetRequest(gApsChannelMask_c), 4);
  ASL_Dec2Str(&aString[9], channel, 2);

  /* copy in NWK address */
  Copy2Bytes(aNwkAddr, NlmeGetRequest(gNwkShortAddress_c));
  if(TwoBytesToUint16(aNwkAddr) == 0xffff)
    FLib_MemCpy(&aString[12], (void *)gszAslNoNwkAddr, sizeof(gszAslNoNwkAddr)-1);
  else
    ASL_Hex2Str(&aString[12], OTA2Native16(*((uint16_t *)aNwkAddr)));

  /* write it to line 2 on display */
  LCD_WriteString(2, (uint8_t*)aString);
}

/********************************************************************************
  ASL_DisplayAppName

  Display application name.
********************************************************************************/
void ASL_DisplayAppName
  (
  void
  )
{
    LCD_WriteString(1, gpszAslAppName);
}



#ifdef gHaDimmableLight_d
#if gHaDimmableLight_d

#if gTargetMC1322xNCB && gLCDSupported_d

/********************************************************************************
  ASL_DisplayLevelBarMC1322xNCB

  Display the level bar
********************************************************************************/
void ASL_DisplayLevelBarMC1322xNCB
  (
    zclLevelValue_t Level,
    zclCmd_t OnOffStatus
  )
{
    uint16_t percent;
    uint16_t displayUnits;
    index_t digits = 2;
    extern uint16_t gZclLevel_diference;
    
    (void) OnOffStatus;
    

    gProgressBarIndex[0] = 0;
    FLib_MemSet(&gProgressBarIndex[1], 0x02, 10);
    gProgressBarIndex[1] = 0x01;
    gProgressBarIndex[10] = 0x03;
    
    if (Level == gZclLevelMaxValue)
    {
      percent = 100;
      digits = 3;
    }
    else
      percent = (uint16_t)(Level * 0.3937);
    if (percent < 10)
      digits = 1;
    displayUnits = percent / 10;
    FLib_MemSet(&gProgressBarIndex[1], 0x04, (uint8_t)displayUnits);      
    FLib_MemSet(&gLevelLcdString[8], ' ', 5);
    ASL_Dec2Str(&gLevelLcdString[8], percent, digits);      
    gLevelLcdString[8 + digits] = '%';      
     
    LCD_WriteBitmap(gProgressBarIndex, 11, 4, (unsigned char*)gProgressBarBitmapPixels);    
   
}


/********************************************************************************
  ASL_DisplayLevelPercentageMC1322xNCB

  Display the level percentage
********************************************************************************/
void ASL_DisplayLevelPercentageMC1322xNCB
  (
    zclLevelValue_t Level,
    zclCmd_t OnOffStatus
  )
{
    uint16_t percent;
    uint16_t displayUnits;
    index_t digits = 2;    

    gProgressBarIndex[0] = 0;
    FLib_MemSet(&gProgressBarIndex[1], 0x02, 10);
    gProgressBarIndex[1] = 0x01;
    gProgressBarIndex[10] = 0x03;
    
    if (OnOffStatus == gZclCmdOnOff_Off_c) 
    {
      ASL_LCDWriteString("Light Off");
    }
    else
    {
      if (Level == gZclLevelMaxValue)
      {
        percent = 100;
        digits = 3;
      }
      else
        percent = (uint16_t)(Level * 0.3937);
      if (percent < 10)
        digits = 1;
      displayUnits = percent / 10;
      FLib_MemSet(&gProgressBarIndex[1], 0x04, (uint8_t)displayUnits);      
      FLib_MemSet(&gLevelLcdString[8], ' ', 5);
      ASL_Dec2Str(&gLevelLcdString[8], percent, digits);      
      gLevelLcdString[8 + digits] = '%';      
      
      ASL_LCDWriteString(gLevelLcdString);
    }
       
}


#endif

#endif /* #if  gHaDimmableLight_d */ 
#endif /* #ifdef gHaDimmableLight_d */ 

#ifdef gHaThermostat_d
#if gHaThermostat_d

#if gTargetMC1322xNCB
void ASL_DisplayThermostatStatusMC1322xNCB
  (
    uint8_t heatStatus,
    uint8_t fanStatus
  )
{ 
  FLib_MemCpy(gThermostatLine1, (void *)gFanLine1, sizeof(gFanLine1));
  FLib_MemCpy(gThermostatLine2, (void *)gFanLine2, sizeof(gFanLine2));
  FLib_MemCpy(gThermostatLine3, (void *)gFanLine3, sizeof(gFanLine3));  
  if (heatStatus == gASL_HCUOff_c)
  {
    FLib_MemCpy(&gThermostatLine1[gHeatStatusIconY_d], (void *)gThermostatOffLine1, sizeof(gThermostatOffLine1));
    FLib_MemCpy(&gThermostatLine2[gHeatStatusIconY_d], (void *)gThermostatOffLine2, sizeof(gThermostatOffLine2));
    FLib_MemCpy(&gThermostatLine3[gHeatStatusIconY_d], (void *)gThermostatOffLine3, sizeof(gThermostatOffLine3));    
  }
  else if (heatStatus == gASL_HCUHeatOn_c)
  {
    FLib_MemCpy(&gThermostatLine1[gHeatStatusIconY_d], (void *)gThermostatHeatLine1, sizeof(gThermostatHeatLine1));
    FLib_MemCpy(&gThermostatLine2[gHeatStatusIconY_d], (void *)gThermostatHeatLine2, sizeof(gThermostatHeatLine2));
    FLib_MemCpy(&gThermostatLine3[gHeatStatusIconY_d], (void *)gThermostatHeatLine3, sizeof(gThermostatHeatLine3));
  }
  else if (heatStatus == gASL_HCUCoolOn_c)
  {
    FLib_MemCpy(&gThermostatLine1[gHeatStatusIconY_d], (void *)gThermostatCoolLine1, sizeof(gThermostatCoolLine1));
    FLib_MemCpy(&gThermostatLine2[gHeatStatusIconY_d], (void *)gThermostatCoolLine2, sizeof(gThermostatCoolLine2));
    FLib_MemCpy(&gThermostatLine3[gHeatStatusIconY_d], (void *)gThermostatCoolLine3, sizeof(gThermostatCoolLine3));
  }  
  
  if (fanStatus == gZcl_FanMode_Off_c)
  {
    FLib_MemCpy(&gThermostatLine1[gFanStatusIconY_d], (void *)gThermostatOffLine1, sizeof(gThermostatOffLine1));
    FLib_MemCpy(&gThermostatLine2[gFanStatusIconY_d], (void *)gThermostatOffLine2, sizeof(gThermostatOffLine2));
    FLib_MemCpy(&gThermostatLine3[gFanStatusIconY_d], (void *)gThermostatOffLine3, sizeof(gThermostatOffLine3));    
  }

  else if (fanStatus == gZcl_FanMode_Low_c)
  {
    FLib_MemCpy(&gThermostatLine1[gFanStatusIconY_d], (void *)gWave1Line1, sizeof(gWave1Line1));
    FLib_MemCpy(&gThermostatLine2[gFanStatusIconY_d], (void *)gWave1Line2, sizeof(gWave1Line2));
    FLib_MemCpy(&gThermostatLine3[gFanStatusIconY_d], (void *)gWave1Line3, sizeof(gWave1Line3));        
  }
  else if (fanStatus == gZcl_FanMode_Medium_c)
  {
    FLib_MemCpy(&gThermostatLine1[gFanStatusIconY_d], (void *)gWave2Line1, sizeof(gWave2Line1));
    FLib_MemCpy(&gThermostatLine2[gFanStatusIconY_d], (void *)gWave2Line2, sizeof(gWave2Line2));
    FLib_MemCpy(&gThermostatLine3[gFanStatusIconY_d], (void *)gWave2Line3, sizeof(gWave2Line3));        
  }
  else
  {
    FLib_MemCpy(&gThermostatLine1[gFanStatusIconY_d], (void *)gWave3Line1, sizeof(gWave3Line1));
    FLib_MemCpy(&gThermostatLine2[gFanStatusIconY_d], (void *)gWave3Line2, sizeof(gWave3Line2));
    FLib_MemCpy(&gThermostatLine3[gFanStatusIconY_d], (void *)gWave3Line3, sizeof(gWave3Line3));        
  }  
  
  LCD_WriteBitmap(gThermostatLine1, sizeof(gThermostatLine1), 4, (unsigned char*)gFanIconBitmapPixels);      
  LCD_WriteBitmap(gThermostatLine2, sizeof(gThermostatLine2), 5, (unsigned char*)gFanIconBitmapPixels);      
  LCD_WriteBitmap(gThermostatLine3, sizeof(gThermostatLine3), 6, (unsigned char*)gFanIconBitmapPixels);        
}

#endif
#endif /* #if gHaThermostat_d  */
#endif /* #ifdef gHaThermostat_d  */


/*****************************************************************************
  ASL_PrintUTCTime

  Converts UTC Time in HH:MM:SS
*****************************************************************************/
void ASL_PrintUTCTime
  (
  void
  )
{
   uint32_t time = OTA2Native32(gTimeAttrsData.LocalTime);
   uint32_t val;
   uint32_t daySec;

   char aStr[14] = "Time= ";
      
   daySec = time % mSecDay_c;
   val = daySec / 3600;
   ASL_Dec2Str(&aStr[6], (uint16_t)val, 2);
   aStr[8]=':';
   val = (daySec % 3600) / 60;
   ASL_Dec2Str(&aStr[9], (uint16_t)val, 2);
   aStr[11]=':';
   val = daySec % 60;
   ASL_Dec2Str(&aStr[12], (uint16_t)val, 2);
   
   #if gTargetMC1322xNCB
   LCD_WriteString(7, (uint8_t *)&aStr[0]);
   #elif gTargetQE128EVB_d || gTargetMC1323xRCM_d
   LCD_WriteString(2, (uint8_t *)&aStr[0]);
   #endif


}


/*****************************************************************************
* ASL_PrintEvent
*
* Print event strings (e.g. msg rcvd, cancel cnf, etc)
*
*****************************************************************************/

void ASL_PrintEvent(char *eventStr)
{  
  
#if gTargetMC1322xNCB
  LCD_WriteString(2, (uint8_t*) eventStr);
#elif gTargetQE128EVB_d || gTargetMC1323xRCM_d
  LCD_WriteString(1, eventStr);
#else
  (void) eventStr;  
#endif

}


#ifdef SmartEnergyApplication_d

void ASL_PrintMessage(uint8_t *aStr) 
{
#if gTargetMC1322xNCB
  uint8_t i, line = mStartLine;
  
  for(i=0; i<gMaxRcvdMsgToDisplay; i+= gMAX_LCD_CHARS_c)
  {
    LCD_WriteString(line++, aStr);
    aStr+= gMAX_LCD_CHARS_c;
  }
#elif gTargetQE128EVB_d || gTargetMC1323xRCM_d
  LCD_WriteString(2, aStr);
#else
(void) aStr;  
#endif
}


void ASL_PrintField(uint8_t fieldNr, void *pData)
{
  const uint8_t *pUnit[10]={"kW","m^3","ft3","ccf","US gl","IMP gl","BTUs","L","kPA(G)","kPA(A)" };
  uint8_t aStrEvent[16];
  uint32_t priceVal;
  uint8_t pointPlace, printDigit, i;
  uint16_t duration;
  zclCmdPrice_PublishPriceRsp_t  *pPrice = (zclCmdPrice_PublishPriceRsp_t*) pData;
  
  FLib_MemCpy(aStrEvent, "                ", 16);
    
  
  switch (fieldNr)  
  {
  case gASL_FieldPriceTier_c: 
  {    
    /* Print Price Tier number */
    FLib_MemCpy(aStrEvent, (uint8_t*)"PriceTier", 9);
    ASL_Dec2Str((char*)(aStrEvent+10), (pPrice->PriceTrailingDigitAndPriceTier & 0x0F), 1);
  }
  break;

  case gASL_FieldLabel_c: 
  {
    /* Print the Label */
    FLib_MemCpy(aStrEvent, (uint8_t*) "Label=", 6);    
    for(i= pPrice->RateLabel.length + 6; i < gMAX_LCD_CHARS_c; i++)
      *(aStrEvent+i)=' ';
    if(pPrice->RateLabel.length + 6 < gMAX_LCD_CHARS_c)
      FLib_MemCpy((aStrEvent+6), &pPrice->RateLabel.aStr[0], pPrice->RateLabel.length);
    else
      FLib_MemCpy((aStrEvent+6), &pPrice->RateLabel.aStr[0], gMAX_LCD_CHARS_c);
  }    
  break;
        
  case gASL_FieldUom_c: 
  {        
    /* Print the unit Of Measure*/
    FLib_MemCpy(aStrEvent, (uint8_t *)"UM=", 3);
    FLib_MemCpy((aStrEvent+3), (uint8_t*) pUnit[(pPrice->UnitOfMeasure&0x0F)], sizeof(*pUnit[(pPrice->UnitOfMeasure&0x0F)]));
    if((pPrice->UnitOfMeasure&0x0F) < 0x0A)
      if(pPrice->UnitOfMeasure & 0x80)
        FLib_MemCpy((aStrEvent+9), (uint8_t*)"BCD", 3);
      else
        FLib_MemCpy((aStrEvent+9), (uint8_t*)"BIN", 3);
    else
      FLib_MemCpy((aStrEvent+3), (uint8_t*) "NotDefined", 10);
  }
  break;
    
  case gASL_FieldPrice_c: 
  {    
    /* Print the Price */
    FLib_MemCpy(aStrEvent, (uint8_t*)"Price=", 6);
    priceVal = FourBytesToUint32(pPrice->Price);
    priceVal = OTA2Native32(priceVal);
    pointPlace = (pPrice->PriceTrailingDigitAndPriceTier >> 4);
    printDigit = 15;
    do
    {      
      if(pointPlace == 15 - printDigit )
      {
        *(aStrEvent+printDigit) = '.';
        --printDigit;
      }
      
      *(aStrEvent+printDigit)= (uint8_t)(priceVal%10)+'0';
      --printDigit;
      priceVal = priceVal / 10;
      
    }
    while((priceVal != 0) ||  (pointPlace >= 15 - printDigit));
  }
  break;
      
  case gASL_FieldDuration_c: 
  {    
    /* Print the duration */
    FLib_MemCpy(aStrEvent, (uint8_t*)"Duration=", 9);
    duration = OTA2Native16(pPrice->DurationInMinutes);
    ASL_Dec2Str((char*)(aStrEvent+ 10), duration, 4);
  }
  break;

#if gTargetQE128EVB_d || gTargetMC1323xRCM_d
#ifdef gSeInPremiseDisplay_d

  case gASL_FieldMessage_c: 
  {
    /* Display message on QE128 and exit */
    ASL_PrintMessage((uint8_t*) pData);
    return;
  }
  break;
  
  case gASL_FieldTime_c:
  {
    /* Display UTC time on QE128 board */
    ASL_PrintUTCTime();
    return;
  }
  break;
#endif      
#endif /* gTargetQE128EVB || gTargetMC1323xRCM_d*/  

  } /* switch */
          
  /* Print the field */
  #if gTargetMC1322xNCB
  LCD_WriteString(5, (uint8_t*)&aStrEvent[0]);
  #elif gTargetQE128EVB_d || gTargetMC1323xRCM_d
  LCD_WriteString(2, (uint8_t*)&aStrEvent[0]);  
  #endif
}

/*****************************************************************************/
void ASL_PrintPriceEvtStatus(uint8_t status) 
{
#if mPrintStringEvStatus_c  
  char *pMsg;
  
  /* Print the Status of the Price event on line 6 */
  switch(status)
  {
  case gPriceReceivedStatus_c:
    pMsg = "PriceReceived   ";
    break;
  case gPriceStartedStatus_c: 
    pMsg = "PriceStarted    ";
    break;
  case gPriceUpdateStatus_c:
    pMsg = "PriceUpdated    ";
    break;
  case gPriceCompletedStatus_c:
    pMsg = "PriceCompleted  ";           
    break;		
  default:
    {
      pMsg = "NoCurrentPrice  ";
    }
    break;
  }
  
  /* Print Status */
  #if gTargetMC1322xNCB
    LCD_WriteString(6, (uint8_t*) pMsg);
  #endif
  #if gTargetQE128EVB_d || gTargetMC1323xRCM_d
    ASL_PrintEvent(pMsg);
  #endif

#else /* mPrintStringEvStatus_c == FALSE */


  uint8_t aStrStatus[] = "Status=         ";
  ASL_Hex2Str((char *)(aStrStatus+7), (uint16_t)status);
  LCD_WriteString(6, &aStrStatus[0]);  

#endif /* #if mPrintStringEvStatus_c */

}




/*****************************************************************************/


void ASL_PrintLdCtrlEvtStatus(uint8_t status) 
{
  
#if mPrintStringEvStatus_c
  uint8_t *pStr = "";
  
  switch(status)
  {
  case gSELCDR_LdCtrlEvtCode_CmdRcvd_c:
    pStr = "Received        ";
    break;
  case gSELCDR_LdCtrlEvtCode_Started_c:
    pStr = "Started         ";
    break;
  case gSELCDR_LdCtrlEvtCode_UserHaveToChooseOptOut_c:
    pStr = "HaveOptOut      ";
    break;
  case gSELCDR_LdCtrlEvtCode_UserHaveToChooseOptIn_c:
    pStr = "HaveOptIn       ";
    break;
  case gSELCDR_LdCtrlEvtCode_EvtCancelled_c:
    pStr = "Cancelled       ";
    break;
  case gSELCDR_LdCtrlEvtCode_EvtSuperseded_c:
    pStr = "Superseded      ";
    break;
  case gSELCDR_LdCtrlEvtCode_Completed_c:
    pStr = "Completed       ";
    break;
  case gSELCDR_LdCtrlEvtCode_EvtPrtlCompletedWithUserOptOut_c:
    pStr = "CompletedOptOut ";
    break;
  case gSELCDR_LdCtrlEvtCode_EvtPrtlCompletedWithUserOptIn_c:
    pStr = "CompletedOptIn  ";
    break;
  case gSELCDR_LdCtrlEvtCode_EvtCompletedWithNoUser_c:
    pStr = "CompletedNoUser ";    
    break;
  default:
    pStr = "LDCtrlNotDefined";
    break;
    
  }
  
  #if gTargetMC1322xNCB
  LCD_WriteString(6, &pStr[0]);
  #else
  LCD_WriteString(1, &pStr[0]);  
  #endif
  
  
#else
  uint8_t aStrStatus[]="Status=         "; 
  ASL_Hex2Str((char *)(aStrStatus+7), (uint16_t)status);
  #if gTargetMC1322xNCB
  LCD_WriteString(6, &aStrStatus[0]);
  #else
  LCD_WriteString(1, &aStrStatus[0]);
  #endif
#endif /* #if mPrintStringEvStatus_c */
  
}

const uint8_t aStrEv[]= "Ev    Min    %ON";
const uint8_t aStrHeatSP[]= "HeatingSP=     C";
const uint8_t aStrCoolSP[]= "CoolingSP=     C";
const uint8_t aStrHeatOFS[]= "HeatingOFS=    C";
const uint8_t aStrCoolOFS[]=  "CoolingOFS=    C";

/*****************************************************************************/
void ASL_PrintLdCtrlEvt(uint8_t fieldNr, zclCmdDmndRspLdCtrl_LdCtrlEvtReq_t *pEvt) 
{
  uint16_t val;
  uint8_t aStrEvent[16];

  
  /* Print duration and duty cycle */
  if(0 == fieldNr)
  {
    FLib_MemCpy(aStrEvent, (uint8_t*)&aStrEv[0], 16);
    val= OTA2Native16(pEvt->DurationInMinutes);
    ASL_Dec2Str((char*)(aStrEvent+3), val, 3);
    ASL_Dec2Str((char*)(aStrEvent+10), pEvt->DutyCycle, 3);
  }
  else
    if(1 == fieldNr)
    {
      /* Print Heating Temperature set point */
      FLib_MemCpy(aStrEvent, (uint8_t*)&aStrHeatSP[0], 16);
      
      val= OTA2Native16(pEvt->HeatingTempSetPoint)/ 100;
      if ((int16_t)val < 0)
  	FLib_MemCpy((aStrEvent+11), "-", 1);
      ASL_Dec2Str((char*)(aStrEvent+12), val, 3);
    }
    else
      if(2 == fieldNr)
      {
        /* Print Cooling Temperature set point */
        FLib_MemCpy(aStrEvent, (uint8_t*)&aStrCoolSP[0], 16);
        val = OTA2Native16(pEvt->CoolingTempSetPoint)/ 100;
        if ((int16_t)val < 0)
          FLib_MemCpy((aStrEvent+11), "-", 1);
        ASL_Dec2Str((char*)(aStrEvent+12), val, 3);
      }
      else
        if(3 == fieldNr)
        {
          /* Print Heating Temperature offset */
          FLib_MemCpy(aStrEvent,(uint8_t*)&aStrHeatOFS[0], 16);
          val = pEvt->HeatingTempOffset/10;
          ASL_Dec2Str((char*)(aStrEvent+12), val, 2);
        }
        else
          if(4 == fieldNr)
          {
            /* Print Cooling Temperature offset */
            FLib_MemCpy(aStrEvent, (uint8_t*)&aStrCoolOFS[0], 16);
            val = pEvt->CoolingTempOffset/10;
            ASL_Dec2Str((char*)(aStrEvent+12), val, 2);
          }
  
  #if gTargetMC1322xNCB
  LCD_WriteString(5, (uint8_t*)&aStrEvent[0]);
  #else
  LCD_WriteString(2, (uint8_t*)&aStrEvent[0]);
  #endif
  
}
#endif /* #ifndef SmartEnergyApplication_d */

#endif /* gLCDSupported_d */


#if (gMC1323xMatrixKBD_d == 1)
void HandleKeysMC1323xMatrixKBD
( 
  uint8_t events, 
  uint8_t pressedKey 
)
{
  if (events == gKBD_EventLong_c)
    pressedKey +=  gKBD_EventLongSW1_c - gKBD_EventSW1_c;
  BeeAppHandleKeys(pressedKey);
}
#endif
