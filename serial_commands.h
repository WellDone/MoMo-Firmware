#ifndef __serial_commands_h__
#define __serial_commands_h__

#include "serial.h"

#define MAX_COMMANDS 30

typedef struct
{
  unsigned int num_params;
  char *params;
} command_params;

//Set up a dispatch array with pointers to command handling functions
typedef void (*CommandHandler)(command_params*);

//Setup-function prototypes
void register_command(char *cmd, CommandHandler handler);
void register_command_handlers();
void process_command();

void process_commands_task();

void fill_param_struct(command_params *params, char *buff);

//Convenience functions for command handlers
char *get_param_string(command_params *params, unsigned int i);

#endif
