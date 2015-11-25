/************************************************************************************
* This header file is provided as part of the interface to the freeScale 802.15.4
* MAC and PHY layer.
*
* This is the header file for the memory and message module.
*
* (c) Copyright 2007, Freescale, Inc.  All rights reserved.
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
************************************************************************************/

#ifndef _PHY_MAC_MSG_H_
#define _PHY_MAC_MSG_H_

//#include "PlatformToMacPhyDbgConfig.h" // For the MM_DEBUG define.
#include "FunctionalityDefines.h" // For gCoorCapability_d
#include "MacPhy.h" // Necessary for the rxPacket_t and txPacket_t

  // Include the MsgSystem file. If debug use the local dbg version
#ifndef MM_DEBUG
  #include "MsgSystem.h"
#else
  #include "MsgSystemDbg.h"
#endif

/************************************************************************************
*************************************************************************************
* Public macros
*************************************************************************************
************************************************************************************/


/************************************************************************************
*************************************************************************************
* Public type definitions
*************************************************************************************
************************************************************************************/

typedef enum {
  gMmEnablePrivatePool_c,
  gMmDisablePrivatePool_c
} privatePoolState_t;


#ifdef MM_DEBUG
void *MM_AllocDebug(uint8_t size, uint8_t *fileName, uint16_t lineNumber);
void *MM_AllocPoolDebug(pools_t *pPools, uint8_t size, uint8_t *fileName, uint16_t lineNumber);
void *MM_AllocFastDebug(uint8_t *fileName, uint16_t lineNumber);

void    MEM_MLME_SapHandlerDebug(void *pMsg, uint8_t *fileName, uint16_t lineNumber);
uint8_t MLME_NWK_SapHandlerDebug(void *pMsg, uint8_t *fileName, uint16_t lineNumber);
void    MLME_MEM_SapHandlerDebug(void *pMsg, uint8_t *fileName, uint16_t lineNumber);
uint8_t MCPS_NWK_SapHandlerDebug(void *pMsg, uint8_t *fileName, uint16_t lineNumber);
uint8_t NWK_MLME_SapHandlerDebug(void *pMsg, uint8_t *fileName, uint16_t lineNumber);
uint8_t NWK_MCPS_SapHandlerDebug(void *pMsg, uint8_t *fileName, uint16_t lineNumber);
uint8_t MEM_ASP_SapHandlerDebug(void *pMsg, uint8_t *fileName, uint16_t lineNumber);
uint8_t ASP_APP_SapHandlerDebug(void *pMsg, uint8_t *fileName, uint16_t lineNumber);
uint8_t APP_ASP_SapHandlerDebug(void *pMsg, uint8_t *fileName, uint16_t lineNumber);

#define MM_AllocFast() MM_AllocFastDebug(__FILE__, (uint16_t)(__LINE__))

#else /* MM_DEBUG */

void *MM_AllocFast(void);

#endif /* MM_DEBUG */


#if gCoorCapability_d
void MM_SetPrivatePool(privatePoolState_t state); // Parameters: gMmDisablePrivatePool_c, or gMmEnablePrivatePool_c
#else
#define MM_SetPrivatePool(state)
#endif // gCoorCapability_d

/************************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
************************************************************************************/

/************************************************************************************
* Removes a list element from anywhere in the list.
*
* This function should be used somewhat like in the following example since it
* requires a pointer to the previous block relative to the current block:
*
*  void *pBlock = List_GetFirst(pMyList);
*  void *pPrev = NULL;
*  anchor_t *pMyList;
*
*  while(pBlock) {
*    if(CheckStuff(pBlock) == RIGHT_STUFF) {
*      List_Remove(pMyList, pPrev, pBlock);
*      break;
*    }
*    pPrev = pBlock;
*    pBlock = List_GetNext(pBlock);
*  }
*
* Interface assumptions:
*   None
*   
* Return value:
*   None
* 
************************************************************************************/
void List_Remove
  (
  anchor_t *pAnchor, //IN: List to remove block from
  void *pPrevBlock,  //IN: Previous List element used to fix the list after modification.
  void *pBlock       //IN: List element used to reference the next element in the list.
  );

/************************************************************************************
* Returns a pointer to the first element in the list.
*
* Interface assumptions:
*   None
*   
* Return value:
*   Pointer to the next list element or NULL if no elements available.
* 
************************************************************************************/
void *List_GetFirst
  (
  anchor_t *pAnchor  //IN: The list where the first element will be returned from.
  );

/************************************************************************************
* Returns a pointer to the next list element in the list.
*
* Interface assumptions:
*   None
*   
* Return value:
*   Pointer to the next list element or NULL if no elements available.
* 
************************************************************************************/
void *List_GetNext
  (
  void *pBlock  //IN: List element used to reference the next element in the list.
  );


#endif /* _PHY_MAC_MSG_H_ */
