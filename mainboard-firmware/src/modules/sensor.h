#ifndef __sensor_h__
#define __sensor_h__

void configure_sensor();
extern volatile unsigned char SENSOR_FLAG;
extern volatile unsigned char SENSOR_TIMEOUT_FLAG;
extern volatile unsigned long pulse_counts;

#define SENSOR_TIMER_ON T2CONbits.TON
#define SENSOR_TO 0x00506800 //should be 5 seconds

#endif
