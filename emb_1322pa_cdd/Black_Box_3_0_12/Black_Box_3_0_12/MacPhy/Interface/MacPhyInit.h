/************************************************************************************
* This header file is provided as part of the interface to the Freescale 802.15.4
* MAC and PHY layer.
*
* This file contains initialization related functionality.
*
* (c) Copyright 2007, Freescale, Inc.  All rights reserved.
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
************************************************************************************/
#ifndef _MACPHYINIT_H_
#define _MACPHYINIT_H_

  // The platform configuration file must be included for
  // gVersionStringCapability_d, gVersionStringBegin_d and gVersionStringEnd_d
#include "PlatformToMacPhyConfig.h"

  // Max 4 characters
#define Database_Label_Version	"214"
#define MAC_Version_No		"214"
#define PHY_Version_No		"214"

#if gVersionStringCapability_d

#pragma gVersionStringMacBegin_d
extern const uint8_t MAC_Version[47];
#pragma gVersionStringMacEnd_d

#pragma gVersionStringCopyrightBegin_d
extern const uint8_t Freescale_Copyright[54];
#pragma gVersionStringCopyrightEnd_d

#pragma gVersionStringFirmwareBegin_d
extern const uint8_t Firmware_Database_Label[38];
#pragma gVersionStringFirmwareEnd_d

#pragma gVersionStringPhyBegin_d
extern const uint8_t PHY_Version[47];
#pragma gVersionStringPhyEnd_d

#endif // gVersionStringCapability_d

/************************************************************************************
* Stack init function called from platform upon reset. 
*
* Interface assumptions:
*   Typically called from Init_802_15_4() in platform
*
* Return value:
*   None
*
************************************************************************************/
void MacPhyInit_Initialize
(
 void                            
);

/************************************************************************************
* Writes the 8 byte IEEE extended address.
*
* Interface assumptions:
*   None
*
* Return value:
*   None
*
************************************************************************************/
void MacPhyInit_WriteExtAddress
(
  uint8_t *pExtendedAddress // A pointer to the 8 byte IEEE address. The address is
                            // copied into local MAC RAM.
);

/************************************************************************************
* Reads the 8 byte IEEE extended address.
*
* Interface assumptions:
*   None
*
* Return value:
*   A pointer to the extended address.
*
************************************************************************************/
uint8_t *MacPhyInit_ReadExtAddress(void);


#endif /* _MACPHYINIT_H_ */
