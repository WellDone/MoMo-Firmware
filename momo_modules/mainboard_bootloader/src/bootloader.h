#ifndef __bootloader_h__
#define __bootloader_h__

#define _BOOTLOADER_CODE __attribute__((section(".bootloader")))

#define APPLICATION_PROGRAM_ADDRESS            0xFF0000

#define BOOTLOADER_MAX_FIRMWARE_SIZE           0x4000 //16kb for pic24f16ka101
#define BOOTLOADER_FLASH_FIRMWARE_OFFSET       0x4 // One byte for "ready", 3 for length

#define BOOTLOADER_FLASH_BLOCK_START           0x0 //TODO
#define BOOTLOADER_FLASH_FIRMWARE_START        (BOOTLOADER_FLASH_BLOCK_START+BOOTLOADER_FLASH_FIRMWARE_OFFSET)

#define BOOTLOADER_FLASH_BACKUP_START          (BOOTLOADER_FLASH_FIRMWARE_START+BOOTLOADER_MAX_FIRMWARE_SIZE)
#define BOOTLOADER_FLASH_BACKUP_FIRMWARE_START (BOOTLOADER_FLASH_BACKUP_START+BOOTLOADER_FLASH_FIRMWARE_OFFSET)

#include "pic24.h"

bool BootloadPending();

bool BootloadApplicationFirmware();
int  RunBootloadedApplication();

typedef uint8 (*FirmwareRowCallback)( uint32 addr, const BYTE** buffer_ptr ); //TODO: Allow arbitrary addresses per callback
bool CacheFirmwareForBootloading( FirmwareRowCallback callback );

#endif