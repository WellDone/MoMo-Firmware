//appcode.h

#ifndef __appcode_h__
#define __appcode_h__

#include "constants.h"

extern void call_app_init();	
extern void call_app_interrupt();
extern void call_app_task();

//Only call pagesel when there are multiple rom pages
#ifdef kMultipageDevice
#define reset_page()			asm("pagesel($)")
#else
#define reset_page()
#endif

#define sleep()  				asm("sleep")

#endif