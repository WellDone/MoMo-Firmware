// CONFIG4
#pragma config DSWDTPS = DSWDTPS1F      // Deep Sleep Watchdog Timer Postscale Select bits (1:68719476736 (25.7 Days))
#pragma config DSWDTOSC = LPRC          // DSWDT Reference Clock Select (DSWDT uses LPRC as reference clock)
#pragma config DSBOREN = ON             // Deep Sleep BOR Enable bit (DSBOR Enabled)
#pragma config DSWDTEN = OFF            // Deep Sleep Watchdog Timer Enable (DSWDT Disabled)
#pragma config DSSWEN = ON              // DSEN Bit Enable (Deep Sleep is controlled by the register bit DSEN)

// CONFIG3
#pragma config WPFP = WPFP63            // Write Protection Flash Page Segment Boundary (Page 52 (0xFC00))
#pragma config VBTBOR = ON              // VBAT BOR enable bit (VBAT BOR enabled)
#pragma config SOSCSEL = ON             // SOSC Selection bits (SOSC circuit selected)
#pragma config WDTWIN = PS75_0          // Watch Dog Timer Window Width (Watch Dog Timer Window Width is 75 percent)
#pragma config BOREN = ON               // Brown-out Reset Enable (Brown-out Reset Enable)
#pragma config WPDIS = WPDIS            // Segment Write Protection Disable (Disabled)
#pragma config WPCFG = WPCFGDIS         // Write Protect Configuration Page Select (Disabled)
#pragma config WPEND = WPENDMEM         // Segment Write Protection End Page Select (Write Protect from WPFP to the last page of memory)

// CONFIG2
#pragma config POSCMD = NONE            // Primary Oscillator Select (Primary Oscillator Disabled)
#pragma config BOREN1 = EN              // BOR Override bit (BOR Enabled [When BOREN=1])
#pragma config IOL1WAY = OFF            // IOLOCK One-Way Set Enable bit (The IOLOCK bit can be set and cleared using the unlock sequence)
#pragma config OSCIOFNC = ON            // OSCO Pin Configuration (OSCO/CLKO/RC15 functions as port I/O (RC15))
#pragma config FCKSM = CSDCMD           // Clock Switching and Fail-Safe Clock Monitor Configuration bits (Clock switching and Fail-Safe Clock Monitor are disabled)
#pragma config FNOSC = FRC              // Initial Oscillator Select (Fast RC Oscillator (FRC))
#pragma config ALTVREF = DLT_AV_DLT_CV  // Alternate VREF/CVREF Pins Selection bit (Voltage reference input, ADC =RA9/RA10 Comparator =RA9,RA10)
#pragma config IESO = OFF               // Internal External Switchover (Disabled)

// CONFIG1
#pragma config WDTPS = PS32768          // Watchdog Timer Postscaler Select (1:32,768)
#pragma config FWPSA = PR128            // WDT Prescaler Ratio Select (1:128)
#pragma config FWDTEN = WDT_SW          // Watchdog Timer Enable (WDT controlled with the SWDTEN bit)
#pragma config WINDIS = OFF             // Windowed WDT Disable (Standard Watchdog Timer)
#pragma config ICS = PGx1               // Emulator Pin Placement Select bits (Emulator functions are shared with PGEC1/PGED1)
#pragma config LPCFG = OFF              // Low power regulator control (Disabled)
#pragma config GWRP = OFF               // General Segment Write Protect (Disabled)
#pragma config GCP = OFF                // General Segment Code Protect (Code protection is disabled)
#pragma config JTAGEN = OFF             // JTAG Port Enable (Disabled)

#include "pic24.h"
#include "ioport.h"
#include "memory.h"
#include "bootloader.h"
#include "constants.h"

unsigned int _BOOTLOADER_VAR reflash __attribute__((persistent));

int _BOOTLOADER_CODE main(void)
{
	DIR(ALARM) = INPUT;

	//Allow 100 ms for someone to assert the alarm pin in case this is a
	//power on reset.
	DELAY_MS(100);

	//If alarm pin and data pin are low for 400 ms, load backup firmware
	if (PIN(ALARM) == 0)
	{
		DELAY_MS(400);
		if (PIN(ALARM) == 0)
		{
			//Wait for the pin to go high, then pull it low and start the reflash
			//This makes sure that if there is just something that pulls alarm low
			//on boot, we don't start reflashing ourselves.
			while(PIN(ALARM) == 0)
				;

			//Reflash ourselves and pull alarm pin low during the operation so
			//people can know when we're done.
			LAT(ALARM) = 0;
			DIR(ALARM) = OUTPUT;

			DELAY_MS(100);

			program_application(kBackupFirmwareSector);
			DIR(ALARM) = INPUT;
		}
	}
	else if (reflash == kReflashMagic)
	{
		//If we are told to reflash, do so.
		reflash = 0x0000;
		LAT(ALARM) = 0;
		DIR(ALARM) = OUTPUT;
		program_application(kMainFirmwareSector);
		DIR(ALARM) = INPUT;
	}

	//TODO: this only works if I jump directly to the start of the code, rather than the
	//goto instruction stored at 0x100.  It is unclear why but it also happens when I 
	//directly program the chip to jump to 0x100 on reset with a goto instruction at 0x100
	//that points to 0x200
	
	if (valid_instruction(0x100))
		goto_address(0x200);

	while(true)
	{
		LAT(ALARM) = 0;
		DIR(ALARM) = ~ DIR(ALARM);
		DELAY_MS(250)
	}

	return 0;
}