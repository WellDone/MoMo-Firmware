
#include <xc.h>
#include "peripheral_def.h"
#include "platform.h"

#define SPICON1MASK		0b00011111
#define SPISTATMASK		0b11000000

//Valid SPI Modes
enum
{
	kSPIMaster_Osc4  = 0b0000,
	kSPIMaster_Osc16 = 0b0001,
	kSPIMaster_Osc64 = 0b0010,
	kSPIMaster_TMR2	 = 0b0011,
	kSPIMaster_BRGEN = 0b1010,

	kSPISlave_SSOn	 = 0b0100,
	kSPISlave_SSOff	 = 0b0101
};

//Valid SPI Line Idle Conditions
//When the line is not being asserted, should it idle high or low
enum
{
	kSPIIdleLow = 0,
	kSPIIdleHigh = 1
};

//Valid SPI Clock Edge Timings
//Defines on what edge of the clock the module outputs data
enum
{
	kSPIIdleToActive = 0,
	kSPIActiveToIdle = 1
};

//Valid SPI Data Sampling Timings
enum
{
	kSPISampleMiddle = 0,
	kSPISampleEnd = 1
};

//Valid SPI Buffer Overwrite Settings
//Only takese effect in SPI slave mode
enum
{
	kSPIOverwriteBuffer = 1,
	kSPINoOverwriteBuffer = 0
};

//SPI Configuration Structure
typedef union
{
	struct 
	{
		uint8	mode: 4;
		uint8 	line_idle : 1;
		uint8	buffer_overwrite: 1;
		uint8	clock_edge : 1;
		uint8	sample_time: 1;
	};

	uint8 value;
} SPIConfig;

//Peripheral Module API
void  p_fun(spi, N, configure)(SPIConfig config);		//spiN_init
void  p_fun(spi, N, setrate)(uint8 rate);				//spiN_setrate
void  p_fun(spi, N, setstate)(PeripheralState state);   //spiN_setenabled
uint8 p_fun(spi, N, transfer)(uint8 val);