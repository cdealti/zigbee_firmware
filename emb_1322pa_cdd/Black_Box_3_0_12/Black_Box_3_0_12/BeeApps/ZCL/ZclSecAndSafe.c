/******************************************************************************
* ZclSecAndSafe.c
*
* This module contains code that handles ZigBee Cluster Library commands and
* clusters for the Security and safety clusters.
*
* Copyright (c) 2009, Freescale, Inc.  All rights reserved.
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
#include "ApsMgmtInterface.h"

#include "zcl.h"
#include "ZclSecAndSafe.h"
#include "HcProfile.h"


/******************************************************************************
*******************************************************************************
* Private macros
*******************************************************************************
******************************************************************************/

/******************************************************************************
*******************************************************************************
* Private prototypes
*******************************************************************************
******************************************************************************/

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
  IAS Zone Cluster 
  See ZCL Specification Section 8.2
*******************************/
/* IAS Zone Cluster Attribute Definitions */
const zclAttrDef_t gaZclIASZoneClusterAttrDef[] = {
  /*Attributes of the Zone Information attribute set */
  { gZclAttrZoneInformationZoneStateSet_c,  gZclDataTypeEnum8_c,   gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(uint8_t),  (void *) MbrOfs(zclIASZoneAttrsRAM_t, zoneState)},
  { gZclAttrZoneInformationZoneTypeSet_c,   gZclDataTypeEnum16_c,  gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(uint16_t), (void *) MbrOfs(zclIASZoneAttrsRAM_t, zoneType)},
  { gZclAttrZoneInformationZoneStatusSet_c, gZclDataTypeBitmap16_c,gZclAttrFlagsInRAM_c|gZclAttrFlagsRdOnly_c, sizeof(uint16_t), (void *) MbrOfs(zclIASZoneAttrsRAM_t, zoneStatus)},
  /*Attributes of the Zone Settings attribute set */
  { gZclAttrZoneSettingsIASCIEAddress_c, gZclDataTypeIeeeAddr_c,   gZclAttrFlagsInRAM_c, sizeof(IEEEaddress_t), (void *) MbrOfs(zclIASZoneAttrsRAM_t, IASCIEAddress)}
};

const zclAttrDefList_t gZclIASZoneClusterAttrDefList = {
  NumberOfElements(gaZclIASZoneClusterAttrDef),
  gaZclIASZoneClusterAttrDef
};

/******************************
  IAS ACE Cluster 
  See ZCL Specification Section 8.3
*******************************/
/*No attributes are defined for this cluster.*/


/******************************
  IAS Warning Device Cluster 
  See ZCL Specification Section 8.4
*******************************/
/* IAS Warning Device Cluster Attribute Definitions */
const zclAttrDef_t gaZclIASWDClusterAttrDef[] = {
  { gZclAttrIASWDMaxDuration_c, gZclDataTypeUint16_c,  gZclAttrFlagsInRAM_c, sizeof(uint16_t), (void *) MbrOfs(zclIASWDAttrsRAM_t, maxDuration)},
  };

const zclAttrDefList_t gZclIASWDClusterAttrDefList = {
  NumberOfElements(gaZclIASWDClusterAttrDef),
  gaZclIASWDClusterAttrDef
};


/******************************
  IAS Zone Cluster 
  See ZCL Specification Section 8.2
*******************************/

