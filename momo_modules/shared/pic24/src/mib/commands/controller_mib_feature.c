#include "bus_slave.h"
#include "controller_mib_feature.h"
#include <string.h>

#include "mib_features.h"

static momo_module_descriptor the_modules[8];
static unsigned int module_count = 0;

static void* list_modules(MIBParamList *param)
{	
	MIBIntParameter *retval;

	bus_free_all();

	retval = (MIBIntParameter*)bus_allocate_int_param();

	bus_init_int_param(retval, 6);
	bus_slave_setreturn(kNoMIBError, (MIBParameterHeader*)retval);

	return NULL;
}

static void* register_module(MIBParamList *param)
{	
	MIBIntParameter *retval;

	bus_free_all();

	retval = (MIBIntParameter*)bus_allocate_int_param();

	bus_init_int_param(retval, 6);
	bus_slave_setreturn(kNoMIBError, (MIBParameterHeader*)retval);

	return NULL;
}

DEFINE_MIB_FEATURE_COMMANDS(controller) {
	{0, list_modules, plist_define0() },
	{1, register_module, plist_define0() }
};
DEFINE_MIB_FEATURE(controller);