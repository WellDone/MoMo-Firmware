#include "bus_slave.h"
#include "controller_mib_feature.h"
#include <string.h>
#include "memory.h"

#include "mib_features.h"

#define MAX_MODULES 8
#define MODULE_BASE_ADDRESS 10
static momo_module_descriptor the_modules[MAX_MODULES];
static unsigned int module_count = 0;

void get_module_count(void)
{	
	loadparams(plist_1param(kMIBInt16Type));
	set_intparam(0, module_count);
	
	bus_slave_setreturn(kNoMIBError | kHasReturnValue);
}

void register_module(void)
{
	MIBBufferParameter *buf = get_buffer_param(0);

	if ( module_count == MAX_MODULES 
	     || buf->header.len != sizeof( momo_module_descriptor ) )
	{
		//TODO: Better error granularity
		bus_slave_seterror( kUnknownError );
		return;
	}

	memcpy( (void*)(&the_modules[module_count]), buf->data, buf->header.len );

	loadparams(plist_1param(kMIBInt16Type));
	set_intparam( 0, MODULE_BASE_ADDRESS + module_count );

	bus_slave_setreturn( kNoMIBError | kHasReturnValue );
	++module_count;
}

void describe_module(void)
{
	unsigned long index = get_uint16_param(0);
	if ( index >= module_count )
	{
		bus_slave_seterror( kUnknownError );
		return;
	}

	loadparams(plist_1param(kMIBBufferType));
	MIBBufferParameter *retval = get_buffer_param(0);

	/*if ( retval->len < sizeof( momo_module_descriptor ) ) {
		bus_slave_seterror( kUnknownError ); //TODO: This should happen at a lower level, like when we allocate the buffer.
		return NULL;
	}*/

	retval->header.len = sizeof( momo_module_descriptor );
	memcpy( (void*) retval->data, &the_modules[index], retval->header.len );
	
	bus_slave_setreturn( kNoMIBError | kHasReturnValue );
}

void cleanup_unresponsive_modules() // Schedule periodically
{
	//TODO: Ping all assigned addresses and dump the ones that don't respond.
}

typedef struct {
	uint8 		  module_type; // Registered module address instead?
	uint8 		  last_sequence;
	unsigned long firmware_length;
	unsigned long base_address;
	unsigned long offset_bytes;
} firmware_bucket;
#define kFirmwareDumpedSuccessfully 0
#define kFirmwareBucketBaseAddress  0xFF
static firmware_bucket the_firmware_bucket = {0,0,0,kFirmwareBucketBaseAddress}; //TODO: Move somewhere better, have multiple buckets.

void start_dump_firmware(void)
{
	unsigned long p0 = get_uint16_param(0);

	uint8 module_type = p0 >> 8;
	uint8 sequence_number = p0 & 0xFF; // Should be 0 for a start.
	if ( the_firmware_bucket.module_type == 0 && sequence_number == 0) {
		the_firmware_bucket.module_type = module_type;
		the_firmware_bucket.last_sequence = 0;
		the_firmware_bucket.firmware_length = get_uint16_param(1);
	} else {
		// ERROR, until we have multiple buckets
	}

	//Should we just say "ok" before starting to actually get the firmware?
	MIBBufferParameter *data = get_buffer_param(2);
	mem_write( /*address*/the_firmware_bucket.base_address+the_firmware_bucket.offset_bytes, data->data, data->header.len ); // Synchronous.  Probably ok?
	the_firmware_bucket.offset_bytes += data->header.len;

	loadparams(plist_1param(kMIBInt16Type));
	set_intparam( 0, the_firmware_bucket.offset_bytes ); // We expect the caller to use this new offset to call us again with the next chunk

	bus_slave_setreturn( kNoMIBError | kHasReturnValue );
}

void dump_firmware_chunk(void)
{
	unsigned long p0 = get_uint16_param(0);

	uint8 module_type = p0 >> 8;
	uint8 sequence_number = p0 & 0xFF;
	if ( module_type != the_firmware_bucket.module_type
	     || the_firmware_bucket.offset_bytes == kFirmwareDumpedSuccessfully ) {
		// ERROR, until we have multiple buckets
	}

	MIBBufferParameter *data = get_buffer_param(1);
	mem_write( /*address*/the_firmware_bucket.base_address+the_firmware_bucket.offset_bytes, data->data, data->header.len ); // Synchronous.  Probably ok?
	the_firmware_bucket.offset_bytes += data->header.len;
	the_firmware_bucket.last_sequence = sequence_number;

	//TODO: What happens if we get an invalid sequence number...?

	if ( the_firmware_bucket.offset_bytes >= the_firmware_bucket.firmware_length ) {
		// We're done.  TODO: Notify people.
		the_firmware_bucket.offset_bytes = kFirmwareDumpedSuccessfully;
		the_firmware_bucket.last_sequence = 0;
	}

	loadparams(plist_1param(kMIBInt16Type));
	set_intparam( 0, the_firmware_bucket.offset_bytes ); // We expect the caller to use this new offset to call us again with the next chunk

	bus_slave_setreturn( kNoMIBError | kHasReturnValue );
}



DEFINE_MIB_FEATURE_COMMANDS(controller) {
	{0x00, register_module, plist_define1(kMIBBufferType) },
	{0x01, get_module_count, plist_define0() },
	{0x02, describe_module, plist_define1(kMIBInt16Type) },
	{0x10, start_dump_firmware, plist_define3(kMIBInt16Type,kMIBInt16Type,kMIBBufferType) },
	{0x11, dump_firmware_chunk, plist_define2(kMIBInt16Type,kMIBBufferType) }
};
DEFINE_MIB_FEATURE(controller);