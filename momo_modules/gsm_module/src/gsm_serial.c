//gsm_serial.c

#include "platform.h"
#include "gsm_serial.h"
#include "global_state.h"
#include "mib12_api.h"
#include "gsm_defines.h"
#include "gsm_module.h"
#include "gsm_strings.h"
#include "timer1.h"
#include <string.h>

void enable_serial()
{
	#ifdef ALTERNATE_SERIAL
	RXDTSEL = 1;
	TXCKSEL = 1;
	#endif

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

	gsm_buffer[0] = 'A';
	gsm_buffer[1] = 'T';
	gsm_buffer[2] = '+';
	gsm_buffer[3] = 'C';
	gsm_buffer[4] = 'M';
	gsm_buffer[5] = 'E';
	gsm_buffer[6] = 'E';
	gsm_buffer[7] = '=';
	gsm_buffer[8] = '1';
	gsm_buffer[9] = '\r';

	buffer_len = 10;

	send_buffer();
	receive_response();

	gsm_buffer[0] = 'A';
	gsm_buffer[1] = 'T';
	gsm_buffer[2] = '+';
	gsm_buffer[3] = 'C';
	gsm_buffer[4] = 'M';
	gsm_buffer[5] = 'G';
	gsm_buffer[6] = 'F';
	gsm_buffer[7] = '=';
	gsm_buffer[8] = '1';
	gsm_buffer[9] = '\r';

	buffer_len = 10;

	send_buffer();
	receive_response();

	return 0;
}

void start_timer()
{
	tmr1_config(pack_tmr1_config(kInstructionClock, kPrescaler1_8));
	tmr1_load(kHalfSecondConstant);
	tmr1_setstate(kEnabled_Sync);
}

bool send_buffer()
{
	uint8 i;

	for (i=0; i<buffer_len; ++i)
	{
		start_timer();	
		while(TXIF == 0 && TMR1IF == 0)
			;
		if ( TMR1IF == 1 )
			return false; // Report an error?

		TXREG = gsm_buffer[i];

		asm("nop");
		asm("nop");
	}
	return true;
}

uint8 peek_rx_buffer_end()
{
	if ( rx_buffer_len == 0 )
		return 0;
	if ( rx_buffer_end == 0 )
		return gsm_rx_buffer[RX_BUFFER_LENGTH-1];
	else
		return gsm_rx_buffer[rx_buffer_end-1];
}

uint8 gsm_receiveone()
{
	start_timer();
	while( !RCIF && TMR1IF == 0 )
		;

	if ( TMR1IF == 1 )
	{
		return 1;
	}

	if ( rx_buffer_len < RX_BUFFER_LENGTH )
	{
		gsm_rx_buffer[rx_buffer_end++] = RCREG;
		++rx_buffer_len;
		if ( rx_buffer_end >= RX_BUFFER_LENGTH )
			rx_buffer_end = 0;
	}
	else
	{
		gsm_rx_buffer[rx_buffer_start++] = RCREG;
		rx_buffer_end = rx_buffer_start;
		if ( rx_buffer_start >= RX_BUFFER_LENGTH )
			rx_buffer_start = 0;
	}
	
	return 0;
}

uint8 receive_response()
{
	if (OERR)
	{
		CREN = 0;
		CREN = 1;
	}
	
	reset_match_counters();
	while( true )
	{
		if ( ok_matched() )
			return 0;
		if ( err_matched() )
			return 1;
		if ( gsm_receiveone() != 0 )
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
	uint8 i = 0;

	while ( rx_buffer_len > 0 && i < kBusMaxMessageSize )
	{
		mib_buffer[i++] = gsm_rx_buffer[rx_buffer_start++];
		if ( rx_buffer_start == RX_BUFFER_LENGTH )
			rx_buffer_start = 0;
		--rx_buffer_len;
	}

	return i;
}

void append_carriage()
{
	gsm_buffer[buffer_len++] = '\r';
}