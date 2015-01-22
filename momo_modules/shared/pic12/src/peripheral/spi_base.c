//spi_base.c

#include "spi_base.h"
#include "bit_utilities.h"

//SSPIE and SSPIF must be defined specifically for each MSSP peripheral, see spi2.c.

#define SSPSTAT 	p_reg(SSP, N, STAT)
#define SSPCON1		p_reg(SSP, N, CON1)
#define SSPCON3		p_reg(SSP, N, CON3)
#define SSPADD		p_reg(SSP, N, ADD)
#define SSPBUF		p_reg(SSP, N, BUF)

#define SSP_WCOL		SSPCON1,7
#define SSP_BF			SSPSTAT,0
#define SSP_OV			SSPSTAT,7
#define SSP_OVERWRITE 	SSPCON3,4
#define SSP_ENABLE		SSPCON1,5

//spi_init
void p_fun(spi, N, configure)(SPIConfig config)
{
	SSPCON1 = config.value & SPICON1MASK;
	SSPSTAT = config.value & SPISTATMASK;

	if (config.buffer_overwrite)
		bit_set(SSP_OVERWRITE);
	else
		bit_clear(SSP_OVERWRITE);
}

//spi_setenabled
void p_fun(spi, N, setstate)(PeripheralState state)
{
	if (state == kDisabled)
	{
		bit_clear(SSP_ENABLE);
		bit_clear(SSPIE);
	}
	else
	{
		bit_clear(SSPIF);
		if (state == kEnabled_Async)
			bit_set(SSPIE);

		SSPBUF;

		bit_set(SSP_ENABLE);
	}
}

//spi_setrate
void p_fun(spi, N, setrate)(uint8 baud)
{
	SSPADD = baud;
}

//spi_transfer
uint8 p_fun(spi, N, transfer)(uint8 val)
{
	bit_clear(SSP_WCOL);
	bit_clear(SSP_OV);

	bit_clear(SSPIF);
	SSPBUF = val;

	//Asynchronous Mode - return immediately
	if (bit(SSPIE))
		return 0;

	while (!bit(SSPIF))
		;

	return SSPBUF;
}