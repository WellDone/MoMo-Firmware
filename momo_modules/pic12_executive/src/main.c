#include <xc.h>
#include "bus_master.h"
#include "bootloader.h"
#include "appcode.h"
#include "watchdog.h"
#include "mib_definitions.h"
#include "i2c_defines.h"
#include "port.h"

//Configuration Words
#pragma config FOSC=INTOSC      /* Use internal oscillator as the frequency oscillator. */
#pragma config WDTE=SWDTEN  /* Enable the watchdog timer under software control. */
#pragma config PLLEN=OFF        /* Disable 4x phase lock loop. */
#pragma config WRT=OFF          /* Flash memory write protection off. */
#pragma config LVP=OFF
#pragma config MCLRE=OFF

extern __persistent bank1 MIBExecutiveStatus status;

void initialize();
extern void restore_status();

void interrupt service_isr() {
    // Handle i2c interrupts (MSSP) in the bootloader.
    if (SSP1IF == 1) 
    {
        while (SSP1IF == 1)
        {
            SSP1IF = 0; //Do this because with our slow clock we might miss an interrupt
            if (status.slave_active) 
                i2c_slave_interrupt();
            
            //Master i2c is not interrupt driven
        }
    } 
    else if (status.valid_app)
    {
        call_app_interrupt();
        reset_page();
    }
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
    
    if (status.valid_app && PIN(ALARM) == 1)
    {  
        call_app_init();
        reset_page();

        while(1)
        {
            call_app_task();
            reset_page();

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

    #ifdef __PIC16LF1847__
    TRISB = 0xff;
    ANSELB = 0;
    #endif

    /* Make sure that the alarm pin is an input so that we can disable app loading if required */
    PIN_DIR(ALARM, INPUT);

    /* Set all PORTA pins to be digital I/O (instead of analog input). */
    ANSELA = 0;
    
    /* Enable interrupts globally. */
    GIE = 1;
    /* Enable peripheral interrupts. */
    PEIE = 1;
}