#include "nullcallback.h"
#include "bus.h"

int master_nullcallback(int state)
{
	i2c_finish_transmission();

	return kCommandFinished;
}