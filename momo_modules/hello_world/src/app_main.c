//app_main.c

#include "platform.h"
#include "watchdog.h"
#include "port.h"
#include "sample.h"
#include "mib12_api.h"
#include "digital_amp.h"
#include "watchdog.h"
#include <stdint.h>

extern unsigned int adc_result;

void task(void)
{ 
	
}

void interrupt service_isr() 
{

}

void initialize(void)
{
	PIN_DIR(VOLT1, INPUT);
	PIN_TYPE(VOLT1, ANALOG);

	PIN_DIR(VOLT2, INPUT);
	PIN_TYPE(VOLT2, ANALOG);
	
	ENSURE_DIGITAL(AN_POWER);
	LATCH(AN_POWER) = 0;
	PIN_DIR(AN_POWER, OUTPUT);

	ENSURE_DIGITAL(AN_SELECT);
	LATCH(AN_SELECT) = 0;
	PIN_DIR(AN_SELECT, OUTPUT);

	ENSURE_DIGITAL(AN_INVERT);
	LATCH(AN_INVERT) = 0;
	PIN_DIR(AN_INVERT, OUTPUT);

	ENSURE_DIGITAL(AN_PROG);
	LATCH(AN_PROG) = 0;
	PIN_DIR(AN_PROG, OUTPUT);

	PIN_TYPE(AN_VOLTAGE, ANALOG);
	PIN_DIR(AN_VOLTAGE, INPUT);

	ENSURE_DIGITAL(PULSE_IN);
	LATCH(PULSE_IN) = 0;
	PIN_DIR(PULSE_IN, OUTPUT);

	//Setup serial port
	ENSURE_DIGITAL(SERIAL_TX);
	LATCH(SERIAL_TX) = 1;
	PIN_DIR(SERIAL_TX, OUTPUT);

	ENSURE_DIGITAL(SERIAL_RX);

	damp_init();
}

void main()
{
	initialize();

	while(1)
	{
		task();
		asm("sleep");
	}
}

uint8_t check_v1(uint8_t length)
{
	sample_v1();

	mib_buffer[0] = (adc_result & 0xFF);
	mib_buffer[1] = (adc_result >> 8) & 0xFF;

	bus_slave_returndata(2);
	return kNoErrorStatus;
}

uint8_t check_v2(uint8_t length)
{
	sample_v2();

	mib_buffer[0] = (adc_result & 0xFF);
	mib_buffer[1] = (adc_result >> 8) & 0xFF;

	bus_slave_returndata(2);
	return kNoErrorStatus;
}

uint8_t acquire_voltage(uint8_t length)
{
	damp_enable();
	sample_analog();
	damp_disable();

	mib_buffer[0] = (adc_result & 0xFF);
	mib_buffer[1] = (adc_result >> 8) & 0xFF;

	bus_slave_returndata(2);
	return kNoErrorStatus;
}

uint8_t set_damp_parameter(uint8_t length)
{
	if (mib_buffer[0] >= kNumParameters)
		return 6;

	damp_set_parameter((AmplifierSetting)mib_buffer[0], mib_buffer[2]);
	
	return kNoErrorStatus;
}

uint8_t power_analog(uint8_t length)
{
	set_analog_power(mib_buffer[0]);
	
	return kNoErrorStatus;
}
