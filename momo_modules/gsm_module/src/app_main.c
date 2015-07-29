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
#include "gsm_module.h"
#include "port.h"
#include "gsmstream.h"
#include "intel_hex.h"
#include "buffers.h"
#include <string.h>


extern uint16_t http_response_status;
extern char* uint_buf;

void task(void)
{
	uint8_t result;

	switch(rpc_request)
	{
		//Turn on the GSM Module and send initial configuration commands
		case kEnableModule:
		mib_buffer[0] = gsm_ensure_on();
		mib_buffer[1] = 0;

		rpc_request = kNoRequestedTask;
		bus_master_async_callback(2);
		break;

		//Turn off the GSM module
		case kDisableModule:
		gsm_module_off();
		
		rpc_request = kNoRequestedTask;
		bus_master_async_callback(0);
		break;

		//Block until the GSM module is connected to the network (module must already be on)
		case kConnectToNetwork:
		mib_buffer[0] = gsm_ensure_registered();
		mib_buffer[1] = 0;

		rpc_request = kNoRequestedTask;
		bus_master_async_callback(2);
		break;

		//Send an arbitrary AT command and receive the response
		case kSendCommand:
		if (async_length > 0)
		{
			gsm_write(mib_buffer, async_length);
			rpc_request = kNoRequestedTask;
			bus_master_async_callback(0);
		}
		else
		{
			gsm_rx_clear();
			result = gsm_cmd("");
			gsm_readback(mib_buffer, kMIBBufferSize);

			rpc_request = kNoRequestedTask;
			bus_master_async_callback(20);
		}
		break;

		//Open a communication stream to the current comm destination
		case kOpenStream:
		mib_buffer[0] = gsm_openstream();
		mib_buffer[1] = 0;

		rpc_request = kNoRequestedTask;
		bus_master_async_callback(2);
		break;

		//Push data to the currently open communication stream
		case kPushToStream:
		mib_buffer[0] = gsm_putstream();
		mib_buffer[1] = 0;

		rpc_request = kNoRequestedTask;
		bus_master_async_callback(2);
		break;

		//Close the currently open communication stream
		case kCloseStream:
		mib_buffer[0] = gsm_closestream();
		mib_buffer[1] = 0;

		rpc_request = kNoRequestedTask;
		bus_master_async_callback(2);
		break;

		default:
		break;
	}
}

void interrupt service_isr()
{
	
}

void initialize(void)
{	
	debug_val = 0;
	http_response_status = 0;
	rpc_request = kNoRequestedTask;
	connection_status = kModuleOff;
	gsm_init();
}

//MIB Endpoints
uint8_t gsm_rpc_on()
{	
	rpc_request = kEnableModule;

	return kAsynchronousResponseStatus;
}
uint8_t gsm_rpc_off()
{
	rpc_request = kDisableModule;
	
	return kAsynchronousResponseStatus;
}

uint8_t gsm_rpc_testsim()
{
	mib_buffer[0] = simdet_detect();
	mib_buffer[1] = 0;

	bus_slave_returndata(2);

	return kNoErrorStatus;
}

uint8_t gsm_rpc_debug()
{
	mib_buffer[0] = connection_status;
	mib_buffer[1] = 0;
	mib_buffer[2] = rx_buffer_start;
	mib_buffer[3] = rx_buffer_end;
	mib_buffer[4] = rx_buffer_len;
	mib_buffer[5] = debug_val;
	((uint16_t*)mib_buffer)[3] = http_response_status;

	bus_slave_returndata(8);

	return kNoErrorStatus;
}

uint8_t gsm_rpc_download()
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

	// uint8_t len;
	// do
	// {
	// 	len = http_read( mib_buffer, kBusMaxMessageSize ); // Just read through the entire document, don't return anything.
	// } while (len > 0);

	// http_term();
	// gprs_disconnect();

	// bus_slave_setreturn(pack_return_status(0, 0));

	return kNoErrorStatus;
}

uint8_t gsm_rpc_sendcommand()
{
	if (connection_status > kModuleOff)
	{
		rpc_request = kSendCommand;
		async_length = mib_packet.call.length;

		return kAsynchronousResponseStatus;
	}

	return 7;
}

uint8_t gsm_rpc_register()
{
	if (connection_status > kModuleOff)
	{
		rpc_request = kConnectToNetwork;
		return kAsynchronousResponseStatus;
	}

	return 7;
}

uint8_t gsm_rpc_openstream()
{
	rpc_request = kOpenStream;
	return kAsynchronousResponseStatus;
}

uint8_t gsm_rpc_pushtostream()
{
	rpc_request = kPushToStream;

	async_length = mib_packet.call.length;
	return kAsynchronousResponseStatus;
}

uint8_t gsm_rpc_closestream()
{
	rpc_request = kCloseStream;
	return kAsynchronousResponseStatus;
}

uint8_t gsm_rpc_abandonstream()
{
	rpc_request = kDisableModule;
	return kAsynchronousResponseStatus;
}

void main()
{
	initialize();

	while (1)
	{
		task();

		asm("sleep");
	}	
}