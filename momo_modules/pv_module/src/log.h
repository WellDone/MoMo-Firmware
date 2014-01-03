#ifndef __log_h__
#define __log_h__

#include "sample.h"
#include "fat32.h"
#include "sdcard.h"

#define kDefaultLogInterval	kLogInterval64s

enum
{
	kLogInterval1s = 0,
	kLogInterval2s = 1,
	kLogInterval4s = 2,
	kLogInterval8s = 3,
	kLogInterval16s = 4,
	kLogInterval32s = 5,
	kLogInterval64s = 6,
	kLogInterval128s = 7,
	kLogInterval256s = 8
};

enum
{
	kWatchdog1s   = 0b010100,
	kWatchdog2s   = 0b010110,
	kWatchdog4s   = 0b011000,
	kWatchdog8s   = 0b011010,
	kWatchdog16s  = 0b011100,
	kWatchdog32s  = 0b011110,
	kWatchdog64s  = 0b100000,
	kWatchdog128s = 0b100001,
	kWatchdog256s = 0b100100
};

uint8 log_init();
uint8 log_logsamples();
void log_getoffset();
void log_getsector();
void log_setinterval();

#endif