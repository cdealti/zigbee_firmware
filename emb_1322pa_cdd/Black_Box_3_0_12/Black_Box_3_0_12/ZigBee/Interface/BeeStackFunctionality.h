/******************************************************************************
 * This file allows for combination settings with a single command-line switch.
 * Used to build the various flavors of BeeStack Libraries.
 *
 * Copyright (c) 2008, Freescale, Inc.  All rights reserved.
 *
 *
 * No part of this document must be reproduced in any form - including copied,
 * transcribed, printed or by any electronic means - without specific written
 * permission from Freescale Semiconductor.
 *
 *****************************************************************************/
#ifndef _BEESTACK_FUNCTIONALITY_H_
#define _BEESTACK_FUNCTIONALITY_H_



#ifndef TRUE
#define TRUE    1
#endif

#ifndef FALSE
#define FALSE   0
#endif



/****************************************************************************/

/* Option names for ZigBee coordinators. */

#ifndef gBeeStackZcNs_d
#define gBeeStackZcNs_d     FALSE
#endif

#ifndef gBeeStackZcRs_d
#define gBeeStackZcRs_d     FALSE
#endif

#ifndef gBeeStackZcRNP_d
#define gBeeStackZcRNP_d    FALSE
#endif

#ifndef gBeeStackZcRsRNP_d
#define gBeeStackZcRsRNP_d  FALSE
#endif

/* Option names for ZigBee routers. */

#ifndef gBeeStackZrNs_d
#define gBeeStackZrNs_d     FALSE
#endif

#ifndef gBeeStackZrRs_d
#define gBeeStackZrRs_d     FALSE
#endif

#ifndef gBeeStackZrRNP_d
#define gBeeStackZrRNP_d    FALSE
#endif

#ifndef gBeeStackZrRsRNP_d
#define gBeeStackZrRsRNP_d  FALSE
#endif

/* Option names for ZigBee end devices. */

#ifndef gBeeStackZedNs_d
#define gBeeStackZedNs_d    FALSE
#endif

#ifndef gBeeStackZedRs_d
#define gBeeStackZedRs_d    FALSE
#endif

#ifndef gBeeStackZedRNP_d
#define gBeeStackZedRNP_d   FALSE
#endif

#ifndef gBeeStackZedRsRNP_d
#define gBeeStackZedRsRNP_d FALSE
#endif

/* Sp2 libraries */
#ifndef gBeeStackZcRNPSp2_d
#define gBeeStackZcRNPSp2_d     FALSE
#endif
#ifndef gBeeStackZcRsRNPSp2_d
#define gBeeStackZcRsRNPSp2_d   FALSE
#endif
#ifndef gBeeStackZcHsRNPSp2_d
#define gBeeStackZcHsRNPSp2_d   FALSE
#endif

#ifndef gBeeStackZedRNPSp2_d
#define gBeeStackZedRNPSp2_d    FALSE
#endif
#ifndef gBeeStackZedRsRNPSp2_d
#define gBeeStackZedRsRNPSp2_d  FALSE
#endif
#ifndef gBeeStackZedHsRNPSp2_d
#define gBeeStackZedHsRNPSp2_d  FALSE
#endif

#ifndef gBeeStackZrRNPSp2_d
#define gBeeStackZrRNPSp2_d     FALSE
#endif
#ifndef gBeeStackZrRsRNPSp2_d
#define gBeeStackZrRsRNPSp2_d   FALSE
#endif
#ifndef gBeeStackZrHsRNPSp2_d
#define gBeeStackZrHsRNPSp2_d   FALSE
#endif

/* Combo device libraries */

#ifndef gBeeStackZxNs_d
#define gBeeStackZxNs_d         FALSE
#endif
#ifndef gBeeStackZxRNP_d
#define gBeeStackZxRNP_d        FALSE
#endif
#ifndef gBeeStackZxRs_d
#define gBeeStackZxRs_d         FALSE
#endif
#ifndef gBeeStackZxRsRNP_d
#define gBeeStackZxRsRNP_d      FALSE
#endif  
#ifndef gBeeStackZxRNPSp2_d
#define gBeeStackZxRNPSp2_d     FALSE
#endif
#ifndef gBeeStackZxRsRNPSp2_d
#define gBeeStackZxRsRNPSp2_d   FALSE
#endif
#ifndef gBeeStackZxHsRNPSp2_d
#define gBeeStackZxHsRNPSp2_d   FALSE
#endif

/* Minimal library */
 
#ifndef gMinimalSet_d
#define gMinimalSet_d             FALSE
#endif
#ifndef gBeeStackZcMinimalSp1_d
#define gBeeStackZcMinimalSp1_d   FALSE
#endif
#ifndef gBeeStackZrMinimalSp1_d
#define gBeeStackZrMinimalSp1_d   FALSE
#endif
#ifndef gBeeStackZedMinimalSp1_d
#define gBeeStackZedMinimalSp1_d  FALSE
#endif

#ifndef gBeeStackZcRsMinimalSp2_d
#define gBeeStackZcRsMinimalSp2_d FALSE
#endif

#ifndef gBeeStackZrRsMinimalSp2_d
#define gBeeStackZrRsMinimalSp2_d FALSE
#endif

#ifndef gBeeStackZedRsMinimalSp2_d
#define gBeeStackZedRsMinimalSp2_d FALSE
#endif

#ifndef gBeeStackZcHsMinimalSp2_d
#define gBeeStackZcHsMinimalSp2_d FALSE
#endif

#ifndef gBeeStackZrHsMinimalSp2_d
#define gBeeStackZrHsMinimalSp2_d FALSE
#endif

#ifndef gBeeStackZedHsMinimalSp2_d
#define gBeeStackZedHsMinimalSp2_d FALSE
#endif


/* Full Load Libraries */
#ifndef gBeeStackZcFullLoadSp1_d
#define gBeeStackZcFullLoadSp1_d  FALSE
#endif

#ifndef gBeeStackZrFullLoadSp1_d
#define gBeeStackZrFullLoadSp1_d  FALSE
#endif

#ifndef gBeeStackZedFullLoadSp1_d
#define gBeeStackZedFullLoadSp1_d  FALSE
#endif

#ifndef gBeeStackZcFullLoadSp2_d
#define gBeeStackZcFullLoadSp2_d  FALSE
#endif

#ifndef gBeeStackZrFullLoadSp2_d
#define gBeeStackZrFullLoadSp2_d  FALSE
#endif

#ifndef gBeeStackZedFullLoadSp2_d
#define gBeeStackZedFullLoadSp2_d  FALSE
#endif

/****************************************************************************/

/* ZigBee Coordinator, tree only (no mesh networking), no security */
#if gBeeStackZcNs_d
#define gCoordinatorCapability_d            TRUE

/* Protocol definition */
#ifndef gZigbeeProIncluded_d
#define gZigbeeProIncluded_d                FALSE
#endif
/* NWK layer definition */
#ifndef gMeshNetworkingCapability_d
#define gMeshNetworkingCapability_d         FALSE   /* On for RNP Zc or Zr, off otherwise. */
#endif
#ifndef gNwkValidationCapability_d
#define gNwkValidationCapability_d          TRUE    /* Turn off if out of ROM */
#endif
#ifndef gNwkGetSetCapability_d
#define gNwkGetSetCapability_d              TRUE    /* Turn off if out of ROM */
#endif
#ifndef gNwkMulticastCapability_d
#define gNwkMulticastCapability_d           FALSE
#endif
#ifndef gNwkManyToOneCapability_d
#define gNwkManyToOneCapability_d           FALSE
#endif
#ifndef gNwkSymLinkCapability_d
#define gNwkSymLinkCapability_d             FALSE
#endif
/* AF layer definition */
#ifndef gAfVerifyParamCapability_d
#define gAfVerifyParamCapability_d          TRUE    /* Turn off if out of ROM */
#endif
/* SSP Layer Definition. */
#ifndef gStandardSecurity_d
#define gStandardSecurity_d                 FALSE   /* On/Off Standar Security. */
#endif

/* gHighSecurity_d if forbidden for for SP1*/

#endif                                  /* #if gBeeStackZcNs_d */

/****************************************************************************/

/* ZigBee coordinator, tree only (no mesh networking), residential security. */
#if gBeeStackZcRs_d

#define gCoordinatorCapability_d            TRUE

/* Protocol definition */
#ifndef gZigbeeProIncluded_d
#define gZigbeeProIncluded_d                FALSE
#endif
/* NWK layer definition */
#ifndef gMeshNetworkingCapability_d
#define gMeshNetworkingCapability_d         FALSE   /* On for RNP Zc or Zr, off otherwise. */
#endif
#ifndef gNwkValidationCapability_d
#define gNwkValidationCapability_d          TRUE
#endif
#ifndef gNwkGetSetCapability_d
#define gNwkGetSetCapability_d              TRUE
#endif
#ifndef gNwkMulticastCapability_d
#define gNwkMulticastCapability_d           FALSE
#endif
#ifndef gNwkManyToOneCapability_d
#define gNwkManyToOneCapability_d           FALSE
#endif
#ifndef gNwkSymLinkCapability_d
#define gNwkSymLinkCapability_d             FALSE
#endif
/* AF layer definition */
#ifndef gAfVerifyParamCapability_d
#define gAfVerifyParamCapability_d          TRUE
#endif

