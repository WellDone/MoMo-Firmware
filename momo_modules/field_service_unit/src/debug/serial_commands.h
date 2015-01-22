#ifndef __serial_commands_h__
#define __serial_commands_h__

#include "uart.h"

#define MAX_COMMANDS 12

typedef struct
{
  unsigned int num_params;
  char *params;
} command_params;

typedef enum {
	kNone    = 0,
	kPending = 1,
	kSuccess = 2,
	kFailure = 3
} CommandStatus;

//Set up a dispatch array with pointers to command handling functions
typedef CommandStatus (*CommandHandler)(command_params*);

//Setup-function prototypes
void register_command(char *cmd, CommandHandler handler);
void register_command_handlers();

void fill_param_struct(command_params *params, char *buff);

//Convenience functions for command handlers
char *get_param_string(command_params *params, unsigned int i);

void set_command_result( bool success );

#endif
