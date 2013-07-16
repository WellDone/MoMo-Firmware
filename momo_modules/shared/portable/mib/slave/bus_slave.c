#include "bus_slave.h"

//static prototypes that are only to be used in this file
static void bus_slave_startcommand();
static void bus_slave_searchcommand();
static void bus_slave_callcommand();

/*
 * MIB Slave Logic 
 */

static void bus_slave_startcommand()
{
	//Initialize all the state
	mib_state.slave_state = kMIBSearchCommand;
	mib_state.slave_handler = kInvalidMIBIndex;
	mib_state.num_reads = 0;

	bus_slave_setreturn(kUnknownError); //Make sure that if nothing else happens we return an error status.

	bus_slave_receive((unsigned char *)&mib_state.bus_command, sizeof(MIBCommandPacket), 0);
}

void bus_slave_seterror(unsigned char error)
{
	mib_state.bus_returnstatus.result = error;
	mib_state.bus_returnstatus.len = 0;

	mib_state.slave_state = kMIBProtocolError;
}

/*
 * The the return status.  The high order bit indicates if a return value is present in 
 * the mib_buffer with 1 being yes and 0 being no.
 */
void bus_slave_setreturn(uint8 status)
{
	mib_state.bus_returnstatus.result = status & 0x7F; //clear high order bit

	if (!BIT_TEST(status,7))
		mib_state.bus_returnstatus.len = 0;
	else
		mib_state.bus_returnstatus.len = ((MIBParameterHeader*)mib_buffer)->len + sizeof(MIBParameterHeader);
}

static void bus_slave_searchcommand()
{
	if (i2c_slave_lasterror() != kI2CNoError)
	{
		bus_slave_seterror(kCommandChecksumError); //Make sure the parameter checksum was valid.
		return;
	}
	
	mib_state.slave_handler = find_handler();
	
	if (mib_state.slave_handler == kInvalidMIBIndex)
	{
		bus_slave_seterror(kUnsupportedCommand);
		return;
	}

	if (mib_state.bus_command.param_length > kBusMaxMessageSize)
	{
		bus_slave_seterror(kParameterTooLong);
		return;
	}

	//initialize i2c to receive parameters if there are any
	if (mib_state.bus_command.param_length > 0)
		bus_slave_receive(mib_buffer, mib_state.bus_command.param_length, 0);
	
	mib_state.slave_state = kMIBFinishCommand;
}

/*
 * @preconditions: mib_buffer is full of a packet of parameters or nothing
 * @return: 1 if the parameters are valid types and 0 otherwise
 * @side effects: sets mib slave error state appropriately
 */
static uint8 bus_slave_validateparams()
{
	if (i2c_slave_lasterror() != kI2CNoError)
	{
		bus_slave_seterror(kParameterChecksumError); //Make sure the parameter checksum was valid.
		return 0;
	}

	if (!validate_params(mib_state.slave_handler))
	{
		bus_slave_seterror(kWrongParameterType); //Make sure the parameter checksum was valid.
		return 0;
	}

	return 1;
}

static void bus_slave_callcommand()
{	
	if (mib_state.slave_handler != kInvalidMIBIndex)
	{
		if (bus_slave_validateparams())
			call_handler(mib_state.slave_handler);
	}
}

void bus_slave_reset()
{
	//A write after any number of reads is a protocol reset so we reset ourselves back to idle
	//This is a failsafe to make sure we can't get into a state where the slave locks up
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
				//Just toggle back and forth between 1, 2 and 3. Clear the slave handler though so
				//that we don't recall the function
				if (mib_state.num_reads == 3)
				{
					mib_state.slave_handler = kInvalidMIBIndex;
					mib_state.num_reads = 1; 
				}
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
			{
				bus_slave_startcommand(); //A write when we're idle indicates a new command
			}
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

		default:
		break;
	}

	i2c_release_clock();
}
