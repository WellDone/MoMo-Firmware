#include "mib12_api.h"
#include "protocol_defines.h"
#include "platform.h"
#include "ultrasound.h"

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