/************************************************************************************
* Function to handle the different power down states.
*
*
* (c) Copyright 2008, Freescale, Inc. All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
************************************************************************************/


/************************************************************************************
*************************************************************************************
* Includes
*************************************************************************************
************************************************************************************/
#include "EmbeddedTypes.h"
#include "PWR_Configuration.h"   
#include "PWR_Interface.h"
#include "Crm.h"
#include "Keyboard.h"
#include "KeyboardHAL.h"
#include "Interrupt.h"
#include "AppAspInterface.h"
#include "AppToPlatformConfig.h"
#include "TMR_Interface.h"
#include "NVM_Interface.h"
/******************************************************************************
*******************************************************************************
* Private macros
*******************************************************************************
******************************************************************************/                                              

#define PWR_MCUDoze()      PWR_MCUSleep(gDoze_c)

#define PWR_MCUHibernate() PWR_MCUSleep(gHibernate_c)

#define mPWR_MacaClk                       (*((volatile uint32_t *)(0x80004048)))
#define mMACA_TIMER_UPDATE_FACTOR_HIB_2K   125
#define mMACA_TIMER_UPDATE_FACTOR_HIB_32K  (125.0/16.0)
#define mMACA_TIMER_UPDATE_FACTOR_DOZE     ((250000.0*128.0)/SYSTEM_CLOCK)

/******************************************************************************
*******************************************************************************
* Private prototypes
*******************************************************************************
******************************************************************************/
#if (cPWR_UsePowerDownMode)
#if (cPWR_UpdateMACAClockAtWakeUp)
static void PWR_MacaTimerUpdate(void);
#endif /* cPWR_UpdateMACAClockAtWakeUp */
#endif /* cPWR_UsePowerDownMode */
/******************************************************************************
*******************************************************************************
* Private type definitions
*******************************************************************************
******************************************************************************/

/******************************************************************************
*******************************************************************************
* Private memory declarations
*******************************************************************************
******************************************************************************/
#if (gTMR_EnableLowPowerTimers_d) 
#if cPWR_UsePowerDownMode
 static uint16_t notCountedTmrTicksBeforeSleep;
#endif //cPWR_UsePowerDownMode
#endif

#if cPWR_UsePowerDownMode
static uint8_t mExtOutPol = cPWR_ExtOutPol;
#endif //cPWR_UsePowerDownMode
/******************************************************************************
*******************************************************************************
* Public memory declarations
*******************************************************************************
******************************************************************************/
static uint8_t mLPMFlag = gAllowDeviceToSleep_c;
/* but recommended to be regarded as private */

#if (cPWR_UpdateMACAClockAtWakeUp)
#if cPWR_UsePowerDownMode
static   uint32_t mSavedMacaClk;
#endif //cPWR_UsePowerDownMode
#endif /* cPWR_UpdateMACAClockAtWakeUp */

volatile PWRLib_WakeupReason_t     PWRLib_MCU_WakeupReason;

#if (cPWR_UseMCUStatus)
  PWRLib_MCUStatus_t               PWRLib_MCUStatus;
#endif  // #if (cPWR_UseMCUStatus)
 
#if (cPWR_UseRADIOStatus)
    PWRLib_RADIOStatus_t           PWRLib_RADIOStatus;
#endif  /* #if (cPWR_UseRADIOStatus) */ 
/*--- Zigbee STACK status ---*/ 
PWRLib_StackPS_t                   PWRLib_StackPS;


/* Sleep timeout in MAC symbols to be used in DeepSleepMode 8 */
#if ((cPWR_UsePowerDownMode) && (cPWR_DeepSleepMode == 8))
 static uint32_t mPWR_SleepTimeoutSym = cPWR_Ms2MACSymbols(cPWR_SleepTimeoutMs);
#endif   
 

/************************************************************************************
* Set the output state of the 4 external drivers (i.e. KBI[3:0] ) for use during sleep.
*
* Interface assumptions:
*   None
*   
* Return value:
*   None
* 
************************************************************************************/
#if cPWR_UsePowerDownMode
void PWR_SetExtOutPol(uint8_t val)
{
  mExtOutPol = val;
}
#endif //cPWR_UsePowerDownMode

