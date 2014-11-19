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

//Configuration Functions
void i2c_enable(unsigned char slave_address);
void i2c_disable();

//Buffer Setup/Validation Functions
uint8 i2c_calculate_checksum();
void i2c_append_checksum();
void i2c_loadbuffer();
void i2c_init_location();
void i2c_read();
void i2c_write();

//Shared routines
void  i2c_set_master_mode(uint8 mode);

//Master Functions
uint8 i2c_master_send_message();
uint8 i2c_master_receive_message();
void  i2c_finish_transmission();


//Slave Functions
void i2c_slave_interrupt();

#endif
