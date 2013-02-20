#include <stdio.h>
#include <stdlib.h>
#include <p24F16KA101.h>

//Write to memory
void mem_write(int addr, int val);
void flush_eeprom();
void clear_eeprom();
void clear_flash();
