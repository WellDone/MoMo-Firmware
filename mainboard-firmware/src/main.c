/*
 * File:   main.c
 * Author: timburke
 *
 * Created on 9 de mayo de 2012, 22:46
 */

#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "rtcc.h"
#include "uart.h"
#include "xc.h"
#include "memory.h"
#include "sensor.h"
#include "oscillator.h"
#include "reset_manager.h"
#include "task_manager.h"
#include "serial_commands.h"
#include "sensor_event_log.h"

// FBS
#pragma config BWRP = OFF               // Table Write Protect Boot (Boot segment may be written)
#pragma config BSS = OFF                // Boot segment Protect (No boot program Flash segment)

// FGS
#pragma config GWRP = OFF               // General Segment Code Flash Write Protection bit (General segment may be written)
#pragma config GCP = OFF                // General Segment Code Flash Code Protection bit (No protection)

// FOSCSEL
#pragma config FNOSC = FRCPLL           // Oscillator Select (8 MHz FRC oscillator (FRC)) with PLL
#pragma config IESO  = OFF              // Internal External Switch Over bit (Internal External Switchover mode disabled (Two-Speed Start-up disabled))

// FOSC
#pragma config POSCMOD = NONE           // Primary Oscillator Configuration bits (Primary oscillator disabled)
#pragma config OSCIOFNC = ON           // CLKO Enable Configuration bit (CLKO output signal is active on the OSCO pin)
#pragma config POSCFREQ = HS            // Primary Oscillator Frequency Range Configuration bits (Primary oscillator/external clock input frequency greater than 8 MHz)
#pragma config SOSCSEL = SOSCHP         // SOSC Power Selection Configuration bits (Secondary oscillator configured for high-power operation)
#pragma config FCKSM = CSECMD           // Clock Switching and Monitor Selection (Clock switching is enabled, Fail-Safe Clock Monitor is disabled)

// FWDT
#pragma config WDTPS = PS32768          // Watchdog Timer Postscale Select bits (1:32,768)
#pragma config FWPSA = PR128            // WDT Prescaler (WDT prescaler ratio of 1:128)
#pragma config WINDIS = OFF             // Windowed Watchdog Timer Disable bit (Standard WDT selected; windowed WDT disabled)
#pragma config FWDTEN = OFF             // Watchdog Timer Enable bit (WDT disabled (control is placed on the SWDTEN bit))

// FPOR
#pragma config BOREN = BOR0             // Brown-out Reset Enable bits (Brown-out Reset disabled in hardware; SBOREN bit disabled)
#pragma config PWRTEN = ON              // Power-up Timer Enable bit (PWRT enabled)
#pragma config I2C1SEL = PRI            // Alternate I2C1 Pin Mapping bit (Default location for SCL1/SDA1 pins)
#pragma config BORV = V18               // Brown-out Reset Voltage bits (Brown-out Reset set to lowest voltage (1.8V))
#pragma config MCLRE = ON              // MCLR Pin Enable bit (RA5 input pin disabled; MCLR enabled)

// FICD
#pragma config ICS = PGx1               // ICD Pin Placement Select bits (PGC1/PGD1 are used for programming and debugging the device)

// FDS
#pragma config DSWDTPS = DSWDTPSF       // Deep Sleep Watchdog Timer Postscale Select bits (1:2,147,483,648 (25.7 Days))
#pragma config DSWDTOSC = LPRC          // DSWDT Reference Clock Select bit (DSWDT uses LPRC as reference clock)
#pragma config RTCOSC = LPRC            // RTCC Reference Clock Select bit (RTCC uses LPRC as reference clock)
#pragma config DSBOREN = ON             // Deep Sleep Zero-Power BOR Enable bit (Deep Sleep BOR enabled in Deep Sleep)
#pragma config DSWDTEN = OFF            // Deep Sleep Watchdog Timer Enable bit (DSWDT disabled)

volatile unsigned char SENSOR_BUF[5];

int main(void) {
    uart_parameters params_uart1;

    AD1PCFG = 0xFFFF;

    _LATA0 = 0;
    //_LATA1 = 0;


    _TRISA0 = 0;
    //_TRISA1 = 0;
    //_TRISA3 = 1; //WISMO READY PIN

    //Configure pin controlling WISMO
    _LATA3 = 1;
    _ODA3 = 1;
    _TRISA3 = 0;

    //Disable div-by-2
    //CLKDIV = 0;

    handle_reset();
    configure_SPI();
    params_uart1.baud = 115200;
    params_uart1.hw_flowcontrol = 0;
    params_uart1.parity = NoParity;
    configure_uart( U1, &params_uart1 );

    init_sensor_event_log( MEMORY_SUBSECTION_SIZE, MEMORY_SUBSECTION_MASK );

    taskloop_loop();

    return (EXIT_SUCCESS);
}
