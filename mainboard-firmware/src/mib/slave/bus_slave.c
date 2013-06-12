#include "bus_slave.h"

//MIB Global State
extern volatile MIBState 		mib_state;
extern volatile unsigned char 	mib_buffer[kBusMaxMessageSize];
extern unsigned int 			mib_firstfree;

//static prototypes that are only to be used in this file
void bus_slave_startcommand();
void bus_slave_receiveparam(MIBParameterHeader *param, int header_or_value);
void bus_slave_searchcommand();
void bus_slave_callcommand();

/*
 * MIB Slave Logic 
 */

void bus_slave_startcommand()
{
	//Initialize all the state
	mib_state.slave_state = kMIBSearchCommand;
	mib_state.slave_handler = kInvalidMIBHandler;
	mib_state.num_reads = 0;

	bus_slave_setreturn(kUnknownError, 0); //Make sure that if nothing else happens we return an error status.

	bus_slave_receive((unsigned char *)&mib_state.slave_command, 2, 0);
}

void bus_slave_seterror(unsigned char error)
{
	mib_state.slave_returnstatus.result = error;
	mib_state.slave_returnstatus.len = 0;

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

	if (mib_state.slave_params->count > 0)
	{
		bus_slave_receiveparam((MIBParameterHeader*)&mib_state.slave_params[0], 0);
		mib_state.slave_state = kMIBReceiveParameterValue;
	}
	else
	{
		//There we no parameters so jump straight to executing the command
		mib_state.slave_state = kMIBExecuteCommandHandler;
	}
}

void bus_slave_callcommand()
{	
	if (mib_state.slave_handler != kInvalidMIBHandler && mib_state.slave_state == kMIBExecuteCommandHandler)
		mib_state.slave_handler(kMIBExecuteCommand, mib_state.slave_params);

	//slave callback should set the return status via bus_slave_setreturn and set mib_buffer to point to the return value if any
	bus_slave_send((unsigned char*)&mib_state.slave_returnstatus, sizeof(MIBReturnValueHeader), kSendImmediately);

	//If we don't expect to send a return value, finish the command after this transmission
	if (mib_state.slave_returnstatus.len == 0)
		mib_state.slave_state = kMIBFinishCommand;
}
void bus_slave_reset()
{
	//A write after any number of reads is a protocol reset so we reset ourselves back to idle
	//This is a failsafe to make sure we can't get into a state where the slave locks up
	bus_free_all();
	mib_state.slave_state = kMIBIdleState;
	mib_state.num_reads = 0;
	i2c_slave_setidle();
}

void bus_slave_callback()
{	
	_RA6 = !_RA6;

	if (i2c_address_received())
	{
		if (i2c_slave_is_read())
		{
			mib_state.num_reads += 1;

			if (mib_state.num_reads == 1)
			{
				//First read is for the return status
				bus_slave_callcommand();
			}
			else if (mib_state.num_reads == 2)
			{
				//Second read is for the return value, which if there is one should be in the mib_buffer
				//if there wasn't one, then this is a protocol error since there shouldn't have been a second read
				//so we can send garbage.
				if (mib_state.slave_returnstatus.len != 0)
					bus_slave_send((unsigned char *)mib_buffer, mib_state.slave_returnstatus.len, kSendImmediately);
				else
					bus_slave_send((unsigned char *)mib_buffer, 1, kSendImmediately); //protocol error so just send 1 byte, doesn't matter
				
				mib_state.slave_state = kMIBFinishCommand;
			}
		}
		else
		{
			//We received a write
			//If this is not a special situation (protocol reset or start of command) ignore the address byte
			//and wait for the data to be clocked in.
			
			if (mib_state.num_reads != 0)
				bus_slave_reset();
			else if (mib_state.slave_state == kMIBIdleState)
				bus_slave_startcommand(); //A write when we're idle indicates a new command
		}

		//Always release the clock.  The slave should never hold the clock forever.
		i2c_release_clock();
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
		/*if (i2c_slave_lasterror() != kI2CNoError)
			bus_slave_seterror(kWrongChecksum);*/
		break;

		case kMIBFinishCommand:
		case kMIBExecuteCommandHandler:
		case kMIBIdleState:
		case kMIBProtocolError:
		break;
	}

	i2c_release_clock();
}