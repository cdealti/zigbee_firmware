/************************************************************************
CALCULATION

This file permits the calculation of some definitions depending on the previous
selected values on BeeStackConfiguration.h and ApplicationConf.h
************************************************************************/
#ifndef _BEESTACK_CALCULATIONS_H_
#define _BEESTACK_CALCULATIONS_H_

#include "embeddedtypes.h"
#include "ApplicationConf.h"
#include "Beestackconfiguration.h"
#include "NVM_Interface.h"

/* if this is not a manufacturer specific profile then set the right value 
for gAppStackProfile_c and tree routing depending on zigbee pro included
*/
#if gMSPstackProfileEnabled_d == FALSE
#undef gAppStackProfile_c
#undef mNwkUseTreeRouting_c
#if gZigbeeProIncluded_d
#define gAppStackProfile_c      0x02
#define mNwkUseTreeRouting_c    FALSE
#else
#define gAppStackProfile_c      0x01
#define mNwkUseTreeRouting_c    TRUE
#endif
#endif

/*mDefaultValueOfMacCapFlags_c*/
/*RxOnWhenIdle*/
#if gRxOnWhenIdle_d
#define RxOnWhenIdleBit      0x08
#else
#define RxOnWhenIdleBit      0x00
#endif
/* Type of power source, it shall be set to 1 if the current power source is mains power (gPowerSource_d = 0).
   Otherwise, the power source sub-field shall be set to 0. */
#if gPowerSource_d
#define PowerSourceBit       0x00
#else
#define PowerSourceBit       0x04
#endif

#if gHighSecurity_d
#define SecurityCapabilityBit   0x40
#else
#define SecurityCapabilityBit   0x00
#endif


/*MAC Security Enabled or not*/
/*This should be always disable and not configurable*/
#ifndef gMacSecurity_c
#define gMacSecurity_c                  FALSE
#endif



/*MacCapabilityFlags for Coordinator and Router*/
#if gCoordinatorCapability_d || gRouterCapability_d
#define mDefaultValueOfMacCapFlags_c                (0x8a | PowerSourceBit | SecurityCapabilityBit)
#else
/*MacCapabilityFlags for End Device*/
#define mDefaultValueOfMacCapFlags_c                (0x80 | RxOnWhenIdleBit | PowerSourceBit | SecurityCapabilityBit)
#endif

/*--------------------------- For NWK layer ---------------------------------*/

/*Default Value of Network Protocol ID (Bits 4-7) -- Should not be configurable*/
#ifndef gDefaultValueOfNwkProtocolId_c
#define gDefaultValueOfNwkProtocolId_c      0x00
#endif

/*gNwkInfobaseMaxNeighborTableEntry_c if a Coordinator*/
#if gCoordinatorCapability_d
#define   gNwkInfobaseMaxNeighborTableEntry_c      gCoordinatorNwkInfobaseMaxNeighborTableEntry_c
#endif

/*gNwkInfobaseMaxNeighborTableEntry_c if a Router*/
#if gRouterCapability_d
#define   gNwkInfobaseMaxNeighborTableEntry_c      gRouterNwkInfobaseMaxNeighborTableEntry_c
#endif

/*gNwkInfobaseMaxNeighborTableEntry_c if an End Device*/
#if gEndDevCapability_d
#define   gNwkInfobaseMaxNeighborTableEntry_c      gEndDevNwkInfobaseMaxNeighborTableEntry_c
#endif

/*gNwkInfobaseMaxNeighborTableEntry_c if a Combo Device*/
#if gComboDeviceCapability_d
#define   gNwkInfobaseMaxNeighborTableEntry_c      gRouterNwkInfobaseMaxNeighborTableEntry_c
#endif

#if !gZigbeeProIncluded_d
#undef mNwkSymmetryLink_c
#define mNwkSymmetryLink_c  FALSE
#endif

/*
  053474r17 ZigBee Spec. Table 3.44 NIB Attributes
  
  Default: 0x00
  Range:   0x00 - 0x02
  
  A value that determines the method used to assign addresses:
  0x00 = use distributed address allocation
  0x01 = reserved
  0x02 = use stochastic address allocation
*/
#if gStochasticAddressingCapability_d
  #ifdef mNwkAddressAlloc_c
  #undef mNwkAddressAlloc_c 
  #endif
  #define mNwkAddressAlloc_c 0x02

