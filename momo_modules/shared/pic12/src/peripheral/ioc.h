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
#define ioc_detect_rising_r_a(pin, val)		(IOCAP##pin) = (val)
#define ioc_detect_falling_r_a(pin, val)	(IOCAN##pin) = (val)
#define ioc_flag_r_a(pin)						(IOCAF##pin)

#define ioc_detect_rising_r_b(pin, val)		(IOCBP##pin) = (val)
#define ioc_detect_falling_r_b(pin, val)	(IOCBN##pin) = (val)
#define ioc_flag_r_b(pin)					(IOCBF##pin)


//User-Callable C API
#define ioc_detect_rising_a(pin, val)	ioc_detect_rising_r_a(pin, val)
#define ioc_detect_falling_a(pin, val)	ioc_detect_falling_r_a(pin, val)
#define ioc_flag_a(pin)					ioc_flag_r_a(pin)

#define ioc_detect_rising_b(pin, val)	ioc_detect_rising_r_b(pin, val)
#define ioc_detect_falling_b(pin, val)	ioc_detect_falling_r_b(pin, val)
#define ioc_flag_b(pin)					ioc_flag_r_b(pin)


#define ioc_clear_pin_a(pin)				do{ioc_detect_falling_a(pin,0); ioc_detect_rising_a(pin,0);} while(0)
#define ioc_clear_pin_b(pin)				do{ioc_detect_falling_b(pin,0); ioc_detect_rising_b(pin,0);} while(0)

#define ioc_enable_a()					do{IOCAF=0; IOCIE=1;} while(0)
#define ioc_enable_b()				 	do{IOCBF=0; IOCIE=1;} while(0)
#define ioc_disable()					IOCIE=0 

#endif
