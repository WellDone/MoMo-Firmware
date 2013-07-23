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
	plist_set_int16(0, module_count);
	
	bus_slave_setreturn(pack_return_status(kNoMIBError, kIntSize));
}

void register_module(void)
{
	if ( module_count == MAX_MODULES 
	     || plist_get_buffer_length() != sizeof( momo_module_descriptor ) )
	{
		//TODO: Better error granularity
		bus_slave_seterror( kUnknownError );
		return;
	}

	memcpy( (void*)(&the_modules[module_count]), plist_get_buffer(0), plist_get_buffer_length() );

	plist_set_int16( 0, MODULE_BASE_ADDRESS + module_count );
	bus_slave_setreturn( pack_return_status(kNoMIBError, kIntSize) );
	++module_count;
}

void describe_module(void)
{
	unsigned long index = plist_get_int16(0);
	if ( index >= module_count )
	{
		bus_slave_seterror( kUnknownError );
		return;
	}

	memcpy( (void*) plist_get_buffer(0), &the_modules[index], sizeof(momo_module_descriptor) );
	
	bus_slave_setreturn( pack_return_status( kNoMIBError, sizeof(momo_module_descriptor) ) );
}

void cleanup_unresponsive_modules() // Schedule periodically
{
	//TODO: Ping all assigned addresses and dump the ones that don't respond.
}

DEFINE_MIB_FEATURE_COMMANDS(controller) {
	{0x00, register_module, plist_spec(0,true) },
	{0x01, get_module_count, plist_spec_empty() },
	{0x02, describe_module, plist_spec(1,false) }
};
DEFINE_MIB_FEATURE(controller);