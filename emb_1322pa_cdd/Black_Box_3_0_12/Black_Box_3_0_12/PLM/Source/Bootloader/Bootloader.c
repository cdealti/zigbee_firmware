/************************************************************************************
* External storage Bootloader for OTA support routines using external SST Flash
* (c) Copyright 2011, Freescale Semiconductor, Inc.  All rights reserved.
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale.
*
************************************************************************************/

#if gUseBootloader_d
#include "EmbeddedTypes.h"
#include "Platform.h"
#include "BootloaderFlashUtils.h"
#include "Bootloader.h"
#include "crm.h"

/************************************************************************************
*************************************************************************************
* Private macros
*************************************************************************************
************************************************************************************/

#define GPIO_BAR                      0x80000000
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


#define SECTOR_SIZE             0x1000
#define FLASH_SIZE              0x20000
#define RAM_SIZE                0x18000

// ROM patch vectors addresses
#define PATCH_VECTOR_START_REDIRECT_REGION  0x00400020
#define PATCH_VECTOR_END_REDIRECT_REGION    0x00400120
#define PATCH_VECTOR_START_REGION           0x00401020
#define PATCH_VECTOR_REGION_SIZE  (PATCH_VECTOR_END_REDIRECT_REGION - PATCH_VECTOR_START_REDIRECT_REGION)

#define INTERNAL_FLASH          FLASH_REGS_P
#define EXTERNAL_FLASH          SPI_REGS_P
#define MAX_READ_WRITE_RETRIES  3

#define USED_FLASH              SPI_REGS_P
#define FLASH_ERROR             gBootloaderExternalFlashError_c
#define SECTORS_TO_ERASE        32
#define SECTOR_ERASE_MASK_SIZE  4
// address of OTA image length is set in flash at 
// the beginning of the second sector (RAM addr 0x0FF8)
#define APP_IMAGE_LEN_ADDRESS_FLASH   0x1000

// Size of the buffer to be allocated on the stack for FLASH reads. The function calls
// where this size is allocated should be done at the beginning of the app execution
// when the stack is not utilized.
#define FLASH_READS_ON_STACK  128 

#define IMAGE_HEADER    8                   //Image signature and length
#define IMAGE_FOOTER    6                   //CRC and internal FLASH sector erase pattern
#define MAX_IMAGE_LEN   (128*1024 - 4*1024 - IMAGE_HEADER - IMAGE_FOOTER) //Max space for the new image


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
static void           BootloaderCrcCompute(uint8_t *pData, uint16_t lenData);
static bootResult_t   BootloaderUpdateImage(uint32_t imageLength, uint32_t sectorEraseBitmap);

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
static uint16_t gCrcValue = 0;                //Current CRC value

