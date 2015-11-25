/************************************************************************************
*
*(c) Copyright 2010, Freescale, Inc.  All rights reserved.
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale.
*
************************************************************************************/

#ifndef _OTA_NVM_H_
#define _OTA_NVM_H_

#include "nvm.h"
/************************************************************************************
*************************************************************************************
* Public macros
*************************************************************************************
************************************************************************************/
#define OTA_NVM_StartNvmRegulator() CRM_VREG_CNTL.vReg1P8VEn = 1
#define OTA_NVM_StopNvmRegulator()  CRM_VREG_CNTL.vReg1P8VEn = 0
#define OTA_NVM_Read(pSpi , pDest, address, numBytes)   OTA_NVM_ReadOp( mNvmOpRead_c , pSpi , pDest , address , numBytes,(uint16_t*)NULL)
#define OTA_NVM_CrcRead(pSpi , pDest, address, numBytes, pCrc)   OTA_NVM_ReadOp( mNvmOpRead_c , pSpi , pDest , address , numBytes,pCrc)
#define OTA_NVM_BlankCheck(pSpi, address, numBytes)     OTA_NVM_ReadOp( mNvmOpBlankCheck_c , pSpi , 0 , address , numBytes,(uint16_t*)NULL)
#define OTA_NVM_Verify(pSpi , pSrc , address, numBytes) OTA_NVM_ReadOp( mNvmOpVerify_c , pSpi, pSrc , address, numBytes,(uint16_t*)NULL)

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
  mNvmOpRead_c,
  mNvmOpVerify_c,
  mNvmOpBlankCheck_c
} mNvmReadOpType_t;
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
* nvmErr
*   gNvmErrInvalidPointer_c    : if any of the pNvmType or pSpi is NULL
*   gNvmErrNoError_c           : if pNvmType or pSpi hold non-NULL values
* 
* *pNvmType
*   gNvmType_SST_c             : SST   NVM was found
*   gNvmType_ST_c              : ST    NVM was found
*   gNvmType_ATM_c             : ATMEL NVM was found
*   gNvmType_NoNvm_c           : No    NVM was found
************************************************************************************/
extern nvmErr_t OTA_NVM_Detect(SpiRegs_t *pSpi, nvmType_t* pNvmType);

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
* nvmErr_t
*   gNvmErrInvalidInterface_c      : if nvmInterface is not one of the two valid values(gNvmInternalInterface_c or gNvmExternalInterface_c)
*   gNvmErrInvalidNvmType_c        : if nvmType is not one of the three known values(gNvmType_SST_c,gNvmType_ST_c,gNvmType_ATM_c)
*   gNvmErrAddressSpaceOverflow_c  : if (address +numBytes-1 )> gNvmMaxAddress_c
*   gNvmErrInvalidPointer_c        : if  pDest == NULL
*   gNvmErrRestrictedArea_c        : if  nvmInterface = gNvmInternalInterface_c and the address space reachs last sector 
*   gNvmErrNoError_c               : in rest
************************************************************************************/
extern nvmErr_t OTA_NVM_ReadOp(mNvmReadOpType_t mNvmReadOpType ,SpiRegs_t *pSpi, uint8_t* pUint8 , uint32_t address, uint32_t numBytes, uint16_t* pCrc);

/************************************************************************************
* Erase a set of sectors, identified by the sector_bitfield.
*   
* for  nvmType = gNvmType_SST_c NVM 
*         Sector size = 4096. Number of sectors = 32 .
*         Each bit of the sectorBitfield represents a sector.Bit 0 represents sector 0... Bit 31 represents sector 31
* 
* for  nvmType = gNvmType_ST_c or nvmType = gNvmType_ATM_c
*         Sector size = 32768. Number of sectors = 4 
*         Only the least 4  significant bits represents a sector.Bit 0 represents sector 0... Bit 3 represents sector 3
*   
* Return value:
*   nvmErr
*     gNvmErrInvalidInterface_c      : if nvmInterface is not one of the two valid values(gNvmInternalInterface_c or gNvmExternalInterface_c)
*     gNvmErrInvalidNvmType_c        : if nvmType is not one of the three known values(gNvmType_SST_c,gNvmType_ST_c,gNvmType_ATM_c) 
*     gNvmErrRestrictedArea_c        : if the last sector is set to be erased  
*     gNvmErrNoError_c               : in rest
************************************************************************************/
extern void OTA_NVM_EraseSector(SpiRegs_t *pSpi,uint32_t addr);

/************************************************************************************
* Writes numBytes of data from the memory location referred to by pSrc to the address location in flash.
*
* Interface assumptions:
*   The address uses only 24 bits.
*   
* Return value:
*   nvmErr
*     gNvmErrInvalidInterface_c      : if nvmInterface is not one of the two valid values(gNvmInternalInterface_c or gNvmExternalInterface_c)
*     gNvmErrInvalidNvmType_c        : if nvmType is not one of the three known values(gNvmType_SST_c,gNvmType_ST_c,gNvmType_ATM_c)
*     gNvmErrAddressSpaceOverflow_c  : if (address +numBytes-1 )> gNvmMaxAddress_c
*     gNvmErrRestrictedArea_c        : if  nvmInterface = gNvmInternalInterface_c and the address space reachs last sector 
*     gNvmErrBlankCheckError_c       : if at lest one of the location to write is not blank(0xff)
*     gNvmErrWriteProtect_c          : if the NVM cannot be enabled to be write   
*     gNvmErrVerifyError_c           : if after the write operation at least one location in NVM is different from it's intended value
*     gNvmErrNoError_c               : in rest
************************************************************************************/
extern nvmErr_t OTA_NVM_Write(SpiRegs_t *pSpi,void *pSrc, uint32_t address, uint32_t numBytes);

#endif //_OTA_NVM_H_
