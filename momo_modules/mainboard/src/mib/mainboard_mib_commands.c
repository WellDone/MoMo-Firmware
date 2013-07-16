#include "mainboard_mib_commands.h"
//#include "mib_command.h"
//#include "mib_command_parameters.h"

#include "test.h"
#include "prog.h"
/*
#define kTestCommandCount 2
static mib_command_handler test_commands[kTestCommandCount] =
	{
		{0, echo_buffer, plist_define1(kMIBBufferType) },
		{1, test_command, plist_define2(kMIBInt16Type, kMIBInt16Type) }
	};

#define kProgrammingCommandCount 3
static mib_command_handler programming_commands[kProgrammingCommandCount] = 
	{
		{0, erase_primaryfirmware, plist_define0() },
		{1, load_into_nvram, plist_define3(kMIBInt16Type, kMIBInt16Type, kMIBBufferType) },
		{2, read_from_nvram, plist_define3(kMIBInt16Type, kMIBInt16Type, kMIBInt16Type) }
	};

static feature_map mainboard_mib_features[kMainboardMIBFeatureCount] =
	{
		{kMIBTestFeature, test_commands, kTestCommandCount},
		{kMIBProgrammingFeature, programming_commands, kProgrammingCommandCount}
	};

void register_mib_commands()
{
	register_mib_features(mainboard_mib_features, kMainboardMIBFeatureCount);
}
*/