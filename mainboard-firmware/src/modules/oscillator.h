//oscillator.h

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
	kHighSpeedSelect = 0b001,
	kLowSpeedSelect  =0b111
} OscillatorSpeedSelector;

void 	oscillator_init();

void    set_sosc_status(int enabled);
int     get_sosc_status();

void 	set_oscillator_speed(OscillatorSpeedSelector speed);