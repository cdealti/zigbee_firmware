/******************************************************************************
* CSkipCalc.c
*
* Copyright (c) 2008, Freescale, Inc.  All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from  Freescale Semiconductor.
*
* This file contains the declarations for different tables used in BeeStack.
*
******************************************************************************/

#if ((gNwkMaximumDepth_c < 1) || (gNwkMaximumDepth_c > 15))
#error gNwkMaximumDepth_c must range from 1 to 15
#endif

#if ((gNwkMaximumChildren_c < 1) || (gNwkMaximumChildren_c > 64))
#error gNwkMaximumChildren_c must range from 1 to 64
#endif

#if ((gNwkMaximumRouters_c < 1) || (gNwkMaximumRouters_c > gNwkMaximumChildren_c))
#error gNwkMaximumRouters_c must range from 1 to gNwkMaximumChildren_c
#endif

/* determine # of children for end device */
#define mMaxEndDeviceChildren_c  (gNwkMaximumChildren_c - gNwkMaximumRouters_c)

/* The Macro CSkipArray defines the array for CSkip, 1-15 entries, each of which is 16-bits */
#if (gNwkMaximumDepth_c == 1)
#define CSkipDepth0_c	1
#define CSkipArray_c	CSkipDepth0_c

/* no error checking needed for gNwkMaximumDepth_c == 2 */
#endif

#if (gNwkMaximumDepth_c == 2)
#define CSkipDepth1_c	1
#define CSkipDepth0_c	(CSkipDepth1_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipArray_c	CSkipDepth0_c, CSkipDepth1_c

/* no error checking needed for gNwkMaximumDepth_c == 2 */
#endif

#if (gNwkMaximumDepth_c == 3)
#define CSkipDepth2_c	1
#define CSkipDepth1_c	(CSkipDepth2_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth0_c	(CSkipDepth1_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipArray_c	CSkipDepth0_c, CSkipDepth1_c, CSkipDepth2_c

/* warning: CSkip can be too large. Check in spreadsheet first! */
#endif	/* gNwkMaximumDepth_c == 3 */

#if (gNwkMaximumDepth_c == 4)
#define CSkipDepth3_c	1
#define CSkipDepth2_c	(CSkipDepth3_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth1_c	(CSkipDepth2_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth0_c	(CSkipDepth1_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipArray_c	CSkipDepth0_c, CSkipDepth1_c, CSkipDepth2_c, CSkipDepth3_c
#endif	/* gNwkMaximumDepth_c == 4 */

#if (gNwkMaximumDepth_c == 5)
#define CSkipDepth4_c	1
#define CSkipDepth3_c	(CSkipDepth4_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth2_c	(CSkipDepth3_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth1_c	(CSkipDepth2_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth0_c	(CSkipDepth1_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipArray_c	CSkipDepth0_c, CSkipDepth1_c, CSkipDepth2_c, CSkipDepth3_c, CSkipDepth4_c
#endif	/* gNwkMaximumDepth_c == 5 */

#if (gNwkMaximumDepth_c == 6)
#define CSkipDepth5_c	1
#define CSkipDepth4_c	(CSkipDepth5_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth3_c	(CSkipDepth4_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth2_c	(CSkipDepth3_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth1_c	(CSkipDepth2_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth0_c	(CSkipDepth1_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipArray_c	CSkipDepth0_c, CSkipDepth1_c, CSkipDepth2_c, CSkipDepth3_c, CSkipDepth4_c, \
	CSkipDepth5_c
#endif	/* gNwkMaximumDepth_c == 6 */

#if (gNwkMaximumDepth_c == 7)
#define CSkipDepth6_c	1
#define CSkipDepth5_c	(CSkipDepth6_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth4_c	(CSkipDepth5_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth3_c	(CSkipDepth4_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth2_c	(CSkipDepth3_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth1_c	(CSkipDepth2_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth0_c	(CSkipDepth1_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipArray_c	CSkipDepth0_c, CSkipDepth1_c, CSkipDepth2_c, CSkipDepth3_c, CSkipDepth4_c, \
	CSkipDepth5_c, CSkipDepth6_c