/*
  053474r17 ZigBee Spec. Table 3.44 NIB Attributes

  A flag that determines whether the NWK layer should detect and correct conflicting addresses:
  TRUE = assume addresses are unique.
  FALSE = addresses may not be unique

  Default: TRUE
  Range:   bool_t
*/
  #define mNwkUniqueAddr_c FALSE

#endif
#if gDistributedAddressingCapability_d
  #ifdef mNwkAddressAlloc_c
  #undef mNwkAddressAlloc_c 
  #endif
  #define mNwkAddressAlloc_c 0x00

  #define mNwkUniqueAddr_c TRUE
#endif
/*--------------------------- For SSP ---------------------------------------*/
/*********************Residential Stack Profile Only *************************/


/*Default Trust Center Capability*/
/*This compiler time option must be selected for Coordinator builds in
  Residential and Commertial Security*/
#ifndef gTrustCenter_d
#define gTrustCenter_d 0
#endif

/*
  The default type of key to handle in the secure environment for the NWK layer auxiliary 
  frame.

  KeyType = 0x01 Standard network key.
  KeyType = 0x05 High Security network key.
*/
#if gHighSecurity_d

#ifdef mDefaultValueOfNwkKeyType_c
#undef mDefaultValueOfNwkKeyType_c
#endif
#define mDefaultValueOfNwkKeyType_c   0x05

#else

#ifdef mDefaultValueOfNwkKeyType_c
#undef mDefaultValueOfNwkKeyType_c
#endif
#define mDefaultValueOfNwkKeyType_c   0x01

#endif

/* No Aps Link Key Security if no security */
#if gApsLinkKeySecurity_d && !gHighSecurity_d && !gStandardSecurity_d
#undef gApsLinkKeySecurity_d
#define gApsLinkKeySecurity_d FALSE
#endif

#if gSKKESupported_d && !gApsLinkKeySecurity_d
#undef gSKKESupported_d
#define gSKKESupported_d  FALSE
#endif

#if !gSKKESupported_d
#undef mDefaultValueOfApplicationKeyType_c
#define mDefaultValueOfApplicationKeyType_c 0x03
#endif



/*****************************************************************************/

/*Bit mask for setting User Descriptor available bit in Logical type byte*/
#if gUser_Desc_rsp_d
#define gUserDescLogicalTypeBitMask_d  0x10
#else
#define gUserDescLogicalTypeBitMask_d  0x00
#endif
/*Bit mask for setting Complex Descriptor available bit in Logical type byte*/
#if gComplex_Desc_rsp_d
#define gComplexDescLogicalTypeBitMask_d  0x08
#else
#define gComplexDescLogicalTypeBitMask_d  0x00
#endif


/*
If network manager is not enabled by the user the default is the coordinator
*/
#if !gNetworkManagerCapability_d
#if gCoordinatorCapability_d
  #ifdef gNetworkManagerCapability_d
  #undef gNetworkManagerCapability_d
  #define gNetworkManagerCapability_d TRUE
  #endif
#endif
#endif

/*****************************************************************************
SERVER MASK CALCULATION
*****************************************************************************/
/* Primary Trust Center */
/* Only the coordinator in residential stack profile can be a PTC */
#if  gTrustCenter_d
#define gServerMask_PTCBit_d       0x01
#else
#define gServerMask_PTCBit_d       0x00
#endif

/*Backup Trust Center*/
/*Not supported - always 0*/
#ifndef gServerMask_BTCBit_d
#define gServerMask_BTCBit_d       0x00
#endif

/*Primary Binding Table Cache*/
/*If any of this commands are enable the device must be a PBTC*/
#if gRecover_Source_Bind_req_d || gBind_Register_rsp_d || \
		gReplace_Device_rsp_d || gStore_Bkup_Bind_Entry_req_d || \
		gRecover_Source_Bind_req_d ||gRemove_Bkup_Bind_Entry_req_d || \
		gBackup_Source_Bind_req_d
