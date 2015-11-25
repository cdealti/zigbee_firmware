/*****************************************************************************
* ZigBee Application.
*
* Copyright (c) 2007, Freescale, Inc. All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
*****************************************************************************/
#include "EmbeddedTypes.h"
#include "ZigBee.h"
#include "BeeAppInit.h"
#include "AppAfInterface.h"
#include "EndPointConfig.h"
#include "AppZdoInterface.h"
#include "ZdpManager.h"
#include "ZDOStateMachineHandler.h"
#include "BeeStackInterface.h"
#include "ASL_ZdpInterface.h"
#include "ASL_ZCLInterface.h"
#include "ASL_UserInterface.h"
#include "TMR_Interface.h"
#include "Timer.h"
#include "ZclGeneral.h"
#include "ZclClosures.h"
/******************************************************************************
*******************************************************************************
* Private macros
*******************************************************************************
******************************************************************************/

#define gLevelControlInitPayloadSize_c 0xFF

/******************************************************************************
*******************************************************************************
* Public prototypes
*******************************************************************************
******************************************************************************/

/******************************************************************************
*******************************************************************************
* Private prototypes
*******************************************************************************
******************************************************************************/

void ASL_ZclStoreSceneTimerCallBack( uint8_t timerId );
void ASL_ZclAddGroupTimerCallBack( uint8_t timerId );
void ASL_ZclFillAddrInfo( afAddrInfo_t *addrInfo, zclClusterId_t clusterId,  zbEndPoint_t srcEndPoint );
zbStatus_t APSME_AddGroupRequest(zbApsmeAddGroupReq_t *pRequest);

/******************************************************************************
*******************************************************************************
* Private type definitions
*******************************************************************************
******************************************************************************/
/* None */
/******************************************************************************
*******************************************************************************
* Private memory declarations
*******************************************************************************
******************************************************************************/
uint8_t       gASL_ZclState;   /* state machine for scene binding */
uint8_t       gASL_ZclTimerID;
zbGroupId_t   gASL_ZclGroupId;
zclSceneId_t  gASL_ZclSceneId;

/******************************************************************************
*******************************************************************************
* Public memory declarations
*******************************************************************************
******************************************************************************/
extern zbEndPoint_t appEndPoint;

/******************************************************************************
*******************************************************************************
* Public functions
*******************************************************************************
******************************************************************************/
void ASL_ZclInit(void){  
   /* allocate a timer for the application */
  gASL_ZclTimerID = TMR_AllocateTimer(); 
  ZCL_Init();
}


#if gASL_ZclIdentifyReq_d
/*-------------------- ASL_ZclIdentifyReq --------------------
	3.5.2.3.1 Identify Command (ClusterID=0x0003, CmmId=0x00).
	The identify command starts or stops the receiving device identifying itself.
*/
zclStatus_t ASL_ZclIdentifyReq
(
	zclIdentifyReq_t *pReq
)
{
	/* send the request over the air */
  return ZCL_GenericReq(gZclCmdIdentify_c, sizeof(zclCmdIdentify_t), (zclGenericReq_t *)pReq);  
}

#endif


#if gASL_ZclIdentifyQueryReq_d
/*-------------------- ASL_ZclIdentifyQueryReq --------------------
	3.5.2.3.2 Identify Query Command (ClusterID=0x0003, CmmId=0x01).

	The identify query command allows the sending device to
	request the target or targets to respond if they are
	currently identifying themselves. 
*/


zclStatus_t ASL_ZclIdentifyQueryReq
(
	zclIdentifyQueryReq_t *pReq
)
{
  return ZCL_GenericReq(gZclCmdIdentifyQuery_c, 0, (zclGenericReq_t *)pReq);  
}

zclStatus_t ASL_ZclIdentifyQueryReqUI
(
	zbEndPoint_t srcEndPoint
)
{
  zclIdentifyQueryReq_t req;

	/* set up address info for a broadcast */
	req.addrInfo.dstAddrMode = gZbAddrMode16Bit_c;
	Copy2Bytes(req.addrInfo.dstAddr.aNwkAddr, gaBroadcastAddress);
	ASL_ZclFillAddrInfo(&req.addrInfo, gZclClusterIdentify_c, srcEndPoint);

  return ASL_ZclIdentifyQueryReq(&req);
}
#endif




