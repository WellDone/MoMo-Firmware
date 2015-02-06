#ifndef __oscillator_h__
#define __oscillator_h__

/*
 * Functions to control the PIC oscillator speed and secondary oscillator.
 */

//These are instruction clocks (so the actual clock / 2)
#define kLowSpeedClockFreq 1000000
#define kHighSpeedClockFreq 8000000

#define kHighSpeedClockDiv  0b001 	//postscale divide by 2
#define kLowSpeedClockDiv	0b010 	//postscale divide by 4

#define CURRENT_OSCILLATOR() (_COSC)

typedef enum
{
	k8MhzFRC 	= 0b000,
	k4MhzFRC 	= 0b001,
	k2MhzFRC 	= 0b010,
	k1MhzFRC 	= 0b011,
	k500KhzFRC	= 0b100,
	k250KhzFRC	= 0b101,
	k125KhzFRC	= 0b110,
	k3125KhzFRC	= 0b111
} FRCPostscaler;

//Clock types that are supported
enum
{
	kFRCDIV = 0b111,
	kFRCDIVPLL = 0b001
};

void 	oscillator_init();

void    set_sosc_status(int enabled);
int     get_sosc_status();

int 	set_oscillator_speed(FRCPostscaler speed, int use_pll);
#endif
