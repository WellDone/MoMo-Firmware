#include "communication.h"
#include <xc.h>
#include "port.h"
#include "spi2.h"
#include "oscillator.h"
#include "tdc7200.h"
#include "tdc1000.h"

void enable_power(void)
{
	LATCH(CLOCKENABLE) = 1;
	LATCH(ENABLE) = 1;

	//Speed up to 32 mhz
	set_32mhz_speed();
	__delay_ms(5);

	tdc7200_push();
	tdc1000_pushfast();
}

void disable_power(void)
{
	LATCH(CLOCKENABLE) = 0;
	LATCH(ENABLE) = 0;

	//Reduce speed back to normal
	set_hf_speed(k4MhzHFOsc);
}

void init_spi(void)
{
	SPIConfig config;

	PIN_DIR(SDO, OUTPUT);
	PIN_DIR(SDI, INPUT);
	PIN_DIR(SCK, OUTPUT);

	LATCH(CS7200) = 1;
	PIN_DIR(CS7200, OUTPUT);

	LATCH(CS1000) = 1;
	PIN_DIR(CS1000, OUTPUT);

	config.mode = kSPIMaster_Osc4;
	config.line_idle = kSPIIdleHigh;
	config.clock_edge = kSPIIdleToActive;
	config.sample_time = kSPISampleEnd;

	spi2_configure(config);
	spi2_setstate(kEnabled_Sync);
}

uint8_t tdc7200_readfast(uint8_t address)
{
	LATCH(CS7200) = 0;

	SSP2IF = 0;
	SSP2BUF = address | 1 << 7; //Autoincrement

	while (!SSP2IF)
		;

	SSP2IF = 0;
	SSP2BUF = 0xff;

	while (!SSP2IF)
		;

	LATCH(CS7200) = 1;
	return SSP2BUF;
}

void tdc7200_writefast(uint8_t address, uint8_t value)
{
	LATCH(CS7200) = 0;

	SSP2IF = 0;
	SSP2BUF = address | 1 << 7 | 1 << 6; //Autoincrement, write not read


	while (!SSP2IF)
		;

	SSP2IF = 0;
	SSP2BUF = value;

	while (!SSP2IF)
		;

	LATCH(CS7200) = 1;
}

void tdc1000_writefast(uint8_t address, uint8_t value)
{

	LATCH(CS1000) = 0;

	SSP2IF = 0;
	SSP2BUF = address | (1 << 6);

	while (!SSP2IF)
		;

	SSP2IF = 0;
	SSP2BUF = value;

	while (!SSP2IF)
		;

	LATCH(CS1000) = 1;
}

uint8_t tdc1000_readfast(uint8_t address)
{
	LATCH(CS1000) = 0;

	SSP2IF = 0;
	SSP2BUF = address;

	while (!SSP2IF)
		;

	SSP2IF = 0;
	SSP2BUF = 0xFF;

	while (!SSP2IF)
		;

	LATCH(CS1000) = 1;

	return SSP2BUF;
}

void tdc7200_send_start(uint8_t value)
{
	LATCH(CS7200) = 0;

	SSP2IF = 0;
	SSP2BUF = (kTDC7200_Config1Reg | (1<<6));
	while (!SSP2IF)
		;

	SSP2IF = 0;
	SSP2BUF = value;
	
	while (!SSP2IF)
		;

	LATCH(CS7200) = 1;
}
