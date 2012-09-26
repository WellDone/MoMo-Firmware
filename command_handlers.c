#include "serial_commands.h"

void handle_led(command_params *params)
{
  if (params->num_params < 1)
  {
    sends("You must pass either 'on' or 'off' to the led command.\n");
    return;
  }

  char *cmd = get_param_string(params,0);

  if (strcmp("on", cmd) == 0)
    _LATA0 = 1;
  else if(strcmp("off", cmd) == 0)
    _LATA0 = 0;
  else
    sends("Invalid argument to led command.\n");
}

void handle_echo_params(command_params *params)
{
  unsigned int i;
  
  if (params->num_params == 0)
    sends("No parameters were passed.\n");
  else
  {
    for (i=0; i<params->num_params; ++i)
    {
      sends(get_param_string(params, i));
      sends("\n");
    }
  }
}

void handle_device(command_params *params)
{
    char *cmd;

    if (params->num_params < 1)
    {
        sends("You must pass a subcommand to the device command.\n");
        return;
    }

    cmd = get_param_string(params, 0);

    if (strcmp(cmd, "reset"))
    {
        sends("Resetting device...\n")
        asm_reset();
    }
}