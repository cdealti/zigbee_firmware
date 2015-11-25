/******************************************************************************
* ZclZtc.c
*
* This module contains BeeStack specific code for interacting with ZCL/HA
* applications through a serial interface, called ZigBee Test Client (ZTC).
* The PC-side application is called Test Tool.
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
#include "ZtcInterface.h"

#include "ASL_ZclInterface.h"
#include "HaProfile.h"
#include "ZclZtc.h"
#include "SEProfile.h"
#include "zclSE.h"
#include "zclSEPrice.h"
#include "ZclProtocInterf.h"



/******************************************************************************
*******************************************************************************
* Private macros & prototypes
*******************************************************************************
******************************************************************************/


/******************************************************************************
*******************************************************************************
* Public Funcitons
*******************************************************************************
******************************************************************************/

void InitZtcForZcl(void)
{
  /* register with ZTC */
  ZTC_RegisterAppInterfaceToTestClient(ZclReceiveZtcMessage);
}

/*
	ZclReceiveZtcMessage

	Receives a message from ZTC (Test Tool). Used to control the ZCL and HA
	application through the serial port.
*/
void ZclReceiveZtcMessage
(
	ZTCMessage_t* pMsg    /* IN: message from ZTC/UART (Test Tool) */
)
{
	uint8_t opCode;
	zbStatus_t status;
	zclAnyReq_t *pAnyReq;

	/* ignore invalid opcodes */
	if(pMsg->opCode != gHaZtcOpCodeGroup_c)
		return;

	/* opcodes */
	opCode = pMsg->opCodeId;
	status = gZclUnsupportedGeneralCommand_c;
	pAnyReq = (void *)pMsg->data;
	switch(opCode)
	{
		/* read one or more attributes */
		case gHaZtcOpCode_ReadAttr_c:
			status = ZCL_ReadAttrReq(&(pAnyReq->readAttrReq));
		  break;

		/* write one or more attributes */
		case gHaZtcOpCode_WriteAttr_c:
			status = ZCL_WriteAttrReq(&(pAnyReq->writeAttrReq));
		  break;
#if gASL_ZclZtcConfigureReporting_d
		/* Configure reporting */
		case gHaZtcOpCode_CfgReporting_c:
			status = ZCL_ConfigureReportingReq(&(pAnyReq->cfgReporting));
		  break;
#endif
                  
#ifdef gHcGenericApp_d          
#if gHcGenericApp_d
    case gGt_AdvertiseProtocolAddr_c:
      status = GenericTunnel_AdvertiseProtocolAddress(&(pAnyReq->Gt_AdvertiseProtoAddr));
      break;
    case gGt_MatchProtocolAddr_c:
      status = GenericTunnel_MatchProtocolAddress(&(pAnyReq->Gt_MatchProtoAddr));
      break;
    case gGt_MatchProtocolAddrRes_c:
      status = GenericTunnel_MatchProtocolAddressResponse(&(pAnyReq->Gt_MatchProtoAddrRes));
      break;
    case g11073_TransferApdu_c:
      status = IEEE11073ProtocolTunnel_TransferApdu(&(pAnyReq->Ieee11073_TrsApdu));
      break;      
    case g11073_ConnectRequest_c:
      status = IEEE11073ProtocolTunnel_ConnectRequest(&(pAnyReq->Ieee11073_ConnectRequest));
      break;      
    case g11073_DisconnectRequest_c:
      status = IEEE11073ProtocolTunnel_DisconnectRequest(&(pAnyReq->Ieee11073_DisconnectRequest));
      break;
    case g11073_ConnectStatusNotif_c: 
      {
        zclCmd11073_ConnectStatusNotif_t* statusNotif = &(pAnyReq->Ieee11073_ConnectStatusNotif);
        if (statusNotif->connectStatus == zcl11073Status_Disconnected)
          status = IEEE11073ProtocolTunnel_InternalDisconnectReq(&(pAnyReq->Ieee11073_ConnectStatusNotif));
        else
          status = IEEE11073ProtocolTunnel_ConnectStatusNotif(&(pAnyReq->Ieee11073_ConnectStatusNotif));
      }
      break;    
    case g11073_GetIEEE11073MessageProcStatus_c:
      status = IEEE11073ProtocolTunnel_GetIEEE11073MessageProcStatus();
      break;        
#if gZclEnablePartition_d        
	  case g11073_SetPartitionThreshold_c:
	    status =  IEEE11073ProtocolTunnel_SetPartitionThreshold(pAnyReq->Ieee11073_SetPartitionThreshold);
	    break;
#endif	    
#endif
#endif

#ifdef gZclEnablePartition_d    
#if (gZclEnablePartition_d == TRUE)
		case gPartitionZtcOpCode_ReadHandshakeParamReq_c:
			status = ZCL_PartitionReadHandshakeParamReq(&(pAnyReq->partitionReadHandshakeParamReq));
		  break;    
		case gPartitionZtcOpCode_WriteHandshakeParamReq_c:
			status = ZCL_PartitionWriteHandshakeParamReq(&(pAnyReq->partitionWriteHandshakeParamReq));
		  break;    		  
        case gPartitionZtcOpCode_RegisterOutgoingFrame_c:
			status = ZCL_PartitionRegisterTxFrame(&(pAnyReq->partitionClusterFrameInfo));
		  break;   
		case gPartitionZtcOpCode_RegisterIncomingFrame_c:
			status = ZCL_PartitionRegisterRxFrame(&(pAnyReq->partitionClusterFrameInfo));
		  break;   		  
	  case gPartitionZtcOpCode_SetDefaultAttrs_c:
	    status =  ZCL_PartitionSetDefaultAttrs(&(pAnyReq->partitionSetDefaultAttrsReq));
	    break;		  
#endif
#endif

#if gASL_ZclGroupAddGroupReq_d                  
    /* add group request (same for add group and add group if identifying) */
    /* use command 0 in request for add, 5 for add if identifying */
    case gHaZtcOpCode_GroupCmd_AddGroup_c:
      status = ASL_ZclGroupAddGroupReq(&(pAnyReq->addGroupReq));
      break;

    /* Sends a AddGroupIfIdentifying command*/
    case gHaZtcOpCode_GroupCmd_AddGroupIfIdentifying_c:
			status = ASL_ZclGroupAddGroupIfIdentifyReq(&(pAnyReq->addGroupIfIdentifyReq));
      break;
#endif       

#if gASL_ZclGroupViewGroupReq_d      
    /* Sends a ViewGroup command*/
    case gHaZtcOpCode_GroupCmd_ViewGroup_c:
      status = ASL_ZclGroupViewGroupReq(&(pAnyReq->viewGroupReq));
      break;
#endif      


#if gASL_ZclGetGroupMembershipReq_d      
    /* Sends a GetGroupMembership command*/
    case gHaZtcOpCode_GroupCmd_GetGroupMembership_c:
      status = ASL_ZclGetGroupMembershipReq(&(pAnyReq->getGroupMembershipReq));
      break;
#endif      

#if gASL_ZclRemoveGroupReq_d      
    /* Sends a RemoveGroup command*/
    case gHaZtcOpCode_GroupCmd_RemoveGroup_c:
      status = ASL_ZclRemoveGroupReq(&(pAnyReq->removeGroupReq));
      break;
#endif      

#if gASL_ZclRemoveAllGroupsReq_d      
    /* remove all groups */
    case gHaZtcOpCode_GroupCmd_RemoveAllGroups_c:
      status = ASL_ZclRemoveAllGroupsReq(&(pAnyReq->removeAllGroupsReq));
      break;
#endif      

#if gASL_ZclSceneAddSceneReq_d      
    /* Sends a AddScene command */
    case gHaZtcOpCode_SceneCmd_AddScene_c:
			status = ASL_ZclSceneAddSceneReq(&(pAnyReq->addSceneReq), pMsg->length - sizeof(pAnyReq->addSceneReq.addrInfo));
			break;
#endif                        

#if gASL_ZclViewSceneReq_d                        
    /* Sends a ViewScene command */
		case gHaZtcOpCode_SceneCmd_ViewScene_c:
			status = ASL_ZclViewSceneReq(&(pAnyReq->viewSceneReq));
			break;
#endif
                       

#if gASL_ZclRemoveSceneReq_d                        
    /* Sends a RemoveScene command */
    case gHaZtcOpCode_SceneCmd_RemoveScene_c:
			status = ASL_ZclRemoveSceneReq(&(pAnyReq->removeSceneReq));
			break;
#endif                        

                        
#if gASL_ZclRemoveAllScenesReq_d                        
    /* Sends a AddScene command */
    case gHaZtcOpCode_SceneCmd_RemoveAllScenes_c:
			status = ASL_ZclRemoveAllScenesReq(&(pAnyReq->removeAllScenesReq));
			break;
#endif
                       
#if gASL_ZclStoreSceneReq_d                        
    /* Sends a StoreScene command */
    case gHaZtcOpCode_SceneCmd_StoreScene_c:
			status = ASL_ZclStoreSceneReq(&(pAnyReq->storeSceneReq));
			break;
#endif                        

#if gASL_ZclRecallSceneReq_d                        
    /* Sends a RecallScene command */
		case gHaZtcOpCode_SceneCmd_RecallScene_c:
			status = ASL_ZclRecallSceneReq(&(pAnyReq->recallSceneReq));
			break;
#endif
                        
#if gASL_ZclGetSceneMembershipReq_d                        
    /* Sends a GetScene Membership command */
    case gHaZtcOpCode_SceneCmd_GetSceneMembership_c:
			status = ASL_ZclGetSceneMembershipReq(&(pAnyReq->getSceneMembershipreq));
			break;
#endif                        

#if gASL_ZclOnOffReq_d                        
    /* Send the on/off command */
    case gHaZtcOpCode_OnOffCmd_Off_c:
      status = ASL_ZclOnOffReq(&(pAnyReq->onOffReq));
      break;
#endif
      
#if gASL_ZclSetWeeklyScheduleReq                        
    /* Send the SetWeeklySchedule command */
    case gHaZtcOpCode_Thermostat_SetWeeklyScheduleReq:
      status = ZCL_ThermostatSetWeeklyScheduleReq(&(pAnyReq->SetWeeklyScheduleReq));
      break;
#endif
      
#if gASL_ZclGetWeeklyScheduleReq                        
    /* Send the GetWeeklySchedule command */
    case gHaZtcOpCode_Thermostat_GetWeeklyScheduleReq:
      status = ZCL_ThermostatGetWeeklyScheduleReq(&(pAnyReq->GetWeeklyScheduleReq));
      break;
#endif
      
#if gASL_ZclClearWeeklyScheduleReq                        
    /* Send the ClearWeeklySchedule command */
    case gHaZtcOpCode_Thermostat_ClearWeeklyScheduleReq:
      status = ZCL_ThermostatClearWeeklyScheduleReq(&(pAnyReq->ClearWeeklyScheduleReq));
      break;
#endif
    
#if gASL_ZclSetpointRaiseLowerReq                        
    /* Send the ClearWeeklySchedule command */
    case gHaZtcOpCode_Thermostat_SetpointRaiseLowerReq:
      status = ZCL_ThermostatSetpointRaiseLowerReq(&(pAnyReq->SetpointRaiseLowerReq));
      break;
#endif
  
#if gASL_ZclDoorLockReq_d                        
    /* Send the lock/unlock command */
    case gHaZtcOpCode_DoorLockCmd_Lock_c:
      status = ASL_ZclDoorLockReq(&(pAnyReq->doorLockReq));
      break;
#endif      
      
#if gASL_ZclIdentifyReq_d      
    /* Send the identify command */
	  case gHaZtcOpCode_IdentifyCmd_Identify_c: 
	    status = ASL_ZclIdentifyReq(&(pAnyReq->identifyReq));
	    break;	    
#endif   
            
#if gASL_ZclAlarms_ResetAlarm_d
    case gZclZtcOpCode_Alarms_ResetAlarm_c:
      status = Alarms_ResetAlarm(&(pAnyReq->resetAlarmReq));
      break;        
#endif
	    
#if gASL_ZclAlarms_ResetAllAlarms_d      
    case gZclZtcOpCode_Alarms_ResetAllAlarms_c:
      status = Alarms_ResetAllAlarms(&(pAnyReq->resetAllAlarmsReq));
      break;        
#endif
	    
#if gASL_ZclAlarms_Alarm_d      
    case gZclZtcOpCode_Alarms_Alarm_c:
      status = Alarms_Alarm(&(pAnyReq->alarmReq));
      break;        
#endif
      
#if gASL_ZclAlarms_GetAlarm_d      
    case gZclZtcOpCode_Alarms_GetAlarm_c:
      status = Alarms_GetAlarm(&(pAnyReq->getAlarmReq));
      break;    
    case gZclZtcOpCode_Alarms_ResetAlarmLog_c:
      status = Alarms_ResetAlarmLog(&(pAnyReq->resetAlarmLogReq));   
      break;
#endif      
	    
#if gASL_ZclIdentifyQueryReq_d            
	  /* Send the identify query command */
	  case gHaZtcOpCode_IdentifyCmd_IdentifyQuery_c:
	    status = ASL_ZclIdentifyQueryReq(&(pAnyReq->identifyQueryReq));
	    break;		  
#endif            

#if gZclDiscoverAttrReq_d            
	  /* Send the discover attributes command */
	  case gHaZtcOpCode_DiscoverAttr_c:
	    status = ZCL_DiscoverAttrReq(&(pAnyReq->discoverAttrReq));
	    break;		  	    
#endif            
/*******************************************************************/	  
#if gASL_ZclCommissioningRestartDeviceRequest_d
	  case gHaZtcOpCode_CommissioningRestartDeviceRequest_c:
	    status = ZCL_Commisioning_RestartDeviceReq( (zclCommissioningRestartDeviceReq_t*) &(pAnyReq->RestartDeviceReq));
	    break;		  	    
#endif	  
#if gASL_ZclCommissioningSaveStartupParametersRequest_d            
	  case gHaZtcOpCode_CommissioningSaveStartupParametersRequest_c:
	    status = ZCL_Commisioning_SaveStartupParametersReq((zclCommissioningSaveStartupParametersReq_t *) &(pAnyReq->SaveStartupParameterReq));
	    break;		  	    
#endif
#if gASL_ZclCommissioningRestoreStartupParametersRequest_d
	  case gHaZtcOpCode_CommissioningRestoreStartupParametersRequest_c:
	    status = ZCL_Commisioning_RestoreStartupParametersReq((zclCommissioningRestoreStartupParametersReq_t*) &(pAnyReq->RestoreStartupParameterReq));
	    break;		  	    
#endif
#if gASL_ZclCommissioningResetStartupParametersRequest_d            
	  case gHaZtcOpCode_CommissioningResetStartupParametersRequest_c:
	    status = ZCL_Commisioning_ResetStartupParametersReq((zclCommissioningResetStartupParametersReq_t *) &(pAnyReq->ResetStartupParameterReq));
	    break;		 
#endif	    
/*******************************************************************/	

#if gASL_ZclIASZoneReq_d
    /*send IAS Zone Commands */
         case gHaZtcOpCode_IASZoneCmd_EnrollReq_c:             
            status = IASZone_ZoneEnrollRequest((zclIASZone_ZoneEnrollRequest_t  *) &(pAnyReq->zoneEnrollReq));
	    break;
         case gHaZtcOpCode_IASZoneCmd_ChangeNotif_c:             
            status = IASZone_ZoneStatusChange((zclIASZone_ZoneStatusChange_t *) &(pAnyReq->statusChangeNotifReq));
	    break;  
#endif

#if gZclClusterOptionals_d         
         case gHaZtcOpCode_SetValueAttr_c:    
            status = ZCL_SetValueAttr((zclSetAttrValue_t *)pAnyReq);
          break;
#endif
      
#if gASL_ZclIASACEReq_d
    /*send IAS ACE Commands */
         case gHaZtcOpCode_IASACECmd_Arm_c:             
            status = IASACE_Arm((zclIASACE_Arm_t  *) &(pAnyReq->arm));
	    break;
         case gHaZtcOpCode_IASACECmd_Bypass_c:             
            status = IASACE_Bypass((zclIASACE_Bypass_t *) &(pAnyReq->bypass));
	    break;  
         case gHaZtcOpCode_IASACECmd_Emergency_c:             
            status = IASACE_Emergency((zclIASACE_EFP_t  *) &(pAnyReq->emergency));
	    break;
         case gHaZtcOpCode_IASACECmd_Fire_c:             
            status = IASACE_Fire((zclIASACE_EFP_t *) &(pAnyReq->fire));
	    break; 
         case gHaZtcOpCode_IASACECmd_Panic_c:             
            status = IASACE_Panic((zclIASACE_EFP_t  *) &(pAnyReq->panic));
	    break;
         case gHaZtcOpCode_IASACECmd_GetZoneIDMap_c:             
        	status = IASACE_GetZoneIDMap((zclIASACE_EFP_t  *) &(pAnyReq->zoneIdMap));
	    break; 
         case gHaZtcOpCode_IASACECmd_GetZoneInf_c:             
            status = IASACE_GetZoneInformation((zclIASACE_GetZoneInformation_t  *) &(pAnyReq->zoneInf));
	    break; 
#endif            
   
#if gASL_ZclIASWDReq_d
    /*send IAS WD Commands */
         case gHaZtcOpCode_IASWDCmd_StartWarning_c:             
            status = IASWD_StartWarning((zclIASWD_StartWarning_t  *) &(pAnyReq->startWarning));
	    break;
         case gHaZtcOpCode_IASWDCmd_Squawk_c:             
            status = IASWD_Squawk((zclIASWD_Squawk_t *) &(pAnyReq->squawk));
	    break;  
#endif            
            
            
#if gASL_ZclDisplayMsgReq_d
        case gAmiZtcOpCode_Msg_DisplayMsgReq_c:
	    status = ZclMsg_DisplayMsgReq((zclDisplayMsgReq_t *) &(pAnyReq->DisplayMsgReq));
	    break;	
#endif
#if gASL_ZclCancelMsgReq_d            
	  case gAmiZtcOpCode_Msg_CancelMsgReq_c:
	    status = ZclMsg_CancelMsgReq((zclCancelMsgReq_t *) &(pAnyReq->CancelMsgReq));
	    break;
#endif 
#if gASL_ZclGetLastMsgReq_d            
	  case gAmiZtcOpCode_Msg_GetLastMsgRequest_c:
	    status = ZclMsg_GetLastMsgReq((zclGetLastMsgReq_t *) &(pAnyReq->GetLastMsgReq));
	    break;
#endif
#if gASL_ZclMsgConfReq_d            
	  case gAmiZtcOpCode_Msg_MsgConfReq:
	    status = ZclMsg_MsgConf((zclMsgConfReq_t *) &(pAnyReq->MsgConfReq));
	    break;
#endif
            
#if gInterPanCommunicationEnabled_c    
#if gASL_ZclInterPanDisplayMsgReq_d
        case gAmiZtcOpCode_Msg_InterPanDisplayMsgReq_c:
	    status = ZclMsg_InterPanDisplayMsgReq((zclInterPanDisplayMsgReq_t *) &(pAnyReq->InterPanDisplayMsgReq));
	    break;	
#endif
#if gASL_ZclInterPanCancelMsgReq_d            
	  case gAmiZtcOpCode_Msg_InterPanCancelMsgReq_c:
	    status = ZclMsg_InterPanCancelMsgReq((zclInterPanCancelMsgReq_t *) &(pAnyReq->InterPanCancelMsgReq));
	    break;
#endif 
#if gASL_ZclInterPanGetLastMsgReq_d            
	  case gAmiZtcOpCode_Msg_InterPanGetLastMsgRequest_c:
	    status = ZclMsg_InterPanGetLastMsgReq((zclInterPanGetLastMsgReq_t *) &(pAnyReq->InterPanGetLastMsgReq));
	    break;
#endif
#if gASL_ZclMsgConfReq_d            
	  case gAmiZtcOpCode_Msg_InterPanMsgConfReq:
	    status = ZclMsg_InterPanMsgConf((zclInterPanMsgConfReq_t *) &(pAnyReq->InterPanMsgConfReq));
	    break;
#endif
#endif /*#if gInterPanCommunicationEnabled_c    */            
            
#if gASL_ZclSmplMet_GetProfReq_d            
	  case gAmiZtcOpCode_SmplMet_GetProfReq_c:
	    status = ZclSmplMet_GetProfReq((zclSmplMet_GetProfReq_t *) &(pAnyReq->SmplMet_GetProfReq));
	    break;
#endif            
#if gASL_ZclSmplMet_GetProfRsp_d
	  case gAmiZtcOpCode_SmplMet_GetProfRsp_c:
	    status = ZclSmplMet_GetProfRsp((zclSmplMet_GetProfRsp_t *) &(pAnyReq->SmplMet_GetProfRsp));
	    break;	              
#endif
#if gZclFastPollMode_d
#if gASL_ZclSmplMet_FastPollModeReq_d
	  case gAmiZtcOpCode_SmplMet_FastPollModeReq_c:
	    status = ZclSmplMet_ReqFastPollModeReq((zclSmplMet_ReqFastPollModeReq_t *) &(pAnyReq->SmplMet_GetProfRsp));
	    break;	              
#endif
#if gASL_ZclSmplMet_FastPollModeRsp_d
	  case gAmiZtcOpCode_SmplMet_FastPollModeRsp_c:
	    status = ZclSmplMet_ReqFastPollModeRsp((zclSmplMet_ReqFastPollModeRsp_t *) &(pAnyReq->SmplMet_GetProfRsp));
	    break;	              
#endif
#if gASL_ZclSmplMet_AcceptFastPollModeReq_d
          case gAmiZtcOpCode_SmplMet_AcceptFastPollModeReq_c:
            status = ZclSmplMet_AcceptFastPollModeReq(pAnyReq->SmplMet_AcceptFastPollModeReq);
          break;
#endif
#endif          
#if gASL_ZclMet_RequestMirrorReq_d
          case gAmiZtcOpCode_Met_RequestMirrorReq_c:
            status = ZclMet_RequestMirrorReq((zclMet_RequestMirrorReq_t *) &(pAnyReq->Met_RequestMirrorReq));
          break;
#endif
#if gASL_ZclMet_RemoveMirrorReq_d
          case gAmiZtcOpCode_Met_RemoveMirrorReq_c:
            status = ZclMet_RemoveMirrorReq((zclMet_RemoveMirrorReq_t *) &(pAnyReq->Met_RemoveMirrorReq));
          break;
#endif
#if gASL_ZclSmplMet_GetSnapshotReq_d
          case gAmiZtcOpCode_SmplMet_GetSnapshotReq_c:
            status = ZclSmplMet_GetSnapshotReq(&(pAnyReq->SmplMet_GetSnapShotReq));
#endif
#if gASL_ZclDmndRspLdCtrl_ReportEvtStatus_d            
	  case gAmiZtcOpCode_DmndRspLdCtrl_ReportEvtStatusReq_c:
	    status = ZclDmndRspLdCtrl_ReportEvtStatus((zclDmndRspLdCtrl_ReportEvtStatus_t *) &(pAnyReq->DmndRspLdCtrl_ReportEvtStatus));
	    break;
#endif
#if gASL_ZclDmndRspLdCtrl_GetScheduledEvtsReq_d            
	  case gAmiZtcOpCode_DmndRspLdCtrl_GetScheduledEvtsReq_c:
	    status = ZclDmndRspLdCtrl_GetScheduledEvtsReq((zclDmndRspLdCtrl_GetScheduledEvts_t *) &(pAnyReq->DmndRspLdCtrl_GetScheduledEvts));
	    break;
#endif 
#if gASL_ZclDmndRspLdCtrl_LdCtrlEvtReq_d            
	  case gAmiZtcOpCode_DmndRspLdCtrl_LdCtrlEvtReq_c:
	    status = ZclDmndRspLdCtrl_LdCtrlEvtReq((zclDmndRspLdCtrl_LdCtrlEvtReq_t *) &(pAnyReq->DmndRspLdCtrl_LdCtrlEvtReq));
	    break;
#endif
#if gASL_ZclDmndRspLdCtrl_CancelLdCtrlEvtReq_d            
	  case gAmiZtcOpCode_DmndRspLdCtrl_CancelLdCtrlEvtReq_c:
	    status = ZclDmndRspLdCtrl_CancelLdCtrlEvtReq((zclDmndRspLdCtrl_CancelLdCtrlEvtReq_t *) &(pAnyReq->DmndRspLdCtrl_CancelLdCtrlEvtReq));
	    break;
#endif
#if gASL_ZclDmndRspLdCtrl_CancelAllLdCtrlEvtsReq_d            
	  case gAmiZtcOpCode_DmndRspLdCtrl_CancelAllLdCtrlEvtReq_c:
	    status = ZclDmndRspLdCtrl_CancelAllLdCtrlEvtReq((zclDmndRspLdCtrl_CancelAllLdCtrlEvtsReq_t *) &(pAnyReq->DmndRspLdCtrl_CancelAllLdCtrlEvtsReq));
	    break;
#endif
#if gASL_ZclDmndRspLdCtrl_ScheduledServerEvts_d            
	  case gAmiZtcOpCode_DmndRspLdCtrl_ScheduleServerEvt_c:
	    status = ZCL_ScheduleServerLdCtrlEvents((zclCmdDmndRspLdCtrl_LdCtrlEvtReq_t *)pAnyReq);
	    break;
#endif              
#if gASL_ZclPrice_GetCurrPriceReq_d            
	  case gAmiZtcOpCode_Price_GetCurrPriceReq_c:
	    status = zclPrice_GetCurrPriceReq((zclPrice_GetCurrPriceReq_t *) &(pAnyReq->Price_GetCurrPriceReq));
	    break;
#endif
#if gASL_ZclPrice_GetSheduledPricesReq_d            
	  case gAmiZtcOpCode_Price_GetSheduledPricesReq_c:
	    status = zclPrice_GetScheduledPricesReq((zclPrice_GetScheduledPricesReq_t *) &(pAnyReq->Price_GetSheduledPricesReq));
	    break;
#endif

#if gASL_ZclPrice_PublishPriceRsp_d            
	  case gAmiZtcOpCode_Price_PublishPriceRsp_c:
	    status = zclPrice_PublishPriceRsp((zclPrice_PublishPriceRsp_t *) &(pAnyReq->Price_PublishPriceRsp));
	    break;
#endif     

#if gASL_ZclPrice_PublishBlockPeriodsRsp_d            
	  case gAmiZtcOpCode_Price_PublishBlockPeriodsRsp_c:
	    status = zclPrice_PublishBlockPeriodRsp((zclPrice_PublishBlockPeriodRsp_t *) &(pAnyReq->Price_BlockPeriodsRsp));
	    break;
#endif 
            
#if gASL_ZclPrice_GetBlockPeriodsReq_d            
	  case gAmiZtcOpCode_Price_GetBlockPeriodsReq_c:
	    status = zclPrice_GetBlockPeriodsReq((zclPrice_GetBlockPeriodsReq_t *) &(pAnyReq->Price_GetBlockPeriodsReq));
	    break;
#endif 

#if gASL_ZclPrice_GetCalorificValueReq_d            
	  case gAmiZtcOpCode_Price_GetCalorificValueReq_c:
	    status = zclPrice_GetCalorificValueReq((zclPrice_GetCalorificValueReq_t *) &(pAnyReq->Price_GetCalorificValueReq));
	    break;
#endif 
            
#if gASL_ZclPrice_GetConversionFactorReq_d
        case gAmiZtcOpCode_Price_GetConversionFactorReq_c:
          status = zclPrice_GetConversionFactorReq((zclPrice_GetConversionFactorReq_t *) &(pAnyReq->Price_GetConversionFactor));
          break;
#endif

#if gASL_ZclSE_Use_NextGenFeatures_d           
#if gASL_ZclPrice_GetBillingPeriodReq_d            
	  case gAmiZtcOpCode_Price_GetBillingPeriodReq_c:
	    status = zclPrice_GetBillingPeriodReq((zclPrice_GetBillingPeriodReq_t *) &(pAnyReq->Price_GetBillingPeriodReq));
	    break;
#endif 
#endif 
            
#if gASL_ZclSE_Use_NextGenFeatures_d           
#if gASL_ZclPrice_GetPriceMatrixReq_d            
	  case gAmiZtcOpCode_Price_GetPriceMatrixReq_c:
	    status = zclPrice_GetPriceMatrixReq((zclPrice_GetPriceMatrixReq_t *) &(pAnyReq->Price_GetPriceMatrixReq));
	    break;
#endif 
#endif 
            
#if gASL_ZclSE_Use_NextGenFeatures_d           
#if gASL_ZclPrice_GetBlockThresholdsReq_d            
	  case gAmiZtcOpCode_Price_GetBlockThresholdsReq_c:
	    status = zclPrice_GetBlockThresholdsReq((zclPrice_GetBlockThresholdsReq_t *) &(pAnyReq->Price_GetBlockThresholdsReq));
	    break;
#endif 
#endif 
            
#if gASL_ZclSE_Use_NextGenFeatures_d           
#if gASL_ZclPrice_GetTariffInformationReq_d            
	  case gAmiZtcOpCode_Price_GetTariffInformationReq_c:
	    status = zclPrice_GetTariffInformationReq((zclPrice_GetTariffInformationReq_t *) &(pAnyReq->Price_GetTariffInformationReq));
	    break;
#endif 
#endif 
            
            
#if gASL_ZclSE_Use_NextGenFeatures_d           
#if gASL_ZclPrice_GetCO2ValueReq_d            
	  case gAmiZtcOpCode_Price_GetCO2ValueReq_c:
	    status = zclPrice_GetCO2ValueReq((zclPrice_GetCO2ValueReq_t *) &(pAnyReq->Price_GetCO2ValueReq));
	    break;
#endif 
#endif 
            
#if gASL_ZclSE_Use_NextGenFeatures_d           
#if gASL_ZclPrice_GetConsolidatedBillReq_d            
	  case gAmiZtcOpCode_Price_GetConsolidatedBillReq_c:
	    status = zclPrice_GetConsolidatedBillReq((zclPrice_GetConsolidatedBillReq_t *) &(pAnyReq->Price_GetConsolidatedBillReq));
	    break;
#endif 
#endif 
            
#if gASL_ZclPrice_Optionals_d && gASL_ZclSE_Use_NextGenFeatures_d            
#if gASL_ZclPrice_PublishConversionFactorRsp_d            
	  case gAmiZtcOpCode_Price_ScheduelServeConversionFactorStore_c:
	    status = ZCL_ScheduleServerConversionFactorStore((ztcCmdPrice_PublishConversionFactorRsp_t *) &(pAnyReq->ztcPublishConversionFactorRsp));
	    break;
#endif
#endif
          
#if gASL_ZclPrice_Optionals_d && gASL_ZclSE_Use_NextGenFeatures_d 
#if gASL_ZclPrice_PublishCalorificValueRsp_d            
	  case gAmiZtcOpCode_Price_ScheduelServeCalorificValueStore_c:
	    status = ZCL_ScheduleServerCalorificValueStore((ztcCmdPrice_PublishCalorificValueRsp_t *) &(pAnyReq->ztcPublishCalorificValueRsp));
	    break;
#endif  
#endif
       
#if gASL_ZclPrice_Optionals_d            
#if gASL_ZclPrice_PublishBlockPeriodsRsp_d            
	  case gAmiZtcOpCode_Price_ScheduleServerBlockPeriods_c:
	    status = ZCL_ScheduleServerBlockPeriodsEvents(( ztcCmdPrice_PublishBlockPeriodRsp_t *)pAnyReq);
	    break;
#endif
#endif

#if gASL_ZclSE_Use_NextGenFeatures_d            
#if gASL_ZclPrice_PublishTariffInformationRsp_d            
	  case gAmiZtcOpCode_Price_ScheduleServerTariffInformation_c:
	    status = ZCL_ScheduleServerTariffInformationEvents(( ztcCmdPrice_PublishTariffInformationRsp_t *)pAnyReq);
	    break;
#endif
#endif
            
#if gASL_ZclSE_Use_NextGenFeatures_d            
#if gASL_ZclPrice_PublishBillingPeriodRsp_d            
	  case gAmiZtcOpCode_Price_ScheduleServerBillingPeriod_c:
	    status = ZCL_ScheduleServerBillingPeriodEvents(( ztcCmdPrice_PublishBillingPeriodRsp_t *)pAnyReq);
	    break;
#endif
#endif
            
#if gASL_ZclSE_Use_NextGenFeatures_d            
#if gASL_ZclPrice_PublishTariffInformationRsp_d            
	  case gAmiZtcOpCode_Price_ScheduleServerTariffInformation_c:
	    status = ZCL_ScheduleServerTariffInformationEvents(( ztcCmdPrice_PublishTariffInformationRsp_t *)pAnyReq);
	    break;
#endif
#endif
            
#if gASL_ZclSE_Use_NextGenFeatures_d            
#if gASL_ZclPrice_PublishCO2ValueRsp_d            
	  case gAmiZtcOpCode_Price_ScheduleServerCO2Value_c:
	    status = ZCL_ScheduleServerCO2ValueEvents(( ztcCmdPrice_PublishCO2ValueRsp_t *)pAnyReq);
	    break;
#endif
#endif
            
#if gASL_ZclSE_Use_NextGenFeatures_d            
#if gASL_ZclPrice_PublishPriceMatrixRsp_d            
	  case gAmiZtcOpCode_Price_StoreServerPriceMatrix_c:
	    status = ZCL_StoreServerPriceMatrix(( ztcCmdPrice_PublishPriceMatrixRsp_t *)pAnyReq);
	    break;
#endif
#endif
            
#if gASL_ZclSE_Use_NextGenFeatures_d            
#if gASL_ZclPrice_PublishBlockThresholdsRsp_d            
	  case gAmiZtcOpCode_Price_StoreServerBlockThresholds_c:
	    status = ZCL_StoreServerBlockThresholds(( ztcCmdPrice_PublishBlockThresholdsRsp_t *)pAnyReq);
	    break;
#endif
#endif
            
#if gASL_ZclSE_Use_NextGenFeatures_d            
#if gASL_ZclPrice_PublishConsolidatedBillRsp_d            
	  case gAmiZtcOpCode_Price_ScheduleServerConsolidatedBill_c:
	    status = ZCL_ScheduleServerConsolidatedBillEvents(( ztcCmdPrice_PublishConsolidatedBillRsp_t *)pAnyReq);
	    break;
#endif
#endif

#if gASL_ZclSE_Use_NextGenFeatures_d            
#if gASL_ZclPrice_PublishCPPEventRsp_d            
	  case gAmiZtcOpCode_Price_ScheduleServerCPPEvent_c:
	    status = ZCL_ScheduleServerCPPEvents((zclCmdPrice_PublishCPPEventRsp_t *)pAnyReq);
	    break;
#endif
#endif

#if gASL_ZclPrice_Optionals_d
#if gASL_ZclPrice_PublishBlockPeriodsRsp_d            
	  case gAmiZtcOpCode_Price_UpdateServerBlockPeriods_c:
	    status = ZCL_UpdateServerBlockPeriodEvents((zclCmdPrice_PublishBlockPeriodRsp_t *)pAnyReq);
	    break;
#endif
#endif
            
#if gASL_ZclPrice_Optionals_d           
#if gASL_ZclPrice_PublishBlockPeriodsRsp_d            
	  case gAmiZtcOpCode_Price_DeleteServerScheduledBlockPeriods_c:
	      ZCL_DeleteScheduleServerBlockPeriods();
              status = gZclSuccess_c;
	    break;
#endif
#endif
            
#if gASL_ZclPrice_PublishPriceRsp_d            
	  case gAmiZtcOpCode_Price_ScheduleServerPrice_c:
	    status = ZCL_ScheduleServerPriceEvents((ztcCmdPrice_PublishPriceRsp_t *)pAnyReq);
	    break;
#endif   

#if gASL_ZclPrice_PublishPriceRsp_d            
	  case gAmiZtcOpCode_Price_UpdateServerPrice_c:
	    status = ZCL_UpdateServerPriceEvents((zclCmdPrice_PublishPriceRsp_t *)pAnyReq);
	    break;
#endif   

#if gASL_ZclPrice_PublishPriceRsp_d            
	  case gAmiZtcOpCode_Price_DeleteServerScheduledPrices_c:
	       ZCL_DeleteServerScheduledPrices();
               status = gZclSuccess_c;
	    break;
#endif               
#if gInterPanCommunicationEnabled_c    
#if gASL_ZclPrice_InterPanGetCurrPriceReq_d            
          case gAmiZtcOpCode_Price_InterPanGetCurrPriceReq_c:
                status = zclPrice_InterPanGetCurrPriceReq((zclPrice_InterPanGetCurrPriceReq_t *) &(pAnyReq->InterPanGetCurrPriceReq));
            break;
#endif
#if gASL_ZclPrice_InterPanGetSheduledPricesReq_d    
          case gAmiZtcOpCode_Price_InterPanGetSheduledPricesReq_c:
                status = zclPrice_InterPanGetScheduledPricesReq((zclPrice_InterPanGetScheduledPricesReq_t *) &(pAnyReq->InterPanGetScheduledPricesReq));
           break;
#endif 
#if gASL_ZclPrice_InterPanPublishPriceRsp_d    
          case gAmiZtcOpCode_Price_InterPanPublishPriceRsp_c:
                status = zclPrice_InterPanPublishPriceRsp((zclPrice_InterPanPublishPriceRsp_t *) &(pAnyReq->InterPanPublishPriceRsp));
          break;    
#endif    
#endif
#if gASL_ZclPrepayment_SelAvailEmergCreditReq_d
	  case gAmiZtcOpCode_Prepayment_SelAvailEmergCreditReq_c:
	    status = zclPrepayment_Client_SelAvailEmergCreditReq((zclPrepayment_SelAvailEmergCreditReq_t *) &(pAnyReq->Prepayment_SelAvailEmergCreditReq));
	    break;	              
#endif
#if gASL_ZclPrepayment_ChangeSupplyReq_d
	  case gAmiZtcOpCode_Prepayment_ChangeSupplyReq_c:
	    status = zclPrepayment_Client_ChangeSupplyReq((zclPrepayment_ChangeSupplyReq_t *) &(pAnyReq->SmplMet_GetProfRsp));
	    break;
#endif
#if gASL_ZclKeyEstab_InitKeyEstabReq_d    
	  case gAmiZtcOpCode_KeyEstab_InitKeyEstabReq_c:
	    status = zclKeyEstab_InitKeyEstabReq((ZclKeyEstab_InitKeyEstabReq_t *) &(pAnyReq->KeyEstab_InitKeyEstabReq));
	    break;	  
#endif
#if gASL_ZclKeyEstab_EphemeralDataReq_d            
	  case gAmiZtcOpCode_KeyEstab_EphemeralDataReq_c:
	    status = zclKeyEstab_EphemeralDataReq((ZclKeyEstab_EphemeralDataReq_t *) &(pAnyReq->KeyEstab_EphemeralDataReq));
	    break;	  	  
#endif
#if gASL_ZclKeyEstab_ConfirmKeyDataReq_d            
	  case gAmiZtcOpCode_KeyEstab_ConfirmKeyDataReq_c:
	    status = zclKeyEstab_ConfirmKeyDataReq((ZclKeyEstab_ConfirmKeyDataReq_t *) &(pAnyReq->KeyEstab_ConfirmKeyDataReq));
	    break;	   
#endif            
#if gASL_ZclKeyEstab_TerminateKeyEstabServer_d            
	  case gAmiZtcOpCode_KeyEstab_TerminateKeyEstabServer_c:
	    status = zclKeyEstab_TerminateKeyEstabServer((ZclKeyEstab_TerminateKeyEstabServer_t *) &(pAnyReq->KeyEstab_TerminateKeyEstabServer));
	    break;	  
#endif
#if gASL_ZclKeyEstab_InitKeyEstabRsp_d            
	  case gAmiZtcOpCode_KeyEstab_InitKeyEstabRsp_c:
	    status = zclKeyEstab_InitKeyEstabRsp((ZclKeyEstab_InitKeyEstabRsp_t *) &(pAnyReq->KeyEstab_InitKeyEstabRsp));
	    break;	  	  
#endif
#if gASL_ZclKeyEstab_EphemeralDataRsp_d            
	  case gAmiZtcOpCode_KeyEstab_EphemeralDataRsp_c:
	    status = zclKeyEstab_EphemeralDataRsp((ZclKeyEstab_EphemeralDataRsp_t *) &(pAnyReq->KeyEstab_EphemeralDataRsp));
	    break;
#endif
#if gASL_ZclKeyEstab_ConfirmKeyDataRsp_d            
	  case gAmiZtcOpCode_KeyEstab_ConfirmKeyDataRsp_c:
	    status = zclKeyEstab_ConfirmKeyDataRsp((ZclKeyEstab_ConfirmKeyDataRsp_t *) &(pAnyReq->KeyEstab_ConfirmKeyDataRsp));
	    break;	  	  
#endif 
#if gASL_ZclKeyEstab_TerminateKeyEstabClient_d            
	  case gAmiZtcOpCode_KeyEstab_TerminateKeyEstabClient_c:
	    status = zclKeyEstab_TerminateKeyEstabClient((ZclKeyEstab_TerminateKeyEstabClient_t *) &(pAnyReq->KeyEstab_TerminateKeyEstabClient));
	    break;	  
#endif
#if gEccIncluded_d == 1
	  case gAmiZtcOpCode_KeyEstab_SetSecMaterial_c:
        status = zclKeyEstab_SetSecurityMaterial((ZclKeyEstab_SetSecurityMaterial_t *) &(pAnyReq->KeyEstab_SetSecurityMaterial));
	    break;
      case gAmiZtcOpCode_KeyEstab_InitKeyEstab_c:
        status = (uint8_t)!ZCL_InitiateKeyEstab(((ZclKeyEstab_InitiateKeyEstab_t *)(pMsg->data))->dstEndpoint, 
                   ((ZclKeyEstab_InitiateKeyEstab_t *)(pMsg->data))->srcEndpoint, ((ZclKeyEstab_InitiateKeyEstab_t *)(pMsg->data))->dstAddr);
        break;      
#endif	    
#if gASL_ZclSE_RegisterDevice_d
        case gAmiZtcOpCode_SE_RegisterDeviceReq_c:
            if (pSE_ESPRegFunc != NULL)
              status = (*pSE_ESPRegFunc)((EspRegisterDevice_t*)pAnyReq);
          break;
        case gAmiZtcOpCode_SE_DeRegisterDeviceReq_c:
            if (pSE_ESPDeRegFunc != NULL)
              status = (*pSE_ESPDeRegFunc)((EspDeRegisterDevice_t*)pAnyReq);
          break;
#endif            
#if gASL_ZclSE_InitTime_d
        case gGeneralZtcOpCode_IniTimeReq_c:
          ZCL_TimeInit((ZCLTimeConf_t *) pAnyReq);
        break;
#endif          

#if gZclEnableOTAServer_d:
    case gOTAImageNotify_c:
      status = ZCL_OTAImageNotify((zclZtcOTAImageNotify_t *)pAnyReq);
    break;
    case gOTAQueryNextImageResponse_c:
      status = ZCL_OTANextImageResponse((zclZtcOTANextImageResponse_t *)pAnyReq);
      break;
    case gOTABlockResponse_c:
      status = ZCL_OTABlockResponse((zclZtcOTABlockResponse_t *)pAnyReq);
      break;
  case gOTAUpgradeEndResponse_c:
      status = ZCL_OTAUpgradeEndResponse((ztcZclOTAUpgdareEndResponse_t *)pAnyReq);
      break;
#endif	
#if (gZclEnableOTAServer_d == TRUE)||(gZclEnableOTAClient_d == TRUE)
  case gOTAInitiateProcess_c:
      status = OTA_InitiateOtaClusterProcess((zclInitiateOtaProcess_t *)pAnyReq);
      break; 
#endif      
#if gZclEnableOTAClient_d:	
    case gOTASetClientParams_c:
      status = ZCL_OTASetClientParams((zclOTAClientParams_t *)pAnyReq);
      break;
    case gOTABlockRequest_c:
      status = ZCL_OTABlockRequest((zclZtcOTABlockRequest_t *)pAnyReq);
      break; 
    case gOTAStartClientNextImageTransfer_c:
      status = OTA_OtaStartClientNextImageTransfer((zclStartClientNextImageTransfer_t *)pAnyReq); 
      break;  
#endif
      
#if gASL_ZclSETunneling_d
          case gSeTunnel_Client_TunnelReq_c:
            status = ZCL_SETunnel_Client_RequestTunnelReq((zclSETunneling_RequestTunnelReq_t *)pAnyReq);
            break;
#if gASL_ZclZtcSETunnelingTesting_d
          case gSeTunnel_LoadFragment_c:
            status = ZCL_SETunnel_ZTCLoadFragment((zclSETunneling_ZTCLoadFragment_t *)pAnyReq);
            break;
          case gSeTunnel_TransferData_c:
            status = ZCL_SETunnel_ZTCTransferDataReq((zclSETunneling_ZTCTransferDataReq_t *)pAnyReq);
            break;
          case gSeTunnel_Server_SetNextTunnelID_c:
            status = ZCL_SETunnel_Server_SetNextTunnelID(pAnyReq->zclSETunneling_TunnelID);
            break;
#if gASL_ZclSETunnelingOptionals_d            
          case gSeTunnel_ReadyRx_c:
            status = ZCL_SETunnel_ReadyRx((zclSETunneling_ReadyRx_t *)pAnyReq);
            break;
#endif
            
#endif
          
#endif
        
#if gASL_ZclLevelControlReq_d          
    default:
      /* Send level cluster commands; they have a count of 8 and are similar, so 
       * they are processed based on opcode difference from MoveToLevel */
      if (opCode >= gHaZtcOpCode_LevelControlCmd_MoveToLevel_c && opCode <= gHaZtcOpCode_LevelControlCmd_StopOnOff_c)
        status = ASL_ZclLevelControlReq(&(pAnyReq->levelControlReq), gZclCmdLevelControl_MoveToLevel_c + (opCode - gHaZtcOpCode_LevelControlCmd_MoveToLevel_c));
      break;
#endif      
      	    
  }
  

  /* done, send confirm */
  /* ZTC data req */
#ifndef gHostApp_d  
  ZTCQueue_QueueToTestClient(&status, gHaZtcOpCodeGroup_c, opCode, sizeof(status));
#else
  ZTCQueue_QueueToTestClient(gpHostAppUart, &status, gHaZtcOpCodeGroup_c, opCode, sizeof(status));
#endif
}

