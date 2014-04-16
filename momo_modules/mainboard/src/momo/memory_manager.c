#include "memory_manager.h"

#include "momo_config.h"
#include "sensor_event_log.h"
#include "firmware_cache.h"

void flash_memory_init()
{
  init_memory();

  init_momo_config( kMomoConfigSubsector );
  fc_init( kFirmwareConfigSubector );

  init_sensor_event_log( MEMORY_SECTION_TO_SUB( kSensorDataSector ),  kSensorLogSubsectors );
}