#ifndef __ioc_inc__
#define __ioc_inc__

#include <xc.inc>

//User friendly ASM API
ioc_disable_asm MACRO
	banksel INTCON 
	bcf IOCIE
ENDM

ioc_enable_asm MACRO
	banksel INTCON
	bcf IOCIF 	 
	bsf IOCIE
ENDM

#endif
