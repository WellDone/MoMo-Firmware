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

#define i2c_set_master_mode()       {SSPM3 = 1; SSPM2 = 0; SSPM1 = 0; SSPM0 = 0;}
#define i2c_set_slave_mode()        {SSPM3 = 1; SSPM2 = 1; SSPM1 = 1; SSPM0 = 0;}

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

#define kInvalidI2CAddress          0x02                //Guaranteed not to be in use by the i2c protocol
#define i2c_address_valid(address)  (address != kInvalidI2CAddress)

typedef enum
{
    kI2CIdleState = 0,
    kI2CSendAddressState,
    kI2CReceivedAddressState,
    kI2CSendDataState,
    kI2CSendChecksumState,
    kI2CReceiveDataState,
    kI2CReceiveChecksumState,
    kI2CUserCallbackState,
    kI2CDisabledState,          //When the slave logic is using the bus, disable the master and vice-versa
    kI2CForceStopState          //When a stop condition is asserted on the bus, make sure we clean up everything
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
    unsigned char len; //The length of the buffer pointed to by data pointer when receiving data, unused when sending

    unsigned char * data_ptr;
    unsigned char * last_data;

    unsigned char checksum;
    unsigned char flags;
} I2CMessage;

enum
{
    kCallbackBeforeChecksum = 1 << 0,
    kSendImmediately = 1 << 1,
    kContinueChecksum = 1 << 2
};

typedef enum 
{
    kMasterSendData = 0,
    kMasterReceiveData = 1
} I2CMasterDirection;

typedef struct
{
    I2CLogicState       state;
    I2CMasterDirection  dir;
    I2CErrorCode        last_error;
} I2CMasterStatus;

typedef struct 
{
    I2CLogicState state;
    I2CErrorCode  last_error;
} I2CSlaveStatus;

//Configuration Functions
void i2c_enable(unsigned char slave_address);
void i2c_disable();

//Shared Common Functions
void i2c_send_message();
void i2c_receive_message();

void i2c_start_transmission();
void i2c_finish_transmission();

//Master Functions
void    i2c_master_setidle();
uint8   i2c_master_lasterror();
void    i2c_master_enable();
void    i2c_master_disable();
void    i2c_master_interrupt();
I2CLogicState i2c_master_state();

//Slave Functions
void    i2c_slave_receivedata();
void    i2c_slave_receivechecksum();
void    i2c_slave_setidle();
void    i2c_slave_sendbyte();
uint8   i2c_slave_lasterror();
void    i2c_slave_interrupt();
I2CLogicState i2c_slave_state();

#endif
