#include <pic12f1822.h>
#include "i2c.h"
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
void interrupt serrvice_isr() {
    #asm
        GOTO 0x204;
    #endasm
}

void main() {
    initialize();
    i2c_slave_init();

    // If button is pressed, then force bootloader mode
    if (BUTT) {
        goto Bootloader;
    }

    // if we have any application loaded, jump to it
    if (flash_memory_read (0x1FFF) == 0x3455) {
        #asm
            GOTO 0x200;
        #endasm
    }

Bootloader:
    while (1) {
        do_i2c_tasks();
        heartbeat();
    }
}

/* Blink the LED every 1 second. */ 
void heartbeat() {
    /* Increment a counter everytime Timer 1 overflows. */
    static volatile unsigned int counter = 0;
    if (TMR1IF) {
        counter++;
        TMR1IF = 0;
    }

    /* Timer 1 is configured to be (32MHz/4)/8 = 1MHz
        16-bit timer overflows 1MHz/(2^16) = ~15 times/second */
    if (counter > 7) {
        if (counter == 15) 
            counter = 0;
        LED = 1;
    } else {
        LED = 0;
    }
}

void initialize ()
{
    /* Software 4x PPL enabled, 16 MHz HF Internal Oscillator. */
    OSCCON = 0xFA;
    /* Set all PORTA pins to be digital I/O (instead of analog input). */
    ANSELA = 0;
    /* Set all PORTA pins to be input. */
    TRISA = 0xff;
    /* Set PORTA pin 4 to be input (tri-state). */
    TRISA4 = 1;
    /* Set PORTA pin 5 to be output. */
    TRISA5 = 0;
    /* Turn Timer1 on with 1:8 prescale using FOSC/4 as source. */
    T1CON = 0x31;
}
