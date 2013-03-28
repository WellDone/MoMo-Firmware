#include <stdio.h>
#include <p24F16KA101.h>
#include "rtcc.h"
#include "sensor.h"
#include "serial_commands.h"
#include "utilities.h"
#include "report.h"

volatile unsigned char SENSOR_FLAG;
volatile unsigned char SENSOR_TIMEOUT_FLAG;
volatile unsigned long pulse_counts;

#define SENSOR_TIMER_ON T2CONbits.TON
#define SENSOR_TO 0x09896800 //should be 5 seconds
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

/*void configure_sensor() {
    _INT2EP = 0; //set INT2 for posedge detect
    _T1IE = 0; //enable interrupts to begin
    _INT2IE = 1;
    _TON = 0; //disable interrupts to begin
    _TCKPS = 3;
    PR1 = SENSOR_TO;
   // _TCS = 1;
  //  _TSYNC = 0;
    pulse_counts = 0;
}
*/
void configure_sensor() {
    _INT2EP = 0; //set INT2 for posedge detect
    _T3IE = 0; //enable interrupts to begin
    _INT2IE = 1;
    T2CONbits.TON = 0; //disable interrupts to begin
    PR2 = SENSOR_TO & 0xFFFF;
    PR3 = (SENSOR_TO >> 16) & 0xFFFF;
    _T32 = 1;
    T2CONbits.TCKPS0 = 1;
    //T2CONbits.TCS = 1;
    //T2CONbits.TSYNC = 0;

    pulse_counts = 0;
}
void goto_sleep() {
  asm_sleep();
}
/**********************************************************************
                                ISRs
 **********************************************************************/
//start based on interrupt
void __attribute__((interrupt,no_auto_psv)) _INT2Interrupt() {
  _INT2IF = 0; //clear INT2 interrupt flag
  T2CONbits.TON = 0; //disable timer for reset
  TMR2 = 0; //reset timer
  TMR3 = 0;
  T2CONbits.TON = 1; //re-enable timer
  _T2IE = 1;
  _T3IE = 1;
  SENSOR_FLAG = 1; //set high when interrupt detected
  pulse_counts++;
}

//timeout interrupt flag
void __attribute__((interrupt,no_auto_psv)) _T3Interrupt() {
  _T3IF = 0; //clear timer interrupt flag
  T2CONbits.TON = 0; //disable timer
  _T2IE = 0;
  _T3IE = 0;
  pulse_counts = 0;
//  taskloop_add();
  SENSOR_TIMEOUT_FLAG = 1;
}
