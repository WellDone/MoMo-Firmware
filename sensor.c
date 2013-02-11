
#include <stdio.h>
#include <stdlib.h>
#include <p24F16KA101.h>
#include "rtcc.h"
#include "sensor.h"
//#include "asm_routines.s"

//start based on interrupt
//void wake_int()


void sample_sensor() {

  int cur_time;
  int end_time;
  int write_data[3];
  //if interrupt, start service routine
  /*write_data[0] = rtc_time(); //first byte is the time sampling started
  while(!I2C_read()); //while accesses fail, keep sampling
  write_data[1] = i2c_data(); //second byte is number of pulses
  write_data[2] = rtc_time(); //last byte is end time
*/
  //return write_data; //return to write to memory
}

  //Write header
  //mem_write(cur_time);
 // mem_write(DATA_TRANSFER);
  //record interval
//  cur_time = RTC_cur_time();

  /*while (is_active()) {
    wait_5sec();
  }*/
  
  //return to main loop
  
 // end_time = RTC_cur_time();
  
  //mem_write(cur_time - end_time);
  //mem_write(get_mid_flag());
  //mem_write(get_high_flag());


void configure_wakeup_interrupt() {
    _INT2EP = 1; //set INT2 for negedge detect
    _INT2IE = 1; //Set INT2 to enable
}

void goto_sleep() {
  asm_sleep();
}

//store data
void store_data() {
  //Get date
  
  //Get data
 
}

//gather data

int gather_data() {
  
}

//wait 5 seconds for another posedge
int wait_5sec() {

}
  
//See if middle conductors are conducting
void is_mid() {

}

//See if high conductors are conducting
void is_high() {

}

//See if on or off
void is_active() {

}
