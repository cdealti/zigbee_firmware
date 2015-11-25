/*****************************************************************************
* Oep11073.c
*
* Class support utility functions implementation
*
* Copyright (c) 2010, Freescale, Inc. All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
*
*****************************************************************************/



#ifdef gHcGenericApp_d




#include "EmbeddedTypes.h"
#include "BeeCommon.h"
#include "NomenclatureCodes.h"
#include "OepManager.h"
#include "OepTypes.h"
#include "HcZtc.h"
#include "OEPClassSupport.h"
#include "Oep11073Config.h"
#include "FunctionLib.h"

#include "NomenclatureCodes.h"
#include "OEPClassSupport.h"


/******************************************************************************
*******************************************************************************
* Private macro's
*******************************************************************************
******************************************************************************/
#if gBigEndian_c
#define MemCpyNative2HcOta(src,dst,len) FLib_MemCpy((src),(dst),(len))
#else
#define MemCpyNative2HcOta(src,dst,len) FLib_MemCpyReverseOrder((src),(dst),(len))
#endif

/******************************************************************************
*******************************************************************************
* Private type definitions
*******************************************************************************
******************************************************************************/



/******************************************************************************
*******************************************************************************
* Private Prototypes
*******************************************************************************
******************************************************************************/

#define getCfgObjLength(cfgObj) (sizeof(oidType_t)+sizeof(oepHandle_t))


uint16_t DumpValue(objectAttribute_t* pAttribute, uint8_t *pBuffer, bool_t includeAttrHeader);
uint16_t oepGenerateConfigReport(configReport_t *pCfgReport);
uint16_t oepDumpObjectInConfigReport(genericClass_t *pObj, configObject_t *pConfigObj);
uint16_t DumpAttrValMap(genericClass_t *pObj, uint8_t *pBuffer);
uint16_t DumpObjectAttributes(genericClass_t *pObj, attributeList_t *pAttrList);
uint16_t DumpSysTypeSpecList( uint8_t *pDst, typeVerList_t *pSrc);
uint16_t DumpBasicValList(uint8_t *pDst, basicValList_t *pSrc);
uint16_t DumpBatteryRemain(uint8_t *pDst, batMeasure_t *pSrc);
uint16_t DumpSystemModel(uint8_t *pDst, systemModel_t *pSrc);
uint16_t DumpVarLenString(uint8_t *pDst, uint8_t *pSrc, uint16_t strLen);

/******************************************************************************
*******************************************************************************
* Private Memory Declarations
*******************************************************************************
******************************************************************************/

/******************************************************************************
*******************************************************************************
* Public memory declarations
*******************************************************************************
******************************************************************************/

/*
   Pointer to  NULL terminated array of objects. 
   Must be initialized by the app.
*/
genericClass_t* (*pObjectArray)[] = NULL;


/******************************************************************************
*******************************************************************************
* Public Functions
*******************************************************************************
******************************************************************************/

/******************************************************************************
  Will search in the specified object an attribute of the specified type.
******************************************************************************/
oepClassSupportErr_t  oepCsGetAttributeByType(genericClass_t* pObject, oidType_t type, objectAttribute_t** pReturnValue)
{
  oepClassSupportErr_t result = gOepCsAttributeNotFound_d;
  uint16_t index;

  //Verify input params.
  if((pObject == NULL) || (pReturnValue == NULL)) return gOepCsInputParamError_d;
  
  //Start searching for the attribute.
  for(index = 0; index < pObject->attributeCount; index++)
  {
    //Should actually look for the partition code also.
    if(pObject->pAttributeArray[index].attributeId == type)
    {
      *pReturnValue = &(pObject->pAttributeArray[index]);
      result = gOepCsSucess_d;  
      break;
    }
  }
  
  return result;
}

/******************************************************************************
  Searches the object for the attribute by type and sets as instructed the 
  enable status of the attribute in the object.
******************************************************************************/
oepClassSupportErr_t  oepCsEnableByAttributeType(genericClass_t* pObject, oidType_t type, bool_t enable)
{
  oepClassSupportErr_t result;
  objectAttribute_t* pAttribute;
  
  //The calles=d function verifies input parameters
  result = oepCsGetAttributeByType(pObject, type, &pAttribute);

  if(result == gOepCsSucess_d)
  {
    if(enable)
    {
      pAttribute->flags |= gAttrFlagDisabled_d;
    }
    else
    {
      pAttribute->flags &= (~gAttrFlagDisabled_d);
    }
  }
    
  return result;
}




/******************************************************************************
  Dumps the attribute number and types for an object in the specified buffer.
******************************************************************************/


//////////////////////////????? All the attributes are considered  ? ////////////////////////////////////
oepClassSupportErr_t  oepCsDumpObjectDescriptor(genericClass_t* pObject, uint8_t* pBuffer, uint16_t* pBytesWritten)
{
  uint16_t length = 0;
  uint16_t index;

  //Verify input params.
  if((pObject == NULL) || (pBuffer == NULL) || (pBytesWritten == NULL)) return gOepCsInputParamError_d;

  //The first byte is the number of attributes
  *pBuffer = (uint8_t)pObject->attributeCount;
  pBuffer++;
  length+=1;
  //Walk the attribute list and copy the nomenclator and type.
  for(index = 0; index < pObject->attributeCount; index++)
  {
      //Copy the partition ID and type
      FLib_MemCpy(pBuffer, (void*)&pObject->pAttributeArray[index], 4);
      pBuffer += 4;
      length += 4;
  }
  //Copy to the return parameter the whole length
  *pBytesWritten = length;

  return gOepCsSucess_d;
}


/******************************************************************************
  Will dump to memory a list of object class and object handle.
  THE CODE BELOW SUPPORTS ONLY BIG ENDIAN PROCESSORS. CHANGES WILL HAVE TO BE
  FOR ENDIANESS INDEPENDENCE.
******************************************************************************/
oepClassSupportErr_t  oepCsGetObjectsHandlesTypes(uint8_t* pBuffer, uint16_t* pReturnValue)
{
  uint16_t index = 0;

  //Verify input params.
  if((pObjectArray == NULL) || (pBuffer == NULL) || (pReturnValue == NULL)) 
    return gOepCsInputParamError_d;
  
  //Walk the object list, copy to the memory buffer the object type and object header.
  while((*pObjectArray)[index] != (genericClass_t*)NULL)
  {
    FLib_MemCpy((uint8_t*)pBuffer + 1 + (index << 2), (uint8_t*)(*pObjectArray)[index], 4);
    index++;
  }
  //Write the number of objects
  *(pBuffer) = (uint8_t)index;
  *pReturnValue = (index << 2) + 1;
  
  return gOepCsSucess_d;
}

/******************************************************************************
  Will return a pointer to the object with the specified handle.
******************************************************************************/
oepClassSupportErr_t  oepCsGetObjectByHandle(oepHandle_t handle, genericClass_t** pReturnValue)
{
  
  oepClassSupportErr_t result = gOepCsObjectNotFound_d;
  uint16_t index = 0;

  //Verify input params.
  if((pObjectArray == NULL) || (pReturnValue == NULL)) return gOepCsInputParamError_d;
  
  //Walk the object list, copy to the memory buffer the object type and object header.
  while((*pObjectArray)[index] != (genericClass_t*)NULL)
  {
    if((*pObjectArray)[index]->handle == handle)
    {
      *pReturnValue = (*pObjectArray)[index];
      result = gOepCsSucess_d;
      break;
    }
    index++;
  }
  return result;
}

