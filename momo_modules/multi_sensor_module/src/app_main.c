//app_main.c

#include "platform.h"
#include "watchdog.h"
#include "port.h"
#include "sample.h"
#include "mib12_api.h"
#include "log.h"
#include "digital_amp.h"
#include "watchdog.h"
#include "state.h"
#include "pulse.h" 
#include "alarm_repeat_times.h"

MultiSensorState state;
extern unsigned int adc_result;

void task(void)
{
	while (state.acquire_pulse)
	{
		pulse_sample();
		state.acquire_pulse = 0;

		if ( state.push_pending )
		{
			state.push_pending = 0;

			bus_master_begin_rpc();
			mib_buffer[0] = mib_address;
			mib_buffer[1] = 0;

			mib_buffer[2] = 0;
			mib_buffer[3] = 0; //metadata

			mib_buffer[4] = pulse_count() & 0xFF;
			mib_buffer[5] = (pulse_count() >> 8) & 0xFF;
			mib_buffer[6] = 0;
			mib_buffer[7] = 0;

			bus_master_prepare_rpc(70, 0, plist_with_buffer(2, 4));
			bus_master_send_rpc(8);
		}
	}
}

void interrupt_handler(void)
{

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
	PIN_DIR(PULSE_IN, INPUT);

	damp_init();
	state.combined_state = 0;

	bus_master_begin_rpc();

	mib_buffer[0] = mib_address;
	mib_buffer[1] = 0;

	mib_buffer[2] = 8;
	mib_buffer[3] = 20;

	mib_buffer[4] = kEvery10Seconds;
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
	mib_buffer[0] = pulse_count() & 0xFF;
	mib_buffer[1] = pulse_count() >> 8;

	bus_slave_setreturn(pack_return_status(0, 2));
}

void scheduled_callback()
{
	if ( state.acquire_pulse == 0 )
	{
		state.acquire_pulse = 1;
		state.push_pending = 1;
	}
}