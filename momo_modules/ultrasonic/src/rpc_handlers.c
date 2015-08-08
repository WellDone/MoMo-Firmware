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

extern module_state	state;
extern int32_t last_automatic_reading;

extern int32_t 		tof1[5];
extern int32_t 		tof2[5];

uint8_t set_power(uint8_t length)
{
	if (mib_buffer[0] == 0)
		disable_power();
	else
		enable_power();

	return kNoErrorStatus;
}

uint8_t get_parameters(uint8_t length)
{
	fill_parameters();

	bus_slave_returndata(6);
	return kNoErrorStatus;
}

uint8_t find_signal_strength_rpc(uint8_t length)
{
	state.flags.find_signal_strength = 1;
	return kAsynchronousResponseStatus;
}

uint8_t find_optimal_gain_rpc(uint8_t length)
{
	state.flags.find_optimal_gain = 1;
	return kAsynchronousResponseStatus;
}

/*
 * Data format:
 * mib_buffer[0]: number of expected stops
 * mib_buffer[2]: number of transmitted pulses
 * mib_buffer[4]: PGA gain
 * mib_buffer[6]: LNA on
 * mib_buffer[8]: Threshold
 */

uint8_t perform_test_level_measurement(uint8_t length)
{
	state.flags.test_level_measurement = 1;
	
	return kAsynchronousResponseStatus;
}

uint8_t find_pulse_variance_rpc(uint8_t length)
{
	state.flags.find_pulse_variance = 1;
	return kAsynchronousResponseStatus;
}

uint8_t take_delta_tof(uint8_t length)
{
	state.flags.take_delta_tof_measurement = 1;
	return kAsynchronousResponseStatus;
}

uint8_t set_measurement_parameters(uint8_t length)
{
	set_parameters(mib_buffer[0], mib_buffer[2], mib_buffer[4], *(uint16_t *)(&mib_buffer[6]));

	return kNoErrorStatus;
}

uint8_t get_variance(uint8_t length)
{
	state.flags.find_variance = 1;
	return kAsynchronousResponseStatus;
}

uint8_t measure_tof(uint8_t length)
{
	state.flags.take_tof_measurement = 1;
	return kAsynchronousResponseStatus;
}

uint8_t get_tof(uint8_t length)
{
	int32_t *mib = (int32_t *)mib_buffer;

	if (mib_buffer[0] == 1)
	{
		mib[0] = tof1[0];
		mib[1] = tof1[1];
		mib[2] = tof1[2];
		mib[3] = tof1[3];
		mib[4] = tof1[4];

		bus_slave_returndata(20);
	}
	else if (mib_buffer[0] == 2)
	{
		mib[0] = tof2[0];
		mib[1] = tof2[1];
		mib[2] = tof2[2];
		mib[3] = tof2[3];
		mib[4] = tof2[4];

		bus_slave_returndata(20);
	}

	return kNoErrorStatus;
}

uint8_t get_last_reading(uint8_t length)
{
	int32_t *mib = (int32_t *)mib_buffer;
	mib[0] = last_automatic_reading;

	bus_slave_returndata(4);
	return kNoErrorStatus;
}