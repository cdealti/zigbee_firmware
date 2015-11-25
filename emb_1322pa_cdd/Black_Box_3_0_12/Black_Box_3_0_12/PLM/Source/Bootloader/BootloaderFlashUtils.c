/************************************************************************************
* Bootloader low level support functions for internal and external flash access
* (c) Copyright 2010, Freescale, Inc.  All rights reserved.
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale.
*
************************************************************************************/

#if gUseBootloader_d
/************************************************************************************
*************************************************************************************
* Private macros
*************************************************************************************
************************************************************************************/
#include "EmbeddedTypes.h"
#include "Platform.h"
#include "BootloaderFlashUtils.h"


#define mBootloaderNvmCmdReadData_c                    0x03
#define mBootloaderNvmCmdReadStatus_c                  0x05
#define mBootloaderNvmCmd_SST_EWSR_c                   0x50
#define mBootloaderNvmCmd_WSR_c                        0x01         //Write Status Register command
#define mBootloaderNvmCmd_WEN_c                        0x06         //Write ENable command
#define mBootloaderNvmCmd_SST_SER_c                    0x20         //Sector ERase command (4KB)
#define mBootloaderNvmCmd_PROG_c                       0x02         //Byte program command


#define mBootloaderNvmMaxAddress_c                     0x1ffff
#define mInternal_SST_RestrictedArea_c       0x1f000
#define mInternal_SST_RestrictedSector_c     0x80000000

#define mBootloaderNvmSectorSize_SST_c                 4096
#define mBootloaderNvmSectorsNumber_SST_c              32
#define mBootloaderNvmSectorSize_ST_ATM_c              32768
#define mBootloaderNvmSectorsNumber_ST_ATM_c           4

#define mBootloaderNvmStatus_Busy_c                    (1<<0)
#define mBootloaderNvmStatus_WE_c                      (1<<1)
#define mBootloaderNvmStatus_WP_c                      ((1<<2)|(1<<3))
#define mBootloaderNvmLastQuarterProtected_c           (1<<2)
#define mBootloaderNvmLastHalfProtected_c              (1<<3)
#define mBootloaderNvmAllProtected_c                   ((1<<2)|(1<<3))
#define mBootloaderNvmLastQuarterFirstAddress_c        0x018000
#define mBootloaderNvmLastHalfFirstAddress_c           0x010000

#define BootloaderNvmClrStatus(pSpi, bits)    BootloaderNvmSetClearStatus(pSpi,mBootloaderNvmClearStatusBits_c, bits)
#define BootloaderNvmSetStatus(pSpi, bits)    BootloaderNvmSetClearStatus(pSpi,mBootloaderNvmSetStatusBits_c, bits)
#define BootloaderNvmEnableStatusWrite(pSpi)  BootloaderNvmSendCmd(pSpi,(uint32_t)mBootloaderNvmCmd_SST_EWSR_c << 24,BootloaderNvm_ClockNo_DataBitsNo(8,8))

#define BootloaderNvmUnlockAll(pSpi)          BootloaderNvmClrStatus(pSpi,1<<7 | 1<<5 | 1<<4 | 1<<3 | 1<<2 )

/************************************************************************************
*************************************************************************************
* Private type definitions
*************************************************************************************
************************************************************************************/
typedef enum
{
  mBootloaderNvmClearStatusBits_c,
  mBootloaderNvmSetStatusBits_c
} mBootloaderNvmStatusBitsOp_t;

/************************************************************************************
*************************************************************************************
* Private prototypes
*************************************************************************************
************************************************************************************/
static uint8_t BootloaderNvmReadStatus(SpiRegs_t *pSpi);
static void BootloaderNvmSetClearStatus(SpiRegs_t *pSpi,mBootloaderNvmStatusBitsOp_t mBootloaderNvmStatusBitsOp,uint8_t bits);
static bootNvmErr_t BootloaderNvmWrite(SpiRegs_t *pSpi, void *pSrc , uint32_t address, uint32_t numBytes);

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

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/

