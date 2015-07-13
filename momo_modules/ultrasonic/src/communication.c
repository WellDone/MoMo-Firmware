#include "communication.h"
#include <xc.h>
#include "port.h"
#include "spi2.h"

#define _XTAL_FREQ			32000000

static uint8_t tdc7200_transfer8(uint8_t cmd, uint8_t value);
static uint8_t tdc1000_transfer8(uint8_t cmd, uint8_t value);

void enable_power(void)
{
	LATCH(CLOCKENABLE) = 1;
	LATCH(ENABLE) = 1;

	//Turn up oscillator clock for faster computation
	OSCCON = (1 << 7) | ((0b1110) << 3);
	while(!PLLR)
		;

	__delay_ms(10);
}

void disable_power(void)
{
	LATCH(CLOCKENABLE) = 0;
	LATCH(ENABLE) = 0;

	OSCCON = (0 << 7) | ((0b1101) << 3);
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

void tdc7200_write8(uint8_t address, uint8_t value)
{
	address &= kTDC7200AddressMask;
	address |= 1 << 7; //Autoincrement
	address |= 1 << 6; //Write, not read

	tdc7200_transfer8(address, value);
}

uint8_t tdc7200_read8(uint8_t address)
{
	address &= kTDC7200AddressMask;
	address |= 1 << 7; //Autoincrement

	return tdc7200_transfer8(address, 0xFF);
}

uint32_t tdc7200_read24(uint8_t address)
{
	uint8_t msb, nsb, lsb;

	address &= kTDC7200AddressMask;
	address |= 1 << 7; //Autoincrement

	LATCH(CS7200) = 0;
	__delay_us(20);

	spi2_transfer(address);
	msb = spi2_transfer(0xFF);
	nsb = spi2_transfer(0xFF);
	lsb = spi2_transfer(0xFF);

	LATCH(CS7200) = 1;

	return (((uint32_t)msb) << 16) | (((uint32_t)nsb) << 8) | lsb;
}

void tdc1000_write8(uint8_t address, uint8_t value)
{
	address &= kTDC1000AddressMask;
	address |= 1 << 6;

	tdc1000_transfer8(address, value);
}

uint8_t tdc1000_read8(uint8_t address)
{
	address &= kTDC1000AddressMask;
	return tdc1000_transfer8(address, 0xFF);
}

static uint8_t transfer8(uint8_t cmd, uint8_t value)
{
	spi2_transfer(cmd);
	return spi2_transfer(value);
}

static uint8_t tdc7200_transfer8(uint8_t cmd, uint8_t value)
{
	uint8_t read_value;

	LATCH(CS7200) = 0;
	__delay_us(20);

	read_value = transfer8(cmd, value);

	LATCH(CS7200) = 1;

	return read_value;
}

static uint8_t tdc1000_transfer8(uint8_t cmd, uint8_t value)
{
	uint8_t read_value;

	LATCH(CS1000) = 0;
	__delay_us(20);

	read_value = transfer8(cmd, value);

	LATCH(CS1000) = 1;

	return read_value;
}