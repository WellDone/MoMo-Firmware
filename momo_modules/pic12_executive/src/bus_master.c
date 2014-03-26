#include "bus_master.h"
#include <string.h>
#include "mib_state.h"
#include "bootloader.h"

extern bank1 __persistent MIBExecutiveStatus status;

//Local Prototypes that should not be called outside of this file
uint8 bus_master_tryrpc();

/*uint8 bus_master_send(uint8 length)				
{
	mib_state.curr_loc = mib_state.buffer_start + length;
	i2c_append_checksum();

	mib_state.buffer_end = mib_state.curr_loc;
	mib_state.curr_loc = mib_state.buffer_start;
	return i2c_master_send_message();						
}

uint8 bus_master_receive(uint8 length)			
{													
	mib_state.buffer_end = mib_state.buffer_start + length;
	return i2c_master_receive_message();					
}*/

void bus_master_begin_rpc()
{
	//Wait until the bus is idle to make sure that we don't trample
	//a currently happening slave call
	while (!bus_is_idle())
		;

	//Take control of the bus from the slave so that we can use the
	//shared buffers for master storage
	i2c_master_enable();
}

uint8 bus_master_send_rpc(unsigned char address)
{
	mib_state.send_address = address << 1;
	mib_state.save_command 	= mib_data.bus_command.command;
	mib_state.save_spec		= mib_data.bus_command.param_spec;

	//Multimaster support, wait until the bus is idle before starting an RPC call
	wait_and_start:
	BCL1IF = 0;
	
	while (!bus_is_idle())
		;

	if (bus_master_tryrpc() == 0)
	{
		i2c_finish_transmission();
		goto wait_and_start;
	}

	i2c_finish_transmission();
	i2c_set_master_mode(0);

	return mib_data.return_status.bus_returnstatus.result;
}

/*
 * Send or resend the rpc call currently stored in mib_state.  
 */

uint8 bus_master_tryrpc()
{
	//Copy the command spec back in because they would have been overwritten if we're
	//retrying a call after getting a checksum error from the slave.
	mib_data.bus_command.command = mib_state.save_command;
	mib_data.bus_command.param_spec = mib_state.save_spec;

	i2c_init_buffer(kCommandOffset);
	if (bus_master_send(sizeof(MIBCommandPacket) + plist_param_length()) == 1)	//Do not include 1 byte of room for checksum since that will be appended
		return 0;

	while (true)
	{
		i2c_init_buffer(kReturnStatusOffset);
		if (bus_master_receive(sizeof(MIBReturnValueHeader) + 1) == 1)
			return 0;

		if (i2c_calculate_checksum() != 0)
		{
			if (mib_data.return_status.bus_returnstatus.return_status == 0xFF)
				break;
			
			continue;
		}

		//At this point we know that we read a valid return status, check what it tells us

		//If there was a checksum error on the command packet
		if (mib_data.return_status.bus_returnstatus.result == kChecksumError)
			return 0;

		//If there was a return value, read it in.
		//Note that for simplicity of implementation, we also read in the return status again.
		//The slave sends the same information on every read.
		if (bus_retval_size() > 0)
		{
			i2c_init_buffer(kReturnStatusOffset);
			if (bus_master_receive(bus_retval_size() + 1 + sizeof(MIBReturnValueHeader) + 1) == 1)
				return 0;

			if (i2c_calculate_checksum() != 0)
				continue;
		}

		//If we got all the way through the loop, the command was successful
		break;
	}

	return 1;
}