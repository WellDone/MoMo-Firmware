//gsm_serial.c

#include "platform.h"
#include "gsm_serial.h"
#include "global_state.h"
#include "gsm_strings.h"
#include "mib12_api.h"
#include "gsm_defines.h"
#include <string.h>

typedef union 
{
	struct
	{
		volatile uint8 open_module:1;
		volatile uint8 close_module:1;
		volatile uint8 send_command:1;
		volatile uint8 module_open:1;
		volatile uint8 last_response:1;
		volatile uint8 wait_for_text: 1;
		volatile uint8 unused:2;
	};

	volatile uint8 gsm_state;
} ModuleState;

extern ModuleState state;

void enable_serial()
{
	TXEN = 1;
	SYNC = 0;
	SPEN = 1;

	BRGH = 1;
	BRG16 = 1;

	SPBRGH = 0;
	SPBRGL = 16;

	CREN = 1;
	
	//clear the contents of RCREG
	RCREG;
	RCREG;

	RCIF = 0;
}

uint8 open_gsm_module()
{
	gsm_buffer[0] = 'A';
	gsm_buffer[1] = 'T';
	gsm_buffer[2] = 'E';
	gsm_buffer[3] = '0';
	gsm_buffer[4] = '\r';

	buffer_len = 5;

	send_buffer();
	receive_response();

	__delay_ms(200);

	gsm_buffer[0] = 'A';
	gsm_buffer[1] = 'T';
	gsm_buffer[2] = '+';
	gsm_buffer[3] = 'C';
	gsm_buffer[4] = 'M';
	gsm_buffer[5] = 'E';
	gsm_buffer[6] = 'E';
	gsm_buffer[7] = '=';
	gsm_buffer[8] = '0';
	gsm_buffer[9] = '\r';

	buffer_len = 10;

	send_buffer();
	receive_response();

	return 0;
}

void send_buffer()
{
	uint8 i;

	for (i=0; i<buffer_len; ++i)
	{
		while(TXIF == 0)
			;

		TXREG = gsm_buffer[i];

		asm("nop");
		asm("nop");
	}
}

uint8 receive_response()
{
	RCREG;
	RCREG;
	buffer_len = 0;

	if (OERR)
	{
		CREN = 0;
		CREN = 1;
	}

	while(1)
	{
		while(!RCIF)
			;

		gsm_buffer[buffer_len++] = RCREG;

		if (gsm_buffer[buffer_len-1] == '\n')
		{
			if (match_okay_response())
			{
				return 0;
			}
			else if (match_error_response())
				return 1;
		}

		if (buffer_len == 32)
			return 2;
	}

	return 3;
}

void copy_mib()
{
	uint8 i;

	buffer_len = mib_buffer_length();

	for (i=0; i<buffer_len; ++i)
		gsm_buffer[i] = mib_buffer[i];
}

uint8 copy_to_mib()
{
	uint8 i;

	for (i=0;i<buffer_len; ++i)
		mib_buffer[i] = gsm_buffer[i];

	return buffer_len;
}

void append_carriage()
{
	gsm_buffer[buffer_len++] = '\r';
}