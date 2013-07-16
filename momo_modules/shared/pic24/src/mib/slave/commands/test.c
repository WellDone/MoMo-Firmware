#include "bus_slave.h"
#include "bus.h"
#include "test.h"
#include <string.h>

<<<<<<< HEAD
void test_command(void)
=======
#include "mib_features.h"

extern volatile unsigned char 	mib_buffer[kBusMaxMessageSize];

void* test_command(MIBParamList *param)
>>>>>>> 241aef35421fb42d8048139c0c0e0f4312ad4468
{
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

<<<<<<< HEAD
	bus_slave_setreturn(kNoMIBError| kHasReturnValue);
}
=======
	bus_slave_setreturn(kNoMIBError, (MIBParameterHeader*)mib_buffer);

	return NULL;
}

DEFINE_MIB_FEATURE_COMMANDS(test)
{
	{0, echo_buffer, plist_define1(kMIBBufferType) },
	{1, test_command, plist_define2(kMIBInt16Type, kMIBInt16Type) }
};
DEFINE_MIB_FEATURE(test);
>>>>>>> 241aef35421fb42d8048139c0c0e0f4312ad4468