// This placeholder const is placed at the end of RAM address space by the linker configuration
// file in order to ensure that bootloader enabled code images always have the maximum allowable
// length of 96K and thus that the ROM bootstrap will always mirror this full length from flash
// to RAM even after updating with a different image file via OTA. The image length is written 
// at the beginning of the flash at address 4 after the signature at address 0.
#pragma location = "IMAGE_END_section"
const uint32_t       bootloaderImageEndVar;  

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/
#pragma location="BOOTLOADER_CODE_2"
bootResult_t Bootloader_Check()
{
  bootResult_t result;
  uint32_t imageLength, bitmapSectorErase;
  volatile uint16_t wait = 0xFF;
  bootNvmErr_t nvmResult;
  uint8_t signature[4];
  uint32_t length;
  uint32_t currentAddress;
  uint16_t temp16;
  uint8_t dataBuffer[FLASH_READS_ON_STACK];//data buffer for FLASH reads to compute the image CRC
    
  // Copy the ROM patch vectors from sector 2 to sector 1 mirrored in RAM
  uint32_t* pSrc, *pDst;
  uint32_t len = PATCH_VECTOR_REGION_SIZE;
  pSrc = (uint32_t*)PATCH_VECTOR_START_REGION;
  pDst = (uint32_t*)PATCH_VECTOR_START_REDIRECT_REGION;
  while (len != 0)
  {
    *pDst = *pSrc;
    pDst++;
    pSrc++;
    len-=sizeof(uint32_t);
  }
  
  //Ensure that the internal NVM regulator is on as the bootstrap leaves the NVM regulator off
  Bootloader_NVM_StartNvmRegulator();
  
  //Wait a little
  while(wait--);
  
  // Enable External SPI
  gpio_func_sel_0 &= (~0xFF00);                 // configure as gpio
  gpio_data_dir_reset_lo  = GPIO_SPI_SS_bit;    //configure spi_ss pin as an input
  gpio_data_sel_lo &=(~GPIO_SPI_SS_bit);        //data is read from the PAD
  gpio_pullup_sel_lo |= GPIO_SPI_SS_bit;        //select pullup for spi_ss pin
  gpio_pad_pu_en_lo |= GPIO_SPI_SS_bit;         // enable pull up
  gpio_func_sel_0 |= 0x5500;
  
  //Detect the external FLASH type
  EXTERNAL_FLASH->Setup = 3;                 // Ensure no glitch when setting "auto", below

  // Setup SPI. Use SPI_CLK_POL = 0, RESYNC = 0, SPI0_SETUP = 1. Other bits 0.
  EXTERNAL_FLASH->Setup = 1;
  
  // Only interested in sample SST External SPI Flash. 
  // Please modify for other flash type 
  BootloaderNvmSendCmd(EXTERNAL_FLASH,(uint32_t)mBootloaderNvmCmd_SST_ReadId_c << 24,
                            BootloaderNvm_ClockNo_DataBitsNo(40,32));
  
  // SST FlashSPI_SPIRX [8.0] has Manufacturer ID 0xBF
  if ((EXTERNAL_FLASH->RxData & 0xFF) != mBootloaderNvmId_SST_c) 
      return gBootloaderExternalFlashError_c;
 
  /* // Example for detecting SPI ST and Atmel Flash
  BootloaderNvmSendCmd(EXTERNAL_FLASH,(uint32_t)mBootloaderNvmCmd_ST_ReadId_c << 24,
                            BootloaderNvm_ClockNo_DataBitsNo(16,8));
  if ((pSpi->RxData  & 0xFF) != mBootloaderNvmId_ST_c) 
      return gBootloaderExternalFlashError_c;

  BootloaderNvmSendCmd(EXTERNAL_FLASH,(uint32_t)mBootloaderNvmCmd_ATM_ReadId_c << 24,
                            BootloaderNvm_ClockNo_DataBitsNo(16,8));
  if ((pSpi->RxData  & 0xFF) != mBootloaderNvmId_ATM_c) 
        return gBootloaderExternalFlashError_c;
  */
  
  //Validate the image from the external flash
  //Read the signature at the beginning of the external flash ('OKOK' or 'SECU')
  nvmResult = Bootloader_NVM_Read(EXTERNAL_FLASH, (uint8_t*)signature, 0x00, 0x04);
  if (nvmResult != gBootloaderNvmErrNoError_c) return gBootloaderExternalFlashError_c;
  
  //Compare with known signatures
  if((signature[0] !=  'O') || (signature[1] !=  'K') || (signature[2] !=  'O') || (signature[3] !=  'K'))
  {
    if((signature[0] !=  'S') || (signature[1] !=  'E') || (signature[2] !=  'C') || (signature[3] !=  'U'))
    {
      return gBootloaderNoImage_c;
    }
  }

  // Get the image advertised length
  nvmResult = Bootloader_NVM_Read(EXTERNAL_FLASH, (uint8_t*)&imageLength, 
                            APP_IMAGE_LEN_ADDRESS_FLASH, 0x04);
  if(nvmResult != gBootloaderNvmErrNoError_c) return gBootloaderExternalFlashError_c;

  // Compute and compare the image CRC
  gCrcValue = 0;
  currentAddress = IMAGE_HEADER;  // flash address 8 after the signature and length
  length = imageLength + SECTOR_ERASE_MASK_SIZE;  // add the sector erase bitmap
  temp16 = FLASH_READS_ON_STACK;
  while(length)
  {
    if(length < FLASH_READS_ON_STACK)
    {
      temp16 = length;
    }
    nvmResult = Bootloader_NVM_Read(EXTERNAL_FLASH , dataBuffer, currentAddress, temp16);
    if(nvmResult != gBootloaderNvmErrNoError_c) return gBootloaderExternalFlashError_c;
    
    //Update CRC
    BootloaderCrcCompute(dataBuffer, temp16);
    
    //Update remaining length and next read address
    length -= temp16;
    currentAddress += temp16;
  }
  
  //Read the stored CRC
  nvmResult = Bootloader_NVM_Read(EXTERNAL_FLASH , (uint8_t*)&temp16, currentAddress, 2);
  if(nvmResult != gBootloaderNvmErrNoError_c) return gBootloaderExternalFlashError_c;

  //Compare the read CRC with the computed one
  if(temp16 != gCrcValue) return gBootloaderCrcError_c;
  
  //All OK, send back the total image length and the sector erase bitmap
  imageLength += IMAGE_HEADER;
  
  // read the sector erase bitmap
  nvmResult = Bootloader_NVM_Read(EXTERNAL_FLASH , (uint8_t*)&bitmapSectorErase, imageLength, 4);
  if(nvmResult != gBootloaderNvmErrNoError_c) return gBootloaderExternalFlashError_c;  
  
  // Valid image in the external FLASH, update the internal image
  // Bootloader will never erase itself in sector 1 and sector 32
  // which contains hardware parameters
  result = BootloaderUpdateImage(imageLength, bitmapSectorErase & 0x7FFFFFFE);
  
  if (result == gBootloaderSucess_c)
    //Invalidate the image by erasing the first sector
    Bootloader_NVM_EraseSector(EXTERNAL_FLASH, 0x00);
  
  // Reset in any case to attempt to boot from new image or retry
  CRM_SoftReset();
  
  return gBootloaderSucess_c;
}

