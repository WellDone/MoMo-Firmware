#include "gsm.h"

#include "gsm_module.h"
#include "gsm_serial.h"
#include "global_state.h"
#include "simcard.h"

void gsm_init()
{
	gsm_module_off();
	simdet_idle();
	gsm_rx_clear();
}
bool gsm_on()
{
	if ( gsm_module_active() )
		return true;

	if ( !gsm_module_on()
		|| gsm_cmd( "ATE0" ) != kCMDOK
		|| gsm_cmd( "AT+CMEE=1" ) != kCMDOK
		|| gsm_cmd( "AT+CMGF=1" ) != kCMDOK )
	{
		gsm_module_off();
		return false;
	}

	gsm_rx_clear();
	return true;
}

bool gsm_register( uint8 timeout_s )
{
	while ( !gsm_registered() )
	{
		if ( timeout_s-- == 0 )
			return false;
		__delay_ms( 1000 );
	}
	return true;
}
bool gsm_registered()
{
	gsm_expect( "+CREG: 0,1" ); // Registered, home network
	gsm_expect2( "+CREG: 0,5" ); // Registered, roaming
	uint8 result = gsm_cmd_raw( "AT+CREG?", kDEFAULT_CMD_TIMEOUT );
	return result == 1 || result == 2;
}
uint8 gsm_cmd(const char* cmd)
{
	gsm_expect_ok_error();
	gsm_write_str(cmd);
	gsm_write_char('\r');
	return gsm_await( kDEFAULT_CMD_TIMEOUT );
}
uint8 gsm_cmd_raw(const char* cmd, uint8 timeout)
{
	gsm_write_str(cmd);
	gsm_write_char('\r');
	return gsm_await( timeout );
}
void gsm_off()
{
	gsm_module_off();
	simdet_idle();
}