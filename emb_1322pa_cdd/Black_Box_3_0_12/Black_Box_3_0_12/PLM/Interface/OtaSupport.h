/************************************************************************************
*
*(c) Copyright 2010, Freescale, Inc.  All rights reserved.
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale.
*
************************************************************************************/

#ifndef _OTA_SUPPORT_H_
#define _OTA_SUPPORT_H_

/************************************************************************************
*************************************************************************************
* Public macros
*************************************************************************************
************************************************************************************/
//gUseInternalFlashForOta_c set to 0 will require an external FLASH for image storage
//gUseInternalFlashForOta_c set to 1 will store the received image in the internal FLASH
#define gUseInternalFlashForOta_c         FALSE

/************************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Public type definitions
*************************************************************************************
************************************************************************************/
typedef enum
{
  gOtaSucess_c = 0,
  gOtaNoImage_c,
  gOtaUpdated_c,
  gOtaError_c,
  gOtaCrcError_c,
  gOtaInvalidParam_c,
  gOtaInvalidOperation_c,
  gOtaExternalFlashError_c,
  gOtaInternalFlashError_c,
} otaResult_t;

/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/

/************************************************************************************
*  Starts the process of writing a new image to the external FLASH.
*
*  Input parameters:
*  - lenght: the lenght of the image to be written in the external FLASH
*  Return:
*  - gOtaInvalidParam_c: the intended lenght is bigger than the FLASH capacity
*  - gOtaInvalidOperation_c: the process is already started (can be cancelled)
*  - gOtaExternalFlashError_c: can not detect extrenal FLASH type, of external FLASH
*       is not SST
************************************************************************************/
extern otaResult_t OTA_StartImage(uint32_t length);

/************************************************************************************
*  Places the next image chunk into the external FLASH.
*
*  Input parameters:
*  - pData: pointer to the data chunk
*  - length: the length of the data chunk
*  - pImageLength: if it is not null and the function call is sucessfull, it will be
*       filled with the current lenght of the image
*  Return:
*  - gOtaInvalidParam_c: pData is NULL or the resulting image would be bigger than the 
*       final image length specified with OTA_StartImage()
*  - gOtaInvalidOperation_c: the process is not started
************************************************************************************/
extern otaResult_t OTA_PushImageChunk(uint8_t* pData, uint16_t length, uint32_t* pImageLength);

/************************************************************************************
*  Finishes the writing of a new image to the external FLASH.
*  It will write the image header (signature and lenght) and footer (sector copy bitmap)
*  and CRC.
*
*  Input parameters:
*  - secured: if TRUE, write the secure image signature, unsecure otherwise
*  - bitmap: 32 bit unsigned integer indicating the sector erase pattern for the
*       internal FLASH before the image update. The MSB of the first byte controls the
*       erasing of sector 0, etc. The LSB should control the erasing of sector 31 but 
*       is ignored, as the last sector of the internal FLASH holds IC production data.
*  Return:
*  - gOtaInvalidOperation_c: the current image lenght is not equal with the 
*       final image length specified with OTA_StartImage(), or the process is not started,
*       or the bitmap bits do not instruct the erasing of enough sectors to contain the image.
*  - gOtaExternalFlashError_c: error while trying to write the external FLASH 
************************************************************************************/
otaResult_t OTA_CommitImage(bool_t secured, uint32_t bitmap);

/*****************************************************************************
*  OTA_SetNewImageFlag
*
*  It is called to set the new image flags (image signature)  
*****************************************************************************/
void OTA_SetNewImageFlag(void);

/************************************************************************************
*  Cancels the process of writing a new image to the external FLASH.
*
*  Input parameters:
*  - None
*  Return:
*  - None
************************************************************************************/
extern void OTA_CancelImage();

/*****************************************************************************
*  OTA_EraseMemory
*
*  This function is called in order to erase external flash or eeprom
*
*****************************************************************************/
otaResult_t OTA_EraseExternalMemory(void);

/*****************************************************************************
*  OTA_InitExternalMemory
*
*  This function is called in order to init external flash or eeprom
*
*****************************************************************************/
void OTA_InitExternalMemory(void);

/*****************************************************************************
*  OTA_WriteFlash
*
*  This function is called in order to write in external flash or eeprom
*
*****************************************************************************/
otaResult_t OTA_WriteExternalMemory(uint8_t* pData, uint8_t length, uint32_t address);

/*****************************************************************************
*  OTA_ReadExternalMemory
*
*  This function is called in order to read from external flash or eeprom
*
*****************************************************************************/
otaResult_t OTA_ReadExternalMemory(uint8_t* pData, uint8_t length, uint32_t address);

/************************************************************************************
*  Updates the CRC based on the received data to process.
*  Updates the global CRC value. This was determined to be optimel from a resource 
*  consumption POV.
*
*  Input parameters:
*  - None
*  Return:
*  - None
************************************************************************************/
uint16_t OTA_CrcCompute(uint8_t *pData, uint16_t lenData, uint16_t crcValueOld);
#endif //_OTA_SUPPORT_H_
