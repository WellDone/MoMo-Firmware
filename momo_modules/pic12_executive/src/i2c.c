/*
 *
 */

#include "i2c.h"
#include "bus.h"

void i2c_disable()
{
    //Disable general call interrupt
    SSP1CON2bits.GCEN = 0;

    //disable interrupts
    SSP1IE = 0;

    //disable serial port
    SSPCONbits.SSPEN = 0;
}