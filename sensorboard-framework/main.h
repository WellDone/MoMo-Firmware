#include <stdio.h>
#include <stdlib.h>
#include <pic12lf1822.h>
typedef enum {on, off} regflag;

//PIC constants
#define I2C_TIMEOUT 10000

//Configure I2C for slave
void configure_i2c_interrupts(regflag mode);

//configure interrupts for INT initially, then I2C interrupts
void configure_external_interrupts(regflag mode);

//set wdt configurations

void configure_wdt(regflag mode);

//configure IO for I2C
void configure_IO();

//Put PIC in sleep
void asm_sleep();

//disable I2C interrupts for sampling
void disable_interrupts();

//send IRQ to master to wake up
void wake_master();

//I2C slave code
void I2C_s_write();

//flag indicating interrupt type
extern volatile unsigned long I2C_or_pulse;
extern volatile unsigned char I2C_M_FLAG;
extern volatile unsigned char wdt_to;
extern volatile unsigned long pulse_count;
