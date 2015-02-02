//gsm_serial.h

#ifndef __gsm_serial_h__
#define __gsm_serial_h__

#include "platform.h"

#define GSM_EXPECT_TIMEOUT 20 //10 seconds;
#define GSM_SERIAL_NODATA 0x00
#define GSM_SERIAL_TIMEDOUT 0xFF

void gsm_serial_init();

void gsm_write_char(char);
void gsm_write(const char*, uint8);
void gsm_write_str(const char*);

char gsm_rx_pop();
void gsm_rx_clear();
char gsm_rx_peek();
bool gsm_rx();
void gsm_rx_push(char value);

uint8 gsm_readback( char* buf, uint8 buf_len );
uint8 gsm_read( char* buf, uint8 buf_len );

void gsm_expect( const char* );
void gsm_expect2( const char* );
void gsm_expect_ok_error();

uint8 gsm_await( uint8 timeout_s );
uint8 gsm_check(uint8 current);

#endif