/************************************************************************************
* Set the NVM write operation semaphore.
*
* This function will set the NVM write operation semaphore so that the idle task wont
* do a NVM write operation unless this is released by calling 
* NVM_ClearCriticalNVMSection.
*
* Interface assumptions:
*   None
*   
* Return value:
*   None
* 
************************************************************************************/

void PWR_AllowDeviceToSleep(void)
{
  unsigned int saveInt;
  saveInt = IntDisableAll();
  
  if( mLPMFlag != 0 )
  {
    mLPMFlag--;
  }
  IntRestoreAll(saveInt);
}

/************************************************************************************
* Release the NVM write operation semaphore.
*
* This function will release the NVM write operation semaphore so that the idle task 
* can do a NVM write operation.
*  
* Interface assumptions:
*   None
*   
* Return value:
*   None
*
************************************************************************************/
 
void PWR_DisallowDeviceToSleep(void)
{
  IntDisableAll();
  mLPMFlag++;
  IntEnableAll();
}

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
bool_t PWR_CheckIfDeviceCanGoToSleep( void ) 
{
  return (bool_t)(mLPMFlag == 0); 
}

/*****************************************************************************/
/* Please see in PWR_Interface.h for description                             */
void PWRLib_Reset(void)
{
 CRM_SoftReset();
}
/*****************************************************************************/

/*****************************************************************************/
void PWR_MCUWait(void) {
#if (cPWR_UsePowerDownMode)
  mMCU_SetStatus( MCU_Wait);
  mRADIO_SetStatus(RADIO_Idle);
  CRM_Wait4Irq();
#endif  /* #if (cPWR_UsePowerDownMode) */
}  /* PWR_MCUWait(void) =================================================== */

/*****************************************************************************/
#if (cPWR_UsePowerDownMode)
static void PWR_CallBeforeSleep(void) 
{
#if (cPWR_UpdateMACAClockAtWakeUp)
  mSavedMacaClk = mPWR_MacaClk;
#endif /* cPWR_UpdateMACAClockAtWakeUp */  
}  /* PWR_CallBeforeSleep(void) =================================================== */
#endif  /* #if (cPWR_UsePowerDownMode) */

/*****************************************************************************/
/* Please see in PWR_Interface.h for description                             */
void PWR_MCUSleep(crmSleep_t crmSleep) {
#if (cPWR_UsePowerDownMode)
 crmSleepCtrl_t  crmSleepCtrl;
 
 crmSleepCtrl.sleepType = crmSleep;
 crmSleepCtrl.ramRet = cPWR_RAMRetentionMode;
 crmSleepCtrl.mcuRet = cPWR_MCURetentionMode;
 crmSleepCtrl.digPadRet = cPWR_MCUPadRetentionMode;
 crmSleepCtrl.pfToDoBeforeSleep = PWR_CallBeforeSleep;
 
 CRM_WU_CNTL.extOutPol = mExtOutPol;
 
 if(crmSleep == gDoze_c)
 {
  mMCU_SetStatus(MCU_Doze);
 }
 
 if(crmSleep == gHibernate_c)
 {
  mMCU_SetStatus(MCU_Hibernate);
 }
 mRADIO_SetStatus(RADIO_Off);
 CRM_GoToSleep(&crmSleepCtrl);
#endif  /* #if (cPWR_UsePowerDownMode) */
}  /* PWR_MCUSleep(void) =================================================== */

/*****************************************************************************/
void PWR_AllOff(void)
{
  #if (cPWR_UsePowerDownMode)
   /* Disable the wake-up sources in crm */
   CRM_WU_CNTL.extWuEn = 0;
   CRM_WU_CNTL.timerWuEn = 0;
   CRM_WU_CNTL.rtcWuEn = 0;
   /* Enter Hibernate with no wake-up sources. wake-up on Reset only */
   PWR_MCUHibernate();
  #endif
}  /* PWR_AllOff(void) =================================================== */


