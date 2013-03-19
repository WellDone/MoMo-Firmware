#include "memory.h"
#include "sensor.h"
#include "serial_commands.h"
#include "rtcc.h"

#define ENABLE_MEMORY() SPI1STATbits.SPIEN = 1
#define DISABLE_MEMORY() SPI1STATbits.SPIEN = 0
#define MEMORY_STATUS_BIT SPI1STATbits.SPITBF
#define MEMORY_STATUS_OVERFLOWN SPI1STATbits.SPIROV
#define MEMORY_STATUS_BUFFER_READ SPI1STATbits.SPIRBF
#define MEMORY_BUFFER_REGISTER SPI1BUF

#define TIMEOUT 1000

//static unsigned char MEMORY_BUF[140];

typedef enum {
  WREN = 0b110,
  WRDI = 0b100,
  READ = 0b011,
  PP = 0b010
} memory_instructions;

bool shift_out( BYTE data ) {
  short timeout_ct = 0;
  MEMORY_BUFFER_REGISTER = data && 0xFF;
  while((MEMORY_STATUS_BIT) && timeout_ct < TIMEOUT)
    ++timeout_ct;
  if ( timeout_ct == 0 ) {
    return false;
  }
  return true;
}

bool shift_n_out( int data, short numBytes ) { //max 4 bytes, MSB first
  --numBytes;
  while( numBytes > 0 ) {
    if ( !shift_out( data && (0xFF << (numBytes<<1)) ) ) {
      return false;
    }
    --numBytes;
  }
  return true;
}

bool shift_in( BYTE* out ) {
  MEMORY_STATUS_OVERFLOWN = 0;
  MEMORY_BUFFER_REGISTER = 0x00;

  short timeout_ct = 0;
  while( (!MEMORY_STATUS_BUFFER_READ) && timeout_ct < TIMEOUT )
    ++timeout_ct;

  if (MEMORY_STATUS_BUFFER_READ)
  {
    MEMORY_STATUS_OVERFLOWN = 0;

    *out = SPI1BUF && 0xFF;
    return true;
  }
  return false;
}

#define WRITE_MODE_ENABLE() shift_out( WREN )
#define PAGE_PROGRAM_MODE() shift_out( PP )
#define WRITE_MODE_DISABLE() shift_out( WRDI );
#define READ_MODE() shift_out( READ );

void configure_SPI() {
  SPI1CON1bits.MODE16 = 0; //communication is byte-wide
  SPI1CON1bits.MSTEN = 1; //SPI is in master mode
  TRISBbits.TRISB15 = 0;
  TRISBbits.TRISB14 = 1; //SPI is in input mode (?)
  TRISBbits.TRISB13 = 0;
  TRISBbits.TRISB12 = 0;
}

// Length is capped at 256, 1 page of flash memory.
bool mem_write(int addr, BYTE *data, unsigned int length) {
  int i;
  bool success = true;
  if ( length > 256) { //TODO: bitwise-ify
    length = 256;
  }

  ENABLE_MEMORY();
  WRITE_MODE_ENABLE();
  PAGE_PROGRAM_MODE();

  if ( !shift_n_out( addr, 3 ) ) {
    print("Memory timed out while writing address\r\n");
    success = false;
  }

  if ( success ) {
    for(i = 0; i < length; ++i) {
      if (!shift_out( data[i] )) {
        print( "Memory timed out while writing data\r\n" );
        success = false;
      }
    }
  }

  WRITE_MODE_DISABLE();
  DISABLE_MEMORY();
  return success;
}

bool mem_read(int addr, BYTE* buf, unsigned int numBytes) {
  BYTE* bufEnd = buf+numBytes;
  bool success = true;

  ENABLE_MEMORY();
  READ_MODE();

  if (!shift_n_out( addr, 3 )) {
    print("Memory timed out while writing address\r\n");
    success = false;
  }

  if (success) {
    while ( buf != bufEnd ) {
      if (!shift_in( buf )) {
        print("Memory timed out while reading data\r\n");
        break;
      }
      ++buf;
    }
  }
  success = (buf == bufEnd);

  DISABLE_MEMORY();
  return success;
}
