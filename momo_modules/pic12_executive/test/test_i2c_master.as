;Name: test_i2c_master
;Targets: all
;Type: executive
;I2C Capture: S, 0x08/WA, 0x0a/A, 0x0b/A, 0x0c/A, 0x0a/A, 0x00/A, 0x00/A, 0x00/A, 0x00/A, 0x00/A, 0x00/A, 0x00/A, 0x00/A, 0x00/A, 0x00/A, 0x00/A, 0x00/A, 0x00/A, 0x00/A, 0x00/A, 0x00/A, 0x00/A, 0x00/A, 0x00/A, 0x00/A, 0xd5/A, RS, 0x08/RA, 0xc0/A, 0x40/A, 0x00/A, 0x14/A, 0x00/A, 0x01/A, 0x02/A, 0x03/A, 0x04/A, 0x05/A, 0x06/A, 0x07/A, 0x08/A, 0x09/A, 0xa/A, 0xb/A, 0xc/A, 0xd/A, 0xe/A, 0xf/A, 0x10/A, 0x11/A, 0x12/A, 0x13/A, 0x2e/N, P
;Attach Slave:8, responder_testpattern.py
;Description:Test to ensure that the i2c master routines are correctly
;returning the appropriate error codes. 

#include <xc.inc>
#include "asm_macros.inc"
#include "symbols.h"
#include "test_macros.inc"
#include "test_asserts.inc"

global _begin_tests
global _loghex, _finish_tests, _assertv
global _mib_buffer, _mib_packet

PSECT text_unittest,local,class=CODE,delta=2

BEGINFUNCTION _begin_tests
	asm_call_initialize()

	movlw 10
	asm_call_bus_init()	;enable mib slave mode

	asm_call_bus_master_begin_rpc()

	banksel _mib_packet
	movlw  10
	movwf BANKMASK(_mib_packet+0)
	movlw  11
	movwf BANKMASK(_mib_packet+1)
	movlw  12
	movwf BANKMASK(_mib_packet+2)
	movlw  10
	movwf BANKMASK(_mib_packet+3)

	asm_call_i2c_append_checksum()
	movlw 8
	asm_call_i2c_master_send_message()
	movlw 0
	btfsc DC
		movlw 1

	assertlw 0

	movlw 8
	asm_call_i2c_master_receive_message()
	movlw 0
	btfsc DC
		movlw 1
	btfsc CARRY
		movlw 2

	assertlw 0

	asm_call_i2c_finish_transmission()
	return
ENDFUNCTION _begin_tests