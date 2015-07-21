#include "tdc1000.h"

tdc1000_config	registers;

void tdc1000_init()
{
	registers.config0.value = 0x45;
	registers.config1.value = 0x40;
	registers.config2.value = 0x00;
	registers.config3.value = 0x03;
	registers.config4.value = 0x1F;

	registers.tof0.value = 0x00;
	registers.tof1.value = 0x00;

	registers.error.value = 0x00;
	registers.timeout.value = 0x19;
	registers.timeout.timeout_ctrl = 3; //FIXME: Hardcoded 128 us timeout for TOF measurement

	registers.clockrate.value = 0x00;

	//FIXME: this is only for testing purposes
	registers.timeout.force_short_tof = 0;
}

void tdc1000_prepare_deltatof(uint8_t channel_averages)
{
	registers.config1.num_avg = channel_averages;
	registers.config2.ch_swp = 0;
	registers.config2.ext_chsel = 1;
	registers.config2.chsel = 0;
	registers.config2.tof_mode = kTDC1000_MeterMode;
}

void tdc1000_setstarttime(uint16_t time)
{
	registers.tof1.timing_msb = (time >> 8) & 0b11;
	registers.tof0.timing_lsb = time & 0xFF;
}

void tdc1000_setchannel(uint8_t channel)
{
	registers.config2.ch_swp = channel & 1;
}

void tdc1000_setexternal(uint8_t ext)
{
	registers.config2.ext_chsel = ext;
}

void tdc1000_reset()
{
	tdc1000_writefast(kTDC1000_ErrorFlagsReg, 0b11);
}

void tdc1000_pushfast()
{
	tdc1000_writefast(kTDC1000_Config0Reg, registers.config0.value);
	tdc1000_writefast(kTDC1000_Config1Reg, registers.config1.value);
	tdc1000_writefast(kTDC1000_Config2Reg, registers.config2.value);
	tdc1000_writefast(kTDC1000_Config3Reg, registers.config3.value);
	tdc1000_writefast(kTDC1000_Config4Reg, registers.config4.value);
	tdc1000_writefast(kTDC1000_TOF_0Reg, registers.tof0.value);
	tdc1000_writefast(kTDC1000_TOF_1Reg, registers.tof1.value);
	tdc1000_writefast(kTDC1000_TimeoutReg, registers.timeout.value);
	tdc1000_writefast(kTDC1000_ClockRateReg, registers.clockrate.value);
	tdc1000_writefast(kTDC1000_ErrorFlagsReg, 0b11);
}

void tdc1000_setgain(PGAGainLevel pga, LNAState lna, EchoThreshold threshold)
{
	registers.tof1.pga_gain = pga;
	registers.tof1.pga_ctrl = 0;

	registers.tof1.lna_ctrl = lna;

	registers.config3.echo_threshold = threshold;
}

void tdc1000_setmode(MeasurementMode mode)
{
	registers.config2.tof_mode = mode;
}

void tdc1000_setexcitation(uint8_t num_pulses, uint8_t num_expected)
{
	//There can be a maximum of 7 expected pulses on the TDC1000
	if (num_expected > 7)
		num_expected = 7;

	registers.config1.num_rx = num_expected;
	registers.config0.num_tx = num_pulses;
}
