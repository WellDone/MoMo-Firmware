/*
 * reset_manager.h
 *
 *
 */

//Prototype for callback functions that manage resets
//function is passed an opaque integer handle that corresponds
//to the function's entry in the global reset manager table
//in case the handler wishes to deregister itself (i.e. it's single
//shot)
typedef void (*reset_handler)(unsigned int);

//The maximum number of handlers that can be registered for each
//reset type
#define MAX_RESETS_PER_TYPE 3
#define INVALID_RESET_HANDLE ((unsigned int)0xFFFF)

typedef enum
{
    kPowerOnReset = 0,
    kSleepReset,
    kDeepSleepReset,
    kSoftwareReset,
    kMCLRReset,
    kNumResets

} ResetType;

typedef enum
{
    kNoError = 0,
    kInvalidHandle = -1,
    kNoHandler = -2
};

