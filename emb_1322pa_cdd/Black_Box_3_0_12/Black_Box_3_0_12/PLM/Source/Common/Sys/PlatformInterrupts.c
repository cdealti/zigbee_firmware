/************************************************************************************
*
* (c) Copyright 2004, Freescale, Inc.  All rights reserved.
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale.
*
************************************************************************************/
#include "EmbeddedTypes.h"
#include "Interrupt.h"
#include "AppMacInterrupt.h"

extern void  CRM_Isr(void);
extern void  TmrIsr(void);

void EnableMacaInt(void);
void DisableMacaInt(void);
unsigned int IsMacaIntEnabled(void);
unsigned int IsMacaIntAsserted(void);

/***********************************************************************************/
void AppInterruptsInit(void)
{
  
  pfEnableMacaInt     = (void(*)(void))EnableMacaInt;
  pfDisableMacaInt    = (void(*)(void))DisableMacaInt;
  pfIsMacaIntEnabled  = (unsigned int(*)(void))IsMacaIntEnabled;
  pfIsMacaIntAsserted = (unsigned int(*)(void))IsMacaIntAsserted;

  pfDisableProccessorIrqs = (int(*)(void))IntDisableAll;
  pfEnableProccessorIrqs = (void(*)(int))IntRestoreAll;

  /* Set priority and handler for Crm, timer, and maca*/

  IntAssignHandler(gTmrInt_c, (IntHandlerFunc_t)TmrIsr);  
  ITC_SetPriority(gTmrInt_c, gItcFastPriority_c);
  ITC_EnableInterrupt(gTmrInt_c);
  
  IntAssignHandler(gCrmInt_c, (IntHandlerFunc_t)CRM_Isr);
  ITC_SetPriority(gCrmInt_c, gItcFastPriority_c);
  ITC_EnableInterrupt(gCrmInt_c);
    
  IntAssignHandler(gMacaInt_c, MACA_Interrupt);
  ITC_SetPriority(gMacaInt_c, gItcFastPriority_c);
  ITC_EnableInterrupt(gMacaInt_c);

}

/***********************************************************************************/
void EnableMacaInt(void)
{
  ITC_EnableInterrupt(gMacaInt_c);
}

/***********************************************************************************/
void DisableMacaInt(void)
{
  ITC_DisableInterrupt(gMacaInt_c);
}

/***********************************************************************************/
unsigned int IsMacaIntEnabled(void)
{
  return ITC_GetIntEnable(gMacaInt_c);
}

/***********************************************************************************/
unsigned int IsMacaIntAsserted(void)
{
  return ITC_GetIntSrc(gMacaInt_c);
}
/***********************************************************************************/
