#include "gprs.h"
#include "gsm.h"
#include "gsm_serial.h"
#include "gsm_defines.h"
#include "mib12_api.h"
#include <xc.h>
#include <string.h>

static char gprs_apn[21];
void gsm_rpc_setapn()
{
	memcpy( gprs_apn, mib_buffer, mib_buffer_length() );
	gprs_apn[mib_buffer_length()] = '\0';
	bus_slave_setreturn(pack_return_status(0,0));
}
bool gprs_connect()
{
	if ( gprs_connected() )
		return true;

	gsm_write_str( "AT+SAPBR=3,1,\"APN\",\"" );
	gsm_write_str(gprs_apn);
	gsm_cmd( "\"" );

	gsm_cmd( "AT+SAPBR=3,1,\"Contype\",\"GPRS\"" );
	__delay_ms(100);
	
	uint8 retry_count = 4;
	do
	{
		if ( gsm_cmd( "AT+SAPBR=1,1" ) == 1 )
			return true;

		__delay_ms(5000);
	} while ( retry_count-- > 0 );
	return false;
}
bool gprs_connected()
{
	gsm_expect( "+SAPBR: 1,1" );
	gsm_expect2( "+SAPBR: 1,3" );
	return gsm_cmd_raw( "AT+SAPBR=2,1", kDEFAULT_CMD_TIMEOUT ) == 1;
}
void gprs_disconnect()
{
	gsm_cmd( "AT+SAPBR=0,1" );
}