/* SSP Layer Definition. */
#ifndef gStandardSecurity_d
#define gStandardSecurity_d                 TRUE
#endif

#ifndef gHighSecurity_d
#define gHighSecurity_d                     FALSE
#endif


#endif                                  /* #if gBeeStackZcRs_d */

/****************************************************************************/

/* ZigBee coordinator, tree and mesh networking, no security */
#if gBeeStackZcRNP_d

#define gCoordinatorCapability_d            TRUE

/* Protocol definition */
#ifndef gZigbeeProIncluded_d
#define gZigbeeProIncluded_d                FALSE
#endif
/* NWK layer definition */
#ifndef gMeshNetworkingCapability_d
#define gMeshNetworkingCapability_d         TRUE
#endif
#ifndef gNwkValidationCapability_d
#define gNwkValidationCapability_d          TRUE
#endif
#ifndef gNwkGetSetCapability_d
#define gNwkGetSetCapability_d              TRUE
#endif
#ifndef gNwkMulticastCapability_d
#define gNwkMulticastCapability_d           FALSE
#endif
#ifndef gNwkManyToOneCapability_d
#define gNwkManyToOneCapability_d           FALSE
#endif
#ifndef gNwkSymLinkCapability_d
#define gNwkSymLinkCapability_d             FALSE
#endif
/* AF layer definition */
#ifndef gAfVerifyParamCapability_d
#define gAfVerifyParamCapability_d          TRUE
#endif

/* SSP Layer Definition. */
#ifndef gStandardSecurity_d
#define gStandardSecurity_d                 FALSE  
#endif

#ifndef gHighSecurity_d
#define gHighSecurity_d                     FALSE
#endif


#endif                                  /* #if gBeeStackZcRNP_d */

/****************************************************************************/

/* ZigBee coordinator, tree and mesh networking, residential security. */
#if gBeeStackZcRsRNP_d

#define gCoordinatorCapability_d            TRUE

/* Protocol definition */
#ifndef gZigbeeProIncluded_d
#define gZigbeeProIncluded_d                FALSE
#endif
/* NWK layer definition */
#ifndef gMeshNetworkingCapability_d
#define gMeshNetworkingCapability_d         TRUE
#endif
#ifndef gNwkValidationCapability_d
#define gNwkValidationCapability_d          TRUE
#endif
#ifndef gNwkGetSetCapability_d
#define gNwkGetSetCapability_d              TRUE
#endif
#ifndef gNwkMulticastCapability_d
#define gNwkMulticastCapability_d           FALSE
#endif
#ifndef gNwkManyToOneCapability_d
#define gNwkManyToOneCapability_d           FALSE
#endif
#ifndef gNwkSymLinkCapability_d
#define gNwkSymLinkCapability_d             FALSE
#endif
/* AF layer definition */
#ifndef gAfVerifyParamCapability_d
#define gAfVerifyParamCapability_d          TRUE
#endif

/* SSP Layer Definition. */
#ifndef gStandardSecurity_d
#define gStandardSecurity_d                 TRUE
#endif

#ifndef gHighSecurity_d
#define gHighSecurity_d                     FALSE
#endif



#endif                                  /* #if gBeeStackZcRsRNP_d */

/****************************************************************************/

/* ZigBee router, tree only (no mesh networking), no security. */
#if gBeeStackZrNs_d

#define gRouterCapability_d                 TRUE

/* Protocol definition */
#ifndef gZigbeeProIncluded_d
#define gZigbeeProIncluded_d                FALSE
#endif
/* NWK layer definition */
#ifndef gMeshNetworkingCapability_d
#define gMeshNetworkingCapability_d         FALSE
#endif
#ifndef gNwkValidationCapability_d
#define gNwkValidationCapability_d          TRUE
#endif
#ifndef gNwkGetSetCapability_d
#define gNwkGetSetCapability_d              TRUE
#endif

/* AF layer definition */
#ifndef gAfVerifyParamCapability_d
#define gAfVerifyParamCapability_d          TRUE
#endif

/* SSP Layer Definition. */
#ifndef gStandardSecurity_d
#define gStandardSecurity_d                 FALSE  
#endif

#ifndef gHighSecurity_d
#define gHighSecurity_d                     FALSE
#endif



#endif                                  /* #if gBeeStackZrNs_d */

/****************************************************************************/

/* ZigBee router, tree only (no mesh networking), residential security. */
#if gBeeStackZrRs_d

#define gRouterCapability_d                 TRUE

/* Protocol definition */
#ifndef gZigbeeProIncluded_d
#define gZigbeeProIncluded_d                FALSE
#endif
/* NWK layer definition */
#ifndef gMeshNetworkingCapability_d
#define gMeshNetworkingCapability_d         FALSE
#endif
#ifndef gNwkValidationCapability_d
#define gNwkValidationCapability_d          TRUE
#endif
#ifndef gNwkGetSetCapability_d
#define gNwkGetSetCapability_d              TRUE
#endif
#ifndef gNwkMulticastCapability_d
#define gNwkMulticastCapability_d           FALSE
#endif
#ifndef gNwkManyToOneCapability_d
#define gNwkManyToOneCapability_d           FALSE
#endif
#ifndef gNwkSymLinkCapability_d
#define gNwkSymLinkCapability_d             FALSE
#endif
/* AF layer definition */
#ifndef gAfVerifyParamCapability_d
#define gAfVerifyParamCapability_d          TRUE
#endif

/* SSP Layer Definition. */
#ifndef gStandardSecurity_d
#define gStandardSecurity_d                 TRUE
#endif

#ifndef gHighSecurity_d
#define gHighSecurity_d                     FALSE
#endif



#endif  /* #if gBeeStackZrRs_d */

/****************************************************************************/

/* ZigBee router, tree and mesh networking, no security. */
#if gBeeStackZrRNP_d

#define gRouterCapability_d                 TRUE

/* Protocol definition */
#ifndef gZigbeeProIncluded_d
#define gZigbeeProIncluded_d                FALSE
#endif
/* NWK layer definition */
#ifndef gMeshNetworkingCapability_d
#define gMeshNetworkingCapability_d         TRUE
#endif
#ifndef gNwkValidationCapability_d
#define gNwkValidationCapability_d          TRUE
#endif
#ifndef gNwkGetSetCapability_d
#define gNwkGetSetCapability_d              TRUE
#endif
#ifndef gNwkMulticastCapability_d
#define gNwkMulticastCapability_d           FALSE
#endif
#ifndef gNwkManyToOneCapability_d
#define gNwkManyToOneCapability_d           FALSE
#endif
#ifndef gNwkSymLinkCapability_d
#define gNwkSymLinkCapability_d             FALSE
#endif
/* AF layer definition */
#ifndef gAfVerifyParamCapability_d
#define gAfVerifyParamCapability_d          TRUE
#endif

/* SSP Layer Definition. */
#ifndef gStandardSecurity_d
#define gStandardSecurity_d                 FALSE  
#endif

#ifndef gHighSecurity_d
#define gHighSecurity_d                     FALSE
#endif



#endif  /* #if gBeeStackZrRNP_d */

/****************************************************************************/

/* ZigBee router, tree and mesh networking, residential security. */
#if gBeeStackZrRsRNP_d

#define gRouterCapability_d                 TRUE

/* Protocol definition */
#ifndef gZigbeeProIncluded_d
#define gZigbeeProIncluded_d                FALSE
#endif
/* NWK layer definition */
#ifndef gMeshNetworkingCapability_d
#define gMeshNetworkingCapability_d         TRUE
#endif
#ifndef gNwkValidationCapability_d
#define gNwkValidationCapability_d          TRUE
#endif
#ifndef gNwkGetSetCapability_d
#define gNwkGetSetCapability_d              TRUE
#endif
#ifndef gNwkMulticastCapability_d
#define gNwkMulticastCapability_d           FALSE
#endif
#ifndef gNwkManyToOneCapability_d
#define gNwkManyToOneCapability_d           FALSE
#endif
#ifndef gNwkSymLinkCapability_d
#define gNwkSymLinkCapability_d             FALSE
#endif
/* AF layer definition */
#ifndef gAfVerifyParamCapability_d
#define gAfVerifyParamCapability_d          TRUE
#endif

/* SSP Layer Definition. */
#ifndef gStandardSecurity_d
#define gStandardSecurity_d                 TRUE
#endif

#ifndef gHighSecurity_d
#define gHighSecurity_d                     FALSE
#endif



