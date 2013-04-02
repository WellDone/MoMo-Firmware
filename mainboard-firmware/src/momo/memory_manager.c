#include "memory_manager.h"
#include "memory.h"

#include "momo_config.h"
#include "sensor_event_log.h"

typedef struct {
  unsigned long start_address;
  unsigned long end_address;
} memory_block;

static inline memory_block create_block( unsigned int subsection_index, unsigned int subsection_length ) {
  memory_block block;
  block.start_address = MEMORY_SUBSECTION_ADDR(subsection_index);
  block.end_address = MEMORY_SUBSECTION_SIZE*subsection_length+block.start_address;
  return block;
}
#define BLOCK_SIZE(block) (block.end_address-block.start_address)

void flash_memory_init() {
  configure_SPI();

  //memory_block meta_memory_block = create_block( 0, 1 );
  init_momo_config( 0 ); // Currently accepts a subsection index, not an address

  memory_block log_memory_block = create_block( 1, MEMORY_SUBSECTION_NUM(mem_capacity())-1 );
  init_sensor_event_log( log_memory_block.start_address,  BLOCK_SIZE(log_memory_block) );
}