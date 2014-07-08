#ifndef __bus_common_h__
#define __bus_common_h__

#include "platform.h"
#include "mib_hal.h"
#include "i2c.h"
#include "protocol.h"
#include "bus_defines.h"

//Takes 2 bits to store
//Cannot change.  Referenced by mib_hal.as in pic12 code
typedef enum
{
	kMIBIdleState = 0,
	kMIBSearchCommand = 1,
	kMIBFinishCommand = 2,
	kMIBProtocolError = 3,
} MIBSlaveState;

//Need these because XC8 is really bad at optimizing bit operations
#define shift_master_state(state)		(state << 4)
#define set_master_state(state)			{mib_state.combined_state &= 0b10001111; mib_state.combined_state |= shift_master_state(state);}
#define shift_slave_state(state)		(state << 2)
#define set_slave_state(state)			{mib_state.combined_state &= 0b111110011; mib_state.combined_state |= shift_slave_state(state);}
#define bus_has_returnvalue()			(mib_state.bus_returnstatus.len != 0)
#define bus_get_returnvalue_length()	(mib_state.bus_returnstatus.len)

//Define macros for manipulating the mib_buffer
#define plist_set_int16(n, val)			((int*)mib_data.buffer)[n] = val
#define plist_set_int32(n, val)			((uint16*)mib_data.buffer)[n>>1] = val
#define plist_set_int8(n, hi, val)		mib_data.buffer[(n<<1) + hi] = val
#define plist_get_int16(n)				((int*)mib_data.buffer)[n]
#define plist_get_int8(n)				mib_data.buffer[(n<<1) ]
#define plist_get_buffer(n)				(mib_data.buffer + (n << 1))
#define plist_get_buffer_length()		(mib_data.bus_command.param_spec & 0b00011111)

//7 bytes long
typedef struct 
{
	//Shared Buffers
	union 
	{
		uint8	slave_handler;	
		uint8	send_address; 
	}; //1 byte

	uint8 		buffer_start;
	uint8 		curr_loc;
	uint8 		buffer_end;

	uint8       slave_address;

	//Master needs to save off these two variables
	//in case there is a bus error and they're overwritten with garbage
	//see bus_master.c
	uint8		save_command;
	uint8		save_spec; 
} MIBState;

/*
 * Make sure files have access to the fundamental MIB global state variables so that
 * we can replace functions with macros that reference these variables.
 */

#ifndef __NO_EXTERN_MIB_STATE__

#include "mib_state.h"

#endif

uint8 plist_param_length(void);
uint8 bus_retval_size();

#endif