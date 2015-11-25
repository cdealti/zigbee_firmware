/************************************************************************************
*
* Author(s): 
*
* (c) Copyright 2007, Freescale, Inc.  All rights reserved.
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale.
*
* Last Inspected:
* Last Tested:
************************************************************************************/
#ifndef _APP_MAC_INTERRUPT_H_
#define _APP_MAC_INTERRUPT_H_

extern void MACA_Interrupt(void);

extern void (*pfEnableMacaInt)(void);
extern void (*pfDisableMacaInt)(void);
extern unsigned int (*pfIsMacaIntEnabled)(void);
extern unsigned int (*pfIsMacaIntAsserted)(void);
extern int (*pfDisableProccessorIrqs)(void);
extern void (*pfEnableProccessorIrqs)(int);

#define AppInterrupts_DisableProcIrqs() (*pfDisableProccessorIrqs)()
#define AppInterrupts_EnableProcIrqs(if_bits)  (*pfEnableProccessorIrqs)(if_bits)

/************************************************************************************
* Primitives to protect/unprotect from processor interrupts.
*   - a safer/slower way of enabling/disabling processor interrupts. 
* Using backup backup of irq status register handles the state
* of the interrupt mask bit in the status register.
* 
* The backup/restore primitives use a local variable for backup.
* The push/pull primitives use the stack. 
*
* NOTE: 
*   While push/pull is slightly faster than backup/restore, it is potentially more risky. 
*   Care must be taken to not corrupt the stack inside the push/pull protection! 
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
*   130505    JT        Created
* 
************************************************************************************/
// Disable processor IRQ and FIRQ. Returns the status of the IRQ and FIRQ.
#define AppInterrupts_PushIrqStatus() {register unsigned int irqTmp;{irqTmp = AppInterrupts_DisableProcIrqs();}
// Restores the IRQ and FIRQ status.
#define AppInterrupts_PullIrqStatus() {AppInterrupts_EnableProcIrqs(irqTmp);}}


/************************************************************************************
* Primitives to enable/disable MACA interrupts.
* Performed locally on the processors external irq registers
* 
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
*   130505    JT        Created
* 
************************************************************************************/
#define AppInterrupts_EnableMACAIrq()    (*pfEnableMacaInt)()
#define AppInterrupts_DisableMACAIrq()   (*pfDisableMacaInt)()

/************************************************************************************
* Primitives to protect/unprotect from MACA interrupts.
* Performed locally on the processors external irq registers.
* Utilizes a backup/restore approach
* 
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
*   130505    JT        Created
* 
************************************************************************************/
#define AppInterrupts_ProtectFromMACAIrq(var)    var = (*pfIsMacaIntEnabled)(); (*pfDisableMacaInt)();
#define AppInterrupts_UnprotectFromMACAIrq(var)  if(var) (*pfEnableMacaInt)();


#endif /* _APP_MAC_INTERRUPT_H_ */
