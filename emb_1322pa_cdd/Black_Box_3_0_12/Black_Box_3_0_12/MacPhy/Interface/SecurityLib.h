/************************************************************************************
* SecurityLib implements AES and CCM* functionality. The functions are avaliable to
* all higher layers. The AES module is not reentrant.
*
* Author(s): JRJEN1, BPPED1
*
* (c) Copyright 2005, Freescale, Inc.  All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale.
*
* Last Inspected:
* Last Tested:
************************************************************************************/

#ifndef _SECURITY_LIB_H_
#define _SECURITY_LIB_H_

/************************************************************************************
*************************************************************************************
* Public macros
*************************************************************************************
************************************************************************************/

#define gCcmNonceLength_c 13 // Length of nonce used in ccm star.
#define gCcmBlockSize_c 16   // Block size in CCM star (128 bit)

/************************************************************************************
*************************************************************************************
* Public types
*************************************************************************************
************************************************************************************/

enum {
  gCcmEncode_c,
  gCcmDecode_c
};
typedef uintn8_t gCcmDirection_t;


/************************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
************************************************************************************/

/************************************************************************************
* Calculate CCM* as defined for ZigBee
*
* Interface assumptions:
*
* Header, Message and Integrity code have to be located in memory as they appear in
* the packet. That is, as a concatenated string in that order.
*
* For levels with encryption Message is encrypted in-place (overwriting Message).
*
* Depending on the security level the function does:
*
* Level  Action                                   CCM* engine inputs
* 0:	   Nothing.
* 1,2,3: Integrity only based on Header||Message. a is Header||Message, m is empty
* 4:	   Encryption of Message only.              a is empty, m is Message
* 5,6,7: "Normal" CCM on Header and Message.      a is Header, m is Message
*
* Please note that the function is not re-entrant.
*
* Return value:
*   Return status of the operation (always ok = 0 for encoding)
* 
* Revision history:
*   date      Author    Comments
*   ------    ------    --------
*   150305    JRJ/BPP   Created
*   040405    JRJ/BPP   Moved/Renamed/Commented
* 
************************************************************************************/
uint8_t SecLib_CcmStar
  (
  uint8_t * pHeader,           // IN/OUT: start of data to perform CCM-star on
  uint8_t headerLength,        // IN: Length of header field
  uint8_t messageLength,       // IN: Length of data field
  const uint8_t key[16],       // IN: 128 bit key
  const uint8_t nonce[13],     // IN: 802.15.4/Zigbee specific nonce
  const uint8_t securityLevel, // IN: Security level 0-7
  gCcmDirection_t direction    // IN: Direction of CCM: gCcmEncode_c, gCcmDecode_c
  );


/************************************************************************************
* Calculate XOR of individual byte pairs in two uint8_t arrays. I.e.
* pDst[i] := pDst[i] ^ pSrc[i] for i=0 to n-1
*
* Interface assumptions:
*   None
*
* Return value:
*   None
* 
* Revision history:
*   date      Author    Comments
*   ------    ------    --------
*   040405    BPP       Created
* 
************************************************************************************/
void CCM_XorN
  (
  uint8_t *pDst, // First operand and result of XOR operation
  uint8_t *pSrc, // Second operand. Not modified.
  uint8_t n      // Number of bytes in input arrays.
  );


#endif // _SECURITY_LIB_H_
