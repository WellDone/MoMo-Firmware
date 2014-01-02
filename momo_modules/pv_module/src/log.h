#ifndef __log_h__
#define __log_h__

#include "sample.h"
#include "fat32.h"
#include "sdcard.h"

void log_init();
void log_logsamples();
void log_getoffset();
void log_getsector();

#endif