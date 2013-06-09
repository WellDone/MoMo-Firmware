#ifndef _pic24_h
#define _pic24_h

#include <p24Fxxxx.h>
#include "common_types.h"
#include "assembly.h"
#define CLOCKSPEED      8000000L //F_osc / 2 (the instruction clock frequency)

#define uninterruptible_start() __builtin_disi(0x3FFF)
#define uninterruptible_end() __builtin_disi(0x0000)

#endif