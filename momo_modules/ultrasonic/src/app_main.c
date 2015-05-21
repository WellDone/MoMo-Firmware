//app_main.c

#include "platform.h"
#include "watchdog.h"
#include "port.h"
#include "mib12_api.h"
#include "watchdog.h"
#include "ioc.h"
#include "alarm_repeat_times.h"
#include <stdint.h>

void task(void)
{ 
	
}

void interrupt_handler(void)
{
	
}

void initialize(void)
{
	//Initialize all of our PINS to digital
	ENSURE_DIGITAL(CLOCKENABLE);
	ENSURE_DIGITAL(RESET1000);
	ENSURE_DIGITAL(ERR1000);
	ENSURE_DIGITAL(CHSEL);
	ENSURE_DIGITAL(INT7200);
	ENSURE_DIGITAL(ENABLE);
	ENSURE_DIGITAL(TRIG);
	ENSURE_DIGITAL(SDO);
	ENSURE_DIGITAL(SDI);
	ENSURE_DIGITAL(SCK);
	ENSURE_DIGITAL(CS7200);
	ENSURE_DIGITAL(CS1000);

	//Setup control pins correctly 
	//Enable and Clock Enable are active high to turn on the
	//clock and ultrasonic chips

	LATCH(CLOCKENABLE) = 0;
	PIN_DIR(CLOCKENABLE, OUTPUT);

	LATCH(ENABLE) = 0;
	PIN_DIR(ENABLE, OUTPUT);

	LATCH(CS7200) = 1;
	PIN_DIR(CS7200, OUTPUT);

	LATCH(CS1000) = 1;
	PIN_DIR(CS1000, OUTPUT);

	//Reset is active high
	LATCH(RESET1000) = 0;
	PIN_DIR(RESET1000, OUTPUT);

	//Trigger is a signal from TDC7200 to TDC1000
	//FIXME: Check to see if we can enable a Weak *PULL DOWN*
	PIN_DIR(TRIG, INPUT);


	//Interrupt is an open-drain signal from TDC7200 to MCU
	//FIXME: Enable a weak pull up on this pin
	PIN_DIR(INT7200, INPUT);

	//Error is an open-drain signal from TDC1000 to MCU
	//FIXME: Enable a weak pull up on this pin
	PIN_DIR(ERR1000, INPUT);
	
	//Channel select is largely ignored in the measurement modes we us
	LATCH(CHSEL) = 0;
	PIN_DIR(CHSEL, OUTPUT);
}

void main(void)
{

}

