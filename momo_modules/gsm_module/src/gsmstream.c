/* gsmstream.c
 *
 * Endpoints for opening and dumping data to a text message stream.
 *
 *
 *
 */

#include "mib12_api.h"
#include "gsm_defines.h"
#include "gsm_strings.h"
#include "gsm_serial.h"
#include "gsm_module.h"
#include "global_state.h"
#include "simcard.h"
#include "port.h"

#define _XTAL_FREQ			4000000

 void gsm_openstream()
 {
 	gsm_on();
 	if (PIN(GSMSTATUSPIN) == 0)
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
 	}
 	state.stream_in_progress = 1;

 	if ( !wait_for_registration() )
 	{
 		gsm_off();
 		bus_slave_setreturn( pack_return_status(6, 0) );
 		return;
 	}

 	__delay_ms( 100 );

 	load_gsm_constant(kStartStreamString);
 	send_buffer();

 	//mib buffer should have the address we send to
 	copy_mib();
 	send_buffer();

 	gsm_buffer[0] = '"';
 	buffer_len = 1;
 	append_carriage();
 	send_buffer();

 	__delay_ms( 200 );

 	bus_slave_setreturn(pack_return_status(0,0));
 }

 void gsm_putstream()
 {
 	if ( !state.stream_in_progress )
 	{
 		bus_slave_setreturn(pack_return_status(7,0));
 		return;
 	}
 	copy_mib();
 	if ( !send_buffer() )
 	{
		bus_slave_setreturn(pack_return_status(6,0));
		return;
 	}

 	bus_slave_setreturn(pack_return_status(0,0));
 }

 void gsm_closestream()
 {
 	gsm_buffer[0] = 0x1A;
 	buffer_len = 1;

 	state.shutdown_pending = 1;  // Shutdown the module after we've sent the message (or timed out)
 	state.stream_in_progress = 0; //TODO: Prevent new streams until the message has actually sent?
 	send_buffer();

 	receive_response();

 	bus_slave_setreturn(pack_return_status(0,0));
 }

 void gsm_abandonstream()
 {
 	gsm_off();
 	bus_slave_setreturn(pack_return_status(0,0));
 }