#include "bus_slave.h"
#include "bus.h"
#include "test.h"
#include <string.h>
#include <stdlib.h>
#include "protocol.h"

uint8_t test_command(uint8_t length)
{
	if ( plist_get_int16(0) != 42 )
		return 7;
	
	const char* out = "testing...";
	bus_slave_return_buffer( out, strlen(out) );

	return kNoErrorCode;
}


DEFINE_MIB_FEATURE_COMMANDS(test)
{
	{1, test_command}
};

DEFINE_MIB_FEATURE(test);