#endif  /* #if gBeeStackZrRsRNP_d */

/****************************************************************************/

/* ZigBee end device, tree only (no mesh networking), no security. */
#if gBeeStackZedNs_d

#define gEndDevCapability_d                 TRUE

/* Protocol definition */
#ifndef gZigbeeProIncluded_d
#define gZigbeeProIncluded_d                FALSE
#endif
/* NWK layer definition */
#ifndef gMeshNetworkingCapability_d
#define gMeshNetworkingCapability_d         FALSE
#endif
#ifndef gNwkValidationCapability_d
#define gNwkValidationCapability_d          FALSE
#endif
#ifndef gNwkGetSetCapability_d
#define gNwkGetSetCapability_d              TRUE
#endif
#ifndef gNwkMulticastCapability_d
#define gNwkMulticastCapability_d           FALSE
#endif
#ifndef gNwkManyToOneCapability_d
#define gNwkManyToOneCapability_d           FALSE
#endif
#ifndef gNwkSymLinkCapability_d
#define gNwkSymLinkCapability_d             FALSE
#endif
/* AF layer definition */
#ifndef gAfVerifyParamCapability_d
#define gAfVerifyParamCapability_d          TRUE
#endif

/* SSP Layer Definition. */
#ifndef gStandardSecurity_d
#define gStandardSecurity_d                 FALSE  
#endif

#ifndef gHighSecurity_d
#define gHighSecurity_d                     FALSE
#endif



#endif  /* #if gBeeStackZedNs_d */

/****************************************************************************/

/* ZigBee end device, tree only (no mesh networking), residential security. */
#if gBeeStackZedRs_d

#define gEndDevCapability_d                 TRUE

/* Protocol definition */
#ifndef gZigbeeProIncluded_d
#define gZigbeeProIncluded_d                FALSE
#endif
/* NWK layer definition */
#ifndef gMeshNetworkingCapability_d
#define gMeshNetworkingCapability_d         FALSE
#endif
#ifndef gNwkValidationCapability_d
#define gNwkValidationCapability_d          FALSE
#endif
#ifndef gNwkGetSetCapability_d
#define gNwkGetSetCapability_d              TRUE
#endif
#ifndef gNwkMulticastCapability_d
#define gNwkMulticastCapability_d           FALSE
#endif
#ifndef gNwkManyToOneCapability_d
#define gNwkManyToOneCapability_d           FALSE
#endif
#ifndef gNwkSymLinkCapability_d
#define gNwkSymLinkCapability_d             FALSE
#endif
/* AF layer definition */
#ifndef gAfVerifyParamCapability_d
#define gAfVerifyParamCapability_d          TRUE
#endif

/* SSP Layer Definition. */
#ifndef gStandardSecurity_d
#define gStandardSecurity_d                 TRUE
#endif

#ifndef gHighSecurity_d
#define gHighSecurity_d                     FALSE
#endif


#endif  /* #if gBeeStackZedRs_d */

/****************************************************************************/

/* ZigBee end device, tree and mesh networking, no security. */
/* This is a copy of BeeStackZedNS. Mesh networking has no menaing for end */
/* device targets; the RNP versions are present for completeness, and to */
/* simplify choosing a library to match an application. */
#if gBeeStackZedRNP_d

#define gEndDevCapability_d                 TRUE

/* Protocol definition */
#ifndef gZigbeeProIncluded_d
#define gZigbeeProIncluded_d                FALSE
#endif
/* NWK layer definition */
#ifndef gMeshNetworkingCapability_d
#define gMeshNetworkingCapability_d         FALSE
#endif
#ifndef gNwkValidationCapability_d
#define gNwkValidationCapability_d          FALSE
#endif
#ifndef gNwkGetSetCapability_d
#define gNwkGetSetCapability_d              TRUE
#endif
#ifndef gNwkMulticastCapability_d
#define gNwkMulticastCapability_d           FALSE
#endif
#ifndef gNwkManyToOneCapability_d
#define gNwkManyToOneCapability_d           FALSE
#endif
#ifndef gNwkSymLinkCapability_d
#define gNwkSymLinkCapability_d             FALSE
#endif
/* AF layer definition */
#ifndef gAfVerifyParamCapability_d
#define gAfVerifyParamCapability_d          TRUE
#endif

/* SSP Layer Definition. */
#ifndef gStandardSecurity_d
#define gStandardSecurity_d                 FALSE  
#endif

#ifndef gHighSecurity_d
#define gHighSecurity_d                     FALSE
#endif


#endif  /* #if gBeeStackZedRNP_d */

/****************************************************************************/

/* ZigBee end device, tree and mesh networking, residential security. */
/* This is a copy of BeeStackZedNS. Mesh networking has no menaing for end */
/* device targets; the RNP versions are present for completeness, and to */
/* simplify choosing a library to match an application. */
#if gBeeStackZedRsRNP_d

#define gEndDevCapability_d                 TRUE

/* Protocol definition */
#ifndef gZigbeeProIncluded_d
#define gZigbeeProIncluded_d                FALSE
#endif
/* NWK layer definition */
#ifndef gMeshNetworkingCapability_d
#define gMeshNetworkingCapability_d         FALSE
#endif
#ifndef gNwkValidationCapability_d
#define gNwkValidationCapability_d          FALSE
#endif
#ifndef gNwkGetSetCapability_d
#define gNwkGetSetCapability_d              TRUE
#endif
#ifndef gNwkMulticastCapability_d
#define gNwkMulticastCapability_d           FALSE
#endif
#ifndef gNwkManyToOneCapability_d
#define gNwkManyToOneCapability_d           FALSE
#endif
#ifndef gNwkSymLinkCapability_d
#define gNwkSymLinkCapability_d             FALSE
#endif
/* AF layer definition */
#ifndef gAfVerifyParamCapability_d
#define gAfVerifyParamCapability_d          TRUE
#endif

/* SSP Layer Definition. */
#ifndef gStandardSecurity_d
#define gStandardSecurity_d                 TRUE
#endif

#ifndef gHighSecurity_d
#define gHighSecurity_d                     FALSE
#endif



#endif  /* #if gBeeStackZedRsRNP_d */

/****************************************************************************/
#if gBeeStackZcRNPSp2_d
#define gCoordinatorCapability_d            TRUE

/* Protocol definition */
#ifndef gZigbeeProIncluded_d
#define gZigbeeProIncluded_d                TRUE
#endif
/* NWK layer definition */
#ifndef gNwkValidationCapability_d
#define gNwkValidationCapability_d          TRUE    /* Turn off if out of ROM */
#endif
#ifndef gNwkGetSetCapability_d
#define gNwkGetSetCapability_d              TRUE    /* Turn off if out of ROM */
#endif
/* AF layer definition */
#ifndef gAfVerifyParamCapability_d
#define gAfVerifyParamCapability_d          TRUE    /* Turn off if out of ROM */
#endif
/* SSP Layer Definition. */
#ifndef gStandardSecurity_d
#define gStandardSecurity_d                 FALSE   /* On/Off Standar Security. */
#endif

#ifndef gHighSecurity_d
#define gHighSecurity_d                     FALSE
#endif


#endif /*gBeeStackZcRNPSp2_d*/
/****************************************************************************/
#if gBeeStackZcRsRNPSp2_d

#define gCoordinatorCapability_d            TRUE

/* Protocol definition */
#ifndef gZigbeeProIncluded_d
#define gZigbeeProIncluded_d                TRUE
#endif
/* NWK layer definition */
#ifndef gNwkValidationCapability_d
#define gNwkValidationCapability_d          TRUE    /* Turn off if out of ROM */
#endif
#ifndef gNwkGetSetCapability_d
#define gNwkGetSetCapability_d              TRUE    /* Turn off if out of ROM */
#endif
/* AF layer definition */
#ifndef gAfVerifyParamCapability_d
#define gAfVerifyParamCapability_d          TRUE    /* Turn off if out of ROM */
#endif
/* SSP Layer Definition. */
#ifndef gStandardSecurity_d
#define gStandardSecurity_d                 TRUE    /* On/Off Standar Security. */
#endif

#ifndef gHighSecurity_d
#define gHighSecurity_d                     FALSE
#endif


#endif 
/******************************************************************************/
#if gBeeStackZcHsRNPSp2_d

#define gCoordinatorCapability_d            TRUE

/* Protocol definition */
#ifndef gZigbeeProIncluded_d
#define gZigbeeProIncluded_d                TRUE
#endif
/* NWK layer definition */
#ifndef gNwkValidationCapability_d
#define gNwkValidationCapability_d          TRUE    /* Turn off if out of ROM */
#endif
#ifndef gNwkGetSetCapability_d
#define gNwkGetSetCapability_d              TRUE    /* Turn off if out of ROM */
#endif
/* AF layer definition */
#ifndef gAfVerifyParamCapability_d
#define gAfVerifyParamCapability_d          TRUE    /* Turn off if out of ROM */
#endif
/* SSP Layer Definition. */
#ifndef gStandardSecurity_d
#define gStandardSecurity_d                 FALSE   /* On/Off Standar Security. */
#endif

