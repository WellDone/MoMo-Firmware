;Assembly routines for porting main.c over from xc8 c to assembly code.
;We need to do this because xc8 is terrible and uses too much memory for
;stupid things like moving W to a GPR register and back just to make a
;zero flag appear.   


#include <xc.inc>
#define __DEFINES_ONLY__
#include "mib_definitions.h"
#define _DEFINES_ONLY
#include "bootloader.h"
#undef  __DEFINES_ONLY__
#include "i2c_defines.h"
#include "asm_locations.h"
#include "constants.h"


ASM_INCLUDE_GLOBALS()

global _bus_master_begin_rpc, _bus_master_send_rpc, _copy_mib_to_boot, _load_boot_address
global _boot_id, _invalid_row, _boot_source, _offset, _app_buffer
global _flash_erase_row, _flash_write_row

PSECT text_bootloader_asm,local,class=CODE,delta=2

;Fetch part of a row from the bootloader source
;Takes in a byte in the global offset (bootloader.c) that specifies the offset that
;request should fetch in the row.
BEGINFUNCTION _get_half_row
	call _bus_master_begin_rpc
	banksel _boot_id
	movf 	BANKMASK(_boot_id),w
	banksel _mib_data
	movwf BANKMASK(mib_buffer+0)
	clrf  BANKMASK(mib_buffer+1)

	banksel _offset
	movf BANKMASK(_offset),w

	banksel _mib_data
	movwf BANKMASK(mib_buffer+2)	;mib_buffer second int param to offset (low byte)
	
	;Load in the feature and comand info
	movlf 0x07, bus_feature
	movlf 0x04, bus_command
	movlf 2 shl 5, bus_spec

	call _load_boot_address
	
	banksel _boot_source
	movf BANKMASK(_boot_source),w
	call _bus_master_send_rpc

	;Make sure that if any of the calls to get half_row succeed then we program the
	;entire row.
	banksel _invalid_row
	iorlw 0x00 ;Make sure that the ZERO flag is set if it's 0
	btfsc ZERO
		clrf BANKMASK(_invalid_row),f

	;Load the offset that we should copy to here
	banksel _offset
	movf BANKMASK(_offset),w
	goto _copy_mib_to_boot
ENDFUNCTION _get_half_row

;Given a uint8 in W that sets which firmware bucket to request from
;the firmware source, store that in the app_buffer for programming into
;the high row of flash so that we remember it when we reset to bootloader mode
;app_buffer is kFlashRowSize words which is 2*kFlashRowSize bytes and stored
;in little endian mode
BEGINFUNCTION _set_firmware_id
	banksel _app_buffer
	movwf _app_buffer + ((kFlashRowSize - 3) << 1)
	movlw kRetlwHighWord
	movwf _app_buffer + ((kFlashRowSize - 3) << 1) + 1
	return
ENDFUNCTION _set_firmware_id

BEGINFUNCTION _prepare_reflash
	banksel _app_buffer

	;Move retlw source to second highest word
	movwf _app_buffer + ((kFlashRowSize - 2) << 1)
	movlw kRetlwHighWord
	movwf _app_buffer + ((kFlashRowSize - 2) << 1) + 1

	;Move retlw magic to highest word 
	movwf _app_buffer + ((kFlashRowSize - 1) << 1) + 1
	movlw kReflashMagicNumber
	movwf _app_buffer + ((kFlashRowSize - 1) << 1)

	;Erase and program the row
	movlw kMIBStructRow
	call _flash_erase_row
	movlw kMIBStructRow
	goto _flash_write_row
ENDFUNCTION _prepare_reflash