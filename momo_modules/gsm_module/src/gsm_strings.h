//gsm_strings.h

#define kErrorString 0
#define kOkayString 1
#define kStartStreamString 2


#ifndef __DEFINES_ONLY__
extern uint8 match_okay_response();
extern uint8 match_error_response();
extern uint8 load_gsm_constant(uint8 index);
#endif