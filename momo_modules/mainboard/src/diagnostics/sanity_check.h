#ifndef __sanity_check_h__
#define __sanity_check_h__

#define SANITY_CHECK_MODULE_COUNT 2 //TODO: make configurable
#define SANITY_CHECK_HUNG_BUS_TIMEOUT_M 30
void sanity_check_schedule();
void sanity_check_run( void* arg );

#endif