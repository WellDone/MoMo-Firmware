#include <stdlib.h>
#include "common.h"
#include "reset_manager.h"
#include "task_manager.h"
#include "scheduler.h"
#include "bus.h"
#include "nullcallback.h"

// FBS
#pragma config BWRP = OFF               // Table Write Protect Boot (Boot segment may be written)
#pragma config BSS = OFF                // Boot segment Protect (No boot program Flash segment)

// FGS
#pragma config GWRP = OFF               // General Segment Code Flash Write Protection bit (General segment may be written)
#pragma config GCP = OFF                // General Segment Code Flash Code Protection bit (No protection)

// FOSCSEL
#pragma config FNOSC = FRCPLL           // Oscillator Select (8 MHz FRC oscillator (FRC)) with PLL
#pragma config IESO  = OFF              // Internal External Switch Over bit (Internal External Switchover mode disabled (Two-Speed Start-up disabled))

// FOSC
#pragma config POSCMOD = NONE           // Primary Oscillator Configuration bits (Primary oscillator disabled)
#pragma config OSCIOFNC = ON           // CLKO Enable Configuration bit (CLKO output signal is active on the OSCO pin)
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

I2CMessage test_msg;
unsigned char 	msg_data[5] = {0xFF, 0x00, 0x44, 0xAA, 0xBB};

void blink_light1(void)
{
	_RA1 = !_RA1;
}

void send_test_message(void)
{
    static unsigned char unused;

	MIBCommandPacket cmd;

    cmd.feature = 0x25;
    cmd.command = 0xAA;

    bus_master_sendcommand(9, &cmd, master_nullcallback, &unused);
}

ScheduledTask task1;
ScheduledTask task0;
ScheduledTask i2c;

int main(void) 
{
    AD1PCFG = 0xFFFF;

    _TRISA1 = 0;
    _TRISA0 = 0;
    _TRISA6 = 0;

    _RA1 = 1;
    _RA0 = 1;
    _RA6 = 1;

    handle_reset();

    //scheduler_schedule_task(blink_light1, kEverySecond, kScheduleForever, &task1);
    //scheduler_schedule_task(send_test_message, kEverySecond, kScheduleForever, &i2c);

    taskloop_loop();

    return (EXIT_SUCCESS);
}
