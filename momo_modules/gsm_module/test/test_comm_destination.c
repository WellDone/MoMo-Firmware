//Name: test_comm_destination
//Targets: all
//Type: application
//Description: Test to make sure that the comm destination buffer is working

#include <xc.h>
#include "symbols.h"
#include "test_log.h"
#include <stdint.h>

char mib_buffer[20] @ _mib_buffer_variable;
char comm_destination[65] @ _comm_destination_variable;
uint8_t param_spec    @ 0xA9;

uint8_t i;

void load_mib();
void test_comm_dest();
void test_comm_dest2();
void load_mib2();

void begin_tests()
{
	i = 18;
	load_mib();
	param_spec = 18;
	c_call_set_comm_destination();
	assert(WREG, 0);
	
	i = 18;
	test_comm_dest();

	load_mib2();
	param_spec = 18;
	mib_buffer[0] = 18;
	c_call_set_comm_destination();
	assert(WREG, 0);
	test_comm_dest2();

	WREG = 0;
	c_call_comm_destination_get();
	assert(WREG, 1);

	WREG = 10;
	c_call_comm_destination_get();
	assert(WREG, 11);

	WREG = 20;
	c_call_comm_destination_get();
	assert(WREG, 21);

	//Make sure overruns are caught
	mib_buffer[0] = 60;
	c_call_set_comm_destination();
	assert(WREG, 1);

	//Make sure overrun catching logic does not have off-by-one error
	param_spec = 10;
	mib_buffer[0] = 54;
	c_call_set_comm_destination();
	assert(WREG, 0);

	param_spec = 10;
	mib_buffer[0] = 55;
	c_call_set_comm_destination();
	assert(WREG, 1);
}

void load_mib()
{
	mib_buffer[0] = 0;
	mib_buffer[1] = 0;

	for (i=2; i<20; ++i)
	{
		mib_buffer[i] = i-1;
		assert(mib_buffer[i], (i-1));
	}
}

void load_mib2()
{
	mib_buffer[0] = 0;
	mib_buffer[1] = 18;

	for (i=2; i<20; ++i)
	{
		mib_buffer[i] = i-1+18;
		assert(mib_buffer[i], (i-1+18));
	}
}

void test_comm_dest()
{
	assert(comm_destination[i], 0);

	for (i=0; i<18; ++i)
	{
		assert(comm_destination[i], i+1);
	}
}

void test_comm_dest2()
{
	assert(comm_destination[36], 0);

	for (i=0; i<36; ++i)
	{
		assert(comm_destination[i], i+1);
	}
}