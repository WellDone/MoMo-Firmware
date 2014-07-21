#ifndef __report_comm_stream_h__
#define __report_comm_stream_h__

void report_stream_abandon();
void report_stream_send( char* buffer );

void notify_report_success();
void notify_report_failure();

#endif