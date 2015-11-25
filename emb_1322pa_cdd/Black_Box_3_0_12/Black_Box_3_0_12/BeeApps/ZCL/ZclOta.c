/******************************************************************************
* ZclOTA.c
*
* This module contains code that handles ZigBee Cluster Library commands and
* clusters for OTA (Over The Air upgrading cluster).
*
* Copyright (c) 2011, Freescale, Inc.  All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
* Documents used in this specification:
* [R1] - 095264r17.pdf
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
#include "ZclOptions.h"
#if gZclEnableOTAServer_d
#if defined(PROCESSOR_HCS08)||defined(PROCESSOR_MC13233C)||defined(PROCESSOR_MC13234C)
#include "BootloaderFlashUtils.h"
#endif
#endif
#if gZclEnableOTAServer_d || gZclEnableOTAClient_d

#include "OtaSupport.h"
#include "ZclOta.h"
#include "ApsMgmtInterface.h"
#include "ASL_UserInterface.h"
#include "ASL_ZdpInterface.h"
#include "EndPointConfig.h"
#include "ZTCInterface.h"

#if (gZclEnableOTAClientECCLibrary_d == TRUE)
#include "ASM_interface.h"
#include "eccapi.h"
#endif


#ifndef __IAR_SYSTEMS_ICC__
#include "PwrLib.h"
#ifdef MC13226Included_d
    #undef MC13226Included_d
#endif    
    #define MC13226Included_d 0
#else
#include "crm.h"
#endif

/******************************************************************************
*******************************************************************************
* Private macros
*******************************************************************************
******************************************************************************/

#define OTA_MANUFACTURER_CODE_FSL         0x1004
#define OTA_MANUFACTURER_CODE_MATCH_ALL   0xFFFF


#define OTA_IMAGE_TYPE_CURRENT            0x0000

#define OTA_IMAGE_TYPE_MAX_MANUFACTURER   0xFFBF
#define OTA_IMAGE_TYPE_CREDENTIAL         0xFFC0
#define OTA_IMAGE_TYPE_CONFIGURATION      0xFFC1
#define OTA_IMAGE_TYPE_LOG                0xFFC2  
#define OTA_IMAGE_TYPE_MATCH_ALL          0xFFFF

#define OTA_FILE_IDENTIFIER               0x0BEEF11E
#define OTA_FILE_VERSION_CURRENT          0x30103010
#define OTA_FILE_VERSION_DL_DEFAULT       0x00000000

#ifdef __IAR_SYSTEMS_ICC__
#if MC13226Included_d
#define OTA_HW_VERSION_CURRENT            0x2260 
#else
#define OTA_HW_VERSION_CURRENT            0x2240
#endif
#else  //  __IAR_SYSTEMS_ICC__
#if gTargetQE128EVB_d                    
#define OTA_HW_VERSION_CURRENT            0x2028
#else
#if gTargetMC1323xRCM_d || gTargetMC1323xREM_d
#define OTA_HW_VERSION_CURRENT            0x2028
#endif
#endif
#endif

#define OTA_CLIENT_MAX_DATA_SIZE          0x30  
#define OTA_CLIENT_MAX_RANDOM_JITTER      100

#define gHaZtcOpCodeGroup_c                 0x70 /* opcode group used for HA commands/events */
#define gOTAImageProgressReport_c           0xDA /* for sending to external app the image progress report */
#define mZtcOtaSupportImageChunkReq_c       0x2F /* for sending a chunk request to host app */
#define mZtcOtaSupportQueryImageReq_c       0xC2 /* for sending a query nex image request */

#define HEADER_LEN_OFFSET                   6
#define SESSION_BUFFER_SIZE                 128
#define BLOCK_PROCESSING_CALLBACK_DELAY     50  /* ms */

#if gZclEnableOTAServer_d
#if defined(PROCESSOR_HCS08)||defined(PROCESSOR_MC13233C)||defined(PROCESSOR_MC13234C)
#define gExtFlash_TotalSize_c               gFlashParams_MaxImageLength_c
#else
#define gExtFlash_TotalSize_c               0x20000  
#endif
#endif

/******************************************************************************
*******************************************************************************
* Private prototypes
*******************************************************************************
******************************************************************************/

#if gZclEnableOTAServer_d
zbStatus_t ZCL_OTAImageNotify(zclZtcOTAImageNotify_t* pZtcImageNotifyParams);
zbStatus_t ZCL_OTANextImageResponse(zclZtcOTANextImageResponse_t* pZtcNextImageResponseParams);
zbStatus_t ZCL_OTABlockResponse(zclZtcOTABlockResponse_t *pZtcBlockResponseParams);
zbStatus_t ZCL_OTAUpgradeEndResponse(ztcZclOTAUpgdareEndResponse_t* pZtcOTAUpgdareEndResponse);
zbStatus_t ZCL_OTAClusterServer_NextImageRequest_Handler(zbApsdeDataIndication_t *pIndication);
#if gZclOtaExternalMemorySupported_d
static void OTAServerBlockRequestTmrCallback(uint8_t tmr);
static void OTAServerQueryImageRequestTmrCallback(uint8_t tmr);
static uint8_t ReadAndTestOtaHeader(uint16_t manufacturerCode, uint16_t imageType, uint32_t fileVersion, bool_t testFileVersion);
static uint8_t OTAServerSearchForImageRequest(uint16_t manufacturerCode, uint16_t imageType, uint32_t fileVersion, bool_t testFileVersion);
#endif
#endif

#if gZclEnableOTAClient_d
zbStatus_t ZCL_OTAImageRequest(zclZtcOTANextImageRequest_t* pZtcNextImageRequestParams);
zbStatus_t ZCL_OTABlockRequest(zclZtcOTABlockRequest_t *pZtcBlockRequestParams);
zbStatus_t ZCL_OTAUpgradeEndRequest(zclZtcOTAUpgradeEndRequest_t *pZtcUpgradeEndParams);
void OTAClusterClientAbortSession(void);
zbStatus_t OTAClusterClientStartSession(uint32_t fileLength, uint32_t fileVersion);
zbStatus_t OTAClusterClientRunImageProcessStateMachine();
zbStatus_t OTAClusterClientProcessBlock(uint8_t *pImageBlock, uint8_t blockLength);
static void OTAClusterClientProcessBlockTimerCallback(uint8_t tmr);
static void OTAClientRetransmitLastBlockTmrCallback(uint8_t tmr);
static void OTAClusterCPUResetCallback(uint8_t tmr);
static void OTAClusterDelayedUpgradeCallback(uint8_t tmr);
static void OTAClusterClientNextImageReqTimerCallback(uint8_t tmr);
static void OTAClusterClientMatchDescReqTimerCallback(uint8_t tmr);
#endif

/******************************************************************************
*******************************************************************************
* Private type definitions
*******************************************************************************
******************************************************************************/

/******************************************************************************
*******************************************************************************
* Public memory definitions
*******************************************************************************
******************************************************************************/

/******************************************************************************
*******************************************************************************
* Private memory declarations
*******************************************************************************
******************************************************************************/

/******************************
  OTA Cluster Data
  See ZCL Specification Section 6.3
*******************************/

zclOTAAttrsRAM_t gOTAAttrs = {
  {0xFF, 0xFF, 0xFF, 0xFF,0xFF, 0xFF, 0xFF, 0xFF},   // server IEEE address 
  gOTAUpgradeStatusNormal_c   // status
#if gZclClusterOptionals_d
  ,0,                         // file offset
  Native2OTA32(OTA_FILE_VERSION_CURRENT),   // current version
  Native2OTA32(OTA_FILE_VERSION_CURRENT)    // downloaded version
#if gZigbeeProIncluded_d
  ,gZclStackVer_ZigBeePro_c,  // stack version  
  gZclStackVer_ZigBeePro_c,   // downloaded stack version 
#else
  ,gZclStackVer_ZigBee2007_c, // stack version 
  gZclStackVer_ZigBee2007_c,  // downloaded stack version
#endif //gZigbeeProIncluded_d
  Native2OTA16(OTA_MANUFACTURER_CODE_FSL),  // manufacturer Code
#endif //gZclClusterOptionals_d
};

const zclAttrDef_t gaZclOTAClusterAttrDef[] = {
  { gZclAttrOTA_UpgradeServerIdId_c,              gZclDataTypeIeeeAddr_c,  gZclAttrFlagsCommon_c|gZclAttrFlagsRdOnly_c,sizeof(IEEEaddress_t), &gOTAAttrs.UpgradeServerId},//(void *)MbrSizeof(zclOTAAttrsRAM_t, UpgradeServerId)},
  { gZclAttrOTA_ImageUpgradeStatusId_c,           gZclDataTypeEnum8_c, gZclAttrFlagsCommon_c|gZclAttrFlagsRdOnly_c, sizeof(uint8_t), &gOTAAttrs.ImageUpgradeStatus},//(void *)MbrSizeof(zclOTAAttrsRAM_t, ImageUpgradeStatus)},
#if gZclClusterOptionals_d
  { gZclAttrOTA_FileOffsetId_c,                   gZclDataTypeUint32_c,gZclAttrFlagsCommon_c|gZclAttrFlagsRdOnly_c,sizeof(uint32_t), &gOTAAttrs.FileOffset},
  { gZclAttrOTA_CurrentFileVersionId_c,           gZclDataTypeUint32_c,gZclAttrFlagsCommon_c|gZclAttrFlagsRdOnly_c,sizeof(uint32_t), &gOTAAttrs.CurrentFileVersion},
  { gZclAttrOTA_CurrentZigBeeStackVersionId_c,    gZclDataTypeUint16_c,gZclAttrFlagsCommon_c|gZclAttrFlagsRdOnly_c,sizeof(uint16_t), &gOTAAttrs.CurrentZigBeeStackVersion},
  { gZclAttrOTA_DownloadedFileVersionId_c,        gZclDataTypeUint32_c,gZclAttrFlagsCommon_c|gZclAttrFlagsRdOnly_c,sizeof(uint32_t), &gOTAAttrs.DownloadedFileVersion},
  { gZclAttrOTA_DownloadedZigBeeStackVersionId_c, gZclDataTypeUint16_c,gZclAttrFlagsCommon_c|gZclAttrFlagsRdOnly_c,sizeof(uint16_t), &gOTAAttrs.DownloadedZigBeeStackVersion},
  { gZclAttrOTA_ManufacturerId_c, gZclDataTypeUint16_c,gZclAttrFlagsCommon_c|gZclAttrFlagsRdOnly_c,sizeof(uint16_t), &gOTAAttrs.ManufacturerId},
#endif
};

const zclAttrDefList_t gZclOTAClusterAttrDefList = {
  NumberOfElements(gaZclOTAClusterAttrDef),
  gaZclOTAClusterAttrDef
};


//  Size optimization. The values represent
//  should represent the size of the Image Notify Command
//  based on the payload type.
const uint8_t imgNotifySizeArray[gPayloadMax_c] = {2, 4, 6, 10};
uint8_t gZclTransactionOtaId = 0x00;

#if gZclEnableOTAClient_d

static uint8_t mMultipleUpgradeImage = 0;
const uint8_t OtaUpgrade_OutputClusterList[2] = {
    gaZclClusterOTA_c,  /* Ota Upgrade Cluster  */ 
};
bool_t mSendOtaMatchDescriptorRequest = FALSE;
bool_t mSendOTAIEEEAddrRequest = FALSE;
zbSimpleDescriptor_t otaCluster_simpleDescriptor = {
									 8, /* Endpoint number */
									 0x0,0x0, /* Application profile ID */
									 0x0, 0x0, /* Application device ID */
									 0, /* Application version ID */
									 1, /* Number of input clusters */
									 (uint8_t *)OtaUpgrade_OutputClusterList, /* Input cluster list */
									 0, /* Number of output clusters */
									 NULL, /* Output cluster list */
									};  
zclOTAClientParams_t  clientParams = {
                                      OTA_MANUFACTURER_CODE_FSL,
                                      OTA_IMAGE_TYPE_CURRENT,
                                      OTA_FILE_VERSION_CURRENT,
                                      OTA_FILE_VERSION_DL_DEFAULT,
                                      OTA_HW_VERSION_CURRENT,
                                      OTA_CLIENT_MAX_DATA_SIZE
                                     };


zclOTAClientSession_t clientSession = { 
                                       FALSE,      /* sessionStarted */
                                       0x00000000, /* fileLength   */
                                       0x00000000, /* currentOffset  */
                                       0x00000000, /* downloadingFileVersion */
                                       0x00,       /* bytesNeedForNextState */
                                       NULL,       /* pStateBuffer */
                                       0x00,       /* stateBufferIndex */
                                       init_c,      /* state */
                                       0  
                                      };
zclOTABlockCallbackState_t* gpBlockCallbackState = NULL;
tmrTimerID_t gBlockCallbackTimer = gTmrInvalidTimerID_c;
tmrTimerID_t gRetransmitLastBlockTimer = gTmrInvalidTimerID_c; 
tmrTimerID_t gNextImageReqTimer = gTmrInvalidTimerID_c;
#endif

#if gZclEnableOTAServer_d

#define gMaxNumberOfOTAImage                    2   /* max number of the ota images supported*/
static ztcServerUtility_t mServerUtility = {
#if gZclOtaExternalMemorySupported_d		
              gUseExternalMemoryForOtaUpdate_c, /* operation mode  */
              gOtaServerLoadingImageComplete_c, /* imageLoadingState */
              0,                                /* currentAddress */    	  
#else
              gDoNotUseExternalMemoryForOtaUpdate_c, /* operation mode */
#endif                                  
              FALSE                            /* blockResponseInProgress */ 
              };

#if gZclOtaExternalMemorySupported_d     
static ztcOtaServerImageListParam_t listOfImage[gMaxNumberOfOTAImage];
static uint8_t mIndexOfCurrentImage = 0;
#endif
static zclZtcOTABlockResponse_t  *pZtcBlockResponse;  
static zclOTAServerInitParams_t mOtaServerInitParams = {
              OTA_CLIENT_MAX_RANDOM_JITTER,  /* querryJitter */
              0,                             /* currentTime */
              40,                            /* upgradeRequestTime */
              0                              /* upgradeTime */
              };
tmrTimerID_t gRequestBlockTimer = gTmrInvalidTimerID_c; 
#endif
/******************************************************************************
*******************************************************************************
* Public functions
*******************************************************************************
******************************************************************************/


/*****************************************************************************
******************************************************************************
*
* ZCL OTA SERVER FUNCTIONS
*
******************************************************************************
*****************************************************************************/

#if gZclEnableOTAServer_d
#include "Bootloader.h"

