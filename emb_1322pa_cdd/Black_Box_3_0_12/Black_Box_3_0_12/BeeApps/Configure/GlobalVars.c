/************************************************************************************
* This module implements the memory allocation, list, and message modules. The memory 
* allocation is build around N (1-4) pools with various memory allocation unit (block)
* sizes. Each pool consists of an anchor with head and tail pointers. The memory blocks
* are all linked to the anchor using a single chained list. Thus each block has a next
* pointer. The user of the functions in this module never has to be concerned with the
* list overhead since this is handled transparently. The block pointer which the user
* receives when allocating memory is pointing to the address after the next-pointer.
*
* FIFO Queues are implemented using the same list functions as used by the memory
* (de)allocation functions. The queue data object is simply an anchor (anchor_t).
* List_AddTail is used for putting allocated blocks on the queue, and List_RemoveHead
* will detach the block from the queue. Before using a queue anchor it must have been
* initialized with List_ClearAnchor. No extra header is required in order to put a
* block in a queue. However, messages should contain type information beside the
* message data so that the message handler at the receiver can reckognize the message.
*
* Messages are sent by allocating a block using MSG_Alloc, and 
* call the Service Access Point (SAP) of the receiver. If the SAP handles specific
* messages synchronously (returns with result immideately) then the block may be
* allocated on the stack of the calling function. The message types which allows this
* are specified in the design documents.
*
* Copyright (c) 2006, Freescale, Inc.  All rights reserved.
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale.
*************************************************************************************/

#include "EmbeddedTypes.h"
#include "MsgSystem.h"

/************************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
************************************************************************************/

/* Implemented in getSP.s*/
uint32_t getSP();

// The heap for MAC, NWK and application memory blocks.
#ifndef __IAR_SYSTEMS_ICC__
#pragma DATA_SEG BUFFER_POOL
uint8_t maMacHeap[mMmTotalPoolSize_c];
#pragma DATA_SEG DEFAULT
#else
uint8_t maMacHeap[mMmTotalPoolSize_c];
#endif

// Memory pool info and anchors.
pools_t maMmPools[gMmNumPools_c];

  // Const array used during initialization. Describes the memory layout.
  // Pools must occur in the table in ascending order according to their size.
  // If coordinator capability is required, then there must always exist an
  // extra MAC pool for receiving data frames. This pool is private to the MAC.
  // An example of a memory layout for coordinators/routers:
  // Pool3]:
  //   SmallMessages[M], // Command messages of each ~22 bytes
  //   BigMessages[N-1], // Data messages of each ~134 bytes
  //   BigMessages[1],   // One data message of ~134 bytes, MAC private.
  // MSG_Alloc ensures that only the MAC has access to the private pool.

  // An example of a memory layout for devices:
  // Pool2]:
  //   SmallMessages[M], // Command messages of each ~22 bytes
  //   BigMessages[N],   // Data messages of each ~134 bytes
  //
#ifdef __IAR_SYSTEMS_ICC__

