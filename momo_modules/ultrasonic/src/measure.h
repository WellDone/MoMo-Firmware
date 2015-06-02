#ifndef __measure_h__
#define __measure_h__

#include <stdint.h>

void 	set_parameters(uint8_t gain, uint8_t threshold, uint8_t pulses, uint16_t mask);
int32_t measure_delta_tof();

uint8_t take_measurement();
void 	sort_measurements();
#endif
