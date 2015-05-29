#ifndef __module_manager_h__
#define __module_manager_h__

#include "platform.h"
#include "mib_definitions.h"

#define MAX_MODULES 32
#define MODULE_BASE_ADDRESS 11

#define kMIBCommunicationType 10

typedef struct {
	uint8 module_type;
	uint8 current_index;
	bool started;
} ModuleIterator;

uint8 add_module( momo_module_descriptor* module );
uint8 module_count();
momo_module_descriptor* get_module( uint8 index );
uint8 get_module_address( uint8 index );
void clear_modules();

ModuleIterator create_module_iterator( uint8 module_type );
uint8 module_iter_address( ModuleIterator* iter );
momo_module_descriptor* module_iter_get( ModuleIterator* iter );
momo_module_descriptor* module_iter_next( ModuleIterator* iter );

#endif