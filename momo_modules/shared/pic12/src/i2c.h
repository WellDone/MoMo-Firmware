/* i2c.h
 * Module for managing the PIC12 i2c module in both master and slave mode. 
 * Slave mode management logic is mapped into a bootloader logic to support
 * reflashing over i2c.
 */

#ifndef __i2c_h__
#define __i2c_h__

#include "platform.h"

enum
{
    kDisableI2CSlewControlFlag = 1 << 7,
    kSMBusLevelCompliantFlag = 1 << 6
};

#define kI2CFlagMask (  kDisableI2CSlewControlFlag | kSMBusLevelCompliantFlag )

#define i2c_set_master_mode()       SSP1CON1 = 0b00111000            //enable and set to master mode (0b1000)
#define i2c_set_slave_mode()        SSP1CON1 = 0b00010110            //enable and set to slave mode (0b0110)

#define i2c_release_clock()         CKP = 1
#define i2c_send_start()            SEN = 1
#define i2c_send_repeatedstart()    RSEN = 1
#define i2c_send_stop()             PEN = 1
#define i2c_begin_receive()         RCEN = 1
#define i2c_send_ack()              {ACKDT = 0; ACKEN = 1;}
#define i2c_send_nack()             {ACKDT = 1; ACKEN = 1;}
#define i2c_received_data()         (BF == 1)
#define i2c_has_checksum()          (!(curr_msg->flags & kI2CMessageNoChecksum))
#define i2c_transmit_full()         (BF == 1)
#define i2c_transmit(byte)          SSP1BUF = (byte)
#define i2c_receive()               (SSP1BUF)
#define i2c_slave_is_read()         (R_nW == 1)
#define i2c_byte_nacked()           (ACKSTAT)

//i2c slave conditions
#define i2c_stop_received()         (P == 1)
#define i2c_address_received()      (D_nA == 0)

#define kInvalidImmediateAddress          0x03                //Guaranteed not to be in use by the i2c protocol
#define kInvalidI2CAddress                0x01                //Also not in use by i2c protocol
#define i2c_address_valid(address)  (!(address > 0 && address < 4))

//CANNOT CHANGE ENUM: Used in i2c_utilities.as
typedef enum
{
    kI2CIdleState = 0,
    kI2CSendAddressState = 1,
    kI2CSendDataState = 2,
    kI2CSendChecksumState = 3,
    kI2CReceiveDataState = 4,
    kI2CReceiveChecksumState = 5,
    kI2CUserCallbackState = 6,
    kI2CDisabledState = 7,          //When the slave logic is using the bus, disable the master and vice-versa
	kI2CReceivedChecksumState = 8
} I2CLogicState;

typedef enum
{
    kI2CNoError = 0,
    kI2CInvalidChecksum,
    kI2CNackReceived
} I2CErrorCode;

typedef struct
{
    unsigned char address;
    unsigned char checksum;

    unsigned char * data_ptr;
    unsigned char * last_data;

} I2CMessage;

enum
{
    kSendImmediately = 1 << 1,
};

typedef struct
{
    uint8               state:7;
    uint8               slave_active:1;
    I2CErrorCode        last_error;
} I2CStatus;

//Configuration Functions
void i2c_enable(unsigned char slave_address);
void i2c_disable();

//Shared Common Functions
void  i2c_receive_message();

void i2c_start_transmission();
void i2c_finish_transmission();

//Master Functions
void    i2c_master_send_message();
void    i2c_master_receive_message();
void    i2c_master_setidle();
void    i2c_master_enable();
void    i2c_master_disable();
void    i2c_master_interrupt();

//Slave Functions
void    i2c_slave_receivedata();
void    i2c_slave_receivechecksum();
void    i2c_slave_setidle();
void    i2c_slave_sendbyte();
void    i2c_slave_send_message();
void    i2c_slave_receive_message();
void    i2c_slave_interrupt();

uint8         i2c_lasterror();
I2CLogicState i2c_state();
uint8         i2c_slave_active();

//Utility Functions (i2c_utilities.as)
void i2c_choose_direction();

#endif
