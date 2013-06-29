#include <pic12f1822.h>
#include "bus_master.h"
#include "flash_memory.h"

/* Use internal oscillator as the frequency oscillator. */
#pragma config FOSC=INTOSC
/* Disable the watchdog timer. */
#pragma config WDTE=OFF
/* Enable 4x phase lock loop. */
#pragma config PLLEN=ON
/* Flash memory write protection off. */
#pragma config WRT=OFF

#define LED RA5
#define BUTT RA4

void heartbeat();
void initialize ();

//*****************************************************************************
//  This project must be compiled with :
//  Optimization settings : SPEED must be set
//  ROM Ranges = 0 - 1FF
//  Additional command line :  -L-pstrings=CODE
//  *All values here are in hex.
//*****************************************************************************

// The bootloader code does not use any interrupts.
// However, the downloaded code may use interrupts.
// The interrupt vector on a PIC12LF1822 is located at
// address 0x0004. The following function will be located
// at the interrupt vector and will contain a jump to
// 0x0204
void interrupt service_isr() {
    // Handle i2c interrupts (MSSP) in the bootloader.
    if (SSP1IF == 1) {
        if (i2c_master_state() == kI2CDisabledState) {
            i2c_slave_interrupt();
        } else {
            i2c_master_interrupt();
        }
        SSP1IF = 0;
    } else {
        /*#asm
            GOTO 0x204;
        #endasm*/
    }
}

void main() {
    initialize();
    bus_init();

    // If button is pressed, then force bootloader mode
    if (BUTT) {
        goto Bootloader;
    }

    // 0x7ff is highest word in flash memory for pic12lf1822
    // if application is loaded, highest byte will be 0x55
    //if (flash_memory_read (0x7FF) == 0x3455) {
        /*#asm
            GOTO 0x200;
        #endasm*/
    //}

Bootloader:
    while (1) {
        heartbeat();
    }
}

/* Blink the LED every 1 second. */ 
void heartbeat() {
    /* Increment a counter everytime Timer 1 overflows. */
    static unsigned int counter = 0;
    if (TMR1IF) {
        counter++;
        TMR1IF = 0;
    }

    /* Timer 1 is configured to be (32MHz/4)/8 = 1MHz
        16-bit timer overflows 1MHz/(2^16) = ~15 times/second */

    bus_master_rpc(NULL, 0x08, 0x10, 0x20, NULL, 0);

    if (counter == 30)
        counter = 0;
}

void initialize ()
{
    /* Software 4x PPL enabled, 32 MHz HF Internal Oscillator. */
    OSCCON = 0xF0;
    /* Set all PORTA pins to be digital I/O (instead of analog input). */
    ANSELA = 0;
    /* Set all PORTA pins to be input. */
    TRISA = 0xff;
    /* Set PORTA pin 5 to be output. */
    TRISA5 = 0;
    /* Turn Timer1 on with 1:8 prescale using FOSC/4 as source. */
    T1CON = 0x31;
    
    /* Enable interrupts globally. */
    GIE = 1;
    /* Enable peripheral interrupts. */
    PEIE = 1;
}
