/************************************************************************************
* This is a global header file for setting up the functionalities that should be
* included in the build.
*
* (c) Copyright 2007, Freescale, Inc.  All rights reserved.
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
************************************************************************************/

#ifndef _FUNCTIONALITY_DEFINES_H_
#define _FUNCTIONALITY_DEFINES_H_

//#include "PlatformToMacPhyConfig.h"
#include "AppToMacPhyConfig.h"

  // All device types: (are defined because of HC08 compiler problems only)
#define gDeviceTypeRFDNBNS_d  1
#define gDeviceTypeRFDNBNV_d  2
#define gDeviceTypeRFDNB_d    3
#define gDeviceTypeRFD_d      4
#define gDeviceTypeFFDNBNS_d  5
#define gDeviceTypeFFDNBNV_d  6
#define gDeviceTypeFFDNB_d    7
#define gDeviceTypeFFDNGTS_d  8
#define gDeviceTypeFFD_d      9
#define gDeviceTypeFFDPNBNV_d 10

#define gDeviceTypeFFDZSNGTSNV_d    11
#define gDeviceTypeFFDZSNGTSNVNS_d  12
#define gDeviceTypeFFDNGTSNVNS_d    13
#define gDeviceTypeFFDZSNBNV_d      14
#define gDeviceTypeFFDZSNBNVNS_d    15
#define gDeviceTypeFFDNBNVNS_d      16

#define gDeviceTypeFFDPZSNGTSNV_d   17
#define gDeviceTypeFFDPZSNGTSNVNS_d 18
#define gDeviceTypeFFDPNGTSNVNS_d   19
#define gDeviceTypeFFDPZSNBNV_d     20
#define gDeviceTypeFFDPZSNBNVNS_d   21
#define gDeviceTypeFFDPNBNVNS_d     22

#define gDeviceTypeRFDZSNV_d        23
#define gDeviceTypeRFDZSNVNS_d      24
#define gDeviceTypeRFDNVNS_d        25
#define gDeviceTypeRFDZSNBNV_d      26
#define gDeviceTypeRFDZSNBNVNS_d    27
#define gDeviceTypeRFDNBNVNS_d      28


// Target specific compiler defines (no value) "Type_..."
/*
#ifdef Type_RFDNBNS
	#define gDeviceType_d gDeviceTypeRFDNBNS_d
	#define DEVICE_TYPE "RFDNBNS"
#endif Type_RFDNBNS

#ifdef Type_RFDNBNV
	#define gDeviceType_d gDeviceTypeRFDNBNV_d
	#define DEVICE_TYPE "RFDNBNV"
#endif Type_RFDNBNV

#ifdef  Type_RFDNB
	#define gDeviceType_d gDeviceTypeRFDNB_d
	#define DEVICE_TYPE "RFDNB"
#endif  Type_RFDNB

#ifdef Type_RFD
	#define gDeviceType_d gDeviceTypeRFD_d
	#define DEVICE_TYPE "RFD  "
#endif Type_RFD

#ifdef Type_FFDNBNS
	#define gDeviceType_d gDeviceTypeFFDNBNS_d
	#define DEVICE_TYPE "FFDNBNS"
#endif Type_FFDNBNS

#ifdef Type_FFDNBNV
	#define gDeviceType_d gDeviceTypeFFDNBNV_d
	#define DEVICE_TYPE "FFDNBNV"
#endif Type_FFDNBNV

#ifdef Type_FFDNB
	#define gDeviceType_d gDeviceTypeFFDNB_d
	#define DEVICE_TYPE "FFDNB"
#endif Type_FFDNB

#ifdef Type_FFDNGTS
	#define gDeviceType_d gDeviceTypeFFDNGTS_d
	#define DEVICE_TYPE "FFDNGTS"
#endif Type_FFDNGTS
*/

#ifdef Type_FFD
	#define gDeviceType_d gDeviceTypeFFD_d
	#define DEVICE_TYPE "FFD  "
#endif /* Type_FFD */

