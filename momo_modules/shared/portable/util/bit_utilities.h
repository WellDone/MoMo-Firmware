#ifndef _bit_utilites_h
#define _bit_utilites_h

#define CLEAR_BIT(field, bitnum) ((field) &= ~(1<<bitnum))
#define SET_BIT(field,bitnum)    ((field) |= 1<<bitnum)
#define TOGGLE_BIT(field,bitnum) ((field) ^= 1<<bitnum)
#define BIT(field, bitnum) 		 ((field) & (1 << bitnum))

//For this macro, field should be an unsigned variable because of the the right shift operator
#define BIT_TEST(field, bitnum)  ((field & (1 << bitnum)) >> bitnum)

#endif