/*****************************************************************************/
/* Please see in PWR_Interface.h for description                             */
void PWR_RunAgain(void) {
  #if (cPWR_UsePowerDownMode)

   while(CRM_STATUS.sleepSync == 0);
   
   if(FALSE == gHardwareParameters.gSystemClock24MHz_c)
   {
    (*((volatile uint32_t *)(0x80003048))) = 0x00000F78;
    (*((volatile uint32_t *)(0x80009000))) = 0xC0050300;
   }
#if (cPWR_UpdateMACAClockAtWakeUp)
   PWR_MacaTimerUpdate();
#endif
   CRM_REGS_P->Status = 1;

    #if (gTMR_EnableLowPowerTimers_d) 
    {
           volatile uint32_t crmSleepTime = CRM_REGS_P->WuCount;
           
           crmSleepTime = cPWR_WuTmrTicks2TmrTicks(crmSleepTime);
           
           crmSleepTime += notCountedTmrTicksBeforeSleep;
           /* Sync. the low power timers */
           TMR_SyncLpmTimers(crmSleepTime);
     }  
    #endif /* #if (gTMR_EnableLowPowerTimers_d)  */

  CRM_REGS_P->Status = (CRM_REGS_P->Status & (~0xF0)); /* Clear interrupt sources except from KBI */

  mMCU_SetStatus(MCU_Running);
  mRADIO_SetStatus(RADIO_Idle);
  #endif  /* #if (cPWR_UsePowerDownMode) */
}  /* PWR_RunAgain(void) ======================================================*/


/*****************************************************************************/
/* Please see in PWR_Interface.h for description                             */
bool_t PWR_DeepSleepAllowed(void) {
  #if ( cPWR_UsePowerDownMode)
    #if ( cPWR_DeepSleepMode < cPWR_DeepSleepModeMax)                               /* Normal running mode*/
      if (PWRLib_GetCurrentZigbeeStackPowerState == StackPS_DeepSleep) {
          if ( Asp_GetMacStateReq() == gAspMacStateIdle_c) {
            return TRUE;
          } else {
            return FALSE;
          }
      } else {
        return FALSE;
      }
    #else                                                /* Debug mode */
      #error "*** ERROR: cPWR_DeepSleepMode has an unsupported value"
    #endif
  #else                                                      /* Debug mode */
    return TRUE;
  #endif  /* #if (cPWR_UsePowerDownMode)*/
}  /* PWR_DeepSleepAllowed ==================================================*/


/*****************************************************************************/
/* Please see in PWR_Interface.h for description                             */
bool_t  PWR_SleepAllowed(void) {  
  #if (cPWR_UsePowerDownMode)
    #if (cPWR_SleepMode < cPWR_SleepModeMax)                               /* Normal running mode*/
      if (( PWRLib_GetCurrentZigbeeStackPowerState == StackPS_Sleep) ||  \
          ( PWRLib_GetCurrentZigbeeStackPowerState == StackPS_DeepSleep)) {
          if (( Asp_GetMacStateReq()==gAspMacStateNotEmpty_c) || \
              ( Asp_GetMacStateReq()==gAspMacStateIdle_c)) {
            return TRUE;
          } else {
            return FALSE;
          }
      } else {
        return FALSE;
      }
    #else                                                      /* Debug mode */
      #error "*** ERROR: cPWR_SleepMode has an unsupported value"
    #endif
  #else                                                      /* Debug mode */
    return TRUE;
  #endif  /* #if (cPWR_UsePowerDownMode) else */
}  /* PWR_SleepAllowed ======================================================*/


