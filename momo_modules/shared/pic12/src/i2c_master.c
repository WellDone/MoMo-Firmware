#include "i2c.h"
#include "bus_master.h"

extern volatile 				I2CStatus i2c_status;
extern bank1 unsigned char 		i2c_slave_address;

#define i2c_msg		(&mib_state.bus_msg)

/*
 * Helper function only used in i2c_master_interrupt() in order to encapsulate the receive logic
 */
inline void i2c_master_receivedata()
{
	if (i2c_received_data())
	{
		unsigned char data = i2c_receive();
		*(i2c_msg->data_ptr) = data;
		i2c_msg->checksum += data;
		i2c_msg->data_ptr += 1;

		//Check if we are at the end of the message
		if (i2c_msg->data_ptr == i2c_msg->last_data)
			i2c_status.state = kI2CReceiveChecksumState;
		else
			i2c_status.state = kI2CReceiveDataState;

		i2c_send_ack(); //Acknowledge all received data bytes
	}
	else
		i2c_begin_receive();
}

void i2c_master_disable()
{
	//TODO: check if we are in the middle of sending an RPC and try later
	i2c_status.slave_active = 1;
	i2c_status.state = kI2CIdleState;

	SSP1IE = 0;
	GCEN = 0;
    
    SSPOV = 0; //clear overflow bit

	SSP1ADD = i2c_slave_address;
	i2c_set_slave_mode();

	SEN = 1;

    /* Enable the MSSP interrupt (for i2c). */
    SSP1IF = 0;
    SSP1IE = 1;
}

void i2c_master_enable()
{
	SSP1IE = 0;
	GCEN = 0;

	SEN = 0; //This has a different meaning in Master mode
	
	i2c_status.slave_active = 0;
	i2c_status.state = kI2CIdleState;

	SSP1ADD = 0x09; //Set baud rate to 100 khz for 4 mhz internal oscillator
	i2c_set_master_mode();

    /* Enable the MSSP interrupt (for i2c). */
    SSP1IF = 0;
    SSP1IE = 1;
}

inline void i2c_master_receivechecksum()
{
	if (i2c_received_data())
	{
		unsigned char check = i2c_receive();

		i2c_msg->checksum = ~i2c_msg->checksum + 1;
		i2c_status.state = kI2CUserCallbackState;

		if (check == i2c_msg->checksum)
			i2c_status.last_error = kI2CNoError;
		else
			i2c_status.last_error = kI2CInvalidChecksum;

		i2c_send_nack(); //We are done with this read transaction, send a nack to the slave per i2c spec
	}
	else
		i2c_begin_receive();
}

void i2c_master_interrupt()
{
	RA5 = !RA5;
	//TODO add code for handling bus collision arbitration losses and stops
	switch(i2c_status.state)
	{
		case kI2CSendAddressState:
		i2c_transmit(i2c_msg->address);
		i2c_status.state = (((i2c_msg->address) & 0x01) == 0) ? kI2CSendDataState : kI2CReceiveDataState;
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
			i2c_status.state = kI2CSendChecksumState; //We will get another interrupt when this byte is done being clocked out
		break;

		case kI2CSendChecksumState:
		i2c_msg->checksum = (~i2c_msg->checksum) + 1;
		i2c_transmit(i2c_msg->checksum);
		i2c_status.state = kI2CUserCallbackState;
		break;

		case kI2CUserCallbackState:
		if (i2c_byte_nacked())
			i2c_status.last_error = kI2CNackReceived;		//Check if the data was successfully sent

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