/*****************************************************************************
* ZCL_OTAClusterServer Function
*
* Incoming OTA cluster frame processing loop for cluster server side
*****************************************************************************/
zbStatus_t ZCL_OTAClusterServer
  (
  zbApsdeDataIndication_t *pIndication, 
  afDeviceDef_t *pDevice
  )
{
    zclFrame_t *pFrame;
    zclOTABlockRequest_t          blockRequest;
    zclOTAUpgradeEndRequest_t     upgradeEndRequest;
    ztcZclOTAUpgdareEndResponse_t upgradeEndResponse;
    zbStatus_t status = gZclSuccess_c;    
     //Parameter not used, avoid compiler warning
    (void)pDevice;
    
    //ZCL frame
    pFrame = (zclFrame_t*)pIndication->pAsdu;
    gZclTransactionOtaId = pIndication->pAsdu[1];
    /* Handle the commands */
    switch(pFrame->command)
    {
      case gZclCmdOTA_QueryNextImageRequest_c:  
        status = ZCL_OTAClusterServer_NextImageRequest_Handler(pIndication);   
        break;
  
      case gZclCmdOTA_ImageBlockRequest_c:
        if(mServerUtility.blockResponseInProgress == FALSE){
          mServerUtility.blockResponseInProgress = TRUE;
          FLib_MemCpy(&blockRequest, (pFrame+1), sizeof(zclOTABlockRequest_t));
    	  pZtcBlockResponse = MSG_Alloc(sizeof(zclZtcOTABlockResponse_t)+ blockRequest.maxDataSize); 
    	  if(!pZtcBlockResponse)
    	  {
    		 mServerUtility.blockResponseInProgress = FALSE; 
    		 return gZclNoMem_c;	
    	  }
    	  Copy2Bytes(&pZtcBlockResponse->aNwkAddr, &pIndication->aSrcAddr);
    	  pZtcBlockResponse->endPoint = pIndication->srcEndPoint;
          if(mServerUtility.operationMode == gUseExternalMemoryForOtaUpdate_c)
          {      
#if gZclOtaExternalMemorySupported_d           
          if(mServerUtility.imageLoadingState == gOtaServerLoadingImageStart_c)
            pZtcBlockResponse->zclOTABlockResponse.status = gZclOTAAbort_c;
          else
			pZtcBlockResponse->zclOTABlockResponse.status = (mServerUtility.imageLoadingState == gOtaServerLoadingImageProcess_c)?gZclOTAWaitForData_c:gZclOTASuccess_c;   
 #endif
          }
          else
            pZtcBlockResponse->zclOTABlockResponse.status = gZclOTASuccess_c;
          pZtcBlockResponse->zclOTABlockResponse.msgData.success.fileOffset = blockRequest.fileOffset;  		
    	  pZtcBlockResponse->zclOTABlockResponse.msgData.success.dataSize = blockRequest.maxDataSize;
          pZtcBlockResponse->zclOTABlockResponse.msgData.success.manufacturerCode = blockRequest.manufacturerCode;
          pZtcBlockResponse->zclOTABlockResponse.msgData.success.imageType = blockRequest.imageType;
          pZtcBlockResponse->zclOTABlockResponse.msgData.success.fileVersion = blockRequest.fileVersion;
          if(mServerUtility.operationMode == gUseExternalMemoryForOtaUpdate_c)
          {
#if gZclOtaExternalMemorySupported_d             
    	    status = ZCL_OTABlockResponse(pZtcBlockResponse);
            mServerUtility.blockResponseInProgress = FALSE;
            MSG_Free(pZtcBlockResponse); 
#endif  //gZclOtaExternalMemorySupported_d               
          }
          else
          {
            ztcOtaServerChunkRequest_t chunkRequest;
            Copy2Bytes(&chunkRequest.deviceID, &pIndication->aSrcAddr);
            chunkRequest.currentOffset = blockRequest.fileOffset; 
            /* Host App control the remaining size of image */
            chunkRequest.blockSize = blockRequest.maxDataSize; 
#ifndef gHostApp_d  
            ZTCQueue_QueueToTestClient((const uint8_t*)&chunkRequest, gZtcReqOpcodeGroup_c, mZtcOtaSupportImageChunkReq_c, sizeof(ztcOtaServerChunkRequest_t));
#else
            ZTCQueue_QueueToTestClient(gpHostAppUart, (const uint8_t*)&chunkRequest, gZtcReqOpcodeGroup_c, mZtcOtaSupportImageChunkReq_c, sizeof(ztcOtaServerChunkRequest_t));
#endif        
            status = gZclSuccess_c; 
          }  
        }
        else
        {
          zclZtcOTABlockResponse_t  *pZtcBlockResponseWaitForData;
          if((pZtcBlockResponse->aNwkAddr[0] == pIndication->aSrcAddr[0])&&
             (pZtcBlockResponse->aNwkAddr[1] == pIndication->aSrcAddr[1]))
               return gZclSuccess_c;
          pZtcBlockResponseWaitForData = MSG_Alloc(sizeof(zclZtcOTABlockResponse_t)); 
    	  if(!pZtcBlockResponseWaitForData)
    	  {
    		 mServerUtility.blockResponseInProgress = FALSE; 
    		 return gZclNoMem_c;	
    	  }
          Copy2Bytes(&pZtcBlockResponseWaitForData->aNwkAddr, &pIndication->aSrcAddr);
    	  pZtcBlockResponseWaitForData->endPoint = pIndication->srcEndPoint;
          pZtcBlockResponseWaitForData->zclOTABlockResponse.status = gZclOTAWaitForData_c;
          status = ZCL_OTABlockResponse(pZtcBlockResponseWaitForData);
          MSG_Free(pZtcBlockResponseWaitForData);
        }
        break;
    case gZclCmdOTA_UpgradeEndRequest_c:
        FLib_MemCpy(&upgradeEndRequest, (pFrame+1), sizeof(zclOTAUpgradeEndRequest_t));
        if(upgradeEndRequest.status == gZclOTASuccess_c)
        {          
        	Copy2Bytes(&upgradeEndResponse.aNwkAddr, &pIndication->aSrcAddr);
        	upgradeEndResponse.endPoint = pIndication->srcEndPoint;
        	upgradeEndResponse.zclOTAUpgradeEndResponse.manufacturerCode = upgradeEndRequest.manufacturerCode;
        	upgradeEndResponse.zclOTAUpgradeEndResponse.imageType = upgradeEndRequest.imageType;
        	upgradeEndResponse.zclOTAUpgradeEndResponse.fileVersion = upgradeEndRequest.fileVersion;
        	upgradeEndResponse.zclOTAUpgradeEndResponse.currentTime = Native2OTA32(mOtaServerInitParams.currentTime);
        	upgradeEndResponse.zclOTAUpgradeEndResponse.upgradeTime = Native2OTA32(mOtaServerInitParams.upgradeTime);                          
        	status = ZCL_OTAUpgradeEndResponse(&upgradeEndResponse);          
        }
        else{
        	status = gZclSuccessDefaultRsp_c;
        }
        if(mServerUtility.operationMode == gUseExternalMemoryForOtaUpdate_c)
        {   
#if (gZclEnableOTAProgressReportToExternalApp_d == TRUE)    
            zclZtcImageOtaProgressReport_t ztcOtaProgressReportInf;
            /*send a report message with status TRUE or FALSE: imageLength field 
            *(TRUE - image successfully transmitted to client , FALSE - otherwise)*/
            ztcOtaProgressReportInf.currentOffset = 0xFFFFFFFF;
            if(status == gZclSuccess_c)
              ztcOtaProgressReportInf.imageLength = Native2OTA32(TRUE);
            else
              ztcOtaProgressReportInf.imageLength = Native2OTA32(FALSE);
            Copy2Bytes(&ztcOtaProgressReportInf.deviceAddr,&pIndication->aSrcAddr); 
#ifndef gHostApp_d  
            ZTCQueue_QueueToTestClient((const uint8_t*)&ztcOtaProgressReportInf, gHaZtcOpCodeGroup_c, gOTAImageProgressReport_c, sizeof(zclZtcImageOtaProgressReport_t));
#else
            ZTCQueue_QueueToTestClient(gpHostAppUart, (const uint8_t*)&ztcOtaProgressReportInf, gHaZtcOpCodeGroup_c, gOTAImageProgressReport_c, sizeof(zclZtcImageOtaProgressReport_t));
#endif 
#endif    
        }
        break;
        // command not supported on this cluster
    default:
        return gZclUnsupportedClusterCommand_c;
    }
    return status;
}

/******************************************************************************
* Request to send the image notify command
*
* Usually called by the ZTC/host
******************************************************************************/

zbStatus_t ZCL_OTAImageNotify(zclZtcOTAImageNotify_t* pZtcImageNotifyParams)
{
  afToApsdeMessage_t *pMsg;
  afAddrInfo_t addrInfo = {gZbAddrMode16Bit_c, {0x00, 0x00}, 0, {gaZclClusterOTA_c}, 0, gApsTxOptionNone_c, 1};
  zclFrameControl_t frameControl;
  uint8_t len;  
  
  //Create the destination address
  Copy2Bytes(&addrInfo.dstAddr,&pZtcImageNotifyParams->aNwkAddr);
  addrInfo.dstEndPoint = pZtcImageNotifyParams->endPoint;
  addrInfo.srcEndPoint = pZtcImageNotifyParams->endPoint;
  addrInfo.radiusCounter = afDefaultRadius_c;
  
  //Initialize the lenght of the command based on the payload type.
  if(pZtcImageNotifyParams->zclOTAImageNotify.payloadType < gPayloadMax_c)
  {
    len = imgNotifySizeArray[pZtcImageNotifyParams->zclOTAImageNotify.payloadType];
  }
  else
  {
    return gZclFailure_c;
  }
  if(!IsValidNwkUnicastAddr(addrInfo.dstAddr.aNwkAddr))
  {
    frameControl = gZclFrameControl_FrameTypeSpecific | gZclFrameControl_DirectionRsp | gZclFrameControl_DisableDefaultRsp;
  }
  else
  {
    frameControl = gZclFrameControl_FrameTypeSpecific | gZclFrameControl_DirectionRsp;
  }

  pMsg = ZCL_CreateFrame( &addrInfo, 
                          gZclCmdOTA_ImageNotify_c,
                          frameControl, 
                          &gZclTransactionId, 
                          &len,
                          &pZtcImageNotifyParams->zclOTAImageNotify);
  gZclTransactionId++;
  if(!pMsg)
    return gZclNoMem_c;
 return ZCL_DataRequestNoCopy(&addrInfo, len, pMsg);  
}


/******************************************************************************
* ZCL_OTAClusterServer_NextImageRequest_Handler
*
* Sends back a next image response 
******************************************************************************/  

zbStatus_t ZCL_OTAClusterServer_NextImageRequest_Handler
(
  zbApsdeDataIndication_t *pIndication  
)
{
   zclFrame_t *pFrame;
   zbStatus_t status = gZclSuccess_c;  
   zclOTANextImageRequest_t   ztcNextImageReq;
   /*ZCL frame*/
   pFrame = (zclFrame_t*)pIndication->pAsdu;
   FLib_MemCpy(&ztcNextImageReq, (pFrame+1), sizeof(zclOTANextImageRequest_t));
   ztcNextImageReq.manufacturerCode = OTA2Native16(ztcNextImageReq.manufacturerCode);
   ztcNextImageReq.imageType = OTA2Native16(ztcNextImageReq.imageType);
   ztcNextImageReq.fileVersion = OTA2Native32(ztcNextImageReq.fileVersion);
   if(mServerUtility.operationMode == gDoNotUseExternalMemoryForOtaUpdate_c)
   {
      ztcOtaServerQueryImageRequest_t queryRequest;

      Copy2Bytes(&queryRequest.deviceID, &pIndication->aSrcAddr);
      queryRequest.manufacturerCode = Native2OTA16(ztcNextImageReq.manufacturerCode);
      queryRequest.imageType = Native2OTA16(ztcNextImageReq.imageType);
      queryRequest.fileVersion = Native2OTA32(ztcNextImageReq.fileVersion);
      /* wait the host application to confirm (see OTAServerQueryImageRsp) */
#ifndef gHostApp_d  
      ZTCQueue_QueueToTestClient((const uint8_t*)&queryRequest, gZtcReqOpcodeGroup_c, mZtcOtaSupportQueryImageReq_c, sizeof(ztcOtaServerQueryImageRequest_t));
#else
      ZTCQueue_QueueToTestClient(gpHostAppUart, (const uint8_t*)&queryRequest, gZtcReqOpcodeGroup_c, mZtcOtaSupportQueryImageReq_c, sizeof(ztcOtaServerQueryImageRequest_t));
#endif 
   }
   else
   {
      zclZtcOTANextImageResponse_t  ztcNextImageRsp;
#if gZclOtaExternalMemorySupported_d       
      uint8_t index = OTAServerSearchForImageRequest(ztcNextImageReq.manufacturerCode,ztcNextImageReq.imageType, ztcNextImageReq.fileVersion, FALSE); 
      if(index == gMaxNumberOfOTAImage)
         ztcNextImageRsp.zclOTANextImageResponse.status = gZclOTANoImageAvailable_c;
      else
      {
        ztcNextImageRsp.zclOTANextImageResponse.status = gZclSuccess_c;
        ztcNextImageRsp.zclOTANextImageResponse.fileVersion = Native2OTA32(listOfImage[index].fileVersion);
        ztcNextImageRsp.zclOTANextImageResponse.imageSize = Native2OTA32(listOfImage[index].totalImageSize);  
      }
#else
      ztcNextImageRsp.zclOTANextImageResponse.status = gZclOTANoImageAvailable_c;      
#endif       
      Copy2Bytes(&ztcNextImageRsp.aNwkAddr,&pIndication->aSrcAddr);
      ztcNextImageRsp.endPoint = pIndication->srcEndPoint;
      ztcNextImageRsp.zclOTANextImageResponse.manufacturerCode = Native2OTA16(ztcNextImageReq.manufacturerCode);
      ztcNextImageRsp.zclOTANextImageResponse.imageType = Native2OTA16(ztcNextImageReq.imageType);
      status = ZCL_OTANextImageResponse(&ztcNextImageRsp);     
   }
   return status;
}

/******************************************************************************
* Request to send the image notify command
*
* Incoming OTA cluster frame processing loop for cluster client side
******************************************************************************/
zbStatus_t ZCL_OTANextImageResponse(zclZtcOTANextImageResponse_t* pZtcNextImageResponseParams)
{
  afToApsdeMessage_t *pMsg;
  afAddrInfo_t addrInfo = {gZbAddrMode16Bit_c, {0x00, 0x00}, 0, {gaZclClusterOTA_c}, 0, gApsTxOptionAckTx_c, 1};
  uint8_t len = 0;  
  
  switch(pZtcNextImageResponseParams->zclOTANextImageResponse.status)
    {
  	  case gZclOTASuccess_c:
  	    len = sizeof(zclOTANextImageResponse_t);
  	    break;
  	  case gZclOTANoImageAvailable_c:
  	  case gZclOTANotAuthorized_c:	  
  	    len = sizeof(zclOTAStatus_t);
  	    break;
  	  default:
  	    //Invalid status
  	    return gZclMalformedCommand_c;
    }
  
  //Create the destination address
  Copy2Bytes(&addrInfo.dstAddr,&pZtcNextImageResponseParams->aNwkAddr);
  addrInfo.dstEndPoint = pZtcNextImageResponseParams->endPoint;
  addrInfo.srcEndPoint = pZtcNextImageResponseParams->endPoint;
  addrInfo.radiusCounter = afDefaultRadius_c;
  
  pMsg = ZCL_CreateFrame( &addrInfo, 
                          gZclCmdOTA_QueryNextImageResponse_c,
                          gZclFrameControl_FrameTypeSpecific | gZclFrameControl_DirectionRsp | gZclFrameControl_DisableDefaultRsp, 
                          &(gZclTransactionOtaId), 
                          &len,
                          &pZtcNextImageResponseParams->zclOTANextImageResponse);
  if(!pMsg)
    return gZclNoMem_c;
  return ZCL_DataRequestNoCopy(&addrInfo, len, pMsg);  
}

