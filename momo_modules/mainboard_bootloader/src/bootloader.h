#ifndef __bootloader_h__
#define __bootloader_h__

#include "pic24.h"

#define _BOOTLOADER_CODE __attribute__((section(".bootloader")))
#define _BOOTLOADER_VAR  __attribute__((section(".bootvar")))

#define kReflashMagic 		0xABCD 

enum
{
	kFlashWriteRow = 0b0100000000000001,
	kFlashErasePage = 0b0100000001000010
}; 

void program_application(unsigned int sector);
void erase_row(unsigned int row);
void patch_reset_vector(unsigned char *row_buffer, uint32 low, uint32 high);
void extract_reset_vector(uint32 *low, uint32 *high);
bool valid_instruction(unsigned int addr);
void goto_address(unsigned int addr);

//Assembly Instructions
extern void flash_operation(unsigned int nvmcon);

//Clock configuration and delays
#define FCY   4000000L  //define your instruction frequency, FCY = FOSC/2
  
#define CYCLES_PER_MS ((unsigned long long)(FCY * 0.001))        //instruction cycles per millisecond
#define CYCLES_PER_US ((unsigned long long)(FCY * 0.000001))   //instruction cycles per microsecond
#define DELAY_MS(ms)  __delay32(CYCLES_PER_MS * ((unsigned long long) ms));   //__delay32 is provided by the compiler, delay some # of milliseconds
#define DELAY_US(us)  __delay32(CYCLES_PER_US * ((unsigned long long) us));    //delay some number of microseconds
extern void __delay32(unsigned long long);

#endif