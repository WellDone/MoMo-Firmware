#include "debug.h"
#include "../core/common.h"
#include "command_handlers.h"
#include "../modules/uart.h"
#include "../core/scheduler.h"
#include "../core/reset_manager.h"

extern volatile int cmd_received;

ScheduledTask disable_debug_task;

void debug_init()
{
    uart_parameters params;

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
	//Disable on the second call to make at least 10 seconds have passed.
	if (cmd_received == 0 && num_calls > 0)
	{
		print("Disabling debug interface");
		uart_set_disabled(U2, 1);
	}

	++num_calls;
}