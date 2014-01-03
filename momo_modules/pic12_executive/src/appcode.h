//appcode.h

#ifndef __appcode_h__
#define __appcode_h__

#include "constants.h"

const char app_init_vector @ kAppInitAddress;
const char app_interrupt_vector @ kAppInterruptAddress;
const char app_task_vector @ kAppTaskAddress;

#define call_app_init()			asm("call _app_init_vector")
#define call_app_interrupt() 	asm("call _app_interrupt_vector")
#define call_app_task()			asm("call _app_task_vector")
#define reset_page()			asm("pagesel($)")

#define sleep()  asm("sleep")

#endif