/*****************************************************************************
* ASL_GenericGroupReq
*
* Helper function to create and send some group commands frames over the air.
*****************************************************************************/
#if gASL_ZclGroupViewGroupReq_d ||  gASL_ZclRemoveGroupReq_d || gASL_ZclRemoveAllGroupsReq_d
zclStatus_t ASL_GenericGroupReq 
(
  zclCmd_t command, uint8_t iPayloadLen, void* pCmdFrame, void* pAddrInfo
) 
{
	afToApsdeMessage_t *pMsg;

	pMsg = ZCL_CreateFrame(pAddrInfo, command, gZclFrameControl_FrameTypeSpecific, NULL, &iPayloadLen, pCmdFrame);
	if(!pMsg)
		return gZclNoMem_c;

	return ZCL_DataRequestNoCopy(pAddrInfo, iPayloadLen, pMsg);  
}
#endif



#if gASL_ZclGroupAddGroupReq_d 
/*-------------------- ASL_ZclGroupAddGroupReq -------------------
	3.6.2.2.3  Add Group Command (ClusterId=0x0004, CmmdId=0x00).

	The add group command allows the sending device to add
	membership in a particular group for one or more endpoints
	on the receiving device. 
*/
zclStatus_t ASL_ZclGroupAddGroupReq
(
	zclGroupAddGroupReq_t *pReq
)
{
  return ZCL_GenericReq(gZclCmdGroup_AddGroup_c, sizeof(zclCmdGroup_AddGroup_t)+(pReq->cmdFrame.szGroupName[0]), (zclGenericReq_t *)pReq);
}
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
)
{
  return ASL_GenericGroupReq(gZclCmdGroup_ViewGroup_c, sizeof(zclCmdGroup_ViewGroup_t), pReq->cmdFrame.aGroupId, &(pReq->addrInfo));
}
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
	zclGroupGetGroupMembershipReq_t*pReq
)
{
	zbSize_t iPayloadLen = MbrOfs(zclCmdGroup_GetGroupMembership_t, aGroupId[0]) + 
								         (pReq->cmdFrame.count * MbrSizeof(zclCmdGroup_GetGroupMembership_t, aGroupId));
  return ZCL_GenericReq(gZclCmdGroup_GetGroupMembership_c, iPayloadLen, (zclGenericReq_t *)pReq);  
}
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
)
{
  return ASL_GenericGroupReq(gZclCmdGroup_RemoveGroup_c, sizeof(zclCmdGroup_RemoveGroup_t), (pReq->cmdFrame.aGroupId), &(pReq->addrInfo));
}
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
)
{
  return ASL_GenericGroupReq(gZclCmdGroup_RemoveAllGroups_c, 0, NULL, &(pReq->addrInfo));
}
#endif

#if gASL_ZclGroupAddGroupReq_d
/*-------------------- ASL_ZclGroupAddGroupReq -------------------
		3.6.2.2.8  Add Group If Identifying Command (ClusterId=0x0004, CmmdId=0x05).

	The add group commands allows the sending device to add
	membership in a particular group for one or more endpoints
	on the receiving device. The add group if identifying adds 
	the group membership only if destination device is identifying
	when it receives the request.
*/
zclStatus_t ASL_ZclGroupAddGroupIfIdentifyReq
(
	zclGroupAddGroupIfIdentifyingReq_t*pReq
)
{
	zbSize_t iPayloadLen = sizeof(zclCmdGroup_AddGroupIfIdentifying_t)+(pReq->cmdFrame.szGroupName[0]);
  return ZCL_GenericReq(gZclCmdGroup_AddGroupIfIdentifying_c, iPayloadLen, (zclGenericReq_t *)pReq);    
}
#endif

/* Scenes cluster commands */

#if gASL_ZclSceneAddSceneReq_d
/*-------------------- ASL_ZclSceneAddSceneReq --------------------
	3.7.2.4.1 Add Scene command (ClusterID=0x0005 CommandID=0x00).

	The add scene command allows the sender to request that the
	receiving entity to add a specified scene to their scene table.

  Returns gZbSuccess_t if worked.
*/
zbStatus_t ASL_ZclSceneAddSceneReq
  (
	zclSceneAddSceneReq_t *pReq,   /* IN */
	uint8_t len                    /* IN: length of command frame (already subtracted off addr info) */
  )
{
  /* generate the request (variable length) */
	return ZCL_GenericReq(gZclCmdScene_AddScene_c,
		len,(zclGenericReq_t *)pReq);
}
#endif

#if gASL_ZclViewSceneReq_d
/*-------------------- ASL_ZclViewSceneReq --------------------
	3.7.2.4.2 View Scene command (ClusterID=0x0005 CommandID=0x01).

	The view scene command allows the sender to request that the
	receiving entity to generate a View Scene response for a specified scene.
	
  Returns gZbSuccess_t if worked.
 */