/*
#ifdef Type_FFDPNBNV
	#define gDeviceType_d gDeviceTypeFFDPNBNV_d
	#define DEVICE_TYPE "FFPNBNV"
#endif Type_FFDPNBNV

// New stack target specific compiler defines (no value) "Type_..."

// FFD types

#ifdef Type_FFDZSNGTSNV
	#define gDeviceType_d gDeviceTypeFFDZSNGTSNV_d
	#define DEVICE_TYPE "FFD    "
#endif Type_FFDZSNGTSNV

#ifdef Type_FFDZSNGTSNVNS
	#define gDeviceType_d gDeviceTypeFFDZSNGTSNVNS_d
	#define DEVICE_TYPE "FFD    "
#endif Type_FFDZSNGTSNVNS

#ifdef Type_FFDNGTSNVNS
	#define gDeviceType_d gDeviceTypeFFDNGTSNVNS_d
	#define DEVICE_TYPE "FFD    "
#endif Type_FFDNGTSNVNS

#ifdef Type_FFDZSNBNV
	#define gDeviceType_d gDeviceTypeFFDZSNBNV_d
	#define DEVICE_TYPE "FFD    "
#endif Type_FFDZSNBNV

#ifdef Type_FFDZSNBNVNS
	#define gDeviceType_d gDeviceTypeFFDZSNBNVNS_d
	#define DEVICE_TYPE "FFD    "
#endif Type_FFDZSNBNVNS

#ifdef Type_FFDNBNVNS
	#define gDeviceType_d gDeviceTypeFFDNBNVNS_d
	#define DEVICE_TYPE "FFD    "
#endif Type_FFDNBNVNS

// FFDP types

#ifdef Type_FFDPZSNGTSNV
	#define gDeviceType_d gDeviceTypeFFDPZSNGTSNV_d
	#define DEVICE_TYPE "FFDP   "
#endif Type_FFDPZSNGTSNV

#ifdef Type_FFDPZSNGTSNVNS
	#define gDeviceType_d gDeviceTypeFFDPZSNGTSNVNS_d
	#define DEVICE_TYPE "FFDP   "
#endif Type_FFDPZSNGTSNVNS

#ifdef Type_FFDPNGTSNVNS
	#define gDeviceType_d gDeviceTypeFFDPNGTSNVNS_d
	#define DEVICE_TYPE "FFDP   "
#endif Type_FFDPNGTSNVNS

#ifdef Type_FFDPZSNBNV
	#define gDeviceType_d gDeviceTypeFFDPZSNBNV_d
	#define DEVICE_TYPE "FFDP   "
#endif Type_FFDPZSNBNV

#ifdef Type_FFDPZSNBNVNS
	#define gDeviceType_d gDeviceTypeFFDPZSNBNVNS_d
	#define DEVICE_TYPE "FFDP   "
#endif Type_FFDPZSNBNVNS

#ifdef Type_FFDPNBNVNS
	#define gDeviceType_d gDeviceTypeFFDPNBNVNS_d
	#define DEVICE_TYPE "FFDP   "
#endif Type_FFDPNBNVNS

// RFD types

#ifdef Type_RFDZSNV
	#define gDeviceType_d gDeviceTypeRFDZSNV_d
	#define DEVICE_TYPE "RFD    "
#endif Type_RFDZSNV

#ifdef Type_RFDZSNVNS
	#define gDeviceType_d gDeviceTypeRFDZSNVNS_d
	#define DEVICE_TYPE "RFD    "
#endif Type_RFDZSNGTSNVNS

#ifdef Type_RFDNVNS
	#define gDeviceType_d gDeviceTypeRFDNVNS_d
	#define DEVICE_TYPE "RFD    "
#endif Type_RFDNVNS

#ifdef Type_RFDZSNBNV
	#define gDeviceType_d gDeviceTypeRFDZSNBNV_d
	#define DEVICE_TYPE "RFD    "
#endif Type_RFDZSNBNV

#ifdef Type_RFDZSNBNVNS
	#define gDeviceType_d gDeviceTypeRFDZSNBNVNS_d
	#define DEVICE_TYPE "RFD    "
#endif Type_RFDZSNBNVNS

#ifdef Type_RFDNBNVNS
	#define gDeviceType_d gDeviceTypeRFDNBNVNS_d
	#define DEVICE_TYPE "RFD    "
#endif Type_RFDNBNVNS
*/

