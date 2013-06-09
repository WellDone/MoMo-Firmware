#include <xc.h>
#include <stdio.h>
#include <string.h>
#include "serial_commands.h"
#include "rtcc.h"
#include "utilities.h"
#include "oscillator.h"
#include "adc.h"
#include <stdio.h>
#include <string.h>
#include "scheduler.h"
#include "reset_manager.h"

extern volatile unsigned int adc_buffer[kADCBufferSize];
ScheduledTask test_task;

void handle_echo_params(command_params *params)
{
  unsigned int i;

  if (params->num_params == 0)
    print( "No parameters were passed.\r\n");
  else
  {
    for (i=0; i<params->num_params; ++i)
    {
      print( get_param_string(params, i));
      print( "\r\n");
    }
  }
}

void handle_adc(command_params *params)
{
    char *cmd;

    if (params->num_params < 1)
    {
        print( "You must pass a subcommand to the device command.\r\n");
        return;
    }

    cmd = get_param_string(params, 0);

    if (strcmp(cmd, "acquire") == 0)
    {
        int num_samples = 1;
        if (params->num_params == 2)
        {
            atoi_small( get_param_string(params, 1), &num_samples );
        }
        ADCConfig config;
        unsigned int channels = 0;

        ADD_CHANNEL(channels, 1);
        ADD_CHANNEL(channels, 5);
        ADD_CHANNEL(channels, kBandgapReference);
        ADD_CHANNEL(channels, kHalfBandgapReference);

        config.output_format = kUIntegerFormat;
        config.trigger = kInternalCounter;
        config.reference = kVDDVSS;
        config.enable_in_idle = 0;
        config.sample_autostart = 1;
        config.scan_input = 0;
        config.alternate_muxes = 0;
        config.autosample_wait = 0b11111;

        config.oneshot = 1;
        config.num_samples = num_samples;

        adc_configure(&config);
        adc_setup_scan(channels);
        _PCFG1 = 0;
        _TRISA1 = 1;
        adc_enable();
        sendf(U2, "ADC Enabled, acquiring %d samples\r\n", num_samples);
    }
    else if (strcmp(cmd, "get") == 0)
    {
        ADCConfig config;
        unsigned int val;

        config.output_format = kUIntegerFormat;
        config.trigger = kInternalCounter;
        config.reference = kVDDVSS;
        config.enable_in_idle = 0;
        config.sample_autostart = 1;
        config.scan_input = 0;
        config.alternate_muxes = 0;
        config.autosample_wait = 0b11111;

        config.oneshot = 1;
        config.num_samples = 1;

        adc_configure(&config);
        adc_set_channel(1);
        _PCFG1 = 0;
        _TRISA1 = 1;
        val = adc_convert_one();

        sendf(U2, "Read: %d\r\n", val);
    }
    else if (strcmp(cmd, "cal") == 0)
    {
        ADCConfig config;
        unsigned int val;

        config.output_format = kUIntegerFormat;
        config.trigger = kInternalCounter;
        config.reference = kVDDVSS;
        config.enable_in_idle = 0;
        config.sample_autostart = 1;
        config.scan_input = 0;
        config.alternate_muxes = 0;
        config.autosample_wait = 0b11111;

        config.oneshot = 1;
        config.num_samples = 1;

        adc_configure(&config);
        adc_set_channel(1);
        _PCFG1 = 0;
        _TRISA1 = 1;
        _OFFCAL = 1;
        val = adc_convert_one();
        _OFFCAL = 0;

        sendf(U2, "Canibration: %d\r\n", val);
    }
    else if (strcmp(cmd, "read") == 0)
    {
        unsigned int i=0;
        sends(U2, "Dumping ADC buffer\r\n");
        for (i=0; i<kADCBufferSize; ++i)
        {
            sendf(U2, "Reading %d: %d\r\n", i, adc_buffer[i]);
        }
    }
    else if (strcmp(cmd, "disable") == 0)
    {
        adc_disable();
        sends(U2, "ADC Disabled\r\n");
    }
    else
        sendf(U2, "Unknown adc command: %s", cmd);
}

void handle_device(command_params *params)
{
    char *cmd;

    if (params->num_params < 1)
    {
        print( "You must pass a subcommand to the device command.\r\n");
        return;
    }

    cmd = get_param_string(params, 0);

    if (strcmp(cmd, "reset") == 0)
    {
        print( "Resetting the device...\r\n");
        asm_reset();
    }
    else if (strcmp(cmd, "rtype") == 0)
    {
            sendf(U2, "Last reset type: %d\r\n", last_reset_type());
    }
    else if (strcmp(cmd, "sleep") == 0)
    {
        print("Sleeping\r\n");
        asm_sleep();
    }
}

void handle_rtcc(command_params *params)
{
    char *cmd;

    if (params->num_params < 1)
    {
        print( "You must pass a subcommand to the rtcc command.\r\n");
        return;
    }

    cmd = get_param_string(params, 0);

    //Farm out the subcommand to our subhandlers

    if (strcmp(cmd, "status") == 0)
    {
        unsigned int enabled = rtcc_enabled();


        sendf(U2, "Realtime Clock Status: %s\r\n", enabled? "Enabled" : "Disabled");
        rtcc_datetime time;

        rtcc_get_time(&time);

        sendf(U2, "Current Time: %d/%d/%d %d:%d:%d\r\n", time.month, time.day, time.year, time.hours, time.minutes, time.seconds);
    }
    else if (strcmp(cmd, "set") == 0)
    {
        rtcc_datetime time_spec;
        char *date, *time;
        char temp[3];
        int m;

        temp[2] = '\0';

        if (params->num_params < 3)
        {
            print( "usage: rtcc set mm/dd/yy hh:mm:ss\r\n");
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
    else if (strcmp(cmd, "enable") == 0)
    {
        enable_rtcc();
        print("RTCC Enabled\r\n");
    }
    else
        sendf(U2, "Unknown rtcc command: %s\r\n", cmd);
}