/******************************************************************************
* ZCL_OTABlockResponse
*
* Send a data block to the client
******************************************************************************/
zbStatus_t ZCL_OTABlockResponse(zclZtcOTABlockResponse_t *pZtcBlockResponseParams)
{
 uint8_t len = 0; 
 afToApsdeMessage_t *pMsg;
 afAddrInfo_t addrInfo = {gZbAddrMode16Bit_c, {0x00, 0x00}, 0, {gaZclClusterOTA_c}, 0, gApsTxOptionAckTx_c, 1};
 
  if(mServerUtility.operationMode == gUseExternalMemoryForOtaUpdate_c)
  {
#if(gZclOtaExternalMemorySupported_d)
    uint32_t addr;
    uint32_t offsetImage = 0;

    uint16_t manufacturerCode = OTA2Native16(pZtcBlockResponseParams->zclOTABlockResponse.msgData.success.manufacturerCode);
    uint16_t imageType = OTA2Native16(pZtcBlockResponseParams->zclOTABlockResponse.msgData.success.imageType);
    uint32_t fileVersion = OTA2Native32(pZtcBlockResponseParams->zclOTABlockResponse.msgData.success.fileVersion);
    
    if(pZtcBlockResponseParams->zclOTABlockResponse.status == gZclOTASuccess_c)
    {
      uint8_t index = OTAServerSearchForImageRequest(manufacturerCode,imageType,fileVersion, TRUE); 
      if(index == gMaxNumberOfOTAImage)
         //pZtcBlockResponseParams->zclOTABlockResponse.status = gZclOTAAbort_c;
    	 return gZclOTANoImageAvailable_c; 
      else
      {
        //FLib_MemCpy(&offsetImage,&pZtcBlockResponseParams->zclOTABlockResponse.msgData.success.fileOffset, sizeof(offsetImage));
        offsetImage = OTA2Native32(pZtcBlockResponseParams->zclOTABlockResponse.msgData.success.fileOffset);
        len = pZtcBlockResponseParams->zclOTABlockResponse.msgData.success.dataSize;
      
        if(listOfImage[index].totalImageSize - offsetImage < len)
			len =listOfImage[index].totalImageSize - offsetImage;
        addr = offsetImage + listOfImage[index].addressOfCurrentImage;
        if (OTA_ReadExternalMemory(&pZtcBlockResponseParams->zclOTABlockResponse.msgData.success.data[0], len, addr) != gOtaSucess_c )
	 	     pZtcBlockResponseParams->zclOTABlockResponse.status = gZclOTAAbort_c; 
        pZtcBlockResponseParams->zclOTABlockResponse.msgData.success.dataSize = len;	

#if (gZclEnableOTAProgressReportToExternalApp_d == TRUE)    
        {
          zclZtcImageOtaProgressReport_t ztcOtaProgressReportInf;
          if((offsetImage == 0)||((offsetImage+len)%(len*100) == 0)||(offsetImage+len == listOfImage[index].totalImageSize))
          {
            ztcOtaProgressReportInf.currentOffset = Native2OTA32(offsetImage+len);
            ztcOtaProgressReportInf.imageLength =  Native2OTA32(listOfImage[index].totalImageSize);
            Copy2Bytes(&ztcOtaProgressReportInf.deviceAddr,&pZtcBlockResponseParams->aNwkAddr); 
#ifndef gHostApp_d  
            ZTCQueue_QueueToTestClient((const uint8_t*)&ztcOtaProgressReportInf, gHaZtcOpCodeGroup_c, gOTAImageProgressReport_c, sizeof(zclZtcImageOtaProgressReport_t));
#else
            ZTCQueue_QueueToTestClient(gpHostAppUart, (const uint8_t*)&ztcOtaProgressReportInf, gHaZtcOpCodeGroup_c, gOTAImageProgressReport_c, sizeof(zclZtcImageOtaProgressReport_t));
#endif 
          }
        }
#endif  //gZclEnableOTAProgressReportToExternApp_d      
      }
    }
#endif //gZclOtaExternalMemorySupported_d
  }
  //The lenght of the frame is dependant on the status
  switch(pZtcBlockResponseParams->zclOTABlockResponse.status)
  {
	  case gZclOTASuccess_c:
	    len = sizeof(zclOTABlockResponse_t) + pZtcBlockResponseParams->zclOTABlockResponse.msgData.success.dataSize - 1;
	    break;
	  case gZclOTAAbort_c:
	    len = sizeof(zclOTAStatus_t);
	    break;
	  case gZclOTAWaitForData_c:
            {
              pZtcBlockResponseParams->zclOTABlockResponse.msgData.wait.currentTime = Native2OTA32(mOtaServerInitParams.currentTime); 
              pZtcBlockResponseParams->zclOTABlockResponse.msgData.wait.requestTime = Native2OTA32(mOtaServerInitParams.upgradeRequestTime); // seconds
	      len = sizeof(zclOTABlockResponseStatusWait_t) + 1;
            }
	    break;
	  default:
	    //Invalid status
	    return gZclMalformedCommand_c;
  }
  //Create the destination address
  Copy2Bytes(&addrInfo.dstAddr,&pZtcBlockResponseParams->aNwkAddr);
  addrInfo.dstEndPoint = pZtcBlockResponseParams->endPoint;
  addrInfo.srcEndPoint = pZtcBlockResponseParams->endPoint;
  addrInfo.radiusCounter = afDefaultRadius_c;
  
  pMsg = ZCL_CreateFrame( &addrInfo, 
	                      gZclCmdOTA_ImageBlockResponse_c,
	                      gZclFrameControl_FrameTypeSpecific | gZclFrameControl_DirectionRsp | gZclFrameControl_DisableDefaultRsp, 
	                      &(gZclTransactionOtaId), 
	                      &len,
	                      &pZtcBlockResponseParams->zclOTABlockResponse);
  if(!pMsg)
	   return gZclNoMem_c;
  return ZCL_DataRequestNoCopy(&addrInfo, len, pMsg);    
}

/******************************************************************************
* Request to send upgrade end response command
*
* Send the response to client's upgrade end request
******************************************************************************/
zbStatus_t ZCL_OTAUpgradeEndResponse(ztcZclOTAUpgdareEndResponse_t* pZtcOTAUpgdareEndResponse)
{
  afToApsdeMessage_t *pMsg;
  afAddrInfo_t addrInfo = {gZbAddrMode16Bit_c, {0x00, 0x00}, 0, {gaZclClusterOTA_c}, 0, gApsTxOptionAckTx_c, 1};
  uint8_t len = sizeof(zclOTAUpgradeEndResponse_t);  
  
  //Create the destination address
  Copy2Bytes(&addrInfo.dstAddr,&pZtcOTAUpgdareEndResponse->aNwkAddr);
  addrInfo.dstEndPoint = pZtcOTAUpgdareEndResponse->endPoint;
  addrInfo.srcEndPoint = pZtcOTAUpgdareEndResponse->endPoint;
  addrInfo.radiusCounter = afDefaultRadius_c;
  
  pMsg = ZCL_CreateFrame( &addrInfo, 
                          gZclCmdOTA_UpgradeEndResponse_c,
                          gZclFrameControl_FrameTypeSpecific | gZclFrameControl_DirectionRsp | gZclFrameControl_DisableDefaultRsp, 
                          &(gZclTransactionOtaId), 
                          &len,
                          &pZtcOTAUpgdareEndResponse->zclOTAUpgradeEndResponse);
  if(!pMsg)
    return gZclNoMem_c;
  return ZCL_DataRequestNoCopy(&addrInfo, len, pMsg);  
}

zbStatus_t OTAServerStartImageProcess(uint8_t operationMode)
{
   mServerUtility.operationMode = operationMode;
   if(mServerUtility.operationMode == gUseExternalMemoryForOtaUpdate_c)
   {
#if (gZclOtaExternalMemorySupported_d)  
      if(mServerUtility.imageLoadingState == gOtaServerLoadingImageProcess_c)
       return gOtaInvalidParam_c;
      mServerUtility.currentAddress = 0x00;
      mIndexOfCurrentImage = 0;
      /* Init external memory */
      OTA_InitExternalMemory();
      /* Erase external memory */
      if(gOtaSucess_c != OTA_EraseExternalMemory())
        return gOtaExternalFlashError_c; 
      /*send a query request */
      gRequestBlockTimer = TMR_AllocateTimer(); 
      if(gRequestBlockTimer == gTmrInvalidTimerID_c)
        return gZclNoMem_c;
      TMR_StartTimer(gRequestBlockTimer, gTmrSingleShotTimer_c,
                         BLOCK_PROCESSING_CALLBACK_DELAY, OTAServerQueryImageRequestTmrCallback);
      return gOtaSucess_c;      
#else      
      return gOtaInvalidParam_c; 
#endif      
   }   
   return gOtaSucess_c;
}

zbStatus_t OTAServerQueryImageRsp(uint8_t *queryImageRsp)
{
 ztcOtaServerQueryImageRsp_t queryResponse;
    
   FLib_MemCpy(&queryResponse, &queryImageRsp[0], sizeof(ztcOtaServerQueryImageRsp_t));
   if(queryResponse.imageStatus != gOtaSucess_c) 
     return gZclFailure_c;
     
   if(mServerUtility.operationMode == gUseExternalMemoryForOtaUpdate_c)
   {
#if (gZclOtaExternalMemorySupported_d)   
      if(mIndexOfCurrentImage >= gMaxNumberOfOTAImage) 
        return gZclFailure_c;
      queryResponse.totalImageSize = OTA2Native32(queryResponse.totalImageSize);
      /* test the posibility to store the image */
      if ((queryResponse.totalImageSize == 0) || 
            (queryResponse.totalImageSize + mServerUtility.currentAddress > gExtFlash_TotalSize_c)) 
        return gZclFailure_c;
      /* store server information */
      listOfImage[mIndexOfCurrentImage].imageType = OTA2Native16(queryResponse.imageType);
      listOfImage[mIndexOfCurrentImage].totalImageSize = queryResponse.totalImageSize;
      listOfImage[mIndexOfCurrentImage].manufacturerCode = OTA2Native16(queryResponse.manufacturerCode);
      listOfImage[mIndexOfCurrentImage].fileVersion = OTA2Native32(queryResponse.fileVersion); 
      listOfImage[mIndexOfCurrentImage].addressOfCurrentImage =  mServerUtility.currentAddress;
      /*send a first block request */
      mServerUtility.imageLoadingState = gOtaServerLoadingImageStart_c;
      gRequestBlockTimer = TMR_AllocateTimer(); 
      if(gRequestBlockTimer == gTmrInvalidTimerID_c)
        return gZclNoMem_c;
      TMR_StartTimer(gRequestBlockTimer, gTmrSingleShotTimer_c,
                         BLOCK_PROCESSING_CALLBACK_DELAY, OTAServerBlockRequestTmrCallback);
#endif  
   }
   else  //case: gNotUseExternalMemoryForOtaUpdate_c
   {
     //send a query next image response over the air
     zclZtcOTANextImageResponse_t  ztcNextImageRsp;
     Copy2Bytes(&ztcNextImageRsp.aNwkAddr, &queryResponse.deviceID);
#if gNum_EndPoints_c != 0  
     ztcNextImageRsp.endPoint = endPointList[0].pEndpointDesc->pSimpleDesc->endPoint;
#endif  
     ztcNextImageRsp.zclOTANextImageResponse.status = queryResponse.imageStatus;
     ztcNextImageRsp.zclOTANextImageResponse.manufacturerCode = queryResponse.manufacturerCode;
     ztcNextImageRsp.zclOTANextImageResponse.imageType = queryResponse.imageType;
     ztcNextImageRsp.zclOTANextImageResponse.fileVersion = queryResponse.fileVersion;
     ztcNextImageRsp.zclOTANextImageResponse.imageSize = queryResponse.totalImageSize;   
     return ZCL_OTANextImageResponse(&ztcNextImageRsp);
   }  
    return gOtaSucess_c;
}

/****************************************************************************
 * newImageReceived
 *   - is called for sending an image notify command over the air  
 ****************************************************************************/
zbStatus_t newImageReceived(ztcOtaServerImageNotify_t imgNotify)
{
    zclZtcOTAImageNotify_t imageNotify;
    /* send a broadcast or unicast image notify */
    Copy2Bytes(&imageNotify.aNwkAddr, &imgNotify.deviceID);
#if gNum_EndPoints_c != 0  
    imageNotify.endPoint = endPointList[0].pEndpointDesc->pSimpleDesc->endPoint;
#endif    
    /* see 6.10.3- ImageNotify Command - Ota cluster specification (r17)*/
    if(IsValidNwkUnicastAddr(imageNotify.aNwkAddr) == FALSE)
      imageNotify.zclOTAImageNotify.payloadType = gJitter_c;
    else
      imageNotify.zclOTAImageNotify.payloadType = gJitterMfgImageFile_c;
    imageNotify.zclOTAImageNotify.queryJitter = OTA_CLIENT_MAX_RANDOM_JITTER;
    /*inform clients:  new image(s) are available*/  
    imageNotify.zclOTAImageNotify.manufacturerCode = imgNotify.manufacturerCode;
    imageNotify.zclOTAImageNotify.imageType = imgNotify.imageType;   
    imageNotify.zclOTAImageNotify.fileVersion = imgNotify.fileVersion; 
    return ZCL_OTAImageNotify(&imageNotify);      
}


