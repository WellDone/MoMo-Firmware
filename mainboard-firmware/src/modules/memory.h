#include <stdio.h>
#include <stdlib.h>
#include "common.h"

#define MEMORY_SUBSECTION_MASK 0xFFFL
#define MEMORY_SUBSECTION_SIZE 0x1000L
#define MEMORY_SUBSECTION_ADDR(num) (MEMORY_SUBSECTION_SIZE*num)
#define MEMORY_SUBSECTION_NUM(addr) (addr>>3)
#define MEMORY_ADDRESS_MASK 0xFFFFFL

void configure_SPI();

bool mem_write( unsigned long addr, const BYTE* data, unsigned int length );
bool mem_read( unsigned long addr, BYTE* buf, unsigned int numBytes );
void mem_clear_subsection( unsigned int addr );
void mem_clear_all();
BYTE mem_status();
unsigned short mem_capacity();
bool mem_test();
