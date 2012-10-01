#include "serial_commands.h"
#include "rtcc.h"
#include "utilities.h"

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

    if (strcmp(cmd, "reset") == 0)
    {
        sends("Resetting the device...\n");
        asm_reset();
    }
}

void handle_rtcc(command_params *params)
{
    char *cmd;

    if (params->num_params < 1)
    {
        sends("You must pass a subcommand to the rtcc command.\n");
        return;
    }

    cmd = get_param_string(params, 0);

    //Farm out the subcommand to our subhandlers

    if (strcmp(cmd, "status") == 0)
    {
        unsigned int enabled = rtcc_enabled();


        sendf("Realtime Clock Status: %s\n", enabled? "Enabled" : "Disabled");
        rtcc_time time;

        rtcc_get_time(&time);

        sendf("Current Time: %d/%d/%d %d:%d:%d", time.month, time.day, time.year, time.hours, time.minutes, time.seconds);
        sends("\n"); //sendf is not synchronous
    }

    else if (strcmp(cmd, "set") == 0)
    {
        rtcc_time time_spec;
        char *date, *time;
        char temp[3];
        int m;
        
        temp[2] = '\0';

        if (params->num_params < 3)
        {
            sends("usage: rtcc set mm/dd/yy hh:mm:ss\n");
            return;
        }

        date = get_param_string(params, 1);
        time = get_param_string(params, 2);

        sendf("Date string: %s\nTime string: %s\n", date, time);
        

        time_spec.month = get_2byte_number(date);
        time_spec.day = get_2byte_number(date+3);
        time_spec.year = get_2byte_number(date+6);
        time_spec.hours = get_2byte_number(time);
        time_spec.minutes = get_2byte_number(time+3);
        time_spec.seconds = get_2byte_number(time+6);

        m = time_spec.month;

        sendf("Input month was: %d\n", m);

        rtcc_set_time(&time_spec);
    }
    else
        sendf("Unknown rtcc command: %s\n", cmd);
}
