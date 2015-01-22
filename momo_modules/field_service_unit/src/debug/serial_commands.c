
/*
 * This file contains the code to initialize the command dispatch tables for this microcontroller.  The actual command handling routines are in the command_handlers.c file
 *
 */

#include "common.h"
#include "serial_commands.h"
#include "command_handlers.h"
#include "task_manager.h"
#include "debug.h"
#include <string.h>

char __attribute__((space(data))) *known_commands[MAX_COMMANDS+1];
CommandHandler __attribute__((space(data))) command_handlers[MAX_COMMANDS+1];
static volatile CommandStatus command_status = kNone;

static void process_commands_task(char* command_buffer, int len, bool overflown);

void register_command_handlers()
{
    register_command("echo", handle_echo_params);
    register_command("device", handle_device);
    register_command("rtcc", handle_rtcc);
    register_command("adc", handle_adc);

    register_command("binrpc", handle_binrpc);
    register_command("alarm", handle_alarm);
    register_command("attached", handle_attached);
    register_command("i2c", handle_i2cstatus);

    debug_setup_handler(process_commands_task);
}

/*
 * Register a new handler for a specific command name.  When this string, the command name, is received from the serial line, the specified handler will get invoked to handle it.
 *
 */
void register_command(char *cmd, CommandHandler handler)
{
  static unsigned int i = 0;

  if (i >= MAX_COMMANDS)
    return;

  known_commands[i] = cmd;
  command_handlers[i] = handler;

  ++i;

  //Sentinal value so we know there are no more valid commands in the array
  known_commands[i] = 0;
  command_handlers[i] = 0;
}

static void send_command_acknowledgement()
{
  put( DEBUG_UART, (command_status == kSuccess)?ACK:NAK );
  command_status = kNone;
}

void set_command_result( bool success )
{
  command_status = ((success)?kSuccess:kFailure);
  send_command_acknowledgement();
}

/*
 * Check if there are any commands pending and if so, process them.
 */
static void process_commands_task(char* command_buffer, int len, bool overflown)
{
  char *params = 0;
  unsigned int i;
  command_buffer[len] = '\0';

  for(i=0; i<=len; ++i)
  {
   if (command_buffer[i] == ' ')
   {
     //The command is separated from the parameters by a space, so start the params here
     command_buffer[i] = '\0';
     params = command_buffer + i + 1;
     break;
   }
  }

  for (i=0; i<MAX_COMMANDS; ++i)
  {
   if (known_commands[i] == 0)
   {
     //We've loooked through all the commands we know how to handle and not found it, punt.
     print( "Unknown command: ");
     print( command_buffer);
     print( "\r\n");
     command_status = kFailure;
     break;
   }
   else if(strcmp(command_buffer, known_commands[i])==0)
   {
     //We have a match
     command_params p;

     fill_param_struct(&p, params);

     command_status = command_handlers[i](&p);
     break;
   }
  }

  if ( command_status != kPending && command_status != kNone ) {
    send_command_acknowledgement();
  }
}

void fill_param_struct(command_params *params, char *buff)
{
  params->num_params = 0;
  params->params = 0;

  //Check if there were any params
  if (buff == 0 || *buff == '\0')
     return;

  params->num_params = 1;
  params->params = buff;


  unsigned char quoted = 0;
  while(*buff != '\0')
  {
    if ( *buff == ' ' && !quoted )
    {
        *buff++ = '\0'; //Convert space to null terminator and skip it
        ++params->num_params;
    } else if ( *buff == '"' ) {
        if ( quoted && *(buff+1) == ' ' ) {
            *buff++ = '\0';
            *buff++ = '"'; // Move the quote to the beginning of the next param, it will be skipped.
            quoted = 0;
        } else {
            ++buff;
            quoted = 1;
        }
    }

    ++buff;
  }
}

char *get_param_string(command_params *params, unsigned int i)
{
  unsigned int j;

  if (i >= params->num_params)
  {
    print( "Invalid parameter number, too large.\r\n");
    return 0;
  }

  char *param = params->params;
  for (j=0; j<i; ++j)
  {
    while (*param++ != '\0') //Skip past one parameter
      ;
  }

  if ( *param == '"' || *param == ' ' )  //ignore one leading quote or space
      ++param;

  return param;
}
