#include "i2c.h"
#include "bus_slave.h"

extern bank1 volatile I2CStatus i2c_status;

#define i2c_msg		(&mib_state.bus_msg)

void i2c_slave_receivedata()
{
	unsigned char data = i2c_receive();

	*(mib_state.bus_msg.data_ptr) = data;
	mib_state.bus_msg.checksum += data;
	mib_state.bus_msg.data_ptr += 1;

	//Check if we are at the end of the message
	if (mib_state.bus_msg.data_ptr == mib_state.bus_msg.last_data)
		i2c_status.state = kI2CReceiveChecksumState;

	i2c_release_clock();
}

void i2c_slave_receivechecksum()
{
	unsigned char check = i2c_receive();

	i2c_msg->checksum = ~i2c_msg->checksum + 1;

	if (check != i2c_msg->checksum)
		i2c_status.last_error = kI2CInvalidChecksum;
	else
		i2c_status.last_error = kI2CNoError;

	i2c_status.state = kI2CUserCallbackState;
	bus_slave_callback();

	i2c_release_clock();
}

uint8 i2c_slave_active()
{
	return i2c_status.slave_active;
}


void i2c_slave_setidle()
{
	i2c_status.state = kI2CIdleState;
	i2c_status.last_error = kI2CNoError;

	//i2c_master_enable(); //let the master logic use the bus again if it needs.

	i2c_release_clock();
}

void i2c_slave_sendbyte()
{
	i2c_transmit(*(i2c_msg->data_ptr));

	i2c_msg->checksum += *(i2c_msg->data_ptr);
	i2c_msg->data_ptr += 1;

	if (i2c_msg->data_ptr == i2c_msg->last_data)
		i2c_status.state = kI2CSendChecksumState;

	i2c_release_clock();
}

void i2c_slave_interrupt()
{	
	if (i2c_address_received())
	{
		i2c_receive();
		SSPOV = 0; //reset receive overflow flag

		//TODO Fix this
		//i2c_master_disable(); //If we receive a valid address, the the master must not be running so take over the i2c data structures

		bus_slave_callback();
	}		
	else 
	{
		switch(i2c_status.state)
		{
			case kI2CReceiveDataState:
			i2c_slave_receivedata();
			break;

			case kI2CReceiveChecksumState:
			i2c_slave_receivechecksum();
			break;

			case kI2CSendDataState:
			i2c_slave_sendbyte();
			break;

			case kI2CSendChecksumState:
			i2c_msg->checksum = (~i2c_msg->checksum) + 1;
			i2c_transmit(i2c_msg->checksum);

			i2c_status.state = kI2CUserCallbackState;
			i2c_release_clock();
			break;

			case kI2CUserCallbackState:
			bus_slave_callback();
			i2c_status.state = kI2CIdleState;
			break;

			default:
			//by default do read bytes if we don't know what to do with them so that the protocol doesn't
			//lock up
			i2c_receive();
			SSPOV = 0; 
			i2c_release_clock();
			break;
		}
	}
}
