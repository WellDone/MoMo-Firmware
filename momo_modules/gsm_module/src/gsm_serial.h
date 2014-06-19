//gsm_serial.h

#ifndef __gsm_serial_h__
#define __gsm_serial_h__

#include "platform.h"

void enable_serial();

bool send_buffer();
uint8 gsm_receiveone();
uint8 peek_rx_buffer_end();
uint8 receive_response();
uint8 open_gsm_module();

void copy_mib();
uint8 copy_to_mib();

void append_carriage();

void gsm_openstream();
void gsm_putstream();
void gsm_closestream();
#endif