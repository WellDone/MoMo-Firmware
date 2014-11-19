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

//Define macros for manipulating the mib_buffer
#define plist_set_int16(n, val)			((int*)mib_data.buffer)[n] = val
#define plist_set_int32(n, val)			((uint16*)mib_data.buffer)[n>>1] = val
#define plist_set_int8(n, hi, val)		mib_data.buffer[(n<<1) + hi] = val
#define plist_get_int16(n)				((int*)mib_data.buffer)[n]
#define plist_get_int8(n)				mib_data.buffer[(n<<1) ]
#define plist_get_buffer(n)				(mib_data.buffer + (n << 1))
#define plist_get_buffer_length()		(mib_data.bus_command.param_spec & 0b00011111)

//3 bytes long
typedef struct 
{
	//Master variables
	uint8   slave_address;
	uint8	send_address;

	//Slave variables
	uint8 	curr_loc;
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