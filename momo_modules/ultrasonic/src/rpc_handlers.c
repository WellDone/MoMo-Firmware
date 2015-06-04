#include "mib12_api.h"
#include "protocol_defines.h"
#include "platform.h"
#include "communication.h"
#include "measure.h"

extern uint8_t test_level_measurement;
extern uint8_t take_delta_tof_measurement;
extern uint8_t find_noise_floor_flag;

uint8_t set_power(uint8_t length)
{
	if (mib_buffer[0] == 0)
		disable_power();
	else
		enable_power();

	return kNoErrorStatus;
}

uint8_t read_tdc7200_register(uint8_t length)
{
	mib_buffer[0] = tdc7200_read8(mib_buffer[0]);

	bus_slave_returndata(1);

	return kNoErrorStatus;
}

uint8_t read_tdc7200_register24(uint8_t length)
{
	uint32_t result = tdc7200_read24(mib_buffer[0]);

	mib_buffer[0] = result & 0xFF;
	mib_buffer[1] = result >> 8 & 0xFF;
	mib_buffer[2] = result >> 16 & 0xFF;

	bus_slave_returndata(3);

	return kNoErrorStatus;
}


uint8_t write_tdc7200_register(uint8_t length)
{
	tdc7200_write8(mib_buffer[0], mib_buffer[2]);

	return kNoErrorStatus;
}

uint8_t read_tdc1000_register(uint8_t length)
{
	mib_buffer[0] = tdc1000_read8(mib_buffer[0]);
	
	bus_slave_returndata(1);

	return kNoErrorStatus;
}

uint8_t write_tdc1000_register(uint8_t length)
{
	tdc1000_write8(mib_buffer[0], mib_buffer[2]);

	return kNoErrorStatus;
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
	test_level_measurement = 1;
	
	return kAsynchronousResponseStatus;
}

uint8_t find_noise_floor(uint8_t length)
{
	find_noise_floor_flag = 1;

	return kAsynchronousResponseStatus;
}

uint8_t take_delta_tof(uint8_t length)
{
	take_delta_tof_measurement = 1;
	return kAsynchronousResponseStatus;
}

uint8_t set_measurement_parameters(uint8_t length)
{
	set_parameters(mib_buffer[0], mib_buffer[2], mib_buffer[4], *(uint16_t *)(&mib_buffer[6]));

	return kNoErrorStatus;
}