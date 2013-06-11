/*sendcommand.c
 * Implements the master state machine for sending an i2c remote procedure call using the mib protocol
 *
 *
 */

#include "bus.h"
#include <string.h>

extern volatile MIBState mib_state;

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
	mib_state.master_command.feature = feature;
	mib_state.master_command.command = cmd;

	mib_state.master_state = kMIBSendParameters;

	bus_master_compose_params(params, param_count);

	return bus_send(address, (unsigned char *)&mib_state.master_command, sizeof(MIBCommandPacket), 0);
}