#ifndef _uart_utilities_h
#define _uart_utilities_h

#include "common.h"
#include "uart.h"
#define print( msg ) sends( DEBUG_UART, msg )

#define SYN 'S'/*0x16*/
#define ACK 'Y'/*0x06*/
#define NAK 'N'/*0x15*/
#define EOT 'E'/*0x04*/

void print_byte( BYTE b );
void println( const char* msg );
void getln( uart_newline_callback handler );

#endif