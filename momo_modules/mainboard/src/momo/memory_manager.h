#ifndef __memory_manager_h__
#define __memory_manager_h__

#include "memory.h"

#define kConfigurationSector			0
#define kMIBFirmwareSector				1 			//Must be right before ControllerFirmwareSector
#define kControllerFirmwareSector		2				//Cannot be moved (see push_firmware_start for details)
#define kBackupFirmwareSector			3
#define kSensorDataSector				4
#define kLogSector 						15

#define kSensorLogSubsectors		  	((kLogSector - kSensorDataSector)*16)

#define kNumFlashSectors			    16

//ConfigurationSector subsector assignments
enum
{
	kMomoConfigSubsector    		= MEMORY_SECTION_TO_SUB(kConfigurationSector) + 0,
	kFirmwareConfigSubector			= MEMORY_SECTION_TO_SUB(kConfigurationSector) + 1,
	kRN4020ConfigSubsector			= MEMORY_SECTION_TO_SUB(kConfigurationSector) + 2,
	kConfigManagerControlSubsector 	= MEMORY_SECTION_TO_SUB(kConfigurationSector) + 3,
	kConfigManagerDataSubsector		= MEMORY_SECTION_TO_SUB(kConfigurationSector) + 4,
	kTestSector1					= MEMORY_SECTION_TO_SUB(kConfigurationSector) + 5
};

//LogSector subsector assignments
enum
{
	kSystemLogSubsector = MEMORY_SECTION_TO_SUB(kLogSector) + 0, // 7 subsections / 64b = 445 entries
	kReportLogSubsector = MEMORY_SECTION_TO_SUB(kLogSector) + 8  // 7 subsections / 118b ~= 240 entries
};

void flash_memory_init();

#endif