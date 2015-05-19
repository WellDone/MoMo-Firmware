#ifndef __bus_common_h__
#define __bus_common_h__

#include "platform.h"
#include "mib_hal.h"
#include "i2c.h"
#include "protocol.h"
#include "executive_state.h"

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
#define plist_set_int16(n, val)			((int*)mib_packet.data)[n] = val
#define plist_set_int32(n, val)			((uint16*)mib_packet.data)[n>>1] = val
#define plist_set_int8(n, hi, val)		mib_packet.data[(n<<1) + hi] = val
#define plist_get_int16(n)				((int*)mib_packet.data)[n]
#define plist_get_int8(n)				mib_packet.data[(n<<1) ]
#define plist_get_buffer(n)				(mib_packet.data + (n << 1))
#define plist_get_buffer_length()		(mib_data.bus_command.param_spec & 0b00011111)

#endif
