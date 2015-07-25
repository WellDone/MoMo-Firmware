#ifndef __http_h__
#define __http_h__

#include "platform.h"
#include "global_state.h"

GSMError http_init();
GSMError http_write_prepare(uint16_t len);

GSMError http_post(const char* url);

uint8_t http_await_response();
uint8_t http_head(const char* url);
uint8_t http_get(const char* url);

uint8_t http_read(char* out, uint8_t outlen);

void http_term();

uint16_t http_status();

#endif