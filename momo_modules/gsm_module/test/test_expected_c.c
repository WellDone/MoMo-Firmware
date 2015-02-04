//Name: test_expected_c
//Targets: all
//Type: application
//Description: Test to make sure that the expected functions are working
//when called from c

#include <xc.h>
#include "symbols.h"
#include "test_log.h"

const char* str = "+HTTPACTION:";
const char* expected1 @ _expected1_variable;
const char* expected2 @ _expected2_variable;

void begin_tests()
{
	char value;

	c_call_gsm_rx_clear();

	//Note that xc8 makes assumptions about the bank that variables will
	//live in and what bank will be selected upon function call by looking
	//at the call chain for that function.
	asm("movlb 2");
	c_call_gsm_expect_ok_error();
	c_call_reset_expected1_ptr();
	c_call_reset_expected2_ptr();

	WREG='O';
	c_call_gsm_rx_push();
	c_call_gsm_rx_peek();
	assert(WREG, 'O');
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

	expected1 = str;
	expected2 = str;
	c_call_reset_expected1_ptr();
	c_call_reset_expected2_ptr();

	WREG='+';
	c_call_gsm_rx_push();
	c_call_gsm_rx_peek();
	c_call_gsm_check();
	value = WREG;
	assert(value, 0);

	WREG='H';
	c_call_gsm_rx_push();
	c_call_gsm_rx_peek();
	c_call_gsm_check();
	value = WREG;

	assert(value, 0);
	WREG='T';
	c_call_gsm_rx_push();
	c_call_gsm_rx_peek();
	c_call_gsm_check();
	value = WREG;
	assert(value, 0);

	WREG='T';
	c_call_gsm_rx_push();
	c_call_gsm_rx_peek();
	c_call_gsm_check();
	value = WREG;
	assert(value, 0);

	WREG='P';
	c_call_gsm_rx_push();
	c_call_gsm_rx_peek();
	c_call_gsm_check();
	value = WREG;
	assert(value, 0);

	WREG='A';
	c_call_gsm_rx_push();
	c_call_gsm_rx_peek();
	c_call_gsm_check();
	value = WREG;
	assert(value, 0);

	WREG='C';
	c_call_gsm_rx_push();
	c_call_gsm_rx_peek();
	c_call_gsm_check();
	value = WREG;
	assert(value, 0);

	WREG='T';
	c_call_gsm_rx_push();
	c_call_gsm_rx_peek();
	c_call_gsm_check();
	value = WREG;
	assert(value, 0);

	WREG='I';
	c_call_gsm_rx_push();
	c_call_gsm_rx_peek();
	c_call_gsm_check();
	value = WREG;
	assert(value, 0);

	WREG='O';
	c_call_gsm_rx_push();
	c_call_gsm_rx_peek();
	c_call_gsm_check();
	value = WREG;
	assert(value, 0);

	WREG='N';
	c_call_gsm_rx_push();
	c_call_gsm_rx_peek();
	c_call_gsm_check();
	value = WREG;
	assert(value, 0);

	WREG=':';
	c_call_gsm_rx_push();
	c_call_gsm_rx_peek();
	c_call_gsm_check();
	value = WREG;
	assert(value, 1);
}
