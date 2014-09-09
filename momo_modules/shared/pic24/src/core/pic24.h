#ifndef _pic24_h
#define _pic24_h

#include <p24Fxxxx.h>
#include "common_types.h"
#include "pic24asm.h"
#define CLOCKSPEED      8000000L //F_osc / 2 (the instruction clock frequency)

//NB: These routines only work for up to ~15000 instructions
//For waits of undetermined amounts, use the functions defined
//in interrupts.h
#define uninterruptible_start() __builtin_disi(0x3FFF)
#define uninterruptible_end() __builtin_disi(0x0000)

#endif