#ifndef __command_handlers_h__
#define	__command_handlers_h__

#include "serial_commands.h"

CommandStatus handle_echo_params(command_params *params);
CommandStatus handle_device(command_params *params);
CommandStatus handle_rtcc(command_params *params);
CommandStatus handle_adc(command_params *params);
CommandStatus handle_rpc(command_params *params);
CommandStatus handle_binrpc(command_params *params);
CommandStatus handle_alarm(command_params *params);

#endif	/* __command_handlers_h__ */

