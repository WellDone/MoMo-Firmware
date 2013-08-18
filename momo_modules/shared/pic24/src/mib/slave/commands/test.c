#include "bus_slave.h"
#include "bus.h"
#include "test.h"
#include <string.h>
#include <stdlib.h>
#include "protocol.h"

void test_command(void)
{
	if ( plist_get_int16(0) != 42 )
	{
		bus_slave_seterror( kCallbackError );
		return;
	}
	//_RA1 = !_RA1; //Blink light
	
	const char* out = "testing...";
	strcpy( plist_get_buffer(0), out );
	bus_slave_setreturn(pack_return_status(kNoMIBError,strlen(out)) );
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