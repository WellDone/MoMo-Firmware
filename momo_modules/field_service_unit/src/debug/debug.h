#ifndef DEBUG_H
#define	DEBUG_H

#include "common.h"
#include "uart.h"

//Types of codes sent on the wire
#define ACK 		0x06
#define NAK 		0x15
#define RESET		0x01
#define HEARTBEAT	0xFF
#define EOT 		0x04

#define print(msg) 		sends(DEBUG_UART, msg)

void debug_init();
void debug_setup_handler(uart_newline_callback handler);
int momo_attached();
#endif	/* DEBUG_H */