/*****************************************************************************/
zbStatus_t ZCL_IASZoneClusterClient
(
    zbApsdeDataIndication_t *pIndication, 
    afDeviceDef_t *pDev
)
{
    zclFrame_t *pFrame;
    zclCmdIASZone_ZoneEnrollRequest_t Command;
    zclIASZone_ZoneEnrollResponse_t *pReq;
    uint8_t event = 0, i = 0, j = 0;
    uint8_t *pIEEE, aExtAddr[8];
    #define ZoneTypeID 13
    uint16_t mZoneType[ZoneTypeID] = 
    {
      gZclZoneType_StandardCIE_c, gZclZoneType_MotionSensor_c, gZclZoneType_ContactSwitch_c,
      gZclZoneType_FireSensor_c, gZclZoneType_WaterSensor_c, gZclZoneType_GasSensor_c,
      gZclZoneType_PersonalEmergencyDevice_c, gZclZoneType_VibrationMovement_c,
      gZclZoneType_RemoteControl_c, gZclZoneType_KeyFob_c, gZclZoneType_Keypad_c,
      gZclZoneType_StandardWarning_c, gZclZoneType_InvalidZone_c
    };
    
    /* prevent compiler warning */
    (void)pDev;
    pFrame = (void *)pIndication->pAsdu;   
    /* handle the command */
    switch( pFrame->command)
    {
        case gZclCmdTxIASZone_ZoneStatusChange_c:
           FLib_MemCpy(&gChageNotif ,(pFrame + 1), sizeof(zclCmdIASZone_ZoneEnrollRequest_t));           
           event = gZclUI_ChangeNotification;
           return gZbSuccess_c;
        case gZclCmdTxIASZone_ZoneEnrollRequest_c:      
           FLib_MemCpy(&Command ,(pFrame + 1), sizeof(zclCmdIASZone_ZoneEnrollRequest_t));
           pReq = MSG_Alloc(sizeof(zclIASZone_ZoneEnrollResponse_t)); 
           pReq->addrInfo.dstAddrMode = gZbAddrMode16Bit_c;
           Copy2Bytes(pReq->addrInfo.dstAddr.aNwkAddr, pIndication->aSrcAddr); 
           pReq->addrInfo.dstEndPoint = pIndication->dstEndPoint;
           pReq->addrInfo.srcEndPoint = pIndication->srcEndPoint;
           pReq->addrInfo.txOptions = 0;
           pReq->addrInfo.radiusCounter = afDefaultRadius_c;
           pIEEE = APS_GetIeeeAddress((uint8_t*)&pIndication->aSrcAddr, aExtAddr);
           /* verify if the IEEEAddress exist already in table */
           for(i=0; i <= gIndexTable; i++)
           {
             if(FLib_MemCmp(&gTableZone[i].ZoneAddress, pIEEE, 8) == TRUE)
             {
                pReq->cmdFrame.ZoneID = gTableZone[gIndexTable].ZoneID;
                pReq->cmdFrame.EnrollResponseCode =  gEnrollResponseCode_Succes_c;
                event = gZclUI_EnrollSuccesfull_c; 
                break;
             }
             else
             {
                if(i == gIndexTable)
                {
                  pReq->cmdFrame.ZoneID = gIndexTable;
                  /* verify the index in the zone table */
                  if(gIndexTable <= MaxZones)
                  {
                      /*verify the zone_type*/
                      for(j=0; j< ZoneTypeID; j++)
                      
                        if(mZoneType[j] == Command.ZoneType)
                        {
                            pReq->cmdFrame.EnrollResponseCode =  gEnrollResponseCode_Succes_c;
                            /*store the device in the gZoneTable*/
                            gTableZone[gIndexTable].ZoneID = gIndexTable;
                            gTableZone[gIndexTable].ZoneType = Command.ZoneType;
                            //FLib_MemCpy(&gTableZone[gIndexTable].ZoneAddress, pIEEE, 8);     
                            Copy8Bytes((uint8_t *)&gTableZone[gIndexTable].ZoneAddress, aExtAddr);
                            /*update index for table zone*/
                            gIndexTable++;
                            event = gZclUI_EnrollSuccesfull_c; 
                            break;
                        }
                        else
                        {
                            if(j == ZoneTypeID - 1)
                            {
                                pReq->cmdFrame.EnrollResponseCode =  gEnrollResponseCode_NoEnrollPermit_c;
                                event = gZclUI_EnrollFailed_c; 
                            }
                        }
                    
                  }            
                  else
                  {
                      pReq->cmdFrame.EnrollResponseCode = gEnrollResponseCode_TooManyZones_c; 
                      event = gZclUI_EnrollFailed_c; 
                  }
                  break;
              }
           }
           }
           (void) IASZone_ZoneEnrollResponse(pReq);       
           BeeAppUpdateDevice(pIndication->dstEndPoint, event, 0, 0, NULL);
           MSG_Free(pReq);       
           return gZbSuccess_c;
        default:
          return gZclUnsupportedClusterCommand_c;
    }
}

/*****************************************************************************/
zbStatus_t ZCL_IASZoneClusterServer
(
    zbApsdeDataIndication_t *pIndication, 
    afDeviceDef_t *pDev
)
{
    zclFrame_t *pFrame;
    uint8_t event = gZclUI_NoEvent_c;
    zclCmdIASZone_ZoneEnrollResponse_t Command;
    zbClusterId_t aClusterId;
    uint8_t state;

    /* prevent compiler warning */
    (void)pDev;
    pFrame = (void *)pIndication->pAsdu;
    Copy2Bytes(aClusterId, pIndication->aClusterId);
    FLib_MemCpy(&Command ,(pFrame + 1), sizeof(zclCmdIASZone_ZoneEnrollResponse_t)); 
    switch( pFrame->command)
    {
        case gZclCmdRxIASZone_ZoneEnrollResponse_c:    
          if(Command.EnrollResponseCode == gZclSuccess_c)
          {
            event = gZclUI_EnrollSuccesfull_c;
            state = gZclZoneState_Enrolled_c;
            (void)ZCL_SetAttribute(pIndication->dstEndPoint, aClusterId, gZclAttrZoneInformationZoneStateSet_c, &state);
          }
          else 
          {
            event = gZclUI_EnrollFailed_c;
            state = gZclZoneState_NotEnrolled_c;
            (void)ZCL_SetAttribute(pIndication->dstEndPoint, aClusterId, gZclAttrZoneInformationZoneStateSet_c, &state);
          }
          BeeAppUpdateDevice(0, event, 0, 0, NULL);
          return gZbSuccess_c;
        default:
          return gZclUnsupportedClusterCommand_c;
    }

}

