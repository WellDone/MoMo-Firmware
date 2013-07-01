#include "i2c.h"

extern volatile I2CMasterStatus master;
extern volatile I2CMessage 		*i2c_msg;
extern task_callback 			i2c_callback;

/*
 * Helper function only used in MI2C1Interrupt in order to encapsulate the receive logic
 */
void i2c_master_receivedata()
{
	if (i2c_received_data())
	{
		unsigned char data = I2C1RCV;
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
}

void i2c_master_enable()
{
	if (master.state == kI2CDisabledState)
		master.state = kI2CIdleState;
}

int i2c_master_lasterror()
{
	return master.last_error;
}

void i2c_master_receivechecksum()
{
	if (i2c_received_data())
	{
		unsigned char check = I2C1RCV;

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

void __attribute__((interrupt,no_auto_psv)) _MI2C1Interrupt()
{
	//_RA6 = !_RA6;

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
		I2C1TRN = *(i2c_msg->data_ptr);
		i2c_msg->checksum += *(i2c_msg->data_ptr);
		i2c_msg->data_ptr += 1;

		if (i2c_msg->data_ptr == i2c_msg->last_data)
			master.state = kI2CSendChecksumState; //We will get another interrupt when this byte is done being clocked out
		break;

		case kI2CSendChecksumState:
		i2c_msg->checksum = (~i2c_msg->checksum) + 1;
		I2C1TRN = i2c_msg->checksum;
		master.state = kI2CUserCallbackState;
		break;

		case kI2CUserCallbackState:
		//This data is now sent or received, we need to execute the callback to see what to do next
		taskloop_add(i2c_callback); //It is the job of the user callback to decide whether to 
		break;

		case kI2CIdleState:
		case kI2CReceivedAddressState:
		case kI2CForceStopState:
		case kI2CDisabledState:
		break;
	}

	_MI2C1IF = 0;
}