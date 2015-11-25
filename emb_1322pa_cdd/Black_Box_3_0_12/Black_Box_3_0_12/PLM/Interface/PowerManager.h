/************************************************************************************
* MCU specific powerup/wakeup system clock checking module
*
* Author(s):
*
* (c) Copyright 2004, Freescale, Inc.  All rights reserved.
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale.
*
* Last Inspected:
* Last Tested:
************************************************************************************/
#ifndef _POWERMANAGER_H_
#define _POWERMANAGER_H_

#include "EmbeddedTypes.h"

enum {
  gSeqPowerModeAwake_c = 0,
  gSeqPowerModeDoze_c,
  gSeqPowerModeHibernate_c,
  gSeqPowerModeAcoma_c
};
typedef uint8_t powerMode_t;


/************************************************************************************
* Upon power up (or wake up) the PowerManager is envoked to ensure that the platform
* is ready to proceed. On HCS08 the power manager ensures that the mcu clock is stable
* through the HCS08 ICG module.
*   
* Interface assumptions:
*   None
*   
* Return value:
*   None
* 
* Revision history:
*   date      Author    Comments
*   ------    ------    --------
*   100505    JT        Created
* 
************************************************************************************/
//void PowerManager_WaitForPowerUp(void);
#define PowerManager_WaitForPowerUp()

/************************************************************************************
* Returns the number of symbols required by the transceiver for waking up from power
* save mode combined with an empirical determined number of symbols required by the
* MCU to be ready after loss of external clock.
*
* The returned value can be regarded as the minimum duration for doze mode.
*
* Interface assumptions:
*   If changing the return value, the MAC must be rebuild.
*   
* Return value:
*   Amount of time before system is ready after waking up from power save mode.
*   Unit is symbols (Milisecs = 16*symbols): 62->1ms, 75->1.2ms, 94->1.5ms
* 
* Revision history:
*   date      Author    Comments
*   ------    ------    --------
*   240505    BPP       Created
* 
************************************************************************************/
#define PowerManager_GetWakeUpTime()  (75) // Default is 1.2ms


/************************************************************************************
* The MAC/PHY will notify the Power Manager module each time the power mode changes
* using PowerManager_SetPowerMode(). The power mode can have one of the values
* described in powerMode_t:
*   gSeqPowerModeAwake_c, gSeqPowerModeDoze_c,
*   gSeqPowerModeHibernate_c or gSeqPowerModeAcoma_c
*
* PowerManager_SetPowerMode() must only be used by the MAC/PHY.
*
* Interface assumptions:
*   None
*   
* Return value:
*   None
* 
* Revision history:
*   date      Author    Comments
*   ------    ------    --------
*   240505    BPP       Created
* 
************************************************************************************/
#define PowerManager_SetPowerMode(powerMode) (gPowerSaveMode = (powerMode))


/************************************************************************************
* Used by the MAC/PHY to read the current power mode.
*
* Interface assumptions:
*   None
*   
* Return value:
*   Returns the power mode. Can be: gSeqPowerModeAwake_c, gSeqPowerModeDoze_c,
*   gSeqPowerModeHibernate_c or gSeqPowerModeAcoma_c.
* 
* Revision history:
*   date      Author    Comments
*   ------    ------    --------
*   240505    BPP       Created
* 
************************************************************************************/
#define PowerManager_GetPowerMode() (gPowerSaveMode)

extern uint8_t gPowerSaveMode;


#endif /* _POWERMANAGER_H_ */
