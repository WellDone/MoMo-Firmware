#ifndef __sensor_manager_h__
#define __sensor_manager_h__

#include "common.h"

void push_sensor_value( uint8 stream_index, uint8 meta, uint64 *value );

#endif