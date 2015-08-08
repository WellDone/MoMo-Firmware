#include "common_types.h"
#include "memory.h"
#include "mib_feature_definition.h"
#include "intel_hex.h"
#include "system_log.h"
#include "firmware_cache.h"

//transient state during a firmware push event
static bool 	firmware_push_started;
static uint8 	current_bucket;

//persistent state stored in a flash_block
static firmware_cache_state fc_state;
static flash_block_info		fc_flashlog;

static const uint8 				bucket_subsectors[kNumFirmwareBuckets] = { \
	MEMORY_SECTION_TO_SUB(kMIBFirmwareSector) + 0*MAX_FIRMWARE_SUBSECTIONS, \
	MEMORY_SECTION_TO_SUB(kMIBFirmwareSector) + 1*MAX_FIRMWARE_SUBSECTIONS, \
	MEMORY_SECTION_TO_SUB(kMIBFirmwareSector) + 2*MAX_FIRMWARE_SUBSECTIONS, \
	MEMORY_SECTION_TO_SUB(kMIBFirmwareSector) + 3*MAX_FIRMWARE_SUBSECTIONS, \
	MEMORY_SECTION_TO_SUB(kControllerFirmwareSector), \
	MEMORY_SECTION_TO_SUB(kBackupFirmwareSector) \
};

static const uint8				bucket_lengths[kNumFirmwareBuckets] = { \
	MAX_FIRMWARE_SUBSECTIONS, \
	MAX_FIRMWARE_SUBSECTIONS, \
	MAX_FIRMWARE_SUBSECTIONS, \
	MAX_FIRMWARE_SUBSECTIONS, \
	MAX_CONTROLLER_SUBSECTIONS, \
	MAX_CONTROLLER_SUBSECTIONS \
};

void fc_init( uint8 memory_subsection )
{
	fb_init(&fc_flashlog, memory_subsection, sizeof(firmware_cache_state), kFirmwareCacheStructureVersion);

	if (fb_count(&fc_flashlog) > 0)
		fb_read(&fc_flashlog, &fc_state);
}

uint8_t fc_startpush(uint8_t length)
{
	uint8 type = plist_get_int16(0)&0xFF;
	uint8 i;

	if (type == kMIBControllerFirmware)
		current_bucket = kControllerFirmwareBucket;
	else if (type == kMIBBackupControllerFirmware)
		current_bucket = kControllerBackupBucket;
	else
	{
		if ( fc_state.count == kNumModuleFirmwareBuckets )
		{
			return kCallbackError;
		}

		current_bucket = fc_state.count;
	}

	fc_state.buckets[current_bucket].module_type = type;
	fc_state.buckets[current_bucket].firmware_length = 0;
	
	for ( i = 0; i < bucket_lengths[current_bucket]; ++i )
	{
		uint32 addr = (MEMORY_SUBSECTION_ADDR(bucket_subsectors[current_bucket]));
		addr +=  i*MEMORY_SUBSECTION_SIZE;
		mem_clear_subsection(addr);
	}

	firmware_push_started = true;

	//Inform everyone that we do expect to use the flash memory again very soon
	//so it should not be turned off when we sleep.
	taskloop_set_flag(kTaskLoopLightSleepBit, 1);

	bus_slave_return_int16(current_bucket);
	return kNoErrorStatus;
}

uint8_t fc_push(uint8_t call_length)
{
	if ( !firmware_push_started ) 
	{
		return kCallbackError;
	}

	//TODO: Validate length
	intel_hex16_body* hex = (intel_hex16_body*)plist_get_buffer(0); // Exactly 19 bytes, yay!

	if ( hex->record_type == HEX_DATA_REC )
	{
		// PARSE HEX16 AND DUMP TO FLASH
		uint32 addr = hex->address;
		uint8 length = call_length-3; /*address and record_type*/
		uint32 max_addr = addr + length;
		uint32 max_size = MEMORY_SUBSECTION_SIZE*bucket_lengths[current_bucket];

		//Make sure we do not overwrite past the end of our bucket.
		//This gets rid of the configuration bits that we cannot flash anyways
		
		//BUGFIX: Note that max_addr is perhaps poorly named because it is actually 1 greater
		//than the maximum address we write to, so we need to compare > the size of our bucket
		//not >=, otherwise we will incorrectly cut off the last row of large firmware images
		if ((max_addr > max_size)) 
			return kNoErrorStatus;

		if ( max_addr > fc_state.buckets[current_bucket].firmware_length)
			fc_state.buckets[current_bucket].firmware_length = max_addr;

		addr += MEMORY_SUBSECTION_ADDR(bucket_subsectors[current_bucket]);

		mem_write( addr, hex->data, length );
	}
	else if ( hex->record_type == HEX_EOF_REC )
	{
		firmware_push_started = false;

		if (current_bucket < kNumModuleFirmwareBuckets)
			++fc_state.count;

		//Update our persistent store with the new data
		fb_write(&fc_flashlog, &fc_state);

		//Inform everyone that we don't expect to use the flash memory very soon.
		taskloop_set_flag(kTaskLoopLightSleepBit, 0);
	}
	//Ignore high address record sections since we don't support addresses path the first 64kb for controller firmware
	//and past the first 16kb for module firmware.

	return kNoErrorStatus;
}

