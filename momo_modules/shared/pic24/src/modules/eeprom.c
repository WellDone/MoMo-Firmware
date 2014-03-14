#include "eeprom.h"
#include <xc.h>

void eeprom_write(unsigned int offset, unsigned int value)
{
	NVMCON = kEEPROMWriteWord;
	TBLPAG = kEEPROMPage;
	offset |= kEEPROMOffset;

	__builtin_tblwtl(offset, value);
	asm volatile ("disi #5");
	__builtin_write_NVM();

	while(_WR)
		;
}

void eeprom_erase(unsigned int offset)
{
	NVMCON = kEEPROMEraseWord;
	TBLPAG = kEEPROMPage;

	offset |= kEEPROMOffset;
	__builtin_tblwtl(offset, 0);
	asm volatile ("disi #5");
	__builtin_write_NVM();

	while(_WR)
		;
}

unsigned int eeprom_read(unsigned int offset)
{
	TBLPAG = kEEPROMPage;
	offset |= kEEPROMOffset;

	return __builtin_tblrdl(offset);
}