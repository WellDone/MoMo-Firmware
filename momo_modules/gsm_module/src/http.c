#include "http.h"
#include "gsm_serial.h"
#include "gsm.h"
#include "global_state.h"
#include <stdlib.h>

uint16 http_read_start;
char http_buf[32];
uint16 http_response_status;

void status_atoi( const char* str )
{
	http_response_status = 0;
	while ( *str >= '0' && *str <= '9' )
	{
		http_response_status *= 10;
		http_response_status += *str - '0';
		++str;
	}
}

bool http_init()
{
	//TODO: HTTPS
	
	return gsm_cmd( "AT+HTTPINIT" ) == kCMDOK;
}

bool http_await_response()
{
	gsm_capture_remainder( http_buf, sizeof(http_buf)-1 );
	gsm_expect( "+HTTPACTION:" );
	if ( gsm_await( 10 ) != 1 )
		return false;
	
	// Skip the first two remainder characters, which are:
	//   '1', '2', or '3'
	//   ','
	status_atoi( http_buf + 2 );
	
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
	utoa( http_buf, len, 10 );
	gsm_expect( "DOWNLOAD" );
	gsm_write_str( "AT+HTTPDATA=" );
	gsm_write_str( http_buf );
	return gsm_cmd_raw( ",10000", 10 ) == 1;
}
bool http_post(const char* url)
{
	http_read_start = 0;
	gsm_cmd( "AT+HTTPPARA=\"CID\",1" );
	gsm_cmd( "AT+HTTPPARA=\"CONTENT\",\"text/plain\"" );
	
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
	//CAUTION: Untested!
	
	if ( out_len > ( sizeof(http_buf) - 4 ) )
		out_len = sizeof(http_buf) - 4; // two digits for the length, \r\n

	gsm_write_str( "AT+HTTPREAD=" );
	utoa( http_buf, http_read_start, 10 );
	gsm_write_str( http_buf );
	gsm_write_char( ',' );
	utoa( http_buf, out_len, 10 );
	gsm_write_str( http_buf );
	http_read_start += out_len;

	gsm_capture_remainder( http_buf, sizeof(http_buf) );
	gsm_expect( "+HTTPREAD:" );
	gsm_expect2( "OK" );
	if ( gsm_cmd_raw( "", 10 ) != 1 )
	{
		return 0;
	}

	if ( !out )
		return out_len;

	uint8 i = 0;
	while ( http_buf[i++] != '\r' )
		continue;
	http_buf[i] = '\0';
	out_len = atoi( http_buf );
	uint8 buf_len = out_len + i;
	if ( buf_len > ( sizeof(http_buf) - 4 ) )
		return 0;

	while ( i < buf_len )
	{
		*(out++) = http_buf[i];
	}
	return out_len;
}

uint8 http_status()
{
	return http_response_status;
}

void http_term()
{
	gsm_cmd( "AT+HTTPTERM" );
}