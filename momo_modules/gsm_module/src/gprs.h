#ifndef __gprs_h__
#define __gprs_h__

#include "platform.h"

bool gprs_set_apn(const char* apn, uint8 len);
bool gprs_connect();
bool gprs_connected();
void gprs_disconnect();

#endif