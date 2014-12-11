#ifndef __gsm_strings_h__
#define __gsm_strings_h__

#define kErrorString 0
#define kOkayString 1
#define kStartStreamString 2
#define kNewMessageString 3

#ifndef __DEFINES_ONLY__
#include "platform.h"

extern uint8 match_okay_response();
extern uint8 match_error_response();
extern uint8 match_newmsg();
extern uint8 match_newmsg2digit();
extern uint8 load_gsm_constant(uint8 index);

void reset_match_counters();
bool cmgs_matched();
bool creg_matched();
bool ok_matched();
bool err_matched();

void capture_error_code();
#endif

#endif