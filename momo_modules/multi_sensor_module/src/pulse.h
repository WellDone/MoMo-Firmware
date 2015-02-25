#ifndef __pulse_h__
#define __pulse_h__

#include <stdint.h>

void 		pulse_sample();
uint16_t 	pulse_count();
uint16_t 	pulse_invalid_count();
uint16_t 	pulse_width(uint8_t pulse);
uint16_t 	pulse_interval(uint8_t pulse);
uint16_t 	pulse_median_interval();

void		pulse_falling_edge();
void		pulse_rising_edge();
#endif