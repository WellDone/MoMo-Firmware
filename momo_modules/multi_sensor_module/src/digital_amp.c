#include "digital_amp.h"

#define _XTAL_FREQ			4000000

void damp_init()
{
	LATCH(AN_PROG) = 0;
	PIN_DIR(AN_PROG, OUTPUT);
}

static void damp_send_zero()
{
	LATCH(AN_PROG) = 1;
	LATCH(AN_PROG) = 0;
	__delay_us(12);
}

static void damp_send_one()
{
	LATCH(AN_PROG)  = 1;
	__delay_us(80);
	LATCH(AN_PROG) = 0;
	__delay_us(12);
}

static void damp_send_start()
{
	damp_send_one();

	damp_send_zero();
	damp_send_zero();
	damp_send_zero();
	damp_send_zero();
	damp_send_zero();
	damp_send_zero();
	damp_send_zero();
	damp_send_zero();
	damp_send_zero();
	damp_send_zero();

	damp_send_one();
}

static void damp_send_end()
{
	damp_send_zero();

	damp_send_one();
	damp_send_one();
	damp_send_one();
	damp_send_one();
	damp_send_one();
	damp_send_one();
	damp_send_one();
	damp_send_one();
	damp_send_one();
	damp_send_one();

	damp_send_zero();
}

static void damp_send(uint8_t bitval)
{
	if (bitval)
		damp_send_one();
	else
		damp_send_zero();
}


static void damp_send_dummy()
{
	damp_send_one();
	damp_send_zero();
}

static void damp_send_2bits(uint8_t bits)
{
	damp_send(bits & 0b10);
	damp_send(bits & 0b01);
}


static void damp_send_8bits(uint8_t bits)
{
	damp_send(bits & (1<<7));
	damp_send(bits & (1<<6));
	damp_send(bits & (1<<5));
	damp_send(bits & (1<<4));
	damp_send(bits & (1<<3));
	damp_send(bits & (1<<2));
	damp_send(bits & (1<<1));
	damp_send(bits & (1<<0));
}

static void damp_send_function(AmplifierFunction func)
{
	damp_send_2bits(func);
}

void damp_set_offset(uint8_t offset_code)
{
	if (PIN(AN_POWER) == 0)
		return;

	//Form and send 38 bit frame telling the digital amplifier
	//to set its output offset
	damp_send_start();
	damp_send_function(kSimulateParameterValue);
	damp_send_2bits(kOutputOffset);
	damp_send_dummy();
	damp_send_8bits(offset_code);
	damp_send_end();
}

void damp_set_stage1_gain(uint8_t gain_code)
{
	if (PIN(AN_POWER) == 0)
		return;

	gain_code &= 0b01111111;

	damp_send_start();
	damp_send_function(kSimulateParameterValue);
	damp_send_2bits(kFirstStageGain);
	damp_send_dummy();
	damp_send_8bits(gain_code);
	damp_send_end();
}

void damp_set_stage2_gain(uint8_t gain_code)
{
	if (PIN(AN_POWER) == 0)
		return;
	
	gain_code &= 0b111;
	
	damp_send_start();
	damp_send_function(kSimulateParameterValue);
	damp_send_2bits(kSecondStageGain);
	damp_send_dummy();
	damp_send_8bits(gain_code);
	damp_send_end();
}