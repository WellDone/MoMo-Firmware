/*
 * File:   report_manager.h
 * Author: Austin
 *
 * Created on March 14, 2013, 6:26 PM
 */

#ifndef __report_manager_h__
#define	__report_manager_h__

typedef struct {
    unsigned int sensorType;
    unsigned int* events;
    unsigned int eventCount;
} sensor_event_log;

void post_report( sensor_event_log* log );

#endif	/* __report_manager_h__ */