/*****************************************************************************/
/* Please see in PWR_Interface.h for description                             */
PWRLib_WakeupReason_t  PWR_HandleDeepSleep( zbClock24_t  DozeDuration) {
  PWRLib_WakeupReason_t  Res;
  Res.AllBits = 0;
  
  /* To avoid warnings */
  (void)DozeDuration;
  
  #if (cPWR_UsePowerDownMode)
  
		  /*---------------------------------------------------------------------------*/
    #if (cPWR_DeepSleepMode==0)
      Res.AllBits = PWRLib_MCU_WakeupReason.AllBits;
      /* No code */

    /*---------------------------------------------------------------------------*/
    #elif ((cPWR_DeepSleepMode == 1)  || (cPWR_DeepSleepMode == 5))
      #if (gKeyBoardSupported_d == TRUE)        
         /* Disable the other wake-up sources */
         CRM_WU_CNTL.timerWuEn = 0;
         CRM_WU_CNTL.rtcWuEn = 0;
         
         #if(cPWR_DeepSleepMode == 1)
          PWR_MCUHibernate();
          PWRLib_MCU_WakeupReason.Bits.FromHibernate = TRUE;
         #else
          PWR_MCUDoze();
          PWRLib_MCU_WakeupReason.Bits.FromDoze = TRUE;
         #endif
          
         if(CRM_STATUS.extWuEvt)
         {
          PWRLib_MCU_WakeupReason.Bits.FromKBI = TRUE;
         } 
         
         Res.AllBits = PWRLib_MCU_WakeupReason.AllBits;         
         
         PWR_RunAgain();         
         
      #else 
        #error "*** ERROR: gKeyBoardSupported_d has to be set to 1"
      #endif
    /*---------------------------------------------------------------------------*/
    #elif ((cPWR_DeepSleepMode == 2) || (cPWR_DeepSleepMode == 6))
       #if (gKeyBoardSupported_d == TRUE)
          /* Disable the Keyboard wake-up in crm */
          CRM_WU_CNTL.extWuEn = 0;
       #endif     
       
       /* Disable the other wake-up sources */
       CRM_WU_CNTL.rtcWuEn = 0;   
       
       /* setup the Wake-up Timer */
       { 
        crmWuCtrl_t    wuCtrl;
        wuCtrl.wuSource = gTimerWu_c;
        wuCtrl.TIMER_WU_EN  = 1;
        wuCtrl.TIMER_WU_INT_EN = 0;
        wuCtrl.TIMER_WU_TIMEOUT = cPWR_SleepTimeout;
        CRM_WuCntl(&wuCtrl);
       } 
       
       #if(cPWR_DeepSleepMode == 2)
        PWR_MCUHibernate();
        PWRLib_MCU_WakeupReason.Bits.FromHibernate = TRUE;
       #else
        PWR_MCUDoze();
        PWRLib_MCU_WakeupReason.Bits.FromDoze = TRUE;
       #endif       
       
       if( (CRM_STATUS.extWuEvt == 0) && (CRM_STATUS.rtcWuEvt == 0))
       {
         PWRLib_MCU_WakeupReason.Bits.FromTimer = TRUE;
       }
          
       #if (gKeyBoardSupported_d == TRUE)   
          /* restore the Keyboard configuration */
          CRM_WU_CNTL.extWuEn = gKbiWuEN_c;
       #endif                    

       Res.AllBits = PWRLib_MCU_WakeupReason.AllBits;

       PWR_RunAgain();
       if(PWRLib_MCU_WakeupReason.Bits.FromTimer == TRUE)
       {  
        cPWR_DeepSleepWakeupStackProc;          /* User function called only on timeout */
       } 

    /*---------------------------------------------------------------------------*/
    #elif ((cPWR_DeepSleepMode==3) || (cPWR_DeepSleepMode==7))
       #if (gKeyBoardSupported_d == TRUE)                 
        /* Disable the other wake-up sources */
        CRM_WU_CNTL.rtcWuEn = 0;   
        
        /* setup the Wake-up Timer */
        { 
         crmWuCtrl_t    wuCtrl;
         wuCtrl.wuSource = gTimerWu_c;
         wuCtrl.TIMER_WU_EN = 1;
         wuCtrl.TIMER_WU_INT_EN = 0;
         wuCtrl.TIMER_WU_TIMEOUT = cPWR_SleepTimeout;
         CRM_WuCntl(&wuCtrl);
        } 
        
        #if (cPWR_DeepSleepMode==3)
          PWR_MCUHibernate();
          PWRLib_MCU_WakeupReason.Bits.FromHibernate = TRUE;
        #else
          PWR_MCUDoze();
          PWRLib_MCU_WakeupReason.Bits.FromDoze = TRUE;
        #endif          
          
        if( (CRM_STATUS.extWuEvt == 0) && (CRM_STATUS.rtcWuEvt == 0))
        {
         PWRLib_MCU_WakeupReason.Bits.FromTimer = TRUE;
        }
        else
        {
         if(CRM_STATUS.extWuEvt)
         {
          PWRLib_MCU_WakeupReason.Bits.FromKBI = TRUE;
         }
        }

        Res.AllBits = PWRLib_MCU_WakeupReason.AllBits;

        PWR_RunAgain();
        if(PWRLib_MCU_WakeupReason.Bits.FromTimer == TRUE)
        {  
         cPWR_DeepSleepWakeupStackProc;          /* User function called only on timeout */
        } 
       #else 
        #error "*** ERROR: gKeyBoardSupported_d has to be set to 1"
       #endif
        
   /*---------------------------------------------------------------------------*/
   #elif (cPWR_DeepSleepMode==8)
     #if (gKeyBoardSupported_d == TRUE)                 
        /* Disable the other wake-up sources */
        CRM_WU_CNTL.rtcWuEn = 0;   
        
        /* setup the Wake-up Timer */
        { 
         crmWuCtrl_t    wuCtrl;
         wuCtrl.wuSource = gTimerWu_c;
         wuCtrl.TIMER_WU_EN = 1;
         wuCtrl.TIMER_WU_INT_EN = 0;
         wuCtrl.TIMER_WU_TIMEOUT = cPWR_MACSymbols2WakeUpTimerTicks(mPWR_SleepTimeoutSym);
         CRM_WuCntl(&wuCtrl);
        } 
        
        PWR_MCUDoze();
        PWRLib_MCU_WakeupReason.Bits.FromDoze = TRUE;
        
        if( (CRM_STATUS.extWuEvt == 0) && (CRM_STATUS.rtcWuEvt == 0))
        {
         PWRLib_MCU_WakeupReason.Bits.FromTimer = TRUE;
        }
        else
        {
         if(CRM_STATUS.extWuEvt)
         {
          PWRLib_MCU_WakeupReason.Bits.FromKBI = TRUE;
         }
        }

        Res.AllBits = PWRLib_MCU_WakeupReason.AllBits;

        PWR_RunAgain();
        if(PWRLib_MCU_WakeupReason.Bits.FromTimer == TRUE)
        {  
         cPWR_DeepSleepWakeupStackProc;          /* User function called only on timeout */
        } 
       #else 
        #error "*** ERROR: gKeyBoardSupported_d has to be set to 1"
       #endif
   //---------------------------------------------------------------------------
   #elif (cPWR_DeepSleepMode==4)       
        PWR_AllOff();

    //---------------------------------------------------------------------------    
    #else
      #error "*** ERROR: Not a valid cPWR_DeepSleepMode chosen"
    #endif
        
    PWRLib_MCU_WakeupReason.AllBits = 0;
    return Res;
    
  #else  /* #if (cPWR_UsePowerDownMode) else */
    PWRLib_MCU_WakeupReason.AllBits = 0;
    return Res;          /*(PWRLib_WakeupReason_t) DozeDuration;*/
  #endif  /* #if (cPWR_UsePowerDownMode) end*/
}  /* PWR_HandleDeepSleep =====================================================*/


