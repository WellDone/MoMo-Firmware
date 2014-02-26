#include "flashblock.h"

FBStatus fb_init(flash_block_info *info, unsigned int subsector, unsigned char size)
{
	FBStatus status;

	mem_read(MEMORY_SUBSECTION_ADDR(subsector), (BYTE*)info, sizeof(flash_block_info));

	if (info->magic != kFBMagic)
		status = kFBNewlyInitialized;
	else if (size != info->item_size || subsector != info->subsector)
		status = kFBDidNotMatch;
	else
		status = kFBAlreadyInitialized;

	//Check if we need to rewrite this block
	if (status != kFBAlreadyInitialized)
	{
		info->magic = kFBMagic;
		info->subsector = subsector;
		info->current = 0;
		info->item_size = size;
		info->bin_shift = fb_nextpow2(size);

		if (info->bin_shift < 2)
			info->bin_shift = 2;

		mem_clear_subsection(MEMORY_SUBSECTION_ADDR(subsector));
		mem_write_aligned(MEMORY_SUBSECTION_ADDR(subsector), (BYTE*)info, sizeof(flash_block_info));
	}

	//Update info with the offset to the latest version of the data
	fb_updatecurrent(info);
	return status;
}

int fb_count(flash_block_info *info)
{
	return info->current;
}

/*
 * flash_block stores a bitmap with one bit that is set from 1 to 0 every time a 
 * new version of the data is written.  Find the latest offset from the bitmap.
 */
void fb_updatecurrent(flash_block_info *info)
{
	unsigned char low = 0;
	unsigned char high = 63;
	uint16 bitmap_entry;
	unsigned int curr;
	uint32 addr = MEMORY_SUBSECTION_ADDR(info->subsector) + kFBBitMapOffset;

	//Check to make sure there is at least one entry set
	mem_read(addr, (BYTE*)&bitmap_entry, 2);
	if (bitmap_entry == 0xFFFF)
	{
		info->current = 0;
		return;
	}

	while(low <= high)
	{
		curr = (low + high) >> 1;
		addr = MEMORY_SUBSECTION_ADDR(info->subsector) + kFBBitMapOffset + (curr << 1);

		//Read in the bitmap in 16 bit segments and check if there is a 0->1 transition
		//in the segment.
		mem_read(addr, (BYTE*)&bitmap_entry, 2);

		if (bitmap_entry == 0xFFFF)
			high = curr-1;
		else if (bitmap_entry == 0x0000)
			low = curr + 1;
		else
			break;
	}

	//We've potentially found a 0->1 transition, set current to the offset of the word*16 + the offset of the
	//first 0 bit.
	info->current = (curr << 4) + (16-fb_countset(bitmap_entry));
	return;
}

/*
 * given a value, compute the next lowest power of two and convert it to
 * the number of bits it is shifted. This computes:
 * log2(round_up_to_pow2(val))
 * from: http://graphics.stanford.edu/~seander/bithacks.html
 */
unsigned char fb_nextpow2(unsigned char val)
{
	unsigned char r;
	unsigned char shift;

	r = 	(val > 0xF) << 2; val >>= r;
	shift = (val > 0x3) << 1; val >>= shift;

	r |= shift;
	r |= (val >> 1);
	r += 1;

	return r;
}

/*
 * Count the number of set bits in the passed value and return
 * the count.
 * from: http://graphics.stanford.edu/~seander/bithacks.html
 */
unsigned int fb_countset(unsigned int val)
{
	unsigned int c;

	for (c=0; val; c++)
		val &= val - 1;

	return c;
}

void fb_updatebitmap(flash_block_info *info)
{
	uint16 byte = info->current >> 3;
	uint32 addr = MEMORY_SUBSECTION_ADDR(info->subsector) + kFBBitMapOffset + byte;
	uint16 offset = info->current & 0b111;
	uint16 bm_value = ~((1 << offset) - 1); //needs to be 16 bit to properly 

	//If we just transitioned over a byte boundary, we need to clear the previous byte
	if (offset == 0 && byte > 0)
	{
		--addr;
		bm_value = 0;
	}

	mem_write_aligned(addr, (BYTE*)&bm_value, 1);
}

void fb_write(flash_block_info *info, const unsigned char *data)
{
	//Compute address of current 
	uint32 addr = info->current;
	addr <<= info->bin_shift;
	addr += kFBDataOffset;

	//If the block is full, erase it and start over, we can just check for equality because
	//the data bins are a power of 2 <= 256 so they evenly divide (MEMORY_SUBSECTION_SIZE - 256)
	if (addr == MEMORY_SUBSECTION_SIZE)
	{
		info->current = 0;
		addr = kFBDataOffset;

		mem_clear_subsection(MEMORY_SUBSECTION_ADDR(info->subsector));
		mem_write_aligned(MEMORY_SUBSECTION_ADDR(info->subsector), (BYTE*)info, sizeof(flash_block_info));
	}

	addr += MEMORY_SUBSECTION_ADDR(info->subsector);
	mem_write_aligned(addr, data, info->item_size);

	info->current++;	
	fb_updatebitmap(info);
}

void fb_read(flash_block_info *info, unsigned char *data)
{
	//Compute address of current - 1 (the last written data)
	//N.B. data read if the flash_block is empty will be nonsense.

	uint32 addr = info->current-1;
	addr <<= info->bin_shift;
	addr += kFBDataOffset;
	addr += MEMORY_SUBSECTION_ADDR(info->subsector);

	mem_read(addr, data, info->item_size);
}