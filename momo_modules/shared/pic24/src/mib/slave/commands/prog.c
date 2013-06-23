#include "bus_slave.h"
#include "memory.h"
#include "prog.h"
#include "bootloader.h"
/*
 * Erase the pic24 program portion of the flash memory
 *
 */

void *erase_primaryfirmware(MIBParamList *param)
{
	mem_clear_subsection(kPic24FirmwareSubsection1);
	mem_clear_subsection(kPic24FirmwareSubsection2);
	mem_clear_subsection(kPic24FirmwareSubsection3);
	mem_clear_subsection(kPic24FirmwareSubsection4);
	mem_clear_subsection(kPic24FirmwareSubsection5);

	bus_slave_setreturn(kNoMIBError, 0);
	return NULL;
}

void* load_into_nvram(MIBParamList *list)
{
	unsigned long page = get_uint16_param(list, 0);
	unsigned long offset = get_uint16_param(list, 1);
	MIBBufferParameter *buf = get_buffer_param(list, 2);

	unsigned long addr = (page << 16) + offset;

	mem_write(addr, buf->data, buf->header.len);

	bus_slave_setreturn(kNoMIBError, 0);
	return NULL;
}

void* read_from_nvram(MIBParamList *list)
{
	unsigned long page = get_uint16_param(list, 0);
	unsigned long offset = get_uint16_param(list, 1);
	unsigned int len = get_uint16_param(list, 2);

	unsigned char *retbuffer;
	MIBParameterHeader *retval;

	unsigned long addr = (page << 16) + offset;

	bus_free_all();
	retval = bus_allocate_return_buffer(&retbuffer);

	mem_read(addr, retbuffer, len);
	retval->len = len;

	bus_slave_setreturn(kNoMIBError, (volatile MIBParameterHeader*)retval);
	return NULL;
}