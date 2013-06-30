
/* bus module
 * This module implements routines and definitions related to the Momo Intermodule Bus (MIB)
 */

#ifndef __bus_h__
#define __bus_h__

#include "platform.h"
#include "i2c.h"
#include "protocol.h"
#include "bus_common.h"

#ifdef _MACRO_SMALL_FUNCTIONS
#define bus_send(add, buffer, len, f)				\
{													\
	mib_state.bus_msg.address = add;				\
	mib_state.bus_msg.data_ptr = buffer;			\
	mib_state.bus_msg.last_data = buffer + len;		\
	mib_state.bus_msg.flags = f;					\
	i2c_send_message();								\
}

#define bus_receive(add, buffer, length, f)			\
{													\
	mib_state.bus_msg.address = add;				\
	mib_state.bus_msg.data_ptr = buffer;			\
	mib_state.bus_msg.last_data = buffer;			\
	mib_state.bus_msg.len = length;					\
	mib_state.bus_msg.flags = f;					\
	i2c_receive_message();							\
}

#define bus_allocate_space(len)		mib_buffer+mib_firstfree; mib_firstfree += len;

#define bus_free_all()								\
{													\
	mib_firstfree = 0;								\
}

#define bus_init_int_param(param, val)				\
{													\
	param->header.type = kMIBInt16Type;				\
	param->header.len = 2;							\
	param->value = val;								\
}

#define bus_init_buffer_param(param, d, length)		\
{													\
	param->header.type = kMIBBufferType;			\
	param->header.len = length;						\
	param->data = d;								\
}

#endif

#endif
