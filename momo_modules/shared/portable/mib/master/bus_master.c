#include "bus_master.h"
#include <string.h>
#include "mib_state.h"

#ifndef _PIC12
#include "rpc_queue.h"
#endif

#ifdef _PIC12
extern bank1 volatile I2CStatus i2c_status;
#endif

//Local Prototypes that should not be called outside of this file
unsigned char 	bus_master_lastaddress();
void 			bus_master_finish(uint8 next);
void 			bus_master_compose_params(uint8 spec);
void 			bus_master_handleerror();
void 			bus_master_sendrpc(unsigned char address);
void 			bus_master_readstatus();

unsigned char bus_master_lastaddress()
{
	return mib_state.bus_msg.address >> 1;
}

void bus_master_finish(uint8 next)
{
	bus_send(bus_master_lastaddress(), (unsigned char *)mib_buffer, 1);
	set_master_state(next);
}

#ifndef _PIC12
void bus_master_init()
{
	mib_state.rpc_done = 1;
	rpc_queue_init();
}

void bus_master_rpc_async_do()
{
	const rpc_info* info = rpc_queue_peek();
	mib_state.bus_command.feature = info->feature;
	mib_state.bus_command.command = info->cmd;
	mib_state.bus_command.param_spec = info->param_spec;
	
	mib_state.master_callback = info->callback;

	bus_master_sendrpc(info->address);
	rpc_dequeue(NULL);
}

void bus_master_rpc_async(mib_rpc_function callback, unsigned char address, unsigned char feature, unsigned char cmd, uint8 spec)
{
	rpc_queue( callback, address, feature, cmd, spec );

	if ( mib_state.rpc_done ) { // Check if we're currently executing an RPC
		bus_master_rpc_async_do();
	}
}

#else

uint8 bus_master_rpc_sync(unsigned char address)
{
	//Multimaster support, wait until the bus is idle before starting an RPC call
	wait_and_start:
	while (!bus_is_idle())
		;

	bus_master_sendrpc(address);

	while(mib_state.rpc_done != 1)
	{
		if (i2c_status.state == kI2CUserCallbackState)
		{
			//If we collided, wait for the bus to become clear and start again.
			if (i2c_status.last_error == kI2CCollision)
			{
				i2c_status.last_error = kI2CNoError;
				goto wait_and_start;
			}

			bus_master_callback();
		}
	}

	return mib_state.bus_returnstatus.result;
}

#endif

/*
 * Send or resend the rpc call currently stored in mib_state.  
 */

void bus_master_sendrpc(unsigned char address)
{
	i2c_master_enable();

	mib_state.rpc_done = 0;

	if (plist_param_length(mib_state.bus_command.param_spec) > 0)
	{	
		set_master_state(kMIBSendParameters);
	}
	else
	{
		set_master_state(kMIBReadReturnStatus);
	}

	bus_send(address, (unsigned char *)&mib_state.bus_command, sizeof(MIBCommandPacket));
}

void bus_master_readstatus()
{
	bus_receive(bus_master_lastaddress(), (unsigned char *)&mib_state.bus_returnstatus, sizeof(MIBReturnValueHeader));
	set_master_state(kMIBReadReturnValue);
}

void bus_master_handleerror()
{
	switch(mib_state.bus_returnstatus.result)
	{
		case kCommandChecksumError:
		case kParameterChecksumError:
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
	switch(mib_state.master_state)
	{
		case kMIBSendParameters:
		bus_send(bus_master_lastaddress(), (unsigned char*)mib_buffer, plist_param_length(mib_state.bus_command.param_spec));
		set_master_state(kMIBReadReturnStatus);
		break;

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
			bus_receive(bus_master_lastaddress(), (unsigned char *)&mib_state.bus_returnstatus, 1);
			set_master_state(kMIBReadReturnStatus);
		}
		else if (mib_state.bus_returnstatus.result != kNoMIBError)
			bus_master_handleerror();
		else
		{
			if (mib_state.bus_returnstatus.len > 0)
			{
				bus_receive(bus_master_lastaddress(), (unsigned char *)mib_buffer, mib_state.bus_returnstatus.len);
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
		bus_master_sendrpc(bus_master_lastaddress());
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

		#ifndef _PIC12
		if ( !rpc_queue_empty() )
				taskloop_add_critical( bus_master_rpc_async_do );
		#endif

		i2c_finish_transmission(); 
		for(i=0; i<200; ++i)
			;			
		
		#ifndef _PIC12
		if (mib_state.master_callback != NULL)
			mib_state.master_callback(mib_state.bus_returnstatus.result);
		#else

		#endif
		break;
	}
}
