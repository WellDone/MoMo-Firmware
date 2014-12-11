/* i2c.h
 * Module for managing the PIC12 i2c module in both master and slave mode. 
 * Slave mode management logic is mapped into a bootloader logic to support
 * reflashing over i2c.
 */

#ifndef __i2c_h__
#define __i2c_h__

#include "platform.h"
#include "i2c_defines.h"

#define i2c_release_clock()         SSP1CON1bits.CKP = 1
#define i2c_send_start()            SSP1CON2bits.SEN = 1
#define i2c_send_repeatedstart()    SSP1CON2bits.RSEN = 1
#define i2c_send_stop()             SSP1CON2bits.PEN = 1
#define i2c_begin_receive()         SSP1CON2bits.RCEN = 1
#define i2c_send_ack()              {SSP1CON2bits.ACKDT = 0; SSP1CON2bits.ACKEN = 1;}
#define i2c_send_nack()             {SSP1CON2bits.ACKDT = 1; SSP1CON2bits.ACKEN = 1;}
#define i2c_received_data()         (SSPSTATbits.BF == 1)
#define i2c_transmit_full()         (SSPSTATbits.BF == 1)
#define i2c_slave_is_read()         (SSPSTATbits.R_nW == 1)
#define i2c_byte_nacked()           (SSP1CON2bits.ACKSTAT)

//i2c slave conditions
#define i2c_stop_received()         (SSPSTATbits.P == 1)
#define i2c_address_received()      (SSPSTATbits.D_nA == 0)

#define kInvalidImmediateAddress          0x03                //Guaranteed not to be in use by the i2c protocol
#define kInvalidI2CAddress                0x01                //Also not in use by i2c protocol
#define i2c_address_valid(address)  (!(address > 0 && address < 4))

#define i2c_master_enable()         i2c_set_master_mode(1)

//Configuration Functions
void i2c_enable(unsigned char slave_address);
void i2c_disable();

//Buffer Setup/Validation Functions
void i2c_init_buffer(uint8 start);
uint8 i2c_calculate_checksum();
void i2c_append_checksum();
void i2c_append_checksum_at_offset(uint8 offset);
void i2c_loadbuffer();
void i2c_read();
void i2c_write();

//Shared Common Functions
void i2c_receive_message();

void  i2c_setstate(uint8 state);
void  i2c_transmit(uint8 value);
uint8 i2c_receive();
void  i2c_set_master_mode(uint8 mode);

void i2c_start_transmission();
void i2c_finish_transmission();

//Master Functions
uint8    i2c_master_send_message();
uint8    i2c_master_receive_message();

//Slave Functions
void    i2c_slave_interrupt();

#endif