/******************************************************************************
  Will dump an attribute of an object, starting with the status:
  gOepCsAttributeNotEnabled_d or gOepCsAttributeEnabled_d.
  The attribute length is limited to 255 bytes.
******************************************************************************/
oepClassSupportErr_t  oepCsDumpAttribute(oepHandle_t handle, oidType_t attrType, uint8_t* pBuffer, uint16_t* pBytesWritten)
{
  
  oepClassSupportErr_t result;
  genericClass_t* pObject;
  objectAttribute_t* pAttribute;  
  
  uint16_t length;
  

  //Verify input params.
  if((pBuffer == NULL) || (pBytesWritten == NULL)) return gOepCsInputParamError_d;
  
  //Find the object indicated by the handler
  result = oepCsGetObjectByHandle(handle, &pObject);
  if(result != gOepCsSucess_d) return result;
  
  //Find the attribute by the specified type
  result = oepCsGetAttributeByType(pObject, attrType, &pAttribute);
  if(result != gOepCsSucess_d) return result;

  //Get the status of the attribute - enabled or disabled
  if(!isAttributeEnabled(pAttribute->flags))
  {
    *pBuffer = gOepCsAttributeNotEnabled_d;
  }
  else
  {
    *pBuffer = gOepCsAttributeEnabled_d;
  }
  pBuffer++;
  
  *pBuffer = (uint8_t)pAttribute->length;
  pBuffer++;
      

  /* Dump the attribute value */
  length = DumpValue(pAttribute, pBuffer, TRUE);
  
  
  *pBytesWritten = length + 2;
  
  return gOepCsSucess_d;
}

/******************************************************************************
  Will dump the value for an attribute of an object
  The attribute length is limited to 255 bytes.
******************************************************************************/
oepClassSupportErr_t  oepCsDumpAttributeValue(oepHandle_t handle, oidType_t attrType, uint8_t* pBuffer, uint16_t* pBytesWritten)
{
  
  oepClassSupportErr_t result;
  genericClass_t* pObject;
  objectAttribute_t* pAttribute;
  uint16_t length;

  //Verify input params.
  if((pBuffer == NULL) || (pBytesWritten == NULL)) 
    return gOepCsInputParamError_d;
  
  //Find the object indicated by the handler
  result = oepCsGetObjectByHandle(handle, &pObject);
  if(result != gOepCsSucess_d) 
    return result;
  
  //Find the attribute by the specified type
  result = oepCsGetAttributeByType(pObject, attrType, &pAttribute);
  if(result != gOepCsSucess_d) return result;

 
  
  length = DumpValue(pAttribute, pBuffer, FALSE);
  
    
  *pBytesWritten = length;
  
  return gOepCsSucess_d;
}

/******************************************************************************
  Will set the attribute of a specified object. Note that the enable state
  of the attribute will not be changed.
  FOR NOW, THERE ARE NO CHECKS FOR BUFFER SIZES. THE CALLER NEEDS TO ENSURE
  HE IS NOT CAUSING A BUFFER OVERWRITE.
******************************************************************************/
oepClassSupportErr_t  oepCsSetAttribute(oepHandle_t handle, oidType_t attrType, uint8_t* pBuffer, uint16_t length)
{
  
  oepClassSupportErr_t result;
  genericClass_t* pObject;
  objectAttribute_t* pAttribute;
  void *pDest;

  //Verify input params.
  if(pBuffer == NULL) return gOepCsInputParamError_d;
  
  //Find the object indicated by the handler
  result = oepCsGetObjectByHandle(handle, &pObject);
  if(result != gOepCsSucess_d) return result;
  
  //Find the attribute by the specified type
  result = oepCsGetAttributeByType(pObject, attrType, &pAttribute);
  if(result != gOepCsSucess_d) return result;
  
  //Copy the value - BUFFER OVERWRITE RISK HERE.
  
  
  /* Find the location of the attribute value */  
  if (isAttributeValueInline(pAttribute->flags)) 
  {    
    pDest = (void*)&(pAttribute->value);
  #if gBigEndian_c
    if (pAttribute->length == 1)
      ((uint8_t*)pDest)+=3;
    else if (pAttribute->length == 2)
      ((uint8_t*)pDest)+=2;
  #endif
  }
  else 
  {    
    pDest = (void*)(pAttribute->value.data32);
  }          
  
  FLib_MemCpy(pDest, (void*)pBuffer, (uint8_t)length);

  
  pAttribute->length = length;  
  return gOepCsSucess_d;
}


