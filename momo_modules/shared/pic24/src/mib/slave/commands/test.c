#include "bus_slave.h"
#include "bus.h"
#include "test.h"
#include <string.h>

void test_command(void)
{
	if ( get_uint16_param(0) != 42 )
	{
		//ERROR
		return;
	}
	MIBBufferParameter* buf = get_buffer_param(1);
	if ( memcmp( buf->data, "freebird", buf->header.len ) == 0 )
	{
		_RA0 = !_RA0;
	}
	_RA1 = !_RA1; //Blink light
	
	loadparams(plist_1param(kMIBInt16Type));
	set_intparam(0, 6);
	
	bus_slave_setreturn(kNoMIBError | kHasReturnValue);
}

void echo_buffer(void)
{
	MIBBufferParameter *buf = get_buffer_param(0);

	memmove((void*)mib_buffer, buf, 2);
	memmove((void*)mib_buffer+2, buf->data, buf->header.len);

	bus_slave_setreturn(kNoMIBError| kHasReturnValue);
}

DEFINE_MIB_FEATURE_COMMANDS(test)
{
	{0, echo_buffer, plist_define1(kMIBBufferType) },
	{1, test_command, plist_define2(kMIBInt16Type, kMIBBufferType) }
};
DEFINE_MIB_FEATURE(test);