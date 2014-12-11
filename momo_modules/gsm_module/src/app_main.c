//app_main.c

#include "platform.h"
#include "protocol.h"
#include "gsm_defines.h"
#include "mib12_api.h"
#include "watchdog.h"
#include "gsm_serial.h"
#include "simcard.h"
#include "gsm_module.h"
#include "global_state.h"
#include "gsm_strings.h"
#include "timer1.h"
#include "port.h"

#define SHUTDOWN_TIMEOUT 240 //One minute, in half seconds

static void report_result(void)
{
	bus_master_begin_rpc();
	mib_packet.param_spec = plist_empty();
	mib_packet.feature = 60;
	mib_packet.command = ( cmgs_matched() )? 0xF0 : 0xF1;
	bus_master_send_rpc(8);
}

static void capture_error(void)
{
	while ( gsm_receiveone() == 0 )
		;

	bus_master_begin_rpc();
	bus_master_prepare_rpc( 42, 0x20, plist_with_buffer( 0, copy_to_mib() ) );
	bus_master_send_rpc( 8 );
}

void task(void)
{
	wdt_disable();
	
	//Don't sleep while the module's on so that we don't miss a
	//serial message
	uint8 timeout_counter = SHUTDOWN_TIMEOUT;
	while(state.module_on)
	{
		if ( gsm_receiveone() == 0 )
			timeout_counter = SHUTDOWN_TIMEOUT;
		else if ( state.shutdown_pending )
			--timeout_counter; // decrement if we timed out waiting for an RX byte.

		if ( state.shutdown_pending )
		{
			if ( timeout_counter == 0 || cmgs_matched() || err_matched() )
			{
				report_result();
				if ( err_matched() )
					capture_error();
				gsm_off();
			}
		}
	}
}

void interrupt_handler(void)
{

}

void initialize(void)
{	
	gsm_init();
	simdet_idle();
}

//MIB Endpoints
void gsm_module_on()
{
	wdt_disable();
	
	mib_buffer[0] = gsm_on();
	mib_buffer[1] = 0;

	bus_slave_setreturn(pack_return_status(0,2));
}
void gsm_module_off()
{
	gsm_off();
	bus_slave_setreturn(pack_return_status(0,0));
}

void gsm_power_on()
{
	ENSURE_DIGITAL(MODULEPOWERPIN);
	DRIVE_HI(MODULEPOWERPIN);
	bus_slave_setreturn(pack_return_status(0,0));
}

void gsm_power_off()
{
	ENSURE_DIGITAL(MODULEPOWERPIN);
	DRIVE_LOW(MODULEPOWERPIN);
	bus_slave_setreturn(pack_return_status(0,0));	
}

void gsm_testsim()
{
	mib_buffer[0] = simdet_detect();
	mib_buffer[1] = 0;

	bus_slave_setreturn(pack_return_status(0,2));	
}

void gsm_dumpbuffer()
{
	bus_slave_setreturn(pack_return_status(0, copy_to_mib()));
}
void gsm_debug()
{
	mib_buffer[0] = state.module_on;
	mib_buffer[1] = state.shutdown_pending;
	mib_buffer[2] = rx_buffer_start;
	mib_buffer[3] = rx_buffer_end;
	mib_buffer[4] = rx_buffer_len;

	bus_slave_setreturn(pack_return_status(0, 5));
}

void gsm_sendcommand()
{
	copy_mib();
	append_carriage();
	
	if (state.module_on)
	{
		send_buffer();

		if ( receive_response() == 2 ) { //timeout
			bus_slave_setreturn( pack_return_status(6,0) );
			return;
		}

		bus_slave_setreturn(pack_return_status(0, copy_to_mib()));
		
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