/*****************************************************************************/
zbStatus_t IASZone_ZoneStatusChange
(
    zclIASZone_ZoneStatusChange_t *pReq
)
{
    pReq->zclTransactionId = gZclTransactionId++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterIASZone_c);	
    return ZCL_SendServerReqSeqPassed(gZclCmdTxIASZone_ZoneStatusChange_c, sizeof(zclCmdIASZone_ZoneStatusChange_t),(zclGenericReq_t *)pReq);
}


void ZCL_SendStatusChangeNotification(zbEndPoint_t DstEndpoint, zbEndPoint_t SrcEndpoint, uint16_t zoneStatusChangeNotif) 
{
    zclIASZone_ZoneStatusChange_t *pReq;
    zbClusterId_t aClusterId={gaZclClusterSecurityZone_c}; 
    //uint8_t  *aDstAddr;
    uint8_t aNwkAddrCpy[2];
    uint8_t iEEEAddress[8];
    pReq = MSG_Alloc(sizeof(zclIASZone_ZoneStatusChange_t)); 
     
    if(pReq) {
        pReq->addrInfo.dstAddrMode = gZbAddrMode16Bit_c;
        (void)ZCL_GetAttribute(SrcEndpoint, aClusterId, gZclAttrZoneSettingsIASCIEAddress_c, iEEEAddress, NULL);
        (void)APS_GetNwkAddress(iEEEAddress, aNwkAddrCpy);
        Copy2Bytes(pReq->addrInfo.dstAddr.aNwkAddr, aNwkAddrCpy);
        pReq->addrInfo.dstEndPoint = DstEndpoint;
        pReq->addrInfo.srcEndPoint = SrcEndpoint;
        pReq->addrInfo.txOptions = 0;
        pReq->addrInfo.radiusCounter = afDefaultRadius_c;
        pReq->cmdFrame.ZoneStatus = zoneStatusChangeNotif;
        pReq->cmdFrame.ExtendedStatus = 0x00;
        (void)ZCL_SetAttribute(SrcEndpoint, aClusterId, gZclAttrZoneInformationZoneStatusSet_c, &zoneStatusChangeNotif);
        
        (void)IASZone_ZoneStatusChange(pReq);
        MSG_Free(pReq);
    }
}
/*****************************************************************************/
zbStatus_t IASZone_ZoneEnrollRequest
(
    zclIASZone_ZoneEnrollRequest_t *pReq
)
{ 
    pReq->zclTransactionId = gZclTransactionId++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterIASZone_c); 
    return ZCL_SendServerReqSeqPassed(gZclCmdTxIASZone_ZoneEnrollRequest_c,sizeof(zclCmdIASZone_ZoneEnrollRequest_t), (zclGenericReq_t *)pReq);
}


void ZCL_SendZoneEnrollRequest(zbEndPoint_t DstEndpoint, zbEndPoint_t SrcEndpoint,  zbNwkAddr_t DstAddr) 
{
    zclIASZone_ZoneEnrollRequest_t *pReq;
    uint16_t zoneType;
    uint8_t aExtAddr[8];
    zbClusterId_t aClusterId={gaZclClusterSecurityZone_c}; 
   
    pReq = MSG_Alloc(sizeof(zclIASZone_ZoneEnrollRequest_t)); 
     
    if(pReq) {
        (void)APS_GetIeeeAddress(DstAddr, aExtAddr);
        (void)ZCL_SetAttribute(SrcEndpoint, aClusterId, gZclAttrZoneSettingsIASCIEAddress_c, aExtAddr);
        pReq->addrInfo.dstAddrMode = gZbAddrMode16Bit_c;
        Copy2Bytes(pReq->addrInfo.dstAddr.aNwkAddr, DstAddr);
        pReq->addrInfo.dstEndPoint = DstEndpoint;
        pReq->addrInfo.srcEndPoint = SrcEndpoint;
        pReq->addrInfo.txOptions = 0;
        pReq->addrInfo.radiusCounter = afDefaultRadius_c;
        (void)ZCL_GetAttribute(SrcEndpoint, aClusterId, gZclAttrZoneInformationZoneTypeSet_c , &zoneType, NULL);
        pReq->cmdFrame.ZoneType = zoneType;
        pReq->cmdFrame.ManufacturerCode = 0x210;
#if (TRUE == gBigEndian_c)
        pReq->cmdFrame.ManufacturerCode = Swap2Bytes( pReq->cmdFrame.ManufacturerCode);
#endif
        (void) IASZone_ZoneEnrollRequest(pReq);
        
        MSG_Free(pReq);
    }
}


