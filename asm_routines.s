


;INCLUDE "p24f16ka101.inc"

;Magic incantation to allow writing to the rtcon
_asm_enable_rtcon_write:
    MOV NVMKEY, W1
    MOV #0x55, W2
    MOV W2, [W1]
    MOV #0xAA, W3
    MOV W3, [W1]
    BSET RCFGCAL, #13
    RETURN

;reset the device
_asm_reset
    RESET

.global _asm_enable_rtcon_write ;note the underscore to handle C name mangling rules
.global _asm_reset

