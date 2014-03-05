//bootloader.c

#include "bootloader.h"
#include "memory.h"

extern uint32 _BOOTLOADER_CODE ReadLatch( uint16, uint16 );
extern void Erase( uint16, uint16, uint16 );
extern void WriteLatch( uint16, uint16, uint16, uint16 );
extern void WriteMem( uint16 );

#define PM_ROW_ERASE 		0x4042
#define PM_ROW_WRITE 		0x4001

bool _BOOTLOADER_CODE BootloadPending()
{
	bool ready = false;
	mem_read( BOOTLOADER_FLASH_BLOCK_START, (BYTE*)&ready, 1 );
	return ready;
}

static uint32 _BOOTLOADER_CODE BootloaderFirmwareLength()
{
	uint32 length = 0;
	mem_read( BOOTLOADER_FLASH_BLOCK_START+1, ((BYTE*)&length)+1, 3 );
	return length;
}

static uint32 BackupFirmwareLength()
{
	uint32 length = 0;
	mem_read( BOOTLOADER_FLASH_BACKUP_START+1, ((BYTE*)&length)+1, 3 );
	return length;
}

static bool BackupExists()
{
	bool ready = false;
	mem_read( BOOTLOADER_FLASH_BACKUP_START, (BYTE*)&ready, 1 );
	return ready;
}

#define PROGRAM_ROW_SIZE 4
static bool _BOOTLOADER_CODE LoadProgramRowFromFlash( uint32 sourceAddress, uint32 length, BYTE* program_row_buffer, uint32* program_row_buffer_length )
{
	if ( length > PROGRAM_ROW_SIZE )
		length = PROGRAM_ROW_SIZE;

	*program_row_buffer_length = length;
	return mem_read( sourceAddress, program_row_buffer, length );
}
static bool _BOOTLOADER_CODE WriteProgramRow( uint32 destinationAddress, uint32 length, BYTE* program_row_buffer, uint32 program_row_buffer_length )
{
	Erase( hi_wordu( destinationAddress ), lo_wordu( destinationAddress ), PM_ROW_ERASE );

	uint16 size,size1;
	uint32 temp;

	for(size = 0,size1=0; size < length; size++)
	{
		temp = make32( make16( program_row_buffer[size1], program_row_buffer[size1+1] ), 
			             make16( program_row_buffer[size1+2], 0 ) );
		size1 += 3;

	  WriteLatch( hi_wordu( destinationAddress ), lo_wordu( destinationAddress ), hi_wordu( temp ), lo_wordu( temp ) );

		destinationAddress += 2;
	}
	return true;
}
static bool _BOOTLOADER_CODE ActuallyBootloadApplicationFirmware( uint32 sourceAddress, uint32 length )
{
	BYTE program_row_buffer[PROGRAM_ROW_SIZE];
	uint32 program_row_buffer_length;
	uint32 destinationAddress = APPLICATION_PROGRAM_ADDRESS;
	while ( length > 0 )
	{
		uint32 tmpLength = PROGRAM_ROW_SIZE;
		if ( length < PROGRAM_ROW_SIZE )
			tmpLength = length;

		if ( !LoadProgramRowFromFlash( sourceAddress, tmpLength, program_row_buffer, &program_row_buffer_length ) )
			return false;
		if ( !WriteProgramRow( destinationAddress, tmpLength, program_row_buffer, program_row_buffer_length ) )
			return false;
		
		destinationAddress += tmpLength;
		sourceAddress += tmpLength;
		length -= tmpLength;
	}

	return true;
}

static inline void RestoreApplicationFirmware()
{
	if ( !BackupExists() )
		return;
	ActuallyBootloadApplicationFirmware( BOOTLOADER_FLASH_BACKUP_FIRMWARE_START, BackupFirmwareLength() );
}

static const BYTE firmware_header[BOOTLOADER_FLASH_FIRMWARE_OFFSET] = {0,0,0,0};
static bool ActuallyCacheFirmwareForBootloading( uint32 addr, FirmwareRowCallback load_buffer_callback )
{
	uint8 buff_length = BOOTLOADER_FLASH_FIRMWARE_OFFSET;
	const BYTE* buff = firmware_header;
	uint32 flash_addr = BOOTLOADER_FLASH_BLOCK_START;
	uint32 remaining_clear = 0;
	uint8  subsections_used = 0;

	while ( buff_length > 0 )
	{
		if ( remaining_clear < buff_length )
		{
			if ( subsections_used >= BOOTLOADER_MAX_FIRMWARE_SIZE/MEMORY_SUBSECTION_SIZE+1 )
				return false;
			mem_clear_subsection( flash_addr+buff_length );
			remaining_clear += MEMORY_SUBSECTION_SIZE;
			++subsections_used;
		}

		if ( !mem_write( flash_addr, buff, buff_length ) )
			return false;
		flash_addr += buff_length;
		remaining_clear -= buff_length;
		buff_length = load_buffer_callback( flash_addr-BOOTLOADER_FLASH_FIRMWARE_START, &buff );
	}

	flash_addr -= BOOTLOADER_FLASH_FIRMWARE_START; // flash_addr => firmware_length
	if ( !mem_write( BOOTLOADER_FLASH_BLOCK_START+1, ((BYTE*)&flash_addr)+1, 3 ) )
		return false;
	if ( !mem_write_byte( BOOTLOADER_FLASH_BLOCK_START, true ) )
		return false;
	return true;
}

bool CacheFirmwareForBootloading( FirmwareRowCallback load_buffer_callback )
{
	return ActuallyCacheFirmwareForBootloading( BOOTLOADER_FLASH_BLOCK_START, load_buffer_callback );
}

/*
static uint8 BackupApplicationFirmwareRow( uint32 addr, const BYTE** buff_ptr )
{
	program_row_buffer_length = 0;
	addr += APPLICATION_PROGRAM_ADDRESS;
	while ( program_row_buffer_length+3 < 256 ); // flash memory page size
	{
		*(uint32*)( program_row_buffer + program_row_buffer_length ) = ReadLatch( hi_wordu( addr ), lo_wordu( addr ) );
		program_row_buffer_length += 3;
		addr += 2;
	}

	*buff_ptr = program_row_buffer;
	return program_row_buffer_length;
}
bool BackupApplicationFirmware()
{
	return ActuallyCacheFirmwareForBootloading( BOOTLOADER_FLASH_BACKUP_START, BackupApplicationFirmwareRow );
}
*/

bool _BOOTLOADER_CODE BootloadApplicationFirmware()
{
	/*if ( !BackupApplicationFirmware() )
		return false;
*/

	if ( !ActuallyBootloadApplicationFirmware( BOOTLOADER_FLASH_FIRMWARE_START, BootloaderFirmwareLength() ) )
	{
	//	RestoreApplicationFirmware();
		return false;
	}
	return true; //TODO
}

int _BOOTLOADER_CODE RunBootloadedApplication()
{
	asm ("goto %0" :: "r"(APPLICATION_PROGRAM_ADDRESS));
	return 0;
}