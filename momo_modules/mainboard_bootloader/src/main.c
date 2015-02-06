
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
#pragma config OSCIOFCN = ON            // OSCO Pin Configuration (OSCO/CLKO/RC15 functions as port I/O (RC15))
#pragma config FCKSM = CSECMD           // Clock Switching and Fail-Safe Clock Monitor Configuration bits (Clock switching enabled and Fail-Safe Clock Monitor disabled)
#pragma config FNOSC = FRC           	// Initial Oscillator Select (Fast RC Oscillator (FRC))
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

void _BOOTLOADER_CODE flash_alarm(unsigned int length);

int _BOOTLOADER_CODE main(void)
{
	ValidationResult metadata_status;

	//Speed up clock to 8 Mhz (from the default 4 Mhz).
	_RCDIV = 000;

	DIR(ALARM) = INPUT;
	ENSURE_DIGITAL(ALARM);
	

	//Allow 100 ms for someone to assert the alarm pin in case this is a
	//power on reset.
	DELAY_MS(100);

	//If alarm pin is low for 400 ms, load backup firmware
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

			//Reflash ourselves
			program_application(kBackupFirmwareSector);
		}
	}
	else if (reflash == kReflashMagic)
	{
		//If we are told to reflash, do so.
		reflash = 0x0000;
		program_application(kMainFirmwareSector);
	}
	else if (reflash == kRecoverMagic)
	{
		reflash = kAlreadyRecoveredMagic;
		program_application(kBackupFirmwareSector);
	}

	metadata_status = validate_metadata();
	if (metadata_status == kValidMetadata)
		goto_address(kMetadataResetVector);

	/*
	 * If there was no valid firmware loaded, try to recover by loading
	 * the recovery firmware.  Only do it once though so we don't wear out
	 * our flash with continual useful reflashes.
	 */
	if (reflash != kAlreadyRecoveredMagic)
	{
		reflash = kRecoverMagic;
		asm volatile("reset");
	}

	/*
	 * Firmware was invalid or not loaded and the recovery firmware was also
	 * not present or not correct, signal our displeasure.
	 * Users with a logic analyzer or other tool can decipher what went wrong
	 * using the period of the output pulses.  The momo python tools also parse
	 * and interpret it.
	 */
	flash_alarm(metadata_status);

	return 0;
}

void _BOOTLOADER_CODE flash_alarm(unsigned int length)
{
	unsigned int i;
	while(true)
	{
		LAT(ALARM) = 0;
		DIR(ALARM) = ~ DIR(ALARM);
		
		for (i=0; i<length; ++i)
			DELAY_MS(50);
	}
}

/*
 *   Since the bootloader has to access flash using the table read instructions, it can
 *   fall afoul of poorly documented address error issues with certain portions of the 
 *	 flash not being implemented and causing problems.  Trap those errors here so that 
 *	 we can debug them.
 */
void _BOOTLOADER_CODE __attribute__((interrupt,no_auto_psv)) _AddressError()
{
	INTCON1bits.ADDRERR = 0;

	flash_alarm(kAddressError);
}