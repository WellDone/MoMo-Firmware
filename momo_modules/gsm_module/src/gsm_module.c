//gsm_module.c

#include "gsm_module.h"
#include "gsm_serial.h"
#include "global_state.h"
#include <xc.h>

void gsm_init()
{
	MAKE_ANALOG(MODULEPOWERPIN);
	SET_DIRECTION(GSMPOWERPIN, 1);
	MAKE_ANALOG(GSMSTATUSPIN);
	MAKE_ANALOG(GSMRESETPIN);

	buffer_len = 0;
	state.module_on = 0;
}

uint8 gsm_on()
{
	SET_DIRECTION(GSMSTATUSPIN, 1);
	MAKE_DIGITAL(GSMSTATUSPIN);

	MAKE_DIGITAL(MODULEPOWERPIN);

	//If power to the module has not been enabled, do so.
	if (PIN(MODULEPOWERPIN) == 0)
	{
		DRIVE_HI(MODULEPOWERPIN);
		__delay_ms(500);
	}

	//If the module is not already on, turn it on
	if (PIN(GSMSTATUSPIN) == 0)
	{
		enable_serial();

		DRIVE_LOW(GSMPOWERPIN);
		__delay_ms(1200);
		SET_DIRECTION(GSMPOWERPIN, 1);
		__delay_ms(3000);

		open_gsm_module();

		state.module_on = 1;
	}

	return PIN(GSMSTATUSPIN);
}

void gsm_off()
{
	gsm_init();
}