/************************************************************************************/
#pragma location="BOOTLOADER_CODE_1"
void Bootloader_NVM_EraseSector(SpiRegs_t *pSpi, uint32_t addr)
{
  register  uint32_t cmd;
  //Set SPI_SS_OUT in Auto/Active Low mode
  pSpi->Setup = 1;
  //Unlock all the FLASH memory for writing
  BootloaderNvmUnlockAll(pSpi);
  //Send the Write ENable command
  BootloaderNvmSendCmd(pSpi,(uint32_t)mBootloaderNvmCmd_WEN_c << 24 , BootloaderNvm_ClockNo_DataBitsNo(8,8));
  //Prepare the sector erase command
  cmd = ((unsigned int)mBootloaderNvmCmd_SST_SER_c << 24) | (addr & 0x00FFFFFF);
  //Send the sector erase command
  BootloaderNvmSendCmd(pSpi,cmd,BootloaderNvm_ClockNo_DataBitsNo(32,32));  
  //Wait untill the sector is erased
  while (BootloaderNvmReadStatus(pSpi) & 0x01);
  //We are done, lock all the FLASH
  BootloaderNvmSetStatus(pSpi,1<<3 | 1<<2);
}

/***********************************************************************************/
#pragma location="BOOTLOADER_CODE_1"
bootNvmErr_t Bootloader_NVM_Write(SpiRegs_t *pSpi,void *pSrc, uint32_t address, uint32_t numBytes)
{
  bootNvmErr_t nvmErr ;

  //Set SPI_SS_OUT in Auto/Active Low mode    
  pSpi->Setup = 1;
  //Unlock all the FLASH memory for writing
  BootloaderNvmUnlockAll(pSpi);
  //Write the data to the specified address
  nvmErr = BootloaderNvmWrite(pSpi, pSrc ,  address, numBytes);
  //We are done, lock all the FLASH
  BootloaderNvmSetStatus(pSpi,1<<3 | 1<<2);
  if(gBootloaderNvmErrNoError_c != nvmErr) 
    return nvmErr;
  //Werify the written data
  return Bootloader_NVM_ReadOp(mBootloaderNvmOpVerify_c , pSpi , (uint8_t*)pSrc , address, numBytes,(uint16_t*)NULL);
}

/***********************************************************************************/
#pragma location="BOOTLOADER_CODE_2"
bootNvmErr_t Bootloader_NVM_ReadOp(mBootloaderNvmReadOpType_t mBootloaderNvmReadOpType ,SpiRegs_t *pSpi, uint8_t* pUint8 , uint32_t address, uint32_t numBytes , uint16_t* pCrc)
{
  bootNvmErr_t nvmErr ;
  uint8_t rx;
  nvmErr = gBootloaderNvmErrNoError_c;
  
  // Setup SPI. Use SPI_CLK_POL = 0, RESYNC = 0, SPI0_SETUP = 2 (CE low). Other bits 0.
  if(pCrc)
    *pCrc = 0;
  pSpi->Setup = 2;
  // Write "read" cmd + address first
  // Write 0x03 0xaddress, 0xaddress, 0xaddress
  BootloaderNvmSendCmd(pSpi,((uint32_t)mBootloaderNvmCmdReadData_c << 24) | (address & 0x00FFFFFF),BootloaderNvm_ClockNo_DataBitsNo(32,32));
  //Start execution the requested operation, byte by byte
  while (numBytes--) 
  {
    BootloaderNvmSendCmd(pSpi,((uint32_t)0x0),BootloaderNvm_ClockNo_DataBitsNo(8,0));  
    rx = pSpi->RxData;
    //Update the CRC
    if(pCrc)
      *pCrc +=rx;
    //Plain read
    if(mBootloaderNvmOpRead_c == mBootloaderNvmReadOpType)       
    {
      *pUint8++ = rx;
    }
    else if(mBootloaderNvmOpVerify_c == mBootloaderNvmReadOpType) //Verify the data from the FLASH
    {
      if(*pUint8++ != rx)      
        {
          nvmErr = gBootloaderNvmErrVerifyError_c;
          break;
        }
    }
    else  //execute blank check
    {
      if( 0xff != rx)
        {
          nvmErr = gBootloaderNvmErrBlankCheckError_c;
          break;
        }  
    } 
  }
  //Read operation finished, set SPI_SS_OUT high
  pSpi->Setup = 3;
  //Restore SPI to default. Use SPI_CLK_POL = 0, RESYNC = 0, SPI0_SETUP = 1. Other bits 0.
  pSpi->Setup = 1;
  return nvmErr;
}

