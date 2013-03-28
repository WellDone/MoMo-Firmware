#include "memory_manager.h"
#include "memory.h"

void initializeFlashMemoryManager() {
  configure_SPI();
  init_sensor_event_log( MEMORY_SUBSECTION(1), MEMORY_SUBSECTION_SIZE*200 );
}