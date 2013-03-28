#include "memory.h"
#include "rtcc.h"
#include "utilities.h"
#include "uart.h"

#define SS_VALUE LATBbits.LATB15
#define ENABLE_MEMORY() SS_VALUE = 0
#define DISABLE_MEMORY() SS_VALUE = 1
#define MEMORY_TX_STATUS SPI1STATbits.SPITBF
#define MEMORY_STATUS_OVERFLOWN SPI1STATbits.SPIROV
#define MEMORY_RX_STATUS SPI1STATbits.SPIRBF
#define MEMORY_BUFFER_REGISTER SPI1BUF
#define TIMEOUT 10000

typedef enum {
  WREN = 0b00000110,
  WRDI = 0b00000100,
  RDID = 0b10011111,
  RDSR = 0b00000101,
  WRSR = 0b00000001,
  READ = 0b00000011,
  PP   = 0b00000010,
  SSE  = 0b00100000,
  BE   = 0b11000111
} memory_instructions;

bool shift_out( BYTE data );

#define WRITE_MODE_ENABLE() shift_out( WREN )
#define WRITE_MODE_DISABLE() shift_out( WRDI )
#define READ_IDENTIFICATION() shift_out( RDID )
#define READ_STATUS_REGISTER() shift_out( RDSR )
#define PAGE_PROGRAM_MODE() shift_out( PP )
#define READ_MODE() shift_out( READ )
#define ERASE_SUBSECTION() shift_out( SSE )
#define ERASE_ALL() shift_out( BE )

bool configure_SPI() {
  SPI1CON1bits.MODE16 = 0; //communication is byte-wide
  SPI1CON1bits.MSTEN = 1; //SPI is in master mode
  SPI1CON1bits.CKP = 1; //data is clocked out on high-low transition
  //SPI1CON1bits.PPRE = 0x0; //TODO: Choose a good clock prescalar
  //SPI1CON1bits.SPRE = 0x0; //TODO: Also secondary prescalar
  SPI1STATbits.SPIEN = 1; // Enable
  SPI1STATbits.SPIROV = 0; // Clear the overflow flag.

  TRISBbits.TRISB15 = 0; // SS
  TRISBbits.TRISB14 = 1; // SDI (IO)
  AD1PCFGbits.PCFG14 = 1; // SDI (analog/digital)
  TRISBbits.TRISB13 = 0; // SDO
  TRISBbits.TRISB12 = 0; // SDCK

  DISABLE_MEMORY(); //idle state of SS is high
  return mem_test();
}

static inline bool shift_impl( BYTE data, BYTE* data_out ) {
  unsigned short count = 0;
  //print_byte( data );
  while ( MEMORY_TX_STATUS && count<TIMEOUT)
    ++count;
  MEMORY_BUFFER_REGISTER = data;
  while ( MEMORY_RX_STATUS == 0 && count<TIMEOUT )
    ++count;

  if (count==TIMEOUT)
    return false;

  *data_out = MEMORY_BUFFER_REGISTER;
  return true;
}
bool shift_out( BYTE data ) {
  return shift_impl( data, &data );
}

//Shift_out the lowest num_bytes bytes of data
//max sizeof(int) bytes, MSB first
bool shift_n_out( unsigned long data, short num_bytes ) {
  num_bytes = (num_bytes-1)<<3; //*=8
  while( num_bytes >= 0 ) {
    if ( !shift_out( (data>>num_bytes)&0xFF )) {
      return false;
    }
    num_bytes -= 8;
  }
  return true;
}

bool shift_in( BYTE* out ) {
  return shift_impl( 0x00, out );
}

bool mem_test() {
  BYTE device_info;
  BYTE info2, info3;
  print("Testing flash memory SPI communication...\r\n");

  ENABLE_MEMORY();

  READ_IDENTIFICATION();
  shift_in( &device_info );
  shift_in( &info2 );
  shift_in( &info3 );

  DISABLE_MEMORY();

  print( "Memory device ID: ");
  print_byte( device_info );
  print_byte( info2 );
  print_byte( info3 );

  if (!device_info) {
    print( "SPI test FAILED!!" );
    print( "\r\n" );
    return false;
  }
  return true;
}

static inline void _impl_mem_status( BYTE* status) {
  READ_STATUS_REGISTER();
  shift_in( status );
}
static inline void mem_enable_write() {
  ENABLE_MEMORY();
  WRITE_MODE_ENABLE();
  DISABLE_MEMORY();
}
static inline void mem_wait_while_writing() {
  ENABLE_MEMORY();
  BYTE status;
  _impl_mem_status( &status );
  while (status & 0b1) {
    shift_in( &status );
  }
  DISABLE_MEMORY();
}

// Length is capped at 256, 1 page of flash memory.
bool mem_write(unsigned long addr, const BYTE *data, unsigned int length) {
  int i;
  bool success = true;
  if ( length > 256) { //TODO: bitwise-ify
    length = 256;
  }

  mem_wait_while_writing();
  mem_enable_write();
  ENABLE_MEMORY();
  PAGE_PROGRAM_MODE();

  addr &= MEMORY_ADDRESS_MASK;
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

bool mem_read(unsigned long addr, BYTE* buf, unsigned int numBytes) {
  BYTE* bufEnd = buf+numBytes;
  bool success = true;

  ENABLE_MEMORY();
  READ_MODE();

  addr &= MEMORY_ADDRESS_MASK;
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

BYTE mem_status() {
  BYTE status;
  ENABLE_MEMORY();
  _impl_mem_status( &status );
  DISABLE_MEMORY();
  return status;
}

void mem_clear_all() {
  mem_wait_while_writing();
  mem_enable_write();

  ENABLE_MEMORY();
  ERASE_ALL();
  DISABLE_MEMORY();
}

void mem_clear_subsection( unsigned int addr ) {
  addr &= MEMORY_ADDRESS_MASK;
  mem_wait_while_writing();
  mem_enable_write();

  ENABLE_MEMORY();
  ERASE_SUBSECTION();
  shift_n_out( addr, 3 );
  DISABLE_MEMORY();
}
