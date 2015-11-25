/******************************************************************************
* This is the Header file for the initialisation of the BeeStack Module.
*
*
* ( c ) Copyright 2005, Freescale, Inc. All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
******************************************************************************/
#ifndef _BEESTACK_INIT_H_
#define _BEESTACK_INIT_H_

#include "EmbeddedTypes.h"
#include "BeeStack_Globals.h"
#include "BeeStackConfiguration.h"
#include "applicationconf.h"

/******************************************************************************
*******************************************************************************
* Public Macros
*******************************************************************************
******************************************************************************/
#ifdef gHostApp_d
#ifdef gLpmIncluded_d 
#undef gLpmIncluded_d
#endif
#define gLpmIncluded_d     0 
#endif

#if (gEndDevCapability_d == 1)  
/*  Default options for a ZED to have the Radio turn off gRxOnWhenIdle_d = 0
  and LowPower is desable  gLpmIncluded_d = 0*/
  #ifndef gLpmIncluded_d
      #if (gRxOnWhenIdle_d == 1)
        #define gLpmIncluded_d     0
      #else
        #define gLpmIncluded_d     1
      #endif       
  #endif /* #ifndef gLpmIncluded_d */
#elif gComboDeviceCapability_d
/* Always set to TRUE. This will be controled in runtime */
#ifndef gLpmIncluded_d
#if (!gRxOnWhenIdle_d)
      #define gLpmIncluded_d  1  /* Low power management enabled */
#endif
#endif  

#else
/* For any ZR and ZC ensure to have the radio on and Low Power Disable*/
  #ifdef gRxOnWhenIdle_d
     #undef gRxOnWhenIdle_d
  #endif
  #define gRxOnWhenIdle_d 1  /* Radio is on */
  #define gLpmIncluded_d  0  /* Low power management disabled */
#endif /* (gEndDevCapability_d == 1)  */


/******************************************************************************
*******************************************************************************
* Public Prototypes
*******************************************************************************
******************************************************************************/
/******************************************************************************
*  This function
*
* Interface assumptions:   
*   													  
*
* Return value: void
*   
* 
******************************************************************************/
void BeeStackInit
  ( 
  void 
  );
/******************************************************************************
*******************************************************************************
* Public type definitions
*******************************************************************************
******************************************************************************/
/* None */
/******************************************************************************
*******************************************************************************
* Public Memory Declarations
*******************************************************************************
******************************************************************************/
/* None */
/******************************************************************************
*******************************************************************************
* Public functions
*******************************************************************************
******************************************************************************/
/* None */
#endif /*_BEESTACK_INIT_H_*/
