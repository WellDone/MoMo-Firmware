#ifndef __report_log_h
#define __report_log_h

#include "platform.h"

void init_report_log( uint8 start_subsection, uint8 subsection_count );
void save_report( char* report, uint8 length );
uint8 read_report_log( uint8 offset, void* output, uint8 max_report_count );

#endif