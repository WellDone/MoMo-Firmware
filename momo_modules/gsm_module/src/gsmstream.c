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
#include "simcard.h"

extern uint8 gsm_buffer[32];
extern uint8 buffer_len;

typedef union 
{
	struct
	{
		volatile uint8 open_module:1;
		volatile uint8 close_module:1;
		volatile uint8 send_command:1;
		volatile uint8 module_open:1;
		volatile uint8 last_response:1;
		volatile uint8 wait_for_text: 1;
		volatile uint8 unused:2;
	};

	volatile uint8 gsm_state;
} ModuleState;

extern ModuleState state;

#define _XTAL_FREQ			4000000

 void gsm_openstream()
 {
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

 	load_gsm_constant(kStartStreamString);
 	send_buffer();

 	//mib buffer should have the address we send to
 	copy_mib();
 	send_buffer();

 	gsm_buffer[0] = '"';
 	buffer_len = 1;
 	append_carriage();
 	send_buffer();

 	__delay_ms(50);

 	bus_slave_setreturn(pack_return_status(0,0));
 }

 void gsm_putstream()
 {
 	copy_mib();
 	send_buffer();

 	bus_slave_setreturn(pack_return_status(0,0));
 }

 void gsm_closestream()
 {
 	gsm_buffer[0] = 0x1A;
 	buffer_len = 1;
 	send_buffer();
 	
 	bus_slave_setreturn(pack_return_status(0,0));
 }