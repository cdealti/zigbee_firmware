/******************************************************************************
* ZclMain.c
*
* This source file contains commands the task used by the ZigBee cluster
* library. This task handles all ZCL events (which may in turn end up in the
* application through the ASL interface).
*
* Copyright (c) 2007, Freescale, Inc. All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
* [R1] - 053520r16ZB_HA_PTG-Home-Automation-Profile.pdf
* [R2] - 075123r00ZB_AFG-ZigBee_Cluster_Library_Specification.pdf
*
******************************************************************************/
#include "EmbeddedTypes.h"
#include "zigbee.h"
#include "FunctionLib.h"
#include "BeeStackConfiguration.h"
#include "BeeStack_Globals.h"
#include "AppAfInterface.h"
#include "AfApsInterface.h"
#include "BeeStackInterface.h"
#include "EndPointConfig.h"
#include "TMR_Interface.h"
#include "ZCL.h"
#include "ASL_ZCLInterface.h"
#include "zclSE.h"
#include "zclSEPrice.h"


/******************************************************************************
*******************************************************************************
* Private macros
*******************************************************************************
******************************************************************************/


/******************************************************************************
*******************************************************************************
* Private type definitions
*******************************************************************************
******************************************************************************/


/******************************************************************************
*******************************************************************************
* Private data
*******************************************************************************
******************************************************************************/

#if gZclEnableReporting_c

index_t gZclReportDeviceIndex;  /* for state machine, sending a report */
index_t gZclReportClusterIndex; /*  */
index_t gAsynchronousClusterIndex=0;

/******************************************************************************
*******************************************************************************
* Public data
*******************************************************************************
******************************************************************************/
bool_t  gfZclReportRestart;
#endif

tsTaskID_t gZclTaskId;
void ZCL_ReportingTimeout(tmrTimerID_t iTimerId);

/******************************************************************************
*******************************************************************************
* Private prototypes
*******************************************************************************
******************************************************************************/

void TS_ZclTask(uint16_t events);
void ZCL_SendReport(void);
uint8_t ZCL_BuildAttrReport
  (
  zclCmdReportAttr_t *pAttrRecord,  /* IN: buffer to build report */
  afDeviceDef_t      *pDevice,      /* IN: device for building report */
  afClusterDef_t     *pCluster      /* IN: cluster definition */
  );


bool_t gApsAckConfirmStatus;  //TRUE = Succes, FALSE = Otherwise
/******************************************************************************
*******************************************************************************
* Public functions
*******************************************************************************
******************************************************************************/

/*****************************************************************************
* ZCL_Init
* 
* Initialize each endpoint which is a ZCL endpoint.
******************************************************************************/
void ZCL_Init
  (
  void
  )
{
  /* allocate a timer for identify */
  gZclIdentifyTimerID = TMR_AllocateTimer();

  /* Initializa the timer for the Level Control Cluster */
  gZclLevel_TransitionTimerID = TMR_AllocateTimer();

  /* initialize ZTC if enabled */
  InitZtcForZcl();

  /* reset each cluster to defaults */
  ZCL_Reset();
  
  gZclTaskId = TS_CreateTask(gTsZclTaskPriority_c, TS_ZclTask);
#if gZclEnableReporting_c
  /* initialize the ZCL task */
  gZclReportingTimerID = TMR_AllocateTimer();
#endif
  
  gZclCommonAttr.basicDeviceEnabled = gZclBasicAttr_DeviceEnabled;

}


/*******************************************************************************
* Task for ZigBee Cluster Library. Used to signal ourselves of events.
*******************************************************************************/
void TS_ZclTask
(
uint16_t events  /* IN: the ID to shoose from which to which sap handler the message should flow */
)
{
/*prevent compiler warnings*/
(void) events;

#if gZclEnableReporting_c  
  /* time to send a report */
  if(events & gZclEventReportTimeout_c)
    ZCL_SendReport();
  
  if(events & gZclEventReportTick_c) {
    
    /*Start a 1 second timer*/
    TMR_StartSingleShotTimer(gZclReportingTimerID, 
                             (tmrTimeInMilliseconds_t)1000,  
                             ZCL_ReportingTimeout);
  }
#endif 
  
#if gASL_ZclDmndRspLdCtrl_ReportEvtStatus_d  
  if(events & gZclEvtHandleLdCtrl_c)
    ZCL_HandleScheduledEventNow();
  if(events & gzclEvtHandleReportEventsStatus_c)
    ZCL_HandleReportEventsStatus();
#endif 
  
#if gASL_ZclDmndRspLdCtrl_GetScheduledEvtsReq_d   
  if(events & gzclEvtHandleGetScheduledLdCtlEvts_c)
    ZCL_HandleGetScheduledLdCtlEvts();
#endif

if(events & gzclEvtHandlePriceClusterEvt_c)
    ZCL_HandleSEPriceClusterEvt();  
}

