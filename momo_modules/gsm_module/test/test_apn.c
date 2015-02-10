//Name: test_apn
//Targets: all
//Type: application
//Description: Test to make sure that the apn buffer is working

#include <xc.h>
#include "symbols.h"
#include "test_log.h"
#include <stdint.h>

char mib_buffer[20] @ _mib_buffer_variable;
char apn_buffer[21] @ _gprs_apn_variable;
uint8_t param_spec    @ 0xA9;

void test_apn();
void test_apn2();
void load_mib();
void load_mib2();

void begin_tests()
{
	uint8_t temp;
	//Make sure buffer is initialized to 0
	c_call_gprs_init_buffers();
	assert(apn_buffer[0], 0);

	//Try filling buffer from MIB buffer
	load_mib();
	param_spec = 20;
	c_call_gprs_set_apn();
	test_apn();

	load_mib2();
	param_spec = 10;
	c_call_gprs_set_apn();
	test_apn2();	

	param_spec = 0;
	c_call_gprs_set_apn();
	assert(apn_buffer[0], 0);
}

void load_mib()
{
	uint8_t i;

	for (i=0; i<20; ++i)
	{
		mib_buffer[i] = i+1;
		assert(mib_buffer[i], (i+1));
	}
}

void load_mib2()
{
	uint8_t i;

	for (i=0; i<20; ++i)
	{
		mib_buffer[i] = i+25;
		assert(mib_buffer[i], (i+25));
	}
}

void test_apn2()
{
	uint8_t i;

	for (i=0; i<param_spec;++i)
	{
		assert(apn_buffer[i], (i+25));
	}
	
	assert(apn_buffer[param_spec], 0);
}

void test_apn()
{
	uint8_t i;

	for (i=0; i<param_spec;++i)
	{
		assert(apn_buffer[i], (i+1));
	}
	
	assert(apn_buffer[param_spec], 0);
}