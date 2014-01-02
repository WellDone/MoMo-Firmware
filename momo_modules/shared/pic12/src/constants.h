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

/*
 * MIB12 Application Modules must define a special structure in the 8 high words of program
 * memory so that the mib12 executive can find information about the loaded application
 */
#define kMIBEndpointStructureSize (6+8+3)
#define kMIBEndpointAddress		0x800 - kMIBEndpointStructureSize
#define kMIBNameOffset			0
#define kMIBTypeOffset			8
#define kMIBVersionOffset		9
#define kMIBFlagsOffset			10
#define kMIBNumFeatureOffset	11
#define kMIBMagicOffset			16

#define kMIBMagicNumber			0xAA
#define kReflashMagicNumber		0x54

#endif