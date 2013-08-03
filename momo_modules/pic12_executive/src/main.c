#include <pic12f1822.h>
#include "bus_master.h"
#include "bootloader.h"

//Configuration Words
#pragma config FOSC=INTOSC      /* Use internal oscillator as the frequency oscillator. */
#pragma config WDTE=OFF         /* Disable the watchdog timer. */
#pragma config PLLEN=OFF        /* Disbable 4x phase lock loop. */
#pragma config WRT=OFF          /* Flash memory write protection off. */

MIBExecutiveStatus status;

void initialize();
void restore_status();

void interrupt service_isr() {
    // Handle i2c interrupts (MSSP) in the bootloader.
    if (SSP1IF == 1) 
    {
        while (SSP1IF == 1)
        {
            SSP1IF = 0; //Do this because with our slow clock we might miss an interrupt
            if (i2c_slave_active()) 
                i2c_slave_interrupt();
            else
                i2c_master_interrupt();
        }
    } 
    else if (status.valid_app)
    {
        #asm
        GOTO kFirstApplicationRow*16+1
        #endasm
    }
}

void main() 
{
    status.status = 0;
    
    initialize();
    restore_status();

    if (status.bootload_mode)
    {
        enter_bootloader();
        restore_status();   //Update our status on what mode we should be in now
    }

    if (status.valid_app)
    {  
        #asm
        GOTO kFirstApplicationRow*16
        #endasm
    }
    while (1) {
        ;
    }
}

void initialize ()
{
    /* Software HS internal oscilator at 4 Mhz */
    OSCCON = (0b1101 << 3);

    //wait for the clock to stabilize.
    while (!HFIOFS)
        ;

    /* Set all PORTA pins to be input. */
    TRISA = 0xff;

    /* Set all PORTA pins to be digital I/O (instead of analog input). */
    ANSELA = 0;
    /* Turn Timer1 on with 1:8 prescale using FOSC/4 as source. */
    T1CON = 0x31;
    
    /* Enable interrupts globally. */
    GIE = 1;
    /* Enable peripheral interrupts. */
    PEIE = 1;
}

void restore_status()
{
    if (get_magic() == kMIBMagicNumber)
        status.valid_app = 1;
    else if (get_magic() == kReflashMagicNumber)
        status.bootload_mode = 1;

    //TODO: mib autoregistration
    //Request an address from controller pic
    if (!status.registered)
    {
        status.registered = 1;
        bus_init(13);
    }
}