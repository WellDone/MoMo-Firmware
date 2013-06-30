/* i2c.h
 * Module for managing the PIC24 i2c module in both master and slave mode. 
 * Slave mode management logic is mapped into a bootloader logic to support
 * reflashing over i2c.
 */

#ifndef __i2c_h__
#define __i2c_h__

#include "pic24.h"
#include "task_manager.h"

enum
{
	kDisableI2CInIdleFlag  = 1 << 13,
	kEnableIPMISupportFlag = 1 << 11,
	kUse10BitAddressingFlag = 1 << 10,
	kDisableI2CSlewControlFlag = 1 << 9,
	kSMBusLevelCompliantFlag = 1 << 8,
	kEnableGeneralCallFlag = 1 << 7,
	kEnableSoftwareClockStretchingFlag = 1 << 6
};

#define kI2CFlagMask (	kDisableI2CInIdleFlag | kEnableIPMISupportFlag |				\
						kUse10BitAddressingFlag | kDisableI2CSlewControlFlag |		\
						kSMBusLevelCompliantFlag | kEnableGeneralCallFlag |			\
						kEnableSoftwareClockStretchingFlag	)

#define i2c_init_flags(conf) 		((conf)->flags = 0)
#define i2c_set_flag(conf, flag)	((conf)->flags |= flag)
#define i2c_release_clock()			_SCLREL = 1
#define i2c_send_start()			_SEN = 1
#define i2c_send_repeatedstart()	_RSEN = 1
#define	i2c_send_stop()				_PEN = 1
#define i2c_begin_receive()			_RCEN = 1
#define i2c_send_ack()				{_ACKDT = 0; _ACKEN = 1;}
#define i2c_send_nack()				{_ACKDT = 1; _ACKEN = 1;}
#define i2c_received_data()			_RBF
#define i2c_has_checksum()			(!(curr_msg->flags & kI2CMessageNoChecksum))
#define i2c_transmit_full()			(_TBF == 1)
#define i2c_transmit(byte)			I2C1TRN = (byte)
#define i2c_slave_is_read()			(_R_NOT_W == 1)

//i2c slave conditions
#define i2c_stop_received()			(_P == 1)
#define i2c_address_received()		(_D_NOT_A == 0)

#define kInvalidI2CAddress			0x02				//Guaranteed not to be in use by the i2c protocol
#define i2c_address_valid(address) 	(address != kInvalidI2CAddress)



typedef struct
{
	unsigned int 	flags;
	unsigned char	address; //7-bit address for this device
	unsigned char	priority;

	task_callback	callback;
	task_callback	slave_callback;
} I2CConfig;

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
	kI2CDisabledState, 			//When the slave logic is using the bus, disable the master and vice-versa
	kI2CForceStopState 			//When a stop condition is asserted on the bus, make sure we clean up everything
} I2CLogicState;

typedef enum
{
	kI2CNoError = 0,
	kI2CInvalidChecksum
} I2CErrorCode;

typedef struct
{
	unsigned char address;
	unsigned char len; //The length of the buffer pointed to by data pointer when receiving data, unused when sending

	volatile unsigned char * volatile data_ptr;
	volatile unsigned char * volatile last_data;

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
	I2CLogicState 		state;
	I2CMasterDirection 	dir;
	I2CErrorCode		last_error;
} I2CMasterStatus;

typedef struct 
{
	I2CLogicState state;
	I2CErrorCode  last_error;
} I2CSlaveStatus;

//Configuration Functions
void i2c_configure(const I2CConfig *config);
void i2c_enable();
void i2c_disable();

//Shared Common Functions
void i2c_send_message();
void i2c_receive_message();

void i2c_start_transmission();
void i2c_finish_transmission();

//Master Functions
void i2c_master_receivedata();
void i2c_master_receivechecksum();
void i2c_master_setidle();
int  i2c_master_lasterror();
void i2c_master_enable();
void i2c_master_disable();

//Slave Functions
void i2c_slave_receivedata();
void i2c_slave_receivechecksum();
void i2c_slave_setidle();
void i2c_slave_sendbyte();
int  i2c_slave_lasterror();

I2CLogicState i2c_slave_state();

#endif