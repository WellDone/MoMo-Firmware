;Assembly routines for dealing with I2C things in an efficient way
;very quickly and with minimal code overhead


#include <xc.inc>
#include "i2c_defines.h"
#include "asm_locations.h"
#include "asm_branches.inc"

ASM_INCLUDE_GLOBALS()

global _i2c_master_receive_message, _i2c_append_checksum, _i2c_master_send_message

PSECT text_bus_master,local,class=CODE,delta=2

;Given a length in W, prepare the buffer to _bus_master_receive
;that many bytes and call i2c routine to start sending
BEGINFUNCTION _bus_master_receive
	banksel _mib_state
	addwf BANKMASK(buffer_start),w
	movwf BANKMASK(buffer_end)
	goto _i2c_master_receive_message
ENDFUNCTION _bus_master_receive


BEGINFUNCTION _bus_master_send
	banksel _mib_state
	addwf BANKMASK(buffer_start),w
	movwf BANKMASK(curr_loc)
	call _i2c_append_checksum

	movf BANKMASK(curr_loc),w
	movwf BANKMASK(buffer_end)

	movf BANKMASK(buffer_start),w
	movwf BANKMASK(curr_loc)

	goto _i2c_master_send_message
ENDFUNCTION _bus_master_send

BEGINFUNCTION _get_mib_result
	movlb	1	; select bank1
	swapf	BANKMASK(bus_retstatus),w
  	rrf		WREG,f
  	andlw	7
  	return
ENDFUNCTION _get_mib_result