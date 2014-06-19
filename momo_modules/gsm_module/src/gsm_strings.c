#include "gsm_strings.h"
#include "gsm_serial.h"

uint8 ok_counter;
uint8 err_counter;
uint8 cmgs_counter;
uint8 creg_counter;

void reset_match_counters()
{
	ok_counter = err_counter = cmgs_counter = creg_counter = 0;
}
bool cmgs_matched()
{
	if ( cmgs_counter == 6 )
		return true;
	uint8 c = peek_rx_buffer_end();
	if ( ( cmgs_counter == 0 && c == '+' )
		|| ( cmgs_counter == 1 && c == 'C' )
		|| ( cmgs_counter == 2 && c == 'M' )
		|| ( cmgs_counter == 3 && c == 'G' )
		|| ( cmgs_counter == 4 && c == 'S' )
		|| ( cmgs_counter == 5 && c == ':' ) )
		++cmgs_counter;
	else
		cmgs_counter = 0;
	return false;
}
bool creg_matched()
{
	if ( creg_counter == 3 )
		return true;
	uint8 c = peek_rx_buffer_end();
	if ( ( creg_counter == 0 && c == '0' )
		|| ( creg_counter == 1 && c == ',' )
		|| ( creg_counter == 2 && c == '1' ) )
		++creg_counter;
	else
		creg_counter = 0;
	return false;
}

bool ok_matched()
{
	if ( ok_counter == 2 )
		return true;
	uint8 c = peek_rx_buffer_end();
	if ( ( ok_counter == 0 && c == 'O' )
		|| ( ok_counter == 1 && c == 'K' ) )
		++ok_counter;
	else
		ok_counter = 0;
	return false;
}

bool err_matched()
{
	if ( err_counter == 5 )
		return true;
	uint8 c = peek_rx_buffer_end();
	if ( ( err_counter == 0 && c == 'E' )
		|| ( err_counter == 1 && c == 'R' )
		|| ( err_counter == 2 && c == 'R' )
		|| ( err_counter == 3 && c == 'O' )
		|| ( err_counter == 4 && c == 'R' ) )
		++err_counter;
	else
		err_counter = 0;
	return false;
}