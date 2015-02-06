#include <xc.inc>

#define DEFINES_ONLY
#include "buffers.h"
#include "asm_macros.inc"
#include "asm_branches.inc"
#include "mib_locations.inc"

global _mib_to_fsr0, _copy, _add_w_fsr1, _load_buffer
global _comm_destination
global _mib_buffer,_mib_packet

PSECT gsmstream,global,class=CODE,delta=2

;Given a mib buffer where the first 2 bytes specify
;the offset and the last X bytes represent the characters
;to write to the comm_destination, copy those characters
;and always NULL terminate.
;Uses: FSR0, FSR1
;Returns: 1 if buffer too large, 0 otherwise
BEGINFUNCTION _set_comm_destination
	banksel _mib_buffer

	;Load in the destination buffer
	;Start at offset indicated by first byte of mib_buffer
	movlw kDestinationBuffer
	fcall _load_buffer
	movf  BANKMASK(_mib_buffer),w
	fcall _add_w_fsr1

	;Calculate needed size and make sure it will fit
	movf	BANKMASK(param_spec),w
	andlw 	param_buffer_mask
	addwf 	BANKMASK(_mib_buffer),w
	skipwgtl (GSM_COMM_DESTINATION_LENGTH-1) ;Check if the size would exceed buffer (including added \0)
		retlw 1

	fcall _mib_to_fsr0
	addfsr FSR0, 2		;Offset to start of buffer portion

	;Do the copying
	movf 	BANKMASK(param_spec),w
	andlw 	param_buffer_mask
	fcall 	_copy
	clrf	INDF1
	retlw 	0
ENDFUNCTION _set_comm_destination

;Give an index in W, return the value at that position
;in the comm_destination buffer
BEGINFUNCTION _comm_destination_get
	movwf FSR0L
	movlw kDestinationBuffer
	fcall _load_buffer
	movf FSR0L,w
	fcall _add_w_fsr1
	movf INDF1,w
	return
ENDFUNCTION _comm_destination_get
