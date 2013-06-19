#ifndef _mainboard_reset_handler_h
#define _mainboard_reset_handler_h

#include "reset_manager.h"

void register_reset_handlers();

//Reset Type Handlers
void handle_poweron_reset(unsigned int type);
void handle_mclr_reset(unsigned int type);

void handle_all_resets_before(unsigned int type);
void handle_all_resets_after(unsigned int type);

#endif