#ifndef __WPU_H__
#define __WPU_H__

#include <xc.h>

//Internal Routines
#define set_pullupr(x)		(WPU##x = 1)
#define clear_pullupr(x)	(WPU##x = 0)

//User Callable Routines
#define enable_pullup(x)		set_pullupr(x)
#define disable_pullup(x)		clear_pullupr(x)
#define enable_weak_pullups()	nWPUEN = 0
#define disable_weak_pullups()	nWPUEN = 1

#endif