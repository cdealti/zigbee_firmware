/******************************************************************************
*  This file is to Initialize the entire BeeStack.
*
* (c) Copyright 2008, Freescale, Inc. All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
******************************************************************************/
#ifndef __IAR_SYSTEMS_ICC__
#ifdef MC13226Included_d
    #undef MC13226Included_d
#endif    
    #define MC13226Included_d 0
#endif
#include "EmbeddedTypes.h"
#include "BeeStackConfiguration.h"
#include "MsgSystem.h"
#include "PlatformInit.h"
#include "NVM_Interface.h"
#include "TMR_Interface.h"
#include "AppZdoInterface.h"
#include "BeeStackUtil.h"
#include "ZdoApsInterface.h"
#include "ZdoNwkInterface.h"
#include "PowerManager.h"
#include "BeeStackInit.h"
#include "BeeStack_Globals.h"
#include "TS_Interface.h"
#ifndef gHostApp_d
#include "AppAspInterface.h"
#endif
#include "AppAfInterface.h"
#include "ZtcInterface.h"
#include "UART_Interface.h"
#include "BeeStackInit.h"
#if gLpmIncluded_d 
  #include "pwr_interface.h"
  #include "pwr_configuration.h"
#endif
#if (gComboDeviceCapability_d)
  #include "BeeStack_Globals.h"
#endif /*gLpmIncluded_d*/
#ifndef __IAR_SYSTEMS_ICC__
#if !gNvStorageIncluded_d  && gNvHalIncluded_d
  #include "NV_FlashHal.h"
#endif
#endif

#if MC13226Included_d
  #include "BeeStackRomLinks.h"
#endif

/******************************************************************************
*******************************************************************************
* Private macros
*******************************************************************************
******************************************************************************/

/******************************************************************************
*******************************************************************************
* Private prototypes
*******************************************************************************
******************************************************************************/

void BeeStackTaskInit(void);
void TS_PlaceHolderTaskInit(void);

/******************************************************************************
*******************************************************************************
* Private type definitions
*******************************************************************************
******************************************************************************/

typedef struct beeTaskEntry_tag {
  tsTaskID_t *pTaskID;
  void (*pInitFunction)(void);
  void (*pEventHandler)(event_t);
  tsTaskPriority_t priority;
} beeTaskEntry_t;

/******************************************************************************
*******************************************************************************
* Private memory declarations
*******************************************************************************
******************************************************************************/

#define Task(taskIdGlobal, taskInitFunc, taskMainFunc, priority) \
  { &taskIdGlobal, taskInitFunc, taskMainFunc, priority },

beeTaskEntry_t const beeTaskTable[] = {
#include "BeeStackTasksTbl.h"
};

/******************************************************************************
*******************************************************************************
* Public functions
*******************************************************************************
******************************************************************************/

void BeeStackInit(void) {
#if (gLpmIncluded_d || gComboDeviceCapability_d)
#if gComboDeviceCapability_d
  if(gLpmIncluded)
#endif
  {
#ifndef gHostApp_d
#if cPWR_UsePowerDownMode    
    PWR_CheckForAndEnterNewPowerState_Init();
    PWRLib_SetCurrentZigbeeStackPowerState(StackPS_DeepSleep);
#endif    
#endif
  }
#endif

#ifndef __IAR_SYSTEMS_ICC__
#if gNvStorageIncluded_d 
  NvModuleInit();
#elif gNvHalIncluded_d
  NvHalInit();
#endif
#else //__IAR_SYSTEMS_ICC__
  NvModuleInit();
#endif

#ifndef __IAR_SYSTEMS_ICC__
  TMR_Init();
#else /* __IAR_SYSTEMS_ICC__ */
#ifndef gHostApp_d
#if MC13226Included_d 
  BeeStackROMInit();
#endif  
#endif  
#endif /* __IAR_SYSTEMS_ICC__ */
  Ztc_TaskInit();
  BeeStackTaskInit();
}

/******************************************************************************
*******************************************************************************
* Private functions
*******************************************************************************
******************************************************************************/

/* This function can be used in the table in BeeStackTasksTbl.h for any task */
/* that does not have it's own init function. */
void TS_PlaceHolderTaskInit(void) {
}

/*****************************************************************************/

/* Init the BeeStack tasks. Add all of them to the kernel's task table, and */
/* call of the init functions. */
void BeeStackTaskInit(void) {
  index_t i;

  /* Add the tasks to the kernel's task table first. That way, if any of the */
  /* init functions want to set events, all of the task ids will be defined. */
  for (i = 0; i < NumberOfElements(beeTaskTable); ++i) {
    *beeTaskTable[i].pTaskID = TS_CreateTask(beeTaskTable[i].priority,
                                             beeTaskTable[i].pEventHandler);
  }

  for (i = 0; i < NumberOfElements(beeTaskTable); ++i) {
    (*beeTaskTable[i].pInitFunction)();
  }
}                                       /* BeeStackTaskInit() */
