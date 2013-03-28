#include <stdio.h>
#include <p24F16KA101.h>
#include "rtcc.h"
#include "sensor.h"
#include "serial_commands.h"
#include "utilities.h"

volatile unsigned char SENSOR_FLAG;
volatile unsigned char SENSOR_TIMEOUT_FLAG;
volatile unsigned long pulse_counts;

#define SENSOR_TIMER_ON T2CONbits.TON
#define DEFAULT_SENSOR_TIMER_CT 0x00A06800 //should be 5 seconds
//static unsigned char SENSOR_BUF[5];

//start based on interrupt
/**********************************************************************
                               MAIN FUNCTIONS
 **********************************************************************/

//sample sensor: increment when there's an interrupt
void sample_sensor() {
  int i;
  _INT2IE = 1; //Set INT2 to enable
  i = 0;
  pulse_counts = 0;
  PR3 = 0x050;
  PR2 = 0x6800; //reset timer
  SENSOR_TIMER_ON = 1; 
//  sends(U2, "dc Timer On\r\n");
  while(!SENSOR_TIMEOUT_FLAG) { 
    if (i % 10 == 0) {
      i = 0;
    }
    i++;
    if(SENSOR_FLAG) {
      SENSOR_FLAG = 0; //clear interrupt flag
      pulse_counts++;
    } 
  }
 // sends(U2, "dc Timer On\r\n");
  SENSOR_TIMEOUT_FLAG = 0; 
  SENSOR_TIMER_ON = 0;
  _INT2IE = 0; //Set INT2 to disable
  
}
/**********************************************************************
                             CONFIG
 **********************************************************************/

void configure_sensor() {
    _INT2EP = 0; //set INT2 for posedge detect
    _T2IE = 1; //enable Timer 2 interrupts
    _T3IE = 1; //enable Timer 3 interrupts
    T2CONbits.TCKPS = 0;
    T3CONbits.TCKPS = 0;
    T2CONbits.T32 = 1; //set T2 and T3 to be one 32 bit counter
}


void goto_sleep() {
  asm_sleep();
}
/**********************************************************************
                                ISRs
 **********************************************************************/
//start based on interrupt
void __attribute__((interrupt,no_auto_psv)) _INT2Interrupt() {
//  sends(U2, "dc interrupt detected\r\n");
  _INT2IF = 0; //clear INT2 interrupt flag
  TMR3 = 0x989;
  TMR2 = 0x6800; //reset timer
  SENSOR_FLAG = 1; //set high when interrupt detected
 // pulse_counts++;
}

//timeout interrupt flag
void __attribute__((interrupt,no_auto_psv)) _T3Interrupt() {
 // sends(U2, "dc timeout detected\r\n");
  _T3IF = 0; //clear timer interrupt flag
  PR3 = 0x989;
  PR2 = 0x6800; //reset timer
  SENSOR_TIMEOUT_FLAG = 1;
}