/******************************************************************************
  Handles a config requests. Sends back the response/confirm.
******************************************************************************/
void oepCsManagerHandleConfigMsg(oepGenericApdu_t* pInApdu, uint8_t* buffer, uint16_t bufferSize, uint8_t srcEndpoint)
{
  oepClassSupportErr_t csResult;
  OepOutgoingMessage_t outMsg;
  oepGenericApdu_t *pApdu = (oepGenericApdu_t*)buffer;
  
  (void)bufferSize;
  
  FLib_MemSet(&outMsg, sizeof(OepOutgoingMessage_t), 0x00);
  
  outMsg.srcEndPoint = srcEndpoint;
  switch(pInApdu->choice)
  {
    case oepObjCfgChoice_GetObjectsReq:
      //No input data, move on to process the request and send the result
      pApdu->choice = oepObjCfgChoice_GetObjectsRsp;
      csResult = oepCsGetObjectsHandlesTypes((uint8_t*)&pApdu->payload, (uint16_t*)&pApdu->apduLen);
      if(csResult == gOepCsSucess_d)
      {
        pApdu->apduLen+=2;
        outMsg.ztcOnly = TRUE;
        outMsg.pApdu = (uint8_t*)pApdu;
        outMsg.length = pApdu->apduLen + 2;
        pApdu->apduLen = Native2OTA16(pApdu->apduLen);
        Oep_ZCL_SAPHandler(&outMsg);
      }
    break;
    
    case oepObjCfgChoice_SetNewHandleReq:
    {
      genericClass_t* pObject;
      oepSetNewHandleReq_t* pSetNewHandle = (oepSetNewHandleReq_t*)pInApdu->payload;

      //Change the endianess for the 16 bit fields received
#if (gBigEndian_c)           
      Swap2BytesArray((uint8_t*)&pSetNewHandle->oldHandle);
      Swap2BytesArray((uint8_t*)&pSetNewHandle->newHandle);
#endif
      //The header of the confirm can be filled in
      pApdu->apduLen = 3;      
      pApdu->choice = oepObjCfgChoice_SetNewHandleCnf;     
      //Search the object by handle
      csResult = oepCsGetObjectByHandle(pSetNewHandle->oldHandle, &pObject);
      if((csResult == gOepCsSucess_d)&&(pObject != NULL))
      {
        pObject->handle = pSetNewHandle->newHandle;
      }
      pApdu->payload[0] = csResult; 
      outMsg.ztcOnly = TRUE;
      outMsg.pApdu = (uint8_t*)pApdu;
      outMsg.length = pApdu->apduLen + 2;
      pApdu->apduLen = Native2OTA16(pApdu->apduLen);
      Oep_ZCL_SAPHandler(&outMsg);
    }
    break;
    
    case oepObjCfgChoice_GetObjectDescriptorReq:
    {
      genericClass_t* pObject;
      oepGetObjectDescriptorRsp_t* pObjectDescriptor;
      oepGetObjectDescriptorReq_t* pGetObject = (oepGetObjectDescriptorReq_t*)pInApdu->payload;

      //Change the endianess for the 16 bit fields received
#if (gBigEndian_c)           
      Swap2BytesArray((uint8_t*)&pGetObject->objHandle);
#endif
      //First get the object by the received handle
      csResult = oepCsGetObjectByHandle(pGetObject->objHandle, &pObject);
      if((csResult == gOepCsSucess_d)&&(pObject != NULL))
      {
        pObjectDescriptor = (oepGetObjectDescriptorRsp_t*)(&pApdu->payload);
        pObjectDescriptor->objHandle = pGetObject->objHandle;
        //Now get dump the object - influenced by how the object descriptor is dumped
        (void)oepCsDumpObjectDescriptor(pObject, &pObjectDescriptor->attrCount, &pApdu->apduLen);
        //Fill in the header and send the message
        pApdu->choice = oepObjCfgChoice_GetObjectDescriptorRsp;
        pApdu->apduLen+=4;
        outMsg.ztcOnly = TRUE;
        outMsg.pApdu = (uint8_t*)pApdu;
        outMsg.length = pApdu->apduLen + 2;
        pApdu->apduLen = Native2OTA16(pApdu->apduLen);
        Oep_ZCL_SAPHandler(&outMsg);
      }
    }
    break;
    
    case oepObjCfgChoice_GetAttributeReq:
    {
      oepGetAttributeRsp_t* pGetAttributeRsp;
      oepGetAttributeReq_t* pGetAttributeReq = (oepGetAttributeReq_t*)pInApdu->payload;

      //Change the endianess for the 16 bit fields received
#if (gBigEndian_c)           
      Swap2BytesArray((uint8_t*)&pGetAttributeReq->objHandle);
      Swap2BytesArray((uint8_t*)&pGetAttributeReq->attrType);
#endif      

      pGetAttributeRsp = (oepGetAttributeRsp_t*)(&pApdu->payload);
      csResult = oepCsDumpAttribute(pGetAttributeReq->objHandle, pGetAttributeReq->attrType, (uint8_t*)(&pGetAttributeRsp->status), &pApdu->apduLen);
      if(csResult == gOepCsSucess_d)
      {
        pGetAttributeRsp->objHandle = pGetAttributeReq->objHandle;
        pGetAttributeRsp->attrType = pGetAttributeReq->attrType;
        //Fill in the header and send the message
        pApdu->choice = oepObjCfgChoice_GetAttributeRsp;
        pApdu->apduLen+=6;
        outMsg.ztcOnly = TRUE;
        outMsg.pApdu = (uint8_t*)pApdu;
        outMsg.length = pApdu->apduLen + 2;
        pApdu->apduLen = Native2OTA16(pApdu->apduLen);
        Oep_ZCL_SAPHandler(&outMsg);
      }
    }
    break;
    
    case oepObjCfgChoice_SetAttributeReq:
    {
      oepSetAttributeCnf_t* pSetAttributeCnf;
      oepSetAttributeReq_t* pSetAttributeReq = (oepSetAttributeReq_t*)pInApdu->payload;
      //Change the endianess for the 16 bit fields received
#if (gBigEndian_c)           
      Swap2BytesArray((uint8_t*)&pSetAttributeReq->objHandle);
      Swap2BytesArray((uint8_t*)&pSetAttributeReq->attrType);
#endif      

      pSetAttributeCnf = (oepSetAttributeCnf_t*)(&pApdu->payload[0]);
      csResult = oepCsSetAttribute(pSetAttributeReq->objHandle, pSetAttributeReq->attrType, &pSetAttributeReq->value[0], pSetAttributeReq->valueLen);
      if(csResult == gOepCsSucess_d)
      {
        pSetAttributeCnf->status = gOepCsSucess_d;
        pSetAttributeCnf->objHandle = pSetAttributeReq->objHandle;
        pSetAttributeCnf->attrType = pSetAttributeReq->attrType;
        //Fill in the header and send the message
        pApdu->choice = oepObjCfgChoice_SetAttributeCnf;
        pApdu->apduLen = sizeof(oepSetAttributeCnf_t) + 2;
        outMsg.length = pApdu->apduLen + 2;
        pApdu->apduLen = Native2OTA16(pApdu->apduLen);
        outMsg.ztcOnly = TRUE;
        outMsg.pApdu = (uint8_t*)pApdu;
        
        Oep_ZCL_SAPHandler(&outMsg);
      }
    }
    break;
    case oepObjCfgChoice_SetAttributeEnabledReq:
    {
      genericClass_t* pObject;
      oepSetAttributeEnabledCnf_t* pSetAttributeEnabledCnf;
      oepSetAttributeEnabledReq_t* pSetAttributeEnabledReq = (oepSetAttributeEnabledReq_t*)pInApdu->payload;
      //Change the endianess for the 16 bit fields received
#if (gBigEndian_c)           
      Swap2BytesArray((uint8_t*)&pSetAttributeEnabledReq->objHandle);
      Swap2BytesArray((uint8_t*)&pSetAttributeEnabledReq->attrType);
#endif      
      pSetAttributeEnabledCnf = (oepSetAttributeEnabledCnf_t*)(&pApdu->payload);
      //First get the object by the received handle
      csResult = oepCsGetObjectByHandle(pSetAttributeEnabledReq->objHandle, &pObject);
      if((csResult == gOepCsSucess_d)&&(pObject != NULL))
      {
        csResult = oepCsEnableByAttributeType(pObject, pSetAttributeEnabledReq->attrType, (bool_t)pSetAttributeEnabledReq->isEnabled);
        pSetAttributeEnabledCnf->status = csResult;
        pSetAttributeEnabledCnf->objHandle = pSetAttributeEnabledReq->objHandle;
        pSetAttributeEnabledCnf->attrType = pSetAttributeEnabledReq->attrType;
        //Fill in the header and send the message
        pApdu->choice = oepObjCfgChoice_SetAttributeEnabledCnf;
        pApdu->apduLen = sizeof(oepSetAttributeEnabledReq_t) + 2;
        outMsg.ztcOnly = TRUE;
        outMsg.pApdu = (uint8_t*)pApdu;
        outMsg.length = pApdu->apduLen + 2;
        pApdu->apduLen = Native2OTA16(pApdu->apduLen);
        Oep_ZCL_SAPHandler(&outMsg);
      }
    }
    break;
    default:
    //We should send some generic error message back to ZTC here
    break;
  }
}

/******************************************************************************
  Created the association request frame, based on the configuration data for
  the node (implemented in Oep11073Config.c).
  THE CODE BELOW SUPPORTS ONLY BIG ENDIAN PROCESSORS. CHANGES WILL HAVE TO BE
  FOR ENDIANESS INDEPENDENCE.
******************************************************************************/
oepClassSupportErr_t oepCsCreateAssociateReq(uint8_t* pOutBuffer, uint16_t* pBufferLength)
{
  uint16_t localLength;
  uint16_t length;
  aarqOepApdu_t* pAarq;
  oepClassSupportErr_t csResult;
  apdu_t* pApdu;
  genericClass_t* pObject;
  objectAttribute_t* pAttribute; 
  
  //Verify input params.
  if((pOutBuffer == NULL)||(pBufferLength == NULL)) return gOepCsInputParamError_d;
  pApdu = (apdu_t*)((uint8_t*)pOutBuffer + sizeof(uint16_t));
  //apdu len to be completed later
  length = 2;
  pAarq =  (aarqOepApdu_t*)&pApdu->u.aarq;
  
  pApdu->choice = gAarq_d;
  //pApdu->length to be completed later
  length+= 4;  
    
  //Move to create the aarq frame
  pAarq->assocVersion = gAssociationVersion_d;
  length+= sizeof(associationVersion_t);
  
  // data proto list count
  pAarq->count = Native2HcOTA16(1);
  length+= sizeof(uint16_t);
  
  // data proto list length, to be completed later
  length+= sizeof(uint16_t);
  
  pAarq->dataProtoId = gDataProtoId_d;
  length += sizeof(dataProtoId_t);
  
  //lengthAssocRqInfo field, to be completed later
  length += sizeof(uint16_t);   
  
  pAarq->protocolVersion = gProtocolVersion_d;
  length += sizeof(protocolVersion_t);
  
  pAarq->encodingRules = gEncodingRules_d;
  length += sizeof(encodingRules_t);
  
  pAarq->nomenclatureVersion = gNomenclatureVer_d;
  length += sizeof(nomenclatureVersion_t);
  
  pAarq->functionalUnits = gFunctionalUnits_d;
  length += sizeof(functionalUnits_t);
  
  pAarq->systemType = gSystemType_d;
  length += sizeof(systemType_t);
  
  //Get the systemId from the MDS object

  csResult = oepCsGetObjectByHandle(0, &pObject);
  if(csResult != gOepCsSucess_d) return gOepCsError_d;
    
  csResult = oepCsGetAttributeByType(pObject, MDC_ATTR_SYS_ID, &pAttribute);
  if(csResult != gOepCsSucess_d) return gOepCsError_d;
   
  localLength = DumpValue(pAttribute,  (uint8_t*)&pAarq->systemIdLen, FALSE);
  //pAarq->systemIdLen = Native2HcOTA16(localLength);
  
  length += localLength;

  
  //Get the configId from the MDS object
  csResult = oepCsDumpAttributeValue(0, MDC_ATTR_DEV_CONFIG_ID, (uint8_t*)&pAarq->devConfigId, &localLength);
  if(csResult != gOepCsSucess_d) return gOepCsError_d;
  length += localLength;
  
  
  pAarq->dataReqModeCapab = dataReqModeCapab;
  length+=sizeof(dataReqModeCapab_t);
  
  //Option list
  pAarq->optionList.count = 0x00;
  pAarq->optionList.length = 0x00;
  length+=4;

  //Write back the total length
  *pBufferLength = length;
  
  //Back lenghts in the structure
  pAarq->lengthAssocRqInfo = Native2HcOTA16(length - 18);
  pAarq->length = Native2HcOTA16(length - 14);
  //The aarq length
  pApdu->length = Native2HcOTA16(length - 6);
  //The len of the whope pApdu length (OTA 11073 frame) - first 2 bytes  
  *(uint16_t*)pOutBuffer = Native2OTA16(length - 2);
  
  
  return gOepCsSucess_d;
}


