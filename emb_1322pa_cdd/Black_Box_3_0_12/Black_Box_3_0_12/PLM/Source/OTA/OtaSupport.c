/************************************************************************************
* OTA cluster support control routines for access to internal and external flash memory
* (c) Copyright 2010, Freescale, Inc.  All rights reserved.
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale.
*
************************************************************************************/

/************************************************************************************
************************************************************************************
* Private macros
*************************************************************************************
************************************************************************************/
#include "EmbeddedTypes.h"
#include "FunctionLib.h"
#include "Platform.h"
#include "OtaFlashUtils.h"
#include "OtaSupport.h"


#define SECTOR_SIZE         0x1000
#define FLASH_SIZE          0x20000
#define RAM_SIZE            0x18000
#define INTERNAL_FLASH      FLASH_REGS_P
#define EXTERNAL_FLASH      SPI_REGS_P
#define mNvFlashCmdRetries_c 64

// address of OTA image length is set in flash at 
// the beginning of the second sector (RAM addr 0x0FF8)
#define APP_IMAGE_LEN_ADDRESS_FLASH_START   0x1000

#if(gUseInternalFlashForOta_c == 0)
#define USED_FLASH          gNvmExternalInterface_c
#define USED_FLASH_OTA     EXTERNAL_FLASH
#define FLASH_ERROR         gOtaExternalFlashError_c
#define SECTORS_TO_ERASE    32
#else
#define USED_FLASH          gNvmInternalInterface_c
#define USED_FLASH_OTA     INTERNAL_FLASH
#define FLASH_ERROR         gOtaInternalFlashError_c
#define SECTORS_TO_ERASE    31
#endif

  

//Size of the buffer to be allocated on the stack for FLASH reads. The function calls
// where this size is allocated should be done at the beginning of the app execution
// when the stack is not utilized.
#define FLASH_READS_ON_STACK  128 

#define IMAGE_HEADER    8                   //Image signature and length
#define IMAGE_FOOTER    6                   //CRC and internal FLASH sector erase pattern
#define MAX_IMAGE_LEN   (128*1024 - 4*1024 - IMAGE_HEADER - IMAGE_FOOTER) //Max space for the new image
#define MAX_LEN  0x176F8


/************************************************************************************
*************************************************************************************
* Private type definitions
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Private prototypes
*************************************************************************************
************************************************************************************/

#if(gUseInternalFlashForOta_c == 0)
static void           otaEnableExtSPI();
static void           otaCrcCompute(uint8_t *pData, uint16_t lenData);
#endif

/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
************************************************************************************/
const uint8_t   securedString[] = "SECU";   //Signature for a valid, secure image
const uint8_t   unsecuredString[] = "OKOK"; //Signature for a valid, unsecure image

static bool_t   gImageInProcess = FALSE;    //TRUE is we are in the process of writinga new image to the external FLASH
static uint32_t gImageLength = 0;           //While writing a new image, the intended final length of the image
static uint32_t gCurrentImageLength = 0;    //While writing a new image, the number of bytes written
static uint32_t gCurrentAddress = 0;        //Current write address in the external FLASH

static bool_t gSecured = FALSE;             //if TRUE, write the secure image signature, unsecure otherwise

#if(gUseInternalFlashForOta_c == 0)
static uint16_t gCrcValue = 0;              //Current CRC value
#endif


/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************

