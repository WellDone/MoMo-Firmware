#ifndef __mib_command_parameters_h
#define __mib_command_parameters_h

#include "mib_command.h"

//Macros for defining parameter lists
#define plist_param_n(n, type) 					((type & 0x01) << (n+3))
#define plist_1param(type) 						plist_param_n(0, type)
#define plist_2params(type1, type2) 			(plist_param_n(0, type1) | plist_param_n(1, type2))
#define plist_3params(type1, type2, type3) 		(plist_param_n(0, type1) | plist_param_n(1, type2) | plist_param_n(2, type3))
#define plist_define(count, params) 			((count & 0b111) | params)

#define plist_define0()							plist_define(0, 0)
#define plist_define1(type)						plist_define(1, plist_1param(type))
#define plist_define2(type1, type2)				plist_define(2, plist_2params(type1, type2))
#define plist_define3(type1, type2, type3)		plist_define(3, plist_3params(type1, type2, type3))

#define extract_param_type(params, n) ((params & (1<<(n+3))) >> (n+3))
#define extract_param_count(params) (params & 0b111)

volatile MIBParamList *	build_params(mib_command_handler* handler);

#endif