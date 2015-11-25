#ifndef _BEESTACK_ROM_LINKS_H_
#define _BEESTACK_ROM_LINKS_H_

#ifdef __IAR_SYSTEMS_ICC__

extern void BeeStackROMInit(void);
extern void    (*pfAddrMap_InitTable)(void);

#endif  /*  ifdef __IAR_SYSTEMS_ICC__ */


#endif  /* _BEESTACK_ROM_LINKS_H_  */
