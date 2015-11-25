/************************************************************************************
* This file is owned by the platform/application specific software and basically 
* defines how the 802.15.4 FreeScale MAC is configured. The file is included by the
* relevant MAC modules and is necessary for the MAC to compile.
*
* Author(s):
*
* (c) Copyright 2005, Freescale, Inc.  All rights reserved.
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale.
*
* Last Inspected:
* Last Tested:
************************************************************************************/

#ifndef _PLATFORMTOMACPHYCONFIG_H_
#define _PLATFORMTOMACPHYCONFIG_H_

  // Specify if placing of execution speed demanding variables in fast memory is 
  // supported. Currently the variables for the AES (sequrity engine) are placed
  // in fast RAM if it is supported.
#define gFastRamSupported_d 0  // Set to 1 to notify the MAC/PHY that fast memory is supported.
#define gFastRamBegin_d
#define gFastRamEnd_d

#define gVersionStringCapability_d 0
#define gVersionStringMacBegin_d
#define gVersionStringMacEnd_d
#define gVersionStringPhyBegin_d
#define gVersionStringPhyEnd_d


#endif /* _PLATFORMTOMACPHYCONFIG_H_ */
