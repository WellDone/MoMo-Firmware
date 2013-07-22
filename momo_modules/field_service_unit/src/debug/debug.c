#include "debug.h"
#include "common.h"
#include "debug_utilities.h"
#include "command_handlers.h"
#include "task_manager.h"
#include "reset_manager.h"
#include "pme.h"

void debug_init()
{
    uart_parameters params;
    

    peripheral_enable(kUART1Module);

    params.baud = 115200;
    params.hw_flowcontrol = 0;
    params.parity = NoParity;
    configure_uart(DEBUG_UART, &params);

    register_command_handlers(); //register the serial commands that we respond to.

    taskloop_set_sleep(0); //Can't sleep with the UART

    sendf(DEBUG_UART, "Type was: %d.\r\n", last_reset_type());
    put(DEBUG_UART, ACK);
    //print(DEBUG_PROMPT);
}