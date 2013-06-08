#include <pic12f1822.h>
#include <stdint.h>
#include "i2c.h"
#include "flash_memory.h"
#include "typedef.h"

/* Mask for current I2C state. */
#define I2C_STATE_MASK 0x25

// pickit serial communication states
#define I2C_NO_TRANSACTION                  0
#define I2C_SLAVE_ADDRESS_RECEIVED          1
#define I2C_WORD_ADDRESS_RECEIVED           2
#define I2C_READ_ADDRESS                    3
#define I2C_READ_DATA                       4
#define I2C_MASTER_NACK                     5

// define i2c states
#define MWA 0x1         //Master Writes Address
#define MWD 0x21        //Master Writes Data
#define MRA 0x5         //Master Reads Address
#define MRD 0x24        //Master Reads Data

// slave address definition
#define SLAVE_ADDR 0xa0

ADDRESS flash_addr_pointer;
uint8_t flash_buffer[16];
uint8_t pksa_wd_address;
uint8_t pksa_index;
uint8_t pksa_status;

void i2c_slave_init()
{
    TRISA1 = 1;                 // SCL input
    TRISA2 = 1;                 // SDA input

    SSP1BUF = 0x0;
    SSP1STAT = 0x80;             // 100Khz
    SSP1ADD = SLAVE_ADDR;
    SSP1CON1 = 0x36;              // Slave mode, 7bit addr
    SSP1CON3 |= 0b01100000;      // enable start and stop conditions
}

void i2c_send(unsigned char data)
{
    do
    {
        WCOL=0;
        SSP1BUF = data;
    }
    while(WCOL);
    CKP = 1;
}

int do_i2c_tasks()
{
    unsigned int dat =0 ;
    unsigned char temp,idx;

    unsigned char token = 0;

    if (SSP1IF) {
        token = SSP1STAT & I2C_STATE_MASK;    //obtain current state
        if(_SSPSTAT_S_POSITION) {
            switch (token) {
                case MWA:                              //MASTER WRITES ADDRESS STATE
                    temp = SSP1BUF;
                    pksa_status = I2C_SLAVE_ADDRESS_RECEIVED;
                    break;
                case MWD:                              //MASTER WRITES DATA STATE
                    temp = SSP1BUF;

                    if (pksa_status == I2C_SLAVE_ADDRESS_RECEIVED) {   
                        // first time we get the slave address, after that set to word address
                        pksa_wd_address = temp;
                        pksa_index = 0;
                        pksa_status = I2C_WORD_ADDRESS_RECEIVED;
                    }  else if (pksa_status == I2C_WORD_ADDRESS_RECEIVED) {
                        // second time we get the word address, so look into word address
                        if (pksa_wd_address == 0x01) { // 0x01 is buffer word address
                            if (pksa_index == 0) {
                                flash_addr_pointer.bytes.byte_H = temp;
                                pksa_index++;
                            }
                            else if (pksa_index == 1) {
                                 flash_addr_pointer.bytes.byte_L = temp;
                            }
                        }
                        
                        if (pksa_wd_address == 0x02) {  // 0x02 write data word address
                            flash_buffer[pksa_index] = temp;
                            pksa_index++;
                            if (pksa_index == 16)
                                pksa_index--;
                        }
                    }
                    break;
                case MRA:                              //MASTER READS ADDRESS STATE
                    if (pksa_wd_address == 0x01) {           // buffer word address
                        // Send first byte here, next byte will be send at MRD case, see below
                        i2c_send (flash_addr_pointer.bytes.byte_H);
                    }
                    if (pksa_wd_address == 0x03) {    // read data from flash memory
                        if (pksa_index == 0) {
                            // read data into flash_buffer
                            for (idx = 0; idx <16; idx+=2) {
                                dat = flash_memory_read (flash_addr_pointer.word.address);
                                flash_buffer[idx  ] = dat>>8;
                                flash_buffer[idx+1] = dat & 0xFF;
                                flash_addr_pointer.word.address++;
                            }
                            // send first byte, the rest will be sent at MRD, see below
                            i2c_send(flash_buffer[pksa_index]);
                            pksa_index++;
                            if (pksa_index == 16)
                                pksa_index--;   // should never get here....
                        }
                    }
                    if (pksa_wd_address == 0x04) {
                        // erase command, erases a row of 32 words
                        flash_memory_erase (flash_addr_pointer.word.address);
                        flash_addr_pointer.word.address +=32;
                        i2c_send(0x00);
                    }
                    if (pksa_wd_address == 0x05) {
                        // write command. What's stored into flash_buffer is written
                        // to FLASH memory at the address pointed to by the address pointer.
                        // The address pointer automatically increments by 8 units.
                        flash_memory_write (flash_addr_pointer.word.address, flash_buffer );
                        flash_addr_pointer.word.address +=8;
                        i2c_send(0x00);

                    }
                    if (pksa_wd_address == 0x06) {
                        // jump to appplication code
                        i2c_send(0x00);
                        for (idx = 0; idx < 255; idx++);
                        #asm
                            RESET;
                        #endasm
                    }
                    break;
                case MRD :                              //MASTER READS DATA STATE
                        if (pksa_wd_address == 0x01) {  // buffer word address
                            i2c_send(flash_addr_pointer.bytes.byte_L);
                        }
                        if (pksa_wd_address == 0x03) {
                            i2c_send(flash_buffer[pksa_index]);
                            pksa_index++;
                            if (pksa_index == 16)
                                pksa_index--;
                        }
                    break;
            }
        }
        else if (_SSPSTAT_P_POSITION) {   //STOP state
            asm("nop");
            pksa_status = I2C_NO_TRANSACTION;
        }

        SSP1IF = 0;
        SSPEN = 1;
        CKP = 1;    //release clock
    }
}
