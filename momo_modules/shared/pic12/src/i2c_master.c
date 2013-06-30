#include "i2c.h"
#include "bus_master.h"

extern volatile I2CMasterStatus master;
extern unsigned char 			i2c_slave_address;

#define i2c_msg		(&mib_state.bus_msg)

/*
 * Helper function only used in i2c_master_interrupt() in order to encapsulate the receive logic
 */
inline void i2c_master_receivedata()
{
	if (i2c_received_data())
	{
		unsigned char data = i2c_receive();
		*(i2c_msg->last_data) = data;
		i2c_msg->checksum += data;
		i2c_msg->last_data += 1;

		//Check if we are at the end of the message
		if ((i2c_msg->last_data - i2c_msg->data_ptr) == i2c_msg->len)
			master.state = kI2CReceiveChecksumState;
		else
			master.state = kI2CReceiveDataState;

		i2c_send_ack(); //Acknowledge all received data bytes
	}
	else
		i2c_begin_receive();
}

void i2c_master_disable()
{
	//TODO: check if we are in the middle of sending an RPC and try later
	master.state = kI2CDisabledState;

	i2c_set_slave_mode();
	SSP1ADD = i2c_slave_address;
}

void i2c_master_enable()
{
	if (master.state == kI2CDisabledState)
		master.state = kI2CIdleState;

	SSP1IE = 0;
	SSPEN = 0;
	GCEN = 0;

	i2c_set_master_mode();
	SSP1ADD = 0x4F; //Set baud rate to 100 khz for 32 mhz internal oscillator

	SSPEN = 1;

    /* Enable the MSSP interrupt (for i2c). */
    SSP1IF = 0;
    SSP1IE = 1;
}

uint8 i2c_master_lasterror()
{
	return master.last_error;
}

I2CLogicState i2c_master_state()
{
	return master.state;
}

inline void i2c_master_receivechecksum()
{
	if (i2c_received_data())
	{
		unsigned char check = i2c_receive();

		i2c_msg->checksum = ~i2c_msg->checksum + 1;
		master.state = kI2CUserCallbackState;

		if (check == i2c_msg->checksum)
			master.last_error = kI2CNoError;
		else
			master.last_error = kI2CInvalidChecksum;

		i2c_send_nack(); //We are done with this read transaction, send a nack to the slave per i2c spec
	}
	else
		i2c_begin_receive();
}

void i2c_master_interrupt()
{
	//TODO add code for handling bus collision arbitration losses and stops
	switch(master.state)
	{
		case kI2CSendAddressState:
		i2c_transmit(i2c_msg->address);
		master.state = (master.dir == kMasterSendData) ? kI2CSendDataState : kI2CReceiveDataState;
		break;

		case kI2CReceiveDataState:
		i2c_master_receivedata();
		break;

		case kI2CReceiveChecksumState:
		i2c_master_receivechecksum();
		break;

		//Send Logic
		case kI2CSendDataState:
		i2c_transmit(*(i2c_msg->data_ptr));
		i2c_msg->checksum += *(i2c_msg->data_ptr);
		i2c_msg->data_ptr += 1;

		if (i2c_msg->data_ptr == i2c_msg->last_data)
			master.state = kI2CSendChecksumState; //We will get another interrupt when this byte is done being clocked out
		break;

		case kI2CSendChecksumState:
		i2c_msg->checksum = (~i2c_msg->checksum) + 1;
		i2c_transmit(i2c_msg->checksum);
		master.state = kI2CUserCallbackState;
		break;

		case kI2CUserCallbackState:
		if (i2c_byte_nacked())
			master.last_error = kI2CNackReceived;		//Check if the data was successfully sent

		//This data is now sent or received, we need to execute the callback to see what to do next
		bus_master_callback(); //It is the job of the user callback to decide what to do
		break;

		case kI2CIdleState:
		case kI2CReceivedAddressState:
		case kI2CForceStopState:
		case kI2CDisabledState:
		break;
	}
}
