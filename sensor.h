#define SENS_BUF_SIZE 384

//start based on interrupt
void wake_int();

//sample sensor. Returns after pulling data from sensor and storing it into
//buffer SENS_BUF
void sample_sensor();

//Configure interrupts for sensor wakeup
void configure_wakeup_interrupt();

//extern unsigned int sensor_buf[SENS_BUF_SIZE] __attribute__((space(dma)));
