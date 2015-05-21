#include "ultrasound.h"
#include <xc.h>
#include "port.h"

#define _XTAL_FREQ			4000000

void enable_power(void)
{
	LATCH(CLOCKENABLE) = 1;
	LATCH(ENABLE) = 1;

	__delay_ms(5);
}

void disable_power(void)
{
	LATCH(CLOCKENABLE) = 0;
	LATCH(ENABLE) = 0;
}