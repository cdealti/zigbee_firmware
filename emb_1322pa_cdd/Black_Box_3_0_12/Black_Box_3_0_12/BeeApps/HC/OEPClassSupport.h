/******************************************************************************
* Class definition as structures for 11073-104zz, alongside utility functions
*
* Copyright (c) 2009, Freescale, Inc.  All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
******************************************************************************/

#ifndef _OEP_CLASSES_INTERFACE_H
#define _OEP_CLASSES_INTERFACE_H

#include "OEPTypes.h"
#include "EmbeddedTypes.h"

/******************************************************************************
*******************************************************************************
* Public macros
*******************************************************************************
******************************************************************************/


/* Flags field for the  objectAttribute_t structure */

#define gAttrFlagNoFlags_d     0
#define gAttrFlagInline_d      (1 << 0) /* Attribute is disabled */
#define gAttrFlagDisabled_d    (1 << 1) /* Attribute is disabled */
#define gAttrFlagNoReport_d    (1 << 2) /* Attribute is not reportable */
#define gAttrFlagInValueMap_d  (1 << 3) /* Attribute is contained in ValueMap */




#define isAttributeEnabled(flags) (!((flags) & gAttrFlagDisabled_d))
#define isAttributeReportable(flags) (!((flags) & gAttrFlagNoReport_d))
#define isAttributeInValueMap(flags) ((flags) & gAttrFlagInValueMap_d)
#define isAttributeValueInline(flags) ((flags) & gAttrFlagInline_d)


#define getAttributeType(flags) ((flags) & gAttrFlagTypeMask_d)



#define enableAttribute(flags)  ((flags) &= ~gAttrFlagDisabled_d)
#define disableAttribute(flags) ((flags) |=  gAttrFlagDisabled_d)





//The size of the object header: class id, handle, attribute count and length
#define gCsObjectHeaderLen_d  8   

//Macros for release request and release response.
#define oepCsCreateRlrq(pOutBuffer, pBufferLength, releaseReason)  \
    oepCsCreateReleaseFrame(gRlrq_d, pOutBuffer, pBufferLength, releaseReason)

#define oepCsCreateRlre(pOutBuffer, pBufferLength, responseReason) \
    oepCsCreateReleaseFrame(gRlre_d, pOutBuffer, pBufferLength, responseReason)



/******************************************************************************
*******************************************************************************
* Public prototypes
*******************************************************************************
******************************************************************************/


/******************************************************************************
*******************************************************************************
* Public type definitions
*******************************************************************************
******************************************************************************/

#ifdef __IAR_SYSTEMS_ICC__
#pragma pack(1)
#endif

typedef struct oepGenericApdu_tag {
  uint16_t  apduLen;
  uint16_t  choice;
  uint8_t   payload[1];
} oepGenericApdu_t;



typedef union objectAttrData_tag
{  
  uint32_t   data32;  
  uint16_t   data16; 
  uint8_t    data8;
  void       *pData;
} objectAttrData_t;


typedef struct objectAttribute_tag 
{ 
  oidType_t   attributeId;
  uint8_t     flags;
  uint16_t     length;  
  objectAttrData_t      value;
} objectAttribute_t;


typedef struct genericClass_tag
{
  oidType_t   classId;
  oepHandle_t handle;
  bool_t      includeInConfigReport;
  uint16_t    attributeCount;
  objectAttribute_t* pAttributeArray;
} genericClass_t;

typedef uint8_t oepClassSupportErr_t;
#define gOepCsSucess_d                0
#define gOepCsError_d                 1
#define gOepCsInputParamError_d       2
#define gOepCsAttributeNotFound_d     3
#define gOepCsAttributeNotEnabled_d   4
#define gOepCsAttributeEnabled_d      5
#define gOepCsObjectNotFound_d        6

 
#ifdef __IAR_SYSTEMS_ICC__
#pragma pack()
#endif

/******************************************************************************
*******************************************************************************
* Public memory declarations
*******************************************************************************
******************************************************************************/
extern genericClass_t* (*pObjectArray)[];

/******************************************************************************
*******************************************************************************
* Public functions
*******************************************************************************
******************************************************************************/
oepClassSupportErr_t  oepCsGetAttributeByType(genericClass_t* pObject, oidType_t type, objectAttribute_t** pReturnValue);
oepClassSupportErr_t  oepCsEnableByAttributeType(genericClass_t* pObject, oidType_t type, bool_t enable);
oepClassSupportErr_t  oepCsGetObjectLength(genericClass_t* pObject, bool_t includeDisabledAttr, int16_t* pReturnValue);
oepClassSupportErr_t  oepCsDumpObject(genericClass_t* pObject, bool_t includeDisabledAttr, uint8_t* pBuffer, uint16_t* pBytesWritten);

oepClassSupportErr_t  oepCsGetObjectsHandlesTypes(uint8_t* pBuffer, uint16_t* pReturnValue);
oepClassSupportErr_t  oepCsGetObjectByHandle(oepHandle_t handle, genericClass_t** pReturnValue);
oepClassSupportErr_t  oepCsDumpObjectDescriptor(genericClass_t* pObject, uint8_t* pBuffer, uint16_t* pBytesWritten);
oepClassSupportErr_t  oepCsDumpAttribute(oepHandle_t handle, oidType_t attrType, uint8_t* pBuffer, uint16_t* pBytesWritten);
oepClassSupportErr_t  oepCsSetAttribute(oepHandle_t handle, oidType_t attrType, uint8_t* pBuffer, uint16_t length);

void oepCsManagerHandleConfigMsg(oepGenericApdu_t* pInApdu, uint8_t* buffer, uint16_t bufferSize, uint8_t srcEndpoint);
oepClassSupportErr_t oepCsCreateAssociateReq(uint8_t* pOutBuffer, uint16_t* pBufferLength);
oepClassSupportErr_t oepCsCreateAssociateRes(uint8_t* pOutBuffer, associateResult_t result, uint16_t* pBufferLength);
oepClassSupportErr_t oepCsVerifyAssociateReq(aarqOepApdu_t* pAarq, associateResult_t *associateResult);
oepClassSupportErr_t oepCsGenerateConfigRes(uint8_t* pOutBuffer, uint16_t* pBufferLength, uint16_t invokeId, configId_t recConfigId, configResult_t configResult);
oepClassSupportErr_t oepCsCreateReportConfig(uint8_t* pOutBuffer, uint16_t* pBufferLength, uint16_t invokeId);

extern oepClassSupportErr_t oepCsCreateDataReport(uint8_t  *pOutBuffer, uint16_t *pBufferLength, uint16_t invokeId, uint16_t dataReqId, uint16_t scanReportNo);
extern oepClassSupportErr_t oepCsCreateDataResponse(uint8_t* pOutBuffer, uint16_t* pBufferLength, uint16_t invokeId);

oepClassSupportErr_t oepCsCreateReleaseFrame(uint16_t type, uint8_t* pOutBuffer, uint16_t* pBufferLength, uint16_t reason);
oepClassSupportErr_t oepCsCreateGetAllAtributesFrame(uint8_t* pOutBuffer, uint16_t* pBufferLength, uint16_t invokeId, oepHandle_t handle);
oepClassSupportErr_t oepCsCreateGetMdsResponseFrame(uint8_t* pOutBuffer, uint16_t* pBufferLength, uint16_t invokeId);

#endif /*_OEP_CLASSES_INTERFACE_H */