/*****************************************************************************/
zbStatus_t IASZone_ZoneEnrollResponse
(
    zclIASZone_ZoneEnrollResponse_t *pReq
)  
{
    pReq->zclTransactionId = gZclTransactionId++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterIASZone_c);	
    return ZCL_SendClientRspSeqPassed(gZclCmdRxIASZone_ZoneEnrollResponse_c,sizeof(zclCmdIASZone_ZoneEnrollResponse_t),(zclGenericReq_t *)pReq);
}



/******************************
  IAS ACE Cluster 
  See ZCL Specification Section 8.3
*******************************/

/*****************************************************************************/
zbStatus_t ZCL_IASACEClusterClient
(
    zbApsdeDataIndication_t *pIndication, 
    afDeviceDef_t *pDev
)
{
    zclFrame_t *pFrame;
    zclCmd_t command;
    zclCmdIASACE_ArmRsp_t commandArmRsp;
    /* prevent compiler warning */
    (void)pDev;
    pFrame = (void *)pIndication->pAsdu;
    /* handle the command */
    command = pFrame->command;  
    switch(command)
    {
       case gZclCmdTxIASACE_ArmRsp_c:
          FLib_MemCpy(&commandArmRsp ,(pFrame + 1), sizeof(zclCmdIASACE_ArmRsp_t)); 
          /*Verify if the Arm_Notification = gArmNotif_AllZonesArmed_c if send a all zones armed command */
          if(commandArmRsp.ArmNotification == gArmNotif_AllZonesArmed_c)         
            return gZbSuccess_c;
          else
            return gZbFailed_c;
       case gZclCmdTxIASACE_GetZoneIDMApRsp_c:      
       case gZclCmdTxIASACE_GetZoneInfRsp_c:
         return gZbSuccess_c;
       default:
          return gZclUnsupportedClusterCommand_c;
    }

}

