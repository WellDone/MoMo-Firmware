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
#include <stdint.h>

MultiSensorState state;

extern unsigned int adc_result;
extern uint8_t periods;

#define kMaxPeriods		10
uint16_t median_intervals[kMaxPeriods];

void task(void)
{ 
	wdt_disable();

	//If we slept for an increment, sample the number of pulses
	//Also allow pulse sampling if we received a sample command RPC
	if (nTO==0 || state.acquire_pulse)
	{
		pulse_sample();
		median_intervals[periods] = pulse_median_interval() >> 8; //divide by 256 to get approx in ms since each clock tick was 8 us
		state.acquire_pulse = 0;

		++periods;
	}

	if (state.push_pending)
	{
		uint16_t average_flow = 0;
		
		/*
		 * Compute and report the average flow rate in pulses per second
		 */
		if (periods > 0)
		{
			uint8_t i;

			for(i=0; i<periods; ++i)
			{
				if (median_intervals[i] > 0)
					average_flow += 1000UL/median_intervals[i];
			}

			average_flow /= periods;
		}

		state.push_pending = 0;

		bus_master_begin_rpc(8);
		mib_buffer[0] = slave_address;
		mib_buffer[1] = 0;

		mib_buffer[2] = 0;
		mib_buffer[3] = 0; //metadata

		mib_buffer[4] = average_flow & 0xFF;
		mib_buffer[5] = average_flow >> 8;
		mib_buffer[6] = 0;
		mib_buffer[7] = 0;

		bus_master_prepare_rpc(70, 0);
		bus_master_send_rpc(8);

		//Start counting again
		periods = 0;
	}

	WDTCON = k16SecondTimeout;
	wdt_enable();
}

void interrupt service_isr() 
{
	if (ioc_flag_b(PULSE_IOC))
	{
		if (PIN(PULSE_IN) == 0)
			pulse_falling_edge();
		else
			pulse_rising_edge();

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
	periods = 0;
	
	bus_master_begin_rpc(8);

	mib_buffer[0] = slave_address;
	mib_buffer[1] = 0;

	mib_buffer[2] = 2;
	mib_buffer[3] = 20;

	mib_buffer[4] = kEveryMinute;
	mib_buffer[5] = 0;
	bus_master_prepare_rpc(43, 0);

	bus_master_send_rpc(6);
}

void main()
{
	initialize();

	while(1)
	{
		task();
		asm("sleep");
	}
}

uint8_t check_v1(uint8_t length)
{
	sample_v1();

	mib_buffer[0] = (adc_result & 0xFF);
	mib_buffer[1] = (adc_result >> 8) & 0xFF;

	bus_slave_returndata(2);
	return kNoErrorStatus;
}

uint8_t check_v2(uint8_t length)
{
	sample_v2();

	mib_buffer[0] = (adc_result & 0xFF);
	mib_buffer[1] = (adc_result >> 8) & 0xFF;

	bus_slave_returndata(2);
	return kNoErrorStatus;
}

uint8_t acquire_voltage(uint8_t length)
{
	damp_enable();
	sample_analog();
	damp_disable();

	mib_buffer[0] = (adc_result & 0xFF);
	mib_buffer[1] = (adc_result >> 8) & 0xFF;

	bus_slave_returndata(2);
	return kNoErrorStatus;
}

uint8_t set_damp_parameter(uint8_t length)
{
	if (mib_buffer[0] >= kNumParameters)
		return 6;

	damp_set_parameter((AmplifierSetting)mib_buffer[0], mib_buffer[2]);
	
	return kNoErrorStatus;
}

uint8_t power_analog(uint8_t length)
{
	set_analog_power(mib_buffer[0]);
	
	return kNoErrorStatus;
}

uint8_t acquire_pulse(uint8_t length)
{
	state.acquire_pulse = 1;
	return kNoErrorStatus;
}

uint8_t number_of_pulses(uint8_t length)
{
	mib_buffer[0] = pulse_count() & 0xFF;
	mib_buffer[1] = pulse_count() >> 8;
	mib_buffer[2] = pulse_invalid_count() & 0xFF;
	mib_buffer[3] = pulse_invalid_count() >> 8;

	bus_slave_returndata(4);
	return kNoErrorStatus;
}

uint8_t read_pulse(uint8_t length)
{
	uint8_t pulse = mib_buffer[0];

	mib_buffer[0] = pulse_width(pulse) & 0xFF;
	mib_buffer[1] = pulse_width(pulse) >> 8;
	mib_buffer[2] = pulse_interval(pulse) & 0xFF;
	mib_buffer[3] = pulse_interval(pulse) >> 8;

	bus_slave_returndata(4);
	return kNoErrorStatus;
}

uint8_t scheduled_callback(uint8_t length)
{
	state.push_pending = 1;

	return kNoErrorStatus;
}

uint8_t read_periods(uint8_t length)
{
	mib_buffer[0] = periods & 0xFF;
	mib_buffer[1] = periods >> 8;

	bus_slave_returndata(2);
	return kNoErrorStatus;
}

uint8_t set_push_status(uint8_t length)
{
	state.push_disabled = !(mib_buffer[0] > 0);
	
	return kNoErrorStatus;
}

uint8_t read_median_interval(uint8_t length)
{
	uint16_t median = pulse_median_interval();

	mib_buffer[0] = median & 0xFF;
	mib_buffer[1] = median >> 8;

	bus_slave_returndata(2);
	return kNoErrorStatus;
}