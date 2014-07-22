#ifndef __report_log_h
#define __report_log_h

#include "platform.h"

void init_report_log( uint8 start_subsection, uint8 subsection_count );
void report_log_write( BYTE* report );
uint16 report_log_count();
void report_log_clear();
uint8 report_log_read( uint8 offset, void* output, uint8 max_report_count );

#endif