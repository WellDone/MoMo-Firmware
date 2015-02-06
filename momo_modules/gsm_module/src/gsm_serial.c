//gsm_serial.c

#include "platform.h"
#include "gsm_serial.h"
#include "global_state.h"
#include "mib12_api.h"
#include "gsm_defines.h"
#include "gsm_module.h"
#include "timer1.h"
#include "buffers.h"
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
	SPBRGL = 51;

	CREN = 0; // Make sure any previous transient errors are cleared.
	CREN = 1;
	
	//clear the contents of RCREG
	RCREG;
	RCREG;

	RCIF = 0;

	gsm_rx_clear();
}

void gsm_clear_receive()
{
	if (OERR)
	{
		CREN = 0;
		CREN = 1;
	}

	while(RCIF)
		RCREG;
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

	gsm_rx_push(RCREG);
	
	return true;
}

extern const char* expected1;
extern const char* expected2;

void gsm_expect(const char* response)
{
	expected1 = response;
	expected2 = response;
}

void gsm_expect2(const char* response)
{
	expected2 = response;
}

void gsm_expect_ok_error()
{
	gsm_expect("OK");
	gsm_expect2("ERROR");
}

uint8 gsm_await( uint8 timeout_s ) // NOTE: This function is time-sensitive
{
	reset_expected1_ptr();
	reset_expected2_ptr();
	
	timeout_s *= 2; // Each RX timeout is .5s
	while ( !gsm_rx() )
	{
		if ( timeout_s-- == 0 )
			return GSM_SERIAL_NODATA;
	}

	do
	{
		uint8 value = gsm_check(gsm_rx_peek());
		if (value != 0)
		{
			// if (value == 2)
			// {
			// 	uint8 rcount = 0;
			// 	while(gsm_rx())
			// 	{
			// 		if (gsm_rx_peek() == '\r')
			// 			++rcount;

			// 		if (rcount == 2)
			// 			return value;
			// 	}
			// }

			return value;
		}
	}
	while ( gsm_rx() || gsm_rx() ); // Try twice to get a char, to be a little more robust.
	
	return GSM_SERIAL_TIMEDOUT;
}

/* 
 * Check if this character matches the current position of one of the variables
 * we are looking for.  We have to check a second time if it does not match since
 * it may match the first character but we were on the 3rd character, for example.
 */
uint8 gsm_check(uint8 current)
{
	uint8 result;

	result = check_inc_expected1(current);
	if (result == 0xFF)
		result = check_inc_expected1(current);

	if (result == 0)
		return 1;

	result = check_inc_expected2(current);
	if (result == 0xFF)
		result = check_inc_expected2(current);

	if (result == 0)
		return 2;

	return 0;
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
	while (*str)
		gsm_write_char(*str++);
}