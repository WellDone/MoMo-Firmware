#ifndef __sms_h__
#define __sms_h__

#include "platform.h"

bool sms_prepare(const char* destination, uint8 length);
bool sms_send();

#endif