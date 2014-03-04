#include "common_types.h"
#include "memory_manager.h"
#include "memory.h"
#include "mib_feature_definition.h"
#include "intel_hex.h"
#include "firmware_cache.h"

//transient state during a firmware push event
static bool 	firmware_push_started;
static uint8 	current_bucket;

//persistent state stored in a flash_block
static firmware_cache_state fc_state;
static flash_block_info		fc_flashlog;

static const uint8 				bucket_subsectors[kNumFirmwareBuckets] = { \
	MEMORY_SECTION_TO_SUB(kMIBFirmwareSector) + 0, \
	MEMORY_SECTION_TO_SUB(kMIBFirmwareSector) + 1, \
	MEMORY_SECTION_TO_SUB(kMIBFirmwareSector) + 2, \
	MEMORY_SECTION_TO_SUB(kMIBFirmwareSector) + 3, \
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

void fc_init()
{
	fb_init(&fc_flashlog, kFirmwareConfigSubector, sizeof(firmware_cache_state));

	if (fb_count(&fc_flashlog) > 0)
		fb_read(&fc_flashlog, &fc_state);
}

void fc_startpush(void)
{
	uint8 type = plist_get_int16(0)&0xFF;
	uint8 i;

	if (type == kMIBControllerFirmware)
		current_bucket = kControllerFirmwareBucket;
	else if (type == kMIBBackupControllerFirmware)
		current_bucket = kControllerBackupBucket;
	else
	{
		if ( fc_state.count == kNumFirmwareBuckets )
		{
			bus_slave_seterror(kCallbackError);
			return;
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
	bus_slave_return_int16(current_bucket);
}

void fc_push(void)
{
	if ( !firmware_push_started ) 
	{
		bus_slave_seterror(kCallbackError);
		return;
	}

	//TODO: Validate length
	intel_hex16_body* hex = (intel_hex16_body*)plist_get_buffer(0); // Exactly 19 bytes, yay!

	if ( hex->record_type == HEX_DATA_REC )
	{
		// PARSE HEX16 AND DUMP TO FLASH
		uint32 addr = hex->address;
		uint8 length = plist_get_buffer_length()-3; /*address and record_type*/
		uint32 max_addr = addr + length;
		uint32 max_size = MEMORY_SUBSECTION_SIZE*bucket_lengths[current_bucket];

		//Make sure we do not overwrite past the end of our bucket.
		//This gets rid of the configuration bits that we cannot flash anyways
		if ((max_addr >= max_size)) 
			return;

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
	}
	//Ignore high address record sections since we don't support addresses path the first 64kb for controller firmware
	//and past the first 16kb for module firmware.
}

void fc_cancelpush(void)
{
	firmware_push_started = false;
}

void fc_getinfo(void)
{
	uint8 index = plist_get_int16(0)&0xFF;

	//Make sure the index is valid and that there's a valid firmware there
	if ( index >= kNumFirmwareBuckets) 
	{
		bus_slave_seterror(kCallbackError);
		return;
	}

	if (fc_state.buckets[index].firmware_length == 0)
	{
		bus_slave_seterror(kCallbackError);
		return;
	}

	plist_set_int16(0, fc_state.buckets[index].module_type );
	plist_set_int16(1, fc_state.buckets[index].firmware_length >> 16 );
	plist_set_int16(2, fc_state.buckets[index].firmware_length & 0xFFFF );
	plist_set_int16(3, (MEMORY_SUBSECTION_ADDR(bucket_subsectors[index])>>16)&0xFFFF);
	plist_set_int16(4, (MEMORY_SUBSECTION_ADDR(bucket_subsectors[index]))&0xFFFF);
	plist_set_int16(5, bucket_subsectors[index]);
	plist_set_int16(6, bucket_lengths[index]);

	bus_slave_set_returnbuffer_length( 7*kIntSize ); // TODO: Is there a better way to return multiple ints?
}
void fc_pull(void)
{
	uint8 index = plist_get_int16(0)&0xFF; //TODO: Use module id/address instead?
	uint16 offset = plist_get_int16(1);

	//Make sure they are requesting a valid firmware and are using a valid offset
	if ( index >= kNumFirmwareBuckets) 
	{
		bus_slave_seterror(kCallbackError);
		return;
	}

	if (offset >= fc_state.buckets[index].firmware_length) 
	{
		bus_slave_seterror(kCallbackError);
		return;
	}

	//TODO: Check to make sure firmware type matches device type.  No pic12 code on a pic24 please

	uint32 address = MEMORY_SUBSECTION_ADDR(bucket_subsectors[index]);
	address += offset;

	uint32 chunk_size = fc_state.buckets[index].firmware_length - offset;
	if ( chunk_size > kBusMaxMessageSize )
		chunk_size = kBusMaxMessageSize;

	uint8 ret_size = chunk_size & 0xFF;

	mem_read( address, plist_get_buffer(0), ret_size );
	bus_slave_set_returnbuffer_length( ret_size );
}

void fc_getcount(void)
{
	unsigned int con = 0;

	plist_set_int16(0, fc_state.count);

	if (fc_state.buckets[kControllerFirmwareBucket].firmware_length > 0)
		con |= 1<<0;

	if (fc_state.buckets[kControllerBackupBucket].firmware_length > 0)
		con |= 1<<1;

	plist_set_int16(1, con);
	bus_slave_set_returnbuffer_length(4);
}

void fc_clear(void)
{
	uint8 i;

	fc_state.count = 0;
	firmware_push_started = false;
	
	for (i=0; i<kNumFirmwareBuckets; ++i)
		fc_state.buckets[i].firmware_length = 0;

	//Update our persistent store with the new data
	fb_write(&fc_flashlog, &fc_state);
}

DEFINE_MIB_FEATURE_COMMANDS(firmware_cache) 
{
	{0x00, fc_startpush, plist_spec( 1, false ) },
	{0x01, fc_push, plist_spec( 0, true ) },
	{0x02, fc_cancelpush, plist_spec_empty() },
	{0x03, fc_getinfo, plist_spec( 1, false ) },
	{0x04, fc_pull, plist_spec( 2, false ) },
	{0x05, fc_getcount, plist_spec_empty() },
	{0x0A, fc_clear, plist_spec_empty() }
};

DEFINE_MIB_FEATURE(firmware_cache);