/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/

/***********************************************************************************/
#pragma location="BOOTLOADER_CODE_2"
void BootloaderNvmSendCmd(SpiRegs_t *pSpi ,uint32_t txData, uint32_t clkCtrl )
{
  pSpi->TxData = txData;
  pSpi->ClkCtrl = clkCtrl; // 48 cycle sequence, "GO", 32 bit output (rest, 16 bit, are input)
  while (!(pSpi->Status & 0x1));
}

/***********************************************************************************/
#pragma location="BOOTLOADER_CODE_1"
static uint8_t BootloaderNvmReadStatus(SpiRegs_t *pSpi)
{
  // Read status register (0x05)
  // Write 0x05, then read 8 bit.
  BootloaderNvmSendCmd(pSpi,(uint32_t)mBootloaderNvmCmdReadStatus_c << 24,BootloaderNvm_ClockNo_DataBitsNo(16,8));    
  return (pSpi->RxData & 0xFF);
}

/***********************************************************************************/
#pragma location="BOOTLOADER_CODE_1"
static void BootloaderNvmSetClearStatus(SpiRegs_t *pSpi,mBootloaderNvmStatusBitsOp_t mBootloaderNvmStatusBitsOp,uint8_t bits)
{
  uint8_t tmpStatus;
  tmpStatus = BootloaderNvmReadStatus(pSpi);
  
  BootloaderNvmEnableStatusWrite(pSpi);
  // Write to status register (0x01), with tmpStatus | bits 
  // Write 0x01, newBits
  if(mBootloaderNvmStatusBitsOp == mBootloaderNvmSetStatusBits_c )
  tmpStatus |= bits;
  else
  tmpStatus &= (~bits);  
  BootloaderNvmSendCmd(pSpi,((uint32_t)mBootloaderNvmCmd_WSR_c << 24) | ((tmpStatus) << 16) , BootloaderNvm_ClockNo_DataBitsNo(16,16));
 }

/***********************************************************************************/
#pragma location="BOOTLOADER_CODE_1"
static bootNvmErr_t BootloaderNvmWrite(SpiRegs_t *pSpi, void *pSrc , uint32_t address, uint32_t numBytes)
{
  uint8_t fStatus;
  uint8_t* pSrc_b;
  pSrc_b =  (uint8_t*)pSrc;
  while(numBytes--)
  {
    while ((fStatus=BootloaderNvmReadStatus(pSpi)) & mBootloaderNvmStatus_Busy_c); // Wait while busy is set!
    switch(fStatus & mBootloaderNvmStatus_WP_c)
    {
    case mBootloaderNvmAllProtected_c:
      return gBootloaderNvmErrWriteProtect_c;
    case mBootloaderNvmLastHalfProtected_c:
      if(address >= mBootloaderNvmLastHalfFirstAddress_c)
        return gBootloaderNvmErrWriteProtect_c;
      break;
    case mBootloaderNvmLastQuarterProtected_c:
      if(address >= mBootloaderNvmLastQuarterFirstAddress_c)
        return gBootloaderNvmErrWriteProtect_c;
      break;
    }
    if((fStatus&0x2)==0)
    {
      // Nvm Write Enable
      BootloaderNvmSendCmd(pSpi,(uint32_t)mBootloaderNvmCmd_WEN_c << 24 , BootloaderNvm_ClockNo_DataBitsNo(8,8));
    }
    pSpi->Setup =  2;
    // Write "WRITE" cmd + address first
    // Write 0x02 0xaddress, 0xaddress, 0xaddress
    BootloaderNvmSendCmd(pSpi,((uint32_t)mBootloaderNvmCmd_PROG_c << 24) | (address++ & 0x00FFFFFF),BootloaderNvm_ClockNo_DataBitsNo(32,32));
    BootloaderNvmSendCmd(pSpi,(uint32_t)*pSrc_b++ << 24,BootloaderNvm_ClockNo_DataBitsNo(8,8));
    pSpi->Setup =  3;
    pSpi->Setup =  1;
  }  
  while (BootloaderNvmReadStatus(pSpi) & 0x01); // Wait while busy is set!	
  return gBootloaderNvmErrNoError_c;
}
#endif
