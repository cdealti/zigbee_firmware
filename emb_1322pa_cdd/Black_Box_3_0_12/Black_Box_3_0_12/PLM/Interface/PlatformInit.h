/************************************************************************************
* Init platform
*
*
* (c) Copyright 2006, Freescale Semiconductor, Inc. All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
************************************************************************************/

#ifndef _PLATFORMINIT_H_
#define _PLATFORMINIT_H_


/*
  Activates the NVM regulator via CRM
*/
void StartNvmReg(void);

/*
  Stops the NVM regulator via CRM
*/
void StopNvmReg(void);


/************************************************************************************
* Perform the complete GPIO port initialization
*
* Interface assumptions:
*
* Return value:
*   NONE
*
* Revision history:
*
*    Date    Author    Comments
*   ------   ------    --------
*   190505   JT        Created
*
************************************************************************************/
void PlatformPortInit(void);

/************************************************************************************
* Initialize platform specific stuff
* NVRAM, Bootloader, low level drivers etc.
*
* Interface assumptions:
*
*   NOTE!!! This First section of this function code is added to ensure that the symbols 
*   are included in the build. The linker will remove these symbols if not referenced.
*   Hope to find a better way 21.10.03 MVC 
*
* Return value:
*   NONE
*
* Revision history:
*
*    Date    Author    Comments
*   ------   ------    --------
*   030505   JT        Created
*
************************************************************************************/
void Platform_Init(void);


/************************************************************************************
* Initialize platform and stack. 
*
* This function is the main initialization procedure, which will be called from the 
* startup code of the project. 
* If the code is build as a stand-alone application, this function is
* called from main() below. 
*
* Interface assumptions:
*
* Return value:
*   NONE
*
* Revision history:
*
*    Date    Author    Comments
*   ------   ------    --------
*   030505   JT        Created
*
************************************************************************************/
void Init_802_15_4 (bool_t mUseTS);


#endif /* _PLATFORMINIT_H_  */ 
