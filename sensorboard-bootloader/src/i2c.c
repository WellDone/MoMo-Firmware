/*
 *
 */

#include "i2c.h"
#include "bus.h"

volatile I2CMasterStatus master;
volatile I2CSlaveStatus  slave;

volatile I2CMessage      *i2c_msg;

void (*i2c_master_callback)(void);
void (*i2c_slave_callback)(void);
unsigned char i2c_slave_address;

void i2c_enable(unsigned char slave_address, void *master_callback, void *slave_callback)
{
    unsigned char unused;

    master.state = kI2CIdleState;
    slave.state = kI2CIdleState;

    unused = SSP1BUF; //Clear out receive buffer
    TRISA1 = 1; //SCL pin as input
    TRISA2 = 1; //SDA pin as input
    SSPEN = 1; //Enable serial port and configure to use SDA/SCL as source

    SSP1STAT = 0xff & kI2CFlagMask;

    /* Enable the MSSP interrupt (for i2c). */
    SSP1IF = 0;
    SSP1IE = 1;

    //Enable general call interrupt
    GCEN = 1;

    //i2c callbacks for processing events
    i2c_master_callback = master_callback;
    i2c_slave_callback = slave_callback;

    //address for slave mode
    i2c_slave_address = slave_address;

    // TODO need to come up with a better way of choosing between slave and master
    SSP1ADD = i2c_slave_address;
    i2c_set_slave_mode();
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

int i2c_send_message(volatile I2CMessage *msg)
{
    i2c_msg = msg;
    msg->checksum = 0;

    //Check if this is a slave transmission
    if (!i2c_address_valid(msg->address))
    {
        slave.state = kI2CSendDataState;
        if (msg->flags & kSendImmediately)
            i2c_slave_sendbyte();

        return 0;
    }

    master.dir = kMasterSendData;
    msg->address <<= 1;

    CLEAR_BIT(msg->address, 0); //set write indication

    i2c_start_transmission();
    return 0;
}

int i2c_receive_message(volatile I2CMessage *msg)
{
    i2c_msg = msg;

    if (!(msg->flags & kContinueChecksum))
        msg->checksum = 0;

    //Check if this is a slave reception
    if (!i2c_address_valid(msg->address))
    {
        slave.state = kI2CReceiveDataState;
        return 0;
    }

    master.dir = kMasterReceiveData;
    msg->address <<= 1;

    SET_BIT(msg->address, 0); //set read indication

    i2c_start_transmission();
    return 0;
}
