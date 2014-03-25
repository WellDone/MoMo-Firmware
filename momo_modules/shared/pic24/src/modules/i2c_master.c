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
		if (i2c_receive_byte())
		{
			//above call returns true when we're at the end of the data we asked for
			master.state = kI2CUserCallbackState;

			//two's complement checksum accumulated with message should sum to 0
			if (i2c_msg->checksum == 0)
				master.last_error = kI2CNoError;
			else
				master.last_error = kI2CInvalidChecksum;

			i2c_send_nack();
		}
		else
		{
			//Acknowledge all received data bytes
			i2c_send_ack();
			master.state = kI2CReceiveDataState;
		}

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
	master.last_error = kI2CNoError;
	master.state = kI2CIdleState;

	I2C1RCV;
}

int i2c_master_lasterror()
{
	return master.last_error;
}

void __attribute__((interrupt,no_auto_psv)) _MI2C1Interrupt()
{
	_MI2C1IF = 0;

	//Check if there was a bus collision and alert the calling routine so they can retransmit
	if (_BCL)
	{
		_BCL = 0;
		master.last_error = kI2CCollision;
		taskloop_add_critical(i2c_callback);
		return;
	}

	switch(master.state)
	{
		case kI2CSendAddressState:
		i2c_transmit(i2c_msg->address);
		I2C1RCV;
		master.state = (master.dir == kMasterSendData) ? kI2CSendDataState : kI2CReceiveDataState;
		break;

		case kI2CReceiveDataState:
		i2c_master_receivedata();
		break;

		case kI2CSendDataState:
		i2c_transmit(*(i2c_msg->data_ptr));
		i2c_msg->data_ptr += 1;

		if (i2c_msg->data_ptr == i2c_msg->last_data)
			master.state = kI2CUserCallbackState; //We will get another interrupt when this byte is done being clocked out
		break;

		case kI2CUserCallbackState:
		//This data is now sent or received, we need to execute the callback to see what to do next
		taskloop_add_critical(i2c_callback); //It is the job of the user callback to decide whether to 
		break;

		default:
		break;
	}
}