/*****************************************************************************/
zbStatus_t ZCL_IASACEClusterServer
(
    zbApsdeDataIndication_t *pIndication, 
    afDeviceDef_t *pDev
)
{
    zclFrame_t *pFrame;
    uint8_t Command;
    uint8_t i=0,j=0;
    zclIASACE_ArmRsp_t *pReq;
    zclIASACE_GetZoneIDMApRsp_t *pReqZoneMap;
    zclIASACE_GetZoneInfRsp_t *pReqZoneInf;
    uint8_t event = gZclUI_NoEvent_c;
	WarningModeAndStrobe_t warningStrobe = {0, 0, 0};
    zclCmdIASWD_Squawk_t squawk;
    uint8_t aNwkAddrCpy[2];
    uint8_t pIEEE[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x00, 0x00};
    
    /* prevent compiler warning */
    (void)pDev;
    pFrame = (void *)pIndication->pAsdu;
    /* handle the command */
    switch( pFrame->command)
    {
        case gZclCmdRxIASACE_Arm_c:
           FLib_MemCpy(&Command ,(pFrame + 1), sizeof(zclCmdIASACE_Arm_t));  /*Command = ArmMode */
           pReq = MSG_Alloc(sizeof(zclIASACE_ArmRsp_t)); 
           pReq->addrInfo.dstAddrMode = gZbAddrMode16Bit_c;
           Copy2Bytes(pReq->addrInfo.dstAddr.aNwkAddr,pIndication->aSrcAddr); 
           pReq->addrInfo.dstEndPoint = pIndication->dstEndPoint;
           pReq->addrInfo.srcEndPoint = pIndication->srcEndPoint;
           pReq->addrInfo.txOptions = 0;
           pReq->addrInfo.radiusCounter = afDefaultRadius_c;
           /*send arm command to devices - for moment is not implemented (armmode = arm notification)*/                  
           pReq->cmdFrame.ArmNotification = Command;    
           (void) IASACE_ArmRsp(pReq);       
           for(i=0; i < gIndexTable; i++)
              if(gTableZone[i].ZoneType == gZclZoneType_StandardWarning_c)
              {
                //FLib_MemCpy( pIEEE,&gTableZone[i].ZoneAddress, 8); 
                Copy8Bytes(pIEEE, (uint8_t *)&gTableZone[i].ZoneAddress);
                (void) APS_GetNwkAddress(pIEEE, aNwkAddrCpy);
                squawk.SquawkMode = SquawkMode_SystemArmed;
                squawk.Strobe = STROBE_PARALLEL;
                squawk.SquawkLevel = SquawkLevel_LowLevel;
                ZCL_SendSqawk( 0x08, 0x08, aNwkAddrCpy, squawk);
              }
           BeeAppUpdateDevice(pIndication->dstEndPoint, event, 0, 0, NULL);
           MSG_Free(pReq);       
           return gZbSuccess_c;
           
        case gZclCmdRxIASACE_GetZoneIDMap_c:      
           pReqZoneMap = MSG_Alloc(sizeof(zclIASACE_GetZoneIDMApRsp_t)); 
           pReqZoneMap->addrInfo.dstAddrMode = gZbAddrMode16Bit_c;
           Copy2Bytes(pReqZoneMap->addrInfo.dstAddr.aNwkAddr,pIndication->aSrcAddr); 
           pReqZoneMap->addrInfo.dstEndPoint = pIndication->dstEndPoint;
           pReqZoneMap->addrInfo.srcEndPoint = pIndication->srcEndPoint;
           pReqZoneMap->addrInfo.txOptions = 0;
           pReqZoneMap->addrInfo.radiusCounter = afDefaultRadius_c;
           for(i=0;i<16;i++) /* for  zone section */
           {
             pReqZoneMap->cmdFrame.ZoneIDMapSection[i] = 0x0000;
             for(j=0;j<16;j++)/* for zone bit section */
             {
               if((gIndexTable-1) >= (i*16+j))
                  pReqZoneMap->cmdFrame.ZoneIDMapSection[i] |=  (0x0001 << j); 
               else
                  break;  
             }
#if (TRUE == gBigEndian_c)
               pReqZoneMap->cmdFrame.ZoneIDMapSection[i] = Swap2Bytes(pReqZoneMap->cmdFrame.ZoneIDMapSection[i]);
#endif
           }
           (void) IASACE_GetZoneIDMapRsp(pReqZoneMap);   
           BeeAppUpdateDevice(pIndication->dstEndPoint, event, 0, 0, NULL);
           MSG_Free(pReqZoneMap);       
           return gZbSuccess_c;      
           
        case gZclCmdRxIASACE_GetZoneInformation_c:   
           FLib_MemCpy(&Command ,(pFrame + 1), sizeof(zclCmdIASACE_GetZoneInformation_t));  /*Command = ZoneID */
           pReqZoneInf = MSG_Alloc(sizeof(zclIASACE_GetZoneInfRsp_t)); 
           pReqZoneInf->addrInfo.dstAddrMode = gZbAddrMode16Bit_c;
           Copy2Bytes(pReqZoneInf->addrInfo.dstAddr.aNwkAddr,pIndication->aSrcAddr); 
           pReqZoneInf->addrInfo.dstEndPoint = pIndication->dstEndPoint;
           pReqZoneInf->addrInfo.srcEndPoint = pIndication->srcEndPoint;
           pReqZoneInf->addrInfo.txOptions = 0;
           pReqZoneInf->addrInfo.radiusCounter = afDefaultRadius_c;
           /* obtain zone information from table zone */              
           pReqZoneInf->cmdFrame.ZoneID       =   gTableZone[Command].ZoneID;   
           pReqZoneInf->cmdFrame.ZoneType     =   gTableZone[Command].ZoneType; 
           Copy8Bytes((uint8_t *)&pReqZoneInf->cmdFrame.IEEEAddress, (uint8_t *)&gTableZone[Command].ZoneAddress);
           (void) IASACE_GetZoneInformationRsp(pReqZoneInf);       
           MSG_Free(pReqZoneInf);   
           BeeAppUpdateDevice(pIndication->dstEndPoint, event, 0, 0, NULL);
           return gZbSuccess_c;
           
        case gZclCmdRxIASACE_Bypass_c:
        case gZclCmdRxIASACE_Fire_c:
           return gZbSuccess_c;
        case gZclCmdRxIASACE_Emergency_c:
        case gZclCmdRxIASACE_Panic_c:       
          warningStrobe.WarningMode = 0x3;
          warningStrobe.Strobe = 0x1;
          for(i=0; i < gIndexTable; i++)
            if(gTableZone[i].ZoneType == gZclZoneType_StandardWarning_c)
            {
               Copy8Bytes(pIEEE, (uint8_t *)&gTableZone[i].ZoneAddress);
              (void) APS_GetNwkAddress(pIEEE, aNwkAddrCpy);
              ZCL_SendStartWarning( 0x08, 0x08, aNwkAddrCpy,  warningStrobe, 0x0A); 
            } 
           return gZbSuccess_c;
        default:
           return gZclUnsupportedClusterCommand_c;
    }

}

