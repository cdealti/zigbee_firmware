/************************************************************************************
* This header file is provided as part of the interface to the Freescale 802.15.4
* MAC and PHY layer.
*
* The file defines constant values given by the 802.15.4 specification.
*
* (c) Copyright 2007, Freescale, Inc.  All rights reserved.
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
************************************************************************************/

#ifndef _PUB_CONST_H_
#define _PUB_CONST_H_

/************************************************************************************
*************************************************************************************
* Public macros
*************************************************************************************
************************************************************************************/

  // MAC enumerations (D18, table 64, p. 109 + table 68 p. 125)
#define gSuccess_c              0x00
#define gPanAtCapacity_c        0x01
#define gPanAccessDenied_c      0x02
#define gCounterError_c            0xDB
#define gImproperKeyType_c         0xDC
#define gImproperSecurityLevel_c   0xDD
#define gUnsupportedLegacy_c       0xDE
#define gUnsupportedSecurity_c     0xDF
#define gBeaconLoss_c           0xE0
#define gChannelAccessFailure_c 0xE1
#define gDenied_c               0xE2
#define gDisableTrxFailure_c    0xE3
#define gFailedSecurityCheck_c  0xE4
#define gFrameTooLong_c         0xE5
#define gInvalidGts_c           0xE6
#define gInvalidHandle_c        0xE7
#define gInvalidParameter_c     0xE8
#define gNoAck_c                0xE9
#define gNoBeacon_c             0xEA
#define gNoData_c               0xEB
#define gNoShortAddress_c       0xEC
#define gOutOfCap_c             0xED
#define gPanIdConflict_c        0xEE
#define gRealignment_c          0xEF
#define gTransactionExpired_c   0xF0
#define gTransactionOverflow_c  0xF1
#define gTxActive_c             0xF2
#define gUnavailableKey_c       0xF3
#define gUnsupportedAttribute_c 0xF4


/************************************************************************************
*************************************************************************************
* Public type definitions
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
************************************************************************************/

#endif /* _PUB_CONST_H_ */
