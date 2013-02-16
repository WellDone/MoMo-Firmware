
//start based on interrupt
void wake_int();

//sample sensor. Returns after pulling data from sensor and storing it into
//buffer SENS_BUF
void sample_sensor();

//Configure sensor: interrupt for wakeup, I2C, timeout
void configure_sensor();

//I2C Master Read
char I2C_READ(unsigned char slave_address, unsigned char n_bytes);
