#include "bus_slave.h"
#include "controller_mib_feature.h"
#include <string.h>

#include "mib_features.h"

#define MAX_MODULES 8
#define MODULE_BASE_ADDRESS 10
static momo_module_descriptor the_modules[MAX_MODULES];
static unsigned int module_count = 0;

static void* get_module_count(MIBParamList *param)
{	
	MIBIntParameter *retval;

	bus_free_all();

	retval = (MIBIntParameter*)bus_allocate_int_param();

	bus_init_int_param(retval, module_count );
	bus_slave_setreturn(kNoMIBError, (MIBParameterHeader*)retval);

	return NULL;
}

static void* register_module(MIBParamList *list)
{
	MIBBufferParameter *buf = (MIBBufferParameter *)list->params[0];

	if ( module_count == MAX_MODULES 
	     || buf->header.len != sizeof( momo_module_descriptor ) )
	{
		//TODO: Better error granularity
		bus_slave_seterror( kUnknownError );
		return NULL;
	}

	memcpy( (void*)(&the_modules[module_count]), buf->data, buf->header.len );

	MIBIntParameter *retval;

	bus_free_all();

	retval = (MIBIntParameter*)bus_allocate_int_param();

	bus_init_int_param(retval, MODULE_BASE_ADDRESS + module_count );
	bus_slave_setreturn(kNoMIBError, (MIBParameterHeader*)retval);

	++module_count;
	return NULL;
}

static void* describe_module(MIBParamList *list)
{
	unsigned long index = get_uint16_param(list, 0);
	if ( index >= module_count )
	{
		bus_slave_seterror( kUnknownError );
		return NULL;
	}

	bus_free_all();
	unsigned char* return_buf;
	MIBParameterHeader *retval = (MIBBufferParameter*)bus_allocate_return_buffer(&return_buf);

	if ( retval->len < sizeof( momo_module_descriptor ) ) {
		bus_slave_seterror( kUnknownError ); //TODO: This should happen at a lower level
		return NULL;
	}

	retval->len = sizeof( momo_module_descriptor );
	memcpy( (void*) return_buf, &the_modules[index], retval->len );
	
	bus_slave_setreturn( kNoMIBError, retval );
	return NULL;
}

DEFINE_MIB_FEATURE_COMMANDS(controller) {
	{0, register_module, plist_define1(kMIBBufferType) },
	{1, get_module_count, plist_define0() },
	{2, describe_module, plist_define1(kMIBInt16Type) }
};
DEFINE_MIB_FEATURE(controller);