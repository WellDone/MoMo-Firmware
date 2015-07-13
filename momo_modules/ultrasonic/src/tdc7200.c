#include "tdc7200.h"

tdc7200_config tdc7200_registers;

static uint8_t tdc7200_push();

void 	tdc7200_init()
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

static uint8_t tdc7200_push()
{
	tdc7200_write8(kTDC7200_Config2Reg, tdc7200_registers.config2.value);
	if (tdc7200_read8(kTDC7200_Config2Reg) != tdc7200_registers.config2.value)
		return 13;

	tdc7200_write8(kTDC7200_INTMaskReg, tdc7200_registers.intmask.value);
	if (tdc7200_read8(kTDC7200_INTMaskReg) != tdc7200_registers.intmask.value)
		return 14;


	//Clear the interrupt flags
	tdc7200_write8(kTDC7200_INTStatusReg, 0b11111);
	if (tdc7200_read8(kTDC7200_INTStatusReg) != 0)
		return 15;

	tdc7200_write8(kTDC7200_ClockStopHigh, tdc7200_registers.stop_mask >> 8);
	if (tdc7200_read8(kTDC7200_ClockStopHigh) != (tdc7200_registers.stop_mask >> 8))
		return 16;

	tdc7200_write8(kTDC7200_ClockStopLow, tdc7200_registers.stop_mask & 0xFF);
	if (tdc7200_read8(kTDC7200_ClockStopLow) != (tdc7200_registers.stop_mask & 0xFF))
		return 17;

	//Push this last since it can start a measurment
	//Don't read it back since we don't want to have a CS edge in the middle of the TOF measurement
	//tdc7200_write8(kTDC7200_Config1Reg, tdc7200_registers.config1.value);
	//if ((tdc7200_read8(kTDC7200_Config1Reg) & 0b11111110) != (tdc7200_registers.config1.value & 0b11111110))
	//	return 18;	

	return 0;
}

uint8_t tdc7200_start()
{
	uint8_t err;

	tdc7200_registers.config1.meas_mode = 0b01;
	tdc7200_registers.config1.start_meas = 1;
	err = tdc7200_push();
	if (err != 0)
		return err;

	tdc7200_send_start(tdc7200_registers.config1.value);
	return 0;
}

void tdc7200_trigger()
{
	tdc7200_send_start(tdc7200_registers.config1.value);
}

void tdc7200_setstops(uint8_t stops)
{
	tdc7200_registers.config2.num_stops = stops;
}

uint32_t tdc7200_calibration()
{
	uint32_t calibration1;
	uint32_t calibration2;
	uint32_t lsb;

	calibration1 = tdc7200_read24(kTDC7200_Calibration1);
	calibration2 = tdc7200_read24(kTDC7200_Calibration2);

	calibration2 -= calibration1;
	calibration2 /= 9;	//FIXME: Assume constant 10 period calibration

	lsb = 125000ULL / calibration2;
	return lsb; //lsb is the time in picoseconds of each ring oscillator cycle
}

int32_t tdc7200_tof(uint8_t index, uint8_t average_cycles)
{
	uint32_t lsb = (int32_t)tdc7200_calibration();
	uint32_t time1 = (int32_t)tdc7200_read24(kTDC7200_Time1);
	uint32_t timeN = (int32_t)tdc7200_read24(kTDC7200_Time1 + 2 + (index << 1));
	uint32_t clockN = (int32_t)tdc7200_read24(kTDC7200_Clock1 + (index << 1));

	return 125000ULL*(clockN >> average_cycles) + (time1 - timeN)*lsb;
}