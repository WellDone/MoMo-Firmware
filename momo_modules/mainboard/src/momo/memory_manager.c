#include "memory_manager.h"

#include "momo_config.h"
#include "sensor_event_log.h"
#include "firmware_cache.h"
#include "system_log.h"
#include "report_log.h"

void flash_memory_init()
{
  mem_init();

  init_momo_config( kMomoConfigSubsector );
  fc_init( kFirmwareConfigSubector );

  init_sensor_event_log(MEMORY_SECTION_TO_SUB(kSensorDataSector),  kSensorLogSubsectors);
  init_system_log(kSystemLogSubsector, 8);
  init_report_log(kReportLogSubsector, 8);
}