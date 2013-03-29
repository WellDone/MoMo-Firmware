//oscillator.c

#include "oscillator.h"
#include "common.h"

//internal utility functions
static inline void write_osccon_h(unsigned char value);
static inline void write_osccon_l(unsigned char value);

void oscillator_init()
{
    set_sosc_status(1); //Secondary oscillator always needs to be enabled
}

void set_oscillator_speed(OscillatorSpeedSelector speed)
{
    unsigned int curr = _COSC;
    unsigned char oscconh;
    unsigned char oscconl;

    if (curr == speed)
        return;

    uninterruptible_start()

    oscconh = OSCCONH;
    oscconl = OSCCONL;

    //select new speed
    oscconh &= ~(0b111 << 8);
    oscconh |= speed;

    write_osccon_h(oscconh);

    //If we're switching to high speed, change clkdiv first so make the PLL always between 4 and 8 mhz
    if (speed == kHighSpeedSelect)
        _RCDIV = kHighSpeedClockDiv;

    SET_BIT(oscconl, 0);
    write_osccon_l(oscconl);

    while (_OSWEN)
        ;
    
    if (speed == kLowSpeedSelect)
        _RCDIV = kLowSpeedClockDiv;


    uninterruptible_end()
}

/*
 * pass enabled=1 to enable the secondary oscillator
 * pass enabled=0 to disable the secondary oscillator
 * STATUS: Tested and working (Tim Burke) 3/11/13
 */
void set_sosc_status(int enabled)
{
    unsigned char oscl = OSCCONL;

    if (enabled > 0)
        SET_BIT(oscl, 1); //SOSCEN is bit 1
    else
        CLEAR_BIT(oscl, 1); //Clear bit

    write_osccon_l(oscl);
}

/*
 * get_sosc_status
 * returns 1 is the secondary oscillator is enabled and 0 otherwise
 * STATUS: Tested and working (Tim Burke) 3/11/13
 */
int get_sosc_status()
{
    return (OSCCONL & (1<<1)) >> 1;
}

/*
 * Disable interrupts and write the given value to the low byte of OSCCON,
 * from: http://www.microchip.com/forums/m544753.aspx
 */
static inline void write_osccon_l(unsigned char value)
{
    int temp1, temp2, temp3;

    asm ("MOV #OSCCONL, %0\n\t"
            "MOV #0x46, %1\n\t"
            "MOV #0x57, %2\n\t"
            "disi #3\n\t"
            "mov.b %1,[%0]\n\t"
            "mov.b %2,[%0]\n\t"
            "mov.b %3,[%0]" : "=r"(temp1), "=r"(temp2), "=r"(temp3) : "r"(value));
 }

static inline void write_osccon_h(unsigned char value)
{
    int temp1, temp2, temp3;

    asm ("MOV #OSCCONH, %0\n\t"
            "MOV #0x78, %1\n\t"
            "MOV #0x9A, %2\n\t"
            "disi #3\n\t"
            "mov.b %1,[%0]\n\t"
            "mov.b %2,[%0]\n\t"
            "mov.b %3,[%0]" : "=r"(temp1), "=r"(temp2), "=r"(temp3) : "r"(value));
 }