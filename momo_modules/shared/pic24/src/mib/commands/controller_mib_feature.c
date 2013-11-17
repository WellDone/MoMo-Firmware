#include "bus_slave.h"
#include "controller_mib_feature.h"
#include <string.h>
#include "memory.h"
#include "mib_definitions.h"
#include "mib_feature_definition.h"
#include "adc.h"

#define MAX_MODULES 8
#define MODULE_BASE_ADDRESS 11
static momo_module_descriptor the_modules[MAX_MODULES];
static unsigned int module_count = 0;

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

void cleanup_unresponsive_modules() // Schedule periodically
{
	//TODO: Ping all assigned addresses and dump the ones that don't respond.
}

void get_battery_voltage()
{
	ADCConfig batt_adc_config;
	unsigned int last_battery_voltage;

	//Store ADC configuration
    batt_adc_config.output_format = kUIntegerFormat;
    batt_adc_config.trigger = kInternalCounter;
    batt_adc_config.reference = kVDDVSS;
    batt_adc_config.enable_in_idle = 0;
    batt_adc_config.sample_autostart = 1;
    batt_adc_config.scan_input = 0;
    batt_adc_config.alternate_muxes = 0;
    batt_adc_config.autosample_wait = 0b11111;

    batt_adc_config.oneshot = 1;
    batt_adc_config.num_samples = 1;

    adc_configure(&batt_adc_config);

    //Battery monitor is on RA2
    _PCFG2 = 0;
    _TRISA2 = 1;

    adc_set_channel(4);

    last_battery_voltage = adc_convert_one();

    mib_buffer[0] = last_battery_voltage & 0xFF;
    mib_buffer[1] = last_battery_voltage >> 8;

	bus_slave_setreturn( pack_return_status( kNoMIBError, 2));
}

DEFINE_MIB_FEATURE_COMMANDS(controller) {
	{0x00, register_module, plist_spec(0,true) },
	{0x01, get_module_count, plist_spec_empty() },
	{0x02, describe_module, plist_spec(1,false) },
	{0x03, get_battery_voltage, plist_spec_empty()}
};
DEFINE_MIB_FEATURE(controller);