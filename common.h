//common.h

#ifndef __common_h__
#define __common_h__

#define CLOCKSPEED      4000000L

#define CLEAR_BIT(field, bitnum) (field ^= 1<<bitnum)
#define SET_BIT(field,bitnum)    (field |= 1<<bitnum)

extern void asm_reset();

#endif