#if gZclEnableReporting_c  
/*******************************************************************************
* State machine to send the next report. If it can't get the memory, tries
* again later.
*
* This will concatinate all of the reportable attributes in a single cluster
*******************************************************************************/
void ZCL_SendReport(void)
{
  afDeviceDef_t   *pDevice;
  uint8_t payloadLen;
  uint8_t reportLen;
  afToApsdeMessage_t *pMsg; /* a message for sending the report */
  afAddrInfo_t addrInfo;
  afClusterDef_t *pCluster;
  zclFrame_t  *pFrame;
  uint8_t i;
  zclReportAttr_t *pReportList;
  
  BeeUtilZeroMemory(&addrInfo, sizeof(addrInfo));
  /* starting over? reset indexes */
  if(gfZclReportRestart) {
    gZclReportDeviceIndex = gZclReportClusterIndex = gAsynchronousClusterIndex = 0;
    gfZclReportRestart = FALSE;
  }

  /* get a buffer to build the next report */
  pMsg = AF_MsgAlloc();
  if(!pMsg) {
    gfZclReportRestart = FALSE;
    ZLC_StartShortReportingTimer();
    return;
  }

  /* walk through all devices */
  while(gZclReportDeviceIndex < gNum_EndPoints_c) {

    /* make sure this app endpoint is a ZCL device */
    pDevice = (afDeviceDef_t *) endPointList[gZclReportDeviceIndex].pDevice;
    
    /* report list */
    pReportList = pDevice->pReportList;
    
    if(!pDevice || !pDevice->pfnZCL) {
      ++gZclReportDeviceIndex;
      continue;
    }

    /* Find if any attribute are a asynchronous, then update the attribute */ 
    do
    {
      for(i=0; i<pDevice->reportCount; ++i)
      {
        zclAttrDef_t *pAttrDef;
        zclReportAttr_t *pCurrentReportList = &pReportList[i];
        /* only looking for this one cluster, to see if it's in the reportinglist */
        if(!IsEqual2Bytes(pCurrentReportList->aClusterId, pDevice->pClusterDef[gAsynchronousClusterIndex].aClusterId))
          continue;            
               
        // If the cluster is on the reporting list, find the reporting attribute
        pAttrDef = ZCL_FindAttr(&pDevice->pClusterDef[gAsynchronousClusterIndex], pCurrentReportList->attrId);
        if(pAttrDef)
        {
          /* It's a asynchronous attribute, update before reporting */
          if(ZclAttrIsAsynchronous_c(pAttrDef->flags))
          {
            ++gAsynchronousClusterIndex;
            BeeAppUpdateDevice(0, gZclUI_SendReportingAttributeRequest_c, pAttrDef->id, pCurrentReportList->aClusterId, NULL);
            //Free the message, because we return from here
            MSG_Free(pMsg);
            return;
          }
        }
        
      }
      ++gAsynchronousClusterIndex;
    }while(gAsynchronousClusterIndex < pDevice->clusterCount);

    /* check each cluster for reporting attributes */
    while(gZclReportClusterIndex < pDevice->clusterCount) {

      pCluster = &pDevice->pClusterDef[gZclReportClusterIndex];

      pFrame = (void *)(&((uint8_t *)pMsg)[ApsmeGetAsduOffset()]);

      /* build the report for the next cluster */
      reportLen = ZCL_BuildAttrReport(
        (zclCmdReportAttr_t *)(pFrame + 1),   /* ptr to report frame */
        pDevice,                              /* ptr to device */
        pCluster
        );            

      /* no reporting attributes this cluster */
      if(!reportLen) {
        ++gZclReportClusterIndex;
        continue;
      }

      /* set up the address info */
      addrInfo.dstAddrMode = gZbAddrModeIndirect_c;
      addrInfo.srcEndPoint = endPointList[gZclReportDeviceIndex].pEndpointDesc->pSimpleDesc->endPoint;
      addrInfo.txOptions = gZclTxOptions;
      addrInfo.radiusCounter = afDefaultRadius_c;

      /* determine which cluster to send it to */
      Copy2Bytes(addrInfo.aClusterId, pCluster->aClusterId);

      /* set up frame */
      pFrame->frameControl = gZclFrameControl_FrameTypeGeneral | gZclFrameControl_DisableDefaultRsp;
      pFrame->transactionId = gZclTransactionId++;
      pFrame->command = gZclCmdReportAttr_c;

      /* send the report */
      payloadLen = sizeof(zclFrame_t) + reportLen;
      (void)ZCL_DataRequestNoCopy(&addrInfo, payloadLen, pMsg);

      ++gZclReportClusterIndex;

      /* start a short timer between reporting clusters */
      gfZclReportRestart = FALSE;
      ZLC_StartShortReportingTimer();
      return;
    }
    
    /* try next device */
    ++gZclReportDeviceIndex;
    gZclReportClusterIndex=0;
    gAsynchronousClusterIndex=0;
  } /* end of while(gZclReportDeviceIndex < gNum_EndPoints_c) */

  if(pMsg)
    MSG_Free(pMsg);

  /* start up a new timer if needed */
  if(gZclReportingSetup.reportTimeout != 0 && gZclReportingSetup.reportTimeout != 0xFFFF)
  {
      gZclReportingSetup.reportCounter = gZclReportingSetup.reportTimeout;
      ZLC_StartReportingTimer();
  }
}

