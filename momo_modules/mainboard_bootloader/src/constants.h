#ifndef __constants_h__
#define __constants_h__

#include "memory.h"

#define kMainFirmwareSector			2ULL
#define kBackupFirmwareSector		3ULL

#define kMainFirmwareAddress		MEMORY_SECTION_ADDR(kMainFirmwareSector)
#define kBackupFirmwareAddress		MEMORY_SECTION_ADDR(kBackupFirmwareSector)

#define kBootloaderSize				1024		//Instructions

#ifdef __PIC24FJ64GA306__
#define kNumFlashInstructions 		22016
#define kFlashRowSizeInstructions 	64
#define kFlashPageSizeInstructions	512

#define ALARM 						D3
#define DATA						G3

#define kHardwareString				"con_v3.1"

#else
#error "Unsupported chip type"
#endif

#define kFlashRowSizeWords		(2*kFlashRowSizeInstructions)
#define kFlashPageSizeWords		(2*kFlashPageSizeInstructions)

#define kNumFirmwarePages		((kNumFlashInstructions - kBootloaderSize) / kFlashPageSizeInstructions)
#define kNumFirmwareRows		((kNumFlashInstructions - kBootloaderSize) / kFlashRowSizeInstructions)
#define kNumRowsPerPage			(kFlashPageSizeInstructions / kFlashRowSizeInstructions)

#define kAppJumpRow				(0x100 / kFlashRowSizeWords)

#define kBootloaderGotoHighByte 0x04

#define kBootloaderMagicEEPROMLoc 0

#endif