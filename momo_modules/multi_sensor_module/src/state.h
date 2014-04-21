#ifndef __state_h__
#define __state_h__

#include <stdint.h>

typedef struct
{
	union
	{
		struct
		{
			uint8_t acquire_pulse : 1;
			uint8_t reserved : 7;
		};

		uint8_t combined_state;
	};
} MultiSensorState;

#endif