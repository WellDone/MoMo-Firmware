/*
 *
 */

#include "i2c.h"
#include "bus.h"

bank1 volatile I2CStatus i2c_status;

unsigned char i2c_slave_address;

//Internal functions
static void i2c_master_receivedata();
static void i2c_master_receivechecksum();

void i2c_enable(unsigned char slave_address)
{
    i2c_status.slave_active = 1;
    i2c_status.state = kI2CIdleState;

    TRISA1 = 1; //SCL pin as input
    TRISA2 = 1; //SDA pin as input

    SSP1IE = 0;

    SSP1STAT = 0xff & kI2CFlagMask;

    //address for slave mode
    i2c_slave_address = slave_address << 1; //Important: pic12 requires address preshifted

    i2c_set_slave_mode();
    i2c_release_clock();
    SEN = 1; //Enable clock stretching in slave mode

    SSPEN = 1;
    SSP1ADD = i2c_slave_address;

    
    i2c_receive();
    SSPOV = 0; //clear overflow bit

    /* Enable the MSSP interrupt (for i2c). */
    SSP1IF = 0;
    SSP1IE = 1;
}

void i2c_disable()
{
    //Disable general call interrupt
    GCEN = 0;

    //disable interrupts
    SSP1IE = 0;

    //disable serial port
    SSPEN = 0;
}

void i2c_start_transmission()
{
    if (i2c_status.state == kI2CIdleState)
        i2c_send_start();
    else
        i2c_send_repeatedstart();

    i2c_status.state = kI2CSendAddressState;
    i2c_status.last_error = kI2CNoError; //Initialize last error
}

void i2c_finish_transmission()
{
    i2c_send_stop();
    i2c_master_disable();
}

void i2c_send_message()
{
    mib_state.bus_msg.checksum = 0;

    //Check if this is a slave transmission
    if (!i2c_address_valid(mib_state.bus_msg.address))
    {
        i2c_status.state = kI2CSendDataState;
        if (mib_state.bus_msg.address == kInvalidImmediateAddress) //check if we should send immediately
            i2c_slave_sendbyte();

        return;
    }

    mib_state.bus_msg.address <<= 1;
    CLEAR_BIT(mib_state.bus_msg.address, 0); //set write indication

    i2c_start_transmission();
}

void i2c_receive_message()
{
    mib_state.bus_msg.checksum = 0;

    //Check if this is a slave reception
    if (!i2c_address_valid(mib_state.bus_msg.address))
    {
        i2c_status.state = kI2CReceiveDataState;
        return;
    }

    mib_state.bus_msg.address <<= 1;
    SET_BIT(mib_state.bus_msg.address, 0); //set read indication

    i2c_start_transmission();
}

uint8 i2c_lasterror()
{
    return i2c_status.last_error;
}

I2CLogicState i2c_state()
{
    return i2c_status.state;
}