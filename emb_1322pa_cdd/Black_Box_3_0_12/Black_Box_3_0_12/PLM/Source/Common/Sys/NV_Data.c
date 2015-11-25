/*****************************************************************************
* Data definitions for the application client of the NV storage module.
*
* c) Copyright 2007, Freescale, Inc. All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
*****************************************************************************/

#include "EmbeddedTypes.h"

#include "AppToPlatformConfig.h"
#include "AppAspInterface.h"

#if gBeeStackIncluded_d
#include "BeeStackConfiguration.h"
#include "NVM_Interface.h"
#include "BeeStack_Globals.h"
#include "BeeStackParameters.h"
/* IMPORTANT: See the comments at the beginning of NVM_Interface.h. */
#include "ASL_UserInterface.h"
#include "HaProfile.h"
#endif

#if ECNetIncluded_d
#include "ECNet_Globals.h"
#endif

/*****************************************************************************
******************************************************************************
* Private macros
******************************************************************************
*****************************************************************************/
#if gTargetMC1322xSRB
  #define gXtalCtuneVal_c     0x08
  #define gXtalFTuneVal_c     0x0F

#elif gTargetMC1322xNCB
  #define gXtalCtuneVal_c     0x08
  #define gXtalFTuneVal_c     0x0F
 
#elif gTargetMC1322xUSB
  #define gXtalCtuneVal_c     0x0A
  #define gXtalFTuneVal_c     0x0E

#elif gTargetMC1322xLPB
  #define gXtalCtuneVal_c     0x0B
  #define gXtalFTuneVal_c     0x12


#elif gTarget_UserDefined
  /* User defined tuning values */
  #define gXtalCtuneVal_c     gXtalCtuneValUserDefined_c
  #define gXtalFTuneVal_c     gXtalFTuneValUserDefined_c
#else
  #error "Error: No valid target selected."
#endif


/* BEGIN Custom Clock Frequency Settings */
#define  gDigitalClock_PNVal_c 24
#define  gDigitalClock_RNVal_c 21
#define gMACA_Clock_DIVVal_c 95
#define gDigitalClock_RAFCVal_c 0
#define gSystemClock24MHzVal_c TRUE
#define gaRFSynVCODivIVal_c  { 0x2f,0x2f,0x2f,0x2f,0x2f,0x2f,0x2f,0x2f,0x2f,0x30,0x30,0x30,0x30,0x30,0x30,0x30 }
#define gaRFSynVCODivFVal_c { 0x00355555,0x006aaaaa,0x00a00000,0x00d55555,0x010aaaaa,0x01400000,0x01755555,0x01aaaaaa,0x01e00000,0x00155555,0x004aaaaa,0x00800000,0x00b55555,0x00eaaaaa,0x01200000,0x01555555 }
/* END Custom Clock Frequency Settings */




/*****************************************************************************
******************************************************************************
* Public memory declarations
******************************************************************************
*****************************************************************************/

/* different for each application */
extern NvDataItemDescription_t const gaNvAppDataSet[];

#pragma location = "nv_data_init_struct"
const HardwareParameters_t gHardwareParameters = 
{
 gaHardwareParametersDelimiterString_c, 
  
 SYSTEM_CLOCK/1000,                                   /* Bus_Frequency_In_MHz */
 
 {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},    /* MAC_Address */
 
  gDefaultPowerLevel_c,                               /* Default Power Level */
 
 #if gTarget_UserDefined
  gDualPortRFOperation_c,                             /* dualPortRFOperation */
 #else
  FALSE,
 #endif
  
  gChannelPa_c,                                       /* paPowerLevelLimits */
  
  gCcaThreshold_c,                                    /* ccaThreshold */
  
 #if gTarget_UserDefined
  gEnableComplementaryPAOutput_c,                     /* enableComplementatyPAOutput */
 #else
  FALSE,
 #endif  
  
  gSystemClock24MHzVal_c,                             /* gSystemClock24MHz_c */
  gXtalCtuneVal_c,                                    /* gXtalCtuneVal_c */
  gXtalFTuneVal_c,                                    /* gXtalFTune_c */
  gDigitalClock_PNVal_c,                              /* gDigitalClock_PN_c */
  gDigitalClock_RNVal_c,                              /* gDigitalClock_RN_c */  
  gMACA_Clock_DIVVal_c,                               /* gMACA_Clock_DIV_c */
  gDigitalClock_RAFCVal_c,                            /* gDigitalClock_RAFC_c */
  gaRFSynVCODivIVal_c,                                /* gaRFSynVCODivI_c[16] */
  gaRFSynVCODivFVal_c,                                /* gaRFSynVCODivF_c[16] */
  #if gTarget_UserDefined
   gPowerLevelLock_c,                                 /* paPowerLevelLock */
  #else
   FALSE,
  #endif  
  gaHardwareParametersDelimiterString_c  
};
/*****************************************************************************
******************************************************************************
* Public memory declarations
******************************************************************************
*****************************************************************************/
/*****************************************************************************************
* WARNING: ANY CHANGE INTO ANY OF THE DATA SETS WILL BE REFLECTED INTO THE FUNCITON WHO
* HANDLES THE NV DATA ZdoNwkMng_SaveToNvm, SO PLEASE ADJUST ANY CHANGE MADE TO DATA SETS
* INTO THE FUNCTION LOCATED AT ZdoNwkManager.c
******************************************************************************************/

