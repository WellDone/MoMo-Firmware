#include <stdio.h>
#include <stdlib.h>
#include <p24F16KA101.h>
#include "common.h"

//Write to memory
bool mem_write( int addr, BYTE* data, unsigned int length );
bool mem_read( int addr, BYTE* buf, unsigned int numBytes );
void mem_readall( int addr );
void mem_clear();
void configure_SPI();
