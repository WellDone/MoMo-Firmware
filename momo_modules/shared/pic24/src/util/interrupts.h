
#ifndef __interrupts_h__
#define __interrupts_h__

#include "pic24.h"

#define kMaxInterruptLevel      7

//Routines
void configure_interrupts();

int  disable_interrupts();
void enable_interrupts(int old_level);

#endif
