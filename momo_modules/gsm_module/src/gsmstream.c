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
#include "protocol_defines.h"

//Defined in buffers.as
extern char comm_destination[65];

uint8_t gsm_rpc_setcommdestination()
{
	if (set_comm_destination() == 1)
		return 7;
	
	return kNoErrorStatus;	
}

uint8_t gsm_openstream()
{
	if (state.stream_in_progress)
	{
		gsm_off(); // Prioritize later streams in case we got hung-up somehow.
		__delay_ms(1000);
	}

	state.stream_in_progress = 1;

	if (gsm_ensure_on() == 0)
		return 1;

	if (simdet_detect() == 0)
		return 2;

	if (gsm_ensure_registered() != 0)
	{
		gsm_remember_band(); // Timeout after 2 minutes

		if (comm_destination_get(0) == '+' )
 		{
	 		state.stream_type = kStreamSMS;
	 		if (!sms_prepare( comm_destination, strlen(comm_destination)))
	 			state.stream_in_progress = 0;
	 	}
 	 	else
	 	{	
	 		state.stream_type = kStreamGPRS;
	 		if ( !gprs_connect()
	 			|| !http_init()
	 			|| !http_write_prepare( *((uint16_t *)mib_buffer ) ))
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

	if (state.stream_in_progress == 0)
	{
		gsm_off();
		return 3;
	}
	
	return 0;
}

uint8_t gsm_putstream()
{
	if (!state.stream_in_progress)
		return 7;

	gsm_write(mib_buffer, async_length);
	__delay_ms(1);

	return 0;
}

uint8_t gsm_closestream()
{
 	uint8_t result = 0, timeout_10s = 8*6; // 8 minutes

 	if ( !state.stream_in_progress )
 		return 7;

	if (state.stream_type == kStreamSMS)
	{
		if (sms_send())
		{
	 		gsm_off();
	 		return 0;
		}

		gsm_expect( "+CMGS:" );
		gsm_expect2( "ERROR" );

		do
		{
			result = gsm_await(10);
			if ( result == 1 || --timeout_10s == 0 )
				break;
		}
		while (true);
	}
	else
	{
		if (!http_post(comm_destination))
		{
			gsm_off();
			return 6;
		}

		do
		{
			result = http_await_response();

			if (result || --timeout_10s == 0)
				break;
		}
		while (true);
		
		result = ( ( result && http_status() == 200 )? 1 : 2 );
		
		// TODO: do something with the result
	}

	state.stream_in_progress = 0;
	state.callback_pending = 1;
	state.stream_success = ( result == 1 ) ? 1 : 0;
	
	gsm_off();
	return 0;
}

