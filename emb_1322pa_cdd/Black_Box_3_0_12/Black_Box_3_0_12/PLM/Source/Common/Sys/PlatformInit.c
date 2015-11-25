/************************************************************************************
*
* (c) Copyright 2007, Freescale, Inc.  All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale.
*
************************************************************************************/
#include "Platform.h"
#include "PortConfig.h"
#include "PlatformInterrupts.h"
#include "Interrupt.h"
#include "PlatformInit.h"
#include "MsgSystem.h"
#include "NwkMacInterface.h"
#include "AppAspInterface.h"
#include "NVM_Interface.h"
#include "MacPhyInit.h"
#include "GPIO_Interface.h"


/* CRM register containing the control of NVM regulator */
#define VregCntl            (*((volatile unsigned long*)(0x80003048)))
#define VREG_1P8V_EN        0x00000080UL
#define BUCK_BYPASS_EN      0x00000004UL
#define BUCK_EN             0x00000001UL


extern void MacPhyInit_Initialize(void);
extern bool_t gbTaskMode;

void Init_802_15_4(bool_t mUseTs);
static void PlatformInitGpio(void);

/**************************************************************************************/
void Platform_Init(void)
{
  Asp_TrimReq( gHardwareParameters.gXtalFTune_c, gHardwareParameters.gXtalCTune_c );

  PlatformInitGpio();
  AppInterruptsInit();  // Initializes interrupts and Itc peripheral
}

/**************************************************************************************/
/*
  Activates the NVM regulator via CRM
*/
void StartNvmReg(void)
{
  volatile uint32_t counter;

  VregCntl &= (~BUCK_EN);
  VregCntl |= BUCK_BYPASS_EN;

  counter = 18200;
  while(counter--);

  VregCntl |= VREG_1P8V_EN;
  counter = 5200;
  while(counter--);
}

/**************************************************************************************/
/*
  Stops the NVM regulator via CRM
*/
void StopNvmReg(void)
{
  volatile uint32_t counter;

  VregCntl &= ~VREG_1P8V_EN;
  counter = 5200;
  while(counter--);
}

/************************************************************************************
* Initialize platform and stack. 
*
* This function is the main initialization procedure, which will be called from the 
* startup code of the project. 
* If the code is build as a stand-alone application, this function is
* called from main() below. 
*
* Interface assumptions:
*
* Return value:
*   NONE
*
* Revision history:
*
*    Date    Author    Comments
*   ------   ------    --------
*   030505   JT        Created
*
************************************************************************************/
void Init_802_15_4(bool_t mUseTS)
{
  gbTaskMode = mUseTS;

  MacPhyInit_Initialize();      // Init 802.15.4 MAC/PHY stack

  MacPhyInit_WriteExtAddress((uint8_t*)(&(gHardwareParameters.MAC_Address[0]))); 

  Asp_SetPowerLevel(gHardwareParameters.defaultPowerLevel);
  
  {
   uint32_t reg_value =  *((volatile uint32_t*)0x80009460);
   reg_value &= (uint32_t)0xFF00FFFF; /* clear the cca threshold */
   reg_value |= (uint32_t) gHardwareParameters.ccaThreshold << 16; /* set the cca threshold */
   *((volatile uint32_t*)0x80009460) = reg_value;
  } 
}

/**************************************************************************************/

static void PlatformInitGpio(void)
{
  GPIO_REGS_P->DataLo = gDataLoValue_c;                  //MBAR_GPIO + 0x08
  GPIO_REGS_P->DataHi = gDataHiValue_c;                  //MBAR_GPIO + 0x0C

  GPIO_REGS_P->DirLo =  gDirLoValue_c;                   //MBAR_GPIO + 0x00
  GPIO_REGS_P->DirHi =  gDirHiValue_c;                   //MBAR_GPIO + 0x04

  GPIO_REGS_P->PuEnLo = gPuEnLoValue_c;                  //MBAR_GPIO + 0x10
  GPIO_REGS_P->PuEnHi = gPuEnHiValue_c;                  //MBAR_GPIO + 0x14
  GPIO_REGS_P->FuncSel0 = gFuncSel0Value_c;              //MBAR_GPIO + 0x18
  GPIO_REGS_P->FuncSel1 = gFuncSel1Value_c;              //MBAR_GPIO + 0x1C
  GPIO_REGS_P->FuncSel2 = gFuncSel2Value_c;              //MBAR_GPIO + 0x20
  GPIO_REGS_P->FuncSel3 = gFuncSel3Value_c;              //MBAR_GPIO + 0x24
  GPIO_REGS_P->InputDataSelLo = gInputDataSelLoValue_c;  //MBAR_GPIO + 0x28
  GPIO_REGS_P->InputDataSelHi = gInputDataSelHiValue_c;  //MBAR_GPIO + 0x2C
  GPIO_REGS_P->PuSelLo = gPuSelLoValue_c;                //MBAR_GPIO + 0x30
  GPIO_REGS_P->PuSelHi = gPuSelHiValue_c;                //MBAR_GPIO + 0x34
  GPIO_REGS_P->HystEnLo = gHystEnLoValue_c;              //MBAR_GPIO + 0x38
  GPIO_REGS_P->HystEnHi = gHystEnHiValue_c;              //MBAR_GPIO + 0x3C
  GPIO_REGS_P->PuKeepLo = gPuKeepLoValue_c;              //MBAR_GPIO + 0x40
  GPIO_REGS_P->PuKeepHi = gPuKeepHiValue_c;              //MBAR_GPIO + 0x44 
}
