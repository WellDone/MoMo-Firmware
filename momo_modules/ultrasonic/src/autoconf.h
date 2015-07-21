#ifndef __autoconf_h__
#define __autoconf_h__

#include "measure.h"

uint8_t 		find_signal_strength();
UltrasoundError find_optimal_gain(int32_t *out_variance, uint8_t *out_i);
UltrasoundError find_pulse_variance(int32_t *out_variances);

#endif