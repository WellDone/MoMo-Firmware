//protocol.c

#include "protocol.h"

extern volatile unsigned char 	mib_buffer[kBusMaxMessageSize];
extern unsigned int 			mib_firstfree;

volatile unsigned char *	bus_allocate_space(unsigned int len)
{
	volatile unsigned char *allocated; 

	if ((kBusMaxMessageSize - mib_firstfree) < len)
		return (unsigned char *)0;

	allocated = &mib_buffer[mib_firstfree];
	
	mib_firstfree += len;

	return allocated;
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

void bus_init_buffer_param(MIBBufferParameter *param, void *data, unsigned char len)
{
	param->header.type = kMIBBufferType;
	param->header.len = len;
	param->data = data;
}

volatile MIBReturnValueHeader *bus_allocate_return_status()
{
	volatile MIBReturnValueHeader *ret = (volatile MIBReturnValueHeader *)bus_allocate_space(sizeof(MIBReturnValueHeader));

	ret->result = 0;
	ret->len = 0;

	return ret;
}

volatile MIBIntParameter *bus_allocate_int_param()
{
	volatile MIBIntParameter *param = (volatile MIBIntParameter *)bus_allocate_space(sizeof(MIBIntParameter));

	if (param == 0)
		return 0;

	param->header.type = kMIBInt16Type;
	param->header.len = sizeof(MIBIntParameter) - sizeof(MIBParameterHeader);

	return param;
}

volatile MIBBufferParameter *bus_allocate_buffer_param(unsigned int len)
{
	volatile MIBBufferParameter *param = (volatile MIBBufferParameter *)bus_allocate_space(sizeof(MIBBufferParameter)+len);

	if (param == 0)
		return 0;

	param->header.type = kMIBBufferType;
	param->header.len = len;
	param->data = ((unsigned char*)param) + sizeof(MIBBufferParameter);

	return param;
}

volatile MIBParamList *bus_allocate_param_list(unsigned int num)
{
	volatile MIBParamList *list = (volatile MIBParamList*)bus_allocate_space(sizeof(MIBParamList) + sizeof(MIBParameterHeader*)*num);

	if (list == 0)
		return 0;

	list->count = num;
	list->curr = 0;

	return list;
}