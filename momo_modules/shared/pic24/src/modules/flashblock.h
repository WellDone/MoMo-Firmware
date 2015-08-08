#ifndef __flashblock_h__
#define __flashblock_h__

#include "memory.h"
#include <stdint.h>

#ifndef __NO_FLASH__

/*
 * flashblock - a flash memory structure for efficiently saving a frequently
 * changing small item.  Since flash memory cannot be easily erased this module
 * saves a log of many copies of the data that is to be stored along with a bitmap
 * that indicates the latest valid version of the data.  It is persistent across
 * a power loss and the data is always recoverable from the flash.  It uses one complete
 * flash subsector (4kb) divided as follows:
 * 10 bytes 	- header
 * 118 bytes 	- reserved (currently unused)
 * 128 bytes 	- bitmap of written locations
 * 3840 bytes 	- log of data entries
 *
 * The data structure works because you can only ever clear bits from 1 to 0 without a
 * time consuming erase cycle.  So, the bitmap starts as all ones and each time a new
 * version of the data item is written to the data section, the next bit is cleared
 * in the bitmap.  The offset of the current data is then easily found by simply finding
 * the index of the last 0 bit in the bitmap, which can be done efficienty using binary
 * search. The module caches the current location in RAM so the binary search only needs
 * to be done once upon power-up to find the last stored location.  Once the subsection is
 * filled, the next write will erase the data, before writing a new copy.
 *
 * Note that the value of flash_block_info->current in flash is not meaningful since you
 * cannot update it.  It is in the data structure so it's easy to read it from Flash and 
 * then is updated in RAM to the correct value using fb_updatecurrent().
 *
 * Features and Limitations: 
 * 1. Data items must be uniformly sized and must have a size less than 256 bytes. 
 * 2. Internally, data items are rounded up to the next power of 2 for easy storage, but
 *    this does not affect the user.  User buffers are only ever read or written in the
 *    size that the user specifies.  The rest of the bucket is unused data.
 * 3. This module should be robust over power outages at any point.  At worst, the last data
	  item will not be saved since the bitmap will not have been updated after it was written
 * 4. A power loss during the period when the block is full and in the proces of clearing itself
  	  will lose all data but the module will recover and reinitialize the block after power is
  	  regained.  All data will be lost however. 
 */

#define kFBMagic			0xFABC
#define kFBBitMapOffset		128ULL
#define kFBDataOffset		256ULL

#define kFBSentinel			0xFFFF

//10 bytes
typedef struct
{
	unsigned int 	magic;
	unsigned int 	subsector;
	unsigned int 	current;
	unsigned int 	version;

	unsigned char 	item_size;
	unsigned char	bin_shift;
} flash_block_info;

typedef enum
{
	kFBNewlyInitialized = 0,
	kFBAlreadyInitialized = 1,
	kFBDidNotMatch = 2
} FBStatus;

FBStatus 		fb_init(flash_block_info *info, unsigned int subsector, unsigned char size, unsigned int version);
void 			fb_write(flash_block_info *info, const void *data);
void 			fb_read(flash_block_info *info, void *data);
int 			fb_count(flash_block_info *info);

//Utility functions for internal use
unsigned char	fb_nextpow2(unsigned char val);
unsigned int	fb_countset(unsigned int val);
void 			fb_updatecurrent(flash_block_info *info);
void 			fb_updatebitmap(flash_block_info *info);

#endif
#endif