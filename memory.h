#include <stdio.h>
#include <stdlib.h>
#include <p24F16KA101.h>


//Write to memory
void mem_write(int addr, int val);
long mem_read(int addr);
void configure_SPI();
static unsigned char TX_BUF[140];
static unsigned long next_free;
static unsigned long next_read;
