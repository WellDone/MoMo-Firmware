#include "bus_slave.h"
#include "bus.h"
#include "test.h"
#include <string.h>

void test_command(void)
{
	_RA1 = !_RA1;
	
	loadparams(plist_1param(kMIBInt16Type));
	set_intparam(0, 6);
	
	bus_slave_setreturn(kNoMIBError, (MIBParameterHeader*)mib_buffer);
}

void echo_buffer(void)
{
	MIBBufferParameter *buf = get_buffer_param(0);

	memmove((void*)mib_buffer, buf, 2);
	memmove((void*)mib_buffer+2, buf->data, buf->header.len);

	bus_slave_setreturn(kNoMIBError, (MIBParameterHeader*)mib_buffer);
}