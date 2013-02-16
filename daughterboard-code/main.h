typedef enum {
}
//PIC constants
#define I2C_TIMEOUT 10000

//Configure I2C for slave
void configure_I2C();

//configure interrupts for INT initially, then I2C interrupts
void configure_interrupts();

//configure IO for I2C
void configure_IO();

//Put PIC in sleep
void go_to_sleep();

//disable I2C interrupts for sampling
void disable_interrupts();

//send IRQ to master to wake up
void wake_master();

//flag indicating interrupt type
extern static volatile unsigned long I2C_or_pulse;
