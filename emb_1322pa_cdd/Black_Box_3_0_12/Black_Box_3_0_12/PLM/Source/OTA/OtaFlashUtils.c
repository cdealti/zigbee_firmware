/************************************************************************************
* OTA support control routines
* (c) Copyright 2010, Freescale, Inc.  All rights reserved.
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale.
*
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Private macros
*************************************************************************************
************************************************************************************/
#include "EmbeddedTypes.h"
#include "Platform.h"
#include "OtaFlashUtils.h"

#define mNvmCmd_SST_ReadId_c                 0x90
#define mNvmCmd_ST_ReadId_c                  0x9F
#define mNvmCmd_ATM_ReadId_c                 0x1d

#define mNvmCmdReadData_c                    0x03
#define mNvmCmdReadStatus_c                  0x05
#define mNvmCmd_SST_EWSR_c                   0x50
#define mNvmCmd_WSR_c                        0x01         //Write Status Register command
#define mNvmCmd_WEN_c                        0x06         //Write ENable command
#define mNvmCmd_SST_SER_c                    0x20         //Sector ERase command (4KB)
#define mNvmCmd_PROG_c                       0x02         //Byte program command

#define mNvmId_SST_c                         0xBF
#define mNvmId_ST_c                          0x20
#define mNvmId_ATM_c                         0x1F

#define mNvmMaxAddress_c                     0x1ffff
#define mInternal_SST_RestrictedArea_c       0x1f000
#define mInternal_SST_RestrictedSector_c     0x80000000

#define mNvmSectorSize_SST_c                 4096
#define mNvmSectorsNumber_SST_c              32
#define mNvmSectorSize_ST_ATM_c              32768
#define mNvmSectorsNumber_ST_ATM_c           4

#define mNvmStatus_Busy_c                    (1<<0)
#define mNvmStatus_WE_c                      (1<<1)
#define mNvmStatus_WP_c                      ((1<<2)|(1<<3))
#define mNvmLastQuarterProtected_c           (1<<2)
#define mNvmLastHalfProtected_c              (1<<3)
#define mNvmAllProtected_c                   ((1<<2)|(1<<3))
#define mNvmLastQuarterFirstAddress_c        0x018000
#define mNvmLastHalfFirstAddress_c           0x010000

#define OtaNvm_ClockNo_DataBitsNo(ClockNo , DataBitsNo)  ( (((ClockNo)-1)<<8) | (1<<7) | (DataBitsNo) )

#define OtaNvmClrStatus(pSpi, bits)    OtaNvmSetClearStatus(pSpi,mNvmClearStatusBits_c, bits)
#define OtaNvmSetStatus(pSpi, bits)    OtaNvmSetClearStatus(pSpi,mNvmSetStatusBits_c, bits)
#define OtaNvmEnableStatusWrite(pSpi)  OtaNvmSendCmd(pSpi,(uint32_t)mNvmCmd_SST_EWSR_c << 24,OtaNvm_ClockNo_DataBitsNo(8,8))

#define OtaNvmUnlockAll(pSpi)          OtaNvmClrStatus(pSpi,1<<7 | 1<<5 | 1<<4 | 1<<3 | 1<<2 )

/************************************************************************************
*************************************************************************************
* Private type definitions
*************************************************************************************
************************************************************************************/
typedef enum
{
  mNvmClearStatusBits_c,
  mNvmSetStatusBits_c
} mNvmStatusBitsOp_t;

/************************************************************************************
*************************************************************************************
* Private prototypes
*************************************************************************************
************************************************************************************/
static uint8_t OtaNvmReadStatus(SpiRegs_t *pSpi);
static void OtaNvmSetClearStatus(SpiRegs_t *pSpi,mNvmStatusBitsOp_t mNvmStatusBitsOp,uint8_t bits);
static void OtaNvmSendCmd(SpiRegs_t *pSpi ,uint32_t txData, uint32_t clkCtrl );

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
void OTA_NVM_EraseSector(SpiRegs_t *pSpi, uint32_t addr)
{
  register  uint32_t cmd;
  //Set SPI_SS_OUT in Auto/Active Low mode
  pSpi->Setup = 1;
  //Unlock all the FLASH memory for writing
  OtaNvmUnlockAll(pSpi);
  //Send the Write ENable command
  OtaNvmSendCmd(pSpi,(uint32_t)mNvmCmd_WEN_c << 24 , OtaNvm_ClockNo_DataBitsNo(8,8));
  //Prepare the sector erase command
  cmd = ((unsigned int)mNvmCmd_SST_SER_c << 24) | (addr & 0x00FFFFFF);
  //Send the sector erase command
  OtaNvmSendCmd(pSpi,cmd,OtaNvm_ClockNo_DataBitsNo(32,32));  
  //Wait untill the sector is erased
  while (OtaNvmReadStatus(pSpi) & 0x01);
  //We are done, lock all the FLASH
  OtaNvmSetStatus(pSpi,1<<3 | 1<<2);
}


/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/

/***********************************************************************************/
static void OtaNvmSendCmd(SpiRegs_t *pSpi ,uint32_t txData, uint32_t clkCtrl )
{
  pSpi->TxData = txData;
  pSpi->ClkCtrl = clkCtrl; // 48 cycle sequence, "GO", 32 bit output (rest, 16 bit, are input)
  while (!(pSpi->Status & 0x1));
}

/***********************************************************************************/
static uint8_t OtaNvmReadStatus(SpiRegs_t *pSpi)
{
  // Read status register (0x05)
  // Write 0x05, then read 8 bit.
  OtaNvmSendCmd(pSpi,(uint32_t)mNvmCmdReadStatus_c << 24,OtaNvm_ClockNo_DataBitsNo(16,8));    
  return (pSpi->RxData & 0xFF);
}

/***********************************************************************************/
static void OtaNvmSetClearStatus(SpiRegs_t *pSpi,mNvmStatusBitsOp_t mNvmStatusBitsOp,uint8_t bits)
{
  uint8_t tmpStatus;
  tmpStatus = OtaNvmReadStatus(pSpi);
  
  OtaNvmEnableStatusWrite(pSpi);
  // Write to status register (0x01), with tmpStatus | bits 
  // Write 0x01, newBits
  if(mNvmStatusBitsOp == mNvmSetStatusBits_c )
  tmpStatus |= bits;
  else
  tmpStatus &= (~bits);  
  OtaNvmSendCmd(pSpi,((uint32_t)mNvmCmd_WSR_c << 24) | ((tmpStatus) << 16) , OtaNvm_ClockNo_DataBitsNo(16,16));
 }

