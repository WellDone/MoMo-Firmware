//app_main.c

#include "platform.h"
#include "watchdog.h"
#include "port.h"
#include "mib12_api.h"
#include "watchdog.h"
#include "ioc.h"
#include "wpu.h"
#include "communication.h"
#include "tdc1000.h"
#include "tdc7200.h"
#include <stdint.h>

static void copy_tof_to_mib(uint8_t tof_index, uint8_t mib_index);

uint8_t test_level_measurement;

void task(void)
{ 
	if (test_level_measurement)
	{
		uint8_t return_length = 1;
		uint8_t retval;

		tdc1000_setgain(mib_buffer[4], mib_buffer[6], mib_buffer[8]);
		tdc1000_setexcitation(mib_buffer[0], mib_buffer[2]);
		tdc1000_setmode(mib_buffer[12]);
		tdc1000_setchannel(mib_buffer[14]);
		tdc1000_setstarttime(255);
		
		if (tdc1000_push() == 0)
		{	
			tdc7200_setstopmask(*(uint16_t *)&(mib_buffer[10]));
			tdc7200_setstops(0b100);
			retval = tdc7200_start();
			if (retval == 0)
			{
				while (PIN(INT7200) && PIN(ERR1000))
					;

				copy_tof_to_mib(0, 0);
				copy_tof_to_mib(1, 4);
				copy_tof_to_mib(2, 8);
				copy_tof_to_mib(3, 12);
				copy_tof_to_mib(4, 16);


				return_length = 20;
			}
			else
				mib_buffer[0] = retval;
		}
		else
			mib_buffer[0] = tdc1000_push();

		test_level_measurement = 0;

		bus_master_async_callback(return_length);
	}
}

static void copy_tof_to_mib(uint8_t tof_index, uint8_t mib_index)
{
	int32_t tof = tdc7200_tof(tof_index);

	mib_buffer[mib_index + 0] = (tof >> 0) & 0xFF;
	mib_buffer[mib_index + 1] = (tof >> 8) & 0xFF;
	mib_buffer[mib_index + 2] = (tof >> 16) & 0xFF;
	mib_buffer[mib_index + 3] = (tof >> 24) & 0xFF;
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
	tdc1000_init();
	tdc7200_init();

	test_level_measurement = 0;
}

void main(void)
{

}

