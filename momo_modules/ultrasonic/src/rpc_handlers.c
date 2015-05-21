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