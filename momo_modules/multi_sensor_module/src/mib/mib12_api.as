;mib12_api.as
;The mib12 executive defines an API that can be used to perform MIB functions
;like sending messages and setting the slave endpoint return status.  These
;functions are linked into a table at a special point in the mib12_executive
;binary so that application modules can find them.  This file defines symbols
;for calling those functions so that application C code can use them

#include <xc.inc>
#include "constants.h"

global _bus_master_send_rpc, _bus_slave_setreturn
global _bus_master_begin_rpc, _trap, _reset_device 
global _mib_buffer,_mib_packet,_slave_address

;API Functions
_reset_device			equ (kFirstApplicationRow)*kFlashRowSize - 5
_trap					equ (kFirstApplicationRow)*kFlashRowSize - 4
_bus_master_begin_rpc	equ (kFirstApplicationRow)*kFlashRowSize - 3
_bus_master_send_rpc 	equ (kFirstApplicationRow)*kFlashRowSize - 2
_bus_slave_setreturn 	equ (kFirstApplicationRow)*kFlashRowSize - 1

;API Data Structure
psect mibstate class=BANK1,abs
_mib_buffer equ 0xAA
_mib_packet equ 0xA7
_slave_address equ 0xA4