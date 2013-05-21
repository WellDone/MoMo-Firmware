#include <htc.h>
#include <pic12f1822.h>
#include "main.h"
#include "delay.h"
#include "pksa.h"
#include "flash_routines.h"


__CONFIG ( FOSC_INTOSC & WDTE_OFF & PLLEN_ON & MCLRE_OFF &  WRT_OFF) ;

unsigned char flash_buffer[16];

unsigned char pksa_wd_address;
unsigned char pksa_index;
unsigned char pksa_status;
unsigned int counter = 0;

ADDRESS flash_addr_pointer;


//*****************************************************************************
//  This project must be compiled with :
//  Optimization settings : SPEED must be set
//  ROM Ranges = 0 - 1FF
//  Additional command line :  -L-pstrings=CODE
//  *All values here are in hex.
//*****************************************************************************



// The bootloader code does not use any interrupts.
// However, the downloaded code may use interrupts.
// The interrupt vector on a PIC16F1937 is located at
// address 0x0004. The following function will be located
// at the interrupt vector and will contain a jump to
// 0x0204
void interrupt serrvice_isr()
{
    #asm
        GOTO    0x204;
    #endasm
}


void main()
{
    Initialize();
    I2C_Slave_Init();

    counter = 0;


    // If button is pressed, then force bootloader mode
    if (!BUTT)
    {
        LED_1 = 1;
        while(!BUTT);
        LED_1 = 0;
        goto App;
    }

    // if we have any application loaded, jump to it
    if (  flash_memory_read (0x1FFF)  == 0x3455)
    {
        #asm
            goto 0x200;
        #endasm
    }

App:

    // main program loop
    while (1)
    {
        do_i2c_tasks();
        BlinkLED();
    }
}

void BlinkLED()
{
    static unsigned int counter = 0;
    counter++;
    if (counter > 0xFF00)
        LED_4 =1;
    else
        LED_4 =0;

    // dummy led test
    if (!BUTT)
    {
        while(!BUTT);

        LED_1 = 1;
        //LED_2 = 1;
        //LED_3 = 1;
        LED_4 = 1;
        DelayMs(32);
        LED_1 = 0;
        //LED_2 = 0;
        //LED_3 = 0;
        LED_4 = 0;
    }
}

void Initialize ()
{
    /* Software 4x PPL enabled, 16 MHz HF Internal Oscillator. */
    OSCCON = 0xFA;

    /* Set all PORTA pins to be digital I/O (instead of analog input). */
    ANSELA = 0;

    /* Set all PORTA pins to be input (instead of output). */
    TRISA = 0xff;
    /* Set PORTA pin 2 to be input (tri-state). */
    TRISA2 = 1;
    /* Set PORTA pin 4 to be output. */
    TRISA4 = 0;
    /* Set PORTA pin 5 to be output. */
    TRISA5 = 0;
}
