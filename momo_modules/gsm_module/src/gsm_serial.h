//gsm_serial.h

#ifndef __gsm_serial_h__
#define __gsm_serial_h__

#include "platform.h"

void enable_serial();

void send_buffer();
uint8 receive_response();
uint8 open_gsm_module();

void copy_mib();
uint8 copy_to_mib();

void append_carriage();
uint8 match_response(const char *resp);

void gsm_openstream();
void gsm_putstream();
void gsm_closestream();
#endif