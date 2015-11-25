/************************************************************************************
* This file is owned by the platform/application specific software and basically 
* defines how the 802.15.4 Freescale MAC is configured. The file is included by the
* relevant MAC modules and is necessary for the MAC to compile.
*
*
* (c) Copyright 2007, Freescale, Inc.  All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
************************************************************************************/

#ifndef _APPTOPLATFORMCONFIG_H_
#define _APPTOPLATFORMCONFIG_H_


/**********************************************************************************
  Define one Hardware platform type (if not defined default is MC1322x SRB)
**********************************************************************************/

/* Check if any target platform has been set in the project */
#if !((defined gTargetMC1322xNCB) || (defined gTargetMC1322xSRB)|| \
      (defined gTargetMC1322xUSB) || (defined gTargetMC1322xLPB))
  /* Select default target if no target settings were defined in the project */
  #define gTargetMC1322xSRB    TRUE
  #define gTargetMC1322xNCB    FALSE
  #define gTargetMC1322xUSB    FALSE
  #define gTargetMC1322xLPB    FALSE
#else
/* If a target was selected, set the others to 0 to avoid
   compiler warnings ("assuming default 0") */
  #ifndef gTargetMC1322xSRB
    #define gTargetMC1322xSRB  FALSE
  #endif

  #ifndef gTargetMC1322xNCB
    #define gTargetMC1322xNCB  FALSE
  #endif

  #ifndef gTargetMC1322xUSB
    #define gTargetMC1322xUSB  FALSE
  #endif

  #ifndef gTargetMC1322xLPB
    #define gTargetMC1322xLPB  FALSE
  #endif

#endif /* !((defined gTargetMC1322xNCB) || (defined gTargetMC1322xSRB)) || (defined gTargetMC1322xUSB) || (defined gTargetMC1322xLPB)) */

/* System Clock for MC1322x in kHz */
#ifndef SYSTEM_CLOCK 
  #define SYSTEM_CLOCK 24000
#endif /* SYSTEM_CLOCK */

//**********************************************************************************

#endif /* _APPTOPLATFORMCONFIG_H_ */

