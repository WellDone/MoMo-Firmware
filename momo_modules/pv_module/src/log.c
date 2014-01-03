#include "log.h"
#include "mib12_api.h"

uint32_t sector_index;
uint16_t sector_off;
uint8	 log_interval;

extern uint8 	sector[512];
extern uint8 	filename[11];
extern unsigned int adc_result;

static uint8 log_nextsector();
static uint8 log_loadsector();
static void log_readtoc();

uint8 log_init()
{
	if (sd_check_inserted() != 1)
	{
		sector_index = 1;
		sector_off = 0;
		log_interval = kDefaultLogInterval;
		return 0;
	}

	//Otherwise, figure out where we are supposed to start writing given the TOC
	log_readtoc();
	if (log_loadsector() == 1)
		return 0;

	return 1;
}

static void log_updatetoc()
{
	uint16_t off = 0;
	uint32_t next_sec = sector_index;

	sector_index = 0;
	log_loadsector();

	sector[off++] = 0x12;
	sector[off++] = 0x34;
	sector[off++] = 0x56;
	sector[off++] = 0x78;
	sector[off++] = next_sec & 0xFF;
	sector[off++] = (next_sec >> 8) & 0xFF;
	sector[off++] = (next_sec >> 16) & 0xFF;
	sector[off++] = (next_sec >> 24) & 0xFF;
	sector[off++] = log_interval;

	sd_power_on();
	f32_sync();
	sd_power_off();

	sector_index = next_sec;
	sector_off = 0;
}

static void log_readtoc()
{
	sector_index = 0;
	log_loadsector();

	//Check if we have a valid TOC sector 
	if ((*(uint32_t*)(sector)) == 0x78563412)
	{
		sector_index = (*(uint32_t*)(sector+4));
		log_interval = (*(uint8*)(sector+8));
		sector_off = 0;
	}
	else
	{
		sector_index = 1;
		sector_off = 0;
		log_interval = kDefaultLogInterval;

		log_updatetoc();
	}
}

uint8 log_logsamples()
{
	sector[sector_off++] = 0xAA;
	sector[sector_off++] = 0xBB;

	sector[sector_off++] = 0xCC;
	sector[sector_off++] = 0xDD;

	sector[sector_off++] = 0xEE;
	sector[sector_off++] = 0xFF;

	sector[sector_off++] = 0x55;
	sector[sector_off++] = 0x99;

	sample_v1();
	sector[sector_off++] = (1 << 7) | (adc_result & 0xFF);
	sector[sector_off++] = (adc_result >> 8) & 0xFF;

	sample_v2();
	sector[sector_off++] = adc_result & 0xFF;
	sector[sector_off++] = (adc_result >> 8) & 0xFF;

	sample_v3();
	sector[sector_off++] = adc_result & 0xFF;
	sector[sector_off++] = (adc_result >> 8) & 0xFF;

	sample_i1();
	sector[sector_off++] = adc_result & 0xFF;
	sector[sector_off++] = (adc_result >> 8) & 0xFF;

	if (sector_off >= 512)
		return log_nextsector();

	return 0;
}

void log_setinterval()
{
	if (log_interval > kLogInterval256s)
		log_interval = kDefaultLogInterval;

	switch (log_interval)
	{
		case kLogInterval1s:
		WDTCON = kWatchdog1s;
		break;

		case kLogInterval2s:
		WDTCON = kWatchdog2s;
		break;

		case kLogInterval4s:
		WDTCON = kWatchdog4s;
		break;
		
		case kLogInterval8s:
		WDTCON = kWatchdog8s;
		break;
		
		case kLogInterval16s:
		WDTCON = kWatchdog16s;
		break;

		case kLogInterval32s:
		WDTCON = kWatchdog32s;
		break;
		
		case kLogInterval64s:
		WDTCON = kWatchdog64s;
		break;

		case kLogInterval128s:
		WDTCON = kWatchdog128s;
		break;
		
		case kLogInterval256s:
		WDTCON = kWatchdog256s;
		break;
	}
}

static uint8 log_loadsector()
{
	if (sd_check_inserted() != 1)
		return 1;

	sd_power_on();
	if (f32_openvol() != kSDNoError)
	{
		sd_power_off();
		return 1;
	}

	filename[0] = 'L';
	filename[1] = 'O';
	filename[2] = 'G';
	filename[3] = ' ';
	filename[4] = ' ';
	filename[5] = ' ';
	filename[6] = ' ';
	filename[7] = ' ';
	filename[8] = 'B';
	filename[9] = 'I';
	filename[10] = 'N';

	if (f32_findfile() != kSDNoError)
	{
		sd_power_off();
		return 1;
	}

	//If we are at the end of the file, maybe try to add another cluster?
	if (f32_seek(sector_index) == kSDEOFError)
	{
		sd_power_off();
		return 1;
	}

	sd_power_off();
	return 0;
}

static uint8 log_nextsector()
{
	uint8 err;
	if (sd_check_inserted() != 1)
	{
		sector_off = 0;
		return 1;
	}

	sd_power_on();
	err = f32_sync();
	sd_power_off();

	if (err != kSDNoError)
		return 1;

	++sector_index;
	sector_off = 0;

	log_updatetoc();

	return log_loadsector();
}

void log_getoffset()
{
	mib_buffer[0] = sector_off & 0xFF;
	mib_buffer[1] = (sector_off >> 8) & 0xFF;

	bus_slave_setreturn(pack_return_status(0, 2));
}

void log_getsector()
{
	mib_buffer[0] = sector_index & 0xFF;
	mib_buffer[1] = (sector_index >> 8) & 0xFF;

	bus_slave_setreturn(pack_return_status(0, 2));
}