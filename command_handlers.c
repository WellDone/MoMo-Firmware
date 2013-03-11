#include <xc.h>
#include "serial_commands.h"
#include "rtcc.h"
//#include "utilities.h"
#include "sensor.h"
#include "memory.h"
static unsigned long next_free; 
static unsigned long next_read;

int gsm_at_cmd( const char* cmd )
{
    sends( U1, cmd );
    sends( U1, "\r\n" );
    wait( 300 );
}

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

void handle_gsm_module(command_params *params)
{
    char *cmd;

    if (params->num_params < 1)
    {
        sends(U2, "You must pass a subcommand to the device command.\r\n");
        return;
    }

    cmd = get_param_string(params, 0);

    if (strcmp(cmd, "power") == 0)
    {
        if (params->num_params < 2)
        {
            sends(U2, "usage: gsm power [on|off]\r\n");
            return;
        }

        if (strcmp(get_param_string(params, 1),"on") == 0)
        {
            _LATA0 = 1;
            sends(U2, "GSM module powered on.\r\n");
        }
        else if (strcmp(get_param_string(params, 1),"off") == 0)
        {
            _LATA0 = 0;
            sends(U2, "GSM module powered off.\r\n");
        }
        else
            sends(U2, "Invalid option to gsm power command.\r\n");
    }

    if (strcmp(cmd, "module") == 0)
    {
        if (params->num_params < 2)
        {
            sends(U2, "usage: gsm module [on|off]\r\n");
            return;
        }

        if (strcmp(get_param_string(params, 1),"on") == 0)
        {
            _LATA2 = 0;
            sends(U2, "GSM module turned on.\r\n");
        }
        else if (strcmp(get_param_string(params, 1),"off") == 0)
        {
            _LATA2 = 1;
            
            gsm_at_cmd("AT+CPOF");
            sends(U2, "GSM module turned off.\r\n");
        }
        else if (strcmp(get_param_string(params, 1), "dump") == 0)
        {
            dump_gsm_buffer();
        }
        else if (strcmp(get_param_string(params, 1), "slow") == 0)
        {
            gsm_at_cmd("AT+IPR=38400");
        }
        else if (strcmp(get_param_string(params, 1), "send") == 0)
        {
            if (params->num_params >= 3)
            {
                gsm_at_cmd(get_param_string(params,2));
                sends(U2, "command sent\r\n");
            }
            else
                sends(U2, "usage: gsm module send <cmd>\r\n");
        }
        else if (strcmp(get_param_string(params, 1), "msg") == 0)
        {
            if (params->num_params >= 3)
            {
                sends(U1, "AT+CMGS=\"");
                sends(U1, get_param_string(params,2));
                sends(U1, "\"\r");

            }
            else
                sends(U2, "usage: gsm module msg <address>\r\n");
        }
        else if (strcmp(get_param_string(params, 1), "msgend") == 0)
        {
            //gsm_at_cmd("AT+CMGF=1");
            gsm_at_cmd("Just a friendly heads up, WellDone has sensed that pump #21 needs repair.\x1A");
            //gsm_at_cmd("Hello World!\x1A");

        }
        else if (strcmp(get_param_string(params, 1), "msgwell") == 0)
        {
            if (params->num_params >= 3)
            {
                sends(U1, get_param_string(params,2));
                sends(U1, "\x1A");

            }
            else
                sends(U2, "usage: gsm module msgwell <status>\r\n");
        }
        else
            sends(U2, "Invalid option to gsm module command.\r\n");
    }

    if (strcmp(cmd, "hello") == 0)
    {
        gsm_at_cmd( "AT+ICF?" );
        //gsm_at_cmd( "AT+CMGF=1" );
        sends(U2, "hello sent\r\n");
    }

    else if (strcmp(cmd, "baud") == 0)
    {
        sendf(U2, "BRG1 Value: %d\r\n", U1BRG);
    }
    else if (strcmp(cmd, "report") == 0)
        {
            if (params->num_params >= 2)
            {
                unsigned int i=0;
                sends(U1, "AT+CMGS=\"");
                sends(U1, "+15107358486");
                sends(U1, "\"\r");

                //delay
                for (i=0; i<65000; ++i)
                    ;

                sends(U1, get_param_string(params,1));
                sends(U1, "\x1A");
            }
            else
                sends(U2, "usage: gsm report <status>\r\n");
        }
    else if (strcmp(cmd, "test") == 0)
    {
        sends(U1, "U\r");
    }
    else if (strcmp(cmd, "pattern") == 0)
    {
        char cmd[4] = {0xFF,0x00,'\r','\0'};
        sends(U1, cmd);
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

void handle_sensor(command_params *params) {
  IEC1bits.INT2IE = 1; //enable interrupt
  sends(U2, "Good night, Sweet Prince");
  Sleep();
  sends(U2, "I can't do that Dave");
  IEC1bits.INT2IE = 0; //disable interrupt
}

void handle_memory(command_params *params) {
    char* cmd;
    cmd = get_param_string(params, 0);
    int val = 0;
    sendf(U2, "handling memory\r\n");
    if (strcmp(cmd, "write") == 0) {
      next_read = 0xA;
      val = get_param_string(params, 1);
      mem_write(val);
    } else if (strcmp(cmd, "read") == 0) {
      next_read = next_free - 1;
      sendf(U2, "Val previous:%d\r\n", val);
      val = ((int) mem_read());
      sendf(U2, "Val read:%d\r\n", val);
    }
}
