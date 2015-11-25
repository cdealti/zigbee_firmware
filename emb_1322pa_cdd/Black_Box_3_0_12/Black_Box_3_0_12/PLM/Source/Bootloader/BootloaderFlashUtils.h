/************************************************************************************
*
*(c) Copyright 2010, Freescale, Inc.  All rights reserved.
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale.
*
************************************************************************************/

#ifndef _BOOTLOADER_NVM_H_
#define _BOOTLOADER_NVM_H_

/************************************************************************************
*************************************************************************************
* Public macros
*************************************************************************************
************************************************************************************/
#define mBootloaderNvmCmd_SST_ReadId_c                 0x90
#define mBootloaderNvmCmd_ST_ReadId_c                  0x9F
#define mBootloaderNvmCmd_ATM_ReadId_c                 0x1d

#define mBootloaderNvmId_SST_c                         0xBF
#define mBootloaderNvmId_ST_c                          0x20
#define mBootloaderNvmId_ATM_c                         0x1F

#define Bootloader_NVM_StartNvmRegulator() CRM_VREG_CNTL.vReg1P8VEn = 1
#define Bootloader_NVM_StopNvmRegulator()  CRM_VREG_CNTL.vReg1P8VEn = 0
#define Bootloader_NVM_Read(pSpi , pDest, address, numBytes)   Bootloader_NVM_ReadOp( mBootloaderNvmOpRead_c , pSpi , pDest , address , numBytes,(uint16_t*)NULL)
#define Bootloader_NVM_CrcRead(pSpi , pDest, address, numBytes, pCrc)   Bootloader_NVM_ReadOp( mBootloaderNvmOpRead_c , pSpi , pDest , address , numBytes,pCrc)
#define Bootloader_NVM_BlankCheck(pSpi, address, numBytes)     Bootloader_NVM_ReadOp( mBootloaderNvmOpBlankCheck_c , pSpi , 0 , address , numBytes,(uint16_t*)NULL)
#define Bootloader_NVM_Verify(pSpi , pSrc , address, numBytes) Bootloader_NVM_ReadOp( mBootloaderNvmOpVerify_c , pSpi, pSrc , address, numBytes,(uint16_t*)NULL)
#define BootloaderNvm_ClockNo_DataBitsNo(ClockNo , DataBitsNo)  ( (((ClockNo)-1)<<8) | (1<<7) | (DataBitsNo) )
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
  mBootloaderNvmOpRead_c,
  mBootloaderNvmOpVerify_c,
  mBootloaderNvmOpBlankCheck_c
} mBootloaderNvmReadOpType_t;

typedef enum
{
  gBootloaderNvmType_NoNvm_c,
  gBootloaderNvmType_SST_c,
  gBootloaderNvmType_ST_c,
  gBootloaderNvmType_ATM_c,
  gBootloaderNvmType_Max_c
} bootloaderNvmType_t;

typedef enum
{
  gBootloaderNvmErrNoError_c = 0,
  gBootloaderNvmErrInvalidInterface_c,
  gBootloaderNvmErrInvalidNvmType_c,
  gBootloaderNvmErrInvalidPointer_c,
  gBootloaderNvmErrWriteProtect_c,
  gBootloaderNvmErrVerifyError_c,
  gBootloaderNvmErrAddressSpaceOverflow_c,
  gBootloaderNvmErrBlankCheckError_c,
  gBootloaderNvmErrRestrictedArea_c,
  gBootloaderNvmErrNvmRegOff_c,
  gBootloaderNvmErrMaxError_c
} bootNvmErr_t;
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
* This function executes a search for the known NVM types
*   
* Interface assumptions:
*   None
*   
* Return value:
* bootloaderNvmErr
*   gBootloaderNvmErrInvalidPointer_c    : if any of the pNvmType or pSpi is NULL
*   gBootloaderNvmErrNoError_c           : if pNvmType or pSpi hold non-NULL values
* 
* *pNvmType
*   gBootloaderNvmType_SST_c             : SST   NVM was found
*   gBootloaderNvmType_ST_c              : ST    NVM was found
*   gBootloaderNvmType_ATM_c             : ATMEL NVM was found
*   gBootloaderNvmType_NoNvm_c           : No    NVM was found
************************************************************************************/
extern bootNvmErr_t Bootloader_NVM_Detect(SpiRegs_t *pSpi, bootloaderNvmType_t* pNvmType);