/******************************************************************************
* OTAServerImageNotify
*
*  - if operationMode = gUseExternalMemoryForOtaUpdate_c  - first tests whether the image is already there, 
                            if not, download it and send the ImageNotify command over the air;
*                     = gNotUseExternalMemoryForOtaUpdate_c  - send the ImageNotify command over the air;                  
******************************************************************************/
zbStatus_t OTAServerImageNotify(uint8_t *imgNotify)
{
  ztcOtaServerImageNotify_t imageNotify;  
  FLib_MemCpy(&imageNotify, &imgNotify[0], sizeof(ztcOtaServerImageNotify_t));
  imageNotify.manufacturerCode = OTA2Native16(imageNotify.manufacturerCode);
  imageNotify.imageType = OTA2Native16(imageNotify.imageType);
  imageNotify.fileVersion = OTA2Native32(imageNotify.fileVersion);
  if(mServerUtility.operationMode == gUseExternalMemoryForOtaUpdate_c)
  {
#if (gZclOtaExternalMemorySupported_d)      
    uint8_t index = OTAServerSearchForImageRequest(imageNotify.manufacturerCode, imageNotify.imageType, imageNotify.fileVersion, TRUE);
    if(index == gMaxNumberOfOTAImage)
    {
      /*start process to download it*/
      gRequestBlockTimer = TMR_AllocateTimer();
      if(gRequestBlockTimer == gTmrInvalidTimerID_c)
        return gZclNoMem_c;
      TMR_StartTimer(gRequestBlockTimer, gTmrSingleShotTimer_c,
                         BLOCK_PROCESSING_CALLBACK_DELAY, OTAServerQueryImageRequestTmrCallback);
      return gZclSuccess_c;
    }
    else
      return gZclFailure_c;
#endif    
  }
  return newImageReceived(imageNotify);
}

#if (gZclOtaExternalMemorySupported_d) 
/******************************************************************************
* OTAServerSearchForImageRequest  
*
* 	- return index of required image in the image list or if error return gMaxNumberOfOTAImage
******************************************************************************/
static uint8_t OTAServerSearchForImageRequest(uint16_t manufacturerCode, uint16_t imageType, uint32_t fileVersion, bool_t testFileVersion)
{
  uint8_t i;
   /*test if the image exist in the list*/
   for(i=0;i<gMaxNumberOfOTAImage;i++)
     if((imageType == listOfImage[i].imageType)&&
    	 (manufacturerCode == listOfImage[i].manufacturerCode))
     {
    	 if(testFileVersion == TRUE)
    	 {
    		 if(fileVersion == listOfImage[i].fileVersion)
    			 return i;
    	 }
    	 else
    		 return i;
     }
      
   return ReadAndTestOtaHeader(manufacturerCode, imageType, fileVersion, testFileVersion); 
}

/******************************************************************************
* ReadAndTestOtaHeader  
*
* 	- read the ota header and compare part of it with image request
******************************************************************************/
static uint8_t ReadAndTestOtaHeader(uint16_t manufacturerCode, uint16_t imageType, uint32_t fileVersion, bool_t testFileVersion) 
{
    uint8_t buffer[sizeof(zclOTAFileHeader_t)]; 
    uint8_t *pData;
    zclOTAFileHeader_t otaHeader; //to store the ota header information
    uint32_t currentAddress = 0x00; 
    uint8_t index = 0; //index in the image list
    /* Initialize External Memory */
    OTA_InitExternalMemory();
    pData = &buffer[0];
    while((currentAddress < gExtFlash_TotalSize_c)&&(index < gMaxNumberOfOTAImage))
    {
        // Read Header
        (void)OTA_ReadExternalMemory(pData, sizeof(zclOTAFileHeader_t), currentAddress);
        FLib_MemCpy(&otaHeader, &pData[0], sizeof(zclOTAFileHeader_t));
        otaHeader.fileIdentifier = OTA2Native32( otaHeader.fileIdentifier);
        if(otaHeader.fileIdentifier != OTA_FILE_IDENTIFIER)
          return gMaxNumberOfOTAImage;
        listOfImage[index].addressOfCurrentImage  = currentAddress;
        listOfImage[index].manufacturerCode = OTA2Native16(otaHeader.manufacturerCode);
        listOfImage[index].imageType = OTA2Native16(otaHeader.imageType);
        listOfImage[index].fileVersion = OTA2Native32(otaHeader.fileVersion);
        listOfImage[index].totalImageSize = OTA2Native32(otaHeader.totalImageSize);
       
        if((imageType == listOfImage[index].imageType)&&
            	 (manufacturerCode == listOfImage[index].manufacturerCode))
        {
          	 if(testFileVersion == TRUE)
          	 {
          		 if(fileVersion == listOfImage[index].fileVersion)
          		 {
          			mServerUtility.imageLoadingState = gOtaServerLoadingImageComplete_c;
          			return index;
          		 }
          		 else
          		 {
                 	currentAddress+= listOfImage[index].totalImageSize;
                 	index++;
          		 }
           	 }
           	 else
           	 {
           		 mServerUtility.imageLoadingState = gOtaServerLoadingImageComplete_c;
           		 return index;
           	 }
        }
        else
        {
        	currentAddress+= listOfImage[index].totalImageSize;
        	index++;
        }
    }
    
   return gMaxNumberOfOTAImage;
}
/**********************************************************************
* OTAServerQueryImageRequestTmrCallback
*
*   - this callback is used only if the server has no information about the image
***********************************************************************/
static void OTAServerQueryImageRequestTmrCallback(uint8_t tmr) 
{
    ztcOtaServerQueryImageRequest_t queryRequest;
    (void)tmr;
    queryRequest.deviceID[0] = 0xFF;
    queryRequest.deviceID[1] = 0xFF;
    queryRequest.manufacturerCode = 0xFFFF;
    queryRequest.imageType = 0xFFFF;
    queryRequest.fileVersion = 0xFFFFFFFF;
    TMR_FreeTimer(gRequestBlockTimer);
#ifndef gHostApp_d  
    ZTCQueue_QueueToTestClient((const uint8_t*)&queryRequest, gZtcReqOpcodeGroup_c, mZtcOtaSupportQueryImageReq_c, sizeof(ztcOtaServerQueryImageRequest_t));
#else
    ZTCQueue_QueueToTestClient(gpHostAppUart, (const uint8_t*)&queryRequest, gZtcReqOpcodeGroup_c, mZtcOtaSupportQueryImageReq_c, sizeof(ztcOtaServerQueryImageRequest_t));
#endif 
}

/**********************************************************************
* OTAServerBlockRequestTmrCallback
*
*   - used for request a block(if the device contain external memory)
***********************************************************************/
static void OTAServerBlockRequestTmrCallback(uint8_t tmr) 
{
  ztcOtaServerChunkRequest_t chunkRequest;
    (void)tmr;  
    Copy2Bytes(chunkRequest.deviceID, NlmeGetRequest(gNwkShortAddress_c));
    if(mServerUtility.operationMode == gUseExternalMemoryForOtaUpdate_c)
    {	
      chunkRequest.currentOffset = (mIndexOfCurrentImage ==0)?Native2OTA32(mServerUtility.currentAddress):Native2OTA32(mServerUtility.currentAddress - listOfImage[mIndexOfCurrentImage-1].totalImageSize);
      /* block size = 200 or rest of image data*/
      chunkRequest.blockSize = (listOfImage[mIndexOfCurrentImage].totalImageSize -
                     mServerUtility.currentAddress + listOfImage[mIndexOfCurrentImage].addressOfCurrentImage > 200)?
                     200:(uint8_t)(listOfImage[mIndexOfCurrentImage].totalImageSize - mServerUtility.currentAddress +
                     listOfImage[mIndexOfCurrentImage].addressOfCurrentImage);  
#ifndef gHostApp_d  
    ZTCQueue_QueueToTestClient((const uint8_t*)&chunkRequest, gZtcReqOpcodeGroup_c, mZtcOtaSupportImageChunkReq_c, sizeof(ztcOtaServerChunkRequest_t));
#else
    ZTCQueue_QueueToTestClient(gpHostAppUart, (const uint8_t*)&chunkRequest, gZtcReqOpcodeGroup_c, mZtcOtaSupportImageChunkReq_c, sizeof(ztcOtaServerChunkRequest_t));
#endif 
    }
}

#endif

/****************************************************************************
 *  ZtcOtaServerBlockReceive
 *   - is called every time a new image block is received.
 ****************************************************************************/

zbStatus_t ZtcOtaServerBlockReceive(uint8_t *dataBlock, uint8_t length)
{
  zbStatus_t status = gOtaSucess_c;
  
  /* Validate parameters */
  if((length == 0) || (dataBlock == NULL)) 
  {
    return gOtaInvalidParam_c;
  }
  
  if(mServerUtility.operationMode == gDoNotUseExternalMemoryForOtaUpdate_c) 
  {
    /*if ExternalMemory = FALSE - complete the image block response*/
    FLib_MemCpy(&pZtcBlockResponse->zclOTABlockResponse.msgData.success.data[0], dataBlock, length);
    pZtcBlockResponse->zclOTABlockResponse.msgData.success.dataSize = length;
    status = ZCL_OTABlockResponse(pZtcBlockResponse);
    mServerUtility.blockResponseInProgress = FALSE;
    MSG_Free(pZtcBlockResponse);   
  }
  else
  { 
    /*operation mode: gUseExternalMemoryForOtaUpdate_c*/
#if (gZclOtaExternalMemorySupported_d)    
    /*Write image to external memory*/
    status = OTA_WriteExternalMemory(dataBlock, length, mServerUtility.currentAddress);
    if(status == gOtaSucess_c)
    {
      mServerUtility.currentAddress+= length;
      //send a request for next block
      if(mServerUtility.currentAddress - listOfImage[mIndexOfCurrentImage].addressOfCurrentImage < listOfImage[mIndexOfCurrentImage].totalImageSize)
        TMR_StartTimer(gRequestBlockTimer, gTmrSingleShotTimer_c,
                          BLOCK_PROCESSING_CALLBACK_DELAY, OTAServerBlockRequestTmrCallback); 
      else  
      { 
        ztcOtaServerImageNotify_t imgNotify = {0xFF, 0xFF, 0, 0, 0};
        mServerUtility.imageLoadingState = gOtaServerLoadingImageComplete_c;
        status = newImageReceived(imgNotify);
        TMR_FreeTimer(gRequestBlockTimer); 
        mIndexOfCurrentImage++;
      }
    }
#endif //  gZclOtaExternalMemorySupported_d  
  } 
  return status;
}
#endif

/*****************************************************************************
******************************************************************************
*
* ZCL OTA CLIENT FUNCTIONS
*
******************************************************************************
*****************************************************************************/

#if gZclEnableOTAClient_d
 
/******************************************************************************
* ZCL_OTAClusterClient_EndUpgradeRequest
*
* Sends upgrade end request to server
******************************************************************************/  

zbStatus_t ZCL_OTAClusterClient_EndUpgradeRequest
(
  zbNwkAddr_t   aNwkAddr,
  zbEndPoint_t  endPoint,
  zclOTAStatus_t status
) 
{
  zclZtcOTAUpgradeEndRequest_t upgradeEndRequest;
  
  Copy2Bytes(&upgradeEndRequest.aNwkAddr, aNwkAddr);
  upgradeEndRequest.endPoint = endPoint;
  upgradeEndRequest.zclOTAUpgradeEndRequest.status = status;
  upgradeEndRequest.zclOTAUpgradeEndRequest.manufacturerCode = Native2OTA16(clientParams.manufacturerCode);
  upgradeEndRequest.zclOTAUpgradeEndRequest.imageType = Native2OTA16(clientParams.imageType);            
  upgradeEndRequest.zclOTAUpgradeEndRequest.fileVersion = Native2OTA32(clientSession.downloadingFileVersion);
  
#if  (gZclEnableOTAProgressReport_d == TRUE) 
  if(status == gZclOTASuccess_c)
  {
      gOtaProgressReportStatus = otaProgress100_c;
      BeeAppUpdateDevice(0x08, gOTAProgressReportEvent_c, 0, 0, NULL);
  }
#endif 

#if (gZclEnableOTAProgressReportToExternalApp_d == TRUE)  
  {
    /*send a report message with status TRUE or FALSE in the imageLength field 
    *(TRUE - image successfully transmitted to client , FALSE - otherwise)*/
    zclZtcImageOtaProgressReport_t ztcOtaProgressReportInf;
    if(status == gZclOTASuccess_c)
      ztcOtaProgressReportInf.imageLength = Native2OTA32(TRUE);
    else
      ztcOtaProgressReportInf.imageLength = Native2OTA32(FALSE);
    ztcOtaProgressReportInf.currentOffset = 0xFFFFFFFF;
    Copy2Bytes(ztcOtaProgressReportInf.deviceAddr,NlmeGetRequest(gNwkShortAddress_c));   
#ifndef gHostApp_d  
    ZTCQueue_QueueToTestClient((const uint8_t*)&ztcOtaProgressReportInf, gHaZtcOpCodeGroup_c, gOTAImageProgressReport_c, sizeof(zclZtcImageOtaProgressReport_t));
#else
    ZTCQueue_QueueToTestClient(gpHostAppUart, (const uint8_t*)&ztcOtaProgressReportInf, gHaZtcOpCodeGroup_c, gOTAImageProgressReport_c, sizeof(zclZtcImageOtaProgressReport_t));
#endif 
  }
#endif   //gZclEnableOTAProgressReportToExternApp_d  
  
  
  return ZCL_OTAUpgradeEndRequest(&upgradeEndRequest);
}

/******************************************************************************
* ZCL_OTAClusterClient_EndUpgradeAbortRequest
*
* Aborts current client session and sends end request with Abort or Invalid Image
*(if security failed) status to server
******************************************************************************/  
zbStatus_t ZCL_OTAClusterClient_EndUpgradeAbortRequest
(
  zbNwkAddr_t   aNwkAddr,
  zbEndPoint_t  endPoint, 
  zclOTAStatus_t status
) 
{
  uint8_t result = ZCL_OTAClusterClient_EndUpgradeRequest(aNwkAddr, endPoint, status);
  OTAClusterClientAbortSession();
  if (result != gZclSuccess_c)
    return gZclOTAAbort_c;
  
  return gZclSuccess_c;
}


/******************************************************************************
* ZCL_OTAClusterClient_NextImageRequest
*
* Sends back a next image request (may be as a result of Image Notify)
******************************************************************************/  

zbStatus_t ZCL_OTAClusterClient_NextImageRequest 
(
  zbNwkAddr_t   aNwkAddr,
  zbEndPoint_t  endPoint    
)
{
  zclZtcOTANextImageRequest_t nextImageRequest;     
  
  //Send back the image request.
  Copy2Bytes(&nextImageRequest.aNwkAddr, aNwkAddr);
  nextImageRequest.endPoint = endPoint;
  nextImageRequest.zclOTANextImageRequest.fieldControl = gZclOTANextImageRequest_HwVersionPresent;
  nextImageRequest.zclOTANextImageRequest.manufacturerCode = Native2OTA16(clientParams.manufacturerCode);
  nextImageRequest.zclOTANextImageRequest.imageType = Native2OTA16(clientParams.imageType);
  nextImageRequest.zclOTANextImageRequest.fileVersion = Native2OTA32(clientParams.currentFileVersion);
  nextImageRequest.zclOTANextImageRequest.hardwareVersion = Native2OTA16(clientParams.hardwareVersion);
  return ZCL_OTAImageRequest(&nextImageRequest);  
}


