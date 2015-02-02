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
	//Does not test anything currently because I need to find a way to infer
	//the proper way to make calls to xc8 functions since they use global variables
	//to pass any arguments except the first 8 bit one.
	c_call_reset_expected1_ptr();
	c_call_reset_expected2_ptr();
}
