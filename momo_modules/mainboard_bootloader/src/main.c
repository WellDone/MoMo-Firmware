// FBS
#pragma config BWRP = OFF               // Table Write Protect Boot (Boot segment may be written)
#pragma config BSS = OFF                // Boot segment Protect (No boot program Flash segment)

// FGS
#pragma config GWRP = OFF               // General Segment Code Flash Write Protection bit (General segment may be written)
#pragma config GCP = OFF                // General Segment Code Flash Code Protection bit (No protection)

// FOSCSEL
#pragma config FNOSC = FRC           	// Oscillator Select (8 MHz FRC oscillator (FRC)) with PLL
#pragma config IESO  = OFF              // Internal External Switch Over bit (Internal External Switchover mode disabled (Two-Speed Start-up disabled))

// FOSC
#pragma config POSCMOD = NONE           // Primary Oscillator Configuration bits (Primary oscillator disabled)
#pragma config OSCIOFNC = OFF            // CLKO Enable Configuration bit (CLKO output signal is active on the OSCO pin)
#pragma config POSCFREQ = HS            // Primary Oscillator Frequency Range Configuration bits (Primary oscillator/external clock input frequency greater than 8 MHz)
#pragma config SOSCSEL = SOSCHP         // SOSC Power Selection Configuration bits (Secondary oscillator configured for high-power operation)
#pragma config FCKSM = CSECMD           // Clock Switching and Monitor Selection (Clock switching is enabled, Fail-Safe Clock Monitor is disabled)

// FWDT
#pragma config WDTPS = PS32768          // Watchdog Timer Postscale Select bits (1:32,768)
#pragma config FWPSA = PR128            // WDT Prescaler (WDT prescaler ratio of 1:128)
#pragma config WINDIS = OFF             // Windowed Watchdog Timer Disable bit (Standard WDT selected; windowed WDT disabled)
#pragma config FWDTEN = OFF             // Watchdog Timer Enable bit (WDT disabled (control is placed on the SWDTEN bit))

// FPOR
#pragma config BOREN = BOR0             // Brown-out Reset Enable bits (Brown-out Reset disabled in hardware; SBOREN bit disabled)
#pragma config PWRTEN = ON              // Power-up Timer Enable bit (PWRT enabled)
#pragma config I2C1SEL = PRI            // Alternate I2C1 Pin Mapping bit (Default location for SCL1/SDA1 pins)
#pragma config BORV = V18               // Brown-out Reset Voltage bits (Brown-out Reset set to lowest voltage (1.8V))
#pragma config MCLRE = ON               // MCLR Pin Enable bit (RA5 input pin disabled; MCLR enabled)

// FICD
#pragma config ICS = PGx1               // ICD Pin Placement Select bits (PGC1/PGD1 are used for programming and debugging the device)

// FDS
#pragma config DSWDTPS = DSWDTPSF       // Deep Sleep Watchdog Timer Postscale Select bits (1:2,147,483,648 (25.7 Days))
#pragma config DSWDTOSC = LPRC          // DSWDT Reference Clock Select bit (DSWDT uses LPRC as reference clock)
#pragma config RTCOSC = LPRC            // RTCC Reference Clock Select bit (RTCC uses LPRC as reference clock)
#pragma config DSBOREN = ON             // Deep Sleep Zero-Power BOR Enable bit (Deep Sleep BOR enabled in Deep Sleep)
#pragma config DSWDTEN = OFF            // Deep Sleep Watchdog Timer Enable bit (DSWDT disabled)

#include "pic24.h"
#include "memory.h"
#include "bootloader.h"
#include "constants.h"

#define kEEPROMPage 0x7F
#define kEEPROMOffset 0xFE00		//valid for pic24f16ka101

enum
{
	kEEPROMEraseWord = 0x4058,
	kEEPROMWriteWord = 0x4004
};

void eeprom_erase(unsigned int offset);
unsigned int eeprom_read(unsigned int offset);

int _BOOTLOADER_CODE main(void)
{
	AD1PCFG = 0xFFFF;
	ALARMTRIS = 1;

	//Allow 100 ms for someone to assert the alarm pin in case this is a
	//power on reset.
	DELAY_MS(100);

	//If alarm pin is low for 400 ms, load backup firmware
	if (ALARMPIN == 0)
	{
		DELAY_MS(400);
		if (ALARMPIN == 0)
		{
			//Reflash ourselves and pull alarm pin low during the operation so
			//people can know when we're done.
			ALARMPIN = 0;
			ALARMTRIS= 0;
			program_application(kBackupFirmwareSector);
			ALARMTRIS = 1;
		}
	}
	else if (eeprom_read(kBootloaderMagicEEPROMLoc) == 0xAA)
	{
		//If we are told to reflash, do so.
		ALARMPIN = 0;
		ALARMTRIS= 0;
		program_application(kMainFirmwareSector);
		eeprom_erase(kBootloaderMagicEEPROMLoc);
		ALARMTRIS = 1;
	}

	//TODO: this only works if I jump directly to the start of the code, rather than the
	//goto instruction stored at 0x100.  It is unclear why but it also happens when I 
	//directly program the chip to jump to 0x100 on reset with a goto instruction at 0x100
	//that points to 0x200
	if (valid_instruction(0x100))
		goto_address(0x200);

	ALARMPIN = 0;
	while(true)
	{
		ALARMTRIS= 0;
		DELAY_MS(250);
		ALARMTRIS=1;
		DELAY_MS(250);
	}

	return 0;
}

void _BOOTLOADER_CODE eeprom_erase(unsigned int offset)
{
	NVMCON = kEEPROMEraseWord;
	TBLPAG = kEEPROMPage;

	offset |= kEEPROMOffset;
	__builtin_tblwtl(offset, 0);
	asm volatile ("disi #5");
	__builtin_write_NVM();

	while(_WR)
		;
}

unsigned int _BOOTLOADER_CODE eeprom_read(unsigned int offset)
{
	TBLPAG = kEEPROMPage;
	offset |= kEEPROMOffset;

	return __builtin_tblrdl(offset);
}