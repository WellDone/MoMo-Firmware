#include "common.h"
#include "sensor.h"
#include "memory.h"

int test_WDT(int delay) {
  RCONbits.SWDTEN = 1; //enable Watchdog timer
  //set for 3 seconds
  //go to sleep
  sends("I can't do that Dave");//on wakeup, send display
  return 1;
}

int test_INT2() {
    configure_wakeup_interrupt();
   //go to sleep
    asm_sleep();
    //on wake, print out
    sends("Good Morning Dave");
}

void self_test() {
  mem_test();
}