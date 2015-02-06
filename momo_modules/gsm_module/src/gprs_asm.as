#include <xc.inc>

#define DEFINES_ONLY
#include "buffers.h"
#include "asm_macros.inc"
#include "asm_branches.inc"
#include "mib_locations.inc"

global _gprs_apn
global _load_buffer
global _mib_buffer,_mib_packet
global _mib_to_fsr0,_copy

PSECT gprs_text,global,class=CODE,delta=2

;Initialize APN to a NULL string
BEGINFUNCTION _gprs_init_buffers
	movlw kAPNBuffer
	fcall _load_buffer
	movlw 0
	movwf INDF1
	return
ENDFUNCTION _gprs_init_buffers

;Copy the APN from the mib buffer into the APN buffer and
;NULL terminate it
BEGINFUNCTION _gprs_set_apn
	fcall 	_mib_to_fsr0
	movlw	kAPNBuffer
	fcall 	_load_buffer
	banksel param_spec
	movf 	BANKMASK(param_spec),w
	andlw 	param_buffer_mask
	fcall 	_copy
	clrf	INDF1
	return
ENDFUNCTION _gprs_set_apn
