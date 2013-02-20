#include <stdio.h>
#include <stdlib.h>
#include <pic12lf1822.h>
//#include <pic12lf1822.inc>
#include "main.h"

volatile unsigned long I2C_or_pulse;
volatile unsigned char I2C_M_FLAG;
volatile unsigned char wdt_to;
volatile unsigned long pulse_count;
/*==============================================================================
  CONFIGURATION FUNCTIONS
  ============================================================================== */

void configure_I2C() {
  SSP1CON1bits.SSPM1 = 0xE; //I2C Slave with 7 bit addr, Start and Stop Intrpt en
  SSP1CON2bits.SEN = 0; //disable clock stretching
}

void configure_external_interrupts(regflag mode) {
  if (mode == on)
    INTCONbits.INTE = 1; //enable external interrupts
  else
    INTCONbits.INTE = 0;

}

void configure_i2c_interrupts(regflag mode) {
  if (mode == on) {
    INTCONbits.PEIE = 1; //enable external interrupts
    PIE1bits.SSP1IE = 1;
  }
  else {
    INTCONbits.PEIE = 0; 
    PIE1bits.SSP1IE = 0;
  }

}

void configure_wdt(regflag mode) {
  if (mode == on) {
    WDTCONbits.WDTPS = 9; //set WDTCON for 8 sec
    WDTCONbits.SWDTEN = 1; //turn on WDT
  }
  else
    WDTCONbits.SWDTEN = 0; //turn off WT
}

void configure_IO() {

}
/*==============================================================================
  END CONFIGURATION FUNCTIONS
  ============================================================================== */

/*==============================================================================
  I2C COMMUNICATION
  ============================================================================== */

//I2C slave write
void I2C_s_write() {
  unsigned char retrycnt = 0, datacnt = 0, i2c_addr;
  unsigned int timeout_cnt=0, byte_ct=0;
    typedef enum {ADDR, ACK} state_enum;
    state_enum state = ADDR;
  while(timeout_cnt++ < I2C_TIMEOUT) 
    {
      switch(state) {
      case ADDR :
	i2c_addr = (SSP1BUF << 1);

	if (i2c_addr == 0x0A) {
	  I2C_M_FLAG = 0;
	  state = ACK;
	  break;
	}

      case ACK : //Send byte
	SSP1BUF = ((char) (pulse_count >> (8 * byte_ct))); //write data to SSP1BUF LSB first
	byte_ct++; //change which byte to send
	if (byte_ct == 4)
	  break; //if last byte, break
	else {
	  state = ACK;
	  break; //else loop back
	}
	
      }
    }
}

//send IRQ to wake master

void wake_master() {

}

/*==============================================================================
  END I2C COMMUNICATION
  ============================================================================== */


