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
#include "EmbeddedTypes.h"
#include "Delay.h"
#include "AppToPlatformConfig.h"    /* SYSTEM_CLOCK */

#define  __PLACE_CONSTANT_ASM(timebase)   __asm("DC32 "#timebase"\n")
#define  __PLACE_CONSTANT(timebase)       __PLACE_CONSTANT_ASM(timebase)

/***********************************************************************************/
#if (__CPU_MODE__ == 1)    /*  Thumb mode */

  /***********************************************************************************/
  /* Delay us function for THUMB Mode. Written in assembly to avoid compiler optimizations (speed/size) */
  void DelayUs(uint32_t us)
  {
    __asm("cmp r0, #0");
    __asm("beq us_func_end");
    __asm("push {r0,r1,r2}");
    __asm("us_func_start: ldr r2, [PC, #4]");
    __asm("b us_loop_start");
    __asm("nop");
    __PLACE_CONSTANT(SYSTEM_CLOCK/6000);
    
    __asm("us_loop_start: movs r1, r2");
    __asm("us_loop_start_r1: subs r1, r1, #1");
    __asm("cmp r1, #0");
    __asm("bne us_loop_start_r1");
    
    __asm("subs r0, r0, #1");
    __asm("cmp r0, #0");
    __asm("bne us_loop_start");
    
    __asm("us_func_end:  pop{r2} \n pop {r1} \n pop {r0}");
  }
  /***********************************************************************************/

  /* Delay ms function for THUMB Mode. Written in assembly to avoid compiler optimizations (speed/size) */
  void DelayMs(uint32_t ms)
  {
    __asm("cmp r0, #0");
    __asm("beq ms_func_end");
    __asm("push {r0,r1,r2}");
    __asm("ms_func_start: ldr r2, [PC, #4]");
    __asm("b ms_loop_start");
    __asm("nop");
    __PLACE_CONSTANT(SYSTEM_CLOCK/5);
    
    __asm("ms_loop_start: movs r1, r2");
    __asm("ms_loop_start_r1: subs r1, r1, #1");
    __asm("cmp r1, #0");
    __asm("bne ms_loop_start_r1");
    
    __asm("subs r0, r0, #1");
    __asm("cmp r0, #0");
    __asm("bne ms_loop_start");
    
    __asm("ms_func_end:  pop{r2} \n pop {r1} \n pop {r0}");
  }
  /***********************************************************************************/
#elif (__CPU_MODE__ == 2)   /* ARM mode */

  /***********************************************************************************/
  /* Delay us function for ARM Mode. Written in assembly to avoid compiler optimizations (speed/size) */
  void DelayUs(uint32_t us)
  {
    __asm("cmp r0, #0");
    __asm("beq us_func_end");
    __asm("push {r0,r1,r2}");
    __asm("us_func_start: ldr r2, [PC, #4]");
    __asm("b us_loop_start");
    __asm("nop");
    __PLACE_CONSTANT(SYSTEM_CLOCK/5000);
    
    __asm("us_loop_start: mov r1, r2");
    __asm("us_loop_start_r1: sub r1, r1, #1");
    __asm("cmp r1, #0");
    __asm("bne us_loop_start_r1");
    
    __asm("sub r0, r0, #1");
    __asm("cmp r0, #0");
    __asm("bne us_loop_start");
    
    __asm("us_func_end:  pop{r2} \n pop {r1} \n pop {r0}");
  }
  /***********************************************************************************/
  /* Delay ms function for ARM Mode. Written in assembly to avoid compiler optimizations (speed/size) */
  void DelayMs(uint32_t ms)
  {
    __asm("cmp r0, #0");
    __asm("beq ms_func_end");
    __asm("push {r0,r1,r2}");
    __asm("ms_func_start: ldr r2, [PC, #4]");
    __asm("b ms_loop_start");
    __asm("nop");
    __PLACE_CONSTANT(SYSTEM_CLOCK/5);
    
    __asm("ms_loop_start: mov r1, r2");
    __asm("ms_loop_start_r1: subs r1, r1, #1");
    __asm("cmp r1, #0");
    __asm("bne ms_loop_start_r1");
    
    __asm("sub r0, r0, #1");
    __asm("cmp r0, #0");
    __asm("bne ms_loop_start");
    
    __asm("ms_func_end:  pop{r2} \n pop {r1} \n pop {r0}");
  }
  /***********************************************************************************/

#endif /* __CPU_MODE__*/
