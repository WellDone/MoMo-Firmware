#include <stdio.h>
#include <stdlib.h>
#include <p24F16KA101.h>
#include "rtcc.h"
#include "sensor.h"

//start based on interrupt
//void wake_int()
void __attribute__((__interrupt__, no_auto_psv)) _T3Interrupt(void) {

  int* cur_time;
  //if interrupt, start service routine
  
  //Write header
  mem_write(cur_time);
  mem_write(DATA_TRANSFER);
  //record interval
  cur_time = RTC_cur_time();

  while (active()) {
    wait_5sec();
  }
  
  //return to main loop
  
  end_time = RTC_cur_time();
  
  mem_write(cur_time - end_time);
  mem_write(get_mid_flag());
  mem_write(get_high_flag());
  return;
}

void configure_wakeup_interrupt() {

}

void goto_sleep() {
  asm sleep;
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