#ifndef gDeviceType_d // If device type is not predefined use the following definitions:

  #define DEVICE_TYPE "XXXXXXX" // Max size

  /**********************************************************************************
  ***********************************************************************************
  * Role capability definitions
  ***********************************************************************************
  **********************************************************************************/

    // Set if the device need to be able to take on the device role
  #define gDeviceCapability_d 1

    // Set if the device need to be able to take on the coordinator role
  #define gCoorCapability_d 1

  /**********************************************************************************
  ***********************************************************************************
  * Functionality inclusion definitions that depend on device type
  ***********************************************************************************
  **********************************************************************************/

    // Set if the device need to be able to take on the pan coordinator role
    // Is never set unless coordinator capability is set
  #define gPanCoorCapability_d (1 && gCoorCapability_d)

    // Set if device (in role of either device or ccordinator) supports beaconed
    // operation. Note that ...
  #define gBeaconedCapability_d 1

    // Set if device (in role of either device or PAN cordinator) supports GTS in
    // beaconed operation. Is never set unless beaconed operation is supported
  #define gGtsCapability_d (1 && gBeaconedCapability_d && (gDeviceCapability_d || gPanCoorCapability_d))

    // SCAN functionalities - optional for RFD and below (=spec. RFD)
  #define gEnergyScanCapability_d 1
  #define gActiveScanCapability_d 1

    // MCPS-PURGE.request capabilities
  #define gPurgeCapability_d 1
  
    // Set if security is supported in the MAC/PHY
  #define gSecurityCapability_d 1

    // Define to 0 to disabled or 1 to enable verification of NWK to MLME primitives
  #define gVerifyParamCapability_d 1

    // Set if the common security library should be included.
  #define gSecurityLibraryCapability_d 0

  // Set if the disassociation capability should be included
  #define gDisassociateCapability_d 1

