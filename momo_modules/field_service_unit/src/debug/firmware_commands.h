#ifndef __firmware_commands_h
#define __firmware_commands_h

#include "common.h"
#include "serial_commands.h"

CommandStatus handle_push_firmware(command_params* params);
CommandStatus handle_pull_firmware(command_params* params);

#endif