/* gsmstream.c
 *
 * Endpoints for opening and dumping data to a text message stream.
 *
 *
 *
 */

#include "gsmstream.h"

#include "mib12_api.h"
#include "gsm_serial.h"
#include "gsm.h"
#include "global_state.h"
#include "simcard.h"
#include "sms.h"
#include "gprs.h"
#include "http.h"
#include "gsm_defines.h"
#include <string.h>

//Defined in buffers.as
extern char comm_destination[65];

void gsm_rpc_setcommdestination()
{
	if (set_comm_destination() == 1)
		bus_slave_setreturn(pack_return_status(7,0));
	else
		bus_slave_setreturn(pack_return_status(0,0));
}

 void gsm_openstream()
 {
 	if (!gsm_on())
 	{
 		bus_slave_setreturn(pack_return_status(6,0));
 		return;
 	}

 	if (simdet_detect() == 0)
 	{
 		bus_slave_setreturn(pack_return_status(6,0));
 		return;
 	}

 	if ( state.stream_in_progress )
 	{
 		bus_slave_setreturn(pack_return_status(7,0)); //TODO: Busy MIB status code
 		return;
 	}
 	state.stream_in_progress = 1;

 	if ( gsm_register() )
 	{
 		gsm_remember_band(); // Times out after 4 minutes

 		if (comm_destination_get(0) == '+' )
	 	{
	 		state.stream_type = kStreamSMS;
	 		if ( !sms_prepare( comm_destination, strlen(comm_destination) ) )
	 		{
	 			state.stream_in_progress = 0;
	 		}
	 	}
	 	else
	 	{	
	 		state.stream_type = kStreamGPRS;
	 		if ( !gprs_connect()
	 			|| !http_init()
	 			|| !http_write_prepare( plist_get_int16(0) ) )
	 		{
	 			state.stream_in_progress = 0;
	 		}
	 	}
 	}
 	else
 	{
 		gsm_forget_band();
 		state.stream_in_progress = 0;
 	}

 	if ( state.stream_in_progress == 0 )
 	{
 		gsm_off();
		bus_slave_setreturn( pack_return_status(6, 0) );
 	}
 	else
 	{
 		bus_slave_setreturn(pack_return_status(0,0));
 	}
 }

 void gsm_putstream()
 {
 	if ( !state.stream_in_progress )
 	{
 		bus_slave_setreturn(pack_return_status(7,0));
 		return;
 	}

 	gsm_write( mib_buffer, mib_buffer_length() );
 	__delay_ms(1);

 	bus_slave_setreturn(pack_return_status(0,0));
 }

 void gsm_closestream()
 {
 	if ( !state.stream_in_progress )
 	{
 		bus_slave_setreturn(pack_return_status(7,0));
 		return;
 	}

 	uint8 result = 0, timeout_10s = 8*6; // 8 minutes
	if ( state.stream_type == kStreamSMS )
	{
		if ( sms_send() )
		{
	 		gsm_off();
	 		bus_slave_setreturn(pack_return_status(0,0));
	 		return;
		}

		gsm_expect( "+CMGS:" );
		gsm_expect2( "ERROR" );
		do
		{
			result = gsm_await( 10 );
			if ( result == 1 || --timeout_10s == 0 )
				break;
		}
		while ( true );	
	}
	else
	{
		if ( !http_post(comm_destination) )
		{
			gsm_off();
			bus_slave_setreturn(pack_return_status(6,0));
			return;
		}
		do
		{
			result = http_await_response();

			if ( result || --timeout_10s == 0 )
				break;
		}
		while (true);
		
		result = ( ( result && http_status() == 200 )? 1 : 2 );

		while ( http_read( NULL, 20 ) != 0 )
			continue;
	}

	state.stream_in_progress = 0;
	state.callback_pending = 1;
	state.stream_success = ( result == 1 ) ? 1 : 0;
	bus_slave_setreturn(pack_return_status(0,0));
	gsm_off();

		// if ( result == 2 && state.stream_type == kStreamSMS )
		// {
		// 	capture_error();
		// }
		// else
		// {
		// 	uint_buf[5] = '\0';
			
		// 	strcpy( mib_buffer, "GPRS ERROR : " );
		// 	strcpy( mib_buffer+13, uint_buf );

		// 	bus_master_begin_rpc();
		// 	bus_master_prepare_rpc( 42, 0x20, plist_with_buffer( 0, 13+strlen(uint_buf) ) );
		// 	bus_master_send_rpc( 8 );
		// }
 }

 void gsm_abandonstream()
 {
 	gsm_off();
 	bus_slave_setreturn(pack_return_status(0,0));
 }