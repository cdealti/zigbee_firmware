/******************************************************************************
* ZclFoundation.h
*
* This module contains code that handles ZigBee Cluster Library commands for the
* closures
*
* Copyright (c) 2007, Freescale, Inc.  All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
* Documents used in this specification:
* [R1] - 053520r16ZB_HA_PTG-Home-Automation-Profile.pdf
* [R2] - 075123r00ZB_AFG-ZigBee_Cluster_Library_Specification.pdf
*******************************************************************************/
#include "EmbeddedTypes.h"
#include "zigbee.h"
#include "FunctionLib.h"
#include "BeeStackConfiguration.h"
#include "BeeStack_Globals.h"
#include "AppAfInterface.h"
#include "AfApsInterface.h"
#include "BeeStackInterface.h"

#include "HaProfile.h"

#ifdef gZclEnablePartition_d    
#if (gZclEnablePartition_d == TRUE)

uint8_t ZCL_BuildReadAttrResponseWithPartition
  (
  afDeviceDef_t  *pDevice,    /* IN: device definition for this endpoint */
  afClusterDef_t *pCluster,   /* IN: cluster that was found */
  uint8_t count,              /* IN: # of responses */
  zclFrame_t *pFrame,         /* IN: read attr request */
  zclFrame_t *pFrameRsp,       /* IN/OUT: response frame, allocated above, filled in here... */
  bool_t partitionReadAttr
  );
  
uint8_t ZCL_BuildWriteAttrResponseWithPartition
  (
  afDeviceDef_t  *pDevice,    /* IN: data to be acted on by this cluster */
  afClusterDef_t *pCluster,   /* IN: cluster definition to write attrs */
  uint8_t asduLen,            /* IN: length of input frame */
  zclFrame_t *pFrame,         /* IN: input frame */
  zclFrame_t *pFrameRsp,      /* IN/OUT: response to write attribute */
  bool_t partitionedWriteAttr,/* IN: write to a partition table instead of main atttr table */
  bool_t forceWritingROAttr   /* IN: write data even if the attribute is readonly */
  );
#endif  
#endif
/******************************************************************************
*******************************************************************************
* Private macros
*******************************************************************************
******************************************************************************/


