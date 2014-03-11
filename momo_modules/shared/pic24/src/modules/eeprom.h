#ifndef __eeprom_h__
#define __eeprom_h__

#define kEEPROMPage 0x7F
#define kEEPROMOffset 0xFE00		//valid for pic24f16ka101

enum
{
	kEEPROMEraseWord = 0x4058,
	kEEPROMWriteWord = 0x4004
};

void 		 eeprom_write(unsigned int offset, unsigned int value);
unsigned int eeprom_read(unsigned int offset);


#endif