/******************************************************************************
* ZCL_OTAClusterClient_ImageNotify_Handler
*
* Handles the receipt of an ImageNotify Command on the client
******************************************************************************/  
zbStatus_t ZCL_OTAClusterClient_ImageNotify_Handler 
(
  zbApsdeDataIndication_t *pIndication
)
{
  zclOTAImageNotify_t *pImageNotify;
  uint8_t frameLen;
  uint8_t clientJitter;
  zclStatus_t result;
  bool_t  dropPacket = FALSE;
  
  bool_t isUnicast = (pIndication->fWasBroadcast == TRUE)?FALSE:TRUE;;//IsValidNwkUnicastAddr(pIndication->aDstAddr);
 
  //update transaction Sequence number
  if(pIndication->pAsdu[1] == gZclTransactionId)
	  gZclTransactionId++;
  pImageNotify =  (zclOTAImageNotify_t *)(pIndication->pAsdu + sizeof(zclFrame_t));
  frameLen = pIndication->asduLength - sizeof(zclFrame_t); 

  // invalid payload type or invalid data length for specified payload type data
  if (frameLen < sizeof(pImageNotify->payloadType) ||
      pImageNotify->payloadType >= gPayloadMax_c ||
      frameLen != imgNotifySizeArray[pImageNotify->payloadType])
    return  gZclMalformedCommand_c;
    
  // unicast
  if (isUnicast) 
  {
      // error cases
     /* if ((pImageNotify->payloadType != gJitter_c)||(pImageNotify->queryJitter != OTA_CLIENT_MAX_RANDOM_JITTER))
         return  gZclMalformedCommand_c;
      else {
    */
      // send back query next image
      result = ZCL_OTAClusterClient_NextImageRequest(pIndication->aSrcAddr, pIndication->srcEndPoint);
      if (result != gZclSuccess_c)
        result = gZclOTAAbort_c;
        return result;
   // }
  }

  // broadcast/multicast
  
  // validate jitter  
  if (pImageNotify->queryJitter > OTA_CLIENT_MAX_RANDOM_JITTER)
    dropPacket = TRUE;
  else
    // compute random jitter
    clientJitter = GetRandomRange(0, OTA_CLIENT_MAX_RANDOM_JITTER);
  
  // validate manufacturer
  if (pImageNotify->payloadType > gJitter_c) 
  {
    pImageNotify->manufacturerCode = OTA2Native16(pImageNotify->manufacturerCode);
    if (pImageNotify->manufacturerCode != clientParams.manufacturerCode &&
        pImageNotify->manufacturerCode != OTA_MANUFACTURER_CODE_MATCH_ALL)
        dropPacket = TRUE;
  }
  
  // validate imageType
  if (pImageNotify->payloadType > gJitterMfg_c) 
  {
    pImageNotify->imageType = OTA2Native16(pImageNotify->imageType);
    if (pImageNotify->imageType != clientParams.imageType &&
        pImageNotify->imageType != OTA_IMAGE_TYPE_MATCH_ALL)
        dropPacket = TRUE;
  }
  
  // validate fileVersion
  if (pImageNotify->payloadType > gJitterMfgImage_c) 
  {
    pImageNotify->fileVersion = OTA2Native32(pImageNotify->fileVersion);
    if (pImageNotify->fileVersion == clientParams.currentFileVersion)
        dropPacket = TRUE;
  }
  
  
  if (!dropPacket &&  clientJitter <= pImageNotify->queryJitter)
    // send jitter message
    {
      result = ZCL_OTAClusterClient_NextImageRequest(pIndication->aSrcAddr, pIndication->srcEndPoint);   
      if (result != gZclSuccess_c)
        result = gZclOTAAbort_c;
        return result;      
    }
  else
    // no further processing
    return gZclSuccess_c; 
  
}


/******************************************************************************
* ZCL_OTAClusterClient_NextBlockRequest
*
* Sends back a next image request (may be as a result of Image Notify)
******************************************************************************/  

zbStatus_t ZCL_OTAClusterClient_NextBlockRequest 
(
  zbNwkAddr_t   aNwkAddr,
  zbEndPoint_t  endPoint    
)
{ 
  zclZtcOTABlockRequest_t     blockRequest;

  Copy2Bytes(&blockRequest.aNwkAddr,aNwkAddr);
  blockRequest.endPoint = endPoint;
  blockRequest.zclOTABlockRequest.fieldControl = 0x00;
  blockRequest.zclOTABlockRequest.manufacturerCode = Native2OTA16(clientParams.manufacturerCode);
  blockRequest.zclOTABlockRequest.imageType = Native2OTA16(clientParams.imageType);
  blockRequest.zclOTABlockRequest.fileVersion = Native2OTA32(clientSession.downloadingFileVersion);
  blockRequest.zclOTABlockRequest.fileOffset = Native2OTA32(clientSession.currentOffset);
  blockRequest.zclOTABlockRequest.maxDataSize = clientParams.maxDataSize;
  return ZCL_OTABlockRequest(&blockRequest);
}

/******************************************************************************
* ZCL_OTAClusterClient_NextImageRequest
*
* Sends back a next image request (may be as a result of Image Notify)
******************************************************************************/  

zbStatus_t ZCL_OTAClusterClient_NextImageResponse_Handler
(
  zbApsdeDataIndication_t *pIndication
)
{
  zclOTANextImageResponse_t   *pNextImageResponse;
  uint8_t result;

  pNextImageResponse = (zclOTANextImageResponse_t *)(pIndication->pAsdu + sizeof(zclFrame_t));
  
  // status is successful
  if(pNextImageResponse->status == gZclOTASuccess_c)
  {
    // validate frame length
    if (pIndication->asduLength - sizeof(zclFrame_t) != sizeof(zclOTANextImageResponse_t))
      return gZclMalformedCommand_c;
    
    pNextImageResponse->manufacturerCode = OTA2Native16(pNextImageResponse->manufacturerCode);
    pNextImageResponse->imageType = OTA2Native16(pNextImageResponse->imageType);
    pNextImageResponse->fileVersion = OTA2Native32(pNextImageResponse->fileVersion);
    pNextImageResponse->imageSize = OTA2Native32(pNextImageResponse->imageSize);    
    
    // validate frame params
    if ((pNextImageResponse->manufacturerCode != clientParams.manufacturerCode &&
        pNextImageResponse->manufacturerCode != OTA_MANUFACTURER_CODE_MATCH_ALL) ||
        (pNextImageResponse->imageType != clientParams.imageType &&
        pNextImageResponse->imageType != OTA_IMAGE_TYPE_MATCH_ALL ))   
      return gZclMalformedCommand_c;
    
    // begin client session
    result = OTAClusterClientStartSession(pNextImageResponse->imageSize, pNextImageResponse->fileVersion);
    if(result != gZbSuccess_c) return gZclOTAAbort_c;
    //stop timer that send query image request periodically
    TMR_FreeTimer(gNextImageReqTimer); 
    
    gRetransmitLastBlockTimer = TMR_AllocateTimer(); 
    if(gRetransmitLastBlockTimer == gTmrInvalidTimerID_c)
       return gZclNoMem_c;	
    
    // send the first block request    
    result = ZCL_OTAClusterClient_NextBlockRequest(pIndication->aSrcAddr, pIndication->srcEndPoint);
    if(result != gZclSuccess_c) {
      return ZCL_OTAClusterClient_EndUpgradeAbortRequest(pIndication->aSrcAddr, pIndication->srcEndPoint, gZclOTAAbort_c);
    }
    return  gZclSuccess_c;
  }
  else
  {
	if(pNextImageResponse->status == gZclOTANoImageAvailable_c)  
	{
                TMR_FreeTimer(gNextImageReqTimer); 
		gNextImageReqTimer = TMR_AllocateTimer(); 
		if(gNextImageReqTimer == gTmrInvalidTimerID_c)
			return gZclNoMem_c;		
		TMR_StartMinuteTimer(gNextImageReqTimer, gMinforNextImageRequest_c, OTAClusterClientNextImageReqTimerCallback);
	}
	else
	{
#if gNum_EndPoints_c != 0    
    	zclInitiateOtaProcess_t  initServerDiscovery = {0, 0};
    	initServerDiscovery.isServer = FALSE;
    	initServerDiscovery.clientInit.endPoint = endPointList[0].pEndpointDesc->pSimpleDesc->endPoint;
		(void)OTA_InitiateOtaClusterProcess(&initServerDiscovery);
#endif		
	}
    return gZclSuccess_c;
  } 
}

/******************************************************************************
* ZCL_OTAClusterClient_BlockResponse_Handler
*
* Handles the block response indication
******************************************************************************/  

zbStatus_t ZCL_OTAClusterClient_BlockResponse_Handler
(
  zbApsdeDataIndication_t *pIndication
) 
{
  zclOTABlockResponse_t *pBlockResponse;

  uint8_t result;
  
  //Handle succes status received
  //Verify the received parameters
  pBlockResponse = (zclOTABlockResponse_t *)(pIndication->pAsdu + sizeof(zclFrame_t));

  if(pBlockResponse->status == gZclOTASuccess_c)
  {
      // validate frame length
    if (pIndication->asduLength - sizeof(zclFrame_t) != 
        sizeof(zclOTAStatus_t) + MbrOfs(zclOTABlockResponseStatusSuccess_t, data) + 
        pBlockResponse->msgData.success.dataSize)
      return gZclMalformedCommand_c;
      
    // command fields
    pBlockResponse->msgData.success.manufacturerCode = OTA2Native16(pBlockResponse->msgData.success.manufacturerCode);
    pBlockResponse->msgData.success.imageType = OTA2Native16(pBlockResponse->msgData.success.imageType);
    pBlockResponse->msgData.success.fileVersion = OTA2Native32(pBlockResponse->msgData.success.fileVersion);
    pBlockResponse->msgData.success.fileOffset = OTA2Native32(pBlockResponse->msgData.success.fileOffset);
          
    // Error cases. Send back malformed command indication - no abort of the current upgrade executed.
    if((pBlockResponse->msgData.success.manufacturerCode != clientParams.manufacturerCode)||
       (pBlockResponse->msgData.success.imageType != clientParams.imageType)||
       (pBlockResponse->msgData.success.fileVersion != clientSession.downloadingFileVersion) ||
       (pBlockResponse->msgData.success.dataSize > clientParams.maxDataSize ) ||
       (pBlockResponse->msgData.success.dataSize + clientSession.currentOffset > clientSession.fileLength)) 
    {  
      return gZclMalformedCommand_c;
    }
    // handle out of sync packets by repeating the request (spec does not handle this as error)
    else if ( pBlockResponse->msgData.success.fileOffset != clientSession.currentOffset ) 
    {
      // send the first block request    
      result = ZCL_OTAClusterClient_NextBlockRequest(pIndication->aSrcAddr, pIndication->srcEndPoint);
      if(result != gZclSuccess_c) 
      {
        return ZCL_OTAClusterClient_EndUpgradeAbortRequest(pIndication->aSrcAddr, pIndication->srcEndPoint, gZclOTAAbort_c);
      }
      return  gZclSuccess_c;
    }
    else 
    {
      // Handle the received data chunk - push it to the image storage if we have a session started
      if(clientSession.sessionStarted == TRUE)
      {
        if(gpBlockCallbackState)
          MSG_Free(gpBlockCallbackState);
        // do it on a timer to handle processing aps and 
        // writing to external memory sync issues
        gBlockCallbackTimer = TMR_AllocateTimer();
        gpBlockCallbackState = AF_MsgAlloc();

        if (gpBlockCallbackState && (gBlockCallbackTimer != gTmrInvalidTimerID_c)) 
        {             
           Copy2Bytes(&gpBlockCallbackState->dstAddr, &pIndication->aSrcAddr);
           gpBlockCallbackState->dstEndpoint = pIndication->srcEndPoint;
           gpBlockCallbackState->blockSize = pBlockResponse->msgData.success.dataSize;
           FLib_MemCpy(gpBlockCallbackState->blockData,
                       pBlockResponse->msgData.success.data, 
                       pBlockResponse->msgData.success.dataSize);
           
           TMR_StartTimer(gBlockCallbackTimer, gTmrSingleShotTimer_c,
                          BLOCK_PROCESSING_CALLBACK_DELAY, OTAClusterClientProcessBlockTimerCallback);
                          
           return gZclSuccess_c;
        } 
        else 
        {
          return ZCL_OTAClusterClient_EndUpgradeAbortRequest(pIndication->aSrcAddr, pIndication->srcEndPoint, gZclOTAAbort_c);
        }
      }
      else 
      {
        return ZCL_OTAClusterClient_EndUpgradeAbortRequest(pIndication->aSrcAddr, pIndication->srcEndPoint, gZclOTAAbort_c);
      }
    }
  }
    else if(pBlockResponse->status == gZclOTAWaitForData_c)
    {
    	uint32_t timeInSeconds = OTA2Native32(pBlockResponse->msgData.wait.requestTime) -
						OTA2Native32(pBlockResponse->msgData.wait.currentTime);
        gApsAckConfirmStatus = TRUE;
        TMR_StartSecondTimer(gRetransmitLastBlockTimer,(uint16_t)timeInSeconds, OTAClientRetransmitLastBlockTmrCallback); 
        return  gZclSuccess_c;
    }
    else if(pBlockResponse->status == gZclOTAAbort_c)
    {
        OTAClusterClientAbortSession();
        return gZclSuccess_c;
    }
    else //unknown status
      //Error detected. Send back malformed command indication - no abort of the current upgrade executed.
      return gZclMalformedCommand_c;

}
  


/******************************************************************************
* ZCL_OTAClusterClient_UpgradeEndResponse_Handler
*
* Handles the upgrade end response indication
******************************************************************************/  

zbStatus_t ZCL_OTAClusterClient_UpgradeEndResponse_Handler
(
  zbApsdeDataIndication_t *pIndication
) 

