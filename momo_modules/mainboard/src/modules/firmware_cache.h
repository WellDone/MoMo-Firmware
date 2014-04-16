#ifndef __firmware_cache_h__
#define __firmware_cache_h__

#include "memory_manager.h"
#include "flashblock.h"

#define kNumModuleFirmwareBuckets		4
#define kNumControllerFirmwareBuckets 	2
#define kControllerFirmwareBucket 		kNumModuleFirmwareBuckets
#define kControllerBackupBucket			(kControllerFirmwareBucket+1)
#define kNumFirmwareBuckets         	(kNumModuleFirmwareBuckets + kNumControllerFirmwareBuckets)

//Controller bucket must be placed directly after other buckets for the math 
//in push_firmware_start to work.
#define kFirmwareBucketBaseAddress  (MEMORY_SECTION_ADDR(kMIBFirmwareSector))
#define MAX_FIRMWARE_SUBSECTIONS	4
#define MAX_FIRMWARE_SIZE           ((uint32)MEMORY_SUBSECTION_SIZE * MAX_FIRMWARE_SUBSECTIONS)
#define MAX_CONTROLLER_SUBSECTIONS	16
#define kControllerBucketAddress  	(MEMORY_SECTION_ADDR(kControllerFirmwareSector))

//6 bytes
typedef struct 
{
	uint8  module_type;
	uint8  reserved;
	uint32 firmware_length;
} firmware_bucket;

//1 + 6*6 = 37 Bytes
typedef struct
{
	firmware_bucket		buckets[kNumFirmwareBuckets];
	uint8				count;
} firmware_cache_state;

//Externally Callable Module Functions
void fc_init( uint8 memory_subsection );
#endif