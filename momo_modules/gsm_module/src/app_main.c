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

void task(void)
{
	wdt_disable();
	
	//Don't sleep while the module's on so that we don't miss a
	//serial message
	while(state.module_on)
	{
		;
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

void gsm_power_on()
{
	MAKE_DIGITAL(MODULEPOWERPIN);
	DRIVE_HI(MODULEPOWERPIN);
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
	uint8 resp_len;
	resp_len = copy_to_mib();

	bus_slave_setreturn(pack_return_status(0, resp_len));
}

void gsm_sendcommand()
{
	copy_mib();
	append_carriage();
	
	if (state.module_on)
	{
		uint8 resp_len;

		send_buffer();
		receive_response();

		resp_len = copy_to_mib();
		bus_slave_setreturn(pack_return_status(0, resp_len));
		
		return;
	}

	bus_slave_setreturn(pack_return_status(6,0));
}

/*
 * Do not use, required to be defined by startup code that xc8 apparently cannot *NOT* link in even though it is optimized away later.
 */	
void main()
{
	
}