#include "bus_slave.h"
#include "bootloader.h"
#include <string.h>
#include "executive_state.h"

//Internal functions used only in this file. file.
void bus_slave_callcommand();

/*
 * MIB Slave Logic 
 */

void bus_slave_startcommand()
{
	/*
	 * If we are currently processing a previous MIB call asynchronously and we happened
	 * to receive another call in the meanwhile, we need to respond that we are busy.
	 * We can't set the busy flag directly in bus_slave_callhandler because we can't set the
	 * busy flag until *after* we finish responding to the call that triggered our asynchronous
	 * response.
	 */
	if (status.async_callback)
		status.respond_busy = true;
	else
	{
		//Initialize all the state
		status.first_read = 1;

		i2c_init_location();	//Initialize buffer to read in command + parameters
	}

	i2c_release_clock();
}

void bus_slave_read_callback()
{
	if (status.first_read)
	{
		bus_slave_callcommand();
		status.first_read = 0;

		i2c_append_checksum();
	}

	/*
	 * To allow for bus error recovery, we keep resending the return status and return value (if any) on all
	 * subsequent reads until the master is satisfied that it has received one with a valid checksum.  If you
	 * just read the return status, it will have a valid checksum and if you read the return value as well it 
	 * will also have a valid checksum.
	 */

	//Set up buffer pointer to point to return status + return value
	i2c_init_location(); 
	i2c_loadbuffer();
	i2c_write(); //Initialize first byte for immediate reading
	
	i2c_release_clock();
}