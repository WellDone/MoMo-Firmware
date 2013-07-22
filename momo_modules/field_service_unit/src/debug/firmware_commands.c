#include "firmware_commands.h"
#include "debug_utilities.h"
#include "mib_features.h"
#include "intel_hex.h"
#include "bus_master.h"
#include "utilities.h"
#include "intel_hex.h"

static intel_hex16 firmwareChunk;
//TODO: Cleanup
bool firmware_populated = false;
bool waiting_for_uart = false;
bool firmware_done = false;

//LONG MESSAGE UART UTILITIES
static void request_firmware_line();
static void push_more_firmware();
static void parse_firmware_line(char* buf, int len, bool overflown)
{
    if (len == 1 && *buf == EOT)
    {
        firmware_done = true;
        set_command_result( true );
        return;
    }
    if ( !compress_intel_hex16_ascii( (intel_hex16_ascii*)buf, &firmwareChunk ) )
    {
        buf[len] = '\0';
        print( "Bad HEX line (" );
        print( buf );
        print( ")\n" );
        set_command_result( false );
        return;
    }

    firmware_populated = true;
    if ( waiting_for_uart ) {
        push_more_firmware();
    }
    waiting_for_uart = false;
}
static void request_firmware_line()
{
    put(DEBUG_UART, SYN);
    getln(parse_firmware_line);
}

// I2C FIRMWARE PUSH
static void push_firmware_callback(unsigned char a)
{
    // TODO: Retry on failure?
    if (firmware_done) {
        put(DEBUG_UART, ACK);
        return;
    }

    if (firmware_populated)
        push_more_firmware();
    else
        waiting_for_uart = true;
}
static void push_more_firmware()
{
    if (firmware_done) {
        //DO NOTHING
    } else {
        bus_master_compose_params( plist_define1(kMIBBufferType) );
        memcpy( get_buffer_loc( 0 ), (char*)&firmwareChunk.body, (unsigned int)sizeof(intel_hex16_body) );
        bus_master_rpc_async( push_firmware_callback, kControllerPICAddress, MIB_FEATURE_ID(firmware_cache), 0x04 );

        firmware_populated = false;
        request_firmware_line();
    }
}

CommandStatus handle_push_firmware(command_params* params)
{
    if ( params->num_params != 2 ) {
        print( "USAGE: push-firmware <module_type> <firmware_length>\n" );
        return kFailure;
    }

    int type;
    int firmware_length;
    if ( !atoi_small( get_param_string( params, 0 ), &type ) || type < 0
      || !atoi_small( get_param_string( params, 1 ), &firmware_length ) || firmware_length < 0 ) {
        print( "Invalid module type or firmware length.\n" );
        return kFailure;
    }
    firmware_populated = false;
    firmware_done = false;
    waiting_for_uart = false;
    request_firmware_line();

    bus_master_compose_params( plist_define2(kMIBInt16Type, kMIBInt16Type) );
    set_intparam( 0, type );
    set_intparam( 1, firmware_length );
    bus_master_rpc_async( push_firmware_callback, kControllerPICAddress, MIB_FEATURE_ID(firmware_cache), 0x03 );
    return kPending;
}

// PULL FIRMWARE
static uint8  current_index;
static uint16 current_offset;
static char asciiFirmwareChunk[sizeof(intel_hex16_ascii)+1];
void pull_firmware_callback(unsigned char a)
{
    expand_intel_hex16_binary( (intel_hex16*)get_buffer_param(0), (intel_hex16_ascii*)asciiFirmwareChunk );
    //TODO: HOW DO WE GET BACK RETURN VALUES!?!
    asciiFirmwareChunk[sizeof(asciiFirmwareChunk)] = '\0';
    print( asciiFirmwareChunk );

    if (false) //TODO: Basecase
    {
        bus_master_compose_params( plist_define2(kMIBInt16Type, kMIBInt16Type) );
        set_intparam( 0, current_index );
        set_intparam( 1, current_offset );
        bus_master_rpc_async( pull_firmware_callback, kControllerPICAddress, MIB_FEATURE_ID(firmware_cache), 0x04 );
    } else {
        set_command_result(true);
    }
}
CommandStatus handle_pull_firmware(command_params* params)
{
    if ( params->num_params != 1 ) {
        print( "USAGE: pull-firmware <index>" );
        return kFailure;
    }

    int index;
    if ( !atoi_small( get_param_string( params, 0 ), &index ) || index < 0 ) {
        print( "Invalid module type or firmware length." );
        return kFailure;
    }

    current_index = index&0xFF;
    current_offset = 0;
    bus_master_compose_params( plist_define2(kMIBInt16Type, kMIBInt16Type) );
    set_intparam( 0, current_index );
    set_intparam( 1, current_offset );
    bus_master_rpc_async( pull_firmware_callback, kControllerPICAddress, MIB_FEATURE_ID(firmware_cache), 0x04 );
    return kPending;
}