#endif	/* gNwkMaximumDepth_c == 7 */

#if (gNwkMaximumDepth_c == 8)
#define CSkipDepth7_c	1
#define CSkipDepth6_c	(CSkipDepth7_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth5_c	(CSkipDepth6_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth4_c	(CSkipDepth5_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth3_c	(CSkipDepth4_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth2_c	(CSkipDepth3_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth1_c	(CSkipDepth2_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth0_c	(CSkipDepth1_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipArray_c	CSkipDepth0_c, CSkipDepth1_c, CSkipDepth2_c, CSkipDepth3_c, CSkipDepth4_c, \
	CSkipDepth5_c, CSkipDepth6_c, CSkipDepth7_c
#endif	/* gNwkMaximumDepth_c == 8 */

#if (gNwkMaximumDepth_c == 9)
#define CSkipDepth8_c	1
#define CSkipDepth7_c	(CSkipDepth8_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth6_c	(CSkipDepth7_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth5_c	(CSkipDepth6_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth4_c	(CSkipDepth5_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth3_c	(CSkipDepth4_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth2_c	(CSkipDepth3_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth1_c	(CSkipDepth2_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth0_c	(CSkipDepth1_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipArray_c	CSkipDepth0_c, CSkipDepth1_c, CSkipDepth2_c, CSkipDepth3_c, CSkipDepth4_c, \
	CSkipDepth5_c, CSkipDepth6_c, CSkipDepth7_c, CSkipDepth8_c
#endif	/* gNwkMaximumDepth_c == 9 */

#if (gNwkMaximumDepth_c == 10)
#define CSkipDepth9_c	1
#define CSkipDepth8_c	(CSkipDepth9_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth7_c	(CSkipDepth8_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth6_c	(CSkipDepth7_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth5_c	(CSkipDepth6_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth4_c	(CSkipDepth5_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth3_c	(CSkipDepth4_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth2_c	(CSkipDepth3_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth1_c	(CSkipDepth2_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth0_c	(CSkipDepth1_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipArray_c	CSkipDepth0_c, CSkipDepth1_c, CSkipDepth2_c, CSkipDepth3_c, CSkipDepth4_c, \
	CSkipDepth5_c, CSkipDepth6_c, CSkipDepth7_c, CSkipDepth8_c, CSkipDepth9_c
#endif	/* gNwkMaximumDepth_c == 10 */

#if (gNwkMaximumDepth_c == 11)
#define CSkipDepth10_c	1
#define CSkipDepth9_c	(CSkipDepth10_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth8_c	(CSkipDepth9_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth7_c	(CSkipDepth8_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth6_c	(CSkipDepth7_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth5_c	(CSkipDepth6_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth4_c	(CSkipDepth5_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth3_c	(CSkipDepth4_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth2_c	(CSkipDepth3_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth1_c	(CSkipDepth2_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth0_c	(CSkipDepth1_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipArray_c	CSkipDepth0_c, CSkipDepth1_c, CSkipDepth2_c, CSkipDepth3_c, CSkipDepth4_c, \
	CSkipDepth5_c, CSkipDepth6_c, CSkipDepth7_c, CSkipDepth8_c, CSkipDepth9_c, \
	CSkipDepth10_c
#endif	/* gNwkMaximumDepth_c == 11 */

#if (gNwkMaximumDepth_c == 12)
#define CSkipDepth11_c	1
#define CSkipDepth10_c	(CSkipDepth11_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth9_c	(CSkipDepth10_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth8_c	(CSkipDepth9_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth7_c	(CSkipDepth8_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth6_c	(CSkipDepth7_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth5_c	(CSkipDepth6_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth4_c	(CSkipDepth5_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth3_c	(CSkipDepth4_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth2_c	(CSkipDepth3_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth1_c	(CSkipDepth2_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth0_c	(CSkipDepth1_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipArray_c	CSkipDepth0_c, CSkipDepth1_c, CSkipDepth2_c, CSkipDepth3_c, CSkipDepth4_c, \
	CSkipDepth5_c, CSkipDepth6_c, CSkipDepth7_c, CSkipDepth8_c, CSkipDepth9_c, \
	CSkipDepth10_c, CSkipDepth11_c
