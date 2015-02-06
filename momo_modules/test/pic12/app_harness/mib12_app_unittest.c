/*
 * mib12_appunittest.c
 * The standard driver file for a unit test meant to be run on mib12_application modules
 */

#pragma config FOSC=INTOSC      /* Use internal oscillator as the frequency oscillator. */
#pragma config WDTE=SWDTEN  /* Enable the watchdog timer under software control. */
#pragma config PLLEN=OFF        /* Disable 4x phase lock loop. */
#pragma config WRT=OFF          /* Flash memory write protection off. */
#pragma config LVP=OFF
#pragma config MCLRE=OFF

//User test files must define this function
extern void begin_tests();

//This function is defined in test_log.as
extern void finish_tests();

void main()
{
	begin_tests();	
	finish_tests();
} 