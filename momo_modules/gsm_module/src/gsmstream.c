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

GSMError gsm_openstream()
{
	GSMError result;

	//FIXME: just return an error code here
	if (state.stream_in_progress)
	{
		gsm_off(); // Prioritize later streams in case we got hung-up somehow.
		__delay_ms(1000);
	}

	result = simdet_detect();
	if (result != kNoGSMError)
		return result;

	result = gsm_ensure_on();
	if (result != kNoGSMError)
		return result;


	result = gsm_ensure_registered();
	if (result != kNoGSMError)
		goto stream_failure;

	gsm_remember_band();

	//TODO: Restructure this in terms of error codes
	if (comm_destination_get(0) == '+' )
	{
		state.stream_type = kStreamSMS;
		if (!sms_prepare( comm_destination, strlen(comm_destination)))
			goto stream_failure;
 	}
	else
 	{	

 		result = gprs_ensure_registered();
 		if (result != kNoGSMError)
 			goto stream_failure;

 		result = gprs_connect();
 		if (result != kNoGSMError)
 			goto stream_failure;

 		result = http_init();
 		if (result != kNoGSMError)
 			goto stream_failure;

		result = http_write_prepare(*((uint16_t *)mib_buffer));
		if (result != kNoGSMError)
			goto stream_failure;

 		 state.stream_type = kStreamGPRS;
 	}

	state.stream_in_progress = 1;
	return kNoGSMError;

	//This stream could not be opened so make sure the module is off for us to try again
	stream_failure:
	gsm_off();
	return result;
}

GSMError gsm_putstream()
{
	if (!state.stream_in_progress)
		return kStreamNotInProgress;

	gsm_write(mib_buffer, async_length);
	__delay_ms(1);

	return kNoGSMError;
}

GSMError gsm_closestream()
{
	GSMError error = kNoGSMError;
 	uint8_t result = 0;
 	uint8_t post_timeout = 8*6; 		//8 minutes since each call times out in 10s

 	if (!state.stream_in_progress)
 		return kStreamNotInProgress;

	if (state.stream_type == kStreamSMS)
	{
		if (sms_send())
		{
	 		gsm_off();
	 		return kNoGSMError;
		}

		gsm_expect("+CMGS:");
		gsm_expect2("ERROR");

		do
		{
			result = gsm_await(10);
			if ( result == 1 || --post_timeout == 0 )
				break;
		}
		while (true);
	}
	else
	{
		error = http_post(comm_destination);
		if (error != kNoGSMError)
			goto streaming_error;

		do
		{
			result = http_await_response();

			if (result)
				break;
		}
		while (--post_timeout != 0);
		
		if (!result)
		{
			error = kHTTPTimeoutWaitingForPost;
			goto streaming_error;
		}
		else if (http_status() != 200)
		{
			error = kHTTPFailureCodeReturned;
			goto streaming_error;
		}

		//TODO: We were successful here, check to see if we should leave the connection
		//		open for additional checks
		state.stream_in_progress = 0;
		gsm_off();
		return kNoGSMError;	
	}

	streaming_error:
	gsm_off();
	return error;
}

