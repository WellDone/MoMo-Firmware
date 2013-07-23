#include "bus_slave.h"
#include "bus.h"
#include "test.h"
#include <string.h>
#include "protocol.h"

void test_command(void)
{
	if ( plist_get_int16(0) != 42 )
	{
		//ERROR
		return;
	}
	if ( memcmp( plist_get_buffer(1), "freebird", plist_get_buffer_length() ) == 0 )
	{
		_RA0 = !_RA0;
	}
	_RA1 = !_RA1; //Blink light
	
	plist_set_int16(0, 6);
	bus_slave_setreturn(pack_return_status(kNoMIBError,kIntSize));
}

/*
void echo_buffer(void)
{
	MIBBufferParameter *buf = get_buffer_param(0);

	memmove((void*)mib_buffer, buf, 2);
	memmove((void*)mib_buffer+2, buf->data, buf->header.len);

	bus_slave_setreturn(kNoMIBError| kHasReturnValue);
}*/

DEFINE_MIB_FEATURE_COMMANDS(test)
{
//	{0, echo_buffer, plist_define1(kMIBBufferType) },
	{1, test_command, plist_spec(1,true) }
};
DEFINE_MIB_FEATURE(test);