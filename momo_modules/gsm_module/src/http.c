#include "http.h"
#include "gsm_serial.h"
#include "gsm.h"
#include "global_state.h"
#include <stdlib.h>

uint16 http_read_start;
char uint_buf[6];
uint16 response_status;

void status_atoi( const char* str )
{
	response_status = 0;
	while ( *str != 0 )
	{
		response_status *= 10;
		response_status += *str - '0';
		++str;
	}
}

bool http_init()
{
	return gsm_cmd( "AT+HTTPINIT" ) == kCMDOK;
}

bool http_await_response( uint8 seconds ) {
	gsm_expect( "+HTTPACTION:" );
	char* ptr = uint_buf;
	if ( gsm_await( seconds ) != 1 )
		return false;
	gsm_rx(); // '1', '2', or '3'
	gsm_rx(); // ','
	while ( gsm_rx() && (ptr < (uint_buf+5)))
	{
		if ( gsm_rx_peek() == ',' )
			break;
		*(ptr++) = gsm_rx_peek();
	}

	while (gsm_rx())
	{
		if ( gsm_rx_peek() == '\r' )
			break;
	}

	*ptr = 0;
	status_atoi( uint_buf );
	debug_val = response_status & 0xFF;

	return true;
}

bool http_head(const char* url)
{
	http_read_start = 0;
	gsm_cmd( "AT+HTTPPARA=\"CID\",1" );
	
	gsm_write_str( "AT+HTTPPARA=\"URL\",\"" );
	gsm_write_str( url );
	gsm_cmd( "\"" );

	gsm_cmd( "AT+HTTPPARA=\"REDIR\",1" );

	if ( gsm_cmd( "AT+HTTPACTION=2" ) != kCMDOK )
		return false;
	return true;
}

bool http_get(const char* url)
{
	http_read_start = 0;
	gsm_cmd( "AT+HTTPPARA=\"CID\",1" );
	
	gsm_write_str( "AT+HTTPPARA=\"URL\",\"" );
	gsm_write_str( url );
	gsm_cmd( "\"" );

	gsm_cmd( "AT+HTTPPARA=\"REDIR\",1" );

	if ( gsm_cmd( "AT+HTTPACTION=0" ) != kCMDOK )
		return false;
	return true;
}

bool http_write_prepare(uint16 len)
{
	utoa( uint_buf, len, 10 );
	gsm_expect( "DOWNLOAD" );
	gsm_write_str( "AT+HTTPDATA=" );
	gsm_write_str( uint_buf );
	return gsm_cmd_raw( ",10000", 10 ) == 1;
}
bool http_post(const char* url)
{
	http_read_start = 0;
	gsm_cmd( "AT+HTTPPARA=\"CID\",1" );
	
	gsm_write_str( "AT+HTTPPARA=\"URL\",\"" );
	gsm_write_str( url );
	gsm_cmd( "\"" );

	gsm_cmd( "AT+HTTPPARA=\"REDIR\",1" );

	if ( gsm_cmd( "AT+HTTPACTION=1" ) != kCMDOK )
		return false;
	return true;
}
uint8 http_read(char* out, uint8 out_len)
{
	gsm_write_str( "AT+HTTPREAD=" );
	utoa( uint_buf, http_read_start, 10 );
	gsm_write_str( uint_buf );
	gsm_write_char( ',' );
	utoa( uint_buf, out_len, 10 );
	gsm_write_str( uint_buf );
	http_read_start += out_len;

	gsm_expect( "+HTTPREAD:" );
	if ( gsm_cmd_raw( "", 10 ) != 1 )
	{
		return 0;
	}
	char* ptr = uint_buf;
	while ( gsm_rx() )
	{
		if ( gsm_rx_peek() == '\r' )
			break;
		*(ptr++) = gsm_rx_peek();
	}
	gsm_rx(); // read the \n
	*ptr = '\0';
	uint8 len = atoi( uint_buf );
	if ( len > out_len )
		return 0;
	if ( gsm_read( out, len ) != len )
		return 0;
	return len;
}

uint8 http_status()
{
	return response_status;
}

void http_term()
{
	gsm_cmd( "AT+HTTPTERM" );
}