zbStatus_t ASL_ZclViewSceneReq
(
	zclSceneViewSceneReq_t *pReq
)
{
	return ZCL_GenericReq(gZclCmdScene_ViewScene_c,
		sizeof(zclCmdScene_ViewScene_t),
	(zclGenericReq_t *)pReq);
}
#endif


#if gASL_ZclRemoveSceneReq_d
/*-------------------- ASL_ZclRemoveSceneReq --------------------
	3.7.2.4.3 Remove Scene command (ClusterID=0x0005 CommandID=0x02).

	The remove scene command allows the sender to request that the
	receiving entity to remove a specified scene from the scene table.
 
  Returns gZbSuccess_t if worked.
*/
zbStatus_t ASL_ZclRemoveSceneReq
  (
  zclSceneRemoveSceneReq_t *pReq
  )
{
  return ZCL_GenericReq(gZclCmdScene_RemoveScene_c,
    sizeof(zclCmdScene_RemoveScene_t),
    (zclGenericReq_t *)pReq);
}
#endif

#if gASL_ZclRemoveAllScenesReq_d
/*-------------------- ASL_ZclRemoveAllScenesReq --------------------
	3.7.2.4.4 Remove All Scenes command (ClusterID=0x0005 CommandID=0x03).

	The remove all scenes command allows the sender to request that the
	receiving entity or entities to remove all scenes from the scene table.

 Returns gZbSuccess_t if worked.
*/
zbStatus_t ASL_ZclRemoveAllScenesReq
  (
  zclSceneRemoveAllScenesReq_t *pReq
  )
{
  return ZCL_GenericReq(gZclCmdScene_RemoveAllScenes_c,
    sizeof(zclCmdScene_RemoveAllScenes_t),
    (zclGenericReq_t *)pReq);
}
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
)
{
	return ZCL_GenericReq(gZclCmdScene_StoreScene_c,
						sizeof(zclCmdScene_StoreScene_t),
						(zclGenericReq_t *)pReq);
}
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
)
{
	return ZCL_GenericReq(gZclCmdScene_RecallScene_c,
												sizeof(zclCmdScene_StoreScene_t),
												(zclGenericReq_t *)pReq);
}
#endif

#if gASL_ZclGetSceneMembershipReq_d
/*-------------------- ASL_ZclGetSceneMembershipReq --------------------
	3.7.2.4.7 Get Scene Membership command (ClusterID=0x0005 CommandID=0x06).

	The get scene membership command allows the sender to request that the
	receiving entity to answer with the scene IDs associated to a specified 
	group ID in its scene table.

  Returns gZbSuccess_t if worked.
*/
zbStatus_t ASL_ZclGetSceneMembershipReq
  (
  zclSceneGetSceneMembershipReq_t *pReq
  )
{
  return ZCL_GenericReq(gZclCmdScene_GetSceneMembership_c,
    sizeof(zclCmdScene_GetSceneMembership_t),
    (zclGenericReq_t *)pReq);
}
#endif

#if gASL_ZclAddGroupHandler_d && gASL_ZclGroupAddGroupReq_d
/*****************************************************************************
* ASL_ZclAddGroupHandler
*
* Adds a group.
*****************************************************************************/
void ASL_ZclAddGroupHandler(void)
{

  /* done with state machine? */
  if(gASL_ZclState >= 1)
  {
    TMR_StopTimer(gASL_ZclTimerID);
    ASL_SetLed(LED3, gLedOn_c);
  }

  /* on to the next state */
  ++gASL_ZclState;
  if(ASL_ZclAddGroupIfIdentifyUI(gASL_ZclState, gASL_ZclGroupId) != gZbSuccess_c) {
    ASL_SetLed(LED3, gLedOff_c);
  }
}
#endif

#if gASL_ZclStoreSceneHandler_d && gASL_ZclStoreSceneReq_d
/*****************************************************************************
* ASL_ZclStoreSceneHandler
*
* Store a scene.
*****************************************************************************/
void ASL_ZclStoreSceneHandler(void)
{
  /* done with state machine? */
  if(gASL_ZclState >= 2)
  {
    TMR_StopTimer(gASL_ZclTimerID);
    ASL_SetLed(LED4, gLedOn_c);
  }

  /* on to the next state */
  ++gASL_ZclState;
  if(ASL_ZclStoreSceneUI(gASL_ZclState, gASL_ZclGroupId, gASL_ZclSceneId) != gZbSuccess_c) {
    ASL_SetLed(LED4, gLedOff_c);
  }
}
#endif