************************************************************************************/
/************************************************************************************/
otaResult_t OTA_StartImage(uint32_t length)
{
  
#if(gUseInternalFlashForOta_c != 0)
  volatile uint16_t wait = 0xFF;
#endif  
  
  nvmType_t nvmType = gNvmType_NoNvm_c;

  //Verify the validity of the operation and received parameter
  if(gImageInProcess) return gOtaInvalidOperation_c;
  if(length > MAX_IMAGE_LEN) return gOtaInvalidParam_c;

#if(gUseInternalFlashForOta_c == 0)  
  //Detect the external FLASH type
  otaEnableExtSPI();
#else  
  //Ensure that the internal NVM regulator is on as the bootstrap leaves the NVM regulator off
  OTA_NVM_StartNvmRegulator();
  //Wait a little
  while(wait--);
#endif
  
  if( gNvmErrNoError_c == NVM_Detect(USED_FLASH, &nvmType))
  {
    if(gNvmType_SST_c !=  nvmType)
      return FLASH_ERROR;
  }  
  else
    return FLASH_ERROR;

  //Mark that we start receiving a new image
  gImageInProcess = TRUE;
  
  //Save the intended image length, init the current image length and CRC
  gImageLength = length;
  gCurrentImageLength = 0;

#if(gUseInternalFlashForOta_c == 0)  
  gCrcValue = 0;
#endif
 
  //Erase the FLASH memory used
  for(gCurrentAddress = 0; gCurrentAddress < SECTORS_TO_ERASE; gCurrentAddress++)
      OTA_NVM_EraseSector(USED_FLASH_OTA, gCurrentAddress << 12);
  
  //Initialize the current write address
  gCurrentAddress = IMAGE_HEADER;
  
  return gOtaSucess_c;
}



/************************************************************************************/
otaResult_t OTA_PushImageChunk(uint8_t* pData, uint16_t length, uint32_t* pImageLength)
{
  nvmErr_t nvmResult;
#if(gUseInternalFlashForOta_c == 0)    
  uint8_t lenPositionInChunk;
  uint32_t currentChunkEndAddress;
  uint8_t bytesToCopy;
  bool_t putLengthInChunk = FALSE;
  uint8_t startAddressofLenCopy = 0;
#endif  
  index_t retries = mNvFlashCmdRetries_c;    
 
  
  //Verify the validity of the operation and received parameters
  if(!gImageInProcess) return gOtaInvalidOperation_c;
  if((length == 0) || (pData == NULL)) return gOtaInvalidParam_c;
  if(gCurrentImageLength + length > gImageLength) return gOtaInvalidParam_c;

#if(gUseInternalFlashForOta_c == 0)
    // put the image length in the image at flash address 0x1000 so
  // that the ota bootloader can read from there; the length may fall
  // between chunks as their lengths are variable. this handles all cases
  currentChunkEndAddress = gCurrentAddress + length;
  
  // if address of image len (0x1000) is in the current chunk
  if (gCurrentAddress <= APP_IMAGE_LEN_ADDRESS_FLASH_START &&
      currentChunkEndAddress > APP_IMAGE_LEN_ADDRESS_FLASH_START)
  {
    // will copy at least one len byte
    putLengthInChunk = TRUE;
    
    // worst case is when currentChunkEndAddress = 0x1001
    bytesToCopy = currentChunkEndAddress - APP_IMAGE_LEN_ADDRESS_FLASH_START;
    
    // else if distance between end of chunk and 0x1000 >= 5 means
    // the full len will fit in current chunk so truncate to 4 bytes
    if (bytesToCopy > sizeof(uint32_t))
      bytesToCopy = sizeof(uint32_t);
    
    // compute offset address to copy len in current chunk;
    lenPositionInChunk = APP_IMAGE_LEN_ADDRESS_FLASH_START - gCurrentAddress;
  }
  // if address of image len (0x1000) is not in current chunk but
  // end the chunk start address is nevertheless <= 1003
  else if (gCurrentAddress > APP_IMAGE_LEN_ADDRESS_FLASH_START &&
           gCurrentAddress - APP_IMAGE_LEN_ADDRESS_FLASH_START < sizeof(uint32_t))
  {
    // will copy at least one byte
    putLengthInChunk = TRUE;
    
    // will copy from 1, 2, or 3
    startAddressofLenCopy = gCurrentAddress - APP_IMAGE_LEN_ADDRESS_FLASH_START;
    
    // we know this is smaller than 4 (eg: if gCurrentAddr = 1001 - will copy 3 bytes)
    bytesToCopy = sizeof(uint32_t) - startAddressofLenCopy;
       
    // chunk length may still be smaller so truncate if so
    if (length < bytesToCopy)
      bytesToCopy = length;
    
    // compute offset address to copy len in current chunk - this will be 0
    lenPositionInChunk = 0;
    
  }
  if (putLengthInChunk)
    FLib_MemCpy(pData+lenPositionInChunk, ((uint8_t*)(&gImageLength)) + startAddressofLenCopy, bytesToCopy);
#endif
  
  //Write the data chunk to the FLASH
  while ( retries-- )
  {
    nvmResult = NVM_Write(USED_FLASH, gNvmType_SST_c, (void*)pData, gCurrentAddress, length);
    if(gNvmErrNoError_c == nvmResult)
      break;
  }
  
  if(nvmResult != gNvmErrNoError_c) return FLASH_ERROR;

#if(gUseInternalFlashForOta_c == 0)  
  //Update the CRC, only used with safe mode
  otaCrcCompute(pData, length);
#endif
  
  //All OK, update operation parameters and return
  gCurrentAddress += length;
  gCurrentImageLength += length;
  if(pImageLength != NULL) *pImageLength = gCurrentImageLength;
  
  return gOtaSucess_c;
}

