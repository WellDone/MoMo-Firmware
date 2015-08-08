//Name: test_flash_queue
//Type: module
//Sources:../../portable

#include "flash_queue.h"

#define kMemorySize		(1024ULL*4ULL)

unsigned char mem[kMemorySize];

void setUp(void) 
{
	uint32_t i;

	for (i=0; i<kMemorySize; ++i)
		mem[i] = 0xFF;
}