/************************************************************************************
* Reads numBytes of data from the flash. The address is used initially, but any
* subsequent reads are done in "continous-read" mode, without setting the address
* again. The read data is copied into the memory location referred to by pDest.
*   
* Interface assumptions:
*   The address uses only 24 bits.
*   
* Return value:
*
* bootNvmErr_t
*   gBootloaderNvmErrInvalidInterface_c      : if nvmInterface is not one of the two valid values(gBootloaderNvmInternalInterface_c or gBootloaderNvmExternalInterface_c)
*   gBootloaderNvmErrInvalidNvmType_c        : if nvmType is not one of the three known values(gBootloaderNvmType_SST_c,gBootloaderNvmType_ST_c,gBootloaderNvmType_ATM_c)
*   gBootloaderNvmErrAddressSpaceOverflow_c  : if (address +numBytes-1 )> gBootloaderNvmMaxAddress_c
*   gBootloaderNvmErrInvalidPointer_c        : if  pDest == NULL
*   gBootloaderNvmErrRestrictedArea_c        : if  nvmInterface = gBootloaderNvmInternalInterface_c and the address space reachs last sector 
*   gBootloaderNvmErrNoError_c               : in rest
************************************************************************************/
extern bootNvmErr_t Bootloader_NVM_ReadOp(mBootloaderNvmReadOpType_t mBootloaderNvmReadOpType ,SpiRegs_t *pSpi, uint8_t* pUint8 , uint32_t address, uint32_t numBytes, uint16_t* pCrc);

/************************************************************************************
* Erase a set of sectors, identified by the sector_bitfield.
*   
* for  nvmType = gBootloaderNvmType_SST_c NVM 
*         Sector size = 4096. Number of sectors = 32 .
*         Each bit of the sectorBitfield represents a sector.Bit 0 represents sector 0... Bit 31 represents sector 31
* 
* for  nvmType = gBootloaderNvmType_ST_c or nvmType = gBootloaderNvmType_ATM_c
*         Sector size = 32768. Number of sectors = 4 
*         Only the least 4  significant bits represents a sector.Bit 0 represents sector 0... Bit 3 represents sector 3
*   
* Return value:
*   nvmErr
*     gBootloaderNvmErrInvalidInterface_c      : if nvmInterface is not one of the two valid values(gBootloaderNvmInternalInterface_c or gBootloaderNvmExternalInterface_c)
*     gBootloaderNvmErrInvalidNvmType_c        : if nvmType is not one of the three known values(gBootloaderNvmType_SST_c,gBootloaderNvmType_ST_c,gBootloaderNvmType_ATM_c) 
*     gBootloaderNvmErrRestrictedArea_c        : if the last sector is set to be erased  
*     gBootloaderNvmErrNoError_c               : in rest
************************************************************************************/
extern void Bootloader_NVM_EraseSector(SpiRegs_t *pSpi,uint32_t addr);

/************************************************************************************
* Writes numBytes of data from the memory location referred to by pSrc to the address location in flash.
*
* Interface assumptions:
*   The address uses only 24 bits.
*   
* Return value:
*   nvmErr
*     gBootloaderNvmErrInvalidInterface_c      : if nvmInterface is not one of the two valid values(gBootloaderNvmInternalInterface_c or gBootloaderNvmExternalInterface_c)
*     gBootloaderNvmErrInvalidNvmType_c        : if nvmType is not one of the three known values(gBootloaderNvmType_SST_c,gBootloaderNvmType_ST_c,gBootloaderNvmType_ATM_c)
*     gBootloaderNvmErrAddressSpaceOverflow_c  : if (address +numBytes-1 )> gBootloaderNvmMaxAddress_c
*     gBootloaderNvmErrRestrictedArea_c        : if  nvmInterface = gBootloaderNvmInternalInterface_c and the address space reachs last sector 
*     gBootloaderNvmErrBlankCheckError_c       : if at lest one of the location to write is not blank(0xff)
*     gBootloaderNvmErrWriteProtect_c          : if the NVM cannot be enabled to be write   
*     gBootloaderNvmErrVerifyError_c           : if after the write operation at least one location in NVM is different from it's intended value
*     gBootloaderNvmErrNoError_c               : in rest
************************************************************************************/
extern bootNvmErr_t Bootloader_NVM_Write(SpiRegs_t *pSpi,void *pSrc, uint32_t address, uint32_t numBytes);

void BootloaderNvmSendCmd(SpiRegs_t *pSpi ,uint32_t txData, uint32_t clkCtrl );

#endif //_BOOTLOADER_NVM_H_