#ifndef gHighSecurity_d
#define gHighSecurity_d                     TRUE
#endif

#endif
/******************************************************************************/

#if gBeeStackZedRNPSp2_d
#define gEndDevCapability_d                 TRUE

/* Protocol definition */
#ifndef gZigbeeProIncluded_d
#define gZigbeeProIncluded_d                TRUE
#endif
/* NWK layer definition */
#ifndef gNwkValidationCapability_d
#define gNwkValidationCapability_d          FALSE    /* Turn off if out of ROM */
#endif
#ifndef gNwkGetSetCapability_d
#define gNwkGetSetCapability_d              TRUE    /* Turn off if out of ROM */
#endif
/* AF layer definition */
#ifndef gAfVerifyParamCapability_d
#define gAfVerifyParamCapability_d          TRUE    /* Turn off if out of ROM */
#endif
/* SSP Layer Definition. */
#ifndef gStandardSecurity_d
#define gStandardSecurity_d                 FALSE   /* On/Off Standar Security. */
#endif

#ifndef gHighSecurity_d
#define gHighSecurity_d                     FALSE
#endif


#endif
/******************************************************************************/
#if gBeeStackZedRsRNPSp2_d
#define gEndDevCapability_d                 TRUE

/* Protocol definition */
#ifndef gZigbeeProIncluded_d
#define gZigbeeProIncluded_d                TRUE
#endif
/* NWK layer definition */
#ifndef gNwkValidationCapability_d
#define gNwkValidationCapability_d          FALSE    /* Turn off if out of ROM */
#endif
#ifndef gNwkGetSetCapability_d
#define gNwkGetSetCapability_d              TRUE    /* Turn off if out of ROM */
#endif
/* AF layer definition */
#ifndef gAfVerifyParamCapability_d
#define gAfVerifyParamCapability_d          TRUE    /* Turn off if out of ROM */
#endif
/* SSP Layer Definition. */
#ifndef gStandardSecurity_d
#define gStandardSecurity_d                 TRUE    /* On/Off Standar Security. */
#endif

#ifndef gHighSecurity_d
#define gHighSecurity_d                     FALSE
#endif


#endif
/******************************************************************************/

#if gBeeStackZedHsRNPSp2_d

#define gEndDevCapability_d                 TRUE

/* Protocol definition */
#ifndef gZigbeeProIncluded_d
#define gZigbeeProIncluded_d                TRUE
#endif
/* NWK layer definition */
#ifndef gNwkValidationCapability_d
#define gNwkValidationCapability_d          FALSE    /* Turn off if out of ROM */
#endif
#ifndef gNwkGetSetCapability_d
#define gNwkGetSetCapability_d              TRUE    /* Turn off if out of ROM */
#endif
/* AF layer definition */
#ifndef gAfVerifyParamCapability_d
#define gAfVerifyParamCapability_d          TRUE    /* Turn off if out of ROM */
#endif
/* SSP Layer Definition. */
#ifndef gStandardSecurity_d
#define gStandardSecurity_d                 FALSE   /* On/Off Standar Security. */
#endif

#ifndef gHighSecurity_d
#define gHighSecurity_d                     TRUE
#endif


#endif
/*****************************************************************************/

#if gBeeStackZrRNPSp2_d

#define gRouterCapability_d                 TRUE

/* Protocol definition */
#ifndef gZigbeeProIncluded_d
#define gZigbeeProIncluded_d                TRUE
#endif
/* NWK layer definition */
#ifndef gNwkValidationCapability_d
#define gNwkValidationCapability_d          TRUE    /* Turn off if out of ROM */
#endif
#ifndef gNwkGetSetCapability_d
#define gNwkGetSetCapability_d              TRUE    /* Turn off if out of ROM */
#endif
/* AF layer definition */
#ifndef gAfVerifyParamCapability_d
#define gAfVerifyParamCapability_d          TRUE    /* Turn off if out of ROM */
#endif
/* SSP Layer Definition. */
#ifndef gStandardSecurity_d
#define gStandardSecurity_d                 FALSE   /* On/Off Standar Security. */
#endif

#ifndef gHighSecurity_d
#define gHighSecurity_d                     FALSE
#endif


#endif
/******************************************************************************/

#if gBeeStackZrRsRNPSp2_d

#define gRouterCapability_d                 TRUE

/* Protocol definition */
#ifndef gZigbeeProIncluded_d
#define gZigbeeProIncluded_d                TRUE
#endif
/* NWK layer definition */
#ifndef gNwkValidationCapability_d
#define gNwkValidationCapability_d          TRUE    /* Turn off if out of ROM */
#endif
#ifndef gNwkGetSetCapability_d
#define gNwkGetSetCapability_d              TRUE    /* Turn off if out of ROM */
#endif
/* AF layer definition */
#ifndef gAfVerifyParamCapability_d
#define gAfVerifyParamCapability_d          TRUE    /* Turn off if out of ROM */
#endif
/* SSP Layer Definition. */
#ifndef gStandardSecurity_d
#define gStandardSecurity_d                 TRUE    /* On/Off Standar Security. */
#endif

#ifndef gHighSecurity_d
#define gHighSecurity_d                     FALSE
#endif


#endif
/******************************************************************************/
#if gBeeStackZrHsRNPSp2_d

#define gRouterCapability_d                 TRUE

/* Protocol definition */
#ifndef gZigbeeProIncluded_d
#define gZigbeeProIncluded_d                TRUE
#endif
/* NWK layer definition */
#ifndef gNwkValidationCapability_d
#define gNwkValidationCapability_d          TRUE    /* Turn off if out of ROM */
#endif
#ifndef gNwkGetSetCapability_d
#define gNwkGetSetCapability_d              TRUE    /* Turn off if out of ROM */
#endif
/* AF layer definition */
#ifndef gAfVerifyParamCapability_d
#define gAfVerifyParamCapability_d          TRUE    /* Turn off if out of ROM */
#endif
/* SSP Layer Definition. */
#ifndef gStandardSecurity_d
#define gStandardSecurity_d                 FALSE    /* On/Off Standar Security. */
#endif

#ifndef gHighSecurity_d
#define gHighSecurity_d                     TRUE
#endif

#endif

/* Combo device libraries */

#if gBeeStackZxNs_d
#define gComboDeviceCapability_d                 TRUE

/* Protocol definition */
#ifndef gZigbeeProIncluded_d
#define gZigbeeProIncluded_d                FALSE
#endif
/* NWK layer definition */
#ifndef gMeshNetworkingCapability_d
#define gMeshNetworkingCapability_d         FALSE   /* On for RNP Zc or Zr, off otherwise. */
#endif
#ifndef gNwkValidationCapability_d
#define gNwkValidationCapability_d          TRUE    /* Turn off if out of ROM */
#endif
#ifndef gNwkGetSetCapability_d
#define gNwkGetSetCapability_d              TRUE    /* Turn off if out of ROM */
#endif
/* AF layer definition */
#ifndef gAfVerifyParamCapability_d
#define gAfVerifyParamCapability_d          TRUE    /* Turn off if out of ROM */
#endif
/* SSP Layer Definition. */
#ifndef gStandardSecurity_d
#define gStandardSecurity_d                 FALSE   /* On/Off Standar Security. */
#endif

#ifndef gHighSecurity_d
#define gHighSecurity_d                     FALSE
#endif


#endif
/******************************************************************************/
#if gBeeStackZxRNP_d
#define gComboDeviceCapability_d                  TRUE

/* Protocol definition */
#ifndef gZigbeeProIncluded_d
#define gZigbeeProIncluded_d                FALSE
#endif
/* NWK layer definition */
#ifndef gMeshNetworkingCapability_d
#define gMeshNetworkingCapability_d         TRUE   /* On for RNP Zc or Zr, off otherwise. */
#endif
#ifndef gNwkValidationCapability_d
#define gNwkValidationCapability_d          TRUE    /* Turn off if out of ROM */
#endif
#ifndef gNwkGetSetCapability_d
#define gNwkGetSetCapability_d              TRUE    /* Turn off if out of ROM */
#endif
/* AF layer definition */
#ifndef gAfVerifyParamCapability_d
#define gAfVerifyParamCapability_d          TRUE    /* Turn off if out of ROM */
#endif
/* SSP Layer Definition. */
#ifndef gStandardSecurity_d
#define gStandardSecurity_d                 FALSE   /* On/Off Standar Security. */
#endif

#ifndef gHighSecurity_d
#define gHighSecurity_d                     FALSE
#endif


#endif
/******************************************************************************/
#if gBeeStackZxRs_d
#define gComboDeviceCapability_d            TRUE

