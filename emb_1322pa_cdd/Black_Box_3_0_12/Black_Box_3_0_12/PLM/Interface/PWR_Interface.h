/************************************************************************************
* Function to handle/set the different power down states.
*
*
* (c) Copyright 2006, Freescale Semiconductor, Inc. All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
***********************************************************************************/
#ifndef _PWR_INTERFACE_H_
#define _PWR_INTERFACE_H_

/************************************************************************************
*************************************************************************************
* Includes and external declarations
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Public Macros
*************************************************************************************
************************************************************************************/

#define gAllowDeviceToSleep_c 0


/******************************************************************************
* Description : Macros to get the current status of the STACK
* Assumptions : None
* Inputs      : None
* Output      : Status from PWRLib_StackPS
* Errors      : Not handled
******************************************************************************/
#define PWRLib_GetCurrentZigbeeStackPowerState      PWRLib_StackPS


/******************************************************************************
* Description : Macro to set the status of the STACK
* Inputs      : NewStackPS  : New state
*                 StackPS_Running   : Busy
*                 StackPS_Sleep     : Sleep allowed. Duration is appl. specific
*                 StackPS_DeepSleep : DeepSleep allowed
* Output      : None
* Errors      : Not handled
******************************************************************************/
#define PWRLib_SetCurrentZigbeeStackPowerState( NewStackPS)  PWRLib_StackPS = NewStackPS;


#if (cPWR_UseMCUStatus)
  #define mMCU_SetStatus(x)     PWRLib_MCUStatus = x;
#else
  #define mMCU_SetStatus(x)     ;
#endif  // #if (cPWR_UseMCUStatus)

//--- Definitions for PWRLib_RADIOStatus writing                          ---//
#if (cPWR_UseRADIOStatus)
  #define mRADIO_SetStatus( x)     PWRLib_RADIOStatus = x;
  #define mRADIO_GetStatus         PWRLib_RADIOStatus
#else
  #define mRADIO_SetStatus( x)     ;
  #define mRADIO_GetStatus
#endif  // #if (cPWR_UseRADIOStatus)

/************************************************************************************
*************************************************************************************
* Public types
*************************************************************************************
************************************************************************************/
//---------------------------------------------------------------------------//
/*--- Parameter type for PWR_CheckForAndEnterNewPowerState(..) call       ---*/
typedef enum {PWR_Run = 77, PWR_Sleep, PWR_DeepSleep, PWR_Reset, PWR_OFF} PWR_CheckForAndEnterNewPowerState_t;

typedef enum {MCU_Running=66, MCU_Wait,   MCU_Hibernate,   MCU_Doze}  PWRLib_MCUStatus_t;
typedef enum {RADIO_Idle=88,  RADIO_Off} PWRLib_RADIOStatus_t;

//---------------------------------------------------------------------------//
//--- Zigbee stack power state                                            ---//
typedef enum {StackPS_Running=122, StackPS_Sleep, StackPS_DeepSleep}      PWRLib_StackPS_t;


//---------------------------------------------------------------------------//
//--- Wakeup reason UNION definitions                                     ---//
typedef  union {
  uint8_t AllBits;
  struct {
    uint8_t   FromHibernate   :1;                // Comming from Hibernate
    uint8_t   FromDoze        :1;                // Comming from Doze
    uint8_t   FromWait        :1;                // Comming from Wait
    uint8_t   FromReset       :1;                // Comming from Reset
    uint8_t   FromKBI         :1;                // Wakeup by KBI interrupt
    uint8_t   FromTimer       :1;                // Wakeup by Wakeup-timer
    uint8_t   LVD_Updated     :1;                
  } Bits;
} PWRLib_WakeupReason_t;

//---------------------------------------------------------------------------//
//--- Reported voltage levels. Level 4 is highest (best)                  ---//
//---        Highest ----------------------------- Lowest voltage         ---//
typedef enum
{ 
  PWR_NODEPOWER_LEVEL_100      = 0x0c, 
  PWR_NODEPOWER_LEVEL_66       = 0x08,
  PWR_NODEPOWER_LEVEL_50,
  PWR_NODEPOWER_LEVEL_33       = 0x04,
  PWR_NODEPOWER_LEVEL_CRITICAL = 0x00
} PWRLib_LVD_VoltageLevel_t;

