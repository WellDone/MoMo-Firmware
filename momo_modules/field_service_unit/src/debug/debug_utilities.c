#include "debug_utilities.h"
#include "common.h"
#include "uart.h"

static volatile char __attribute__((space(data))) debug_buffer[UART_BUFFER_SIZE+1];
static uart_newline_callback debug_callback;

void print_byte( BYTE b )
{
  print( "0b");
  BYTE mask = 0x80;
  while (mask!=0x0) {
    if ( ( b & mask ) )
      print("1");
    else
      print("0");
    mask = mask >> 1;
  }
  print("\n");
}

void println( const char* msg ) {
  print( msg );
  print( "\n" );
}

static void getln_callback(char* buf, int len, bool overflown)
{
  clear_uart_rx_newline_callback( DEBUG_UART );
  uart_newline_callback tmp_callback = debug_callback;
  debug_callback = 0;
  debug_buffer[UART_BUFFER_SIZE] = '\0';
  tmp_callback( buf, len, overflown );
}
void getln( uart_newline_callback handler )
{
  debug_callback = handler;
  set_uart_rx_newline_callback( DEBUG_UART, getln_callback, debug_buffer, UART_BUFFER_SIZE );
}