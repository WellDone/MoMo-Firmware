/*
 *
 */

#include "i2c.h"
#include "pme.h"

void i2c_configure(const I2CConfig *config)
{
	peripheral_enable(kI2CModule);
	
	//Set flags
	I2C1CON = config->flags & kI2CFlagMask;
	
	//Set slave address
	I2C1MSK = 0x00; //All address bits are significant
	I2C1ADD = config->address;

	//Set priority
	_SI2C1P = config->priority;
}

void i2c_enable()
{
	//enable module power
	peripheral_enable(kI2CModule);

	//set up interrupts
	_SI2C1IF = 0;
	_SI2C1IE = 1;

}

void i2c_disable()
{
	//disable interrupts
	_SI2C1IE = 0;

	//cut power to module
	peripheral_disable(kI2CModule);
}