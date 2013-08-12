//gsm_serial.c

#include "platform.h"
#include "gsm_serial.h"
#include "mib12_api.h"
#include <string.h>

uint8 gsm_buffer[32];
uint8 buffer_len;

void open_gsm_module()
{
	gsm_buffer[0] = 'A';
	gsm_buffer[1] = 'T';
	gsm_buffer[2] = '\r';

	buffer_len = 3;

	send_buffer();

	receive_response();
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
	if (OERR)
	{
		CREN = 0;
		CREN = 1;
	}

	while(1)
	{
		while(!RCIF)
			;

		gsm_buffer[buffer_len] = RCREG;

		if (gsm_buffer[buffer_len] == '\n')
		{
			buffer_len += 1;
			return 0;
		}

		buffer_len += 1;

		if (buffer_len == 32)
			return 1;
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