{
    zclOTAUpgradeEndResponse_t *pUpgradeEndResponse;
    pUpgradeEndResponse = (zclOTAUpgradeEndResponse_t*)(pIndication->pAsdu + sizeof(zclFrame_t));
    pUpgradeEndResponse->manufacturerCode = OTA2Native16(pUpgradeEndResponse->manufacturerCode);
    pUpgradeEndResponse->imageType = OTA2Native16(pUpgradeEndResponse->imageType);
    pUpgradeEndResponse->fileVersion = OTA2Native32(pUpgradeEndResponse->fileVersion);
    pUpgradeEndResponse->currentTime = OTA2Native32(pUpgradeEndResponse->currentTime);
    pUpgradeEndResponse->upgradeTime = OTA2Native32(pUpgradeEndResponse->upgradeTime);
      
    //Verify the data received in the response  
    if((pUpgradeEndResponse->manufacturerCode != clientParams.manufacturerCode)||
       (pUpgradeEndResponse->imageType != clientParams.imageType)||
       (pUpgradeEndResponse->fileVersion != clientSession.downloadingFileVersion))
    {
      return gZclMalformedCommand_c;
    }
    else
    {
      uint32_t offset = pUpgradeEndResponse->upgradeTime - pUpgradeEndResponse->currentTime;

      if(offset == 0)
      {   
        offset++;
        if(clientSession.steps >= 3)
        {
          // allow at least 1 second before reset
          gBlockCallbackTimer = TMR_AllocateSecondTimer();
          // Flash flags will be write in next instance of idle task 
          OTA_SetNewImageFlag();
          // OTATODO this does not accept 32 bit second timers
          TMR_StartSecondTimer(gBlockCallbackTimer, (uint16_t)offset, OTAClusterCPUResetCallback);      
        }
      }
      else
      {
        if(pUpgradeEndResponse->upgradeTime == 0xFFFFFFFF)
        {
            // wait to upgrate after receiving the 2nd Upgrade End Response   
            gOTAAttrs.ImageUpgradeStatus = gOTAUpgradeStatusWaitingToUpgrade_c;      
             // OTATODO do the next request in 60 minutes
        }
        else
        {
            //wait offset seconds after the client shall perform to apply the upgrade process;
            gOTAAttrs.ImageUpgradeStatus = gOTAUpgradeStatusCountDown_c; 
            gBlockCallbackTimer = TMR_AllocateSecondTimer();
            // OTATODO this does not accept 32 bit second timers
            TMR_StartSecondTimer(gBlockCallbackTimer, (uint16_t)offset, OTAClusterDelayedUpgradeCallback);  
        }

      } 
      
      return gZclSuccess_c;
    }

}

/******************************************************************************
* OTAClusterDelayedUpgradeCallback
*
* Resets the node to upgrade after Upgrade End Response
******************************************************************************/
static void OTAClusterDelayedUpgradeCallback(uint8_t tmr) 
{
  (void)tmr;  
  // Flash flags will be write in next instance of idle task 
  OTA_SetNewImageFlag();
  // allow at least 1 second before reset
  TMR_StartSecondTimer(gBlockCallbackTimer, 0x01, OTAClusterCPUResetCallback); 
}

/******************************************************************************
* OTAClusterResetCallback
*
* Resets the node to upgrade after Upgrade End Response
******************************************************************************/
static void OTAClusterCPUResetCallback(uint8_t tmr) 
{
  (void)tmr;
#ifndef __IAR_SYSTEMS_ICC__
  // S08 platform reset
  PWRLib_Reset();               
#else
  CRM_SoftReset();
#endif  
}


/******************************************************************************
* ZCL_OTAClusterClient
*
* Incoming OTA cluster frame processing loop for cluster client side
******************************************************************************/
zbStatus_t ZCL_OTAClusterClient
(
  zbApsdeDataIndication_t *pIndication, 
  afDeviceDef_t *pDevice
)
{
    zclCmd_t            command;
    zclFrame_t          *pFrame;
    zbStatus_t          result = gZclUnsupportedClusterCommand_c;    
    zclCmdDefaultRsp_t * pDefaultRsp;
    zbIeeeAddr_t aExtAddr;
    /* prevent compiler warning */
    (void)pDevice;
    pFrame = (void *)pIndication->pAsdu;

    /* handle the command */
    command = pFrame->command;
    switch(command)
    {
      case gZclCmdOTA_ImageNotify_c:
    	gApsAckConfirmStatus = FALSE;  
    	/*ignore command if an ota session started */
    	if(clientSession.sessionStarted == TRUE)
    		return gZclSuccess_c;
        /*test the upgradeServerID attribute*/
        (void)APS_GetIeeeAddress(pIndication->aSrcAddr, aExtAddr);
        if(!FLib_MemCmp(aExtAddr,gOTAAttrs.UpgradeServerId, 8))
        {
#if gNum_EndPoints_c != 0      
        	otaCluster_simpleDescriptor.endPoint =  endPointList[0].pEndpointDesc->pSimpleDesc->endPoint;  
        	Copy2Bytes(&otaCluster_simpleDescriptor.aAppProfId, endPointList[0].pEndpointDesc->pSimpleDesc->aAppProfId);     
			  
        	ASL_MatchDescriptor_req(NULL,pIndication->aSrcAddr, (zbSimpleDescriptor_t*)&otaCluster_simpleDescriptor); 
        	mSendOtaMatchDescriptorRequest = TRUE;
#endif	    
        }
        return ZCL_OTAClusterClient_ImageNotify_Handler(pIndication);   
      case gZclCmdOTA_QueryNextImageResponse_c:
    	gApsAckConfirmStatus = FALSE;     
        return ZCL_OTAClusterClient_NextImageResponse_Handler(pIndication);
      case gZclCmdOTA_ImageBlockResponse_c:
    	 result = gZclSuccess_c;
    	 if(gApsAckConfirmStatus == TRUE){
    	    //if receive an APS Ack with succes	
    	    TMR_StopTimer(gRetransmitLastBlockTimer);
    	    gApsAckConfirmStatus = FALSE;
    	    result =  ZCL_OTAClusterClient_BlockResponse_Handler(pIndication);
    	 }
    	 if(TMR_IsTimerActive(gRetransmitLastBlockTimer)==FALSE)
            if(TMR_IsTimerReady(gRetransmitLastBlockTimer)==FALSE)
    	    	TMR_StartIntervalTimer(gRetransmitLastBlockTimer, 3000, OTAClientRetransmitLastBlockTmrCallback); 
    	 break;
        //return ZCL_OTAClusterClient_BlockResponse_Handler(pIndication);
      case gZclCmdOTA_UpgradeEndResponse_c:  
        return ZCL_OTAClusterClient_UpgradeEndResponse_Handler(pIndication);
      case gZclCmdDefaultRsp_c:
         pDefaultRsp = ZCL_GetPayload(pIndication);
         // abort the session when receive an DefaultResponse with status 0x01 for End Request Command;
         if(((uint8_t)(pDefaultRsp->status) == gZclFailure_c)&&(pDefaultRsp->cmd==gZclCmdOTA_UpgradeEndRequest_c))
           OTAClusterClientAbortSession();
         result = gZclSuccess_c;
         break;
      default:
        break;
    }
    return result;
}


/******************************************************************************
* ZCL_OTASetClientParams
*
* Interface function to set client parameters
******************************************************************************/

zbStatus_t ZCL_OTASetClientParams(zclOTAClientParams_t* pClientParams)
{
  FLib_MemCpy(&clientParams, pClientParams, sizeof(zclOTAClientParams_t));
  clientParams.manufacturerCode = OTA2Native16(clientParams.manufacturerCode);
  clientParams.imageType = OTA2Native16(clientParams.imageType);
  clientParams.currentFileVersion = OTA2Native32(clientParams.currentFileVersion);
  clientParams.downloadedFileVersion = OTA2Native32(clientParams.downloadedFileVersion);
  clientParams.hardwareVersion = OTA2Native16(clientParams.hardwareVersion);
#if gZclClusterOptionals_d 
  gOTAAttrs.CurrentFileVersion = Native2OTA32(clientParams.currentFileVersion);
  gOTAAttrs.DownloadedFileVersion = Native2OTA32(clientParams.downloadedFileVersion);
#endif
  return gZclSuccess_c;
}

/******************************************************************************
* ZCL_OTAImageRequest
*
* Request to send a image request
******************************************************************************/
zbStatus_t ZCL_OTAImageRequest(zclZtcOTANextImageRequest_t* pZtcNextImageRequestParams)
{
  afToApsdeMessage_t *pMsg;
  afAddrInfo_t addrInfo = {gZbAddrMode16Bit_c, {0x00, 0x00}, 0, {gaZclClusterOTA_c}, 0, gApsTxOptionAckTx_c, 1};
  uint8_t len = sizeof(zclOTANextImageRequest_t);  
  
  if(pZtcNextImageRequestParams->zclOTANextImageRequest.fieldControl == 0x00) //gZclOTANextImageRequest_HwVersionPresent = FALSE
	  len -=sizeof(uint16_t); //remove hardware version from packet
  //Create the destination address
  Copy2Bytes(&addrInfo.dstAddr,&pZtcNextImageRequestParams->aNwkAddr);
  addrInfo.dstEndPoint = pZtcNextImageRequestParams->endPoint;
  addrInfo.srcEndPoint = pZtcNextImageRequestParams->endPoint;
  addrInfo.radiusCounter = afDefaultRadius_c;
  
  pMsg = ZCL_CreateFrame( &addrInfo, 
                          gZclCmdOTA_QueryNextImageRequest_c,
                          gZclFrameControl_FrameTypeSpecific, 
                          NULL, 
                          &len,
                          &pZtcNextImageRequestParams->zclOTANextImageRequest);
  if(!pMsg)
    return gZclNoMem_c;
 return ZCL_DataRequestNoCopy(&addrInfo, len, pMsg);
}

/******************************************************************************
* ZCL_OTABlockRequest
*
* Request to send an image block
******************************************************************************/
zbStatus_t ZCL_OTABlockRequest(zclZtcOTABlockRequest_t *pZtcBlockRequestParams)
{
  afToApsdeMessage_t *pMsg;
  afAddrInfo_t addrInfo = {gZbAddrMode16Bit_c, {0x00, 0x00}, 0, {gaZclClusterOTA_c}, 0, gApsTxOptionAckTx_c, 1};
  uint8_t len = sizeof(zclOTABlockRequest_t);

  //Create the destination address
	Copy2Bytes(&addrInfo.dstAddr,&pZtcBlockRequestParams->aNwkAddr);
  addrInfo.dstEndPoint = pZtcBlockRequestParams->endPoint;
  addrInfo.srcEndPoint = pZtcBlockRequestParams->endPoint;
  addrInfo.radiusCounter = afDefaultRadius_c;
  
  pMsg = ZCL_CreateFrame( &addrInfo, 
                          gZclCmdOTA_ImageBlockRequest_c,
                          gZclFrameControl_FrameTypeSpecific, 
                          NULL, 
                          &len,
                          &pZtcBlockRequestParams->zclOTABlockRequest);
  if(!pMsg)
    return gZclNoMem_c;
 return ZCL_DataRequestNoCopy(&addrInfo, len, pMsg);  
}

/******************************************************************************
* ZCL_OTAUpgradeEndRequest
*
* Request to send an upgrade end request
******************************************************************************/
zbStatus_t ZCL_OTAUpgradeEndRequest(zclZtcOTAUpgradeEndRequest_t *pZtcUpgradeEndParams)
{
  afToApsdeMessage_t *pMsg;
  afAddrInfo_t addrInfo = {gZbAddrMode16Bit_c, {0x00, 0x00}, 0, {gaZclClusterOTA_c}, 0, gApsTxOptionAckTx_c, 1};
  uint8_t len;

  //Create the destination address
	Copy2Bytes(&addrInfo.dstAddr,&pZtcUpgradeEndParams->aNwkAddr);
  addrInfo.dstEndPoint = pZtcUpgradeEndParams->endPoint;
  addrInfo.srcEndPoint = pZtcUpgradeEndParams->endPoint;
  addrInfo.radiusCounter = afDefaultRadius_c;
  
  len = sizeof(zclOTAUpgradeEndRequest_t);

  
  pMsg = ZCL_CreateFrame( &addrInfo, 
                          gZclCmdOTA_UpgradeEndRequest_c,
                          gZclFrameControl_FrameTypeSpecific, 
                          NULL, 
                          &len,
                          &pZtcUpgradeEndParams->zclOTAUpgradeEndRequest);
  if(!pMsg)
    return gZclNoMem_c;
 return ZCL_DataRequestNoCopy(&addrInfo, len, pMsg);  
}

/******************************************************************************
* OTAClusterClientAbortSession
*
* Private function. Abort a started image download session
******************************************************************************/
void OTAClusterClientAbortSession()
{
  clientSession.sessionStarted = FALSE;
  clientSession.fileLength = 0;
  clientSession.currentOffset = 0;  
  clientSession.steps = 0;
  
  TMR_FreeTimer(gRetransmitLastBlockTimer); 
  TMR_FreeTimer(gBlockCallbackTimer);
  MSG_Free(gpBlockCallbackState);
  OTA_CancelImage();
  //(void)OTA_EraseExternalMemory();
  gNextImageReqTimer = TMR_AllocateTimer(); 
  if(gNextImageReqTimer == gTmrInvalidTimerID_c)
     return;
  TMR_StartMinuteTimer(gNextImageReqTimer, gMinforNextImageRequest_c, OTAClusterClientNextImageReqTimerCallback);
  gOTAAttrs.ImageUpgradeStatus = gOTAUpgradeStatusNormal_c;
#if gZclClusterOptionals_d
  gOTAAttrs.CurrentFileVersion = Native2OTA32(clientParams.currentFileVersion);
  gOTAAttrs.DownloadedFileVersion = Native2OTA32(clientParams.downloadedFileVersion);
  gOTAAttrs.FileOffset = 0;
#endif
  if(clientSession.pStateBuffer != NULL)
  {
    MSG_Free(clientSession.pStateBuffer);
    clientSession.pStateBuffer = NULL;
  }
}

/******************************************************************************
* OTAClusterClientStartSession
*
* Private function. Starts an image download session.
******************************************************************************/
#if  (gZclEnableOTAProgressReport_d == TRUE)
uint8_t gOtaProgressReportStatus = otaStartProgress_c;  //have 4 states: 0 = startProgress, 1 = 33%, 2 = 66%, 3 = 100%
#endif

#if (gZclEnableOTAClientECCLibrary_d == TRUE)
  static uint8_t  mMsgDigest[AES_MMO_SIZE];
#endif  
  
zbStatus_t OTAClusterClientStartSession(uint32_t fileLength, uint32_t fileVersion)
{
  //Download file management
  clientSession.sessionStarted = TRUE;
  clientSession.fileLength = fileLength;
  clientSession.downloadingFileVersion = fileVersion;
  clientSession.currentOffset = 0;  
  //Buffer management and state initialization for image file stream processing
  clientSession.state = init_c;
  clientSession.steps = 0;
  clientSession.bytesNeededForState = HEADER_LEN_OFFSET + sizeof(uint16_t);
  clientSession.stateBufferIndex = 0;
  if(clientSession.pStateBuffer == NULL)
  {
    clientSession.pStateBuffer = MSG_Alloc(SESSION_BUFFER_SIZE);
  }
  if(clientSession.pStateBuffer == NULL) return gZbNoMem_c;

#if (gZclEnableOTAClientECCLibrary_d == TRUE)
  FLib_MemSet(&mMsgDigest[0], 0x00, AES_MMO_SIZE);
#endif  
#if  (gZclEnableOTAProgressReport_d == TRUE)
  gOtaProgressReportStatus = otaStartProgress_c;  //have 4 states: 0 = startProgress, 1 = 33%, 2 = 66%, 3 = 100%
#endif  
  gOTAAttrs.ImageUpgradeStatus = gOTAUpgradeStatusDownloadInProgress_c;
  return gZbSuccess_c;
}

