#include <stdio.h>
#include <stdlib.h>
#include <pic12lf1822.h>

int main() {
//wakeup from interrupt
  configure_I2C();
  configure_interrupts();
  configure_IO();
  while(1) {
    go_to_sleep();
    disable_interrupts();
    while(!did_counter_change());   //if interrupt routine has stopped incrementing
    //pulse_count, exit
    wake_master(); // send interrupt to master PIC to start sampling over I2C
    enable_I2C(); //starts responding to master as I2C slave
    enable_interrupts();
  }
}

interrupt_function {
     pulse count++;
}

void configure_I2C() {
  
}

//I2C slave write
void I2C_s_write() {
	unsigned char state = 0, retrycnt = 0, datacnt = 0;
	unsigned int timeout_cnt=0;
	while(timeout_cnt++ < I2C_TIMEOUT) 
	  {
	    switch(state) {
	    case ADDR :
	      if (addr == 0x0A)
		state = ACK;
	    case NACK :
	    case SEND :
	    case SUCCESS :
	    case FAILURE :
	    }
	  }
}

__interrupt(type) void ISR() {
  if (I2C_int) {
    I2C_s_write();
  } else {
    pulse_count++;
  }
}
