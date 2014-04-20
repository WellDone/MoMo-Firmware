#ifndef __memory_manager_h__
#define __memory_manager_h__

#include "memory.h"

#define kConfigurationSector 		  0
#define kMIBFirmwareSector			  1 			//Must be right before ControllerFirmwareSector
#define kControllerFirmwareSector	2			//Cannot be moved (see push_firmware_start for details)
#define kBackupFirmwareSector		  3
#define kSensorDataSector			    4

#define kNumFlashSectors			    16
#define kSensorLogSubsectors		  ((kNumFlashSectors - kSensorDataSector)*16)

//ConfigurationSector subsector assignments
enum
{
	kMomoConfigSubsector    = MEMORY_SECTION_TO_SUB(kConfigurationSector) + 0,
	kFirmwareConfigSubector	= MEMORY_SECTION_TO_SUB(kConfigurationSector) + 1,

	kReportLogSubsector     = MEMORY_SECTION_TO_SUB(kConfigurationSector) + 8, //SIZE: 4
	kDebugLogSubsector      = MEMORY_SECTION_TO_SUB(kConfigurationSector) + 12 //SIZE: 4
};

void flash_memory_init();

#endif