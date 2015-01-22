#ifndef __common_h__
#define __common_h__

#include "pic24.h"

#define RTC_INTERRUPT_BIT       IFS3bits.RTCIF
#define DEBUG_UART              U1

#define ALARM_PIN 				_RB15
#define ALARM_TRIS				_TRISB15

#endif
