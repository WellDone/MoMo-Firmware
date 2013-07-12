//command_list.h

#ifndef __command_list_h__
#define __command_list_h__

#define kMIBEndpointAddress		0x7FA		//the last 6 bytes of program memory
#define kMIBMagicNumber			0xAA

typedef struct
{
	unsigned char num_features;

	unsigned char feature_list;	//goto
	unsigned char cmd_list;		//goto
	unsigned char param_specs; //another goto

	unsigned char jumppoint; //a goto instruction with the location of the 


	unsigned char magic;
} MIBEndpointInfo;

/*
 * The pic12 skeleton code looks for a MIBEndpointInfo structure
 * at a specific address at the top of program memory and uses this 
 * to find handlers.  If there is no application code loaded, the magic
 * number will be wrong and MIB will not load any endpoints.
 */
const MIBEndpointInfo rpc_info @ kMIBEndpointAddress;

//external assembly functions for getting application code rpc information
extern uint8 get_num_features();
extern uint8 get_feature(uint8 feature);
extern uint8 get_command(uint8 command);
extern uint8 get_spec(uint8 index);
extern uint8 validate_params(uint8 index);
extern uint8 loadparams(uint8 spec);
extern void  call_handler(uint8 handler_index);
extern uint8 get_magic();

#endif