/******************************************************************************
  Created the association request frame, based on the configuration data for
  the node (implemented in Oep11073Config.c).
  THE CODE BELOW SUPPORTS ONLY BIG ENDIAN PROCESSORS. CHANGES WILL HAVE TO BE
  FOR ENDIANESS INDEPENDENCE.
******************************************************************************/

oepClassSupportErr_t oepCsCreateAssociateRes(uint8_t* pOutBuffer, associateResult_t result, uint16_t* pBufferLength)
{
  uint16_t length;
  uint16_t localLength;
  aareOepApdu_t* pAare;
  oepClassSupportErr_t csResult;
  apdu_t* pApdu;  
  genericClass_t* pObject;
  objectAttribute_t* pAttribute; 


  //Verify input params.
  if((pOutBuffer == NULL)||(pBufferLength == NULL)) return gOepCsInputParamError_d;
  pApdu = (apdu_t*)((uint8_t*)pOutBuffer + sizeof(uint16_t));
  //apdu len to be completed later
  length = 2;
  
  pAare =  (aareOepApdu_t*)&pApdu->u.aare;
  
  pApdu->choice = gAare_d;
  //pApdu->length to be completed later
  length += 4;  
    
  //Move to create the aare frame
  pAare->result = result;
  length+= sizeof(associateResult_t);


  pAare->dataProtoId = gDataProtoId_d;
  length+= sizeof(dataProtoId_t);

  //pAare->lengthAssocReInfo = To be completed later
  length+= sizeof(uint16_t);
  
  pAare->protocolVersion = gProtocolVersion_d;
  length+= sizeof(protocolVersion_t);
  
  pAare->encodingRules = gEncodingRules_d;
  length+= sizeof(encodingRules_t);
  
  pAare->nomenclatureVersion = gNomenclatureVer_d;
  length+= sizeof(nomenclatureVersion_t);

  pAare->functionalUnits = gFunctionalUnits_d;
  length+= sizeof(functionalUnits_t);
  
  pAare->systemType = gSystemType_d;
  length+= sizeof(systemType_t);
  

  
  //Get the systemId from the MDS object

  csResult = oepCsGetObjectByHandle(255, &pObject);
  if(csResult != gOepCsSucess_d) return gOepCsError_d;
    
  csResult = oepCsGetAttributeByType(pObject, MDC_ATTR_SYS_ID, &pAttribute);
  if(csResult != gOepCsSucess_d) return gOepCsError_d;   
  localLength = DumpValue(pAttribute,  (uint8_t*)&pAare->systemIdLen, FALSE);
  length += localLength;
 

  pAare->devConfigId = 0;
  length+= sizeof(configId_t);

  FLib_MemSet(&pAare->dataReqModeCapab, sizeof(dataReqModeCapab_t), 0);
  length+= sizeof(dataReqModeCapab_t);

  //Option list
  pAare->optionList.count = 0x00;
  pAare->optionList.length = 0x00;
  length+=4;

  
  //Write back the total length
  *pBufferLength = length;
  
  //Back lenghts in the structure
  pAare->lengthAssocReInfo = Native2HcOTA16(length - 12);
  pApdu->length = Native2HcOTA16(length - 6);
  //pApdu length - first 2 bytes
  *(uint16_t*)pOutBuffer = Native2OTA16(length - 2); 
  
  return gOepCsSucess_d;
}

/******************************************************************************
  Created the association request frame, based on the configuration data for
  the node (implemented in Oep11073Config.c).
  THE CODE BELOW SUPPORTS ONLY BIG ENDIAN PROCESSORS. CHANGES WILL HAVE TO BE
  FOR ENDIANESS INDEPENDENCE.
******************************************************************************/

#if (gHcDeviceType_d == gHcDcu_c)  
oepClassSupportErr_t oepCsVerifyAssociateReq(aarqOepApdu_t* pAarq, associateResult_t *pAssociateResult)
{
  //Verify input params.
  if((pAarq == NULL)||(pAssociateResult == NULL)) return gOepCsInputParamError_d;

  //Basic verification on the aarq
  if(pAarq->assocVersion != gAssociationVersion_d)
  {
    *pAssociateResult = gRejectedUnsupportedAssocVersion_d;
    return gOepCsSucess_d;   
  }
  if(pAarq->dataProtoId != gDataProtoId_d)
  {
    *pAssociateResult = gRejectedNoCommonProtocol_d;
    return gOepCsSucess_d;   
  }
    
  /*
  if(pAarq->dataProtoId != )
  {
    *pAssociateResult = ;
    return gOepCsSucess_d;   
  }
  if(pAarq-> != )
  {
    *pAssociateResult = ;
    return gOepCsSucess_d;   
  }
  if(pAarq-> != )
  {
    *pAssociateResult = ;
    return gOepCsSucess_d;   
  }
  */
  if(unknownConfigIdAlways)
  {
    *pAssociateResult = gAcceptedUnknownConfig_d;
  }
  else
  {
    *pAssociateResult = gAccepted_d;
  }
  return gOepCsSucess_d; 
}
#endif /* #if (gHcDeviceType_d == gHcDcu_c) */

/******************************************************************************
  Created the configuration report (remote operation invoke report 
  configuration), based on the configuration data for the node (implemented in
  Oep11073Config.c).
  THE CODE BELOW SUPPORTS ONLY BIG ENDIAN PROCESSORS. CHANGES WILL HAVE TO BE
  FOR ENDIANESS INDEPENDENCE.
******************************************************************************/
#if (gHcDeviceType_d != gHcDcu_c)  



/*******************************************
 oepCsCreateReportConfig
 Creates a report config event notification
*******************************************/

