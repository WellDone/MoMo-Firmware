/*
 *
 */

#include "i2c.h"
#include "bus.h"
#include "pme.h"

volatile I2CMasterStatus master;
volatile I2CSlaveStatus  slave;

volatile I2CMessage 	 *master_msg;
volatile I2CMessage      *slave_msg;


task_callback i2c_callback;
task_callback i2c_slave_callback; 

void i2c_configure(const I2CConfig *config)
{
	peripheral_enable(kI2CModule);
	
	//Set flags
	I2C1CON = config->flags & kI2CFlagMask;
	
	//Set slave address
	I2C1MSK = 0x00; //All address bits are significant
	I2C1ADD = config->address;

	I2C1BRG = 0x4E; //Set baud rate to 100 khz for an 8 mhz instruction freq (F_osc/2)

	//Set priority
	_SI2C1P = config->priority;
	_MI2C1P = config->priority;

	//i2c callbacks for processing events
	i2c_callback = config->callback;
	i2c_slave_callback = config->slave_callback;
}

void i2c_enable()
{
	unsigned char unused;
	//enable module power
	peripheral_enable(kI2CModule);

	master.state = kMasterIdleState;
	slave.state = kSlaveIdleState;

	unused = I2C1RCV; //Clear out receive buffer
	_I2CEN = 1;

	//set up interrupts
	_SI2C1IF = 0;
	_SI2C1IE = 1;

	_MI2C1IF = 0;
	_MI2C1IE = 1;
}

void i2c_disable()
{
	//disable interrupts
	_SI2C1IE = 0;

	//cut power to module
	peripheral_disable(kI2CModule);
}

void i2c_start_transmission()
{
	if (master.state == kMasterIdleState)
		i2c_send_start();
	else
		i2c_send_repeatedstart();

	master.state = kMasterSendAddressState;
}

void i2c_finish_transmission()
{
	i2c_send_stop();

	master.state = kMasterIdleState;
}

volatile I2CMessage** i2c_select_pointer(volatile I2CMessage *msg)
{
	if (!i2c_address_valid(msg->address))
		return &slave_msg;

	return &master_msg;
}

int i2c_send_message(volatile I2CMessage *msg)
{
	volatile I2CMessage **msg_ptr = i2c_select_pointer(msg);

	*msg_ptr = msg;
	msg->checksum = 0;

	//Check if this is a slave transmission
	if (!i2c_address_valid(msg->address))
	{
		slave.state = kSlaveTransmitState;
		return 0;
	}

	master.dir = kMasterSendData;
	msg->address <<= 1;

	CLEAR_BIT(msg->address, 0); //set write indication

	i2c_start_transmission();
	return 0;
}

int i2c_receive_message(volatile I2CMessage *msg)
{
	volatile I2CMessage **msg_ptr = i2c_select_pointer(msg);

	*msg_ptr = msg;
	msg->checksum = 0;

	//Check if this is a slave reception
	if (!i2c_address_valid(msg->address))
	{
		_RA1 = 0;
		slave.state = kSlaveReceiveState;
		return 0;
	}

	master.dir = kMasterReceiveData;
	msg->address <<= 1;

	SET_BIT(msg->address, 0); //set read indication

	i2c_start_transmission();
	return 0;
}

/*
 * Helper function only used in MI2C1Interrupt in order to encapsulate the receive logic
 */
void i2c_master_receivedata()
{
	if (i2c_received_data())
	{
		unsigned char data = I2C1RCV;
		*(master_msg->last_data) = data;
		master_msg->checksum += data;
		master_msg->last_data += 1;

		//Check if we are at the end of the message
		if ((master_msg->last_data - master_msg->data_ptr) == master_msg->len)
			master.state = kMasterReceiveChecksumState;
		else
			master.state = kMasterReceiveDataState;

		i2c_send_ack(); //Acknowledge all received data bytes
	}
	else
		i2c_begin_receive();
}

void i2c_master_receivechecksum()
{
	if (i2c_received_data())
	{
		unsigned char check = I2C1RCV;

		master_msg->checksum = ~master_msg->checksum + 1;

		if (check == master_msg->checksum)
		{
			master.state = kMasterUserCallbackState;
			i2c_send_ack();
		}
		else
		{
			master.state = kMasterUserCallbackState;
			i2c_send_nack();
		}
	}
	else
		i2c_begin_receive();
}

