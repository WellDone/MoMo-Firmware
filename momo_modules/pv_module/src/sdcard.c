/*
 * sdcard.c 
 * A module for communicating with 
 *
 */
#include "sdcard.h"
#include "port.h"
#include "wpu.h"
#include "pv_defines.h"
#include "spi2.h"

#define _XTAL_FREQ			4000000

uint8 cmd_packet[6];
uint8 sector[512];

typedef union
{
	SDResponse parsed;
	uint8 	   data[5];
} SDResponseUnion;

SDResponseUnion resp;

//Internal Functions
static void 		sd_initcmd(uint8 cmd);
static uint8 		sd_sendpacket(SDResponseType rtype);
static uint8 		sd_sendcommand(SDResponseType rtype);
static SDErrorCode 	sd_bulktransfer(SDTransferOperation op);

void sd_pins_idle()
{
	enable_weak_pullups();

	PIN_TYPE(SDO, DIGITAL);
	PIN_TYPE(SDI, DIGITAL);
	PIN_TYPE(SDSEL, DIGITAL);

	PIN_SET(SDO, 0);
	PIN_SET(SDI, 0);
	PIN_SET(SDSEL, 0);
	PIN_SET(SDDET, 0);
	PIN_SET(SDCLK, 0);

	PIN_DIR(SDO, OUTPUT);
	PIN_DIR(SDI, OUTPUT);
	PIN_DIR(SDSEL, OUTPUT);
	PIN_DIR(SDDET, OUTPUT);
	PIN_DIR(SDCLK, OUTPUT);
	PIN_DIR(SDPOW, INPUT);
}

uint8 sd_power_on()
{
	//SDO should drive idle high in case a card is connected
	enable_pullup(SDI);
	PIN_SET(SDSEL, 1);
	PIN_SET(SDO, 1);
	PIN_SET(SDCLK, 1);

	PIN_SET(SDPOW, 1);

	PIN_DIR(SDI, INPUT);
	
	PIN_DIR(SDPOW, OUTPUT);

	__delay_ms(100);

	sd_initialize();

	return 0;
}

void sd_power_off()
{
	PIN_SET(SDO, 0);
	PIN_SET(SDSEL, 0);
	PIN_SET(SDCLK, 0);

	PIN_DIR(SDPOW, INPUT);
	
	disable_pullup(SDI);
	PIN_DIR(SDI, OUTPUT);
	PIN_SET(SDI, 0);

	spi2_setstate(kDisabled);
}

uint8 sd_check_inserted()
{
	PIN_DIR(SDDET, INPUT);
	enable_pullup(SDDET);

	if (PIN(SDDET) == 1)
	{
		disable_pullup(SDDET);
		PIN_DIR(SDDET, OUTPUT);
		return 0;
	}

	disable_pullup(SDDET);
	PIN_DIR(SDDET, OUTPUT);
	return 1;
}

/* Initialization Sequence is per the following documents:
 * http://mfb.nopdesign.sk/datasheet/cat_d/MMC/spitiming.pdf
 * https://www.sdcard.org/downloads/pls/simplified_specs/part1_410.pdf
 */
SDErrorCode sd_initialize()
{
	SPIConfig config;
	uint8 i;

	config.mode = kSPIMaster_Osc16;
	config.line_idle = kSPIIdleHigh;
	config.buffer_overwrite = kSPIOverwriteBuffer;
	config.clock_edge = kSPIIdleToActive;
	config.sample_time = kSPISampleEnd;

	spi2_configure(config);
	spi2_setstate(kEnabled_Sync);

	PIN_SET(SDSEL, 1);

	//Send at least 74 clocks per the SD card spec
	for (i=0; i<10; ++i)
		spi2_transfer(0xFF);

	//Send CMD0 with proper CRC code to switch to SPI Mode and reset the card
	sd_initcmd(0);
	cmd_packet[5] = 0x95;
	if (sd_sendcommand(kSDTypeR1) != 0x01)
		return kSDUnknownError; //Correct response should have only idle bit set


	//Increase to maximum speed now that the card is reset successfully
	config.mode = kSPIMaster_Osc4;
	spi2_configure(config);
	spi2_setstate(kEnabled_Sync);

	//Send CMD8 to initialize card and set the voltage range
	sd_initcmd(8);
	cmd_packet[3] = 0x01;
	cmd_packet[4] = 0xAA;
	cmd_packet[5] = 0x87;
	if (sd_sendcommand(kSDTypeR3) != 0x1)
		return kSDUnknownError;

	//Make sure the card accepts our voltage range and echos the test pattern
	if (resp.data[3] != 0x01 || resp.data[4] != 0xAA)
		return kSDUnknownError;

	//Now poll telling it we support high capacity cards (SD v2.0)
	//and wait until it finished initializing and leaves idle mode
	//Send Cmd55 + ACMD 41 repeatedly to poll for initialized status
	do
	{
		sd_initcmd(55);
		sd_sendcommand(kSDTypeR1);

		sd_initcmd(41);
		cmd_packet[1] = 1 << 6;			//set HCS, high capacity support
		
	} while(sd_sendcommand(kSDTypeR1) == 0x01);

	if (resp.data[0] != 0x00)
		return kSDUnknownError;

	//Check to make sure this is an SDHC card.
	sd_initcmd(58);
	sd_sendcommand(kSDTypeR3);
	if ((resp.data[1] & 0b01000000) == 0)
		return kSDUnknownError;

	return kSDNoError;
}

