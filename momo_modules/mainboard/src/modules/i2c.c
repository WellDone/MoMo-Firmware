/*
 *
 */

#include "i2c.h"
#include "bus.h"
#include "pme.h"
#include "utilities.h"

volatile I2CMasterStatus master;
volatile I2CSlaveStatus  slave;

volatile I2CMessage 	 *i2c_msg;

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

int i2c_send_message(volatile I2CMessage *msg)
{
	i2c_msg = msg;
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
	i2c_msg = msg;

	if (!(msg->flags & kContinueChecksum))
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