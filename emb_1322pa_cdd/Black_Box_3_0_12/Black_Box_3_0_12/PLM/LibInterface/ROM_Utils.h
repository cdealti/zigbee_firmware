/************************************************************************************
* Public header file for ROM_Utils
*
* Author(s):  R04473 
*
* (c) Copyright 2007, Freescale, Inc.  All rights reserved.
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale.
*
Revision history:
      date                       Author                               Comments
   ----------               ----------------------                    -------
   14.11.2007                    R04473                               Created
************************************************************************************/

#ifndef _ROM_UTILS_H_
#define _ROM_UTILS_H_

/************************************************************************************
*************************************************************************************
* Public macros
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
************************************************************************************/

/*=================================================================================*/
/*===== GetRandomNumber =====*/
/*
@brief  Returns a 32 bit random number. No seeding is needed for the RNG - see the MACA module
        description in the MC1322x documentation.
        Should this be a macro instead of a function?

@param  none

@return uint32_t
        Returns a 32 bit random number.

Revision history:
   Date             Author                        Comments
   ----------       -----------------------       --------
   13.11.2007       PNSN001                       Created
*/
/*=================================================================================*/
uint32_t GetRandomNumber(void);

/**************************************************************************************/
/*=================================================================================*/
/*===== GetRandomRange =====*/
/*
@brief  Receives a pointer to the body of a initialization file.
        Validates the input and processes the file.

@param  uint8_t low, high  - input parameters: ends of the range
        For example, if you want a number between 0-15, pass 0,15

@return uint8_t
        Returns a random 8bit number in the range specified by the caller.

Revision history:
   Date             Author                        Comments
   ----------       -----------------------       --------
   13.11.2007       PNSN001                       Created
   14.11.2007       R04473                        Modified
*/
/*=================================================================================*/
uint8_t GetRandomRange(uint8_t low, uint8_t high);

/**************************************************************************************/

#endif /* _ROM_UTILS_H_ */
