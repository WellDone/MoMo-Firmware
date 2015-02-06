#ifndef __bootloader_h__
#define __bootloader_h__

#include <stdint.h>
#include "pic24.h"

#define _BOOTLOADER_CODE //__attribute__((section(".bootloader")))
#define _BOOTLOADER_VAR  __attribute__((section(".bootvar")))

#define kReflashMagic 			0xABCD
#define kRecoverMagic			0xACCA
#define kAlreadyRecoveredMagic	0xBDDB		//If we have already attempted recovery, don't keep on doing so	

//These section definitions need to stay in sync with controllerblock.py in pymomo.hex
//These constants define the boundaries of contigous memory regions that are checksummed
//separately by the bootloader.
#define kIVTStart				0x04
#define kIVTLength				(0x100 - 0x04)

#define kAIVTStart				0x100
#define kAIVTLength				0x100

#define kMetadataStart			0x200
#define kMetadataCheckLength 	30
#define kMetadataLength			32
#define kMetadataHWLength 		8

#define kCodeStart				(0x200 + kMetadataLength)
#define kCodeLength				(0xA600 - kMetadataLength)
#define kTotalLength 			0xA600

#define kMagicBlock1			0xBAAD
#define kMagicBlock2			0xDAAD

#define kMetadataResetVector	(kMetadataStart + 4)

//The checksum that would be calculated from 30 0xFFFFFF words so that we can distinguish
//between an invalid firmware and no firmware for debugging purposes.
#define kNoFirmwareChecksum		0xf11e

enum
{
	kFlashWriteRow = 0b0100000000000001,
	kFlashErasePage = 0b0100000001000010
}; 

typedef enum
{
	kValidMetadata = 1,
	kInvalidMagic = 2,
	kInvalidFirmware = 3,
	kInvalidMetadata = 4,
	kNonmatchingHardware = 5,
	kAddressError = 6,
	kNoFirmwareLoaded = 7,
	kWrongFirmwareLength = 8
} ValidationResult;

void program_application(unsigned int sector);
void erase_row(unsigned int row);
void patch_reset_vector(unsigned char *row_buffer, uint16_t vector);
uint16_t extract_reset_vector();
void goto_address(unsigned int addr);

//Validation routines
unsigned int calculate_checksum(unsigned int start_addr, unsigned int length);
ValidationResult validate_metadata();

//Assembly Instructions
extern void flash_operation(unsigned int nvmcon);

//Clock configuration and delays
#define FCY   kClockspeed  		//define your instruction frequency, FCY = FOSC/2
  
#define CYCLES_PER_MS ((unsigned long long)(FCY * 0.001))        //instruction cycles per millisecond
#define CYCLES_PER_US ((unsigned long long)(FCY * 0.000001))   //instruction cycles per microsecond
#define DELAY_MS(ms)  __delay32(CYCLES_PER_MS * ((unsigned long long) ms));   //__delay32 is provided by the compiler, delay some # of milliseconds
#define DELAY_US(us)  __delay32(CYCLES_PER_US * ((unsigned long long) us));    //delay some number of microseconds
extern void __delay32(unsigned long long);

#endif