#include "digital_amp.h"

#define _XTAL_FREQ			4000000

static uint8_t parameters[kNumParameters];

//Internal functions
void damp_send_offset(uint8_t offset_code);
void damp_send_stage1_gain(uint8_t gain_code);
void damp_send_stage2_gain(uint8_t gain_code);
void damp_send_inverted(uint8_t inverted);
void damp_select_input(uint8_t select_current);

void damp_init()
{
	LATCH(AN_PROG) = 0;
	PIN_DIR(AN_PROG, OUTPUT);

	damp_set_parameter(kOffsetParameter, 0);
	damp_set_parameter(kStage1GainParameter, 0);
	damp_set_parameter(kStage2GainParameter, 0);
	damp_set_parameter(kSelectCurrentParameter, 0);
	damp_set_parameter(kInvertInputParamter, 0);
	damp_set_parameter(kStabilizationDelayMS, 10);
}

void damp_enable()
{
	LATCH(AN_POWER) = 1;
	__delay_ms(1);

	damp_send_offset(damp_get_parameter(kOffsetParameter));
	damp_send_stage1_gain(damp_get_parameter(kStage1GainParameter));
	damp_send_stage2_gain(damp_get_parameter(kStage1GainParameter));
	damp_send_inverted(damp_get_parameter(kInvertInputParamter));
	damp_select_input(damp_get_parameter(kSelectCurrentParameter));

	uint8_t i;

	for (i=0; i<damp_get_parameter(kStabilizationDelayMS); ++i)
		__delay_ms(1);
}

void damp_disable()
{
	damp_send_inverted(0);
	damp_select_input(0);
	LATCH(AN_PROG) = 0;
	LATCH(AN_POWER) = 0;
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

void damp_send_offset(uint8_t offset_code)
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

void damp_send_stage1_gain(uint8_t gain_code)
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

void damp_send_stage2_gain(uint8_t gain_code)
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

void damp_send_inverted(uint8_t inverted)
{
	if (PIN(AN_POWER) == 0)
		return;

	if (inverted)
		LATCH(AN_INVERT) = 1;
	else
		LATCH(AN_INVERT) = 0;
}

void damp_select_input(uint8_t current)
{
	if (PIN(AN_POWER) == 0)
		return;

	if (current)
		LATCH(AN_SELECT) = 1;
	else
		LATCH(AN_SELECT) = 0;
}

void damp_set_parameter(AmplifierSetting setting, uint8_t value)
{
	parameters[setting] = value;
}

uint8_t damp_get_parameter(AmplifierSetting setting)
{
	return parameters[setting];
}