/* Protocol definition */
#ifndef gZigbeeProIncluded_d
#define gZigbeeProIncluded_d                FALSE
#endif
/* NWK layer definition */
#ifndef gMeshNetworkingCapability_d
#define gMeshNetworkingCapability_d         FALSE   /* On for RNP Zc or Zr, off otherwise. */
#endif
#ifndef gNwkValidationCapability_d
#define gNwkValidationCapability_d          TRUE    /* Turn off if out of ROM */
#endif
#ifndef gNwkGetSetCapability_d
#define gNwkGetSetCapability_d              TRUE    /* Turn off if out of ROM */
#endif
/* AF layer definition */
#ifndef gAfVerifyParamCapability_d
#define gAfVerifyParamCapability_d          TRUE    /* Turn off if out of ROM */
#endif
/* SSP Layer Definition. */
#ifndef gStandardSecurity_d
#define gStandardSecurity_d                 TRUE   /* On/Off Standar Security. */
#endif

#ifndef gHighSecurity_d
#define gHighSecurity_d                     FALSE
#endif


#endif
/******************************************************************************/
#if gBeeStackZxRsRNP_d
#define gComboDeviceCapability_d            TRUE

/* Protocol definition */
#ifndef gZigbeeProIncluded_d
#define gZigbeeProIncluded_d                FALSE
#endif
/* NWK layer definition */
#ifndef gMeshNetworkingCapability_d
#define gMeshNetworkingCapability_d         TRUE   /* On for RNP Zc or Zr, off otherwise. */
#endif
#ifndef gNwkValidationCapability_d
#define gNwkValidationCapability_d          TRUE    /* Turn off if out of ROM */
#endif
#ifndef gNwkGetSetCapability_d
#define gNwkGetSetCapability_d              TRUE    /* Turn off if out of ROM */
#endif
/* AF layer definition */
#ifndef gAfVerifyParamCapability_d
#define gAfVerifyParamCapability_d          TRUE    /* Turn off if out of ROM */
#endif
/* SSP Layer Definition. */
#ifndef gStandardSecurity_d
#define gStandardSecurity_d                 TRUE   /* On/Off Standar Security. */
#endif

#ifndef gHighSecurity_d
#define gHighSecurity_d                     FALSE
#endif


#endif  
/******************************************************************************/
#if gBeeStackZxRNPSp2_d

#define gComboDeviceCapability_d            TRUE

/* Protocol definition */
#ifndef gZigbeeProIncluded_d
#define gZigbeeProIncluded_d                TRUE
#endif
/* NWK layer definition */
#ifndef gNwkValidationCapability_d
#define gNwkValidationCapability_d          TRUE    /* Turn off if out of ROM */
#endif
#ifndef gNwkGetSetCapability_d
#define gNwkGetSetCapability_d              TRUE    /* Turn off if out of ROM */
#endif
/* AF layer definition */
#ifndef gAfVerifyParamCapability_d
#define gAfVerifyParamCapability_d          TRUE    /* Turn off if out of ROM */
#endif
/* SSP Layer Definition. */
#ifndef gStandardSecurity_d
#define gStandardSecurity_d                 FALSE   /* On/Off Standar Security. */
#endif

#ifndef gHighSecurity_d
#define gHighSecurity_d                     FALSE
#endif


#endif
/******************************************************************************/

#if gBeeStackZxRsRNPSp2_d

#define gComboDeviceCapability_d            TRUE

/* Protocol definition */
#ifndef gZigbeeProIncluded_d
#define gZigbeeProIncluded_d                TRUE
#endif
/* NWK layer definition */
#ifndef gNwkValidationCapability_d
#define gNwkValidationCapability_d          TRUE    /* Turn off if out of ROM */
#endif
#ifndef gNwkGetSetCapability_d
#define gNwkGetSetCapability_d              TRUE    /* Turn off if out of ROM */
#endif
/* AF layer definition */
#ifndef gAfVerifyParamCapability_d
#define gAfVerifyParamCapability_d          TRUE    /* Turn off if out of ROM */
#endif
/* SSP Layer Definition. */
#ifndef gStandardSecurity_d
#define gStandardSecurity_d                 TRUE    /* On/Off Standar Security. */
#endif

#ifndef gHighSecurity_d
#define gHighSecurity_d                     FALSE
#endif


#endif
/******************************************************************************/

#if gBeeStackZxHsRNPSp2_d

#define gComboDeviceCapability_d            TRUE

/* Protocol definition */
#ifndef gZigbeeProIncluded_d
#define gZigbeeProIncluded_d                TRUE
#endif
/* NWK layer definition */
#ifndef gNwkValidationCapability_d
#define gNwkValidationCapability_d          TRUE    /* Turn off if out of ROM */
#endif
#ifndef gNwkGetSetCapability_d
#define gNwkGetSetCapability_d              TRUE    /* Turn off if out of ROM */
#endif
/* AF layer definition */
#ifndef gAfVerifyParamCapability_d
#define gAfVerifyParamCapability_d          TRUE    /* Turn off if out of ROM */
#endif
/* SSP Layer Definition. */
#ifndef gStandardSecurity_d
#define gStandardSecurity_d                 FALSE   /* On/Off Standar Security. */
#endif

#ifndef gHighSecurity_d
#define gHighSecurity_d                     TRUE
#endif

#endif
/******************************************************************************/

/* Minimal library */

#if gBeeStackZcMinimalSp1_d
#define gCoordinatorCapability_d            TRUE

#ifdef  gMinimalSet_d
#undef  gMinimalSet_d
#endif
#define gMinimalSet_d                       TRUE

/* Protocol definition */
#ifndef gZigbeeProIncluded_d
#define gZigbeeProIncluded_d                FALSE
#endif
/* NWK layer definition */
#ifndef gMeshNetworkingCapability_d
#define gMeshNetworkingCapability_d         FALSE   /* On for RNP Zc or Zr, off otherwise. */
#endif
#ifndef gNwkValidationCapability_d
#define gNwkValidationCapability_d          FALSE
#endif
#ifndef gNwkGetSetCapability_d
#define gNwkGetSetCapability_d              FALSE
#endif
#ifndef gNwkPanIdConflict_d
#define gNwkPanIdConflict_d                 FALSE
#endif
/* AF layer definition */
#ifndef gAfVerifyParamCapability_d
#define gAfVerifyParamCapability_d          FALSE
#endif

/* ZDO layer definition */
#ifndef gNWK_addr_req_d
#define gNWK_addr_req_d                     FALSE
#endif


/* SSP Layer Definition. */
#ifndef gStandardSecurity_d
#define gStandardSecurity_d                 FALSE
#endif

#ifndef gHighSecurity_d
#define gHighSecurity_d                     FALSE
#endif


#endif
/******************************************************************************/
#if gBeeStackZrMinimalSp1_d
#define gRouterCapability_d                 TRUE

#ifdef  gMinimalSet_d
#undef  gMinimalSet_d
#endif
#define gMinimalSet_d                       TRUE

/* Protocol definition */
#ifndef gZigbeeProIncluded_d
#define gZigbeeProIncluded_d                FALSE
#endif
/* NWK layer definition */
#ifndef gMeshNetworkingCapability_d
#define gMeshNetworkingCapability_d         FALSE   /* On for RNP Zc or Zr, off otherwise. */
#endif
#ifndef gNwkValidationCapability_d
#define gNwkValidationCapability_d          FALSE
#endif
#ifndef gNwkGetSetCapability_d
#define gNwkGetSetCapability_d              FALSE
#endif
#ifndef gNwkPanIdConflict_d
#define gNwkPanIdConflict_d                 FALSE
#endif
/* AF layer definition */
#ifndef gAfVerifyParamCapability_d
#define gAfVerifyParamCapability_d          FALSE
#endif

/* ZDO layer definition */
#ifndef gNWK_addr_req_d
#define gNWK_addr_req_d                     FALSE
#endif

/* SSP Layer Definition. */
#ifndef gStandardSecurity_d
#define gStandardSecurity_d                 FALSE
#endif

#ifndef gHighSecurity_d
#define gHighSecurity_d                     FALSE
#endif


#endif
/******************************************************************************/
#if gBeeStackZedMinimalSp1_d
        
#define gEndDevCapability_d                 TRUE

#ifdef  gMinimalSet_d
#undef  gMinimalSet_d
#endif
#define gMinimalSet_d                       TRUE

/* Protocol definition */
#ifndef gZigbeeProIncluded_d
#define gZigbeeProIncluded_d                FALSE
#endif
/* NWK layer definition */
#ifndef gMeshNetworkingCapability_d
#define gMeshNetworkingCapability_d         FALSE   /* On for RNP Zc or Zr, off otherwise. */
#endif
#ifndef gNwkValidationCapability_d
#define gNwkValidationCapability_d          FALSE
#endif
#ifndef gNwkGetSetCapability_d
#define gNwkGetSetCapability_d              FALSE
#endif
#ifndef gNwkPanIdConflict_d
#define gNwkPanIdConflict_d                 FALSE
#endif
/* AF layer definition */
#ifndef gAfVerifyParamCapability_d
#define gAfVerifyParamCapability_d          FALSE
#endif