#else // If gDeviceType_d is defined use one of the following presets:

  #if gDeviceType_d == gDeviceTypeFFD_d
    #define gDeviceCapability_d      1
    #define gCoorCapability_d        1
    #define gPanCoorCapability_d     1
    #define gBeaconedCapability_d    1
    #define gGtsCapability_d         1
    #define gEnergyScanCapability_d  1
    #define gActiveScanCapability_d  1
    #define gPurgeCapability_d       1
    #define gSecurityCapability_d    1
    #define gVerifyParamCapability_d 1
    #define gSecurityLibraryCapability_d 0
    #define gDisassociateCapability_d 1	
  #elif gDeviceType_d == gDeviceTypeFFDNGTS_d
    #define gDeviceCapability_d      1
    #define gCoorCapability_d        1
    #define gPanCoorCapability_d     1
    #define gBeaconedCapability_d    1
    #define gGtsCapability_d         0
    #define gEnergyScanCapability_d  1
    #define gActiveScanCapability_d  1
    #define gPurgeCapability_d       1
    #define gSecurityCapability_d    1
    #define gVerifyParamCapability_d 1
    #define gSecurityLibraryCapability_d 0
    #define gDisassociateCapability_d 1
  #elif gDeviceType_d == gDeviceTypeFFDNB_d
    #define gDeviceCapability_d      1
    #define gCoorCapability_d        1
    #define gPanCoorCapability_d     1
    #define gBeaconedCapability_d    0
    #define gGtsCapability_d         0
    #define gEnergyScanCapability_d  1
    #define gActiveScanCapability_d  1
    #define gPurgeCapability_d       1
    #define gSecurityCapability_d    1
    #define gVerifyParamCapability_d 1
    #define gSecurityLibraryCapability_d 0
    #define gDisassociateCapability_d 1
  #elif gDeviceType_d == gDeviceTypeFFDNBNS_d
    #define gDeviceCapability_d      1
    #define gCoorCapability_d        1
    #define gPanCoorCapability_d     1
    #define gBeaconedCapability_d    0
    #define gGtsCapability_d         0
    #define gEnergyScanCapability_d  1
    #define gActiveScanCapability_d  1
    #define gPurgeCapability_d       1
    #define gSecurityCapability_d    0
    #define gVerifyParamCapability_d 1
    #define gSecurityLibraryCapability_d 0
    #define gDisassociateCapability_d 1
  #elif gDeviceType_d == gDeviceTypeFFDNBNV_d
    #define gDeviceCapability_d      1
    #define gCoorCapability_d        1
    #define gPanCoorCapability_d     1
    #define gBeaconedCapability_d    0
    #define gGtsCapability_d         0
    #define gEnergyScanCapability_d  1
    #define gActiveScanCapability_d  1
    #define gPurgeCapability_d       1
    #define gSecurityCapability_d    1
    #define gVerifyParamCapability_d 0
    #define gSecurityLibraryCapability_d 0
    #define gDisassociateCapability_d 1
  #elif gDeviceType_d == gDeviceTypeRFD_d
    #define gDeviceCapability_d      1
    #define gCoorCapability_d        0
    #define gPanCoorCapability_d     0
    #define gBeaconedCapability_d    1
    #define gGtsCapability_d         0
    #define gEnergyScanCapability_d  0
    #define gActiveScanCapability_d  1
    #define gPurgeCapability_d       0 
    #define gSecurityCapability_d    1
    #define gVerifyParamCapability_d 1
    #define gSecurityLibraryCapability_d 0
    #define gDisassociateCapability_d 1
  #elif gDeviceType_d == gDeviceTypeRFDNB_d
    #define gDeviceCapability_d      1
    #define gCoorCapability_d        0
    #define gPanCoorCapability_d     0
    #define gBeaconedCapability_d    0
    #define gGtsCapability_d         0
    #define gEnergyScanCapability_d  0
    #define gActiveScanCapability_d  1
    #define gPurgeCapability_d       0 
    #define gSecurityCapability_d    1
    #define gVerifyParamCapability_d 1
    #define gSecurityLibraryCapability_d 0
    #define gDisassociateCapability_d 1
  #elif gDeviceType_d == gDeviceTypeRFDNBNS_d
    #define gDeviceCapability_d      1
    #define gCoorCapability_d        0
    #define gPanCoorCapability_d     0
    #define gBeaconedCapability_d    0
    #define gGtsCapability_d         0
    #define gEnergyScanCapability_d  0
    #define gActiveScanCapability_d  1
    #define gPurgeCapability_d       0 
    #define gSecurityCapability_d    0
    #define gVerifyParamCapability_d 1
    #define gSecurityLibraryCapability_d 0
    #define gDisassociateCapability_d 1
  #elif gDeviceType_d == gDeviceTypeRFDNBNV_d
    #define gDeviceCapability_d      1
    #define gCoorCapability_d        0
    #define gPanCoorCapability_d     0
    #define gBeaconedCapability_d    0
    #define gGtsCapability_d         0
    #define gEnergyScanCapability_d  0
    #define gActiveScanCapability_d  1
    #define gPurgeCapability_d       0 
    #define gSecurityCapability_d    1
    #define gVerifyParamCapability_d 0
    #define gSecurityLibraryCapability_d 0
    #define gDisassociateCapability_d 1
  #elif gDeviceType_d == gDeviceTypeFFDPNBNV_d
    #define gDeviceCapability_d      0
    #define gCoorCapability_d        1
    #define gPanCoorCapability_d     1
    #define gBeaconedCapability_d    0
    #define gGtsCapability_d         0
    #define gEnergyScanCapability_d  1
    #define gActiveScanCapability_d  1
    #define gPurgeCapability_d       1
    #define gSecurityCapability_d    1
    #define gVerifyParamCapability_d 0
    #define gSecurityLibraryCapability_d 0
    #define gDisassociateCapability_d 1
	#elif gDeviceType_d == gDeviceTypeFFDZSNGTSNV_d // New stack types: FFD
    #define gDeviceCapability_d      1
    #define gCoorCapability_d        1
    #define gPanCoorCapability_d     1
    #define gBeaconedCapability_d    1
    #define gGtsCapability_d         0
    #define gEnergyScanCapability_d  1
    #define gActiveScanCapability_d  1
    #define gPurgeCapability_d       1
    #define gSecurityCapability_d    1
    #define gVerifyParamCapability_d 0
    #define gSecurityLibraryCapability_d 1
    #define gDisassociateCapability_d 1
	#elif gDeviceType_d == gDeviceTypeFFDZSNGTSNVNS_d
    #define gDeviceCapability_d      1
    #define gCoorCapability_d        1
    #define gPanCoorCapability_d     1
    #define gBeaconedCapability_d    1
    #define gGtsCapability_d         0
    #define gEnergyScanCapability_d  1
    #define gActiveScanCapability_d  1
    #define gPurgeCapability_d       1
    #define gSecurityCapability_d    0
    #define gVerifyParamCapability_d 0
    #define gSecurityLibraryCapability_d 1
    #define gDisassociateCapability_d 1
	#elif gDeviceType_d == gDeviceTypeFFDNGTSNVNS_d
    #define gDeviceCapability_d      1
    #define gCoorCapability_d        1
    #define gPanCoorCapability_d     1
    #define gBeaconedCapability_d    1
    #define gGtsCapability_d         0
    #define gEnergyScanCapability_d  1
    #define gActiveScanCapability_d  1
    #define gPurgeCapability_d       1
    #define gSecurityCapability_d    0
    #define gVerifyParamCapability_d 0
    #define gSecurityLibraryCapability_d 0
    #define gDisassociateCapability_d 1
	#elif gDeviceType_d == gDeviceTypeFFDZSNBNV_d
    #define gDeviceCapability_d      1
    #define gCoorCapability_d        1
    #define gPanCoorCapability_d     1
    #define gBeaconedCapability_d    0
    #define gGtsCapability_d         0
    #define gEnergyScanCapability_d  1
    #define gActiveScanCapability_d  1
    #define gPurgeCapability_d       1
    #define gSecurityCapability_d    1
    #define gVerifyParamCapability_d 0
    #define gSecurityLibraryCapability_d 1
    #define gDisassociateCapability_d 1
	#elif gDeviceType_d == gDeviceTypeFFDZSNBNVNS_d
    #define gDeviceCapability_d      1
    #define gCoorCapability_d        1
    #define gPanCoorCapability_d     1
    #define gBeaconedCapability_d    0
    #define gGtsCapability_d         0
    #define gEnergyScanCapability_d  1
    #define gActiveScanCapability_d  1
    #define gPurgeCapability_d       1
    #define gSecurityCapability_d    0
    #define gVerifyParamCapability_d 0
    #define gSecurityLibraryCapability_d 1
    #define gDisassociateCapability_d 1
	#elif gDeviceType_d == gDeviceTypeFFDNBNVNS_d
    #define gDeviceCapability_d      1
    #define gCoorCapability_d        1
    #define gPanCoorCapability_d     1
    #define gBeaconedCapability_d    0
    #define gGtsCapability_d         0
    #define gEnergyScanCapability_d  1
    #define gActiveScanCapability_d  1
    #define gPurgeCapability_d       1
    #define gSecurityCapability_d    0
    #define gVerifyParamCapability_d 0
    #define gSecurityLibraryCapability_d 0
    #define gDisassociateCapability_d 1
	#elif gDeviceType_d == gDeviceTypeFFDPZSNGTSNV_d // New stack types: FFDP
    #define gDeviceCapability_d      0
    #define gCoorCapability_d        1
    #define gPanCoorCapability_d     1
    #define gBeaconedCapability_d    1
    #define gGtsCapability_d         0
    #define gEnergyScanCapability_d  1
    #define gActiveScanCapability_d  1
    #define gPurgeCapability_d       1
    #define gSecurityCapability_d    1
    #define gVerifyParamCapability_d 0
    #define gSecurityLibraryCapability_d 1
    #define gDisassociateCapability_d 1
	#elif gDeviceType_d == gDeviceTypeFFDPZSNGTSNVNS_d
    #define gDeviceCapability_d      0
    #define gCoorCapability_d        1
    #define gPanCoorCapability_d     1
    #define gBeaconedCapability_d    1
    #define gGtsCapability_d         0
    #define gEnergyScanCapability_d  1
    #define gActiveScanCapability_d  1
    #define gPurgeCapability_d       1
    #define gSecurityCapability_d    0
    #define gVerifyParamCapability_d 0
    #define gSecurityLibraryCapability_d 1
    #define gDisassociateCapability_d 1
	#elif gDeviceType_d == gDeviceTypeFFDPNGTSNVNS_d
    #define gDeviceCapability_d      0
    #define gCoorCapability_d        1
    #define gPanCoorCapability_d     1
    #define gBeaconedCapability_d    1
    #define gGtsCapability_d         0
    #define gEnergyScanCapability_d  1
    #define gActiveScanCapability_d  1
    #define gPurgeCapability_d       1
    #define gSecurityCapability_d    0
    #define gVerifyParamCapability_d 0
    #define gSecurityLibraryCapability_d 0
    #define gDisassociateCapability_d 1
	#elif gDeviceType_d == gDeviceTypeFFDPZSNBNV_d
    #define gDeviceCapability_d      0
    #define gCoorCapability_d        1
    #define gPanCoorCapability_d     1
    #define gBeaconedCapability_d    0
    #define gGtsCapability_d         0
    #define gEnergyScanCapability_d  1
    #define gActiveScanCapability_d  1
    #define gPurgeCapability_d       1
    #define gSecurityCapability_d    1
    #define gVerifyParamCapability_d 0
    #define gSecurityLibraryCapability_d 1
    #define gDisassociateCapability_d 1
	#elif gDeviceType_d == gDeviceTypeFFDPZSNBNVNS_d
    #define gDeviceCapability_d      0
    #define gCoorCapability_d        1
    #define gPanCoorCapability_d     1
    #define gBeaconedCapability_d    0
    #define gGtsCapability_d         0
    #define gEnergyScanCapability_d  1
    #define gActiveScanCapability_d  1
    #define gPurgeCapability_d       1
    #define gSecurityCapability_d    0
    #define gVerifyParamCapability_d 0
    #define gSecurityLibraryCapability_d 1
    #define gDisassociateCapability_d 1
	#elif gDeviceType_d == gDeviceTypeFFDPNBNVNS_d
    #define gDeviceCapability_d      0
    #define gCoorCapability_d        1
    #define gPanCoorCapability_d     1
    #define gBeaconedCapability_d    0
    #define gGtsCapability_d         0
    #define gEnergyScanCapability_d  1
    #define gActiveScanCapability_d  1
    #define gPurgeCapability_d       1
    #define gSecurityCapability_d    0
    #define gVerifyParamCapability_d 0
    #define gSecurityLibraryCapability_d 0
    #define gDisassociateCapability_d 1
	#elif gDeviceType_d == gDeviceTypeRFDZSNV_d // New stack types: RFD
    #define gDeviceCapability_d      1
    #define gCoorCapability_d        0
    #define gPanCoorCapability_d     0
    #define gBeaconedCapability_d    1
    #define gGtsCapability_d         0
    #define gEnergyScanCapability_d  0
    #define gActiveScanCapability_d  1
    #define gPurgeCapability_d       0
    #define gSecurityCapability_d    1
    #define gVerifyParamCapability_d 0
    #define gSecurityLibraryCapability_d 1
    #define gDisassociateCapability_d 1
	#elif gDeviceType_d == gDeviceTypeRFDZSNVNS_d
    #define gDeviceCapability_d      1
    #define gCoorCapability_d        0
    #define gPanCoorCapability_d     0
    #define gBeaconedCapability_d    1
    #define gGtsCapability_d         0
    #define gEnergyScanCapability_d  0
    #define gActiveScanCapability_d  1
    #define gPurgeCapability_d       0
    #define gSecurityCapability_d    0
    #define gVerifyParamCapability_d 0
    #define gSecurityLibraryCapability_d 1
    #define gDisassociateCapability_d 1
	#elif gDeviceType_d == gDeviceTypeRFDNVNS_d
    #define gDeviceCapability_d      1
    #define gCoorCapability_d        0
    #define gPanCoorCapability_d     0
    #define gBeaconedCapability_d    1
    #define gGtsCapability_d         0
    #define gEnergyScanCapability_d  0
    #define gActiveScanCapability_d  1
    #define gPurgeCapability_d       0
    #define gSecurityCapability_d    0
    #define gVerifyParamCapability_d 0
    #define gSecurityLibraryCapability_d 0
    #define gDisassociateCapability_d 1
	#elif gDeviceType_d == gDeviceTypeRFDZSNBNV_d
    #define gDeviceCapability_d      1
    #define gCoorCapability_d        0
    #define gPanCoorCapability_d     0
    #define gBeaconedCapability_d    0
    #define gGtsCapability_d         0
    #define gEnergyScanCapability_d  0
    #define gActiveScanCapability_d  1
    #define gPurgeCapability_d       0
    #define gSecurityCapability_d    1
    #define gVerifyParamCapability_d 0
    #define gSecurityLibraryCapability_d 1
    #define gDisassociateCapability_d 1
	#elif gDeviceType_d == gDeviceTypeRFDZSNBNVNS_d
    #define gDeviceCapability_d      1
    #define gCoorCapability_d        0
    #define gPanCoorCapability_d     0
    #define gBeaconedCapability_d    0
    #define gGtsCapability_d         0
    #define gEnergyScanCapability_d  0
    #define gActiveScanCapability_d  1
    #define gPurgeCapability_d       0
    #define gSecurityCapability_d    0
    #define gVerifyParamCapability_d 0
    #define gSecurityLibraryCapability_d 1
    #define gDisassociateCapability_d 1
	#elif gDeviceType_d == gDeviceTypeRFDNBNVNS_d
    #define gDeviceCapability_d      1
    #define gCoorCapability_d        0
    #define gPanCoorCapability_d     0
    #define gBeaconedCapability_d    0
    #define gGtsCapability_d         0
    #define gEnergyScanCapability_d  0
    #define gActiveScanCapability_d  1
    #define gPurgeCapability_d       0
    #define gSecurityCapability_d    0
    #define gVerifyParamCapability_d 0
    #define gSecurityLibraryCapability_d 0
    #define gDisassociateCapability_d 1
  #else
    #error Invalid gDeviceType_d define value
  #endif // gDeviceType_d
