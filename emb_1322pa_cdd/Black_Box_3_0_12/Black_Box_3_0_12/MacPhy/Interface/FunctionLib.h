/************************************************************************************
* This header file is provided as part of the interface to the Freescale 802.15.4
* MAC and PHY layer.
*
* The file gives access to the generic function library used by the MAC/PHY.
*
* (c) Copyright 2007, Freescale, Inc.  All rights reserved.
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
************************************************************************************/

#ifndef _FUNCTION_LIB_H_
#define _FUNCTION_LIB_H_

#include "EmbeddedTypes.h"

/************************************************************************************
*************************************************************************************
* Public macros
*************************************************************************************
************************************************************************************/
#define FLib_MemCpyDir(b1, b2, dir, n) if(dir) FLib_MemCpy((b1), (b2), (n)); else FLib_MemCpy((b2), (b1), (n))

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
* Copy the content of one memory block to another. The amount of data to copy must
* be specified in number of bytes.
*
* Interface assumptions:
*   None
*   
* Return value:
*   None
* 
************************************************************************************/
void FLib_MemCpy
  (
  void *pDst,     // IN: Pointer to destination memory block
  void *pSrc,     // IN: Pointer to source memory block
  uint16_t cBytes // IN: Number of bytes to copy
  );

/************************************************************************************
* Reset bytes in a memory block to a certain value. The value, and the number of
* bytes o be set, are supplied as arguments.
*
* Interface assumptions:
*   None
*
* Return value:
*   None
* 
************************************************************************************/
 void FLib_MemSet
  (
  void *pData,    // IN: Pointer to memory block to reset
  uint8_t value,  // IN: Value that memory block will be reset to.
  uint16_t cBytes // IN: Number of bytes to reset.
  );

/************************************************************************************
* Copy up to 255 bytes. The byte at index i from the source buffer is copied to index
* ((n-1) - i) in the destination buffer (and vice versa).
*   
* Interface assumptions:
*   None
*   
* Return value:
*   None
* 
************************************************************************************/
void FLib_MemCpyReverseOrder
  (
  void *pDst, // Destination buffer
  void *pSrc, // Source buffer
  uint8_t n   // Byte count
  );

/************************************************************************************
* Compare two memory blocks. The number of bytes to compare must be specified. If the
* blocks are equal byte by byte, the function returns TRUE, and FALSE otherwise.
*
* Interface assumptions:
*   None
*
* Return value:
*   TRUE if memory areas are equal. FALSE othwerwise.
*
************************************************************************************/
bool_t FLib_MemCmp
  (
  void *pData1,   // IN: First memory block to compare
  void *pData2,   // IN: Second memory block to compare
  uint16_t cBytes // IN: Number of bytes to compare.
  );

/************************************************************************************
* Copy up to 256 bytes, possibly into the same overlapping memory as it is taken from
*   
* Interface assumptions:
*   None
*   
* Return value:
*   None
* 
************************************************************************************/
void FLib_MemInPlaceCpy
  (
  void *pDst, // Destination buffer
  void *pSrc, // Source buffer
  uint8_t n   // Byte count
  );

/************************************************************************************
* Add an offset to a pointer. The offset can be in the range 0-255.
* 
* Interface assumptions:
*   None
*   
* Return value:
*    None
* 
************************************************************************************/
#define FLib_AddOffsetToPointer(ptr, offset) *(ptr) = (*(ptr)) + (offset)
#define FLib_AddOffsetToPtr(pPtr,offset) FLib_AddOffsetToPointer((pPtr),(offset))

/************************************************************************************
* Compare two bytes.
*   
* Interface assumptions:
*   None
*   
* Return value:
*   TRUE if content of buffers is equal, and FALSE otherwise.
*
* 
************************************************************************************/
#define FLib_Cmp2Bytes(c1, c2) (*((uint16_t*) c1) == *((uint16_t*) c2))

/************************************************************************************
* Returns the maximum value of arguments a and b.
* 
* Interface assumptions:
*   The primitive should must be implemented as a macro, as it should be possible to
*   evaluate the result on compile time if the arguments are constants.
*   
* Return value:
*   The maximum value of arguments a and b
* 
************************************************************************************/
#define FLib_GetMax(a,b)    (((a) > (b)) ? (a) : (b))

/************************************************************************************
* Returns the minimum value of arguments a and b.
* 
* Interface assumptions:
*   The primitive should must be implemented as a macro, as it should be possible to
*   evaluate the result on compile time if the arguments are constants.
*
************************************************************************************/
#define FLib_GetMin(a,b)    (((a) < (b)) ? (a) : (b))

#endif /* _FUNCTION_LIB_H_ */
