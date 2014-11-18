#ifndef __asm_locations_h__
#define __asm_locations_h__

#include "asm_macros.inc"

#define __DEFINES_ONLY__
#include "mib_definitions.h"
#undef __DEFINES_ONLY__

/*
 * ASM Variable Locations
 * Every variable that is referenced in an assembly file must store its  
 * address here.  This includes bit offsets into other variables and 
 * structure member offsets.  This file can contain only #define information
 * so that it may be included in ASM files as well as C files.
 */

//Variables inside MIBData
#define bus_feature		(_mib_data + 0)
#define bus_command 	(_mib_data + 1)
#define bus_spec		(_mib_data + 2)
#define bus_sender		(_mib_data + 3)

#define bus_status		(_mib_data + 0)
#define bus_statuscheck (_mib_data + 1)
#define bus_length		(_mib_data + 3)
#define mib_buffer		(_mib_data + 4)

//Variables inside MIBState
#define slave_address	(_mib_state + 0)
#define send_address	(_mib_state + 1)
#define curr_loc		(_mib_state + 2)

//Variables inside of ExecutiveStatus
#define WatchdogBit		0
#define ValidAppBit		1
#define BootloadBit		2
#define RegisteredBit	3
#define DirtyResetBit	4
#define SlaveActiveBit	5
#define FirstReadBit	6
#define TrapBit			7

#define ASM_INCLUDE_GLOBALS()		global _mib_data, _mib_state, _status

#ifdef kMultipageDevice
#define reset_page()			pagesel($)
#else
#define reset_page()
#endif

#endif