#ifndef __gsm_h__
#define __gsm_h__

#include "platform.h"
#include "global_state.h"

#define kCMDOK 1
#define kCMDERR 2

#define kDEFAULT_CMD_TIMEOUT 2

#define GSM_REGISTRATION_TIMEOUT_S 120

void gsm_init();

GSMError gsm_ensure_on();
uint8_t gsm_ensure_registered();

void gsm_remember_band();
void gsm_recall_band();
void gsm_forget_band();

uint8_t gsm_cmd(const char* cmd);
uint8_t gsm_cmd_raw(const char* cmd, uint8_t timeout);
void gsm_off();

#endif