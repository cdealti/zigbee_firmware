;#####################################################################
;# Description:
;#   - Interrupt vectors and C-init
;#####################################################################

; Initialization procedure when NOT using External Storage Bootloader
#ifndef gUseBootloader_d

F_Bit      EQU   0x40
I_Bit      EQU   0x80

Mode_Uvc   EQU   0x10  ; // user mode - has same registers as system mode
Mode_Fvc   EQU   0x11  ; // FIQ mode
Mode_Ivc   EQU   0x12  ; // IRQ mode
Mode_Svc   EQU   0x13  ; // supervisor mode
Mode_Avc   EQU   0x17  ; // Abort mode
Mode_und   EQU   0x1B  ; // undefined instr mode
Mode_sys   EQU   0x1F  ; // system mode

__USR_stack_size     EQU  340*4
__IRQ_stack_size     EQU   96*4
__FIQ_stack_size     EQU  128*4
__UND_stack_size     EQU    8*4
__ABT_stack_size     EQU    8*4
__SUP_stack_size     EQU    8*4

  EXPORT _start_vector_

  IMPORT Main
  IMPORT RP_Vector_Init
  
#if (__VER__ >= 5020000 )
  IMPORT __iar_data_init2
#else
  IMPORT __iar_data_init
#endif  

  IMPORT __rom_data_init
  
  IMPORT Region$$Table$$Base
  IMPORT Region$$Table$$Limit
  IMPORT Region_ROM_Table_ROM_Base
  IMPORT Region_ROM_Table_ROM_Limit

  IMPORT  RP_Vector_Thumb_0
  IMPORT  RP_Vector_Thumb_1
  IMPORT  RP_Vector_Thumb_2
  IMPORT  RP_Vector_Thumb_3
  
#ifndef NO_USE_INTERRUPTS
    IMPORT IrqHandler
    IMPORT FiqHandler
#endif

#ifndef NO_USE_EXCEPTIONS
  ;imported from ItcDriver.c
    IMPORT mExceptionHandlerPrefetchAbort
    IMPORT mExceptionHandlerDataAbort
    IMPORT mExceptionHandlerUndefined
    IMPORT mExceptionHandlerSupervisor
#endif

#ifdef BOOT_SOURCE_IDENTIFICATION
    EXPORT gBootSource
    EXPORT gProgramSize
#endif

  SECTION .vect: CODE

;//--Entry point-------------------------------------------------  

    CODE32 

_start_vector_
    B       __start_                      ; // reset

#ifdef NO_USE_EXCEPTIONS
    B       __reserved_                   ; // reserved
    B       __reserved_                   ; // reserved
    B       __reserved_                   ; // reserved
    B       __reserved_                   ; // reserved
#else
    B       __UndefInstructionHndlr_      ; // undefined instruction
    B       __SupervisorHndlr_            ; // supervisor
    B       __PrefetchAbtHndlr_           ; // prefetch abort
    B       __DataAbtHndlr_               ; // data abort
#endif
    B       __reserved_                   ; // reserved
#ifdef NO_USE_INTERRUPTS
    B       __reserved_                   ; // reserved
    B       __reserved_                   ; // reserved
#else
    B       IrqHandler                    ; // IRQ
    B       FiqHandler                    ; // FIQ
#endif

  SECTION .text: CODE

__start_

#ifdef BOOT_SOURCE_IDENTIFICATION
    LDR   r2,=gBootSource
    STR   r0,[r2]
    LDR   r2,=gProgramSize
    STR   r1,[r2]
#endif

    LDR     r1,=system_stack
    MOV     r0, #Mode_Svc | F_Bit | I_Bit   ;// supervisor mode, disable all interrupts
    MSR     cpsr_c,r0
    ADD     r1,r1,#__SUP_stack_size
    MOV     sp,r1

    MOV     r0, #Mode_Ivc | F_Bit | I_Bit   ;// IRQ mode, disable all interrupts
    MSR     cpsr_c,r0
    ADD     r1,r1,#__IRQ_stack_size
    MOV     sp,r1
    
    MOV     r0, #Mode_Fvc | F_Bit | I_Bit   ;// FIQ mode, disable all interrupts
    MSR     cpsr_c,r0
    ADD     r1,r1,#__FIQ_stack_size
    MOV     sp,r1  

    MOV     r0, #Mode_und | F_Bit | I_Bit   ;// UND mode, disable all interrupts
    MSR     cpsr_c,r0
    ADD     r1,r1,#__UND_stack_size
    MOV     sp,r1

    MOV     r0, #Mode_Avc | F_Bit | I_Bit   ;// ABT mode, disable all interrupts
    MSR     cpsr_c,r0
    ADD     r1,r1,#__ABT_stack_size
    MOV     sp,r1

    MOV     r0, #Mode_sys | F_Bit | I_Bit   ;// system mode, disable all interrupts
    MSR     cpsr_c,r0
    ADD     r1,r1,#__USR_stack_size
    MOV     sp,r1

