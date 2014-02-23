#include "common_types.h"
#include "memory_manager.h"
#include "memory.h"
#include "mib_feature_definition.h"
#include "intel_hex.h"

static uint8 	firmware_bucket_count;
static bool 	firmware_push_started;
static uint8 	current_bucket;

typedef struct 
{
	uint8  module_type;
	uint32 firmware_length;
	uint32 base_address;
	uint8  addr_high;
} firmware_bucket;

//Controller bucket must be placed directly after other buckets for the math 
//in push_firmware_start to work.
#define kFirmwareBucketBaseAddress  (MEMORY_SECTION_ADDR(kMIBFirmwareSector))
#define MAX_FIRMWARE_SUBSECTIONS	4
#define MAX_FIRMWARE_SIZE           ((uint32)MEMORY_SUBSECTION_SIZE * MAX_FIRMWARE_SUBSECTIONS)
#define MAX_CONTROLLER_SUBSECTIONS	16
#define kControllerBucketAddress  	(MEMORY_SECTION_ADDR(kControllerFirmwareSector))


#define kNumModuleFirmwareBuckets		4
#define kNumControllerFirmwareBuckets 	2
#define kControllerFirmwareBucket 		kNumModuleFirmwareBuckets
#define kControllerBackupBucket			(kControllerFirmwareBucket+1)
#define kNumFirmwareBuckets         	(kNumModuleFirmwareBuckets + kNumControllerFirmwareBuckets)

static firmware_bucket 				the_firmware_buckets[kNumFirmwareBuckets];

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

void push_firmware_start(void)
{
	uint8 type = plist_get_int16(0)&0xFF;
	uint8 i;

	if (type == kMIBControllerFirmware)
		current_bucket = kControllerFirmwareBucket;
	else if (type == kMIBBackupControllerFirmware)
		current_bucket = kControllerBackupBucket;
	else
	{
		if ( firmware_bucket_count == kNumFirmwareBuckets )
		{
			bus_slave_seterror(kCallbackError);
			return;
		}

		current_bucket = firmware_bucket_count;
	}

	the_firmware_buckets[current_bucket].module_type = type;
	the_firmware_buckets[current_bucket].firmware_length = 0;
	the_firmware_buckets[current_bucket].base_address = MEMORY_SUBSECTION_ADDR(bucket_subsectors[current_bucket]);
	the_firmware_buckets[current_bucket].addr_high = 0;
	
	for ( i = 0; i < bucket_lengths[current_bucket]; ++i )
		mem_clear_subsection( the_firmware_buckets[current_bucket].base_address + i*MEMORY_SUBSECTION_SIZE );

	firmware_push_started = true;
	bus_slave_return_int16(current_bucket);
}

void push_firmware_chunk(void)
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
		uint32 addr = make32(the_firmware_buckets[current_bucket].addr_high, hex->address);
		uint8 length = plist_get_buffer_length()-3; /*address and record_type*/
		uint32 max_addr = addr + length;
		uint32 max_size = MEMORY_SUBSECTION_SIZE*bucket_lengths[current_bucket];

		//Make sure we do not overwrite past the end of our bucket.
		//This gets rid of the configuration bits that we cannot flash anyways
		if ((max_addr >= max_size)) 
			return;

		if ( max_addr > the_firmware_buckets[current_bucket].firmware_length)
			the_firmware_buckets[current_bucket].firmware_length = max_addr;

		addr += the_firmware_buckets[current_bucket].base_address;

		mem_write( addr, hex->data, length );
	}
	else if ( hex->record_type == HEX_EXTADDR_REC )
	{
		//TODO.  This would actually break things currently because of 16-bit limitations.
		the_firmware_buckets[current_bucket].addr_high = hex->data[1]; // Only need the lower 8 bits
	}
	else if ( hex->record_type == HEX_EOF_REC )
	{
		firmware_push_started = false;

		if (current_bucket < kNumModuleFirmwareBuckets)
			++firmware_bucket_count;
	}
}

void push_firmware_cancel(void)
{
	firmware_push_started = false;
}

void get_firmware_info(void)
{
	uint8 index = plist_get_int16(0)&0xFF;

	//Make sure the index is valid and that there's a valid firmware there
	if ( index >= kNumFirmwareBuckets) 
	{
		bus_slave_seterror(kCallbackError);
		return;
	}

	if (the_firmware_buckets[index].firmware_length == 0)
	{
		bus_slave_seterror(kCallbackError);
		return;
	}

	plist_set_int16(0, the_firmware_buckets[index].module_type );
	plist_set_int16(1, the_firmware_buckets[index].firmware_length >> 16 );
	plist_set_int16(2, the_firmware_buckets[index].firmware_length & 0xFFFF );
	plist_set_int16(3, (the_firmware_buckets[index].base_address>>16)&0xFFFF);
	plist_set_int16(4, (the_firmware_buckets[index].base_address)&0xFFFF);
	plist_set_int16(5, bucket_subsectors[index]);
	plist_set_int16(6, bucket_lengths[index]);

	bus_slave_set_returnbuffer_length( 7*kIntSize ); // TODO: Is there a better way to return multiple ints?
}
void pull_firmware_chunk(void)
{
	uint8 index = plist_get_int16(0)&0xFF; //TODO: Use module id/address instead?
	uint16 offset = plist_get_int16(1);

	//Make sure they are requesting a valid firmware and are using a valid offset
	if ( index >= kNumFirmwareBuckets) 
	{
		bus_slave_seterror(kCallbackError);
		return;
	}

	if (offset >= the_firmware_buckets[index].firmware_length ) 
	{
		bus_slave_seterror(kCallbackError);
		return;
	}

	//TODO: Check to make sure firmware type matches device type.  No pic12 code on a pic24 please

	uint32 address = the_firmware_buckets[index].base_address + offset;
	uint32 chunk_size = the_firmware_buckets[index].firmware_length - offset;
	if ( chunk_size > kBusMaxMessageSize )
		chunk_size = kBusMaxMessageSize;

	uint8 ret_size = chunk_size & 0xFF;

	mem_read( address, plist_get_buffer(0), ret_size );
	bus_slave_set_returnbuffer_length( ret_size );
}

void get_firmware_count(void)
{
	unsigned int con = 0;

	plist_set_int16(0, firmware_bucket_count);

	if (the_firmware_buckets[kControllerFirmwareBucket].firmware_length > 0)
		con |= 1<<0;

	if (the_firmware_buckets[kControllerBackupBucket].firmware_length > 0)
		con |= 1<<1;

	plist_set_int16(1, con);
	bus_slave_set_returnbuffer_length(4);
}

void clear_firmware_cache(void)
{
	uint8 i;

	firmware_bucket_count = 0;
	firmware_push_started = false;
	
	for (i=0; i<kNumFirmwareBuckets; ++i)
		the_firmware_buckets[i].firmware_length = 0;
}

DEFINE_MIB_FEATURE_COMMANDS(firmware_cache) 
{
	{0x00, push_firmware_start, plist_spec( 1, false ) },
	{0x01, push_firmware_chunk, plist_spec( 0, true ) },
	{0x02, push_firmware_cancel, plist_spec_empty() },
	{0x03, get_firmware_info, plist_spec( 1, false ) },
	{0x04, pull_firmware_chunk, plist_spec( 2, false ) },
	{0x05, get_firmware_count, plist_spec_empty() },
	{0x0A, clear_firmware_cache, plist_spec_empty() }
};

DEFINE_MIB_FEATURE(firmware_cache);