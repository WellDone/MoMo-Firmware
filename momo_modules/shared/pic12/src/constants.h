/*
 * constants.h
 * All PIC12 special addresses and constant values that are shared between
 * the mib12 executive and mib12 application modules
 */

#ifndef __constants_h__
#define __constants_h__

/*
 * The MIB12 executive takes up the lower portion of memory.  Application modules
 * live in the higher portion of memory.
 */
#define kAppInitAddress			kFirstApplicationRow*kFlashRowSize
#define kAppInterruptAddress	kFirstApplicationRow*kFlashRowSize+1
#define kAppTaskAddress			kFirstApplicationRow*kFlashRowSize+2

#endif