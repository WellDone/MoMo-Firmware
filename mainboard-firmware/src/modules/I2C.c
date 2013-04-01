#include "common.h"
#include "uart.h"

#define I2C1_RETRY_MAX 5
#define I2C_TIMEOUT 10000 //test this, may not need timeout

volatile unsigned char I2C1_RX_BUF[16];
volatile unsigned char I2C1_TX_BUF[16];

volatile char I2C1_NACK_F;	//if no ack, be 1
volatile char I2C1_MASTER_F;	//if occured i2c interrupt as master, 1

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
			sends(U2, "Start bit");
			break;

		//Send Slave Address with a read indication
		case 1:
			if(I2C1_MASTER_F == 1)
			{
				I2C1_MASTER_F = 0;
				I2C1TRN = (((slave_address & 0x7F) << 1 ) | 0x01);
				state++;
				sends(U2, "Send addr");
			}
			break;

		//Wait for ACK
		case 2:
			if(I2C1_MASTER_F == 1)
			{
			  sends(U2, "Slave ACK");
				I2C1_MASTER_F = 0;
				if(I2C1STATbits.ACKSTAT == 1)
				{
				  sends(U2, "NACK found");
					I2C1_NACK_F = 1;
					if(retrycnt < I2C1_RETRY_MAX)state = 7;
					else state = 9;
				}
				else
				{
				  sends(U2, "ACK found");
					I2C1_NACK_F = 0;
					retrycnt = 0;
					state++;
				}
			}
			break;

		//Receive Enable
		case 3:
		  sends(U2, "receive enable");
			I2C1CONbits.RCEN = 1;
			state++;
			break;

		//Receive Data1
		case 4:
			if(I2C1_MASTER_F == 1)
			{
			  sends(U2, "receive Data1");
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
			  sends(U2, "receive enable");
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
			  sends(U2, "receive Data2");
				I2C1_MASTER_F = 0;
				I2C1_RX_BUF[datacnt] = I2C1RCV;
				I2C1CONbits.ACKEN = 1;
				I2C1CONbits.ACKDT = 1;
				state = 11;
			}
			break;

		//Stop to Retry
		case 7:
		  sends(U2, "stop and retry");
			I2C1CONbits.PEN = 1; //STOP I2C
			state++;
			break;

		//Retry
		case 8:
			if(I2C1_MASTER_F == 1)
			{
			  sends(U2, "retry");
				I2C1_MASTER_F = 0;
				retrycnt++;
				state = 0;
			}
			break;

		//Error EXIT
		case 9:
		  sends(U2, "error exit");
			I2C1CONbits.PEN = 1; //STOP I2C
			state++;
			break;

		//Error EXIT
		case 10:
			if(I2C1_MASTER_F == 1)
			{
			  sends(U2, "error exit");
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
