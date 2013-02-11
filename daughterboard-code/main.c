#include <pic12lf1822.h>

int main() {
//wakeup from interrupt
disable_interrupts();
wake_master(); // send interrupt to master PIC to start sampling over I2C
enable_counter_external(); //enable timer module to count external clock pulses
while(!did_counter_change());   //if interrupt routine has stopped incrementing
                                //pulse_count, exit
enable_I2C(); //starts responding to master as I2C slave

}

interrupt_function {
     pulse count++;
}
