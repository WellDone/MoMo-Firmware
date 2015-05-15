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
 * Set the return status
 */

void bus_slave_setreturn(uint8 status)
{
	mib_unified.packet.response.status_value = status;
	mib_unified.packet.response.status_checksum = (~status) + 1;
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
	bus_slave_receive((unsigned char *)&mib_unified.packet, kMIBMessageSize);
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

static void bus_slave_callcommand()
{	
	if (mib_state.slave_handler != kInvalidMIBIndex)
	{
		bus_slave_setreturn( pack_return_status( kNoMIBError, 0 ) );
		call_handler(mib_state.slave_handler);
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
			bus_slave_send((uint8_t*) &mib_unified.packet, kMIBMessageSize);
		}
		else
			bus_slave_startcommand(); //A write always indicates a new command
	}

	//Always release the clock, the slave should never hold the clock forever
	i2c_release_clock();
}
