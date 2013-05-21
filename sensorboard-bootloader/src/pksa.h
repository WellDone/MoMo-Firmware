#include "typedef.h"

// pickit serial communication states
#define I2C_NO_TRANSACTION                  0
#define I2C_SLAVE_ADDRESS_RECEIVED                              1
#define I2C_WORD_ADDRESS_RECEIVED                               2
#define I2C_READ_ADDRESS                    3
#define I2C_READ_DATA                       4
#define I2C_MASTER_NACK                     5

// slave address definition
#define SLAVE_ADDR 0xa0

// define states
#define MWA 0x1         //Master Writes Address
#define MWD 0x21        //Master Writes Data
#define MRA 0x5         //Master Reads Address
#define MRD 0x24        //Master Reads Data

// address typdef
extern ADDRESS  flash_addr_pointer;

// function prototypes
void I2C_Slave_Init(void);
int  do_i2c_tasks(void);

// externs
extern unsigned char nrbytes;
extern unsigned char flash_buffer[];
extern unsigned char pksa_index;
extern unsigned char pksa_wd_address;
extern unsigned char pksa_status;
extern unsigned char pksa_cmd;
