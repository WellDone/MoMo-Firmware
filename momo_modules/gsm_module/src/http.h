#ifndef __http_h__
#define __http_h__

#include "platform.h"

uint8_t http_init();
uint8_t http_await_response();
uint8_t http_head(const char* url);
uint8_t http_get(const char* url);
uint8_t http_post(const char* url);
uint8_t http_read(char* out, uint8_t outlen);
uint8_t http_write_prepare(uint16_t);
void http_term();

uint8_t http_status();

#endif