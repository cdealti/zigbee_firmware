/******************************************************************************
* This file contains the interface between the application and the ZigBee
* Cluster Library (ZCL).
*
* (c) Copyright 2007, Freescale, Inc. All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
******************************************************************************/
#ifndef _ASL_ZCL_INTERFACE_H_
#define _ASL_ZCL_INTERFACE_H_

#include "embeddedtypes.h"
#include "zigbee.h"
#include "Keyboard.h"
#include "Display.h"
#include "Led.h"
#include "zcl.h"
#include "zclGeneral.h"
#include "zclSensing.h"
#include "ZclClosures.h"
#include "zclHVAC.h"
#include "ASL_UserInterface.h"

/******************************************************************************
*******************************************************************************
* Public macros
*******************************************************************************
******************************************************************************/
#define gASL_Zcl_InitState_c     0
#define gASL_Zcl_IdentifyState_c 1
#define gASL_Zcl_RequestState_c  2

/******************************************************************************
*******************************************************************************
* Public prototypes
*******************************************************************************
******************************************************************************/
void ASL_ZclInit(void);

#if gASL_ZclStoreSceneHandler_d && gASL_ZclStoreSceneReq_d
void ASL_ZclStoreSceneHandler(void);
#else
#define ASL_ZclStoreSceneHandler()
#endif

#if gASL_ZclAddGroupHandler_d && gASL_ZclGroupAddGroupReq_d
void ASL_ZclAddGroupHandler(void);
#else
#define ASL_ZclAddGroupHandler()
#endif


/******************************************************************************
*******************************************************************************
* Public type definitions
*******************************************************************************
******************************************************************************/

/*
  Function to receive the response from a ZCL call. Comes in originally on the 
  APSDE-DATA.indication, into the application's BeeAppDataIndication() function.
*/
typedef void (*fnZclResponseHandler_t)(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice);

/******************************************************************************
*******************************************************************************
* Public memory declarations
*******************************************************************************
********************************************************************************/

/******************************************************************************
*******************************************************************************
* Public functions
*******************************************************************************
******************************************************************************/\
#if gASL_ZclIdentifyReq_d
/*-------------------- ASL_ZclIdentifyReq --------------------
	3.5.2.3.1 Identify Command (ClusterID=0x0003, CmmId=0x00).
	The identify command starts or stops the receiving device identifying itself.
*/
zclStatus_t ASL_ZclIdentifyReq(zclIdentifyReq_t *pReq);
#else
#define ASL_ZclIdentifyReq(pReq) FALSE
#endif


#if gASL_ZclIdentifyQueryReq_d
/*-------------------- ASL_ZclIdentifyQueryReq --------------------
	3.5.2.3.2 Identify Query Command (ClusterID=0x0003, CmmId=0x01).

	The identify query command allows the sending device to
	request the target or targets to respond if they are
	currently identifying themselves. 
	
	Note that this implements	the identify query request using a 
	broadcast	destination and destination endpoint.
*/
zclStatus_t ASL_ZclIdentifyQueryReq(zclIdentifyQueryReq_t *pReq);
zclStatus_t ASL_ZclIdentifyQueryReqUI(zbEndPoint_t srcEndPoint);
#else
#define ASL_ZclIdentifyQueryReq(pReq) FALSE
#define ASL_ZclIdentifyQueryReqUI(srcEndPoint) FALSE
#endif



#if gASL_ZclGroupAddGroupReq_d
/*-------------------- ASL_ZclGroupAddGroupReq -------------------
	3.6.2.2.3  Add Group Command (ClusterId=0x0004, CmmdId=0x00).
	3.6.2.2.8  Add Group If Identifying Command (ClusterId=0x0004, CmmdId=0x05).

	The add group commands allows the sending device to add
	membership in a particular group for one or more endpoints
	on the receiving device. The add group if identifying adds 
	the group membership only if destination device is identifying
	when it receives the request.
*/
zclStatus_t ASL_ZclGroupAddGroupReq(zclGroupAddGroupReq_t *pReq);
zclStatus_t ASL_ZclGroupAddGroupIfIdentifyReq(zclGroupAddGroupIfIdentifyingReq_t*pReq);
zbStatus_t ASL_ZclAddGroupIfIdentifyUI(uint8_t mode, zbGroupId_t aGroupId);
#else
#define ASL_ZclGroupAddGroupReq(pReq)  FALSE
#define ASL_ZclGroupAddGroupIfIdentifyReq(pReq) FALSE
#define ASL_ZclAddGroupIfIdentifyUI(mode, aGroupId)
#endif


