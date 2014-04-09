#ifndef __sensor_manager_h__
#define __sensor_manager_h__

#include "common.h"
#include "rtcc.h"

#define MAX_SENSOR_STREAM_COUNT 32

void push_sensor_value( uint8 stream_index, uint8 meta, uint64 *value );
void schedule_sensor_polling( AlarmRepeatTime interval, uint8 mib_address, uint8 callback_feature, uint8 callback_command );

#endif