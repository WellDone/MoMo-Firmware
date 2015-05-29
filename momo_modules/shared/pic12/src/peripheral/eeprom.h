#ifndef __eeprom_h__
#define __eeprom_h__

#include <xc.h>
#include "platform.h"

unsigned char 	eeprom_read(unsigned char offset);

//NB Interrupts are disabled during the EEPROM writing proces
//they are disabled on startwrite and reenabled on endwrite
void 			eeprom_startwrite(unsigned char offset);
void			eeprom_write(unsigned char value);
void			eeprom_endwrite();

#endif