/******************************************************************************
* OTAClusterClientRunImageProcessStateMachine
*
* Private function. Process a block of received data.
******************************************************************************/
zbStatus_t OTAClusterClientRunImageProcessStateMachine()
{
  zclOTAFileHeader_t* pHeader;
  zclOTAFileSubElement_t* pSubElement;
  static uint32_t subElementLen;
  static uint8_t *pBitmap;
  uint16_t headerLen;
#if (gZclEnableOTAClientECCLibrary_d == FALSE)
  uint32_t crcReceived = 0;
    /* Current CRC value */
  static uint16_t mCrcCompute  = 0;
#else
  static IdentifyCert_t mCertReceived;
#endif
  
  switch(clientSession.state)
  {
  case init_c:  
    //In the init state we only extract the header length and move to the next state.
    //The bytes received so far are not consumed
    headerLen = *(uint16_t*)(clientSession.pStateBuffer + HEADER_LEN_OFFSET);
    clientSession.bytesNeededForState = (uint8_t)OTA2Native16(headerLen);
    clientSession.state = processHeader_c;
#if (gZclEnableOTAClientECCLibrary_d == FALSE)	
    mCrcCompute  = 0;
#endif
    break;
  case processHeader_c:
    pHeader = (zclOTAFileHeader_t*)clientSession.pStateBuffer;
#if (gZclEnableOTAClientECCLibrary_d == FALSE) 
        /*process image CRC*/
    mCrcCompute = OTA_CrcCompute(clientSession.pStateBuffer, clientSession.bytesNeededForState, mCrcCompute);
#endif    
    //Check the header for consistency
    if((pHeader->headerVersion != gZclOtaHeaderVersion_c)||(OTA2Native16(pHeader->imageType) != clientParams.imageType)||
       (OTA2Native16(pHeader->manufacturerCode) != clientParams.manufacturerCode))
    {
      return gZclOTAAbort_c;
    }
    //check the field control for supported features - upgrade file destination and security credential version not supported 
    if((OTA2Native16(pHeader->fieldControl) & SECURITY_CREDENTIAL_VERSION_PRESENT)||(OTA2Native16(pHeader->fieldControl) & DEVICE_SPECIFIC_FILE))
      return gZclOTAAbort_c;
    //If HW version is specified, verify it against our own
    if(OTA2Native16(pHeader->fieldControl) & HARDWARE_VERSION_PRESENT)
    {
      if(!((OTA2Native16(pHeader->minHWVersion) <= clientParams.hardwareVersion)&&(clientParams.hardwareVersion <= OTA2Native16(pHeader->maxHWVersion))))
        return gZclOTAAbort_c;
    }
    //If we got here it means we are ready for the upgrade image tag.
    //All bytes from the buffer have been processed. The next state requires receiving a sub-element
    clientSession.state = processSubElementTag_c;
    clientSession.bytesNeededForState =  sizeof(zclOTAFileSubElement_t);
    clientSession.stateBufferIndex = 0;
    clientSession.steps++;
  break;
  case processSubElementTag_c:
    pSubElement = (zclOTAFileSubElement_t*)clientSession.pStateBuffer;
#if (gZclEnableOTAClientECCLibrary_d == FALSE)	    
        //process image CRC
    if(pSubElement->id != gZclOtaUpgradeCRCTagId_c)    
        mCrcCompute = OTA_CrcCompute(clientSession.pStateBuffer, clientSession.bytesNeededForState, mCrcCompute);
#endif
    switch(pSubElement->id)
    {      
      case gZclOtaUpgradeImageTagId_c:
        clientSession.state = processUpgradeImage_c;
        //All OK, get the image length
        subElementLen = OTA2Native32(pSubElement->length);
        //Start the FLASH upgrade process
        if(OTA_StartImage(subElementLen) != gOtaSucess_c)
        {
          return gZclOTAAbort_c;
        } 
        clientSession.steps++;
        clientSession.bytesNeededForState = (uint8_t)((subElementLen > SESSION_BUFFER_SIZE) ? SESSION_BUFFER_SIZE : subElementLen);
        break;
      case gZclOtaSectorBitmapTagId_c:  
        clientSession.state = processBitmap_c;
        clientSession.bytesNeededForState = (uint8_t)OTA2Native32(pSubElement->length);
        break;
#if (gZclEnableOTAClientECCLibrary_d == FALSE)	        
      case gZclOtaUpgradeCRCTagId_c:
        clientSession.state = processCRC_c;
        clientSession.bytesNeededForState = (uint8_t)OTA2Native32(pSubElement->length);
        break;
#else        
      case gZclOtaECDSASigningCertTagId_c: 
        clientSession.state = processECDSASignCert_c;
        clientSession.bytesNeededForState = (uint8_t)OTA2Native32(pSubElement->length);
        break;
      case gZclOtaECDSASignatureTagId_c: 
        clientSession.state = processECDSASigning_c;
        clientSession.bytesNeededForState = (uint8_t)OTA2Native32(pSubElement->length);
        break;
#endif        
      default:
        clientSession.state = stateMax_c;
        clientSession.bytesNeededForState = 1;   
        break;
    }     
    clientSession.stateBufferIndex = 0;
    break;
  case processUpgradeImage_c:
#if (gZclEnableOTAClientECCLibrary_d == FALSE)    
    /*process image CRC*/
    mCrcCompute = OTA_CrcCompute(clientSession.pStateBuffer, clientSession.bytesNeededForState, mCrcCompute);
#endif    
    //New image chunk arrived. upgradeImageLen is updated by the OTA platform component.
    if(OTA_PushImageChunk(clientSession.pStateBuffer, clientSession.bytesNeededForState, NULL) != gOtaSucess_c)
    {
      return gZclOTAAbort_c;
    }
    subElementLen-=clientSession.bytesNeededForState;
    //Prepare for next chunk or next state if the image was downloaded
    if(subElementLen != 0)
    {
      //More chuncks to come
      clientSession.bytesNeededForState =  (uint8_t)((subElementLen > SESSION_BUFFER_SIZE) ? SESSION_BUFFER_SIZE : subElementLen);
      clientSession.stateBufferIndex = 0;
    }
    else
    {
      //We need to move to the next state
      clientSession.state = processSubElementTag_c;
      clientSession.bytesNeededForState = sizeof(zclOTAFileSubElement_t);
      clientSession.stateBufferIndex = 0;
    }
    break;  
  case processBitmap_c:
   pBitmap = MSG_Alloc(clientSession.bytesNeededForState); 
   if(!pBitmap) 
       return gZclOTAAbort_c;
   FLib_MemCpy(pBitmap, clientSession.pStateBuffer, (clientSession.bytesNeededForState));   
#if (gZclEnableOTAClientECCLibrary_d == FALSE)	
    /*process image CRC*/
    mCrcCompute = OTA_CrcCompute(clientSession.pStateBuffer, clientSession.bytesNeededForState, mCrcCompute);
#endif    
    clientSession.state = processSubElementTag_c;
    clientSession.bytesNeededForState = sizeof(zclOTAFileSubElement_t);
    clientSession.stateBufferIndex = 0;
    clientSession.steps++;
    break;  
#if (gZclEnableOTAClientECCLibrary_d == FALSE)  
  case processCRC_c:
    //check the CRC Value;
    FLib_MemCpy(&crcReceived, clientSession.pStateBuffer, sizeof(crcReceived));
    crcReceived = OTA2Native32(crcReceived);
    if(crcReceived != mCrcCompute)
        return gZclOTAInvalidImage_c; 	  
#else
  case processECDSASigning_c:
    //Verify ECDSA signature
    if(OtaSignatureVerification((uint8_t*)&clientSession.pStateBuffer[0], mCertReceived, (uint8_t*)&clientSession.pStateBuffer[sizeof(zbIeeeAddr_t)])!= gZbSuccess_c)
       return gZclOTAInvalidImage_c;  
#endif  //#if(gEccIncluded_d == FALSE)   
    //We need to close the written image here; commit image has different prototype on ARM7 vs S08
    if(clientSession.steps >= 3)
    {
#if (gBigEndian_c != TRUE)
      if(OTA_CommitImage(FALSE, *(uint32_t *)(pBitmap)) != gOtaSucess_c) return gZclOTAAbort_c;
#else
      if(OTA_CommitImage(pBitmap) != gOtaSucess_c) return gZclOTAAbort_c;
#endif    
      MSG_Free(pBitmap); 
    }
    //Advance to an illegal state. This state machine should not be called again in this upgrade session.
    clientSession.state = stateMax_c;
    clientSession.bytesNeededForState = 1; 
    clientSession.stateBufferIndex = 0;    
    break;
#if (gZclEnableOTAClientECCLibrary_d == TRUE)	  
  case processECDSASignCert_c:  
    //store the certificate
    FLib_MemCpy(&mCertReceived, clientSession.pStateBuffer, sizeof(IdentifyCert_t));
     //Prepare for next state
    clientSession.state = processSubElementTag_c;
    clientSession.bytesNeededForState = sizeof(zclOTAFileSubElement_t);
    clientSession.stateBufferIndex = 0;
    break;    
#endif //#if(gEccIncluded_d == TRUE)    
  case stateMax_c:
  default:
    return gZclOTAAbort_c;
  }  
  return gZbSuccess_c;
}

/******************************************************************************
* OTAClusterClientProcessBlock
*
* Private function. Process a block of received data.
******************************************************************************/
zbStatus_t OTAClusterClientProcessBlock(uint8_t *pImageBlock, uint8_t blockLength)
{
  uint8_t bytesToCopy;
  uint8_t bytesCopied = 0;
  zbStatus_t result = gZbSuccess_c; 
  
#if (gZclEnableOTAClientECCLibrary_d == TRUE) 
  
  uint8_t sizeBlock = 2*AES_MMO_SIZE;
  static uint8_t aesMmoBlock[2*AES_MMO_SIZE];
  static uint8_t mPosition = 0;
  static bool_t lastBlockForHash = FALSE;
  
  if(clientSession.fileLength > (clientSession.currentOffset + blockLength + 2*SECT163K1_SIGNATURE_ELEMENT_LENGTH))
  {
    while(bytesCopied < blockLength)
    {
        lastBlockForHash=FALSE;
        bytesToCopy = sizeBlock - mPosition;
        if(bytesToCopy > (blockLength - bytesCopied))
        {
            bytesToCopy = (blockLength - bytesCopied);
        }
        FLib_MemCpy(aesMmoBlock+mPosition, pImageBlock+bytesCopied, bytesToCopy);
        bytesCopied +=bytesToCopy;
        mPosition+=bytesToCopy;
        if(mPosition == sizeBlock)
        {
              OTAClusterClientAesMMO_hash(aesMmoBlock, sizeBlock, lastBlockForHash);
              mPosition = 0;
        }
      }
  }
  else
  {
    if(lastBlockForHash==FALSE)
    {
      uint8_t *lastAesMmoBlock;
      lastBlockForHash = TRUE;
      bytesToCopy = clientSession.fileLength - clientSession.currentOffset - 2*SECT163K1_SIGNATURE_ELEMENT_LENGTH;
      sizeBlock = mPosition + bytesToCopy;
      lastAesMmoBlock = MSG_Alloc(sizeBlock); 
      FLib_MemCpy(lastAesMmoBlock, aesMmoBlock, mPosition);
      FLib_MemCpy(lastAesMmoBlock+mPosition, pImageBlock, bytesToCopy);
      OTAClusterClientAesMMO_hash(lastAesMmoBlock, sizeBlock, lastBlockForHash);
      mPosition = 0;
      MSG_Free(lastAesMmoBlock);
    }
  }
  
#endif  
  bytesCopied = 0;
  while(bytesCopied < blockLength)
  {
    bytesToCopy = clientSession.bytesNeededForState - clientSession.stateBufferIndex;
    if(bytesToCopy > (blockLength - bytesCopied))
    {
      bytesToCopy = (blockLength - bytesCopied);
    }
    FLib_MemCpy(clientSession.pStateBuffer + clientSession.stateBufferIndex, pImageBlock + bytesCopied, bytesToCopy);
    bytesCopied +=bytesToCopy;
    clientSession.stateBufferIndex+=bytesToCopy;
    if(clientSession.stateBufferIndex == clientSession.bytesNeededForState)
    {
      result = OTAClusterClientRunImageProcessStateMachine();
      if(result != gZbSuccess_c) return result;
    }
  }
 
 #if  (gZclEnableOTAProgressReport_d == TRUE) 
  {
	  static uint8_t mSendReport = 0;
	  if((clientSession.currentOffset > clientSession.fileLength/3) && (clientSession.currentOffset < 2*clientSession.fileLength/3))
		  gOtaProgressReportStatus = otaProgress33_c;
	  else
		  if((clientSession.currentOffset > 2*clientSession.fileLength/3) && (clientSession.currentOffset < clientSession.fileLength))
			  gOtaProgressReportStatus = otaProgress66_c;
	  mSendReport++;
	  if(mSendReport%2 == 0)
	  {
		  mSendReport = 0;
		  BeeAppUpdateDevice(0x08, gOTAProgressReportEvent_c, 0, 0, NULL);
	  }
  }	
#endif  
   
#if (gZclEnableOTAProgressReportToExternalApp_d == TRUE)  
  {
    zclZtcImageOtaProgressReport_t ztcOtaProgressReportInf;
    if((clientSession.currentOffset == 0)||((clientSession.currentOffset+blockLength)%(blockLength*100) == 0)||(clientSession.currentOffset+blockLength ==  clientSession.fileLength))
    {
      ztcOtaProgressReportInf.currentOffset = Native2OTA32(clientSession.currentOffset + blockLength);
      ztcOtaProgressReportInf.imageLength = Native2OTA32(clientSession.fileLength);
      Copy2Bytes(ztcOtaProgressReportInf.deviceAddr, NlmeGetRequest(gNwkShortAddress_c));   
#ifndef gHostApp_d  
        ZTCQueue_QueueToTestClient((const uint8_t*)&ztcOtaProgressReportInf, gHaZtcOpCodeGroup_c, gOTAImageProgressReport_c, sizeof(zclZtcImageOtaProgressReport_t));
#else
        ZTCQueue_QueueToTestClient(gpHostAppUart, (const uint8_t*)&ztcOtaProgressReportInf, gHaZtcOpCodeGroup_c, gOTAImageProgressReport_c, sizeof(zclZtcImageOtaProgressReport_t));
#endif 
    }
  }
#endif   //gZclEnableOTAProgressReportToExternApp_d
  return result;
}