/* ZDO layer definition */
#ifndef gNWK_addr_req_d
#define gNWK_addr_req_d                     FALSE
#endif

/* SSP Layer Definition. */
#ifndef gStandardSecurity_d
#define gStandardSecurity_d                 FALSE
#endif

#ifndef gHighSecurity_d
#define gHighSecurity_d                     FALSE
#endif


#endif
/******************************************************************************/
#if gBeeStackZcRsMinimalSp2_d

#define gCoordinatorCapability_d            TRUE

#ifdef  gMinimalSet_d
#undef  gMinimalSet_d
#endif
#define gMinimalSet_d                       TRUE

/* Protocol definition */
#ifndef gZigbeeProIncluded_d
#define gZigbeeProIncluded_d                TRUE
#endif
#ifndef gNwkValidationCapability_d
#define gNwkValidationCapability_d          FALSE
#endif
#ifndef gNwkGetSetCapability_d
#define gNwkGetSetCapability_d              FALSE
#endif
/* AF layer definition */
#ifndef gAfVerifyParamCapability_d
#define gAfVerifyParamCapability_d          FALSE
#endif

/* SSP Layer Definition. */
#ifndef gStandardSecurity_d
#define gStandardSecurity_d                 TRUE
#endif

#ifndef gHighSecurity_d
#define gHighSecurity_d                     FALSE
#endif


#endif
/******************************************************************************/
#if gBeeStackZrRsMinimalSp2_d

#define gRouterCapability_d                 TRUE

#ifdef  gMinimalSet_d
#undef  gMinimalSet_d
#endif
#define gMinimalSet_d                       TRUE

/* Protocol definition */
#ifndef gZigbeeProIncluded_d
#define gZigbeeProIncluded_d                TRUE
#endif
/* NWK layer definition */
#ifndef gNwkValidationCapability_d
#define gNwkValidationCapability_d          FALSE
#endif
#ifndef gNwkGetSetCapability_d
#define gNwkGetSetCapability_d              FALSE
#endif
/* AF layer definition */
#ifndef gAfVerifyParamCapability_d
#define gAfVerifyParamCapability_d          FALSE
#endif

/* SSP Layer Definition. */
#ifndef gStandardSecurity_d
#define gStandardSecurity_d                 TRUE
#endif

#ifndef gHighSecurity_d
#define gHighSecurity_d                     FALSE
#endif


#endif
/******************************************************************************/
#if gBeeStackZedRsMinimalSp2_d

#define gEndDevCapability_d                 TRUE

#ifdef  gMinimalSet_d
#undef  gMinimalSet_d
#endif
#define gMinimalSet_d                       TRUE

/* Protocol definition */
#ifndef gZigbeeProIncluded_d
#define gZigbeeProIncluded_d                TRUE
#endif
/* NWK layer definition */
#ifndef gNwkValidationCapability_d
#define gNwkValidationCapability_d          FALSE
#endif
#ifndef gNwkGetSetCapability_d
#define gNwkGetSetCapability_d              FALSE
#endif
/* AF layer definition */
#ifndef gAfVerifyParamCapability_d
#define gAfVerifyParamCapability_d          FALSE
#endif

/* SSP Layer Definition. */
#ifndef gStandardSecurity_d
#define gStandardSecurity_d                 TRUE
#endif

#ifndef gHighSecurity_d
#define gHighSecurity_d                     FALSE
#endif

#endif
/******************************************************************************/
#if gBeeStackZcHsMinimalSp2_d

#define gCoordinatorCapability_d            TRUE

#ifdef  gMinimalSet_d
#undef  gMinimalSet_d
#endif
#define gMinimalSet_d                       TRUE

/* Protocol definition */
#ifndef gZigbeeProIncluded_d
#define gZigbeeProIncluded_d                TRUE
#endif

/* NWK layer definition */

#ifndef gNwkValidationCapability_d
#define gNwkValidationCapability_d          FALSE
#endif

#ifndef gNwkGetSetCapability_d
#define gNwkGetSetCapability_d              FALSE
#endif

#ifndef gNwkSymLinkCapability_d
#define gNwkSymLinkCapability_d             TRUE
#endif

/* AF layer definition */
#ifndef gAfVerifyParamCapability_d
#define gAfVerifyParamCapability_d          FALSE
#endif

/* SSP Layer Definition. */
#ifndef gStandardSecurity_d
#define gStandardSecurity_d                 FALSE
#endif

#ifndef gHighSecurity_d
#define gHighSecurity_d                     TRUE
#endif


#endif
/******************************************************************************/
#if gBeeStackZrHsMinimalSp2_d

#define gRouterCapability_d            TRUE

#ifdef  gMinimalSet_d
#undef  gMinimalSet_d
#endif
#define gMinimalSet_d                       TRUE

/* Protocol definition */
#ifndef gZigbeeProIncluded_d
#define gZigbeeProIncluded_d                TRUE
#endif

/* NWK layer definition */

#ifndef gNwkValidationCapability_d
#define gNwkValidationCapability_d          FALSE
#endif

#ifndef gNwkGetSetCapability_d
#define gNwkGetSetCapability_d              FALSE
#endif

#ifndef gNwkSymLinkCapability_d
#define gNwkSymLinkCapability_d             TRUE
#endif


/* AF layer definition */
#ifndef gAfVerifyParamCapability_d
#define gAfVerifyParamCapability_d          FALSE
#endif

/* SSP Layer Definition. */
#ifndef gStandardSecurity_d
#define gStandardSecurity_d                 FALSE
#endif

#ifndef gHighSecurity_d
#define gHighSecurity_d                     TRUE
#endif


#endif
/******************************************************************************/
#if gBeeStackZedHsMinimalSp2_d

#define gEndDevCapability_d                 TRUE

#ifdef  gMinimalSet_d
#undef  gMinimalSet_d
#endif
#define gMinimalSet_d                       TRUE

/* Protocol definition */
#ifndef gZigbeeProIncluded_d
#define gZigbeeProIncluded_d                TRUE
#endif

/* NWK layer definition */

#ifndef gNwkValidationCapability_d
#define gNwkValidationCapability_d          FALSE
#endif

#ifndef gNwkGetSetCapability_d
#define gNwkGetSetCapability_d              FALSE
#endif

#ifndef gNwkSymLinkCapability_d
#define gNwkSymLinkCapability_d             FALSE
#endif


/* AF layer definition */
#ifndef gAfVerifyParamCapability_d
#define gAfVerifyParamCapability_d          FALSE
#endif

/* SSP Layer Definition. */
#ifndef gStandardSecurity_d
#define gStandardSecurity_d                 FALSE
#endif

#ifndef gHighSecurity_d
#define gHighSecurity_d                     TRUE
#endif


#endif
/******************************************************************************/ 
#if gBeeStackZcFullLoadSp1_d
#define gCoordinatorCapability_d            TRUE

/* Protocol definition */
#ifndef gZigbeeProIncluded_d
#define gZigbeeProIncluded_d                FALSE
#endif
/* NWK layer definition */
#ifndef gMeshNetworkingCapability_d
#define gMeshNetworkingCapability_d         TRUE   /* On for RNP Zc or Zr, off otherwise. */
#endif
#ifndef gNwkValidationCapability_d
#define gNwkValidationCapability_d          TRUE
#endif
#ifndef gNwkGetSetCapability_d
#define gNwkGetSetCapability_d              TRUE
#endif
/* AF layer definition */
#ifndef gAfVerifyParamCapability_d
#define gAfVerifyParamCapability_d          TRUE
#endif
/* Forbiden in SP1 */
#ifndef gNwkMulticastCapability_d
#define gNwkMulticastCapability_d           FALSE
#endif
/* Forbiden in SP1 */
#ifndef gNwkManyToOneCapability_d
#define gNwkManyToOneCapability_d           FALSE
#endif
#ifndef gNwkSymLinkCapability_d
#define gNwkSymLinkCapability_d             FALSE
#endif

/* SSP Layer Definition. */
#ifndef gStandardSecurity_d
#define gStandardSecurity_d                 TRUE
#endif

#ifndef gHighSecurity_d
#define gHighSecurity_d                     FALSE
#endif


#ifndef gMsgDebugEnabled_d
#define gMsgDebugEnabled_d                  TRUE
#endif
#ifndef gNvStorageIncluded_d
#define gNvStorageIncluded_d                TRUE
#endif

#endif
/******************************************************************************/ 

#if gBeeStackZrFullLoadSp1_d
#define gRouterCapability_d            TRUE