#if !gArm7ExtendedNVM_d
/* Define the NV storage data sets. Each table must end with "{NULL, 0}". */

/* If the NV storage module's self test flag is on, the NV code will use */
/* it's own internal data. */
#if !gNvSelfTest_d
#if gBeeStackIncluded_d
/* This data set contains network layer variables to be preserved across resets */
NvDataItemDescription_t const gaNvNwkDataSet[] = {
  {&gNwkData,               sizeof(gNwkData)},
  {gaNeighborTable,         sizeof(neighborTable_t) * gNwkInfobaseMaxNeighborTableEntry_c },
  {&gBeeStackConfig,        sizeof(beeStackConfigParams_t)},
  {NULL, 0}       /* Required end-of-table marker. */ 
};

/* This data set holds the Address map data and the Aps secure material set. */
NvDataItemDescription_t const gaNvApsAndSecurityDataSet[] = {
  {gaApsAddressMap, sizeof(zbAddressMap_t) * gApsMaxAddrMapEntries_c},
  {gaApsAddressMapBitMask, sizeof(uint8_t) * gApsAddressMapBitMaskInBits_c},
#if gApsLinkKeySecurity_d
  {gaApsDeviceKeyPairSet, sizeof(zbApsDeviceKeyPairSet_t) * gApsMaxSecureMaterialEntries_c},
#endif
  {NULL, 0}       /* Required end-of-table marker. */
};

/* This data set holds the NwkSecure Frame counters and the key table for APS security. */
#if gStandardSecurity_d || gHighSecurity_d
NvDataItemDescription_t const gaNvSecurityDataSet[] = {
  {gaIncomingFrameCounterSet1, sizeof(zbIncomingFrameCounterSet_t) * gNwkInfobaseMaxNeighborTableEntry_c},
  {gaIncomingFrameCounterSet2, sizeof(zbIncomingFrameCounterSet_t) * gNwkInfobaseMaxNeighborTableEntry_c},
#if (gApsLinkKeySecurity_d)
  {gaApsKeySet,                sizeof(zbAESKey_t) * gApsMaxLinkKeys_c},
#endif
  {NULL, 0}       /* Required end-of-table marker. */
};
#endif
#endif


#if ECNetIncluded_d
/* This data set contains network layer variables to be preserved across resets */
NvDataItemDescription_t const gaNvNwkDataSet[] = {
  {(uint8_t*)&nodeData, sizeof(nodeData_t)},
  {NULL, 0}       /* Required end-of-table marker. */
};
#endif /* ECNetIncluded_d */

#if gMacStandAlone_d
/* This data set contains MAC layer variables to be preserved across resets */
NvDataItemDescription_t const gaNvNwkDataSet[] = {
  {NULL, 0}       /* Required end-of-table marker. */
};
#endif

/* different for each application */
extern NvDataItemDescription_t const gaNvAppDataSet[];

/*****************************************************************************************
* WARNING: ANY CHANGE INTO ANY OF THE DATA SETS WILL BE REFLECTED INTO THE FUNCITON WHO
* HANDLES THE NV DATA ZdoNwkMng_SaveToNvm, SO PLEASE ADJUST ANY CHANGE MADE TO DATA SETS
* INTO THE FUNCTION LOCATED AT ZdoNwkManager.c
******************************************************************************************/
/* Table of data sets. Required by the NV storage module. */
/* There must be gNvNumberOfDataSets_c entries in this table. If the */
/* number of entries changes, gNvNumberOfDataSets_c must be changed. */
NvDataSetDescription_t const NvDataSetTable[gNvNumberOfDataSets_c] = {
  {gNvDataSet_Nwk_ID_c, gaNvNwkDataSet},
  {gNvDataSet_App_ID_c, gaNvAppDataSet},
  {gNvDataSet_Aps_Sec_ID_c, gaNvApsAndSecurityDataSet}
#if gStandardSecurity_d || gHighSecurity_d
  ,{gNvDataSet_Sec_ID_c, gaNvSecurityDataSet}
#endif
};

#endif                                  /* #if !gNvSelfTest_d */

/*****************************************************************************
*****************************************************************************/
#else /* #if gArm7ExtendedNVM_d */
/*****************************************************************************
*****************************************************************************/