#endif	/* gNwkMaximumDepth_c == 12 */

#if (gNwkMaximumDepth_c == 13)
#define CSkipDepth12_c	1
#define CSkipDepth11_c	(CSkipDepth12_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth10_c	(CSkipDepth11_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth9_c	(CSkipDepth10_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth8_c	(CSkipDepth9_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth7_c	(CSkipDepth8_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth6_c	(CSkipDepth7_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth5_c	(CSkipDepth6_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth4_c	(CSkipDepth5_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth3_c	(CSkipDepth4_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth2_c	(CSkipDepth3_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth1_c	(CSkipDepth2_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth0_c	(CSkipDepth1_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipArray_c	CSkipDepth0_c, CSkipDepth1_c, CSkipDepth2_c, CSkipDepth3_c, CSkipDepth4_c, \
	CSkipDepth5_c, CSkipDepth6_c, CSkipDepth7_c, CSkipDepth8_c, CSkipDepth9_c, \
	CSkipDepth10_c, CSkipDepth11_c, CSkipDepth12_c
#endif	/* gNwkMaximumDepth_c == 13 */

#if (gNwkMaximumDepth_c == 14)
#define CSkipDepth13_c	1
#define CSkipDepth12_c	(CSkipDepth13_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth11_c	(CSkipDepth12_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth10_c	(CSkipDepth11_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth9_c	(CSkipDepth10_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth8_c	(CSkipDepth9_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth7_c	(CSkipDepth8_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth6_c	(CSkipDepth7_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth5_c	(CSkipDepth6_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth4_c	(CSkipDepth5_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth3_c	(CSkipDepth4_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth2_c	(CSkipDepth3_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth1_c	(CSkipDepth2_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth0_c	(CSkipDepth1_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipArray_c	CSkipDepth0_c, CSkipDepth1_c, CSkipDepth2_c, CSkipDepth3_c, CSkipDepth4_c, \
	CSkipDepth5_c, CSkipDepth6_c, CSkipDepth7_c, CSkipDepth8_c, CSkipDepth9_c, \
	CSkipDepth10_c, CSkipDepth11_c, CSkipDepth12_c, CSkipDepth13_c
#endif	/* gNwkMaximumDepth_c == 14 */

#if (gNwkMaximumDepth_c == 15)
#define CSkipDepth14_c	1
#define CSkipDepth13_c	(CSkipDepth14_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth12_c	(CSkipDepth13_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth11_c	(CSkipDepth12_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth10_c	(CSkipDepth11_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth9_c	(CSkipDepth10_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth8_c	(CSkipDepth9_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth7_c	(CSkipDepth8_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth6_c	(CSkipDepth7_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth5_c	(CSkipDepth6_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth4_c	(CSkipDepth5_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth3_c	(CSkipDepth4_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth2_c	(CSkipDepth3_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth1_c	(CSkipDepth2_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipDepth0_c	(CSkipDepth1_c * gNwkMaximumRouters_c + 1 + mMaxEndDeviceChildren_c)
#define CSkipArray_c	CSkipDepth0_c, CSkipDepth1_c, CSkipDepth2_c, CSkipDepth3_c, CSkipDepth4_c,\
	CSkipDepth5_c, CSkipDepth6_c, CSkipDepth7_c, CSkipDepth8_c, CSkipDepth9_c, \
	CSkipDepth10_c, CSkipDepth11_c, CSkipDepth12_c, CSkipDepth13_c, CSkipDepth14_c
#endif	/* gNwkMaximumDepth_c == 15 */

#if ( gDistributedAddressingCapability_d == 1 )
/* This is the actual CSkip Array */
const uint16_t CSkip[ gNwkMaximumDepth_c + 1 ] = { CSkipArray_c, 0 };
#endif
