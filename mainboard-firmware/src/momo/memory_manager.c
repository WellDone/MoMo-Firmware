#include "memory_manager.h"
#include "memory.h"
#include "sensor_event_log.h"

typedef struct {
  unsigned long start_address;
  unsigned long end_address;
} memory_block;

static inline memory_block create_block( unsigned int subsection_index, unsigned int subsection_length ) {
  memory_block block;
  block.start_address = MEMORY_SUBSECTION(subsection_index);
  block.end_address = MEMORY_SUBSECTION_SIZE*0xFL+block.start_address;
}
#define BLOCK_SIZE(block) (block.end_address-block.start_address)

void initialize_flash_memory_manager() {
  configure_SPI();

  memory_block log_memory_block = create_block( 1, 0xF );
  init_sensor_event_log( log_memory_block.start_address,  BLOCK_SIZE(log_memory_block) );
}