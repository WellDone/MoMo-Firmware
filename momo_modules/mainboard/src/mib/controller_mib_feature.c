#include "bus_slave.h"
#include "controller_mib_feature.h"
#include <string.h>
#include "memory.h"
#include "mib_definitions.h"
#include "mib_feature_definition.h"
#include "adc.h"
#include "flashblock.h"
#include "common.h"
#include "battery.h"
#include "eeprom.h"

#define MAX_MODULES 8
#define MODULE_BASE_ADDRESS 11

static momo_module_descriptor the_modules[MAX_MODULES];
static unsigned int module_count = 0;
static flash_block_info fb_info;

void con_init()
{
	BUS_ENABLE_TRIS = 1;
	BUS_ENABLE_DIG = 1;
	BUS_ENABLE_LAT = 1;
}

void con_reset_bus()
{
	BUS_ENABLE_LAT = 1;
	BUS_ENABLE_TRIS = 0;

	module_count = 0;
	DELAY_MS(50);

	BUS_ENABLE_TRIS = 1;
}

void get_module_count(void)
{	
	bus_slave_return_int16( module_count );
}

void register_module(void)
{
	if ( module_count == MAX_MODULES 
	     || plist_get_buffer_length() != sizeof( momo_module_descriptor ) )
	{
		//TODO: Better error granularity
		bus_slave_seterror( kCallbackError ); //TODO: User error
		return;
	}

	memcpy( (void*)(&the_modules[module_count]), plist_get_buffer(0), sizeof( momo_module_descriptor ) );

	bus_slave_return_int16( MODULE_BASE_ADDRESS + module_count );
	++module_count;
}

void describe_module(void)
{
	unsigned long index = plist_get_int16(0);
	if ( index >= module_count )
	{
		bus_slave_seterror( kCallbackError ); //TODO: User error
		return;
	}
	
	bus_slave_return_buffer( (const char*)&the_modules[index], sizeof(momo_module_descriptor) );
}

void read_flash_rpc()
{
	uint32 addr;

	addr = plist_get_int16(1);
	addr <<= 16;
	addr |= plist_get_int16(0);

	mem_read( addr, plist_get_buffer(0), kBusMaxMessageSize);
	bus_slave_setreturn(pack_return_status( kNoMIBError, kBusMaxMessageSize));
}

void write_flash_rpc()
{
	uint32 addr;

	addr = plist_get_int16(1);
	addr <<= 16;
	addr |= plist_get_int16(0);

	mem_write(addr, plist_get_buffer(2), plist_get_buffer_length());

	plist_set_int16(0, (addr & 0xFFFF));
	plist_set_int16(1, (addr >> 16));
	bus_slave_setreturn(pack_return_status(kNoMIBError, 4));
}

void erase_subsection_rpc()
{
	uint32 addr;

	addr = plist_get_int16(1);
	addr <<= 16;
	addr |= plist_get_int16(0);

	mem_clear_subsection(addr);
	bus_slave_setreturn(pack_return_status(kNoMIBError, 0));
}

void test_fb_init()
{
	uint16 sub = plist_get_int16(0);
	uint16 val;
	
	val = fb_init(&fb_info, sub, 20);

	plist_set_int16(0, val);
	plist_set_int16(1, fb_info.magic);
	plist_set_int16(2, fb_info.subsector);
	plist_set_int16(3, fb_info.current);
	plist_set_int16(4, fb_info.item_size);
	plist_set_int16(5, fb_info.bin_shift);
	bus_slave_setreturn(pack_return_status(kNoMIBError, 12));
}

void test_fb_write()
{
	fb_write(&fb_info, plist_get_buffer(0));
	bus_slave_return_int16(fb_info.current);
}

void test_fb_read()
{
	fb_read(&fb_info, plist_get_buffer(0));
	bus_slave_setreturn(pack_return_status(kNoMIBError, 20));
}

void reflash_self()
{
	eeprom_write(0, 0xAA);
	asm volatile("reset");
}


DEFINE_MIB_FEATURE_COMMANDS(controller) {
	{0x00, register_module, plist_spec(0,true) },
	{0x01, get_module_count, plist_spec_empty() },
	{0x02, describe_module, plist_spec(1,false)},
	{0x03, read_flash_rpc, plist_spec(2, false)},
	{0x04, write_flash_rpc, plist_spec(2, true)},
	{0x05, con_reset_bus, plist_spec_empty()},
	{0x06, erase_subsection_rpc, plist_spec(2, false)},
	{0x07, test_fb_init, plist_spec(1, false)},
	{0x08, test_fb_write, plist_spec(0, true)},
	{0x09, test_fb_read, plist_spec(0, false)},
	{0x0A, reflash_self, plist_spec_empty()},
	{0x0B, report_battery, plist_spec_empty()}
};
DEFINE_MIB_FEATURE(controller);