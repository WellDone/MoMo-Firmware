//app_main.c

#include "platform.h"
#include "protocol.h"
#include "gsm_defines.h"
#include "mib12_api.h"
#include "watchdog.h"
#include "gsm_serial.h"
#include "gsm.h"
#include "gprs.h"
#include "http.h"
#include "simcard.h"
#include "global_state.h"
#include "port.h"
#include "intel_hex.h"
#include "buffers.h"
#include <string.h>

static void report_result(bool success)
{
	bus_master_begin_rpc();
	mib_packet.param_spec = plist_empty();
	mib_packet.feature = 60;
	mib_packet.command = ( success )? 0xF0 : 0xF1;
	bus_master_send_rpc(8);
}

static void capture_error(void)
{
	uint8 len = gsm_read( mib_buffer, kBusMaxMessageSize );
	if ( len == 0 )
		return;

	bus_master_begin_rpc();
	bus_master_prepare_rpc( 42, 0x20, plist_with_buffer( 0, len ) );
	bus_master_send_rpc( 8 );
}

extern char* uint_buf;
void task(void)
{
	wdt_disable();
	
	//Don't sleep while the module's on so that we don't miss a
	//serial message
	while(state.module_on)
	{
		gsm_rx();

		if ( state.shutdown_pending )
		{
			uint8 result, timeout_10s = 8*6; // 8 minutes
			if ( state.stream_type == kStreamSMS )
			{
				gsm_expect( "+CMGS:" );
				gsm_expect2( "ERROR" );
				do
				{
					result = gsm_await( 10 );
					if ( --timeout_10s == 0 )
						break;
				}
				while ( true );	
			}
			else
			{
				do
				{
					result = http_await_response( 10 );
					if ( result || --timeout_10s == 0 )
						break;
				}
				while (true);

				result = ( ( result && http_status() == 200 )? 1 : 2 );
			}

			report_result( result == 1 );
			if ( result == 2 && state.stream_type == kStreamSMS )
			{
				capture_error();
			}
			else
			{
				uint_buf[5] = '\0';
				
				strcpy( mib_buffer, "GPRS ERROR : " );
				strcpy( mib_buffer+13, uint_buf );

				bus_master_begin_rpc();
				bus_master_prepare_rpc( 42, 0x20, plist_with_buffer( 0, 13+strlen(uint_buf) ) );
				bus_master_send_rpc( 8 );
			}
			gsm_off();
		}
	}
}

void interrupt_handler(void)
{

}

void initialize(void)
{	
	debug_val = 0;
	gsm_init();
}

//MIB Endpoints
void gsm_rpc_on()
{
	wdt_disable();
	
	mib_buffer[0] = gsm_on();
	mib_buffer[1] = 0;

	bus_slave_setreturn(pack_return_status(0,2));
}
void gsm_rpc_off()
{
	gsm_off();
	bus_slave_setreturn(pack_return_status(0,0));
}

void gsm_rpc_power_on()
{
	ENSURE_DIGITAL(MODULEPOWERPIN);
	DRIVE_HI(MODULEPOWERPIN);
	bus_slave_setreturn(pack_return_status(0,0));
}

void gsm_rpc_power_off()
{
	ENSURE_DIGITAL(MODULEPOWERPIN);
	DRIVE_LOW(MODULEPOWERPIN);
	bus_slave_setreturn(pack_return_status(0,0));	
}

void gsm_rpc_testsim()
{
	mib_buffer[0] = simdet_detect();
	mib_buffer[1] = 0;

	bus_slave_setreturn(pack_return_status(0,2));	
}

void gsm_rpc_dumpbuffer()
{
	bus_slave_setreturn(pack_return_status(0, gsm_readback( mib_buffer, kBusMaxMessageSize ) ));
}
void gsm_rpc_debug()
{
	mib_buffer[0] = state.module_on;
	mib_buffer[1] = state.shutdown_pending;
	mib_buffer[2] = rx_buffer_start;
	mib_buffer[3] = rx_buffer_end;
	mib_buffer[4] = debug_val;

	bus_slave_setreturn(pack_return_status(0, 5));
}

void gsm_rpc_download()
{	
	// gprs_set_apn( "wap.cingular", 12 );
	// if ( !gsm_registered() )
	// {
	// 	bus_slave_setreturn(pack_return_status(7, 0));
	// 	return;
	// }
	// if ( !gprs_connect() //TODO: Retry?
	// 	|| !http_init() )
	// {
	// 	bus_slave_setreturn(pack_return_status(6, 0));
	// 	return;
	// }

	// http_write_start(4);
	// http_write("test", 4);

	// if ( !http_post("http://requestb.in/14ylomw1") || http_status() != 200 )
	// {
	// 	http_term();
	// 	gprs_disconnect();
	// 	bus_slave_setreturn(pack_return_status(6, 0));
	// 	return;
	// }

	// uint8 len;
	// do
	// {
	// 	len = http_read( mib_buffer, kBusMaxMessageSize ); // Just read through the entire document, don't return anything.
	// } while (len > 0);

	// http_term();
	// gprs_disconnect();

	// bus_slave_setreturn(pack_return_status(0, 0));
}

void gsm_rpc_sendcommand()
{
	if (state.module_on)
	{
		gsm_write( mib_buffer, mib_buffer_length() );
		uint8 result = gsm_cmd( "" );
		if ( result == GSM_SERIAL_TIMEDOUT || result == GSM_SERIAL_NODATA )
		{
			bus_slave_setreturn( pack_return_status(6,0) );
			return;
		}

		gsm_rpc_dumpbuffer();
		return;
	}

	bus_slave_setreturn(pack_return_status(7,0));
}

/*
 * Do not use, required to be defined by startup code that xc8 apparently cannot *NOT* link in even though it is optimized away later.
 */	
void main()
{
	
}