//gsm_serial.c

#include "platform.h"
#include "gsm_serial.h"
#include "global_state.h"
#include "mib12_api.h"
#include "gsm_defines.h"
#include "gsm_module.h"
#include "timer1.h"
#include <string.h>

void gsm_serial_init()
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

	CREN = 0; // Make sure any previous transient errors are cleared.
	CREN = 1;
	
	//clear the contents of RCREG
	RCREG;
	RCREG;

	RCIF = 0;

	gsm_rx_clear();
}

void gsm_write_char(char c)
{
	if (OERR)
	{
		CREN = 0;
		CREN = 1;
	}

	tmr1_config(pack_tmr1_config(kInstructionClock, kPrescaler1_8));
	tmr1_load(kHalfSecondConstant);
	tmr1_setstate(kEnabled_Sync);

	while ( !TXIF && TMR1IF == 0 )
		;

	if ( TMR1IF == 1 )
		return;

	TXREG = c;
}

char gsm_rx_peek()
{
	if ( rx_buffer_len == 0 )
		return 0;
	if ( rx_buffer_end == 0 )
		return gsm_rx_buffer[RX_BUFFER_LENGTH-1];
	else
		return gsm_rx_buffer[rx_buffer_end-1];
}
char gsm_rx_pop()
{
	if ( rx_buffer_len == 0 )
		return 0;
	--rx_buffer_len;
	char res = gsm_rx_buffer[rx_buffer_start++];
	if ( rx_buffer_start == RX_BUFFER_LENGTH )
		rx_buffer_start = 0;
	return res;
}

void gsm_rx_clear()
{
	rx_buffer_start = rx_buffer_end = rx_buffer_len = 0;
}

bool gsm_rx()
{
	if (OERR)
	{
		CREN = 0;
		CREN = 1;
	}
	
	tmr1_config(pack_tmr1_config(kInstructionClock, kPrescaler1_8));
	tmr1_load(kHalfSecondConstant);
	tmr1_setstate(kEnabled_Sync);

	while( !RCIF && TMR1IF == 0 )
		;

	if ( TMR1IF == 1 )
		return false;

	if ( rx_buffer_len < RX_BUFFER_LENGTH )
	{
		gsm_rx_buffer[rx_buffer_end++] = RCREG;
		if ( rx_buffer_end >= RX_BUFFER_LENGTH )
			rx_buffer_end = 0;
		++rx_buffer_len;
	}
	else
	{
		gsm_rx_buffer[rx_buffer_start++] = RCREG;
		rx_buffer_end = rx_buffer_start;
		if ( rx_buffer_start >= RX_BUFFER_LENGTH )
			rx_buffer_start = 0;
	}
	
	return true;
}

const char* expected_response1;
const char* expected_response2;
const char* expectation_counter1;
const char* expectation_counter2;
void gsm_expect( const char* response )
{
	expected_response1 = response;
	expected_response2 = NULL;
}
void gsm_expect2( const char* response )
{
	expected_response2 = response;
}
uint8 gsm_await( uint8 timeout_s ) // NOTE: This function is time-sensitive
{
	expectation_counter1 = expected_response1;
	expectation_counter2 = expected_response2;
	
	timeout_s *= 2; // Each RX timeout is .5s
	while ( !gsm_rx() )
	{
		if ( timeout_s-- == 0 )
			return GSM_SERIAL_NODATA;
	}

	do
	{
		if ( *expectation_counter1 != gsm_rx_peek() )
			expectation_counter1 = expected_response1;
		else if ( *(++expectation_counter1) == '\0' )
			return 1;

		if ( expected_response2 != NULL )
		{
			if ( *expectation_counter2 != gsm_rx_peek() )
				expectation_counter2 = expected_response2;
			else if ( *(++expectation_counter2) == '\0' )
				return 2;
		}
	}
	while ( gsm_rx() || gsm_rx() ); // Try twice to get a char, to be a little more robust.
	
	return GSM_SERIAL_TIMEDOUT;
}

uint8 gsm_readback( char* buf, uint8 max_len )
{
	uint8 i = 0;
	while ( i < max_len && rx_buffer_len != 0 )
	{
		buf[i++] = gsm_rx_pop();
	}
	return i;
}
uint8 gsm_read( char* buf, uint8 max_len )
{
	uint8 timeout_counter = GSM_EXPECT_TIMEOUT;
	while ( !gsm_rx() )
	{
		if ( --timeout_counter == 0 )
			return GSM_SERIAL_NODATA;
	}

	uint8 len = 0;
	do
	{
		if ( buf != NULL )
			*(buf++) = gsm_rx_peek();
		if ( ++len == max_len )
			break;
	}
	while ( gsm_rx() );

	return len;
}

void gsm_write_carriage()
{
	gsm_write_char('\r');
}
void gsm_write(const char* buf, uint8 len)
{
	while ( len-- > 0 )
	{
		gsm_write_char( *(buf++) );
	}
}
void gsm_write_str(const char* str)
{
	gsm_write( str, strlen( str ) );
}