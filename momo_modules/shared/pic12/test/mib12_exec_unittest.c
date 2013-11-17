/*
 * mib12_exec_unittest.c
 * The standard driver file for a unit test meant to be run on the mib12_executive
 */

#include "platform.h"
#include "watchdog.h"

extern void loghex(char val);
extern void finish_tests();

void task(void)
{
	loghex(0xAA);
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