/*****************************************************************************/
/* Please see in PWR_Interface.h for description                             */
PWRLib_WakeupReason_t  PWR_HandleSleep( zbClock24_t DozeDuration) {
  PWRLib_WakeupReason_t   Res;

  Res.AllBits = 0;
  #if (cPWR_UsePowerDownMode)
    /*---------------------------------------------------------------------------*/
    #if (cPWR_SleepMode==0)
      Res.AllBits = PWRLib_MCU_WakeupReason.AllBits;
      PWRLib_MCU_WakeupReason.AllBits = 0;

    /*---------------------------------------------------------------------------*/
    #elif (cPWR_SleepMode==1)
        PWR_MCUWait();
        PWRLib_MCU_WakeupReason.Bits.FromWait = TRUE;
        
        Res.AllBits = PWRLib_MCU_WakeupReason.AllBits;
    /*---------------------------------------------------------------------------*/
    #else
      #error "*** ERROR: Not a valid cPWR_SleepMode chosen"
    #endif
        
    PWRLib_MCU_WakeupReason.AllBits = 0;
    return Res; 
    
  #else  /* #if (cPWR_UsePowerDownMode) else */
    return Res;
  #endif  /* #if (cPWR_UsePowerDownMode) end */
}  /* PWR_HandleSleep =========================================================*/


