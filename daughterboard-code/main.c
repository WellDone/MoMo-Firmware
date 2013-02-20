
#include <stdio.h>
#include <stdlib.h>
#include <pic12lf1822.h>
#include "main.h"
#include <xc.h>
//#include "asm_routines.s"

//configure watchdog to be software enabled
#pragma config WDTE = 1 
volatile unsigned long pulse_count = 0, prev_pulse_count = 0;
void interrupt ISR() {
  if (1 == PIR1bits.SSP1IF) {
    I2C_M_FLAG = 1; //set I2C flag
    PIR1bits.SSP1IF = 0;
  } else if (INTCONbits.INTF == 1) {
    pulse_count++;
    INTCONbits.INTF = 0; //clear interrupt flag
    asm("CLRWDT");
  } else {
    wdt_to = 1;
  }
}

void main() {
//wakeup from interrupt
  configure_i2c_interrupts(on);
  configure_external_interrupts(on);
  configure_IO();
  SLEEP();
  while(1) {
    //Do initial pulse_count on wakeup and initialize WDT
    pulse_count++;
    configure_wdt(on);
    
    //accumulate data
    if(!wdt_to) 
      continue; //if interrupt is still incrementing, loop back

    /* ============SEND DATA TO MASTER============ */
    //Disable WDT
    configure_wdt(off);
    wdt_to = 0;

    //pulse_count, exit
    configure_i2c_interrupts(on);
    I2C_s_write(); //send pulse_count to master
    wake_master(); // send interrupt to master PIC to start sampling over I2C
    /* ============SEND DATA TO MASTER============ */

    //reset counts
    SLEEP();
    pulse_count = 0;
    prev_pulse_count = 0;

  }
}
