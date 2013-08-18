//gsm_serial.c

#include "platform.h"
#include "gsm_serial.h"
#include "gsm_strings.h"
#include "mib12_api.h"
#include <string.h>

uint8 gsm_buffer[32];
uint8 buffer_len;

uint8 test_siminserted();

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
}

uint8 wait_for_text()
{
	RCREG;
	RCREG;
	buffer_len = 0;

	if (OERR)
	{
		CREN = 0;
		CREN = 1;
	}

	state.wait_for_text = 1;  
	while(state.wait_for_text)
	{
		//RC2 = !RC2;
		if (RCIF)
		{
			if (buffer_len == 32)
				buffer_len = 0;

			gsm_buffer[buffer_len++] = RCREG;

			if (gsm_buffer[buffer_len-1] == '\n')
			{				
				if (match_newmsg())
					return 1;

				if (match_newmsg2digit())
					return 1;

				buffer_len = 2; //reset the buffer after every \r\n so we don't overflow
			}
		}
	}
}

void copy_mib()
{
	uint8 i;

	buffer_len = mib_buffer_length();

	for (i=0; i<buffer_len; ++i)
		gsm_buffer[i] = mib_buffer[i];
}

void append_carriage()
{
	gsm_buffer[buffer_len++] = '\r';
}