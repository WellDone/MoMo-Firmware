#include "i2c.h"

extern volatile I2CSlaveStatus  slave;
extern volatile I2CMessage      *i2c_msg;
extern task_callback 			i2c_slave_callback; 

void i2c_slave_receivedata()
{
	unsigned char data = I2C1RCV;

	*(i2c_msg->data_ptr++) = data;
	i2c_msg->checksum += data;

	//Check if we are at the end of the message
	if (i2c_msg->data_ptr == i2c_msg->last_data)
	{
		//two's complement checksum accumulated with message should sum to 0
		if (i2c_msg->checksum == 0)
			slave.last_error = kI2CNoError;
		else
			slave.last_error = kI2CInvalidChecksum;

		slave.state = kI2CUserCallbackState;
		taskloop_add_critical(i2c_slave_callback, NULL);
	}

	i2c_release_clock();
}

I2CLogicState i2c_slave_state()
{
	return slave.state;
}

void i2c_slave_receive_mesage()
{
    slave.state = kI2CReceiveDataState;
}

void i2c_slave_sendbyte()
{
	slave.state = kI2CSendDataState;

	i2c_transmit(*(i2c_msg->data_ptr++));

	if (i2c_msg->data_ptr == i2c_msg->last_data)
		slave.state = kI2CIdleState;

	i2c_release_clock();
}

int i2c_slave_lasterror()
{
	return slave.last_error;
}

void __attribute__((interrupt,no_auto_psv)) _SI2C1Interrupt()
{

	if (i2c_address_received())
	{
		I2C1RCV;
		_I2COV = 0;

		i2c_master_disable(); //If we receive a valid address, the the master must not be running so take over the i2c data structures
		taskloop_add_critical(i2c_slave_callback, NULL);
	}		
	else 
	{
		switch(slave.state)
		{
			case kI2CReceiveDataState:
			i2c_slave_receivedata();
			break;

			case kI2CSendDataState:
			i2c_slave_sendbyte();
			break;

			case kI2CUserCallbackState:
			taskloop_add_critical(i2c_slave_callback, NULL);
			break;

			default:
			//by default do not read bytes if we don't know what to do with them so that the master sees the 
			//error and does a bus reset.
			_I2COV = 0; //If the receive buffer is full we will nack, but if we don't clear the overflow we won't get any more interrupts
			i2c_release_clock();
			break;
		}
	}

	_SI2C1IF = 0;
}