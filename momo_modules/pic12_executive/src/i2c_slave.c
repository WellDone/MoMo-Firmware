#include "i2c.h"
#include "bus_slave.h"

extern bank1 volatile I2CStatus i2c_status;

#define i2c_msg		(&mib_state.bus_msg)

uint8 i2c_slave_active()
{
	return i2c_status.slave_active;
}


void i2c_slave_setidle()
{
	i2c_setstate(kI2CIdleState);
	i2c_status.last_error = kI2CNoError;

	//i2c_master_enable(); //let the master logic use the bus again if it needs.

	i2c_release_clock();
}

//#pragma interrupt_level 1
void i2c_slave_interrupt()
{	
	if (i2c_address_received())
	{
		i2c_receive();
		SSPCON1bits.SSPOV = 0; //reset receive overflow flag

		bus_slave_callback();
	}		
	else 
	{
		switch(i2c_status.state)
		{
			case kI2CReceiveDataState:
			i2c_core_transfer(pack_i2c_states(0,0, kI2CReceiveDataState, kI2CReceiveChecksumState));
			break;

			case kI2CReceiveChecksumState:
			i2c_core_receivechecksum();
			bus_slave_callback();
			i2c_release_clock();
			break;

			case kI2CSendDataState:
			i2c_core_transfer(pack_i2c_states(0,1, kI2CSendDataState, kI2CSendChecksumState));
			break;

			case kI2CSendChecksumState:
			i2c_msg->checksum = (~i2c_msg->checksum) + 1;
			i2c_transmit(i2c_msg->checksum);

			i2c_setstate(kI2CUserCallbackState);
			i2c_release_clock();
			break;

			case kI2CUserCallbackState:
			bus_slave_callback();
			i2c_setstate(kI2CIdleState);
			break;

			default:
			//by default do read bytes even if we don't know what to do with them so that the protocol doesn't
			//lock up
			i2c_receive();
			SSPCON1bits.SSPOV = 0; 
			i2c_release_clock();
			break;
		}
	}
}
