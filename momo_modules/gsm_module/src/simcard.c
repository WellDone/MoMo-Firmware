//simcard.c

#include "simcard.h"
#include "gsm_defines.h"

void simdet_idle()
{
	MAKE_ANALOG(SIMDET_POW);
	MAKE_ANALOG(SIMDET_SENSE);
}


uint8 simdet_detect()
{
	SET_DIRECTION(SIMDET_SENSE, 1);
	MAKE_DIGITAL(SIMDET_SENSE);

	SET_DIRECTION(SIMDET_POW, 0);
	SET_LEVEL(SIMDET_POW, 1);
	MAKE_DIGITAL(SIMDET_POW);

	if (PIN(SIMDET_SENSE) == 0)
	{
		simdet_idle();
		return 0;
	}

	simdet_idle();
	return 1;
}