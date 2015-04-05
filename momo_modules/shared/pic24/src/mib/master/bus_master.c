#include "bus_master.h"
#include <string.h>
#include "mib_state.h"
#include "rpc_queue.h"

//Local Prototypes that should not be called outside of this file
static void		bus_master_finish();
void 			bus_master_handleerror();
void 			bus_master_sendrpc();
void 			bus_master_readresult(unsigned int length);
void 			bus_master_rpc_async_do();

const rpc_info *master_rpcdata;

rtcc_timestamp rpc_start_time;

unsigned int bus_master_idle()
{
	return mib_state.rpc_done;
}

static void bus_master_finish()
{
	unsigned int i=0;

	//Set the flag that this RPC is done for whomever is waiting.
	mib_state.rpc_done = 1;
	rpc_start_time = 0;

	if ( !rpc_queue_empty() )
			taskloop_add_critical( bus_master_rpc_async_do, NULL );

	i2c_finish_transmission(); 
	for(i=0; i<200; ++i)
		;			
	
	if (mib_state.master_callback != NULL)
		mib_state.master_callback( mib_unified.bus_returnstatus.result );
}

void bus_master_init()
{
	mib_state.rpc_done = 1;
	rpc_start_time = 0;
	rpc_queue_init();
}

void bus_master_rpc_async_do( void* arg )
{
	master_rpcdata = rpc_queue_peek();
	mib_state.master_callback = master_rpcdata->callback;

	bus_master_sendrpc(NULL);
	rpc_dequeue(NULL);
}

void bus_master_rpc_async(mib_rpc_function callback, MIBUnified *data)
{
	bus_append_checksum((unsigned char*)&(data->bus_command), sizeof(MIBCommandPacket)+plist_param_length(data->bus_command.param_spec));	
	rpc_queue(callback, data);

	if (mib_state.rpc_done)
		bus_master_rpc_async_do( NULL );
}

/*
 * Send or resend the rpc call currently pointed to .  
 */

void bus_master_sendrpc()
{
	//Always clear the rpc_done flag so that we don't repeatedly call sendrpc if someone else does
	//bus_master_rpc_async before the bus becomes idle. 
	mib_state.rpc_done = 0;

	//If the bus is not idle do not start an RPC, try later
	if (!bus_is_idle())
	{
		taskloop_add_critical(bus_master_sendrpc, NULL);
		return;
	}

	rpc_start_time = rtcc_get_timestamp();
	i2c_master_enable();

	set_master_state(kMIBReadReturnStatus);
	bus_send(master_rpcdata->data.address, (unsigned char *)&(master_rpcdata->data.bus_command), sizeof(MIBCommandPacket)+plist_param_length(master_rpcdata->data.bus_command.param_spec));
}

void bus_master_readresult(unsigned int length)
{
	bus_receive(master_rpcdata->data.address, (unsigned char *)&mib_unified.bus_returnstatus, length);
}

void bus_master_handleerror()
{
	switch(mib_unified.bus_returnstatus.result)
	{
		case kChecksumError:
		bus_master_sendrpc(master_rpcdata->data.address);
		break;

		default:
		bus_master_finish();
		break;
	}
}

void bus_master_callback()
{
	if (i2c_master_lasterror() == kI2CCollision)
	{
		taskloop_add_critical(bus_master_sendrpc, NULL);
		return;
	}
	
	switch(mib_state.master_state)
	{
		case kMIBReadReturnStatus:
		bus_master_readresult(1);
		set_master_state(kMIBReadReturnValue);
		break;

		case kMIBReadReturnValue:
		if (i2c_master_lasterror() != kI2CNoError)
		{
			//There was a checksum error reading the return status
			//Keep trying to read it until we don't get a checksum error, unless the slave is just gone

			//Check if we received all 0xFF bytes indicating the slave is not there
			if (mib_unified.bus_returnstatus.return_status == 0xFF)
			{
				bus_master_finish();
				break;
			}

			//If the slave sent something, try again to read the status.
			bus_master_readresult(1);
		}
		else if (mib_unified.bus_returnstatus.result != kNoMIBError)
			bus_master_handleerror();
		else if (mib_unified.bus_returnstatus.len > 0)
		{
			bus_master_readresult(mib_unified.bus_returnstatus.len+2);
			set_master_state(kMIBExecuteCallback);
		}
		else
			bus_master_finish();
		
		break;

		case kMIBExecuteCallback:
		if (i2c_master_lasterror() != kI2CNoError)
			bus_master_readresult(mib_unified.bus_returnstatus.len+2); //Reread the return status and return value since there was a checksum error
		else
			bus_master_finish();
		break;
	}
}

rtcc_timestamp bus_master_rpc_start_timestamp()
{
	return rpc_start_time;
}
