//common.h

#ifndef __common_h__
#define __common_h__

#define CLOCKSPEED      8000000L //F_osc / 2 (the instruction clock frequency)

#define CLEAR_BIT(field, bitnum) (field ^= 1<<bitnum)
#define SET_BIT(field,bitnum)    (field |= 1<<bitnum)

extern void asm_reset();

#endif
