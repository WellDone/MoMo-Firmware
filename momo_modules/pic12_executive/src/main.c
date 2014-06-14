#include <xc.h>
#include "bus_master.h"
#include "bootloader.h"
#include "appcode.h"
#include "watchdog.h"
#include "mib_definitions.h"
#include "i2c_defines.h"
#include "port.h"
#include "ioc.h"

#define _XTAL_FREQ          4000000

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
    //Check if an alarm occurred and reset if so.
    #ifndef __PIC16LF1847__  
    if(ioc_flag(ALARMIOC))
    {
        //software debounce the pin to add noise immunity
        __delay_us(10);
        if (PIN(ALARM) == 0)
            exec_reset();
    }
    #endif

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

    /* Check if we reset in a dirty fashion, if so trap and wait for debugging
     * Do this after reregistering above so that we can be sure that we have a 
     * valid address for debugging purposes.
     */
    if (status.dirty_reset)
        trap(0);

    /*
     * set the poison reset bit so that we know if the module has reset for a 
     * reason other than by calling the reset API function
     */
    status.dirty_reset = 1;
    
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

    /* Set all PORT pins to be input. */
    TRISA = 0xff;

    #ifdef __PIC16LF1823__
    TRISC = 0xff;
    #endif

    #ifdef __PIC16LF1847__
    TRISB = 0xff;
    #endif

    /* Disable all analog functions */
    ANSELA = 0;
    
    #ifdef __PIC16LF1823__
    ANSELC = 0;
    #endif

    #ifdef __PIC16LF1847__
    ANSELB = 0;
    #endif

    /* Make sure that the alarm pin is configured to notify us if the alarm line is ever pulled low */
    #ifndef __PIC16LF1847__
    ioc_detect_falling(ALARMIOC, 1);
    ioc_enable();
    #endif


    /* Set all PORTA pins to be digital I/O (instead of analog input). */
    
    /* Enable interrupts globally. */
    GIE = 1;
    /* Enable peripheral interrupts. */
    PEIE = 1;
}