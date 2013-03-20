#include "memory.h"
#include "sensor.h"
#include "serial_commands.h"
#include "rtcc.h"
#include "utilities.h"

#define SS_VALUE LATBbits.LATB15
#define ENABLE_MEMORY() SPI1STATbits.SPIEN = 1
#define DISABLE_MEMORY() SPI1STATbits.SPIEN = 0
#define MEMORY_TX_STATUS SPI1STATbits.SPITBF
#define MEMORY_STATUS_OVERFLOWN SPI1STATbits.SPIROV
#define MEMORY_RX_STATUS SPI1STATbits.SPIRBF
#define MEMORY_BUFFER_REGISTER SPI1BUF

#define TIMEOUT 1000

//static unsigned char MEMORY_BUF[140];

typedef enum {
  WREN = 0b110,
  WRDI = 0b100,
  RDSR = 0b101,
  WRSR = 0b001,
  READ = 0b011,
  PP = 0b010,
  BE = 0b11000111
} memory_instructions;

void configure_SPI() {
  SPI1CON1bits.MODE16 = 0; //communication is byte-wide
  SPI1CON1bits.MSTEN = 1; //SPI is in master mode
  SPI1STATbits.SPIEN = 1;

  TRISBbits.TRISB15 = 0; // SS
  TRISBbits.TRISB14 = 1; // SDI
  AD1PCFGbits.PCFG14 = 1;
  TRISBbits.TRISB13 = 0; // SDO
  TRISBbits.TRISB12 = 0; // SDCK

  SS_VALUE = 0x1;
}

bool shift_out( BYTE data ) {
  BYTE out;
  char res[2];
  res[0] = data;
  if (!data)
    res[0] = 0x7E;
  res[1] = 0x0;
  print("write ");
  print( res );
  print("\r\n");
  unsigned short count = 0;

  while (MEMORY_TX_STATUS && count<TIMEOUT)
    ++count;
  MEMORY_BUFFER_REGISTER = data;

  while (!MEMORY_RX_STATUS && count<TIMEOUT)
    ++count;
  sendf( U2, "%d\r\n", count );
  if (count==TIMEOUT)
    return false;

  out = MEMORY_BUFFER_REGISTER;
  return true;
}

bool shift_n_out( int data, short numBytes ) { //max 4 bytes, MSB first
  while( numBytes > 0 ) {
    if ( !shift_out( data & 0xFF << 3*numBytes ) ) {
      return false;
    }
    --numBytes;
    //data = data ;
  }
  return true;
}

bool shift_in( BYTE* out ) {
  MEMORY_STATUS_OVERFLOWN = 0;
  MEMORY_BUFFER_REGISTER = 0x3A;

  unsigned short count = 0;
  while( MEMORY_RX_STATUS ==0 && count<TIMEOUT )
    ++count;

  if (count!=TIMEOUT && MEMORY_RX_STATUS)
  {
    MEMORY_STATUS_OVERFLOWN = 0;
    *out = SPI1BUF & 0xFF;
    return true;
  }
  return false;
}

#define WRITE_MODE_ENABLE() shift_out( WREN )
#define PAGE_PROGRAM_MODE() shift_out( PP )
#define WRITE_MODE_DISABLE() shift_out( WRDI );
#define READ_MODE() shift_out( READ );

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
        success = false;
        break;
      }
      ++buf;
    }
  }
  success = (buf == bufEnd);

  DISABLE_MEMORY();
  return success;
}

void mem_read_all( int addr ) {
  ENABLE_MEMORY();
  READ_MODE();

  if (!shift_n_out( addr, 3 )) {
    print("Memory timed out while writing address\r\n");
  }

  char buf = 0x0;
  while ( 1 ) {
    if (!shift_in( &buf )) {
      print("Memory timed out while reading data\r\n");
      break;
    }
    put( U2, buf );
  }

  DISABLE_MEMORY();
}

BYTE mem_status() {
  shift_out( RDSR );
  BYTE status = 0xFF;
  shift_in( &status );
  return status;
}

void mem_clear() {
  ENABLE_MEMORY();

  WRITE_MODE_ENABLE();
  shift_out( BE );

  DISABLE_MEMORY();
  ENABLE_MEMORY();

  BYTE status = mem_status();
  print_byte( status );
  while (status & 0b1) {
    shift_in( &status );
    print_byte( status );
  }
}