/* Protocol definition */
#ifndef gZigbeeProIncluded_d
#define gZigbeeProIncluded_d                FALSE
#endif
/* NWK layer definition */
#ifndef gMeshNetworkingCapability_d
#define gMeshNetworkingCapability_d         TRUE   /* On for RNP Zc or Zr, off otherwise. */
#endif
#ifndef gNwkValidationCapability_d
#define gNwkValidationCapability_d          TRUE
#endif
#ifndef gNwkGetSetCapability_d
#define gNwkGetSetCapability_d              TRUE
#endif
/* AF layer definition */
#ifndef gAfVerifyParamCapability_d
#define gAfVerifyParamCapability_d          TRUE
#endif
/* Forbiden in SP1 */
#ifndef gNwkMulticastCapability_d
#define gNwkMulticastCapability_d           FALSE
#endif
/* Forbiden in SP1 */
#ifndef gNwkManyToOneCapability_d
#define gNwkManyToOneCapability_d           FALSE
#endif
#ifndef gNwkSymLinkCapability_d
#define gNwkSymLinkCapability_d             FALSE
#endif

/* SSP Layer Definition. */
#ifndef gStandardSecurity_d
#define gStandardSecurity_d                 TRUE
#endif

#ifndef gHighSecurity_d
#define gHighSecurity_d                     FALSE
#endif


#ifndef gMsgDebugEnabled_d
#define gMsgDebugEnabled_d                  TRUE
#endif
#ifndef gNvStorageIncluded_d
#define gNvStorageIncluded_d                TRUE
#endif

#endif
/******************************************************************************/ 
#if gBeeStackZedFullLoadSp1_d
#define gEndDevCapability_d            TRUE

/* Protocol definition */
#ifndef gZigbeeProIncluded_d
#define gZigbeeProIncluded_d                FALSE
#endif
/* NWK layer definition */
#ifndef gMeshNetworkingCapability_d
#define gMeshNetworkingCapability_d         TRUE   /* On for RNP Zc or Zr, off otherwise. */
#endif
#ifndef gNwkValidationCapability_d
#define gNwkValidationCapability_d          TRUE
#endif
#ifndef gNwkGetSetCapability_d
#define gNwkGetSetCapability_d              TRUE
#endif
/* Forbiden in SP1 */
#ifndef gNwkMulticastCapability_d
#define gNwkMulticastCapability_d           FALSE
#endif
/* Forbiden in SP1 */
#ifndef gNwkManyToOneCapability_d
#define gNwkManyToOneCapability_d           FALSE
#endif
#ifndef gNwkSymLinkCapability_d
#define gNwkSymLinkCapability_d             FALSE
#endif

/* AF layer definition */
#ifndef gAfVerifyParamCapability_d
#define gAfVerifyParamCapability_d          TRUE
#endif

/* SSP Layer Definition. */
#ifndef gStandardSecurity_d
#define gStandardSecurity_d                 TRUE
#endif

#ifndef gHighSecurity_d
#define gHighSecurity_d                     FALSE
#endif


#ifndef gMsgDebugEnabled_d
#define gMsgDebugEnabled_d                  TRUE
#endif
#ifndef gNvStorageIncluded_d
#define gNvStorageIncluded_d                TRUE
#endif

#endif
/******************************************************************************/ 

#if gBeeStackZcFullLoadSp2_d
#define gCoordinatorCapability_d            TRUE

/* Protocol definition */
#ifndef gZigbeeProIncluded_d
#define gZigbeeProIncluded_d                TRUE
#endif
/* NWK layer definition */
#ifndef gMeshNetworkingCapability_d
#define gMeshNetworkingCapability_d         TRUE   /* On for RNP Zc or Zr, off otherwise. */
#endif
#ifndef gNwkValidationCapability_d
#define gNwkValidationCapability_d          TRUE
#endif
#ifndef gNwkGetSetCapability_d
#define gNwkGetSetCapability_d              TRUE
#endif
#ifndef gNwkMulticastCapability_d
#define gNwkMulticastCapability_d           TRUE
#endif
#ifndef gNwkManyToOneCapability_d
#define gNwkManyToOneCapability_d           TRUE
#endif
#ifndef gNwkSymLinkCapability_d
#define gNwkSymLinkCapability_d             TRUE
#endif

/* AF layer definition */
#ifndef gAfVerifyParamCapability_d
#define gAfVerifyParamCapability_d          TRUE
#endif

/* SSP Layer Definition. */
#ifndef gStandardSecurity_d
#define gStandardSecurity_d                 TRUE
#endif

#ifndef gHighSecurity_d
#define gHighSecurity_d                     FALSE
#endif


#ifndef gMsgDebugEnabled_d
#define gMsgDebugEnabled_d                  TRUE
#endif
#ifndef gNvStorageIncluded_d
#define gNvStorageIncluded_d                TRUE
#endif


#endif
/******************************************************************************/ 

#if gBeeStackZrFullLoadSp2_d
#define gRouterCapability_d            TRUE

/* Protocol definition */
#ifndef gZigbeeProIncluded_d
#define gZigbeeProIncluded_d                TRUE
#endif
/* NWK layer definition */
#ifndef gMeshNetworkingCapability_d
#define gMeshNetworkingCapability_d         TRUE   /* On for RNP Zc or Zr, off otherwise. */
#endif
#ifndef gNwkValidationCapability_d
#define gNwkValidationCapability_d          TRUE
#endif
#ifndef gNwkGetSetCapability_d
#define gNwkGetSetCapability_d              TRUE
#endif
#ifndef gNwkMulticastCapability_d
#define gNwkMulticastCapability_d           TRUE
#endif
#ifndef gNwkManyToOneCapability_d
#define gNwkManyToOneCapability_d           TRUE
#endif
#ifndef gNwkSymLinkCapability_d
#define gNwkSymLinkCapability_d             TRUE
#endif

/* AF layer definition */
#ifndef gAfVerifyParamCapability_d
#define gAfVerifyParamCapability_d          TRUE
#endif

/* SSP Layer Definition. */
#ifndef gStandardSecurity_d
#define gStandardSecurity_d                 TRUE
#endif

#ifndef gHighSecurity_d
#define gHighSecurity_d                     FALSE
#endif


#ifndef gMsgDebugEnabled_d
#define gMsgDebugEnabled_d                  TRUE
#endif
#ifndef gNvStorageIncluded_d
#define gNvStorageIncluded_d                TRUE
#endif

#endif
/******************************************************************************/ 
#if gBeeStackZedFullLoadSp2_d
#define gEndDevCapability_d            TRUE

/* Protocol definition */
#ifndef gZigbeeProIncluded_d
#define gZigbeeProIncluded_d                TRUE
#endif
/* NWK layer definition */
#ifndef gMeshNetworkingCapability_d
#define gMeshNetworkingCapability_d         TRUE   /* On for RNP Zc or Zr, off otherwise. */
#endif
#ifndef gNwkValidationCapability_d
#define gNwkValidationCapability_d          TRUE
#endif
#ifndef gNwkGetSetCapability_d
#define gNwkGetSetCapability_d              TRUE
#endif
#ifndef gNwkMulticastCapability_d
#define gNwkMulticastCapability_d           TRUE
#endif
#ifndef gNwkManyToOneCapability_d
#define gNwkManyToOneCapability_d           TRUE
#endif
#ifndef gNwkSymLinkCapability_d
#define gNwkSymLinkCapability_d             TRUE
#endif

/* AF layer definition */
#ifndef gAfVerifyParamCapability_d
#define gAfVerifyParamCapability_d          TRUE
#endif

/* SSP Layer Definition. */
#ifndef gStandardSecurity_d
#define gStandardSecurity_d                 TRUE
#endif

#ifndef gHighSecurity_d
#define gHighSecurity_d                     FALSE
#endif


#ifndef gMsgDebugEnabled_d
#define gMsgDebugEnabled_d                  TRUE
#endif
#ifndef gNvStorageIncluded_d
#define gNvStorageIncluded_d                TRUE
#endif

#endif
/******************************************************************************/ 
/* Everything false for minimal sets */
#if gMinimalSet_d
/* These default values are almost always correct. */

#ifndef gApsAckCapability_d
#define gApsAckCapability_d                         FALSE
#endif

#ifndef gBindCapability_d
#define gBindCapability_d                           FALSE
#endif

#ifndef gNWK_addr_req_d
#define gNWK_addr_req_d                             FALSE
#endif
/* fragmentation in the stack (not app) portion */
#ifndef gFragmentationInAps_d
#define gFragmentationInAps_d                       FALSE
#endif

#ifndef gNwkBroadcastPassiveAckRetryCapability_d
#define gNwkBroadcastPassiveAckRetryCapability_d    FALSE
#endif

#ifndef gNwkDirectJoinSupportCapability_d
#define gNwkDirectJoinSupportCapability_d           FALSE
#endif

