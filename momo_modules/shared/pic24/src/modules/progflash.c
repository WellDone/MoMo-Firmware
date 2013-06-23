#include "progflash.h"

void progflash_setop(ProgFlashOperation type)
{
	_NVMOP = type;

	if (type == kWrite1Row)
		progflash_set_write();
	else
		progflash_set_erase();
}

void progflash_execute()
{
	asm volatile("DISI #5");
	__builtin_write_NVM();

	while(!progflash_op_finished())
		;
}

void progflash_loadlatches(unsigned int offset, unsigned int *buffer, unsigned int word_len)
{
	unsigned int i;

	for (i=0; i<word_len; i+=2)
	{
		__builtin_tblwtl(offset, buffer[i]);
		__builtin_tblwth(offset, buffer[i+1]);

		offset += 2;
	}
}

void progflash_erase(unsigned char page, unsigned int offset, ProgFlashOperation op)
{
	progflash_setop(op);
	progflash_setpage(page);

	__builtin_tblwtl(offset, 0x0000); //dummy write to load the address

	progflash_execute();
}