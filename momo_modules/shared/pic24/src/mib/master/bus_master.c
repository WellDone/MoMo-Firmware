#include "bus_master.h"
#include <string.h>
#include "mib_state.h"
#include "rpc_queue.h"

//Local Prototypes that should not be called outside of this file
void 			bus_master_finish(uint8 next);
void 			bus_master_handleerror();
void 			bus_master_sendrpc();
void 			bus_master_readstatus();

const rpc_info *master_rpcdata;

void bus_master_finish(uint8 next)
{
	bus_send(master_rpcdata->data.address, (unsigned char *)mib_unified.mib_buffer, 1);
	set_master_state(next);
}

void bus_master_init()
{
	mib_state.rpc_done = 1;
	rpc_queue_init();
}

void bus_master_rpc_async_do()
{
	master_rpcdata = rpc_queue_peek();
	mib_state.master_callback = master_rpcdata->callback;

	bus_master_sendrpc();
	rpc_dequeue(NULL);
}

void bus_master_rpc_async(mib_rpc_function callback, const MIBUnified *data)
{
	rpc_queue(callback, data);

	if (mib_state.rpc_done)
		bus_master_rpc_async_do();
}

/*
 * Send or resend the rpc call currently pointed to .  
 */

void bus_master_sendrpc()
{
	//If the bus is not idle do not start an RPC, try later
	if (!bus_is_idle())
	{
		taskloop_add_critical(bus_master_sendrpc);
		return;
	}

	i2c_master_enable();

	mib_state.rpc_done = 0;
	set_master_state(kMIBReadReturnStatus);
	bus_send(master_rpcdata->data.address, (unsigned char *)&(master_rpcdata->data.bus_command), sizeof(MIBCommandPacket)+plist_param_length(master_rpcdata->data.bus_command.param_spec));
}

void bus_master_readstatus()
{
	bus_receive(master_rpcdata->data.address, (unsigned char *)&mib_state.bus_returnstatus, sizeof(MIBReturnValueHeader));
	set_master_state(kMIBReadReturnValue);
}

void bus_master_handleerror()
{
	switch(mib_state.bus_returnstatus.result)
	{
		case kChecksumError:
		bus_master_finish(kMIBResendCommand);
		break;

		default:
		bus_master_finish(kMIBFinalizeMessage);
		break;
	}
}

void bus_master_callback()
{
	int i;

	if (i2c_master_lasterror() == kI2CCollision)
	{
		taskloop_add_critical(bus_master_sendrpc);
		return;
	}
	
	switch(mib_state.master_state)
	{
		case kMIBReadReturnStatus:
		bus_master_readstatus();
		break;

		case kMIBReadReturnValue:
		if (i2c_master_lasterror() != kI2CNoError)
		{
			//There was a checksum error reading the return status
			//Keep trying to read it until we don't get a checksum error.  The slave may be sending a return value, so issue
			//one read to clear that and the slave will resend the return status for all odd reads.

			//Check if we received all 0xFF bytes indicating the slave is not there
			if (mib_state.bus_returnstatus.return_status == 0xFF)
			{
				bus_master_finish(kMIBFinalizeMessage);
				break;
			}

			//This is discarded, but we need to issue a read in case the slave is sending us a return value
			bus_receive(master_rpcdata->data.address, (unsigned char *)&mib_state.bus_returnstatus, 1);
			set_master_state(kMIBReadReturnStatus);
		}
		else if (mib_state.bus_returnstatus.result != kNoMIBError)
			bus_master_handleerror();
		else
		{
			if (mib_state.bus_returnstatus.len > 0)
			{
				bus_receive(master_rpcdata->data.address, (unsigned char *)mib_unified.mib_buffer, mib_state.bus_returnstatus.len);
				set_master_state(kMIBExecuteCallback);
			}
			else
			{
				//void return value, just call the callback
				bus_master_finish(kMIBFinalizeMessage);
			}
		}
		break;

		case kMIBResendCommand:
		bus_master_sendrpc(master_rpcdata->data.address);
		break;

		case kMIBExecuteCallback:
		if (i2c_master_lasterror() != kI2CNoError)
			bus_master_readstatus(); //Reread the return status and return value since there was a checksum error
		else
			bus_master_finish(kMIBFinalizeMessage);
		break;

		case kMIBFinalizeMessage:
		//Set the flag that this RPC is done for whomever is waiting.
		mib_state.rpc_done = 1;

		if ( !rpc_queue_empty() )
				taskloop_add_critical( bus_master_rpc_async_do );

		i2c_finish_transmission(); 
		for(i=0; i<200; ++i)
			;			
		
		if (mib_state.master_callback != NULL)
			mib_state.master_callback(mib_state.bus_returnstatus.result);

		break;
	}
}
