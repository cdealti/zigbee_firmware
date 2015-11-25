
/*****************************************************************************
* This is a header file which defines the constnts for ZDO layer
*
*
* (c) Copyright 2005, Freescale, Inc. All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
******************************************************************************/


#ifndef _ZdoCommon_h_
#define _ZdoCommon_h_

#include "TMR_Interface.h"

/**********************************************************************
***********************************************************************
* Public Macros
***********************************************************************
***********************************************************************/

/* Maximum size of AF data frame at AF level as specified by spec. But
application can send data frame more than 70 bytes. */
#if gStandardSecurity_d || gHighSecurity_d
  #define gAfMaxCommandSize_c 0x43
#else
  #define gAfMaxCommandSize_c 70
#endif

/**********************************************************************
***********************************************************************
* Public Prototypes
***********************************************************************
***********************************************************************/

/* None */

/**********************************************************************
***********************************************************************
* Public type definitions
***********************************************************************
***********************************************************************/

/* None */

/**********************************************************************
***********************************************************************
* Public Memory Declarations
***********************************************************************
***********************************************************************/

/*None*/

/**********************************************************************
***********************************************************************
* Public functions
***********************************************************************
**********************************************************************/

/*None*/

/**********************************************************************
***********************************************************************
* Public type definitions
***********************************************************************
***********************************************************************/

/* None */

/**********************************************************************
***********************************************************************
* Public Memory Declarations
***********************************************************************
***********************************************************************/

/* code reduction */
extern tmrTimerID_t gZdoTimerIDs[];
#define gEndDevBindTimerID            gZdoTimerIDs[0]
#define gZdoDiscoveryAttemptTimerID   gZdoTimerIDs[1]
#define gZdoFormationAttemptTimerID   gZdoTimerIDs[2] /* forming and leaving can't happen at same time */
#define gZdoLeavingTimerID            gZdoTimerIDs[2] /* forming and leaving can't happen at same time */
#define gZdoOrphanScanAttemptTimerID  gZdoTimerIDs[3]
#define gTimeBetScansTimerID          gZdoTimerIDs[4]
#define gZdoEstablishKeysTimerID      gZdoTimerIDs[5]
#define gFAChannelChangeTimerID       gZdoTimerIDs[6]
#define gNwkConcentratorDiscoveryTimerID gZdoTimerIDs[7]
#endif _ZdoCommon_h_