/************************************************************************************/
otaResult_t OTA_CommitImage(bool_t secured, uint32_t bitmap)
{
  nvmErr_t nvmResult;
  uint8_t erasedSectors;
  uint32_t bit;
  uint8_t idx;
  index_t retries = mNvFlashCmdRetries_c;   
#if(gUseInternalFlashForOta_c == 0) 
  uint32_t maxLen = MAX_LEN;
#endif
  //Verify the validity of the operation and received parameters
  if(!gImageInProcess) return gOtaInvalidOperation_c;
  if(gCurrentImageLength != gImageLength) return gOtaInvalidOperation_c;
  // Validate that the bitmap requires enough sectors to be erased in the internal 
  // FLASH so that the new image will be correctly written.
  
  erasedSectors = 0;
  bit = 0x80000000;
  
  //Count the consecutive number of sectors to be erased
  for(idx = 0; idx < 31; idx++)
  {
    if(bit & bitmap)
      erasedSectors++;  
    else
      break;    
    bit = bit >> 1;
  }
  
  //Compare with the number of sectors that will have to be written
  if(erasedSectors < (((gImageLength + 8) >> 12) + 1))
    return gOtaInvalidOperation_c;

  //Write the sector bitmap and update the CRC
  while ( retries-- )
  {
    nvmResult = NVM_Write(USED_FLASH, gNvmType_SST_c, (void*)&bitmap, gCurrentAddress, 4);
    if(gNvmErrNoError_c == nvmResult)
      break;
  }
  
  if (nvmResult != gNvmErrNoError_c)
    return FLASH_ERROR;

  gCurrentAddress += 4;

#if(gUseInternalFlashForOta_c == 0)  
  //Update the CRC
  otaCrcCompute((uint8_t*)&bitmap, 4);
  //Write the CRC
  retries = mNvFlashCmdRetries_c;
  while ( retries-- )
  {  
    nvmResult = NVM_Write(USED_FLASH, gNvmType_SST_c, (void*)&gCrcValue, gCurrentAddress, 2);
    if(gNvmErrNoError_c == nvmResult)
      break;    
  }
  if(nvmResult != gNvmErrNoError_c) return FLASH_ERROR;
#endif  
  
//Write the image length
  retries = mNvFlashCmdRetries_c;
  while ( retries-- )
  {    
#if(gUseInternalFlashForOta_c == 0) 
    nvmResult = NVM_Write(USED_FLASH, gNvmType_SST_c, (void*)&maxLen, 4, 4);
#else
    nvmResult = NVM_Write(USED_FLASH, gNvmType_SST_c, (void*)&gImageLength, 4, 4);
#endif
    if(gNvmErrNoError_c == nvmResult)
      break;    
  }
  if(nvmResult != gNvmErrNoError_c) return FLASH_ERROR;
  
  gSecured = secured;
  
  //Write the image signature     
/*  retries = mNvFlashCmdRetries_c;
  while ( retries-- )
  {        
    if(secured)
      nvmResult = NVM_Write(USED_FLASH, gNvmType_SST_c, (void*)securedString, 0, 4);
    else
      nvmResult = NVM_Write(USED_FLASH, gNvmType_SST_c, (void*)unsecuredString, 0, 4);
    if(gNvmErrNoError_c == nvmResult)
      break;        
  }
  if(nvmResult != gNvmErrNoError_c) return FLASH_ERROR;
  
 */ 
  //Prepare for next image
  gImageInProcess = FALSE;
  
  return gOtaSucess_c;
}

