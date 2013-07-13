#include <stdlib.h>
#include "common.h"
#include "mainboard_reset_handler.h"
#include "task_manager.h"
#include "scheduler.h"
#include "bus_master.h"
#include <string.h>

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

void blink_light1(void)
{
	_RA0 = !_RA0;
}

void send_erase_message(void)
{
    bus_master_rpc(NULL, 8, 255, 0x00);
}

void send_write_message(void)
{
    MIBBufferParameter *param3;
    char *msg = "test";

    bus_master_compose_params(plist_define3(kMIBInt16Type, kMIBInt16Type, kMIBBufferType));
    set_intparam(0, 0);
    set_intparam(1, 1<<12);
    param3 = get_buffer_param(2);
    memmove(get_buffer_loc(2), msg, 5);
    param3->header.len = 5;

    bus_master_rpc(NULL, 8, 255, 0x01);
}

void send_read_message(void)
{
    bus_master_compose_params(plist_define3(kMIBInt16Type, kMIBInt16Type, kMIBInt16Type));
    set_intparam(0, 0);
    set_intparam(1, 1<<12);
    set_intparam(2, 5);

    bus_master_rpc(NULL, 8, 255, 0x02);
}

void send_test_message(void)
{
    static unsigned int i = 0;

    if (i == 0)
        send_erase_message();
    else if(i==1)
        send_write_message();
    else if(i==2)
        send_read_message();

    ++i;

    if (i == 3)
        i = 0;
}

void send_blink_message(void)
{
    bus_master_compose_params(plist_define1(kMIBInt16Type));
    set_intparam(0, 5);

    bus_master_rpc(NULL, 0x09, 0x01, 0x00);
}

ScheduledTask task1;
ScheduledTask i2c;

int main(void)
{
    AD1PCFG = 0xFFFF;

    _TRISA1 = 0;
    _TRISA0 = 0;
    //_TRISA6 = 0;

    _RA1 = 1;
    _RA0 = 1;
    //_RA6 = 1;

    register_reset_handlers();
    handle_reset();

    //scheduler_schedule_task(blink_light1, kEverySecond, kScheduleForever, &task1);
    scheduler_schedule_task(send_blink_message, kEverySecond, kScheduleForever, &i2c);

    taskloop_loop();

    return (EXIT_SUCCESS);
}
