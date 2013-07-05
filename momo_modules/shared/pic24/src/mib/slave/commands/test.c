#include "bus_slave.h"
#include "test.h"
#include <string.h>

extern unsigned char 	mib_buffer[kBusMaxMessageSize];

void test_command(void)
{
	_RA1 = !_RA1;
	
	MIBIntParameter *retval;

	retval = (MIBIntParameter*)bus_allocate_int_param();

	bus_init_int_param(retval, 6);
	bus_slave_setreturn(kNoMIBError, (MIBParameterHeader*)retval);
}

void echo_buffer(void)
{
	MIBBufferParameter *buf = get_buffer_param(0);

	memmove((void*)mib_buffer, buf, 2);
	memmove((void*)mib_buffer+2, buf->data, buf->header.len);

	bus_slave_setreturn(kNoMIBError, (MIBParameterHeader*)mib_buffer);
}