oepClassSupportErr_t oepCsCreateReportConfig(uint8_t* pOutBuffer, 
                                             uint16_t* pBufferLength, 
                                             uint16_t invokeId)
{

  uint16_t length;  
  prstOepCfgRepApdu_t* pCfgReport;
  apdu_t* pApdu;
  
  
  /* Verify input params */
  if((pOutBuffer == NULL)||(pBufferLength == NULL)) 
    return gOepCsInputParamError_d;
    
    
  pApdu = (apdu_t*)((uint8_t*)pOutBuffer + sizeof(uint16_t));
  pCfgReport =  &pApdu->u.cfgReport;
   
    
  pApdu->choice = gPrst_d;
       
  /* Create the cfgReport frame */
  pCfgReport->invokeId = Native2HcOTA16(invokeId);   
  pCfgReport->choice = gRoivCmipConfirmedEventReport_d;   
  pCfgReport->objHandle = gMdsObjectHandler_d; /* Always MDS handler here */  
  pCfgReport->eventTime = 0xFFFFFFFF;
  pCfgReport->eventType = MDC_NOTI_CONFIG;
  
  /*
    The packet structure below is used for calculating the appropiate lengths
    apdu  +choice(2)	
          +apdu.length(2)
          +cfgrptapdu
                +length(2) 
                +invokeid(2) 
                +choice(2) 
                +lengthapdu(2) 
                +objhandle(2) 
                +eventtime(4) 
                +eventtype(2)
                +cfgReportlength(2)
  */  
  
  length = oepGenerateConfigReport(&pCfgReport->cfgReport);
  
  pCfgReport->lengthCfgReport = Native2HcOTA16(length);
  pCfgReport->lengthDataApdu = Native2HcOTA16(length + 10);
  pCfgReport->length = Native2HcOTA16(length + 16);
  pApdu->length = Native2HcOTA16(length + 18);
  *(uint16_t*)pOutBuffer = Native2OTA16(length + 22);
  
  /* Write back the total length */
  *pBufferLength = length + 24;  
  
  
  return gOepCsSucess_d; 

}



/******************************************************************************
  Created the data report (remote operation invoke report 
  configuration), based on the data supplied by the caller.
*******************************************************************************/  
oepClassSupportErr_t oepCsCreateDataReport(
                          uint8_t  *pOutBuffer, 
                          uint16_t *pBufferLength, 
                          uint16_t invokeId, 
                          uint16_t dataReqId, 
                          uint16_t scanReportNo)
{
  apdu_t *pApdu;
  genericClass_t *pObj;
  objectAttribute_t *pAttribute;
  prstOepDataReport_t* pDataReport;
  observationScanFixed_t *scanEntry;
  uint16_t scanReportLength = 0, scanEntryLength = 0, attrlen = 0;
  uint8_t  *pBuf, scanReportCount = 0, i, j;
  
  
  
  /* Verify input params */
  if((pOutBuffer == NULL)||(pBufferLength == NULL)) 
    return gOepCsInputParamError_d;
      
  
  pApdu = (apdu_t*)((uint8_t*)pOutBuffer + sizeof(uint16_t));  
            
  pApdu->choice = gPrst_d;    
  //pApdu->length = length;
  
  
  /* 1. Create the pDataReport frame */
  pDataReport = &pApdu->u.dataReport;
  //pDataReport->length = length;
    
  pDataReport->invokeId = Native2HcOTA16(invokeId);   
  pDataReport->choice = gRoivCmipConfirmedEventReport_d;   
  //pDataReport->lengthDataApdu = length;
  

  pDataReport->objHandle = gMdsObjectHandler_d;  
  pDataReport->eventTime = 0xFFFFFFFF;
  pDataReport->eventType = MDC_NOTI_SCAN_REPORT_FIXED;     
  //pDataReport->lengthDataReport = length;

  /* 2. Add Scan report */
  pDataReport->scanReport.dataReqId = Native2HcOTA16(dataReqId);
  pDataReport->scanReport.scanReportNo = Native2HcOTA16(scanReportNo);
  
  
  /* This is where the scan list starts */
  pBuf =  (uint8_t*)&pDataReport->scanReport.obsScanFixed.value[0];
  
  
  /* 2.1 Add scan report elements */
  
  /* Search through the objects */
  i = 0;
  while ((*pObjectArray)[i] != (genericClass_t*)NULL) 
  {   
    pObj = (*pObjectArray)[i];
    
    /* ZHCTODO: Check if this object has ValMap attributes ??? */
    if (pObj->includeInConfigReport)
    {
      /* Add a new scan entry */
      scanReportCount++;
      scanEntry = (observationScanFixed_t*) (pBuf);
      scanEntry->objHandle = Native2HcOTA16(pObj->handle);
      scanEntryLength = 0;      
      attrlen = 0;
                                   
      /* Dump the value of attributes in the ValMap of this object */
      for (j= 0; j < pObj->attributeCount; j++)
      {
        pAttribute = &pObj->pAttributeArray[j];

        if (isAttributeInValueMap(pAttribute->flags))
        {          
          attrlen = DumpValue(pAttribute,
                              (uint8_t*)((int)&scanEntry->obsValData.value + attrlen),
                              FALSE);
          scanEntryLength += attrlen;
        }
      }
            
      scanEntry->obsValData.length = Native2HcOTA16(scanEntryLength);           
      
      
      scanReportLength += scanEntryLength + 4; /* Add handle + length fields */          
      pBuf += scanEntryLength + 4;
    
    } /* if */
    
    /* move to the next object in the list */    
     i++;  
    
  } /* while */
  
    
  pDataReport->scanReport.obsScanFixed.count = Native2HcOTA16(scanReportCount);  
  pDataReport->scanReport.obsScanFixed.length = Native2HcOTA16(scanReportLength);
  
  
  /* 3. Fill the length fields */
  pDataReport->lengthDataReport = Native2HcOTA16(scanReportLength + 8);
  pDataReport->lengthDataApdu = Native2HcOTA16(scanReportLength + 18);
  pDataReport->length = Native2HcOTA16(scanReportLength + 24);
  pApdu->length = Native2HcOTA16(scanReportLength+26);   
  *(uint16_t*)pOutBuffer = Native2OTA16(scanReportLength+30);

   *pBufferLength = scanReportLength+32;
  
  return gOepCsSucess_d; 

}




/******************************************************************************
  Create a get MDS response frame.
  THE CODE BELOW SUPPORTS ONLY BIG ENDIAN PROCESSORS. CHANGES WILL HAVE TO BE
  FOR ENDIANESS INDEPENDENCE.
******************************************************************************/
oepClassSupportErr_t oepCsCreateGetMdsResponseFrame
       (
           uint8_t* pOutBuffer, 
           uint16_t* pBufferLength, 
           uint16_t invokeId
       )
{

  prstOepGetFrame_t* pGetResponse;
  
  genericClass_t *pMdsObj;
  uint16_t length;
  apdu_t* pApdu;


  
  /* Verify input params */
  if((pOutBuffer == NULL)||(pBufferLength == NULL)) 
    return gOepCsInputParamError_d;
    

  pApdu = (apdu_t*)((uint8_t*)pOutBuffer + sizeof(uint16_t));
  pApdu->choice = gPrst_d;
  
  pGetResponse =  &pApdu->u.getFrame;
      
  pGetResponse->invokeId = Native2HcOTA16(invokeId);
  pGetResponse->choice = gRorsCmipGet_d; 
  pGetResponse->objHandle = Native2HcOTA16(gMdsObjectHandler_d);
  

  /* Dump the attributes of the MDS handler */
  (void) oepCsGetObjectByHandle(gMdsObjectHandler_d, &pMdsObj);    
  length =  DumpObjectAttributes(pMdsObj, &pGetResponse->attrList);  
       
    
  pGetResponse->lengthDataApdu = Native2HcOTA16(length + 2);
  pGetResponse->length = Native2HcOTA16(length + 8);
  pApdu->length = Native2HcOTA16(length + 10);

  //The len of the whole pApdu length (OTA 11073 frame) - first 2 bytes
  *(uint16_t*)pOutBuffer = Native2OTA16(length + 14);
  
  
  //Write back the total length
  *pBufferLength = length + 14;

  return gOepCsSucess_d;
}


