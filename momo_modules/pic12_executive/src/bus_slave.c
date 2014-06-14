#include "bus_slave.h"
#include "bootloader.h"
#include <string.h>

extern bank1 __persistent MIBExecutiveStatus status;

//Internal functions used only in this file. file.
void bus_slave_callcommand();

/*
 * MIB Slave Logic 
 */

void bus_slave_startcommand()
{
	//Initialize all the state
	status.first_read = 1;

	i2c_init_buffer(kCommandOffset);	//Initialize buffer to read in command + parameters
	i2c_release_clock();
}

void bus_slave_read_callback()
{
	if (status.first_read)
	{
		bus_slave_callcommand();
		status.first_read = 0;

		//Prepare the return value response
		i2c_init_buffer(kReturnStatusOffset);
		i2c_append_checksum_at_offset(1);
		
		i2c_init_buffer(kReturnStatusOffset);
		i2c_append_checksum_at_offset(bus_retval_size() + 2);	//Second checksum includes return status + first checksum byte + return value
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