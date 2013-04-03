#include "debug.h"
#include "common.h"
#include "command_handlers.h"
#include "uart.h"
#include "scheduler.h"
#include "reset_manager.h"
#include "pme.h"

extern volatile int cmd_received;

ScheduledTask disable_debug_task;

void debug_init()
{
    uart_parameters params;

    peripheral_enable(kUART2Module);
    register_command_handlers(); //register the serial commands that we respond to.

    params.baud = 38400;
    params.hw_flowcontrol = 0;
    params.parity = NoParity;
    configure_uart(U2, &params);

    print("Device reset complete.\r\n");
    sendf(U2, "Type was: %d.\r\n", last_reset_type());
    print(DEBUG_PROMPT);

    //Set a callback to check in 10 seconds if we were used and disable us if no commands
    //have been received since then.
    scheduler_schedule_task(debug_disable_unconnected, kEvery10Seconds, 2, &disable_debug_task);
}

void debug_disable_unconnected()
{
	static int num_calls = 0;
	//If no commands have been received, disable the debug interface
	//Disable on the second call to make sure at least 10 seconds have passed.
	if (cmd_received == 0 && num_calls > 0)
	{
		print("Disabling debug interface");
		peripheral_disable(kUART2Module);
	}

	++num_calls;
}