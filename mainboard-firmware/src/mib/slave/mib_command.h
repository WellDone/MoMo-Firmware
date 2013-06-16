#ifndef __mib_command_h__
#define __mib_command_h__

enum
{
	kMIBCreateParameters = 1,
	kMIBExecuteCommand = 2
};

//Callback Type
typedef void* (*mib_callback)(int, void *);

#define kInvalidMIBHandler  (mib_callback)0

mib_callback find_handler(unsigned char feature, unsigned char cmd);

//supported commands
void* test_command(int state, void *param);
void* echo_buffer(int state, void *param);
#endif