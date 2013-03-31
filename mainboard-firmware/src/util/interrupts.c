#include "interrupts.h"

void configure_interrupts()
{
    _NSTDIS = 0; //Allow interrupt nesting
    _ALTIVT = 0;
}
