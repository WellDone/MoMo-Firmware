#include <stdlib.h>
#include "common.h"
#include "mainboard_reset_handler.h"
#include "task_manager.h"
#include "scheduler.h"
#include "memory.h"
#include "bus_master.h"
#include "report_manager.h"
#include <string.h>

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

TaskManagerSleepStatus sleep_handler(TaskManagerCallbackReason task);

int main(void)
{
    register_reset_handlers();
    handle_reset();

    taskloop_set_sleephandler(sleep_handler);
    taskloop_loop();

    return (EXIT_SUCCESS);
}

TaskManagerSleepStatus sleep_handler(TaskManagerCallbackReason task)
{
	if (bus_master_idle())
	{
        if (taskloop_get_flag(kTaskLoopLightSleepBit) == 0)
		  mem_remove_power();
		
        return kCanEnterSleep;
	}

	return kCannotEnterSleep;
}
