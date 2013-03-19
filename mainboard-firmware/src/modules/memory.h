#include <stdio.h>
#include <stdlib.h>
#include <p24F16KA101.h>
#include "common.h"

//Write to memory
bool mem_write( int addr, BYTE* data, unsigned int length );
bool mem_read( int addr, BYTE* buf, unsigned int numBytes );
void configure_SPI();
