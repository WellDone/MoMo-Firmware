#include "bus_slave.h"
#include "memory.h"
#include "prog.h"
#include "bootloader.h"

#include "mib_features.h"
/*
 * Erase the pic24 program portion of the flash memory
 *
 */

void erase_primaryfirmware(void)
{
	mem_clear_subsection(kPic24FirmwareSubsection1);
	mem_clear_subsection(kPic24FirmwareSubsection2);
	mem_clear_subsection(kPic24FirmwareSubsection3);
	mem_clear_subsection(kPic24FirmwareSubsection4);
	mem_clear_subsection(kPic24FirmwareSubsection5);

	bus_slave_setreturn(kNoMIBError);
}

void load_into_nvram(void)
{
	unsigned long page = get_uint16_param(0);
	unsigned long offset = get_uint16_param(1);
	MIBBufferParameter *buf = get_buffer_param(2);

	unsigned long addr = (page << 16) + offset;

	mem_write(addr, buf->data, buf->header.len);

	bus_slave_setreturn(kNoMIBError);
}

void read_from_nvram(void)
{
	unsigned long page = get_uint16_param(0);
	unsigned long offset = get_uint16_param(1);
	unsigned int len = get_uint16_param(2);

	MIBBufferParameter *retval;

	unsigned long addr = (page << 16) + offset;

	loadparams(plist_1param(kMIBBufferType));
	retval = get_buffer_param(0);

	mem_read(addr, mib_buffer+2, len);
	retval->header.len = len;

	bus_slave_setreturn(kNoMIBError | kHasReturnValue);

}

DEFINE_MIB_FEATURE_COMMANDS(programming)
{
	{0, erase_primaryfirmware, plist_define0() },
	{1, load_into_nvram, plist_define3(kMIBInt16Type, kMIBInt16Type, kMIBBufferType) },
	{2, read_from_nvram, plist_define3(kMIBInt16Type, kMIBInt16Type, kMIBInt16Type) }
};
DEFINE_MIB_FEATURE(programming);
