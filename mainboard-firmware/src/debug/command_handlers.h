#ifndef __command_handlers_h__
#define	__command_handlers_h__

#include "serial_commands.h"

void handle_echo_params(command_params *params);
void handle_device(command_params *params);
void handle_rtcc(command_params *params);
void handle_gsm(command_params *params);
void handle_sensor(command_params *params);
void handle_memory(command_params *params);
void handle_adc(command_params *params);
void handle_log(command_params *params);
void handle_report(command_params *params);
void handle_registration(command_params *params);

#endif	/* __command_handlers_h__ */

