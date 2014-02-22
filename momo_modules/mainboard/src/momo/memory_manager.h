#ifndef __memory_manager_h__
#define __memory_manager_h__

#include "memory.h"

#define kConfigurationSector 		0
#define kMIBFirmwareSector			1
#define kControllerFirmwareSector	2
#define kReservedSector				3
#define kSensorDataSector			4
#define kNumFlashSectors			16

#define kSensorLogSubsectors		((kNumFlashSectors - kSensorDataSector)*16)

//ConfigurationSector subsector assignments
enum
{
	kMomoConfigSubsector	= MEMORY_SECTION_TO_SUB(kConfigurationSector) + 0
};

void flash_memory_init();

#endif