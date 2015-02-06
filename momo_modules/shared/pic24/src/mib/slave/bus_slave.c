#include "bus_slave.h"
#include <string.h>

//static prototypes that are only to be used in this file
static void bus_slave_startcommand();
static void bus_slave_searchcommand();
static void bus_slave_callcommand();

/*
 * MIB Slave Logic 
 */

/*
 * Set the return status, the high order 3 bits define the status, the low order 5 bits set the 
 * length of the return value
 */
void bus_slave_setreturn(uint8 status)
{
	mib_unified.bus_returnstatus.return_status = status;
}

inline void bus_slave_set_returnbuffer_length(uint8 length) 
{
	bus_slave_setreturn(pack_return_status( kNoMIBError, length ));	
}

void bus_slave_return_buffer( const void* buff, uint8 length ) 
{
	if (length > kBusMaxMessageSize) 
	{
		bus_slave_seterror( kUnknownError ); //TODO: Better
		return;
	}
	memcpy( plist_get_buffer(0), buff, length );
	bus_slave_set_returnbuffer_length( length );
}

void bus_slave_return_int16( int16 val ) 
{
	plist_set_int16( 0, val );
	bus_slave_set_returnbuffer_length( kIntSize );
}

static void bus_slave_startcommand()
{
	//Initialize all the state
	mib_state.slave_handler = kInvalidMIBIndex;
	mib_state.first_read = 1;

	bus_slave_setreturn(kUnknownError); //Make sure that if nothing else happens we return an error status.
	bus_slave_receive((unsigned char *)&mib_unified.bus_command, sizeof(MIBCommandPacket) + kBusMaxMessageSize);
}

static void bus_slave_searchcommand()
{
	if (i2c_slave_lasterror() != kI2CNoError)
	{
		bus_slave_seterror(kChecksumError); //Make sure the parameter checksum was valid.
		return;
	}

	mib_state.slave_handler = find_handler();

	if (mib_state.slave_handler == kInvalidMIBIndex)
	{
		bus_slave_seterror(kUnsupportedCommand);
		return;
	}

	if (plist_param_length(mib_unified.bus_command.param_spec) > kBusMaxMessageSize)
	{
		bus_slave_seterror(kParameterTooLong);
		return;
	}
}

/*
 * @preconditions: mib_unified.mib_buffer is full of a packet of parameters or nothing
 * @return: 1 if the parameters are valid types and 0 otherwise
 * @side effects: sets mib slave error state appropriately
 */
static uint8 bus_slave_validateparams()
{
	if (!validate_param_spec(mib_state.slave_handler))
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
		{
			bus_slave_setreturn( pack_return_status( kNoMIBError, 0 ) );
			call_handler(mib_state.slave_handler);
		}
	}
}

void bus_slave_callback()
{
	if (i2c_address_received())
	{
		if (i2c_slave_is_read())
		{
			
			if (mib_state.first_read)
			{
				bus_slave_searchcommand();
				bus_slave_callcommand();

				bus_append_checksum((unsigned char*)&mib_unified.bus_returnstatus, 1);
				bus_append_checksum((unsigned char*)&mib_unified.bus_returnstatus, mib_unified.bus_returnstatus.len+2);
				mib_state.first_read = 0;
			}

			//Position the i2c message pointer to the start of the return status.  This is located right before the return_value
			//in memory, so we will send the return value, if there is one, automatically on subsequent reads.
			bus_slave_send(&mib_unified.bus_returnstatus.return_status, sizeof(MIBReturnValueHeader)+kBusMaxMessageSize+2);
		}
		else
			bus_slave_startcommand(); //A write always indicates a new command
	}

	//Always release the clock, the slave should never hold the clock forever
	i2c_release_clock();
}
