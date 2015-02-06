//bootloader.c

#include "bootloader.h"
#include "memory.h"
#include "ioport.h"
#include "constants.h"

#define HARDWARE_STRING_R(str)		#str
#define HARDWARE_STRING(str)		HARDWARE_STRING_R(str)

const char *hw_version = HARDWARE_STRING(kHardwareVersion);

void _BOOTLOADER_CODE write_row(unsigned int row, unsigned char *row_buffer)
{
	unsigned int j;

	TBLPAG = 0;

	for (j=0; j<kFlashRowSizeInstructions; ++j)
	{
		unsigned int low = (((unsigned int)row_buffer[j*3+1]) << 8) | ((unsigned int)row_buffer[j*3]);
		unsigned int high = row_buffer[j*3+2];

		__builtin_tblwtl((row*kFlashRowSizeWords + j*2) & 0xFFFF, low);
		__builtin_tblwth((row*kFlashRowSizeWords + j*2) & 0xFFFF, high);
	}

	flash_operation(kFlashWriteRow);
}

void _BOOTLOADER_CODE erase_page(unsigned int page)
{
	TBLPAG = 0; //For devices with <= 64kb of RAM, we don't need to page.
	
	__builtin_tblwtl((page*kFlashPageSizeWords) & 0xFFFF, 0);
	flash_operation(kFlashErasePage);
}

void _BOOTLOADER_CODE program_application(unsigned int sector)
{
	uint32 	addr = MEMORY_SECTION_ADDR(sector);
	unsigned int row, page, i;
	unsigned char row_buffer[kFlashRowSizeInstructions*3];

	uint16_t reset_vector;

	reset_vector = extract_reset_vector();

	//Pull alarm low during reflashing so the world knows it
	LAT(ALARM) = 0;
	DIR(ALARM) = OUTPUT;

	//Enable the Memory Module
    LAT(MEM_POWER) = 1;
    DIR(MEM_POWER) = OUTPUT;
	configure_SPI();

	//Give the flash memory time to warm up. (it needs at least 30 us after VCC reaches min value)
	DELAY_MS(1);

	for (page=0, row=0; page<kNumFirmwarePages; ++page)
	{
		erase_page(page);

		//Read in all of the rows from flash for this page and program them
		for (i=0; i<kNumRowsPerPage; ++i)
		{
			mem_read(addr, row_buffer, kFlashRowSizeInstructions*3);

			//Patch the reset vector with a jump to us.
			if (row == 0)
			{
				row_buffer[0] = reset_vector & 0xFF;
				row_buffer[1] = (reset_vector >> 8) & 0xFF;
				row_buffer[2] = 0x04;

				row_buffer[3] = 0;
				row_buffer[4] = 0;
				row_buffer[5] = 0;
			}

			write_row(row, row_buffer);
			++row;

			addr += kFlashRowSizeInstructions*3;
		}
	}

	DIR(ALARM) = INPUT;
}

unsigned int _BOOTLOADER_CODE calculate_checksum(unsigned int start_addr, unsigned int length)
{
	unsigned int checksum = 0;
	unsigned int i;

	TBLPAG = 0;
	for (i=start_addr; i<(start_addr+length); i+=2)
	{
		checksum += __builtin_tblrdl(i);
		checksum += __builtin_tblrdh(i);
	}

	return (~checksum) + 1;
}

/*
 * Check and make sure the metadata block embedded at 0x200 with length 32 words
 * is correct and has all of the right information
 */
ValidationResult _BOOTLOADER_CODE validate_metadata()
{
	unsigned int 	value;
	unsigned int 	metacheck;
	unsigned int 	i;

	TBLPAG = 0;

	//Validate metadata block and make sure that it hasn't been corrupted
	metacheck = __builtin_tblrdl(kMetadataStart + 30);
	value = calculate_checksum(kMetadataStart, kMetadataCheckLength);
	if (value == kNoFirmwareChecksum)
		return kNoFirmwareLoaded;
	else if (value != metacheck)
		return kInvalidMetadata;
	
	//Validate that this firmware image is the right size.*/
	value = __builtin_tblrdl(kMetadataStart + 8);
	if (value != kTotalLength)
		return kWrongFirmwareLength;

	/*
	//Validate IVT, AIVT and Firmware code
	value = __builtin_tblrdl(kMetadataStart + 18);
	value ^= calculate_checksum(kIVTStart, kIVTLength);
	value ^= __builtin_tblrdl(kMetadataStart + 20);
	value ^= calculate_checksum(kAIVTStart, kAIVTLength);
	value ^= __builtin_tblrdl(kMetadataStart + 22);
	value ^= calculate_checksum(kCodeStart, kCodeLength);
	if (value != 0)
		return kInvalidFirmware;*/
	

	//Check to make sure the hardware is compatible
	for (i=0; i<kMetadataHWLength; i+=2)
	{
		unsigned int comp = (((unsigned int)hw_version[i+1]) << 8) | hw_version[i];
		if (comp != __builtin_tblrdl(kMetadataStart+10+i))
			return kNonmatchingHardware;
	}

	return kValidMetadata;
}

/*
 * The reset vector is programmed at address 0 in ROM with a 0x4 as the 16-23 bits.  The low
 * 16 bits is the low word of the address.  Since our device only have 64K of ROM, we can ignore
 * the high word of the address, which is stored at address 2 since it will always be 0;
 */
uint16_t _BOOTLOADER_CODE extract_reset_vector()
{
	TBLPAG = 0;
	return __builtin_tblrdl(0);
}

void _BOOTLOADER_CODE goto_address(unsigned int addr)
{
	asm volatile ("goto w0");
}