
/*
 * This file contains the code to initialize the command dispatch tables for this microcontroller.  The actual command handling routines are in the command_handlers.c file
 *
 */

#include "serial_commands.h"
#include <string.h>

char command_buffer[UART_BUFFER_SIZE];

char *known_commands[MAX_COMMANDS];
CommandHandler command_handlers[MAX_COMMANDS];


void register_command_handlers()
{
    register_command("led", handle_led);
    register_command("echo", handle_echo_params);
    register_command("device", handle_device);
    register_command("rtcc", handle_rtcc);
    register_command("gsm", handle_gsm );
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

/*
 * Process the command line currently resident in the command_buffer, parsing the parameters and calling the appropriate command handler.
 *
 */
void process_command()
{
   char *params = 0;
   unsigned int i, len;

   len = strlen(command_buffer);

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
       sends(U2, "Unknown command: ");
       sends(U2, command_buffer);
       sends(U2, "\n");
       break;
     }
     else if(strcmp(command_buffer, known_commands[i])==0)
     {
       //We have a match
       command_params p;

       fill_param_struct(&p, params);

       command_handlers[i](&p);
       break;
     }
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

  while(*buff != '\0')
  {
    if (*buff == ' ')
    {
        *buff++ = '\0'; //Convert space to null terminator and skip it
        ++params->num_params;
    }

    ++buff;
  }
}

char *get_param_string(command_params *params, unsigned int i)
{
  unsigned int j;

  if (i >= params->num_params)
  {
    sends(U2, "Invalid parameter number, too large.\n");
    return 0;
  }
 
  char *param = params->params;
  for (j=0; j<i; ++j)
  {
    while (*param++ != '\0') //Skip past one parameter
      ;
  }

  return param;
}
