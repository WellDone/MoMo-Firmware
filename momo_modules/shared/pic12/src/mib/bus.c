#include "bus.h"

void bus_init()
{
	mib_firstfree = 0;
	i2c_enable(0x10);
}