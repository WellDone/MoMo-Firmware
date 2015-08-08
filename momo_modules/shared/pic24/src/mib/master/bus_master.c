#include "bus_master.h"
#include "bus_slave.h"
#include <string.h>
#include "mib_state.h"
#include "rpc_queue.h"
#include "system_log.h"
#include "log_definitions.h"

//Local Prototypes that should not be called outside of this file
static void 	bus_master_finish(bool send_stop);
static void		bus_master_localrpc(void *arg);
void 			bus_master_handleerror();
void 			bus_master_sendrpc();
void 			bus_master_readresult(unsigned int length);
void 			bus_master_rpc_async_do();
void 			bus_master_queue_async_rpc(uint8_t sender, mib_rpc_function callback, void *state);
void 			bus_master_init_async();


const rpc_info *master_rpcdata;

async_rpc_data	async_rpcs[kMaxAsyncRPCNUmber];

void bus_master_init_async()
{
	unsigned int i=0;

	for(i=0; i<kMaxAsyncRPCNUmber; ++i)
	{
		async_rpcs[i].sender = 0;
		async_rpcs[i].flags = 0;
		async_rpcs[i].callback = NULL;
		async_rpcs[i].state = NULL;
	}
}

/*
 * Place this asynchronous RPC in the queue in the first open spot
 */
void bus_master_queue_async_rpc(uint8_t sender, mib_rpc_function callback, void *state)
{
	unsigned int i=0;

	for (i=0; i<kMaxAsyncRPCNUmber; ++i)
	{
		if (async_rpcs[i].sender == 0)
		{
			async_rpcs[i].sender = sender;
			async_rpcs[i].callback = callback;
			async_rpcs[i].state = state;
			break;
		}
	}

	//If the queue was full then there's nothing we can do.
	if (i == kMaxAsyncRPCNUmber)
	{
		LOG_CRITICAL(kAsyncRPCQueueFullError);
		LOG_INT(sender);
	}
}

void bus_master_finish_async_rpc(uint8_t sender)
{
	unsigned int i=0;

	for (i=0; i<kMaxAsyncRPCNUmber; ++i)
	{
		if (async_rpcs[i].sender == sender)
		{
			if (async_rpcs[i].callback != NULL)
			{
				uint8_t status = kNoErrorStatus;
				mib_unified.packet.response.length = mib_unified.packet.call.length;

				if (mib_unified.packet.response.length > 0)
					status = kNoErrorWithDataStatus;

				mib_unified.packet.response.status_value = status;

				async_rpcs[i].callback(status, async_rpcs[i].state);
			}

			//Free up this spot in the async RPC queue
			async_rpcs[i].sender = 0;
			async_rpcs[i].callback = NULL;
			break;
		}
	}

	//Send an error if we couldn't find a record of the calling RPC
	if (i == kMaxAsyncRPCNUmber)
	{
		LOG_CRITICAL(kCouldNotFindRPCError);
		LOG_INT(sender);
	}
}

rtcc_timestamp rpc_start_time;

unsigned int bus_master_idle()
{
	return mib_state.rpc_done;
}

static void bus_master_finish(bool send_stop)
{
	unsigned int i=0;

	//Set the flag that this RPC is done for whomever is waiting.
	mib_state.rpc_done = 1;
	rpc_start_time = 0;

	if ( !rpc_queue_empty() )
			taskloop_add_critical( bus_master_rpc_async_do, NULL );

	if (send_stop)
	{
		i2c_finish_transmission(); 
		for(i=0; i<200; ++i)
			;
	}

	if (mib_unified.packet.response.status_value == kAsynchronousResponseStatus)
		bus_master_queue_async_rpc(master_rpcdata->data.address, mib_state.master_callback, master_rpcdata->state);
	else if (mib_state.master_callback != NULL)
		mib_state.master_callback(mib_unified.packet.response.status_value, master_rpcdata->state);
}

void bus_master_init()
{
	mib_state.rpc_done = 1;
	rpc_start_time = 0;
	rpc_queue_init();
	bus_master_init_async();
}

void bus_master_rpc_async_do( void* arg )
{
	master_rpcdata = rpc_queue_peek();
	mib_state.master_callback = master_rpcdata->callback;

	bus_master_sendrpc(NULL);
	rpc_dequeue(NULL);
}

void bus_master_rpc_async(mib_rpc_function callback, MIBUnified *data, void *state)
{
	data->packet.call.sender = mib_state.my_address;
	data->packet.call.flags_and_length &= 0b00011111; //Make sure the flags are cleared out

	bus_append_checksum((unsigned char*)&(data->packet), kMIBMessageNoChecksumSize);	
	rpc_queue(callback, data, state);

	if (mib_state.rpc_done)
		bus_master_rpc_async_do(NULL);
}

/*
 * Send or resend the rpc call currently pointed to .  
 */

void bus_master_localrpc(void *arg)
{
	memcpy((char *)&mib_unified.packet, (char*)&master_rpcdata->data.packet, kMIBMessageSize);
	
	//Attempt to call the local handler
	bus_slave_set_returnbuffer_length(0);
	mib_state.slave_handler = find_handler();

	if (mib_state.slave_handler == kInvalidMIBIndex)
	{
		mib_unified.packet.response.length = 0;
		bus_slave_setreturn(kCommandNotFoundStatus); //Make sure that if nothing else happens we return an error status.
	}
	else
		bus_slave_callcommand();

	bus_append_checksum((unsigned char*)&(mib_unified.packet), kMIBMessageNoChecksumSize);
	bus_master_finish(0);
}

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

	//If we're sending this packet to ourselves, just execute the callback
	//We can't execute this here because the call to bus_master_finish can requeue the RPC.
	if (master_rpcdata->data.address == mib_state.my_address)
		taskloop_add_critical(bus_master_localrpc, NULL);
	else
	{
		rpc_start_time = rtcc_get_timestamp();
		i2c_master_enable();
		set_master_state(kMIBReadReturnStatus);
		bus_send(master_rpcdata->data.address, (unsigned char *)&(master_rpcdata->data.packet), kMIBMessageNoChecksumSize);
	}
}

void bus_master_readresult(unsigned int length)
{
	bus_receive(master_rpcdata->data.address, (unsigned char *)&mib_unified.packet, length);
}

void bus_master_handleerror()
{
	unsigned int i;
	
	switch(mib_unified.packet.response.status_value)
	{
		case kChecksumMismatchStatus:
		i2c_finish_transmission(); 
		for(i=0; i<200; ++i)
			;

		bus_master_sendrpc(master_rpcdata->data.address);
		break;

		default:
		bus_master_finish(1);
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
			if (mib_unified.packet.response.status_value == 0xFF)
			{
				bus_master_finish(1);
				break;
			}

			//If the slave sent something, try again to read the status.
			bus_master_readresult(1);
		}
		else if (status_is_error(mib_unified.packet.response.status_value))
			bus_master_handleerror();
		else if (packet_has_data(mib_unified.packet.response.status_value))
		{
			bus_master_readresult(kMIBMessageNoChecksumSize);
			set_master_state(kMIBExecuteCallback);
		}
		else
			bus_master_finish(1);
		
		break;

		case kMIBExecuteCallback:
		if (i2c_master_lasterror() != kI2CNoError)
			bus_master_readresult(kMIBMessageNoChecksumSize); //Reread the return status and return value since there was a checksum error
		else
			bus_master_finish(1);
		break;
	}
}

rtcc_timestamp bus_master_rpc_start_timestamp()
{
	return rpc_start_time;
}