/*****************************************************************************
*  OTA_SetNewImageFlag
*
*  It is called to set the new image flags (image signature)  
*****************************************************************************/
void OTA_SetNewImageFlag(void)
{
  nvmErr_t nvmResult;
  index_t retries = mNvFlashCmdRetries_c;  
  
  //Write the image signature  
    while ( retries-- )
    {        
      if(gSecured)
        nvmResult = NVM_Write(USED_FLASH, gNvmType_SST_c, (void*)securedString, 0, 4);
      else
        nvmResult = NVM_Write(USED_FLASH, gNvmType_SST_c, (void*)unsecuredString, 0, 4);
      if(gNvmErrNoError_c == nvmResult)
        break;        
    }
    if(nvmResult != gNvmErrNoError_c) return;
}

/************************************************************************************/
void OTA_CancelImage()
{
  gImageInProcess = FALSE;
}

/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/
/*****************************************************************************
*  OTA_EraseMemory
*
*  This function is called in order to erase external flash or eeprom
*
*****************************************************************************/
otaResult_t OTA_EraseExternalMemory(void)
{
  uint8_t currentSector;

  for(currentSector = 0; currentSector < SECTORS_TO_ERASE; currentSector++)
  {
    OTA_NVM_EraseSector(EXTERNAL_FLASH, currentSector << 12);
  }
  return gOtaSucess_c;
}

/*****************************************************************************
*  OTA_InitExternalMemory
*
*  This function is called in order to init external flash or eeprom
*
*****************************************************************************/
void OTA_InitExternalMemory(void)
{
#if(gUseInternalFlashForOta_c == 0) 
    /* Initialize external SPI */
    otaEnableExtSPI();
#endif    
}

/*****************************************************************************
*  OTA_WriteFlash
*
*  This function is called in order to write in external flash or eeprom
*
*****************************************************************************/
otaResult_t OTA_WriteExternalMemory(uint8_t* pData, uint8_t length, uint32_t address)
{
  uint8_t retries = mNvFlashCmdRetries_c;
  
  while ( retries-- )
  {   
    if(gNvmErrNoError_c == NVM_Write(USED_FLASH, gNvmType_SST_c, (void*)pData,address, length))
    {
      return gOtaSucess_c;  
    }  
  }
  return gOtaExternalFlashError_c;
}

/*****************************************************************************
*  OTA_ReadExternalMemory
*
*  This function is called in order to read from external flash or eeprom
*
*****************************************************************************/
otaResult_t OTA_ReadExternalMemory(uint8_t* pData, uint8_t length, uint32_t address)
{
  uint8_t  retries = mNvFlashCmdRetries_c;
  
  while ( retries-- )
  {    
    if(gNvmErrNoError_c == NVM_Read(USED_FLASH, gNvmType_SST_c, (void*)pData,address, length))
    {
      return gOtaSucess_c;  
    }  
  }
  return gOtaExternalFlashError_c;
}

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
uint16_t OTA_CrcCompute(uint8_t *pData, uint16_t lenData, uint16_t crcValueOld)
{
  
  uint8_t i;
  
  while(lenData--)
  {
    crcValueOld ^= *pData++ << 8;
    for( i = 0; i < 8; ++i )
    {
      if( crcValueOld & 0x8000 )
      {
	crcValueOld = (crcValueOld << 1) ^ 0x1021;
      }
      else
      {
        crcValueOld = crcValueOld << 1;
      }
    }
  }
  return crcValueOld;
}
/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/
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

