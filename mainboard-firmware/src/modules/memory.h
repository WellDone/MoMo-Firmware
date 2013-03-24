#include <stdio.h>
#include <stdlib.h>
#include "common.h"

bool configure_SPI();

bool mem_write( int addr, const BYTE* data, unsigned int length );
bool mem_read( int addr, BYTE* buf, unsigned int numBytes );
void mem_clear();
BYTE mem_status();
bool mem_test();