#endif // !gDeviceType_d

/************************************************************************************
*************************************************************************************
* Functionality inclusion definitions that are not directly dependant on device type
*************************************************************************************
************************************************************************************/

  // Set if ASP features are supported
#define gAspPowerSaveCapability_d   (1 && gAspCapability_d)
#define gAspEventCapability_d       (1 && gAspCapability_d)
#define gAspHwCapability_d          (1 && gAspCapability_d)
#define gAspPowerLevelCapability_d  (1 && gAspCapability_d)

  // Set to 1 if new disassociate request primitive must be supported. Does not require additional code.
#define gNewDisassociateReq_d 0

  // Set to 0 if you do not want random initialisation of macDSN, macBSN, and the random backoff seed
#if !defined gInitializeWithRandomSeed_d
#define gInitializeWithRandomSeed_d 1
#endif /* gInitializeWithRandomSeed_d */

  // Set to one to support the RX Enable feature
#define gRxEnableCapability_d 1

#if gGtsCapability_d==1
#undef gGtsCapability_d
#define gGtsCapability_d 1   // Master switch for GTS feature
#endif // gGtsCapability_d


/************************************************************************************
*************************************************************************************
* Proprietary stuff... 
*************************************************************************************
************************************************************************************/

  // Treemode capability
#define gTreemodeCapability_d (1 && gCoorCapability_d && gDeviceCapability_d && gBeaconedCapability_d)

  // Beacon start indications
#define gBeaconStartIndicationCapability_d 0 // Used to be: (0 || gTreemodeCapability_d)

#endif /* _FUNCTIONALITY_DEFINES_H_ */
