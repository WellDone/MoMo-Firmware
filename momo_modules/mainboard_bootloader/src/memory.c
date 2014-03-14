#include "memory.h"
#include "bootloader.h"

#define SS_VALUE LATBbits.LATB15
#define ENABLE_MEMORY() SS_VALUE = 0
#define DISABLE_MEMORY() SS_VALUE = 1
#define MEMORY_TX_STATUS SPI1STATbits.SPITBF
#define MEMORY_STATUS_OVERFLOWN SPI1STATbits.SPIROV
#define MEMORY_RX_STATUS SPI1STATbits.SPIRBF
#define MEMORY_BUFFER_REGISTER SPI1BUF

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

//Functions only for this 
static BYTE spi_transfer(BYTE data);
static void spi_send_addr(unsigned long data);

#define spi_receive()   spi_transfer(0x00)

#define WRITE_MODE_ENABLE() spi_transfer( WREN )
#define WRITE_MODE_DISABLE() spi_transfer( WRDI )
#define READ_IDENTIFICATION() spi_transfer( RDID )
#define READ_STATUS_REGISTER() spi_transfer( RDSR )
#define PAGE_PROGRAM_MODE() spi_transfer( PP )
#define READ_MODE() spi_transfer( READ )
#define ERASE_SUBSECTION() spi_transfer( SSE )
#define ERASE_ALL() spi_transfer( BE )

void _BOOTLOADER_CODE configure_SPI() {
  SPI1CON1bits.MODE16 = 0; //communication is byte-wide
  SPI1CON1bits.MSTEN = 1; //SPI is in master mode
  SPI1CON1bits.CKP = 1; //data is clocked out on high-low transition
  SPI1STATbits.SPIROV = 0; // Clear the overflow flag.
  SPI1STATbits.SPIEN = 1; // Enable

  TRISBbits.TRISB15 = 0; // SS
  TRISBbits.TRISB14 = 1; // SDI (IO)
  AD1PCFGbits.PCFG14 = 1; // SDI (analog/digital)
  TRISBbits.TRISB13 = 0; // SDO
  TRISBbits.TRISB12 = 0; // SDCK

  DISABLE_MEMORY(); //idle state of SS is high
}

static BYTE _BOOTLOADER_CODE spi_transfer(BYTE data)
{

  while(MEMORY_TX_STATUS)
    ;

  MEMORY_BUFFER_REGISTER = data;
  while(MEMORY_RX_STATUS == 0)
    ;

  return MEMORY_BUFFER_REGISTER;
}

static void _BOOTLOADER_CODE spi_send_addr(unsigned long data)
{
    data &= MEMORY_ADDRESS_MASK;
    spi_transfer((data>>16)&0xFF);
    spi_transfer((data>>8)&0xFF);
    spi_transfer(data&0xFF);
}

void _BOOTLOADER_CODE mem_read(uint32 addr, BYTE* buf, unsigned int numBytes) 
{
  BYTE* bufEnd = buf+numBytes;

  ENABLE_MEMORY();
  READ_MODE();

  spi_send_addr(addr);

  while ( buf != bufEnd ) 
    *buf++ = spi_receive();

  DISABLE_MEMORY();
}

BYTE _BOOTLOADER_CODE mem_status() 
{
  BYTE status;

  ENABLE_MEMORY();
  READ_STATUS_REGISTER();
  status = spi_receive();
  DISABLE_MEMORY();

  return status;
}