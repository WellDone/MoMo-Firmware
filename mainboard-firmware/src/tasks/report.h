#ifndef __report_h__
#define	__report_h__

typedef struct {
    unsigned int sensorType;
    unsigned int* events;
    unsigned int eventCount;
} sensor_event_log;

void PostReport( sensor_event_log* log );

#endif	/* __report_h__ */

