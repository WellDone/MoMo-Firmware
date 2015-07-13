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
#include "measure.h"
#include <stdint.h>

#define _XTAL_FREQ			4000000

static void copy_tof_to_mib(uint8_t tof_index, uint8_t mib_index);

uint8_t test_level_measurement;
uint8_t take_delta_tof_measurement;
uint8_t find_noise_floor_flag;
uint8_t find_variance_flag;
uint8_t optimize_flag;

void task(void)
{
	tdc1000_setmode(kTDC1000_TOFMode);
	tdc1000_setexternal(1);
	while (1)
	{
		enable_power();
		take_measurement();
		__delay_ms(10);
		disable_power();
		__delay_ms(20);
	}

	/*if (test_level_measurement)
	{
		uint8_t return_length = 1;
		uint8_t retval;

		enable_power();
		tdc1000_setgain(mib_buffer[4], mib_buffer[6], mib_buffer[8]);
		tdc1000_setexcitation(mib_buffer[0], mib_buffer[2]);
		tdc1000_setmode(mib_buffer[12]);
		tdc1000_setchannel(mib_buffer[14]);
		
		if (tdc1000_push() == 0)
		{	
			tdc7200_setstopmask(*(uint16_t *)&(mib_buffer[10]));
			tdc7200_setstops(0b100);
			retval = tdc7200_start();
			if (retval == 0)
			{
				while (PIN(INT7200) && PIN(ERR1000))
					;

				if (PIN(INT7200) == 0)
				{
					copy_tof_to_mib(0, 0);
					copy_tof_to_mib(1, 4);
					copy_tof_to_mib(2, 8);
					copy_tof_to_mib(3, 12);
					copy_tof_to_mib(4, 16);

					return_length = 20;
				}
				else
				{
					mib_buffer[0] = tdc1000_readerror().value;
					mib_buffer[2] = tdc7200_read8(kTDC7200_INTStatusReg);
					return_length = 2;
				}
			}
			else
				mib_buffer[0] = retval;
		}
		else
			mib_buffer[0] = tdc1000_push();

		test_level_measurement = 0;

		disable_power();
		bus_master_async_callback(return_length);
	}

	if (take_delta_tof_measurement)
	{
		//FIXME: Do delta tof measurement and return the values

		take_delta_tof_measurement = 0;
		bus_master_async_callback(5);
	}

	if (find_noise_floor_flag)
	{
		uint32_t noise = noise_floor_voltage((uint32_t *)&mib_buffer[4], (uint32_t *)&mib_buffer[8]);

		mib_buffer[0] = (noise >> 0) & 0xFF;
		mib_buffer[1] = (noise >> 8) & 0xFF;
		mib_buffer[2] = (noise >> 16) & 0xFF;
		mib_buffer[3] = (noise >> 24) & 0xFF;

		find_noise_floor_flag = 0;
		bus_master_async_callback(12);
	}

	if (find_variance_flag)
	{
		find_variance_flag = 0;
		bus_master_async_callback(4);
	}

	if (optimize_flag)
	{
		optimize_flag = 0;
		bus_master_async_callback(7);
	}*/
}

static void copy_tof_to_mib(uint8_t tof_index, uint8_t mib_index)
{
	int32_t tof = tdc7200_tof(tof_index, 0);

	mib_buffer[mib_index + 0] = (tof >> 0) & 0xFF;
	mib_buffer[mib_index + 1] = (tof >> 8) & 0xFF;
	mib_buffer[mib_index + 2] = (tof >> 16) & 0xFF;
	mib_buffer[mib_index + 3] = (tof >> 24) & 0xFF;
}

void interrupt service_isr()
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

	//Channel select is largely ignored in the measurement modes we use
	LATCH(CHSEL) = 0;
	PIN_DIR(CHSEL, OUTPUT);

	init_spi();
	tdc1000_init();
	tdc7200_init();

	initialize_parameters();

	tdc1000_setmode(kTDC1000_TOFMode);
	tdc1000_setexternal(1);

	test_level_measurement = 0;
	take_delta_tof_measurement = 0;
	find_noise_floor_flag = 0;
	optimize_flag = 0;
}

void main(void)
{
	initialize();

	while(1)
	{
		task();
		asm("sleep");
	}
}

