#include "debug.h"
#include "common.h"
#include "uart_utilities.h"
#include "command_handlers.h"
#include "pme.h"

void debug_init()
{
    uart_parameters params;

    peripheral_enable(kUART2Module);
    register_command_handlers(); //register the serial commands that we respond to.

    params.baud = 38400;
    params.hw_flowcontrol = 0;
    params.parity = NoParity;
    configure_uart(U2, &params);

    taskloop_set_sleep(0); //Can't sleep with the UART

    print("Device reset complete.\r\n");
    sendf(U2, "Type was: %d.\r\n", last_reset_type());
    print(DEBUG_PROMPT);
}