/*******************************************************************************
* Build the report for this cluster. May be empty.
*
* Returns 0 if nothing to report. Returns length of report if one or more
* attributes to report.
*******************************************************************************/
uint8_t ZCL_BuildAttrReport
  (
  zclCmdReportAttr_t *pAttrRecord,  /* IN: buffer to build report */
  afDeviceDef_t      *pDevice,      /* IN: device for building report */
  afClusterDef_t     *pCluster      /* IN: cluster definition */
  )
{
  uint8_t i;
  uint8_t attrLen;
  uint8_t reportLen;
  zclReportAttr_t *pReportList;
  uint8_t * pAttrData;
  zclAttrDef_t *  pAttrDef;

  /* no reportable attributes on this endpoint */
  if(!pDevice->reportCount)
    return 0;

  /* report list */
  pReportList = pDevice->pReportList;

  /* prepare for building the attribute list from the clusters */ 
  /* look through attributes on this cluster */
  reportLen = 0;
  for(i=0; i<pDevice->reportCount; ++i) {
    zclReportAttr_t *pCurrentReportList = &pReportList[i];

    /* only looking for this one cluster */
    if(!IsEqual2Bytes(pCurrentReportList->aClusterId, pCluster->aClusterId))
      continue;

    /* report the attribute if being reported */

    /* although it's reportable, is it being reported? */
    if(BeeUtilGetIndexedBit(pDevice->pData, i)) {

      /* add the attribute to the list */
      pAttrRecord->attrId = pCurrentReportList->attrId;

      pAttrDef = ZCL_FindAttr(pCluster, pAttrRecord->attrId);
      if (pAttrDef)
        pAttrRecord->attrType = pAttrDef->type;

        /* get a pointer to the data */
        pAttrData = ZCL_GetAttributePtr(pDevice, pCluster, pCurrentReportList->attrId, &attrLen);
        FLib_MemCpy(pAttrRecord->aData, pAttrData, attrLen);

        /* on to next record */
        attrLen += (sizeof(zclCmdReportAttr_t) - MbrSizeof(zclCmdReportAttr_t,aData));
        pAttrRecord = (zclCmdReportAttr_t *)(((uint8_t *)pAttrRecord) + attrLen); 
        reportLen += attrLen;

    } /* end of if being reported */
  } /* end of for loop */

  return reportLen;
}

/******************************************************************************
*******************************************************************************
* Private functions
*******************************************************************************
******************************************************************************/

#endif  /* gZclEnableReporting_c */

void ZclApsAckConfirm(zbNwkAddr_t address, zbStatus_t status)
{
	(void)address;
	if(status == gZbSuccess_c)
		gApsAckConfirmStatus = TRUE;
	else
		gApsAckConfirmStatus = FALSE; 
}
