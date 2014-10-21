#ifndef __http_h__
#define __http_h__

#include "platform.h"

bool http_init();
bool http_await_response( uint8 seconds );
bool http_head(const char* url);
bool http_get(const char* url);
bool http_post(const char* url);
bool http_read(char* out, uint8 outlen);
bool http_write_prepare(uint16);
void http_term();

bool http_await_response();
uint8 http_status();

#endif