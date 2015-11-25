/************************************************************************************
* Japan TELEC header file.
*
* (c) Copyright 2007, Freescale, Inc.  All rights reserved.
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
************************************************************************************/

#ifndef _TELEC_H_
#define _TELEC_H_

#define BER_EN (1<<15)    /* Abel_reg30 */
#define PAEN_MASK (1<<15) /* Abel_reg8 */
#define PSM_MASK (1<<3) /* Abel_reg31*/

/************************************************************************************
* Mainloop called from outer main loop.
*   
* Interface assumptions:
*   None
*   
* Return value:
*   None
* 
************************************************************************************/
void ASP_TestMain(void);

/************************************************************************************
* Start tests by calling this function
*   
* Interface assumptions:
*   None
*   
* Return value:
*   None
* 
************************************************************************************/
void ASP_TelecTest(uint8_t mode);


/************************************************************************************
* Set new frequency channel
*   
* Interface assumptions:
*   None
*   
* Return value:
*   None
* 
************************************************************************************/
void ASP_TelecSetFreq(uint8_t channel);

/************************************************************************************
* Send a data packet specifying the PSDU
*   
* Interface assumptions:
*   Synchronous function, will return after the packet is sent.
*   Max len is 125, min is 3 (MACA limitation).
*   
* Return value:
*   None
* 
************************************************************************************/
void ASP_TelecSendRawData(uint8_t length, uint8_t* dataPtr);

#endif //_TELEC_H_
