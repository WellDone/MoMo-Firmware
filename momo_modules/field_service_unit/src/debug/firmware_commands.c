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
    if ( !compress_intel_hex16_ascii( (intel_hex16_ascii*)buf, &firmwareChunk, len ) )
    {
        buf[len] = '\0';
        print( "Bad HEX line (" );
        print( buf );
        print( ")\n" );

        // CANCEL.  TODO: Wait for return before indicating failure on UART
        bus_master_compose_params( plist_define0() );
        bus_master_rpc_async( NULL, kControllerPICAddress, MIB_FEATURE_ID(firmware_cache), 0x02 );

        set_command_result( false );
        return;
    }

    push_more_firmware();
}
static void request_firmware_line()
{
    put(DEBUG_UART, SYN);
    getln(parse_firmware_line);
}

// I2C FIRMWARE PUSH
static void push_firmware_callback(unsigned char a)
{
    if ( !firmware_done )
        request_firmware_line();
    else
        set_command_result( true );
}
static void push_more_firmware()
{
    if ( firmwareChunk.body.record_type == HEX_EOF_REC )
        firmware_done = true;
    bus_master_compose_params( plist_define1(kMIBBufferType) );
    memcpy( get_buffer_loc( 0 ), (char*)&firmwareChunk.body, (unsigned int)sizeof(intel_hex16_body) );
    bus_master_rpc_async( push_firmware_callback, kControllerPICAddress, MIB_FEATURE_ID(firmware_cache), 0x01 );
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
    firmware_done = false;

    bus_master_compose_params( plist_define3(kMIBInt16Type, kMIBInt16Type, kMIBInt16Type) );
    set_intparam( 0, type );
    set_intparam( 1, 0 ); // 0 HIGH BITS of firmware_length
    set_intparam( 2, firmware_length );
    bus_master_rpc_async( push_firmware_callback, kControllerPICAddress, MIB_FEATURE_ID(firmware_cache), 0x00 );
    return kPending;
}

// PULL FIRMWARE
static uint8  current_index;
static uint16 current_offset;
static char asciiFirmwareChunk[sizeof(intel_hex16_ascii)+1];
void pull_firmware_callback(unsigned char a)
{
    //expand_intel_hex16_binary( (intel_hex16*)get_buffer_param(0), (intel_hex16_ascii*)asciiFirmwareChunk );
    //TODO: HOW DO WE GET BACK RETURN VALUES!?!
    //asciiFirmwareChunk[sizeof(asciiFirmwareChunk)] = '\0';
    //print( asciiFirmwareChunk );

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