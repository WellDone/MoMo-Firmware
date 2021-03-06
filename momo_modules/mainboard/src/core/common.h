#ifndef __common_h__
#define __common_h__

#include "pic24.h"
#include "ioport.h"

#define SENSOR_INTERRUPT_BIT    IFS1bits.INT2IF
#define RTC_INTERRUPT_BIT       IFS3bits.RTCIF

//Clock configuration and delays
#define FCY   4000000L  //define your instruction frequency, FCY = FOSC/2
  
#define CYCLES_PER_MS ((unsigned long long)(FCY * 0.001))        //instruction cycles per millisecond
#define CYCLES_PER_US ((unsigned long long)(FCY * 0.000001))   //instruction cycles per microsecond
#define DELAY_MS(ms)  __delay32(CYCLES_PER_MS * ((unsigned long long) ms));   //__delay32 is provided by the compiler, delay some # of milliseconds
#define DELAY_US(us)  __delay32(CYCLES_PER_US * ((unsigned long long) us));    //delay some number of microseconds
extern void __delay32(unsigned long long);

#endif
