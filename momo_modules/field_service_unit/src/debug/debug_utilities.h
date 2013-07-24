#ifndef _uart_utilities_h
#define _uart_utilities_h

#include "common.h"
#include "uart.h"
#define print( msg ) sends( DEBUG_UART, msg )

#define SYN 0x16
#define ACK 0x06
#define NAK 0x15
#define EOT 0x04

void print_byte( BYTE b );
void println( const char* msg );
void getln( uart_newline_callback handler );

#endif