//bus.c

//This is where we declare all the MIB state, so don't pull in the external definitions
#define __NO_EXTERN_MIB_STATE__

#include "bus.h"

MIBState 				mib_state;
unsigned char 			mib_buffer[kBusMaxMessageSize];
unsigned int 			mib_firstfree;

//These functions are too small to be efficiently used on the PIC12 which must pass parameters
#ifndef _MACRO_SMALL_FUNCTIONS
void bus_send(unsigned char address, unsigned char *buffer, unsigned char len, unsigned char flags)
{
	//Fill in message contents
	mib_state.bus_msg.address = address;
	mib_state.bus_msg.data_ptr = buffer;
	mib_state.bus_msg.last_data = buffer + len;
	mib_state.bus_msg.flags = flags;

	i2c_send_message();
}

void bus_receive(unsigned char address, unsigned char *buffer, unsigned char len, unsigned char flags)
{
	mib_state.bus_msg.address = address;
	mib_state.bus_msg.data_ptr = buffer;
	
	mib_state.bus_msg.last_data = buffer;
	mib_state.bus_msg.len = len;

	mib_state.bus_msg.flags = flags;

	i2c_receive_message();
}

void bus_free_all()
{
	mib_firstfree = 0;
}

void bus_init_int_param(MIBIntParameter *param, int value)
{
	param->header.type = kMIBInt16Type;
	param->header.len = 2;
	param->value = value;
}

void bus_init_buffer_param(MIBBufferParameter *param, void *data, uint8 len)
{
	param->header.type = kMIBBufferType;
	param->header.len = len;
	param->data = data;
}

#endif

#define bus_allocate_space(len)		mib_buffer+mib_firstfree; mib_firstfree += len;


MIBParameterHeader *bus_allocate_return_buffer(unsigned char **out_buffer)
{
	unsigned char len = kBusMaxMessageSize - mib_firstfree;
	MIBParameterHeader *ret = (MIBParameterHeader *)bus_allocate_space(len);

	ret->type = kMIBBufferType;
	ret->len = len - sizeof(MIBParameterHeader);

	*out_buffer = ((unsigned char*)ret) + sizeof(MIBParameterHeader);

	return ret;
}

MIBIntParameter *bus_allocate_int_param()
{
	MIBIntParameter *param = (MIBIntParameter *)bus_allocate_space(sizeof(MIBIntParameter));

	bus_init_int_param(param, 0);

	return param;
}

MIBBufferParameter *bus_allocate_buffer_param(uint8 len)
{
	MIBBufferParameter *param;

	//If len == 0, allocate all remaining space
	if (len == 0)
		len = kBusMaxMessageSize - mib_firstfree - sizeof(MIBBufferParameter);

	param = (MIBBufferParameter *)bus_allocate_space(sizeof(MIBBufferParameter)+len);

	bus_init_buffer_param(param, ((unsigned char*)param) + sizeof(MIBBufferParameter), len);

	return param;
}

MIBParamList *bus_allocate_param_list(uint8 num)
{
	MIBParamList *list = (MIBParamList*)bus_allocate_space(sizeof(MIBParamList) + sizeof(MIBParameterHeader*)*num);

	if (list == 0)
		return 0;

	list->count = num;
	list->curr = 0;

	return list;
}