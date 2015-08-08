#ifndef __gprs_h__
#define __gprs_h__

#include "platform.h"
#include "global_state.h"

#define GPRS_REGISTRATION_TIMEOUT 		60
#define GPRS_CONNECT_TIMEOUT			30		//It can take a long time for the gprs connection to activate

void 		gprs_init_buffers();
void 		gprs_set_apn();

GSMError 	gprs_ensure_registered();
GSMError 	gprs_connect();
GSMError 	gprs_disconnect();

#endif