/************************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
************************************************************************************/
extern volatile PWRLib_WakeupReason_t     PWRLib_MCU_WakeupReason;
extern PWRLib_StackPS_t                   PWRLib_StackPS;

#if (cPWR_UseMCUStatus)
  extern PWRLib_MCUStatus_t               PWRLib_MCUStatus;
#endif  // #if (cPWR_UseMCUStatus)

#if (cPWR_UseRADIOStatus)
  extern PWRLib_RADIOStatus_t             PWRLib_RADIOStatus;
#endif  /* #if (cPWR_UseRADIOStatus) */

/******************************************************************************
* Description : Executes a illegal instruction to force a MCU reset.
* Assumptions : None
* Inputs      : None
* Output      : None
* Errors      : Not handled
******************************************************************************/
extern void PWRLib_Reset(void);

/*****************************************************************************
* This function is used to  set the global variable which 
* permits(on SET)/restricts(On RESET)the device to enter low power state. 
*
*
* Interface assumptions:
* None
* 
* The routine limitations.
* None
*
* Return value:
* None
*
* Effects on global data.
* It sets the  mLPMFlag
*
* Source of algorithm used.
* None
*
*****************************************************************************/ 
void PWR_AllowDeviceToSleep(void);

/*****************************************************************************
* This function is used to  reset the global variable which 
* permits(on SET)/restricts(On RESET)the device to enter low power state. 
*
*
* Interface assumptions:
* None
* 
* The routine limitations.
* None
*
* Return value:
* None
*
* Effects on global data.
* It resets the  mLPMFlag
*
* Source of algorithm used.
* None
*
*****************************************************************************/ 
void PWR_DisallowDeviceToSleep(void);

/***********************************************************************************
* This function is used to  Check  the Permission flag to go to 
* low power mode
*
* Interface assumptions:
* None
* 
* The routine limitations.
* None
*
* Return value:
*   TRUE  : If the device is allowed to go to the LPM else FALSE
*
* Effects on global data.
* None
*
* Source of algorithm used.
* None
*
*****************************************************************************/ 
bool_t PWR_CheckIfDeviceCanGoToSleep( void );

/******************************************************************************
* Description : This function makes the decision between Light Sleep and 
                Deep Sleep and goes to the sleep mode.
                The function is protected from interrupt.
* Input       : None
* Output      : PWRLib_WakeupReason_t status
* Errors      : Not handled
******************************************************************************/
PWRLib_WakeupReason_t PWR_EnterLowPower(void);
/*****************************************************************************/

 /******************************************************************************
* Description : Will initialize the project specific registers and setup for
*               correct function of PWR_CheckForAndEnterNewPowerState() function.
* Assumptions : Intended for use in main code and to be modified for each new
*               project.
* Input       : None
* Output      : None
* Errors      : Not handled
******************************************************************************/
void PWR_CheckForAndEnterNewPowerState_Init(void);

/******************************************************************************
* Description : Detection of low voltage.
                PWR_NODEPOWER_LEVEL_100 is always returned and no collection of 
                level is done
* Assumptions : None
* Inputs      : None
* Output      : PWR_NODEPOWER_LEVEL_100 : As specified above
* Errors      : Not handled
******************************************************************************/ 
PWRLib_LVD_VoltageLevel_t   PWRLib_LVD_ReportLevel(void);

/******************************************************************************
* Description : Set the output state of the 4 external drivers (i.e. KBI[3:0] ) 
*                        for use during sleep.
*
* Assumptions : None
* Inputs      : The output state of KBI[3:0]
* Output      : None
* Errors      : Not handled
****************************************************************************/
void PWR_SetExtOutPol(uint8_t val);

/******************************************************************************
* Description : This function is used to modify the default configured Doze Time 
*               in DeepSleep mode 6.
*             
* Input       : dozeTimeSym - new doze time duration in Mac symbols (16us)
* Output      : none
* Errors      : Not handled
******************************************************************************/
void PWR_SetDozeTimeSymbols(uint32_t dozeTimeSym);
 
#endif /* _PWR_INTERFACE_H_ */