uint8_t fc_cancelpush(uint8_t length)
{
	firmware_push_started = false;
	return kNoErrorStatus;
}

uint8_t fc_getinfo(uint8_t length)
{
	uint8 index = plist_get_int16(0)&0xFF;

	//Make sure the index is valid and that there's a valid firmware there
	if ( index >= kNumFirmwareBuckets) 
	{
		return kCallbackError;
	}

	if (fc_state.buckets[index].firmware_length == 0)
	{
		return kCallbackError;
	}

	plist_set_int16(0, fc_state.buckets[index].module_type );
	plist_set_int16(1, fc_state.buckets[index].firmware_length >> 16 );
	plist_set_int16(2, fc_state.buckets[index].firmware_length & 0xFFFF );
	plist_set_int16(3, (MEMORY_SUBSECTION_ADDR(bucket_subsectors[index])>>16)&0xFFFF);
	plist_set_int16(4, (MEMORY_SUBSECTION_ADDR(bucket_subsectors[index]))&0xFFFF);
	plist_set_int16(5, bucket_subsectors[index]);
	plist_set_int16(6, bucket_lengths[index]);

	bus_slave_set_returnbuffer_length( 7*2 ); // TODO: Is there a better way to return multiple ints?
	return kNoErrorStatus;
}

uint8_t fc_pull(uint8_t length)
{
	uint8 index = plist_get_int16(0)&0xFF; //TODO: Use module id/address instead?
	uint16 offset = plist_get_int16(1);

	//Make sure they are requesting a valid firmware and are using a valid offset
	if ( index >= kNumFirmwareBuckets) 
	{
		return kCallbackError;
	}

	if (offset >= fc_state.buckets[index].firmware_length) 
	{
		return kCallbackError;
	}

	//TODO: Check to make sure firmware type matches device type.  No pic12 code on a pic24 please

	uint32 address = MEMORY_SUBSECTION_ADDR(bucket_subsectors[index]);
	address += offset;

	uint32 chunk_size = fc_state.buckets[index].firmware_length - offset;
	if ( chunk_size > kMIBBufferSize )
		chunk_size = kMIBBufferSize;

	uint8 ret_size = chunk_size & 0xFF;

	mem_read( address, plist_get_buffer(0), ret_size );
	bus_slave_set_returnbuffer_length( ret_size );

	return kNoErrorStatus;
}

uint8_t fc_getcount(uint8_t length)
{
	unsigned int con = 0;

	plist_set_int16(0, fc_state.count);

	if (fc_state.buckets[kControllerFirmwareBucket].firmware_length > 0)
		con |= 1<<0;

	if (fc_state.buckets[kControllerBackupBucket].firmware_length > 0)
		con |= 1<<1;

	plist_set_int16(1, con);
	bus_slave_set_returnbuffer_length(4);

	return kNoErrorStatus;
}

uint8_t fc_clear(uint8_t length)
{
	uint8 i;

	fc_state.count = 0;
	firmware_push_started = false;
	
	for (i=0; i<kNumFirmwareBuckets; ++i)
		fc_state.buckets[i].firmware_length = 0;

	//Update our persistent store with the new data
	fb_write(&fc_flashlog, &fc_state);
	return kNoErrorStatus;
}

DEFINE_MIB_FEATURE_COMMANDS(firmware_cache) 
{
	{0x00, fc_startpush},
	{0x01, fc_push},
	{0x02, fc_cancelpush},
	{0x03, fc_getinfo},
	{0x04, fc_pull},
	{0x05, fc_getcount},
	{0x0A, fc_clear}
};

DEFINE_MIB_FEATURE(firmware_cache);