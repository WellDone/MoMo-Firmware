#include "bus_slave.h"
#include "controller_mib_feature.h"
#include <string.h>

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

DEFINE_MIB_FEATURE_COMMANDS(controller) {
	{0, register_module, plist_define1(kMIBBufferType) },
	{1, get_module_count, plist_define0() },
	{2, describe_module, plist_define1(kMIBInt16Type) }
};
DEFINE_MIB_FEATURE(controller);