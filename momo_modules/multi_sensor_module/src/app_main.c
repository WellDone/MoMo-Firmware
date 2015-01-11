//app_main.c

#include "platform.h"
#include "watchdog.h"
#include "port.h"
#include "sample.h"
#include "mib12_api.h"
#include "log.h"
#include "digital_amp.h"
#include "watchdog.h"
#include "ioc.h"
#include "state.h"
#include "pulse.h" 
#include "alarm_repeat_times.h"

MultiSensorState state;

extern unsigned int adc_result;
extern uint16_t counter;
extern uint16_t periods;

void task(void)
{
	wdt_disable();

	//If we slept for an increment, sample the number of pulses
	if (nTO==0)
		pulse_sample();

	if (state.push_pending)
	{
		uint16_t correction_factor = 1;
		state.push_pending = 0;

		/*
		 * We report data every minute, and we sample for 0.1 seconds
		 * per time.  So we need to correct our sampled counter for the
		 * ratio between the period of time that we are covering (60 seconds)
		 * and the amount of time we were sampling for (periods*0.1 seconds)
		 */

		if (periods > 0)
			correction_factor = 600/periods;

		counter *= correction_factor;

		bus_master_begin_rpc();
		mib_buffer[0] = mib_address;
		mib_buffer[1] = 0;

		mib_buffer[2] = 0;
		mib_buffer[3] = 0; //metadata

		mib_buffer[4] = counter & 0xFF;
		mib_buffer[5] = counter & 0xFF;
		mib_buffer[6] = 0;
		mib_buffer[7] = 0;

		bus_master_prepare_rpc(70, 0, plist_with_buffer(2, 4));
		bus_master_send_rpc(8);
	}

	WDTCON = k4SecondTimeout;
	wdt_enable();
}

void interrupt_handler(void)
{
	if (ioc_flag_b(PULSE_IOC))
	{
		counter += 1;
		ioc_flag_b(PULSE_IOC) = 0;
	}
}

void initialize(void)
{
	PIN_DIR(VOLT1, INPUT);
	PIN_TYPE(VOLT1, ANALOG);

	PIN_DIR(VOLT2, INPUT);
	PIN_TYPE(VOLT2, ANALOG);
	
	ENSURE_DIGITAL(AN_POWER);
	LATCH(AN_POWER) = 0;
	PIN_DIR(AN_POWER, OUTPUT);

	ENSURE_DIGITAL(AN_SELECT);
	LATCH(AN_SELECT) = 0;
	PIN_DIR(AN_SELECT, OUTPUT);

	ENSURE_DIGITAL(AN_INVERT);
	LATCH(AN_INVERT) = 0;
	PIN_DIR(AN_INVERT, OUTPUT);

	ENSURE_DIGITAL(AN_PROG);
	LATCH(AN_PROG) = 0;
	PIN_DIR(AN_PROG, OUTPUT);

	PIN_TYPE(AN_VOLTAGE, ANALOG);
	PIN_DIR(AN_VOLTAGE, INPUT);

	ENSURE_DIGITAL(PULSE_IN);
	LATCH(PULSE_IN) = 0;
	PIN_DIR(PULSE_IN, OUTPUT);

	//Setup serial port
	ENSURE_DIGITAL(SERIAL_TX);
	LATCH(SERIAL_TX) = 1;
	PIN_DIR(SERIAL_TX, OUTPUT);

	ENSURE_DIGITAL(SERIAL_RX);

	damp_init();
	state.combined_state = 0;

	ioc_enable_b();
	counter = 0;
	
	bus_master_begin_rpc();

	mib_buffer[0] = mib_address;
	mib_buffer[1] = 0;

	mib_buffer[2] = 8;
	mib_buffer[3] = 20;

	mib_buffer[4] = kEveryMinute;
	mib_buffer[5] = 0;
	bus_master_prepare_rpc(43, 0, plist_ints(3));

	bus_master_send_rpc(8);
}

void main()
{

}

void check_v1()
{
	sample_v1();

	mib_buffer[0] = (adc_result & 0xFF);
	mib_buffer[1] = (adc_result >> 8) & 0xFF;

	bus_slave_setreturn(pack_return_status(0, 2));
}

void check_v2()
{
	sample_v2();

	mib_buffer[0] = (adc_result & 0xFF);
	mib_buffer[1] = (adc_result >> 8) & 0xFF;

	bus_slave_setreturn(pack_return_status(0, 2));
}

void acquire_voltage()
{
	damp_enable();
	sample_analog();
	damp_disable();

	mib_buffer[0] = (adc_result & 0xFF);
	mib_buffer[1] = (adc_result >> 8) & 0xFF;

	bus_slave_setreturn(pack_return_status(0, 2));
}

void set_damp_parameter()
{
	if (mib_buffer[0] >= kNumParameters)
	{
		bus_slave_setreturn(pack_return_status(6, 0));
		return;
	}

	damp_set_parameter((AmplifierSetting)mib_buffer[0], mib_buffer[2]);
	bus_slave_setreturn(pack_return_status(0, 0));
}

void power_analog()
{
	set_analog_power(mib_buffer[0]);
	bus_slave_setreturn(pack_return_status(0, 0));
}

void acquire_pulse()
{
	state.acquire_pulse = 1;
	bus_slave_setreturn(pack_return_status(0, 0));
}

void read_pulses()
{
	mib_buffer[0] = counter & 0xFF;
	mib_buffer[1] = counter >> 8;

	counter = 0;
	bus_slave_setreturn(pack_return_status(0, 2));
}

void scheduled_callback()
{
	state.push_pending = 1;
}