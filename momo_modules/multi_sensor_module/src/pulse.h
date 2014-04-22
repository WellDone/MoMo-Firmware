#ifndef __pulse_h__
#define __pulse_h__

#include <stdint.h>

#define kHalfSecondConstant 3036UL

void 		pulse_sample();
uint16_t 	pulse_count();

#endif