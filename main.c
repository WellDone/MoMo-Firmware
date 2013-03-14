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
#include "xc.h"
#include "memory.h"
#include "oscillator.h"
#include "reset_manager.h"
#include "tasks.h"
#include "serial_commands.h"

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

static unsigned char SENSOR_BUF[5];

int main(void) {
    uart_parameters params_uart1;
    uart_parameters params_uart2;

    AD1PCFG = 0xFFFF;

    //_LATA0 = 0;
    //_LATA1 = 0;


    //_TRISA0 = 0;
    //_TRISA1 = 0;
    //_TRISA3 = 1; //WISMO READY PIN

    //Configure pin controlling WISMO
    //_LATA2 = 1;
    //_ODA2 = 1;
    //_TRISA2 = 0;

    //Disable div-by-2
    //CLKDIV = 0;

    configure_interrupts();
    //configure_SPI();
    //configure_rtcc();
    //enable_rtcc();
    //set_recurring_task(EverySecond, blink_light);
    handle_reset();
    //taskloop_add(process_commands_task);

    params_uart1.baud = 115200;
    params_uart1.hw_flowcontrol = 0;
    params_uart1.parity = NoParity;
    configure_uart( U1, &params_uart1 );

    //init_gsm();

    register_command_handlers(); //register the serial commands that we respond to.

    params_uart2.baud = 38400;
    params_uart2.hw_flowcontrol = 0;
    params_uart2.parity = NoParity;
    configure_uart( U2, &params_uart2 );

    //Extend the welcome mat

    while (1)
    {
        print( "Device reset complete.\r\n");
        print( "PIC 24f16ka101> ");
    }


    taskloop_loop();


    return (EXIT_SUCCESS);
}

void main_loop() {
    enum { SLEEP_MODE, CHECK_INT, SENSING, MEMORY_WRITE, MEMORY_READ, GSM } state;
    unsigned long pulse_count = 0;
    while(1) {
      switch(state) {
      case SLEEP_MODE: //SLEEP mode
	Sleep(); //put pic to sleep
	state = CHECK_INT;
	break;
      case CHECK_INT: //check which interrupt woke it up
	if(SENSOR_INTERRUPT_BIT) {
	  if (pulse_count == 200) //COMMENT THIS SHIT OUT
	    pulse_count = 0;
	  pulse_count++;
	  state = SENSING; //if sensor interrupt, sample sensor
	  SENSOR_INTERRUPT_BIT = 0; //clear interrupt bit
	}
	else if (RTC_INTERRUPT_BIT) {
	  state = GSM; //if RTC interrupt transmit GSM
	  RTC_INTERRUPT_BIT = 0;
	}
	break;
      case SENSING:
	//sample_sensor(); //<(0.0<) Read what it says mofugga
	state = MEMORY_WRITE;
	break;
      case MEMORY_WRITE:
	//	mem_write();
	state = SLEEP_MODE;
	break;
      case MEMORY_READ:
	//mem_read();
	state = GSM;
	break;
      case GSM:
	//GSM send code
	state = SLEEP_MODE;
	break;
      }
      }
}
