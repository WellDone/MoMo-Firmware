#include "gprs.h"
#include "gsm.h"
#include "gsm_serial.h"
#include "gsm_defines.h"
#include "global_state.h"
#include "mib12_api.h"
#include <xc.h>
#include <string.h>
#include "buffers.h"
#include "protocol.h"

static uint8_t gprs_registered();
static uint8_t gprs_connected();

uint8_t gsm_rpc_setapn()
{
	gprs_set_apn();
	
	return kNoErrorStatus;
}

GSMError gprs_connect()
{
	GSMError error;

	if (gprs_connected())
		return kNoGSMError;

	error = gprs_ensure_registered();
	if (error != kNoGSMError)
		return error;

	if (gsm_cmd( "AT+SAPBR=3,1,\"Contype\",\"GPRS\"" ) != kCMDOK)
		return kGPRSErrorSettingBearerType;

	__delay_ms(100);

	gsm_write_str( "AT+SAPBR=3,1,\"APN\",\"" );
	gsm_write_str(gprs_apn);
	if (gsm_cmd( "\"" ) != kCMDOK)
		return kGPRSErrorSettingAPN;

	__delay_ms(100);

	gsm_expect_ok_error();
	if (gsm_cmd_raw("AT+SAPBR=1,1", GPRS_CONNECT_TIMEOUT) != kCMDOK)
		return kGPRSTimeoutWaitingBearer;
	
	//Make sure there's a guard band around the this command before sending any others
	__delay_ms(100);
	return kNoGSMError;
}

GSMError gprs_ensure_registered()
{
	uint8_t timeout_s = GPRS_REGISTRATION_TIMEOUT;

	if (connection_status == kModuleOff)
		return kModuleNotOnWhenExpected;

	while (!gprs_registered())
	{
		if ( timeout_s-- == 0 )
			return kGPRSTimeoutAwaitingCGREG;

		__delay_ms(1000);
	}

	connection_status = kModuleOnDataOn;
	return kNoGSMError;
}

GSMError gprs_disconnect()
{
	if (gsm_cmd( "AT+SAPBR=0,1" ) != kCMDOK)
		return kGPRSErrorDisconnecting;

	return kNoGSMError;
}

static uint8_t gprs_connected()
{
	uint8_t result;
	gsm_expect( "+SAPBR: 1,1" );
	gsm_expect2( "+SAPBR: 1,3" );
	result = gsm_cmd_raw( "AT+SAPBR=2,1", kDEFAULT_CMD_TIMEOUT );
	__delay_ms(100);

	return result == 1;
}

static uint8_t gprs_registered()
{
	gsm_expect( "+CGREG: 0,1" ); // Registered, home network
	gsm_expect2( "+CGREG: 0,5" ); // Registered, roaming
	uint8_t result = gsm_cmd_raw( "AT+CGREG?", kDEFAULT_CMD_TIMEOUT );

	__delay_ms(100);
	
	return (result == 1 || result == 2);
}
