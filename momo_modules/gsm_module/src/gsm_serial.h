//gsm_serial.h

#ifndef __gsm_serial_h__
#define __gsm_serial_h__

void send_buffer();
uint8 receive_response();
void open_gsm_module();

void copy_mib();
void append_carriage();

#endif