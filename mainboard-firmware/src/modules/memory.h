#include <stdio.h>
#include <stdlib.h>
#include "common.h"

#define MEMORY_SUBSECTION_MASK 0xFFF
#define MEMORY_SUBSECTION_SIZE 0x1000
#define MEMORY_ADDRESS_MASK 0xFFFFF

bool configure_SPI();

bool mem_write( unsigned long addr, const BYTE* data, unsigned int length );
bool mem_read( unsigned long addr, BYTE* buf, unsigned int numBytes );
void mem_clear_subsection( unsigned int addr );
void mem_clear_all();
BYTE mem_status();
bool mem_test();
