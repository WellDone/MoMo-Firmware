//Name: test_gsmbuffer
//Targets: all
//Type: application
//Additional: support_repeater.cmd
//Description: Test to ensure that the basic unit testing framework is working
//and that this test can be compile, run and interpreted.  

#include "symbols.h"
#include "test_log.h"
#include <xc.h>

extern unsigned char gsm_rx_buffer[64] @ _gsm_rx_buffer_variable;

void begin_tests(void)
{
	c_call_gsm_serial_init();
	c_call_gsm_rx_clear();
	WREG = 'A';
	c_call_gsm_write_char();
	c_call_gsm_rx();
	assert(0, WREG);
	c_call_gsm_rx_peek();
	assert('A', WREG);
}

void load_buffer(void)
{
	unsigned char i;

	for (i=0;i<64; ++i)
	{
		gsm_rx_buffer[i] = i;
	}
}