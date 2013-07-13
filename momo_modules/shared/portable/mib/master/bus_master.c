#include "bus_master.h"
#include <string.h>

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
	bus_send(bus_master_lastaddress(), (unsigned char *)mib_buffer, 1, 0);
	mib_state.master_state = next;
}

void bus_master_compose_params(uint8 spec)
{
	mib_state.bus_command.param_length = loadparams(spec);
}

void bus_master_rpc(mib_rpc_function callback, unsigned char address, unsigned char feature, unsigned char cmd)
{
	mib_state.bus_command.feature = feature;
	mib_state.bus_command.command = cmd;
	mib_state.master_callback = callback;

	bus_master_sendrpc(address);
}

/*
 * Send or resend the rpc call currently stored in mib_state.  
 */

void bus_master_sendrpc(unsigned char address)
{
	i2c_master_enable();

	if (mib_state.bus_command.param_length > 0)
		mib_state.master_state = kMIBSendParameters;
	else
		mib_state.master_state = kMIBReadReturnStatus;

	bus_send(address, (unsigned char *)&mib_state.bus_command, sizeof(MIBCommandPacket), 0);
}

/*
 * Send a special rpc value to get the slave to resend its call execution status and return value (if any)
 */

void bus_master_readstatus()
{
	bus_receive(bus_master_lastaddress(), (unsigned char *)&mib_state.bus_returnstatus, sizeof(MIBReturnValueHeader), 0);
	mib_state.master_state = kMIBReadReturnValue;
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
	switch(mib_state.master_state)
	{
		case kMIBSendParameters:
		bus_send(bus_master_lastaddress(), (unsigned char*)mib_buffer, mib_state.bus_command.param_length, 0);
		mib_state.master_state = kMIBReadReturnStatus;
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
			if (mib_state.bus_returnstatus.result == 0xFF && mib_state.bus_returnstatus.len == 0xFF)
			{
				bus_master_finish(kMIBFinalizeMessage);
				break;
			}

			//This is discarded, but we need to issue a read in case the slave is sending us a return value
			bus_receive(bus_master_lastaddress(), (unsigned char *)&mib_state.bus_returnstatus, 1, 0);
			mib_state.master_state = kMIBReadReturnStatus;
		}
		else if (mib_state.bus_returnstatus.result != kNoMIBError)
			bus_master_handleerror();
		else
		{
			if (mib_state.bus_returnstatus.len > 0)
			{
				bus_receive(bus_master_lastaddress(), (unsigned char *)mib_buffer, mib_state.bus_returnstatus.len, 0);
				mib_state.master_state = kMIBExecuteCallback;
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
		if (mib_state.master_callback != NULL)
			mib_state.master_callback(mib_state.bus_returnstatus.result);

		//bus_free_all();
		i2c_finish_transmission(); 
		break;
	}
}
