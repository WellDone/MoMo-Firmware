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
#include "rtcc.h"
#include "i2c.h"
#include "bus.h"
#include "module_manager.h"
#include "system_log.h"
#include "memory_manager.h"
#include "perf.h"
#include "log_definitions.h"
#include "rn4020.h"

#include "momo_config.h"
#include "sensor_event_log.h"

static flash_block_info fb_info;

static unsigned int _BOOTLOADER_VAR reflash __attribute__((persistent));

unsigned int debug_flag_value = 0;

void con_init()
{
	//Make all pins digital if they could be analog
	ENSURE_DIGITAL(SCL);
	ENSURE_DIGITAL(SDA);
	ENSURE_DIGITAL(ALARM);
	ENSURE_DIGITAL(BUS_ENABLE);

	LAT(BUS_ENABLE) = 0;
	DIR(BUS_ENABLE) = OUTPUT;

	DIR(ALARM) = INPUT;
	LAT(ALARM) = 1;

	con_reset_bus();
}

void con_reset_bus()
{
	i2c_disable();

	LAT(SCL) = 0;
	LAT(SDA) = 0;
	LAT(ALARM) = 0;

	DIR(SCL) = OUTPUT;
	DIR(SDA) = OUTPUT;
	DIR(ALARM) = OUTPUT;

	//We need to wait for all of the modules to reset and disable
	//any functionality on their boards that may parasitically
	//power the bus when we cut power to it.  100 ms is tested to
	//be long enough for the gsm_module.  10 ms is too short.
	DELAY_MS(100);

	//Bus disable FET is active high to remove power
	//from the bus.
	LAT(BUS_ENABLE) = 1;
	DIR(BUS_ENABLE) = OUTPUT;

	clear_modules();

	DELAY_MS(50);

	DIR(SCL) = INPUT;
	DIR(SDA) = INPUT;
	DIR(ALARM) = INPUT;

	LAT(BUS_ENABLE) = 0;

	bus_init(kMIBControllerAddress);
}

void get_module_count(void)
{	
	bus_slave_return_int16( module_count() );
}

void register_module(void)
{
	if ( plist_get_buffer_length() != sizeof( momo_module_descriptor ) )
	{
		//TODO: Better error granularity
		bus_slave_seterror( kCallbackError );
		return;
	}
	uint8 addr = add_module( (momo_module_descriptor*)plist_get_buffer(0) );
	if ( addr == 0 )
	{
		//TODO: Better error granularity
		bus_slave_seterror( kCallbackError );
		return;	
	}

	LOG_DEBUG(kSubmoduleAddressRequestNotice);
	
	bus_slave_return_int16( addr );
}

void describe_module(void)
{
	unsigned long index = plist_get_int16(0);
	if ( index >= module_count() )
	{
		bus_slave_seterror( kCallbackError ); //TODO: User error
		return;
	}
	
	bus_slave_return_buffer( (const char*)get_module(index), sizeof(momo_module_descriptor) );
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
	LOG_CRITICAL(kControllerReflashNotice);
	LOG_FLUSH();
	reflash = kReflashMagic;
	asm volatile("reset");
}

void reset_self()
{
	LOG_CRITICAL(kControllerResetNotice);
	LOG_FLUSH();
	asm volatile("reset");
}

void factory_reset()
{
	LOG_CRITICAL(kControllerFactoryResetNotice);
	mem_clear_all();
	flash_memory_init();
}

void current_time()
{
	rtcc_datetime t;

	rtcc_get_time(&t);

	plist_set_int16(0, t.year);
	plist_set_int16(1, t.month);
	plist_set_int16(2, t.day);
	plist_set_int16(3, t.hours);
	plist_set_int16(4, t.minutes);
	plist_set_int16(5, t.seconds);

	bus_slave_setreturn(pack_return_status(kNoMIBError, 12));
}

void set_time()
{
	if ( plist_get_buffer_length() != 8 )
		return bus_slave_seterror( kCallbackError );
	rtcc_set_time( (rtcc_datetime*)plist_get_buffer(0) );
}

void debug_value()
{
	bus_slave_return_int16(debug_flag_value);
}

void set_sleep()
{
	if (plist_get_int16(0))
		taskloop_set_flag(kTaskLoopSleepBit, 1);
	else
		taskloop_set_flag(kTaskLoopSleepBit, 0);
}

void write_log()
{
	//FIXME: Re-enable remote logging
	//write_system_log( kRemoteLog, plist_get_buffer(0), plist_get_buffer_length() );
}

void log_count()
{
	bus_slave_return_int16( system_log_count() );
}
void read_log()
{
	GenericLogEntry log_buffer;
	if (!read_system_log( plist_get_int16(0), &log_buffer ))
		bus_slave_seterror(kCallbackError);
	else
		bus_slave_return_buffer((BYTE*)(&log_buffer), sizeof(GenericLogEntry));
}
void clear_log()
{
	clear_system_log();
}

void read_ram()
{
	unsigned char *val = (unsigned char *)(plist_get_int16(0));
	bus_slave_return_buffer(val, 20);
}

void get_perf_counter()
{
	uint16 counter = plist_get_int16(0);
	const performance_counter *val = perf_get_counter((PerformanceCounter)counter);

	bus_slave_return_buffer(val, 20);
}

void get_uuid()
{
	plist_set_int16( 0, current_momo_state.uuid & 0xFFFF );
	plist_set_int16( 1, (current_momo_state.uuid >> 16) & 0xFFFF );
	bus_slave_set_returnbuffer_length( 4 );
}

void set_uuid()
{
	current_momo_state.uuid = makeu32(plist_get_int16(1), plist_get_int16(0));
	save_momo_state();
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
	{0x0B, report_battery, plist_spec_empty()},
	{0x0C, current_time, plist_spec_empty()},
	{0x0D, debug_value, plist_spec_empty()},
	{0x0E, set_sleep, plist_spec(1, false)},
	{0x0F, reset_self, plist_spec_empty()},
	{0x10, factory_reset, plist_spec_empty()},
	{0x11, read_ram, plist_spec(1, false)},
	{0x12, set_time, plist_spec(0, true)},

	{0x13, get_uuid, plist_spec(0, false)},
	{0x14, set_uuid, plist_spec(2, false)},

	{0x20, write_log, plist_spec(0, true)},
	{0x21, log_count, plist_spec_empty()},
	{0x22, read_log, plist_spec(2, false)},
	{0x23, clear_log, plist_spec_empty() },

	{0x26, get_perf_counter, plist_spec(1, false)},
	{0x27, bt_debug_buffer, plist_spec(0, false)}
};
DEFINE_MIB_FEATURE(controller);