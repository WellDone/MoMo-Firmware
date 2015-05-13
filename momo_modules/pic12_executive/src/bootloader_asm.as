;Assembly routines for porting main.c over from xc8 c to assembly code.
;We need to do this because xc8 is terrible and uses too much memory for
;stupid things like moving W to a GPR register and back just to make a
;zero flag appear.   


#include <xc.inc>
#define _DEFINES_ONLY
#include "bootloader.h"
#undef  __DEFINES_ONLY__
#include "i2c_defines.h"
#include "asm_locations.h"
#include "mib12_block.h"

ASM_INCLUDE_GLOBALS()

global _bus_master_begin_rpc, _bus_master_send_rpc, _copy_mib_to_boot, _load_boot_address
global _boot_id, _invalid_row, _boot_source, _offset, _app_buffer
global _flash_erase_row, _flash_write_row, _get_boot_source, _get_firmware_id

;Global bootloader variables (put the app buffer in bank0 right after other variables)
;Place the rest in bank1 so that we don't use the highest 2 bytes 
;dabs 1, 0x20, 25, _mib_packet
;dabs 1, 0x39, 3, _mib_state
;dabs 1, 0x3c, 1, _status
dabs 1, 0x3d, 32, _app_buffer

dabs 1, 0xA0, 1, _boot_id
dabs 1, 0xA1, 1, _boot_count
dabs 1, 0xA2, 1, _boot_source
dabs 1, 0xA3, 1, _offset
dabs 1, 0xA4, 1, _invalid_row

PSECT text_bootloader_asm,local,class=CODE,delta=2

;Fetch part of a row from the bootloader source
;Takes in a byte in the global offset (see global variables above) that specifies the offset that
;request should fetch in the row.
BEGINFUNCTION _get_half_row
	banksel _boot_source
	movf BANKMASK(_boot_source),w
	call _bus_master_begin_rpc
	
	banksel _boot_id
	movf 	BANKMASK(_boot_id),w
	banksel _mib_packet
	movwf BANKMASK(mib_buffer+0)
	clrf  BANKMASK(mib_buffer+1)

	banksel _offset
	movf BANKMASK(_offset),w

	banksel _mib_packet
	movwf BANKMASK(mib_buffer+2)	;mib_buffer second int param to offset (low byte)
	
	;Load in the feature and comand info
	movlf 0x07, bus_cmdlo
	movlf 0x04, bus_cmdhi

	call _load_boot_address

	movlw 4
	call _bus_master_send_rpc

	;Make sure that if any of the calls to get half_row succeed then we program the
	;entire row.
	banksel _invalid_row
	xorlw kNoErrorWithDataStatus
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
	movwf BANKMASK(_app_buffer + kFirmwareIDBufferOffset)
	movlw kRetlwHighWord
	movwf BANKMASK(_app_buffer + kFirmwareIDBufferOffset + 1)
	return
ENDFUNCTION _set_firmware_id

BEGINFUNCTION _prepare_reflash
	banksel _app_buffer

	;Move retlw source to its spot
	movwf BANKMASK(_app_buffer + kFirmwareSourceBufferOffset)
	movlw kRetlwHighWord
	movwf BANKMASK(_app_buffer + kFirmwareSourceBufferOffset + 1)

	;Move retlw magic to its spot
	movwf BANKMASK(_app_buffer + kMagicNumberBufferOffset + 1)
	movlw kReflashMagicNumber
	movwf BANKMASK(_app_buffer + kMagicNumberBufferOffset)

	;Erase and program the row
	movlw kMIBStructRow
	call _flash_erase_row
	movlw kMIBStructRow
	goto _flash_write_row
ENDFUNCTION _prepare_reflash

BEGINFUNCTION _enter_bootloader
	;Save off the boot source, id and row counter
	banksel _boot_source
	call _get_boot_source
	movwf BANKMASK(_boot_source)
	call _get_firmware_id
	movwf BANKMASK(_boot_id)

	movlw kFirstApplicationRow-1
	movwf BANKMASK(_boot_count)

	;FIXME IOC DISABLE and ALARM
	;//Let everyone know that we are reflashing
	;ioc_disable(); //make sure we don't reset ourselves when we take the bus down

	;LATCH(ALARM) = 0;
	;PIN_DIR(ALARM, OUTPUT);

	bootload_loop:
		banksel _boot_count
		incf BANKMASK(_boot_count),f

		movlw 1
		movwf BANKMASK(_invalid_row)

		clrf BANKMASK(_offset)
		fetch_loop:
			;check offset != kBootloaderBufferSize
			movf BANKMASK(_offset),w
			xorlw kBootloaderBufferSize
			btfsc ZERO
				goto done_fetching

			call _get_half_row

			;offset += kMIBRequestSize
			banksel _offset
			movlw kMIBRequestSize
			addwf BANKMASK(_offset),f
			goto fetch_loop

		done_fetching:

		;Erase the flash row
		movf BANKMASK(_boot_count),w
		call _flash_erase_row
		
		;If _invalid_row == 0, program the row
		banksel _invalid_row
		movf BANKMASK(_invalid_row),w
		btfss ZERO
			goto done_programming
		
		;Program the row
		movf BANKMASK(_boot_count),w
		call _flash_write_row

		done_programming:
		movlw kNumFlashRows-1
		banksel _boot_count
		xorwf BANKMASK(_boot_count),w
		btfss ZERO
			goto bootload_loop

	;FIXME: Reenable ioc and clear ALARM
	;PIN_DIR(ALARM, INPUT);
	;ioc_enable(ALARMPORT); 
	return
ENDFUNCTION _enter_bootloader