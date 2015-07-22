#include "gsm.h"

#include "gsm_module.h"
#include "gsm_serial.h"
#include "gprs.h"
#include "global_state.h"
#include "simcard.h"

char sticky_band[21];

static uint8_t gsm_on();
static uint8_t gsm_registered();

void gsm_init()
{
	sticky_band[0] ='\0';
	gsm_module_off();
	simdet_idle();
	gsm_rx_clear();
	gprs_init_buffers();
}

uint8_t gsm_ensure_on()
{
	uint8_t status;
	if (connection_status < kModuleOnNotConnected)
	{
		status = gsm_on();
		if (status == true)
			connection_status = kModuleOnNotConnected;

		return status;
	}

	return 2;
}

static uint8_t gsm_on()
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

	__delay_ms(1000);

	gsm_rx_clear();
	gsm_recall_band();
	return true;
}

static uint8_t gsm_registered()
{
	gsm_expect( "+CREG: 0,1" ); // Registered, home network
	gsm_expect2( "+CREG: 0,5" ); // Registered, roaming
	uint8_t result = gsm_cmd_raw( "AT+CREG?", kDEFAULT_CMD_TIMEOUT );

	__delay_ms(100);
	
	return result == 1 || result == 2;
}

uint8_t gsm_ensure_registered()
{
	uint8_t timeout_s = GSM_REGISTRATION_TIMEOUT_S;

	if (connection_status > kModuleOnNotConnected)
		return 2;

	while ( !gsm_registered() )
	{
		if ( timeout_s-- == 0 )
			return 0;

		__delay_ms( 1000 );
	}

	connection_status = kModuleOnConnected;
	return 1;
}

void gsm_remember_band()
{
	gsm_capture_remainder( sticky_band, sizeof(sticky_band) );
	gsm_expect( "+CBAND: " );
	gsm_cmd_raw( "AT+CBAND?" , kDEFAULT_CMD_TIMEOUT );
	
	uint8_t i = 0;
	while ( sticky_band[i] != '\0' && sticky_band[i] != ',' && sticky_band[i] != '\r' && i++ < sizeof(sticky_band)-1 )
		continue;
	sticky_band[i] = '\0';
}
void gsm_recall_band()
{
	if ( sticky_band[0] != '\0' )
	{
		gsm_write_str("AT+CBAND=\"");
		gsm_write_str(sticky_band);
		if ( !gsm_cmd("\"") )
		{
			gsm_forget_band();
		}
	}
}
void gsm_forget_band()
{
	sticky_band[0] = 'A';
	sticky_band[1] = 'L';
	sticky_band[2] = 'L';
	sticky_band[3] = '_';
	sticky_band[4] = 'B';
	sticky_band[5] = 'A';
	sticky_band[6] = 'N';
	sticky_band[7] = 'D';
	sticky_band[8] = '\0';
}

uint8_t gsm_cmd(const char* cmd)
{
	//Clear out rcreg so that when we call gsm_await we don't start our timeout
	//immediately.
	gsm_clear_receive();

	uint8_t retval;
	gsm_expect_ok_error();
	gsm_write_str(cmd);
	gsm_write_char('\r');
	retval = gsm_await(kDEFAULT_CMD_TIMEOUT);

	//Make sure we can't send commands too fast to the modem
	__delay_ms(100);
	return retval;
}

uint8_t gsm_cmd_raw(const char* cmd, uint8_t timeout)
{
	gsm_clear_receive();

	gsm_write_str(cmd);
	gsm_write_char('\r');
	return gsm_await( timeout );
}
void gsm_off()
{
	gsm_module_off();
	simdet_idle();
}
