/*
 *
 */

#include "i2c.h"
#include "bus.h"
#include "pme.h"

volatile I2CMasterStatus master;
volatile I2CSlaveStatus  slave;

volatile unsigned char last_byte;

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

	master.state = kI2CIdleState;
	slave.state = kI2CIdleState;

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
	if (master.state == kI2CIdleState)
		i2c_send_start();
	else
		i2c_send_repeatedstart();

	master.state = kI2CSendAddressState;
	master.last_error = kI2CNoError; //Initialize last error
}

void i2c_finish_transmission()
{
	i2c_send_stop();

	master.state = kI2CIdleState;
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
		slave.state = kI2CSendDataState;
		if (msg->flags & kSendImmediately)
			i2c_slave_sendbyte();
		
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
		slave.state = kI2CReceiveDataState;
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
			master.state = kI2CReceiveChecksumState;
		else
			master.state = kI2CReceiveDataState;

		i2c_send_ack(); //Acknowledge all received data bytes
	}
	else
		i2c_begin_receive();
}

int i2c_master_lasterror()
{
	return master.last_error;
}

int i2c_slave_lasterror()
{
	return slave.last_error;
}

void i2c_master_receivechecksum()
{
	if (i2c_received_data())
	{
		unsigned char check = I2C1RCV;

		master_msg->checksum = ~master_msg->checksum + 1;
		master.state = kI2CUserCallbackState;

		if (check == master_msg->checksum)
			master.last_error = kI2CNoError;
		else
			master.last_error = kI2CInvalidChecksum;

		i2c_send_nack(); //We are done with this read transaction, send a nack to the slave per i2c spec
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
		slave.state = kI2CReceiveChecksumState;

		if (slave_msg->flags & kCallbackBeforeChecksum)
		{
			taskloop_add(i2c_slave_callback);
			return; //Don't release clock, job of callback in this case
		}
	}

	i2c_release_clock();
}

void i2c_slave_receivechecksum()
{
	unsigned char check = I2C1RCV;

	slave_msg->checksum = ~slave_msg->checksum + 1;

	slave.state = kI2CUserCallbackState;
	taskloop_add(i2c_slave_callback);

	if (check != slave_msg->checksum)
	{
		//TODO: set checksum error here
	}
	//TODO: Add error processing here

	i2c_release_clock();
}

void __attribute__((interrupt,no_auto_psv)) _MI2C1Interrupt()
{
	//TODO add code for handling bus collision arbitration losses and stops
	switch(master.state)
	{
		case kI2CSendAddressState:
		i2c_transmit(master_msg->address);
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
		I2C1TRN = *(master_msg->data_ptr);
		master_msg->checksum += *(master_msg->data_ptr);
		master_msg->data_ptr += 1;

		if (master_msg->data_ptr == master_msg->last_data)
			master.state = kI2CSendChecksumState; //We will get another interrupt when this byte is done being clocked out
		break;

		case kI2CSendChecksumState:
		master_msg->checksum = (~master_msg->checksum) + 1;
		I2C1TRN = master_msg->checksum;
		master.state = kI2CUserCallbackState;
		break;

		case kI2CUserCallbackState:
		//This data is now sent or received, we need to execute the callback to see what to do next
		taskloop_add(i2c_callback); //It is the job of the user callback to decide whether to 
		break;

		case kI2CIdleState:
		case kI2CReceivedAddressState:
		case kI2CForceStopState:
		break;
	}

	_MI2C1IF = 0;
}

I2CLogicState i2c_slave_state()
{
	return slave.state;
}

void i2c_slave_setidle()
{
	slave.state = kI2CIdleState;
	i2c_release_clock();
}

void i2c_slave_sendbyte()
{
		i2c_transmit(*(slave_msg->data_ptr));

		slave_msg->checksum += *(slave_msg->data_ptr);
		slave_msg->data_ptr += 1;

		if (slave_msg->data_ptr == slave_msg->last_data)
			slave.state = kI2CSendChecksumState;

		i2c_release_clock();
}

void __attribute__((interrupt,no_auto_psv)) _SI2C1Interrupt()
{
	if (i2c_address_received())
		last_byte = I2C1RCV;

	if (slave.state == kI2CIdleState && i2c_address_received() && !i2c_slave_is_read())
	{
		//If we're idle and a write is indicated, this indicates a new rpc call otherwise its a repeated start and we ignore it
		//and wait for the data bytes after it.

		slave.state = kI2CReceivedAddressState;
		taskloop_add(i2c_slave_callback);  //callback has to release SCL once it's located the appropriate command handler
	}
	else if (i2c_address_received() && i2c_slave_is_read())
		taskloop_add(i2c_slave_callback); //we're either being asked for a return status or a return value
	else if (i2c_address_received() && !i2c_slave_is_read())
		i2c_release_clock(); //Ignore address on writes
	else if (slave.state == kI2CReceiveDataState)
		i2c_slave_receivedata();
	else if (slave.state == kI2CReceiveChecksumState)
		i2c_slave_receivechecksum();
	else if (slave.state == kI2CSendDataState)
		i2c_slave_sendbyte();
	else if (slave.state == kI2CSendChecksumState)
	{
		slave_msg->checksum = (~slave_msg->checksum) + 1;
		I2C1TRN = slave_msg->checksum;

		slave.state = kI2CUserCallbackState;
		taskloop_add(i2c_slave_callback);

		i2c_release_clock();
	}
	else if (slave.state == kI2CUserCallbackState)
		taskloop_add(i2c_slave_callback);
	else
	{
		last_byte = I2C1RCV;
		i2c_release_clock(); //If we're in an error state just clock in the bytes and ignore them
	}

	_SI2C1IF = 0;
}