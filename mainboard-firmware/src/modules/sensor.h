//Configure sensor: interrupt for wakeup, I2C, timeout
void configure_sensor();

//I2C Master Read
//char I2C_READ(unsigned char slave_address, unsigned char n_bytes);

//space to put the sampled data
//extern volatile unsigned long SENSOR_BUF[5];

extern volatile unsigned char SENSOR_FLAG;
extern volatile unsigned char SENSOR_TIMEOUT_FLAG;
extern volatile unsigned long pulse_counts;