#if(gUseInternalFlashForOta_c == 0)
static void otaCrcCompute(uint8_t *pData, uint16_t lenData)
{
  while(lenData--)
  {
    uint8_t i;
    gCrcValue ^= *pData++ << 8;
    for( i = 0; i < 8; ++i )
    {
      if( gCrcValue & 0x8000 )
      {
        gCrcValue = (gCrcValue << 1) ^ 0x1021;
      }
      else
      {
        gCrcValue = gCrcValue << 1;
      }
    }
  }
}

/************************************************************************************
*  Enables the extrenal SPI. By default this is disabled.
*  Updates the global CRC value. This was determined to be optimel from a resource 
*  consumption POV.
*
*  Input parameters:
*  - None
*  Return:
*  - None
************************************************************************************/
#define GPIO_BAR                    0x80000000
#define gpio_data_or_pad_in_lo        (*((volatile unsigned int *)(GPIO_BAR + 0x08)))  /* Part 1 of gpio_data_or_pad_in  */
#define gpio_data_or_pad_in_hi        (*((volatile unsigned int *)(GPIO_BAR + 0x0c)))  /* Part 2 of gpio_data_or_pad_in  */
#define gpio_pad_pu_en_lo             (*((volatile unsigned int *)(GPIO_BAR + 0x10)))  /* Part 1 of gpio_pad_pu_en       */
#define gpio_pad_pu_en_hi             (*((volatile unsigned int *)(GPIO_BAR + 0x14)))  /* Part 2 of gpio_pad_pu_en       */
#define gpio_func_sel_0               (*((volatile unsigned int *)(GPIO_BAR + 0x18)))  /* Part 1 of gpio_func_sel        */
#define gpio_func_sel_1               (*((volatile unsigned int *)(GPIO_BAR + 0x1c)))  /* Part 2 of gpio_func_sel        */
#define gpio_func_sel_2               (*((volatile unsigned int *)(GPIO_BAR + 0x20)))  /* Part 3 of gpio_func_sel        */
#define gpio_data_sel_lo              (*((volatile unsigned int *)(GPIO_BAR + 0x28)))  /* Part 1 of gpio_data_sel        */
#define gpio_data_sel_hi              (*((volatile unsigned int *)(GPIO_BAR + 0x2c)))  /* Part 2 of gpio_data_sel        */
#define gpio_pullup_sel_lo            (*((volatile unsigned int *)(GPIO_BAR + 0x30)))  /* Part 1 of gpio_pullup_sel      */
#define gpio_pullup_sel_hi            (*((volatile unsigned int *)(GPIO_BAR + 0x34)))  /* Part 2 of gpio_pullup_sel      */
#define gpio_data_dir_set_lo          (*((volatile unsigned int *)(GPIO_BAR + 0x58)))  /* Part 1 of gpio_data_dir_set    */
#define gpio_data_dir_set_hi          (*((volatile unsigned int *)(GPIO_BAR + 0x5c)))  /* Part 2 of gpio_data_dir_set    */
#define gpio_data_dir_reset_lo        (*((volatile unsigned int *)(GPIO_BAR + 0x60)))  /* Part 1 of gpio_data_dir_reset  */
#define gpio_data_dir_reset_hi        (*((volatile unsigned int *)(GPIO_BAR + 0x64)))  /* Part 2 of gpio_data_dir_reset  */

static void otaEnableExtSPI()
{
  gpio_func_sel_0 &= (~0xFF00);// configure as gpio
  gpio_data_dir_reset_lo  = GPIO_SPI_SS_bit;//configure spi_ss pin as an input
  gpio_data_sel_lo &=(~GPIO_SPI_SS_bit);//data is read from the PAD
  gpio_pullup_sel_lo |= GPIO_SPI_SS_bit;//select pullup for spi_ss pin
  gpio_pad_pu_en_lo |= GPIO_SPI_SS_bit;// enable pull up
  gpio_func_sel_0 |= 0x5500;
}
#endif