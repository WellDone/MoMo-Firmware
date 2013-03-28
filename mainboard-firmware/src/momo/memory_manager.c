#include "memory_manager.h"
#include "memory.h"
#include "sensor_event_log.h"

void initialize_flash_memory_manager() {
  configure_SPI();
  init_sensor_event_log( MEMORY_SUBSECTION(1), MEMORY_SUBSECTION_SIZE*200L );
}