/* This define enable or disable the Pan Identifier conflict option, 
   ZigBee Spec 053474r17 
*/
#ifndef gNwkPanIdConflict_d
#define gNwkPanIdConflict_d                         FALSE
#endif
/*
  ZigBee Spec 053474r17
  3.6.6 Multicast Communication   
*/
#ifndef gNwkMulticastCapability_d
#define gNwkMulticastCapability_d                 FALSE
#endif

#ifndef gNwkManyToOneCapability_d
#define gNwkManyToOneCapability_d                 FALSE
#endif

#ifndef gBlockAddressingCapability_d
#define gBlockAddressingCapability_d              FALSE
#endif

#ifndef gMacSecurityCapability_d
#define gMacSecurityCapability_d                  FALSE
#endif

#ifndef gBeaconSupportCapability_d
#define gBeaconSupportCapability_d                FALSE
#endif

#ifndef gEntityAuthenticationEnable_d
#define gEntityAuthenticationEnable_d             FALSE
#endif

#ifndef gMsgDebugEnabled_d
#define gMsgDebugEnabled_d                        FALSE
#endif

#ifndef gNwkSymLinkCapability_d
#define gNwkSymLinkCapability_d                   FALSE
#endif

#endif /* gMinimalSet_d */

#ifndef gZigbeeProIncluded_d
#define gZigbeeProIncluded_d                TRUE
#endif

/* Set derived values. */

#if gZigbeeProIncluded_d
#define gDefaultValueOfNwkStackProfile_c    0x02 
/* Mesh is mandatory for SP2 */
#ifdef gMeshNetworkingCapability_d
#undef gMeshNetworkingCapability_d         
#endif
#define gMeshNetworkingCapability_d         TRUE
#else
#define gDefaultValueOfNwkStackProfile_c    0x01
#endif

/**************************************/

#ifndef gCoordinatorCapability_d
#define gCoordinatorCapability_d    FALSE
#endif

#ifndef gRouterCapability_d
#define gRouterCapability_d         FALSE
#endif

#ifndef gEndDevCapability_d
#define gEndDevCapability_d         FALSE
#endif

#ifndef gComboDeviceCapability_d
#define gComboDeviceCapability_d    FALSE
#endif

/**************************************/

/* These default values are almost always correct. */

#ifndef gApsAckCapability_d
#define gApsAckCapability_d                         TRUE
#endif

#ifndef gBindCapability_d
#define gBindCapability_d                           TRUE
#endif

/* fragmentation in the stack (not app) portion */
#ifndef gFragmentationInAps_d
#define gFragmentationInAps_d                       TRUE
#endif

#ifndef gNwkBroadcastPassiveAckRetryCapability_d
#define gNwkBroadcastPassiveAckRetryCapability_d    FALSE
#endif

#ifndef gNwkDirectJoinSupportCapability_d
#define gNwkDirectJoinSupportCapability_d           TRUE
#endif

#ifndef gNwkSymLinkCapability_d
#define gNwkSymLinkCapability_d                     TRUE
#endif

/* This define enable or disable the Pan Identifier conflict option, 
   ZigBee Spec 053474r17 
*/
#ifndef gNwkPanIdConflict_d
#define gNwkPanIdConflict_d                 TRUE
#endif

/*
  Enables or disables corrupt beacon detection based on the reserved octets in
  the beacon payload. This is used by the pan id conflict detection mechanism
  to decrease the chance of corrupted beacons triggering pan id conflict resolution
*/
#ifndef gCorruptBeaconDetection_c
#define gCorruptBeaconDetection_c FALSE
#endif

/* 
  Symmetric links prevent the problem of one node with a powerful PA that can't hear 
  other nodes causing false routes.
  
  TRUE  routes are considers to be comprised of symmetric links. Backward
        and forward routes are created during one route discovery and they are
        identical.
  FALSE routes are not consider to be comprised of symmetric links. Only
        the forward route is stored during route discovery

  Default: TRUE
*/
#ifndef mNwkSymmetryLink_c
#define mNwkSymmetryLink_c                     TRUE
#endif


#ifndef gMeshNetworkingCapability_d
#define gMeshNetworkingCapability_d         TRUE
#endif

/*
  ZigBee Spec 053474r17
  3.6.6 Multicast Communication 
*/
#ifndef gNwkMulticastCapability_d
#define gNwkMulticastCapability_d           TRUE
#endif

#ifndef gNwkManyToOneCapability_d
#define gNwkManyToOneCapability_d           TRUE
#endif

#ifndef gBlockAddressingCapability_d
#define gBlockAddressingCapability_d        FALSE
#endif

#ifndef gMacSecurityCapability_d
#define gMacSecurityCapability_d            FALSE
#endif

#ifndef gBeaconSupportCapability_d
#define gBeaconSupportCapability_d          FALSE
#endif

#ifndef gEntityAuthenticationEnable_d
#define gEntityAuthenticationEnable_d       FALSE
#endif

/* some generic modules need this */
#ifndef gBeeStackIncluded_d
#define gBeeStackIncluded_d                 TRUE
#endif

#ifndef gMsgDebugEnabled_d
#define gMsgDebugEnabled_d                  FALSE
#endif

/**************************************/

/* Security features. */

#ifndef gStandardSecurity_d
#define gStandardSecurity_d                 FALSE   /* On/Off Standar Security. */
#endif

#ifndef gHighSecurity_d
#define gHighSecurity_d                     FALSE   /* On/Off High Security. */
#endif


#ifndef gEntityAuthenticationEnable_d
#define gEntityAuthenticationEnable_d       FALSE
#endif

/* When BeeStack is configured for use the profile 1 the node has the distrubuted addressing 
capability otherwise it will uses the Stochastic addressing capability. */
#if( gDefaultValueOfNwkStackProfile_c == 0x01)   
  #define gStochasticAddressingCapability_d   FALSE
  #undef  gDistributedAddressingCapability_d
  #define gDistributedAddressingCapability_d  TRUE
#endif
#if( gDefaultValueOfNwkStackProfile_c == 0x02)   
  #undef  gStochasticAddressingCapability_d
  #define gStochasticAddressingCapability_d   TRUE
  #define gDistributedAddressingCapability_d  FALSE
#endif

/* No High security for SP1 */
#if gHighSecurity_d && (gDefaultValueOfNwkStackProfile_c == 0x01)
#undef gHighSecurity_d
#define gHighSecurity_d FALSE
#endif

/* No Many to one for  SP1 */
#if gNwkManyToOneCapability_d && (gDefaultValueOfNwkStackProfile_c == 0x01)
#undef gNwkManyToOneCapability_d
#define gNwkManyToOneCapability_d FALSE
#endif


/* No Mesh for End Devices*/
#if gEndDevCapability_d && gMeshNetworkingCapability_d
#undef gMeshNetworkingCapability_d
#define gMeshNetworkingCapability_d FALSE
#endif

/* SP1 doesn't have SymLink */
#if (gDefaultValueOfNwkStackProfile_c == 0x01)

#ifdef  gNwkSymLinkCapability_d
#undef  gNwkSymLinkCapability_d
#endif
#define gNwkSymLinkCapability_d  FALSE

#ifdef mNwkSymmetryLink_c
#undef mNwkSymmetryLink_c
#endif
#define mNwkSymmetryLink_c  FALSE

#ifdef  gNwkMulticastCapability_d
#undef  gNwkMulticastCapability_d
#endif
#define  gNwkMulticastCapability_d  FALSE

#ifdef  gNwkManyToOneCapability_d
#undef  gNwkManyToOneCapability_d
#endif
#define  gNwkManyToOneCapability_d  FALSE

#ifdef  gNwkValidationCapability_d
#undef  gNwkValidationCapability_d
#endif
#define  gNwkValidationCapability_d  FALSE

#ifdef  gStochasticAddressingCapability_d
#undef  gStochasticAddressingCapability_d
#endif
#define  gStochasticAddressingCapability_d  FALSE

#endif



/*********************************************************************************************/
#ifdef gMeshNetworkingCapability_d
#define gRnplusCapability_d                 gMeshNetworkingCapability_d
#endif

#ifndef gRnplusCapability_d
#define gRnplusCapability_d                 FALSE
#endif


#ifndef gRnminusCapability_d
#define gRnminusCapability_d                (!gRnplusCapability_d)
#endif



#if gCoordinatorCapability_d
#define gStaticAddressingCapability_d       TRUE
#define gTrustCenter_d                      (gStandardSecurity_d || gHighSecurity_d)
#endif

#if gRouterCapability_d
#define gStaticAddressingCapability_d       TRUE
#define gTrustCenter_d                      FALSE
#endif

#if gEndDevCapability_d
#define gStaticAddressingCapability_d       FALSE
#define gTrustCenter_d                      FALSE
#endif

#if gComboDeviceCapability_d
#define gStaticAddressingCapability_d       TRUE
#define gTrustCenter_d                      FALSE
#endif

#endif _BEESTACK_FUNCTIONALITY_H_
