/*
 * mib12_appunittest.c
 * The standard driver file for a unit test meant to be run on mib12_application modules
 */

//User test files must define this function
extern void begin_tests();

//This function is defined in test_log.as
extern void finish_tests();

void main()
{
	begin_tests();	
	finish_tests();
} 