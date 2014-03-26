#include "battery.h"
#include "uart.h"
#include "scheduler.h"
#include "adc.h"
#include "bus.h"
#include "mib_feature_definition.h"

static ScheduledTask battery_task;
static ADCConfig batt_adc_config;

int charging_allowed = 0;
unsigned int last_battery_voltage = 0;

void battery_init()
{
	//Configure pin for measuring battery voltage
	BATTERY_VOLTAGE_TRIS = 1;
	BATTERY_VOLTAGE_DIGITAL = 0;

    //Battery measurement enable is active low Open drain control. 
    //Always turn the FET on b/c it's a 1 uA draw from the battery
    BATTERY_MEASURE_TRIS = 0;
    BATTERY_MEASURE_EN = 0;

	//Configure charge controller pin (0 disables charging)
	CHARGE_LATCH = 0;
	CHARGE_TRIS = 0;

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

    charging_allowed = 1;

	scheduler_schedule_task(battery_callback, kEvery10Seconds, kScheduleForever, &battery_task);
}

void battery_callback()
{
	if (!charging_allowed)
		return;

	adc_configure(&batt_adc_config);
    adc_set_channel(4);
    last_battery_voltage = adc_convert_one();

    //Disable charging if battery voltage is greater than max charge level
    if (last_battery_voltage >= kBatteryChargedLevel)
    	disable_charging();
    else if (last_battery_voltage < kBatteryHysteresisLevel) //Reenable charging once battery level falls below hysteresis
    	enable_charging();
}

void battery_set_charging_allowed(int allowed)
{
	charging_allowed = allowed;

	if (!allowed)
		disable_charging();
}

void report_battery()
{
    unsigned int voltage;

    adc_configure(&batt_adc_config);
    adc_set_channel(4);
    voltage = adc_convert_one();

    bus_slave_return_int16(voltage);
}