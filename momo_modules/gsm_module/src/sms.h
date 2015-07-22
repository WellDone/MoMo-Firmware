#ifndef __sms_h__
#define __sms_h__

#include "platform.h"

uint8_t sms_prepare(const char* destination, uint8_t length);
uint8_t sms_send();

#endif