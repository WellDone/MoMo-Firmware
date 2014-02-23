#include "common_types.h"
#include "memory_manager.h"
#include "memory.h"
#include "mib_feature_definition.h"
#include "intel_hex.h"

static uint8 __attribute__((space(data))) firmware_bucket_count;
static bool __attribute__((space(data))) firmware_push_started;
typedef struct 
{
	uint8  module_type;
	uint32 base_address;
	uint32 firmware_length;
	uint8  addr_high;
} firmware_bucket;

#define kFirmwareBucketBaseAddress  (MEMORY_SECTION_ADDR(kMIBFirmwareSector))
#define kControllerBucketAddress  	(MEMORY_SECTION_ADDR(kControllerFirmwareSector))
#define MAX_FIRMWARE_SIZE           ((uint32)MEMORY_SUBSECTION_SIZE * 4)
#define kNumFirmwareBuckets         4

static firmware_bucket __attribute__((space(data))) the_firmware_buckets[kNumFirmwareBuckets];

void push_firmware_start(void)
{
	uint8 type = plist_get_int16(0)&0xFF;
	uint8 i, index;

	if ( firmware_bucket_count == kNumFirmwareBuckets )
	{
		bus_slave_seterror(kCallbackError);
		return;
	}
	index = firmware_bucket_count;

	the_firmware_buckets[index].module_type = type;
	the_firmware_buckets[index].firmware_length = 0;
	the_firmware_buckets[index].base_address = kFirmwareBucketBaseAddress + (uint32)index * (uint32)MAX_FIRMWARE_SIZE;
	the_firmware_buckets[index].addr_high = 0;
	
	for ( i = 0; i < MAX_FIRMWARE_SIZE/MEMORY_SUBSECTION_SIZE; ++i )
		mem_clear_subsection( the_firmware_buckets[index].base_address + i*MEMORY_SUBSECTION_SIZE );

	firmware_push_started = true;

	bus_slave_return_int16( index );
}

void push_firmware_chunk(void)
{
	if ( !firmware_push_started ) {
		bus_slave_seterror(kCallbackError);
		return;
	}
	//TODO: Validate length
	intel_hex16_body* hex = (intel_hex16_body*)plist_get_buffer(0); // Exactly 19 bytes, yay!

	if ( hex->record_type == HEX_DATA_REC )
	{
		// PARSE HEX16 AND DUMP TO FLASH
		uint32 addr = make32(the_firmware_buckets[firmware_bucket_count].addr_high, hex->address);
		uint8 length = plist_get_buffer_length()-3; /*address and record_type*/

		if ( addr+length > MAX_FIRMWARE_SIZE ) 
		{
			// Drop this chunk on the floor, we can't flash configuration bits
			return;
		}

		if ( addr+length > the_firmware_buckets[firmware_bucket_count].firmware_length)
			the_firmware_buckets[firmware_bucket_count].firmware_length = addr+length;

		addr += the_firmware_buckets[firmware_bucket_count].base_address;

		mem_write( addr, hex->data, length );
	}
	else if ( hex->record_type == HEX_EXTADDR_REC )
	{
		//TODO.  This would actually break things currently because of 16-bit limitations.
		//       Shouldn't matter for the pic12's small memory footprint
		the_firmware_buckets[firmware_bucket_count].addr_high = hex->data[1]; // Only need the lower 8 bits
	}
	else if ( hex->record_type == HEX_EOF_REC )
	{
		// We're done!
		++firmware_bucket_count;
		firmware_push_started = false;
	}
}

void push_firmware_cancel(void)
{
	firmware_push_started = false;
}

void get_firmware_info(void)
{
	uint8 index = plist_get_int16(0)&0xFF;
	if ( index >= firmware_bucket_count ) {
		bus_slave_seterror(kCallbackError);
		return;
	}
	plist_set_int16(0, the_firmware_buckets[index].module_type );
	plist_set_int16(1, the_firmware_buckets[index].firmware_length >> 16 );
	plist_set_int16(2, the_firmware_buckets[index].firmware_length & 0xFFFF );
	plist_set_int16(3, (the_firmware_buckets[index].base_address>>16)&0xFFFF);
	plist_set_int16(4, (the_firmware_buckets[index].base_address)&0xFFFF);

	bus_slave_set_returnbuffer_length( 5*kIntSize ); // TODO: Is there a better way to return multiple ints?
}
void pull_firmware_chunk(void)
{
	uint8 index = plist_get_int16(0)&0xFF; //TODO: Use module id/address instead?
	uint16 offset = plist_get_int16(1);

	if ( index >= firmware_bucket_count || offset > the_firmware_buckets[index].firmware_length ) {
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
	bus_slave_return_int16( firmware_bucket_count );
}

void clear_firmware_cache(void)
{
	firmware_bucket_count = 0;
	firmware_push_started = false;
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