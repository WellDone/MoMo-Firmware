#include <xc.h>
#include "bus_master.h"
#include "bootloader.h"
#include "appcode.h"
#include "watchdog.h"
#include "registration.h"
#include "mib_definitions.h"

//Configuration Words
#pragma config FOSC=INTOSC      /* Use internal oscillator as the frequency oscillator. */
#pragma config WDTE=SWDTEN  /* Enable the watchdog timer under software control. */
#pragma config PLLEN=OFF        /* Disable 4x phase lock loop. */
#pragma config WRT=OFF          /* Flash memory write protection off. */
#pragma config LVP=OFF
#pragma config MCLRE=OFF

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
            {
                i2c_slave_interrupt();
            }
            else
                i2c_master_interrupt();
        }
    } 
    else if (status.valid_app)
    {
        wdt_settimeout(k1SecondTimeout);
        wdt_enable(); 
        call_app_interrupt();
        wdt_disable();
    }

    wdt_popenabled();
}

void main() 
{    
    initialize();

    TRISA2 = !TRISA2;

    restore_status();

    TRISA2 = !TRISA2;

    if (status.bootload_mode)
    {
        enter_bootloader();
        restore_status();   //Update our status on what mode we should be in now
    }

    check_app_fault();
    
    if (status.valid_app)
    {  
        wdt_settimeout(k1SecondTimeout);
        wdt_enable();
        call_app_init();
        wdt_disable();

        while(1)
        {
            wdt_settimeout(k1SecondTimeout);
            wdt_enable();
            call_app_task();
            wdt_disable();

            sleep();
        }
    }
    //Otherwise wait forever for new firmware to be downloaded
    while (1)
    {
        sleep();
    }

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

    #ifdef __PIC16LF1823__
    TRISC = 0xff;
    ANSELC = 0;
    #endif

    RA2 = 0;

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

    //Request an address from controller pic
    if (!status.registered)
    {
        uint8 address = 0;
        
        //Wait 1 second to make the controller had time to power on
        //in case this is a power on reset
        wdt_settimeout(k1SecondTimeout);
        wdt_enable();
        sleep();
        wdt_disable();

        bus_init(kMIBUnenumeratedAddress);

        address = register_module();

        if (address > 0)
        {
            bus_init(address);
            status.registered = 1;
        }
    }
}

/*
 * If we reset because of a watchdog timeout, assume the application code failed and disable it.
 *
 * TODO: remap pins on GSM pic so that A5 is on alarm pin to make it consistent with out modules.
 */
void check_app_fault()
{
    if (status.wdt_timedout)
        flash_erase_row(kNumFlashRows-1);
}