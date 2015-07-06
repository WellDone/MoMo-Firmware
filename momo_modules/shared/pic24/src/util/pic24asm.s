#ifdef __PIC24FJ64GA306__
#include <p24FJ64GA306.inc>
#endif

#ifdef __PIC24F16KA101__
#include <p24F16KA101.inc>
#endif

 .equiv SLEEP_MODE, 0x0000
 .equiv IDLE_MODE, 0x0001

;Magic incantation to allow writing to the rtcon
_asm_enable_rtcon_write:
    PUSH w7
    PUSH w8
    DISI #5 ;Disable interrupts for 5 instructions
    MOV #0x55, w7
    MOV w7, _NVMKEY
    MOV #0xAA, w8
    MOV w8, _NVMKEY
    BSET _RCFGCAL, #13
    POP w8
    POP w7
    return


;reset the device
_asm_reset:
    RESET

_asm_sleep:
    PWRSAV #SLEEP_MODE
    return

_asm_safe_sleep:
    disi #4
    nop ;1
    nop ;2
#ifdef __PIC24FJ64GA306__
    BSET _U1MODE, #7        ;Set the wake bit so that we can wakeup from reception of a character 
#else
    nop ;3
#endif
    btss    INTTREG, #15            
    pwrsav  #SLEEP_MODE
    BCLR _U1MODE, #7
    return

.global _asm_enable_rtcon_write ;note the underscore to handle C name mangling rules
.global _asm_reset
.global _asm_safe_sleep
.global _asm_sleep

