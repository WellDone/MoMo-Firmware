//bus.c

#include "bus.h"

volatile MIBState mib_state;

void bus_init()
{
	I2CConfig config;

	config.address = 0x05;
	config.priority = 0b010;

	i2c_init_flags(&config);
	i2c_set_flag(&config, kEnableGeneralCallFlag);
	i2c_set_flag(&config, kEnableSoftwareClockStretchingFlag);

	i2c_configure(&config);
	i2c_enable();
}