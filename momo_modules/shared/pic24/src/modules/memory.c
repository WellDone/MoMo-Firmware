#include "memory.h"

//Clock configuration and delays
#define FCY   4000000L  //define your instruction frequency, FCY = FOSC/2
  
#define CYCLES_PER_MS ((unsigned long long)(FCY * 0.001))        //instruction cycles per millisecond
#define CYCLES_PER_US ((unsigned long long)(FCY * 0.000001))   //instruction cycles per microsecond
#define DELAY_MS(ms)  __delay32(CYCLES_PER_MS * ((unsigned long long) ms));   //__delay32 is provided by the compiler, delay some # of milliseconds
#define DELAY_US(us)  __delay32(CYCLES_PER_US * ((unsigned long long) us));    //delay some number of microseconds
extern void __delay32(unsigned long long);

memory_config status;

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
static void configure_SPI();


#define spi_receive()   spi_transfer(0x00)

#define WRITE_MODE_ENABLE() spi_transfer( WREN )
#define WRITE_MODE_DISABLE() spi_transfer( WRDI )
#define READ_IDENTIFICATION() spi_transfer( RDID )
#define READ_STATUS_REGISTER() spi_transfer( RDSR )
#define PAGE_PROGRAM_MODE() spi_transfer( PP )
#define READ_MODE() spi_transfer( READ )
#define ERASE_SUBSECTION() spi_transfer( SSE )
#define ERASE_ALL() spi_transfer( BE )

void init_memory()
{
  status.enabled = 0;
}

void configure_SPI() {
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

void enable_memory(uint8 for_writing)
{
  if (status.enabled == 0)
  {
    configure_SPI();

    //Enable the Memory Module
    _RB7 = 1;
    _TRISB7 = 0;

    status.enabled = 1;
    status.write_wait = 1;

    //Memory specifies a 30 us delay between power on and assert CS, allow for slop in clock
    DELAY_MS(1);
  }
  
  if (for_writing && status.write_wait)
  {
    DELAY_MS(12);
    status.write_wait = 0;
  }
}

unsigned int memory_enabled()
{
  return status.enabled;
}

void disable_memory()
{
  SPI1STATbits.SPIEN = 0;

  _RB7 = 0;
  _TRISB7 = 0;

  //Drive all pins low to minimize power consumption
  TRISBbits.TRISB15 = 0; // SS
  TRISBbits.TRISB14 = 0; // SDI (IO)
  AD1PCFGbits.PCFG14 = 1; // SDI (analog/digital)
  TRISBbits.TRISB13 = 0; // SDO
  TRISBbits.TRISB12 = 0; // SDCK
  _LATB15 = 0;
  _LATB14 = 0;
  _LATB13 = 0;
  _LATB12 = 0;

  status.enabled = 0;
}

static BYTE spi_transfer(BYTE data)
{

  while(MEMORY_TX_STATUS)
    ;

  MEMORY_BUFFER_REGISTER = data;
  while(MEMORY_RX_STATUS == 0)
    ;

  return MEMORY_BUFFER_REGISTER;
}

static void spi_send_addr(unsigned long data)
{
    data &= MEMORY_ADDRESS_MASK;
    spi_transfer((data>>16)&0xFF);
    spi_transfer((data>>8)&0xFF);
    spi_transfer(data&0xFF);
}

bool mem_test() 
{
  BYTE manufacturer_id;
  BYTE memory_type;
  BYTE memory_capacity;

  enable_memory(0);

  ENABLE_MEMORY();

  READ_IDENTIFICATION();
  manufacturer_id = spi_receive();
  memory_type  = spi_receive();
  memory_capacity = spi_receive();

  DISABLE_MEMORY();

  if ( manufacturer_id != 0x20 || memory_type != 0x71 || memory_capacity != 0x14 ) // M25PX80 = 0x20, 0x71
    return false;

  return true;
}

static inline void mem_enable_write() 
{
  ENABLE_MEMORY();
  WRITE_MODE_ENABLE();
  DISABLE_MEMORY();
}

void mem_wait_while_writing() 
{
  BYTE received = 0b1;

  ENABLE_MEMORY();
  
  READ_STATUS_REGISTER();
  while (received & 0b1)
    received = spi_receive();

  DISABLE_MEMORY();
}

void mem_write(uint32 addr, const BYTE* data, unsigned int length)
{
  uint32 end_addr = addr+length;
  
  while (addr < end_addr)
  {
    uint32 page_rem = 256 - (addr & 0xFF);
    uint32 rem = end_addr - addr;
    unsigned int write_len;

    if (page_rem >= rem)
      write_len = rem;
    else
      write_len = page_rem;

    mem_write_aligned(addr, data, write_len);
    addr += write_len;
    data += write_len;
  }
}

// Length is capped at 256, 1 page of flash memory.
void mem_write_aligned(const uint32 addr, const BYTE *data, unsigned int length) 
{
  unsigned int i;

  enable_memory(1);

  mem_enable_write();
  ENABLE_MEMORY();
  PAGE_PROGRAM_MODE();

  spi_send_addr(addr);

  for(i = 0; i < length; ++i) 
    spi_transfer(data[i]);

  DISABLE_MEMORY();

  mem_wait_while_writing();
}

void mem_read(uint32 addr, BYTE* buf, unsigned int numBytes) 
{
  BYTE* bufEnd = buf+numBytes;

  enable_memory(0);

  ENABLE_MEMORY();
  READ_MODE();

  spi_send_addr(addr);

  while ( buf != bufEnd ) 
    *buf++ = spi_receive();

  DISABLE_MEMORY();
}

BYTE mem_status() 
{
  BYTE status;

  enable_memory(0);

  ENABLE_MEMORY();
  READ_STATUS_REGISTER();
  status = spi_receive();
  DISABLE_MEMORY();

  return status;
}

void mem_clear_all() 
{
  enable_memory(1);
  mem_enable_write();

  ENABLE_MEMORY();
  ERASE_ALL();
  DISABLE_MEMORY();

  mem_wait_while_writing();
}

void mem_clear_subsection(uint32 addr) 
{
  enable_memory(1);
  mem_enable_write();

  ENABLE_MEMORY();
  ERASE_SUBSECTION();
  spi_send_addr(addr);
  DISABLE_MEMORY();

  mem_wait_while_writing();
}
