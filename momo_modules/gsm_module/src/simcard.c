//simcard.c

#include "simcard.h"
#include "gsm_defines.h"
#include "port.h"

void simdet_idle()
{
	ANALOG_IF_POSSIBLE(SIMDET_POW);
	ANALOG_IF_POSSIBLE(SIMDET_SENSE);
	PIN_DIR(SIMDET_POW, 1);
	PIN_DIR(SIMDET_SENSE, 1);
}


uint8 simdet_detect()
{
	PIN_DIR(SIMDET_SENSE, 1);
	ENSURE_DIGITAL(SIMDET_SENSE);

	PIN_DIR(SIMDET_POW, 0);
	ENSURE_DIGITAL(SIMDET_POW);
	PIN_SET(SIMDET_POW, 1);

	__delay_ms(10);

	if (PIN(SIMDET_SENSE) == 0)
	{
		simdet_idle();
		return 0;
	}

	simdet_idle();
	return 1;
}