#endif /* #if (gHcDeviceType_d != gHcDcu_c) */

/******************************************************************************
  Created the association request frame, based on the configuration data for
  the node (implemented in Oep11073Config.c).
  THE CODE BELOW SUPPORTS ONLY BIG ENDIAN PROCESSORS. CHANGES WILL HAVE TO BE
  FOR ENDIANESS INDEPENDENCE.
******************************************************************************/
#if (gHcDeviceType_d == gHcDcu_c)  
oepClassSupportErr_t oepCsGenerateConfigRes(uint8_t* pOutBuffer, uint16_t* pBufferLength, uint16_t invokeId, configId_t recConfigId, configResult_t configResult)
{
  uint16_t length;
  prstOepCfgRspApdu_t* pCfgResponse;
  apdu_t* pApdu;
  
  //Verify input params.
  if((pOutBuffer == NULL)||(pBufferLength == NULL)) return gOepCsInputParamError_d;
  pApdu = (apdu_t*)((uint8_t*)pOutBuffer + sizeof(uint16_t));
  pCfgResponse =  &pApdu->u.cfgResponse;

  length = 2;  
  //apdu len to be completed later ####len1
  
  //Move to apdu fill-in
  pApdu->choice = gPrst_d;
  length+= 4;  
  //pApdu->length to be completed later ###len2
  pApdu->length = length;

  length+= 2;  
  //pCfgResponse->length to be completed later ###len3
  pCfgResponse->length = length;
  
  //Move to create the cfgReport frame
  pCfgResponse->invokeId = invokeId;
  length+= sizeof(invokeId_t);
  
  pCfgResponse->choice = gRorsCmipConfirmedEventReport_d;
  length+= sizeof(uint16_t);
  
  //pCfgResponse->lengthDataApdu to be comleted later ###len4
  length+= sizeof(uint16_t);
  pCfgResponse->lengthDataApdu = length;
  
  pCfgResponse->objHandle = 0;
  length+= sizeof(oepHandle_t);
  
  pCfgResponse->currentTime = 0xFFFFFFFF;
  length+= sizeof(relativeTime_t);

  pCfgResponse->eventType = MDC_NOTI_CONFIG;
  length+= sizeof(oidType_t);

  pCfgResponse->lengthReply = Native2HcOTA16(4);
  length+= sizeof(uint16_t);
  
  pCfgResponse->configReportId = recConfigId;
  length+= sizeof(configId_t);
  
  pCfgResponse->configResult = configResult;
  length+= sizeof(configResult_t);

  //Write back the total length
  *pBufferLength = length;
  
  //Back lenghts in the structure
  // # len4
  pCfgResponse->lengthDataApdu = Native2HcOTA16(length - pCfgResponse->lengthDataApdu);
  // # len3
  pCfgResponse->length = Native2HcOTA16(length - pCfgResponse->length);
  // # len2
  pApdu->length = Native2HcOTA16(length - pApdu->length);

  //The len of the whole pApdu length (OTA 11073 frame) - first 2 bytes
  *(uint16_t*)pOutBuffer = Native2OTA16(length - 2);

  return gOepCsSucess_d; 
}



/******************************************************************************
  Create the response to the data report based on the data supplied by the caller.
  THE CODE BELOW SUPPORTS ONLY BIG ENDIAN PROCESSORS. CHANGES WILL HAVE TO BE
  FOR ENDIANESS INDEPENDENCE.
******************************************************************************/
oepClassSupportErr_t oepCsCreateDataResponse(uint8_t* pOutBuffer, uint16_t* pBufferLength, uint16_t invokeId)
{
  uint16_t length;
  prstOepDataRspApdu_t* pDataResponse;
  apdu_t* pApdu;
  
  //Verify input params.
  if((pOutBuffer == NULL)||(pBufferLength == NULL)) return gOepCsInputParamError_d;
  pApdu = (apdu_t*)((uint8_t*)pOutBuffer + sizeof(uint16_t));
  pDataResponse =  &pApdu->u.dataResponse;

  length = 2;  
  //apdu len to be completed later ####(len1 = len - 2)
  
  //Move to apdu fill-in
  pApdu->choice = gPrst_d;
  length+= 4;  
  //pApdu->length to be completed later ###(len2 = len - 6)
  pApdu->length = length;

  length+= 2;  
  //pDataResponse->length to be completed later ###(len3 = len - 8)
  pDataResponse->length = length;
  
  //Move to create the pDataResponse frame
  pDataResponse->invokeId = invokeId;
  length+= sizeof(invokeId_t);
  
  pDataResponse->choice = gRorsCmipConfirmedEventReport_d;
  length+= sizeof(uint16_t);

  //pDataResponse->lengthDataApdu to be completed later ###(len4 = len - 14)
  length+= 2;
  pDataResponse->lengthDataApdu = length;
  


  pDataResponse->objHandle = 0;
  length+= sizeof(oepHandle_t);
  pDataResponse->currentTime = 0xFFFFFFFF; /* Not supported */
  length+= sizeof(relativeTime_t);
  pDataResponse->eventType = MDC_NOTI_SCAN_REPORT_FIXED;
  length+= sizeof(oidType_t);

  //This is simple :D
  pDataResponse->lengthReply = 0;
  length+= sizeof(uint16_t);


  //Write back the total length
  *pBufferLength = length;
  
  //# len4
  pDataResponse->lengthDataApdu = Native2HcOTA16(length - pDataResponse->lengthDataApdu);

  //# len3
  pDataResponse->length = Native2HcOTA16(length - pDataResponse->length);

  //# len2
  pApdu->length = Native2HcOTA16(length - pApdu->length);

  //The len of the whole pApdu length (OTA 11073 frame) - first 2 bytes
  *(uint16_t*)pOutBuffer = Native2OTA16(length - 2);

  return gOepCsSucess_d; 

}


#endif /* #if (gHcDeviceType_d == gHcDcu_c) */

/******************************************************************************
  Create a get request for all atributes fo the specified object.
  THE CODE BELOW SUPPORTS ONLY BIG ENDIAN PROCESSORS. CHANGES WILL HAVE TO BE
  FOR ENDIANESS INDEPENDENCE.
******************************************************************************/
oepClassSupportErr_t oepCsCreateGetAllAtributesFrame(uint8_t* pOutBuffer, uint16_t* pBufferLength, uint16_t invokeId, oepHandle_t handle)
{
  uint16_t length;
  prstOepGetFrame_t* pGetRequest;
  apdu_t* pApdu;
  
  //Verify input params.
  if((pOutBuffer == NULL)||(pBufferLength == NULL)) return gOepCsInputParamError_d;
  pApdu = (apdu_t*)((uint8_t*)pOutBuffer + sizeof(uint16_t));
  pGetRequest =  &pApdu->u.getFrame;

  length = 2;  
  //apdu len to be completed later ####(len1 = len - 2)
  
  //Move to apdu fill-in
  pApdu->choice = gPrst_d;
  length+= 4;  
  //pApdu->length to be completed later ###(len2 = len - 6)
  pApdu->length = length;

  length+= 2;  
  //pGetRequest->length to be completed later ###(len3 = len - 8)
  pGetRequest->length = length;
  
  //Move to create the pDataResponse frame
  pGetRequest->invokeId = Native2HcOTA16(invokeId);
  length+= sizeof(invokeId_t);
  
  pGetRequest->choice = gRoivCmipGet_d;
  length+= sizeof(uint16_t);

  //pGetRequest->lengthDataApdu to be completed later ###(len4 = len - 14)
  length+= 2;
  pGetRequest->lengthDataApdu = length;
  


  pGetRequest->objHandle = handle;
  length+= sizeof(oepHandle_t);
  
  //We request all attributes
  pGetRequest->attrList.count = 0x00;
  length+= sizeof(uint16_t);
  pGetRequest->attrList.length = 0x00;
  length+= sizeof(uint16_t);


  //Write back the total length
  *pBufferLength = length;
  
  //# len4
  pGetRequest->lengthDataApdu = Native2HcOTA16(length - pGetRequest->lengthDataApdu);

  //# len3
  pGetRequest->length = Native2HcOTA16(length - pGetRequest->length);

  //# len2
  pApdu->length = Native2HcOTA16(length - pApdu->length);

  //The len of the whole pApdu length (OTA 11073 frame) - first 2 bytes
  *(uint16_t*)pOutBuffer = Native2OTA16(length - 2);

  return gOepCsSucess_d;
}



