#include "memory.h"

// Flush eeprom to Flash memory when done
void flush_eeprom() {
  //Read byte
}

// Reset EEPROM to 0's
void clear_eeprom() {

}
//Clear flash memory after transmission
void clear_flash() {

}

//Write a value to EEPROM
void mem_write(int addr, int val) {
  //Write to
  //Send WREN

  //SEND val
  SPI1BUF = val; //put value to write in SPI1BUF
  while(!SPI1STATbits.SPIRBF); //wait while busy
 // return SPI1BUF;
}
