//Name: test_expected_c
//Targets: all
//Type: application
//Description: Test to make sure that the expected functions are working
//when called from c

#include <xc.h>
#include "symbols.h"
#include "test_log.h"

void begin_tests()
{
	char value;

	c_call_gsm_rx_clear();
	c_call_gsm_expect_ok_error();
	c_call_reset_expected1_ptr();
	c_call_reset_expected2_ptr();

	WREG='O';
	c_call_gsm_rx_push();
	c_call_gsm_rx_peek();
	c_call_check_inc_expected1();
	value = WREG;
	assert(value, 'K');
	
	WREG='K';
	c_call_gsm_rx_push();
	c_call_gsm_rx_peek();
	c_call_check_inc_expected1();
	value = WREG;
	assert(value, 0);

	//Check gsm_check
	WREG='O';
	c_call_gsm_rx_push();
	c_call_gsm_rx_peek();
	c_call_gsm_check();
	value = WREG;
	assert(value, 0);
	
	WREG='K';
	c_call_gsm_rx_push();
	c_call_gsm_rx_peek();
	c_call_gsm_check();
	value = WREG;
	assert(value, 1);
}
