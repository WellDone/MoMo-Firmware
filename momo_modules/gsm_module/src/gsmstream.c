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
#include <string.h>

char comm_destination[65] = { '\0' };

void gsm_rpc_setcommdestination()
{
	if ( plist_get_int8(0) + mib_buffer_length() > 64 )
		bus_slave_setreturn(pack_return_status(7,0));

	memcpy( comm_destination + plist_get_int8(0), mib_buffer+2, mib_buffer_length() );
	comm_destination[plist_get_int8(0) + mib_buffer_length()] = '\0';
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

 	if ( gsm_register( 60 ) )
 	{
 		if ( comm_destination[0] == '+' )
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

 	bus_slave_setreturn(pack_return_status(0,0));
 }

 void gsm_closestream()
 {
 	
 	state.stream_in_progress = 0; //TODO: Prevent new streams until the message has actually sent?

	if ( state.stream_type == kStreamSMS )
	{
		if ( sms_send() )
	 		gsm_off();
	 	else
	 		state.shutdown_pending = 1;  // Shutdown the module after we've sent the message (or timed out)
	 	bus_slave_setreturn(pack_return_status(0,0));
	}
	else
	{
		if ( !http_post(comm_destination) )
		{
			gsm_off();
			bus_slave_setreturn(pack_return_status(6,0));
		}
		else
		{
			state.shutdown_pending = 1;
			bus_slave_setreturn(pack_return_status(0,0));
		}
	}
 }

 void gsm_abandonstream()
 {
 	gsm_off();
 	bus_slave_setreturn(pack_return_status(0,0));
 }