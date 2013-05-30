/*
 *
 */

#include "i2c.h"
#include "pme.h"

volatile I2CMasterStatus master;

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
}

void i2c_enable()
{
	//enable module power
	peripheral_enable(kI2CModule);

	master.state = kMasterIdleState;
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

unsigned int i2c_send_message(I2CMessage *msg)
{
	if (master.state != kMasterIdleState)
		return -1;

	master.curr_msg = msg;
	master.dir = kMasterSendData;
	msg->checksum = 0;
	msg->address <<= 1;

	CLEAR_BIT(msg->address, 0); //set write indication

	master.state = kMasterSendingStartState;
	i2c_send_start();

	return 0;
}

void __attribute__((interrupt,no_auto_psv)) _MI2C1Interrupt()
{
	switch(master.state)
	{
		case kMasterSendingStartState:
		I2C1TRN = master.curr_msg->address;
		master.state = kMasterSendingAddressState;
		_RA0 = !_RA0;
		break;

		case kMasterSendingAddressState:
		master.state = kMasterSendingDataState;
		//Purposely fall through

		case kMasterSendingDataState:
		if (master.dir == kMasterSendData)
		{
			//Clock out the data byte in the message
			if (master.curr_msg->data_ptr != master.curr_msg->last_data)
			{
				I2C1TRN = *(master.curr_msg->data_ptr);
				master.curr_msg->checksum += *(master.curr_msg->data_ptr);
				master.curr_msg->data_ptr += 1;
			}
			else
			{
				//Clock out the checksum
				master.curr_msg->checksum = ~master.curr_msg->checksum + 1;
				I2C1TRN = master.curr_msg->checksum;

				master.state = kMasterSendingChecksumState;
			}
		}
		break;

		case kMasterSendingChecksumState:
		master.state = kMasterSendingStopState;
		i2c_send_stop();
		break;

		case kMasterSendingStopState:
		master.state = kMasterIdleState;
		break;

		case kMasterIdleState:
		break;
	}

	_MI2C1IF = 0;
}
