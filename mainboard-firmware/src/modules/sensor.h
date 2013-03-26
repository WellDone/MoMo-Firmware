
//start based on interrupt
void wake_int();

//sample sensor. Returns after pulling data from sensor and storing it into
//buffer SENS_BUF
void sample_sensor();

//Configure sensor: interrupt for wakeup, I2C, timeout
void configure_sensor();

//I2C Master Read
char I2C_READ(unsigned char slave_address, unsigned char n_bytes);

//space to put the sampled data
//extern volatile unsigned long SENSOR_BUF[5];

//#include "asm_routines.s"
extern volatile unsigned char I2C1_RX_BUF[16];
extern volatile unsigned char I2C1_TX_BUF[16];

extern volatile char I2C1_NACK_F;	//if no ack, be 1
extern volatile char I2C1_MASTER_F;	//if occured i2c interrupt as master, 1
