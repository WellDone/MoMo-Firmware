/*
 * constants.h
 * All PIC12 special addresses and constant values that are shared between
 * the mib12 executive and mib12 application modules
 */

#ifndef __constants_h__
#define __constants_h__

/*
 * The MIB12 executive takes up the lower portion of memory.  This constant defines the
 * first row of program memory that can be used by mib12 applications.  This value must be
 * kept in sync between the mib12 executive, and the build scripts for each application
 * module, i.e. it should not be changed very often.
 */
#define kFirstApplicationRow	79
#define kAppInitAddress			kFirstApplicationRow*16
#define kAppInterruptAddress	kFirstApplicationRow*16+1
#define kAppTaskAddress			kFirstApplicationRow*16+2

/*
 * MIB12 Application Modules must define a special structure in the 8 high words of program
 * memory so that the mib12 executive can find information about the loaded application
 */
#define kMIBEndpointAddress		0x7F8

#define kMIBMagicNumber			0xAA
#define kReflashMagicNumber		0x54

#endif