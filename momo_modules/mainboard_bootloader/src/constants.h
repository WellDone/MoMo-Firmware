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
#define	kFlashRowSizeInstructions 32		//Instructions
#define ALARMPIN				  _RB0
#define ALARMTRIS				  _TRISB0
#else
#error "Unsupported chip type"
#endif

#define kFlashRowSizeWords		(2*kFlashRowSizeInstructions)
#define kNumFirmwareRows		((kNumFlashInstructions - kBootloaderSize) / kFlashRowSizeInstructions)

#define kAppJumpRow				(0x100 / kFlashRowSizeWords)

#define kBootloaderGotoLow 		0x040200ULL
#define kBootloaderGotoHigh		0x000000ULL

#endif