#include "flashqueue2.h"

#ifndef __NO_FLASH__

static uint32_t 	fq_wrapindex(const fq_data *queue, uint32_t index);
static uint32_t 	fq_address(const fq_data *queue, uint32_t index);


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
	return queue->counters.item_count == queue->length;
}

uint32_t fq_wrapindex(const fq_data *queue, uint32_t index)
{
	return index % queue->length;
}

uint32_t fq_address(const fq_data *queue, uint32_t index)
{
	return queue->start_address + (index << queue->element_shift);
}

void fq_push(fq_data *queue, const void *data)
{
	uint32_t next_index = fq_wrapindex(queue, queue->counters.start + queue->counters.item_count);
	uint32_t address = fq_address(queue, next_index);

	//If we're starting a new subsection, make sure we clear it first and check if we need to
	//move the start pointer because it was located in the subsection that we just cleared
	if (MEMORY_SUBSECTION_OFFSET(address) == 0)
	{
		//We only need to potentially move the start pointer if there is data stored
		if (queue->counters.item_count != 0)
		{
			uint32_t start_index = next_index;
			uint32_t end_index = start_index + (MEMORY_SUBSECTION_SIZE >> queue->element_shift); //don't wrap so end_index > start_index
		
			//If start pointer was in this subsection, move it to the beginning of the next subsection, wrapping if necessary
			if (queue->counters.start >= start_index && queue->counters.start < end_index)
			{
				uint32_t delta = end_index - queue->counters.start;
				queue->counters.start = fq_wrapindex(queue, queue->counters.start + delta);
				queue->counters.item_count -= delta;
			}
		}

		mem_clear_subsection(address);
	}

	mem_write_aligned(address, data, queue->element_size);
	++queue->counters.item_count;

	fb_write(&queue->state_block, &queue->counters);
}

bool fq_pop(fq_data *queue, void *out)
{
	if (fq_count(queue) == 0)
		return false;

	if (out != NULL)
	{
		uint32_t address = fq_address(queue, queue->counters.start);
		mem_read(address, out, queue->element_size);
	}

	queue->counters.item_count -= 1;
	queue->counters.start = fq_wrapindex(queue, queue->counters.start + 1);

	fb_write(&queue->state_block, &queue->counters);
	return true;
}

uint32_t fq_popn(fq_data *queue, void *out, uint32_t max)
{
	uint32_t i;
	char *out_ptr = (char *)out;

	for (i=1; i<=max; ++i)
	{
		if (fq_pop(queue, out_ptr) == false)
			return i;

		if (out != NULL)
			out_ptr += queue->element_size;
	}

	return i;
}

void fqwalker_init(fq_walker_data *walker, fq_data *queue, uint32_t offset)
{
	walker->queue = queue;
	walker->offset = offset;
}

bool fqwalker_next(fq_walker_data *walker, void *out)
{
	uint32_t index;
	uint32_t address;

	if (walker->offset > walker->queue->counters.item_count)
		return false;

	index = fq_wrapindex(walker->queue, walker->queue->counters.start + walker->offset);
	address = fq_address(walker->queue, index);

	if (out != NULL)
		mem_read(address, out, walker->queue->element_size);

	walker->offset += 1;
	return true;
}


#endif
