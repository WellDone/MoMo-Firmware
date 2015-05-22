//app_main.c

#include "platform.h"
#include "watchdog.h"
#include "port.h"
#include "mib12_api.h"
#include "watchdog.h"
#include "ioc.h"
#include "wpu.h"
#include "ultrasound.h"
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

	//Reset is active high
	LATCH(RESET1000) = 0;
	PIN_DIR(RESET1000, OUTPUT);

	enable_weak_pullups();

	//Trigger is a signal from TDC7200 to TDC1000
	PIN_DIR(TRIG, INPUT);

	//Interrupt is an open-drain signal from TDC7200 to MCU
	PIN_DIR(INT7200, INPUT);
	enable_pullup(INT7200);

	//Error is an open-drain signal from TDC1000 to MCU
	PIN_DIR(ERR1000, INPUT);
	enable_pullup(ERR1000);

	//Channel select is largely ignored in the measurement modes we us
	LATCH(CHSEL) = 0;
	PIN_DIR(CHSEL, OUTPUT);

	init_spi();
}

void main(void)
{

}