/*****************************************************************************/                         
zbStatus_t IASACE_Arm
(
    zclIASACE_Arm_t *pReq
)
{
    pReq->zclTransactionId = gZclTransactionId++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterIASACE_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxIASACE_Arm_c, sizeof(zclCmdIASACE_Arm_t),(zclGenericReq_t *)pReq);
}

void ZCL_SendArmCommand(zbEndPoint_t DstEndpoint, zbEndPoint_t SrcEndpoint, zbNwkAddr_t DstAddr, uint8_t armMode) 
{
    zclIASACE_Arm_t *pReq;
    pReq = MSG_Alloc(sizeof(zclIASACE_Arm_t)); 
  
    if(pReq) {
        pReq->addrInfo.dstAddrMode = gZbAddrMode16Bit_c;
        Copy2Bytes(pReq->addrInfo.dstAddr.aNwkAddr, DstAddr);
        pReq->addrInfo.dstEndPoint = DstEndpoint;
        pReq->addrInfo.srcEndPoint = SrcEndpoint;
        pReq->addrInfo.txOptions = 0;
        pReq->addrInfo.radiusCounter = afDefaultRadius_c;
        pReq->cmdFrame.ArmMode = armMode;
        
        (void) IASACE_Arm(pReq);
        MSG_Free(pReq);
    }
}

zbStatus_t IASACE_ArmRsp
(
    zclIASACE_ArmRsp_t *pReq
)
{
    pReq->zclTransactionId = gZclTransactionId++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterIASACE_c);	
    return ZCL_SendServerRspSeqPassed(gZclCmdTxIASACE_ArmRsp_c, sizeof(zclCmdIASACE_ArmRsp_t),(zclGenericReq_t *)pReq);
}

/*****************************************************************************/
zbStatus_t IASACE_Bypass
(
    zclIASACE_Bypass_t *pReq
)
{
    pReq->zclTransactionId = gZclTransactionId++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterIASACE_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxIASACE_Bypass_c,((1 + pReq->cmdFrame.NumberOfZones)),(zclGenericReq_t *)pReq);
}


void ZCL_SendBypassCommand(zbEndPoint_t DstEndpoint, zbEndPoint_t SrcEndpoint, zbNwkAddr_t DstAddr, uint8_t NrOfZones, uint8_t *pZoneId) 
{
    zclIASACE_Bypass_t *pReq;
    uint8_t i=0;
    
    pReq = AF_MsgAlloc(); 
    
    if(pReq) {
        pReq->addrInfo.dstAddrMode = gZbAddrMode16Bit_c;
        Copy2Bytes(pReq->addrInfo.dstAddr.aNwkAddr, DstAddr);
        pReq->addrInfo.dstEndPoint = DstEndpoint;
        pReq->addrInfo.srcEndPoint = SrcEndpoint;
        pReq->addrInfo.txOptions = 0;
        pReq->addrInfo.radiusCounter = afDefaultRadius_c;
        pReq->cmdFrame.NumberOfZones = NrOfZones;
        for(i=0; i < NrOfZones; i++)
          pReq->cmdFrame.pZoneId[i] = pZoneId[i];
        
        (void) IASACE_Bypass(pReq);
         MSG_Free(pReq);
    }
}

/*****************************************************************************/
/*for Emergency, Panic, Fire Command  */
void ZCL_SendEFPCommand(zbEndPoint_t DstEndpoint, zbEndPoint_t SrcEndpoint, zbNwkAddr_t DstAddr, uint8_t CommandEFP) 
{
    zclIASACE_EFP_t *pReq;
    
    pReq = MSG_Alloc(sizeof(zclIASACE_EFP_t)); 
     
    if(pReq) {
        pReq->addrInfo.dstAddrMode = gZbAddrMode16Bit_c;
        Copy2Bytes(pReq->addrInfo.dstAddr.aNwkAddr, DstAddr);
        pReq->addrInfo.dstEndPoint = DstEndpoint;
        pReq->addrInfo.srcEndPoint = SrcEndpoint;
        pReq->addrInfo.txOptions = 0;
        pReq->addrInfo.radiusCounter = afDefaultRadius_c;
        Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterIASACE_c);	
        pReq->zclTransactionId = gZclTransactionId++;
    
        (void)ZCL_SendClientReqSeqPassed(CommandEFP,0,(zclGenericReq_t *)pReq);
        MSG_Free(pReq);
    }
}

