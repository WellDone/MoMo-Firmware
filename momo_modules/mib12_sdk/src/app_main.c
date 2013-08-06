//app_main.c

#include "platform.h"
#include "watchdog.h"

void task(void)
{
	TRISA5 = 0;
	volatile unsigned char i,j;
	while(1)
	{
		for (i=0; i<255; ++i)
		{
			for (j=0; j<255;++j)
				;

			clearwdt();
		}
		
		RA5 = !RA5;
	}
}

void interrupt_handler(void)
{

}

void initialize(void)
{
	
}

void main()
{
	
}