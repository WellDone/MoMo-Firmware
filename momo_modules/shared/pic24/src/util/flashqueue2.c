#include "flashqueue2.h"

#ifndef __NO_FLASH__

void fq_init(fq_data *queue, uint8_t version, uint8_t start_subsection, uint8_t element_size, uint8_t subsection_count)
{
	uint16_t combined_version = (kFlashQueue2StructureVersion << 8) | version;

	queue->start_address	= MEMORY_SUBSECTION_ADDR(start_subsection+1);
    queue->element_size 	= element_size;
    queue->element_shift	= fb_nextpow2(element_size);
    queue->length 			= ((subsection_count-1)*MEMORY_SUBSECTION_SIZE) >> queue->element_shift;

    fb_init(&queue->state_block, start_subsection, sizeof(fq_state), combined_version);
    if (fb_count(&queue->state_block) > 0)
		fb_read(&queue->state_block, &queue->counters);
    else
		fq_clear(queue);

}

void fq_clear(fq_data *queue)
{
	queue->counters.start = 0;
	queue->counters.item_count = 0;

	fb_write(&queue->state_block, &queue->counters);
}

uint32_t fq_count(const fq_data *queue)
{
	return queue->counters.item_count;
}

bool fq_full(const fq_data *queue)
{
	return fq_count == queue->length;
}

uint32_t fq_wrapindex(const fq_data *queue, uint32_t index)
{
	return index % queue->length;
}

uint32_t fq_address(const fq_data *queue, uint32_t index)
{
	index = fq_wrapindex(index);

	return queue->start_address + (index << queue->element_shift);
}

uint32_t fq_index(const fq_data *queue, uint32_t address)
{
	uint32_t index = (address - queue->start_address) >> queue->element_size;

	return fq_wrapindex(queue, index);
}

void fq_queue(fq_data *queue, const void *data)
{
	uint32_t address = fq_address(queue, queue->counters.item_count);

	if (fq_full(queue))
	{
		uint32_t new_start_address = MEMORY_ADDR_SUBSECTION_ADDR(address) + MEMORY_SUBSECTION_SIZE;
		uint32_t delta_address = new_start_address - fq_address(queue, queue->start);
		uint32_t delta = delta_address >> queue->element_shift;

		queue->start = fq_wrapindex(queue, queue->start + delta);
		queue->item_count -= delta;
	}

	
}

#endif
