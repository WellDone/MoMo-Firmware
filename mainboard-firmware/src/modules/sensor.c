#include "sensor.h"
#include "common.h"
#include "utilities.h"
#include "sensor_event_log.h"

volatile static unsigned char SENSOR_FLAG;
volatile static unsigned char SENSOR_TIMEOUT_FLAG;
volatile static unsigned long pulse_counts;

#define SENSOR_TIMER_ON T2CONbits.TON
#define SENSOR_TO 0x09896800 //should be 5 seconds

typedef struct {
  unsigned long value;
  rtcc_datetime timestamp;
} saved_sensor_event;

#define EVENT_BUF_SIZE 1
saved_sensor_event sensor_event_buf_data[EVENT_BUF_SIZE];
ringbuffer sensor_event_buf;

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
    ringbuffer_create( &sensor_event_buf, sensor_event_buf_data, sizeof(saved_sensor_event), EVENT_BUF_SIZE );
}

void save_event()
{

  saved_sensor_event event;
  ringbuffer_pop( &sensor_event_buf, &event );

  log_sensor_event(momo_pulse_counter, &event.timestamp, event.value);
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

void queue_save_event()
{
  saved_sensor_event event;
  event.value = pulse_counts;
  rtcc_get_time(&event.timestamp);
  ringbuffer_push( &sensor_event_buf, &event );
  taskloop_add( save_event );
}

//timeout interrupt flag
void __attribute__((interrupt,no_auto_psv)) _T3Interrupt() {
  _T3IF = 0; //clear timer interrupt flag
  T2CONbits.TON = 0; //disable timer
  _T2IE = 0;
  _T3IE = 0;

  queue_save_event();
  pulse_counts = 0;
  SENSOR_TIMEOUT_FLAG = 1;
}
