#ifndef _pic24_h
#define _pic24_h

#include <xc.h>
#include "common_types.h"
#include "pic24asm.h"

//NB: These routines only work for up to ~15000 instructions
//For waits of undetermined amounts, use the functions defined
//in interrupts.h
#ifdef __TEST__
#define uninterruptible_start()
#define uninterruptible_end()
#else
#define uninterruptible_start() __builtin_disi(0x3FFF)
#define uninterruptible_end() __builtin_disi(0x0000)
#endif

#endif