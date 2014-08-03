#include "module_manager.h"
#include <string.h>

static momo_module_descriptor the_modules[MAX_MODULES];
static uint8 the_module_count = 0;

uint8 add_module( momo_module_descriptor* module )
{
	if ( the_module_count == MAX_MODULES )
		return 0;
	memcpy( (void*)(&the_modules[the_module_count]), module, sizeof( momo_module_descriptor ) );
	return get_module_address( the_module_count++ );
}
uint8 module_count()
{
	return the_module_count;
}
momo_module_descriptor* get_module( uint8 index )
{
	if ( index > the_module_count )
		return NULL;
	return &the_modules[index];
}
uint8 get_module_address( uint8 index )
{
	return MODULE_BASE_ADDRESS + index;
}

void clear_modules()
{
	the_module_count = 0;
}

ModuleIterator create_module_iterator( uint8 type )
{
	ModuleIterator iter;
	iter.module_type = type;
	iter.current_index = 0;
	iter.started = false;
	return iter;
}

uint8 module_iter_address( ModuleIterator* iter )
{
	return get_module_address( iter->current_index );
}
momo_module_descriptor* module_iter_get( ModuleIterator* iter )
{
	if ( !iter->started || iter->current_index == MAX_MODULES )
		return NULL;
	return &the_modules[iter->current_index];
}

momo_module_descriptor* module_iter_next( ModuleIterator* iter )
{
	if ( !iter->started )
		iter->started = true;
	else
		iter->current_index += 1;
	while ( iter->current_index < MAX_MODULES && the_modules[iter->current_index].module_type != iter->module_type )
	{
		iter->current_index += 1;
	}
	if ( iter->current_index == MAX_MODULES )
		return NULL;
	return &the_modules[iter->current_index];
}