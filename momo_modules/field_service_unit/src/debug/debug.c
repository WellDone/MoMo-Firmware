#include "debug.h"
#include "common.h"
#include "command_handlers.h"
#include "task_manager.h"
#include "reset_manager.h"
#include "pme.h"

static volatile char __attribute__((space(data))) debug_buffer[UART_BUFFER_SIZE+1];
static uart_newline_callback debug_callback;

void debug_init()
{
    uart_parameters params;
    
    peripheral_enable(kUART1Module);

    params.baud = 115200;
    params.hw_flowcontrol = 0;
    params.parity = NoParity;
    configure_uart(DEBUG_UART, &params);

    register_command_handlers(); //register the serial commands that we respond to.

    taskloop_set_flag(kTaskLoopSleepBit, 0); //Can't sleep with the UART
    
    uart_set_flag(DEBUG_UART, kResetOnNullCharacterFlag, 1);
    uart_set_flag(DEBUG_UART, kEnableHeartbeatFlag, 1);

    put(DEBUG_UART, RESET);
}

static void getln_callback(char* buf, int len, bool overflown)
{
  uart_set_flag(DEBUG_UART, kDiscardReceivedCharactersFlag, 1);
  debug_buffer[UART_BUFFER_SIZE] = '\0';
  debug_callback(buf, len, overflown);

  //Clear the buffer in order to receive the next command
  uart_clear_receive_buffer(DEBUG_UART);
  uart_set_flag(DEBUG_UART, kDiscardReceivedCharactersFlag, 0);

}

void debug_setup_handler(uart_newline_callback handler)
{
  debug_callback = handler;
  uart_set_newline_callback(DEBUG_UART, getln_callback, debug_buffer, UART_BUFFER_SIZE);
}