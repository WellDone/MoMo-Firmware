/*
 * mib12_exec_unittest.c
 * The standard driver file for a unit test meant to be run on the mib12_executive
 */

//User test files must define this function
extern void begin_tests();
extern void finish_tests();

void task(void)
{
	begin_tests();	
	finish_tests();
}

void interrupt_handler(void)
{

}

void initialize(void)
{
	
}

void main()
{
	
} 