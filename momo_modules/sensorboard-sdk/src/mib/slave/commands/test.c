#include "bus_slave.h"
#include "test.h"

void* test_command(MIBParamList *param)
{
	/*//_RA1 = !_RA1;
	RA5 = !RA5;
	
	MIBIntParameter *retval;

	bus_free_all();

	retval = (MIBIntParameter*)bus_allocate_int_param();

	bus_init_int_param(retval, 6);
	bus_slave_setreturn(kNoMIBError, (MIBParameterHeader*)retval);
	*/
	return NULL;
}

void* echo_buffer(MIBParamList *list)
{
	/*
	MIBBufferParameter *buf = (MIBBufferParameter *)list->params[0];
	bus_free_all();

	//memmove((char*)mib_buffer, buf, 2);
	//memmove((char*)mib_buffer+2, buf->data, buf->header.len);

	bus_slave_setreturn(kNoMIBError, (MIBParameterHeader*)mib_buffer);
	*/
	return NULL;
}