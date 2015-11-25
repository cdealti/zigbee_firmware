/******************************************************************************
* This file contains the declarations for different tables used in BeeStack.
*
* Copyright (c) 2008, Freescale, Inc.  All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from  Freescale Semiconductor.
*
******************************************************************************/

#ifndef _BeeStackParameters_h
#define _BeeStackParameters_h
#ifndef __IAR_SYSTEMS_ICC__
#ifdef MC13226Included_d
    #undef MC13226Included_d
#endif    
    #define MC13226Included_d 0
#endif
#include "beestack_globals.h"
/******************************************************************************
*******************************************************************************
* Public type declarations
*******************************************************************************
******************************************************************************/

/* includes the APS Information Base (AIB) Table 2.24 */
typedef struct beeStackParameters_tag {

  /* Memory Allocated for AddressMapIndex of ZdoNVM Section */
  uint8_t iAddrMapIndex;

  /* this window size must be the same on both sending and receiving nodes (1-8) */
  uint8_t gApsMaxWindowSize;

  /* ID=0xc9 APS Fragmentation: 0-255, delay in ms between frames */
  uint8_t gApsInterframeDelay;

  /* ID=APS fragmentation, maximum size of fragment for each OTA frame (1-0x4e) */
  uint8_t gApsMaxFragmentLength;

  /* ID=0xc2, should we start up as a coordinator or router? */
  bool_t gfApsDesignatedCoordinator;

  /* ID=0xc6, For Multicast, stack profile 0x02 only */
  uint8_t  gApsNonmemberRadius;

  /* ID=0xc8, should we start up using insecure join? */
  bool_t gfApsUseInsecureJoinDefault;

#if gStandardSecurity_d || gHighSecurity_d
  /*
    053474r17 Table 4.36
    Name: apsTrustCenterAddress
    Type: Device Address
    Range: Any valid 64-bt address
    Default: --
    AIB Id: 0xab
  */

  /* The short addres must be save on NVM as well. */
  zbNwkAddr_t  apsTrustCenterNwkAddress;

  /*
    053474r17 Table 4.36
    Name: The period of time a device will wait for an expected security protocol
          frame (in milliseconds)
    Type: Integer
    Range: 0x0000 - 0xffff
    Default: 1000
    AIB Id: 0xac
  */
  uint16_t apsSecurityTimeOutPeriod;
#endif
} beeStackParameters_t;

/******************************************************************************
*******************************************************************************
* Public data declarations
*******************************************************************************
******************************************************************************/

extern beeStackParameters_t gBeeStackParameters;



  /* ID=0xc1, Memory Allocated for Binding table */
  extern apsBindingTable_t gaApsBindingTable[];
  
  /* Memory Allocated for AddressMap table */
  extern zbAddressMap_t gaApsAddressMap[];
  extern uint8_t gaApsAddressMapBitMask[];
extern addrMapIndex_t gGlobalAddressMapCounter;


/* Memory allocated for GroupTable */
  
#if MC13226Included_d
 extern uint8_t gEpMaskSize;
#else
 extern const uint8_t gEpMaskSize;
#endif
	

extern zbGroupTable_t gaApsGroupTable[];

/* Memory allocated for the security material */
/* Only gets included if the compile time option is enable */
#if gApsLinkKeySecurity_d
  extern zbApsDeviceKeyPairSet_t gaApsDeviceKeyPairSet[];
#if MC13226Included_d
  extern uint8_t giApsDeviceKeyPairCount;
#else
  extern const uint8_t giApsDeviceKeyPairCount;
#endif
#endif /*gApsLinkKeySecurity_d*/


extern zbKeyEstablish_t  *apSKKEMaterial[];
/* Memory allocated for the skke state machine. */
/* Only gets included if the compile time option is enable */
extern uint8_t gSKKEStateMachineSize;



/*****************************************************************************************
* WARNING: ANY CHANGE INTO ANY OF THE DATA SETS WILL BE REFLECTED INTO THE FUNCITON WHO
* HANDLES THE NV DATA ZdoNwkMng_SaveToNvm, SO PLEASE ADJUST ANY CHANGE MADE TO DATA SETS
* INTO THE FUNCTION LOCATED AT ZdoNwkManager.c
******************************************************************************************/
/* used for storing the APS data set in NVM */
#ifndef gHostApp_d

#if ((gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d) && gRnplusCapability_d)
#if gArm7ExtendedNVM_d
#define gAPS_DATA_SET_FOR_NVM\
      {&gBeeStackParameters, sizeof(beeStackParameters_t)},\
      {&gSAS_Ram, sizeof(zbCommissioningAttributes_t)},\
      {gaApsBindingTable, sizeof(apsBindingTable_t) * gMaximumApsBindingTableEntries_c},\
      {gaApsGroupTable, sizeof(zbGroupTable_t) * gApsMaxGroups_c}
#else
#define gAPS_DATA_SET_FOR_NVM\
      {&gBeeStackParameters, sizeof(beeStackParameters_t)},\
      {&gSAS_Ram, sizeof(zbCommissioningAttributes_t)},\
      {gaApsBindingTable, sizeof(apsBindingTable_t) * gMaximumApsBindingTableEntries_c},\
      {gaApsGroupTable, sizeof(zbGroupTable_t) * gApsMaxGroups_c},\
      {gaRouteTable, sizeof(routeTable_t) * gNwkInfobaseMaxRouteTableEntry_c }
#endif /* #if gArm7ExtendedNVM_d */
#else
#define gAPS_DATA_SET_FOR_NVM\
      {&gBeeStackParameters, sizeof(beeStackParameters_t)},\
      {&gSAS_Ram, sizeof(zbCommissioningAttributes_t)},\
      {gaApsBindingTable, sizeof(apsBindingTable_t) * gMaximumApsBindingTableEntries_c},\
      {gaApsGroupTable, sizeof(zbGroupTable_t) * gApsMaxGroups_c}
#endif /* ((gCoordinatorCapability_d || gRouterCapability_d || gComboDeviceCapability_d) && gRnplusCapability_d)*/

#else

#define gAPS_DATA_SET_FOR_NVM\
      {&gSAS_Ram, sizeof(zbCommissioningAttributes_t)},\
      {gaApsGroupTable, sizeof(zbGroupTable_t) * gApsMaxGroups_c}

#endif /* gHostApp_d */
#endif                                  /* #ifndef _BeeStackParameters_h */
