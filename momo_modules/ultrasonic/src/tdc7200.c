#include "tdc7200.h"
#include "spi2.h"
#include <xc.h>
#include "port.h"

static void 	tdc7200_calc_lsb();

tdc7200_config tdc7200_registers;

static uint32_t calibration;

void tdc7200_init()
{
	tdc7200_registers.config1.value = 0x00;
	tdc7200_registers.config2.value = 0x40;
	tdc7200_registers.intmask.value = 0x07;
	tdc7200_registers.stop_mask = 0;
}

void tdc7200_setstopmask(uint16_t mask)
{
	tdc7200_registers.stop_mask = mask;
}

void tdc7200_setaverages(uint8_t averages)
{
	tdc7200_registers.config2.avg_cycles = averages;
}

uint8_t tdc7200_push()
{
	tdc7200_writefast(kTDC7200_Config2Reg, tdc7200_registers.config2.value);
	tdc7200_writefast(kTDC7200_INTMaskReg, tdc7200_registers.intmask.value);
	
	//Clear the interrupt flags
	tdc7200_writefast(kTDC7200_INTStatusReg, 0b11111);
	
	tdc7200_writefast(kTDC7200_ClockStopHigh, tdc7200_registers.stop_mask >> 8);
	tdc7200_writefast(kTDC7200_ClockStopLow, tdc7200_registers.stop_mask & 0xFF);

	//Don't push config1 since that can start the measurement
	return 0;
}	

void tdc7200_trigger()
{
	tdc7200_registers.config1.meas_mode = 0b01;
	tdc7200_registers.config1.start_meas = 1;
	tdc7200_send_start(tdc7200_registers.config1.value);
}

void tdc7200_setstops(uint8_t stops)
{
	tdc7200_registers.config2.num_stops = stops;
}

void tdc7200_readresults()
{
	uint8_t i=0;
	LATCH(CS7200) = 0;

	spi2_transfer((1 << 7) | kTDC7200_Time1);

	for (i=0; i<39; ++i)
		tdc7200_registers.results.bytes[i] = spi2_transfer(0xFF);

	LATCH(CS7200) = 1;

	tdc7200_registers.clocks[0] = (((int32_t)tdc7200_registers.results.tofs[0].clock.msb) << 16) | (((int32_t)tdc7200_registers.results.tofs[0].clock.nsb) << 8) | ((int32_t)tdc7200_registers.results.tofs[0].clock.lsb);
	tdc7200_registers.clocks[1] = (((int32_t)tdc7200_registers.results.tofs[1].clock.msb) << 16) | (((int32_t)tdc7200_registers.results.tofs[1].clock.nsb) << 8) | ((int32_t)tdc7200_registers.results.tofs[1].clock.lsb);
	tdc7200_registers.clocks[2] = (((int32_t)tdc7200_registers.results.tofs[2].clock.msb) << 16) | (((int32_t)tdc7200_registers.results.tofs[2].clock.nsb) << 8) | ((int32_t)tdc7200_registers.results.tofs[2].clock.lsb);
	tdc7200_registers.clocks[3] = (((int32_t)tdc7200_registers.results.tofs[3].clock.msb) << 16) | (((int32_t)tdc7200_registers.results.tofs[3].clock.nsb) << 8) | ((int32_t)tdc7200_registers.results.tofs[3].clock.lsb);
	tdc7200_registers.clocks[4] = (((int32_t)tdc7200_registers.results.tofs[4].clock.msb) << 16) | (((int32_t)tdc7200_registers.results.tofs[4].clock.nsb) << 8) | ((int32_t)tdc7200_registers.results.tofs[4].clock.lsb);

	tdc7200_registers.times[0] = (((int32_t)tdc7200_registers.results.tofs[0].time.msb) << 16) | (((int32_t)tdc7200_registers.results.tofs[0].time.nsb) << 8) | ((int32_t)tdc7200_registers.results.tofs[0].time.lsb);
	tdc7200_registers.times[1] = (((int32_t)tdc7200_registers.results.tofs[1].time.msb) << 16) | (((int32_t)tdc7200_registers.results.tofs[1].time.nsb) << 8) | ((int32_t)tdc7200_registers.results.tofs[1].time.lsb);
	tdc7200_registers.times[2] = (((int32_t)tdc7200_registers.results.tofs[2].time.msb) << 16) | (((int32_t)tdc7200_registers.results.tofs[2].time.nsb) << 8) | ((int32_t)tdc7200_registers.results.tofs[2].time.lsb);
	tdc7200_registers.times[3] = (((int32_t)tdc7200_registers.results.tofs[3].time.msb) << 16) | (((int32_t)tdc7200_registers.results.tofs[3].time.nsb) << 8) | ((int32_t)tdc7200_registers.results.tofs[3].time.lsb);
	tdc7200_registers.times[4] = (((int32_t)tdc7200_registers.results.tofs[4].time.msb) << 16) | (((int32_t)tdc7200_registers.results.tofs[4].time.nsb) << 8) | ((int32_t)tdc7200_registers.results.tofs[4].time.lsb);
	tdc7200_registers.times[5] = (((int32_t)tdc7200_registers.results.time6.msb) << 16) | (((int32_t)tdc7200_registers.results.time6.nsb) << 8) | ((int32_t)tdc7200_registers.results.time6.lsb);

	if (1)
		tdc7200_calc_lsb();
	else
		tdc7200_registers.lsb = 53;
}

static void tdc7200_calc_lsb()
{
	int32_t calibration1;
	int32_t calibration2;

	calibration1 = (((int32_t)tdc7200_registers.results.calibration1.msb) << 16) | (((int32_t)tdc7200_registers.results.calibration1.nsb) << 8) | ((int32_t)tdc7200_registers.results.calibration1.lsb);
	calibration2 = (((int32_t)tdc7200_registers.results.calibrationN.msb) << 16) | (((int32_t)tdc7200_registers.results.calibrationN.nsb) << 8) | ((int32_t)tdc7200_registers.results.calibrationN.lsb);

	calibration2 -= calibration1;
	//calibration2 /= 9;	//FIXME: Assuming constant 10 period calibration

	tdc7200_registers.lsb = (125000LL*9LL) / calibration2;
}

//Return the tof in units of 16 picoseconds
int32_t tdc7200_tof(uint8_t index, uint8_t average_bits)
{
	return (125000LL*(tdc7200_registers.clocks[index] >> average_bits) + (tdc7200_registers.times[0] - tdc7200_registers.times[index+1])*tdc7200_registers.lsb) >> 4; //in units of 16 ps 
}