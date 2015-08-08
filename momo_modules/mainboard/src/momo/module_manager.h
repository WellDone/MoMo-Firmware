#ifndef __module_manager_h__
#define __module_manager_h__

#include "platform.h"
#include "mib_definitions.h"
#include <stdint.h>

#define MAX_MODULES 32
#define MODULE_BASE_ADDRESS 11

uint8_t 				add_module( momo_module_descriptor* module );
uint8_t 				module_count();
momo_module_descriptor* get_module( uint8 index );
uint8_t 				get_module_address( uint8 index );
void 					clear_modules();
uint8_t 				find_module_by_name(const char *name);

#endif