/*****************************************************************************
* ASL_ZclStoreSceneTimerCallBack
*
* Store a scene.
*****************************************************************************/
void ASL_ZclStoreSceneTimerCallBack( uint8_t timerId )
{
  /* avoid compiler warning */
  (void)timerId;

  TS_SendEvent(gAppTaskID, gAppEvtStoreScene_c);
}

/*****************************************************************************
* ASL_ZclAddGroupCallBack
*
* Store a scene.
*****************************************************************************/
void ASL_ZclAddGroupTimerCallBack( uint8_t timerId )
{
  /* avoid compiler warning */
  (void)timerId;

  TS_SendEvent(gAppTaskID, gAppEvtAddGroup_c);
}

void ASL_ZclInitStateUI
(
  zbGroupId_t aGroupId,
  zclSceneId_t sceneId,
  void (*pfTimerCallBack)(tmrTimerID_t)
)
{
	gASL_ZclState = gASL_Zcl_InitState_c;   /* timer will update the state */
	Copy2Bytes(gASL_ZclGroupId, aGroupId);	
	gASL_ZclSceneId = sceneId;
	TMR_StartIntervalTimer(gASL_ZclTimerID, 100, pfTimerCallBack);
}

/*****************************************************************************
* ASL_ZclAddGroupIfIdentifyUI
*
* Adds remote nodes to the group if they are in identify mode.
* Function is used to add group adding functionality to board UI (button press).
*****************************************************************************/
#if gASL_ZclGroupAddGroupReq_d
zbStatus_t ASL_ZclAddGroupIfIdentifyUI
(
  uint8_t mode,
  zbGroupId_t aGroupId   /* IN: */
  
)
{
	zbStatus_t status = gZbSuccess_c;
	zclGroupAddGroupIfIdentifyingReq_t groupReq;

	/* start up the state machine */
	if(mode == gASL_Zcl_InitState_c) {
    ASL_ZclInitStateUI(aGroupId, 0, ASL_ZclAddGroupTimerCallBack); 
		return gZbSuccess_c;
	}

	/* add the group for those in identify mode, in this case this will be done if the, identify query returns success*/
	if(mode == gASL_Zcl_IdentifyState_c) {
		/* set up address info for a broadcast */
		groupReq.addrInfo.dstAddrMode = gZbAddrMode16Bit_c;
		Copy2Bytes(groupReq.addrInfo.dstAddr.aNwkAddr, gaBroadcastAddress);
		ASL_ZclFillAddrInfo(&groupReq.addrInfo, gZclClusterGroups_c, appEndPoint);

		/* set up add group information, group number and name to be added to the group table*/
		Copy2Bytes(groupReq.cmdFrame.aGroupId, gASL_ZclGroupId);
		groupReq.cmdFrame.szGroupName[0] = 0x00; /* no OTA name */
		/* Send the Add group req */
		status = ASL_ZclGroupAddGroupIfIdentifyReq(&groupReq);
	}
	
  return status;
}
#endif


/*****************************************************************************
* ASL_ZclStoreSceneUI
*
* Stores a scene on remote nodes that are in identify mode. Also adds the
* remote nodes to the group.
* Function is used to add store scene functionality to board UI (button press).
*****************************************************************************/
#if gASL_ZclStoreSceneReq_d
zbStatus_t ASL_ZclStoreSceneUI
  (
  uint8_t mode,
  zbGroupId_t aGroupId,   /* IN: */
  zclSceneId_t sceneId    /* IN: */
  )
{
  zbStatus_t status = gZbSuccess_c;
  zclSceneStoreSceneReq_t sceneReq;
  zclGroupAddGroupReq_t   groupReq;

  /* start up the state machine */
  if(mode == gASL_Zcl_InitState_c) {
    ASL_ZclInitStateUI(aGroupId, sceneId, ASL_ZclStoreSceneTimerCallBack); 
    return gZbSuccess_c;
  }

  /* set up address info for a broadcast */
  groupReq.addrInfo.dstAddrMode = gZbAddrModeGroup_c;
  Copy2Bytes(groupReq.addrInfo.dstAddr.aGroupId, aGroupId);
  ASL_ZclFillAddrInfo(&groupReq.addrInfo, gZclClusterGroups_c, appEndPoint);  

  /* call query identify (results come back to the app endpoint) */
  if(mode == gASL_Zcl_IdentifyState_c)
      return gZbSuccess_c;

  /* set up request */
  FLib_MemCpy(&sceneReq.addrInfo, &groupReq.addrInfo, sizeof(groupReq.addrInfo));  
  Set2Bytes(sceneReq.addrInfo.aClusterId, gZclClusterScenes_c);
  Copy2Bytes(sceneReq.cmdFrame.aGroupId, aGroupId);
  sceneReq.cmdFrame.sceneId = sceneId;

  /* call query identify (results come back to the app endpoint) */
  if(mode == gASL_Zcl_RequestState_c) {
    status = ASL_ZclStoreSceneReq(&sceneReq);
  }

  return status;
}
#endif


