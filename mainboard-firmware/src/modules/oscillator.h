//oscillator.h

/*
 * Functions to control the PIC oscillator speed and secondary oscillator.
 */


void    set_sosc_status(int enabled);
int     get_sosc_status();

//internal utility functions
static inline void write_osccon_h(unsigned char value);
static inline void write_osccon_l(unsigned char value);
