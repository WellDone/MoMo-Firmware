#include "receivecommand.h"
#include "common.h"

int slave_receive_command(int state)
{
	if (state == kCommandBegin)
	{
		unsigned char resp = 0xAA;
		bus_slave_send(&resp, 1, 0);

		return 1;
	}
	else if (state == 1)
		;//_RA0 = !_RA0;

	return kCommandFinished;
}