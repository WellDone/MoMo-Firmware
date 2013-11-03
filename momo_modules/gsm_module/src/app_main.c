//app_main.c

#include "platform.h"
#include "protocol.h"
#include "gsm_defines.h"
#include "mib12_api.h"
#include "watchdog.h"
#include "gsm_serial.h"
#include "simcard.h"

#define _XTAL_FREQ			4000000

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

ModuleState state;

void gsm_module_on();

void task(void)
{
	wdt_disable();
	/*
	//Wait until a sim card is inserted and then power up the GSM module
	while(test_siminserted() == 0)
	{
		WDTCON = k1SecondTimeout;
		wdt_enable();
		asm("sleep");
	}

	gsm_module_on();

	if (open_gsm_module() == 0)
		state.module_open = 1;
			
	state.open_module = 0;	

	//wait 15 seconds
	__delay_ms(15000);

	mib_buffer[0] = '+';
	mib_buffer[1] = '2';
	mib_buffer[2] = '5';
	mib_buffer[3] = '5';
	mib_buffer[4] = '7';
	mib_buffer[5] = '1';
	mib_buffer[6] = '4';
	mib_buffer[7] = '2';
	mib_buffer[8] = '3';
	mib_buffer[9] = '8';
	mib_buffer[10] = '4';
	mib_buffer[11] = '7';
	mib_buffer[12] = '5';
	mib_packet.param_spec = 13;

	gsm_openstream();
	mib_buffer[12] = '7';
	gsm_putstream();
	gsm_closestream();

	while (1)
	{	
		if (state.open_module)
		{
			if (open_gsm_module() == 0)
				state.module_open = 1;
			
			state.open_module = 0;	
		}
		
		if (state.module_open)
		{
			if (state.send_command)
			{
				send_buffer();
				state.last_response = receive_response();

				state.send_command = 0;
			}

			if (wait_for_text() == 1)
			{
				//Have it send a command
				mib_packet.feature = 60;
				mib_packet.command = 0;
				mib_packet.param_spec = 0;

				bus_master_rpc_sync(8);
			}
		}
	}

	*/

	while (1)
		;
}

void interrupt_handler(void)
{
}

void enable_serial()
{
	TXEN = 1;
	SYNC = 0;
	SPEN = 1;

	BRGH = 1;
	BRG16 = 1;

	SPBRGH = 0;
	SPBRGL = 16;

	CREN = 1;
	
	//clear the contents of RCREG
	RCREG;
	RCREG;

	RCIF = 0;
}

void initialize(void)
{	
	POWERPIN = 0;
	POWERTRIS = 0;

	GSMPOWERTRIS = 1;
	GSMPOWERPIN = 0;
	GSMRESETTRIS = 1;
	GSMRESETPIN = 0;

	GSMSTATUSTRIS = 1;

	RXDTSEL = 0;
	TXCKSEL = 0;

	state.gsm_state = 0;
	buffer_len = 0;
}

/*
 * Do not use, required to be defined by startup code that xc8 apparently cannot *NOT* link in even though it is optimized away later.
 */	
void main()
{
	
}

void gsm_setpower()
{
	if (plist_get_int8(0) == 0)
		POWERPIN = 0;
	else
		POWERPIN = 1;

	bus_slave_setreturn(pack_return_status(0,2));
}

void gsm_module_on()
{
	wdt_disable();
	enable_serial();

	if (GSMSTATUSPIN == 0)
	{
		GSMPOWERPIN = 0;
		__delay_ms(500);

		GSMPOWERTRIS = 0;
		__delay_ms(1200);
		
		GSMPOWERTRIS = 1;

		__delay_ms(3000);

		state.open_module = 1;
	}

	mib_buffer[0] = RA5;
	mib_buffer[1] = 0;

	bus_slave_setreturn(pack_return_status(0,2));
}

void gsm_testsim()
{
	mib_buffer[0] = test_siminserted();
	mib_buffer[1] = 0;
	bus_slave_setreturn(pack_return_status(0,2));	
}

void gsm_sendcommand()
{
	copy_mib();
	append_carriage();
	
	state.send_command = 1;
	state.wait_for_text = 0;
	
	bus_slave_setreturn(pack_return_status(0,0));
}

void gsm_readresponse()
{
	bus_slave_setreturn(pack_return_status(0,0));
}