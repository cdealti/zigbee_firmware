/************************************************************************************
* Function to handle/set the different power down states.
*
*
* (c) Copyright 2005, Freescale Semiconductor, Inc. All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
***********************************************************************************/
#ifndef _PWR_CONFIGURATION_H_
#define _PWR_CONFIGURATION_H_

/************************************************************************************
*************************************************************************************
* Module configuration constants
*************************************************************************************
************************************************************************************/
//-----------------------------------------------------------------------------
// To enable/disable all of the code in this PWR/PWRLib files.
//   TRUE =  1: Use PowerDown functions (Normal)
//   FALSE = 0: Don't use PowerDown. Will cut variables and code out. But
//     functions still exist. Useful for debugging and test purposes
#define cPWR_UsePowerDownMode                    1

// To configure the MCU retention mode used during sleep
//   0:          No MCU retention
//   1:          MCU retention is On during sleep
#define cPWR_MCURetentionMode                    1

// To configure the MCU Pad retention mode used during sleep (if MCU retention configured)
//   0:          No Pad retention
//   1:          MCU Pad retention on id MCU retention was configured also 
#define cPWR_MCUPadRetentionMode                 1

// These bits set the output state of the 4 external drivers (i.e. KBI[3:0] ) for use during sleep.
// values: 0 (0000) -> 0xF (1111)
#define cPWR_ExtOutPol   0

// To configure the RAM retention mode used during sleep
//   gRamRet8k_c:         First 8k of RAM retained during sleep
//   gRamRet32k_c:        First 32k of RAM retained during sleep
//   gRamRet64k_c:        First 64k of RAM retained during sleep
//   gRamRet96k_c:        First 96k (All) of RAM retained during sleep
#define cPWR_RAMRetentionMode                    gRamRet96k_c  

//-----------------------------------------------------------------------------
// The way that DeepSleep mode are handled. Following possibilities exist:
//   0: No DeepSleep done, but PTC application can set modes
//   1: CPU Sleep: Hibernate, Wake-up on KBI(gKeyBoardSupported_d)
//   2: CPU Sleep: Hibernate, Wake-up on Wakeup Timer
//   3: CPU Sleep: Hibernate, Wake-up on either KBI(gKeyBoardSupported_d) or Wakeup Timer
//   4: CPU Sleep: Hibernate, Wake-up on Reset
//   5: CPU Sleep: Doze, Wake-up on KBI(gKeyBoardSupported_d)
//   6: CPU Sleep: Doze, Wake-up on Wakeup Timer
//   7: CPU Sleep: Doze, Wake-up on either KBI(gKeyBoardSupported_d) or Wakeup Timer
//   8: CPU Sleep: Doze, Wake-up on either KBI(gKeyBoardSupported_d) or Wakeup Timer with runtime configurable wake-up timeout
#define cPWR_DeepSleepMode                      7

// Maximum number of configurable deep sleep modes
#define cPWR_DeepSleepModeMax                   9


//-----------------------------------------------------------------------------
// The way that Sleep mode are handled. Following possibilities exist:
//   0: No Sleep done, but PTC application can set modes
//   1: Doze mode on RADIO with clock enabled and WAIT on MCU
#define cPWR_SleepMode                          1

// Maximum number of configurable deep sleep modes
#define cPWR_SleepModeMax                       2


//-----------------------------------------------------------------------------
//   Sleep Timeout in milliseconds.
//   The maximum sleep time depends on the Low Power mode used:
//
//                             |          HIBERNATE            |         DOZE          |
//                             | 2KHz Ring Osc |  32 KHz XTAL  |       XTAL/128        |
//                             ---------------------------------------------------------
//   MAX SLEEP TIME            |   24.8 DAYS   |   36.4 HOURS  |  11.75 HOURS @ 13 MHz |
//                                                             |   6.36 HOURS @ 24 MHz |
//                                                             |   5.87 HOURS @ 26 MHz |
//   
#define cPWR_SleepTimeoutMs                     3000

//-----------------------------------------------------------------------------
//   Milliseconds to WakeUp timer ticks.
//   This macro depends on the DeepSleepMode option. Modify only if Hibernate/Doze modes configuration are changed (not the selected mode).
#define cPWR_Ms2WakeUpTimerTicks(ms)           ( \
                                                (cPWR_DeepSleepMode < 5) ?  /* Hibernate */ \
                                                 ( \
                                                  ((CRM_SYS_CNTL.xtal32Exists == 1) && (CRM_XTAL32_CNTL.xtal32En == 1)) ?  \
                                                    (uint32_t)((ms) * 32) : /* Hibernate with 32KHz XTAL */ \
                                                      (uint32_t)((ms) * 2)  /* Hibernate with 2KHz Osc */ \
                                                        ) : \
                                                         (uint32_t)(( (uint64_t)(ms) * SYSTEM_CLOCK) / 128) /* Doze with SYSTEM_CLOCK / 128 KHz */ \
                                                          )

