/*
 * registration.c
 * Handle the work of registering this module with the MIB controller upon reset and getting an i2c address
 */ 

#include "registration.h"
#include "bus_master.h"
#include "constants.h"
#include "mib_definitions.h"

uint8 register_module()
{
	//If no application is loaded, fill in default values
	if (get_magic() != kMIBMagicNumber)
	{
		plist_set_int8(0,0, kMIBExecutiveOnlyType);
		plist_set_int8(0,1, kMIBPic12lf1822);
		plist_set_int8(1,0, kMIBVersion1);
		plist_set_int8(1,1, 1);				//Only support 1 executive feature
		plist_set_int8(2,0, 0); 			//No flags
		//plist_set_int8(2,1, 'N'); 
		//plist_set_int8(3,0, 'o'); 
		//plist_set_int8(3,1, ' '); 
		//plist_set_int8(4,0, 'N'); 
		//plist_set_int8(4,1, 'a'); 
		//plist_set_int8(5,0, 'm'); 
		//plist_set_int8(5,1, 'e'); 
		//plist_set_int8(6,0, '!'); 
	}
	else
	{
		//Otherwise load the information from the application module
		plist_set_int8(0,0, get_mib_block(kMIBTypeOffset));
		plist_set_int8(0,1, kMIBPic12lf1822);
		plist_set_int8(1,0, kMIBVersion1);
		plist_set_int8(1,1, get_mib_block(kMIBNumFeatureOffset)+1);
		plist_set_int8(2,0, get_mib_block(kMIBFlagsOffset)); 
		//plist_set_int8(2,1, get_mib_block(kMIBNameOffset+0)); 
		//plist_set_int8(3,0, get_mib_block(kMIBNameOffset+1)); 
		//plist_set_int8(3,1, get_mib_block(kMIBNameOffset+2)); 
		//plist_set_int8(4,0, get_mib_block(kMIBNameOffset+3)); 
		//plist_set_int8(4,1, get_mib_block(kMIBNameOffset+4)); 
		//plist_set_int8(5,0, get_mib_block(kMIBNameOffset+5)); 
		//plist_set_int8(5,1, get_mib_block(kMIBNameOffset+6)); 
		//plist_set_int8(6,0, get_mib_block(kMIBNameOffset+7)); 
	}

	bus_master_prepare(42, 0, plist_with_buffer(0, sizeof(momo_module_descriptor)));

	if (bus_master_rpc_sync(kMIBControllerAddress) == 0)
		return plist_get_int8(0);
	
	//If we failed, return an invalid mib address to indicate that
	return 0;
}