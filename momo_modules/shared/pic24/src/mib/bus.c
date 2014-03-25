//bus.c

#include "bus.h"

/*
 * Basic MIB data send and receive primitives.  These functions all add 1 to the 
 * length of data specified in the parameters to account for the checksum that must be
 * appended to the end of the buffer by the caller using bus_append_checksum(buffer, len)
 *
 */

static void bus_prepare_message(unsigned char address, unsigned char *buffer, unsigned char len)
{
	mib_state.bus_msg.address = address;
	mib_state.bus_msg.data_ptr = buffer;
	mib_state.bus_msg.last_data = buffer + len + 1;
	mib_state.bus_msg.checksum = 0;
}	

void bus_send(unsigned char address, unsigned char *buffer, unsigned char len)
{
	bus_prepare_message(address, buffer, len);
	i2c_send_master_message();
}

void bus_receive(unsigned char address, unsigned char *buffer, unsigned char len)
{
	bus_prepare_message(address, buffer, len);
	i2c_receive_message();
}

void bus_slave_send(unsigned char* buffer, uint8 len)					
{
	bus_prepare_message(kInvalidI2CAddress, buffer, len);
	i2c_slave_sendbyte();
}

void bus_slave_receive(unsigned char* buffer, uint8 len) 				
{
	bus_prepare_message(kInvalidI2CAddress, buffer, len);
	i2c_slave_receive_mesage();
}

/*
 * Given a buffer of size at least length+1, compute a 2's complement checksum
 * of the first length bytes and store it at position length+1.
 */

void bus_append_checksum(unsigned char *buffer, unsigned int length)
{
	unsigned int i;
	unsigned char checksum = 0;

	for (i=0; i<length; ++i)
		checksum += buffer[i];

	buffer[length] = (~checksum) + 1;
}