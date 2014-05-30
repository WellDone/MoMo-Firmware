//gsm_module.h

#ifndef __gsm_module_h__
#define __gsm_module_h__

#include "gsm_defines.h"
#include "platform.h"

void gsm_init();
uint8 gsm_on();
void gsm_off();

#define GSM_REGISTRATION_TIMEOUT_S 30
bool wait_for_registration();

#endif