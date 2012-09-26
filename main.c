/* 
 * File:   main.c
 * Author: timburke
 *
 * Created on 9 de mayo de 2012, 22:46
 */

#include <stdio.h>
#include <stdlib.h>
#include <p24F16KA101.h>
#include "rtcc.h"
#include "serial.h"

//Configuration Bits
_FBS(BWRP_OFF & BSS_OFF);                                   //No boot sector protection
_FGS(GCP_OFF & GWRP_OFF);                                   
_FOSCSEL(FNOSC_FRC & IESO_OFF);                             //Internal RC 8mhz oscillator with no div and no pll
_FOSC(POSCMOD_NONE & OSCIOFNC_ON & FCKSM_CSDCMD);
_FWDT(FWDTEN_OFF);
_FPOR(MCLRE_ON & I2C1SEL_PRI & PWRTEN_ON & BOREN_BOR1);
_FICD(ICS_PGx1 & COE_OFF & BKBUG_OFF);
_FDS(DSWDTEN_OFF);                                          //Deep sleep watchdog timer disabled
/*
 * 
 */

void blink_light()
{
    _LATA0 = !_LATA0;
}

int main(void) {
    unsigned short i,j;
    uart_parameters params;

    _ODA0 = 0; //Enable open drain
    _TRISA0 = 0; //Configure as output
    _LATA0 = 1; //Set high initially

    configure_interrupts();
    //configure_rtcc();
    //enable_rtcc();
    //set_recurring_task(EverySecond, blink_light);

    params.baud = 57600;
    params.hw_flowcontrol = 0;
    params.parity = NoParity;

    configure_uart(&params);

    register_command_handlers(); //register the serial commands that we respond to.

    //Extend the welcome mat
    sends("Device reset complete.\n");
    sends("PIC 24f16ka101> ");

    while(1)
        ;

    return (EXIT_SUCCESS);
}
