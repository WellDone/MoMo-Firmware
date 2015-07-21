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
#include "watchdog.h"

extern module_state	state;
extern int32_t 	delta_tof_accum;

int32_t last_automatic_reading;

void automatic_measurement()
{
	uint8_t gain;
	int32_t old_reading;
	//Search for a signal

	state.status = kSearchingForSignal;

	while (find_optimal_gain(&old_reading, &gain) != kNoUltrasoundError)
	{
		WDTCON = 0b01010; //1s timeout
		wdt_enable();
		asm("sleep");
		wdt_disable();
	}

	set_gain_index(gain);

	last_automatic_reading = 0;
	old_reading = 0;

	state.status = kAcquiringData;

	while (state.mode == kAutomaticMode)
	{
		UltrasoundError err;
		int32_t delta;

		//Don't allow rpc calls during tof measurements for stability
		GIE = 0;
		err = fast_accumulate_delta_tof(3);
		GIE = 1;

		if (err == kNoUltrasoundError)
		{
			//Make sure the reading is sensible (between -50ns and 50ns and lowpass filter it to remove noise)
			if (delta_tof_accum < 3125L && delta_tof_accum > -3125L)
			{
				GIE = 0;
				delta = delta_tof_accum - old_reading;
				last_automatic_reading = old_reading + (delta >> 3);
				old_reading = last_automatic_reading;
				GIE = 1;
			}
		}

		WDTCON = 0b00111; //128 ms timeout
		wdt_enable();
		asm("sleep");
		wdt_disable();
	}
}