/************************************************************************************
*************************************************************************************
* Very Public functions, recommended for general use
*************************************************************************************
************************************************************************************/

/*****************************************************************************/
/* Please see in PWR_Interface.h for description                             */
PWRLib_WakeupReason_t PWR_CheckForAndEnterNewPowerState(PWR_CheckForAndEnterNewPowerState_t NewPowerState, zbClock24_t DozeDuration) {
  PWRLib_WakeupReason_t    ReturnValue;
  ReturnValue.AllBits = 0;

  #if (cPWR_UsePowerDownMode)

    if ( NewPowerState == PWR_Run) {
      /* ReturnValue = 0; */
    }
    else if ( NewPowerState == PWR_OFF) {      
      while (1) 
      {
        PWR_AllOff();
      }    /* Never returns */
    }
    else if ( NewPowerState == PWR_Reset) {
      PWRLib_Reset();                      /* Never returns	 */
    }
     
    else if (( NewPowerState == PWR_DeepSleep) && PWR_DeepSleepAllowed()) {
      ReturnValue = PWR_HandleDeepSleep( DozeDuration);
    } 
    else if (( NewPowerState == PWR_Sleep) && PWR_SleepAllowed()) {
      ReturnValue = PWR_HandleSleep( DozeDuration);
    }
    else {
      /* ReturnValue = FALSE; */
    }

    PWRLib_MCU_WakeupReason.AllBits = 0;                     /* Clear wakeup reason */
  #else
    ReturnValue.AllBits = 0xff | (NewPowerState > 0) | (DozeDuration > 0); 
                              /* To remove warning for variabels in functioncall */
  #endif  /* #if (cPWR_UsePowerDownMode) */

  return ReturnValue;
}   /* PWR_CheckForAndEnterNewPowerState ============================================*/

/*****************************************************************************/
/* Please see in PWR_Interface.h for description                             */
PWRLib_LVD_VoltageLevel_t   PWRLib_LVD_ReportLevel(void) {
 
  /* TO DO: add code for sampling battery */
  /* the function should return one of the following power levels: 
  PWR_NODEPOWER_LEVEL_100 
  PWR_NODEPOWER_LEVEL_66
  PWR_NODEPOWER_LEVEL_50
  PWR_NODEPOWER_LEVEL_33
  PWR_NODEPOWER_LEVEL_CRITICAL
  */

  return PWR_NODEPOWER_LEVEL_100;
}  /* PWRLib_LVD_ReportLevel ================================================*/

/*****************************************************************************/
/* Please see in PWR_Interface.h for description                             */
void PWR_CheckForAndEnterNewPowerState_Init(void) {
#if (cPWR_UsePowerDownMode)
 mMCU_SetStatus( MCU_Running);
 mRADIO_SetStatus(RADIO_Idle);
 PWRLib_SetCurrentZigbeeStackPowerState( StackPS_DeepSleep);
 
 PWRLib_MCU_WakeupReason.AllBits = 0;
 if(CRM_STATUS.hibWuEvt == 1)
 {
  PWRLib_MCU_WakeupReason.Bits.FromHibernate = TRUE;  
 }
 else
 {
  if(CRM_STATUS.dozeWuEvt == 1)
  {
   PWRLib_MCU_WakeupReason.Bits.FromDoze = TRUE;  
  }
  else
  {
   PWRLib_MCU_WakeupReason.Bits.FromReset = TRUE;  
  }
 }
#endif  /* #if (cPWR_UsePowerDownMode) */
}   /* PWR_CheckForAndEnterNewPowerState_Init ==================================*/


