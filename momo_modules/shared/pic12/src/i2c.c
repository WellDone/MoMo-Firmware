/*
 *
 */

#include "i2c.h"
#include "bus.h"

volatile I2CMasterStatus master;
volatile I2CSlaveStatus  slave;

extern MIBState       mib_state;

unsigned char i2c_slave_address;

//Internal functions
static void i2c_master_receivedata();
static void i2c_master_receivechecksum();

void i2c_enable(unsigned char slave_address)
{
    i2c_receive(); 

    master.state = kI2CIdleState;
    slave.state = kI2CIdleState;

    TRISA1 = 1; //SCL pin as input
    TRISA2 = 1; //SDA pin as input

    SSP1STAT = 0xff & kI2CFlagMask;
    SSPEN = 1; //Enable serial port and configure to use SDA/SCL as source

    /* Enable the MSSP interrupt (for i2c). */
    SSP1IF = 0;
    SSP1IE = 1;

    //Enable general call interrupt
    GCEN = 1;

    //address for slave mode
    i2c_slave_address = slave_address;

    // TODO need to come up with a better way of choosing between slave and master
    i2c_master_disable();
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
    if (master.state == kI2CIdleState)
        i2c_send_start();
    else
        i2c_send_repeatedstart();

    master.state = kI2CSendAddressState;
    master.last_error = kI2CNoError; //Initialize last error
}

void i2c_finish_transmission()
{
    i2c_send_stop();

    master.state = kI2CIdleState;
}

void i2c_send_message()
{
    mib_state.bus_msg.checksum = 0;

    //Check if this is a slave transmission
    if (!i2c_address_valid(mib_state.bus_msg.address))
    {
        slave.state = kI2CSendDataState;
        if (mib_state.bus_msg.flags & kSendImmediately)
            i2c_slave_sendbyte();
    }

    master.dir = kMasterSendData;
    mib_state.bus_msg.address <<= 1;

    CLEAR_BIT(mib_state.bus_msg.address, 0); //set write indication

    i2c_start_transmission();
}

void i2c_receive_message()
{
    if (!(mib_state.bus_msg.flags & kContinueChecksum))
        mib_state.bus_msg.checksum = 0;

    //Check if this is a slave reception
    if (!i2c_address_valid(mib_state.bus_msg.address))
    {
        slave.state = kI2CReceiveDataState;
        return;
    }

    master.dir = kMasterReceiveData;
    mib_state.bus_msg.address <<= 1;

    SET_BIT(mib_state.bus_msg.address, 0); //set read indication

    i2c_start_transmission();
}
