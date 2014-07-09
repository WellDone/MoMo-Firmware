#include "report_log.h"

#include "flash_queue.h"
#include "report_manager.h"

flash_queue report_log_queue;
void init_report_log( uint8 start_subsection, uint8 subsection_count )
{
	flash_queue_create( &report_log_queue, start_subsection,
                      RAW_REPORT_MAX_LENGTH,
                      subsection_count );
}
void save_raw_report( BYTE* report )
{
	flash_queue_queue( &report_log_queue, report );
}

uint8 read_report_log( uint8 offset, void* output, uint8 max_report_count )
{
	flash_queue_walker walker = new_flash_queue_walker( &report_log_queue, offset );
	return flash_queue_walk( walker, output, max_report_count );
}