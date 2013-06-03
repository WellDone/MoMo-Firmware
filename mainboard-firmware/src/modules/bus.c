//bus.c

#include "bus.h"
#include "sendcommand.h"
#include "receivecommand.h"
#include "task_manager.h"
#include <string.h>

volatile MIBState 		mib_state;
volatile unsigned char 	mib_buffer[kBusMaxMessageSize];

void bus_init()
{
	I2CConfig config;

	config.address = 8;
	config.priority = 0b010;
	config.callback = bus_master_callback;
	config.slave_callback = bus_slave_callback;

	i2c_init_flags(&config);
	i2c_set_flag(&config, kEnableGeneralCallFlag);
	i2c_set_flag(&config, kEnableSoftwareClockStretchingFlag);

	i2c_configure(&config);
	i2c_enable();
}

int bus_send(unsigned char address, unsigned char *buffer, unsigned char len, unsigned char flags)
{
	volatile I2CMessage *msg = i2c_address_valid(address) ? &mib_state.master_msg : &mib_state.slave_msg;

	if (len > kBusMaxMessageSize)
		return -1;

	memcpy((void *)mib_buffer, buffer, len); //volatile warning checked and okay

	//Fill in message contents
	msg->address = address;
	msg->data_ptr = mib_buffer;
	msg->last_data = mib_buffer + len;
	msg->flags = flags;

	return i2c_send_message(msg);
}

unsigned char bus_master_lastaddress()
{
	return mib_state.master_msg.address >> 1;
}


int bus_receive(unsigned char address, volatile unsigned char *buffer, unsigned char len, unsigned char flags)
{
	volatile I2CMessage *msg = i2c_address_valid(address) ? &mib_state.master_msg : &mib_state.slave_msg;

	msg->address = address;
	msg->data_ptr = buffer;
	
	msg->last_data = buffer;
	msg->len = len;

	msg->flags = flags;

	return i2c_receive_message(msg);
}

int bus_master_sendcommand(unsigned char address, MIBCommandPacket *cmd, mib_callback callback, unsigned char *response)
{
	mib_state.subcommand_handler = sendcommand_master_callback;
	mib_state.subcommand_state = kCommandBegin;

	mib_state.master_handler = callback;
	mib_state.master_state = kCommandBegin;

	return bus_send(address, (unsigned char *)cmd, sizeof(MIBCommandPacket), 0);
}

void bus_master_callback()
{
	if (mib_state.subcommand_state == kCommandFinished)
		mib_state.subcommand_handler = kInvalidMIBHandler;

	//If we're executing a subcommand, use that handler, otherwise use master command handler
	if (mib_state.subcommand_handler != kInvalidMIBHandler)
		mib_state.subcommand_state = mib_state.subcommand_handler(mib_state.subcommand_state);
	else
	{
		if (mib_state.master_state == kCommandFinished)
			mib_state.master_handler = kInvalidMIBHandler;

		if (mib_state.master_handler != kInvalidMIBHandler)
			mib_state.master_state = mib_state.master_handler(mib_state.master_state);
	}
}

void bus_slave_startcommand()
{
	mib_state.slave_subhandler = slave_receive_command;
	mib_state.slave_substate = kCommandBegin;

	mib_state.slave_handler = kInvalidMIBHandler;

	bus_slave_receive((unsigned char *)&mib_state.slave_command, 2, 0);
}

void bus_slave_callback()
{
	if (i2c_slave_state() == kSlaveReceivedAddressState)
	{
		bus_slave_startcommand();
		i2c_release_clock(); 		//We clock stretched until this point
		return;
	}

	if (mib_state.slave_substate == kCommandFinished)
		mib_state.slave_subhandler = kInvalidMIBHandler;

	//If we're executing a subcommand, use that handler, otherwise use master command handler
	if (mib_state.slave_subhandler != kInvalidMIBHandler)
		mib_state.slave_substate = mib_state.slave_subhandler(mib_state.slave_substate);
	else
	{
		if (mib_state.slave_state == kCommandFinished)
			mib_state.slave_handler = kInvalidMIBHandler;

		if (mib_state.slave_handler != kInvalidMIBHandler)
			mib_state.slave_state = mib_state.slave_handler(mib_state.slave_state);
	}
}