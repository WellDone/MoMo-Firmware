#include <pic12f1822.h>
#include "bus_master.h"
#include "bootloader.h"
#include "appcode.h"
#include "watchdog.h"

//Configuration Words
#pragma config FOSC=INTOSC      /* Use internal oscillator as the frequency oscillator. */
#pragma config WDTE=SWDTEN  /* Enable the watchdog timer under software control. */
#pragma config PLLEN=OFF        /* Disable 4x phase lock loop. */
#pragma config WRT=OFF          /* Flash memory write protection off. */

__persistent MIBExecutiveStatus status;

void initialize();
void restore_status();
void check_app_fault();

void interrupt service_isr() {
    wdt_pushenabled();
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
        wdt_enable(); 
        call_app_interrupt();
        wdt_disable();
    }

    wdt_popenabled();
}

void main() 
{    
    initialize();
    restore_status();

    if (status.bootload_mode)
    {
        enter_bootloader();
        restore_status();   //Update our status on what mode we should be in now
    }

    check_app_fault();

    if (status.valid_app)
    {  
        wdt_enable();
        call_app_init();
        wdt_disable();

        while(1)
        {
            wdt_enable();
            call_app_task();
            wdt_disable();

            sleep();
        }
    }

    //Otherwise wait forever for new firmware to be downloaded
    while (1)
        ;
}

void initialize()
{
    /* Software HS internal oscilator at 4 Mhz */
    OSCCON = (0b1101 << 3);

    //wait for the clock to stabilize.
    while (!HFIOFS)
        ;

    /* Set all PORTA pins to be input. */
    TRISA = 0xff;
    RA5 = 0;

    /* Set all PORTA pins to be digital I/O (instead of analog input). */
    ANSELA = 0;
    
    /* Enable interrupts globally. */
    GIE = 1;
    /* Enable peripheral interrupts. */
    PEIE = 1;
    wdt_settimeout(k1SecondTimeout);
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

/*
 * If we reset because of a watchdog timeout, assume the application code failed, and delay before recalling it
 * so that we have a chance to reflash if necessary
 */
void check_app_fault()
{
    if (status.wdt_timedout)
    {
        wdt_settimeout(k4SecondTimeout);
        wdt_enable();
        TRISA5 = 0;
        sleep();
        TRISA5 = 1;
        wdt_settimeout(k1SecondTimeout);
    }
}