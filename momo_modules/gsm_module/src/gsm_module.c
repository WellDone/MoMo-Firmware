//gsm_module.c

#include "gsm_module.h"
#include "gsm_serial.h"
#include "gsm_strings.h"
#include "global_state.h"
#include <xc.h>

void gsm_reset()
{
	MAKE_ANALOG(MODULEPOWERPIN);
	SET_DIRECTION(GSMPOWERPIN, 1);
	MAKE_ANALOG(GSMSTATUSPIN);

	MAKE_ANALOG(GSMRESETPIN);

	state.module_on = 0;
	state.shutdown_pending = 0;
}

void gsm_init()
{
	gsm_reset();

	buffer_len = 0;
	rx_buffer_start = rx_buffer_end = rx_buffer_len = 0;
}

uint8 gsm_on()
{
	SET_DIRECTION(GSMSTATUSPIN, 1);
	MAKE_DIGITAL(GSMSTATUSPIN);
	MAKE_DIGITAL(GSMRESETPIN);
	SET_DIRECTION(GSMRESETPIN, 1);
	SET_LEVEL(GSMRESETPIN, 1);

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


void send_creg_query()
{
	gsm_buffer[0] = 'A';
	gsm_buffer[1] = 'T';
	gsm_buffer[2] = '+';
	gsm_buffer[3] = 'C';
	gsm_buffer[4] = 'R';
	gsm_buffer[5] = 'E';
	gsm_buffer[6] = 'G';
	gsm_buffer[7] = '?';
	buffer_len = 8;
	append_carriage();
	send_buffer();
}
bool wait_for_registration()
{
	uint8 counter = GSM_REGISTRATION_TIMEOUT_S * 2;
	while ( true )
	{
		if ( counter == 0 )
			return false;

		reset_match_counters();
		send_creg_query();

		while ( true )
		{
			if ( gsm_receiveone() == 1 )
			{
				counter -= 1;
				break;
			}
			if ( creg_matched() )
				return true;
		}
	}
	return false;
}

void gsm_off()
{
	gsm_reset();
}