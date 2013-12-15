/*
 *
 */

#include "i2c.h"
#include "bus.h"

extern bank1 volatile I2CStatus i2c_status;
unsigned char i2c_slave_address;

//Internal functions
static void i2c_master_receivedata();
static void i2c_master_receivechecksum();

void i2c_disable()
{
    //Disable general call interrupt
    SSP1CON2bits.GCEN = 0;

    //disable interrupts
    SSP1IE = 0;

    //disable serial port
    SSPCONbits.SSPEN = 0;
}

//#pragma interrupt_level 1
void i2c_start_transmission()
{
    if (i2c_status.state == kI2CIdleState)
        i2c_send_start();
    else
        i2c_send_repeatedstart();

    i2c_setstate(kI2CSendAddressState);
    i2c_status.last_error = kI2CNoError; //Initialize last error
}

//#pragma interrupt_level 1
void i2c_finish_transmission()
{
    i2c_send_stop();
    i2c_set_master_mode(0);
}

void i2c_master_send_message()
{
    mib_state.bus_msg.checksum = 0;
    mib_state.bus_msg.address <<= 1;
    CLEAR_BIT(mib_state.bus_msg.address, 0); //set write indication

    i2c_start_transmission();
}

void i2c_slave_send_message()
{
    mib_state.bus_msg.checksum = 0;

    i2c_setstate(kI2CSendDataState);
    if (mib_state.bus_msg.address == kInvalidImmediateAddress) //check if we should send immediately
        i2c_core_transfer(pack_i2c_states(0,1, kI2CSendDataState, kI2CSendChecksumState));
}

void i2c_master_receive_message()
{
    mib_state.bus_msg.checksum = 0;

    mib_state.bus_msg.address <<= 1;
    SET_BIT(mib_state.bus_msg.address, 0); //set read indication

    i2c_start_transmission();
}

void i2c_slave_receive_message()
{
    mib_state.bus_msg.checksum = 0;

    i2c_setstate(kI2CReceiveDataState);
}

uint8 i2c_lasterror()
{
    return i2c_status.last_error;
}

uint8 i2c_state()
{
    return i2c_status.state;
}