#include "log.h"
#include "mib12_api.h"

uint32_t sector_index;
uint16_t sector_off;

extern uint8 	sector[512];
extern uint8 	filename[11];
extern unsigned int adc_result;

static void log_nextsector();
static void log_loadsector();

void log_init()
{
	sector_index = 0;
	sector_off = 0;

	log_loadsector();
}

void log_logsamples()
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
		log_nextsector();
}

static void log_loadsector()
{
	if (sd_check_inserted() != 1)
		return;

	sd_power_on();
	f32_openvol();

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

	f32_findfile();
	f32_seek(sector_index);

	sd_power_off();
}

static void log_nextsector()
{
	if (sd_check_inserted() != 1)
	{
		sector_off = 0;
		return;
	}

	sd_power_on();
	f32_sync();
	sd_power_off();

	++sector_index;
	sector_off = 0;

	log_loadsector();

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