//   Milliseconds to WakeUp timer ticks. To be used only after wake-up (wake-up from hibernate/doze info available)
//                                       Wait also for SLEEP_SYNC status bit before using this macro
#define cPWR_WuTmrTicks2TmrTicks(ticks)        ( \
                                                (CRM_STATUS.hibWuEvt == 1) ? /* Hibernate */ \
                                                  ( \
                                                    ((CRM_SYS_CNTL.xtal32Exists == 1) && (CRM_XTAL32_CNTL.xtal32En == 1)) ?  \
                                                      ((uint32_t)(ticks) * (SYSTEM_CLOCK / (gTmrPrescaler_c * 32))) : /* Hibernate with 32KHz XTAL */ \
                                                        ((uint32_t)(ticks) * (SYSTEM_CLOCK / (gTmrPrescaler_c * 2)))  /* Hibernate with 2KHz Osc */ \
                                                          ) : \
                                                             ((uint32_t)(ticks) * (128 / gTmrPrescaler_c)) /* Doze with SYSTEM_CLOCK / 128 KHz */ \
                                                             )

//  MAC symbols (16us) to wake-up timer ticks (doze with XTAL/128 only)
#define cPWR_MACSymbols2WakeUpTimerTicks(sym)        ( \
                                                      (SYSTEM_CLOCK == 24000) ? /* 1 MAC symbol (16us) = 3 XTAL/128 ticks */ \
                                                        (uint32_t)( (uint32_t)(sym) * 3) : /* Doze with (SYSTEM_CLOCK / 128) KHz */ \
                                                          (uint32_t)( ((uint64_t)(sym) * SYSTEM_CLOCK) / 8000) /* Doze with (SYSTEM_CLOCK / 128) KHz */ \
                                                            )
// Milliseconds to MAC symbols (16us) 
#define cPWR_Ms2MACSymbols(ms)                      (uint32_t)( ((uint32_t)(ms) * 1000) / 16)
//-----------------------------------------------------------------------------
// 32bits value representing the Wake-up timer timeout in ticks:
//   In Doze mode the Wakeup_timer frequency is XTAL/128
//   In Hibernate mode the Wakeup_timer frequency is 2KHz internal (default) or 32KHz external (must be configured by the application)
#define cPWR_SleepTimeout                       cPWR_Ms2WakeUpTimerTicks(cPWR_SleepTimeoutMs)


//-----------------------------------------------------------------------------
// Enabling of external call to a procedure each time that DeepSleep are exited
//   0: Don't call any functions after DeepSleep (MAC)
//   1: Call a function after DeepSleep (Stack)
#define cPWR_CallWakeupStackProcAfterDeepSleep  1


//-----------------------------------------------------------------------------
// The extra function to call every time RTI clock run's out. Used by Stack.
#if (cPWR_CallWakeupStackProcAfterDeepSleep == 0)
  #define cPWR_DeepSleepWakeupStackProc         ;
#else
  extern void                                   DeepSleepWakeupStackProc(void);
  #define cPWR_DeepSleepWakeupStackProc         DeepSleepWakeupStackProc();  
#endif

//-----------------------------------------------------------------------------
// The use of Low Voltage detection has the following possibilities:
//   0: Don't use Low voltage detection at all (Normal)
//   1: Use polled => Check made each time the function is called.
//   2: RTI timer used for handling when to poll LVD, according
//      to the cPWR_LVD_Ticks constant (Normal)
#define cPWR_LVD_Enable                         0


//-----------------------------------------------------------------------------
// As a service to Application this variable can be created and updated
//   TRUE =  1: Use/update the PWRLib_RADIOStatus variable (Debug/Application)
//   FALSE = 0: Don't (Normal)
#define cPWR_UseRADIOStatus                     0


//-----------------------------------------------------------------------------
// As a service to Application this variable can be created and updated
//   TRUE =  1: Use/update the MCU_Status variable (Debug/Application)
//   FALSE = 0: Don't (Normal)
#define cPWR_UseMCUStatus                       0


// As a service to Application this variable can be created and updated
//   TRUE =  1: MACA Clock is updated with the Sleep time upon wake-up (default).
//   FALSE = 0: MACA Clock is not updated with the Sleep time upon wake-up.
#define cPWR_UpdateMACAClockAtWakeUp             1

#endif /* _PWR_CONFIGURATION_H_ */