oepClassSupportErr_t oepCsCreateReleaseFrame(uint16_t type, uint8_t* pOutBuffer, uint16_t* pBufferLength, uint16_t reason)
{
  apdu_t* pApdu;
  //Verify input params.
  if((pOutBuffer == NULL)||(pBufferLength == NULL)) return gOepCsInputParamError_d;
  //Get a pointer to the apdu frame and fill in the data
  pApdu = (apdu_t*)((uint8_t*)pOutBuffer + sizeof(uint16_t));
  pApdu->choice = type;
  pApdu->length = Native2HcOTA16(2);
  pApdu->u.rlrq.reason = Native2HcOTA16(reason);
  //The len of the whole pApdu length (OTA 11073 frame) - first 2 bytes
  *(uint16_t*)pOutBuffer = Native2OTA16(6);
  //Write back the total length
  *pBufferLength = 8;

  return gOepCsSucess_d;
}


///////////////////#endif



/******************************************************************************
*******************************************************************************
* Private Functions
*******************************************************************************
******************************************************************************/


/*****************************************************
Generates the configReport_t using the pObjectArray
Returns the length of the config report
******************************************************/

uint16_t oepGenerateConfigReport(configReport_t *pCfgReport) 
{

  uint16_t len, totalLen = 0, listCount = 0, i = 0, objLen = 0, offset = 0;
  genericClass_t *pObj;
  void* pBuff;

  /* Get the configId from the MDS object */
  (void) oepCsDumpAttributeValue(
                      gMdsObjectHandler_d, 
                      MDC_ATTR_DEV_CONFIG_ID, 
                      (uint8_t*)&(pCfgReport->configReportId),
                      &len);
     
  /* This is where the first object report goes */
  pBuff = (void*) &(pCfgReport->configObjList.value);

        
  while ((*pObjectArray)[i] != (genericClass_t*)NULL) 
  {   
    pObj = (*pObjectArray)[i];
    
    if (pObj->includeInConfigReport) 
    {
      /* one more object to report */
      listCount++;
      
      /* offset from the begining of pBuff */
      offset += objLen;

      /* add object to config report */
      objLen = oepDumpObjectInConfigReport(pObj,(configObject_t*)(((int)pBuff+offset)));
      totalLen += objLen;            
      
    }

    /* move to the next object in the list */    
    i++;  
  }
    

  pCfgReport->configObjList.length = Native2HcOTA16(totalLen);
  pCfgReport->configObjList.count = Native2HcOTA16(listCount);  

  return totalLen + 6; /* Add  reportid + count + length */
}




/*********************************************
oepDumpObjectInConfigReport
Returns the length of the object report

pObj - source object
pConfigObj - destination object report
**********************************************/
uint16_t oepDumpObjectInConfigReport(genericClass_t *pObj, configObject_t *pConfigObj) 
{
  uint16_t totalLength, tmp, attrlen;
  void* pBuff;  
  
  
  pConfigObj->objClass = Native2HcOTA16(pObj->classId);
  pConfigObj->objHandle = Native2HcOTA16(pObj->handle);
  
  
  pBuff = (void*)&pConfigObj->attributes;
  
  
  totalLength = DumpObjectAttributes(pObj, pBuff);
  
    
  /* Add the ValMap attribute */
  attrlen = DumpAttrValMap(pObj,  (uint8_t*)(((int)pBuff+totalLength)));
  if (attrlen > 0) 
  {    
    totalLength += attrlen;
    
    /* Add the valmap size to the attributes */
    tmp = HcOTA2Native16(pConfigObj->attributes.count);
    pConfigObj->attributes.count = Native2HcOTA16(tmp+1);
    
    tmp = HcOTA2Native16(pConfigObj->attributes.length);
    pConfigObj->attributes.length = Native2HcOTA16(tmp + attrlen);
  }
  

  return totalLength + 4; /* add size of objclass,objhandle */
}




/******************************************************************************
  Dumps the ValMap for this object
  
  pAttribute - source attribute
  pBuffer - destination buffer
  
  Returns the number of bytes.
******************************************************************************/
uint16_t DumpAttrValMap(genericClass_t *pObj, uint8_t *pBuffer) 
{

  uint16_t count = 0, len = 0;
  objectAttribute_t *pAttribute;
  attrValMap_t *pValMap;
  uint8_t i;
  ava_t *pDstAttr;
      

  pDstAttr = (ava_t *)pBuffer;  
  pDstAttr->attributeId = Native2HcOTA16(MDC_ATTR_ATTRIBUTE_VAL_MAP);
  pValMap = (attrValMap_t*)&(pDstAttr->attributeValue.value[0]);
  

  for (i= 0; i < pObj->attributeCount; i++)   
  {
    pAttribute = &pObj->pAttributeArray[i];
    
    if (isAttributeInValueMap(pAttribute->flags)) 
    {
      pValMap->value[count].attributeId  =  Native2HcOTA16(pAttribute->attributeId);
      pValMap->value[count].attributeLen =  Native2HcOTA16(pAttribute->length);
      
      count++;
      len += 4;        
    }      
  }
  
  if (count > 0) 
  {    
    pValMap->count = Native2HcOTA16(count);
    pValMap->length = Native2HcOTA16(len); 
    len += 4; /* Add size of valmap.len + valmap.count */
    pDstAttr->attributeValue.length = Native2HcOTA16(len);     
    return len + 4; /* Add size of attr-id and attr-len */
  } 
  else
    return 0;
  
}


/******************************************************************************
  Dumps the attributes for the specified object 
  The format is:
    Attr->ID
    Attr->Length
    Attr->Value
  
  pObj - source object
  pAttrList - destination buffer
  
  Returns the number of bytes.
******************************************************************************/
uint16_t DumpObjectAttributes(genericClass_t *pObj, attributeList_t *pAttrList) 
{
  

  uint16_t totalLength = 0, attrlen = 0, offset = 0, count = 0;
  objectAttribute_t *pAttribute;
  void* pBuff;
  int i;
        
  
  pBuff = (void*)&pAttrList->value[0];


  for (i= 0; i < pObj->attributeCount; i++)   
  {
    pAttribute = &pObj->pAttributeArray[i];
    
    if (isAttributeEnabled(pAttribute->flags) &&
        isAttributeReportable(pAttribute->flags) &&        
        !isAttributeInValueMap(pAttribute->flags)
       ) 
    {    
      count++;
      offset += attrlen;
      attrlen = DumpValue(pAttribute,  (uint8_t*)(((int)pBuff+offset)), TRUE);
      totalLength += attrlen;    
    }            
  }
          
  pAttrList->count = Native2HcOTA16(count);
  pAttrList->length = Native2HcOTA16(totalLength);

  return totalLength + 4; /* add size of attrcount,attrlen fields */
  
}