#if gASL_ZclRecallSceneReq_d

/*****************************************************************************
* ASL_ZclRecallSceneUI
*
* Recalls a previously stored scene on remote nodes.
* Function is used to add recall scene functionality to board UI (button press).
*****************************************************************************/
void ASL_ZclRecallSceneUI
  (
  zbGroupId_t aGroupId,   /* IN: */
  zclSceneId_t sceneId    /* IN: */
  )
{
  zclSceneRecallSceneReq_t req;

  /* set up address info for a broadcast */
  
  req.addrInfo.dstAddrMode = gZbAddrModeGroup_c;
  Copy2Bytes(req.addrInfo.dstAddr.aGroupId, aGroupId);
  ASL_ZclFillAddrInfo(&req.addrInfo, gZclClusterScenes_c, appEndPoint);

  Copy2Bytes(req.cmdFrame.aGroupId, aGroupId);
  req.cmdFrame.sceneId = sceneId;

  (void)ASL_ZclRecallSceneReq(&req);
}

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
)
{
	Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterOnOff_c);
	return ZCL_GenericReqNoData(&(pReq->addrInfo), pReq->command);
}
#endif

#if gASL_ZclDoorLockReq_d
/*-------------------- ASL_ZclDoorLockReq --------------------
	7.3.2.3.1 Lock Door Command (ClusterID=0x0101 CommandID=0x00).
	7.3.2.3.2 Unlock Door Command (ClusterID=0x0101 CommandID=0x01).
*/
zbStatus_t ASL_ZclDoorLockReq
(
	zclDoorLockReq_t *pReq
)
{
	Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterDoorLock_c);
	return ZCL_GenericReqNoData(&(pReq->addrInfo), pReq->command);
}
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
	zclLevelControlReq_t * pReq,
	uint8_t command
)
{
  zbSize_t iPayloadLen = gLevelControlInitPayloadSize_c;
  afToApsdeMessage_t *pMsg=NULL;

  switch (command) {
     case gZclCmdLevelControl_MoveToLevel_c:
     case gZclCmdLevelControl_MoveToLevelOnOff_c:
         iPayloadLen = sizeof(zclCmdLevelControl_MoveToLevel_t);
         break;
     case gZclCmdLevelControl_Move_c:
     case gZclCmdLevelControl_MoveOnOff_c:
         iPayloadLen = sizeof(zclCmdLevelControl_Move_t);
         break;
     case gZclCmdLevelControl_Step_c:
     case gZclCmdLevelControl_StepOnOff_c:     
         iPayloadLen = sizeof(zclCmdLevelControl_Step_t);
         break;
     case gZclCmdLevelControl_Stop_c:
     case gZclCmdLevelControl_StopOnOff_c:
         iPayloadLen = gZero_c;
         break;
     default:
         break;     
  }                   
  
  if (iPayloadLen != gLevelControlInitPayloadSize_c) {
    pMsg = ZCL_CreateFrame(&(pReq->stopReq.addrInfo), command,gZclFrameControl_FrameTypeSpecific | gZclFrameControl_DisableDefaultRsp,
            NULL, &iPayloadLen,&(pReq->stopReq.cmdFrame));
    if (!pMsg)
      return gZbNoMem_c;
                     
    return ZCL_DataRequestNoCopy(&(pReq->stopReq.addrInfo), iPayloadLen, pMsg);
  }

  return gZclNoMem_c;
}
#endif

/******************************************************************************
*******************************************************************************
* Private functions
*******************************************************************************
******************************************************************************/
void ASL_ZclFillAddrInfo
(
  afAddrInfo_t *addrInfo, zclClusterId_t clusterId,  zbEndPoint_t srcEndPoint
) 
{
  addrInfo->dstEndPoint = gZbBroadcastEndPoint_c;
  Set2Bytes(addrInfo->aClusterId, clusterId);
  addrInfo->srcEndPoint = srcEndPoint;
  addrInfo->txOptions = gZclTxOptions;
  addrInfo->radiusCounter = afDefaultRadius_c;
}


/******************************************************************************
*******************************************************************************
* Private Debug stuff
*******************************************************************************
******************************************************************************/

