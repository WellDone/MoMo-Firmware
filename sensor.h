
//start based on interrupt
void wake_int();

//sample sensor. Returns after pulling data from sensor and storing it into
//buffer SENS_BUF
void sample_sensor();

//Configure interrupts for sensor wakeup
void configure_wakeup_interrupt();

//space to put sampled data
extern unsigned int sensor_buf[5];
