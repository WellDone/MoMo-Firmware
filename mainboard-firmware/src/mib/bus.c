//bus.c

#include "bus.h"
#include "task_manager.h"
#include <string.h>

volatile MIBState 		mib_state;
volatile unsigned char 	mib_buffer[kBusMaxMessageSize];
unsigned int 			mib_firstfree;

void bus_init()
{
	I2CConfig config;

	config.address = kControllerPICAddress;
	config.priority = 0b010;
	config.callback = bus_master_callback;
	config.slave_callback = bus_slave_callback;

	i2c_init_flags(&config);
	i2c_set_flag(&config, kEnableGeneralCallFlag);
	i2c_set_flag(&config, kEnableSoftwareClockStretchingFlag);

	mib_firstfree = 0;

	i2c_configure(&config);
	i2c_enable();
}

int bus_send(unsigned char address, unsigned char *buffer, unsigned char len, unsigned char flags)
{
	volatile I2CMessage *msg = i2c_address_valid(address) ? &mib_state.master_msg : &mib_state.slave_msg;

	if (len > kBusMaxMessageSize)
		return -1;

	//Fill in message contents
	msg->address = address;
	msg->data_ptr = buffer;
	msg->last_data = buffer + len;
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

void bus_master_callback()
{
	switch(mib_state.master_state)
	{
		case kMIBSendParameters:
		bus_send(bus_master_lastaddress(), (unsigned char*)mib_buffer, mib_state.master_param_length, 0);
		mib_state.master_state = kMIBReadReturnStatus;
		break;

		case kMIBReadReturnStatus:
		bus_receive(bus_master_lastaddress(), (unsigned char *)mib_buffer, sizeof(MIBReturnValueHeader), 0);
		mib_state.master_state = kMIBReadReturnValue;
		break;

		case kMIBReadReturnValue:
		if (i2c_master_lasterror() != kI2CNoError)
		{
			//TODO: properly handle error here by calling callback with result code;
			bus_free_all();
			i2c_finish_transmission();
		}
		else
		{
			MIBReturnValueHeader *header = (MIBReturnValueHeader *)mib_buffer;
			mib_state.master_result = header->result;

			if (header->len > 0)
			{
				bus_receive(bus_master_lastaddress(), (unsigned char *)mib_buffer, header->len, 0);
				mib_state.master_state = kMIBExecuteCallback;
			}
			else
			{
				bus_free_all();
				i2c_finish_transmission(); //TODO: Call rpc callback to inform of error
			}
		}
		break;

		case kMIBExecuteCallback:
		bus_free_all();
		i2c_finish_transmission(); 
		break;
	}
}

void bus_slave_startcommand()
{
	mib_state.slave_state = kMIBSearchCommand;
	mib_state.slave_handler = kInvalidMIBHandler;

	bus_slave_receive((unsigned char *)&mib_state.slave_command, 2, 0);
}

void bus_slave_seterror(int error)
{
	mib_state.last_slave_error = error;
	mib_state.slave_state = kMIBProtocolError;
}

void bus_slave_setreturn(unsigned char status, volatile MIBParameterHeader *value)
{
	mib_state.slave_returnstatus.result = status;

	if (value == 0)
		mib_state.slave_returnstatus.len = 0;
	else
		mib_state.slave_returnstatus.len = value->len + sizeof(MIBParameterHeader);
}

/* call with 0 to get the header, > 0 to get the value */
void bus_slave_receiveparam(MIBParameterHeader *param, int header_or_value)
{
	if (header_or_value == 0)
		bus_slave_receive((unsigned char *)&mib_state.last_param, sizeof(MIBParameterHeader), kCallbackBeforeChecksum);
	else
	{
		//Make sure the type is right
		if (mib_state.last_param.type != param->type)
		{
			bus_slave_seterror(kWrongParameterType);
			return;
		}
		else if(mib_state.last_param.len > param->len)
		{
			bus_slave_seterror(kParameterTooLong);
			return;
		}

		if (param->type == kMIBInt16Type)
			bus_slave_receive((unsigned char *)(&((MIBIntParameter*)param)->value), param->len, kCallbackBeforeChecksum);
		else
			bus_slave_receive((unsigned char *)((MIBBufferParameter*)param)->data, ((MIBBufferParameter*)param)->header.len, kCallbackBeforeChecksum);
	}
}

void bus_slave_searchcommand()
{
	mib_state.slave_handler = find_handler(mib_state.slave_command.feature, mib_state.slave_command.command);
	if (mib_state.slave_handler == kInvalidMIBHandler)
	{
		bus_slave_seterror(kUnsupportedCommand);
		return;
	}

	//Get the list of parameters that we have to receive
	mib_state.slave_params = mib_state.slave_handler(kMIBCreateParameters, 0);
	mib_state.slave_params->curr = 0;
	//TODO: Handle case of inability to allocate slave_params data structure

	//TODO: Handle case of command with no parameters

	bus_slave_receiveparam((MIBParameterHeader*)&mib_state.slave_params[0], 0);
	mib_state.slave_state = kMIBReceiveParameterValue;
}

void bus_slave_callcommand()
{	
	mib_state.slave_handler(kMIBExecuteCommand, mib_state.slave_params);
	//slave callback should set the return status via bus_slave_setreturn and set mib_buffer to point to the return value if any
	bus_slave_send((unsigned char*)&mib_state.slave_returnstatus, sizeof(MIBReturnValueHeader), kSendImmediately);

	if (mib_state.slave_returnstatus.len > 0)
		mib_state.slave_state = kMIBSendReturnValue;
	else
		mib_state.slave_state = kMIBFinishCommandState;
}

void bus_slave_callback()
{
	_RA6 = !_RA6;
	
	if (i2c_slave_state() == kI2CReceivedAddressState)
	{
		bus_slave_startcommand();
		i2c_release_clock(); 		//We clock stretched until this point
		return;
	}
	else if (i2c_slave_is_read() && mib_state.slave_state == kMIBProtocolError)
	{
		bus_free_all();

		mib_state.slave_returnstatus.result = mib_state.last_slave_error;
		mib_state.slave_returnstatus.len = 0;

		bus_slave_send((unsigned char*)&mib_state.slave_returnstatus, sizeof(MIBReturnValueHeader), kSendImmediately); //Must send immediately since we're called after address byte so no callback before master clocks first byte in
		mib_state.slave_state = kMIBFinishCommandState;
		return;
	}

	switch (mib_state.slave_state)
	{
		case kMIBSearchCommand:
		bus_slave_searchcommand();
		break;

		case kMIBReceiveParameterValue:
		bus_slave_receiveparam(mib_state.slave_params->params[mib_state.slave_params->curr], 1);
		mib_state.slave_params->curr += 1;
		if (mib_state.slave_params->curr == mib_state.slave_params->count)
			mib_state.slave_state = kMIBFinishedReceivingParameters;
		else
			mib_state.slave_state = kMIBReceiveParameterHeader;
		break;

		case kMIBReceiveParameterHeader:
		bus_slave_receiveparam(mib_state.slave_params->params[mib_state.slave_params->curr], 0);
		mib_state.slave_state = kMIBReceiveParameterValue;
		break;

		case kMIBFinishedReceivingParameters:
		mib_state.slave_state = kMIBReceivedParameterChecksum;
		break;

		case kMIBReceivedParameterChecksum:
		mib_state.slave_state = kMIBExecuteCommandHandler;
		break;

		case kMIBExecuteCommandHandler:
		bus_slave_callcommand();
		break;

		case kMIBSendReturnValue:
		//If we're in this state, the mib_buffer has the return value already loaded and the length is in the slave_returnstatus struct
		bus_slave_send((unsigned char *)mib_buffer, mib_state.slave_returnstatus.len, 0);
		mib_state.slave_state = kMIBWaitForSentReturnValue;
		break;

		case kMIBWaitForSentReturnValue:
		mib_state.slave_state = kMIBFinishCommandState;
		break;

		case kMIBFinishCommandState:
		bus_free_all();
		mib_state.slave_state = kMIBIdleState;
		i2c_slave_setidle();
		break;

		case kMIBIdleState:
		case kMIBProtocolError:
		break;
	}

	i2c_release_clock();
}