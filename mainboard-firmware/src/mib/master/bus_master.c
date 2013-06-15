#include "bus_master.h"
#include <string.h>

//MIB Global State
extern volatile MIBState 		mib_state;
extern volatile unsigned char 	mib_buffer[kBusMaxMessageSize];
extern unsigned int 			mib_firstfree;

//Local Prototypes that should not be called outside of this file
unsigned char 	bus_master_lastaddress();
void 			bus_master_finish();
void 			bus_master_compose_params(MIBParameterHeader **params, unsigned char param_count);


unsigned char bus_master_lastaddress()
{
	return mib_state.bus_msg.address >> 1;
}

void bus_master_finish()
{
	bus_send(bus_master_lastaddress(), (unsigned char *)mib_buffer, 1, 0);
	mib_state.master_state = kMIBFinalizeMessage;
}

void bus_master_compose_params(MIBParameterHeader **params, unsigned char param_count)
{
	volatile unsigned char *buffer;
	unsigned int i=0, j=0;

	bus_free_all();
	buffer = bus_allocate_space(kBusMaxMessageSize);

	for (i=0; i<param_count;++i)
	{
		MIBParameterHeader *header = params[i];

		memcpy((void *)&buffer[j], (void *)header, sizeof(MIBParameterHeader));
		j += sizeof(MIBParameterHeader);

		if (header->type == kMIBInt16Type)
		{
			MIBIntParameter *param = (MIBIntParameter*)header;
			memcpy((void *)&buffer[j], (void *)&param->value, sizeof(int));

			j+=sizeof(int);
		}
		else
		{
			MIBBufferParameter *param = (MIBBufferParameter*)header;
			memcpy((void *)&buffer[j], (void *)param->data, param->header.len);

			j += param->header.len;
		}
	}

	mib_state.master_param_length = j;
}

int bus_master_rpc(unsigned char address, unsigned char feature, unsigned char cmd, MIBParameterHeader **params, unsigned char param_count)
{
	mib_state.bus_command.feature = feature;
	mib_state.bus_command.command = cmd;

	mib_state.master_state = kMIBSendParameters;

	bus_master_compose_params(params, param_count);

	return bus_send(address, (unsigned char *)&mib_state.bus_command, sizeof(MIBCommandPacket), 0);
}

void bus_master_callback()
{
	switch(mib_state.master_state)
	{
		case kMIBSendParameters:
		bus_send(bus_master_lastaddress(), (unsigned char*)mib_buffer, mib_state.master_param_length, 0);
		mib_state.master_state = kMIBReadReturnStatus;
		break;

		case kMIBReadReturnStatus:
		bus_receive(bus_master_lastaddress(), (unsigned char *)mib_buffer, sizeof(MIBReturnValueHeader), 0);
		mib_state.master_state = kMIBReadReturnValue;
		break;

		case kMIBReadReturnValue:
		if (i2c_master_lasterror() != kI2CNoError)
		{
			//TODO: properly handle error here by calling callback with result code;
			bus_master_finish();
		}
		else
		{
			MIBReturnValueHeader *header = (MIBReturnValueHeader *)mib_buffer;
			mib_state.master_result = header->result;

			if (header->len > 0)
			{
				bus_receive(bus_master_lastaddress(), (unsigned char *)mib_buffer, header->len, 0);
				mib_state.master_state = kMIBExecuteCallback;
			}
			else
			{
				bus_master_finish();
			}
		}
		break;

		case kMIBExecuteCallback:
		//TODO: callback with return value
		bus_master_finish();
		break;

		case kMIBFinalizeMessage:
		bus_free_all();
		i2c_finish_transmission(); 
		break;
	}
}