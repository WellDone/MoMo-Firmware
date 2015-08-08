#ifndef __flashqueue2_h__
#define __flashqueue2_h__

#include "platform.h"
#include "flashblock.h"
#include <stdint.h>

#ifndef __NO_FLASH__

#define kFlashQueue2StructureVersion   1

typedef struct {
	uint32_t   	start; 			//index of the first valid item
	uint32_t   	item_count;
} fq_state;

typedef struct 
{
	uint8_t					element_size;
	uint8_t					element_shift;
	uint32_t				start_address;
	uint32_t				length;

	flash_block_info		state_block;
	fq_state				counters;
} fq_data;

//subsection_count must be >= 2
void fq_init(fq_data *queue, uint8_t version, uint8_t start_subsection, uint8_t element_size, uint8_t subsection_count);
void fq_clear(fq_data *queue);

#endif

#endif