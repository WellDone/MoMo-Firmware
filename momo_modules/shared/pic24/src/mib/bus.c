//bus.c

#include "bus.h"

/*
 * immediate must be 0 for all master calls.  Can be 1 for slave calls if the first message byte should 
 * be clocked out immediately, i.e. the slave is currently clock stretching and the master is waiting
 * to clock the byte in.  Those are the only valid values.  Other values WILL break things.
 */ 

void bus_send(unsigned char address, unsigned char *buffer, unsigned char len)
{
	//Fill in message contents
	mib_state.bus_msg.address = address;
	mib_state.bus_msg.data_ptr = buffer;
	mib_state.bus_msg.last_data = buffer + len;

	i2c_send_message();
}

void bus_receive(unsigned char address, unsigned char *buffer, unsigned char len)
{
	mib_state.bus_msg.address = address;
	mib_state.bus_msg.data_ptr = buffer;
	mib_state.bus_msg.last_data = buffer+len;

	i2c_receive_message();
}

void bus_slave_send(unsigned char* buffer, uint8 len, unsigned char imm)					\
{
	mib_state.bus_msg.address = kInvalidI2CAddress | imm;
	mib_state.bus_msg.data_ptr = buffer;
	mib_state.bus_msg.last_data = buffer + len;
	i2c_send_message();
}

void bus_slave_receive(unsigned char* buffer, uint8 len, unsigned char imm) 				\
{
	mib_state.bus_msg.address = kInvalidI2CAddress | imm;
	mib_state.bus_msg.data_ptr = buffer;
	mib_state.bus_msg.last_data = buffer + len;
	i2c_receive_message();
}