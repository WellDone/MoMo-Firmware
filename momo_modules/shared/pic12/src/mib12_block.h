#ifndef __mib12_block_h__
#define __mib12_block_h__

/*
 * MIB12 Application Modules must define a special structure in the 16 high words of program
 * memory so that the mib12 executive can find information about the loaded application
 */

#define kMIBEndpointStructureSize 		16
#define kMIBEndpointAddress				(0x800 - kMIBEndpointStructureSize)

#define kMIBHWTypeOffset				0
#define kMIBMajorAPIOffset				1
#define kMIBMinorAPIOffset				2
#define kMIBModuleNameOffset			3
#define kMIBModuleMajorVersionOffset	9
#define kMIBModuleMinorVersionOffset	10
#define kMIBModulePatchVersionOffset	11
#define kMIBApplicationChecksumOffset	12
#define kMIBMagicNumberOffset			13
#define kMIBCommandMapGotoOffset 		14
#define kMIBInterfaceMapGotoOffset		15

#define mib_block_address(offset)		(kMIBEndpointAddress + offset)

#define kMIBMagicNumber					0xAA
#define kReflashMagicNumber				0x54

#define kMIBCommandMapSentinelValue		0xFF


#endif