#if gASL_ZclGroupViewGroupReq_d
/*-------------------- ASL_ZclGroupViewGroupReq --------------------
	3.6.2.2.4 View Group Command (ClusterID=0x0004, CommandID=0x01).

	The view group command allows the sending device to request that the
	receiving entity or entities respond with a view group response
	command containing the application name string for a particular

	Client for View Group, sends the packet OTA.
*/
zclStatus_t ASL_ZclGroupViewGroupReq
(
	zclGroupViewGroupReq_t *pReq
);
#else
#define ASL_ZclGroupViewGroupReq(pReq)  FALSE
#endif

#if gASL_ZclGetGroupMembershipReq_d
/*-------------------- ASL_ZclGetGroupMembershipReq -------------------
	3.6.2.2.5 Get group membership command (CLusterID=0x0004 CommandID=0x02).

	The get group membership command allows the sending device to
	inquire about the group membership of the receiving device and
	endpoint in a number of ways.

	Client for Get Group Membership, sends the package OTA.
*/
zclStatus_t ASL_ZclGetGroupMembershipReq
(
	zclGroupGetGroupMembershipReq_t *pReq
);
#else
#define ASL_ZclGetGroupMembershipReq(pReq)  FALSE
#endif

#if gASL_ZclRemoveGroupReq_d
/*-------------------- ASL_ZclRemoveGroupReq --------------------
	3.6.2.2.6 Remove Group command (ClusterID=0x0004 CommandID=0x03).

	The remove group command allows the sender to request that the
	receiving entity or entities remove their membership, if any, in
	a particular group.

	Note that if a group is removed the scenes associated with that
	group should be removed.
*/
zclStatus_t ASL_ZclRemoveGroupReq
(
	zclGroupRemoveGroupReq_t *pReq
);
#else
#define ASL_ZclRemoveGroupReq(pReq)  FALSE
#endif


#if gASL_ZclRemoveAllGroupsReq_d
/*-------------------- ASL_ZclRemoveAllGroupsReq --------------------
	3.6.2.2.7 Remove All Groups command (ClusterID=0x0004 Command=0x04)

	The remove all groups command allows the sending device to direct
	the receiving entity or entities to remove all group associations.

	Note that removing all groups necessitates the removal of all
	associated scenes as well. (Note: scenes not associated with a
	group need not be removed).
*/
zclStatus_t ASL_ZclRemoveAllGroupsReq
(
	zclGroupRemoveAllGroupsReq_t *pReq
);
#else
#define ASL_ZclRemoveAllGroupsReq(pReq)  FALSE
#endif

/*-------------------- ASL_ZclSceneAddSceneReq --------------------
	3.7.2.4.1 Add Scene command (ClusterID=0x0005 CommandID=0x00).

	The add scene command allows the sender to request that the
	receiving entity to add a specified scene to their scene table.
*/
#if gASL_ZclSceneAddSceneReq_d
zclStatus_t ASL_ZclSceneAddSceneReq
(
	zclSceneAddSceneReq_t *pReq,
	uint8_t len
);
#else
#define ASL_ZclSceneAddSceneReq(pReq,len)  FALSE
#endif

/*-------------------- ASL_ZclViewSceneReq --------------------
	3.7.2.4.2 View Scene command (ClusterID=0x0005 CommandID=0x01).

	The view scene command allows the sender to request that the
	receiving entity to generate a View Scene response for a specified scene.
*/
#if gASL_ZclViewSceneReq_d
zclStatus_t ASL_ZclViewSceneReq
(
	zclSceneViewSceneReq_t *pReq
);
#else
#define ASL_ZclViewSceneReq(pReq)  FALSE
#endif

/*-------------------- ASL_ZclRemoveSceneReq --------------------
	3.7.2.4.3 Remove Scene command (ClusterID=0x0005 CommandID=0x02).

	The remove scene command allows the sender to request that the
	receiving entity to remove a specified scene from the scene table.
*/
#if gASL_ZclRemoveSceneReq_d
zclStatus_t ASL_ZclRemoveSceneReq
(
	zclSceneRemoveSceneReq_t *pReq
);
#else
#define ASL_ZclRemoveSceneReq(pReq)  FALSE
#endif

/*-------------------- ASL_ZclRemoveAllScenesReq --------------------
	3.7.2.4.4 Remove All Scenes command (ClusterID=0x0005 CommandID=0x03).

	The remove all scenes command allows the sender to request that the
	receiving entity or entities to remove all scenes from the scene table.
*/
#if gASL_ZclRemoveAllScenesReq_d
zclStatus_t ASL_ZclRemoveAllScenesReq
(
	zclSceneRemoveAllScenesReq_t *pReq
);
#else
#define ASL_ZclRemoveAllScenesReq(pReq)  FALSE
#endif

