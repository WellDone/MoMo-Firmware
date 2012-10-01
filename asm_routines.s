


;INCLUDE "p24f16ka101.inc"

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


;reset the device
_asm_reset:
    RESET

.global _asm_enable_rtcon_write ;note the underscore to handle C name mangling rules
.global _asm_reset

