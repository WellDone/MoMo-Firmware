//ioc.h
//Interrupt on Change Module, implemented on all Port A pins of
//PIC12 and PIC16 families

/*
 * Interrupt on Change Module
 * These macros configure the internal edge detection interrups on 
 * port A pins.  Call them by passing an integer literal specifying
 * which port A pin you wish to configure
 */

#ifndef __ioc_h__
#define __ioc_h__

#include <xc.h>

//Internal Macros
#define ioc_detect_rising_r(pin, val)	(IOCAP##pin) = (val)
#define ioc_detect_falling_r(pin, val)	(IOCAN##pin) = (val)
#define ioc_flag_r(pin)					(IOCAF##pin)

//User-Callable C API
#define ioc_detect_rising(pin, val)		ioc_detect_rising_r(pin, val)
#define ioc_detect_falling(pin, val)	ioc_detect_falling_r(pin, val)
#define ioc_flag(pin)					ioc_flag_r(pin)
#define ioc_clear_pin(pin)				do{ioc_detect_falling(pin,0); ioc_detect_rising(pin,0);} while(0)

#define ioc_enable()					do{IOCAF=0; IOCIE=1;} while(0)
#define ioc_disable()					IOCIE=0 

#endif
