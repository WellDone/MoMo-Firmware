//oscillator.h

/*
 * Functions to control the PIC oscillator speed and secondary oscillator.
 */


void 	oscillator_init();

void    set_sosc_status(int enabled);
int     get_sosc_status();
