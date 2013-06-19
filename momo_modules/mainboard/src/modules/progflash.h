/*
 * progflash.h
 * Routines for erasing and programming blocks of flash memory.  
 * The programming commands and addresses are specific to the PIC24F16KA1XX family
 * of processors.
 */

#ifndef __progflash_h__
#define __progflash_h__

#include "common.h"

#define table_page(addr) ((addr & 0x7F0000) >> 16);
#define table_offset(addr) (addr & 0x00FFFF)

#define kNumInstructionsPerRow	32

typedef enum
{
	kErase1Row = 0b011000,
	kErase2Rows = 0b011001,
	kErase4Rows = 0b011010,
	kEraseGeneralMemory = 0b001100,
	kWrite1Row = 0b000100,
	kEraseEEPROM = 0b010000,
	kErase1EEPROMWord = kErase1Row,
	kErase4EEPROMWords = kErase2Rows,
	kErase8EEPROMWords = kErase4Rows,
	kWrite1EEPROMWord = kWrite1Row,
	kEraseConfigBlock = 0b010100
} ProgFlashOperation;

enum
{
	kEEPROMPage = 0x7F,
	kEEPROMStartAddress = 0xFE00,
	kConfigPage = 0xF8,
	kConfigStartAddress = 0x0000
};

#define progflash_set_erase() _ERASE = 1
#define progflash_set_write() _ERASE = 0
#define progflash_op_finished() (_WR == 0)
#define progflash_setpage(page) TBLPAG = page

//Runtime reflashing routines
void progflash_setop(ProgFlashOperation type);
void progflash_execute();
void progflash_setaddr(unsigned char page, unsigned int address);
void progflash_loadlatches(unsigned int address, unsigned int *buffer, unsigned int word_len);

#endif