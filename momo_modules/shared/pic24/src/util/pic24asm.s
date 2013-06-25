#include <p24F16KA101.inc>
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

.global _asm_enable_rtcon_write ;note the underscore to handle C name mangling rules
.global _asm_reset
.global _asm_sleep

