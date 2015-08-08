#ifndef __communication_h__
#define __communication_h__

#include <stdint.h>
#include <xc.h>

void enable_power();
void disable_power();
void init_spi();

//TDC7200 interface
uint8_t 	tdc7200_readfast(uint8_t address);
void 		tdc7200_writefast(uint8_t address, uint8_t value);
void 		tdc7200_send_start(uint8_t value);

//TDC1000 interface
void 		tdc1000_write8(uint8_t address, uint8_t value);
void 		tdc1000_writefast(uint8_t address, uint8_t value);
uint8_t 	tdc1000_read8(uint8_t address);
uint8_t 	tdc1000_readfast(uint8_t address);


#define 	kTDC7200AddressMask 		0b00111111
#define 	kTDC1000AddressMask			0b00111111

#endif