/*
 * Internal function to send an SD command packet and read the response
 * without controlling the chip select pin so that it can be used with 
 * normal commands and bulk read and write commands.
 */
static uint8 sd_sendpacket(SDResponseType rtype)
{
	uint8 i;
	uint8 tmp;

	for (i=0; i<6; ++i)
		spi2_transfer(cmd_packet[i]);

	i = 0;

	//Wait until the response appears or we timeout
	//and then clock in the appropriate number of bytes
	do
	{
		++i;
		if (i>=SD_TIMEOUT)
			return kSDTimeoutError;

		tmp = spi2_transfer(0xFF);
	} while (((tmp & 0x80) != 0));

	//Read in the rest of the response
	for(i=0; i<(rtype-1); ++i)
	{
		resp.data[i] = tmp;
		tmp = spi2_transfer(0xFF);
	}
	resp.data[rtype-1] = tmp;

	return resp.data[0];
}

/*
 * Send an SD command over SPI.  Assert CS, send the command, read the response
 * and then desassert CS.  Also send 1 byte
 */
static uint8 sd_sendcommand(SDResponseType rtype)
{
	SDErrorCode err;

	PIN_SET(SDSEL,0);
	err = sd_sendpacket(rtype);
	PIN_SET(SDSEL,1);

	spi2_transfer(0xFF);

	return err;
}

static void sd_initcmd(uint8 cmd)
{
	cmd_packet[0] = 0b01000000 | (cmd & 0b00111111);
	cmd_packet[1] = 0x00;
	cmd_packet[2] = 0x00;
	cmd_packet[3] = 0x00;
	cmd_packet[4] = 0x00;
	cmd_packet[5] = 0x01;
}

SDErrorCode sd_write(uint32_t blocknum)
{
	sd_initcmd(24);
	cmd_packet[4] = blocknum & 0xFF;
	cmd_packet[3] = (blocknum >> 8) & 0xFF;
	cmd_packet[2] = (blocknum >> 16) & 0xFF;
	cmd_packet[1] = (blocknum >> 24) & 0xFF;

	return sd_bulktransfer(kSDWriteOperation);
}

SDErrorCode sd_read(uint32_t blocknum)
{
	sd_initcmd(17);
	cmd_packet[4] = blocknum & 0xFF;
	cmd_packet[3] = (blocknum >> 8) & 0xFF;
	cmd_packet[2] = (blocknum >> 16) & 0xFF;
	cmd_packet[1] = (blocknum >> 24) & 0xFF;

	return sd_bulktransfer(kSDReadOperation);
}

static SDErrorCode sd_bulktransfer(SDTransferOperation op)
{
	uint16 i;

	PIN_SET(SDSEL, 0);

	i = sd_sendpacket(kSDTypeR1);

	//SD Card responds with 0x00 if command is accepted
	if (i != 0)
	{
		PIN_SET(SDSEL, 1);
		spi2_transfer(0xFF);
		return kSDUnknownError;
	}

	spi2_transfer(0xFF);			//Need 1 byte of space between command response and start token

	if (op == kSDWriteOperation)
		spi2_transfer(0xFE); 		//Send the start token
	else
	{
		//Wait until the card sends us a data start token
		while(spi2_transfer(0xFF) != 0xFE)
			;		
	}

	for (i=0; i<512; ++i)
	{
		if (op == kSDWriteOperation)
			spi2_transfer(sector[i]);
		else
			sector[i] = spi2_transfer(0xFF);
	}

	//Transfer fake CRC if writing, read CRC if reading
	spi2_transfer(0x00);
	spi2_transfer(0x00);

	if (op == kSDWriteOperation)
	{
		//Transfer the response
		resp.data[0] = spi2_transfer(0xFF);
		
		//Wait until the write operation finishes
		while(spi2_transfer(0xFF) != 0xFF)
			;
	}

	PIN_SET(SDSEL, 1);

	spi2_transfer(0xFF);

	return resp.data[0]; 
}