#ifndef __gsm_h__
#define __gsm_h__

#include "platform.h"

#define kCMDOK 1
#define kCMDERR 2

#define kDEFAULT_CMD_TIMEOUT 2

void gsm_init();
bool gsm_on();

bool gsm_register( uint8 timeout_s );
bool gsm_registered();

void gsm_remember_band();
void gsm_recall_band();
void gsm_forget_band();

uint8 gsm_cmd(const char* cmd);
uint8 gsm_cmd_raw(const char* cmd, uint8 timeout);
void gsm_off();

#endif