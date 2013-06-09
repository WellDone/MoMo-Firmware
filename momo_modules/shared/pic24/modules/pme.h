/*
 * Peripheral Module Power Control
 * Control functions for removing power from peripherals that are unused so that they don't waste power.
 *
 */

#ifndef __pme_h__
#define __pme_h__

#include "pic24.h"

#define kPMDRegOffset	5
#define kPMDBitMask		0b11111

#define PACK_POWER_LOC(reg, bit)	(((reg&0b111)<< kPMDRegOffset) | (bit&kPMDBitMask))
#define PowerRegisterOffset(sel)	((sel >> kPMDRegOffset) & 0b111)
#define PowerRegisterBit(sel)		(sel & kPMDBitMask)
enum
{
	kPMD1 = 0,
	kPMD2 = 1,
	kPMD3 = 2,
	kPMD4 = 3
};

//Bit Locations for each peripheral in one of the PMD registers
enum
{
	//PMD3
	kComparatorPMDBit = 10,
	kRTCCPMDBit = 9,
	kCRCPMDBit = 8,

	//PMD4
	kEEPROMPMDBit = 4,
	kRefOscPMDBit = 3,
	kCTMUPMDBit = 2,
	kHLVDPMDBit = 1,

	//PMD2
	kInputCapturePMDBit = 8,
	kInputComparePMDBit = 0,

	//PMD1
	kTimer3PMDBit = 13,
	kTimer2PMDBit = 12,
	kTimer1PMDBit = 11,
	kI2CPMDBit = 7,
	kUART2PMDBit = 6,
	kUART1PMDBit = 5,
	kSPIPMDBit = 3,
	kADCPMDBit = 0
};

typedef enum
{
	kTimer3Module = PACK_POWER_LOC(kPMD1, kTimer3PMDBit),
	kTimer2Module = PACK_POWER_LOC(kPMD1, kTimer2PMDBit),
	kTimer1Module = PACK_POWER_LOC(kPMD1, kTimer1PMDBit),
	kUART2Module = PACK_POWER_LOC(kPMD1, kUART2PMDBit),
	kUART1Module = PACK_POWER_LOC(kPMD1, kUART1PMDBit),
	kI2CModule = PACK_POWER_LOC(kPMD1, kI2CPMDBit),
	kSPIModule = PACK_POWER_LOC(kPMD1, kSPIPMDBit),
	kADCModule = PACK_POWER_LOC(kPMD1, kADCPMDBit),

	kInputCaptureModule = PACK_POWER_LOC(kPMD2, kInputCapturePMDBit),
	kInputCompareModule = PACK_POWER_LOC(kPMD2, kInputComparePMDBit),

	kComparatorModule = PACK_POWER_LOC(kPMD3, kComparatorPMDBit),
	kRTCCModule = PACK_POWER_LOC(kPMD3, kRTCCPMDBit),
	kCRCModule = PACK_POWER_LOC(kPMD3, kCRCPMDBit),

	kEEPROMModule = PACK_POWER_LOC(kPMD4, kEEPROMPMDBit),
	kRefOscModule = PACK_POWER_LOC(kPMD4, kRefOscPMDBit),
	kCTMUModule = PACK_POWER_LOC(kPMD4, kCTMUPMDBit),
	kHLVDModule = PACK_POWER_LOC(kPMD4, kHLVDPMDBit)
} PeripheralPowerSelector;

void peripheral_disable(PeripheralPowerSelector module);
void peripheral_enable(PeripheralPowerSelector module);

void disable_unneeded_peripherals();

#endif
