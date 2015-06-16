//appcode.h

#ifndef __appcode_h__
#define __appcode_h__

//Only call pagesel when there are multiple rom pages
#ifdef kMultipageDevice
#define reset_page()			asm("pagesel($)")
#else
#define reset_page()
#endif

#define sleep()  				asm("sleep")

#define goto_address_r(address)	asm("goto " # address)
#define goto_address(address)	goto_address_r(address)

#define goto_application()		goto_address(kApplicationAddress)
#define goto_interrupt()		goto_address(kApplicationAddress+4)

#endif