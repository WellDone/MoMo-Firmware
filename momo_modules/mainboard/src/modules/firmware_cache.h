#ifndef __firmware_cache_h__
#define __firmware_cache_h__

#include "memory_manager.h"
#include "flashblock.h"

#define kNumModuleFirmwareBuckets		4
#define kNumControllerFirmwareBuckets 	2
#define kControllerFirmwareBucket 		kNumModuleFirmwareBuckets
#define kControllerBackupBucket			(kControllerFirmwareBucket+1)
#define kNumFirmwareBuckets         	(kNumModuleFirmwareBuckets + kNumControllerFirmwareBuckets)

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
void fc_init();
#endif