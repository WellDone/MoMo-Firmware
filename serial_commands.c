
/*
 * This file contains the code to initialize the command dispatch tables for this microcontroller.  The actual command handling routines are in the command_handlers.c file
 *
 */

#include "serial_commands.h"
#include <string.h>

volatile char __attribute__((space(data))) command_buffer[UART_BUFFER_SIZE];
volatile int  __attribute__((space(data))) cmd_ready;

char __attribute__((space(data))) *known_commands[MAX_COMMANDS+1];
CommandHandler __attribute__((space(data))) command_handlers[MAX_COMMANDS+1];

extern UART_STATUS uart_stats[2];

void register_command_handlers()
{
    command_buffer[0] = '\0';
    cmd_ready = 0;
    
    register_command("led", handle_led);
    register_command("echo", handle_echo_params);
    register_command("device", handle_device);
    register_command("rtcc", handle_rtcc);
    register_command("gsm", handle_gsm_module );
    register_command("gsm2", handle_gsm );
    register_command("sensor", handle_sensor);
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
 * Check if there are any commands pending and if so, process them.
 */
void process_commands_task()
{
    if (cmd_ready)
    {
        strncpy(command_buffer, uart_stats[1].rcv_buffer, UART_BUFFER_SIZE);
        process_command();

        sends( U2, "PIC 24f16ka101> ");

        cmd_ready = 0;
    }

    taskloop_add(process_commands_task); //Make sure we keep getting added to the list FIXME (make this interrupt driven
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
       sends(U2, "\r\n");
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
    sends(U2, "Invalid parameter number, too large.\r\n");
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
