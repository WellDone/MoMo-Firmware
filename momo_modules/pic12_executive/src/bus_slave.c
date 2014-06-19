#include "bus_slave.h"
#include "bootloader.h"
#include <string.h>

extern bank1 __persistent MIBExecutiveStatus status;

//static prototypes that are only to be used in this file

static void bus_slave_searchcommand();
static void bus_slave_callcommand();

/*
 * MIB Slave Logic 
 */

void bus_slave_startcommand()
{
	//Initialize all the state
	mib_state.slave_handler = kInvalidMIBIndex;
	status.first_read = 1;

	i2c_init_buffer(kCommandOffset);	//Initialize buffer to read in command + parameters
	i2c_release_clock();
}

static void bus_slave_searchcommand()
{
	if (i2c_calculate_checksum() != 0)
	{
		bus_slave_setreturn(pack_return_status(kChecksumError, 0)); //Make sure the parameter checksum was valid.
		return;
	}

	if (bus_slave_checkparamsize() == 0)
	{
		bus_slave_setreturn(pack_return_status(kParameterTooLong,0));
		return;
	}

	mib_state.slave_handler = find_handler();
	if (mib_state.slave_handler == kInvalidMIBIndex)
	{
		bus_slave_setreturn(pack_return_status(kUnsupportedCommand,0));
		return;
	}
}

/*
 * @preconditions: mib_buffer is full of a packet of parameters or nothing
 * @return: 1 if the parameters are valid types and 0 otherwise
 * @side effects: sets mib slave error state appropriately
 */
static uint8 bus_slave_validateparams()
{
	if (!validate_param_spec())
	{
		bus_slave_setreturn(pack_return_status(kWrongParameterType, 0)); //Make sure the parameter checksum was valid.
		return 0;
	}

	return 1;
}

static void bus_slave_callcommand()
{	
	if (mib_state.slave_handler != kInvalidMIBIndex)
	{
		if (bus_slave_validateparams()) {
			bus_slave_setreturn( pack_return_status( kNoMIBError, 0 ) );
			call_handler();
		}
	}
}

void bus_slave_read_callback()
{
	if (status.first_read)
	{
		bus_slave_searchcommand();
		bus_slave_callcommand();
		status.first_read = 0;

		//Prepare the return value response
		i2c_init_buffer(kReturnStatusOffset);
		i2c_setoffset(1);
		i2c_append_checksum();
		
		i2c_init_buffer(kReturnStatusOffset);
		i2c_setoffset(bus_retval_size() + 2);	//Second checksum includes return status + first checksum byte + return value
		i2c_append_checksum();
	}

	/*
	 * To allow for bus error recovery, we keep resending the return status and return value (if any) on all
	 * subsequent reads until the master is satisfied that it has received one with a valid checksum.  If you
	 * just read the return status, it will have a valid checksum and if you read the return value as well it 
	 * will also have a valid checksum.
	 */

	i2c_init_buffer(kReturnStatusOffset); //Set up buffer pointer to point to return status + return value

	i2c_loadbuffer();
	i2c_write(); //Initialize first byte for immediate reading
	i2c_release_clock();
}