zbStatus_t IASACE_Emergency
(
    zclIASACE_EFP_t *pReq
)
{
    pReq->zclTransactionId = gZclTransactionId++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterIASACE_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxIASACE_Emergency_c,0,(zclGenericReq_t *)pReq);
}

zbStatus_t IASACE_Fire
(
    zclIASACE_EFP_t *pReq
)
{
    pReq->zclTransactionId = gZclTransactionId++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterIASACE_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxIASACE_Fire_c,0,(zclGenericReq_t *)pReq);
}

zbStatus_t IASACE_Panic
(
    zclIASACE_EFP_t *pReq
)
{
    pReq->zclTransactionId = gZclTransactionId++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterIASACE_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxIASACE_Panic_c,0,(zclGenericReq_t *)pReq);
}

zbStatus_t IASACE_GetZoneIDMap
(
    zclIASACE_EFP_t *pReq
)
{
    pReq->zclTransactionId = gZclTransactionId++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterIASACE_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxIASACE_GetZoneIDMap_c,0,(zclGenericReq_t *)pReq);
}


zbStatus_t IASACE_GetZoneIDMapRsp
(
    zclIASACE_GetZoneIDMApRsp_t *pReq
)
{
    pReq->zclTransactionId = gZclTransactionId++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterIASACE_c);	
    return ZCL_SendServerRspSeqPassed(gZclCmdTxIASACE_GetZoneIDMApRsp_c, sizeof(zclCmdIASACE_GetZoneIDMApRsp_t),(zclGenericReq_t *)pReq);
}
/*****************************************************************************/
zbStatus_t IASACE_GetZoneInformation
(
    zclIASACE_GetZoneInformation_t *pReq
)
{
    pReq->zclTransactionId = gZclTransactionId++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterIASACE_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxIASACE_GetZoneInformation_c, sizeof(zclCmdIASACE_GetZoneInformation_t),(zclGenericReq_t *)pReq);
}

void ZCL_SendGetZoneInformationCommand(zbEndPoint_t DstEndpoint, zbEndPoint_t SrcEndpoint, zbNwkAddr_t DstAddr, uint8_t zoneId) 
{
    zclIASACE_GetZoneInformation_t *pReq;
    
    pReq = MSG_Alloc(sizeof(zclIASACE_GetZoneInformation_t));   
    if(pReq) {
        pReq->addrInfo.dstAddrMode = gZbAddrMode16Bit_c;
        Copy2Bytes(pReq->addrInfo.dstAddr.aNwkAddr, DstAddr);
        pReq->addrInfo.dstEndPoint = DstEndpoint;
        pReq->addrInfo.srcEndPoint = SrcEndpoint;
        pReq->addrInfo.txOptions = 0;
        pReq->addrInfo.radiusCounter = afDefaultRadius_c;
        pReq->cmdFrame.ZoneID = zoneId;
             
        (void) IASACE_GetZoneInformation(pReq);
         MSG_Free(pReq);
    }
}

zbStatus_t IASACE_GetZoneInformationRsp
(
    zclIASACE_GetZoneInfRsp_t *pReq
)
{
    pReq->zclTransactionId = gZclTransactionId++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterIASACE_c);	
    return ZCL_SendServerRspSeqPassed(gZclCmdTxIASACE_GetZoneInfRsp_c, sizeof(zclCmdIASZone_GetZoneInfRsp_t),(zclGenericReq_t *)pReq);
}

/******************************
  IAS Warning Device Cluster 
  See ZCL Specification Section 8.4
*******************************/

/*****************************************************************************/
zbStatus_t ZCL_IASWDClusterClient
(
    zbApsdeDataIndication_t *pIndication, 
    afDeviceDef_t *pDev
)
{
    zclFrame_t *pFrame;
    zclCmd_t command;

    /* prevent compiler warning */
    (void)pDev;
    pFrame = (void *)pIndication->pAsdu;
    /* handle the command */
    command = pFrame->command;  
    switch(command)
    {
       default:
          return gZclUnsupportedClusterCommand_c;
    }

}

