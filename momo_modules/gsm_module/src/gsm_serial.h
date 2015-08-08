//gsm_serial.h

#ifndef __gsm_serial_h__
#define __gsm_serial_h__

#include "platform.h"

#define GSM_EXPECT_TIMEOUT 20 //10 seconds;
#define GSM_SERIAL_NODATA 0x00
#define GSM_SERIAL_TIMEDOUT 0xFF

void gsm_serial_init();

void gsm_write_char(char);
void gsm_write(const char*, uint8_t);
void gsm_write_str(const char*);

void gsm_clear_receive();
char gsm_rx_pop();
void gsm_rx_clear();
char gsm_rx_peek();
uint8_t gsm_rx();
void gsm_rx_push(char value);

uint8_t gsm_readback( char* buf, uint8_t buf_len );
uint8_t gsm_read( char* buf, uint8_t buf_len );

void gsm_expect( const char* );
void gsm_expect2( const char* );
void gsm_capture_remainder( char*, uint8_t );
void gsm_expect_ok_error();

uint8_t gsm_await( uint8_t timeout_s );
uint8_t gsm_check(uint8_t current);

#endif