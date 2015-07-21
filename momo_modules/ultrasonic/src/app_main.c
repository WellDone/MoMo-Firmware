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
#include "state.h"
#include "autoconf.h"
#include "automeasure.h"
#include <stdint.h>

//Global State
module_state	state;

static void copy_tof_to_mib(uint8_t tof_index, uint8_t mib_index);
static void process_async_actions();

extern int32_t 	delta_tof_accum;
extern uint16_t num_measurements;
extern int32_t last_automatic_reading;

void task(void)
{
	switch(state.mode)
	{
		case kManualMode:
		if (state.flags.value)
			process_async_actions();
		break;

		case kAutomaticMode:
		automatic_measurement();
		break;
	}
}

static void process_async_actions()
{
	if (state.flags.take_delta_tof_measurement)
	{
		UltrasoundError err;
		int32_t *mib = (int32_t *)mib_buffer;

		if (mib_buffer[2] == 0)
			err = accumulate_delta_tof(mib_buffer[0]);
		else
			err = fast_accumulate_delta_tof(mib_buffer[0]);

		mib[0] = delta_tof_accum;
		mib[1] = num_measurements;
		mib_buffer[8] = err;

		state.flags.take_delta_tof_measurement = 0;

		bus_master_async_callback(9);
	}


	if (state.flags.take_tof_measurement)
	{
		uint8_t direction = mib_buffer[0];

		enable_power();
		tdc7200_setaverages(0);
		tdc1000_prepare_deltatof(0);

		LATCH(CHSEL) = direction;
		
		take_measurement();
		tdc7200_readresults();

		copy_tof_to_mib(0, 0);
		copy_tof_to_mib(1, 4);
		copy_tof_to_mib(2, 8);
		copy_tof_to_mib(3, 12);
		mib_buffer[16] = tdc7200_readfast(kTDC7200_INTStatusReg);

		//copy_tof_to_mib(4, 16);

		state.flags.take_tof_measurement = 0;
		disable_power();

		bus_master_async_callback(17);
	}

	if (state.flags.find_signal_strength)
	{
		mib_buffer[0] = find_signal_strength();
		state.flags.find_signal_strength = 0;
		bus_master_async_callback(1);
	}

	if (state.flags.find_optimal_gain)
	{
		mib_buffer[5] = find_optimal_gain((int32_t *)&mib_buffer[0], &mib_buffer[4]);
		state.flags.find_optimal_gain = 0;
		bus_master_async_callback(6);
	}

	if (state.flags.find_pulse_variance)
	{
		find_pulse_variance((int32_t *)&mib_buffer[0]);
		state.flags.find_pulse_variance = 0;
		bus_master_async_callback(20);
	}
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

	last_automatic_reading = 0;

	state.flags.value = 0;
	state.mode = kAutomaticMode;
	state.autostatus = kNotInAutomaticMode;
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

static void copy_tof_to_mib(uint8_t tof_index, uint8_t mib_index)
{
	int32_t tof = tdc7200_tof(tof_index, 0);

	mib_buffer[mib_index + 0] = (tof >> 0) & 0xFF;
	mib_buffer[mib_index + 1] = (tof >> 8) & 0xFF;
	mib_buffer[mib_index + 2] = (tof >> 16) & 0xFF;
	mib_buffer[mib_index + 3] = (tof >> 24) & 0xFF;
}