#include "receivecommand.h"
#include "common.h"

int slave_receive_command(int state)
{
	if (state == kCommandBegin)
	{
		unsigned char resp = 0xAB;
		bus_slave_send(&resp, 1, 0);

		return 1;
	}
	else if (state == 1)
	{
		_RA1 = !_RA1;
		i2c_slave_setidle();
	}

	return kCommandFinished;
}