/*
 * mib12_api.h
 * Version 1.0 (5/13/2015)
 * The API provided by the pic12_executive for use by application modules.
 *
 * This file defines the API functions that application modules can call
 * as well as the shared data structures used for communication between
 * the executive and application routines.
 *
 * Interaction between the executive and application modules proceeds in two ways.
 * First, the executive can call application MIB endpoints in interrupt mode when
 * the module receives the appropriate RPC call.  Second, the application can call
 * into the executive from mainline or interrupt code to perform certain executive
 * provided functions like sending an RPC call, resetting the device or trapping an
 * error.
 *
 * The specific RAM locations for the the shared data structures and ROM locations
 * for the shared API functions are stored in the associated mib12_api.as and must be 
 * included in the compilation fo any mib12 module.
 */

#ifndef __mib12_api_h__
#define __mib12_api_h_

#include "protocol.h"

// MIB12 API Functions 
void  	trap(uint8 code);
void  	reset_device();
void  	bus_master_begin_rpc(uint8_t address);
uint8_t bus_master_send_rpc(uint8_t param_length);
void  	bus_slave_returndata(uint8_t length);

// MIB12 API Data Structures Shared Between Executive and Application
extern unsigned char 		mib_buffer[kBusMaxMessageSize];
extern MIBCommandPacket   	mib_packet;
extern uint8_t              slave_address;

#endif