/*****************************************************************************/
zbStatus_t ZCL_IASWDClusterServer
(
    zbApsdeDataIndication_t *pIndication, 
    afDeviceDef_t *pDev
)
{
    zclFrame_t *pFrame;
    uint8_t event;
    zclCmdIASWD_StartWarning_t  Command;
    zbClusterId_t aClusterId;
    uint16_t maxDurationAttr;
    /* prevent compiler warning */
    (void)pDev;
    pFrame = (void *)pIndication->pAsdu;
    Copy2Bytes(aClusterId, pIndication->aClusterId);
    /* handle the command */
    (void)ZCL_GetAttribute(pIndication->dstEndPoint, aClusterId, gZclAttrIASWDMaxDuration_c, &maxDurationAttr, NULL);
    switch(pFrame->command)
    {
        case gZclCmdRxIASWD_StartWarning_c:
            FLib_MemCpy(&Command ,(pFrame + 1), sizeof(zclCmdIASWD_StartWarning_t));
            if(Command.WarningModeandStrobe.WarningMode != 0)
              if(Command.WarningModeandStrobe.Strobe != 0)      
                event = gZclUI_StartWarningStrobe_c;
              else
                event = gZclUI_StartWarning_c;
            else
              if(Command.WarningModeandStrobe.Strobe != 0)
                event = gZclUI_StartStrobe_c;
              else
                event = gZclUI_NoEvent_c;
            gDurationWarning = ((Command.WarningDuration > maxDurationAttr)?maxDurationAttr:Command.WarningDuration);  
            BeeAppUpdateDevice(0, event, 0, 0, NULL);
            return gZbSuccess_c;         
        case gZclCmdRxIASWD_Squawk_c:
            FLib_MemCpy(&gParamSquawk ,(pFrame + 1), sizeof(zclCmdIASWD_Squawk_t));
            event = (gStartWarningActive > 0)?gZclUI_NoEvent_c:gZclUI_Squawk_c;
            BeeAppUpdateDevice(0, event, 0, 0, NULL);
            return gZbSuccess_c;
        default:
          return gZclUnsupportedClusterCommand_c;
    }

}

/*****************************************************************************/                         
zbStatus_t IASWD_StartWarning
(
   zclIASWD_StartWarning_t *pReq
)
{
    pReq->zclTransactionId = gZclTransactionId++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterIASWD_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxIASWD_StartWarning_c,sizeof(zclCmdIASWD_StartWarning_t),(zclGenericReq_t *)pReq);
}


void ZCL_SendStartWarning(zbEndPoint_t DstEndpoint, zbEndPoint_t SrcEndpoint, zbNwkAddr_t DstAddr, WarningModeAndStrobe_t warningStrobe, uint16_t  warningDuration) 
{
    zclIASWD_StartWarning_t *pReq;
    pReq = MSG_Alloc(sizeof(zclIASWD_StartWarning_t)); 
  
    if(pReq) {
        pReq->addrInfo.dstAddrMode = gZbAddrMode16Bit_c;
        Copy2Bytes(pReq->addrInfo.dstAddr.aNwkAddr, DstAddr);
        pReq->addrInfo.dstEndPoint = DstEndpoint;
        pReq->addrInfo.srcEndPoint = SrcEndpoint;
        pReq->addrInfo.txOptions = 0;
        pReq->addrInfo.radiusCounter = afDefaultRadius_c;
        pReq->cmdFrame.WarningModeandStrobe = warningStrobe;
        pReq->cmdFrame.WarningDuration = warningDuration;
        
        (void)IASWD_StartWarning(pReq);
        MSG_Free(pReq);
    }
}

/*****************************************************************************/
zbStatus_t IASWD_Squawk
(
    zclIASWD_Squawk_t *pReq
)     
{
    pReq->zclTransactionId = gZclTransactionId++;
    Set2Bytes(pReq->addrInfo.aClusterId, gZclClusterIASWD_c);	
    return ZCL_SendClientReqSeqPassed(gZclCmdRxIASWD_Squawk_c,sizeof(zclCmdIASWD_Squawk_t),(zclGenericReq_t *)pReq);
}

/*****************************************************************************/


void ZCL_SendSqawk(zbEndPoint_t DstEndpoint, zbEndPoint_t SrcEndpoint, zbNwkAddr_t DstAddr, zclCmdIASWD_Squawk_t squawk) 
{
    zclIASWD_Squawk_t *pReq;
    pReq = MSG_Alloc(sizeof(zclIASWD_StartWarning_t)); 
  
    if(pReq) {
        pReq->addrInfo.dstAddrMode = gZbAddrMode16Bit_c;
        Copy2Bytes(pReq->addrInfo.dstAddr.aNwkAddr, DstAddr);
        pReq->addrInfo.dstEndPoint = DstEndpoint;
        pReq->addrInfo.srcEndPoint = SrcEndpoint;
        pReq->addrInfo.txOptions = 0;
        pReq->addrInfo.radiusCounter = afDefaultRadius_c;
        pReq->cmdFrame = squawk; 
       (void)IASWD_Squawk(pReq);
        MSG_Free(pReq);
    }
}