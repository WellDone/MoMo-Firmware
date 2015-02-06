//gsm_module.c

#include "gsm_module.h"
#include "gsm_serial.h"
#include "global_state.h"
#include "port.h"
#include <xc.h>

bool gsm_module_active()
{
	return PIN(GSMSTATUSPIN) == 1;
}
void gsm_module_off()
{
	ANALOG_IF_POSSIBLE(MODULEPOWERPIN);
	PIN_DIR(MODULEPOWERPIN, 1);
	PIN_DIR(GSMPOWERPIN, 1);
	ANALOG_IF_POSSIBLE(GSMSTATUSPIN);

	ANALOG_IF_POSSIBLE(GSMRESETPIN);

	state.module_on = 0;
	state.shutdown_pending = 0;
	state.stream_in_progress = 0;
}

uint8 gsm_module_on()
{
	PIN_DIR(GSMSTATUSPIN, 1);
	ENSURE_DIGITAL(GSMSTATUSPIN);
	ENSURE_DIGITAL(GSMRESETPIN);
	PIN_DIR(GSMRESETPIN, 1);
	PIN_SET(GSMRESETPIN, 1);

	ENSURE_DIGITAL(MODULEPOWERPIN);

	//If power to the module has not been enabled, do so.
	if (PIN(MODULEPOWERPIN) == 0)
	{
		DRIVE_HI(MODULEPOWERPIN);
		__delay_ms(500);
	}

	//If the module is not already on, turn it on
	if (PIN(GSMSTATUSPIN) == 0)
	{
		gsm_serial_init();

		DRIVE_LOW(GSMPOWERPIN);
		__delay_ms(1200);
		PIN_DIR(GSMPOWERPIN, 1);
		__delay_ms(3000);

		gsm_rx();
		gsm_rx();
		gsm_rx_clear();
		state.module_on = 1;
	}

	return PIN(GSMSTATUSPIN);
}