#if gEndDevCapability_d && (!gRxOnWhenIdle_d)
#define gServerMask_PBTCBit_d      0x00
#else
#define gServerMask_PBTCBit_d      0x04
#endif
#else
#define gServerMask_PBTCBit_d      0x00
#endif

/*Backup Binding Table Cache*/
/*If any of this commands are enable the device must be a BBTC*/
#if gStore_Bkup_Bind_Entry_rsp_d  || gRecover_Source_Bind_rsp_d || \
		gRemove_Bkup_Bind_Entry_rsp_d || gBackup_Source_Bind_rsp_d
#if gEndDevCapability_d && (!gRxOnWhenIdle_d)
#define gServerMask_BBTCBit_d      0x00
#else
#define gServerMask_BBTCBit_d      0x08
#endif
#else
#define gServerMask_BBTCBit_d      0x00
#endif

/*Primary Discovery Cache*/
/*If any of this commands are enable the device must be a PDC*/
#if gDiscovery_store_rsp_d  || gNode_Desc_store_rsp_d || \
		gPower_Desc_store_rsp_d || gActive_EP_store_rsp_d || \
		gSimple_Desc_store_rsp_d || gRemove_node_cache_rsp_d || \
		gFind_node_cache_rsp_d || gDiscovery_Cache_rsp_d || \
		gMgmt_Cache_rsp_d
#if gEndDevCapability_d && (!gRxOnWhenIdle_d)
#define gServerMask_PDCBit_d       0x00
#else
#define gServerMask_PDCBit_d       0x10
#endif
#else
#define gServerMask_PDCBit_d       0x00
#endif

/*Backup Discovery Cache*/
/*If any of this commands are anable and the flag for Dicovery Cache back up is on, and the device s must be BDC*/
#if (gDiscovery_store_rsp_d  || gNode_Desc_store_rsp_d || \
		gPower_Desc_store_rsp_d || gActive_EP_store_rsp_d || \
		gSimple_Desc_store_rsp_d || gRemove_node_cache_rsp_d || \
		gFind_node_cache_rsp_d || gDiscovery_Cache_rsp_d || \
		gMgmt_Cache_rsp_d) && gBkup_Discovery_cache_d
#if gEndDevCapability_d && (!gRxOnWhenIdle_d)
#define gServerMask_BDCBit_d       0x00
#else
#define gServerMask_BDCBit_d       0x20
#endif
#else
#define gServerMask_BDCBit_d       0x00
#endif

/* Network Manager */
/* Only the coordinator or a router in Beestack 2007 and pro Can be the Network Manager */
#if  ((gCoordinatorCapability_d ||gRouterCapability_d )&& (gNetworkManagerCapability_d))
#define gServerMask_NMCBit_d       0x40
#else
#define gServerMask_NMCBit_d       0x00
#endif

/*We need to divide the 16 bits in two bytes to have the little endian form*/
/*Calculation of the first byte in little endian form*/
#define gServerMask_lEndianFirstByte_d   gServerMask_BDCBit_d | gServerMask_PDCBit_d | gServerMask_BBTCBit_d | gServerMask_PBTCBit_d | gServerMask_BTCBit_d | gServerMask_PTCBit_d | gServerMask_NMCBit_d
/*6-15 bits reserved*/
#define gServerMask_lEndianSecondByte_d   0x00

/*Server Mask*/
#define gServerMask_c   gServerMask_lEndianFirstByte_d, gServerMask_lEndianSecondByte_d

/*
Configuration options for ZDP, This defines thesize of the backup binding cache table and binding cache table
*/
#if gReplace_Device_rsp_d || gRemove_Bkup_Bind_Entry_rsp_d || gBackup_Bind_Table_rsp_d || \
		gStore_Bkup_Bind_Entry_rsp_d || gRecover_Bind_Table_rsp_d || gBind_Register_rsp_d || gMgmt_Bind_rsp_d
#ifndef gMaximumBindingCacheTableList_c
#define gMaximumBindingCacheTableList_c    3  /* The binding cache table size */
#endif
#endif

#endif _BEESTACK_CALCULATIONS_H_