void i2c_slave_receivedata()
{
	unsigned char data = I2C1RCV;

	*(slave_msg->last_data) = data;
	slave_msg->checksum += data;
	slave_msg->last_data += 1;

	//Check if we are at the end of the message
	if ((slave_msg->last_data - slave_msg->data_ptr) == slave_msg->len)
	{
		slave.state = kSlaveReceiveChecksumState;
	}

	i2c_release_clock();
}

void i2c_slave_receivechecksum()
{
	unsigned char check = I2C1RCV;

	slave_msg->checksum = ~slave_msg->checksum + 1;

	if (check == slave_msg->checksum)
	{
		slave.state = kSlaveUserCallbackState;
		taskloop_add(i2c_slave_callback);
	}

	i2c_release_clock();
}

void __attribute__((interrupt,no_auto_psv)) _MI2C1Interrupt()
{
	switch(master.state)
	{
		case kMasterSendAddressState:
		if (master.dir == kMasterSendData)
		{
			I2C1TRN = master_msg->address;
			master.state = kMasterTransferDataState;
		}
		else
		{
			I2C1TRN = master_msg->address;
			master.state = kMasterReceiveDataState;
		}
		break;

		case kMasterReceiveDataState:
		i2c_master_receivedata();
		break;

		case kMasterReceiveChecksumState:
		i2c_master_receivechecksum();
		break;

		//Send Logic
		case kMasterTransferDataState:
		I2C1TRN = *(master_msg->data_ptr);
		master_msg->checksum += *(master_msg->data_ptr);
		master_msg->data_ptr += 1;

		if (master_msg->data_ptr == master_msg->last_data)
			master.state = kMasterSendChecksumState;
		break;

		case kMasterSendChecksumState:
		master_msg->checksum = (~master_msg->checksum) + 1;
		I2C1TRN = master_msg->checksum;
		master.state = kMasterUserCallbackState;
		break;

		case kMasterUserCallbackState:
		//This data is now sent or received, we need to execute the callback to see what to do next
		taskloop_add(i2c_callback); //It is the job of the user callback to decide whether to 
		break;

		case kMasterIdleState:
		break;
	}

	_MI2C1IF = 0;
}

I2CSlaveState i2c_slave_state()
{
	return slave.state;
}

void __attribute__((interrupt,no_auto_psv)) _SI2C1Interrupt()
{
	/*
	 * Possible states
	 * 1) Receive address when idle -> begin receiving command
	 * 2) Receive stop -> let usercode clean up and stop
	 * 3) Receive address when not idle -> ignore since we just care about commands without repeated starts
	 */

	if (i2c_stop_received())
	{
		if (slave.state != kSlaveIdleState)
		{
			//TODO: allow usercode to clean up here
		}

		slave.state = kSlaveIdleState;
		i2c_release_clock();
	}
	else if ((slave.state == kSlaveIdleState) && i2c_address_received())
	{
		volatile unsigned char unused = I2C1RCV; //Clear the buffer
		//If we're idle, this indicates a new command otherwise its a repeated start and we ignore it
		//and wait for the data bytes after it.
		slave.state = kSlaveReceivedAddressState;
		taskloop_add(i2c_slave_callback);  //callback has to release SCL
	}
	else if (slave.state == kSlaveReceiveState && i2c_received_data())
	{
		_RA1 = 0;
		i2c_slave_receivedata();
	}
	else if (slave.state == kSlaveReceiveChecksumState && i2c_received_data())
		i2c_slave_receivechecksum();
	else if (slave.state == kSlaveTransmitState && !i2c_transmit_full())
	{
		_RA6 = 0;
		I2C1TRN = *(slave_msg->data_ptr);
		slave_msg->checksum += *(slave_msg->data_ptr);
		slave_msg->data_ptr += 1;

		if (slave_msg->data_ptr == slave_msg->last_data)
			slave.state = kSlaveSendChecksumState;

		i2c_release_clock();
	}
	else if (slave.state == kSlaveSendChecksumState && !i2c_transmit_full())
	{
		slave_msg->checksum = (~slave_msg->checksum) + 1;
		I2C1TRN = slave_msg->checksum;
		master.state = kSlaveUserCallbackState;

		i2c_release_clock();
	}
	else if (slave.state == kSlaveUserCallbackState)
		taskloop_add(i2c_slave_callback);
	
	_SI2C1IF = 0;
}