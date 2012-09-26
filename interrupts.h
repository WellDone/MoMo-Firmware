
#ifndef __interrupts_h__
#define __interrupts_h__

#include <p24f16ka101.h>

typedef void (*isr_handler)();

#define CLEAR_PRI(reg, lsb)     ((*reg) ^= (0b111 << lsb))
#define SET_PRI(reg, lsb, pri)  ((*reg) |= (pri&0b111) << lsb)     //CLEAR_PRI must first be called


typedef struct
{
    unsigned char   priority; //3 bit priorty (higher order bits are masked)
    unsigned char   enabled; //1 for enabled interrupt, 0 for disabled

    //Location of the specific registers for this isr
    //Should be filled in by an initialization macro.
    unsigned int    *iec_reg;
    unsigned int    *pri_reg;
    unsigned int    *isf_reg;

    unsigned char   iec_bit;
    unsigned char   pri_bit; //lsb of the 3-bit priority of this interrupt
    unsigned char   isf_bit;
} isr_descriptor;

//Routines
void configure_interrupts();

#endif
