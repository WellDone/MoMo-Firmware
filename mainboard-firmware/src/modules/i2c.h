/* i2c.h
 * Module for managing the PIC24 i2c module in both master and slave mode. 
 * Slave mode management logic is mapped into a bootloader logic to support
 * reflashing over i2c.
 */

#ifndef __i2c_h__
#define __i2c_h__

#include "common.h"

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

#define i2c_init_flags(conf) 		(conf->flags = 0)
#define i2c_set_flag(conf, flag)	(conf->flags |= flag)
#define i2c_release_clock()			_SCLREL = 1

typedef struct
{
	unsigned int 	flags;
	unsigned char	address; //7-bit address for this device
	unsigned char	priority;
} I2CConfig;

void i2c_configure(const I2CConfig *config);
void i2c_enable();
void i2c_disable();

#endif