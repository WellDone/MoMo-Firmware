#include "firmware_cache_mib_feature.h"
#include "common_types.h"
#include "protocol.h"
#include "bus_slave.h"
#include "memory.h"
#include "mib_feature_definition.h"
#include "intel_hex.h"

static BYTE firmware_chunk_cache[kBusMaxMessageSize];
static uint8 __attribute__((space(data))) firmware_bucket_count;
static bool __attribute__((space(data))) firmware_push_started;
typedef struct {
	uint8  module_type;
	uint32 base_address;
	uint32 firmware_length;
	uint8  addr_high;
} firmware_bucket;
#define kFirmwareBucketBaseAddress  MEMORY_SUBSECTION_SIZE * 100 // probably an ok spot.
// 16kb memory - the maximum for pic24.  pic12 is 2000 words (3kb?) so we could have a bunch o smaller buckets
#define MAX_FIRMWARE_SIZE           MEMORY_SUBSECTION_SIZE * 16
#define kNumFirmwareBuckets         8
static firmware_bucket __attribute__((space(data))) the_firmware_buckets[kNumFirmwareBuckets]; //TODO: Move somewhere better, have multiple buckets.

void push_firmware_start(void)
{
	uint8 type = get_uint16_param(0)&0xFF;
	unsigned long length = get_uint16_param(1);
	uint8 i, index;

	if ( firmware_bucket_count == kNumFirmwareBuckets )
	{
		//ERROR
		return;
	}
	index = firmware_bucket_count;

	the_firmware_buckets[index].module_type = type;
	the_firmware_buckets[index].firmware_length = length;
	the_firmware_buckets[index].base_address = kFirmwareBucketBaseAddress + index * MAX_FIRMWARE_SIZE;
	the_firmware_buckets[index].addr_high = 0;
	
	for ( i = 0; i < MAX_FIRMWARE_SIZE/MEMORY_SUBSECTION_SIZE; ++i )
		mem_clear_subsection( the_firmware_buckets[index].base_address + i*MEMORY_SUBSECTION_SIZE );

	firmware_push_started = true;

	loadparams(plist_1param(kMIBInt16Type));
	set_intparam( 0, index ); // We expect the caller to use this new offset to call us again with the next chunk

	bus_slave_setreturn( kNoMIBError | kHasReturnValue );
}

void push_firmware_chunk(void)
{
	if ( !firmware_push_started ) {
		//ERROR
		return;
	}
	MIBBufferParameter *buf = get_buffer_param(0);
	intel_hex16_body* hex = (intel_hex16_body*)&buf->data; // Exactly 19 bytes, yay!

	if ( hex->record_type == HEX_DATA_REC )
	{
		// PARSE HEX16 AND DUMP TO FLASH
		uint32 addr = make32(the_firmware_buckets[firmware_bucket_count].addr_high, hex->address);

		//TODO: Since we only have a uin16 as the firmware_length, 
		//      sending a really large firmware will actually break here.
		//      We never use the top higher bits (>16) of addr.
		if ( addr > the_firmware_buckets[firmware_bucket_count].firmware_length ) {
			//ERROR
			firmware_push_started = false;
			return;
		}
		addr += the_firmware_buckets[firmware_bucket_count].base_address;

		mem_write( addr, hex->data, buf->header.len - 3 /*address and record_type*/ );
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

void pull_firmware_chunk(void)
{
	uint8 index = get_uint16_param(0)&0xFF; //TODO: Use module id/address instead?
	uint16 offset = get_uint16_param(1);

	if ( index < firmware_bucket_count || offset > the_firmware_buckets[index].firmware_length ) {
		//ERROR
	}

	//TODO: Check to make sure firmware type matches device type.  No pic12 code on a pic24 please

	uint16 address = the_firmware_buckets[index].base_address + offset;

	mem_read( address, firmware_chunk_cache, sizeof(firmware_chunk_cache) );
}

void get_firmware_count(void)
{
	loadparams(plist_define1(kMIBInt16Type));
	set_intparam( 0, firmware_bucket_count );
	bus_slave_setreturn( kNoMIBError | kHasReturnValue );
}

DEFINE_MIB_FEATURE_COMMANDS(firmware_cache) {
	{0x00, push_firmware_start, plist_define2(kMIBInt16Type,kMIBInt16Type) },
	{0x01, push_firmware_chunk, plist_define1(kMIBBufferType) },
	{0x04, pull_firmware_chunk, plist_define2(kMIBInt16Type,kMIBInt16Type) },
	{0x05, get_firmware_count, plist_define0() }
};
DEFINE_MIB_FEATURE(firmware_cache);