poolInfo_t poolInfo[gMmNumPools_c] = {
  gMmPoolSize0_c, mMmBlockSize0_c, mMmBlockSize1_c, 0,/* last 0 is padding */
#if gMmNumPools_c > 1
  gMmPoolSize1_c, mMmBlockSize1_c, mMmBlockSize2_c, 0,/* last 0 is padding */
#if gMmNumPools_c > 2
  gMmPoolSize2_c, mMmBlockSize2_c, 0, 0,/* last 0 is padding */
#if gMmNumPools_c > 3
  gMmPoolSize3_c, mMmBlockSize3_c, 0, 0/* last 0 is padding */
#endif // gMmNumPools_c > 3
#endif // gMmNumPools_c > 2
#endif // gMmNumPools_c > 1  
  
#else

const poolInfo_t poolInfo[gMmNumPools_c] = {
  gMmPoolSize0_c, mMmBlockSize0_c, mMmBlockSize1_c,
#if gMmNumPools_c > 1
  gMmPoolSize1_c, mMmBlockSize1_c, mMmBlockSize2_c,
#if gMmNumPools_c > 2
  gMmPoolSize2_c, mMmBlockSize2_c, 0, 
#if gMmNumPools_c > 3
  gMmPoolSize3_c, mMmBlockSize3_c, 0
#endif // gMmNumPools_c > 3
#endif // gMmNumPools_c > 2
#endif // gMmNumPools_c > 1

#endif
};

pools_t *pLastPool = &maMmPools[mMmLastPoolIdx_c];

#if gBigMsgsMacOnly_d
pools_t *pSecondLastPool = &maMmPools[mMmLastPoolIdx_c-1];

#endif /* gBigMsgsMacOnly_d */

  // Application allocated space for MAC PIB ACL Entry descriptors.
#if gNumAclEntryDescriptors_c > 0
  aclEntryDescriptor_t gPIBaclEntryDescriptorSet[gNumAclEntryDescriptors_c];

    // Set number of ACL entries. Used by the MAC.
  uint8_t gNumAclEntryDescriptors = gNumAclEntryDescriptors_c;
#endif // gNumAclEntryDescriptors_c


#if MsgTracking_d
const uint8_t TotalNoOfMsgs_c = (gTotalBigMsgs_d + gTotalSmallMsgs_d);

MsgTracking_t MsgTrackingArray[(gTotalBigMsgs_d + gTotalSmallMsgs_d)] = {0};
uint8_t NoOfWrongAddrs = 0;
extern uint32_t MM_GetLinkRegister(void);
#endif  
  
/************************************************************************************
*************************************************************************************
* Private prototypes
*************************************************************************************
************************************************************************************/


/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/

/************************************************************************************
* Initialize Msg tracking array. It is called by the Msg system when the MAC is reset
*
* Interface assumptions:
*   None
*   
* Return value:
*   None.
* 
************************************************************************************/

void MM_InitMsgTracking(void) {
#if MsgTracking_d
   FLib_MemSet((uint8_t* )&MsgTrackingArray[0],0,(sizeof(MsgTracking_t)/sizeof(uint8_t)) * TotalNoOfMsgs_c);
   NoOfWrongAddrs = 0;
#endif   
}
/************************************************************************************
* Initialize Msg tracking array a current index with a Msg address.
* This functions is called for once for every Msg when the MAC is reset.
*
* Interface assumptions:
*   None
*   
* Return value:
*   None.
* 
************************************************************************************/
#ifdef __IAR_SYSTEMS_ICC__

void   MM_AddMsgToTrackingArray(uint8_t Index, uint32_t Addr) {
#if MsgTracking_d
      MsgTrackingArray[Index].MsgAddr = Addr;  
#else
  /*To prevent compiler warinngs:*/
  (void) Index;
  (void) Addr;  
#endif      
}

#else

void   MM_AddMsgToTrackingArray(uint8_t Index, uint16_t Addr) {
#if MsgTracking_d
      MsgTrackingArray[Index].MsgAddr = Addr;  
#else
  /*To prevent compiler warinngs:*/
  (void) Index;
  (void) Addr;  
#endif      
}

#endif
/************************************************************************************
* This function finds the index of a Msg in the Tracking array
* -1 is returned if Msg not found and the NoOfWrongAddrs is incremented.
*
* Interface assumptions:
*   None
*   
* Return value:
*   Index of message in tracking array
* 
************************************************************************************/  
#ifdef __IAR_SYSTEMS_ICC__

int8_t MM_GetMsgIndex(const uint32_t MsgAddr)
{
#if MsgTracking_d
  uint8_t i;    
  /*Search through the Tracking array*/ 
  for (i = 0; i < TotalNoOfMsgs_c; i++) 
  {      
    /*If Msg Addr found then exit with Index*/
    if (MsgAddr == MsgTrackingArray[i].MsgAddr)
      return i;
  }
  /*Msg addr not found, increment error counter and return*/  
  NoOfWrongAddrs++;
  return -1;
#else  
  /*To prevent compiler warinngs:*/
 (void) MsgAddr;
 return -1;
#endif 
}

#else

int8_t MM_GetMsgIndex(const uint16_t MsgAddr)
{
#if MsgTracking_d
  uint8_t i;    
  /*Search through the Tracking array*/ 
  for (i = 0; i < TotalNoOfMsgs_c; i++) 
  {      
    /*If Msg Addr found then exit with Index*/
    if (MsgAddr == MsgTrackingArray[i].MsgAddr)
      return i;
  }
  /*Msg addr not found, increment error counter and return*/  
  NoOfWrongAddrs++;
  return -1;
#else  
  /*To prevent compiler warinngs:*/
 (void) MsgAddr;
 return -1;
#endif 
}


#endif
/************************************************************************************
* This function is called when ever a Msg is freed or allocated and updates
* the tracking information for that particular Msg
* 
*
* Interface assumptions:
*   None
*   
* Return value:
*   Success status
* 
************************************************************************************/  
#ifdef __IAR_SYSTEMS_ICC__
/*uint32_t  FirstFree;
uint32_t  SecondFree;*/
bool_t MM_UpdateMsgTracking(const void *pBlock, const bool_t Alloc)
{
#if MsgTracking_d
  int8_t index = MM_GetMsgIndex((uint32_t)pBlock);

  /* If the block is found, then process it. */
  if (index >= 0)
  {
    /* If the buffer is already allocated or freed and the same is  */
    if (MsgTrackingArray[index].AllocStatus == Alloc)
    {
      /*while (1)
      {
        FirstFree = MsgTrackingArray[index].FreeAddr;
        SecondFree = MM_GetLinkRegister();
      }*/
      return FALSE;
    }

    /* Update the buffer status. */
    MsgTrackingArray[index].AllocStatus = Alloc;

    /*Update MsgTracking array counters and return address of alloc (TRUE) or free (FALSE) */
    if (Alloc)
    {
      MsgTrackingArray[index].AllocCounter++;
      MsgTrackingArray[index].AllocAddr = MM_GetLinkRegister();
    }
    else
    {
      MsgTrackingArray[index].FreeCounter++;
      MsgTrackingArray[index].FreeAddr = MM_GetLinkRegister();
    }

    /* Success on the porcess. */
    return TRUE;
  }
  else
  {
    /* Invalid buffer quit.! */
    return FALSE;
  }
#else /* else #if MsgTracking_d*/
  /*To prevent compiler warinngs:*/
  (void) pBlock;
  (void) Alloc;
  return TRUE;
#endif  /* end #if MsgTracking_d*/
}

#else /* else #ifdef __IAR_SYSTEMS_ICC__ */
bool_t MM_UpdateMsgTracking(const void *pBlock, const bool_t Alloc, const uint8_t offset)
{
#if MsgTracking_d
  uint16_t StackPointer = 0;
  int8_t index = MM_GetMsgIndex((uint16_t)pBlock);
  
  if (index != -1)
  {          
    MsgTrackingArray[index].AllocStatus = Alloc;
    
    /*Get stack pointer so we can find return addr of orginal free/alloc caller*/
    _asm
    {
      tsx        
        sthx StackPointer
    }
    /*Update MsgTracking array counters and return address of alloc or free */  
    if (Alloc) {
      MsgTrackingArray[index].AllocCounter++;
      MsgTrackingArray[index].AllocAddr = (uint16_t) (*((volatile uint16_t*) (StackPointer - 1 + offset)));
    }
    else {
      MsgTrackingArray[index].FreeCounter++;        
      MsgTrackingArray[index].FreeAddr = (uint16_t) (*((volatile uint16_t*) (StackPointer - 1 + offset)));      
    }
    return TRUE;
    
  } 
  else  
    return FALSE;
#else /* else #if MsgTracking_d*/
  /*To prevent compiler warinngs:*/
  (void) pBlock;
  (void) Alloc;
  (void) offset;
  return FALSE;
#endif /* end #if MsgTracking_d*/ 
}
#endif /*__IAR_SYSTEMS_ICC__*/    

