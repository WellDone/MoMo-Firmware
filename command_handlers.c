#include "serial_commands.h"
#include "rtcc.h"
#include "utilities.h"

void handle_led(command_params *params)
{
  if (params->num_params < 1)
  {
    sends(U2, "You must pass either 'on' or 'off' to the led command.\r\n");
    return;
  }

  char *cmd = get_param_string(params,0);

  if (strcmp("on", cmd) == 0)
    _LATA0 = 1;
  else if(strcmp("off", cmd) == 0)
    _LATA0 = 0;
  else
    sends(U2, "Invalid argument to led command.\r\n");
}

void handle_echo_params(command_params *params)
{
  unsigned int i;
  
  if (params->num_params == 0)
    sends(U2, "No parameters were passed.\r\n");
  else
  {
    for (i=0; i<params->num_params; ++i)
    {
      sends(U2, get_param_string(params, i));
      sends(U2, "\r\n");
    }
  }
}

void handle_device(command_params *params)
{
    char *cmd;

    if (params->num_params < 1)
    {
        sends(U2, "You must pass a subcommand to the device command.\r\n");
        return;
    }

    cmd = get_param_string(params, 0);

    if (strcmp(cmd, "reset") == 0)
    {
        sends(U2, "Resetting the device...\r\n");
        asm_reset();
    }
}

void handle_rtcc(command_params *params)
{
    char *cmd;

    if (params->num_params < 1)
    {
        sends(U2, "You must pass a subcommand to the rtcc command.\r\n");
        return;
    }

    cmd = get_param_string(params, 0);

    //Farm out the subcommand to our subhandlers

    if (strcmp(cmd, "status") == 0)
    {
        unsigned int enabled = rtcc_enabled();


        sendf(U2, "Realtime Clock Status: %s\r\n", enabled? "Enabled" : "Disabled");
        rtcc_time time;

        rtcc_get_time(&time);

        sendf(U2, "Current Time: %d/%d/%d %d:%d:%d\r\n", time.month, time.day, time.year, time.hours, time.minutes, time.seconds);
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
            sends(U2, "usage: rtcc set mm/dd/yy hh:mm:ss\r\n");
            return;
        }

        date = get_param_string(params, 1);
        time = get_param_string(params, 2);

        sendf(U2, "Date string: %s\r\nTime string: %s\r\n", date, time);

        time_spec.month = get_2byte_number(date);
        time_spec.day = get_2byte_number(date+3);
        time_spec.year = get_2byte_number(date+6);
        time_spec.hours = get_2byte_number(time);
        time_spec.minutes = get_2byte_number(time+3);
        time_spec.seconds = get_2byte_number(time+6);

        m = time_spec.month;

        sendf(U2, "Input month was: %d\r\n", m);

        rtcc_set_time(&time_spec);
    }
    else
        sendf(U2, "Unknown rtcc command: %s\r\n", cmd);
}
