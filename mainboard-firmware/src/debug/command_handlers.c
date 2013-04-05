#include <xc.h>
#include <stdio.h>
#include <string.h>
#include "serial_commands.h"
#include "rtcc.h"
#include "utilities.h"
#include "sensor.h"
#include "memory.h"
#include "oscillator.h"
#include "gsm.h"
#include "adc.h"
#include <stdio.h>
#include <string.h>
#include "scheduler.h"
#include "reset_manager.h"
#include "sensor_event_log.h"
#include "report_manager.h"
#include "momo_config.h"
#include "registration.h"

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
            num_samples = atoi(get_param_string(params, 1));
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

void handle_gsm(command_params *params)
{
    char *cmd;

    if (params->num_params < 1)
    {
        print( "You must pass a subcommand to the device command.\r\n");
        return;
    }

    cmd = get_param_string(params, 0);

    if (strcmp(cmd, "module") == 0)
    {
        if (params->num_params < 2)
        {
            print( "usage: gsm module [on|off]\r\n");
            return;
        }

        if (strcmp(get_param_string(params, 1),"on") == 0)
        {
            if ( gsm_on() ) {
                print( "GSM module turned on.\r\n");
            } else {
                print( "FAILED!\r\n" );
            }
        }
        else if (strcmp(get_param_string(params, 1),"off") == 0)
        {
            gsm_off();
            print( "GSM module turned off.\r\n");
        }
        else
            print( "Invalid option to gsm module command.\r\n");
    }
    else if (strcmp(cmd, "cmd") == 0)
    {
        if (params->num_params >= 2)
        {
            gsm_send_at_cmd(get_param_string(params,1));
            print( "command sent\r\n");
        }
        else
            print( "usage: gsm cmd <cmd>\r\n");
    }
    else if (strcmp(cmd, "msg") == 0)
    {
        if (params->num_params == 3)
        {
            gsm_send_sms( get_param_string(params, 1), get_param_string(params, 2) ); //TODO: Allow multi-word messages.  "quoted string" support?
        }
        else
            print( "usage: gsm msg <address> <message>\r\n");
    }
    else if (strcmp(cmd, "dump") == 0)
    {
        dump_gsm_buffer();
    }
    else if (strcmp(cmd, "status") == 0)
    {
        print_byte( gsm_status() );
    }
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

void handle_sensor(command_params *params) {
  sensor_event* events;
  /*  sendf(U2, "Sensor start : SENSOR_TO = 16%x\r\n", SENSOR_TO);
  asm_sleep();
  while(SENSOR_FLAG);
  sendf(U2, "%d pulses\r\n", pulse_counts);
  read_sensor_events(events, 1); */
}

static BYTE memory_buffer[32];
void handle_memory(command_params *params) {
    char* cmd;
    char* data;
    int l = 0, addr = 0;
    cmd = get_param_string(params, 0);
    if (strcmp(cmd, "write") == 0) {
      atoi_small( get_param_string(params, 1), &addr );
      data = get_param_string(params,2);

      mem_write(addr, (unsigned char*)data, strlen(data));
    } else if (strcmp(cmd, "read") == 0) {
      if ( !atoi_small( get_param_string(params, 1), &addr ) ||
           !atoi_small( get_param_string(params, 2), &l) ) {
        print( "atoi failed!\r\n");
        return;
      }
      l &= 0x1F;
      mem_read(addr, memory_buffer, l);
      memory_buffer[l] = 0x0;

      print((const char*)memory_buffer);
      print("\r\n");
    } else if (strcmp(cmd, "erase") == 0) {
        mem_clear_all();
    } else if (strcmp(cmd, "status") == 0) {
        print_byte( mem_status() );
    } else if (strcmp(cmd, "test") == 0) {
        print("Testing flash memory SPI communication...\r\n");
        if ( !mem_test() )
            print( "SPI test FAILED!!\r\n" );
        else
            print( "SPI test SUCCEEDED!!\r\n" );
    } else {
        print( "Unrecognized memory command!\r\n");
    }
}

void handle_log(command_params *params) {
    char val[5];
    int value;
    rtcc_datetime time;
    if (params->num_params != 1) {
        print("usage: log read/empty?/count/<value>\r\n");
        return;
    }
    const char* p = get_param_string(params,0);
    if (strcmp(p, "read") == 0) {
        while ( !sensor_event_log_empty() ) {
            sensor_event event;
            if ( read_sensor_events( &event, 1 ) == 0 ) {
                print("No items in log\r\n");
                return;
            }
            val[0] = val[1] = val[2] = val[3] = '0';
            val[itoa_small( val, 4, event.value)] = '\0';
            print("Read value: ");
            print( val );
            print( "\r\n");
        }
        return;
    }
    if (strcmp(p, "empty?") == 0) {
        if ( sensor_event_log_empty() ) {
            print( "yes\r\n");
        } else {
            print( "no\r\n");
        }
        return;
    }
    if (strcmp(p, "count") == 0) {
        val[0] = val[1] = val[2] = val[3] = '0';
        val[itoa_small( val, 4, sensor_event_log_count() )] = '\0';
        print( val );
        print( "\r\n");
        return;
    }

    if ( !atoi_small( p, &value ) ) {
        print( "atoi failed!\r\n" );
        return;
    }
    rtcc_get_time(&time);
    log_sensor_event( momo_pulse_counter, &time, value );
}

void handle_registration( command_params *params ) {
    if ( params->num_params == 0 ) {
        if ( current_momo_state.registered ) {
            print( "registered\r\n" );
        } else {
            print( "NOT registered\r\n" );
        }
    }
    else if ( strcmp( get_param_string( params, 0 ), "do" ) == 0 )
    {
        if (!momo_register_and_start_reporting())
        {
            print( "FAILED!\r\n" );
        }
    }
    else if ( strcmp( get_param_string( params, 0 ), "clear" ) == 0 )
    {
        current_momo_state.registered = false;
        save_momo_state();
    }
    else
    {
        print( "Unrecognized registration command.\r\n" );
    }
}

void handle_report(command_params *params) {
    post_report();
}

void handle_exit(command_params *) {
    debug_disable_unconnected();
}
