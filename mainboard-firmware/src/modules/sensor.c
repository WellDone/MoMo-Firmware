#include <stdio.h>
#include <p24F16KA101.h>
#include "rtcc.h"
#include "sensor.h"
#include "serial_commands.h"
#include "utilities.h"
#include "../momo/sensor_event_log.h"

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

/**********************************************************************
                             CONFIG
 **********************************************************************/

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
  sensor_type sens_type = momo_pulse_counter;
  rtcc_datetime cur_time;
  rtcc_get_time(&cur_time);
  _T3IF = 0; //clear timer interrupt flag
  T2CONbits.TON = 0; //disable timer
  _T2IE = 0;
  _T3IE = 0;

  log_sensor_event(sens_type, &cur_time, pulse_counts);
  pulse_counts = 0;
  SENSOR_TIMEOUT_FLAG = 1;
}
