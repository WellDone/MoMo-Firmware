#ifndef _uart_utilities_h
#define _uart_utilities_h

#include "uart.h"
#define print( msg ) sends( DEBUG_UART, msg )

void print_byte( BYTE b );
void println( const char* msg );
#endif