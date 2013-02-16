
#include <stdio.h>
#include <stdlib.h>
#include <p24F16KA101.h>
#include "rtcc.h"
#include "sensor.h"
#define I2C1_RETRY_MAX 5
#define I2C_TIMEOUT 10000 //test this, may not need timeout

//#include "asm_routines.s"
volatile unsigned char I2C1_RX_BUF[16];
volatile unsigned char I2C1_TX_BUF[16];
volatile unsigned char I2C1_RTC_RX_BUF[20];
volatile unsigned char I2C1_RTC_TX_BUF[20];
volatile unsigned char I2C2_RX_BUF[4];
volatile unsigned char I2C2_TX_BUF[4];

volatile char I2C1_NACK_F;	//if no ack, be 1
volatile char I2C1_MASTER_F;	//if occured i2c interrupt as master, 1

volatile char I2C2_NACK_F;	//if no ack, be 1
volatile char I2C2_MASTER_F;	//if occured i2c interrupt as master, 1
//start based on interrupt
//space to put sampled data
volatile unsigned long sensor_buf[5];

void sample_sensor() {

  rtcc_time temp_rtc_time;
  
  /*FIRST PART OF PACKET
    ======================================== */
  //if interrupt, start service routine
  rtcc_get_time(&temp_rtc_time);
  sensor_buf[0] = ((long )temp_rtc_time.seconds) | 
    ((long)temp_rtc_time.minutes << 8) | 
    ((long) temp_rtc_time.hours << 16); //first byte is the time sampling started
  sensor_buf[1] = ((long)temp_rtc_time.day) | 
    ((long)temp_rtc_time.weekday << 8) | 
    ((long)temp_rtc_time.month << 16) | ((long)temp_rtc_time.year << 24);

  /*SECOND PART OF PACKET
    ======================================== */
  while(!I2C_READ(0x0A, 4)); //while accesses fail, keep sampling
  sensor_buf[2] = I2C1_RX_BUF[0] | ((unsigned long) I2C1_RX_BUF[1] << 8) |
    ((unsigned long) I2C1_RX_BUF[2] << 16  | ((unsigned long) I2C1_RX_BUF[3] << 24))    ; //second byte is number of pulses

  /*THIRD PART OF PACKET
    ======================================== */

  rtcc_get_time(&temp_rtc_time);
  sensor_buf[3] = ((long )temp_rtc_time.seconds) | 
    ((long)temp_rtc_time.minutes << 8) | 
    ((long) temp_rtc_time.hours << 16); //first byte is the time sampling started
  sensor_buf[4] = ((long)temp_rtc_time.day) | 
    ((long)temp_rtc_time.weekday << 8) | 
    ((long)temp_rtc_time.month << 16) | ((long)temp_rtc_time.year << 24);

}

void configure_wakeup_interrupt() {
    _INT2EP = 1; //set INT2 for negedge detect
    _INT2IE = 1; //Set INT2 to enable
}

void goto_sleep() {
  asm_sleep();
}

//store data
void store_data() {
  //Get date
  
  //Get data
 
}

//gather data

int gather_data() {
  
}

//wait 5 seconds for another posedge
int wait_5sec() {

}
  
//See if middle conductors are conducting
void is_mid() {

}

//See if high conductors are conducting
void is_high() {

}

//See if on or off
void is_active() {

}
//I2C Master Interrupt Service Routine
void __attribute__((interrupt, no_auto_psv)) _MI2C1Interrupt(void)
{
  I2C1_MASTER_F = 1;
  IFS1bits.MI2C1IF = 0;		//Clear the I2C1 as Master Interrupt Flag;
}

//
char I2C_READ(unsigned char slave_address, unsigned char n_bytes)
{
	unsigned char state = 0, retrycnt = 0, datacnt = 0;
	unsigned int timeout_cnt=0;

	while(timeout_cnt++ < I2C_TIMEOUT)
	{
		switch(state)
		{
		//Start I2C
		case 0:
			I2C1CONbits.SEN = 1;
			state++;
			break;

		//Send Slave Address with a read indication
		case 1:
			if(I2C1_MASTER_F == 1)
			{
				I2C1_MASTER_F = 0;
				I2C1TRN = (((slave_address & 0x7F) << 1 ) | 0x01);
				state++;
			}
			break;

		//Wait for ACK
		case 2:
			if(I2C1_MASTER_F == 1)
			{
				I2C1_MASTER_F = 0;
				if(I2C1STATbits.ACKSTAT == 1)
				{
					I2C1_NACK_F = 1;
					if(retrycnt < I2C1_RETRY_MAX)state = 7;
					else state = 9;
				}
				else
				{
					I2C1_NACK_F = 0;
					retrycnt = 0;
					state++;
				}
			}
			break;

		//Receive Enable
		case 3:
			I2C1CONbits.RCEN = 1;
			state++;
			break;

		//Receive Data1
		case 4:
			if(I2C1_MASTER_F == 1)
			{
				I2C1_MASTER_F = 0;
				I2C1_RX_BUF[datacnt] = I2C1RCV;
				I2C1CONbits.ACKEN = 1;
				I2C1CONbits.ACKDT = 0;
				datacnt++;
				state++;
			}
			break;
		
		//Receive Enable
		case 5:
			if(I2C1_MASTER_F == 1)
			{
				I2C1_MASTER_F = 0;
				I2C1CONbits.RCEN = 1;
				if(datacnt < n_bytes - 1)state = 4;
				else if(datacnt >= n_bytes - 1)state = 6;
			}	
			break; 
			
		//Receive Data2
		case 6:
			if(I2C1_MASTER_F == 1)
			{
				I2C1_MASTER_F = 0;
				I2C1_RX_BUF[datacnt] = I2C1RCV;
				I2C1CONbits.ACKEN = 1;
				I2C1CONbits.ACKDT = 1;
				state = 11;
			}
			break;

		//Stop to Retry
		case 7:
			I2C1CONbits.PEN = 1; //STOP I2C
			state++;
			break;

		//Retry
		case 8:
			if(I2C1_MASTER_F == 1)
			{
				I2C1_MASTER_F = 0;
				retrycnt++;
				state = 0;
			}
			break;

		//Error EXIT
		case 9:
			I2C1CONbits.PEN = 1; //STOP I2C
			state++;
			break;
		
		//Error EXIT
		case 10:
			if(I2C1_MASTER_F == 1)
			{
				I2C1_MASTER_F = 0;
				return 0;
			}
			break;

		//Normal EXIT
		case 11:
			if(I2C1_MASTER_F == 1)
			{
				I2C1_MASTER_F = 0;
				I2C1CONbits.PEN = 1; //STOP I2C
				state++;
			}
			break;

		//Normal EXIT
		case 12:
			if(I2C1_MASTER_F == 1)
			{
				I2C1_MASTER_F = 0;
				return 1;
			}
			break;
		}
	}	

	return 0;
}
