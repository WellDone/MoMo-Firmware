#include "pme.h"

volatile unsigned int * const pmd_map[4] = 
{
	&PMD1,
	&PMD2,
	&PMD3,
	&PMD4
};

void peripheral_disable(PeripheralPowerSelector module)
{
	volatile unsigned int *reg = pmd_map[PowerRegisterOffset(module)];
	unsigned int bit = PowerRegisterBit(module);

	SET_BIT(*reg, bit);
}

void peripheral_enable(PeripheralPowerSelector module)
{
	volatile unsigned int *reg = pmd_map[PowerRegisterOffset(module)];
	unsigned int bit = PowerRegisterBit(module);

	CLEAR_BIT(*reg, bit);
}

void disable_unneeded_peripherals()
{
	peripheral_disable(kInputCaptureModule);
	peripheral_disable(kInputCompareModule);
	//peripheral_disable(kI2CModule);
	peripheral_disable(kCRCModule);
	peripheral_disable(kComparatorModule);
	peripheral_disable(kRefOscModule);
	peripheral_disable(kCTMUModule);
	peripheral_disable(kHLVDModule);
	//	peripheral_disable(kTimer2Module);
	//peripheral_disable(kTimer3Module);
	peripheral_disable(kUART2Module);
	peripheral_disable(kUART1Module);
}
