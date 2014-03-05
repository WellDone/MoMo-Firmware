#ifndef __constants_h__
#define __constants_h__

#include "memory.h"

#define kMainFirmwareSector		2ULL
#define kBackupFirmwareSector	3ULL

#define kMainFirmwareAddress	MEMORY_SECTION_ADDR(kMainFirmwareSector)
#define kBackupFirmwareAddress	MEMORY_SECTION_ADDR(kBackupFirmwareSector)

#define kBootloaderSize			512		//Instructions

#ifdef __PIC24F16KA101__
#define kNumFlashInstructions	5632
#define	kFlashRowSize			32		//Instructions
#else
#error "Unsupported chip type"
#endif

#define kNumFirmwareRows		((kNumFlashInstructions - kBootloaderSize) / kFlashRowSize)

#endif