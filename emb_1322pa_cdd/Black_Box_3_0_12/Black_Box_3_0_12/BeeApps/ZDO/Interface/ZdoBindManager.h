/*****************************************************************************
* This is a header file for Zdo Bind Manager
*
* Copyright (c) 2008, Freescale, Inc. All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
******************************************************************************/

#ifndef _ZdoBindManager_h_
#define _ZdoBindManager_h_

#include "AppZdoInterface.h"
#include "ZdoApsInterface.h"
#include "AfApsInterface.h"

/**********************************************************************
***********************************************************************
* Public Macros
***********************************************************************
***********************************************************************/


/**********************************************************************
***********************************************************************
* Public type definitions
***********************************************************************
***********************************************************************/


/**********************************************************************
***********************************************************************
* Public Memory Declarations
***********************************************************************
***********************************************************************/

/* used for communications with end-device-bind */
extern zbClusterId_t gaZdpMatchedOutCluster[];
extern index_t giMatchIndex;

extern bool_t gZdoInEndDeviceBind;

/**********************************************************************
***********************************************************************
* Public prototypes
***********************************************************************
**********************************************************************/

/* resets the end-device bind ne */
void ZdpEndDeviceBindReset(void);

/* moves on to the next state for the coordinator binding engine */
void ZdpEndDeviceBindStateMachine(void);

/* received an EDB.request. Either start up the engine or continue it */
zbSize_t ZdpEndDeviceBindGotRequest(zbEndDeviceBindRequest_t *pReqFrame);

/* a bind or unbind sent out by the EndDeviceBind engine on the ZC has returned */
void ZdpEndDeviceBindGotResponse(zbNwkAddr_t aSrcAddr, zbStatus_t status);



/**********************************************************************
***********************************************************************
* Interface Macro Declarations
***********************************************************************
***********************************************************************/

/*None*/

#endif /* _ZdoBindManager_h_ */

