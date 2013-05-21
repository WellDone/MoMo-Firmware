/* Writes to PORTA are actually written to corresponding LATA register.
   Reads from PORTA register is return of actual I/O pin values. */

#define LED_1 LATA4
//#define LED_2 LATE2
//#define LED_3 LATE1
#define LED_4 LATA5

#define BUTT RA2


void BlinkLED();
void Initialize ();