;    MOV     r0, #Mode_Uvc | F_Bit | I_Bit   ;// user mode, disable all interrupts
;    MSR     cpsr_c,r0

#if (__VER__ >= 5020000 )
    BL      __iar_data_init2
#else
    BL      __iar_data_init
#endif  
;    // See if we need to initialize the ROM variables
    LDR     r0, =Region_ROM_Table_ROM_Base
    LDR     r3, =Region_ROM_Table_ROM_Limit
    CMP     r0, r3
;    // If Region_ROM_Table_ROM_Base and Region_ROM_Table_ROM_Limit are equal, then the ROM image is loaded in RAM (simulation)
;    // No need to init the ROM variables.
    BEQ     __call_main
    BL      __rom_data_init

__call_main
;// the main function was renamed to Main in order to avoid inclusion of C library code for supplementary initializations
    BL      RP_Vector_Init
    B       Main         ; // jump to "Main()"

__reserved_
    MOVS     pc,lr

#ifndef NO_USE_EXCEPTIONS

__UndefInstructionHndlr_
    STMFD    r13!, {r0-r12,r14}
    LDR      r0, =mExceptionHandlerUndefined
    LDR      r0,[r0,#0]
    CMP      r0,#0
    MOVNE    r14,pc
    BXNE     r0
    LDMFD    r13!, {r0-r12,r14}
    MOVS     PC, R14

__SupervisorHndlr_
    STMFD    r13!, {r0-r12,r14}
    LDR      r0, =mExceptionHandlerSupervisor
    LDR      r0,[r0,#0]
    CMP      r0,#0
    MOVNE    r14,pc
    BXNE     r0
    LDMFD    r13!, {r0-r12,r14}
    MOVS     PC, R14

__PrefetchAbtHndlr_
    STMFD    r13!,{r0-r12,r14}
    LDR      r0, =mExceptionHandlerPrefetchAbort
    LDR      r0,[r0,#0]
    CMP      r0,#0
    MOVNE    r14,pc
    BXNE     r0
    LDMFD    r13!,{r0-r12,r14}
    SUBS     PC, r14, #4

__DataAbtHndlr_
    STMFD    r13!,{r0-r12,r14}
    LDR      r0, =mExceptionHandlerDataAbort
    LDR      r0,[r0,#0]
    CMP      r0,#0
    MOVNE    r14,pc
    BXNE     r0
    LDMFD    r13!,{r0-r12,r14}
    SUBS     PC, r14, #4

#endif

  SECTION .stack:DATA(2)   ;// stack in internal RAM
system_stack
    DS8  __USR_stack_size+__IRQ_stack_size+__FIQ_stack_size+__UND_stack_size+__ABT_stack_size+__SUP_stack_size

#ifdef BOOT_SOURCE_IDENTIFICATION
  SECTION .noinit :DATA(2)
gBootSource  DS32 1
gProgramSize DS32 1
#endif

 ;// force the ROM patching referencing
  REQUIRE RP_Vector_Thumb_0
  REQUIRE RP_Vector_Thumb_1
  REQUIRE RP_Vector_Thumb_2
  REQUIRE RP_Vector_Thumb_3
    END
    
;---------------------------------------------------------------  
#else    ; gUseBootloader_d 
; Initialization procedure when using External Storage Bootloader

F_Bit      EQU   0x40
I_Bit      EQU   0x80

Mode_Uvc   EQU   0x10  ; // user mode - has same registers as system mode
Mode_Fvc   EQU   0x11  ; // FIQ mode
Mode_Ivc   EQU   0x12  ; // IRQ mode
Mode_Svc   EQU   0x13  ; // supervisor mode
Mode_Avc   EQU   0x17  ; // Abort mode
Mode_und   EQU   0x1B  ; // undefined instr mode
Mode_sys   EQU   0x1F  ; // system mode

__USR_stack_size     EQU  340*4
__IRQ_stack_size     EQU   96*4
__FIQ_stack_size     EQU  128*4
__UND_stack_size     EQU    8*4
__ABT_stack_size     EQU    8*4
__SUP_stack_size     EQU    8*4

  EXPORT _start_vector_

  IMPORT Main
  IMPORT RP_Vector_Init
  IMPORT Bootloader_Check
  
#if (__VER__ >= 5020000 )
  IMPORT __iar_data_init2
#else
  IMPORT __iar_data_init
#endif  

  IMPORT __rom_data_init
  
  IMPORT Region$$Table$$Base
  IMPORT Region$$Table$$Limit
  IMPORT Region_ROM_Table_ROM_Base
  IMPORT Region_ROM_Table_ROM_Limit

  IMPORT  RP_Vector_Thumb_0
  IMPORT  RP_Vector_Thumb_1
  IMPORT  RP_Vector_Thumb_2
  IMPORT  RP_Vector_Thumb_3
  
#ifndef NO_USE_INTERRUPTS
    IMPORT IrqHandler
    IMPORT FiqHandler
#endif

#ifndef NO_USE_EXCEPTIONS
  ;imported from ItcDriver.c
    IMPORT mExceptionHandlerPrefetchAbort
    IMPORT mExceptionHandlerDataAbort
    IMPORT mExceptionHandlerUndefined
    IMPORT mExceptionHandlerSupervisor
#endif

#ifdef BOOT_SOURCE_IDENTIFICATION
    EXPORT gBootSource
    EXPORT gProgramSize
#endif

  SECTION first_sector_vect: CODE

;//--Entry point-------------------------------------------------  

    CODE32 

_start_vector_
    B       __first_sector_start_         ; // reset and go to bootloader

#ifdef NO_USE_EXCEPTIONS
    B       __reserved_                   ; // reserved
    B       __reserved_                   ; // reserved
    B       __reserved_                   ; // reserved
    B       __reserved_                   ; // reserved
#else
    B       __UndefInstructionHndlr_      ; // undefined instruction
    B       __SupervisorHndlr_            ; // supervisor
    B       __PrefetchAbtHndlr_           ; // prefetch abort
    B       __DataAbtHndlr_               ; // data abort
#endif
    B       __reserved_                   ; // reserved
#ifdef NO_USE_INTERRUPTS
    B       __reserved_                   ; // reserved
    B       __reserved_                   ; // reserved
#else
    B       IrqHandler                    ; // IRQ
    B       FiqHandler                    ; // FIQ
#endif


  SECTION first_sector_start: CODE

__first_sector_start_

    BL Bootloader_Check
    
    PUSH {LR}
    BL __second_sector_start_
    POP {R0}
    BX R0    


  SECTION second_sector_start: CODE

__second_sector_start_

#ifdef BOOT_SOURCE_IDENTIFICATION
    LDR   r2,=gBootSource
    STR   r0,[r2]
    LDR   r2,=gProgramSize
    STR   r1,[r2]
#endif

    LDR     r1,=system_stack
    MOV     r0, #Mode_Svc | F_Bit | I_Bit   ;// supervisor mode, disable all interrupts
    MSR     cpsr_c,r0
    ADD     r1,r1,#__SUP_stack_size
    MOV     sp,r1

    MOV     r0, #Mode_Ivc | F_Bit | I_Bit   ;// IRQ mode, disable all interrupts
    MSR     cpsr_c,r0
    ADD     r1,r1,#__IRQ_stack_size
    MOV     sp,r1
    
    MOV     r0, #Mode_Fvc | F_Bit | I_Bit   ;// FIQ mode, disable all interrupts
    MSR     cpsr_c,r0
    ADD     r1,r1,#__FIQ_stack_size
    MOV     sp,r1  

    MOV     r0, #Mode_und | F_Bit | I_Bit   ;// UND mode, disable all interrupts
    MSR     cpsr_c,r0
    ADD     r1,r1,#__UND_stack_size
    MOV     sp,r1

    MOV     r0, #Mode_Avc | F_Bit | I_Bit   ;// ABT mode, disable all interrupts
    MSR     cpsr_c,r0
    ADD     r1,r1,#__ABT_stack_size
    MOV     sp,r1

    MOV     r0, #Mode_sys | F_Bit | I_Bit   ;// system mode, disable all interrupts
    MSR     cpsr_c,r0
    ADD     r1,r1,#__USR_stack_size
    MOV     sp,r1

;    MOV     r0, #Mode_Uvc | F_Bit | I_Bit   ;// user mode, disable all interrupts
;    MSR     cpsr_c,r0

#if (__VER__ >= 5020000 )
    BL      __iar_data_init2
#else
    BL      __iar_data_init
#endif  
;    // See if we need to initialize the ROM variables
    LDR     r0, =Region_ROM_Table_ROM_Base
    LDR     r3, =Region_ROM_Table_ROM_Limit
    CMP     r0, r3
;    // If Region_ROM_Table_ROM_Base and Region_ROM_Table_ROM_Limit are equal, then the ROM image is loaded in RAM (simulation)
;    // No need to init the ROM variables.
    BEQ     __call_main
    BL      __rom_data_init

__call_main
;// the main function was renamed to Main in order to avoid inclusion of C library code for supplementary initializations
    BL      RP_Vector_Init
    B       Main         ; // jump to "Main()"

__reserved_
    MOVS     pc,lr

#ifndef NO_USE_EXCEPTIONS

__UndefInstructionHndlr_
    STMFD    r13!, {r0-r12,r14}
    LDR      r0, =mExceptionHandlerUndefined
    LDR      r0,[r0,#0]
    CMP      r0,#0
    MOVNE    r14,pc
    BXNE     r0
    LDMFD    r13!, {r0-r12,r14}
    MOVS     PC, R14

__SupervisorHndlr_
    STMFD    r13!, {r0-r12,r14}
    LDR      r0, =mExceptionHandlerSupervisor
    LDR      r0,[r0,#0]
    CMP      r0,#0
    MOVNE    r14,pc
    BXNE     r0
    LDMFD    r13!, {r0-r12,r14}
    MOVS     PC, R14

__PrefetchAbtHndlr_
    STMFD    r13!,{r0-r12,r14}
    LDR      r0, =mExceptionHandlerPrefetchAbort
    LDR      r0,[r0,#0]
    CMP      r0,#0
    MOVNE    r14,pc
    BXNE     r0
    LDMFD    r13!,{r0-r12,r14}
    SUBS     PC, r14, #4

__DataAbtHndlr_
    STMFD    r13!,{r0-r12,r14}
    LDR      r0, =mExceptionHandlerDataAbort
    LDR      r0,[r0,#0]
    CMP      r0,#0
    MOVNE    r14,pc
    BXNE     r0
    LDMFD    r13!,{r0-r12,r14}
    SUBS     PC, r14, #4

#endif


  SECTION .stack:DATA(2)   ;// stack in internal RAM
system_stack
    DS8  __USR_stack_size+__IRQ_stack_size+__FIQ_stack_size+__UND_stack_size+__ABT_stack_size+__SUP_stack_size

#ifdef BOOT_SOURCE_IDENTIFICATION
  SECTION .noinit :DATA(2)
gBootSource  DS32 1
gProgramSize DS32 1
#endif

  SECTION rp_vector_redirect_0: CODE:ROOT(2)
  CODE16
  PUSH {LR}
  BL RP_Vector_Thumb_0 
  POP {R0}
  BX R0

  SECTION rp_vector_redirect_1: CODE:ROOT(2)
  CODE16
  PUSH {LR}
  BL RP_Vector_Thumb_1 
  POP {R0}
  BX R0 

  SECTION rp_vector_redirect_2: CODE:ROOT(2)
  CODE16
  PUSH {LR}
  BL RP_Vector_Thumb_2 
  POP {R0}
  BX R0

  SECTION rp_vector_redirect_3: CODE:ROOT(2)
  CODE16
  PUSH {LR}
  BL RP_Vector_Thumb_3 
  POP {R0}
  BX R0
  

 ;// force the ROM patching referencing
  REQUIRE RP_Vector_Thumb_0
  REQUIRE RP_Vector_Thumb_1
  REQUIRE RP_Vector_Thumb_2
  REQUIRE RP_Vector_Thumb_3
    END


#endif   ; gUseBootloader_d 
