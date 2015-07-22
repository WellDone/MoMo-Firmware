#ifndef __gprs_h__
#define __gprs_h__

#include "platform.h"

#define GPRS_CONNECT_TIMEOUT	30		//It can take a long time for the gprs connection to activate

void gprs_init_buffers();
void gprs_set_apn();
uint8_t gprs_connect();
uint8_t gprs_connected();
void gprs_disconnect();

#endif