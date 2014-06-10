#include "interrupts.h"

void configure_interrupts()
{
    _NSTDIS = 0; //Allow interrupt nesting
    _ALTIVT = 0;
}

int  disable_interrupts()
{
	int old_ipl;

	SET_AND_SAVE_CPU_IPL(old_ipl, kMaxInterruptLevel);
	return old_ipl;
}

void enable_interrupts(int old_level)
{
	RESTORE_CPU_IPL(old_level);
}