/* Table 1 data sets */
NvDataItemDescription_t const gaNvNwkDataSet[] = {
  {&gNwkData,               sizeof(gNwkData)},
  {&gBeeStackConfig,        sizeof(beeStackConfigParams_t)},
#if gRnplusCapability_d
  {gaRouteTable, sizeof(routeTable_t) * gNwkInfobaseMaxRouteTableEntry_c },
#endif
  {NULL,0}  /* Required end-of-table marker. */
};

#if gApsLinkKeySecurity_d
NvDataItemDescription_t const gaNvApsKeysDataSet[] = {
  {gaApsKeySet,                sizeof(zbAESKey_t) * gApsMaxLinkKeys_c},
  {NULL,0}  /* Required end-of-table marker. */
};
#endif

/* Table 2 data sets */
NvDataItemDescription_t const gaNvNTDataSet[] = {
  {gaNeighborTable, sizeof(neighborTable_t) * gNwkInfobaseMaxNeighborTableEntry_c},
  {NULL,0}  /* Required end-of-table marker. */
};

NvDataItemDescription_t const gaNvAddrMapDataSet[] = {
  {gaApsAddressMapBitMask, sizeof(uint8_t) * gApsAddressMapBitMaskInBits_c},
  {gaApsAddressMap, sizeof(zbAddressMap_t) * gApsMaxAddrMapEntries_c},
  {NULL,0}  /* Required end-of-table marker. */
};

/* Table 3 data sets */
/* This data set contains network security variables to be preserved across resets */
#if gStandardSecurity_d || gHighSecurity_d
NvDataItemDescription_t const gaNvNwkAndApsSecNwkDataSet[] = {
  {gaIncomingFrameCounterSet1, sizeof(zbIncomingFrameCounterSet_t) * gNwkInfobaseMaxNeighborTableEntry_c},
  {gaIncomingFrameCounterSet2, sizeof(zbIncomingFrameCounterSet_t) * gNwkInfobaseMaxNeighborTableEntry_c},
  {&(gNwkData.aNwkSecurityMaterialSet[0].outgoingFrameCounter), sizeof(zbFrameCounter_t)},
  {&(gNwkData.aNwkSecurityMaterialSet[1].outgoingFrameCounter), sizeof(zbFrameCounter_t)},
#if gApsLinkKeySecurity_d
  {gaApsDeviceKeyPairSet,  sizeof(zbApsDeviceKeyPairSet_t) * gApsMaxSecureMaterialEntries_c},
#endif
  {NULL,0}  /* Required end-of-table marker. */
};
#endif //gStandardSecurity_d || gHighSecurity_d

/* Table of data sets. Required by the NV storage module. */
/* There must be gNvNumberOfDataSets_c entries in this table. If the */
/* number of entries changes, gNvNumberOfDataSets_c must be changed. */

/* Table 1 */
NvDataSetDescription_t const NvGeneralDataSetTable[gNvNumberOfDataSets1_c] = {
  {gNvNwkDataSet_ID_c, gaNvNwkDataSet}
  ,{gNvAppDataSet_ID_c, gaNvAppDataSet}
#if gApsLinkKeySecurity_d
  ,{gNvApsKeysDataSet_ID_c, gaNvApsKeysDataSet}
#endif
};

/* Table 2 */
NvDataSetDescription_t const NvNTAndAddrMapDataSetTable[gNvNumberOfDataSets2_c] = {
  {gNvNTDataSet_ID_c, gaNvNTDataSet},
  {gNvAddrMapDataSet_ID_c, gaNvAddrMapDataSet}
};

/* Table 3 */
#if gStandardSecurity_d || gHighSecurity_d
NvDataSetDescription_t const NvNwkSecDataSetTable[gNvNumberOfDataSets3_c] = {
  {gNvNwkAndApsSecDataSet_ID_c, gaNvNwkAndApsSecNwkDataSet}
};
#endif

NvDataSetDescription_t const* paNvDataSetTable[gNvNumberOfDataSetTypes_c]={ NvGeneralDataSetTable
                                                                           ,NvNTAndAddrMapDataSetTable
#if gStandardSecurity_d || gHighSecurity_d
                                                                           ,NvNwkSecDataSetTable
#endif
                                                                           };

/*
  Important: gNvNwkAndApsSecDataSet_ID_c must be restored after the gNvNwkDataSet_ID_c,
  in this case gNvNwkSecDataSet_ID_c must be at a higher index than gNvNwkDataSet_ID_c
  in allDataSets.
*/
NvDataSetID_t const allDataSets[gNvNumberOfDataSets_c] = {gNvNwkDataSet_ID_c,
                                                          gNvAppDataSet_ID_c,
#if gApsLinkKeySecurity_d
                                                          gNvApsKeysDataSet_ID_c,
#endif
                                                          gNvNTDataSet_ID_c,
                                                          gNvAddrMapDataSet_ID_c
#if gStandardSecurity_d || gHighSecurity_d
                                                          ,gNvNwkAndApsSecDataSet_ID_c
#endif
                                                          };
#endif /* #if gArm7ExtendedNVM_d */