/******************************************************************************
  Dumps the attribute's value
  
  pAttribute - source attribute
  pBuffer - destination buffer
  includeAttrHeader -  Attr-id and Attr-length fields included or not
  
  Returns the number of bytes.
******************************************************************************/
uint16_t DumpValue(objectAttribute_t* pAttribute, uint8_t *pBuffer, bool_t includeAttrHeader)
{
  
  uint16_t len;    
  void *pSrc, *pDst;
  ava_t *pDstAttr;  
  
  if (includeAttrHeader) 
  {    
    pDstAttr = (ava_t *)pBuffer;  
    pDstAttr->attributeId = Native2HcOTA16(pAttribute->attributeId);
    pDst = (void*)&(pDstAttr->attributeValue.value[0]);
  } 
  else 
  {
    pDst = (void*)pBuffer;
  }
    
  
        
  /* Find the location of the attribute value */  
  if (isAttributeValueInline(pAttribute->flags)) 
  {    
    pSrc = (void*)&(pAttribute->value);
  #if gBigEndian_c
    if (pAttribute->length == 1)
      ((uint8_t*)pSrc)+=3;
    else if (pAttribute->length == 2)
      ((uint8_t*)pSrc)+=2;
  #endif
  }
  else 
  {    
    pSrc = (void*)(pAttribute->value.data32);///pData;    
  }
  
  
  
  switch (pAttribute->attributeId)   
  {
  
  case MDC_ATTR_SYS_TYPE_SPEC_LIST:
    len = DumpSysTypeSpecList((uint8_t*)pDst, (typeVerList_t*)pSrc);
    break;

  case MDC_ATTR_NU_CMPD_VAL_OBS_BASIC:
  case MDC_ATTR_ID_PHYSIO_LIST:
     len = DumpBasicValList((uint8_t*)pDst, (basicValList_t*) pSrc);
     break;
     
  case MDC_ATTR_TIME_BATT_REMAIN:
    len = DumpBatteryRemain((uint8_t*)pDst, (batMeasure_t*) pSrc);
    break;
    
  case MDC_ATTR_ID_MODEL:
    len = DumpSystemModel((uint8_t*)pDst, (systemModel_t*) pSrc);
    break;
    
  case MDC_ATTR_SYS_ID:
    len = DumpVarLenString((uint8_t*)pDst, (uint8_t*) pSrc, pAttribute->length);
    break;
    
  default:
    
    if (pAttribute->length > 4 && !isAttributeValueInline(pAttribute->flags))
      /* 
         Assume that values for which length is > 4 are stored as arrays
         and therefore endianess independent.
         E.g. See the MDC_ATTR_SYS_ID definition in Oep11073Config.c
      */
      FLib_MemCpy(pDst, pSrc, (uint8_t)pAttribute->length);
    else
      /* For the other cases, mind for the endianess */
      MemCpyNative2HcOta(pDst, pSrc, (uint8_t)pAttribute->length);      
      
    len = pAttribute->length;  
  }    
  
  if (includeAttrHeader)
  {    
    pDstAttr->attributeValue.length = Native2HcOTA16(len);
    return len + 4;
  }
  else
    return len;
      
}



/******************************************************************************
  Handlers for different attribute id's
  
  pDst - destination buffer
  pSrc - source buffer
    
  Returns the number of bytes.
******************************************************************************/



/******************************************************************************
    Handler for MDC_ATTR_SYS_TYPE_SPEC_LIST  
    Format is: 
    count(2)|length(2)|type_1(2)|version_1(2)|...|type_n(2)|version_n(2)
******************************************************************************/
uint16_t DumpSysTypeSpecList( uint8_t *pDst, typeVerList_t *pSrc)
{
  uint8_t i;
  uint16_t len;


  len = sizeof(typeVer_t) * pSrc->count;
    
  //*(uint16_t*)pDst = Native2HcOTA16(pSrc->count);
  MemCpyNative2HcOta(pDst, &pSrc->count, 2);
  
  pDst += 2;
  
  //*(uint16_t*)(pDst) = Native2HcOTA16(len);
  MemCpyNative2HcOta(pDst, &len, 2);
  
  
  for (i = 0; i < pSrc->count; i++) 
  {
    pDst += 2;
    
    // handle memory aligment
    //*(uint16_t*)(pDst) = Native2HcOTA16(pSrc->value[i].type);
    MemCpyNative2HcOta(pDst, &pSrc->value[i].type, 2);
    
    pDst += 2;
    // handle memory aligment
    //*(uint16_t*)(pDst) = Native2HcOTA16(pSrc->value[i].version);
    MemCpyNative2HcOta(pDst, &pSrc->value[i].version, 2);
    
  }
  
  return (len + 4); /* Add length + count fields */
  
}



/******************************************************************************
    Handler for lists of 2byte values
        MDC_ATTR_NU_CMPD_VAL_OBS_BASIC,
        MDC_ATTR_ID_PHYSIO_LIST
        
    Format is: 
    count(2)|length(2)|val1(2)|...|val_n(2)
******************************************************************************/
uint16_t DumpBasicValList(uint8_t *pDst, basicValList_t *pSrc) 
{

  uint8_t i;
  uint16_t len;
  
  len = sizeof(basicNuObsValue_t) * pSrc->count;

  //*(uint16_t*)pDst = Native2HcOTA16(pSrc->count);
  MemCpyNative2HcOta(pDst, &pSrc->count, 2);
  
  pDst += 2;
  // handle memory alignment
  //*(uint16_t*)(pDst) = Native2HcOTA16(len);
  MemCpyNative2HcOta(pDst, &len, 2);

  for (i = 0; i < pSrc->count; i++)
  {      
    pDst += 2;
    // handle memory alignment
    //*(uint16_t*)(pDst) = Native2HcOTA16(pSrc->value[i]);
    MemCpyNative2HcOta(pDst, &pSrc->value[i], 2);
  }
  
  return (len + 4); /* Add length + count fields */  
}

/******************************************************************************
    Handler for batMeasure_t attribute type

    
******************************************************************************/
uint16_t DumpBatteryRemain(uint8_t *pDst, batMeasure_t *pSrc) 
{

  MemCpyNative2HcOta(pDst, (uint8_t*)&pSrc->value, 4); 
  
  pDst += 4;
  
  MemCpyNative2HcOta(pDst, &pSrc->unit, 2);

  
  return (sizeof(batMeasure_t)); /* Add length + count fields */  
}



/******************************************************************************
    Handler for systemModel_t attribute type

    
******************************************************************************/
uint16_t DumpSystemModel(uint8_t *pDst, systemModel_t *pSrc) 
{
  
  uint16_t len;
  
  len = pSrc->manufacturerLen + pSrc->modelNumberLen + 2*sizeof(uint16_t); /* also add the size of the two length fields */
  
 
  //*(uint16_t*)(pDst) = Native2HcOTA16(pSrc->manufacturerLen);
  MemCpyNative2HcOta(pDst, &pSrc->manufacturerLen, 2);
  pDst += 2;
 
  /* No need to reverse string for little endian platforms. */
  FLib_MemCpy(pDst, pSrc->manufacturer, (uint8_t)pSrc->manufacturerLen);
  pDst += pSrc->manufacturerLen;
  
   // handle memory alignment
  //*(uint16_t*)(pDst) = Native2HcOTA16(pSrc->modelNumberLen);
  MemCpyNative2HcOta(pDst, &pSrc->modelNumberLen, 2);
  
  pDst += 2;
   
  /* No need to reverse string for little endian platforms. */
  FLib_MemCpy(pDst, pSrc->modelNumber, (uint8_t)pSrc->modelNumberLen);
   

  
  return (len);
}



/******************************************************************************
    Handler for string with variable length
    Adds the 16bit length before the actual string.    
******************************************************************************/
uint16_t DumpVarLenString(uint8_t *pDst, uint8_t *pSrc, uint16_t strLen)
{
  uint16_t len;
  
  len = strLen + 2; /* add the size of the length field */
  
   // handle memory alignment
  //*(uint16_t*)(pDst) = Native2HcOTA16(strLen);
  MemCpyNative2HcOta(pDst, &strLen, 2);  
  
  pDst += 2;
  
  FLib_MemCpyReverseOrder(pDst, pSrc, (uint8_t)strLen);
  
  return (len);
  
}

#endif
