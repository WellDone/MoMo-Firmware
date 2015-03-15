#include "sms.h"
#include "gsm.h"
#include "gsm_serial.h"
#include "mib12_api.h"

bool sms_prepare(const char* destination, uint8 length)
{
	gsm_write_str("AT+CMGS=\"");
 	gsm_write(destination, length);
 	gsm_write_str( "\"\r" );
 	gsm_expect( ">" );
 	return gsm_await( 5 );
}
bool sms_send()
{
	gsm_write_char( 0x1A );
 	gsm_expect( "+CMGS" );
 	return gsm_await( 5 ) == 1;
}