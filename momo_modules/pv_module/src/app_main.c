//app_main.c

#include "platform.h"
#include "watchdog.h"
#include "pv_defines.h"
#include "port.h"
#include "sample.h"
#include "sdcard.h"
#include "mib12_api.h"
#include "fat32.h"
#include "log.h"
#include "watchdog.h"

extern uint8 filename[11];
extern uint8 sector[512];
extern FAT32VolumeInfo vol;
extern uint8 log_interval;

void task(void)
{
	wdt_disable();

	while(1)
	{
		//Try repeatedly to initialize the SD Card
		while(log_init() == 0)
		{
			WDTCON = k4SecondTimeout;
			wdt_enable();
			asm("sleep");
			wdt_disable();
		}

		//Log until there is an error and then try
		//to reinitialize the SD card. 
		do
		{
			TRISA7 = !TRISA7;
			log_setinterval();
			wdt_enable();
			asm("sleep");
			wdt_disable();
			TRISA7 = !TRISA7;
		} while(log_logsamples() == 0);
	}
}

void interrupt_handler(void)
{

}

void initialize(void)
{
	wdt_disable();

	PIN_DIR(VOLT1, INPUT);
	PIN_TYPE(VOLT1, ANALOG);

	PIN_DIR(VOLT2, INPUT);
	PIN_TYPE(VOLT2, ANALOG);
	
	PIN_DIR(VOLT3, INPUT);
	PIN_TYPE(VOLT3, ANALOG);
	
	PIN_DIR(CURR1, INPUT);
	PIN_TYPE(CURR1, ANALOG);

	sd_pins_idle();
}

void main()
{
}

void power_sdcard()
{
	/*if(mib_buffer[0] == 0)
		sd_power_off();
	else
	{
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

		mib_buffer[1] = 0;

		f32_findfile();
		
		if (f32_seek(51) != kSDNoError)
			mib_buffer[0] = 1;
		else
		{
			uint8 i=0;
			sector[i++] = 0xA0;
			sector[i++] = 0xB0;
			sector[i++] = 0xC0;
			sector[i++] = 0xD0;

			mib_buffer[0] = 2;
			f32_sync();
		}


		bus_slave_setreturn(pack_return_status(0, 2));
		return;
	}*/

	bus_slave_setreturn(pack_return_status(0, 0));
}

void check_sdcard()
{
	mib_buffer[0] = sd_check_inserted();
	mib_buffer[1] = 0;

	bus_slave_setreturn(pack_return_status(0, 2));
}

void open_size()
{
	mib_buffer[0] = log_interval;
	mib_buffer[1] = 0;

	bus_slave_setreturn(pack_return_status(0, 2));
}