/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/


/************************************************************************************
*  Updates the internal FLASH image with the new image contained in the external one.
*
*  Input parameters:
*  - imageLenght: the number of bytes to copy from the external FLASH to the internal one
*  - sectorEraseBitmap: the sectors to erase in the internal FLASH before copying
*       the image. The MSB represents sector 0. The LSB, that should represent sector 
*       31, is ignored, as the last sector of the internal FLASH holds IC production data.
*
*  Return:
*  - bootResult_t: the result of the update process
************************************************************************************/
#pragma location="BOOTLOADER_CODE_1"
static bootResult_t BootloaderUpdateImage(uint32_t imageLength, uint32_t sectorEraseBitmap)
{
  bootNvmErr_t nvmResult;
  uint32_t bit;
  uint8_t idx;
  uint8_t retries;
  uint32_t length;
  uint32_t currentAddress;
  uint32_t nextAddress;
  uint16_t temp16;
  uint8_t dataBuffer[FLASH_READS_ON_STACK];//data buffer for FLASH reads to compute the image CRC
  
  //Erase and re-write the sectors indicated by the sectorEraseBitmap
  bit = 0x80000000;
  temp16 = FLASH_READS_ON_STACK;
  
  for(idx = 0; idx < 32; idx++)
  {
    // check if sector should be erased
    if(bit & sectorEraseBitmap)
    {
      // compute sector address limits
      currentAddress = idx << 12;
      nextAddress = currentAddress + 0x1000;
      
      // erase the sector
      Bootloader_NVM_EraseSector(INTERNAL_FLASH, currentAddress);
      
      // compute length from image to copy for the current sector
      length = 0x1000;
      
      if (nextAddress > imageLength)
        length = imageLength - currentAddress;
      
      if (currentAddress > imageLength)
        length = 0;
          
       //Perform the copy from the external FLASH to the INTERNAL FLASH
      while(length)
      {
        // temp16 stores how many bytes to copy for an operation
        if(length < FLASH_READS_ON_STACK)
        {
          temp16 = length;
        }
        retries = MAX_READ_WRITE_RETRIES;
        do
        {
          // read temp16 from current address from external flash into dataBuffer
          nvmResult = Bootloader_NVM_Read(EXTERNAL_FLASH, dataBuffer, currentAddress, temp16);
          
          // if reading was OK, write to internal flash
          if (nvmResult == gBootloaderNvmErrNoError_c)
            nvmResult = Bootloader_NVM_Write(INTERNAL_FLASH, dataBuffer, currentAddress, temp16);
          
          // if there was an error reading or writing and retry count is 0 return an error
          if (nvmResult != gBootloaderNvmErrNoError_c && retries == 0)
              return gBootloaderInternalFlashError_c;
          // else decrement retry count          
          else
              retries--;
        } while (nvmResult != gBootloaderNvmErrNoError_c); 
    
        //Update remaining length and next read/write address
        length -= temp16;
        currentAddress += temp16;
      }
    }
    // go to next sector
    bit = bit >> 1;
  }
  // should Read and write the signature but that is in sector 1
  // so it won't be updated in this version of the bootloader
  /*
  nvmResult = Bootloader_NVM_Read(EXTERNAL_FLASH , dataBuffer, 0, 4);
  if(nvmResult != gBootloaderNvmErrNoError_c) return gBootloaderExternalFlashError_c;
  nvmResult = Bootloader_NVM_Write(INTERNAL_FLASH , dataBuffer, 0, 4);
  if(nvmResult != gBootloaderNvmErrNoError_c) return gBootloaderInternalFlashError_c;
  */
  return gBootloaderSucess_c;
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
#pragma location="BOOTLOADER_CODE_2"
static void BootloaderCrcCompute(uint8_t *pData, uint16_t lenData)
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

#endif
