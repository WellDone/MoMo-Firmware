#include "i2c.h"
#include "bus_master.h"

extern bank1 volatile I2CStatus i2c_status;
extern unsigned char 			i2c_slave_address;

#define i2c_msg		(&mib_state.bus_msg)

/*
 * Helper function only used in i2c_master_interrupt() in order to encapsulate the receive logic
 */
void i2c_master_receivedata()
{
	if (i2c_received_data())
	{
		i2c_core_transfer(pack_i2c_states(1, 0, kI2CReceiveDataState, kI2CReceiveChecksumState));

		i2c_send_ack(); //Acknowledge all received data bytes
	}
	else
		i2c_begin_receive();
}

void i2c_master_receivechecksum()
{
	if (i2c_received_data())
	{
		i2c_core_receivechecksum();

		i2c_send_nack(); //We are done with this read transaction, send a nack to the slave per i2c spec
	}
	else
		i2c_begin_receive();
}