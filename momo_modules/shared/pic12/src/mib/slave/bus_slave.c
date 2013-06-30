#include "bus_slave.h"
#include "mib_command.h"

//static prototypes that are only to be used in this file
static void bus_slave_startcommand();
static void bus_slave_receiveparam(MIBParameterHeader *param, uint8 header_or_value, unsigned char flag);
static void bus_slave_searchcommand();
static void bus_slave_callcommand();

/*
 * MIB Slave Logic 
 */

static void bus_slave_startcommand()
{
	//Initialize all the state
	mib_state.slave_state = kMIBSearchCommand;
	mib_state.slave_handler = NULL;
	mib_state.num_reads = 0;

	bus_slave_setreturn(kUnknownError, 0); //Make sure that if nothing else happens we return an error status.

	bus_slave_receive((unsigned char *)&mib_state.bus_command, 2, 0);
}

void bus_slave_seterror(unsigned char error)
{
	mib_state.bus_returnstatus.result = error;
	mib_state.bus_returnstatus.len = 0;

	mib_state.slave_state = kMIBProtocolError;
}

void bus_slave_setreturn(unsigned char status, volatile MIBParameterHeader *value)
{
	mib_state.bus_returnstatus.result = status;

	if (value == 0)
		mib_state.bus_returnstatus.len = 0;
	else
		mib_state.bus_returnstatus.len = value->len + sizeof(MIBParameterHeader);
}

/* call with 0 to get the header, > 0 to get the value */
static void bus_slave_receiveparam(MIBParameterHeader *param, uint8 header_or_value, unsigned char flag)
{
	if (header_or_value == 0)
	{
		bus_slave_receive((unsigned char *)&mib_state.last_param, sizeof(MIBParameterHeader), kCallbackBeforeChecksum|flag);
	}
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

		//Update the parameter with the read in size.  If this is a buffer
		//this is important so that we record the actual size, rather than the
		//size of the buffer we allocated for it.
		param->len = mib_state.last_param.len;

		if (param->type == kMIBInt16Type)
		{
			bus_slave_receive((unsigned char *)(&((MIBIntParameter*)param)->value), param->len, kCallbackBeforeChecksum|flag);
		}
		else
		{
			bus_slave_receive((unsigned char *)((MIBBufferParameter*)param)->data, param->len, kCallbackBeforeChecksum|flag);
		}
	}
}

static void bus_slave_searchcommand()
{
	uint8 index;
	if (i2c_slave_lasterror() != kI2CNoError)
	{
		bus_slave_seterror(kCommandChecksumError); //Make sure the parameter checksum was valid.
		return;
	}
	
	index = find_handler(mib_state.bus_command.feature, mib_state.bus_command.command);
	mib_state.slave_handler = get_handler(index);
	if (mib_state.slave_handler == NULL)
	{
		bus_slave_seterror(kUnsupportedCommand);
		return;
	}

	//Get the list of parameters that we have to receive
	mib_state.slave_params = (MIBParamList*)build_params(index);
	if (mib_state.slave_params == NULL)
	{
		//Give the slave handler a chance to allocate the parameters
		mib_state.slave_params = mib_state.slave_handler(NULL);
		if (mib_state.slave_params == NULL)
		{
			bus_slave_seterror(kParameterListNotBuilt);
			return;
		}
	}

	mib_state.slave_params->curr = 0;

	if (mib_state.slave_params->count > 0)
	{
		bus_slave_receiveparam((MIBParameterHeader*)&mib_state.slave_params[0], 0, 0);
		mib_state.slave_state = kMIBReceiveParameterValue;
	}
	else
	{
		//There we no parameters so jump straight to executing the command
		mib_state.slave_state = kMIBExecuteCommandHandler;
	}
}

static void bus_slave_callcommand()
{	
	if (mib_state.slave_handler != NULL && mib_state.slave_state == kMIBExecuteCommandHandler)
		mib_state.slave_handler(mib_state.slave_params);
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
	if (i2c_address_received())
	{
		if (i2c_slave_is_read())
		{
			mib_state.num_reads += 1;

			//Odd reads are for the return status
			//Even reads are for the return value
			if (mib_state.num_reads & 0x01)
			{
				/*
				 * To allow for bus error recovery, we keep resending the return status and return value (if any) on all
				 * subsequent reads until the master is satisfied that it has received one with a valid checksum.
				 */

				if (mib_state.num_reads == 1)
					bus_slave_callcommand();

				//slave callback should set the return status via bus_slave_setreturn and set mib_buffer to point to the return value if any
				bus_slave_send((unsigned char*)&mib_state.bus_returnstatus, sizeof(MIBReturnValueHeader), kSendImmediately);

				//If we don't expect to send a return value, finish the command after this transmission
				if (mib_state.bus_returnstatus.len == 0)
					mib_state.slave_state = kMIBFinishCommand;
			}
			else
			{
				//even reads are for the return value, which if there is one should be in the mib_buffer
				//if there wasn't one, then this is a protocol error (or a failed return status checksum) 
				//since there shouldn't have been a second read so we can send garbage since it will be 
				//ignored
				if (mib_state.bus_returnstatus.len != 0)
				{
					bus_slave_send((unsigned char *)mib_buffer, mib_state.bus_returnstatus.len, kSendImmediately);
				}
				else
				{
					bus_slave_send((unsigned char *)mib_buffer, 1, kSendImmediately); //protocol error so just send 1 byte, doesn't matter
				}
				
				mib_state.slave_state = kMIBFinishCommand;

				//Make sure we can never overflow
				if (mib_state.num_reads == 10)
					mib_state.num_reads = 4; 
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
		bus_slave_receiveparam(mib_state.slave_params->params[mib_state.slave_params->curr], 1, kContinueChecksum);
		mib_state.slave_params->curr += 1;
		if (mib_state.slave_params->curr == mib_state.slave_params->count)
			mib_state.slave_state = kMIBFinishedReceivingParameters;
		else
			mib_state.slave_state = kMIBReceiveParameterHeader;
		break;

		case kMIBReceiveParameterHeader:
		bus_slave_receiveparam(mib_state.slave_params->params[mib_state.slave_params->curr], 0, kContinueChecksum);
		mib_state.slave_state = kMIBReceiveParameterValue;
		break;

		case kMIBFinishedReceivingParameters:
		mib_state.slave_state = kMIBReceivedParameterChecksum;
		break;

		case kMIBReceivedParameterChecksum:
		mib_state.slave_state = kMIBExecuteCommandHandler;
		if (i2c_slave_lasterror() != kI2CNoError)
			bus_slave_seterror(kParameterChecksumError); //Make sure the parameter checksum was valid.
		break;

		case kMIBFinishCommand:
		case kMIBExecuteCommandHandler:
		case kMIBIdleState:
		case kMIBProtocolError:
		break;
	}

	i2c_release_clock();
}