/******************************************************************************
* OTAClusterClientProcessBlockTimerCallback
*
* Timer callback to process block indications
******************************************************************************/
static void OTAClusterClientProcessBlockTimerCallback(uint8_t tmr) 
{
  zbStatus_t          result = gZclUnsupportedClusterCommand_c;    
  (void) tmr;
                                                               
  result = OTAClusterClientProcessBlock(gpBlockCallbackState->blockData, gpBlockCallbackState->blockSize);
  
  if(result != gZbSuccess_c)
  {
     (void)ZCL_OTAClusterClient_EndUpgradeAbortRequest(gpBlockCallbackState->dstAddr,  gpBlockCallbackState->dstEndpoint, result);
     return;
  }
  //Update the transfer info
  clientSession.currentOffset += gpBlockCallbackState->blockSize;
#if gZclClusterOptionals_d
  gOTAAttrs.FileOffset = Native2OTA32(clientSession.currentOffset);
#endif
  
  TMR_FreeTimer(gBlockCallbackTimer);
  
  if(clientSession.currentOffset < clientSession.fileLength)
  {
    //More data to be received - send back a block request
    (void)ZCL_OTAClusterClient_NextBlockRequest(gpBlockCallbackState->dstAddr, gpBlockCallbackState->dstEndpoint);
  }
  else 
  {
    //Save relevant data on the new image
    clientParams.downloadedFileVersion = clientSession.downloadingFileVersion;
#if gZclClusterOptionals_d
    gOTAAttrs.DownloadedFileVersion = Native2OTA32(clientSession.downloadingFileVersion);
#endif    
    gOTAAttrs.ImageUpgradeStatus = gOTAUpgradeStatusDownloadComplete_c;
    TMR_FreeTimer(gRetransmitLastBlockTimer); 
    if(mMultipleUpgradeImage != gZclOTARequireMoreImage_c)
      //All image data received. Issue upgrade end request.
      (void)ZCL_OTAClusterClient_EndUpgradeRequest(gpBlockCallbackState->dstAddr, gpBlockCallbackState->dstEndpoint, gZclSuccess_c);    
    else
    {
      (void)ZCL_OTAClusterClient_EndUpgradeRequest(gpBlockCallbackState->dstAddr, gpBlockCallbackState->dstEndpoint, gZclOTARequireMoreImage_c);  
      gOTAAttrs.ImageUpgradeStatus = gOTAUpgradeStatusWaitForMore_c;
      //initiate new client session: init client params;
      mMultipleUpgradeImage = 0x00;
      gNextImageReqTimer = TMR_AllocateTimer(); 
      if(gNextImageReqTimer == gTmrInvalidTimerID_c)
    	return;	
      //wait 2 second and start new ota upgrade image process
      TMR_StartSecondTimer(gNextImageReqTimer, 2, OTAClusterClientNextImageReqTimerCallback);
    }
  }

 // MSG_Free(gpBlockCallbackState);             
 }

static void OTAClientRetransmitLastBlockTmrCallback(uint8_t tmr) 
{
	zbClusterId_t aClusterId={gaZclClusterOTA_c}; 
	uint8_t aNwkAddrCpy[2];
	uint8_t iEEEAddress[8];
	
	(void)tmr;
#if gNum_EndPoints_c != 0  	
    (void)ZCL_GetAttribute(endPointList[0].pEndpointDesc->pSimpleDesc->endPoint, aClusterId, gZclAttrOTA_UpgradeServerIdId_c, iEEEAddress, NULL);
    (void)APS_GetNwkAddress(iEEEAddress, aNwkAddrCpy);
    (void)ZCL_OTAClusterClient_NextBlockRequest(aNwkAddrCpy, endPointList[0].pEndpointDesc->pSimpleDesc->endPoint);  
#endif 
}

#endif

#if (gZclEnableOTAClientECCLibrary_d == TRUE)	

/******************************************************************************
* aesHashNextBlock
*
* This function computes the AES-128 of the <tt>input</tt> parameter using 
* the Key <tt>keyInit</tt>, and stores the result in <tt>output</tt>.
******************************************************************************/
void aesHashNextBlock(uint8_t* input, uint8_t *keyInit, uint8_t* output)
{
  uint8_t i=0;
  uint32_t tempIn[AES_MMO_SIZE/4],tempKey[AES_MMO_SIZE/4], tempOut[AES_MMO_SIZE/4];
  for(i=0;i<AES_MMO_SIZE/4;i++){
    FLib_MemCpyReverseOrder(&tempIn[i], &input[i*4], AES_MMO_SIZE/4);
    FLib_MemCpyReverseOrder(&tempKey[i], &keyInit[i*4], AES_MMO_SIZE/4);
    FLib_MemCpyReverseOrder(&tempOut[i], &output[i*4], AES_MMO_SIZE/4);
  }
  Asm_CbcMacEncryptDecrypt(tempKey, NULL, tempIn, tempOut);
  for(i=0;i<AES_MMO_SIZE/4;i++)
    FLib_MemCpyReverseOrder(&output[i*4], &tempOut[i], AES_MMO_SIZE/4);
  for(i=0; i<AES_MMO_SIZE; i++)
    output[i] ^=input[i];
}

/******************************************************************************
* OTAClusterAesMMO_hash
*
* This function computes the AES MMO hash for OTA Application including also 
* the signature tag, signer IEEE for image and stores the result in <tt>output</tt>.
******************************************************************************/
#if gZclEnableOTAClient_d
void OTAClusterClientAesMMO_hash(uint8_t *blockToProcess, uint8_t length, bool_t lastBlock)
{
  uint8_t temp[AES_MMO_SIZE];
  uint8_t moreDataLength = length;
  
  for (;AES_MMO_SIZE <= moreDataLength; 
        blockToProcess += AES_MMO_SIZE, moreDataLength -= AES_MMO_SIZE)
      aesHashNextBlock(blockToProcess, mMsgDigest, mMsgDigest);
  
  if(lastBlock == TRUE){
      FLib_MemSet(&temp[0], 0x00, AES_MMO_SIZE);
      FLib_MemCpy(&temp[0], &blockToProcess[0], moreDataLength);
      temp[moreDataLength] = 0x80;
    
      if (AES_MMO_SIZE - moreDataLength < 3) {
        aesHashNextBlock(temp, mMsgDigest, mMsgDigest);
        FLib_MemSet(&temp[0], 0x00, AES_MMO_SIZE);
      }
      temp[AES_MMO_SIZE - 2] = (uint8_t)((clientSession.fileLength-2*SECT163K1_SIGNATURE_ELEMENT_LENGTH)>> 5);
      temp[AES_MMO_SIZE - 1] = (uint8_t)((clientSession.fileLength-2*SECT163K1_SIGNATURE_ELEMENT_LENGTH)<< 3);
      aesHashNextBlock(temp, mMsgDigest, mMsgDigest); 
  }
  
}

extern const uint8_t CertAuthPubKey[gZclCmdKeyEstab_CompressedPubKeySize_c];
extern const uint8_t CertAuthIssuerID[8];
zbStatus_t OtaSignatureVerification(uint8_t *signerIEEE,IdentifyCert_t certificate, uint8_t* signature)
{
   uint8_t devicePublicKey_rec[gZclCmdKeyEstab_CompressedPubKeySize_c];
   uint8_t signerIEEE_reverse[8];
   zbIeeeAddr_t signersIEEEList = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01};
   uint8_t i=0;
   
   for(i=0;i<8;i++)
      signerIEEE_reverse[i] = signerIEEE[8-i-1];
   /*first Step - determine if the signer of the image is an authorized signer*/
   if(Cmp8Bytes(&signersIEEEList[0], &signerIEEE_reverse[0])==FALSE)
     return gZbFailed_c;
   /*Verify the signer IEEE address within the subject field of the ECDSA Certificate */
   if(Cmp8Bytes(&certificate.Subject[0], &signerIEEE_reverse[0])==FALSE)
     return gZbFailed_c;
   /*Test the issuer field of the ECDSA Certificate with a specific list*/
   if(Cmp8Bytes(&certificate.Issuer[0], (uint8_t*)&CertAuthIssuerID[0])==FALSE)
     return gZbFailed_c;
   /*reconstruct public key using certificate and specific CertAuthPubKey*/
   ZSE_ECQVReconstructPublicKey((uint8_t *)&certificate,(uint8_t *)CertAuthPubKey, devicePublicKey_rec,aesMmoHash, NULL, 0);
   /*ECDSA Verification*/
   if(ZSE_ECDSAVerify(devicePublicKey_rec, mMsgDigest,  &signature[0], &signature[SECT163K1_SIGNATURE_ELEMENT_LENGTH],NULL,0)!=gZbSuccess_c)
     return gZbFailed_c;
   /*if all ok return gZbSuccess_c*/
   return gZbSuccess_c;
}

#endif //gZclEnableOTAClient_d

#endif //gEccIncluded_d


zbStatus_t OTA_InitiateOtaClusterProcess 
(
    zclInitiateOtaProcess_t* initiateOtaProcess
)
{
#if (gZclEnableOTAClient_d)
#if gNum_EndPoints_c != 0     
  uint8_t i = 0;
  otaCluster_simpleDescriptor.endPoint =  initiateOtaProcess->clientInit.endPoint;     
  for(i=0; i< gNum_EndPoints_c; ++i) 
  {
      if(endPointList[i].pEndpointDesc->pSimpleDesc->endPoint == initiateOtaProcess->clientInit.endPoint) 
         Copy2Bytes(&otaCluster_simpleDescriptor.aAppProfId, endPointList[i].pEndpointDesc->pSimpleDesc->aAppProfId);     
  }  
  /*wait 1 second to send Match_Desc_req*/
  TMR_FreeTimer(gNextImageReqTimer);
  gNextImageReqTimer = TMR_AllocateTimer();	
  if(gNextImageReqTimer == gTmrInvalidTimerID_c)
     	return FALSE;
  TMR_StartSecondTimer(gNextImageReqTimer, 1, OTAClusterClientMatchDescReqTimerCallback);

#endif  //gNum_EndPoints_c
#endif  //gZclEnableOTAClient_d
#if gZclEnableOTAServer_d    
  mOtaServerInitParams.querryJitter = initiateOtaProcess->serverInit.querryJitter;
  mOtaServerInitParams.currentTime = OTA2Native32(initiateOtaProcess->serverInit.currentTime); 
  mOtaServerInitParams.upgradeRequestTime = OTA2Native32(initiateOtaProcess->serverInit.upgradeRequestTime);
  mOtaServerInitParams.upgradeTime = OTA2Native32(initiateOtaProcess->serverInit.upgradeTime); 
#endif
 return gZbSuccess_c;
}

#if gZclEnableOTAClient_d 

bool_t InterpretOtaMatchDescriptor(zbNwkAddr_t  aDestAddress, uint8_t endPoint)
{

  if(mSendOtaMatchDescriptorRequest == TRUE)
  {
    gOTAAttrs.ImageUpgradeStatus = gOTAUpgradeStatusNormal_c;
#if gZclClusterOptionals_d
    gOTAAttrs.FileOffset = 0;
    gOTAAttrs.CurrentZigBeeStackVersion = NlmeGetRequest(gNwkStackProfile_c);
    gOTAAttrs.DownloadedZigBeeStackVersion = NlmeGetRequest(gNwkStackProfile_c);
#endif
    /*Send IEEE_Addr_Req*/
    mSendOtaMatchDescriptorRequest = FALSE;
    ASL_IEEE_addr_req(NULL, aDestAddress, aDestAddress, 0x00, 0x00);
    mSendOTAIEEEAddrRequest = TRUE;
    return TRUE;
  }
  return FALSE;
}

void InterpretOtaIEEEAddrReq(zbIeeeAddr_t  aIeeeAddr)
{
	if(mSendOTAIEEEAddrRequest == TRUE)
	{
	    FLib_MemCpy(gOTAAttrs.UpgradeServerId, aIeeeAddr, 8);
	    mSendOTAIEEEAddrRequest = FALSE;
	    gNextImageReqTimer = TMR_AllocateTimer(); 
	    if(gNextImageReqTimer == gTmrInvalidTimerID_c)
	    	return;	
	    TMR_StartMinuteTimer(gNextImageReqTimer, gMinforNextImageRequest_c, OTAClusterClientNextImageReqTimerCallback);  
	}
}


/******************************************************************************
ZCL_StartClientNextImageTransfer - used only if you discovered a valid OTA Server
*****************************************************************************/
zbStatus_t OTA_OtaStartClientNextImageTransfer 
(
    zclStartClientNextImageTransfer_t* startClientNextImageTransfer
)
{
  uint8_t aNwkAddrCpy[2];
  zbStatus_t status = gZbSuccess_c;
 
  /* required EndRequest Status*/
  mMultipleUpgradeImage = startClientNextImageTransfer->multipleUpgradeImage;
  /* is valid upgradeServerId attribute? */
  if(Cmp8BytesToFs(gOTAAttrs.UpgradeServerId))
  {
#if gNum_EndPoints_c != 0   
      zclInitiateOtaProcess_t initServerDiscovery = {0, 0};
      initServerDiscovery.isServer = FALSE;
      initServerDiscovery.clientInit.endPoint = endPointList[0].pEndpointDesc->pSimpleDesc->endPoint;
      (void)OTA_InitiateOtaClusterProcess(&initServerDiscovery);
#endif
	  return gZbFailed_c;
  }
  (void)APS_GetNwkAddress(gOTAAttrs.UpgradeServerId, aNwkAddrCpy);
  /* send a Query Next Image Request */
#if gNum_EndPoints_c != 0 	    
  status = ZCL_OTAClusterClient_NextImageRequest(aNwkAddrCpy, endPointList[0].pEndpointDesc->pSimpleDesc->endPoint);
#endif   
  return status;
}

static void OTAClusterClientNextImageReqTimerCallback(uint8_t tmr) 
{
  zbClusterId_t aClusterId={gaZclClusterOTA_c}; 
  uint8_t aNwkAddrCpy[2];
  uint8_t iEEEAddress[8];
  
  (void)tmr;
#if gNum_EndPoints_c != 0    
    (void)ZCL_GetAttribute(endPointList[0].pEndpointDesc->pSimpleDesc->endPoint, aClusterId, gZclAttrOTA_UpgradeServerIdId_c, iEEEAddress, NULL);
    (void)APS_GetNwkAddress(iEEEAddress, aNwkAddrCpy);
    (void)ZCL_OTAClusterClient_NextImageRequest(aNwkAddrCpy, endPointList[0].pEndpointDesc->pSimpleDesc->endPoint);  
#endif      
    TMR_StartMinuteTimer(gNextImageReqTimer, gMinforNextImageRequest_c, OTAClusterClientNextImageReqTimerCallback);
}

static void OTAClusterClientMatchDescReqTimerCallback(uint8_t tmr) 
{
  zbNwkAddr_t  aDestAddress = {0xFD, 0xFF};
  (void)tmr;

  ASL_MatchDescriptor_req(NULL,(uint8_t *)aDestAddress, (zbSimpleDescriptor_t*)&otaCluster_simpleDescriptor); 
  mSendOtaMatchDescriptorRequest = TRUE;
  TMR_FreeTimer(gNextImageReqTimer);
}

#endif //gZclEnableOTAClient_d

#endif
