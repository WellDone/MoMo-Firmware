//bus.c

//This is where we declare all the MIB state, so don't pull in the external definitions
#define __NO_EXTERN_MIB_STATE__

#include "bus.h"

MIBState 				mib_state;
bank1 unsigned char 	mib_buffer[kBusMaxMessageSize];

//These functions are too small to be efficiently used on the PIC12 which must pass parameters
#ifndef _MACRO_SMALL_FUNCTIONS
void bus_send(unsigned char address, unsigned char *buffer, unsigned char len, unsigned char flags)
{
	//Fill in message contents
	mib_state.bus_msg.address = address;
	mib_state.bus_msg.data_ptr = buffer;
	mib_state.bus_msg.last_data = buffer + len;
	mib_state.bus_msg.flags = flags;

	i2c_send_message();
}

void bus_receive(unsigned char address, unsigned char *buffer, unsigned char len, unsigned char flags)
{
	mib_state.bus_msg.address = address;
	mib_state.bus_msg.data_ptr = buffer;
	
	mib_state.bus_msg.last_data = buffer;
	mib_state.bus_msg.len = len;

	mib_state.bus_msg.flags = flags;

	i2c_receive_message();
}

#endif