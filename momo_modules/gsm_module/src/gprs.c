#include "gprs.h"
#include "gsm.h"
#include "gsm_serial.h"
#include "gsm_defines.h"
#include "mib12_api.h"
#include <xc.h>
#include <string.h>
#include "buffers.h"

void gsm_rpc_setapn()
{
	gprs_set_apn();
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
		//Need to wait a longer time for this command because it can take > 3s to return
		gsm_expect_ok_error();
		if ( gsm_cmd_raw( "AT+SAPBR=1,1", GPRS_CONNECT_TIMEOUT) == 1)
		{
			//Make sure there's a guard band around the this command before sending any others
			__delay_ms(100);
			return true;
		}

		__delay_ms(5000);
	} while ( --retry_count > 0 );

	
	return false;
}
bool gprs_connected()
{
	uint8 result;
	gsm_expect( "+SAPBR: 1,1" );
	gsm_expect2( "+SAPBR: 1,3" );
	result = gsm_cmd_raw( "AT+SAPBR=2,1", kDEFAULT_CMD_TIMEOUT );
	__delay_ms(100);

	return result == 1;
}
void gprs_disconnect()
{
	gsm_cmd( "AT+SAPBR=0,1" );
}