/********************************************************************************/
PWRLib_WakeupReason_t PWR_EnterLowPower(void)
{
  PWRLib_WakeupReason_t ReturnValue;  
#if (cPWR_UsePowerDownMode)    
  bool_t interruptsEnabled = FALSE;
#endif
  ReturnValue.AllBits = 0;
  
#if (cPWR_UsePowerDownMode)  
 /* if Low Power Capability is enabled*/
 #if (gTMR_EnableLowPowerTimers_d)  
   if (TMR_AreAllTimersOff())  /*No timer running*/
   {
    /* if more low power timers are running, stop the hardware timer
     * and save the spend time in millisecond that wasn't counted.
     */
    notCountedTmrTicksBeforeSleep = TMR_NotCountedTicksBeforeSleep();
  }
 #endif /* #if (gTMR_EnableLowPowerTimers_d)  */
  
 /* Low Voltage check ... not currently supported */
  
  IntDisableAll();
  if (TS_PendingEvents() == FALSE)
  {		 
    PWRLib_SetCurrentZigbeeStackPowerState(StackPS_DeepSleep);
    if (TMR_AreAllTimersOff())  /*No timer running*/
    {			
      ReturnValue = PWR_CheckForAndEnterNewPowerState ( PWR_DeepSleep, 0);
    }else /*timers are running*/
    { 	 
     interruptsEnabled = TRUE;
     // Enable the interrupts before entering the Sleep mode. The wake-up from WAIT is possible only if the interrupts are globaly enabled
     IntEnableAll();
     ReturnValue = PWR_CheckForAndEnterNewPowerState ( PWR_Sleep, 0);
    }
  } /* enable irq's if there is pending evens */
  if(interruptsEnabled == FALSE)
  {
   IntEnableAll();  
  } 
#endif  
  
  return ReturnValue;
}

/* Please see in PWR_Interface.h for description                                          */
void PWR_SetDozeTimeSymbols(uint32_t dozeTimeSym)
{
#if ((cPWR_UsePowerDownMode) && (cPWR_DeepSleepMode == 8))
  if(dozeTimeSym)
  {
   mPWR_SleepTimeoutSym = dozeTimeSym;
  } 
#else
 (void)dozeTimeSym;
#endif 
}

/******************************************************************************
*******************************************************************************
* Private functions
*******************************************************************************
******************************************************************************/
#if (cPWR_UsePowerDownMode) 
#if (cPWR_UpdateMACAClockAtWakeUp)
static void PWR_MacaTimerUpdate(void)
{

 if(CRM_STATUS.hibWuEvt)
 {
   if(CRM_SYS_CNTL.xtal32Exists)
   {
    mPWR_MacaClk = mSavedMacaClk + (uint32_t)(CRM_REGS_P->WuCount * mMACA_TIMER_UPDATE_FACTOR_HIB_32K);
   }
   else
   {
    mPWR_MacaClk = mSavedMacaClk + CRM_REGS_P->WuCount * mMACA_TIMER_UPDATE_FACTOR_HIB_2K;
   }
 }
 else
 { 
   if(CRM_STATUS.dozeWuEvt)
   {
    mPWR_MacaClk = mSavedMacaClk + (uint32_t)(CRM_REGS_P->WuCount * mMACA_TIMER_UPDATE_FACTOR_DOZE);
   }
 }  

}
#endif /* cPWR_UpdateMACAClockAtWakeUp */
#endif /* cPWR_UsePowerDownMode */

/******************************************************************************
*******************************************************************************
* Private Debug stuff
*******************************************************************************
******************************************************************************/
