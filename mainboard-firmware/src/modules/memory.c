#include "memory.h"
#include "rtcc.h"
#include "utilities.h"
#include <spi.h>

#define SS_VALUE LATBbits.LATB15
#define ENABLE_MEMORY() SS_VALUE = 0 //SPI1STATbits.SPIEN = 1
#define DISABLE_MEMORY() SS_VALUE = 1 //SPI1STATbits.SPIEN = 0
#define MEMORY_TX_STATUS SPI1STATbits.SPITBF
#define MEMORY_STATUS_OVERFLOWN SPI1STATbits.SPIROV
#define MEMORY_RX_STATUS SPI1STATbits.SPIRBF
#define MEMORY_BUFFER_REGISTER SPI1BUF

#define TIMEOUT 10000

typedef enum {
  WREN = 0b110,
  WRDI = 0b100,
  RDSR = 0b101,
  WRSR = 0b001,
  READ = 0b011,
  PP = 0b010,
  BE = 0b11000111
} memory_instructions;

#define WRITE_MODE_ENABLE() shift_out( WREN )
#define WRITE_MODE_DISABLE() shift_out( WRDI );
#define READ_STATUS_REGISTER() shift_out( RDSR );
#define PAGE_PROGRAM_MODE() shift_out( PP )
#define READ_MODE() shift_out( READ );
#define ERASE_ALL() shift_out( BE );

void configure_SPI() {
 /*
  unsigned int config1 = SPI_MODE8_ON|SPI_CKE_ON|MASTER_ENABLE_ON|SEC_PRESCAL_4_1|PRI_PRESCAL_4_1;
  unsigned int config2 = FRAME_ENABLE_OFF;
  unsigned int config3 = SPI_ENABLE;

  OpenSPI1(config1, config2, config3);
  */

  SPI1CON1bits.MODE16 = 0; //communication is byte-wide
  SPI1CON1bits.MSTEN = 1; //SPI is in master mode
  //SPI1CON1bits.PPRE = 0x0; //TODO: Choose a good clock prescalar
  //SPI1CON1bits.SPRE = 0x0; //TODO: Also secondary prescalar
  SPI1STATbits.SPIEN = 1; // Enable
  SPI1STATbits.SPIROV = 0;

  TRISBbits.TRISB15 = 0; // SS
  TRISBbits.TRISB14 = 1; // SDI
  AD1PCFGbits.PCFG14 = 1; // SDI
  TRISBbits.TRISB13 = 0; // SDO
  TRISBbits.TRISB12 = 0; // SDCK

  DISABLE_MEMORY(); //idle state of SS is high
}

void mem_test() {
  print("testing...\r\n");

  ENABLE_MEMORY();
  wait_ms(1);

  //TODO

  wait_ms(1);
  DISABLE_MEMORY();
  wait_ms(1);
  //CloseSPI1();
}

void dbg_byte_print( BYTE b ) {
  char str[2];
  str[0] = b;
  if (!b)
    str[0] = 0x7E;
  str[1] = 0x0;

  print("(");
  print( str );
  print(")");
  print_byte( b );
}

bool shift_out( BYTE data ) {
  unsigned short count = 0;
  dbg_byte_print( data );

  while ( MEMORY_TX_STATUS && count<TIMEOUT)
    ++count;
  MEMORY_BUFFER_REGISTER = data;
  while ( MEMORY_RX_STATUS == 0 && count<TIMEOUT )
    ++count;

  //sendf( U2, "%d\r\n", count );
  if (count==TIMEOUT)
    return false;

  data = MEMORY_BUFFER_REGISTER;
  return true;
}

//max sizeof(int) bytes, MSB first
bool shift_n_out( int data, short numBytes ) {
  while( numBytes > 0 ) {
    if ( !shift_out( data & (0xFF << 3*numBytes) ) ) {
      return false;
    }
    --numBytes;
  }
  return true;
}

bool shift_in( BYTE* out ) {
  MEMORY_STATUS_OVERFLOWN = 0;
  MEMORY_BUFFER_REGISTER = 0x00;

  unsigned short count = 0;
  while( !MEMORY_RX_STATUS && count<TIMEOUT )
    ++count;

  if (count!=TIMEOUT && MEMORY_RX_STATUS)
  {
    MEMORY_STATUS_OVERFLOWN = 0;
    BYTE data = (MEMORY_BUFFER_REGISTER & 0xFF);
    *out = data;
    //dbg_byte_print( *out );
    return true;
  }
  return false;
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

BYTE _impl_mem_status() {
  BYTE status = 0xFF;
  READ_STATUS_REGISTER()
  shift_in( &status );
  return status;
}

BYTE mem_status() {
  ENABLE_MEMORY();
  BYTE status = _impl_mem_status();
  DISABLE_MEMORY();
  return status;
}

void mem_clear() {
  ENABLE_MEMORY();

  WRITE_MODE_ENABLE();
  ERASE_ALL();

  BYTE status = _impl_mem_status();
  print_byte( status );
  while (status & 0b1) {
    shift_in( &status );
    print_byte( status );
  }

  DISABLE_MEMORY();
}