#if gASL_ZclStoreSceneReq_d
/*-------------------- ASL_ZclStoreSceneReq --------------------
	3.7.2.4.5 Store Scene command (ClusterID=0x0005 CommandID=0x04).

	On receipt of this command, the device shall (if possible) add
	an entry to the Scene Table with the Scene ID and Group ID given
	in the command, and all extension fields corresponding to the current
	state of other clusters on the device. If an entry already exists
	with the same Scene ID and Group ID it will be replaced.

	If the command was addressed to a single device (not to a group) then
	it shall generate an appropriate Store Scene Response command indicating
	success or failure. See 3.7.2.5.5.
*/
zbStatus_t ASL_ZclStoreSceneReq
(
	zclSceneStoreSceneReq_t *pReq
);
zbStatus_t ASL_ZclStoreSceneUI
(
  uint8_t mode,
  zbGroupId_t aGroupId,
  zclSceneId_t sceneId
);
#else
#define ASL_ZclStoreSceneReq(pReq)  FALSE
#define ASL_ZclStoreSceneUI(mode, aGroupId, sceneId)
#endif

#if gASL_ZclRecallSceneReq_d
/*-------------------- ASL_ZclRecallSceneReq --------------------
	3.7.2.4.6 Recall Scene command (ClusterID=0x0005 Command=0x05)

	On receipt of this command, the device shall (if possible) locate
	the entry in its Scene Table with the Group ID and Scene ID given
	in the command. For each other cluster on the device, it shall then
	retrieve any corresponding extension fields from the Scene Table
	and set the attributes and corresponding state of the cluster accordingly.
*/
zbStatus_t ASL_ZclRecallSceneReq
(
	zclSceneRecallSceneReq_t *pReq
);
void ASL_ZclRecallSceneUI 
(
  zbGroupId_t aGroupId,
  zclSceneId_t sceneId
);
#else
#define ASL_ZclRecallSceneReq(pReq)  FALSE
#define ASL_ZclRecallSceneUI(aGroupId, sceneId)
#endif

/*-------------------- ASL_ZclGetSceneMembershipReq --------------------
	3.7.2.4.7 Get Scene Membership command (ClusterID=0x0005 CommandID=0x06).

	The get scene membership command allows the sender to request that the
	receiving entity to answer with the scene IDs associated to a specified 
	group ID in its scene table.
*/
#if gASL_ZclGetSceneMembershipReq_d
zclStatus_t ASL_ZclGetSceneMembershipReq(zclSceneGetSceneMembershipReq_t *pReq);
#else
#define ASL_ZclGetSceneMembershipReq(pReq)  FALSE
#endif

#if gASL_ZclOnOffReq_d
/*-------------------- ASL_ZclOnOffReq --------------------
	3.8.2.3.1 Off Command (ClusterID=0x0006 CommandID=0x00).
	3.8.2.3.2 On Command (ClusterID=0x0006 CommandID=0x01).
	3.8.2.3.3 Toggle Command (ClusterID=0x0006 CommandID=0x02).
*/

zbStatus_t ASL_ZclOnOffReq
(
	zclOnOffReq_t *pReq
);
#else
#define ASL_ZclOnOffReq(pReq)  FALSE
#endif

#if gASL_ZclDoorLockReq_d
/*-------------------- ASL_ZclDoorLockReq --------------------
	7.3.2.3.1 Look Door Command (ClusterID=0x0101 CommandID=0x00).
	7.3.2.3.2 Unlook Door Command (ClusterID=0x0101 CommandID=0x01).
*/

zbStatus_t ASL_ZclDoorLockReq
(
	zclDoorLockReq_t *pReq
);
#else
#define ASL_ZclDoorLockReq(pReq)  FALSE
#endif

/*-------------------- ASL_ZclLevelControlReq --------------------
  Level control requests:
  3.10.2.3.1 Move To Level (ClusterID=0x0008 CommandID=0x00).
  3.10.2.3.2 Move (ClusterID=0x0008 CommandID=0x01).
  3.10.2.3.3 Step (ClusterID=0x0008 CommandID=0x02).
  3.10.2.3.4 Stop (ClusterID=0x0008 CommandID=0x03).  
  3.10.2.3.5 Move To Level with On/Off (ClusterID=0x0008 CommandID=0x04).
  3.10.2.3.6 Move with On/Off (ClusterID=0x0008 CommandID=0x05).
  3.10.2.3.7 Step with On/Off (ClusterID=0x0008 CommandID=0x06).
  3.10.2.3.8 Stop with On/Off (ClusterID=0x0008 CommandID=0x07).   
*/
#if gASL_ZclLevelControlReq_d
zbStatus_t ASL_ZclLevelControlReq
(
	zclLevelControlReq_t *pReq, 
	uint8_t command
);
#else
#define ASL_ZclLevelControlReq(pReq, command)  FALSE
#endif


/*****************************************************************************
* ZCL_Register
*
* Register a callback for sending responses from ZCL.
*****************************************************************************/
void ZCL_Register
  (
  fnZclResponseHandler_t fnResponseHandler
  );


#endif  /*  _ASL_ZCL_INTERFACE_H_  */
